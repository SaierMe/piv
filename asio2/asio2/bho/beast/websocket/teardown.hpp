//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef BHO_BEAST_WEBSOCKET_TEARDOWN_HPP
#define BHO_BEAST_WEBSOCKET_TEARDOWN_HPP

#include <asio2/bho/beast/core/detail/config.hpp>
#include <asio2/bho/beast/core/error.hpp>
#include <asio2/bho/beast/core/role.hpp>
#include <asio/basic_stream_socket.hpp>
#include <type_traits>

namespace bho {
namespace beast {
namespace websocket {

/** Tear down a connection.

    This tears down a connection. The implementation will call
    the overload of this function based on the `Socket` parameter
    used to consruct the socket. When `Socket` is a user defined
    type, and not a `net::ip::tcp::socket` or any
    `net::ssl::stream`, callers are responsible for
    providing a suitable overload of this function.

    @param role The role of the local endpoint

    @param socket The socket to tear down.

    @param ec Set to the error if any occurred.
*/
template<class Socket>
void
teardown(
    role_type role,
    Socket& socket,
    error_code& ec)
{
    bho::ignore_unused(role, socket, ec);
/*
    If you are trying to use OpenSSL and this goes off, you need to
    add an include for <asio2/bho/beast/websocket/ssl.hpp>.

    If you are creating an instance of beast::websocket::stream with your
    own user defined type, you must provide an overload of teardown with
    the corresponding signature (including the role_type).
*/
    static_assert(sizeof(Socket)==-1,
        "Unknown Socket type in teardown.");
}

/** Start tearing down a connection.

    This begins tearing down a connection asynchronously.
    The implementation will call the overload of this function
    based on the `Socket` parameter used to consruct the socket.
    When `Stream` is a user defined type, and not a
    `net::ip::tcp::socket` or any `net::ssl::stream`,
    callers are responsible for providing a suitable overload
    of this function.

    @param role The role of the local endpoint

    @param socket The socket to tear down.

    @param handler The completion handler to invoke when the operation
    completes. The implementation takes ownership of the handler by
    performing a decay-copy. The equivalent function signature of
    the handler must be:
    @code
    void handler(
        error_code const& error // result of operation
    );
    @endcode
    If the handler has an associated immediate executor,
    an immediate completion will be dispatched to it.
    Otherwise, the handler will not be invoked from within
    this function. Invocation of the handler will be performed in a
    manner equivalent to using `net::post`.

*/
template<
    class Socket,
    class TeardownHandler>
void
async_teardown(
    role_type role,
    Socket& socket,
    TeardownHandler&& handler)
{
    bho::ignore_unused(role, socket, handler);
/*
    If you are trying to use OpenSSL and this goes off, you need to
    add an include for <asio2/bho/beast/websocket/ssl.hpp>.

    If you are creating an instance of beast::websocket::stream with your
    own user defined type, you must provide an overload of teardown with
    the corresponding signature (including the role_type).
*/
    static_assert(sizeof(Socket)==-1,
        "Unknown Socket type in async_teardown.");
}

} // websocket

//------------------------------------------------------------------------------

namespace websocket {

/** Tear down a `net::ip::tcp::socket`.

    This tears down a connection. The implementation will call
    the overload of this function based on the `Stream` parameter
    used to consruct the socket. When `Stream` is a user defined
    type, and not a `net::ip::tcp::socket` or any
    `net::ssl::stream`, callers are responsible for
    providing a suitable overload of this function.

    @param role The role of the local endpoint

    @param socket The socket to tear down.

    @param ec Set to the error if any occurred.
*/
template<class Protocol, class Executor>
void
teardown(
    role_type role,
    net::basic_stream_socket<
        Protocol, Executor>& socket,
    error_code& ec);

/** Start tearing down a `net::ip::tcp::socket`.

    This begins tearing down a connection asynchronously.
    The implementation will call the overload of this function
    based on the `Stream` parameter used to consruct the socket.
    When `Stream` is a user defined type, and not a
    `net::ip::tcp::socket` or any `net::ssl::stream`,
    callers are responsible for providing a suitable overload
    of this function.

    @param role The role of the local endpoint

    @param socket The socket to tear down.

    @param handler The completion handler to invoke when the operation
    completes. The implementation takes ownership of the handler by
    performing a decay-copy. The equivalent function signature of
    the handler must be:
    @code
    void handler(
        error_code const& error // result of operation
    );
    @endcode   
    If the handler has an associated immediate executor,
    an immediate completion will be dispatched to it.
    Otherwise, the handler will not be invoked from within
    this function. Invocation of the handler will be performed in a
    manner equivalent to using `net::post`.

    @par Per-Operation Cancellation

    This asynchronous operation supports cancellation for the following
    net::cancellation_type values:

    @li @c net::cancellation_type::terminal
    @li @c net::cancellation_type::partial
    @li @c net::cancellation_type::total

    if they are also supported by the socket's @c async_wait operation.

*/
template<
    class Protocol, class Executor,
    class TeardownHandler>
void
async_teardown(
    role_type role,
    net::basic_stream_socket<
        Protocol, Executor>& socket,
    TeardownHandler&& handler);

} // websocket
} // beast
} // bho

#include <asio2/bho/beast/websocket/impl/teardown.hpp>

#endif
