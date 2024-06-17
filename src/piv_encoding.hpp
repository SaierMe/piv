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

    namespace detail
    {
        static ::piv::string_view const base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";
        static ::piv::string_view const base85_chars =
            "0123456789"
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            ".-:+=^!/*?&<>()[]{}@%$#";
        static ::piv::string_view const base91_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789!#$%&()*+,./:;-="
            "\\?@[]^_`{|}~\'";
    } // namespace detail

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
            piv::encoding::U2Wex(utf16str, utf8str, utf8len);
            piv::encoding::W2Aex(mbstr, utf16str.c_str(), utf16str.size(), code_page);
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
            piv::encoding::A2Wex(utf16str, mbstr, mbslen, code_page);
            piv::encoding::W2Uex(utf8str, utf16str.c_str(), utf16str.size());
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
                if (!(piv::encoding::IsUrlValidWord(lpszSrc[i], ReservedWord)))
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

        static const char hexUpCh[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        static const char hexLowCh[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

        /**
         * @brief 到十六进制文本
         * @param separator 是否带分隔符
         * @param hexstr 返回的十六进制文本
         * @return
         */
        template <typename StringT, typename CharT>
        std::basic_string<CharT> &str_to_hex(const StringT &str, bool separator = false, std::basic_string<CharT> &hexstr = std::basic_string<CharT>{})
        {
            hexstr.clear();
            if (str.empty())
                return hexstr;
            hexstr.reserve(str.size() * (separator ? 3 : 2));
            const unsigned char *pStr = reinterpret_cast<const unsigned char *>(str.data());
            size_t count = str.size() * sizeof(typename StringT::value_type);
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

        template <typename StringT>
        CWString &str_to_hex(const StringT &str, bool separator = false, CWString &hexstr = CWString{})
        {
            hexstr.Empty();
            if (str.empty())
                return hexstr;
            hexstr.SetNumAlignChars(str.size() * (separator ? 3 : 2));
            const unsigned char *pStr = reinterpret_cast<const unsigned char *>(str.data());
            size_t count = str.size() * sizeof(typename StringT::value_type);
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
        template <typename T, typename StringT>
        T &hex_to_str(const StringT &hexstr, T &str = T{})
        {
            str.clear();
            if (hexstr.size() < 2 * sizeof(typename StringT::value_type))
                return str;
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
            return str;
        }

        /**
         * @brief 到USC2编码
         * @param str 所欲编码的UNICODE文本
         * @param en_ascii 是否编码ASCII字符
         * @param usc2 返回的USC2文本
         * @return
         */
        template <typename CharT, typename StringT>
        std::basic_string<CharT> &to_usc2(const StringT &str, bool en_ascii = false, std::basic_string<CharT> &usc2 = std::basic_string<CharT>{})
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

        template <typename StringT>
        static CWString &to_usc2str(const StringT &str, bool en_ascii, CWString &usc2 = CWString{})
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

        /**
         * @brief USC2编码到文本
         * @param usc2 USC2编码
         * @param ret 返回文本
         * @return
         */
        template <typename StringT>
        std::wstring &usc2_to_str(const StringT &usc2, std::wstring &ret = std::wstring{})
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
        CWString &value_to_hex(const T &v, CWString &hex, bool upper = true)
        {
            const unsigned char *pValue = reinterpret_cast<const unsigned char *>(&v);
            wchar_t *pStr = const_cast<wchar_t *>(reinterpret_cast<const wchar_t *>(hex.m_mem.Alloc(((sizeof(T) * 2) + 1) * 2, TRUE)));
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
        std::basic_string<CharT> &value_to_hex(const T &v, std::basic_string<CharT> &hex, bool upper = true)
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
    PivA2W(const char *mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(mbstr, mbslen, code_page);
    }
    PivA2W(const std::string &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(mbstr, mbslen, code_page);
    }
    PivA2W(const piv::string_view &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(mbstr, mbslen, code_page);
    }
    PivA2W(const CVolMem &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(mbstr, mbslen, code_page);
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
    operator CWString() const
    {
        return CWString{utf16str.c_str()};
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
    inline void Convert(const char *mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        piv::encoding::A2Wex(utf16str, mbstr, mbslen, code_page);
    }
    inline void Convert(const std::string &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(mbstr.c_str(), (mbslen == -1) ? mbstr.size() : mbslen, code_page);
    }
    inline void Convert(const piv::string_view &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(mbstr.data(), (mbslen == -1) ? mbstr.size() : mbslen, code_page);
    }
    inline void Convert(const CVolMem &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(reinterpret_cast<const char *>(mbstr.GetPtr()), (mbslen == -1) ? static_cast<size_t>(mbstr.GetSize()) : mbslen, code_page);
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
        return utf16str.empty() ? nullptr : utf16str.data();
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
    inline CWString &GetStr(CWString &str) const
    {
        str.SetText(utf16str.c_str());
        return str;
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &GetMem(CVolMem &mem, bool null_char = false) const
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
    PivW2A(const wchar_t *utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        Convert(utf16str, utf16len, code_page);
    }
    PivW2A(const std::wstring &utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        Convert(utf16str, utf16len, code_page);
    }
    PivW2A(const CWString &utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        Convert(utf16str, utf16len, code_page);
    }
    PivW2A(const piv::wstring_view &utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        Convert(utf16str, utf16len, code_page);
    }
    PivW2A(const CVolMem &utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        Convert(utf16str, utf16len, code_page);
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
    inline void Convert(const wchar_t *utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        piv::encoding::W2Aex(mbstr, utf16str, utf16len, code_page);
    }
    inline void Convert(const std::wstring &utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        Convert(utf16str.c_str(), (utf16len == -1) ? utf16str.size() : utf16len, code_page);
    }
    inline void Convert(const CWString &utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        Convert(utf16str.GetText(), (utf16len == -1) ? static_cast<size_t>(utf16str.GetLength()) : utf16len, code_page);
    }
    inline void Convert(const piv::wstring_view &utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        Convert(utf16str.data(), (utf16len == -1) ? utf16str.size() : utf16len, code_page);
    }
    inline void Convert(const CVolMem &utf16str, size_t utf16len = -1, uint32_t code_page = CP_ACP)
    {
        Convert(reinterpret_cast<const wchar_t *>(utf16str.GetPtr()), (utf16len == -1) ? static_cast<size_t>(utf16str.GetSize() / 2) : utf16len, code_page);
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
        return mbstr.empty() ? nullptr : mbstr.data();
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
    inline CVolMem &GetMem(CVolMem &mem, bool null_char = false) const
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
    PivW2U(const wchar_t *utf16str, size_t utf16len = -1)
    {
        Convert(utf16str, utf16len);
    }
    PivW2U(const std::wstring &utf16str, size_t utf16len = -1)
    {
        Convert(utf16str, utf16len);
    }
    PivW2U(const CWString &utf16str, size_t utf16len = -1)
    {
        Convert(utf16str, utf16len);
    }
    PivW2U(const piv::wstring_view &utf16str, size_t utf16len = -1)
    {
        Convert(utf16str, utf16len);
    }
    PivW2U(const CVolMem &utf16str, size_t utf16len = -1)
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
    inline void Convert(const wchar_t *utf16str, size_t utf16len = -1)
    {
        piv::encoding::W2Uex(utf8str, utf16str, utf16len);
    }
    inline void Convert(const std::wstring &utf16str, size_t utf16len = -1)
    {
        Convert(utf16str.c_str(), (utf16len == -1) ? utf16str.size() : utf16len);
    }
    inline void Convert(const CWString &utf16str, size_t utf16len = -1)
    {
        Convert(utf16str.GetText(), (utf16len == -1) ? static_cast<size_t>(utf16str.GetLength()) : utf16len);
    }
    inline void Convert(const piv::wstring_view &utf16str, size_t utf16len = -1)
    {
        Convert(utf16str.data(), (utf16len == -1) ? utf16str.size() : utf16len);
    }
    inline void Convert(const CVolMem &utf16str, size_t utf16len = -1)
    {
        Convert(reinterpret_cast<const wchar_t *>(utf16str.GetPtr()), (utf16len == -1) ? static_cast<size_t>(utf16str.GetSize() / 2) : utf16len);
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
        return utf8str.empty() ? nullptr : utf8str.data();
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
    inline CVolMem &GetMem(CVolMem &mem, bool null_char = false) const
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
    PivU2W(const char *utf8str, size_t utf8len = -1)
    {
        Convert(utf8str, utf8len);
    }
    PivU2W(const std::string &utf8str, size_t utf8len = -1)
    {
        Convert(utf8str, utf8len);
    }
    PivU2W(const piv::string_view &utf8str, size_t utf8len = -1)
    {
        Convert(utf8str, utf8len);
    }
    PivU2W(const CVolMem &utf8str, size_t utf8len = -1)
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
    operator CWString() const
    {
        return CWString{utf16str.c_str()};
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
    inline void Convert(const char *utf8str, size_t utf8len = -1)
    {
        piv::encoding::U2Wex(utf16str, utf8str, utf8len);
    }
    inline void Convert(const std::string &utf8str, size_t utf8len = -1)
    {
        Convert(utf8str.c_str(), (utf8len == -1) ? utf8str.size() : utf8len);
    }
    inline void Convert(const piv::string_view &utf8str, size_t utf8len = -1)
    {
        Convert(utf8str.data(), (utf8len == -1) ? utf8str.size() : utf8len);
    }
    inline void Convert(const CVolMem &utf8str, size_t utf8len = -1)
    {
        Convert(reinterpret_cast<const char *>(utf8str.GetPtr()), (utf8len == -1) ? static_cast<size_t>(utf8str.GetSize()) : utf8len);
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
        return utf16str.empty() ? nullptr : utf16str.data();
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
    inline CWString &GetStr(CWString &str) const
    {
        str.SetText(utf16str.c_str());
        return str;
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &GetMem(CVolMem &mem, bool null_char = false) const
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
    PivU2A(const char *utf8str, size_t utf8len = -1, uint32_t code_page = CP_ACP)
    {
        Convert(utf8str, utf8len, code_page);
    }
    PivU2A(const std::string &utf8str, size_t utf8len = -1, uint32_t code_page = CP_ACP)
    {
        Convert(utf8str, utf8len, code_page);
    }
    PivU2A(const piv::string_view &utf8str, size_t utf8len = -1, uint32_t code_page = CP_ACP)
    {
        Convert(utf8str, utf8len, code_page);
    }
    PivU2A(const CVolMem &utf8str, size_t utf8len = -1, uint32_t code_page = CP_ACP)
    {
        Convert(utf8str, utf8len, code_page);
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
    inline void Convert(const char *utf8str, size_t utf8len = -1, uint32_t code_page = CP_ACP)
    {
        piv::encoding::U2Aex(mbstr, utf8str, utf8len, code_page);
    }
    inline void Convert(const std::string &utf8str, size_t utf8len = -1, uint32_t code_page = CP_ACP)
    {
        Convert(utf8str.c_str(), (utf8len == -1) ? utf8str.size() : utf8len, code_page);
    }
    inline void Convert(const piv::string_view &utf8str, size_t utf8len = -1, uint32_t code_page = CP_ACP)
    {
        Convert(utf8str.data(), (utf8len == -1) ? utf8str.size() : utf8len, code_page);
    }
    inline void Convert(const CVolMem &utf8str, size_t utf8len = -1, uint32_t code_page = CP_ACP)
    {
        Convert(reinterpret_cast<const char *>(utf8str.GetPtr()), (utf8len == -1) ? static_cast<size_t>(utf8str.GetSize()) : utf8len, code_page);
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
        return mbstr.empty() ? nullptr : mbstr.data();
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
    inline CVolMem &GetMem(CVolMem &mem, bool null_char = false) const
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
    PivA2U(const char *mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(mbstr, mbslen, code_page);
    }
    PivA2U(const std::string &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(mbstr, mbslen, code_page);
    }
    PivA2U(const piv::string_view &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(mbstr, mbslen, code_page);
    }
    PivA2U(const CVolMem &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(mbstr, mbslen, code_page);
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
    inline void Convert(const char *mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        piv::encoding::A2Uex(utf8str, mbstr, mbslen, code_page);
    }
    inline void Convert(const std::string &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(mbstr.c_str(), (mbslen == -1) ? mbstr.size() : mbslen, code_page);
    }
    inline void Convert(const piv::string_view &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(mbstr.data(), (mbslen == -1) ? mbstr.size() : mbslen, code_page);
    }
    inline void Convert(const CVolMem &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(reinterpret_cast<const char *>(mbstr.GetPtr()), (mbslen == -1) ? static_cast<size_t>(mbstr.GetSize()) : mbslen, code_page);
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
        return utf8str.empty() ? nullptr : utf8str.data();
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
    inline CVolMem &GetMem(CVolMem &mem, bool null_char = false) const
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
    CWString utf16str;

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
    PivA2Ws(const char *mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(mbstr, mbslen, code_page);
    }
    PivA2Ws(const std::string &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(mbstr, mbslen, code_page);
    }
    PivA2Ws(const piv::string_view &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(mbstr, mbslen, code_page);
    }
    PivA2Ws(const CVolMem &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(mbstr, mbslen, code_page);
    }

    operator const wchar_t *() const noexcept
    {
        return utf16str.GetText();
    }
    operator CWString const &() const noexcept
    {
        return utf16str;
    }
    operator CWString &() noexcept
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
    CWString *operator->() noexcept
    {
        return &utf16str;
    }
    const CWString *operator->() const noexcept
    {
        return &utf16str;
    }
    CWString &operator*() noexcept
    {
        return utf16str;
    }
    const CWString &operator*() const noexcept
    {
        return utf16str;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的ANSI文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    inline void Convert(const char *mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        piv::encoding::A2Wex(utf16str, mbstr, mbslen, code_page);
    }
    inline void Convert(const std::string &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(mbstr.c_str(), (mbslen == -1) ? mbstr.size() : mbslen, code_page);
    }
    inline void Convert(const piv::string_view &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(mbstr.data(), (mbslen == -1) ? mbstr.size() : mbslen, code_page);
    }
    inline void Convert(const CVolMem &mbstr, size_t mbslen = -1, uint32_t code_page = CP_ACP)
    {
        Convert(reinterpret_cast<const char *>(mbstr.GetPtr()), (mbslen == -1) ? static_cast<size_t>(mbstr.GetSize()) : mbslen, code_page);
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
        return utf16str.IsEmpty() ? nullptr : utf16str.m_mem.GetTextPtr();
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
    inline CWString &ToStr()
    {
        return utf16str;
    }

    /**
     * @brief 生成一个火山的文本型,其中包含了转换后的文本数据
     * @return 返回所转换的文本型
     */
    inline CWString &GetStr(CWString &str) const
    {
        str.SetText(utf16str.GetText());
        return str;
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &GetMem(CVolMem &mem, bool null_char = false) const
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
    CWString utf16str;

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
    PivU2Ws(const char *utf8str, size_t utf8len = -1)
    {
        Convert(utf8str, utf8len);
    }
    PivU2Ws(const std::string &utf8str, size_t utf8len = -1)
    {
        Convert(utf8str, utf8len);
    }
    PivU2Ws(const piv::string_view &utf8str, size_t utf8len = -1)
    {
        Convert(utf8str, utf8len);
    }
    PivU2Ws(const CVolMem &utf8str, size_t utf8len = -1)
    {
        Convert(utf8str, utf8len);
    }

    operator const wchar_t *() const noexcept
    {
        return utf16str.GetText();
    }
    operator CWString const &() const noexcept
    {
        return utf16str;
    }
    operator CWString &() noexcept
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
    CWString *operator->() noexcept
    {
        return &utf16str;
    }
    const CWString *operator->() const noexcept
    {
        return &utf16str;
    }
    CWString &operator*() noexcept
    {
        return utf16str;
    }
    const CWString &operator*() const noexcept
    {
        return utf16str;
    }

    /**
     * @brief 编码转换函数
     * @param mbstr 所欲转换的文本
     * @param mbslen 文本的字符长度,为-1时文本必须带结尾0字符
     */
    inline void Convert(const char *utf8str, size_t utf8len = -1)
    {
        piv::encoding::U2Wex(utf16str, utf8str, utf8len);
    }
    inline void Convert(const std::string &utf8str, size_t utf8len = -1)
    {
        Convert(utf8str.c_str(), (utf8len == -1) ? utf8str.size() : utf8len);
    }
    inline void Convert(const piv::string_view &utf8str, size_t utf8len = -1)
    {
        Convert(utf8str.data(), (utf8len == -1) ? utf8str.size() : utf8len);
    }
    inline void Convert(const CVolMem &utf8str, size_t utf8len = -1)
    {
        Convert(reinterpret_cast<const char *>(utf8str.GetPtr()), (utf8len == -1) ? static_cast<size_t>(utf8str.GetSize()) : utf8len);
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
        return utf16str.IsEmpty() ? nullptr : utf16str.m_mem.GetTextPtr();
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
    inline CWString &ToStr()
    {
        return utf16str;
    }

    /**
     * @brief 生成一个火山的文本型,其中包含了转换后的文本数据
     * @return 返回所转换的文本型
     */
    inline CWString &GetStr(CWString &str) const
    {
        str.SetText(utf16str.GetText());
        return str;
    }

    /**
     * @brief 将转换后的文本数据复制火山的字节集类
     * @param mem 文本数据将复制到此字节集中
     * @param null_char 字节集是否带结尾0字符
     */
    inline CVolMem &GetMem(CVolMem &mem, bool null_char = false) const
    {
        mem.Empty();
        mem.Append(utf16str.GetText(), (utf16str.GetLength() + (null_char ? 1 : 0)) * 2);
        return mem;
    }
}; // PivU2Ws

/**
 * @brief 取指定编码的文本指针,只有CWString可能会产生编码转换,其他取指针输出.
 */
template <typename CharT>
class PivFromAny
{
private:
    std::unique_ptr<std::string> m_str{nullptr};
    const CharT *m_cstr = nullptr;

public:
    PivFromAny() = delete;
    ~PivFromAny() {}

    PivFromAny(const CWString &s)
    {
        PIV_IF(sizeof(CharT) == 2)
        {
            m_cstr = s.GetText();
        }
        else
        {
            m_str.reset(new std::string{});
            piv::encoding::W2Uex(*m_str, s.GetText(), s.GetLength());
        }
    }

    PivFromAny(const std::basic_string<CharT> &s)
    {
        m_cstr = s.c_str();
    }

    PivFromAny(const CVolMem &s)
    {
        m_cstr = static_cast<const CharT *>(s.GetPtr());
    }

    PivFromAny(const CharT *s)
    {
        m_cstr = s;
    }

    /**
     * @brief 获取转换后的文本指针(至少返回空字符串)
     * @return 字符串指针
     */
    inline const CharT *GetText() const noexcept
    {
        if (m_cstr)
            return m_cstr;
        if (m_str)
            return reinterpret_cast<const CharT *>(m_str->c_str());
        return nullptr;
    }

    operator const CharT *() const noexcept
    {
        return this->GetText();
    }

    const CharT *operator*() const noexcept
    {
        return this->GetText();
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
    PivAny2U(const char *s, size_t count = -1)
    {
        if (s == nullptr || count == 0)
            return;
        if (count == -1)
            pSv.reset(new piv::string_view{s});
        else
            pSv.reset(new piv::string_view{s, count});
    }

    PivAny2U(const wchar_t *s, size_t count = -1)
    {
        if (s == nullptr || count == 0)
            return;
        pStr.reset(new std::string{});
        piv::encoding::W2Uex(*pStr, s, count);
    }

    PivAny2U(const CWString &s)
    {
        if (s.IsEmpty())
            return;
        pStr.reset(new std::string{});
        piv::encoding::W2Uex(*pStr, s.GetText(), static_cast<size_t>(s.GetLength()));
    }

    PivAny2U(const CVolMem &s)
    {
        if (s.IsEmpty())
            return;
#ifdef SIMDUTF_H
        int encodings = simdutf::detect_encodings(reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize()));
        if ((encodings & 2) == 2) // UTF-16LE
        {
            pStr.reset(new std::string{});
            piv::encoding::W2Uex(*pStr, s.GetTextPtr(), static_cast<size_t>(s.GetSize()) / 2);
        }
        else if ((encodings & 1) == 1) // UTF-8
        {
            pSv.reset(new piv::string_view{reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize())});
        }
        else // ANSI
        {
            pStr.reset(new std::string{});
            piv::encoding::A2Uex(*pStr, reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize()));
        }
#else
        pSv.reset(new piv::string_view{reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize())});
#endif
    }

    PivAny2U(const std::string &s)
    {
        if (s.empty())
            return;
        pSv.reset(new piv::string_view{s});
    }

    PivAny2U(const std::wstring &s)
    {
        if (s.empty())
            return;
        pStr.reset(new std::string{});
        piv::encoding::W2Uex(*pStr, s.c_str(), s.size());
    }

    PivAny2U(const piv::string_view &s)
    {
        if (s.empty())
            return;
        pSv.reset(new piv::string_view{s});
    }

    PivAny2U(const piv::wstring_view &s)
    {
        if (s.empty())
            return;
        pStr.reset(new std::string{});
        piv::encoding::W2Uex(*pStr, s.data(), s.size());
    }

    PivAny2U(int32_t v)
    {
        if (v == 0)
            return;
        char buf[16] = {'\0'};
        _ltoa(v, buf, 10);
        pStr.reset(new std::string{buf});
    }

    PivAny2U(int64_t v)
    {
        if (v == 0)
            return;
        char buf[32] = {'\0'};
        _i64toa(v, buf, 10);
        pStr.reset(new std::string{buf});
    }

    /**
     * @brief 获取转换后的文本指针(至少返回空字符串)
     * @return 字符串指针
     */
    inline const char *GetText() const noexcept
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
    inline char *GetPtr() noexcept
    {
        if (pStr)
            return pStr->data();
        if (pSv)
            return const_cast<char *>(pSv->data());
        return nullptr;
    }

    /**
     * @brief 获取转换后的尾字符指针(可能会返回空指针)
     * @return 尾字符指针
     */
    inline const char *GetEndPtr() const noexcept
    {
        if (pStr)
            return pStr->data() + pStr->size();
        if (pSv)
            return pSv->data() + pSv->size();
        return nullptr;
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t GetLength() const noexcept
    {
        if (pStr)
            return pStr->size();
        if (pSv)
            return pSv->size();
        return 0;
    }

    operator const char *() const noexcept
    {
        return this->GetText();
    }

    const char *operator*() const noexcept
    {
        return this->GetText();
    }
}; // PivAny2U

/**
 * @brief 任意编码转UTF-8 std::string的封装类(输出std::string)
 */
class PivAny2Us
{
private:
    std::string u8str;

public:
    PivAny2Us() = delete;
    ~PivAny2Us() {}

    /**
     * @brief 构造的同时将提供的文本转换
     * @param s 所欲转换的任意编码文本
     */
    PivAny2Us(const char *s, size_t count = -1)
    {
        if (s == nullptr || count == 0)
            return;
        if (count == -1)
            u8str.assign(s);
        else
            u8str.assign(s, count);
    }

    PivAny2Us(const wchar_t *s, size_t count = -1)
    {
        if (s == nullptr || count == 0)
            return;
        piv::encoding::W2Uex(u8str, s, count);
    }

    PivAny2Us(const CWString &s)
    {
        if (s.IsEmpty())
            return;
        piv::encoding::W2Uex(u8str, s.GetText(), static_cast<size_t>(s.GetLength()));
    }

    PivAny2Us(const CVolMem &s)
    {
        if (s.IsEmpty())
            return;
#ifdef SIMDUTF_H
        int encodings = simdutf::detect_encodings(reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize()));
        if ((encodings & 2) == 2) // UTF-16LE
        {
            piv::encoding::W2Uex(u8str, s.GetTextPtr(), static_cast<size_t>(s.GetSize()) / 2);
        }
        else if ((encodings & 1) == 1) // UTF-8
        {
            u8str.assign(reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize()));
        }
        else // ANSI
        {
            piv::encoding::A2Uex(u8str, reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize()));
        }
#else
        u8str.assign(reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize()));
#endif
    }

    PivAny2Us(const std::string &s)
    {
        if (s.empty())
            return;
        u8str.assign(s);
    }

    PivAny2Us(std::string &&s)
    {
        u8str = std::move(s);
    }

    PivAny2Us(const std::wstring &s)
    {
        if (s.empty())
            return;
        piv::encoding::W2Uex(u8str, s.c_str(), s.size());
    }

    PivAny2Us(const piv::string_view &s)
    {
        if (s.empty())
            return;
        u8str.assign(s.data(), s.size());
    }

    PivAny2Us(const piv::wstring_view &s)
    {
        if (s.empty())
            return;
        piv::encoding::W2Uex(u8str, s.data(), s.size());
    }

    PivAny2Us(int32_t v)
    {
        if (v == 0)
            return;
        char buf[16] = {'\0'};
        _ltoa(v, buf, 10);
        u8str.assign(buf);
    }

    PivAny2Us(int64_t v)
    {
        if (v == 0)
            return;
        char buf[32] = {'\0'};
        _i64toa(v, buf, 10);
        u8str.assign(buf);
    }

    /**
     * @brief 判断转换后的文本是否为空
     * @return 为空时返回真,未执行转换前始终为真
     */
    inline bool IsEmpty() const noexcept
    {
        return u8str.empty();
    }

    /**
     * @brief 获取转换后的文本指针(至少返回空字符串)
     * @return 字符串指针
     */
    inline const char *GetText() const noexcept
    {
        return u8str.c_str();
    }

    /**
     * @brief 获取转换后的文本指针(可能会返回空指针)
     * @return 字符串指针
     */
    inline char *GetPtr() noexcept
    {
        return u8str.empty() ? nullptr : u8str.data();
    }

    /**
     * @brief 获取转换后的尾字符指针
     * @return 尾字符指针
     */
    inline const char *GetEndPtr() const noexcept
    {
        return u8str.data() + u8str.size();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t GetLength() const noexcept
    {
        return u8str.size();
    }

    operator const std::string &() const noexcept
    {
        return u8str;
    }

    const std::string &operator*() const noexcept
    {
        return u8str;
    }

    /**
     * @brief 返回内部文本(转换后)的参考
     * @return 转换后的std::basic_string参考
     */
    inline std::string &String() noexcept
    {
        return u8str;
    }
    inline const std::string &String() const noexcept
    {
        return u8str;
    }
}; // PivAny2Us

/**
 * @brief 任意编码转UTF-8编码的std::string_view
 */
class PivS2V
{
private:
    piv::string_view _sv;
    std::unique_ptr<std::string> _buffer{nullptr};

public:
    PivS2V() = delete;
    ~PivS2V() {}

    /**
     * @brief 构造的同时将提供的文本转换
     * @param s 所欲转换的任意编码文本
     */
    PivS2V(const char *s, size_t count = -1)
    {
        _sv = (count == -1) ? piv::string_view{s} : piv::string_view{s, count};
    }

    PivS2V(const wchar_t *s, size_t count = -1)
    {
        _buffer.reset(new std::string{});
        piv::encoding::W2Uex(*_buffer, s, count);
        _sv = *_buffer;
    }

    PivS2V(const CWString &s)
    {
        _buffer.reset(new std::string{});
        piv::encoding::W2Uex(*_buffer, s.GetText(), static_cast<size_t>(s.GetLength()));
        _sv = *_buffer;
    }

    PivS2V(const CVolMem &s)
    {
#ifdef SIMDUTF_H
        int encodings = simdutf::detect_encodings(reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize()));
        if ((encodings & 2) == 2) // UTF-16LE
        {
            _buffer.reset(new std::string{});
            piv::encoding::W2Uex(*_buffer, s.GetTextPtr(), static_cast<size_t>(s.GetSize()) / 2);
            _sv = *_buffer;
        }
        else if ((encodings & 1) == 1) // UTF-8
        {
            _sv = piv::string_view{reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize())};
        }
        else // ANSI
        {
            _buffer.reset(new std::string{});
            piv::encoding::A2Uex(*_buffer, reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize()));
            _sv = *_buffer;
        }
#else
        _sv = piv::string_view{reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize())};
#endif
    }

    PivS2V(const std::string &s)
    {
        _sv = s;
    }

    PivS2V(const std::wstring &s)
    {
        _buffer.reset(new std::string{});
        piv::encoding::W2Uex(*_buffer, s.c_str(), s.size());
        _sv = *_buffer;
    }

    PivS2V(const piv::string_view &s)
    {
        _sv = s;
    }

    PivS2V(const piv::wstring_view &s)
    {
        _buffer.reset(new std::string{});
        piv::encoding::W2Uex(*_buffer, s.data(), s.size());
        _sv = *_buffer;
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const char *GetText() const noexcept
    {
        return _sv.data();
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const char *GetPtr() const noexcept
    {
        return _sv.empty() ? nullptr : _sv.data();
    }

    /**
     * @brief 获取转换后的尾字符指针
     * @return 尾字符指针
     */
    inline const char *GetEndPtr() const noexcept
    {
        return _sv.data() + _sv.size();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t GetLength() const noexcept
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
    std::unique_ptr<std::string> _buffer{nullptr};

public:
    PivS2AV() = delete;
    ~PivS2AV() {}

    /**
     * @brief 构造的同时将提供的文本转换
     * @param s 所欲转换的任意编码文本
     */
    PivS2AV(const char *s, size_t count = -1)
    {
        _sv = (count == -1) ? piv::string_view{s} : piv::string_view{s, count};
    }

    PivS2AV(const wchar_t *s, size_t count = -1)
    {
        _buffer.reset(new std::string{});
        piv::encoding::W2Aex(*_buffer, s, count);
        _sv = *_buffer;
    }

    PivS2AV(const CWString &s)
    {
        _buffer.reset(new std::string{});
        piv::encoding::W2Aex(*_buffer, s.GetText(), static_cast<size_t>(s.GetLength()));
        _sv = *_buffer;
    }

    PivS2AV(const CVolMem &s)
    {
#ifdef SIMDUTF_H
        int encodings = simdutf::detect_encodings(reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize()));
        if ((encodings & 2) == 2) // UTF-16LE
        {
            _buffer.reset(new std::string{});
            piv::encoding::W2Uex(*_buffer, s.GetTextPtr(), static_cast<size_t>(s.GetSize()) / 2);
            _sv = *_buffer;
        }
        else if ((encodings & 1) == 1) // UTF-8
        {
            _sv = piv::string_view{reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize())};
        }
        else // ANSI
        {
            _buffer.reset(new std::string{});
            piv::encoding::A2Uex(*_buffer, reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize()));
            _sv = *_buffer;
        }
#else
        _sv = piv::string_view{reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize())};
#endif
    }

    PivS2AV(const std::string &s)
    {
        _sv = s;
    }

    PivS2AV(const std::wstring &s)
    {
        _buffer.reset(new std::string{});
        piv::encoding::W2Aex(*_buffer, s.c_str(), s.size());
        _sv = *_buffer;
    }

    PivS2AV(const piv::string_view &s)
    {
        _sv = s;
    }

    PivS2AV(const piv::wstring_view &s)
    {
        _buffer.reset(new std::string{});
        piv::encoding::W2Aex(*_buffer, s.data(), s.size());
        _sv = *_buffer;
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const char *GetText() const noexcept
    {
        return _sv.data();
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const char *GetPtr() const noexcept
    {
        return _sv.empty() ? nullptr : _sv.data();
    }

    /**
     * @brief 获取转换后的尾字符指针
     * @return 尾字符指针
     */
    inline const char *GetEndPtr() const noexcept
    {
        return _sv.data() + _sv.size();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t GetLength() const noexcept
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
    std::unique_ptr<std::wstring> _buffer{nullptr};

public:
    PivS2WV() = delete;
    ~PivS2WV() {}

    /**
     * @brief 构造的同时将提供的文本转换
     * @param s 所欲转换的任意编码文本
     */
    PivS2WV(const wchar_t *s, size_t count = -1)
    {
        _sv = (count == -1) ? piv::wstring_view{s} : piv::wstring_view{s, count};
    }

    PivS2WV(const char *s, size_t count = -1)
    {
        _buffer.reset(new std::wstring{});
        piv::encoding::U2Wex(*_buffer, s, count);
        _sv = *_buffer;
    }

    PivS2WV(const CWString &s)
    {
        _sv = s.GetText();
    }

    PivS2WV(const CVolMem &s)
    {
#ifdef SIMDUTF_H
        int encodings = simdutf::detect_encodings(reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize()));
        if ((encodings & 2) == 2) // UTF-16LE
        {
            _sv = piv::wstring_view{s.GetTextPtr(), static_cast<size_t>(s.GetSize()) / 2};
        }
        else if ((encodings & 1) == 1) // UTF-8
        {
            _buffer.reset(new std::wstring{});
            piv::encoding::U2Wex(*_buffer, reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / 2);
            _sv = *_buffer;
        }
        else // ANSI
        {
            _buffer.reset(new std::wstring{});
            piv::encoding::A2Wex(*_buffer, reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize()));
            _sv = *_buffer;
        }
#else
        _sv = piv::wstring_view{s.GetTextPtr(), static_cast<size_t>(s.GetSize()) / 2};
#endif
    }

    PivS2WV(const std::string &s)
    {
        _buffer.reset(new std::wstring{});
        piv::encoding::U2Wex(*_buffer, s.c_str(), s.size());
        _sv = *_buffer;
    }

    PivS2WV(const std::wstring &s)
    {
        _sv = s;
    }

    PivS2WV(const piv::string_view &s)
    {
        _buffer.reset(new std::wstring{});
        piv::encoding::U2Wex(*_buffer, s.data(), s.size());
        _sv = *_buffer;
    }

    PivS2WV(const piv::wstring_view &s)
    {
        _sv = s;
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const wchar_t *GetText() const noexcept
    {
        return _sv.data();
    }

    /**
     * @brief 获取转换后的文本指针
     * @return 字符串指针
     */
    inline const wchar_t *GetPtr() const noexcept
    {
        return _sv.empty() ? nullptr : _sv.data();
    }

    /**
     * @brief 获取转换后的尾字符指针
     * @return 尾字符指针
     */
    inline const wchar_t *GetEndPtr() const noexcept
    {
        return _sv.data() + _sv.size();
    }

    /**
     * @brief 获取转换后的文本长度
     * @return 文本的字符长度
     */
    inline size_t GetLength() const noexcept
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
        template <typename CharT>
        std::basic_string<CharT> &UrlStrDecode(const ::piv::basic_string_view<CharT> &str, bool utf8 = true, std::basic_string<CharT> &decoded = std::basic_string<CharT>{})
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
        std::basic_string<CharT> &UrlStrDecode(const std::basic_string<CharT> &str, bool utf8 = true, std::basic_string<CharT> &decoded = std::basic_string<CharT>{})
        {
            return piv::encoding::UrlStrDecode(::piv::basic_string_view<CharT>{str}, utf8, decoded);
        }

        template <typename CharT>
        CVolMem &UrlStrDecode(const ::piv::basic_string_view<CharT> &str, CVolMem &decoded, bool utf8 = true)
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
        CVolMem &UrlStrDecode(const std::basic_string<CharT> &str, CVolMem &decoded, bool utf8 = true)
        {
            return piv::encoding::UrlStrDecode(::piv::basic_string_view<CharT>{str.c_str()}, decoded, utf8);
        }
        template <typename = void>
        CVolMem &UrlStrDecode(const CVolMem &str, bool utf8 = true, CVolMem &decoded = CVolMem{})
        {
            return piv::encoding::UrlStrDecode(::piv::string_view(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())), decoded, utf8);
        }
        template <typename = void>
        CWString &UrlStrDecode(const CVolMem &str, bool utf8, CWString &decoded = CWString{})
        {
            if (piv::encoding::UrlDecodeNeed(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())))
            {
                CVolMem buffer;
                piv::encoding::UrlStrDecode(::piv::string_view(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())), buffer, utf8);
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
        CWString &UrlStrDecode(const ::piv::basic_string_view<CharT> &str, CWString &decoded, bool utf8 = true)
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
        std::basic_string<CharT> &UrlStrEncode(const ::piv::basic_string_view<CharT> &str, bool utf8 = true, bool ReservedWord = false, std::basic_string<CharT> &encoded = std::basic_string<CharT>{})
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
        std::basic_string<CharT> &UrlStrEncode(const std::basic_string<CharT> &str, bool utf8 = true, bool ReservedWord = false, std::basic_string<CharT> &encoded = std::basic_string<CharT>{})
        {
            return piv::encoding::UrlStrEncode(::piv::basic_string_view<CharT>{str.c_str()}, utf8, ReservedWord, encoded);
        }

        template <typename CharT>
        CVolMem &UrlDataEncode(const basic_string_view<CharT> &str, bool utf8 = true, bool ReservedWord = false, CVolMem &encoded = CVolMem{})
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
        CVolMem &UrlDataEncode(const std::basic_string<CharT> &str, bool utf8 = true, bool ReservedWord = false, CVolMem &encoded = CVolMem{})
        {
            return piv::encoding::UrlDataEncode(std::basic_string<CharT>{str.c_str()}, utf8, ReservedWord, encoded);
        }

        template <typename = void>
        CVolMem &UrlDataEncode(const CWString &str, bool utf8 = true, bool ReservedWord = false, CVolMem &encoded = CVolMem{})
        {
            return piv::encoding::UrlDataEncode(::piv::wstring_view(str.GetText()), utf8, ReservedWord, encoded);
        }
        template <typename = void>
        CVolMem &UrlDataEncode(const CVolMem &str, bool utf8 = true, bool ReservedWord = false, CVolMem &encoded = CVolMem{})
        {
            return piv::encoding::UrlDataEncode(::piv::string_view(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())), utf8, ReservedWord, encoded);
        }
        template <typename = void>
        CWString &UrlStrEncode(const CVolMem &str, CWString &encoded, bool utf8 = true, bool ReservedWord = false)
        {
            if (piv::encoding::UrlEncodeNeed(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize()), ReservedWord))
            {
                CVolMem buffer;
                piv::encoding::UrlDataEncode<char>(::piv::string_view(reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())), utf8, ReservedWord, buffer);
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

    /**
     * @brief BASE64编解码类
     * @tparam CharT 字符类型
     */
    template <typename CharT>
    class base64
    {
    private:
        inline bool is_base64(const unsigned char &c) noexcept
        {
            return (c == 43 ||               // +
                    (c >= 47 && c <= 57) ||  // /-9
                    (c >= 65 && c <= 90) ||  // A-Z
                    (c >= 97 && c <= 122) || // a-z
                    (c == 10) || (c == 13)); // \r\n
        }
        inline void AddChar(std::basic_string<CharT> &s, const unsigned char &c, uint32_t &count, uint32_t max_line_len) noexcept
        {
            if (count < max_line_len)
            {
                s.push_back(c);
                count++;
            }
            else
            {
                s.push_back('\r');
                s.push_back('\n');
                s.push_back(c);
                count = 1;
            }
        }
        inline void AddChar(CWString &s, const unsigned char &c, uint32_t &count, uint32_t max_line_len) noexcept
        {
            if (count < max_line_len)
            {
                s.AddChar(c);
                count++;
            }
            else
            {
                s.AddText("\r\n");
                s.AddChar(c);
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
        inline T encode(const unsigned char *input, size_t len, int line_len = 0)
        {
            T ret;
            int i = 0;
            int j = 0;
            unsigned char char_array_3[3];
            unsigned char char_array_4[4];
            uint32_t count = 0;
            uint32_t max_line_len;
            if (line_len < 0)
                max_line_len = (uint32_t)-1;
            else
                max_line_len = (line_len == 0) ? 76 : static_cast<uint32_t>(line_len);
            while (len--)
            {
                char_array_3[i++] = *(input++);
                if (i == 3)
                {
                    char_array_4[0] = (unsigned char)((char_array_3[0] & 0xfc) >> 2);
                    char_array_4[1] = (unsigned char)(((char_array_3[0] & 0x03) << 4) +
                                                      ((char_array_3[1] & 0xf0) >> 4));
                    char_array_4[2] = (unsigned char)(((char_array_3[1] & 0x0f) << 2) +
                                                      ((char_array_3[2] & 0xc0) >> 6));
                    char_array_4[3] = (unsigned char)(char_array_3[2] & 0x3f);
                    for (i = 0; (i < 4); i++)
                    {
                        AddChar(ret, detail::base64_chars[char_array_4[i]], count, max_line_len);
                    }
                    i = 0;
                }
            }
            if (i)
            {
                for (j = i; j < 3; j++)
                {
                    char_array_3[j] = '\0';
                }
                char_array_4[0] = (unsigned char)((char_array_3[0] & 0xfc) >> 2);
                char_array_4[1] = (unsigned char)(((char_array_3[0] & 0x03) << 4) +
                                                  ((char_array_3[1] & 0xf0) >> 4));
                char_array_4[2] = (unsigned char)(((char_array_3[1] & 0x0f) << 2) +
                                                  ((char_array_3[2] & 0xc0) >> 6));
                char_array_4[3] = (unsigned char)(char_array_3[2] & 0x3f);
                for (j = 0; (j < i + 1); j++)
                {
                    AddChar(ret, detail::base64_chars[char_array_4[j]], count, max_line_len);
                }
                while ((i++ < 3))
                {
                    AddChar(ret, '=', count, max_line_len);
                }
            }
            return ret;
        }
        inline std::basic_string<CharT> encode(const basic_string_view<CharT> &input, int line_len = 0)
        {
            return encode<std::basic_string<CharT>>(reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT), line_len);
        }
        inline std::basic_string<CharT> encode(const std::basic_string<CharT> &input, int line_len = 0)
        {
            return encode<std::basic_string<CharT>>(reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT), line_len);
        }
        inline std::basic_string<CharT> encode(const CVolMem &input, int line_len = 0)
        {
            return encode<std::basic_string<CharT>>(reinterpret_cast<const unsigned char *>(input.GetPtr()), static_cast<size_t>(input.GetSize()), line_len);
        }

        inline CWString Encode(const unsigned char *input, size_t len, int line_len = 0)
        {
            return encode<CWString>(input, len, line_len);
        }
        inline CWString Encode(const ::piv::basic_string_view<CharT> &input, int line_len = 0)
        {
            return encode<CWString>(reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT), line_len);
        }
        inline CWString Encode(const std::basic_string<CharT> &input, int line_len = 0)
        {
            return encode<CWString>(reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT), line_len);
        }
        inline CWString Encode(const CVolMem &input, int line_len = 0)
        {
            return encode<CWString>(reinterpret_cast<const unsigned char *>(input.GetPtr()), static_cast<size_t>(input.GetSize()), line_len);
        }

        /**
         * @brief 将提供的BASE64编码文本解码
         * @param input 所欲转换的BASE64编码
         * @return 返回解码后的数据
         */
        template <typename T>
        inline void decode(const T *input, CVolMem &output, size_t len = -1)
        {
            if (len == -1)
                len = sizeof(T) == 1 ? strlen(reinterpret_cast<const char *>(input)) : wcslen(reinterpret_cast<const wchar_t *>(input));
            /*
            #ifdef SIMDUTF_H
                        output.Alloc(simdutf::maximal_binary_length_from_base64(input, len), TRUE);
                        simdutf::base64_to_binary(input, len, output.GetPtr(), simdutf::base64_default);
            #else
            */
            int i = 0;
            int j = 0;
            int in_ = 0;
            unsigned char char_array_4[4], char_array_3[3];
            output.Empty();

            while (len-- && (input[in_] != '=') && is_base64(static_cast<unsigned char>(input[in_])))
            {
                if (input[in_] == '\r' || input[in_] == '\n')
                {
                    in_++;
                    continue;
                }
                char_array_4[i++] = input[in_];
                in_++;
                if (i == 4)
                {
                    for (i = 0; i < 4; i++)
                    {
                        char_array_4[i] = static_cast<unsigned char>(detail::base64_chars.find(char_array_4[i]));
                    }

                    char_array_3[0] = (unsigned char)((char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4));
                    char_array_3[1] = (unsigned char)(((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2));
                    char_array_3[2] = (unsigned char)(((char_array_4[2] & 0x3) << 6) + char_array_4[3]);

                    for (i = 0; (i < 3); i++)
                    {
                        output.AddByte(char_array_3[i]);
                    }
                    i = 0;
                }
            }

            if (i)
            {
                for (j = i; j < 4; j++)
                    char_array_4[j] = 0;

                for (j = 0; j < 4; j++)
                    char_array_4[j] = static_cast<unsigned char>(detail::base64_chars.find(char_array_4[j]));

                char_array_3[0] = (unsigned char)((char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4));
                char_array_3[1] = (unsigned char)(((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2));
                char_array_3[2] = (unsigned char)(((char_array_4[2] & 0x3) << 6) + char_array_4[3]);

                for (j = 0; (j < i - 1); j++)
                {
                    output.AddByte(char_array_3[j]);
                }
            }
            // #endif
        }

        inline void decode(const ::piv::basic_string_view<CharT> &input, CVolMem &output)
        {
            decode(input.data(), output, input.size());
        }
        inline void decode(const std::basic_string<CharT> &input, CVolMem &output)
        {
            decode(input.c_str(), output, input.size());
        }
        inline void decode(const CVolMem &input, CVolMem &output)
        {
            decode(reinterpret_cast<const CharT *>(input.GetPtr()), output, static_cast<size_t>(input.GetSize()) / sizoef(CharT));
        }
        inline void decode(const CWString &input, CVolMem &output)
        {
            decode(input.GetText(), output, static_cast<size_t>(input.GetLength()));
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
            CVolMem output;
            decode(input, output);
            return output;
        }

    }; // base64

    /**
     * @brief BASE85编解码类
     * @tparam CharT 字符类型
     */
    template <typename CharT>
    class base85
    {
    private:
        inline void AddChar(std::basic_string<CharT> &s, const unsigned char &c) noexcept
        {
            s.push_back(c);
        }
        inline void AddChar(CWString &s, const unsigned char &c) noexcept
        {
            s.AddChar(c);
        }
        inline void ClearStr(std::basic_string<CharT> &s) noexcept
        {
            s.clear();
        }
        inline void ClearStr(CWString &s) noexcept
        {
            s.Empty();
        }

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
            ClearStr(output);
            if (len % 4)
                return false; // error: raw string size must be multiple of 4
            unsigned char tmp[5];
            for (size_t o = 0; o < len * 5 / 4; input += 4, o += 5)
            {
                unsigned value = (input[0] << 24) | (input[1] << 16) | (input[2] << 8) | input[3];
                tmp[0] = detail::base85_chars[(value / 0x31C84B1) % 0x55];
                tmp[1] = detail::base85_chars[(value / 0x95EED) % 0x55];
                tmp[2] = detail::base85_chars[(value / 0x1C39) % 0x55];
                tmp[3] = detail::base85_chars[(value / 0x55) % 0x55];
                tmp[4] = detail::base85_chars[value % 0x55];
                for (size_t i = 0; i < 5; i++)
                    AddChar(output, tmp[i]);
            }
            return true;
        }

        inline std::basic_string<CharT> encode(const ::piv::basic_string_view<CharT> &input, bool padding = false)
        {
            std::basic_string<CharT> output;
            size_t remainder = input.size() * sizeof(CharT) % 4;
            if (remainder == 0)
            {
                encode<std::basic_string<CharT>>(reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT), output);
            }
            else
            {
                if (padding)
                {
                    std::basic_string<CharT> pad{input.data(), input.size()};
                    pad.append(remainder / sizeof(CharT), '\0');
                    encode<std::basic_string<CharT>>(reinterpret_cast<const unsigned char *>(pad.data()), pad.size() * sizeof(CharT), output);
                }
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
            if (input.GetSize() % 4 == 0)
            {
                encode<CWString>(reinterpret_cast<const unsigned char *>(input.GetPtr()), static_cast<size_t>(input.GetSize()), output);
            }
            else
            {
                if (padding)
                {
                    CVolMem pad = input;
                    pad.AddManyU8Chars('\0', input.GetSize() % 4);
                    encode<CWString>(reinterpret_cast<const unsigned char *>(pad.GetPtr()), static_cast<size_t>(pad.GetSize()), output);
                }
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
        template <typename T>
        inline bool decode(const T *input, CVolMem &output, size_t len = -1)
        {
            if (len == -1)
                len = sizeof(T) == 1 ? strlen(reinterpret_cast<const char *>(input)) : wcslen(reinterpret_cast<const wchar_t *>(input));
            if (len % 5)
                return false; // error: z85 string size must be multiple of 5
            // decode
            const unsigned char decoder[128] = {
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                 // 0x00..0x0F
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                 // 0x10..0x1F
                0, 68, 0, 84, 83, 82, 72, 0, 75, 76, 70, 65, 0, 63, 62, 69,     // 0x20..0x2F
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 64, 0, 73, 66, 74, 71,            // 0x30..0x3F
                81, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, // 0x40..0x4F
                51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 77, 0, 78, 67, 0,   // 0x50..0x5F
                0, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,  // 0x60..0x6F
                25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 79, 0, 80, 0, 0,    // 0x70..0x7F
            };
            output.SetMemAlignSize(len * 4 / 5);
            for (size_t o = 0; o < len * 4 / 5; input += 5, o += 4)
            {
                uint32_t value = decoder[input[0]] * 0x31C84B1 + decoder[input[1]] * 0x95EED +
                                 decoder[input[2]] * 0x1C39 + decoder[input[3]] * 0x55 + decoder[input[4]];
                output.AddU8Char(U8CHAR((value >> 24) & 0xFF));
                output.AddU8Char(U8CHAR((value >> 16) & 0xFF));
                output.AddU8Char(U8CHAR((value >> 8) & 0xFF));
                output.AddU8Char(U8CHAR((value >> 0) & 0xFF));
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
    template <typename CharT>
    class base91
    {
    private:
        inline void AddChar(std::basic_string<CharT> &s, const unsigned char &c) noexcept
        {
            s.push_back(c);
        }
        inline void AddChar(CWString &s, unsigned char c) noexcept
        {
            s.AddChar(c);
        }

    public:
        /**
         * @brief 将提供的数据编码为BASE91文本
         * @param input 所欲转换的数据
         * @param len 所欲转换数据的长度
         * @return 返回编码后的BASE91文本
         */
        template <typename T>
        inline T encode(const unsigned char *input, size_t len)
        {
            T ret;
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
                    AddChar(ret, detail::base91_chars[val % 91]);
                    AddChar(ret, detail::base91_chars[val / 91]);
                }
            }
            /* process remaining bits from bit queue; write up to 2 bytes */
            if (nbits)
            {
                AddChar(ret, detail::base91_chars[queue % 91]);
                if (nbits > 7 || queue > 90)
                    AddChar(ret, detail::base91_chars[queue / 91]);
            }
            return ret;
        }

        inline std::basic_string<CharT> encode(const ::piv::basic_string_view<CharT> &input)
        {
            return encode<std::basic_string<CharT>>(reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT));
        }
        inline std::basic_string<CharT> encode(const std::basic_string<CharT> &input)
        {
            return encode<std::basic_string<CharT>>(reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT));
        }
        inline std::basic_string<CharT> encode(const CVolMem &input)
        {
            return encode<std::basic_string<CharT>>(reinterpret_cast<const unsigned char *>(input.GetPtr()), static_cast<size_t>(input.GetSize()));
        }

        inline CWString Encode(const ::piv::basic_string_view<CharT> &input)
        {
            return encode<CWString>(reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT));
        }
        inline CWString Encode(const std::basic_string<CharT> &input)
        {
            return encode<CWString>(reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT));
        }
        inline CWString Encode(const CVolMem &input)
        {
            return encode<CWString>(reinterpret_cast<const unsigned char *>(input.GetPtr()), static_cast<size_t>(input.GetSize()));
        }

        /**
         * @brief 将提供的BASE91编码文本解码
         * @param input 所欲转换的BASE91编码
         * @return 返回解码后的数据
         */
        template <typename T>
        inline void decode(const T *input, CVolMem &output, size_t len = -1)
        {
            if (len == -1)
                len = sizeof(T) == 1 ? strlen(reinterpret_cast<const char *>(input)) : wcslen(reinterpret_cast<const wchar_t *>(input));
            const unsigned char dectab[256] = {
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
            output.SetMemAlignSize(len * sizeof(T) * 6 / 4);
            const unsigned char *ib = reinterpret_cast<const unsigned char *>(input);

            unsigned long queue = 0;
            unsigned int nbits = 0;
            int val = -1;

            for (size_t i = len * sizeof(T); i--;)
            {
                unsigned int d = dectab[*ib++];
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
                        output.AddU8Char((U8CHAR)queue);
                        queue >>= 8;
                        nbits -= 8;
                    } while (nbits > 7);
                    val = -1; /* mark value complete */
                }
            }
            /* process remaining bits; write at most 1 byte */
            if (val != -1)
                output.AddU8Char((U8CHAR)(queue | val << nbits));
        }
        inline void decode(const ::piv::basic_string_view<CharT> &input, CVolMem &output)
        {
            decode(input.data(), output, input.size());
        }
        inline void decode(const std::basic_string<CharT> &input, CVolMem &output)
        {
            decode(input.c_str(), output, input.size());
        }
        inline void decode(const CVolMem &input, CVolMem &output)
        {
            decode(reinterpret_cast<const CharT *>(input.GetPtr()), output, static_cast<size_t>(input.GetSize()) / sizoef(CharT));
        }
        inline void decode(const CWString &input, CVolMem &output)
        {
            decode(input.GetText(), output, static_cast<size_t>(input.GetLength()));
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
