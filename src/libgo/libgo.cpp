#include "common/config.h"
#include "context/fcontext.h"
#include "scheduler/processer.h"
#include <string.h>
#include <stdarg.h>
#include <poll.h>
#if defined(LIBGO_SYS_Unix)
#include <sys/time.h>
#include <pthread.h>
#endif

#include "scheduler/ref.h"
#include "cls/co_local_storage.h"
#include "routine_sync_libgo/libgo_switcher.h"
#include "scheduler/scheduler.h"


namespace co {

#if LIBGO_ENABLE_HOOK
  #if defined(LIBGO_SYS_Linux) || defined(LIBGO_SYS_Windows)
    extern void initHook();
  #endif
#endif

static int staticInitialize()
{
    // scheduler
    LIBGO_TaskRefInit(Affinity);
    LIBGO_TaskRefInit(Location);
    LIBGO_TaskRefInit(DebugInfo);
//    LIBGO_TaskRefInit(SuspendId);

    // cls
    LIBGO_TaskRefInit(ClsMap);

#if LIBGO_ENABLE_HOOK
  #if defined(LIBGO_SYS_Linux) || defined(LIBGO_SYS_Windows)
    initHook();
  #endif
#endif
    return 0;
}

void LibgoInitialize()
{
    static int ignore = staticInitialize();
    (void)ignore;
}

std::mutex gDbgLock;

CoroutineOptions::CoroutineOptions()
    : protect_stack_page(StackTraits::GetProtectStackPageSize()),
    stack_malloc_fn(StackTraits::MallocFunc()),
    stack_free_fn(StackTraits::FreeFunc())
{
}

int GetCurrentThreadID()
{
    auto proc = Processer::GetCurrentProcesser();
    return proc ? proc->Id() : -1;
}

int GetCurrentCoroID()
{
    Task* tk = Processer::GetCurrentTask();
    return tk ? static_cast<int>(tk->id_) : 0;
}

const char* PollEvent2Str(short int event)
{
    event &= POLLIN|POLLOUT|POLLERR;
    switch (event) {
        LIBGO_E2S_DEFINE(POLLIN);
        LIBGO_E2S_DEFINE(POLLOUT);
        LIBGO_E2S_DEFINE(POLLERR);
        LIBGO_E2S_DEFINE(POLLNVAL);
        LIBGO_E2S_DEFINE(POLLIN|POLLOUT);
        LIBGO_E2S_DEFINE(POLLIN|POLLERR);
        LIBGO_E2S_DEFINE(POLLOUT|POLLERR);
        LIBGO_E2S_DEFINE(POLLIN|POLLOUT|POLLERR);
        default:
        return "Zero";
    }
}

int Processer::s_check_ = 0;

Processer::Processer(Scheduler * scheduler, int id)
    : scheduler_(scheduler), id_(id)
{
    waitQueue_.setLock(&runnableQueue_.LockRef());
}

Processer* & Processer::GetCurrentProcesser()
{
    static thread_local Processer *proc = nullptr;
    return proc;
}

Scheduler* Processer::GetCurrentScheduler()
{
    auto proc = GetCurrentProcesser();
    return proc ? proc->scheduler_ : nullptr;
}

void Processer::AddTask(Task *tk)
{
    LIBGO_DebugPrint(dbg_task | dbg_scheduler, "task(%s) add into proc(%u)(%p)", tk->DebugInfo(), id_, (void*)this);
    std::unique_lock<TaskQueue::lock_t> lock(newQueue_.LockRef());
    newQueue_.pushWithoutLock(tk);
    newQueue_.AssertLink();
    if (waiting_)
        cv_.notify_all();
    else
        notified_ = true;
}

void Processer::AddTask(SList<Task> && slist)
{
    LIBGO_DebugPrint(dbg_scheduler, "task(num=%d) add into proc(%u)", (int)slist.size(), id_);
    std::unique_lock<TaskQueue::lock_t> lock(newQueue_.LockRef());
    newQueue_.pushWithoutLock(std::move(slist));
    newQueue_.AssertLink();
    if (waiting_)
        cv_.notify_all();
    else
        notified_ = true;
}

void Processer::NotifyCondition()
{
    std::unique_lock<TaskQueue::lock_t> lock(newQueue_.LockRef());
    if (waiting_) {
        LIBGO_DebugPrint(dbg_scheduler, "NotifyCondition for condition. [Proc(%d)] --------------------------", id_);
        cv_.notify_all();
    }
    else {
        LIBGO_DebugPrint(dbg_scheduler, "NotifyCondition for flag. [Proc(%d)] --------------------------", id_);
        notified_ = true;
    }
}

void Processer::Process()
{
    GetCurrentProcesser() = this;

#if defined(LIBGO_SYS_Windows)
    FiberScopedGuard sg;
#endif

    while (!scheduler_->IsStop())
    {
        runnableQueue_.front(runningTask_);

        if (!runningTask_) {
            if (AddNewTasks())
                runnableQueue_.front(runningTask_);

            if (!runningTask_) {
                WaitCondition();
                AddNewTasks();
                continue;
            }
        }

#if LIBGO_ENABLE_DEBUGGER
        LIBGO_DebugPrint(dbg_scheduler, "Run [Proc(%d) QueueSize:%zu] --------------------------", id_, RunnableSize());
#endif

        addNewQuota_ = 1;
        while (runningTask_ && !scheduler_->IsStop()) {
            runningTask_->state_ = TaskState::runnable;
            runningTask_->proc_ = this;

#if LIBGO_ENABLE_DEBUGGER
            LIBGO_DebugPrint(dbg_switch, "enter task(%s)", runningTask_->DebugInfo());
            if (Listener::GetTaskListener())
                Listener::GetTaskListener()->onSwapIn(runningTask_->id_);
#endif

            ++switchCount_;

            runningTask_->SwapIn();

#if LIBGO_ENABLE_DEBUGGER
            LIBGO_DebugPrint(dbg_switch, "leave task(%s) state=%d", runningTask_->DebugInfo(), (int)runningTask_->state_);
#endif

            switch (runningTask_->state_) {
                case TaskState::runnable:
                    {
                        std::unique_lock<TaskQueue::lock_t> lock(runnableQueue_.LockRef());
                        auto next = (Task*)runningTask_->next;
                        if (next) {
                            runningTask_ = next;
                            runningTask_->check_ = runnableQueue_.check_;
                            break;
                        }

                        if (addNewQuota_ < 1 || newQueue_.emptyUnsafe()) {
                            runningTask_ = nullptr;
                        } else {
                            lock.unlock();
                            if (AddNewTasks()) {
                                runnableQueue_.next(runningTask_, runningTask_);
                                -- addNewQuota_;
                            } else {
                                std::unique_lock<TaskQueue::lock_t> lock2(runnableQueue_.LockRef());
                                runningTask_ = nullptr;
                            }
                        }

                    }
                    break;

                case TaskState::block:
                    {
                        std::unique_lock<TaskQueue::lock_t> lock(runnableQueue_.LockRef());
                        runningTask_ = nextTask_;
                        nextTask_ = nullptr;
                    }
                    break;

                case TaskState::done:
                default:
                    {
                        runnableQueue_.next(runningTask_, nextTask_);
                        if (!nextTask_ && addNewQuota_ > 0) {
                            if (AddNewTasks()) {
                                runnableQueue_.next(runningTask_, nextTask_);
                                -- addNewQuota_;
                            }
                        }

                        LIBGO_DebugPrint(dbg_task, "task(%s) done.", runningTask_->DebugInfo());
                        runnableQueue_.erase(runningTask_);
                        if (gcQueue_.size() > 16)
                            GC();
                        gcQueue_.push(runningTask_);
                        if (runningTask_->eptr_) {
                            std::exception_ptr ep = runningTask_->eptr_;
                            std::rethrow_exception(ep);
                        }

                        std::unique_lock<TaskQueue::lock_t> lock(runnableQueue_.LockRef());
                        runningTask_ = nextTask_;
                        nextTask_ = nullptr;
                    }
                    break;
            }
        }
    }
}

Task* Processer::GetCurrentTask()
{
    auto proc = GetCurrentProcesser();
    return proc ? proc->runningTask_ : nullptr;
}

bool Processer::IsCoroutine()
{
    return !!GetCurrentTask();
}

std::size_t Processer::RunnableSize()
{
    return runnableQueue_.size() + newQueue_.size();
}

void Processer::WaitCondition()
{
    GC();
    std::unique_lock<TaskQueue::lock_t> lock(newQueue_.LockRef());
    if (notified_) {
        LIBGO_DebugPrint(dbg_scheduler, "WaitCondition by Notified. [Proc(%d)] --------------------------", id_);
        notified_ = false;
        return ;
    }

    waiting_ = true;
    LIBGO_DebugPrint(dbg_scheduler, "WaitCondition. [Proc(%d)] --------------------------", id_);
    cv_.wait(lock);
    waiting_ = false;
}

void Processer::GC()
{
    auto list = gcQueue_.pop_all();
    for (Task & tk : list) {
        tk.DecrementRef();
    }
    list.clear();
}

bool Processer::AddNewTasks()
{
    runnableQueue_.push(newQueue_.pop_all());
    newQueue_.AssertLink();
    return true;
}

bool Processer::IsBlocking()
{
    if (!markSwitch_ || markSwitch_ != switchCount_) return false;
    return NowMicrosecond() > markTick_ + CoroutineOptions::getInstance().cycle_timeout_us;
}

void Processer::Mark()
{
    if (runningTask_ && markSwitch_ != switchCount_) {
        markSwitch_ = switchCount_;
        markTick_ = NowMicrosecond();
    }
}

int64_t Processer::NowMicrosecond()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(FastSteadyClock::now().time_since_epoch()).count();
}

