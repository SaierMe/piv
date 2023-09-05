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
#include <memory>

namespace piv
{
    namespace edit
    {
        /**
         * @brief 寻找文本(不区分大小写)
         * @tparam CharT 字符类型
         * @param suc 被寻找的文本
         * @param des 所欲寻找的文本
         * @param pos 起始搜寻索引位置
         * @return 找到的位置
         */
        template <typename CharT>
        size_t ifind(const basic_string_view<CharT> &suc, const basic_string_view<CharT> &des, const size_t &pos = 0)
        {
            if (pos >= suc.size() || des.empty())
                return std::basic_string<CharT>::npos;
            for (auto OuterIt = std::next(suc.begin(), pos); OuterIt != suc.end(); ++OuterIt)
            {
                auto InnerIt = OuterIt;
                auto SubstrIt = des.begin();
                for (; InnerIt != suc.end() && SubstrIt != des.end(); ++InnerIt, ++SubstrIt)
                {
                    if (std::tolower(static_cast<unsigned char>(*InnerIt)) != std::tolower(static_cast<unsigned char>(*SubstrIt)))
                        break;
                }
                if (SubstrIt == des.end())
                    return std::distance(suc.begin(), OuterIt);
            }
            return std::basic_string<CharT>::npos;
        }

        /**
         * @brief 倒找文本(不区分大小写)
         * @tparam CharT 字符类型
         * @param suc 被寻找的文本
         * @param des 所欲寻找的文本
         * @param pos 起始搜寻索引位置
         * @return 找到的位置
         */
        template <typename CharT>
        size_t irfind(const basic_string_view<CharT> &suc, const basic_string_view<CharT> &des, const size_t &pos = (size_t)-1)
        {
            if (des.empty())
                return std::basic_string<CharT>::npos;
            for (auto OuterIt = std::prev(suc.end(), suc.size() - ((pos == (size_t)-1) ? suc.size() : pos)); OuterIt != suc.begin(); --OuterIt)
            {
                auto InnerIt = OuterIt;
                auto SubstrIt = des.begin();
                for (; InnerIt != suc.end() && SubstrIt != des.end(); ++InnerIt, ++SubstrIt)
                {
                    if (std::tolower(static_cast<unsigned char>(*InnerIt)) != std::tolower(static_cast<unsigned char>(*SubstrIt)))
                        break;
                }
                if (SubstrIt == des.end())
                    return std::distance(suc.begin(), OuterIt);
            }
            return std::basic_string<CharT>::npos;
        }

        /**
         * @brief 删首空
         * @param 所欲操作的文本 str
         */
        template <typename CharT>
        void trim_left(std::basic_string<CharT> &str)
        {
            if (str.empty())
                return;
            size_t pos = 0;
            for (; pos < str.size(); ++pos)
            {
                // if (!std::isspace(static_cast<unsigned char>(str[pos])))
                if (static_cast<unsigned char>(str[pos]) > ' ')
                    break;
            }
            str.erase(0, pos);
        }
        template <typename CharT>
        void trim_left(basic_string_view<CharT> &str)
        {
            if (str.empty())
                return;
            size_t pos = 0;
            for (; pos < str.size(); ++pos)
            {
                // if (!std::isspace(static_cast<unsigned char>(str[pos])))
                if (static_cast<unsigned char>(str[pos]) > ' ')
                    break;
            }
            str.remove_prefix(pos);
        }

        /**
         * @brief 删尾空
         * @param 所欲操作的文本 str
         */
        template <typename CharT>
        void trim_right(std::basic_string<CharT> &str)
        {
            if (str.empty())
                return;
            size_t pos = str.size() - 1;
            for (; pos != size_t(-1); pos--)
            {
                // if (!std::isspace(static_cast<unsigned char>(str[pos])))
                if (static_cast<unsigned char>(str[pos]) > ' ')
                    break;
            }
            str.erase(pos + 1);
        }
        template <typename CharT>
        void trim_right(basic_string_view<CharT> &str)
        {
            if (str.empty())
                return;
            size_t pos = str.size() - 1;
            for (; pos != size_t(-1); pos--)
            {
                // if (!std::isspace(static_cast<unsigned char>(str[pos])))
                if (static_cast<unsigned char>(str[pos]) > ' ')
                    break;
            }
            str.remove_suffix(str.size() - pos - 1);
        }

        /**
         * @brief ASCII字符到小写
         * @param c
         * @return
         */
        template <typename CharT>
        inline CharT ascii_tolower(const CharT &c) noexcept
        {
            return CharT(((static_cast<unsigned>(c) - 65U) < 26) ? c + 'a' - 'A' : c);
        }

