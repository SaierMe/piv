/*********************************************\
 * 火山视窗PIV模块 - nlohmann JSON           *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 版本: 2023/01/09                          *
\*********************************************/

#ifndef _PIV_NLOHMANN_JSON_H
#define _PIV_NLOHMANN_JSON_H

// 包含火山视窗基本类,它在火山里的包含顺序比较靠前(全局-110)
#ifndef __VOL_BASE_H__
#include <sys/base/libs/win_base/vol_base.h>
#endif

// 火山的_CT宏跟chrono冲突,需要临时取消定义
#ifdef _CT
#undef _CT
#endif

// 包含json
#include "json.hpp"

// 重新定义_CT宏
#ifndef _CT
#define _CT(x) CVolConstString(_T(x))
#endif

#include "piv_string.hpp"

using PivJSON = nlohmann::basic_json<>;
using PivOrderJSON = nlohmann::basic_json<nlohmann::ordered_map>;

using PivJSON_exception = PivJSON::exception;
using PivJSON_parse_error = PivJSON::parse_error;
using PivJSON_invalid_iterator = PivJSON::invalid_iterator;
using PivJSON_type_error = PivJSON::type_error;
using PivJSON_out_of_range = PivJSON::out_of_range;
using PivJSON_other_error = PivJSON::other_error;

namespace piv
{
    namespace json
    {
        // 是否可解析
        template <typename J>
        inline bool accept(const CVolString &input, const CVolMem &json_str, const bool ignore_comments, const int32_t encoding)
        {
            if (!input.IsEmpty())
            {
                CVolString json_test = input;
                json_test.TrimAll();
                if (json_test.LeadOf(L'{') || json_test.LeadOf(L'[')) // JSON文本
                {
                    return J::accept(json_test.GetText(), json_test.GetText() + json_test.GetLength(), ignore_comments);
                }
                else // JSON文件
                {
                    FILE *fp = _wfopen(json_test.GetText(), L"r");
                    if (!fp)
                    {
                        return false;
                    }
                    bool ret = J::accept(fp, ignore_comments);
                    fclose(fp);
                    return ret;
                }
            }
            if (!json_str.IsEmpty())
            {
                const char *pstr = reinterpret_cast<const char *>(json_str.GetPtr());
                size_t len = static_cast<size_t>(json_str.GetSize());
#ifdef PIV_ENABLE_SIMDUTF
                int str_encoding = simdutf::detect_encodings(pstr, len);
#else
                int str_encoding = encoding;
#endif
                if ((str_encoding & 8) == 8) // UTF-32LE
                {
                    return J::accept(reinterpret_cast<const char32_t *>(pstr), reinterpret_cast<const char32_t *>(pstr) + len / 4, ignore_comments);
                }
                else if ((str_encoding & 2) == 2) // UTF-16LE
                {
                    return J::accept(reinterpret_cast<const wchar_t *>(pstr), reinterpret_cast<const wchar_t *>(pstr) + len / 2, ignore_comments);
                }
                else if ((str_encoding & 1) == 1) // UTF-8
                {
                    return J::accept(pstr, pstr + len, ignore_comments);
                }
                else if (str_encoding == 0) // ANSI
                {
                    return J::accept(PivA2U(json_str).String(), ignore_comments);
                }
            }
            return false;
        }

        // 解析
        template <typename J>
        inline bool parse(J &json, CVolString &input, const bool allow_exceptions, const bool ignore_comments)
        {
            json = J::parse(input.GetText(), input.GetText() + input.GetLength(), nullptr, allow_exceptions, ignore_comments);
            return !json.is_discarded();
        }

        // 解析UTF8
        template <typename J>
        inline bool parse(J &json, CVolMem &input, const bool allow_exceptions, const bool ignore_comments)
        {
            json = J::parse(reinterpret_cast<const char *>(input.GetPtr()), reinterpret_cast<const char *>(input.GetEndPtr()), nullptr, allow_exceptions, ignore_comments);
            return !json.is_discarded();
        }

