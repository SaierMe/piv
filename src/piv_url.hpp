/*********************************************\
 * 火山视窗PIV模块 - URL辅助                 *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 版本: 2023/02/09                          *
\*********************************************/

#ifndef _PIV_URL_HPP
#define _PIV_URL_HPP

#include "piv_string.hpp"
#include <map>

/**
 * @brief URL地址解析类
 * @tparam CharT 字符类型
 * @tparam EncodeType 文本编码,按类型的尺寸区分(1=ANSI;2=UTF-16LE;4=UTF8)
 */
template <typename CharT, typename EncodeType = CharT>
class PivUrlParser
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
    std::map<piv::basic_string_view<CharT>, piv::basic_string_view<CharT>> params;

public:
    /**
     * @brief 默认构造于析构函数
     */
    PivUrlParser() {}
    ~PivUrlParser() {}

    /**
     * @brief 复制构造函数
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
    PivUrlParser(const CharT *urlAdress, const bool storeBuf = false, const bool simple = false, size_t count = static_cast<size_t>(-1))
    {
        Parse(urlAdress, storeBuf, simple, count);
    }
    PivUrlParser(const piv::basic_string_view<CharT> &urlAdress, const bool storeBuf = false, const bool simple = false)
    {
        Parse(urlAdress, storeBuf, simple);
    }
    PivUrlParser(const std::basic_string<CharT> &urlAdress, const bool storeBuf = false, const bool simple = false)
    {
        Parse(urlAdress, storeBuf, simple);
    }
    PivUrlParser(const CVolMem &urlAdress, const bool storeBuf = false, const bool simple = false)
    {
        Parse(urlAdress, storeBuf, simple);
    }
    PivUrlParser(const CVolString &urlAdress, const bool storeBuf = false, const bool simple = false)
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

    /**
     * @brief 解析URL地址
     * @param urlAdress 所欲解析的URL地址
     * @param simple 是否简易解析
     * @param storeBuf 是否缓存地址数据
     * @param count URL地址长度
     */
    void Parse(const piv::basic_string_view<CharT> &urlAdress, const bool storeBuf = false, const bool simple = false)
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
        piv::trim_left(url);
        piv::trim_right(url);
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
                std::transform(scheme.begin(), scheme.end(), scheme_lower.begin(), std::tolower);
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
        pos1 = 0;
        pos2 = query.find('&', 0);
        while (pos2 != piv::basic_string_view<CharT>::npos)
        {
            pos3 = query.find('=', pos1);
            if (pos3 != piv::basic_string_view<CharT>::npos)
                params[query.substr(pos1, pos3 - pos1)] = query.substr(pos3 + 1, pos2 - pos3 - 1);
            pos1 = pos2 + 1;
            pos2 = query.find('&', pos1);
        }
        pos3 = query.find('=', pos1);
        if (pos3 != piv::basic_string_view<CharT>::npos)
            params[query.substr(pos1, pos3 - pos1)] = query.substr(pos3 + 1, query.size() - pos3 - 1);
    }
    inline void Parse(const CharT *urlAdress, const bool storeBuf = false, const bool simple = false, size_t count = static_cast<size_t>(-1))
    {
        Parse((count == static_cast<size_t>(-1)) ? piv::basic_string_view<CharT>{urlAdress} : piv::basic_string_view<CharT>{urlAdress, count}, storeBuf, simple);
    }
    inline void Parse(const std::basic_string<CharT> &urlAdress, const bool storeBuf = false, const bool simple = false)
    {
        Parse(piv::basic_string_view<CharT>{urlAdress}, storeBuf, simple);
    }
    inline void Parse(const CVolMem &urlAdress, const bool storeBuf = false, const bool simple = false)
    {
        Parse(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(urlAdress.GetPtr()), static_cast<size_t>(urlAdress.GetSize()) / sizeof(CharT)}, storeBuf, simple);
    }
    inline void Parse(const CVolString &urlAdress, const bool storeBuf = false, const bool simple = false)
    {
        if (sizeof(EncodeType) == 2)
            Parse(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(urlAdress.GetText())}, storeBuf, simple);
        else if (sizeof(EncodeType) == 4)
            Parse(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{urlAdress}.GetText())}, true);
        else
            Parse(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{urlAdress}.GetText())}, true, simple);
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
    inline piv::basic_string_view<CharT> &Scheme() const
    {
        return scheme;
    }
    inline piv::basic_string_view<CharT> &User() const
    {
        return user;
    }
    inline piv::basic_string_view<CharT> &Password() const
    {
        return password;
    }
    inline piv::basic_string_view<CharT> &Host() const
    {
        return host;
    }
    inline piv::basic_string_view<CharT> &Port() const
    {
        return port;
    }
    inline piv::basic_string_view<CharT> &Path() const
    {
        return path;
    }
    inline piv::basic_string_view<CharT> &Query() const
    {
        return query;
    }
    inline piv::basic_string_view<CharT> &Fragment() const
    {
        return fragment;
    }

    /**
     * @brief 取协议
     * @return 返回文本型
     */
    inline CVolString GetScheme() const
    {
        if (sizeof(EncodeType) == 2)
            return CVolString(scheme.data(), scheme.size());
        else if (sizeof(EncodeType) == 4)
            return PivU2W{reinterpret_cast<const char *>(scheme.data()), scheme.size()};
        else
            return PivA2W{reinterpret_cast<const char *>(scheme.data()), scheme.size()};
    }

    /**
     * @brief 取账户
     * @param urlDecode 是否URL解码
     * @param utf8 是否为UTF-8编码
     * @return 返回文本
     */
    inline CVolString GetUser(const bool urlDecode = false, const bool utf8 = true) const
    {
        std::basic_string<CharT> str = piv::encoding::UrlStrDecode(user, urlDecode, utf8);
        if (sizeof(EncodeType) == 2)
            return CVolString(str.c_str());
        else if (sizeof(EncodeType) == 4)
            return PivU2W{reinterpret_cast<const char *>(str.data()), str.size()};
        else
            return PivA2W{reinterpret_cast<const char *>(str.data()), str.size()};
    }
    inline std::basic_string<CharT> DecodeUser(const bool urlDecode = false, const bool utf8 = true) const
    {
        return piv::encoding::UrlStrDecode(user, urlDecode, utf8);
    }

    /**
     * @brief 取密码
     * @param urlDecode 是否URL解码
     * @param utf8 是否为UTF-8编码
     * @return 返回文本
     */
    inline CVolString GetPassword(const bool urlDecode = false, const bool utf8 = true) const
    {
        std::basic_string<CharT> str = piv::encoding::UrlStrDecode(password, urlDecode, utf8);
        if (sizeof(EncodeType) == 2)
            return CVolString(str.c_str());
        else if (sizeof(EncodeType) == 4)
            return PivU2W{reinterpret_cast<const char *>(str.data()), str.size()};
        else
            return PivA2W{reinterpret_cast<const char *>(str.data()), str.size()};
    }
    inline std::basic_string<CharT> DecodePassword(const bool urlDecode = false, const bool utf8 = true) const
    {
        return piv::encoding::UrlStrDecode(password, urlDecode, utf8);
    }

    /**
     * @brief 取域名
     * @return 返回文本型
     */
    inline CVolString GetHost() const
    {
        if (sizeof(EncodeType) == 2)
            return CVolString(host.data(), host.size());
        else if (sizeof(EncodeType) == 4)
            return PivU2W{reinterpret_cast<const char *>(host.data()), host.size()};
        else
            return PivA2W{reinterpret_cast<const char *>(host.data()), host.size()};
    }

    /**
     * @brief 取路径
     * @return 返回文本型
     */
    inline CVolString GetPath() const
    {
        if (sizeof(EncodeType) == 2)
            return CVolString(path.data(), path.size());
        else if (sizeof(EncodeType) == 4)
            return PivU2W{reinterpret_cast<const char *>(path.data()), path.size()};
        else
            return PivA2W{reinterpret_cast<const char *>(path.data()), path.size()};
    }

    /**
     * @brief 取端口
     * @return 返回文本型
     */
    inline CVolString GetPort() const
    {
        if (sizeof(EncodeType) == 2)
            return CVolString(port.data(), port.size());
        else if (sizeof(EncodeType) == 4)
            return PivU2W{reinterpret_cast<const char *>(port.data()), port.size()};
        else
            return PivA2W{reinterpret_cast<const char *>(port.data()), port.size()};
    }

    /**
     * @brief 取参数
     * @return 返回文本型
     */
    inline CVolString GetQuery() const
    {
        if (sizeof(EncodeType) == 2)
            return CVolString(query.data(), query.size());
        else if (sizeof(EncodeType) == 4)
            return PivU2W{reinterpret_cast<const char *>(query.data()), query.size()};
        else
            return PivA2W{reinterpret_cast<const char *>(query.data()), query.size()};
    }

    /**
     * @brief 取段落
     * @return 返回文本型
     */
    inline CVolString GetFragment() const
    {
        if (sizeof(EncodeType) == 2)
            return CVolString(fragment.data(), fragment.size());
        else if (sizeof(EncodeType) == 4)
            return PivU2W{reinterpret_cast<const char *>(fragment.data()), fragment.size()};
        else
            return PivA2W{reinterpret_cast<const char *>(fragment.data()), fragment.size()};
    }

    /**
     * @brief 取指定参数
     * @param name 参数名称
     * @param urlDecode 是否URL解码
     * @param utf8 是否为UTF-8编码
     * @return 返回文本型
     */
    inline CVolString GetParamVolStr(const piv::basic_string_view<CharT> &name, const bool urlDecode = false, const bool utf8 = true)
    {
        std::basic_string<CharT> str = piv::encoding::UrlStrDecode(params[name], urlDecode, utf8);
        if (sizeof(EncodeType) == 2)
            return CVolString(str.c_str());
        else if (sizeof(EncodeType) == 4)
            return PivU2W{reinterpret_cast<const char *>(str.data()), str.size()};
        else
            return PivA2W{reinterpret_cast<const char *>(str.data()), str.size()};
    }
    inline CVolString GetParamVolStr(const CharT *name, const bool urlDecode = false, const bool utf8 = true)
    {
        return GetParamVolStr(piv::basic_string_view<CharT>{name}, urlDecode, utf8);
    }
    inline CVolString GetParamVolStr(const std::basic_string<CharT> &name, const bool urlDecode = false, const bool utf8 = true)
    {
        return GetParamVolStr(piv::basic_string_view<CharT>{name.data(), name.size()}, urlDecode, utf8);
    }
    inline CVolString GetParamVolStr(const CVolMem &name, const bool urlDecode = false, const bool utf8 = true)
    {
        return GetParamVolStr(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(name.GetPtr()), static_cast<size_t>(name.GetSize()) / sizeof(CharT)}, urlDecode, utf8);
    }
    inline CVolString GetParamVolStr(const CVolString &name, const bool urlDecode = false, const bool utf8 = true)
    {
        if (sizeof(EncodeType) == 2)
            return GetParamVolStr(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(name.GetText()), static_cast<size_t>(name.GetLength())}, urlDecode, utf8);
        else if (sizeof(EncodeType) == 4)
            return GetParamVolStr(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{name}.GetText())}, urlDecode, utf8);
        else
            return GetParamVolStr(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{name}.GetText())}, urlDecode, utf8);
    }

    /**
     * @brief 取指定参数
     * @param name 参数名称
     * @param urlDecode 是否URL解码
     * @param utf8 是否为UTF-8编码
     * @return 返回std::basic_string
     */
    inline std::basic_string<CharT> GetParam(const piv::basic_string_view<CharT> &name, const bool urlDecode = false, const bool utf8 = true)
    {
        return piv::encoding::UrlStrDecode(params[name], urlDecode, utf8);
    }
    inline std::basic_string<CharT> GetParam(const CharT *name, const bool urlDecode = false, const bool utf8 = true)
    {
        return GetParam(piv::basic_string_view<CharT>{name}, urlDecode, utf8);
    }
    inline std::basic_string<CharT> GetParam(const std::basic_string<CharT> &name, const bool urlDecode = false, const bool utf8 = true)
    {
        return GetParam(piv::basic_string_view<CharT>{name.data(), name.size()}, urlDecode, utf8);
    }
    inline std::basic_string<CharT> GetParam(const CVolMem &name, const bool urlDecode = false, const bool utf8 = true)
    {
        return GetParam(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(name.GetPtr()), static_cast<size_t>(name.GetSize()) / sizeof(CharT)}, urlDecode, utf8);
    }
    inline std::basic_string<CharT> GetParam(const CVolString &name, const bool urlDecode = false, const bool utf8 = true)
    {
        if (sizeof(EncodeType) == 2)
            return GetParam(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(name.GetText()), static_cast<size_t>(name.GetLength())}, urlDecode, utf8);
        else if (sizeof(EncodeType) == 4)
            return GetParam(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{name}.GetText())}, urlDecode, true);
        else
            return GetParam(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{name}.GetText())}, urlDecode, false);
    }
}; // PivUrlParser

