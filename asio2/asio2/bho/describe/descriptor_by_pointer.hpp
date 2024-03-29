#ifndef BHO_DESCRIBE_DESCRIPTOR_BY_POINTER_HPP_INCLUDED
#define BHO_DESCRIBE_DESCRIPTOR_BY_POINTER_HPP_INCLUDED

// Copyright 2021 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <asio2/bho/describe/detail/config.hpp>

#if defined(__cpp_nontype_template_parameter_auto) && __cpp_nontype_template_parameter_auto >= 201606L

#include <asio2/bho/mp11/algorithm.hpp>
#include <asio2/bho/mp11/bind.hpp>
#include <asio2/bho/mp11/integral.hpp>

namespace bho
{
namespace describe
{

namespace detail
{

template<class Pm> constexpr bool cx_pmeq( Pm p1, Pm p2 )
{
    return p1 == p2;
}

template<class Pm1, class Pm2> constexpr bool cx_pmeq( Pm1, Pm2 )
{
    return false;
}

template<auto Pm> struct match_by_pointer
{
    template<class D> using fn = mp11::mp_bool< cx_pmeq( D::pointer, Pm ) >;
};

} // namespace detail

template<class L, auto Pm> using descriptor_by_pointer = mp11::mp_at<L, mp11::mp_find_if_q<L, detail::match_by_pointer<Pm>>>;

} // namespace describe
} // namespace bho

#endif // __cpp_nontype_template_parameter_auto

#endif // #ifndef BHO_DESCRIBE_DESCRIPTOR_BY_POINTER_HPP_INCLUDED
