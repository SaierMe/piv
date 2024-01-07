/*********************************************\
 * 火山视窗PIV模块 - Ntdll                   *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef _PIV_NTDLL_HPP
#define _PIV_NTDLL_HPP

#include "detail/piv_base.hpp"
#include <winternl.h>

namespace NtPiv
{
    enum MEMORY_INFORMATION_CLASS
    {
        MemoryBasicInformation = 0,
        MemoryWorkingSetInformation,
        MemoryMappedFilenameInformation,
        MemoryRegionInformation,
        MemoryWorkingSetExInformation
    };

    struct CLIENT_ID
    {
        HANDLE UniqueProcess;
        HANDLE UniqueThread;
    };

    // Kernel32
    typedef BOOL(WINAPI *pGetProcessDEPPolicy)(IN HANDLE, IN LPDWORD, OUT PBOOL);
    typedef BOOL(WINAPI *pIsWow64Process)(HANDLE, OUT PBOOL);

    // Nt
    typedef NTSTATUS(NTAPI *pNtOpenProcess)(OUT PHANDLE ProcessHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes, IN OPTIONAL CLIENT_ID *ClientId);
    typedef NTSTATUS(NTAPI *pNtClose)(IN HANDLE Handle);
    typedef NTSTATUS(NTAPI *pNtCreateThreadEx)(OUT PHANDLE hThread, IN ACCESS_MASK DesiredAccess, IN PVOID ObjectAttributes,
                                               IN HANDLE ProcessHandle, IN PVOID lpStartAddress, IN PVOID lpParameter, IN ULONG Flags,
                                               IN SIZE_T StackZeroBits, IN SIZE_T SizeOfStackCommit, IN SIZE_T SizeOfStackReserve, OUT PVOID lpBytesBuffer);
    typedef NTSTATUS(NTAPI *pNtQueryInformationProcess)(IN HANDLE ProcessHandle, IN PROCESSINFOCLASS ProcessInformationClass, OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT OPTIONAL PULONG ReturnLength);
    typedef NTSTATUS(NTAPI *pNtAllocateVirtualMemory)(IN HANDLE ProcessHandle, IN OUT PVOID *BaseAddress, IN ULONG_PTR ZeroBits, IN OUT PSIZE_T RegionSize, IN ULONG AllocationType, IN ULONG Protect);
    typedef NTSTATUS(NTAPI *pNtFreeVirtualMemory)(IN HANDLE ProcessHandle, IN OUT PVOID *BaseAddress, IN OUT PSIZE_T RegionSize, IN ULONG FreeType);
    typedef NTSTATUS(NTAPI *pNtReadVirtualMemory)(IN HANDLE ProcessHandle, IN OPTIONAL PVOID BaseAddress, OUT PVOID BufferData, IN SIZE_T BufferLength, OUT OPTIONAL PSIZE_T NumberOfBytesRead);
    typedef NTSTATUS(NTAPI *pNtWriteVirtualMemory)(IN HANDLE ProcessHandle, IN OPTIONAL PVOID BaseAddress, IN CONST VOID *BufferData, IN SIZE_T BufferSize, OUT OPTIONAL PSIZE_T NumberOfBytesWritten);
    typedef NTSTATUS(NTAPI *pNtProtectVirtualMemory)(IN HANDLE ProcessHandle, IN OUT PVOID *BaseAddress, IN OUT PSIZE_T RegionSize, IN ULONG NewProtect, OUT PULONG OldProtect);
    typedef NTSTATUS(NTAPI *pNtQueryVirtualMemory)(IN HANDLE ProcessHandle, IN PVOID BaseAddress, IN MEMORY_INFORMATION_CLASS MemoryInformationClass, OUT PVOID MemoryInformation, IN SIZE_T MemoryInformationLength, OUT OPTIONAL PSIZE_T ReturnLength);

    // Wow64
    typedef NTSTATUS(NTAPI *pNtWow64QueryInformationProcess64)(IN HANDLE ProcessHandle, IN PROCESSINFOCLASS ProcessInformationClass, OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT OPTIONAL PULONG *ReturnLength);
    typedef NTSTATUS(NTAPI *pNtWow64AllocateVirtualMemory64)(IN HANDLE ProcessHandle, IN OUT PVOID64 *BaseAddress, IN ULONG64 ZeroBits, IN OUT PUINT64 RegionSize, IN ULONG AllocationType, IN ULONG Protect);
    typedef NTSTATUS(NTAPI *pNtWow64ReadVirtualMemory64)(IN HANDLE ProcessHandle, IN PVOID64 BaseAddress, OUT PVOID BufferData, IN UINT64 BufferLength, OUT OPTIONAL PUINT64 NumberOfBytesRead);
    typedef NTSTATUS(NTAPI *pNtWow64WriteVirtualMemory64)(IN HANDLE ProcessHandle, IN PVOID64 BaseAddress, IN CONST VOID *BufferData, IN UINT64 BufferSize, OUT OPTIONAL PUINT64 NumberOfBytesWritten);
    typedef NTSTATUS(NTAPI *pNtWow64QueryVirtualMemory64)(IN HANDLE ProcessHandle, IN PVOID64 BaseAddress, IN DWORD MemoryInformationClass, OUT PVOID Buffer, IN UINT64 Length, OUT OPTIONAL PULONG ResultLength);

    // Rtl
    typedef NTSTATUS(NTAPI *pRtlAdjustPrivilege)(IN ULONG Privilege, IN BOOLEAN Enable, IN BOOLEAN CurrentThread, IN PBOOLEAN Enabled);
    typedef VOID(NTAPI *pRtlInitAnsiString)(OUT PANSI_STRING DestinationString, IN OPTIONAL PCSZ SourceString);
    typedef VOID(NTAPI *pRtlInitUnicodeString)(OUT PUNICODE_STRING DestinationString, IN OPTIONAL PCWSTR SourceString);

    // Ldr
    typedef NTSTATUS(NTAPI *pLdrLoadDll)(IN OPTIONAL PWCHAR PathToFile, IN OPTIONAL ULONG Flags, IN PUNICODE_STRING ModuleFileName, OUT HMODULE *ModuleHandle);
    typedef NTSTATUS(NTAPI *pLdrUnloadDll)(IN HANDLE ModuleHandle);
    typedef NTSTATUS(NTAPI *pLdrGetDllHandle)(IN OPTIONAL PWORD pwPath, IN OPTIONAL PVOID Unused, IN PUNICODE_STRING ModuleFileName, OUT HMODULE *pHModule);
    typedef NTSTATUS(NTAPI *pLdrGetProcedureAddress)(IN HMODULE ModuleHandle, IN OPTIONAL PANSI_STRING FunctionName, IN OPTIONAL WORD Oridinal, OUT PVOID *FunctionAddress);

    // Psapi
    typedef BOOL(WINAPI *pGetProcessMemoryInfo)(IN HANDLE Process, IN OUT PVOID ppsmemCounters, IN DWORD cb);
    typedef DWORD(WINAPI *pGetProcessImageFileNameW)(IN HANDLE hProcess, IN OUT LPWSTR lpImageFileName, IN DWORD nSize);

    template <typename T = PSTR>
    struct STRING_T
    {
        USHORT Length;
        USHORT MaximumLength;
        T Buffer;
    };

    template <typename T = PWSTR>
    struct UNICODE_STRING_T
    {
        USHORT Length;
        USHORT MaximumLength;
        T Buffer;
    };

    template <typename T>
    struct LIST_ENTRY_T
    {
        T Flink;
        T Blink;
    };

    template <typename T, int A>
    struct PEB_T
    {
        uint8_t InheritedAddressSpace;
        uint8_t ReadImageFileExecOptions;
        uint8_t BeingDebugged;
        union
        {
            uint8_t BitField;
            struct
            {
                uint8_t ImageUsesLargePages : 1;
                uint8_t IsProtectedProcess : 1;
                uint8_t IsImageDynamicallyRelocated : 1;
                uint8_t SkipPatchingUser32Forwarders : 1;
                uint8_t IsPackagedProcess : 1;
                uint8_t IsAppContainer : 1;
                uint8_t IsProtectedProcessLight : 1;
                uint8_t SpareBits : 1;
            };
        };
        T Mutant;
        T ImageBaseAddress;
        T Ldr;
        T ProcessParameters;
        T SubSystemData;
        T ProcessHeap;
        T FastPebLock;
        T AtlThunkSListPtr;
        T IFEOKey;
        union
        {
            T CrossProcessFlags;
            struct
            {
                uint32_t ProcessInJob : 1;
                uint32_t ProcessInitializing : 1;
                uint32_t ProcessUsingVEH : 1;
                uint32_t ProcessUsingVCH : 1;
                uint32_t ProcessUsingFTH : 1;
                uint32_t ReservedBits0 : 27;
            };
        };
        union
        {
            T KernelCallbackTable;
            T UserSharedInfoPtr;
        };
        uint32_t SystemReserved;
        uint32_t AtlThunkSListPtr32;
        T ApiSetMap;
        union
        {
            uint32_t TlsExpansionCounter;
            T Padding2;
        };
        T TlsBitmap;
        uint32_t TlsBitmapBits[2];
        T ReadOnlySharedMemoryBase;
        T SparePvoid0;
        T ReadOnlyStaticServerData;
        T AnsiCodePageData;
        T OemCodePageData;
        T UnicodeCaseTableData;
        uint32_t NumberOfProcessors;
        uint32_t NtGlobalFlag;
        LARGE_INTEGER CriticalSectionTimeout;
        T HeapSegmentReserve;
        T HeapSegmentCommit;
        T HeapDeCommitTotalFreeThreshold;
        T HeapDeCommitFreeBlockThreshold;
        uint32_t NumberOfHeaps;
        uint32_t MaximumNumberOfHeaps;
        T ProcessHeaps;
        T GdiSharedHandleTable;
        T ProcessStarterHelper;
        union
        {
            uint32_t GdiDCAttributeList;
            T Padding3;
        };
        T LoaderLock;
        uint32_t OSMajorVersion;
        uint32_t OSMinorVersion;
        uint16_t OSBuildNumber;
        uint16_t OSCSDVersion;
        uint32_t OSPlatformId;
        uint32_t ImageSubsystem;
        uint32_t ImageSubsystemMajorVersion;
        union
        {
            uint32_t ImageSubsystemMinorVersion;
            T Padding4;
        };
        T ActiveProcessAffinityMask;
        uint32_t GdiHandleBuffer[A];
        T PostProcessInitRoutine;
        T TlsExpansionBitmap;
        uint32_t TlsExpansionBitmapBits[32];
        union
        {
            uint32_t SessionId;
            T Padding5;
        };
        ULARGE_INTEGER AppCompatFlags;
        ULARGE_INTEGER AppCompatFlagsUser;
        T pShimData;
        T AppCompatInfo;
        UNICODE_STRING_T<T> CSDVersion;
        T ActivationContextData;
        T ProcessAssemblyStorageMap;
        T SystemDefaultActivationContextData;
        T SystemAssemblyStorageMap;
        T MinimumStackCommit;
        T FlsCallback;
        LIST_ENTRY_T<T> FlsListHead;
        T FlsBitmap;
        uint32_t FlsBitmapBits[4];
        uint32_t FlsHighIndex;
        T WerRegistrationData;
        T WerShipAssertPtr;
        T pUnused;
        T pImageHeaderHash;
        union
        {
            uint64_t TracingFlags;
            struct
            {
                uint32_t HeapTracingEnabled : 1;
                uint32_t CritSecTracingEnabled : 1;
                uint32_t LibLoaderTracingEnabled : 1;
                uint32_t SpareTracingBits : 29;
            };
        };
        T CsrServerReadOnlySharedMemoryBase;
    };

    template <typename T>
    struct RTL_DRIVE_LETTER_CURDIR_T
    {
        WORD Flags;
        WORD Length;
        ULONG TimeStamp;
        STRING_T<T> DosPath;
    };

    template <typename T>
    struct CURDIR_T
    {
        UNICODE_STRING_T<T> DosPath;
        T Handle;
    };

    template <typename T>
    struct RTL_USER_PROCESS_PARAMETERS_T
    {
        ULONG MaximumLength;
        ULONG Length;
        ULONG Flags;
        ULONG DebugFlags;
        T ConsoleHandle;
        ULONG ConsoleFlags;
        T StandardInput;
        T StandardOutput;
        T StandardError;
        CURDIR_T<T> CurrentDirectory;
        UNICODE_STRING_T<T> DllPath;
        UNICODE_STRING_T<T> ImagePathName; // 进程完整路径
        UNICODE_STRING_T<T> CommandLine;   // 进程命令行
        T Environment;                     // 环境变量（地址）
        ULONG StartingX;
        ULONG StartingY;
        ULONG CountX;
        ULONG CountY;
        ULONG CountCharsX;
        ULONG CountCharsY;
        ULONG FillAttribute;
        ULONG WindowFlags;
        ULONG ShowWindowFlags;
        UNICODE_STRING_T<T> WindowTitle;
        UNICODE_STRING_T<T> DesktopInfo;
        UNICODE_STRING_T<T> ShellInfo;
        UNICODE_STRING_T<T> RuntimeData;
        RTL_DRIVE_LETTER_CURDIR_T<T> CurrentDirectores[32];
        ULONG EnvironmentSize;
    };

    typedef PEB_T<ULONG32, 34> PEB32;
    typedef PEB_T<ULONG64, 60> PEB64;
    typedef RTL_USER_PROCESS_PARAMETERS_T<ULONG32> RTL_USER_PROCESS_PARAMETERS32;
    typedef RTL_USER_PROCESS_PARAMETERS_T<ULONG64> RTL_USER_PROCESS_PARAMETERS64;

    struct PROCESS_BASIC_INFORMATION64
    {
        NTSTATUS ExitStatus;
        UINT32 Reserved0;
        UINT64 PebBaseAddress;
        UINT64 AffinityMask;
        UINT32 BasePriority;
        UINT32 Reserved1;
        UINT64 UniqueProcessId;
        UINT64 InheritedFromUniqueProcessId;
    };

    struct PROCESS_BASIC_INFORMATION32
    {
        NTSTATUS ExitStatus;
        UINT32 PebBaseAddress;
        UINT32 AffinityMask;
        UINT32 BasePriority;
        UINT32 UniqueProcessId;
        UINT32 InheritedFromUniqueProcessId;
    };

    template <typename T>
    struct PEB_LDR_DATA_T
    {
        ULONG Length;
        BOOLEAN Initialized;
        ULONG SsHandle;
        LIST_ENTRY_T<T> InLoadOrderModuleList;
        LIST_ENTRY_T<T> InMemoryOrderModuleList;
        LIST_ENTRY_T<T> InInitializationOrderModuleList;
        ULONG EntryInProgress;
    };

    template <typename T>
    struct LDR_DATA_TABLE_ENTRY_T
    {
        LIST_ENTRY_T<T> InLoadOrderLinks;
        LIST_ENTRY_T<T> InMemoryOrderModuleList;
        LIST_ENTRY_T<T> InInitializationOrderModuleList;
        T DllBase;
        T EntryPoint;
        ULONG SizeOfImage;
        UNICODE_STRING_T<T> FullDllName;
        UNICODE_STRING_T<T> BaseDllName;
        ULONG Flags;
        USHORT LoadCount;
        USHORT TlsIndex;
        union
        {
            LIST_ENTRY_T<T> HashLinks;
            T SectionPointer;
        };
        ULONG CheckSum;
        union
        {
            ULONG TimeDateStamp;
            T LoadedImports;
        };
        T EntryPointActivationContext;
        T PatchInformation;
    };

    template <typename T = PWSTR>
    struct MEMORY_MAPPED_FILE_NAME_INFORMATION
    {
        UNICODE_STRING_T<T> Name;
        WCHAR Buffer[512];
    };

#if defined(_WIN64)
    typedef PROCESS_BASIC_INFORMATION64 PROCESS_BASIC_INFORMATION;
    typedef PEB_T<ULONG64, 60> PEB;
#else
    typedef PROCESS_BASIC_INFORMATION32 PROCESS_BASIC_INFORMATION;
    typedef PEB_T<ULONG32, 34> PEB;
#endif

} // namespace NtPiv

/**
 * @brief 动态加载NtDll.dll(单例对象)
 */
