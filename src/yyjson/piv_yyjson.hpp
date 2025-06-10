/*********************************************\
 * 火山视窗PIV模块 - yyjson                  *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 仓库: https://github.com/SaierMe/piv      *
\*********************************************/

#ifndef _PIV_YYJSON_HPP
#define _PIV_YYJSON_HPP

#include "../piv_encoding.hpp"
#include <vector>
#include <mutex>
#include <atomic>
#include "yyjson.h"

class piv_yyjson_mut_val;

/**
 * @brief YYJSON内存分配器(单例模式)
 */
class piv_yyjson_alc
{
private:
    std::atomic<yyjson_alc*> m_pref_alc = nullptr;
    std::atomic<yyjson_alc*> m_vol_alc = nullptr;
    std::atomic<yyjson_alc*> m_def_alc = nullptr;
    std::atomic<yyjson_alc*> m_mi_alc = nullptr;
    std::mutex m_mutex;

    piv_yyjson_alc() {}

    ~piv_yyjson_alc()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_vol_alc.load() != nullptr)
            delete m_vol_alc.load();
        if (m_def_alc.load() != nullptr)
            delete m_def_alc.load();
        if (m_mi_alc.load() != nullptr)
            delete m_mi_alc.load();
    }

    piv_yyjson_alc(const piv_yyjson_alc&) = delete;
    piv_yyjson_alc(piv_yyjson_alc&&) = delete;
    piv_yyjson_alc& operator=(const piv_yyjson_alc&) = delete;
    piv_yyjson_alc& operator=(piv_yyjson_alc&&) = delete;

public:
    static piv_yyjson_alc& instance()
    {
        static piv_yyjson_alc inst;
#ifndef MIMALLOC_H
        if (inst.m_vol_alc.load(std::memory_order_acquire) == nullptr)
        {
            inst.set_pref(inst.vol_alc());
        }
#else
        if (inst.m_mi_alc.load(std::memory_order_acquire) == nullptr)
        {
            inst.set_pref(inst.mi_alc());
        }
#endif
        return inst;
    }

    inline yyjson_alc* pref()
    {
        return m_pref_alc.load(std::memory_order_acquire);
    }

    inline void set_pref(yyjson_alc* p)
    {
        return m_pref_alc.store(p, std::memory_order_release);
    }

    yyjson_alc* vol_alc()
    {
        yyjson_alc* p = m_vol_alc.load(std::memory_order_acquire);
        if (p == nullptr)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            p = m_vol_alc.load(std::memory_order_acquire);
            if (p == nullptr)
            {
                p = new yyjson_alc{&vol_malloc, &vol_realloc, &vol_free, g_objVolApp.GetPoolMem()};
                m_vol_alc.store(p, std::memory_order_release);
            }
        }
        return p;
    }

    yyjson_alc* def_alc()
    {
        yyjson_alc* p = m_def_alc.load(std::memory_order_acquire);
        if (p == nullptr)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            p = m_def_alc.load(std::memory_order_acquire);
            if (p == nullptr)
            {
                p = new yyjson_alc{&default_malloc, &default_realloc, &default_free, nullptr};
                m_def_alc.store(p, std::memory_order_release);
            }
        }
        return p;
    }

    yyjson_alc* mi_alc()
    {
#ifdef MIMALLOC_H
        yyjson_alc* p = m_mi_alc.load(std::memory_order_acquire);
        if (p == nullptr)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            p = m_mi_alc.load(std::memory_order_acquire);
            if (p == nullptr)
            {
                p = new yyjson_alc{&mimalloc_malloc, &mimalloc_realloc, &mimalloc_free, nullptr};
                m_mi_alc.store(p, std::memory_order_release);
            }
        }
        return p;
#else
        return nullptr;
#endif
    }

    static void* vol_malloc(void* ctx, size_t size)
    {
        return reinterpret_cast<CPoolMem*>(ctx)->Alloc((INT_P)size);
    }

    static void* vol_realloc(void* ctx, void* ptr, size_t old_size, size_t size)
    {
        return reinterpret_cast<CPoolMem*>(ctx)->Realloc(ptr, (INT_P)size);
    }

    static void vol_free(void* ctx, void* ptr)
    {
        reinterpret_cast<CPoolMem*>(ctx)->Free(ptr);
    }

    static void* default_malloc(void* ctx, size_t size)
    {
        return malloc(size);
    }

    static void* default_realloc(void* ctx, void* ptr, size_t old_size, size_t size)
    {
        return realloc(ptr, size);
    }

    static void default_free(void* ctx, void* ptr)
    {
        free(ptr);
    }

#ifdef MIMALLOC_H
    static void* mimalloc_malloc(void* ctx, size_t size)
    {
        return mi_malloc(size);
    }

    static void* mimalloc__realloc(void* ctx, void* ptr, size_t old_size, size_t size)
    {
        return mi_realloc(ptr, size);
    }

    static void mimalloc__free(void* ctx, void* ptr)
    {
        mi_free(ptr);
    }
#endif

}; // piv_yyjson_alc

/**
 * @brief YYJSON只读值
 */
class piv_yyjson_val : public CVolObject
{
public:
    /**
     * @brief YYJSON只读文档
     */
    class piv_yyjson_doc
    {
    public:
        yyjson_doc* doc = nullptr;

        piv_yyjson_doc() {}

        piv_yyjson_doc(yyjson_doc* rhs)
        {
            doc = rhs;
        }

        ~piv_yyjson_doc()
        {
            free_doc();
        }

        inline void free_doc()
        {
            if (doc)
            {
                yyjson_doc_free(doc);
                doc = nullptr;
            }
        }

        inline bool empty() const
        {
            return doc == nullptr;
        }

        inline yyjson_val* root()
        {
            return yyjson_doc_get_root(doc);
        }
    }; // piv_yyjson_doc

    /**
     * @brief 迭代值容器
     */
    struct iter_cnt
    {
        size_t idx = 0;
        yyjson_val* key = nullptr;
        yyjson_val* val = nullptr;
        piv_yyjson_val* doc = nullptr;

        piv_yyjson_val get_key()
        {
            return doc ? piv_yyjson_val{doc->m_shared_doc ? doc->m_shared_doc : doc->m_weak_doc, key} : piv_yyjson_val{};
        }

        const char* get_key_str()
        {
            return yyjson_get_str(key);
        }

        piv_yyjson_val get_val()
        {
            return doc ? piv_yyjson_val{doc->m_shared_doc ? doc->m_shared_doc : doc->m_weak_doc, val} : piv_yyjson_val{};
        }
    }; // struct iter_cnt

    /**
     * @brief 迭代器
     */
    class iterator
    {
    private:
        iter_cnt value;
        union
        {
            yyjson_obj_iter obj_iter{0};
            yyjson_arr_iter arr_iter;
        };

    public:
        iterator(const iterator& rhs)
        {
            memmove(&obj_iter, &rhs.obj_iter, sizeof(yyjson_arr_iter));
            memmove(&value, &rhs.value, sizeof(iter_cnt));
        }

        iterator(size_t _max_count, size_t _cursor, piv_yyjson_val* _this)
        {
            value.idx = _cursor;
            if (_max_count > 0 && _cursor != _max_count && _this != nullptr)
            {
                value.doc = _this;
                if (_this->is_arr())
                {
                    if (yyjson_arr_iter_init(_this->m_val, &arr_iter))
                    {
                        value.val = yyjson_arr_iter_next(&arr_iter);
                    }
                    else
                    {
                        value.idx = _max_count;
                    }
                }
                else
                {
                    if (yyjson_obj_iter_init(_this->m_val, &obj_iter))
                    {
                        value.key = yyjson_obj_iter_next(&obj_iter);
                        value.val = yyjson_obj_iter_get_val(value.key);
                    }
                    else
                    {
                        value.idx = _max_count;
                    }
                }
            }
        }

        ~iterator() {}

        iter_cnt& operator*()
        {
            return value;
        }

        iter_cnt* operator->()
        {
            return &value;
        }

        bool operator!=(const iterator& rhs)
        {
            return (value.idx != rhs.value.idx);
        }

        iterator& operator++()
        {
            value.idx++;
            if (value.doc)
            {
                if (value.doc->is_arr())
                {
                    value.val = yyjson_arr_iter_next(&arr_iter);
                }
                else
                {
                    value.key = yyjson_obj_iter_next(&obj_iter);
                    value.val = yyjson_obj_iter_get_val(value.key);
                }
            }
            return *this;
        }
    }; // class iterator

private:
    std::shared_ptr<piv_yyjson_doc> m_shared_doc;
    std::weak_ptr<piv_yyjson_doc> m_weak_doc;

    inline piv_yyjson_doc* _doc()
    {
        if (!m_shared_doc)
        {
            if (m_weak_doc.expired())
                return nullptr;
            m_shared_doc = m_weak_doc.lock();
        }
        return m_shared_doc.get();
    }

    inline piv_yyjson_doc* _doc() const
    {
        if (m_shared_doc)
            return m_shared_doc.get();
        if (m_weak_doc.expired())
            return nullptr;
        auto _ptr = m_weak_doc.lock();
        return _ptr.get();
    }

    template <typename T>
    T to_num(yyjson_val* val)
    {
        if (!val)
            return static_cast<T>(0);
        uint8_t tag = unsafe_yyjson_get_tag(val);
        if (tag == (YYJSON_TYPE_NUM | YYJSON_SUBTYPE_REAL))
        {
            return static_cast<T>(val->uni.f64);
        }
        else if (tag == (YYJSON_TYPE_NUM | YYJSON_SUBTYPE_SINT))
        {
            return static_cast<T>(val->uni.i64);
        }
        else if (tag == (YYJSON_TYPE_NUM | YYJSON_SUBTYPE_UINT))
        {
            return static_cast<T>(val->uni.u64);
        }
        return static_cast<T>(0);
    }

public:
    yyjson_val* m_val = nullptr;

