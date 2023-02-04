/*********************************************\
 * 火山视窗PIV模块 - URL辅助                 *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 版本: 2023/02/04                          *
\*********************************************/

#ifndef _PIV_URL_HPP
#define _PIV_URL_HPP

#include "piv_string.hpp"
#include <map>

/**
 * @brief URL拆分解析类
 * @tparam CharT 字符类型
 * @tparam EncodeType 文本编码,按类型的尺寸区分(1=ANSI;2=UTF-16LE;4=UTF8)
 */
template <typename CharT, typename EncodeType = CharT>
class PivUrlCrack
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
    PivUrlCrack() {}
    ~PivUrlCrack() {}

    /**
     * @brief 复制构造函数
     * @param other 所欲复制的对象
     */
    PivUrlCrack(const PivUrlCrack &other)
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
    PivUrlCrack(PivUrlCrack &&other)
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
    PivUrlCrack(const CharT *urlAdress, const bool simple = false, const bool storeBuf = true, const ptrdiff_t count = -1)
    {
        Ctreate(urlAdress, simple, storeBuf, count);
    }
    PivUrlCrack(const piv::basic_string_view<CharT> &urlAdress, const bool simple = false, const bool storeBuf = true)
    {
        Ctreate(urlAdress, simple, storeBuf);
    }
    PivUrlCrack(const std::basic_string<CharT> &urlAdress, const bool simple = false, const bool storeBuf = true)
    {
        Ctreate(urlAdress, simple, storeBuf);
    }
    PivUrlCrack(const CVolMem &urlAdress, const bool simple = false, const bool storeBuf = true)
    {
        Ctreate(urlAdress, simple, storeBuf);
    }
    PivUrlCrack(const CVolString &urlAdress, const bool simple = false, const bool storeBuf = true)
    {
        Ctreate(urlAdress, simple, storeBuf);
    }

    /**
     * @brief 赋值操作符
     * @param other 所欲复制的对象
     * @return 返回自身
     */
    PivUrlCrack &operator=(const PivUrlCrack &other)
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
    PivUrlCrack &operator=(PivUrlCrack &&other)
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
    void Clear()
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
    void Ctreate(const piv::basic_string_view<CharT> &urlAdress, const bool simple = false, const bool storeBuf = true)
    {
        Clear();
        if (urlAdress.empty())
        {
            return;
        }
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
            {
                host = url.substr(pos1, pos2 - pos1);
            }
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
        {
            return;
        }
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
    void Ctreate(const CharT *urlAdress, const bool simple = false, const bool storeBuf = true, const ptrdiff_t count = -1)
    {
        Ctreate((count == -1) ? piv::basic_string_view<CharT>{urlAdress} : piv::basic_string_view<CharT>{urlAdress, static_cast<size_t>(count)}, simple, storeBuf);
    }
    void Ctreate(const std::basic_string<CharT> &urlAdress, const bool simple = false, const bool storeBuf = true)
    {
        Ctreate(piv::basic_string_view<CharT>{urlAdress}, simple, storeBuf);
    }
    void Ctreate(const CVolMem &urlAdress, const bool simple = false, const bool storeBuf = true)
    {
        Ctreate(piv::basic_string_view<CharT>{urlAdress.GetPtr(), static_cast<size_t>(urlAdress.GetSize()) / sizeof(CharT)}, simple, storeBuf);
    }
    void Ctreate(const CVolString &urlAdress, const bool simple = false, const bool storeBuf = true)
    {
        if (sizeof(EncodeType) == 2)
            Ctreate(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(urlAdress.GetText())}, simple, storeBuf);
        else if (sizeof(EncodeType) == 4)
            Ctreate(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U(urlAdress).GetText())}, simple, true);
        else
            Ctreate(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A(urlAdress).GetText())}, simple, true);
    }

    /**
     * @brief 是否有缓存
     * @return 返回真表示有缓存
     */
    bool HasBuffer()
    {
        return !urlstr.empty();
    }

    /**
     * @brief 获取URL各组件的文本视图
     * @return 返回文本视图,不会产生文本复制
     */
    piv::basic_string_view<CharT> &Scheme()
    {
        return scheme;
    }
    piv::basic_string_view<CharT> &User()
    {
        return user;
    }
    piv::basic_string_view<CharT> &Password()
    {
        return password;
    }
    piv::basic_string_view<CharT> &Host()
    {
        return host;
    }
    piv::basic_string_view<CharT> &Port()
    {
        return port;
    }
    piv::basic_string_view<CharT> &Path()
    {
        return path;
    }
    piv::basic_string_view<CharT> &Query()
    {
        return query;
    }
    piv::basic_string_view<CharT> &Fragment()
    {
        return fragment;
    }

    /**
     * @brief 取协议
     * @return 返回文本型
     */
    CVolString GetScheme()
    {
        if (sizeof(EncodeType) == 2)
            return CVolString(scheme.data(), scheme.size());
        else if (sizeof(EncodeType) == 4)
            return PivU2W(reinterpret_cast<const char *>(scheme.data()), scheme.size());
        else
            return PivA2W(reinterpret_cast<const char *>(scheme.data()), scheme.size());
    }

    /**
     * @brief 取账户
     * @param urlDecode 是否URL解码
     * @param utf8 是否为UTF-8编码
     * @return 返回文本
     */
    CVolString GetUser(const bool urlDecode = false, const bool utf8 = true)
    {
        std::basic_string<CharT> str = Decode(user, urlDecode, utf8);
        if (sizeof(EncodeType) == 2)
            return CVolString(str.c_str());
        else if (sizeof(EncodeType) == 4)
            return PivU2W(reinterpret_cast<const char *>(str.data()), str.size());
        else
            return PivA2W(reinterpret_cast<const char *>(str.data()), str.size());
    }
    std::basic_string<CharT> DecodeUser(const bool urlDecode = false, const bool utf8 = true)
    {
        return Decode(user, urlDecode, utf8);
    }

    /**
     * @brief 取密码
     * @param urlDecode 是否URL解码
     * @param utf8 是否为UTF-8编码
     * @return 返回文本
     */
    CVolString GetPassword(const bool urlDecode = false, const bool utf8 = true)
    {
        std::basic_string<CharT> str = Decode(password, urlDecode, utf8);
        if (sizeof(EncodeType) == 2)
            return CVolString(str.c_str());
        else if (sizeof(EncodeType) == 4)
            return PivU2W(reinterpret_cast<const char *>(str.data()), str.size());
        else
            return PivA2W(reinterpret_cast<const char *>(str.data()), str.size());
    }
    std::basic_string<CharT> DecodePassword(const bool urlDecode = false, const bool utf8 = true)
    {
        return Decode(password, urlDecode, utf8);
    }

    /**
     * @brief 取域名
     * @return 返回文本型
     */
    CVolString GetHost()
    {
        if (sizeof(EncodeType) == 2)
            return CVolString(host.data(), host.size());
        else if (sizeof(EncodeType) == 4)
            return PivU2W(reinterpret_cast<const char *>(host.data()), host.size());
        else
            return PivA2W(reinterpret_cast<const char *>(host.data()), host.size());
    }

    /**
     * @brief 取路径
     * @return 返回文本型
     */
    CVolString GetPath()
    {
        if (sizeof(EncodeType) == 2)
            return CVolString(path.data(), path.size());
        else if (sizeof(EncodeType) == 4)
            return PivU2W(reinterpret_cast<const char *>(path.data()), path.size());
        else
            return PivA2W(reinterpret_cast<const char *>(path.data()), path.size());
    }

    /**
     * @brief 取端口
     * @return 返回文本型
     */
    CVolString GetPort()
    {
        if (sizeof(EncodeType) == 2)
            return CVolString(port.data(), port.size());
        else if (sizeof(EncodeType) == 4)
            return PivU2W(reinterpret_cast<const char *>(port.data()), port.size());
        else
            return PivA2W(reinterpret_cast<const char *>(port.data()), port.size());
    }

    /**
     * @brief 取参数
     * @return 返回文本型
     */
    CVolString GetQuery()
    {
        if (sizeof(EncodeType) == 2)
            return CVolString(query.data(), query.size());
        else if (sizeof(EncodeType) == 4)
            return PivU2W(reinterpret_cast<const char *>(query.data()), query.size());
        else
            return PivA2W(reinterpret_cast<const char *>(query.data()), query.size());
    }

    /**
     * @brief 取段落
     * @return 返回文本型
     */
    CVolString GetFragment()
    {
        if (sizeof(EncodeType) == 2)
            return CVolString(fragment.data(), fragment.size());
        else if (sizeof(EncodeType) == 4)
            return PivU2W(reinterpret_cast<const char *>(fragment.data()), fragment.size());
        else
            return PivA2W(reinterpret_cast<const char *>(fragment.data()), fragment.size());
    }

    /**
     * @brief 取指定参数
     * @param name 参数名称
     * @param urlDecode 是否URL解码
     * @param utf8 是否为UTF-8编码
     * @return 返回文本型
     */
    CVolString GetParams(const piv::basic_string_view<CharT> &name, const bool urlDecode = false, const bool utf8 = true)
    {
        std::basic_string<CharT> str = Decode(params[name], urlDecode, utf8);
        if (sizeof(EncodeType) == 2)
            return CVolString(str.c_str());
        else if (sizeof(EncodeType) == 4)
            return PivU2W(reinterpret_cast<const char *>(str.data()), str.size());
        else
            return PivA2W(reinterpret_cast<const char *>(str.data()), str.size());
    }
    CVolString GetParams(const CharT *name, const bool urlDecode = false, const bool utf8 = true)
    {
        return GetParams(piv::basic_string_view<CharT>{name}, urlDecode, utf8);
    }
    CVolString GetParams(const std::basic_string<CharT> &name, const bool urlDecode = false, const bool utf8 = true)
    {
        return GetParams(piv::basic_string_view<CharT>{name.data(), name.size()}, urlDecode, utf8);
    }
    CVolString GetParams(const CVolMem &name, const bool urlDecode = false, const bool utf8 = true)
    {
        return GetParams(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(name.GetPtr()), static_cast<size_t>(name.GetSize()) / sizeof(CharT)}, urlDecode, utf8);
    }
    CVolString GetParams(const CVolString &name, const bool urlDecode = false, const bool utf8 = true)
    {
        if (sizeof(EncodeType) == 2)
            return GetParams(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(name.GetText()), static_cast<size_t>(name.GetLength())}, urlDecode, utf8);
        else if (sizeof(EncodeType) == 4)
            return GetParams(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U(name).GetText())}, urlDecode, utf8);
        else
            return GetParams(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A(name).GetText())}, urlDecode, utf8);
    }

    /**
     * @brief 取指定参数
     * @param name 参数名称
     * @param urlDecode 是否URL解码
     * @param utf8 是否为UTF-8编码
     * @return 返回std::basic_string
     */
    std::basic_string<CharT> GetParams2(const piv::basic_string_view<CharT> &name, const bool urlDecode = false, const bool utf8 = true)
    {
        return Decode(params[name], urlDecode, utf8);
    }
    std::basic_string<CharT> GetParams2(const CharT *name, const bool urlDecode = false, const bool utf8 = true)
    {
        return GetParams2(piv::basic_string_view<CharT>{name}, urlDecode, utf8);
    }
    std::basic_string<CharT> GetParams2(const std::basic_string<CharT> &name, const bool urlDecode = false, const bool utf8 = true)
    {
        return GetParams2(piv::basic_string_view<CharT>{name.data(), name.size()}, urlDecode, utf8);
    }
    std::basic_string<CharT> GetParams2(const CVolMem &name, const bool urlDecode = false, const bool utf8 = true)
    {
        return GetParams2(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(name.GetPtr()), static_cast<size_t>(name.GetSize()) / sizeof(CharT)}, urlDecode, utf8);
    }
    std::basic_string<CharT> GetParams2(const CVolString &name, const bool urlDecode = false, const bool utf8 = true)
    {
        if (sizeof(EncodeType) == 2)
            return GetParams2(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(name.GetText()), static_cast<size_t>(name.GetLength())}, urlDecode, utf8);
        else if (sizeof(EncodeType) == 4)
            return GetParams2(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U(name).GetText())}, urlDecode, true);
        else
            return GetParams2(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A(name).GetText())}, urlDecode, false);
    }

    /**
     * @brief 解码
     * @param DecodeStr 所欲解码的文本
     * @param urlDecode 是否URL解码
     * @param utf8 是否为UTF-8编码
     * @return 返回std::basic_string
     */
    std::basic_string<CharT> Decode(const piv::basic_string_view<CharT> &DecodeStr, const bool urlDecode = false, const bool utf8 = true)
    {
        if (DecodeStr.size() == 0)
        {
            return std::basic_string<CharT>{};
        }
        if (urlDecode)
        {
            ptrdiff_t buffer_len;
            if (sizeof(CharT) == 2)
            {
                if (utf8)
                {
                    PivW2U urlstr(reinterpret_cast<const wchar_t *>(DecodeStr.data()), DecodeStr.size());
                    buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const BYTE *>(urlstr.GetPtr()), urlstr.GetSize());
                    if (buffer_len > 0 && buffer_len < static_cast<ptrdiff_t>(urlstr.GetSize()))
                    {
                        std::string buffer;
                        buffer.resize(buffer_len);
                        piv::encoding::UrlDecode(reinterpret_cast<const BYTE *>(urlstr.GetPtr()), urlstr.GetSize(), const_cast<BYTE *>(reinterpret_cast<const BYTE *>(buffer.data())), buffer_len);
                        return std::basic_string<CharT>{reinterpret_cast<const CharT *>(PivU2W{buffer.c_str()}.GetText())};
                    }
                    else
                    {
                        return std::basic_string<CharT>{reinterpret_cast<const CharT *>(PivU2W{urlstr.GetText()}.GetText())};
                    }
                }
                else
                {
                    PivW2A urlstr(reinterpret_cast<const wchar_t *>(DecodeStr.data()), DecodeStr.size());
                    buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const BYTE *>(urlstr.GetPtr()), urlstr.GetSize());
                    if (buffer_len > 0 && buffer_len < static_cast<ptrdiff_t>(urlstr.GetSize()))
                    {
                        std::string buffer;
                        buffer.resize(buffer_len);
                        piv::encoding::UrlDecode(reinterpret_cast<const BYTE *>(urlstr.GetPtr()), urlstr.GetSize(), const_cast<BYTE *>(reinterpret_cast<const BYTE *>(buffer.data())), buffer_len);
                        return std::basic_string<CharT>{reinterpret_cast<const CharT *>(PivA2W{buffer.c_str()}.GetText())};
                    }
                    else
                    {
                        return std::basic_string<CharT>{reinterpret_cast<const CharT *>(PivA2W{urlstr.GetText()}.GetText())};
                    }
                }
            }
            else
            {
                buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const BYTE *>(DecodeStr.data()), DecodeStr.size());
                if (buffer_len > 0 && buffer_len < static_cast<ptrdiff_t>(DecodeStr.size()))
                {
                    std::basic_string<CharT> buffer;
                    buffer.resize(buffer_len);
                    piv::encoding::UrlDecode(reinterpret_cast<const BYTE *>(DecodeStr.data()), DecodeStr.size(), const_cast<BYTE *>(reinterpret_cast<const BYTE *>(buffer.data())), buffer_len);
                    return buffer;
                }
            }
        }
        return std::basic_string<CharT>{DecodeStr.data(), DecodeStr.size()};
    }
}; // PivUrlCrack

