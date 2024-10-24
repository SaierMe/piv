﻿/*********************************************\
 * 火山视窗 - MMKV封装类                     *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef _PIV_MMKV_HPP
#define _PIV_MMKV_HPP

#include "../../src/piv_string.hpp"

#include "MMKV.h"

namespace piv
{
    namespace mmkv
    {

        /**
         * @brief 全局初始化
         * @param rootDir 根目录
         * @param MMKVLogInfo 日志级别
         */
        static void InitializeMMKV(const wchar_t *rootDir = L"", int32_t MMKVLogInfo = 2)
        {
            std::wstring _rootDir(rootDir);
            if (_rootDir.empty())
            {
                _rootDir.resize(MAX_PATH);
                ::GetModuleFileNameW(NULL, const_cast<wchar_t *>(reinterpret_cast<const wchar_t *>(_rootDir.data())), MAX_PATH);
                _rootDir.resize(_rootDir.rfind(L'\\'));
            }
            else
            {
                if (_rootDir.back() == L'\\')
                {
                    _rootDir.pop_back();
                }
            }
            MMKV::initializeMMKV(_rootDir, static_cast<MMKVLogLevel>(MMKVLogInfo), nullptr);
        }

        /**
         * @brief 全局清理
         */
        static void OnExit()
        {
            MMKV::onExit();
        }

        /**
         * @brief 取根目录
         * @return
         */
        static CWString GetRootDir()
        {
            return CWString{MMKV::getRootDir().c_str()};
        }

        /**
         * @brief 备份表
         * @param mmapID 表名称
         * @param dstDir 目标目录
         * @param srcDir 来源目录
         * @return
         */
        static bool BackupOneToDirectory(const wchar_t *mmapID, const wchar_t *dstDir, const wchar_t *srcDir)
        {
            std::wstring _srcDir{srcDir};
            return MMKV::backupOneToDirectory(PivW2U{mmapID}, dstDir, _srcDir.empty() ? nullptr : &_srcDir);
        }

        /**
         * @brief 恢复表
         * @param mmapID 表名称
         * @param srcDir 来源目录
         * @param dstDir 目标目录
         * @return
         */
        static bool RestoreOneFromDirectory(const wchar_t *mmapID, const wchar_t *srcDir, const wchar_t *dstDir)
        {

            std::wstring _dstDir{dstDir};
            return MMKV::restoreOneFromDirectory(PivW2U{mmapID}, srcDir, _dstDir.empty() ? nullptr : &_dstDir);
        }

        /**
         * @brief 备份所有表
         * @param dstDir 目标目录
         * @param srcDir 来源目录
         * @return
         */
        static int32_t BackupAllToDirectory(const wchar_t *dstDir, const wchar_t *srcDir)
        {
            std::wstring _srcDir{srcDir};
            return static_cast<int32_t>(MMKV::backupAllToDirectory(dstDir, _srcDir.empty() ? nullptr : &_srcDir));
        }

        /**
         * @brief 恢复所有表
         * @param srcDir 来源目录
         * @param dstDir 目标目录
         * @return
         */
        static int32_t RestoreAllFromDirectory(const wchar_t *srcDir, const wchar_t *dstDir)
        {
            std::wstring _dstDir{dstDir};
            return static_cast<int32_t>(MMKV::restoreAllFromDirectory(srcDir, _dstDir.empty() ? nullptr : &_dstDir));
        }

        /**
         * @brief 校验表文件
         * @param mmapID 表名称
         * @param relatePath 相对路径
         * @return
         */
        static bool IsFileValid(const wchar_t *mmapID, const wchar_t *relatePath)
        {
            std::wstring _relatePath{relatePath};
            return MMKV::isFileValid(PivW2U{mmapID}, _relatePath.empty() ? nullptr : &_relatePath);
        }

        /**
         * @brief 删除表文件
         * @param mmapID 表名称
         * @param relatePath 相对路径
         * @return
         */
        static bool RemoveStorage(const wchar_t *mmapID, const wchar_t *relatePath)
        {
            std::wstring _relatePath{relatePath};
            return MMKV::removeStorage(PivW2U{mmapID}, _relatePath.empty() ? nullptr : &_relatePath);
        }

        /**
         * @brief 置日志级别
         * @param level 日志级别
         */
        static void SetLogLevel(int32_t level)
        {
            MMKV::setLogLevel(static_cast<MMKVLogLevel>(level));
        }
    } // namespace mmkv

    /**
     * @brief MMKV事件指针存取(单例对象)
     */
    class mmkvEvent
    {
    private:
        mmkvEvent() {}
        ~mmkvEvent() {}
        mmkvEvent(const mmkvEvent &) = delete;
        mmkvEvent(mmkvEvent &&) = delete;
        mmkvEvent &operator=(const mmkvEvent &) = delete;
        mmkvEvent &operator=(mmkvEvent &&) = delete;

        void *userdate = nullptr;

    public:
        static mmkvEvent &data()
        {
            static mmkvEvent event;
            return event;
        }
        inline void set(void *event) noexcept
        {
            userdate = event;
        }
        inline bool hasValue() noexcept
        {
            return !!userdate;
        }
        template <typename T = void *>
        inline T get() noexcept
        {
            return reinterpret_cast<T>(userdate);
        }
    }; // class mmkvEvent
} // namespace piv

