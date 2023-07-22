#ifndef LIBGO_COMMON_SYNTAX_HELPER_H
#define LIBGO_COMMON_SYNTAX_HELPER_H
#include "../scheduler/scheduler.h"

namespace co
{

enum {
    opt_scheduler,
    opt_stack_size,
    opt_dispatch,
    opt_affinity,
};

template <int OptType>
struct __go_option;

template <>
struct __go_option<opt_scheduler>
{
    Scheduler* scheduler_;
    explicit __go_option(Scheduler* scheduler) : scheduler_(scheduler) {}
    explicit __go_option(Scheduler& scheduler) : scheduler_(&scheduler) {}
};

template <>
struct __go_option<opt_stack_size>
{
    std::size_t stack_size_;
    explicit __go_option(std::size_t v) : stack_size_(v) {}
};

template <>
struct __go_option<opt_affinity>
{
    bool affinity_;
    explicit __go_option(bool affinity) : affinity_(affinity) {}
};

struct __go
{
    __go(const char* file, int lineno)
    {
        scheduler_ = nullptr;
        opt_.file_ = file;
        opt_.lineno_ = lineno;
    }

    template <typename Function>
    LIBGO_INLINE void operator-(Function const& f)
    {
        if (!scheduler_) scheduler_ = Processer::GetCurrentScheduler();
        if (!scheduler_) scheduler_ = &Scheduler::getInstance();
        scheduler_->CreateTask(f, opt_);
    }

    LIBGO_INLINE __go& operator-(__go_option<opt_scheduler> const& opt)
    {
        scheduler_ = opt.scheduler_;
        return *this;
    }

    LIBGO_INLINE __go& operator-(__go_option<opt_stack_size> const& opt)
    {
        opt_.stack_size_ = opt.stack_size_;
        return *this;
    }

    LIBGO_INLINE __go& operator-(__go_option<opt_affinity> const& opt)
    {
        opt_.affinity_ = opt.affinity_;
        return *this;
    }

    TaskOpt opt_;
    Scheduler* scheduler_;
};

} //namespace co

#endif // LIBGO_COMMON_SYNTAX_HELPER_H
