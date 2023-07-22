#ifndef LIBGO_POOL_ASYNC_COROUTINE_POOL_H
#define LIBGO_POOL_ASYNC_COROUTINE_POOL_H
#include "../common/config.h"
#include "../common/spinlock.h"
#include "../common/syntax_helper.h"
#include "../sync/channel.h"
#include "../scheduler/scheduler.h"

namespace co {

// 协程池
// 可以无缝与异步代码结合, 处理异步框架中的阻塞事件
class AsyncCoroutinePool
{
public:
    static AsyncCoroutinePool* Create(size_t maxCallbackPoints = 128)
    {
        return new AsyncCoroutinePool(maxCallbackPoints);
    }

    typedef std::function<void()> Func;

    // 初始化协程数量
    void InitCoroutinePool(size_t maxCoroutineCount)
    {
        maxCoroutineCount_ = maxCoroutineCount;
    }

    // 启动协程池 
    void Start(int minThreadNumber, int maxThreadNumber = 0)
    {
        if (!started_.try_lock()) return ;
        std::thread([=]{ 
                    scheduler_->Start(minThreadNumber, maxThreadNumber); 
                }).detach();
        if (maxCoroutineCount_ == 0) {
            maxCoroutineCount_ = (std::max)(minThreadNumber * 128, maxThreadNumber);
            maxCoroutineCount_ = (std::min<size_t>)(maxCoroutineCount_, 10240);
        }
        for (size_t i = 0; i < maxCoroutineCount_; ++i) {
            ::co::__go(__FILE__, __LINE__)-::co::__go_option<::co::opt_scheduler>{scheduler_}-[this]{
                this->Go();
            };
        }
    }

    void Post(Func const& func, Func const& callback)
    {
        PoolTask task{func, callback};
        tasks_ << std::move(task);
    }

    template <typename R>
    void Post(Channel<R> const& ret, std::function<R()> const& func) {
        Post([=]{ ret << func(); }, NULL);
    }

    void Post(Channel<void> const& ret, std::function<void()> const& func) {
        Post([=]{ func(); ret << nullptr; }, NULL);
    }

    template <typename R>
    void Post(std::function<R()> const& func, std::function<void(R&)> const& callback) {
        std::shared_ptr<R> ctx(new R);
        Post([=]{ *ctx = func(); }, [=]{ callback(*ctx); });
    }

    // 触发点
    struct CallbackPoint
    {
        size_t Run(size_t maxTrigger = 0)
        {
            size_t i = 0;
            for (; i < maxTrigger || maxTrigger == 0; ++i) {
                Func cb;
                if (!channel_.TryPop(cb))
                    break;
                cb();
            }
            return i;
        }

        void SetNotifyFunc(Func const& notify)
        {
            notify_ = notify;
        }

    private:
        friend class AsyncCoroutinePool;

        void Post(Func && cb)
        {
            channel_ << std::move(cb);
        }

        void Notify()
        {
            if (notify_) notify_();
        }

    private:
        Channel<Func> channel_;
        Func notify_;
    };

    // 绑定回调函数触发点, 可以绑定多个触发点, 轮流使用.
    // 如果不绑定触发点, 则回调函数直接在协程池的工作线程中触发.
    // 线程安全接口
    bool AddCallbackPoint(CallbackPoint * point)
    {
        size_t writeIdx = writePointsCount_++;
        if (writeIdx >= maxCallbackPoints_) {
            --writePointsCount_;
            return false;
        }

        points_[writeIdx] = point;
        while (!pointsCount_.compare_exchange_weak(writeIdx, writeIdx + 1,
                    std::memory_order_acq_rel, std::memory_order_relaxed)) ;
        return true;
    }

private:
    AsyncCoroutinePool(size_t maxCallbackPoints)
    {
        maxCoroutineCount_ = 0;
        maxCallbackPoints_ = maxCallbackPoints;
        scheduler_ = Scheduler::Create();
        points_ = new CallbackPoint*[maxCallbackPoints_];
    }
    ~AsyncCoroutinePool() {}

    void Go()
    {
        for (;;) {
            PoolTask task;
            tasks_ >> task;

            if (task.func_)
                task.func_();

            if (!task.cb_)
                continue;

            size_t pointsCount = pointsCount_;
            if (!pointsCount) {
                task.cb_();
                continue;
            }

            size_t idx = ++robin_ % pointsCount;
            points_[idx]->Post(std::move(task.cb_));
            points_[idx]->Notify();
        }
    }

    struct PoolTask {
        Func func_;
        Func cb_;
    };

private:
    size_t maxCoroutineCount_;
    std::atomic<int> coroutineCount_{0};
    Scheduler* scheduler_;
    Channel<PoolTask> tasks_;
    std::atomic<size_t> pointsCount_{0};
    std::atomic<size_t> writePointsCount_{0};
    size_t maxCallbackPoints_;
    std::atomic<size_t> robin_{0};
    CallbackPoint ** points_;
    LFLock started_;
};

} // namespace co
#endif // LIBGO_POOL_ASYNC_COROUTINE_POOL_H
