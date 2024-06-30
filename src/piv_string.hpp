/*********************************************\
 * 火山视窗PIV模块 - 文本辅助                *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef _PIV_STRING_HPP
#define _PIV_STRING_HPP

#include "piv_encoding.hpp"
#include <vector>

namespace piv
{
    namespace edit
    {
        /**
         * @brief ASCII字符到小写
         * @param c
         * @return
         */
        static uint16_t tolower(uint16_t c) noexcept
        {
            return (c >= 'A' && c <= 'Z') ? (c + 32U) : c;
        }

        /**
         * @brief ASCII字符到大写
         * @param c
         * @return
         */
        static uint16_t toupper(uint16_t c) noexcept
        {
            return (c >= 'a' && c <= 'z') ? (c - 32U) : c;
        }

        template <typename CharT>
        size_t count(const CharT *cstr)
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

        /**
         * @brief 寻找文本(不区分大小写)
         * @param lhs 被寻找的文本
         * @param rhs 所欲寻找的文本
         * @param pos 起始搜寻索引位置
         * @return 找到的位置
         */
        template <typename StringL, typename StringR>
        size_t ifind(const StringL &lhs, const StringR &rhs, size_t pos = 0)
        {
            if (pos >= lhs.size() || rhs.empty())
                return StringL::npos;
            for (auto OuterIt = std::next(lhs.begin(), pos); OuterIt != lhs.end(); ++OuterIt)
            {
                auto InnerIt = OuterIt;
                auto SubstrIt = rhs.begin();
                for (; InnerIt != lhs.end() && SubstrIt != rhs.end(); ++InnerIt, ++SubstrIt)
                {
                    if (piv::edit::tolower(static_cast<uint16_t>(*InnerIt)) != piv::edit::tolower(static_cast<uint16_t>(*SubstrIt)))
                        break;
                }
                if (SubstrIt == rhs.end())
                    return std::distance(lhs.begin(), OuterIt);
            }
            return StringL::npos;
        }

        /**
         * @brief 倒找文本(不区分大小写)
         * @param lhs 被寻找的文本
         * @param rhs 所欲寻找的文本
         * @param pos 起始搜寻索引位置
         * @return 找到的位置
         */
        template <typename StringL, typename StringR>
        size_t irfind(const StringL &lhs, const StringR &rhs, size_t pos = (size_t)-1)
        {
            if (rhs.empty())
                return StringL::npos;
            for (auto OuterIt = std::prev(lhs.end(), lhs.size() - ((pos == (size_t)-1) ? lhs.size() : pos)); OuterIt != lhs.begin(); --OuterIt)
            {
                auto InnerIt = OuterIt;
                auto SubstrIt = rhs.begin();
                for (; InnerIt != lhs.end() && SubstrIt != rhs.end(); ++InnerIt, ++SubstrIt)
                {
                    if (piv::edit::tolower(static_cast<uint16_t>(*InnerIt)) != piv::edit::tolower(static_cast<uint16_t>(*SubstrIt)))
                        break;
                }
                if (SubstrIt == rhs.end())
                    return std::distance(lhs.begin(), OuterIt);
            }
            return StringL::npos;
        }

        /**
         * @brief 删首空
         * @param 所欲操作的文本 str
         */
        template <typename CharT>
        std::basic_string<CharT> &trim_left(std::basic_string<CharT> &str)
        {
            if (str.empty())
                return str;
            size_t count = 0;
            for (auto it = str.begin(); it != str.end(); it++, count++)
            {
                if (static_cast<uint16_t>(*it) > ' ')
                    break;
            }
            str.erase(0, count);
            return str;
        }

        template <typename CharT>
        basic_string_view<CharT> &trim_left(basic_string_view<CharT> &str)
        {
            if (str.empty())
                return str;
            size_t count = 0;
            for (auto it = str.begin(); it != str.end(); it++, count++)
            {
                if (static_cast<uint16_t>(*it) > ' ')
                    break;
            }
            str.remove_prefix(count);
            return str;
        }

        /**
         * @brief 删尾空
         * @param 所欲操作的文本 str
         */
        template <typename CharT>
        std::basic_string<CharT> &trim_right(std::basic_string<CharT> &str)
        {
            if (str.empty())
                return str;
            size_t count = 0;
            for (auto it = str.rbegin(); it != str.rend(); it++, count++)
            {
                if (static_cast<uint16_t>(*it) > ' ')
                    break;
            }
            str.erase(count);
            return str;
        }

        template <typename CharT>
        basic_string_view<CharT> &trim_right(basic_string_view<CharT> &str)
        {
            if (str.empty())
                return str;
            size_t count = 0;
            for (auto it = str.rbegin(); it != str.rend(); it++, count++)
            {
                if (static_cast<uint16_t>(*it) > ' ')
                    break;
            }
            str.remove_suffix(count);
            return str;
        }

        /**
         * @brief 不区分大小写比较文本
         * @param lhs 所欲比较的文本1
         * @param rhs 所欲比较的文本2
         * @return 返回两个文本是否相同
         */
        template <typename String_l, typename String_r>
        bool iequals(const String_l &lhs, const String_r &rhs)
        {
            size_t n = lhs.size();
            if (rhs.size() != n)
                return false;
            auto p1 = lhs.data();
            auto p2 = rhs.data();
            typename String_l::value_type a, b;
            while (n--)
            {
                a = *p1++;
                b = *p2++;
                if (a != b)
                    goto slow;
            }
            return true;
        slow:
            do
            {
                if (piv::edit::tolower(static_cast<uint16_t>(a)) != piv::edit::tolower(static_cast<uint16_t>(b)))
                    return false;
                a = *p1++;
                b = *p2++;
            } while (n--);
            return true;
        }

        /**
         * @brief 文本比较
         * @param lhs 本文本
         * @param rhs 所欲比较的文本
         * @param case_insensitive 是否区分大小写
         * @return 比较结果
         */
        template <typename StringT>
        int32_t compare(const StringT &lhs, const typename StringT::value_type *rhs, bool case_insensitive = true, size_t count = -1)
        {
            if (count == -1)
                count = piv::edit::count<typename StringT::value_type>(rhs);
            if (case_insensitive)
            {
                return lhs.compare(0, lhs.size(), rhs, count);
            }
            else
            {
                int32_t ret;
                PIV_IF(sizeof(typename StringT::value_type) == 2)
                {
                    ret = _wcsnicmp(reinterpret_cast<const wchar_t *>(lhs.data()), reinterpret_cast<const wchar_t *>(rhs), (std::min)(lhs.size(), count));
                }
                else
                {
                    ret = _strnicmp(reinterpret_cast<const char *>(lhs.data()), reinterpret_cast<const char *>(rhs), (std::min)(lhs.size(), count));
                }
                if (ret == 0 && lhs.size() != count)
                    return (lhs.size() < count) ? -1 : 1;
                return ret;
            }
        }

        template <typename StringT>
        int32_t compare(const StringT &lhs, const std::basic_string<typename StringT::value_type> &rhs, bool case_insensitive = true)
        {
            return piv::edit::compare(lhs, rhs.data(), case_insensitive, rhs.size());
        }

        template <typename StringT>
        int32_t compare(const StringT &lhs, const piv::basic_string_view<typename StringT::value_type> &rhs, bool case_insensitive = true)
        {
            return piv::edit::compare(lhs, rhs.data(), case_insensitive, rhs.size());
        }

        template <typename StringT>
        int32_t compare(const StringT &lhs, const CVolMem &rhs, bool case_insensitive = true)
        {
            return piv::edit::compare(lhs, reinterpret_cast<const typename StringT::value_type *>(rhs.GetPtr()),
                                      case_insensitive, static_cast<size_t>(rhs.GetSize() / sizeof(typename StringT::value_type)));
        }

        template <typename StringT>
        int32_t compare(const StringT &lhs, const CWString &rhs, bool case_insensitive = true)
        {
            PIV_IF(sizeof(typename StringT::value_type) == 2)
            {
                return piv::edit::compare(lhs, reinterpret_cast<const typename StringT::value_type *>(rhs.GetText()), case_insensitive);
            }
            else
            {
                return piv::edit::compare(lhs, reinterpret_cast<const typename StringT::value_type *>(PivW2U{rhs}.c_str()), case_insensitive);
            }
        }

        template <typename = void>
        std::string formatv(const char *format, va_list args)
        {
            std::string s;
            if (format && *format)
            {
                va_list args_copy;
                va_copy(args_copy, args);
                int len = std::vsnprintf(nullptr, 0, format, args_copy);
                if (len > 0)
                {
                    s.resize(len);
                    va_copy(args_copy, args);
                    std::vsprintf((char *)s.data(), format, args_copy);
                }
            }
            return s;
        }
        template <typename = void>
        std::wstring formatv(const wchar_t *format, va_list args)
        {
            std::wstring s;
            if (format && *format)
            {

                va_list args_copy;
                while (true)
                {
                    s.resize(s.capacity());
                    va_copy(args_copy, args);
                    int len = std::vswprintf((wchar_t *)(&s[0]), s.size(), format, args_copy);
                    if (len == -1)
                        s.reserve(s.capacity() * 2);
                    else
                    {
                        s.resize(len);
                        break;
                    }
                }
            }
            return s;
        }

        /**
         * @brief std::string 格式化
         * @param format
         */
        template <typename = void>
        std::string format(const char *format, ...)
        {
            std::string s;
            if (format && *format)
            {
                va_list args;
                va_start(args, format);
                s = formatv(format, args);
                va_end(args);
            }
            return s;
        }

        /**
         * @brief std::wstring 格式化
         * @param format
         */
        template <typename = void>
        std::wstring format(const wchar_t *format, ...)
        {
            std::wstring s;
            if (format && *format)
            {
                va_list args;
                va_start(args, format);
                s = formatv(format, args);
                va_end(args);
            }
            return s;
        }

        /**
         * @brief 取逆序文本
         */
        template <typename EncodeT, typename StringT>
        std::basic_string<typename StringT::value_type> reverse_text(const StringT &str)
        {
            std::basic_string<typename StringT::value_type> reverse;
            size_t n = str.size(), i = 0;
            typename StringT::value_type ch;
            reverse.resize(n);
            PIV_IF(sizeof(EncodeT) == 2)
            {
                while (n--)
                {
                    ch = str[i++];
                    if (ch >= (typename StringT::value_type)0xD800 && ch <= (typename StringT::value_type)0xDFFF)
                    {
                        reverse[n] = str[i++];
                        reverse[--n] = ch;
                    }
                    else
                    {
                        reverse[n] = ch;
                    }
                }
            }
            PIV_ELSE_IF(sizeof(EncodeT) == 3)
            {
                size_t len = n;
                while (n--)
                {
                    ch = str[i];
                    if (ch >> 7 == 0) // c <= 0x7F 1字节
                    {
                        reverse[n] = ch;
                        i++;
                    }
                    else if (ch >= (char)0xE0 && ch <= (char)0xEF && i + 2 <= len) // 3字节
                    {
                        reverse[n] = str[i + 2];
                        reverse[--n] = str[i + 1];
                        reverse[--n] = ch;
                        i += 3;
                    }
                    else if (ch >= (char)0xC0 && ch <= (char)0xDF && i + 1 <= len) // 2字节
                    {
                        reverse[n] = str[i + 1];
                        reverse[--n] = ch;
                        i += 2;
                    }
                    else if (ch >= (char)0xF0 && ch <= (char)0xF7 && i + 3 <= len) // 4字节
                    {
                        reverse[n] = str[i + 3];
                        reverse[--n] = str[i + 2];
                        reverse[--n] = str[i + 1];
                        reverse[--n] = ch;
                        i += 4;
                    }
                }
            }
            else
            {
                while (n--)
                {
                    ch = str[i++];
                    if (ch >> 7 == 0)
                    {
                        reverse[n] = ch;
                    }
                    else
                    {
                        reverse[n] = str[i++];
                        reverse[--n] = ch;
                    }
                }
            }
            return reverse;
        }

        /**
         * @brief 到全角
         * @param option 转换选项 1 2 4
         * @return 返回转换后的文本
         */
        template <typename EncodeT, typename StringT>
        std::basic_string<typename StringT::value_type> to_fullwidth(const StringT &str, int32_t option)
        {
            std::basic_string<typename StringT::value_type> ret;
            if (str.empty())
                return ret;
            ret.reserve(str.size() * 2);
            ret = str;
            typename StringT::value_type ch;
            PIV_IF(sizeof(EncodeT) == 2)
            {
                for (auto it = ret.begin(); it != ret.end(); it++)
                {
                    ch = *it;
                    if (ch == ' ')
                    {
                        *it = static_cast<typename StringT::value_type>(0x3000);
                    }
                    else if ((1 & option && ((ch >= '!' && ch <= '/') || (ch >= ':' && ch <= '@') || (ch >= '[' && ch <= '`') || (ch >= '{' && ch <= '~'))) ||
                             (2 & option && ch >= '0' && ch <= '9') ||
                             (4 & option && ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))))
                    {
                        *it = ch + static_cast<typename StringT::value_type>(0xFEE0);
                    }
                }
            }
            PIV_ELSE_IF(sizeof(EncodeT) == 3)
            {
                typename StringT::value_type qj[4]{};
                for (size_t i = 0; i < ret.size(); i++)
                {
                    ch = ret[i];
                    if (ch == ' ')
                    {
                        ret[i] = static_cast<typename StringT::value_type>(0xE3);
                        ret.insert(++i, 1, static_cast<typename StringT::value_type>(0x80));
                        ret.insert(++i, 1, static_cast<typename StringT::value_type>(0x80));
                    }
                    else
                    {
                        if ((1 & option && ((ch >= '!' && ch <= '/') || (ch >= ':' && ch <= '@') || (ch >= '[' && ch <= '_'))) ||
                            (2 & option && ch >= '0' && ch <= '9') ||
                            (4 & option && ch >= 'A' && ch <= 'Z'))
                        {
                            *((int32_t *)qj) = ch + 0xEFBC60;
                            ret[i] = qj[2];
                            ret.insert(++i, 1, qj[1]);
                            ret.insert(++i, 1, qj[0]);
                        }
                        else if ((4 & option && ch >= 'a' && ch <= 'z') ||
                                 (1 & option && (ch == '`' || (ch >= '{' && ch <= '~'))))
                        {
                            *((int32_t *)qj) = ch + 0xEFBD20;
                            ret[i] = qj[2];
                            ret.insert(++i, 1, qj[1]);
                            ret.insert(++i, 1, qj[0]);
                        }
                    }
                }
            }
            else
            {
                typename StringT::value_type qj[2]{};
                for (size_t i = 0; i < ret.size(); i++)
                {
                    ch = ret[i];
                    if (ch == ' ')
                    {
                        ret[i] = static_cast<typename StringT::value_type>(0xA1);
                        ret.insert(i++, 1, static_cast<typename StringT::value_type>(0xA1));
                    }
                    else if ((1 & option && ((ch >= '!' && ch <= '/') || (ch >= ':' && ch <= '@') || (ch >= '[' && ch <= '`') || (ch >= '{' && ch <= '~'))) ||
                             (2 & option && ch >= '0' && ch <= '9') ||
                             (4 & option && ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))))
                    {
                        *((int16_t *)qj) = ret[i] + 0xA380;
                        ret[i] = qj[0];
                        ret.insert(i++, 1, qj[1]);
                    }
                }
            }
            return ret;
        }

        /**
         * @brief 到半角
         * @param option 转换选项 1 2 4
         * @return 返回转换后的文本
         */
        template <typename EncodeT, typename StringT>
        std::basic_string<typename StringT::value_type> to_halfwidth(const StringT &str, int32_t option)
        {
            std::basic_string<typename StringT::value_type> ret;
            if (str.empty())
                return ret;
            ret.reserve(str.size());
            typename StringT::value_type ch;
            PIV_IF(sizeof(EncodeT) == 2)
            {
                ret = str;
                for (auto it = ret.begin(); it != ret.end(); it++)
                {
                    ch = *it;
                    if (ch == 0x3000)
                    {
                        *it = ' ';
                    }
                    else if ((1 & option && ((ch >= 0xFF01 && ch <= 0xFF0F) || (ch >= 0xFF1A && ch <= 0xFF20) || (ch >= 0xFF3B && ch <= 0xFF40) || (ch >= 0xFF5B && ch <= 0xFF5E))) ||
                             (2 & option && ch >= 0xFF10 && ch <= 0xFF19) ||
                             (4 & option && ((ch >= 0xFF21 && ch <= 0xFF3A) || (ch >= 0xFF41 && ch <= 0xFF5A))))
                    {
                        *it = ch - 0xFEE0;
                    }
                }
            }
            PIV_ELSE_IF(sizeof(EncodeT) == 3)
            {
                uint32_t fullCh;
                size_t len = str.size();
                for (size_t i = 0; i < len; i++)
                {
                    ch = str[i];
                    if (ch >= (char)0xE0 && ch <= (char)0xEF && i + 2 < len)
                    {
                        fullCh = (((uint32_t)((((uint16_t)str[i + 2]) & 0xFF) | (((uint16_t)str[i + 1]) << 8))) & 0xFFFF) | (((uint32_t)(uint8_t)ch) << 16);
                        if (fullCh == 0xE38080)
                        {
                            ret.push_back(' ');
                        }
                        else if ((1 & option && ((fullCh >= 0xEFBC81 && fullCh <= 0xEFBC8F) || (fullCh >= 0xEFBC9A && fullCh <= 0xEFBCA0) || (fullCh >= 0xEFBCBB && fullCh <= 0xEFBCBF))) ||
                                 (2 & option && fullCh >= 0xEFBC90 && fullCh <= 0xEFBC99) ||
                                 (4 & option && fullCh >= 0xEFBCA1 && fullCh <= 0xEFBCBA))
                        {
                            ret.push_back(static_cast<typename StringT::value_type>(fullCh - 0xEFBC60));
                        }
                        else if ((4 & option && fullCh >= 0xEFBD81 && fullCh <= 0xEFBD9A) ||
                                 (1 & option && (fullCh == 0xEFBD80 || (fullCh >= 0xEFBD9B && fullCh <= 0xEFBD9E))))
                        {
                            ret.push_back(static_cast<typename StringT::value_type>(fullCh - 0xEFBD20));
                        }
                        else
                        {
                            ret.push_back(ch);
                            ret.push_back(str[i + 1]);
                            ret.push_back(str[i + 2]);
                        }
                        i += 2;
                    }
                    else
                    {
                        ret.push_back(ch);
                    }
                }
            }
            else
            {
                uint16_t fullCh;
                size_t len = str.size();
                for (size_t i = 0; i < len; i++)
                {
                    ch = str[i];
                    if (ch >> 7 != 0 && (i + 1 < len))
                    {
                        fullCh = (((uint16_t)str[i + 1]) & 0xFF) | (((uint16_t)ch) << 8);
                        if (fullCh == 0xA1A1)
                        {
                            ret.push_back(' ');
                        }
                        else if ((1 & option && ((fullCh >= 0xA3A1 && fullCh <= 0xA3AF) || (fullCh >= 0xA3BA && fullCh <= 0xA3C0) || (fullCh >= 0xA3DB && fullCh <= 0xA3E0) || (fullCh >= 0xA3FB && fullCh <= 0xA3FE))) ||
                                 (2 & option && fullCh >= 0xA3B0 && fullCh <= 0xA3B9) ||
                                 (4 & option && ((fullCh >= 0xA3C1 && fullCh <= 0xA3DA) || (fullCh >= 0xA3E1 && fullCh <= 0xA3FA))))
                        {
                            ret.push_back(static_cast<typename StringT::value_type>(fullCh - 0xA380));
                        }
                        else
                        {
                            ret.push_back(ch);
                            ret.push_back(str[i + 1]);
                        }
                        i++;
                    }
                    else
                    {
                        ret.push_back(ch);
                    }
                }
            }
            return ret;
        }

        /**
         * @brief 取文字长度
         */
        template <typename EncodeT, typename StringT>
        size_t count_with_word(const StringT &str)
        {
            typename StringT::value_type ch;
            size_t count = 0;
            PIV_IF(sizeof(EncodeT) == 2)
            {
                for (size_t i = 0; i < str.size(); i++, count++)
                {
                    ch = str[i];
                    if (ch >= (typename StringT::value_type)0xD800 && ch <= (typename StringT::value_type)0xDFFF)
                        i++;
                }
            }
            PIV_ELSE_IF(sizeof(EncodeT) == 3)
            {
                for (size_t i = 0; i < str.size(); i++, count++)
                {
                    ch = str[i];
                    // if (ch >> 7 == 0) // c <= 0x7F 1字节
                    if (ch >= (typename StringT::value_type)0xE0 && ch <= (typename StringT::value_type)0xEF) // 3字节
                        i += 2;
                    else if (ch >= (typename StringT::value_type)0xC0 && ch <= (typename StringT::value_type)0xDF) // 2字节
                        i++;
                    else if (ch >= (typename StringT::value_type)0xF0 && ch <= (typename StringT::value_type)0xF7) // 4字节
                        i += 3;
                }
            }
            else
            {
                for (size_t i = 0; i < str.size(); i++, count++)
                {
                    if (str[i] >> 7 != 0)
                        i++;
                }
            }
            return count;
        }

        /**
         * @brief 文字长度转换到字符长度
         * @param str 所欲操作文本
         * @param pos 提供文字索引,返回字符索引
         * @param count 提供文字个数,返回字符个数
         */
        template <typename EncodeT, typename StringT>
        void pos_with_word(const StringT &str, size_t &pos, size_t &count)
        {
            if (count == 0)
                return;
            size_t length = str.size();
            size_t spos = 0, cb = 0;
            typename StringT::value_type ch;
            for (size_t i = 0, n = 0, c = 0; i < length; i++, n++)
            {
                if (n == pos)
                    spos = i;
                ch = str[i];
                PIV_IF(sizeof(EncodeT) == 2)
                {
                    if (ch >= (typename StringT::value_type)0xD800 && ch <= (typename StringT::value_type)0xDFFF)
                        i++;
                }
                PIV_ELSE_IF(sizeof(EncodeT) == 3)
                {
                    if (ch >= (typename StringT::value_type)0xE0 && ch <= (typename StringT::value_type)0xEF) // 3字节
                        i += 2;
                    else if (ch >= (typename StringT::value_type)0xC0 && ch <= (typename StringT::value_type)0xDF) // 2字节
                        i++;
                    else if (ch >= (typename StringT::value_type)0xF0 && ch <= (typename StringT::value_type)0xF7) // 4字节
                        i += 3;
                }
                else
                {
                    if (ch >> 7 != 0)
                        i++;
                }

                if (n + 1 > pos && ++c == count)
                {
                    cb = i - spos + 1;
                    break;
                }
            }
            pos = spos;
            count = (cb != 0) ? cb : (str.size() - spos);
        }

        /**
         * @brief 取子文本(以文字为单位)
         * @param str 所欲取子文本的文本
         * @param pos 起始位置
         * @param count 文字长度
         * @return
         */
        template <typename EncodeT, typename StringT>
        StringT substr_with_word(const StringT &str, size_t pos, size_t count)
        {
            pos_with_word<EncodeT, StringT>(str, pos, count);
            if (pos < str.size() && count > 0)
                return str.substr(pos, count);
            return StringT{};
        }

        /**
         * @brief 删除文字
         * @param str 所欲操作的文本
         * @param pos 删除的起始位置
         * @param count 删除的文字数量
         */
        template <typename EncodeT, typename CharT>
        std::basic_string<CharT> &remove_words(std::basic_string<CharT> &str, size_t pos, size_t count)
        {
            pos_with_word<EncodeT>(str, pos, count);
            if (pos < str.size() && count > 0)
                str.erase(pos, count);
            return str;
        }

        /**
         * @brief 寻找文本
         * @tparam CharT 字符类型
         * @param lhs 被寻找文本
         * @param rhs 欲寻找文本
         * @param pos 起始搜寻位置
         * @param case_insensitive 是否不区分大小写
         * @return 寻找到的位置
         */
        template <typename StringL, typename StringR>
        size_t search(const StringL &lhs, const StringR &rhs, size_t pos = 0, bool case_insensitive = false)
        {
            if (!case_insensitive)
                return lhs.find(rhs.data(), pos, rhs.size());
            else
                return piv::edit::ifind(lhs, rhs, pos);
        }

        /**
         * @brief 倒找文本
         * @tparam CharT 字符类型
         * @param lhs 被寻找文本
         * @param rhs 欲寻找文本
         * @param pos 起始搜寻位置
         * @param case_insensitive 是否不区分大小写
         * @return 寻找到的位置
         */
        template <typename StringL, typename StringR>
        size_t rsearch(const StringL &lhs, const StringR &rhs, size_t pos = -1, bool case_insensitive = false)
        {
            if (!case_insensitive)
                return lhs.rfind(rhs.data(), (pos == -1) ? lhs.size() : pos);
            else
                return piv::edit::irfind(lhs, rhs, (pos == -1) ? lhs.size() : pos);
        }

        /**
         * @brief 是否以文本开头
         * @param lhs 本文本
         * @param rhs 所欲检查的文本
         * @param case_sensitive 是否区分大小写
         * @return
         */
        template <typename StringL, typename StringR>
        size_t starts_with(const StringL &lhs, const StringR &rhs, bool case_sensitive = true)
        {
            return piv::edit::search(lhs, rhs, 0, !case_sensitive) == 0;
        }

        /**
         * @brief 是否以文本结束
         * @param lhs 本文本
         * @param str 所欲检查的文本
         * @param case_sensitive 是否区分大小写
         * @return
         */
        template <typename StringL, typename StringR>
        size_t ends_with(const StringL &lhs, const StringR &rhs, bool case_sensitive = true)
        {
            if (rhs.size() > lhs.size())
                return false;
            return piv::edit::rsearch(lhs, rhs, -1, !case_sensitive) == (lhs.size() - rhs.size());
        }

        /**
         * @brief 分割文本
         * @param lhs 欲分割文本
         * @param delimit 分割字符集合
         * @param arr 结果存放数组
         * @param trimAll 是否删除首尾空
         * @param ignoreEmptyStr 是否忽略空白结果
         * @param max_count 最大分割数
         * @return 所分割出来的结果文本数目
         */
        template <typename StringT, typename ViewT, typename ValueT>
        size_t split(StringT &lhs, const ViewT &delimit, std::vector<ValueT> &arr, bool trimAll = true, bool ignoreEmptyStr = true, size_t max_count = -1)
        {
            arr.clear();
            if (delimit.empty())
            {
                arr.emplace_back(lhs);
                return arr.size();
            }
            size_t n = 0, spos = 0, fpos = 0, epos = lhs.size();
            ViewT sub;
            ViewT sv{lhs};
            while (fpos < epos)
            {
                fpos = sv.find_first_of(delimit, spos);
                if (fpos != StringT::npos && ++n < max_count)
                {
                    sub = sv.substr(spos, fpos - spos);
                    spos = fpos + 1;
                }
                else
                {
                    sub = sv.substr(spos, sv.size() - spos);
                    fpos = epos;
                }
                if (trimAll)
                {
                    piv::edit::trim_left(sub);
                    piv::edit::trim_right(sub);
                    if (ignoreEmptyStr && sub.empty())
                        continue;
                }
                if (ignoreEmptyStr)
                {
                    if (sub.empty())
                        continue;
                    bool isEmpty = true;
                    for (size_t i = 0; i < sub.size(); i++)
                    {
                        if (static_cast<uint16_t>(sub[i]) > ' ')
                        {
                            isEmpty = false;
                            break;
                        }
                    }
                    if (isEmpty)
                        continue;
                }
                arr.emplace_back(sub);
            }
            return arr.size();
        }

        /**
         * @brief 分割子文本
         * @param lhs 欲分割文本
         * @param delimit 分割用子文本
         * @param arr 结果存放数组
         * @param trimAll 是否删除首尾空
         * @param ignoreEmptyStr 是否忽略空白结果
         * @param max_count 最大分割数
         * @return 所分割出来的结果文本数目
         */
        template <typename StringT, typename ViewT, typename ValueT>
        size_t split_substr(StringT &lhs, const ViewT &delimit, std::vector<ValueT> &arr, bool trimAll = true, bool ignoreEmptyStr = true, size_t max_count = -1)
        {
            arr.clear();
            if (delimit.empty())
            {
                arr.emplace_back(lhs);
                return arr.size();
            }
            size_t n = 0, spos = 0, fpos = 0, epos = lhs.size();
            ViewT sub;
            ViewT sv{lhs};
            while (fpos < epos)
            {
                fpos = sv.find(delimit, spos);
                if (fpos != StringT::npos && ++n < max_count)
                {
                    sub = sv.substr(spos, fpos - spos);
                    spos = fpos + delimit.size();
                }
                else
                {
                    sub = sv.substr(spos, sv.size() - spos);
                    fpos = epos;
                }
                if (trimAll)
                {
                    piv::edit::trim_left(sub);
                    piv::edit::trim_right(sub);
                    if (ignoreEmptyStr && sub.empty())
                        continue;
                }
                if (ignoreEmptyStr)
                {
                    if (sub.empty())
                        continue;
                    bool isEmpty = true;
                    for (size_t i = 0; i < sub.size(); i++)
                    {
                        if (static_cast<uint16_t>(sub[i]) > ' ')
                        {
                            isEmpty = false;
                            break;
                        }
                    }
                    if (isEmpty)
                        continue;
                }
                arr.emplace_back(sub);
            }
            return arr.size();
        }

        /**
         * @brief 取子文本中间
         * @param lhs 被查找的文本
         * @param lstr 开始文本
         * @param rstr 结束文本
         * @param pos 起始搜寻位置
         * @param case_insensitive 是否不区分大小写
         * @param include_lstr 是否包含开始文本
         * @param include_rstr 是否包含结束文本
         * @param retorg_failed 失败返回原文本
         * @return 返回取出的子文本
         */
        template <typename StringT, typename StringL, typename StringR>
        StringT get_middle(const StringT &lhs, const StringL &lstr, const StringR &rstr, size_t pos, bool case_insensitive = false, bool include_lstr = false, bool include_rstr = false, bool retorg_failed = false)
        {
            size_t spos = piv::edit::search(lhs, lstr, pos, case_insensitive);
            if (spos == -1)
                return retorg_failed ? lhs : StringT{};
            size_t epos = spos + lstr.size();
            if (!include_lstr)
                spos = epos;
            epos = piv::edit::search(lhs, rstr, epos, case_insensitive);
            if (epos == -1)
                return retorg_failed ? lhs : StringT{};
            return lhs.substr(spos, epos - spos + (include_rstr ? rstr.size() : 0));
        }

        template <typename StringT, typename StringL, typename StringR>
        size_t get_middles(const StringT &lhs, std::vector<StringT> &arr, const StringL &lstr, const StringR &rstr, size_t pos, bool case_insensitive = false, bool include_lstr = false, bool include_rstr = false)
        {
            arr.clear();
            size_t spos = pos, epos = 0;
            while (true)
            {
                spos = piv::edit::search(lhs, lstr, spos, case_insensitive);
                if (spos == -1)
                    break;
                epos = spos + lstr.size();
                if (!include_lstr)
                    spos = epos;
                epos = piv::edit::search(lhs, rstr, epos, case_insensitive);
                if (epos == -1)
                    break;
                arr.emplace_back(lhs.substr(spos, epos - spos + (include_rstr ? rstr.size() : 0)));
            }
            return arr.size();
        }

        /**
         * @brief 取子文本左边
         * @param lhs 被查找的文本
         * @param rhs 要查找的文本
         * @param pos 起始搜寻位置
         * @param case_insensitive 是否不区分大小写
         * @param include_str 是否包含查找的文本
         * @param reverse 是否反向查找
         * @param retorg_failed 失败返回原文本
         * @return 返回取出的子文本
         */
        template <typename StringT, typename StringR>
        StringT get_left(const StringT &lhs, const StringR &rhs, size_t pos, bool case_insensitive = false, bool include_str = false, bool reverse = false, bool retorg_failed = false)
        {
            size_t spos;
            if (reverse)
                spos = piv::edit::rsearch(lhs, rhs, pos, case_insensitive);
            else
                spos = piv::edit::search(lhs, rhs, pos, case_insensitive);
            if (spos == -1)
                return retorg_failed ? lhs : StringT{};
            return lhs.substr(0, spos + (include_str ? rhs.size() : 0));
        }

        /**
         * @brief 取子文本右边
         * @param lhs 被查找的文本
         * @param rhs 要查找的文本
         * @param pos 起始搜寻位置
         * @param case_insensitive 是否不区分大小写
         * @param include_str 是否包含查找的文本
         * @param reverse 是否反向查找
         * @param retorg_failed 失败返回原文本
         * @return 返回取出的子文本
         */
        template <typename StringT, typename StringL>
        StringT get_right(const StringT &lhs, const StringL &rhs, size_t pos, bool case_insensitive = false, bool include_str = false, bool reverse = false, bool retorg_failed = false)
        {
            size_t spos = 0;
            if (reverse)
                spos = piv::edit::rsearch(lhs, rhs, pos, case_insensitive);
            else
                spos = piv::edit::search(lhs, rhs, pos, case_insensitive);
            if (spos == -1)
                return retorg_failed ? lhs : StringT{};
            if (!include_str)
                spos += rhs.size();
            return lhs.substr(spos, lhs.size() - spos);
        }

        /**
         * @brief 加载资源
         * @tparam CharT 字符类型
         * @param lhs 本文本视图
         * @param resId 资源ID
         * @return 是否成功
         */
        template <typename CharT>
        bool load_resdata(basic_string_view<CharT> &lhs, int32_t resId)
        {
            lhs.swap(basic_string_view<CharT>{});
            if (resId == 0)
                return false;
            HMODULE hModule = g_objVolApp.GetInstanceHandle();
            HRSRC hSrc = ::FindResourceW(hModule, MAKEINTRESOURCEW(static_cast<WORD>(resId)), RT_RCDATA);
            if (hSrc == NULL)
                return false;
            HGLOBAL resdata = ::LoadResource(hModule, hSrc);
            if (resdata == NULL)
                return false;
            uint8_t *data = reinterpret_cast<uint8_t *>(::LockResource(resdata));
            size_t count = static_cast<size_t>(::SizeofResource(hModule, hSrc));
            if (count > 2)
            {
                if (data[0] == 0xFF && data[1] == 0xFE)
                {
                    data += 2;
                    count -= 2;
                }
                else if (data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF)
                {
                    data += 3;
                    count -= 3;
                }
            }
            lhs.swap(basic_string_view<CharT>{reinterpret_cast<const CharT *>(data), count / sizeof(CharT)});
            return true;
        }

    } // namespace edit

    namespace detail
    {
        /**
         * @brief 文本小于比较(不区分大小写)
         */
        template <typename KeyT>
        struct ci_less
        {
            struct nocase_compare
            {
                bool operator()(const int &c1, const int &c2) const
                {
                    return (piv::edit::tolower(static_cast<uint16_t>(c1)) < piv::edit::tolower(static_cast<uint16_t>(c2)));
                }
            };
            bool operator()(const KeyT &s1, const KeyT &s2) const
            {
                return std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end(), nocase_compare());
            }
        };

        /**
         * @brief 取大小写无关哈希值
         */
        template <typename StringT>
        size_t ci_hash(StringT &&str)
        {
            size_t upHash = 0;
            for (size_t Idx = 0; Idx < str.size(); ++Idx)
            {
                const size_t upChar = str[Idx];
                upHash *= 5;
                if (upChar >= 'a' && upChar <= 'z') // 为小写字母?
                    upHash += (upChar - 'a' + 'A'); // 转换为大写字母加入
                else
                    upHash += upChar;
            }
            return (upHash | 1);
        }

    } // namespace detail

} // namespace piv

