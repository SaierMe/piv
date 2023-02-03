/*********************************************\
 * 火山视窗PIV模块 - 标准格式化类(FMT)       *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 版本: 2022/11/23                          *
\*********************************************/

#ifndef _PIV_FMT_H
#define _PIV_FMT_H

// 包含火山视窗基本类,它在火山里的包含顺序比较靠前(全局-110)
#ifndef __VOL_BASE_H__
#include <sys/base/libs/win_base/vol_base.h>
#endif

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
#define _CT(x)  CVolConstString (_T (x))
#endif

#endif // _PIV_FMT_H
