/*********************************************\
 * 火山视窗-PIV模块                          *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

// 包含火山视窗基本类,它在火山里的包含顺序比较靠前(全局-110)
#ifndef __VOL_BASE_H__
#include <sys/base/libs/win_base/vol_base.h>
#endif

#include <cstddef>
#include <cstdint>
#include <cwctype>
#include <cstdlib>
#include <type_traits>
#include <algorithm>
#include <memory>

#ifndef _PIV_BASE_HPP
#define _PIV_BASE_HPP

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

#ifndef _PIV_BUFFER_CLASS
#define _PIV_BUFFER_CLASS
template <typename ELEMENT_T = BYTE, typename MEM_SIZE = DWORD>
class PivBuffer
{
private:
    ELEMENT_T *p = nullptr;
    MEM_SIZE size = 0;

public:
    PivBuffer() {}
    ~PivBuffer() { Free(); }

    /**
     * @brief 带参构造
     * @param npSize 成员数
     * @param zero 是否清零
     */
    PivBuffer(MEM_SIZE npSize, bool zero = true) { Alloc(npSize, zero); }

    /**
     * @brief 分配缓存
     * @param npSize 成员数
     * @param zero 是否清零
     * @return 缓存指针
     */
    ELEMENT_T *Alloc(MEM_SIZE npSize, bool zero = true)
    {
        size = npSize * sizeof(ELEMENT_T);
        if (!p)
            p = reinterpret_cast<ELEMENT_T *>(g_objVolApp.GetPoolMem()->Alloc(static_cast<INT_P>(size)));
        else
            p = reinterpret_cast<ELEMENT_T *>(g_objVolApp.GetPoolMem()->Realloc((void *)p, static_cast<INT_P>(size)));
        if (zero)
            memset(p, 0, static_cast<size_t>(size));
        return p;
    }

    /**
     * @brief 重分配缓存
     * @param npNewSize 新成员数
     * @param zero 是否清零
     * @return 缓存指针
     */
    inline ELEMENT_T *Realloc(MEM_SIZE npNewSize, bool zero = true)
    {
        size = npNewSize * sizeof(ELEMENT_T);
        p = reinterpret_cast<ELEMENT_T *>(g_objVolApp.GetPoolMem()->Realloc((void *)p, static_cast<INT_P>(size)));
        if (zero)
            memset(p, 0, static_cast<size_t>(size));
        return p;
    }

    /**
     * @brief 释放缓存
     */
    inline void Free()
    {
        if (p)
            g_objVolApp.GetPoolMem()->Free((void *)p);
        size = 0;
    }

    /**
     * @brief 取指针
     * @return
     */
    inline ELEMENT_T *GetPtr() { return p; }

    /**
     * @brief 取指定类型指针
     * @tparam R 数据类型
     * @return
     */
    template <typename R = ELEMENT_T>
    inline R *Get() { return reinterpret_cast<R *>(p); }

    /**
     * @brief 取字节长度
     * @return
     */
    inline MEM_SIZE GetSize() { return size; }

    /**
     * @brief 取成员数
     * @return
     */
    inline MEM_SIZE GetCount() { return size / sizeof(ELEMENT_T); }

    /**
     * @brief 取成员
     * @param pos 索引
     * @return
     */
    ELEMENT_T &At(MEM_SIZE pos) { return p[pos]; }

    ELEMENT_T *operator*() { return p; }
    ELEMENT_T &operator[](MEM_SIZE pos) { return p[pos]; }
}; // PivBuffer
#endif // _PIV_BUFFER_CLASS

#ifndef _PIV_ASSERT_HPP
#define _PIV_ASSERT_HPP
#ifdef _DEBUG
#define PIV_ASSERT(t, _ERROR, _SOURCE) PivMsgAssert((t), (_ERROR), (_SOURCE))
static BOOL PivMsgAssert(const BOOL blpSucceed, const WCHAR *szErrorMessage, const char *szFileName)
{
    if (blpSucceed)
        return blpSucceed;
    CWString strOutput;
    const BOOL blpDebuggerPresent = ::IsDebuggerPresent();
    if (blpDebuggerPresent)
        strOutput.AddText(L"* \a");
    strOutput.AddFormatText(L"运行时校验失败(\"%s\"): %s", GetWideText(szFileName, CVolMem(), NULL), szErrorMessage);
    if (blpDebuggerPresent)
    {
        ::OutputDebugStringW(strOutput.GetText());
        __debugbreak();
    }
    else
    {
        ::MessageBox(NULL, strOutput.GetText(), L"运行时校验失败", (MB_ICONERROR | MB_OK));
        assert(FALSE);
    }
    return blpSucceed;
}
#else
#define PIV_ASSERT(t, _ERROR, _SOURCE)
#endif
#endif // _PIV_ASSERT_HPP