class PivNT
{
private:
    PivNT() {}
    ~PivNT()
    {
        if (PsapiModule && LdrUnloadDll)
            LdrUnloadDll(PsapiModule);
    }
    PivNT(const PivNT &) = delete;
    PivNT(PivNT &&) = delete;
    PivNT &operator=(const PivNT &) = delete;
    PivNT &operator=(PivNT &&) = delete;

    HMODULE PsapiModule = NULL;
    HMODULE NtdllModule = NULL;
    HMODULE KernelModule = NULL;
    bool _isLoaded = false;

public:
    // Kernel32
    NtPiv::pGetProcessDEPPolicy GetProcessDEPPolicy = nullptr;
    NtPiv::pIsWow64Process IsWow64Process = nullptr;

    // Nt
    NtPiv::pNtOpenProcess NtOpenProcess = nullptr;
    NtPiv::pNtClose NtClose = nullptr;
    NtPiv::pNtCreateThreadEx NtCreateThreadEx = nullptr;
    NtPiv::pNtQueryInformationProcess NtQueryInformationProcess = nullptr;
    NtPiv::pNtAllocateVirtualMemory NtAllocateVirtualMemory = nullptr;
    NtPiv::pNtFreeVirtualMemory NtFreeVirtualMemory = nullptr;
    NtPiv::pNtReadVirtualMemory NtReadVirtualMemory = nullptr;
    NtPiv::pNtWriteVirtualMemory NtWriteVirtualMemory = nullptr;
    NtPiv::pNtProtectVirtualMemory NtProtectVirtualMemory = nullptr;
    NtPiv::pNtQueryVirtualMemory NtQueryVirtualMemory = nullptr;

