/*********************************************\
 * 火山视窗PIV模块 - 文本编码辅助            *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 版本: 2023/02/08                          *
\*********************************************/

#ifndef _PIV_ENCODING_HPP
#define _PIV_ENCODING_HPP

// 包含火山视窗基本类,它在火山里的包含顺序比较靠前(全局-110)
#ifndef __VOL_BASE_H__
#include <sys/base/libs/win_base/vol_base.h>
#endif

#include <algorithm>
#include <string>
#if _MSVC_LANG < 201703L
#include "string_view.hpp"
#else
#include <string_view>
#endif

// 判断是否开启了simdutf
#ifdef SIMDUTF_H
#define PIV_ENABLE_SIMDUTF
#endif

namespace piv
{
#if _MSVC_LANG < 201703L
    using nonstd::basic_string_view;
    using nonstd::string_view;
    using nonstd::wstring_view;
#else
    using std::basic_string_view;
    using std::string_view;
    using std::wstring_view;
#endif

    namespace encoding
    {
        /**
         * @brief ANSI转UTF-16LE
         * @param utf16str 转换后的字符串在此参数中返回
         * @param mbstr 转换前的字符串
         * @param mbslen 转换前的字符串字符长度,-1为自动获取
         */
        static void A2Wex(std::wstring &utf16str, const char *mbstr, const size_t mbslen = static_cast<size_t>(-1))
        {
            int utf16len = ::MultiByteToWideChar(CP_ACP, 0, mbstr, (mbslen == static_cast<size_t>(-1)) ? -1 : static_cast<int>(mbslen), NULL, 0);
            if (utf16len > 0)
            {
                if (mbslen == static_cast<size_t>(-1))
                    utf16len -= 1;
                utf16str.resize(utf16len, '\0');
                ::MultiByteToWideChar(CP_ACP, 0, mbstr, (mbslen == static_cast<size_t>(-1)) ? -1 : static_cast<int>(mbslen), const_cast<wchar_t *>(utf16str.data()), utf16len);
            }
            else
            {
                utf16str = L"";
            }
        }

        /**
         * @brief UTF-16LE转ANSI
         * @param mbstr 转换后的字符串在此参数中返回
         * @param utf16str 转换前的字符串
         * @param utf16len 转换前的字符串字符长度,-1为自动获取
         */
        static void W2Aex(std::string &mbstr, const wchar_t *utf16str, const size_t utf16len = static_cast<size_t>(-1))
        {
            int mbslen = ::WideCharToMultiByte(CP_ACP, 0, utf16str, (utf16len == static_cast<size_t>(-1)) ? -1 : static_cast<int>(utf16len), NULL, 0, NULL, NULL);
            if (mbslen > 0)
            {
                if (utf16len == static_cast<size_t>(-1))
                    mbslen -= 1;
                mbstr.resize(mbslen, '\0');
                ::WideCharToMultiByte(CP_ACP, 0, utf16str, (utf16len == static_cast<size_t>(-1)) ? -1 : static_cast<int>(utf16len), const_cast<char *>(mbstr.data()), mbslen, NULL, NULL);
            }
            else
            {
                mbstr = "";
            }
        }

        /**
         * @brief UTF-16LE转UTF-8
         * @param utf8str 转换后的字符串在此参数中返回
         * @param utf16str 转换前的字符串
         * @param utf16len 转换前的字符串字符长度,-1为自动获取
         */
        static void W2Uex(std::string &utf8str, const wchar_t *utf16str, const size_t utf16len = static_cast<size_t>(-1))
        {
#ifdef PIV_ENABLE_SIMDUTF
            size_t utf16words = (utf16len == static_cast<size_t>(-1)) ? wcslen(utf16str) : utf16len;
            size_t utf8words = simdutf::utf8_length_from_utf16le(reinterpret_cast<const char16_t *>(utf16str), utf16words);
            if (utf8words == 0)
            {
                utf8str = "";
            }
            else
            {
                utf8str.resize(utf8words, '\0');
                simdutf::convert_valid_utf16le_to_utf8(reinterpret_cast<const char16_t *>(utf16str), utf16words, const_cast<char *>(reinterpret_cast<const char *>(utf8str.data())));
            }
#else
            int utf8len = ::WideCharToMultiByte(CP_UTF8, 0, utf16str, (utf16len == static_cast<size_t>(-1)) ? -1 : static_cast<int>(utf16len), NULL, 0, NULL, NULL);
            if (utf8len > 0)
            {
                if (utf16len == static_cast<size_t>(-1))
                    utf8len -= 1;
                utf8str.resize(utf8len, '\0');
                ::WideCharToMultiByte(CP_UTF8, 0, utf16str, (utf16len == static_cast<size_t>(-1)) ? -1 : static_cast<int>(utf16len), const_cast<char *>(utf8str.data()), utf8len, NULL, NULL);
            }
            else
            {
                utf8str = "";
            }
#endif
        }

        /**
         * @brief UTF-8转UTF-16LE
         * @param utf16str 转换后的字符串在此参数中返回
         * @param utf8str 转换前的字符串
         * @param utf8len 转换前的字符串字符长度,-1为自动获取
         */
        static void U2Wex(std::wstring &utf16str, const char *utf8str, const size_t utf8len = static_cast<size_t>(-1))
        {
#ifdef PIV_ENABLE_SIMDUTF
            size_t utf8words = (utf8len == static_cast<size_t>(-1)) ? strlen(utf8str) : utf8len;
            size_t utf16words = simdutf::utf16_length_from_utf8(utf8str, utf8words);
            if (utf16words == 0)
            {
                utf16str = L"";
            }
            else
            {
                utf16str.resize(utf16words, '\0');
                utf16words = simdutf::convert_valid_utf8_to_utf16le(utf8str, utf8words, reinterpret_cast<char16_t *>(const_cast<wchar_t *>(utf16str.data())));
            }
#else
            int utf16len = ::MultiByteToWideChar(CP_UTF8, 0, utf8str, (utf8len == static_cast<size_t>(-1)) ? -1 : static_cast<int>(utf8len), NULL, 0);
            if (utf16len > 0)
            {
                if (utf8len == static_cast<size_t>(-1))
                    utf16len -= 1;
                utf16str.resize(utf16len, '\0');
                ::MultiByteToWideChar(CP_UTF8, 0, utf8str, (utf8len == static_cast<size_t>(-1)) ? -1 : static_cast<int>(utf8len), const_cast<wchar_t *>(utf16str.data()), utf16len);
            }
            else
            {
                utf16str = L"";
            }
#endif
        }

