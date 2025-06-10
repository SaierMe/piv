/*********************************************\
 * 火山视窗PIV模块 - NT进程操作类            *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef _PIV_NT_PROCESS_HPP
#define _PIV_NT_PROCESS_HPP

#include "piv_ntdll.hpp"
#include <string>
#include <map>

/**
 * @brief 进程操作类
 */
class PivProcessNT
{
public:
    /**
     * @brief 模块信息类
     */
    struct ModuleInfo
    {
        USHORT BaseDllNameLen = 0;
        ULONG SizeOfImage = 0;
        CVolString FullDllName;
        ULONG64 lpBaseOfDll = 0;
        ULONG64 EntryPoint = 0;

        ModuleInfo() {}
        ~ModuleInfo() {}
        ModuleInfo(ULONG64 dll_base, ULONG64 entry_point, ULONG size_of_image, const wchar_t* dll_name, USHORT base_dllname_len)
        {
            lpBaseOfDll = dll_base;
            EntryPoint = entry_point;
            SizeOfImage = size_of_image;
            FullDllName.SetText(dll_name);
            BaseDllNameLen = base_dllname_len;
        }
    };

    /**
     * @brief 进程信息类
     */
    struct ProcessInfo
    {
        bool isWow64 = false;
        bool needClose = true;
        DWORD PID = 0;
        HANDLE hProcess = NULL;
        std::map<ULONG64, ModuleInfo> ModulesMap;
        ULONG64 ImageBaseAddress = 0;                    // 进程加载基地址
        ULONG64 Ldr = 0;                                 // 模块加载基地址
        ULONG64 ProcessHeap = 0;                         // 进程默认堆
        ULONG64 ProcessParameters = 0;                   // 进程信息
        NtPiv::UNICODE_STRING_T<ULONG64> Environment{0}; // 环境变量
        NtPiv::UNICODE_STRING_T<ULONG64> CommandLine{0}; // 命令行

        ProcessInfo() {}
        ~ProcessInfo()
        {
            if (needClose && hProcess != NULL)
                PivNT::instance().NtClose(hProcess);
        }
    };

private:
    std::unique_ptr<ProcessInfo> m_process = nullptr;
    const uint32_t BLOCKMAXSIZE = 409600;
    PivNT& Nt = PivNT::instance();

private:
    /**
     * @brief 获取所打开进程的基本信息
     */
    bool _get_base_info()
    {
        if (!m_process)
            return false;
        if (is_wow64(TRUE) == TRUE && is_wow64(FALSE) == FALSE)
        {
            m_process->isWow64 = true;
            NtPiv::PROCESS_BASIC_INFORMATION64 pbi{0};
            if (NT_SUCCESS(Nt.NtWow64QueryInformationProcess64(m_process->hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), NULL)))
            {
                NtPiv::PEB64 peb{0};
                NtPiv::RTL_USER_PROCESS_PARAMETERS64 Parameters{0};
                if (read_memory(&peb, reinterpret_cast<PVOID64>(pbi.PebBaseAddress), sizeof(peb)) &&
                    read_memory(&Parameters, reinterpret_cast<PVOID64>(peb.ProcessParameters), sizeof(Parameters)))
                {
                    m_process->ImageBaseAddress = peb.ImageBaseAddress;
                    m_process->Ldr = peb.Ldr;
                    m_process->ProcessHeap = peb.ProcessHeap;
                    m_process->ProcessParameters = peb.ProcessParameters;
                    m_process->Environment.Buffer = Parameters.Environment;
                    m_process->Environment.Length = static_cast<USHORT>(Parameters.EnvironmentSize);
                    m_process->CommandLine = Parameters.CommandLine;
                    // if (m_process->ImageBaseAddress != 0 && m_process->Ldr != 0)
                    //    return true;
                }
            }
        }
        else
        {
            NtPiv::PROCESS_BASIC_INFORMATION pbi{0};
            if (NT_SUCCESS(Nt.NtQueryInformationProcess(m_process->hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), NULL)))
            {
                NtPiv::PEB peb{0};
                NtPiv::RTL_USER_PROCESS_PARAMETERS_T<SIZE_T> Parameters{0};
                if (read_memory(&peb, reinterpret_cast<PVOID64>(pbi.PebBaseAddress), sizeof(peb)) &&
                    read_memory(&Parameters, reinterpret_cast<PVOID64>(peb.ProcessParameters), sizeof(Parameters)))
                {
                    m_process->ImageBaseAddress = peb.ImageBaseAddress;
                    m_process->Ldr = peb.Ldr;
                    m_process->ProcessHeap = peb.ProcessHeap;
                    m_process->ProcessParameters = peb.ProcessParameters;
                    m_process->Environment.Buffer = Parameters.Environment;
                    m_process->Environment.Length = static_cast<USHORT>(Parameters.EnvironmentSize);
                    m_process->CommandLine.Buffer = Parameters.CommandLine.Buffer;
                    m_process->CommandLine.Length = Parameters.CommandLine.Length;
                    // if (m_process->ImageBaseAddress != 0 && m_process->Ldr != 0)
                    //    return true;
                }
            }
        }
        return true;
    }

    /**
     * @brief 提升至调试权限
     * @return 返回是否成功
     */
    BOOL _enable_debug_privilege()
    {
        if (Nt.RtlAdjustPrivilege)
        {
            BOOLEAN Enabled;
            return NT_SUCCESS(Nt.RtlAdjustPrivilege(0x14, 1, 0, &Enabled));
        }
        else
        {
            HANDLE hToken;
            if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
                return FALSE;
            TOKEN_PRIVILEGES tokenPrivilege{0};
            if (!::LookupPrivilegeValueW(NULL, SE_DEBUG_NAME, &tokenPrivilege.Privileges[0].Luid))
                return FALSE;
            tokenPrivilege.PrivilegeCount = 1;
            tokenPrivilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            BOOL result = ::AdjustTokenPrivileges(hToken, FALSE, &tokenPrivilege, sizeof(tokenPrivilege), NULL, NULL);
            ::CloseHandle(hToken);
            return result;
        }
    }

    /**
     * @brief 查询虚拟内存
     * @param BaseAddress 虚拟内存地址
     * @param RegionSize 返回区域大小
     * @return 返回是否成功
     */
    bool _query_virtual_memory(PVOID64 BaseAddress, ULONGLONG* RegionSize, bool* check_mem = nullptr, DWORD protect = 0, DWORD mem_type = 0)
    {
        if (!m_process)
            return false;
        if (m_process->isWow64 && Nt.NtWow64QueryVirtualMemory64)
        {
            MEMORY_BASIC_INFORMATION64 mbi{0};
            if (NT_SUCCESS(Nt.NtWow64QueryVirtualMemory64(m_process->hProcess, BaseAddress, NtPiv::MemoryBasicInformation, &mbi, sizeof(mbi), NULL)))
            {
                *RegionSize = mbi.RegionSize;
                if (check_mem)
                {
                    if ((mbi.AllocationBase == 0 || mbi.Protect == PAGE_NOACCESS) || (protect != 0 && protect != mbi.Protect) || (mem_type != 0 && mem_type != mbi.Type))
                        *check_mem = false;
                    else
                        *check_mem = true;
                }
                return true;
            }
        }
        else
        {
            MEMORY_BASIC_INFORMATION mbi{0};
            if (NT_SUCCESS(Nt.NtQueryVirtualMemory(m_process->hProcess, reinterpret_cast<PVOID>(BaseAddress), NtPiv::MemoryBasicInformation, &mbi, sizeof(mbi), NULL)))
            {
                *RegionSize = mbi.RegionSize;
                if (check_mem)
                {
                    if ((mbi.AllocationBase == nullptr || mbi.Protect == PAGE_NOACCESS) || (protect != 0 && protect != mbi.Protect) || (mem_type != 0 && mem_type != mbi.Type))
                        *check_mem = false;
                    else
                        *check_mem = true;
                }
                return true;
            }
        }
        return false;
    }

    DWORD _query_protect(PVOID64 BaseAddress)
    {
        if (!m_process)
            return 0;
        if (m_process->isWow64 && Nt.NtWow64QueryVirtualMemory64)
        {
            MEMORY_BASIC_INFORMATION64 mbi{0};
            if (NT_SUCCESS(Nt.NtWow64QueryVirtualMemory64(m_process->hProcess, BaseAddress, NtPiv::MemoryBasicInformation, &mbi, sizeof(mbi), NULL)))
                return mbi.Protect;
        }
        else
        {
            MEMORY_BASIC_INFORMATION mbi{0};
            if (NT_SUCCESS(Nt.NtQueryVirtualMemory(m_process->hProcess, reinterpret_cast<PVOID>(BaseAddress), NtPiv::MemoryBasicInformation, &mbi, sizeof(mbi), NULL)))
                return mbi.Protect;
        }
        return 0;
    }

    ULONGLONG _query_base_address(PVOID64 BaseAddress)
    {
        if (!m_process)
            return 0;
        if (m_process->isWow64 && Nt.NtWow64QueryVirtualMemory64)
        {
            MEMORY_BASIC_INFORMATION64 mbi{0};
            if (NT_SUCCESS(Nt.NtWow64QueryVirtualMemory64(m_process->hProcess, BaseAddress, NtPiv::MemoryBasicInformation, &mbi, sizeof(mbi), NULL)))
                return mbi.BaseAddress;
        }
        else
        {
            MEMORY_BASIC_INFORMATION mbi{0};
            if (NT_SUCCESS(Nt.NtQueryVirtualMemory(m_process->hProcess, reinterpret_cast<PVOID>(BaseAddress), NtPiv::MemoryBasicInformation, &mbi, sizeof(mbi), NULL)))
                return reinterpret_cast<ULONGLONG>(mbi.BaseAddress);
        }
        return 0;
    }

