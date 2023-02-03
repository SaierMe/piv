/*********************************************\
 * 火山视窗 - MMKV封装类                     *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 版本: 2022/11/28                          *
\*********************************************/

#ifndef _PIV_MMKV_HPP
#define _PIV_MMKV_HPP

#if ((_MSC_VER == 1900) && defined(NTDDI_WIN8))
#error 使用 Visual Studio 2015 编译 MMKV 时,必须将火山视窗的编译选项--"支持WinXP操作系统"设置为真.
#endif

// 包含火山视窗基本类,它在火山里的包含顺序比较靠前(全局-110)
#ifndef __VOL_BASE_H__
#include <sys/base/libs/win_base/vol_base.h>
#endif

// 包含MMKV头文件
#include "include/MMKV.h"
#include "include/MMKVPredef.h"
// 包含自用的文本支持
#include "../src/piv_encoding.hpp"

namespace piv
{
    namespace mmkv
    {
        // 全局初始化
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
            MMKV::initializeMMKV(_rootDir, static_cast<MMKVLogLevel>(MMKVLogInfo));
        }
        // 全局清理
        static void OnExit()
        {
            MMKV::onExit();
        }
        // 取保存目录
        static CVolString GetRootDir()
        {
            return CVolString(MMKV::getRootDir().c_str());
        }
        // 备份表
        static bool BackupOneToDirectory(const wchar_t *mmapID, const wchar_t *dstDir, const wchar_t *srcDir)
        {
            std::wstring _srcDir{srcDir};
            return MMKV::backupOneToDirectory(PivW2U(mmapID), dstDir, _srcDir.empty() ? nullptr : &_srcDir);
        }
        // 恢复表
        static bool RestoreOneFromDirectory(const wchar_t *mmapID, const wchar_t *srcDir, const wchar_t *dstDir)
        {

            std::wstring _dstDir{dstDir};
            return MMKV::restoreOneFromDirectory(PivW2U(mmapID), srcDir, _dstDir.empty() ? nullptr : &_dstDir);
        }
        // 备份所有表
        static int32_t BackupAllToDirectory(const wchar_t *dstDir, const wchar_t *srcDir)
        {
            std::wstring _srcDir{srcDir};
            return static_cast<int32_t>(MMKV::backupAllToDirectory(dstDir, _srcDir.empty() ? nullptr : &_srcDir));
        }
        // 恢复所有表
        static int32_t RestoreAllFromDirectory(const wchar_t *srcDir, const wchar_t *dstDir)
        {
            std::wstring _dstDir{dstDir};
            return static_cast<int32_t>(MMKV::restoreAllFromDirectory(srcDir, _dstDir.empty() ? nullptr : &_dstDir));
        }
        // 校验表文件
        static bool IsFileValid(const wchar_t *mmapID, const wchar_t *relatePath)
        {
            std::wstring _relatePath{relatePath};
            return MMKV::isFileValid(PivW2U(mmapID), _relatePath.empty() ? nullptr : &_relatePath);
        }
        // 置日志级别
        static void SetLogLevel(int32_t level)
        {
            MMKV::setLogLevel(static_cast<MMKVLogLevel>(level));
        }
        // 火山"MMKV事件类"的类指针
        static void *g_mmkvEvent = nullptr;
        // 置"MMKV事件类"指针
        static void SetEventClass(void *pClass)
        {
            g_mmkvEvent = pClass;
        }
        // 取"MMKV事件类"指针
        static void *GetEventClass()
        {
            return g_mmkvEvent;
        }
    } // namespace mmkv
} // namespace piv

