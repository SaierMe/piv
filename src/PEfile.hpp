/*********************************************\
 * 火山视窗PIV模块 - PE文件操作              *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef PIV_PEFILE_HPP
#define PIV_PEFILE_HPP

#include "piv_NtProcess.hpp"
#include "piv_file.hpp"
#include <vector>

/**
 * @brief 自写的PE文件操作类
 */
class PivPeFile
{
protected:
    struct PEFILE_INFO
    {
        bool bIs64Pe = false;      // 是否64位PE
        bool bIsMemAlign = false;  // 是否内存对齐
        bool bIsLoaded = false;    // 是否已加载运行
        bool bIsWriteable = false; // PE是否可写
        void* pPeBase = nullptr;   // PE数据的基址
        DWORD dwSectionCount = 0;  // 节区数量
        size_t cbPeSize = 0;       // PE数据的当前尺寸
        PIMAGE_DOS_HEADER pDosHeader = nullptr;
        PIMAGE_NT_HEADERS32 pNtHeader = nullptr;
        PIMAGE_SECTION_HEADER pSection = nullptr;
        PIMAGE_EXPORT_DIRECTORY pExport = nullptr;
        PIMAGE_IMPORT_DESCRIPTOR pImport = nullptr;
        PivFileMapping FileMap; // 内存映射文件

