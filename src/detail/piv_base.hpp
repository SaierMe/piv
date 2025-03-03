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
#include <cctype>
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

#if defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L)
#define PIV_HAS_CPP20
#endif

#ifdef PIV_HAS_CPP17
#define PIV_IF if constexpr
#define PIV_ELSE_IF else if constexpr
#else // PIV_HAS_CPP17
#define PIV_IF if
#define PIV_ELSE_IF else if
#endif // PIV_HAS_CPP17

// 向前移动指针N个字节
#define PIV_PTR_FORWARD(cast, ptr, offset) ((cast)((uint64_t)(ptr) + (int64_t)(offset)))
// 向后移动指针N个字节
#define PIV_PTR_BACKWARD(cast, ptr, offset) ((cast)((uint64_t)(ptr) - (int64_t)(offset)))

#endif // _PIV_BASE_HPP

#ifndef _PIV_BUFFER_CLASS
#define _PIV_BUFFER_CLASS
template <typename ELEMENT_T = BYTE, typename MEM_SIZE = DWORD>
class PivBuffer
{
private:
    ELEMENT_T *_p = nullptr;
    MEM_SIZE _size = 0;

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
        _size = npSize * sizeof(ELEMENT_T);
        if (!_p)
            _p = reinterpret_cast<ELEMENT_T *>(g_objVolApp.GetPoolMem()->Alloc(static_cast<INT_P>(_size)));
        else
            _p = reinterpret_cast<ELEMENT_T *>(g_objVolApp.GetPoolMem()->Realloc((void *)_p, static_cast<INT_P>(_size)));
        if (zero)
            memset(_p, 0, static_cast<size_t>(_size));
        return _p;
    }

    /**
     * @brief 重分配缓存
     * @param npNewSize 新成员数
     * @param zero 是否清零
     * @return 缓存指针
     */
    inline ELEMENT_T *Realloc(MEM_SIZE npNewSize, bool zero = true)
    {
        _size = npNewSize * sizeof(ELEMENT_T);
        _p = reinterpret_cast<ELEMENT_T *>(g_objVolApp.GetPoolMem()->Realloc((void *)_p, static_cast<INT_P>(_size)));
        if (zero)
            memset(_p, 0, static_cast<size_t>(_size));
        return _p;
    }

    /**
     * @brief 释放缓存
     */
    inline void Free()
    {
        if (_p)
            g_objVolApp.GetPoolMem()->Free((void *)_p);
        _size = 0;
    }

    /**
     * @brief 取指针
     * @return
     */
    inline ELEMENT_T *GetPtr() { return _p; }

    /**
     * @brief 取指定类型指针
     * @tparam R 数据类型
     * @return
     */
    template <typename R = ELEMENT_T>
    inline R *Get() { return reinterpret_cast<R *>(_p); }

    /**
     * @brief 取字节长度
     * @return
     */
    inline MEM_SIZE GetSize() { return _size; }

    /**
     * @brief 取成员数
     * @return
     */
    inline MEM_SIZE GetCount() { return _size / sizeof(ELEMENT_T); }

    /**
     * @brief 取成员
     * @param pos 索引
     * @return
     */
    ELEMENT_T &At(MEM_SIZE pos) { return _p[pos]; }

    ELEMENT_T *operator*() { return _p; }
    ELEMENT_T &operator[](MEM_SIZE pos) { return _p[pos]; }
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
    CVolMem buf;
    strOutput.AddFormatText(L"运行时校验失败(\"%s\"): %s", GetWideText(szFileName, buf, NULL), szErrorMessage);
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

#ifndef _PIV_HASH_FUNCTION
#define _PIV_HASH_FUNCTION
namespace piv
{
    namespace detail
    {
        /**
         * @brief 取哈希值(与MSVC std::hash的算法相同)
         */
        static size_t hash(const unsigned char *_First, size_t _Count)
        {
#if defined(_WIN64)
            size_t _basis = 14695981039346656037ULL;
            size_t _prime = 1099511628211ULL;
#else
            size_t _basis = 2166136261U;
            size_t _prime = 16777619U;
#endif
            size_t _val = _basis;
            for (size_t _Idx = 0; _Idx < _Count; ++_Idx)
            {
                _val ^= static_cast<size_t>(_First[_Idx]);
                _val *= _prime;
            }
            return _val;
        }
    } // namespace detail

    /**
     * @brief 取哈希值基类(不区分大小写)
     * @tparam T
     */
    template <class T>
    struct ci_hash
    {
        size_t operator()(const T &_Keyval) const noexcept
        {
            return 0;
        }
    };

    /**
     * @brief 取文本型哈希值(不区分大小写)
     */
    template <>
    struct ci_hash<CVolString>
    {
        size_t operator()(const CVolString &_Keyval) const noexcept
        {
            CVolString val(_Keyval);
            val.MakeUpper();
            return ::piv::detail::hash((const unsigned char *)val.GetText(), val.GetLength() * 2);
        }
    };

    /**
     * @brief 小于比较基类(不区分大小写)
     * @tparam T
     */
    template <typename T>
    struct ci_less
    {
        bool operator()(const T &lhs, const T &rhs) const
        {
            return lhs < rhs;
        }
    };

    /**
     * @brief 文本型小于比较(不区分大小写)
     */
    template <>
    struct ci_less<CVolString>
    {
        bool operator()(const CVolString &lhs, const CVolString &rhs) const
        {
            return lhs.compare(rhs, false) < 0;
        }
    };

    /**
     * @brief 等于比较基类(不区分大小写)
     * @tparam T
     */
    template <class T>
    struct ci_equal_to
    {
        bool operator()(const T &lhs, const T &rhs) const
        {
            return lhs == rhs;
        }
    };

    /**
     * @brief 文本型等于比较(不区分大小写)
     */
    template <>
    struct ci_equal_to<CVolString>
    {
        bool operator()(const CVolString &lhs, const CVolString &rhs) const
        {
            return lhs.IIsEqual(rhs);
        }
    };

}

namespace std
{
    /**
     * @brief 文本型的std::hash
     */
    template <>
    struct hash<CVolString>
    {
        size_t operator()(const CVolString &_Keyval) const noexcept
        {
            return ::piv::detail::hash((const unsigned char *)_Keyval.GetText(), _Keyval.GetLength() * 2);
        }
    };

    /**
     * @brief 字节集类的std::hash
     */
    template <>
    struct hash<CVolMem>
    {
        size_t operator()(const CVolMem &_Keyval) const noexcept
        {
            return ::piv::detail::hash((const unsigned char *)_Keyval.GetPtr(), _Keyval.GetSize());
        }
    };
}
#endif // _PIV_HASH_FUNCTION