public:
    PivProcessNT() {}
    ~PivProcessNT() {}

    PivProcessNT(DWORD process_id, DWORD desired_access)
    {
        open_process(process_id, desired_access);
    }

    /**
     * @brief 打开进程
     * @param process_id 进程ID
     * @param desired_access 访问权限
     * @param enable_debug 是否提权
     * @return 返回是否成功
     */
    bool open_process(DWORD process_id, DWORD desired_access, BOOL enable_debug = FALSE)
    {
        if (Nt.isLoaded() == true)
        {
            if (enable_debug)
                _enable_debug_privilege();
            m_process.reset(new ProcessInfo);
            m_process->PID = process_id ? process_id : ::GetCurrentProcessId();
            OBJECT_ATTRIBUTES oa{sizeof(OBJECT_ATTRIBUTES), NULL, NULL, 0, NULL, NULL};
            NtPiv::CLIENT_ID cid = {reinterpret_cast<HANDLE>(m_process->PID), NULL};
            if (NT_SUCCESS(Nt.NtOpenProcess(&m_process->hProcess, desired_access, &oa, &cid)) && _get_base_info())
                return true;
        }
        m_process.reset(nullptr);
        return false;
    }

    // 打开自句柄
    bool open_handle(HANDLE process, BOOL auto_close = false)
    {
        if (Nt.isLoaded() == true)
        {
            m_process.reset(new ProcessInfo);
            m_process->hProcess = process;
            if (_get_base_info())
            {
                m_process->needClose = auto_close;
                m_process->PID = ::GetProcessId(process);
                return true;
            }
        }
        m_process.reset(nullptr);
        return false;
    }

    /**
     * @brief 关闭进程
     */
    inline void close_process() noexcept
    {
        m_process.reset(nullptr);
    }

    /**
     * @brief 取模块信息排序表
     */
    inline std::map<ULONG64, ModuleInfo>& modules_map()
    {
        return m_process->ModulesMap;
    }

    /**
     * @brief 取进程句柄
     */
    inline HANDLE get_process_handle() const noexcept
    {
        return m_process ? m_process->hProcess : nullptr;
    }

    /**
     * @brief 取进程ID
     */
    inline DWORD get_process_id() const noexcept
    {
        return m_process ? m_process->PID : 0;
    }

    /**
     * @brief 取句柄数量
     */
    inline DWORD get_handle_count() const noexcept
    {
        DWORD dwHandleCount = 0;
        if (m_process)
            ::GetProcessHandleCount(m_process->hProcess, &dwHandleCount);
        return dwHandleCount;
    }

    /**
     * @brief 取进程基址
     */
    inline ULONG64 get_base_address() const noexcept
    {
        return m_process ? m_process->ImageBaseAddress : 0;
    }

    /**
     * @brief 取创建时间
     * @param 时区偏差
     */
    double get_creation_time(int32_t time_zoon = 2147483647) const noexcept
    {
        double dtime = 0.0;
        if (m_process)
        {
            if (time_zoon == 2147483647)
            {
                TIME_ZONE_INFORMATION tz{0};
                ::GetTimeZoneInformation(&tz);
                time_zoon = tz.Bias;
            }
            FILETIME CreationTime, ExitTime, KernelTime, UserTime;
            if (::GetProcessTimes(m_process->hProcess, &CreationTime, &ExitTime, &KernelTime, &UserTime))
            {
                SYSTEMTIME st;
                ::FileTimeToSystemTime(&CreationTime, &st);
                ::SystemTimeToVariantTime(&st, &dtime);
                dtime = TimeChange(dtime, 6, time_zoon);
            }
        }
        return dtime;
    }

    /**
     * @brief 取内存使用情况
     * @param 内存使用情况
     * @return 返回是否成功
     */
    inline BOOL get_memory_info(VOID* pMemCounters, size_t cb) const noexcept
    {
        if (m_process && Nt.loadPsapi())
        {
            return Nt.GetProcessMemoryInfo(m_process->hProcess, pMemCounters, static_cast<DWORD>(cb));
        }
        return FALSE;
    }

    /**
     * @brief 取优先级
     */
    inline DWORD get_priority_class() const noexcept
    {
        return m_process ? ::GetPriorityClass(m_process->hProcess) : 0;
    }

    /**
     * @brief 置优先级
     * @param priority 优先级
     * @return
     */
    inline BOOL set_priority_class(DWORD priority) const noexcept
    {
        return m_process ? ::SetPriorityClass(m_process->hProcess, priority) : FALSE;
    }

    /**
     * @brief 取进程DEP策略
     * @param flags 返回DEP设置
     * @param is_permanet 返回是否固定
     * @return 是否成功
     */
    inline BOOL get_dep_policy(DWORD& flags, BOOL& is_permanet) const noexcept
    {
        flags = 0;
        is_permanet = FALSE;
        if (m_process && Nt.GetProcessDEPPolicy)
            return Nt.GetProcessDEPPolicy(m_process->hProcess, &flags, &is_permanet);
        return FALSE;
    }

    /**
     * @brief 是否为WOW64模式
     * @param currentProcess 是否查询当前进程
     * @return
     */
    inline BOOL is_wow64(BOOL currentProcess = TRUE) const noexcept
    {
        BOOL result = FALSE;
        if (Nt.IsWow64Process)
        {
            if (currentProcess)
            {
                Nt.IsWow64Process(::GetCurrentProcess(), &result);
            }
            else if (m_process)
            {
                Nt.IsWow64Process(m_process->hProcess, &result);
            }
        }
        return result;
    }

    /**
     * @brief 取命令行
     */
    inline CVolString get_commandline() noexcept
    {
        if (m_process && m_process->CommandLine.Buffer != 0 && m_process->CommandLine.Length != 0)
        {
            CVolString result;
            result.SetLength(m_process->CommandLine.Length / 2);
            if (read_memory(const_cast<wchar_t*>(result.GetText()), reinterpret_cast<PVOID64>(m_process->CommandLine.Buffer), m_process->CommandLine.Length))
                return result;
        }
        return CWString(L"");
    }

    /**
     * @brief 取环境变量
     * @param env_array 环境变量数组
     * @return 返回变量数量
     */
    int32_t get_environment(CMStringArray& env_array) noexcept
    {
        env_array.RemoveAll();
        if (m_process && m_process->Environment.Buffer != NULL)
        {
            std::unique_ptr<BYTE[]> env(new BYTE[m_process->Environment.Length + 2]{0});
            if (read_memory(env.get(), reinterpret_cast<PVOID64>(m_process->Environment.Buffer), m_process->Environment.Length))
            {
                BYTE* szEnv = env.get();
                while (szEnv != NULL)
                {
                    env_array.Add(reinterpret_cast<const WCHAR*>(szEnv));
                    while (*reinterpret_cast<WCHAR*>(szEnv) != '\0')
                        szEnv += 2;
                    szEnv += 2;
                    if (*szEnv == '\0')
                        break;
                }
            }
        }
        return static_cast<int32_t>(env_array.GetCount());
    }

    /**
     * @brief 取模块句柄
     * @param module_name 模块名称
     * @param force_flush 是否重新枚举模块
     */
    ULONG64 get_module_handle(const CVolString& module_name, BOOL force_flush = FALSE) noexcept
    {
        if (m_process && enum_modules(static_cast<bool>(force_flush)) > 0)
        {
            if (module_name.IsEmpty())
                return m_process->ImageBaseAddress;
            for (auto& it = m_process->ModulesMap.begin(); it != m_process->ModulesMap.end(); it++)
            {
                if (it->second.FullDllName.SearchText(module_name.GetText(), -1, TRUE, TRUE) != -1)
                    return it->first;
            }
        }
        return 0;
    }

    /**
     * @brief 取模块基址
     * @param hModule 模块句柄
     */
    inline ULONG64 get_module_base(ULONG64 hModule) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                return m_process->ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end())
                return it->second.lpBaseOfDll;
        }
        return 0;
    }

    /**
     * @brief 枚举模块
     * @return 返回模块数量
     */
    int32_t enum_modules(bool force_flush = false) noexcept
    {
        if (!m_process && m_process->Ldr == 0)
            return 0;
        if (!force_flush && m_process->ModulesMap.size() > 0)
            return static_cast<int32_t>(m_process->ModulesMap.size());
        m_process->ModulesMap.clear();
        if (m_process->isWow64)
        {
            LIST_ENTRY64 ModuleList = {0};
            NtPiv::LDR_DATA_TABLE_ENTRY_T<ULONG64> CurrentModule = {0};
            wchar_t ModPath[MAX_PATH];
            if (read_memory(&ModuleList, PIV_PTR_FORWARD(PVOID64, m_process->Ldr, offsetof(NtPiv::PEB_LDR_DATA_T<ULONG64>, InLoadOrderModuleList)), sizeof(ModuleList)) &&
                read_memory(&CurrentModule, reinterpret_cast<PVOID64>(ModuleList.Flink), sizeof(CurrentModule)))
            {
                while (CurrentModule.InLoadOrderLinks.Flink != ModuleList.Flink)
                {
                    if (read_memory(ModPath, reinterpret_cast<PVOID64>(CurrentModule.FullDllName.Buffer), sizeof(ModPath)))
                        m_process->ModulesMap.emplace(std::piecewise_construct, std::forward_as_tuple(CurrentModule.DllBase),
                                                      std::forward_as_tuple(CurrentModule.DllBase, CurrentModule.EntryPoint, CurrentModule.SizeOfImage,
                                                                            ModPath, CurrentModule.BaseDllName.Length / 2));
                    if (!read_memory(&CurrentModule, reinterpret_cast<PVOID64>(CurrentModule.InLoadOrderLinks.Flink), sizeof(CurrentModule)))
                        break;
                }
            }
        }
        else
        {
            NtPiv::LIST_ENTRY_T<SIZE_T> ModuleList = {0};
            NtPiv::LDR_DATA_TABLE_ENTRY_T<SIZE_T> CurrentModule = {0};
            wchar_t ModPath[MAX_PATH];
            if (read_memory(&ModuleList, PIV_PTR_FORWARD(PVOID64, m_process->Ldr, offsetof(NtPiv::PEB_LDR_DATA_T<SIZE_T>, InLoadOrderModuleList)), sizeof(ModuleList)) &&
                read_memory(&CurrentModule, reinterpret_cast<PVOID64>(ModuleList.Flink), sizeof(CurrentModule)))
            {
                while (CurrentModule.InLoadOrderLinks.Flink != ModuleList.Flink)
                {
                    if (read_memory(ModPath, reinterpret_cast<PVOID64>(CurrentModule.FullDllName.Buffer), sizeof(ModPath)))
                        m_process->ModulesMap.emplace(std::piecewise_construct, std::forward_as_tuple(CurrentModule.DllBase),
                                                      std::forward_as_tuple(CurrentModule.DllBase, CurrentModule.EntryPoint, CurrentModule.SizeOfImage,
                                                                            ModPath, CurrentModule.BaseDllName.Length / 2));
                    if (!read_memory(&CurrentModule, reinterpret_cast<PVOID64>(CurrentModule.InLoadOrderLinks.Flink), sizeof(CurrentModule)))
                        break;
                }
            }
        }
        return static_cast<int32_t>(m_process->ModulesMap.size());
    }

    /**
     * @brief 枚举模块句柄
     * @param module_array 模块句柄数组
     * @return 返回模块数量
     */
    inline int32_t enum_modules(CMArray<INT64>& module_array) noexcept
    {
        module_array.RemoveAll();
        if (m_process && enum_modules(TRUE) > 0)
        {
            for (auto& it = m_process->ModulesMap.cbegin(); it != m_process->ModulesMap.cend(); it++)
                module_array.Add(static_cast<INT64>(it->first));
        }
        return static_cast<int32_t>(module_array.GetCount());
    }

    /**
     * @brief 枚举模块名称
     * @param module_array 模块句柄数组
     * @param name_array 模块名称数组
     * @param is_fullpath 是否完整路径
     * @return 返回模块数量
     */
    int32_t enum_modules(CMArray<INT64>& module_array, CMStringArray& name_array, BOOL is_fullpath) noexcept
    {
        module_array.RemoveAll();
        name_array.RemoveAll();
        if (m_process && enum_modules(TRUE) > 0)
        {
            for (auto& it = m_process->ModulesMap.begin(); it != m_process->ModulesMap.end(); it++)
            {
                module_array.Add(static_cast<INT64>(it->first));
                if (is_fullpath)
                    name_array.Add(it->second.FullDllName.GetText());
                else
                    name_array.Add(it->second.FullDllName.Right(it->second.BaseDllNameLen));
            }
        }
        return static_cast<int32_t>(module_array.GetCount());
    }

    /**
     * @brief 取模块信息
     * @param hModule 模块句柄
     * @param module_info 返回模块信息
     * @return 返回模块数量
     */
    bool get_module_info(ULONG64 hModule, ModuleInfo& module_info) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                hModule = m_process->ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end())
            {
                module_info = it->second;
                return true;
            }
        }
        return false;
    }

    /**
     * @brief 取模块名称
     * @param hModule 模块句柄
     */
    inline CVolString get_module_name(ULONG64 hModule) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                hModule = m_process->ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end())
                return it->second.FullDllName.Right(it->second.BaseDllNameLen);
        }
        return CWString(L"");
    }

    /**
     * @brief 取模块文件名
     * @param hModule 模块句柄
     */
    inline CVolString get_module_filename(ULONG64 hModule) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                hModule = m_process->ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end())
                return it->second.FullDllName;
        }
        return CWString(L"");
    }

    /**
     * @brief 取模块路径
     * @param hModule 模块句柄
     */
    inline CVolString get_module_path(ULONG64 hModule) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                hModule = m_process->ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end())
                return it->second.FullDllName.Left(it->second.FullDllName.GetLength() - it->second.BaseDllNameLen);
        }
        return CWString(L"");
    }

    /**
     * @brief 取内存映射文件
     * @param vmem_address 虚拟内存地址
     */
    inline CVolString get_mapped_filename(PVOID64 vmem_address) noexcept
    {
        if (m_process)
        {
            if (m_process->isWow64 && Nt.NtWow64QueryVirtualMemory64)
            {
                NtPiv::MEMORY_MAPPED_FILE_NAME_INFORMATION<ULONG64> mapFileName{0};
                if (NT_SUCCESS(Nt.NtWow64QueryVirtualMemory64(m_process->hProcess, vmem_address, NtPiv::MemoryMappedFilenameInformation, &mapFileName, sizeof(mapFileName), NULL)))
                    return CVolString(mapFileName.Buffer);
            }
            else
            {
                NtPiv::MEMORY_MAPPED_FILE_NAME_INFORMATION<PWSTR> mapFileName{0};
                if (NT_SUCCESS(Nt.NtQueryVirtualMemory(m_process->hProcess, reinterpret_cast<PVOID>(vmem_address), NtPiv::MemoryMappedFilenameInformation, &mapFileName, sizeof(mapFileName), NULL)))
                    return CVolString(mapFileName.Buffer);
            }
        }
        return CWString(L"");
    }

    /**
     * @brief 取进程映像文件名
     */
    inline CVolString get_image_filename() noexcept
    {
        if (m_process && Nt.loadPsapi())
        {
            CVolString ImageFilename;
            ImageFilename.m_mem.Alloc(1026, TRUE);
            if (Nt.GetProcessImageFileNameW(m_process->hProcess, const_cast<wchar_t*>(ImageFilename.GetText()), 512) > 0)
                return ImageFilename;
        }
        return CWString(L"");
    }

    /**
     * @brief 创建远程线程
     * @param func_ptr 函数指针
     * @param parm_ptr 参数指针
     * @param is_wait 是否等待
     * @return
     */
    inline bool create_remote_thread(void* func_ptr, void* parm_ptr, BOOL is_wait) noexcept
    {
        if (m_process)
        {
            HANDLE hThread = NULL;
            if (Nt.NtCreateThreadEx)
                Nt.NtCreateThreadEx(&hThread, 0x1FFFFF, NULL, m_process->hProcess, func_ptr, parm_ptr, 0, 0, 0, 0, NULL);
            else
                hThread = ::CreateRemoteThread(m_process->hProcess, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(func_ptr),
                                               parm_ptr, 0, NULL);
            if (hThread != NULL)
            {
                if (is_wait)
                    ::WaitForSingleObject(hThread, INFINITE);
                Nt.NtClose(hThread);
                return true;
            }
        }
        return false;
    }

    /**
     * @brief 分配虚拟内存
     * @param address 指定内存地址
     * @param men_size 分配大小
     * @param allocation_type 分配类型
     * @param protect 保护类型
     * @return 返回虚拟内存地址
     */
    inline PVOID64 virtual_alloc(PVOID64 address, uint64_t men_size, DWORD allocation_type, DWORD protect) noexcept
    {
        if (m_process)
        {
            NTSTATUS status;
            if (m_process->isWow64 && Nt.NtWow64AllocateVirtualMemory64)
                status = Nt.NtWow64AllocateVirtualMemory64(m_process->hProcess, &address, 0, &men_size, allocation_type, protect);
            else
                status = Nt.NtAllocateVirtualMemory(m_process->hProcess, reinterpret_cast<PVOID*>(&address), 0, reinterpret_cast<PSIZE_T>(&men_size), allocation_type, protect);
            if (NT_SUCCESS(status))
                return address;
        }
        return 0;
    }

    /**
     * @brief 分配邻近内存
     * @param address 指定内存地址
     * @param men_size 分配大小
     * @param allocation_type 分配类型
     * @param protect 保护类型
     * @param try_times 尝试次数
     * @return 返回虚拟内存地址
     */
    inline PVOID64 virtual_alloc_near(PVOID64 address, uint64_t men_size, DWORD allocation_type, DWORD protect, int try_times = 1000) noexcept
    {
        PVOID64 ret = virtual_alloc(address, men_size, allocation_type, protect);
        if (ret == nullptr)
        {
            ULONGLONG base_address = _query_base_address(address);
            if (base_address != 0)
            {
                base_address = base_address / 0x10000 * 0x10000;
                for (int i = 1; i < try_times; i++)
                {
                    ret = virtual_alloc(reinterpret_cast<PVOID64>(base_address + i * 0x10000), men_size, allocation_type, protect);
                    if (nullptr != ret)
                        break;
                    ret = virtual_alloc(reinterpret_cast<PVOID64>(base_address - i * 0x10000), men_size, allocation_type, protect);
                    if (nullptr != ret)
                        break;
                }
            }
        }
        return ret;
    }

    /**
     * @brief 释放虚拟内存
     * @param address 内存地址
     * @param men_size 内存大小
     * @param free_type 释放类型
     * @return 是否成功
     */
    inline bool virtual_free(void* address, size_t men_size, DWORD free_type) noexcept
    {
        if (m_process)
            return NT_SUCCESS(Nt.NtFreeVirtualMemory(m_process->hProcess, &address, reinterpret_cast<PSIZE_T>(&men_size), free_type));
        return false;
    }

    /**
     * @brief 修改内存保护
     * @param address 所欲修改的地址
     * @param mem_size 所欲修改的长度
     * @param new_protect 新的内存保护属性
     * @return 是否成功
     */
    inline int32_t modify_memory_protect(void* address, size_t mem_size, DWORD new_protect) noexcept
    {
        if (m_process && !m_process->isWow64)
        {
            ULONG OldProtect;
            if (NT_SUCCESS(Nt.NtProtectVirtualMemory(m_process->hProcess, &address, reinterpret_cast<PSIZE_T>(&mem_size), new_protect, &OldProtect)))
                return static_cast<int32_t>(OldProtect);
        }
        return -1;
    }

    /**
     * @brief 寻找内存特征码
     * @param signatures 所欲寻找的特征码
     * @param address_array 返回内存地址数组
     * @param start_ptr 起始地址
     * @param end_ptr 结束地址
     * @param max_count 选择返回地址的数量
     * @return 返回找到的内存地址数量
     */
    int32_t find_signatures(const wchar_t* signatures, CMArray<INT64>& address_array, PVOID64 start_ptr, PVOID64 end_ptr, DWORD protect = 0, DWORD mem_type = 0, ptrdiff_t max_count = PTRDIFF_MAX) noexcept
    {
        address_array.RemoveAll();
        if (!m_process || enum_modules() == 0)
            return 0;
        // PVOID64 StartAddress = (std::min)((std::max)(reinterpret_cast<PVOID64>(m_process->ModulesMap.begin()->first), start_ptr),
        //                                   reinterpret_cast<PVOID64>(m_process->ModulesMap.rbegin()->first));
        PVOID64 StartAddress = start_ptr;
        PVOID64 EndAddress = (end_ptr == 0) ? reinterpret_cast<PVOID64>(m_process->ModulesMap.rbegin()->first) : (std::max)(StartAddress, end_ptr);
        ULONGLONG RegionSize = 0;
        // 特征码转字节数组
        CWString SignatureStr{signatures};
        SignatureStr.Replace(L" ", L"");
        if (SignatureStr.GetLength() % 2 != 0)
            return 0;
        wchar_t* SignatureCode = const_cast<wchar_t*>(SignatureStr.GetText());
        size_t SignatureLength = static_cast<size_t>(SignatureStr.GetLength()) / 2;
        std::unique_ptr<WORD[]> BytesetSequence(new WORD[SignatureLength]);
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
                        a[i] = num[i] - '0';
                    else if (num[i] >= 'a' && num[i] <= 'z')
                        a[i] = num[i] - 87;
                    else if (num[i] >= 'A' && num[i] <= 'Z')
                        a[i] = num[i] - 55;
                }
                BytesetSequence[n] = a[0] * 16 + a[1];
            }
            else
            {
                BytesetSequence[n] = 256;
            }
        }
        // 获取Next数组
        short Next[260]; // 特征码的每个字节的范围在0-255(0-FF)之间,256用来表示问号,到260是为了防止越界
        for (short i = 0; i < 260; i++)
            Next[i] = -1;
        for (short i = 0; i < static_cast<short>(SignatureLength); i++)
            Next[BytesetSequence[i]] = i;

        std::unique_ptr<BYTE[]> MemoryData(new BYTE[BLOCKMAXSIZE]);
        bool check_mem;
        while (_query_virtual_memory(StartAddress, &RegionSize, &check_mem, protect, mem_type))
        {
            if (!check_mem)
                goto Next_Region;
            uint32_t i = 0;
            ULONG64 BlockSize = RegionSize;
            while (BlockSize >= BLOCKMAXSIZE)
            {
                if (read_memory(MemoryData.get(), PIV_PTR_FORWARD(PVOID64, StartAddress, BLOCKMAXSIZE * i), BLOCKMAXSIZE))
                {
                    for (size_t m = 0, j, k; m < BLOCKMAXSIZE;)
                    {
                        j = m;
                        k = 0;
                        for (; k < SignatureLength && j < BLOCKMAXSIZE && (BytesetSequence[k] == MemoryData[j] || BytesetSequence[k] == 256); k++, j++)
                            ;
                        if (k == SignatureLength)
                        {
                            if (address_array.Add2(reinterpret_cast<INT64>(StartAddress) + (BLOCKMAXSIZE * i) + m) >= (max_count - 1))
                                return static_cast<int32_t>(max_count); // 达到所欲获取的最大数组成员数后退出
                        }
                        if ((m + SignatureLength) >= BLOCKMAXSIZE)
                            break;
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
            if (read_memory(MemoryData.get(), PIV_PTR_FORWARD(PVOID64, StartAddress, BLOCKMAXSIZE * i), BlockSize))
            {
                for (size_t m = 0, j, k; m < BlockSize;)
                {
                    j = m;
                    k = 0;
                    for (; k < SignatureLength && j < BlockSize && (BytesetSequence[k] == MemoryData[j] || BytesetSequence[k] == 256); k++, j++)
                        ;
                    if (k == SignatureLength)
                    {
                        if (address_array.Add2(reinterpret_cast<INT64>(StartAddress) + (BLOCKMAXSIZE * i) + m) >= (max_count - 1))
                            return static_cast<int32_t>(max_count); // 达到所欲获取的最大数组成员数后退出
                        break;
                    }
                    if ((m + SignatureLength) >= BlockSize)
                        break;
                    int32_t num = Next[MemoryData[m + SignatureLength]];
                    if (num == -1)
                        m += (SignatureLength - Next[256]);
                    else
                        m += (SignatureLength - num);
                }
            }
        Next_Region:
            StartAddress = PIV_PTR_FORWARD(PVOID64, StartAddress, RegionSize);
            if (StartAddress >= EndAddress)
                break;
        }
        return static_cast<int32_t>(address_array.GetCount());
    }

    /**
     * @brief 寻找内存特征码
     * @param signatures 所欲寻找的特征码
     * @param start_ptr 起始地址
     * @param end_ptr 结束地址
     * @return 返回找到的内存地址
     */
    inline INT64 find_signatures(const wchar_t* signatures, PVOID64 start_ptr, PVOID64 end_ptr, DWORD protect = 0, DWORD mem_type = 0) noexcept
    {
        if (m_process)
        {
            CMArray<INT64> address_array;
            if (find_signatures(signatures, address_array, start_ptr, end_ptr, protect, mem_type, 1) > 0)
                return address_array.GetAt(0);
        }
        return 0;
    }

    /**
     * @brief 寻找所有内存字节集
     * @param mem_data 所欲寻找的字节集
     * @param address_array 返回内存地址数组
     * @param start_ptr 起始地址
     * @param end_ptr 结束地址
     * @param max_count 选择返回地址的数量
     * @return 返回找到的内存地址数量
     */
    int32_t find_memory(const CVolMem& mem_data, CMArray<INT64>& address_array, PVOID64 start_ptr, PVOID64 end_ptr, DWORD protect = 0, DWORD mem_type = 0, ptrdiff_t max_count = PTRDIFF_MAX) noexcept
    {
        address_array.RemoveAll();
        if (!m_process || enum_modules() == 0)
            return 0;
        // PVOID64 StartAddress = (std::min)((std::max)(reinterpret_cast<PVOID64>(m_process->ModulesMap.begin()->first), start_ptr),
        //                                   reinterpret_cast<PVOID64>(m_process->ModulesMap.rbegin()->first));
        PVOID64 StartAddress = start_ptr;
        PVOID64 EndAddress = (end_ptr == 0) ? reinterpret_cast<PVOID64>(m_process->ModulesMap.rbegin()->first) : (std::max)(StartAddress, end_ptr);
        ULONGLONG RegionSize = 0;
        CVolMem MemoryData;
        MemoryData.Alloc(BLOCKMAXSIZE, TRUE);
        bool check_mem;
        while (_query_virtual_memory(StartAddress, &RegionSize, &check_mem, protect, mem_type))
        {
            if (!check_mem)
                goto Next_Region;
            uint32_t i = 0;
            ULONGLONG BlockSize = RegionSize;
            while (BlockSize >= BLOCKMAXSIZE)
            {
                if (read_memory(MemoryData.GetPtr(), PIV_PTR_FORWARD(PVOID64, StartAddress, BLOCKMAXSIZE * i), BLOCKMAXSIZE))
                {
                    INT_P ret = MemoryData.FindBin(mem_data, 0);
                    if (ret != -1)
                    {
                        if (address_array.Add2(reinterpret_cast<INT64>(StartAddress) + (BLOCKMAXSIZE * i) + ret) >= (max_count - 1))
                            return static_cast<int32_t>(max_count); // 达到所欲获取的最大数组成员数后退出
                    }
                }
                BlockSize -= BLOCKMAXSIZE;
                i++;
            }
            MemoryData.Zero();
            if (read_memory(MemoryData.GetPtr(), PIV_PTR_FORWARD(PVOID64, StartAddress, BLOCKMAXSIZE * i), BlockSize))
            {
                INT_P ret = MemoryData.FindBin(mem_data, 0);
                if (ret != -1)
                {
                    if (address_array.Add2(reinterpret_cast<INT64>(StartAddress) + (BLOCKMAXSIZE * i) + ret) >= (max_count - 1)) // 达到所欲获取的最大数组成员数后退出
                        return static_cast<int32_t>(max_count);
                }
            }
        Next_Region:
            StartAddress = PIV_PTR_FORWARD(PVOID64, StartAddress, RegionSize);
            if (StartAddress >= EndAddress)
                break;
        }
        return static_cast<int32_t>(address_array.GetCount());
    }

    /**
     * @brief 寻找内存字节集
     * @param mem_data 所欲寻找的字节集
     * @param start_ptr 起始地址
     * @param end_ptr 结束地址
     * @return 返回找到的内存地址
     */
    inline INT64 find_memory(const CVolMem& mem_data, PVOID64 start_ptr, PVOID64 end_ptr, DWORD protect = 0, DWORD mem_type = 0) noexcept
    {
        if (m_process)
        {
            CMArray<INT64> address_array;
            if (find_memory(mem_data, address_array, start_ptr, end_ptr, protect, mem_type, 1) > 0)
                return address_array.GetAt(0);
        }
        return 0;
    }

    /**
     * @brief 读内存数据
     * @param write_address 保存数据的地址
     * @param read_address 所欲读取的地址
     * @param read_size 所欲读取的长度
     * @return 返回是否成功
     */
    inline bool read_memory(void* write_address, PVOID64 read_address, uint64_t read_size) noexcept
    {
        if (m_process)
        {
            if (m_process->isWow64 && Nt.NtWow64ReadVirtualMemory64)
                return NT_SUCCESS(Nt.NtWow64ReadVirtualMemory64(m_process->hProcess, read_address, write_address, read_size, NULL));
            else
                return NT_SUCCESS(Nt.NtReadVirtualMemory(m_process->hProcess, reinterpret_cast<void*>(read_address), write_address, static_cast<SIZE_T>(read_size), NULL));
        }
        return false;
    }

    /**
     * @brief 读内存字节集
     * @param read_address 所欲读取的地址
     * @param read_size 所欲读取的长度
     * @return 返回读取的字节集
     */
    inline CVolMem read_memory(PVOID64 read_address, uint64_t read_size) noexcept
    {
        if (m_process)
        {
            CVolMem MemoryData;
            MemoryData.Alloc(static_cast<INT_P>(read_size), TRUE);
            if (read_memory(MemoryData.GetPtr(), read_address, read_size))
                return MemoryData;
        }
        return CVolMem();
    }

    /**
     * @brief 读内存多字节
     * @param read_address 所欲读取的地址
     * @return 返回读取的字节集
     */
    inline CVolMem read_memory_char(PVOID64 read_address) noexcept
    {
        CVolMem MemoryData;
        if (m_process)
        {
            std::unique_ptr<char[]> buffer{new char[1024]};
            ULONGLONG RegionSize = 0;
            if (_query_virtual_memory(read_address, &RegionSize, nullptr, 0, 0))
            {
                size_t i = 0;
                while (RegionSize >= 1024)
                {
                    if (read_memory(buffer.get(), PIV_PTR_FORWARD(PVOID64, read_address, 1024 * i), 1024))
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

    /**
     * @brief 读内存文本
     * @param read_address 所欲读取的地址
     * @return 返回读取的文本
     */
    inline CVolString read_memory_wchar(PVOID64 read_address) noexcept
    {
        CVolString MemoryData;
        if (m_process)
        {
            std::unique_ptr<wchar_t[]> buffer{new wchar_t[512]};
            ULONGLONG RegionSize = 0;
            if (_query_virtual_memory(read_address, &RegionSize, nullptr, 0, 0))
            {
                size_t i = 0;
                while (RegionSize >= 1024)
                {
                    if (read_memory(buffer.get(), PIV_PTR_FORWARD(PVOID64, read_address, 1024 * i), 1024))
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

    /**
     * @brief 读内存值
     * @tparam T 值类型
     * @param read_address 所欲读取的地址
     * @param value 返回所读取的值
     * @return 是否成功
     */
    template <typename T>
    inline BOOL read_memory_num(PVOID64 read_address, T& value) noexcept
    {
        return read_memory(&value, read_address, sizeof(T));
    }

    /**
     * @brief 读内存值
     * @tparam R 值类型
     * @param read_address 所欲读取的地址
     * @return 返回所读取的值
     */
    template <typename R>
    inline R read_memory_value(PVOID64 read_address) noexcept
    {
        R value;
        read_memory(&value, read_address, sizeof(R));
        return value;
    }

    /**
     * @brief 写内存数据
     * @param write_address 所欲写入的地址
     * @param data_address 所欲写入的数据
     * @param data_size 所欲写入的长度
     * @return 是否写入成功
     */
    inline bool write_memory(PVOID64 write_address, const void* data_address, size_t data_size, BOOL change_protect = TRUE) noexcept
    {
        bool result = false;
        if (m_process)
        {
            if (m_process->isWow64 && Nt.NtWow64WriteVirtualMemory64)
            {
                DWORD Protect = _query_protect(write_address);
                if ((Protect & PAGE_READWRITE) == PAGE_READWRITE || (Protect & PAGE_EXECUTE_READWRITE) == PAGE_EXECUTE_READWRITE ||
                    (Protect & PAGE_EXECUTE_WRITECOPY) == PAGE_EXECUTE_WRITECOPY || (Protect & PAGE_WRITECOPY) == PAGE_WRITECOPY)
                    result = NT_SUCCESS(Nt.NtWow64WriteVirtualMemory64(m_process->hProcess, write_address, data_address, data_size, NULL));
            }
            else
            {
                DWORD dwNewProtect = PAGE_READWRITE;
                if (change_protect)
                {
                    DWORD Protect = _query_protect(write_address);
                    if ((Protect & PAGE_READWRITE) == PAGE_READWRITE || (Protect & PAGE_EXECUTE_READWRITE) == PAGE_EXECUTE_READWRITE ||
                        (Protect & PAGE_EXECUTE_WRITECOPY) == PAGE_EXECUTE_WRITECOPY || (Protect & PAGE_WRITECOPY) == PAGE_WRITECOPY)
                    {
                        change_protect = FALSE;
                    }
                    else if ((Protect & PAGE_EXECUTE) == PAGE_EXECUTE || (Protect & PAGE_EXECUTE_READ) == PAGE_EXECUTE_READ)
                    {
                        dwNewProtect = PAGE_EXECUTE_READWRITE;
                    }
                }
                if (!change_protect)
                    return NT_SUCCESS(Nt.NtWriteVirtualMemory(m_process->hProcess, reinterpret_cast<PVOID>(write_address), data_address, data_size, NULL));
                DWORD dwOldProtect = 0;
                VOID* BaseAddress = reinterpret_cast<VOID*>(write_address);
                SIZE_T RegionSize = static_cast<SIZE_T>(data_size);
                if (NT_SUCCESS(Nt.NtProtectVirtualMemory(m_process->hProcess, &BaseAddress, &RegionSize, dwNewProtect, &dwOldProtect)))
                {
                    result = NT_SUCCESS(Nt.NtWriteVirtualMemory(m_process->hProcess, reinterpret_cast<PVOID>(write_address), data_address, data_size, NULL));
                    Nt.NtProtectVirtualMemory(m_process->hProcess, &BaseAddress, &RegionSize, dwOldProtect, &dwOldProtect);
                }
            }
        }
        return result;
    }

    /**
     * @brief 写内存字节集
     * @param write_address 所欲写入的地址
     * @param data 所欲写入的字节集
     * @return 是否写入成功
     */
    inline bool write_memory(PVOID64 write_address, const CVolMem& data, BOOL change_protect = TRUE) noexcept
    {
        return write_memory(write_address, data.GetPtr(), static_cast<size_t>(data.GetSize()), change_protect);
    }

    /**
     * @brief 写内存文本
     * @param write_address 所欲写入的地址
     * @param text 所欲写入的文本
     * @return 是否写入成功
     */
    inline bool write_memory(PVOID64 write_address, const wchar_t* text, BOOL change_protect = TRUE) noexcept
    {
        return write_memory(write_address, text, (wcslen(text) + 1) * 2, change_protect);
    }

    inline bool write_memory(PVOID64 write_address, const char* text, BOOL change_protect = TRUE) noexcept
    {
        return write_memory(write_address, text, strlen(text) + 1, change_protect);
    }

    /**
     * @brief 写内存值
     * @tparam T 值类型
     * @param write_address 所欲写入的地址
     * @param value 所欲写入的值
     * @return 是否写入成功
     */
    template <typename T>
    inline bool write_memory_value(PVOID64 write_address, T value, BOOL change_protect = TRUE) noexcept
    {
        return write_memory(write_address, &value, sizeof(T), change_protect);
    }

    /**
     * @brief 寻找模块特征码
     * @param hModule 模块句柄
     * @param signatures 所欲寻找的特征码
     * @param start_off 起始偏移地址
     * @param end_off 结束偏移地址
     * @return 返回找到的虚拟内存地址
     */
    inline INT64 find_module_signatures(ULONG64 hModule, const wchar_t* signatures, size_t start_off, size_t end_off, DWORD protect = 0, DWORD mem_type = 0) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                hModule = m_process->ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end() && start_off < it->second.SizeOfImage && end_off < it->second.SizeOfImage)
                return find_signatures(signatures, PIV_PTR_FORWARD(PVOID64, it->first, start_off),
                                       PIV_PTR_FORWARD(PVOID64, it->first, end_off ? end_off : it->second.SizeOfImage), protect, mem_type);
        }
        return 0;
    }

    /**
     * @brief 寻找所有模块特征码
     * @param hModule 模块句柄
     * @param signatures 所欲寻找的特征码
     * @param address_array 结果地址数组
     * @param start_off 起始偏移地址
     * @param end_off 结束偏移地址
     * @return 返回找到的虚拟内存地址数量
     */
    inline int32_t find_module_signatures(ULONG64 hModule, const wchar_t* signatures, CMArray<INT64>& address_array, size_t start_off, size_t end_off, DWORD protect = 0, DWORD mem_type = 0) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                hModule = m_process->ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end() && start_off < it->second.SizeOfImage && end_off < it->second.SizeOfImage)
                return find_signatures(signatures, address_array, PIV_PTR_FORWARD(PVOID64, it->first, start_off),
                                       PIV_PTR_FORWARD(PVOID64, it->first, end_off ? end_off : it->second.SizeOfImage), protect, mem_type);
        }
        return 0;
    }

    /**
     * @brief 寻找模块字节集
     * @param hModule 模块句柄
     * @param mem_data 所欲寻找的字节集
     * @param start_off 起始偏移地址
     * @param end_off 结束偏移地址
     * @return 返回找到的虚拟内存地址
     */
    inline INT64 find_module_memory(ULONG64 hModule, const CVolMem& mem_data, size_t start_off, size_t end_off, DWORD protect = 0, DWORD mem_type = 0) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end() && start_off < it->second.SizeOfImage && end_off < it->second.SizeOfImage)
                return find_memory(mem_data, PIV_PTR_FORWARD(PVOID64, it->first, start_off),
                                   PIV_PTR_FORWARD(PVOID64, it->first, end_off ? end_off : it->second.SizeOfImage), protect, mem_type);
        }
        return 0;
    }

    /**
     * @brief 寻找所有模块字节集
     * @param hModule 模块句柄
     * @param mem_data 所欲寻找的字节集
     * @param address_array 结果地址数组
     * @param start_off 起始偏移地址
     * @param end_off 结束偏移地址
     * @return 返回找到的虚拟内存地址数量
     */
    inline int32_t find_module_memory(ULONG64 hModule, const CVolMem& mem_data, CMArray<INT64>& address_array, size_t start_off, size_t end_off, DWORD protect = 0, DWORD mem_type = 0) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                hModule = m_process->ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end() && start_off < it->second.SizeOfImage && end_off < it->second.SizeOfImage)
                return find_memory(mem_data, address_array, PIV_PTR_FORWARD(PVOID64, it->first, start_off),
                                   PIV_PTR_FORWARD(PVOID64, it->first, end_off ? end_off : it->second.SizeOfImage), protect, mem_type);
        }
        return 0;
    }

    /**
     * @brief 读模块数据
     * @param hModule 模块句柄
     * @param write_address 保存数据的地址
     * @param read_off 所欲读取的偏移地址
     * @param read_size 所欲读取的长度
     * @return 是否读取成功
     */
    inline bool read_module_memory(ULONG64 hModule, void* write_address, size_t read_off, size_t read_size) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                hModule = m_process->ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end() && read_off < it->second.SizeOfImage)
                return read_memory(write_address, PIV_PTR_FORWARD(PVOID64, it->first, read_off), read_size);
        }
        return false;
    }

    /**
     * @brief 读模块字节集
     * @param hModule 模块句柄
     * @param read_off 所欲读取的偏移地址
     * @param read_size 所欲读取的长度
     * @return 是否读取的字节集
     */
    inline CVolMem read_module_memory(ULONG64 hModule, size_t read_off, size_t read_size) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                hModule = m_process->ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end() && read_off < it->second.SizeOfImage)
                return read_memory(PIV_PTR_FORWARD(PVOID64, it->first, read_off), read_size);
        }
        return CVolMem();
    }

    /**
     * @brief 读模块多字节
     * @param hModule 模块句柄
     * @param read_off 所欲读取的偏移地址
     * @return 是否读取的字节集
     */
    inline CVolMem read_module_char(ULONG64 hModule, size_t read_off) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                hModule = m_process->ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end() && read_off < it->second.SizeOfImage)
                return read_memory_char(PIV_PTR_FORWARD(PVOID64, it->first, read_off));
        }
        return CVolMem();
    }

    /**
     * @brief 读模块文本
     * @param hModule 模块句柄
     * @param read_off 所欲读取的偏移地址
     * @return 是否读取的字节集
     */
    inline CVolString read_module_wchar(ULONG64 hModule, size_t read_off) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                hModule = m_process->ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end() && read_off < it->second.SizeOfImage)
                return read_memory_wchar(PIV_PTR_FORWARD(PVOID64, it->first, read_off));
        }
        return CVolString();
    }

    /**
     * @brief 读模块内存值
     * @tparam R 值类型
     * @param hModule 模块句柄
     * @param read_off 所欲读取的偏移地址
     * @return 返回读取的值
     */
    template <typename T>
    inline bool read_module_num(ULONG64 hModule, T& value, size_t read_off) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                hModule = ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end() && read_off < it->second.SizeOfImage)
                return read_memory(&value, PIV_PTR_FORWARD(PVOID64, it->first, read_off), sizeof(T));
        }
        return false;
    }

    /**
     * @brief 读模块内存值
     * @tparam R 值类型
     * @param hModule 模块句柄
     * @param read_off 所欲读取的偏移地址
     * @return 返回读取的值
     */
    template <typename R>
    inline R read_module_value(ULONG64 hModule, size_t read_off) noexcept
    {
        R value;
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                hModule = ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end() && read_off < it->second.SizeOfImage)
                return read_memory(&value, PIV_PTR_FORWARD(PVOID64, it->first, read_off), sizeof(R));
        }
        return value;
    }

    /**
     * @brief 写模块数据
     * @param hModule 模块句柄
     * @param write_off 所欲写入的偏移地址
     * @param data_address 所欲写入的数据
     * @param data_size 所欲写入的长度
     * @return 是否写入成功
     */
    inline bool write_module_memory(ULONG64 hModule, size_t write_off, const void* data_address, size_t data_size, BOOL change_protect = TRUE) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                hModule = m_process->ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end() && write_off < it->second.SizeOfImage)
                return write_memory(PIV_PTR_FORWARD(PVOID64, it->first, write_off), data_address, data_size, change_protect);
        }
        return false;
    }

    /**
     * @brief 写模块字节集
     * @param hModule 模块句柄
     * @param write_off 所欲写入的偏移地址
     * @param data 所欲写入的字节集
     * @return 是否写入成功
     */
    inline bool write_module_memory(ULONG64 hModule, size_t write_off, const CVolMem& data, BOOL change_protect = TRUE) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                hModule = m_process->ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end() && write_off < it->second.SizeOfImage)
                return write_memory(PIV_PTR_FORWARD(PVOID64, it->first, write_off), data, change_protect);
        }
        return false;
    }

    /**
     * @brief 写模块文本
     * @param hModule 模块句柄
     * @param write_off 所欲写入的偏移地址
     * @param text 所欲写入的文本
     * @return 是否写入成功
     */
    inline bool write_module_memory(ULONG64 hModule, size_t write_off, const wchar_t* text, BOOL change_protect = TRUE) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                hModule = m_process->ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end() && write_off < it->second.SizeOfImage)
                return write_memory(PIV_PTR_FORWARD(PVOID64, it->first, write_off), text, (wcslen(text) + 1) * 2, change_protect);
        }
        return false;
    }

    inline bool write_module_memory(ULONG64 hModule, size_t write_off, const char* text, BOOL change_protect = TRUE) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                hModule = m_process->ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end() && write_off < it->second.SizeOfImage)
                return write_memory(PIV_PTR_FORWARD(PVOID64, it->first, write_off), text, strlen(text) + 1, change_protect);
        }
        return false;
    }

    /**
     * @brief 写模块值
     * @tparam T 值类型
     * @param hModule 模块句柄
     * @param write_off 所欲写入的偏移地址
     * @param value 所欲写入的值
     * @return
     */
    template <typename T>
    inline bool write_module_value(ULONG64 hModule, size_t write_off, T value, BOOL change_protect = TRUE) noexcept
    {
        if (m_process && enum_modules() > 0)
        {
            if (hModule == 0)
                hModule = m_process->ImageBaseAddress;
            auto& it = m_process->ModulesMap.find(hModule);
            if (it != m_process->ModulesMap.end() && write_off < it->second.SizeOfImage)
                return write_memory(PIV_PTR_FORWARD(PVOID64, it->first, write_off), &value, sizeof(T), change_protect);
        }
        return false;
    }
}; // PivProcessNT

#endif // _PIV_NT_PROCESS_HPP
