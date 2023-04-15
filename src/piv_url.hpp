/*********************************************\
 * 火山视窗PIV模块 - URL辅助                 *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 版本: 2023/04/15                          *
\*********************************************/

#ifndef _PIV_URL_HPP
#define _PIV_URL_HPP

#include "piv_string.hpp"
#include <map>

/**
 * @brief URL地址解析类
 * @tparam CharT 字符类型
 * @tparam EncodeType 文本编码,按类型的尺寸区分(1=ANSI; 2=UTF-16LE; 3=UTF8;)
 */
template <typename CharT, typename EncodeType>
class PivUrlParser : public CVolObject
{
private:
    std::basic_string<CharT> urlstr;
    std::basic_string<CharT> port_str;
    piv::basic_string_view<CharT> url;
    piv::basic_string_view<CharT> scheme;
    piv::basic_string_view<CharT> user;
    piv::basic_string_view<CharT> password;
    piv::basic_string_view<CharT> host;
    piv::basic_string_view<CharT> port;
    piv::basic_string_view<CharT> path;
    piv::basic_string_view<CharT> query;
    piv::basic_string_view<CharT> fragment;
    std::map<piv::basic_string_view<CharT>, piv::basic_string_view<CharT>, piv::detail::ci_less<piv::basic_string_view<CharT>>> params;

public:
    /**
     * @brief 默认构造于析构函数
     */
    PivUrlParser() {}
    ~PivUrlParser() {}

    /**
     * @brief 复制和移动构造函数
     * @param other 所欲复制的对象
     */
    PivUrlParser(const PivUrlParser &other)
    {
        urlstr = other.urlstr;
        port_str = other.port_str;
        url = other.url;
        scheme = other.scheme;
        user = other.user;
        password = other.password;
        host = other.host;
        port = other.port;
        path = other.path;
        query = other.query;
        fragment = other.fragment;
        params = other.params;
    }
    PivUrlParser(PivUrlParser &&other) noexcept
    {
        urlstr = std::move(other.urlstr);
        port_str = std::move(other.port_str);
        url = std::move(other.url);
        scheme = std::move(other.scheme);
        user = std::move(other.user);
        password = std::move(other.password);
        host = std::move(other.host);
        port = std::move(other.port);
        path = std::move(other.path);
        query = std::move(other.query);
        fragment = std::move(other.fragment);
        params = std::move(other.params);
    }

    /**
     * @brief 带参构造函数
     * @param urlAdress 所欲解析的URL地址
     * @param simple 是否简易解析
     * @param storeBuf 是否缓存地址数据
     * @param count URL地址长度
     */
    PivUrlParser(const CharT *urlAdress, const bool &storeBuf = false, const bool &simple = false, const size_t &count = static_cast<size_t>(-1))
    {
        Parse(urlAdress, storeBuf, simple, count);
    }
    PivUrlParser(const piv::basic_string_view<CharT> &urlAdress, const bool &storeBuf = false, const bool &simple = false)
    {
        Parse(urlAdress, storeBuf, simple);
    }
    PivUrlParser(const std::basic_string<CharT> &urlAdress, const bool &storeBuf = false, const bool &simple = false)
    {
        Parse(urlAdress, storeBuf, simple);
    }
    PivUrlParser(const CVolMem &urlAdress, const bool &storeBuf = false, const bool &simple = false)
    {
        Parse(urlAdress, storeBuf, simple);
    }
    PivUrlParser(const CVolString &urlAdress, const bool &storeBuf = false, const bool &simple = false)
    {
        Parse(urlAdress, storeBuf, simple);
    }

    /**
     * @brief 赋值操作符
     * @param other 所欲复制的对象
     * @return 返回自身
     */
    PivUrlParser &operator=(const PivUrlParser &other)
    {
        urlstr = other.urlstr;
        port_str = other.port_str;
        url = other.url;
        scheme = other.scheme;
        user = other.user;
        password = other.password;
        host = other.host;
        port = other.port;
        path = other.path;
        query = other.query;
        fragment = other.fragment;
        params = other.params;
        return *this;
    }
    PivUrlParser &operator=(PivUrlParser &&other) noexcept
    {
        urlstr = std::move(other.urlstr);
        port_str = std::move(other.port_str);
        url = std::move(other.url);
        scheme = std::move(other.scheme);
        user = std::move(other.user);
        password = std::move(other.password);
        host = std::move(other.host);
        port = std::move(other.port);
        path = std::move(other.path);
        query = std::move(other.query);
        fragment = std::move(other.fragment);
        params = std::move(other.params);
        return *this;
    }

    /**
     * @brief 清空
     */
    inline void Clear()
    {
        urlstr.clear();
        port_str.clear();
        url.swap(piv::basic_string_view<CharT>{});
        scheme.swap(piv::basic_string_view<CharT>{});
        user.swap(piv::basic_string_view<CharT>{});
        password.swap(piv::basic_string_view<CharT>{});
        host.swap(piv::basic_string_view<CharT>{});
        port.swap(piv::basic_string_view<CharT>{});
        path.swap(piv::basic_string_view<CharT>{});
        query.swap(piv::basic_string_view<CharT>{});
        fragment.swap(piv::basic_string_view<CharT>{});
        params.clear();
    }

    virtual void GetDumpString(CVolString &strDump, INT nMaxDumpSize) override
    {
        strDump.AddFormatText(L"<URL地址解析类%s> 解析结果:\r\n1. 协议: %s\r\n2. 账户: %s\r\n3. 密码: %s\r\n4. 域名: %s\r\n5. 端口: %s\r\n6. 路径: %s\r\n7. 参数: %s\r\n8. 段落: %s\r\n",
                              (sizeof(EncodeType) == 2) ? L"W" : (sizeof(EncodeType) == 3 ? L"U" : L"A"),
                              GetScheme().GetText(), GetUser().GetText(), GetPassword().GetText(), GetHost().GetText(),
                              GetPort().GetText(), GetPath().GetText(), GetQuery().GetText(), GetFragment().GetText());
    }

    /**
     * @brief 置对象数据
     * @param stream 基本输入流
     */
    virtual void LoadFromStream(CVolBaseInputStream &stream)
    {
        Clear();
        if (stream.eof())
            return;
        uint32_t Size;
        stream.read(&Size, 4);
        urlstr.resize(Size / sizeof(CharT));
        if (stream.ReadExact(const_cast<CharT *>(urlstr.data()), Size))
            Parse(urlstr);
        else
            urlstr.clear();
    }

