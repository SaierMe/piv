#ifndef LIBGO_SCHEDULER_SCHEDULER_H
#define LIBGO_SCHEDULER_SCHEDULER_H
#include "../common/config.h"
#include "../common/deque.h"
#include "../common/spinlock.h"
#include "../routine_sync/timer.h"
#include "../task/task.h"
#include "../debug/listener.h"
#include "processer.h"
#include <mutex>

#include "../common/error.h"
#include "../common/clock.h"
#include <stdio.h>
#include <system_error>
#include <unistd.h>
#include <time.h>
#include "ref.h"
#include <thread>

namespace co {

struct TaskOpt
{
    bool affinity_ = false;
    int lineno_ = 0;
    std::size_t stack_size_ = 0;
    const char* file_ = nullptr;
};

inline static atomic_t<unsigned long long> & GetTaskIdFactory()
{
    static atomic_t<unsigned long long> factory;
    return factory;
}

static std::mutex& ExitListMtx()
{
    static std::mutex mtx;
    return mtx;
}
static std::vector<std::function<void()>>* ExitList()
{
    static std::vector<std::function<void()>> *vec = new std::vector<std::function<void()>>;
    return vec;
}

static void onExit(void) {
    auto vec = ExitList();
    for (auto fn : *vec) {
        fn();
    }
    vec->clear();
//    return 0;
}

static int InitOnExit() {
    atexit(&onExit);
    return 0;
}

// 协程调度器
// 负责管理1到N个调度线程, 调度从属协程.
// 可以调用Create接口创建更多额外的调度器
class Scheduler
{
    friend class Processer;

public:
    LIBGO_INLINE static Scheduler& getInstance();

    static Scheduler* Create()
    {
        static int ignore = InitOnExit();
        (void)ignore;

        Scheduler* sched = new Scheduler;
        std::unique_lock<std::mutex> lock(ExitListMtx());
        auto vec = ExitList();
        vec->push_back([=] { delete sched; });
        return sched;
    }

    // 创建一个协程
    void CreateTask(TaskF const& fn, TaskOpt const& opt)
    {
        Task* tk = new Task(fn, opt.stack_size_ ? opt.stack_size_ : CoroutineOptions::getInstance().stack_size);
    //    printf("new tk = %p  impl = %p\n", tk, tk->impl_);
        tk->SetDeleter(Deleter(&Scheduler::DeleteTask, this));
        tk->id_ = ++GetTaskIdFactory();
        TaskRefAffinity(tk) = opt.affinity_;
        TaskRefLocation(tk).Init(opt.file_, opt.lineno_);
        ++taskCount_;

        LIBGO_DebugPrint(dbg_task, "task(%s) created in scheduler(%p).", TaskDebugInfo(tk), (void*)this);
#if LIBGO_ENABLE_DEBUGGER
        if (Listener::GetTaskListener()) {
            Listener::GetTaskListener()->onCreated(tk->id_);
        }
#endif

        AddTask(tk);
    }

    // 当前是否处于协程中
    bool IsCoroutine()
    {
        return !!Processer::GetCurrentTask();
    }

    // 是否没有协程可执行
    bool IsEmpty()
    {
        return taskCount_ == 0;
    }

