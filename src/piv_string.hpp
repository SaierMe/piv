/*********************************************\
 * 火山视窗PIV模块 - 文本辅助                *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 版本: 2023/02/02                          *
\*********************************************/

#ifndef _PIV_STRING_HPP
#define _PIV_STRING_HPP

#include "piv_encoding.hpp"
#include <algorithm>
#include <vector>
#include <memory>

namespace piv
{
    /**
     * @brief 寻找文本(不区分大小写)
     * @tparam CharT 字符类型
     * @param suc 被寻找的文本
     * @param des 所欲寻找的文本
     * @param pos 起始搜寻索引位置
     * @return 找到的位置
     */
    template <typename CharT>
    size_t ifind(const basic_string_view<CharT> &suc, const basic_string_view<CharT> &des, const size_t pos = 0) noexcept
    {
        if (pos >= suc.size() || des.empty())
            return std::basic_string<CharT>::npos;
        for (auto OuterIt = std::next(suc.begin(), pos); OuterIt != suc.end(); ++OuterIt)
        {
            auto InnerIt = OuterIt;
            auto SubstrIt = des.begin();
            for (; InnerIt != suc.end() && SubstrIt != des.end(); ++InnerIt, ++SubstrIt)
            {
                if (std::tolower(static_cast<unsigned char>(*InnerIt)) != std::tolower(static_cast<unsigned char>(*SubstrIt)))
                    break;
            }
            if (SubstrIt == des.end())
                return std::distance(suc.begin(), OuterIt);
        }
        return std::basic_string<CharT>::npos;
    }

    /**
     * @brief 倒找文本(不区分大小写)
     * @tparam CharT 字符类型
     * @param suc 被寻找的文本
     * @param des 所欲寻找的文本
     * @param pos 起始搜寻索引位置
     * @return 找到的位置
     */
    template <typename CharT>
    size_t irfind(const basic_string_view<CharT> &suc, const basic_string_view<CharT> &des, const size_t pos = (size_t)-1) noexcept
    {
        if (pos == (size_t)-1)
            pos = suc.size();
        if (des.empty())
            return std::basic_string<CharT>::npos;
        for (auto OuterIt = std::prev(suc.end(), suc.size() - pos); OuterIt != suc.begin(); --OuterIt)
        {
            auto InnerIt = OuterIt;
            auto SubstrIt = des.begin();
            for (; InnerIt != suc.end() && SubstrIt != des.end(); ++InnerIt, ++SubstrIt)
            {
                if (std::tolower(static_cast<unsigned char>(*InnerIt)) != std::tolower(static_cast<unsigned char>(*SubstrIt)))
                    break;
            }
            if (SubstrIt == des.end())
                return std::distance(suc.begin(), OuterIt);
        }
        return std::basic_string<CharT>::npos;
    }

    /**
     * @brief 删首空
     * @param 所欲操作的文本 str
     */
    template <typename CharT>
    void trim_left(std::basic_string<CharT> &str) noexcept
    {
        if (str.empty())
            return;
        size_t pos = 0;
        for (; pos < str.size(); ++pos)
        {
            // if (!std::isspace(static_cast<unsigned char>(str[pos])))
            if (static_cast<unsigned char>(str[pos]) > ' ')
                break;
        }
        str.erase(0, pos);
    }
    template <typename CharT>
    void trim_left(basic_string_view<CharT> &str) noexcept
    {
        if (str.empty())
            return;
        size_t pos = 0;
        for (; pos < str.size(); ++pos)
        {
            // if (!std::isspace(static_cast<unsigned char>(str[pos])))
            if (static_cast<unsigned char>(str[pos]) > ' ')
                break;
        }
        str.remove_prefix(pos);
    }

    /**
     * @brief 删尾空
     * @param 所欲操作的文本 str
     */
    template <typename CharT>
    void trim_right(std::basic_string<CharT> &str) noexcept
    {
        if (str.empty())
            return;
        size_t pos = str.size() - 1;
        for (; pos != size_t(-1); pos--)
        {
            // if (!std::isspace(static_cast<unsigned char>(str[pos])))
            if (static_cast<unsigned char>(str[pos]) > ' ')
                break;
        }
        str.erase(pos + 1);
    }
    template <typename CharT>
    void trim_right(basic_string_view<CharT> &str) noexcept
    {
        if (str.empty())
            return;
        size_t pos = str.size() - 1;
        for (; pos != size_t(-1); pos--)
        {
            // if (!std::isspace(static_cast<unsigned char>(str[pos])))
            if (static_cast<unsigned char>(str[pos]) > ' ')
                break;
        }
        str.remove_suffix(str.size() - pos - 1);
    }

    template <typename CharT>
    inline CharT ascii_tolower(CharT c) noexcept
    {
        return CharT(((static_cast<unsigned>(c) - 65U) < 26) ? c + 'a' - 'A' : c);
    }

    /**
     * @brief 不区分大小写比较文本
     * @param lhs 所欲比较的文本1
     * @param rhs 所欲比较的文本2
     * @return 返回两个文本是否相同
     */
    template <typename CharT>
    bool iequals(const basic_string_view<CharT> lhs, const basic_string_view<CharT> rhs) noexcept
    {
        size_t n = lhs.size();
        if (rhs.size() != n)
            return false;
        auto p1 = lhs.data();
        auto p2 = rhs.data();
        CharT a, b;
        // fast loop
        while (n--)
        {
            a = *p1++;
            b = *p2++;
            if (a != b)
                goto slow;
        }
        return true;
    slow:
        do
        {
            if (ascii_tolower<CharT>(a) != ascii_tolower<CharT>(b))
                return false;
            a = *p1++;
            b = *p2++;
        } while (n--);
        return true;
    }
    template <typename CharT>
    bool iequals(const std::basic_string<CharT> lhs, const std::basic_string<CharT> rhs) noexcept
    {
        return iequals(basic_string_view<CharT>{lhs}, basic_string_view<CharT>{rhs});
    }

    template <typename = void>
    std::string formatv(const char *format, va_list args)
    {
        std::string s;
        if (format && *format)
        {
            va_list args_copy;
            va_copy(args_copy, args);
            int len = std::vsnprintf(nullptr, 0, format, args_copy);
            if (len > 0)
            {
                s.resize(len);
                va_copy(args_copy, args);
                std::vsprintf((char *)s.data(), format, args_copy);
            }
        }
        return s;
    }
    template <typename = void>
    std::wstring formatv(const wchar_t *format, va_list args)
    {
        std::wstring s;
        if (format && *format)
        {
            va_list args_copy;
            while (true)
            {
                s.resize(s.capacity());
                va_copy(args_copy, args);
                int len = std::vswprintf((wchar_t *)(&s[0]), s.size(), format, args_copy);
                if (len == -1)
                    s.reserve(s.capacity() * 2);
                else
                {
                    s.resize(len);
                    break;
                }
            }
        }
        return s;
    }

    /**
     * std::string format
     */
    template <typename = void>
    std::string format(const char *format, ...)
    {
        std::string s;
        if (format && *format)
        {
            // under windows and linux system,std::vsnprintf(nullptr, 0, format, args)
            // can get the need buffer len for the output,
            va_list args;
            va_start(args, format);
            s = formatv(format, args);
            va_end(args);
        }
        return s;
    }

    /**
     * std::wstring format
     */
    template <typename = void>
    std::wstring format(const wchar_t *format, ...)
    {
        std::wstring s;
        if (format && *format)
        {
            va_list args;
            va_start(args, format);
            s = formatv(format, args);
            va_end(args);
        }
        return s;
    }

} // namespace piv

/*********************************************
 * @brief  标准文本模板类
 * @tparam CharT 字符类型
 */
template <typename CharT>
class PivString : public CVolObject
{
private:
    std::basic_string<CharT> str;

public:
    /**
     * @brief 默认构造函数
     */
    PivString() {}

    /**
     * @brief 默认析构函数
     */
    ~PivString() {}

