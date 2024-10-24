/*********************************************\
 * 火山视窗PIV模块 - TOML++                  *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef _PIV_TOML_PLUS_PLUS_HPP
#define _PIV_TOML_PLUS_PLUS_HPP

#if _MSVC_LANG < 201703L
#error TOML++需要 C++17 或更高标准,请使用 Visual Studio 2017 以上版本的编译器.
#endif

// 包含TOML++ https://github.com/marzer/tomlplusplus
// 禁用TOML++的异常
#ifndef TOML_EXCEPTIONS
#define TOML_EXCEPTIONS 0
#else
#undef TOML_EXCEPTIONS
#define TOML_EXCEPTIONS 0
#endif

// 启用TOML的UTF-16支持
#ifndef TOML_ENABLE_WINDOWS_COMPAT
#define TOML_ENABLE_WINDOWS_COMPAT 1
#else
#undef TOML_ENABLE_WINDOWS_COMPAT
#define TOML_ENABLE_WINDOWS_COMPAT 1
#endif

// 因为TOML++的源码里有个变量跟火山的inline_宏重名,这里要临时取消定义
#ifdef inline_
#undef inline_
#endif
#include "toml.hpp"
#ifndef inline_
#define inline_ inline
#endif

// 包含文本编码转换包装类
#include "piv_string.hpp"

namespace piv
{
    namespace toml
    {
        /**
         * @brief 解析文件
         * @param file_path 文件路径
         * @return 是否解析成功
         */
        template <typename T>
        bool ParseFile(::toml::table &tbl, const T &file_path)
        {
            ::toml::parse_result result = ::toml::parse_file(*PivS2V{file_path});
            if (result)
            {
                tbl = std::move(result.table());
                return true;
            }
            tbl.clear();
            return false;
        }

        /**
         * @brief 解析文本
         * @param doc 文本内容
         * @param source_path 来源路径
         * @return 是否解析成功
         */
        template <typename D, typename S>
        bool Parse(::toml::table &tbl, const D &doc, const S &source_path)
        {
            ::toml::parse_result result = ::toml::parse(*PivS2V{doc}, *PivS2V{source_path});
            if (result)
            {
                tbl = std::move(result.table());
                return true;
            }
            tbl.clear();
            return false;
        }

        // 取基本值
        template <typename R, typename K>
        R GetAs(const ::toml::table &tbl, const K &key)
        {
            auto val = tbl.get_as<R>(*PivS2V{key});
            if (val)
                return **val;
            return R{};
        }

        // 取键值表
        template <typename K>
        ::toml::table &GetTable(::toml::table &tbl, const K &key, ::toml::table &def_val)
        {
            auto val = tbl.get_as<::toml::table>(*PivS2V{key});
            if (val)
                return const_cast<::toml::table &>(*val);
            return def_val;
        }

        template <typename K>
        auto GetTable(::toml::table &tbl, const K &key, ::toml::table &&def_val = ::toml::table{})
        {
            auto val = tbl.get_as<::toml::table>(*PivS2V{key});
            if (val)
                return *val;
            return std::forward<::toml::table &&>(def_val);
        }

        // 取表数组
        template <typename K>
        ::toml::array &GetArr(::toml::table &tbl, const K &key, ::toml::array &def_val)
        {
            auto val = tbl.get_as<::toml::array>(*PivS2V{key});
            if (val)
                return *val;
            return def_val;
        }

        template <typename K>
        auto GetArr(::toml::table &tbl, const K &key, ::toml::array &&def_val = ::toml::array{})
        {
            auto val = tbl.get_as<::toml::array>(*PivS2V{key});
            if (val)
                return *val;
            return std::forward<::toml::array &&>(def_val);
        }
    }
}

#endif // _PIV_TOML_PLUS_PLUS_HPP