        // 解析字节集
        template <typename J>
        inline bool parse_detect(J &json, CVolMem &input, const bool allow_exceptions, const bool ignore_comments, const int32_t encoding)
        {
            const char *pstr = reinterpret_cast<const char *>(input.GetPtr());
            size_t len = static_cast<size_t>(input.GetSize());
#ifdef PIV_ENABLE_SIMDUTF
            int str_encoding = simdutf::detect_encodings(pstr, len);
#else
            int str_encoding = encoding;
#endif
            if ((str_encoding & 8) == 8) // UTF-32LE
            {
                json = J::parse(reinterpret_cast<const char32_t *>(pstr), reinterpret_cast<const char32_t *>(pstr) + len / 4, nullptr, allow_exceptions, ignore_comments);
            }
            else if ((str_encoding & 2) == 2) // UTF-16LE
            {
                json = J::parse(reinterpret_cast<const wchar_t *>(pstr), reinterpret_cast<const wchar_t *>(pstr) + len / 2, nullptr, allow_exceptions, ignore_comments);
            }
            else if ((str_encoding & 1) == 1) // UTF-8
            {
                json = J::parse(pstr, pstr + len, nullptr, allow_exceptions, ignore_comments);
            }
            else if (str_encoding == 0) // ANSI
            {
                json = J::parse(PivA2U(reinterpret_cast<const char *>(input.GetPtr()), static_cast<size_t>(input.GetSize())).String(),
                                nullptr, allow_exceptions, ignore_comments);
            }
            return !json.is_discarded();
        }

        // 解析文件
        template <typename J>
        inline bool parse(J &json, const wchar_t *input, const bool allow_exceptions, const bool ignore_comments)
        {
            FILE *fp = _wfopen(input, L"r");
            if (!fp)
            {
                return false;
            }
            json = J::parse(fp, nullptr, allow_exceptions, ignore_comments);
            fclose(fp);
            return !json.is_discarded();
        }

        // 解析字节集BJDATA
        template <typename J>
        inline bool from_bjdata(J &json, const CVolMem &input, const bool strict, const bool allow_exceptions)
        {
            json = J::from_bjdata(input.GetPtr(), input.GetEndPtr(), strict, allow_exceptions);
            return !json.is_discarded();
        }

        // 解析字节集BSON
        template <typename J>
        inline bool from_bson(J &json, const CVolMem &input, const bool strict, const bool allow_exceptions)
        {
            json = J::from_bson(input.GetPtr(), input.GetEndPtr(), strict, allow_exceptions);
            return !json.is_discarded();
        }

        // 解析字节集CBOR
        template <typename J>
        inline bool from_cbor(J &json, const CVolMem &input, const bool strict, const bool allow_exceptions, const int32_t tag_handler)
        {
            json = J::from_cbor(input.GetPtr(), input.GetEndPtr(), strict, allow_exceptions, static_cast<PivJSON::cbor_tag_handler_t>(tag_handler));
            return !json.is_discarded();
        }

        // 解析字节集MSGPACK
        template <typename J>
        inline bool from_msgpack(J &json, const CVolMem &input, const bool strict, const bool allow_exceptions)
        {
            json = J::from_msgpack(input.GetPtr(), input.GetEndPtr(), strict, allow_exceptions);
            return !json.is_discarded();
        }

        // 解析字节集UBJSON
        template <typename J>
        inline bool from_ubjson(J &json, const CVolMem &input, const bool strict, const bool allow_exceptions)
        {
            json = J::from_ubjson(input.GetPtr(), input.GetEndPtr(), strict, allow_exceptions);
            return !json.is_discarded();
        }

        // 到文本、到字节集
        template <typename R, typename J>
        inline R dump_utf16(J &json, const int32_t indent, const char indent_char, const bool ensure_ascii, const int32_t error_handler)
        {
            std::string str = json.dump(indent, indent_char, ensure_ascii, static_cast<PivJSON::error_handler_t>(error_handler));
            return PivU2W(str);
        }

        // 到UTF8
        template <typename J>
        inline CVolMem dump(J &json, const int32_t indent, const char indent_char, const bool ensure_ascii, const int32_t error_handler, const bool null_char)
        {
            std::string str = json.dump(indent, indent_char, ensure_ascii, static_cast<PivJSON::error_handler_t>(error_handler));
            return CVolMem(str.c_str(), str.size() + (null_char ? 1 : 0));
        }

