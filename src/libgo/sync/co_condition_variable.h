#ifndef LIBGO_SYNC_CO_CONDITION_VARIABLE_H
#define LIBGO_SYNC_CO_CONDITION_VARIABLE_H
#include "../common/config.h"
#include "../scheduler/processer.h"
#include <list>
#include <condition_variable>
#include "../routine_sync/condition_variable.h"

namespace co
{

typedef libgo::ConditionVariable ConditionVariable;
typedef ConditionVariable co_condition_variable;

} // namespace co
#endif // LIBGO_SYNC_CO_CONDITION_VARIABLE_H
