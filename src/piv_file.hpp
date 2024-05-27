/*********************************************\
 * 火山视窗PIV模块 - 内存映射文件类          *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef PIV_FILE_HPP
#define PIV_FILE_HPP

#include "detail/piv_base.hpp"
#include "piv_encoding.hpp"

class PivFile
{
private:
    bool m_AutoClose = true;
    HANDLE hFile = INVALID_HANDLE_VALUE;

public:
    PivFile() {}
    ~PivFile()
    {
        if (m_AutoClose)
            Close();
    }

    inline PivFile &operator=(PivFile &&rhs)
    {
        m_AutoClose = rhs.m_AutoClose;
        hFile = rhs.hFile;
    }

    /**
     * @brief 打开文件
     * @param lpFileName 文件名称
     * @param nDesiredAccess 访问权限
     * @param nShareMode 共享方式
     * @param nCreationDisposition 创建方式
     * @return 是否成功
     */
    BOOL Open(const wchar_t *lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition)
    {
        Close();
        hFile = ::CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
        m_AutoClose = true;
        return (INVALID_HANDLE_VALUE != hFile);
    }

    /**
     * @brief 打开文件句柄(类析构时不会自动关闭文件)
     * @param file 外部文件句柄
     * @return 是否成功
     */
    BOOL Open(HANDLE file)
    {
        Close();
        if (::GetFileType(file) == FILE_TYPE_DISK)
        {
            hFile = file;
            m_AutoClose = false;
            return TRUE;
        }
        return FALSE;
    }

    /**
     * @brief 关闭文件
     */
    inline void Close()
    {
        if (INVALID_HANDLE_VALUE != hFile)
        {
            ::CloseHandle(hFile);
            hFile = INVALID_HANDLE_VALUE;
        }
    }

    /**
     * @brief 是否已打开
     * @return 是否打开
     */
    inline BOOL IsOpen() const
    {
        return (INVALID_HANDLE_VALUE != hFile);
    }

    /**
     * @brief 交换
     * @param rhs 另一个对象
     */
    inline void Swap(PivFile &rhs)
    {
        std::swap(m_AutoClose, rhs.m_AutoClose);
        std::swap(hFile, rhs.hFile);
    }

    /**
     * @brief 取文件句柄
     * @return
     */
    inline HANDLE GetFileHandle() const
    {
        return hFile;
    }

    /**
     * @brief 取文件长度
     * @return 失败返回-1
     */
    inline int64_t GetSize() const
    {
        LARGE_INTEGER fileSize{0};
        if (INVALID_HANDLE_VALUE != hFile && ::GetFileSizeEx(hFile, &fileSize))
            return fileSize.QuadPart;
        return -1;
    }

    /**
     * @brief 置文件长度
     * @param file_size 欲设置的长度
     * @return 是否成功
     */
    inline BOOL SetFileSize(int64_t file_size)
    {
        if (INVALID_HANDLE_VALUE != hFile)
        {
            LARGE_INTEGER pos{0};
            pos.QuadPart = file_size;
            if (::SetFilePointerEx(hFile, pos, NULL, FILE_BEGIN) && ::SetEndOfFile(hFile))
                return TRUE;
        }
        return FALSE;
    }

    /**
     * @brief 刷新文件
     * @return 是否成功
     */
    inline BOOL Flush()
    {
        return (INVALID_HANDLE_VALUE == hFile) ? FALSE : ::FlushFileBuffers(hFile);
    }

    /**
     * @brief 锁住文件
     * @param offset 欲加锁数据的偏移位置
     * @param lock_size 欲加锁数据的尺寸
     * @param try_time 加锁重试时间
     * @return 是否成功
     */
    BOOL Lock(int64_t offset, int64_t lock_size, int32_t try_time)
    {
        if (INVALID_HANDLE_VALUE == hFile)
            return FALSE;
        const DWORD dwBeginTickCount = ::GetTickCount();
        while (true)
        {
            if (::LockFile(hFile, (DWORD)offset, (DWORD)(offset >> 32), (DWORD)lock_size, (DWORD)(lock_size >> 32)))
                return TRUE;
            const DWORD dwLastError = ::GetLastError();
            if ((dwLastError != ERROR_SHARING_VIOLATION && dwLastError != ERROR_LOCK_VIOLATION) ||
                try_time == 0 || (try_time > 0 && ::GetTickCount() - dwBeginTickCount > (DWORD)try_time))
                break;
            Sleep(20);
        }
        return FALSE;
    }

    /**
     * @brief 解锁文件
     * @param offset 欲解锁数据的偏移位置
     * @param lock_size 欲解锁数据的尺寸
     * @return 是否成功
     */
    inline BOOL Unlock(int64_t offset, int64_t lock_size)
    {
        if (INVALID_HANDLE_VALUE != hFile)
            return ::UnlockFile(hFile, (DWORD)offset, (DWORD)(offset >> 32), (DWORD)lock_size, (DWORD)(lock_size >> 32));
        return FALSE;
    }

    /**
     * @brief 移到文件首
     * @param n64Offset 移动距离
     * @param dwMoveMethod 基准移动位置
     * @return 是否成功
     */
    BOOL SetCurrentPos(int64_t n64Offset, DWORD dwMoveMethod = 0)
    {
        if (INVALID_HANDLE_VALUE == hFile)
            return FALSE;
        LARGE_INTEGER pos{0};
        pos.QuadPart = n64Offset;
        return ::SetFilePointerEx(hFile, pos, NULL, dwMoveMethod);
    }

    /**
     * @brief 取读写位置
     * @return
     */
    inline int64_t GetCurrentPos()
    {
        return (hFile == INVALID_HANDLE_VALUE) ? -1 : MoveAndGetFilePos(FILE_CURRENT);
    }

    /**
     * @brief 置文件尾
     * @return
     */
    inline BOOL SetEof()
    {
        return (hFile == INVALID_HANDLE_VALUE) ? FALSE : ::SetEndOfFile(hFile);
    }

    /**
     * @brief 是否在文件尾
     * @param isTextFile 是否为判断文本已读完
     * @return
     */
    BOOL IsEof(BOOL isTextFile, BOOL isUnicode)
    {
        if (hFile == INVALID_HANDLE_VALUE)
            return TRUE;
        const int64_t n64CurrentPosition = this->GetCurrentPos();
        if (n64CurrentPosition < 0)
            return TRUE;
        const int64_t n64FileLength = this->MoveAndGetFilePos(FILE_END);
        if (n64FileLength < 0)
            return TRUE;
        this->SetCurrentPos(n64CurrentPosition);
        if (n64CurrentPosition >= n64FileLength)
            return TRUE;
        if (isTextFile == FALSE)
            return FALSE;
        const BOOL blpEnd = ((isUnicode ? this->GetWchar() : this->GetChar()) == -1);
        this->SetCurrentPos(n64CurrentPosition);
        return blpEnd;
    }

    /**
     * @brief 移动及获取读写位置
     * @param dwMoveMethod 基准移动位置
     * @return
     */
    int64_t MoveAndGetFilePos(DWORD dwMoveMethod)
    {
        if (INVALID_HANDLE_VALUE == hFile)
            return -1;
        LARGE_INTEGER dis, current_pos;
        dis.QuadPart = current_pos.QuadPart = 0;
        return (::SetFilePointerEx(hFile, dis, &current_pos, dwMoveMethod) ? current_pos.QuadPart : -1);
    }

    /**
     * @brief 读
     * @tparam T 数据类型
     * @param default 缺省值
     * @return
     */
    template <typename T>
    T Get(const T &default_value)
    {
        if (INVALID_HANDLE_VALUE != hFile)
        {
            T value;
            DWORD dwRead = 0;
            if (::ReadFile(hFile, &value, (DWORD)sizeof(T), &dwRead, NULL) && dwRead == sizeof(T))
                return value;
            ::SetFilePointer(hFile, 0, NULL, FILE_END);
        }
        return default_value;
    }

    /**
     * @brief 读字符
     * @return
     */
    int32_t GetChar()
    {
        if (INVALID_HANDLE_VALUE != hFile)
        {
            char value;
            if (::ReadFile(hFile, &value, 1, NULL, NULL))
                return static_cast<int32_t>(value);
            ::SetFilePointer(hFile, 0, NULL, FILE_END);
        }
        return -1;
    }

    /**
     * @brief 读宽字符
     * @return
     */
    int32_t GetWchar()
    {
        if (INVALID_HANDLE_VALUE != hFile)
        {
            char value;
            DWORD dwRead = 0;
            if (::ReadFile(hFile, &value, 2, &dwRead, NULL) && dwRead == 2)
                return static_cast<int32_t>(value);
            ::SetFilePointer(hFile, 0, NULL, FILE_END);
        }
        return -1;
    }

    /**
     * @brief 读入数据
     * @tparam T 数据类型
     * @param value 所读入数据的变量3
     * @return
     */
    template <typename T>
    int32_t Read(T &value)
    {
        if (INVALID_HANDLE_VALUE != hFile)
        {
            DWORD dwRead = 0;
            if (::ReadFile(hFile, &value, (DWORD)sizeof(T), &dwRead, NULL) && dwRead == sizeof(T))
                return static_cast<int32_t>(dwRead);
            ::SetFilePointer(hFile, 0, NULL, FILE_END);
        }
        return 0;
    }

    /**
     * @brief 写出数据
     * @tparam T 数据类型
     * @param value 所欲写出的数据
     * @return
     */
    template <typename T>
    int32_t Write(const T &value)
    {
        if (INVALID_HANDLE_VALUE != hFile)
        {
            DWORD dwWritten = 0;
            if (::WriteFile(hFile, &value, (DWORD)sizeof(T), &dwWritten, NULL))
                return static_cast<int32_t>(dwWritten);
        }
        return 0;
    }

    /**
     * @brief 读指针数据
     * @param pData 数据指针
     * @param npDataSize 数据尺寸
     * @return
     */
    int64_t ReadData(void *pData, ptrdiff_t npDataSize)
    {
        if (INVALID_HANDLE_VALUE != hFile && npDataSize > 0)
        {
            DWORD dwRead = 0;
            if (::ReadFile(hFile, pData, (DWORD)npDataSize, &dwRead, NULL))
                return static_cast<int64_t>(dwRead);
            ::SetFilePointer(hFile, 0, NULL, FILE_END);
        }
        return 0;
    }

    /**
     * @brief 写指针数据
     * @param pData 数据指针
     * @param npDataSize 数据尺寸
     * @return
     */
    int64_t WriteData(const void *pData, ptrdiff_t npDataSize)
    {
        if (INVALID_HANDLE_VALUE != hFile && npDataSize > 0)
        {
            DWORD dwWritten = 0;
            if (::WriteFile(hFile, pData, (DWORD)npDataSize, &dwWritten, NULL))
                return static_cast<int64_t>(dwWritten);
        }
        return 0;
    }

    /**
     * @brief 读入字节集
     * @param data 存放数据的字节集
     * @param npDataSize 欲读入数据的尺寸
     * @return 实际读取长度
     */
    int64_t ReadVolMem(CVolMem &data, ptrdiff_t npDataSize)
    {
        if (INVALID_HANDLE_VALUE != hFile && npDataSize > 0)
        {
            DWORD dwRead = 0;
            if (::ReadFile(hFile, data.Alloc(npDataSize), (DWORD)npDataSize, &dwRead, NULL))
            {
                data.Realloc(static_cast<INT_P>(dwRead));
                return static_cast<int64_t>(dwRead);
            }
            ::SetFilePointer(hFile, 0, NULL, FILE_END);
        }
        return 0;
    }

    /**
     * @brief 写出字节集
     * @param data 欲写出的字节集数据
     * @return 实际写出长度
     */
    inline int64_t WriteVolMem(const CVolMem &data)
    {
        return this->WriteData(data.GetPtr(), data.GetSize());
    }

    /**
     * @brief 读入文本
     * @param text 存放读入文本的变量
     * @param read_words 欲读入的最多字符数
     * @param encode 文本编码
     * @return 读入的字节长度
     */
    int64_t ReadTextWithEncode(CVolString &text, int64_t read_words = -1, VOL_STRING_ENCODE_TYPE encode = VSET_UTF_16)
    {
        text.Empty();
        if (INVALID_HANDLE_VALUE == hFile || read_words == 0)
            return 0;
        int64_t ret;
        switch (encode)
        {
        case VSET_UTF_8:
        {
            std::string str;
            if ((ret = this->ReadText(str, read_words)) > 0)
                piv::encoding::U2Wex(text, str.c_str(), str.size());
            break;
        }
        case VSET_MBCS:
        {
            std::string str;
            if ((ret = this->ReadText(str, read_words)) > 0)
                piv::encoding::A2Wex(text, str.c_str(), str.size(), 0);
            break;
        }
        default:
        {
            ret = this->ReadText(text, read_words);
            break;
        }
        }
        return ret;
    }

    template <typename CharT>
    int64_t ReadText(std::basic_string<CharT> &text, int64_t read_words = -1)
    {
        text.clear();
        if (INVALID_HANDLE_VALUE == hFile || read_words == 0)
            return 0;

        int64_t CurrentPos = this->GetCurrentPos();
        if (CurrentPos == 0) // 判断跳过BOM签名
        {
            if (sizeof(CharT) == 2)
            {
                wchar_t bom = 0;
                if (::ReadFile(hFile, &bom, 2, NULL, NULL) && bom == 0xFEFF)
                    CurrentPos = 2;
            }
            else
            {
                char bom[3]{0};
                if (::ReadFile(hFile, bom, 3, NULL, NULL) && bom[0] == (char)0xEF && bom[1] == (char)0xBB && bom[2] == (char)0xBF)
                    CurrentPos = 3;
            }
            this->SetCurrentPos(CurrentPos, FILE_BEGIN);
        }

        int64_t ReadSize = 0;
        if (read_words < 0)
        {
            ReadSize = this->MoveAndGetFilePos(FILE_END) - CurrentPos;
            this->SetCurrentPos(CurrentPos, FILE_BEGIN);
        }
        else
        {
            ReadSize = read_words * sizeof(CharT);
        }

        DWORD dwRead;
        PivBuffer<CharT, DWORD> buf{static_cast<DWORD>((ReadSize > 0x10000) ? 0x10000 : (ReadSize / sizeof(CharT))), true};
        for (int32_t i = 0; i < ReadSize / 0x10000; i++)
        {
            if (::ReadFile(hFile, buf.GetPtr(), buf.GetSize(), &dwRead, NULL))
            {
                const CharT *ps = buf.GetPtr();
                for (DWORD n = 0; n < buf.GetCount(); n++)
                {
                    if (ps[n] == '\0' || ps[n] == 0x1A)
                    {
                        text.append(buf.GetPtr(), n);
                        this->SetCurrentPos(CurrentPos + text.size() * sizeof(CharT), FILE_BEGIN);
                        return static_cast<int64_t>(text.size() * sizeof(CharT));
                    }
                }
                text.append(buf.GetPtr(), buf.GetCount());
            }
        }
        if (ReadSize % 0x10000 != 0)
        {
            buf.Realloc(static_cast<DWORD>((ReadSize % 0x10000) / sizeof(CharT)), true);
            if (::ReadFile(hFile, buf.GetPtr(), buf.GetSize(), &dwRead, NULL))
            {
                const CharT *ps = buf.GetPtr();
                for (DWORD n = 0; n < buf.GetCount(); n++)
                {
                    if (ps[n] == '\0' || ps[n] == 0x1A)
                    {
                        text.append(buf.GetPtr(), n);
                        this->SetCurrentPos(CurrentPos + text.size() * sizeof(CharT), FILE_BEGIN);
                        return static_cast<int64_t>(text.size() * sizeof(CharT));
                    }
                }
                text.append(buf.GetPtr(), buf.GetCount());
            }
        }
        this->SetCurrentPos(CurrentPos + text.size() * sizeof(CharT), FILE_BEGIN);
        return static_cast<int64_t>(text.size() * sizeof(CharT));
    }

    int64_t ReadText(CVolString &text, int64_t read_words = -1)
    {
        text.Empty();
        if (INVALID_HANDLE_VALUE == hFile || read_words == 0)
            return 0;

        int64_t CurrentPos = this->GetCurrentPos();
        if (CurrentPos == 0) // 判断跳过BOM签名
        {
            wchar_t bom = 0;
            if (::ReadFile(hFile, &bom, 2, NULL, NULL) && bom == 0xFEFF)
                CurrentPos = 2;
            this->SetCurrentPos(CurrentPos, FILE_BEGIN);
        }

        int64_t ReadSize = 0;
        if (read_words < 0)
        {
            ReadSize = this->MoveAndGetFilePos(FILE_END) - CurrentPos;
            this->SetCurrentPos(CurrentPos, FILE_BEGIN);
        }
        else
        {
            ReadSize = read_words * 2;
        }

        DWORD dwRead;
        PivBuffer<wchar_t, DWORD> buf{static_cast<DWORD>((ReadSize > 0x10000) ? 0x10000 : (ReadSize / 2)), true};
        for (int32_t i = 0; i < ReadSize / 0x10000; i++)
        {
            if (::ReadFile(hFile, buf.GetPtr(), buf.GetSize(), &dwRead, NULL))
            {
                const wchar_t *ps = buf.GetPtr();
                for (DWORD i = 0; i < buf.GetCount(); i++)
                {
                    if (ps[i] == '\0' || ps[i] == 0x1A)
                    {
                        text.AddText(buf.GetPtr(), i);
                        this->SetCurrentPos(CurrentPos + text.GetLength() * 2, FILE_BEGIN);
                        return static_cast<int64_t>(text.GetLength() * 2);
                    }
                }
                text.AddText(buf.GetPtr(), buf.GetCount());
            }
        }
        if (ReadSize % 0x10000 != 0)
        {
            buf.Realloc(static_cast<DWORD>((ReadSize % 0x10000) / 2), true);
            if (::ReadFile(hFile, buf.GetPtr(), buf.GetSize(), &dwRead, NULL))
            {
                const wchar_t *ps = buf.GetPtr();
                for (DWORD i = 0; i < buf.GetCount(); i++)
                {
                    if (ps[i] == '\0' || ps[i] == 0x1A)
                    {
                        text.AddText(buf.GetPtr(), i);
                        this->SetCurrentPos(CurrentPos + text.GetLength() * 2, FILE_BEGIN);
                        return static_cast<int64_t>(text.GetLength() * 2);
                    }
                }
                text.AddText(buf.GetPtr(), buf.GetCount());
            }
        }
        this->SetCurrentPos(CurrentPos + text.GetLength() * 2, FILE_BEGIN);
        return static_cast<int64_t>(text.GetLength() * 2);
    }

    /**
     * @brief 读入一行
     * @param text 存放读入文本的变量
     * @param encode 文本编码
     * @return 读入的字节长度
     */
    int64_t ReadLineWithEncode(CVolString &text, VOL_STRING_ENCODE_TYPE encode = VSET_UTF_16)
    {
        int64_t ret = 0;
        text.Empty();
        switch (encode)
        {
        case VSET_UTF_8:
        {
            std::string str;
            if ((ret = this->ReadLine(str)) > 0)
                piv::encoding::U2Wex(text, str.c_str(), str.size());
            break;
        }
        case VSET_MBCS:
        {
            std::string str;
            if ((ret = this->ReadLine(str)) > 0)
                piv::encoding::A2Wex(text, str.c_str(), str.size(), 0);
            break;
        }
        default:
        {
            ret = this->ReadLine(text);
            break;
        }
        }
        return ret;
    }

    template <typename CharT>
    int64_t ReadLine(std::basic_string<CharT> &text)
    {
        text.clear();
        if (INVALID_HANDLE_VALUE == hFile)
            return 0;

        int64_t CurrentPos = this->GetCurrentPos();
        if (CurrentPos == 0) // 判断跳过BOM签名
        {
            if (sizeof(CharT) == 2)
            {
                wchar_t bom = 0;
                if (::ReadFile(hFile, &bom, 2, NULL, NULL) && bom == 0xFEFF)
                    CurrentPos = 2;
            }
            else
            {
                char bom[3]{0};
                if (::ReadFile(hFile, bom, 3, NULL, NULL) && bom[0] == (char)0xEF && bom[1] == (char)0xBB && bom[2] == (char)0xBF)
                    CurrentPos = 3;
            }
        }
        int64_t ReadSize = this->MoveAndGetFilePos(FILE_END) - CurrentPos;
        this->SetCurrentPos(CurrentPos, FILE_BEGIN);

        int64_t TotalRead = 0;
        DWORD dwRead;
        PivBuffer<CharT, DWORD> buf{static_cast<DWORD>((ReadSize > 0x10000) ? 0x10000 : (ReadSize / 2)), true};
        for (int32_t i = 0; i < ReadSize / 0x10000; i++)
        {
            if (::ReadFile(hFile, buf.GetPtr(), buf.GetSize(), &dwRead, NULL))
            {
                const CharT *ps = buf.GetPtr();
                bool hasCRLF = false;
                DWORD n;
                for (n = 0; n < buf.GetCount(); n++)
                {
                    if (ps[n] == '\0' || ps[n] == 0x1A || ps[n] == '\n')
                    {
                        hasCRLF = true;
                        TotalRead += (n + 1) * sizeof(CharT);
                        break;
                    }
                    if (ps[n] == '\r')
                    {
                        hasCRLF = true;
                        if (n + 1 < buf.GetCount() && ps[n + 1] == '\n')
                            TotalRead += (n + 2) * sizeof(CharT);
                        else
                            TotalRead += (n + 1) * sizeof(CharT);
                        break;
                    }
                }
                if (hasCRLF)
                {
                    text.append(buf.GetPtr(), n);
                    this->SetCurrentPos(CurrentPos + TotalRead, FILE_BEGIN);
                    return TotalRead;
                }
                else
                {
                    TotalRead += dwRead;
                    text.append(buf.GetPtr(), buf.GetCount());
                }
            }
        }
        if (ReadSize % 0x10000 != 0)
        {
            buf.Realloc(static_cast<DWORD>((ReadSize % 0x10000) / 2), true);
            if (::ReadFile(hFile, buf.GetPtr(), buf.GetSize(), &dwRead, NULL))
            {
                const CharT *ps = buf.GetPtr();
                bool hasCRLF = false;
                DWORD n;
                for (n = 0; n < buf.GetCount(); n++)
                {
                    if (ps[n] == '\0' || ps[n] == 0x1A || ps[n] == '\n')
                    {
                        hasCRLF = true;
                        TotalRead += (n + 1) * sizeof(CharT);
                        break;
                    }
                    if (ps[n] == '\r')
                    {
                        hasCRLF = true;
                        if (n + 1 < buf.GetCount() && ps[n + 1] == '\n')
                            TotalRead += (n + 2) * sizeof(CharT);
                        else
                            TotalRead += (n + 1) * sizeof(CharT);
                        break;
                    }
                }
                if (hasCRLF)
                {
                    text.append(buf.GetPtr(), n);
                    this->SetCurrentPos(CurrentPos + TotalRead, FILE_BEGIN);
                    return TotalRead;
                }
                else
                {
                    TotalRead += dwRead;
                    text.append(buf.GetPtr(), buf.GetCount());
                }
            }
        }
        this->SetCurrentPos(CurrentPos + TotalRead, FILE_BEGIN);
        return TotalRead;
    }

    int64_t ReadLine(CVolString &text)
    {
        text.Empty();
        if (INVALID_HANDLE_VALUE == hFile)
            return 0;

        int64_t CurrentPos = this->GetCurrentPos();
        if (CurrentPos == 0) // 判断跳过BOM签名
        {
            wchar_t bom = 0;
            if (::ReadFile(hFile, &bom, 2, NULL, NULL) && bom == 0xFEFF)
                CurrentPos = 2;
        }
        int64_t ReadSize = this->MoveAndGetFilePos(FILE_END) - CurrentPos;
        this->SetCurrentPos(CurrentPos, FILE_BEGIN);

        int64_t TotalRead = 0;
        DWORD dwRead;
        PivBuffer<wchar_t, DWORD> buf{static_cast<DWORD>((ReadSize > 0x10000) ? 0x10000 : (ReadSize / 2)), true};
        for (int32_t i = 0; i < ReadSize / 0x10000; i++)
        {
            if (::ReadFile(hFile, buf.GetPtr(), buf.GetSize(), &dwRead, NULL))
            {
                const wchar_t *ps = buf.GetPtr();
                bool hasCRLF = false;
                DWORD n;
                for (n = 0; n < buf.GetCount(); n++)
                {
                    if (ps[n] == '\0' || ps[n] == 0x1A || ps[n] == '\n')
                    {
                        hasCRLF = true;
                        TotalRead += (n + 1) * 2;
                        break;
                    }
                    if (ps[n] == '\r')
                    {
                        hasCRLF = true;
                        if (n + 1 < buf.GetCount() && ps[n + 1] == '\n')
                            TotalRead += (n + 2) * 2;
                        else
                            TotalRead += (n + 1) * 2;
                        break;
                    }
                }
                if (hasCRLF)
                {
                    text.AddText(buf.GetPtr(), n);
                    this->SetCurrentPos(CurrentPos + TotalRead, FILE_BEGIN);
                    return TotalRead;
                }
                else
                {
                    TotalRead += dwRead;
                    text.AddText(buf.GetPtr(), buf.GetCount());
                }
            }
        }
        if (ReadSize % 0x10000 != 0)
        {
            buf.Realloc(static_cast<DWORD>((ReadSize % 0x10000) / 2), true);
            if (::ReadFile(hFile, buf.GetPtr(), buf.GetSize(), &dwRead, NULL))
            {
                const wchar_t *ps = buf.GetPtr();
                bool hasCRLF = false;
                DWORD n;
                for (n = 0; n < buf.GetCount(); n++)
                {
                    if (ps[n] == '\0' || ps[n] == 0x1A || ps[n] == '\n')
                    {
                        hasCRLF = true;
                        TotalRead += (n + 1) * 2;
                        break;
                    }
                    if (ps[n] == '\r')
                    {
                        hasCRLF = true;
                        if (n + 1 < buf.GetCount() && ps[n + 1] == '\n')
                            TotalRead += (n + 2) * 2;
                        else
                            TotalRead += (n + 1) * 2;
                        break;
                    }
                }
                if (hasCRLF)
                {
                    text.AddText(buf.GetPtr(), n);
                    this->SetCurrentPos(CurrentPos + TotalRead, FILE_BEGIN);
                    return TotalRead;
                }
                else
                {
                    TotalRead += dwRead;
                    text.AddText(buf.GetPtr(), buf.GetCount());
                }
            }
        }
        this->SetCurrentPos(CurrentPos + TotalRead, FILE_BEGIN);
        return TotalRead;
    }

    /**
     * @brief 写出文本
     * @param str 欲写出的文本
     * @param null_terminated 是否包括结束零字符
     * @param encode 文本编码类型
     * @return
     */
    int64_t WriteText(const wchar_t *str, BOOL null_terminated = FALSE, VOL_STRING_ENCODE_TYPE encode = VSET_UTF_16)
    {
        if (INVALID_HANDLE_VALUE == hFile)
            return 0;
        switch (encode)
        {
        case VSET_UTF_8:
        {
            PivW2U u8{str};
            return this->WriteData(u8.GetText(), u8.GetSize() + null_terminated ? 1 : 0);
            break;
        }
        case VSET_MBCS:
        {
            PivW2A mbcs{str};
            return this->WriteData(mbcs.GetText(), mbcs.GetSize() + null_terminated ? 1 : 0);
            break;
        }
        default:
        {
            return this->WriteData(str, (wcslen(str) + null_terminated ? 1 : 0) * 2);
            break;
        }
        }
        return 0;
    }

    /**
     * @brief 写出一行
     * @param str 欲写出的文本
     * @param encode 文本编码类型
     * @return
     */
    int64_t WriteLine(const wchar_t *str, VOL_STRING_ENCODE_TYPE encode = VSET_UTF_16)
    {
        if (INVALID_HANDLE_VALUE == hFile)
            return 0;
        int64_t ret = this->WriteText(str, FALSE, encode);
        if (encode == VSET_UTF_16)
        {
            ret += this->Write<char>('\r');
            ret += this->Write<char>('\n');
        }
        else
        {
            ret += this->Write<wchar_t>('\r');
            ret += this->Write<wchar_t>('\n');
        }
        return ret;
    }

    /**
     * @brief 删除数据
     * @param offset 起始位置
     * @param length 删除长度
     * @return
     */
    BOOL RemoveData(int64_t offset, int64_t length)
    {
        if (INVALID_HANDLE_VALUE == hFile || length <= 0)
            return FALSE;
        int64_t file_size = this->GetSize();
        if (offset < 0)
            offset = this->GetCurrentPos();
        if (offset > file_size)
            return FALSE;
        if (offset + length >= file_size)
            return this->SetFileSize(offset);
        else
        {
            int64_t read_off = offset + length;
            int64_t data_size = file_size - read_off;
            PivBuffer<BYTE, DWORD> buf{static_cast<DWORD>((data_size > 0x10000) ? 0x10000 : data_size), true};
            for (DWORD i = 0; i < data_size / 0x10000; i++)
            {
                read_off += 0x10000 * i;
                if (!this->SetCurrentPos(read_off, FILE_BEGIN) || this->ReadData(buf.GetPtr(), buf.GetSize()) <= 0)
                    return FALSE;
                if (!this->SetCurrentPos(offset + 0x10000 * i, FILE_BEGIN) || this->WriteData(buf.GetPtr(), buf.GetSize()) <= 0)
                    return FALSE;
            }
            if (data_size % 0x10000 != 0)
            {
                buf.Realloc(static_cast<DWORD>(data_size % 0x10000), true);
                if (!this->SetCurrentPos(read_off, FILE_BEGIN) || this->ReadData(buf.GetPtr(), buf.GetSize()) <= 0)
                    return FALSE;
                if (!this->SetCurrentPos(offset + 0x10000 * (data_size / 0x10000), FILE_BEGIN) || this->WriteData(buf.GetPtr(), buf.GetSize()) <= 0)
                    return FALSE;
            }
            if (::SetEndOfFile(hFile))
                return TRUE;
        }
        return FALSE;
    }
}; // PivFile

