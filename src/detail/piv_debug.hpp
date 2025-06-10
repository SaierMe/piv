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

// 输出指针值
template <typename T, typename std::enable_if<std::is_pointer<T>::value>::type* = nullptr>
inline void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, T _ptr)
{
    TCHAR buf[64];
    buf[0] = '0';
    buf[1] = 'x';
    ToHexStr((unsigned long long)_ptr, sizeof(T) * 2, &buf[2], NUM_ELEMENTS_OF(buf) - 2);
    strDebug.SetText(buf);
}

template <typename T>
inline void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, const T* ptr)
{
    PivDumpStr(strDebug, nMaxDumpSize, const_cast<T*>(ptr));
}

#if defined(_DEBUG) && defined(_PIV_DUMP_ENABLE)

// 输出元素信息
template <typename T>
T PivDumpElementsInfo(CVolString& strDebug, INT nMaxDumpSize, const TCHAR* strName, const T nCount)
{
    T nDumpedCount = static_cast<T>(nMaxDumpSize <= 0 ? 64 : nMaxDumpSize);
    nDumpedCount = static_cast<T>(nDumpedCount < nCount ? nDumpedCount : nCount);
    if (nCount == nDumpedCount)
        strDebug.AddFormatText(L"<%s> 总共 %u 个成员:", strName, nCount);
    else
        strDebug.AddFormatText(L"<%s> 总共 %u 个成员,输出了 %u 个,还有 %u 个未被输出:", strName, nCount, nDumpedCount, nCount - nDumpedCount);
    return nDumpedCount;
}

// 基本类型数组
template <typename T, unsigned N, typename std::enable_if<std::is_fundamental<T>::value>::type* = nullptr>
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, T (&_array)[N])
{
    strDebug.AddChar('{');
    for (unsigned i = 0; i < N; i++)
    {
        if (i != 0)
            strDebug.AddText(L", ");
        strDebug.AddText(PivGetDumpStr(_array[i]));
    }
    strDebug.AddChar('}');
}

// 指针、对象数组
template <typename T, unsigned N, typename std::enable_if<std::is_compound<T>::value>::type* = nullptr>
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, T (&_array)[N])
{
    unsigned nDumpSize = PivDumpElementsInfo(strDebug, nMaxDumpSize, L"基本数组", N);
    for (unsigned i = 0; i < nDumpSize; i++)
    {
        CVolString str;
        PivGetDumpStr(_array[i], 2, str);
        str.TrimLeft();
        strDebug.AddFormatText(L"\r\n%u. %s", i, str.GetText());
    }
}

// 文本型数组
template <unsigned N>
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, CVolString (&_array)[N])
{
    unsigned nDumpSize = PivDumpElementsInfo(strDebug, nMaxDumpSize, L"文本数组", N);
    for (unsigned i = 0; i < nDumpSize; i++)
    {
        CVolString str;
        strDebug.AddFormatText(L"\r\n%u. %s", i, MakeStringFromPlainText((_array[i]).GetText(), &str, TRUE));
    }
}

// 类引用转换到类指针
template <typename T, typename std::enable_if<std::is_class<typename std::decay<T>::type>::value>::type* = nullptr>
inline void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, T& obj)
{
    if (std::is_base_of<CVolObject, std::remove_reference<T>::type>::value)
        PivDumpStr(strDebug, nMaxDumpSize, (CVolObject*)&obj);
    else
        PivDumpStr(strDebug, nMaxDumpSize, &obj);
}