/**
 * @brief 表单构造类
 * @tparam CharT 字符类型
 * @tparam EncodeType 文本编码,按类型的尺寸区分(1=ANSI;2=UTF-16LE;4=UTF8)
 */
template <typename CharT, typename EncodeType = CharT>
class PivFormCreater : public CVolObject
{
private:
    CVolMem form;

    /**
     * @brief 添加键名(内部使用)
     * @param key 键名
     * @param count 键名长度
     */
    inline void AddKey(const CharT *key, const size_t count = -1)
    {
        INT_P npSize = static_cast<INT_P>((count == -1) ? ((sizeof(EncodeType) == 2) ? wcslen(reinterpret_cast<const wchar_t *>(key)) * 2 : strlen(reinterpret_cast<const char *>(key))) : count);
        if (!form.IsEmpty())
        {
            if (sizeof(EncodeType) == 2)
                form.AddWChar('&');
            else
                form.AddU8Char('&');
        }
        form.Append(reinterpret_cast<const void *>(key), npSize);
    }
    inline void AddKey(const CVolString &key)
    {
        if (sizeof(EncodeType) == 2)
            AddKey(reinterpret_cast<const CharT *>(key.GetText()));
        else if (sizeof(EncodeType) == 4)
            AddKey(reinterpret_cast<const CharT *>(PivW2U{key}.GetText()));
        else
            AddKey(reinterpret_cast<const CharT *>(PivW2A{key}.GetText()));
    }
    inline void AddKey(const CVolMem &value)
    {
        AddKey(reinterpret_cast<const CharT *>(value.GetPtr()), static_cast<size_t>(value.GetSize()) / sizeof(CharT));
    }
    inline void AddKey(const std::basic_string<CharT> &value)
    {
        AddKey(value.c_str());
    }
    inline void AddKey(const piv::basic_string_view<CharT> &value)
    {
        AddKey(value.data(), value.size());
    }