        // 到多字节
        template <typename J>
        inline CVolMem dump_ansi(J &json, const int32_t indent, const char indent_char, const bool ensure_ascii, const int32_t error_handler, const bool null_char)
        {
            std::string str = json.dump(indent, indent_char, ensure_ascii, static_cast<PivJSON::error_handler_t>(error_handler));
            CVolMem mbsmem;
            PivU2W mbs{str};
            mbs.GetMem(mbsmem, true);
            return mbsmem;
        }

        // 到BJDATA
        template <typename J>
        inline CVolMem to_bjdata(J &json, const bool use_size, const bool use_type)
        {
            std::vector<std::uint8_t> bin;
            J::to_bjdata(json, bin, use_size, use_type);
            return CVolMem(bin.data(), bin.size());
        }

        // 到BSON
        template <typename J>
        inline CVolMem to_bson(J &json)
        {
            std::vector<std::uint8_t> bin;
            J::to_bson(json, bin);
            return CVolMem(bin.data(), bin.size());
        }

        // 到CBOR
        template <typename J>
        inline CVolMem to_cbor(J &json)
        {
            std::vector<std::uint8_t> bin;
            J::to_cbor(json, bin);
            return CVolMem(bin.data(), bin.size());
        }

        // 到MsgPack
        template <typename J>
        inline CVolMem to_msgpack(J &json)
        {
            std::vector<std::uint8_t> bin;
            J::to_msgpack(json, bin);
            return CVolMem(bin.data(), bin.size());
        }

        // 到UBJSON
        template <typename J>
        inline CVolMem to_ubjson(J &json, const bool use_size, const bool use_type)
        {
            std::vector<std::uint8_t> bin;
            J::to_ubjson(json, bin, use_size, use_type);
            return CVolMem(bin.data(), bin.size());
        }

        // 变参模板: 对象添加键值对无效
        template <typename J>
        inline void set_obj_args(J &json, const char *typeNames)
        {
        }

        // 变参模板: 对象添加空值键值对
        template <typename J, typename K>
        inline void set_obj_args(J &json, const char *typeNames, K key)
        {
            PivJSON j;
            json[*PivW2U(key)] = nullptr;
        }

        // 变参模板: 对象添加文本值键值对
        template <typename J, typename... Args>
        inline void set_obj_args(J &json, const char *typeNames, const wchar_t *key, const wchar_t *val, Args... args)
        {
            ++typeNames;
            json[*PivW2U(key)] = *PivW2U(val);
            set_obj_args(json, ++typeNames, args...);
        }

        // 变参模板: 对象添加数值键值对
        template <typename J, typename V, typename... Args>
        inline void set_obj_args(J &json, const char *typeNames, const wchar_t *key, V val, Args... args)
        {
            ++typeNames;
            if (*typeNames == 'B')
            {
                json[*PivW2U(key)] = static_cast<bool>(val);
            }
            else
            {
                json[*PivW2U(key)] = val;
            }
            set_obj_args(json, ++typeNames, args...);
        }

        // 创建对象
        template <typename J, typename... Args>
        inline J new_object(const char *typeNames, Args... args)
        {
            J json = J::object();
            set_obj_args(json, typeNames, args...);
            return json;
        }

        // 设置对象
        template <typename J, typename... Args>
        inline J &set_object(J &json, const char *typeNames, Args... args)
        {
            json = J::object();
            set_obj_args(json, ++typeNames, args...);
            return json;
        }

        // 变参模板: 数组加入无效成员
        template <typename J>
        inline void push_back_args(J &json, const char *typeNames)
        {
        }

        // 变参模板: 数组加入文本成员
        template <typename J, typename... Args>
        inline void push_back_args(J &json, const char *typeNames, const wchar_t *head, Args... args)
        {
            json.push_back(*PivW2U(head));
            push_back_args(json, ++typeNames, args...);
        }

        // 变参模板: 数组加入数值成员
        template <typename J, typename T, typename... Args>
        inline void push_back_args(J &json, const char *typeNames, T head, Args... args)
        {
            if (*typeNames == 'B')
            {
                json.push_back(static_cast<bool>(head));
            }
            else
            {
                json.push_back(head);
            }
            push_back_args(json, ++typeNames, args...);
        }