    // 启动调度器
    // @minThreadNumber : 最小调度线程数, 为0时, 设置为cpu核心数.
    // @maxThreadNumber : 最大调度线程数, 为0时, 设置为minThreadNumber.
    //          如果maxThreadNumber大于minThreadNumber, 则当协程产生长时间阻塞时,
    //          可以自动扩展调度线程数.
    void Start(int minThreadNumber = 1, int maxThreadNumber = 0)
    {
        if (!started_.try_lock())
            throw std::logic_error("libgo repeated call Scheduler::Start");

        if (minThreadNumber < 1)
        minThreadNumber = std::thread::hardware_concurrency();

        if (maxThreadNumber == 0 || maxThreadNumber < minThreadNumber)
            maxThreadNumber = minThreadNumber;

        minThreadNumber_ = minThreadNumber;
        maxThreadNumber_ = maxThreadNumber;

        auto mainProc = processers_[0];

        for (int i = 0; i < minThreadNumber_ - 1; i++) {
            NewProcessThread();
        }

        // 调度线程
        if (maxThreadNumber_ > 1) {
            LIBGO_DebugPrint(dbg_scheduler, "---> Create DispatcherThread");
            std::thread t([this]{
                    LIBGO_DebugPrint(dbg_thread, "Start dispatcher(sched=%p) thread id: %lu", (void*)this, NativeThreadID());
                    this->DispatcherThread();
                    });
            dispatchThread_.swap(t);
        } else {
            LIBGO_DebugPrint(dbg_scheduler, "---> No DispatcherThread");
        }

        std::thread(FastSteadyClock::ThreadRun).detach();

        LIBGO_DebugPrint(dbg_scheduler, "Scheduler::Start minThreadNumber_=%d, maxThreadNumber_=%d", minThreadNumber_, maxThreadNumber_);
        mainProc->Process();
    }

    void goStart(int minThreadNumber = 1, int maxThreadNumber = 0)
    {
        std::thread([=]{ this->Start(minThreadNumber, maxThreadNumber); }).detach();
    }

    static const int s_ulimitedMaxThreadNumber = 40960;

    // 停止调度 
    // 注意: 停止后无法恢复, 仅用于安全退出main函数, 不保证终止所有线程.
    //       如果某个调度线程被协程阻塞, 必须等待阻塞结束才能退出.
    void Stop()
    {
        std::unique_lock<std::mutex> lock(stopMtx_);

        if (stop_) return;

        stop_ = true;
        size_t n = processers_.size();
        for (size_t i = 0; i < n; ++i) {
            auto p = processers_[i];
            if (p)
                p->NotifyCondition();
        }

        if (timer_) timer_->stop();

        if (dispatchThread_.joinable())
            dispatchThread_.join();
    }

    // 使用独立的定时器线程
    void UseAloneTimerThread()
    {
        if (!timer_) {
            timer_ = new TimerType;
        }
    }

    // 当前调度器中的协程数量
    uint32_t TaskCount()
    {
        return taskCount_;
    }

    // 当前协程ID, ID从1开始（不在协程中则返回0）
    uint64_t GetCurrentTaskID()
    {
        Task* tk = Processer::GetCurrentTask();
        return tk ? tk->id_ : 0;
    }

    // 当前协程切换的次数
    uint64_t GetCurrentTaskYieldCount()
    {
        Task* tk = Processer::GetCurrentTask();
        return tk ? tk->yieldCount_ : 0;
    }

    // 设置当前协程调试信息, 打印调试信息时将回显
    void SetCurrentTaskDebugInfo(std::string const& info)
    {
        Task* tk = Processer::GetCurrentTask();
        if (!tk) return ;
        TaskRefDebugInfo(tk) = info;
    }

    typedef ::libgo::RoutineSyncTimer TimerType;

public:
    inline TimerType & GetTimer() { return timer_ ? *timer_ : StaticGetTimer(); }

    inline bool IsStop() { return stop_; }

    static bool& IsExiting()
    {
        static bool exiting = false;
        return exiting;
    }

private:
    using idx_t     = std::size_t;
    using ActiveMap = std::multimap<std::size_t, idx_t>;
    using BlockMap  = std::map<idx_t, std::size_t>;
    Scheduler()
        {
        LibgoInitialize();
        processers_.push_back(new Processer(this, 0));
    }
    ~Scheduler()
    {
        IsExiting() = true;
        Stop();
    }

    Scheduler(Scheduler const&) = delete;
    Scheduler(Scheduler &&) = delete;
    Scheduler& operator=(Scheduler const&) = delete;
    Scheduler& operator=(Scheduler &&) = delete;

    static void DeleteTask(RefObject* tk, void* arg)
    {
        Scheduler* self = (Scheduler*)arg;
        delete tk;
        --self->taskCount_;
    }

