/*********************************************\
 * 火山视窗PIV模块 - TOML++                  *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 版本: 2023/03/10                          *
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
#include "piv_encoding.hpp"

namespace piv
{
    namespace toml
    {
        template <typename = void>
        inline const std::wstring_view &to_wview(const std::wstring_view &str)
        {
            return str;
        }
        template <typename = void>
        inline std::wstring_view to_wview(const std::wstring &str)
        {
            return std::wstring_view{str};
        }
        template <typename = void>
        inline std::wstring_view to_wview(const CVolString &str)
        {
            return std::wstring_view{str.GetText()};
        }
        template <typename = void>
        inline std::wstring_view to_wview(const wchar_t *str)
        {
            return std::wstring_view{str};
        }
        template <typename = void>
        inline std::wstring_view to_wview(const CVolMem &str)
        {
            return std::wstring_view{reinterpret_cast<const wchar_t *>(str.GetPtr()), static_cast<size_t>(str.GetSize()) / 2};
        }

        /*
        template <typename = void>
        inline std::wstring_view to_wview(const std::string &str)
        {
            return std::wstring_view{*PivU2W{str}};
        }
        template <typename = void>
        inline std::wstring_view to_wview(const std::string_view &str)
        {
            return std::wstring_view{*PivU2W{str}};
        }
        template <typename = void>
        inline std::wstring_view to_wview(const char *str)
        {
            return std::wstring_view{*PivU2W{str}};
        }
        */

        template <typename = void>
        inline const std::string_view &to_view(const std::string_view &str)
        {
            return str;
        }
        template <typename = void>
        inline std::string_view to_view(const std::string &str)
        {
            return std::string_view{str};
        }
        template <typename = void>
        inline std::string_view to_view(const char *str)
        {
            return std::string_view{str};
        }
        template <typename = void>
        inline std::string_view to_view(const CVolMem &str)
        {
            return std::string_view{reinterpret_cast<const char *>(str.GetPtr()), static_cast<size_t>(str.GetSize())};
        }
        /*
        template <typename = void>
        inline const std::string_view &to_view(const std::wstring_view &str)
        {
            return std::string_view{*PivW2U{str}};
        }
        template <typename = void>
        inline std::string_view to_view(const std::wstring &str)
        {
            return std::string_view{*PivW2U{str}};
        }
        template <typename = void>
        inline std::string_view to_view(const CVolString &str)
        {
            return std::string_view{*PivW2U{str}};
        }
        template <typename = void>
        inline std::string_view to_view(const wchar_t *str)
        {
            return std::string_view{*PivW2U{str}};
        }
        */
    }
}

/**
 * @brief TOML键值表类
 */
class PivTomlTable : public CVolObject
{
protected:
    toml::table tbl{};

public:
    PivTomlTable() {}
    ~PivTomlTable() {}

    PivTomlTable(const toml::table &other) : tbl{other} {}
    PivTomlTable(toml::table &&other) : tbl{other} {}
    PivTomlTable(const toml::table *other)
    {
        if (other != nullptr)
            tbl = *other;
        else
            tbl.clear();
    }

    inline PivTomlTable &operator=(const PivTomlTable &other)
    {
        tbl = other.tbl;
        return *this;
    }
    inline PivTomlTable &operator=(PivTomlTable &&other)
    {
        tbl = std::move(other.tbl);
        return *this;
    }
    inline PivTomlTable &operator=(const toml::table *other)
    {
        tbl = *other;
        return *this;
    }
    inline PivTomlTable &operator=(const toml::table &other)
    {
        tbl = other;
        return *this;
    }
    inline PivTomlTable &operator=(toml::table &&other)
    {
        tbl = std::move(other);
        return *this;
    }

    inline bool operator==(const PivTomlTable &other) const noexcept
    {
        return tbl == other.tbl;
    }
    inline bool operator==(const toml::table &other) const noexcept
    {
        return tbl == other;
    }