/**
 * @brief MMKV包装类
 */
class PivMMKV
{
private:
    MMKV *m_MMKV = nullptr;

public:
    PivMMKV() {}
    ~PivMMKV()
    {
        Close();
    }

    /**
     * @brief 打开
     * @param mmapID 表名称
     * @param mode 模式
     * @param cryptKey 密钥
     * @param rootPath 根路径
     * @param expectedCapacity 预期容量
     * @return
     */
    int32_t OpenMMKV(const wchar_t *mmapID, int32_t mode, const wchar_t *cryptKey, const wchar_t *rootPath, size_t expectedCapacity = 0)
    {
        Close();
        if (MMKV::getRootDir().empty() == true)
        {
            piv::mmkv::InitializeMMKV();
        }
        PivW2U _mmapID{mmapID};
        PivW2U _cryptKey{cryptKey};
        std::wstring _rootPath{rootPath};
        if (_mmapID.empty())
            m_MMKV = MMKV::defaultMMKV(static_cast<MMKVMode>(mode), _cryptKey.empty() ? nullptr : &_cryptKey.ref());
        else
            m_MMKV = MMKV::mmkvWithID(_mmapID, static_cast<MMKVMode>(mode), _cryptKey.empty() ? nullptr : &_cryptKey.ref(),
                                      _rootPath.empty() ? nullptr : &_rootPath, expectedCapacity);
        if (m_MMKV != nullptr)
            return (m_MMKV->count(false) > 0) ? 1 : 0;
        else
            return -1;
    }

    /**
     * @brief 关闭
     */
    inline void Close()
    {
        if (m_MMKV)
        {
            m_MMKV->close();
            m_MMKV = nullptr;
        }
    }

    /**
     * @brief 取表名称
     * @return
     */
    inline CWString GetMmapID()
    {
        return m_MMKV ? PivU2Ws{m_MMKV->mmapID()}.cref() : CWString(L"");
    }

    /**
     * @brief 是否多进程
     * @return
     */
    inline bool IsInterProcess() const
    {
        return m_MMKV ? m_MMKV->m_isInterProcess : false;
    }

    /**
     * @brief 取密钥
     * @return
     */
    inline CWString GetCryptKey()
    {
        return m_MMKV ? PivU2Ws{m_MMKV->cryptKey()}.cref() : CWString(L"");
    }

    /**
     * @brief 更改密钥
     * @param cryptKey 新密钥
     * @return
     */
    inline bool ReKey(const wchar_t *cryptKey)
    {
        return m_MMKV ? m_MMKV->reKey(PivW2U{cryptKey}.cref()) : false;
    }

    /**
     * @brief 检查更改密钥
     * @param cryptKey 新密钥
     */
    inline void CheckReSetCryptKey(const wchar_t *cryptKey)
    {
        if (m_MMKV)
        {
            PivW2U _cryptKey{cryptKey};
            m_MMKV->checkReSetCryptKey(&_cryptKey.cref());
        }
    }

    /**
     * @brief 取总大小
     * @return
     */
    inline ptrdiff_t GetTotalSize()
    {
        return m_MMKV ? static_cast<ptrdiff_t>(m_MMKV->totalSize()) : 0;
    }

