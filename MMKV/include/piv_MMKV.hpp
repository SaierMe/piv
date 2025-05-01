/*********************************************\
 * 火山视窗 - MMKV封装类                     *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef _PIV_MMKV_HPP
#define _PIV_MMKV_HPP

#include "../../src/piv_string.hpp"

#include "MMKV.h"

/**
 * @brief MMKV事件指针存取(单例对象)
 */
class PivMMKVEvent
{
private:
    PivMMKVEvent() {}
    ~PivMMKVEvent() {}
    PivMMKVEvent(const PivMMKVEvent &) = delete;
    PivMMKVEvent(PivMMKVEvent &&) = delete;
    PivMMKVEvent &operator=(const PivMMKVEvent &) = delete;
    PivMMKVEvent &operator=(PivMMKVEvent &&) = delete;

    void *userdate = nullptr;

public:
    static PivMMKVEvent &instance()
    {
        static PivMMKVEvent inst;
        return inst;
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
}; // class PivMMKVEvent

/**
 * @brief MMKV包装类
 */
class PivMMKV
{
private:
    MMKV *m_MMKV = nullptr;

public:
    /**
     * @brief 全局初始化
     * @param rootDir 根目录
     * @param MMKVLogInfo 日志级别
     */
    static void initializeMMKV(const wchar_t *rootDir = L"", int32_t MMKVLogInfo = 2)
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
    static void onExit()
    {
        MMKV::onExit();
    }

    /**
     * @brief 取根目录
     * @return
     */
    static const std::wstring &getRootDir()
    {
        return MMKV::getRootDir();
    }

    /**
     * @brief 备份表
     * @param mmapID 表名称
     * @param dstDir 目标目录
     * @param srcDir 来源目录
     * @return
     */
    static bool backupOneToDirectory(const wchar_t *mmapID, const wchar_t *dstDir, const wchar_t *srcDir)
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
    static bool restoreOneFromDirectory(const wchar_t *mmapID, const wchar_t *srcDir, const wchar_t *dstDir)
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
    static size_t backupAllToDirectory(const wchar_t *dstDir, const wchar_t *srcDir)
    {
        std::wstring _srcDir{srcDir};
        return MMKV::backupAllToDirectory(dstDir, _srcDir.empty() ? nullptr : &_srcDir);
    }

    /**
     * @brief 恢复所有表
     * @param srcDir 来源目录
     * @param dstDir 目标目录
     * @return
     */
    static size_t restoreAllFromDirectory(const wchar_t *srcDir, const wchar_t *dstDir)
    {
        std::wstring _dstDir{dstDir};
        return MMKV::restoreAllFromDirectory(srcDir, _dstDir.empty() ? nullptr : &_dstDir);
    }

    /**
     * @brief 校验表文件
     * @param mmapID 表名称
     * @param relatePath 相对路径
     * @return
     */
    static bool isFileValid(const wchar_t *mmapID, const wchar_t *relatePath)
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
    static bool removeStorage(const wchar_t *mmapID, const wchar_t *relatePath)
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

    /**
     * @brief 表是否存在
     * @param mmapID 表名称
     * @param relatePath 相对路径
     * @return
     */
    static bool checkExist(const wchar_t *mmapID, const wchar_t *relatePath)
    {
        std::wstring _relatePath{relatePath};
        return MMKV::checkExist(PivW2U{mmapID}, _relatePath.empty() ? nullptr : &_relatePath);
    }