    // Wow64
    NtPiv::pNtWow64QueryInformationProcess64 NtWow64QueryInformationProcess64 = nullptr;
    NtPiv::pNtWow64AllocateVirtualMemory64 NtWow64AllocateVirtualMemory64 = nullptr;
    NtPiv::pNtWow64ReadVirtualMemory64 NtWow64ReadVirtualMemory64 = nullptr;
    NtPiv::pNtWow64WriteVirtualMemory64 NtWow64WriteVirtualMemory64 = nullptr;
    NtPiv::pNtWow64QueryVirtualMemory64 NtWow64QueryVirtualMemory64 = nullptr;

    // Rtl
    NtPiv::pRtlAdjustPrivilege RtlAdjustPrivilege = nullptr;
    NtPiv::pRtlInitAnsiString RtlInitAnsiString = nullptr;
    NtPiv::pRtlInitUnicodeString RtlInitUnicodeString = nullptr;

    // Ldr
    NtPiv::pLdrLoadDll LdrLoadDll = nullptr;
    NtPiv::pLdrUnloadDll LdrUnloadDll = nullptr;
    NtPiv::pLdrGetDllHandle LdrGetDllHandle = nullptr;
    NtPiv::pLdrGetProcedureAddress LdrGetProcedureAddress = nullptr;