    /**
     * @brief 复制构造函数
     * @param s 所欲复制的文本
     * @param count 所欲复制的字符长度
     */
    PivString(const PivString &s)
    {
        str = s.str;
    }
    PivString(PivString &&s)
    {
        str = std::move(s.str);
    }
    PivString(const std::basic_string<CharT> &s)
    {
        str = s;
    }
    PivString(std::basic_string<CharT> &&s)
    {
        str = std::move(s);
    }
    PivString(const piv::basic_string_view<CharT> &s)
    {
        str.assign(s.data(), s.size());
    }
    PivString(const CharT *s, ptrdiff_t count = -1)
    {
        if (count == -1)
            str.assign(s);
        else
            str.assign(s, static_cast<size_t>(count));
    }
    PivString(const CVolString &s)
    {
        str.assign(s.GetText());
    }
    PivString(const CVolMem &s)
    {
        str.assign(reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT));
    }

    operator std::basic_string<CharT> &()
    {
        return str;
    }
    operator const std::basic_string<CharT> &()
    {
        return str;
    }
    operator const CharT *()
    {
        return str.c_str();
    }

    /**
     * @brief 赋值操作符
     * @param other 所欲复制的标准文本类
     * @return 返回自身
     */
    PivString &operator=(const PivString &other)
    {
        str = other.str;
        return (*this);
    }
    PivString &operator=(PivString &&other)
    {
        str = std::move(other.str);
        return (*this);
    }
    PivString &operator=(const CharT &s)
    {
        str = s;
        return (*this);
    }
    PivString &operator=(const std::basic_string<CharT> &s)
    {
        str = s;
        return (*this);
    }
    PivString &operator=(std::basic_string<CharT> &&s)
    {
        str = std::move(s);
        return (*this);
    }

    /**
     * @brief 比较操作符
     * @param other 所欲比较的标准文本类
     * @return 返回两个文本的内容是否相同
     */
    bool operator==(const PivString &other)
    {
        return str == other.str;
    }
    bool operator==(const std::basic_string<CharT> &other)
    {
        return str == other;
    }

    /**
     * @brief 获取文本类中的string
     * @return 返回string
     */
    std::basic_string<CharT> &data()
    {
        return str;
    }

    /**
     * @brief 获取文本类中的string指针
     */
    std::basic_string<CharT> *pdata()
    {
        return &str;
    }

    /**
     * @brief 取展示内容(调试输出)
     * @param strDump 展示内容
     * @param nMaxDumpSize 最大展示数据尺寸
     */
    virtual void GetDumpString(CVolString &strDump, INT nMaxDumpSize) override
    {
        if (sizeof(CharT) == 2)
        {
            strDump.SetText(str.c_str());
        }
        else
        {
            strDump.SetText(L"标准文本类");
            CVolMem(reinterpret_cast<const void *>(str.data()), str.size()).GetDumpString(strDump, nMaxDumpSize);
        }
    }

    /**
     * @brief 取文本指针
     * @return 返回文本指针
     */
    const CharT *GetText()
    {
        return str.c_str();
    }

    /**
     * @brief 取文本长度
     */
    size_t GetLength()
    {
        return str.length();
    }

    /**
     * @brief 取文本长度
     */
    size_t GetSize()
    {
        return str.size();
    }

    /**
     * @brief 取文本哈希值
     * @param case_insensitive 是否不区分大小写
     * @return
     */
    size_t GetHash(bool case_insensitive = false)
    {
        if (!case_insensitive)
        {
            return std::hash<std::basic_string<CharT>>{}(str);
        }
        else
        {
            std::basic_string<CharT> upper;
            upper.resize(str.size());
            std::transform(str.begin(), str.end(), upper.begin(), std::toupper);
            return std::hash<std::basic_string<CharT>>{}(upper);
        }
    }

    /**
     * @brief 是否为空
     * @return 返回文本是否为空
     */
    bool IsEmpty()
    {
        return str.empty();
    }

    /**
     * @brief 是否相同
     * @param other 所欲比较的标准文本类
     * @return 返回两个文本类的内容是否相同
     */
    bool IsEqual(const PivString &other)
    {
        return str == other.str;
    }

    /**
     * @brief 文本比较
     * @param s 所欲比较的文本
     * @return
     */
    int32_t Compare(const CharT *s)
    {
        return str.compare(s);
    }
    int32_t Compare(const std::basic_string<CharT> &s)
    {
        return str.compare(s);
    }
    int32_t Compare(const piv::basic_string_view<CharT> &s)
    {
        return str.compare(0, str.size(), s.data());
    }
    int32_t Compare(const CVolMem &s)
    {
        return str.compare(0, str.size(), reinterpret_cast<const CharT *>(s.GetPtr()));
    }
    int32_t Compare(const CVolString &s)
    {
        if (sizeof(CharT) == 2)
            return str.compare(0, str.size(), reinterpret_cast<const CharT *>(s.GetText()));
        else
            return str.compare(PivW2A(s.GetText()).String());
    }
    /**
     * @brief 清空文本
     */
    PivString &Clear()
    {
        str.clear();
        return *this;
    }

    /**
     * @brief 填充文本
     * @param count 重复的字符数量
     * @param ch 所欲填充的字符
     */
    PivString &InitWithChars(const ptrdiff_t count, const CharT ch)
    {
        str.assign(static_cast<size_t>(count), ch);
        return *this;
    }

    /**
     * @brief 置文本
     * @param s 所欲置入的文本数据
     * @param count 文本长度
     * @return 返回自身
     */
    PivString &SetText(const CharT *s, ptrdiff_t count = -1)
    {
        if (count == -1)
            str.assign(s);
        else
            str.assign(s, static_cast<size_t>(count));
        if (!str.empty() && str.back() == '\0')
            str.pop_back();
        return *this;
    }
    PivString &SetText(const ptrdiff_t s, const ptrdiff_t count)
    {
        return SetText(reinterpret_cast<const CharT *>(s), count);
    }
    PivString &SetText(const CVolString &s, const ptrdiff_t count = -1)
    {
        if (sizeof(CharT) == 2)
        {
            str.assign(reinterpret_cast<const CharT *>(s.GetText()), (count == -1) ? static_cast<size_t>(s.GetLength()) : static_cast<size_t>(count));
        }
        else
        {
            PivW2A ansi{s.GetText()};
            str.assign(reinterpret_cast<const CharT *>(ansi.GetText()), (count == -1) ? ansi.GetSize() : static_cast<size_t>(count));
        }
        return *this;
    }
    PivString &SetText(const CVolMem &s, const ptrdiff_t count = -1)
    {
        str.assign(reinterpret_cast<const CharT *>(s.GetPtr()),
                   (count == -1) ? static_cast<size_t>(s.GetSize()) / sizeof(CharT) : static_cast<size_t>(count));
        return *this;
    }
    PivString &SetText(const piv::basic_string_view<CharT> &s, const ptrdiff_t count = -1)
    {
        str.assign(s.data(), (count == -1) ? s.size() : static_cast<size_t>(count));
        return *this;
    }
    PivString &SetText(const int64_t n)
    {
        if (sizeof(CharT) == 2)
            str.assign(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.assign(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    PivString &SetText(const int32_t n)
    {
        if (sizeof(CharT) == 2)
            str.assign(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.assign(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    PivString &SetText(const wchar_t n)
    {
        if (sizeof(CharT) == 2)
            str.assign(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.assign(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    PivString &SetText(const int16_t n)
    {
        if (sizeof(CharT) == 2)
            str.assign(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.assign(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    PivString &SetText(const int8_t n)
    {
        if (sizeof(CharT) == 2)
            str.assign(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.assign(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    PivString &SetText(const double n)
    {
        if (sizeof(CharT) == 2)
            str.assign(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.assign(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }

    /**
     * @brief 置文件资源
     * @param resId 所欲指向的数据资源
     * @param storeBuf 缓存文本数据
     * @return
     */
    bool LoadResData(const size_t resId)
    {
        str.clear();
        if (resId == 0)
        {
            return false;
        }
        HMODULE hModule = g_objVolApp.GetInstanceHandle();
        HRSRC hSrc = ::FindResourceW(hModule, MAKEINTRESOURCE(static_cast<WORD>(resId)), RT_RCDATA);
        if (hSrc == NULL)
        {
            return false;
        }
        HGLOBAL resdata = ::LoadResource(hModule, hSrc);
        if (resdata == NULL)
        {
            return false;
        }
        byte *data = reinterpret_cast<byte *>(::LockResource(resdata));
        size_t count = static_cast<size_t>(::SizeofResource(hModule, hSrc));
        if (count > 2)
        {
            if (data[0] == 0xFF && data[1] == 0xFE)
            {
                data += 2;
                count -= 2;
            }
            else if (data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF)
            {
                data += 3;
                count -= 3;
            }
        }
        str.assign(reinterpret_cast<const CharT *>(data), count / sizeof(CharT));
        return true;
    }

    /**
     * @brief 读入文本文件
     * @param FileName 所欲读取文件名
     * @param ReadDataSize 所欲读取数据尺寸
     * @param ReadEncodeType 所欲写出文本编码
     * @param EncodeType 当前的文本编码
     * @return
     */
    bool ReadFromFile(const wchar_t *FileName, const int32_t ReadDataSize = -1, const VOL_STRING_ENCODE_TYPE ReadEncodeType = VSET_UTF_16, const VOL_STRING_ENCODE_TYPE EncodeType = VSET_UNKNOWN)
    {
        ASSERT(ReadDataSize >= -1);
        ASSERT_R_STR(FileName);
        ASSERT(EncodeType != VSET_UNKNOWN);
        FILE *in = _wfopen(FileName, L"rb");
        if (in == NULL)
            return false;
        bool Succeeded = false;
        fseek(in, 0, SEEK_END);
        size_t fileSize = static_cast<size_t>(ftell(in));
        fseek(in, 0, SEEK_SET);
        if (fileSize > 2)
        {
            byte bom[3];
            fread(bom, 1, 3, in);
            if (bom[0] == 0xFF && bom[1] == 0xFE)
            {
                ReadEncodeType = VSET_UTF_16;
                fseek(in, 2, SEEK_SET);
                fileSize -= 2;
            }
            else if (bom[0] == 0xEF && bom[1] == 0xBB && bom[1] == 0xBF)
            {
                ReadEncodeType = VSET_UTF_8;
                fseek(in, 3, SEEK_SET);
                fileSize -= 3;
            }
        }
        if (ReadEncodeType == VSET_UNKNOWN)
        {
            ReadEncodeType = VSET_MBCS;
        }
        if (ReadDataSize > 0 && static_cast<long>(ReadDataSize) < fileSize)
        {
            fileSize = static_cast<long>(ReadDataSize);
        }
        if (ReadEncodeType == VSET_UTF_16)
        {
            if (EncodeType == VSET_UTF_16)
            {
                str.resize(fileSize / sizeof(CharT));
                Succeeded = (fread(str.data(), sizeof(CharT), fileSize / sizeof(CharT), in) == fileSize / sizeof(CharT));
            }
            else
            {
                char *data = new char[fileSize];
                Succeeded = (fread(data, 1, fileSize, in) == fileSize);
                if (EncodeType == VSET_UTF_8)
                {
                    PivU2W utf{reinterpret_cast<const char *>(data), fileSize};
                    str.assign(reinterpret_cast<const CharT *>(utf.GetText()), utf.GetSize());
                }
                else if (EncodeType == VSET_MBCS)
                {
                    PivA2W utf{reinterpret_cast<const char *>(data), fileSize};
                    str.assign(reinterpret_cast<const CharT *>(utf.GetText()), utf.GetSize());
                }
                delete[] data;
            }
        }
        else if (ReadEncodeType == VSET_UTF_8)
        {
            if (EncodeType == VSET_UTF_8)
            {
                str.resize(fileSize / sizeof(CharT));
                Succeeded = (fread(str.data(), sizeof(CharT), fileSize / sizeof(CharT), in) == fileSize / sizeof(CharT));
            }
            else
            {
                char *data = new char[fileSize];
                Succeeded = (fread(data, 1, fileSize, in) == fileSize);
                if (EncodeType == VSET_UTF_16)
                {
                    PivW2U utf{reinterpret_cast<const wchar_t *>(data), fileSize / 2};
                    str.assign(reinterpret_cast<const CharT *>(utf.GetText()), utf.GetSize());
                }
                else if (EncodeType == VSET_MBCS)
                {
                    PivA2U utf{reinterpret_cast<const char *>(data), fileSize};
                    str.assign(reinterpret_cast<const CharT *>(utf.GetText()), utf.GetSize());
                }
                delete[] data;
            }
        }
        else if (ReadEncodeType == VSET_MBCS)
        {
            if (EncodeType == VSET_MBCS)
            {
                str.resize(fileSize / sizeof(CharT));
                Succeeded = (fread(str.data(), sizeof(CharT), fileSize / sizeof(CharT), in) == fileSize / sizeof(CharT));
            }
            else
            {
                char *data = new char[fileSize];
                Succeeded = (fread(data, 1, fileSize, in) == fileSize);
                if (EncodeType == VSET_UTF_16)
                {
                    PivW2A mbcs{reinterpret_cast<const wchar_t *>(data), fileSize / 2};
                    str.assign(reinterpret_cast<const CharT *>(mbcs.GetText()), mbcs.GetSize());
                }
                else if (EncodeType == VSET_UTF_8)
                {
                    PivU2A mbcs{reinterpret_cast<const char *>(data), fileSize};
                    str.assign(reinterpret_cast<const CharT *>(mbcs.GetText()), mbcs.GetSize());
                }
                delete[] data;
            }
        }
        fclose(in);
        return Succeeded;
    }

    /**
     * @brief 写出文本文件
     * @param FileName 所欲写到文件名
     * @param WriteDataSize 所欲写出文本长度
     * @param WriteEncodeType 所欲写出文本编码
     * @param EncodeType 当前的文本编码
     * @return
     */
    bool WriteIntoFile(const wchar_t *FileName, const int32_t WriteDataSize = -1, const VOL_STRING_ENCODE_TYPE WriteEncodeType = VSET_UTF_16, const VOL_STRING_ENCODE_TYPE EncodeType = VSET_UNKNOWN)
    {
        ASSERT(WriteDataSize >= -1);
        ASSERT_R_STR(FileName);
        ASSERT(WriteEncodeType != VSET_UNKNOWN && EncodeType != VSET_UNKNOWN);
        FILE *out = _wfopen(FileName, L"wb");
        if (out == NULL)
            return false;
        bool Succeeded = false;
        size_t count = (WriteDataSize == -1) ? str.size() : ((static_cast<size_t>(WriteDataSize) > str.size()) ? str.size() : static_cast<size_t>(WriteDataSize));
        if (WriteEncodeType == VSET_UTF_16)
        {
            const byte bom[] = {0xFF, 0xFE};
            fwrite(bom, 1, 2, out);
            if (EncodeType == VSET_UTF_16)
            {
                Succeeded = (fwrite(str.data(), sizeof(CharT), count, out) == count);
            }
            else if (EncodeType == VSET_UTF_8)
            {
                PivU2W utf{reinterpret_cast<const char *>(str.c_str()), count};
                Succeeded = (fwrite(utf.GetText(), 2, utf.GetSize(), out) == utf.GetSize());
            }
            else if (EncodeType == VSET_MBCS)
            {
                PivA2W utf{reinterpret_cast<const char *>(str.c_str()), count};
                Succeeded = (fwrite(utf.GetText(), 2, utf.GetSize(), out) == utf.GetSize());
            }
        }
        else if (WriteEncodeType == VSET_UTF_8)
        {
            const byte bom[] = {0xEF, 0xBB, 0xBF};
            fwrite(bom, 1, 3, out);
            if (EncodeType == VSET_UTF_16)
            {
                PivW2U utf{reinterpret_cast<const wchar_t *>(str.c_str()), count};
                Succeeded = (fwrite(utf.GetText(), 1, utf.GetSize(), out) == utf.GetSize());
            }
            else if (EncodeType == VSET_UTF_8)
            {
                Succeeded = (fwrite(str.data(), sizeof(CharT), count, out) == count);
            }
            else if (EncodeType == VSET_MBCS)
            {
                PivA2U utf{reinterpret_cast<const char *>(str.c_str()), count};
                Succeeded = (fwrite(utf.GetText(), 1, utf.GetSize(), out) == utf.GetSize());
            }
        }
        else if (WriteEncodeType == VSET_MBCS)
        {
            if (EncodeType == VSET_UTF_16)
            {
                PivW2A mbcs{reinterpret_cast<const wchar_t *>(str.c_str()), count};
                Succeeded = (fwrite(mbcs.GetText(), 1, mbcs.GetSize(), out) == mbcs.GetSize());
            }
            else if (EncodeType == VSET_UTF_8)
            {
                PivU2A mbcs{reinterpret_cast<const char *>(str.c_str()), count};
                Succeeded = (fwrite(mbcs.GetText(), 1, mbcs.GetSize(), out) == mbcs.GetSize());
            }
            else if (EncodeType == VSET_MBCS)
            {
                Succeeded = (fwrite(str.data(), sizeof(CharT), count, out) == count);
            }
        }
        fclose(out);
        return Succeeded;
    }

    /**
     * @brief 置文本长度
     * @param length
     * @return
     */
    PivString &SetLength(const ptrdiff_t length)
    {
        str.resize(static_cast<size_t>, ' ');
        return *this;
    }

    /**
     * @brief 加入字符
     * @param ch 所欲添加的字符
     * @return
     */
    PivString &AddChar(const CharT ch)
    {
        str.push_back(ch);
        return *this;
    }
    template <typename... Args>
    PivString &AddChar(const CharT ch, const Args... args)
    {
        str.push_back(ch);
        return AddChar(args...);
    }

    /**
     * @brief 加入重复字符
     * @param count 所欲添加的次数
     * @param ch 所欲添加的字符
     * @return
     */
    PivString &AddManyChar(const ptrdiff_t count, const CharT ch)
    {
        ASSERT(count >= 0);
        str.append(static_cast<size_t>(count), ch);
        return *this;
    }

    /**
     * @brief 加入文本
     * @param s 所欲加入到尾部的文本数据
     * @return 返回自身
     */
    PivString &AddText(const CharT *s)
    {
        str.append(s);
        return *this;
    }
    PivString &AddText(const CVolString &s)
    {
        if (sizeof(CharT) == 2)
        {
            str.append(reinterpret_cast<const CharT *>(s.GetText()));
        }
        else
        {
            PivW2A ansi{s.GetText()};
            str.append(reinterpret_cast<const CharT *>(ansi.GetText()));
        }
        return *this;
    }
    PivString &AddText(const CVolMem &s)
    {
        str.append(reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT));
        return *this;
    }
    PivString &AddText(const std::basic_string<CharT> &s)
    {
        str.append(s);
        return *this;
    }
    PivString &AddText(const piv::basic_string_view<CharT> &s)
    {
        str.append(s.data(), s.size());
        return *this;
    }
    PivString &AddText(const PivString &s)
    {
        str.append(s.str);
        return *this;
    }
    PivString &AddText(const int64_t n)
    {
        if (sizeof(CharT) == 2)
            str.append(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.append(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    PivString &AddText(const int32_t n)
    {
        if (sizeof(CharT) == 2)
            str.append(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.append(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    PivString &AddText(const wchar_t n)
    {
        if (sizeof(CharT) == 2)
            str.append(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.append(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    PivString &AddText(const int16_t n)
    {
        if (sizeof(CharT) == 2)
            str.append(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.append(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    PivString &AddText(const int8_t n)
    {
        if (sizeof(CharT) == 2)
            str.append(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.append(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    PivString &AddText(const double n)
    {
        if (sizeof(CharT) == 2)
            str.append(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.append(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }

    template <typename T, typename... Args>
    PivString &AddText(T s, Args... args)
    {
        return AddText(s).AddText(args...);
    }

    /**
     * @brief 加入部分文本
     * @param s 所欲加入到尾部的文本数据
     * @param count 所欲加入的文本长度
     * @return 返回自身
     */
    PivString &Append(const CharT *s, const ptrdiff_t count = -1)
    {
        if (count == -1)
            str.append(s);
        else
            str.append(s, static_cast<size_t>(count));
        return *this;
    }
    PivString &Append(const CVolString &s, const ptrdiff_t count = -1)
    {

        if (sizeof(CharT) == 2)
        {
            str.append(reinterpret_cast<const CharT *>(s.GetText()), (count == -1) ? static_cast<size_t>(s.GetLength()) : static_cast<size_t>(count));
        }
        else
        {
            PivW2A ansi{s.GetText()};
            str.append(reinterpret_cast<const CharT *>(ansi.GetText()), (count == -1) ? ansi.GetSize() : static_cast<size_t>(count));
        }
        return *this;
    }
    PivString &Append(const CVolMem &s, const ptrdiff_t count = -1)
    {
        str.append(reinterpret_cast<const CharT *>(s.GetPtr()),
                   (count == -1) ? static_cast<size_t>(s.GetSize()) / sizeof(CharT) : static_cast<size_t>(count));
        return *this;
    }
    PivString &Append(const std::basic_string<CharT> &s, const ptrdiff_t count = -1)
    {
        str.append(s, 0, (count == -1) ? s.size() : static_cast<size_t>(count));
        return *this;
    }
    PivString &Append(const piv::basic_string_view<CharT> &s, const ptrdiff_t count = -1)
    {
        str.append(s.data(), (count == -1) ? s.size() : static_cast<size_t>(count));
        return *this;
    }
    PivString &Append(const PivString &s, const ptrdiff_t count = -1)
    {
        str.append(s.str, 0, (count == -1) ? s.str.size() : static_cast<size_t>(count));
        return *this;
    }

    /**
     * @brief 加入重复文本
     * @param s 所欲添加的文本
     * @param times 所欲添加的次数
     * @param count 所欲添加的文本长度
     * @return 返回自身
     */
    PivString &AddManyText(const CharT *s, const ptrdiff_t times, const size_t count = (size_t)-1)
    {
        for (ptrdiff_t i = 0; i < times; i++)
        {
            if (count == (size_t)-1)
                str.append(s);
            else
                str.append(s, count);
        }
        return *this;
    }
    PivString &AddManyText(const std::basic_string<CharT> &s, const ptrdiff_t times)
    {
        return AddManyText(s.c_str(), times);
    }
    PivString &AddManyText(const piv::basic_string_view<CharT> &s, const ptrdiff_t times)
    {
        return AddManyText(s.data(), times, s.size());
    }
    PivString &AddManyText(const CVolMem &s, const ptrdiff_t times)
    {
        return AddManyText(reinterpret_cast<const CharT *>(s.GetPtr()), times, static_cast<size_t>(s.GetSize()) / sizeof(CharT));
    }
    PivString &AddManyText(const CVolString &s, const ptrdiff_t times)
    {
        if (sizeof(CharT) == 2)
            return AddManyText(reinterpret_cast<const CharT *>(s.GetText()), times);
        else
            return AddManyText(reinterpret_cast<const CharT *>(PivW2A(s.GetText()).GetText()), times);
    }

    template <typename... Ts>
    PivString &AddFormatText(const CharT *format, Ts... args)
    {
        str.append(piv::format(format, args...));
        return *this;
    }

    /**
     * @brief 插入文本
     * @param index 所欲插入的索引位置
     * @param s 所欲插入的文本
     * @param ch 所欲插入的字符
     * @param count 所欲插入的文本长度
     * @return
     */
    PivString &InsertText(const ptrdiff_t index, const CharT ch)
    {
        ASSERT(index > 0 && index < str.size());
        str.insert(static_cast<size_t>(index), 1, ch);
        return *this;
    }
    PivString &InsertText(const ptrdiff_t index, const CharT *s, const ptrdiff_t count = -1)
    {
        ASSERT(index > 0 && index < str.size());
        if (count == -1)
            str.insert(static_cast<size_t>(index), s);
        else
            str.insert(static_cast<size_t>(index), s, static_cast<size_t>(count));
        return *this;
    }
    PivString &InsertText(const ptrdiff_t index, const std::basic_string<CharT> &s, const ptrdiff_t count = -1)
    {
        ASSERT(index > 0 && index < str.size());
        if (count == -1)
            str.insert(static_cast<size_t>(index), s);
        else
            str.insert(static_cast<size_t>(index), s, 0, static_cast<size_t>(count));
        return *this;
    }
    PivString &InsertText(const ptrdiff_t index, const piv::basic_string_view<CharT> &s, const ptrdiff_t count = -1)
    {
        return InsertText(index, s.data(), s.size());
    }

    PivString &InsertText(const ptrdiff_t index, const CVolMem &s, const ptrdiff_t count = -1)
    {
        return InsertText(index, reinterpret_cast<const CharT *>(s.GetPtr()),
                          (count == -1) ? s.GetSize() / sizeof(CharT) : count);
    }
    PivString &InsertText(const ptrdiff_t index, const PivString &s, const ptrdiff_t count = -1)
    {
        return InsertText(index, s.str, count);
    }
    PivString &InsertText(const ptrdiff_t index, const CVolString &s, const ptrdiff_t count = -1)
    {
        if (sizeof(CharT) == 2)

            return InsertText(index, reinterpret_cast<const CharT *>(s.GetText()),
                              (count == -1) ? s.GetLength() : count);
        else
            return InsertText(index, PivW2A(s.GetText(), (count == -1) ? static_cast<size_t>(s.GetLength()) : static_cast<size_t>(count)), -1);
    }

    /**
     * @brief 插入行首空格
     * @param count 所欲插入的空格数
     * @return
     */
    PivString &InsertLineBeginLeaderSpaces(const int32_t count)
    {
        ASSERT(count >= 0);
        if (str.empty())
            return *this;
        size_t fpos = 0, pos = 0;
        str.insert(0, static_cast<size_t>(count), ' ');
        while (fpos < str.size())
        {
            fpos = str.find_first_of('\n', pos);
            if (fpos == std::basic_string<CharT>::npos)
                break;
            str.insert(fpos + 1, static_cast<size_t>(count), ' ');
            pos = fpos + static_cast<size_t>(count);
        }
        return *this;
    }

    /**
     * @brief 删除字符
     * @param pos 首字符索引位置
     * @param count 欲删除字符数目
     * @return
     */
    PivString &RemoveChar(const ptrdiff_t pos, const ptrdiff_t count)
    {
        ASSERT(pos > 0 && pos < str.size() && count >= 0);
        str.erase(static_cast<size_t>(pos), static_cast<size_t>(count));
        return *this;
    }

    /**
     * @brief 删除部分文本
     * @param pos 首字符索引位置
     * @param count 欲删除字符数目
     * @return
     */
    ptrdiff_t &RemoveChars(const ptrdiff_t pos, const ptrdiff_t count)
    {
        ASSERT(pos > 0 && pos < str.size() && count >= 0);
        size_t ret = str.size();
        str.erase(static_cast<size_t>(pos), static_cast<size_t>(count));
        return ret - str.size();
    }

    /**
     * @brief 取字符
     */
    const CharT &At(const size_t pos)
    {
        // 判断索引是否有效和文本是否为空
        ASSERT(pos >= 0 && pos < str.size() && str.empty() == false);
        return str[pos];
    }

    /**
     * @brief 取首字符
     */
    const CharT &Front()
    {
        return str.empty() ? 0 : str.front();
    }

    /**
     * @brief 取尾字符
     */
    const CharT &Back()
    {
        return str.empty() ? 0 : str.back();
    }

    /**
     * @brief 取文本中间
     * @param pos 起始取出索引位置
     * @param count 欲取出字符的数目
     * @return 返回文本类
     */
    PivString Middle(const size_t pos, const size_t count)
    {
        ASSERT(pos <= str.size()); // 判断索引位置是否有效
        return PivString{str.substr(pos, count)};
    }

    /**
     * @brief 取文本左边
     * @param count 欲取出字符的数目
     * @return 返回文本类
     */
    PivString Left(const ptrdiff_t count)
    {
        return PivString{str.substr(0, static_cast<size_t>(count))};
    }

    /**
     * @brief 取文本右边
     * @param count 欲取出字符的数目
     * @return 返回文本类
     */
    PivString Right(const ptrdiff_t count)
    {
        size_t pos = static_cast<size_t>(count) > str.size() ? 0 : str.size() - static_cast<size_t>(count);
        return PivString{str.substr(pos, static_cast<size_t>(count))};
    }

    /**
     * @brief 寻找字符
     * @param character 欲寻找的字符
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    size_t FindChar(const CharT character, const ptrdiff_t off = 0)
    {
        if (off < 0 || off >= str.size())
        {
            return std::basic_string<CharT>::npos;
        }
        return str.find(character, static_cast<size_t>(off));
    }

    /**
     * @brief 倒找字符
     * @param character 欲寻找的字符
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    size_t ReverseFindChar(const CharT character, const ptrdiff_t off = -1)
    {
        if (off < 0)
        {
            off = str.size();
        }
        else if (off >= str.size())
        {
            return std::basic_string<CharT>::npos;
        }
        return str.rfind(character, static_cast<size_t>(off));
    }

    /**
     * @brief 寻找字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    size_t FindFirstOf(const std::basic_string<CharT> &chars, const ptrdiff_t pos = 0)
    {
        return str.find_first_of(chars, static_cast<size_t>(pos));
    }
    size_t FindFirstOf(const CharT *chars, const ptrdiff_t pos = 0, const ptrdiff_t count = -1)
    {
        return (count == -1) ? str.find_first_of(chars, static_cast<size_t>(pos))
                             : str.find_first_of(chars, static_cast<size_t>(pos), static_cast<size_t>(count));
    }
    size_t FindFirstOf(const CVolMem &chars, const ptrdiff_t pos = 0)
    {
        return FindFirstOf(reinterpret_cast<const CharT *>(chars.GetPtr()), pos, chars.GetSize() / sizeof(CharT));
    }
    size_t FindFirstOf(const PivString &chars, const ptrdiff_t pos = 0)
    {
        return FindFirstOf(chars.str, pos);
    }
    size_t FindFirstOf(const piv::basic_string_view<CharT> &chars, const ptrdiff_t pos = 0)
    {
        return FindFirstOf(chars.data(), pos, chars.size());
    }
    size_t FindFirstOf(const CVolString &chars, const ptrdiff_t pos = 0)
    {
        if (sizeof(CharT) == 2)
        {
            return FindFirstOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        }
        else
        {
            return FindFirstOf(PivW2A(chars.GetText()).String(), pos);
        }
    }

    /**
     * @brief 倒找字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    size_t FindLastOf(const std::basic_string<CharT> &chars, const ptrdiff_t pos = -1)
    {
        size_t p = (pos == -1) ? str.size() : static_cast<size_t>(pos);
        return str.find_last_of(chars, p);
    }
    size_t FindLastOf(const CharT *chars, const ptrdiff_t pos = -1, const ptrdiff_t count = -1)
    {
        size_t p = (pos == -1) ? str.size() : static_cast<size_t>(pos);
        return (count == -1) ? str.find_last_of(chars, p)
                             : str.find_last_of(chars, p, static_cast<size_t>(count));
    }
    size_t FindLastOf(const CVolMem &chars, const ptrdiff_t pos = -1)
    {
        return FindLastOf(reinterpret_cast<const CharT *>(chars.GetPtr()), pos, chars.GetSize() / sizeof(CharT));
    }
    size_t FindLastOf(const PivString &chars, const ptrdiff_t pos = -1)
    {
        return FindLastOf(chars.str, pos);
    }
    size_t FindLastOf(const piv::basic_string_view<CharT> &chars, const ptrdiff_t pos = -1)
    {
        return FindLastOf(chars.data(), pos, chars.size());
    }
    size_t FindLastOf(const CVolString &chars, const ptrdiff_t pos = -1)
    {
        if (sizeof(CharT) == 2)
        {
            return FindLastOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        }
        else
        {
            return FindLastOf(PivW2A(chars.GetText()).String(), pos);
        }
    }

    /**
     * @brief 寻找不符合字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    size_t FindFirstNotOf(const std::basic_string<CharT> &chars, const ptrdiff_t pos = 0)
    {
        return str.find_first_not_of(chars, static_cast<size_t>(pos));
    }
    size_t FindFirstNotOf(const CharT *chars, const ptrdiff_t pos = 0, const ptrdiff_t count = -1)
    {
        return (count == -1) ? str.find_first_not_of(chars, static_cast<size_t>(pos))
                             : str.find_first_not_of(chars, static_cast<size_t>(pos), static_cast<size_t>(count));
    }
    size_t FindFirstNotOf(const CVolMem &chars, const ptrdiff_t pos = 0)
    {
        return FindFirstNotOf(reinterpret_cast<const CharT *>(chars.GetPtr()), pos, chars.GetSize() / sizeof(CharT));
    }
    size_t FindFirstNotOf(const PivString &chars, const ptrdiff_t pos = 0)
    {
        return FindFirstNotOf(chars.str, pos);
    }
    size_t FindFirstNotOf(const piv::basic_string_view<CharT> &chars, const ptrdiff_t pos = 0)
    {
        return FindFirstNotOf(chars.data(), pos, chars.size());
    }
    size_t FindFirstNotOf(const CVolString &chars, const ptrdiff_t pos = 0)
    {
        if (sizeof(CharT) == 2)
        {
            return FindFirstNotOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        }
        else
        {
            return FindFirstNotOf(PivW2A(chars.GetText()).String(), pos);
        }
    }

    /**
     * @brief 倒找字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    size_t FindLastNotOf(const std::basic_string<CharT> &chars, const ptrdiff_t pos = -1)
    {
        size_t p = (pos == -1) ? str.size() : static_cast<size_t>(pos);
        return str.find_last_not_of(chars, p);
    }
    size_t FindLastNotOf(const CharT *chars, const ptrdiff_t pos = -1, const ptrdiff_t count = -1)
    {
        size_t p = (pos == -1) ? str.size() : static_cast<size_t>(pos);
        return (count == -1) ? str.find_last_not_of(chars, p)
                             : str.find_last_not_of(chars, p, static_cast<size_t>(count));
    }
    size_t FindLastNotOf(const CVolMem &chars, const ptrdiff_t pos = -1)
    {
        return FindLastNotOf(reinterpret_cast<const CharT *>(chars.GetPtr()), pos, chars.GetSize() / sizeof(CharT));
    }
    size_t FindLastNotOf(const PivString &chars, const ptrdiff_t pos = -1)
    {
        return FindLastNotOf(chars.str, pos);
    }
    size_t FindLastNotOf(const piv::basic_string_view<CharT> &chars, const ptrdiff_t pos = -1)
    {
        return FindLastNotOf(chars.data(), pos, chars.size());
    }
    size_t FindLastNotOf(const CVolString &chars, const ptrdiff_t pos = -1)
    {
        if (sizeof(CharT) == 2)
        {
            return FindLastNotOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        }
        else
        {
            return FindLastNotOf(PivW2A(chars.GetText()).String(), pos);
        }
    }

    /**
     * @brief 寻找文本
     * @param text 欲寻找的文本
     * @param pos 起始搜寻位置
     * @param case_insensitive 是否不区分大小写
     * @param count 欲寻找文本的长度
     * @return 寻找到的位置
     */
    size_t SearchText(const piv::basic_string_view<CharT> &text, const ptrdiff_t pos = 0, const bool case_insensitive = false)
    {
        if (!case_insensitive)
            return str.find(text.data(), static_cast<size_t>(pos), text.size());
        else
            return piv::ifind(piv::basic_string_view<CharT>{str}, piv::basic_string_view<CharT>{text}, pos);
    }
    size_t SearchText(const std::basic_string<CharT> &text, const ptrdiff_t pos = 0, const bool case_insensitive = false)
    {
        if (!case_insensitive)
            return str.find(text, static_cast<size_t>(pos));
        else
            return piv::ifind(piv::basic_string_view<CharT>{str}, piv::basic_string_view<CharT>{text}, pos);
    }
    size_t SearchText(const CharT *text, const ptrdiff_t pos = 0, const bool case_insensitive = false, const ptrdiff_t count = -1)
    {
        return SearchText((count == -1) ? piv::basic_string_view<CharT>{text} : piv::basic_string_view<CharT>{text, static_cast<size_t>(count)}, pos, case_insensitive);
    }
    size_t SearchText(const CVolMem &text, const ptrdiff_t pos = 0, const bool case_insensitive = false)
    {
        return SearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetPtr()), static_cast<size_t>(text.GetSize()) / sizeof(CharT)}, pos, case_insensitive);
    }
    size_t SearchText(const PivString &text, const ptrdiff_t pos = 0, const bool case_insensitive = false)
    {
        return SearchText(text.str, pos, case_insensitive);
    }
    size_t SearchText(const CVolString &text, const ptrdiff_t pos = 0, const bool case_insensitive = false)
    {
        if (sizeof(CharT) == 2)
        {
            return SearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetText())}, pos, case_insensitive);
        }
        else
        {
            return SearchText(PivW2A(text.GetText()).String(), pos, case_insensitive);
        }
    }

    /**
     * @brief 倒找文本
     * @param text 欲寻找的文本
     * @param pos 起始搜寻位置
     * @param count 欲寻找文本的长度
     * @param case_insensitive 是否不区分大小写
     * @return 寻找到的位置
     */
    size_t ReverseSearchText(const piv::basic_string_view<CharT> &text, const ptrdiff_t pos = -1, const bool case_insensitive = false)
    {
        size_t p = (pos == -1) ? str.size() : static_cast<size_t>(pos);
        if (!case_insensitive)
            return str.rfind(text.data(), p, text.size());
        else
            return piv::irfind(piv::basic_string_view<CharT>{str}, text, p);
    }
    size_t ReverseSearchText(const std::basic_string<CharT> &text, const ptrdiff_t pos = -1, const bool case_insensitive = false)
    {
        return ReverseSearchText(piv::basic_string_view<CharT>{text}, pos, case_insensitive);
    }
    size_t ReverseSearchText(const CharT *text, const ptrdiff_t pos = -1, const bool case_insensitive = false, const ptrdiff_t count = -1)
    {
        return ReverseSearchText((count == -1) ? piv::basic_string_view<CharT>{text} : piv::basic_string_view<CharT>{text, static_cast<size_t>(count)}, pos, case_insensitive);
    }
    size_t ReverseSearchText(const CVolMem &text, const ptrdiff_t pos = -1, const bool case_insensitive = false)
    {
        return ReverseSearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetPtr()), static_cast<size_t>(text.GetSize()) / sizeof(CharT)}, pos, case_insensitive);
    }
    size_t ReverseSearchText(const PivString &text, const ptrdiff_t pos = -1, const bool case_insensitive = false)
    {
        return ReverseSearchText(text.str, pos, case_insensitive);
    }
    size_t ReverseSearchText(const CVolString &text, const ptrdiff_t pos = -1, const bool case_insensitive = false)
    {
        if (sizeof(CharT) == 2)
            return ReverseSearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetText())}, pos, case_insensitive);
        else
            return ReverseSearchText(PivW2A(text.GetText()).String(), pos, case_insensitive);
    }

    /**
     * @brief 是否以字符/文本开头
     * @param s 所欲检查的文本
     * @param ch 所欲检查的字符
     * @param count 所欲检查文本的长度
     * @param case_sensitive 是否区分大小写
     * @return
     */
    bool LeadOf(const CharT ch, const bool case_sensitive = true)
    {
        if (str.empty())
            return false;
        if (case_sensitive)
            return (str.front() == ch);
        else
            return (std::tolower(static_cast<unsigned char>(str.front())) == std::tolower(static_cast<unsigned char>(ch)));
    }
    bool LeadOf(const CharT *s, const bool case_sensitive = true, const size_t count = (size_t)-1)
    {
        return (SearchText(s, 0, !case_sensitive, count) == 0);
    }
    bool LeadOf(const std::basic_string<CharT> &s, const bool case_sensitive = true)
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }
    bool LeadOf(const CVolMem &s, const bool case_sensitive = true)
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }
    bool LeadOf(const PivString &s, const bool case_sensitive = true)
    {
        return (SearchText(s.str, 0, !case_sensitive) == 0);
    }
    bool LeadOf(const piv::basic_string_view<CharT> &s, const bool case_sensitive = true)
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }
    bool LeadOf(const CVolString &s, const bool case_sensitive = true)
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }

    /**
     * @brief 是否以字符/文本结束
     * @param s 所欲检查的文本
     * @param ch 所欲检查的字符
     * @param count 所欲检查文本的长度
     * @param case_sensitive 是否区分大小写
     * @return
     */
    bool EndOf(const CharT ch, const bool case_sensitive = true)
    {
        if (str.empty())
            return false;
        if (case_sensitive)
            return (str.back() == ch);
        else
            return (std::tolower(static_cast<unsigned char>(str.back())) == std::tolower(static_cast<unsigned char>(ch)));
    }
    bool EndOf(const piv::basic_string_view<CharT> &s, const bool case_sensitive = true)
    {
        return (ReverseSearchText(s, -1, !case_sensitive) == (str.size() - s.size()));
    }
    bool EndOf(const CharT *s, const bool case_sensitive = true, const size_t count = (size_t)-1)
    {
        return EndOf((count == -1) ? piv::basic_string_view<CharT>{s} : piv::basic_string_view<CharT>{s, static_cast<size_t>(count)}, case_sensitive);
    }
    bool EndOf(const std::basic_string<CharT> &s, const bool case_sensitive = true)
    {
        return EndOf(piv::basic_string_view<CharT>{s}, case_sensitive);
    }
    bool EndOf(const CVolMem &s, const bool case_sensitive = true)
    {
        return EndOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT)}, case_sensitive);
    }
    bool EndOf(const PivString &s, const bool case_sensitive = true)
    {
        return EndOf(s.str, case_sensitive);
    }
    bool EndOf(const CVolString &s, const bool case_sensitive = true)
    {
        if (sizeof(CharT) == 2)
            return EndOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetText())}, case_sensitive);
        else
            return EndOf(PivW2A(s.GetText()).String(), case_sensitive);
    }

    /**
     * @brief 替换字符
     * @param BeginCharIndex 起始替换索引位置
     * @param Find 所欲被替换的字符
     * @param Replace 所欲替换到的字符
     * @return 返回是否产生了实际替换
     */
    bool Replace(const ptrdiff_t pos, const ptrdiff_t findCh, const CharT replaceCH)
    {
        ASSERT(pos >= 0 && pos <= str.length());
        bool replaced = false;
        for (size_t i = static_cast<size_t>(pos); i < str.size(); i++)
        {
            if (str[i] == findCh)
            {
                str[i] = replaceCH;
                replaced = true;
            }
        }
        return replaced;
    }

    /**
     * @brief 文本替换
     * @param pos 起始替换索引位置
     * @param count 替换长度
     * @param s 用作替换的文本
     * @return
     */
    PivString &Replace(const ptrdiff_t pos, const ptrdiff_t count, const CharT *s, const ptrdiff_t count2 = -1)
    {
        ASSERT(pos >= 0 && pos <= str.length() && count >= 0);
        if (count2 == -1)
            str.replace(static_cast<size_t>(pos), static_cast<size_t>(count), s);
        else
            str.replace(static_cast<size_t>(pos), static_cast<size_t>(count), s, static_cast<size_t>(count2));
        return *this;
    }
    PivString &Replace(const ptrdiff_t pos, const ptrdiff_t count, const std::basic_string<CharT> &s)
    {
        ASSERT(pos >= 0 && pos <= str.length() && count >= 0);
        str.replace(static_cast<size_t>(pos), static_cast<size_t>(count), s);
        return *this;
    }
    PivString &Replace(const ptrdiff_t pos, const ptrdiff_t count, const piv::basic_string_view<CharT> &s)
    {
        return Replace(pos, count, reinterpret_cast<const CharT *>(s.data()), s.size());
    }
    PivString &Replace(const ptrdiff_t pos, const ptrdiff_t count, const CVolMem &s)
    {
        return Replace(pos, count, reinterpret_cast<const CharT *>(s.GetPtr()), s.GetSize() * sizeof(CharT));
    }
    PivString &Replace(const ptrdiff_t pos, const ptrdiff_t count, const CVolString &s)
    {
        if (sizeof(CharT) == 2)
            return Replace(pos, count, reinterpret_cast<const CharT *>(s.GetText()), -1);
        else
            return Replace(pos, count, PivW2A(s.GetText()).String());
    }

    /**
     * @brief 子文本替换
     * @param findStr 欲被替换的子文本
     * @param repStr 用作替换的文本
     * @param pos 起始替换索引位置
     * @param count 替换次数
     * @param case_sensitive 是否区分大小写
     * @return
     */
    PivString &ReplaceSubText(const piv::basic_string_view<CharT> &findStr, const piv::basic_string_view<CharT> &repStr, const ptrdiff_t pos = 0, const ptrdiff_t count = -1, const bool case_sensitive = true)
    {
        ASSERT(pos >= 0 && pos <= str.length());
        size_t fpos = 0, i = 0;
        while (fpos < str.length())
        {
            if (case_sensitive)
                fpos = str.find(findStr.data(), pos, findStr.size());
            else
                fpos = piv::ifind(piv::basic_string_view<CharT>{str}, findStr, pos);
            if (fpos == std::basic_string<CharT>::npos)
                break;
            str.replace(fpos, findStr.size(), repStr.data(), repStr.size());
            pos = fpos + repStr.size();
            i++;
            if (count > 0 && i >= count)
                break;
        }
        return *this;
    }
    PivString &ReplaceSubText(const CharT *findStr, const CharT *repStr, const ptrdiff_t pos = 0, const ptrdiff_t count = -1, const bool case_sensitive = true, const size_t findLen = (size_t)-1, const size_t repLen = (size_t)-1)
    {
        ASSERT(pos >= 0 && pos <= str.length());
        if (findLen == (size_t)-1)
            findLen = (sizeof(CharT) == 2) ? wcslen(reinterpret_cast<const wchar_t *>(findStr)) : strlen(reinterpret_cast<const char *>(findStr));
        if (repLen == (size_t)-1)
            repLen = (sizeof(CharT) == 2) ? wcslen(reinterpret_cast<const wchar_t *>(repStr)) : strlen(reinterpret_cast<const char *>(repStr));
        return ReplaceSubText(piv::basic_string_view<CharT>{findStr, findLen}, piv::basic_string_view<CharT>{repStr, repLen}, pos, count, case_sensitive);
    }
    PivString &ReplaceSubText(const std::basic_string<CharT> &findStr, const std::basic_string<CharT> &repStr, const ptrdiff_t pos = 0, const ptrdiff_t count = -1, const bool case_sensitive = true)
    {
        return ReplaceSubText(piv::basic_string_view<CharT>{findStr.c_str()}, piv::basic_string_view<CharT>{repStr.c_str()}, pos, count, case_sensitive);
    }
    PivString &ReplaceSubText(const PivString &findStr, const PivString &repStr, const ptrdiff_t pos = 0, const ptrdiff_t count = -1, const bool case_sensitive = true)
    {
        return ReplaceSubText(findStr.str, repStr.str, pos, count, case_sensitive);
    }
    PivString &ReplaceSubText(const CVolMem &findStr, const CVolMem &repStr, const ptrdiff_t pos = 0, const ptrdiff_t count = -1, const bool case_sensitive = true)
    {
        return ReplaceSubText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(findStr.GetPtr()), static_cast<size_t>(findStr.GetSize()) / sizeof(CharT)},
                              piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(repStr.GetPtr()), static_cast<size_t>(repStr.GetSize()) / sizeof(CharT)}, pos, count, case_sensitive);
    }
    PivString &ReplaceSubText(const CVolString &findStr, const CVolString &repStr, const ptrdiff_t pos = 0, const ptrdiff_t count = -1, const bool case_sensitive = true)
    {
        if (sizeof(CharT) == 2)
        {
            return ReplaceSubText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(findStr.GetText())},
                                  piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(repStr.GetText())}, pos, count, case_sensitive);
        }
        else
        {
            PivW2A find_mbcs{findStr.GetText()};
            PivW2A rep_mbcs{repStr.GetText()};
            return ReplaceSubText(find_mbcs.String(), rep_mbcs.String(), pos, count, case_sensitive);
        }
    }

    /**
     * @brief 替换控制符
     */
    PivString &ReplaceAllControlCharsToSpace()
    {
        if (str.empty())
            return *this;
        for (size_t i = 0; i < str.size(); ++i)
        {
            if (std::iscntrl(static_cast<unsigned char>(str[i])))
                str[i] = ' ';
        }
        return *this;
    }

    /**
     * @brief 检查加入换行文本
     * @return
     */
    PivString &CheckAddCRLF()
    {
        if (str.empty())
        {
            str.push_back('\r');
            str.push_back('\n');
            return *this;
        }
        if (str.back() != '\n')
        {
            if (str.back() != '\r')
            {
                str.push_back('\r');
            }
            str.push_back('\n');
        }
        return *this;
    }

    /**
     * @brief 删除空白行
     * @return
     */
    PivString &RemoveAllSpaceLines()
    {
        if (str.empty())
            return *this;
        std::basic_string<CharT> ret;
        ret.reserve(str.size());
        size_t spos = 0, fpos = 0, epos = str.size();
        piv::basic_string_view<CharT> sv = str;
        piv::basic_string_view<CharT> tmp;
        while (fpos < epos)
        {
            fpos = sv.find('\n', spos);
            if (fpos != std::basic_string<CharT>::npos)
            {
                tmp = sv.substr(spos, fpos - spos + 1);
                spos = fpos + 1;
            }
            else
            {
                tmp = sv.substr(spos, sv.size() - spos);
                fpos = epos;
            }
            if (tmp.empty())
                continue;
            bool isSpace = true;
            for (auto it = tmp.begin(); it != tmp.end(); it++)
            {
                if (*it > ' ')
                {
                    isSpace = false;
                    break;
                }
            }
            if (isSpace == false)
            {
                ret.append(tmp.data(), tmp.size());
            }
        }
        str.swap(ret);
        return *this;
    }

    /**
     * @brief 检查加入路径字符
     * @return
     */
    PivString &CheckAddPathChar()
    {
        if (str.empty())
        {
            str.push_back('\\');
            return *this;
        }
        if (str.back() != '\\')
            str.push_back('\\');
        return *this;
    }

    /**
     * @brief 检查删除路径字符
     * @return
     */
    PivString &RemoveEndPathChar()
    {
        if (str.empty())
        {
            return *this;
        }
        while (str.back() == '\\')
        {
            if (str.size() == 3 && str[1] == ':')
            {
                break;
            }
            str.pop_back();
        }
        return *this;
    }

    /**
     * @brief 删首空
     */
    PivString &TrimLeft()
    {
        piv::trim_left(str);
        return *this;
    }

    /**
     * @brief 删尾空
     */
    PivString &TrimRight()
    {
        piv::trim_right(str);
        return *this;
    }

    /**
     * @brief 删首尾空
     */
    PivString &TrimAll()
    {
        piv::trim_left(str);
        piv::trim_right(str);
        return *this;
    }

    /**
     * @brief 删首字符
     */
    PivString &RemoveFront()
    {
        if (!str.empty())
        {
            str.erase(0, 1);
        }
        return *this;
    }

    /**
     * @brief 删尾字符
     */
    PivString &RemoveBack()
    {
        if (!str.empty())
        {
            str.pop_back();
        }
        return *this;
    }

    /**
     * @brief 分割文本
     * @param delimit 分割字符集合
     * @param strArray 结果存放数组
     * @param trimAll 是否删除首尾空
     * @param ignoreEmptyStr 是否忽略空白结果
     * @return 所分割出来的结果文本数目
     */
    size_t SplitStrings(const CharT *delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true, const size_t count = static_cast<size_t>(-1))
    {
        strArray.clear();
        if (count == -1)
            count = (sizeof(CharT) == 2) ? wcslen(reinterpret_cast<const wchar_t *>(delimit)) : strlen(reinterpret_cast<const char *>(delimit));
        if (count == 0)
        {
            strArray.push_back(str);
        }
        else
        {
            size_t spos = 0, fpos = 0, epos = str.size();
            std::basic_string<CharT> s;
            while (fpos < epos)
            {
                fpos = str.find_first_of(delimit, spos, count);
                if (fpos != std::basic_string<CharT>::npos)
                {
                    s = str.substr(spos, fpos - spos);
                    spos = fpos + 1;
                }
                else
                {
                    s = str.substr(spos, str.size() - spos);
                    fpos = epos;
                }
                if (trimAll)
                {
                    piv::trim_left(s);
                    piv::trim_right(s);
                    if (ignoreEmptyStr && s.empty())
                        continue;
                }
                else if (ignoreEmptyStr)
                {
                    if (s.empty())
                        continue;
                    bool isEmpty = true;
                    for (auto it = s.begin(); it != s.end(); it++)
                    {
                        if (*it > ' ')
                        {
                            isEmpty = false;
                            break;
                        }
                    }
                    if (isEmpty)
                        continue;
                }
                strArray.push_back(s);
            }
        }
        return strArray.size();
    }
    size_t SplitStrings(const std::basic_string<CharT> &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        return SplitStrings(delimit.c_str(), strArray, trimAll, ignoreEmptyStr, delimit.size());
    }
    size_t SplitStrings(const CVolMem &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        return SplitStrings(reinterpret_cast<const CharT *>(delimit.GetPtr()), svArray, trimAll, ignoreEmptyStr, static_cast<size_t>(delimit.GetSize() / sizeof(CharT)));
    }
    size_t SplitStrings(const PivString &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        return SplitStrings(delimit.str, strArray, trimAll, ignoreEmptyStr);
    }
    size_t SplitStrings(const piv::basic_string_view<CharT> &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        return SplitStrings(delimit.data(), strArray, trimAll, ignoreEmptyStr, delimit.size());
    }
    size_t SplitStrings(const CVolString &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        if (sizeof(CharT) == 2)
        {
            return SplitStrings(reinterpret_cast<const CharT *>(delimit.GetText()), strArray, trimAll, ignoreEmptyStr, static_cast<size_t>(delimit.GetLength()));
        }
        else
        {
            return SplitStrings(PivW2A(delimit.GetText()).String(), strArray, trimAll, ignoreEmptyStr);
        }
    }

    /**
     * @brief 分割子文本
     * @param delimit 分割用子文本
     * @param strArray 结果存放数组
     * @param trimAll 是否删除首尾空
     * @param ignoreEmptyStr 是否忽略空白结果
     * @return 所分割出来的结果文本数目
     */
    size_t SplitSubStrings(const CharT *delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true, const size_t count = static_cast<size_t>(-1))
    {
        strArray.clear();
        if (count == -1)
            count = (sizeof(CharT) == 2) ? wcslen(reinterpret_cast<const wchar_t *>(delimit)) : strlen(reinterpret_cast<const char *>(delimit));
        if (count == 0)
        {
            strArray.push_back(str);
        }
        else
        {
            size_t spos = 0, fpos = 0, epos = str.size();
            std::basic_string<CharT> s;
            while (fpos < epos)
            {
                fpos = str.find(delimit, spos, count);
                if (fpos != std::basic_string<CharT>::npos)
                {
                    s = str.substr(spos, fpos - spos);
                    spos = fpos + count;
                }
                else
                {
                    s = str.substr(spos, str.size() - spos);
                    fpos = epos;
                }
                if (trimAll)
                {
                    piv::trim_left(s);
                    piv::trim_right(s);
                    if (ignoreEmptyStr && s.empty())
                        continue;
                }
                else if (ignoreEmptyStr)
                {
                    if (s.empty())
                        continue;
                    bool isEmpty = true;
                    for (auto it = s.begin(); it != s.end(); it++)
                    {
                        if (*it > ' ')
                        {
                            isEmpty = false;
                            break;
                        }
                    }
                    if (isEmpty)
                        continue;
                }
                strArray.push_back(s);
            }
        }
        return strArray.size();
    }
    size_t SplitSubStrings(const std::basic_string<CharT> &delimit, std::vector<std::basic_string<CharT>> &strArray, bool trimAll = true, bool ignoreEmptyStr = true)
    {
        return SplitSubStrings(delimit.c_str(), strArray, trimAll, ignoreEmptyStr, delimit.size());
    }
    size_t SplitSubStrings(const CVolMem &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        return SplitSubStrings(reinterpret_cast<const CharT *>(delimit.GetPtr()), strArray, trimAll, ignoreEmptyStr, static_cast<size_t>(delimit.GetSize()) / sizeof(CharT));
    }
    size_t SplitSubStrings(const PivString &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        return SplitSubStrings(delimit.str, strArray, trimAll, ignoreEmptyStr);
    }
    size_t SplitSubStrings(const piv::basic_string_view<CharT> &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        return SplitSubStrings(delimit.data(), strArray, trimAll, ignoreEmptyStr, delimit.size());
    }
    size_t SplitSubStrings(const CVolString &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        if (sizeof(CharT) == 2)
        {
            return SplitSubStrings(reinterpret_cast<const CharT *>(delimit.GetText()), strArray, trimAll, ignoreEmptyStr, static_cast<size_t>(delimit.GetLength()));
        }
        else
        {
            return SplitSubStrings(PivW2A(delimit.GetText()).String(), strArray, trimAll, ignoreEmptyStr);
        }
    }

    /**
     * @brief 复制文本
     * @param dest 欲复制到的目标
     * @param pos 欲复制的起始索引位置
     * @param count 欲复制的字符数目
     * @return 复制的字符数
     */
    size_t CopyStr(CharT *dest, const size_t pos, const size_t count)
    {
        ASSERT(pos <= str.size()); // 判断索引位置是否有效
        return str.copy(dest, count, pos);
    }
    size_t CopyStr(ptrdiff_t dest, const size_t pos, const size_t count)
    {
        return CopyStr(reinterpret_cast<CharT *>(dest), pos, count);
    }
    size_t CopyStr(CVolString &dest, const size_t pos, const size_t count)
    {
        dest.SetLength(count);
        size_t n = CopyStr(const_cast<CharT *>(reinterpret_cast<cont CharT *>(dest.GetText())), pos, count);
        if (n == 0)
            dest.Empty();
        else
            dest.RemoveChars(n, count - n);
        return n;
    }
    size_t CopyStr(CVolMem &dest, const size_t pos, const size_t count)
    {
        dest.Alloc(count, TRUE);
        size_t n = CopyStr(const_cast<CharT *>(reinterpret_cast<cont CharT *>(dest.GetText())), pos, count);
        if (n == 0)
            dest.Empty();
        else
            dest.Realloc(n);
        return n;
    }
    size_t CopyStr(std::basic_string<CharT> &dest, const size_t pos, const size_t count)
    {
        dest.resize(count, '\0');
        size_t n = CopyStr(const_cast<CharT *>(dest.data()), pos, count);
        if (n == 0)
            dest.clear();
        else
            dest.resize(n);
        return n;
    }

    /**
     * @brief 到字节集
     * @return
     */
    CVolMem ToVolMem(const bool null_terminated)
    {
        return CVolMem(reinterpret_cast<const void *>(str.data()), (str.size() + (null_terminated ? 1 : 0)) * sizeof(CharT));
    }

    /**
     * @brief 到文本型
     * @return
     */
    CVolString ToVolString()
    {
        ASSERT(sizeof(CharT) == 2); // 只有UTF-16LE编码的数据可以返回火山文本型
        return CVolString(reinterpret_cast<const wchar_t *>(str.c_str()));
    }

    /**
     * @brief 到整数
     * @param base 进制
     * @return
     */
    int32_t ToInt(const int base)
    {
        try
        {
            return std::stoi(str, nullptr, base);
        }
        catch (const std::exception &e)
        {
            return 0;
        }
    }

    /**
     * @brief 到长整数
     * @param base 进制
     * @return
     */
    int64_t ToInt64(const int base)
    {
        try
        {
            return std::stoll(str, nullptr, base);
        }
        catch (const std::exception &e)
        {
            return 0;
        }
    }

    /**
     * @brief 到小数
     * @return
     */
    double ToDouble()
    {
        try
        {
            return std::stod(str);
        }
        catch (const std::exception &e)
        {
            return 0;
        }
    }

    /**
     * @brief 到文本视图
     * @return
     */
    piv::basic_string_view<CharT> ToStringView()
    {
        return piv::basic_string_view<CharT>{str.c_str()};
    }

    /**
     * @brief 到小写
     * @return
     */
    PivString MakeLower()
    {
        std::basic_string<CharT> lower;
        lower.resize(str.size());
        std::transform(str.begin(), str.end(), lower.begin(), std::tolower);
        return lower;
    }
    PivString &ToLower()
    {
        std::transform(str.begin(), str.end(), str.begin(), std::tolower);
        return *this;
    }

    /**
     * @brief 到大写
     * @return
     */
    PivString MakeUpper()
    {
        std::basic_string<CharT> upper;
        upper.resize(str.size());
        std::transform(str.begin(), str.end(), upper.begin(), std::toupper);
        return upper;
    }
    PivString &ToUpper()
    {
        std::transform(str.begin(), str.end(), str.begin(), std::toupper);
        return *this;
    }

}; // PivString

/*********************************************
 * @brief  文本视图模板类
 * @tparam CharT 字符类型
 */
template <typename CharT>
class PivStringView : public CVolObject
{
private:
    piv::basic_string_view<CharT> sv;
    std::shared_ptr<std::basic_string<CharT>> pStr{nullptr};

public:
    /**
     * @brief 默认构造函数,空文本视窗
     */
    PivStringView() {}

    /**
     * @brief 默认析构函数
     */
    ~PivStringView() {}

    /**
     * @brief 复制构造函数
     * @param s 所欲复制的string_view
     * @param count 文本的字符长度
     * @param ptr 所欲复制的智能指针
     * @param storeBuf 是否缓存文本数据
     */
    PivStringView(const PivStringView &s)
    {
        sv = s.sv;
        pStr = s.pStr;
    }
    PivStringView(PivStringView &&s)
    {
        sv = std::move(s.sv);
        pStr.swap(s.pStr);
    }
    PivStringView(const piv::basic_string_view<CharT> &s)
    {
        sv = s;
    }
    PivStringView(piv::basic_string_view<CharT> &&s)
    {
        sv = std::move(s);
    }
    PivStringView(const piv::basic_string_view<CharT> &s, const std::shared_ptr<std::basic_string<CharT>> &ptr)
    {
        sv = s;
        pStr = ptr;
    }
    PivStringView(const CharT *s, const ptrdiff_t count = -1, const bool storeBuf = false)
    {
        this->SetText(s, count, storeBuf);
    }
    PivStringView(const CVolString &s, const bool storeBuf = false)
    {
        this->SetText(s, storeBuf);
    }
    PivStringView(const CVolMem &s, const bool storeBuf = false)
    {
        this->SetText(s, storeBuf);
    }
    PivStringView(const PivString<CharT> &s, const bool storeBuf = false)
    {
        this->SetText(s, storeBuf);
    }
    PivStringView(const std::basic_string<CharT> &s, const bool storeBuf = false)
    {
        this->SetText(s, storeBuf);
    }

    operator piv::basic_string_view<CharT> &()
    {
        return sv;
    }
    operator const piv::basic_string_view<CharT> &()
    {
        return sv;
    }

    /**
     * @brief 赋值操作符
     * @param other 所欲复制的文本视图类
     * @return 返回自身
     */
    PivStringView &operator=(const PivStringView &other)
    {
        sv = other.sv;
        pStr = other.pStr;
        return (*this);
    }
    PivStringView &operator=(PivStringView &&other)
    {
        sv = std::move(other.sv);
        pStr.swap(other.pStr);
        return (*this);
    }

    /**
     * @brief 比较操作符
     * @param other 所欲比较的文本视图类
     * @return 返回两个文本视图的内容是否相同
     */
    bool operator==(const PivStringView &other)
    {
        return sv == other.sv;
    }

    /**
     * @brief 获取文本视图类中的string_view
     * @return 返回string_view
     */
    piv::basic_string_view<CharT> &data()
    {
        return sv;
    }

    /**
     * @brief 获取文本视图类中的string_view指针
     */
    piv::basic_string_view<CharT> *pdata()
    {
        return &sv;
    }

    /**
     * @brief 取展示内容(调试输出)
     * @param strDump 展示内容
     * @param nMaxDumpSize 最大展示数据尺寸
     */
    virtual void GetDumpString(CVolString &strDump, INT nMaxDumpSize) override
    {
        if (sizeof(CharT) == 2)
        {
            strDump.SetText(sv.data(), sv.length());
        }
        else
        {
            strDump.SetText(L"文本视图类");
            CVolMem(reinterpret_cast<const void *>(sv.data()), sv.size()).GetDumpString(strDump, nMaxDumpSize);
        }
    }

    /**
     * @brief 清空视图
     */
    PivStringView &Clear()
    {
        sv.swap(piv::basic_string_view<CharT>{});
        pStr.reset();
        return *this;
    }

    /**
     * @brief 置文本
     * @param s 所欲指向的文本指针
     * @param count 文本的字符长度
     * @param storeBuf 是否缓存文本数据
     */
    PivStringView &SetText(const CharT *s, const ptrdiff_t count = -1, const bool storeBuf = false)
    {
        if (storeBuf)
        {
            pStr.reset((count == -1) ? new std::basic_string<CharT>{s} : new std::basic_string<CharT>{s, static_cast<size_t>(count)});
            sv = *pStr;
        }
        else
        {
            if (count == -1)
            {
                sv = piv::basic_string_view<CharT>{s};
            }
            else
            {
                sv = piv::basic_string_view<CharT>{s, static_cast<size_t>(count)};
            }
        }
        if (!sv.empty() && sv.back() == '\0')
            sv.remove_suffix(1);
        return *this;
    }
    PivStringView &SetText(const ptrdiff_t s, const ptrdiff_t count, const bool storeBuf = false)
    {
        return SetText(reinterpret_cast<const CharT *>(s), count, storeBuf);
    }
    PivStringView &SetText(const CVolString &s, const bool storeBuf = false)
    {
        ASSERT(sizeof(CharT) == 2); // 只有UTF-16LE编码的文本视图可以置入火山文本型
        if (storeBuf)
        {
            pStr.reset(new std::basic_string<CharT>{s.GetText()});
            sv = *pStr;
        }
        else
        {
            sv = piv::basic_string_view<CharT>{s.GetText()};
        }
        return *this;
    }
    PivStringView &SetText(const CVolMem &s, const bool storeBuf = false)
    {
        if (storeBuf)
        {
            pStr.reset(new std::basic_string<CharT>{reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT)});
            sv = *pStr;
        }
        else
        {
            sv = piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT)};
        }
        return *this;
    }
    PivStringView &SetText(const PivString<CharT> &s, const bool storeBuf = false)
    {
        if (storeBuf)
        {
            pStr.reset(new std::basic_string<CharT>{s.data()});
            sv = *pStr;
        }
        else
        {
            sv = piv::basic_string_view<CharT>{s.data()};
        }
        return *this;
    }
    PivStringView &SetText(const std::basic_string<CharT> &s, const bool storeBuf = false)
    {
        if (storeBuf)
        {
            pStr.reset(new std::basic_string<CharT>{s});
            sv = *pStr;
        }
        else
        {
            sv = piv::basic_string_view<CharT>{s.c_str()};
        }
        return *this;
    }

    /**
     * @brief 置文件资源
     * @param resId 所欲指向的数据资源
     * @param storeBuf 缓存文本数据
     * @return
     */
    bool LoadResData(const size_t resId, const bool storeBuf = false)
    {
        Clear();
        if (resId == 0)
        {
            return false;
        }
        HMODULE hModule = g_objVolApp.GetInstanceHandle();
        HRSRC hSrc = ::FindResourceW(hModule, MAKEINTRESOURCE(static_cast<WORD>(resId)), RT_RCDATA);
        if (hSrc == NULL)
        {
            return false;
        }
        HGLOBAL resdata = ::LoadResource(hModule, hSrc);
        if (resdata == NULL)
        {
            return false;
        }
        byte *data = reinterpret_cast<byte *>(::LockResource(resdata));
        size_t count = static_cast<size_t>(::SizeofResource(hModule, hSrc));
        if (count > 2)
        {
            if (data[0] == 0xFF && data[1] == 0xFE)
            {
                data += 2;
                count -= 2;
            }
            else if (data[0] == 0xEF && data[1] == 0xBB && data[2] == 0xBF)
            {
                data += 3;
                count -= 3;
            }
        }
        if (storeBuf)
        {
            pStr.reset(new std::basic_string<CharT>{reinterpret_cast<const CharT *>(data), count / sizeof(CharT)});
            sv = *pStr;
        }
        else
        {
            sv = piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(data), count / sizeof(CharT)};
        }
        return true;
    }

    /**
     * @brief 取文本指针
     * @return 返回文本指针
     */
    const CharT *GetText()
    {
        return sv.data();
    }

    /**
     * @brief 取文本长度
     */
    size_t GetLength()
    {
        return sv.length();
    }

    /**
     * @brief 取文本长度
     */
    size_t GetSize()
    {
        return sv.size();
    }

    /**
     * @brief 取文本哈希值
     * @return
     */
    size_t GetHash()
    {
        return std::hash<piv::basic_string_view<CharT>>{}(sv);
    }

    /**
     * @brief 取引用计数
     */
    int32_t UseConut()
    {
        return pStr.use_count();
    }

    /**
     * @brief 是否为空
     * @return 返回文本视图是否为空
     */
    bool IsEmpty()
    {
        return sv.empty();
    }

    /**
     * @brief 是否相同
     * @param other 所欲比较的文本视图类
     * @return 返回两个文本视图类的内容是否相同
     */
    bool IsEqual(const PivStringView &other)
    {
        return sv == other.sv;
    }

    /**
     * @brief 后移起点
     * @param n 所欲从视图起始移除的字符数
     */
    PivStringView &RemovePrefix(const size_t n)
    {
        ASSERT(n <= sv.size()); // 判断移除的字符数是否不大于视图长度
        sv.remove_prefix(n);
        return *this;
    }

    /**
     * @brief 前移终点
     * @param n 所欲从视图终点移除的字符数
     */
    PivStringView &RemoveSuffix(const size_t n)
    {
        ASSERT(n <= sv.size()); // 判断移除的字符数是否不大于视图长度
        sv.remove_suffix(n);
        return *this;
    }

    /**
     * @brief 复制文本
     * @param dest 欲复制到的目标
     * @param pos 欲复制的起始索引位置
     * @param count 欲复制的字符数目
     * @return 复制的字符数
     */
    size_t CopyStr(CharT *dest, const size_t pos, const size_t count)
    {
        ASSERT(pos <= sv.size()); // 判断索引位置是否有效
        return sv.copy(dest, count, pos);
    }
    size_t CopyStr(ptrdiff_t dest, const size_t pos, const size_t count)
    {
        return CopyStr(reinterpret_cast<CharT *>(dest), pos, count);
    }
    size_t CopyStr(CVolString &dest, const size_t pos, const size_t count)
    {
        dest.SetLength(count);
        size_t n = CopyStr(const_cast<CharT *>(reinterpret_cast<cont CharT *>(dest.GetText())), pos, count);
        if (n == 0)
            dest.Empty();
        else
            dest.RemoveChars(n, count - n);
        return n;
    }
    size_t CopyStr(CVolMem &dest, const size_t pos, const size_t count)
    {
        dest.Alloc(count, TRUE);
        size_t n = CopyStr(const_cast<CharT *>(reinterpret_cast<cont CharT *>(dest.GetText())), pos, count);
        if (n == 0)
            dest.Empty();
        else
            dest.Realloc(n);
        return n;
    }
    size_t CopyStr(std::basic_string<CharT> &dest, const size_t pos, const size_t count)
    {
        dest.resize(count, '\0');
        size_t n = CopyStr(const_cast<CharT *>(dest.data()), pos, count);
        if (n == 0)
            dest.clear();
        else
            dest.resize(n);
        return n;
    }

    /**
     * @brief 到字节集
     * @return
     */
    CVolMem ToVolMem(const bool null_terminated)
    {
        return CVolMem(reinterpret_cast<const void *>(sv.data()), (sv.size() + (null_terminated ? 1 : 0)) * sizeof(CharT));
    }

    /**
     * @brief 到文本型
     * @return
     */
    CVolString ToVolString()
    {
        ASSERT(sizeof(CharT) == 2); // 只有UTF-16LE编码的文本视图可以置入火山文本型;
        return CVolString(reinterpret_cast<const wchar_t *>(sv.data()), sv.size());
    }

    /**
     * @brief 到标准文本
     * @return
     */
    std::basic_string<CharT> ToString()
    {
        return std::basic_string<CharT>{sv.data(), sv.size()};
    }

    /**
     * @brief 取字符
     */
    const CharT &At(const size_t pos)
    {
        // 判断索引是否有效和视图是否为空
        ASSERT(pos >= 0 && pos < sv.size() && sv.empty() == false);
        return sv[pos];
    }

    /**
     * @brief 取首字符
     */
    const CharT &Front()
    {
        return sv.empty() ? 0 : sv.front();
    }

    /**
     * @brief 取尾字符
     */
    const CharT &Back()
    {
        return sv.empty() ? 0 : sv.back();
    }

    /**
     * @brief 取文本中间
     * @param pos 起始取出索引位置
     * @param count 欲取出字符的数目
     * @return 返回文本视图类
     */
    PivStringView Middle(const size_t pos, const size_t count)
    {
        ASSERT(pos <= sv.size()); // 判断索引位置是否有效
        return PivStringView{sv.substr(pos, count), pStr};
    }

    /**
     * @brief 取文本左边
     * @param count 欲取出字符的数目
     * @return 返回文本视图类
     */
    PivStringView Left(const ptrdiff_t count)
    {
        return PivStringView{sv.substr(0, static_cast<size_t>(count)), pStr};
    }

    /**
     * @brief 取文本右边
     * @param count 欲取出字符的数目
     * @return 返回文本视图类
     */
    PivStringView Right(const ptrdiff_t count)
    {
        size_t pos = static_cast<size_t>(count) > sv.size() ? 0 : sv.size() - static_cast<size_t>(count);
        return PivStringView{sv.substr(pos, static_cast<size_t>(count)), pStr};
    }

    /**
     * @brief 寻找字符
     * @param character 欲寻找的字符
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    size_t FindChar(const CharT character, const ptrdiff_t off = 0)
    {
        if (off < 0 || off >= sv.size())
        {
            return piv::basic_string_view<CharT>::npos;
        }
        return sv.find(character, static_cast<size_t>(off));
    }

    /**
     * @brief 倒找字符
     * @param character 欲寻找的字符
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    size_t ReverseFindChar(const CharT character, const ptrdiff_t off = -1)
    {
        if (off < 0)
        {
            off = sv.size();
        }
        else if (off >= sv.size())
        {
            return piv::basic_string_view<CharT>::npos;
        }
        return sv.rfind(character, static_cast<size_t>(off));
    }

    /**
     * @brief 寻找字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    size_t FindFirstOf(const piv::basic_string_view<CharT> &chars, const ptrdiff_t pos = 0)
    {
        return sv.find_first_of(chars, static_cast<size_t>(pos));
    }
    size_t FindFirstOf(const CharT *chars, const ptrdiff_t pos = 0, const ptrdiff_t count = -1)
    {
        return (count == -1) ? FindFirstOf(piv::basic_string_view<CharT>{chars}, pos)
                             : FindFirstOf(piv::basic_string_view<CharT>{chars, static_cast<size_t>(count)}, pos);
    }
    size_t FindFirstOf(const CVolMem &chars, const ptrdiff_t pos = 0)
    {
        return FindFirstOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(chars.GetPtr()), static_cast<size_t>(chars.GetSize()) / sizeof(CharT)}, pos);
    }
    size_t FindFirstOf(const PivStringView &chars, const ptrdiff_t pos = 0)
    {
        return FindFirstOf(chars.sv, pos);
    }
    size_t FindFirstOf(const PivString<CharT> &chars, const ptrdiff_t pos = 0)
    {
        return FindFirstOf(piv::basic_string_view<CharT>{chars.str}, pos);
    }
    size_t FindFirstOf(const CVolString &chars, const ptrdiff_t pos = 0)
    {
        if (sizeof(CharT) == 2)
        {
            return FindFirstOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        }
        else
        {
            return FindFirstOf(piv::basic_string_view<CharT>{PivW2A(chars.GetText()).String()}, pos);
        }
    }

    /**
     * @brief 倒找字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    size_t FindLastOf(const piv::basic_string_view<CharT> &chars, const ptrdiff_t pos = -1)
    {
        size_t p = (pos == -1) ? sv.size() : static_cast<size_t>(pos);
        return sv.find_last_of(chars, p);
    }
    size_t FindLastOf(const CharT *chars, const ptrdiff_t pos = -1, const ptrdiff_t count = -1)
    {
        return (count == -1) ? FindLastOf(piv::basic_string_view<CharT>{chars}, pos)
                             : FindLastOf(piv::basic_string_view<CharT>{chars, static_cast<size_t>(count)}, pos);
    }
    size_t FindLastOf(const CVolMem &chars, const ptrdiff_t pos = -1)
    {
        return FindLastOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(chars.GetPtr()), static_cast<size_t>(chars.GetSize()) / sizeof(CharT)}, pos);
    }
    size_t FindLastOf(const PivStringView &chars, const ptrdiff_t pos = -1)
    {
        return FindLastOf(chars.sv, pos);
    }
    size_t FindLastOf(const PivString<CharT> &chars, const ptrdiff_t pos = -1)
    {
        return FindLastOf(piv::basic_string_view<CharT>{chars.str}, pos);
    }
    size_t FindLastOf(const CVolString &chars, const ptrdiff_t pos = -1)
    {
        if (sizeof(CharT) == 2)
        {
            return FindLastOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        }
        else
        {
            return FindLastOf(piv::basic_string_view<CharT>{PivW2A(chars.GetText()).String()}, pos);
        }
    }

    /**
     * @brief 寻找不符合字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    size_t FindFirstNotOf(const piv::basic_string_view<CharT> &chars, const ptrdiff_t pos = 0)
    {
        return sv.find_first_not_of(chars, static_cast<size_t>(pos));
    }
    size_t FindFirstNotOf(const CharT *chars, const ptrdiff_t pos = 0, const ptrdiff_t count = -1)
    {
        return (count == -1) ? FindFirstNotOf(piv::basic_string_view<CharT>{chars}, pos)
                             : FindFirstNotOf(piv::basic_string_view<CharT>{chars, static_cast<size_t>(count)}, pos);
    }
    size_t FindFirstNotOf(const CVolMem &chars, const ptrdiff_t pos = 0)
    {
        return FindFirstNotOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(chars.GetPtr()), static_cast<size_t>(chars.GetSize()) / sizeof(CharT)}, pos);
    }
    size_t FindFirstNotOf(const PivStringView &chars, const ptrdiff_t pos = 0)
    {
        return FindFirstNotOf(chars.sv, pos);
    }
    size_t FindFirstNotOf(const PivString<CharT> &chars, const ptrdiff_t pos = 0)
    {
        return FindFirstNotOf(piv::basic_string_view<CharT>{chars.str}, pos);
    }
    size_t FindFirstNotOf(const CVolString &chars, const ptrdiff_t pos = 0)
    {
        if (sizeof(CharT) == 2)
        {
            return FindFirstNotOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        }
        else
        {
            return FindFirstNotOf(piv::basic_string_view<CharT>{PivW2A(chars.GetText()).String()}, pos);
        }
    }

    /**
     * @brief 倒找字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    size_t FindLastNotOf(const piv::basic_string_view<CharT> &chars, const ptrdiff_t pos = -1)
    {
        size_t p = (pos == -1) ? sv.size() : static_cast<size_t>(pos);
        return sv.find_last_not_of(chars, p);
    }
    size_t FindLastNotOf(const CharT *chars, const ptrdiff_t pos = -1, const ptrdiff_t count = -1)
    {
        return (count == -1) ? FindLastNotOf(piv::basic_string_view<CharT>{chars}, pos)
                             : FindLastNotOf(piv::basic_string_view<CharT>{chars, static_cast<size_t>(count)}, pos);
    }
    size_t FindLastNotOf(const CVolMem &chars, const ptrdiff_t pos = -1)
    {
        return FindLastNotOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(chars.GetPtr()), static_cast<size_t>(chars.GetSize()) / sizeof(CharT)}, pos);
    }
    size_t FindLastNotOf(const PivStringView &chars, const ptrdiff_t pos = -1)
    {
        return FindLastNotOf(chars.sv, pos);
    }
    size_t FindLastNotOf(const PivString<CharT> &chars, const ptrdiff_t pos = -1)
    {
        return FindLastNotOf(piv::basic_string_view<CharT>{chars.str}, pos);
    }
    size_t FindLastNotOf(const CVolString &chars, const ptrdiff_t pos = -1)
    {
        if (sizeof(CharT) == 2)
        {
            return FindLastNotOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        }
        else
        {
            return FindLastNotOf(piv::basic_string_view<CharT>{PivW2A(chars.GetText()).String()}, pos);
        }
    }

    /**
     * @brief 寻找文本
     * @param text 欲寻找的文本
     * @param pos 起始搜寻位置
     * @param case_insensitive 是否不区分大小写
     * @param count 欲寻找文本的长度
     * @return 寻找到的位置
     */
    size_t SearchText(const piv::basic_string_view<CharT> &text, const ptrdiff_t pos = 0, const bool case_insensitive = false)
    {
        if (!case_insensitive)
            return sv.find(text, static_cast<size_t>(pos));
        else
            return piv::ifind(sv, text, pos);
    }
    size_t SearchText(const std::basic_string<CharT> &text, const ptrdiff_t pos = 0, const bool case_insensitive = false)
    {
        if (!case_insensitive)
            return str.find(text, static_cast<size_t>(pos));
        else
            return piv::ifind(piv::basic_string_view<CharT>{str}, piv::basic_string_view<CharT>{text}, pos);
    }
    size_t SearchText(const CharT *text, const ptrdiff_t pos = 0, const bool case_insensitive = false, const ptrdiff_t count = -1)
    {
        return SearchText((count == -1) ? piv::basic_string_view<CharT>{text} : piv::basic_string_view<CharT>{text, static_cast<size_t>(count)}, pos, case_insensitive);
    }
    size_t SearchText(const CVolMem &text, const ptrdiff_t pos = 0, const bool case_insensitive = false)
    {
        return SearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetPtr()), static_cast<size_t>(text.GetSize()) / sizeof(CharT)}, pos, case_insensitive);
    }
    size_t SearchText(const CVolString &text, const ptrdiff_t pos = 0, const bool case_insensitive = false)
    {
        if (sizeof(CharT) == 2)
        {
            return SearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetText())}, pos, case_insensitive);
        }
        else
        {
            return SearchText(PivW2A(text.GetText()).String(), pos, case_insensitive);
        }
    }
    size_t SearchText(const PivStringView &text, const ptrdiff_t pos = 0, const bool case_insensitive = false)
    {
        return SearchText(text.sv, pos, case_insensitive);
    }
    size_t SearchText(const PivString<CharT> &text, const ptrdiff_t pos = 0, const bool case_insensitive = false)
    {
        return SearchText(piv::basic_string_view<CharT>{text.str}, pos, case_insensitive);
    }

    /**
     * @brief 倒找文本
     * @param text 欲寻找的文本
     * @param pos 起始搜寻位置
     * @param count 欲寻找文本的长度
     * @param case_insensitive 是否不区分大小写
     * @return 寻找到的位置
     */
    size_t ReverseSearchText(const piv::basic_string_view<CharT> &text, const ptrdiff_t pos = -1, const bool case_insensitive = false)
    {
        size_t p = (pos == -1) ? sv.size() : static_cast<size_t>(pos);
        if (!case_insensitive)
            return sv.rfind(text, p);
        else
            return piv::irfind(sv, text, p);
    }
    size_t ReverseSearchText(const std::basic_string<CharT> &text, const ptrdiff_t pos = -1, const bool case_insensitive = false)
    {
        return ReverseSearchText(piv::basic_string_view<CharT>{text}, pos, case_insensitive);
    }
    size_t ReverseSearchText(const CharT *text, const ptrdiff_t pos = -1, const bool case_insensitive = false, const ptrdiff_t count = -1)
    {
        return ReverseSearchText((count == -1) ? piv::basic_string_view<CharT>{text} : piv::basic_string_view<CharT>{text, static_cast<size_t>(count)}, pos, case_insensitive);
    }
    size_t ReverseSearchText(const CVolMem &text, const ptrdiff_t pos = -1, const bool case_insensitive = false)
    {
        return ReverseSearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetPtr()), static_cast<size_t>(text.GetSize()) / sizeof(CharT)}, pos, case_insensitive);
    }
    size_t ReverseSearchText(const PivString<CharT> &text, const ptrdiff_t pos = -1, const bool case_insensitive = false)
    {
        return ReverseSearchText(text.str, pos, case_insensitive);
    }
    size_t ReverseSearchText(const CVolString &text, const ptrdiff_t pos = -1, const bool case_insensitive = false)
    {
        if (sizeof(CharT) == 2)
            return ReverseSearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetText())}, pos, case_insensitive);
        else
            return ReverseSearchText(PivW2A(text.GetText()).String(), pos, case_insensitive);
    }
    size_t ReverseSearchText(const PivStringView &text, const ptrdiff_t pos = -1, const bool case_insensitive = false)
    {
        return ReverseSearchText(text.sv, pos, case_insensitive);
    }

    /**
     * @brief 是否以字符/文本开头
     * @param s 所欲检查的文本
     * @param ch 所欲检查的字符
     * @param count 所欲检查文本的长度
     * @param case_sensitive 是否区分大小写
     * @return
     */
    bool LeadOf(const CharT ch, const bool case_sensitive = true)
    {
        if (sv.empty())
            return false;
        if (case_sensitive)
            return (sv.front() == ch);
        else
            return (std::tolower(static_cast<unsigned char>(sv.front())) == std::tolower(static_cast<unsigned char>(ch)));
    }
    bool LeadOf(const piv::basic_string_view<CharT> &s, const bool case_sensitive = true)
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }
    bool LeadOf(const CharT *s, const bool case_sensitive = true, const size_t count = (size_t)-1)
    {
        return (SearchText(s, 0, !case_sensitive, count) == 0);
    }
    bool LeadOf(const std::basic_string<CharT> &s, const bool case_sensitive = true)
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }
    bool LeadOf(const CVolMem &s, const bool case_sensitive = true)
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }
    bool LeadOf(const PivStringView &s, const bool case_sensitive = true)
    {
        return (SearchText(s.sv, 0, !case_sensitive) == 0);
    }
    bool LeadOf(const PivString<CharT> &s, const bool case_sensitive = true)
    {
        return (SearchText(s.str, 0, !case_sensitive) == 0);
    }
    bool LeadOf(const CVolString &s, const bool case_sensitive = true)
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }

    /**
     * @brief 是否以字符/文本结束
     * @param s 所欲检查的文本
     * @param ch 所欲检查的字符
     * @param count 所欲检查文本的长度
     * @param case_sensitive 是否区分大小写
     * @return
     */
    bool EndOf(const CharT ch, const bool case_sensitive = true)
    {
        if (sv.empty())
            return false;
        if (case_sensitive)
            return (sv.back() == ch);
        else
            return (std::tolower(static_cast<unsigned char>(sv.back())) == std::tolower(static_cast<unsigned char>(ch)));
    }
    bool EndOf(const piv::basic_string_view<CharT> &s, const bool case_sensitive = true)
    {
        return (ReverseSearchText(s, -1, !case_sensitive) == (sv.size() - s.size()));
    }
    bool EndOf(const CharT *s, const bool case_sensitive = true, const size_t count = (size_t)-1)
    {
        return EndOf((count == -1) ? piv::basic_string_view<CharT>{s} : piv::basic_string_view<CharT>{s, static_cast<size_t>(count)}, case_sensitive);
    }
    bool EndOf(const std::basic_string<CharT> &s, const bool case_sensitive = true)
    {
        return EndOf(piv::basic_string_view<CharT>{s}, case_sensitive);
    }
    bool EndOf(const CVolMem &s, const bool case_sensitive = true)
    {
        return EndOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT)}, case_sensitive);
    }
    bool EndOf(const PivStringView &s, const bool case_sensitive = true)
    {
        return EndOf(s.sv, case_sensitive);
    }
    bool EndOf(const PivString<CharT> &s, const bool case_sensitive = true)
    {
        return EndOf(s.str, case_sensitive);
    }
    bool EndOf(const CVolString &s, const bool case_sensitive = true)
    {
        if (sizeof(CharT) == 2)
            return EndOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetText())}, case_sensitive);
        else
            return EndOf(PivW2A(s.GetText()).String(), case_sensitive);
    }

    /**
     * @brief 删首空
     */
    PivStringView &TrimLeft()
    {
        piv::trim_left(sv);
        return *this;
    }

    /**
     * @brief 删尾空
     */
    PivStringView &TrimRight()
    {
        piv::trim_right(sv);
        return *this;
    }

    /**
     * @brief 删首尾空
     */
    PivStringView &TrimAll()
    {
        piv::trim_left(sv);
        piv::trim_right(sv);
        return *this;
    }

    /**
     * @brief 分割文本
     * @param delimit 分割字符集合
     * @param svArray 结果存放数组
     * @param trimAll 是否删除首尾空
     * @param ignoreEmptyStr 是否忽略空白结果
     * @return 所分割出来的结果文本数目
     */
    size_t SplitStrings(const piv::basic_string_view<CharT> &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        svArray.clear();
        if (delimit.empty())
        {
            svArray.push_back(sv);
        }
        else
        {
            size_t spos = 0, fpos = 0, epos = sv.size();
            piv::basic_string_view<CharT> str;
            while (fpos < epos)
            {
                fpos = sv.find_first_of(delimit, spos);
                if (fpos != piv::basic_string_view<CharT>::npos)
                {
                    str = sv.substr(spos, fpos - spos);
                    spos = fpos + 1;
                }
                else
                {
                    str = sv.substr(spos, sv.size() - spos);
                    fpos = epos;
                }
                if (trimAll)
                {
                    piv::trim_left(str);
                    piv::trim_right(str);
                    if (ignoreEmptyStr && str.empty())
                        continue;
                }
                else if (ignoreEmptyStr)
                {
                    if (str.empty())
                        continue;
                    bool isEmpty = true;
                    for (size_t i = 0; i < str.size(); i++)
                    {
                        // if (!std::isspace(static_cast<unsigned char>(str[i])))
                        if (static_cast<unsigned char>(str[i]) > ' ')
                        {
                            isEmpty = false;
                            break;
                        }
                    }
                    if (isEmpty)
                        continue;
                }
                svArray.push_back(str);
            }
        }
        return svArray.size();
    }
    size_t SplitStrings(const CharT *delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true, const ptrdiff_t count = -1)
    {
        return (count == -1) ? SplitStrings(piv::basic_string_view<CharT>{delimit}, svArray, trimAll, ignoreEmptyStr)
                             : SplitStrings(piv::basic_string_view<CharT>{delimit, static_cast<size_t>(count)}, svArray, trimAll, ignoreEmptyStr);
    }
    size_t SplitStrings(const CVolMem &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        return SplitStrings(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(delimit.GetPtr()), static_cast<size_t>(delimit.GetSize()) / sizeof(CharT)}, svArray, trimAll, ignoreEmptyStr);
    }
    size_t SplitStrings(const PivStringView &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        return SplitStrings(delimit.sv, svArray, trimAll, ignoreEmptyStr);
    }
    size_t SplitStrings(const PivString<CharT> &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        return SplitStrings(piv::basic_string_view<CharT>{delimit.str}, svArray, trimAll, ignoreEmptyStr);
    }
    size_t SplitStrings(const CVolString &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        if (sizeof(CharT) == 2)
        {
            return SplitStrings(reinterpret_cast<const CharT *>(delimit.GetText()), svArray, trimAll, ignoreEmptyStr);
        }
        else
        {
            return SplitStrings(piv::basic_string_view<CharT>{PivW2A(delimit.GetText()).String()}, svArray, trimAll, ignoreEmptyStr);
        }
    }

    /**
     * @brief 分割子文本
     * @param delimit 分割用子文本
     * @param svArray 结果存放数组
     * @param trimAll 是否删除首尾空
     * @param ignoreEmptyStr 是否忽略空白结果
     * @return 所分割出来的结果文本数目
     */
    size_t SplitSubStrings(const piv::basic_string_view<CharT> &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        svArray.clear();
        if (delimit.empty())
        {
            svArray.push_back(sv);
        }
        else
        {
            size_t spos = 0, fpos = 0, epos = sv.size();
            piv::basic_string_view<CharT> str;
            while (fpos < epos)
            {
                fpos = sv.find(delimit, spos);
                if (fpos != piv::basic_string_view<CharT>::npos)
                {
                    str = sv.substr(spos, fpos - spos);
                    spos = fpos + delimit.size();
                }
                else
                {
                    str = sv.substr(spos, sv.size() - spos);
                    fpos = epos;
                }
                if (trimAll)
                {
                    piv::trim_left(str);
                    piv::trim_right(str);
                    if (ignoreEmptyStr && str.empty())
                        continue;
                }
                else if (ignoreEmptyStr)
                {
                    if (str.empty())
                        continue;
                    bool isEmpty = true;
                    for (auto it = str.begin(); it != str.end(); it++)
                    {
                        if (*it > ' ')
                        {
                            isEmpty = false;
                            break;
                        }
                    }
                    if (isEmpty)
                        continue;
                }
                svArray.push_back(str);
            }
        }
        return svArray.size();
    }
    size_t SplitSubStrings(const CharT *delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true, const ptrdiff_t count = -1)
    {
        return (count == -1) ? SplitSubStrings(piv::basic_string_view<CharT>{delimit}, svArray, trimAll, ignoreEmptyStr)
                             : SplitSubStrings(piv::basic_string_view<CharT>{delimit, static_cast<size_t>(count)}, svArray, trimAll, ignoreEmptyStr);
    }
    size_t SplitSubStrings(const CVolMem &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        return SplitSubStrings(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(delimit.GetPtr()), static_cast<size_t>(delimit.GetSize()) / sizeof(CharT)}, svArray, trimAll, ignoreEmptyStr);
    }
    size_t SplitSubStrings(const PivStringView &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        return SplitSubStrings(delimit.sv, svArray, trimAll, ignoreEmptyStr);
    }
    size_t SplitSubStrings(const PivString<CharT> &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        return SplitSubStrings(piv::basic_string_view<CharT>{delimit.str}, svArray, trimAll, ignoreEmptyStr);
    }
    size_t SplitSubStrings(const CVolString &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true)
    {
        if (sizeof(CharT) == 2)
        {
            return SplitSubStrings(reinterpret_cast<const CharT *>(delimit.GetText()), svArray, trimAll, ignoreEmptyStr);
        }
        else
        {
            return SplitSubStrings(piv::basic_string_view<CharT>{PivW2A(delimit.GetText()).String()}, svArray, trimAll, ignoreEmptyStr);
        }
    }

}; // PivStringView

#endif // _PIV_STRING_HPP