#endif // _PIV_STRING_HPP

#ifndef _PIV_STD_STRING_HPP
#define _PIV_STD_STRING_HPP
namespace piv
{
    namespace str
    {
        /**
         * @brief 置文本
         * @tparam CharT 字符类型
         * @param lhs 本文本
         * @param rhs 所欲赋值的数据
         * @param count 所欲赋值的数据长度
         * @return 返回本文本
         */
        template <typename CharT>
        std::basic_string<CharT> &assign(std::basic_string<CharT> &lhs, const std::basic_string<CharT> &rhs)
        {
            return lhs.assign(rhs);
        }

        template <typename CharT>
        std::basic_string<CharT> &assign(std::basic_string<CharT> &lhs, std::basic_string<CharT> &&rhs)
        {
            return lhs.assign(rhs);
        }

        template <typename CharT>
        std::basic_string<CharT> &assign(std::basic_string<CharT> &lhs, const CharT *rhs, size_t count = -1)
        {
            if (count == -1)
                return lhs.assign(rhs);
            else
                return lhs.assign(rhs, count);
        }

        static std::string &assign(std::string &lhs, const wchar_t *rhs, size_t count = -1)
        {
            return lhs.assign(*PivW2A{rhs, count});
        }

        template <typename CharT>
        std::basic_string<CharT> &assign(std::basic_string<CharT> &lhs, const CVolMem &rhs, size_t count = -1)
        {
            return lhs.assign(reinterpret_cast<const CharT *>(rhs.GetPtr()), count == -1 ? static_cast<size_t>(rhs.GetSize() / sizeof(CharT)) : count);
        }

