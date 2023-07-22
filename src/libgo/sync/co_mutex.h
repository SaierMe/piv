#ifndef LIBGO_SYNC_CO_MUTEX_H
#define LIBGO_SYNC_CO_MUTEX_H
#include "../common/config.h"
#include "../routine_sync/mutex.h"

namespace co
{

typedef libgo::Mutex CoMutex;
typedef CoMutex co_mutex;

} //namespace co
#endif // LIBGO_SYNC_CO_MUTEX_H
