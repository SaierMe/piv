/*********************************************\
 * 火山视窗PIV模块 - 进程操作类              *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 版本: 2022/12/13                          *
\*********************************************/

#ifndef _PIV_PROCESS_HPP
#define _PIV_PROCESS_HPP

// 包含火山视窗基本类,它在火山里的包含顺序比较靠前(全局-110)
#ifndef __VOL_BASE_H__
#include <sys/base/libs/win_base/vol_base.h>
#endif

#include <cstdint>
#include <string>
#include <psapi.h>
#include <tlhelp32.h>
#include <winternl.h>
#pragma comment(lib, "Psapi.lib")

class PivProcess
{
private:
    HANDLE hProcess = NULL;
    BYTE *ProcessStart = NULL;
    BYTE *ProcessEnd = NULL;
    DWORD PID;
    MODULEINFO sProcessModuleInfo{0, 0, 0};

    const uint32_t BLOCKMAXSIZE{409600};
    typedef BOOL(WINAPI *Piv_GetProcessDEPPolicy)(HANDLE, LPDWORD, PBOOL);
    typedef BOOL(WINAPI *Piv_IsWow64Process)(HANDLE, PBOOL);
    typedef NTSTATUS(NTAPI *Piv_ZwQueryInformationProcess)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);
    typedef NTSTATUS(NTAPI *Piv_NtCreateThreadEx)(OUT PHANDLE hThread, IN ACCESS_MASK DesiredAccess, IN PVOID ObjectAttributes,
                                                  IN HANDLE ProcessHandle, IN PVOID lpStartAddress, IN PVOID lpParameter, IN ULONG Flags,
                                                  IN SIZE_T StackZeroBits, IN SIZE_T SizeOfStackCommit, IN SIZE_T SizeOfStackReserve, OUT PVOID lpBytesBuffer);

    // 取首模块信息
    void get_base_module_info(DWORD pid)
    {
        HANDLE hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
        if (hModuleSnap != INVALID_HANDLE_VALUE)
        {
            MODULEENTRY32W me32{0};
            me32.dwSize = sizeof(me32);
            if (::Module32FirstW(hModuleSnap, &me32))
            {
                ProcessStart = me32.modBaseAddr;
                do
                {
                    if (ProcessStart == NULL || me32.modBaseAddr < ProcessStart)
                    {
                        ProcessStart = me32.modBaseAddr;
                    }
                    if (ProcessEnd < me32.modBaseAddr + me32.modBaseSize)
                    {
                        ProcessEnd = me32.modBaseAddr + me32.modBaseSize;
                    }
                } while (::Module32NextW(hModuleSnap, &me32));
            }
            ::CloseHandle(hModuleSnap);
        }
    }

