#ifndef LIBGO_COMMON_CLOCK_H
#define LIBGO_COMMON_CLOCK_H
#include <time.h>
#include <chrono>
#include <thread>
#include <limits>
#include "spinlock.h"

namespace co
{

class FastSteadyClock
	: public std::chrono::steady_clock
{
public:
	static void ThreadRun() {}
};

} // namespace co
#endif // LIBGO_COMMON_CLOCK_H