    /**
     * @brief 添加参数值(内部使用)
     * @param value 参数值
     * @param url_encode 是否URL编码
     * @param utf8 是否基于UTF-8进行URL编码
     * @param count 参数值长度
     */
    void AddValue(const piv::basic_string_view<CharT> &value, const bool url_encode, const bool utf8)
    {
        if (sizeof(EncodeType) == 2)
            form.AddWChar('=');
        else
            form.AddU8Char('=');
        if (url_encode && value.size() > 0)
        {
            if (sizeof(EncodeType) == 2)
            {
                std::basic_string<CharT> buffer;
                piv::encoding::UrlStrEncode(value, buffer, utf8);
                form.Append(reinterpret_cast<const void *>(buffer.c_str()), buffer.size() * sizeof(CharT));
                return;
            }
            else
            {
                CVolMem buffer;
                piv::encoding::UrlStrEncode(value, buffer, utf8);
                form.Append(buffer);
                return;
            }
        }
        // 如果上面都没有返回,说明不需要URL编码
        form.Append(reinterpret_cast<const void *>(value.data()), value.size() * sizeof(CharT));
    }
    inline void AddValue(const CVolString &value, const bool url_encode, const bool utf8)
    {
        if (sizeof(EncodeType) == 2)
            AddValue(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(value.GetText())}, url_encode, utf8);
        else if (sizeof(EncodeType) == 4)
            AddValue(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{value}.GetText())}, url_encode, utf8);
        else
            AddValue(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{value}.GetText())}, url_encode, utf8);
    }
    inline void AddValue(const CVolMem &value, const bool url_encode, const bool utf8)
    {
        AddValue(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(value.GetPtr()), static_cast<size_t>(value.GetSize()) / sizeof(CharT)}, url_encode, utf8);
    }
    inline void AddValue(const std::basic_string<CharT> &value, const bool url_encode, const bool utf8)
    {
        AddValue(piv::basic_string_view<CharT>{value.c_str()}, url_encode, utf8);
    }
    inline void AddValue(const CharT *value, const bool url_encode, const bool utf8, const size_t count = -1)
    {
        AddValue((count == static_cast<size_t>(-1)) ? piv::basic_string_view<CharT>{value} : piv::basic_string_view<CharT>{value, count}, url_encode, utf8);
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
     * @brief 复制构造函数
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
     * @param url_encode 是否URL编码
     * @param utf8 是否基于UTF-8进行URL编码
     * @return 返回自身
     */
    template <typename K, typename V>
    PivFormCreater &AddParam(K key, V value, const bool url_encode, const bool utf8)
    {
        AddKey(key);
        AddValue(value, url_encode, utf8);
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
        if (sizeof(EncodeType) == 2)
            return CVolString(reinterpret_cast<const wchar_t *>(form.GetPtr()), form.GetSize() / 2);
        else if (sizeof(EncodeType) == 4)
            return PivU2W{reinterpret_cast<const char *>(form.GetPtr()), static_cast<size_t>(form.GetSize())};
        else
            return PivA2W{reinterpret_cast<const char *>(form.GetPtr()), static_cast<size_t>(form.GetSize())};
    }

    /**
     * @brief 取文本
     * @param s 用来获取数据的文本变量
     */
    inline void GetStr(CVolString &s) const
    {
        if (sizeof(EncodeType) == 2)
            s.SetText(reinterpret_cast<const wchar_t *>(form.GetPtr()), form.GetSize() / 2);
        else if (sizeof(EncodeType) == 4)
            PivU2W{reinterpret_cast<const char *>(form.GetPtr()), static_cast<size_t>(form.GetSize())}.GetStr(s);
        else
            PivA2W{reinterpret_cast<const char *>(form.GetPtr()), static_cast<size_t>(form.GetSize())}.GetStr(s);
    }
}; // PivFormCreater