        // 创建数组
        template <typename J, typename... Args>
        inline J new_array(const char *typeNames, Args... args)
        {
            J jsonArray = J::array();
            push_back_args(jsonArray, typeNames, args...);
            return jsonArray;
        }

        // 设置数组
        template <typename J, typename... Args>
        inline J &set_array(J &json, const char *typeNames, Args... args)
        {
            json = J::array();
            push_back_args(json, ++typeNames, args...);
            return json;
        }

        // 成员
        template <typename J>
        inline J &get_at(J &json, const int32_t idx, const bool allow_exceptions)
        {
            if (!allow_exceptions && !json.is_array())
            {
                json = J::array();
            }
            return json[(std::max)(0, idx)];
        }
        template <typename J>
        inline J &get_at(J &json, const wchar_t *key, const bool allow_exceptions)
        {
            if (!allow_exceptions && !json.is_object())
            {
                json = J::object();
            }
            return json[*PivW2U(key)];
        }

        // 取成员
        template <typename J>
        inline J &at(J &json, const int32_t idx)
        {
            return json.at((std::max)(0, idx));
        }
        template <typename J>
        inline J &at(J &json, const wchar_t *key)
        {
            return json.at(*PivW2U(key));
        }

        // 加入成员
        template <typename J>
        inline J &push_back(J &json, char tName, const wchar_t *val)
        {
            json.push_back(*PivW2U(val));
            return json;
        }
        template <typename J, typename T>
        inline J &push_back(J &json, char tName, const T &val)
        {
            if (tName == 'B')
            {
                json.push_back(static_cast<bool>(val));
            }
            else
            {
                json.push_back(val);
            }
            return json;
        }

        // 插入成员
        template <typename J>
        inline J &insert(J &json, char tName, int32_t pos, const wchar_t *val)
        {
            json.insert(json.begin() + pos, *PivW2U(val));
            return json;
        }
        template <typename J, typename T>
        inline J &insert(J &json, char tName, int32_t pos, const T &val)
        {
            if (tName == 'B')
            {
                json.insert(json.begin() + pos, static_cast<bool>(val));
            }
            else
            {
                json.insert(json.begin() + pos, val);
            }
            return json;
        }

        // 取文本Ex
        template <typename J>
        inline CVolString get_string(const J &json)
        {
            PivJSON::value_t type = json.type();
            switch (type)
            {
            case PivJSON::value_t::null:
            {
                return _CT("null");
            }
            case PivJSON::value_t::object:
            {
                return dump_utf16<CVolString>(json, -1, ' ', false, 1);
            }
            case PivJSON::value_t::array:
            {
                return dump_utf16<CVolString>(json, -1, ' ', false, 1);
            }
            case PivJSON::value_t::string:
            {
                return PivU2W(json.get<std::string>());
            }
            case PivJSON::value_t::boolean:
            {
                return json.get<bool>() ? _CT("true") : _CT("false");
            }
            case PivJSON::value_t::number_integer:
            {
                return CVolString(json.get<std::int64_t>());
            }
            case PivJSON::value_t::number_unsigned:
            {
                return CVolString(static_cast<INT64>(json.get<std::uint64_t>()));
            }
            case PivJSON::value_t::number_float:
            {
                return CVolString(json.get<double>());
            }
            case PivJSON::value_t::binary:
            {
                return dump_utf16<CVolString>(json, -1, ' ', false, 1);
            }
            case PivJSON::value_t::discarded:
            {
                return _CT("<discarded>");
            }
            default:
                return _CT("");
            }
        }

        // 删除成员
        template <typename J>
        inline size_t erase(J &json, const int32_t idx)
        {
            json.erase((std::max)(0, idx));
            return 0;
        }
        template <typename J>
        inline size_t erase(J &json, const wchar_t *key)
        {
            return json.erase(*PivW2U(key));
        }

        // 取路径文本值
        template <typename J>
        inline CVolString str_value_path(const J &json, const wchar_t *json_pointer, const wchar_t *default_value)
        {
            try
            {
                return PivU2W(json.at(J::json_pointer(*PivW2U(json_pointer))).get<std::string>());
            }
            catch (PivJSON_exception &)
            {
                return CVolString(default_value);
            }
        }