    /**
     * @brief 取对象数据
     * @param stream 基本输出流
     */
    virtual void SaveIntoStream(CVolBaseOutputStream &stream)
    {
        uint32_t Size = static_cast<int32_t>((url.size()) * sizeof(CharT));
        stream.write(&Size, 4);
        stream.write(url.data(), Size);
    }

    /**
     * @brief 解析URL地址
     * @param urlAdress 所欲解析的URL地址
     * @param simple 是否简易解析
     * @param storeBuf 是否缓存地址数据
     * @param count URL地址长度
     */
    void Parse(const piv::basic_string_view<CharT> &urlAdress, const bool &storeBuf = false, const bool &simple = false)
    {
        Clear();
        if (urlAdress.empty())
            return;
        if (storeBuf)
        {
            urlstr.assign(urlAdress.data(), urlAdress.size());
            url = urlstr;
        }
        else
        {
            url = urlAdress;
        }
        // 删自身首尾空
        piv::edit::trim_left(url);
        piv::edit::trim_right(url);
        size_t pos1, pos2, pos3;
        // 寻找和获取协议
        pos1 = url.find(':');
        if (pos1 != piv::basic_string_view<CharT>::npos && pos1 + 2 < url.size() && url[pos1 + 1] == '/' && url[pos1 + 2] == '/')
        {
            scheme = url.substr(0, pos1);
            pos1 += 3;
        }
        else
        {
            pos1 = 0;
        }
        // 寻找和获取路径
        pos2 = url.find('/', pos1);
        if (pos2 != piv::basic_string_view<CharT>::npos)
        {
            path = url.substr(pos2, url.size() - pos2);
        }
        else
        {
            pos2 = url.find('?', pos1);
            if (pos2 != piv::basic_string_view<CharT>::npos)
                path = url.substr(pos2, url.size() - pos2);
            else
                pos2 = 0;
        }
        if (simple == false)
        {
            // 获取域名
            if (pos2 >= pos1 && pos1 < url.size())
                host = url.substr(pos1, pos2 - pos1);
            // 寻找和获取账户和密码
            pos1 = host.find(':');
            pos2 = host.find('@');
            if (pos1 != piv::basic_string_view<CharT>::npos && pos2 != piv::basic_string_view<CharT>::npos)
            {
                user = host.substr(0, pos1);
                password = host.substr(pos1 + 1, pos2 - pos1 - 1);
                host.remove_prefix(pos2 + 1);
            }
            // 寻找和获取端口号
            pos1 = host.find(':');
            if (pos1 != piv::basic_string_view<CharT>::npos)
            {
                port = host.substr(pos1 + 1, host.size() - pos1 - 1);
                host.remove_suffix(host.size() - pos1);
            }
            else
            {
                std::basic_string<CharT> scheme_lower;
                scheme_lower.resize(scheme.size());
                std::transform(scheme.begin(), scheme.end(), scheme_lower.begin(), (int (*)(int))std::tolower);
                if (scheme_lower.size() >= 4 && scheme_lower[0] == 'h' && scheme_lower[1] == 't' && scheme_lower[2] == 't' && scheme_lower[3] == 'p')
                {
                    if (scheme_lower.size() >= 5 && scheme_lower[4] == 's')
                    {
                        port_str.push_back('4');
                        port_str.push_back('4');
                        port_str.push_back('3');
                        port = port_str;
                    }
                    else
                    {
                        port_str.push_back('8');
                        port_str.push_back('0');
                        port = port_str;
                    }
                }
            }
        }
        if (path.empty())
            return;
        // 寻找和获取段落
        pos1 = path.find('#');
        if (pos1 != piv::basic_string_view<CharT>::npos)
        {
            fragment = path.substr(pos1 + 1, path.size() - pos1 - 1);
            path.remove_suffix(path.size() - pos1);
        }
        // 寻找和获取参数
        pos1 = path.find('?');
        if (pos1 != piv::basic_string_view<CharT>::npos)
        {
            query = path.substr(pos1 + 1, path.size() - pos1 - 1);
            path.remove_suffix(path.size() - pos1);
        }
        if (query.size() < 3) // 长度不足,无法构成键值对
            return;
        // 分割键值对
        pos1 = 0;
        pos2 = query.find('&', 0);
        while (pos2 != piv::basic_string_view<CharT>::npos)
        {
            pos3 = query.find('=', pos1);
            if (pos3 != piv::basic_string_view<CharT>::npos)
            {
                params[query.substr(pos1, pos3 - pos1)] = query.substr(pos3 + 1, pos2 - pos3 - 1);
            }
            pos1 = pos2 + 1;
            pos2 = query.find('&', pos1);
        }
        pos3 = query.find('=', pos1);
        if (pos3 != piv::basic_string_view<CharT>::npos)
        {
            params[query.substr(pos1, pos3 - pos1)] = query.substr(pos3 + 1, query.size() - pos3 - 1);
        }
    }
    inline void Parse(const CharT *urlAdress, const bool &storeBuf = false, const bool &simple = false, const size_t &count = static_cast<size_t>(-1))
    {
        Parse((count == static_cast<size_t>(-1)) ? piv::basic_string_view<CharT>{urlAdress} : piv::basic_string_view<CharT>{urlAdress, count}, storeBuf, simple);
    }
    inline void Parse(const std::basic_string<CharT> &urlAdress, const bool &storeBuf = false, const bool &simple = false)
    {
        Parse(piv::basic_string_view<CharT>{urlAdress}, storeBuf, simple);
    }
    inline void Parse(const CVolMem &urlAdress, const bool &storeBuf = false, const bool &simple = false)
    {
        Parse(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(urlAdress.GetPtr()), static_cast<size_t>(urlAdress.GetSize()) / sizeof(CharT)}, storeBuf, simple);
    }
    inline void Parse(const CVolString &urlAdress, const bool &storeBuf = false, const bool &simple = false)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            Parse(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(urlAdress.GetText())}, storeBuf, simple);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            Parse(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{urlAdress}.GetText())}, true);
        }
        else
        {
            Parse(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{urlAdress}.GetText())}, true, simple);
        }
    }

    /**
     * @brief 是否有缓存
     * @return 返回真表示有缓存
     */
    inline bool HasBuffer() const
    {
        return !urlstr.empty();
    }

    /**
     * @brief 获取URL各组件的文本视图
     * @return 返回文本视图,不会产生文本复制
     */
    inline const piv::basic_string_view<CharT> &Url() const noexcept
    {
        return url;
    }
    inline const piv::basic_string_view<CharT> &Scheme() const noexcept
    {
        return scheme;
    }
    inline const piv::basic_string_view<CharT> &User() const noexcept
    {
        return user;
    }
    inline const piv::basic_string_view<CharT> &Password() const noexcept
    {
        return password;
    }
    inline const piv::basic_string_view<CharT> &Host() const noexcept
    {
        return host;
    }
    inline const piv::basic_string_view<CharT> &Port() const noexcept
    {
        return port;
    }
    inline const piv::basic_string_view<CharT> &Path() const noexcept
    {
        return path;
    }
    inline const piv::basic_string_view<CharT> &Query() const noexcept
    {
        return query;
    }
    inline const piv::basic_string_view<CharT> &Fragment() const noexcept
    {
        return fragment;
    }

    /**
     * @brief 取协议
     * @return 返回文本型
     */
    inline CVolString GetScheme() const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return CVolString(scheme.data(), scheme.size());
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return *PivU2Ws{reinterpret_cast<const char *>(scheme.data()), scheme.size()};
        }
        else
        {
            return *PivA2Ws{reinterpret_cast<const char *>(scheme.data()), scheme.size()};
        }
    }

    /**
     * @brief 取账户
     * @param utf8 是否为UTF-8编码
     * @return 返回文本
     */
    inline CVolString GetUser(const bool &utf8 = true) const
    {
        std::basic_string<CharT> str;
        if (piv::encoding::UrlDecodeNeed(user.data(), user.size()))
            piv::encoding::UrlStrDecode(user, utf8, str);
        else
            str.assign(user.data(), user.size());
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return CVolString(str.c_str());
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return *PivU2Ws{reinterpret_cast<const char *>(str.data()), str.size()};
        }
        else
        {
            return *PivA2Ws{reinterpret_cast<const char *>(str.data()), str.size()};
        }
    }
    inline std::basic_string<CharT> DecodeUser(const bool &utf8 = true) const
    {
        return piv::encoding::UrlStrDecode(user, utf8);
    }

    /**
     * @brief 取密码
     * @param urlDecode 是否URL解码
     * @param utf8 是否为UTF-8编码
     * @return 返回文本
     */
    inline CVolString GetPassword(const bool &utf8 = true) const
    {
        std::basic_string<CharT> str;
        if (piv::encoding::UrlDecodeNeed(password.data(), password.size()))
            piv::encoding::UrlStrDecode(password, utf8, str);
        else
            str.assign(password.data(), password.size());
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return CVolString(str.c_str());
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return *PivU2Ws{reinterpret_cast<const char *>(str.data()), str.size()};
        }
        else
        {
            return *PivA2Ws{reinterpret_cast<const char *>(str.data()), str.size()};
        }
    }
    inline std::basic_string<CharT> DecodePassword(const bool &utf8 = true) const
    {
        return piv::encoding::UrlStrDecode(password, utf8);
    }

    /**
     * @brief 取域名
     * @return 返回文本型
     */
    inline CVolString GetHost() const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return CVolString(host.data(), host.size());
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return *PivU2Ws{reinterpret_cast<const char *>(host.data()), host.size()};
        }
        else
        {
            return *PivA2Ws{reinterpret_cast<const char *>(host.data()), host.size()};
        }
    }

    /**
     * @brief 取路径
     * @return 返回文本型
     */
    inline CVolString GetPath() const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return CVolString(path.data(), path.size());
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return *PivU2Ws{reinterpret_cast<const char *>(path.data()), path.size()};
        }
        else
        {
            return *PivA2Ws{reinterpret_cast<const char *>(path.data()), path.size()};
        }
    }

    /**
     * @brief 取端口
     * @return 返回文本型
     */
    inline CVolString GetPort() const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return CVolString(port.data(), port.size());
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return *PivU2Ws{reinterpret_cast<const char *>(port.data()), port.size()};
        }
        else
        {
            return *PivA2Ws{reinterpret_cast<const char *>(port.data()), port.size()};
        }
    }

    /**
     * @brief 取参数
     * @return 返回文本型
     */
    inline CVolString GetQuery() const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return CVolString(query.data(), query.size());
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return *PivU2Ws{reinterpret_cast<const char *>(query.data()), query.size()};
        }
        else
        {
            return *PivA2Ws{reinterpret_cast<const char *>(query.data()), query.size()};
        }
    }

    /**
     * @brief 取段落
     * @return 返回文本型
     */
    inline CVolString GetFragment() const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return CVolString(fragment.data(), fragment.size());
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return *PivU2Ws{reinterpret_cast<const char *>(fragment.data()), fragment.size()};
        }
        else
        {
            return *PivA2Ws{reinterpret_cast<const char *>(fragment.data()), fragment.size()};
        }
    }

    /**
     * @brief 取参数视图
     * @param name 参数名称
     * @param utf8 是否为UTF-8编码
     * @return 返回std::basic_string
     */
    inline piv::basic_string_view<CharT> GetParam(const piv::basic_string_view<CharT> &name, const bool &utf8 = true)
    {
        if (params.find(name) != params.end())
        {
            return params[name];
        }
        else if (piv::encoding::UrlEncodeNeed(name.data(), name.size()))
        {
            return params[piv::basic_string_view<CharT>{piv::encoding::UrlStrEncode(name, utf8, false)}];
        }
        return piv::basic_string_view<CharT>{};
    }
    inline piv::basic_string_view<CharT> GetParam(const CharT *name, const bool &utf8 = true)
    {
        return GetParam(piv::basic_string_view<CharT>{name}, utf8);
    }
    inline piv::basic_string_view<CharT> GetParam(const std::basic_string<CharT> &name, const bool &utf8 = true)
    {
        return GetParam(piv::basic_string_view<CharT>{name.data(), name.size()}, utf8);
    }
    inline piv::basic_string_view<CharT> GetParam(const CVolMem &name, const bool &utf8 = true)
    {
        return GetParam(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(name.GetPtr()), static_cast<size_t>(name.GetSize()) / sizeof(CharT)}, utf8);
    }
    inline piv::basic_string_view<CharT> GetParam(const CVolString &name, const bool &utf8 = true)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return GetParam(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(name.GetText()), static_cast<size_t>(name.GetLength())}, utf8);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return GetParam(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{name}.GetText())}, true);
        }
        else
        {
            return GetParam(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{name}.GetText())}, false);
        }
    }

    /**
     * @brief 取指定参数
     * @param name 参数名称
     * @param utf8 是否为UTF-8编码
     * @return 返回文本型
     */
    inline CVolString &GetParamVolStr(const piv::basic_string_view<CharT> &name, const bool &utf8 = true, CVolString &param = CVolString{})
    {
        return piv::encoding::UrlStrDecode(GetParam(name, utf8), param, utf8);
    }
    inline CVolString GetParamVolStr(const CharT *name, const bool &utf8 = true, CVolString &param = CVolString{})
    {
        return piv::encoding::UrlStrDecode(GetParam(name, utf8), param, utf8);
    }
    inline CVolString GetParamVolStr(const std::basic_string<CharT> &name, const bool &utf8 = true, CVolString &param = CVolString{})
    {
        return piv::encoding::UrlStrDecode(GetParam(name, utf8), param, utf8);
    }
    inline CVolString GetParamVolStr(const CVolMem &name, const bool &utf8 = true, CVolString &param = CVolString{})
    {
        return piv::encoding::UrlStrDecode(GetParam(name, utf8), param, utf8);
    }
    inline CVolString GetParamVolStr(const CVolString &name, const bool &utf8 = true, CVolString &param = CVolString{})
    {
        return piv::encoding::UrlStrDecode(GetParam(name, utf8), param, utf8);
    }

    /**
     * @brief 取指定参数
     * @param name 参数名称
     * @param utf8 是否为UTF-8编码
     * @return 返回std::basic_string
     */
    inline std::basic_string<CharT> GetDecodeParam(const piv::basic_string_view<CharT> &name, const bool &utf8 = true)
    {
        return piv::encoding::UrlStrDecode(GetParam(name, utf8), utf8);
    }
    inline std::basic_string<CharT> GetDecodeParam(const CharT *name, const bool &utf8 = true)
    {
        return piv::encoding::UrlStrDecode(GetParam(name, utf8), utf8);
    }
    inline std::basic_string<CharT> GetDecodeParam(const std::basic_string<CharT> &name, const bool &utf8 = true)
    {
        return piv::encoding::UrlStrDecode(GetParam(name, utf8), utf8);
    }
    inline std::basic_string<CharT> GetDecodeParam(const CVolMem &name, const bool &utf8 = true)
    {
        return piv::encoding::UrlStrDecode(GetParam(name, utf8), utf8);
    }
    inline std::basic_string<CharT> GetDecodeParam(const CVolString &name, const bool &utf8 = true)
    {
        return piv::encoding::UrlStrDecode(GetParam(name, utf8), utf8);
    }

}; // PivUrlParser

