/*********************************************\
 * 火山视窗PIV模块 - TOML++                  *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 版本: 2022/11/26                          *
\*********************************************/

#ifndef _PIV_TOML_PLUS_PLUS_HPP
#define _PIV_TOML_PLUS_PLUS_HPP

// 包含火山视窗基本类,它在火山里的包含顺序比较靠前(全局-110)
#ifndef __VOL_BASE_H__
#include <sys/base/libs/win_base/vol_base.h>
#endif

#if _MSVC_LANG < 201703L
#error TOML++需要 C++17 或更高标准,请使用 Visual Studio 2017 以上版本的编译器.
#endif

// 开始包含TOML++ https://github.com/marzer/tomlplusplus
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
// 结束包含TOML++

// 包含文本编码转换包装类
#include "piv_encoding.hpp"


/**
 * @brief TOML键值表类
 */
class PivTomlTable : public CVolObject
{
protected:
    toml::table tbl{};

public:
    PivTomlTable() {}
    PivTomlTable(const toml::table &other) : tbl{other} {}
    PivTomlTable(toml::table &&other) : tbl{other} {}
    PivTomlTable(toml::table *other)
    {
        if (other != nullptr)
        {
            tbl = *other;
        }
        else
        {
            tbl.clear();
        }
    }
    ~PivTomlTable()
    {
    }
    PivTomlTable &operator=(PivTomlTable &other)
    {
        tbl = other.tbl;
        return *this;
    }
    bool operator==(PivTomlTable &other)
    {
        return tbl == other.tbl;
    }
    virtual void GetDumpString(CVolString &strDump, INT nMaxDumpSize) override
    {
        strDump.AddText(this->ToString());
    }
    CVolString ToString()
    {
        std::stringstream ss;
        ss << tbl;
        return PivU2W(ss.str());
    }
    // 解析文件
    bool ParseFile(const wchar_t *file_path)
    {
        toml::parse_result result = toml::parse_file(file_path);
        if (result)
        {
            tbl = std::move(result).table();
            return true;
        }
        tbl.clear();
        return false;
    }
    // 解析文本
    bool Parse(const wchar_t *doc, const wchar_t *source_path)
    {
        toml::parse_result result = toml::parse(PivW2U(doc).String(), source_path);
        if (result)
        {
            tbl = std::move(result).table();
            return true;
        }
        tbl.clear();
        return false;
    }
    // 解析字节集(UTF-8)
    bool Parse(CVolMem &doc, const wchar_t *source_path)
    {
        std::string_view doc_sv{reinterpret_cast<const char *>(doc.GetPtr()), static_cast<size_t>(doc.GetSize())};
        toml::parse_result result = toml::parse(doc_sv, source_path);
        if (result)
        {
            tbl = std::move(result).table();
            return true;
        }
        tbl.clear();
        return false;
    }
    // 是否为空
    bool IsEmpty()
    {
        return tbl.empty();
    }
    // 取成员数
    int32_t GetSize()
    {
        return static_cast<int32_t>(tbl.size());
    }
    // 是否为同类元素
    bool IsHomogeneous(int8_t ntype, toml::node *first_nonmatch)
    {
        return tbl.is_homogeneous(static_cast<toml::node_type>(ntype), first_nonmatch);
    }
    // 是否为内联表
    bool IsInline()
    {
        return tbl.is_inline();
    }
    // 取节点
    toml::node *Get(const wchar_t *key)
    {
        return tbl.get(key);
    }
    // 取基本值
    template <typename R>
    R GetAs(const wchar_t *key)
    {
        if (auto val = tbl.get_as<R>(key))
        {
            return **val;
        }
        return R{};
    }
    // 取路径基本值
    template <typename R>
    R GetPathAs(const wchar_t *path)
    {
        if (auto val = tbl.at_path(path).as<R>())
        {
            return **val;
        }
        return R{};
    }
    // 删除键
    int32_t Erase(const wchar_t *key)
    {
        return static_cast<int32_t>(tbl.erase(key));
    }
    // 删除空成员
    void Prune(bool recursive)
    {
        tbl.prune(recursive);
    }
    // 清空
    void Clear()
    {
        tbl.clear();
    }
}; // PivTomlTable

/**
 * @brief TOML节点类
 */
class PivTomlNode : public CVolObject
{
protected:
    toml::node *node{};

public:
    PivTomlNode() {}
    PivTomlNode(toml::node *other)
    {
        node = other;
    }
    ~PivTomlNode() {}
    // 是否为空
    bool IsNull()
    {
        return node == nullptr;
    }
    // 取节点类型
    int8_t Type()
    {
        return node ? static_cast<int8_t>(node->type()) : false;
    }
    // 是否为同类元素
    bool IsHomogeneous(int8_t ntype, toml::node *first_nonmatch)
    {
        return node ? node->is_homogeneous(static_cast<toml::node_type>(ntype), first_nonmatch) : false;
    }
    // 是否为...
    template <typename T>
    bool Is()
    {
        return node ? node->is<T>() : false;
    }
    // 到表
    toml::table *AsTable()
    {
        if (node)
        {
            return node->as_table();
        }
        return nullptr;
    }
    // 到数组
    toml::array *AsArray()
    {
        if (node)
        {
            return node->as_array();
        }
        return nullptr;
    }
    // 到基本值
    template <typename R>
    R As()
    {
        if (node)
        {
            if (auto val = node->as<R>())
            {
                return **val;
            }
        }
        return R{};
    }
    // 取路径基本值
    template <typename R>
    R GetPathAs(const wchar_t *path)
    {
        if (node &&auto val = node->at_path(path).as<R>())
        {
            return **val;
        }
        return R{};
    }
}; // PivTomlNode