        /**
         * @brief UTF-8转ANSI
         * @param mbstr 转换后的字符串在此参数中返回
         * @param utf8str 转换前的字符串
         * @param utf8len 转换前的字符串字符长度,-1为自动获取
         */
        static void U2Aex(std::string &mbstr, const char *utf8str, const size_t utf8len = static_cast<size_t>(-1))
        {
            std::wstring utf16str;
            piv::encoding::U2Wex(utf16str, utf8str, utf8len);
            piv::encoding::W2Aex(mbstr, utf16str.c_str(), 0);
        }

        /**
         * @brief ANSI转UTF-8
         * @param utf8str 转换后的字符串在此参数中返回
         * @param mbstr 转换前的字符串
         * @param mbslen 转换前的字符串字符长度,-1为自动获取
         */
        static void A2Uex(std::string &utf8str, const char *mbstr, const size_t mbslen = static_cast<size_t>(-1))
        {
            std::wstring utf16str;
            piv::encoding::A2Wex(utf16str, mbstr, mbslen);
            piv::encoding::W2Uex(utf8str, utf16str.c_str(), 0);
        }

        /**
         * @brief 获取URL编码后的长度
         * @param lpszSrc 所欲编码的数据指针
         * @param SrcLen 所欲编码的数据长度
         * @return
         */
        static ptrdiff_t GuessUrlEncodeBound(const BYTE *lpszSrc, const ptrdiff_t SrcLen)
        {
            ptrdiff_t Add = 0;
            for (ptrdiff_t i = 0; i < SrcLen; i++)
            {
                BYTE c = lpszSrc[i];
                if (!(isalnum(c) || c == ' ' || c == '.' || c == '-' || c == '_' || c == '*'))
                    Add += 2;
            }
            return SrcLen + Add;
        }

        /**
         * @brief 获取URL解码后的长度
         * @param lpszSrc 所欲解码的数据指针
         * @param SrcLen 所欲解码的数据长度
         * @return
         */
        static ptrdiff_t GuessUrlDecodeBound(const BYTE *lpszSrc, const ptrdiff_t SrcLen)
        {
            ptrdiff_t Percent = 0;
            for (ptrdiff_t i = 0; i < SrcLen; i++)
            {
                if (lpszSrc[i] == '%')
                {
                    ++Percent;
                    i += 2;
                }
            }
            return (SrcLen < Percent * 2) ? 0 : (SrcLen - Percent * 2);
        }

        /**
         * @brief URL编码
         * @param lpszSrc 所欲编码的数据指针
         * @param SrcLen 所欲编码的数据长度
         * @param lpszDest 保存编码后数据的指针
         * @param DestLen 编码后数据的长度
         * @return
         */
        static int32_t UrlEncode(const BYTE *lpszSrc, const ptrdiff_t SrcLen, BYTE *lpszDest, ptrdiff_t &DestLen)
        {
            if (lpszDest == nullptr || DestLen == 0)
                goto ERROR_DEST_LEN;
            BYTE c, n;
            ptrdiff_t j = 0;
            for (ptrdiff_t i = 0; i < SrcLen; i++)
            {
                if (j >= DestLen)
                    goto ERROR_DEST_LEN;
                c = lpszSrc[i];
                if (isalnum(c) || c == '.' || c == '-' || c == '_' || c == '*')
                    lpszDest[j++] = c;
                else if (c == ' ')
                    lpszDest[j++] = '+';
                else
                {
                    if (j + 3 > DestLen)
                        goto ERROR_DEST_LEN;
                    lpszDest[j++] = '%';
                    n = c >> 4;
                    *(lpszDest + j) = n <= 9 ? n + '0' : (n <= 'F' ? n + 'A' - 0X0A : n + 'a' - 0X0A);
                    n = c & 0X0F;
                    *((lpszDest + j) + 1) = n <= 9 ? n + '0' : (n <= 'F' ? n + 'A' - 0X0A : n + 'a' - 0X0A);
                    j += 2;
                }
            }
            if (DestLen > j)
            {
                lpszDest[j] = 0;
                DestLen = j;
            }
            return 0;
        ERROR_DEST_LEN:
            DestLen = GuessUrlEncodeBound(lpszSrc, SrcLen);
            return -5;
        }