        template <typename CharT>
        std::basic_string<CharT> &assign(std::basic_string<CharT> &lhs, const ::piv::basic_string_view<CharT> &rhs, size_t count = -1)
        {
            return lhs.assign(rhs.data(), count == -1 ? rhs.size() : count);
        }

        static std::wstring &assign(std::wstring &lhs, const CWString &rhs, size_t count = -1)
        {
            return lhs.assign(rhs.GetText(), count == -1 ? rhs.GetLength() : count);
        }

        static std::string &assign(std::string &lhs, const CWString &rhs, size_t count = -1)
        {
            return lhs.assign(*PivW2U{rhs, count});
        }

        static std::string &assign(std::string &lhs, int64_t rhs)
        {
            char buf[66] = {'\0'};
            _i64toa(rhs, buf, 10);
            return lhs.assign(buf);
        }

        static std::string &assign(std::string &lhs, uint64_t rhs)
        {
            char buf[66] = {'\0'};
            _ui64toa(rhs, buf, 10);
            return lhs.assign(buf);
        }

        static std::string &assign(std::string &lhs, int32_t rhs)
        {
            char buf[34] = {'\0'};
            _ltoa(rhs, buf, 10);
            return lhs.assign(buf);
        }

        static std::string &assign(std::string &lhs, uint32_t rhs)
        {
            char buf[34] = {'\0'};
            _ultoa(rhs, buf, 10);
            return lhs.assign(buf);
        }

