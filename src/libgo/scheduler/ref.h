#ifndef LIBGO_SCHEDULER_REF_H
#define LIBGO_SCHEDULER_REF_H
#include "../common/config.h"
#include "../task/task.h"
#include "../common/util.h"

namespace co
{

LIBGO_TaskRefDefine(bool, Affinity)
LIBGO_TaskRefDefine(SourceLocation, Location)
LIBGO_TaskRefDefine(std::string, DebugInfo)
//LIBGO_TaskRefDefine(atomic_t<uint64_t>, SuspendId)

#define LIBGO_TaskRefSuspendId(tk) tk->suspendId_

inline const char* TaskDebugInfo(Task *tk)
{
    std::string& info = TaskRefDebugInfo(tk);
    if (info.empty()) {
        char buf[128] = {};
        SourceLocation& loc = TaskRefLocation(tk);
        int len = snprintf(buf, sizeof(buf) - 1, "id:%lu, file:%s, line:%d", (unsigned long)tk->id_, loc.file_, loc.lineno_);
        info.assign(buf, len);
    }
    return info.c_str();
}

} // namespace co
#endif // LIBGO_SCHEDULER_REF_H