/**
 * @brief TOML日期类
 */
class PivTomlDate : public CVolObject
{
protected:
    toml::date date{};

public:
    PivTomlDate() {}
    PivTomlDate(uint16_t y, uint8_t m, uint8_t d) : date{y, m, d} {}
    PivTomlDate(toml::date *val)
    {
        date = *val;
    }
    PivTomlDate(toml::date &val)
    {
        date = val;
    }
    ~PivTomlDate() {}
    void CreateDate(int16_t y, int8_t m, int8_t d)
    {
        date = toml::date{y, m, d};
    }
    PivTomlDate &operator=(PivTomlDate &other)
    {
        date = other.date;
        return *this;
    }
    bool operator==(PivTomlDate &other)
    {
        return date == other.date;
    }
    virtual void GetDumpString(CVolString &strDump, INT nMaxDumpSize) override
    {
        strDump.AddText(this->ToString());
    }
    int16_t year()
    {
        return static_cast<int16_t>(date.year);
    }
    int8_t month()
    {
        return static_cast<int8_t>(date.month);
    }
    int8_t day()
    {
        return static_cast<int8_t>(date.day);
    }
    bool SetVariantTime(double vtime)
    {
        SYSTEMTIME SystemTime{};
        if (::VariantTimeToSystemTime(vtime, &SystemTime))
        {
            CreateDate(static_cast<int16_t>(SystemTime.wYear), static_cast<int8_t>(SystemTime.wMonth),
                       static_cast<int8_t>(SystemTime.wDay));
            return true;
        }
        return false;
    }
    double ToVariantTime()
    {
        return ToTimestamp() / 86400.0 + 25569.0;
    }
    CVolString ToString()
    {
        std::stringstream ss;
        ss << date;
        return PivU2W(ss.str());
    }
    int64_t ToTimestamp()
    {
        struct tm timeinfo
        {
            0, 0, 0, date.day, date.month - 1, date.year - 1900, 0, 0, 0
        };
        return static_cast<int64_t>(::mktime(&timeinfo));
    }
}; // PivTomlDate

/**
 * @brief TOML时间类
 */
class PivTomlTime : public CVolObject
{
protected:
    toml::time time{};

public:
    PivTomlTime() {}
    PivTomlTime(uint8_t h, uint8_t m, uint8_t s, uint32_t ns = 0) : time{h, m, s, ns} {}
    PivTomlTime(toml::time *val)
    {
        time = *val;
    }
    PivTomlTime(toml::time &val)
    {
        time = val;
    }
    ~PivTomlTime() {}
    PivTomlTime &operator=(PivTomlTime &other)
    {
        time = other.time;
        return *this;
    }
    bool operator==(PivTomlTime &other)
    {
        return time == other.time;
    }
    virtual void GetDumpString(CVolString &strDump, INT nMaxDumpSize) override
    {
        strDump.AddText(this->ToString());
    }
    void CreateTime(int8_t h, int8_t m, int8_t s, int32_t ns = 0)
    {
        time = toml::time{h, m, s, ns};
    }
    uint8_t hour()
    {
        return static_cast<uint8_t>(time.hour);
    }
    int8_t minute()
    {
        return static_cast<int8_t>(time.minute);
    }
    int8_t second()
    {
        return static_cast<int8_t>(time.second);
    }
    int32_t nanosecond()
    {
        return static_cast<int32_t>(time.nanosecond);
    }
    bool SetVariantTime(double vtime)
    {
        SYSTEMTIME SystemTime{};
        if (::VariantTimeToSystemTime(vtime, &SystemTime))
        {
            CreateTime(static_cast<int8_t>(SystemTime.wHour), static_cast<int8_t>(SystemTime.wMinute),
                       static_cast<int8_t>(SystemTime.wSecond),
                       static_cast<int32_t>(SystemTime.wMilliseconds * 1000000));
            return true;
        }
        return false;
    }
    double ToVariantTime()
    {
        return (time.hour * 60 * 60.0 + time.second * 60.0 + time.second + time.nanosecond / 1000000000.0) / 86400.0;
    }
    CVolString ToString()
    {
        std::stringstream ss;
        ss << time;
        return PivU2W(ss.str());
    }
}; // PivTomlTime

/**
 * @brief TOML日期时间类
 */