        static std::string &assign(std::string &lhs, int16_t rhs)
        {
            char buf[18] = {'\0'};
            _ltoa(rhs, buf, 10);
            return lhs.assign(buf);
        }

        static std::string &assign(std::string &lhs, uint16_t rhs)
        {
            char buf[18] = {'\0'};
            _ultoa(rhs, buf, 10);
            return lhs.assign(buf);
        }

        static std::string &assign(std::string &lhs, int8_t rhs)
        {
            char buf[10] = {'\0'};
            _ltoa(rhs, buf, 10);
            return lhs.assign(buf);
        }

        static std::string &assign(std::string &lhs, uint8_t rhs)
        {
            char buf[10] = {'\0'};
            _ultoa(rhs, buf, 10);
            return lhs.assign(buf);
        }

        static std::string &assign(std::string &lhs, double rhs)
        {
            char buf[130] = {'\0'};
            sprintf(buf, "%f", rhs);
            return lhs.assign(buf);
        }

        static std::string &assign(std::string &lhs, float rhs)
        {
            char buf[130] = {'\0'};
            sprintf(buf, "%lf", rhs);
            return lhs.assign(buf);
        }

        static std::wstring &assign(std::wstring &lhs, int64_t rhs)
        {
            wchar_t buf[66] = {'\0'};
            _i64tow(rhs, buf, 10);
            return lhs.assign(buf);
        }

        static std::wstring &assign(std::wstring &lhs, uint64_t rhs)
        {
            wchar_t buf[66] = {'\0'};
            _ui64tow(rhs, buf, 10);
            return lhs.assign(buf);
        }

        static std::wstring &assign(std::wstring &lhs, int32_t rhs)
        {
            wchar_t buf[34] = {'\0'};
            _ltow(rhs, buf, 10);
            return lhs.assign(buf);
        }

        static std::wstring &assign(std::wstring &lhs, uint32_t rhs)
        {
            wchar_t buf[34] = {'\0'};
            _ultow(rhs, buf, 10);
            return lhs.assign(buf);
        }

        static std::wstring &assign(std::wstring &lhs, int16_t rhs)
        {
            wchar_t buf[18] = {'\0'};
            _ltow(rhs, buf, 10);
            return lhs.assign(buf);
        }

        static std::wstring &assign(std::wstring &lhs, uint16_t rhs)
        {
            wchar_t buf[18] = {'\0'};
            _ultow(rhs, buf, 10);
            return lhs.assign(buf);
        }

        static std::wstring &assign(std::wstring &lhs, int8_t rhs)
        {
            wchar_t buf[10] = {'\0'};
            _ltow(rhs, buf, 10);
            return lhs.assign(buf);
        }

        static std::wstring &assign(std::wstring &lhs, uint8_t rhs)
        {
            wchar_t buf[10] = {'\0'};
            _ultow(rhs, buf, 10);
            return lhs.assign(buf);
        }