        /**
         * @brief 不区分大小写比较文本
         * @param lhs 所欲比较的文本1
         * @param rhs 所欲比较的文本2
         * @return 返回两个文本是否相同
         */
        template <typename CharT>
        bool iequals(const basic_string_view<CharT> &lhs, const basic_string_view<CharT> &rhs)
        {
            size_t n = lhs.size();
            if (rhs.size() != n)
                return false;
            auto p1 = lhs.data();
            auto p2 = rhs.data();
            CharT a, b;
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
                if (ascii_tolower<CharT>(a) != ascii_tolower<CharT>(b))
                    return false;
                a = *p1++;
                b = *p2++;
            } while (n--);
            return true;
        }
        template <typename CharT>
        bool iequals(const std::basic_string<CharT> &lhs, const std::basic_string<CharT> &rhs)
        {
            return iequals(basic_string_view<CharT>{lhs}, basic_string_view<CharT>{rhs});
        }
        template <typename CharT>
        bool iequals(const basic_string_view<CharT> &lhs, const std::basic_string<CharT> &rhs)
        {
            return iequals(lhs, basic_string_view<CharT>{rhs});
        }
        template <typename CharT>
        bool iequals(const std::basic_string<CharT> &lhs, const basic_string_view<CharT> &rhs)
        {
            return iequals(basic_string_view<CharT>{lhs}, rhs);
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
         * @brief 取文字长度
         */
        template <typename CharT, typename EncodeType>
        size_t get_word_length(const basic_string_view<CharT> &str)
        {
            CharT ch;
            size_t count = 0;
            PIV_IF(sizeof(EncodeType) == 2)
            {
                for (size_t i = 0; i < str.size(); i++, count++)
                {
                    ch = str[i];
                    if (ch >= (CharT)0xD800 && ch <= (CharT)0xDBFF)
                        i++;
                }
            }
            PIV_ELSE_IF(sizeof(EncodeType) == 3)
            {
                for (size_t i = 0; i < str.size(); i++, count++)
                {
                    ch = str[i];
                    // if (ch >> 7 == 0) // c <= 0x7F 1字节
                    if (ch >= (char)0xE0 && ch <= (char)0xEF) // 3字节
                        i += 2;
                    else if (ch >= (char)0xC0 && ch <= (char)0xDF) // 2字节
                        i++;
                    else if (ch >= (char)0xF0 && ch <= (char)0xF7) // 4字节
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
         * @brief 取逆序文本
         */
        template <typename CharT, typename EncodeType>
        std::basic_string<CharT> reverse_text(const basic_string_view<CharT> &str)
        {
            std::basic_string<CharT> reverse;
            size_t n = str.size(), i = 0;
            CharT ch;
            reverse.resize(n);
            PIV_IF(sizeof(EncodeType) == 2)
            {
                while (n--)
                {
                    ch = str[i++];
                    if (ch >= (CharT)0xD800 && ch <= (CharT)0xDBFF)
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
            PIV_ELSE_IF(sizeof(EncodeType) == 3)
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
        template <typename CharT, typename EncodeType>
        std::basic_string<CharT> reverse_text(const std::basic_string<CharT> &str)
        {
            return reverse_text<CharT, EncodeType>(basic_string_view<CharT>{str});
        }

        /**
         * @brief 到全角
         * @param option 转换选项 1 2 4
         * @return 返回转换后的文本
         */
        template <typename CharT, typename EncodeType>
        std::basic_string<CharT> to_fullwidth(const basic_string_view<CharT> &str, const int32_t &option)
        {
            std::basic_string<CharT> ret;
            if (str.empty())
                return ret;
            ret.reserve(str.size() * 2);
            ret = str;
            CharT ch;
            PIV_IF(sizeof(EncodeType) == 2)
            {
                for (auto it = ret.begin(); it != ret.end(); it++)
                {
                    ch = *it;
                    if (ch == ' ')
                    {
                        *it = static_cast<CharT>(0x3000);
                    }
                    else if ((1 & option && ((ch >= '!' && ch <= '/') || (ch >= ':' && ch <= '@') || (ch >= '[' && ch <= '`') || (ch >= '{' && ch <= '~'))) ||
                             (2 & option && ch >= '0' && ch <= '9') ||
                             (4 & option && ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))))
                    {
                        *it = ch + static_cast<CharT>(0xFEE0);
                    }
                }
            }
            PIV_ELSE_IF(sizeof(EncodeType) == 3)
            {
                CharT qj[4]{};
                for (size_t i = 0; i < ret.size(); i++)
                {
                    ch = ret[i];
                    if (ch == ' ')
                    {
                        ret[i] = static_cast<CharT>(0xE3);
                        ret.insert(++i, 1, static_cast<CharT>(0x80));
                        ret.insert(++i, 1, static_cast<CharT>(0x80));
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
                CharT qj[2]{};
                for (size_t i = 0; i < ret.size(); i++)
                {
                    ch = ret[i];
                    if (ch == ' ')
                    {
                        ret[i] = static_cast<CharT>(0xA1);
                        ret.insert(i++, 1, static_cast<CharT>(0xA1));
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
        template <typename CharT, typename EncodeType>
        std::basic_string<CharT> to_halfwidth(const basic_string_view<CharT> &str, const int32_t &option)
        {
            std::basic_string<CharT> ret;
            if (str.empty())
                return ret;
            ret.reserve(str.size());
            CharT ch;
            PIV_IF(sizeof(EncodeType) == 2)
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
            PIV_ELSE_IF(sizeof(EncodeType) == 3)
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
                            ret.push_back(static_cast<CharT>(fullCh - 0xEFBC60));
                        }
                        else if ((4 & option && fullCh >= 0xEFBD81 && fullCh <= 0xEFBD9A) ||
                                 (1 & option && (fullCh == 0xEFBD80 || (fullCh >= 0xEFBD9B && fullCh <= 0xEFBD9E))))
                        {
                            ret.push_back(static_cast<CharT>(fullCh - 0xEFBD20));
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
                            ret.push_back(static_cast<CharT>(fullCh - 0xA380));
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
         * @brief 返回文本自身,用于确定参数类型
         */
        template <typename CharT>
        inline const basic_string_view<CharT> &cstr(const basic_string_view<CharT> &s)
        {
            return s;
        }
        template <typename CharT>
        inline const std::basic_string<CharT> &cstr(const std::basic_string<CharT> &s)
        {
            return s;
        }
        template <typename CharT>
        inline const CharT *cstr(const CharT *s)
        {
            return s;
        }

        /**
         * @brief 文字长度转换到字符长度
         * @param str 所欲操作文本
         * @param pos 提供文字索引,返回字符索引
         * @param count 提供文字个数,返回字符个数
         */
        template <typename CharT, typename EncodeType>
        void length_word_to_char(const basic_string_view<CharT> &str, size_t &pos, size_t &count)
        {
            if (count == 0)
                return;
            size_t length = str.size();
            size_t spos = 0, cb = 0;
            CharT ch;
            for (size_t i = 0, n = 0, c = 0; i < length; i++, n++)
            {
                if (n == pos)
                    spos = i;
                ch = str[i];
                PIV_IF(sizeof(EncodeType) == 2)
                {
                    if (ch >= (CharT)0xD800 && ch <= (CharT)0xDBFF)
                        i++;
                }
                PIV_ELSE_IF(sizeof(EncodeType) == 3)
                {
                    if (ch >= (char)0xE0 && ch <= (char)0xEF) // 3字节
                        i += 2;
                    else if (ch >= (char)0xC0 && ch <= (char)0xDF) // 2字节
                        i++;
                    else if (ch >= (char)0xF0 && ch <= (char)0xF7) // 4字节
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
        template <typename CharT, typename EncodeType>
        basic_string_view<CharT> substr(const basic_string_view<CharT> &str, const size_t &pos, const size_t &count)
        {
            size_t spos = pos, cb = count;
            length_word_to_char<CharT, EncodeType>(str, spos, cb);
            if (spos < str.size() && cb > 0)
                return str.substr(spos, cb);
            return basic_string_view<CharT>{};
        }
        template <typename CharT, typename EncodeType>
        basic_string_view<CharT> substr(const std::basic_string<CharT> &str, const size_t &pos, const size_t &count)
        {
            return substr<CharT, EncodeType>(basic_string_view<CharT>{str}, pos, count);
        }

        /**
         * @brief 删除文字
         * @param str 所欲操作的文本
         * @param pos 删除的起始位置
         * @param count 删除的文字数量
         */
        template <typename CharT, typename EncodeType>
        void remove_words(std::basic_string<CharT> &str, const size_t &pos, const size_t &count)
        {
            size_t spos = pos, cb = count;
            length_word_to_char<CharT, EncodeType>(basic_string_view<CharT>{str}, spos, cb);
            if (spos < str.size() && cb > 0)
                str.erase(spos, cb);
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
                    return (tolower(c1) < tolower(c2));
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
        template <typename CharT>
        size_t ci_hash(const basic_string_view<CharT> &str)
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
        template <typename CharT>
        size_t ci_hash(const std::basic_string<CharT> &str)
        {
            return ci_hash<CharT>(basic_string_view<CharT>{str});
        }

    } // namespace detail

} // namespace piv

/*********************************************
 * @brief  文本视图模板类
 * @tparam CharT 字符类型
 * @tparam EncodeType 文本编码,按类型的尺寸区分(1=ANSI; 2=UTF-16LE; 3=UTF8;)
 */
template <typename CharT, typename EncodeType>
class PivStringView : public CVolObject
{
private:
    piv::basic_string_view<CharT> sv;
    std::shared_ptr<std::basic_string<CharT>> pStr{nullptr};

public:
    using EncodeType_t = EncodeType;

    /**
     * @brief 默认构造和析构函数
     */
    PivStringView() {}
    ~PivStringView() {}

    /**
     * @brief 复制和移动构造函数
     * @param s 所欲复制的string_view
     * @param count 文本的字符长度
     * @param ptr 所欲复制的智能指针
     * @param storeBuf 是否缓存文本数据
     */
    PivStringView(const PivStringView &s)
    {
        sv = s.sv;
        pStr = s.pStr;
    }
    PivStringView(PivStringView &&s) noexcept
    {
        sv = std::move(s.sv);
        pStr.swap(s.pStr);
    }
    PivStringView(const piv::basic_string_view<CharT> &s)
    {
        sv = s;
    }
    PivStringView(piv::basic_string_view<CharT> &&s) noexcept
    {
        sv = std::move(s);
    }
    PivStringView(const piv::basic_string_view<CharT> &s, const std::shared_ptr<std::basic_string<CharT>> &ptr)
    {
        sv = s;
        pStr = ptr;
    }
    PivStringView(const CharT *s, const size_t &count = (size_t)-1, const bool &storeBuf = false)
    {
        this->SetText(s, count, storeBuf);
    }
    PivStringView(const CVolString &s, const bool &storeBuf = false)
    {
        this->SetText(s, storeBuf);
    }
    PivStringView(const CVolMem &s, const bool &storeBuf = false)
    {
        this->SetText(s, storeBuf);
    }
    PivStringView(const std::basic_string<CharT> &s, const bool &storeBuf = false)
    {
        this->SetText(s, storeBuf);
    }

    inline operator piv::basic_string_view<CharT> &() noexcept
    {
        return sv;
    }
    inline operator piv::basic_string_view<CharT> const &() const noexcept
    {
        return sv;
    }

    /**
     * @brief 赋值操作符
     * @param other 所欲复制的文本视图类
     * @return 返回自身
     */
    inline PivStringView &operator=(const PivStringView &other)
    {
        sv = other.sv;
        pStr = other.pStr;
        return *this;
    }
    inline PivStringView &operator=(PivStringView &&other) noexcept
    {
        sv = std::move(other.sv);
        pStr.swap(other.pStr);
        return *this;
    }
    inline PivStringView &operator=(const piv::basic_string_view<CharT> &other)
    {
        sv = other;
        return *this;
    }
    inline PivStringView &operator=(piv::basic_string_view<CharT> &&other) noexcept
    {
        sv = std::move(other);
        return *this;
    }

    /**
     * @brief 比较操作符
     * @param other 所欲比较的文本视图类
     * @return 返回两个文本视图的内容是否相同
     */
    inline bool operator==(const PivStringView &other) const
    {
        return sv == other.sv;
    }
    inline bool operator<(const PivStringView &other) const
    {
        return sv < other.sv;
    }
    inline bool operator<=(const PivStringView &other) const
    {
        return sv <= other.sv;
    }
    inline bool operator>(const PivStringView &other) const
    {
        return sv > other.sv;
    }
    inline bool operator>=(const PivStringView &other) const
    {
        return sv >= other.sv;
    }
    inline bool operator==(const piv::basic_string_view<CharT> &other) const
    {
        return sv == other;
    }
    inline bool operator<(const piv::basic_string_view<CharT> &other) const
    {
        return sv < other;
    }
    inline bool operator<=(const piv::basic_string_view<CharT> &other) const
    {
        return sv <= other;
    }
    inline bool operator>(const piv::basic_string_view<CharT> &other) const
    {
        return sv > other;
    }
    inline bool operator>=(const piv::basic_string_view<CharT> &other) const
    {
        return sv >= other;
    }

    /**
     * @brief 获取文本视图类中的string_view
     * @return 返回string_view
     */
    inline piv::basic_string_view<CharT> &data()
    {
        return sv;
    }
    inline const piv::basic_string_view<CharT> &data() const
    {
        return sv;
    }

    /**
     * @brief 获取文本视图类中的string_view指针
     */
    inline piv::basic_string_view<CharT> *pdata()
    {
        return &sv;
    }
    inline const piv::basic_string_view<CharT> *pdata() const
    {
        return &sv;
    }

    /**
     * @brief 取展示内容(调试输出)
     * @param strDump 展示内容
     * @param nMaxDumpSize 最大展示数据尺寸
     */
    virtual void GetDumpString(CVolString &strDump, INT nMaxDumpSize) override
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            strDump.SetText(reinterpret_cast<const wchar_t *>(sv.data()), sv.length());
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            PivU2Ws{reinterpret_cast<const char *>(sv.data()), sv.length()}.GetStr(strDump);
        }
        else
        {
            PivA2Ws{reinterpret_cast<const char *>(sv.data()), sv.length()}.GetStr(strDump);
        }
    }

    /**
     * @brief 置对象数据
     * @param stream 基本输入流
     */
    virtual void LoadFromStream(CVolBaseInputStream &stream)
    {
        sv.swap(piv::basic_string_view<CharT>{});
        if (stream.eof())
            return;
        pStr.reset(new std::basic_string<CharT>{});
        uint32_t Size;
        stream.read(&Size, 4);
        pStr->resize(Size / sizeof(CharT));
        if (stream.ReadExact(const_cast<CharT *>(pStr->data()), Size))
        {
            if (pStr->back() == '\0')
                pStr->pop_back();
            sv = *pStr;
        }
        else
        {
            pStr.reset();
        }
    }

    /**
     * @brief 取对象数据
     * @param stream 基本输出流
     */
    virtual void SaveIntoStream(CVolBaseOutputStream &stream)
    {
        uint32_t Size = static_cast<uint32_t>(sv.size() * sizeof(CharT));
        stream.write(&Size, 4);
        stream.write(sv.data(), Size);
    }

    /**
     * @brief 迭代器
     * @return
     */
    auto begin()
    {
        return sv.begin();
    }
    auto end()
    {
        return sv.end();
    }

    /**
     * @brief 清空视图
     */
    inline PivStringView &Clear()
    {
        sv.swap(piv::basic_string_view<CharT>{});
        pStr.reset();
        return *this;
    }

    /**
     * @brief 置文本
     * @param s 所欲指向的文本指针
     * @param count 文本的字符长度
     * @param storeBuf 是否缓存文本数据
     */
    inline PivStringView &SetText(const CharT *s, const size_t &count = (size_t)-1, const bool &storeBuf = false)
    {
        if (storeBuf)
        {
            pStr.reset((count == (size_t)-1) ? new std::basic_string<CharT>{s} : new std::basic_string<CharT>{s, count});
            sv = *pStr;
        }
        else
        {
            if (count == (size_t)-1)
                sv = piv::basic_string_view<CharT>{s};
            else
                sv = piv::basic_string_view<CharT>{s, count};
        }
        if (!sv.empty() && sv.back() == '\0')
            sv.remove_suffix(1);
        return *this;
    }
    inline PivStringView &SetText(const ptrdiff_t s, const size_t &count, const bool &storeBuf = false)
    {
        return SetText(reinterpret_cast<const CharT *>(s), count, storeBuf);
    }
    inline PivStringView &SetText(const CVolString &s, const bool &storeBuf = false)
    {
        ASSERT(sizeof(CharT) == 2); // 只有UTF-16LE编码的文本视图可以置入火山文本型
        if (storeBuf)
        {
            pStr.reset(new std::basic_string<CharT>{reinterpret_cast<const CharT *>(s.GetText())});
            sv = *pStr;
        }
        else
        {
            sv = piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetText())};
        }
        return *this;
    }
    inline PivStringView &SetText(const CVolMem &s, const bool &storeBuf = false)
    {
        if (storeBuf)
        {
            pStr.reset(new std::basic_string<CharT>{reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT)});
            sv = *pStr;
        }
        else
        {
            sv = piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT)};
        }
        return *this;
    }
    inline PivStringView &SetText(const std::basic_string<CharT> &s, const bool &storeBuf = false)
    {
        if (storeBuf)
        {
            pStr.reset(new std::basic_string<CharT>{s});
            sv = *pStr;
        }
        else
        {
            sv = piv::basic_string_view<CharT>{s.c_str()};
        }
        return *this;
    }

    /**
     * @brief 置文件资源
     * @param resId 所欲指向的数据资源
     * @param storeBuf 缓存文本数据
     * @return
     */
    bool LoadResData(const size_t resId, const bool &storeBuf = false)
    {
        Clear();
        if (resId == 0)
        {
            return false;
        }
        HMODULE hModule = g_objVolApp.GetInstanceHandle();
        HRSRC hSrc = ::FindResourceW(hModule, MAKEINTRESOURCE(static_cast<WORD>(resId)), RT_RCDATA);
        if (hSrc == NULL)
        {
            return false;
        }
        HGLOBAL resdata = ::LoadResource(hModule, hSrc);
        if (resdata == NULL)
        {
            return false;
        }
        byte *data = reinterpret_cast<byte *>(::LockResource(resdata));
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
        if (storeBuf)
        {
            pStr.reset(new std::basic_string<CharT>{reinterpret_cast<const CharT *>(data), count / sizeof(CharT)});
            sv = *pStr;
        }
        else
        {
            sv = piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(data), count / sizeof(CharT)};
        }
        return true;
    }

    /**
     * @brief 读入文本文件
     * @param FileName 所欲读取文件名
     * @param ReadDataSize 所欲读取数据尺寸
     * @param ReadEncode 所欲读取文本编码
     * @return
     */
    bool ReadFromFile(const wchar_t *FileName, const int32_t &ReadDataSize = -1, const VOL_STRING_ENCODE_TYPE &ReadEncode = VSET_UTF_16)
    {
        ASSERT(ReadDataSize >= -1);
        ASSERT_R_STR(FileName);
        VOL_STRING_ENCODE_TYPE StrEncode = ReadEncode;
        pStr.reset(new std::basic_string<CharT>{});
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
            PIV_IF(sizeof(EncodeType) == 2)
            {
                pStr->resize(fileSize / sizeof(CharT));
                Succeeded = (fread(const_cast<CharT *>(pStr->data()), sizeof(CharT), fileSize / sizeof(CharT), in) == fileSize / sizeof(CharT));
            }
            else
            {
                std::unique_ptr<char> data{new char[fileSize]};
                Succeeded = (fread(const_cast<char *>(data.get()), 1, fileSize, in) == fileSize);
                PIV_IF(sizeof(EncodeType) == 3)
                {
                    PivU2W utf{reinterpret_cast<const char *>(data.get()), fileSize};
                    pStr->assign(reinterpret_cast<const CharT *>(utf.GetText()), utf.GetLength());
                }
                else
                {
                    PivA2W utf{reinterpret_cast<const char *>(data.get()), fileSize};
                    pStr->assign(reinterpret_cast<const CharT *>(utf.GetText()), utf.GetLength());
                }
            }
        }
        else if (StrEncode == VSET_UTF_8)
        {
            PIV_IF(sizeof(EncodeType) == 3)
            {
                pStr->resize(fileSize);
                Succeeded = (fread(const_cast<CharT *>(pStr->data()), 1, fileSize, in) == fileSize);
            }
            else
            {
                std::unique_ptr<char> data{new char[fileSize]};
                Succeeded = (fread(const_cast<char *>(data.get()), 1, fileSize, in) == fileSize);
                PIV_IF(sizeof(EncodeType) == 2)
                {
                    PivW2U utf{reinterpret_cast<const wchar_t *>(data.get()), fileSize / 2};
                    pStr->assign(reinterpret_cast<const CharT *>(utf.GetText()), utf.GetLength());
                }
                else
                {
                    PivA2U utf{reinterpret_cast<const char *>(data.get()), fileSize};
                    pStr->assign(reinterpret_cast<const CharT *>(utf.GetText()), utf.GetLength());
                }
            }
        }
        else if (StrEncode == VSET_MBCS)
        {
            PIV_IF(sizeof(EncodeType) == 1)
            {
                pStr->resize(fileSize / sizeof(CharT));
                Succeeded = (fread(const_cast<CharT *>(pStr->data()), sizeof(CharT), fileSize / sizeof(CharT), in) == fileSize / sizeof(CharT));
            }
            else
            {
                std::unique_ptr<char> data{new char[fileSize]};
                Succeeded = (fread(const_cast<char *>(data.get()), 1, fileSize, in) == fileSize);
                PIV_IF(sizeof(EncodeType) == 2)
                {
                    PivW2A mbcs{reinterpret_cast<const wchar_t *>(data.get()), fileSize / 2};
                    pStr->assign(reinterpret_cast<const CharT *>(mbcs.GetText()), mbcs.GetLength());
                }
                PIV_ELSE_IF(sizeof(EncodeType) == 3)
                {
                    PivU2A mbcs{reinterpret_cast<const char *>(data.get()), fileSize};
                    pStr->assign(reinterpret_cast<const CharT *>(mbcs.GetText()), mbcs.GetLength());
                }
            }
        }
        fclose(in);
        if (Succeeded)
        {
            pStr.reset();
            sv.swap(piv::basic_string_view<CharT>{});
        }
        else
        {
            sv = *pStr;
        }
        return Succeeded;
    }

    /**
     * @brief 写出文本文件
     * @param FileName 所欲写到文件名
     * @param WriteDataSize 所欲写出文本长度
     * @param WriteEncode 所欲写出文本编码
     * @return
     */
    bool WriteIntoFile(const wchar_t *FileName, const int32_t &WriteDataSize = -1, const VOL_STRING_ENCODE_TYPE &WriteEncode = VSET_UTF_16, const bool &with_bom = true)
    {
        ASSERT(WriteDataSize >= -1);
        ASSERT_R_STR(FileName);
        ASSERT(WriteEncode != VSET_UNKNOWN);
        FILE *out = _wfopen(FileName, L"wb");
        if (out == NULL)
            return false;
        bool Succeeded = false;
        size_t count = (WriteDataSize == -1) ? sv.size() : ((static_cast<size_t>(WriteDataSize) > sv.size()) ? sv.size() : static_cast<size_t>(WriteDataSize));
        if (WriteEncode == VSET_UTF_16)
        {
            if (with_bom)
            {
                const byte bom[] = {0xFF, 0xFE};
                fwrite(bom, 1, 2, out);
            }
            PIV_IF(sizeof(EncodeType) == 2)
            {
                Succeeded = (fwrite(sv.data(), sizeof(CharT), count, out) == count);
            }
            PIV_ELSE_IF(sizeof(EncodeType) == 3)
            {
                PivU2W utf{reinterpret_cast<const char *>(sv.data()), count};
                Succeeded = (fwrite(utf.GetText(), 2, utf.GetLength(), out) == utf.GetLength());
            }
            else
            {
                PivA2W utf{reinterpret_cast<const char *>(sv.data()), count};
                Succeeded = (fwrite(utf.GetText(), 2, utf.GetLength(), out) == utf.GetLength());
            }
        }
        else if (WriteEncode == VSET_UTF_8)
        {
            if (with_bom)
            {
                const byte bom[] = {0xEF, 0xBB, 0xBF};
                fwrite(bom, 1, 3, out);
            }
            PIV_IF(sizeof(EncodeType) == 2)
            {
                PivW2U utf{reinterpret_cast<const wchar_t *>(sv.data()), count};
                Succeeded = (fwrite(utf.GetText(), 1, utf.GetLength(), out) == utf.GetLength());
            }
            PIV_ELSE_IF(sizeof(EncodeType) == 3)
            {
                Succeeded = (fwrite(sv.data(), sizeof(CharT), count, out) == count);
            }
            else
            {
                PivA2U utf{reinterpret_cast<const char *>(sv.data()), count};
                Succeeded = (fwrite(utf.GetText(), 1, utf.GetLength(), out) == utf.GetLength());
            }
        }
        else if (WriteEncode == VSET_MBCS)
        {
            PIV_IF(sizeof(EncodeType) == 2)
            {
                PivW2A mbcs{reinterpret_cast<const wchar_t *>(sv.data()), count};
                Succeeded = (fwrite(mbcs.GetText(), 1, mbcs.GetLength(), out) == mbcs.GetLength());
            }
            PIV_ELSE_IF(sizeof(EncodeType) == 3)
            {
                PivU2A mbcs{reinterpret_cast<const char *>(sv.data()), count};
                Succeeded = (fwrite(mbcs.GetText(), 1, mbcs.GetLength(), out) == mbcs.GetLength());
            }
            else
            {
                Succeeded = (fwrite(sv.data(), sizeof(CharT), count, out) == count);
            }
        }
        fclose(out);
        return Succeeded;
    }

    /**
     * @brief 取文本指针
     * @return 返回文本指针
     */
    inline const CharT *GetText() const
    {
        return sv.data();
    }

    /**
     * @brief 取文本长度
     */
    inline size_t GetLength() const
    {
        return sv.length();
    }

    /**
     * @brief 取字节长度
     */
    inline size_t GetSize() const
    {
        return sv.size() * sizeof(CharT);
    }

    /**
     * @brief 取文字长度
     */
    inline size_t GetWordLength() const
    {
        return piv::edit::get_word_length<CharT, EncodeType>(sv);
    }

    /**
     * @brief 取文本哈希值
     * @return
     */
    inline size_t GetHash() const
    {
        return std::hash<piv::basic_string_view<CharT>>{}(sv);
    }
    /**
     * @brief 取文本大小写无关哈希值
     * @return
     */
    inline size_t GetIHash() const
    {
        return piv::detail::ci_hash(sv);
    }

    /**
     * @brief 取引用计数
     */
    inline int32_t UseConut() const
    {
        return pStr.use_count();
    }

    /**
     * @brief 是否为空
     * @return 返回文本视图是否为空
     */
    inline bool IsEmpty() const
    {
        return sv.empty();
    }

    /**
     * @brief 是否相同
     * @param other 所欲比较的文本视图类
     * @param case_sensitive 是否区分大小写
     * @return 返回两个文本视图类的内容是否相同
     */
    inline bool IsEqual(const PivStringView &other, const bool &case_sensitive = false) const
    {
        if (case_sensitive)
            return (sv == other.sv);
        else
            return piv::edit::iequals(sv, other.sv);
    }
    inline bool IsEqual(const piv::basic_string_view<CharT> &other, const bool &case_sensitive = false) const
    {
        if (case_sensitive)
            return (sv == other);
        else
            return piv::edit::iequals(sv, other);
    }
    inline bool IsEqual(const std::basic_string<CharT> &other, const bool &case_sensitive = false) const
    {
        return IsEqual(PivStringView{other}, case_sensitive);
    }
    inline bool IsEqual(const CVolString &other, const bool &case_sensitive = false) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return IsEqual(PivStringView{other}, case_sensitive);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return IsEqual(PivStringView{reinterpret_cast<const CharT *>(PivW2U{other}.GetText())}, case_sensitive);
        }
        else
        {
            return IsEqual(PivStringView{reinterpret_cast<const CharT *>(PivW2A{other}.GetText())}, case_sensitive);
        }
    }
    inline bool IsEqual(const CVolMem &other, const bool &case_sensitive = false) const
    {
        return IsEqual(PivStringView{other}, case_sensitive);
    }

    /**
     * @brief 是否相同(不区分大小写)
     * @param other 所欲比较的文本视图类
     * @return 返回两个文本视图类的内容是否相同
     */
    inline bool IIsEqual(const PivStringView &other) const
    {
        return piv::edit::iequals(sv, other.sv)
    }
    inline bool IIsEqual(const piv::basic_string_view<CharT> &other) const
    {
        return piv::edit::iequals(sv, other)
    }
    inline bool IIsEqual(const std::basic_string<CharT> &other) const
    {
        return IIsEqual(PivStringView{other});
    }
    inline bool IIsEqual(const CVolString &other) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return IIsEqual(PivStringView{other});
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return IIsEqual(PivStringView{reinterpret_cast<const CharT *>(PivW2U{other}.GetText())});
        }
        else
        {
            return IIsEqual(PivStringView{reinterpret_cast<const CharT *>(PivW2A{other}.GetText())});
        }
    }
    inline bool IIsEqual(const CVolMem &other) const
    {
        return IIsEqual(PivStringView{other});
    }

    /**
     * @brief 文本比较
     * @param s 所欲比较的文本
     * @param s case_insensitive 是否区分大小写
     * @return
     */
    inline int32_t compare(const piv::basic_string_view<CharT> &s, const bool &case_insensitive = true) const
    {
        if (case_insensitive)
        {
            return sv.compare(0, sv.size(), s.data(), s.size());
        }
        else
        {
            int32_t ret;
            PIV_IF(sizeof(CharT) == 2)
            {
                ret = wcsnicmp(reinterpret_cast<const wchar_t *>(sv.data()), reinterpret_cast<const wchar_t *>(s.data()), (std::min)(sv.size(), s.size()));
            }
            else
            {
                ret = strnicmp(reinterpret_cast<const char *>(sv.data()), reinterpret_cast<const char *>(s.data()), (std::min)(sv.size(), s.size()));
            }
            if (ret == 0)
            {
                if (sv.size() < s.size())
                    return -1;
                else
                    return 1;
            }
            return ret;
        }
    }
    inline int32_t compare(const CharT *s, const bool &case_insensitive = true) const
    {
        return this->compare(piv::basic_string_view<CharT>{s}, case_insensitive);
    }
    inline int32_t compare(const std::basic_string<CharT> &s, const bool &case_insensitive = true) const
    {
        if (case_insensitive)
        {
            return sv.compare(0, sv.size(), s.data, s.size());
        }
        else
        {
            int32_t ret;
            PIV_IF(sizeof(CharT) == 2)
            {
                ret = wcsnicmp(reinterpret_cast<const wchar_t *>(sv.data()), reinterpret_cast<const wchar_t *>(s.data()), (std::min)(sv.size(), s.size()));
            }
            else
            {
                ret = strnicmp(reinterpret_cast<const char *>(sv.data()), reinterpret_cast<const char *>(s.data()), (std::min)(sv.size(), s.size()));
            }
            if (ret == 0)
            {
                if (sv.size() < s.size())
                    return -1;
                else
                    return 1;
            }
            return ret;
        }
    }
    inline int32_t compare(const CVolMem &s, const bool &case_insensitive = true) const
    {
        return this->compare(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize() / sizeof(CharT))}, case_insensitive);
    }
    inline int32_t compare(const CVolString &s, const bool &case_insensitive = true) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return this->compare(reinterpret_cast<const CharT *>(s.GetText()), case_insensitive);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return this->compare(reinterpret_cast<const CharT *>(PivW2U{s}.GetText()), case_insensitive);
        }
        else
        {
            return this->compare(reinterpret_cast<const CharT *>(PivW2A{s}.GetText()), case_insensitive);
        }
    }

    /**
     * @brief 后移起点
     * @param n 所欲从视图起始移除的字符数
     */
    inline PivStringView &RemovePrefix(const size_t &n)
    {
        ASSERT(n <= sv.size()); // 判断移除的字符数是否不大于视图长度
        sv.remove_prefix(n);
        return *this;
    }

    /**
     * @brief 前移终点
     * @param n 所欲从视图终点移除的字符数
     */
    inline PivStringView &RemoveSuffix(const size_t &n)
    {
        ASSERT(n <= sv.size()); // 判断移除的字符数是否不大于视图长度
        sv.remove_suffix(n);
        return *this;
    }

    /**
     * @brief 复制文本
     * @param dest 欲复制到的目标
     * @param pos 欲复制的起始索引位置
     * @param count 欲复制的字符数目
     * @return 复制的字符数
     */
    inline size_t CopyStr(CharT *dest, const size_t &pos, const size_t &count) const
    {
        ASSERT(pos <= sv.size()); // 判断索引位置是否有效
        return sv.copy(dest, count, pos);
    }
    inline size_t CopyStr(ptrdiff_t dest, const size_t &pos, const size_t &count) const
    {
        return CopyStr(reinterpret_cast<CharT *>(dest), pos, count);
    }
    inline size_t CopyStr(CVolString &dest, const size_t &pos, const size_t &count) const
    {
        dest.SetLength(count);
        size_t n = CopyStr(const_cast<CharT *>(reinterpret_cast<const CharT *>(dest.GetText())), pos, count);
        if (n == 0)
            dest.Empty();
        else
            dest.RemoveChars(n, count - n);
        return n;
    }
    inline size_t CopyStr(CVolMem &dest, const size_t &pos, const size_t &count) const
    {
        dest.Alloc(count, TRUE);
        size_t n = CopyStr(const_cast<CharT *>(reinterpret_cast<const CharT *>(dest.GetPtr())), pos, count);
        if (n == 0)
            dest.Empty();
        else
            dest.Realloc(n);
        return n;
    }
    inline size_t CopyStr(std::basic_string<CharT> &dest, const size_t &pos, const size_t &count) const
    {
        dest.resize(count, '\0');
        size_t n = CopyStr(const_cast<CharT *>(dest.data()), pos, count);
        if (n == 0)
            dest.clear();
        else
            dest.resize(n);
        return n;
    }

    /**
     * @brief 到字节集
     * @return
     */
    inline CVolMem ToVolMem(const bool &null_terminated) const
    {
        return CVolMem(reinterpret_cast<const void *>(sv.data()), (sv.size() + (null_terminated ? 1 : 0)) * sizeof(CharT));
    }

    /**
     * @brief 到文本型
     * @return
     */
    inline CVolString ToVolString() const
    {
        ASSERT(sizeof(CharT) == 2); // 只有UTF-16LE编码的文本视图可以置入火山文本型;
        return CVolString(reinterpret_cast<const wchar_t *>(sv.data()), sv.size());
    }

    /**
     * @brief 到整数
     * @return
     */
    inline int32_t ToInt(const int32_t &radix = 0) const
    {
        PIV_IF(sizeof(CharT) == 2)
        {
            return static_cast<int32_t>(wcstoul(reinterpret_cast<const wchar_t *>(std::basic_string<CharT>{sv.data(), sv.size()}.c_str()), nullptr, radix));
        }
        else
        {
            return static_cast<int32_t>(strtoul(reinterpret_cast<const char *>(std::basic_string<CharT>{sv.data(), sv.size()}.c_str()), nullptr, radix));
        }
    }

    /**
     * @brief 到长整数
     * @param base 进制
     * @return
     */
    inline int64_t ToInt64(const int32_t &radix = 0) const
    {
        PIV_IF(sizeof(CharT) == 2)
        {
            return static_cast<int64_t>(wcstoull(reinterpret_cast<const wchar_t *>(std::basic_string<CharT>{sv.data(), sv.size()}.c_str()), nullptr, radix));
        }
        else
        {
            return static_cast<int64_t>(strtoull(reinterpret_cast<const char *>(std::basic_string<CharT>{sv.data(), sv.size()}.c_str()), nullptr, radix));
        }
    }

    /**
     * @brief 到小数
     * @return
     */
    inline double ToDouble() const
    {
        PIV_IF(sizeof(CharT) == 2)
        {
            return _wtof(reinterpret_cast<const wchar_t *>(std::basic_string<CharT>{sv.data(), sv.size()}.c_str()));
        }
        else
        {
            return atof(reinterpret_cast<const char *>(std::basic_string<CharT>{sv.data(), sv.size()}.c_str()));
        }
    }

    /**
     * @brief 到标准文本
     * @return
     */
    inline std::basic_string<CharT> ToString() const
    {
        return std::basic_string<CharT>{sv.data(), sv.size()};
    }

    /**
     * @brief 取字符
     */
    inline const CharT &At(const size_t &pos) const
    {
        // 判断索引是否有效和视图是否为空
        ASSERT(pos >= 0 && pos < sv.size() && sv.empty() == false);
        return sv[pos];
    }

    /**
     * @brief 取首字符
     */
    inline const CharT &Front() const
    {
        return sv.empty() ? 0 : sv.front();
    }

    /**
     * @brief 取尾字符
     */
    inline const CharT &Back() const
    {
        return sv.empty() ? 0 : sv.back();
    }

    /**
     * @brief 取文本左边
     * @param count 欲取出字符的数目
     * @return 返回文本视图类
     */
    inline PivStringView Left(const size_t &count) const
    {
        return PivStringView{sv.substr(0, count), pStr};
    }

    /**
     * @brief 取文本中间
     * @param pos 起始取出索引位置
     * @param count 欲取出字符的数目
     * @return 返回文本视图类
     */
    inline PivStringView Middle(const size_t &pos, const size_t &count) const
    {
        ASSERT(pos <= sv.size()); // 判断索引位置是否有效
        return PivStringView{sv.substr(pos, count), pStr};
    }

    /**
     * @brief 取文本右边
     * @param count 欲取出字符的数目
     * @return 返回文本视图类
     */
    inline PivStringView Right(const size_t &count) const
    {
        size_t pos = count > sv.size() ? 0 : sv.size() - count;
        return PivStringView{sv.substr(pos, count), pStr};
    }

    /**
     * @brief 取左边文字
     * @param count 欲取出文字的数目
     * @return 返回文本视图类
     */
    inline PivStringView LeftWords(const size_t &count) const
    {
        return PivStringView{piv::edit::substr<CharT, EncodeType>(sv, 0, count), pStr};
    }

    /**
     * @brief 取中间文字
     * @param pos 起始取出索引位置
     * @param count 欲取出文字的数目
     * @return 返回文本视图类
     */
    inline PivStringView MiddleWords(const size_t &pos, const size_t &count) const
    {
        return PivStringView{piv::edit::substr<CharT, EncodeType>(sv, pos, count), pStr};
    }

    /**
     * @brief 取右边文字
     * @param count 欲取出文字的数目
     * @return 返回文本视图类
     */
    inline PivStringView RightWords(const size_t &count) const
    {
        size_t length = piv::edit::get_word_length<CharT, EncodeType>(sv);
        size_t pos = (count >= length) ? 0 : (length - count);
        return PivStringView{piv::edit::substr<CharT, EncodeType>(sv, pos, count), pStr};
    }

    /**
     * @brief 取子文本中间
     * @param lstr 开始文本
     * @param rstr 结束文本
     * @param pos 起始搜寻位置
     * @param case_insensitive 是否不区分大小写
     * @param include_lstr 是否包含开始文本
     * @param include_rstr 是否包含结束文本
     * @param retorg_failed 失败返回原文本
     * @return
     */
    template <typename L, typename R>
    inline PivStringView GetMiddle(const L &lstr, const R &rstr, const size_t &pos, const bool &case_insensitive = false, const bool &include_lstr = false, const bool &include_rstr = false, const bool &retorg_failed = false) const
    {
        size_t strlen = 0;
        size_t spos = SearchText(lstr, pos, case_insensitive, &strlen);
        if (spos == -1)
            return retorg_failed ? PivStringView{sv} : PivStringView{};
        size_t epos = spos + strlen;
        if (!include_lstr)
            spos = epos;
        epos = SearchText(rstr, epos, case_insensitive, &strlen);
        if (epos == -1)
            return retorg_failed ? PivStringView{sv} : PivStringView{};
        return PivStringView{sv.substr(spos, epos - spos + (include_rstr ? strlen : 0))};
    }
    template <typename L, typename R>
    inline size_t GetMiddle(std::vector<piv::basic_string_view<CharT>> &array, const L &lstr, const R &rstr, const size_t &pos, const bool &case_insensitive = false, const bool &include_lstr = false, const bool &include_rstr = false) const
    {
        array.clear();
        size_t strlen = 0, spos = pos, epos = 0;
        while (true)
        {
            spos = SearchText(lstr, spos, case_insensitive, &strlen);
            if (spos == -1)
                break;
            epos = spos + strlen;
            if (!include_lstr)
                spos = epos;
            epos = SearchText(rstr, epos, case_insensitive, &strlen);
            if (epos == -1)
                break;
            array.push_back(sv.substr(spos, epos - spos + (include_rstr ? strlen : 0)));
        }
        return array.size();
    }

    /**
     * @brief 取子文本左边
     * @param lstr 要查找的文本
     * @param pos 起始搜寻位置
     * @param case_insensitive 是否不区分大小写
     * @param include_lstr 是否包含查找的文本
     * @param reverse 是否反向查找
     * @param retorg_failed 失败返回原文本
     * @return
     */
    template <typename L>
    inline PivStringView GetLeft(const L &lstr, const size_t &pos, const bool &case_insensitive = false, const bool &include_lstr = false, const bool &reverse = false, const bool &retorg_failed = false) const
    {
        size_t spos, strlen = 0;
        if (reverse)
            spos = ReverseSearchText(lstr, pos, case_insensitive, &strlen);
        else
            spos = SearchText(lstr, pos, case_insensitive, &strlen);
        if (spos == -1)
            return retorg_failed ? PivStringView{sv} : PivStringView{};
        return PivStringView{sv.substr(0, spos + (include_lstr ? strlen : 0))};
    }

    /**
     * @brief 取子文本右边
     * @param rstr 要查找的文本
     * @param pos 起始搜寻位置
     * @param case_insensitive 是否不区分大小写
     * @param include_rstr 是否包含查找的文本
     * @param reverse 是否反向查找
     * @param retorg_failed 失败返回原文本
     * @return
     */
    template <typename R>
    inline PivStringView GetRight(const R &rstr, const size_t &pos, const bool &case_insensitive = false, const bool &include_rstr = false, const bool &reverse = false, const bool &retorg_failed = false) const
    {
        size_t spos, strlen = 0;
        if (reverse)
            spos = ReverseSearchText(rstr, pos, case_insensitive, &strlen);
        else
            spos = SearchText(rstr, pos, case_insensitive, &strlen);
        if (spos == -1)
            return retorg_failed ? PivStringView{sv} : PivStringView{};
        if (!include_rstr)
            spos += strlen;
        return PivStringView{sv.substr(spos, sv.size() - spos)};
    }

    /**
     * @brief 寻找字符
     * @param character 欲寻找的字符
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t FindChar(const CharT &character, const size_t &off = 0) const
    {
        if (off >= sv.size())
            return piv::basic_string_view<CharT>::npos;
        return sv.find(character, off);
    }

    /**
     * @brief 倒找字符
     * @param character 欲寻找的字符
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t ReverseFindChar(const CharT &character, const size_t &off = (size_t)-1) const
    {
        if (off >= sv.size())
            return piv::basic_string_view<CharT>::npos;
        return sv.rfind(character, (off == (size_t)-1) ? sv.size() : off);
    }

    /**
     * @brief 寻找字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t FindFirstOf(const piv::basic_string_view<CharT> &chars, const size_t &pos = 0) const
    {
        return sv.find_first_of(chars, pos);
    }
    inline size_t FindFirstOf(const CharT *chars, const size_t &pos = 0, const size_t &count = (size_t)-1) const
    {
        return (count == (size_t)-1) ? FindFirstOf(piv::basic_string_view<CharT>{chars}, pos)
                                     : FindFirstOf(piv::basic_string_view<CharT>{chars, count}, pos);
    }
    inline size_t FindFirstOf(const CVolMem &chars, const size_t &pos = 0) const
    {
        return FindFirstOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(chars.GetPtr()), static_cast<size_t>(chars.GetSize()) / sizeof(CharT)}, pos);
    }
    inline size_t FindFirstOf(const std::basic_string<CharT> &chars, const size_t &pos = 0) const
    {
        return FindFirstOf(piv::basic_string_view<CharT>{chars}, pos);
    }
    inline size_t FindFirstOf(const CVolString &chars, const size_t &pos = 0) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return FindFirstOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return FindFirstOf(reinterpret_cast<const CharT *>(PivW2U{chars}.GetText()), pos);
        }
        else
        {
            return FindFirstOf(reinterpret_cast<const CharT *>(PivW2A{chars}.GetText()), pos);
        }
    }

    /**
     * @brief 倒找字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t FindLastOf(const piv::basic_string_view<CharT> &chars, const size_t &pos = (size_t)-1) const
    {
        return sv.find_last_of(chars, (pos == (size_t)-1) ? sv.size() : pos);
    }
    inline size_t FindLastOf(const CharT *chars, const size_t &pos = (size_t)-1, const size_t &count = (size_t)-1) const
    {
        return (count == (size_t)-1) ? FindLastOf(piv::basic_string_view<CharT>{chars}, pos)
                                     : FindLastOf(piv::basic_string_view<CharT>{chars, count}, pos);
    }
    inline size_t FindLastOf(const CVolMem &chars, const size_t &pos = (size_t)-1) const
    {
        return FindLastOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(chars.GetPtr()), static_cast<size_t>(chars.GetSize()) / sizeof(CharT)}, pos);
    }
    inline size_t FindLastOf(const std::basic_string<CharT> &chars, const size_t &pos = (size_t)-1) const
    {
        return FindLastOf(piv::basic_string_view<CharT>{chars}, pos);
    }
    inline size_t FindLastOf(const CVolString &chars, const size_t &pos = (size_t)-1) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return FindLastOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return FindLastOf(reinterpret_cast<const CharT *>(PivW2U{chars}.GetText()), pos);
        }
        else
        {
            return FindLastOf(reinterpret_cast<const CharT *>(PivW2A{chars}.GetText()), pos);
        }
    }

    /**
     * @brief 寻找不符合字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t FindFirstNotOf(const piv::basic_string_view<CharT> &chars, const size_t &pos = 0) const
    {
        return sv.find_first_not_of(chars, pos);
    }
    inline size_t FindFirstNotOf(const CharT *chars, const size_t &pos = 0, const size_t &count = (size_t)-1) const
    {
        return (count == -1) ? FindFirstNotOf(piv::basic_string_view<CharT>{chars}, pos)
                             : FindFirstNotOf(piv::basic_string_view<CharT>{chars, count}, pos);
    }
    inline size_t FindFirstNotOf(const CVolMem &chars, const size_t &pos = 0) const
    {
        return FindFirstNotOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(chars.GetPtr()), static_cast<size_t>(chars.GetSize()) / sizeof(CharT)}, pos);
    }
    inline size_t FindFirstNotOf(const std::basic_string<CharT> &chars, const size_t &pos = 0) const
    {
        return FindFirstNotOf(piv::basic_string_view<CharT>{chars}, pos);
    }
    inline size_t FindFirstNotOf(const CVolString &chars, const size_t &pos = 0) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return FindFirstNotOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return FindFirstNotOf(reinterpret_cast<const CharT *>(PivW2U{chars}.GetText()), pos);
        }
        else
        {
            return FindFirstNotOf(reinterpret_cast<const CharT *>(PivW2A{chars}.GetText()), pos);
        }
    }

    /**
     * @brief 倒找字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t FindLastNotOf(const piv::basic_string_view<CharT> &chars, const size_t &pos = (size_t)-1) const
    {
        return sv.find_last_not_of(chars, (pos == (size_t)-1) ? sv.size() : pos);
    }
    inline size_t FindLastNotOf(const CharT *chars, const size_t &pos = (size_t)-1, const size_t &count = (size_t)-1) const
    {
        return (count == (size_t)-1) ? FindLastNotOf(piv::basic_string_view<CharT>{chars}, pos)
                                     : FindLastNotOf(piv::basic_string_view<CharT>{chars, count}, pos);
    }
    inline size_t FindLastNotOf(const CVolMem &chars, const size_t &pos = (size_t)-1) const
    {
        return FindLastNotOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(chars.GetPtr()), static_cast<size_t>(chars.GetSize()) / sizeof(CharT)}, pos);
    }
    inline size_t FindLastNotOf(const std::basic_string<CharT> &chars, const size_t &pos = (size_t)-1) const
    {
        return FindLastNotOf(piv::basic_string_view<CharT>{chars}, pos);
    }
    inline size_t FindLastNotOf(const CVolString &chars, const size_t &pos = (size_t)-1) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return FindLastNotOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return FindLastNotOf(reinterpret_cast<const CharT *>(PivW2U{chars}.GetText()), pos);
        }
        else
        {
            return FindLastNotOf(reinterpret_cast<const CharT *>(PivW2A{chars}.GetText()), pos);
        }
    }

    /**
     * @brief 寻找文本
     * @param text 欲寻找的文本
     * @param pos 起始搜寻位置
     * @param case_insensitive 是否不区分大小写
     * @param count 欲寻找文本的长度
     * @param textlen 返回文本长度
     * @return 寻找到的位置
     */
    inline size_t SearchText(const piv::basic_string_view<CharT> &text, const size_t &pos = 0, const bool &case_insensitive = false, size_t *textlen = nullptr) const
    {
        if (textlen != nullptr)
            *textlen = text.size();
        if (!case_insensitive)
            return sv.find(text, pos);
        else
            return piv::edit::ifind(sv, text, pos);
    }
    inline size_t SearchText(const std::basic_string<CharT> &text, const size_t &pos = 0, const bool &case_insensitive = false, size_t *textlen = nullptr) const
    {
        if (textlen != nullptr)
            *textlen = text.size();
        if (!case_insensitive)
            return str.find(text, pos);
        else
            return piv::edit::ifind(piv::basic_string_view<CharT>{str}, piv::basic_string_view<CharT>{text}, pos);
    }
    inline size_t SearchText(const CharT *text, const size_t &pos = 0, const bool &case_insensitive = false, const size_t &count = (size_t)-1, size_t *textlen = nullptr) const
    {
        return SearchText((count == (size_t)-1) ? piv::basic_string_view<CharT>{text}
                                                : piv::basic_string_view<CharT>{text, count},
                          pos, case_insensitive, textlen);
    }
    inline size_t SearchText(const CVolMem &text, const size_t &pos = 0, const bool &case_insensitive = false, size_t *textlen = nullptr) const
    {
        return SearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetPtr()), static_cast<size_t>(text.GetSize()) / sizeof(CharT)}, pos, case_insensitive, textlen);
    }
    inline size_t SearchText(const CVolString &text, const size_t &pos = 0, const bool &case_insensitive = false, size_t *textlen = nullptr) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return SearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetText())}, pos, case_insensitive, textlen);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return SearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{text}.GetText())}, pos, case_insensitive, textlen);
        }
        else
        {
            return SearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{text}.GetText())}, pos, case_insensitive, textlen);
        }
    }

    /**
     * @brief 倒找文本
     * @param text 欲寻找的文本
     * @param pos 起始搜寻位置
     * @param count 欲寻找文本的长度
     * @param case_insensitive 是否不区分大小写
     * @param textlen 返回文本长度
     * @return 寻找到的位置
     */
    inline size_t ReverseSearchText(const piv::basic_string_view<CharT> &text, const size_t &pos = (size_t)-1, const bool &case_insensitive = false, size_t *textlen = nullptr) const
    {
        if (textlen != nullptr)
            *textlen = text.size();
        if (!case_insensitive)
            return sv.rfind(text, (pos == (size_t)-1) ? sv.size() : pos);
        else
            return piv::edit::irfind(sv, text, (pos == (size_t)-1) ? sv.size() : pos);
    }
    inline size_t ReverseSearchText(const std::basic_string<CharT> &text, const size_t &pos = (size_t)-1, const bool &case_insensitive = false, size_t *textlen = nullptr) const
    {
        return ReverseSearchText(piv::basic_string_view<CharT>{text}, pos, case_insensitive, textlen);
    }
    inline size_t ReverseSearchText(const CharT *text, const size_t &pos = (size_t)-1, const bool &case_insensitive = false, const size_t &count = (size_t)-1, size_t *textlen = nullptr) const
    {
        return ReverseSearchText((count == (size_t)-1) ? piv::basic_string_view<CharT>{text} : piv::basic_string_view<CharT>{text, count}, pos, case_insensitive, textlen);
    }
    inline size_t ReverseSearchText(const CVolMem &text, const size_t &pos = (size_t)-1, const bool &case_insensitive = false, size_t *textlen = nullptr) const
    {
        return ReverseSearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetPtr()), static_cast<size_t>(text.GetSize()) / sizeof(CharT)}, pos, case_insensitive, textlen);
    }
    inline size_t ReverseSearchText(const CVolString &text, const size_t &pos = (size_t)-1, const bool &case_insensitive = false, size_t *textlen = nullptr) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return ReverseSearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetText())}, pos, case_insensitive, textlen);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return ReverseSearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{text}.GetText())}, pos, case_insensitive, textlen);
        }
        else
        {
            return ReverseSearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{text}.GetText())}, pos, case_insensitive, textlen);
        }
    }

    /**
     * @brief 是否以字符/文本开头
     * @param s 所欲检查的文本
     * @param ch 所欲检查的字符
     * @param count 所欲检查文本的长度
     * @param case_sensitive 是否区分大小写
     * @return
     */
    inline bool LeadOf(const CharT &ch, const bool &case_sensitive = true) const
    {
        if (sv.empty())
            return false;
        if (case_sensitive)
            return (sv.front() == ch);
        else
            return (std::tolower(static_cast<unsigned char>(sv.front())) == std::tolower(static_cast<unsigned char>(ch)));
    }
    inline bool LeadOf(const piv::basic_string_view<CharT> &s, const bool &case_sensitive = true) const
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }
    inline bool LeadOf(const CharT *s, const bool &case_sensitive = true, size_t count = (size_t)-1) const
    {
        return (SearchText(s, 0, !case_sensitive, count) == 0);
    }
    inline bool LeadOf(const std::basic_string<CharT> &s, const bool &case_sensitive = true) const
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }
    inline bool LeadOf(const CVolMem &s, const bool &case_sensitive = true) const
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }
    inline bool LeadOf(const CVolString &s, const bool &case_sensitive = true) const
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }

    /**
     * @brief 是否以字符/文本结束
     * @param s 所欲检查的文本
     * @param ch 所欲检查的字符
     * @param count 所欲检查文本的长度
     * @param case_sensitive 是否区分大小写
     * @return
     */
    inline bool EndOf(const CharT &ch, const bool &case_sensitive = true) const
    {
        if (sv.empty())
            return false;
        if (case_sensitive)
            return (sv.back() == ch);
        else
            return (std::tolower(static_cast<unsigned char>(sv.back())) == std::tolower(static_cast<unsigned char>(ch)));
    }
    inline bool EndOf(const piv::basic_string_view<CharT> &s, const bool &case_sensitive = true) const
    {
        return (ReverseSearchText(s, -1, !case_sensitive) == (sv.size() - s.size()));
    }
    inline bool EndOf(const CharT *s, const bool &case_sensitive = true, const size_t &count = (size_t)-1)
    {
        return EndOf((count == -1) ? piv::basic_string_view<CharT>{s} : piv::basic_string_view<CharT>{s, static_cast<size_t>(count)}, case_sensitive);
    }
    inline bool EndOf(const std::basic_string<CharT> &s, const bool &case_sensitive = true) const
    {
        return EndOf(piv::basic_string_view<CharT>{s}, case_sensitive);
    }
    inline bool EndOf(const CVolMem &s, const bool &case_sensitive = true) const
    {
        return EndOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT)}, case_sensitive);
    }
    inline bool EndOf(const CVolString &s, const bool &case_sensitive = true) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return EndOf(reinterpret_cast<const CharT *>(s.GetText()), case_sensitive);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return EndOf(reinterpret_cast<const CharT *>(PivW2U{s}.GetText()), case_sensitive);
        }
        else
        {
            return EndOf(reinterpret_cast<const CharT *>(PivW2A{s}.GetText()), case_sensitive);
        }
    }

    /**
     * @brief 删首空
     */
    inline PivStringView &TrimLeft()
    {
        piv::edit::trim_left(sv);
        return *this;
    }

    /**
     * @brief 删尾空
     */
    inline PivStringView &TrimRight()
    {
        piv::edit::trim_right(sv);
        return *this;
    }

    /**
     * @brief 删首尾空
     */
    inline PivStringView &TrimAll()
    {
        piv::edit::trim_left(sv);
        piv::edit::trim_right(sv);
        return *this;
    }

    /**
     * @brief 分割文本
     * @param delimit 分割字符集合
     * @param svArray 结果存放数组
     * @param trimAll 是否删除首尾空
     * @param ignoreEmptyStr 是否忽略空白结果
     * @param max_count 最大分割数
     * @return 所分割出来的结果文本数目
     */
    size_t SplitStrings(const piv::basic_string_view<CharT> &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        svArray.clear();
        if (delimit.empty())
        {
            svArray.push_back(sv);
        }
        else
        {
            size_t n = 0, spos = 0, fpos = 0, epos = sv.size();
            piv::basic_string_view<CharT> str;
            while (fpos < epos)
            {
                fpos = sv.find_first_of(delimit, spos);
                if (fpos != piv::basic_string_view<CharT>::npos && ++n < max_count)
                {
                    str = sv.substr(spos, fpos - spos);
                    spos = fpos + 1;
                }
                else
                {
                    str = sv.substr(spos, sv.size() - spos);
                    fpos = epos;
                }
                if (trimAll)
                {
                    piv::edit::trim_left(str);
                    piv::edit::trim_right(str);
                    if (ignoreEmptyStr && str.empty())
                        continue;
                }
                else if (ignoreEmptyStr)
                {
                    if (str.empty())
                        continue;
                    bool isEmpty = true;
                    for (size_t i = 0; i < str.size(); i++)
                    {
                        // if (!std::isspace(static_cast<unsigned char>(str[i])))
                        if (static_cast<unsigned char>(str[i]) > ' ')
                        {
                            isEmpty = false;
                            break;
                        }
                    }
                    if (isEmpty)
                        continue;
                }
                svArray.push_back(str);
            }
        }
        return svArray.size();
    }
    inline size_t SplitStrings(const CharT *delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        return SplitStrings(piv::basic_string_view<CharT>{delimit}, svArray, trimAll, ignoreEmptyStr, max_count);
    }
    inline size_t SplitStrings(const CVolMem &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        return SplitStrings(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(delimit.GetPtr()), static_cast<size_t>(delimit.GetSize()) / sizeof(CharT)}, svArray, trimAll, ignoreEmptyStr, max_count);
    }
    inline size_t SplitStrings(const std::basic_string<CharT> &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        return SplitStrings(piv::basic_string_view<CharT>{delimit}, svArray, trimAll, ignoreEmptyStr, max_count);
    }
    inline size_t SplitStrings(const CVolString &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return SplitStrings(reinterpret_cast<const CharT *>(delimit.GetText()), svArray, trimAll, ignoreEmptyStr, max_count);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return SplitStrings(reinterpret_cast<const CharT *>(PivW2U{delimit}.GetText()), svArray, trimAll, ignoreEmptyStr, max_count);
        }
        else
        {
            return SplitStrings(reinterpret_cast<const CharT *>(PivW2A{delimit}.GetText()), svArray, trimAll, ignoreEmptyStr, max_count);
        }
    }

    /**
     * @brief 分割子文本
     * @param delimit 分割用子文本
     * @param svArray 结果存放数组
     * @param trimAll 是否删除首尾空
     * @param ignoreEmptyStr 是否忽略空白结果
     * @return 所分割出来的结果文本数目
     */
    size_t SplitSubStrings(const piv::basic_string_view<CharT> &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        svArray.clear();
        if (delimit.empty())
        {
            svArray.push_back(sv);
        }
        else
        {
            size_t n = 0, spos = 0, fpos = 0, epos = sv.size();
            piv::basic_string_view<CharT> str;
            while (fpos < epos)
            {
                fpos = sv.find(delimit, spos);
                if (fpos != piv::basic_string_view<CharT>::npos && ++n < max_count)
                {
                    str = sv.substr(spos, fpos - spos);
                    spos = fpos + delimit.size();
                }
                else
                {
                    str = sv.substr(spos, sv.size() - spos);
                    fpos = epos;
                }
                if (trimAll)
                {
                    piv::edit::trim_left(str);
                    piv::edit::trim_right(str);
                    if (ignoreEmptyStr && str.empty())
                        continue;
                }
                else if (ignoreEmptyStr)
                {
                    if (str.empty())
                        continue;
                    bool isEmpty = true;
                    for (auto it = str.begin(); it != str.end(); it++)
                    {
                        if (*it > ' ')
                        {
                            isEmpty = false;
                            break;
                        }
                    }
                    if (isEmpty)
                        continue;
                }
                svArray.push_back(str);
            }
        }
        return svArray.size();
    }
    inline size_t SplitSubStrings(const CharT *delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        return SplitSubStrings(piv::basic_string_view<CharT>{delimit}, svArray, trimAll, ignoreEmptyStr, max_count);
    }
    inline size_t SplitSubStrings(const CVolMem &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        return SplitSubStrings(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(delimit.GetPtr()), static_cast<size_t>(delimit.GetSize()) / sizeof(CharT)}, svArray, trimAll, ignoreEmptyStr, max_count);
    }
    inline size_t SplitSubStrings(const std::basic_string<CharT> &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        return SplitSubStrings(piv::basic_string_view<CharT>{delimit}, svArray, trimAll, ignoreEmptyStr, max_count);
    }
    inline size_t SplitSubStrings(const CVolString &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return SplitSubStrings(reinterpret_cast<const CharT *>(delimit.GetText()), svArray, trimAll, ignoreEmptyStr, max_count);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return SplitSubStrings(reinterpret_cast<const CharT *>(PivW2U{delimit}.GetText()), svArray, trimAll, ignoreEmptyStr, max_count);
        }
        else
        {
            return SplitSubStrings(reinterpret_cast<const CharT *>(PivW2A{delimit}.GetText()), svArray, trimAll, ignoreEmptyStr, max_count);
        }
    }

    /**
     * @brief 到小写
     * @return
     */
    inline std::basic_string<CharT> MakeLower() const
    {
        std::basic_string<CharT> lower;
        lower.resize(sv.size());
        std::transform(sv.begin(), sv.end(), lower.begin(), (int (*)(int))std::tolower);
        return lower;
    }

    /**
     * @brief 到大写
     * @return
     */
    inline std::basic_string<CharT> MakeUpper() const
    {
        std::basic_string<CharT> upper;
        upper.resize(sv.size());
        std::transform(sv.begin(), sv.end(), upper.begin(), (int (*)(int))std::toupper);
        return upper;
    }

    /**
     * @brief 到全角
     * @param option 转换选项
     * @return 返回转换后的文本
     */
    inline std::basic_string<CharT> ToFullWidth(const int32_t &option) const
    {
        return piv::edit::to_fullwidth<CharT, EncodeType>(sv, option);
    }

    /**
     * @brief 到半角
     * @param option 转换选项
     * @return 返回转换后的文本
     */
    inline std::basic_string<CharT> ToHalfWidth(const int32_t &option) const
    {
        return piv::edit::to_halfwidth<CharT, EncodeType>(sv, option);
    }

    /**
     * @brief 到十六进制文本
     * @param hexstr 返回的十六进制文本
     * @return
     */
    inline std::basic_string<CharT> &ToHex(const bool &separator = false, std::basic_string<CharT> &hexstr = std::basic_string<CharT>{}) const
    {
        return piv::encoding::str_to_hex(sv, separator, hexstr);
    }
    inline CVolString &ToHexStr(const bool &separator = false, CVolString &hexstr = CVolString{}) const
    {
        return piv::encoding::str_to_hex(sv, separator, hexstr);
    }

    /**
     * @brief 到USC2
     * @param en_ascii 是否编码ASCII字符
     * @return
     */
    inline std::basic_string<CharT> &ToUsc2(const bool &en_ascii = false, std::basic_string<CharT> &usc2 = std::basic_string<CharT>{}) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return piv::encoding::to_usc2<CharT>(sv, en_ascii, usc2);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return piv::encoding::to_usc2<CharT>(piv::wstring_view{*PivU2W{reinterpret_cast<const char *>(sv.data()), sv.size()}}, en_ascii, usc2);
        }
        else
        {
            return piv::encoding::to_usc2<CharT>(piv::wstring_view{*PivA2W{reinterpret_cast<const char *>(sv.data()), sv.size()}}, en_ascii, usc2);
        }
    }
    inline CVolString &ToUsc2Str(const bool &en_ascii = false, CVolString &usc2 = CVolString{}) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return piv::encoding::to_usc2str(sv, en_ascii, usc2);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return piv::encoding::to_usc2str(piv::wstring_view{*PivU2W{reinterpret_cast<const char *>(sv.data()), sv.size()}}, en_ascii, usc2);
        }
        else
        {
            return piv::encoding::to_usc2str(piv::wstring_view{*PivA2W{reinterpret_cast<const char *>(sv.data()), sv.size()}}, en_ascii, usc2);
        }
    }

    /**
     * @brief URL编码
     * @param utf8
     * @param ReservedWord 是否不编码保留字符
     * @return
     */
    inline std::basic_string<CharT> UrlEncode(const bool &utf8 = true, const bool &ReservedWord = false) const
    {
        return piv::encoding::UrlStrEncode(str, utf8, ReservedWord);
    }

    /**
     * @brief URL解码
     * @param utf8 是否为UTF-8编码
     * @return
     */
    inline std::basic_string<CharT> UrlDecode(const bool &utf8 = true) const
    {
        return piv::encoding::UrlStrDecode(str, utf8);
    }

    /**
     * @brief BASE64编码
     * @return
     */
    inline std::basic_string<CharT> EncodeBase64(const int32_t &line_len = 0) const
    {
        return piv::base64<CharT>{}.encode(sv, line_len);
    }

    /**
     * @brief BASE64解码
     * @return
     */
    inline std::basic_string<CharT> DecodeBase64() const
    {
        return piv::base64<CharT>{}.decode(sv);
    }

    /**
     * @brief BASE64到字节集
     * @return
     */
    inline CVolMem DecodeBase64Bin() const
    {
        CVolMem buffer;
        piv::base64<CharT>{}.decode(sv, buffer);
        return buffer;
    }

    /**
     * @brief BASE85编码
     * @return
     */
    inline std::basic_string<CharT> EncodeBase85(const bool &padding = false) const
    {
        return piv::base85<CharT>{}.encode(sv, padding);
    }

    /**
     * @brief BASE85解码
     * @return
     */
    inline std::basic_string<CharT> DecodeBase85() const
    {
        return piv::base85<CharT>{}.decode(sv);
    }

    /**
     * @brief BASE85到字节集
     * @return
     */
    inline CVolMem DecodeBase85Bin() const
    {
        CVolMem buffer;
        piv::base85<CharT>{}.decode(sv, buffer);
        return buffer;
    }

    /**
     * @brief BASE91编码
     * @return
     */
    inline std::basic_string<CharT> EncodeBase91() const
    {
        return piv::base91<CharT>{}.encode(sv);
    }

    /**
     * @brief BASE91解码
     * @return
     */
    inline std::basic_string<CharT> DecodeBase91() const
    {
        return piv::base91<CharT>{}.decode(sv);
    }

    /**
     * @brief BASE64到字节集
     * @return
     */
    inline CVolMem DecodeBase91Bin() const
    {
        CVolMem buffer;
        piv::base91<CharT>{}.decode(sv, buffer);
        return buffer;
    }

}; // PivStringView