/**
 * @brief 表单构造类
 * @tparam CharT 字符类型
 * @tparam EncodeType 文本编码,按类型的尺寸区分(1=ANSI; 2=UTF-16LE; 3=UTF8;)
 */
template <typename CharT, typename EncodeType>
class PivFormCreater : public CVolObject
{
private:
    CVolMem form;

    /**
     * @brief 添加键名(内部使用)
     * @param key 键名
     * @param count 键名长度
     * @param utf8 是否基于UTF-8进行URL编码
     */
    inline void AddKey(const piv::basic_string_view<CharT> &key, const bool &utf8 = true)
    {
        if (!form.IsEmpty())
        {
            PIV_IF(sizeof(EncodeType) == 2)
            {
                form.AddWChar('&');
            }
            else
            {
                form.AddU8Char('&');
            }
        }
        if (key.empty())
            return;
        if (piv::encoding::UrlEncodeNeed(key.data(), key.size(), false))
        {
            CVolMem buffer;
            piv::encoding::UrlDataEncode(key, utf8, false, buffer);
            form.Append(buffer);
        }
        else
        {
            form.Append(reinterpret_cast<const void *>(key.data()), key.size() * sizeof(CharT));
        }
        PIV_IF(sizeof(EncodeType) == 2)
        {
            form.AddWChar('=');
        }
        else
        {
            form.AddU8Char('=');
        }
    }
    inline void AddKey(const CVolString &key, const bool &utf8 = true)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            AddKey(reinterpret_cast<const CharT *>(key.GetText()), -1, utf8);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            AddKey(reinterpret_cast<const CharT *>(PivW2U{key}.GetText()), -1, utf8);
        }
        else
        {
            AddKey(reinterpret_cast<const CharT *>(PivW2A{key}.GetText()), -1, utf8);
        }
    }
    inline void AddKey(const CVolMem &key, const bool &utf8 = true)
    {
        AddKey(reinterpret_cast<const CharT *>(key.GetPtr()), static_cast<size_t>(key.GetSize()) / sizeof(CharT), utf8);
    }
    inline void AddKey(const std::basic_string<CharT> &key, const bool &utf8 = true)
    {
        AddKey(key.c_str(), -1, utf8);
    }
    inline void AddKey(const CharT *key, const size_t count = -1, const bool &utf8 = true)
    {
        AddKey((count == -1) ? piv::basic_string_view<CharT>{key} : piv::basic_string_view<CharT>{key, count}, utf8);
    }

    /**
     * @brief 添加参数值(内部使用)
     * @param value 参数值
     * @param utf8 是否基于UTF-8进行URL编码
     * @param count 参数值长度
     */
    void AddValue(const piv::basic_string_view<CharT> &value, const bool &utf8)
    {
        if (piv::encoding::UrlEncodeNeed(value.data(), value.size()))
        {
            PIV_IF(sizeof(EncodeType) == 2)
            {
                std::basic_string<CharT> buffer;
                piv::encoding::UrlStrEncode(value, utf8, false, buffer);
                form.Append(reinterpret_cast<const void *>(buffer.c_str()), buffer.size() * sizeof(CharT));
                return;
            }
            else
            {
                CVolMem buffer;
                piv::encoding::UrlDataEncode(value, utf8, false, buffer);
                form.Append(buffer);
                return;
            }
        }
        // 如果上面都没有返回,说明不需要URL编码
        form.Append(reinterpret_cast<const void *>(value.data()), value.size() * sizeof(CharT));
    }
    inline void AddValue(const CVolString &value, const bool &utf8)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            AddValue(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(value.GetText())}, utf8);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            AddValue(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{value}.GetText())}, utf8);
        }
        else
        {
            AddValue(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{value}.GetText())}, utf8);
        }
    }
    inline void AddValue(const CVolMem &value, const bool &utf8)
    {
        AddValue(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(value.GetPtr()), static_cast<size_t>(value.GetSize()) / sizeof(CharT)}, utf8);
    }
    inline void AddValue(const std::basic_string<CharT> &value, const bool &utf8)
    {
        AddValue(piv::basic_string_view<CharT>{value.c_str()}, utf8);
    }
    inline void AddValue(const CharT *value, const bool &utf8, size_t count = -1)
    {
        AddValue((count == static_cast<size_t>(-1)) ? piv::basic_string_view<CharT>{value} : piv::basic_string_view<CharT>{value, count}, utf8);
    }

