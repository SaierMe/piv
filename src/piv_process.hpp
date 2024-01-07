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
#include <string>
#include <psapi.h>
#include <winternl.h>
#pragma comment(lib, "Psapi.lib")

class PivProcess
{
private:
    HANDLE hProcess = NULL;
    const BYTE *ProcessStart = NULL;
    const BYTE *ProcessEnd = NULL;
    DWORD PID;

    const uint32_t BLOCKMAXSIZE{409600};
    typedef BOOL(WINAPI *Piv_GetProcessDEPPolicy)(HANDLE, LPDWORD, PBOOL);
    typedef BOOL(WINAPI *Piv_IsWow64Process)(HANDLE, PBOOL);
    typedef NTSTATUS(NTAPI *Piv_ZwQueryInformationProcess)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);
    typedef NTSTATUS(NTAPI *Piv_NtCreateThreadEx)(OUT PHANDLE hThread, IN ACCESS_MASK DesiredAccess, IN PVOID ObjectAttributes,
                                                  IN HANDLE ProcessHandle, IN PVOID lpStartAddress, IN PVOID lpParameter, IN ULONG Flags,
                                                  IN SIZE_T StackZeroBits, IN SIZE_T SizeOfStackCommit, IN SIZE_T SizeOfStackReserve, OUT PVOID lpBytesBuffer);

    // 取首模块信息
    BOOL get_base_module_info()
    {
        if (hProcess == NULL)
            return FALSE;
        CMArray<HMODULE> hModuleArray;
        DWORD dwNeeded = 0;
        ::EnumProcessModulesEx(hProcess, NULL, 0, &dwNeeded, 3);
        hModuleArray.InitCount(dwNeeded / sizeof(HMODULE), TRUE);
        ::EnumProcessModulesEx(hProcess, hModuleArray.GetData(), dwNeeded, &dwNeeded, 3);
        if (hModuleArray.GetCount() <= 0)
            return FALSE;
        MODULEINFO sModuleInfo{0};
        if (!::GetModuleInformation(hProcess, hModuleArray.GetAt(0), &sModuleInfo, sizeof(sModuleInfo)))
            return FALSE;
        ProcessStart = static_cast<const BYTE *>(sModuleInfo.lpBaseOfDll);
        if (!::GetModuleInformation(hProcess, hModuleArray.GetLastElement(), &sModuleInfo, sizeof(sModuleInfo)))
            return FALSE;
        ProcessEnd = static_cast<const BYTE *>(sModuleInfo.lpBaseOfDll) + sModuleInfo.SizeOfImage;
        return TRUE;
    }

