#ifndef BHO_CORE_MEMORY_RESOURCE_HPP_INCLUDED
#define BHO_CORE_MEMORY_RESOURCE_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  Copyright 2023 Peter Dimov
//  Distributed under the Boost Software License, Version 1.0.
//  https://www.boost.org/LICENSE_1_0.txt

#include <asio2/bho/core/max_align.hpp>
#include <asio2/bho/config.hpp>
#include <asio2/bho/config/workaround.hpp>
#include <cstddef>

// Define our own placement new to avoid the inclusion of <new>
// (~9K extra lines) at Ion Gaztanhaga's request.
//
// We can use our own because [intro.object] p13 says:
//
// Any implicit or explicit invocation of a function named `operator new`
// or `operator new[]` implicitly creates objects in the returned region of
// storage and returns a pointer to a suitable created object.

namespace bho
{
namespace core
{
namespace detail
{

struct placement_new_tag {};

} // namespace detail
} // namespace core
} // namespace bho

inline void* operator new( std::size_t, void* p, bho::core::detail::placement_new_tag )
{
    return p;
}

inline void operator delete( void*, void*, bho::core::detail::placement_new_tag )
{
}

namespace bho
{
namespace core
{

class memory_resource
{
public:

#if defined(BHO_NO_CXX11_DEFAULTED_FUNCTIONS) || BHO_WORKAROUND(BHO_GCC, < 40700)

    virtual ~memory_resource() {}

#else

    virtual ~memory_resource() = default;

#endif

    BHO_ATTRIBUTE_NODISCARD void* allocate( std::size_t bytes, std::size_t alignment = max_align )
    {
        // https://github.com/boostorg/container/issues/199
        // https://cplusplus.github.io/LWG/issue3471

        return ::operator new( bytes, do_allocate( bytes, alignment ), core::detail::placement_new_tag() );
    }

    void deallocate( void* p, std::size_t bytes, std::size_t alignment = max_align )
    {
        do_deallocate( p, bytes, alignment );
    }

    bool is_equal( memory_resource const & other ) const BHO_NOEXCEPT
    {
        return do_is_equal( other );
    }

private:

    virtual void* do_allocate( std::size_t bytes, std::size_t alignment ) = 0;
    virtual void do_deallocate( void* p, std::size_t bytes, std::size_t alignment ) = 0;

    virtual bool do_is_equal( memory_resource const & other ) const BHO_NOEXCEPT = 0;
};

inline bool operator==( memory_resource const& a, memory_resource const& b ) BHO_NOEXCEPT
{
    return &a == &b || a.is_equal( b );
}

inline bool operator!=( memory_resource const& a, memory_resource const& b ) BHO_NOEXCEPT
{
    return !( a == b );
}

} // namespace core
} // namespace bho

#endif  // #ifndef BHO_CORE_MEMORY_RESOURCE_HPP_INCLUDED