        // 取路径基本值
        template <typename ReturnType, typename J>
        inline ReturnType value_path(const J &json, const wchar_t *json_pointer, const ReturnType &default_value)
        {
            try
            {
                return json.at(J::json_pointer(*PivW2U(json_pointer))).get<ReturnType>();
            }
            catch (PivJSON_exception &)
            {
                return default_value;
            }
        }

        // 取路径对象
        template <typename J>
        inline J obj_value_path(J &json, const wchar_t *json_pointer)
        {
            try
            {
                J::json_pointer ptr(*PivW2U(json_pointer));
                if (json.at(ptr).is_object())
                {
                    return json.at(ptr);
                }
            }
            catch (PivJSON_exception &)
            {
            }
            return J::object();
        }

        // 取路径数组
        template <typename J>
        inline J arr_value_path(J &json, const wchar_t *json_pointer)
        {
            try
            {
                J::json_pointer ptr(*PivW2U(json_pointer));
                if (json.at(ptr).is_array())
                {
                    return json.at(ptr);
                }
            }
            catch (PivJSON_exception &)
            {
            }
            return J::array();
        }

        // 取成员文本值
        template <typename J>
        inline CVolString str_value(const J &json, const wchar_t *key, const wchar_t *default_value)
        {
            try
            {
                return PivU2W(json.at(*PivW2U(key)).get<std::string>());
            }
            catch (PivJSON_exception &)
            {
                return CVolString(default_value);
            }
        }
        template <typename J>
        inline CVolString str_value(const J &json, const int32_t idx, const wchar_t *default_value)
        {
            try
            {
                return PivU2W(json.at(idx).get<std::string>());
            }
            catch (PivJSON_exception &)
            {
                return CVolString(default_value);
            }
        }

        // 取成员基本值
        template <typename ReturnType, typename J>
        inline ReturnType value(const J &json, const wchar_t *key, const ReturnType &default_value)
        {
            try
            {
                return json.at(*PivW2U(key)).get<ReturnType>();
            }
            catch (PivJSON_exception &)
            {
                return default_value;
            }
        }
        template <typename ReturnType, typename J>
        inline ReturnType value(const J &json, const int32_t idx, const ReturnType &default_value)
        {
            try
            {
                return json.at(idx).get<ReturnType>();
            }
            catch (PivJSON_exception &)
            {
                return default_value;
            }
        }

        // 取成员对象
        template <typename J>
        inline J obj_value(J &json, const wchar_t *key)
        {
            try
            {
                PivW2U utf8key(key);
                if (json.at(*utf8key).is_object())
                {
                    return json.at(*utf8key);
                }
            }
            catch (PivJSON_exception &)
            {
            }
            return J::object();
        }
        template <typename J>
        inline J obj_value(J &json, const int32_t idx)
        {
            try
            {
                if (json.at(idx).is_object())
                {
                    return json.at(idx);
                }
            }
            catch (PivJSON_exception &)
            {
            }
            return J::object();
        }

        // 取成员数组
        template <typename J>
        inline J arr_value(J &json, const wchar_t *key)
        {
            try
            {
                PivW2U utf8key{key};
                if (json.at(*utf8key).is_array())
                {
                    return json.at(*utf8key);
                }
            }
            catch (PivJSON_exception &)
            {
            }
            return J::array();
        }
        template <typename J>
        inline J arr_value(J &json, const int32_t idx)
        {
            try
            {
                if (json.at(idx).is_array())
                {
                    return json.at(idx);
                }
            }
            catch (PivJSON_exception &)
            {
            }
            return J::array();
        }