public:
    /**
     * @brief 默认构造与析构函数
     */
    PivFormCreater()
    {
        form.SetMemAlignSize(2048);
    }
    ~PivFormCreater() {}

    /**
     * @brief 复制和移动构造函数
     * @param other 所欲复制的对象
     */
    PivFormCreater(const PivFormCreater &other)
    {
        form = other.form;
    }
    PivFormCreater(PivFormCreater &&other) noexcept
    {
        form = std::move(other.form);
    }

    /**
     * @brief 赋值操作符
     * @param other 所欲复制的文本视图类
     * @return 返回自身
     */
    PivFormCreater &operator=(const PivFormCreater &other)
    {
        form = other.form;
        return *this;
    }
    PivFormCreater &operator=(PivFormCreater &&other) noexcept
    {
        form = std::move(other.form);
        return *this;
    }

    /**
     * @brief 取展示内容(调试输出)
     * @param strDump 展示内容
     * @param nMaxDumpSize 最大展示数据尺寸
     */
    virtual void GetDumpString(CVolString &strDump, INT nMaxDumpSize) override
    {
        this->GetStr(strDump);
    }

    /**
     * @brief 置对象数据
     * @param stream 基本输入流
     */
    virtual void LoadFromStream(CVolBaseInputStream &stream)
    {
        Clear();
        if (stream.eof())
            return;
        uint32_t Size;
        stream.read(&Size, 4);
        if (!stream.ReadExact(form, Size))
            form.Empty();
    }

    /**
     * @brief 取对象数据
     * @param stream 基本输出流
     */
    virtual void SaveIntoStream(CVolBaseOutputStream &stream)
    {
        uint32_t Size = static_cast<int32_t>(form.GetSize());
        stream.write(&Size, 4);
        stream.write(form.GetPtr(), Size);
    }

    /**
     * @brief 置预分配尺寸
     * @param size 预分配尺寸
     */
    inline void SetAlignSize(const ptrdiff_t size)
    {
        form.SetMemAlignSize(size);
    }

    /**
     * @brief 清空
     */
    inline void Clear()
    {
        form.Free();
    }

    /**
     * @brief 是否为空
     * @return 返回表单是否为空
     */
    inline bool IsEmpty() const
    {
        return form.IsEmpty();
    }

    /**
     * @brief 添加参数
     * @tparam K 键类型
     * @tparam V 值类型
     * @param key 参数键名
     * @param value 参数值
     * @param utf8 是否基于UTF-8进行URL编码
     * @return 返回自身
     */
    template <typename K, typename V>
    PivFormCreater &AddParam(const K &key, const V &value, const bool &utf8)
    {
        AddKey(key, utf8);
        AddValue(value, utf8);
        return *this;
    }

    /**
     * @brief 数据
     * @return 返回字节集参考
     */
    inline CVolMem &GetMem()
    {
        return form;
    }

    /**
     * @brief 到文本视图
     * @return 返回文本视图
     */
    inline piv::basic_string_view<CharT> ToStrView() const
    {
        return piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(form.GetPtr()), static_cast<size_t>(form.GetSize()) / sizeof(CharT)};
    }

    /**
     * @brief 到文本型
     * @return 返回火山文本型
     */
    inline CVolString ToStr() const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return CVolString(reinterpret_cast<const wchar_t *>(form.GetPtr()), form.GetSize() / 2);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return *PivU2Ws{reinterpret_cast<const char *>(form.GetPtr()), static_cast<size_t>(form.GetSize())};
        }
        else
        {
            return *PivA2Ws{reinterpret_cast<const char *>(form.GetPtr()), static_cast<size_t>(form.GetSize())};
        }
    }

    /**
     * @brief 取文本
     * @param s 用来获取数据的文本变量
     */
    inline void GetStr(CVolString &s) const
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            s.SetText(reinterpret_cast<const wchar_t *>(form.GetPtr()), form.GetSize() / 2);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            PivU2Ws{reinterpret_cast<const char *>(form.GetPtr()), static_cast<size_t>(form.GetSize())}.GetStr(s);
        }
        else
        {
            PivA2Ws{reinterpret_cast<const char *>(form.GetPtr()), static_cast<size_t>(form.GetSize())}.GetStr(s);
        }
    }
}; // PivFormCreater