    /**
     * @brief 取实际大小
     * @return
     */
    inline ptrdiff_t GetActualSize()
    {
        return m_MMKV ? static_cast<ptrdiff_t>(m_MMKV->actualSize()) : 0;
    }

    /**
     * @brief 置入键值
     * @param value 值
     * @param key 键名
     * @param expireDuration 有效期
     * @return
     */
    template <typename T, typename KeyT>
    inline bool SetValue(T value, const KeyT &key, int32_t expireDuration = -1)
    {
        if (!m_MMKV)
            return false;
        return expireDuration == -1 ? m_MMKV->set(value, *PivAny2Us{key}) : m_MMKV->set(value, *PivAny2Us{key}, static_cast<uint32_t>(expireDuration));
    }

    /**
     * @brief 置入文本值
     * @param value 文本值
     * @param key 键名
     * @param expireDuration 有效期
     * @return
     */
    template <typename KeyT>
    inline bool SetCVolStr(CWString &value, const KeyT &key, int32_t expireDuration = 0)
    {
        if (!m_MMKV)
            return false;
        mmkv::MMBuffer buffer{const_cast<wchar_t *>(value.GetText()), static_cast<size_t>(value.GetLength() * 2), mmkv::MMBufferNoCopy};
        return expireDuration == -1 ? m_MMKV->set(buffer, *PivAny2Us{key}) : m_MMKV->set(buffer, *PivAny2Us{key}, static_cast<uint32_t>(expireDuration));
    }

    /**
     * @brief 置入字节集
     * @param value 字节集值
     * @param key 键名
     * @param expireDuration 有效期
     * @return
     */
    template <typename KeyT>
    inline bool SetCVolMem(CVolMem &value, const KeyT &key, int32_t expireDuration = 0)
    {
        if (!m_MMKV)
            return false;
        mmkv::MMBuffer buffer{value.GetPtr(), static_cast<size_t>(value.GetSize()), mmkv::MMBufferNoCopy};
        return expireDuration == -1 ? m_MMKV->set(buffer, *PivAny2Us{key}) : m_MMKV->set(buffer, *PivAny2Us{key}, static_cast<uint32_t>(expireDuration));
    }

    /**
     * @brief 置入对象值
     * @param object 对象值
     * @param key 键名
     * @param expireDuration 有效期
     * @return
     */
    template <typename KeyT>
    inline bool SetObject(const CVolObject &object, const KeyT &key, int32_t expireDuration = 0)
    {
        if (!m_MMKV)
            return false;
        CVolMemoryOutputStream memStream;
        object.VolSaveIntoStream(memStream);
        CVolMem bin = memStream.GetBin(CVolMem());
        mmkv::MMBuffer buffer{bin.GetPtr(), static_cast<size_t>(bin.GetSize()), mmkv::MMBufferNoCopy};
        return expireDuration == -1 ? m_MMKV->set(buffer, *PivAny2Us{key}) : m_MMKV->set(buffer, *PivAny2Us{key}, static_cast<uint32_t>(expireDuration));
    }

    /**
     * @brief 取逻辑值
     * @param key 键名
     * @param defaultValue 默认值
     * @param hasValue 是否存在
     * @return
     */
    template <typename KeyT>
    inline BOOL GetBool(const KeyT &key, BOOL defaultValue, bool *hasValue)
    {
        return m_MMKV ? m_MMKV->getBool(*PivAny2Us{key}, defaultValue ? true : false, hasValue) : defaultValue;
    }

    /**
     * @brief 取整数值
     * @param key 键名
     * @param defaultValue 默认值
     * @param hasValue 是否存在
     * @return
     */
    template <typename KeyT>
    inline int32_t GetInt32(const KeyT &key, int32_t defaultValue, bool *hasValue)
    {
        return m_MMKV ? m_MMKV->getInt32(*PivAny2Us{key}, defaultValue, hasValue) : defaultValue;
    }

    /**
     * @brief 取长整数值
     * @param key 键名
     * @param defaultValue 默认值
     * @param hasValue 是否存在
     * @return
     */
    template <typename KeyT>
    inline int64_t GetInt64(const KeyT &key, int64_t defaultValue, bool *hasValue)
    {
        return m_MMKV ? m_MMKV->getInt64(*PivAny2Us{key}, defaultValue, hasValue) : defaultValue;
    }

