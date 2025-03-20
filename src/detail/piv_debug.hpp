/*********************************************\
 * 火山视窗-PIV模块                          *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 仓库: https://github.com/SaierMe/piv      *
 * 注意: 本头文件的include顺序应该尽量靠后   *
\*********************************************/

#ifndef _PIV_DEBUG_TRACE_HPP
#define _PIV_DEBUG_TRACE_HPP

#ifndef __VOL_BASE_H__
#include <sys/base/libs/win_base/vol_base.h>
#endif

#include <cstdint>
#include <type_traits>
/**
 * @brief 取展示内容
 * @tparam T
 * @param strDebug 展示内容
 * @param nMaxDumpSize 最大展示数据尺寸
 * @param value 欲展示数据
 * 自定义类型请重载本方法,注意类和结构体需要封装它的指针类型
=============================================================================
@begin
<global>
<include>
static void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, T* value)
{
    if (value != nullptr)
    {
        ...
    }
    else
        strDebug.SetText(L"nullptr");
}
</include>
</global>
@end
=============================================================================
*/

template <typename T>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, T *value)
{
    TCHAR buf[64];
    buf[0] = '0';
    buf[1] = 'x';
    ToHexStr((UINT64)value, sizeof(UINT64) * 2, &buf[2], NUM_ELEMENTS_OF(buf) - 2);
    strDebug.SetText(buf);
}

template <typename T>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, const T *value)
{
    PivDumpStr(strDebug, nMaxDumpSize, const_cast<T *>(value));
}

template <typename T, int nArraySize> // 数组专用
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, T _array[nArraySize])
{
    strDebug.AddChar('{');
    for (int i = 0; i < nArraySize; i++)
    {
        if (i != 0)
            strDebug.AddText(L", ");
        strDebug.AddText(PivGetDumpStr(_array[i]));
    }
    strDebug.AddChar('}');
}

template <typename T, typename std::enable_if<std::is_class<typename std::decay<T>::type>::value, int>::type = 0>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, T &value)
{
    if (std::is_base_of<CVolObject, T>::value)
        PivDumpStr(strDebug, nMaxDumpSize, (CVolObject *)&value);
    else
        PivDumpStr(strDebug, nMaxDumpSize, &value);
}

static void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, CVolObject *value)
{
    if (value != nullptr)
    {
        CVolString strBuf;
        value->GetDumpString(strBuf, nMaxDumpSize); // 调用对应对象的取展示内容方法
        strDebug.SetText(strBuf);
    }
    else
    {
        strDebug.SetText(L"nullptr");
    }
}

static void PivDumpStr(CVolString &strDebug, INT nMaxDumpSiz, const wchar_t *value)
{
    strDebug.AddText(value);
}

static void PivDumpStr(CVolString &strDebug, INT nMaxDumpSiz, wchar_t *value)
{
    strDebug.AddText(value);
}

static void PivDumpStr(CVolString &strDebug, INT nMaxDumpSiz, const char *value)
{
    strDebug.AddText(value);
}

static void PivDumpStr(CVolString &strDebug, INT nMaxDumpSiz, char *value)
{
    strDebug.AddText(value);
}

static void PivDumpStr(CVolString &strDebug, INT nMaxDumpSiz, wchar_t value)
{
    strDebug.AddChar(value);
}

static void PivDumpStr(CVolString &strDebug, INT nMaxDumpSiz, char value)
{
    strDebug.AddChar(value);
}

static void PivDumpStr(CVolString &strDebug, INT nMaxDumpSiz, int8_t value)
{
    strDebug.AddIntText(value);
}

static void PivDumpStr(CVolString &strDebug, INT nMaxDumpSiz, uint8_t value)
{
    strDebug.AddIntText(value);
}

static void PivDumpStr(CVolString &strDebug, INT nMaxDumpSiz, int16_t value)
{
    strDebug.AddIntText(value);
}

static void PivDumpStr(CVolString &strDebug, INT nMaxDumpSiz, uint16_t value)
{
    strDebug.AddIntText(value);
}

static void PivDumpStr(CVolString &strDebug, INT nMaxDumpSiz, int32_t value)
{
    strDebug.AddIntText(value);
}