/**
 * @brief 表单解析类
 * @tparam CharT 字符类型
 * @tparam EncodeType 文本编码,按类型的尺寸区分(1=ANSI;2=UTF-16LE;4=UTF8)
 */
template <typename CharT, typename EncodeType = CharT>
class PivFormParser
{
private:
    std::basic_string<CharT> form_str;
    piv::basic_string_view<CharT> form;
    std::map<piv::basic_string_view<CharT>, piv::basic_string_view<CharT>> params;

public:
    /**
     * @brief 默认构造于析构函数
     */
    PivFormParser() {}
    ~PivFormParser() {}

    /**
     * @brief 复制构造函数
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
    PivFormParser(const CharT *s, const bool storeBuf = false, const size_t count = static_cast<size_t>(-1))
    {
        Parse(s, storeBuf, count);
    }
    PivFormParser(const piv::basic_string_view<CharT> &s, const bool storeBuf = false)
    {
        Parse(s, storeBuf);
    }
    PivFormParser(const std::basic_string<CharT> &s, const bool storeBuf = false)
    {
        Parse(s, storeBuf);
    }
    PivFormParser(const CVolMem &s, const bool storeBuf = false)
    {
        Parse(s, storeBuf);
    }
    PivFormParser(const CVolString &s, const bool storeBuf = false)
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
    bool Parse(const piv::basic_string_view<CharT> &s, const bool storeBuf = false)
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
                params[form.substr(pos1, pos3 - pos1)] = form.substr(pos3 + 1, pos2 - pos3 - 1);
            pos1 = pos2 + 1;
            pos2 = form.find('&', pos1);
        }
        pos3 = form.find('=', pos1);
        if (pos3 != piv::basic_string_view<CharT>::npos)
            params[form.substr(pos1, pos3 - pos1)] = form.substr(pos3 + 1, form.size() - pos3 - 1);
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
    inline bool Parse(const CharT *s, const bool storeBuf = true, size_t count = static_cast<size_t>(-1))
    {
        return Parse((count == static_cast<size_t>(-1)) ? piv::basic_string_view<CharT>{s} : piv::basic_string_view<CharT>{s, count}, storeBuf);
    }
    inline bool Parse(const std::basic_string<CharT> &s, const bool storeBuf = true)
    {
        return Parse(piv::basic_string_view<CharT>{s}, storeBuf);
    }
    inline bool Parse(const CVolMem &s, const bool storeBuf = true)
    {
        return Parse(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT)}, storeBuf);
    }
    inline bool Parse(const CVolString &s, const bool storeBuf = true)
    {
        if (sizeof(EncodeType) == 2)
            return Parse(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetText())}, storeBuf);
        else if (sizeof(EncodeType) == 4)
            return Parse(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{s}.GetText())}, true);
        else
            return Parse(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{s}.GetText())}, true);
    }

    /**
     * @brief 取指定参数
     * @param name 参数名称
     * @return 返回std::basic_string_view
     */
    inline piv::basic_string_view<CharT> GetParam(const piv::basic_string_view<CharT> &name)
    {
        return params[name];
    }
    inline piv::basic_string_view<CharT> GetParam(const CharT *name, size_t count = static_cast<size_t>(-1))
    {
        return params[(count == static_cast<size_t>(-1)) ? piv::basic_string_view<CharT>{name} : piv::basic_string_view<CharT>{name, count}];
    }
    inline piv::basic_string_view<CharT> GetParam(const std::basic_string<CharT> &name)
    {
        return GetParam(piv::basic_string_view<CharT>{name});
    }
    inline piv::basic_string_view<CharT> GetParam(const CVolMem &name)
    {
        return GetParam(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(name.GetPtr()), static_cast<size_t>(name.GetSize()) / sizeof(CharT)});
    }
    inline piv::basic_string_view<CharT> GetParam(const CVolString &name)
    {
        if (sizeof(EncodeType) == 2)
            return GetParam(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(name.GetText())});
        else if (sizeof(EncodeType) == 4)
            return GetParam(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{name}.GetText())});
        else
            return GetParam(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{name}.GetText())});
    }

    /**
     * @brief 取解码后的参数
     * @tparam T
     * @param name 参数名称
     * @param utf8 是否为UTF-8编码
     * @return
     */
    template <typename T>
    std::basic_string<CharT> GetDecodeParam(T name, const bool utf8 = true)
    {
        return piv::encoding::UrlStrDecode(GetParam(name), true, utf8);
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

#endif // _PIV_URL_HPP
