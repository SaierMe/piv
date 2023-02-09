/*********************************************\
 * 火山视窗PIV模块 - 文本辅助                *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 版本: 2023/02/09                          *
\*********************************************/

#ifndef _PIV_STRING_HPP
#define _PIV_STRING_HPP

#include "piv_encoding.hpp"
#include "piv_base.hpp"
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
    size_t ifind(const basic_string_view<CharT> &suc, const basic_string_view<CharT> &des, const size_t pos = 0)
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
    size_t irfind(const basic_string_view<CharT> &suc, const basic_string_view<CharT> &des, const size_t pos = (size_t)-1)
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
    void trim_left(std::basic_string<CharT> &str)
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
    void trim_left(basic_string_view<CharT> &str)
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
    void trim_right(std::basic_string<CharT> &str)
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
    void trim_right(basic_string_view<CharT> &str)
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
    bool iequals(const basic_string_view<CharT> lhs, const basic_string_view<CharT> rhs)
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
    bool iequals(const std::basic_string<CharT> lhs, const std::basic_string<CharT> rhs)
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
 * @brief  文本视图模板类
 * @tparam CharT 字符类型
 * @tparam EncodeType 文本编码,按类型的尺寸区分(1=ANSI;2=UTF-16LE;4=UTF8)
 */