/**
 * @brief 表单解析类
 * @tparam CharT 字符类型
 * @tparam EncodeType 文本编码,按类型的尺寸区分(1=ANSI; 2=UTF-16LE; 3=UTF8;)
 */
template <typename CharT, typename EncodeType>
class PivFormParser : public CVolObject
{
private:
    std::basic_string<CharT> form_str;
    piv::basic_string_view<CharT> form;
    std::map<piv::basic_string_view<CharT>, piv::basic_string_view<CharT>, piv::detail::ci_less<piv::basic_string_view<CharT>>> params;

public:
    /**
     * @brief 默认构造于析构函数
     */
    PivFormParser() {}
    ~PivFormParser() {}

    /**
     * @brief 复制和移动构造函数
     * @param other 所欲复制的对象
     */
    PivFormParser(const PivFormParser &other)
    {
        form = other.form;
        form_str = other.form_str;
        params = other.params;
    }
    PivFormParser(PivFormParser &&other) noexcept
    {
        form = std::move(other.form);
        form_str = std::move(other.form_str);
        params = std::move(other.params);
    }

    /**
     * @brief 带参构造函数
     * @param s 所欲解析的表单参数
     * @param storeBuf 是否缓存表单参数
     * @param count 表单参数长度
     */
    PivFormParser(const CharT *s, const bool &storeBuf = false, const size_t &count = static_cast<size_t>(-1))
    {
        Parse(s, storeBuf, count);
    }
    PivFormParser(const piv::basic_string_view<CharT> &s, const bool &storeBuf = false)
    {
        Parse(s, storeBuf);
    }
    PivFormParser(const std::basic_string<CharT> &s, const bool &storeBuf = false)
    {
        Parse(s, storeBuf);
    }
    PivFormParser(const CVolMem &s, const bool &storeBuf = false)
    {
        Parse(s, storeBuf);
    }
    PivFormParser(const CVolString &s, const bool &storeBuf = false)
    {
        Parse(s, storeBuf);
    }