    /**
     * @brief 取小数值
     * @param key 键名
     * @param defaultValue 默认值
     * @param hasValue 是否存在
     * @return
     */
    template <typename KeyT>
    inline double GetDouble(const KeyT &key, double defaultValue, bool *hasValue)
    {
        return m_MMKV ? m_MMKV->getDouble(*PivAny2Us{key}, defaultValue, hasValue) : defaultValue;
    }

    /**
     * @brief 取单精度小数值
     * @param key 键名
     * @param defaultValue 默认值
     * @param hasValue 是否存在
     * @return
     */
    template <typename KeyT>
    inline float GetFloat(const KeyT &key, float defaultValue, bool *hasValue)
    {
        return m_MMKV ? m_MMKV->getFloat(*PivAny2Us{key}, defaultValue, hasValue) : defaultValue;
    }

    /**
     * @brief 取文本值
     * @param key 键名
     * @param value 取回值
     * @return
     */
    template <typename KeyT>
    inline bool GetString(const KeyT &key, CWString &value)
    {
        value.Empty();
        if (m_MMKV)
        {
            mmkv::MMBuffer buffer;
            if (m_MMKV->getBytes(*PivAny2Us{key}, buffer))
            {
                value.SetText(reinterpret_cast<const wchar_t *>(buffer.getPtr()), buffer.length() / 2);
                return true;
            }
        }
        return false;
    }
    template <typename KeyT>
    inline CWString GetString(const KeyT &key)
    {
        CWString result;
        if (m_MMKV)
        {
            mmkv::MMBuffer buffer;
            if (m_MMKV->getBytes(*PivAny2Us{key}, buffer))
            {
                result.SetText(reinterpret_cast<const wchar_t *>(buffer.getPtr()), buffer.length() / 2);
            }
        }
        return result;
    }
    template <typename KeyT>
    inline bool GetString(const KeyT &key, std::string &value)
    {
        value.clear();
        if (m_MMKV)
        {
            return m_MMKV->getString(*PivAny2Us{key}, value);
        }
        return false;
    }
    template <typename KeyT>
    inline std::string GetStringU(const KeyT &key)
    {
        std::string value;
        if (m_MMKV)
        {
            m_MMKV->getString(*PivAny2Us{key}, value);
        }
        return value;
    }

    template <typename KeyT>
    inline bool GetVector(const KeyT &key, std::vector<std::string> &result)
    {
        if (m_MMKV)
        {
            return m_MMKV->getVector(*PivAny2Us{key}, result);
        }
        return false;
    }

    /**
     * @brief 取字节集
     * @param key 键名
     * @param value 取回值
     * @return
     */
    template <typename KeyT>
    inline bool GetMem(const KeyT &key, CVolMem &value)
    {
        value.Empty();
        if (m_MMKV)
        {
            mmkv::MMBuffer buffer;
            if (m_MMKV->getBytes(*PivAny2Us{key}, buffer))
            {
                value.Append(buffer.getPtr(), buffer.length());
                return true;
            }
        }
        return false;
    }
    template <typename KeyT>
    inline CVolMem GetMem(const KeyT &key)
    {
        CVolMem result;
        if (m_MMKV)
        {
            mmkv::MMBuffer buffer;
            if (m_MMKV->getBytes(*PivAny2Us{key}, buffer))
            {
                result.Append(buffer.getPtr(), buffer.length());
            }
        }
        return result;
    }

    /**
     * @brief 取对象值
     * @param key 键名
     * @param object 取回对象
     * @return
     */
    template <typename KeyT>
    inline bool GetObject(const KeyT &key, CVolObject &object)
    {
        object.ResetToNullObject();
        if (m_MMKV)
        {
            mmkv::MMBuffer buffer;
            if (m_MMKV->getBytes(*PivAny2Us{key}, buffer))
            {
                CVolMemoryInputStream memStream;
                memStream.ResetMemory(buffer.getPtr(), buffer.length());
                object.VolLoadFromStream(memStream);
                return true;
            }
        }
        return false;
    }

    /**
     * @brief 取键值对大小
     * @param key 键名
     * @param actualSize 值实际大小
     * @return
     */
    template <typename KeyT>
    inline ptrdiff_t GetValueSize(const KeyT &key, bool actualSize)
    {
        return m_MMKV ? static_cast<ptrdiff_t>(m_MMKV->getValueSize(*PivAny2Us{key}, actualSize)) : 0;
    }

