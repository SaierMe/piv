#ifndef LIBGO_TASK_TASK_H
#define LIBGO_TASK_TASK_H
#include "../common/config.h"
#include "../common/ts_queue.h"
#include "../common/anys.h"
#include "../context/context.h"
#include "../debug/debugger.h"
#include "../routine_sync/timer.h"

namespace co
{

enum class TaskState
{
    runnable,
    block,
    done,
};

static const char* GetTaskStateName(TaskState state)
{
    switch (state) {
    case TaskState::runnable:
        return "Runnable";
    case TaskState::block:
        return "Block";
    case TaskState::done:
        return "Done";
    default:
        return "Unkown";
    }
}

typedef std::function<void()> TaskF;

struct TaskGroupKey {};
typedef Anys<TaskGroupKey> TaskAnys;

class Processer;

struct Task
    : public TSQueueHook, public SharedRefObject, public CoDebugger::DebuggerBase<Task>
{
    TaskState state_ = TaskState::runnable;
    uint64_t id_;
    Processer* proc_ = nullptr;
    Context ctx_;
    TaskF fn_;
    std::exception_ptr eptr_;           // 保存exception的指针
    TaskAnys anys_;
    void* extern_switcher_ {nullptr};

    ::libgo::RoutineSyncTimer::TimerId suspendTimerId_;
    ::libgo::RoutineSyncTimer* schedTimer_ = nullptr;
    bool isInTimer_ {false};

    uint64_t yieldCount_ = 0;

    atomic_t<uint64_t> suspendId_ {0};

    Task(TaskF const& fn, std::size_t stack_size);
    ~Task();

    LIBGO_INLINE void SwapIn()
    {
        ctx_.SwapIn();
    }
    //LIBGO_INLINE void SwapTo(Task* other)
    //{
    //    ctx_.SwapTo(other->ctx_);
    //}
    LIBGO_INLINE void SwapOut()
    {
        ctx_.SwapOut();
    }

    const char* DebugInfo();

private:
    void Run();

    static void FCONTEXT_CALL StaticRun(intptr_t vp);

    Task(Task const&) = delete;
    Task(Task &&) = delete;
    Task& operator=(Task const&) = delete;
    Task& operator=(Task &&) = delete;
};

#define LIBGO_TaskRefDefine(type, name) \
    LIBGO_INLINE type& TaskRef ## name(Task *tk) \
    { \
        typedef type T; \
        static std::size_t idx = -1; \
        if (UNLIKELY(tk == reinterpret_cast<Task*>(0x1))) { \
            if (idx == -1) \
                idx = TaskAnys::Register<T>(); \
            static T ignore{}; \
            return ignore; \
        } \
        return tk->anys_.get<T>(idx); \
    }
#define LIBGO_TaskRefInit(name) do { TaskRef ## name(reinterpret_cast<Task*>(0x1)); } while(0)

} // namespace co
#endif // LIBGO_TASK_TASK_H
