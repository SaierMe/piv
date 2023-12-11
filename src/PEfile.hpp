/*********************************************\
 * 火山视窗PIV模块 - PE文件操作              *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef PIV_PEFILE_HPP
#define PIV_PEFILE_HPP

#include "piv_process.hpp"
#include <vector>

class PivFileMapping
{
public:
    PivFileMapping() {}
    ~PivFileMapping()
    {
        Close();
    }

private:
    PVOID m_pvFile = INVALID_HANDLE_VALUE;
    HANDLE hFile = NULL;
    HANDLE hFileMap = NULL;

public:
    BOOL Create(LPCTSTR lpFileName, DWORD dwCreationDisposition, INT64 n64Size, LPCWSTR lpName)
    {
        Close();

        if (dwCreationDisposition != -1)
        {
            hFile = CreateFile(
                lpFileName,
                GENERIC_READ | GENERIC_WRITE, // 读写
                0,
                NULL,
                dwCreationDisposition,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
            if (hFile == INVALID_HANDLE_VALUE)
                return FALSE;
        }

        hFileMap = CreateFileMapping(hFile, NULL, PAGE_READWRITE, (DWORD)(n64Size >> 32), (DWORD)n64Size, lpName);
        return (hFileMap != NULL);
    }

    BOOL Open(LPCTSTR lpName)
    {
        Close();
        hFileMap = OpenFileMapping(FILE_MAP_WRITE, FALSE, lpName);
        return (hFileMap != NULL);
    }

    PVOID Read()
    {
        return m_pvFile;
    }

    BOOL Write(DWORD dwFileOffset, PVOID pvData, SIZE_T stSize)
    {
        if (m_pvFile == NULL)
            return FALSE;
        memcpy((BYTE *)m_pvFile + dwFileOffset, pvData, stSize);
        return TRUE;
    }

    BOOL Save(SIZE_T stSize)
    {
        if (m_pvFile == NULL)
            return FALSE;
        return FlushViewOfFile(m_pvFile, stSize);
    }

    BOOL MapToMemory(INT64 n64FileOffset, SIZE_T stSize)
    {
        if (hFileMap == NULL)
            return FALSE;
        if (NULL != m_pvFile)
            UnmapViewOfFile(m_pvFile); // 取消映射
        m_pvFile = MapViewOfFile(hFileMap, (FILE_MAP_WRITE | FILE_MAP_READ), (DWORD)(n64FileOffset >> 32), (DWORD)n64FileOffset, stSize);
        return (m_pvFile != NULL);
    }

    BOOL UnMapToMemory()
    {
        return (m_pvFile == NULL ? FALSE : UnmapViewOfFile(m_pvFile));
    }

    VOID Close()
    {
        if (NULL != m_pvFile)
        {
            UnmapViewOfFile(m_pvFile); // 取消映射
            m_pvFile = NULL;
        }
        if (NULL != hFileMap)
        {
            CloseHandle(hFileMap); // 关闭文件映射
            hFileMap = NULL;
        }
        if (NULL != hFile && INVALID_HANDLE_VALUE != hFile) // 关闭文件
        {
            CloseHandle(hFile);
            hFile = INVALID_HANDLE_VALUE;
        }
    }

}; // PivFileMapping

/**
 * @brief 自写的PE文件操作类
 */
class PivPeFile
{
protected:
    struct PEFILE_INFO
    {
        void *pPeBase = nullptr;             // PE数据的基址
        HANDLE hFile = INVALID_HANDLE_VALUE; // 文件句柄
        HANDLE hFileMap = nullptr;           // 内存映射文件句柄
        PIMAGE_DOS_HEADER pDosHeader = nullptr;
        PIMAGE_NT_HEADERS32 pNtHeader = nullptr;
        PIMAGE_SECTION_HEADER pSection = nullptr;
        PIMAGE_EXPORT_DIRECTORY pExport = nullptr;
        PIMAGE_IMPORT_DESCRIPTOR pImport = nullptr;
        size_t cbPeSize = 0;       // PE数据的当前尺寸
        DWORD dwSectionCount = 0;  // 节区数量
        bool bIs64Pe = false;      // 是否64位PE
        bool bIsMemAlign = false;  // 是否内存对齐
        bool bIsLoaded = false;    // 是否已加载运行
        bool bIsWriteable = false; // PE是否可写

        PEFILE_INFO() {}
        ~PEFILE_INFO()
        {
            if (pPeBase)
            {
                if (bIsWriteable)
                    NtPiv::NtFreeVirtualMemory(NULL, &pPeBase, NULL, MEM_RELEASE);
                else
                    ::UnmapViewOfFile(pPeBase);
            }
            if (hFileMap)
                ::CloseHandle(hFileMap);
            if (hFile != INVALID_HANDLE_VALUE)
                ::CloseHandle(hFile);
        }
    };
    const WCHAR *szErrorsMessage[31]{
        L"错误(0): 操作成功完成。",
        L"错误(1): 打开文件失败，可能是目标文件被其他程序占用。",
        L"错误(2): 创建内存映射文件失败。",
        L"错误(3): 将文件映射到内存失败。",
        L"错误(4): DOS头签名不符，目标不是PE文件。",
        L"错误(5): NT头签名不符，目标不是PE文件。",
        L"错误(6): NT文件头和可选头的程序位数不符，目标不是有效的PE文件。",
        L"错误(7): NT头解析错误，目标不是PE文件。",
        L"错误(8): 进程和PE文件的程序位数不一致，不能继续操作。",
        L"错误(9): 当前未打开PE文件，不能进行其他操作。",
        L"错误(10): PE文件的内存基址无效，这是程序设计错误。",
        L"错误(11): 不是有效的DLL文件。",
        L"错误(12): 不是有效的可执行映像文件。",
        L"错误(13): 索引超出范围。",
        L"错误(14): 导出表头位置无效。",
        L"错误(15): 导出表为空。",
        L"错误(16): 导入表头位置无效。",
        L"错误(17): 导入表为空。",
        L"错误(18): 无法计算出文件的内存映像尺寸。",
        L"错误(19): 分配虚拟内存失败。",
        L"错误(20): 重建导入表时无法加载模块。",
        L"错误(21): 重建导入表时无法获取函数地址。",
        L"错误(22): 之前已经加载了内存DLL，不再重复加载。",
        L"错误(23): 内存DLL执行入口点函数失败。",
        L"错误(24): 内存DLL未加载，不能进行此操作。",
        L"错误(25): 函数序号无效。",
        L"错误(26): 函数地址无效。",
        L"错误(27): 该PE文件无需修复。",
        L"错误(28): 无法保存文件。",
        L"错误(29): 模块的基址无效，可能是模块已被加壳，需要找到原始入口点(OEP)。"
        L"错误(30): 不是有效的EXE文件。",
    };

    // 成员变量
    std::unique_ptr<PEFILE_INFO> m_PE;
    std::vector<HMODULE> m_vecHmodule;
    INT m_Error = 0;

public:
    PivPeFile()
    {
        NtPiv::LoadNt();
    }
    ~PivPeFile() {}

    // 以下函数任何时候皆可调用

    /**
     * @brief 取最后错误代码
     * @return
     */
    INT GetErrorCode() const
    {
        return m_Error;
    }

    /**
     * @brief 取最后错误信息
     * @return
     */
    const WCHAR *GetErrorMessage() const
    {
        return szErrorsMessage[m_Error];
    }