    /**
     * @brief 是否存在键
     * @param key 键名
     * @return
     */
    template <typename KeyT>
    inline bool IsContainsKey(const KeyT &key)
    {
        return m_MMKV ? m_MMKV->containsKey(*PivAny2Us{key}) : false;
    }

    /**
     * @brief 取键值对数量
     * @param filterExpire 过滤到期键值
     * @return
     */
    inline ptrdiff_t GetCount(bool filterExpire = false)
    {
        return m_MMKV ? static_cast<ptrdiff_t>(m_MMKV->count(filterExpire)) : 0;
    }

    /**
     * @brief 取所有键名
     * @param keysArray 键名数组
     * @param filterExpire 过滤到期键值
     * @return
     */
    ptrdiff_t GetAllKeys(CMStringArray &keysArray, bool filterExpire = false)
    {
        keysArray.RemoveAll();
        if (m_MMKV)
        {
            std::vector<std::string> keys = m_MMKV->allKeys(filterExpire);
            for (auto iter = keys.cbegin(); iter != keys.cend(); ++iter)
            {
                keysArray.Add(PivU2W(*iter).c_str());
            }
            return keysArray.GetCount();
        }
        return 0;
    }

    /**
     * @brief 批量删除键值对
     * @param keysArray 键名数组
     */
    void RemoveValuesForKeys(CMStringArray &keysArray)
    {
        if (m_MMKV)
        {
            std::vector<std::string> keys;
            for (INT_P i = 0; i < keysArray.GetCount(); ++i)
            {
                keys.push_back(*PivW2U{keysArray.GetAt(i)});
            }
            m_MMKV->removeValuesForKeys(keys);
        }
    }
    void RemoveValuesForKeys(std::vector<std::string> &keysArray)
    {
        if (m_MMKV)
        {
            m_MMKV->removeValuesForKeys(keysArray);
        }
    }

    /**
     * @brief 删除键值对
     * @tparam KeyT
     * @param key 键名
     */
    template <typename KeyT>
    inline void RemoveValueForKey(const KeyT &key)
    {
        if (m_MMKV)
        {
            m_MMKV->removeValueForKey(*PivAny2Us{key});
        }
    }

    /**
     * @brief 清空
     */
    inline void ClearAll(bool keepSpace = false)
    {
        if (m_MMKV)
        {
            m_MMKV->clearAll(keepSpace);
        }
    }

    /**
     * @brief 释放空间
     */
    inline void Trim()
    {
        if (m_MMKV)
        {
            m_MMKV->trim();
        }
    }

    /**
     * @brief 清理缓存
     */
    inline void ClearMemoryCache(bool keepSpace = false)
    {
        if (m_MMKV)
        {
            m_MMKV->clearMemoryCache(keepSpace);
        }
    }

    /**
     * @brief 执行同步
     * @param syncMode 同步模式
     */
    inline void Sync(bool syncMode)
    {
        if (m_MMKV)
        {
            m_MMKV->sync(static_cast<SyncFlag>(syncMode));
        }
    }

    /**
     * @brief 锁定
     */
    inline void Lock()
    {
        if (m_MMKV)
        {
            m_MMKV->lock();
        }
    }

    /**
     * @brief 解锁
     */
    inline void UnLock()
    {
        if (m_MMKV)
        {
            m_MMKV->unlock();
        }
    }

    /**
     * @brief 尝试锁定
     * @return
     */
    inline bool TryLock()
    {
        return m_MMKV ? m_MMKV->try_lock() : false;
    }

    /**
     * @brief 检查内容更改
     */
    inline void CheckContentChanged()
    {
        if (m_MMKV)
        {
            m_MMKV->checkContentChanged();
        }
    }

    /**
     * @brief 启用键值自动过期
     * @param expiredInSeconds 有效期
     * @return
     */
    inline bool EnableAutoKeyExpire(uint32_t expiredInSeconds = 0)
    {
        return m_MMKV ? m_MMKV->enableAutoKeyExpire(expiredInSeconds) : false;
    }

    /**
     * @brief 禁用键值自动过期
     * @return
     */
    inline bool DisableAutoKeyExpire()
    {
        return m_MMKV ? m_MMKV->disableAutoKeyExpire() : false;
    }

}; // PivMMKV

#endif // _PIV_MMKV_HPP