/**
 * @brief 内存映射文件类
 */
class PivFileMapping
{
private:
    BYTE *m_pvFile = nullptr; // 映射文件视图地址
    HANDLE m_hFileMap = NULL; // 内存映射文件对象
    int64_t m_offset = -1;    // 当前读写偏移位置
    int64_t m_MapSize = 0;    // 内存映射文件大小
    int64_t m_ViewSize = 0;   // 映射文件视图大小
    PivFile m_flie;           // 文件读写类

public:
    PivFileMapping() {}
    ~PivFileMapping()
    {
        Close();
    }

    inline PivFileMapping &operator=(PivFileMapping &&rhs)
    {
        m_pvFile = rhs.m_pvFile;
        m_hFileMap = rhs.m_hFileMap;
        m_offset = rhs.m_offset;
        m_MapSize = rhs.m_MapSize;
        m_ViewSize = rhs.m_ViewSize;
        m_flie = std::move(rhs.m_flie);
    }

    /**
     * @brief 创建映射内存
     * @param lpFileName 文件名称
     * @param dwDesiredAccess 访问权限
     * @param dwShareMode 共享方式
     * @param dwCreationDisposition 创建方式
     * @param n64Size 映射尺寸
     * @param dwProtect 内存页保护
     * @param lpName 内存映射名称
     * @return
     */
    BOOL Create(const wchar_t *lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, DWORD dwCreationDisposition, int64_t n64Size, DWORD dwProtect, const wchar_t *lpName)
    {
        Close();
        if (dwCreationDisposition != -1)
        {
            if (!m_flie.Open(lpFileName, dwDesiredAccess, dwShareMode, dwCreationDisposition))
                return FALSE;
        }
        m_hFileMap = ::CreateFileMappingW(m_flie.GetFileHandle(), NULL, dwProtect, (DWORD)(n64Size >> 32), (DWORD)n64Size, lpName);
        if (NULL != m_hFileMap)
        {
            m_MapSize = (n64Size != 0) ? n64Size : m_flie.GetSize();
            return TRUE;
        }
        return FALSE;
    }