    piv_yyjson_val()
    {
        m_shared_doc = std::make_shared<piv_yyjson_doc>();
        m_val = nullptr;
    }

    piv_yyjson_val(const piv_yyjson_val& rhs)
    {
        m_shared_doc = rhs.m_shared_doc;
        m_weak_doc = rhs.m_weak_doc;
        m_val = rhs.m_val;
    }

    piv_yyjson_val(const std::shared_ptr<piv_yyjson_doc>& doc, yyjson_val* val)
    {
        m_weak_doc = doc;
        m_val = val;
    }

    piv_yyjson_val(const std::weak_ptr<piv_yyjson_doc>& doc, yyjson_val* val)
    {
        m_weak_doc = doc;
        m_val = val;
    }

    piv_yyjson_val(yyjson_doc* doc, yyjson_val* val = nullptr)
    {
        m_shared_doc = std::make_shared<piv_yyjson_doc>(doc);
        m_val = val ? val : m_shared_doc->root();
    }

    piv_yyjson_val(yyjson_val* val)
    {
        m_shared_doc = std::make_shared<piv_yyjson_doc>();
        m_val = val;
    }

    ~piv_yyjson_val() {}

    piv_yyjson_val& operator=(const piv_yyjson_val& rhs) noexcept
    {
        m_shared_doc = rhs.m_shared_doc;
        m_weak_doc = rhs.m_weak_doc;
        m_val = rhs.m_val;
        return *this;
    }

    piv_yyjson_val& operator=(piv_yyjson_val&& rhs) noexcept
    {
        m_shared_doc = std::move(rhs.m_shared_doc);
        m_weak_doc = std::move(rhs.m_weak_doc);
        m_val = rhs.m_val;
        return *this;
    }

    bool operator==(const piv_yyjson_val& rhs) const noexcept
    {
        if (!m_shared_doc && m_weak_doc.expired())
            return false;
        return (m_val == rhs.m_val) ? true : yyjson_equals(m_val, rhs.m_val);
    }

    virtual void GetDumpString(CWString& strDump, INT nMaxDumpSize) override
    {
        strDump = to_vol_str(0, piv_yyjson_alc::instance().pref());
    }

    virtual void LoadFromStream(CVolBaseInputStream& stream)
    {
        clear();
        if (stream.eof())
            return;
        size_t len = 0;
        stream.read(&len, 4);
        std::unique_ptr<char[]> data{new char[len]};
        if (stream.ReadExact(data.get(), len))
            parse_doc(data.get(), len, 0, piv_yyjson_alc::instance().pref(), nullptr);
    }

    virtual void SaveIntoStream(CVolBaseOutputStream& stream)
    {
        yyjson_alc* alc = piv_yyjson_alc::instance().pref();
        size_t len;
        char* str = yyjson_write_opts(_doc()->doc, 0, alc, &len, nullptr);
        if (str)
        {
            stream.write(&len, 4);
            stream.write(str, (INT_P)len);
            if (alc == nullptr)
                free(str);
            else
                (alc->free)(alc->ctx, str);
        }
    }

    inline void clear()
    {
        m_shared_doc.reset();
        m_weak_doc.reset();
        m_val = nullptr;
    }

    inline bool parse_doc(const char* dat, size_t len, yyjson_read_flag flag = 0, const yyjson_alc* alc = nullptr, yyjson_read_err* err = nullptr)
    {
        _doc()->free_doc();
        _doc()->doc = yyjson_read_opts((char*)dat, len, flag, alc ? alc : piv_yyjson_alc::instance().pref(), err);
        m_val = _doc()->root();
        return !_doc()->empty();
    }

    template <typename T>
    inline bool parse(T&& dat, yyjson_read_flag flag = 0, const yyjson_alc* alc = nullptr, yyjson_read_err* err = nullptr)
    {
        PivAny2U<true> sv{dat};
        return parse_doc(sv.data(), sv.size(), flag, alc ? alc : piv_yyjson_alc::instance().pref(), err);
    }

    bool parse_file(const wchar_t* filename, yyjson_read_flag flag = 0, const yyjson_alc* alc = nullptr, yyjson_read_err* err = nullptr)
    {
        _doc()->free_doc();
        FILE* fp = nullptr;
        _wfopen_s(&fp, filename, L"rb");
        _doc()->doc = yyjson_read_fp(fp, flag, alc ? alc : piv_yyjson_alc::instance().pref(), err);
        m_val = _doc()->root();
        if (fp)
            fclose(fp);
        return !_doc()->empty();
    }

    bool write_file(const wchar_t* filename, yyjson_write_flag flag = 0, const yyjson_alc* alc = nullptr, yyjson_write_err* err = nullptr)
    {
        FILE* fp = nullptr;
        _wfopen_s(&fp, filename, L"wb+");
        bool ret = yyjson_write_fp(fp, _doc()->doc, flag, alc ? alc : piv_yyjson_alc::instance().pref(), err);
        if (fp)
            fclose(fp);
        return ret;
    }

    CWString write_vol_str(yyjson_write_flag flag = 0, const yyjson_alc* alc = nullptr, yyjson_write_err* err = nullptr)
    {
        if (!alc)
            alc = piv_yyjson_alc::instance().pref();
        CWString ret;
        char* str = yyjson_write_opts(_doc()->doc, flag, alc, nullptr, err);
        if (str)
        {
            ret.SetText(str);
            if (alc == nullptr)
                free(str);
            else
                (alc->free)(alc->ctx, str);
        }
        return ret;
    }

    std::string write_std_str(yyjson_write_flag flag = 0, const yyjson_alc* alc = nullptr, yyjson_write_err* err = nullptr)
    {
        if (!alc)
            alc = piv_yyjson_alc::instance().pref();
        std::string ret;
        char* str = yyjson_write_opts(_doc()->doc, flag, alc, nullptr, err);
        if (str)
        {
            ret.assign(str);
            if (alc == nullptr)
                free(str);
            else
                (alc->free)(alc->ctx, str);
        }
        return ret;
    }

    CVolMem write_vol_mem(yyjson_write_flag flag = 0, const yyjson_alc* alc = nullptr, yyjson_write_err* err = nullptr)
    {
        if (!alc)
            alc = piv_yyjson_alc::instance().pref();
        size_t len;
        CVolMem ret;
        char* str = yyjson_write_opts(_doc()->doc, flag, alc, &len, err);
        if (str)
        {
            ret.Append(str, static_cast<INT_P>(len));
            if (alc == nullptr)
                free(str);
            else
                (alc->free)(alc->ctx, str);
        }
        return ret;
    }

    CWString to_vol_str(yyjson_write_flag flag = 0, const yyjson_alc* alc = nullptr)
    {
        if (!alc)
            alc = piv_yyjson_alc::instance().pref();
        CWString ret;
        char* str = yyjson_val_write_opts(m_val, flag, alc, nullptr, nullptr);
        if (str)
        {
            ret.SetText(str);
            if (alc == nullptr)
                free(str);
            else
                (alc->free)(alc->ctx, str);
        }
        return ret;
    }

    std::string to_std_str(yyjson_write_flag flag = 0, const yyjson_alc* alc = nullptr)
    {
        if (!alc)
            alc = piv_yyjson_alc::instance().pref();
        std::string ret;
        char* str = yyjson_val_write_opts(m_val, flag, alc, nullptr, nullptr);
        if (str)
        {
            ret.assign(str);
            if (alc == nullptr)
                free(str);
            else
                (alc->free)(alc->ctx, str);
        }
        return ret;
    }

    inline piv_yyjson_mut_val to_mut_val();

    inline yyjson_doc* doc()
    {
        return _doc()->doc;
    }

    inline yyjson_doc* doc() const
    {
        return _doc()->doc;
    }

    inline bool empty() const
    {
        return (!m_shared_doc && m_weak_doc.expired());
    }

    inline bool is_root() const
    {
        return m_val == _doc()->root();
    }

    inline bool is_same_doc(const piv_yyjson_val& rhs) const
    {
        return empty() ? false : (_doc() == rhs._doc());
    }

    inline size_t size()
    {
        return yyjson_get_len(m_val);
    }

    inline size_t arr_size()
    {
        return yyjson_arr_size(m_val);
    }

    inline size_t obj_size()
    {
        return yyjson_obj_size(m_val);
    }

    inline uint8_t type()
    {
        return yyjson_get_type(m_val);
    }

    inline uint8_t subtype()
    {
        return yyjson_get_subtype(m_val);
    }

    inline uint8_t tag()
    {
        return yyjson_get_tag(m_val);
    }

    inline const char* desc()
    {
        return yyjson_get_type_desc(m_val);
    }

    inline bool empty_val()
    {
        return !m_val;
    }

    inline bool is_raw()
    {
        return yyjson_is_raw(m_val);
    }

    inline bool is_null()
    {
        return yyjson_is_null(m_val);
    }

    inline bool is_bool()
    {
        return yyjson_is_bool(m_val);
    }

    inline bool is_num()
    {
        return yyjson_is_num(m_val);
    }

    inline bool is_sint()
    {
        return yyjson_is_sint(m_val);
    }

    inline bool is_uint()
    {
        return yyjson_is_uint(m_val);
    }