    virtual void GetDumpString(CVolString &strDump, INT nMaxDumpSize) override
    {
        strDump.AddText(this->ToVolString());
    }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << tbl;
        return ss.str();
    }

    CVolString ToVolString() const
    {
        std::stringstream ss;
        ss << tbl;
        return *PivU2Ws(ss.str());
    }

    /**
     * @brief 解析文件
     * @param file_path 文件路径
     * @return 是否解析成功
     */
    template <typename T>
    bool ParseFile(const T &file_path)
    {
        toml::parse_result result = toml::parse_file(piv::toml::to_wview(file_path));
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
    bool Parse(const D &doc, const S &source_path)
    {
        toml::parse_result result = toml::parse(piv::toml::to_view(doc), piv::toml::to_wview(source_path));
        if (result)
        {
            tbl = std::move(result.table());
            return true;
        }
        tbl.clear();
        return false;
    }

    // 是否为空
    inline bool IsEmpty() const noexcept
    {
        return tbl.empty();
    }

    // 取成员数
    inline int32_t GetSize() const noexcept
    {
        return static_cast<int32_t>(tbl.size());
    }

    // 是否为同类元素
    inline bool IsHomogeneous(const int8_t &ntype, toml::node *first_nonmatch) noexcept
    {
        return tbl.is_homogeneous(static_cast<toml::node_type>(ntype), first_nonmatch);
    }

    // 是否为内联表
    inline bool IsInline() const noexcept
    {
        return tbl.is_inline();
    }

    // 取节点
    template <typename K>
    toml::node *Get(const K &key)
    {
        return tbl.get(piv::toml::to_wview(key));
    }

    // 取基本值
    template <typename R, typename K>
    R GetAs(const K &key)
    {
        auto val = tbl.get_as<R>(piv::toml::to_wview(key));
        if (val)
            return **val;
        return R{};
    }

    // 取路径基本值
    template <typename R, typename T>
    R GetPathAs(const T &path)
    {
        auto val = tbl.at_path(piv::toml::to_wview(path)).as<R>();
        if (val)
            return **val;
        return R{};
    }

    // 删除键
    template <typename T>
    int32_t Erase(const T &key)
    {
        return static_cast<int32_t>(tbl.erase(piv::toml::to_wview(key)));
    }

    // 删除空成员
    inline void Prune(const bool &recursive)
    {
        tbl.prune(recursive);
    }

    // 清空
    inline void Clear()
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
    ~PivTomlNode() {}

    PivTomlNode(toml::node *other)
    {
        node = other;
    }

    // 是否为空
    inline bool IsNull() const noexcept
    {
        return node == nullptr;
    }

    // 取节点类型
    inline int8_t Type() const
    {
        return node ? static_cast<int8_t>(node->type()) : 0;
    }

    // 是否为同类元素
    bool IsHomogeneous(int8_t ntype, toml::node *first_nonmatch)
    {
        return node ? node->is_homogeneous(static_cast<toml::node_type>(ntype), first_nonmatch) : false;
    }

    // 是否为...
    template <typename T>
    bool Is() const noexcept
    {
        return node ? node->is<T>() : false;
    }

    // 到表
    inline toml::table *AsTable() noexcept
    {
        return node ? node->as_table() : nullptr;
    }

    // 到数组
    inline toml::array *AsArray() noexcept
    {
        return node ? node->as_array() : nullptr;
    }

    // 到基本值
    template <typename R>
    R As()
    {
        if (node)
        {
            auto val = node->as<R>();
            if (val)
                return **val;
        }
        return R{};
    }

    // 取路径基本值
    template <typename R, typename T>
    R GetPathAs(const T &path)
    {
        if (node)
        {
            auto val = node->at_path(piv::toml::to_wview(path)).as<R>();
            if (val)
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
    ~PivTomlDate() {}

    PivTomlDate(uint16_t y, uint8_t m, uint8_t d) : date{y, m, d} {}

    PivTomlDate(const toml::date *val)
    {
        date = *val;
    }
    PivTomlDate(toml::date *val)
    {
        date = *val;
    }
    PivTomlDate(const toml::date &val)
    {
        date = val;
    }
    PivTomlDate(toml::date &&val)
    {
        date = std::move(val);
    }

    inline PivTomlDate &operator=(const PivTomlDate &other)
    {
        date = other.date;
        return *this;
    }
    inline PivTomlDate &operator=(PivTomlDate &&other)
    {
        date = std::move(other.date);
        return *this;
    }
    inline PivTomlDate &operator=(const toml::date *other)
    {
        date = *other;
        return *this;
    }
    inline PivTomlDate &operator=(const toml::date &other)
    {
        date = other;
        return *this;
    }
    inline PivTomlDate &operator=(toml::date &&other)
    {
        date = std::move(other);
        return *this;
    }

    inline bool operator==(const PivTomlDate &other) const
    {
        return date == other.date;
    }
    inline bool operator==(const toml::date &other) const
    {
        return date == other;
    }

    virtual void GetDumpString(CVolString &strDump, INT nMaxDumpSize) override
    {
        strDump.AddText(this->ToVolString());
    }

    void CreateDate(int16_t y, int8_t m, int8_t d)
    {
        date = std::move(toml::date{y, m, d});
    }

    inline int16_t year() const
    {
        return static_cast<int16_t>(date.year);
    }

    inline int8_t month() const
    {
        return static_cast<int8_t>(date.month);
    }

    inline int8_t day() const
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

    inline double ToVariantTime() const
    {
        return ToTimestamp() / 86400.0 + 25569.0;
    }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << date;
        return ss.str();
    }

    CVolString ToVolString() const
    {
        std::stringstream ss;
        ss << date;
        return *PivU2Ws(ss.str());
    }

    int64_t ToTimestamp() const
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
    ~PivTomlTime() {}

    PivTomlTime(uint8_t h, uint8_t m, uint8_t s, uint32_t ns = 0) : time{h, m, s, ns} {}

    PivTomlTime(const toml::time *val)
    {
        time = *val;
    }
    PivTomlTime(const toml::time &val)
    {
        time = val;
    }
    PivTomlTime(toml::time &&val)
    {
        time = std::move(val);
    }

    inline PivTomlTime &operator=(const PivTomlTime &other)
    {
        time = other.time;
        return *this;
    }
    inline PivTomlTime &operator=(PivTomlTime &&other)
    {
        time = std::move(other.time);
        return *this;
    }
    inline PivTomlTime &operator=(const toml::time *other)
    {
        time = *other;
        return *this;
    }
    inline PivTomlTime &operator=(const toml::time &other)
    {
        time = other;
        return *this;
    }
    inline PivTomlTime &operator=(toml::time &&other)
    {
        time = std::move(other);
        return *this;
    }

    inline bool operator==(const PivTomlTime &other) const
    {
        return time == other.time;
    }
    inline bool operator==(const toml::time &other) const
    {
        return time == other;
    }

    virtual void GetDumpString(CVolString &strDump, INT nMaxDumpSize) override
    {
        strDump.AddText(this->ToVolString());
    }

    void CreateTime(int8_t h, int8_t m, int8_t s, int32_t ns = 0)
    {
        time = toml::time{h, m, s, ns};
    }

    inline uint8_t hour() const
    {
        return static_cast<uint8_t>(time.hour);
    }

    inline int8_t minute() const
    {
        return static_cast<int8_t>(time.minute);
    }

    inline int8_t second() const
    {
        return static_cast<int8_t>(time.second);
    }

    inline int32_t nanosecond() const
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

    inline double ToVariantTime() const
    {
        return (time.hour * 60 * 60.0 + time.second * 60.0 + time.second + time.nanosecond / 1000000000.0) / 86400.0;
    }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << time;
        return ss.str();
    }

    CVolString ToVolString() const
    {
        std::stringstream ss;
        ss << time;
        return *PivU2Ws(ss.str());
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
    ~PivTomlDateTime() {}

    PivTomlDateTime(const toml::date_time *val)
    {
        date_time = *val;
    }
    PivTomlDateTime(const toml::date_time &val)
    {
        date_time = val;
    }
    PivTomlDateTime(toml::date_time &&val)
    {
        date_time = std::move(val);
    }

    inline PivTomlDateTime &operator=(const PivTomlDateTime &other)
    {
        date_time = other.date_time;
        return *this;
    }
    inline PivTomlDateTime &operator=(PivTomlDateTime &&other)
    {
        date_time = std::move(other.date_time);
        return *this;
    }
    inline PivTomlDateTime &operator=(const toml::date_time *other)
    {
        date_time = *other;
        return *this;
    }
    inline PivTomlDateTime &operator=(const toml::date_time &other)
    {
        date_time = other;
        return *this;
    }
    inline PivTomlDateTime &operator=(toml::date_time &&other)
    {
        date_time = std::move(other);
        return *this;
    }

    inline bool operator==(const PivTomlDateTime &other) const noexcept
    {
        return date_time == other.date_time;
    }
    inline bool operator==(const toml::date_time &other) const noexcept
    {
        return date_time == other;
    }

    virtual void GetDumpString(CVolString &strDump, INT nMaxDumpSize) override
    {
        strDump.AddText(this->ToVolString());
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

    inline bool IsLocal() const
    {
        return date_time.is_local();
    }

    inline toml::date &GetDate()
    {
        return date_time.date;
    }

    inline toml::time &GetTime()
    {
        return date_time.time;
    }

    inline int16_t GetOffset() const
    {
        if (date_time.is_local())
            return date_time.offset->minutes;
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

    double ToVariantTime() const
    {
        TIME_ZONE_INFORMATION tz{0};
        ::GetTimeZoneInformation(&tz);
        return (ToMsTimestamp() - tz.Bias * 1000 * 60) / (1000.0 * 60.0 * 60.0 * 24.0) + 25569;
    }

    std::string ToString() const
    {
        std::stringstream ss;
        ss << date_time;
        return ss.str();
    }

    CVolString ToVolString() const
    {
        std::stringstream ss;
        ss << date_time;
        return *PivU2Ws(ss.str());
    }

    int64_t ToTimestamp() const
    {
        struct tm timeinfo
        {
            date_time.time.second, date_time.time.minute, date_time.time.hour, date_time.date.day,
                date_time.date.month - 1, date_time.date.year - 1900, 0, 0, 0
        };
        return static_cast<int64_t>(::mktime(&timeinfo)) + (date_time.is_local() ? 0 : date_time.offset->minutes * 60);
    }

    int64_t ToMsTimestamp() const
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