        /**
         * @brief URL解码
         * @param lpszSrc 所欲解码的数据指针
         * @param SrcLen 所欲解码的数据长度
         * @param lpszDest 保存解码后数据的指针
         * @param DestLen 解码后数据的长度
         * @return
         */
        static int32_t UrlDecode(const BYTE *lpszSrc, const ptrdiff_t SrcLen, BYTE *lpszDest, ptrdiff_t &DestLen)
        {
            if (lpszDest == nullptr || DestLen == 0)
                goto ERROR_DEST_LEN;
            BYTE c, n;
            ptrdiff_t j = 0;
            for (ptrdiff_t i = 0; i < SrcLen; i++)
            {
                if (j >= DestLen)
                    goto ERROR_DEST_LEN;
                c = lpszSrc[i];
                if (c == '+')
                    lpszDest[j++] = ' ';
                else if (c != '%')
                    lpszDest[j++] = c;
                else
                {
                    if (i + 2 >= SrcLen)
                        goto ERROR_SRC_DATA;
                    c = *(lpszSrc + i + 1);
                    n = *(lpszSrc + i + 2);
                    c = c <= '9' ? c - '0' : (c <= 'F' ? c - 'A' + 0x0A : c - 'a' + 0X0A);
                    n = n <= '9' ? n - '0' : (n <= 'F' ? n - 'A' + 0x0A : n - 'a' + 0X0A);
                    lpszDest[j++] = (c << 4) | n;
                    i += 2;
                }
            }
            if (DestLen > j)
            {
                lpszDest[j] = 0;
                DestLen = j;
            }
            return 0;
        ERROR_SRC_DATA:
            DestLen = 0;
            return -3;
        ERROR_DEST_LEN:
            DestLen = GuessUrlDecodeBound(lpszSrc, SrcLen);
            return -5;
        }
    } // namespace encoding

} // namespace piv

/**
 * @brief ANSI编码转UTF-16LE编码的封装类
 */
class PivA2W
{
private:
    std::wstring utf16str;

public:
    /**
     * @brief 默认构造与析构函数
     */
    PivA2W() {}
    ~PivA2W() {}

    /**
     * @brief 默认复制构造函数
     * @param s 所欲复制的对象
     */
    PivA2W(const PivA2W &s)
    {
        utf16str = s.utf16str;
    }
    PivA2W(PivA2W &&s)
    {
        utf16str = std::move(s.utf16str);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivA2W(const char *mbstr, const size_t mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr, mbslen);
    }

    /**
     * @brief 构造的同时将提供的std::string文本转换
     * @param mbstr 所欲转换的文本
     */
    PivA2W(const std::string &mbstr)
    {
        Convert(mbstr.c_str());
    }