        static std::wstring &assign(std::wstring &lhs, double rhs)
        {
            wchar_t buf[130] = {'\0'};
            swprintf(buf, L"%f", rhs);
            return lhs.assign(buf);
        }

        static std::wstring &assign(std::wstring &lhs, float rhs)
        {
            wchar_t buf[130] = {'\0'};
            swprintf(buf, L"%f", rhs);
            return lhs.assign(buf);
        }

        /**
         * @brief 置十六进制文本
         * @tparam CharT 字符类型
         * @param lhs 本文本
         * @param hexstr 十六进制文本
         * @param count 十六进制文本长度
         * @return 返回本文本
         */
        template <typename CharT>
        std::basic_string<CharT> &assign_hex(std::basic_string<CharT> &lhs, const piv::string_view &hexstr, size_t count = -1)
        {
            return piv::encoding::hex_to_str((count == -1) ? hexstr : hexstr.substr(0, count), lhs);
        }

        template <typename CharT>
        std::basic_string<CharT> &assign_hex(std::basic_string<CharT> &lhs, const piv::wstring_view &hexstr, size_t count = -1)
        {
            return piv::encoding::hex_to_str((count == -1) ? hexstr : hexstr.substr(0, count), lhs);
        }

        template <typename CharT>
        std::basic_string<CharT> &assign_hex(std::basic_string<CharT> &lhs, const std::string &hexstr, size_t count = -1)
        {
            return piv::encoding::hex_to_str((count == -1) ? hexstr : piv::string_view{hexstr.c_str(), count}, lhs);
        }

        template <typename CharT>
        std::basic_string<CharT> &assign_hex(std::basic_string<CharT> &lhs, const std::wstring &hexstr, size_t count = -1)
        {
            return piv::encoding::hex_to_str((count == -1) ? hexstr : piv::wstring_view{hexstr.c_str(), count}, lhs);
        }

        template <typename CharT>
        std::basic_string<CharT> &assign_hex(std::basic_string<CharT> &lhs, const char *hexstr, size_t count = -1)
        {
            return piv::encoding::hex_to_str((count == -1) ? piv::string_view{hexstr} : piv::string_view{hexstr, count},
                                             lhs);
        }

        template <typename CharT>
        std::basic_string<CharT> &assign_hex(std::basic_string<CharT> &lhs, const wchar_t *hexstr, size_t count = -1)
        {
            return piv::encoding::hex_to_str((count == -1) ? piv::wstring_view{hexstr} : piv::wstring_view{hexstr, count},
                                             lhs);
        }

        template <typename CharT>
        std::basic_string<CharT> &assign_hex(std::basic_string<CharT> &lhs, const CWString &hexstr, size_t count = -1)
        {
            return piv::encoding::hex_to_str((count == -1) ? piv::wstring_view{hexstr.GetText()} : piv::wstring_view{hexstr.GetText(), count},
                                             lhs);
        }

        template <typename CharT>
        std::basic_string<CharT> &assign_hex(std::basic_string<CharT> &lhs, const CVolMem &hexstr, size_t count = -1)
        {
            if (hexstr.GetSize() < 2)
                return lhs;
            if (hexstr.Get_S_BYTE(1) == 0)
                return piv::encoding::hex_to_str(piv::wstring_view{reinterpret_cast<const wchar_t *>(hexstr.GetPtr()),
                                                                   static_cast<size_t>(hexstr.GetSize() / sizeof(wchar_t))},
                                                 lhs);
            else
                return piv::encoding::hex_to_str(piv::string_view{reinterpret_cast<const char *>(hexstr.GetPtr()),
                                                                  static_cast<size_t>(hexstr.GetSize())},
                                                 lhs);
        }

        /**
         * @brief 置文件资源
         * @tparam CharT 字符类型
         * @param lhs 本文本
         * @param resId 所欲指向的数据资源
         * @param storeBuf 缓存文本数据
         * @return 是否成功
         */
        template <typename CharT>
        bool assign_resdata(std::basic_string<CharT> &lhs, int32_t resId)
        {
            piv::basic_string_view<CharT> sv;
            if (piv::edit::load_resdata(sv, resId))
            {
                lhs.assign(sv.data(), sv.size());
                return true;
            }
            lhs.clear();
            return false;
        }

        /**
         * @brief 加入字符
         * @tparam CharT 字符类型
         * @tparam ...Args
         * @param lhs 本文本
         * @param ch 欲添加的字符
         * @param ...args
         * @return 返回本文本
         */
        template <typename CharT, typename... Args>
        std::basic_string<CharT> &push_back(std::basic_string<CharT> &lhs, const CharT &ch, Args &&...args)
        {
            lhs.push_back(ch);
            return piv::str::push_back(lhs, std::forward<Args>(args)...);
        }

        template <typename CharT>
        std::basic_string<CharT> &push_back(std::basic_string<CharT> &lhs, const CharT &ch)
        {
            lhs.push_back(ch);
            return lhs;
        }

        /**
         * @brief 加入文本
         * @tparam CharT 字符类型
         * @param lhs 本文本
         * @param rhs 所欲加入的数据
         * @param count 所欲加入的数据长度
         * @return 返回本文本
         */
        template <typename CharT>
        std::basic_string<CharT> &append(std::basic_string<CharT> &lhs, const std::basic_string<CharT> &rhs)
        {
            return lhs.append(rhs);
        }

        template <typename CharT>
        std::basic_string<CharT> &append(std::basic_string<CharT> &lhs, std::basic_string<CharT> &&rhs)
        {
            return lhs.append(rhs);
        }

        template <typename CharT>
        std::basic_string<CharT> &append(std::basic_string<CharT> &lhs, const CharT *rhs, size_t count = -1)
        {
            if (count == -1)
                return lhs.append(rhs);
            else
                return lhs.append(rhs, count);
        }

        static std::string &append(std::string &lhs, const wchar_t *rhs, size_t count = -1)
        {
            return lhs.append(*PivW2A{rhs, count});
        }

        template <typename CharT>
        std::basic_string<CharT> &append(std::basic_string<CharT> &lhs, const CVolMem &rhs, size_t count = -1)
        {
            return lhs.append(reinterpret_cast<const CharT *>(rhs.GetPtr()), count == -1 ? static_cast<size_t>(rhs.GetSize() / sizeof(CharT)) : count);
        }

        template <typename CharT>
        std::basic_string<CharT> &append(std::basic_string<CharT> &lhs, const ::piv::basic_string_view<CharT> &rhs, size_t count = -1)
        {
            return lhs.append(rhs.data(), count == -1 ? rhs.size() : count);
        }

        static std::wstring &append(std::wstring &lhs, const CWString &rhs, size_t count = -1)
        {
            return lhs.append(rhs.GetText(), count == -1 ? rhs.GetLength() : count);
        }

        static std::string &append(std::string &lhs, const CWString &rhs, size_t count = -1)
        {
            return lhs.append(*PivW2U{rhs, count});
        }

        static std::string &append(std::string &lhs, int64_t rhs)
        {
            char buf[66] = {'\0'};
            _i64toa(rhs, buf, 10);
            return lhs.append(buf);
        }

        static std::string &append(std::string &lhs, uint64_t rhs)
        {
            char buf[66] = {'\0'};
            _ui64toa(rhs, buf, 10);
            return lhs.append(buf);
        }

        static std::string &append(std::string &lhs, int32_t rhs)
        {
            char buf[34] = {'\0'};
            _ltoa(rhs, buf, 10);
            return lhs.append(buf);
        }

        static std::string &append(std::string &lhs, uint32_t rhs)
        {
            char buf[34] = {'\0'};
            _ultoa(rhs, buf, 10);
            return lhs.append(buf);
        }

        static std::string &append(std::string &lhs, int16_t rhs)
        {
            char buf[18] = {'\0'};
            _ltoa(rhs, buf, 10);
            return lhs.append(buf);
        }

        static std::string &append(std::string &lhs, uint16_t rhs)
        {
            char buf[18] = {'\0'};
            _ultoa(rhs, buf, 10);
            return lhs.append(buf);
        }

        static std::string &append(std::string &lhs, int8_t rhs)
        {
            char buf[10] = {'\0'};
            _ltoa(rhs, buf, 10);
            return lhs.append(buf);
        }

        static std::string &append(std::string &lhs, uint8_t rhs)
        {
            char buf[10] = {'\0'};
            _ultoa(rhs, buf, 10);
            return lhs.append(buf);
        }

        static std::string &append(std::string &lhs, double rhs)
        {
            char buf[130] = {'\0'};
            sprintf(buf, "%f", rhs);
            return lhs.append(buf);
        }

        static std::string &append(std::string &lhs, float rhs)
        {
            char buf[130] = {'\0'};
            sprintf(buf, "%f", rhs);
            return lhs.append(buf);
        }

        static std::wstring &append(std::wstring &lhs, int64_t rhs)
        {
            wchar_t buf[66] = {'\0'};
            _i64tow(rhs, buf, 10);
            return lhs.append(buf);
        }

        static std::wstring &append(std::wstring &lhs, uint64_t rhs)
        {
            wchar_t buf[66] = {'\0'};
            _ui64tow(rhs, buf, 10);
            return lhs.append(buf);
        }

        static std::wstring &append(std::wstring &lhs, int32_t rhs)
        {
            wchar_t buf[34] = {'\0'};
            _ltow(rhs, buf, 10);
            return lhs.append(buf);
        }

        static std::wstring &append(std::wstring &lhs, uint32_t rhs)
        {
            wchar_t buf[34] = {'\0'};
            _ultow(rhs, buf, 10);
            return lhs.append(buf);
        }

        static std::wstring &append(std::wstring &lhs, int16_t rhs)
        {
            wchar_t buf[18] = {'\0'};
            _ltow(rhs, buf, 10);
            return lhs.append(buf);
        }

        static std::wstring &append(std::wstring &lhs, uint16_t rhs)
        {
            wchar_t buf[18] = {'\0'};
            _ultow(rhs, buf, 10);
            return lhs.append(buf);
        }

        static std::wstring &append(std::wstring &lhs, int8_t rhs)
        {
            wchar_t buf[10] = {'\0'};
            _ltow(rhs, buf, 10);
            return lhs.append(buf);
        }

        static std::wstring &append(std::wstring &lhs, uint8_t rhs)
        {
            wchar_t buf[10] = {'\0'};
            _ultow(rhs, buf, 10);
            return lhs.append(buf);
        }

        static std::wstring &append(std::wstring &lhs, double rhs)
        {
            wchar_t buf[130] = {'\0'};
            swprintf(buf, L"%f", rhs);
            return lhs.append(buf);
        }