template <typename CharT, typename EncodeType = CharT>
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
    PivStringView(PivStringView &&s) noexcept
    {
        sv = std::move(s.sv);
        pStr.swap(s.pStr);
    }
    PivStringView(const piv::basic_string_view<CharT> &s)
    {
        sv = s;
    }
    PivStringView(piv::basic_string_view<CharT> &&s) noexcept
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
    PivStringView(const std::basic_string<CharT> &s, const bool storeBuf = false)
    {
        this->SetText(s, storeBuf);
    }

    inline operator piv::basic_string_view<CharT> &()
    {
        return sv;
    }
    inline operator const piv::basic_string_view<CharT> &()
    {
        return sv;
    }

    /**
     * @brief 赋值操作符
     * @param other 所欲复制的文本视图类
     * @return 返回自身
     */
    inline PivStringView &operator=(const PivStringView &other)
    {
        sv = other.sv;
        pStr = other.pStr;
        return *this;
    }
    inline PivStringView &operator=(PivStringView &&other) noexcept
    {
        sv = std::move(other.sv);
        pStr.swap(other.pStr);
        return *this;
    }

    /**
     * @brief 比较操作符
     * @param other 所欲比较的文本视图类
     * @return 返回两个文本视图的内容是否相同
     */
    inline bool operator==(const PivStringView &other) const
    {
        return sv == other.sv;
    }

    /**
     * @brief 获取文本视图类中的string_view
     * @return 返回string_view
     */
    inline piv::basic_string_view<CharT> &data()
    {
        return sv;
    }

    /**
     * @brief 获取文本视图类中的string_view指针
     */
    inline piv::basic_string_view<CharT> *pdata() const
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
        if (sizeof(EncodeType) == 2)
            strDump.SetText(reinterpret_cast<const wchar_t *>(sv.data()), sv.length());
        else if (sizeof(EncodeType) == 4)
            PivU2W{reinterpret_cast<const char *>(sv.data()), sv.length()}.GetStr(strDump);
        else
            PivA2W{reinterpret_cast<const char *>(sv.data()), sv.length()}.GetStr(strDump);
        // strDump.SetText(L"文本视图类");
        // CVolMem(reinterpret_cast<const void *>(sv.data()), sv.size()).GetDumpString(strDump, nMaxDumpSize);
    }

    /**
     * @brief 清空视图
     */
    inline PivStringView &Clear()
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
    inline PivStringView &SetText(const CharT *s, const ptrdiff_t count = -1, const bool storeBuf = false)
    {
        if (storeBuf)
        {
            pStr.reset((count == -1) ? new std::basic_string<CharT>{s} : new std::basic_string<CharT>{s, static_cast<size_t>(count)});
            sv = *pStr;
        }
        else
        {
            if (count == -1)
                sv = piv::basic_string_view<CharT>{s};
            else
                sv = piv::basic_string_view<CharT>{s, static_cast<size_t>(count)};
        }
        if (!sv.empty() && sv.back() == '\0')
            sv.remove_suffix(1);
        return *this;
    }
    inline PivStringView &SetText(const ptrdiff_t s, const ptrdiff_t count, const bool storeBuf = false)
    {
        return SetText(reinterpret_cast<const CharT *>(s), count, storeBuf);
    }
    inline PivStringView &SetText(const CVolString &s, const bool storeBuf = false)
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
    inline PivStringView &SetText(const CVolMem &s, const bool storeBuf = false)
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
    inline PivStringView &SetText(const std::basic_string<CharT> &s, const bool storeBuf = false)
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
    inline const CharT *GetText() const
    {
        return sv.data();
    }

    /**
     * @brief 取文本长度
     */
    inline size_t GetLength() const
    {
        return sv.length();
    }

    /**
     * @brief 取文本长度
     */
    inline size_t GetSize() const
    {
        return sv.size();
    }

    /**
     * @brief 取文本哈希值
     * @return
     */
    inline size_t GetHash() const
    {
        return std::hash<piv::basic_string_view<CharT>>{}(sv);
    }

    /**
     * @brief 取引用计数
     */
    inline int32_t UseConut() const
    {
        return pStr.use_count();
    }

    /**
     * @brief 是否为空
     * @return 返回文本视图是否为空
     */
    inline bool IsEmpty() const
    {
        return sv.empty();
    }

    /**
     * @brief 是否相同
     * @param other 所欲比较的文本视图类
     * @return 返回两个文本视图类的内容是否相同
     */
    inline bool IsEqual(const PivStringView &other) const
    {
        return sv == other.sv;
    }

    /**
     * @brief 后移起点
     * @param n 所欲从视图起始移除的字符数
     */
    inline PivStringView &RemovePrefix(const size_t n)
    {
        ASSERT(n <= sv.size()); // 判断移除的字符数是否不大于视图长度
        sv.remove_prefix(n);
        return *this;
    }

    /**
     * @brief 前移终点
     * @param n 所欲从视图终点移除的字符数
     */
    inline PivStringView &RemoveSuffix(const size_t n)
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
    inline size_t CopyStr(CharT *dest, const size_t pos, const size_t count) const
    {
        ASSERT(pos <= sv.size()); // 判断索引位置是否有效
        return sv.copy(dest, count, pos);
    }
    inline size_t CopyStr(ptrdiff_t dest, const size_t pos, const size_t count) const
    {
        return CopyStr(reinterpret_cast<CharT *>(dest), pos, count);
    }
    inline size_t CopyStr(CVolString &dest, const size_t pos, const size_t count) const
    {
        dest.SetLength(count);
        size_t n = CopyStr(const_cast<CharT *>(reinterpret_cast<const CharT *>(dest.GetText())), pos, count);
        if (n == 0)
            dest.Empty();
        else
            dest.RemoveChars(n, count - n);
        return n;
    }
    inline size_t CopyStr(CVolMem &dest, const size_t pos, const size_t count) const
    {
        dest.Alloc(count, TRUE);
        size_t n = CopyStr(const_cast<CharT *>(reinterpret_cast<const CharT *>(dest.GetText())), pos, count);
        if (n == 0)
            dest.Empty();
        else
            dest.Realloc(n);
        return n;
    }
    inline size_t CopyStr(std::basic_string<CharT> &dest, const size_t pos, const size_t count) const
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
    inline CVolMem ToVolMem(const bool null_terminated) const
    {
        return CVolMem(reinterpret_cast<const void *>(sv.data()), (sv.size() + (null_terminated ? 1 : 0)) * sizeof(CharT));
    }

    /**
     * @brief 到文本型
     * @return
     */
    inline CVolString ToVolString() const
    {
        ASSERT(sizeof(CharT) == 2); // 只有UTF-16LE编码的文本视图可以置入火山文本型;
        return CVolString(reinterpret_cast<const wchar_t *>(sv.data()), sv.size());
    }

    /**
     * @brief 到标准文本
     * @return
     */
    inline std::basic_string<CharT> ToString() const
    {
        return std::basic_string<CharT>{sv.data(), sv.size()};
    }

    /**
     * @brief 取字符
     */
    inline const CharT &At(const size_t pos) const
    {
        // 判断索引是否有效和视图是否为空
        ASSERT(pos >= 0 && pos < sv.size() && sv.empty() == false);
        return sv[pos];
    }

    /**
     * @brief 取首字符
     */
    inline const CharT &Front() const
    {
        return sv.empty() ? 0 : sv.front();
    }

    /**
     * @brief 取尾字符
     */
    inline const CharT &Back() const
    {
        return sv.empty() ? 0 : sv.back();
    }

    /**
     * @brief 取文本中间
     * @param pos 起始取出索引位置
     * @param count 欲取出字符的数目
     * @return 返回文本视图类
     */
    inline PivStringView Middle(const size_t pos, const size_t count) const
    {
        ASSERT(pos <= sv.size()); // 判断索引位置是否有效
        return PivStringView{sv.substr(pos, count), pStr};
    }

    /**
     * @brief 取文本左边
     * @param count 欲取出字符的数目
     * @return 返回文本视图类
     */
    inline PivStringView Left(const ptrdiff_t count) const
    {
        return PivStringView{sv.substr(0, static_cast<size_t>(count)), pStr};
    }

    /**
     * @brief 取文本右边
     * @param count 欲取出字符的数目
     * @return 返回文本视图类
     */
    inline PivStringView Right(const ptrdiff_t count) const
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
    inline size_t FindChar(const CharT character, const ptrdiff_t off = 0) const
    {
        if (off < 0 || off >= sv.size())
            return piv::basic_string_view<CharT>::npos;
        return sv.find(character, static_cast<size_t>(off));
    }

    /**
     * @brief 倒找字符
     * @param character 欲寻找的字符
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t ReverseFindChar(const CharT character, const ptrdiff_t off = -1) const
    {
        if (off < 0)
            off = sv.size();
        else if (off >= sv.size())
            return piv::basic_string_view<CharT>::npos;
        return sv.rfind(character, static_cast<size_t>(off));
    }

    /**
     * @brief 寻找字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t FindFirstOf(const piv::basic_string_view<CharT> &chars, const ptrdiff_t pos = 0) const
    {
        return sv.find_first_of(chars, static_cast<size_t>(pos));
    }
    inline size_t FindFirstOf(const CharT *chars, const ptrdiff_t pos = 0, const ptrdiff_t count = -1) const
    {
        return (count == -1) ? FindFirstOf(piv::basic_string_view<CharT>{chars}, pos)
                             : FindFirstOf(piv::basic_string_view<CharT>{chars, static_cast<size_t>(count)}, pos);
    }
    inline size_t FindFirstOf(const CVolMem &chars, const ptrdiff_t pos = 0) const
    {
        return FindFirstOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(chars.GetPtr()), static_cast<size_t>(chars.GetSize()) / sizeof(CharT)}, pos);
    }
    inline size_t FindFirstOf(const PivStringView &chars, const ptrdiff_t pos = 0) const
    {
        return FindFirstOf(chars.sv, pos);
    }
    inline size_t FindFirstOf(const CVolString &chars, const ptrdiff_t pos = 0) const
    {
        if (sizeof(EncodeType) == 2)
            return FindFirstOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        else if (sizeof(EncodeType) == 4)
            return FindFirstOf(reinterpret_cast<const CharT *>(PivW2U{chars}.GetText()), pos);
        else
            return FindFirstOf(reinterpret_cast<const CharT *>(PivW2A{chars}.GetText()), pos);
    }

    /**
     * @brief 倒找字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t FindLastOf(const piv::basic_string_view<CharT> &chars, const ptrdiff_t pos = -1) const
    {
        size_t p = (pos == -1) ? sv.size() : static_cast<size_t>(pos);
        return sv.find_last_of(chars, p);
    }
    inline size_t FindLastOf(const CharT *chars, const ptrdiff_t pos = -1, const ptrdiff_t count = -1) const
    {
        return (count == -1) ? FindLastOf(piv::basic_string_view<CharT>{chars}, pos)
                             : FindLastOf(piv::basic_string_view<CharT>{chars, static_cast<size_t>(count)}, pos);
    }
    inline size_t FindLastOf(const CVolMem &chars, const ptrdiff_t pos = -1) const
    {
        return FindLastOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(chars.GetPtr()), static_cast<size_t>(chars.GetSize()) / sizeof(CharT)}, pos);
    }
    inline size_t FindLastOf(const PivStringView &chars, const ptrdiff_t pos = -1) const
    {
        return FindLastOf(chars.sv, pos);
    }
    inline size_t FindLastOf(const CVolString &chars, const ptrdiff_t pos = -1) const
    {
        if (sizeof(EncodeType) == 2)
            return FindLastOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        else if (sizeof(EncodeType) == 4)
            return FindLastOf(reinterpret_cast<const CharT *>(PivW2U{chars}.GetText()), pos);
        else
            return FindLastOf(reinterpret_cast<const CharT *>(PivW2A{chars}.GetText()), pos);
    }

    /**
     * @brief 寻找不符合字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t FindFirstNotOf(const piv::basic_string_view<CharT> &chars, const ptrdiff_t pos = 0) const
    {
        return sv.find_first_not_of(chars, static_cast<size_t>(pos));
    }
    inline size_t FindFirstNotOf(const CharT *chars, const ptrdiff_t pos = 0, const ptrdiff_t count = -1) const
    {
        return (count == -1) ? FindFirstNotOf(piv::basic_string_view<CharT>{chars}, pos)
                             : FindFirstNotOf(piv::basic_string_view<CharT>{chars, static_cast<size_t>(count)}, pos);
    }
    inline size_t FindFirstNotOf(const CVolMem &chars, const ptrdiff_t pos = 0) const
    {
        return FindFirstNotOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(chars.GetPtr()), static_cast<size_t>(chars.GetSize()) / sizeof(CharT)}, pos);
    }
    inline size_t FindFirstNotOf(const PivStringView &chars, const ptrdiff_t pos = 0) const
    {
        return FindFirstNotOf(chars.sv, pos);
    }
    inline size_t FindFirstNotOf(const CVolString &chars, const ptrdiff_t pos = 0) const
    {
        if (sizeof(EncodeType) == 2)
            return FindFirstNotOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        else if (sizeof(EncodeType) == 4)
            return FindFirstNotOf(reinterpret_cast<const CharT *>(PivW2U{chars}.GetText()), pos);
        else
            return FindFirstNotOf(reinterpret_cast<const CharT *>(PivW2A{chars}.GetText()), pos);
    }

    /**
     * @brief 倒找字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t FindLastNotOf(const piv::basic_string_view<CharT> &chars, const ptrdiff_t pos = -1) const
    {
        size_t p = (pos == -1) ? sv.size() : static_cast<size_t>(pos);
        return sv.find_last_not_of(chars, p);
    }
    inline size_t FindLastNotOf(const CharT *chars, const ptrdiff_t pos = -1, const ptrdiff_t count = -1) const
    {
        return (count == -1) ? FindLastNotOf(piv::basic_string_view<CharT>{chars}, pos)
                             : FindLastNotOf(piv::basic_string_view<CharT>{chars, static_cast<size_t>(count)}, pos);
    }
    inline size_t FindLastNotOf(const CVolMem &chars, const ptrdiff_t pos = -1) const
    {
        return FindLastNotOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(chars.GetPtr()), static_cast<size_t>(chars.GetSize()) / sizeof(CharT)}, pos);
    }
    inline size_t FindLastNotOf(const PivStringView &chars, const ptrdiff_t pos = -1) const
    {
        return FindLastNotOf(chars.sv, pos);
    }
    inline size_t FindLastNotOf(const CVolString &chars, const ptrdiff_t pos = -1) const
    {
        if (sizeof(EncodeType) == 2)
            return FindLastNotOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        else if (sizeof(EncodeType) == 4)
            return FindLastNotOf(reinterpret_cast<const CharT *>(PivW2U{chars}.GetText()), pos);
        else
            return FindLastNotOf(reinterpret_cast<const CharT *>(PivW2A{chars}.GetText()), pos);
    }

    /**
     * @brief 寻找文本
     * @param text 欲寻找的文本
     * @param pos 起始搜寻位置
     * @param case_insensitive 是否不区分大小写
     * @param count 欲寻找文本的长度
     * @return 寻找到的位置
     */
    inline size_t SearchText(const piv::basic_string_view<CharT> &text, const ptrdiff_t pos = 0, const bool case_insensitive = false) const
    {
        if (!case_insensitive)
            return sv.find(text, static_cast<size_t>(pos));
        else
            return piv::ifind(sv, text, pos);
    }
    inline size_t SearchText(const std::basic_string<CharT> &text, const ptrdiff_t pos = 0, const bool case_insensitive = false) const
    {
        if (!case_insensitive)
            return str.find(text, static_cast<size_t>(pos));
        else
            return piv::ifind(piv::basic_string_view<CharT>{str}, piv::basic_string_view<CharT>{text}, pos);
    }
    inline size_t SearchText(const CharT *text, const ptrdiff_t pos = 0, const bool case_insensitive = false, const ptrdiff_t count = -1) const
    {
        return SearchText((count == -1) ? piv::basic_string_view<CharT>{text} : piv::basic_string_view<CharT>{text, static_cast<size_t>(count)}, pos, case_insensitive);
    }
    inline size_t SearchText(const CVolMem &text, const ptrdiff_t pos = 0, const bool case_insensitive = false) const
    {
        return SearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetPtr()), static_cast<size_t>(text.GetSize()) / sizeof(CharT)}, pos, case_insensitive);
    }
    inline size_t SearchText(const CVolString &text, const ptrdiff_t pos = 0, const bool case_insensitive = false) const
    {
        if (sizeof(EncodeType) == 2)
            return SearchText(reinterpret_cast<const CharT *>(text.GetText()), pos, case_insensitive);
        else if (sizeof(EncodeType) == 4)
            return SearchText(reinterpret_cast<const CharT *>(PivW2U{text}.GetText()), pos, case_insensitive);
        else
            return SearchText(reinterpret_cast<const CharT *>(PivW2A{text}.GetText()), pos, case_insensitive);
    }
    inline size_t SearchText(const PivStringView &text, const ptrdiff_t pos = 0, const bool case_insensitive = false) const
    {
        return SearchText(text.sv, pos, case_insensitive);
    }

    /**
     * @brief 倒找文本
     * @param text 欲寻找的文本
     * @param pos 起始搜寻位置
     * @param count 欲寻找文本的长度
     * @param case_insensitive 是否不区分大小写
     * @return 寻找到的位置
     */
    inline size_t ReverseSearchText(const piv::basic_string_view<CharT> &text, const ptrdiff_t pos = -1, const bool case_insensitive = false) const
    {
        size_t p = (pos == -1) ? sv.size() : static_cast<size_t>(pos);
        if (!case_insensitive)
            return sv.rfind(text, p);
        else
            return piv::irfind(sv, text, p);
    }
    inline size_t ReverseSearchText(const std::basic_string<CharT> &text, const ptrdiff_t pos = -1, const bool case_insensitive = false) const
    {
        return ReverseSearchText(piv::basic_string_view<CharT>{text}, pos, case_insensitive);
    }
    inline size_t ReverseSearchText(const CharT *text, const ptrdiff_t pos = -1, const bool case_insensitive = false, const ptrdiff_t count = -1) const
    {
        return ReverseSearchText((count == -1) ? piv::basic_string_view<CharT>{text} : piv::basic_string_view<CharT>{text, static_cast<size_t>(count)}, pos, case_insensitive);
    }
    inline size_t ReverseSearchText(const CVolMem &text, const ptrdiff_t pos = -1, const bool case_insensitive = false) const
    {
        return ReverseSearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetPtr()), static_cast<size_t>(text.GetSize()) / sizeof(CharT)}, pos, case_insensitive);
    }
    inline size_t ReverseSearchText(const CVolString &text, const ptrdiff_t pos = -1, const bool case_insensitive = false) const
    {
        if (sizeof(EncodeType) == 2)
            return ReverseSearchText(reinterpret_cast<const CharT *>(text.GetText()), pos, case_insensitive);
        else if (sizeof(EncodeType) == 4)
            return ReverseSearchText(reinterpret_cast<const CharT *>(PivW2U{text}.GetText()), pos, case_insensitive);
        else
            return ReverseSearchText(reinterpret_cast<const CharT *>(PivW2A{text}.GetText()), pos, case_insensitive);
    }
    inline size_t ReverseSearchText(const PivStringView &text, const ptrdiff_t pos = -1, const bool case_insensitive = false) const
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
    inline bool LeadOf(const CharT ch, const bool case_sensitive = true) const
    {
        if (sv.empty())
            return false;
        if (case_sensitive)
            return (sv.front() == ch);
        else
            return (std::tolower(static_cast<unsigned char>(sv.front())) == std::tolower(static_cast<unsigned char>(ch)));
    }
    inline bool LeadOf(const piv::basic_string_view<CharT> &s, const bool case_sensitive = true) const
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }
    inline bool LeadOf(const CharT *s, const bool case_sensitive = true, const size_t count = (size_t)-1) const
    {
        return (SearchText(s, 0, !case_sensitive, count) == 0);
    }
    inline bool LeadOf(const std::basic_string<CharT> &s, const bool case_sensitive = true) const
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }
    inline bool LeadOf(const CVolMem &s, const bool case_sensitive = true) const
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }
    inline bool LeadOf(const PivStringView &s, const bool case_sensitive = true) const
    {
        return (SearchText(s.sv, 0, !case_sensitive) == 0);
    }
    inline bool LeadOf(const CVolString &s, const bool case_sensitive = true) const
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
    inline bool EndOf(const CharT ch, const bool case_sensitive = true) const
    {
        if (sv.empty())
            return false;
        if (case_sensitive)
            return (sv.back() == ch);
        else
            return (std::tolower(static_cast<unsigned char>(sv.back())) == std::tolower(static_cast<unsigned char>(ch)));
    }
    inline bool EndOf(const piv::basic_string_view<CharT> &s, const bool case_sensitive = true) const
    {
        return (ReverseSearchText(s, -1, !case_sensitive) == (sv.size() - s.size()));
    }
    inline bool EndOf(const CharT *s, const bool case_sensitive = true, const size_t count = (size_t)-1)
    {
        return EndOf((count == -1) ? piv::basic_string_view<CharT>{s} : piv::basic_string_view<CharT>{s, static_cast<size_t>(count)}, case_sensitive);
    }
    inline bool EndOf(const std::basic_string<CharT> &s, const bool case_sensitive = true) const
    {
        return EndOf(piv::basic_string_view<CharT>{s}, case_sensitive);
    }
    inline bool EndOf(const CVolMem &s, const bool case_sensitive = true) const
    {
        return EndOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT)}, case_sensitive);
    }
    inline bool EndOf(const PivStringView &s, const bool case_sensitive = true) const
    {
        return EndOf(s.sv, case_sensitive);
    }
    inline bool EndOf(const CVolString &s, const bool case_sensitive = true) const
    {
        if (sizeof(EncodeType) == 2)
            return EndOf(reinterpret_cast<const CharT *>(s.GetText()), case_sensitive);
        else if (sizeof(EncodeType) == 4)
            return EndOf(reinterpret_cast<const CharT *>(PivW2U{s}.GetText()), case_sensitive);
        else
            return EndOf(reinterpret_cast<const CharT *>(PivW2A{s}.GetText()), case_sensitive);
    }

    /**
     * @brief 删首空
     */
    inline PivStringView &TrimLeft()
    {
        piv::trim_left(sv);
        return *this;
    }

    /**
     * @brief 删尾空
     */
    inline PivStringView &TrimRight()
    {
        piv::trim_right(sv);
        return *this;
    }

    /**
     * @brief 删首尾空
     */
    inline PivStringView &TrimAll()
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
    size_t SplitStrings(const piv::basic_string_view<CharT> &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true) const
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
    inline size_t SplitStrings(const CharT *delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true, const ptrdiff_t count = -1) const
    {
        return (count == -1) ? SplitStrings(piv::basic_string_view<CharT>{delimit}, svArray, trimAll, ignoreEmptyStr)
                             : SplitStrings(piv::basic_string_view<CharT>{delimit, static_cast<size_t>(count)}, svArray, trimAll, ignoreEmptyStr);
    }
    inline size_t SplitStrings(const CVolMem &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true) const
    {
        return SplitStrings(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(delimit.GetPtr()), static_cast<size_t>(delimit.GetSize()) / sizeof(CharT)}, svArray, trimAll, ignoreEmptyStr);
    }
    inline size_t SplitStrings(const PivStringView &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true) const
    {
        return SplitStrings(delimit.sv, svArray, trimAll, ignoreEmptyStr);
    }
    inline size_t SplitStrings(const CVolString &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true) const
    {
        if (sizeof(EncodeType) == 2)
            return SplitStrings(reinterpret_cast<const CharT *>(delimit.GetText()), svArray, trimAll, ignoreEmptyStr);
        else if (sizeof(EncodeType) == 4)
            return SplitStrings(reinterpret_cast<const CharT *>(PivW2U{delimit}.GetText()), svArray, trimAll, ignoreEmptyStr);
        else
            return SplitStrings(reinterpret_cast<const CharT *>(PivW2A{delimit}.GetText()), svArray, trimAll, ignoreEmptyStr);
    }

    /**
     * @brief 分割子文本
     * @param delimit 分割用子文本
     * @param svArray 结果存放数组
     * @param trimAll 是否删除首尾空
     * @param ignoreEmptyStr 是否忽略空白结果
     * @return 所分割出来的结果文本数目
     */
    size_t SplitSubStrings(const piv::basic_string_view<CharT> &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true) const
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
    inline size_t SplitSubStrings(const CharT *delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true, const ptrdiff_t count = -1) const
    {
        return (count == -1) ? SplitSubStrings(piv::basic_string_view<CharT>{delimit}, svArray, trimAll, ignoreEmptyStr)
                             : SplitSubStrings(piv::basic_string_view<CharT>{delimit, static_cast<size_t>(count)}, svArray, trimAll, ignoreEmptyStr);
    }
    inline size_t SplitSubStrings(const CVolMem &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true) const
    {
        return SplitSubStrings(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(delimit.GetPtr()), static_cast<size_t>(delimit.GetSize()) / sizeof(CharT)}, svArray, trimAll, ignoreEmptyStr);
    }
    inline size_t SplitSubStrings(const PivStringView &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true) const
    {
        return SplitSubStrings(delimit.sv, svArray, trimAll, ignoreEmptyStr);
    }
    inline size_t SplitSubStrings(const CVolString &delimit, std::vector<piv::basic_string_view<CharT>> &svArray, const bool trimAll = true, const bool ignoreEmptyStr = true) const
    {
        if (sizeof(EncodeType) == 2)
            return SplitSubStrings(reinterpret_cast<const CharT *>(delimit.GetText()), svArray, trimAll, ignoreEmptyStr);
        else if (sizeof(EncodeType) == 4)
            return SplitSubStrings(reinterpret_cast<const CharT *>(PivW2U{delimit}.GetText()), svArray, trimAll, ignoreEmptyStr);
        else
            return SplitSubStrings(reinterpret_cast<const CharT *>(PivW2A{delimit}.GetText()), svArray, trimAll, ignoreEmptyStr);
    }

    /**
     * @brief URL编码
     * @param utf8
     * @return
     */
    inline std::basic_string<CharT> UrlEncode(const bool utf8 = true) const
    {
        return piv::encoding::UrlStrEncode(str, utf8);
    }

    /**
     * @brief URL解码
     * @param utf8 是否为UTF-8编码
     * @return
     */
    inline std::basic_string<CharT> UrlDecode(const bool utf8 = true) const
    {
        return piv::encoding::UrlStrDecode(str, true, utf8);
    }

    /**
     * @brief BASE64编码
     * @return
     */
    inline std::basic_string<CharT> EncodeBase64(const int32_t line_len = 0) const
    {
        return piv::base64<CharT>{}.encode(sv, line_len);
    }

    /**
     * @brief BASE64解码
     * @return
     */
    inline std::basic_string<CharT> DecodeBase64() const
    {
        return piv::base64<CharT>{}.decode(sv);
    }

    /**
     * @brief BASE64到字节集
     * @return
     */
    inline CVolMem DecodeBase64Bin() const
    {
        CVolMem buffer;
        piv::base64<CharT>{}.decode(sv, buffer);
        return buffer;
    }

    /**
     * @brief BASE85编码
     * @return
     */
    inline std::basic_string<CharT> EncodeBase85(const bool padding = false) const
    {
        return piv::base85<CharT>{}.encode(sv, padding);
    }

    /**
     * @brief BASE85解码
     * @return
     */
    inline std::basic_string<CharT> DecodeBase85() const
    {
        return piv::base85<CharT>{}.decode(sv);
    }

    /**
     * @brief BASE85到字节集
     * @return
     */
    inline CVolMem DecodeBase85Bin() const
    {
        CVolMem buffer;
        piv::base85<CharT>{}.decode(sv, buffer);
        return buffer;
    }

    /**
     * @brief BASE91编码
     * @return
     */
    inline std::basic_string<CharT> EncodeBase91() const
    {
        return piv::base91<CharT>{}.encode(sv);
    }

    /**
     * @brief BASE91解码
     * @return
     */
    inline std::basic_string<CharT> DecodeBase91() const
    {
        return piv::base91<CharT>{}.decode(sv);
    }

    /**
     * @brief BASE64到字节集
     * @return
     */
    inline CVolMem DecodeBase91Bin() const
    {
        CVolMem buffer;
        piv::base91<CharT>{}.decode(sv, buffer);
        return buffer;
    }

}; // PivStringView

