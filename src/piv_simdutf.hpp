/*********************************************\
 * 火山视窗PIV模块 - simdutf包装类           *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef _PIV_SIMDUTF_HPP
#define _PIV_SIMDUTF_HPP

#ifndef __VOL_BASE_H__
#include <sys/base/libs/win_base/vol_base.h>
#endif

// 包含simdutf头文件
#ifndef SIMDUTF_NO_PORTABILITY_WARNING
#define SIMDUTF_NO_PORTABILITY_WARNING
#endif
#include "simdutf.h"
// 包含C++标准库
#include <memory>

namespace piv
{
    namespace utf
    {

        // UTF-8转换到UTF-16LE(多种写法和类型重载)
        static void simdU2W(std::wstring &utf16str, const char *utf8str, const size_t &utf8len = 0)
        {
            size_t utf8words = (utf8len > 0) ? utf8len : strlen(utf8str);
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
        }
        static wchar_t *simdU2W(const char *utf8str, const size_t &utf8len, CVolMem &utf16str)
        {
            size_t utf8words = utf8len ? utf8len : strlen(utf8str);
            size_t utf16words = simdutf::utf16_length_from_utf8(utf8str, utf8words);
            if (!utf16words)
            {
                return L"";
            }
            utf16str.Alloc((utf16words + 1) * 2, TRUE);
            utf16words = simdutf::convert_valid_utf8_to_utf16le(utf8str, utf8words, reinterpret_cast<char16_t *>(utf16str.GetPtr()));
            return utf16words ? reinterpret_cast<wchar_t *>(utf16str.Realloc((utf16words + 1) * 2)) : L"";
        }
        static size_t simdU2W(const std::string &utf8str, CVolString &utf16str)
        {
            const char *pStr = utf8str.c_str();
            size_t len = utf8str.length();
            size_t expected_utf16words = simdutf::utf16_length_from_utf8(pStr, len);
            utf16str.SetLength(expected_utf16words);
            size_t utf16words = simdutf::convert_valid_utf8_to_utf16le(pStr, len, reinterpret_cast<char16_t *>(const_cast<WCHAR *>(utf16str.GetText())));
            if (utf16words != expected_utf16words)
            {
                utf16str.RemoveChar(utf16words, expected_utf16words - utf16words);
            }
            return utf16words;
        }
        static CVolString simdU2W(const std::string &utf8str)
        {
            CVolString utf16str;
            simdU2W(utf8str, utf16str);
            return utf16str;
        }
        static size_t simdU2W(const std::string &utf8str, CVolMem &utf16str)
        {
            const char *pStr = utf8str.c_str();
            size_t len = utf8str.length();
            size_t utf16words = simdutf::utf16_length_from_utf8(pStr, len);
            utf16str.Alloc((utf16words + 1) * 2, TRUE);
            utf16words = simdutf::convert_valid_utf8_to_utf16le(pStr, len, reinterpret_cast<char16_t *>(utf16str.GetPtr()));
            return utf16words;
        }
        static CVolString simdU2W(const char *utf8str)
        {
            size_t len = strlen(utf8str);
            size_t utf16words = simdutf::utf16_length_from_utf8(utf8str, len);
            CVolString utf16str;
            utf16str.SetLength(utf16words);
            utf16words = simdutf::convert_valid_utf8_to_utf16le(utf8str, len, reinterpret_cast<char16_t *>(const_cast<WCHAR *>(utf16str.GetText())));
            return utf16words ? utf16str.Left(utf16words) : _CT("");
        }

        // UTF-16LE转换到UTF-8(多种写法和类型重载)
        static void simdW2U(std::string &utf8str, const wchar_t *utf16str, const size_t &utf16len = 0)
        {
            size_t utf16words = (utf16len > 0) ? utf16len : wcslen(utf16str);
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
        }
        static char *simdW2U(const wchar_t *utf16str, const size_t &utf16len, CVolMem &memBuf)
        {
            size_t utf16words = utf16len ? utf16len : wcslen(utf16str);
            size_t utf8words = simdutf::utf8_length_from_utf16le(reinterpret_cast<const char16_t *>(utf16str), utf16words);
            if (!utf8words)
                return u8"";
            memBuf.Alloc((utf8words + 1) * 2, TRUE);
            utf8words = simdutf::convert_valid_utf16le_to_utf8(reinterpret_cast<const char16_t *>(utf16str), utf16words, reinterpret_cast<char *>(memBuf.GetPtr()));
            return utf8words ? reinterpret_cast<char *>(memBuf.Realloc(utf8words)) : u8"";
        }
        static size_t simdW2U(const wchar_t *utf16str, std::string &utf8str)
        {
            size_t len = wcslen(utf16str);
            size_t utf8words = simdutf::utf8_length_from_utf16le(reinterpret_cast<const char16_t *>(utf16str), len);
            if (utf8words == 0)
            {
                utf8str = "";
                return 0;
            }
            utf8str.resize(utf8words, '\0');
            return simdutf::convert_valid_utf16le_to_utf8(reinterpret_cast<const char16_t *>(utf16str), len, const_cast<char *>(reinterpret_cast<const char *>(utf8str.data())));
        }
        static size_t simdW2U(const CVolString &utf16str, std::string &utf8str)
        {
            return simdW2U(utf16str.GetText(), utf8str);
        }
        static std::string simdW2U(const CVolString &utf16str)
        {
            std::string utf8str;
            simdW2U(utf16str.GetText(), utf8str);
            return utf8str;
        }
        static std::string simdW2U(const wchar_t *utf16str)
        {
            std::string utf8str;
            simdW2U(utf16str, utf8str);
            return utf8str;
        }

        // ANSI转UTF-16LE(多种写法和类型重载)
        static void simdA2W(std::wstring &utf16str, const char *mbstr, const size_t &mbslen = 0)
        {
            int utf16len = ::MultiByteToWideChar(CP_ACP, 0, mbstr, (mbslen > 0) ? static_cast<int>(mbslen) : -1, NULL, 0);
            if (utf16len > 0)
            {
                if (mbslen == 0)
                {
                    utf16len -= 1;
                }
                utf16str.resize(utf16len, '\0');
                ::MultiByteToWideChar(CP_ACP, 0, mbstr, (mbslen > 0) ? static_cast<int>(mbslen) : -1, const_cast<wchar_t *>(utf16str.data()), utf16len);
            }
            else
            {
                utf16str = L"";
            }
        }
        static const wchar_t *simdA2W(const char *mbs, CVolMem &utf16str, size_t *utf16words)
        {
            if (!mbs)
            {
                utf16str.Empty();
                return L"";
            }
            int32_t mbslen = static_cast<int>(strlen(mbs)) + 1;
            int32_t utf16len = ::MultiByteToWideChar(CP_ACP, 0, mbs, mbslen, NULL, 0);
            if (!utf16len)
            {
                utf16str.Empty();
                return L"";
            }
            wchar_t *pwsBuf = reinterpret_cast<wchar_t *>(utf16str.Alloc(utf16len * 2, TRUE));
            utf16len = ::MultiByteToWideChar(CP_ACP, 0, mbs, mbslen, pwsBuf, utf16len);
            if (utf16len > 0)
            {
                if (utf16words)
                {
                    *utf16words = utf16len - 1;
                }
                return pwsBuf;
            }
            if (utf16words)
            {
                *utf16words = 0;
            }
            utf16str.Empty();
            return L"";
        }
        static const wchar_t *simdA2W(const CVolMem &mbs, CVolMem &utf16str, size_t *utf16words)
        {
            if (*(mbs.GetEndPtr() - 1) != 0)
            {
                CVolMem mbstr(mbs);
                mbstr.AddByte(0);
                return simdA2W(reinterpret_cast<const char *>(mbstr.GetPtr()), utf16str, utf16words);
            }
            return simdA2W(reinterpret_cast<const char *>(mbs.GetPtr()), utf16str, utf16words);
        }

        // UTF-16LE转ANSI(多种写法和类型重载)
        static void simdW2A(std::string &mbstr, const wchar_t *utf16str, const size_t &utf16len = 0)
        {
            int mbslen = ::WideCharToMultiByte(CP_ACP, 0, utf16str, (utf16len > 0) ? static_cast<int>(utf16len) : -1, NULL, 0, NULL, NULL);
            if (mbslen > 0)
            {
                if (utf16len == 0)
                {
                    mbslen -= 1;
                }
                mbstr.resize(mbslen, '\0');
                ::WideCharToMultiByte(CP_ACP, 0, utf16str, (utf16len > 0) ? static_cast<int>(utf16len) : -1, const_cast<char *>(mbstr.data()), mbslen, NULL, NULL);
            }
            else
            {
                mbstr = "";
            }
        }
        static const char *simdW2A(const wchar_t *utf16str, CVolMem &mbs, size_t *mbswords)
        {
            if (!utf16str)
            {
                mbs.Empty();
                return "";
            }
            int32_t utf16len = static_cast<int>(wcslen(utf16str)) + 1;
            int32_t mbslen = ::WideCharToMultiByte(CP_ACP, 0, utf16str, utf16len, NULL, 0, NULL, NULL);
            if (!mbslen)
            {
                mbs.Empty();
                return "";
            }
            char *psBuf = reinterpret_cast<char *>(mbs.Alloc(mbslen, TRUE));
            mbslen = ::WideCharToMultiByte(CP_ACP, 0, utf16str, utf16len, psBuf, mbslen, NULL, NULL);
            if (mbslen > 0)
            {
                if (mbswords)
                {
                    *mbswords = mbslen - 1;
                }
                return psBuf;
            }
            if (mbswords)
            {
                *mbswords = 0;
            }
            mbs.Empty();
            return "";
        }
        static const char *simdW2A(const CVolMem &utf16str, CVolMem &mbs, size_t *mbswords)
        {
            if (*(utf16str.GetEndPtr() - 2) != 0)
            {
                CVolMem szUtf16(utf16str);
                szUtf16.AddChar(0);
                return simdW2A(reinterpret_cast<const wchar_t *>(szUtf16.GetPtr()), mbs, mbswords);
            }
            return simdW2A(reinterpret_cast<const wchar_t *>(utf16str.GetPtr()), mbs, mbswords);
        }

        // ANSI转UTF-8(多种写法和类型重载)
        static void simdA2U(std::string &utf8str, const char *mbstr, const size_t &msblen = 0)
        {
            std::wstring utf16str;
            simdA2W(utf16str, mbstr, msblen);
            simdW2U(utf8str, utf16str.c_str(), -1);
        }
        static size_t simdA2U(const CVolMem &mbs, std::string &utf8str)
        {
            CVolMem buffer;
            size_t utf16words;
            const wchar_t *utf16str = simdA2W(mbs, buffer, &utf16words);
            if (utf16words == 0)
            {
                utf8str.clear();
                return 0;
            }
            return simdW2U(utf16str, utf8str);
        }
        static std::string simdA2U(const CVolMem &mbs)
        {
            std::string utf8str;
            simdA2U(mbs, utf8str);
            return utf8str;
        }

        // UTF-8转ANSI(多种写法和类型重载)
        static void simdU2A(std::string &mbstr, const char *utf8str, const size_t &utf8len = 0)
        {
            std::wstring utf16str;
            simdU2W(utf16str, utf8str, utf8len);
            simdW2A(mbstr, utf16str.c_str(), -1);
        }
        static const char *simdU2A(std::string &utf8str, CVolMem &buffer)
        {
            CVolMem utf16str;
            simdU2W(utf8str, utf16str);
            return simdW2A(utf16str, buffer, nullptr);
        }

        /********************************************************************************/

        // 多字节到UTF8
        static size_t ansi_to_utf8(const CVolMem &input, CVolMem &utf8str, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            CVolMem buffer;
            size_t utf16words;
            const char16_t *pStr = reinterpret_cast<const char16_t *>(simdA2W(input, buffer, &utf16words));
            size_t utf8words = simdutf::utf8_length_from_utf16le(pStr, utf16words);
            if (!utf8words)
            {
                utf8str.Empty();
                return 0;
            }
            char *pUtf8str = reinterpret_cast<char *>(utf8str.Alloc(utf8words + 1, TRUE));
            utf8words = ValidCheck ? simdutf::convert_utf16le_to_utf8(pStr, utf16words, pUtf8str)
                                   : simdutf::convert_valid_utf16le_to_utf8(pStr, utf16words, pUtf8str);
            utf8str.Realloc(utf8words ? (utf8words + (null_terminated ? 1 : 0)) : 0);
            return utf8words;
        }
        static CVolMem ansi_to_utf8(const CVolMem &input, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            CVolMem utf8str;
            ansi_to_utf8(input, utf8str, null_terminated, ValidCheck);
            return utf8str;
        }

        // UTF8到多字节
        static size_t utf8_to_ansi(const CVolMem &input, CVolMem &mbs, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            const char *pStr = reinterpret_cast<const char *>(input.GetPtr());
            size_t len = static_cast<size_t>(input.GetSize());
            size_t utf16words = simdutf::utf16_length_from_utf8(pStr, len);
            if (!utf16words)
            {
                mbs.Empty();
                return 0;
            }
            std::unique_ptr<char16_t[]> utf16str{new char16_t[utf16words + 1]{0}};
            utf16words = ValidCheck ? simdutf::convert_utf8_to_utf16le(pStr, len, utf16str.get())
                                    : simdutf::convert_valid_utf8_to_utf16le(pStr, len, utf16str.get());
            if (!utf16words)
            {
                mbs.Empty();
                return 0;
            }
            size_t mbswords;
            simdW2A(reinterpret_cast<const WCHAR *>(utf16str.get()), mbs, &mbswords);
            mbs.Realloc(mbswords + (null_terminated ? 1 : 0));
            return mbswords;
        }
        static CVolMem utf8_to_ansi(const CVolMem &input, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            CVolMem mbs;
            utf8_to_ansi(input, mbs, null_terminated, ValidCheck);
            return mbs;
        }

        // 自动检测编码
        static int32_t autodetect_encoding(const CVolMem &input)
        {
            return static_cast<int32_t>(simdutf::autodetect_encoding(reinterpret_cast<const char *>(input.GetPtr()),
                                                                     static_cast<size_t>(input.GetSize())));
        }

        // 检测编码
        static int32_t detect_encodings(const CVolMem &input)
        {
            return static_cast<int32_t>(simdutf::detect_encodings(reinterpret_cast<const char *>(input.GetPtr()),
                                                                  static_cast<size_t>(input.GetSize())));
        }

        // UTF8到文本
        static size_t utf8_to_utf16le(const CVolMem &input, CVolString &utf16str, const BOOL &ValidCheck)
        {
            const char *pStr = reinterpret_cast<const char *>(input.GetPtr());
            size_t len = static_cast<size_t>(input.GetSize());
            size_t expected_utf16words = simdutf::utf16_length_from_utf8(pStr, len);
            if (!expected_utf16words)
            {
                utf16str.Empty();
                return 0;
            }
            utf16str.SetLength(expected_utf16words);
            size_t utf16words = ValidCheck ? simdutf::convert_utf8_to_utf16le(pStr, len, reinterpret_cast<char16_t *>(const_cast<WCHAR *>(utf16str.GetText())))
                                           : simdutf::convert_valid_utf8_to_utf16le(pStr, len, reinterpret_cast<char16_t *>(const_cast<WCHAR *>(utf16str.GetText())));
            if (utf16words != expected_utf16words)
            {
                utf16str.RemoveChar(utf16words, expected_utf16words - utf16words);
            }
            return utf16words;
        }
        static CVolString utf8_to_utf16le(const CVolMem &input, const BOOL &ValidCheck)
        {
            CVolString utf16str;
            utf8_to_utf16le(input, utf16str, ValidCheck);
            return utf16str;
        }

        // UTF8到UTF16LE
        static size_t utf8_to_utf16le(const CVolMem &input, CVolMem &utf16str, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            const char *pStr = reinterpret_cast<const char *>(input.GetPtr());
            size_t len = static_cast<size_t>(input.GetSize());
            size_t utf16words = simdutf::utf16_length_from_utf8(pStr, len);
            if (!utf16words)
            {
                utf16str.Empty();
                return 0;
            }
            utf16str.Alloc((utf16words + 1) * 2, TRUE);
            utf16words = ValidCheck ? simdutf::convert_utf8_to_utf16le(pStr, len, reinterpret_cast<char16_t *>(utf16str.GetPtr()))
                                    : simdutf::convert_valid_utf8_to_utf16le(pStr, len, reinterpret_cast<char16_t *>(utf16str.GetPtr()));
            utf16str.Realloc(utf16words ? ((utf16words + (null_terminated ? 1 : 0)) * 2) : 0);
            return utf16words;
        }
        static CVolMem utf8_to_utf16le(const CVolMem &input, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            CVolMem utf16str;
            utf8_to_utf16le(input, utf16str, null_terminated, ValidCheck);
            return utf16str;
        }

        // UTF8到UTF16BE
        static size_t utf8_to_utf16Be(const CVolMem &input, CVolMem &utf16str, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            const char *pStr = reinterpret_cast<const char *>(input.GetPtr());
            size_t len = static_cast<size_t>(input.GetSize());
            size_t utf16words = simdutf::utf16_length_from_utf8(pStr, len);
            if (!utf16words)
            {
                utf16str.Empty();
                return 0;
            }
            utf16str.Alloc((utf16words + 1) * 2, TRUE);
            utf16words = ValidCheck ? simdutf::convert_utf8_to_utf16be(pStr, len, reinterpret_cast<char16_t *>(utf16str.GetPtr()))
                                    : simdutf::convert_valid_utf8_to_utf16be(pStr, len, reinterpret_cast<char16_t *>(utf16str.GetPtr()));
            utf16str.Realloc(utf16words ? ((utf16words + (null_terminated ? 1 : 0)) * 2) : 0);
            return utf16words;
        }
        static CVolMem utf8_to_utf16Be(const CVolMem &input, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            CVolMem utf16str;
            utf8_to_utf16Be(input, utf16str, null_terminated, ValidCheck);
            return utf16str;
        }

        // UTF8到UTF32
        static size_t utf8_to_utf32(const CVolMem &input, CVolMem &utf32str, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            const char *pStr = reinterpret_cast<const char *>(input.GetPtr());
            size_t len = static_cast<size_t>(input.GetSize());
            size_t utf32words = simdutf::utf32_length_from_utf8(pStr, len);
            if (!utf32words)
            {
                utf32str.Empty();
                return 0;
            }
            utf32str.Alloc((utf32words + 1) * 4, TRUE);
            utf32words = ValidCheck ? simdutf::convert_utf8_to_utf32(pStr, len, reinterpret_cast<char32_t *>(utf32str.GetPtr()))
                                    : simdutf::convert_valid_utf8_to_utf32(pStr, len, reinterpret_cast<char32_t *>(utf32str.GetPtr()));
            utf32str.Realloc(utf32words ? ((utf32words + (null_terminated ? 1 : 0)) * 4) : 0);
            return utf32words;
        }
        static CVolMem utf8_to_utf32(const CVolMem &input, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            CVolMem utf32str;
            utf8_to_utf32(input, utf32str, null_terminated, ValidCheck);
            return utf32str;
        }

        // 文本到UTF8
        static size_t utf16le_to_utf8(const CVolString &input, CVolMem &utf8str, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            const char16_t *pStr = reinterpret_cast<const char16_t *>(input.GetText());
            size_t len = static_cast<size_t>(input.GetLength());
            size_t utf8words = simdutf::utf8_length_from_utf16le(pStr, len);
            if (!utf8words)
            {
                utf8str.Empty();
                return 0;
            }
            utf8str.Alloc(utf8words + 1, TRUE);
            utf8words = ValidCheck ? simdutf::convert_utf16le_to_utf8(pStr, len, reinterpret_cast<char *>(utf8str.GetPtr()))
                                   : simdutf::convert_valid_utf16le_to_utf8(pStr, len, reinterpret_cast<char *>(utf8str.GetPtr()));
            utf8str.Realloc(utf8words ? (utf8words + (null_terminated ? 1 : 0)) : 0);
            return utf8words;
        }
        static CVolMem utf16le_to_utf8(const CVolString &input, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            CVolMem utf8str;
            utf16le_to_utf8(input, utf8str, null_terminated, ValidCheck);
            return utf8str;
        }

        // UTF16LE到UTF8
        static size_t utf16le_to_utf8(const CVolMem &input, CVolMem &utf8str, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            const char16_t *pStr = reinterpret_cast<const char16_t *>(input.GetPtr());
            size_t len = static_cast<size_t>(input.GetSize()) / 2;
            size_t utf8words = simdutf::utf8_length_from_utf16le(pStr, len);
            if (!utf8words)
            {
                utf8str.Empty();
                return 0;
            }
            utf8str.Alloc(utf8words + 1, TRUE);
            utf8words = ValidCheck ? simdutf::convert_utf16le_to_utf8(pStr, len, reinterpret_cast<char *>(utf8str.GetPtr()))
                                   : simdutf::convert_valid_utf16le_to_utf8(pStr, len, reinterpret_cast<char *>(utf8str.GetPtr()));
            utf8str.Realloc(utf8words ? (utf8words + (null_terminated ? 1 : 0)) : 0);
            return utf8words;
        }
        static CVolMem utf16le_to_utf8(const CVolMem &input, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            CVolMem utf8str;
            utf16le_to_utf8(input, utf8str, null_terminated, ValidCheck);
            return utf8str;
        }

        // UTF16BE到UTF8
        static size_t utf16be_to_utf8(const CVolMem &input, CVolMem &utf8str, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            const char16_t *pStr = reinterpret_cast<const char16_t *>(input.GetPtr());
            size_t len = static_cast<size_t>(input.GetSize()) / 2;
            size_t utf8words = simdutf::utf8_length_from_utf16be(pStr, len);
            if (!utf8words)
            {
                utf8str.Empty();
                return 0;
            }
            utf8str.Alloc(utf8words + 1, TRUE);
            utf8words = ValidCheck ? simdutf::convert_utf16be_to_utf8(pStr, len, reinterpret_cast<char *>(utf8str.GetPtr()))
                                   : simdutf::convert_valid_utf16be_to_utf8(pStr, len, reinterpret_cast<char *>(utf8str.GetPtr()));
            utf8str.Realloc(utf8words ? (utf8words + (null_terminated ? 1 : 0)) : 0);
            return utf8words;
        }
        static CVolMem utf16be_to_utf8(const CVolMem &input, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            CVolMem utf8str;
            utf16be_to_utf8(input, utf8str, null_terminated, ValidCheck);
            return utf8str;
        }

        // 文本到UTF32
        static size_t utf16le_to_utf32(const CVolString &input, CVolMem &utf32str, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            const char16_t *pStr = reinterpret_cast<const char16_t *>(input.GetText());
            size_t len = static_cast<size_t>(input.GetLength());
            size_t utf32words = simdutf::utf32_length_from_utf16le(pStr, len);
            if (!utf32words)
            {
                utf32str.Empty();
                return 0;
            }
            utf32str.Alloc((utf32words + 1) * 4, TRUE);
            utf32words = ValidCheck ? simdutf::convert_utf16le_to_utf32(pStr, len, reinterpret_cast<char32_t *>(utf32str.GetPtr()))
                                    : simdutf::convert_valid_utf16le_to_utf32(pStr, len, reinterpret_cast<char32_t *>(utf32str.GetPtr()));
            utf32str.Realloc(utf32words ? ((utf32words + (null_terminated ? 1 : 0)) * 4) : 0);
            return utf32words;
        }
        static CVolMem utf16le_to_utf32(const CVolString &input, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            CVolMem utf32str;
            utf16le_to_utf32(input, utf32str, null_terminated, ValidCheck);
            return utf32str;
        }

        // UTF16LE到UTF32
        static size_t utf16le_to_utf32(const CVolMem &input, CVolMem &utf32str, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            const char16_t *pStr = reinterpret_cast<const char16_t *>(input.GetPtr());
            size_t len = static_cast<size_t>(input.GetSize()) / 2;
            size_t utf32words = simdutf::utf32_length_from_utf16le(pStr, len);
            if (!utf32words)
            {
                utf32str.Empty();
                return 0;
            }
            utf32str.Alloc((utf32words + 1) * 4, TRUE);
            utf32words = ValidCheck ? simdutf::convert_utf16le_to_utf32(pStr, len, reinterpret_cast<char32_t *>(utf32str.GetPtr()))
                                    : simdutf::convert_valid_utf16le_to_utf32(pStr, len, reinterpret_cast<char32_t *>(utf32str.GetPtr()));
            utf32str.Realloc(utf32words ? ((utf32words + (null_terminated ? 1 : 0)) * 4) : 0);
            return utf32words;
        }
        static CVolMem utf16le_to_utf32(const CVolMem &input, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            CVolMem utf32str;
            utf16le_to_utf32(input, utf32str, null_terminated, ValidCheck);
            return utf32str;
        }

        // UTF16BE到UTF32
        static size_t utf16be_to_utf32(const CVolMem &input, CVolMem &utf32str, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            const char16_t *pStr = reinterpret_cast<const char16_t *>(input.GetPtr());
            size_t len = static_cast<size_t>(input.GetSize()) / 2;
            size_t utf32words = simdutf::utf32_length_from_utf16be(pStr, len);
            if (!utf32words)
            {
                utf32str.Empty();
                return 0;
            }
            utf32str.Alloc((utf32words + 1) * 4, TRUE);
            utf32words = ValidCheck ? simdutf::convert_utf16be_to_utf32(pStr, len, reinterpret_cast<char32_t *>(utf32str.GetPtr()))
                                    : simdutf::convert_valid_utf16be_to_utf32(pStr, len, reinterpret_cast<char32_t *>(utf32str.GetPtr()));
            utf32str.Realloc(utf32words ? ((utf32words + (null_terminated ? 1 : 0)) * 4) : 0);
            return utf32words;
        }
        static CVolMem utf16be_to_utf32(const CVolMem &input, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            CVolMem utf32str;
            utf16be_to_utf32(input, utf32str, null_terminated, ValidCheck);
            return utf32str;
        }

        // UTF32到UTF8
        static size_t utf32_to_utf8(const CVolMem &input, CVolMem &utf8str, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            const char32_t *pStr = reinterpret_cast<const char32_t *>(input.GetPtr());
            size_t len = static_cast<size_t>(input.GetSize()) / 4;
            size_t utf8words = simdutf::utf8_length_from_utf32(pStr, len);
            if (!utf8words)
            {
                utf8str.Empty();
                return 0;
            }
            utf8str.Alloc(utf8words + 1, TRUE);
            utf8words = ValidCheck ? simdutf::convert_utf32_to_utf8(pStr, len, reinterpret_cast<char *>(utf8str.GetPtr()))
                                   : simdutf::convert_valid_utf32_to_utf8(pStr, len, reinterpret_cast<char *>(utf8str.GetPtr()));
            utf8str.Realloc(utf8words ? (utf8words + (null_terminated ? 1 : 0)) : 0);
            return utf8words;
        }
        static CVolMem utf32_to_utf8(const CVolMem &input, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            CVolMem utf8str;
            utf32_to_utf8(input, utf8str, null_terminated, ValidCheck);
            return utf8str;
        }

        // UTF32到UTF16LE
        static size_t utf32_to_utf16le(const CVolMem &input, CVolMem &utf16str, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            const char32_t *pStr = reinterpret_cast<const char32_t *>(input.GetPtr());
            size_t len = static_cast<size_t>(input.GetSize()) / 4;
            size_t utf16words = simdutf::utf16_length_from_utf32(pStr, len);
            if (!utf16words)
            {
                utf16str.Empty();
                return 0;
            }
            utf16str.Alloc((utf16words + 1) * 2, TRUE);
            utf16words = ValidCheck ? simdutf::convert_utf32_to_utf16le(pStr, len, reinterpret_cast<char16_t *>(utf16str.GetPtr()))
                                    : simdutf::convert_valid_utf32_to_utf16le(pStr, len, reinterpret_cast<char16_t *>(utf16str.GetPtr()));
            utf16str.Realloc(utf16words ? ((utf16words + (null_terminated ? 1 : 0)) * 2) : 0);
            return utf16words;
        }
        static CVolMem utf32_to_utf16le(const CVolMem &input, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            CVolMem utf16str;
            utf32_to_utf16le(input, utf16str, null_terminated, ValidCheck);
            return utf16str;
        }

        // UTF32到文本
        static size_t utf32_to_utf16le(const CVolMem &input, CVolString &utf16str, const BOOL &ValidCheck)
        {
            const char32_t *pStr = reinterpret_cast<const char32_t *>(input.GetPtr());
            size_t len = static_cast<size_t>(input.GetSize()) / 4;
            size_t expected_utf16words = simdutf::utf16_length_from_utf32(pStr, len);
            if (!expected_utf16words)
            {
                utf16str.Empty();
                return 0;
            }
            utf16str.SetLength(expected_utf16words);
            size_t utf16words = ValidCheck ? simdutf::convert_utf32_to_utf16le(pStr, len, reinterpret_cast<char16_t *>(const_cast<WCHAR *>(utf16str.GetText())))
                                           : simdutf::convert_valid_utf32_to_utf16le(pStr, len, reinterpret_cast<char16_t *>(const_cast<WCHAR *>(utf16str.GetText())));
            if (utf16words != expected_utf16words)
            {
                utf16str.RemoveChar(utf16words, expected_utf16words - utf16words);
            }
            return utf16words;
        }
        static CVolString utf32_to_utf16le(const CVolMem &input, const BOOL &ValidCheck)
        {
            CVolString utf16str;
            utf32_to_utf16le(input, utf16str, ValidCheck);
            return utf16str;
        }

        // UTF32到UTF16BE
        static size_t utf32_to_utf16be(const CVolMem &input, CVolMem &utf16str, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            const char32_t *pStr = reinterpret_cast<const char32_t *>(input.GetPtr());
            size_t len = static_cast<size_t>(input.GetSize()) / 4;
            size_t utf16words = simdutf::utf16_length_from_utf32(pStr, len);
            if (!utf16words)
            {
                utf16str.Empty();
                return 0;
            }
            utf16str.Alloc((utf16words + 1) * 2, TRUE);
            utf16words = ValidCheck ? simdutf::convert_utf32_to_utf16be(pStr, len, reinterpret_cast<char16_t *>(utf16str.GetPtr()))
                                    : simdutf::convert_valid_utf32_to_utf16be(pStr, len, reinterpret_cast<char16_t *>(utf16str.GetPtr()));
            utf16str.Realloc(utf16words ? ((utf16words + (null_terminated ? 1 : 0)) * 2) : 0);
            return utf16words;
        }
        static CVolMem utf32_to_utf16be(const CVolMem &input, const BOOL &null_terminated, const BOOL &ValidCheck)
        {
            CVolMem utf16str;
            utf32_to_utf16be(input, utf16str, null_terminated, ValidCheck);
            return utf16str;
        }

        // 反转文本字节序
        static CVolMem change_endianness_utf16(const CVolMem &input)
        {
            CVolMem utf16str;
            utf16str.Alloc(input.GetSize(), TRUE);
            simdutf::change_endianness_utf16(reinterpret_cast<const char16_t *>(input.GetPtr()), static_cast<size_t>(input.GetSize()),
                                             reinterpret_cast<char16_t *>(utf16str.GetPtr()));
            return utf16str;
        }

        // 验证UTF8文本
        static const BOOL validate_utf8(const CVolMem &input)
        {
            return (const BOOL)simdutf::validate_utf8(reinterpret_cast<const char *>(input.GetPtr()), static_cast<size_t>(input.GetSize()));
        }

        // 验证ASCII文本
        static const BOOL validate_ascii(const CVolMem &input)
        {
            return (const BOOL)simdutf::validate_ascii(reinterpret_cast<const char *>(input.GetPtr()), static_cast<size_t>(input.GetSize()));
        }

        // 验证UTF16LE文本
        static const BOOL validate_utf16le(const CVolMem &input)
        {
            return simdutf::validate_utf16le(reinterpret_cast<const char16_t *>(input.GetPtr()), static_cast<size_t>(input.GetSize()) / 2);
        }

        // 验证UTF16BE文本
        static const BOOL validate_utf16be(const CVolMem &input)
        {
            return simdutf::validate_utf16be(reinterpret_cast<const char16_t *>(input.GetPtr()), static_cast<size_t>(input.GetSize()) / 2);
        }

        // 验证UTF32文本
        static const BOOL validate_utf32(const CVolMem &input)
        {
            return simdutf::validate_utf32(reinterpret_cast<const char32_t *>(input.GetPtr()), static_cast<size_t>(input.GetSize()) / 4);
        }

        // 取UTF8文本长度
        static int32_t count_utf8(const CVolMem &input)
        {
            return static_cast<int32_t>(simdutf::count_utf8(reinterpret_cast<const char *>(input.GetPtr()),
                                                            static_cast<size_t>(input.GetSize())));
        }

        // 取UTF16LE文本长度
        static int32_t count_utf16le(const CVolMem &input)
        {
            return static_cast<int32_t>(simdutf::count_utf16le(reinterpret_cast<const char16_t *>(input.GetPtr()),
                                                               static_cast<size_t>(input.GetSize()) / 2));
        }

        // 取UTF16BE文本长度
        static int32_t count_utf16be(const CVolMem &input)
        {
            return static_cast<int32_t>(simdutf::count_utf16be(reinterpret_cast<const char16_t *>(input.GetPtr()),
                                                               static_cast<size_t>(input.GetSize()) / 2));
        }

    } // namespace utf
} // namespace piv

#endif // _PIV_SIMDUTF_HPP
