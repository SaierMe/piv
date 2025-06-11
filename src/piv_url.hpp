/*********************************************\
 * 火山视窗PIV模块 - URL辅助                 *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef _PIV_URL_HPP
#define _PIV_URL_HPP

#include "piv_string.hpp"
#include <map>

/**
 * @brief URL地址解析类
 * @tparam CharT 字符类型
 */
template <typename CharT>
class PivUrlParser : public CVolObject
{
private:
    std::basic_string<CharT> urlstr;
    std::basic_string<CharT> port_str;
    std::map<piv::basic_string_view<CharT>, piv::basic_string_view<CharT>, piv::ci_less<piv::basic_string_view<CharT>>> params;

public:
    piv::basic_string_view<CharT> url;
    piv::basic_string_view<CharT> scheme;
    piv::basic_string_view<CharT> user;
    piv::basic_string_view<CharT> password;
    piv::basic_string_view<CharT> host;
    piv::basic_string_view<CharT> port;
    piv::basic_string_view<CharT> path;
    piv::basic_string_view<CharT> query;
    piv::basic_string_view<CharT> fragment;

    /**
     * @brief 默认构造与析构函数
     */
    PivUrlParser() {}
    ~PivUrlParser() {}

    /**
     * @brief 复制和移动构造函数
     * @param other 所欲复制的对象
     */
    PivUrlParser(const PivUrlParser& other)
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

    PivUrlParser(PivUrlParser&& other) noexcept
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
    template <typename T>
    PivUrlParser(T&& urlAdress, bool storeBuf = false, bool simple = false, size_t count = static_cast<size_t>(-1))
    {
        Parse(std::forward<T>(urlAdress), storeBuf, simple, count);
    }

    /**
     * @brief 赋值操作符
     * @param other 所欲复制的对象
     * @return 返回自身
     */
    PivUrlParser& operator=(const PivUrlParser& other)
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