    inline bool is_real()
    {
        return yyjson_is_real(m_val);
    }

    inline bool is_str()
    {
        return yyjson_is_str(m_val);
    }

    inline bool is_arr()
    {
        return yyjson_is_arr(m_val);
    }

    inline bool is_obj()
    {
        return yyjson_is_obj(m_val);
    }

    inline bool is_ctn()
    {
        return yyjson_is_ctn(m_val);
    }

    template <typename T>
    inline yyjson_val* get_val(T idx,
                               typename std::enable_if<std::is_integral<T>::value, T*>::type = nullptr)
    {
        return yyjson_arr_get(m_val, static_cast<size_t>(idx));
    }

    template <typename T>
    yyjson_val* get_val(T&& key,
                        typename std::enable_if<!std::is_integral<typename std::decay<T>::type>::value,
                                                typename std::decay<T>::type*>::type = nullptr)
    {
        PivAny2U<true> sv{key};
        return yyjson_obj_getn(m_val, sv.data(), sv.size());
    }

    template <typename T>
    inline piv_yyjson_val get(T&& idx_or_key)
    {
        return piv_yyjson_val{m_shared_doc ? m_shared_doc : m_weak_doc, get_val(std::forward<T>(idx_or_key))};
    }

    template <typename T>
    inline piv_yyjson_val operator[](T&& idx_or_key)
    {
        return piv_yyjson_val{m_shared_doc ? m_shared_doc : m_weak_doc, get_val(std::forward<T>(idx_or_key))};
    }

    inline piv_yyjson_val arr_get_first()
    {
        return piv_yyjson_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_arr_get_first(m_val)};
    }

    inline piv_yyjson_val arr_get_last()
    {
        return piv_yyjson_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_arr_get_last(m_val)};
    }

    template <typename T>
    yyjson_val* ptr_val(T&& ptr, yyjson_ptr_err* err = nullptr)
    {
        PivAny2U<true> sv{ptr};
        if (m_val == _doc()->root())
            return yyjson_doc_ptr_getx(_doc()->doc, sv.data(), sv.size(), err);
        else
            return yyjson_ptr_getx(m_val, sv.data(), sv.size(), err);
    }

    template <typename T>
    inline piv_yyjson_val ptr(T&& ptr, yyjson_ptr_err* err = nullptr)
    {
        return piv_yyjson_val{m_shared_doc ? m_shared_doc : m_weak_doc, ptr_val(std::forward<T>(ptr), err)};
    }

    inline const char* get_str()
    {
        return yyjson_get_str(m_val);
    }

    template <typename R>
    inline R get_num()
    {
        return to_num<R>(m_val);
    }

    inline double get_real()
    {
        return yyjson_get_num(m_val);
    }

    inline bool get_bool()
    {
        return yyjson_get_bool(m_val);
    }

    template <typename T>
    inline const char* get_str(T&& idx_or_key)
    {
        return yyjson_get_str(get_val(std::forward<T>(idx_or_key)));
    }

    template <typename R, typename T>
    inline R get_num(T&& idx_or_key)
    {
        return to_num<R>(get_val(std::forward<T>(idx_or_key)));
    }

    template <typename T>
    inline double get_real(T&& idx_or_key)
    {
        return yyjson_get_num(get_val(std::forward<T>(idx_or_key)));
    }

    template <typename T>
    inline bool get_bool(T&& idx_or_key)
    {
        return yyjson_get_bool(get_val(std::forward<T>(idx_or_key)));
    }

    template <typename T>
    inline const char* get_ptr_str(T&& ptr, yyjson_ptr_err* err = nullptr)
    {
        return yyjson_get_str(ptr_val(std::forward<T>(ptr), err));
    }

    template <typename R, typename T>
    inline R get_ptr_num(T&& ptr, yyjson_ptr_err* err = nullptr)
    {
        return to_num<R>(ptr_val(std::forward<T>(ptr), err));
    }

    template <typename T>
    inline double get_ptr_real(T&& ptr, yyjson_ptr_err* err = nullptr)
    {
        return yyjson_get_num(ptr_val(std::forward<T>(ptr), err));
    }

    template <typename T>
    inline bool get_ptr_bool(T&& ptr, yyjson_ptr_err* err = nullptr)
    {
        return yyjson_get_bool(ptr_val(std::forward<T>(ptr), err));
    }

    size_t eume_key(std::vector<std::string>& key_arr)
    {
        yyjson_obj_iter iter;
        if (yyjson_obj_iter_init(m_val, &iter))
        {
            yyjson_val* key = yyjson_obj_iter_next(&iter);
            while (key)
            {
                key_arr.emplace_back(yyjson_get_str(key));
                key = yyjson_obj_iter_next(&iter);
            }
            return key_arr.size();
        }
        return 0;
    }

    INT_P eume_key(CMStringArray& key_arr)
    {
        yyjson_obj_iter iter;
        if (yyjson_obj_iter_init(m_val, &iter))
        {
            yyjson_val* key = yyjson_obj_iter_next(&iter);
            while (key)
            {
                key_arr.Add(yyjson_get_str(key));
                key = yyjson_obj_iter_next(&iter);
            }
            return key_arr.GetCount();
        }
        return 0;
    }

    iterator begin()
    {
        if (yyjson_is_ctn(m_val))
        {
            size_t len = yyjson_get_len(m_val);
            return iterator(len, 0, this);
        }
        return iterator(0, 0, nullptr);
    }

    iterator end()
    {
        if (yyjson_is_ctn(m_val))
        {
            size_t len = yyjson_get_len(m_val);
            return iterator(len, len, nullptr);
        }
        return iterator(0, 0, nullptr);
    }

}; // class piv_yyjson_val

/**
 * @brief YYJSON可变值
 */
class piv_yyjson_mut_val : public CVolObject
{
public:
    /**
     * @brief YYJSON可变文档
     */
    class piv_yyjson_mut_doc
    {
    public:
        yyjson_mut_doc* doc = nullptr;

        piv_yyjson_mut_doc()
        {
            doc = yyjson_mut_doc_new(piv_yyjson_alc::instance().pref());
        }

        piv_yyjson_mut_doc(const yyjson_alc* alc)
        {
            doc = yyjson_mut_doc_new(alc ? alc : piv_yyjson_alc::instance().pref());
        }

        piv_yyjson_mut_doc(yyjson_mut_doc* rhs)
        {
            doc = rhs;
        }

        ~piv_yyjson_mut_doc()
        {
            free_doc();
        }

        inline void free_doc()
        {
            if (doc)
            {
                yyjson_mut_doc_free(doc);
                doc = nullptr;
            }
        }

        inline yyjson_mut_doc* new_doc(const yyjson_alc* alc = nullptr)
        {
            free_doc();
            doc = yyjson_mut_doc_new(alc ? alc : piv_yyjson_alc::instance().pref());
            return doc;
        }

        inline void copy_doc(yyjson_doc* rhs, const yyjson_alc* alc = nullptr)
        {
            free_doc();
            doc = yyjson_doc_mut_copy(rhs, alc ? alc : piv_yyjson_alc::instance().pref());
        }

        inline void copy_doc(yyjson_mut_doc* rhs, const yyjson_alc* alc = nullptr)
        {
            free_doc();
            doc = yyjson_mut_doc_mut_copy(rhs, alc ? alc : piv_yyjson_alc::instance().pref());
        }

        inline bool empty() const
        {
            return doc == nullptr;
        }

        inline yyjson_mut_val* root()
        {
            return yyjson_mut_doc_get_root(doc);
        }

        inline void set_root(yyjson_mut_val* root)
        {
            return yyjson_mut_doc_set_root(doc, root);
        }

        inline bool set_val(yyjson_mut_val* val, bool num, bool deep_copy = true)
        {
            return yyjson_mut_set_bool(val, num);
        }

        inline bool set_val(yyjson_mut_val* val, int32_t num, bool deep_copy = true)
        {
            return yyjson_mut_set_int(val, num);
        }

        inline bool set_val(yyjson_mut_val* val, uint32_t num, bool deep_copy = true)
        {
            return yyjson_mut_set_uint(val, num);
        }

        inline bool set_val(yyjson_mut_val* val, int64_t num, bool deep_copy = true)
        {
            return yyjson_mut_set_sint(val, num);
        }

        inline bool set_val(yyjson_mut_val* val, uint64_t num, bool deep_copy = true)
        {
            return yyjson_mut_set_uint(val, num);
        }

        inline bool set_val(yyjson_mut_val* val, DWORD num, bool deep_copy = true)
        {
            return yyjson_mut_set_uint(val, num);
        }

        inline bool set_val(yyjson_mut_val* val, double num, bool deep_copy = true)
        {
            return yyjson_mut_set_real(val, num);
        }

        inline bool set_val(yyjson_mut_val* val, const char* str, size_t len, bool deep_copy = true)
        {
            if (!val)
                return false;
            const char* new_str = deep_copy ? unsafe_yyjson_mut_strncpy(doc, str, len) : str;
            if (new_str)
            {
                val->tag = ((uint64_t)len << YYJSON_TAG_BIT) | YYJSON_TYPE_STR;
                val->uni.str = new_str;
                return true;
            }
            return false;
        }

        inline bool set_val(yyjson_mut_val* val, const char* str, bool deep_copy = true)
        {
            return set_val(val, str, strlen(str), deep_copy);
        }

        inline bool set_val(yyjson_mut_val* val, const std::string& str, bool deep_copy = true)
        {
            return set_val(val, str.c_str(), str.size(), deep_copy);
        }