    /**
     * @brief 赋值操作符
     * @param other 所欲复制的对象
     * @return 返回自身
     */
    PivFormParser &operator=(const PivFormParser &other)
    {
        form = other.form;
        form_str = other.form_str;
        params = other.params;
        return *this;
    }
    PivFormParser &operator=(PivFormParser &&other) noexcept
    {
        form = std::move(other.form);
        form_str = std::move(other.form_str);
        params = std::move(other.params);
        return *this;
    }

    /**
     * @brief 取展示内容(调试输出)
     * @param strDump 展示内容
     * @param nMaxDumpSize 最大展示数据尺寸
     */
    virtual void GetDumpString(CVolString &strDump, INT nMaxDumpSize) override
    {
        strDump.AddFormatLine(L"<表单解析类%s> 总共 %d 个键值对:", (sizeof(EncodeType) == 2) ? L"W" : (sizeof(EncodeType) == 3 ? L"U" : L"A"), (int32_t)params.size());
        int32_t i = 0;
        for (auto it = params.cbegin(); it != params.cend(); it++, i++)
        {
            PIV_IF(sizeof(EncodeType) == 2)
            {
                strDump.AddFormatLine(L"%d. %s = %s", i, std::wstring{reinterpret_cast<const wchar_t *>(it->first.data()), it->first.size()}.c_str(), std::wstring{reinterpret_cast<const wchar_t *>(it->second.data()), it->second.size()}.c_str());
            }
            PIV_ELSE_IF(sizeof(EncodeType) == 3)
            {
                strDump.AddFormatLine(L"%d. %s = %s", i, PivU2Ws{reinterpret_cast<const char *>(it->first.data()), it->first.size()}.GetText(), PivU2Ws{reinterpret_cast<const char *>(it->second.data()), it->second.size()}.GetText());
            }
            else
            {
                strDump.AddFormatLine(L"%d. %s = %s", i, PivA2Ws{reinterpret_cast<const char *>(it->first.data()), it->first.size()}.GetText(), PivA2Ws{reinterpret_cast<const char *>(it->second.data()), it->second.size()}.GetText());
            }
        }
    }

    /**
     * @brief 置对象数据
     * @param stream 基本输入流
     */
    virtual void LoadFromStream(CVolBaseInputStream &stream)
    {
        Clear();
        if (stream.eof())
            return;
        uint32_t Size;
        stream.read(&Size, 4);
        form_str.resize(Size / sizeof(CharT));
        if (stream.ReadExact(const_cast<CharT *>(form_str.data()), Size))
        {
            if (form_str.back() == '\0')
                form_str.pop_back();
            Parse(form_str, false);
        }
        else
        {
            form_str.clear();
        }
    }

    /**
     * @brief 取对象数据
     * @param stream 基本输出流
     */
    virtual void SaveIntoStream(CVolBaseOutputStream &stream)
    {
        uint32_t Size = static_cast<uint32_t>((form_str.size() + 1) * sizeof(CharT));
        stream.write(&Size, 4);
        stream.write(form_str.data(), Size);
    }

    /**
     * @brief 清空
     */
    inline void Clear()
    {
        form_str.clear();
        form.swap(piv::basic_string_view<CharT>{});
        params.clear();
    }

    /**
     * @brief 是否有缓存
     * @return 返回真表示有缓存
     */
    inline bool HasBuffer() const
    {
        return !form_str.empty();
    }

