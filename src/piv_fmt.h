/*********************************************\
 * 火山视窗PIV模块 - 标准格式化类(FMT)       *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 版本: 2023/02/28                          *
\*********************************************/

#ifndef _PIV_FMT_H
#define _PIV_FMT_H

#include "piv_string.hpp"
// 火山的_CT宏跟chrono冲突,需要临时取消定义
#ifdef _CT
#undef _CT
#endif

#ifndef FMT_HEADER_ONLY
#define FMT_HEADER_ONLY
#endif

#include "fmt/format.h"
#include "fmt/xchar.h"

// 重新定义_CT宏
#ifndef _CT
#define _CT(x) CVolConstString(_T(x))
#endif

// 自定义fmtlib的格式化参数,以支持PivSting和PivStringView
template <>
struct fmt::formatter<PivStringW, wchar_t> : formatter<fmt::basic_string_view<wchar_t>, wchar_t>
{
    template <typename FormatContext>
    auto format(PivStringW const &val, FormatContext &ctx) const
        -> decltype(ctx.out())
    {
        return formatter<fmt::basic_string_view<wchar_t>, wchar_t>::format(val.data(), ctx);
    }
};
template <>
struct fmt::formatter<PivStringU, char> : formatter<fmt::basic_string_view<char>, char>
{
    template <typename FormatContext>
    auto format(PivStringU const &val, FormatContext &ctx) const
        -> decltype(ctx.out())
    {
        return formatter<fmt::basic_string_view<char>, char>::format(val.data(), ctx);
    }
};
template <>
struct fmt::formatter<PivStringA, char> : formatter<fmt::basic_string_view<char>, char>
{
    template <typename FormatContext>
    auto format(PivStringA const &val, FormatContext &ctx) const
        -> decltype(ctx.out())
    {
        return formatter<fmt::basic_string_view<char>, char>::format(val.data(), ctx);
    }
};

template <>
struct fmt::formatter<PivStringViewW, wchar_t> : formatter<fmt::basic_string_view<wchar_t>, wchar_t>
{
    template <typename FormatContext>
    auto format(PivStringViewW const &val, FormatContext &ctx) const
        -> decltype(ctx.out())
    {
        return formatter<fmt::basic_string_view<wchar_t>, wchar_t>::format(val.data(), ctx);
    }
};
template <>
struct fmt::formatter<PivStringViewU, char> : formatter<fmt::basic_string_view<char>, char>
{
    template <typename FormatContext>
    auto format(PivStringViewU const &val, FormatContext &ctx) const
        -> decltype(ctx.out())
    {
        return formatter<fmt::basic_string_view<char>, char>::format(val.data(), ctx);
    }
};
template <>
struct fmt::formatter<PivStringViewA, char> : formatter<fmt::basic_string_view<char>, char>
{
    template <typename FormatContext>
    auto format(PivStringViewA const &val, FormatContext &ctx) const
        -> decltype(ctx.out())
    {
        return formatter<fmt::basic_string_view<char>, char>::format(val.data(), ctx);
    }
};

#endif // _PIV_FMT_H