        inline bool set_val(yyjson_mut_val* val, const piv::string_view& str, bool deep_copy = true)
        {
            return set_val(val, str.data(), str.size(), deep_copy);
        }

        inline bool set_val(yyjson_mut_val* val, const CVolMem& str, bool deep_copy = true)
        {
            return set_val(val, (const char*)str.GetPtr(), (size_t)str.GetSize(), deep_copy);
        }

        inline bool set_val(yyjson_mut_val* val, const wchar_t* str, bool deep_copy = true)
        {
            PivW2U u8str{str};
            return set_val(val, u8str.c_str(), u8str.size(), true);
        }

        inline bool set_val(yyjson_mut_val* val, const std::wstring& str, bool deep_copy = true)
        {
            PivW2U u8str{str};
            return set_val(val, u8str.c_str(), u8str.size(), true);
        }

        inline bool set_val(yyjson_mut_val* val, const piv::wstring_view& str, bool deep_copy = true)
        {
            PivW2U u8str{str};
            return set_val(val, u8str.c_str(), u8str.size(), true);
        }

        inline bool set_val(yyjson_mut_val* val, const CWString& str, bool deep_copy = true)
        {
            PivW2U u8str{str};
            return set_val(val, u8str.c_str(), u8str.size(), true);
        }

        inline bool set_val(yyjson_mut_val* val, yyjson_val* other_val, bool deep_copy = true)
        {
            if (!val || !other_val)
                return false;
            uint8_t type = unsafe_yyjson_get_type(other_val);
            val->tag = other_val->tag;
            if ((deep_copy && (type == YYJSON_TYPE_RAW || type == YYJSON_TYPE_STR || type)) || type == YYJSON_TYPE_ARR || type == YYJSON_TYPE_OBJ)
            {
                yyjson_mut_val* temp = yyjson_val_mut_copy(doc, other_val);
                if (!temp)
                    return false;
                val->uni = temp->uni;
                return true;
            }
            val->uni = other_val->uni;
            return true;
        }

        inline bool set_val(yyjson_mut_val* val, const piv_yyjson_val& other_val, bool deep_copy = true)
        {
            return this->set_val(val, other_val.m_val, deep_copy);
        }

        inline bool set_val(yyjson_mut_val* val, yyjson_mut_val* other_val, bool deep_copy = true)
        {
            if (!val || !other_val)
                return false;
            uint8_t type = yyjson_mut_get_type(other_val);
            val->tag = other_val->tag;
            if (deep_copy && (type == YYJSON_TYPE_RAW || type == YYJSON_TYPE_STR || type == YYJSON_TYPE_ARR || type == YYJSON_TYPE_OBJ))
            {
                yyjson_mut_val* temp = yyjson_mut_val_mut_copy(doc, other_val);
                if (!temp)
                    return false;
                val->uni = temp->uni;
            }
            val->uni = other_val->uni;
            return true;
        }

        inline bool set_val(yyjson_mut_val* val, const piv_yyjson_mut_val& other_val, bool deep_copy = true)
        {
            return this->set_val(val, other_val.m_val, other_val.doc() == doc ? false : deep_copy);
        }

        inline yyjson_mut_val* to_val(bool num, bool deep_copy = true)
        {
            return yyjson_mut_bool(doc, num);
        }

        inline yyjson_mut_val* to_val(int32_t num, bool deep_copy = true)
        {
            return yyjson_mut_sint(doc, num);
        }

        inline yyjson_mut_val* to_val(uint32_t num, bool deep_copy = true)
        {
            return yyjson_mut_uint(doc, num);
        }

        inline yyjson_mut_val* to_val(int64_t num, bool deep_copy = true)
        {
            return yyjson_mut_sint(doc, num);
        }

        inline yyjson_mut_val* to_val(uint64_t num, bool deep_copy = true)
        {
            return yyjson_mut_uint(doc, num);
        }

        inline yyjson_mut_val* to_val(DWORD num, bool deep_copy = true)
        {
            return yyjson_mut_uint(doc, num);
        }

        inline yyjson_mut_val* to_val(double num, bool deep_copy = true)
        {
            return yyjson_mut_real(doc, num);
        }

        inline yyjson_mut_val* to_val(yyjson_val* val, bool deep_copy = true)
        {
            return yyjson_val_mut_copy(doc, val);
        }

        inline yyjson_mut_val* to_val(yyjson_mut_val* val, bool deep_copy = true)
        {
            if (deep_copy)
                return yyjson_mut_val_mut_copy(doc, val);
            if (!doc)
                return nullptr;
            yyjson_mut_val* new_val = unsafe_yyjson_mut_val(doc, 1);
            new_val->tag = val->tag;
            new_val->uni = val->uni;
            return new_val;
        }

        inline yyjson_mut_val* to_val(const piv_yyjson_val& val, bool deep_copy = true)
        {
            return yyjson_val_mut_copy(doc, val.m_val);
        }

        inline yyjson_mut_val* to_val(const piv_yyjson_mut_val& val, bool deep_copy = true)
        {
            return this->to_val(val.m_val, deep_copy);
        }

        inline yyjson_mut_val* to_val(const char* str, size_t len, bool deep_copy = true)
        {
            return deep_copy ? yyjson_mut_strncpy(doc, str, len) : yyjson_mut_strn(doc, str, len);
        }

        inline yyjson_mut_val* to_val(const char* str, bool deep_copy = true)
        {
            return deep_copy ? yyjson_mut_strcpy(doc, str) : yyjson_mut_str(doc, str);
        }

        inline yyjson_mut_val* to_val(const std::string& str, bool deep_copy = true)
        {
            return deep_copy ? yyjson_mut_strncpy(doc, str.c_str(), str.size()) : yyjson_mut_strn(doc, str.c_str(), str.size());
        }

        inline yyjson_mut_val* to_val(const piv::string_view& str, bool deep_copy = true)
        {
            return deep_copy ? yyjson_mut_strncpy(doc, str.data(), str.size()) : yyjson_mut_strn(doc, str.data(), str.size());
        }

        inline yyjson_mut_val* to_val(const CVolMem& str, bool deep_copy = true)
        {
            return deep_copy ? yyjson_mut_strncpy(doc, (const char*)str.GetPtr(), (size_t)str.GetSize()) : yyjson_mut_strn(doc, (const char*)str.GetPtr(), (size_t)str.GetSize());
        }

        inline yyjson_mut_val* to_val(const wchar_t* str, bool deep_copy = true)
        {
            PivW2U u8str{str};
            return yyjson_mut_strncpy(doc, u8str.c_str(), u8str.size());
        }

        inline yyjson_mut_val* to_val(const std::wstring& str, bool deep_copy = true)
        {
            PivW2U u8str{str};
            return yyjson_mut_strncpy(doc, u8str.c_str(), u8str.size());
        }

        inline yyjson_mut_val* to_val(const piv::wstring_view& str, bool deep_copy = true)
        {
            PivW2U u8str{str};
            return yyjson_mut_strncpy(doc, u8str.c_str(), u8str.size());
        }

        inline yyjson_mut_val* to_val(const CWString& str, bool deep_copy = true)
        {
            PivW2U u8str{str};
            return yyjson_mut_strncpy(doc, u8str.c_str(), u8str.size());
        }

        inline yyjson_mut_val* to_key(const char* str, size_t len, bool deep_copy = true)
        {
            return deep_copy ? yyjson_mut_strncpy(doc, str, len) : yyjson_mut_strn(doc, str, len);
        }

        inline yyjson_mut_val* to_key(const char* str, bool deep_copy = true)
        {
            return yyjson_mut_str(doc, str); // 改为浅拷贝
            // return deep_copy ? yyjson_mut_strcpy(doc, str) : yyjson_mut_str(doc, str);
        }

        inline yyjson_mut_val* to_key(const std::string& str, bool deep_copy = true)
        {
            return deep_copy ? yyjson_mut_strncpy(doc, str.c_str(), str.size()) : yyjson_mut_strn(doc, str.c_str(), str.size());
        }

        inline yyjson_mut_val* to_key(const piv::string_view& str, bool deep_copy = true)
        {
            return deep_copy ? yyjson_mut_strncpy(doc, str.data(), str.size()) : yyjson_mut_strn(doc, str.data(), str.size());
        }

        inline yyjson_mut_val* to_key(const CVolMem& str, bool deep_copy = true)
        {
            return deep_copy ? yyjson_mut_strncpy(doc, (const char*)str.GetPtr(), (size_t)str.GetSize()) : yyjson_mut_strn(doc, (const char*)str.GetPtr(), (size_t)str.GetSize());
        }

        inline yyjson_mut_val* to_key(const wchar_t* str, bool deep_copy = true)
        {
            PivW2U u8str{str};
            return yyjson_mut_strncpy(doc, u8str.c_str(), u8str.size());
        }

        inline yyjson_mut_val* to_key(const std::wstring& str, bool deep_copy = true)
        {
            PivW2U u8str{str};
            return yyjson_mut_strncpy(doc, u8str.c_str(), u8str.size());
        }

        inline yyjson_mut_val* to_key(const piv::wstring_view& str, bool deep_copy = true)
        {
            PivW2U u8str{str};
            return yyjson_mut_strncpy(doc, u8str.c_str(), u8str.size());
        }

        inline yyjson_mut_val* to_key(const CWString& str, bool deep_copy = true)
        {
            PivW2U u8str{str};
            return yyjson_mut_strncpy(doc, u8str.c_str(), u8str.size());
        }

        template <typename T>
        inline bool append(yyjson_mut_val* _arr, bool deep_copy, T&& val)
        {
            return yyjson_mut_arr_append(_arr, to_val(std::forward<T>(val), deep_copy));
        }

