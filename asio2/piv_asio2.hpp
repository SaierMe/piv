#ifndef _PIV_ASIO2_HPP
#define _PIV_ASIO2_HPP

#include <asio2/asio2.hpp>
#include <piv_string.hpp>

namespace piv
{
    class ping : public asio2::ping_t<ping>
    {
    private:
        std::any _recv_fun;

    public:
        using asio2::ping_t<ping>::ping_t;

        template <class Arg, class F>
        inline ping &set_recv_fun(F &&fun)
        {
            _recv_fun.emplace<std::function<void(Arg &)>>(std::forward<F>(fun));
            return *this;
        }

        inline bool has_recv_fun() const
        {
            return _recv_fun.has_value();
        }

        template <class T>
        inline T &get_recv_fun()
        {
            return std::any_cast<T &>(_recv_fun);
        }

        inline int32_t get_packet_size() const
        {
            return static_cast<int32_t>(body_.size() + 8);
        }
    };
} // namespace piv

#endif // _PIV_ASIO2_HPP
