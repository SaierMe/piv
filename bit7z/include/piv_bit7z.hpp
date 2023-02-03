/*********************************************\
 * 火山视窗 - bit7z封装类                    *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 版本: 2022/12/03                          *
\*********************************************/

#ifndef _PIV_BIT7Z_HPP
#define _PIV_BIT7Z_HPP

// 包含火山视窗基本类,它在火山里的包含顺序比较靠前(全局-110)
#ifndef __VOL_BASE_H__
#include <sys/base/libs/win_base/vol_base.h>
#endif

// 包含bit7z头文件
#ifndef BIT7Z_AUTO_FORMAT
#define BIT7Z_AUTO_FORMAT
#endif
#ifndef BIT7Z_REGEX_MATCHING
#define BIT7Z_REGEX_MATCHING
#endif
#ifndef BIT7Z_USE_NATIVE_STRING
#define BIT7Z_USE_NATIVE_STRING
#endif
#ifndef BITWINDOWS_HPP
#define BITWINDOWS_HPP
#endif
#include "bit7z.hpp"
#include "bitarchiveeditor.hpp"
// 包含C++标准库
#include <mutex>
#include <memory>
// 包含stream包装类
// #include "..\..\src\piv_stream.hpp"

namespace piv
{
    namespace Archive
    {
        // 加载7z.dll动态库
        static bit7z::Bit7zLibrary *m_7zLib = nullptr;
        static bool Load7zLib(const bit7z::tstring &library_path = bit7z::default_library)
        {
            try
            {
                m_7zLib = new bit7z::Bit7zLibrary{library_path};
                return m_7zLib != nullptr;
            }
            catch (...)
            {
                return false;
            }
        }
        // 卸载7z.dll动态库
        static void Free7zLib()
        {
            if (m_7zLib)
            {
                delete m_7zLib;
                m_7zLib = nullptr;
            }
        }
        // 获取7z.dll动态库
        inline static bit7z::Bit7zLibrary &Get7zLib()
        {
            if (!m_7zLib)
            {
                Load7zLib();
            }
            return *m_7zLib;
        }

        static void VolstrArrToVector(const CMStringArray &arr, std::vector<bit7z::tstring> &vec)
        {
            for (INT_P i = 0; i < arr.GetCount(); i++)
            {
                vec.push_back(bit7z::tstring{arr.GetAt(i)});
            }
        }
        static void VolStrMapToStdMap(const std::map<CVolString, CVolString> &A, std::map<bit7z::tstring, bit7z::tstring> &B)
        {
            for (auto iter = A.begin(); iter != A.end(); iter++)
            {
                B[bit7z::tstring{iter->first.GetText()}] = bit7z::tstring{iter->second.GetText()};
            }
        }

        static bit7z::buffer_t *MemToVectorPtr(CVolMem &mem)
        {
            if (mem.IsEmpty())
                return new bit7z::buffer_t;
            else
                return new bit7z::buffer_t{reinterpret_cast<bit7z::byte_t *>(mem.GetPtr()),
                                           reinterpret_cast<bit7z::byte_t *>(mem.GetPtr()) + mem.GetSize()};
        }
        static bit7z::buffer_t *MemToVectorPtr(ptrdiff_t mem, ptrdiff_t size)
        {
            return new bit7z::buffer_t{reinterpret_cast<bit7z::byte_t *>(mem),
                                       reinterpret_cast<bit7z::byte_t *>(mem) + size};
        }

        // 存档压缩等级
        static auto GetCompressionLevel(int32_t value)
        {
            switch (value)
            {
            case 0:
                return bit7z::BitCompressionLevel::None;
            case 1:
                return bit7z::BitCompressionLevel::Fastest;
            case 3:
                return bit7z::BitCompressionLevel::Fast;
            case 5:
                return bit7z::BitCompressionLevel::Normal;
            case 7:
                return bit7z::BitCompressionLevel::Max;
            case 9:
                return bit7z::BitCompressionLevel::Ultra;
            }
            return bit7z::BitCompressionLevel::Normal;
        }

        // 存档更新方式
        static auto GetUpdateMode(int32_t value)
        {
            switch (value)
            {
            case 0:
                return bit7z::UpdateMode::None;
            case 1:
                return bit7z::UpdateMode::Append;
            case 2:
                return bit7z::UpdateMode::Overwrite;
            case 3:
                return bit7z::UpdateMode::OverwriteArchive;
            }
            return bit7z::UpdateMode::None;
        }

        // 存档输出格式
        static const bit7z::BitInOutFormat &GetInOutFormat(int8_t value)
        {
            switch (value)
            {
            case 0x01:
                return bit7z::BitFormat::Zip;
            case 0x02:
                return bit7z::BitFormat::BZip2;
            case 0x07:
                return bit7z::BitFormat::SevenZip;
            case 0x0C:
                return bit7z::BitFormat::Xz;
            case 0xE6:
                return bit7z::BitFormat::Wim;
            case 0xEE:
                return bit7z::BitFormat::Tar;
            case 0xEF:
                return bit7z::BitFormat::GZip;
            }
            return bit7z::BitFormat::Zip;
        }

        // 存档输入格式
        static const bit7z::BitInFormat &GetInFormat(int8_t value)
        {
            switch (value)
            {
            case 0x00:
                return bit7z::BitFormat::Auto;
            case 0x01:
                return bit7z::BitFormat::Zip;
            case 0x02:
                return bit7z::BitFormat::BZip2;
            case 0x03:
                return bit7z::BitFormat::Rar;
            case 0x04:
                return bit7z::BitFormat::Arj;
            case 0x05:
                return bit7z::BitFormat::Z;
            case 0x06:
                return bit7z::BitFormat::Lzh;
            case 0x07:
                return bit7z::BitFormat::SevenZip;
            case 0x08:
                return bit7z::BitFormat::Cab;
            case 0x09:
                return bit7z::BitFormat::Nsis;
            case 0x0A:
                return bit7z::BitFormat::Lzma;
            case 0x0B:
                return bit7z::BitFormat::Lzma86;
            case 0x0C:
                return bit7z::BitFormat::Xz;
            case 0x0D:
                return bit7z::BitFormat::Ppmd;
            // case 0xC4: return bit7z::BitFormat::Vhdx; 不知为何报错
            case 0xC6:
                return bit7z::BitFormat::COFF;
            case 0xC7:
                return bit7z::BitFormat::Ext;
            case 0xC8:
                return bit7z::BitFormat::VMDK;
            case 0xC9:
                return bit7z::BitFormat::VDI;
            case 0xCA:
                return bit7z::BitFormat::QCow;
            case 0xCB:
                return bit7z::BitFormat::GPT;
            case 0xCC:
                return bit7z::BitFormat::Rar5;
            case 0xCD:
                return bit7z::BitFormat::IHex;
            case 0xCE:
                return bit7z::BitFormat::Hxs;
            case 0xCF:
                return bit7z::BitFormat::TE;
            case 0xD0:
                return bit7z::BitFormat::UEFIc;
            case 0xD1:
                return bit7z::BitFormat::UEFIs;
            case 0xD2:
                return bit7z::BitFormat::SquashFS;
            case 0xD3:
                return bit7z::BitFormat::CramFS;
            case 0xD4:
                return bit7z::BitFormat::APM;
            case 0xD5:
                return bit7z::BitFormat::Mslz;
            case 0xD6:
                return bit7z::BitFormat::Flv;
            case 0xD9:
                return bit7z::BitFormat::Ntfs;
            case 0xDA:
                return bit7z::BitFormat::Fat;
            case 0xDC:
                return bit7z::BitFormat::Vhd;
            case 0xDD:
                return bit7z::BitFormat::Pe;
            case 0xDE:
                return bit7z::BitFormat::Elf;
            case 0xDF:
                return bit7z::BitFormat::Macho;
            case 0xE0:
                return bit7z::BitFormat::Udf;
            case 0xE1:
                return bit7z::BitFormat::Xar;
            case 0xE2:
                return bit7z::BitFormat::Mub;
            case 0xE3:
                return bit7z::BitFormat::Hfs;
            case 0xE4:
                return bit7z::BitFormat::Dmg;
            case 0xE5:
                return bit7z::BitFormat::Compound;
            case 0xE6:
                return bit7z::BitFormat::Wim;
            case 0xE7:
                return bit7z::BitFormat::Iso;
            case 0xE9:
                return bit7z::BitFormat::Chm;
            case 0xEA:
                return bit7z::BitFormat::Split;
            case 0xEB:
                return bit7z::BitFormat::Rpm;
            case 0xEC:
                return bit7z::BitFormat::Deb;
            case 0xED:
                return bit7z::BitFormat::Cpio;
            case 0xEE:
                return bit7z::BitFormat::Tar;
            case 0xEF:
                return bit7z::BitFormat::GZip;
            }
            return bit7z::BitFormat::Auto;
        }