        static std::wstring &append(std::wstring &lhs, float rhs)
        {
            wchar_t buf[130] = {'\0'};
            swprintf(buf, L"%f", rhs);
            return lhs.append(buf);
        }

        /**
         * @brief 加入文本(可变参数)
         * @tparam CharT 字符类型
         * @tparam T 所欲加入的数据类型
         * @tparam ...Args 可变参数
         * @param lhs 本文本
         * @param first 所欲加入的首数据
         * @param ...args 可扩展数据
         * @return 返回本文本
         */
        template <typename CharT, typename T>
        std::basic_string<CharT> &appends(std::basic_string<CharT> &lhs, T &&first)
        {
            return ::piv::str::append(lhs, std::forward<T>(first));
        }

        template <typename CharT, typename T, typename... Args>
        std::basic_string<CharT> &appends(std::basic_string<CharT> &lhs, T &&first, Args &&...args)
        {
            ::piv::str::append(lhs, std::forward<T>(first));
            return ::piv::str::appends(lhs, std::forward<Args>(args)...);
        }

        /**
         * @brief 加入文本行
         * @tparam CharT 字符类型
         * @tparam T 所欲添加的文本类型
         * @param lhs 本文本
         * @param rhs 所欲加入到尾部的文本数据
         * @return 返回本文本
         */
        template <typename CharT, typename T>
        std::basic_string<CharT> &append_line(std::basic_string<CharT> &lhs, T &&rhs)
        {
            piv::str::append(lhs, std::forward<T>(rhs));
            lhs.push_back('\r');
            lhs.push_back('\n');
            return lhs;
        }

        template <typename CharT, typename T, typename... Args>
        std::basic_string<CharT> &append_line(std::basic_string<CharT> &lhs, T &&first, Args &&...args)
        {
            piv::str::append_line(lhs, std::forward<T>(first));
            return piv::str::append_line(lhs, std::forward<Args>(args)...);
        }

        /**
         * @brief 加入无符号值
         * @tparam CharT 字符类型
         * @tparam T 所欲添加的文本类型
         * @param lhs 本文本
         * @param rhs 无符号值
         * @return 返回本文本
         */
        template <typename CharT, typename T>
        std::basic_string<CharT> &append_unsigned(std::basic_string<CharT> &lhs, T &&rhs)
        {
            if (std::is_integral<T>::value)
                return piv::str::append(lhs, static_cast<std::make_unsigned<T>::type>(rhs));
            return lhs;
        }

        /**
         * @brief 加入重复文本
         * @tparam CharT 字符类型
         * @tparam T 所欲添加的文本类型
         * @param lhs 本文本
         * @param rhs 所欲添加的文本
         * @param times 所欲添加的次数
         * @return 返回本文本
         */
        template <typename CharT, typename T>
        std::basic_string<CharT> &append_repeat(std::basic_string<CharT> &lhs, T &&rhs, size_t times)
        {
            for (size_t i = 0; i < times; i++)
            {
                piv::str::append(lhs, std::forward<T>(rhs));
            }
            return lhs;
        }

        /**
         * @brief 加入格式文本
         * @tparam CharT 字符类型
         * @param lhs 本文本
         * @param format 格式文本
         * @param ...args 替换数据
         * @return 返回本文本
         */
        template <typename EncodeT, typename CharT, typename... Args>
        std::basic_string<CharT> &append_format(std::basic_string<CharT> &lhs, const std::basic_string<CharT> &format, Args &&...args)
        {
            return lhs.append(piv::edit::format<CharT>(format.c_str(), std::forward<Args>(args)...));
        }

        template <typename EncodeT, typename CharT, typename... Args>
        std::basic_string<CharT> &append_format(std::basic_string<CharT> &lhs, const piv::basic_string_view<CharT> &format, Args &&...args)
        {
            return lhs.append(piv::edit::format<CharT>(format.data(), std::forward<Args>(args)...));
        }

        template <typename EncodeT, typename CharT, typename... Args>
        std::basic_string<CharT> &append_format(std::basic_string<CharT> &lhs, const CVolMem &format, Args &&...args)
        {
            return lhs.append(piv::edit::format<CharT>(reinterpret_cast<const CharT *>(format.GetPtr()), std::forward<Args>(args)...));
        }

        template <typename EncodeT, typename CharT, typename... Args>
        std::basic_string<CharT> &append_format(std::basic_string<CharT> &lhs, const CharT *format, Args &&...args)
        {
            return lhs.append(piv::edit::format<CharT>(format, std::forward<Args>(args)...));
        }

        template <typename EncodeT, typename... Args>
        std::string &append_format(std::string &lhs, const wchar_t *format, Args &&...args)
        {
            PIV_IF(sizeof(EncodeT) == 3)
            {
                return lhs.append(piv::edit::format<char>(PivW2U{format}.c_str(), std::forward<Args>(args)...));
            }
            else
            {
                return lhs.append(piv::edit::format<char>(PivW2A{format}.c_str(), std::forward<Args>(args)...));
            }
        }

        /**
         * @brief 加入十六进制文本
         * @tparam CharT 字符类型
         * @param lhs 本文本
         * @param hexstr 十六进制文本
         * @param count 十六进制文本长度
         * @return 返回本文本
         */
        template <typename CharT>
        std::basic_string<CharT> &append_hex(std::basic_string<CharT> &lhs, const piv::string_view &hexstr, size_t count = -1)
        {
            return lhs.append(piv::encoding::hex_to_str((count == -1) ? hexstr : hexstr.substr(0, count), std::basic_string<CharT>{}));
        }

        template <typename CharT>
        std::basic_string<CharT> &append_hex(std::basic_string<CharT> &lhs, const piv::wstring_view &hexstr, size_t count = -1)
        {
            return lhs.append(piv::encoding::hex_to_str((count == -1) ? hexstr : hexstr.substr(0, count), std::basic_string<CharT>{}));
        }

        template <typename CharT>
        std::basic_string<CharT> &append_hex(std::basic_string<CharT> &lhs, const std::string &hexstr, size_t count = -1)
        {
            return lhs.append(piv::encoding::hex_to_str((count == -1) ? hexstr : piv::string_view{hexstr.c_str(), count}, std::basic_string<CharT>{}));
        }

        template <typename CharT>
        std::basic_string<CharT> &append_hex(std::basic_string<CharT> &lhs, const std::wstring &hexstr, size_t count = -1)
        {
            return lhs.append(piv::encoding::hex_to_str((count == -1) ? hexstr : piv::wstring_view{hexstr.c_str(), count}, std::basic_string<CharT>{}));
        }

        template <typename CharT>
        std::basic_string<CharT> &append_hex(std::basic_string<CharT> &lhs, const char *hexstr, size_t count = -1)
        {
            return lhs.append(piv::encoding::hex_to_str((count == -1) ? piv::string_view{hexstr} : piv::string_view{hexstr, count},
                                                        std::basic_string<CharT>{}));
        }

        template <typename CharT>
        std::basic_string<CharT> &append_hex(std::basic_string<CharT> &lhs, const wchar_t *hexstr, size_t count = -1)
        {
            return lhs.append(piv::encoding::hex_to_str((count == -1) ? piv::wstring_view{hexstr} : piv::wstring_view{hexstr, count},
                                                        std::basic_string<CharT>{}));
        }

        template <typename CharT>
        std::basic_string<CharT> &append_hex(std::basic_string<CharT> &lhs, const CWString &hexstr, size_t count = -1)
        {
            return lhs.append(piv::encoding::hex_to_str((count == -1) ? piv::wstring_view{hexstr.GetText()} : piv::wstring_view{hexstr.GetText(), count},
                                                        std::basic_string<CharT>{}));
        }

        template <typename CharT>
        std::basic_string<CharT> &append_hex(std::basic_string<CharT> &lhs, const CVolMem &hexstr, size_t count = -1)
        {
            if (hexstr.GetSize() < 2)
                return lhs;
            if (hexstr.Get_S_BYTE(1) == 0)
                return lhs.append(piv::encoding::hex_to_str(piv::wstring_view{reinterpret_cast<const wchar_t *>(hexstr.GetPtr()),
                                                                              static_cast<size_t>(hexstr.GetSize() / sizeof(wchar_t))},
                                                            std::basic_string<CharT>{}));
            else
                return lhs.append(piv::encoding::hex_to_str(piv::string_view{reinterpret_cast<const char *>(hexstr.GetPtr()),
                                                                             static_cast<size_t>(hexstr.GetSize())},
                                                            std::basic_string<CharT>{}));
        }

        /**
         * @brief 加入小写文本
         * @tparam CharT 字符类型
         * @param lhs 本文本
         * @param rhs 所欲加入并转换为小写的文本
         * @return 返回本文本
         */
        template <typename CharT>
        std::basic_string<CharT> &append_lower(std::basic_string<CharT> &lhs, const piv::basic_string_view<CharT> &rhs)
        {
            std::basic_string<CharT> lower;
            lower.resize(rhs.size());
            std::transform(rhs.begin(), rhs.end(), lower.begin(),
                           [](CharT c) -> CharT
                           { return (CharT)piv::edit::tolower(static_cast<uint16_t>(c)); });
            return lhs.append(lower);
        }

        template <typename CharT>
        std::basic_string<CharT> &append_lower(std::basic_string<CharT> &lhs, const std::basic_string<CharT> &rhs)
        {
            std::basic_string<CharT> lower;
            lower.resize(rhs.size());
            std::transform(rhs.begin(), rhs.end(), lower.begin(),
                           [](CharT c) -> CharT
                           { return (CharT)piv::edit::tolower(static_cast<uint16_t>(c)); });
            return lhs.append(lower);
        }

        template <typename CharT>
        std::basic_string<CharT> &append_lower(std::basic_string<CharT> &lhs, const CharT *rhs, size_t count = -1)
        {
            return piv::str::append_lower(lhs, (count == -1) ? piv::basic_string_view<CharT>{rhs} : piv::basic_string_view<CharT>{rhs, count});
        }

        /**
         * @brief 加入大写文本
         * @tparam CharT 字符类型
         * @param lhs 本文本
         * @param rhs 所欲加入并转换为大写的文本
         * @return 返回本文本
         */
        template <typename CharT>
        std::basic_string<CharT> &append_upper(std::basic_string<CharT> &lhs, const piv::basic_string_view<CharT> &rhs)
        {
            std::basic_string<CharT> lower;
            lower.resize(rhs.size());
            std::transform(rhs.begin(), rhs.end(), lower.begin(),
                           [](CharT c) -> CharT
                           { return (CharT)piv::edit::toupper(static_cast<uint16_t>(c)); });
            return lhs.append(lower);
        }

