/*********************************************\
 * 火山视窗PIV模块 - 文本编码辅助            *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef _PIV_ENCODING_HPP
#define _PIV_ENCODING_HPP

// 包含火山视窗基本类,它在火山里的包含顺序比较靠前(全局-110)
#ifndef __VOL_BASE_H__
#include <sys/base/libs/win_base/vol_base.h>
#endif

#include <algorithm>
#include <string>
#if defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L)
#include <string_view>
#define PIV_IF if constexpr
#define PIV_ELSE_IF else if constexpr
#else
#include "detail/string_view.hpp"
#define PIV_IF if
#define PIV_ELSE_IF else if
#endif

namespace piv
{
#if defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L)
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
        unsigned char bom = 0;
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

    namespace encoding
    {
        /**
         * @brief ANSI转UTF-16LE
         * @param utf16str 转换后的字符串在此参数中返回
         * @param mbstr 转换前的字符串
         * @param mbslen 转换前的字符串字符长度,-1为自动获取
         */
        static void A2Wex(std::wstring &utf16str, const char *mbstr, const size_t &mbslen = static_cast<size_t>(-1), const uint32_t& code_page = CP_ACP)
        {
            int utf16len = ::MultiByteToWideChar(code_page, 0, mbstr, (mbslen == static_cast<size_t>(-1)) ? -1 : static_cast<int>(mbslen), NULL, 0);
            if (utf16len > 0)
            {
                if (mbslen == static_cast<size_t>(-1))
                    utf16len -= 1;
                utf16str.resize(utf16len, '\0');
                ::MultiByteToWideChar(code_page, 0, mbstr, (mbslen == static_cast<size_t>(-1)) ? -1 : static_cast<int>(mbslen), const_cast<wchar_t *>(utf16str.data()), utf16len);
            }
            else
            {
                utf16str = L"";
            }
        }
        static void A2Wex(CVolString &utf16str, const char *mbstr, const size_t &mbslen = static_cast<size_t>(-1), const uint32_t& code_page = CP_ACP)
        {
            int utf16len = ::MultiByteToWideChar(code_page, 0, mbstr, (mbslen == static_cast<size_t>(-1)) ? -1 : static_cast<int>(mbslen), NULL, 0);
            if (utf16len > 0)
            {
                if (mbslen == static_cast<size_t>(-1))
                    utf16len -= 1;
                utf16str.m_mem.Alloc((utf16len + 1) * 2, TRUE);
                ::MultiByteToWideChar(code_page, 0, mbstr, (mbslen == static_cast<size_t>(-1)) ? -1 : static_cast<int>(mbslen), const_cast<wchar_t *>(utf16str.GetText()), utf16len);
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
         */
        static void W2Aex(std::string &mbstr, const wchar_t *utf16str, const size_t &utf16len = static_cast<size_t>(-1), const uint32_t& code_page = CP_ACP)
        {
            int mbslen = ::WideCharToMultiByte(code_page, 0, utf16str, (utf16len == static_cast<size_t>(-1)) ? -1 : static_cast<int>(utf16len), NULL, 0, NULL, NULL);
            if (mbslen > 0)
            {
                if (utf16len == static_cast<size_t>(-1))
                    mbslen -= 1;
                mbstr.resize(mbslen, '\0');
                ::WideCharToMultiByte(code_page, 0, utf16str, (utf16len == static_cast<size_t>(-1)) ? -1 : static_cast<int>(utf16len), const_cast<char *>(mbstr.data()), mbslen, NULL, NULL);
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
        static void W2Uex(std::string &utf8str, const wchar_t *utf16str, const size_t &utf16len = static_cast<size_t>(-1))
        {
#ifdef SIMDUTF_H
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
        static void U2Wex(std::wstring &utf16str, const char *utf8str, const size_t &utf8len = static_cast<size_t>(-1))
        {
#ifdef SIMDUTF_H
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
        static void U2Wex(CVolString &utf16str, const char *utf8str, const size_t &utf8len = static_cast<size_t>(-1))
        {
#ifdef SIMDUTF_H
            size_t utf8words = (utf8len == static_cast<size_t>(-1)) ? strlen(utf8str) : utf8len;
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
            int utf16len = ::MultiByteToWideChar(CP_UTF8, 0, utf8str, (utf8len == static_cast<size_t>(-1)) ? -1 : static_cast<int>(utf8len), NULL, 0);
            if (utf16len > 0)
            {
                if (utf8len == static_cast<size_t>(-1))
                    utf16len -= 1;
                utf16str.m_mem.Alloc((utf16len + 1) * 2, TRUE);
                ::MultiByteToWideChar(CP_UTF8, 0, utf8str, (utf8len == static_cast<size_t>(-1)) ? -1 : static_cast<int>(utf8len), const_cast<wchar_t *>(utf16str.GetText()), utf16len);
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
        static void U2Aex(std::string &mbstr, const char *utf8str, const size_t &utf8len = static_cast<size_t>(-1), const uint32_t& code_page = CP_ACP)
        {
            std::wstring utf16str;
            piv::encoding::U2Wex(utf16str, utf8str, utf8len);
            piv::encoding::W2Aex(mbstr, utf16str.c_str(), 0, code_page);
        }

        /**
         * @brief ANSI转UTF-8
         * @param utf8str 转换后的字符串在此参数中返回
         * @param mbstr 转换前的字符串
         * @param mbslen 转换前的字符串字符长度,-1为自动获取
         */
        static void A2Uex(std::string &utf8str, const char *mbstr, const size_t &mbslen = static_cast<size_t>(-1), const uint32_t& code_page = CP_ACP)
        {
            std::wstring utf16str;
            piv::encoding::A2Wex(utf16str, mbstr, mbslen, code_page);
            piv::encoding::W2Uex(utf8str, utf16str.c_str(), 0);
        }

        /**
         * @brief 是否有效URL编码字符
         * @param c 所欲判断的字符
         * @param Reserved 是否允许有特殊意义的保留字符
         * @return 返回字符是否为有效的URL编码字符
         */
        static bool IsUrlValidWord(const int &c, const bool &Reserved = false) noexcept
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
        static bool UrlEncodeNeed(const wchar_t *lpszSrc, const size_t &SrcLen, const bool &ReservedWord = false) noexcept
        {
            for (size_t i = 0; i < SrcLen; i++)
            {
                if (!(piv::encoding::IsUrlValidWord(lpszSrc[i], ReservedWord)))
                { // 判断是否已经URL编码
                    if (!(lpszSrc[i] == '%' && (i + 2) >= SrcLen && iswxdigit(lpszSrc[i + 1]) && iswxdigit(lpszSrc[i + 2])))
                        return true;
                }
            }
            return false;
        }
        static bool UrlEncodeNeed(const char *lpszSrc, const size_t &SrcLen, const bool &ReservedWord = false) noexcept
        {
            for (size_t i = 0; i < SrcLen; i++)
            {
                if (!(piv::encoding::IsUrlValidWord(lpszSrc[i], ReservedWord)))
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
        static bool UrlDecodeNeed(const wchar_t *lpszSrc, const size_t &SrcLen) noexcept
        {
            for (size_t i = 0; i < SrcLen; i++)
            {
                if (lpszSrc[i] == '%' && (i + 2) < SrcLen && iswxdigit(lpszSrc[i + 1]) && iswxdigit(lpszSrc[i + 2]))
                    return true;
            }
            return false;
        }
        static bool UrlDecodeNeed(const char *lpszSrc, const size_t &SrcLen) noexcept
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
        static size_t GuessUrlEncodeBound(const unsigned char *lpszSrc, const size_t &SrcLen, const bool &ReservedWord = false) noexcept
        {
            size_t Add = 0;
            for (size_t i = 0; i < SrcLen; i++)
            {
                unsigned char c = lpszSrc[i];
                if (!(piv::encoding::IsUrlValidWord(lpszSrc[i], ReservedWord)))
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
        static size_t GuessUrlDecodeBound(const unsigned char *lpszSrc, const size_t &SrcLen) noexcept
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
        static int32_t UrlEncode(const unsigned char *lpszSrc, const size_t &SrcLen, unsigned char *lpszDest, size_t &DestLen, const bool &ReservedWord = false) noexcept
        {
            if (lpszDest == nullptr || DestLen == 0)
                goto ERROR_DEST_LEN;
            unsigned char c, n;
            size_t j = 0;
            for (size_t i = 0; i < SrcLen; i++)
            {
                if (j >= DestLen)
                    goto ERROR_DEST_LEN;
                c = lpszSrc[i];
                if (piv::encoding::IsUrlValidWord(lpszSrc[i], ReservedWord))
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
        static int32_t UrlDecode(const unsigned char *lpszSrc, const size_t &SrcLen, unsigned char *lpszDest, size_t &DestLen) noexcept
        {
            if (lpszDest == nullptr || DestLen == 0)
                goto ERROR_DEST_LEN;
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
            if (chHexChar >= 'A' && chHexChar <= 'F')
                return static_cast<unsigned char>(chHexChar - 'A' + 10);
            if (chHexChar >= 'a' && chHexChar <= 'f')
                return static_cast<unsigned char>(chHexChar - 'a' + 10);
            return static_cast<unsigned char>(chHexChar);
        }

        static const char hexUpCh[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        static const char hexLowCh[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

        /**
         * @brief 到十六进制文本
         * @param separator 是否带分隔符
         * @param hexstr 返回的十六进制文本
         * @return
         */
        template <typename CharT>
        std::basic_string<CharT> &str_to_hex(const basic_string_view<CharT> &str, const bool &separator = false, std::basic_string<CharT> &hexstr = std::basic_string<CharT>{})
        {
            hexstr.clear();
            if (str.empty())
                return hexstr;
            hexstr.reserve(str.size() * (separator ? 3 : 2));
            const unsigned char *pStr = reinterpret_cast<const unsigned char *>(str.data());
            size_t count = str.size() * sizeof(CharT);
            for (size_t i = 0; i < count; ++i)
            {
                hexstr.push_back(hexUpCh[static_cast<unsigned char>(pStr[i]) >> 4]);
                hexstr.push_back(hexUpCh[static_cast<unsigned char>(pStr[i]) & 0xF]);
                if (separator)
                    hexstr.push_back(' ');
            }
            if (hexstr.back() == ' ')
                hexstr.pop_back();
            return hexstr;
        }

        template <typename CharT>
        CVolString &str_to_hex(const basic_string_view<CharT> &str, const bool &separator = false, CVolString &hexstr = CVolString{})
        {
            hexstr.Empty();
            if (str.empty())
                return hexstr;
            hexstr.SetNumAlignChars(str.size() * (separator ? 3 : 2));
            const unsigned char *pStr = reinterpret_cast<const unsigned char *>(str.data());
            size_t count = str.size() * sizeof(CharT);
            for (size_t i = 0; i < count; ++i)
            {
                hexstr.AddChar(hexUpCh[static_cast<unsigned char>(pStr[i]) >> 4]);
                hexstr.AddChar(hexUpCh[static_cast<unsigned char>(pStr[i]) & 0xF]);
                if (separator)
                    hexstr.AddChar(' ');
            }
            hexstr.TrimRight();
            return hexstr;
        }

        /**
         * @brief 置十六进制文本
         * @param hexstr 所欲还原的十六进制文本
         * @param str 返回还原后的文本
         * @return
         */
        template <typename CharT>
        std::basic_string<CharT> &hex_to_str(const basic_string_view<CharT> &hexstr, std::basic_string<CharT> &str = std::basic_string<CharT>{})
        {
            str.clear();
            if (hexstr.size() < 2 * sizeof(CharT))
                return str;
            str.resize(hexstr.size() / (2 * sizeof(CharT)));
            unsigned char *pStr = reinterpret_cast<unsigned char *>(str.data());
            CharT ch1, ch2;
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
            str.resize(n / sizeof(CharT));
            return str;
        }

        /**
         * @brief 到USC2编码
         * @param str 所欲编码的UNICODE文本
         * @param en_ascii 是否编码ASCII字符
         * @param usc2 返回的USC2文本
         * @return
         */
        template <typename CharT>
        std::basic_string<CharT> &to_usc2(const wstring_view &str, const bool &en_ascii = false, std::basic_string<CharT> &usc2 = std::basic_string<CharT>{})
        {
            usc2.clear();
            usc2.reserve(str.size() * 6);
            wchar_t ch;
            for (size_t i = 0; i < str.size(); i++)
            {
                ch = str[i];
                if (!en_ascii && ch < 128)
                {
                    usc2.push_back(static_cast<CharT>(ch));
                    continue;
                }
                usc2.push_back('\\');
                usc2.push_back('u');
                usc2.push_back(hexLowCh[static_cast<unsigned char>(ch >> 12)]);
                usc2.push_back(hexLowCh[static_cast<unsigned char>(ch >> 8 & 0x000F)]);
                usc2.push_back(hexLowCh[static_cast<unsigned char>((ch & 0x00FF) >> 4)]);
                usc2.push_back(hexLowCh[static_cast<unsigned char>(ch & 0x000F)]);
            }
            return usc2;
        }
        static CVolString &to_usc2str(const wstring_view &str, const bool &en_ascii = false, CVolString &usc2 = CVolString{})
        {
            usc2.Empty();
            usc2.SetNumAlignChars(str.size() * 6);
            wchar_t ch;
            for (size_t i = 0; i < str.size(); i++)
            {
                ch = str[i];
                if (!en_ascii && ch < 128)
                {
                    usc2.AddChar(ch);
                    continue;
                }
                usc2.AddChar('\\');
                usc2.AddChar('u');
                usc2.AddChar(hexLowCh[static_cast<unsigned char>(ch >> 12)]);
                usc2.AddChar(hexLowCh[static_cast<unsigned char>(ch >> 8 & 0x000F)]);
                usc2.AddChar(hexLowCh[static_cast<unsigned char>((ch & 0x00FF) >> 4)]);
                usc2.AddChar(hexLowCh[static_cast<unsigned char>(ch & 0x000F)]);
            }
            return usc2;
        }
        static CVolString &to_usc2str(const CVolString &str, const bool &en_ascii = false, CVolString &usc2 = CVolString{})
        {
            return to_usc2str(wstring_view{str.GetText()}, en_ascii, usc2);
        }

        template <typename CharT>
        std::wstring &usc2_to_str(const basic_string_view<CharT> &usc2, std::wstring &ret = std::wstring{})
        {
            ret.clear();
            size_t len = usc2.size();
            ret.reserve(len);
            CharT ch;
            for (size_t i = 0; i < len; i++)
            {
                ch = usc2[i];
                if (ch == '\\' && i + 5 < len && (usc2[i + 1] == 'u' || usc2[i + 1] == 'U'))
                {
                    ret.push_back(hexchar_to_value(usc2[i + 2]) << 12 | hexchar_to_value(usc2[i + 3]) << 8 | hexchar_to_value(usc2[i + 4]) << 4 | hexchar_to_value(usc2[i + 5]));
                    i += 5;
                }
                else
                {
                    ret.push_back(static_cast<wchar_t>(ch));
                }
            }
            return ret;
        }

        /**
         * @brief 值到十六进制
         * @param v 所欲转换的hash值
         * @param hexstr 返回的十六进制
         * @return
         */
        template <typename T>
        CVolMem &value_to_hex(const T &v, CVolMem &hex = CVolMem())
        {
            hex.Append(&v, sizeof(T));
            return hex;
        }
        template <typename T>
        CVolString &value_to_hex(const T &v, CVolString &hex, const bool &upper = true)
        {
            const unsigned char *pValue = reinterpret_cast<const unsigned char *>(&v);
            wchar_t *pStr = reinterpret_cast<wchar_t *>(hex.m_mem.Alloc(((sizeof(T) * 2) + 1) * 2, TRUE));
            if (upper)
            {
                for (size_t i = 0, n = 0; i < sizeof(T); ++i)
                {
                    pStr[n++] = hexUpCh[static_cast<unsigned char>(pValue[i]) >> 4];
                    pStr[n++] = hexUpCh[static_cast<unsigned char>(pValue[i]) & 0xF];
                }
            }
            else
            {
                for (size_t i = 0, n = 0; i < sizeof(T); ++i)
                {
                    pStr[n++] = hexLowCh[static_cast<unsigned char>(pValue[i]) >> 4];
                    pStr[n++] = hexLowCh[static_cast<unsigned char>(pValue[i]) & 0xF];
                }
            }
            return hex;
        }
        template <typename T, typename CharT>
        std::basic_string<CharT> &value_to_hex(const T &v, std::basic_string<CharT> &hex, const bool &upper = true)
        {
            const unsigned char *pValue = reinterpret_cast<const unsigned char *>(&v);
            if (upper)
            {
                for (size_t i = 0; i < sizeof(T); ++i)
                {
                    hex.push_back(hexUpCh[static_cast<unsigned char>(pValue[i]) >> 4]);
                    hex.push_back(hexUpCh[static_cast<unsigned char>(pValue[i]) & 0xF]);
                }
            }
            else
            {
                for (size_t i = 0; i < sizeof(T); ++i)
                {
                    hex.push_back(hexLowCh[static_cast<unsigned char>(pValue[i]) >> 4]);
                    hex.push_back(hexLowCh[static_cast<unsigned char>(pValue[i]) & 0xF]);
                }
            }
            return hex;
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
     * @brief 复制和移动构造函数
     * @param s 所欲复制的对象
     */
    PivA2W(const PivA2W &s)
    {
        utf16str = s.utf16str;
    }
    PivA2W(PivA2W &&s) noexcept
    {
        utf16str = std::move(s.utf16str);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivA2W(const char *mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr, mbslen);
    }
    PivA2W(const std::string &mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr, mbslen);
    }
    PivA2W(const piv::string_view &mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr, mbslen);
    }
    PivA2W(const CVolMem &mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr, mbslen);
    }

    operator const wchar_t *() const noexcept
    {
        return utf16str.c_str();
    }
    operator std::wstring const &() const noexcept
    {
        return utf16str;
    }
    operator std::wstring &() noexcept
    {
        return utf16str;
    }
    operator CVolString() const
    {
        return CVolString(utf16str.c_str());
    }
    operator CVolMem() const
    {
        return CVolMem(utf16str.c_str(), utf16str.size() * 2);
    }
    PivA2W &operator=(const PivA2W &_PivA2W)
    {
        utf16str = _PivA2W.utf16str;
        return *this;
    }
    PivA2W &operator=(PivA2W &&_PivA2W) noexcept
    {
        utf16str = std::move(_PivA2W.utf16str);
        return *this;
    }
    PivA2W &operator=(const char *mbstr)
    {
        Convert(mbstr);
        return *this;
    }
    bool operator==(const PivA2W &_PivA2W) const noexcept
    {
        return utf16str == _PivA2W.utf16str;
    }
    std::wstring *operator->() noexcept
    {
        return &utf16str;
    }
    const std::wstring *operator->() const noexcept
    {
        return &utf16str;
    }
    std::wstring &operator*() noexcept
    {
        return utf16str;
    }
    const std::wstring &operator*() const noexcept
    {
        return utf16str;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的ANSI文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    inline void Convert(const char *mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        piv::encoding::A2Wex(utf16str, mbstr, mbslen);
    }
    inline void Convert(const std::string &mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr.c_str(), (mbslen == static_cast<size_t>(-1)) ? mbstr.size() : mbslen);
    }
    inline void Convert(const piv::string_view &mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr.data(), (mbslen == static_cast<size_t>(-1)) ? mbstr.size() : mbslen);
    }
    inline void Convert(const CVolMem &mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(reinterpret_cast<const char *>(mbstr.GetPtr()), (mbslen == static_cast<size_t>(-1)) ? static_cast<size_t>(mbstr.GetSize()) : mbslen);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const wchar_t *GetText() const noexcept
    {
        return utf16str.c_str();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t GetLength() const noexcept
    {
        return utf16str.size();
    }

    /**
     * @brief 获取转换后的文本字节长度
     * @return 文本的字节长度
     */
    inline size_t GetSize() const noexcept
    {
        return utf16str.size() * 2;
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    inline wchar_t *GetPtr() const noexcept
    {
        return const_cast<wchar_t *>(utf16str.data());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    inline bool IsEmpty() const noexcept
    {
        return utf16str.empty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    inline std::wstring &String() noexcept
    {
        return utf16str;
    }
    inline const std::wstring &String() const noexcept
    {
        return utf16str;
    }

    /**
     * @brief 生成一个火山的文本型,其中包含了转换后的文本数据
     * @return 返回所转换的文本型
     */
    inline CVolString &GetStr(CVolString &str = CVolString{}) const
    {
        str.SetText(utf16str.c_str());
        return str;
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &GetMem(CVolMem &mem = CVolMem{}, const bool &null_char = false) const
    {
        mem.Empty();
        mem.Append(utf16str.c_str(), (utf16str.size() + (null_char ? 1 : 0)) * 2);
        return mem;
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
     * @brief 复制和移动构造函数
     * @param s 所欲复制的对象
     */
    PivW2A(const PivW2A &s)
    {
        mbstr = s.mbstr;
    }
    PivW2A(PivW2A &&s) noexcept
    {
        mbstr = std::move(s.mbstr);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param utf16str 所欲转换的UTF-16LE文本
     * @param utf16len 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivW2A(const wchar_t *utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        Convert(utf16str, utf16len);
    }
    PivW2A(const std::wstring &utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        Convert(utf16str, utf16len);
    }
    PivW2A(const CVolString &utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        Convert(utf16str, utf16len);
    }
    PivW2A(const piv::wstring_view &utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        Convert(utf16str, utf16len);
    }
    PivW2A(const CVolMem &utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        Convert(utf16str, utf16len);
    }

    operator const char *() const noexcept
    {
        return mbstr.c_str();
    }
    operator std::string const &() const noexcept
    {
        return mbstr;
    }
    operator std::string &() noexcept
    {
        return mbstr;
    }
    operator CVolMem() const
    {
        return CVolMem(mbstr.c_str(), mbstr.size());
    }
    PivW2A &operator=(const PivW2A &_PivW2A)
    {
        mbstr = _PivW2A.mbstr;
        return *this;
    }
    PivW2A &operator=(PivW2A &&_PivW2A) noexcept
    {
        mbstr = std::move(_PivW2A.mbstr);
        return *this;
    }
    PivW2A &operator=(const wchar_t *utf16str)
    {
        Convert(utf16str);
        return *this;
    }
    bool operator==(const PivW2A &_PivW2A) const noexcept
    {
        return mbstr == _PivW2A.mbstr;
    }
    std::string *operator->() noexcept
    {
        return &mbstr;
    }
    const std::string *operator->() const noexcept
    {
        return &mbstr;
    }
    std::string &operator*() noexcept
    {
        return mbstr;
    }
    const std::string &operator*() const noexcept
    {
        return mbstr;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    inline void Convert(const wchar_t *utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        piv::encoding::W2Aex(mbstr, utf16str, utf16len);
    }
    inline void Convert(const std::wstring &utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        Convert(utf16str.c_str(), (utf16len == static_cast<size_t>(-1)) ? utf16str.size() : utf16len);
    }
    inline void Convert(const CVolString &utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        Convert(utf16str.GetText(), (utf16len == static_cast<size_t>(-1)) ? static_cast<size_t>(utf16str.GetLength()) : utf16len);
    }
    inline void Convert(const piv::wstring_view &utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        Convert(utf16str.data(), (utf16len == static_cast<size_t>(-1)) ? utf16str.size() : utf16len);
    }
    inline void Convert(const CVolMem &utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        Convert(reinterpret_cast<const wchar_t *>(utf16str.GetPtr()), (utf16len == static_cast<size_t>(-1)) ? static_cast<size_t>(utf16str.GetSize() / 2) : utf16len);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const char *GetText() const noexcept
    {
        return mbstr.c_str();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t GetLength() const noexcept
    {
        return mbstr.size();
    }

    /**
     * @brief 获取转换后的文本字长度
     * @return 文本的字节长度
     */
    inline size_t GetSize() const noexcept
    {
        return mbstr.size();
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    inline char *GetPtr() noexcept
    {
        return const_cast<char *>(mbstr.data());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    inline bool IsEmpty() const noexcept
    {
        return mbstr.empty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    inline std::string &String() noexcept
    {
        return mbstr;
    }
    inline const std::string &String() const noexcept
    {
        return mbstr;
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &GetMem(CVolMem &mem = CVolMem{}, const bool &null_char = false) const
    {
        mem.Empty();
        mem.Append(mbstr.c_str(), mbstr.size() + (null_char ? 1 : 0));
        return mem;
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
     * @brief 复制和移动构造函数
     * @param s 所欲复制的对象
     */
    PivW2U(const PivW2U &s)
    {
        utf8str = s.utf8str;
    }
    PivW2U(PivW2U &&s) noexcept
    {
        utf8str = std::move(s.utf8str);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param utf16str 所欲转换的UTF-16LE文本
     * @param utf16len 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivW2U(const wchar_t *utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        Convert(utf16str, utf16len);
    }
    PivW2U(const std::wstring &utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        Convert(utf16str, utf16len);
    }
    PivW2U(const CVolString &utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        Convert(utf16str, utf16len);
    }
    PivW2U(const piv::wstring_view &utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        Convert(utf16str, utf16len);
    }
    PivW2U(const CVolMem &utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        Convert(utf16str, utf16len);
    }

    operator CVolMem() const
    {
        return CVolMem(utf8str.c_str(), utf8str.size());
    }
    operator const char *() const noexcept
    {
        return utf8str.c_str();
    }
    operator std::string const &() const noexcept
    {
        return utf8str;
    }
    operator std::string &() noexcept
    {
        return utf8str;
    }
    PivW2U &operator=(const PivW2U &_PivW2U)
    {
        utf8str = _PivW2U.utf8str;
        return *this;
    }
    PivW2U &operator=(PivW2U &&_PivW2U) noexcept
    {
        utf8str = std::move(_PivW2U.utf8str);
        return *this;
    }
    PivW2U &operator=(const wchar_t *utf16str)
    {
        Convert(utf16str);
        return *this;
    }
    bool operator==(const PivW2U &_PivW2U) const noexcept
    {
        return utf8str == _PivW2U.utf8str;
    }
    std::string *operator->() noexcept
    {
        return &utf8str;
    }
    const std::string *operator->() const noexcept
    {
        return &utf8str;
    }
    std::string &operator*() noexcept
    {
        return utf8str;
    }
    const std::string &operator*() const noexcept
    {
        return utf8str;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    inline void Convert(const wchar_t *utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        piv::encoding::W2Uex(utf8str, utf16str, utf16len);
    }
    inline void Convert(const std::wstring &utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        Convert(utf16str.c_str(), (utf16len == static_cast<size_t>(-1)) ? utf16str.size() : utf16len);
    }
    inline void Convert(const CVolString &utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        Convert(utf16str.GetText(), (utf16len == static_cast<size_t>(-1)) ? static_cast<size_t>(utf16str.GetLength()) : utf16len);
    }
    inline void Convert(const piv::wstring_view &utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        Convert(utf16str.data(), (utf16len == static_cast<size_t>(-1)) ? utf16str.size() : utf16len);
    }
    inline void Convert(const CVolMem &utf16str, const size_t &utf16len = static_cast<size_t>(-1))
    {
        Convert(reinterpret_cast<const wchar_t *>(utf16str.GetPtr()), (utf16len == static_cast<size_t>(-1)) ? static_cast<size_t>(utf16str.GetSize() / 2) : utf16len);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const char *GetText() const noexcept
    {
        return utf8str.c_str();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t GetLength() const noexcept
    {
        return utf8str.size();
    }

    /**
     * @brief 获取转换后的文本字节长度
     * @return 文本的字节长度
     */
    inline size_t GetSize() const noexcept
    {
        return utf8str.size();
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    inline char *GetPtr() noexcept
    {
        return const_cast<char *>(utf8str.data());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    inline bool IsEmpty() const noexcept
    {
        return utf8str.empty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    inline std::string &String() noexcept
    {
        return utf8str;
    }
    inline const std::string &String() const noexcept
    {
        return utf8str;
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &GetMem(CVolMem &mem = CVolMem{}, const bool &null_char = false) const
    {
        mem.Empty();
        mem.Append(utf8str.c_str(), utf8str.size() + (null_char ? 1 : 0));
        return mem;
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
     * @brief 复制和移动构造函数
     * @param s 所欲复制的对象
     */
    PivU2W(const PivU2W &s)
    {
        utf16str = s.utf16str;
    }
    PivU2W(PivU2W &&s) noexcept
    {
        utf16str = std::move(s.utf16str);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param utf8str 所欲转换的UTF-8文本
     * @param utf8len 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivU2W(const char *utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str, utf8len);
    }
    PivU2W(const std::string &utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str, utf8len);
    }
    PivU2W(const piv::string_view &utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str, utf8len);
    }
    PivU2W(const CVolMem &utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str, utf8len);
    }

    operator const wchar_t *() const noexcept
    {
        return utf16str.c_str();
    }
    operator std::wstring const &() const noexcept
    {
        return utf16str;
    }
    operator std::wstring &()
    {
        return utf16str;
    }
    operator CVolString() const
    {
        return CVolString(utf16str.c_str());
    }
    operator CVolMem() const
    {
        return CVolMem(utf16str.c_str(), utf16str.size() * 2);
    }
    PivU2W &operator=(const PivU2W &_PivU2W)
    {
        utf16str = _PivU2W.utf16str;
        return *this;
    }
    PivU2W &operator=(PivU2W &&_PivU2W) noexcept
    {
        utf16str = std::move(_PivU2W.utf16str);
        return *this;
    }
    PivU2W &operator=(const char *utf8str)
    {
        Convert(utf8str);
        return *this;
    }
    bool operator==(const PivU2W &_PivU2W) const noexcept
    {
        return utf16str == _PivU2W.utf16str;
    }
    std::wstring *operator->() noexcept
    {
        return &utf16str;
    }
    const std::wstring *operator->() const noexcept
    {
        return &utf16str;
    }
    std::wstring &operator*() noexcept
    {
        return utf16str;
    }
    const std::wstring &operator*() const noexcept
    {
        return utf16str;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    inline void Convert(const char *utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        piv::encoding::U2Wex(utf16str, utf8str, utf8len);
    }
    inline void Convert(const std::string &utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str.c_str(), (utf8len == static_cast<size_t>(-1)) ? utf8str.size() : utf8len);
    }
    inline void Convert(const piv::string_view &utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str.data(), (utf8len == static_cast<size_t>(-1)) ? utf8str.size() : utf8len);
    }
    inline void Convert(const CVolMem &utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(reinterpret_cast<const char *>(utf8str.GetPtr()), (utf8len == static_cast<size_t>(-1)) ? static_cast<size_t>(utf8str.GetSize()) : utf8len);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const wchar_t *GetText() const noexcept
    {
        return utf16str.c_str();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t GetLength() const noexcept
    {
        return utf16str.size();
    }

    /**
     * @brief 获取转换后的文本字节长度
     * @return 文本的字节长度
     */
    inline size_t GetSize() const noexcept
    {
        return utf16str.size() * 2;
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    inline wchar_t *GetPtr() noexcept
    {
        return const_cast<wchar_t *>(utf16str.data());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    inline bool IsEmpty() const noexcept
    {
        return utf16str.empty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    inline std::wstring &String() noexcept
    {
        return utf16str;
    }
    inline const std::wstring &String() const noexcept
    {
        return utf16str;
    }

    /**
     * @brief 生成一个火山的文本型,其中包含了转换后的文本数据
     * @return 返回所转换的文本型
     */
    inline CVolString &GetStr(CVolString &str = CVolString{}) const
    {
        str.SetText(utf16str.c_str());
        return str;
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &GetMem(CVolMem &mem = CVolMem{}, const bool &null_char = false) const
    {
        mem.Empty();
        mem.Append(utf16str.c_str(), (utf16str.size() + (null_char ? 1 : 0)) * 2);
        return mem;
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
     * @brief 复制和移动构造函数
     * @param s 所欲复制的对象
     */
    PivU2A(const PivU2A &s)
    {
        mbstr = s.mbstr;
    }
    PivU2A(PivU2A &&s) noexcept
    {
        mbstr = std::move(s.mbstr);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param utf8str 所欲转换的UTF-8文本
     * @param utf8len 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivU2A(const char *utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str, utf8len);
    }
    PivU2A(const std::string &utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str, utf8len);
    }
    PivU2A(const piv::string_view &utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str, utf8len);
    }
    PivU2A(const CVolMem &utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str, utf8len);
    }

    operator const char *() const noexcept
    {
        return mbstr.c_str();
    }
    operator std::string const &() const noexcept
    {
        return mbstr;
    }
    operator CVolMem() const
    {
        return CVolMem(mbstr.c_str(), mbstr.size());
    }
    PivU2A &operator=(const PivU2A &_PivU2A)
    {
        mbstr = _PivU2A.mbstr;
        return *this;
    }
    PivU2A &operator=(PivU2A &&_PivU2A) noexcept
    {
        mbstr = std::move(_PivU2A.mbstr);
        return *this;
    }
    PivU2A &operator=(const char *utf8str)
    {
        Convert(utf8str);
        return *this;
    }
    bool operator==(const PivU2A &_PivU2A) const noexcept
    {
        return mbstr == _PivU2A.mbstr;
    }
    std::string *operator->() noexcept
    {
        return &mbstr;
    }
    const std::string *operator->() const noexcept
    {
        return &mbstr;
    }
    std::string &operator*() noexcept
    {
        return mbstr;
    }
    const std::string &operator*() const noexcept
    {
        return mbstr;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    inline void Convert(const char *utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        piv::encoding::U2Aex(mbstr, utf8str, utf8len);
    }
    inline void Convert(const std::string &utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str.c_str(), (utf8len == static_cast<size_t>(-1)) ? utf8str.size() : utf8len);
    }
    inline void Convert(const piv::string_view &utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str.data(), (utf8len == static_cast<size_t>(-1)) ? utf8str.size() : utf8len);
    }
    inline void Convert(const CVolMem &utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(reinterpret_cast<const char *>(utf8str.GetPtr()), (utf8len == static_cast<size_t>(-1)) ? static_cast<size_t>(utf8str.GetSize()) : utf8len);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const char *GetText() const noexcept
    {
        return mbstr.c_str();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t GetLength() const noexcept
    {
        return mbstr.size();
    }

    /**
     * @brief 获取转换后的字节长度
     * @return 文本的字节长度
     */
    inline size_t GetSize() const noexcept
    {
        return mbstr.size();
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    inline char *GetPtr() noexcept
    {
        return const_cast<char *>(mbstr.data());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    inline bool IsEmpty() const noexcept
    {
        return mbstr.empty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    inline std::string &string() noexcept
    {
        return mbstr;
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &GetMem(CVolMem &mem = CVolMem{}, const bool &null_char = false) const
    {
        mem.Empty();
        mem.Append(mbstr.c_str(), mbstr.size() + (null_char ? 1 : 0));
        return mem;
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
     * @brief 复制和移动构造函数
     * @param s 所欲复制的对象
     */
    PivA2U(const PivA2U &s)
    {
        utf8str = s.utf8str;
    }
    PivA2U(PivA2U &&s) noexcept
    {
        utf8str = std::move(s.utf8str);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param mbstr 所欲转换的ANSI文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivA2U(const char *mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr, mbslen);
    }
    PivA2U(const std::string &mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr, mbslen);
    }
    PivA2U(const piv::string_view &mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr, mbslen);
    }
    PivA2U(const CVolMem &mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr, mbslen);
    }

    operator const char *() const noexcept
    {
        return utf8str.c_str();
    }
    operator std::string const &() const noexcept
    {
        return utf8str;
    }
    operator CVolMem() const
    {
        return CVolMem(utf8str.c_str(), utf8str.size());
    }
    PivA2U &operator=(const PivA2U &_PivA2U)
    {
        utf8str = _PivA2U.utf8str;
        return *this;
    }
    PivA2U &operator=(PivA2U &&_PivA2U) noexcept
    {
        utf8str = std::move(_PivA2U.utf8str);
        return *this;
    }
    PivA2U &operator=(const char *mbstr)
    {
        Convert(mbstr);
        return *this;
    }
    bool operator==(const PivA2U &_PivA2U) const noexcept
    {
        return utf8str == _PivA2U.utf8str;
    }
    std::string *operator->() noexcept
    {
        return &utf8str;
    }
    const std::string *operator->() const noexcept
    {
        return &utf8str;
    }
    std::string &operator*() noexcept
    {
        return utf8str;
    }
    const std::string &operator*() const noexcept
    {
        return utf8str;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    inline void Convert(const char *mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        piv::encoding::A2Uex(utf8str, mbstr, mbslen);
    }
    inline void Convert(const std::string &mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr.c_str(), (mbslen == static_cast<size_t>(-1)) ? mbstr.size() : mbslen);
    }
    inline void Convert(const piv::string_view &mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr.data(), (mbslen == static_cast<size_t>(-1)) ? mbstr.size() : mbslen);
    }
    inline void Convert(const CVolMem &mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(reinterpret_cast<const char *>(mbstr.GetPtr()), (mbslen == static_cast<size_t>(-1)) ? static_cast<size_t>(mbstr.GetSize()) : mbslen);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const char *GetText() const noexcept
    {
        return utf8str.c_str();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t GetLength() const noexcept
    {
        return utf8str.size();
    }

    /**
     * @brief 获取转换后的字节长度
     * @return 文本的字节长度
     */
    inline size_t GetSize() const noexcept
    {
        return utf8str.size();
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    inline char *GetPtr() noexcept
    {
        return const_cast<char *>(utf8str.data());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    inline bool IsEmpty() const noexcept
    {
        return utf8str.empty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    inline std::string &String() noexcept
    {
        return utf8str;
    }
    inline const std::string &String() const noexcept
    {
        return utf8str;
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &GetMem(CVolMem &mem = CVolMem{}, const bool &null_char = false) const
    {
        mem.Empty();
        mem.Append(utf8str.c_str(), utf8str.size() + (null_char ? 1 : 0));
        return mem;
    }
}; // PivA2U

/**
 * @brief ANSI编码转UTF-16LE编码的封装类(内部数据为火山文本型)
 */
class PivA2Ws
{
private:
    CVolString utf16str;

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
    PivA2Ws(const PivA2Ws &s)
    {
        utf16str = s.utf16str;
    }
    PivA2Ws(PivA2Ws &&s) noexcept
    {
        utf16str = std::move(s.utf16str);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivA2Ws(const char *mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr, mbslen);
    }
    PivA2Ws(const std::string &mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr, mbslen);
    }
    PivA2Ws(const piv::string_view &mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr, mbslen);
    }
    PivA2Ws(const CVolMem &mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr, mbslen);
    }

    operator const wchar_t *() const noexcept
    {
        return utf16str.GetText();
    }
    operator CVolString const &() const noexcept
    {
        return utf16str;
    }
    operator CVolString &() noexcept
    {
        return utf16str;
    }
    operator CVolMem() const
    {
        return CVolMem(utf16str.GetText(), utf16str.GetLength() * 2);
    }
    PivA2Ws &operator=(const PivA2Ws &_PivA2W)
    {
        utf16str = _PivA2W.utf16str;
        return *this;
    }
    PivA2Ws &operator=(PivA2Ws &&_PivA2W) noexcept
    {
        utf16str = std::move(_PivA2W.utf16str);
        return *this;
    }
    PivA2Ws &operator=(const char *mbstr)
    {
        Convert(mbstr);
        return *this;
    }
    bool operator==(const PivA2Ws &_PivA2W) const noexcept
    {
        return utf16str == _PivA2W.utf16str;
    }
    CVolString *operator->() noexcept
    {
        return &utf16str;
    }
    const CVolString *operator->() const noexcept
    {
        return &utf16str;
    }
    CVolString &operator*() noexcept
    {
        return utf16str;
    }
    const CVolString &operator*() const noexcept
    {
        return utf16str;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的ANSI文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    inline void Convert(const char *mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        piv::encoding::A2Wex(utf16str, mbstr, mbslen);
    }
    inline void Convert(const std::string &mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr.c_str(), (mbslen == static_cast<size_t>(-1)) ? mbstr.size() : mbslen);
    }
    inline void Convert(const piv::string_view &mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(mbstr.data(), (mbslen == static_cast<size_t>(-1)) ? mbstr.size() : mbslen);
    }
    inline void Convert(const CVolMem &mbstr, const size_t &mbslen = static_cast<size_t>(-1))
    {
        Convert(reinterpret_cast<const char *>(mbstr.GetPtr()), (mbslen == static_cast<size_t>(-1)) ? static_cast<size_t>(mbstr.GetSize()) : mbslen);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const wchar_t *GetText() const noexcept
    {
        return utf16str.GetText();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t GetLength() const noexcept
    {
        return static_cast<size_t>(utf16str.GetLength());
    }

    /**
     * @brief 获取转换后的字节长度
     * @return 文本的字节长度
     */
    inline size_t GetSize() const noexcept
    {
        return utf16str.GetLength() * 2;
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    inline wchar_t *GetPtr() const noexcept
    {
        return const_cast<wchar_t *>(utf16str.GetText());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    inline bool IsEmpty() const noexcept
    {
        return utf16str.IsEmpty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    inline std::wstring String() const noexcept
    {
        return std::wstring{utf16str.GetText()};
    }

    /**
     * @brief 生成一个火山的文本型,其中包含了转换后的文本数据
     * @return 返回所转换的文本型
     */
    inline CVolString &ToStr()
    {
        return utf16str;
    }

    /**
     * @brief 生成一个火山的文本型,其中包含了转换后的文本数据
     * @return 返回所转换的文本型
     */
    inline CVolString &GetStr(CVolString &str = CVolString{}) const
    {
        str.SetText(utf16str.GetText());
        return str;
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &GetMem(CVolMem &mem = CVolMem{}, const bool &null_char = false) const
    {
        mem.Empty();
        mem.Append(utf16str.GetText(), (utf16str.GetLength() + (null_char ? 1 : 0)) * 2);
        return mem;
    }
}; // PivA2Ws

/**
 * @brief UTF-8编码转UTF-16LE编码的封装类(内部数据为火山文本型)
 */
class PivU2Ws
{
private:
    CVolString utf16str;

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
    PivU2Ws(const PivU2Ws &s)
    {
        utf16str = s.utf16str;
    }
    PivU2Ws(PivU2Ws &&s) noexcept
    {
        utf16str = std::move(s.utf16str);
    }

    /**
     * @brief 构造的同时将提供的文本转换
     * @param utf8str 所欲转换的UTF-8文本
     * @param utf8len 文本的字符长度,为-1时文本必须带结尾0字符
     */
    PivU2Ws(const char *utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str, utf8len);
    }
    PivU2Ws(const std::string &utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str, utf8len);
    }
    PivU2Ws(const piv::string_view &utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str, utf8len);
    }
    PivU2Ws(const CVolMem &utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str, utf8len);
    }

    operator const wchar_t *() const noexcept
    {
        return utf16str.GetText();
    }
    operator CVolString const &() const noexcept
    {
        return utf16str;
    }
    operator CVolString &() noexcept
    {
        return utf16str;
    }
    operator CVolMem() const
    {
        return CVolMem(utf16str.GetText(), utf16str.GetLength() * 2);
    }
    PivU2Ws &operator=(const PivU2Ws &_PivU2W)
    {
        utf16str = _PivU2W.utf16str;
        return *this;
    }
    PivU2Ws &operator=(PivU2Ws &&_PivU2W) noexcept
    {
        utf16str = std::move(_PivU2W.utf16str);
        return *this;
    }
    PivU2Ws &operator=(const char *utf8str)
    {
        Convert(utf8str);
        return *this;
    }
    bool operator==(const PivU2Ws &_PivU2W) const noexcept
    {
        return utf16str == _PivU2W.utf16str;
    }
    CVolString *operator->() noexcept
    {
        return &utf16str;
    }
    const CVolString *operator->() const noexcept
    {
        return &utf16str;
    }
    CVolString &operator*() noexcept
    {
        return utf16str;
    }
    const CVolString &operator*() const noexcept
    {
        return utf16str;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    inline void Convert(const char *utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        piv::encoding::U2Wex(utf16str, utf8str, utf8len);
    }
    inline void Convert(const std::string &utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str.c_str(), (utf8len == static_cast<size_t>(-1)) ? utf8str.size() : utf8len);
    }
    inline void Convert(const piv::string_view &utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(utf8str.data(), (utf8len == static_cast<size_t>(-1)) ? utf8str.size() : utf8len);
    }
    inline void Convert(const CVolMem &utf8str, const size_t &utf8len = static_cast<size_t>(-1))
    {
        Convert(reinterpret_cast<const char *>(utf8str.GetPtr()), (utf8len == static_cast<size_t>(-1)) ? static_cast<size_t>(utf8str.GetSize()) : utf8len);
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const wchar_t *GetText() const noexcept
    {
        return utf16str.GetText();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t GetLength() const noexcept
    {
        return static_cast<size_t>(utf16str.GetLength());
    }

    /**
     * @brief 获取转换后的字节长度
     * @return 文本的字节长度
     */
    inline size_t GetSize() const noexcept
    {
        return utf16str.GetLength() * 2;
    }

    /**
     * @brief 获取转换的文本首字符指针
     * @return 文本的首字符指针
     */
    inline wchar_t *GetPtr() noexcept
    {
        return const_cast<wchar_t *>(utf16str.GetText());
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    inline bool IsEmpty() const noexcept
    {
        return utf16str.IsEmpty();
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    inline std::wstring String() noexcept
    {
        return std::wstring{utf16str.GetText()};
    }

    /**
     * @brief 生成一个火山的文本型,其中包含了转换后的文本数据
     * @return 返回所转换的文本型
     */
    inline CVolString &ToStr()
    {
        return utf16str;
    }

    /**
     * @brief 生成一个火山的文本型,其中包含了转换后的文本数据
     * @return 返回所转换的文本型
     */
    inline CVolString &GetStr(CVolString &str = CVolString{}) const
    {
        str.SetText(utf16str.GetText());
        return str;
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &GetMem(CVolMem &mem = CVolMem{}, const bool &null_char = false) const
    {
        mem.Empty();
        mem.Append(utf16str.GetText(), (utf16str.GetLength() + (null_char ? 1 : 0)) * 2);
        return mem;
    }
}; // PivU2Ws

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
        template <typename CharT>
        std::basic_string<CharT> &UrlStrDecode(const basic_string_view<CharT> &str, const bool &utf8 = true, std::basic_string<CharT> &decoded = std::basic_string<CharT>{})
        {
            decoded.clear();
            if (str.size() == 0)
                return decoded;
            if (piv::encoding::UrlDecodeNeed(str.data(), str.size()))
            {
                size_t buffer_len;
                PIV_IF(sizeof(CharT) == 2)
                {
                    std::string buffer;
                    if (utf8)
                    {
                        PivW2U urlstr(reinterpret_cast<const wchar_t *>(str.data()), str.size());
                        buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const unsigned char *>(urlstr.GetPtr()), urlstr.GetSize());
                        buffer.resize(buffer_len);
                        piv::encoding::UrlDecode(reinterpret_cast<const unsigned char *>(urlstr.GetPtr()), urlstr.GetSize(), const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(buffer.data())), buffer_len);
                        decoded.assign(reinterpret_cast<const CharT *>(PivU2W{buffer}.GetText()));
                    }
                    else
                    {
                        PivW2A urlstr(reinterpret_cast<const wchar_t *>(str.data()), str.size());
                        buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const unsigned char *>(urlstr.GetPtr()), urlstr.GetSize());
                        buffer.resize(buffer_len);
                        piv::encoding::UrlDecode(reinterpret_cast<const unsigned char *>(urlstr.GetPtr()), urlstr.GetSize(), const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(buffer.data())), buffer_len);
                        decoded.assign(reinterpret_cast<const CharT *>(PivA2W{buffer}.GetText()));
                    }
                }
                else
                {
                    buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const unsigned char *>(str.data()), str.size());
                    decoded.resize(buffer_len);
                    piv::encoding::UrlDecode(reinterpret_cast<const unsigned char *>(str.data()), str.size(), const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(decoded.data())), buffer_len);
                }
            }
            else
            {
                decoded.assign(str.data(), str.size());
            }
            return decoded;
        }
        template <typename CharT>
        std::basic_string<CharT> &UrlStrDecode(const std::basic_string<CharT> &str, const bool &utf8 = true, std::basic_string<CharT> &decoded = std::basic_string<CharT>{})
        {
            return piv::encoding::UrlStrDecode(basic_string_view<CharT>{str}, utf8, decoded);
        }

        template <typename CharT>
        CVolMem &UrlStrDecode(const basic_string_view<CharT> &str, CVolMem &decoded = CVolMem{}, const bool &utf8 = true)
        {
            decoded.Empty();
            if (str.size() == 0)
                return decoded;
            size_t buffer_len;
            if (piv::encoding::UrlDecodeNeed(str.data(), str.size()))
            {

                PIV_IF(sizeof(CharT) == 2)
                {
                    std::string buffer;
                    if (utf8)
                    {
                        PivW2U urlstr(reinterpret_cast<const wchar_t *>(str.data()), str.size());
                        buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const unsigned char *>(urlstr.GetPtr()), urlstr.GetSize());
                        buffer.resize(buffer_len);
                        piv::encoding::UrlDecode(reinterpret_cast<const unsigned char *>(urlstr.GetPtr()), urlstr.GetSize(), const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(buffer.data())), buffer_len);
                        return PivU2W{buffer}.GetMem(decoded);
                    }
                    else
                    {
                        PivW2A urlstr(reinterpret_cast<const wchar_t *>(str.data()), str.size());
                        buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const unsigned char *>(urlstr.GetPtr()), urlstr.GetSize());
                        buffer.resize(buffer_len);
                        piv::encoding::UrlDecode(reinterpret_cast<const unsigned char *>(urlstr.GetPtr()), urlstr.GetSize(), const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(buffer.data())), buffer_len);
                        return PivA2W{buffer}.GetMem(decoded);
                    }
                }
                else
                {
                    buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const unsigned char *>(str.data()), str.size());
                    piv::encoding::UrlDecode(reinterpret_cast<const unsigned char *>(str.data()), str.size(), decoded.Alloc(static_cast<INT_P>(buffer_len), TRUE), buffer_len);
                    return decoded;
                }
            }
            decoded.Append(reinterpret_cast<const void *>(str.data()), str.size() * sizeof(CharT));
            return decoded;
        }
        template <typename CharT>
        CVolMem &UrlStrDecode(const std::basic_string<CharT> &str, CVolMem &decoded = CVolMem{}, const bool &utf8 = true)
        {
            return piv::encoding::UrlStrDecode(basic_string_view<CharT>{str.c_str()}, decoded, utf8);
        }
        template <typename = void>
        CVolMem &UrlStrDecode(const CVolMem &str, const bool &utf8 = true, CVolMem &decoded = CVolMem{})
        {
            return piv::encoding::UrlStrDecode(string_view(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())), decoded, utf8);
        }
        template <typename = void>
        CVolString &UrlStrDecode(const CVolMem &str, const bool &utf8 = true, CVolString &decoded = CVolString{})
        {
            if (piv::encoding::UrlDecodeNeed(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.size())))
            {
                CVolMem buffer;
                piv::encoding::UrlStrDecode(string_view(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())), buffer, utf8);
                if (utf8)
                    return PivU2Ws{buffer}.GetStr(decoded);
                else
                    return PivA2Ws{buffer}.GetStr(decoded);
            }
            else
            {
                if (utf8)
                    return PivU2Ws{str}.GetStr(decoded);
                else
                    return PivA2Ws{str}.GetStr(decoded);
            }
        }
        template <typename CharT>
        CVolString &UrlStrDecode(const basic_string_view<CharT> &str, CVolString &decoded = CVolString{}, const bool &utf8 = true)
        {
            decoded.Empty();
            if (str.size() == 0)
                return decoded;
            size_t buffer_len;
            if (piv::encoding::UrlDecodeNeed(str.data(), str.size()))
            {
                std::string buffer;
                PIV_IF(sizeof(CharT) == 2)
                {
                    if (utf8)
                    {
                        PivW2U urlstr(reinterpret_cast<const wchar_t *>(str.data()), str.size());
                        buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const unsigned char *>(urlstr.GetPtr()), urlstr.GetSize());
                        buffer.resize(buffer_len);
                        piv::encoding::UrlDecode(reinterpret_cast<const unsigned char *>(urlstr.GetPtr()), urlstr.GetSize(), const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(buffer.data())), buffer_len);
                        return PivU2Ws{buffer}.GetStr(decoded);
                    }
                    else
                    {
                        PivW2A urlstr(reinterpret_cast<const wchar_t *>(str.data()), str.size());
                        buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const unsigned char *>(urlstr.GetPtr()), urlstr.GetSize());
                        buffer.resize(buffer_len);
                        piv::encoding::UrlDecode(reinterpret_cast<const unsigned char *>(urlstr.GetPtr()), urlstr.GetSize(), const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(buffer.data())), buffer_len);
                        return PivA2Ws{buffer}.GetStr(decoded);
                    }
                }
                else
                {
                    buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const unsigned char *>(str.data()), str.size());
                    buffer.resize(buffer_len);
                    piv::encoding::UrlDecode(reinterpret_cast<const unsigned char *>(str.data()), str.size(), const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(buffer.data())), buffer_len);
                    if (utf8)
                        return PivU2Ws{buffer}.GetStr(decoded);
                    else
                        return PivA2Ws{buffer}.GetStr(decoded);
                }
            }
            PIV_IF(sizeof(CharT) == 1)
            {
                if (utf8)
                    return PivU2Ws{reinterpret_cast<const char *>(str.data()), str.size()}.GetStr(decoded);
                else
                    return PivA2Ws{reinterpret_cast<const char *>(str.data()), str.size()}.GetStr(decoded);
            }
            decoded.SetText(reinterpret_cast<const wchar_t *>(str.data()), str.size());
            return decoded;
        }

        /**
         * @brief URL文本编码
         * @param str 所欲编码的文本
         * @param encoded 返回编码后的文本
         * @param utf8 是否为UTF-8编码
         * @param ReservedWord 是否不编码保留字符
         */
        template <typename CharT>
        std::basic_string<CharT> &UrlStrEncode(const basic_string_view<CharT> &str, const bool &utf8 = true, const bool &ReservedWord = false, std::basic_string<CharT> &encoded = std::basic_string<CharT>{})
        {
            size_t buffer_len;
            if (piv::encoding::UrlEncodeNeed(str.data(), str.size(), ReservedWord))
            {
                PIV_IF(sizeof(CharT) == 2)
                {
                    CVolMem buffer;
                    if (utf8)
                    {
                        PivW2U text{reinterpret_cast<const wchar_t *>(str.data()), str.size()};
                        buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const unsigned char *>(text.GetPtr()), text.GetSize(), ReservedWord);
                        CVolMem buffer;
                        piv::encoding::UrlEncode(reinterpret_cast<const unsigned char *>(text.GetPtr()), text.GetSize(), buffer.Alloc(static_cast<INT_P>(buffer_len), TRUE), buffer_len, ReservedWord);
                        PivU2W urled{buffer, buffer_len};
                        encoded.assign(reinterpret_cast<const CharT *>(urled.GetText()), urled.GetLength());
                    }
                    else
                    {
                        PivW2A text{reinterpret_cast<const wchar_t *>(str.data()), str.size()};
                        buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const unsigned char *>(text.GetPtr()), text.GetSize(), ReservedWord);
                        CVolMem buffer;
                        piv::encoding::UrlEncode(reinterpret_cast<const unsigned char *>(text.GetPtr()), text.GetSize(), buffer.Alloc(static_cast<INT_P>(buffer_len), TRUE), buffer_len, ReservedWord);
                        PivA2W urled{buffer, buffer_len};
                        encoded.assign(reinterpret_cast<const CharT *>(urled.GetText()), urled.GetLength());
                    }
                }
                else
                {
                    buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const unsigned char *>(str.data()), str.size(), ReservedWord);
                    encoded.resize(buffer_len);
                    piv::encoding::UrlEncode(reinterpret_cast<const unsigned char *>(str.data()), str.size(), const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(encoded.data())), buffer_len, ReservedWord);
                    encoded.resize(buffer_len);
                }
            }
            else
            {
                encoded.assign(str.data(), str.size());
            }
            return encoded;
        }
        template <typename CharT>
        std::basic_string<CharT> &UrlStrEncode(const std::basic_string<CharT> &str, const bool &utf8 = true, const bool &ReservedWord = false, std::basic_string<CharT> &encoded = std::basic_string<CharT>{})
        {
            return piv::encoding::UrlStrEncode(basic_string_view<CharT>{str.c_str()}, utf8, ReservedWord, encoded);
        }

        template <typename CharT>
        CVolMem &UrlDataEncode(const basic_string_view<CharT> &str, const bool &utf8 = true, const bool &ReservedWord = false, CVolMem &encoded = CVolMem{})
        {
            encoded.Empty();
            size_t buffer_len;
            if (piv::encoding::UrlEncodeNeed(str.data(), str.size(), ReservedWord))
            {
                PIV_IF(sizeof(CharT) == 2)
                {
                    CVolMem buffer;
                    if (utf8)
                    {
                        PivW2U text{reinterpret_cast<const wchar_t *>(str.data()), str.size()};
                        buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const unsigned char *>(text.GetPtr()), text.GetSize(), ReservedWord);
                        piv::encoding::UrlEncode(reinterpret_cast<const unsigned char *>(text.GetPtr()), text.GetSize(), buffer.Alloc(static_cast<INT_P>(buffer_len), TRUE), buffer_len, ReservedWord);
                        return PivU2W{buffer, buffer_len}.GetMem(encoded);
                    }
                    else
                    {
                        PivW2A text{reinterpret_cast<const wchar_t *>(str.data()), str.size()};
                        buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const unsigned char *>(text.GetPtr()), text.GetSize(), ReservedWord);
                        piv::encoding::UrlEncode(reinterpret_cast<const unsigned char *>(text.GetPtr()), text.GetSize(), buffer.Alloc(static_cast<INT_P>(buffer_len), TRUE), buffer_len, ReservedWord);
                        return PivA2W{buffer, buffer_len}.GetMem(encoded);
                    }
                }
                else
                {
                    buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const unsigned char *>(str.data()), str.size(), ReservedWord);
                    piv::encoding::UrlEncode(reinterpret_cast<const unsigned char *>(str.data()), str.size(), encoded.Alloc(static_cast<INT_P>(buffer_len), TRUE), buffer_len, ReservedWord);
                    encoded.Realloc(buffer_len);
                    return encoded;
                }
            }
            else
            {
                encoded.Append(reinterpret_cast<const void *>(str.data()), str.size() * sizeof(CharT));
            }
            return encoded;
        }
        template <typename CharT>
        CVolMem &UrlDataEncode(const std::basic_string<CharT> &str, const bool &utf8 = true, const bool &ReservedWord = false, CVolMem &encoded = CVolMem{})
        {
            return piv::encoding::UrlDataEncode(std::basic_string<CharT>{str.c_str()}, utf8, ReservedWord, encoded);
        }

        template <typename = void>
        CVolMem &UrlDataEncode(const CVolString &str, const bool &utf8 = true, const bool &ReservedWord = false, CVolMem &encoded = CVolMem{})
        {
            return piv::encoding::UrlDataEncode(wstring_view(str.GetText()), utf8, ReservedWord, encoded);
        }
        template <typename = void>
        CVolMem &UrlDataEncode(const CVolMem &str, const bool &utf8 = true, const bool &ReservedWord = false, CVolMem &encoded = CVolMem{})
        {
            return piv::encoding::UrlDataEncode(string_view(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())), utf8, ReservedWord, encoded);
        }
        template <typename = void>
        CVolString &UrlStrEncode(const CVolMem &str, CVolString &encoded, const bool &utf8 = true, const bool &ReservedWord = false)
        {
            if (piv::encoding::UrlEncodeNeed(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize()), ReservedWord))
            {
                CVolMem buffer;
                piv::encoding::UrlStrEncode(string_view(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())), buffer, ReservedWord);
                if (utf8)
                    return PivU2Ws{buffer}.GetStr(encoded);
                else
                    return PivA2Ws{buffer}.GetStr(encoded);
            }
            else
            {
                if (utf8)
                    return PivU2Ws{str}.GetStr(encoded);
                else
                    return PivA2Ws{str}.GetStr(encoded);
            }
            encoded.Empty();
            return encoded;
        }

    } // namespace encoding
} // namespace piv

#endif // _PIV_ENCODING_HPP
