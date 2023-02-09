/*********************************************\
 * 火山视窗PIV模块 - Base64/85/91编解码      *
 * 参考开源项目:                             *
 * https://github.com/r-lyeh-archived/base   *
 * https://github.com/zhllxt/asio2           *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 版本: 2023/02/09                          *
\*********************************************/

#ifndef _PIV_BASE_ENCODING_HPP
#define _PIV_BASE_ENCODING_HPP

#include "piv_encoding.hpp"

namespace piv
{
    namespace detail
    {
        static string_view const base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";
        static string_view const base85_chars =
            "0123456789"
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            ".-:+=^!/*?&<>()[]{}@%$#";
        static string_view const base91_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789!#$%&()*+,./:;-="
            "\\?@[]^_`{|}~\'";
    } // namespace detail

    /**
     * @brief BASE64编解码类
     * @tparam CharT 字符类型
     */
    template <typename CharT>
    class base64
    {
    private:
        inline bool is_base64(unsigned char c) noexcept
        {
            return (c == 43 ||               // +
                    (c >= 47 && c <= 57) ||  // /-9
                    (c >= 65 && c <= 90) ||  // A-Z
                    (c >= 97 && c <= 122) || // a-z
                    (c == 10) || (c == 13)); // \r\n
        }
        inline void AddChar(std::basic_string<CharT> &s, unsigned char c, uint32_t &count, const uint32_t max_line_len) noexcept
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
        inline void AddChar(CVolString &s, unsigned char c, uint32_t &count, const uint32_t max_line_len) noexcept
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
        inline T encode(const unsigned char *input, size_t len, const int line_len = 0)
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
        inline std::basic_string<CharT> encode(const basic_string_view<CharT> &input, const int line_len = 0)
        {
            return encode<std::basic_string<CharT>>(reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT), line_len);
        }
        inline std::basic_string<CharT> encode(const std::basic_string<CharT> &input, const int line_len = 0)
        {
            return encode<std::basic_string<CharT>>(reinterpret_cast<const unsigned char *>(input.data()), input.size() * sizeof(CharT), line_len);
        }
        inline std::basic_string<CharT> encode(const CVolMem &input, const int line_len = 0)
        {
            return encode<std::basic_string<CharT>>(reinterpret_cast<const unsigned char *>(input.GetPtr()), static_cast<size_t>(input.GetSize()), line_len);
        }
        inline CVolString Encode(const CVolMem &input, const int line_len = 0)
        {
            return encode<CVolString>(reinterpret_cast<const unsigned char *>(input.GetPtr()), static_cast<size_t>(input.GetSize()), line_len);
        }