    /**
     * @brief 解析表单参数
     * @param s 所欲解析的表单参数
     * @param storeBuf 是否缓存表单参数
     * @param count 表单参数长度
     */
    bool Parse(const piv::basic_string_view<CharT> &s, const bool &storeBuf = false)
    {
        Clear();
        if (s.empty())
            return false;
        if (storeBuf)
        {
            form_str.assign(s.data(), s.size());
            form = form_str;
        }
        else
        {
            form = s;
        }
        if (form.front() == '{')
        {
            form_str.clear();
            form.swap(piv::basic_string_view<CharT>{});
            return false;
        }
        size_t pos1 = 0, pos2 = 0, pos3 = 0;
        // 寻找和获取参数
        pos2 = form.find('&', 0);
        while (pos2 != piv::basic_string_view<CharT>::npos)
        {
            pos3 = form.find('=', pos1);
            if (pos3 != piv::basic_string_view<CharT>::npos)
            {
                params[form.substr(pos1, pos3 - pos1)] = form.substr(pos3 + 1, pos2 - pos3 - 1);
            }
            pos1 = pos2 + 1;
            pos2 = form.find('&', pos1);
        }
        pos3 = form.find('=', pos1);
        if (pos3 != piv::basic_string_view<CharT>::npos)
        {
            params[form.substr(pos1, pos3 - pos1)] = form.substr(pos3 + 1, form.size() - pos3 - 1);
        }
        if (params.empty())
        {
            form_str.clear();
            form.swap(piv::basic_string_view<CharT>{});
            return false;
        }
        else
        {
            return true;
        }
    }
    inline bool Parse(const CharT *s, const bool &storeBuf = false, const size_t &count = static_cast<size_t>(-1))
    {
        return Parse((count == static_cast<size_t>(-1)) ? piv::basic_string_view<CharT>{s} : piv::basic_string_view<CharT>{s, count}, storeBuf);
    }
    inline bool Parse(const std::basic_string<CharT> &s, const bool &storeBuf = false)
    {
        return Parse(piv::basic_string_view<CharT>{s}, storeBuf);
    }
    inline bool Parse(const CVolMem &s, const bool &storeBuf = false)
    {
        return Parse(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT)}, storeBuf);
    }
    inline bool Parse(const CVolString &s, const bool &storeBuf = false)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return Parse(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetText())}, storeBuf);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return Parse(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{s}.GetText())}, true);
        }
        else
        {
            return Parse(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{s}.GetText())}, true);
        }
    }

    /**
     * @brief 取指定参数
     * @param name 参数名称
     * @return 返回std::basic_string_view
     */
    inline piv::basic_string_view<CharT> GetParam(const piv::basic_string_view<CharT> &name, const bool &utf8 = true)
    {
        if (params.find(name) != params.end())
        {
            return params[name];
        }
        else if (piv::encoding::UrlEncodeNeed(name.data(), name.size()))
        {
            return params[piv::basic_string_view<CharT>{piv::encoding::UrlStrEncode(name, utf8, false)}];
        }
        return piv::basic_string_view<CharT>{};
    }
    inline piv::basic_string_view<CharT> GetParam(const CharT *name, const bool &utf8 = true, const size_t &count = static_cast<size_t>(-1))
    {
        return GetParam((count == static_cast<size_t>(-1)) ? piv::basic_string_view<CharT>{name} : piv::basic_string_view<CharT>{name, count}, utf8);
    }
    inline piv::basic_string_view<CharT> GetParam(const std::basic_string<CharT> &name, const bool &utf8 = true)
    {
        return GetParam(piv::basic_string_view<CharT>{name}, utf8);
    }
    inline piv::basic_string_view<CharT> GetParam(const CVolMem &name, const bool &utf8 = true)
    {
        return GetParam(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(name.GetPtr()), static_cast<size_t>(name.GetSize()) / sizeof(CharT)}, utf8);
    }
    inline piv::basic_string_view<CharT> GetParam(const CVolString &name, const bool &utf8 = true)
    {
        PIV_IF(sizeof(EncodeType) == 2)
        {
            return GetParam(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(name.GetText())}, utf8);
        }
        PIV_ELSE_IF(sizeof(EncodeType) == 3)
        {
            return GetParam(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{name}.GetText())}, utf8);
        }
        else
        {
            return GetParam(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{name}.GetText())}, utf8);
        }
    }

    /**
     * @brief 取解码后的参数
     * @param name 参数名称
     * @param utf8 是否为UTF-8编码
     * @return
     */
    template <typename T>
    std::basic_string<CharT> GetDecodeParam(const T &name, const bool &utf8 = true)
    {
        return piv::encoding::UrlStrDecode(GetParam(name, utf8), utf8);
    }

    /**
     * @brief 取解码后的参数
     * @param name 参数名称
     * @param utf8 是否为UTF-8编码
     * @param value 参数返回到此文本型中
     * @return
     */
    template <typename T>
    CVolString &GetParamStr(const T &name, const bool &utf8 = true, CVolString &value = CVolString{})
    {
        return piv::encoding::UrlStrDecode(GetParam(name, utf8), value, utf8);
    }

    /**
     * @brief 取所有键名
     * @param array 存放键名数组
     * @return
     */
    size_t EnumKey(std::vector<piv::basic_string_view<CharT>> &array) const
    {
        array.clear();
        for (auto it = params.cbegin(); it != params.cend(); it++)
        {
            array.push_back(it->first);
        }
        return array.size();
    }
}; // PivFormParser

/**
 * @brief URL地址构建类
 */
class PivUrlCreaterW : public CVolObject
{
private:
    CVolString url;
    INT_P host_len;
    INT_P path_len;

    /**
     * @brief 添加键名(内部使用)
     * @param key 所欲添加的键名
     * @param len 键名的字符长度
     * @param utf8 URL编码时是否为UTF-8
     */
    inline void AddKey(const piv::wstring_view &key, const bool &utf8 = true)
    {
        if (url.GetLength() == host_len + path_len)
            url.AddChar('?');
        else
            url.AddChar('&');
        if (key.empty())
            return;
        if (piv::encoding::UrlEncodeNeed(key.data(), key.size(), false))
        {
            CVolMem buffer;
            piv::encoding::UrlDataEncode(key, utf8, false, buffer);
            url.AddText(buffer.GetTextPtr(), buffer.GetSize() / 2);
        }
        else
        {
            url.AddText(key.data(), key.size());
        }
        url.AddChar('=');
    }
    inline void AddKey(const wchar_t *key, const size_t &len, const bool &utf8 = true)
    {
        AddKey(piv::wstring_view{key, len}, utf8);
    }
    inline void AddKey(const wchar_t *key, const bool &utf8 = true)
    {
        AddKey(piv::wstring_view{key}, utf8);
    }
    inline void AddKey(const CVolString &key, const bool &utf8 = true)
    {
        AddKey(piv::wstring_view{key.GetText()}, utf8);
    }
    inline void AddKey(const std::wstring &key, const bool &utf8 = true)
    {
        AddKey(piv::wstring_view{key.c_str()}, utf8);
    }
    inline void AddKey(const CVolMem &key, const bool &utf8 = true)
    {
        AddKey(piv::wstring_view{key.GetTextPtr(), static_cast<size_t>(key.GetSize() / 2)}, utf8);
    }

    /**
     * @brief 添加值(内部使用)
     * @param key 所欲添加的值
     * @param value 值的字符长度
     * @param utf8 URL编码时是否为UTF-8
     */
    inline void AddValue(const wchar_t *value, const size_t &len, const bool &utf8 = true)
    {
        if (piv::encoding::UrlEncodeNeed(value, len, false))
        {
            CVolMem buffer;
            piv::encoding::UrlDataEncode(piv::wstring_view{value, len}, utf8, false, buffer);
            url.AddText(buffer.GetTextPtr(), buffer.GetSize() / 2);
        }
        else
        {
            url.AddText(value, len);
        }
    }
    inline void AddValue(const wchar_t *value, const bool &utf8 = true)
    {
        AddValue(value, wcslen(value), utf8);
    }
    inline void AddValue(const CVolString &value, const bool &utf8 = true)
    {
        AddValue(value.GetText(), static_cast<size_t>(value.GetLength()), utf8);
    }
    inline void AddValue(const std::wstring &value, const bool &utf8 = true)
    {
        AddValue(value.c_str(), value.size(), utf8);
    }
    inline void AddValue(const piv::wstring_view &value, const bool &utf8 = true)
    {
        AddValue(value.data(), value.size(), utf8);
    }
    inline void AddValue(const CVolMem &value, const bool &utf8 = true)
    {
        AddValue(value.GetTextPtr(), static_cast<size_t>(value.GetSize() / 2), utf8);
    }

public:
    /**
     * @brief 默认构造于析构函数
     */
    PivUrlCreaterW()
    {
        url.SetNumAlignChars(1024);
        host_len = 0;
        path_len = 0;
    }
    ~PivUrlCreaterW() {}

