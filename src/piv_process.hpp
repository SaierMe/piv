/*********************************************\
 * 火山视窗PIV模块 - 进程操作类              *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef _PIV_PROCESS_HPP
#define _PIV_PROCESS_HPP

#ifndef __VOL_BASE_H__
    #include <sys/base/libs/win_base/vol_base.h>
#endif

#include <cstdint>
#include <psapi.h>
#include <winternl.h>
#pragma comment(lib, "Psapi.lib")
#include <memory>

class PivProcess
{
private:
    BOOL m_need_close = true;
    HANDLE m_process = NULL;
    const BYTE* m_process_start = NULL;
    const BYTE* m_process_end = NULL;
    DWORD m_pid = 0;

    const uint32_t BLOCKMAXSIZE{409600};
    typedef BOOL(WINAPI* Piv_GetProcessDEPPolicy)(HANDLE, LPDWORD, PBOOL);
    typedef BOOL(WINAPI* Piv_IsWow64Process)(HANDLE, PBOOL);
    typedef NTSTATUS(NTAPI* Piv_ZwQueryInformationProcess)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);
    typedef NTSTATUS(NTAPI* Piv_NtCreateThreadEx)(OUT PHANDLE hThread, IN ACCESS_MASK DesiredAccess, IN PVOID ObjectAttributes,
                                                  IN HANDLE ProcessHandle, IN PVOID lpStartAddress, IN PVOID lpParameter, IN ULONG Flags,
                                                  IN SIZE_T StackZeroBits, IN SIZE_T SizeOfStackCommit, IN SIZE_T SizeOfStackReserve, OUT PVOID lpBytesBuffer);

    // 取首模块信息
    BOOL get_base_module_info()
    {
        if (m_process == NULL)
            return FALSE;
        CMArray<HMODULE> hModuleArray;
        DWORD dwNeeded = 0;
        ::EnumProcessModulesEx(m_process, NULL, 0, &dwNeeded, 3);
        hModuleArray.InitCount(dwNeeded / sizeof(HMODULE), TRUE);
        ::EnumProcessModulesEx(m_process, hModuleArray.GetData(), dwNeeded, &dwNeeded, 3);
        if (hModuleArray.GetCount() <= 0)
            return FALSE;
        MODULEINFO sModuleInfo{0};
        if (!::GetModuleInformation(m_process, hModuleArray.GetAt(0), &sModuleInfo, sizeof(sModuleInfo)))
            return FALSE;
        m_process_start = static_cast<const BYTE*>(sModuleInfo.lpBaseOfDll);
        if (!::GetModuleInformation(m_process, hModuleArray.GetLastElement(), &sModuleInfo, sizeof(sModuleInfo)))
            return FALSE;
        m_process_end = static_cast<const BYTE*>(sModuleInfo.lpBaseOfDll) + sModuleInfo.SizeOfImage;
        return TRUE;
    }

public:
    PivProcess() {}
    PivProcess(DWORD process_id, DWORD desired_access)
    {
        open_process(process_id, desired_access);
    }
    ~PivProcess()
    {
        close_process();
    }

    // 打开进程
    BOOL open_process(DWORD process_id, DWORD desired_access)
    {
        close_process();
        m_pid = process_id ? process_id : ::GetCurrentProcessId();
        m_process = ::OpenProcess(desired_access, FALSE, m_pid);
        if (m_process == NULL)
            return FALSE;
        get_base_module_info();
        return TRUE;
    }

    // 打开自句柄
    inline BOOL open_handle(HANDLE process, BOOL auto_close = false)
    {
        close_process();
        m_process = process;
        m_pid = ::GetProcessId(m_process);
        /*
        if (!get_base_module_info())
        {
            close_process();
            return FALSE;
        }
        */
        m_need_close = auto_close;
        return TRUE;
    }

    // 关闭进程
    inline void close_process()
    {
        if (m_need_close && m_process != NULL)
            ::CloseHandle(m_process);
        m_process = NULL;
        m_process_start = NULL;
        m_process_end = NULL;
        m_pid = 0;
        m_need_close = true;
    }

    // 取进程句柄
    inline HANDLE get_process_handle() const noexcept
    {
        return m_process;
    }

    // 取进程ID
    inline DWORD get_process_id() const noexcept
    {
        return m_pid;
    }

    // 取句柄数量
    inline DWORD get_handle_count() const noexcept
    {
        DWORD dwHandleCount = 0;
        if (m_process != NULL)
            ::GetProcessHandleCount(m_process, &dwHandleCount);
        return dwHandleCount;
    }

    // 取创建时间
    double get_creation_time(int32_t time_zoon) const
    {
        double dtime = 0.0;
        if (m_process != NULL)
        {
            FILETIME CreationTime, ExitTime, KernelTime, UserTime;
            if (::GetProcessTimes(m_process, &CreationTime, &ExitTime, &KernelTime, &UserTime))
            {
                SYSTEMTIME st;
                ::FileTimeToSystemTime(&CreationTime, &st);
                ::SystemTimeToVariantTime(&st, &dtime);
                dtime = TimeChange(dtime, 5, time_zoon);
            }
        }
        return dtime;
    }

    // 取内存使用情况
    inline BOOL get_memory_info(PROCESS_MEMORY_COUNTERS* pMemCounters)
    {
        if (m_process != NULL && pMemCounters != nullptr)
        {
            pMemCounters->cb = sizeof(PROCESS_MEMORY_COUNTERS);
            return ::GetProcessMemoryInfo(m_process, pMemCounters, sizeof(PROCESS_MEMORY_COUNTERS));
        }
        return FALSE;
    }

    // 取优先级
    inline DWORD get_priority_class()
    {
        return m_process ? ::GetPriorityClass(m_process) : 0;
    }

    // 置优先级
    inline BOOL set_priority_class(DWORD priority)
    {
        return ::SetPriorityClass(m_process, priority);
    }

    // 取进程DEP策略
    inline BOOL get_dep_policy(DWORD& flags, BOOL& is_permanet)
    {
        flags = 0;
        is_permanet = FALSE;
        if (m_process != NULL)
        {
            static Piv_GetProcessDEPPolicy pfn = reinterpret_cast<Piv_GetProcessDEPPolicy>(
                ::GetProcAddress(::GetModuleHandleW(L"Kernel32.dll"), "GetProcessDEPPolicy"));
            if (pfn)
            {
                return pfn(m_process, &flags, &is_permanet);
            }
        }
        return FALSE;
    }

    // 是否为WOW64模式
    inline BOOL is_wow64(BOOL currentProcess = TRUE)
    {
        BOOL result = FALSE;
        if (m_process != NULL)
        {
            static Piv_IsWow64Process pfn = reinterpret_cast<Piv_IsWow64Process>(
                ::GetProcAddress(::GetModuleHandle(L"Kernel32.dll"), "IsWow64Process"));
            if (pfn)
            {
                pfn(currentProcess ? ::GetCurrentProcess() : m_process, &result);
            }
        }
        return result;
    }

    // 取命令行
    CVolString get_commandline()
    {
        CVolString result;
        if (m_process != NULL)
        {
            Piv_ZwQueryInformationProcess pfn = reinterpret_cast<Piv_ZwQueryInformationProcess>(
                ::GetProcAddress(::GetModuleHandleW(L"ntdll.dll"), "ZwQueryInformationProcess"));
            if (pfn)
            {
                PROCESS_BASIC_INFORMATION BasicInfo{0};
                PEB PebBaseInfo{0};
                RTL_USER_PROCESS_PARAMETERS ProcessParameters{0};
                if (pfn(m_process, PROCESSINFOCLASS::ProcessBasicInformation, &BasicInfo, sizeof(BasicInfo), NULL) == 0)
                {
                    if (::ReadProcessMemory(m_process, BasicInfo.PebBaseAddress, &PebBaseInfo, sizeof(PebBaseInfo), NULL) &&
                        ::ReadProcessMemory(m_process, PebBaseInfo.ProcessParameters, &ProcessParameters, sizeof(ProcessParameters), NULL))
                    {
                        DWORD dwSize = ProcessParameters.CommandLine.Length + 2;
                        WCHAR* lpBuffer = (WCHAR*)calloc(dwSize, sizeof(WCHAR));
                        if (::ReadProcessMemory(m_process, ProcessParameters.CommandLine.Buffer, lpBuffer,
                                                ProcessParameters.CommandLine.Length, NULL))
                        {
                            result.SetText(lpBuffer, dwSize);
                        }
                        free(lpBuffer);
                    }
                }
            }
        }
        return result;
    }

    // 取模块句柄
    HMODULE get_module_handle(const wchar_t* module_name)
    {
        if (m_process != NULL)
        {
            CMArray<HMODULE> hModuleArray;
            DWORD dwNeeded = 0;
            ::EnumProcessModulesEx(m_process, NULL, 0, &dwNeeded, 3);
            hModuleArray.InitCount(dwNeeded / sizeof(HMODULE), TRUE);
            ::EnumProcessModulesEx(m_process, hModuleArray.GetData(), dwNeeded, &dwNeeded, 3);
            CVolString module_name_sv{module_name};
            wchar_t buf[MAX_PATH];
            if (module_name_sv.IsEmpty())
            {
                ::GetModuleBaseNameW(m_process, 0, buf, MAX_PATH);
                module_name_sv.SetText(buf);
            }
            CVolString sFileName;
            for (INT_P i = 0; i < hModuleArray.GetCount(); i++)
            {
                ::GetModuleFileNameExW(m_process, hModuleArray.GetAt(i), buf, MAX_PATH);
                sFileName.SetText(buf);
                if (sFileName.EndOf(module_name_sv.GetText(), FALSE) == TRUE)
                    return hModuleArray.GetAt(i);
            }
        }
        return 0;
    }

    // 取模块基址
    inline void* get_module_base(HMODULE hMoudle)
    {
        if (m_process != NULL)
        {
            MODULEINFO sModuleInfo{0};
            if (::GetModuleInformation(m_process, hMoudle, &sModuleInfo, sizeof(sModuleInfo)))
            {
                return sModuleInfo.lpBaseOfDll;
            }
        }
        return NULL;
    }

    // 枚举模块句柄
    int32_t enum_modules(CMArray<INT_P>& module_array, DWORD filter_flag)
    {
        module_array.RemoveAll();
        if (m_process != NULL)
        {
            DWORD dwNeeded = 0;
            ::EnumProcessModulesEx(m_process, NULL, 0, &dwNeeded, filter_flag);
            HMODULE* lphModule = (HMODULE*)malloc(dwNeeded);
            if (lphModule == NULL)
                return 0;
            ::EnumProcessModulesEx(m_process, lphModule, dwNeeded, &dwNeeded, filter_flag);
            size_t nModuleCount = dwNeeded / sizeof(HMODULE);
            for (size_t i = 0; i < nModuleCount; i++)
            {
                module_array.Add(reinterpret_cast<INT_P>(lphModule[i]));
            }
            free(lphModule);
        }
        return static_cast<int32_t>(module_array.GetCount());
    }

    // 枚举模块名称
    int32_t enum_modules(CMArray<INT_P>& module_array, CMStringArray& name_array, BOOL is_fullpath, DWORD filter_flag)
    {
        module_array.RemoveAll();
        name_array.RemoveAll();
        if (m_process != NULL)
        {
            DWORD dwNeeded = 0;
            ::EnumProcessModulesEx(m_process, NULL, 0, &dwNeeded, filter_flag);
            HMODULE* lphModule = (HMODULE*)malloc(dwNeeded);
            if (lphModule == NULL)
                return 0;
            ::EnumProcessModulesEx(m_process, lphModule, dwNeeded, &dwNeeded, filter_flag);
            size_t nModuleCount = dwNeeded / sizeof(HMODULE);
            WCHAR szFileName[MAX_PATH + 1]{'\0'};
            for (size_t i = 0; i < nModuleCount; i++)
            {
                if (is_fullpath)
                    ::GetModuleFileNameExW(m_process, lphModule[i], szFileName, MAX_PATH);
                else
                    ::GetModuleBaseNameW(m_process, lphModule[i], szFileName, MAX_PATH);
                name_array.Add(szFileName);
                module_array.Add(reinterpret_cast<INT_P>(lphModule[i]));
            }
            free(lphModule);
        }
        return static_cast<int32_t>(module_array.GetCount());
    }

    // 取模块信息
    inline BOOL get_module_info(HMODULE hModule, MODULEINFO* pModuleInfo)
    {
        if (m_process != NULL)
        {
            return ::GetModuleInformation(m_process, hModule, pModuleInfo, sizeof(MODULEINFO));
        }
        return false;
    }

    // 取模块名称
    inline CVolString get_module_name(HMODULE hModule)
    {
        if (m_process != NULL)
        {
            CVolString BaseName;
            BaseName.m_mem.Alloc((MAX_PATH + 1) * 2, TRUE);
            if (::GetModuleBaseNameW(m_process, hModule, const_cast<wchar_t*>(BaseName.GetText()), MAX_PATH) > 0)
            {
                return BaseName;
            }
        }
        return CWString(L"");
    }

    // 取模块文件名
    inline CVolString get_module_filename(HMODULE hModule)
    {
        if (m_process != NULL)
        {
            CVolString FileName;
            FileName.m_mem.Alloc(MAX_PATH * 6, TRUE);
            if (::GetModuleFileNameExW(m_process, hModule, const_cast<wchar_t*>(FileName.GetText()), MAX_PATH * 3) > 0)
            {
                return FileName;
            }
        }
        return CWString(L"");
    }

    // 取模块路径
    inline CVolString get_module_path(HMODULE hModule)
    {
        if (m_process != NULL)
        {
            CVolString BaseName, FileName;
            BaseName.m_mem.Alloc(MAX_PATH * 6, TRUE);
            FileName.m_mem.Alloc(MAX_PATH * 6, TRUE);
            if (::GetModuleFileNameExW(m_process, hModule, const_cast<wchar_t*>(FileName.GetText()), MAX_PATH * 3) > 0 &&
                ::GetModuleBaseNameW(m_process, hModule, const_cast<wchar_t*>(BaseName.GetText()), MAX_PATH * 3) > 0)
            {
                INT_P pos = FileName.SearchText(BaseName.GetText(), -1, FALSE, TRUE);
                if (pos != -1)
                    return FileName.Left(pos);
            }
        }
        return CWString(L"");
    }

    // 取内存映射文件
    inline CVolString get_mapped_filename(void* mem_ptr)
    {
        if (m_process != NULL)
        {
            CVolString FileName;
            FileName.m_mem.Alloc(MAX_PATH * 6, TRUE);
            if (::GetMappedFileNameW(m_process, mem_ptr, const_cast<wchar_t*>(FileName.GetText()), MAX_PATH * 3) > 0)
            {
                return FileName;
            }
        }
        return CWString(L"");
    }

    // 取进程映像文件名
    inline CVolString get_image_filename()
    {
        if (m_process != NULL)
        {
            CVolString ImageFilename;
            ImageFilename.m_mem.Alloc(MAX_PATH * 6, TRUE);
            if (::GetProcessImageFileNameW(m_process, const_cast<wchar_t*>(ImageFilename.GetText()), MAX_PATH * 3) > 0)
            {
                return ImageFilename;
            }
        }
        return CWString(L"");
    }

    // 创建远程线程
    BOOL create_remote_thread(void* func_ptr, void* parm_ptr, BOOL is_wait)
    {
        BOOL result = false;
        if (m_process != NULL)
        {
            HANDLE hThread = NULL;
            Piv_NtCreateThreadEx pfn = reinterpret_cast<Piv_NtCreateThreadEx>(
                ::GetProcAddress(::GetModuleHandleW(L"ntdll.dll"), "NtCreateThreadEx"));
            if (pfn != NULL)
            {
                pfn(&hThread, 0x1FFFFF, NULL, m_process, func_ptr, parm_ptr, 0, NULL, NULL, NULL, NULL);
            }
            else
            {
                hThread = ::CreateRemoteThread(m_process, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(func_ptr),
                                               parm_ptr, 0, NULL);
            }
            if (hThread != NULL)
            {
                result = true;
                if (is_wait)
                {
                    ::WaitForSingleObject(hThread, INFINITE);
                }
                ::CloseHandle(hThread);
            }
        }
        return result;
    }

    // 分配虚拟内存
    inline void* virtual_alloc(void* address, size_t men_size, DWORD allocation_type, DWORD protect)
    {
        return ::VirtualAllocEx(m_process, address, men_size, allocation_type, protect);
    }

    // 分配邻近内存
    void* virtual_alloc_near(void* address, size_t men_size, DWORD allocation_type, DWORD protect, int try_times = 1000)
    {
        void* ret = ::VirtualAllocEx(m_process, address, men_size, allocation_type, protect);
        if (ret == nullptr)
        {
            MEMORY_BASIC_INFORMATION mbi{0};
            if (::VirtualQueryEx(m_process, address, &mbi, sizeof(mbi)) != 0 && mbi.BaseAddress != nullptr)
            {
                size_t base_address = reinterpret_cast<size_t>(mbi.BaseAddress) / 0x10000 * 0x10000;
                for (int i = 1; i < try_times; i++)
                {
                    ret = ::VirtualAllocEx(m_process, reinterpret_cast<void*>(base_address + i * 0x10000), men_size, allocation_type, protect);
                    if (nullptr != ret)
                        break;
                    ret = ::VirtualAllocEx(m_process, reinterpret_cast<void*>(base_address - i * 0x10000), men_size, allocation_type, protect);
                    if (nullptr != ret)
                        break;
                }
            }
        }
        return ret;
    }

    // 释放虚拟内存
    inline BOOL virtual_free(void* address, size_t men_size, DWORD free_type)
    {
        return ::VirtualFreeEx(m_process, address, men_size, free_type);
    }

    // 修改内存保护
    inline int32_t modify_memory_protect(void* address, size_t mem_size, DWORD new_protect)
    {
        if (m_process != NULL)
        {
            DWORD dwOldProtect;
            if (::VirtualProtectEx(m_process, address, mem_size, new_protect, &dwOldProtect))
            {
                return static_cast<int32_t>(dwOldProtect);
            }
        }
        return -1;
    }

    // 寻找所有内存特征码
    int32_t find_signatures(const wchar_t* signatures, CMArray<INT_P>& address_array, const void* start_ptr, const void* end_ptr, DWORD protect = 0, DWORD mem_type = 0, ptrdiff_t max_count = PTRDIFF_MAX)
    {
        address_array.RemoveAll();
        if (m_process == NULL)
            return 0;
        if (m_process_start == NULL || m_process_end == NULL)
        {
            if (!get_base_module_info())
                return 0;
        }
        const BYTE* StartAddress = reinterpret_cast<const BYTE*>(MIN(start_ptr, end_ptr));
        const BYTE* EndAddress = (end_ptr == NULL) ? m_process_end : reinterpret_cast<const BYTE*>(MAX(StartAddress, end_ptr));
        // 特征码转字节数组
        CWString SignatureStr{signatures};
        SignatureStr.Replace(L" ", L"");
        if (SignatureStr.GetLength() % 2 != 0)
            return 0;
        wchar_t* SignatureCode = const_cast<wchar_t*>(SignatureStr.GetText());
        size_t SignatureLength = static_cast<size_t>(SignatureStr.GetLength()) / 2;
        std::unique_ptr<WORD[]> BytesetSequence{new WORD[SignatureLength]};
        // 将十六进制特征码转为十进制
        for (size_t i = 0, n = 0; i < static_cast<size_t>(SignatureStr.GetLength()); n++)
        {
            wchar_t num[2]{SignatureCode[i++], SignatureCode[i++]};
            if (num[0] != '?' && num[1] != '?')
            {
                WORD a[2];
                for (size_t i = 0; i < 2; i++)
                {
                    if (num[i] >= '0' && num[i] <= '9')
                    {
                        a[i] = num[i] - '0';
                    }
                    else if (num[i] >= 'a' && num[i] <= 'z')
                    {
                        a[i] = num[i] - 87;
                    }
                    else if (num[i] >= 'A' && num[i] <= 'Z')
                    {
                        a[i] = num[i] - 55;
                    }
                }
                BytesetSequence[n] = a[0] * 16 + a[1];
            }
            else
            {
                BytesetSequence[n] = 256;
            }
        }
        // 获取Next数组begin
        // 特征码的每个字节的范围在0-255(0-FF)之间,256用来表示问号,到260是为了防止越界
        short Next[260];
        for (short i = 0; i < 260; i++)
        {
            Next[i] = -1;
        }
        for (short i = 0; i < static_cast<short>(SignatureLength); i++)
        {
            Next[BytesetSequence[i]] = i;
        }
        // 获取Next数组end
        MEMORY_BASIC_INFORMATION mbi{0};
        std::unique_ptr<BYTE[]> MemoryData{new BYTE[BLOCKMAXSIZE]};
        while (::VirtualQueryEx(m_process, StartAddress, &mbi, sizeof(mbi)) != 0)
        {
            if ((protect != 0 && protect != mbi.Protect) || (mem_type != 0 && mem_type != mbi.Type))
                goto Next_Region;
            uint32_t i = 0;
            uint32_t BlockSize = static_cast<uint32_t>(mbi.RegionSize);
            while (BlockSize >= BLOCKMAXSIZE)
            {
                if (::ReadProcessMemory(m_process, StartAddress + (BLOCKMAXSIZE * i), MemoryData.get(), BLOCKMAXSIZE, NULL))
                {
                    for (size_t m = 0, j, k; m < BLOCKMAXSIZE;)
                    {
                        j = m;
                        k = 0;
                        for (; k < SignatureLength && j < BLOCKMAXSIZE && (BytesetSequence[k] == MemoryData[j] || BytesetSequence[k] == 256); k++, j++)
                            ;
                        if (k == SignatureLength)
                        {
                            if (address_array.Add2(reinterpret_cast<INT_P>(StartAddress) + (BLOCKMAXSIZE * i) + m) >= (max_count - 1))
                            {
                                return static_cast<int32_t>(max_count); // 达到所欲获取的最大数组成员数后退出
                            }
                        }
                        if ((m + SignatureLength) >= BLOCKMAXSIZE)
                        {
                            break;
                        }
                        int32_t num = Next[MemoryData[m + SignatureLength]];
                        if (num == -1)
                            m += (SignatureLength - Next[256]);
                        else
                            m += (SignatureLength - num);
                    }
                }
                BlockSize -= BLOCKMAXSIZE;
                i++;
            }
            if (::ReadProcessMemory(m_process, StartAddress + (BLOCKMAXSIZE * i), MemoryData.get(), BlockSize, NULL))
            {
                for (size_t m = 0, j, k; m < BlockSize;)
                {
                    j = m;
                    k = 0;
                    for (; k < SignatureLength && j < BlockSize && (BytesetSequence[k] == MemoryData[j] || BytesetSequence[k] == 256); k++, j++)
                        ;
                    if (k == SignatureLength)
                    {
                        if (address_array.Add2(reinterpret_cast<INT_P>(StartAddress) + (BLOCKMAXSIZE * i) + m) >= (max_count - 1))
                        {
                            return static_cast<int32_t>(max_count); // 达到所欲获取的最大数组成员数后退出
                        }
                        break;
                    }
                    if ((m + SignatureLength) >= BlockSize)
                    {
                        break;
                    }
                    int32_t num = Next[MemoryData[m + SignatureLength]];
                    if (num == -1)
                        m += (SignatureLength - Next[256]);
                    else
                        m += (SignatureLength - num);
                }
            }
        Next_Region:
            StartAddress += mbi.RegionSize;
            if (StartAddress >= EndAddress)
            {
                break;
            }
        }
        return static_cast<int32_t>(address_array.GetCount());
    }

    // 寻找内存特征码
    inline ptrdiff_t find_signatures(const wchar_t* signatures, const void* start_ptr, const void* end_ptr, DWORD protect = 0, DWORD mem_type = 0)
    {
        if (m_process != NULL)
        {
            CMArray<INT_P> address_array;
            if (find_signatures(signatures, address_array, start_ptr, end_ptr, protect, mem_type, 1) > 0)
            {
                return address_array.GetAt(0);
            }
        }
        return 0;
    }

    // 寻找所有内存字节集
    int32_t find_memory(const CVolMem& mem_data, CMArray<INT_P>& address_array, const void* start_ptr, const void* end_ptr, DWORD protect = 0, DWORD mem_type = 0, ptrdiff_t max_count = PTRDIFF_MAX)
    {
        address_array.RemoveAll();
        if (m_process == NULL)
        {
            return 0;
        }
        if (m_process_start == NULL || m_process_end == NULL)
        {
            if (!get_base_module_info())
                return 0;
        }
        const BYTE* StartAddress = reinterpret_cast<const BYTE*>(MIN(start_ptr, m_process_end));
        const BYTE* EndAddress = (end_ptr == NULL) ? m_process_end : reinterpret_cast<const BYTE*>(MAX(StartAddress, end_ptr));
        MEMORY_BASIC_INFORMATION mbi{0};
        CVolMem MemoryData;
        MemoryData.Alloc(BLOCKMAXSIZE, TRUE);
        while (::VirtualQueryEx(m_process, StartAddress, &mbi, sizeof(mbi)) != 0)
        {
            if ((protect != 0 && protect != mbi.Protect) || (mem_type != 0 && mem_type != mbi.Type))
                goto Next_Region;
            uint32_t i = 0;
            uint32_t BlockSize = static_cast<uint32_t>(mbi.RegionSize);
            while (BlockSize >= BLOCKMAXSIZE)
            {
                if (::ReadProcessMemory(m_process, StartAddress + (BLOCKMAXSIZE * i), MemoryData.GetPtr(), BLOCKMAXSIZE, NULL))
                {
                    INT_P ret = MemoryData.FindBin(mem_data, 0);
                    if (ret != -1)
                    {
                        if (address_array.Add2(reinterpret_cast<INT_P>(StartAddress) + (BLOCKMAXSIZE * i) + ret) >= (max_count - 1))
                        {
                            return static_cast<int32_t>(max_count); // 达到所欲获取的最大数组成员数后退出
                        }
                    }
                }
                BlockSize -= BLOCKMAXSIZE;
                i++;
            }
            MemoryData.Zero();
            if (::ReadProcessMemory(m_process, StartAddress + (BLOCKMAXSIZE * i), MemoryData.GetPtr(), BlockSize, NULL))
            {
                INT_P ret = MemoryData.FindBin(mem_data, 0);
                if (ret != -1)
                {
                    if (address_array.Add2(reinterpret_cast<INT_P>(StartAddress) + (BLOCKMAXSIZE * i) + ret) >= (max_count - 1)) // 达到所欲获取的最大数组成员数后退出
                    {
                        return static_cast<int32_t>(max_count);
                    }
                }
            }
        Next_Region:
            StartAddress += mbi.RegionSize;
            if (StartAddress >= EndAddress)
            {
                break;
            }
        }
        return static_cast<int32_t>(address_array.GetCount());
    }

    // 寻找内存字节集
    inline ptrdiff_t find_memory(const CVolMem& mem_data, const void* start_ptr, const void* end_ptr, DWORD protect = 0, DWORD mem_type = 0)
    {
        if (m_process != NULL)
        {
            CMArray<INT_P> address_array;
            if (find_memory(mem_data, address_array, start_ptr, end_ptr, protect, mem_type, 1) > 0)
            {
                return address_array.GetAt(0);
            }
        }
        return 0;
    }

    // 读内存数据
    inline BOOL read_memory(void* write_address, const void* read_address, size_t read_size)
    {
        if (m_process != NULL)
        {
            return ::ReadProcessMemory(m_process, read_address, write_address, read_size, NULL);
        }
        return FALSE;
    }

    // 读内存字节集
    inline CVolMem read_memory(const void* read_address, size_t read_size)
    {
        if (m_process != NULL)
        {
            CVolMem MemoryData;
            MemoryData.Alloc(read_size, TRUE);
            if (::ReadProcessMemory(m_process, read_address, MemoryData.GetPtr(),
                                    read_size, NULL))
            {
                return MemoryData;
            }
        }
        return CVolMem();
    }

    // 读内存多字节
    inline CVolMem read_memory_char(const void* read_address)
    {
        CVolMem MemoryData;
        if (m_process != NULL)
        {
            MEMORY_BASIC_INFORMATION mbi{0};
            std::unique_ptr<char[]> buffer{new char[1024]};
            if (::VirtualQueryEx(m_process, read_address, &mbi, sizeof(mbi)) != 0)
            {
                size_t i = 0;
                size_t RegionSize = mbi.RegionSize;
                while (RegionSize >= 1024)
                {
                    if (::ReadProcessMemory(m_process, reinterpret_cast<const BYTE*>(read_address) + (1024 * i), buffer.get(), 1024, NULL))
                    {
                        for (size_t n = 0; n < 1024; n++)
                        {
                            if (buffer[n] == '\0')
                            {
                                MemoryData.Append(buffer.get(), n);
                                return MemoryData;
                            }
                        }
                    }
                    MemoryData.Append(buffer.get(), 1024);
                    RegionSize -= 1024;
                    i++;
                }
            }
        }
        return MemoryData;
    }

    // 读内存文本
    inline CVolString read_memory_wchar(const void* read_address)
    {
        CVolString MemoryData;
        if (m_process != NULL)
        {
            MEMORY_BASIC_INFORMATION mbi{0};
            std::unique_ptr<wchar_t[]> buffer{new wchar_t[512]};
            if (::VirtualQueryEx(m_process, read_address, &mbi, sizeof(mbi)) != 0)
            {
                size_t i = 0;
                size_t RegionSize = mbi.RegionSize;
                while (RegionSize >= 1024)
                {
                    if (::ReadProcessMemory(m_process, reinterpret_cast<const BYTE*>(read_address) + (1024 * i), buffer.get(), 1024, NULL))
                    {
                        for (size_t n = 0; n < 512; n++)
                        {
                            if (buffer[n] == '\0')
                            {
                                MemoryData.AddText(buffer.get(), n);
                                return MemoryData;
                            }
                        }
                    }
                    MemoryData.AddText(buffer.get(), 512);
                    RegionSize -= 1024;
                    i++;
                }
            }
        }
        return MemoryData;
    }

    // 读内存数值
    template <typename T>
    inline BOOL read_memory_num(const void* read_address, T& value)
    {
        return (m_process != NULL && ::ReadProcessMemory(m_process, read_address, &value, sizeof(T), NULL));
    }

    // 读内存值
    template <typename R>
    inline R read_memory_value(const void* read_address)
    {
        R value;
        if (m_process != NULL && ::ReadProcessMemory(m_process, read_address, &value, sizeof(R), NULL))
        {
            return value;
        }
        return value;
    }

    // 写内存数据
    BOOL write_memory(void* write_address, const void* data_address, size_t data_size, BOOL change_protect = TRUE)
    {
        if (m_process == NULL)
            return FALSE;
        DWORD dwNewProtect = PAGE_READWRITE;
        if (change_protect)
        {
            MEMORY_BASIC_INFORMATION info{0};
            ::VirtualQueryEx(m_process, write_address, &info, sizeof(MEMORY_BASIC_INFORMATION));
            if ((info.Protect & PAGE_READWRITE) == PAGE_READWRITE || (info.Protect & PAGE_EXECUTE_READWRITE) == PAGE_EXECUTE_READWRITE ||
                (info.Protect & PAGE_EXECUTE_WRITECOPY) == PAGE_EXECUTE_WRITECOPY || (info.Protect & PAGE_WRITECOPY) == PAGE_WRITECOPY)
            {
                change_protect = FALSE;
            }
            else if ((info.Protect & PAGE_EXECUTE) == PAGE_EXECUTE || (info.Protect & PAGE_EXECUTE_READ) == PAGE_EXECUTE_READ)
            {
                dwNewProtect = PAGE_EXECUTE_READWRITE;
            }
        }
        if (!change_protect)
            return ::WriteProcessMemory(m_process, write_address, data_address, data_size, NULL);
        BOOL result = FALSE;
        DWORD dwOldProtect = 0;
        if (::VirtualProtectEx(m_process, write_address, data_size, dwNewProtect, &dwOldProtect))
        {
            result = ::WriteProcessMemory(m_process, write_address, data_address, data_size, NULL);
            ::VirtualProtectEx(m_process, write_address, data_size, dwOldProtect, &dwOldProtect);
        }
        return result;
    }

    // 写内存字节集
    inline BOOL write_memory(void* write_address, const CVolMem& data, BOOL change_protect = TRUE)
    {
        return write_memory(write_address, data.GetPtr(), static_cast<size_t>(data.GetSize()), change_protect);
    }

    // 写内存文本
    inline BOOL write_memory(void* write_address, const wchar_t* text, BOOL change_protect = TRUE)
    {
        return write_memory(write_address, text, (wcslen(text) + 1) * 2, change_protect);
    }

    inline BOOL write_memory(void* write_address, const char* text, BOOL change_protect = TRUE)
    {
        return write_memory(write_address, text, strlen(text) + 1, change_protect);
    }

    // 写内存值
    template <typename T>
    inline BOOL write_memory_value(void* write_address, const T& value, BOOL change_protect = TRUE)
    {
        return write_memory(write_address, &value, sizeof(T), change_protect);
    }

    // 寻找模块特征码
    inline ptrdiff_t find_module_signatures(HMODULE hModule, const wchar_t* signatures, size_t start_off, size_t end_off, DWORD protect = 0, DWORD mem_type = 0)
    {
        ptrdiff_t result = 0;
        if (m_process != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(m_process, hModule, &ModuleInfo, sizeof(MODULEINFO)) && start_off <= ModuleInfo.SizeOfImage && end_off <= ModuleInfo.SizeOfImage)
            {
                const BYTE* module_base = static_cast<const BYTE*>(ModuleInfo.lpBaseOfDll);
                result = find_signatures(signatures, module_base + start_off,
                                         module_base + (end_off ? end_off : ModuleInfo.SizeOfImage), protect, mem_type);
            }
        }
        return result;
    }

    // 寻找所有模块特征码
    inline int32_t find_module_signatures(HMODULE hModule, const wchar_t* signatures, CMArray<INT_P>& address_array, size_t start_off, size_t end_off, DWORD protect = 0, DWORD mem_type = 0)
    {
        int32_t result = 0;
        if (m_process != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(m_process, hModule, &ModuleInfo, sizeof(MODULEINFO)) && start_off <= ModuleInfo.SizeOfImage && end_off <= ModuleInfo.SizeOfImage)
            {
                const BYTE* module_base = static_cast<const BYTE*>(ModuleInfo.lpBaseOfDll);
                result = find_signatures(signatures, address_array, module_base + start_off, module_base + (end_off ? end_off : ModuleInfo.SizeOfImage), protect, mem_type);
            }
        }
        return result;
    }

    // 寻找模块字节集
    inline ptrdiff_t find_module_memory(HMODULE hModule, const CVolMem& mem_data, size_t start_off, size_t end_off, DWORD protect = 0, DWORD mem_type = 0)
    {
        ptrdiff_t result = 0;
        if (m_process != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(m_process, hModule, &ModuleInfo, sizeof(MODULEINFO)) && start_off < ModuleInfo.SizeOfImage && end_off < ModuleInfo.SizeOfImage)
            {
                const BYTE* module_base = static_cast<const BYTE*>(ModuleInfo.lpBaseOfDll);
                result = find_memory(mem_data, module_base + start_off, module_base + (end_off ? end_off : ModuleInfo.SizeOfImage), protect, mem_type);
            }
        }
        return result;
    }

    // 寻找所有模块字节集
    inline int32_t find_module_memory(HMODULE hModule, const CVolMem& mem_data, CMArray<INT_P>& address_array, size_t start_off, size_t end_off, DWORD protect = 0, DWORD mem_type = 0)
    {
        int32_t result = 0;
        if (m_process != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(m_process, hModule, &ModuleInfo, sizeof(MODULEINFO)) && start_off < ModuleInfo.SizeOfImage && end_off < ModuleInfo.SizeOfImage)
            {
                const BYTE* module_base = static_cast<const BYTE*>(ModuleInfo.lpBaseOfDll);
                result = find_memory(mem_data, address_array, module_base + start_off, module_base + (end_off ? end_off : ModuleInfo.SizeOfImage), protect, mem_type);
            }
        }
        return result;
    }

    // 读模块数据
    BOOL read_module_memory(HMODULE hModule, void* write_address, size_t read_off, size_t read_size)
    {
        if (m_process != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(m_process, hModule, &ModuleInfo, sizeof(MODULEINFO)) && read_off < ModuleInfo.SizeOfImage)
            {
                return ::ReadProcessMemory(m_process, reinterpret_cast<const BYTE*>(ModuleInfo.lpBaseOfDll) + read_off,
                                           write_address, read_size, NULL);
            }
        }
        return false;
    }

    // 读模块字节集
    inline CVolMem read_module_memory(HMODULE hModule, size_t read_off, size_t read_size)
    {
        if (m_process != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(m_process, hModule, &ModuleInfo, sizeof(MODULEINFO)) && read_off < ModuleInfo.SizeOfImage)
            {
                CVolMem MemoryData;
                MemoryData.Alloc(read_size, TRUE);
                if (::ReadProcessMemory(m_process, reinterpret_cast<const BYTE*>(ModuleInfo.lpBaseOfDll) + read_off,
                                        MemoryData.GetPtr(), read_size, NULL))
                {
                    return MemoryData;
                }
            }
        }
        return CVolMem();
    }

    // 读模块多字节
    inline CVolMem read_module_char(HMODULE hModule, size_t read_off)
    {
        if (m_process != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(m_process, hModule, &ModuleInfo, sizeof(MODULEINFO)) && read_off < ModuleInfo.SizeOfImage)
            {
                return read_memory_char(reinterpret_cast<const BYTE*>(ModuleInfo.lpBaseOfDll) + read_off);
            }
        }
        return CVolMem();
    }

    // 读模块文本
    inline CVolString read_module_wchar(HMODULE hModule, size_t read_off)
    {
        if (m_process != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(m_process, hModule, &ModuleInfo, sizeof(MODULEINFO)) && read_off < ModuleInfo.SizeOfImage)
            {
                return read_memory_wchar(reinterpret_cast<const BYTE*>(ModuleInfo.lpBaseOfDll) + read_off);
            }
        }
        return CVolString();
    }

    // 读模块内存值
    template <typename T>
    inline BOOL read_module_num(HMODULE hModule, T& value, size_t read_off)
    {
        if (m_process != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(m_process, hModule, &ModuleInfo, sizeof(MODULEINFO)) && read_off < ModuleInfo.SizeOfImage)
            {
                if (::ReadProcessMemory(m_process, reinterpret_cast<const BYTE*>(ModuleInfo.lpBaseOfDll) + read_off,
                                        &value, sizeof(T), NULL))
                {
                    return TRUE;
                }
            }
        }
        return FALSE;
    }

    // 读模块内存值
    template <typename R>
    inline R read_module_value(HMODULE hModule, size_t read_off)
    {
        R value;
        if (m_process != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(m_process, hModule, &ModuleInfo, sizeof(MODULEINFO)) && read_off < ModuleInfo.SizeOfImage)
            {
                if (::ReadProcessMemory(m_process, reinterpret_cast<const BYTE*>(ModuleInfo.lpBaseOfDll) + read_off,
                                        &value, sizeof(R), NULL))
                {
                    return value;
                }
            }
        }
        return value;
    }

    // 写模块数据
    inline BOOL write_module_memory(HMODULE hModule, size_t write_off, const void* data_address, size_t data_size, BOOL change_protect = TRUE)
    {
        if (m_process == NULL)
            return FALSE;
        MODULEINFO ModuleInfo{0};
        BOOL result = FALSE;
        if (::GetModuleInformation(m_process, hModule, &ModuleInfo, sizeof(MODULEINFO)) && write_off < ModuleInfo.SizeOfImage)
        {
            BYTE* write_address = reinterpret_cast<BYTE*>(ModuleInfo.lpBaseOfDll) + write_off;
            return write_memory(write_address, data_address, data_size, change_protect);
        }
        return result;
    }

    // 写模块字节集
    inline BOOL write_module_memory(HMODULE hModule, size_t write_off, const CVolMem& data, BOOL change_protect = TRUE)
    {
        return write_module_memory(hModule, write_off, data.GetPtr(), static_cast<size_t>(data.GetSize()), change_protect);
    }

    // 写模块文本
    inline BOOL write_module_memory(HMODULE hModule, size_t write_off, const wchar_t* text, BOOL change_protect = TRUE)
    {
        return write_module_memory(hModule, write_off, text, (wcslen(text) + 1) * 2, change_protect);
    }

    inline BOOL write_module_memory(HMODULE hModule, size_t write_off, const char* text, BOOL change_protect = TRUE)
    {
        return write_module_memory(hModule, write_off, text, strlen(text) + 1, change_protect);
    }

    // 写模块值
    template <typename T>
    inline BOOL write_module_value(HMODULE hModule, size_t write_off, const T& value, BOOL change_protect = TRUE)
    {
        return write_module_memory(hModule, write_off, &value, sizeof(T), change_protect);
    }
}; // PivProcess

#endif // _PIV_PROCESS_HPP
