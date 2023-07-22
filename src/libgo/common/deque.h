#ifndef LIBGO_COMMON_DEQUE_H
#define LIBGO_COMMON_DEQUE_H

#include <deque>

namespace co
{
    template <typename T, typename Alloc = std::allocator<T>>
    using Deque = std::deque<T, Alloc>;

    // TODO: 实现多读一写线程安全的deque

} // namespace co
#endif // LIBGO_COMMON_DEQUE_H