SList<Task> Processer::Steal(std::size_t n)
{
    if (n > 0) {
        // steal some
        newQueue_.AssertLink();
        auto slist = newQueue_.pop_back(static_cast<uint32_t>(n));
        newQueue_.AssertLink();
        if (slist.size() >= n)
            return slist;

        std::unique_lock<TaskQueue::lock_t> lock(runnableQueue_.LockRef());
        bool pushRunningTask = false, pushNextTask = false;
        if (runningTask_)
            pushRunningTask = runnableQueue_.eraseWithoutLock(runningTask_, true) || slist.erase(runningTask_, newQueue_.check_);
        if (nextTask_)
            pushNextTask = runnableQueue_.eraseWithoutLock(nextTask_, true) || slist.erase(nextTask_, newQueue_.check_);
        auto slist2 = runnableQueue_.pop_backWithoutLock(static_cast<uint32_t>(n - slist.size()));
        if (pushRunningTask)
            runnableQueue_.pushWithoutLock(runningTask_);
        if (pushNextTask)
            runnableQueue_.pushWithoutLock(nextTask_);
        lock.unlock();

        slist2.append(std::move(slist));
        if (!slist2.empty())
            LIBGO_DebugPrint(dbg_scheduler, "Proc(%d).Stealed = %d", id_, (int)slist2.size());
        return slist2;
    } else {
        // steal all
        newQueue_.AssertLink();
        auto slist = newQueue_.pop_all();
        newQueue_.AssertLink();

        std::unique_lock<TaskQueue::lock_t> lock(runnableQueue_.LockRef());
        bool pushRunningTask = false, pushNextTask = false;
        if (runningTask_)
            pushRunningTask = runnableQueue_.eraseWithoutLock(runningTask_, true) || slist.erase(runningTask_, newQueue_.check_);
        if (nextTask_)
            pushNextTask = runnableQueue_.eraseWithoutLock(nextTask_, true) || slist.erase(nextTask_, newQueue_.check_);
        auto slist2 = runnableQueue_.pop_allWithoutLock();
        if (pushRunningTask)
            runnableQueue_.pushWithoutLock(runningTask_);
        if (pushNextTask)
            runnableQueue_.pushWithoutLock(nextTask_);
        lock.unlock();

        slist2.append(std::move(slist));
        if (!slist2.empty())
            LIBGO_DebugPrint(dbg_scheduler, "Proc(%d).Stealed all = %d", id_, (int)slist2.size());
        return slist2;
    }
}

