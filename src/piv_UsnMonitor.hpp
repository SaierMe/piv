/*********************************************\
 * 火山视窗PIV模块 - 磁盘监视类              *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef _PIV_USN_MONITOR_HPP
#define _PIV_USN_MONITOR_HPP

#include "detail/piv_base.hpp"

#include <winioctl.h>
#include <functional>
#include <mutex>
#include <thread>
#include <map>
#include <set>

class PivUsnMonitor
{
private:
    bool m_IndexPath = false;                                      // 是否索引路径
    bool m_hasPathMap = false;                                     // 是否维护FRN到路径表
    bool m_hasParentMap = false;                                   // 是否维护子到父FRN表
    DWORD m_delay = 0;                                             // 延迟时间
    HANDLE m_hVol = INVALID_HANDLE_VALUE;                          // 卷句柄
    HANDLE m_Event = NULL;                                         // 同步事件(用于退出线程)
    LONGLONG m_StartTime = 0;                                      // 启动文件时间戳
    std::unique_ptr<std::map<DWORDLONG, CVolString>> m_FRNtoPATH;  // 文件FRN到文本名称排序表
    std::unique_ptr<std::map<DWORDLONG, DWORDLONG>> m_FRNtoParent; // 文件FRN到父目录FRN排序表
    std::unique_ptr<std::set<DWORDLONG>> m_MonitorDir;             // 监视目录的FRN集合
    USN_JOURNAL_DATA m_ujd{0};                                     // USN日志数据
    CVolString m_root;                                             // 根路径

    std::function<int32_t(void)> func_Inited;                                                              // 初始化完毕
    std::function<int32_t(CVolString &, CVolString &, FILETIME &, int32_t)> func_Created;                  // 文件被创建
    std::function<int32_t(CVolString &, CVolString &, FILETIME &, int32_t)> func_Deleted;                  // 文件被删除
    std::function<int32_t(CVolString &, CVolString &, int32_t, FILETIME &, int32_t)> func_DataChanged;     // 文件内容改变
    std::function<int32_t(CVolString &, CVolString &, CVolString &, FILETIME &, int32_t)> func_Renamed;    // 文件被重命名
    std::function<int32_t(CVolString &, CVolString &, int32_t, FILETIME &, int32_t)> func_AttributeChange; // 文件属性改变
    std::function<int32_t(CVolString &, CVolString &, int32_t, FILETIME &, int32_t)> func_OtherChange;     // 其他状态改变

    typedef struct _IO_STATUS_BLOCK
    {
        union
        {
            LONG Status;
            PVOID Pointer;
        };
        ULONG_PTR Information;
    } IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

    typedef struct _UNICODE_STRING
    {
        USHORT Length, MaximumLength;
        PWCH Buffer;
    } UNICODE_STRING, *PUNICODE_STRING;

    typedef struct _OBJECT_ATTRIBUTES
    {
        ULONG Length;
        HANDLE RootDirectory;
        PUNICODE_STRING ObjectName;
        ULONG Attributes;
        PVOID SecurityDescriptor;
        PVOID SecurityQualityOfService;
    } OBJECT_ATTRIBUTES;

    typedef ULONG(__stdcall *PNtCreateFile)(PHANDLE FileHandle, ULONG DesiredAccess, PVOID ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock,
                                            PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PVOID EaBuffer, ULONG EaLength);
    typedef LONG(__stdcall *PNtQueryInformationFile)(HANDLE FileHandle, PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation, DWORD Length, DWORD FileInformationClass);

    PNtQueryInformationFile fNtQueryInformationFile = (PNtQueryInformationFile)GetProcAddress(::GetModuleHandleW(L"ntdll.dll"), "NtQueryInformationFile");
    PNtCreateFile fNtCreatefile = (PNtCreateFile)GetProcAddress(::GetModuleHandleW(L"ntdll.dll"), "NtCreateFile");

    /**
     * @brief 文件引用编号到路径
     * @param frn 文件引用编号
     * @param path 返回路径
     */
    void PathFromFRN(const DWORDLONG &frn, CVolString &path)
    {
        if (!m_hasPathMap)
        {
            if (!fNtCreatefile || !fNtQueryInformationFile)
                return;
            UNICODE_STRING fidstr = {8, 8, (PWSTR)&frn};
            // OBJ_CASE_INSENSITIVE = 0x00000040UL;
            OBJECT_ATTRIBUTES oa = {sizeof(OBJECT_ATTRIBUTES), m_hVol, &fidstr, 0x00000040UL, 0, 0};
            HANDLE hFile;
            IO_STATUS_BLOCK IoStatus;
            // FILE_OPEN = 0x00000001UL;
            // FILE_OPEN_BY_FILE_ID = 0x00002000UL;
            if (fNtCreatefile(&hFile, GENERIC_READ, &oa, &IoStatus, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE, 0x00000001UL, 0x00002000UL, NULL, 0) == 0)
            {
                PivBuffer<WCHAR, DWORD> buf{MAX_PATH * 4};
                if (fNtQueryInformationFile(hFile, &IoStatus, buf.GetPtr(), buf.GetSize(), 9) == 0)
                {
                    path.SetText(m_root);
                    path.AddText(buf.GetPtr() + 3, *buf.Get<DWORD>() / 2 - 1);
                    path.CheckAddPathChar();
                }
                ::CloseHandle(hFile);
            }
        }
        else
        {
            auto it = m_FRNtoPATH->find(frn);
            if (it != m_FRNtoPATH->end())
            {
                path.InsertChar(0, '\\');
                path.InsertText(0, it->second);
                auto it2 = m_FRNtoParent->find(it->first);
                if (it2 != m_FRNtoParent->end())
                    PathFromFRN(it2->second, path);
            }
            else
            {
                path.InsertText(0, m_root);
            }
        }
    }

    /**
     * @brief 路径到文件引用编号
     * @param path 文件路径
     * @return 文件引用编号
     */
    DWORDLONG PathToFRN(const WCHAR *path)
    {
        DWORDLONG ret = 0;
        HANDLE hDir = ::CreateFileW(path, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
        if (hDir != INVALID_HANDLE_VALUE)
        {
            BY_HANDLE_FILE_INFORMATION info;
            if (::GetFileInformationByHandle(hDir, &info))
                ret = (DWORDLONG)info.nFileIndexLow | ((DWORDLONG)info.nFileIndexHigh << 32);
            ::CloseHandle(hDir);
        }
        return ret;
    }

    /**
     * @brief 判断指定的文件引用编号需要监视
     * @param frn 文件引用编号
     * @return
     */
    bool IsMonitored(const DWORDLONG &frn)
    {
        if (!m_MonitorDir)
            return true;
        if (m_MonitorDir->find(frn) != m_MonitorDir->end())
            return true;
        auto it = m_FRNtoParent->find(frn);
        if (it != m_FRNtoParent->end())
            return IsMonitored(it->second);
        return false;
    }

    /**
     * @brief 初始化USN日志
     * @return
     */
    bool InitUsn()
    {
        if (m_hVol == INVALID_HANDLE_VALUE)
            return false;
        CREATE_USN_JOURNAL_DATA cujd{0, 0};
        // 初始化USN日志文件
        if (::DeviceIoControl(m_hVol, FSCTL_CREATE_USN_JOURNAL, &cujd, sizeof(cujd), NULL, 0, NULL, NULL) == FALSE)
            return false;
        // 获取USN日志基本信息
        if (::DeviceIoControl(m_hVol, FSCTL_QUERY_USN_JOURNAL, NULL, 0, &m_ujd, sizeof(m_ujd), NULL, NULL) == FALSE)
            return false;
        m_StartTime = (LONGLONG)(time(nullptr) + 11644473600) * 10000000;
        return true;
    }

    /**
     * @brief 删除USN日志(删除后会影响其他使用USN日志的软件)
     * @return
     */
    bool DeleteUsn()
    {
        if (m_hVol == INVALID_HANDLE_VALUE)
            return false;
        DELETE_USN_JOURNAL_DATA dujd{m_ujd.UsnJournalID, USN_DELETE_FLAG_DELETE};
        DWORD br;
        return ::DeviceIoControl(m_hVol, FSCTL_DELETE_USN_JOURNAL, &dujd, sizeof(dujd), NULL, 0, &br, NULL);
    }

    /**
     * @brief 索引路径名称
     */
    void IndexPath()
    {
        if (m_hVol == INVALID_HANDLE_VALUE)
            return;
        if (m_hasPathMap)
            m_FRNtoPATH.reset(new std::map<DWORDLONG, CVolString>);
        m_FRNtoParent.reset(new std::map<DWORDLONG, DWORDLONG>);
        MFT_ENUM_DATA med{0};
        med.StartFileReferenceNumber = 0;
        med.LowUsn = 0;            // m_ujd.FirstUsn
        med.HighUsn = MAXLONGLONG; // m_ujd.NextUsn
        PivBuffer<CHAR, DWORD> buffer{10240, true};
        DWORD dwBytes = 0;
        PUSN_RECORD UsnRecord;
        while (::DeviceIoControl(m_hVol, FSCTL_ENUM_USN_DATA, &med, sizeof(med), buffer.GetPtr(), buffer.GetSize(), &dwBytes, NULL))
        {
            DWORD dwRetBytes = dwBytes - sizeof(USN);
            UsnRecord = (PUSN_RECORD)(buffer.GetPtr() + sizeof(USN));
            while (dwRetBytes > 0)
            {
                if (UsnRecord->FileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    m_FRNtoParent->emplace(UsnRecord->FileReferenceNumber, UsnRecord->ParentFileReferenceNumber);
                    if (m_hasPathMap)
                        m_FRNtoPATH->emplace(std::piecewise_construct, std::forward_as_tuple(UsnRecord->FileReferenceNumber),
                                             std::forward_as_tuple((const WCHAR *)((PBYTE)UsnRecord + UsnRecord->FileNameOffset), UsnRecord->FileNameLength / 2));
                }
                // 获取下一个记录
                dwRetBytes -= UsnRecord->RecordLength;
                UsnRecord = (PUSN_RECORD)(((CHAR *)UsnRecord) + UsnRecord->RecordLength);
            }
            med.StartFileReferenceNumber = *buffer.Get<USN>();
        }
        MonitorUsn();
    }

    /**
     * @brief 监视USN日志
     */
    void MonitorUsn()
    {
        if (m_hVol == INVALID_HANDLE_VALUE)
            return;
        if (func_Inited)
            func_Inited();
    ReadRecord:
        READ_USN_JOURNAL_DATA ReadData = {0, 0xFFFFFFFF, 0, 0, 0};
        ReadData.UsnJournalID = m_ujd.UsnJournalID;
        PivBuffer<CHAR, DWORD> buffer{4096, true};
        DWORD dwBytes = 0, dwRetBytes = 0;
        PUSN_RECORD UsnRecord;
        CVolString OldName;
        while (::DeviceIoControl(m_hVol, FSCTL_READ_USN_JOURNAL, &ReadData, sizeof(ReadData), buffer.GetPtr(), buffer.GetSize(), &dwBytes, NULL))
        {
            CVolString filename;
            CVolString path;
            dwRetBytes = dwBytes - sizeof(USN);
            // 找到第一个 USN 记录
            UsnRecord = (PUSN_RECORD)(buffer.GetPtr() + sizeof(USN));
            if (::WaitForSingleObject(m_Event, dwRetBytes ? 0 : m_delay) == WAIT_OBJECT_0)
                return;
            while (dwRetBytes > 0)
            {
                // 跳过开始监视之前的日志
                if (UsnRecord->TimeStamp.QuadPart < m_StartTime)
                    goto NextRecord;
                // 文件或目录更改后都会关闭句柄,以此判断更改是否已完成
                if (UsnRecord->Reason & USN_REASON_CLOSE)
                {
                    // 检查是否限定了监视目录
                    if (!IsMonitored(UsnRecord->ParentFileReferenceNumber))
                        goto NextRecord;
                    bool isDir = ((m_hasParentMap || m_hasPathMap) && (UsnRecord->FileAttributes & FILE_ATTRIBUTE_DIRECTORY));
                    // 获取路径
                    path.Empty();
                    if (m_IndexPath)
                        PathFromFRN(UsnRecord->ParentFileReferenceNumber, path);
                    // 获取文件或目录名称
                    filename.SetText((const WCHAR *)((PBYTE)UsnRecord + UsnRecord->FileNameOffset), UsnRecord->FileNameLength / 2);
                    // 以下为文件或更改事件
                    if (UsnRecord->Reason & USN_REASON_FILE_CREATE)
                    {
                        if (isDir)
                        {
                            m_FRNtoParent->emplace(UsnRecord->FileReferenceNumber, UsnRecord->ParentFileReferenceNumber);
                            if (m_FRNtoPATH)
                                (*m_FRNtoPATH)[UsnRecord->FileReferenceNumber] = filename;
                        }
                        if (func_Created)
                            func_Created(filename, path, FILETIME{UsnRecord->TimeStamp.LowPart, (DWORD)UsnRecord->TimeStamp.HighPart}, static_cast<int32_t>(UsnRecord->FileAttributes));
                    }
                    else if (UsnRecord->Reason & USN_REASON_FILE_DELETE)
                    {
                        if (isDir)
                        {
                            m_FRNtoParent->erase(UsnRecord->FileReferenceNumber);
                            if (m_FRNtoPATH)
                                m_FRNtoPATH->erase(UsnRecord->FileReferenceNumber);
                        }
                        if (func_Deleted)
                            func_Deleted(filename, path, FILETIME{UsnRecord->TimeStamp.LowPart, (DWORD)UsnRecord->TimeStamp.HighPart}, static_cast<int32_t>(UsnRecord->FileAttributes));
                    }
                    else if (UsnRecord->Reason & USN_REASON_DATA_EXTEND || UsnRecord->Reason & USN_REASON_DATA_OVERWRITE || UsnRecord->Reason & USN_REASON_DATA_TRUNCATION ||
                             UsnRecord->Reason & USN_REASON_NAMED_DATA_EXTEND || UsnRecord->Reason & USN_REASON_NAMED_DATA_OVERWRITE || UsnRecord->Reason & USN_REASON_NAMED_DATA_TRUNCATION)
                    {
                        if (func_DataChanged)
                            func_DataChanged(filename, path, static_cast<int32_t>(UsnRecord->Reason), FILETIME{UsnRecord->TimeStamp.LowPart, (DWORD)UsnRecord->TimeStamp.HighPart}, static_cast<int32_t>(UsnRecord->FileAttributes));
                    }
                    else if (UsnRecord->Reason & USN_REASON_RENAME_NEW_NAME)
                    {
                        if (isDir && m_FRNtoPATH)
                        {
                            (*m_FRNtoPATH)[UsnRecord->FileReferenceNumber] = filename;
                        }
                        if (func_Renamed)
                            func_Renamed(OldName, filename, path, FILETIME{UsnRecord->TimeStamp.LowPart, (DWORD)UsnRecord->TimeStamp.HighPart}, static_cast<int32_t>(UsnRecord->FileAttributes));
                    }
                    else if (UsnRecord->Reason & USN_REASON_BASIC_INFO_CHANGE || UsnRecord->Reason & USN_REASON_EA_CHANGE || UsnRecord->Reason & USN_REASON_INDEXABLE_CHANGE ||
                             UsnRecord->Reason & USN_REASON_SECURITY_CHANGE || UsnRecord->Reason & USN_REASON_REPARSE_POINT_CHANGE || UsnRecord->Reason & 0x00800000)
                    {
                        if (func_AttributeChange)
                            func_AttributeChange(filename, path, static_cast<int32_t>(UsnRecord->Reason), FILETIME{UsnRecord->TimeStamp.LowPart, (DWORD)UsnRecord->TimeStamp.HighPart}, static_cast<int32_t>(UsnRecord->FileAttributes));
                    }
                    else
                    {
                        if (func_OtherChange)
                            func_OtherChange(filename, path, static_cast<int32_t>(UsnRecord->Reason), FILETIME{UsnRecord->TimeStamp.LowPart, (DWORD)UsnRecord->TimeStamp.HighPart}, static_cast<int32_t>(UsnRecord->FileAttributes));
                    }
                }
                else if (UsnRecord->Reason & USN_REASON_RENAME_OLD_NAME) // 记住重命名前的旧名称
                {
                    // 检查是否限定了监视目录
                    if (!IsMonitored(UsnRecord->ParentFileReferenceNumber))
                        goto NextRecord;
                    OldName.SetText((const WCHAR *)((PBYTE)UsnRecord + UsnRecord->FileNameOffset), UsnRecord->FileNameLength / 2);
                }
            // 获取下一个记录
            NextRecord:
                dwRetBytes -= UsnRecord->RecordLength;
                UsnRecord = (PUSN_RECORD)(((PCHAR)UsnRecord) + UsnRecord->RecordLength);
            }
            ReadData.StartUsn = *buffer.Get<USN>();
        }
        // 可能是其他程序关闭了USN日志,尝试重新创建USN日志
        if (InitUsn())
            goto ReadRecord;
    }

public:
    PivUsnMonitor()
    {
        m_Event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    }
    ~PivUsnMonitor()
    {
        Stop();
        ::CloseHandle(m_Event);
    }

    /**
     * @brief 带参构造并开始监视
     * @param drive_letter 盘符
     * @param monitor_dirs 监视目录组
     * @param delay 延迟时间
     * @param index_path 索引完整路径
     * @return 是否成功
     */
    PivUsnMonitor(const wchar_t &drive_letter, CMStringArray &monitor_dirs, const int32_t &delay = 1000, const int32_t &index_path = true)
    {
        m_Event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
        Start(drive_letter, monitor_dirs, delay, index_path);
    }

    /**
     * @brief 是否已开始监视
     * @return
     */
    bool IsStart()
    {
        return m_hVol != INVALID_HANDLE_VALUE;
    }

    /**
     * @brief 开始监视
     * @param drive_letter 盘符
     * @param monitor_dirs 监视目录组
     * @param delay 延迟时间
     * @param index_path 索引完整路径
     * @return 是否成功
     */
    bool Start(const wchar_t &drive_letter, CMStringArray &monitor_dirs, const int32_t &delay = 1000, const int32_t &index_path = 1)
    {
        Stop();
        m_root.Empty();
        m_root.AddChar(drive_letter);
        m_root.AddText(L":\\");
        // 检查是否为NTFS文件系统
        WCHAR FileSystemName[MAX_PATH + 1]{0};
        if (::GetVolumeInformationW(m_root.GetText(), NULL, 0, NULL, NULL, NULL, FileSystemName, MAX_PATH))
        {
            if (wcscmp(FileSystemName, L"NTFS") != 0)
                return FALSE;
        }
        // 检查是否为本地磁盘
        if (::GetDriveTypeW(m_root.GetText()) != DRIVE_FIXED)
            return FALSE;
        // 打开设备句柄
        m_hVol = ::CreateFileW(CVolString().Format(L"\\\\.\\%c:", drive_letter).GetText(), GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
        if (m_hVol == INVALID_HANDLE_VALUE)
        {
            _DEBUG_STATMENT(DebugMessageBox(FALSE, 0, 0, _T("S"),
                                            L"“文件监视类”初始化失败！\r\n注意：欲使用本类必须以管理员身份运行程序：\r\n"
                                            L"1、在项目属性中，将“以管理员权限运行”设置为真；\r\n"
                                            L"2、调试运行时同样需要以管理员身份运行火山软件开发平台；"));
            return FALSE;
        }
        m_delay = static_cast<DWORD>(delay);
        m_IndexPath = (index_path != 0);
        // 初始化USN日志
        if (!InitUsn())
            goto Init_Failed;
        // 初始化监视目录组
        m_hasPathMap = (index_path == 2);
        if (monitor_dirs.IsEmpty() == FALSE)
        {
            m_MonitorDir.reset(new std::set<DWORDLONG>);
            CVolString dir;
            for (INT_P i = 0; i < monitor_dirs.GetCount(); i++)
            {
                dir.SetText(monitor_dirs.GetAt(i));
                if (dir.LeadOf(m_root, FALSE))
                {
                    DWORDLONG frn = PathToFRN(dir.GetText());
                    if (frn != 0)
                        m_MonitorDir->insert(frn);
                }
            }
            if (!m_MonitorDir->empty())
                m_hasParentMap = true;
            else
                m_MonitorDir.reset(nullptr);
        }
        // 初始化成功
        ::ResetEvent(m_Event);
        if (m_hasParentMap || m_hasPathMap)
            std::thread(&PivUsnMonitor::IndexPath, this).detach();
        else
            std::thread(&PivUsnMonitor::MonitorUsn, this).detach();
        return true;
    // 初始化失败后清理句柄
    Init_Failed:
        ::CloseHandle(m_hVol);
        m_hVol = INVALID_HANDLE_VALUE;
        return false;
    }

    /**
     * @brief 停止监视
     * @param delete_usn 是否删除(禁用)USN日志
     */
    void Stop(const bool &delete_usn = false)
    {
        ::SetEvent(m_Event);
        if (m_hVol != INVALID_HANDLE_VALUE)
        {
            if (delete_usn)
                DeleteUsn();
            ::CloseHandle(m_hVol);
            m_hVol = INVALID_HANDLE_VALUE;
        }
        m_FRNtoPATH.reset(nullptr);
        m_FRNtoParent.reset(nullptr);
        m_MonitorDir.reset(nullptr);
        func_Inited = nullptr;
        func_Created = nullptr;
        func_Deleted = nullptr;
        func_DataChanged = nullptr;
        func_Renamed = nullptr;
        func_AttributeChange = nullptr;
        func_OtherChange = nullptr;
    }

    /**
     * @brief 绑定初始化完毕事件
     * @param fun 执行函数
     * @param ...args 参数列表
     * @return
     */
    template <typename Fun, typename... Args>
    inline void BindInited(Fun &&fun, Args &&...args)
    {
        func_Inited = std::bind(std::forward<Fun>(fun), std::forward<Args>(args)...);
    }

    /**
     * @brief 绑定文件被创建事件
     * @param fun 执行函数
     * @param ...args 参数列表
     * @return
     */
    template <typename Fun, typename... Args>
    inline void BindCreated(Fun &&fun, Args &&...args)
    {
        func_Created = std::bind(std::forward<Fun>(fun), std::forward<Args>(args)...);
    }

    /**
     * @brief 绑定文件被删除事件
     * @param fun 执行函数
     * @param ...args 参数列表
     * @return
     */
    template <typename Fun, typename... Args>
    inline void BindDeleted(Fun &&fun, Args &&...args)
    {
        func_Deleted = std::bind(std::forward<Fun>(fun), std::forward<Args>(args)...);
    }

    /**
     * @brief 绑定文件内容改变事件
     * @param fun 执行函数
     * @param ...args 参数列表
     * @return
     */
    template <typename Fun, typename... Args>
    inline void BindDataChanged(Fun &&fun, Args &&...args)
    {
        func_DataChanged = std::bind(std::forward<Fun>(fun), std::forward<Args>(args)...);
    }

    /**
     * @brief 绑定文件被重命名事件
     * @param fun 执行函数
     * @param ...args 参数列表
     * @return
     */
    template <typename Fun, typename... Args>
    inline void BindRenamed(Fun &&fun, Args &&...args)
    {
        func_Renamed = std::bind(std::forward<Fun>(fun), std::forward<Args>(args)...);
    }

    /**
     * @brief 绑定文件属性改变事件
     * @param fun 执行函数
     * @param ...args 参数列表
     * @return
     */
    template <typename Fun, typename... Args>
    inline void BindAttributeChange(Fun &&fun, Args &&...args)
    {
        func_AttributeChange = std::bind(std::forward<Fun>(fun), std::forward<Args>(args)...);
    }

    /**
     * @brief 绑定其他状态改变事件
     * @param fun 执行函数
     * @param ...args 参数列表
     * @return
     */
    template <typename Fun, typename... Args>
    inline void BindOtherChange(Fun &&fun, Args &&...args)
    {
        func_OtherChange = std::bind(std::forward<Fun>(fun), std::forward<Args>(args)...);
    }
}; // PivUsnMonitor

#endif // _PIV_USN_MONITOR_HPP