    // Psapi
    NtPiv::pGetProcessMemoryInfo GetProcessMemoryInfo = nullptr;
    NtPiv::pGetProcessImageFileNameW GetProcessImageFileNameW = nullptr;

    static PivNT &data()
    {
        static PivNT Nt;
        if (Nt._isLoaded == false)
        {
            Nt.NtdllModule = ::GetModuleHandleW(L"ntdll.dll");
            Nt.RtlInitAnsiString = reinterpret_cast<NtPiv::pRtlInitAnsiString>(::GetProcAddress(Nt.NtdllModule, "RtlInitAnsiString"));
            Nt.LdrGetProcedureAddress = reinterpret_cast<NtPiv::pLdrGetProcedureAddress>(::GetProcAddress(Nt.NtdllModule, "LdrGetProcedureAddress"));

            if (Nt.NtdllModule && Nt.LdrGetProcedureAddress && Nt.RtlInitAnsiString)
            {
                ANSI_STRING FunctionName;

                Nt.RtlInitAnsiString(&FunctionName, "RtlInitUnicodeString");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.RtlInitUnicodeString));
                Nt.RtlInitAnsiString(&FunctionName, "LdrGetDllHandle");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.LdrGetDllHandle));
                Nt.RtlInitAnsiString(&FunctionName, "LdrLoadDll");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.LdrLoadDll));
                Nt.RtlInitAnsiString(&FunctionName, "LdrUnloadDll");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.LdrUnloadDll));

                // Kernel32
                UNICODE_STRING ModuleFileName;
                Nt.RtlInitUnicodeString(&ModuleFileName, L"Kernel32.dll");
                Nt.LdrGetDllHandle(0, 0, &ModuleFileName, &Nt.KernelModule);
                if (Nt.KernelModule)
                {
                    Nt.RtlInitAnsiString(&FunctionName, "GetProcessDEPPolicy");
                    Nt.LdrGetProcedureAddress(Nt.KernelModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.GetProcessDEPPolicy));
                    Nt.RtlInitAnsiString(&FunctionName, "IsWow64Process");
                    Nt.LdrGetProcedureAddress(Nt.KernelModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.IsWow64Process));
                }

                // Nt
                Nt.RtlInitAnsiString(&FunctionName, "NtOpenProcess");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.NtOpenProcess));
                Nt.RtlInitAnsiString(&FunctionName, "NtClose");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.NtClose));
                Nt.RtlInitAnsiString(&FunctionName, "NtCreateThreadEx");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.NtCreateThreadEx));
                Nt.RtlInitAnsiString(&FunctionName, "NtQueryInformationProcess");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.NtQueryInformationProcess));
                Nt.RtlInitAnsiString(&FunctionName, "NtAllocateVirtualMemory");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.NtAllocateVirtualMemory));
                Nt.RtlInitAnsiString(&FunctionName, "NtFreeVirtualMemory");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.NtFreeVirtualMemory));
                Nt.RtlInitAnsiString(&FunctionName, "NtReadVirtualMemory");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.NtReadVirtualMemory));
                Nt.RtlInitAnsiString(&FunctionName, "NtWriteVirtualMemory");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.NtWriteVirtualMemory));
                Nt.RtlInitAnsiString(&FunctionName, "NtProtectVirtualMemory");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.NtProtectVirtualMemory));
                Nt.RtlInitAnsiString(&FunctionName, "NtQueryVirtualMemory");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.NtQueryVirtualMemory));