Processer::SuspendEntry Processer::Suspend()
{
    Task* tk = GetCurrentTask();
    assert(tk);
    assert(tk->proc_);
    return tk->proc_->SuspendBySelf(tk);
}

Processer::SuspendEntry Processer::Suspend(FastSteadyClock::duration dur)
{
    FastSteadyClock::time_point tp = FastSteadyClock::now();
    tp += dur;
    return Suspend(tp);
}
Processer::SuspendEntry Processer::Suspend(FastSteadyClock::time_point timepoint)
{
    SuspendEntry entry = Suspend();
    Task* tk = GetCurrentTask();
    if (tk->isInTimer_) {
        tk->schedTimer_->join_unschedule(tk->suspendTimerId_);
        tk->schedTimer_ = nullptr;
    } else {
        tk->isInTimer_ = true;
    }

    tk->schedTimer_ = &GetCurrentScheduler()->GetTimer();
    tk->schedTimer_->schedule(tk->suspendTimerId_, timepoint,
            [entry, tk]() mutable {
                Processer::Wakeup(entry, [tk]{
                        tk->isInTimer_ = false;
                        tk->schedTimer_ = nullptr;
                    });
            });
    return entry;
}

Processer::SuspendEntry Processer::SuspendBySelf(Task* tk)
{
    assert(tk == runningTask_);
    assert(tk->state_ == TaskState::runnable);

    tk->state_ = TaskState::block;
    uint64_t id = ++ LIBGO_TaskRefSuspendId(tk);

    std::unique_lock<TaskQueue::lock_t> lock(runnableQueue_.LockRef());
    runnableQueue_.nextWithoutLock(runningTask_, nextTask_);
    runnableQueue_.eraseWithoutLock(runningTask_, false, false);

    LIBGO_DebugPrint(dbg_suspend, "tk(%s) Suspend. nextTask(%s)", tk->DebugInfo(), nextTask_->DebugInfo());
    waitQueue_.pushWithoutLock(runningTask_, false);
    return SuspendEntry{ WeakPtr<Task>(tk), id };
}

