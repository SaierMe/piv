/*********************************************\
 * 火山视窗PIV模块 - 标准格式化类(FMT)       *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef FMT_HEADER_ONLY
#define FMT_HEADER_ONLY
#endif

#include "piv_encoding.hpp"

#include "fmt/format.h"
#include "fmt/xchar.h"

#ifndef _PIV_FMT_HPP
#define _PIV_FMT_HPP

template <>
struct fmt::formatter<CWConstString, char>
{
	template<typename FormatParseContext>
	constexpr auto parse(FormatParseContext& ctx) -> decltype(ctx.begin())
	{
		// Parse the presentation format and store it in the formatter:
		auto it = ctx.begin(), end = ctx.end();

		// Check if reached the end of the range:
		if (it != end && *it != '}') throw format_error("invalid format");

		// Return an iterator past the end of the parsed range:
		return it;
	}

	template <typename FormatContext>
	auto format(const CWConstString& s, FormatContext& ctx) -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}", PivW2U{s}.GetText());
	}
};

template <>
struct fmt::formatter<CWConstString, wchar_t>
{
	template<typename FormatParseContext>
	constexpr auto parse(FormatParseContext& ctx) -> decltype(ctx.begin())
	{
		// Parse the presentation format and store it in the formatter:
		auto it = ctx.begin(), end = ctx.end();

		// Check if reached the end of the range:
		if (it != end && *it != '}') throw format_error("invalid format");

		// Return an iterator past the end of the parsed range:
		return it;
	}

	template <typename FormatContext>
	auto format(const CWConstString& s, FormatContext& ctx) -> decltype(ctx.out())
	{
		return format_to(ctx.out(), L"{}", s.GetText());
	}
};

template <>
struct fmt::formatter<CWString, char>
{
	template<typename FormatParseContext>
	constexpr auto parse(FormatParseContext& ctx) -> decltype(ctx.begin())
	{
		// Parse the presentation format and store it in the formatter:
		auto it = ctx.begin(), end = ctx.end();

		// Check if reached the end of the range:
		if (it != end && *it != '}') throw format_error("invalid format");

		// Return an iterator past the end of the parsed range:
		return it;
	}

	template <typename FormatContext>
	auto format(const CWString& s, FormatContext& ctx) -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}", PivW2U{s}.GetText());
	}
};

template <>
struct fmt::formatter<CWString, wchar_t>
{
	template<typename FormatParseContext>
	constexpr auto parse(FormatParseContext& ctx) -> decltype(ctx.begin())
	{
		// Parse the presentation format and store it in the formatter:
		auto it = ctx.begin(), end = ctx.end();

		// Check if reached the end of the range:
		if (it != end && *it != '}') throw format_error("invalid format");

		// Return an iterator past the end of the parsed range:
		return it;
	}

	template <typename FormatContext>
	auto format(const CWString& s, FormatContext& ctx) -> decltype(ctx.out())
	{
		return format_to(ctx.out(), L"{}", s.GetText());
	}
};

#endif // _PIV_FMT_HPP