/*********************************************
 * @brief  标准文本模板类
 * @tparam CharT 字符类型
 * @tparam EncodeType 文本编码,按类型的尺寸区分(1=ANSI; 2=UTF-16LE; 3=UTF8;)
 */
template <typename CharT, typename EncodeType>
class PivString : public CVolObject
{
private:
    std::basic_string<CharT> str;

public:
    using EncodeType_t = EncodeType;

    /**
     * @brief 默认构造和析构函数
     */
    PivString() {}
    ~PivString() {}

    /**
     * @brief 复制和移动构造函数
     * @param s 所欲复制的文本
     * @param count 所欲复制的字符长度
     */
    PivString(const PivString &s)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            if (s.type() == 2)
                str = s.str;
            else if (s.type() == 3)
                str.assign(reinterpret_cast<const CharT *>(PivU2W{reinterpret_cast<const char *>(s.GetText())}.GetText()));
            else
                str.assign(reinterpret_cast<const CharT *>(PivA2W{reinterpret_cast<const char *>(s.GetText())}.GetText()));
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            if (s.type() == 2)
                str.assign(reinterpret_cast<const CharT *>(PivW2U{reinterpret_cast<const wchar_t *>(s.GetText())}.GetText()));
            else if (s.type() == 3)
                str = s.str;
            else
                str.assign(reinterpret_cast<const CharT *>(PivA2U{reinterpret_cast<const char *>(s.GetText())}.GetText()));
        }
        else
        {
            if (s.type() == 2)
                str.assign(reinterpret_cast<const CharT *>(PivW2A{reinterpret_cast<const wchar_t *>(s.GetText())}.GetText()));
            else if (s.type() == 3)
                str.assign(reinterpret_cast<const CharT *>(PivU2A{reinterpret_cast<const char *>(s.GetText())}.GetText()));
            else
                str = s.str;
        }
        // str = s.str;
    }
    PivString(PivString &&s) noexcept
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            if (s.type() == 2)
                str = std::move(s.str);
            else if (s.type() == 3)
                str.assign(reinterpret_cast<const CharT *>(PivU2W{reinterpret_cast<const char *>(s.GetText())}.GetText()));
            else
                str.assign(reinterpret_cast<const CharT *>(PivA2W{reinterpret_cast<const char *>(s.GetText())}.GetText()));
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            if (s.type() == 2)
                str.assign(reinterpret_cast<const CharT *>(PivW2U{reinterpret_cast<const wchar_t *>(s.GetText())}.GetText()));
            else if (s.type() == 3)
                str = std::move(s.str);
            else
                str.assign(reinterpret_cast<const CharT *>(PivA2U{reinterpret_cast<const char *>(s.GetText())}.GetText()));
        }
        else
        {
            if (s.type() == 2)
                str.assign(reinterpret_cast<const CharT *>(PivW2A{reinterpret_cast<const wchar_t *>(s.GetText())}.GetText()));
            else if (s.type() == 3)
                str.assign(reinterpret_cast<const CharT *>(PivU2A{reinterpret_cast<const char *>(s.GetText())}.GetText()));
            else
                str = std::move(s.str);
        }
        // str = std::move(s.str);
    }
    PivString(const std::basic_string<char> &s)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            str.assign(reinterpret_cast<const CharT *>(PivU2W{s}.GetText()));
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            str = s;
        }
        else
        {
            str = s;
        }
    }
    PivString(std::basic_string<char> &&s)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            str.assign(reinterpret_cast<const CharT *>(PivU2W{s}.GetText()));
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            str = std::move(s);
        }
        else
        {
            str = std::move(s);
        }
    }
    PivString(const std::basic_string<wchar_t> &s)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            str.assign(reinterpret_cast<const CharT *>(s.c_str()));
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            str.assign(reinterpret_cast<const CharT *>(PivW2U{s}.GetText()));
        }
        else
        {
            str.assign(reinterpret_cast<const CharT *>(PivW2A{s}.GetText()));
        }
    }
    PivString(std::basic_string<wchar_t> &&s)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            str = std::move(s);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            str.assign(reinterpret_cast<const CharT *>(PivW2U{s}.GetText()));
        }
        else
        {
            str.assign(reinterpret_cast<const CharT *>(PivW2A{s}.GetText()));
        }
    }
    PivString(const piv::basic_string_view<char> &s)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            str.assign(reinterpret_cast<const CharT *>(PivU2W{s}.GetText()));
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            str.assign(reinterpret_cast<const CharT *>(s.data()), s.size());
        }
        else
        {
            str.assign(reinterpret_cast<const CharT *>(s.data()), s.size());
        }
    }
    PivString(const piv::basic_string_view<wchar_t> &s)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            str.assign(reinterpret_cast<const CharT *>(s.data()), s.size());
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            str.assign(reinterpret_cast<const CharT *>(PivW2U{s}.GetText()));
        }
        else
        {
            str.assign(reinterpret_cast<const CharT *>(PivW2A{s}.GetText()));
        }
    }
    PivString(const char *s, size_t count = (size_t)-1)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            str.assign(reinterpret_cast<const CharT *>(PivU2W{s, count}.GetText()));
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            if (count == (size_t)-1)
                str.assign(reinterpret_cast<const CharT *>(s));
            else
                str.assign(reinterpret_cast<const CharT *>(s), count);
        }
        else
        {
            if (count == (size_t)-1)
                str.assign(reinterpret_cast<const CharT *>(s));
            else
                str.assign(reinterpret_cast<const CharT *>(s), count);
        }
    }
    PivString(const wchar_t *s, size_t count = (size_t)-1)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            if (count == (size_t)-1)
                str.assign(reinterpret_cast<const CharT *>(s));
            else
                str.assign(reinterpret_cast<const CharT *>(s), count);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            str.assign(reinterpret_cast<const CharT *>(PivW2U{s, count}.GetText()));
        }
        else
        {
            str.assign(reinterpret_cast<const CharT *>(PivW2A{s, count}.GetText()));
        }
    }
    PivString(const CVolString &s)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            str.assign(reinterpret_cast<const CharT *>(s.GetText()));
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            str.assign(reinterpret_cast<const CharT *>(PivW2U{s}.GetText()));
        }
        else
        {
            str.assign(reinterpret_cast<const CharT *>(PivW2A{s}.GetText()));
        }
    }
    PivString(const CVolMem &s)
    {
        str.assign(reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT));
    }

    inline operator std::basic_string<CharT> &() noexcept
    {
        return str;
    }
    inline operator std::basic_string<CharT> const &() const noexcept
    {
        return str;
    }

    /**
     * @brief 赋值操作符
     * @param other 所欲复制的标准文本类
     * @return 返回自身
     */
    inline PivString &operator=(const PivString &other)
    {
        str = other.str;
        return *this;
    }
    inline PivString &operator=(PivString &&other) noexcept
    {
        str = std::move(other.str);
        return *this;
    }
    inline PivString &operator=(const CharT &s)
    {
        str = s;
        return *this;
    }
    inline PivString &operator=(const std::basic_string<CharT> &s)
    {
        str = s;
        return *this;
    }
    inline PivString &operator=(std::basic_string<CharT> &&s) noexcept
    {
        str = std::move(s);
        return *this;
    }

    /**
     * @brief 比较操作符
     * @param other 所欲比较的标准文本类
     * @return 返回两个文本的内容是否相同
     */
    inline bool operator==(const PivString &other) const
    {
        return str == other.str;
    }
    inline bool operator<(const PivString &other) const
    {
        return str < other.str;
    }
    inline bool operator<=(const PivString &other) const
    {
        return str <= other.str;
    }
    inline bool operator>(const PivString &other) const
    {
        return str > other.str;
    }
    inline bool operator>=(const PivString &other) const
    {
        return str > other.str;
    }

    inline bool operator==(const std::basic_string<CharT> &other) const
    {
        return str == other;
    }
    inline bool operator<(const std::basic_string<CharT> &other) const
    {
        return str < other;
    }
    inline bool operator<=(const std::basic_string<CharT> &other) const
    {
        return str <= other;
    }
    inline bool operator>(const std::basic_string<CharT> &other) const
    {
        return str > other;
    }
    inline bool operator>=(const std::basic_string<CharT> &other) const
    {
        return str > other;
    }

    /**
     * @brief 获取文本类中的string
     * @return 返回string
     */
    inline std::basic_string<CharT> &data()
    {
        return str;
    }
    inline const std::basic_string<CharT> &data() const
    {
        return str;
    }

    /**
     * @brief 获取文本类中的string指针
     */
    inline std::basic_string<CharT> *pdata()
    {
        return &str;
    }
    inline const std::basic_string<CharT> *pdata() const
    {
        return &str;
    }

    inline int32_t type() const
    {
        return static_cast<int32_t>(sizeof(EncodeType));
    }

    /**
     * @brief 取展示内容(调试输出)
     * @param strDump 展示内容
     * @param nMaxDumpSize 最大展示数据尺寸
     */
    virtual void GetDumpString(CVolString &strDump, INT nMaxDumpSize) override
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            strDump.SetText(reinterpret_cast<const wchar_t *>(str.c_str()));
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            PivU2Ws{reinterpret_cast<const char *>(str.c_str())}.GetStr(strDump);
        }
        else
        {
            PivA2Ws{reinterpret_cast<const char *>(str.c_str())}.GetStr(strDump);
        }
    }

    /**
     * @brief 置对象数据
     * @param stream 基本输入流
     */
    virtual void LoadFromStream(CVolBaseInputStream &stream)
    {
        str.clear();
        if (stream.eof())
            return;
        uint32_t Size;
        stream.read(&Size, 4);
        str.resize(Size / sizeof(CharT));
        if (stream.ReadExact(const_cast<CharT *>(str.data()), Size))
        {
            if (str.back() == '\0')
                str.pop_back();
        }
        else
        {
            str.clear();
        }
    }

    /**
     * @brief 取对象数据
     * @param stream 基本输出流
     */
    virtual void SaveIntoStream(CVolBaseOutputStream &stream)
    {
        uint32_t Size = static_cast<uint32_t>((str.size() + 1) * sizeof(CharT));
        stream.write(&Size, 4);
        stream.write(str.data(), Size);
    }

    /**
     * @brief 迭代器
     * @return
     */
    auto begin()
    {
        return str.begin();
    }
    auto end()
    {
        return str.end();
    }

    /**
     * @brief 取文本指针
     * @return 返回文本指针
     */
    inline const CharT *GetText() const
    {
        return str.c_str();
    }

    /**
     * @brief 取文本长度
     */
    inline size_t GetLength() const
    {
        return str.length();
    }

    /**
     * @brief 取字节长度
     */
    inline size_t GetSize() const
    {
        return str.size() * sizeof(CharT);
    }

    /**
     * @brief 取文字长度
     */
    inline size_t GetWordLength() const
    {
        return piv::edit::get_word_length<CharT, EncodeType>(piv::basic_string_view<CharT>{str});
    }

    /**
     * @brief 取文本哈希值
     * @param case_insensitive 是否不区分大小写
     * @return
     */
    inline size_t GetHash() const
    {
        return std::hash<std::basic_string<CharT>>{}(str);
    }

    /**
     * @brief 取文本大小写无关哈希值
     * @return
     */
    inline size_t GetIHash() const
    {
        return piv::detail::ci_hash(str);
    }

    /**
     * @brief 是否为空
     * @return 返回文本是否为空
     */
    inline bool IsEmpty() const
    {
        return str.empty();
    }

    /**
     * @brief 是否相同
     * @param other 所欲比较的标准文本类
     * @param case_sensitive 是否区分大小写
     * @return 返回两个文本类的内容是否相同
     */
    inline bool IsEqual(const PivString &other, const bool &case_sensitive = false) const
    {
        if (case_sensitive)
            return (str == other.str);
        else
            return piv::edit::iequals(str, other.str);
    }
    inline bool IsEqual(const piv::basic_string_view<CharT> &other, const bool &case_sensitive = false) const
    {
        if (case_sensitive)
            return (piv::basic_string_view<CharT>{str} == other);
        else
            return piv::edit::iequals(str, other);
    }
    inline bool IsEqual(const CVolString &other, const bool &case_sensitive = false) const
    {
        PIV_IF(sizeof(CharT) == 2)
        {
            return IsEqual(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(other.GetText())}, case_sensitive);
        }
        else
        {
            return IsEqual(PivString{other}, case_sensitive);
        }
    }
    inline bool IsEqual(const CVolMem &other, const bool &case_sensitive = false) const
    {
        return IsEqual(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(other.GetPtr()), static_cast<size_t>(other.GetSize() / sizeof(CharT))}, case_sensitive);
    }

    /**
     * @brief 是否相同(不区分大小写)
     * @param other 所欲比较的标准文本类
     * @return 返回两个文本类的内容是否相同
     */
    inline bool IIsEqual(const PivString &other) const
    {
        return piv::edit::iequals(str, other.str);
    }
    inline bool IIsEqual(const piv::basic_string_view<CharT> &other) const
    {
        return piv::edit::iequals(str, other);
    }
    inline bool IIsEqual(const CVolString &other) const
    {
        PIV_IF(sizeof(CharT) == 2)
        {
            return IIsEqual(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(other.GetText())});
        }
        else
        {
            return IIsEqual(PivString{other});
        }
    }
    inline bool IIsEqual(const CVolMem &other) const
    {
        return IIsEqual(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(other.GetPtr()), static_cast<size_t>(other.GetSize() / sizeof(CharT))});
    }

    /**
     * @brief 文本比较
     * @param s 所欲比较的文本
     * @param s case_insensitive 是否区分大小写
     * @return
     */
    inline int32_t compare(const CharT *s, const bool &case_insensitive = true) const
    {
        if (case_insensitive)
        {
            return str.compare(s);
        }
        else
        {
            PIV_IF(sizeof(CharT) == 2)
            {
                return wcsicmp(reinterpret_cast<const wchar_t *>(str.c_str()), reinterpret_cast<const wchar_t *>(s));
            }
            else
            {
                return stricmp(reinterpret_cast<const char *>(str.c_str()), reinterpret_cast<const char *>(s));
            }
        }
    }
    inline int32_t compare(const std::basic_string<CharT> &s, const bool &case_insensitive = true) const
    {
        if (case_insensitive)
        {
            return str.compare(s);
        }
        else
        {
            PIV_IF(sizeof(CharT) == 2)
            {
                return wcsicmp(reinterpret_cast<const wchar_t *>(str.c_str()), reinterpret_cast<const wchar_t *>(s.c_str()), str.size());
            }
            else
            {
                return stricmp(reinterpret_cast<const char *>(str.c_str()), reinterpret_cast<const char *>(s.c_str()), str.size());
            }
        }
    }
    inline int32_t compare(const piv::basic_string_view<CharT> &s, const bool &case_insensitive = true) const
    {
        if (case_insensitive)
        {
            return str.compare(0, str.size(), s.data(), s.size());
        }
        else
        {
            int32_t ret;
            PIV_IF(sizeof(CharT) == 2)
            {
                ret = wcsnicmp(reinterpret_cast<const wchar_t *>(str.c_str()), reinterpret_cast<const wchar_t *>(s.data()), (std::min)(str.size(), s.size()));
            }
            else
            {
                ret = strnicmp(reinterpret_cast<const char *>(str.c_str()), reinterpret_cast<const char *>(s.data()), (std::min)(str.size(), s.size()));
            }
            if (ret == 0)
            {
                if (str.size() < s.size())
                    return -1;
                else
                    return 1;
            }
            return ret;
        }
    }
    inline int32_t compare(const CVolMem &s, const bool &case_insensitive = true) const
    {
        return this->compare(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize() / sizeof(CharT))}, case_insensitive);
    }
    inline int32_t compare(const CVolString &s, const bool &case_insensitive = true) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return this->compare(reinterpret_cast<const CharT *>(s.GetText()), case_insensitive);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return this->compare(reinterpret_cast<const CharT *>(PivW2U{s}.GetText()), case_insensitive);
        }
        else
        {
            return this->compare(reinterpret_cast<const CharT *>(PivW2A{s}.GetText()), case_insensitive);
        }
    }

    /**
     * @brief 清空文本
     */
    inline PivString &Clear()
    {
        str.clear();
        return *this;
    }

    /**
     * @brief 填充文本
     * @param count 重复的字符数量
     * @param ch 所欲填充的字符
     */
    inline PivString &InitWithChars(const size_t &count, const CharT &ch)
    {
        str.assign(count, ch);
        return *this;
    }

    /**
     * @brief 置文本
     * @param s 所欲置入的文本数据
     * @param count 文本长度
     * @return 返回自身
     */
    inline PivString &SetText(const CharT *s, const size_t &count = (size_t)-1)
    {
        if (count == (size_t)-1)
            str.assign(s);
        else
            str.assign(s, count);
        if (!str.empty() && str.back() == '\0')
            str.pop_back();
        return *this;
    }
    inline PivString &SetText(const ptrdiff_t s, const size_t &count)
    {
        return SetText(reinterpret_cast<const CharT *>(s), count);
    }
    inline PivString &SetText(const CVolString &s, const size_t &count = (size_t)-1)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            str.assign(reinterpret_cast<const CharT *>(s.GetText()), (count == (size_t)-1) ? static_cast<size_t>(s.GetLength()) : count);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            PivW2U utf8{s};
            str.assign(reinterpret_cast<const CharT *>(utf8.GetText()), (count == (size_t)-1) ? utf8.GetLength() : count);
        }
        else
        {
            PivW2A ansi{s};
            str.assign(reinterpret_cast<const CharT *>(ansi.GetText()), (count == (size_t)-1) ? ansi.GetLength() : count);
        }
        return *this;
    }
    inline PivString &SetText(const CVolMem &s, const size_t &count = (size_t)-1)
    {
        str.assign(reinterpret_cast<const CharT *>(s.GetPtr()),
                   (count == (size_t)-1) ? static_cast<size_t>(s.GetSize()) / sizeof(CharT) : count);
        return *this;
    }
    inline PivString &SetText(const piv::basic_string_view<CharT> &s, const size_t &count = (size_t)-1)
    {
        str.assign(s.data(), (count == (size_t)-1) ? s.size() : count);
        return *this;
    }
    inline PivString &SetText(const int64_t &n)
    {
        CharT buf[32] = {'\0'};
        PIV_IF(sizeof(CharT) == 2)
        {
            _i64tow(n, reinterpret_cast<wchar_t *>(buf), 10);
        }
        else
        {
            _i64toa(n, reinterpret_cast<char *>(buf), 10);
        }
        str.assign(buf);
        return *this;
    }
    inline PivString &SetText(const int32_t &n)
    {
        CharT buf[16] = {'\0'};
        PIV_IF(sizeof(CharT) == 2)
        {
            _ltow(n, reinterpret_cast<wchar_t *>(buf), 10);
        }
        else
        {
            _ltoa(n, reinterpret_cast<char *>(buf), 10);
        }
        str.assign(buf);
        return *this;
    }
    inline PivString &SetText(const wchar_t &n)
    {
        CharT buf[8] = {'\0'};
        PIV_IF(sizeof(CharT) == 2)
        {
            _ultow(n, reinterpret_cast<wchar_t *>(buf), 10);
        }
        else
        {
            _ultoa(n, reinterpret_cast<char *>(buf), 10);
        }
        str.assign(buf);
        return *this;
    }
    inline PivString &SetText(const int16_t &n)
    {
        CharT buf[8] = {'\0'};
        PIV_IF(sizeof(CharT) == 2)
        {
            _ltow(n, reinterpret_cast<wchar_t *>(buf), 10);
        }
        else
        {
            _ltoa(n, reinterpret_cast<char *>(buf), 10);
        }
        str.assign(buf);
        return *this;
    }
    inline PivString &SetText(const int8_t &n)
    {
        CharT buf[8] = {'\0'};
        PIV_IF(sizeof(CharT) == 2)
        {
            _ltow(n, reinterpret_cast<wchar_t *>(buf), 10);
        }
        else
        {
            _ltoa(n, reinterpret_cast<char *>(buf), 10);
        }
        str.assign(buf);
        return *this;
    }
    inline PivString &SetText(const double &n)
    {
        CharT buf[128] = {'\0'};
        PIV_IF(sizeof(CharT) == 2)
        {
            wsprintfW(reinterpret_cast<wchar_t *>(buf), L"%lf", n);
        }
        else
        {
            sprintf(reinterpret_cast<char *>(buf), "%lf", n);
        }
        str.assign(buf);
        return *this;
    }
    inline PivString &SetText(const float &n)
    {
        CharT buf[128] = {'\0'};
        PIV_IF(sizeof(CharT) == 2)
        {
            wsprintfW(reinterpret_cast<wchar_t *>(buf), L"%f", n);
        }
        else
        {
            sprintf(reinterpret_cast<char *>(buf), "%f", n);
        }
        str.assign(buf);
        return *this;
    }

    /**
     * @brief 置文件资源
     * @param resId 所欲指向的数据资源
     * @param storeBuf 缓存文本数据
     * @return
     */
    bool LoadResData(const size_t &resId)
    {
        str.clear();
        if (resId == 0)
        {
            return false;
        }
        HMODULE hModule = g_objVolApp.GetInstanceHandle();
        HRSRC hSrc = ::FindResourceW(hModule, MAKEINTRESOURCE(static_cast<WORD>(resId)), RT_RCDATA);
        if (hSrc == NULL)
        {
            return false;
        }
        HGLOBAL resdata = ::LoadResource(hModule, hSrc);
        if (resdata == NULL)
        {
            return false;
        }
        byte *data = reinterpret_cast<byte *>(::LockResource(resdata));
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
        str.assign(reinterpret_cast<const CharT *>(data), count / sizeof(CharT));
        return true;
    }

    /**
     * @brief 读入文本文件
     * @param FileName 所欲读取文件名
     * @param ReadDataSize 所欲读取数据尺寸
     * @param ReadEncode 所欲读取文本编码
     * @return
     */
    bool ReadFromFile(const wchar_t *FileName, const int32_t &ReadDataSize = -1, const VOL_STRING_ENCODE_TYPE &ReadEncode = VSET_UTF_16)
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
            PIV_IF(sizeof(EncodeType) == 2)
            {
                str.resize(fileSize / sizeof(CharT));
                Succeeded = (fread(const_cast<CharT *>(str.data()), sizeof(CharT), fileSize / sizeof(CharT), in) == fileSize / sizeof(CharT));
            }
            else
            {
                std::unique_ptr<char> data{new char[fileSize]};
                Succeeded = (fread(const_cast<char *>(data.get()), 1, fileSize, in) == fileSize);
                PIV_IF(sizeof(EncodeType) == 3)
                {
                    PivU2W utf{reinterpret_cast<const char *>(data.get()), fileSize};
                    str.assign(reinterpret_cast<const CharT *>(utf.GetText()), utf.GetLength());
                }
                else
                {
                    PivA2W utf{reinterpret_cast<const char *>(data.get()), fileSize};
                    str.assign(reinterpret_cast<const CharT *>(utf.GetText()), utf.GetLength());
                }
            }
        }
        else if (StrEncode == VSET_UTF_8)
        {
            PIV_IF(sizeof(EncodeType) == 3)
            {
                str.resize(fileSize);
                Succeeded = (fread(const_cast<CharT *>(str.data()), 1, fileSize, in) == fileSize);
            }
            else
            {
                std::unique_ptr<char> data{new char[fileSize]};
                Succeeded = (fread(const_cast<char *>(data.get()), 1, fileSize, in) == fileSize);
                PIV_IF(sizeof(EncodeType) == 2)
                {
                    PivW2U utf{reinterpret_cast<const wchar_t *>(data.get()), fileSize / 2};
                    str.assign(reinterpret_cast<const CharT *>(utf.GetText()), utf.GetLength());
                }
                else
                {
                    PivA2U utf{reinterpret_cast<const char *>(data.get()), fileSize};
                    str.assign(reinterpret_cast<const CharT *>(utf.GetText()), utf.GetLength());
                }
            }
        }
        else if (StrEncode == VSET_MBCS)
        {
            PIV_IF(sizeof(EncodeType) == 1)
            {
                str.resize(fileSize / sizeof(CharT));
                Succeeded = (fread(const_cast<CharT *>(str.data()), sizeof(CharT), fileSize / sizeof(CharT), in) == fileSize / sizeof(CharT));
            }
            else
            {
                std::unique_ptr<char> data{new char[fileSize]};
                Succeeded = (fread(const_cast<char *>(data.get()), 1, fileSize, in) == fileSize);
                PIV_IF(sizeof(EncodeType) == 2)
                {
                    PivW2A mbcs{reinterpret_cast<const wchar_t *>(data.get()), fileSize / 2};
                    str.assign(reinterpret_cast<const CharT *>(mbcs.GetText()), mbcs.GetLength());
                }
                else
                {
                    PivU2A mbcs{reinterpret_cast<const char *>(data.get()), fileSize};
                    str.assign(reinterpret_cast<const CharT *>(mbcs.GetText()), mbcs.GetLength());
                }
            }
        }
        fclose(in);
        if (!Succeeded)
            str.clear();
        return Succeeded;
    }

    /**
     * @brief 写出文本文件
     * @param FileName 所欲写到文件名
     * @param WriteDataSize 所欲写出文本长度
     * @param WriteEncode 所欲写出文本编码
     * @return
     */
    bool WriteIntoFile(const wchar_t *FileName, const int32_t &WriteDataSize = -1, const VOL_STRING_ENCODE_TYPE &WriteEncode = VSET_UTF_16, const bool &with_bom = true)
    {
        ASSERT(WriteDataSize >= -1);
        ASSERT_R_STR(FileName);
        ASSERT(WriteEncode != VSET_UNKNOWN);
        FILE *out = _wfopen(FileName, L"wb");
        if (out == NULL)
            return false;
        bool Succeeded = false;
        size_t count = (WriteDataSize == -1) ? str.size() : ((static_cast<size_t>(WriteDataSize) > str.size()) ? str.size() : static_cast<size_t>(WriteDataSize));
        if (WriteEncode == VSET_UTF_16)
        {
            if (with_bom)
            {
                const byte bom[] = {0xFF, 0xFE};
                fwrite(bom, 1, 2, out);
            }
            PIV_IF(sizeof(EncodeType) == 2)
            {
                Succeeded = (fwrite(str.data(), sizeof(CharT), count, out) == count);
            }
            PIV_ELSE_IF(sizeof(EncodeType) == 3)
            {
                PivU2W utf{reinterpret_cast<const char *>(str.c_str()), count};
                Succeeded = (fwrite(utf.GetText(), 2, utf.GetLength(), out) == utf.GetLength());
            }
            else
            {
                PivA2W utf{reinterpret_cast<const char *>(str.c_str()), count};
                Succeeded = (fwrite(utf.GetText(), 2, utf.GetLength(), out) == utf.GetLength());
            }
        }
        else if (WriteEncode == VSET_UTF_8)
        {
            if (with_bom)
            {
                const byte bom[] = {0xEF, 0xBB, 0xBF};
                fwrite(bom, 1, 3, out);
            }
            PIV_IF(sizeof(EncodeType) == 2)
            {
                PivW2U utf{reinterpret_cast<const wchar_t *>(str.c_str()), count};
                Succeeded = (fwrite(utf.GetText(), 1, utf.GetLength(), out) == utf.GetLength());
            }
            PIV_ELSE_IF(sizeof(EncodeType) == 3)
            {
                Succeeded = (fwrite(str.data(), sizeof(CharT), count, out) == count);
            }
            else
            {
                PivA2U utf{reinterpret_cast<const char *>(str.c_str()), count};
                Succeeded = (fwrite(utf.GetText(), 1, utf.GetLength(), out) == utf.GetLength());
            }
        }
        else if (WriteEncode == VSET_MBCS)
        {
            PIV_IF(sizeof(EncodeType) == 2)
            {
                PivW2A mbcs{reinterpret_cast<const wchar_t *>(str.c_str()), count};
                Succeeded = (fwrite(mbcs.GetText(), 1, mbcs.GetLength(), out) == mbcs.GetLength());
            }
            PIV_ELSE_IF(sizeof(EncodeType) == 3)
            {
                PivU2A mbcs{reinterpret_cast<const char *>(str.c_str()), count};
                Succeeded = (fwrite(mbcs.GetText(), 1, mbcs.GetLength(), out) == mbcs.GetLength());
            }
            else
            {
                Succeeded = (fwrite(str.data(), sizeof(CharT), count, out) == count);
            }
        }
        fclose(out);
        return Succeeded;
    }

    /**
     * @brief 置文本长度
     * @param length
     * @return
     */
    inline PivString &SetLength(const size_t &length)
    {
        str.resize(length, ' ');
        return *this;
    }

    /**
     * @brief 加入字符
     * @param ch 所欲添加的字符
     * @return
     */
    inline PivString &AddChar(const CharT &ch)
    {
        str.push_back(ch);
        return *this;
    }
    template <typename... Args>
    PivString &AddChar(const CharT &ch, const Args... args)
    {
        str.push_back(ch);
        return AddChar(args...);
    }

    /**
     * @brief 加入重复字符
     * @param count 所欲添加的次数
     * @param ch 所欲添加的字符
     * @return
     */
    inline PivString &AddManyChar(const size_t &count, const CharT &ch)
    {
        str.append(count, ch);
        return *this;
    }

    /**
     * @brief 加入文本
     * @param s 所欲加入到尾部的文本数据
     * @return 返回自身
     */
    inline PivString &AddText(const CharT *s)
    {
        str.append(s);
        return *this;
    }
    inline PivString &AddText(const CVolString &s)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            str.append(reinterpret_cast<const CharT *>(s.GetText()));
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            PivW2U utf8{s};
            str.append(reinterpret_cast<const CharT *>(utf8.GetText()));
        }
        else
        {
            PivW2A ansi{s};
            str.append(reinterpret_cast<const CharT *>(ansi.GetText()));
        }
        return *this;
    }
    inline PivString &AddText(const CVolConstString &s)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            str.append(reinterpret_cast<const CharT *>(s.GetText()));
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            PivW2U utf8{s};
            str.append(reinterpret_cast<const CharT *>(utf8.GetText()));
        }
        else
        {
            PivW2A ansi{s};
            str.append(reinterpret_cast<const CharT *>(ansi.GetText()));
        }
        return *this;
    }
    inline PivString &AddText(const CVolMem &s)
    {
        str.append(reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT));
        return *this;
    }
    inline PivString &AddText(const std::basic_string<CharT> &s)
    {
        str.append(s);
        return *this;
    }
    inline PivString &AddText(const piv::basic_string_view<CharT> &s)
    {
        str.append(s.data(), s.size());
        return *this;
    }
    inline PivString &AddText(const PivStringView<CharT, EncodeType> &s)
    {
        str.append(s.data().data(), s.data().size());
        return *this;
    }
    inline PivString &AddText(const PivString &s)
    {
        str.append(s.str);
        return *this;
    }
    inline PivString &AddText(const int64_t &n)
    {
        CharT buf[32] = {'\0'};
        PIV_IF(sizeof(CharT) == 2)
        {
            _i64tow(n, reinterpret_cast<wchar_t *>(buf), 10);
        }
        else
        {
            _i64toa(n, reinterpret_cast<char *>(buf), 10);
        }
        str.append(buf);
        return *this;
    }
    inline PivString &AddText(const int32_t &n)
    {
        CharT buf[16] = {'\0'};
        PIV_IF(sizeof(CharT) == 2)
        {
            _ltow(n, reinterpret_cast<wchar_t *>(buf), 10);
        }
        else
        {
            _ltoa(n, reinterpret_cast<char *>(buf), 10);
        }
        str.append(buf);
        return *this;
    }
    inline PivString &AddText(const wchar_t &n)
    {
        CharT buf[8] = {'\0'};
        PIV_IF(sizeof(CharT) == 2)
        {
            _ultow(n, reinterpret_cast<wchar_t *>(buf), 10);
        }
        else
        {
            _ultoa(n, reinterpret_cast<wchar_t *>(buf), 10);
        }
        str.append(buf);
        return *this;
    }
    inline PivString &AddText(const int16_t &n)
    {
        CharT buf[8] = {'\0'};
        PIV_IF(sizeof(CharT) == 2)
        {
            _ltow(n, reinterpret_cast<wchar_t *>(buf), 10);
        }
        else
        {
            _ltoa(n, reinterpret_cast<char *>(buf), 10);
        }
        str.append(buf);
        return *this;
    }
    inline PivString &AddText(const int8_t &n)
    {
        CharT buf[8] = {'\0'};
        PIV_IF(sizeof(CharT) == 2)
        {
            _ltow(n, reinterpret_cast<wchar_t *>(buf), 10);
        }
        else
        {
            _ltoa(n, reinterpret_cast<char *>(buf), 10);
        }
        str.append(buf);
        return *this;
    }
    inline PivString &AddText(const double &n)
    {
        CharT buf[128] = {'\0'};
        PIV_IF(sizeof(CharT) == 2)
        {
            wsprintfW(reinterpret_cast<wchar_t *>(buf), L"%lf", n);
        }
        else
        {
            sprintf(reinterpret_cast<char *>(buf), "%lf", n);
        }
        str.append(buf);
        return *this;
    }
    inline PivString &AddText(const float &n)
    {
        CharT buf[128] = {'\0'};
        PIV_IF(sizeof(CharT) == 2)
        {
            wsprintfW(reinterpret_cast<wchar_t *>(buf), L"%lf", n);
        }
        else
        {
            sprintf(reinterpret_cast<char *>(buf), "%lf", n);
        }
        str.append(buf);
        return *this;
    }
    template <typename T, typename... Args>
    PivString &AddText(const T &s, const Args... args)
    {
        return AddText(s).AddText(args...);
    }

    /**
     * @brief 加入无符号数值
     * @param value 所欲加入的无符号数值
     * @return
     */
    inline PivString &AddUnsignedValue(const int64_t &value)
    {
        CharT buf[32] = {0};
        PIV_IF(sizeof(CharT) == 2)
        {
            _ui64tow(static_cast<uint64_t>(value), reinterpret_cast<wchar_t *>(buf), 10);
        }
        else
        {
            _ui64toa(static_cast<uint64_t>(value), reinterpret_cast<char *>(buf), 10);
        }
        str.append(buf);
        return *this;
    }
    inline PivString &AddUnsignedValue(const int32_t &value)
    {
        CharT buf[16] = {0};
        PIV_IF(sizeof(CharT) == 2)
        {
            _ultow(static_cast<uint32_t>(value), reinterpret_cast<wchar_t *>(buf), 10);
        }
        else
        {
            _ultoa(static_cast<uint32_t>(value), reinterpret_cast<char *>(buf), 10);
        }
        str.append(buf);
        return *this;
    }
    inline PivString &AddUnsignedValue(const wchar_t &value)
    {
        CharT buf[8] = {0};
        PIV_IF(sizeof(CharT) == 2)
        {
            _ultow(value, reinterpret_cast<wchar_t *>(buf), 10);
        }
        else
        {
            _ultoa(value, reinterpret_cast<char *>(buf), 10);
        }
        str.append(buf);
        return *this;
    }
    inline PivString &AddUnsignedValue(const int16_t &value)
    {
        CharT buf[8] = {0};
        PIV_IF(sizeof(CharT) == 2)
        {
            _ultow(static_cast<uint16_t>(value), reinterpret_cast<wchar_t *>(buf), 10);
        }
        else
        {
            _ultoa(static_cast<uint16_t>(value), reinterpret_cast<char *>(buf), 10);
        }
        str.append(buf);
        return *this;
    }
    inline PivString &AddUnsignedValue(const int8_t &value)
    {
        CharT buf[8] = {0};
        PIV_IF(sizeof(CharT) == 2)
        {
            _ultow(static_cast<uint8_t>(value), reinterpret_cast<wchar_t *>(buf), 10);
        }
        else
        {
            _ultoa(static_cast<uint8_t>(value), reinterpret_cast<char *>(buf), 10);
        }
        str.append(buf);
        return *this;
    }

    /**
     * @brief 加入部分文本
     * @param s 所欲加入到尾部的文本数据
     * @param count 所欲加入的文本长度
     * @return 返回自身
     */
    inline PivString &Append(const CharT *s, const size_t &count = (size_t)-1)
    {
        if (count == (size_t)-1)
            str.append(s);
        else
            str.append(s, count);
        return *this;
    }
    inline PivString &Append(const CVolString &s, const size_t &count = (size_t)-1)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            str.append(reinterpret_cast<const CharT *>(s.GetText()), (count == (size_t)-1) ? static_cast<size_t>(s.GetLength()) : count);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            PivW2U utf8{s};
            str.append(reinterpret_cast<const CharT *>(utf8.GetText()), (count == (size_t)-1) ? utf8.GetLength() : count);
        }
        else
        {
            PivW2A ansi{s};
            str.append(reinterpret_cast<const CharT *>(ansi.GetText()), (count == (size_t)-1) ? ansi.GetLength() : count);
        }
        return *this;
    }
    inline PivString &Append(const CVolMem &s, const size_t &count = (size_t)-1)
    {
        str.append(reinterpret_cast<const CharT *>(s.GetPtr()),
                   (count == (size_t)-1) ? static_cast<size_t>(s.GetSize()) / sizeof(CharT) : count);
        return *this;
    }
    inline PivString &Append(const std::basic_string<CharT> &s, const size_t &count = (size_t)-1)
    {
        str.append(s, 0, (count == (size_t)-1) ? s.size() : count);
        return *this;
    }
    inline PivString &Append(const piv::basic_string_view<CharT> &s, const size_t &count = (size_t)-1)
    {
        str.append(s.data(), (count == (size_t)-1) ? s.size() : count);
        return *this;
    }

    /**
     * @brief 加入重复文本
     * @param s 所欲添加的文本
     * @param times 所欲添加的次数
     * @param count 所欲添加的文本长度
     * @return 返回自身
     */
    inline PivString &AddManyText(const CharT *s, const size_t &times, const size_t &count = (size_t)-1)
    {
        for (size_t i = 0; i < times; i++)
        {
            if (count == (size_t)-1)
                str.append(s);
            else
                str.append(s, count);
        }
        return *this;
    }
    inline PivString &AddManyText(const std::basic_string<CharT> &s, const size_t &times)
    {
        return AddManyText(s.c_str(), times);
    }
    inline PivString &AddManyText(const piv::basic_string_view<CharT> &s, const size_t &times)
    {
        return AddManyText(s.data(), times, s.size());
    }
    inline PivString &AddManyText(const CVolMem &s, const size_t &times)
    {
        return AddManyText(reinterpret_cast<const CharT *>(s.GetPtr()), times, static_cast<size_t>(s.GetSize()) / sizeof(CharT));
    }
    inline PivString &AddManyText(const CVolString &s, const size_t &times)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return AddManyText(reinterpret_cast<const CharT *>(s.GetText()), times);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return AddManyText(reinterpret_cast<const CharT *>(PivW2U{s}.GetText()), times);
        }
        else
        {
            return AddManyText(reinterpret_cast<const CharT *>(PivW2A{s}.GetText()), times);
        }
    }

    /**
     * @brief 加入小写文本
     * @param s 所欲加入并转换为小写的文本
     * @return 返回自身
     */
    inline PivString &AddLowerText(const piv::basic_string_view<CharT> &s)
    {
        std::basic_string<CharT> lower;
        lower.resize(s.size());
        std::transform(s.begin(), s.end(), lower.begin(), (int (*)(int))std::tolower);
        str.append(lower);
        return *this;
    }
    inline PivString &AddLowerText(const std::basic_string<CharT> &s)
    {
        return AddLowerText(piv::basic_string_view<CharT>{s});
    }
    inline PivString &AddLowerText(const CharT *s, const size_t &count = (size_t)-1)
    {
        if (count == (size_t)-1)
            return AddLowerText(piv::basic_string_view<CharT>{s});
        else
            return AddLowerText(piv::basic_string_view<CharT>{s, count});
    }
    inline PivString &AddLowerText(const CVolMem &s)
    {
        return AddLowerText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT)});
    }
    inline PivString &AddLowerText(const CVolString &s)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return AddLowerText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetText())});
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return AddLowerText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{s}.GetText())});
        }
        else
        {
            return AddLowerText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{s}.GetText())});
        }
    }

    /**
     * @brief 加入大写文本
     * @param s 所欲加入并转换为大写的文本
     * @return 返回自身
     */
    inline PivString &AddUpperText(const piv::basic_string_view<CharT> &s)
    {
        std::basic_string<CharT> lower;
        lower.resize(s.size());
        std::transform(s.begin(), s.end(), lower.begin(), (int (*)(int))std::toupper);
        str.append(lower);
        return *this;
    }
    inline PivString &AddUpperText(const std::basic_string<CharT> &s)
    {
        return AddUpperText(piv::basic_string_view<CharT>{s});
    }
    inline PivString &AddUpperText(const CharT *s, const size_t &count = (size_t)-1)
    {
        if (count == (size_t)-1)
            return AddUpperText(piv::basic_string_view<CharT>{s});
        else
            return AddUpperText(piv::basic_string_view<CharT>{s, count});
    }
    inline PivString &AddUpperText(const CVolMem &s)
    {
        return AddUpperText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT)});
    }
    inline PivString &AddUpperText(const CVolString &s)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return AddUpperText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetText())});
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return AddUpperText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{s}.GetText())});
        }
        else
        {
            return AddUpperText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{s}.GetText())});
        }
    }

    /**
     * @brief 加入格式文本
     * @tparam ...Ts 扩展参数
     * @param format 格式
     * @param ...args 扩展参数
     * @return
     */
    template <typename... Ts>
    PivString &AddFormatText(const CharT *format, const Ts... args)
    {
        str.append(piv::edit::format(format, args...));
        return *this;
    }
    template <typename... Ts>
    PivString &AddFormatText(const std::basic_string<CharT> &format, const Ts... args)
    {
        str.append(piv::edit::format(format.c_str(), args...));
        return *this;
    }
    template <typename... Ts>
    PivString &AddFormatText(const piv::basic_string_view<CharT> &format, const Ts... args)
    {
        str.append(piv::edit::format(std::basic_string<CharT>{format.data(), format.size()}.c_str(), args...));
        return *this;
    }
    template <typename... Ts>
    PivString &AddFormatText(const CVolString &format, const Ts... args)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            str.append(piv::edit::format(reinterpret_cast<const CharT *>(format.GetText()), args...));
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            str.append(piv::edit::format(reinterpret_cast<const CharT *>(PivW2U{format}.GetText()), args...));
        }
        else
        {
            str.append(piv::edit::format(reinterpret_cast<const CharT *>(PivW2A{format}.GetText()), args...));
        }
        return *this;
    }
    template <typename... Ts>
    PivString &AddFormatText(const CVolMem &format, const Ts... args)
    {
        str.append(piv::edit::format(reinterpret_cast<const CharT *>(format.GetPtr()), args...));
        return *this;
    }

    /**
     * @brief 插入文本
     * @param index 所欲插入的索引位置
     * @param s 所欲插入的文本
     * @param ch 所欲插入的字符
     * @param count 所欲插入的文本长度
     * @return
     */
    inline PivString &InsertText(const size_t &index, const CharT &ch)
    {
        ASSERT(index < str.size());
        str.insert(index, 1, ch);
        return *this;
    }
    inline PivString &InsertText(const size_t &index, const CharT *s, const size_t &count = (size_t)-1)
    {
        ASSERT(index < str.size());
        if (count == (size_t)-1)
            str.insert(index, s);
        else
            str.insert(index, s, count);
        return *this;
    }
    inline PivString &InsertText(const size_t &index, const std::basic_string<CharT> &s, const size_t &count = (size_t)-1)
    {
        ASSERT(index < str.size());
        if (count == (size_t)-1)
            str.insert(index, s);
        else
            str.insert(index, s, 0, count);
        return *this;
    }
    inline PivString &InsertText(const size_t &index, const piv::basic_string_view<CharT> &s, const size_t &count = (size_t)-1)
    {
        return InsertText(index, s.data(), s.size());
    }
    inline PivString &InsertText(const size_t &index, const CVolMem &s, const size_t &count = (size_t)-1)
    {
        return InsertText(index, reinterpret_cast<const CharT *>(s.GetPtr()),
                          (count == (size_t)-1) ? s.GetSize() / sizeof(CharT) : count);
    }
    inline PivString &InsertText(const size_t &index, const CVolString &s, const size_t &count = (size_t)-1)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return InsertText(index, reinterpret_cast<const CharT *>(s.GetText()), (count == (size_t)-1) ? s.GetLength() : count);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return InsertText(index, reinterpret_cast<const CharT *>(PivW2U{s}.GetText()), count);
        }
        else
        {
            return InsertText(index, reinterpret_cast<const CharT *>(PivW2A{s}.GetText()), count);
        }
    }

    /**
     * @brief 插入行首空格
     * @param count 所欲插入的空格数
     * @return
     */
    inline PivString &InsertLineBeginLeaderSpaces(const int32_t &count)
    {
        ASSERT(count >= 0);
        if (str.empty())
            return *this;
        size_t fpos = 0, pos = 0;
        str.insert(0, static_cast<size_t>(count), ' ');
        while (fpos < str.size())
        {
            fpos = str.find_first_of('\n', pos);
            if (fpos == std::basic_string<CharT>::npos)
                break;
            str.insert(fpos + 1, static_cast<size_t>(count), ' ');
            pos = fpos + static_cast<size_t>(count);
        }
        return *this;
    }

    /**
     * @brief 删除字符
     * @param pos 首字符索引位置
     * @param count 欲删除字符数目
     * @return
     */
    inline PivString &RemoveChar(const size_t &pos, const size_t &count)
    {
        ASSERT(pos >= 0 && pos < str.size() && count >= 0);
        str.erase(pos, count);
        return *this;
    }

    /**
     * @brief 删除部分文本
     * @param pos 首字符索引位置
     * @param count 欲删除字符数目
     * @return
     */
    inline size_t &RemoveChars(const size_t &pos, const size_t &count)
    {
        ASSERT(pos >= 0 && pos < str.size() && count >= 0);
        size_t ret = str.size();
        str.erase(pos, count);
        return ret - str.size();
    }

    /**
     * @brief 删除文字
     * @param pos 首文字索引位置
     * @param count 欲删除文字数目
     * @return
     */
    inline PivString &RemoveWord(const size_t &pos, const size_t &count)
    {
        piv::edit::remove_words<CharT, EncodeType>(str, pos, count);
        return *this;
    }

    /**
     * @brief 取字符
     */
    inline const CharT &At(const size_t &pos) const
    {
        // 判断索引是否有效和文本是否为空
        ASSERT(pos >= 0 && pos < str.size() && str.empty() == false);
        return str[pos];
    }

    /**
     * @brief 取首字符
     */
    inline const CharT &Front() const
    {
        return str.empty() ? 0 : str.front();
    }

    /**
     * @brief 取尾字符
     */
    inline const CharT &Back() const
    {
        return str.empty() ? 0 : str.back();
    }

    /**
     * @brief 取文本中间
     * @param pos 起始取出索引位置
     * @param count 欲取出字符的数目
     * @return 返回文本类
     */
    inline PivString Middle(const size_t &pos, const size_t &count) const
    {
        ASSERT(pos <= str.size()); // 判断索引位置是否有效
        return PivString{str.substr(pos, count)};
    }

    /**
     * @brief 取文本左边
     * @param count 欲取出字符的数目
     * @return 返回文本类
     */
    inline PivString Left(const size_t &count) const
    {
        return PivString{str.substr(0, count)};
    }

    /**
     * @brief 取文本右边
     * @param count 欲取出字符的数目
     * @return 返回文本类
     */
    inline PivString Right(const size_t &count) const
    {
        size_t pos = count > str.size() ? 0 : str.size() - count;
        return PivString{str.substr(pos, count)};
    }

    /**
     * @brief 取左边文字
     * @param count 欲取出文字的数目
     * @return 返回文本视图类
     */
    inline PivString LeftWords(const size_t &count) const
    {
        return PivString{piv::edit::substr<CharT, EncodeType>(str, 0, count)};
    }

    /**
     * @brief 取中间文字
     * @param pos 起始取出索引位置
     * @param count 欲取出文字的数目
     * @return 返回文本视图类
     */
    inline PivString MiddleWords(const size_t &pos, const size_t &count) const
    {
        return PivString{piv::edit::substr<CharT, EncodeType>(str, pos, count)};
    }

    /**
     * @brief 取右边文字
     * @param count 欲取出文字的数目
     * @return 返回文本视图类
     */
    inline PivString RightWords(const size_t &count) const
    {
        size_t length = piv::edit::get_word_length<CharT, EncodeType>(str);
        size_t pos = (count >= length) ? 0 : (length - count);
        return PivString{piv::edit::substr<CharT, EncodeType>(str, pos, count)};
    }

    /**
     * @brief 取子文本中间
     * @param lstr 开始文本
     * @param rstr 结束文本
     * @param pos 起始搜寻位置
     * @param case_insensitive 是否不区分大小写
     * @param include_lstr 是否包含开始文本
     * @param include_rstr 是否包含结束文本
     * @param retorg_failed 失败返回原文本
     * @return
     */
    template <typename L, typename R>
    inline PivString GetMiddle(const L &lstr, const R &rstr, const size_t &pos, const bool &case_insensitive = false, const bool &include_lstr = false, const bool &include_rstr = false, const bool &retorg_failed = false) const
    {
        size_t strlen = 0;
        size_t spos = SearchText(lstr, pos, case_insensitive, &strlen);
        if (spos == -1)
            return retorg_failed ? PivString{str} : PivString{};
        size_t epos = spos + strlen;
        if (!include_lstr)
            spos = epos;
        epos = SearchText(rstr, epos, case_insensitive, &strlen);
        if (epos == -1)
            return retorg_failed ? PivString{str} : PivString{};
        return PivString{str.substr(spos, epos - spos + (include_rstr ? strlen : 0))};
    }
    template <typename L, typename R>
    inline size_t GetMiddle(std::vector<std::basic_string<CharT>> &array, const L &lstr, const R &rstr, const size_t &pos, const bool &case_insensitive = false, const bool &include_lstr = false, const bool &include_rstr = false) const
    {
        array.clear();
        size_t strlen = 0, spos = pos, epos = 0;
        while (true)
        {
            spos = SearchText(lstr, spos, case_insensitive, &strlen);
            if (spos == -1)
                break;
            epos = spos + strlen;
            if (!include_lstr)
                spos = epos;
            epos = SearchText(rstr, epos, case_insensitive, &strlen);
            if (epos == -1)
                break;
            array.push_back(sv.substr(spos, epos - spos + (include_rstr ? strlen : 0)));
        }
        return array.size();
    }

    /**
     * @brief 取子文本左边
     * @param lstr 要查找的文本
     * @param pos 起始搜寻位置
     * @param case_insensitive 是否不区分大小写
     * @param include_lstr 是否包含查找的文本
     * @param reverse 是否反向查找
     * @param retorg_failed 失败返回原文本
     * @return
     */
    template <typename L>
    inline PivString GetLeft(const L &lstr, const size_t &pos, const bool &case_insensitive = false, const bool &include_lstr = false, const bool &reverse = false, const bool &retorg_failed = false) const
    {
        size_t spos, strlen = 0;
        if (reverse)
            spos = ReverseSearchText(lstr, pos, case_insensitive, &strlen);
        else
            spos = SearchText(lstr, pos, case_insensitive, &strlen);
        if (spos == -1)
            return retorg_failed ? PivString{str} : PivString{};
        return PivString{str.substr(0, spos + (include_lstr ? strlen : 0))};
    }

    /**
     * @brief 取子文本右边
     * @param rstr 要查找的文本
     * @param pos 起始搜寻位置
     * @param case_insensitive 是否不区分大小写
     * @param include_rstr 是否包含查找的文本
     * @param reverse 是否反向查找
     * @param retorg_failed 失败返回原文本
     * @return
     */
    template <typename R>
    inline PivString GetRight(const R &rstr, const size_t &pos, const bool &case_insensitive = false, const bool &include_rstr = false, const bool &reverse = false, const bool &retorg_failed = false) const
    {
        size_t spos, strlen = 0;
        if (reverse)
            spos = ReverseSearchText(rstr, pos, case_insensitive, &strlen);
        else
            spos = SearchText(rstr, pos, case_insensitive, &strlen);
        if (spos == -1)
            return retorg_failed ? PivString{str} : PivString{};
        if (!include_rstr)
            spos += strlen;
        return PivString{str.substr(spos, str.size() - spos)};
    }

    /**
     * @brief 寻找字符
     * @param character 欲寻找的字符
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t FindChar(const CharT &character, const size_t &off = 0) const
    {
        if (off >= str.size())
            return std::basic_string<CharT>::npos;
        return str.find(character, off);
    }

    /**
     * @brief 倒找字符
     * @param character 欲寻找的字符
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t ReverseFindChar(const CharT &character, const size_t &off = (size_t)-1) const
    {
        if (off >= str.size())
            return std::basic_string<CharT>::npos;
        return str.rfind(character, (off == (size_t)-1) ? str.size() : off);
    }

    /**
     * @brief 寻找字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t FindFirstOf(const std::basic_string<CharT> &chars, const size_t &pos = 0) const
    {
        return str.find_first_of(chars, pos);
    }
    inline size_t FindFirstOf(const CharT *chars, const size_t &pos = 0, const size_t &count = (size_t)-1) const
    {
        return (count == (size_t)-1) ? str.find_first_of(chars, pos)
                                     : str.find_first_of(chars, pos, count);
    }
    inline size_t FindFirstOf(const CVolMem &chars, const size_t &pos = 0) const
    {
        return FindFirstOf(reinterpret_cast<const CharT *>(chars.GetPtr()), pos, chars.GetSize() / sizeof(CharT));
    }
    inline size_t FindFirstOf(const piv::basic_string_view<CharT> &chars, const size_t &pos = 0) const
    {
        return FindFirstOf(chars.data(), pos, chars.size());
    }
    inline size_t FindFirstOf(const CVolString &chars, const size_t &pos = 0) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return FindFirstOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return FindFirstOf(reinterpret_cast<const CharT *>(PivW2U{chars}.GetText()), pos);
        }
        else
        {
            return FindFirstOf(reinterpret_cast<const CharT *>(PivW2A{chars}.GetText()), pos);
        }
    }

    /**
     * @brief 倒找字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t FindLastOf(const std::basic_string<CharT> &chars, const size_t &pos = (size_t)-1) const
    {
        return str.find_last_of(chars, (pos == (size_t)-1) ? str.size() : pos);
    }
    inline size_t FindLastOf(const CharT *chars, const size_t &pos = (size_t)-1, const size_t &count = (size_t)-1) const
    {
        return (count == (size_t)-1) ? str.find_last_of(chars, (pos == (size_t)-1) ? str.size() : pos)
                                     : str.find_last_of(chars, (pos == (size_t)-1) ? str.size() : pos, count);
    }
    inline size_t FindLastOf(const CVolMem &chars, const size_t &pos = (size_t)-1) const
    {
        return FindLastOf(reinterpret_cast<const CharT *>(chars.GetPtr()), pos, chars.GetSize() / sizeof(CharT));
    }
    inline size_t FindLastOf(const piv::basic_string_view<CharT> &chars, const size_t &pos = (size_t)-1) const
    {
        return FindLastOf(chars.data(), pos, chars.size());
    }
    inline size_t FindLastOf(const CVolString &chars, const size_t &pos = (size_t)-1) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return FindLastOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return FindLastOf(reinterpret_cast<const CharT *>(PivW2U{chars}.GetText()), pos);
        }
        else
        {
            return FindLastOf(reinterpret_cast<const CharT *>(PivW2A{chars}.GetText()), pos);
        }
    }

    /**
     * @brief 寻找不符合字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t FindFirstNotOf(const std::basic_string<CharT> &chars, const size_t &pos = 0) const
    {
        return str.find_first_not_of(chars, pos);
    }
    inline size_t FindFirstNotOf(const CharT *chars, const size_t &pos = 0, const size_t &count = (size_t)-1) const
    {
        return (count == (size_t)-1) ? str.find_first_not_of(chars, pos)
                                     : str.find_first_not_of(chars, pos, count);
    }
    inline size_t FindFirstNotOf(const CVolMem &chars, const size_t &pos = 0) const
    {
        return FindFirstNotOf(reinterpret_cast<const CharT *>(chars.GetPtr()), pos, chars.GetSize() / sizeof(CharT));
    }
    inline size_t FindFirstNotOf(const piv::basic_string_view<CharT> &chars, const size_t &pos = 0)
    {
        return FindFirstNotOf(chars.data(), pos, chars.size());
    }
    inline size_t FindFirstNotOf(const CVolString &chars, const size_t &pos = 0) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return FindFirstNotOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return FindFirstNotOf(reinterpret_cast<const CharT *>(PivW2U{chars}.GetText()), pos);
        }
        else
        {
            return FindFirstNotOf(reinterpret_cast<const CharT *>(PivW2A{chars}.GetText()), pos);
        }
    }

    /**
     * @brief 倒找字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t FindLastNotOf(const std::basic_string<CharT> &chars, const size_t &pos = (size_t)-1) const
    {
        return str.find_last_not_of(chars, (pos == (size_t)-1) ? str.size() : pos);
    }
    inline size_t FindLastNotOf(const CharT *chars, const size_t &pos = (size_t)-1, const size_t &count = (size_t)-1) const
    {
        return (count == (size_t)-1) ? str.find_last_not_of(chars, (pos == (size_t)-1) ? str.size() : pos)
                                     : str.find_last_not_of(chars, (pos == (size_t)-1) ? str.size() : pos, count);
    }
    inline size_t FindLastNotOf(const CVolMem &chars, const size_t &pos = (size_t)-1) const
    {
        return FindLastNotOf(reinterpret_cast<const CharT *>(chars.GetPtr()), pos, chars.GetSize() / sizeof(CharT));
    }
    inline size_t FindLastNotOf(const piv::basic_string_view<CharT> &chars, const size_t &pos = (size_t)-1) const
    {
        return FindLastNotOf(chars.data(), pos, chars.size());
    }
    inline size_t FindLastNotOf(const CVolString &chars, const size_t &pos = (size_t)-1) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return FindLastNotOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return FindLastNotOf(reinterpret_cast<const CharT *>(PivW2U{chars}.GetText()), pos);
        }
        else
        {
            return FindLastNotOf(reinterpret_cast<const CharT *>(PivW2A{chars}.GetText()), pos);
        }
    }

    /**
     * @brief 寻找文本
     * @param text 欲寻找的文本
     * @param pos 起始搜寻位置
     * @param case_insensitive 是否不区分大小写
     * @param count 欲寻找文本的长度
     * @param textlen 返回文本长度
     * @return 寻找到的位置
     */
    inline size_t SearchText(const piv::basic_string_view<CharT> &text, const size_t &pos = 0, const bool &case_insensitive = false, size_t *textlen = nullptr) const
    {
        if (textlen != nullptr)
            *textlen = text.size();
        if (!case_insensitive)
            return str.find(text.data(), pos, text.size());
        else
            return piv::edit::ifind(piv::basic_string_view<CharT>{str}, piv::basic_string_view<CharT>{text}, pos);
    }
    inline size_t SearchText(const std::basic_string<CharT> &text, const size_t &pos = 0, const bool &case_insensitive = false, size_t *textlen = nullptr) const
    {
        if (textlen != nullptr)
            *textlen = text.size();
        if (!case_insensitive)
            return str.find(text, pos);
        else
            return piv::edit::ifind(piv::basic_string_view<CharT>{str}, piv::basic_string_view<CharT>{text}, pos);
    }
    inline size_t SearchText(const CharT *text, const size_t &pos = 0, const bool &case_insensitive = false, const size_t &count = (size_t)-1, size_t *textlen = nullptr) const
    {
        return SearchText((count == (size_t)-1) ? piv::basic_string_view<CharT>{text} : piv::basic_string_view<CharT>{text, count}, pos, case_insensitive, textlen);
    }
    inline size_t SearchText(const CVolMem &text, const size_t &pos = 0, const bool &case_insensitive = false, size_t *textlen = nullptr) const
    {
        return SearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetPtr()), static_cast<size_t>(text.GetSize()) / sizeof(CharT)}, pos, case_insensitive, textlen);
    }
    inline size_t SearchText(const CVolString &text, const size_t &pos = 0, const bool &case_insensitive = false, size_t *textlen = nullptr) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return SearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetText())}, pos, case_insensitive, textlen);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return SearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{text}.GetText())}, pos, case_insensitive, textlen);
        }
        else
        {
            return SearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{text}.GetText())}, pos, case_insensitive, textlen);
        }
    }

    /**
     * @brief 倒找文本
     * @param text 欲寻找的文本
     * @param pos 起始搜寻位置
     * @param count 欲寻找文本的长度
     * @param case_insensitive 是否不区分大小写
     * @param textlen 返回文本长度
     * @return 寻找到的位置
     */
    inline size_t ReverseSearchText(const piv::basic_string_view<CharT> &text, const size_t &pos = (size_t)-1, const bool &case_insensitive = false, size_t *textlen = nullptr) const
    {
        if (textlen != nullptr)
            *textlen = text.size();
        if (!case_insensitive)
            return str.rfind(text.data(), (pos == (size_t)-1) ? str.size() : pos, text.size());
        else
            return piv::edit::irfind(piv::basic_string_view<CharT>{str}, text, (pos == (size_t)-1) ? str.size() : pos);
    }
    inline size_t ReverseSearchText(const std::basic_string<CharT> &text, const size_t &pos = (size_t)-1, const bool &case_insensitive = false, size_t *textlen = nullptr) const
    {
        return ReverseSearchText(piv::basic_string_view<CharT>{text}, pos, case_insensitive, textlen);
    }
    inline size_t ReverseSearchText(const CharT *text, const size_t &pos = (size_t)-1, const bool &case_insensitive = false, const size_t &count = (size_t)-1, size_t *textlen = nullptr) const
    {
        return ReverseSearchText((count == -1) ? piv::basic_string_view<CharT>{text} : piv::basic_string_view<CharT>{text, static_cast<size_t>(count)}, pos, case_insensitive, textlen);
    }
    inline size_t ReverseSearchText(const CVolMem &text, const size_t &pos = (size_t)-1, const bool &case_insensitive = false, size_t *textlen = nullptr) const
    {
        return ReverseSearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetPtr()), static_cast<size_t>(text.GetSize()) / sizeof(CharT)}, pos, case_insensitive, textlen);
    }
    inline size_t ReverseSearchText(const CVolString &text, const size_t &pos = (size_t)-1, const bool &case_insensitive = false, size_t *textlen = nullptr) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return ReverseSearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetText())}, pos, case_insensitive, textlen);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return ReverseSearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{text}.GetText())}, pos, case_insensitive, textlen);
        }
        else
        {
            return ReverseSearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{text}.GetText())}, pos, case_insensitive, textlen);
        }
    }

    /**
     * @brief 是否以字符/文本开头
     * @param s 所欲检查的文本
     * @param ch 所欲检查的字符
     * @param count 所欲检查文本的长度
     * @param case_sensitive 是否区分大小写
     * @return
     */
    inline bool LeadOf(const CharT &ch, const bool &case_sensitive = true) const
    {
        if (str.empty())
            return false;
        if (case_sensitive)
            return (str.front() == ch);
        else
            return (std::tolower(static_cast<unsigned char>(str.front())) == std::tolower(static_cast<unsigned char>(ch)));
    }
    inline bool LeadOf(const CharT *s, const bool &case_sensitive = true, const size_t &count = (size_t)-1) const
    {
        return (SearchText(s, 0, !case_sensitive, count) == 0);
    }
    inline bool LeadOf(const std::basic_string<CharT> &s, const bool &case_sensitive = true) const
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }
    inline bool LeadOf(const CVolMem &s, const bool &case_sensitive = true) const
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }
    inline bool LeadOf(const piv::basic_string_view<CharT> &s, const bool &case_sensitive = true) const
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }
    inline bool LeadOf(const CVolString &s, const bool &case_sensitive = true) const
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }

    /**
     * @brief 是否以字符/文本结束
     * @param s 所欲检查的文本
     * @param ch 所欲检查的字符
     * @param count 所欲检查文本的长度
     * @param case_sensitive 是否区分大小写
     * @return
     */
    inline bool EndOf(const CharT &ch, const bool &case_sensitive = true) const
    {
        if (str.empty())
            return false;
        if (case_sensitive)
            return (str.back() == ch);
        else
            return (std::tolower(static_cast<unsigned char>(str.back())) == std::tolower(static_cast<unsigned char>(ch)));
    }
    inline bool EndOf(const piv::basic_string_view<CharT> &s, const bool &case_sensitive = true) const
    {
        return (ReverseSearchText(s, -1, !case_sensitive) == (str.size() - s.size()));
    }
    inline bool EndOf(const CharT *s, const bool &case_sensitive = true, const size_t &count = (size_t)-1)
    {
        return EndOf((count == -1) ? piv::basic_string_view<CharT>{s} : piv::basic_string_view<CharT>{s, static_cast<size_t>(count)}, case_sensitive);
    }
    inline bool EndOf(const std::basic_string<CharT> &s, const bool &case_sensitive = true) const
    {
        return EndOf(piv::basic_string_view<CharT>{s}, case_sensitive);
    }
    inline bool EndOf(const CVolMem &s, const bool &case_sensitive = true) const
    {
        return EndOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT)}, case_sensitive);
    }
    inline bool EndOf(const CVolString &s, const bool &case_sensitive = true) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return EndOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetText())}, case_sensitive);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return EndOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{text}.GetText())}, case_sensitive);
        }
        else
        {
            return EndOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{text}.GetText())}, case_sensitive);
        }
    }

    /**
     * @brief 替换字符
     * @param BeginCharIndex 起始替换索引位置
     * @param Find 所欲被替换的字符
     * @param Replace 所欲替换到的字符
     * @return 返回是否产生了实际替换
     */
    bool Replace(const size_t &pos, const size_t &findCh, const CharT &replaceCH)
    {
        ASSERT(pos >= 0 && pos <= str.length());
        bool replaced = false;
        for (size_t i = pos; i < str.size(); i++)
        {
            if (str[i] == findCh)
            {
                str[i] = replaceCH;
                replaced = true;
            }
        }
        return replaced;
    }

    /**
     * @brief 文本替换
     * @param pos 起始替换索引位置
     * @param count 替换长度
     * @param s 用作替换的文本
     * @return
     */
    inline PivString &Replace(const size_t &pos, const size_t &count, const CharT *s, const size_t &count2 = (size_t)-1)
    {
        ASSERT(pos >= 0 && pos <= str.length() && count >= 0);
        if (count2 == (size_t)-1)
            str.replace(pos, count, s);
        else
            str.replace(pos, count, s, count2);
        return *this;
    }
    inline PivString &Replace(const size_t &pos, const size_t &count, const std::basic_string<CharT> &s)
    {
        ASSERT(pos >= 0 && pos <= str.length() && count >= 0);
        str.replace(pos, count, s);
        return *this;
    }
    inline PivString &Replace(const size_t &pos, const size_t &count, const piv::basic_string_view<CharT> &s)
    {
        return Replace(pos, count, reinterpret_cast<const CharT *>(s.data()), s.size());
    }
    inline PivString &Replace(const size_t &pos, const size_t &count, const CVolMem &s)
    {
        return Replace(pos, count, reinterpret_cast<const CharT *>(s.GetPtr()), s.GetSize() * sizeof(CharT));
    }
    inline PivString &Replace(const size_t &pos, const size_t &count, const CVolString &s)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return Replace(pos, count, reinterpret_cast<const CharT *>(text.GetText()), -1);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return Replace(pos, count, reinterpret_cast<const CharT *>(PivW2U{text}.GetText()), -1);
        }
        else
        {
            return Replace(pos, count, reinterpret_cast<const CharT *>(PivW2A{text}.GetText()), -1);
        }
    }

    /**
     * @brief 子文本替换
     * @param findStr 欲被替换的子文本
     * @param repStr 用作替换的文本
     * @param pos 起始替换索引位置
     * @param count 替换次数
     * @param case_sensitive 是否区分大小写
     * @return
     */
    PivString &ReplaceSubText(const piv::basic_string_view<CharT> &findStr, const piv::basic_string_view<CharT> &repStr, const size_t &pos = 0, const size_t &count = (size_t)-1, const bool &case_sensitive = true)
    {
        ASSERT(pos >= 0 && pos <= str.length());
        size_t fpos = 0, opos = pos;
        size_t i = 0;
        while (fpos < str.length())
        {
            if (case_sensitive)
                fpos = str.find(findStr.data(), opos, findStr.size());
            else
                fpos = piv::edit::ifind(piv::basic_string_view<CharT>{str}, findStr, opos);
            if (fpos == std::basic_string<CharT>::npos)
                break;
            str.replace(fpos, findStr.size(), repStr.data(), repStr.size());
            opos = fpos + repStr.size();
            i++;
            if (count > 0 && i >= count)
                break;
        }
        return *this;
    }
    inline PivString &ReplaceSubText(const CharT *findStr, const CharT *repStr, const size_t &pos = 0, const size_t &count = (size_t)-1, const bool &case_sensitive = true, const size_t &findLen = (size_t)-1, const size_t &repLen = (size_t)-1)
    {
        ASSERT(pos >= 0 && pos <= str.length());
        if (findLen == (size_t)-1)
            findLen = (sizeof(CharT) == 2) ? wcslen(reinterpret_cast<const wchar_t *>(findStr)) : strlen(reinterpret_cast<const char *>(findStr));
        if (repLen == (size_t)-1)
            repLen = (sizeof(CharT) == 2) ? wcslen(reinterpret_cast<const wchar_t *>(repStr)) : strlen(reinterpret_cast<const char *>(repStr));
        return ReplaceSubText(piv::basic_string_view<CharT>{findStr, findLen}, piv::basic_string_view<CharT>{repStr, repLen}, pos, count, case_sensitive);
    }
    inline PivString &ReplaceSubText(const std::basic_string<CharT> &findStr, const std::basic_string<CharT> &repStr, const size_t &pos = 0, const size_t &count = (size_t)-1, const bool &case_sensitive = true)
    {
        return ReplaceSubText(piv::basic_string_view<CharT>{findStr.c_str()}, piv::basic_string_view<CharT>{repStr.c_str()}, pos, count, case_sensitive);
    }
    inline PivString &ReplaceSubText(const CVolMem &findStr, const CVolMem &repStr, const size_t &pos = 0, const size_t &count = (size_t)-1, const bool &case_sensitive = true)
    {
        return ReplaceSubText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(findStr.GetPtr()), static_cast<size_t>(findStr.GetSize()) / sizeof(CharT)},
                              piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(repStr.GetPtr()), static_cast<size_t>(repStr.GetSize()) / sizeof(CharT)}, pos, count, case_sensitive);
    }
    PivString &ReplaceSubText(const CVolString &findStr, const CVolString &repStr, const size_t &pos = 0, const size_t &count = (size_t)-1, const bool &case_sensitive = true)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return ReplaceSubText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(findStr.GetText())},
                                  piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(repStr.GetText())}, pos, count, case_sensitive);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            PivW2U find_utf8{findStr};
            PivW2U rep_utf8{repStr};
            return ReplaceSubText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(find_utf8.GetText())},
                                  piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(rep_utf8.GetText())}, pos, count, case_sensitive);
        }
        else
        {
            PivW2A find_mbcs{findStr};
            PivW2A rep_mbcs{repStr};
            return ReplaceSubText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(find_mbcs.GetText())},
                                  piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(rep_mbcs.GetText())}, pos, count, case_sensitive);
        }
    }

    /**
     * @brief 替换控制符
     */
    PivString &ReplaceAllControlCharsToSpace()
    {
        if (str.empty())
            return *this;
        for (size_t i = 0; i < str.size(); ++i)
        {
            if (std::iscntrl(static_cast<unsigned char>(str[i])))
                str[i] = ' ';
        }
        return *this;
    }

    /**
     * @brief 检查加入换行文本
     * @return
     */
    inline PivString &CheckAddCRLF()
    {
        if (str.empty())
        {
            str.push_back('\r');
            str.push_back('\n');
            return *this;
        }
        if (str.back() != '\n')
        {
            if (str.back() != '\r')
            {
                str.push_back('\r');
            }
            str.push_back('\n');
        }
        return *this;
    }

    /**
     * @brief 删除空白行
     * @return
     */
    PivString &RemoveAllSpaceLines()
    {
        if (str.empty())
            return *this;
        std::basic_string<CharT> ret;
        ret.reserve(str.size());
        size_t spos = 0, fpos = 0, epos = str.size();
        piv::basic_string_view<CharT> sv = str;
        piv::basic_string_view<CharT> tmp;
        while (fpos < epos)
        {
            fpos = sv.find('\n', spos);
            if (fpos != std::basic_string<CharT>::npos)
            {
                tmp = sv.substr(spos, fpos - spos + 1);
                spos = fpos + 1;
            }
            else
            {
                tmp = sv.substr(spos, sv.size() - spos);
                fpos = epos;
            }
            if (tmp.empty())
                continue;
            bool isSpace = true;
            for (auto it = tmp.begin(); it != tmp.end(); it++)
            {
                if (*it > ' ')
                {
                    isSpace = false;
                    break;
                }
            }
            if (isSpace == false)
            {
                ret.append(tmp.data(), tmp.size());
            }
        }
        str.swap(ret);
        return *this;
    }

    /**
     * @brief 检查加入路径字符
     * @return
     */
    inline PivString &CheckAddPathChar()
    {
        if (str.empty())
        {
            str.push_back('\\');
            return *this;
        }
        if (str.back() != '\\')
            str.push_back('\\');
        return *this;
    }

    /**
     * @brief 检查删除路径字符
     * @return
     */
    PivString &RemoveEndPathChar()
    {
        if (str.empty())
        {
            return *this;
        }
        while (str.back() == '\\')
        {
            if (str.size() == 3 && str[1] == ':')
            {
                break;
            }
            str.pop_back();
        }
        return *this;
    }

    /**
     * @brief 删首空
     */
    inline PivString &TrimLeft()
    {
        piv::edit::trim_left(str);
        return *this;
    }

    /**
     * @brief 删尾空
     */
    inline PivString &TrimRight()
    {
        piv::edit::trim_right(str);
        return *this;
    }

    /**
     * @brief 删首尾空
     */
    inline PivString &TrimAll()
    {
        piv::edit::trim_left(str);
        piv::edit::trim_right(str);
        return *this;
    }

    /**
     * @brief 删全部空
     * @param reserveLFCR 是否保留换行符
     * @return
     */
    inline PivString &TrimAllSpace(const bool &reserveLFCR)
    {
        unsigned char ch;
        for (size_t i = 0; i < str.size();)
        {
            ch = static_cast<unsigned char>(str[i]);
            if ((ch >= 0 && ch <= 0x09) || (ch >= 0x0B && ch <= 0x0C) || (!reserveLFCR && (ch == '\r' || ch == '\n')) || (ch >= 0x0E && ch <= ' '))
                str.erase(i, 1);
            else
                i++;
        }
        return *this;
    }

    /**
     * @brief 删首字符
     */
    inline PivString &RemoveFront()
    {
        if (!str.empty())
            str.erase(0, 1);
        return *this;
    }

    /**
     * @brief 删尾字符
     */
    inline PivString &RemoveBack()
    {
        if (!str.empty())
            str.pop_back();
        return *this;
    }

    /**
     * @brief 分割文本
     * @param delimit 分割字符集合
     * @param strArray 结果存放数组
     * @param trimAll 是否删除首尾空
     * @param ignoreEmptyStr 是否忽略空白结果
     * @return 所分割出来的结果文本数目
     */
    size_t SplitStrings(const piv::basic_string_view<CharT> &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = static_cast<size_t>(-1)) const
    {
        strArray.clear();
        if (delimit.empty())
        {
            strArray.push_back(str);
        }
        else
        {
            size_t spos = 0, fpos = 0, epos = str.size();
            std::basic_string<CharT> s;
            while (fpos < epos)
            {
                fpos = str.find_first_of(delimit.data(), spos, delimit.size());
                if (fpos != std::basic_string<CharT>::npos)
                {
                    s = str.substr(spos, fpos - spos);
                    spos = fpos + 1;
                }
                else
                {
                    s = str.substr(spos, str.size() - spos);
                    fpos = epos;
                }
                if (trimAll)
                {
                    piv::edit::trim_left(s);
                    piv::edit::trim_right(s);
                    if (ignoreEmptyStr && s.empty())
                        continue;
                }
                else if (ignoreEmptyStr)
                {
                    if (s.empty())
                        continue;
                    bool isEmpty = true;
                    for (auto it = s.begin(); it != s.end(); it++)
                    {
                        if (*it > ' ')
                        {
                            isEmpty = false;
                            break;
                        }
                    }
                    if (isEmpty)
                        continue;
                }
                strArray.push_back(s);
            }
        }
        return strArray.size();
    }
    inline size_t SplitStrings(const CharT *delimit, std::vector<std::basic_string<CharT>> &strArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        return SplitStrings(piv::basic_string_view<CharT>{delimit}, strArray, trimAll, ignoreEmptyStr, max_count);
    }
    inline size_t SplitStrings(const std::basic_string<CharT> &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        return SplitStrings(piv::basic_string_view<CharT>{delimit}, strArray, trimAll, ignoreEmptyStr, max_count);
    }
    inline size_t SplitStrings(const CVolMem &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        return SplitStrings(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(delimit.GetPtr()), static_cast<size_t>(delimit.GetSize() / sizeof(CharT))}, svArray, trimAll, ignoreEmptyStr, max_count);
    }
    inline size_t SplitStrings(const CVolString &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return SplitStrings(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(delimit.GetText())}, strArray, trimAll, ignoreEmptyStr, max_count);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return SplitStrings(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{delimit}.GetText())}, strArray, trimAll, ignoreEmptyStr, max_count);
        }
        else
        {
            return SplitStrings(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{delimit}.GetText())}, strArray, trimAll, ignoreEmptyStr, max_count);
        }
    }

    /**
     * @brief 分割文本视图
     * @param delimit 分割字符集合
     * @param svArray 结果存放数组
     * @param trimAll 是否删除首尾空
     * @param ignoreEmptyStr 是否忽略空白结果
     * @return 所分割出来的结果文本数目
     */
    size_t SplitStringsView(const piv::basic_string_view<CharT> &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        svArray.clear();
        piv::basic_string_view<CharT> sv{str};
        if (delimit.empty())
        {
            svArray.push_back(sv);
        }
        else
        {
            size_t n = 0, spos = 0, fpos = 0, epos = sv.size();
            piv::basic_string_view<CharT> ss;
            while (fpos < epos)
            {
                fpos = sv.find_first_of(delimit, spos);
                if (fpos != piv::basic_string_view<CharT>::npos && ++n < max_count)
                {
                    ss = sv.substr(spos, fpos - spos);
                    spos = fpos + 1;
                }
                else
                {
                    ss = sv.substr(spos, sv.size() - spos);
                    fpos = epos;
                }
                if (trimAll)
                {
                    piv::edit::trim_left(ss);
                    piv::edit::trim_right(ss);
                    if (ignoreEmptyStr && ss.empty())
                        continue;
                }
                else if (ignoreEmptyStr)
                {
                    if (ss.empty())
                        continue;
                    bool isEmpty = true;
                    for (size_t i = 0; i < str.size(); i++)
                    {
                        // if (!std::isspace(static_cast<unsigned char>(ss[i])))
                        if (static_cast<unsigned char>(ss[i]) > ' ')
                        {
                            isEmpty = false;
                            break;
                        }
                    }
                    if (isEmpty)
                        continue;
                }
                svArray.push_back(ss);
            }
        }
        return svArray.size();
    }
    inline size_t SplitStringsView(const CharT *delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        return SplitStringsView(piv::basic_string_view<CharT>{delimit}, svArray, trimAll, ignoreEmptyStr, max_count);
    }
    inline size_t SplitStringsView(const CVolMem &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        return SplitStringsView(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(delimit.GetPtr()), static_cast<size_t>(delimit.GetSize()) / sizeof(CharT)}, svArray, trimAll, ignoreEmptyStr, max_count);
    }
    inline size_t SplitStringsView(const std::basic_string<CharT> &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        return SplitStringsView(piv::basic_string_view<CharT>{delimit}, svArray, trimAll, ignoreEmptyStr, max_count);
    }
    inline size_t SplitStringsView(const CVolString &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return SplitStringsView(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(delimit.GetText())}, svArray, trimAll, ignoreEmptyStr, max_count);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return SplitStringsView(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{delimit}.GetText())}, svArray, trimAll, ignoreEmptyStr, max_count);
        }
        else
        {
            return SplitStringsView(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{delimit}.GetText())}, svArray, trimAll, ignoreEmptyStr, max_count);
        }
    }

    /**
     * @brief 分割子文本
     * @param delimit 分割用子文本
     * @param strArray 结果存放数组
     * @param trimAll 是否删除首尾空
     * @param ignoreEmptyStr 是否忽略空白结果
     * @return 所分割出来的结果文本数目
     */
    size_t SplitSubStrings(const piv::basic_string_view<CharT> delimit, std::vector<std::basic_string<CharT>> &strArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        strArray.clear();
        if (delimit.empty())
        {
            strArray.push_back(str);
        }
        else
        {
            size_t n = 0, spos = 0, fpos = 0, epos = str.size();
            std::basic_string<CharT> s;
            while (fpos < epos)
            {
                fpos = str.find(delimit.data(), spos, delimit.size());
                if (fpos != std::basic_string<CharT>::npos && ++n < max_count)
                {
                    s = str.substr(spos, fpos - spos);
                    spos = fpos + delimit.size();
                }
                else
                {
                    s = str.substr(spos, str.size() - spos);
                    fpos = epos;
                }
                if (trimAll)
                {
                    piv::edit::trim_left(s);
                    piv::edit::trim_right(s);
                    if (ignoreEmptyStr && s.empty())
                        continue;
                }
                else if (ignoreEmptyStr)
                {
                    if (s.empty())
                        continue;
                    bool isEmpty = true;
                    for (auto it = s.begin(); it != s.end(); it++)
                    {
                        if (*it > ' ')
                        {
                            isEmpty = false;
                            break;
                        }
                    }
                    if (isEmpty)
                        continue;
                }
                strArray.push_back(s);
            }
        }
        return strArray.size();
    }
    inline size_t SplitSubStrings(const CharT *delimit, std::vector<std::basic_string<CharT>> &strArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        return SplitSubStrings(piv::basic_string_view<CharT>{delimit}, strArray, trimAll, ignoreEmptyStr, max_count);
    }
    inline size_t SplitSubStrings(const std::basic_string<CharT> &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        return SplitSubStrings(piv::basic_string_view<CharT>{delimit}, strArray, trimAll, ignoreEmptyStr, max_count);
    }
    inline size_t SplitSubStrings(const CVolMem &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        return SplitSubStrings(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(delimit.GetPtr()), static_cast<size_t>(delimit.GetSize()) / sizeof(CharT)}, strArray, trimAll, ignoreEmptyStr, max_count);
    }
    inline size_t SplitSubStrings(const CVolString &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return SplitSubStrings(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(delimit.GetText())}, strArray, trimAll, ignoreEmptyStr, max_count);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return SplitSubStrings(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{delimit}.GetText())}, strArray, trimAll, ignoreEmptyStr, max_count);
        }
        else
        {
            return SplitSubStrings(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{delimit}.GetText())}, strArray, trimAll, ignoreEmptyStr, max_count);
        }
    }

    /**
     * @brief 分割子文本视图
     * @param delimit 分割用子文本
     * @param svArray 结果存放数组
     * @param trimAll 是否删除首尾空
     * @param ignoreEmptyStr 是否忽略空白结果
     * @return 所分割出来的结果文本数目
     */
    size_t SplitSubStringViews(const piv::basic_string_view<CharT> &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        svArray.clear();
        piv::basic_string_view<CharT> sv{str};
        if (delimit.empty())
        {
            svArray.push_back(sv);
        }
        else
        {
            size_t n = 0, spos = 0, fpos = 0, epos = sv.size();
            piv::basic_string_view<CharT> ss;
            while (fpos < epos)
            {
                fpos = sv.find(delimit, spos);
                if (fpos != piv::basic_string_view<CharT>::npos && ++n < max_count)
                {
                    ss = sv.substr(spos, fpos - spos);
                    spos = fpos + delimit.size();
                }
                else
                {
                    ss = sv.substr(spos, sv.size() - spos);
                    fpos = epos;
                }
                if (trimAll)
                {
                    piv::edit::trim_left(ss);
                    piv::edit::trim_right(ss);
                    if (ignoreEmptyStr && ss.empty())
                        continue;
                }
                else if (ignoreEmptyStr)
                {
                    if (ss.empty())
                        continue;
                    bool isEmpty = true;
                    for (auto it = ss.begin(); it != ss.end(); it++)
                    {
                        if (*it > ' ')
                        {
                            isEmpty = false;
                            break;
                        }
                    }
                    if (isEmpty)
                        continue;
                }
                svArray.push_back(ss);
            }
        }
        return svArray.size();
    }
    inline size_t SplitSubStringViews(const CharT *delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        return SplitSubStringViews(piv::basic_string_view<CharT>{delimit}, svArray, trimAll, ignoreEmptyStr, max_count);
    }
    inline size_t SplitSubStringViews(const CVolMem &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        return SplitSubStringViews(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(delimit.GetPtr()), static_cast<size_t>(delimit.GetSize()) / sizeof(CharT)}, svArray, trimAll, ignoreEmptyStr, max_count);
    }
    inline size_t SplitSubStringViews(const std::basic_string<CharT> &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        return SplitSubStringViews(piv::basic_string_view<CharT>{delimit}, svArray, trimAll, ignoreEmptyStr, max_count);
    }
    inline size_t SplitSubStringViews(const CVolString &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool &trimAll = true, const bool &ignoreEmptyStr = true, const size_t &max_count = (size_t)-1) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return SplitSubStringViews(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(delimit.GetText())}, svArray, trimAll, ignoreEmptyStr, max_count);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return SplitSubStringViews(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{delimit}.GetText())}, svArray, trimAll, ignoreEmptyStr, max_count);
        }
        else
        {
            return SplitSubStringViews(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{delimit}.GetText())}, svArray, trimAll, ignoreEmptyStr, max_count);
        }
    }

    /**
     * @brief 复制文本
     * @param dest 欲复制到的目标
     * @param pos 欲复制的起始索引位置
     * @param count 欲复制的字符数目
     * @return 复制的字符数
     */
    inline size_t CopyStr(CharT *dest, const size_t &pos, const size_t &count) const
    {
        ASSERT(pos <= str.size()); // 判断索引位置是否有效
        return str.copy(dest, count, pos);
    }
    inline size_t CopyStr(ptrdiff_t dest, const size_t &pos, const size_t &count) const
    {
        return CopyStr(reinterpret_cast<CharT *>(dest), pos, count);
    }
    inline size_t CopyStr(CVolString &dest, const size_t &pos, const size_t &count) const
    {
        dest.SetLength(count);
        size_t n = CopyStr(const_cast<CharT *>(reinterpret_cast<cont CharT *>(dest.GetText())), pos, count);
        if (n == 0)
            dest.Empty();
        else
            dest.RemoveChars(n, count - n);
        return n;
    }
    inline size_t CopyStr(CVolMem &dest, const size_t &pos, const size_t &count) const
    {
        dest.Alloc(count, TRUE);
        size_t n = CopyStr(const_cast<CharT *>(reinterpret_cast<cont CharT *>(dest.GetPtr())), pos, count);
        if (n == 0)
            dest.Empty();
        else
            dest.Realloc(n);
        return n;
    }
    inline size_t CopyStr(std::basic_string<CharT> &dest, const size_t &pos, const size_t &count) const
    {
        dest.resize(count, '\0');
        size_t n = CopyStr(const_cast<CharT *>(dest.data()), pos, count);
        if (n == 0)
            dest.clear();
        else
            dest.resize(n);
        return n;
    }

    /**
     * @brief 到字节集
     * @return
     */
    inline CVolMem ToVolMem(const bool &null_terminated) const
    {
        return CVolMem(reinterpret_cast<const void *>(str.data()), (str.size() + (null_terminated ? 1 : 0)) * sizeof(CharT));
    }

    /**
     * @brief 到文本型
     * @return
     */
    inline CVolString ToVolString() const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return CVolString(reinterpret_cast<const wchar_t *>(str.c_str()));
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return *PivU2Ws{reinterpret_cast<const char *>(str.c_str())};
        }
        else
        {
            return *PivA2Ws{reinterpret_cast<const char *>(str.c_str())};
        }
    }

    /**
     * @brief 到整数
     * @return
     */
    inline int32_t ToInt(const int32_t &radix = 0) const
    {
        PIV_IF(sizeof(CharT) == 2)
        {
            return static_cast<int32_t>(wcstoul(reinterpret_cast<const wchar_t *>(str.c_str()), nullptr, radix));
        }
        else
        {
            return static_cast<int32_t>(strtoul(reinterpret_cast<const char *>(str.c_str()), nullptr, radix));
        }
    }

    /**
     * @brief 到长整数
     * @return
     */
    inline int64_t ToInt64(const int32_t &radix = 0) const
    {
        PIV_IF(sizeof(CharT) == 2)
        {
            return static_cast<int64_t>(wcstoull(reinterpret_cast<const wchar_t *>(str.c_str()), nullptr, radix));
        }
        else
        {
            return static_cast<int64_t>(strtoull(reinterpret_cast<const char *>(str.c_str()), nullptr, radix));
        }
    }

    /**
     * @brief 到小数
     * @return
     */
    inline double ToDouble() const
    {
        PIV_IF(sizeof(CharT) == 2)
        {
            return _wtof(reinterpret_cast<const wchar_t *>(str.c_str()));
        }
        else
        {
            return atof(reinterpret_cast<const char *>(str.c_str()));
        }
    }

    /**
     * @brief 到文本视图
     * @return
     */
    inline piv::basic_string_view<CharT> ToStringView() const
    {
        return piv::basic_string_view<CharT>{str.c_str()};
    }

    /**
     * @brief 到小写
     * @return
     */
    inline PivString MakeLower() const
    {
        std::basic_string<CharT> lower;
        lower.resize(str.size());
        std::transform(str.begin(), str.end(), lower.begin(), (int (*)(int))std::tolower);
        return PivString{lower};
    }
    inline PivString &ToLower()
    {
        std::transform(str.begin(), str.end(), str.begin(), (int (*)(int))std::tolower);
        return *this;
    }

    /**
     * @brief 到大写
     * @return
     */
    inline PivString MakeUpper() const
    {
        std::basic_string<CharT> upper;
        upper.resize(str.size());
        std::transform(str.begin(), str.end(), upper.begin(), (int (*)(int))std::toupper);
        return PivString{upper};
    }
    inline PivString &ToUpper()
    {
        std::transform(str.begin(), str.end(), str.begin(), (int (*)(int))std::toupper);
        return *this;
    }

    /**
     * @brief 到全角
     * @param option 转换选项
     * @return 返回转换后的文本
     */
    inline PivString ToFullWidth(const int32_t &option) const
    {
        return PivString{piv::edit::to_fullwidth<CharT, EncodeType>(piv::basic_string_view<CharT>{str}, option)};
    }

    /**
     * @brief 到半角
     * @param option 转换选项
     * @return 返回转换后的文本
     */
    inline PivString ToHalfWidth(const int32_t &option) const
    {
        return PivString{piv::edit::to_halfwidth<CharT, EncodeType>(piv::basic_string_view<CharT>{str}, option)};
    }

    /**
     * @brief URL编码
     * @param utf8
     * @param ReservedWord 是否不编码保留字符
     * @return
     */
    inline PivString UrlEncode(const bool &utf8 = true, const bool &ReservedWord = false) const
    {
        return PivString{piv::encoding::UrlStrEncode(str, utf8, ReservedWord)};
    }

    /**
     * @brief 到十六进制文本
     * @param hexstr 返回的十六进制文本
     * @return
     */
    inline PivString ToHex(const bool &separator = false, std::basic_string<CharT> &hexstr = std::basic_string<CharT>{}) const
    {
        return PivString{piv::encoding::str_to_hex(piv::basic_string_view<CharT>{str}, separator, hexstr)};
    }
    inline CVolString &ToHexStr(const bool &separator = false, CVolString &hexstr = CVolString{}) const
    {
        return piv::encoding::str_to_hex(piv::basic_string_view<CharT>{str}, separator, hexstr);
    }

    /**
     * @brief 置十六进制文本
     * @param hexstr 十六进制文本
     * @return 返回自身
     */
    inline PivString &FromHexStr(const piv::basic_string_view<CharT> &hexstr)
    {
        piv::encoding::hex_to_str(hexstr, str);
        return *this;
    }
    inline PivString &FromHexStr(const std::basic_string<CharT> &hexstr)
    {
        piv::encoding::hex_to_str(piv::basic_string_view<CharT>{hexstr}, str);
        return *this;
    }
    inline PivString &FromHexStr(const CVolMem &hexstr)
    {
        piv::encoding::hex_to_str(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(hexstr.GetPtr()), static_cast<size_t>(hexstr.GetSize() / sizeof(CharT))}, str);
        return *this;
    }
    inline PivString &FromHexStr(const CVolString &hexstr)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            piv::encoding::hex_to_str(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(hexstr.GetText())}, str);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            piv::encoding::hex_to_str(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{hexstr}.GetText())}, str);
        }
        else
        {
            piv::encoding::hex_to_str(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{hexstr}.GetText())}, str);
        }
        return *this;
    }

    /**
     * @brief 加入十六进制文本
     * @param hexstr 十六进制文本
     * @return 返回自身
     */
    inline PivString &AddHexStr(const piv::basic_string_view<CharT> &hexstr)
    {
        str.append(piv::encoding::hex_to_str(hexstr));
        return *this;
    }
    inline PivString &AddHexStr(const std::basic_string<CharT> &hexstr)
    {
        str.append(piv::encoding::hex_to_str(piv::basic_string_view<CharT>{hexstr}));
        return *this;
    }
    inline PivString &AddHexStr(const CVolMem &hexstr)
    {
        str.append(piv::encoding::hex_to_str(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(hexstr.GetPtr()), static_cast<size_t>(hexstr.GetSize() / sizeof(CharT))}));
        return *this;
    }
    inline PivString &AddHexStr(const CVolString &hexstr)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            str.append(piv::encoding::hex_to_str(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(hexstr.GetText())}));
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            str.append(piv::encoding::hex_to_str(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{hexstr}.GetText())}));
        }
        else
        {
            str.append(piv::encoding::hex_to_str(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{hexstr}.GetText())}));
        }
        return *this;
    }

    /**
     * @brief 到USC2
     * @param en_ascii 是否编码ASCII字符
     * @return
     */
    inline PivString ToUsc2(const bool &en_ascii = false) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return PivString{piv::encoding::to_usc2<CharT>(piv::wstring_view{reinterpret_cast<const wchar_t *>(str.c_str())}, en_ascii)};
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return PivString{piv::encoding::to_usc2<CharT>(piv::wstring_view{*PivU2W{reinterpret_cast<const char *>(str.c_str())}}, en_ascii)};
        }
        else
        {
            return PivString{piv::encoding::to_usc2<CharT>(piv::wstring_view{*PivA2W{reinterpret_cast<const char *>(str.c_str())}}, en_ascii)};
        }
    }
    inline CVolString &ToUsc2Str(const bool &en_ascii = false, CVolString &usc2 = CVolString{}) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return piv::encoding::to_usc2str(piv::wstring_view{reinterpret_cast<const wchar_t *>(str.c_str())}, en_ascii, usc2);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return piv::encoding::to_usc2str(piv::wstring_view{*PivU2W{reinterpret_cast<const char *>(str.c_str())}}, en_ascii, usc2);
        }
        else
        {
            return piv::encoding::to_usc2str(piv::wstring_view{*PivA2W{reinterpret_cast<const char *>(str.c_str())}}, en_ascii, usc2);
        }
    }

    /**
     * @brief 置USC2文本
     * @param usc2 所欲置入的USC2文本
     * @return
     */
    inline PivString &FromUsc2Str(const piv::basic_string_view<CharT> &usc2)
    {
        std::wstring ret;
        PIV_IF(sizeof(EncodeType) == 2)
        {
            piv::encoding::usc2_to_str(usc2, ret);
            str.assign(reinterpret_cast<const CharT *>(ret.c_str()));
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            piv::encoding::usc2_to_str(piv::wstring_view{*PivU2W{reinterpret_cast<const char *>(usc2.data()), usc2.size()}}, ret);
            str.assign(reinterpret_cast<const CharT *>(PivW2U{ret}.GetText()));
        }
        else
        {
            piv::encoding::usc2_to_str(piv::wstring_view{*PivA2W{reinterpret_cast<const char *>(usc2.data()), usc2.size()}}, ret);
            str.assign(reinterpret_cast<const CharT *>(PivW2A{ret}.GetText()));
        }
        return *this;
    }
    inline PivString &FromUsc2Str(const std::basic_string<CharT> &usc2)
    {
        return FromUsc2Str(piv::basic_string_view<CharT>{usc2});
    }
    inline PivString &FromUsc2Str(const CVolMem &usc2)
    {
        return FromUsc2Str(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(usc2.GetPtr()), static_cast<size_t>(usc2.GetSize()) / sizeof(CharT)});
    }
    inline PivString &FromUsc2Str(const CVolString &usc2)
    {
        std::wstring ret;
        piv::encoding::usc2_to_str(piv::wstring_view{usc2.GetText()}, ret);
        PIV_IF(sizeof(EncodeType) == 2)
        {
            str.assign(reinterpret_cast<const CharT *>(ret.c_str()));
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            str.assign(reinterpret_cast<const CharT *>(PivW2U{ret}.GetText()));
        }
        else
        {
            str.assign(reinterpret_cast<const CharT *>(PivW2A{ret}.GetText()));
        }
        return *this;
    }

    /**
     * @brief 加入USC2文本
     * @param usc2 所欲加入的USC2文本
     * @return
     */
    inline PivString &AddUsc2Str(const piv::basic_string_view<CharT> &usc2)
    {
        std::wstring ret;
        PIV_IF(sizeof(EncodeType) == 2)
        {
            piv::encoding::usc2_to_str(usc2, ret);
            str.append(reinterpret_cast<const CharT *>(ret.c_str()));
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            piv::encoding::usc2_to_str(piv::wstring_view{*PivU2W{reinterpret_cast<const char *>(usc2.data()), usc2.size()}}, ret);
            str.append(reinterpret_cast<const CharT *>(PivW2U{ret}.GetText()));
        }
        else
        {
            piv::encoding::usc2_to_str(piv::wstring_view{*PivA2W{reinterpret_cast<const char *>(usc2.data()), usc2.size()}}, ret);
            str.append(reinterpret_cast<const CharT *>(PivW2A{ret}.GetText()));
        }
        return *this;
    }
    inline PivString &AddUsc2Str(const std::basic_string<CharT> &usc2)
    {
        return AddUsc2Str(piv::basic_string_view<CharT>{usc2});
    }
    inline PivString &AddUsc2Str(const CVolMem &usc2)
    {
        return AddUsc2Str(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(usc2.GetPtr()), static_cast<size_t>(usc2.GetSize()) / sizeof(CharT)});
    }
    inline PivString &AddUsc2Str(const CVolString &usc2)
    {
        std::wstring ret;
        piv::encoding::usc2_to_str(piv::wstring_view{usc2.GetText()}, ret);
        PIV_IF(sizeof(EncodeType) == 2)
        {
            str.append(reinterpret_cast<const CharT *>(ret.c_str()));
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            str.append(reinterpret_cast<const CharT *>(PivW2U{ret}.GetText()));
        }
        else
        {
            str.append(reinterpret_cast<const CharT *>(PivW2A{ret}.GetText()));
        }
        return *this;
    }

    /**
     * @brief URL解码
     * @param utf8 是否为UTF-8编码
     * @return
     */
    inline PivString UrlDecode(const bool &utf8 = true) const
    {
        return PivString{piv::encoding::UrlStrDecode(str, utf8)};
    }

    /**
     * @brief BASE64编码
     * @return
     */
    inline PivString EncodeBase64(const int32_t &line_len = 0) const
    {
        return PivString{piv::base64<CharT>{}.encode(str, line_len)};
    }

    /**
     * @brief BASE64解码
     * @return
     */
    inline PivString DecodeBase64() const
    {
        return PivString{piv::base64<CharT>{}.decode(str)};
    }

    /**
     * @brief BASE64到字节集
     * @return
     */
    inline CVolMem DecodeBase64Bin() const
    {
        CVolMem buffer;
        piv::base64<CharT>{}.decode(str, buffer);
        return buffer;
    }

    /**
     * @brief BASE85编码
     * @return
     */
    inline PivString EncodeBase85(const bool &padding = false) const
    {
        return PivString{piv::base85<CharT>{}.encode(str, padding)};
    }

    /**
     * @brief BASE85解码
     * @return
     */
    inline PivString DecodeBase85() const
    {
        return PivString{piv::base85<CharT>{}.decode(str)};
    }

    /**
     * @brief BASE85到字节集
     * @return
     */
    inline CVolMem DecodeBase85Bin() const
    {
        CVolMem buffer;
        piv::base85<CharT>{}.decode(str, buffer);
        return buffer;
    }

    /**
     * @brief BASE91编码
     * @return
     */
    inline PivString EncodeBase91() const
    {
        return PivString{piv::base91<CharT>{}.encode(str)};
    }

    /**
     * @brief BASE91解码
     * @return
     */
    inline PivString DecodeBase91() const
    {
        return PivString{piv::base91<CharT>{}.decode(str)};
    }

    /**
     * @brief BASE64到字节集
     * @return
     */
    inline CVolMem DecodeBase91Bin() const
    {
        CVolMem buffer;
        piv::base91<CharT>{}.decode(str, buffer);
        return buffer;
    }

    /**
     * @brief 取逆序文本
     * @return
     */
    inline PivString GetReverseText() const
    {
        return PivString{piv::edit::reverse_text<CharT, EncodeType>(str)};
    }

    /**
     * @brief 逆序置文本
     * @return
     */
    inline PivString ReverseSetText()
    {
        str = PivString{piv::edit::reverse_text<CharT, EncodeType>(str)};
        return *this;
    }
}; // PivString