/*********************************************
 * @brief  标准文本模板类
 * @tparam CharT 字符类型
 * @tparam EncodeType 文本编码,按类型的尺寸区分(1=ANSI;2=UTF-16LE;4=UTF8)
 */
template <typename CharT, typename EncodeType = CharT>
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
    PivString(PivString &&s) noexcept
    {
        str = std::move(s.str);
    }
    PivString(const std::basic_string<CharT> &s)
    {
        str = s;
    }
    PivString(std::basic_string<CharT> &&s) noexcept
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
        if (sizeof(EncodeType) == 2)
            str.assign(reinterpret_cast<const CharT *>(s.GetText()));
        else if (sizeof(EncodeType) == 4)
            str.assign(reinterpret_cast<const CharT *>(PivW2U{s}.GetText()));
        else if (sizeof(EncodeType) == 1)
            str.assign(reinterpret_cast<const CharT *>(PivW2A{s}.GetText()));
    }
    PivString(const CVolMem &s)
    {
        str.assign(reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT));
    }

    inline operator std::basic_string<CharT> &()
    {
        return str;
    }
    inline operator const std::basic_string<CharT> &()
    {
        return str;
    }

    /**
     * @brief 赋值操作符
     * @param other 所欲复制的标准文本类
     * @return 返回自身
     */
    inline PivString &operator=(const PivString &other)
    {
        str = other.str;
        return *this;
    }
    inline PivString &operator=(PivString &&other) noexcept
    {
        str = std::move(other.str);
        return *this;
    }
    inline PivString &operator=(const CharT &s)
    {
        str = s;
        return *this;
    }
    inline PivString &operator=(const std::basic_string<CharT> &s)
    {
        str = s;
        return *this;
    }
    inline PivString &operator=(std::basic_string<CharT> &&s) noexcept
    {
        str = std::move(s);
        return *this;
    }

    /**
     * @brief 比较操作符
     * @param other 所欲比较的标准文本类
     * @return 返回两个文本的内容是否相同
     */
    inline bool operator==(const PivString &other) const
    {
        return str == other.str;
    }
    inline bool operator==(const std::basic_string<CharT> &other) const
    {
        return str == other;
    }

    /**
     * @brief 获取文本类中的string
     * @return 返回string
     */
    inline std::basic_string<CharT> &data()
    {
        return str;
    }

    /**
     * @brief 获取文本类中的string指针
     */
    inline std::basic_string<CharT> *pdata() const
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
        if (sizeof(EncodeType) == 2)
            strDump.SetText(reinterpret_cast<const wchar_t *>(str.c_str()));
        else if (sizeof(EncodeType) == 4)
            PivU2W{reinterpret_cast<const char *>(str.c_str())}.GetStr(strDump);
        else
            PivA2W{reinterpret_cast<const char *>(str.c_str())}.GetStr(strDump);
        // strDump.SetText(L"标准文本类");
        // CVolMem(reinterpret_cast<const void *>(str.data()), str.size()).GetDumpString(strDump, nMaxDumpSize);
    }

    /**
     * @brief 取文本指针
     * @return 返回文本指针
     */
    inline const CharT *GetText() const
    {
        return str.c_str();
    }

    /**
     * @brief 取文本长度
     */
    inline size_t GetLength() const
    {
        return str.length();
    }

    /**
     * @brief 取文本长度
     */
    inline size_t GetSize() const
    {
        return str.size();
    }

    /**
     * @brief 取文本哈希值
     * @param case_insensitive 是否不区分大小写
     * @return
     */
    inline size_t GetHash(bool case_insensitive = false) const
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
    inline bool IsEmpty() const
    {
        return str.empty();
    }

    /**
     * @brief 是否相同
     * @param other 所欲比较的标准文本类
     * @return 返回两个文本类的内容是否相同
     */
    inline bool IsEqual(const PivString &other) const
    {
        return str == other.str;
    }

    /**
     * @brief 文本比较
     * @param s 所欲比较的文本
     * @return
     */
    inline int32_t Compare(const CharT *s) const
    {
        return str.compare(s);
    }
    inline int32_t Compare(const std::basic_string<CharT> &s) const
    {
        return str.compare(s);
    }
    inline int32_t Compare(const piv::basic_string_view<CharT> &s) const
    {
        return str.compare(0, str.size(), s.data());
    }
    inline int32_t Compare(const CVolMem &s) const
    {
        return str.compare(0, str.size(), reinterpret_cast<const CharT *>(s.GetPtr()));
    }
    inline int32_t Compare(const CVolString &s) const
    {
        if (sizeof(EncodeType) == 2)
            return str.compare(0, str.size(), reinterpret_cast<const CharT *>(s.GetText()));
        else if (sizeof(EncodeType) == 4)
            return str.compare(reinterpret_cast<const CharT *>(PivW2U{s}.GetText()));
        else
            return str.compare(reinterpret_cast<const CharT *>(PivW2A{s}.GetText()));
    }

    /**
     * @brief 清空文本
     */
    inline PivString &Clear()
    {
        str.clear();
        return *this;
    }

    /**
     * @brief 填充文本
     * @param count 重复的字符数量
     * @param ch 所欲填充的字符
     */
    inline PivString &InitWithChars(const ptrdiff_t count, const CharT ch)
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
    inline PivString &SetText(const CharT *s, ptrdiff_t count = -1)
    {
        if (count == -1)
            str.assign(s);
        else
            str.assign(s, static_cast<size_t>(count));
        if (!str.empty() && str.back() == '\0')
            str.pop_back();
        return *this;
    }
    inline PivString &SetText(const ptrdiff_t s, const ptrdiff_t count)
    {
        return SetText(reinterpret_cast<const CharT *>(s), count);
    }
    inline PivString &SetText(const CVolString &s, const ptrdiff_t count = -1)
    {
        if (sizeof(EncodeType) == 2)
        {
            str.assign(reinterpret_cast<const CharT *>(s.GetText()), (count == -1) ? static_cast<size_t>(s.GetLength()) : static_cast<size_t>(count));
        }
        else if (sizeof(EncodeType) == 4)
        {
            PivW2U utf8{s};
            str.assign(reinterpret_cast<const CharT *>(utf8.GetText()), (count == -1) ? utf8.GetSize() : static_cast<size_t>(count));
        }
        else if (sizeof(EncodeType) == 1)
        {
            PivW2A ansi{s};
            str.assign(reinterpret_cast<const CharT *>(ansi.GetText()), (count == -1) ? ansi.GetSize() : static_cast<size_t>(count));
        }
        return *this;
    }
    inline PivString &SetText(const CVolMem &s, const ptrdiff_t count = -1)
    {
        str.assign(reinterpret_cast<const CharT *>(s.GetPtr()),
                   (count == -1) ? static_cast<size_t>(s.GetSize()) / sizeof(CharT) : static_cast<size_t>(count));
        return *this;
    }
    inline PivString &SetText(const piv::basic_string_view<CharT> &s, const ptrdiff_t count = -1)
    {
        str.assign(s.data(), (count == -1) ? s.size() : static_cast<size_t>(count));
        return *this;
    }
    inline PivString &SetText(const int64_t n)
    {
        if (sizeof(CharT) == 2)
            str.assign(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.assign(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    inline PivString &SetText(const int32_t n)
    {
        if (sizeof(CharT) == 2)
            str.assign(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.assign(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    inline PivString &SetText(const wchar_t n)
    {
        if (sizeof(CharT) == 2)
            str.assign(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.assign(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    inline PivString &SetText(const int16_t n)
    {
        if (sizeof(CharT) == 2)
            str.assign(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.assign(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    inline PivString &SetText(const int8_t n)
    {
        if (sizeof(CharT) == 2)
            str.assign(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.assign(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    inline PivString &SetText(const double n)
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
     * @param ReadEncodeType 所欲读取文本编码
     * @return
     */
    bool ReadFromFile(const wchar_t *FileName, const int32_t ReadDataSize = -1, const VOL_STRING_ENCODE_TYPE ReadEncodeType = VSET_UTF_16)
    {
        ASSERT(ReadDataSize >= -1);
        ASSERT_R_STR(FileName);
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
            if (sizeof(EncodeType) == 2)
            {
                str.resize(fileSize / sizeof(CharT));
                Succeeded = (fread(str.data(), sizeof(CharT), fileSize / sizeof(CharT), in) == fileSize / sizeof(CharT));
            }
            else
            {
                char *data = new char[fileSize];
                Succeeded = (fread(data, 1, fileSize, in) == fileSize);
                if (sizeof(EncodeType) == 4)
                {
                    PivU2W utf{reinterpret_cast<const char *>(data), fileSize};
                    str.assign(reinterpret_cast<const CharT *>(utf.GetText()), utf.GetSize());
                }
                else if (sizeof(EncodeType) == 1)
                {
                    PivA2W utf{reinterpret_cast<const char *>(data), fileSize};
                    str.assign(reinterpret_cast<const CharT *>(utf.GetText()), utf.GetSize());
                }
                delete[] data;
            }
        }
        else if (ReadEncodeType == VSET_UTF_8)
        {
            if (sizeof(EncodeType) == 4)
            {
                str.resize(fileSize / sizeof(CharT));
                Succeeded = (fread(str.data(), sizeof(CharT), fileSize / sizeof(CharT), in) == fileSize / sizeof(CharT));
            }
            else
            {
                char *data = new char[fileSize];
                Succeeded = (fread(data, 1, fileSize, in) == fileSize);
                if (sizeof(EncodeType) == 2)
                {
                    PivW2U utf{reinterpret_cast<const wchar_t *>(data), fileSize / 2};
                    str.assign(reinterpret_cast<const CharT *>(utf.GetText()), utf.GetSize());
                }
                else if (sizeof(EncodeType) == 1)
                {
                    PivA2U utf{reinterpret_cast<const char *>(data), fileSize};
                    str.assign(reinterpret_cast<const CharT *>(utf.GetText()), utf.GetSize());
                }
                delete[] data;
            }
        }
        else if (ReadEncodeType == VSET_MBCS)
        {
            if (sizeof(EncodeType) == 1)
            {
                str.resize(fileSize / sizeof(CharT));
                Succeeded = (fread(str.data(), sizeof(CharT), fileSize / sizeof(CharT), in) == fileSize / sizeof(CharT));
            }
            else
            {
                char *data = new char[fileSize];
                Succeeded = (fread(data, 1, fileSize, in) == fileSize);
                if (sizoef(EncodeType) == 2)
                {
                    PivW2A mbcs{reinterpret_cast<const wchar_t *>(data), fileSize / 2};
                    str.assign(reinterpret_cast<const CharT *>(mbcs.GetText()), mbcs.GetSize());
                }
                else if (sizeof(EncodeType) == 4)
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
     * @return
     */
    bool WriteIntoFile(const wchar_t *FileName, const int32_t WriteDataSize = -1, const VOL_STRING_ENCODE_TYPE WriteEncodeType = VSET_UTF_16)
    {
        ASSERT(WriteDataSize >= -1);
        ASSERT_R_STR(FileName);
        ASSERT(WriteEncodeType != VSET_UNKNOWN);
        FILE *out = _wfopen(FileName, L"wb");
        if (out == NULL)
            return false;
        bool Succeeded = false;
        size_t count = (WriteDataSize == -1) ? str.size() : ((static_cast<size_t>(WriteDataSize) > str.size()) ? str.size() : static_cast<size_t>(WriteDataSize));
        if (WriteEncodeType == VSET_UTF_16)
        {
            const byte bom[] = {0xFF, 0xFE};
            fwrite(bom, 1, 2, out);
            if (sizeof(EncodeType) == 2)
            {
                Succeeded = (fwrite(str.data(), sizeof(CharT), count, out) == count);
            }
            else if (sizeof(EncodeType) == 4)
            {
                PivU2W utf{reinterpret_cast<const char *>(str.c_str()), count};
                Succeeded = (fwrite(utf.GetText(), 2, utf.GetSize(), out) == utf.GetSize());
            }
            else if (sizeof(EncodeType) == 1)
            {
                PivA2W utf{reinterpret_cast<const char *>(str.c_str()), count};
                Succeeded = (fwrite(utf.GetText(), 2, utf.GetSize(), out) == utf.GetSize());
            }
        }
        else if (WriteEncodeType == VSET_UTF_8)
        {
            const byte bom[] = {0xEF, 0xBB, 0xBF};
            fwrite(bom, 1, 3, out);
            if (sizeof(EncodeType) == 2)
            {
                PivW2U utf{reinterpret_cast<const wchar_t *>(str.c_str()), count};
                Succeeded = (fwrite(utf.GetText(), 1, utf.GetSize(), out) == utf.GetSize());
            }
            else if (sizeof(EncodeType) == 4)
            {
                Succeeded = (fwrite(str.data(), sizeof(CharT), count, out) == count);
            }
            else if (sizeof(EncodeType) == 1)
            {
                PivA2U utf{reinterpret_cast<const char *>(str.c_str()), count};
                Succeeded = (fwrite(utf.GetText(), 1, utf.GetSize(), out) == utf.GetSize());
            }
        }
        else if (WriteEncodeType == VSET_MBCS)
        {
            if (sizeof(EncodeType) == 2)
            {
                PivW2A mbcs{reinterpret_cast<const wchar_t *>(str.c_str()), count};
                Succeeded = (fwrite(mbcs.GetText(), 1, mbcs.GetSize(), out) == mbcs.GetSize());
            }
            else if (sizeof(EncodeType) == 4)
            {
                PivU2A mbcs{reinterpret_cast<const char *>(str.c_str()), count};
                Succeeded = (fwrite(mbcs.GetText(), 1, mbcs.GetSize(), out) == mbcs.GetSize());
            }
            else if (sizeof(EncodeType) == 1)
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
    inline PivString &SetLength(const ptrdiff_t length)
    {
        str.resize(static_cast<size_t>, ' ');
        return *this;
    }

    /**
     * @brief 加入字符
     * @param ch 所欲添加的字符
     * @return
     */
    inline PivString &AddChar(const CharT ch)
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
    inline PivString &AddManyChar(const ptrdiff_t count, const CharT ch)
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
    inline PivString &AddText(const CharT *s)
    {
        str.append(s);
        return *this;
    }
    inline PivString &AddText(const CVolString &s)
    {
        if (sizeof(EncodeType) == 2)
        {
            str.append(reinterpret_cast<const CharT *>(s.GetText()));
        }
        else if (sizeof(EncodeType) == 4)
        {
            PivW2U utf8{s};
            str.append(reinterpret_cast<const CharT *>(utf8.GetText()));
        }
        else if (sizeof(EncodeType) == 1)
        {
            PivW2A ansi{s};
            str.append(reinterpret_cast<const CharT *>(ansi.GetText()));
        }
        return *this;
    }
    inline PivString &AddText(const CVolConstString &s)
    {
        if (sizeof(EncodeType) == 2)
        {
            str.append(reinterpret_cast<const CharT *>(s.GetText()));
        }
        else if (sizeof(EncodeType) == 4)
        {
            PivW2U utf8{s};
            str.append(reinterpret_cast<const CharT *>(utf8.GetText()));
        }
        else if (sizeof(EncodeType) == 1)
        {
            PivW2A ansi{s};
            str.append(reinterpret_cast<const CharT *>(ansi.GetText()));
        }
        return *this;
    }
    inline PivString &AddText(const CVolMem &s)
    {
        str.append(reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT));
        return *this;
    }
    inline PivString &AddText(const std::basic_string<CharT> &s)
    {
        str.append(s);
        return *this;
    }
    inline PivString &AddText(const piv::basic_string_view<CharT> &s)
    {
        str.append(s.data(), s.size());
        return *this;
    }
    inline PivString &AddText(const PivStringView<CharT, EncodeType> &s)
    {
        str.append(s.data().data(), s.data().size());
        return *this;
    }
    inline PivString &AddText(const PivString &s)
    {
        str.append(s.str);
        return *this;
    }
    inline PivString &AddText(const int64_t n)
    {
        if (sizeof(CharT) == 2)
            str.append(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.append(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    inline PivString &AddText(const int32_t n)
    {
        if (sizeof(CharT) == 2)
            str.append(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.append(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    inline PivString &AddText(const wchar_t n)
    {
        if (sizeof(CharT) == 2)
            str.append(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.append(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    inline PivString &AddText(const int16_t n)
    {
        if (sizeof(CharT) == 2)
            str.append(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.append(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    inline PivString &AddText(const int8_t n)
    {
        if (sizeof(CharT) == 2)
            str.append(reinterpret_cast<const CharT *>(std::to_wstring(n).c_str()));
        else
            str.append(reinterpret_cast<const CharT *>(std::to_string(n).c_str()));
        return *this;
    }
    inline PivString &AddText(const double n)
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
    inline PivString &Append(const CharT *s, const ptrdiff_t count = -1)
    {
        if (count == -1)
            str.append(s);
        else
            str.append(s, static_cast<size_t>(count));
        return *this;
    }
    inline PivString &Append(const CVolString &s, const ptrdiff_t count = -1)
    {
        if (sizeof(EncodeType) == 2)
        {
            str.append(reinterpret_cast<const CharT *>(s.GetText()), (count == -1) ? static_cast<size_t>(s.GetLength()) : static_cast<size_t>(count));
        }
        else if (sizeof(EncodeType) == 4)
        {
            PivW2U utf8{s};
            str.append(reinterpret_cast<const CharT *>(utf8.GetText()), (count == -1) ? utf8.GetSize() : static_cast<size_t>(count));
        }
        else if (sizeof(EncodeType) == 1)
        {
            PivW2A ansi{s};
            str.append(reinterpret_cast<const CharT *>(ansi.GetText()), (count == -1) ? ansi.GetSize() : static_cast<size_t>(count));
        }
        return *this;
    }
    inline PivString &Append(const CVolMem &s, const ptrdiff_t count = -1)
    {
        str.append(reinterpret_cast<const CharT *>(s.GetPtr()),
                   (count == -1) ? static_cast<size_t>(s.GetSize()) / sizeof(CharT) : static_cast<size_t>(count));
        return *this;
    }
    inline PivString &Append(const std::basic_string<CharT> &s, const ptrdiff_t count = -1)
    {
        str.append(s, 0, (count == -1) ? s.size() : static_cast<size_t>(count));
        return *this;
    }
    inline PivString &Append(const piv::basic_string_view<CharT> &s, const ptrdiff_t count = -1)
    {
        str.append(s.data(), (count == -1) ? s.size() : static_cast<size_t>(count));
        return *this;
    }
    inline PivString &Append(const PivString &s, const ptrdiff_t count = -1)
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
    inline PivString &AddManyText(const CharT *s, const ptrdiff_t times, const size_t count = (size_t)-1)
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
    inline PivString &AddManyText(const std::basic_string<CharT> &s, const ptrdiff_t times)
    {
        return AddManyText(s.c_str(), times);
    }
    inline PivString &AddManyText(const piv::basic_string_view<CharT> &s, const ptrdiff_t times)
    {
        return AddManyText(s.data(), times, s.size());
    }
    inline PivString &AddManyText(const CVolMem &s, const ptrdiff_t times)
    {
        return AddManyText(reinterpret_cast<const CharT *>(s.GetPtr()), times, static_cast<size_t>(s.GetSize()) / sizeof(CharT));
    }
    inline PivString &AddManyText(const CVolString &s, const ptrdiff_t times)
    {
        if (sizeof(EncodeType) == 2)
            return AddManyText(reinterpret_cast<const CharT *>(s.GetText()), times);
        else if (sizeof(EncodeType) == 4)
            return AddManyText(reinterpret_cast<const CharT *>(PivW2U{s}.GetText()), times);
        else
            return AddManyText(reinterpret_cast<const CharT *>(PivW2A{s}.GetText()), times);
    }

    /**
     * @brief 加入格式文本
     * @tparam ...Ts 扩展参数
     * @param format 格式
     * @param ...args 扩展参数
     * @return
     */
    template <typename... Ts>
    PivString &AddFormatText(const CharT *format, Ts... args)
    {
        str.append(piv::format(format, args...));
        return *this;
    }
    template <typename... Ts>
    PivString &AddFormatText(const CVolString &format, Ts... args)
    {
        if (sizeof(EncodeType) == 2)
            str.append(piv::format(reinterpret_cast<const CharT *>(format.GetText()), args...));
        else if (sizeof(EncodeType) == 4)
            str.append(piv::format(reinterpret_cast<const CharT *>(PivW2U{format}.GetText()), args...));
        else
            str.append(piv::format(reinterpret_cast<const CharT *>(PivW2A{format}.GetText()), args...));
        return *this;
    }
    template <typename... Ts>
    PivString &AddFormatText(const CVolConstString &format, Ts... args)
    {
        if (sizeof(EncodeType) == 2)
            str.append(piv::format(reinterpret_cast<const CharT *>(format.GetText()), args...));
        else if (sizeof(EncodeType) == 4)
            str.append(piv::format(reinterpret_cast<const CharT *>(PivW2U{format}.GetText()), args...));
        else
            str.append(piv::format(reinterpret_cast<const CharT *>(PivW2A{format}.GetText()), args...));
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
    inline PivString &InsertText(const ptrdiff_t index, const CharT ch)
    {
        ASSERT(index > 0 && index < str.size());
        str.insert(static_cast<size_t>(index), 1, ch);
        return *this;
    }
    inline PivString &InsertText(const ptrdiff_t index, const CharT *s, const ptrdiff_t count = -1)
    {
        ASSERT(index > 0 && index < str.size());
        if (count == -1)
            str.insert(static_cast<size_t>(index), s);
        else
            str.insert(static_cast<size_t>(index), s, static_cast<size_t>(count));
        return *this;
    }
    inline PivString &InsertText(const ptrdiff_t index, const std::basic_string<CharT> &s, const ptrdiff_t count = -1)
    {
        ASSERT(index > 0 && index < str.size());
        if (count == -1)
            str.insert(static_cast<size_t>(index), s);
        else
            str.insert(static_cast<size_t>(index), s, 0, static_cast<size_t>(count));
        return *this;
    }
    inline PivString &InsertText(const ptrdiff_t index, const piv::basic_string_view<CharT> &s, const ptrdiff_t count = -1)
    {
        return InsertText(index, s.data(), s.size());
    }
    inline PivString &InsertText(const ptrdiff_t index, const CVolMem &s, const ptrdiff_t count = -1)
    {
        return InsertText(index, reinterpret_cast<const CharT *>(s.GetPtr()),
                          (count == -1) ? s.GetSize() / sizeof(CharT) : count);
    }
    inline PivString &InsertText(const ptrdiff_t index, const PivString &s, const ptrdiff_t count = -1)
    {
        return InsertText(index, s.str, count);
    }
    inline PivString &InsertText(const ptrdiff_t index, const CVolString &s, const ptrdiff_t count = -1)
    {
        if (sizeof(EncodeType) == 2)
            return InsertText(index, reinterpret_cast<const CharT *>(s.GetText()), (count == -1) ? s.GetLength() : count);
        else if (sizeof(EncodeType) == 4)
            return InsertText(index, reinterpret_cast<const CharT *>(PivW2U{s}.GetText()), count);
        else
            return InsertText(index, reinterpret_cast<const CharT *>(PivW2A{s}.GetText()), count);
    }

    /**
     * @brief 插入行首空格
     * @param count 所欲插入的空格数
     * @return
     */
    inline PivString &InsertLineBeginLeaderSpaces(const int32_t count)
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
    inline PivString &RemoveChar(const ptrdiff_t pos, const ptrdiff_t count)
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
    inline ptrdiff_t &RemoveChars(const ptrdiff_t pos, const ptrdiff_t count)
    {
        ASSERT(pos > 0 && pos < str.size() && count >= 0);
        size_t ret = str.size();
        str.erase(static_cast<size_t>(pos), static_cast<size_t>(count));
        return ret - str.size();
    }

    /**
     * @brief 取字符
     */
    inline const CharT &At(const size_t pos) const
    {
        // 判断索引是否有效和文本是否为空
        ASSERT(pos >= 0 && pos < str.size() && str.empty() == false);
        return str[pos];
    }

    /**
     * @brief 取首字符
     */
    inline const CharT &Front() const
    {
        return str.empty() ? 0 : str.front();
    }

    /**
     * @brief 取尾字符
     */
    inline const CharT &Back() const
    {
        return str.empty() ? 0 : str.back();
    }

    /**
     * @brief 取文本中间
     * @param pos 起始取出索引位置
     * @param count 欲取出字符的数目
     * @return 返回文本类
     */
    inline PivString Middle(const size_t pos, const size_t count) const
    {
        ASSERT(pos <= str.size()); // 判断索引位置是否有效
        return PivString{str.substr(pos, count)};
    }

    /**
     * @brief 取文本左边
     * @param count 欲取出字符的数目
     * @return 返回文本类
     */
    inline PivString Left(const ptrdiff_t count) const
    {
        return PivString{str.substr(0, static_cast<size_t>(count))};
    }

    /**
     * @brief 取文本右边
     * @param count 欲取出字符的数目
     * @return 返回文本类
     */
    inline PivString Right(const ptrdiff_t count) const
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
    inline size_t FindChar(const CharT character, const ptrdiff_t off = 0) const
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
    inline size_t ReverseFindChar(const CharT character, const ptrdiff_t off = -1) const
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
    inline size_t FindFirstOf(const std::basic_string<CharT> &chars, const ptrdiff_t pos = 0) const
    {
        return str.find_first_of(chars, static_cast<size_t>(pos));
    }
    inline size_t FindFirstOf(const CharT *chars, const ptrdiff_t pos = 0, const ptrdiff_t count = -1) const
    {
        return (count == -1) ? str.find_first_of(chars, static_cast<size_t>(pos))
                             : str.find_first_of(chars, static_cast<size_t>(pos), static_cast<size_t>(count));
    }
    inline size_t FindFirstOf(const CVolMem &chars, const ptrdiff_t pos = 0) const
    {
        return FindFirstOf(reinterpret_cast<const CharT *>(chars.GetPtr()), pos, chars.GetSize() / sizeof(CharT));
    }
    inline size_t FindFirstOf(const PivString &chars, const ptrdiff_t pos = 0) const
    {
        return FindFirstOf(chars.str, pos);
    }
    inline size_t FindFirstOf(const piv::basic_string_view<CharT> &chars, const ptrdiff_t pos = 0) const
    {
        return FindFirstOf(chars.data(), pos, chars.size());
    }
    inline size_t FindFirstOf(const CVolString &chars, const ptrdiff_t pos = 0) const
    {
        if (sizeof(EncodeType) == 2)
            return FindFirstOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        else if (sizeof(EncodeType) == 4)
            return FindFirstOf(reinterpret_cast<const CharT *>(PivW2U{chars}.GetText()), pos);
        else
            return FindFirstOf(reinterpret_cast<const CharT *>(PivW2A{chars}.GetText()), pos);
    }

    /**
     * @brief 倒找字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t FindLastOf(const std::basic_string<CharT> &chars, const ptrdiff_t pos = -1) const
    {
        size_t p = (pos == -1) ? str.size() : static_cast<size_t>(pos);
        return str.find_last_of(chars, p);
    }
    inline size_t FindLastOf(const CharT *chars, const ptrdiff_t pos = -1, const ptrdiff_t count = -1) const
    {
        size_t p = (pos == -1) ? str.size() : static_cast<size_t>(pos);
        return (count == -1) ? str.find_last_of(chars, p)
                             : str.find_last_of(chars, p, static_cast<size_t>(count));
    }
    inline size_t FindLastOf(const CVolMem &chars, const ptrdiff_t pos = -1) const
    {
        return FindLastOf(reinterpret_cast<const CharT *>(chars.GetPtr()), pos, chars.GetSize() / sizeof(CharT));
    }
    inline size_t FindLastOf(const PivString &chars, const ptrdiff_t pos = -1) const
    {
        return FindLastOf(chars.str, pos);
    }
    inline size_t FindLastOf(const piv::basic_string_view<CharT> &chars, const ptrdiff_t pos = -1) const
    {
        return FindLastOf(chars.data(), pos, chars.size());
    }
    inline size_t FindLastOf(const CVolString &chars, const ptrdiff_t pos = -1) const
    {
        if (sizeof(EncodeType) == 2)
            return FindLastOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        else if (sizeof(EncodeType) == 4)
            return FindLastOf(reinterpret_cast<const CharT *>(PivW2U{chars}.GetText()), pos);
        else
            return FindLastOf(reinterpret_cast<const CharT *>(PivW2A{chars}.GetText()), pos);
    }

    /**
     * @brief 寻找不符合字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t FindFirstNotOf(const std::basic_string<CharT> &chars, const ptrdiff_t pos = 0) const
    {
        return str.find_first_not_of(chars, static_cast<size_t>(pos));
    }
    inline size_t FindFirstNotOf(const CharT *chars, const ptrdiff_t pos = 0, const ptrdiff_t count = -1) const
    {
        return (count == -1) ? str.find_first_not_of(chars, static_cast<size_t>(pos))
                             : str.find_first_not_of(chars, static_cast<size_t>(pos), static_cast<size_t>(count));
    }
    inline size_t FindFirstNotOf(const CVolMem &chars, const ptrdiff_t pos = 0) const
    {
        return FindFirstNotOf(reinterpret_cast<const CharT *>(chars.GetPtr()), pos, chars.GetSize() / sizeof(CharT));
    }
    inline size_t FindFirstNotOf(const PivString &chars, const ptrdiff_t pos = 0) const
    {
        return FindFirstNotOf(chars.str, pos);
    }
    inline size_t FindFirstNotOf(const piv::basic_string_view<CharT> &chars, const ptrdiff_t pos = 0)
    {
        return FindFirstNotOf(chars.data(), pos, chars.size());
    }
    inline size_t FindFirstNotOf(const CVolString &chars, const ptrdiff_t pos = 0) const
    {
        if (sizeof(EncodeType) == 2)
            return FindFirstNotOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        else if (sizeof(EncodeType) == 4)
            return FindFirstNotOf(reinterpret_cast<const CharT *>(PivW2U{chars}.GetText()), pos);
        else
            return FindFirstNotOf(reinterpret_cast<const CharT *>(PivW2A{chars}.GetText()), pos);
    }

    /**
     * @brief 倒找字符集合
     * @param chars 欲寻找的字符集合
     * @param off 起始搜寻位置
     * @return 寻找到的位置
     */
    inline size_t FindLastNotOf(const std::basic_string<CharT> &chars, const ptrdiff_t pos = -1) const
    {
        size_t p = (pos == -1) ? str.size() : static_cast<size_t>(pos);
        return str.find_last_not_of(chars, p);
    }
    inline size_t FindLastNotOf(const CharT *chars, const ptrdiff_t pos = -1, const ptrdiff_t count = -1) const
    {
        size_t p = (pos == -1) ? str.size() : static_cast<size_t>(pos);
        return (count == -1) ? str.find_last_not_of(chars, p)
                             : str.find_last_not_of(chars, p, static_cast<size_t>(count));
    }
    inline size_t FindLastNotOf(const CVolMem &chars, const ptrdiff_t pos = -1) const
    {
        return FindLastNotOf(reinterpret_cast<const CharT *>(chars.GetPtr()), pos, chars.GetSize() / sizeof(CharT));
    }
    inline size_t FindLastNotOf(const PivString &chars, const ptrdiff_t pos = -1) const
    {
        return FindLastNotOf(chars.str, pos);
    }
    inline size_t FindLastNotOf(const piv::basic_string_view<CharT> &chars, const ptrdiff_t pos = -1) const
    {
        return FindLastNotOf(chars.data(), pos, chars.size());
    }
    inline size_t FindLastNotOf(const CVolString &chars, const ptrdiff_t pos = -1) const
    {
        if (sizeof(EncodeType) == 2)
            return FindLastNotOf(reinterpret_cast<const CharT *>(chars.GetText()), pos);
        else if (sizeof(EncodeType) == 4)
            return FindLastNotOf(reinterpret_cast<const CharT *>(PivW2U{chars}.GetText()), pos);
        else
            return FindLastNotOf(reinterpret_cast<const CharT *>(PivW2A{chars}.GetText()), pos);
    }

    /**
     * @brief 寻找文本
     * @param text 欲寻找的文本
     * @param pos 起始搜寻位置
     * @param case_insensitive 是否不区分大小写
     * @param count 欲寻找文本的长度
     * @return 寻找到的位置
     */
    inline size_t SearchText(const piv::basic_string_view<CharT> &text, const ptrdiff_t pos = 0, const bool case_insensitive = false) const
    {
        if (!case_insensitive)
            return str.find(text.data(), static_cast<size_t>(pos), text.size());
        else
            return piv::ifind(piv::basic_string_view<CharT>{str}, piv::basic_string_view<CharT>{text}, pos);
    }
    inline size_t SearchText(const std::basic_string<CharT> &text, const ptrdiff_t pos = 0, const bool case_insensitive = false) const
    {
        if (!case_insensitive)
            return str.find(text, static_cast<size_t>(pos));
        else
            return piv::ifind(piv::basic_string_view<CharT>{str}, piv::basic_string_view<CharT>{text}, pos);
    }
    inline size_t SearchText(const CharT *text, const ptrdiff_t pos = 0, const bool case_insensitive = false, const ptrdiff_t count = -1) const
    {
        return SearchText((count == -1) ? piv::basic_string_view<CharT>{text} : piv::basic_string_view<CharT>{text, static_cast<size_t>(count)}, pos, case_insensitive);
    }
    inline size_t SearchText(const CVolMem &text, const ptrdiff_t pos = 0, const bool case_insensitive = false) const
    {
        return SearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetPtr()), static_cast<size_t>(text.GetSize()) / sizeof(CharT)}, pos, case_insensitive);
    }
    inline size_t SearchText(const PivString &text, const ptrdiff_t pos = 0, const bool case_insensitive = false) const
    {
        return SearchText(text.str, pos, case_insensitive);
    }
    inline size_t SearchText(const CVolString &text, const ptrdiff_t pos = 0, const bool case_insensitive = false) const
    {
        if (sizeof(EncodeType) == 2)
            return SearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetText())}, pos, case_insensitive);
        else if (sizeof(EncodeType) == 4)
            return SearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{text}.GetText())}, pos, case_insensitive);
        else
            return SearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{text}.GetText())}, pos, case_insensitive);
    }

    /**
     * @brief 倒找文本
     * @param text 欲寻找的文本
     * @param pos 起始搜寻位置
     * @param count 欲寻找文本的长度
     * @param case_insensitive 是否不区分大小写
     * @return 寻找到的位置
     */
    inline size_t ReverseSearchText(const piv::basic_string_view<CharT> &text, const ptrdiff_t pos = -1, const bool case_insensitive = false) const
    {
        size_t p = (pos == -1) ? str.size() : static_cast<size_t>(pos);
        if (!case_insensitive)
            return str.rfind(text.data(), p, text.size());
        else
            return piv::irfind(piv::basic_string_view<CharT>{str}, text, p);
    }
    inline size_t ReverseSearchText(const std::basic_string<CharT> &text, const ptrdiff_t pos = -1, const bool case_insensitive = false) const
    {
        return ReverseSearchText(piv::basic_string_view<CharT>{text}, pos, case_insensitive);
    }
    inline size_t ReverseSearchText(const CharT *text, const ptrdiff_t pos = -1, const bool case_insensitive = false, const ptrdiff_t count = -1) const
    {
        return ReverseSearchText((count == -1) ? piv::basic_string_view<CharT>{text} : piv::basic_string_view<CharT>{text, static_cast<size_t>(count)}, pos, case_insensitive);
    }
    inline size_t ReverseSearchText(const CVolMem &text, const ptrdiff_t pos = -1, const bool case_insensitive = false) const
    {
        return ReverseSearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetPtr()), static_cast<size_t>(text.GetSize()) / sizeof(CharT)}, pos, case_insensitive);
    }
    inline size_t ReverseSearchText(const PivString &text, const ptrdiff_t pos = -1, const bool case_insensitive = false) const
    {
        return ReverseSearchText(text.str, pos, case_insensitive);
    }
    inline size_t ReverseSearchText(const CVolString &text, const ptrdiff_t pos = -1, const bool case_insensitive = false) const
    {
        if (sizeof(EncodeType) == 2)
            return ReverseSearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetText())}, pos, case_insensitive);
        else if (sizeof(EncodeType) == 4)
            return ReverseSearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{text}.GetText())}, pos, case_insensitive);
        else
            return ReverseSearchText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{text}.GetText())}, pos, case_insensitive);
    }

    /**
     * @brief 是否以字符/文本开头
     * @param s 所欲检查的文本
     * @param ch 所欲检查的字符
     * @param count 所欲检查文本的长度
     * @param case_sensitive 是否区分大小写
     * @return
     */
    inline bool LeadOf(const CharT ch, const bool case_sensitive = true) const
    {
        if (str.empty())
            return false;
        if (case_sensitive)
            return (str.front() == ch);
        else
            return (std::tolower(static_cast<unsigned char>(str.front())) == std::tolower(static_cast<unsigned char>(ch)));
    }
    inline bool LeadOf(const CharT *s, const bool case_sensitive = true, const size_t count = (size_t)-1) const
    {
        return (SearchText(s, 0, !case_sensitive, count) == 0);
    }
    inline bool LeadOf(const std::basic_string<CharT> &s, const bool case_sensitive = true) const
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }
    inline bool LeadOf(const CVolMem &s, const bool case_sensitive = true) const
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }
    inline bool LeadOf(const PivString &s, const bool case_sensitive = true) const
    {
        return (SearchText(s.str, 0, !case_sensitive) == 0);
    }
    inline bool LeadOf(const piv::basic_string_view<CharT> &s, const bool case_sensitive = true) const
    {
        return (SearchText(s, 0, !case_sensitive) == 0);
    }
    inline bool LeadOf(const CVolString &s, const bool case_sensitive = true) const
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
    inline bool EndOf(const CharT ch, const bool case_sensitive = true) const
    {
        if (str.empty())
            return false;
        if (case_sensitive)
            return (str.back() == ch);
        else
            return (std::tolower(static_cast<unsigned char>(str.back())) == std::tolower(static_cast<unsigned char>(ch)));
    }
    inline bool EndOf(const piv::basic_string_view<CharT> &s, const bool case_sensitive = true) const
    {
        return (ReverseSearchText(s, -1, !case_sensitive) == (str.size() - s.size()));
    }
    inline bool EndOf(const CharT *s, const bool case_sensitive = true, const size_t count = (size_t)-1)
    {
        return EndOf((count == -1) ? piv::basic_string_view<CharT>{s} : piv::basic_string_view<CharT>{s, static_cast<size_t>(count)}, case_sensitive);
    }
    inline bool EndOf(const std::basic_string<CharT> &s, const bool case_sensitive = true) const
    {
        return EndOf(piv::basic_string_view<CharT>{s}, case_sensitive);
    }
    inline bool EndOf(const CVolMem &s, const bool case_sensitive = true) const
    {
        return EndOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(s.GetPtr()), static_cast<size_t>(s.GetSize()) / sizeof(CharT)}, case_sensitive);
    }
    inline bool EndOf(const PivString &s, const bool case_sensitive = true) const
    {
        return EndOf(s.str, case_sensitive);
    }
    inline bool EndOf(const CVolString &s, const bool case_sensitive = true) const
    {
        if (sizeof(EncodeType) == 2)
            return EndOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(text.GetText())}, case_sensitive);
        else if (sizeof(EncodeType) == 4)
            return EndOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2U{text}.GetText())}, case_sensitive);
        else
            return EndOf(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(PivW2A{text}.GetText())}, case_sensitive);
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
    inline PivString &Replace(const ptrdiff_t pos, const ptrdiff_t count, const CharT *s, const ptrdiff_t count2 = -1)
    {
        ASSERT(pos >= 0 && pos <= str.length() && count >= 0);
        if (count2 == -1)
            str.replace(static_cast<size_t>(pos), static_cast<size_t>(count), s);
        else
            str.replace(static_cast<size_t>(pos), static_cast<size_t>(count), s, static_cast<size_t>(count2));
        return *this;
    }
    inline PivString &Replace(const ptrdiff_t pos, const ptrdiff_t count, const std::basic_string<CharT> &s)
    {
        ASSERT(pos >= 0 && pos <= str.length() && count >= 0);
        str.replace(static_cast<size_t>(pos), static_cast<size_t>(count), s);
        return *this;
    }
    inline PivString &Replace(const ptrdiff_t pos, const ptrdiff_t count, const piv::basic_string_view<CharT> &s)
    {
        return Replace(pos, count, reinterpret_cast<const CharT *>(s.data()), s.size());
    }
    inline PivString &Replace(const ptrdiff_t pos, const ptrdiff_t count, const CVolMem &s)
    {
        return Replace(pos, count, reinterpret_cast<const CharT *>(s.GetPtr()), s.GetSize() * sizeof(CharT));
    }
    inline PivString &Replace(const ptrdiff_t pos, const ptrdiff_t count, const CVolString &s)
    {
        if (sizeof(EncodeType) == 2)
            return Replace(pos, count, reinterpret_cast<const CharT *>(text.GetText()), -1);
        else if (sizeof(EncodeType) == 4)
            return Replace(pos, count, reinterpret_cast<const CharT *>(PivW2U{text}.GetText()), -1);
        else
            return Replace(pos, count, reinterpret_cast<const CharT *>(PivW2A{text}.GetText()), -1);
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
    inline PivString &ReplaceSubText(const CharT *findStr, const CharT *repStr, const ptrdiff_t pos = 0, const ptrdiff_t count = -1, const bool case_sensitive = true, const size_t findLen = (size_t)-1, const size_t repLen = (size_t)-1)
    {
        ASSERT(pos >= 0 && pos <= str.length());
        if (findLen == (size_t)-1)
            findLen = (sizeof(CharT) == 2) ? wcslen(reinterpret_cast<const wchar_t *>(findStr)) : strlen(reinterpret_cast<const char *>(findStr));
        if (repLen == (size_t)-1)
            repLen = (sizeof(CharT) == 2) ? wcslen(reinterpret_cast<const wchar_t *>(repStr)) : strlen(reinterpret_cast<const char *>(repStr));
        return ReplaceSubText(piv::basic_string_view<CharT>{findStr, findLen}, piv::basic_string_view<CharT>{repStr, repLen}, pos, count, case_sensitive);
    }
    inline PivString &ReplaceSubText(const std::basic_string<CharT> &findStr, const std::basic_string<CharT> &repStr, const ptrdiff_t pos = 0, const ptrdiff_t count = -1, const bool case_sensitive = true)
    {
        return ReplaceSubText(piv::basic_string_view<CharT>{findStr.c_str()}, piv::basic_string_view<CharT>{repStr.c_str()}, pos, count, case_sensitive);
    }
    inline PivString &ReplaceSubText(const PivString &findStr, const PivString &repStr, const ptrdiff_t pos = 0, const ptrdiff_t count = -1, const bool case_sensitive = true)
    {
        return ReplaceSubText(findStr.str, repStr.str, pos, count, case_sensitive);
    }
    inline PivString &ReplaceSubText(const CVolMem &findStr, const CVolMem &repStr, const ptrdiff_t pos = 0, const ptrdiff_t count = -1, const bool case_sensitive = true)
    {
        return ReplaceSubText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(findStr.GetPtr()), static_cast<size_t>(findStr.GetSize()) / sizeof(CharT)},
                              piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(repStr.GetPtr()), static_cast<size_t>(repStr.GetSize()) / sizeof(CharT)}, pos, count, case_sensitive);
    }
    PivString &ReplaceSubText(const CVolString &findStr, const CVolString &repStr, const ptrdiff_t pos = 0, const ptrdiff_t count = -1, const bool case_sensitive = true)
    {
        if (sizeof(EncodeType) == 2)
        {
            return ReplaceSubText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(findStr.GetText())},
                                  piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(repStr.GetText())}, pos, count, case_sensitive);
        }
        else if (sizeof(EncodeType) == 4)
        {
            PivW2U find_utf8{findStr};
            PivW2U rep_utf8{repStr};
            return ReplaceSubText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(find_utf8.GetText())},
                                  piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(rep_utf8.GetText())}, pos, count, case_sensitive);
        }
        else
        {
            PivW2A find_mbcs{findStr};
            PivW2A rep_mbcs{repStr};
            return ReplaceSubText(piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(find_mbcs.GetText())},
                                  piv::basic_string_view<CharT>{reinterpret_cast<const CharT *>(rep_mbcs.GetText())}, pos, count, case_sensitive);
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
    inline PivString &CheckAddCRLF()
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
    inline PivString &CheckAddPathChar()
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
    inline PivString &TrimLeft()
    {
        piv::trim_left(str);
        return *this;
    }

    /**
     * @brief 删尾空
     */
    inline PivString &TrimRight()
    {
        piv::trim_right(str);
        return *this;
    }

    /**
     * @brief 删首尾空
     */
    inline PivString &TrimAll()
    {
        piv::trim_left(str);
        piv::trim_right(str);
        return *this;
    }

    /**
     * @brief 删首字符
     */
    inline PivString &RemoveFront()
    {
        if (!str.empty())
            str.erase(0, 1);
        return *this;
    }

    /**
     * @brief 删尾字符
     */
    inline PivString &RemoveBack()
    {
        if (!str.empty())
            str.pop_back();
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
    size_t SplitStrings(const CharT *delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true, const size_t count = static_cast<size_t>(-1)) const
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
    inline size_t SplitStrings(const std::basic_string<CharT> &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true) const
    {
        return SplitStrings(delimit.c_str(), strArray, trimAll, ignoreEmptyStr, delimit.size());
    }
    inline size_t SplitStrings(const CVolMem &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true) const
    {
        return SplitStrings(reinterpret_cast<const CharT *>(delimit.GetPtr()), svArray, trimAll, ignoreEmptyStr, static_cast<size_t>(delimit.GetSize() / sizeof(CharT)));
    }
    inline size_t SplitStrings(const PivString &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true) const
    {
        return SplitStrings(delimit.str, strArray, trimAll, ignoreEmptyStr);
    }
    inline size_t SplitStrings(const piv::basic_string_view<CharT> &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true) const
    {
        return SplitStrings(delimit.data(), strArray, trimAll, ignoreEmptyStr, delimit.size());
    }
    inline size_t SplitStrings(const CVolString &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true) const
    {
        if (sizeof(EncodeType) == 2)
            return SplitStrings(reinterpret_cast<const CharT *>(delimit.GetText()), strArray, trimAll, ignoreEmptyStr, static_cast<size_t>(delimit.GetLength()));
        else if (sizeof(EncodeType) == 4)
            return SplitStrings(reinterpret_cast<const CharT *>(PivW2U{delimit}.GetText()), strArray, trimAll, ignoreEmptyStr);
        else
            return SplitStrings(reinterpret_cast<const CharT *>(PivW2A{delimit}.GetText()), strArray, trimAll, ignoreEmptyStr);
    }

    /**
     * @brief 分割子文本
     * @param delimit 分割用子文本
     * @param strArray 结果存放数组
     * @param trimAll 是否删除首尾空
     * @param ignoreEmptyStr 是否忽略空白结果
     * @return 所分割出来的结果文本数目
     */
    size_t SplitSubStrings(const CharT *delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true, const size_t count = static_cast<size_t>(-1)) const
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
    inline size_t SplitSubStrings(const std::basic_string<CharT> &delimit, std::vector<std::basic_string<CharT>> &strArray, bool trimAll = true, bool ignoreEmptyStr = true) const
    {
        return SplitSubStrings(delimit.c_str(), strArray, trimAll, ignoreEmptyStr, delimit.size());
    }
    inline size_t SplitSubStrings(const CVolMem &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true) const
    {
        return SplitSubStrings(reinterpret_cast<const CharT *>(delimit.GetPtr()), strArray, trimAll, ignoreEmptyStr, static_cast<size_t>(delimit.GetSize()) / sizeof(CharT));
    }
    inline size_t SplitSubStrings(const PivString &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true) const
    {
        return SplitSubStrings(delimit.str, strArray, trimAll, ignoreEmptyStr);
    }
    inline size_t SplitSubStrings(const piv::basic_string_view<CharT> &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true) const
    {
        return SplitSubStrings(delimit.data(), strArray, trimAll, ignoreEmptyStr, delimit.size());
    }
    inline size_t SplitSubStrings(const CVolString &delimit, std::vector<std::basic_string<CharT>> &strArray, const bool trimAll = true, const bool ignoreEmptyStr = true) const
    {
        if (sizeof(EncodeType) == 2)
            return SplitSubStrings(reinterpret_cast<const CharT *>(delimit.GetText()), strArray, trimAll, ignoreEmptyStr, static_cast<size_t>(delimit.GetLength()));
        else if (sizeof(EncodeType) == 4)
            return SplitSubStrings(reinterpret_cast<const CharT *>(PivW2U{delimit}.GetText()), strArray, trimAll, ignoreEmptyStr);
        else
            return SplitSubStrings(reinterpret_cast<const CharT *>(PivW2A{delimit}.GetText()), strArray, trimAll, ignoreEmptyStr);
    }

    /**
     * @brief 复制文本
     * @param dest 欲复制到的目标
     * @param pos 欲复制的起始索引位置
     * @param count 欲复制的字符数目
     * @return 复制的字符数
     */
    inline size_t CopyStr(CharT *dest, const size_t pos, const size_t count) const
    {
        ASSERT(pos <= str.size()); // 判断索引位置是否有效
        return str.copy(dest, count, pos);
    }
    inline size_t CopyStr(ptrdiff_t dest, const size_t pos, const size_t count) const
    {
        return CopyStr(reinterpret_cast<CharT *>(dest), pos, count);
    }
    inline size_t CopyStr(CVolString &dest, const size_t pos, const size_t count) const
    {
        dest.SetLength(count);
        size_t n = CopyStr(const_cast<CharT *>(reinterpret_cast<cont CharT *>(dest.GetText())), pos, count);
        if (n == 0)
            dest.Empty();
        else
            dest.RemoveChars(n, count - n);
        return n;
    }
    inline size_t CopyStr(CVolMem &dest, const size_t pos, const size_t count) const
    {
        dest.Alloc(count, TRUE);
        size_t n = CopyStr(const_cast<CharT *>(reinterpret_cast<cont CharT *>(dest.GetText())), pos, count);
        if (n == 0)
            dest.Empty();
        else
            dest.Realloc(n);
        return n;
    }
    inline size_t CopyStr(std::basic_string<CharT> &dest, const size_t pos, const size_t count) const
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
    inline CVolMem ToVolMem(const bool null_terminated) const
    {
        return CVolMem(reinterpret_cast<const void *>(str.data()), (str.size() + (null_terminated ? 1 : 0)) * sizeof(CharT));
    }

    /**
     * @brief 到文本型
     * @return
     */
    inline CVolString ToVolString() const
    {
        if (sizeof(EncodeType) == 2)
            return CVolString(reinterpret_cast<const wchar_t *>(str.c_str()));
        else if (sizeof(EncodeType) == 4)
            return PivU2W{reinterpret_cast<const char *>(str.c_str())}.ToStr();
        else
            return PivA2W{reinterpret_cast<const char *>(str.c_str())}.ToStr();
    }

    /**
     * @brief 到整数
     * @param base 进制
     * @return
     */
    inline int32_t ToInt(const int base) const
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
    inline int64_t ToInt64(const int base) const
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
    inline double ToDouble() const
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
    inline piv::basic_string_view<CharT> ToStringView() const
    {
        return piv::basic_string_view<CharT>{str.c_str()};
    }

    /**
     * @brief 到小写
     * @return
     */
    inline PivString MakeLower() const
    {
        std::basic_string<CharT> lower;
        lower.resize(str.size());
        std::transform(str.begin(), str.end(), lower.begin(), std::tolower);
        return lower;
    }
    inline PivString &ToLower()
    {
        std::transform(str.begin(), str.end(), str.begin(), std::tolower);
        return *this;
    }

    /**
     * @brief 到大写
     * @return
     */
    inline PivString MakeUpper() const
    {
        std::basic_string<CharT> upper;
        upper.resize(str.size());
        std::transform(str.begin(), str.end(), upper.begin(), std::toupper);
        return upper;
    }
    inline PivString &ToUpper()
    {
        std::transform(str.begin(), str.end(), str.begin(), std::toupper);
        return *this;
    }

    /**
     * @brief URL编码
     * @param utf8
     * @return
     */
    inline PivString UrlEncode(const bool utf8 = true) const
    {
        return PivString(piv::encoding::UrlStrEncode(str, utf8));
    }

    /**
     * @brief URL解码
     * @param utf8 是否为UTF-8编码
     * @return
     */
    inline PivString UrlDecode(const bool utf8 = true) const
    {
        return PivString(piv::encoding::UrlStrDecode(str, true, utf8));
    }

    /**
     * @brief BASE64编码
     * @return
     */
    inline PivString EncodeBase64(const int32_t line_len = 0) const
    {
        return PivString(piv::base64<CharT>{}.encode(str, line_len));
    }

    /**
     * @brief BASE64解码
     * @return
     */
    inline PivString DecodeBase64() const
    {
        return PivString(piv::base64<CharT>{}.decode(str));
    }

    /**
     * @brief BASE64到字节集
     * @return
     */
    inline CVolMem DecodeBase64Bin() const
    {
        CVolMem buffer;
        piv::base64<CharT>{}.decode(str, buffer);
        return buffer;
    }

    /**
     * @brief BASE85编码
     * @return
     */
    inline PivString EncodeBase85(const bool padding = false) const
    {
        return PivString(piv::base85<CharT>{}.encode(str, padding));
    }

    /**
     * @brief BASE85解码
     * @return
     */
    inline PivString DecodeBase85() const
    {
        return PivString(piv::base85<CharT>{}.decode(str));
    }

    /**
     * @brief BASE85到字节集
     * @return
     */
    inline CVolMem DecodeBase85Bin() const
    {
        CVolMem buffer;
        piv::base85<CharT>{}.decode(str, buffer);
        return buffer;
    }

    /**
     * @brief BASE91编码
     * @return
     */
    inline PivString EncodeBase91() const
    {
        return PivString(piv::base91<CharT>{}.encode(str));
    }

    /**
     * @brief BASE91解码
     * @return
     */
    inline PivString DecodeBase91() const
    {
        return PivString(piv::base91<CharT>{}.decode(str));
    }

    /**
     * @brief BASE64到字节集
     * @return
     */
    inline CVolMem DecodeBase91Bin() const
    {
        CVolMem buffer;
        piv::base91<CharT>{}.decode(str, buffer);
        return buffer;
    }

}; // PivString

#endif // _PIV_STRING_HPP
