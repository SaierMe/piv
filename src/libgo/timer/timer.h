﻿#ifndef LIBGO_TIMER_TIMER_H
#define LIBGO_TIMER_TIMER_H
#include "../common/config.h"
#include "../common/spinlock.h"
#include "../routine_sync/timer.h"
#include "../routine_sync/condition_variable.h"
#include "../scheduler/scheduler.h"
#include "../scheduler/processer.h"
#include "../sync/channel.h"
#include "../libgo.h"

namespace co
{

class CoTimer
{
public:
    typedef ::libgo::RoutineSyncTimerT<::libgo::Mutex, ::libgo::ConditionVariable> CoroutineTimer;
    typedef CoroutineTimer::func_type func_t;

    struct TimerIdImpl
    {
    public:
        explicit TimerIdImpl(std::shared_ptr<CoroutineTimer> const& timer) : timer_(timer) {}

        bool join_unschedule()
        {
            auto timer = timer_.lock();
            if (!timer) return false;
            return timer->join_unschedule(id_);
        }

    private:
        friend class CoTimer;
        std::weak_ptr<CoroutineTimer> timer_;
        CoroutineTimer::TimerId id_;
    };
    typedef std::shared_ptr<TimerIdImpl> TimerIdImplPtr;

    struct TimerId
    {
    public:
        TimerId() = default;
        TimerId(TimerIdImplPtr const& ptr) : impl_(ptr) {}

        explicit operator bool() const {
            return !!impl_;
        }

        bool StopTimer() {
            if (!impl_) return false;
            return impl_->join_unschedule();
        }

    private:
        TimerIdImplPtr impl_;
    };

public:
    template <typename Rep, typename Period>
    explicit CoTimer(std::chrono::duration<Rep, Period> dur, Scheduler * scheduler = nullptr)
    {
        impl_.reset(new CoroutineTimer);
        Initialize(scheduler);
    }

    explicit CoTimer(Scheduler * scheduler = nullptr)
    {
        impl_.reset(new CoroutineTimer);
        Initialize(scheduler);
    }

    ~CoTimer()
    {
        impl_->stop();
    }

    TimerId ExpireAt(FastSteadyClock::duration dur, func_t const& cb)
    {
        auto tp = FastSteadyClock::now();
        tp += dur;
        return ExpireAt(tp, cb);
    }

    TimerId ExpireAt(FastSteadyClock::time_point tp, func_t const& cb)
    {
        TimerIdImplPtr idImpl = std::make_shared<TimerIdImpl>(impl_);
        func_t f = [idImpl, cb] {
            cb();
        };
        impl_->schedule(idImpl->id_, tp, f);
        return TimerId(idImpl);
    }

    template <typename Rep, typename Period>
    TimerId ExpireAt(std::chrono::duration<Rep, Period> dur, func_t const& fn) {
        return ExpireAt(std::chrono::duration_cast<FastSteadyClock::duration>(dur), fn);
    }

private:
    CoTimer(CoTimer const&) = delete;
    CoTimer& operator=(CoTimer const&) = delete;

    void Initialize(Scheduler * scheduler)
    {
        std::shared_ptr<CoroutineTimer> ptr = impl_;
        ::co::__go(__FILE__, __LINE__) - ::co::__go_option<::co::opt_scheduler>{scheduler} - [ptr] {
            ptr->run();
        };
    }

private:
    std::shared_ptr<CoroutineTimer> impl_;
};

} // namespace co
#endif // LIBGO_TIMER_TIMER_H