using PivStringViewA = PivStringView<char, piv::ansi>;
using PivStringViewU = PivStringView<char, piv::utf8>;
using PivStringViewW = PivStringView<wchar_t, piv::utf16_le>;

using PivStringA = PivString<char, piv::ansi>;
using PivStringU = PivString<char, piv::utf8>;
using PivStringW = PivString<wchar_t, piv::utf16_le>;

/**
 * @brief 任意编码转UTF-8的封装类
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
    PivAny2U(const char *s)
    {
        if (s == nullptr || strlen(s) == 0)
            return;
        pSv.reset(new piv::string_view{s});
    }

    PivAny2U(const wchar_t *s)
    {
        if (s == nullptr || wcslen(s) == 0)
            return;
        pStr.reset(new std::string{});
        piv::encoding::W2Uex(*pStr, s, -1);
    }

    PivAny2U(const CVolString &s)
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
            pStr.reset(std::string{});
            piv::encoding::A2Uex(*pStr, reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize()));
        }
#else
        pSv.reset(new piv::string_view{reinterpret_cast<const char *>(s.GetPtr()), static_cast<size_t>(s.GetSize())});
#endif
    }

    PivAny2U(const PivStringA &s)
    {
        if (s.IsEmpty())
            return;
        pStr.reset(new std::string{});
        piv::encoding::A2Uex(*pStr, s.GetText(), s.GetLength());
    }

    PivAny2U(const PivStringU &s)
    {
        if (s.IsEmpty())
            return;
        pSv.reset(new piv::string_view{s.GetText()});
    }

    PivAny2U(const PivStringW &s)
    {
        if (s.IsEmpty())
            return;
        pStr.reset(new std::string{});
        piv::encoding::W2Uex(*pStr, s.GetText(), s.GetLength());
    }

    PivAny2U(const PivStringViewA &s)
    {
        if (s.IsEmpty())
            return;
        pStr.reset(new std::string{});
        piv::encoding::A2Uex(*pStr, s.GetText(), s.GetLength());
    }

    PivAny2U(const PivStringViewU &s)
    {
        if (s.IsEmpty())
            return;
        pStr.reset(new std::string{s.GetText(), s.GetLength()});
    }

    PivAny2U(const PivStringViewW &s)
    {
        if (s.IsEmpty())
            return;
        pStr.reset(new std::string{});
        piv::encoding::W2Uex(*pStr, s.GetText(), s.GetLength());
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
    inline const char *GetPtr() const noexcept
    {
        if (pStr)
            return pStr->data();
        if (pSv)
            return pSv->data();
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

#endif // _PIV_STRING_HPP