// MMKV包装类
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
    // 打开
    int32_t OpenMMKV(const wchar_t *mmapID, int32_t mode, const wchar_t *cryptKey, const wchar_t *rootPath)
    {
        Close();
        if (MMKV::getRootDir().empty() == true)
        {
            piv::mmkv::InitializeMMKV();
        }
        PivW2U _mmapID{mmapID};
        PivW2U _cryptKey{cryptKey};
        std::wstring _rootPath{rootPath};
        if (_mmapID.IsEmpty())
        {
            m_MMKV = MMKV::defaultMMKV(static_cast<MMKVMode>(mode), _cryptKey.IsEmpty() ? nullptr : &_cryptKey.String());
        }
        else
        {
            m_MMKV = MMKV::mmkvWithID(_mmapID, static_cast<MMKVMode>(mode), _cryptKey.IsEmpty() ? nullptr : &_cryptKey.String(),
                                      _rootPath.empty() ? nullptr : &_rootPath);
        }
        if (m_MMKV != nullptr)
        {
            return (m_MMKV->count() > 0) ? 1 : 0;
        }
        else
        {
            return -1;
        }
    }
    // 关闭
    void Close()
    {
        if (m_MMKV)
        {
            m_MMKV->close();
            m_MMKV = nullptr;
        }
    }
    // 是否多进程
    bool IsInterProcess() const
    {
        return m_MMKV ? m_MMKV->m_isInterProcess : false;
    }
    // 取表名称
    CVolString GetMmapID()
    {
        return m_MMKV ? CVolString(PivU2W(m_MMKV->mmapID())) : _CT("");
    }
    // 取加密密钥
    CVolString GetCryptKey()
    {
        return m_MMKV ? CVolString(PivU2W(m_MMKV->cryptKey())) : _CT("");
    }
    // 更改密钥
    bool ReKey(const wchar_t *cryptKey)
    {
        if (m_MMKV)
        {
            return m_MMKV->reKey(PivW2U(cryptKey));
        }
        return false;
    }
    // 检查更改密钥
    void CheckReSetCryptKey(const wchar_t *cryptKey)
    {
        if (m_MMKV)
        {
            PivW2U _cryptKey{cryptKey};
            m_MMKV->checkReSetCryptKey(&_cryptKey.String());
        }
    }
    // 取总大小
    ptrdiff_t GetTotalSize()
    {
        return m_MMKV ? static_cast<ptrdiff_t>(m_MMKV->totalSize()) : 0;
    }
    // 取实际大小
    ptrdiff_t GetActualSize()
    {
        return m_MMKV ? static_cast<ptrdiff_t>(m_MMKV->actualSize()) : 0;
    }

    // 置入逻辑值
    bool SetValue(bool value, const wchar_t *key)
    {
        return m_MMKV ? m_MMKV->set(value, PivW2U(key)) : false;
    }
    // 置入整数值
    bool SetValue(int32_t value, const wchar_t *key)
    {
        return m_MMKV ? m_MMKV->set(value, PivW2U(key)) : false;
    }
    // 置入长整数值
    bool SetValue(int64_t value, const wchar_t *key)
    {
        return m_MMKV ? m_MMKV->set(value, PivW2U(key)) : false;
    }
    // 置入小数值
    bool SetValue(double value, const wchar_t *key)
    {
        return m_MMKV ? m_MMKV->set(value, PivW2U(key)) : false;
    }
    // 置入文本值
    bool SetValue(const wchar_t *value, const wchar_t *key)
    {
        if (m_MMKV)
        {
            mmkv::MMBuffer buffer{reinterpret_cast<void *>(const_cast<wchar_t *>(value)),
                                  (wcslen(value) + 1) * 2, mmkv::MMBufferNoCopy};
            return m_MMKV->set(buffer, PivW2U(key));
        }
        return false;
    }
    // 置入字节集
    bool SetValue(CVolMem &value, const wchar_t *key)
    {
        if (m_MMKV)
        {
            mmkv::MMBuffer buffer{value.GetPtr(), static_cast<size_t>(value.GetSize()), mmkv::MMBufferNoCopy};
            return m_MMKV->set(buffer, PivW2U(key));
        }
        return false;
    }
    // 置入对象值
    bool SetObject(CVolObject &object, const wchar_t *key)
    {
        if (m_MMKV)
        {
            CVolMemoryOutputStream memStream;
            object.VolSaveIntoStream(memStream);
            CVolMem bin = memStream.GetBin(CVolMem());
            mmkv::MMBuffer buffer{bin.GetPtr(), static_cast<size_t>(bin.GetSize()), mmkv::MMBufferNoCopy};
            return m_MMKV->set(buffer, PivW2U(key));
        }
        return false;
    }
    // 取逻辑值
    BOOL GetBool(const wchar_t *key, BOOL defaultValue, BOOL &hasValue)
    {
        hasValue = false;
        if (m_MMKV)
        {
            return m_MMKV->getBool(PivW2U(key), defaultValue ? true : false, reinterpret_cast<bool *>(&hasValue));
        }
        return defaultValue;
    }
    // 取整数值
    int32_t GetInt32(const wchar_t *key, int32_t defaultValue, BOOL &hasValue)
    {
        hasValue = false;
        if (m_MMKV)
        {
            return m_MMKV->getInt32(PivW2U(key), defaultValue, reinterpret_cast<bool *>(&hasValue));
        }
        return defaultValue;
    }
    // 取长整数值
    int64_t GetInt64(const wchar_t *key, int64_t defaultValue, BOOL &hasValue)
    {
        hasValue = false;
        if (m_MMKV)
        {
            return m_MMKV->getInt64(PivW2U(key), defaultValue, reinterpret_cast<bool *>(&hasValue));
        }
        return defaultValue;
    }
    // 取小数值
    double GetDouble(const wchar_t *key, double defaultValue, BOOL &hasValue)
    {
        hasValue = false;
        if (m_MMKV)
        {
            return m_MMKV->getDouble(PivW2U(key), defaultValue, reinterpret_cast<bool *>(&hasValue));
        }
        return defaultValue;
    }
    // 取文本值
    bool GetString(const wchar_t *key, CVolString &value)
    {
        value.Empty();
        if (m_MMKV)
        {
            mmkv::MMBuffer buffer;
            if (m_MMKV->getBytes(PivW2U(key), buffer))
            {
                value.SetText(reinterpret_cast<const wchar_t *>(buffer.getPtr()), buffer.length() / 2);
                return true;
            }
        }
        return false;
    }
    CVolString GetString(const wchar_t *key)
    {
        CVolString result;
        if (m_MMKV)
        {
            GetString(key, result);
            return result;
        }
        return result;
    }
    // 取字节集
    bool GetMem(const wchar_t *key, CVolMem &value)
    {
        value.Empty();
        if (m_MMKV)
        {
            mmkv::MMBuffer buffer;
            if (m_MMKV->getBytes(PivW2U(key), buffer))
            {
                value.Append(buffer.getPtr(), buffer.length());
                return true;
            }
        }
        return false;
    }
    CVolMem GetMem(const wchar_t *key)
    {
        CVolMem result;
        if (m_MMKV)
        {
            GetMem(key, result);
            return result;
        }
        return result;
    }
    // 取对象值
    bool GetObject(const wchar_t *key, CVolObject &object)
    {
        object.ResetToNullObject();
        if (m_MMKV)
        {
            mmkv::MMBuffer buffer;
            if (m_MMKV->getBytes(PivW2U(key), buffer))
            {
                CVolMemoryInputStream memStream;
                memStream.ResetMemory(buffer.getPtr(), buffer.length());
                object.VolLoadFromStream(memStream);
                return true;
            }
        }
        return false;
    }
    // 取键值对大小
    ptrdiff_t GetValueSize(const wchar_t *key, bool actualSize)
    {
        if (m_MMKV)
        {
            return static_cast<ptrdiff_t>(m_MMKV->getValueSize(PivW2U(key), actualSize));
        }
        return 0;
    }
    // 是否存在键
    bool IsContainsKey(const wchar_t *key)
    {
        if (m_MMKV)
        {
            return m_MMKV->containsKey(PivW2U(key));
        }
        return false;
    }
    // 取键值对数量
    ptrdiff_t GetCount()
    {
        return m_MMKV ? static_cast<ptrdiff_t>(m_MMKV->count()) : 0;
    }
    // 取所有键名
    ptrdiff_t GetAllKeys(CMStringArray &keysArray)
    {
        keysArray.RemoveAll();
        if (m_MMKV)
        {
            std::vector<std::string> keys = m_MMKV->allKeys();
            for (auto iter = keys.cbegin(); iter != keys.cend(); ++iter)
            {
                keysArray.Add(PivU2W(*iter));
            }
            return keysArray.GetCount();
        }
        return 0;
    }
    // 批量删除键值对
    void RemoveValuesForKeys(CMStringArray &keysArray)
    {
        if (m_MMKV)
        {
            std::vector<std::string> keys;
            CVolMem buf;
            for (INT_P i = 0; i < keysArray.GetCount(); ++i)
            {
                keys.push_back(PivW2U(keysArray.GetAt(i)));
            }
            m_MMKV->removeValuesForKeys(keys);
        }
    }
    // 删除键值对
    void RemoveValueForKey(const wchar_t *key)
    {
        if (m_MMKV)
        {
            m_MMKV->removeValueForKey(PivW2U(key));
        }
    }
    // 清空
    void ClearAll()
    {
        if (m_MMKV)
        {
            m_MMKV->clearAll();
        }
    }
    // 释放空间
    void Trim()
    {
        if (m_MMKV)
        {
            m_MMKV->trim();
        }
    }
    // 清理缓存
    void ClearMemoryCache()
    {
        if (m_MMKV)
        {
            m_MMKV->clearMemoryCache();
        }
    }
    // 同步
    void Sync(bool syncMode)
    {
        if (m_MMKV)
        {
            m_MMKV->sync(static_cast<SyncFlag>(syncMode));
        }
    }
    // 锁定
    void Lock()
    {
        if (m_MMKV)
        {
            m_MMKV->lock();
        }
    }
    // 解锁
    void UnLock()
    {
        if (m_MMKV)
        {
            m_MMKV->unlock();
        }
    }
    // 解锁
    bool TryLock()
    {
        if (m_MMKV)
        {
            return m_MMKV->try_lock();
        }
        return false;
    }
    // 检查内容更改
    void CheckContentChanged()
    {
        if (m_MMKV)
        {
            m_MMKV->checkContentChanged();
        }
    }
}; // PivMMKV

#endif // _PIV_MMKV_HPP