// 火山类调用GetDumpString输出
inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, CVolObject* obj)
{
    if (obj != nullptr)
    {
        CVolString strBuf;
        obj->GetDumpString(strBuf, nMaxDumpSize); // 调用对应对象的取展示内容方法
        strDebug.SetText(strBuf);
    }
    else
    {
        strDebug.SetText(L"nullptr");
    }
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, const wchar_t* value)
{
    strDebug.SetText(value);
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, wchar_t* value)
{
    strDebug.SetText(value);
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, const char* value)
{
    #ifdef _PIV_ENCODING_HPP
    strDebug.SetText(PivAny2W<false>{value}.c_str());
    #else
    strDebug.SetText(value);
    #endif
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, char* value)
{
    #ifdef _PIV_ENCODING_HPP
    strDebug.SetText(PivAny2W<false>{value}.c_str());
    #else
    strDebug.SetText(value);
    #endif
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, wchar_t value)
{
    strDebug.AddChar(value);
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, char value)
{
    strDebug.AddChar(value);
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, bool value)
{
    strDebug.AddText(value ? L"true" : L"false");
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, signed char value)
{
    strDebug.AddIntText(value);
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, unsigned char value)
{
    strDebug.AddIntText(value);
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, short value)
{
    strDebug.AddIntText(value);
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, unsigned short value)
{
    strDebug.AddIntText(value);
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, int value)
{
    strDebug.AddIntText(value);
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, unsigned int value)
{
    strDebug.AddDWordText(value);
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, long value)
{
    strDebug.AddIntText((int)value);
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, unsigned long value)
{
    strDebug.AddDWordText(value);
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, long long value)
{
    strDebug.AddInt64Text(value);
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, unsigned long long value)
{
    strDebug.AddUInt64Text(value);
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, float value)
{
    strDebug.AddFloatText(value);
}

inline static void PivDumpStr(CVolString& strDebug, INT nMaxDumpSiz, double value)
{
    strDebug.AddDoubleText(value);
}

    #include <string>
template <class CharT, class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT>>
inline void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::basic_string<CharT, Traits, Allocator>* str)
{
    if (str != nullptr)
    {
    #ifdef _PIV_ENCODING_HPP
        strDebug.SetText(PivAny2W<false>{*str}.c_str());
    #else
        strDebug.SetText(str->c_str());
    #endif
    }
    else
        strDebug.SetText(L"std::basic_string: nullptr");
}

    #include <vector>