        // 置入文本值
        template <typename J>
        inline bool emplace_str(J &json, const wchar_t *path, const wchar_t *value, const bool replace)
        {
            try
            {
                if (path[0] == L'/')
                {
                    json[J::json_pointer(*PivW2U(path))] = *PivW2U(value);
                }
                else
                {
                    if (!json.is_object() && !json.is_null())
                    {
                        if (replace)
                            json = J::object();
                        else
                            return false;
                    }
                    json[*PivW2U(path)] = *PivW2U(value);
                    return true;
                }
            }
            catch (PivJSON_exception &)
            {
                return false;
            }
        }
        template <typename J>
        inline bool emplace_str(J &json, const int32_t idx, const wchar_t *value, const bool replace)
        {
            try
            {
                if (!json.is_array() && !json.is_null())
                {
                    if (replace)
                        json = J::array();
                    else
                        return false;
                }
                json[idx] = *PivW2U(value);
                return true;
            }
            catch (PivJSON_exception &)
            {
                return false;
            }
        }
        // 置入基本值
        template <typename V, typename J>
        inline bool emplace(J &json, const wchar_t *path, const V &value, const bool replace)
        {
            try
            {
                if (path[0] == L'/')
                {
                    json[J::json_pointer(*PivW2U(path))] = static_cast<V>(value);
                }
                else
                {
                    if (!json.is_object() && !json.is_null())
                    {
                        if (replace)
                            json = J::object();
                        else
                            return false;
                    }
                    json[*PivW2U(path)] = static_cast<V>(value);
                    return true;
                }
            }
            catch (PivJSON_exception &)
            {
                return false;
            }
        }
        template <typename V, typename J>
        inline bool emplace(J &json, const int32_t idx, const V &value, const bool replace)
        {
            try
            {
                if (!json.is_array() && !json.is_null())
                {
                    if (replace)
                        json = J::array();
                    else
                        return false;
                }
                json[idx] = static_cast<V>(value);
                return true;
            }
            catch (PivJSON_exception &)
            {
                return false;
            }
        }

        // 置入对象值
        template <typename J>
        inline bool emplace_obj(J &json, const wchar_t *path, const J &value, const bool replace)
        {
            if (!value.is_object())
            {
                return false;
            }
            try
            {
                if (path[0] == L'/')
                {
                    json[J::json_pointer(*PivW2U(path))] = value;
                }
                else
                {
                    if (!json.is_object() && !json.is_null())
                    {
                        if (replace)
                            json = J::object();
                        else
                            return false;
                    }
                    json[*PivW2U(path)] = value;
                    return true;
                }
            }
            catch (PivJSON_exception &)
            {
                return false;
            }
        }
        template <typename V, typename J>
        inline bool emplace_obj(J &json, const int32_t idx, const V &value, const bool replace)
        {
            if (!value.is_object())
            {
                return false;
            }
            try
            {
                if (!json.is_array() && !json.is_null())
                {
                    if (replace)
                        json = J::array();
                    else
                        return false;
                }
                json[idx] = value;
                return true;
            }
            catch (PivJSON_exception &)
            {
                return false;
            }
        }

        // 置入数组值
        template <typename J>
        inline bool emplace_arr(J &json, const wchar_t *path, const J &value, const bool replace)
        {
            if (!value.is_array)
            {
                return false;
            }
            try
            {
                if (path[0] == L'/')
                {
                    json[J::json_pointer(*PivW2U(path))] = value;
                }
                else
                {
                    if (!json.is_object() && !json.is_null())
                    {
                        if (replace)
                            json = J::object();
                        else
                            return false;
                    }
                    json[*PivW2U(path)] = value;
                    return true;
                }
            }
            catch (PivJSON_exception &)
            {
                return false;
            }
        }
        template <typename V, typename J>
        inline bool emplace_arr(J &json, const int32_t idx, const V &value, const bool replace)
        {
            if (!value.is_array)
            {
                return false;
            }
            try
            {
                if (!json.is_array() && !json.is_null())
                {
                    if (replace)
                        json = J::array();
                    else
                        return false;
                }
                json[idx] = value;
                return true;
            }
            catch (PivJSON_exception &)
            {
                return false;
            }
        }

        // 取所有键
        template <typename J>
        int32_t enum_key(const J &json, CMStringArray &keyArray)
        {
            keyArray.RemoveAll();
            if (json.is_object())
            {
                for (auto &el : json.items())
                {
                    keyArray.Add(PivU2W(el.key()).GetText());
                }
                return static_cast<int32_t>(keyArray.GetCount());
            }
            return 0;
        }

    } // namespace json
} // namespace piv

#endif // _PIV_NLOHMANN_JSON_H