    // 将一个协程加入可执行队列中
    void AddTask(Task* tk)
    {
        LIBGO_DebugPrint(dbg_scheduler, "Add task(%s) to runnable list.", tk->DebugInfo());
        auto proc = tk->proc_;
        if (proc && proc->active_) {
            proc->AddTask(tk);
            return ;
        }

        proc = Processer::GetCurrentProcesser();
        if (proc && proc->active_ && proc->GetScheduler() == this) {
            proc->AddTask(tk);
            return ;
        }

        std::size_t pcount = processers_.size();
        std::size_t idx = lastActive_;
        for (std::size_t i = 0; i < pcount; ++i, ++idx) {
            idx = idx % pcount;
            proc = processers_[idx];
            if (proc && proc->active_)
                break;
        }
        proc->AddTask(tk);
    }

    // dispatcher线程函数
    // 1.根据待执行协程计算负载, 将高负载的P中的协程steal一些给空载的P
    // 2.侦测到阻塞的P(单个协程运行时间超过阀值), 将P中的其他协程steal给其他P
    void DispatcherThread()
    {
        LIBGO_DebugPrint(dbg_scheduler, "---> Start DispatcherThread");
        while (!stop_) {
            // TODO: 用condition_variable降低cpu使用率
            std::this_thread::sleep_for(std::chrono::microseconds(CoroutineOptions::getInstance().dispatcher_thread_cycle_us));
    
            // 1.收集负载值, 收集阻塞状态, 打阻塞标记, 唤醒处于等待状态但是有任务的P
            idx_t pcount = processers_.size();
            std::size_t totalLoadaverage = 0;
            ActiveMap actives;
            BlockMap blockings;

            int isActiveCount = 0;
            for (std::size_t i = 0; i < pcount; i++) {
                auto p = processers_[i];
                //等待中的p不能算阻塞,无法加入新协程导致p饿死
                if (!p->IsWaiting() && p->IsBlocking()) {
                    blockings[i] = p->RunnableSize();
                    if (p->active_) {
                        p->active_ = false;
                        LIBGO_DebugPrint(dbg_scheduler, "Block processer(%d)", (int)i);
                    }
                }
                
                if (p->active_)
                    isActiveCount++;
            }
        

            // 还可激活几个P
            int activeQuota = isActiveCount < minThreadNumber_ ? (minThreadNumber_ - isActiveCount) : 0;
            
            std::size_t activeTasks = 0;
            for (std::size_t i = 0; i < pcount; i++) {
                auto p = processers_[i];
                std::size_t loadaverage = p->RunnableSize();
                totalLoadaverage += loadaverage;

                if (!p->active_) {
                    //处于等待中的p也应该唤醒
                    if (activeQuota > 0 && (!p->IsBlocking() || p->IsWaiting())) {
                        p->active_ = true;
                        activeQuota--;
                        LIBGO_DebugPrint(dbg_scheduler, "Active processer(%d)", (int)i);
                        lastActive_ = static_cast<uint32_t>(i);
                    }
                }

                if (p->active_) {
                    actives.insert(ActiveMap::value_type{loadaverage, i});
                    activeTasks += p->RunnableSize();
                    p->Mark();
                }

                if (loadaverage > 0 && p->IsWaiting()) {
                    p->NotifyCondition();
                }
            }

            if (actives.empty() && (int)pcount < maxThreadNumber_) {
                // 全部阻塞, 并且还有协程待执行, 起新线程
                NewProcessThread();
                actives.insert(ActiveMap::value_type{0, pcount});
                ++pcount;
            }

            
            // 全部阻塞并且不能起新线程, 无需调度, 等待即可
            if (actives.empty())
                continue;
            
            DispatchBlocks(blockings,actives);

            LoadBalance(actives,activeTasks);
        
        }
    }