        template <typename CharT>
        std::basic_string<CharT> &append_upper(std::basic_string<CharT> &lhs, const std::basic_string<CharT> &rhs)
        {
            std::basic_string<CharT> lower;
            lower.resize(rhs.size());
            std::transform(rhs.begin(), rhs.end(), lower.begin(),
                           [](CharT c) -> CharT
                           { return (CharT)piv::edit::toupper(static_cast<uint16_t>(c)); });
            return lhs.append(lower);
        }

        template <typename CharT>
        std::basic_string<CharT> &append_upper(std::basic_string<CharT> &lhs, const CharT *rhs, size_t count = -1)
        {
            return piv::str::append_upper(lhs, (count == -1) ? piv::basic_string_view<CharT>{rhs} : piv::basic_string_view<CharT>{rhs, count});
        }

        /**
         * @brief 读入文本文件
         * @param lhs 本文本
         * @param FileName 所欲读取文件名
         * @param ReadDataSize 所欲读取数据尺寸
         * @param ReadEncode 文件文本编码
         * @param code_page 代码页
         * @return 是否成功
         */
        template <typename EncodeT, typename CharT>
        bool read_from_file(std::basic_string<CharT> &lhs, const wchar_t *FileName, const int32_t &ReadDataSize = -1, const VOL_STRING_ENCODE_TYPE &ReadEncode = VSET_UTF_16, uint32_t code_page = CP_ACP)
        {
            ASSERT(ReadDataSize >= -1);
            ASSERT_R_STR(FileName);
            VOL_STRING_ENCODE_TYPE StrEncode = ReadEncode;
            FILE *in = _wfopen(FileName, L"rb");
            if (in == NULL)
                return false;
            bool Succeeded = false;
            fseek(in, 0, SEEK_END);
            size_t fileSize = static_cast<size_t>(ftell(in));
            fseek(in, 0, SEEK_SET);
            if (ReadDataSize > 0 && static_cast<size_t>(ReadDataSize) < fileSize)
                fileSize = static_cast<size_t>(ReadDataSize);
            if (fileSize > 2)
            {
                uint8_t bom[3];
                fread(bom, 1, 3, in);
                if (bom[0] == 0xFF && bom[1] == 0xFE)
                {
                    StrEncode = VSET_UTF_16;
                    fseek(in, 2, SEEK_SET);
                    fileSize -= 2;
                }
                else if (bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF)
                {
                    StrEncode = VSET_UTF_8;
                    fseek(in, 3, SEEK_SET);
                    fileSize -= 3;
                }
                else
                {
                    fseek(in, 0, SEEK_SET);
                }
            }
            if (StrEncode == VSET_UNKNOWN)
                StrEncode = VSET_MBCS;
            if (StrEncode == VSET_UTF_16)
            {
                PIV_IF(sizeof(EncodeT) == 2)
                {
                    lhs.resize(fileSize / sizeof(CharT));
                    Succeeded = (fread(const_cast<CharT *>(lhs.data()), sizeof(CharT), fileSize / sizeof(CharT), in) == fileSize / sizeof(CharT));
                }
                else
                {
                    std::unique_ptr<char> data{new char[fileSize]};
                    Succeeded = (fread(const_cast<char *>(data.get()), 1, fileSize, in) == fileSize);
                    PIV_IF(sizeof(EncodeT) == 3)
                    {
                        PivW2U convert{reinterpret_cast<const wchar_t *>(data.get()), fileSize / 2};
                        lhs.assign(reinterpret_cast<const CharT *>(convert.c_str()), convert.size());
                    }
                    else
                    {
                        PivW2A convert{reinterpret_cast<const wchar_t *>(data.get()), fileSize / 2, code_page};
                        lhs.assign(reinterpret_cast<const CharT *>(convert.c_str()), convert.size());
                    }
                }
            }
            else if (StrEncode == VSET_UTF_8)
            {
                PIV_IF(sizeof(EncodeT) == 3)
                {
                    lhs.resize(fileSize);
                    Succeeded = (fread(const_cast<CharT *>(lhs.data()), 1, fileSize, in) == fileSize);
                }
                else
                {
                    std::unique_ptr<char> data{new char[fileSize]};
                    Succeeded = (fread(const_cast<char *>(data.get()), 1, fileSize, in) == fileSize);
                    PIV_IF(sizeof(EncodeT) == 2)
                    {
                        PivU2W convert{reinterpret_cast<const char *>(data.get()), fileSize};
                        lhs.assign(reinterpret_cast<const CharT *>(convert.c_str()), convert.size());
                    }
                    else
                    {
                        PivU2A convert{reinterpret_cast<const char *>(data.get()), fileSize, code_page};
                        lhs.assign(reinterpret_cast<const CharT *>(convert.c_str()), convert.size());
                    }
                }
            }
            else if (StrEncode == VSET_MBCS)
            {
                PIV_IF(sizeof(EncodeT) == 1)
                {
                    lhs.resize(fileSize / sizeof(CharT));
                    Succeeded = (fread(const_cast<CharT *>(lhs.data()), sizeof(CharT), fileSize / sizeof(CharT), in) == fileSize / sizeof(CharT));
                }
                else
                {
                    std::unique_ptr<char> data{new char[fileSize]};
                    Succeeded = (fread(const_cast<char *>(data.get()), 1, fileSize, in) == fileSize);
                    PIV_IF(sizeof(EncodeT) == 2)
                    {
                        PivA2W convert{reinterpret_cast<const char *>(data.get()), fileSize, code_page};
                        lhs.assign(reinterpret_cast<const CharT *>(convert.c_str()), convert.size());
                    }
                    else
                    {
                        PivA2U convert{reinterpret_cast<const char *>(data.get()), fileSize, code_page};
                        lhs.assign(reinterpret_cast<const CharT *>(convert.c_str()), convert.size());
                    }
                }
            }
            fclose(in);
            if (!Succeeded)
                lhs.clear();
            return Succeeded;
        }

        /**
         * @brief 写出文本文件
         * @param FileName 所欲写到文件名
         * @param WriteDataSize 所欲写出文本长度
         * @param WriteEncode 所欲写出文本编码
         * @return
         */
        template <typename EncodeT, typename CharT>
        bool write_into_file(std::basic_string<CharT> &lhs, const wchar_t *FileName, const int32_t &WriteDataSize = -1, const VOL_STRING_ENCODE_TYPE &WriteEncode = VSET_UTF_16, bool with_bom = true, uint32_t code_page = CP_ACP)
        {
            ASSERT(WriteDataSize >= -1);
            ASSERT_R_STR(FileName);
            ASSERT(WriteEncode != VSET_UNKNOWN);
            FILE *out = _wfopen(FileName, L"wb");
            if (out == NULL)
                return false;
            bool Succeeded = false;
            size_t count = (WriteDataSize == -1) ? lhs.size() : ((static_cast<size_t>(WriteDataSize) > lhs.size()) ? lhs.size() : static_cast<size_t>(WriteDataSize));
            if (WriteEncode == VSET_UTF_16)
            {
                if (with_bom)
                {
                    const uint8_t bom[] = {0xFF, 0xFE};
                    fwrite(bom, 1, 2, out);
                }
                PIV_IF(sizeof(EncodeT) == 2)
                {
                    Succeeded = (fwrite(lhs.data(), sizeof(CharT), count, out) == count);
                }
                PIV_ELSE_IF(sizeof(EncodeT) == 3)
                {
                    PivU2W convert{reinterpret_cast<const char *>(lhs.c_str()), count};
                    Succeeded = (fwrite(convert.c_str(), 2, convert.size(), out) == convert.size());
                }
                else
                {
                    PivA2W convert{reinterpret_cast<const char *>(lhs.c_str()), count, code_page};
                    Succeeded = (fwrite(convert.c_str(), 2, convert.size(), out) == convert.size());
                }
            }
            else if (WriteEncode == VSET_UTF_8)
            {
                if (with_bom)
                {
                    const uint8_t bom[] = {0xEF, 0xBB, 0xBF};
                    fwrite(bom, 1, 3, out);
                }
                PIV_IF(sizeof(EncodeT) == 2)
                {
                    PivW2U convert{reinterpret_cast<const wchar_t *>(lhs.c_str()), count};
                    Succeeded = (fwrite(convert.c_str(), 1, convert.size(), out) == convert.size());
                }
                PIV_ELSE_IF(sizeof(EncodeT) == 3)
                {
                    Succeeded = (fwrite(lhs.data(), sizeof(CharT), count, out) == count);
                }
                else
                {
                    PivA2U convert{reinterpret_cast<const char *>(lhs.c_str()), count, code_page};
                    Succeeded = (fwrite(convert.c_str(), 1, convert.size(), out) == convert.size());
                }
            }
            else if (WriteEncode == VSET_MBCS)
            {
                PIV_IF(sizeof(EncodeT) == 2)
                {
                    PivW2A convert{reinterpret_cast<const wchar_t *>(lhs.c_str()), count, code_page};
                    Succeeded = (fwrite(convert.c_str(), 1, convert.size(), out) == convert.size());
                }
                PIV_ELSE_IF(sizeof(EncodeT) == 3)
                {
                    PivU2A convert{reinterpret_cast<const char *>(lhs.c_str()), count, code_page};
                    Succeeded = (fwrite(convert.c_str(), 1, convert.size(), out) == convert.size());
                }
                else
                {
                    Succeeded = (fwrite(lhs.data(), sizeof(CharT), count, out) == count);
                }
            }
            fclose(out);
            return Succeeded;
        }

        /**
         * @brief 插入文本
         * @tparam CharT 字符类型
         * @param lhs 本文本
         * @param index 所欲插入的索引位置
         * @param rhs 所欲插入的文本
         * @param count 所欲插入的字符数
         * @return 返回本文本
         */
        template <typename CharT>
        std::basic_string<CharT> &insert(std::basic_string<CharT> &lhs, size_t index, const CharT *rhs, size_t count = (size_t)-1)
        {
            ASSERT(index <= lhs.size());
            return (count == (size_t)-1) ? lhs.insert(index, rhs) : lhs.insert(index, rhs, count);
        }

        template <typename CharT>
        std::basic_string<CharT> &insert(std::basic_string<CharT> &lhs, size_t index, const std::basic_string<CharT> &rhs, size_t count = (size_t)-1)
        {
            ASSERT(index <= lhs.size());
            return (count == (size_t)-1) ? lhs.insert(index, rhs) : lhs.insert(index, rhs, 0, count);
        }