        template <typename T, typename... Args>
        inline bool append(yyjson_mut_val* _arr, bool deep_copy, T&& val, Args&&... args)
        {
            if (!yyjson_mut_arr_append(_arr, to_val(std::forward<T>(val), deep_copy)))
                return false;
            return append(_arr, deep_copy, std::forward<Args>(args)...);
        }

        template <typename T>
        inline bool prepend(yyjson_mut_val* _arr, bool deep_copy, T&& val)
        {
            return yyjson_mut_arr_prepend(_arr, to_val(std::forward<T>(val), deep_copy));
        }

        template <typename T, typename... Args>
        inline bool prepend(yyjson_mut_val* _arr, bool deep_copy, T&& val, Args&&... args)
        {
            if (!yyjson_mut_arr_prepend(_arr, to_val(std::forward<T>(val), deep_copy)))
                return false;
            return prepend(_arr, deep_copy, std::forward<Args>(args)...);
        }

        template <typename K>
        inline bool add(yyjson_mut_val* _obj, bool deep_copy, bool replaced, K&& key)
        {
            if (replaced)
                return yyjson_mut_obj_put(_obj, to_key(std::forward<K>(key), deep_copy), yyjson_mut_null(doc));
            else
                return yyjson_mut_obj_add(_obj, to_key(std::forward<K>(key), deep_copy), yyjson_mut_null(doc));
        }

        template <typename K, typename V>
        inline bool add(yyjson_mut_val* _obj, bool deep_copy, bool replaced, K&& key, V&& val)
        {
            if (replaced)
                return yyjson_mut_obj_put(_obj, to_key(std::forward<K>(key), deep_copy), to_val(std::forward<V>(val), deep_copy));
            else
                return yyjson_mut_obj_add(_obj, to_key(std::forward<K>(key), deep_copy), to_val(std::forward<V>(val), deep_copy));
        }

        template <typename K, typename V, typename... Args>
        inline bool add(yyjson_mut_val* _obj, bool deep_copy, bool replaced, K&& key, V&& val, Args&&... args)
        {
            bool ret;
            if (replaced)
                ret = yyjson_mut_obj_put(_obj, to_key(std::forward<K>(key), deep_copy), to_val(std::forward<V>(val), deep_copy));
            else
                ret = yyjson_mut_obj_add(_obj, to_key(std::forward<K>(key), deep_copy), to_val(std::forward<V>(val), deep_copy));
            if (!ret)
                return false;
            return add(_obj, deep_copy, replaced, std::forward<Args>(args)...);
        }
    }; // piv_yyjson_mut_doc

    /**
     * @brief 迭代值容器
     */
    struct iter_cnt
    {
        size_t idx = 0;
        yyjson_mut_val* key = nullptr;
        yyjson_mut_val* val = nullptr;
        piv_yyjson_mut_val* doc = nullptr;

        piv_yyjson_mut_val get_key()
        {
            return doc ? piv_yyjson_mut_val{doc->m_shared_doc ? doc->m_shared_doc : doc->m_weak_doc, key} : piv_yyjson_mut_val{};
        }

        const char* get_key_str()
        {
            return yyjson_mut_get_str(key);
        }

        piv_yyjson_mut_val get_val()
        {
            return doc ? piv_yyjson_mut_val{doc->m_shared_doc ? doc->m_shared_doc : doc->m_weak_doc, val} : piv_yyjson_mut_val{};
        }
    }; // struct iter_cnt

    /**
     * @brief 迭代器
     */
    class iterator
    {
    private:
        iter_cnt value;
        union
        {
            yyjson_mut_obj_iter obj_iter{0};
            yyjson_mut_arr_iter arr_iter;
        };

    public:
        iterator(const iterator& rhs)
        {
            memmove(&arr_iter, &rhs.arr_iter, sizeof(yyjson_mut_arr_iter));
            memmove(&value, &rhs.value, sizeof(iter_cnt));
        }

        iterator(size_t _max_count, size_t _cursor, piv_yyjson_mut_val* _this)
        {
            value.idx = _cursor;
            if (_max_count > 0 && _cursor != _max_count && _this != nullptr)
            {
                value.doc = _this;
                if (_this->is_arr())
                {
                    if (yyjson_mut_arr_iter_init(_this->m_val, &arr_iter))
                    {
                        value.val = yyjson_mut_arr_iter_next(&arr_iter);
                    }
                    else
                    {
                        value.idx = _max_count;
                    }
                }
                else
                {
                    if (yyjson_mut_obj_iter_init(_this->m_val, &obj_iter))
                    {
                        value.key = yyjson_mut_obj_iter_next(&obj_iter);
                        value.val = yyjson_mut_obj_iter_get_val(value.key);
                    }
                    else
                    {
                        value.idx = _max_count;
                    }
                }
            }
        }

        iter_cnt& operator*()
        {
            return value;
        }

        iter_cnt* operator->()
        {
            return &value;
        }

        bool operator!=(const iterator& rhs)
        {
            return (value.idx != rhs.value.idx);
        }

        iterator& operator++()
        {
            value.idx++;
            if (value.doc)
            {
                if (value.doc->is_arr())
                {
                    value.val = yyjson_mut_arr_iter_next(&arr_iter);
                }
                else
                {
                    value.key = yyjson_mut_obj_iter_next(&obj_iter);
                    value.val = yyjson_mut_obj_iter_get_val(value.key);
                }
            }
            return *this;
        }
    }; // class iterator

    /**
     * @brief JSON路径上下文
     */
    struct ptr_ctx
    {
        std::weak_ptr<piv_yyjson_mut_doc> doc;
        yyjson_ptr_ctx ctx;

        ptr_ctx()
        {
            memset(&ctx, 0, sizeof(yyjson_ptr_ctx));
        }

        template <typename V>
        inline bool append(V&& value, bool deep_copy = true)
        {
            return yyjson_ptr_ctx_append(&ctx, nullptr, doc.lock()->to_val(std::forward<V>(value), deep_copy));
        }

        template <typename K, typename V>
        inline bool append(K&& key, V&& value, bool deep_copy = true)
        {
            auto _doc = doc.lock();
            return yyjson_ptr_ctx_append(&ctx, _doc->to_key(std::forward<K>(key), deep_copy), _doc->to_val(std::forward<V>(value), deep_copy));
        }

        template <typename V>
        inline bool replace(V&& value)
        {
            return yyjson_ptr_ctx_replace(&ctx, doc.lock()->to_val(std::forward<V>(value)));
        }

        inline bool remove()
        {
            return yyjson_ptr_ctx_remove(&ctx);
        }
    }; // ptr_ctx

private:
    yyjson_mut_val* m_val = nullptr;
    std::shared_ptr<piv_yyjson_mut_doc> m_shared_doc;
    std::weak_ptr<piv_yyjson_mut_doc> m_weak_doc;

    inline piv_yyjson_mut_doc* _doc()
    {
        if (!m_shared_doc)
        {
            if (m_weak_doc.expired())
                return nullptr;
            m_shared_doc = m_weak_doc.lock();
        }
        return m_shared_doc.get();
    }

    inline piv_yyjson_mut_doc* _doc() const
    {
        if (m_shared_doc)
            return m_shared_doc.get();
        if (m_weak_doc.expired())
            return nullptr;
        auto _ptr = m_weak_doc.lock();
        return _ptr.get();
    }

    template <typename T>
    T to_num(yyjson_mut_val* val)
    {
        if (!val)
            return static_cast<T>(0);
        uint8_t tag = unsafe_yyjson_get_tag(val);
        if (tag == (YYJSON_TYPE_NUM | YYJSON_SUBTYPE_REAL))
        {
            return static_cast<T>(val->uni.f64);
        }
        else if (tag == (YYJSON_TYPE_NUM | YYJSON_SUBTYPE_SINT))
        {
            return static_cast<T>(val->uni.i64);
        }
        else if (tag == (YYJSON_TYPE_NUM | YYJSON_SUBTYPE_UINT))
        {
            return static_cast<T>(val->uni.u64);
        }
        return static_cast<T>(0);
    }

public:
    piv_yyjson_mut_val()
    {
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_mut_null(m_shared_doc->doc);
        m_shared_doc->set_root(m_val);
    }

    ~piv_yyjson_mut_val() {}

    piv_yyjson_mut_val(const piv_yyjson_mut_val& rhs)
    {
        m_shared_doc = rhs.m_shared_doc;
        m_weak_doc = rhs.m_weak_doc;
        m_val = rhs.m_val;
    }

    piv_yyjson_mut_val(const std::shared_ptr<piv_yyjson_mut_doc>& doc, yyjson_mut_val* val)
    {
        m_weak_doc = doc;
        m_val = val;
    }

    piv_yyjson_mut_val(const std::weak_ptr<piv_yyjson_mut_doc>& doc, yyjson_mut_val* val)
    {
        m_weak_doc = doc;
        m_val = val;
    }

    piv_yyjson_mut_val(yyjson_mut_val* val)
    {
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_mut_val_mut_copy(m_shared_doc->doc, val);
        m_shared_doc->set_root(m_val);
    }

    piv_yyjson_mut_val(yyjson_val* val)
    {
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_val_mut_copy(m_shared_doc->doc, val);
        m_shared_doc->set_root(m_val);
    }

    piv_yyjson_mut_val(bool num)
    {
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_mut_bool(m_shared_doc->doc, num);
        m_shared_doc->set_root(m_val);
    }

    piv_yyjson_mut_val(int32_t num)
    {
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_mut_sint(m_shared_doc->doc, num);
        m_shared_doc->set_root(m_val);
    }

