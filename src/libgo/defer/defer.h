#ifndef LIBGO_DEFER_DEFER_H
#define LIBGO_DEFER_DEFER_H

#include <utility>
#include "../scheduler/scheduler.h"
#include "../cls/co_local_storage.h"

namespace co
{
    static inline void*& GetLastDeferTls()
    {
        thread_local static void* p = nullptr;
        return p;
    }

    struct dismisser
    {
        virtual bool dismiss() = 0;

        static dismisser*& GetLastDefer()
        {
            Task* tk = Processer::GetCurrentTask();
            if (tk) {
                LIBGO_CLS_REF(dismisser*) defer_cls = LIBGO_CLS(dismisser*, nullptr);
                return (dismisser*&)defer_cls;
            }

            return reinterpret_cast<dismisser*&>(GetLastDeferTls());
        }
        static void SetLastDefer(dismisser* ptr)
        {
            GetLastDefer() = ptr;
        }
        static void ClearLastDefer(dismisser*)
        {
            GetLastDefer() = nullptr;
        }
    };

    template<typename Func>
    class __defer : public dismisser
    {
    public:
        explicit __defer(Func && on_exit_scope)
            : on_exit_scope_(on_exit_scope), dismiss_(false) 
        {
            SetLastDefer(this);
        }

        __defer(__defer && other)
            : on_exit_scope_(std::forward<Func>(other.on_exit_scope_))
        {
            dismiss_ = other.dismiss_;
            other.dismiss_ = true;
            SetLastDefer(this);
        }

        __defer& operator=(__defer && other)
        {
            on_exit_scope_ = std::forward<Func>(other.on_exit_scope_);
            dismiss_ = other.dismiss_;
            other.dismiss_ = true;
            SetLastDefer(this);
        }

        ~__defer()
        {
            if (GetLastDefer() == this) {
                ClearLastDefer(this);
            }

            if (!dismiss_)
                on_exit_scope_();
        }

        virtual bool dismiss() override
        {
            if (!dismiss_) {
                dismiss_ = true;
                return true;
            } else {
                return false;
            }
        }

    private:
        Func on_exit_scope_;
        bool dismiss_;

    private: // noncopyable
        __defer(__defer const &) = delete;
        __defer& operator=(__defer const &) = delete;
    };

    struct __defer_op
    {
        template <typename Func>
        __defer<Func> operator-(Func && func)
        {
            return __defer<Func>(std::forward<Func>(func));
        }
    };

    struct FakeDismisser : public dismisser
    {
        virtual bool dismiss() override { return false; }
    };

    static dismisser& GetLastDefer()
    {
        dismisser* d = dismisser::GetLastDefer();
        if (d)
            return *d;

        static FakeDismisser nullD;
        return nullD;
    }

} // namespace co
#endif // LIBGO_DEFER_DEFER_H