bool Processer::IsExpire(SuspendEntry const& entry)
{
    IncursivePtr<Task> tkPtr = entry.tk_.lock();
    if (!tkPtr) return true;
    if (entry.id_ != LIBGO_TaskRefSuspendId(tkPtr.get())) return true;
    return false;
}

bool Processer::Wakeup(SuspendEntry const& entry, std::function<void()> const& functor)
{
    IncursivePtr<Task> tkPtr = entry.tk_.lock();
    if (!tkPtr) return false;

    auto proc = tkPtr->proc_;
    return proc ? proc->WakeupBySelf(tkPtr, entry.id_, functor) : false;
}

bool Processer::WakeupBySelf(IncursivePtr<Task> const& tkPtr, uint64_t id, std::function<void()> const& functor)
{
    Task* tk = tkPtr.get();

    if (id != LIBGO_TaskRefSuspendId(tk)) return false;

    std::unique_lock<TaskQueue::lock_t> lock(waitQueue_.LockRef());
    if (id != LIBGO_TaskRefSuspendId(tk)) return false;
    ++ LIBGO_TaskRefSuspendId(tk);

    if (functor)
        functor();

    if (tk->isInTimer_) {
        tk->isInTimer_ = false;
        tk->schedTimer_->join_unschedule(tk->suspendTimerId_);
        tk->schedTimer_ = nullptr;
    }

    bool ret = waitQueue_.eraseWithoutLock(tk, false, false);
    (void)ret;
    assert(ret);
    size_t sizeAfterPush = runnableQueue_.pushWithoutLock(tk, false);
    LIBGO_DebugPrint(dbg_suspend, "tk(%s) Wakeup. tk->state_ = %s. is-in-proc(%d). sizeAfterPush=%zu",
            tk->DebugInfo(), GetTaskStateName(tk->state_), GetCurrentProcesser() == this, sizeAfterPush);
    if (sizeAfterPush == 1 && GetCurrentProcesser() != this) {
        lock.unlock();
        NotifyCondition();
    }
    return true;
}

// task/task.cpp begin ---------------------------------------------------------------
    void Task::Run()
    {
        auto call_fn = [this]() {
    #if LIBGO_ENABLE_DEBUGGER
            if (Listener::GetTaskListener()) {
                Listener::GetTaskListener()->onStart(this->id_);
            }
    #endif

            this->fn_();
            this->fn_ = TaskF(); //让协程function对象的析构也在协程中执行

    #if LIBGO_ENABLE_DEBUGGER
            if (Listener::GetTaskListener()) {
                Listener::GetTaskListener()->onCompleted(this->id_);
            }
    #endif
        };

        if (CoroutineOptions::getInstance().exception_handle == eCoExHandle::immedaitely_throw) {
            call_fn();
        } else {
            try {
                call_fn();
            } catch (...) {
                this->fn_ = TaskF();

                std::exception_ptr eptr = std::current_exception();
                LIBGO_DebugPrint(dbg_exception, "task(%s) catched exception.", DebugInfo());

    #if LIBGO_ENABLE_DEBUGGER
                if (Listener::GetTaskListener()) {
                    Listener::GetTaskListener()->onException(this->id_, eptr);
                }
    #endif
            }
        }

    #if LIBGO_ENABLE_DEBUGGER
        if (Listener::GetTaskListener()) {
            Listener::GetTaskListener()->onFinished(this->id_);
        }
    #endif

        state_ = TaskState::done;
        Processer::StaticCoYield();
    }

    void FCONTEXT_CALL Task::StaticRun(intptr_t vp)
    {
        Task* tk = (Task*)vp;
        tk->Run();
    }

    Task::Task(TaskF const& fn, std::size_t stack_size)
        : ctx_(&Task::StaticRun, (intptr_t)this, stack_size), fn_(fn)
    {
    //    LIBGO_DebugPrint(dbg_task, "task(%s) construct. this=%p", DebugInfo(), this);
        extern_switcher_ = (void*)(new LibgoSwitcher);
    }

    Task::~Task()
    {
    //    printf("delete Task = %p, impl = %p, weak = %ld\n", this, this->impl_, (long)this->impl_->weak_);
        assert(!this->prev);
        assert(!this->next);
    //    LIBGO_DebugPrint(dbg_task, "task(%s) destruct. this=%p", DebugInfo(), this);
        delete (LibgoSwitcher*)extern_switcher_;
        extern_switcher_ = nullptr;
    }

    const char* Task::DebugInfo()
    {
        if (reinterpret_cast<void*>(this) == nullptr) return "nil";

        return TaskDebugInfo(this);
    }