    /**
     * @brief 复制和移动构造函数
     * @param other 所欲复制的对象
     */
    PivUrlCreaterW(const PivUrlCreaterW &other)
    {
        url = other.url;
        host_len = other.host_len;
        path_len = other.path_len;
    }
    PivUrlCreaterW(PivUrlCreaterW &&other) noexcept
    {
        url = std::move(other.url);
        host_len = other.host_len;
        path_len = other.path_len;
    }

    PivUrlCreaterW &operator=(const PivUrlCreaterW &other)
    {
        url = other.url;
        host_len = other.host_len;
        path_len = other.path_len;
        return *this;
    }
    PivUrlCreaterW &operator=(PivUrlCreaterW &&other) noexcept
    {
        url = std::move(other.url);
        host_len = other.host_len;
        path_len = other.path_len;
        return *this;
    }

    /**
     * @brief 取地址
     * @return
     */
    inline CVolString &data() noexcept
    {
        return url;
    }

    /**
     * @brief 是否为空
     * @return 返回本对象是否为空
     */
    inline bool IsEmpty() const noexcept
    {
        return url.IsEmpty();
    }

    /**
     * @brief 清空
     */
    inline void Clear() noexcept
    {
        url.Empty();
    }

    /**
     * @brief 置域名
     * @param host 所欲设置的域名
     * @param len 域名的字符长度
     * @return 返回自身
     */
    inline PivUrlCreaterW &SetHost(const wchar_t *host, const INT_P &len)
    {
        if (host_len > 0)
            url.RemoveChar(0, host_len);
        host_len = len;
        if (host_len > 0)
        {
            if (host[host_len - 1] == '/')
                host_len--;
            url.InsertText(0, host, host_len);
        }
        return *this;
    }
    inline PivUrlCreaterW &SetHost(const wchar_t *host)
    {
        return SetHost(host, static_cast<INT_P>(wcslen(host)));
    }
    inline PivUrlCreaterW &SetHost(const CVolString &host)
    {
        return SetHost(host.GetText(), host.GetLength());
    }
    inline PivUrlCreaterW &SetHost(const std::wstring &host)
    {
        return SetHost(host.c_str(), static_cast<INT_P>(host.size()));
    }
    inline PivUrlCreaterW &SetHost(const piv::wstring_view &host)
    {
        return SetHost(host.data(), static_cast<INT_P>(host.size()));
    }
    inline PivUrlCreaterW &SetHost(const CVolMem &host)
    {
        return SetHost(host.GetTextPtr(), host.GetSize() / 2);
    }

    /**
     * @brief 置路径
     * @param path 所欲设置的路径
     * @param len 路径的字符长度
     * @return 返回自身
     */
    inline PivUrlCreaterW &SetPath(const wchar_t *path, const INT_P &len)
    {
        if (path_len > 0)
            url.RemoveChar(host_len, path_len);
        path_len = 0;
        if (len > 0)
        {
            if (path[0] != '/')
            {
                url.AddChar('/');
                path_len++;
            }
            if (piv::encoding::UrlEncodeNeed(path, len, true))
            {
                CVolMem buffer;
                piv::encoding::UrlDataEncode(piv::wstring_view{path, static_cast<size_t>(len)}, true, true, buffer);
                path_len += buffer.GetSize() / 2;
                url.InsertText(host_len, buffer.GetTextPtr(), path_len);
            }
            else
            {
                url.InsertText(host_len, path, len);
                path_len += len;
            }
        }
        return *this;
    }
    inline PivUrlCreaterW &SetPath(const wchar_t *path)
    {
        return SetPath(path, static_cast<INT_P>(wcslen(path)));
    }
    inline PivUrlCreaterW &SetPath(const CVolString &path)
    {
        return SetPath(path.GetText(), path.GetLength());
    }
    inline PivUrlCreaterW &SetPath(const std::wstring &path)
    {
        return SetPath(path.c_str(), static_cast<INT_P>(path.size()));
    }
    inline PivUrlCreaterW &SetPath(const piv::wstring_view &path)
    {
        return SetPath(path.data(), static_cast<INT_P>(path.size()));
    }
    inline PivUrlCreaterW &SetPath(const CVolMem &path)
    {
        return SetPath(path.GetTextPtr(), path.GetSize() / 2);
    }

    /**
     * @brief 添加参数
     * @tparam K 键名的类型
     * @tparam V 值的类型
     * @param key 所欲添加的键名
     * @param value 所欲添加的值
     * @param utf8 URL编码时是否为UTF-8
     * @return 返回自身
     */
    template <typename K, typename V>
    PivUrlCreaterW &AddParam(const K &key, const V &value, const bool &utf8)
    {
        AddKey(key, utf8);
        AddValue(value, utf8);
        return *this;
    }

    virtual void GetDumpString(CVolString &strDump, INT nMaxDumpSize) override
    {
        strDump = url;
    }

    /**
     * @brief 置对象数据
     * @param stream 基本输入流
     */
    virtual void LoadFromStream(CVolBaseInputStream &stream)
    {
        Clear();
        if (stream.eof())
            return;
        uint32_t Size;
        stream.read(&Size, 4);
        url.SetLength((Size / 2) - 1);
        if (stream.ReadExact(const_cast<wchar_t *>(url.GetText()), Size))
        {
            stream.read(&host_len, 4);
            stream.read(&path_len, 4);
        }
        else
        {
            url.Empty();
        }
    }

    /**
     * @brief 取对象数据
     * @param stream 基本输出流
     */
    virtual void SaveIntoStream(CVolBaseOutputStream &stream)
    {
        uint32_t Size = static_cast<uint32_t>((url.GetLength() + 1) * 2);
        stream.write(&Size, 4);
        stream.write(url.GetText(), Size);
        stream.write(&host_len, 4);
        stream.write(&path_len, 4);
    }

}; // PivUrlCreaterW

using PivUrlParserA = PivUrlParser<char, char>;
using PivUrlParserU = PivUrlParser<char, piv::utf8>;
using PivUrlParserW = PivUrlParser<wchar_t, piv::utf16_le>;

using PivFormCreaterA = PivFormCreater<char, char>;
using PivFormCreaterU = PivFormCreater<char, piv::utf8>;
using PivFormCreaterW = PivFormCreater<wchar_t, piv::utf16_le>;

using PivFormParserA = PivFormParser<char, int32_t>;
using PivFormParserU = PivFormParser<char, piv::utf8>;
using PivFormParserW = PivFormParser<wchar_t, piv::utf16_le>;

#endif // _PIV_URL_HPP