    /**
     * @brief 打开PE文件
     * @param szFileName
     * @return
     */
    BOOL OpenPeFile(const WCHAR *szFileName)
    {
        m_PE.reset(new PEFILE_INFO);
        m_PE->hFile = ::CreateFileW((LPCWSTR)szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (m_PE->hFile == INVALID_HANDLE_VALUE)
        {
            m_Error = 1;
            goto _PIVPE_FAIL_;
        }
        LARGE_INTEGER li;
        if (::GetFileSizeEx(m_PE->hFile, &li))
            m_PE->cbPeSize = static_cast<size_t>(li.QuadPart);
        m_PE->hFileMap = ::CreateFileMappingW(m_PE->hFile, NULL, PAGE_READONLY, 0, 0, NULL);
        if (m_PE->hFileMap == nullptr)
        {
            m_Error = 2;
            goto _PIVPE_FAIL_;
        }
        m_PE->pPeBase = ::MapViewOfFile(m_PE->hFileMap, FILE_MAP_READ, 0, 0, 0);
        if (m_PE->pPeBase == nullptr)
        {
            m_Error = 3;
            goto _PIVPE_FAIL_;
        }
        m_PE->bIsMemAlign = false;
        if (CheckHeaders())
        {
            m_PE->bIsMemAlign = false;
            m_PE->bIsLoaded = false;
            return TRUE;
        }
    _PIVPE_FAIL_:
        m_PE.reset(nullptr);
        return FALSE;
    }

    /**
     * @brief 打开内存PE
     * @param lpPeAddress
     * @param dwPeDataSize
     * @param bIsMemAlign
     * @param bIsLoaded
     * @return
     */
    BOOL OpenPeData(void *lpPeAddress, size_t dwPeDataSize, bool bIsMemAlign = false, bool bIsLoaded = false)
    {
        if (lpPeAddress == (void *)0x400000 || lpPeAddress == (void *)0x140000000 || lpPeAddress == (void *)0x10000000 || lpPeAddress == (void *)0x180000000)
        { // 如果模块基址被隐藏了,说明PE文件已经被加壳
            m_Error = 29;
            return FALSE;
        }
        m_PE.reset(new PEFILE_INFO);
        m_PE->pPeBase = lpPeAddress;
        m_PE->cbPeSize = dwPeDataSize;
        m_PE->bIsMemAlign = bIsMemAlign;
        m_PE->bIsLoaded = bIsLoaded;
        if (CheckHeaders())
            return TRUE;
        m_PE.reset(nullptr);
        return FALSE;
    }

    /**
     * @brief 关闭PE数据
     */
    void ClosePeData()
    {
        m_PE.reset(nullptr);
    }

    /**
     * @brief 将当前的PE数据原样复制到新内存
     * @param bCloseSrc
     * @return
     */
    VOID *CopyToNewMem(BOOL bCloseSrc)
    {
        if (!m_PE)
        {
            m_Error = 9;
            return 0;
        }
        VOID *pMemAddress = CopyToMem();
        if (pMemAddress == nullptr)
        {
            return 0;
        }
        if (bCloseSrc)
        {
            size_t dwPeSize = m_PE->cbPeSize;
            bool bIsMemAlign = m_PE->bIsMemAlign;
            bool bIsLoaded = m_PE->bIsLoaded;
            m_PE.reset(new PEFILE_INFO);
            m_PE->pPeBase = pMemAddress;
            m_PE->cbPeSize = dwPeSize;
            m_PE->bIsMemAlign = bIsMemAlign;
            m_PE->bIsLoaded = bIsLoaded;
            if (!CheckHeaders())
            {
                m_PE.reset(nullptr);
                return 0;
            }
        }
        m_Error = 0;
        return pMemAddress;
    }

    /**
     * @brief 保存数据
     * @param szFileName
     * @param bRepairPe
     * @return
     */
    BOOL SavePeDatas(const WCHAR *szFileName, BOOL bRepairPe)
    {
        void *pMemoryAddress = CopyToNewMem(FALSE);
        if (pMemoryAddress == nullptr)
        {
            return FALSE;
        }
        size_t dwWriteSize;
        if (bRepairPe)
        {
            if (RepairToPeFile(pMemoryAddress) == FALSE)
            {
                NtPiv::NtFreeVirtualMemory(NULL, &pMemoryAddress, NULL, MEM_RELEASE);
                return FALSE;
            }
            dwWriteSize = CalcPeFileSize();
        }
        else
        {
            dwWriteSize = m_PE->cbPeSize;
        }
        bool bRes = false;
        FILE *out = _wfopen(szFileName, L"wb");
        if (out != NULL)
        {
            bRes = (fwrite(pMemoryAddress, 1, dwWriteSize, out) == dwWriteSize);
            fclose(out);
        }
        if (pMemoryAddress)
        {
            NtPiv::NtFreeVirtualMemory(NULL, &pMemoryAddress, NULL, MEM_RELEASE);
        }
        if (bRes)
        {
            m_Error = 0;
            return TRUE;
        }
        else
        {
            m_Error = 28;
            return FALSE;
        }
    }

    // 以下函数打开PE成功后可用

    /**
     * @brief 取PE数据指针
     * @return
     */
    const void *GetBaseAddress() const
    {
        return m_PE ? m_PE->pPeBase : 0;
    }

    /**
     * @brief 计算映像文件内存对齐后的尺寸
     * @return
     */
    DWORD CalcTotalImageSize()
    {
        if (m_PE == nullptr)
        {
            m_Error = 9;
            return 0;
        }
        if (m_PE->pPeBase == nullptr)
        {
            m_Error = 10;
            return 0;
        }
        DWORD dwImageSize = 0;
        DWORD dwAlignment = m_PE->bIs64Pe ? reinterpret_cast<PIMAGE_NT_HEADERS64>(m_PE->pNtHeader)->OptionalHeader.SectionAlignment
                                          : m_PE->pNtHeader->OptionalHeader.SectionAlignment;
        dwImageSize = (m_PE->bIs64Pe ? reinterpret_cast<PIMAGE_NT_HEADERS64>(m_PE->pNtHeader)->OptionalHeader.SizeOfHeaders
                                     : m_PE->pNtHeader->OptionalHeader.SizeOfHeaders + dwAlignment - 1) /
                      dwAlignment * dwAlignment;
        for (DWORD i = 0; i < m_PE->dwSectionCount; i++)
        {
            DWORD dwCodeSize = m_PE->pSection[i].Misc.VirtualSize;
            DWORD dwLoadSize = m_PE->pSection[i].SizeOfRawData;
            DWORD dwMaxSize = (dwLoadSize > dwCodeSize) ? (dwLoadSize) : (dwCodeSize);
            DWORD dwSectionSize = (m_PE->pSection[i].VirtualAddress + dwMaxSize + dwAlignment - 1) / dwAlignment * dwAlignment;
            if (dwImageSize < dwSectionSize)
                dwImageSize = dwSectionSize;
        }
        return dwImageSize;
    }

    /**
     * @brief 计算PE文件的大小
     * @return
     */
    DWORD CalcPeFileSize()
    {
        if (m_PE == nullptr)
        {
            m_Error = 9;
            return 0;
        }
        m_Error = 0;
        return m_PE->pSection[m_PE->dwSectionCount - 1].SizeOfRawData + m_PE->pSection[m_PE->dwSectionCount - 1].PointerToRawData;
    }

    /**
     * @brief 取PE数据的当前尺寸
     * @return
     */
    size_t GetDataSize() const
    {
        return m_PE ? m_PE->cbPeSize : 0;
    }

    /**
     * @brief 取入口点
     * @return
     */
    DWORD GetEntryPoint()
    {
        if (m_PE)
            return m_PE->bIs64Pe ? reinterpret_cast<PIMAGE_NT_HEADERS64>(m_PE->pNtHeader)->OptionalHeader.AddressOfEntryPoint
                                 : m_PE->pNtHeader->OptionalHeader.AddressOfEntryPoint;
        m_Error = 9;
        return 0;
    }

    /**
     * @brief 校验PE是否为DLL文件
     * @return
     */
    BOOL IsDllData()
    {
        if (m_PE == nullptr)
        {
            m_Error = 9;
            return FALSE;
        }
        if ((m_PE->pNtHeader->FileHeader.Characteristics & IMAGE_FILE_DLL) == 0)
        {
            m_Error = 11;
            return FALSE;
        }
        if ((m_PE->pNtHeader->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) == 0)
        {
            m_Error = 12;
            return FALSE;
        }
        m_Error = 0;
        return TRUE;
    }

    /**
     * @brief 校验PE是否为EXE文件
     * @return
     */
    BOOL IsExeData()
    {
        if (m_PE == nullptr)
        {
            m_Error = 9;
            return FALSE;
        }
        if ((m_PE->pNtHeader->FileHeader.Characteristics & IMAGE_FILE_DLL) == IMAGE_FILE_DLL)
        {
            m_Error = 30;
            return FALSE;
        }
        if ((m_PE->pNtHeader->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) == 0)
        {
            m_Error = 12;
            return FALSE;
        }
        m_Error = 0;
        return TRUE;
    }

    /**
     * @brief 返回当前的PE数据是否可写
     * @return
     */
    BOOL IsWriteable() const
    {
        return m_PE ? m_PE->bIsWriteable : FALSE;
    }

    /**
     * @brief 相对虚拟地址到文件偏移
     * @param dwRVA
     * @return
     */
    DWORD Rva2Foa(DWORD dwRVA)
    {
        if (m_PE == nullptr)
        {
            m_Error = 9;
            return FALSE;
        }
        if (m_PE->pPeBase == nullptr)
        {
            m_Error = 10;
            return FALSE;
        }
        DWORD dwDiffer = 0;
        DWORD dwAlignment = m_PE->bIs64Pe ? reinterpret_cast<PIMAGE_NT_HEADERS64>(m_PE->pNtHeader)->OptionalHeader.SectionAlignment
                                          : m_PE->pNtHeader->OptionalHeader.SectionAlignment;
        for (DWORD i = 0; i < m_PE->dwSectionCount; i++)
        {
            DWORD dwBlockCount = m_PE->pSection[i].SizeOfRawData / dwAlignment;
            dwBlockCount += m_PE->pSection[i].SizeOfRawData % dwAlignment ? 1 : 0;
            DWORD dwBeginVA = m_PE->pSection[i].VirtualAddress;
            DWORD dwEndVA = m_PE->pSection[i].VirtualAddress + dwBlockCount * dwAlignment;
            if (dwRVA >= dwBeginVA && dwRVA < dwEndVA)
            {
                dwDiffer = dwRVA - dwBeginVA;
                return m_PE->pSection[i].PointerToRawData + dwDiffer;
            }
            else if (dwRVA < dwBeginVA)
            {
                return dwRVA;
            }
        }
        return 0;
    }

    /**
     * @brief 文件偏移到虚拟地址
     * @param dwFoa
     * @return
     */
    DWORD Foa2Rva(DWORD dwFoa)
    {
        if (m_PE == nullptr)
        {
            m_Error = 9;
            return FALSE;
        }
        if (m_PE->pPeBase == nullptr)
        {
            m_Error = 10;
            return FALSE;
        }
        DWORD dwVPK = 0;
        for (DWORD i = 0; i < m_PE->dwSectionCount; i++)
        {
            DWORD dwBeginVA = m_PE->pSection[i].PointerToRawData;
            DWORD dwEndVA = m_PE->pSection[i].PointerToRawData + m_PE->pSection[i].SizeOfRawData;
            // 如果dwFoa在某个区段中
            if (dwFoa >= dwBeginVA && dwFoa < dwEndVA)
            {
                dwVPK = m_PE->pSection[i].VirtualAddress - dwBeginVA;
                return dwFoa + dwVPK;
            }
            else if (dwFoa < dwBeginVA) // 在文件头中直接返回
            {
                return dwFoa;
            }
        }
        return 0;
    }

    /**
     * @brief 加载内存EXE
     * @return
     */
    BOOL MemLoadPE()
    {
        if (m_PE == nullptr)
        {
            m_Error = 9;
            return FALSE;
        }
        if (m_PE->bIsLoaded == TRUE)
        {
            m_Error = 22;
            return TRUE;
        }
        // 验证DLL和程序的位数是否一致
#ifdef _WIN64
        if (m_PE->bIs64Pe == FALSE)
        {
            m_Error = 8;
            return FALSE;
        }
#else
        if (m_PE->bIs64Pe == TRUE)
        {
            m_Error = 8;
            return FALSE;
        }
#endif
        if ((m_PE->pNtHeader->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) == 0)
        {
            m_Error = 12;
            return FALSE;
        }
        if (m_PE->bIsWriteable == FALSE)
        {
            if (CopyToMem() == 0)
            {
                return FALSE;
            }
        }
        if (m_PE->bIsMemAlign == FALSE)
        {
            if (AlignPeDatas() == FALSE)
            {
                return FALSE;
            }
        }
        if (m_PE->bIsLoaded == FALSE)
        {
            if (GetDataDirectoryRva(IMAGE_DIRECTORY_ENTRY_BASERELOC) > 0 && GetDataDirectorySize(IMAGE_DIRECTORY_ENTRY_BASERELOC) > 0)
            {
                RebuildRelocation();
            }
            m_vecHmodule.clear();
            if (RebuildImport(&m_vecHmodule) == FALSE)
            {
                return FALSE;
            }
        }
        m_PE->bIsLoaded = TRUE;
        m_Error = 0;
        return TRUE;
    }

    // 以下函数用于IMAGE_DOS_SIGNATURE

    /**
     * @brief 获取DOS头
     * @return
     */
    const PIMAGE_DOS_HEADER GetDosHeader() const
    {
        return m_PE ? m_PE->pDosHeader : nullptr;
    }

    /**
     * @brief 获取DOS的入口地址
     * @return
     */
    DWORD GetDosEntryPoint() const
    {
        return m_PE ? static_cast<DWORD>(m_PE->pDosHeader->e_cs + m_PE->pDosHeader->e_cparhdr) * 0x10UL + m_PE->pDosHeader->e_ip : 0;
    }

    // 以下函数用于IMAGE_NT_SIGNATURE

    /**
     * @brief 获取NT头,如果是64位PE,实际返回的是PIMAGE_NT_HEADERS64
     * @return
     */
    const PIMAGE_NT_HEADERS32 GetNtHeader() const
    {
        return m_PE ? m_PE->pNtHeader : nullptr;
    }

    /**
     * @brief 返回PE数据是否为64位
     * @return
     */
    BOOL Is64Pe() const
    {
        return m_PE ? m_PE->bIs64Pe : FALSE;
    }

    /**
     * @brief 获取映像基址
     * @return
     */
    ULONGLONG GetImageBase()
    {
        if (m_PE)
            return m_PE->bIs64Pe ? reinterpret_cast<PIMAGE_NT_HEADERS64>(m_PE->pNtHeader)->OptionalHeader.ImageBase : m_PE->pNtHeader->OptionalHeader.ImageBase;
        m_Error = 9;
        return 0;
    }

    /**
     * @brief 获取节区表头数组
     * @param dwSectionCount
     * @return
     */
    const PIMAGE_SECTION_HEADER GetSectionHeader(PDWORD dwSectionCount)
    {
        if (m_PE)
        {
            *dwSectionCount = m_PE->dwSectionCount;
            m_Error = 0;
            return m_PE->pSection;
        }
        *dwSectionCount = 0;
        m_Error = 9;
        return nullptr;
    }

    /**
     * @brief 获取导出表头
     * @return
     */
    const PIMAGE_EXPORT_DIRECTORY GetExportHeader() const
    {
        return m_PE ? m_PE->pExport : nullptr;
    }

    /**
     * @brief 获取导入表描述符
     * @return
     */
    const PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor() const
    {
        return m_PE ? m_PE->pImport : nullptr;
    }

    /**
     * @brief 获取各DataDirectory入口的RVA
     * @param dwIndex
     * @return
     */
    DWORD GetDataDirectoryRva(DWORD dwIndex)
    {
        if (m_PE == nullptr)
        {
            m_Error = 9;
            return 0;
        }
        PIMAGE_DATA_DIRECTORY pIDD = m_PE->bIs64Pe ? reinterpret_cast<IMAGE_NT_HEADERS64 *>(m_PE->pNtHeader)->OptionalHeader.DataDirectory
                                                   : m_PE->pNtHeader->OptionalHeader.DataDirectory;
        if (dwIndex >= 0 && dwIndex <= 15)
        {
            m_Error = 0;
            return pIDD ? pIDD[dwIndex].VirtualAddress : 0;
        }
        else
        {
            m_Error = 13;
            return 0;
        }
    }

    /**
     * @brief 获取各DataDirectory的大小
     * @param dwIndex
     * @return
     */
    DWORD GetDataDirectorySize(DWORD dwIndex)
    {
        if (m_PE == nullptr)
        {
            m_Error = 9;
            return 0;
        }
        PIMAGE_DATA_DIRECTORY pIDD = m_PE->bIs64Pe ? reinterpret_cast<IMAGE_NT_HEADERS64 *>(m_PE->pNtHeader)->OptionalHeader.DataDirectory
                                                   : m_PE->pNtHeader->OptionalHeader.DataDirectory;
        if (dwIndex >= 0 && dwIndex <= 15)
        {
            m_Error = 0;
            return pIDD ? pIDD[dwIndex].Size : 0;
        }
        else
        {
            m_Error = 13;
            return 0;
        }
    }

    // 以下函数与各种表有关

    /**
     * @brief 1. 获取导出表信息
     * @param szModuleName
     * @param vecFunOrdinal
     * @param vecFunAddress
     * @param vecFunName
     * @return
     */
    DWORD GetExportInfo(CVolString *szModuleName, std::vector<DWORD> *vecFunOrdinal, std::vector<DWORD> *vecFunAddress, std::vector<CVolString> *vecFunName)
    {
        if (m_PE == nullptr)
        {
            m_Error = 9;
            return 0;
        }
        if (m_PE->pExport == nullptr)
        {
            m_Error = 14;
            return 0;
        }
        if (m_PE->pExport->NumberOfFunctions == 0)
        {
            m_Error = 15;
            return 0;
        }
        szModuleName->SetText(GetWideText(PIV_PTR_FORWARD(const CHAR *, m_PE->pPeBase, GetRealAddress(m_PE->pExport->Name)), CVolMem(), NULL));
        DWORD *pFunctions = PIV_PTR_FORWARD(DWORD *, m_PE->pPeBase, GetRealAddress(m_PE->pExport->AddressOfFunctions));
        DWORD *pNames = PIV_PTR_FORWARD(DWORD *, m_PE->pPeBase, GetRealAddress(m_PE->pExport->AddressOfNames));
        WORD *pOrdinals = PIV_PTR_FORWARD(WORD *, m_PE->pPeBase, GetRealAddress(m_PE->pExport->AddressOfNameOrdinals));
        for (DWORD i = 0; i < m_PE->pExport->NumberOfFunctions; i++)
        {
            vecFunAddress->push_back(pFunctions[i]);
            vecFunOrdinal->push_back(i + m_PE->pExport->Base);
            vecFunName->push_back(CVolString(L""));
        }
        for (DWORD i = 0; i < m_PE->pExport->NumberOfNames; i++)
        {
            vecFunName->at(i).SetText(GetWideText(PIV_PTR_FORWARD(const CHAR *, m_PE->pPeBase, GetRealAddress(pNames[i])), CVolMem(), NULL));
        }
        m_Error = 0;
        return m_PE->pExport->NumberOfFunctions;
    }

    /**
     * @brief 2. 获取导入表信息
     * @param vecModuleName
     * @param vecFunOrdinal
     * @param vecFunAddress
     * @param vecFunName
     * @return
     */
    DWORD GetImportInfo(std::vector<CVolString> *vecModuleName, std::vector<std::vector<DWORD>> *vecFunOrdinal,
                        std::vector<std::vector<ULONGLONG>> *vecFunAddress, std::vector<std::vector<CVolString>> *vecFunName)
    {
        if (m_PE == nullptr)
        {
            m_Error = 9;
            return 0;
        }
        PIMAGE_IMPORT_DESCRIPTOR pImport = m_PE->pImport;
        DWORD dwThunkSize = GetThunkSize();
        if (dwThunkSize == 0)
        {
            m_Error = 17;
            return 0;
        }
        DWORD dwDiff = dwThunkSize == 4 ? 1 : 2;
        while (pImport->Name) // 历遍导入表
        {
            vecModuleName->push_back(CVolString(GetWideText(PIV_PTR_FORWARD(const CHAR *, m_PE->pPeBase, GetRealAddress(pImport->Name)), CVolMem(), NULL)));
            PDWORD dwINT_Thunk = PIV_PTR_FORWARD(PDWORD, m_PE->pPeBase, GetRealAddress(pImport->OriginalFirstThunk));
            PDWORD dwIAT_Thunk = PIV_PTR_FORWARD(PDWORD, m_PE->pPeBase, GetRealAddress(pImport->FirstThunk));
            PDWORD dwThunk = pImport->OriginalFirstThunk ? dwINT_Thunk : dwIAT_Thunk;
            std::vector<DWORD> vecOrdinal;
            std::vector<ULONGLONG> vecAddress;
            std::vector<CVolString> vecName;
            while (*dwThunk) // 历遍模块的导入函数
            {
                PIMAGE_THUNK_DATA32 pINT_Thunk = (PIMAGE_THUNK_DATA32)dwINT_Thunk;
                PIMAGE_THUNK_DATA32 pIAT_Thunk = (PIMAGE_THUNK_DATA32)dwIAT_Thunk;
                ULONGLONG llAddress = 0;
                DWORD dwOrdinal = 0;
                CVolString szName;
                PIMAGE_IMPORT_BY_NAME pByName = nullptr;
                if (pImport->TimeDateStamp) // 如果已经绑定了导入表,FirstThunk指向的地址被替换为真实的函数地址(VA)
                    llAddress = m_PE->bIs64Pe ? reinterpret_cast<PIMAGE_THUNK_DATA64>(pIAT_Thunk)->u1.Function : pIAT_Thunk->u1.Function;
                if (m_PE->bIsLoaded == FALSE) // PE未运行时,FirstThunk指向函数序号和名称
                {
                    if (dwDiff == 2)
                    {
                        if (IMAGE_SNAP_BY_ORDINAL64(reinterpret_cast<PIMAGE_THUNK_DATA64>(pIAT_Thunk)->u1.Ordinal))
                            dwOrdinal = IMAGE_ORDINAL64(reinterpret_cast<PIMAGE_THUNK_DATA64>(pIAT_Thunk)->u1.Ordinal);
                        else
                            pByName = PIV_PTR_FORWARD(PIMAGE_IMPORT_BY_NAME, m_PE->pPeBase, GetRealAddress(static_cast<DWORD>(pIAT_Thunk->u1.AddressOfData)));
                    }
                    else
                    {
                        if (IMAGE_SNAP_BY_ORDINAL32(pIAT_Thunk->u1.Ordinal))
                            dwOrdinal = IMAGE_ORDINAL32(pIAT_Thunk->u1.Ordinal);
                        else
                            pByName = PIV_PTR_FORWARD(PIMAGE_IMPORT_BY_NAME, m_PE->pPeBase, GetRealAddress(static_cast<DWORD>(pIAT_Thunk->u1.AddressOfData)));
                    }
                    if (pByName)
                    {
                        dwOrdinal = pByName->Hint;
                        szName.SetText(GetWideText(pByName->Name, CVolMem(), NULL));
                    }
                }
                else // PE运行时,FirstThunk指向的地址被替换为真实的函数地址(VA)
                {
                    llAddress = m_PE->bIs64Pe ? reinterpret_cast<PIMAGE_THUNK_DATA64>(pIAT_Thunk)->u1.Function : pIAT_Thunk->u1.Function;
                    if (pImport->OriginalFirstThunk) // 如果原始桥有效,取函数序号和名称
                    {
                        if (dwDiff == 2)
                        {
                            if (IMAGE_SNAP_BY_ORDINAL64(reinterpret_cast<PIMAGE_THUNK_DATA64>(pINT_Thunk)->u1.Ordinal))
                                dwOrdinal = IMAGE_ORDINAL64(reinterpret_cast<PIMAGE_THUNK_DATA64>(pINT_Thunk)->u1.Ordinal);
                            else
                                pByName = PIV_PTR_FORWARD(PIMAGE_IMPORT_BY_NAME, m_PE->pPeBase, GetRealAddress(static_cast<DWORD>(pINT_Thunk->u1.AddressOfData)));
                        }
                        else
                        {
                            if (IMAGE_SNAP_BY_ORDINAL32(pINT_Thunk->u1.Ordinal))
                                dwOrdinal = IMAGE_ORDINAL32(pINT_Thunk->u1.Ordinal);
                            else
                                pByName = PIV_PTR_FORWARD(PIMAGE_IMPORT_BY_NAME, m_PE->pPeBase, GetRealAddress(static_cast<DWORD>(pINT_Thunk->u1.AddressOfData)));
                        }
                        if (pByName)
                        {
                            dwOrdinal = pByName->Hint;
                            szName.SetText(GetWideText(pByName->Name, CVolMem(), NULL));
                        }
                    }
                }
                vecOrdinal.push_back(dwOrdinal);
                vecAddress.push_back(llAddress);
                vecName.push_back(szName);
                dwINT_Thunk = dwINT_Thunk + dwDiff;
                dwIAT_Thunk = dwIAT_Thunk + dwDiff;
                dwThunk = dwThunk + dwDiff;
            }
            vecFunOrdinal->push_back(vecOrdinal);
            vecFunAddress->push_back(vecAddress);
            vecFunName->push_back(vecName);
            pImport++;
        }
        m_Error = 0;
        return (DWORD)vecModuleName->size();
    }

    // 3. 获取异常表

    // 以下函数与加载运行PE有关
protected:
    /**
     * @brief 将当前的PE数据原样复制到新内存,申请的内存大小为PE的映像尺寸
     * @return
     */
    VOID *CopyToMem()
    {
        SIZE_T dwImageSize = CalcTotalImageSize();
        if (dwImageSize == 0)
        {
            m_Error = 18;
            return 0;
        }
        void *pMemoryAddress = nullptr;
        NtPiv::NtAllocateVirtualMemory(NULL, &pMemoryAddress, 0, &dwImageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (pMemoryAddress == nullptr)
        {
            m_Error = 19;
            return 0;
        }
        memcpy(pMemoryAddress, m_PE->pPeBase, m_PE->cbPeSize);
        m_PE->bIsWriteable = TRUE;
        m_Error = 0;
        return pMemoryAddress;
    }

    /**
     * @brief 将PE的数据复制到内存,并对齐所有节
     * @return
     */
    BOOL AlignPeDatas()
    {
        SIZE_T dwImageSize = CalcTotalImageSize();
        if (dwImageSize == 0)
        {
            m_Error = 18;
            return FALSE;
        }
        void *pMemoryAddress = nullptr;
        NtPiv::NtAllocateVirtualMemory(NULL, &pMemoryAddress, 0, &dwImageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (pMemoryAddress == nullptr)
        {
            m_Error = 19;
            return FALSE;
        }
        DWORD cbHeaders = m_PE->bIs64Pe ? reinterpret_cast<PIMAGE_NT_HEADERS64>(m_PE->pNtHeader)->OptionalHeader.SizeOfHeaders
                                        : m_PE->pNtHeader->OptionalHeader.SizeOfHeaders;
        memcpy(pMemoryAddress, m_PE->pPeBase, cbHeaders);
        for (DWORD i = 0; i < m_PE->dwSectionCount; ++i)
        {
            if (m_PE->pSection[i].VirtualAddress == 0 || m_PE->pSection[i].SizeOfRawData == 0)
                continue;
            void *pSectionAddress = PIV_PTR_FORWARD(void *, pMemoryAddress, m_PE->pSection[i].VirtualAddress);
            memcpy(pSectionAddress,
                   PIV_PTR_FORWARD(void *, m_PE->pPeBase, m_PE->pSection[i].PointerToRawData), m_PE->pSection[i].SizeOfRawData);
        }
        // 重定位基址和获取各种PE头的地址
        m_PE->bIsMemAlign = TRUE;
        m_PE->pPeBase = pMemoryAddress;
        m_PE->cbPeSize = dwImageSize;
        m_PE->bIsWriteable = TRUE;
        return (CheckHeaders());
    }

    /**
     * @brief 计算IMAGE_THUNK_DATA结构体的尺寸,导入表为空时返回0.
     * @return
     */
    DWORD GetThunkSize()
    {
        if (m_PE->bIs64Pe)
        {
            return 8;
        } // 64位PE肯定是8位长度.
        DWORD dwSize = 0;
        // 因为可能存在单桥结构,所以只读取FirstThunk
        if ((m_PE->pImport + 1)->Name) // 导入表有大于1个模块
        {
            PDWORD pSecondThunk = PIV_PTR_FORWARD(PDWORD, m_PE->pPeBase, GetRealAddress((m_PE->pImport + 1)->FirstThunk));
            dwSize = *(pSecondThunk - 2) ? 4 : 8;
        }
        else // 导入表只有1个模块
        {
            PDWORD pSecondDword = PIV_PTR_FORWARD(PDWORD, m_PE->pPeBase, GetRealAddress(m_PE->pImport->FirstThunk));
            dwSize = *pSecondDword ? 4 : 8;
        }
        m_Error = 0;
        return dwSize;
    }

    /**
     * @brief 重建重定位表
     * @return
     */
    BOOL RebuildRelocation()
    {
        PIMAGE_BASE_RELOCATION pRelocation = PIV_PTR_FORWARD(PIMAGE_BASE_RELOCATION,
                                                            m_PE->pPeBase, GetDataDirectoryRva(IMAGE_DIRECTORY_ENTRY_BASERELOC));
        while ((pRelocation->VirtualAddress + pRelocation->SizeOfBlock) != 0)
        {
            WORD *pLocData = PIV_PTR_FORWARD(WORD *, pRelocation, sizeof(IMAGE_BASE_RELOCATION));
            DWORD nNumberOfReloc = (pRelocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
            for (DWORD i = 0; i < nNumberOfReloc; i++)
            {
                // 每个WORD由两部分组成,高4位指出了重定位的类型,WINNT.H中的一系列IMAGE_REL_BASED_xxx定义了重定位类型的取值
                // 低12位是相对于VirtualAddress域的偏移,指出了必须进行重定位的位置
                if (pLocData[i] >> 12 == IMAGE_REL_BASED_DIR64)
                {
                    // 64位dll重定位,IMAGE_REL_BASED_DIR64
                    // 对于IA-64的可执行文件,重定位似乎总是IMAGE_REL_BASED_DIR64类型的
                    if (m_PE->bIs64Pe)
                    {
                        ULONGLONG *pAddress = PIV_PTR_FORWARD(ULONGLONG *, m_PE->pPeBase, pRelocation->VirtualAddress + (pLocData[i] & 0x0FFF));
                        ULONGLONG ullDelta = reinterpret_cast<ULONGLONG>(m_PE->pPeBase) - reinterpret_cast<PIMAGE_NT_HEADERS64>(m_PE->pNtHeader)->OptionalHeader.ImageBase;
                        *pAddress += ullDelta;
                    }
                }
                else if (pLocData[i] >> 12 == IMAGE_REL_BASED_HIGHLOW)
                {
                    // 32位dll重定位,IMAGE_REL_BASED_HIGHLOW
                    // 对于x86的可执行文件,所有的基址重定位都是IMAGE_REL_BASED_HIGHLOW类型的
                    if (m_PE->bIs64Pe == FALSE)
                    {
                        DWORD *pAddress = PIV_PTR_FORWARD(DWORD *, m_PE->pPeBase, pRelocation->VirtualAddress + (pLocData[i] & 0x0FFF));
                        DWORD dwDelta = PIV_PTR_BACKWARD(DWORD, m_PE->pPeBase, m_PE->pNtHeader->OptionalHeader.ImageBase);
                        *pAddress += dwDelta;
                    }
                }
            }
            pRelocation = (PIMAGE_BASE_RELOCATION)((PBYTE)pRelocation + pRelocation->SizeOfBlock);
        }
        if (m_PE->bIs64Pe)
            reinterpret_cast<PIMAGE_NT_HEADERS64>(m_PE->pNtHeader)->OptionalHeader.ImageBase = reinterpret_cast<ULONGLONG>(m_PE->pPeBase);
        else
            m_PE->pNtHeader->OptionalHeader.ImageBase = static_cast<DWORD>(reinterpret_cast<size_t>(m_PE->pPeBase));
        m_Error = 0;
        return TRUE;
    }

    /**
     * @brief 重建导入表,将IAT替换成真实的函数地址(VA),进程和PE的位数不一样会导致失败
     * @param vecHmodule
     * @return
     */
    BOOL RebuildImport(std::vector<HMODULE> *vecHmodule)
    {
        PIMAGE_IMPORT_DESCRIPTOR pImport = m_PE->pImport;
        if (m_PE->pImport == nullptr)
        {
            m_Error = 16;
            return 0;
        }
        DWORD dwThunkSize = GetThunkSize();
        if (dwThunkSize == 0)
        {
            m_Error = 17;
            return FALSE;
        }
        DWORD dwDiff = dwThunkSize == 4 ? 1 : 2;
        HMODULE hModule = NULL;
        UNICODE_STRING ModuleFileName;
        ANSI_STRING FunctionName;
        CVolMem buffer;
        while (pImport->Name != 0)
        {
            PDWORD pRealThunk = PIV_PTR_FORWARD(PDWORD, m_PE->pPeBase, pImport->FirstThunk);
            NtPiv::RtlInitUnicodeString(&ModuleFileName, GetWideText(PIV_PTR_FORWARD(LPCSTR, m_PE->pPeBase, pImport->Name), buffer, NULL));
            if (NT_SUCCESS(NtPiv::LdrLoadDll(NULL, 0, &ModuleFileName, &hModule)))
            {
                vecHmodule->push_back(hModule);
            }
            else
            {
                for (size_t i = 0; vecHmodule->size(); i++)
                {
                    NtPiv::LdrUnloadDll(vecHmodule->at(i));
                }
                vecHmodule->clear();
                m_Error = 20;
                return FALSE;
            }
            while (*pRealThunk != 0)
            {
                if (pImport->TimeDateStamp) // 导入表已经被绑定
                    break;
                VOID *lpFunction = nullptr;
                PIMAGE_THUNK_DATA pRealIAT = reinterpret_cast<PIMAGE_THUNK_DATA>(pRealThunk);
                if (pRealIAT->u1.Ordinal & IMAGE_ORDINAL_FLAG)
                {
                    NtPiv::LdrGetProcedureAddress(hModule, NULL, static_cast<WORD>(pRealIAT->u1.Ordinal & 0x0000FFFF), &lpFunction);
                }
                else
                {
                    PIMAGE_IMPORT_BY_NAME pByName = PIV_PTR_FORWARD(PIMAGE_IMPORT_BY_NAME,
                                                                   m_PE->pPeBase, pRealIAT->u1.AddressOfData);
                    NtPiv::RtlInitAnsiString(&FunctionName, pByName->Name);
                    NtPiv::LdrGetProcedureAddress(hModule, &FunctionName, pByName->Hint, &lpFunction);
                }
                if (lpFunction != nullptr)
                {
                    pRealIAT->u1.Function = reinterpret_cast<DWORD_PTR>(lpFunction);
                }
                else
                {
                    for (size_t i = 0; vecHmodule->size(); i++)
                    {
                        NtPiv::LdrUnloadDll(vecHmodule->at(i));
                    }
                    vecHmodule->clear();
                    m_Error = 21;
                    return FALSE;
                }
                pRealThunk = pRealThunk + dwDiff;
            }
            pImport = PIV_PTR_FORWARD(PIMAGE_IMPORT_DESCRIPTOR, pImport, sizeof(IMAGE_IMPORT_DESCRIPTOR));
        }
        m_Error = 0;
        return TRUE;
    }

    // 以下函数用于修复PE文件(将内存的PE数据还原成文件)
public:
    /**
     * @brief 修复PE文件
     * @param pMemAddress
     * @return
     */
    BOOL RepairToPeFile(void *pMemAddress)
    {
        if (m_PE == nullptr)
        {
            m_Error = 9;
            return FALSE;
        }
        if (m_PE->bIsLoaded == FALSE)
        {
            m_Error = 27;
            return FALSE;
        }
        if (RepairImport(pMemAddress) == FALSE)
        {
            return FALSE;
        }
        if (RepairRelocation(pMemAddress) == FALSE)
        {
            return FALSE;
        }

        if (DeAlignPeDatas(pMemAddress) == FALSE)
        {
            return FALSE;
        }
        m_Error = 0;
        return TRUE;
    }

protected:
    /**
     * @brief 修复导入表,将IAT还原成原样,只有双桥结构的能还原
     * @param pMemAddress
     * @return
     */
    BOOL RepairImport(void *pMemAddress)
    {
        PIMAGE_IMPORT_DESCRIPTOR pImport = m_PE->pImport;
        if (m_PE->pImport == nullptr)
        {
            m_Error = 16;
            return FALSE;
        }
        DWORD dwThunkSize = GetThunkSize();
        if (dwThunkSize == 0)
        {
            m_Error = 17;
            return FALSE;
        }
        DWORD dwDiff = dwThunkSize == 4 ? 1 : 2;
        while (pImport->Name != 0)
        {
            PDWORD pOriginalThunk = PIV_PTR_FORWARD(PDWORD, pMemAddress, pImport->OriginalFirstThunk);
            PDWORD pRealThunk = PIV_PTR_FORWARD(PDWORD, pMemAddress, pImport->FirstThunk);
            while (*pOriginalThunk != 0)
            {
                if (pImport->TimeDateStamp) // 导入表已经被绑定,就不还原了
                    break;
                if (dwDiff == 1)
                {
                    *pRealThunk = *pOriginalThunk;
                }
                else if (dwDiff == 2)
                {
                    *(UINT64 *)pRealThunk = *(UINT64 *)pOriginalThunk;
                }
                pRealThunk = pRealThunk + dwDiff;
                pOriginalThunk = pOriginalThunk + dwDiff;
            }
            pImport = PIV_PTR_FORWARD(PIMAGE_IMPORT_DESCRIPTOR, pImport, sizeof(IMAGE_IMPORT_DESCRIPTOR));
        }
        m_Error = 0;
        return TRUE;
    }

    /**
     * @brief 修复重定位表
     * @param pMemAddress
     * @return
     */
    BOOL RepairRelocation(void *pMemAddress)
    {
        PIMAGE_BASE_RELOCATION pRelocation = PIV_PTR_FORWARD(PIMAGE_BASE_RELOCATION,
                                                            m_PE->pPeBase, GetDataDirectoryRva(IMAGE_DIRECTORY_ENTRY_BASERELOC));

        PIMAGE_DOS_HEADER pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(pMemAddress);
        if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        {
            m_Error = 4;
            return FALSE;
        }
        PIMAGE_NT_HEADERS32 pNtHeader = PIV_PTR_FORWARD(PIMAGE_NT_HEADERS32, pMemAddress, pDosHeader->e_lfanew);
        if (m_PE->bIs64Pe)
            ((PIMAGE_NT_HEADERS64)pNtHeader)->OptionalHeader.ImageBase = IsDllData() ? 0x0000000180000000 : 0x0000000140000000;
        else
            pNtHeader->OptionalHeader.ImageBase = IsDllData() ? 0x10000000 : 0x400000;
        while ((pRelocation->VirtualAddress + pRelocation->SizeOfBlock) != 0)
        {
            WORD *pLocData = PIV_PTR_FORWARD(WORD *, pRelocation, sizeof(IMAGE_BASE_RELOCATION));
            DWORD nNumberOfReloc = (pRelocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
            for (DWORD i = 0; i < nNumberOfReloc; i++)
            {
                // 每个WORD由两部分组成,高4位指出了重定位的类型,WINNT.H中的一系列IMAGE_REL_BASED_xxx定义了重定位类型的取值
                // 低12位是相对于VirtualAddress域的偏移,指出了必须进行重定位的位置
                if (m_PE->bIs64Pe)
                {
                    if (pLocData[i] >> 12 == IMAGE_REL_BASED_DIR64)
                    {
                        // 64位dll重定位,IMAGE_REL_BASED_DIR64
                        // 对于IA-64的可执行文件,重定位似乎总是IMAGE_REL_BASED_DIR64类型的
                        ULONGLONG *pAddress = PIV_PTR_FORWARD(ULONGLONG *, pMemAddress, pRelocation->VirtualAddress + (pLocData[i] & 0x0FFF));
                        ULONGLONG ullDelta = *pAddress - reinterpret_cast<ULONGLONG>(m_PE->pPeBase) + reinterpret_cast<PIMAGE_NT_HEADERS64>(pNtHeader)->OptionalHeader.ImageBase;
                        *(ULONGLONG *)pAddress = ullDelta;
                    }
                }
                else
                {
                    if (pLocData[i] >> 12 == IMAGE_REL_BASED_HIGHLOW)
                    {
                        // 32位dll重定位,IMAGE_REL_BASED_HIGHLOW
                        // 对于x86的可执行文件,所有的基址重定位都是IMAGE_REL_BASED_HIGHLOW类型的
                        DWORD *pAddress = PIV_PTR_FORWARD(DWORD *, pMemAddress, pRelocation->VirtualAddress + (pLocData[i] & 0x0FFF));
                        DWORD dwDelta = *pAddress - static_cast<DWORD>(reinterpret_cast<size_t>(m_PE->pPeBase)) + pNtHeader->OptionalHeader.ImageBase;
                        *(DWORD *)pAddress = dwDelta;
                    }
                }
            }
            pRelocation = PIV_PTR_FORWARD(PIMAGE_BASE_RELOCATION, pRelocation, pRelocation->SizeOfBlock);
        }
        m_Error = 0;
        return TRUE;
    }

    /**
     * @brief 将在内存对齐的PE数据还原
     * @param pMemAddress
     * @return
     */
    BOOL DeAlignPeDatas(void *pMemAddress)
    {
        for (DWORD i = 0; i < m_PE->dwSectionCount; ++i)
        {
            if (m_PE->pSection[i].VirtualAddress == 0 || m_PE->pSection[i].SizeOfRawData == 0)
                continue;
            void *pSectionAddress = PIV_PTR_FORWARD(void *, pMemAddress, m_PE->pSection[i].VirtualAddress);
            memcpy(PIV_PTR_FORWARD(void *, pMemAddress, m_PE->pSection[i].PointerToRawData), pSectionAddress, m_PE->pSection[i].SizeOfRawData);
        }
        m_Error = 0;
        return TRUE;
    }

    // 以下为内部函数
protected:
    /**
     * @brief 检查是否内存DLL,并返回对应的RVA地址或FOA地址
     * @param dwAddress
     * @return
     */
    DWORD GetRealAddress(DWORD dwAddress)
    {
        return m_PE->bIsMemAlign ? dwAddress : Rva2Foa(dwAddress);
    }

    /**
     * @brief 检查文件头,返回是否为有效的PE格式
     * @return
     */
    BOOL CheckHeaders()
    {
        m_PE->pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(m_PE->pPeBase);
        if (m_PE->pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        {
            m_Error = 4;
            return FALSE;
        }
        m_PE->pNtHeader = PIV_PTR_FORWARD(PIMAGE_NT_HEADERS32, m_PE->pPeBase, m_PE->pDosHeader->e_lfanew);
        if (m_PE->pNtHeader->Signature != IMAGE_NT_SIGNATURE)
        {
            m_Error = 5;
            return FALSE;
        }
        WORD Machine = m_PE->pNtHeader->FileHeader.Machine;
        if (Machine == IMAGE_FILE_MACHINE_AMD64 || Machine == IMAGE_FILE_MACHINE_IA64)
        {
            if (reinterpret_cast<PIMAGE_NT_HEADERS64>(m_PE->pNtHeader)->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
            {
                m_Error = 6;
                return FALSE;
            }
            m_PE->bIs64Pe = TRUE;
        }
        else if (Machine == IMAGE_FILE_MACHINE_I386)
        {
            if (m_PE->pNtHeader->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
            {
                m_Error = 6;
                return FALSE;
            }
            m_PE->bIs64Pe = FALSE;
        }
        else
        {
            m_Error = 7;
            return FALSE;
        }
        m_PE->dwSectionCount = m_PE->pNtHeader->FileHeader.NumberOfSections;
        m_PE->pSection = IMAGE_FIRST_SECTION(m_PE->pNtHeader);
        m_PE->pImport = PIV_PTR_FORWARD(PIMAGE_IMPORT_DESCRIPTOR, m_PE->pPeBase,
                                       GetRealAddress(GetDataDirectoryRva(IMAGE_DIRECTORY_ENTRY_IMPORT)));
        m_PE->pExport = PIV_PTR_FORWARD(PIMAGE_EXPORT_DIRECTORY, m_PE->pPeBase,
                                       GetRealAddress(GetDataDirectoryRva(IMAGE_DIRECTORY_ENTRY_EXPORT)));
        m_Error = 0;
        return TRUE;
    }
}; // PivPeFile

/**
 * @brief 内存DLL加载类,继承了PivPeFile
 */
class PivMemLoader : public PivPeFile
{
private:
    typedef BOOL(__stdcall *ProcDllMain)(HINSTANCE, DWORD, LPVOID);

public:
    PivMemLoader()
    {
        m_bIsLoadOk = FALSE;
        m_fnDllMain = nullptr;
    }
    ~PivMemLoader()
    {
        MemFreeLibrary();
    }

private:
    BOOL m_bIsLoadOk;
    ProcDllMain m_fnDllMain;
    std::vector<HMODULE> m_vecHmodule;

public:
    /**
     * @brief 卸载内存DLL
     * @return
     */
    VOID MemFreeLibrary()
    {
        if (m_bIsLoadOk == TRUE)
        {
            m_fnDllMain(reinterpret_cast<HINSTANCE>(m_PE->pPeBase), DLL_PROCESS_DETACH, 0);
            for (size_t i = 0; m_vecHmodule.size(); i++)
            {
                NtPiv::LdrUnloadDll(m_vecHmodule[i]);
            }
            m_vecHmodule.clear();
            NtPiv::NtFreeVirtualMemory(NULL, &m_PE->pPeBase, NULL, MEM_RELEASE);
            m_PE->pPeBase = nullptr;
            m_fnDllMain = nullptr;
            m_bIsLoadOk = FALSE;
        }
    }

    /**
     * @brief 加载内存DLL
     * @param lpFileData
     * @param dwDataLength
     * @return
     */
    BOOL MemLoadLibrary(void *lpFileData, DWORD dwDataLength)
    {
        if (m_bIsLoadOk == TRUE)
        {
            m_Error = 22;
            return FALSE;
        }
        if (OpenPeData(lpFileData, dwDataLength, FALSE, FALSE) == FALSE)
            return FALSE;
            // 验证DLL和程序的位数是否一致
#ifdef _WIN64
        if (m_PE->bIs64Pe == FALSE)
        {
            m_Error = 8;
            return FALSE;
        }
#else
        if (m_PE->bIs64Pe == TRUE)
        {
            m_Error = 8;
            return FALSE;
        }
#endif
        if (IsDllData() == FALSE)
            return FALSE;
        m_vecHmodule.clear();
        if (AlignPeDatas() == FALSE)
        {
            NtPiv::NtFreeVirtualMemory(NULL, &m_PE->pPeBase, NULL, MEM_RELEASE);
            m_PE->pPeBase = nullptr;
            return FALSE;
        }
        if (GetDataDirectoryRva(IMAGE_DIRECTORY_ENTRY_BASERELOC) > 0 && GetDataDirectorySize(IMAGE_DIRECTORY_ENTRY_BASERELOC) > 0)
        {
            RebuildRelocation();
        }
        if (RebuildImport(&m_vecHmodule) == FALSE)
        {
            NtPiv::NtFreeVirtualMemory(NULL, &m_PE->pPeBase, NULL, MEM_RELEASE);
            m_PE->pPeBase = nullptr;
            return FALSE;
        }
        m_fnDllMain = PIV_PTR_FORWARD(ProcDllMain, m_PE->pPeBase, GetEntryPoint());
        BOOL InitResult = m_fnDllMain(reinterpret_cast<HINSTANCE>(m_PE->pPeBase), DLL_PROCESS_ATTACH, 0);
        if (InitResult == FALSE) // 初始化失败
        {
            m_fnDllMain(reinterpret_cast<HINSTANCE>(m_PE->pPeBase), DLL_PROCESS_DETACH, 0);
            for (size_t i = 0; m_vecHmodule.size(); i++)
            {
                NtPiv::LdrUnloadDll(m_vecHmodule[i]);
            }
            m_vecHmodule.clear();
            NtPiv::NtFreeVirtualMemory(NULL, &m_PE->pPeBase, NULL, MEM_RELEASE);
            m_fnDllMain = nullptr;
            m_Error = 23;
            return FALSE;
        }
        m_bIsLoadOk = TRUE;
        m_Error = 0;
        return TRUE;
    }

    /**
     * @brief 取函数地址
     * @param lpProcName
     * @return
     */
    FARPROC MemGetProcAddress(LPCSTR lpProcName)
    {
        if (!m_bIsLoadOk)
        {
            m_Error = 24;
            return nullptr;
        }
        DWORD dwOffsetStart = GetDataDirectoryRva(IMAGE_DIRECTORY_ENTRY_EXPORT);
        DWORD dwSize = GetDataDirectorySize(IMAGE_DIRECTORY_ENTRY_EXPORT);
        if (dwOffsetStart == 0 || dwSize == 0)
        {
            m_Error = 15;
            return nullptr;
        }
        DWORD dwBase = m_PE->pExport->Base;
        int nNumberOfFunctions = (int)m_PE->pExport->NumberOfFunctions;
        int nNumberOfNames = (int)m_PE->pExport->NumberOfNames;
        LPDWORD pAddressOfFunctions = PIV_PTR_FORWARD(LPDWORD, m_PE->pPeBase, m_PE->pExport->AddressOfFunctions);
        LPWORD pAddressOfOrdinals = PIV_PTR_FORWARD(LPWORD, m_PE->pPeBase, m_PE->pExport->AddressOfNameOrdinals);
        LPDWORD pAddressOfNames = PIV_PTR_FORWARD(LPDWORD, m_PE->pPeBase, m_PE->pExport->AddressOfNames);
        int nOrdinal = -1;
        if (((DWORD_PTR)lpProcName & 0xFFFF0000) == 0)
        {
            nOrdinal = (DWORD_PTR)lpProcName & 0x0000FFFF - dwBase;
        }
        else
        {
            int nFound = -1;
            for (int i = 0; i < nNumberOfNames; i++)
            {
                char *szName = PIV_PTR_FORWARD(char *, m_PE->pPeBase, pAddressOfNames[i]);
                if (strcmp(szName, lpProcName) == 0) // 比较函数名称
                {
                    nFound = i;
                    break;
                }
            }
            if (nFound >= 0)
                nOrdinal = static_cast<DWORD>(pAddressOfOrdinals[nFound]);
        }
        if (nOrdinal < 0 || nOrdinal >= nNumberOfFunctions)
        {
            m_Error = 25;
            return nullptr;
        }
        else
        {
            DWORD pFunctionOffset = pAddressOfFunctions[nOrdinal];
            if (pFunctionOffset > dwOffsetStart && pFunctionOffset < (dwOffsetStart + dwSize))
            {
                m_Error = 26;
                return nullptr;
            }
            else
            {
                m_Error = 0;
                return PIV_PTR_FORWARD(FARPROC, m_PE->pPeBase, pFunctionOffset);
            }
        }
    }

    /**
     * @brief 返回是否已成功加载内存DLL
     * @return
     */
    BOOL IsLoadOk() const
    {
        return m_bIsLoadOk;
    }
}; // PivMemLoader

#endif // PIV_PEFILE_HPP