    /**
     * @brief 创建自文件句柄
     * @param hFile 文件句柄
     * @param n64Size 映射尺寸
     * @param dwProtect 内存页保护
     * @param lpName 内存映射名称
     * @return
     */
    BOOL Create(HANDLE hFile, int64_t n64Size, DWORD dwProtect, const wchar_t *lpName)
    {
        Close();
        if (m_flie.Open(hFile) == FALSE)
            return FALSE;
        m_hFileMap = ::CreateFileMappingW(hFile, NULL, dwProtect, (DWORD)(n64Size >> 32), (DWORD)n64Size, lpName);
        if (NULL != m_hFileMap)
        {
            if (0 == (m_MapSize = n64Size))
                m_MapSize = m_flie.GetSize();
            return TRUE;
        }
        return FALSE;
    }

    /**
     * @brief 创建映射内存
     * @param n64Size 映射尺寸
     * @param dwProtect 内存页保护
     * @param lpName 内存映射名称
     * @return
     */
    BOOL Create(int64_t n64Size, DWORD dwProtect, const wchar_t *lpName)
    {
        Close();
        m_hFileMap = ::CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, dwProtect, (DWORD)(n64Size >> 32), (DWORD)n64Size, lpName);
        if (NULL != m_hFileMap)
        {
            m_MapSize = n64Size;
            return TRUE;
        }
        return FALSE;
    }

    /**
     * @brief 打开映射文件
     * @param lpName 内存映射名称
     * @return
     */
    BOOL Open(const wchar_t *lpName, DWORD dwDesiredAccess)
    {
        Close();
        m_hFileMap = ::OpenFileMappingW(dwDesiredAccess, FALSE, lpName);
        return (NULL != m_hFileMap);
    }

    /**
     * @brief 关闭映射文件
     * @return
     */
    void Close()
    {
        UnMapToMemory(); // 取消映射
        if (NULL != m_hFileMap)
        {
            ::CloseHandle(m_hFileMap); // 关闭文件映射
            m_hFileMap = NULL;
        }
        m_flie.Close(); // 关闭文件
        m_offset = -1;
        m_ViewSize = 0;
        m_MapSize = 0;
    }

    /**
     * @brief 是否已打开
     * @return
     */
    inline BOOL IsOpen() const
    {
        return (NULL != m_hFileMap);
    }

    /**
     * @brief 交换
     * @param rhs 另一个对象
     */
    inline void Swap(PivFileMapping &rhs)
    {
        std::swap(m_pvFile, rhs.m_pvFile);
        std::swap(m_hFileMap, rhs.m_hFileMap);
        std::swap(m_offset, rhs.m_offset);
        std::swap(m_MapSize, rhs.m_MapSize);
        std::swap(m_ViewSize, rhs.m_ViewSize);
        m_flie.Swap(rhs.m_flie);
    }

    /**
     * @brief 取文件句柄
     * @return
     */
    inline HANDLE GetFileHandle() const
    {
        return m_flie.GetFileHandle();
    }

    /**
     * @brief 映射到内存
     * @param n64FileOffset 起始位置
     * @param n64Size 映射尺寸
     * @return
     */
    BOOL MapToMemory(int64_t n64FileOffset, int64_t n64Size, DWORD dwProtect)
    {
        if (NULL == m_hFileMap)
            return FALSE;
        UnMapToMemory(); // 取消映射
        m_pvFile = reinterpret_cast<BYTE *>(::MapViewOfFile(m_hFileMap, dwProtect, (DWORD)(n64FileOffset >> 32), (DWORD)n64FileOffset, (SIZE_T)n64Size));
        if (m_pvFile)
        {
            m_ViewSize = (n64Size != 0) ? (SIZE_T)n64Size : m_MapSize - n64FileOffset;
            m_offset = 0;
            return TRUE;
        }
        return FALSE;
    }

    /**
     * @brief 解除映射
     * @return
     */
    inline BOOL UnMapToMemory()
    {
        if (m_pvFile)
        {
            ::UnmapViewOfFile(m_pvFile);
            m_pvFile = nullptr;
            return TRUE;
        }
        return FALSE;
    }

    /**
     * @brief 保存映射文件
     * @param n64Size
     * @return
     */
    inline BOOL FlushView(int64_t n64Size)
    {
        if (!m_pvFile)
            return FALSE;
        BOOL ret = ::FlushViewOfFile(m_pvFile, (SIZE_T)n64Size);
        m_flie.Flush();
        return ret;
    }

    /**
     * @brief 取数据指针
     * @tparam T 指针类型
     * @param offset 偏移位置
     * @return
     */
    template <typename T = BYTE>
    T *GetPtr(int64_t offset = -1)
    {
        if (!m_pvFile || offset > m_ViewSize)
            return nullptr;
        if (offset < 0)
            offset = m_offset;
        return reinterpret_cast<T *>(m_pvFile + offset);
    }

    /**
     * @brief 读
     * @tparam T 数据类型
     * @param default_value 缺省值
     * @return
     */
    template <typename T>
    T Get(const T &default_value)
    {
        size_t rsize = sizeof(T);
        if (!m_pvFile || !(m_offset + rsize < m_ViewSize))
        {
            m_offset = m_ViewSize;
            return default_value;
        }
        T ret = *reinterpret_cast<T *>(m_pvFile + m_offset);
        m_offset += rsize;
        return ret;
    }

    /**
     * @brief 读字符
     * @return
     */
    int32_t GetChar()
    {
        m_offset++;
        if (!m_pvFile || m_offset >= m_ViewSize || m_offset < 0)
        {
            m_offset = m_ViewSize;
            return -1;
        }
        return *reinterpret_cast<char *>(m_pvFile + m_offset);
    }

    /**
     * @brief 读宽字符
     * @return
     */
    int32_t GetWchar()
    {
        m_offset += 2;
        if (!m_pvFile || m_offset >= m_ViewSize || m_offset < 0)
        {
            m_offset = m_ViewSize;
            return -1;
        }
        return *reinterpret_cast<wchar_t *>(m_pvFile + m_offset);
    }

    /**
     * @brief 读入数据
     * @tparam T 数据类型
     * @param value 值变量
     * @return
     */
    template <typename T>
    int32_t Read(T &value)
    {
        size_t rsize = sizeof(T);
        if (!m_pvFile || m_offset + rsize >= m_ViewSize)
        {
            m_offset = m_ViewSize;
            return 0;
        }
        value = *reinterpret_cast<T *>(m_pvFile + m_offset);
        m_offset += rsize;
        return static_cast<int32_t>(rsize);
    }

    /**
     * @brief 读指针数据
     * @param pvData 数据指针
     * @param n64Size 数据尺寸
     * @return
     */
    int64_t ReadData(void *pvData, int64_t n64Size)
    {
        if (!m_pvFile)
            return 0;
        if (m_offset + n64Size >= m_ViewSize)
            n64Size = m_ViewSize - m_offset;
        memcpy(m_pvFile + m_offset, pvData, static_cast<size_t>(n64Size));
        m_offset += static_cast<size_t>(n64Size);
        return n64Size;
    }

    /**
     * @brief 读数据
     * @param cData 字节集数据
     * @param stSize 读取长度
     * @return
     */
    ptrdiff_t ReadVolMem(CVolMem &cData, ptrdiff_t stSize)
    {
        if (!m_pvFile)
            return 0;
        if (m_offset + stSize >= m_ViewSize)
            stSize = m_ViewSize - m_offset;
        cData.CopyFrom(m_pvFile + m_offset, stSize);
        m_offset += stSize;
        return stSize;
    }

    /**
     * @brief 写
     * @tparam T 数据类型
     * @param value 值
     * @return
     */
    template <typename T>
    int32_t Write(T value)
    {
        size_t rsize = sizeof(T);
        if (!m_pvFile || static_cast<int64_t>(m_offset + rsize) >= m_ViewSize)
            return 0;
        memcpy(m_pvFile + m_offset, &value, rsize);
        m_offset += rsize;
        return static_cast<int32_t>(rsize);
    }

    /**
     * @brief 写指针数据
     * @param pvData 数据指针
     * @param n64Size 数据尺寸
     * @return
     */
    int64_t WriteData(const void *pvData, int64_t n64Size)
    {
        if (!m_pvFile)
            return 0;
        if (m_offset + n64Size >= m_ViewSize)
            n64Size = m_ViewSize - m_offset;
        memcpy(m_pvFile + m_offset, pvData, static_cast<size_t>(n64Size));
        m_offset += static_cast<size_t>(n64Size);
        return n64Size;
    }

    /**
     * @brief 写出字节集
     * @param pvData 数据指针
     * @return
     */
    inline ptrdiff_t WriteVolMem(const CVolMem &Data)
    {
        return static_cast<ptrdiff_t>(this->WriteData(Data.GetPtr(),
                                                      static_cast<size_t>(Data.GetSize())));
    }

    /**
     * @brief 读入文本
     * @param text 存放读入文本的变量
     * @param read_words 欲读入的最多字符数
     * @param encode 文本编码
     * @return 读入的字节长度
     */
    int64_t ReadTextWithEncode(CVolString &text, int64_t read_words = -1, VOL_STRING_ENCODE_TYPE encode = VSET_UTF_16)
    {
        int64_t ret = 0;
        text.Empty();
        switch (encode)
        {
        case VSET_UTF_8:
        {
            std::string str;
            if ((ret = this->ReadText(str, read_words)) > 0)
                piv::encoding::U2Wex(text, str.c_str(), str.size());
            break;
        }
        case VSET_MBCS:
        {
            std::string str;
            if ((ret = this->ReadText(str, read_words)) > 0)
                piv::encoding::A2Wex(text, str.c_str(), str.size(), 0);
            break;
        }
        default:
        {
            ret = this->ReadText(text, read_words);
            break;
        }
        }
        return ret;
    }

    int64_t ReadText(CVolString &text, int64_t read_words = -1)
    {
        text.Empty();
        if (!m_pvFile || m_offset >= m_ViewSize)
        {
            m_offset = m_ViewSize;
            return 0;
        }
        const wchar_t *ps = reinterpret_cast<const wchar_t *>(m_pvFile + m_offset);
        int64_t count = 0;
        if (read_words == -1)
            read_words = (m_ViewSize - m_offset) / sizeof(wchar_t);
        do
        {
            if (count >= read_words)
                break;
            ps++;
            count++;
        } while (*ps != '\0' && *ps != 0x1A);
        text.AddText(reinterpret_cast<const wchar_t *>(m_pvFile + m_offset), static_cast<INT_P>(count));
        m_offset += sizeof(wchar_t) * count;
        return sizeof(wchar_t) * count;
    }

    template <typename CharT>
    int64_t ReadText(std::basic_string<CharT> &text, int64_t read_words = -1)
    {
        text.clear();
        if (!m_pvFile || m_offset >= m_ViewSize)
        {
            m_offset = m_ViewSize;
            return 0;
        }
        const CharT *ps = reinterpret_cast<const CharT *>(m_pvFile + m_offset);
        int64_t count = 0;
        if (read_words == -1)
            read_words = (m_ViewSize - m_offset) / sizeof(CharT);
        do
        {
            if (count >= read_words)
                break;
            ps++;
            count++;
        } while (*ps != '\0' && *ps != 0x1A);
        text.assign(reinterpret_cast<const CharT *>(m_pvFile + m_offset), static_cast<size_t>(count));
        m_offset += sizeof(CharT) * count;
        return sizeof(CharT) * count;
    }

    /**
     * @brief 读入一行
     * @param text 存放读入文本的变量
     * @param encode 文本编码
     * @return 读入的字节长度
     */
    int64_t ReadLineWithEncode(CVolString &text, VOL_STRING_ENCODE_TYPE encode = VSET_UTF_16)
    {
        int64_t ret = 0;
        text.Empty();
        switch (encode)
        {
        case VSET_UTF_8:
        {
            std::string str;
            if ((ret = this->ReadLine(str)) > 0)
                piv::encoding::U2Wex(text, str.c_str(), str.size());
            break;
        }
        case VSET_MBCS:
        {
            std::string str;
            if ((ret = this->ReadLine(str)) > 0)
                piv::encoding::A2Wex(text, str.c_str(), str.size(), 0);
            break;
        }
        default:
        {
            std::wstring str;
            if ((ret = this->ReadLine(str)) > 0)
                text.SetText(str.c_str());
            break;
        }
        }
        return ret;
    }

    int64_t ReadLine(CVolString &text)
    {
        text.Empty();
        if (!m_pvFile || m_offset >= m_ViewSize)
        {
            m_offset = m_ViewSize;
            return 0;
        }
        const wchar_t *ps = reinterpret_cast<const wchar_t *>(m_pvFile + m_offset);
        int64_t count = 0;
        int64_t max_count = (m_ViewSize - m_offset) / sizeof(wchar_t);
        do
        {
            wchar_t ch = ps[count++];
            if (ch == '\0' || ch == 0x1A || ch == '\n')
            {
                break;
            }
            if (ch == '\r')
            {
                if (count < max_count && ps[count] == '\n')
                    count++;
                break;
            }
        } while (count < max_count);
        text.AddText(ps, static_cast<INT_P>(count - 1));
        if (!text.IsEmpty() && text.GetLastChar() == '\r')
            text.SetLength(text.GetLength() - 1);
        m_offset += sizeof(wchar_t) * count;
        return sizeof(wchar_t) * count;
    }

    template <typename CharT>
    int64_t ReadLine(std::basic_string<CharT> &text)
    {
        text.clear();
        if (!m_pvFile || m_offset >= m_ViewSize)
        {
            m_offset = m_ViewSize;
            return 0;
        }
        const CharT *ps = reinterpret_cast<const CharT *>(m_pvFile + m_offset);
        int64_t count = 0;
        int64_t max_count = (m_ViewSize - m_offset) / sizeof(CharT);
        do
        {
            CharT ch = ps[count++];
            if (ch == '\0' || ch == 0x1A || ch == '\n')
            {
                break;
            }
            if (ch == '\r')
            {
                if (count < max_count && ps[count] == '\n')
                    count++;
                break;
            }
        } while (count < max_count);
        text.assign(ps, static_cast<size_t>(count - 1));
        if (!text.empty() && text.back() == '\r')
            text.pop_back();
        m_offset += sizeof(CharT) * count;
        return sizeof(CharT) * count;
    }

    /**
     * @brief 写出文本
     * @param str 欲写出的文本
     * @param null_terminated 是否包括结束零字符
     * @param encode 文本编码类型
     * @return
     */
    int64_t WriteText(const wchar_t *str, BOOL null_terminated = FALSE, VOL_STRING_ENCODE_TYPE encode = VSET_UTF_16)
    {
        switch (encode)
        {
        case VSET_UTF_8:
        {
            PivW2U u8{str};
            return this->WriteData(u8.GetText(), u8.GetSize() + null_terminated ? 1 : 0);
            break;
        }
        case VSET_MBCS:
        {
            PivW2A mbcs{str};
            return this->WriteData(mbcs.GetText(), mbcs.GetSize() + null_terminated ? 1 : 0);
            break;
        }
        default:
        {
            return this->WriteData(str, (wcslen(str) + null_terminated ? 1 : 0) * 2);
            break;
        }
        }
        return 0;
    }

    /**
     * @brief 写出一行
     * @param str 欲写出的文本
     * @param encode 文本编码类型
     * @return
     */
    int64_t WriteLine(const wchar_t *str, VOL_STRING_ENCODE_TYPE encode = VSET_UTF_16)
    {
        int64_t ret = this->WriteText(str, FALSE, encode);
        if (encode == VSET_UTF_16)
        {
            ret += this->Write<char>('\r');
            ret += this->Write<char>('\n');
        }
        else
        {
            ret += this->Write<wchar_t>('\r');
            ret += this->Write<wchar_t>('\n');
        }
        return ret;
    }

    /**
     * @brief 删除数据
     * @param offset 起始位置
     * @param length 删除长度
     * @return
     */
    inline BOOL RemoveData(int64_t offset, int64_t length)
    {
        if (offset < 0)
            offset = m_offset;
        if (!m_pvFile || offset >= m_MapSize || length <= 0 || length > m_MapSize - offset)
            return FALSE;
        memmove(m_pvFile + offset, m_pvFile + offset + length, static_cast<size_t>(m_MapSize - offset - length));
        memset(m_pvFile + (m_MapSize - length), 0, length);
        return TRUE;
    }

    /**
     * @brief 取文件长度
     * @return 失败返回-1
     */
    inline int64_t GetSize() const
    {
        return m_flie.GetSize();
    }

    /**
     * @brief 取映射尺寸
     * @return
     */
    inline int64_t GetViewSize() const
    {
        return m_ViewSize;
    }

    /**
     * @brief 取映射页尺寸
     * @return
     */
    inline int64_t GetMapSize() const
    {
        if (m_pvFile)
        {
            MEMORY_BASIC_INFORMATION info{0};
            if (::VirtualQuery(m_pvFile, &info, sizeof(info)))
                return static_cast<int64_t>(info.RegionSize);
        }
        return 0;
    }

    /**
     * @brief 取读写位置
     * @return
     */
    inline int64_t GetCurrentPos() const
    {
        return m_offset;
    }

    /**
     * @brief 移动读写位置
     * @param offset 移动距离
     * @param dwMoveMethod 基准移动位置
     * @return
     */
    inline BOOL SetCurrentPos(int64_t offset, int32_t dwMoveMethod = 0)
    {
        if (!m_pvFile || m_ViewSize == 0)
            return FALSE;
        if (dwMoveMethod == FILE_BEGIN)
        {
            if (offset >= 0 && offset <= m_ViewSize)
            {
                m_offset = offset;
                return TRUE;
            }
        }
        else if (dwMoveMethod == FILE_CURRENT)
        {
            if (m_offset >= 0 && m_offset + offset <= m_ViewSize)
            {
                m_offset += offset;
                return TRUE;
            }
        }
        else if (dwMoveMethod == FILE_END)
        {
            if (offset <= 0 && m_ViewSize + offset <= m_ViewSize)
            {
                m_offset = m_ViewSize + offset;
                return TRUE;
            }
        }
        return FALSE;
    }

    /**
     * @brief 是否在尾部
     * @return
     */
    inline BOOL IsEof() const
    {
        return (m_offset == m_ViewSize);
    }
}; // PivFileMapping

#endif // PIV_FILE_HPP