    operator const wchar_t *()
    {
        return utf16str.c_str();
    }
    operator const std::wstring &()
    {
        return utf16str;
    }
    operator std::wstring &()
    {
        return utf16str;
    }
    operator CVolString()
    {
        return CVolString(utf16str.c_str());
    }
    operator CVolMem()
    {
        return CVolMem(utf16str.c_str(), utf16str.size() * 2);
    }
    PivA2W &operator=(const PivA2W &_PivA2W)
    {
        utf16str = _PivA2W.utf16str;
        return *this;
    }
    PivA2W &operator=(PivA2W &&_PivA2W)
    {
        utf16str = std::move(_PivA2W.utf16str);
        return *this;
    }
    PivA2W &operator=(const char *mbstr)
    {
        Convert(mbstr);
        return *this;
    }
    bool operator==(const PivA2W &_PivA2W)
    {
        return utf16str == _PivA2W.utf16str;
    }
    std::wstring *operator->()
    {
        return &utf16str;
    }
    std::wstring &operator*()
    {
        return utf16str;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的ANSI文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    void Convert(const char *mbstr, const size_t mbslen = static_cast<size_t>(-1))
    {
        piv::encoding::A2Wex(utf16str, mbstr, mbslen);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    const wchar_t *GetText()
    {
        return utf16str.c_str();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    size_t GetSize() const
    {
        return utf16str.size();
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    wchar_t *GetPtr()
    {
        return const_cast<wchar_t *>(utf16str.data());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    bool IsEmpty()
    {
        return utf16str.empty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    std::wstring &String()
    {
        return utf16str;
    }

    /**
     * @brief 生成一个火山的文本型,其中包含了转换后的文本数据
     * @return 返回所转换的文本型
     */
    CVolString ToStr()
    {
        return CVolString(utf16str.c_str());
    }

    /**
     * @brief 将转换后的文本数据复制火山的文本型
     * @param str 文本数据将复制到此文本型变量中
     */
    void GetStr(CVolString &str)
    {
        str.SetText(utf16str.c_str());
    }

    /**
     * @brief 生成一个字节集类,其中包含了转换后的文本数据
     * @param null_char 字节集是否带结尾0字符
     * @return 返回所转换的字节集
     */
    CVolMem ToMem(const bool null_char = false)
    {
        return CVolMem(utf16str.c_str(), (utf16str.size() + null_char ? 1 : 0) * 2);
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    void GetMem(CVolMem &mem, const bool null_char = false)
    {
        mem.Empty();
        mem.Append(utf16str.c_str(), (utf16str.size() + null_char ? 1 : 0) * 2);
    }
}; // PivA2W

/**
 * @brief UTF-16LE编码转ANSI编码的封装类
 */
class PivW2A
{
private:
    std::string mbstr;

public:
    /**
     * @brief 默认构造与析构函数
     */
    PivW2A() {}
    ~PivW2A() {}

    /**
     * @brief 默认复制构造函数
     * @param s 所欲复制的对象
     */
    PivW2A(const PivW2A &s)
    {
        mbstr = s.mbstr;
    }
    PivW2A(PivW2A &&s)
    {
        mbstr = std::move(s.mbstr);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param utf16str 所欲转换的UTF-16LE文本
     * @param utf16len 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivW2A(const wchar_t *utf16str, const size_t utf16len = static_cast<size_t>(-1))
    {
        Convert(utf16str, utf16len);
    }
    PivW2A(const std::wstring &utf16str)
    {
        Convert(utf16str.c_str());
    }
    PivW2A(const CVolString &utf16str)
    {
        Convert(utf16str.GetText());
    }

    operator const char *()
    {
        return mbstr.c_str();
    }
    operator const std::string &()
    {
        return mbstr;
    }
    operator std::string &()
    {
        return mbstr;
    }
    operator CVolMem()
    {
        return CVolMem(mbstr.c_str(), mbstr.size());
    }
    PivW2A &operator=(const PivW2A &_PivW2A)
    {
        mbstr = _PivW2A.mbstr;
        return *this;
    }
    PivW2A &operator=(PivW2A &&_PivW2A)
    {
        mbstr = std::move(_PivW2A.mbstr);
        return *this;
    }
    PivW2A &operator=(const wchar_t *utf16str)
    {
        Convert(utf16str);
        return *this;
    }
    bool operator==(const PivW2A &_PivW2A)
    {
        return mbstr == _PivW2A.mbstr;
    }
    std::string *operator->()
    {
        return &mbstr;
    }
    std::string &operator*()
    {
        return mbstr;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    void Convert(const wchar_t *utf16str, const size_t utf16len = static_cast<size_t>(-1))
    {
        piv::encoding::W2Aex(mbstr, utf16str, utf16len);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    const char *GetText()
    {
        return mbstr.c_str();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    size_t GetSize() const
    {
        return mbstr.size();
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    char *GetPtr()
    {
        return const_cast<char *>(mbstr.data());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    bool IsEmpty()
    {
        return mbstr.empty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    std::string &String()
    {
        return mbstr;
    }

    /**
     * @brief 生成一个字节集类,其中包含了转换后的文本数据
     * @param null_char 字节集是否带结尾0字符
     * @return 返回所转换的字节集
     */
    CVolMem ToMem(const bool null_char = false)
    {
        return CVolMem(mbstr.c_str(), mbstr.size() + null_char ? 1 : 0);
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    void GetMem(CVolMem &mem, const bool null_char = false)
    {
        mem.Empty();
        mem.Append(mbstr.c_str(), mbstr.size() + null_char ? 1 : 0);
    }
}; // PivW2A

/**
 * @brief UTF-16LE编码转UTF-8编码的封装类
 */
class PivW2U
{
private:
    std::string utf8str;

public:
    /**
     * @brief 默认构造与析构函数
     */
    PivW2U() {}
    ~PivW2U() {}

    /**
     * @brief 默认复制构造函数
     * @param s 所欲复制的对象
     */
    PivW2U(const PivW2U &s)
    {
        utf8str = s.utf8str;
    }
    PivW2U(PivW2U &&s)
    {
        utf8str = std::move(s.utf8str);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param utf16str 所欲转换的UTF-16LE文本
     * @param utf16len 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivW2U(const wchar_t *utf16str, const size_t utf16len = static_cast<size_t>(-1))
    {
        Convert(utf16str, utf16len);
    }
    PivW2U(const std::wstring &utf16str)
    {
        Convert(utf16str.c_str());
    }
    PivW2U(const CVolString &utf16str)
    {
        Convert(utf16str.GetText());
    }

    operator CVolMem()
    {
        return CVolMem(utf8str.c_str(), utf8str.size());
    }
    operator const char *()
    {
        return utf8str.c_str();
    }
    operator const std::string &()
    {
        return utf8str;
    }
    operator std::string &()
    {
        return utf8str;
    }
    PivW2U &operator=(const PivW2U &_PivW2U)
    {
        utf8str = _PivW2U.utf8str;
        return *this;
    }
    PivW2U &operator=(PivW2U &&_PivW2U)
    {
        utf8str = std::move(_PivW2U.utf8str);
        return *this;
    }
    PivW2U &operator=(const wchar_t *utf16str)
    {
        Convert(utf16str);
        return *this;
    }
    bool operator==(const PivW2U &_PivW2U)
    {
        return utf8str == _PivW2U.utf8str;
    }
    std::string *operator->()
    {
        return &utf8str;
    }
    std::string &operator*()
    {
        return utf8str;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    void Convert(const wchar_t *utf16str, const size_t utf16len = static_cast<size_t>(-1))
    {
        piv::encoding::W2Uex(utf8str, utf16str, utf16len);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    const char *GetText()
    {
        return utf8str.c_str();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    size_t GetSize() const
    {
        return utf8str.size();
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    char *GetPtr()
    {
        return const_cast<char *>(utf8str.data());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    bool IsEmpty()
    {
        return utf8str.empty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    std::string &String()
    {
        return utf8str;
    }

    /**
     * @brief 生成一个字节集类,其中包含了转换后的文本数据
     * @param null_char 字节集是否带结尾0字符
     * @return 返回所转换的字节集
     */
    CVolMem ToMem(const bool null_char = false)
    {
        return CVolMem(utf8str.c_str(), utf8str.size() + null_char ? 1 : 0);
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    void GetMem(CVolMem &mem, const bool null_char = false)
    {
        mem.Empty();
        mem.Append(utf8str.c_str(), utf8str.size() + null_char ? 1 : 0);
    }
}; // PivW2U

/**
 * @brief UTF-8编码转UTF-16LE编码的封装类
 */
class PivU2W
{
private:
    std::wstring utf16str;

public:
    /**
     * @brief 默认构造与析构函数
     */
    PivU2W() {}
    ~PivU2W() {}

    /**
     * @brief 默认复制构造函数
     * @param s 所欲复制的对象
     */
    PivU2W(const PivU2W &s)
    {
        utf16str = s.utf16str;
    }
    PivU2W(PivU2W &&s)
    {
        utf16str = std::move(s.utf16str);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param utf8str 所欲转换的UTF-8文本
     * @param utf8len 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivU2W(const char *utf8str, const size_t utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str, utf8len);
    }

    /**
     * @brief 构造的同时将提供的std::string文本转换
     * @param utf8str 所欲转换的文本
     */
    PivU2W(const std::string &utf8str)
    {
        Convert(utf8str.c_str());
    }

    operator const wchar_t *()
    {
        return utf16str.c_str();
    }
    operator const std::wstring &()
    {
        return utf16str;
    }
    operator std::wstring &()
    {
        return utf16str;
    }
    operator CVolString()
    {
        return CVolString(utf16str.c_str());
    }
    operator CVolMem()
    {
        return CVolMem(utf16str.c_str(), utf16str.size() * 2);
    }
    PivU2W &operator=(const PivU2W &_PivU2W)
    {
        utf16str = _PivU2W.utf16str;
        return *this;
    }
    PivU2W &operator=(PivU2W &&_PivU2W)
    {
        utf16str = std::move(_PivU2W.utf16str);
        return *this;
    }
    PivU2W &operator=(const char *utf8str)
    {
        Convert(utf8str);
        return *this;
    }
    bool operator==(const PivU2W &_PivU2W)
    {
        return utf16str == _PivU2W.utf16str;
    }
    std::wstring *operator->()
    {
        return &utf16str;
    }
    std::wstring &operator*()
    {
        return utf16str;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    void Convert(const char *utf8str, const size_t utf8len = static_cast<size_t>(-1))
    {
        piv::encoding::U2Wex(utf16str, utf8str, utf8len);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    const wchar_t *GetText()
    {
        return utf16str.c_str();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    size_t GetSize() const
    {
        return utf16str.size();
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    wchar_t *GetPtr()
    {
        return const_cast<wchar_t *>(utf16str.data());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    bool IsEmpty()
    {
        return utf16str.empty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    std::wstring &String()
    {
        return utf16str;
    }

    /**
     * @brief 生成一个火山的文本型,其中包含了转换后的文本数据
     * @return 返回所转换的文本型
     */
    CVolString ToStr()
    {
        return CVolString(utf16str.c_str());
    }

    /**
     * @brief 将转换后的文本数据复制火山的文本型
     * @param str 文本数据将复制到此文本型变量中
     */
    void GetStr(CVolString &str)
    {
        str.SetText(utf16str.c_str());
    }

    /**
     * @brief 生成一个字节集类,其中包含了转换后的文本数据
     * @param null_char 字节集是否带结尾0字符
     * @return 返回所转换的字节集
     */
    CVolMem ToMem(const bool null_char = false)
    {
        return CVolMem(utf16str.c_str(), (utf16str.size() + null_char ? 1 : 0) * 2);
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    void GetMem(CVolMem &mem, const bool null_char = false)
    {
        mem.Empty();
        mem.Append(utf16str.c_str(), (utf16str.size() + null_char ? 1 : 0) * 2);
    }
}; // PivU2W

/**
 * @brief UTF-8编码转ANSI编码的封装类
 */
class PivU2A
{
private:
    std::string mbstr;

public:
    /**
     * @brief 默认构造与析构函数
     */
    PivU2A() {}
    ~PivU2A() {}

    /**
     * @brief 默认复制构造函数
     * @param s 所欲复制的对象
     */
    PivU2A(const PivU2A &s)
    {
        mbstr = s.mbstr;
    }
    PivU2A(PivU2A &&s)
    {
        mbstr = std::move(s.mbstr);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param utf8str 所欲转换的UTF-8文本
     * @param utf8len 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivU2A(const char *utf8str, const size_t utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str, utf8len);
    }

    /**
     * @brief 构造的同时将提供的std::string文本转换
     * @param utf8str 所欲转换的文本
     */
    PivU2A(const std::string &utf8str)
    {
        Convert(utf8str.c_str());
    }

    operator const char *()
    {
        return mbstr.c_str();
    }
    operator const std::string &()
    {
        return mbstr;
    }
    operator CVolMem()
    {
        return CVolMem(mbstr.c_str(), mbstr.size());
    }
    PivU2A &operator=(const PivU2A &_PivU2A)
    {
        mbstr = _PivU2A.mbstr;
        return *this;
    }
    PivU2A &operator=(PivU2A &&_PivU2A)
    {
        mbstr = std::move(_PivU2A.mbstr);
        return *this;
    }
    PivU2A &operator=(const char *utf8str)
    {
        Convert(utf8str);
        return *this;
    }
    bool operator==(const PivU2A &_PivU2A)
    {
        return mbstr == _PivU2A.mbstr;
    }
    std::string *operator->()
    {
        return &mbstr;
    }
    std::string &operator*()
    {
        return mbstr;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    void Convert(const char *utf8str, const size_t utf8len = static_cast<size_t>(-1))
    {
        piv::encoding::U2Aex(mbstr, utf8str, utf8len);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    const char *GetText()
    {
        return mbstr.c_str();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    size_t GetSize() const
    {
        return mbstr.size();
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    char *GetPtr()
    {
        return const_cast<char *>(mbstr.data());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    bool IsEmpty()
    {
        return mbstr.empty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    std::string &string()
    {
        return mbstr;
    }

    /**
     * @brief 生成一个字节集类,其中包含了转换后的文本数据
     * @param null_char 字节集是否带结尾0字符
     * @return 返回所转换的字节集
     */
    CVolMem ToMem(const bool null_char = false)
    {
        return CVolMem(mbstr.c_str(), mbstr.size() + null_char ? 1 : 0);
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    void GetMem(CVolMem &mem, const bool null_char = false)
    {
        mem.Empty();
        mem.Append(mbstr.c_str(), mbstr.size() + null_char ? 1 : 0);
    }
}; // PivU2A

/**
 * @brief ANSI编码转UTF-8编码的封装类
 */
class PivA2U
{
private:
    std::string utf8str;

public:
    /**
     * @brief 默认构造与析构函数
     */
    PivA2U() {}
    ~PivA2U() {}

    /**
     * @brief 默认复制构造函数
     * @param s 所欲复制的对象
     */
    PivA2U(const PivA2U &s)
    {
        utf8str = s.utf8str;
    }
    PivA2U(PivA2U &&s)
    {
        utf8str = std::move(s.utf8str);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param mbstr 所欲转换的ANSI文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivA2U(const char *mbstr, const size_t mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr, mbslen);
    }

    /**
     * @brief 构造的同时将提供的std::string文本转换
     * @param mbstr 所欲转换的文本
     */
    PivA2U(const std::string &mbstr)
    {
        Convert(mbstr.c_str());
    }

    operator const char *()
    {
        return utf8str.c_str();
    }
    operator const std::string &()
    {
        return utf8str;
    }
    operator CVolMem()
    {
        return CVolMem(utf8str.c_str(), utf8str.size());
    }
    PivA2U &operator=(const PivA2U &_PivA2U)
    {
        utf8str = _PivA2U.utf8str;
        return *this;
    }
    PivA2U &operator=(PivA2U &&_PivA2U)
    {
        utf8str = std::move(_PivA2U.utf8str);
        return *this;
    }
    PivA2U &operator=(const char *mbstr)
    {
        Convert(mbstr);
        return *this;
    }
    bool operator==(const PivA2U &_PivA2U)
    {
        return utf8str == _PivA2U.utf8str;
    }
    std::string *operator->()
    {
        return &utf8str;
    }
    std::string &operator*()
    {
        return utf8str;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    void Convert(const char *mbstr, const size_t mbslen = static_cast<size_t>(-1))
    {
        piv::encoding::A2Uex(utf8str, mbstr, mbslen);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    const char *GetText()
    {
        return utf8str.c_str();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    size_t GetSize() const
    {
        return utf8str.size();
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    char *GetPtr()
    {
        return const_cast<char *>(utf8str.data());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    bool IsEmpty()
    {
        return utf8str.empty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    std::string &String()
    {
        return utf8str;
    }

    /**
     * @brief 生成一个字节集类,其中包含了转换后的文本数据
     * @param null_char 字节集是否带结尾0字符
     * @return 返回所转换的字节集
     */
    CVolMem ToMem(const bool null_char = false)
    {
        return CVolMem(utf8str.c_str(), utf8str.size() + null_char ? 1 : 0);
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    void GetMem(CVolMem &mem, const bool null_char = false)
    {
        mem.Empty();
        mem.Append(utf8str.c_str(), utf8str.size() + null_char ? 1 : 0);
    }
}; // PivA2U

namespace piv
{
    namespace encoding
    {
        /**
         * @brief URL文本解码
         * @param str 所欲解码的文本
         * @param decoded 返回解码后的文本
         * @param urlDecode 是否进行URL解码
         * @param utf8 是否为UTF-8编码
         */
        template <typename CharT>
        void UrlStrDecode(const basic_string_view<CharT> &str, std::basic_string<CharT> &decoded, const bool urlDecode = true, const bool utf8 = true)
        {
            if (str.size() == 0)
            {
                decoded.clear();
                return;
            }
            if (urlDecode)
            {
                ptrdiff_t buffer_len;
                if (sizeof(CharT) == 2)
                {
                    if (utf8)
                    {
                        PivW2U urlstr(reinterpret_cast<const wchar_t *>(str.data()), str.size());
                        buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const BYTE *>(urlstr.GetPtr()), urlstr.GetSize());
                        if (buffer_len > 0 && buffer_len < static_cast<ptrdiff_t>(urlstr.GetSize()))
                        {
                            std::string buffer;
                            buffer.resize(static_cast<size_t>(buffer_len));
                            piv::encoding::UrlDecode(reinterpret_cast<const BYTE *>(urlstr.GetPtr()), urlstr.GetSize(), const_cast<BYTE *>(reinterpret_cast<const BYTE *>(buffer.data())), buffer_len);
                            decoded.assign(reinterpret_cast<const CharT *>(PivU2W{buffer.c_str()}.GetText()));
                            return;
                        }
                    }
                    else
                    {
                        PivW2A urlstr(reinterpret_cast<const wchar_t *>(str.data()), str.size());
                        buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const BYTE *>(urlstr.GetPtr()), urlstr.GetSize());
                        if (buffer_len > 0 && buffer_len < static_cast<ptrdiff_t>(urlstr.GetSize()))
                        {
                            std::string buffer;
                            buffer.resize(static_cast<size_t>(buffer_len));
                            piv::encoding::UrlDecode(reinterpret_cast<const BYTE *>(urlstr.GetPtr()), urlstr.GetSize(), const_cast<BYTE *>(reinterpret_cast<const BYTE *>(buffer.data())), buffer_len);
                            decoded.assign(reinterpret_cast<const CharT *>(PivA2W{buffer.c_str()}.GetText()));
                            return;
                        }
                    }
                }
                else
                {
                    buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const BYTE *>(str.data()), str.size());
                    if (buffer_len > 0 && buffer_len < static_cast<ptrdiff_t>(str.size()))
                    {
                        decoded.resize(static_cast<size_t>(buffer_len));
                        piv::encoding::UrlDecode(reinterpret_cast<const BYTE *>(str.data()), str.size(), const_cast<BYTE *>(reinterpret_cast<const BYTE *>(decoded.data())), buffer_len);
                        return;
                    }
                }
            }
            decoded.assign(str.data(), str.size());
        }
        template <typename CharT>
        void UrlStrDecode(const std::basic_string<CharT> &str, std::basic_string<CharT> &decoded, const bool urlDecode = true, const bool utf8 = true)
        {
            piv::encoding::UrlStrDecode(basic_string_view<CharT>{str}, decoded, urlDecode, utf8);
        }
        template <typename CharT>
        std::basic_string<CharT> UrlStrDecode(const basic_string_view<CharT> &str, const bool urlDecode = true, const bool utf8 = true)
        {
            std::basic_string<CharT> decoded;
            piv::encoding::UrlStrDecode(str, decoded, urlDecode, utf8);
            return decoded;
        }
        template <typename CharT>
        std::basic_string<CharT> UrlStrDecode(const std::basic_string<CharT> &str, const bool urlDecode = true, const bool utf8 = true)
        {
            return piv::encoding::UrlStrDecode(basic_string_view<CharT>{str}, urlDecode, utf8);
        }

        template <typename CharT>
        void UrlStrDecode(const basic_string_view<CharT> &str, CVolMem &decoded, const bool utf8 = true)
        {
            decoded.Empty();
            if (str.size() == 0)
            {
                return;
            }
            ptrdiff_t buffer_len;
            if (sizeof(CharT) == 2)
            {
                if (utf8)
                {
                    PivW2U urlstr(reinterpret_cast<const wchar_t *>(str.data()), str.size());
                    buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const BYTE *>(urlstr.GetPtr()), urlstr.GetSize());
                    if (buffer_len > 0 && buffer_len < static_cast<ptrdiff_t>(urlstr.GetSize()))
                    {
                        std::string buffer;
                        buffer.resize(static_cast<size_t>(buffer_len));
                        piv::encoding::UrlDecode(reinterpret_cast<const BYTE *>(urlstr.GetPtr()), urlstr.GetSize(), const_cast<BYTE *>(reinterpret_cast<const BYTE *>(buffer.data())), buffer_len);
                        PivU2W{buffer.c_str()}.GetMem(decoded);
                        return;
                    }
                }
                else
                {
                    PivW2A urlstr(reinterpret_cast<const wchar_t *>(str.data()), str.size());
                    buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const BYTE *>(urlstr.GetPtr()), urlstr.GetSize());
                    if (buffer_len > 0 && buffer_len < static_cast<ptrdiff_t>(urlstr.GetSize()))
                    {
                        std::string buffer;
                        buffer.resize(static_cast<size_t>(buffer_len));
                        piv::encoding::UrlDecode(reinterpret_cast<const BYTE *>(urlstr.GetPtr()), urlstr.GetSize(), const_cast<BYTE *>(reinterpret_cast<const BYTE *>(buffer.data())), buffer_len);
                        PivA2W{buffer.c_str()}.GetMem(decoded);
                        return;
                    }
                }
            }
            else
            {
                buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const BYTE *>(str.data()), str.size());
                if (buffer_len > 0 && buffer_len < static_cast<ptrdiff_t>(str.size()))
                {
                    piv::encoding::UrlDecode(reinterpret_cast<const BYTE *>(str.data()), str.size(), decoded.Alloc(buffer_len, TRUE), buffer_len);
                    return;
                }
            }
            decoded.Append(reinterpret_cast<const void *>(str.data()), str.size() * sizeof(CharT));
        }
        template <typename CharT>
        void UrlStrDecode(const std::basic_string<CharT> &str, CVolMem &decoded, const bool utf8 = true)
        {
            piv::encoding::UrlStrDecode(basic_string_view<CharT>{str.c_str()}, decoded, utf8);
        }
        void UrlStrDecode(const CVolMem &str, CVolMem &decoded, const bool utf8 = true)
        {
            piv::encoding::UrlStrDecode(string_view(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())), decoded, utf8);
        }
        void UrlStrDecode(const CVolMem &str, CVolString &decoded, const bool utf8 = true)
        {
            CVolMem buffer;
            piv::encoding::UrlStrDecode(string_view(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())), buffer, utf8);
            if (utf8)
                PivU2W{reinterpret_cast<const char *>(buffer.GetPtr()), static_cast<size_t>(buffer.GetSize())}.GetStr(decoded);
            else
                PivA2W{reinterpret_cast<const char *>(buffer.GetPtr()), static_cast<size_t>(buffer.GetSize())}.GetStr(decoded);
        }
        CVolString UrlStrDecode(const CVolMem &str, const bool utf8 = true)
        {
            CVolMem buffer;
            piv::encoding::UrlStrDecode(string_view(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())), buffer, utf8);
            if (utf8)
                return PivU2W{reinterpret_cast<const char *>(buffer.GetPtr()), static_cast<size_t>(buffer.GetSize())}.ToStr();
            else
                return PivA2W{reinterpret_cast<const char *>(buffer.GetPtr()), static_cast<size_t>(buffer.GetSize())}.ToStr();
        }

        /**
         * @brief URL文本编码
         * @param str 所欲编码的文本
         * @param encoded 返回编码后的文本
         * @param utf8 是否为UTF-8编码
         */
        template <typename CharT>
        void UrlStrEncode(const basic_string_view<CharT> &str, std::basic_string<CharT> &encoded, const bool utf8 = true)
        {
            ptrdiff_t buffer_len;
            if (sizeof(CharT) == 2)
            {
                if (utf8)
                {
                    PivW2U text{reinterpret_cast<const wchar_t *>(str.data()), str.size()};
                    buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const BYTE *>(text.GetPtr()), static_cast<ptrdiff_t>(text.GetSize()));
                    if (buffer_len > static_cast<ptrdiff_t>(text.GetSize()))
                    {
                        CVolMem buffer;
                        piv::encoding::UrlEncode(reinterpret_cast<const BYTE *>(text.GetPtr()), static_cast<ptrdiff_t>(text.GetSize()), buffer.Alloc(buffer_len, TRUE), buffer_len);
                        PivU2W urled{reinterpret_cast<const char *>(buffer.GetPtr()), static_cast<size_t>(buffer_len)};
                        encoded.assign(reinterpret_cast<const CharT *>(urled.GetText()), urled.GetSize());
                        return;
                    }
                }
                else
                {
                    PivW2A text{reinterpret_cast<const wchar_t *>(str.data()), str.size()};
                    buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const BYTE *>(text.GetPtr()), static_cast<ptrdiff_t>(text.GetSize()));
                    if (buffer_len > static_cast<ptrdiff_t>(text.GetSize()))
                    {
                        CVolMem buffer;
                        piv::encoding::UrlEncode(reinterpret_cast<const BYTE *>(text.GetPtr()), static_cast<ptrdiff_t>(text.GetSize()), buffer.Alloc(buffer_len, TRUE), buffer_len);
                        PivA2W urled{reinterpret_cast<const char *>(buffer.GetPtr()), static_cast<size_t>(buffer_len)};
                        encoded.assign(reinterpret_cast<const CharT *>(urled.GetText()), urled.GetSize());
                        return;
                    }
                }
            }
            else
            {
                buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const BYTE *>(str.data()), static_cast<ptrdiff_t>(str.size()));
                if (buffer_len > static_cast<ptrdiff_t>(str.size()))
                {
                    encoded.resize(static_cast<size_t>(buffer_len));
                    piv::encoding::UrlEncode(reinterpret_cast<const BYTE *>(str.data()), static_cast<ptrdiff_t>(str.size()), const_cast<BYTE *>(reinterpret_cast<const BYTE *>(encoded.data())), buffer_len);
                    return;
                }
            }
            encoded.assign(str.data(), str.size());
        }
        template <typename CharT>
        void UrlStrEncode(const std::basic_string<CharT> &str, std::basic_string<CharT> &encoded, const bool utf8 = true)
        {
            piv::encoding::UrlStrEncode(basic_string_view<CharT>{str.c_str()}, encoded, utf8);
        }
        template <typename CharT>
        std::basic_string<CharT> UrlStrEncode(const basic_string_view<CharT> &str, const bool utf8 = true)
        {
            std::basic_string<CharT> encoded;
            piv::encoding::UrlStrEncode(str, encoded, utf8);
            return encoded;
        }
        template <typename CharT>
        std::basic_string<CharT> UrlStrEncode(const std::basic_string<CharT> &str, const bool utf8 = true)
        {
            std::basic_string<CharT> encoded;
            piv::encoding::UrlStrEncode(basic_string_view<CharT>{str.c_str()}, encoded, utf8);
            return encoded;
        }

        template <typename CharT>
        void UrlStrEncode(const basic_string_view<CharT> &str, CVolMem &encoded, const bool utf8 = true)
        {
            ptrdiff_t buffer_len;
            if (sizeof(CharT) == 2)
                if (utf8)
                {
                    PivW2U text{reinterpret_cast<const wchar_t *>(str.data()), str.size()};
                    buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const BYTE *>(text.GetPtr()), static_cast<ptrdiff_t>(text.GetSize()));
                    if (buffer_len > static_cast<ptrdiff_t>(text.GetSize()))
                    {
                        CVolMem buffer;
                        piv::encoding::UrlEncode(reinterpret_cast<const BYTE *>(text.GetPtr()), static_cast<ptrdiff_t>(text.GetSize()), buffer.Alloc(buffer_len, TRUE), buffer_len);
                        PivU2W{reinterpret_cast<const char *>(buffer.GetPtr()), static_cast<size_t>(buffer_len)}.GetMem(encoded);
                        return;
                    }
                }
                else
                {
                    PivW2A text{reinterpret_cast<const wchar_t *>(str.data()), str.size()};
                    buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const BYTE *>(text.GetPtr()), static_cast<ptrdiff_t>(text.GetSize()));
                    if (buffer_len > static_cast<ptrdiff_t>(text.GetSize()))
                    {
                        CVolMem buffer;
                        piv::encoding::UrlEncode(reinterpret_cast<const BYTE *>(text.GetPtr()), static_cast<ptrdiff_t>(text.GetSize()), buffer.Alloc(buffer_len, TRUE), buffer_len);
                        PivA2W{reinterpret_cast<const char *>(buffer.GetPtr()), static_cast<size_t>(buffer_len)}.GetMem(encoded);
                        return;
                    }
                }
            else
            {
                buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const BYTE *>(str.data()), static_cast<ptrdiff_t>(str.size()));
                if (buffer_len > static_cast<ptrdiff_t>(str.size()))
                {
                    piv::encoding::UrlEncode(reinterpret_cast<const BYTE *>(str.data()), static_cast<ptrdiff_t>(str.size()), encoded.Alloc(buffer_len, TRUE), buffer_len);
                    return;
                }
            }
            encoded.Empty();
            encoded.Append(reinterpret_cast<const void *>(str.data()), str.size());
        }
        template <typename CharT>
        void UrlStrEncode(const std::basic_string<CharT> &str, CVolMem &encoded, const bool utf8 = true)
        {
            piv::encoding::UrlStrEncode(std::basic_string<CharT>{str.c_str()}, encoded, utf8);
        }
        void UrlStrEncode(const CVolString &str, CVolMem &encoded, const bool utf8 = true)
        {
            piv::encoding::UrlStrEncode(wstring_view(str.GetText()), encoded, utf8);
        }
        void UrlStrEncode(const CVolMem &str, CVolMem &encoded)
        {
            piv::encoding::UrlStrEncode(string_view(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())), encoded);
        }
        void UrlStrEncode(const CVolMem &str, CVolString &encoded, const bool utf8 = true)
        {
            CVolMem buffer;
            piv::encoding::UrlStrEncode(string_view(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())), buffer);
            if (utf8)
                PivU2W{reinterpret_cast<const char *>(buffer.GetPtr()), static_cast<size_t>(buffer.GetSize())}.GetStr(encoded);
            else
                PivA2W{reinterpret_cast<const char *>(buffer.GetPtr()), static_cast<size_t>(buffer.GetSize())}.GetStr(encoded);
        }
        CVolString UrlStrEncode(const CVolMem &str, const bool utf8 = true)
        {
            CVolMem buffer;
            piv::encoding::UrlStrEncode(string_view(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())), buffer);
            if (utf8)
                return PivU2W{reinterpret_cast<const char *>(buffer.GetPtr()), static_cast<size_t>(buffer.GetSize())}.ToStr();
            else
                return PivA2W{reinterpret_cast<const char *>(buffer.GetPtr()), static_cast<size_t>(buffer.GetSize())}.ToStr();
        }
    } // namespace encoding
} // namespace piv

#endif // _PIV_ENCODING_HPP