        // 存档压缩方法
        static const bit7z::BitCompressionMethod GetCompressionMethod(int32_t value)
        {
            switch (value)
            {
            case 0:
                return bit7z::BitCompressionMethod::Copy;
            case 1:
                return bit7z::BitCompressionMethod::Deflate;
            case 2:
                return bit7z::BitCompressionMethod::Deflate64;
            case 3:
                return bit7z::BitCompressionMethod::BZip2;
            case 4:
                return bit7z::BitCompressionMethod::Lzma;
            case 5:
                return bit7z::BitCompressionMethod::Lzma2;
            case 6:
                return bit7z::BitCompressionMethod::Ppmd;
            }
            return bit7z::BitCompressionMethod::Copy;
        }

        // PivArchiveOperate 的错误信息
        static std::string g_error{"OK"};
        static std::mutex g_error_mutex;

    } // namespace Archive
} // namespace piv

class PivArchiveOperate
{
public:
    PivArchiveOperate() {}
    ~PivArchiveOperate() {}
    // 取最后错误
    static CVolString GetLastError()
    {
        std::lock_guard<std::mutex> guard(piv::Archive::g_error_mutex);
        return CVolString(piv::Archive::g_error.c_str());
    }
    static void SetError(const char *msg)
    {
        std::lock_guard<std::mutex> guard(piv::Archive::g_error_mutex);
        piv::Archive::g_error = msg;
    }
    // 添加到压缩包
    static bool CompressPaths(const CMStringArray &paths, const wchar_t *out_file, const wchar_t *password, int8_t format, int32_t level, int32_t update_mode)
    {
        if (paths.IsEmpty())
        {
            SetError("未提供所欲压缩的文件或目录.");
            return false;
        }
        try
        {
            std::vector<bit7z::tstring> in_paths;
            piv::Archive::VolstrArrToVector(paths, in_paths);
            bit7z::BitFileCompressor Compressor{piv::Archive::Get7zLib(), piv::Archive::GetInOutFormat(format)};
            Compressor.setPassword(password);
            Compressor.setCompressionLevel(piv::Archive::GetCompressionLevel(level));
            Compressor.setUpdateMode(piv::Archive::GetUpdateMode(update_mode));
            Compressor.compress(in_paths, out_file);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 添加别名到压缩包
    static bool CompressPaths(const std::map<CVolString, CVolString> &path_pair, const wchar_t *out_file, const wchar_t *password, int8_t format, int32_t level, int32_t update_mode)
    {
        if (path_pair.empty())
        {
            SetError("未提供所欲压缩的文件或目录.");
            return false;
        }
        try
        {
            std::map<bit7z::tstring, bit7z::tstring> in_paths;
            piv::Archive::VolStrMapToStdMap(path_pair, in_paths);
            bit7z::BitFileCompressor Compressor{piv::Archive::Get7zLib(), piv::Archive::GetInOutFormat(format)};
            Compressor.setPassword(password);
            Compressor.setCompressionLevel(piv::Archive::GetCompressionLevel(level));
            Compressor.setUpdateMode(piv::Archive::GetUpdateMode(update_mode));
            Compressor.compress(in_paths, out_file);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 添加文件到压缩包
    static bool CompressFiles(const CMStringArray &paths, const wchar_t *out_file, const wchar_t *password, int8_t format, int32_t level, int32_t update_mode)
    {
        if (paths.IsEmpty())
        {
            SetError("未提供所欲压缩的文件.");
            return false;
        }
        try
        {
            std::vector<bit7z::tstring> in_paths;
            piv::Archive::VolstrArrToVector(paths, in_paths);
            bit7z::BitFileCompressor Compressor{piv::Archive::Get7zLib(), piv::Archive::GetInOutFormat(format)};
            Compressor.setPassword(password);
            Compressor.setCompressionLevel(piv::Archive::GetCompressionLevel(level));
            Compressor.setUpdateMode(piv::Archive::GetUpdateMode(update_mode));
            Compressor.compress(in_paths, out_file);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 添加匹配文件到压缩包
    static bool CompressFiles(const wchar_t *in_dir, const wchar_t *out_file, const wchar_t *filter, bool recursive, const wchar_t *password, int8_t format, int32_t level, int32_t update_mode)
    {
        try
        {
            bit7z::BitFileCompressor Compressor{piv::Archive::Get7zLib(), piv::Archive::GetInOutFormat(format)};
            Compressor.setPassword(password);
            Compressor.setCompressionLevel(piv::Archive::GetCompressionLevel(level));
            Compressor.setUpdateMode(piv::Archive::GetUpdateMode(update_mode));
            Compressor.compressFiles(in_dir, out_file, recursive, filter);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 添加目录到压缩包
    static bool CompressDirectory(const wchar_t *in_dir, const wchar_t *out_file, const wchar_t *password, int8_t format, int32_t level, int32_t update_mode)
    {
        try
        {
            bit7z::BitFileCompressor Compressor{piv::Archive::Get7zLib(), piv::Archive::GetInOutFormat(format)};
            Compressor.setPassword(password);
            Compressor.setCompressionLevel(piv::Archive::GetCompressionLevel(level));
            Compressor.setUpdateMode(piv::Archive::GetUpdateMode(update_mode));
            Compressor.compressDirectory(in_dir, out_file);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    // 解压到
    static bool Extract(const wchar_t *in_file, const wchar_t *out_dir, const wchar_t *password, int8_t format)
    {
        try
        {
            bit7z::BitFileExtractor extractor{piv::Archive::Get7zLib(), piv::Archive::GetInFormat(format)};
            extractor.setPassword(password);
            extractor.extract(in_file, out_dir);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 解压到内存
    static bool Extract(const wchar_t *in_file, std::map<CVolString, CVolMem> &out_buffer, const wchar_t *password, int8_t format)
    {
        out_buffer.clear();
        try
        {
            bit7z::BitFileExtractor extractor{piv::Archive::Get7zLib(), piv::Archive::GetInFormat(format)};
            extractor.setPassword(password);
            bit7z::BitInputArchive in_archive(extractor, in_file);
            for (auto itr = in_archive.begin(); itr != in_archive.end(); itr++)
            {
                if (!itr->isDir())
                {
                    CVolString strPath;
                    strPath.SetText(itr->path().c_str());
                    out_buffer[strPath] = CVolMem();
                    out_buffer[strPath].Alloc(static_cast<INT_P>(itr->size()), true);
                    in_archive.extract(reinterpret_cast<bit7z::byte_t *>(out_buffer[strPath].GetPtr()), static_cast<std::size_t>(itr->size()), itr->index());
                }
            }
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            out_buffer.clear();
            SetError(ex.what());
            return false;
        }
    }
    // 解压匹配文件
    static bool Extract(const wchar_t *in_file, const wchar_t *filter, const wchar_t *out_dir, const wchar_t *password, bool policy, int8_t format)
    {
        try
        {
            bit7z::BitFileExtractor extractor{piv::Archive::Get7zLib(), piv::Archive::GetInFormat(format)};
            extractor.setPassword(password);
            extractor.extractMatching(in_file, filter, out_dir, policy ? bit7z::FilterPolicy::Exclude : bit7z::FilterPolicy::Include);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 解压匹配文件到字节集
    static bool Extract(const wchar_t *in_file, const wchar_t *filter, CVolMem &out_buffer, const wchar_t *password, CVolString &out_file, bool policy, int8_t format)
    {
        out_file.Empty();
        try
        {
            if (filter == nullptr || wcslen(filter) == 0)
            {
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::FilterNotSpecified));
            }
            bit7z::BitFileExtractor extractor{piv::Archive::Get7zLib(), piv::Archive::GetInFormat(format)};
            extractor.setPassword(password);
            bit7z::BitInputArchive in_archive(extractor, in_file);
            for (const auto &item : in_archive)
            {
                bool item_matches = bit7z::filesystem::fsutil::wildcardMatch(filter, item.path());
                if (item_matches != policy)
                {
                    out_buffer.Alloc(static_cast<INT_P>(item.size()), true);
                    in_archive.extract(reinterpret_cast<bit7z::byte_t *>(out_buffer.GetPtr()), static_cast<std::size_t>(out_buffer.GetSize()), item.index());
                    out_file.SetText(item.path().c_str());
                    SetError("OK");
                    return true;
                }
            }
            throw bit7z::BitException("Failed to extract items", bit7z::make_error_code(bit7z::BitError::NoMatchingItems));
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 解压正则匹配文件
    static bool ExtractRegex(const wchar_t *in_file, const wchar_t *regex, const wchar_t *out_dir, const wchar_t *password, bool policy, int8_t format)
    {
        try
        {
            bit7z::BitFileExtractor extractor{piv::Archive::Get7zLib(), piv::Archive::GetInFormat(format)};
            extractor.setPassword(password);
            extractor.extractMatchingRegex(in_file, regex, out_dir, policy ? bit7z::FilterPolicy::Exclude : bit7z::FilterPolicy::Include);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
        catch (const std::regex_error &r)
        {
            SetError(r.what());
            return false;
        }
    }
    // 解压正则匹配文件到字节集
    static bool ExtractRegex(const wchar_t *in_file, const wchar_t *regex, CVolMem &out_buffer, const wchar_t *password, CVolString &out_file, bool policy, int8_t format)
    {
        out_file.Empty();
        try
        {
            if (regex == nullptr || wcslen(regex) == 0)
            {
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::FilterNotSpecified));
            }
            bit7z::BitFileExtractor extractor{piv::Archive::Get7zLib(), piv::Archive::GetInFormat(format)};
            extractor.setPassword(password);
            bit7z::BitInputArchive in_archive(extractor, in_file);
            const bit7z::tregex regex_filter(regex, bit7z::tregex::ECMAScript | bit7z::tregex::optimize);
            for (const auto &item : in_archive)
            {
                bool item_matches = std::regex_match(item.path(), regex_filter);
                if (item_matches != policy)
                {
                    out_buffer.Alloc(static_cast<INT_P>(item.size()), true);
                    in_archive.extract(reinterpret_cast<bit7z::byte_t *>(out_buffer.GetPtr()), static_cast<std::size_t>(out_buffer.GetSize()), item.index());
                    out_file.SetText(item.path().c_str());
                    SetError("OK");
                    return true;
                }
            }
            throw bit7z::BitException("Failed to extract items", bit7z::make_error_code(bit7z::BitError::NoMatchingItems));
        }
        catch (const bit7z::BitException &ex)
        {
            out_buffer.Empty();
            SetError(ex.what());
            return false;
        }
        catch (const std::regex_error &r)
        {
            out_buffer.Empty();
            SetError(r.what());
            return false;
        }
    }
    // 测试
    static bool Test(const wchar_t *in_file, const wchar_t *password, int8_t format)
    {
        try
        {
            bit7z::BitFileExtractor extractor{piv::Archive::Get7zLib(), piv::Archive::GetInFormat(format)};
            extractor.setPassword(password);
            extractor.test(in_file);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
}; // PivArchiveOperate

class PivArchiveReader
{
private:
    bit7z::BitArchiveReader *m_archive = nullptr;
    std::unique_ptr<std::vector<bit7z::byte_t>> m_buffer{nullptr};
    // PivMemoryInStream<char> m_in_stream;
    std::string m_error;
    CVolString m_progress_file;
    uint64_t m_total_size = 0;
    int32_t m_ratio = 0;

public:
    PivArchiveReader() {}
    ~PivArchiveReader()
    {
        CloseArchive();
    }
    inline bit7z::BitArchiveReader &arc()
    {
        return *m_archive;
    }
    inline bit7z::BitArchiveReader *pArc()
    {
        return m_archive;
    }
    inline bool IsNull()
    {
        return m_archive == nullptr;
    }
    inline CVolString GetError()
    {
        return CVolString(GetWideText(m_error.c_str(), CVolMem(), NULL));
    }
    inline void SetError(const CHAR *Error)
    {
        m_error = Error;
    }
    inline void CloseArchive()
    {
        m_buffer.reset();
        if (m_archive)
        {
            delete m_archive;
            m_archive = nullptr;
        }
    }

    // CallBack
    virtual int32_t ExtractProgress(CVolString progressFile, int32_t progress, int64_t totalSize, int64_t progress_size, int32_t ratio) { return 0; }
    void FileNameCB(bit7z::tstring fileName)
    {
        m_progress_file.SetText(fileName.c_str());
    }
    void TotalSizeCB(uint64_t total_size)
    {
        m_total_size = total_size;
    }
    void RatioCB(uint64_t input, uint64_t output)
    {
        m_ratio = static_cast<int32_t>(100.0 * input / output);
    }
    bool ProgressCB(uint64_t progress_size)
    {
        if (progress_size == 0)
            return true;
        return !this->ExtractProgress(m_progress_file, static_cast<int32_t>(100.0 * progress_size / m_total_size),
                                      static_cast<int64_t>(m_total_size), static_cast<int64_t>(progress_size), m_ratio);
    }
    void EnableFeeback()
    {
        if (!IsNull())
        {
            const bit7z::FileCallback cbFileName = std::bind(&PivArchiveReader::FileNameCB, this, std::placeholders::_1);
            arc().setFileCallback(cbFileName);
            const bit7z::TotalCallback cbTotalSize = std::bind(&PivArchiveReader::TotalSizeCB, this, std::placeholders::_1);
            arc().setTotalCallback(cbTotalSize);
            const bit7z::RatioCallback cbRatio = std::bind(&PivArchiveReader::RatioCB, this, std::placeholders::_1, std::placeholders::_2);
            arc().setRatioCallback(cbRatio);
            const bit7z::ProgressCallback cbProgress = std::bind(&PivArchiveReader::ProgressCB, this, std::placeholders::_1);
            arc().setProgressCallback(cbProgress);
        }
    }
    // 打开文件
    bool OpenArchive(const wchar_t *in_file, bool feedback, const wchar_t *password, int8_t format)
    {
        CloseArchive();
        try
        {
            m_archive = new bit7z::BitArchiveReader{piv::Archive::Get7zLib(), in_file, piv::Archive::GetInFormat(format), password};
            if (feedback)
            {
                EnableFeeback();
            }
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            CloseArchive();
            SetError(ex.what());
            return false;
        }
    }
    // 打开vector
    bool OpenArchive(std::vector<bit7z::byte_t> &in_buffer, bool feedback, const wchar_t *password, int8_t format)
    {
        CloseArchive();
        try
        {
            m_archive = new bit7z::BitArchiveReader{piv::Archive::Get7zLib(), in_buffer, piv::Archive::GetInFormat(format), password};
            if (feedback)
            {
                EnableFeeback();
            }
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            CloseArchive();
            SetError(ex.what());
            return false;
        }
    }
    // 打开内存输入流
    bool OpenArchive(std::istream &in_stream, bool feedback, const wchar_t *password, int8_t format)
    {
        CloseArchive();
        try
        {
            m_archive = new bit7z::BitArchiveReader{piv::Archive::Get7zLib(), in_stream, piv::Archive::GetInFormat(format), password};
            if (feedback)
            {
                EnableFeeback();
            }
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            CloseArchive();
            SetError(ex.what());
            return false;
        }
    }
    // 打开字节集
    bool OpenArchive(CVolMem &in_data, bool feedback, const wchar_t *password, int8_t format /*, bool copy_data*/)
    {
        CloseArchive();
        try
        {
            // m_in_stream.OpenStream(in_data, copy_data);
            m_buffer.reset(new std::vector<bit7z::byte_t>{reinterpret_cast<bit7z::byte_t *>(in_data.GetPtr()),
                                                          reinterpret_cast<bit7z::byte_t *>(in_data.GetPtr()) + in_data.GetSize()});
            m_archive = new bit7z::BitArchiveReader{piv::Archive::Get7zLib(), *m_buffer, piv::Archive::GetInFormat(format), password};
            if (feedback)
            {
                EnableFeeback();
            }
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            CloseArchive();
            SetError(ex.what());
            return false;
        }
    }
    // 打开内存
    bool OpenArchive(ptrdiff_t mem_ptr, ptrdiff_t mem_size, bool feedback, const wchar_t *password, int8_t format /*, bool copy_data*/)
    {
        CloseArchive();
        try
        {
            // m_in_stream.OpenStream(mem_ptr, mem_size, copy_data);
            m_buffer.reset(new std::vector<bit7z::byte_t>{reinterpret_cast<bit7z::byte_t *>(mem_ptr),
                                                          reinterpret_cast<bit7z::byte_t *>(mem_ptr) + mem_size});
            m_archive = new bit7z::BitArchiveReader{piv::Archive::Get7zLib(), *m_buffer, piv::Archive::GetInFormat(format), password};
            if (feedback)
            {
                EnableFeeback();
            }
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            CloseArchive();
            SetError(ex.what());
            return false;
        }
    }
    // 打开文件资源
    /*
    bool OpenArchive(size_t resid, bool feedback, const wchar_t *password, int8_t format)
    {
        CloseArchive();
        try
        {
            m_in_stream.OpenStream(resid);
            m_archive = new bit7z::BitArchiveReader{piv::Archive::Get7zLib(), m_in_stream.is(), piv::Archive::GetInFormat(format), password};
            if (feedback)
            {
                EnableFeeback();
            }
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            CloseArchive();
            SetError(ex.what());
            return false;
        }
    }
    */
    // 解压到
    bool Extract(const wchar_t *out_dir)
    {
        if (IsNull())
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            arc().extract(out_dir);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 解压到内存
    bool Extract(std::map<CVolString, CVolMem> &out_buffer)
    {
        out_buffer.clear();
        if (IsNull())
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            std::map<bit7z::tstring, std::vector<bit7z::byte_t>> out_map;
            arc().extract(out_map);
            for (auto itr = out_map.begin(); itr != out_map.end(); itr++)
            {
                CVolString Key;
                Key.SetText(itr->first.c_str());
                out_buffer[Key] = CVolMem();
                out_buffer[Key].SetMemAlignSize(static_cast<ptrdiff_t>(itr->second.size()));
                out_buffer[Key].Append(itr->second.data(), itr->second.size());
                itr->second.clear();
            }
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            out_buffer.clear();
            SetError(ex.what());
            return false;
        }
    }
    // 解压到内存2
    bool ExtractEx(std::map<CVolString, CVolMem> &out_buffer)
    {
        out_buffer.clear();
        if (IsNull())
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            for (auto itr = arc().begin(); itr != arc().end(); itr++)
            {
                if (!itr->isDir())
                {
                    CVolString path;
                    path.SetText(itr->path().c_str());
                    out_buffer[path] = CVolMem();
                    out_buffer[path].Alloc((INT_P)itr->size(), true);
                    arc().extract(reinterpret_cast<bit7z::byte_t *>(out_buffer[path].GetPtr()), static_cast<std::size_t>(itr->size()), itr->index());
                }
            }
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            out_buffer.clear();
            SetError(ex.what());
            return false;
        }
    }
    // 解压项目到字节集
    bool Extract(int32_t idx, CVolMem &out_buffer, CVolString &out_path)
    {
        out_path.Empty();
        if (IsNull())
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            if (idx < 0 || static_cast<uint32_t>(idx) > arc().itemsCount() - 1)
            {
                throw bit7z::BitException("Cannot extract item at the index " + std::to_string(idx), bit7z::make_error_code(bit7z::BitError::InvalidIndex));
            }
            out_buffer.Alloc(static_cast<ptrdiff_t>(arc().itemProperty(static_cast<uint32_t>(idx), bit7z::BitProperty::Size).getUInt64()), true);
            arc().extract(reinterpret_cast<bit7z::byte_t *>(out_buffer.GetPtr()), static_cast<std::size_t>(out_buffer.GetSize()), static_cast<uint32_t>(idx));
            out_path.SetText(arc().itemProperty(static_cast<uint32_t>(idx), bit7z::BitProperty::Path).getString().c_str());
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            out_buffer.Empty();
            SetError(ex.what());
            return false;
        }
    }
    // 解压多个项目到
    bool Extract(CMArray<int32_t> &idx_array, const wchar_t *out_dir)
    {
        if (IsNull())
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            if (idx_array.IsEmpty())
            {
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::IndicesNotSpecified));
            }
            std::vector<uint32_t> indices(reinterpret_cast<uint32_t *>(idx_array.GetData()),
                                          reinterpret_cast<uint32_t *>(idx_array.GetData()) + idx_array.GetCount());
            uint32_t n_items = arc().itemsCount();
            const auto find_res = std::find_if(indices.cbegin(), indices.cend(),
                                               [&n_items](uint32_t index) -> bool
                                               { return index >= n_items; });
            if (find_res != indices.cend())
            {
            }
            arc().extract(out_dir, indices);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 解压匹配文件
    bool Extract(const wchar_t *filter, const wchar_t *out_dir, bool policy)
    {
        if (IsNull())
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            if (filter == nullptr || wcslen(filter) == 0)
            {
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::FilterNotSpecified));
            }
            std::vector<uint32_t> matched_indices;
            for (const auto &item : arc())
            {
                bool item_matches = bit7z::filesystem::fsutil::wildcardMatch(filter, item.path());
                if (item_matches != policy)
                {
                    matched_indices.push_back(item.index());
                }
            }

            if (matched_indices.empty())
            {
                {
                    throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::NoMatchingItems));
                }
                arc().extract(out_dir, matched_indices);
                SetError("OK");
                return true;
            }
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 解压匹配文件到字节集
    bool Extract(const wchar_t *filter, CVolMem &out_buffer, CVolString &out_path, bool policy)
    {
        out_path.Empty();
        if (IsNull())
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            if (filter == nullptr || wcslen(filter) == 0)
            {
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::FilterNotSpecified));
            }
            for (const auto &item : arc())
            {
                bool item_matches = bit7z::filesystem::fsutil::wildcardMatch(filter, item.path());
                if (item_matches != policy)
                {
                    out_buffer.Alloc(static_cast<ptrdiff_t>(item.size()), true);
                    arc().extract(reinterpret_cast<bit7z::byte_t *>(out_buffer.GetPtr()), static_cast<std::size_t>(out_buffer.GetSize()), item.index());
                    out_path.SetText(item.path().c_str());
                    SetError("OK");
                    return true;
                }
            }
        }
        catch (const bit7z::BitException &ex)
        {
            out_buffer.Empty();
            SetError(ex.what());
            return false;
        }
    }
    // 解压正则匹配文件
    bool ExtractRegex(const wchar_t *regex, const wchar_t *out_dir, bool policy)
    {
        if (IsNull())
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            if (regex == nullptr || wcslen(regex) == 0)
            {
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::FilterNotSpecified));
            }
            std::vector<uint32_t> matched_indices;
            const bit7z::tregex regex_filter(regex, bit7z::tregex::ECMAScript | bit7z::tregex::optimize);
            for (const auto &item : arc())
            {
                bool item_matches = std::regex_match(item.path(), regex_filter);
                if (item_matches != policy)
                {
                    matched_indices.push_back(item.index());
                }
            }
            if (matched_indices.empty())
            {
                {
                    throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::NoMatchingItems));
                }
                arc().extract(out_dir, matched_indices);
                SetError("OK");
                return true;
            }
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
        catch (const std::regex_error &r)
        {
            SetError(r.what());
            return false;
        }
    }
    // 解压正则匹配文件到字节集
    bool ExtractRegex(const wchar_t *regex, CVolMem &out_buffer, CVolString &out_path, bool policy)
    {
        out_path.Empty();
        if (IsNull())
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            if (regex == nullptr || wcslen(regex) == 0)
            {
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::FilterNotSpecified));
            }
            std::vector<uint32_t> matched_indices;
            const bit7z::tregex regex_filter(regex, bit7z::tregex::ECMAScript | bit7z::tregex::optimize);
            for (const auto &item : arc())
            {
                bool item_matches = std::regex_match(item.path(), regex_filter);
                if (item_matches != policy)
                {
                    out_buffer.Alloc(static_cast<ptrdiff_t>(item.size()), true);
                    arc().extract(reinterpret_cast<bit7z::byte_t *>(out_buffer.GetPtr()), static_cast<std::size_t>(out_buffer.GetSize()), item.index());
                    out_path.SetText(item.path().c_str());
                    SetError("OK");
                    return true;
                }
            }
        }
        catch (const bit7z::BitException &ex)
        {
            out_buffer.Empty();
            SetError(ex.what());
            return false;
        }
        catch (const std::regex_error &r)
        {
            out_buffer.Empty();
            SetError(r.what());
            return false;
        }
    }
    // 测试
    bool Test()
    {
        if (IsNull())
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            arc().test();
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 压缩密码
    inline void SetPassword(const wchar_t *password)
    {
        if (!IsNull())
        {
            arc().setPassword(password);
        }
    }
    inline CVolString Password()
    {
        return IsNull() ? _CT("") : CVolString(arc().password().c_str());
    }
    // 清除密码
    inline void ClearPassword()
    {
        if (!IsNull())
        {
            arc().clearPassword();
        }
    }
    // 是否有密码
    inline bool IsPasswordDefined()
    {
        return IsNull() ? false : arc().isPasswordDefined();
    }
    // 保留目录结构
    inline void SetRetainDirectories(bool retain)
    {
        if (!IsNull())
        {
            arc().setRetainDirectories(retain);
        }
    }
    inline bool RetainDirectories()
    {
        return IsNull() ? false : arc().retainDirectories();
    }
    // 取压缩格式
    inline int8_t DetectedFormat()
    {
        return IsNull() ? 0 : static_cast<int8_t>(arc().detectedFormat().value());
    }
    // 取压缩包属性表
    inline std::map<bit7z::BitProperty, bit7z::BitPropVariant> &ArchiveProperties()
    {
        return IsNull() ? std::map<bit7z::BitProperty, bit7z::BitPropVariant>() : arc().archiveProperties();
    }
    // 取压缩包属性
    inline bit7z::BitPropVariant &ArchiveProperty(int32_t property)
    {
        return IsNull() ? bit7z::BitPropVariant() : arc().archiveProperty(static_cast<bit7z::BitProperty>(property));
    }
    // 取项目信息表
    inline std::vector<bit7z::BitArchiveItemInfo> &Items()
    {
        return IsNull() ? std::vector<bit7z::BitArchiveItemInfo>() : arc().items();
    }
    // 取项目属性
    inline bit7z::BitPropVariant &ItemProperty(int32_t idx, int32_t property)
    {
        return IsNull() ? bit7z::BitPropVariant() : arc().itemProperty(static_cast<uint32_t>(idx), static_cast<bit7z::BitProperty>(property));
    }
    // 取项目数
    inline int32_t ItemsCount()
    {
        return IsNull() ? -1 : static_cast<int32_t>(arc().itemsCount());
    }
    // 是否为目录项目
    inline bool IsItemFolder(int32_t idx)
    {
        return IsNull() ? false : arc().isItemFolder(static_cast<uint32_t>(idx));
    }
    // 是否为加密项目
    inline bool IsItemEncrypted(int32_t idx)
    {
        return IsNull() ? false : arc().isItemEncrypted(static_cast<uint32_t>(idx));
    }
    // 取压缩文件路径
    inline CVolString ArchivePath()
    {
        return IsNull() ? _CT("") : CVolString(arc().archivePath().c_str());
    }
    // 取文件夹数量
    inline int32_t foldersCount()
    {
        return IsNull() ? -1 : static_cast<int32_t>(arc().foldersCount());
    }
    // 取文件数量
    inline int32_t FilesCount()
    {
        return IsNull() ? -1 : static_cast<int32_t>(arc().filesCount());
    }
    // 取总长度
    inline int64_t Size()
    {
        return IsNull() ? -1 : static_cast<int64_t>(arc().size());
    }
    // 取打包长度
    inline int64_t PackSize()
    {
        return IsNull() ? -1 : static_cast<int64_t>(arc().packSize());
    }
    // 是否有加密项目
    inline bool HasEncryptedItems()
    {
        return IsNull() ? false : arc().hasEncryptedItems();
    }
    // 取分卷数量
    inline int32_t VolumesCount()
    {
        return IsNull() ? -1 : static_cast<int32_t>(arc().volumesCount());
    }
    // 是否为分卷
    inline bool IsMultiVolume()
    {
        return IsNull() ? false : arc().isMultiVolume();
    }
    // 是否为固实
    inline bool IsSolid()
    {
        return IsNull() ? false : arc().isSolid();
    }
    // 取尾项目索引
    inline int32_t EndIndex()
    {
        return IsNull() ? -1 : static_cast<int32_t>(arc().end()->index() - 1);
    }
    // 寻找项目索引
    inline int32_t FindIndex(const wchar_t *item_path)
    {
        if (IsNull())
        {
            return -1;
        }
        uint32_t itemIndex = arc().find(item_path)->index();
        return (itemIndex != arc().end()->index()) ? static_cast<int32_t>(itemIndex) : -1;
    }
    // 项目是否存在
    inline bool IsContain(const wchar_t *item_path)
    {
        return IsNull() ? false : arc().contains(item_path);
    }
}; // PivArchiveReader