public:
    PivProcess() {}
    PivProcess(const DWORD &process_id, const DWORD &desired_access)
    {
        open_process(process_id, desired_access);
    }
    ~PivProcess()
    {
        close_process();
    }

    // 打开进程
    BOOL open_process(const DWORD &process_id, const DWORD &desired_access)
    {
        close_process();
        PID = process_id ? process_id : ::GetCurrentProcessId();
        hProcess = ::OpenProcess(desired_access, FALSE, PID);
        if (hProcess == NULL)
            return FALSE;
        get_base_module_info();
        return TRUE;
    }

    // 关闭进程
    void close_process()
    {
        if (hProcess != NULL)
        {
            ::CloseHandle(hProcess);
            hProcess = NULL;
            ProcessStart = NULL;
            ProcessEnd = NULL;
            PID = 0;
        }
    }

    // 取进程句柄
    HANDLE get_process_handle() const noexcept
    {
        return hProcess;
    }

    // 取进程ID
    DWORD get_process_id() const noexcept
    {
        return PID;
    }

    // 取句柄数量
    DWORD get_handle_count() const noexcept
    {
        DWORD dwHandleCount = 0;
        if (hProcess != NULL)
            ::GetProcessHandleCount(hProcess, &dwHandleCount);
        return dwHandleCount;
    }

    // 取创建时间
    double get_creation_time(const int32_t &time_zoon) const
    {
        double dtime = 0.0;
        if (hProcess != NULL)
        {
            FILETIME CreationTime, ExitTime, KernelTime, UserTime;
            if (::GetProcessTimes(hProcess, &CreationTime, &ExitTime, &KernelTime, &UserTime))
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
    BOOL get_memory_info(PROCESS_MEMORY_COUNTERS *pMemCounters)
    {
        if (hProcess != NULL)
            return ::GetProcessMemoryInfo(hProcess, pMemCounters, sizeof(PROCESS_MEMORY_COUNTERS));
        return FALSE;
    }

    // 取优先级
    DWORD get_priority_class()
    {
        return hProcess ? ::GetPriorityClass(hProcess) : 0;
    }

    // 置优先级
    BOOL set_priority_class(const DWORD &priority)
    {
        return ::SetPriorityClass(hProcess, priority);
    }

    // 取进程DEP策略
    BOOL get_dep_policy(DWORD &flags, BOOL &is_permanet)
    {
        flags = 0;
        is_permanet = FALSE;
        if (hProcess != NULL)
        {
            Piv_GetProcessDEPPolicy pfn = reinterpret_cast<Piv_GetProcessDEPPolicy>(
                ::GetProcAddress(::GetModuleHandleW(L"Kernel32.dll"), "GetProcessDEPPolicy"));
            if (pfn)
            {
                return pfn(hProcess, &flags, &is_permanet);
            }
        }
        return FALSE;
    }

    // 是否为WOW64模式
    BOOL is_wow64(BOOL currentProcess = TRUE)
    {
        BOOL result = FALSE;
        if (hProcess != NULL)
        {
            Piv_IsWow64Process pfn = reinterpret_cast<Piv_IsWow64Process>(
                ::GetProcAddress(::GetModuleHandle(L"Kernel32.dll"), "IsWow64Process"));
            if (pfn)
            {
                pfn(currentProcess ? ::GetCurrentProcess() : hProcess, &result);
            }
        }
        return result;
    }

    // 取命令行
    CVolString get_commandline()
    {
        CVolString result;
        if (hProcess != NULL)
        {
            Piv_ZwQueryInformationProcess pfn = reinterpret_cast<Piv_ZwQueryInformationProcess>(
                ::GetProcAddress(::GetModuleHandleW(L"ntdll.dll"), "ZwQueryInformationProcess"));
            if (pfn)
            {
                PROCESS_BASIC_INFORMATION BasicInfo{0};
                PEB PebBaseInfo{0};
                RTL_USER_PROCESS_PARAMETERS ProcessParameters{0};
                if (pfn(hProcess, PROCESSINFOCLASS::ProcessBasicInformation, &BasicInfo, sizeof(BasicInfo), NULL) == 0)
                {
                    if (::ReadProcessMemory(hProcess, BasicInfo.PebBaseAddress, &PebBaseInfo, sizeof(PebBaseInfo), NULL) &&
                        ::ReadProcessMemory(hProcess, PebBaseInfo.ProcessParameters, &ProcessParameters, sizeof(ProcessParameters), NULL))
                    {
                        DWORD dwSize = ProcessParameters.CommandLine.Length + 2;
                        WCHAR *lpBuffer = (WCHAR *)calloc(dwSize, sizeof(WCHAR));
                        if (::ReadProcessMemory(hProcess, ProcessParameters.CommandLine.Buffer, lpBuffer,
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
    HMODULE get_module_handle(const wchar_t *module_name)
    {
        if (hProcess != NULL)
        {
            CMArray<HMODULE> hModuleArray;
            DWORD dwNeeded = 0;
            ::EnumProcessModulesEx(hProcess, NULL, 0, &dwNeeded, 3);
            hModuleArray.InitCount(dwNeeded / sizeof(HMODULE), TRUE);
            ::EnumProcessModulesEx(hProcess, hModuleArray.GetData(), dwNeeded, &dwNeeded, 3);
            std::wstring module_name_sv{module_name};
            if (module_name_sv.empty())
            {
                module_name_sv.resize(MAX_PATH);
                ::GetModuleBaseNameW(hProcess, 0, const_cast<wchar_t *>(module_name_sv.data()), MAX_PATH);
            }
            std::wstring sFileName;
            sFileName.resize(MAX_PATH);
            for (INT_P i = 0; i < hModuleArray.GetCount(); i++)
            {
                ::GetModuleFileNameExW(hProcess, hModuleArray.GetAt(i), const_cast<wchar_t *>(sFileName.data()), MAX_PATH);
                if (sFileName.rfind(module_name_sv, std::wstring::npos) != std::wstring::npos)
                    return hModuleArray.GetAt(i);
            }
        }
        return 0;
    }

    // 取模块基址
    void *get_module_base(const HMODULE &hMoudle)
    {
        if (hProcess != NULL)
        {
            MODULEINFO sModuleInfo{0};
            if (::GetModuleInformation(hProcess, hMoudle, &sModuleInfo, sizeof(sModuleInfo)))
            {
                return sModuleInfo.lpBaseOfDll;
            }
        }
        return NULL;
    }

    // 枚举模块句柄
    int32_t enum_modules(CMArray<INT_P> &module_array, const DWORD &filter_flag)
    {
        module_array.RemoveAll();
        if (hProcess != NULL)
        {
            DWORD dwNeeded = 0;
            ::EnumProcessModulesEx(hProcess, NULL, 0, &dwNeeded, filter_flag);
            HMODULE *lphModule = (HMODULE *)malloc(dwNeeded);
            if (lphModule == NULL)
                return 0;
            ::EnumProcessModulesEx(hProcess, lphModule, dwNeeded, &dwNeeded, filter_flag);
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
    int32_t enum_modules(CMArray<INT_P> &module_array, CMStringArray &name_array, const BOOL &is_fullpath, const DWORD &filter_flag)
    {
        module_array.RemoveAll();
        name_array.RemoveAll();
        if (hProcess != NULL)
        {
            DWORD dwNeeded = 0;
            ::EnumProcessModulesEx(hProcess, NULL, 0, &dwNeeded, filter_flag);
            HMODULE *lphModule = (HMODULE *)malloc(dwNeeded);
            if (lphModule == NULL)
                return 0;
            ::EnumProcessModulesEx(hProcess, lphModule, dwNeeded, &dwNeeded, filter_flag);
            size_t nModuleCount = dwNeeded / sizeof(HMODULE);
            WCHAR szFileName[MAX_PATH + 1]{'\0'};
            for (size_t i = 0; i < nModuleCount; i++)
            {
                if (is_fullpath)
                    ::GetModuleFileNameExW(hProcess, lphModule[i], szFileName, MAX_PATH);
                else
                    ::GetModuleBaseNameW(hProcess, lphModule[i], szFileName, MAX_PATH);
                name_array.Add(szFileName);
                module_array.Add(reinterpret_cast<INT_P>(lphModule[i]));
            }
            free(lphModule);
        }
        return static_cast<int32_t>(module_array.GetCount());
    }

    // 取模块信息
    BOOL get_module_info(const HMODULE &hModule, MODULEINFO *pModuleInfo)
    {
        if (hProcess != NULL)
        {
            return ::GetModuleInformation(hProcess, hModule, pModuleInfo, sizeof(MODULEINFO));
        }
        return false;
    }

    // 取模块名称
    CVolString get_module_name(const HMODULE &hModule)
    {
        if (hProcess != NULL)
        {
            CVolString BaseName;
            BaseName.m_mem.Alloc((MAX_PATH + 1) * 2, TRUE);
            if (::GetModuleBaseNameW(hProcess, hModule, const_cast<wchar_t *>(BaseName.GetText()), MAX_PATH) > 0)
            {
                return BaseName;
            }
        }
        return _CT("");
    }

    // 取模块文件名
    CVolString get_module_filename(const HMODULE &hModule)
    {
        if (hProcess != NULL)
        {
            CVolString FileName;
            FileName.m_mem.Alloc(MAX_PATH * 6, TRUE);
            if (::GetModuleFileNameExW(hProcess, hModule, const_cast<wchar_t *>(FileName.GetText()), MAX_PATH * 3) > 0)
            {
                return FileName;
            }
        }
        return _CT("");
    }

    // 取模块路径
    CVolString get_module_path(const HMODULE &hModule)
    {
        if (hProcess != NULL)
        {
            CVolString BaseName, FileName;
            BaseName.m_mem.Alloc(MAX_PATH * 6, TRUE);
            FileName.m_mem.Alloc(MAX_PATH * 6, TRUE);
            if (::GetModuleFileNameExW(hProcess, hModule, const_cast<wchar_t *>(FileName.GetText()), MAX_PATH * 3) > 0 &&
                ::GetModuleBaseNameW(hProcess, hModule, const_cast<wchar_t *>(BaseName.GetText()), MAX_PATH * 3) > 0)
            {
                INT_P pos = FileName.SearchText(BaseName.GetText(), -1, FALSE, TRUE);
                if (pos != -1)
                    return FileName.Left(pos);
            }
        }
        return _CT("");
    }

    // 取内存映射文件
    CVolString get_mapped_filename(void *mem_ptr)
    {
        if (hProcess != NULL)
        {
            CVolString FileName;
            FileName.m_mem.Alloc(MAX_PATH * 6, TRUE);
            if (::GetMappedFileNameW(hProcess, mem_ptr, const_cast<wchar_t *>(FileName.GetText()), MAX_PATH * 3) > 0)
            {
                return FileName;
            }
        }
        return _CT("");
    }

    // 取进程映像文件名
    CVolString get_image_filename()
    {
        if (hProcess != NULL)
        {
            CVolString ImageFilename;
            ImageFilename.m_mem.Alloc(MAX_PATH * 6, TRUE);
            if (::GetProcessImageFileNameW(hProcess, const_cast<wchar_t *>(ImageFilename.GetText()), MAX_PATH * 3) > 0)
            {
                return ImageFilename;
            }
        }
        return _CT("");
    }

    // 创建远程线程
    BOOL create_remote_thread(void *func_ptr, void *parm_ptr, const BOOL &is_wait)
    {
        BOOL result = false;
        if (hProcess != NULL)
        {
            HANDLE hThread = NULL;
            Piv_NtCreateThreadEx pfn = reinterpret_cast<Piv_NtCreateThreadEx>(
                ::GetProcAddress(::GetModuleHandleW(L"ntdll.dll"), "NtCreateThreadEx"));
            if (pfn != NULL)
            {
                pfn(&hThread, 0x1FFFFF, NULL, hProcess, func_ptr, parm_ptr, 0, NULL, NULL, NULL, NULL);
            }
            else
            {
                hThread = ::CreateRemoteThread(hProcess, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(func_ptr),
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
    void *virtual_alloc(void *address, const size_t &men_size, const DWORD &allocation_type, const DWORD &protect)
    {
        return ::VirtualAllocEx(hProcess, address, men_size, allocation_type, protect);
    }

    // 释放虚拟内存
    BOOL virtual_free(void *address, const size_t &men_size, const DWORD &free_type)
    {
        return ::VirtualFreeEx(hProcess, address, men_size, free_type);
    }
    // 修改内存保护
    int32_t modify_memory_protect(void *address, const size_t &mem_size, const DWORD &new_protect)
    {
        if (hProcess != NULL)
        {
            DWORD dwOldProtect;
            if (::VirtualProtectEx(hProcess, address, mem_size, new_protect, &dwOldProtect))
            {
                return static_cast<int32_t>(dwOldProtect);
            }
        }
        return -1;
    }

    // 寻找所有内存特征码
    int32_t find_signatures(const wchar_t *signatures, CMArray<INT_P> &address_array, const BYTE *start_ptr, const BYTE *end_ptr, const ptrdiff_t &max_count = PTRDIFF_MAX)
    {
        address_array.RemoveAll();
        if (hProcess == NULL)
            return 0;
        if (ProcessStart == NULL || ProcessEnd == NULL)
        {
            if (!get_base_module_info())
                return 0;
        }
        const BYTE *StartAddress = MIN(MAX(ProcessStart, start_ptr), ProcessEnd);
        const BYTE *EndAddress = (end_ptr == NULL) ? ProcessEnd : MAX(StartAddress, end_ptr);
        const BYTE *RetAddress = 0;
        // 特征码转字节数组
        CVolMem sign_buf;
        GetMbsText(signatures, sign_buf, NULL);
        char *Tzm = reinterpret_cast<char *>(sign_buf.GetPtr());
        size_t TzmLength = strlen(Tzm) / 3 + 1;
        WORD *TzmArray = new WORD[TzmLength];
        // 将十六进制特征码转为十进制
        for (size_t i = 0, n = 0; i < strlen(Tzm); i++, n++)
        {
            char num[2]{Tzm[i++], Tzm[i++]};
            if (num[0] != '?' && num[1] != '?')
            {
                char a[2];
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
                TzmArray[n] = a[0] * 16 + a[1];
            }
            else
            {
                TzmArray[n] = 256;
            }
        }
        // 获取Next数组begin
        // 特征码的每个字节的范围在0-255(0-FF)之间,256用来表示问号,到260是为了防止越界
        short Next[260];
        for (short i = 0; i < 260; i++)
        {
            Next[i] = -1;
        }
        for (short i = 0; i < static_cast<short>(TzmLength); i++)
        {
            Next[TzmArray[i]] = i;
        }
        // 获取Next数组end
        MEMORY_BASIC_INFORMATION mbi{0};
        BYTE *MemoryData = new BYTE[BLOCKMAXSIZE];
        while (::VirtualQueryEx(hProcess, StartAddress, &mbi, sizeof(mbi)) != 0)
        {
            uint32_t i = 0;
            uint32_t BlockSize = static_cast<uint32_t>(mbi.RegionSize);
            while (BlockSize >= BLOCKMAXSIZE)
            {
                if (::ReadProcessMemory(hProcess, StartAddress + (BLOCKMAXSIZE * i), MemoryData, BLOCKMAXSIZE, NULL))
                {
                    for (size_t m = 0, j, k; m < BLOCKMAXSIZE;)
                    {
                        j = m;
                        k = 0;
                        for (; k < TzmLength && j < BLOCKMAXSIZE && (TzmArray[k] == MemoryData[j] || TzmArray[k] == 256); k++, j++)
                            ;
                        if (k == TzmLength)
                        {
                            if (address_array.Add2(reinterpret_cast<INT_P>(StartAddress) + (BLOCKMAXSIZE * i) + m) >= (max_count - 1))
                            {
                                return static_cast<int32_t>(max_count); // 达到所欲获取的最大数组成员数后退出
                            }
                        }
                        if ((m + TzmLength) >= BLOCKMAXSIZE)
                        {
                            break;
                        }
                        int32_t num = Next[MemoryData[m + TzmLength]];
                        if (num == -1)
                            m += (TzmLength - Next[256]);
                        else
                            m += (TzmLength - num);
                    }
                }
                BlockSize -= BLOCKMAXSIZE;
                i++;
            }
            if (::ReadProcessMemory(hProcess, StartAddress + (BLOCKMAXSIZE * i), MemoryData, BlockSize, NULL))
            {
                for (size_t m = 0, j, k; m < BlockSize;)
                {
                    j = m;
                    k = 0;
                    for (; k < TzmLength && j < BlockSize && (TzmArray[k] == MemoryData[j] || TzmArray[k] == 256); k++, j++)
                        ;
                    if (k == TzmLength)
                    {
                        if (address_array.Add2(reinterpret_cast<INT_P>(StartAddress) + (BLOCKMAXSIZE * i) + m) >= (max_count - 1))
                        {
                            return static_cast<int32_t>(max_count); // 达到所欲获取的最大数组成员数后退出
                        }
                        break;
                    }
                    if ((m + TzmLength) >= BlockSize)
                    {
                        break;
                    }
                    int32_t num = Next[MemoryData[m + TzmLength]];
                    if (num == -1)
                        m += (TzmLength - Next[256]);
                    else
                        m += (TzmLength - num);
                }
            }
            StartAddress += mbi.RegionSize;
            if (StartAddress >= EndAddress)
            {
                break;
            }
        }
        delete[] TzmArray;
        delete[] MemoryData;
        return static_cast<int32_t>(address_array.GetCount());
    }

    // 寻找内存特征码
    ptrdiff_t find_signatures(const wchar_t *signatures, const BYTE *start_ptr, const BYTE *end_ptr)
    {
        if (hProcess != NULL)
        {
            CMArray<INT_P> address_array;
            if (find_signatures(signatures, address_array, start_ptr, end_ptr, 1) > 0)
            {
                return address_array.GetAt(0);
            }
        }
        return 0;
    }

    // 寻找所有内存字节集
    int32_t find_memory(const CVolMem &mem_data, CMArray<INT_P> &address_array, const BYTE *start_ptr, const BYTE *end_ptr, const ptrdiff_t &max_count = PTRDIFF_MAX)
    {
        address_array.RemoveAll();
        if (hProcess == NULL)
        {
            return 0;
        }
        if (ProcessStart == NULL || ProcessEnd == NULL)
        {
            if (!get_base_module_info())
                return 0;
        }
        const BYTE *StartAddress = MIN(MAX(ProcessStart, start_ptr), ProcessEnd);
        const BYTE *EndAddress = (end_ptr == NULL) ? ProcessEnd : MAX(StartAddress, end_ptr);
        MEMORY_BASIC_INFORMATION mbi{0};
        CVolMem MemoryData;
        MemoryData.Alloc(BLOCKMAXSIZE, TRUE);
        while (::VirtualQueryEx(hProcess, StartAddress, &mbi, sizeof(mbi)) != 0)
        {
            uint32_t i = 0;
            uint32_t BlockSize = static_cast<uint32_t>(mbi.RegionSize);
            while (BlockSize >= BLOCKMAXSIZE)
            {
                if (::ReadProcessMemory(hProcess, StartAddress + (BLOCKMAXSIZE * i), MemoryData.GetPtr(), BLOCKMAXSIZE, NULL))
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
            if (::ReadProcessMemory(hProcess, StartAddress + (BLOCKMAXSIZE * i), MemoryData.GetPtr(), BlockSize, NULL))
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
            StartAddress += mbi.RegionSize;
            if (StartAddress >= EndAddress)
            {
                break;
            }
        }
        return static_cast<int32_t>(address_array.GetCount());
    }

    // 寻找内存字节集
    ptrdiff_t find_memory(const CVolMem &mem_data, const BYTE *start_ptr, const BYTE *end_ptr)
    {
        if (hProcess != NULL)
        {
            CMArray<INT_P> address_array;
            if (find_memory(mem_data, address_array, start_ptr, end_ptr, 1) > 0)
            {
                return address_array.GetAt(0);
            }
        }
        return 0;
    }

    // 读内存数据
    BOOL read_memory(void *write_address, const void *read_address, const size_t &read_size)
    {
        if (hProcess != NULL)
        {
            return ::ReadProcessMemory(hProcess, read_address, write_address, read_size, NULL);
        }
        return FALSE;
    }

    // 读内存字节集
    CVolMem read_memory(const void *read_address, const size_t &read_size)
    {
        if (hProcess != NULL)
        {
            CVolMem MemoryData;
            MemoryData.Alloc(read_size, TRUE);
            if (::ReadProcessMemory(hProcess, read_address, MemoryData.GetPtr(),
                                    read_size, NULL))
            {
                return MemoryData;
            }
        }
        return CVolMem();
    }

    // 读内存值
    template <typename R>
    R read_memory_value(const void *read_address)
    {
        R value;
        if (hProcess != NULL && ::ReadProcessMemory(hProcess, read_address, &value, sizeof(R), NULL))
        {
            return value;
        }
        return value;
    }

    // 写内存数据
    BOOL write_memory(void *write_address, const void *data_address, const size_t &data_size)
    {
        BOOL result = FALSE;
        if (hProcess != NULL)
        {
            DWORD dwOldProtect = 0;
            if (::VirtualProtectEx(hProcess, write_address, data_size, PAGE_EXECUTE_READWRITE, &dwOldProtect))
            {
                result = ::WriteProcessMemory(hProcess, write_address, data_address, data_size, NULL);
                ::VirtualProtectEx(hProcess, write_address, data_size, dwOldProtect, NULL);
            }
        }
        return result;
    }

    // 写内存字节集
    BOOL write_memory(void *write_address, const CVolMem &data)
    {
        BOOL result = FALSE;
        if (hProcess != NULL)
        {
            DWORD dwOldProtect = 0;
            if (::VirtualProtectEx(hProcess, write_address, static_cast<size_t>(data.GetSize()), PAGE_EXECUTE_READWRITE, &dwOldProtect))
            {
                result = ::WriteProcessMemory(hProcess, write_address, data.GetPtr(),
                                              static_cast<size_t>(data.GetSize()), NULL);
                ::VirtualProtectEx(hProcess, write_address, static_cast<size_t>(data.GetSize()), dwOldProtect, NULL);
            }
        }
        return result;
    }

    // 写内存值
    template <typename T>
    BOOL write_memory_value(void *write_address, const T &value)
    {
        BOOL result = false;
        if (hProcess != NULL)
        {
            DWORD dwOldProtect = 0;
            if (::VirtualProtectEx(hProcess, write_address, sizeof(T), PAGE_EXECUTE_READWRITE, &dwOldProtect))
            {
                result = ::WriteProcessMemory(hProcess, write_address,
                                              &value, sizeof(T), NULL);
                ::VirtualProtectEx(hProcess, write_address, sizeof(T), dwOldProtect, NULL);
            }
        }
        return result;
    }

    // 寻找模块特征码
    ptrdiff_t find_module_signatures(const HMODULE &hModule, const wchar_t *signatures, const size_t &start_off, const size_t &end_off)
    {
        ptrdiff_t result = 0;
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, hModule, &ModuleInfo, sizeof(MODULEINFO)) && start_off <= ModuleInfo.SizeOfImage && end_off <= ModuleInfo.SizeOfImage)
            {
                const BYTE *module_base = static_cast<const BYTE *>(ModuleInfo.lpBaseOfDll);
                result = find_signatures(signatures, module_base + start_off,
                                         module_base + (end_off ? end_off : ModuleInfo.SizeOfImage));
            }
        }
        return result;
    }

    // 寻找所有模块特征码
    int32_t find_module_signatures(const HMODULE &hModule, const wchar_t *signatures, CMArray<INT_P> &address_array, const size_t &start_off, const size_t &end_off)
    {
        int32_t result = 0;
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, hModule, &ModuleInfo, sizeof(MODULEINFO)) && start_off <= ModuleInfo.SizeOfImage && end_off <= ModuleInfo.SizeOfImage)
            {
                const BYTE *module_base = static_cast<const BYTE *>(ModuleInfo.lpBaseOfDll);
                result = find_signatures(signatures, address_array, module_base + start_off, module_base + (end_off ? end_off : ModuleInfo.SizeOfImage));
            }
        }
        return result;
    }

    // 寻找模块字节集
    ptrdiff_t find_module_memory(const HMODULE &hModule, const CVolMem &mem_data, const size_t &start_off, const size_t &end_off)
    {
        ptrdiff_t result = 0;
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, hModule, &ModuleInfo, sizeof(MODULEINFO)) && start_off < ModuleInfo.SizeOfImage && end_off < ModuleInfo.SizeOfImage)
            {
                const BYTE *module_base = static_cast<const BYTE *>(ModuleInfo.lpBaseOfDll);
                result = find_memory(mem_data, module_base + start_off, module_base + (end_off ? end_off : ModuleInfo.SizeOfImage));
            }
        }
        return result;
    }

    // 寻找所有模块字节集
    int32_t find_module_memory(const HMODULE &hModule, const CVolMem &mem_data, CMArray<INT_P> &address_array, const size_t &start_off, const size_t &end_off)
    {
        int32_t result = 0;
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, hModule, &ModuleInfo, sizeof(MODULEINFO)) && start_off < ModuleInfo.SizeOfImage && end_off < ModuleInfo.SizeOfImage)
            {
                const BYTE *module_base = static_cast<const BYTE *>(ModuleInfo.lpBaseOfDll);
                result = find_memory(mem_data, address_array, module_base + start_off, module_base + (end_off ? end_off : ModuleInfo.SizeOfImage));
            }
        }
        return result;
    }

    // 读内存数据
    BOOL read_module_memory(const HMODULE &hModule, void *write_address, const size_t &read_off, const size_t &read_size)
    {
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, hModule, &ModuleInfo, sizeof(MODULEINFO)) && read_off < ModuleInfo.SizeOfImage)
            {
                return ::ReadProcessMemory(hProcess, reinterpret_cast<const BYTE *>(ModuleInfo.lpBaseOfDll) + read_off,
                                           write_address, read_size, NULL);
            }
        }
        return false;
    }

    // 读内存字节集
    CVolMem read_module_memory(const HMODULE &hModule, const size_t &read_off, const size_t &read_size)
    {
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, hModule, &ModuleInfo, sizeof(MODULEINFO)) && read_off < ModuleInfo.SizeOfImage)
            {
                CVolMem MemoryData;
                MemoryData.Alloc(read_size, TRUE);
                if (::ReadProcessMemory(hProcess, reinterpret_cast<const BYTE *>(ModuleInfo.lpBaseOfDll) + read_off,
                                        MemoryData.GetPtr(), read_size, NULL))
                {
                    return MemoryData;
                }
            }
        }
        return CVolMem();
    }

    // 读模块内存值
    template <typename R>
    R read_module_value(const HMODULE &hModule, const size_t &read_off)
    {
        R value;
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, hModule, &ModuleInfo, sizeof(MODULEINFO)) && read_off < ModuleInfo.SizeOfImage)
            {
                if (::ReadProcessMemory(hProcess, reinterpret_cast<const BYTE *>(ModuleInfo.lpBaseOfDll) + read_off,
                                        &value, sizeof(R), NULL))
                {
                    return value;
                }
            }
        }
        return value;
    }
    // 写模块数据
    BOOL write_module_memory(const HMODULE &hModule, const size_t &write_off, const void *data_address, const size_t &data_size)
    {
        BOOL result = false;
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, hModule, &ModuleInfo, sizeof(MODULEINFO)) && write_off < ModuleInfo.SizeOfImage)
            {
                DWORD dwOldProtect = 0;
                BYTE *write_address = reinterpret_cast<BYTE *>(ModuleInfo.lpBaseOfDll) + write_off;
                if (::VirtualProtectEx(hProcess, write_address, data_size, PAGE_EXECUTE_READWRITE, &dwOldProtect))
                {
                    result = ::WriteProcessMemory(hProcess, write_address, data_address, data_size, NULL);
                    ::VirtualProtectEx(hProcess, write_address, data_size, dwOldProtect, NULL);
                }
            }
        }
        return result;
    }

    // 写模块字节集
    BOOL write_module_memory(const HMODULE &hModule, const size_t &write_off, const CVolMem &data)
    {
        BOOL result = false;
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, hModule, &ModuleInfo, sizeof(MODULEINFO)) && write_off < ModuleInfo.SizeOfImage)
            {
                DWORD dwOldProtect = 0;
                BYTE *write_address = reinterpret_cast<BYTE *>(ModuleInfo.lpBaseOfDll) + write_off;
                if (::VirtualProtectEx(hProcess, write_address, static_cast<size_t>(data.GetSize()), PAGE_EXECUTE_READWRITE, &dwOldProtect))
                {
                    result = ::WriteProcessMemory(hProcess, write_address, data.GetPtr(), static_cast<size_t>(data.GetSize()), NULL);
                    ::VirtualProtectEx(hProcess, write_address, static_cast<size_t>(data.GetSize()), dwOldProtect, NULL);
                }
            }
        }
        return result;
    }

    // 写模块值
    template <typename T>
    BOOL write_module_value(const HMODULE &hModule, const size_t &write_off, const T &value)
    {
        BOOL result = false;
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, hModule, &ModuleInfo, sizeof(MODULEINFO)) && write_off < ModuleInfo.SizeOfImage)
            {
                DWORD dwOldProtect = 0;
                BYTE *write_address = reinterpret_cast<BYTE *>(ModuleInfo.lpBaseOfDll) + write_off;
                if (::VirtualProtectEx(hProcess, write_address, sizeof(T), PAGE_EXECUTE_READWRITE, &dwOldProtect))
                {
                    result = ::WriteProcessMemory(hProcess, write_address, &value, sizeof(T), NULL);
                    ::VirtualProtectEx(hProcess, write_address, sizeof(T), dwOldProtect, NULL);
                }
            }
        }
        return result;
    }
}; // PivProcess

#endif // _PIV_PROCESS_HPP