        template <typename CharT>
        std::basic_string<CharT> &insert(std::basic_string<CharT> &lhs, size_t index, const piv::basic_string_view<CharT> &rhs, size_t count = (size_t)-1)
        {
            ASSERT(index <= lhs.size());
            return (count == (size_t)-1) ? lhs.insert(index, rhs.data(), rhs.size()) : lhs.insert(index, rhs.data(), 0, count);
        }

        /**
         * @brief 复制文本
         * @tparam CharT 字符类型
         * @param lhs 本文本
         * @param dest 欲复制到的目标
         * @param pos 欲复制的起始索引位置
         * @param count 欲复制的字符数目
         * @return 复制的字符数
         */
        template <typename CharT>
        size_t copy(std::basic_string<CharT> &lhs, CharT *dest, size_t pos, size_t count)
        {
            ASSERT(pos <= lhs.size()); // 判断索引位置是否有效
            return lhs.copy(dest, count, pos);
        }

        template <typename CharT>
        size_t copy(std::basic_string<CharT> &lhs, const ptrdiff_t &dest, size_t pos, size_t count)
        {
            return lhs.copy(const_cast<CharT *>(reinterpret_cast<const CharT *>(dest)), pos, count);
        }

        template <typename CharT>
        size_t copy(std::basic_string<CharT> &lhs, CVolMem &dest, size_t pos, size_t count)
        {
            dest.Alloc(count, TRUE);
            size_t n = lhs.copy(const_cast<CharT *>(reinterpret_cast<const CharT *>(dest.GetPtr())), pos, count);
            if (n == 0)
                dest.Empty();
            else
                dest.Realloc(n);
            return n;
        }

        template <typename CharT>
        size_t copy(std::basic_string<CharT> &lhs, std::basic_string<CharT> &dest, size_t pos, size_t count)
        {
            dest.resize(count, '\0');
            size_t n = copy(const_cast<CharT *>(dest.data()), pos, count);
            if (n == 0)
                dest.clear();
            else
                dest.resize(n);
            return n;
        }

        static size_t copy(std::wstring &lhs, CWString &dest, size_t pos, size_t count)
        {
            dest.SetLength(count);
            size_t n = lhs.copy(const_cast<wchar_t *>(dest.GetText()), pos, count);
            if (n == 0)
                dest.Empty();
            else
                dest.RemoveChars(n, count - n);
            return n;
        }

        /**
         * @brief 文本替换
         * @tparam CharT 字符类型
         * @param lhs 本文本
         * @param pos 起始替换索引位置
         * @param count 替换长度
         * @param rhs 用作替换的文本
         * @param count2 用作替换的长度
         * @return 返回本文本
         */
        template <typename CharT>
        std::basic_string<CharT> &replace(std::basic_string<CharT> &lhs, size_t pos, size_t count, const piv::basic_string_view<CharT> &rhs, size_t pos2 = 0, size_t count2 = (size_t)-1)
        {
            ASSERT(pos >= 0 && pos <= lhs.size() && count >= 0);
            if (count2 == (size_t)-1)
                return lhs.replace(pos, count, rhs.data(), rhs.size());
            else
                return lhs.replace(pos, count, rhs, pos2, count2);
        }

        template <typename CharT>
        std::basic_string<CharT> &replace(std::basic_string<CharT> &lhs, size_t pos, size_t count, const std::basic_string<CharT> &rhs, size_t pos2 = 0, size_t count2 = (size_t)-1)
        {
            ASSERT(pos >= 0 && pos <= lhs.size() && count >= 0);
            if (count2 == (size_t)-1)
                return lhs.replace(pos, count, rhs);
            else
                return lhs.replace(pos, count, rhs, pos2, count2);
        }

        /**
         * @brief 子文本替换
         * @tparam CharT 字符类型
         * @param lhs 本文本
         * @param str1 欲被替换的子文本
         * @param str2 用作替换的文本
         * @param pos 起始替换索引位置
         * @param times 替换次数
         * @param case_sensitive 是否区分大小写
         * @return 返回本文本
         */
        template <typename CharT>
        std::basic_string<CharT> &replace_text(std::basic_string<CharT> &lhs, const piv::basic_string_view<CharT> &str1, const piv::basic_string_view<CharT> &str2,
                                               size_t pos = 0, size_t times = (size_t)-1, bool case_sensitive = true)
        {
            ASSERT(pos >= 0 && pos <= lhs.size());
            size_t fpos = 0, i = 0;
            while (fpos < lhs.size())
            {
                if (case_sensitive)
                    fpos = lhs.find(str1.data(), pos, str1.size());
                else
                    fpos = piv::edit::ifind(lhs, str1, pos);
                if (fpos == std::basic_string<CharT>::npos)
                    break;
                lhs.replace(fpos, str1.size(), str2.data(), str2.size());
                pos = fpos + str2.size();
                i++;
                if (times > 0 && i >= times)
                    break;
            }
            return lhs;
        }

        /**
         * @brief 寻找字符集合
         * @param lhs 本文本
         * @param rhs 欲寻找的字符集合
         * @param pos 起始搜寻位置
         * @return 找到的位置
         */
        template <typename CharT>
        size_t find_first_of(std::basic_string<CharT> &lhs, const piv::basic_string_view<CharT> &rhs, size_t pos = 0)
        {
            return lhs.find_first_of(rhs.data(), pos, rhs.size());
        }

        template <typename CharT>
        size_t find_first_of(std::basic_string<CharT> &lhs, const std::basic_string<CharT> &rhs, size_t pos = 0)
        {
            return lhs.find_first_of(rhs, pos);
        }

        template <typename CharT>
        size_t find_first_of(std::basic_string<CharT> &lhs, const CharT *rhs, size_t pos = 0)
        {
            return lhs.find_first_of(rhs, pos);
        }

        /**
         * @brief 倒找字符集合
         * @param lhs 本文本
         * @param rhs 欲寻找的字符集合
         * @param pos 起始搜寻位置
         * @return 找到的位置
         */
        template <typename CharT>
        size_t find_last_of(std::basic_string<CharT> &lhs, const piv::basic_string_view<CharT> &rhs, size_t pos = 0)
        {
            return lhs.find_last_of(rhs.data(), pos, rhs.size());
        }

        template <typename CharT>
        size_t find_last_of(std::basic_string<CharT> &lhs, const std::basic_string<CharT> &rhs, size_t pos = 0)
        {
            return lhs.find_last_of(rhs, pos);
        }

        template <typename CharT>
        size_t find_last_of(std::basic_string<CharT> &lhs, const CharT *rhs, size_t pos = 0)
        {
            return lhs.find_last_of(rhs, pos);
        }

        /**
         * @brief 寻找不符合字符集合
         * @param lhs 本文本
         * @param rhs 欲寻找的字符集合
         * @param pos 起始搜寻位置
         * @return 找到的位置
         */
        template <typename CharT>
        size_t find_first_not_of(std::basic_string<CharT> &lhs, const piv::basic_string_view<CharT> &rhs, size_t pos = 0)
        {
            return lhs.find_first_not_of(rhs.data(), pos, rhs.size());
        }

        template <typename CharT>
        size_t find_first_not_of(std::basic_string<CharT> &lhs, const std::basic_string<CharT> &rhs, size_t pos = 0)
        {
            return lhs.find_first_not_of(rhs, pos);
        }

        template <typename CharT>
        size_t find_first_not_of(std::basic_string<CharT> &lhs, const CharT *rhs, size_t pos = 0)
        {
            return lhs.find_first_not_of(rhs, pos);
        }

        /**
         * @brief 倒找不符合字符集合
         * @param lhs 本文本
         * @param rhs 欲寻找的字符集合
         * @param pos 起始搜寻位置
         * @return 找到的位置
         */
        template <typename CharT>
        size_t find_last_not_of(std::basic_string<CharT> &lhs, const piv::basic_string_view<CharT> &rhs, size_t pos = 0)
        {
            return lhs.find_last_not_of(rhs.data(), pos, rhs.size());
        }

        template <typename CharT>
        size_t find_last_not_of(std::basic_string<CharT> &lhs, const std::basic_string<CharT> &rhs, size_t pos = 0)
        {
            return lhs.find_last_not_of(rhs, pos);
        }

        template <typename CharT>
        size_t find_last_not_of(std::basic_string<CharT> &lhs, const CharT *rhs, size_t pos = 0)
        {
            return lhs.find_last_not_of(rhs, pos);
        }

    } // namespace str

    namespace sv
    {
        /**
         * @brief 复制文本
         * @tparam CharT 字符类型
         * @param lhs 本文本
         * @param dest 欲复制到的目标
         * @param pos 欲复制的起始索引位置
         * @param count 欲复制的字符数目
         * @return 复制的字符数
         */
        template <typename CharT>
        size_t copy(piv::basic_string_view<CharT> &lhs, CharT *dest, size_t pos, size_t count)
        {
            ASSERT(pos <= lhs.size()); // 判断索引位置是否有效
            return lhs.copy(dest, count, pos);
        }

        template <typename CharT>
        size_t copy(piv::basic_string_view<CharT> &lhs, const ptrdiff_t &dest, size_t pos, size_t count)
        {
            return lhs.copy(const_cast<CharT *>(reinterpret_cast<const CharT *>(dest)), pos, count);
        }

        template <typename CharT>
        size_t copy(piv::basic_string_view<CharT> &lhs, CVolMem &dest, size_t pos, size_t count)
        {
            dest.Alloc(count, TRUE);
            size_t n = lhs.copy(const_cast<CharT *>(reinterpret_cast<const CharT *>(dest.GetPtr())), pos, count);
            if (n == 0)
                dest.Empty();
            else
                dest.Realloc(n);
            return n;
        }

        template <typename CharT>
        size_t copy(piv::basic_string_view<CharT> &lhs, std::basic_string<CharT> &dest, size_t pos, size_t count)
        {
            dest.resize(count, '\0');
            size_t n = copy(const_cast<CharT *>(dest.data()), pos, count);
            if (n == 0)
                dest.clear();
            else
                dest.resize(n);
            return n;
        }

        static size_t copy(piv::wstring_view &lhs, CWString &dest, size_t pos, size_t count)
        {
            dest.SetLength(count);
            size_t n = lhs.copy(const_cast<wchar_t *>(dest.GetText()), pos, count);
            if (n == 0)
                dest.Empty();
            else
                dest.RemoveChars(n, count - n);
            return n;
        }

    } // namespace str
} // namespace piv

#endif // _PIV_STD_STRING_HPP