class PivArchiveWirter
{
protected:
    bit7z::BitArchiveWriter *m_archive = nullptr;
    std::unique_ptr<std::vector<bit7z::byte_t>> m_buffer{nullptr};
    std::vector<std::vector<bit7z::byte_t> *> m_buffer_array;
    // PivMemoryInStream<char> m_in_stream;
    // std::vector<PivMemoryInStream<char>> m_istream_array;
    std::string m_error;
    CVolString m_progress_file;
    uint64_t m_total_size = 0;
    int32_t m_ratio = 0;

public:
    PivArchiveWirter() {}
    virtual ~PivArchiveWirter()
    {
        CloseArchive();
    }
    virtual inline bit7z::BitArchiveWriter &arc()
    {
        return *m_archive;
    }
    virtual inline bit7z::BitArchiveWriter *pArc()
    {
        return m_archive;
    }
    virtual inline void CloseArchive()
    {
        for (size_t i = 0; i < m_buffer_array.size(); ++i)
        {
            delete m_buffer_array[i];
        }
        m_buffer_array.clear();
        m_buffer.reset();
        // m_istream_array.clear();
        // m_in_stream.CloseStream(true);
        if (m_archive)
        {
            delete m_archive;
            m_archive = nullptr;
        }
    }
    virtual inline bool IsNull()
    {
        return m_archive == nullptr;
    }
    inline CVolString GetError()
    {
        return CVolString(GetWideText(m_error.c_str(), CVolMem(), NULL));
    }
    inline void SetError(const CHAR *Error)
    {
        m_error = Error;
    }