// task/task.cpp end -----------------------------------------------------------------


// debug/debugger.cpp begin ----------------------------------------------------------
    std::string CoDebugger::GetAllInfo()
    {
        std::string s;
        s += P("==============================================");
        s += P("TaskCount: %d", TaskCount());
        s += P("CurrentTaskID: %lu", GetCurrentTaskID());
        s += P("CurrentTaskInfo: %s", GetCurrentTaskDebugInfo());
        s += P("CurrentTaskYieldCount: %lu", GetCurrentTaskYieldCount());
        s += P("CurrentThreadID: %d", GetCurrentThreadID());
    #if defined(LIBGO_SYS_Unix)
        s += P("ReactorThreadNumber: %d", Reactor::GetReactorThreadCount());
    #endif
        s += P("--------------------------------------------");
        s += P("Task Map:");
    #if LIBGO_ENABLE_DEBUGGER
        auto mPtr = Task::SafeGetDbgMap();
        std::map<TaskState, std::map<SourceLocation, std::vector<Task*>>> locMap;
        for (auto & ptr : *mPtr)
        {
            Task* tk = (Task*)ptr;
            locMap[tk->state_][TaskRefLocation(tk)].push_back(tk);
        }

        for (auto & kkv : locMap) {
            s += P("  state = %s ->", GetTaskStateName(kkv.first));
            for (auto & kv : kkv.second)
            {
                s += P("    [%d] Loc {%s}", (int)kv.second.size(), kv.first.ToString().c_str());
                int i = 0;
                for (auto tk : kv.second) {
                    s += P("     -> [%d] Task {%s}", i++, tk->DebugInfo());
                }
            }
        }
        mPtr.reset();
    #else
        s += P("No data, please make libgo with 'cmake .. -DENABLE_DEBUGGER=ON'");
    #endif
        s += P("--------------------------------------------");

        return s;
    }
    int CoDebugger::TaskCount()
    {
    #if LIBGO_ENABLE_DEBUGGER
        return Task::getCount();
    #else
        return -1;
    #endif
    }
    unsigned long CoDebugger::GetCurrentTaskID()
    {
        return static_cast<unsigned long>(::co::Scheduler::getInstance().GetCurrentTaskID());
    }
    unsigned long CoDebugger::GetCurrentTaskYieldCount()
    {
        return static_cast<unsigned long>(::co::Scheduler::getInstance().GetCurrentTaskYieldCount());
    }
    void CoDebugger::SetCurrentTaskDebugInfo(const std::string & info)
    {
        ::co::Scheduler::getInstance().SetCurrentTaskDebugInfo(info);
    }
    const char * CoDebugger::GetCurrentTaskDebugInfo()
    {
        return Processer::GetCurrentTask()->DebugInfo();
    }
// debug/debugger.cpp end ------------------------------------------------------------

} //namespace co

// routine_sync_libgo/libgo_switcher.cpp begin ---------------------------------------
namespace libgo
{

// 初始化函数 (用户需要打开namespace自行实现这个初始化函数)
// 在这个函数体内执行 RoutineSyncPolicy::registerSwitchers函数
void routine_sync_init_callback()
{
    // libgo默认用0级, 用户自定义更多协程支持的时候可以使用1级或更高等级
    RoutineSyncPolicy::registerSwitchers<::co::LibgoSwitcher>(0);
}
// routine_sync_libgo/libgo_switcher.cpp end -----------------------------------------


} // namespace libgo