static void PivDumpStr(CVolString &strDebug, INT nMaxDumpSiz, uint32_t value)
{
    strDebug.AddDWordText(value);
}

static void PivDumpStr(CVolString &strDebug, INT nMaxDumpSiz, DWORD value)
{
    strDebug.AddDWordText(value);
}

static void PivDumpStr(CVolString &strDebug, INT nMaxDumpSiz, int64_t value)
{
    strDebug.AddInt64Text(value);
}

static void PivDumpStr(CVolString &strDebug, INT nMaxDumpSiz, uint64_t value)
{
    strDebug.AddUInt64Text(value);
}

// #include <string>
#ifdef _STRING_
template <class CharT, class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT>>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, std::basic_string<CharT, Traits, Allocator> *value)
{
    if (value != nullptr)
        strDebug.SetText(value->c_str());
    else
        strDebug.SetText(L"std::basic_string: nullptr");
}
#endif // _STRING_

// #include <string_view>
#ifdef _STRING_VIEW_
template <class CharT, class Traits = std::char_traits<CharT>>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, std::basic_string_view<CharT, Traits> *value)
{
    if (value != nullptr)
        strDebug.SetText(value->data(), value->size());
    else
        strDebug.SetText(L"std::basic_string_view: nullptr");
}
#endif // _STRING_VIEW_