template <class T, class Allocator = std::allocator<T>>
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::vector<T, Allocator>* _array)
{
    if (_array != nullptr)
    {
        size_t nDumpSize = PivDumpElementsInfo(strDebug, nMaxDumpSize, L"动态数组/vector", _array->size());
        for (size_t i = 0; i < nDumpSize; i++)
        {
            CVolString strBuf;
            PivDumpStr(strBuf, nMaxDumpSize, _array->at(i));
            strDebug.AddFormatText(L"\r\n%d. %s", i, strBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::vector: nullptr");
}

    #include <array>
template <class T, std::size_t N>
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::array<T, N>* _array)
{
    if (_array != nullptr)
    {
        size_t nDumpSize = PivDumpElementsInfo(strDebug, nMaxDumpSize, L"定长数组/array", _array->size());
        for (size_t i = 0; i < nDumpSize; i++)
        {
            CVolString strBuf;
            PivDumpStr(strBuf, nMaxDumpSize, _array->at(i));
            strDebug.AddFormatText(L"\r\n%d. %s", i, strBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::array: nullptr");
}

    #include <valarray>
template <class T>
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::valarray<T>* _array)
{
    if (_array == nullptr)
    {
        strDebug.SetText(L"std::valarray: nullptr");
    }
    else
    {
        size_t nDumpSize = PivDumpElementsInfo(strDebug, nMaxDumpSize, L"值数组/valarray", _array->size());
        for (size_t i = 0; i < nDumpSize; ++i)
        {
            CVolString strBuf;
            PivDumpStr(strBuf, nMaxDumpSize, (*_array)[i]);
            strDebug.AddFormatText(L"\r\n%d. %s", i, strBuf.GetText());
        }
    }
}

    #include <utility>
template <class T1, class T2>
inline void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::pair<T1, T2>* _pair)
{
    if (_pair != nullptr)
    {
        CVolString keyBuf, valBuf;
        PivDumpStr(keyBuf, nMaxDumpSize, _pair->first);
        PivDumpStr(valBuf, nMaxDumpSize, _pair->second);
        strDebug.AddFormatText(L"<键值对/pair> 关键字: %s; 对应值: %s", keyBuf.GetText(), valBuf.GetText());
    }
    else
        strDebug.SetText(L"std::pair: nullptr");
}

    #include <map>
template <class Key, class T, class Compare = std::less<Key>, class Allocator = std::allocator<std::pair<const Key, T>>>
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::map<Key, T, Compare, Allocator>* _map)
{
    if (_map != nullptr)
    {
        strDebug.AddFormatText(L"<排序表/map> 总共 %d 个成员:", _map->size());
        int32_t i = 0;
        for (auto& it = _map->begin(); it != _map->end(); it++, i++)
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
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::multimap<Key, T, Compare, Allocator>* _map)
{
    if (_map != nullptr)
    {
        strDebug.AddFormatText(L"<多值排序表/multimap> 总共 %d 个成员:", _map->size());
        int32_t i = 0;
        for (auto& it = _map->begin(); it != _map->end(); it++, i++)
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

    #include <set>
template <class Key, class Compare = std::less<Key>, class Allocator = std::allocator<Key>>
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::set<Key, Compare, Allocator>* _set)
{
    if (_set != nullptr)
    {
        strDebug.AddFormatText(L"<排序集/set> 总共 %d 个成员:", _set->size());
        int32_t i = 0;
        for (auto it = _set->begin(); it != _set->end(); it++, i++)
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
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::multiset<Key, Compare, Allocator>* _set)
{
    if (_set != nullptr)
    {
        strDebug.AddFormatText(L"<多值排序集/multiset> 总共 %d 个成员:", _set->size());
        int32_t i = 0;
        for (auto it = _set->begin(); it != _set->end(); it++, i++)
        {
            CVolString keyBuf;
            PivDumpStr(keyBuf, nMaxDumpSize, *it);
            strDebug.AddFormatText(L"\r\n%d. 关键字: %s\r\n", i, keyBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::multiset: nullptr");
}

    #include <unordered_map>
template <class Key, class T, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>, class Allocator = std::allocator<std::pair<const Key, T>>>
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::unordered_map<Key, T, Hash, KeyEqual, Allocator>* _map)
{
    if (_map != nullptr)
    {
        strDebug.AddFormatText(L"<哈希表/unordered_map> 总共 %d 个成员:", _map->size());
        int32_t i = 0;
        for (auto& it = _map->begin(); it != _map->end(); it++, i++)
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
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::unordered_multimap<Key, T, Hash, KeyEqual, Allocator>* _map)
{
    if (_map != nullptr)
    {
        strDebug.AddFormatText(L"<多值哈希表/unordered_multimap> 总共 %d 个成员:", _map->size());
        int32_t i = 0;
        for (auto& it = _map->begin(); it != _map->end(); it++, i++)
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

    #include <unordered_set>
template <class Key, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>, class Allocator = std::allocator<Key>>
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::unordered_set<Key, Hash, KeyEqual, Allocator>* _set)
{
    if (_set != nullptr)
    {
        strDebug.AddFormatText(L"<哈希集/unordered_set> 总共 %d 个成员:", _set->size());
        int32_t i = 0;
        for (auto it = _set->begin(); it != _set->end(); it++, i++)
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
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::unordered_multiset<Key, Hash, KeyEqual, Allocator>* _set)
{
    if (_set != nullptr)
    {
        strDebug.AddFormatText(L"<多值哈希集/unordered_multiset> 总共 %d 个成员:", _set->size());
        int32_t i = 0;
        for (auto it = _set->begin(); it != _set->end(); it++, i++)
        {
            CVolString keyBuf;
            PivDumpStr(keyBuf, nMaxDumpSize, *it);
            strDebug.AddFormatText(L"\r\n%d. 关键字: %s", i, keyBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::unordered_multiset: nullptr");
}

    #include <queue>
template <class T, class Container = std::deque<T>>
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::queue<T, Container>* _queue)
{
    if (_queue != nullptr)
    {
        if (_queue->empty())
        {
            strDebug.AddFormatText(L"<队列/queue> 当前为空.", _queue->size());
        }
        else
        {
            strDebug.AddFormatText(L"<队列/queue> 总共 %d 个成员:", _queue->size());
            strDebug.AddFormatText(L"\r\n首成员: %s", PivGetDumpStr(_queue->front()));
            strDebug.AddFormatText(L"\r\n尾成员: %s", PivGetDumpStr(_queue->back()));
        }
    }
    else
        strDebug.SetText(L"std::queue: nullptr");
}

template <class T, class Container = std::vector<T>, class Compare = std::less<typename Container::value_type>>
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::priority_queue<T, Container, Compare>* _queue)
{
    if (_queue == nullptr)
    {
        strDebug.SetText(L"std::priority_queue: nullptr");
    }
    else
    {
        if (_queue->empty())
        {
            strDebug.AddFormatText(L"<优先队列(priority_queue)> 当前为空.", _queue->size());
        }
        else
        {
            strDebug.AddFormatText(L"<优先队列(priority_queue)> 总共 %d 个成员:", _queue->size());
            strDebug.AddFormatText(L"\r\n首成员: %s", PivGetDumpStr(_queue->top()));
        }
    }
}

    #include <deque>
template <class T, class Allocator = std::allocator<T>>
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::deque<T, Allocator>* _queue)
{
    if (_queue != nullptr)
    {
        strDebug.AddFormatText(L"<双端队列/deque> 总共 %d 个成员:", _queue->size());
        int32_t i = 0;
        for (auto it = _queue->begin(); it != _queue->end(); it++, i++)
        {
            CVolString strBuf;
            PivDumpStr(strBuf, nMaxDumpSize, *it);
            strDebug.AddFormatText(L"\r\n%d. %s", i, strBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::deque: nullptr");
}

    #include <list>
template <class T, class Allocator = std::allocator<T>>
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::list<T, Allocator>* _list)
{
    if (_list != nullptr)
    {
        strDebug.AddFormatText(L"<双向链表/list> 总共 %d 个成员:", _list->size());
        int32_t i = 0;
        for (auto it = _list->begin(); it != _list->end(); it++, i++)
        {
            CVolString strBuf;
            PivDumpStr(strBuf, nMaxDumpSize, *it);
            strDebug.AddFormatText(L"\r\n%d. %s", i, strBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::list: nullptr");
}

    #include <forward_list>
template <class T, class Allocator = std::allocator<T>>
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::forward_list<T, Allocator>* _list)
{
    if (_list != nullptr)
    {
        strDebug.AddText(L"<单向链表/forward_list>:");
        int32_t i = 0;
        for (auto it = _list->begin(); it != _list->end(); it++, i++)
        {
            CVolString strBuf;
            PivDumpStr(strBuf, nMaxDumpSize, *it);
            strDebug.AddFormatText(L"\r\n%d. %s", i, strBuf.GetText());
        }
    }
    else
        strDebug.SetText(L"std::forward_list: nullptr");
}

    #include <stack>
template <class T, class Container = std::deque<T>>
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::stack<T, Container>* _stack)
{
    if (_stack != nullptr)
    {
        if (_stack->size() > 0)
        {
            CVolString strBuf;
            PivDumpStr(strBuf, nMaxDumpSize, _stack->top());
            strDebug.AddFormatText(L"<栈/stack> 总共 %d 个成员, 栈顶成员: ", _stack->size(), strBuf.GetText());
        }
        else
        {
            strDebug.SetText(L"<栈/stack> 总共 0 个成员.");
        }
    }
    else
        strDebug.SetText(L"std::stack: nullptr");
}

    #include <bitset>
template <std::size_t N>
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::bitset<N>* _bitset)
{
    if (_bitset != nullptr)
        PivDumpStr(strDebug, nMaxDumpSize, _bitset->to_string<wchar_t>().c_str());
    else
        strDebug.SetText(L"std::bitset: nullptr");
}

    #include <memory>
template <class T, class Deleter = std::default_delete<T>>
inline void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::unique_ptr<T, Deleter>* _ptr)
{
    if (_ptr != nullptr && _ptr->get() != nullptr)
        PivDumpStr(strDebug, nMaxDumpSize, _ptr->get());
    else
        strDebug.SetText(L"nullptr");
}

template <class T, class Deleter = std::default_delete<T>>
inline void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::unique_ptr<T[], Deleter>* _ptr)
{
    if (_ptr != nullptr && _ptr->get() != nullptr)
        PivDumpStr(strDebug, nMaxDumpSize, _ptr->get());
    else
        strDebug.SetText(L"nullptr");
}

template <class T>
inline void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::shared_ptr<T>* _ptr)
{
    if (_ptr != nullptr && _ptr->get() != nullptr)
        PivDumpStr(strDebug, nMaxDumpSize, _ptr->get());
    else
        strDebug.SetText(L"nullptr");
}

template <class T>
inline void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::weak_ptr<T>* _ptr)
{
    if (_ptr != nullptr && !_ptr->expired())
        PivDumpStr(strDebug, nMaxDumpSize, _ptr->lock().get());
    else
        strDebug.SetText(L"nullptr");
}

    #include <tuple>
template <typename... T>
inline void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::tuple<T...>* value)
{
    if (value == nullptr)
        strDebug.SetText(L"std::tuple: nullptr");
    else
        strDebug.AddFormatText(L"<元组(tuple)> 总共 %d 个成员.", std::tuple_size<decltype(*value)>::value);
}

    #if defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L) // C++17
        #include <string_view>
template <class CharT, class Traits = std::char_traits<CharT>>
inline void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::basic_string_view<CharT, Traits>* sv)
{
    if (sv != nullptr)
    {
        #ifdef _PIV_ENCODING_HPP
        strDebug.SetText(PivAny2W<false>{*sv}.c_str());
        #else
        strDebug.SetText(sv->data(), sv->size());
        #endif
    }
    else
        strDebug.SetText(L"std::basic_string_view: nullptr");
}

        #include <variant>
template <typename... T>
void PivDumpStr(CVolString& strDebug, INT nMaxDumpSize, std::variant<T...>* value)
{
    if (value == nullptr)
    {
        strDebug.SetText(L"std::variant: nullptr");
    }
    else
    {
        strDebug.AddFormatText(L"<变体(variant)> 当前类型: %d", value->index());
        std::visit([&strDebug, nMaxDumpSize](auto&& arg)
                   {
            CVolString strBuf;
            PivDumpStr(strBuf, nMaxDumpSize, arg);
            strDebug.AddFormatText (L"; 值: %s", strBuf.GetText()); }, *value);
    }
}

    #endif // C++17

#endif // _DEBUG && _PIV_DUMP_ENABLE

/**
 * @brief 取调试文本
 * @tparam T
 * @param value 欲输出数据
 * @param npNumLeaderSpaces 行首空格数
 * @param strDebug 调试输出文本
 * @return 文本指针
 */
template <class T>
inline const wchar_t* PivGetDumpStr(T&& value, int npNumLeaderSpaces = 0, CVolString& strDebug = CVolString{})
{
    PivDumpStr(strDebug, 0, std::forward<T>(value));
    if (npNumLeaderSpaces > 0)
        strDebug.InsertLineBeginLeaderSpaces(npNumLeaderSpaces);
    return strDebug.GetText();
}

template <typename T, unsigned N> // 基本数组专用
inline const wchar_t* PivGetDumpStr(T (&_array)[N], int npNumLeaderSpaces = 0, CVolString& strDebug = CVolString{})
{
    PivDumpStr(strDebug, 0, _array);
    if (npNumLeaderSpaces > 0)
        strDebug.InsertLineBeginLeaderSpaces(npNumLeaderSpaces);
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
inline const TCHAR* PivAddDebugDumpString(const BOOL blpStringFormatText, const INT nMaxDumpSize, INT_P npParamTypeIndex, const TCHAR* szParamTypes, CVolString& strDebug)
{
    return strDebug.GetText();
}

// 指针
template <typename T, typename std::enable_if<std::is_pointer<T>::value>::type* = nullptr>
const TCHAR* PivAddDebugDumpString(const BOOL blpStringFormatText, const INT nMaxDumpSize, INT_P npParamTypeIndex, const TCHAR* szParamTypes, CVolString& strDebug, T value)
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
                strDebug.AddText(MakeStringFromPlainText((const TCHAR*)value, &str, TRUE));
            }
            else
                strDebug.AddText((const TCHAR*)value);
            break;
        }
        case _C_VOL_CLASS:
        {
            CVolString strBuf;
            ((CVolObject*)value)->GetDumpString(strBuf, nMaxDumpSize);
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

// 基本数组
template <typename T, unsigned N>
const TCHAR* PivAddDebugDumpString(const BOOL blpStringFormatText, const INT nMaxDumpSize, INT_P npParamTypeIndex, const TCHAR* szParamTypes, CVolString& strDebug, T (&_array)[N])
{
    CVolString strBuf;
    PivDumpStr(strBuf, nMaxDumpSize, _array);
    strDebug.AddText(strBuf);
    return strDebug.GetText();
}

// 整数和浮点数
template <typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
const TCHAR* PivAddDebugDumpString(const BOOL blpStringFormatText, const INT nMaxDumpSize, INT_P npParamTypeIndex, const TCHAR* szParamTypes, CVolString& strDebug, T value)
{
    if (szParamTypes != nullptr && szParamTypes[npParamTypeIndex] == _C_VOL_BOOL)
    {
        strDebug.AddChar(value ? L'真' : L'假');
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
template <typename T, typename std::enable_if<std::is_class<typename std::decay<T>::type>::value>::type* = nullptr>
const TCHAR* PivAddDebugDumpString(const BOOL blpStringFormatText, const INT nMaxDumpSize, INT_P npParamTypeIndex, const TCHAR* szParamTypes, CVolString& strDebug, T&& value)
{
    CVolString strBuf;
    if (std::is_base_of<CVolObject, std::remove_reference<T>::type>::value)
        PivDumpStr(strBuf, nMaxDumpSize, (CVolObject*)&value);
    else
        PivDumpStr(strBuf, nMaxDumpSize, &value);
    strDebug.AddText(strBuf);
    return strDebug.GetText();
}

// 枚举
template <typename T, typename std::enable_if<std::is_enum<T>::value>::type* = nullptr>
const TCHAR* PivAddDebugDumpString(const BOOL blpStringFormatText, const INT nMaxDumpSize, INT_P npParamTypeIndex, const TCHAR* szParamTypes, CVolString& strDebug, T value)
{
    strDebug.AddIntText((int)value);
    return strDebug.GetText();
}

template <typename T, typename... Args>
const TCHAR* PivAddDebugDumpString(const BOOL blpStringFormatText, const INT nMaxDumpSize, INT_P npParamTypeIndex, const TCHAR* szParamTypes, CVolString& strDebug, T&& value, Args&&... args)
{
    PivAddDebugDumpString(blpStringFormatText, nMaxDumpSize, npParamTypeIndex, szParamTypes, strDebug, std::forward<T>(value));
    strDebug.AddText(_T (", "));
    return PivAddDebugDumpString(blpStringFormatText, nMaxDumpSize, npParamTypeIndex + 1, szParamTypes, strDebug, std::forward<Args>(args)...);
}

/**
 * @brief 取调试文本Ex
 * @tparam ...Args
 * @param blpStringFormatText 是否以字符串格式输出文本
 * @param nMaxDumpSize 最大允许展示数据尺寸
 * @param npParamTypeIndex 首参数类型索引(必须为0)
 * @param szParamTypes 参数类型文本
 * @param strDebug 存放返回的文本
 * @param ...args 欲输出数据(可变参数)
 * @return
 */
template <typename... Args>
CVolString& PivGetDebugDumpString(const BOOL blpStringFormatText, const INT nMaxDumpSize, INT_P npParamTypeIndex, const TCHAR* szParamTypes, CVolString* pstrDebug, Args&&... args)
{
    ASSERT(pstrDebug != NULL);
    pstrDebug->Empty();
#ifdef _DEBUG
    PivAddDebugDumpString(blpStringFormatText, nMaxDumpSize, npParamTypeIndex, szParamTypes, *pstrDebug, std::forward<Args>(args)...);
#endif
    return *pstrDebug;
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
void PivDebugTrace(const BOOL blpStringFormatText, const INT nMaxDumpSize, const INT_P npFirstExtendParamTypeIndex, const TCHAR* szParamTypes, Args&&... args)
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
void PivDebugTraceWithSourcePos(const TCHAR* szSourceFileName, const TCHAR* szLineNumber, const BOOL blpStringFormatText,
                                const INT nMaxDumpSize, const INT_P npFirstExtendParamTypeIndex, const TCHAR* szParamTypes, Args&&... args)
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
        OutputDebugStringW(strDebug.Left(0x3FFF).GetText());
    else
        OutputDebugStringW(strDebug.GetText());
#endif
}

/**
 * @brief 输出调试(嵌入代码专用)
 * @tparam ...Args
 * @param ...args 欲输出数据(可变参数)
 */
template <typename... Args>
void PivDebug(Args&&... args)
{
#ifdef _DEBUG
    CVolString strDebug(_T_VOL_DEBUG_OUT_STRING_LEADER);
    PivAddDebugDumpString(FALSE, 0, 0, nullptr, strDebug, std::forward<Args>(args)...);
    if (strDebug.GetLength() > 0x3FFF)
    {
        OutputDebugStringW(strDebug.Left(0x3FFF).GetText());
    }
    else
    {
        strDebug.AddText(_T ("\r\n"));
        OutputDebugStringW(strDebug.GetText());
    }
#endif
}

#endif // _PIV_DEBUG_TRACE_HPP