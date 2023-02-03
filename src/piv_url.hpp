/*********************************************\
 * 火山视窗PIV模块 - URL辅助                 *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 版本: 2023/01/31                          *
\*********************************************/

#ifndef _PIV_URL_HPP
#define _PIV_URL_HPP

#include "piv_string.hpp"
#include <map>

/**
 * @brief URL拆分解析类
 */
template <typename CharT>
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
    PivUrlCrack() {}
    ~PivUrlCrack() {}
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
        pos1 = url.find(':');
        if (pos1 != piv::basic_string_view<CharT>::npos && pos1 + 2 < url.size() && url[pos1 + 1] == '/' && url[pos1 + 2] == '/')
        {
            scheme = url.substr(0, pos1);
            pos2 = pos1 + 3;
        }
        else
        {
            pos2 = 0;
        }
        pos2 = url.find('/', pos2);
        if (pos2 != piv::basic_string_view<CharT>::npos)
        {
            path = url.substr(pos2, url.size() - pos2);
        }
        if (simple == false)
        {
            if (pos2 > 3)
            {
                host = url.substr(pos1 + 3, pos2 - pos1 - 3);
                pos1 = host.find(':');
                pos2 = host.find('@');
                if (pos1 != piv::basic_string_view<CharT>::npos && pos2 != piv::basic_string_view<CharT>::npos)
                {
                    user = host.substr(0, pos1);
                    password = host.substr(pos1 + 1, pos2 - pos1 - 1);
                    host.remove_prefix(pos2 + 1);
                }
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
            if (path.size() == 0)
            {
                return;
            }
            pos1 = path.find('#');
            if (pos1 != piv::basic_string_view<CharT>::npos)
            {
                fragment = path.substr(pos1 + 1, path.size() - pos1 - 1);
                path.remove_suffix(path.size() - pos1);
            }
        }
        pos1 = path.find('?');
        if (pos1 != piv::basic_string_view<CharT>::npos)
        {
            query = path.substr(pos1 + 1, path.size() - pos1 - 1);
            path.remove_suffix(path.size() - pos1);
        }
        if (query.size() < 3)
        {
            return;
        }
        pos2 = query.find('&', 0);
        pos1 = 0;
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
        if (sizeof(CharT))
            Ctreate(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(urlAdress.GetText())}, simple, storeBuf);
        else
            Ctreate(PivW2A(urlAdress.GetText()).String(), simple, true);
    }
    bool HasBuffer()
    {
        return !urlstr.empty();
    }
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
    CVolString GetScheme(const VOL_STRING_ENCODE_TYPE EncodeType)
    {
        if (sizeof(CharT) == 2)
            return CVolString(scheme.data(), scheme.size());
        if (EncodeType == VSET_UTF_8)
            return PivU2W(reinterpret_cast<const char *>(scheme.data()), scheme.size());
        else
            return PivA2W(reinterpret_cast<const char *>(scheme.data()), scheme.size());
    }
    CVolString GetUser(const VOL_STRING_ENCODE_TYPE EncodeType, const bool urlDecode = false, const bool utf8 = true)
    {
        std::basic_string<CharT> str = Decode(user, urlDecode, utf8);
        if (sizeof(CharT) == 2)
            return CVolString(str.c_str());
        if (EncodeType == VSET_UTF_8)
            return PivU2W(reinterpret_cast<const char *>(str.data()), str.size());
        else
            return PivA2W(reinterpret_cast<const char *>(str.data()), str.size());
    }
    std::basic_string<CharT> GetUser(const bool urlDecode = false, const bool utf8 = true)
    {
        return Decode(user, urlDecode, utf8);
    }

    CVolString GetPassword(const VOL_STRING_ENCODE_TYPE EncodeType, const bool urlDecode = false, const bool utf8 = true)
    {
        std::basic_string<CharT> str = Decode(password, urlDecode, utf8);
        if (sizeof(CharT) == 2)
            return CVolString(str.c_str());
        if (EncodeType == VSET_UTF_8)
            return PivU2W(reinterpret_cast<const char *>(str.data()), str.size());
        else
            return PivA2W(reinterpret_cast<const char *>(str.data()), str.size());
    }
    std::basic_string<CharT> GetPassword(const bool urlDecode = false, const bool utf8 = true)
    {
        return Decode(password, urlDecode, utf8);
    }

    CVolString GetHost(const VOL_STRING_ENCODE_TYPE EncodeType)
    {
        if (sizeof(CharT) == 2)
            return CVolString(host.data(), host.size());
        if (EncodeType == VSET_UTF_8)
            return PivU2W(reinterpret_cast<const char *>(host.data()), host.size());
        else
            return PivA2W(reinterpret_cast<const char *>(host.data()), host.size());
    }
    CVolString GetPath(const VOL_STRING_ENCODE_TYPE EncodeType)
    {
        if (sizeof(CharT) == 2)
            return CVolString(path.data(), path.size());
        if (EncodeType == VSET_UTF_8)
            return PivU2W(reinterpret_cast<const char *>(path.data()), path.size());
        else
            return PivA2W(reinterpret_cast<const char *>(path.data()), path.size());
    }
    CVolString GetPort(const VOL_STRING_ENCODE_TYPE EncodeType)
    {
        if (sizeof(CharT) == 2)
            return CVolString(port.data(), port.size());
        if (EncodeType == VSET_UTF_8)
            return PivU2W(reinterpret_cast<const char *>(port.data()), port.size());
        else
            return PivA2W(reinterpret_cast<const char *>(port.data()), port.size());
    }
    CVolString GetQuery(const VOL_STRING_ENCODE_TYPE EncodeType)
    {
        if (sizeof(CharT) == 2)
            return CVolString(query.data(), query.size());
        if (EncodeType == VSET_UTF_8)
            return PivU2W(reinterpret_cast<const char *>(query.data()), query.size());
        else
            return PivA2W(reinterpret_cast<const char *>(query.data()), query.size());
    }
    CVolString GetFragment(const VOL_STRING_ENCODE_TYPE EncodeType)
    {
        if (sizeof(CharT) == 2)
            return CVolString(fragment.data(), fragment.size());
        if (EncodeType == VSET_UTF_8)
            return PivU2W(reinterpret_cast<const char *>(fragment.data()), fragment.size());
        else
            return PivA2W(reinterpret_cast<const char *>(fragment.data()), fragment.size());
    }
    CVolString GetParams(const VOL_STRING_ENCODE_TYPE EncodeType, const piv::basic_string_view<CharT> &name, const bool urlDecode = false, const bool utf8 = true)
    {
        std::basic_string<CharT> str = Decode(params[name], urlDecode, utf8);
        if (sizeof(CharT) == 2)
        {
            return CVolString(str.c_str());
        }
        if (EncodeType == VSET_UTF_8)
            return PivU2W(reinterpret_cast<const char *>(str.data()), str.size());
        else
            return PivA2W(reinterpret_cast<const char *>(str.data()), str.size());
    }
    CVolString GetParams(const VOL_STRING_ENCODE_TYPE EncodeType, const CharT *name, const bool urlDecode = false, const bool utf8 = true)
    {
        return GetParams(EncodeType, piv::basic_string_view<CharT>{name}, urlDecode, utf8);
    }
    CVolString GetParams(const VOL_STRING_ENCODE_TYPE EncodeType, const std::basic_string<CharT> &name, const bool urlDecode = false, const bool utf8 = true)
    {
        return GetParams(EncodeType, piv::basic_string_view<CharT>{name.data(), name.size()}, urlDecode, utf8);
    }
    CVolString GetParams(const VOL_STRING_ENCODE_TYPE EncodeType, const CVolMem &name, const bool urlDecode = false, const bool utf8 = true)
    {
        return GetParams(EncodeType, piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(name.GetPtr()), static_cast<size_t>(name.GetSize()) / sizeof(CharT)}, urlDecode, utf8);
    }
    CVolString GetParams(const VOL_STRING_ENCODE_TYPE EncodeType, const CVolString &name, const bool urlDecode = false, const bool utf8 = true)
    {
        if (sizeof(CharT) == 2)
            return GetParams(EncodeType, piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(name.GetText()), static_cast<size_t>(name.GetLength())}, urlDecode, utf8);
        else
            return GetParams(EncodeType, PivW2A(name.GetText()).String(), urlDecode, utf8);
    }

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
        if (sizeof(CharT) == 2)
            return GetParams2(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(name.GetText()), static_cast<size_t>(name.GetLength())}, urlDecode, utf8);
        else
            return GetParams2(PivW2A(name.GetText()).String(), urlDecode, utf8);
    }

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
                CVolMem buffer;
                if (utf8)
                {
                    PivW2U urlstr(reinterpret_cast<const wchar_t *>(DecodeStr.data()), DecodeStr.size());
                    buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const BYTE *>(urlstr.GetPtr()), urlstr.GetSize());
                    piv::encoding::UrlDecode(reinterpret_cast<const BYTE *>(urlstr.GetPtr()), urlstr.GetSize(), reinterpret_cast<BYTE *>(buffer.Alloc(buffer_len, TRUE)), buffer_len);
                    return std::basic_string<CharT>{reinterpret_cast<const CharT *>(PivU2W{reinterpret_cast<const char *>(buffer.GetPtr()), static_cast<size_t>(buffer.GetSize())}.GetText())};
                }
                else
                {
                    PivW2A urlstr(reinterpret_cast<const wchar_t *>(DecodeStr.data()), DecodeStr.size());
                    buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const BYTE *>(urlstr.GetPtr()), urlstr.GetSize());
                    piv::encoding::UrlDecode(reinterpret_cast<const BYTE *>(urlstr.GetPtr()), urlstr.GetSize(), reinterpret_cast<BYTE *>(buffer.Alloc(buffer_len, TRUE)), buffer_len);
                    return std::basic_string<CharT>{reinterpret_cast<const CharT *>(PivA2W{reinterpret_cast<const char *>(buffer.GetPtr()), static_cast<size_t>(buffer.GetSize())}.GetText())};
                }
            }
            else
            {
                std::basic_string<CharT> buffer;
                buffer_len = piv::encoding::GuessUrlDecodeBound(reinterpret_cast<const BYTE *>(DecodeStr.data()), DecodeStr.size());
                buffer.resize(buffer_len);
                piv::encoding::UrlDecode(reinterpret_cast<const BYTE *>(DecodeStr.data()), DecodeStr.size(), const_cast<BYTE *>(reinterpret_cast<const BYTE *>(buffer.data())), buffer_len);
                return buffer;
            }
        }
        else
        {
            return std::basic_string<CharT>{DecodeStr.data(), DecodeStr.size()};
        }
    }
}; // PivUrlCrack

#endif // _PIV_URL_HPP