    // CallBack
    virtual int32_t WriterProgress(CVolString progressFile, int32_t progress, int64_t totalSize, int64_t progress_size, int32_t ratio) { return 0; }
    void FileNameCB(bit7z::tstring fileName)
    {
        m_progress_file.SetText(fileName.c_str());
    }
    void TotalSizeCB(uint64_t total_size)
    {
        m_total_size = total_size;
    }
    void RatioCB(uint64_t input, uint64_t output)
    {
        m_ratio = static_cast<int32_t>(100.0 * input / output);
    }
    bool ProgressCB(uint64_t progress_size)
    {
        if (progress_size == 0)
            return true;
        return !this->WriterProgress(m_progress_file, static_cast<int32_t>(100.0 * progress_size / m_total_size),
                                     static_cast<int64_t>(m_total_size), static_cast<int64_t>(progress_size), m_ratio);
    }
    void EnableFeeback()
    {
        if (!IsNull())
        {
            const bit7z::FileCallback cbFileName = std::bind(&PivArchiveWirter::FileNameCB, this, std::placeholders::_1);
            arc().setFileCallback(cbFileName);
            const bit7z::TotalCallback cbTotalSize = std::bind(&PivArchiveWirter::TotalSizeCB, this, std::placeholders::_1);
            arc().setTotalCallback(cbTotalSize);
            const bit7z::RatioCallback cbRatio = std::bind(&PivArchiveWirter::RatioCB, this, std::placeholders::_1, std::placeholders::_2);
            arc().setRatioCallback(cbRatio);
            const bit7z::ProgressCallback cbProgress = std::bind(&PivArchiveWirter::ProgressCB, this, std::placeholders::_1);
            arc().setProgressCallback(cbProgress);
        }
    }
    // 创建空压缩包
    bool CreateArchive(bool feedback, int8_t format)
    {
        CloseArchive();
        try
        {
            m_archive = new bit7z::BitArchiveWriter{piv::Archive::Get7zLib(), piv::Archive::GetInOutFormat(format)};
            if (feedback)
            {
                EnableFeeback();
            }
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            CloseArchive();
            SetError(ex.what());
            return false;
        }
    }
    // 创建自文件
    bool CreateArchive(const wchar_t *in_file, bool feedback, int8_t format, const wchar_t *password)
    {
        CloseArchive();
        try
        {
            m_archive = new bit7z::BitArchiveWriter{piv::Archive::Get7zLib(), in_file,
                                                    piv::Archive::GetInOutFormat(format), password};
            if (feedback)
            {
                EnableFeeback();
            }
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            CloseArchive();
            SetError(ex.what());
            return false;
        }
    }
    // 创建自vector
    bool CreateArchive(std::vector<bit7z::byte_t> &in_buffer, bool feedback, int8_t format, const wchar_t *password)
    {
        CloseArchive();
        try
        {
            m_archive = new bit7z::BitArchiveWriter{piv::Archive::Get7zLib(), in_buffer,
                                                    piv::Archive::GetInOutFormat(format), password};
            if (feedback)
            {
                EnableFeeback();
            }
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            CloseArchive();
            SetError(ex.what());
            return false;
        }
    }
    // 创建自输入流
    bool CreateArchive(std::istream &in_stream, bool feedback, int8_t format, const wchar_t *password)
    {
        CloseArchive();
        try
        {
            m_archive = new bit7z::BitArchiveWriter{piv::Archive::Get7zLib(), in_stream,
                                                    piv::Archive::GetInOutFormat(format), password};
            if (feedback)
            {
                EnableFeeback();
            }
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            CloseArchive();
            SetError(ex.what());
            return false;
        }
    }
    // 创建自字节集
    bool CreateArchive(CVolMem &in_buffer, bool feedback, int8_t format, const wchar_t *password /* , bool copy_data*/)
    {
        CloseArchive();
        try
        {
            // m_in_stream.OpenStream(in_buffer, copy_data);
            m_buffer.reset(new std::vector<bit7z::byte_t>{reinterpret_cast<bit7z::byte_t *>(in_buffer.GetPtr()),
                                                          reinterpret_cast<bit7z::byte_t *>(in_buffer.GetPtr()) + in_buffer.GetSize()});
            m_archive = new bit7z::BitArchiveWriter{piv::Archive::Get7zLib(), *m_buffer,
                                                    piv::Archive::GetInOutFormat(format), password};
            if (feedback)
            {
                EnableFeeback();
            }
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            CloseArchive();
            SetError(ex.what());
            return false;
        }
    }
    // 创建自文件资源
    /*
    bool CreateArchive(size_t resid, bool feedback, int8_t format, const wchar_t *password)
    {
        CloseArchive();
        try
        {
            m_in_stream.OpenStream(resid);
            m_archive = new bit7z::BitArchiveWriter{piv::Archive::Get7zLib(), m_in_stream.is(),
                                                    piv::Archive::GetInOutFormat(format), password};
            if (feedback)
            {
                EnableFeeback();
            }
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            CloseArchive();
            SetError(ex.what());
            return false;
        }
    }
    */
    // 通用