/**
 * @brief 表单参数类
 * @tparam CharT 字符类型
 * @tparam EncodeType 文本编码,按类型的尺寸区分(1=ANSI;2=UTF-16LE;4=UTF8)
 */
template <typename CharT, typename EncodeType = CharT>
class PivFormParams : public CVolObject
{
private:
    CVolMem form;

    /**
     * @brief 添加键名(内部使用)
     * @param key 键名
     * @param count 键名长度
     */
    void AddKey(const CharT *key, const size_t count = -1)
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
    void AddKey(const CVolString &key)
    {
        if (sizeof(EncodeType) == 2)
            AddKey(reinterpret_cast<const CharT *>(key.GetText()));
        else if (sizeof(EncodeType) == 4)
            AddKey(reinterpret_cast<const CharT *>(PivW2U(key).GetText()));
        else
            AddKey(reinterpret_cast<const CharT *>(PivW2A(key).GetText()));
    }
    void AddKey(const CVolMem &value)
    {
        AddKey(reinterpret_cast<const CharT *>(value.GetPtr()), static_cast<size_t>(value.GetSize()) / sizeof(CharT));
    }
    void AddKey(const std::basic_string<CharT> &value)
    {
        AddKey(value.c_str());
    }
    void AddKey(const piv::basic_string_view<CharT> &value)
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
    void AddValue(const CharT *value, const bool url_encode, const bool utf8, const size_t count = -1)
    {
        if (sizeof(EncodeType) == 2)
            form.AddWChar('=');
        else
            form.AddU8Char('=');
        INT_P npSize = static_cast<INT_P>((count == -1) ? ((sizeof(EncodeType) == 2) ? wcslen(reinterpret_cast<const wchar_t *>(value)) : strlen(reinterpret_cast<const char *>(value))) : count);
        if (url_encode)
        {
            ptrdiff_t buffer_len;
            CVolMem buffer;
            if (sizeof(EncodeType) == 2)
            {
                if (utf8)
                {
                    PivW2U text{reinterpret_cast<const wchar_t *>(value), static_cast<size_t>(npSize)};
                    buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const BYTE *>(text.GetPtr()), static_cast<ptrdiff_t>(text.GetSize()));
                    if (buffer_len > 0 && buffer_len > static_cast<ptrdiff_t>(text.GetSize()))
                    {
                        piv::encoding::UrlEncode(reinterpret_cast<const BYTE *>(text.GetPtr()), static_cast<ptrdiff_t>(text.GetSize()), buffer.Alloc(buffer_len, TRUE), buffer_len);
                        PivU2W urled(reinterpret_cast<const char *>(buffer.GetPtr()), buffer_len);
                        form.Append(reinterpret_cast<const void *>(urled.GetText()), urled.GetSize() * 2);
                        return;
                    }
                }
                else
                {
                    PivW2A text{reinterpret_cast<const wchar_t *>(value), static_cast<size_t>(npSize)};
                    buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const BYTE *>(text.GetPtr()), static_cast<ptrdiff_t>(text.GetSize()));
                    if (buffer_len > 0 && buffer_len > static_cast<ptrdiff_t>(text.GetSize()))
                    {
                        piv::encoding::UrlEncode(reinterpret_cast<const BYTE *>(text.GetPtr()), static_cast<ptrdiff_t>(text.GetSize()), buffer.Alloc(buffer_len, TRUE), buffer_len);
                        PivA2W urled(reinterpret_cast<const char *>(buffer.GetPtr()), buffer_len);
                        form.Append(reinterpret_cast<const void *>(urled.GetText()), urled.GetSize() * 2);
                        return;
                    }
                }
            }
            else
            {
                buffer_len = piv::encoding::GuessUrlEncodeBound(reinterpret_cast<const BYTE *>(value), npSize);
                if (buffer_len > 0 && buffer_len > npSize)
                {
                    piv::encoding::UrlEncode(reinterpret_cast<const BYTE *>(value), npSize, buffer.Alloc(buffer_len, TRUE), buffer_len);
                    form.Append(buffer);
                    return;
                }
            }
        }
        // 如果上面都没有返回,说明不需要URL编码
        form.Append(reinterpret_cast<const void *>(value), npSize * sizeof(CharT));
    }
    void AddValue(const CVolString &value, const bool url_encode, const bool utf8)
    {
        if (sizeof(EncodeType) == 2)
            AddValue(reinterpret_cast<const CharT *>(value.GetText()), url_encode, utf8);
        else if (sizeof(EncodeType) == 4)
            AddValue(reinterpret_cast<const CharT *>(PivW2U(value).GetText()), url_encode, utf8);
        else
            AddValue(reinterpret_cast<const CharT *>(PivW2A(value).GetText()), url_encode, utf8);
    }
    void AddValue(const CVolMem &value, const bool url_encode, const bool utf8)
    {
        AddValue(reinterpret_cast<const CharT *>(value.GetPtr()), url_encode, utf8, static_cast<size_t>(value.GetSize()) / sizeof(CharT));
    }
    void AddValue(const std::basic_string<CharT> &value, const bool url_encode, const bool utf8)
    {
        AddValue(value.c_str(), url_encode, utf8);
    }
    void AddValue(const piv::basic_string_view<CharT> &value, const bool url_encode, const bool utf8)
    {
        AddValue(value.data(), url_encode, utf8, value.size());
    }