    PivMMKV() {}
    ~PivMMKV()
    {
        close();
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
    int32_t open(const wchar_t *mmapID, MMKVMode mode, const wchar_t *cryptKey, const wchar_t *rootPath, size_t expectedCapacity = 0)
    {
        close();
        if (MMKV::getRootDir().empty() == true)
        {
            PivMMKV::initializeMMKV();
        }
        PivW2U _mmapID{mmapID};
        PivW2U _cryptKey{cryptKey};
        std::wstring _rootPath{rootPath};
        if (_mmapID.empty())
            m_MMKV = MMKV::defaultMMKV(mode, _cryptKey.empty() ? nullptr : &_cryptKey.str);
        else
            m_MMKV = MMKV::mmkvWithID(_mmapID, mode, _cryptKey.empty() ? nullptr : &_cryptKey.str,
                                      _rootPath.empty() ? nullptr : &_rootPath, expectedCapacity);
        if (m_MMKV != nullptr)
            return (m_MMKV->count(false) > 0) ? 1 : 0;
        else
            return -1;
    }

    /**
     * @brief 关闭
     */
    inline void close()
    {
        if (m_MMKV)
        {
            m_MMKV->close();
            m_MMKV = nullptr;
        }
    }

    /**
     * @brief 导入
     * @param src 来源
     * @return 
     */
    inline size_t importFrom(PivMMKV &src)
    {
        return m_MMKV ? m_MMKV->importFrom(src.m_MMKV) : 0;
    }

    /**
     * @brief 取表名称
     * @return
     */
    inline const std::string &mmapID()
    {
        return m_MMKV ? m_MMKV->mmapID() : "";
    }

    /**
     * @brief 是否多进程
     * @return
     */
    inline bool isMultiProcess() const
    {
        return m_MMKV ? m_MMKV->isMultiProcess() : false;
    }

    /**
     * @brief 是否只读
     * @return
     */
    inline bool isReadOnly() const
    {
        return m_MMKV ? m_MMKV->isReadOnly() : false;
    }

    /**
     * @brief 取密钥
     * @return
     */
    inline const std::string &cryptKey()
    {
        return m_MMKV ? m_MMKV->cryptKey() : "";
    }

    /**
     * @brief 更改密钥
     * @param cryptKey 新密钥
     * @return
     */
    inline bool reKey(const wchar_t *cryptKey)
    {
        return m_MMKV ? m_MMKV->reKey(PivW2U{cryptKey}) : false;
    }

    /**
     * @brief 检查更改密钥
     * @param cryptKey 新密钥
     */
    inline void checkReSetCryptKey(const wchar_t *cryptKey)
    {
        if (m_MMKV)
        {
            PivW2U _cryptKey{cryptKey};
            m_MMKV->checkReSetCryptKey(&_cryptKey.str);
        }
    }

    /**
     * @brief 取总大小
     * @return
     */
    inline size_t totalSize()
    {
        return m_MMKV ? m_MMKV->totalSize() : 0;
    }

    /**
     * @brief 取实际大小
     * @return
     */
    inline size_t actualSize()
    {
        return m_MMKV ? m_MMKV->actualSize() : 0;
    }

    /**
     * @brief 置入键值
     * @param value 值
     * @param key 键名
     * @param expireDuration 有效期
     * @return
     */
    template <typename T, typename KeyT>
    inline bool set(T &&value, const KeyT &key, int32_t expireDuration = -1)
    {
        if (!m_MMKV)
            return false;
        PivS2V _key{key};
        return expireDuration == -1 ? m_MMKV->set(value, _key) : m_MMKV->set(value, _key, static_cast<uint32_t>(expireDuration));
    }

    template <typename T, typename KeyT>
    inline bool setString(T &&value, const KeyT &key, int32_t expireDuration = -1)
    {
        return set(PivS2V{value}.sv, key, expireDuration);
    }

    /**
     * @brief 置入文本值
     * @param value 文本值
     * @param key 键名
     * @param expireDuration 有效期
     * @return
     */
    template <typename KeyT>
    inline bool setVolStr(CWString &value, const KeyT &key, int32_t expireDuration = 0)
    {
        if (!m_MMKV)
            return false;
        mmkv::MMBuffer buffer{const_cast<wchar_t *>(value.GetText()), static_cast<size_t>(value.GetLength() * 2), mmkv::MMBufferNoCopy};
        PivS2V _key{key};
        return expireDuration == -1 ? m_MMKV->set(buffer, _key) : m_MMKV->set(buffer, _key, static_cast<uint32_t>(expireDuration));
    }

    /**
     * @brief 置入字节集
     * @param value 字节集值
     * @param key 键名
     * @param expireDuration 有效期
     * @return
     */
    template <typename KeyT>
    inline bool setVolMem(CVolMem &value, const KeyT &key, int32_t expireDuration = 0)
    {
        if (!m_MMKV)
            return false;
        mmkv::MMBuffer buffer{value.GetPtr(), static_cast<size_t>(value.GetSize()), mmkv::MMBufferNoCopy};
        PivS2V _key{key};
        return expireDuration == -1 ? m_MMKV->set(buffer, _key) : m_MMKV->set(buffer, _key, static_cast<uint32_t>(expireDuration));
    }

    /**
     * @brief 置入对象值
     * @param object 对象值
     * @param key 键名
     * @param expireDuration 有效期
     * @return
     */
    template <typename KeyT>
    inline bool setVolObj(const CVolObject &object, const KeyT &key, int32_t expireDuration = 0)
    {
        if (!m_MMKV)
            return false;
        CVolMemoryOutputStream memStream;
        object.VolSaveIntoStream(memStream);
        INT_P pDataSize;
        const void *data = memStream.GetCurrentData(&pDataSize);
        mmkv::MMBuffer buffer{(void *)data, static_cast<size_t>(pDataSize), mmkv::MMBufferNoCopy};
        PivS2V _key{key};
        return expireDuration == -1 ? m_MMKV->set(buffer, _key) : m_MMKV->set(buffer, _key, static_cast<uint32_t>(expireDuration));
    }

    /**
     * @brief 取逻辑值
     * @param key 键名
     * @param defaultValue 默认值
     * @param hasValue 是否存在
     * @return
     */
    template <typename KeyT>
    inline BOOL getBool(const KeyT &key, BOOL defaultValue, bool *hasValue)
    {
        return m_MMKV ? m_MMKV->getBool(PivS2V{key}, defaultValue ? true : false, hasValue) : defaultValue;
    }

    /**
     * @brief 取整数值
     * @param key 键名
     * @param defaultValue 默认值
     * @param hasValue 是否存在
     * @return
     */
    template <typename KeyT>
    inline int32_t getInt32(const KeyT &key, int32_t defaultValue, bool *hasValue)
    {
        return m_MMKV ? m_MMKV->getInt32(PivS2V{key}, defaultValue, hasValue) : defaultValue;
    }

    template <typename KeyT>
    inline uint32_t getUInt32(const KeyT &key, uint32_t defaultValue, bool *hasValue)
    {
        return m_MMKV ? m_MMKV->getUInt32(PivS2V{key}, defaultValue, hasValue) : defaultValue;
    }

    /**
     * @brief 取长整数值
     * @param key 键名
     * @param defaultValue 默认值
     * @param hasValue 是否存在
     * @return
     */
    template <typename KeyT>
    inline int64_t getInt64(const KeyT &key, int64_t defaultValue, bool *hasValue)
    {
        return m_MMKV ? m_MMKV->getInt64(PivS2V{key}, defaultValue, hasValue) : defaultValue;
    }

    template <typename KeyT>
    inline uint64_t getUInt64(const KeyT &key, uint64_t defaultValue, bool *hasValue)
    {
        return m_MMKV ? m_MMKV->getUInt64(PivS2V{key}, defaultValue, hasValue) : defaultValue;
    }

    /**
     * @brief 取小数值
     * @param key 键名
     * @param defaultValue 默认值
     * @param hasValue 是否存在
     * @return
     */
    template <typename KeyT>
    inline double getDouble(const KeyT &key, double defaultValue, bool *hasValue)
    {
        return m_MMKV ? m_MMKV->getDouble(PivS2V{key}, defaultValue, hasValue) : defaultValue;
    }

    /**
     * @brief 取单精度小数值
     * @param key 键名
     * @param defaultValue 默认值
     * @param hasValue 是否存在
     * @return
     */
    template <typename KeyT>
    inline float getFloat(const KeyT &key, float defaultValue, bool *hasValue)
    {
        return m_MMKV ? m_MMKV->getFloat(PivS2V{key}, defaultValue, hasValue) : defaultValue;
    }

    /**
     * @brief 取文本值
     * @param key 键名
     * @param value 取回值
     * @return
     */
    template <typename KeyT>
    inline bool getVolStr(const KeyT &key, CWString &value)
    {
        value.Empty();
        if (m_MMKV)
        {
            mmkv::MMBuffer buffer;
            if (m_MMKV->getBytes(PivS2V{key}, buffer))
            {
                value.SetText(reinterpret_cast<const wchar_t *>(buffer.getPtr()), buffer.length() / 2);
                return true;
            }
        }
        return false;
    }

    template <typename KeyT>
    inline CWString getVolStr(const KeyT &key)
    {
        CWString result;
        getVolStr(key, result);
        return result;
    }

    template <typename KeyT>
    inline bool getString(const KeyT &key, std::string &value)
    {
        value.clear();
        if (m_MMKV)
        {
            return m_MMKV->getString(PivS2V{key}, value);
        }
        return false;
    }

    template <typename KeyT>
    inline std::string getString(const KeyT &key)
    {
        std::string value;
        if (m_MMKV)
        {
            m_MMKV->getString(PivS2V{key}, value);
        }
        return value;
    }

    template <typename KeyT>
    inline bool getVector(const KeyT &key, std::vector<std::string> &result)
    {
        if (m_MMKV)
        {
            return m_MMKV->getVector(PivS2V{key}, result);
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
    inline bool getVolMem(const KeyT &key, CVolMem &value)
    {
        value.Empty();
        if (m_MMKV)
        {
            mmkv::MMBuffer buffer;
            if (m_MMKV->getBytes(PivS2V{key}, buffer))
            {
                value.Append(buffer.getPtr(), buffer.length());
                return true;
            }
        }
        return false;
    }

    template <typename KeyT>
    inline CVolMem getVolMem(const KeyT &key)
    {
        CVolMem result;
        getVolMem(key, result);
        return result;
    }

    /**
     * @brief 取对象值
     * @param key 键名
     * @param object 取回对象
     * @return
     */
    template <typename KeyT>
    inline bool getVolObj(const KeyT &key, CVolObject &object)
    {
        object.ResetToNullObject();
        if (m_MMKV)
        {
            mmkv::MMBuffer buffer;
            if (m_MMKV->getBytes(PivS2V{key}, buffer))
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
    inline size_t getValueSize(const KeyT &key, bool actualSize)
    {
        return m_MMKV ? m_MMKV->getValueSize(PivS2V{key}, actualSize) : 0;
    }

    /**
     * @brief 是否存在键
     * @param key 键名
     * @return
     */
    template <typename KeyT>
    inline bool containsKey(const KeyT &key)
    {
        return m_MMKV ? m_MMKV->containsKey(PivS2V{key}) : false;
    }

    /**
     * @brief 取键值对数量
     * @param filterExpire 过滤到期键值
     * @return
     */
    inline size_t count(bool filterExpire = false)
    {
        return m_MMKV ? m_MMKV->count(filterExpire) : 0;
    }

    /**
     * @brief 取所有键名
     * @param keysArray 键名数组
     * @param filterExpire 过滤到期键值
     * @return
     */
    inline ptrdiff_t allKeys(CMStringArray &keysArray, bool filterExpire = false)
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

    inline std::vector<std::string> allKeys(bool filterExpire = false)
    {
        return m_MMKV ? m_MMKV->allKeys(filterExpire) : std::vector<std::string>{};
    }

    /**
     * @brief 批量删除键值对
     * @param keysArray 键名数组
     */
    inline bool removeValuesForKeys(CMStringArray &keysArray)
    {
        if (m_MMKV)
        {
            std::vector<std::string> keys;
            for (INT_P i = 0; i < keysArray.GetCount(); ++i)
            {
                keys.push_back(PivW2U{keysArray.GetAt(i)}.str);
            }
            return m_MMKV->removeValuesForKeys(keys);
        }
        return false;
    }

    inline bool removeValuesForKeys(std::vector<std::string> &keysArray)
    {
        if (m_MMKV)
        {
            return m_MMKV->removeValuesForKeys(keysArray);
        }
        return false;
    }

    /**
     * @brief 删除键值对
     * @tparam KeyT
     * @param key 键名
     */
    template <typename KeyT>
    inline bool removeValueForKey(const KeyT &key)
    {
        if (m_MMKV)
        {
            return m_MMKV->removeValueForKey(PivS2V{key});
        }
        return false;
    }

    /**
     * @brief 清空
     */
    inline void clearAll(bool keepSpace = false)
    {
        if (m_MMKV)
        {
            m_MMKV->clearAll(keepSpace);
        }
    }

    /**
     * @brief 释放空间
     */
    inline void trim()
    {
        if (m_MMKV)
        {
            m_MMKV->trim();
        }
    }

    /**
     * @brief 清理缓存
     */
    inline void clearMemoryCache(bool keepSpace = false)
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
    inline void sync(bool syncMode)
    {
        if (m_MMKV)
        {
            m_MMKV->sync(static_cast<SyncFlag>(syncMode));
        }
    }

    /**
     * @brief 锁定
     */
    inline void lock()
    {
        if (m_MMKV)
        {
            m_MMKV->lock();
        }
    }

    /**
     * @brief 解锁
     */
    inline void unlock()
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
    inline bool try_lock()
    {
        return m_MMKV ? m_MMKV->try_lock() : false;
    }

    /**
     * @brief 检查内容更改
     */
    inline void checkContentChanged()
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
    inline bool enableAutoKeyExpire(uint32_t expiredInSeconds = 0)
    {
        return m_MMKV ? m_MMKV->enableAutoKeyExpire(expiredInSeconds) : false;
    }

    /**
     * @brief 禁用键值自动过期
     * @return
     */
    inline bool disableAutoKeyExpire()
    {
        return m_MMKV ? m_MMKV->disableAutoKeyExpire() : false;
    }

}; // class PivMMKV

#endif // _PIV_MMKV_HPP