    // 添加项目
    bool AddItems(CMStringArray &path_array)
    {
        if (IsNull())
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            std::vector<bit7z::tstring> in_paths;
            piv::Archive::VolstrArrToVector(path_array, in_paths);
            arc().addItems(in_paths);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 添加别名项目
    bool AddItems(std::map<CVolString, CVolString> &path_map)
    {
        if (IsNull())
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            std::map<bit7z::tstring, bit7z::tstring> in_paths;
            piv::Archive::VolStrMapToStdMap(path_map, in_paths);
            arc().addItems(in_paths);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 添加文件
    bool AddFile(const wchar_t *in_path, const wchar_t *alias)
    {
        if (IsNull())
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            arc().addFile(in_path, alias);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 添加内存流
    /*
    bool AddFile(std::istream &in_stream, const wchar_t *path)
    {
        if (IsNull())
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            arc().addFile(in_stream, path);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    */
    // 添加字节集
    bool AddFile(CVolMem &in_buffer, const wchar_t *path /* , bool copy_data*/)
    {
        if (IsNull())
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            // PivMemoryInStream<char> in_stream;
            // in_stream.OpenStream(in_buffer, copy_data);
            // arc().addFile(in_stream.is(), path);
            // m_istream_array.push_back(in_stream);
            bit7z::buffer_t *buffer = piv::Archive::MemToVectorPtr(in_buffer);
            arc().addFile(*buffer, path);
            m_buffer_array.push_back(buffer);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 添加内存
    bool AddFile(ptrdiff_t in_buffer, ptrdiff_t buffer_len, const wchar_t *path /*, bool copy_data*/)
    {
        if (IsNull())
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            // PivMemoryInStream<char> in_stream;
            // in_stream.OpenStream(in_buffer, buffer_len, copy_data);
            // arc().addFile(in_stream.is(), path);
            // m_istream_array.push_back(in_stream);
            bit7z::buffer_t *buffer = piv::Archive::MemToVectorPtr(in_buffer, buffer_len);
            arc().addFile(*buffer, path);
            m_buffer_array.push_back(buffer);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 添加文件资源内存
    /*
    bool AddFile(size_t resid, ptrdiff_t buffer_len, const wchar_t *path)
    {
        if (IsNull())
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            PivMemoryInStream<char> in_stream;
            in_stream.OpenStream(resid);
            arc().addFile(in_stream.is(), path);
            m_istream_array.push_back(in_stream);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    */
    // 添加文件数组
    bool AddFiles(CMStringArray &in_files)
    {
        if (IsNull())
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            std::vector<bit7z::tstring> in_paths;
            piv::Archive::VolstrArrToVector(in_files, in_paths);
            arc().addFiles(in_paths);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 添加匹配文件
    bool AddFiles(const wchar_t *in_dir, const wchar_t *filter, bool recursion)
    {
        if (IsNull())
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            arc().addFiles(in_dir, filter, recursion);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 添加目录
    bool AddDirectory(const wchar_t *in_dir)
    {
        if (IsNull())
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            arc().addDirectory(in_dir);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 压缩到文件
    bool CompressTo(const wchar_t *out_file)
    {
        if (IsNull())
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            arc().compressTo(out_file);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 压缩到字节集
    bool CompressTo(CVolMem &out_buffer)
    {
        out_buffer.Empty();
        if (IsNull())
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            std::vector<bit7z::byte_t> buffer;
            arc().compressTo(buffer);
            out_buffer.Append(buffer.data(), buffer.size());
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    // 取压缩格式
    inline int8_t CompressionFormat()
    {
        return IsNull() ? 0 : static_cast<int8_t>(arc().compressionFormat().value());
    }
    // 取项目数量
    inline int32_t ItemsCount()
    {
        return IsNull() ? 0 : static_cast<int32_t>(arc().itemsCount());
    }
    // 压缩密码
    inline void SetPassword(const wchar_t *password)
    {
        if (!IsNull())
        {
            arc().setPassword(password);
        }
    }
    inline CVolString Password()
    {
        return IsNull() ? _CT("") : CVolString(arc().password().c_str());
    }
    // 压缩等级
    inline void SetCompressionLevel(int32_t level)
    {
        if (!IsNull())
        {
            arc().setCompressionLevel(piv::Archive::GetCompressionLevel(level));
        }
    }
    inline int32_t CompressionLevel()
    {
        return IsNull() ? 0 : static_cast<int32_t>(arc().compressionLevel());
    }
    // 压缩方法
    inline void SetCompressionMethod(int32_t method)
    {
        if (!IsNull())
        {
            arc().setCompressionMethod(piv::Archive::GetCompressionMethod(method));
        }
    }
    inline int32_t CompressionMethod()
    {
        return IsNull() ? 0 : static_cast<int32_t>(arc().compressionMethod());
    }
    // 字典大小
    inline void SetDictionarySize(int32_t dictionary_size)
    {
        if (!IsNull())
        {
            arc().setDictionarySize(static_cast<uint32_t>(dictionary_size));
        }
    }
    inline int32_t DictionarySize()
    {
        return IsNull() ? 0 : static_cast<int32_t>(arc().dictionarySize());
    }
    // 单词大小
    inline void SetWordSize(int32_t word_size)
    {
        if (!IsNull())
        {
            arc().setWordSize(static_cast<uint32_t>(word_size));
        }
    }
    inline int32_t WordSize()
    {
        return IsNull() ? 0 : static_cast<int32_t>(arc().wordSize());
    }
    // 固实压缩
    inline void SetSolidMode(bool solid_mode)
    {
        if (!IsNull())
        {
            arc().setSolidMode(solid_mode);
        }
    }
    inline bool SolidMode()
    {
        return IsNull() ? false : arc().solidMode();
    }
    // 更新方式
    inline void SetUpdateMode(int32_t mode)
    {
        if (!IsNull())
        {
            arc().setUpdateMode(piv::Archive::GetUpdateMode(mode));
        }
    }
    inline int32_t UpdateMode()
    {
        return IsNull() ? 0 : static_cast<int32_t>(arc().updateMode());
    }
    // 分卷大小
    inline void SetVolumeSize(int32_t volume_size)
    {
        if (!IsNull())
        {
            arc().setVolumeSize(static_cast<uint32_t>(volume_size));
        }
    }
    inline int32_t VolumeSize()
    {
        return IsNull() ? 0 : static_cast<int32_t>(arc().volumeSize());
    }
    // 线程数
    inline void SetThreadsCount(int32_t threads_count)
    {
        if (!IsNull())
        {
            arc().setThreadsCount(static_cast<uint32_t>(threads_count));
        }
    }
    inline int32_t ThreadsCount()
    {
        return IsNull() ? 0 : static_cast<int32_t>(arc().threadsCount());
    }
    // 保留目录结构
    inline void SetRetainDirectories(bool retain)
    {
        if (!IsNull())
        {
            arc().setRetainDirectories(retain);
        }
    }
    inline bool RetainDirectories()
    {
        return IsNull() ? false : arc().retainDirectories();
    }
    // 设置密码
    inline void SetPassword(const wchar_t *password, bool crypt_headers)
    {
        if (!IsNull())
        {
            arc().setPassword(password, crypt_headers);
        }
    }
    // 清除密码
    inline void ClearPassword()
    {
        if (!IsNull())
        {
            arc().clearPassword();
        }
    }
    // 是否有密码
    inline bool IsPasswordDefined()
    {
        return IsNull() ? false : arc().isPasswordDefined();
    }
    // 是否加密文件头
    inline bool CryptHeaders()
    {
        return IsNull() ? false : arc().cryptHeaders();
    }
}; // PivArchiveWirter

class PivArchiveEditor : public PivArchiveWirter
{
protected:
    bit7z::BitArchiveEditor *m_archive = nullptr;

public:
    PivArchiveEditor() {}
    ~PivArchiveEditor() override
    {
        CloseArchive();
    }
    inline bit7z::BitArchiveEditor &arc() override
    {
        return *m_archive;
    }
    inline bit7z::BitArchiveEditor *pArc() override
    {
        return m_archive;
    }
    inline void CloseArchive() override
    {
        for (size_t i = 0; i < m_buffer_array.size(); ++i)
        {
            delete m_buffer_array[i];
        }
        m_buffer_array.clear();
        // m_istream_array.clear();
        if (m_archive)
        {
            delete m_archive;
            m_archive = nullptr;
        }
    }
    inline bool IsNull() override
    {
        return m_archive == nullptr;
    }
    // 打开文件
    bool OpenFile(const wchar_t *in_file, bool feedback, int8_t format, int32_t update_mode, const wchar_t *password)
    {
        CloseArchive();
        try
        {
            m_archive = new bit7z::BitArchiveEditor{piv::Archive::Get7zLib(), in_file,
                                                    piv::Archive::GetInOutFormat(format), password};
            arc().setUpdateMode(piv::Archive::GetUpdateMode(update_mode));
            if (feedback)
            {
                EnableFeeback();
            }
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            CloseArchive();
            SetError(ex.what());
            return false;
        }
    }
    // 应用更改
    bool ApplyChanges()
    {
        if (IsNull())
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            arc().applyChanges();
            for (size_t i = 0; i < m_buffer_array.size(); ++i)
            {
                delete m_buffer_array[i];
            }
            m_buffer_array.clear();
            // m_istream_array.clear();
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 索引项目更名
    bool RenameItem(int32_t index, const wchar_t *new_path)
    {
        if (IsNull())
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            arc().renameItem(static_cast<uint32_t>(index), new_path);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 路径项目更名
    bool RenameItem(const wchar_t *old_path, const wchar_t *new_path)
    {
        if (IsNull())
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            arc().renameItem(old_path, new_path);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 更新索引项目自文件
    bool UpdateItem(int32_t index, const wchar_t *in_file)
    {
        if (IsNull())
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            arc().updateItem(static_cast<uint32_t>(index), in_file);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 更新索引项目自字节集
    bool UpdateItem(int32_t index, CVolMem &in_buffer /* , bool copy_data*/)
    {
        if (IsNull())
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            /*
            PivMemoryInStream<char> in_stream;
            in_stream.OpenStream(in_buffer, copy_data);
            arc().updateItem(static_cast<uint32_t>(index), in_stream.is());
            m_istream_array.push_back(in_stream);
            */
            bit7z::buffer_t *buffer = piv::Archive::MemToVectorPtr(in_buffer);
            arc().updateItem(static_cast<uint32_t>(index), *buffer);
            m_buffer_array.push_back(buffer);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 更新索引项目自内存流
    bool UpdateItem(int32_t index, std::istream &in_stream)
    {
        if (IsNull())
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            arc().updateItem(static_cast<uint32_t>(index), in_stream);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 更新路径项目自文件
    bool UpdateItem(const wchar_t *item_path, const wchar_t *in_file)
    {
        if (IsNull())
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            arc().updateItem(item_path, in_file);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 更新路径项目自字节集
    bool UpdateItem(const wchar_t *item_path, CVolMem &in_buffer /* , bool copy_data*/)
    {
        if (IsNull())
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            /*
            PivMemoryInStream<char> in_stream;
            in_stream.OpenStream(in_buffer, copy_data);
            arc().updateItem(item_path, in_stream.is());
            m_istream_array.push_back(in_stream);
            */
            bit7z::buffer_t *buffer = piv::Archive::MemToVectorPtr(in_buffer);
            arc().updateItem(item_path, *buffer);
            m_buffer_array.push_back(buffer);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 更新路径项目自内存流
    bool UpdateItem(const wchar_t *item_path, std::istream &in_stream)
    {
        if (IsNull())
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            arc().updateItem(item_path, in_stream);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 删除索引项目
    bool DeleteItem(int32_t index)
    {
        if (IsNull())
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            arc().deleteItem(static_cast<uint32_t>(index));
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    // 删除路径项目
    bool DeleteItem(const wchar_t *item_path)
    {
        if (IsNull())
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            arc().deleteItem(item_path);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
}; // PivArchiveEditor

#endif // _PIV_BIT7Z_HPP