    void NewProcessThread()
    {
        auto p = new Processer(this, static_cast<int>(processers_.size()));
        LIBGO_DebugPrint(dbg_scheduler, "---> Create Processer(%d)", p->id_);
        std::thread t([this, p]{
                LIBGO_DebugPrint(dbg_thread, "Start process(sched=%p) thread id: %lu", (void*)this, NativeThreadID());
                p->Process();
                });
        t.detach();
        processers_.push_back(p);
    }

    void DispatchBlocks(BlockMap &blockings,ActiveMap &actives)
    {
        if(blockings.size() == 0)
            return;
        //将阻塞p的协程都steal出来
        SList<Task> tasks;
        for (auto &kv : blockings) {
                auto p = processers_[kv.first];
                tasks.append(p->Steal(0));
            }
            
            if(tasks.empty())
            return;
        
            ActiveMap newActives;
            //总协程数
            std::size_t totalTasks = tasks.size();
            //需要平分协程p的数量
            std::size_t LowerNum = 0;
            //平分的协程数
            std::size_t avg = 0;    
            
            auto LowerP = actives.begin();

            for( ; LowerP != actives.end(); ++LowerP)
            {
                totalTasks += LowerP->first;
                ++LowerNum;
                avg = totalTasks / LowerNum;
                //p的负载大于avg则将它移除
                if(LowerP->first > avg)
                {
                    totalTasks -= LowerP->first;
                    --LowerNum;
                    avg = totalTasks / LowerNum;
                    break;
                }        
            }

            if(LowerP != actives.end())
            ++LowerP;
            
            for(auto it = actives.begin(); it != LowerP; ++it)
            {
                SList<Task> in = tasks.cut(avg - it->first);
                
                if(in.empty())
                break;

                auto p = processers_[it->second];

                p->AddTask(std::move(in));
            }
            //还剩下task就全都给最小的p
            if(!tasks.empty())
            {
                auto p = processers_[actives.begin()->second];
                p->AddTask(std::move(tasks));
            }
    }

    void LoadBalance(ActiveMap &actives,std::size_t activeTasks)
    {
        std::size_t avg = activeTasks / actives.size();
        
        if(actives.begin()->first > avg * CoroutineOptions::getInstance().load_balance_rate)
            return;
        
        SList<Task> tasks;
        for(auto it = actives.rbegin(); it != actives.rend(); ++it)
        {
            
            if(it->first <= avg)
                break;

            auto p = processers_[it->second];

            SList<Task> in = p->Steal(it->first - avg); 

            tasks.append(std::move(in));
        }

        if(tasks.empty())
            return;
        
        for(auto &kv : actives)
        {
            if(kv.first >= avg)
                break;
            auto p = processers_[kv.second];

            auto in = tasks.cut(avg - kv.first);
                
            p->AddTask(std::move(in));
        }
        //如果还剩下task,全都给最小的p
        if(!tasks.empty())
        {
            auto p = processers_[actives.begin()->second];
            p->AddTask(std::move(tasks));
        }
    }
    
    TimerType & StaticGetTimer()
    {
        static Scheduler::TimerType *ptimer = new Scheduler::TimerType;

        std::unique_lock<std::mutex> lock(ExitListMtx());
        auto vec = ExitList();
        vec->push_back([=] {
                ptimer->stop();
            });
        return *ptimer;
    }

    // deque of Processer, write by start or dispatch thread
    Deque<Processer*> processers_;

    LFLock started_;

    atomic_t<uint32_t> taskCount_{0};

    volatile uint32_t lastActive_ = 0;

    TimerType *timer_ = nullptr;
    
    int minThreadNumber_ = 1;
    int maxThreadNumber_ = 1;

    std::thread dispatchThread_;

    std::mutex stopMtx_;

    bool stop_ = false;
};

//Scheduler::TimerType & Scheduler::StaticGetTimer() {
//    static TimerType & timer = staticGetTimer();
//    return timer;
//}

} //namespace co

namespace co
{
    LIBGO_INLINE Scheduler& Scheduler::getInstance()
    {
        static Scheduler obj;
        return obj;
    }

} //namespace co
#endif // LIBGO_SCHEDULER_SCHEDULER_H