#ifdef _WIN64
                // Rtl
                Nt.RtlInitAnsiString(&FunctionName, "RtlAdjustPrivilege");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.RtlAdjustPrivilege));
#else
                Nt.RtlInitAnsiString(&FunctionName, "NtWow64QueryInformationProcess64");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.NtWow64QueryInformationProcess64));
                Nt.RtlInitAnsiString(&FunctionName, "NtWow64AllocateVirtualMemory64");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.NtWow64AllocateVirtualMemory64));
                Nt.RtlInitAnsiString(&FunctionName, "NtWow64ReadVirtualMemory64");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.NtWow64ReadVirtualMemory64));
                Nt.RtlInitAnsiString(&FunctionName, "NtWow64WriteVirtualMemory64");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.NtWow64WriteVirtualMemory64));
                Nt.RtlInitAnsiString(&FunctionName, "NtWow64QueryVirtualMemory64");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.NtWow64QueryVirtualMemory64));
                Nt.RtlInitAnsiString(&FunctionName, "NtWow64QueryInformationProcess64");
                Nt.LdrGetProcedureAddress(Nt.NtdllModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&Nt.NtWow64QueryInformationProcess64));
#endif
                if (Nt.NtOpenProcess && Nt.NtClose && Nt.NtQueryInformationProcess && Nt.NtReadVirtualMemory)
                    Nt._isLoaded = true;
            }
        }
        return Nt;
    }

    inline bool loadPsapi()
    {
        if (GetProcessImageFileNameW && GetProcessMemoryInfo)
            return true;
        if (_isLoaded)
        {
            UNICODE_STRING ModuleFileName;
            ANSI_STRING FunctionName;
            RtlInitUnicodeString(&ModuleFileName, L"psapi.dll");
            if (NT_SUCCESS(LdrLoadDll(NULL, 0, &ModuleFileName, &PsapiModule)))
            {
                RtlInitAnsiString(&FunctionName, "GetProcessImageFileNameW");
                LdrGetProcedureAddress(PsapiModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&GetProcessImageFileNameW));
                RtlInitAnsiString(&FunctionName, "GetProcessMemoryInfo");
                LdrGetProcedureAddress(PsapiModule, &FunctionName, 0, reinterpret_cast<PVOID *>(&GetProcessMemoryInfo));
                if (GetProcessImageFileNameW && GetProcessMemoryInfo)
                    return true;
            }
        }
        return false;
    }

    inline bool isLoaded() const noexcept
    {
        return _isLoaded;
    }
}; // class PivNT

#endif // _PIV_NTDLL_HPP