    PivUrlParser& operator=(PivUrlParser&& other) noexcept
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
        url = std::move(piv::basic_string_view<CharT>{});
        scheme = std::move(piv::basic_string_view<CharT>{});
        user = std::move(piv::basic_string_view<CharT>{});
        password = std::move(piv::basic_string_view<CharT>{});
        host = std::move(piv::basic_string_view<CharT>{});
        port = std::move(piv::basic_string_view<CharT>{});
        path = std::move(piv::basic_string_view<CharT>{});
        query = std::move(piv::basic_string_view<CharT>{});
        fragment = std::move(piv::basic_string_view<CharT>{});
        params.clear();
    }

    virtual void ResetObject() override
    {
        Clear();
    }

    virtual void GetDumpString(CWString& strDump, INT nMaxDumpSize) override
    {
        strDump.AddFormatText(L"<URL地址解析类> 解析结果:\r\n1. 协议: %s\r\n2. 账户: %s\r\n3. 密码: %s\r\n4. 域名: %s\r\n5. 端口: %s\r\n6. 路径: %s\r\n7. 参数: %s\r\n8. 段落: %s\r\n",
                              GetScheme().GetText(), GetUser().GetText(), GetPassword().GetText(), GetHost().GetText(),
                              GetPort().GetText(), GetPath().GetText(), GetQuery().GetText(), GetFragment().GetText());
    }

    /**
     * @brief 置对象数据
     * @param stream 基本输入流
     */
    virtual void LoadFromStream(CVolBaseInputStream& stream)
    {
        Clear();
        if (stream.eof())
            return;
        uint32_t Size;
        stream.read(&Size, 4);
        urlstr.resize(Size / sizeof(CharT));
        if (stream.ReadExact(const_cast<CharT*>(urlstr.data()), Size))
            Parse(urlstr);
        else
            urlstr.clear();
    }

    /**
     * @brief 取对象数据
     * @param stream 基本输出流
     */
    virtual void SaveIntoStream(CVolBaseOutputStream& stream)
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
    void Parse(const piv::basic_string_view<CharT>& urlAdress, bool storeBuf = false, bool simple = false)
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

    template <typename T>
    inline void Parse(const T& urlAdress, bool storeBuf = false, bool simple = false)
    {
        PIV_IF(sizeof(CharT) == 2)
        {
            PivS2WV _url{urlAdress};
            Parse(_url.sv, _url.own() ? true : storeBuf, simple);
        }
        else
        {
            PivS2UV _url{urlAdress};
            Parse(_url.sv, _url.own() ? true : storeBuf, simple);
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
     * @brief 取协议
     * @return 返回文本型
     */
    inline CWString GetScheme() const
    {
        return PivAny2Ws<CWString, true>{scheme}.str;
    }

    /**
     * @brief 取账户
     * @param utf8 是否为UTF-8编码
     * @return 返回文本
     */
    inline CWString GetUser(bool utf8 = true) const
    {
        CWString ret;
        return piv::encoding::UrlStrDecode(user, utf8, ret);
    }

    inline std::basic_string<CharT> DecodeUser(bool utf8 = true) const
    {
        std::basic_string<CharT> ret;
        return piv::encoding::UrlStrDecode(user, utf8, ret);
    }

    /**
     * @brief 取密码
     * @param urlDecode 是否URL解码
     * @param utf8 是否为UTF-8编码
     * @return 返回文本
     */
    inline CWString GetPassword(bool utf8 = true) const
    {
        CWString ret;
        return piv::encoding::UrlStrDecode(password, utf8, ret);
    }

    inline std::basic_string<CharT> DecodePassword(bool utf8 = true) const
    {
        std::basic_string<CharT> ret;
        return piv::encoding::UrlStrDecode(password, utf8, ret);
    }

    /**
     * @brief 取域名
     * @return 返回文本型
     */
    inline CWString GetHost() const
    {
        return PivAny2Ws<CWString, true>{host}.str;
    }

    /**
     * @brief 取路径
     * @return 返回文本型
     */
    inline CWString GetPath() const
    {
        return PivAny2Ws<CWString, true>{path}.str;
    }

    /**
     * @brief 取端口
     * @return 返回文本型
     */
    inline CWString GetPort() const
    {
        return PivAny2Ws<CWString, true>{port}.str;
    }

    /**
     * @brief 取参数
     * @return 返回文本型
     */
    inline CWString GetQuery() const
    {
        return PivAny2Ws<CWString, true>{query}.str;
    }

    /**
     * @brief 取段落
     * @return 返回文本型
     */
    inline CWString GetFragment() const
    {
        return PivAny2Ws<CWString, true>{fragment}.str;
    }

    /**
     * @brief 取参数视图
     * @param name 参数名称
     * @param utf8 是否为UTF-8编码
     * @return 返回std::basic_string
     */
    inline piv::basic_string_view<CharT> GetParam(const piv::basic_string_view<CharT>& name, bool utf8 = true)
    {
        if (params.find(name) != params.end())
        {
            return params[name];
        }
        else if (piv::encoding::UrlEncodeNeed(name.data(), name.size()))
        {
            return params[piv::basic_string_view<CharT>{piv::encoding::UrlStrEncode(name, utf8, false, std::basic_string<CharT>{})}];
        }
        return piv::basic_string_view<CharT>{};
    }

    template <typename T>
    inline piv::basic_string_view<CharT> GetParam(const T& name, bool utf8 = true)
    {
        PIV_IF(sizeof(CharT) == 2)
        return GetParam(PivS2WV{name}.sv, utf8);
        else return GetParam(PivS2UV{name}.sv, utf8);
    }

    /**
     * @brief 取参数文本
     * @param name 参数名称
     * @param utf8 是否为UTF-8编码
     * @return 返回文本型
     */
    template <typename T>
    inline CWString& GetParamVolStr(T&& name, bool utf8, CWString& param)
    {
        return piv::encoding::UrlStrDecode(GetParam(name, utf8), utf8, param);
    }

    template <typename T>
    inline CWString&& GetParamVolStr(T&& name, bool utf8, CWString&& param = CWString{})
    {
        return std::forward<CWString&&>(piv::encoding::UrlStrDecode(GetParam(name, utf8), utf8, param));
    }

    /**
     * @brief 取指定参数
     * @param name 参数名称
     * @param utf8 是否为UTF-8编码
     * @return 返回std::basic_string
     */
    template <typename T>
    inline std::basic_string<CharT> GetDecodeParam(T&& name, bool utf8 = true)
    {
        return piv::encoding::UrlStrDecode(GetParam(name, utf8), utf8, std::basic_string<CharT>{});
    }

}; // PivUrlParser

/**
 * @brief 表单构造类
 * @tparam CharT 字符类型
 */
class PivFormCreater : public CVolObject
{
private:
    /**
     * @brief 添加键名(内部使用)
     * @param key 键名
     * @param count 键名长度
     * @param utf8 是否基于UTF-8进行URL编码
     */
    inline void AddKey(const char* key, bool utf8 = true, size_t count = -1)
    {
        if (count == -1)
            count = strlen(key);
        if (count == 0)
            return;
        if (!form.IsEmpty())
            form.AddU8Char('&');
        if (piv::encoding::UrlEncodeNeed(key, count, false))
        {
            CVolMem buffer;
            piv::encoding::UrlDataEncode(key, utf8, false, buffer, count);
            form.Append(buffer);
        }
        else
        {
            form.Append(key, static_cast<INT_P>(count));
        }
        form.AddU8Char('=');
    }

    inline void AddKey(const wchar_t* key, bool utf8 = true, size_t count = -1)
    {
        if (utf8)
        {
            PivW2U str{key, count};
            AddKey(str.c_str(), true, str.size());
        }
        else
        {
            PivW2A str{key, count};
            AddKey(str.c_str(), false, str.size());
        }
    }

    template <typename CharT>
    inline void AddKey(const piv::basic_string_view<CharT>& key, bool utf8 = true)
    {
        AddKey(key.data(), utf8, key.size());
    }

    inline void AddKey(const CWString& key, bool utf8 = true)
    {
        AddKey(key.GetText(), utf8, static_cast<size_t>(key.GetLength()));
    }

    inline void AddKey(const CVolMem& key, bool utf8 = true)
    {
        AddKey(reinterpret_cast<const char*>(key.GetPtr()), utf8, static_cast<size_t>(key.GetSize()));
    }

    template <typename CharT>
    inline void AddKey(const std::basic_string<CharT>& key, bool utf8 = true)
    {
        AddKey(key.c_str(), utf8, key.size());
    }

    /**
     * @brief 添加参数值(内部使用)
     * @param value 参数值
     * @param utf8 是否基于UTF-8进行URL编码
     * @param count 参数值长度
     */
    void AddValue(const char* value, bool utf8 = true, size_t count = -1)
    {
        if (count == -1)
            count = strlen(value);
        if (piv::encoding::UrlEncodeNeed(value, count))
        {
            CVolMem buffer;
            piv::encoding::UrlDataEncode(value, utf8, false, buffer, count);
            form.Append(buffer);
            return;
        }
        // 如果上面都没有返回,说明不需要URL编码
        form.Append(reinterpret_cast<const void*>(value), static_cast<INT_P>(count));
    }

    void AddValue(const wchar_t* value, bool utf8 = true, size_t count = -1)
    {
        if (utf8)
        {
            PivW2U str{value, count};
            AddValue(str.c_str(), true, str.size());
        }
        else
        {
            PivW2A str{value, count};
            AddValue(str.c_str(), false, str.size());
        }
    }

    inline void AddValue(const CWString& value, bool utf8 = true)
    {
        AddValue(value.GetText(), utf8, value.GetLength());
    }

    inline void AddValue(const CVolMem& value, bool utf8 = true)
    {
        AddValue(reinterpret_cast<const char*>(value.GetPtr()), utf8, static_cast<size_t>(value.GetSize()));
    }

    template <typename CharT>
    inline void AddValue(const std::basic_string<CharT>& value, bool utf8 = true)
    {
        AddValue(value.c_str(), utf8, value.size());
    }

    template <typename CharT>
    inline void AddValue(const piv::basic_string_view<CharT>& value, bool utf8 = true)
    {
        AddValue(value.data(), utf8, value.size());
    }

public:
    CVolMem form;
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
    PivFormCreater(const PivFormCreater& other)
    {
        form = other.form;
    }

    PivFormCreater(PivFormCreater&& other) noexcept
    {
        form = std::move(other.form);
    }

    /**
     * @brief 赋值操作符
     * @param other 所欲复制的文本视图类
     * @return 返回自身
     */
    PivFormCreater& operator=(const PivFormCreater& other)
    {
        form = other.form;
        return *this;
    }

    PivFormCreater& operator=(PivFormCreater&& other) noexcept
    {
        form = std::move(other.form);
        return *this;
    }

    virtual void ResetObject() override
    {
        Clear();
    }

    /**
     * @brief 取展示内容(调试输出)
     * @param strDump 展示内容
     * @param nMaxDumpSize 最大展示数据尺寸
     */
    virtual void GetDumpString(CWString& strDump, INT nMaxDumpSize) override
    {
        this->GetStr(strDump);
    }

    /**
     * @brief 置对象数据
     * @param stream 基本输入流
     */
    virtual void LoadFromStream(CVolBaseInputStream& stream)
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
    virtual void SaveIntoStream(CVolBaseOutputStream& stream)
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
    PivFormCreater& AddParam(K&& key, V&& value, bool utf8)
    {
        AddKey(std::forward<K>(key), utf8);
        AddValue(std::forward<V>(value), utf8);
        return *this;
    }

    /**
     * @brief 数据
     * @return 返回字节集参考
     */
    inline CVolMem& GetMem()
    {
        return form;
    }

    /**
     * @brief 到文本视图
     * @return 返回文本视图
     */
    inline piv::string_view ToStrView() const
    {
        return piv::string_view{reinterpret_cast<const char*>(form.GetPtr()), static_cast<size_t>(form.GetSize())};
    }

    /**
     * @brief 到文本型
     * @return 返回火山文本型
     */
    inline CWString ToStr() const
    {
        return CWString{reinterpret_cast<const char*>(form.GetPtr()), form.GetSize()};
    }

    /**
     * @brief 取文本
     * @param s 用来获取数据的文本变量
     */
    inline void GetStr(CWString& s) const
    {
        s.SetText(reinterpret_cast<const char*>(form.GetPtr()), form.GetSize());
    }
}; // PivFormCreater

/**
 * @brief 表单解析类
 * @tparam CharT 字符类型
 */
template <typename CharT>
class PivFormParser : public CVolObject
{
private:
    std::basic_string<CharT> form_str;
    piv::basic_string_view<CharT> form;
    std::map<piv::basic_string_view<CharT>, piv::basic_string_view<CharT>, piv::ci_less<piv::basic_string_view<CharT>>> params;

public:
    /**
     * @brief 默认构造与析构函数
     */
    PivFormParser() {}
    ~PivFormParser() {}

    /**
     * @brief 复制和移动构造函数
     * @param other 所欲复制的对象
     */
    PivFormParser(const PivFormParser& other)
    {
        form = other.form;
        form_str = other.form_str;
        params = other.params;
    }

    PivFormParser(PivFormParser&& other) noexcept
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
    template <typename T>
    PivFormParser(const T& s, bool storeBuf = false)
    {
        Parse(s, storeBuf);
    }

    /**
     * @brief 赋值操作符
     * @param other 所欲复制的对象
     * @return 返回自身
     */
    PivFormParser& operator=(const PivFormParser& other)
    {
        form = other.form;
        form_str = other.form_str;
        params = other.params;
        return *this;
    }

    PivFormParser& operator=(PivFormParser&& other) noexcept
    {
        form = std::move(other.form);
        form_str = std::move(other.form_str);
        params = std::move(other.params);
        return *this;
    }

    virtual void ResetObject() override
    {
        Clear();
    }

    /**
     * @brief 取展示内容(调试输出)
     * @param strDump 展示内容
     * @param nMaxDumpSize 最大展示数据尺寸
     */
    virtual void GetDumpString(CWString& strDump, INT nMaxDumpSize) override
    {
        strDump.AddFormatLine(L"<表单解析类> 总共 %u 个键值对:", params.size());
        int32_t i = 0;
        for (auto it = params.cbegin(); it != params.cend(); it++, i++)
        {
            strDump.AddFormatLine(L"%d. %s = %s", i, PivAny2W<false>{it->first}.c_str(), PivAny2W<false>{it->second}.c_str());
        }
    }

    /**
     * @brief 置对象数据
     * @param stream 基本输入流
     */
    virtual void LoadFromStream(CVolBaseInputStream& stream)
    {
        Clear();
        if (stream.eof())
            return;
        uint32_t Size;
        stream.read(&Size, 4);
        form_str.resize(Size / sizeof(CharT));
        if (stream.ReadExact(const_cast<CharT*>(form_str.data()), Size))
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
    virtual void SaveIntoStream(CVolBaseOutputStream& stream)
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
        form = std::move(piv::basic_string_view<CharT>{});
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
    bool Parse(const piv::basic_string_view<CharT>& s, bool storeBuf = false)
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
            form = std::move(piv::basic_string_view<CharT>{});
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
            form = std::move(piv::basic_string_view<CharT>{});
            return false;
        }
        return true;
    }

    inline bool Parse(const CVolMem& s, bool storeBuf = false)
    {
        PIV_IF(sizeof(CharT) == 2)
        {
            PivS2WV text{reinterpret_cast<const char*>(s.GetPtr()), static_cast<size_t>(s.GetSize())};
            return Parse(text.sv, text.own() ? true : storeBuf);
        }
        else
        {
            PivS2UV text{reinterpret_cast<const char*>(s.GetPtr()), static_cast<size_t>(s.GetSize())};
            return Parse(text.sv, text.own() ? true : storeBuf);
        }
    }

    template <typename T>
    inline bool Parse(const T& s, bool storeBuf = false)
    {
        PIV_IF(sizeof(CharT) == 2)
        {
            PivS2WV text{s};
            return Parse(text.sv, text.own() ? true : storeBuf);
        }
        else
        {
            PivS2UV text{s};
            return Parse(text.sv, text.own() ? true : storeBuf);
        }
    }

    /**
     * @brief 取指定参数
     * @param name 参数名称
     * @return 返回std::basic_string_view
     */
    inline piv::basic_string_view<CharT> GetParam(const piv::basic_string_view<CharT>& name, bool utf8 = true)
    {
        if (params.find(name) != params.end())
        {
            return params[name];
        }
        else if (piv::encoding::UrlEncodeNeed(name.data(), name.size()))
        {
            return params[piv::basic_string_view<CharT>{piv::encoding::UrlStrEncode(name, utf8, false, std::basic_string<CharT>{})}];
        }
        return piv::basic_string_view<CharT>{};
    }

    template <typename T>
    inline piv::basic_string_view<CharT> GetParam(const T& name, bool utf8 = true)
    {
        PIV_IF(sizeof(CharT) == 2)
        {
            return GetParam(PivS2WV{name}.sv, utf8);
        }
        else
        {
            return GetParam(PivS2UV{name}.sv, utf8);
        }
    }

    /**
     * @brief 取解码后的参数
     * @param name 参数名称
     * @param utf8 是否为UTF-8编码
     * @return
     */
    template <typename T>
    std::basic_string<CharT> GetDecodeParam(T&& name, bool utf8 = true)
    {
        return piv::encoding::UrlStrDecode(GetParam(name, utf8), utf8, std::basic_string<CharT>{});
    }

    /**
     * @brief 取解码后的参数
     * @param name 参数名称
     * @param utf8 是否为UTF-8编码
     * @param value 参数返回到此文本型中
     * @return
     */
    template <typename T>
    CWString& GetParamStr(T&& name, bool utf8, CWString& value)
    {
        return piv::encoding::UrlStrDecode(GetParam(name, utf8), utf8, value);
    }

    template <typename T>
    CWString&& GetParamStr(T&& name, bool utf8 = true, CWString&& value = CWString{})
    {
        return std::forward<CWString&&>(piv::encoding::UrlStrDecode(GetParam(name, utf8), utf8, value));
    }

    /**
     * @brief 取所有键名
     * @param array 存放键名数组
     * @return
     */
    size_t EnumKey(std::vector<piv::basic_string_view<CharT>>& array) const
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
class PivUrlCreater : public CVolObject
{
private:
    CWString url;
    ptrdiff_t host_len;
    ptrdiff_t path_len;

    /**
     * @brief 添加键名(内部使用)
     * @param key 所欲添加的键名
     * @param len 键名的字符长度
     * @param utf8 URL编码时是否为UTF-8
     */
    inline void AddKey(const wchar_t* key, bool utf8 = true, size_t count = -1)
    {
        if (url.GetLength() == host_len + path_len)
            url.AddChar('?');
        else
            url.AddChar('&');
        if (count == -1)
            count = wcslen(key);
        if (count == 0)
            return;
        if (piv::encoding::UrlEncodeNeed(key, count, false))
        {
            CWString urltext;
            piv::encoding::UrlStrEncode(key, utf8, false, urltext, count);
            url.AddText(urltext.GetText());
        }
        else
        {
            url.AddText(key, count);
        }
        url.AddChar('=');
    }

    template <typename T>
    inline void AddKey(T&& key, bool utf8 = true)
    {
        PivAny2W<true> str{key};
        AddKey(str.c_str(), utf8, str.size());
    }

    /**
     * @brief 添加值(内部使用)
     * @param key 所欲添加的值
     * @param value 值的字符长度
     * @param utf8 URL编码时是否为UTF-8
     */
    inline void AddValue(const wchar_t* value, bool utf8 = true, size_t count = -1)
    {
        if (count == -1)
            count = wcslen(value);
        if (piv::encoding::UrlEncodeNeed(value, count, false))
        {
            CWString urltext;
            piv::encoding::UrlStrEncode(value, utf8, false, urltext, count);
            url.AddText(urltext.GetText());
        }
        else
        {
            url.AddText(value, count);
        }
    }

    template <typename T>
    inline void AddValue(T&& value, bool utf8 = true)
    {
        PivAny2W<true> str{value};
        AddValue(str.c_str(), utf8, str.size());
    }

public:
    /**
     * @brief 默认构造与析构函数
     */
    PivUrlCreater()
    {
        url.SetNumAlignChars(1024);
        host_len = 0;
        path_len = 0;
    }

    ~PivUrlCreater() {}

    /**
     * @brief 复制和移动构造函数
     * @param other 所欲复制的对象
     */
    PivUrlCreater(const PivUrlCreater& other)
    {
        url = other.url;
        host_len = other.host_len;
        path_len = other.path_len;
    }

    PivUrlCreater(PivUrlCreater&& other) noexcept
    {
        url = std::move(other.url);
        host_len = other.host_len;
        path_len = other.path_len;
    }

    PivUrlCreater& operator=(const PivUrlCreater& other)
    {
        url = other.url;
        host_len = other.host_len;
        path_len = other.path_len;
        return *this;
    }
    PivUrlCreater& operator=(PivUrlCreater&& other) noexcept
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
    inline CWString& data() noexcept
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
    inline PivUrlCreater& SetHost(const wchar_t* host, ptrdiff_t len = -1)
    {
        if (len == -1)
            len = static_cast<ptrdiff_t>(wcslen(host));
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

    template <typename T>
    inline PivUrlCreater& SetHost(T&& path)
    {
        PivAny2W<true> text{path};
        return SetHost(text.c_str(), text.size());
    }

    /**
     * @brief 置路径
     * @param path 所欲设置的路径
     * @param len 路径的字符长度
     * @return 返回自身
     */
    inline PivUrlCreater& SetPath(const wchar_t* path, ptrdiff_t len)
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
                std::wstring buffer;
                piv::encoding::UrlStrEncode(path, true, true, buffer, static_cast<size_t>(len));
                path_len += buffer.size();
                url.InsertText(host_len, buffer.c_str(), path_len);
            }
            else
            {
                url.InsertText(host_len, path, len);
                path_len += len;
            }
        }
        return *this;
    }

    template <typename T>
    inline PivUrlCreater& SetPath(T&& path)
    {
        PivAny2W<true> text{path};
        return SetPath(text.c_str(), text.size());
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
    PivUrlCreater& AddParam(K&& key, V&& value, bool utf8)
    {
        AddKey(std::forward<K>(key), utf8);
        AddValue(std::forward<V>(value), utf8);
        return *this;
    }

    virtual void ResetObject() override
    {
        Clear();
    }

    virtual void GetDumpString(CWString& strDump, INT nMaxDumpSize) override
    {
        strDump = url;
    }

    /**
     * @brief 置对象数据
     * @param stream 基本输入流
     */
    virtual void LoadFromStream(CVolBaseInputStream& stream)
    {
        Clear();
        if (stream.eof())
            return;
        uint32_t Size;
        stream.read(&Size, 4);
        url.SetLength((Size / 2) - 1);
        if (stream.ReadExact(const_cast<wchar_t*>(url.GetText()), Size))
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
    virtual void SaveIntoStream(CVolBaseOutputStream& stream)
    {
        uint32_t Size = static_cast<uint32_t>((url.GetLength() + 1) * 2);
        stream.write(&Size, 4);
        stream.write(url.GetText(), Size);
        stream.write(&host_len, 4);
        stream.write(&path_len, 4);
    }

}; // PivUrlCreater

#endif // _PIV_URL_HPP
