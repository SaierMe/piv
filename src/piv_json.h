/*********************************************\
 * 火山视窗PIV模块 - nlohmann JSON           *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef _PIV_NLOHMANN_JSON_H
#define _PIV_NLOHMANN_JSON_H

#include "piv_string.hpp"
#include "json.hpp"

using PivJSON = nlohmann::basic_json<nlohmann::ordered_map>;
using PivJSON_Unordered = nlohmann::basic_json<>;

namespace piv
{
    namespace json
    {

        /**
         * @brief 文本是否可解析
         * @param input 所欲测试的文本
         * @param ignore_comments 忽略注释
         * @return 返回是否为json数据
         */
        template <typename J, typename T>
        inline bool AcceptText(const T &input, bool ignore_comments)
        {
            return J::accept(input, ignore_comments);
        }

        /**
         * @brief 文件是否可解析
         * @param input 所欲测试的文件
         * @param ignore_comments 忽略注释
         * @return 返回是否为json文件
         */
        template <typename J>
        bool AcceptFile(const wchar_t *file, bool ignore_comments)
        {
            FILE *fp = _wfopen(file, L"r");
            if (!fp)
                return false;
            bool ret = J::accept(fp, ignore_comments);
            fclose(fp);
            return ret;
        }

        /**
         * @brief 是否可解析
         * @param input 所欲测试的数据
         * @param ignore_comments 忽略注释
         * @param encoding 字节集编码
         * @return 返回是否为json数据
         */
        template <typename J>
        bool Accept(const ::piv::string_view &input, bool ignore_comments, int32_t encoding = 1)
        {
            ::piv::string_view test{input};
            piv::edit::trim_left(test);
            piv::edit::trim_right(test);
            if (test.empty())
                return false;
            if (test.front() == '{' || test.front() == '[')
                return AcceptText<J>(test, ignore_comments);
            else
                return AcceptFile<J>(PivU2W{test.data(), test.size()}.c_str(), ignore_comments);
        }

        template <typename J>
        bool Accept(const ::piv::wstring_view &input, bool ignore_comments, int32_t encoding = 1)
        {
            ::piv::wstring_view test{input};
            piv::edit::trim_left(test);
            piv::edit::trim_right(test);
            if (test.empty())
                return false;
            if (test.front() == '{' || test.front() == '[')
                return AcceptText<J>(test, ignore_comments);
            else
                return AcceptFile<J>(std::wstring{test.data(), test.size()}.c_str(), ignore_comments);
        }

        template <typename J>
        inline bool Accept(const std::string &input, bool ignore_comments, int32_t encoding = 1)
        {
            return Accept<J>(::piv::string_view{input}, ignore_comments, encoding);
        }

        template <typename J>
        bool Accept(const std::wstring &input, bool ignore_comments, int32_t encoding = 1)
        {
            ::piv::wstring_view test{input};
            piv::edit::trim_left(test);
            piv::edit::trim_right(test);
            if (test.empty())
                return false;
            if (test.front() == '{' || test.front() == '[')
                return AcceptText<J>(test, ignore_comments);
            else
                return AcceptFile<J>(input.c_str(), ignore_comments);
        }

        template <typename J>
        inline bool Accept(const wchar_t *input, bool ignore_comments, int32_t encoding = 1)
        {
            return Accept<J>(::piv::wstring_view{input}, ignore_comments, encoding);
        }

        template <typename J>
        bool Accept(const CWString &input, bool ignore_comments, int32_t encoding = 1)
        {
            ::piv::wstring_view test{input.GetText()};
            piv::edit::trim_left(test);
            piv::edit::trim_right(test);
            if (test.empty())
                return false;
            if (test.front() == '{' || test.front() == '[')
                return AcceptText<J>(test, ignore_comments);
            else
                return AcceptFile<J>(input.GetText(), ignore_comments);
        }

        template <typename J>
        bool Accept(const CVolMem &input, bool ignore_comments, int32_t encoding = 1)
        {
            size_t len = static_cast<size_t>(input.GetSize());
            if (len == 0)
                return false;
#ifdef SIMDUTF_H
            int str_encoding = simdutf::detect_encodings(reinterpret_cast<const char *>(input.GetPtr()), len);
#else
            int str_encoding = encoding;
#endif
            if ((str_encoding & 8) == 8) // UTF-32LE
            {
                if (static_cast<uint8_t>(input.Get_S_BYTE(0)) == 0xFF && len > 4 && static_cast<uint8_t>(input.Get_S_BYTE(1)) == 0xFE)
                    return J::accept(reinterpret_cast<const char32_t *>(input.GetPtr()) + 1, reinterpret_cast<const char32_t *>(input.GetPtr()) + len / 4, ignore_comments);
                else
                    return J::accept(reinterpret_cast<const char32_t *>(input.GetPtr()), reinterpret_cast<const char32_t *>(input.GetPtr()) + len / 4, ignore_comments);
            }
            else if ((str_encoding & 2) == 2) // UTF-16LE
            {
                if (static_cast<uint8_t>(input.Get_S_BYTE(0)) == 0xFF && len > 2 && static_cast<uint8_t>(input.Get_S_BYTE(1)) == 0xFE)
                    return Accept<J>(::piv::wstring_view{reinterpret_cast<const wchar_t *>(input.GetPtr()) + 1, (len / 2) - 1}, ignore_comments);
                else
                    return Accept<J>(::piv::wstring_view{reinterpret_cast<const wchar_t *>(input.GetPtr()), len / 2}, ignore_comments);
            }
            else if ((str_encoding & 1) == 1) // UTF-8
            {
                if (static_cast<uint8_t>(input.Get_S_BYTE(0)) == 0xEF && len > 3 && static_cast<uint8_t>(input.Get_S_BYTE(1)) == 0xBB && static_cast<uint8_t>(input.Get_S_BYTE(2)) == 0xBF)
                    return Accept<J>(::piv::string_view{reinterpret_cast<const char *>(input.GetPtr()) + 3, len - 3}, ignore_comments);
                else
                    return Accept<J>(::piv::string_view{reinterpret_cast<const char *>(input.GetPtr()), len}, ignore_comments);
            }
            else if (str_encoding == 0) // ANSI
            {
                return Accept<J>(PivA2U{reinterpret_cast<const char *>(input.GetPtr()), len}.str, ignore_comments);
            }
            return false;
        }

        /**
         * @brief 解析文本
         * @param input 所欲解析的文本
         * @param allow_exceptions 允许异常
         * @param ignore_comments 忽略注释
         * @param cb 解析回调
         * @return 返回是否解析成功
         */
        template <typename J, typename T, typename F>
        inline bool ParseText(J &&json, const T &input, const F cb, bool allow_exceptions, bool ignore_comments)
        {
            json = std::decay<J>::type::parse(input, cb, allow_exceptions, ignore_comments);
            return !json.is_discarded();
        }

        /**
         * @brief 解析文件
         * @param input 所欲解析的文件
         * @param allow_exceptions 允许异常
         * @param ignore_comments 忽略注释
         * @param cb 解析回调
         * @return 返回是否解析成功
         */
        template <typename J, typename F>
        bool ParseFile(J &&json, const wchar_t *file, const F cb, bool allow_exceptions, bool ignore_comments)
        {
            FILE *fp = _wfopen(file, L"r");
            if (!fp)
                return false;
            json = std::decay<J>::type::parse(fp, cb, allow_exceptions, ignore_comments);
            fclose(fp);
            return !json.is_discarded();
        }

        /**
         * @brief 解析
         * @param json JSON对象
         * @param input 所欲解析的数据
         * @param allow_exceptions 允许异常
         * @param ignore_comments 忽略注释
         * @param encoding 字节集编码
         * @param cb 解析回调
         * @return 返回是否解析成功
         */
        template <typename J, typename F>
        bool Parse(J &&json, const ::piv::string_view &input, const F cb, bool allow_exceptions, bool ignore_comments, int32_t encoding = 1)
        {
            ::piv::string_view test{input};
            piv::edit::trim_left(test);
            piv::edit::trim_right(test);
            if (test.empty())
                return false;
            if (test.front() == '{' || test.front() == '[')
                return ParseText(json, test, cb, allow_exceptions, ignore_comments);
            else
                return ParseFile(json, PivU2W{test.data(), test.size()}.c_str(), cb, allow_exceptions, ignore_comments);
        }

        template <typename J, typename F>
        bool Parse(J &&json, const ::piv::wstring_view &input, const F cb, bool allow_exceptions, bool ignore_comments, int32_t encoding = 1)
        {
            ::piv::wstring_view test{input};
            piv::edit::trim_left(test);
            piv::edit::trim_right(test);
            if (test.empty())
                return false;
            if (test.front() == '{' || test.front() == '[')
                return ParseText(json, test, cb, allow_exceptions, ignore_comments);
            else
                return ParseFile(json, std::wstring{test.data(), test.size()}.c_str(), cb, allow_exceptions, ignore_comments);
        }

        template <typename J, typename F>
        inline bool Parse(J &&json, const std::string &input, const F cb, bool allow_exceptions, bool ignore_comments, int32_t encoding = 1)
        {
            return Parse(json, ::piv::string_view{input}, cb, allow_exceptions, ignore_comments);
        }

        template <typename J, typename F>
        bool Parse(J &&json, const std::wstring &input, const F cb, bool allow_exceptions, bool ignore_comments, int32_t encoding = 1)
        {
            ::piv::wstring_view test{input};
            piv::edit::trim_left(test);
            piv::edit::trim_right(test);
            if (test.empty())
                return false;
            if (test.front() == '{' || test.front() == '[')
                return ParseText(json, test, cb, allow_exceptions, ignore_comments);
            else
                return ParseFile(json, input.c_str(), cb, allow_exceptions, ignore_comments);
        }

        template <typename J, typename F>
        bool Parse(J &&json, const CWString &input, const F cb, bool allow_exceptions, bool ignore_comments, int32_t encoding = 1)
        {
            ::piv::wstring_view test{input.GetText()};
            piv::edit::trim_left(test);
            piv::edit::trim_right(test);
            if (test.empty())
                return false;
            if (test.front() == '{' || test.front() == '[')
                return ParseText(json, test, cb, allow_exceptions, ignore_comments);
            else
                return ParseFile(json, input.GetText(), cb, allow_exceptions, ignore_comments);
        }

        template <typename J, typename F>
        inline bool Parse(J &&json, const wchar_t *input, const F cb, bool allow_exceptions, bool ignore_comments, int32_t encoding = 1)
        {
            return Parse(json, ::piv::wstring_view{input}, cb, allow_exceptions, ignore_comments, encoding);
        }

        template <typename J, typename F>
        inline bool Parse(J &&json, const CVolMem &input, const F cb, bool allow_exceptions, bool ignore_comments, int32_t encoding = 1)
        {
            const BYTE *data = input.GetPtr();
            size_t len = static_cast<size_t>(input.GetSize());
            if (len == 0)
                return false;
#ifdef SIMDUTF_H
            if (encoding == -1)
                encoding = simdutf::detect_encodings(reinterpret_cast<const char *>(input.GetPtr()), len);
#endif
            if ((encoding & 8) == 8) // UTF-32LE
            {
                if (data[0] == 0xFF && len > 4 && data[1] == 0xFE)
                {
                    data += 4;
                    len -= 4;
                }
                json = std::decay<J>::type::parse(reinterpret_cast<const char32_t *>(data), reinterpret_cast<const char32_t *>(data) + len, cb, allow_exceptions, ignore_comments);
                return !json.is_discarded();
            }
            if ((encoding & 2) == 2) // UTF-16LE
            {
                if (data[0] == 0xFF && len > 2 && data[1] == 0xFE)
                {
                    data += 2;
                    len -= 2;
                }
                if (reinterpret_cast<const wchar_t *>(data)[0] == '{' || reinterpret_cast<const wchar_t *>(data)[0] == '[')
                    return ParseText(json, ::piv::wstring_view{reinterpret_cast<const wchar_t *>(data), len}, cb, allow_exceptions, ignore_comments);
            }
            if ((encoding & 1) == 1) // UTF-8
            {
                if (data[0] == 0xEF && len > 3 && data[1] == 0xBB && data[2] == 0xBF)
                {
                    data += 3;
                    len -= 3;
                }
                return ParseText(json, ::piv::string_view{reinterpret_cast<const char *>(data), len}, cb, allow_exceptions, ignore_comments);
            }
            if (encoding == 0) // ANSI
            {
                return ParseText(json, PivA2U{reinterpret_cast<const char *>(data), len}.str, cb, allow_exceptions, ignore_comments);
            }
            return false;
        }

        /**
         * @brief 转换到输入类型的数据
         * @param input 输入数据
         * @return 返回输入类型
         */
        template <typename = void>
        const ::piv::string_view input_t(const CVolMem &input)
        {
            return ::piv::string_view{reinterpret_cast<const char *>(input.GetPtr()), static_cast<size_t>(input.GetSize())};
        }

        template <typename = void>
        const std::vector<uint8_t> &input_t(const std::vector<uint8_t> &input)
        {
            return input;
        }

        template <typename = void>
        const ::piv::string_view &input_t(const ::piv::string_view &input)
        {
            return input;
        }

        template <typename = void>
        const std::string &input_t(const std::string &input)
        {
            return input;
        }

        /**
         * @brief 解析BJData
         * @param json JSON对象
         * @param input 所欲解析的数据
         * @param strict 严格模式
         * @param allow_exceptions 允许异常
         * @return 返回是否解析成功
         */
        template <typename J, typename T>
        inline bool From_bjdata(J &&json, const T &input, bool strict, bool allow_exceptions)
        {
            json = std::decay<J>::type::from_bjdata(input_t(input), strict, allow_exceptions);
            return !json.is_discarded();
        }

        /**
         * @brief 解析BSON
         * @param json JSON对象
         * @param input 所欲解析的数据
         * @param strict 严格模式
         * @param allow_exceptions 允许异常
         * @return 返回是否解析成功
         */
        template <typename J, typename T>
        inline bool From_bson(J &&json, const T &input, bool strict, bool allow_exceptions)
        {
            json = std::decay<J>::type::from_bson(input_t(input), strict, allow_exceptions);
            return !json.is_discarded();
        }

        /**
         * @brief 解析CBOR
         * @param json JSON对象
         * @param input 所欲解析的数据
         * @param strict 严格模式
         * @param allow_exceptions 允许异常
         * @param tag_handler 标签处理
         * @return 返回是否解析成功
         */
        template <typename J, typename T>
        inline bool From_cbor(J &&json, const T &input, bool strict, bool allow_exceptions, int32_t tag_handler)
        {
            json = std::decay<J>::type::from_cbor(input_t(input), strict, allow_exceptions, static_cast<J::cbor_tag_handler_t>(tag_handler));
            return !json.is_discarded();
        }

        /**
         * @brief 解析MSGPACK
         * @param json JSON对象
         * @param input 所欲解析的数据
         * @param strict 严格模式
         * @param allow_exceptions 允许异常
         * @return 返回是否解析成功
         */
        template <typename J, typename T>
        inline bool From_msgpack(J &&json, const T &input, bool strict, bool allow_exceptions)
        {
            json = std::decay<J>::type::from_msgpack(input_t(input), strict, allow_exceptions);
            return !json.is_discarded();
        }

        /**
         * @brief 解析UBJSON
         * @param json JSON对象
         * @param input 所欲解析的数据
         * @param strict 严格模式
         * @param allow_exceptions 允许异常
         * @return 返回是否解析成功
         */
        template <typename J, typename T>
        inline bool From_ubjson(J &&json, const T &input, bool strict, bool allow_exceptions)
        {
            json = std::decay<J>::type::from_ubjson(input_t(input), strict, allow_exceptions);
            return !json.is_discarded();
        }

        /**
         * @brief 缩进宽字符转换为ASCII字符(内部使用)
         * @param c 所欲转换的宽字符
         * @return 返回转换的ASCII字符
         */
        static char IndentWcharToChar(const wchar_t &c)
        {
            if (c < 128)
                return static_cast<char>(c);
            return ' '; // 非ASCII字符都返回为空格
        }

        /**
         * @brief 到可读文本(序列化)
         * @param json JSON对象
         * @param indent 缩进
         * @param indent_char 缩进字符
         * @param ensure_ascii 确保ASCII
         * @param error_handler 错误处理方式
         * @return 返回序列化文本
         */
        template <typename J>
        inline std::string Dump(J &&json, int32_t indent, wchar_t indent_char, bool ensure_ascii, int32_t error_handler)
        {
            return json.dump(indent, IndentWcharToChar(indent_char), ensure_ascii, static_cast<typename std::decay<J>::type::error_handler_t>(error_handler));
        }

        template <typename R, typename J>
        inline R DumpTo(J &&json, int32_t indent, wchar_t indent_char, bool ensure_ascii, int32_t error_handler)
        {
            std::string str = json.dump(indent, IndentWcharToChar(indent_char), ensure_ascii, static_cast<typename std::decay<J>::type::error_handler_t>(error_handler));
            return R(str.data(), str.size());
        }

        /**
         * @brief 到序列化二进制数据
         * @param json JSON对象
         * @param use_size 使用大小注解
         * @param use_type 使用类型注解
         * @return 返回字节集
         */
        template <typename J> // 到BJData
        inline CVolMem To_bjdata(const J &json, bool use_size, bool use_type)
        {
            std::vector<std::uint8_t> bin;
            J::to_bjdata(json, bin, use_size, use_type);
            return CVolMem(bin.data(), bin.size());
        }

        template <typename J> // 到BSON
        inline CVolMem To_bson(const J &json)
        {
            std::vector<std::uint8_t> bin;
            J::to_bson(json, bin);
            return CVolMem(bin.data(), bin.size());
        }

        template <typename J> // 到CBOR
        inline CVolMem To_cbor(const J &json)
        {
            std::vector<std::uint8_t> bin;
            J::to_cbor(json, bin);
            return CVolMem(bin.data(), bin.size());
        }

        template <typename J> // 到MsgPack
        inline CVolMem To_msgpack(const J &json)
        {
            std::vector<std::uint8_t> bin;
            J::to_msgpack(json, bin);
            return CVolMem(bin.data(), bin.size());
        }

        template <typename J> // 到UBJSON
        inline CVolMem To_ubjson(const J &json, bool use_size, bool use_type)
        {
            std::vector<std::uint8_t> bin;
            J::to_ubjson(json, bin, use_size, use_type);
            return CVolMem(bin.data(), bin.size());
        }

        /**
         * @brief 转换到JSON的值类型
         * @param num 所欲转换的数值类型
         * @param str 所欲转换的文本类型
         * @return 返回兼容的JSON值类型
         */
        template <typename = void>
        bool to_value(bool num)
        {
            return num;
        }

        template <typename V, typename = std::enable_if<std::is_integral<V>::value>::type>
        V to_value(V num)
        {
            return num;
        }

        template <typename = void>
        double to_value(double num)
        {
            return num;
        }

        template <typename = void>
        float to_value(float num)
        {
            return num;
        }

        template <typename = void>
        std::string to_value(const wchar_t *str)
        {
            return PivW2U{str}.str;
        }

        template <typename = void>
        ::piv::string_view to_value(const char *str)
        {
            return ::piv::string_view{str};
        }

        template <typename = void>
        std::string to_value(const CWString &str)
        {
            return PivW2U{str}.str;
        }

        template <typename = void>
        const std::string &to_value(const std::string &str)
        {
            return str;
        }

        template <typename = void>
        const ::piv::string_view &to_value(const ::piv::string_view &str)
        {
            return str;
        }

        template <typename = void>
        std::string to_value(const std::wstring &str)
        {
            return PivW2U{str}.str;
        }

        template <typename = void>
        std::string to_value(const ::piv::wstring_view &str)
        {
            return PivW2U{str.data(), str.size()}.str;
        }

        template <typename = void>
        const ::piv::string_view &to_value(const CVolMem &str)
        {
            return ::piv::string_view{reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())};
        }

        template <typename = void>
        PivJSON &to_value(PivJSON &value)
        {
            return value;
        }

        template <typename = void>
        PivJSON_Unordered &to_value(PivJSON_Unordered &value)
        {
            return value;
        }

        template <typename = void>
        const PivJSON &to_value(const PivJSON &value)
        {
            return value;
        }

        template <typename = void>
        const PivJSON_Unordered &to_value(const PivJSON_Unordered &value)
        {
            return value;
        }

        /**
         * @brief 转换到JSON索引或键名
         * @param idx 所欲转换的索引
         * @param key 所欲转换的键名
         * @return 返回兼容的JSON索引或键名
         */
        template <typename I, typename = std::enable_if<std::is_integral<I>::value>::type>
        typename std::make_unsigned<I>::type to_key(I idx)
        {
            return static_cast<typename std::make_unsigned<I>::type>(idx);
        }

        template <typename = void>
        std::string to_key(const wchar_t *key)
        {
            return PivW2U{key}.str;
        }

        template <typename = void>
        ::piv::string_view to_key(const char *key)
        {
            return ::piv::string_view{key};
        }

        template <typename = void>
        std::string to_key(const CWString &key)
        {
            return PivW2U{key}.str;
        }

        template <typename = void>
        const std::string &to_key(const std::string &key)
        {
            return key;
        }

        template <typename = void>
        const ::piv::string_view &to_key(const ::piv::string_view &key)
        {
            return key;
        }

        template <typename = void>
        std::string to_key(const std::wstring &key)
        {
            return PivW2U{key}.str;
        }

        template <typename = void>
        std::string to_key(const ::piv::wstring_view &key)
        {
            return PivW2U{key.data(), key.size()}.str;
        }

        template <typename = void>
        ::piv::string_view to_key(const CVolMem &key)
        {
            return ::piv::string_view{reinterpret_cast<const char *>(key.GetPtr()), static_cast<size_t>(key.GetSize())};
        }

        /**
         * @brief 转换到JSON Pointer
         * @param path 所欲转换的路径
         * @return 返回JSON Pointer
         */
        template <typename J, typename T>
        auto to_pointer(T &&path)
        {
            return typename J::json_pointer{PivAny2Us{path}.str};
        }

        /**
         * @brief 取成员
         * @param json JSON对象
         * @param idx 数组索引
         * @param key 对象键名
         * @param allow_exceptions 允许异常
         * @return 返回成员值参考
         */

        template <typename J, typename K>
        inline J &At(J &json, K &&key, bool allow_exceptions = false)
        {
            if (!allow_exceptions)
            {
                if (std::is_integral<typename std::decay<K>::type>::value)
                {
                    if (!json.is_array())
                        json = J::array();
                }
                else if (!json.is_object())
                {
                    json = J::object();
                }
            }
            return json[to_key(std::forward<K>(key))];
        }

        /**
         * @brief 数组加入成员
         * @param json JSON对象
         * @param val 成员值
         * @param ...args 可变长成员值
         */
        template <typename J, typename V>
        inline void PushBack(J &&json, V &&val)
        {
            json.push_back(to_value(val));
        }

        template <typename J, typename V, typename... Args>
        inline void PushBack(J &&json, V &&val, Args &&...args)
        {
            json.push_back(to_value(val));
            PushBack(json, std::forward<Args>(args)...);
        }

        /**
         * @brief 对象加入键值对
         * @param json JSON对象
         * @param key 键名
         * @param val 键置
         */
        template <typename J>
        inline void Emplace(J &&json)
        {
        }

        template <typename J, typename K>
        inline void Emplace(J &&json, K &&key)
        {
            json[to_key(std::forward<K>(key))] = nullptr;
        }

        template <typename J, typename K, typename V, typename... Args>
        inline void Emplace(J &&json, K &&key, V &&val, Args &&...args)
        {
            json[to_key(std::forward<K>(key))] = to_value(val);
            Emplace(json, std::forward<Args>(args)...);
        }

        /**
         * @brief 创建数组
         * @param ...args 可变长参数
         * @return 返回创建的数组
         */
        template <typename J, typename... Args>
        inline J CreateArray(Args &&...args)
        {
            J array = J::array();
            PushBack(array, std::forward<Args>(args)...);
            return array;
        }

        /**
         * @brief 设置数组
         * @param json JSON数组
         * @param ...args 可变长成员值
         * @return 返回自身
         */
        template <typename J, typename... Args>
        inline J &&SetArray(J &&json, Args &&...args)
        {
            json = std::decay<J>::type::array();
            PushBack(json, std::forward<Args>(args)...);
            return std::forward<J>(json);
        }

        /**
         * @brief 数组加入多个成员
         * @param json JSON对象
         * @param ...args 可变长成员值
         * @return 返回自身
         */
        template <typename J, typename... Args>
        inline J &&PushBackArgs(J &&json, Args &&...args)
        {
            PushBack(json, std::forward<Args>(args)...);
            return std::forward<J>(json);
        }

        /**
         * @brief 数组插入成员
         * @param json JSON对象
         * @param pos 插入位置
         * @param val 成员值
         * @param cnt 插入次数
         * @return 返回自身
         */

        template <typename J>
        inline J &&Insert(J &&json, ptrdiff_t pos, const J &val, size_t cnt = 1)
        {
            json.insert(json.begin() + pos, cnt, val);
            return std::forward<J>(json);
        }

        template <typename J, typename V>
        inline J &&Insert(J &&json, ptrdiff_t pos, const V &val, size_t cnt = 1)
        {
            json.insert(json.begin() + pos, cnt, to_value(val));
            return std::forward<J>(json);
        }

        /**
         * @brief 创建对象
         * @param ...args 可变长键值对
         * @return 返回所创建的JSON对象
         */
        template <typename J, typename... Args>
        inline J CreateObject(Args &&...args)
        {
            J json = J::object();
            Emplace(json, std::forward<Args>(args)...);
            return json;
        }

        /**
         * @brief 置对象
         * @param json JSON对象
         * @param ...args 可变长键值对
         * @return 返回自身
         */
        template <typename J, typename... Args>
        inline J &&SetObject(J &&json, Args &&...args)
        {
            json = std::decay<J>::type::object();
            Emplace(json, std::forward<Args>(args)...);
            return std::forward<J>(json);
        }

        /**
         * @brief 取成员Ex
         * @param json JSON对象
         * @return 返回成员的文本值
         */
        template <typename J>
        std::string GetString(const J &json)
        {
            switch (json.type())
            {
            case J::value_t::null:
            {
                return "null";
            }
            case J::value_t::object:
            {
                return json.dump(-1, ' ', false, J::error_handler_t::replace);
            }
            case J::value_t::array:
            {
                return json.dump(-1, ' ', false, J::error_handler_t::replace);
            }
            case J::value_t::string:
            {
                return json.get_ref<const std::string &>();
            }
            case J::value_t::boolean:
            {
                return json.get<bool>() ? "true" : "false";
            }
            case J::value_t::number_integer:
            {
                return std::to_string(json.get<int64_t>());
            }
            case J::value_t::number_unsigned:
            {
                return std::to_string(static_cast<int64_t>(json.get<uint64_t>()));
            }
            case J::value_t::number_float:
            {
                return std::to_string(json.get<double>());
            }
            case J::value_t::binary:
            {
                return json.dump(-1, ' ', false, J::error_handler_t::replace);
            }
            case J::value_t::discarded:
            {
                return "<discarded>";
            }
            default:
                return "";
            }
        }

        /**
         * @brief 取路径基本值
         * @param json JSON对象
         * @param path 路径
         * @param default_value 默认值(失败时返回)
         * @return 返回成员值
         */
        template <typename R, typename J, typename P>
        R ValuePath(const J &json, P &&path, const R &default_value)
        {
            try
            {
                return json.at(to_pointer<J>(std::forward<P>(path))).get<R>();
            }
            catch (const J::exception &e)
            {
                (void)e;
            }
            return default_value;
        }

        /**
         * @brief 取路径文本
         * @param json JSON对象
         * @param path 路径
         * @param default_value 默认值(失败时返回)
         * @return 返回成员值
         */
        template <typename J, typename P>
        CWString ValuePathStr(const J &json, P &&path, const CWString &default_value)
        {
            try
            {
                return PivU2Ws{json.at(to_pointer<J>(std::forward<P>(path))).get_ref<const std::string &>()}.str;
            }
            catch (const J::exception &e)
            {
                (void)e;
            }
            return default_value;
        }

        /**
         * @brief 取路径文本视图
         * @param json JSON对象
         * @param path 路径
         * @param default_value 默认值(失败时返回)
         * @return 返回成员值
         */
        template <typename J, typename P>
        ::piv::string_view ValuePathView(const J &json, P &&path, const std::string &default_value)
        {
            try
            {
                return ::piv::string_view{json.at(to_pointer<J>(std::forward<P>(path))).get_ref<const J::string_t &>()};
            }
            catch (const J::exception &e)
            {
                (void)e;
            }
            return ::piv::string_view{default_value};
        }

        /**
         * @brief 取路径对象
         * @param json JSON对象
         * @param path 路径
         * @return 返回JSON对象
         */
        template <typename J, typename P>
        J ValuePathObj(const J &json, P &&path)
        {
            try
            {
                auto ret = json.at(to_pointer<J>(std::forward<P>(path)));
                if (ret.is_object())
                    return ret;
            }
            catch (const J::exception &e)
            {
                (void)e;
            }
            return J::object();
        }

        /**
         * @brief 取路径数组
         * @param json JSON对象
         * @param path 路径
         * @return 返回JSON数组
         */
        template <typename J, typename P>
        J ValuePathArray(const J &json, P &&path)
        {
            try
            {
                auto ret = json.at(to_pointer<J>(std::forward<P>(path)));
                if (ret.is_array())
                    return ret;
            }
            catch (const J::exception &e)
            {
                (void)e;
            }
            return J::array();
        }

        /**
         * @brief 取成员值
         * @tparam R 返回类型
         * @param json JSON对象
         * @param key 键名或索引
         * @param default_value 默认值(失败时返回)
         * @return 返回成员值
         */
        template <typename R, typename J, typename K>
        R Value(const J &json, K &&key, const R &default_value)
        {
            try
            {
                return json.at(to_key(std::forward<K>(key))).get<R>();
            }
            catch (const J::exception &e)
            {
                (void)e;
            }
            return default_value;
        }

        /**
         * @brief 取成员文本
         * @param json JSON对象
         * @param key 键名或索引
         * @param default_value 默认值(失败时返回)
         * @return 返回成员值
         */
        template <typename J, typename K>
        std::string ValueStr(const J &json, K &&key, const std::string &default_value)
        {
            try
            {
                return json.at(to_key(std::forward<K>(key))).get_ref<const std::string &>();
            }
            catch (const J::exception &e)
            {
                (void)e;
            }
            return default_value;
        }

        template <typename J, typename K>
        CWString ValueStr(const J &json, K &&key, const CWString &default_value)
        {
            try
            {
                return PivU2Ws{json.at(to_key(std::forward<K>(key))).get_ref<const std::string &>()}.str;
            }
            catch (const J::exception &e)
            {
                (void)e;
            }
            return default_value;
        }

        /**
         * @brief 取成员对象
         * @param json JSON对象
         * @param key 键名或索引
         * @return 返回JSON对象
         */
        template <typename J, typename K>
        J ValueObj(const J &json, K &&key)
        {
            try
            {
                J ret = json.at(to_key(std::forward<K>(key)));
                if (ret.is_object())
                    return ret;
            }
            catch (typename J::exception &e)
            {
                (void)e;
            }
            return J::object();
        }

        /**
         * @brief 取成员数组
         * @param json JSON对象
         * @param key 键名或索引
         * @return 返回JSON数组
         */
        template <typename J, typename K>
        J ValueArray(const J &json, K &&key)
        {
            try
            {
                J ret = json.at(to_key(std::forward<K>(key)));
                if (ret.is_array())
                    return ret;
            }
            catch (const J::exception &e)
            {
                (void)e;
            }
            return J::array();
        }

        /**
         * @brief 数组索引置入成员值
         * @param json JSON对象
         * @param idx 数组索引
         * @param val 成员值
         * @param replace 强制置入
         * @return 返回是否置入成功
         */
        template <typename J, typename I, typename V>
        bool EmplaceAtIdx(J &&json, const I &idx, V &&val, bool replace = false)
        {
            try
            {
                if (!json.is_array() && !json.is_null())
                {
                    if (replace)
                        json = std::decay<J>::type::array();
                    else
                        return false;
                }
                json[idx] = val;
                return true;
            }
            catch (const typename std::decay<J>::type::exception &e)
            {
                (void)e;
                return false;
            }
        }

        /**
         * @brief 对象键名置入成员值
         * @param json JSON对象
         * @param key 对象键名
         * @param val 成员值
         * @param replace 强制置入
         * @return 返回是否置入成功
         */
        template <typename J, typename K, typename V>
        bool EmplaceAtKey(J &&json, K &&key, V &&val, bool replace = false)
        {
            try
            {
                if (!json.is_object() && !json.is_null())
                {
                    if (replace)
                        json = std::decay<J>::type::object();
                    else
                        return false;
                }
                json[std::forward<K>(key)] = val;
                return true;
            }
            catch (const typename std::decay<J>::type::exception &e)
            {
                (void)e;
                return false;
            }
        }

        /**
         * @brief 路径置入成员值
         * @param json JSON对象
         * @param path JSON路径
         * @param val 成员值
         * @return 返回是否置入成功
         */
        template <typename J, typename P, typename V>
        bool EmplaceAtPath(J &&json, P &&path, V &&val)
        {
            try
            {
                json[std::forward<P>(path)] = val;
                return true;
            }
            catch (const typename std::decay<J>::type::exception &e)
            {
                (void)e;
                return false;
            }
        }

        /**
         * @brief 置入成员值(通用)
         * @param json JSON对象
         * @param idx 数组索引
         * @param key 对象键名
         * @param path JSON路径
         * @param val 成员值
         * @param replace 强制置入
         * @return 返回是否置入成功
         */
        template <typename J, typename V>
        inline bool EmplaceAt(J &&json, int32_t idx, V &&val, bool replace = false)
        {
            return EmplaceAtIdx(json, static_cast<uint32_t>(idx), to_value(val), replace);
        }

        template <typename J, typename V>
        inline bool EmplaceAt(J &&json, int64_t idx, V &&val, bool replace = false)
        {
            return EmplaceAtIdx(json, static_cast<uint64_t>(idx), to_value(val), replace);
        }

        template <typename J, typename V>
        inline bool EmplaceAt(J &&json, const wchar_t *path, V &&val, bool replace = false)
        {
            if (path[0] == '/')
                return EmplaceAtPath(json, std::decay<J>::type::json_pointer{PivW2U{path}.str}, to_value(val));
            else
                return EmplaceAtKey(json, to_key(path), to_value(val), replace);
        }

        template <typename J, typename V>
        inline bool EmplaceAt(J &&json, const char *path, V &&val, bool replace = false)
        {
            if (path[0] == '/')
                return EmplaceAtPath(json, std::decay<J>::type::json_pointer{path}, to_value(val));
            else
                return EmplaceAtKey(json, to_key(path), to_value(val), replace);
        }

        template <typename J, typename V>
        inline bool EmplaceAt(J &&json, const CWString &path, V &&val, bool replace = false)
        {
            if (path.IsEmpty())
                return false;
            if (path.GetCharAt(0) == '/')
                return EmplaceAtPath(json, std::decay<J>::type::json_pointer{PivW2U{path}.str}, to_value(val));
            else
                return EmplaceAtKey(json, to_key(path), to_value(val), replace);
        }

        template <typename J, typename V>
        inline bool EmplaceAt(J &&json, const std::string &path, V &&val, bool replace = false)
        {
            if (path.empty())
                return false;
            if (path.front() == '/')
                return EmplaceAtPath(json, std::decay<J>::type::json_pointer{path}, to_value(val));
            else
                return EmplaceAtKey(json, to_key(path), to_value(val), replace);
        }

        template <typename J, typename V>
        inline bool EmplaceAt(J &&json, const std::wstring &path, V &&val, bool replace = false)
        {
            if (path.empty())
                return false;
            if (path.front() == '/')
                return EmplaceAtPath(json, std::decay<J>::type::json_pointer{PivW2U{path}.str}, to_value(val));
            else
                return EmplaceAtKey(json, to_key(path), to_value(val), replace);
        }

        template <typename J, typename V>
        inline bool EmplaceAt(J &&json, const ::piv::string_view &path, V &&val, bool replace = false)
        {
            if (path.empty())
                return false;
            if (path.front() == '/')
                return EmplaceAtPath(json, std::decay<J>::type::json_pointer{path.data()}, to_value(val));
            else
                return EmplaceAtKey(json, to_key(path), to_value(val), replace);
        }

        template <typename J, typename V>
        inline bool EmplaceAt(J &&json, const ::piv::wstring_view &path, V &&val, bool replace = false)
        {
            if (path.empty())
                return false;
            if (path.front() == '/')
                return EmplaceAtPath(json, std::decay<J>::type::json_pointer{PivW2U{path}.str}, to_value(val));
            else
                return EmplaceAtKey(json, to_key(path), to_value(val), replace);
        }

        template <typename J, typename V>
        inline bool EmplaceAt(J &&json, const CVolMem &path, V &&val, bool replace = false)
        {
            if (path.IsEmpty())
                return false;
            if (path.Get_S_BYTE(0) == '/')
                return EmplaceAtPath(json, std::decay<J>::type::json_pointer{PivW2U{path}.str}, to_value(val));
            else
                return EmplaceAtKey(json, to_key(path), to_value(val), replace);
        }

        /**
         * @brief 取所有键名
         * @param json JSON对象
         * @param keyArray 键名数组
         * @return 键数量
         */
        template <typename J>
        int32_t EnumKey(const J &json, CMStringArray &keyArray)
        {
            keyArray.RemoveAll();
            if (json.is_object())
            {
                for (auto &el : json.items())
                {
                    keyArray.Add(PivU2W{el.key()}.c_str());
                }
            }
            return static_cast<int32_t>(keyArray.GetCount());
        }

        template <typename J>
        int32_t EnumKey(const J &json, std::vector<::piv::string_view> &keyArray)
        {
            keyArray.clear();
            if (json.is_object())
            {
                for (auto &el : json.items())
                {
                    keyArray.push_back(::piv::string_view{el.key()});
                }
            }
            return static_cast<int32_t>(keyArray.size());
        }

    } // namespace json
} // namespace piv

#endif // _PIV_NLOHMANN_JSON_H
