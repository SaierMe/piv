/*********************************************\
 * 火山视窗PIV模块 - 文本编码辅助            *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef _PIV_ENCODING_HPP
#define _PIV_ENCODING_HPP

#include "detail/piv_base.hpp"

#include <string>
#ifdef PIV_HAS_CPP17
#include <string_view>
#else
#include "detail/string_view.hpp"
#endif

namespace piv
{
#ifdef PIV_HAS_CPP17
    using std::basic_string_view;
    using std::string_view;
    using std::wstring_view;
#else
    using nonstd::basic_string_view;
    using nonstd::string_view;
    using nonstd::wstring_view;
#endif

    struct ansi
    {
        unsigned char bom[1] = {0};
    };
    struct utf8
    {
        unsigned char bom[3] = {0xEF, 0xBB, 0xBF};
    };
    struct utf16_le
    {
        unsigned char bom[2] = {0xFF, 0xFE};
    };
    struct utf16_be
    {
        unsigned char bom[2] = {0xFE, 0xFF};
    };
    struct utf32_le
    {
        unsigned char bom[4] = {0xFF, 0xFE, 0x00, 0x00};
    };
    struct utf32_be
    {
        unsigned char bom[4] = {0x00, 0x00, 0xFE, 0xFF};
    };

    /**
     * @brief 创建文本视图
     * @tparam CharT 字符类型
     * @param rhs 指向的文本数据
     * @param count 文本长度
     * @return 所创建的文本视图
     */
    template <typename CharT, typename T>
    ::piv::basic_string_view<CharT> make_sv(const T *rhs, size_t count = -1)
    {
        if (!std::is_same<CharT, T>::value)
            return ::piv::basic_string_view<CharT>{};
        if (count == -1)
            return ::piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(rhs)};
        else
            return ::piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(rhs), count};
    }

    template <typename CharT, typename T>
    ::piv::basic_string_view<CharT> make_sv(const std::basic_string<T> &rhs, size_t count = -1)
    {
        if (!std::is_same<CharT, T>::value)
            return ::piv::basic_string_view<CharT>{};
        if (count == -1)
            return ::piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(rhs.c_str())};
        else
            return ::piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(rhs.c_str()), count};
    }

    template <typename CharT, typename T>
    ::piv::basic_string_view<CharT> make_sv(const ::piv::basic_string_view<T> &rhs, size_t count = -1)
    {
        if (!std::is_same<CharT, T>::value)
            return ::piv::basic_string_view<CharT>{};
        if (count == -1)
            return ::piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(rhs.data())};
        else
            return ::piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(rhs.data()), count};
    }

    template <typename CharT>
    ::piv::basic_string_view<CharT> make_sv(const CWString &rhs, size_t count = -1)
    {
        if (!std::is_same<CharT, wchar_t>::value)
            return ::piv::basic_string_view<CharT>{};
        if (count == -1)
            return ::piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(rhs.GetText())};
        else
            return ::piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(rhs.GetText()), count};
    }

    template <typename CharT>
    ::piv::basic_string_view<CharT> make_sv(const CVolMem &rhs, size_t count = -1)
    {
        if (count == -1)
            return ::piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize()) / sizeof(CharT)};
        else
            return ::piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(rhs.GetTextPtr()), count};
    }

    namespace edit
    {
        /**
         * @brief push_back
         * @tparam CharT
         * @param lhs
         * @param c
         */
        template <typename CharT, typename T>
        static void push_back(std::basic_string<CharT> &lhs, T value)
        {
            lhs.push_back(static_cast<CharT>(value));
        }

        template <typename T>
        static void push_back(CWString &lhs, T value)
        {
            lhs.AddChar(static_cast<wchar_t>(value));
        }

        template <typename T>
        static void push_back(CU8String &lhs, T value)
        {
            lhs.AddChar(static_cast<char>(value));
        }

        template <typename T>
        static void push_back(CVolMem &lhs, T value)
        {
            lhs.Append(&value, sizeof(T));
        }

        /**
         * @brief clear
         * @tparam CharT
         * @param lhs
         */
        template <typename CharT>
        static void clear(std::basic_string<CharT> &lhs)
        {
            lhs.clear();
        }

        static void clear(CWString &lhs)
        {
            lhs.Empty();
        }

        static void clear(CU8String &lhs)
        {
            lhs.Empty();
        }

        static void clear(CVolMem &lhs)
        {
            lhs.Empty();
        }

        /**
         * @brief reserve
         * @tparam CharT
         * @param lhs
         * @param _size
         */
        template <typename CharT>
        static void reserve(std::basic_string<CharT> &lhs, size_t _size)
        {
            lhs.reserve(_size);
        }

        static void reserve(CWString &lhs, size_t _size)
        {
            lhs.SetNumAlignChars(static_cast<INT_P>(_size));
        }

        static void reserve(CU8String &lhs, size_t _size)
        {
            lhs.SetNumAlignChars(static_cast<INT_P>(_size));
        }

        static void reserve(CVolMem &lhs, size_t _size)
        {
            lhs.SetMemAlignSize(static_cast<INT_P>(_size));
        }

        /**
         * @brief 取字符大小
         * @tparam CharT
         * @param cstr
         * @return
         */
        template <typename CharT>
        static size_t size(const CharT *cstr)
        {
            PIV_IF(sizeof(CharT) == 2)
            {
                return wcslen(reinterpret_cast<const wchar_t *>(cstr));
            }
            else
            {
                return strlen(reinterpret_cast<const char *>(cstr));
            }
        }

        template <typename CharT>
        static size_t size(const std::basic_string<CharT> &lhs)
        {
            return lhs.size();
        }

        template <typename CharT>
        static size_t size(const ::piv::basic_string_view<CharT> &lhs)
        {
            return lhs.size();
        }

        static size_t size(const CWString &lhs)
        {
            return lhs.GetLength();
        }

        static size_t size(const CU8String &lhs)
        {
            return lhs.GetLength();
        }

        static size_t size(const CVolMem &lhs)
        {
            return lhs.GetSize();
        }

        /**
         * @brief 取字节大小
         * @tparam CharT
         * @param lhs
         * @return
         */
        template <typename CharT>
        static size_t size_bytes(const std::basic_string<CharT> &lhs)
        {
            return lhs.size() * sizeof(CharT);
        }

        template <typename CharT>
        static size_t size_bytes(const ::piv::basic_string_view<CharT> &lhs)
        {
            return lhs.size() * sizeof(CharT);
        }

        static size_t size_bytes(const CWString &lhs)
        {
            return static_cast<size_t>(lhs.GetLength() * 2);
        }

        static size_t size_bytes(const CU8String &lhs)
        {
            return static_cast<size_t>(lhs.GetLength());
        }

        static size_t size_bytes(const CVolMem &lhs)
        {
            return static_cast<size_t>(lhs.GetSize());
        }

    } // namespace edit

    namespace encoding
    {
        /**
         * @brief ANSI转UTF-16LE
         * @param utf16str 转换后的字符串在此参数中返回
         * @param mbstr 转换前的字符串
         * @param mbslen 转换前的字符串字符长度,-1为自动获取
         * @param code_page 编码页
         */
        static void A2Wex(std::wstring &utf16str, const char *mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
        {
            int utf16len = ::MultiByteToWideChar(code_page, 0, mbstr, (mbslen == -1) ? -1 : static_cast<int>(mbslen), NULL, 0);
            if (utf16len > 0)
            {
                if (mbslen == -1)
                    utf16len -= 1;
                utf16str.resize(utf16len, '\0');
                ::MultiByteToWideChar(code_page, 0, mbstr, (mbslen == -1) ? -1 : static_cast<int>(mbslen), const_cast<wchar_t *>(utf16str.data()), utf16len);
            }
            else
            {
                utf16str.clear();
            }
        }
        static void A2Wex(CWString &utf16str, const char *mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
        {
            int utf16len = ::MultiByteToWideChar(code_page, 0, mbstr, (mbslen == -1) ? -1 : static_cast<int>(mbslen), NULL, 0);
            if (utf16len > 0)
            {
                if (mbslen == -1)
                    utf16len -= 1;
                utf16str.m_mem.Alloc((utf16len + 1) * 2, TRUE);
                ::MultiByteToWideChar(code_page, 0, mbstr, (mbslen == -1) ? -1 : static_cast<int>(mbslen), const_cast<wchar_t *>(utf16str.GetText()), utf16len);
            }
            else
            {
                utf16str.Empty();
            }
        }

        /**
         * @brief UTF-16LE转ANSI
         * @param mbstr 转换后的字符串在此参数中返回
         * @param utf16str 转换前的字符串
         * @param utf16len 转换前的字符串字符长度,-1为自动获取
         * @param code_page 编码页
         */
        static void W2Aex(std::string &mbstr, const wchar_t *utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
        {
            int mbslen = ::WideCharToMultiByte(code_page, 0, utf16str, (utf16len == -1) ? -1 : static_cast<int>(utf16len), NULL, 0, NULL, NULL);
            if (mbslen > 0)
            {
                if (utf16len == -1)
                    mbslen -= 1;
                mbstr.resize(mbslen, '\0');
                ::WideCharToMultiByte(code_page, 0, utf16str, (utf16len == -1) ? -1 : static_cast<int>(utf16len), const_cast<char *>(mbstr.data()), mbslen, NULL, NULL);
            }
            else
            {
                mbstr.clear();
            }
        }

        /**
         * @brief UTF-16LE转UTF-8
         * @param utf8str 转换后的字符串在此参数中返回
         * @param utf16str 转换前的字符串
         * @param utf16len 转换前的字符串字符长度,-1为自动获取
         */
        static void W2Uex(std::string &utf8str, const wchar_t *utf16str, size_t utf16len = -1)
        {
#ifdef SIMDUTF_H
            size_t utf16words = (utf16len == -1) ? wcslen(utf16str) : utf16len;
            size_t utf8words = simdutf::utf8_length_from_utf16le(reinterpret_cast<const char16_t *>(utf16str), utf16words);
            if (utf8words == 0)
            {
                utf8str.clear();
            }
            else
            {
                utf8str.resize(utf8words, '\0');
                simdutf::convert_valid_utf16le_to_utf8(reinterpret_cast<const char16_t *>(utf16str), utf16words, const_cast<char *>(reinterpret_cast<const char *>(utf8str.data())));
            }
#else
            int utf8len = ::WideCharToMultiByte(CP_UTF8, 0, utf16str, (utf16len == -1) ? -1 : static_cast<int>(utf16len), NULL, 0, NULL, NULL);
            if (utf8len > 0)
            {
                if (utf16len == -1)
                    utf8len -= 1;
                utf8str.resize(utf8len, '\0');
                ::WideCharToMultiByte(CP_UTF8, 0, utf16str, (utf16len == -1) ? -1 : static_cast<int>(utf16len), const_cast<char *>(utf8str.data()), utf8len, NULL, NULL);
            }
            else
            {
                utf8str.clear();
            }
#endif
        }

        /**
         * @brief UTF-8转UTF-16LE
         * @param utf16str 转换后的字符串在此参数中返回
         * @param utf8str 转换前的字符串
         * @param utf8len 转换前的字符串字符长度,-1为自动获取
         */
        static void U2Wex(std::wstring &utf16str, const char *utf8str, size_t utf8len = -1)
        {
#ifdef SIMDUTF_H
            size_t utf8words = (utf8len == -1) ? strlen(utf8str) : utf8len;
            size_t utf16words = simdutf::utf16_length_from_utf8(utf8str, utf8words);
            if (utf16words == 0)
            {
                utf16str.clear();
            }
            else
            {
                utf16str.resize(utf16words, '\0');
                utf16words = simdutf::convert_valid_utf8_to_utf16le(utf8str, utf8words, reinterpret_cast<char16_t *>(const_cast<wchar_t *>(utf16str.data())));
            }
#else
            int utf16len = ::MultiByteToWideChar(CP_UTF8, 0, utf8str, (utf8len == -1) ? -1 : static_cast<int>(utf8len), NULL, 0);
            if (utf16len > 0)
            {
                if (utf8len == -1)
                    utf16len -= 1;
                utf16str.resize(utf16len, '\0');
                ::MultiByteToWideChar(CP_UTF8, 0, utf8str, (utf8len == -1) ? -1 : static_cast<int>(utf8len), const_cast<wchar_t *>(utf16str.data()), utf16len);
            }
            else
            {
                utf16str.clear();
            }
#endif
        }
        static void U2Wex(CWString &utf16str, const char *utf8str, size_t utf8len = -1)
        {
#ifdef SIMDUTF_H
            size_t utf8words = (utf8len == -1) ? strlen(utf8str) : utf8len;
            size_t utf16words = simdutf::utf16_length_from_utf8(utf8str, utf8words);
            if (utf16words == 0)
            {
                utf16str.Empty();
            }
            else
            {
                utf16str.m_mem.Alloc((utf16words + 1) * 2, TRUE);
                utf16words = simdutf::convert_valid_utf8_to_utf16le(utf8str, utf8words, reinterpret_cast<char16_t *>(const_cast<wchar_t *>(utf16str.GetText())));
            }
#else
            int utf16len = ::MultiByteToWideChar(CP_UTF8, 0, utf8str, (utf8len == -1) ? -1 : static_cast<int>(utf8len), NULL, 0);
            if (utf16len > 0)
            {
                if (utf8len == -1)
                    utf16len -= 1;
                utf16str.m_mem.Alloc((utf16len + 1) * 2, TRUE);
                ::MultiByteToWideChar(CP_UTF8, 0, utf8str, (utf8len == -1) ? -1 : static_cast<int>(utf8len), const_cast<wchar_t *>(utf16str.GetText()), utf16len);
            }
            else
            {
                utf16str.Empty();
            }
#endif
        }

        /**
         * @brief UTF-8转ANSI
         * @param mbstr 转换后的字符串在此参数中返回
         * @param utf8str 转换前的字符串
         * @param utf8len 转换前的字符串字符长度,-1为自动获取
         */
        static void U2Aex(std::string &mbstr, const char *utf8str, size_t utf8len = -1, uint32_t code_page = CP_ACP)
        {
            std::wstring utf16str;
            ::piv::encoding::U2Wex(utf16str, utf8str, utf8len);
            ::piv::encoding::W2Aex(mbstr, utf16str.c_str(), utf16str.size(), code_page);
        }

        /**
         * @brief ANSI转UTF-8
         * @param utf8str 转换后的字符串在此参数中返回
         * @param mbstr 转换前的字符串
         * @param mbslen 转换前的字符串字符长度,-1为自动获取
         */
        static void A2Uex(std::string &utf8str, const char *mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
        {
            std::wstring utf16str;
            ::piv::encoding::A2Wex(utf16str, mbstr, mbslen, code_page);
            ::piv::encoding::W2Uex(utf8str, utf16str.c_str(), utf16str.size());
        }

        /**
         * @brief 是否有效URL编码字符
         * @param c 所欲判断的字符
         * @param Reserved 是否允许有特殊意义的保留字符
         * @return 返回字符是否为有效的URL编码字符
         */
        static bool IsUrlValidWord(const int &c, bool Reserved = false) noexcept
        {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') // 安全字符
                return true;
            else if (Reserved && (c == ';' || c == '/' || c == '?' || c == ':' || c == '@' || c == '=' || c == '&')) // 保留字符
                return true;
            else
                return false;
        }

        static bool IsValidAlphaNum(const int &c) noexcept
        {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
                return true;
            else
                return false;
        }

        /**
         * @brief 判断是否需要URL编码
         * @param lpszSrc 所欲编码的数据指针
         * @param SrcLen 所欲编码的数据长度
         * @param ReservedWord 是否不编码保留字符
         * @return
         */
        static bool UrlEncodeNeed(const wchar_t *lpszSrc, size_t SrcLen, bool ReservedWord = false) noexcept
        {
            for (size_t i = 0; i < SrcLen; i++)
            {
                if (!(::piv::encoding::IsUrlValidWord(lpszSrc[i], ReservedWord)))
                { // 判断是否已经URL编码
                    if (!(lpszSrc[i] == '%' && (i + 2) >= SrcLen && iswxdigit(lpszSrc[i + 1]) && iswxdigit(lpszSrc[i + 2])))
                        return true;
                }
            }
            return false;
        }
        static bool UrlEncodeNeed(const char *lpszSrc, size_t SrcLen, bool ReservedWord = false) noexcept
        {
            for (size_t i = 0; i < SrcLen; i++)
            {
                if (!(::piv::encoding::IsUrlValidWord(lpszSrc[i], ReservedWord)))
                { // 判断是否已经URL编码
                    if (!(lpszSrc[i] == '%' && (i + 2) >= SrcLen && isxdigit(static_cast<unsigned char>(lpszSrc[i + 1])) && isxdigit(static_cast<unsigned char>(lpszSrc[i + 2]))))
                        return true;
                }
            }
            return false;
        }

        /**
         * @brief 判断是否需要URL解码
         * @param lpszSrc 所欲编码的数据指针
         * @param SrcLen 所欲编码的数据长度
         * @param ReservedWord 是否不编码保留字符
         * @return
         */
        static bool UrlDecodeNeed(const wchar_t *lpszSrc, size_t SrcLen) noexcept
        {
            for (size_t i = 0; i < SrcLen; i++)
            {
                if (lpszSrc[i] == '%' && (i + 2) < SrcLen && iswxdigit(lpszSrc[i + 1]) && iswxdigit(lpszSrc[i + 2]))
                    return true;
            }
            return false;
        }
        static bool UrlDecodeNeed(const char *lpszSrc, size_t SrcLen) noexcept
        {
            for (size_t i = 0; i < SrcLen; i++)
            {
                if (lpszSrc[i] == '%' && (i + 2) < SrcLen && isxdigit(static_cast<unsigned char>(lpszSrc[i + 1])) && isxdigit(static_cast<unsigned char>(lpszSrc[i + 2])))
                    return true;
            }
            return false;
        }

        /**
         * @brief 获取URL编码后的长度
         * @param lpszSrc 所欲编码的数据指针
         * @param SrcLen 所欲编码的数据长度
         * @param ReservedWord 是否不编码保留字符
         * @return
         */
        static size_t GuessUrlEncodeBound(const unsigned char *lpszSrc, size_t SrcLen, bool ReservedWord = false) noexcept
        {
            size_t Add = 0;
            for (size_t i = 0; i < SrcLen; i++)
            {
                unsigned char c = lpszSrc[i];
                if (!(::piv::encoding::IsUrlValidWord(lpszSrc[i], ReservedWord)))
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
        static size_t GuessUrlDecodeBound(const unsigned char *lpszSrc, size_t SrcLen) noexcept
        {
            size_t Percent = 0;
            for (size_t i = 0; i < SrcLen; i++)
            {
                if (lpszSrc[i] == '%' && (i + 2) < SrcLen && isxdigit(lpszSrc[i + 1]) && isxdigit(lpszSrc[i + 2]))
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
         * @param ReservedWord 是否不编码保留字符
         * @return
         */
        static int32_t UrlEncode(const unsigned char *lpszSrc, size_t SrcLen, unsigned char *lpszDest, size_t &DestLen, bool ReservedWord = false) noexcept
        {
            if (lpszDest == nullptr || DestLen == 0)
            {
                DestLen = GuessUrlEncodeBound(lpszSrc, SrcLen);
                return -5;
            }
            unsigned char c, n;
            size_t j = 0;
            for (size_t i = 0; i < SrcLen; i++)
            {
                if (j >= DestLen)
                    goto ERROR_DEST_LEN;
                c = lpszSrc[i];
                if (::piv::encoding::IsUrlValidWord(lpszSrc[i], ReservedWord))
                    lpszDest[j++] = c;
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
        static int32_t UrlDecode(const unsigned char *lpszSrc, size_t SrcLen, unsigned char *lpszDest, size_t &DestLen) noexcept
        {
            if (lpszDest == nullptr || DestLen == 0)
            {
                DestLen = GuessUrlDecodeBound(lpszSrc, SrcLen);
                return -5;
            }
            unsigned char c, n;
            size_t j = 0;
            for (size_t i = 0; i < SrcLen; i++)
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

        /**
         * @brief 十六进制转换到字符值
         * @param chHexChar 十六进制字符
         * @return
         */
        template <typename CharT>
        unsigned char hexchar_to_value(const CharT &chHexChar) noexcept
        {
            if (chHexChar >= '0' && chHexChar <= '9')
                return static_cast<unsigned char>(chHexChar - '0');
            if (chHexChar >= 'a' && chHexChar <= 'f')
                return static_cast<unsigned char>(chHexChar - 'a' + 10);
            if (chHexChar >= 'A' && chHexChar <= 'F')
                return static_cast<unsigned char>(chHexChar - 'A' + 10);
            return static_cast<unsigned char>(chHexChar);
        }

        static constexpr char hexUpCh[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        static constexpr char hexLowCh[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

        /**
         * @brief 到十六进制文本
         * @param separator 是否带分隔符
         * @param hexstr 返回的十六进制文本
         * @return
         */
        template <typename CharT, typename StringT, typename T>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, std::basic_string<CharT>>::value, T>::type
        str_to_hex(const StringT &str, bool separator /* = false */, T &&hexstr)
        {
            hexstr.clear();
            if (str.empty())
                return std::forward<T>(hexstr);
            size_t count = str.size() * sizeof(typename StringT::value_type);
            hexstr.resize(count * (separator ? 3 : 2), ' ');
            StringT::value_type *pHex = const_cast<StringT::value_type *>(hexstr.data());
            const unsigned char *pStr = reinterpret_cast<const unsigned char *>(str.data());
            for (size_t i = 0; i < count; ++i)
            {
                *pHex++ = hexUpCh[static_cast<unsigned char>(pStr[i]) >> 4];
                *pHex++ = hexUpCh[static_cast<unsigned char>(pStr[i]) & 0xF];
                if (separator)
                    pHex++;
            }
            return std::forward<T>(hexstr);
        }

        template <typename StringT, typename T /* = CWString */>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, CWString>::value, T>::type
        str_to_hex(const StringT &str, bool separator /* = false */, T &&hexstr)
        {
            hexstr.Empty();
            if (str.empty())
                return std::forward<T>(hexstr);
            size_t count = str.size() * sizeof(typename StringT::value_type);
            wchar_t *pHex = nullptr;
            if (!separator)
            {
                pHex = reinterpret_cast<wchar_t *>(hexstr.m_mem.Realloc(count * 4 + 2));
                pHex[count * 2] = '\0';
            }
            else
            {
                hexstr.SetLength(count * 3);
                pHex = const_cast<wchar_t *>(hexstr.GetText());
            }
            const unsigned char *pStr = reinterpret_cast<const unsigned char *>(str.data());
            for (size_t i = 0; i < count; ++i)
            {
                *pHex++ = hexUpCh[static_cast<unsigned char>(pStr[i]) >> 4];
                *pHex++ = hexUpCh[static_cast<unsigned char>(pStr[i]) & 0xF];
                if (separator)
                    pHex++;
            }
            return std::forward<T>(hexstr);
        }

        /**
         * @brief 置十六进制文本
         * @param hexstr 所欲还原的十六进制文本
         * @param str 返回还原后的文本
         * @return
         */
        template <typename T /* = std::basic_string<CharT> */, typename StringT>
        T &&hex_to_str(const StringT &hexstr, T &&str)
        {
            str.clear();
            if (hexstr.size() < 2 * sizeof(typename StringT::value_type))
                return std::forward<T>(str);
            str.resize(hexstr.size() / (2 * sizeof(typename StringT::value_type)));
            unsigned char *pStr = const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(str.data()));
            typename StringT::value_type ch1, ch2;
            size_t count = hexstr.size(), n = 0;
            for (size_t i = 0; i < count; i++)
            {
                ch1 = hexstr[i];
                if (ch1 != ' ' && i + 1 < count)
                {
                    ch2 = hexstr[++i];
                    *pStr++ = (hexchar_to_value(ch1) << 4 | hexchar_to_value(ch2));
                    n++;
                }
            }
            str.resize(n / sizeof(typename StringT::value_type));
            return std::forward<T>(str);
        }

        /**
         * @brief 到USC2编码
         * @param str 所欲编码的UNICODE文本
         * @param en_ascii 是否编码ASCII字符
         * @param usc2 返回的USC2文本
         * @return
         */
        template <typename StringT, typename T>
        T &&to_usc2(const StringT &str, bool en_ascii, T &&usc2)
        {
            edit::clear(usc2);
            edit::reserve(usc2, str.size() * 6);
            wchar_t ch;
            for (size_t i = 0; i < str.size(); i++)
            {
                ch = str[i];
                if (!en_ascii && ch < 128)
                {
                    edit::push_back(usc2, ch);
                    continue;
                }
                edit::push_back(usc2, '\\');
                edit::push_back(usc2, 'u');
                edit::push_back(usc2, hexLowCh[static_cast<unsigned char>(ch >> 12)]);
                edit::push_back(usc2, hexLowCh[static_cast<unsigned char>(ch >> 8 & 0x000F)]);
                edit::push_back(usc2, hexLowCh[static_cast<unsigned char>((ch & 0x00FF) >> 4)]);
                edit::push_back(usc2, hexLowCh[static_cast<unsigned char>(ch & 0x000F)]);
            }
            return std::forward<T>(usc2);
        }

        /**
         * @brief USC2编码到文本
         * @param usc2 USC2编码
         * @param ret 返回文本
         * @return
         */
        template <typename StringT, typename T>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, std::wstring>::value, T>::type
        usc2_to_str(const StringT &usc2, T &&ret)
        {
            ret.clear();
            size_t len = usc2.size();
            ret.reserve(len);
            typename StringT::value_type ch;
            unsigned char hex[4];
            for (size_t i = 0; i < len; i++)
            {
                ch = usc2[i];
                if (ch == '\\' && i + 5 < len && (usc2[i + 1] == 'u' || usc2[i + 1] == 'U'))
                {
                    hex[0] = hexchar_to_value(usc2[i + 2]);
                    hex[1] = hexchar_to_value(usc2[i + 3]);
                    hex[2] = hexchar_to_value(usc2[i + 4]);
                    hex[3] = hexchar_to_value(usc2[i + 5]);
                    if (hex[0] == usc2[i + 2] || hex[1] == usc2[i + 3] || hex[2] == usc2[i + 4] || hex[3] == usc2[i + 5])
                    {
                        ret.push_back(static_cast<wchar_t>(ch));
                        continue;
                    }
                    ret.push_back(hex[0] << 12 | hex[1] << 8 | hex[2] << 4 | hex[3]);
                    i += 5;
                }
                else
                {
                    ret.push_back(static_cast<wchar_t>(ch));
                }
            }
            return std::forward<T>(ret);
        }

        /**
         * @brief 值到十六进制
         * @param v 所欲转换的hash值
         * @param hexstr 返回的十六进制
         * @return
         */
        template <typename V>
        CVolMem &value_to_hex(const V &v, CVolMem &hex)
        {
            hex.Append(&v, sizeof(V));
            return hex;
        }

        template <typename V, typename T>
        T &&value_to_hex(const V &v, T &&hex, bool upper = true)
        {
            const unsigned char *pValue = reinterpret_cast<const unsigned char *>(&v);
            if (upper)
            {
                for (size_t i = 0, n = 0; i < sizeof(V); i++)
                {
                    ::piv::edit::push_back(hex, hexUpCh[static_cast<unsigned char>(pValue[i]) >> 4]);
                    ::piv::edit::push_back(hex, hexUpCh[static_cast<unsigned char>(pValue[i]) & 0xF]);
                }
            }
            else
            {
                for (size_t i = 0, n = 0; i < sizeof(V); i++)
                {
                    ::piv::edit::push_back(hex, hexLowCh[static_cast<unsigned char>(pValue[i]) >> 4]);
                    ::piv::edit::push_back(hex, hexLowCh[static_cast<unsigned char>(pValue[i]) & 0xF]);
                }
            }
            return std::forward<T>(hex);
        }
    } // namespace encoding

} // namespace piv

/**
 * @brief ANSI编码转UTF-16LE编码的封装类
 */
class PivA2W
{
private:
    std::wstring _data;

public:
    /**
     * @brief 默认构造与析构函数
     */
    PivA2W() {}

    ~PivA2W() {}

    /**
     * @brief 复制和移动构造函数
     * @param s 所欲复制的对象
     */
    PivA2W(const PivA2W &rhs)
    {
        _data = rhs._data;
    }

    PivA2W(PivA2W &&rhs) noexcept
    {
        _data = std::move(rhs._data);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivA2W(const char *mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(mbstr, mbslen, code_page);
    }

    PivA2W(const std::string &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(mbstr, mbslen, code_page);
    }

    PivA2W(const piv::string_view &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(mbstr, mbslen, code_page);
    }

    PivA2W(const CVolMem &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(mbstr, mbslen, code_page);
    }

    operator const wchar_t *() const noexcept
    {
        return _data.c_str();
    }

    operator const std::wstring &() const noexcept
    {
        return _data;
    }

    operator std::wstring &() noexcept
    {
        return _data;
    }

    operator CWString() const
    {
        return CWString{_data.c_str()};
    }

    operator CVolMem() const
    {
        return CVolMem(_data.c_str(), _data.size() * 2);
    }

    PivA2W &operator=(const PivA2W &rhs)
    {
        _data = rhs._data;
        return *this;
    }

    PivA2W &operator=(PivA2W &&rhs) noexcept
    {
        _data = std::move(rhs._data);
        return *this;
    }

    PivA2W &operator=(const char *rhs)
    {
        convert(rhs);
        return *this;
    }

    bool operator==(const PivA2W &rhs) const noexcept
    {
        return _data == rhs._data;
    }

    std::wstring *operator->() noexcept
    {
        return &_data;
    }

    const std::wstring *operator->() const noexcept
    {
        return &_data;
    }

    std::wstring &operator*() noexcept
    {
        return _data;
    }

    const std::wstring &operator*() const noexcept
    {
        return _data;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的ANSI文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    inline void convert(const char *mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        piv::encoding::A2Wex(_data, mbstr, mbslen, code_page);
    }

    inline void convert(const std::string &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(mbstr.c_str(), (mbslen == -1) ? mbstr.size() : mbslen, code_page);
    }

    inline void convert(const piv::string_view &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(mbstr.data(), (mbslen == -1) ? mbstr.size() : mbslen, code_page);
    }

    inline void convert(const CVolMem &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(reinterpret_cast<const char *>(mbstr.GetPtr()), (mbslen == -1) ? static_cast<size_t>(mbstr.GetSize()) : mbslen, code_page);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const wchar_t *c_str() const noexcept
    {
        return _data.c_str();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t size() const noexcept
    {
        return _data.size();
    }

    /**
     * @brief 获取转换后的文本字节长度
     * @return 文本的字节长度
     */
    inline size_t size_bytes() const noexcept
    {
        return _data.size() * 2;
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    inline wchar_t *data() noexcept
    {
        return _data.empty() ? nullptr : const_cast<wchar_t *>(_data.data());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    inline bool empty() const noexcept
    {
        return _data.empty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    inline std::wstring &ref() noexcept
    {
        return _data;
    }
    inline const std::wstring &cref() const noexcept
    {
        return _data;
    }

    /**
     * @brief 生成一个火山的文本型,其中包含了转换后的文本数据
     * @return 返回所转换的文本型
     */
    inline CWString &to_volstr(CWString &str) const
    {
        str.SetText(_data.c_str());
        return str;
    }

    inline CWString &&to_volstr(CWString &&str) const
    {
        str.SetText(_data.c_str());
        return std::forward<CWString &&>(str);
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &to_volmem(CVolMem &mem, bool null_char = false) const
    {
        mem.Empty();
        mem.Append(_data.c_str(), (_data.size() + (null_char ? 1 : 0)) * 2);
        return mem;
    }

    inline CVolMem &&to_volmem(CVolMem &&mem, bool null_char = false) const
    {
        mem.Empty();
        mem.Append(_data.c_str(), (_data.size() + (null_char ? 1 : 0)) * 2);
        return std::forward<CVolMem &&>(mem);
    }
}; // PivA2W

/**
 * @brief UTF-16LE编码转ANSI编码的封装类
 */
class PivW2A
{
private:
    std::string _data;

public:
    /**
     * @brief 默认构造与析构函数
     */
    PivW2A() {}

    ~PivW2A() {}

    /**
     * @brief 复制和移动构造函数
     * @param s 所欲复制的对象
     */
    PivW2A(const PivW2A &rhs)
    {
        _data = rhs._data;
    }

    PivW2A(PivW2A &&rhs) noexcept
    {
        _data = std::move(rhs._data);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param utf16str 所欲转换的UTF-16LE文本
     * @param utf16len 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivW2A(const wchar_t *utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        convert(utf16str, utf16len, code_page);
    }

    PivW2A(const std::wstring &utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        convert(utf16str, utf16len, code_page);
    }

    PivW2A(const CWString &utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        convert(utf16str, utf16len, code_page);
    }

    PivW2A(const piv::wstring_view &utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        convert(utf16str, utf16len, code_page);
    }

    PivW2A(const CVolMem &utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        convert(utf16str, utf16len, code_page);
    }

    operator const char *() const noexcept
    {
        return _data.c_str();
    }

    operator const std::string &() const noexcept
    {
        return _data;
    }

    operator std::string &() noexcept
    {
        return _data;
    }

    operator CVolMem() const
    {
        return CVolMem(_data.c_str(), _data.size());
    }

    PivW2A &operator=(const PivW2A &rhs)
    {
        _data = rhs._data;
        return *this;
    }

    PivW2A &operator=(PivW2A &&rhs) noexcept
    {
        _data = std::move(rhs._data);
        return *this;
    }

    PivW2A &operator=(const wchar_t *rhs)
    {
        convert(rhs);
        return *this;
    }

    bool operator==(const PivW2A &rhs) const noexcept
    {
        return _data == rhs._data;
    }

    std::string *operator->() noexcept
    {
        return &_data;
    }

    const std::string *operator->() const noexcept
    {
        return &_data;
    }

    std::string &operator*() noexcept
    {
        return _data;
    }

    const std::string &operator*() const noexcept
    {
        return _data;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    inline void convert(const wchar_t *utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        piv::encoding::W2Aex(_data, utf16str, utf16len, code_page);
    }

    inline void convert(const std::wstring &utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        convert(utf16str.c_str(), (utf16len == -1) ? utf16str.size() : utf16len, code_page);
    }

    inline void convert(const CWString &utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        convert(utf16str.GetText(), (utf16len == -1) ? static_cast<size_t>(utf16str.GetLength()) : utf16len, code_page);
    }

    inline void convert(const piv::wstring_view &utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        convert(utf16str.data(), (utf16len == -1) ? utf16str.size() : utf16len, code_page);
    }

    inline void convert(const CVolMem &utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        convert(reinterpret_cast<const wchar_t *>(utf16str.GetPtr()), (utf16len == -1) ? static_cast<size_t>(utf16str.GetSize() / 2) : utf16len, code_page);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const char *c_str() const noexcept
    {
        return _data.c_str();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t size() const noexcept
    {
        return _data.size();
    }

    /**
     * @brief 获取转换后的文本字长度
     * @return 文本的字节长度
     */
    inline size_t size_bytes() const noexcept
    {
        return _data.size();
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    inline char *data() noexcept
    {
        return _data.empty() ? nullptr : const_cast<char *>(_data.data());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    inline bool empty() const noexcept
    {
        return _data.empty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    inline std::string &ref() noexcept
    {
        return _data;
    }
    inline const std::string &cref() const noexcept
    {
        return _data;
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &to_volmem(CVolMem &mem, bool null_char = false) const
    {
        mem.Empty();
        mem.Append(_data.c_str(), _data.size() + (null_char ? 1 : 0));
        return mem;
    }

    inline CVolMem &&to_volmem(CVolMem &&mem, bool null_char = false) const
    {
        mem.Empty();
        mem.Append(_data.c_str(), _data.size() + (null_char ? 1 : 0));
        return std::forward<CVolMem &&>(mem);
    }
}; // PivW2A

/**
 * @brief UTF-16LE编码转UTF-8编码的封装类
 */
class PivW2U
{
private:
    std::string _data;

public:
    /**
     * @brief 默认构造与析构函数
     */
    PivW2U() {}

    ~PivW2U() {}

    /**
     * @brief 复制和移动构造函数
     * @param s 所欲复制的对象
     */
    PivW2U(const PivW2U &rhs)
    {
        _data = rhs._data;
    }

    PivW2U(PivW2U &&rhs) noexcept
    {
        _data = std::move(rhs._data);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param utf16str 所欲转换的UTF-16LE文本
     * @param utf16len 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivW2U(const wchar_t *utf16str, size_t utf16len = -1)
    {
        convert(utf16str, utf16len);
    }

    PivW2U(const std::wstring &utf16str, size_t utf16len = -1)
    {
        convert(utf16str, utf16len);
    }

    PivW2U(const CWString &utf16str, size_t utf16len = -1)
    {
        convert(utf16str, utf16len);
    }

    PivW2U(const piv::wstring_view &utf16str, size_t utf16len = -1)
    {
        convert(utf16str, utf16len);
    }

    PivW2U(const CVolMem &utf16str, size_t utf16len = -1)
    {
        convert(utf16str, utf16len);
    }

    operator CVolMem() const
    {
        return CVolMem(_data.c_str(), _data.size());
    }

    operator const char *() const noexcept
    {
        return _data.c_str();
    }

    operator const std::string &() const noexcept
    {
        return _data;
    }

    operator std::string &() noexcept
    {
        return _data;
    }

    PivW2U &operator=(const PivW2U &rhs)
    {
        _data = rhs._data;
        return *this;
    }

    PivW2U &operator=(PivW2U &&rhs) noexcept
    {
        _data = std::move(rhs._data);
        return *this;
    }

    PivW2U &operator=(const wchar_t *rhs)
    {
        convert(rhs);
        return *this;
    }

    bool operator==(const PivW2U &rhs) const noexcept
    {
        return _data == rhs._data;
    }

    std::string *operator->() noexcept
    {
        return &_data;
    }

    const std::string *operator->() const noexcept
    {
        return &_data;
    }

    std::string &operator*() noexcept
    {
        return _data;
    }

    const std::string &operator*() const noexcept
    {
        return _data;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    inline void convert(const wchar_t *utf16str, size_t utf16len = -1)
    {
        piv::encoding::W2Uex(_data, utf16str, utf16len);
    }

    inline void convert(const std::wstring &utf16str, size_t utf16len = -1)
    {
        convert(utf16str.c_str(), (utf16len == -1) ? utf16str.size() : utf16len);
    }

    inline void convert(const CWString &utf16str, size_t utf16len = -1)
    {
        convert(utf16str.GetText(), (utf16len == -1) ? static_cast<size_t>(utf16str.GetLength()) : utf16len);
    }

    inline void convert(const piv::wstring_view &utf16str, size_t utf16len = -1)
    {
        convert(utf16str.data(), (utf16len == -1) ? utf16str.size() : utf16len);
    }

    inline void convert(const CVolMem &utf16str, size_t utf16len = -1)
    {
        convert(reinterpret_cast<const wchar_t *>(utf16str.GetPtr()), (utf16len == -1) ? static_cast<size_t>(utf16str.GetSize() / 2) : utf16len);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const char *c_str() const noexcept
    {
        return _data.c_str();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t size() const noexcept
    {
        return _data.size();
    }

    /**
     * @brief 获取转换后的文本字节长度
     * @return 文本的字节长度
     */
    inline size_t size_bytes() const noexcept
    {
        return _data.size();
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    inline char *data() noexcept
    {
        return _data.empty() ? nullptr : const_cast<char *>(_data.data());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    inline bool empty() const noexcept
    {
        return _data.empty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    inline std::string &ref() noexcept
    {
        return _data;
    }
    inline const std::string &cref() const noexcept
    {
        return _data;
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &to_volmem(CVolMem &mem, bool null_char = false) const
    {
        mem.Empty();
        mem.Append(_data.c_str(), _data.size() + (null_char ? 1 : 0));
        return mem;
    }

    inline CVolMem &&to_volmem(CVolMem &&mem, bool null_char = false) const
    {
        mem.Empty();
        mem.Append(_data.c_str(), _data.size() + (null_char ? 1 : 0));
        return std::forward<CVolMem &&>(mem);
    }
}; // PivW2U

/**
 * @brief UTF-8编码转UTF-16LE编码的封装类
 */
class PivU2W
{
private:
    std::wstring _data;

public:
    /**
     * @brief 默认构造与析构函数
     */
    PivU2W() {}

    ~PivU2W() {}

    /**
     * @brief 复制和移动构造函数
     * @param s 所欲复制的对象
     */
    PivU2W(const PivU2W &rhs)
    {
        _data = rhs._data;
    }

    PivU2W(PivU2W &&rhs) noexcept
    {
        _data = std::move(rhs._data);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param utf8str 所欲转换的UTF-8文本
     * @param utf8len 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivU2W(const char *utf8str, size_t utf8len = -1)
    {
        convert(utf8str, utf8len);
    }

    PivU2W(const std::string &utf8str, size_t utf8len = -1)
    {
        convert(utf8str, utf8len);
    }

    PivU2W(const piv::string_view &utf8str, size_t utf8len = -1)
    {
        convert(utf8str, utf8len);
    }

    PivU2W(const CVolMem &utf8str, size_t utf8len = -1)
    {
        convert(utf8str, utf8len);
    }

    operator const wchar_t *() const noexcept
    {
        return _data.c_str();
    }

    operator const std::wstring &() const noexcept
    {
        return _data;
    }

    operator std::wstring &()
    {
        return _data;
    }

    operator CWString() const
    {
        return CWString{_data.c_str()};
    }

    operator CVolMem() const
    {
        return CVolMem(_data.c_str(), _data.size() * 2);
    }

    PivU2W &operator=(const PivU2W &rhs)
    {
        _data = rhs._data;
        return *this;
    }

    PivU2W &operator=(PivU2W &&rhs) noexcept
    {
        _data = std::move(rhs._data);
        return *this;
    }

    PivU2W &operator=(const char *rhs)
    {
        convert(rhs);
        return *this;
    }

    bool operator==(const PivU2W &rhs) const noexcept
    {
        return _data == rhs._data;
    }

    std::wstring *operator->() noexcept
    {
        return &_data;
    }

    const std::wstring *operator->() const noexcept
    {
        return &_data;
    }

    std::wstring &operator*() noexcept
    {
        return _data;
    }

    const std::wstring &operator*() const noexcept
    {
        return _data;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    inline void convert(const char *utf8str, size_t utf8len = -1)
    {
        piv::encoding::U2Wex(_data, utf8str, utf8len);
    }

    inline void convert(const std::string &utf8str, size_t utf8len = -1)
    {
        convert(utf8str.c_str(), (utf8len == -1) ? utf8str.size() : utf8len);
    }

    inline void convert(const piv::string_view &utf8str, size_t utf8len = -1)
    {
        convert(utf8str.data(), (utf8len == -1) ? utf8str.size() : utf8len);
    }

    inline void convert(const CVolMem &utf8str, size_t utf8len = -1)
    {
        convert(reinterpret_cast<const char *>(utf8str.GetPtr()), (utf8len == -1) ? static_cast<size_t>(utf8str.GetSize()) : utf8len);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const wchar_t *c_str() const noexcept
    {
        return _data.c_str();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t size() const noexcept
    {
        return _data.size();
    }

    /**
     * @brief 获取转换后的文本字节长度
     * @return 文本的字节长度
     */
    inline size_t size_bytes() const noexcept
    {
        return _data.size() * 2;
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    inline wchar_t *data() noexcept
    {
        return _data.empty() ? nullptr : const_cast<wchar_t *>(_data.data());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    inline bool empty() const noexcept
    {
        return _data.empty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    inline std::wstring &ref() noexcept
    {
        return _data;
    }
    inline const std::wstring &cref() const noexcept
    {
        return _data;
    }

    /**
     * @brief 生成一个火山的文本型,其中包含了转换后的文本数据
     * @return 返回所转换的文本型
     */
    inline CWString &to_volstr(CWString &str) const
    {
        str.SetText(_data.c_str());
        return str;
    }

    inline CWString &&to_volstr(CWString &&str) const
    {
        str.SetText(_data.c_str());
        return std::forward<CWString &&>(str);
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &to_volmem(CVolMem &mem, bool null_char = false) const
    {
        mem.Empty();
        mem.Append(_data.c_str(), (_data.size() + (null_char ? 1 : 0)) * 2);
        return mem;
    }

    inline CVolMem &&to_volmem(CVolMem &&mem, bool null_char = false) const
    {
        mem.Empty();
        mem.Append(_data.c_str(), (_data.size() + (null_char ? 1 : 0)) * 2);
        return std::forward<CVolMem &&>(mem);
    }
}; // PivU2W

/**
 * @brief UTF-8编码转ANSI编码的封装类
 */
class PivU2A
{
private:
    std::string _data;

public:
    /**
     * @brief 默认构造与析构函数
     */
    PivU2A() {}

    ~PivU2A() {}

    /**
     * @brief 复制和移动构造函数
     * @param s 所欲复制的对象
     */
    PivU2A(const PivU2A &rhs)
    {
        _data = rhs._data;
    }

    PivU2A(PivU2A &&rhs) noexcept
    {
        _data = std::move(rhs._data);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param utf8str 所欲转换的UTF-8文本
     * @param utf8len 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivU2A(const char *utf8str, size_t utf8len = -1, uint32_t code_page = CP_ACP)
    {
        convert(utf8str, utf8len, code_page);
    }

    PivU2A(const std::string &utf8str, size_t utf8len = -1, uint32_t code_page = CP_ACP)
    {
        convert(utf8str, utf8len, code_page);
    }

    PivU2A(const piv::string_view &utf8str, size_t utf8len = -1, uint32_t code_page = CP_ACP)
    {
        convert(utf8str, utf8len, code_page);
    }

    PivU2A(const CVolMem &utf8str, size_t utf8len = -1, uint32_t code_page = CP_ACP)
    {
        convert(utf8str, utf8len, code_page);
    }

    operator const char *() const noexcept
    {
        return _data.c_str();
    }

    operator const std::string &() const noexcept
    {
        return _data;
    }

    operator std::string &() noexcept
    {
        return _data;
    }

    operator CVolMem() const
    {
        return CVolMem(_data.c_str(), _data.size());
    }

    PivU2A &operator=(const PivU2A &rhs)
    {
        _data = rhs._data;
        return *this;
    }

    PivU2A &operator=(PivU2A &&rhs) noexcept
    {
        _data = std::move(rhs._data);
        return *this;
    }

    PivU2A &operator=(const char *utf8str)
    {
        convert(utf8str);
        return *this;
    }

    bool operator==(const PivU2A &rhs) const noexcept
    {
        return _data == rhs._data;
    }

    std::string *operator->() noexcept
    {
        return &_data;
    }

    const std::string *operator->() const noexcept
    {
        return &_data;
    }

    std::string &operator*() noexcept
    {
        return _data;
    }

    const std::string &operator*() const noexcept
    {
        return _data;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    inline void convert(const char *utf8str, size_t utf8len = -1, uint32_t code_page = CP_ACP)
    {
        piv::encoding::U2Aex(_data, utf8str, utf8len, code_page);
    }

    inline void convert(const std::string &utf8str, size_t utf8len = -1, uint32_t code_page = CP_ACP)
    {
        convert(utf8str.c_str(), (utf8len == -1) ? utf8str.size() : utf8len, code_page);
    }

    inline void convert(const piv::string_view &utf8str, size_t utf8len = -1, uint32_t code_page = CP_ACP)
    {
        convert(utf8str.data(), (utf8len == -1) ? utf8str.size() : utf8len, code_page);
    }

    inline void convert(const CVolMem &utf8str, size_t utf8len = -1, uint32_t code_page = CP_ACP)
    {
        convert(reinterpret_cast<const char *>(utf8str.GetPtr()), (utf8len == -1) ? static_cast<size_t>(utf8str.GetSize()) : utf8len, code_page);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const char *c_str() const noexcept
    {
        return _data.c_str();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t size() const noexcept
    {
        return _data.size();
    }

    /**
     * @brief 获取转换后的字节长度
     * @return 文本的字节长度
     */
    inline size_t size_bytes() const noexcept
    {
        return _data.size();
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    inline char *data() noexcept
    {
        return _data.empty() ? nullptr : const_cast<char *>(_data.data());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    inline bool empty() const noexcept
    {
        return _data.empty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    inline std::string &ref() noexcept
    {
        return _data;
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    inline const std::string &cref() const noexcept
    {
        return _data;
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &to_volmem(CVolMem &mem, bool null_char = false) const
    {
        mem.Empty();
        mem.Append(_data.c_str(), _data.size() + (null_char ? 1 : 0));
        return mem;
    }

    inline CVolMem &&to_volmem(CVolMem &&mem, bool null_char = false) const
    {
        mem.Empty();
        mem.Append(_data.c_str(), _data.size() + (null_char ? 1 : 0));
        return std::forward<CVolMem &&>(mem);
    }
}; // PivU2A

/**
 * @brief ANSI编码转UTF-8编码的封装类
 */
class PivA2U
{
private:
    std::string _data;

public:
    /**
     * @brief 默认构造与析构函数
     */
    PivA2U() {}

    ~PivA2U() {}

    /**
     * @brief 复制和移动构造函数
     * @param s 所欲复制的对象
     */
    PivA2U(const PivA2U &rhs)
    {
        _data = rhs._data;
    }

    PivA2U(PivA2U &&rhs) noexcept
    {
        _data = std::move(rhs._data);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param mbstr 所欲转换的ANSI文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivA2U(const char *mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(mbstr, mbslen, code_page);
    }

    PivA2U(const std::string &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(mbstr, mbslen, code_page);
    }

    PivA2U(const piv::string_view &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(mbstr, mbslen, code_page);
    }

    PivA2U(const CVolMem &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(mbstr, mbslen, code_page);
    }

    operator const char *() const noexcept
    {
        return _data.c_str();
    }

    operator const std::string &() const noexcept
    {
        return _data;
    }

    operator std::string &() noexcept
    {
        return _data;
    }

    operator CVolMem() const
    {
        return CVolMem(_data.c_str(), _data.size());
    }

    PivA2U &operator=(const PivA2U &rhs)
    {
        _data = rhs._data;
        return *this;
    }

    PivA2U &operator=(PivA2U &&rhs) noexcept
    {
        _data = std::move(rhs._data);
        return *this;
    }

    PivA2U &operator=(const char *mbstr)
    {
        convert(mbstr);
        return *this;
    }

    bool operator==(const PivA2U &rhs) const noexcept
    {
        return _data == rhs._data;
    }

    std::string *operator->() noexcept
    {
        return &_data;
    }

    const std::string *operator->() const noexcept
    {
        return &_data;
    }

    std::string &operator*() noexcept
    {
        return _data;
    }

    const std::string &operator*() const noexcept
    {
        return _data;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    inline void convert(const char *mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        piv::encoding::A2Uex(_data, mbstr, mbslen, code_page);
    }

    inline void convert(const std::string &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(mbstr.c_str(), (mbslen == -1) ? mbstr.size() : mbslen, code_page);
    }

    inline void convert(const piv::string_view &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(mbstr.data(), (mbslen == -1) ? mbstr.size() : mbslen, code_page);
    }

    inline void convert(const CVolMem &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(reinterpret_cast<const char *>(mbstr.GetPtr()), (mbslen == -1) ? static_cast<size_t>(mbstr.GetSize()) : mbslen, code_page);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const char *c_str() const noexcept
    {
        return _data.c_str();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t size() const noexcept
    {
        return _data.size();
    }

    /**
     * @brief 获取转换后的字节长度
     * @return 文本的字节长度
     */
    inline size_t size_bytes() const noexcept
    {
        return _data.size();
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    inline char *data() noexcept
    {
        return _data.empty() ? nullptr : const_cast<char *>(_data.data());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    inline bool empty() const noexcept
    {
        return _data.empty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    inline std::string &ref() noexcept
    {
        return _data;
    }

    inline const std::string &cref() const noexcept
    {
        return _data;
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &to_volmem(CVolMem &mem, bool null_char = false) const
    {
        mem.Empty();
        mem.Append(_data.c_str(), _data.size() + (null_char ? 1 : 0));
        return mem;
    }

    inline CVolMem &&to_volmem(CVolMem &&mem, bool null_char = false) const
    {
        mem.Empty();
        mem.Append(_data.c_str(), _data.size() + (null_char ? 1 : 0));
        return std::forward<CVolMem &&>(mem);
    }
}; // PivA2U

/**
 * @brief ANSI编码转UTF-16LE编码的封装类(内部数据为火山文本型)
 */
class PivA2Ws
{
private:
    CWString _data;

public:
    /**
     * @brief 默认构造与析构函数
     */
    PivA2Ws() {}

    ~PivA2Ws() {}

    /**
     * @brief 复制和移动构造函数
     * @param s 所欲复制的对象
     */
    PivA2Ws(const PivA2Ws &rhs)
    {
        _data = rhs._data;
    }

    PivA2Ws(PivA2Ws &&rhs) noexcept
    {
        _data = std::move(rhs._data);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivA2Ws(const char *mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(mbstr, mbslen, code_page);
    }

    PivA2Ws(const std::string &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(mbstr, mbslen, code_page);
    }

    PivA2Ws(const piv::string_view &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(mbstr, mbslen, code_page);
    }

    PivA2Ws(const CVolMem &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(mbstr, mbslen, code_page);
    }

    operator const wchar_t *() const noexcept
    {
        return _data.GetText();
    }

    operator const CWString &() const noexcept
    {
        return _data;
    }

    operator CWString &() noexcept
    {
        return _data;
    }

    operator CVolMem() const
    {
        return CVolMem(_data.GetText(), _data.GetLength() * 2);
    }

    PivA2Ws &operator=(const PivA2Ws &rhs)
    {
        _data = rhs._data;
        return *this;
    }

    PivA2Ws &operator=(PivA2Ws &&rhs) noexcept
    {
        _data = std::move(rhs._data);
        return *this;
    }

    PivA2Ws &operator=(const char *rhs)
    {
        convert(rhs);
        return *this;
    }

    bool operator==(const PivA2Ws &rhs) const noexcept
    {
        return _data == rhs._data;
    }
    CWString *operator->() noexcept
    {
        return &_data;
    }

    const CWString *operator->() const noexcept
    {
        return &_data;
    }

    CWString &operator*() noexcept
    {
        return _data;
    }

    const CWString &operator*() const noexcept
    {
        return _data;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的ANSI文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    inline void convert(const char *mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        piv::encoding::A2Wex(_data, mbstr, mbslen, code_page);
    }

    inline void convert(const std::string &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(mbstr.c_str(), (mbslen == -1) ? mbstr.size() : mbslen, code_page);
    }

    inline void convert(const piv::string_view &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(mbstr.data(), (mbslen == -1) ? mbstr.size() : mbslen, code_page);
    }

    inline void convert(const CVolMem &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        convert(reinterpret_cast<const char *>(mbstr.GetPtr()), (mbslen == -1) ? static_cast<size_t>(mbstr.GetSize()) : mbslen, code_page);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const wchar_t *c_str() const noexcept
    {
        return _data.GetText();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t size() const noexcept
    {
        return static_cast<size_t>(_data.GetLength());
    }

    /**
     * @brief 获取转换后的字节长度
     * @return 文本的字节长度
     */
    inline size_t size_bytes() const noexcept
    {
        return _data.GetLength() * 2;
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    inline wchar_t *data() noexcept
    {
        return _data.IsEmpty() ? nullptr : const_cast<wchar_t *>(_data.m_mem.GetTextPtr());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    inline bool empty() const noexcept
    {
        return _data.IsEmpty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return
     */
    inline CWString &ref() noexcept
    {
        return _data;
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return
     */
    inline const CWString &cref() const noexcept
    {
        return _data;
    }

    /**
     * @brief 返回一个标准文本类,其中包含了转换后的文本数据
     * @return 返回转换后的std::basic_string
     */
    inline std::wstring to_str() const noexcept
    {
        return std::wstring{_data.GetText()};
    }

    /**
     * @brief 生成一个火山的文本型,其中包含了转换后的文本数据
     * @return 返回所转换的文本型
     */
    inline CWString &to_volstr(CWString &str) const
    {
        str.SetText(_data.GetText());
        return str;
    }

    inline CWString &&to_volstr(CWString &&str) const
    {
        str.SetText(_data.GetText());
        return std::forward<CWString &&>(str);
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &to_volmem(CVolMem &mem, bool null_char = false) const
    {
        mem.Empty();
        mem.Append(_data.GetText(), (_data.GetLength() + (null_char ? 1 : 0)) * 2);
        return mem;
    }

    inline CVolMem &&to_volmem(CVolMem &&mem, bool null_char = false) const
    {
        mem.Empty();
        mem.Append(_data.GetText(), (_data.GetLength() + (null_char ? 1 : 0)) * 2);
        return std::forward<CVolMem &&>(mem);
    }
}; // PivA2Ws

/**
 * @brief UTF-8编码转UTF-16LE编码的封装类(内部数据为火山文本型)
 */
class PivU2Ws
{
private:
    CWString _data;

public:
    /**
     * @brief 默认构造与析构函数
     */
    PivU2Ws() {}

    ~PivU2Ws() {}

    /**
     * @brief 复制和移动构造函数
     * @param s 所欲复制的对象
     */
    PivU2Ws(const PivU2Ws &rhs)
    {
        _data = rhs._data;
    }

    PivU2Ws(PivU2Ws &&rhs) noexcept
    {
        _data = std::move(rhs._data);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param utf8str 所欲转换的UTF-8文本
     * @param utf8len 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivU2Ws(const char *utf8str, size_t utf8len = -1)
    {
        convert(utf8str, utf8len);
    }

    PivU2Ws(const std::string &utf8str, size_t utf8len = -1)
    {
        convert(utf8str, utf8len);
    }

    PivU2Ws(const piv::string_view &utf8str, size_t utf8len = -1)
    {
        convert(utf8str, utf8len);
    }

    PivU2Ws(const CVolMem &utf8str, size_t utf8len = -1)
    {
        convert(utf8str, utf8len);
    }

    operator const wchar_t *() const noexcept
    {
        return _data.GetText();
    }

    operator const CWString &() const noexcept
    {
        return _data;
    }

    operator CWString &() noexcept
    {
        return _data;
    }

    operator CVolMem() const
    {
        return CVolMem(_data.GetText(), _data.GetLength() * 2);
    }

    PivU2Ws &operator=(const PivU2Ws &rhs)
    {
        _data = rhs._data;
        return *this;
    }

    PivU2Ws &operator=(PivU2Ws &&rhs) noexcept
    {
        _data = std::move(rhs._data);
        return *this;
    }

    PivU2Ws &operator=(const char *utf8str)
    {
        convert(utf8str);
        return *this;
    }

    bool operator==(const PivU2Ws &rhs) const noexcept
    {
        return _data == rhs._data;
    }

    CWString *operator->() noexcept
    {
        return &_data;
    }

    const CWString *operator->() const noexcept
    {
        return &_data;
    }

    CWString &operator*() noexcept
    {
        return _data;
    }

    const CWString &operator*() const noexcept
    {
        return _data;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    inline void convert(const char *utf8str, size_t utf8len = -1)
    {
        piv::encoding::U2Wex(_data, utf8str, utf8len);
    }

    inline void convert(const std::string &utf8str, size_t utf8len = -1)
    {
        convert(utf8str.c_str(), (utf8len == -1) ? utf8str.size() : utf8len);
    }

    inline void convert(const piv::string_view &utf8str, size_t utf8len = -1)
    {
        convert(utf8str.data(), (utf8len == -1) ? utf8str.size() : utf8len);
    }

    inline void convert(const CVolMem &utf8str, size_t utf8len = -1)
    {
        convert(reinterpret_cast<const char *>(utf8str.GetPtr()), (utf8len == -1) ? static_cast<size_t>(utf8str.GetSize()) : utf8len);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const wchar_t *c_str() const noexcept
    {
        return _data.GetText();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t size() const noexcept
    {
        return static_cast<size_t>(_data.GetLength());
    }

    /**
     * @brief 获取转换后的字节长度
     * @return 文本的字节长度
     */
    inline size_t size_bytes() const noexcept
    {
        return _data.GetLength() * 2;
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    inline wchar_t *data() noexcept
    {
        return _data.IsEmpty() ? nullptr : const_cast<wchar_t *>(_data.m_mem.GetTextPtr());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    inline bool empty() const noexcept
    {
        return _data.IsEmpty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return
     */
    inline CWString &ref() noexcept
    {
        return _data;
    }

    inline const CWString &cref() const noexcept
    {
        return _data;
    }

    /**
     * @brief 生成一个标准文本类,其中包含了转换后的文本数据
     * @return 转换后的std::basic_string
     */
    inline std::wstring to_str() noexcept
    {
        return std::wstring{_data.GetText()};
    }

    /**
     * @brief 生成一个火山的文本型,其中包含了转换后的文本数据
     * @return 返回所转换的文本型
     */
    inline CWString &to_volstr(CWString &str) const
    {
        str.SetText(_data.GetText());
        return str;
    }

    inline CWString &&to_volstr(CWString &&str) const
    {
        str.SetText(_data.GetText());
        return std::forward<CWString &&>(str);
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &to_volmem(CVolMem &mem, bool null_char = false) const
    {
        mem.Empty();
        mem.Append(_data.GetText(), (_data.GetLength() + (null_char ? 1 : 0)) * 2);
        return mem;
    }

    inline CVolMem &&to_volmem(CVolMem &&mem, bool null_char = false) const
    {
        mem.Empty();
        mem.Append(_data.GetText(), (_data.GetLength() + (null_char ? 1 : 0)) * 2);
        return std::forward<CVolMem &&>(mem);
    }
}; // PivU2Ws

/**
 * @brief 取指定编码的文本指针,只有CWString可能会产生编码转换,其他取指针输出.
 */
template <typename CharT>
class PivFromAny
{
private:
    std::unique_ptr<std::string> _data{nullptr};
    const CharT *_c_str = nullptr;

public:
    PivFromAny() = delete;

    ~PivFromAny() {}

    PivFromAny(const CWString &rhs)
    {
        PIV_IF(sizeof(CharT) == 2)
        {
            _c_str = (const CharT *)rhs.GetText();
        }
        else
        {
            _data.reset(new std::string{});
            piv::encoding::W2Uex(*_data, rhs.GetText(), rhs.GetLength());
        }
    }

    PivFromAny(const std::basic_string<CharT> &rhs)
    {
        _c_str = rhs.c_str();
    }

    PivFromAny(const CVolMem &rhs)
    {
        _c_str = static_cast<const CharT *>(rhs.GetPtr());
    }

    PivFromAny(const CharT *rhs)
    {
        _c_str = rhs;
    }

    /**
     * @brief 获取转换后的文本指针(至少返回空字符串)
     * @return 字符串指针
     */
    inline const CharT *c_str() const noexcept
    {
        if (_c_str)
            return _c_str;
        if (_data)
            return reinterpret_cast<const CharT *>(_data->c_str());
        return nullptr;
    }

    operator const CharT *() const noexcept
    {
        return this->c_str();
    }

    const CharT *operator*() const noexcept
    {
        return this->c_str();
    }
}; // PivFromAny

/**
 * @brief 任意编码转UTF-8的封装类(输出文本指针)
 */
class PivAny2U
{
private:
    std::unique_ptr<std::string> pStr{nullptr};
    std::unique_ptr<piv::string_view> pSv{nullptr};

public:
    PivAny2U() = delete;

    ~PivAny2U() {}

    /**
     * @brief 构造的同时将提供的文本转换
     * @param s 所欲转换的任意编码文本
     */
    PivAny2U(const char *rhs, size_t count = -1)
    {
        if (rhs == nullptr || count == 0)
            return;
        if (count == -1)
            pSv.reset(new piv::string_view{rhs});
        else
            pSv.reset(new piv::string_view{rhs, count});
    }

    PivAny2U(const wchar_t *rhs, size_t count = -1)
    {
        if (rhs == nullptr || count == 0)
            return;
        pStr.reset(new std::string{});
        piv::encoding::W2Uex(*pStr, rhs, count);
    }

    PivAny2U(const CWString &rhs)
    {
        if (rhs.IsEmpty())
            return;
        pStr.reset(new std::string{});
        piv::encoding::W2Uex(*pStr, rhs.GetText(), static_cast<size_t>(rhs.GetLength()));
    }

    PivAny2U(const CVolMem &rhs)
    {
        if (rhs.IsEmpty())
            return;
#ifdef SIMDUTF_H
        int encodings = simdutf::detect_encodings(reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize()));
        if ((encodings & 2) == 2) // UTF-16LE
        {
            pStr.reset(new std::string{});
            piv::encoding::W2Uex(*pStr, rhs.GetTextPtr(), static_cast<size_t>(rhs.GetSize()) / 2);
        }
        else if ((encodings & 1) == 1) // UTF-8
        {
            pSv.reset(new piv::string_view{reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize())});
        }
        else // ANSI
        {
            pStr.reset(new std::string{});
            piv::encoding::A2Uex(*pStr, reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize()));
        }
#else
        pSv.reset(new piv::string_view{reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize())});
#endif
    }

    PivAny2U(const std::string &rhs)
    {
        if (rhs.empty())
            return;
        pSv.reset(new piv::string_view{rhs});
    }

    PivAny2U(const std::wstring &rhs)
    {
        if (rhs.empty())
            return;
        pStr.reset(new std::string{});
        piv::encoding::W2Uex(*pStr, rhs.c_str(), rhs.size());
    }

    PivAny2U(const piv::string_view &rhs)
    {
        if (rhs.empty())
            return;
        pSv.reset(new piv::string_view{rhs});
    }

    PivAny2U(const piv::wstring_view &rhs)
    {
        if (rhs.empty())
            return;
        pStr.reset(new std::string{});
        piv::encoding::W2Uex(*pStr, rhs.data(), rhs.size());
    }

    PivAny2U(int32_t rhs)
    {
        char buf[16] = {'\0'};
        _ltoa(rhs, buf, 10);
        pStr.reset(new std::string{buf});
    }

    PivAny2U(int64_t rhs)
    {
        char buf[32] = {'\0'};
        _i64toa(rhs, buf, 10);
        pStr.reset(new std::string{buf});
    }

    /**
     * @brief 获取转换后的文本指针(至少返回空字符串)
     * @return 字符串指针
     */
    inline const char *c_str() const noexcept
    {
        if (pStr)
            return pStr->c_str();
        if (pSv)
            return pSv->data();
        return "";
    }

    /**
     * @brief 获取转换后的文本指针(可能会返回空指针)
     * @return 字符串指针
     */
    inline char *data() noexcept
    {
        if (pStr)
            return const_cast<char *>(pStr->data());
        if (pSv)
            return const_cast<char *>(pSv->data());
        return nullptr;
    }

    /**
     * @brief 获取转换后的尾字符指针(可能会返回空指针)
     * @return 尾字符指针
     */
    inline char *end_data() noexcept
    {
        if (pStr)
            return const_cast<char *>(pStr->data()) + pStr->size();
        if (pSv)
            return const_cast<char *>(pSv->data()) + pSv->size();
        return nullptr;
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t size() const noexcept
    {
        if (pStr)
            return pStr->size();
        if (pSv)
            return pSv->size();
        return 0;
    }

    operator const char *() const noexcept
    {
        return this->c_str();
    }

    const char *operator*() const noexcept
    {
        return this->c_str();
    }
}; // PivAny2U

/**
 * @brief 任意编码转UTF-8 std::string的封装类(输出std::string)
 */
class PivAny2Us
{
private:
    std::string _data;

public:
    PivAny2Us() = delete;

    ~PivAny2Us() {}

    /**
     * @brief 构造的同时将提供的文本转换
     * @param s 所欲转换的任意编码文本
     */
    PivAny2Us(const char *rhs, size_t count = -1)
    {
        if (rhs == nullptr || count == 0)
            return;
        if (count == -1)
            _data.assign(rhs);
        else
            _data.assign(rhs, count);
    }

    PivAny2Us(const wchar_t *rhs, size_t count = -1)
    {
        if (rhs == nullptr || count == 0)
            return;
        piv::encoding::W2Uex(_data, rhs, count);
    }

    PivAny2Us(const CWString &rhs)
    {
        if (rhs.IsEmpty())
            return;
        piv::encoding::W2Uex(_data, rhs.GetText(), static_cast<size_t>(rhs.GetLength()));
    }

    PivAny2Us(const CVolMem &rhs)
    {
        if (rhs.IsEmpty())
            return;
#ifdef SIMDUTF_H
        int encodings = simdutf::detect_encodings(reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize()));
        if ((encodings & 2) == 2) // UTF-16LE
        {
            piv::encoding::W2Uex(_data, rhs.GetTextPtr(), static_cast<size_t>(rhs.GetSize()) / 2);
        }
        else if ((encodings & 1) == 1) // UTF-8
        {
            _data.assign(reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize()));
        }
        else // ANSI
        {
            piv::encoding::A2Uex(_data, reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize()));
        }
#else
        _data.assign(reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize()));
#endif
    }

    PivAny2Us(const std::string &rhs)
    {
        if (rhs.empty())
            return;
        _data.assign(rhs);
    }

    PivAny2Us(std::string &&rhs)
    {
        _data = std::move(rhs);
    }

    PivAny2Us(const std::wstring &rhs)
    {
        if (rhs.empty())
            return;
        piv::encoding::W2Uex(_data, rhs.c_str(), rhs.size());
    }

    PivAny2Us(const piv::string_view &rhs)
    {
        if (rhs.empty())
            return;
        _data.assign(rhs.data(), rhs.size());
    }

    PivAny2Us(const piv::wstring_view &rhs)
    {
        if (rhs.empty())
            return;
        piv::encoding::W2Uex(_data, rhs.data(), rhs.size());
    }

    PivAny2Us(int32_t rhs)
    {
        char buf[16] = {'\0'};
        _ltoa(rhs, buf, 10);
        _data.assign(buf);
    }

    PivAny2Us(int64_t rhs)
    {
        char buf[32] = {'\0'};
        _i64toa(rhs, buf, 10);
        _data.assign(buf);
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    inline bool empty() const noexcept
    {
        return _data.empty();
    }

    /**
     * @brief 获取转换后的文本指针(至少返回空字符串)
     * @return 字符串指针
     */
    inline const char *c_str() const noexcept
    {
        return _data.c_str();
    }

    /**
     * @brief 获取转换后的文本指针(可能会返回空指针)
     * @return 字符串指针
     */
    inline char *data() noexcept
    {
        return _data.empty() ? nullptr : const_cast<char *>(_data.data());
    }

    /**
     * @brief 获取转换后的尾字符指针
     * @return 尾字符指针
     */
    inline char *end_data() noexcept
    {
        return const_cast<char *>(_data.data()) + _data.size();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t size() const noexcept
    {
        return _data.size();
    }

    operator std::string &() noexcept
    {
        return _data;
    }

    operator const std::string &() const noexcept
    {
        return _data;
    }

    const std::string &operator*() const noexcept
    {
        return _data;
    }

    std::string &operator*() noexcept
    {
        return _data;
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    inline std::string &ref() noexcept
    {
        return _data;
    }
    inline const std::string &cref() const noexcept
    {
        return _data;
    }
}; // PivAny2Us

/**
 * @brief 任意编码转UTF-8编码的std::string_view
 */
class PivS2V
{
private:
    piv::string_view _sv;
    std::unique_ptr<std::string> _data{nullptr};

public:
    PivS2V() = delete;

    ~PivS2V() {}

    /**
     * @brief 构造的同时将提供的文本转换
     * @param s 所欲转换的任意编码文本
     */
    PivS2V(const char *rhs, size_t count = -1)
    {
        _sv = (count == -1) ? piv::string_view{rhs} : piv::string_view{rhs, count};
    }

    PivS2V(const wchar_t *rhs, size_t count = -1)
    {
        _data.reset(new std::string{});
        piv::encoding::W2Uex(*_data, rhs, count);
        _sv = *_data;
    }

    PivS2V(const CWString &rhs)
    {
        _data.reset(new std::string{});
        piv::encoding::W2Uex(*_data, rhs.GetText(), static_cast<size_t>(rhs.GetLength()));
        _sv = *_data;
    }

    PivS2V(const CVolMem &rhs)
    {
#ifdef SIMDUTF_H
        int encodings = simdutf::detect_encodings(reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize()));
        if ((encodings & 2) == 2) // UTF-16LE
        {
            _data.reset(new std::string{});
            piv::encoding::W2Uex(*_data, rhs.GetTextPtr(), static_cast<size_t>(rhs.GetSize()) / 2);
            _sv = *_data;
        }
        else if ((encodings & 1) == 1) // UTF-8
        {
            _sv = piv::string_view{reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize())};
        }
        else // ANSI
        {
            _data.reset(new std::string{});
            piv::encoding::A2Uex(*_data, reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize()));
            _sv = *_data;
        }
#else
        _sv = piv::string_view{reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize())};
#endif
    }

    PivS2V(const std::string &rhs)
    {
        _sv = rhs;
    }

    PivS2V(const std::wstring &rhs)
    {
        _data.reset(new std::string{});
        piv::encoding::W2Uex(*_data, rhs.c_str(), rhs.size());
        _sv = *_data;
    }

    PivS2V(const piv::string_view &rhs)
    {
        _sv = rhs;
    }

    PivS2V(const piv::wstring_view &rhs)
    {
        _data.reset(new std::string{});
        piv::encoding::W2Uex(*_data, rhs.data(), rhs.size());
        _sv = *_data;
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const char *c_str() const noexcept
    {
        return _sv.data();
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline char *data() noexcept
    {
        return _sv.empty() ? nullptr : const_cast<char *>(_sv.data());
    }

    /**
     * @brief 获取转换后的尾字符指针
     * @return 尾字符指针
     */
    inline char *end_data() noexcept
    {
        return _sv.empty() ? nullptr : (const_cast<char *>(_sv.data()) + _sv.size());
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t size() const noexcept
    {
        return _sv.size();
    }

    operator const piv::string_view &() const noexcept
    {
        return _sv;
    }

    operator piv::string_view &() noexcept
    {
        return _sv;
    }

    const piv::string_view &operator*() const noexcept
    {
        return _sv;
    }

    piv::string_view &operator*() noexcept
    {
        return _sv;
    }
}; // PivS2V

/**
 * @brief 任意编码转ANSI编码的std::wstring_view
 */
class PivS2AV
{
private:
    piv::string_view _sv;
    std::unique_ptr<std::string> _data{nullptr};

public:
    PivS2AV() = delete;

    ~PivS2AV() {}

    /**
     * @brief 构造的同时将提供的文本转换
     * @param s 所欲转换的任意编码文本
     */
    PivS2AV(const char *rhs, size_t count = -1)
    {
        _sv = (count == -1) ? piv::string_view{rhs} : piv::string_view{rhs, count};
    }

    PivS2AV(const wchar_t *rhs, size_t count = -1)
    {
        _data.reset(new std::string{});
        piv::encoding::W2Aex(*_data, rhs, count);
        _sv = *_data;
    }

    PivS2AV(const CWString &rhs)
    {
        _data.reset(new std::string{});
        piv::encoding::W2Aex(*_data, rhs.GetText(), static_cast<size_t>(rhs.GetLength()));
        _sv = *_data;
    }

    PivS2AV(const CVolMem &rhs)
    {
#ifdef SIMDUTF_H
        int encodings = simdutf::detect_encodings(reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize()));
        if ((encodings & 2) == 2) // UTF-16LE
        {
            _data.reset(new std::string{});
            piv::encoding::W2Uex(*_data, rhs.GetTextPtr(), static_cast<size_t>(rhs.GetSize()) / 2);
            _sv = *_data;
        }
        else if ((encodings & 1) == 1) // UTF-8
        {
            _sv = piv::string_view{reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize())};
        }
        else // ANSI
        {
            _data.reset(new std::string{});
            piv::encoding::A2Uex(*_data, reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize()));
            _sv = *_data;
        }
#else
        _sv = piv::string_view{reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize())};
#endif
    }

    PivS2AV(const std::string &rhs)
    {
        _sv = rhs;
    }

    PivS2AV(const std::wstring &rhs)
    {
        _data.reset(new std::string{});
        piv::encoding::W2Aex(*_data, rhs.c_str(), rhs.size());
        _sv = *_data;
    }

    PivS2AV(const piv::string_view &rhs)
    {
        _sv = rhs;
    }

    PivS2AV(const piv::wstring_view &rhs)
    {
        _data.reset(new std::string{});
        piv::encoding::W2Aex(*_data, rhs.data(), rhs.size());
        _sv = *_data;
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const char *c_str() const noexcept
    {
        return _sv.data();
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline char *data() noexcept
    {
        return _sv.empty() ? nullptr : const_cast<char *>(_sv.data());
    }

    /**
     * @brief 获取转换后的尾字符指针
     * @return 尾字符指针
     */
    inline char *end_data() noexcept
    {
        return _sv.empty() ? nullptr : const_cast<char *>(_sv.data()) + _sv.size();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t size() const noexcept
    {
        return _sv.size();
    }

    operator const piv::string_view &() const noexcept
    {
        return _sv;
    }

    operator piv::string_view &() noexcept
    {
        return _sv;
    }

    const piv::string_view &operator*() const noexcept
    {
        return _sv;
    }

    piv::string_view &operator*() noexcept
    {
        return _sv;
    }
}; // PivS2AV

/**
 * @brief 任意编码转std::wstring_view
 */
class PivS2WV
{
private:
    piv::wstring_view _sv;
    std::unique_ptr<std::wstring> _data{nullptr};

public:
    PivS2WV() = delete;

    ~PivS2WV() {}

    /**
     * @brief 构造的同时将提供的文本转换
     * @param s 所欲转换的任意编码文本
     */
    PivS2WV(const wchar_t *rhs, size_t count = -1)
    {
        _sv = (count == -1) ? piv::wstring_view{rhs} : piv::wstring_view{rhs, count};
    }

    PivS2WV(const char *rhs, size_t count = -1)
    {
        _data.reset(new std::wstring{});
        piv::encoding::U2Wex(*_data, rhs, count);
        _sv = *_data;
    }

    PivS2WV(const CWString &rhs)
    {
        _sv = rhs.GetText();
    }

    PivS2WV(const CVolMem &rhs)
    {
#ifdef SIMDUTF_H
        int encodings = simdutf::detect_encodings(reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize()));
        if ((encodings & 2) == 2) // UTF-16LE
        {
            _sv = piv::wstring_view{rhs.GetTextPtr(), static_cast<size_t>(rhs.GetSize()) / 2};
        }
        else if ((encodings & 1) == 1) // UTF-8
        {
            _data.reset(new std::wstring{});
            piv::encoding::U2Wex(*_data, reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize()) / 2);
            _sv = *_data;
        }
        else // ANSI
        {
            _data.reset(new std::wstring{});
            piv::encoding::A2Wex(*_data, reinterpret_cast<const char *>(rhs.GetPtr()), static_cast<size_t>(rhs.GetSize()));
            _sv = *_data;
        }
#else
        _sv = piv::wstring_view{rhs.GetTextPtr(), static_cast<size_t>(rhs.GetSize()) / 2};
#endif
    }

    PivS2WV(const std::string &rhs)
    {
        _data.reset(new std::wstring{});
        piv::encoding::U2Wex(*_data, rhs.c_str(), rhs.size());
        _sv = *_data;
    }

    PivS2WV(const std::wstring &rhs)
    {
        _sv = rhs;
    }

    PivS2WV(const piv::string_view &rhs)
    {
        _data.reset(new std::wstring{});
        piv::encoding::U2Wex(*_data, rhs.data(), rhs.size());
        _sv = *_data;
    }

    PivS2WV(const piv::wstring_view &rhs)
    {
        _sv = rhs;
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const wchar_t *c_str() const noexcept
    {
        return _sv.data();
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline wchar_t *data() noexcept
    {
        return _sv.empty() ? nullptr : const_cast<wchar_t *>(_sv.data());
    }

    /**
     * @brief 获取转换后的尾字符指针
     * @return 尾字符指针
     */
    inline wchar_t *end_data() noexcept
    {
        return _sv.empty() ? nullptr : const_cast<wchar_t *>(_sv.data()) + _sv.size();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t size() const noexcept
    {
        return _sv.size();
    }

    operator const piv::wstring_view &() const noexcept
    {
        return _sv;
    }

    operator piv::wstring_view &() noexcept
    {
        return _sv;
    }

    const piv::wstring_view &operator*() const noexcept
    {
        return _sv;
    }

    piv::wstring_view &operator*() noexcept
    {
        return _sv;
    }
}; // PivS2WV

namespace piv
{
    namespace encoding
    {
        /**
         * @brief URL文本解码
         * @param str 所欲解码的文本
         * @param decoded 返回解码后的文本
         * @param utf8 是否为UTF-8编码
         */
        template <typename T>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, std::string>::value, T>::type
        UrlStrDecode(const ::piv::string_view &str, bool utf8, T &&decoded)
        {
            decoded.clear();
            if (str.size() == 0)
                return std::forward<T>(decoded);
            if (piv::encoding::UrlDecodeNeed(str.data(), str.size()))
            {
                size_t buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const unsigned char *>(str.data()), str.size());
                decoded.resize(buffer_len);
                piv::encoding::UrlDecode(reinterpret_cast<const unsigned char *>(str.data()), str.size(), const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(decoded.data())), buffer_len);
            }
            else
            {
                decoded.assign(str.data(), str.size());
            }
            return std::forward<T>(decoded);
        }

        template <typename T>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, std::wstring>::value, T>::type
        UrlStrDecode(const ::piv::wstring_view &str, bool utf8, T &&decoded)
        {
            decoded.clear();
            if (str.size() == 0)
                return std::forward<T>(decoded);
            if (piv::encoding::UrlDecodeNeed(str.data(), str.size()))
            {
                size_t buffer_len;
                std::string buffer;
                if (utf8)
                {
                    PivW2U urlstr(str.data(), str.size());
                    buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const unsigned char *>(urlstr.data()), urlstr.size_bytes());
                    buffer.resize(buffer_len);
                    piv::encoding::UrlDecode(reinterpret_cast<const unsigned char *>(urlstr.data()), urlstr.size_bytes(), const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(buffer.data())), buffer_len);
                    decoded.assign(PivU2W{buffer}.c_str());
                }
                else
                {
                    PivW2A urlstr(str.data(), str.size());
                    buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const unsigned char *>(urlstr.data()), urlstr.size_bytes());
                    buffer.resize(buffer_len);
                    piv::encoding::UrlDecode(reinterpret_cast<const unsigned char *>(urlstr.data()), urlstr.size_bytes(), const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(buffer.data())), buffer_len);
                    decoded.assign(PivA2W{buffer}.c_str());
                }
            }
            else
            {
                decoded.assign(str.data(), str.size());
            }
            return std::forward<T>(decoded);
        }

        template <typename CharT>
        std::basic_string<CharT> &UrlStrDecode(const std::basic_string<CharT> &str, bool utf8, std::basic_string<CharT> &decoded)
        {
            return piv::encoding::UrlStrDecode(::piv::basic_string_view<CharT>{str}, utf8, decoded);
        }

        template <typename CharT>
        std::basic_string<CharT> &&UrlStrDecode(const std::basic_string<CharT> &str, bool utf8, std::basic_string<CharT> &&decoded)
        {
            return std::forward<std::basic_string<CharT> &&>(piv::encoding::UrlStrDecode(::piv::basic_string_view<CharT>{str}, utf8, decoded));
        }

        template <typename T>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, CVolMem>::value, T>::type
        UrlStrDecode(const ::piv::string_view &str, bool utf8, T &&decoded)
        {
            decoded.Empty();
            if (str.size() == 0)
                return std::forward<T>(decoded);
            size_t buffer_len;
            if (piv::encoding::UrlDecodeNeed(str.data(), str.size()))
            {
                buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const unsigned char *>(str.data()), str.size());
                piv::encoding::UrlDecode(reinterpret_cast<const unsigned char *>(str.data()), str.size(), decoded.Alloc(static_cast<INT_P>(buffer_len), TRUE), buffer_len);
                return std::forward<T>(decoded);
            }
            decoded.Append(reinterpret_cast<const void *>(str.data()), str.size());
            return std::forward<T>(decoded);
        }

        template <typename T>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, CVolMem>::value, T>::type
        UrlStrDecode(const ::piv::wstring_view &str, bool utf8, T &&decoded)
        {
            decoded.Empty();
            if (str.size() == 0)
                return std::forward<T>(decoded);
            size_t buffer_len;
            if (piv::encoding::UrlDecodeNeed(str.data(), str.size()))
            {
                std::string buffer;
                if (utf8)
                {
                    PivW2U urlstr(str.data(), str.size());
                    buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const unsigned char *>(urlstr.data()), urlstr.size_bytes());
                    buffer.resize(buffer_len);
                    piv::encoding::UrlDecode(reinterpret_cast<const unsigned char *>(urlstr.data()), urlstr.size_bytes(), const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(buffer.data())), buffer_len);

                    return std::forward<T>(PivU2W{buffer}.to_volmem(decoded));
                }
                else
                {
                    PivW2A urlstr(str.data(), str.size());
                    buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const unsigned char *>(urlstr.data()), urlstr.size_bytes());
                    buffer.resize(buffer_len);
                    piv::encoding::UrlDecode(reinterpret_cast<const unsigned char *>(urlstr.data()), urlstr.size_bytes(), const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(buffer.data())), buffer_len);
                    return std::forward<T>(PivA2W{buffer}.to_volmem(decoded));
                }
            }
            decoded.Append(reinterpret_cast<const void *>(str.data()), str.size() * sizeof(wchar_t));
            return std::forward<T>(decoded);
        }

        template <typename CharT, typename T>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, CVolMem>::value, T>::type
        UrlStrDecode(const std::basic_string<CharT> &str, bool utf8, T &&decoded)
        {
            return std::forward<T>(piv::encoding::UrlStrDecode(::piv::basic_string_view<CharT>{str.c_str()}, utf8, decoded));
        }

        template <typename T>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, CWString>::value, T>::type
        UrlStrDecode(const ::piv::string_view &str, bool utf8, T &&decoded)
        {
            decoded.Empty();
            if (str.size() == 0)
                return std::forward<T>(decoded);
            if (piv::encoding::UrlDecodeNeed(str.data(), str.size()))
            {
                std::string buffer;
                size_t buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const unsigned char *>(str.data()), str.size());
                buffer.resize(buffer_len);
                piv::encoding::UrlDecode(reinterpret_cast<const unsigned char *>(str.data()), str.size(), const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(buffer.data())), buffer_len);
                if (utf8)
                    return std::forward<T>(PivU2Ws{buffer}.to_volstr(decoded));
                else
                    return std::forward<T>(PivA2Ws{buffer}.to_volstr(decoded));
            }
            else
            {
                if (utf8)
                    return std::forward<T>(PivU2Ws{reinterpret_cast<const char *>(str.data()), str.size()}.to_volstr(decoded));
                else
                    return std::forward<T>(PivA2Ws{reinterpret_cast<const char *>(str.data()), str.size()}.to_volstr(decoded));
            }
        }

        template <typename T>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, CWString>::value, T>::type
        UrlStrDecode(const ::piv::wstring_view &str, bool utf8, T &&decoded)
        {
            decoded.Empty();
            if (str.size() == 0)
                return std::forward<T>(decoded);
            if (piv::encoding::UrlDecodeNeed(str.data(), str.size()))
            {
                std::string buffer;
                if (utf8)
                {
                    PivW2U urlstr(str.data(), str.size());
                    size_t buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const unsigned char *>(urlstr.data()), urlstr.size_bytes());
                    buffer.resize(buffer_len);
                    piv::encoding::UrlDecode(reinterpret_cast<const unsigned char *>(urlstr.data()), urlstr.size_bytes(), const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(buffer.data())), buffer_len);
                    return std::forward<T>(PivU2Ws{buffer}.to_volstr(decoded));
                }
                else
                {
                    PivW2A urlstr(str.data(), str.size());
                    size_t buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const unsigned char *>(urlstr.data()), urlstr.size_bytes());
                    buffer.resize(buffer_len);
                    piv::encoding::UrlDecode(reinterpret_cast<const unsigned char *>(urlstr.data()), urlstr.size_bytes(), const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(buffer.data())), buffer_len);
                    return std::forward<T>(PivA2Ws{buffer}.to_volstr(decoded));
                }
            }
            decoded.SetText(str.data(), static_cast<INT_P>(str.size()));
            return std::forward<T>(decoded);
        }

        template <typename T>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, CWString>::value, T>::type
        UrlStrDecode(const CVolMem &str, bool utf8, T &&decoded)
        {
            return std::forward<T>(piv::encoding::UrlStrDecode(::piv::string_view{reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())}, utf8, decoded));
        }

        /**
         * @brief URL文本编码
         * @param str 所欲编码的文本
         * @param encoded 返回编码后的文本
         * @param utf8 是否为UTF-8编码
         * @param ReservedWord 是否不编码保留字符
         */
        template <typename T>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, std::string>::value, T>::type
        UrlStrEncode(const ::piv::string_view &str, bool utf8, bool ReservedWord, T &&encoded)
        {
            if (piv::encoding::UrlEncodeNeed(str.data(), str.size(), ReservedWord))
            {
                size_t buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const unsigned char *>(str.data()), str.size(), ReservedWord);
                encoded.resize(buffer_len);
                piv::encoding::UrlEncode(reinterpret_cast<const unsigned char *>(str.data()), str.size(), const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(encoded.data())), buffer_len, ReservedWord);
                encoded.resize(buffer_len);
            }
            else
            {
                encoded.assign(str.data(), str.size());
            }
            return std::forward<T>(encoded);
        }

        template <typename T>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, std::wstring>::value, T>::type
        UrlStrEncode(const ::piv::wstring_view &str, bool utf8, bool ReservedWord, T &&encoded)
        {
            if (piv::encoding::UrlEncodeNeed(str.data(), str.size(), ReservedWord))
            {
                CVolMem buffer;
                size_t buffer_len;
                if (utf8)
                {
                    PivW2U text{str.data(), str.size()};
                    buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const unsigned char *>(text.data()), text.size_bytes(), ReservedWord);
                    CVolMem buffer;
                    piv::encoding::UrlEncode(reinterpret_cast<const unsigned char *>(text.data()), text.size_bytes(), buffer.Alloc(static_cast<INT_P>(buffer_len), TRUE), buffer_len, ReservedWord);
                    PivU2W urled{buffer, buffer_len};
                    encoded.assign(urled.c_str(), urled.size());
                }
                else
                {
                    PivW2A text{str.data(), str.size()};
                    buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const unsigned char *>(text.data()), text.size_bytes(), ReservedWord);
                    CVolMem buffer;
                    piv::encoding::UrlEncode(reinterpret_cast<const unsigned char *>(text.data()), text.size_bytes(), buffer.Alloc(static_cast<INT_P>(buffer_len), TRUE), buffer_len, ReservedWord);
                    PivA2W urled{buffer, buffer_len};
                    encoded.assign(urled.c_str(), urled.size());
                }
            }
            else
            {
                encoded.assign(str.data(), str.size());
            }
            return std::forward<T>(encoded);
        }

        template <typename CharT>
        std::basic_string<CharT> &UrlStrEncode(const std::basic_string<CharT> &str, bool utf8, bool ReservedWord, std::basic_string<CharT> &encoded)
        {
            return piv::encoding::UrlStrEncode(::piv::basic_string_view<CharT>{str.c_str()}, utf8, ReservedWord, encoded);
        }

        template <typename CharT>
        std::basic_string<CharT> &&UrlStrEncode(const std::basic_string<CharT> &str, bool utf8, bool ReservedWord, std::basic_string<CharT> &&encoded)
        {
            return std::forward<std::basic_string<CharT> &&>(piv::encoding::UrlStrEncode(::piv::basic_string_view<CharT>{str.c_str()}, utf8, ReservedWord, encoded));
        }

        template <typename T>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, CVolMem>::value, T>::type
        UrlDataEncode(const ::piv::string_view &str, bool utf8, bool ReservedWord, T &&encoded)
        {
            encoded.Empty();
            if (piv::encoding::UrlEncodeNeed(str.data(), str.size(), ReservedWord))
            {
                size_t buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const unsigned char *>(str.data()), str.size(), ReservedWord);
                piv::encoding::UrlEncode(reinterpret_cast<const unsigned char *>(str.data()), str.size(), encoded.Alloc(static_cast<INT_P>(buffer_len), TRUE), buffer_len, ReservedWord);
                encoded.Realloc(buffer_len);
                return std::forward<T>(encoded);
            }
            else
            {
                encoded.Append(reinterpret_cast<const void *>(str.data()), str.size() * 2);
            }
            return std::forward<T>(encoded);
        }

        template <typename T>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, CVolMem>::value, T>::type
        UrlDataEncode(const ::piv::wstring_view &str, bool utf8, bool ReservedWord, T &&encoded)
        {
            encoded.Empty();
            if (piv::encoding::UrlEncodeNeed(str.data(), str.size(), ReservedWord))
            {
                CVolMem buffer;
                size_t buffer_len;
                if (utf8)
                {
                    PivW2U text{str.data(), str.size()};
                    buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const unsigned char *>(text.data()), text.size_bytes(), ReservedWord);
                    piv::encoding::UrlEncode(reinterpret_cast<const unsigned char *>(text.data()), text.size_bytes(), buffer.Alloc(static_cast<INT_P>(buffer_len), TRUE), buffer_len, ReservedWord);
                    return std::forward<T>(PivU2W{buffer, buffer_len}.to_volmem(encoded));
                }
                else
                {
                    PivW2A text{str.data(), str.size()};
                    buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const unsigned char *>(text.data()), text.size_bytes(), ReservedWord);
                    piv::encoding::UrlEncode(reinterpret_cast<const unsigned char *>(text.data()), text.size_bytes(), buffer.Alloc(static_cast<INT_P>(buffer_len), TRUE), buffer_len, ReservedWord);
                    return std::forward<T>(PivA2W{buffer, buffer_len}.to_volmem(encoded));
                }
            }
            else
            {
                encoded.Append(reinterpret_cast<const void *>(str.data()), str.size() * 2);
            }
            return std::forward<T>(encoded);
        }

        template <typename CharT, typename T>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, CVolMem>::value, T>::type
        UrlDataEncode(const std::basic_string<CharT> &str, bool utf8, bool ReservedWord, T &&encoded)
        {
            return std::forward<T>(piv::encoding::UrlDataEncode(std::basic_string<CharT>{str.c_str()}, utf8, ReservedWord, encoded));
        }

        template <typename T>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, CVolMem>::value, T>::type
        UrlDataEncode(const CWString &str, bool utf8, bool ReservedWord, T &&encoded)
        {
            return std::forward<T>(piv::encoding::UrlDataEncode(::piv::wstring_view(str.GetText()), utf8, ReservedWord, encoded));
        }

        template <typename T>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, CVolMem>::value, T>::type
        UrlDataEncode(const CVolMem &str, bool utf8, bool ReservedWord, T &&encoded)
        {
            return std::forward<T>(piv::encoding::UrlDataEncode(::piv::string_view(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())), utf8, ReservedWord, encoded));
        }

        template <typename T>
        typename std::enable_if<std::is_same<typename std::remove_reference<T>::type, CWString>::value, T>::type
        UrlStrEncode(const CVolMem &str, T &&encoded, bool utf8 = true, bool ReservedWord = false)
        {
            if (piv::encoding::UrlEncodeNeed(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize()), ReservedWord))
            {
                CVolMem buffer;
                piv::encoding::UrlDataEncode<char>(::piv::string_view(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())), utf8, ReservedWord, buffer);
                if (utf8)
                    return std::forward<T>(PivU2Ws{buffer}.to_volstr(encoded));
                else
                    return std::forward<T>(PivA2Ws{buffer}.to_volstr(encoded));
            }
            else
            {
                if (utf8)
                    return std::forward<T>(PivU2Ws{str}.to_volstr(encoded));
                else
                    return std::forward<T>(PivA2Ws{str}.to_volstr(encoded));
            }
            encoded.Empty();
            return std::forward<T>(encoded);
        }

    } // namespace encoding

    namespace detail
    {
        struct base64_table
        {
            static constexpr char *encode =
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "0123456789+/";
            static constexpr unsigned char decode[128] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                // 0x00..0x0F
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                // 0x10..0x1F
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63,              // 0x20..0x2F
                52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0,      // 0x30..0x3F
                0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,           // 0x40..0x4F
                15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0,     // 0x50..0x5F
                0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, // 0x60..0x6F
                41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0,     // 0x70..0x7F
            };
        };

        struct base85_table
        {
            static constexpr char *encode =
                "0123456789"
                "abcdefghijklmnopqrstuvwxyz"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                ".-:+=^!/*?&<>()[]{}@%$#";
            static constexpr unsigned char decode[128] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                 // 0x00..0x0F
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                 // 0x10..0x1F
                0, 68, 0, 84, 83, 82, 72, 0, 75, 76, 70, 65, 0, 63, 62, 69,     // 0x20..0x2F
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 64, 0, 73, 66, 74, 71,            // 0x30..0x3F
                81, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, // 0x40..0x4F
                51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 77, 0, 78, 67, 0,   // 0x50..0x5F
                0, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,  // 0x60..0x6F
                25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 79, 0, 80, 0, 0,    // 0x70..0x7F
            };
        };

        struct base91_table
        {
            static constexpr char *encode =
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "0123456789!#$%&()*+,./:;-="
                "\\?@[]^_`{|}~\'";
            static constexpr unsigned char decode[256] = {
                91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, // 000..015
                91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, // 016..031
                91, 62, 91, 63, 64, 65, 66, 90, 67, 68, 69, 70, 71, 76, 72, 73, // 032..047 // @34: ", @39: ', @45: -
                52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 74, 75, 91, 77, 91, 79, // 048..063 // @60: <, @62: >
                80, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,           // 064..079
                15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 81, 78, 82, 83, 84, // 080..095 // @92: slash
                85, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, // 096..111
                41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 86, 87, 88, 89, 91, // 112..127
                91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, // 128..143
                91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, // 144..159
                91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, // 160..175
                91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, // 176..191
                91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, // 192..207
                91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, // 208..223
                91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, // 224..239
                91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91  // 240..255
            };
        };
    } // namespace detail

    /**
     * @brief BASE64编解码类
     * @tparam CharT 字符类型
     */
    template <typename CharT, typename TableT = detail::base64_table>
    class base64
    {
    private:
        template <typename T>
        inline void _AddChar(T &lhs, const unsigned char c, int32_t &count, int32_t max_line_len) noexcept
        {
            if (count < max_line_len)
            {
                edit::push_back(lhs, c);
                count++;
            }
            else
            {
                edit::push_back(lhs, '\r');
                edit::push_back(lhs, '\n');
                edit::push_back(lhs, c);
                count = 1;
            }
        }

    public:
        /**
         * @brief 将提供的数据编码为BASE64文本
         * @param input 所欲转换的数据
         * @param len 所欲转换数据的长度
         * @return 返回编码后的BASE64文本
         */
        template <typename T>
        inline T &encode(const unsigned char *input, size_t inlen, T &output, int32_t line_len = 0)
        {
            edit::clear(output);
            int32_t count = 0;
            line_len = (line_len < 0) ? (uint32_t)-1 : ((line_len == 0) ? 76 : line_len);
            unsigned char num = inlen % 3;
            edit::reserve(output, inlen / 3 * 4);
            for (int i = 0; i < inlen - num; i += 3)
            {
                _AddChar(output, TableT::encode[input[i] >> 2], count, line_len);
                _AddChar(output, TableT::encode[(input[i] & 0x03) << 4 | (input[i + 1] >> 4)], count, line_len);
                _AddChar(output, TableT::encode[(input[i + 1] & 0x0f) << 2 | (input[i + 2] >> 6)], count, line_len);
                _AddChar(output, TableT::encode[input[i + 2] & 0x3f], count, line_len);
            }
            if (num == 1)
            {
                _AddChar(output, TableT::encode[input[inlen - 1] >> 2], count, line_len);
                _AddChar(output, TableT::encode[(input[inlen - 1] & 0x03) << 4], count, line_len);
                _AddChar(output, '=', count, line_len);
                _AddChar(output, '=', count, line_len);
            }
            else if (num == 2)
            {
                _AddChar(output, TableT::encode[input[inlen - 2] >> 2], count, line_len);
                _AddChar(output, TableT::encode[(input[inlen - 2] & 0x03) << 4 | (input[inlen - 1] >> 4)], count, line_len);
                _AddChar(output, TableT::encode[(input[inlen - 1] & 0x0f) << 2], count, line_len);
                _AddChar(output, '=', count, line_len);
            }
            return output;
        }

        inline std::basic_string<CharT> &encode(const basic_string_view<CharT> &input, std::basic_string<CharT> &output, int line_len = 0)
        {
            return encode<std::basic_string<CharT>>(reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT), output, line_len);
        }

        inline std::basic_string<CharT> &encode(const std::basic_string<CharT> &input, std::basic_string<CharT> &output, int line_len = 0)
        {
            return encode<std::basic_string<CharT>>(reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT), output, line_len);
        }

        inline std::basic_string<CharT> &encode(const CVolMem &input, std::basic_string<CharT> &output, int line_len = 0)
        {
            return encode<std::basic_string<CharT>>(reinterpret_cast<const unsigned char *>(input.GetPtr()), static_cast<size_t>(input.GetSize()), output, line_len);
        }

        inline CWString &Encode(const unsigned char *input, size_t len, CWString &output, int line_len = 0)
        {
            return encode<CWString>(input, len, output, line_len);
        }

        inline CWString &Encode(const ::piv::basic_string_view<CharT> &input, CWString &output, int line_len = 0)
        {
            return encode<CWString>(reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT), output, line_len);
        }

        inline CWString &Encode(const std::basic_string<CharT> &input, CWString &output, int line_len = 0)
        {
            return encode<CWString>(reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT), output, line_len);
        }

        inline CWString &Encode(const CVolMem &input, CWString &output, int line_len = 0)
        {
            return encode<CWString>(reinterpret_cast<const unsigned char *>(input.GetPtr()), static_cast<size_t>(input.GetSize()), output, line_len);
        }

        /**
         * @brief 将提供的BASE64编码文本解码
         * @param input 所欲转换的BASE64编码
         * @return 返回解码后的数据
         */
        template <typename T>
        inline T &decode(const CharT *input, T &output, size_t length = -1)
        {
            edit::clear(output);
            if (length == -1)
                length = edit::size(input);
            int i = 0, j = 0;
            CharT temp[4];
            for (i = 0; i < length; i++)
            {
                if (input[i] == '\r' || input[i] == '\n')
                    continue;
                temp[j] = input[i];
                if (j++ == 3)
                {
                    edit::push_back(output, static_cast<unsigned char>((TableT::decode[(unsigned char)temp[0]] << 2) | (TableT::decode[(unsigned char)temp[1]] >> 4)));
                    edit::push_back(output, static_cast<unsigned char>((TableT::decode[(unsigned char)temp[1]] << 4) | (TableT::decode[(unsigned char)temp[2]] >> 2)));
                    edit::push_back(output, static_cast<unsigned char>((TableT::decode[(unsigned char)temp[2]] << 6) | (TableT::decode[(unsigned char)temp[3]])));
                    j = 0;
                }
            }
            if (j != 0)
            {
                if (j < 3)
                    temp[3] = '=';
                if (j < 2)
                    temp[2] = '=';
                edit::push_back(output, static_cast<unsigned char>((TableT::decode[(unsigned char)temp[0]] << 2) | (TableT::decode[(unsigned char)temp[1]] >> 4)));
                edit::push_back(output, static_cast<unsigned char>((TableT::decode[(unsigned char)temp[1]] << 4) | (TableT::decode[(unsigned char)temp[2]] >> 2)));
                edit::push_back(output, static_cast<unsigned char>((TableT::decode[(unsigned char)temp[2]] << 6) | (TableT::decode[(unsigned char)temp[3]])));
            }
            return output;
        }

        inline CVolMem &decode(const ::piv::basic_string_view<CharT> &input, CVolMem &output)
        {
            return decode(input.data(), output, input.size());
        }

        inline CVolMem &decode(const std::basic_string<CharT> &input, CVolMem &output)
        {
            return decode(input.c_str(), output, input.size());
        }

        inline CVolMem &decode(const CVolMem &input, CVolMem &output)
        {
            return decode(reinterpret_cast<const CharT *>(input.GetPtr()), output, static_cast<size_t>(input.GetSize()) / sizoef(CharT));
        }

        inline CVolMem &decode(const CWString &input, CVolMem &output)
        {
            return decode(input.GetText(), output, static_cast<size_t>(input.GetLength()));
        }

        template <typename T>
        inline std::basic_string<CharT> decode(const T &input)
        {
            CVolMem output;
            decode(input, output);
            return std::basic_string<CharT>{reinterpret_cast<const CharT *>(output.GetPtr()), output.GetSize() * sizeof(CharT)};
        }

        template <typename T>
        inline CVolMem Decode(const T &input)
        {
            CVolMem output;
            return decode(input, output);
        }

    }; // base64

    /**
     * @brief BASE85编解码类
     * @tparam CharT 字符类型
     */
    template <typename CharT, typename TableT = detail::base85_table>
    class base85
    {
    public:
        /**
         * @brief 将提供的数据编码为BASE85文本
         * @param input 所欲转换的数据
         * @param len 数据长度
         * @param output 转换的BASE85文本
         * @return 是否成功
         */
        template <typename T>
        inline bool encode(const unsigned char *input, size_t len, T &output)
        {
            edit::clear(output);
            if (len % 4)
                return false; // error: raw string size must be multiple of 4
            for (size_t o = 0; o < len * 5 / 4; input += 4, o += 5)
            {
                unsigned int value = (input[0] << 24) | (input[1] << 16) | (input[2] << 8) | input[3];
                edit::push_back(output, TableT::encode[(value / 0x31C84B1) % 0x55]);
                edit::push_back(output, TableT::encode[(value / 0x95EED) % 0x55]);
                edit::push_back(output, TableT::encode[(value / 0x1C39) % 0x55]);
                edit::push_back(output, TableT::encode[(value / 0x55) % 0x55]);
                edit::push_back(output, TableT::encode[value % 0x55]);
            }
            return true;
        }

        inline std::basic_string<CharT> encode(const ::piv::basic_string_view<CharT> &input, bool padding = false)
        {
            std::basic_string<CharT> output;
            size_t mod = (input.size() * sizeof(CharT)) % 4;
            if (mod == 0)
            {
                encode(reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT), output);
            }
            else if (padding)
            {
                std::basic_string<CharT> pad{input.data(), input.size()};
                pad.append((4 - mod) / sizeof(CharT), '\0');
                encode(reinterpret_cast<const unsigned char *>(pad.data()), pad.size() * sizeof(CharT), output);
            }
            return output;
        }

        inline std::basic_string<CharT> encode(const std::basic_string<CharT> &input, bool padding = false)
        {
            return encode(::piv::basic_string_view<CharT>{input}, padding);
        }

        inline std::basic_string<CharT> encode(const CVolMem &input, bool padding = false)
        {
            return encode(::piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(input.GetPtr()), static_cast<size_t>(input.GetSize()) / sizeof(CharT)}, padding);
        }

        inline CWString Encode(const CVolMem &input, bool padding = false)
        {
            CWString output;
            INT_P mod = input.GetSize() % 4;
            if (mod == 0)
            {
                encode<CWString>(reinterpret_cast<const unsigned char *>(input.GetPtr()), static_cast<size_t>(input.GetSize()), output);
            }
            else if (padding)
            {
                CVolMem pad = input;
                pad.AddManyU8Chars('\0', 4 - mod);
                encode<CWString>(reinterpret_cast<const unsigned char *>(pad.GetPtr()), static_cast<size_t>(pad.GetSize()), output);
            }
            return output;
        }

        inline CWString Encode(const std::basic_string<CharT> &input, bool padding = false)
        {
            return Encode(CVolMem(input.c_str(), input.size() * sizeof(CharT)), padding);
        }

        inline CWString Encode(const ::piv::basic_string_view<CharT> &input, bool padding = false)
        {
            return Encode(CVolMem(input.data(), input.size() * sizeof(CharT)), padding);
        }

        /**
         * @brief 将提供的BASE85编码文本解码
         * @param input 所欲转换的BASE85编码
         * @return 返回解码后的数据
         */
        template <typename CharT, typename T>
        inline bool decode(const CharT *input, T &output, size_t length = -1)
        {
            if (length == -1)
                length = edit::size(input);
            if (length % 5)
                return false; // error: z85 string size must be multiple of 5
            edit::reserve(output, length * 4 / 5);
            for (size_t o = 0; o < length * 4 / 5; input += 5, o += 4)
            {
                uint32_t value = TableT::decode[input[0]] * 0x31C84B1 + TableT::decode[input[1]] * 0x95EED +
                                 TableT::decode[input[2]] * 0x1C39 + TableT::decode[input[3]] * 0x55 + TableT::decode[input[4]];
                edit::push_back(output, static_cast<unsigned char>((value >> 24) & 0xFF));
                edit::push_back(output, static_cast<unsigned char>((value >> 16) & 0xFF));
                edit::push_back(output, static_cast<unsigned char>((value >> 8) & 0xFF));
                edit::push_back(output, static_cast<unsigned char>((value >> 0) & 0xFF));
            }
            return true;
        }

        inline bool decode(const ::piv::basic_string_view<CharT> &input, CVolMem &output)
        {
            return decode(input.data(), output, input.size());
        }

        inline bool decode(const std::basic_string<CharT> &input, CVolMem &output)
        {
            return decode(input.c_str(), output, input.size());
        }

        inline bool decode(const CVolMem &input, CVolMem &output)
        {
            return decode(reinterpret_cast<const CharT *>(input.GetPtr()), output, static_cast<size_t>(input.GetSize()) / sizoef(CharT));
        }

        inline bool decode(const CWString &input, CVolMem &output)
        {
            return decode(input.GetText(), output, static_cast<size_t>(input.GetLength()));
        }

        template <typename T>
        inline std::basic_string<CharT> decode(const T &input)
        {
            CVolMem buffer;
            decode(input, buffer);
            return std::basic_string<CharT>{reinterpret_cast<const CharT *>(buffer.GetPtr()), static_cast<size_t>(buffer.GetSize()) / sizeof(CharT)};
        }

        template <typename T>
        inline CVolMem Decode(const T &input)
        {
            CVolMem buffer;
            decode(input, buffer);
            return buffer;
        }

    }; // base85

    /**
     * @brief BASE91编解码类
     * @tparam CharT 字符类型
     */
    template <typename CharT, typename TableT = detail::base91_table>
    class base91
    {
    public:
        /**
         * @brief 将提供的数据编码为BASE91文本
         * @param input 所欲转换的数据
         * @param len 所欲转换数据的长度
         * @return 返回编码后的BASE91文本
         */
        template <typename T>
        inline T &encode(T &output, const unsigned char *input, size_t len)
        {
            edit::clear(output);
            unsigned long queue = 0;
            unsigned int nbits = 0;

            for (size_t length = len; length--;)
            {
                queue |= *input++ << nbits;
                nbits += 8;
                if (nbits > 13)
                { /* enough bits in queue */
                    unsigned int val = queue & 8191;
                    if (val > 88)
                    {
                        queue >>= 13;
                        nbits -= 13;
                    }
                    else
                    { /* we can take 14 bits */
                        val = queue & 16383;
                        queue >>= 14;
                        nbits -= 14;
                    }
                    edit::push_back(output, TableT::encode[val % 91]);
                    edit::push_back(output, TableT::encode[val / 91]);
                }
            }
            /* process remaining bits from bit queue; write up to 2 bytes */
            if (nbits)
            {
                edit::push_back(output, TableT::encode[queue % 91]);
                if (nbits > 7 || queue > 90)
                    edit::push_back(output, TableT::encode[queue / 91]);
            }
            return output;
        }

        inline std::basic_string<CharT> &encode(std::basic_string<CharT> &output, const ::piv::basic_string_view<CharT> &input)
        {
            return encode(output, reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT));
        }

        inline std::basic_string<CharT> encode(const ::piv::basic_string_view<CharT> &input)
        {
            std::basic_string<CharT> output;
            return encode(output, reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT));
        }

        inline std::basic_string<CharT> &encode(std::basic_string<CharT> &output, const std::basic_string<CharT> &input)
        {
            return encode(output, reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT));
        }

        inline std::basic_string<CharT> encode(const std::basic_string<CharT> &input)
        {
            std::basic_string<CharT> output;
            return encode(output, reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT));
        }

        inline std::basic_string<CharT> &encode(std::basic_string<CharT> &output, const CVolMem &input)
        {
            return encode(output, reinterpret_cast<const unsigned char *>(input.GetPtr()), static_cast<size_t>(input.GetSize()));
        }

        inline std::basic_string<CharT> encode(const CVolMem &input)
        {
            std::basic_string<CharT> output;
            return encode(output, reinterpret_cast<const unsigned char *>(input.GetPtr()), static_cast<size_t>(input.GetSize()));
        }

        inline CWString &Encode(CWString &output, const ::piv::basic_string_view<CharT> &input)
        {
            return encode(output, reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT));
        }

        inline CWString Encode(const ::piv::basic_string_view<CharT> &input)
        {
            CWString output;
            return encode(output, reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT));
        }

        inline CWString &Encode(CWString &output, const std::basic_string<CharT> &input)
        {
            return encode(output, reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT));
        }

        inline CWString Encode(const std::basic_string<CharT> &input)
        {
            CWString output;
            return encode(output, reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT));
        }

        inline CWString &Encode(CWString &output, const CVolMem &input)
        {
            return encode(output, reinterpret_cast<const unsigned char *>(input.GetPtr()), static_cast<size_t>(input.GetSize()));
        }

        inline CWString Encode(const CVolMem &input)
        {
            CWString output;
            return encode(output, reinterpret_cast<const unsigned char *>(input.GetPtr()), static_cast<size_t>(input.GetSize()));
        }

        /**
         * @brief 将提供的BASE91编码文本解码
         * @param input 所欲转换的BASE91编码
         * @return 返回解码后的数据
         */
        template <typename T>
        inline void decode(const CharT *input, T &output, size_t length = -1)
        {
            edit::clear(output);
            size_t cbInput = ((length == -1) ? edit::size(input) : length) * sizeof(CharT);
            if (cbInput <= 0)
                return;
            edit::reserve(output, cbInput * 6 / 4);

            unsigned long queue = 0;
            unsigned int nbits = 0;
            int val = -1;
            const unsigned char *ib = reinterpret_cast<const unsigned char *>(input);

            for (size_t i = cbInput; i--;)
            {
                unsigned char d = TableT::decode[*ib++];
                if (d == 91)
                    continue; /* ignore non-alphabet chars */
                if (val == -1)
                    val = d; /* start next value */
                else
                {
                    val += d * 91;
                    queue |= val << nbits;
                    nbits += (val & 8191) > 88 ? 13 : 14;
                    do
                    {
                        edit::push_back(output, static_cast<unsigned char>(queue));
                        queue >>= 8;
                        nbits -= 8;
                    } while (nbits > 7);
                    val = -1; /* mark value complete */
                }
            }
            /* process remaining bits; write at most 1 byte */
            if (val != -1)
                edit::push_back(output, static_cast<unsigned char>(queue | val << nbits));
        }

        inline void decode(const ::piv::basic_string_view<CharT> &input, CVolMem &output)
        {
            decode(input.data(), output, input.size());
        }

        inline void decode(const std::basic_string<CharT> &input, CVolMem &output)
        {
            decode(input.c_str(), output, input.size());
        }

        inline void decode(const ::piv::basic_string_view<CharT> &input, std::string &output)
        {
            decode(input.data(), output, input.size());
        }

        inline void decode(const std::basic_string<CharT> &input, std::string &output)
        {
            decode(input.c_str(), output, input.size());
        }

        inline void decode(const CVolMem &input, CVolMem &output)
        {
            decode(reinterpret_cast<const CharT *>(input.GetPtr()), output, input.GetSize() / sizeof(CharT));
        }

        inline void decode(const CWString &input, CVolMem &output)
        {
            decode(input.GetText(), output, static_cast<size_t>(input.GetLength()));
        }

        inline void decode(const CU8String &input, CVolMem &output)
        {
            decode(rinput.GetText(), output, static_cast<size_t>(input.GetLength()));
        }

        template <typename T>
        inline std::basic_string<CharT> decode(const T &input)
        {
            CVolMem buffer;
            decode(input, buffer);
            return std::basic_string<CharT>{reinterpret_cast<const CharT *>(buffer.GetPtr()), static_cast<size_t>(buffer.GetSize()) / sizeof(CharT)};
        }

        template <typename T>
        inline CVolMem Decode(const T &input)
        {
            CVolMem buffer;
            decode(input, buffer);
            return buffer;
        }
    }; // base91
} // namespace piv

#endif // _PIV_ENCODING_HPP