    piv_yyjson_mut_val(uint32_t num)
    {
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_mut_uint(m_shared_doc->doc, num);
        m_shared_doc->set_root(m_val);
    }

    piv_yyjson_mut_val(int64_t num)
    {
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_mut_sint(m_shared_doc->doc, num);
        m_shared_doc->set_root(m_val);
    }

    piv_yyjson_mut_val(uint64_t num)
    {
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_mut_uint(m_shared_doc->doc, num);
        m_shared_doc->set_root(m_val);
    }

    piv_yyjson_mut_val(DWORD num)
    {
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_mut_uint(m_shared_doc->doc, num);
        m_shared_doc->set_root(m_val);
    }

    piv_yyjson_mut_val(double num)
    {
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_mut_real(m_shared_doc->doc, num);
        m_shared_doc->set_root(m_val);
    }

    piv_yyjson_mut_val(const char* str, size_t len = -1)
    {
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_mut_strncpy(m_shared_doc->doc, str, len == -1 ? strlen(str) : len);
        m_shared_doc->set_root(m_val);
    }

    piv_yyjson_mut_val(const std::string& str)
    {
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_mut_strncpy(m_shared_doc->doc, str.c_str(), str.size());
        m_shared_doc->set_root(m_val);
    }

    piv_yyjson_mut_val(const piv::string_view& str)
    {
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_mut_strncpy(m_shared_doc->doc, str.data(), str.size());
        m_shared_doc->set_root(m_val);
    }

    piv_yyjson_mut_val(const CVolMem& str)
    {
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_mut_strncpy(m_shared_doc->doc, (const char*)str.GetPtr(), (size_t)str.GetSize());
        m_shared_doc->set_root(m_val);
    }

    piv_yyjson_mut_val(const wchar_t* str, size_t len = -1)
    {
        PivW2U u8str{str, len};
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_mut_strncpy(m_shared_doc->doc, u8str.c_str(), u8str.size());
        m_shared_doc->set_root(m_val);
    }

    piv_yyjson_mut_val(const std::wstring& str)
    {
        PivW2U u8str{str};
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_mut_strncpy(m_shared_doc->doc, u8str.c_str(), u8str.size());
        m_shared_doc->set_root(m_val);
    }

    piv_yyjson_mut_val(const piv::wstring_view& str)
    {
        PivW2U u8str{str};
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_mut_strncpy(m_shared_doc->doc, u8str.c_str(), u8str.size());
        m_shared_doc->set_root(m_val);
    }

    piv_yyjson_mut_val(const CWString& str)
    {
        PivW2U u8str{str};
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_mut_strncpy(m_shared_doc->doc, u8str.c_str(), u8str.size());
        m_shared_doc->set_root(m_val);
    }

    piv_yyjson_mut_val& operator=(const piv_yyjson_mut_val& rhs)
    {
        m_shared_doc = rhs.m_shared_doc;
        m_weak_doc = rhs.m_weak_doc;
        m_val = rhs.m_val;
        return *this;
    }

    piv_yyjson_mut_val& operator=(piv_yyjson_mut_val&& rhs) noexcept
    {
        m_shared_doc = std::move(rhs.m_shared_doc);
        m_weak_doc = std::move(rhs.m_weak_doc);
        m_val = rhs.m_val;
        return *this;
    }

    bool operator==(const piv_yyjson_mut_val& rhs) const noexcept
    {
        return (m_val == rhs.m_val) ? true : yyjson_mut_equals(m_val, rhs.m_val);
    }

    virtual void GetDumpString(CWString& strDump, INT nMaxDumpSize) override
    {
        strDump = to_vol_str(0, piv_yyjson_alc::instance().pref());
    }

    virtual void LoadFromStream(CVolBaseInputStream& stream)
    {
        clear();
        if (stream.eof())
            return;
        size_t len = 0;
        stream.read(&len, 4);
        std::unique_ptr<char[]> data{new char[len]};
        if (stream.ReadExact(data.get(), len))
            parse_doc(data.get(), len, 0, piv_yyjson_alc::instance().pref(), nullptr);
    }

    virtual void SaveIntoStream(CVolBaseOutputStream& stream)
    {
        yyjson_alc* alc = piv_yyjson_alc::instance().pref();
        size_t len;
        char* str = yyjson_mut_write_opts(_doc()->doc, 0, alc, &len, nullptr);
        if (str)
        {
            stream.write(&len, 4);
            stream.write(str, (INT_P)len);
            if (alc == nullptr)
                free(str);
            else
                (alc->free)(alc->ctx, str);
        }
    }