        /**
         * @brief 将提供的BASE64编码文本解码
         * @param input 所欲转换的BASE64编码
         * @return 返回解码后的数据
         */
        inline void decode(const basic_string_view<CharT> &input, CVolMem &output)
        {
            size_t in_len = input.size();
            int i = 0;
            int j = 0;
            int in_ = 0;
            unsigned char char_array_4[4], char_array_3[3];
            output.Empty();

            while (in_len-- && (input[in_] != '=') && is_base64(static_cast<unsigned char>(input[in_])))
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
        }
        inline void decode(const std::basic_string<CharT> &input, CVolMem &output)
        {
            decode(basic_string_view<CharT>{input}, output);
        }
        inline void decode(const CharT *input, CVolMem &output, const size_t count = static_cast<size_t>(-1))
        {
            decode((count == static_cast<size_t>(-1)) ? basic_string_view<CharT>{input} : basic_string_view<CharT>{input, count}, output);
        }
        inline void decode(const CVolMem &input, CVolMem &output)
        {
            decode(basic_string_view<CharT>{reinterpret_cast<const CharT *>(input.GetPtr()), static_cast<size_t>(input.GetSize()) / sizoef(CharT)}, output);
        }
        inline void decode(const CVolString &input, CVolMem &output)
        {
            decode(basic_string_view<CharT>{reinterpret_cast<const CharT *>(input.GetText())}, output);
        }
        template <typename T>
        inline std::basic_string<CharT> decode(T &input)
        {
            CVolMem buffer;
            decode(input, buffer);
            return std::basic_string<CharT>{reinterpret_cast<const CharT *>(buffer.GetPtr()), static_cast<size_t>(buffer.GetSize()) / sizeof(CharT)};
        }
        template <typename T>
        inline CVolMem Decode(CVolString &input)
        {
            CVolMem output;
            decode(basic_string_view<CharT>{reinterpret_cast<const CharT *>(input.GetText())}, output);
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
        inline void AddChar(std::basic_string<CharT> &s, unsigned char c) noexcept
        {
            s.push_back(c);
        }
        inline void AddChar(CVolString &s, unsigned char c) noexcept
        {
            s.AddChar(c);
        }
        inline void ClearStr(std::basic_string<CharT> &s) noexcept
        {
            s.clear();
        }
        inline void ClearStr(CVolString &s) noexcept
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
        inline std::basic_string<CharT> encode(const basic_string_view<CharT> &input, const bool padding = false)
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
        inline std::basic_string<CharT> encode(const std::basic_string<CharT> &input, const bool padding = false)
        {
            return encode(basic_string_view<CharT>{input}, padding);
        }
        inline std::basic_string<CharT> encode(const CVolMem &input, const bool padding = false)
        {
            return encode(basic_string_view<CharT>{reinterpret_cast<const CharT *>(input.GetPtr()), static_cast<size_t>(input.GetSize()) / sizeof(CharT)}, padding);
        }
        inline CVolString Encode(const CVolMem &input, const bool padding = false)
        {
            CVolString output;
            if (input.GetSize() % 4 == 0)
            {
                encode<CVolString>(reinterpret_cast<const unsigned char *>(input.GetPtr()), static_cast<size_t>(input.GetSize()), output);
            }
            else
            {
                if (padding)
                {
                    CVolMem pad = input;
                    pad.AddManyU8Chars('\0', input.GetSize() % 4);
                    encode<CVolString>(reinterpret_cast<const unsigned char *>(pad.GetPtr()), static_cast<size_t>(pad.GetSize()), output);
                }
            }
            return output;
        }

        /**
         * @brief 将提供的BASE85编码文本解码
         * @param input 所欲转换的BASE85编码
         * @return 返回解码后的数据
         */
        inline bool decode(const basic_string_view<CharT> &input, CVolMem &output)
        {
            if (input.size() % 5)
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
            const CharT *z85 = input.data();
            output.SetMemAlignSize(input.size() * 4 / 5);
            for (size_t o = 0; o < input.size() * 4 / 5; z85 += 5, o += 4)
            {
                uint32_t value = decoder[z85[0]] * 0x31C84B1 + decoder[z85[1]] * 0x95EED +
                                 decoder[z85[2]] * 0x1C39 + decoder[z85[3]] * 0x55 + decoder[z85[4]];
                output.AddU8Char(U8CHAR((value >> 24) & 0xFF));
                output.AddU8Char(U8CHAR((value >> 16) & 0xFF));
                output.AddU8Char(U8CHAR((value >> 8) & 0xFF));
                output.AddU8Char(U8CHAR((value >> 0) & 0xFF));
            }
            return true;
        }
        inline bool decode(const std::basic_string<CharT> &input, CVolMem &output)
        {
            return decode(basic_string_view<CharT>{input}, output);
        }
        inline bool decode(const CharT *input, CVolMem &output, const size_t count = static_cast<size_t>(-1))
        {
            return decode((count == static_cast<size_t>(-1)) ? basic_string_view<CharT>{input} : basic_string_view<CharT>{input, count}, output);
        }
        inline bool decode(const CVolMem &input, CVolMem &output)
        {
            return decode(basic_string_view<CharT>{reinterpret_cast<const CharT *>(input.GetPtr()), static_cast<size_t>(input.GetSize()) / sizoef(CharT)}, output);
        }
        inline bool decode(const CVolString &input, CVolMem &output)
        {
            return decode(basic_string_view<CharT>{reinterpret_cast<const CharT *>(input.GetText())}, output);
        }
        template <typename T>
        inline std::basic_string<CharT> decode(T &input)
        {
            CVolMem buffer;
            decode(input, buffer);
            return std::basic_string<CharT>{reinterpret_cast<const CharT *>(buffer.GetPtr()), static_cast<size_t>(buffer.GetSize()) / sizeof(CharT)};
        }

        inline CVolMem Decode(const CVolString &input)
        {
            CVolMem buffer;
            decode(wstring_view{input.GetText()}, buffer);
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
        inline void AddChar(std::basic_string<CharT> &s, unsigned char c) noexcept
        {
            s.push_back(c);
        }
        inline void AddChar(CVolString &s, unsigned char c) noexcept
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
        inline std::basic_string<CharT> encode(const basic_string_view<CharT> &input)
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
        inline CVolString Encode(const CVolMem &input)
        {
            return encode<CVolString>(reinterpret_cast<const unsigned char *>(input.GetPtr()), static_cast<size_t>(input.GetSize()));
        }

        /**
         * @brief 将提供的BASE91编码文本解码
         * @param input 所欲转换的BASE91编码
         * @return 返回解码后的数据
         */
        inline void decode(const basic_string_view<CharT> &input, CVolMem &output)
        {
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
            output.SetMemAlignSize(input.size() * sizeof(CharT) * 6 / 4);
            const unsigned char *ib = reinterpret_cast<const unsigned char *>(input.data());

            unsigned long queue = 0;
            unsigned int nbits = 0;
            int val = -1;

            for (size_t len = input.size() * sizeof(CharT); len--;)
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
        inline void decode(const std::basic_string<CharT> &input, CVolMem &output)
        {
            decode(basic_string_view<CharT>{input}, output);
        }
        inline void decode(const CharT *input, CVolMem &output, const size_t count = static_cast<size_t>(-1))
        {
            decode((count == static_cast<size_t>(-1)) ? basic_string_view<CharT>{input} : basic_string_view<CharT>{input, count}, output);
        }
        inline void decode(const CVolMem &input, CVolMem &output)
        {
            decode(basic_string_view<CharT>{reinterpret_cast<const CharT *>(input.GetPtr()), static_cast<size_t>(input.GetSize()) / sizoef(CharT)}, output);
        }
        inline void decode(const CVolString &input, CVolMem &output)
        {
            decode(basic_string_view<CharT>{reinterpret_cast<const CharT *>(input.GetText())}, output);
        }
        template <typename T>
        inline std::basic_string<CharT> decode(T &input)
        {
            CVolMem buffer;
            decode(input, buffer);
            return std::basic_string<CharT>{reinterpret_cast<const CharT *>(buffer.GetPtr()), static_cast<size_t>(buffer.GetSize()) / sizeof(CharT)};
        }
        inline CVolMem Decode(const CVolString &input)
        {
            CVolMem buffer;
            decode(wstring_view{input.GetText()}, buffer);
            return buffer;
        }

    }; // base91

} // namespace piv

#endif // _PIV_BASE_ENCODING_HPP
