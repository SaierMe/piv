﻿/*********************************************\
 * 火山视窗-PIV模块                          *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef _PIV_BASE_HPP
#define _PIV_BASE_HPP

// 包含火山视窗基本类,它在火山里的包含顺序比较靠前(全局-110)
#ifndef __VOL_BASE_H__
#include <sys/base/libs/win_base/vol_base.h>
#endif

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <memory>

#if defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L)
#define PIV_HAS_CPP17
#endif

#ifdef PIV_HAS_CPP17
#define PIV_IF if constexpr
#define PIV_ELSE_IF else if constexpr
#else
#define PIV_IF if
#define PIV_ELSE_IF else if
#endif

// 向前移动指针N个字节
#define PIV_PTR_FORWARD(cast, ptr, offset) ((cast)((uint64_t)(ptr) + (int64_t)(offset)))
// 向后移动指针N个字节
#define PIV_PTR_BACKWARD(cast, ptr, offset) ((cast)((uint64_t)(ptr) - (int64_t)(offset)))

namespace piv
{
    namespace detail
    {
    } // namespace detail


} // namespace piv

#endif // _PIV_BASE_HPP