public:
    PivProcess() {}
    PivProcess(int32_t process_id, int32_t desired_access)
    {
        open_process(process_id, desired_access);
    }
    ~PivProcess()
    {
        close_process();
    }

    // 打开进程
    BOOL open_process(int32_t process_id, int32_t desired_access)
    {
        close_process();
        PID = process_id ? static_cast<DWORD>(process_id) : ::GetCurrentProcessId();
        hProcess = ::OpenProcess(static_cast<DWORD>(desired_access), FALSE, PID);
        if (hProcess == NULL)
        {
            return false;
        }
        get_base_module_info(PID);
        return true;
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
    ptrdiff_t get_process_handle()
    {
        return reinterpret_cast<ptrdiff_t>(hProcess);
    }

    // 取进程ID
    int32_t get_process_id()
    {
        return static_cast<int32_t>(PID);
    }

    // 取句柄数量
    int32_t get_handle_count()
    {
        DWORD dwHandleCount = 0;
        if (hProcess != NULL)
        {
            ::GetProcessHandleCount(hProcess, &dwHandleCount);
        }
        return static_cast<int32_t>(dwHandleCount);
    }

    // 取创建时间
    double get_creation_time(int32_t time_zoon)
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
        {
            return ::GetProcessMemoryInfo(hProcess, pMemCounters, sizeof(PROCESS_MEMORY_COUNTERS));
        }
        return false;
    }

    // 取优先级
    int32_t get_priority_class()
    {
        return hProcess ? ::GetPriorityClass(hProcess) : 0;
    }

    // 置优先级
    BOOL set_priority_class(int32_t priority)
    {
        return ::SetPriorityClass(hProcess, static_cast<DWORD>(priority));
    }

    // 取进程DEP策略
    BOOL get_dep_policy(int32_t &flags, BOOL &is_permanet)
    {
        flags = 0;
        is_permanet = false;
        if (hProcess != NULL)
        {
            Piv_GetProcessDEPPolicy pfn = reinterpret_cast<Piv_GetProcessDEPPolicy>(
                ::GetProcAddress(::GetModuleHandleW(L"Kernel32.dll"), "GetProcessDEPPolicy"));
            if (pfn)
            {
                return pfn(hProcess, reinterpret_cast<LPDWORD>(&flags), reinterpret_cast<PBOOL>(&is_permanet));
            }
        }
        return false;
    }

    // 是否为WOW64模式
    BOOL is_wow64()
    {
        BOOL result = false;
        if (hProcess != NULL)
        {
            Piv_IsWow64Process pfn = reinterpret_cast<Piv_IsWow64Process>(
                ::GetProcAddress(::GetModuleHandle(L"Kernel32.dll"), "IsWow64Process"));
            if (pfn)
            {
                pfn(::GetCurrentProcess(), &result);
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
    ptrdiff_t get_module_handle(const wchar_t *module_name)
    {
        HMODULE hRes = 0;
        if (hProcess != NULL)
        {
            DWORD dwNeeded = 0;
            ::EnumProcessModulesEx(hProcess, NULL, 0, &dwNeeded, 3);
            HMODULE *lphModule = (HMODULE *)malloc(dwNeeded + 4);
            if (lphModule == NULL)
            {
                return 0;
            }
            std::wstring module_name_sv{module_name};
            WCHAR szBaseName[MAX_PATH + 1]{'\0'};
            if (module_name_sv.empty())
            {
                ::GetModuleBaseNameW(hProcess, 0, szBaseName, MAX_PATH);
                module_name_sv = szBaseName;
            }
            ::EnumProcessModulesEx(hProcess, lphModule, dwNeeded + 4, &dwNeeded, 3);
            size_t nModuleCount = dwNeeded / sizeof(HMODULE);
            WCHAR szFileName[MAX_PATH + 1]{'\0'};
            for (size_t i = 0; i < nModuleCount; i++)
            {
                ::GetModuleFileNameExW(hProcess, lphModule[i], szFileName, MAX_PATH);
                std::wstring sFileName{szFileName};
                if (sFileName.rfind(module_name_sv, MAX_PATH) != std::wstring::npos)
                {
                    hRes = lphModule[i];
                    break;
                }
            }
            free(lphModule);
        }
        return reinterpret_cast<ptrdiff_t>(hRes);
    }

    // 取模块基址
    ptrdiff_t get_module_base(ptrdiff_t hMoudle)
    {
        if (hProcess != NULL)
        {
            MODULEINFO sModuleInfo{0};
            if (::GetModuleInformation(hProcess, reinterpret_cast<HMODULE>(hMoudle), &sModuleInfo, sizeof(sModuleInfo)))
            {
                return reinterpret_cast<ptrdiff_t>(sModuleInfo.lpBaseOfDll);
            }
        }
        return 0;
    }

    // 枚举模块句柄
    int32_t enum_modules(CMArray<INT_P> &module_array, int32_t filter_flag)
    {
        module_array.RemoveAll();
        if (hProcess != NULL)
        {
            DWORD dwNeeded = 0;
            ::EnumProcessModulesEx(hProcess, NULL, 0, &dwNeeded, static_cast<DWORD>(filter_flag));
            HMODULE *lphModule = (HMODULE *)malloc(dwNeeded + 4);
            if (lphModule == NULL)
            {
                return 0;
            }
            ::EnumProcessModulesEx(hProcess, lphModule, dwNeeded + 4, &dwNeeded, static_cast<DWORD>(filter_flag));
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
    int32_t enum_modules(CMArray<INT_P> &module_array, CMStringArray &name_array, BOOL is_fullpath, int32_t filter_flag)
    {
        module_array.RemoveAll();
        name_array.RemoveAll();
        if (hProcess != NULL)
        {
            DWORD dwNeeded = 0;
            ::EnumProcessModulesEx(hProcess, NULL, 0, &dwNeeded, static_cast<DWORD>(filter_flag));
            HMODULE *lphModule = (HMODULE *)malloc(dwNeeded + 4);
            if (lphModule == NULL)
            {
                return 0;
            }
            ::EnumProcessModulesEx(hProcess, lphModule, dwNeeded + 4, &dwNeeded, static_cast<DWORD>(filter_flag));
            size_t nModuleCount = dwNeeded / sizeof(HMODULE);
            WCHAR szFileName[MAX_PATH + 1]{'\0'};
            for (size_t i = 0; i < nModuleCount; i++)
            {
                if (is_fullpath)
                {
                    ::GetModuleFileNameExW(hProcess, lphModule[i], szFileName, MAX_PATH);
                }
                else
                {
                    ::GetModuleBaseNameW(hProcess, lphModule[i], szFileName, MAX_PATH);
                }
                name_array.Add(szFileName);
                module_array.Add(reinterpret_cast<INT_P>(lphModule[i]));
            }
            free(lphModule);
        }
        return static_cast<int32_t>(module_array.GetCount());
    }

    // 取模块信息
    BOOL get_module_info(ptrdiff_t hModule, MODULEINFO *pModuleInfo)
    {
        if (hProcess != NULL)
        {
            return ::GetModuleInformation(hProcess, reinterpret_cast<HMODULE>(hModule), pModuleInfo, sizeof(MODULEINFO));
        }
        return false;
    }

    // 取模块名称
    CVolString get_module_name(ptrdiff_t hModule)
    {
        if (hProcess != NULL)
        {
            WCHAR szBaseName[MAX_PATH + 1]{'\0'};
            if (::GetModuleBaseNameW(hProcess, reinterpret_cast<HMODULE>(hModule), szBaseName, MAX_PATH) > 0)
            {
                return CVolString(szBaseName);
            }
        }
        return _CT("");
    }

    // 取模块文件名
    CVolString get_module_filename(ptrdiff_t hModule)
    {
        if (hProcess != NULL)
        {
            WCHAR szFilename[MAX_PATH + 1]{'\0'};
            if (::GetModuleFileNameExW(hProcess, reinterpret_cast<HMODULE>(hModule), szFilename, MAX_PATH) > 0)
            {
                return CVolString(szFilename);
            }
        }
        return _CT("");
    }

    // 取内存映射文件
    CVolString get_mapped_filename(ptrdiff_t mem_ptr)
    {
        if (hProcess != NULL)
        {
            WCHAR szFilename[MAX_PATH + 1]{'\0'};
            if (::GetMappedFileNameW(hProcess, reinterpret_cast<LPVOID>(mem_ptr), szFilename, MAX_PATH) > 0)
            {
                return CVolString(szFilename);
            }
        }
        return _CT("");
    }

    // 取进程映像文件名
    CVolString get_image_filename()
    {
        if (hProcess != NULL)
        {
            WCHAR szImageFilename[MAX_PATH + 1]{'\0'};
            if (::GetProcessImageFileNameW(hProcess, szImageFilename, MAX_PATH) > 0)
            {
                return CVolString(szImageFilename);
            }
        }
        return _CT("");
    }

    // 创建远程线程
    BOOL create_remote_thread(ptrdiff_t func_ptr, ptrdiff_t parm_ptr, BOOL is_wait)
    {
        BOOL result = false;
        if (hProcess != NULL)
        {
            HANDLE hThread = NULL;
            Piv_NtCreateThreadEx pfn = reinterpret_cast<Piv_NtCreateThreadEx>(
                ::GetProcAddress(::GetModuleHandleW(L"ntdll.dll"), "NtCreateThreadEx"));
            if (pfn != NULL)
            {
                pfn(&hThread, 0x1FFFFF, NULL, hProcess, reinterpret_cast<PVOID>(func_ptr),
                    reinterpret_cast<PVOID>(parm_ptr), 0, NULL, NULL, NULL, NULL);
            }
            else
            {
                hThread = ::CreateRemoteThread(hProcess, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(func_ptr),
                                               reinterpret_cast<PVOID>(parm_ptr), 0, NULL);
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
    ptrdiff_t virtual_alloc(ptrdiff_t address, ptrdiff_t men_size, int32_t allocation_type, int32_t protect)
    {
        return reinterpret_cast<ptrdiff_t>(::VirtualAllocEx(hProcess, reinterpret_cast<LPVOID>(address),
                                                            static_cast<SIZE_T>(men_size), static_cast<DWORD>(allocation_type),
                                                            static_cast<DWORD>(protect)));
    }

    // 释放虚拟内存
    BOOL virtual_free(ptrdiff_t address, ptrdiff_t men_size, int32_t free_type)
    {
        return ::VirtualFreeEx(hProcess, reinterpret_cast<LPVOID>(address),
                               static_cast<SIZE_T>(men_size), static_cast<DWORD>(free_type));
    }
    // 修改内存保护
    int32_t modify_memory_protect(ptrdiff_t address, ptrdiff_t mem_size, int32_t new_protect)
    {
        if (hProcess != NULL)
        {
            DWORD dwOldProtect;
            if (::VirtualProtectEx(hProcess, reinterpret_cast<LPVOID>(address), static_cast<SIZE_T>(mem_size),
                                   static_cast<DWORD>(new_protect), &dwOldProtect))
            {
                return static_cast<int32_t>(dwOldProtect);
            }
        }
        return -1;
    }

    // 寻找所有内存特征码
    int32_t find_signatures(const wchar_t *signatures, CMArray<INT_P> &address_array, ptrdiff_t start_ptr, ptrdiff_t end_ptr, ptrdiff_t max_count = PTRDIFF_MAX)
    {
        address_array.RemoveAll();
        if (hProcess == NULL)
        {
            return 0;
        }
        size_t StartAddress = MAX(reinterpret_cast<size_t>(ProcessStart), static_cast<size_t>(start_ptr));
        size_t EndAddress = (end_ptr <= 0) ? reinterpret_cast<size_t>(ProcessEnd) : MAX(StartAddress, static_cast<size_t>(end_ptr));
        size_t RetAddress = 0;
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
        while (::VirtualQueryEx(hProcess, reinterpret_cast<LPCVOID>(StartAddress), &mbi, sizeof(mbi)) != 0)
        {
            uint32_t i = 0;
            uint32_t BlockSize = static_cast<uint32_t>(mbi.RegionSize);
            while (BlockSize >= BLOCKMAXSIZE)
            {
                if (::ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(StartAddress + (BLOCKMAXSIZE * i)), MemoryData, BLOCKMAXSIZE, NULL))
                {
                    for (size_t m = 0, j, k; m < BLOCKMAXSIZE;)
                    {
                        j = m;
                        k = 0;
                        for (; k < TzmLength && j < BLOCKMAXSIZE && (TzmArray[k] == MemoryData[j] || TzmArray[k] == 256); k++, j++)
                            ;
                        if (k == TzmLength)
                        {
                            if (address_array.Add2(static_cast<INT_P>(StartAddress + (BLOCKMAXSIZE * i) + m)) >= (max_count - 1))
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
            if (::ReadProcessMemory(hProcess, (LPCVOID)(StartAddress + (BLOCKMAXSIZE * i)), MemoryData, BlockSize, NULL))
            {
                for (size_t m = 0, j, k; m < BlockSize;)
                {
                    j = m;
                    k = 0;
                    for (; k < TzmLength && j < BlockSize && (TzmArray[k] == MemoryData[j] || TzmArray[k] == 256); k++, j++)
                        ;
                    if (k == TzmLength)
                    {
                        if (address_array.Add2(static_cast<INT_P>(StartAddress + (BLOCKMAXSIZE * i) + m)) >= (max_count - 1))
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
    ptrdiff_t find_signatures(const wchar_t *signatures, ptrdiff_t start_ptr = 0, ptrdiff_t end_ptr = PTRDIFF_MAX)
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
    int32_t find_memory(CVolMem &mem_data, CMArray<INT_P> &address_array, ptrdiff_t start_ptr = 0, ptrdiff_t end_ptr = PTRDIFF_MAX, ptrdiff_t max_count = PTRDIFF_MAX)
    {
        address_array.RemoveAll();
        if (hProcess == NULL)
        {
            return 0;
        }
        size_t StartAddress = MAX(reinterpret_cast<size_t>(ProcessStart), static_cast<size_t>(start_ptr));
        size_t EndAddress = (end_ptr <= 0) ? reinterpret_cast<size_t>(ProcessEnd) : MAX(StartAddress, static_cast<size_t>(end_ptr));
        MEMORY_BASIC_INFORMATION mbi{0};
        CVolMem MemoryData;
        MemoryData.Alloc(BLOCKMAXSIZE, TRUE);
        while (::VirtualQueryEx(hProcess, reinterpret_cast<LPCVOID>(StartAddress), &mbi, sizeof(mbi)) != 0)
        {
            uint32_t i = 0;
            uint32_t BlockSize = static_cast<uint32_t>(mbi.RegionSize);
            while (BlockSize >= BLOCKMAXSIZE)
            {
                if (::ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(StartAddress + (BLOCKMAXSIZE * i)), reinterpret_cast<LPVOID>(MemoryData.GetPtr()), BLOCKMAXSIZE, NULL))
                {
                    INT_P ret = MemoryData.FindBin(mem_data, 0);
                    if (ret != -1)
                    {
                        if (address_array.Add2(StartAddress + (BLOCKMAXSIZE * i) + static_cast<size_t>(ret)) >= (max_count - 1))
                        {
                            return static_cast<int32_t>(max_count); // 达到所欲获取的最大数组成员数后退出
                        }
                    }
                }
                BlockSize -= BLOCKMAXSIZE;
                i++;
            }
            MemoryData.Zero();
            if (::ReadProcessMemory(hProcess, (LPCVOID)(StartAddress + (BLOCKMAXSIZE * i)), reinterpret_cast<LPVOID>(MemoryData.GetPtr()), BlockSize, NULL))
            {
                INT_P ret = MemoryData.FindBin(mem_data, 0);
                if (ret != -1)
                {
                    if (address_array.Add2(StartAddress + (BLOCKMAXSIZE * i) + static_cast<size_t>(ret)) >= (max_count - 1)) // 达到所欲获取的最大数组成员数后退出
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
    ptrdiff_t find_memory(CVolMem &mem_data, ptrdiff_t start_ptr = 0, ptrdiff_t end_ptr = PTRDIFF_MAX)
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
    BOOL read_memory(ptrdiff_t write_address, ptrdiff_t read_address, ptrdiff_t read_size)
    {
        if (hProcess != NULL)
        {
            return ::ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(read_address), reinterpret_cast<LPVOID>(write_address),
                                       static_cast<SIZE_T>(read_size), NULL);
        }
        return false;
    }

    // 读内存字节集
    CVolMem read_memory(ptrdiff_t read_address, ptrdiff_t read_size)
    {
        if (hProcess != NULL)
        {
            CVolMem MemoryData;
            MemoryData.Alloc(read_size, TRUE);
            if (::ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(read_address), MemoryData.GetPtr(),
                                    static_cast<SIZE_T>(read_size), NULL))
            {
                return MemoryData;
            }
        }
        return CVolMem();
    }

    // 读内存值
    template <typename R>
    R read_memory_value(ptrdiff_t read_address)
    {
        R value;
        if (hProcess != NULL)
        {
            if (::ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(read_address),
                                    &value, sizeof(R), NULL))
            {
                return value;
            }
        }
        return value;
    }

    // 写内存数据
    BOOL write_memory(ptrdiff_t write_address, ptrdiff_t data_address, ptrdiff_t data_size)
    {
        BOOL result = false;
        if (hProcess != NULL)
        {
            DWORD dwOldProtect = 0;
            if (::VirtualProtectEx(hProcess, reinterpret_cast<LPVOID>(write_address), static_cast<SIZE_T>(data_size), PAGE_EXECUTE_READWRITE, &dwOldProtect))
            {
                result = ::WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(write_address), reinterpret_cast<LPCVOID>(data_address),
                                              static_cast<SIZE_T>(data_size), NULL);
                ::VirtualProtectEx(hProcess, reinterpret_cast<LPVOID>(write_address), static_cast<SIZE_T>(data_size), dwOldProtect, NULL);
            }
        }
        return result;
    }

    // 写内存字节集
    BOOL write_memory(ptrdiff_t write_address, CVolMem &data)
    {
        BOOL result = false;
        if (hProcess != NULL)
        {
            DWORD dwOldProtect = 0;
            if (::VirtualProtectEx(hProcess, reinterpret_cast<LPVOID>(write_address), static_cast<SIZE_T>(data.GetSize()), PAGE_EXECUTE_READWRITE, &dwOldProtect))
            {
                result = ::WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(write_address), data.GetPtr(),
                                              static_cast<SIZE_T>(data.GetSize()), NULL);
                ::VirtualProtectEx(hProcess, reinterpret_cast<LPVOID>(write_address), static_cast<SIZE_T>(data.GetSize()), dwOldProtect, NULL);
            }
        }
        return result;
    }

    // 写内存值
    template <typename T>
    BOOL write_memory_value(ptrdiff_t write_address, T value)
    {
        BOOL result = false;
        if (hProcess != NULL)
        {
            DWORD dwOldProtect = 0;
            if (::VirtualProtectEx(hProcess, reinterpret_cast<LPVOID>(write_address), sizeof(T), PAGE_EXECUTE_READWRITE, &dwOldProtect))
            {
                result = ::WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(write_address),
                                              &value, sizeof(T), NULL);
                ::VirtualProtectEx(hProcess, reinterpret_cast<LPVOID>(write_address), sizeof(T), dwOldProtect, NULL);
            }
        }
        return result;
    }

    // 寻找模块特征码
    ptrdiff_t find_module_signatures(ptrdiff_t hModule, const wchar_t *signatures, ptrdiff_t start_off, ptrdiff_t end_off)
    {
        ptrdiff_t result = 0;
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, reinterpret_cast<HMODULE>(hModule), &ModuleInfo, sizeof(MODULEINFO)))
            {
                ptrdiff_t module_base = reinterpret_cast<ptrdiff_t>(ModuleInfo.lpBaseOfDll);
                result = find_signatures(signatures, start_off < module_base ? module_base : start_off,
                                         end_off > static_cast<ptrdiff_t>(ModuleInfo.SizeOfImage) ? module_base + ModuleInfo.SizeOfImage : module_base + end_off);
            }
        }
        return result;
    }

    // 寻找所有模块特征码
    ptrdiff_t find_module_signatures(ptrdiff_t hModule, const wchar_t *signatures, CMArray<INT_P> &address_array, ptrdiff_t start_off, ptrdiff_t end_off)
    {
        ptrdiff_t result = 0;
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, reinterpret_cast<HMODULE>(hModule), &ModuleInfo, sizeof(MODULEINFO)))
            {
                ptrdiff_t module_base = reinterpret_cast<ptrdiff_t>(ModuleInfo.lpBaseOfDll);
                result = find_signatures(signatures, address_array, start_off < module_base ? module_base : start_off,
                                         end_off > static_cast<ptrdiff_t>(ModuleInfo.SizeOfImage) ? module_base + ModuleInfo.SizeOfImage : module_base + end_off);
            }
        }
        return result;
    }

    // 寻找模块字节集
    ptrdiff_t find_module_memory(ptrdiff_t hModule, CVolMem &mem_data, ptrdiff_t start_off, ptrdiff_t end_off)
    {
        ptrdiff_t result = 0;
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, reinterpret_cast<HMODULE>(hModule), &ModuleInfo, sizeof(MODULEINFO)))
            {
                ptrdiff_t module_base = reinterpret_cast<ptrdiff_t>(ModuleInfo.lpBaseOfDll);
                result = find_memory(mem_data, start_off < module_base ? module_base : start_off,
                                     end_off > static_cast<ptrdiff_t>(ModuleInfo.SizeOfImage) ? module_base + ModuleInfo.SizeOfImage : module_base + end_off);
            }
        }
        return result;
    }

    // 寻找模块字节集
    ptrdiff_t find_module_memory(ptrdiff_t hModule, CVolMem &mem_data, CMArray<INT_P> &address_array, ptrdiff_t start_off, ptrdiff_t end_off)
    {
        ptrdiff_t result = 0;
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, reinterpret_cast<HMODULE>(hModule), &ModuleInfo, sizeof(MODULEINFO)))
            {
                ptrdiff_t module_base = reinterpret_cast<ptrdiff_t>(ModuleInfo.lpBaseOfDll);
                result = find_memory(mem_data, address_array, start_off < module_base ? module_base : start_off,
                                     end_off > static_cast<ptrdiff_t>(ModuleInfo.SizeOfImage) ? module_base + ModuleInfo.SizeOfImage : module_base + end_off);
            }
        }
        return result;
    }

    // 读内存数据
    BOOL read_module_memory(ptrdiff_t hModule, ptrdiff_t write_address, ptrdiff_t read_off, ptrdiff_t read_size)
    {
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, reinterpret_cast<HMODULE>(hModule), &ModuleInfo, sizeof(MODULEINFO)))
            {
                if (read_off < static_cast<ptrdiff_t>(ModuleInfo.SizeOfImage))
                {
                    return ::ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(reinterpret_cast<ptrdiff_t>(ModuleInfo.lpBaseOfDll) + read_off),
                                               reinterpret_cast<LPVOID>(write_address), static_cast<SIZE_T>(read_size), NULL);
                }
            }
        }
        return false;
    }

    // 读内存字节集
    CVolMem read_module_memory(ptrdiff_t hModule, ptrdiff_t read_off, ptrdiff_t read_size)
    {
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, reinterpret_cast<HMODULE>(hModule), &ModuleInfo, sizeof(MODULEINFO)))
            {
                if (read_off < static_cast<ptrdiff_t>(ModuleInfo.SizeOfImage))
                {
                    CVolMem MemoryData;
                    MemoryData.Alloc(read_size, TRUE);
                    if (::ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(reinterpret_cast<ptrdiff_t>(ModuleInfo.lpBaseOfDll) + read_off),
                                            MemoryData.GetPtr(), static_cast<SIZE_T>(read_size), NULL))
                    {
                        return MemoryData;
                    }
                }
            }
        }
        return CVolMem();
    }

    // 读模块内存值
    template <typename R>
    R read_module_value(ptrdiff_t hModule, ptrdiff_t read_off)
    {
        R value;
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, reinterpret_cast<HMODULE>(hModule), &ModuleInfo, sizeof(MODULEINFO)))
            {
                if (read_off < static_cast<ptrdiff_t>(ModuleInfo.SizeOfImage))
                {
                    if (::ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(reinterpret_cast<ptrdiff_t>(ModuleInfo.lpBaseOfDll) + read_off),
                                            &value, sizeof(R), NULL))
                    {
                        return value;
                    }
                }
            }
        }
        return value;
    }
    // 写模块数据
    BOOL write_module_memory(ptrdiff_t hModule, ptrdiff_t write_off, ptrdiff_t data_address, ptrdiff_t data_size)
    {
        BOOL result = false;
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, reinterpret_cast<HMODULE>(hModule), &ModuleInfo, sizeof(MODULEINFO)))
            {
                DWORD dwOldProtect = 0;
                LPVOID write_address = reinterpret_cast<LPVOID>(reinterpret_cast<ptrdiff_t>(ModuleInfo.lpBaseOfDll) + write_off);
                if (::VirtualProtectEx(hProcess, write_address, static_cast<SIZE_T>(data_size), PAGE_EXECUTE_READWRITE, &dwOldProtect))
                {
                    result = ::WriteProcessMemory(hProcess, write_address, reinterpret_cast<LPCVOID>(data_address), static_cast<SIZE_T>(data_size), NULL);
                    ::VirtualProtectEx(hProcess, write_address, static_cast<SIZE_T>(data_size), dwOldProtect, NULL);
                }
            }
        }
        return result;
    }

    // 写模块字节集
    BOOL write_module_memory(ptrdiff_t hModule, ptrdiff_t write_off, CVolMem &data)
    {
        BOOL result = false;
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, reinterpret_cast<HMODULE>(hModule), &ModuleInfo, sizeof(MODULEINFO)))
            {
                DWORD dwOldProtect = 0;
                LPVOID write_address = reinterpret_cast<LPVOID>(reinterpret_cast<ptrdiff_t>(ModuleInfo.lpBaseOfDll) + write_off);
                if (::VirtualProtectEx(hProcess, write_address, static_cast<SIZE_T>(data.GetSize()), PAGE_EXECUTE_READWRITE, &dwOldProtect))
                {
                    result = ::WriteProcessMemory(hProcess, write_address, data.GetPtr(), static_cast<SIZE_T>(data.GetSize()), NULL);
                    ::VirtualProtectEx(hProcess, write_address, static_cast<SIZE_T>(data.GetSize()), dwOldProtect, NULL);
                }
            }
        }
        return result;
    }

    // 写模块值
    template <typename T>
    BOOL write_module_value(ptrdiff_t hModule, ptrdiff_t write_off, T value)
    {
        BOOL result = false;
        if (hProcess != NULL)
        {
            MODULEINFO ModuleInfo{0};
            if (::GetModuleInformation(hProcess, reinterpret_cast<HMODULE>(hModule), &ModuleInfo, sizeof(MODULEINFO)))
            {
                DWORD dwOldProtect = 0;
                LPVOID write_address = reinterpret_cast<LPVOID>(reinterpret_cast<ptrdiff_t>(ModuleInfo.lpBaseOfDll) + write_off);
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

namespace piv
{
    namespace process
    {
        // 创建进程
        static int32_t create_process(const wchar_t *app_name, const wchar_t *commandline, BOOL waitend, ptrdiff_t show_window)
        {
            STARTUPINFO infStartup{0};
            infStartup.cb = sizeof(infStartup);
            if (show_window >= 0)
            {
                infStartup.dwFlags |= STARTF_USESHOWWINDOW;
                infStartup.wShowWindow = static_cast<WORD>(show_window);
            }
            PROCESS_INFORMATION pi{0};
            CVolMem memCommandLine;
            memCommandLine.AddString(commandline);
            if (!::CreateProcessW(wcslen(app_name) ? app_name : NULL,
                                  wcslen(commandline) ? reinterpret_cast<LPWSTR>(memCommandLine.GetPtr()) : NULL,
                                  NULL, NULL, FALSE, 0, NULL, NULL, &infStartup, &pi))
            {
                return 0;
            }
            if (waitend)
            {
                ::WaitForSingleObject(pi.hProcess, INFINITE);
            }
            else
            {
                ::WaitForInputIdle(pi.hProcess, 500);
            }
            ::CloseHandle(pi.hThread);
            ::CloseHandle(pi.hProcess);
            return static_cast<int32_t>(pi.dwProcessId);
        }

        // 提升进程权限
        static BOOL adjust_privileges(const wchar_t *privilege)
        {
            HANDLE hToken;
            if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
            {
                return false;
            }
            TOKEN_PRIVILEGES tokenPrivilege{0};
            if (!::LookupPrivilegeValueW(NULL, privilege, &tokenPrivilege.Privileges[0].Luid))
            {
                return false;
            }
            tokenPrivilege.PrivilegeCount = 1;
            tokenPrivilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            BOOL result = ::AdjustTokenPrivileges(hToken, FALSE, &tokenPrivilege, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
            ::CloseHandle(hToken);
            return result;
        }

        // 调整当前权限
        static int32_t Rtl_adjust_privilege(int32_t privilege, bool enbale, bool currentThread, BOOL *enabled)
        {
            typedef DWORD(WINAPI * NT_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
            NT_RtlAdjustPrivilege pfn = reinterpret_cast<NT_RtlAdjustPrivilege>(::GetProcAddress(::GetModuleHandle(L"Ntdll.dll"), "RtlAdjustPrivilege"));
            if (pfn)
            {
                return static_cast<int32_t>(pfn(static_cast<ULONG>(privilege), enbale, currentThread, reinterpret_cast<PBOOLEAN>(enabled)));
            }
            return -1;
        }

        // 置进程DEP策略
        static BOOL set_dep_policy(int32_t dep)
        {
            typedef BOOL(WINAPI * Typedef_SetProcessDEPPolicy)(DWORD);
            Typedef_SetProcessDEPPolicy pfn = reinterpret_cast<Typedef_SetProcessDEPPolicy>(::GetProcAddress(::GetModuleHandleW(L"Kernel32.dll"), "SetProcessDEPPolicy"));
            if (pfn)
            {
                return pfn(static_cast<DWORD>(dep));
            }
            return false;
        }

        // 进程ID取窗口句柄
        static ptrdiff_t get_hwnd_from_pid(int32_t pid)
        {
            HWND hWnd = ::GetTopWindow(NULL);
            DWORD dwFindPID = pid ? static_cast<DWORD>(pid) : ::GetCurrentProcessId();
            while (hWnd)
            {
                DWORD dwPID = 0;
                DWORD dwTheardId = ::GetWindowThreadProcessId(hWnd, &dwPID);
                if (dwTheardId)
                {
                    if (dwPID == dwFindPID)
                        return (INT_P)hWnd;
                }
                hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT);
            }
            return 0;
        }

    }
} // namespace piv

#endif // _PIV_PROCESS_HPP