public:
    /**
     * @brief 默认构造与析构函数
     */
    PivFormParams(){};
    ~PivFormParams(){};

    /**
     * @brief 复制构造函数
     * @param other 所欲复制的对象
     */
    PivFormParams(const PivFormParams &other)
    {
        form = other.form;
    }
    PivFormParams(PivFormParams &&other)
    {
        form = std::move(other.form);
    }

    /**
     * @brief 赋值操作符
     * @param other 所欲复制的文本视图类
     * @return 返回自身
     */
    PivFormParams &operator=(const PivFormParams &other)
    {
        form = other.form;
        return *this;
    }
    PivFormParams &operator=(PivFormParams &&other)
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
     * @brief 清空
     */
    void Clear()
    {
        form.Free();
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
    PivFormParams &AddParam(K key, V value, const bool url_encode, const bool utf8)
    {
        AddKey(key);
        AddValue(value, url_encode, utf8);
        return *this;
    }

    /**
     * @brief 数据
     * @return 返回字节集参考
     */
    CVolMem &GetMem()
    {
        return form;
    }

    /**
     * @brief 到文本视图
     * @return 返回文本视图
     */
    piv::basic_string_view<CharT> ToStrView()
    {
        return piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(form.GetPtr()), static_cast<size_t>(form.GetSize()) / sizeof(CharT)};
    }

    /**
     * @brief 到文本型
     * @return 返回火山文本型
     */
    CVolString ToStr()
    {
        if (sizeof(EncodeType) == 2)
            return CVolString(reinterpret_cast<const wchar_t *>(form.GetPtr()), form.GetSize() / 2);
        else if (sizeof(EncodeType) == 4)
            return PivU2W(reinterpret_cast<const char *>(form.GetPtr()), form.GetSize());
        else
            return PivA2W(reinterpret_cast<const char *>(form.GetPtr()), form.GetSize());
    }

    /**
     * @brief 取文本
     * @param s 用来获取数据的文本变量
     */
    void GetStr(CVolString &s)
    {
        if (sizeof(EncodeType) == 2)
            s.SetText(reinterpret_cast<const wchar_t *>(form.GetPtr()), form.GetSize() / 2);
        else if (sizeof(EncodeType) == 4)
            PivU2W(reinterpret_cast<const char *>(form.GetPtr()), form.GetSize()).GetStr(s);
        else
            PivA2W(reinterpret_cast<const char *>(form.GetPtr()), form.GetSize()).GetStr(s);
    }
}; // PivFormParams

#endif // _PIV_URL_HPP