    inline piv_yyjson_mut_val make_val(bool num)
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_bool(_doc()->doc, num)};
    }

    inline piv_yyjson_mut_val make_val(int32_t num)
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_sint(_doc()->doc, num)};
    }

    inline piv_yyjson_mut_val make_val(uint32_t num)
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_uint(_doc()->doc, num)};
    }

    inline piv_yyjson_mut_val make_val(int64_t num)
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_sint(_doc()->doc, num)};
    }

    inline piv_yyjson_mut_val make_val(uint64_t num)
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_uint(_doc()->doc, num)};
    }

    inline piv_yyjson_mut_val make_val(DWORD num)
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_uint(_doc()->doc, num)};
    }

    inline piv_yyjson_mut_val make_val(double num)
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_real(_doc()->doc, num)};
    }

    inline piv_yyjson_mut_val make_val(const char* str, size_t len = -1)
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_strncpy(_doc()->doc, str, len == -1 ? strlen(str) : len)};
    }

    inline piv_yyjson_mut_val make_val(const std::string& str)
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_strncpy(_doc()->doc, str.c_str(), str.size())};
    }

    inline piv_yyjson_mut_val make_val(const piv::string_view& str)
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_strncpy(_doc()->doc, str.data(), str.size())};
    }

    inline piv_yyjson_mut_val make_val(const CVolMem& str)
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_strncpy(_doc()->doc, (const char*)str.GetPtr(), (size_t)str.GetSize())};
    }

    inline piv_yyjson_mut_val make_val(const wchar_t* str, size_t len = -1)
    {
        PivW2U u8str{str, len};
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_strncpy(_doc()->doc, u8str.c_str(), u8str.size())};
    }

    inline piv_yyjson_mut_val make_val(const std::wstring& str)
    {
        PivW2U u8str{str};
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_strncpy(_doc()->doc, u8str.c_str(), u8str.size())};
    }

    inline piv_yyjson_mut_val make_val(const piv::wstring_view& str)
    {
        PivW2U u8str{str};
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_strncpy(_doc()->doc, u8str.c_str(), u8str.size())};
    }

    inline piv_yyjson_mut_val make_val(const CWString& str)
    {
        PivW2U u8str{str};
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_strncpy(_doc()->doc, u8str.c_str(), u8str.size())};
    }

    inline piv_yyjson_mut_val make_null()
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_null(_doc()->doc)};
    }

    inline piv_yyjson_mut_val make_obj()
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_obj(_doc()->doc)};
    }

    template <typename... Args>
    inline piv_yyjson_mut_val make_obj(bool deep_copy, bool replaced, Args&&... args)
    {
        yyjson_mut_val* _obj = yyjson_mut_obj(_doc()->doc);
        _doc()->add(_obj, deep_copy, replaced, std::forward<Args>(args)...);
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, _obj};
    }

    inline piv_yyjson_mut_val make_arr()
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_arr(_doc()->doc)};
    }

    template <typename... Args>
    inline piv_yyjson_mut_val make_arr(bool deep_copy, Args&&... args)
    {
        yyjson_mut_val* _arr = yyjson_mut_arr(_doc()->doc);
        _doc()->append(_arr, deep_copy, std::forward<Args>(args)...);
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, _arr};
    }

    inline piv_yyjson_mut_val& copy(const piv_yyjson_mut_val& rhs)
    {
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = yyjson_mut_val_mut_copy(_doc()->doc, rhs.m_val);
        _doc()->set_root(m_val);
        return *this;
    }

    inline bool set_str_pool_size(size_t len)
    {
        return yyjson_mut_doc_set_str_pool_size(_doc()->doc, len);
    }

    inline bool set_val_pool_size(size_t count)
    {
        return yyjson_mut_doc_set_val_pool_size(_doc()->doc, count);
    }

    inline void clear()
    {
        m_weak_doc.reset();
        m_shared_doc = std::make_shared<piv_yyjson_mut_doc>();
        m_val = nullptr;
    }

    inline bool parse_doc(const char* dat, size_t len, yyjson_read_flag flag = 0, const yyjson_alc* alc = nullptr, yyjson_read_err* err = nullptr)
    {
        _doc()->free_doc();
        yyjson_doc* imut_doc = yyjson_read_opts((char*)dat, len, flag, alc ? alc : piv_yyjson_alc::instance().pref(), err);
        if (imut_doc)
        {
            _doc()->copy_doc(imut_doc, alc);
            yyjson_doc_free(imut_doc);
        }
        m_val = _doc()->root();
        return !_doc()->empty();
    }

    template <typename T>
    inline bool parse(T&& dat, yyjson_read_flag flag = 0, const yyjson_alc* alc = nullptr, yyjson_read_err* err = nullptr)
    {
        PivAny2U<true> sv{dat};
        return parse_doc(sv.data(), sv.size(), flag, alc ? alc : piv_yyjson_alc::instance().pref(), err);
    }

    bool parse_file(const wchar_t* filename, yyjson_read_flag flag = 0, const yyjson_alc* alc = nullptr, yyjson_read_err* err = nullptr)
    {
        _doc()->free_doc();
        if (!alc)
            alc = piv_yyjson_alc::instance().pref();
        FILE* fp = nullptr;
        _wfopen_s(&fp, filename, L"rb");
        yyjson_doc* imut_doc = yyjson_read_fp(fp, flag, alc, err);
        if (imut_doc)
        {
            _doc()->copy_doc(imut_doc, alc);
            yyjson_doc_free(imut_doc);
        }
        m_val = _doc()->root();
        if (fp)
            fclose(fp);
        return !_doc()->empty();
    }

    bool write_file(const wchar_t* filename, yyjson_write_flag flag = 0, const yyjson_alc* alc = nullptr, yyjson_write_err* err = nullptr)
    {
        FILE* fp = nullptr;
        _wfopen_s(&fp, filename, L"wb+");
        bool ret = yyjson_mut_write_fp(fp, _doc()->doc, flag, alc ? alc : piv_yyjson_alc::instance().pref(), err);
        if (fp)
            fclose(fp);
        return ret;
    }

    CWString write_vol_str(yyjson_write_flag flag = 0, const yyjson_alc* alc = nullptr, yyjson_write_err* err = nullptr)
    {
        if (!alc)
            alc = piv_yyjson_alc::instance().pref();
        CWString ret;
        char* str = yyjson_mut_write_opts(_doc()->doc, flag, alc, nullptr, err);
        if (str)
        {
            ret.SetText(str);
            if (alc == nullptr)
                free(str);
            else
                (alc->free)(alc->ctx, str);
        }
        return ret;
    }

    std::string write_std_str(yyjson_write_flag flag = 0, const yyjson_alc* alc = nullptr, yyjson_write_err* err = nullptr)
    {
        if (!alc)
            alc = piv_yyjson_alc::instance().pref();
        std::string ret;
        char* str = yyjson_mut_write_opts(_doc()->doc, flag, alc, nullptr, err);
        if (str)
        {
            ret.assign(str);
            if (alc == nullptr)
                free(str);
            else
                (alc->free)(alc->ctx, str);
        }
        return ret;
    }

    CVolMem write_vol_mem(yyjson_write_flag flag = 0, const yyjson_alc* alc = nullptr, yyjson_write_err* err = nullptr)
    {
        if (!alc)
            alc = piv_yyjson_alc::instance().pref();
        size_t len;
        CVolMem ret;
        char* str = yyjson_mut_write_opts(_doc()->doc, flag, alc, &len, err);
        if (str)
        {
            ret.Append(str, static_cast<INT_P>(len));
            if (alc == nullptr)
                free(str);
            else
                (alc->free)(alc->ctx, str);
        }
        return ret;
    }

    CWString to_vol_str(yyjson_write_flag flag = 0, const yyjson_alc* alc = nullptr)
    {
        if (!alc)
            alc = piv_yyjson_alc::instance().pref();
        CWString ret;
        char* str = yyjson_mut_val_write_opts(m_val, flag, alc, nullptr, nullptr);
        if (str)
        {
            ret.SetText(str);
            if (alc == nullptr)
                free(str);
            else
                (alc->free)(alc->ctx, str);
        }
        return ret;
    }

    std::string to_std_str(yyjson_write_flag flag = 0, const yyjson_alc* alc = nullptr)
    {
        if (!alc)
            alc = piv_yyjson_alc::instance().pref();
        std::string ret;
        char* str = yyjson_mut_val_write_opts(m_val, flag, alc, nullptr, nullptr);
        if (str)
        {
            ret.assign(str);
            if (alc == nullptr)
                free(str);
            else
                (alc->free)(alc->ctx, str);
        }
        return ret;
    }

    inline yyjson_mut_doc* doc()
    {
        return _doc()->doc;
    }

    inline yyjson_mut_doc* doc() const
    {
        return _doc()->doc;
    }

    inline piv_yyjson_mut_val& set_obj()
    {
        yyjson_mut_set_obj(m_val);
        return *this;
    }

    inline piv_yyjson_mut_val& set_arr()
    {
        yyjson_mut_set_arr(m_val);
        return *this;
    }

    inline piv_yyjson_val to_imut_doc(const yyjson_alc* alc = nullptr)
    {
        return piv_yyjson_val{yyjson_mut_doc_imut_copy(_doc()->doc, alc ? alc : piv_yyjson_alc::instance().pref())};
    }

    inline piv_yyjson_val to_imut_val(const yyjson_alc* alc = nullptr)
    {
        return piv_yyjson_val{yyjson_mut_val_imut_copy(m_val, alc ? alc : piv_yyjson_alc::instance().pref())};
    }

    inline bool empty() const
    {
        return (!m_shared_doc && m_weak_doc.expired()) ? true : !_doc()->doc;
    }

    inline bool is_root() const
    {
        return m_val == _doc()->root();
    }

    inline bool is_same_doc(const piv_yyjson_mut_val& rhs) const
    {
        return empty() ? false : (_doc() == rhs._doc());
    }

    inline size_t size()
    {
        return yyjson_mut_get_len(m_val);
    }

    inline size_t arr_size()
    {
        return yyjson_mut_arr_size(m_val);
    }

    inline size_t obj_size()
    {
        return yyjson_mut_obj_size(m_val);
    }

    inline uint8_t type()
    {
        return yyjson_mut_get_type(m_val);
    }

    inline uint8_t subtype()
    {
        return yyjson_mut_get_subtype(m_val);
    }

    inline uint8_t tag()
    {
        return yyjson_mut_get_tag(m_val);
    }

    inline const char* desc()
    {
        return yyjson_mut_get_type_desc(m_val);
    }

    inline bool empty_val()
    {
        return !m_val;
    }

    inline bool is_raw()
    {
        return yyjson_mut_is_raw(m_val);
    }

    inline bool is_null()
    {
        return yyjson_mut_is_null(m_val);
    }

    inline bool is_bool()
    {
        return yyjson_mut_is_bool(m_val);
    }

    inline bool is_num()
    {
        return yyjson_mut_is_num(m_val);
    }

    inline bool is_sint()
    {
        return yyjson_mut_is_sint(m_val);
    }

    inline bool is_uint()
    {
        return yyjson_mut_is_uint(m_val);
    }

    inline bool is_real()
    {
        return yyjson_mut_is_real(m_val);
    }

    inline bool is_str()
    {
        return yyjson_mut_is_str(m_val);
    }

    inline bool is_arr()
    {
        return yyjson_mut_is_arr(m_val);
    }

    inline bool is_obj()
    {
        return yyjson_mut_is_obj(m_val);
    }

    inline bool is_ctn()
    {
        return yyjson_mut_is_ctn(m_val);
    }

    inline bool arr_remove(size_t idx, size_t len)
    {
        return yyjson_mut_arr_remove_range(m_val, idx, len);
    }

    inline bool arr_clear()
    {
        return yyjson_mut_arr_clear(m_val);
    }

    inline bool arr_rotate(size_t idx)
    {
        return yyjson_mut_arr_rotate(m_val, idx);
    }

    inline bool obj_clear()
    {
        return yyjson_mut_obj_clear(m_val);
    }

    inline bool obj_rotate(size_t idx)
    {
        return yyjson_mut_obj_rotate(m_val, idx);
    }

    template <typename T>
    inline yyjson_mut_val* get_val(T idx, typename std::enable_if<std::is_integral<T>::value, T*>::type = nullptr)
    {
        return yyjson_mut_arr_get(m_val, static_cast<size_t>(idx));
    }

    template <typename T>
    yyjson_mut_val* get_val(T&& key, typename std::enable_if<!std::is_integral<typename std::decay<T>::type>::value, typename std::decay<T>::type*>::type = nullptr)
    {
        PivAny2U<true> sv{key};
        return yyjson_mut_obj_getn(m_val, sv.data(), sv.size());
    }

    template <typename T>
    inline piv_yyjson_mut_val get(T&& idx_or_key)
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, get_val(std::forward<T>(idx_or_key))};
    }

    template <typename T>
    inline piv_yyjson_mut_val operator[](T&& idx_or_key)
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, get_val(std::forward<T>(idx_or_key))};
    }

    inline piv_yyjson_mut_val arr_get_first()
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_arr_get_first(m_val)};
    }

    inline piv_yyjson_mut_val arr_get_last()
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_arr_get_last(m_val)};
    }

    template <typename T>
    yyjson_mut_val* ptr_val(T&& ptr, yyjson_ptr_err* err = nullptr, piv_yyjson_mut_val::ptr_ctx* ctx = nullptr)
    {
        PivAny2U<true> sv{ptr};
        if (ctx)
            ctx->doc = m_shared_doc ? m_shared_doc : m_weak_doc;
        if (m_val == _doc()->root())
            return yyjson_mut_doc_ptr_getx(_doc()->doc, sv.data(), sv.size(), ctx ? &ctx->ctx : nullptr, err);
        else
            return yyjson_mut_ptr_getx(m_val, sv.data(), sv.size(), ctx ? &ctx->ctx : nullptr, err);
    }

    template <typename T>
    inline piv_yyjson_mut_val ptr(T&& ptr, yyjson_ptr_err* err = nullptr, piv_yyjson_mut_val::ptr_ctx* ctx = nullptr)
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, ptr_val(std::forward<T>(ptr), err, ctx)};
    }

    inline const char* get_str()
    {
        return yyjson_mut_get_str(m_val);
    }

    template <typename R>
    inline R get_num()
    {
        return to_num<R>(m_val);
    }

    inline double get_real()
    {
        return yyjson_mut_get_num(m_val);
    }

    inline bool get_bool()
    {
        return yyjson_mut_get_bool(m_val);
    }

    template <typename T>
    inline const char* get_str(T&& idx_or_key)
    {
        return yyjson_mut_get_str(get_val(std::forward<T>(idx_or_key)));
    }

    template <typename R, typename T>
    inline R get_num(T&& idx_or_key)
    {
        return to_num<R>(get_val(std::forward<T>(idx_or_key)));
    }

    template <typename T>
    inline double get_real(T&& idx_or_key)
    {
        return yyjson_mut_get_num(get_val(std::forward<T>(idx_or_key)));
    }

    template <typename T>
    inline bool get_bool(T&& idx_or_key)
    {
        return yyjson_mut_get_bool(get_val(std::forward<T>(idx_or_key)));
    }

    template <typename T>
    inline const char* get_ptr_str(T&& ptr, yyjson_ptr_err* err = nullptr, piv_yyjson_mut_val::ptr_ctx* ctx = nullptr)
    {
        return yyjson_mut_get_str(ptr_val(std::forward<T>(ptr), err, ctx));
    }

    template <typename R, typename T>
    inline R get_ptr_num(T&& ptr, yyjson_ptr_err* err = nullptr, piv_yyjson_mut_val::ptr_ctx* ctx = nullptr)
    {
        return to_num<R>(ptr_val(std::forward<T>(ptr), err, ctx));
    }

    template <typename T>
    inline double get_ptr_real(T&& ptr, yyjson_ptr_err* err = nullptr, piv_yyjson_mut_val::ptr_ctx* ctx = nullptr)
    {
        return yyjson_mut_get_num(ptr_val(std::forward<T>(ptr), err, ctx));
    }

    template <typename T>
    inline bool get_ptr_bool(T&& ptr, yyjson_ptr_err* err = nullptr, piv_yyjson_mut_val::ptr_ctx* ctx = nullptr)
    {
        return yyjson_mut_get_bool(ptr_val(std::forward<T>(ptr), err, ctx));
    }

    size_t eume_key(std::vector<std::string>& key_arr)
    {
        yyjson_mut_obj_iter iter;
        if (yyjson_mut_obj_iter_init(m_val, &iter))
        {
            yyjson_mut_val* key = yyjson_mut_obj_iter_next(&iter);
            while (key)
            {
                key_arr.emplace_back(yyjson_mut_get_str(key));
                key = yyjson_mut_obj_iter_next(&iter);
            }
            return key_arr.size();
        }
        return 0;
    }

    INT_P eume_key(CMStringArray& key_arr)
    {
        yyjson_mut_obj_iter iter;
        if (yyjson_mut_obj_iter_init(m_val, &iter))
        {
            yyjson_mut_val* key = yyjson_mut_obj_iter_next(&iter);
            while (key)
            {
                key_arr.Add(yyjson_mut_get_str(key));
                key = yyjson_mut_obj_iter_next(&iter);
            }
            return key_arr.GetCount();
        }
        return 0;
    }

    iterator begin()
    {
        if (yyjson_mut_is_ctn(m_val))
        {
            size_t len = yyjson_mut_get_len(m_val);
            return iterator(len, 0, this);
        }
        return iterator(0, 0, nullptr);
    }

    iterator end()
    {
        if (yyjson_mut_is_ctn(m_val))
        {
            size_t len = yyjson_mut_get_len(m_val);
            return iterator(len, len, nullptr);
        }
        return iterator(0, 0, nullptr);
    }

    template <typename T>
    inline bool set_val(T&& val, bool deep_copy = true)
    {
        return _doc()->set_val(m_val, std::forward<T>(val), deep_copy);
    }

    template <typename T, typename V>
    inline bool set_val(T&& idx_or_key, V&& val, bool deep_copy = true)
    {
        return _doc()->set_val(get_val(std::forward<T>(idx_or_key)), std::forward<V>(val), deep_copy);
    }

    template <typename... Args>
    inline bool append(bool deep_copy, Args&&... args)
    {
        if (yyjson_mut_is_null(m_val))
            yyjson_mut_set_arr(m_val);
        return _doc()->append(m_val, deep_copy, std::forward<Args>(args)...);
    }

    template <typename... Args>
    inline bool prepend(bool deep_copy, Args&&... args)
    {
        if (yyjson_mut_is_null(m_val))
            yyjson_mut_set_arr(m_val);
        return _doc()->prepend(m_val, deep_copy, std::forward<Args>(args)...);
    }

    template <typename V>
    inline bool arr_insert(size_t idx, V&& val, bool deep_copy = true)
    {
        return yyjson_mut_arr_insert(m_val, _doc()->to_val(std::forward<V>(val), deep_copy), idx);
    }

    inline piv_yyjson_mut_val arr_remove(size_t idx)
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_arr_remove(m_val, idx)};
    }

    inline piv_yyjson_mut_val remove_first()
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_arr_remove_first(m_val)};
    }

    inline piv_yyjson_mut_val remove_last()
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_arr_remove_last(m_val)};
    }

    template <typename K>
    inline piv_yyjson_mut_val obj_remove(K&& key)
    {
        PivAny2U<true> sv{key};
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_obj_remove_keyn(m_val, sv.data(), sv.size())};
    }

    template <typename P>
    inline piv_yyjson_mut_val ptr_remove(P&& ptr, yyjson_ptr_err* err = nullptr, piv_yyjson_mut_val::ptr_ctx* ctx = nullptr)
    {
        PivAny2U<true> sv{ptr};
        if (ctx)
            ctx->doc = m_shared_doc ? m_shared_doc : m_weak_doc;
        if (m_val == _doc()->root())
            return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_doc_ptr_removex(_doc()->doc, sv.data(), sv.size(), ctx ? &ctx->ctx : nullptr, err)};
        else
            return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_ptr_removex(m_val, sv.data(), sv.size(), ctx ? &ctx->ctx : nullptr, err)};
    }

    template <typename K, typename N>
    inline bool obj_rename(K&& key, N&& new_key)
    {
        PivAny2U<true> sv{key}, new_sv{new_key};
        return yyjson_mut_obj_rename_keyn(_doc()->doc, m_val, sv.data(), sv.size(), new_sv.data(), new_sv.size());
    }

    template <typename V>
    inline piv_yyjson_mut_val arr_replace(size_t idx, V&& val, bool deep_copy = true)
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_arr_replace(m_val, idx, _doc()->to_val(std::forward<V>(val), deep_copy))};
    }

    template <typename K, typename V>
    inline bool obj_replace(K&& key, V&& val, bool deep_copy = true)
    {
        return yyjson_mut_obj_replace(m_val, _doc()->to_key(std::forward<K>(key), deep_copy), _doc()->to_val(std::forward<V>(val), deep_copy));
    }

    template <typename K, typename V>
    inline bool add(K&& key, V&& val, bool deep_copy = true, bool replaced = true)
    {
        if (yyjson_mut_is_null(m_val))
            yyjson_mut_set_obj(m_val);
        if (replaced)
            return yyjson_mut_obj_put(m_val, _doc()->to_key(std::forward<K>(key), deep_copy), _doc()->to_val(std::forward<V>(val), deep_copy));
        else
            return yyjson_mut_obj_add(m_val, _doc()->to_key(std::forward<K>(key), deep_copy), _doc()->to_val(std::forward<V>(val), deep_copy));
    }

    template <typename P, typename V>
    inline bool ptr_add(P&& ptr, V&& val, bool deep_copy = true, bool replaced = true, yyjson_ptr_err* err = nullptr, piv_yyjson_mut_val::ptr_ctx* ctx = nullptr)
    {
        PivAny2U<true> sv{ptr};
        if (ctx)
            ctx->doc = m_shared_doc ? m_shared_doc : m_weak_doc;
        yyjson_mut_val* val_to_add = _doc()->to_val(std::forward<V>(val), deep_copy);
        if (yyjson_mut_is_null(m_val))
            yyjson_mut_set_obj(m_val);
        if (m_val == _doc()->root())
        {
            if (replaced)
                return yyjson_mut_doc_ptr_setx(_doc()->doc, sv.data(), sv.size(), val_to_add, true, ctx ? &ctx->ctx : nullptr, err);
            else
                return yyjson_mut_doc_ptr_addx(_doc()->doc, sv.data(), sv.size(), val_to_add, true, ctx ? &ctx->ctx : nullptr, err);
        }
        else
        {
            if (replaced)
                return yyjson_mut_ptr_setx(m_val, sv.data(), sv.size(), val_to_add, _doc()->doc, true, ctx ? &ctx->ctx : nullptr, err);
            else
                return yyjson_mut_ptr_addx(m_val, sv.data(), sv.size(), val_to_add, _doc()->doc, true, ctx ? &ctx->ctx : nullptr, err);
        }
    }

    inline piv_yyjson_mut_val patch(const piv_yyjson_mut_val& patch, yyjson_patch_err* err)
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_patch(_doc()->doc, m_val, patch.m_val, err)};
    }

    inline piv_yyjson_mut_val merge_patch(const piv_yyjson_mut_val& patch)
    {
        return piv_yyjson_mut_val{m_shared_doc ? m_shared_doc : m_weak_doc, yyjson_mut_merge_patch(_doc()->doc, m_val, patch.m_val)};
    }

}; // class piv_yyjson_mut_val

// Class Implement Begin
piv_yyjson_mut_val piv_yyjson_val::to_mut_val()
{
    return piv_yyjson_mut_val{m_val};
}
// Class Implement End

#endif // _PIV_YYJSON_HPP