        PEFILE_INFO() {}
        ~PEFILE_INFO()
        {
            if (pPeBase != nullptr && bIsLoaded == false && bIsWriteable == true)
            {
                PivNT::instance().NtFreeVirtualMemory(NULL, &pPeBase, NULL, MEM_RELEASE);
            }
        }
    };
    const WCHAR* szErrorsMessage[31]{
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
    std::unique_ptr<PEFILE_INFO> m_pe;
    std::vector<HMODULE> m_hmodule_arr;
    INT m_error = 0;
    PivNT& Nt = PivNT::instance();

public:
    PivPeFile() {}
    ~PivPeFile() {}

    // 以下函数任何时候皆可调用

    /**
     * @brief 取最后错误代码
     * @return
     */
    INT GetErrorCode() const
    {
        return m_error;
    }

    /**
     * @brief 取最后错误信息
     * @return
     */
    const WCHAR* GetErrorMessage() const
    {
        return szErrorsMessage[m_error];
    }

    /**
     * @brief 打开PE文件
     * @param szFileName
     * @return
     */
    BOOL OpenPeFile(const WCHAR* szFileName)
    {
        m_pe.reset(new PEFILE_INFO);
        if (FALSE == m_pe->FileMap.Create(szFileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, 0, PAGE_READONLY, nullptr))
        {
            m_error = 1;
            goto _PIVPE_FAIL_;
        }
        if (FALSE == m_pe->FileMap.MapToMemory(0, 0, FILE_MAP_READ))
        {
            m_error = 3;
            goto _PIVPE_FAIL_;
        }
        m_pe->pPeBase = m_pe->FileMap.GetPtr<void>(0);
        m_pe->cbPeSize = m_pe->FileMap.GetViewSize();
        m_pe->bIsMemAlign = false;
        if (CheckHeaders())
        {
            m_pe->bIsMemAlign = false;
            m_pe->bIsLoaded = false;
            return TRUE;
        }
    _PIVPE_FAIL_:
        m_pe.reset(nullptr);
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
    BOOL OpenPeData(void* lpPeAddress, size_t dwPeDataSize, bool bIsMemAlign = false, bool bIsLoaded = false)
    {
        if (lpPeAddress == (void*)0x400000 || lpPeAddress == (void*)0x140000000 || lpPeAddress == (void*)0x10000000 || lpPeAddress == (void*)0x180000000)
        { // 如果模块基址被隐藏了,说明PE文件已经被加壳
            m_error = 29;
            return FALSE;
        }
        m_pe.reset(new PEFILE_INFO);
        m_pe->pPeBase = lpPeAddress;
        m_pe->cbPeSize = dwPeDataSize;
        m_pe->bIsMemAlign = bIsMemAlign;
        m_pe->bIsLoaded = bIsLoaded;
        if (CheckHeaders())
            return TRUE;
        m_pe.reset(nullptr);
        return FALSE;
    }

    /**
     * @brief 关闭PE数据
     */
    void ClosePeData()
    {
        m_pe.reset(nullptr);
    }

    /**
     * @brief 将当前的PE数据原样复制到新内存
     * @param bCloseSrc
     * @return
     */
    VOID* CopyToNewMem(BOOL bCloseSrc)
    {
        if (!m_pe)
        {
            m_error = 9;
            return 0;
        }
        VOID* pMemAddress = CopyToMem();
        if (pMemAddress == nullptr)
        {
            return 0;
        }
        if (bCloseSrc)
        {
            size_t dwPeSize = m_pe->cbPeSize;
            bool bIsMemAlign = m_pe->bIsMemAlign;
            bool bIsLoaded = m_pe->bIsLoaded;
            m_pe.reset(new PEFILE_INFO);
            m_pe->pPeBase = pMemAddress;
            m_pe->cbPeSize = dwPeSize;
            m_pe->bIsMemAlign = bIsMemAlign;
            m_pe->bIsLoaded = bIsLoaded;
            if (!CheckHeaders())
            {
                m_pe.reset(nullptr);
                return 0;
            }
        }
        m_error = 0;
        return pMemAddress;
    }

    /**
     * @brief 保存数据
     * @param szFileName
     * @param bRepairPe
     * @return
     */
    BOOL SavePeDatas(const WCHAR* szFileName, BOOL bRepairPe)
    {
        void* pMemoryAddress = CopyToNewMem(FALSE);
        if (pMemoryAddress == nullptr)
        {
            return FALSE;
        }
        size_t dwWriteSize;
        if (bRepairPe)
        {
            if (RepairToPeFile(pMemoryAddress) == FALSE)
            {
                Nt.NtFreeVirtualMemory(NULL, &pMemoryAddress, NULL, MEM_RELEASE);
                return FALSE;
            }
            dwWriteSize = CalcPeFileSize();
        }
        else
        {
            dwWriteSize = m_pe->cbPeSize;
        }
        bool bRes = false;
        FILE* out = NULL;
        _wfopen_s(&out, szFileName, L"wb");
        if (out != NULL)
        {
            bRes = (fwrite(pMemoryAddress, 1, dwWriteSize, out) == dwWriteSize);
            fclose(out);
        }
        if (pMemoryAddress)
        {
            Nt.NtFreeVirtualMemory(NULL, &pMemoryAddress, NULL, MEM_RELEASE);
        }
        if (bRes)
        {
            m_error = 0;
            return TRUE;
        }
        else
        {
            m_error = 28;
            return FALSE;
        }
    }

    // 以下函数打开PE成功后可用

    /**
     * @brief 取PE数据指针
     * @return
     */
    const void* GetBaseAddress() const
    {
        return m_pe ? m_pe->pPeBase : 0;
    }

    /**
     * @brief 计算映像文件内存对齐后的尺寸
     * @return
     */
    DWORD CalcTotalImageSize()
    {
        if (m_pe == nullptr)
        {
            m_error = 9;
            return 0;
        }
        if (m_pe->pPeBase == nullptr)
        {
            m_error = 10;
            return 0;
        }
        DWORD dwImageSize = 0;
        DWORD dwAlignment = m_pe->bIs64Pe ? reinterpret_cast<PIMAGE_NT_HEADERS64>(m_pe->pNtHeader)->OptionalHeader.SectionAlignment
                                          : m_pe->pNtHeader->OptionalHeader.SectionAlignment;
        dwImageSize = (m_pe->bIs64Pe ? reinterpret_cast<PIMAGE_NT_HEADERS64>(m_pe->pNtHeader)->OptionalHeader.SizeOfHeaders
                                     : m_pe->pNtHeader->OptionalHeader.SizeOfHeaders + dwAlignment - 1) /
            dwAlignment * dwAlignment;
        for (DWORD i = 0; i < m_pe->dwSectionCount; i++)
        {
            DWORD dwCodeSize = m_pe->pSection[i].Misc.VirtualSize;
            DWORD dwLoadSize = m_pe->pSection[i].SizeOfRawData;
            DWORD dwMaxSize = (dwLoadSize > dwCodeSize) ? (dwLoadSize) : (dwCodeSize);
            DWORD dwSectionSize = (m_pe->pSection[i].VirtualAddress + dwMaxSize + dwAlignment - 1) / dwAlignment * dwAlignment;
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
        if (m_pe == nullptr)
        {
            m_error = 9;
            return 0;
        }
        m_error = 0;
        return m_pe->pSection[m_pe->dwSectionCount - 1].SizeOfRawData + m_pe->pSection[m_pe->dwSectionCount - 1].PointerToRawData;
    }

    /**
     * @brief 取PE数据的当前尺寸
     * @return
     */
    size_t GetDataSize() const
    {
        return m_pe ? m_pe->cbPeSize : 0;
    }

    /**
     * @brief 取入口点
     * @return
     */
    DWORD GetEntryPoint()
    {
        if (m_pe)
            return m_pe->bIs64Pe ? reinterpret_cast<PIMAGE_NT_HEADERS64>(m_pe->pNtHeader)->OptionalHeader.AddressOfEntryPoint
                                 : m_pe->pNtHeader->OptionalHeader.AddressOfEntryPoint;
        m_error = 9;
        return 0;
    }

    /**
     * @brief 校验PE是否为DLL文件
     * @return
     */
    BOOL IsDllData()
    {
        if (m_pe == nullptr)
        {
            m_error = 9;
            return FALSE;
        }
        if ((m_pe->pNtHeader->FileHeader.Characteristics & IMAGE_FILE_DLL) == 0)
        {
            m_error = 11;
            return FALSE;
        }
        if ((m_pe->pNtHeader->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) == 0)
        {
            m_error = 12;
            return FALSE;
        }
        m_error = 0;
        return TRUE;
    }

    /**
     * @brief 校验PE是否为EXE文件
     * @return
     */
    BOOL IsExeData()
    {
        if (m_pe == nullptr)
        {
            m_error = 9;
            return FALSE;
        }
        if ((m_pe->pNtHeader->FileHeader.Characteristics & IMAGE_FILE_DLL) == IMAGE_FILE_DLL)
        {
            m_error = 30;
            return FALSE;
        }
        if ((m_pe->pNtHeader->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) == 0)
        {
            m_error = 12;
            return FALSE;
        }
        m_error = 0;
        return TRUE;
    }

    /**
     * @brief 返回当前的PE数据是否可写
     * @return
     */
    BOOL IsWriteable() const
    {
        return m_pe ? m_pe->bIsWriteable : FALSE;
    }

    /**
     * @brief 相对虚拟地址到文件偏移
     * @param dwRVA
     * @return
     */
    DWORD Rva2Foa(DWORD dwRVA)
    {
        if (m_pe == nullptr)
        {
            m_error = 9;
            return FALSE;
        }
        if (m_pe->pPeBase == nullptr)
        {
            m_error = 10;
            return FALSE;
        }
        DWORD dwDiffer = 0;
        DWORD dwAlignment = m_pe->bIs64Pe ? reinterpret_cast<PIMAGE_NT_HEADERS64>(m_pe->pNtHeader)->OptionalHeader.SectionAlignment
                                          : m_pe->pNtHeader->OptionalHeader.SectionAlignment;
        for (DWORD i = 0; i < m_pe->dwSectionCount; i++)
        {
            DWORD dwBlockCount = m_pe->pSection[i].SizeOfRawData / dwAlignment;
            dwBlockCount += m_pe->pSection[i].SizeOfRawData % dwAlignment ? 1 : 0;
            DWORD dwBeginVA = m_pe->pSection[i].VirtualAddress;
            DWORD dwEndVA = m_pe->pSection[i].VirtualAddress + dwBlockCount * dwAlignment;
            if (dwRVA >= dwBeginVA && dwRVA < dwEndVA)
            {
                dwDiffer = dwRVA - dwBeginVA;
                return m_pe->pSection[i].PointerToRawData + dwDiffer;
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
        if (m_pe == nullptr)
        {
            m_error = 9;
            return FALSE;
        }
        if (m_pe->pPeBase == nullptr)
        {
            m_error = 10;
            return FALSE;
        }
        DWORD dwVPK = 0;
        for (DWORD i = 0; i < m_pe->dwSectionCount; i++)
        {
            DWORD dwBeginVA = m_pe->pSection[i].PointerToRawData;
            DWORD dwEndVA = m_pe->pSection[i].PointerToRawData + m_pe->pSection[i].SizeOfRawData;
            // 如果dwFoa在某个区段中
            if (dwFoa >= dwBeginVA && dwFoa < dwEndVA)
            {
                dwVPK = m_pe->pSection[i].VirtualAddress - dwBeginVA;
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
        if (m_pe == nullptr)
        {
            m_error = 9;
            return FALSE;
        }
        if (m_pe->bIsLoaded == TRUE)
        {
            m_error = 22;
            return TRUE;
        }
        // 验证DLL和程序的位数是否一致
#ifdef _WIN64
        if (m_pe->bIs64Pe == FALSE)
        {
            m_error = 8;
            return FALSE;
        }
#else
        if (m_pe->bIs64Pe == TRUE)
        {
            m_error = 8;
            return FALSE;
        }
#endif
        if ((m_pe->pNtHeader->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) == 0)
        {
            m_error = 12;
            return FALSE;
        }
        if (m_pe->bIsWriteable == FALSE)
        {
            if (CopyToMem() == 0)
            {
                return FALSE;
            }
        }
        if (m_pe->bIsMemAlign == FALSE)
        {
            if (AlignPeDatas() == FALSE)
            {
                return FALSE;
            }
        }
        if (m_pe->bIsLoaded == FALSE)
        {
            if (GetDataDirectoryRva(IMAGE_DIRECTORY_ENTRY_BASERELOC) > 0 && GetDataDirectorySize(IMAGE_DIRECTORY_ENTRY_BASERELOC) > 0)
            {
                RebuildRelocation();
            }
            m_hmodule_arr.clear();
            if (RebuildImport(&m_hmodule_arr) == FALSE)
            {
                return FALSE;
            }
        }
        m_pe->bIsLoaded = TRUE;
        m_error = 0;
        return TRUE;
    }

    // 以下函数用于IMAGE_DOS_SIGNATURE

    /**
     * @brief 获取DOS头
     * @return
     */
    const PIMAGE_DOS_HEADER GetDosHeader() const
    {
        return m_pe ? m_pe->pDosHeader : nullptr;
    }

    /**
     * @brief 获取DOS的入口地址
     * @return
     */
    DWORD GetDosEntryPoint() const
    {
        return m_pe ? static_cast<DWORD>(m_pe->pDosHeader->e_cs + m_pe->pDosHeader->e_cparhdr) * 0x10UL + m_pe->pDosHeader->e_ip : 0;
    }

    // 以下函数用于IMAGE_NT_SIGNATURE

    /**
     * @brief 获取NT头,如果是64位PE,实际返回的是PIMAGE_NT_HEADERS64
     * @return
     */
    const PIMAGE_NT_HEADERS32 GetNtHeader() const
    {
        return m_pe ? m_pe->pNtHeader : nullptr;
    }

    /**
     * @brief 返回PE数据是否为64位
     * @return
     */
    BOOL Is64Pe() const
    {
        return m_pe ? m_pe->bIs64Pe : FALSE;
    }

    /**
     * @brief 获取映像基址
     * @return
     */
    ULONGLONG GetImageBase()
    {
        if (m_pe)
            return m_pe->bIs64Pe ? reinterpret_cast<PIMAGE_NT_HEADERS64>(m_pe->pNtHeader)->OptionalHeader.ImageBase : m_pe->pNtHeader->OptionalHeader.ImageBase;
        m_error = 9;
        return 0;
    }

    /**
     * @brief 获取节区表头数组
     * @param dwSectionCount
     * @return
     */
    const PIMAGE_SECTION_HEADER GetSectionHeader(PDWORD dwSectionCount)
    {
        if (m_pe)
        {
            *dwSectionCount = m_pe->dwSectionCount;
            m_error = 0;
            return m_pe->pSection;
        }
        *dwSectionCount = 0;
        m_error = 9;
        return nullptr;
    }

    /**
     * @brief 获取导出表头
     * @return
     */
    const PIMAGE_EXPORT_DIRECTORY GetExportHeader() const
    {
        return m_pe ? m_pe->pExport : nullptr;
    }

    /**
     * @brief 获取导入表描述符
     * @return
     */
    const PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor() const
    {
        return m_pe ? m_pe->pImport : nullptr;
    }

    /**
     * @brief 获取各DataDirectory入口的RVA
     * @param dwIndex
     * @return
     */
    DWORD GetDataDirectoryRva(DWORD dwIndex)
    {
        if (m_pe == nullptr)
        {
            m_error = 9;
            return 0;
        }
        PIMAGE_DATA_DIRECTORY pIDD = m_pe->bIs64Pe ? reinterpret_cast<IMAGE_NT_HEADERS64*>(m_pe->pNtHeader)->OptionalHeader.DataDirectory
                                                   : m_pe->pNtHeader->OptionalHeader.DataDirectory;
        if (dwIndex >= 0 && dwIndex <= 15)
        {
            m_error = 0;
            return pIDD ? pIDD[dwIndex].VirtualAddress : 0;
        }
        else
        {
            m_error = 13;
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
        if (m_pe == nullptr)
        {
            m_error = 9;
            return 0;
        }
        PIMAGE_DATA_DIRECTORY pIDD = m_pe->bIs64Pe ? reinterpret_cast<IMAGE_NT_HEADERS64*>(m_pe->pNtHeader)->OptionalHeader.DataDirectory
                                                   : m_pe->pNtHeader->OptionalHeader.DataDirectory;
        if (dwIndex >= 0 && dwIndex <= 15)
        {
            m_error = 0;
            return pIDD ? pIDD[dwIndex].Size : 0;
        }
        else
        {
            m_error = 13;
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
    DWORD GetExportInfo(CVolString* szModuleName, std::vector<DWORD>* vecFunOrdinal, std::vector<DWORD>* vecFunAddress, std::vector<CVolString>* vecFunName)
    {
        if (m_pe == nullptr)
        {
            m_error = 9;
            return 0;
        }
        if (m_pe->pExport == nullptr)
        {
            m_error = 14;
            return 0;
        }
        if (m_pe->pExport->NumberOfFunctions == 0)
        {
            m_error = 15;
            return 0;
        }
        szModuleName->SetText(PivA2Ws{PIV_PTR_FORWARD(const CHAR*, m_pe->pPeBase, GetRealAddress(m_pe->pExport->Name))}.c_str());
        DWORD* pFunctions = PIV_PTR_FORWARD(DWORD*, m_pe->pPeBase, GetRealAddress(m_pe->pExport->AddressOfFunctions));
        DWORD* pNames = PIV_PTR_FORWARD(DWORD*, m_pe->pPeBase, GetRealAddress(m_pe->pExport->AddressOfNames));
        WORD* pOrdinals = PIV_PTR_FORWARD(WORD*, m_pe->pPeBase, GetRealAddress(m_pe->pExport->AddressOfNameOrdinals));
        for (DWORD i = 0; i < m_pe->pExport->NumberOfFunctions; i++)
        {
            vecFunAddress->push_back(pFunctions[i]);
            vecFunOrdinal->push_back(i + m_pe->pExport->Base);
            vecFunName->push_back(CVolString(L""));
        }
        for (DWORD i = 0; i < m_pe->pExport->NumberOfNames; i++)
        {
            vecFunName->at(i).SetText(PivA2Ws{PIV_PTR_FORWARD(const CHAR*, m_pe->pPeBase, GetRealAddress(pNames[i]))}.c_str());
        }
        m_error = 0;
        return m_pe->pExport->NumberOfFunctions;
    }

    /**
     * @brief 2. 获取导入表信息
     * @param vecModuleName
     * @param vecFunOrdinal
     * @param vecFunAddress
     * @param vecFunName
     * @return
     */
    DWORD GetImportInfo(std::vector<CVolString>* vecModuleName, std::vector<std::vector<DWORD>>* vecFunOrdinal,
                        std::vector<std::vector<ULONGLONG>>* vecFunAddress, std::vector<std::vector<CVolString>>* vecFunName)
    {
        if (m_pe == nullptr)
        {
            m_error = 9;
            return 0;
        }
        PIMAGE_IMPORT_DESCRIPTOR pImport = m_pe->pImport;
        DWORD dwThunkSize = GetThunkSize();
        if (dwThunkSize == 0)
        {
            m_error = 17;
            return 0;
        }
        DWORD dwDiff = dwThunkSize == 4 ? 1 : 2;
        while (pImport->Name) // 历遍导入表
        {
            vecModuleName->emplace_back(PivA2Ws{PIV_PTR_FORWARD(const CHAR*, m_pe->pPeBase, GetRealAddress(pImport->Name))}.c_str());
            PDWORD dwINT_Thunk = PIV_PTR_FORWARD(PDWORD, m_pe->pPeBase, GetRealAddress(pImport->OriginalFirstThunk));
            PDWORD dwIAT_Thunk = PIV_PTR_FORWARD(PDWORD, m_pe->pPeBase, GetRealAddress(pImport->FirstThunk));
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
                    llAddress = m_pe->bIs64Pe ? reinterpret_cast<PIMAGE_THUNK_DATA64>(pIAT_Thunk)->u1.Function : pIAT_Thunk->u1.Function;
                if (m_pe->bIsLoaded == FALSE) // PE未运行时,FirstThunk指向函数序号和名称
                {
                    if (dwDiff == 2)
                    {
                        if (IMAGE_SNAP_BY_ORDINAL64(reinterpret_cast<PIMAGE_THUNK_DATA64>(pIAT_Thunk)->u1.Ordinal))
                            dwOrdinal = IMAGE_ORDINAL64(reinterpret_cast<PIMAGE_THUNK_DATA64>(pIAT_Thunk)->u1.Ordinal);
                        else
                            pByName = PIV_PTR_FORWARD(PIMAGE_IMPORT_BY_NAME, m_pe->pPeBase, GetRealAddress(static_cast<DWORD>(pIAT_Thunk->u1.AddressOfData)));
                    }
                    else
                    {
                        if (IMAGE_SNAP_BY_ORDINAL32(pIAT_Thunk->u1.Ordinal))
                            dwOrdinal = IMAGE_ORDINAL32(pIAT_Thunk->u1.Ordinal);
                        else
                            pByName = PIV_PTR_FORWARD(PIMAGE_IMPORT_BY_NAME, m_pe->pPeBase, GetRealAddress(static_cast<DWORD>(pIAT_Thunk->u1.AddressOfData)));
                    }
                    if (pByName)
                    {
                        dwOrdinal = pByName->Hint;
                        szName.SetText(PivA2Ws{pByName->Name}.c_str());
                    }
                }
                else // PE运行时,FirstThunk指向的地址被替换为真实的函数地址(VA)
                {
                    llAddress = m_pe->bIs64Pe ? reinterpret_cast<PIMAGE_THUNK_DATA64>(pIAT_Thunk)->u1.Function : pIAT_Thunk->u1.Function;
                    if (pImport->OriginalFirstThunk) // 如果原始桥有效,取函数序号和名称
                    {
                        if (dwDiff == 2)
                        {
                            if (IMAGE_SNAP_BY_ORDINAL64(reinterpret_cast<PIMAGE_THUNK_DATA64>(pINT_Thunk)->u1.Ordinal))
                                dwOrdinal = IMAGE_ORDINAL64(reinterpret_cast<PIMAGE_THUNK_DATA64>(pINT_Thunk)->u1.Ordinal);
                            else
                                pByName = PIV_PTR_FORWARD(PIMAGE_IMPORT_BY_NAME, m_pe->pPeBase, GetRealAddress(static_cast<DWORD>(pINT_Thunk->u1.AddressOfData)));
                        }
                        else
                        {
                            if (IMAGE_SNAP_BY_ORDINAL32(pINT_Thunk->u1.Ordinal))
                                dwOrdinal = IMAGE_ORDINAL32(pINT_Thunk->u1.Ordinal);
                            else
                                pByName = PIV_PTR_FORWARD(PIMAGE_IMPORT_BY_NAME, m_pe->pPeBase, GetRealAddress(static_cast<DWORD>(pINT_Thunk->u1.AddressOfData)));
                        }
                        if (pByName)
                        {
                            dwOrdinal = pByName->Hint;
                            szName.SetText(PivA2Ws{pByName->Name}.c_str());
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
        m_error = 0;
        return (DWORD)vecModuleName->size();
    }

    // 3. 获取异常表

    // 以下函数与加载运行PE有关
protected:
    /**
     * @brief 将当前的PE数据原样复制到新内存,申请的内存大小为PE的映像尺寸
     * @return
     */
    VOID* CopyToMem()
    {
        SIZE_T dwImageSize = CalcTotalImageSize();
        if (dwImageSize == 0)
        {
            m_error = 18;
            return 0;
        }
        void* pMemoryAddress = nullptr;
        Nt.NtAllocateVirtualMemory(NULL, &pMemoryAddress, 0, &dwImageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (pMemoryAddress == nullptr)
        {
            m_error = 19;
            return 0;
        }
        memcpy(pMemoryAddress, m_pe->pPeBase, m_pe->cbPeSize);
        m_pe->bIsWriteable = TRUE;
        m_error = 0;
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
            m_error = 18;
            return FALSE;
        }
        void* pMemoryAddress = nullptr;
        Nt.NtAllocateVirtualMemory(NULL, &pMemoryAddress, 0, &dwImageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (pMemoryAddress == nullptr)
        {
            m_error = 19;
            return FALSE;
        }
        DWORD cbHeaders = m_pe->bIs64Pe ? reinterpret_cast<PIMAGE_NT_HEADERS64>(m_pe->pNtHeader)->OptionalHeader.SizeOfHeaders
                                        : m_pe->pNtHeader->OptionalHeader.SizeOfHeaders;
        memcpy(pMemoryAddress, m_pe->pPeBase, cbHeaders);
        for (DWORD i = 0; i < m_pe->dwSectionCount; ++i)
        {
            if (m_pe->pSection[i].VirtualAddress == 0 || m_pe->pSection[i].SizeOfRawData == 0)
                continue;
            void* pSectionAddress = PIV_PTR_FORWARD(void*, pMemoryAddress, m_pe->pSection[i].VirtualAddress);
            memcpy(pSectionAddress,
                   PIV_PTR_FORWARD(void*, m_pe->pPeBase, m_pe->pSection[i].PointerToRawData), m_pe->pSection[i].SizeOfRawData);
        }
        // 重定位基址和获取各种PE头的地址
        m_pe->bIsMemAlign = TRUE;
        m_pe->pPeBase = pMemoryAddress;
        m_pe->cbPeSize = dwImageSize;
        m_pe->bIsWriteable = TRUE;
        return (CheckHeaders());
    }

    /**
     * @brief 计算IMAGE_THUNK_DATA结构体的尺寸,导入表为空时返回0.
     * @return
     */
    DWORD GetThunkSize()
    {
        if (m_pe->bIs64Pe)
        {
            return 8;
        } // 64位PE肯定是8位长度.
        DWORD dwSize = 0;
        // 因为可能存在单桥结构,所以只读取FirstThunk
        if ((m_pe->pImport + 1)->Name) // 导入表有大于1个模块
        {
            PDWORD pSecondThunk = PIV_PTR_FORWARD(PDWORD, m_pe->pPeBase, GetRealAddress((m_pe->pImport + 1)->FirstThunk));
            dwSize = *(pSecondThunk - 2) ? 4 : 8;
        }
        else // 导入表只有1个模块
        {
            PDWORD pSecondDword = PIV_PTR_FORWARD(PDWORD, m_pe->pPeBase, GetRealAddress(m_pe->pImport->FirstThunk));
            dwSize = *pSecondDword ? 4 : 8;
        }
        m_error = 0;
        return dwSize;
    }

    /**
     * @brief 重建重定位表
     * @return
     */
    BOOL RebuildRelocation()
    {
        PIMAGE_BASE_RELOCATION pRelocation = PIV_PTR_FORWARD(PIMAGE_BASE_RELOCATION,
                                                             m_pe->pPeBase, GetDataDirectoryRva(IMAGE_DIRECTORY_ENTRY_BASERELOC));
        while ((pRelocation->VirtualAddress + pRelocation->SizeOfBlock) != 0)
        {
            WORD* pLocData = PIV_PTR_FORWARD(WORD*, pRelocation, sizeof(IMAGE_BASE_RELOCATION));
            DWORD nNumberOfReloc = (pRelocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
            for (DWORD i = 0; i < nNumberOfReloc; i++)
            {
                // 每个WORD由两部分组成,高4位指出了重定位的类型,WINNT.H中的一系列IMAGE_REL_BASED_xxx定义了重定位类型的取值
                // 低12位是相对于VirtualAddress域的偏移,指出了必须进行重定位的位置
                if (pLocData[i] >> 12 == IMAGE_REL_BASED_DIR64)
                {
                    // 64位dll重定位,IMAGE_REL_BASED_DIR64
                    // 对于IA-64的可执行文件,重定位似乎总是IMAGE_REL_BASED_DIR64类型的
                    if (m_pe->bIs64Pe)
                    {
                        ULONGLONG* pAddress = PIV_PTR_FORWARD(ULONGLONG*, m_pe->pPeBase, pRelocation->VirtualAddress + (pLocData[i] & 0x0FFF));
                        ULONGLONG ullDelta = reinterpret_cast<ULONGLONG>(m_pe->pPeBase) - reinterpret_cast<PIMAGE_NT_HEADERS64>(m_pe->pNtHeader)->OptionalHeader.ImageBase;
                        *pAddress += ullDelta;
                    }
                }
                else if (pLocData[i] >> 12 == IMAGE_REL_BASED_HIGHLOW)
                {
                    // 32位dll重定位,IMAGE_REL_BASED_HIGHLOW
                    // 对于x86的可执行文件,所有的基址重定位都是IMAGE_REL_BASED_HIGHLOW类型的
                    if (m_pe->bIs64Pe == FALSE)
                    {
                        DWORD* pAddress = PIV_PTR_FORWARD(DWORD*, m_pe->pPeBase, pRelocation->VirtualAddress + (pLocData[i] & 0x0FFF));
                        DWORD dwDelta = PIV_PTR_BACKWARD(DWORD, m_pe->pPeBase, m_pe->pNtHeader->OptionalHeader.ImageBase);
                        *pAddress += dwDelta;
                    }
                }
            }
            pRelocation = (PIMAGE_BASE_RELOCATION)((PBYTE)pRelocation + pRelocation->SizeOfBlock);
        }
        if (m_pe->bIs64Pe)
            reinterpret_cast<PIMAGE_NT_HEADERS64>(m_pe->pNtHeader)->OptionalHeader.ImageBase = reinterpret_cast<ULONGLONG>(m_pe->pPeBase);
        else
            m_pe->pNtHeader->OptionalHeader.ImageBase = static_cast<DWORD>(reinterpret_cast<size_t>(m_pe->pPeBase));
        m_error = 0;
        return TRUE;
    }

    /**
     * @brief 重建导入表,将IAT替换成真实的函数地址(VA),进程和PE的位数不一样会导致失败
     * @param vecHmodule
     * @return
     */
    BOOL RebuildImport(std::vector<HMODULE>* vecHmodule)
    {
        PIMAGE_IMPORT_DESCRIPTOR pImport = m_pe->pImport;
        if (m_pe->pImport == nullptr)
        {
            m_error = 16;
            return 0;
        }
        DWORD dwThunkSize = GetThunkSize();
        if (dwThunkSize == 0)
        {
            m_error = 17;
            return FALSE;
        }
        DWORD dwDiff = dwThunkSize == 4 ? 1 : 2;
        HMODULE hModule = NULL;
        UNICODE_STRING ModuleFileName;
        ANSI_STRING FunctionName;
        CVolMem buffer;
        while (pImport->Name != 0)
        {
            PDWORD pRealThunk = PIV_PTR_FORWARD(PDWORD, m_pe->pPeBase, pImport->FirstThunk);
            Nt.RtlInitUnicodeString(&ModuleFileName, PivA2Ws{PIV_PTR_FORWARD(LPCSTR, m_pe->pPeBase, pImport->Name)}.c_str());
            if (NT_SUCCESS(Nt.LdrLoadDll(NULL, 0, &ModuleFileName, &hModule)))
            {
                vecHmodule->push_back(hModule);
            }
            else
            {
                for (size_t i = 0; vecHmodule->size(); i++)
                {
                    Nt.LdrUnloadDll(vecHmodule->at(i));
                }
                vecHmodule->clear();
                m_error = 20;
                return FALSE;
            }
            while (*pRealThunk != 0)
            {
                if (pImport->TimeDateStamp) // 导入表已经被绑定
                    break;
                VOID* lpFunction = nullptr;
                PIMAGE_THUNK_DATA pRealIAT = reinterpret_cast<PIMAGE_THUNK_DATA>(pRealThunk);
                if (pRealIAT->u1.Ordinal & IMAGE_ORDINAL_FLAG)
                {
                    Nt.LdrGetProcedureAddress(hModule, NULL, static_cast<WORD>(pRealIAT->u1.Ordinal & 0x0000FFFF), &lpFunction);
                }
                else
                {
                    PIMAGE_IMPORT_BY_NAME pByName = PIV_PTR_FORWARD(PIMAGE_IMPORT_BY_NAME,
                                                                    m_pe->pPeBase, pRealIAT->u1.AddressOfData);
                    Nt.RtlInitAnsiString(&FunctionName, pByName->Name);
                    Nt.LdrGetProcedureAddress(hModule, &FunctionName, pByName->Hint, &lpFunction);
                }
                if (lpFunction != nullptr)
                {
                    pRealIAT->u1.Function = reinterpret_cast<DWORD_PTR>(lpFunction);
                }
                else
                {
                    for (size_t i = 0; vecHmodule->size(); i++)
                    {
                        Nt.LdrUnloadDll(vecHmodule->at(i));
                    }
                    vecHmodule->clear();
                    m_error = 21;
                    return FALSE;
                }
                pRealThunk = pRealThunk + dwDiff;
            }
            pImport = PIV_PTR_FORWARD(PIMAGE_IMPORT_DESCRIPTOR, pImport, sizeof(IMAGE_IMPORT_DESCRIPTOR));
        }
        m_error = 0;
        return TRUE;
    }

    // 以下函数用于修复PE文件(将内存的PE数据还原成文件)
public:
    /**
     * @brief 修复PE文件
     * @param pMemAddress
     * @return
     */
    BOOL RepairToPeFile(void* pMemAddress)
    {
        if (m_pe == nullptr)
        {
            m_error = 9;
            return FALSE;
        }
        if (m_pe->bIsLoaded == FALSE)
        {
            m_error = 27;
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
        m_error = 0;
        return TRUE;
    }

protected:
    /**
     * @brief 修复导入表,将IAT还原成原样,只有双桥结构的能还原
     * @param pMemAddress
     * @return
     */
    BOOL RepairImport(void* pMemAddress)
    {
        PIMAGE_IMPORT_DESCRIPTOR pImport = m_pe->pImport;
        if (m_pe->pImport == nullptr)
        {
            m_error = 16;
            return FALSE;
        }
        DWORD dwThunkSize = GetThunkSize();
        if (dwThunkSize == 0)
        {
            m_error = 17;
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
                    *(UINT64*)pRealThunk = *(UINT64*)pOriginalThunk;
                }
                pRealThunk = pRealThunk + dwDiff;
                pOriginalThunk = pOriginalThunk + dwDiff;
            }
            pImport = PIV_PTR_FORWARD(PIMAGE_IMPORT_DESCRIPTOR, pImport, sizeof(IMAGE_IMPORT_DESCRIPTOR));
        }
        m_error = 0;
        return TRUE;
    }

    /**
     * @brief 修复重定位表
     * @param pMemAddress
     * @return
     */
    BOOL RepairRelocation(void* pMemAddress)
    {
        PIMAGE_BASE_RELOCATION pRelocation = PIV_PTR_FORWARD(PIMAGE_BASE_RELOCATION,
                                                             m_pe->pPeBase, GetDataDirectoryRva(IMAGE_DIRECTORY_ENTRY_BASERELOC));

        PIMAGE_DOS_HEADER pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(pMemAddress);
        if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        {
            m_error = 4;
            return FALSE;
        }
        PIMAGE_NT_HEADERS32 pNtHeader = PIV_PTR_FORWARD(PIMAGE_NT_HEADERS32, pMemAddress, pDosHeader->e_lfanew);
        if (m_pe->bIs64Pe)
            ((PIMAGE_NT_HEADERS64)pNtHeader)->OptionalHeader.ImageBase = IsDllData() ? 0x0000000180000000 : 0x0000000140000000;
        else
            pNtHeader->OptionalHeader.ImageBase = IsDllData() ? 0x10000000 : 0x400000;
        while ((pRelocation->VirtualAddress + pRelocation->SizeOfBlock) != 0)
        {
            WORD* pLocData = PIV_PTR_FORWARD(WORD*, pRelocation, sizeof(IMAGE_BASE_RELOCATION));
            DWORD nNumberOfReloc = (pRelocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
            for (DWORD i = 0; i < nNumberOfReloc; i++)
            {
                // 每个WORD由两部分组成,高4位指出了重定位的类型,WINNT.H中的一系列IMAGE_REL_BASED_xxx定义了重定位类型的取值
                // 低12位是相对于VirtualAddress域的偏移,指出了必须进行重定位的位置
                if (m_pe->bIs64Pe)
                {
                    if (pLocData[i] >> 12 == IMAGE_REL_BASED_DIR64)
                    {
                        // 64位dll重定位,IMAGE_REL_BASED_DIR64
                        // 对于IA-64的可执行文件,重定位似乎总是IMAGE_REL_BASED_DIR64类型的
                        ULONGLONG* pAddress = PIV_PTR_FORWARD(ULONGLONG*, pMemAddress, pRelocation->VirtualAddress + (pLocData[i] & 0x0FFF));
                        ULONGLONG ullDelta = *pAddress - reinterpret_cast<ULONGLONG>(m_pe->pPeBase) + reinterpret_cast<PIMAGE_NT_HEADERS64>(pNtHeader)->OptionalHeader.ImageBase;
                        *(ULONGLONG*)pAddress = ullDelta;
                    }
                }
                else
                {
                    if (pLocData[i] >> 12 == IMAGE_REL_BASED_HIGHLOW)
                    {
                        // 32位dll重定位,IMAGE_REL_BASED_HIGHLOW
                        // 对于x86的可执行文件,所有的基址重定位都是IMAGE_REL_BASED_HIGHLOW类型的
                        DWORD* pAddress = PIV_PTR_FORWARD(DWORD*, pMemAddress, pRelocation->VirtualAddress + (pLocData[i] & 0x0FFF));
                        DWORD dwDelta = *pAddress - static_cast<DWORD>(reinterpret_cast<size_t>(m_pe->pPeBase)) + pNtHeader->OptionalHeader.ImageBase;
                        *(DWORD*)pAddress = dwDelta;
                    }
                }
            }
            pRelocation = PIV_PTR_FORWARD(PIMAGE_BASE_RELOCATION, pRelocation, pRelocation->SizeOfBlock);
        }
        m_error = 0;
        return TRUE;
    }

    /**
     * @brief 将在内存对齐的PE数据还原
     * @param pMemAddress
     * @return
     */
    BOOL DeAlignPeDatas(void* pMemAddress)
    {
        for (DWORD i = 0; i < m_pe->dwSectionCount; ++i)
        {
            if (m_pe->pSection[i].VirtualAddress == 0 || m_pe->pSection[i].SizeOfRawData == 0)
                continue;
            void* pSectionAddress = PIV_PTR_FORWARD(void*, pMemAddress, m_pe->pSection[i].VirtualAddress);
            memcpy(PIV_PTR_FORWARD(void*, pMemAddress, m_pe->pSection[i].PointerToRawData), pSectionAddress, m_pe->pSection[i].SizeOfRawData);
        }
        m_error = 0;
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
        return m_pe->bIsMemAlign ? dwAddress : Rva2Foa(dwAddress);
    }

    /**
     * @brief 检查文件头,返回是否为有效的PE格式
     * @return
     */
    BOOL CheckHeaders()
    {
        m_pe->pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(m_pe->pPeBase);
        if (m_pe->pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        {
            m_error = 4;
            return FALSE;
        }
        m_pe->pNtHeader = PIV_PTR_FORWARD(PIMAGE_NT_HEADERS32, m_pe->pPeBase, m_pe->pDosHeader->e_lfanew);
        if (m_pe->pNtHeader->Signature != IMAGE_NT_SIGNATURE)
        {
            m_error = 5;
            return FALSE;
        }
        WORD Machine = m_pe->pNtHeader->FileHeader.Machine;
        if (Machine == IMAGE_FILE_MACHINE_AMD64 || Machine == IMAGE_FILE_MACHINE_IA64)
        {
            if (reinterpret_cast<PIMAGE_NT_HEADERS64>(m_pe->pNtHeader)->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
            {
                m_error = 6;
                return FALSE;
            }
            m_pe->bIs64Pe = TRUE;
        }
        else if (Machine == IMAGE_FILE_MACHINE_I386)
        {
            if (m_pe->pNtHeader->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
            {
                m_error = 6;
                return FALSE;
            }
            m_pe->bIs64Pe = FALSE;
        }
        else
        {
            m_error = 7;
            return FALSE;
        }
        m_pe->dwSectionCount = m_pe->pNtHeader->FileHeader.NumberOfSections;
        m_pe->pSection = IMAGE_FIRST_SECTION(m_pe->pNtHeader);
        m_pe->pImport = PIV_PTR_FORWARD(PIMAGE_IMPORT_DESCRIPTOR, m_pe->pPeBase,
                                        GetRealAddress(GetDataDirectoryRva(IMAGE_DIRECTORY_ENTRY_IMPORT)));
        m_pe->pExport = PIV_PTR_FORWARD(PIMAGE_EXPORT_DIRECTORY, m_pe->pPeBase,
                                        GetRealAddress(GetDataDirectoryRva(IMAGE_DIRECTORY_ENTRY_EXPORT)));
        m_error = 0;
        return TRUE;
    }
}; // PivPeFile

/**
 * @brief 内存DLL加载类,继承了PivPeFile
 */
class PivMemLoader : public PivPeFile
{
private:
    typedef BOOL(__stdcall* ProcDllMain)(HINSTANCE, DWORD, LPVOID);

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
    std::vector<HMODULE> m_hmodule_arr;

public:
    /**
     * @brief 卸载内存DLL
     * @return
     */
    VOID MemFreeLibrary()
    {
        if (m_bIsLoadOk == TRUE)
        {
            m_fnDllMain(reinterpret_cast<HINSTANCE>(m_pe->pPeBase), DLL_PROCESS_DETACH, 0);
            for (size_t i = 0; m_hmodule_arr.size(); i++)
            {
                Nt.LdrUnloadDll(m_hmodule_arr[i]);
            }
            m_hmodule_arr.clear();
            Nt.NtFreeVirtualMemory(NULL, &m_pe->pPeBase, NULL, MEM_RELEASE);
            m_pe->pPeBase = nullptr;
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
    BOOL MemLoadLibrary(void* lpFileData, DWORD dwDataLength)
    {
        if (m_bIsLoadOk == TRUE)
        {
            m_error = 22;
            return FALSE;
        }
        if (OpenPeData(lpFileData, dwDataLength, FALSE, FALSE) == FALSE)
            return FALSE;
        // 验证DLL和程序的位数是否一致
#ifdef _WIN64
        if (m_pe->bIs64Pe == FALSE)
        {
            m_error = 8;
            return FALSE;
        }
#else
        if (m_pe->bIs64Pe == TRUE)
        {
            m_error = 8;
            return FALSE;
        }
#endif
        if (IsDllData() == FALSE)
            return FALSE;
        m_hmodule_arr.clear();
        if (AlignPeDatas() == FALSE)
        {
            Nt.NtFreeVirtualMemory(NULL, &m_pe->pPeBase, NULL, MEM_RELEASE);
            m_pe->pPeBase = nullptr;
            return FALSE;
        }
        if (GetDataDirectoryRva(IMAGE_DIRECTORY_ENTRY_BASERELOC) > 0 && GetDataDirectorySize(IMAGE_DIRECTORY_ENTRY_BASERELOC) > 0)
        {
            RebuildRelocation();
        }
        if (RebuildImport(&m_hmodule_arr) == FALSE)
        {
            Nt.NtFreeVirtualMemory(NULL, &m_pe->pPeBase, NULL, MEM_RELEASE);
            m_pe->pPeBase = nullptr;
            return FALSE;
        }
        m_fnDllMain = PIV_PTR_FORWARD(ProcDllMain, m_pe->pPeBase, GetEntryPoint());
        BOOL InitResult = m_fnDllMain(reinterpret_cast<HINSTANCE>(m_pe->pPeBase), DLL_PROCESS_ATTACH, 0);
        if (InitResult == FALSE) // 初始化失败
        {
            m_fnDllMain(reinterpret_cast<HINSTANCE>(m_pe->pPeBase), DLL_PROCESS_DETACH, 0);
            for (size_t i = 0; m_hmodule_arr.size(); i++)
            {
                Nt.LdrUnloadDll(m_hmodule_arr[i]);
            }
            m_hmodule_arr.clear();
            Nt.NtFreeVirtualMemory(NULL, &m_pe->pPeBase, NULL, MEM_RELEASE);
            m_fnDllMain = nullptr;
            m_error = 23;
            return FALSE;
        }
        m_bIsLoadOk = TRUE;
        m_error = 0;
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
            m_error = 24;
            return nullptr;
        }
        DWORD dwOffsetStart = GetDataDirectoryRva(IMAGE_DIRECTORY_ENTRY_EXPORT);
        DWORD dwSize = GetDataDirectorySize(IMAGE_DIRECTORY_ENTRY_EXPORT);
        if (dwOffsetStart == 0 || dwSize == 0)
        {
            m_error = 15;
            return nullptr;
        }
        DWORD dwBase = m_pe->pExport->Base;
        int nNumberOfFunctions = (int)m_pe->pExport->NumberOfFunctions;
        int nNumberOfNames = (int)m_pe->pExport->NumberOfNames;
        LPDWORD pAddressOfFunctions = PIV_PTR_FORWARD(LPDWORD, m_pe->pPeBase, m_pe->pExport->AddressOfFunctions);
        LPWORD pAddressOfOrdinals = PIV_PTR_FORWARD(LPWORD, m_pe->pPeBase, m_pe->pExport->AddressOfNameOrdinals);
        LPDWORD pAddressOfNames = PIV_PTR_FORWARD(LPDWORD, m_pe->pPeBase, m_pe->pExport->AddressOfNames);
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
                char* szName = PIV_PTR_FORWARD(char*, m_pe->pPeBase, pAddressOfNames[i]);
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
            m_error = 25;
            return nullptr;
        }
        else
        {
            DWORD pFunctionOffset = pAddressOfFunctions[nOrdinal];
            if (pFunctionOffset > dwOffsetStart && pFunctionOffset < (dwOffsetStart + dwSize))
            {
                m_error = 26;
                return nullptr;
            }
            else
            {
                m_error = 0;
                return PIV_PTR_FORWARD(FARPROC, m_pe->pPeBase, pFunctionOffset);
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