// #include <vector>
#ifdef _VECTOR_
template <class T, class Allocator = std::allocator<T>>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, std::vector<T, Allocator> *value)
{
    if (value != nullptr)
    {
        strDebug.AddFormatText(L"<动态数组/vector> 总共 %d 个成员:", value->size());
        for (size_t i = 0; i < value->size(); i++)
        {
            CVolString strBuf;
            PivDumpStr(strBuf, nMaxDumpSize, value->at(i));
            strDebug.AddFormatText(L"\r\n%d. %s", i, strBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::vector: nullptr");
}
#endif // _VECTOR_

// #include <array>
#ifdef _ARRAY_
template <class T, std::size_t N>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, std::array<T, N> *value)
{
    if (value != nullptr)
    {
        strDebug.AddFormatText(L"<标准数组/array> 总共 %d 个成员:", value->size());
        for (size_t i = 0; i < value->size(); i++)
        {
            CVolString strBuf;
            PivDumpStr(strBuf, nMaxDumpSize, value->at(i));
            strDebug.AddFormatText(L"\r\n%d. %s", i, strBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::array: nullptr");
}
#endif // _ARRAY_

// #include <utility>
#ifdef _UTILITY_
template <class T1, class T2>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, std::pair<T1, T2> *value)
{
    if (value != nullptr)
    {
        CVolString keyBuf, valBuf;
        PivDumpStr(keyBuf, nMaxDumpSize, value->first);
        PivDumpStr(valBuf, nMaxDumpSize, value->second);
        strDebug.AddFormatText(L"<键值对/pair> 关键字: %s; 对应值: %s", keyBuf.GetText(), valBuf.GetText());
    }
    else
        strDebug.SetText(L"std::pair: nullptr");
}
#endif // _UTILITY_

// #include <map>
#ifdef _MAP_
template <class Key, class T, class Compare = std::less<Key>, class Allocator = std::allocator<std::pair<const Key, T>>>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, std::map<Key, T, Compare, Allocator> *value)
{
    if (value != nullptr)
    {
        strDebug.AddFormatText(L"<排序表/map> 总共 %d 个成员:", value->size());
        int32_t i = 0;
        for (auto &it = value->begin(); it != value->end(); it++, i++)
        {
            CVolString keyBuf, valBuf;
            PivDumpStr(keyBuf, nMaxDumpSize, it->first);
            PivDumpStr(valBuf, nMaxDumpSize, it->second);
            strDebug.AddFormatText(L"\r\n%d. 关键字: %s; 对应值: %s", i, keyBuf.GetText(), valBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::map: nullptr");
}

template <class Key, class T, class Compare = std::less<Key>, class Allocator = std::allocator<std::pair<const Key, T>>>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, std::multimap<Key, T, Compare, Allocator> *value)
{
    if (value != nullptr)
    {
        strDebug.AddFormatText(L"<多值排序表/multimap> 总共 %d 个成员:", value->size());
        int32_t i = 0;
        for (auto &it = value->begin(); it != value->end(); it++, i++)
        {
            CVolString keyBuf, valBuf;
            PivDumpStr(keyBuf, nMaxDumpSize, it->first);
            PivDumpStr(valBuf, nMaxDumpSize, it->second);
            strDebug.AddFormatText(L"\r\n%d. 关键字: %s; 对应值: %s", i, keyBuf.GetText(), valBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::multimap: nullptr");
}
#endif // _MAP_

// #include <set>
#ifdef _SET_
template <class Key, class Compare = std::less<Key>, class Allocator = std::allocator<Key>>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, std::set<Key, Compare, Allocator> *value)
{
    if (value != nullptr)
    {
        strDebug.AddFormatText(L"<排序集/set> 总共 %d 个成员:", value->size());
        int32_t i = 0;
        for (auto it = value->begin(); it != value->end(); it++, i++)
        {
            CVolString keyBuf;
            PivDumpStr(keyBuf, nMaxDumpSize, *it);
            strDebug.AddFormatText(L"\r\n%d. 关键字: %s", i, keyBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::set: nullptr");
}

template <class Key, class Compare = std::less<Key>, class Allocator = std::allocator<Key>>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, std::multiset<Key, Compare, Allocator> *value)
{
    if (value != nullptr)
    {
        strDebug.AddFormatText(L"<多值排序集/multiset> 总共 %d 个成员:", value->size());
        int32_t i = 0;
        for (auto it = value->begin(); it != value->end(); it++, i++)
        {
            CVolString keyBuf;
            PivDumpStr(keyBuf, nMaxDumpSize, *it);
            strDebug.AddFormatText(L"\r\n%d. 关键字: %s\r\n", i, keyBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::multiset: nullptr");
}
#endif // _SET_

// #include <unordered_map>
#ifdef _UNORDERED_MAP_
template <class Key, class T, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>, class Allocator = std::allocator<std::pair<const Key, T>>>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, std::unordered_map<Key, T, Hash, KeyEqual, Allocator> *value)
{
    if (value != nullptr)
    {
        strDebug.AddFormatText(L"<哈希表/unordered_map> 总共 %d 个成员:", value->size());
        int32_t i = 0;
        for (auto &it = value->begin(); it != value->end(); it++, i++)
        {
            CVolString keyBuf, valBuf;
            PivDumpStr(keyBuf, nMaxDumpSize, it->first);
            PivDumpStr(valBuf, nMaxDumpSize, it->second);
            strDebug.AddFormatText(L"\r\n%d. 关键字: %s; 对应值: %s", i, keyBuf.GetText(), valBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::unordered_map: nullptr");
}

template <class Key, class T, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>, class Allocator = std::allocator<std::pair<const Key, T>>>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator> *value)
{
    if (value != nullptr)
    {
        strDebug.AddFormatText(L"<多值哈希表/unordered_multimap> 总共 %d 个成员:", value->size());
        int32_t i = 0;
        for (auto &it = value->begin(); it != value->end(); it++, i++)
        {
            CVolString keyBuf, valBuf;
            PivDumpStr(keyBuf, nMaxDumpSize, it->first);
            PivDumpStr(valBuf, nMaxDumpSize, it->second);
            strDebug.AddFormatText(L"\r\n%d. 关键字: %s; 对应值: %s", i, keyBuf.GetText(), valBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::unordered_multimap: nullptr");
}
#endif // _UNORDERED_MAP_

// #include <unordered_set>
#ifdef _UNORDERED_SET_
template <class Key, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>, class Allocator = std::allocator<Key>>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, std::unordered_set<Key, Hash, KeyEqual, Allocator> *value)
{
    if (value != nullptr)
    {
        strDebug.AddFormatText(L"<哈希集/unordered_set> 总共 %d 个成员:", value->size());
        int32_t i = 0;
        for (auto it = value->begin(); it != value->end(); it++, i++)
        {
            CVolString keyBuf;
            PivDumpStr(keyBuf, nMaxDumpSize, *it);
            strDebug.AddFormatText(L"\r\n%d. 关键字: %s", i, keyBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::unordered_set: nullptr");
}

template <class Key, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>, class Allocator = std::allocator<Key>>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, std::unordered_multiset<Key, Hash, KeyEqual, Allocator> *value)
{
    if (value != nullptr)
    {
        strDebug.AddFormatText(L"<多值哈希集/unordered_multiset> 总共 %d 个成员:", value->size());
        int32_t i = 0;
        for (auto it = value->begin(); it != value->end(); it++, i++)
        {
            CVolString keyBuf;
            PivDumpStr(keyBuf, nMaxDumpSize, *it);
            strDebug.AddFormatText(L"\r\n%d. 关键字: %s", i, keyBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::unordered_multiset: nullptr");
}
#endif // _SET_

// #include <queue>
#ifdef _QUEUE_
template <class T, class Container = std::deque<T>>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, std::queue<T, Container> *value)
{
    if (value != nullptr)
    {
        strDebug.AddFormatText(L"<队列/queue> 总共 %d 个成员.", value->size());
    }
    else
        strDebug.SetText(L"std::queue: nullptr");
}
#endif // _QUEUE_

// #include <deque>
#ifdef _DEQUE_
template <class T, class Allocator = std::allocator<T>>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, std::deque<T, Allocator> *value)
{
    if (value != nullptr)
    {
        strDebug.AddFormatText(L"<双端队列/deque> 总共 %d 个成员:", value->size());
        int32_t i = 0;
        for (auto it = value->begin(); it != value->end(); it++, i++)
        {
            CVolString strBuf;
            PivDumpStr(strBuf, nMaxDumpSize, *it);
            strDebug.AddFormatText(L"\r\n%d. %s", i, strBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::deque: nullptr");
}
#endif // _QUEUE_

// #include <list>
#ifdef _LIST_
template <class T, class Allocator = std::allocator<T>>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, std::list<T, Allocator> *value)
{
    if (value != nullptr)
    {
        strDebug.AddFormatText(L"<双向链表/list> 总共 %d 个成员:", value->size());
        int32_t i = 0;
        for (auto it = value->begin(); it != value->end(); it++, i++)
        {
            CVolString strBuf;
            PivDumpStr(strBuf, nMaxDumpSize, *it);
            strDebug.AddFormatText(L"\r\n%d. %s", i, strBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::list: nullptr");
}
#endif // _LIST_

// #include <forward_list>
#ifdef _FORWARD_LIST_
template <class T, class Allocator = std::allocator<T>>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, std::forward_list<T, Allocator> *value)
{
    if (value != nullptr)
    {
        strDebug.AddText(L"<单向链表/forward_list>:");
        int32_t i = 0;
        for (auto it = value->begin(); it != value->end(); it++, i++)
        {
            CVolString strBuf;
            PivDumpStr(strBuf, nMaxDumpSize, *it);
            strDebug.AddFormatText(L"\r\n%d. %s", i, strBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::forward_list: nullptr");
}
#endif // _FORWARD_LIST_

// #include <stack>
#ifdef _STACK_
template <class T, class Container = std::deque<T>>
void PivDumpStr(CVolString &strDebug, INT nMaxDumpSize, std::stack<T, Container> *value)
{
    if (value != nullptr)
    {
        if (value->size() > 0)
        {
            CVolString strBuf;
            PivDumpStr(strBuf, nMaxDumpSize, value->top());
            strDebug.AddFormatText(L"<栈/stack> 总共 %d 个成员, 栈顶成员: ", value->size(), strBuf.GetText());
        }
        else
        {
            strDebug.SetText(L"<栈/stack> 总共 0 个成员.");
        }
    }
    else
        strDebug.SetText(L"std::stack: nullptr");
}
#endif // _STACK_

/**
 * @brief 取调试文本
 * @tparam T
 * @param value 欲输出数据
 * @param npNumLeaderSpaces 行首空格数
 * @param strDebug 调试输出文本
 * @return 文本指针
 */
template <class T>
const wchar_t *PivGetDumpStr(T &&value, int npNumLeaderSpaces = 0, CVolString &strDebug = CVolString{})
{
    if (npNumLeaderSpaces == 0)
    {
        PivDumpStr(strDebug, 0, std::forward<T>(value));
    }
    else
    {
        CVolString strBuf;
        PivDumpStr(strBuf, 0, std::forward<T>(value));
        strDebug.AddMutilLineTextWithLeaderSpaces(strBuf, npNumLeaderSpaces);
    }
    return strDebug.GetText();
}

template <typename T, int nArraySize> // 数组专用
const wchar_t *PivGetDumpStr(T _array[nArraySize], CVolString &strDebug = CVolString{})
{
    strDebug.AddChar('{');
    for (int i = 0; i < nArraySize; i++)
    {
        if (i != 0)
            strDebug.AddText(L", ");
        strDebug.AddText(PivGetDumpStr(_array[i]));
    }
    strDebug.AddChar('}');
    return strDebug.GetText();
}

/**
 * @brief 加入调试文本
 * @tparam T
 * @tparam ...Args
 * @param blpStringFormatText 是否以字符串格式输出文本
 * @param nMaxDumpSize 最大允许展示数据尺寸
 * @param npParamTypeIndex 首参数类型索引(必须为0)
 * @param szParamTypes 参数类型文本
 * @param strDebug 调试输出文本
 * @param value 首输出数据
 * @param ...args 欲输出数据(可变参数)
 * @return
 */
template <typename = void>
const TCHAR *PivAddDebugDumpString(const BOOL blpStringFormatText, const INT nMaxDumpSize, INT_P npParamTypeIndex, const TCHAR *szParamTypes, CVolString &strDebug)
{
    return strDebug.GetText();
}

// 指针
template <typename T>
const TCHAR *PivAddDebugDumpString(const BOOL blpStringFormatText, const INT nMaxDumpSize, INT_P npParamTypeIndex, const TCHAR *szParamTypes, CVolString &strDebug, T *value)
{
    if (value == nullptr)
    {
        strDebug.AddText(L"nullptr");
        return strDebug.GetText();
    }
    if (szParamTypes != nullptr)
    {
        switch (szParamTypes[npParamTypeIndex])
        {
        case _C_VOL_STRING:
        {
            if (blpStringFormatText)
            {
                CVolString str;
                strDebug.AddText(MakeStringFromPlainText((const TCHAR *)value, &str, TRUE));
            }
            else
                strDebug.AddText((const TCHAR *)value);
            break;
        }
        case _C_VOL_CLASS:
        {
            CVolString strBuf;
            ((CVolObject *)value)->GetDumpString(strBuf, nMaxDumpSize);
            strDebug.AddText(strBuf);
            break;
        }
        default:
            CVolString strBuf;
            PivDumpStr(strBuf, nMaxDumpSize, value);
            strDebug.AddText(strBuf);
        }
    }
    else
    {
        CVolString strBuf;
        PivDumpStr(strBuf, nMaxDumpSize, value);
        strDebug.AddText(strBuf);
    }
    return strDebug.GetText();
}

// 整数和浮点数
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value, int>::type = 0>
const TCHAR *PivAddDebugDumpString(const BOOL blpStringFormatText, const INT nMaxDumpSize, INT_P npParamTypeIndex, const TCHAR *szParamTypes, CVolString &strDebug, T value)
{
    if (szParamTypes != nullptr && szParamTypes[npParamTypeIndex] == _C_VOL_BOOL)
    {
        strDebug.AddText(value ? L"真" : L"假");
    }
    else
    {
        CVolString strBuf;
        PivDumpStr(strBuf, nMaxDumpSize, value);
        strDebug.AddText(strBuf);
    }
    return strDebug.GetText();
}

// 类和结构体
template <typename T, typename std::enable_if<std::is_class<typename std::decay<T>::type>::value, int>::type = 1>
const TCHAR *PivAddDebugDumpString(const BOOL blpStringFormatText, const INT nMaxDumpSize, INT_P npParamTypeIndex, const TCHAR *szParamTypes, CVolString &strDebug, T &&value)
{
    CVolString strBuf;
    PivDumpStr(strBuf, nMaxDumpSize, &value);
    strDebug.AddText(strBuf);
    return strDebug.GetText();
}

template <typename T, typename... Args>
const TCHAR *PivAddDebugDumpString(const BOOL blpStringFormatText, const INT nMaxDumpSize, INT_P npParamTypeIndex, const TCHAR *szParamTypes, CVolString &strDebug, T &&value, Args &&...args)
{
    PivAddDebugDumpString(blpStringFormatText, nMaxDumpSize, npParamTypeIndex, szParamTypes, strDebug, std::forward<T>(value));
    strDebug.AddText(_T (", "));
    return PivAddDebugDumpString(blpStringFormatText, nMaxDumpSize, npParamTypeIndex + 1, szParamTypes, strDebug, std::forward<Args>(args)...);
}

/**
 * @brief 输出调试
 * @tparam ...Args
 * @param blpStringFormatText 是否以字符串格式输出文本
 * @param nMaxDumpSize 最大允许展示数据尺寸
 * @param npFirstExtendParamTypeIndex 首参数类型索引(必须为0)
 * @param szParamTypes 参数类型文本
 * @param ...args 欲输出数据(可变参数)
 */
template <typename... Args>
void PivDebugTrace(const BOOL blpStringFormatText, const INT nMaxDumpSize, const INT_P npFirstExtendParamTypeIndex, const TCHAR *szParamTypes, Args &&...args)
{
#ifdef _DEBUG
    CVolString strDebug(_T_VOL_DEBUG_OUT_STRING_LEADER);
    PivAddDebugDumpString(blpStringFormatText, nMaxDumpSize, 0, szParamTypes, strDebug, std::forward<Args>(args)...);
    if (strDebug.GetLength() > 0x3FFF)
    {
        DEBUG_PRINT(strDebug.Left(0x3FFF).GetText());
    }
    else
    {
        strDebug.AddText(_T ("\r\n"));
        DEBUG_PRINT(strDebug.GetText());
    }
#endif
}

/**
 * @brief 可跳转输出调试
 * @tparam ...Args
 * @param szSourceFileName 源文件名
 * @param szLineNumber 源码行号
 * @param blpStringFormatText 是否以字符串格式输出文本
 * @param nMaxDumpSize 最大允许展示数据尺寸
 * @param npFirstExtendParamTypeIndex 首参数类型索引(必须为0)
 * @param szParamTypes 参数类型文本
 * @param ...args 欲输出数据(可变参数)
 */
template <typename... Args>
void PivDebugTraceWithSourcePos(const TCHAR *szSourceFileName, const TCHAR *szLineNumber, const BOOL blpStringFormatText,
                                const INT nMaxDumpSize, const INT_P npFirstExtendParamTypeIndex, const TCHAR *szParamTypes, Args &&...args)
{
#ifdef _DEBUG
    CVolString strDebug(_T_VOL_DEBUG_OUT_STRING_LEADER);
    if (IsEmptyStr(szSourceFileName) == FALSE && IsEmptyStr(szLineNumber) == FALSE)
    {
        const INT_P npLineNumber = _ttoi(szLineNumber);
        if (npLineNumber >= 1)
            strDebug.AddFormatText(_T ("<%s>, %d: "), szSourceFileName, (INT)npLineNumber);
    }
    PivAddDebugDumpString(blpStringFormatText, nMaxDumpSize, 0, szParamTypes, strDebug, std::forward<Args>(args)...);
    if (strDebug.GetLength() > 0x3FFF)
        DEBUG_PRINT(strDebug.Left(0x3FFF).GetText());
    else
        DEBUG_PRINT(strDebug.GetText());
#endif
}

/**
 * @brief 输出调试(嵌入代码专用)
 * @tparam ...Args
 * @param ...args 欲输出数据(可变参数)
 */
template <typename... Args>
void PivDebug(Args &&...args)
{
#ifdef _DEBUG
    CVolString strDebug(_T_VOL_DEBUG_OUT_STRING_LEADER);
    PivAddDebugDumpString(FALSE, 0, 0, nullptr, strDebug, std::forward<Args>(args)...);
    if (strDebug.GetLength() > 0x3FFF)
    {
        DEBUG_PRINT(strDebug.Left(0x3FFF).GetText());
    }
    else
    {
        strDebug.AddText(_T ("\r\n"));
        DEBUG_PRINT(strDebug.GetText());
    }
#endif
}

#endif // _PIV_DEBUG_TRACE_HPP