class PivTomlDateTime : public CVolObject
{
protected:
    toml::date_time date_time{};

public:
    PivTomlDateTime() {}
    PivTomlDateTime(toml::date_time *val)
    {
        date_time = *val;
    }
    PivTomlDateTime(toml::date_time &val)
    {
        date_time = val;
    }
    ~PivTomlDateTime() {}
    PivTomlDateTime &operator=(PivTomlDateTime &other)
    {
        date_time = other.date_time;
        return *this;
    }
    bool operator==(PivTomlDateTime &other)
    {
        return date_time == other.date_time;
    }
    virtual void GetDumpString(CVolString &strDump, INT nMaxDumpSize) override
    {
        strDump.AddText(this->ToString());
    }
    void CreateDateTime(int16_t y, int8_t m, int8_t d, int8_t H, int8_t M, int8_t S, int32_t ns = 0, int16_t offset = 0)
    {
        date_time.date = toml::date{y, m, d};
        date_time.time = toml::time{H, M, S, ns};
        if (date_time.offset.has_value())
        {
            date_time.offset->minutes = offset;
        }
    }
    bool IsLocal()
    {
        return date_time.is_local();
    }
    toml::date &GetDate()
    {
        return date_time.date;
    }
    toml::time &GetTime()
    {
        return date_time.time;
    }
    int16_t GetOffset()
    {
        if (date_time.is_local())
        {
            return date_time.offset->minutes;
        }
        return 0;
    }
    bool SetVariantTime(double vtime)
    {
        SYSTEMTIME SystemTime{};
        if (::VariantTimeToSystemTime(vtime, &SystemTime))
        {
            TIME_ZONE_INFORMATION tz{0};
            ::GetTimeZoneInformation(&tz);
            CreateDateTime(static_cast<int16_t>(SystemTime.wYear), static_cast<int8_t>(SystemTime.wMonth),
                           static_cast<int8_t>(SystemTime.wDay), static_cast<int8_t>(SystemTime.wHour),
                           static_cast<int8_t>(SystemTime.wMinute), static_cast<int8_t>(SystemTime.wSecond),
                           static_cast<int32_t>(SystemTime.wMilliseconds * 1000000),
                           static_cast<int16_t>(tz.Bias * 60));
            return true;
        }
        return false;
    }
    bool SetTimestamp(int64_t timestamp)
    {
        struct tm timeinfo;
        if (localtime_s(&timeinfo, &timestamp) == 0)
        {
            TIME_ZONE_INFORMATION tz{0};
            ::GetTimeZoneInformation(&tz);
            CreateDateTime(static_cast<int16_t>(timeinfo.tm_yday + 1900), static_cast<int8_t>(timeinfo.tm_mon + 1),
                           static_cast<int8_t>(timeinfo.tm_mday), static_cast<int8_t>(timeinfo.tm_hour),
                           static_cast<int8_t>(timeinfo.tm_min), static_cast<int8_t>(timeinfo.tm_sec),
                           static_cast<int32_t>(0), static_cast<int16_t>(tz.Bias * 60));
            return true;
        }
        return false;
    }
    bool SetMsTimestamp(int64_t timestamp)
    {
        struct tm timeinfo;
        if (localtime_s(&timeinfo, &timestamp) == 0)
        {
            TIME_ZONE_INFORMATION tz{0};
            ::GetTimeZoneInformation(&tz);
            CreateDateTime(static_cast<int16_t>(timeinfo.tm_yday + 1900), static_cast<int8_t>(timeinfo.tm_mon + 1),
                           static_cast<int8_t>(timeinfo.tm_mday), static_cast<int8_t>(timeinfo.tm_hour),
                           static_cast<int8_t>(timeinfo.tm_min), static_cast<int8_t>(timeinfo.tm_sec),
                           static_cast<int32_t>((timestamp % 1000) * 1000000), static_cast<int16_t>(tz.Bias * 60));
            return true;
        }
        return false;
    }
    double ToVariantTime()
    {
        TIME_ZONE_INFORMATION tz{0};
        ::GetTimeZoneInformation(&tz);
        return (ToMsTimestamp() - tz.Bias * 1000 * 60) / (1000.0 * 60.0 * 60.0 * 24.0) + 25569;
    }
    CVolString ToString()
    {
        std::stringstream ss;
        ss << date_time;
        return PivU2W(ss.str());
    }
    int64_t ToTimestamp()
    {
        struct tm timeinfo
        {
            date_time.time.second, date_time.time.minute, date_time.time.hour, date_time.date.day,
                date_time.date.month - 1, date_time.date.year - 1900, 0, 0, 0
        };
        return static_cast<int64_t>(::mktime(&timeinfo)) + (date_time.is_local() ? 0 : date_time.offset->minutes * 60);
    }
    int64_t ToMsTimestamp()
    {
        struct tm timeinfo
        {
            date_time.time.second, date_time.time.minute, date_time.time.hour, date_time.date.day,
                date_time.date.month - 1, date_time.date.year - 1900, 0, 0, 0
        };
        return (static_cast<int64_t>(::mktime(&timeinfo)) + (date_time.is_local() ? 0 : date_time.offset->minutes * 60)) * 1000 + date_time.time.nanosecond / 1000000;
    }
}; // PivTomlDateTime

#endif // _PIV_TOML_PLUS_PLUS_HPP
