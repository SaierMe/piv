/*********************************************\
 * 火山视窗 - bit7z封装类                    *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef _PIV_BIT7Z_HPP
#define _PIV_BIT7Z_HPP

#ifndef __VOL_BASE_H__
#include <sys/base/libs/win_base/vol_base.h>
#endif

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
#ifndef BIT7Z_AUTO_PREFIX_LONG_PATHS
#define BIT7Z_AUTO_PREFIX_LONG_PATHS
#endif
#ifndef BIT7Z_PATH_SANITIZATION
#define BIT7Z_PATH_SANITIZATION
#endif

#include "bit7z.hpp"
#include "bitarchiveeditor.hpp"

#include <mutex>
#include <memory>
#include <atomic>

namespace piv
{
    namespace Archive
    {
        static std::atomic<bit7z::Bit7zLibrary *> m_7zLib = nullptr; // 7z.dll动态库
        static std::string g_error{"OK"};                            // 错误信息
        static std::mutex g_mutex;

        /**
         * @brief 加载静态库
         * @param library_path 7z.dll的路径
         * @return 是否加载成功
         */
        static bool Load7zLib(const bit7z::tstring &library_path = bit7z::kDefaultLibrary)
        {
            try
            {
                bit7z::Bit7zLibrary *p = m_7zLib.load(std::memory_order_relaxed);
                std::atomic_thread_fence(std::memory_order_acquire);
                if (p == nullptr)
                {
                    std::lock_guard<std::mutex> lock(g_mutex);
                    p = m_7zLib.load(std::memory_order_relaxed);
                    if (p == nullptr)
                    {
                        p = new bit7z::Bit7zLibrary{library_path};
                        std::atomic_thread_fence(std::memory_order_release);
                        m_7zLib.store(p, std::memory_order_relaxed);
                        p->setLargePageMode();
                    }
                }
                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        /**
         * @brief 卸载7z.dll动态库
         */
        static void Free7zLib()
        {
            bit7z::Bit7zLibrary *p = m_7zLib.load(std::memory_order_relaxed);
            std::atomic_thread_fence(std::memory_order_acquire);
            if (p != nullptr)
            {
                std::lock_guard<std::mutex> lock(g_mutex);
                p = m_7zLib.load(std::memory_order_relaxed);
                if (p != nullptr)
                {
                    delete p;
                    std::atomic_thread_fence(std::memory_order_release);
                    m_7zLib.store(nullptr, std::memory_order_relaxed);
                }
            }
        }

        /**
         * @brief 获取7z动态库实例
         */
        inline static bit7z::Bit7zLibrary &Get7zLib()
        {
            bit7z::Bit7zLibrary *p = m_7zLib.load(std::memory_order_relaxed);
            std::atomic_thread_fence(std::memory_order_acquire);
            if (p == nullptr)
            {
                std::lock_guard<std::mutex> lock(g_mutex);
                p = m_7zLib.load(std::memory_order_relaxed);
                if (p == nullptr)
                {
                    p = new bit7z::Bit7zLibrary{bit7z::kDefaultLibrary};
                    std::atomic_thread_fence(std::memory_order_release);
                    m_7zLib.store(p, std::memory_order_relaxed);
                    p->setLargePageMode();
                }
            }
            return *p;
        }

        /**
         * @brief 返回存档输出格式
         * @param value 输出格式值
         */
        static const bit7z::BitInOutFormat &GetOutFormat(int8_t value)
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
            return bit7z::BitFormat::SevenZip;
        }

        /**
         * @brief 返回存档输入格式
         * @param value 输入格式值
         */
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
            case 0xC4:
                return bit7z::BitFormat::Vhdx;
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
            case 0xD7:
                return bit7z::BitFormat::Swf;
            case 0xD8:
                return bit7z::BitFormat::Swfc;
            case 0xD9:
                return bit7z::BitFormat::Ntfs;
            case 0xDA:
                return bit7z::BitFormat::Fat;
            case 0xDB:
                return bit7z::BitFormat::Mbr;
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
    } // namespace Archive
} // namespace piv

class PivArchiveOperate
{
public:
    PivArchiveOperate() {}
    ~PivArchiveOperate() {}

    static bool LoadResIdData(int32_t resId, std::vector<bit7z::byte_t> &buffer)
    {
        if (resId == 0)
            return false;
        HMODULE hModule = g_objVolApp.GetInstanceHandle();
        HRSRC hSrc = ::FindResourceW(hModule, MAKEINTRESOURCE(static_cast<WORD>(resId)), RT_RCDATA);
        if (hSrc == NULL)
            return false;
        HGLOBAL resdata = ::LoadResource(hModule, hSrc);
        if (resdata == NULL)
            return false;
        bit7z::byte_t *data = reinterpret_cast<bit7z::byte_t *>(::LockResource(resdata));
        buffer.assign(data, data + ::SizeofResource(hModule, hSrc));
        return !buffer.empty();
    }

    /**
     * @brief 取最后错误
     * @return
     */
    static CVolString GetLastError()
    {
        std::lock_guard<std::mutex> guard(piv::Archive::g_mutex);
        return CVolString{piv::Archive::g_error.c_str()};
    }

    /**
     * @brief 置错误信息
     * @param msg
     */
    static void SetError(const char *msg)
    {
        std::lock_guard<std::mutex> guard(piv::Archive::g_mutex);
        piv::Archive::g_error = msg;
    }

    /**
     * @brief 添加到压缩包
     * @param paths 路径数组
     * @param out_file 输出文件名
     * @param password 压缩密码
     * @param format 压缩格式
     * @param level 压缩等级
     * @param update_mode 更新方式
     * @return
     */
    static bool CompressPaths(const CMStringArray &paths, const bit7z::tstring &out_file, const bit7z::tstring &password, const bit7z::BitInOutFormat &format, int32_t level, int32_t update_mode)
    {
        if (paths.IsEmpty())
        {
            SetError("未提供所欲压缩的文件或目录.");
            return false;
        }
        try
        {
            std::vector<bit7z::tstring> in_paths;
            for (INT_P i = 0; i < paths.GetCount(); i++)
            {
                in_paths.push_back(bit7z::tstring{paths.GetAt(i)});
            }
            bit7z::BitFileCompressor Compressor{piv::Archive::Get7zLib(), format};
            Compressor.setPassword(password);
            Compressor.setCompressionLevel(static_cast<bit7z::BitCompressionLevel>(level));
            Compressor.setUpdateMode(static_cast<bit7z::UpdateMode>(update_mode));
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

    /**
     * @brief 添加别名到压缩包
     * @param path_pair 路径键值对
     * @param out_file 输出文件名
     * @param password 压缩密码
     * @param format 压缩格式
     * @param level 压缩等级
     * @param update_mode 更新方式
     * @return
     */
    static bool CompressPaths(const std::map<bit7z::tstring, bit7z::tstring> &in_paths, const bit7z::tstring &out_file, const bit7z::tstring &password, const bit7z::BitInOutFormat &format, int32_t level, int32_t update_mode)
    {
        if (in_paths.empty())
        {
            SetError("未提供所欲压缩的文件或目录.");
            return false;
        }
        try
        {
            bit7z::BitFileCompressor Compressor{piv::Archive::Get7zLib(), format};
            Compressor.setPassword(password);
            Compressor.setCompressionLevel(static_cast<bit7z::BitCompressionLevel>(level));
            Compressor.setUpdateMode(static_cast<bit7z::UpdateMode>(update_mode));
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
    static bool CompressPaths(const std::map<CVolString, CVolString> &path_pair, const bit7z::tstring &out_file, const bit7z::tstring &password, const bit7z::BitInOutFormat &format, int32_t level, int32_t update_mode)
    {
        if (path_pair.empty())
        {
            SetError("未提供所欲压缩的文件或目录.");
            return false;
        }
        std::map<bit7z::tstring, bit7z::tstring> in_paths;
        for (auto iter = path_pair.begin(); iter != path_pair.end(); iter++)
        {
            in_paths[bit7z::tstring{iter->first.GetText()}] = bit7z::tstring{iter->second.GetText()};
        }
        return CompressPaths(in_paths, out_file, password, format, level, update_mode);
    }

    /**
     * @brief 添加文件到压缩包
     * @param paths 路径数组
     * @param out_file 输出文件名
     * @param password 压缩密码
     * @param format 压缩格式
     * @param level 压缩等级
     * @param update_mode 更新方式
     * @return
     */
    static bool CompressFiles(const std::vector<bit7z::tstring> &in_paths, const bit7z::tstring &out_file, const bit7z::tstring &password, const bit7z::BitInOutFormat &format, int32_t level, int32_t update_mode)
    {
        if (in_paths.empty())
        {
            SetError("未提供所欲压缩的文件.");
            return false;
        }
        try
        {
            bit7z::BitFileCompressor Compressor{piv::Archive::Get7zLib(), format};
            Compressor.setPassword(password);
            Compressor.setCompressionLevel(static_cast<bit7z::BitCompressionLevel>(level));
            Compressor.setUpdateMode(static_cast<bit7z::UpdateMode>(update_mode));
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
    static bool CompressFiles(const CMStringArray &paths, const bit7z::tstring &out_file, const bit7z::tstring &password, const bit7z::BitInOutFormat &format, int32_t level, int32_t update_mode)
    {
        if (paths.IsEmpty())
        {
            SetError("未提供所欲压缩的文件.");
            return false;
        }
        std::vector<bit7z::tstring> in_paths;
        for (INT_P i = 0; i < paths.GetCount(); i++)
        {
            in_paths.push_back(bit7z::tstring{paths.GetAt(i)});
        }
        return CompressFiles(in_paths, out_file, password, format, level, update_mode);
    }

    /**
     * @brief 添加匹配文件到压缩包
     * @param in_dir 输入目录
     * @param out_file 输出文件名
     * @param filter 筛选器
     * @param recursive 递归子目录
     * @param password 压缩密码
     * @param format 压缩格式
     * @param level 压缩等级
     * @param update_mode 更新方式
     * @return
     */
    static bool CompressFiles(const bit7z::tstring &in_dir, const bit7z::tstring &out_file, const bit7z::tstring &filter, bool recursive, const bit7z::tstring &password, const bit7z::BitInOutFormat &format, int32_t level, int32_t update_mode)
    {
        try
        {
            bit7z::BitFileCompressor Compressor{piv::Archive::Get7zLib(), format};
            Compressor.setPassword(password);
            Compressor.setCompressionLevel(static_cast<bit7z::BitCompressionLevel>(level));
            Compressor.setUpdateMode(static_cast<bit7z::UpdateMode>(update_mode));
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

    /**
     * @brief 添加目录到压缩包
     * @param in_dir 输入目录
     * @param out_file 输出文件名
     * @param password 压缩密码
     * @param format 压缩格式
     * @param level 压缩等级
     * @param update_mode 更新方式
     * @return
     */
    static bool CompressDirectory(const bit7z::tstring &in_dir, const bit7z::tstring &out_file, const bit7z::tstring &password, const bit7z::BitInOutFormat &format, int32_t level, int32_t update_mode)
    {
        try
        {
            bit7z::BitFileCompressor Compressor{piv::Archive::Get7zLib(), format};
            Compressor.setPassword(password);
            Compressor.setCompressionLevel(static_cast<bit7z::BitCompressionLevel>(level));
            Compressor.setUpdateMode(static_cast<bit7z::UpdateMode>(update_mode));
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

    /**
     * @brief 解压到
     * @param in_file 压缩文件
     * @param resId 资源文件ID
     * @param out_dir 输出路径
     * @param password 密码
     * @param format 解压格式
     * @return
     */
    static bool Extract(const bit7z::tstring &in_file, const bit7z::tstring &out_dir, const bit7z::tstring &password, const bit7z::BitInFormat &format)
    {
        try
        {
            bit7z::BitFileExtractor extractor{piv::Archive::Get7zLib(), format};
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

    static bool Extract(int32_t resId, const bit7z::tstring &out_dir, const bit7z::tstring &password, const bit7z::BitInFormat &format)
    {
        std::vector<bit7z::byte_t> buffer;
        if (!LoadResIdData(resId, buffer))
            return false;
        try
        {
            bit7z::BitMemExtractor extractor{piv::Archive::Get7zLib(), format};
            extractor.setPassword(password);
            extractor.extract(buffer, out_dir);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 解压到内存
     * @param in_file 压缩文件
     * @param resId 资源文件ID
     * @param out_buffer 解压结果
     * @param password 密码
     * @param format 解压格式
     * @return
     */
    static bool Extract(const bit7z::tstring &in_file, std::map<CVolString, CVolMem> &out_buffer, const bit7z::tstring &password, const bit7z::BitInFormat &format)
    {
        out_buffer.clear();
        try
        {
            bit7z::BitFileExtractor extractor{piv::Archive::Get7zLib(), format};
            extractor.setPassword(password);
            bit7z::BitInputArchive in_archive{extractor, in_file};
            for (auto itr = in_archive.begin(); itr != in_archive.end(); itr++)
            {
                if (!itr->isDir())
                {
                    CVolString strPath;
                    strPath.SetText(itr->path().c_str());
                    out_buffer[strPath] = CVolMem();
                    out_buffer[strPath].Alloc(static_cast<INT_P>(itr->size()), true);
                    in_archive.extractTo(reinterpret_cast<bit7z::byte_t *>(out_buffer[strPath].GetPtr()), static_cast<std::size_t>(itr->size()), itr->index());
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

    static bool Extract(int32_t resId, std::map<CVolString, CVolMem> &out_buffer, const bit7z::tstring &password, const bit7z::BitInFormat &format)
    {
        out_buffer.clear();
        std::vector<bit7z::byte_t> buffer;
        if (!LoadResIdData(resId, buffer))
            return false;
        try
        {
            bit7z::BitMemExtractor extractor{piv::Archive::Get7zLib(), format};
            extractor.setPassword(password);
            bit7z::BitInputArchive in_archive{extractor, buffer};
            for (auto itr = in_archive.begin(); itr != in_archive.end(); itr++)
            {
                if (!itr->isDir())
                {
                    CVolString strPath;
                    strPath.SetText(itr->path().c_str());
                    out_buffer[strPath] = CVolMem();
                    out_buffer[strPath].Alloc(static_cast<INT_P>(itr->size()), true);
                    in_archive.extractTo(reinterpret_cast<bit7z::byte_t *>(out_buffer[strPath].GetPtr()), static_cast<std::size_t>(itr->size()), itr->index());
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

    /**
     * @brief 解压匹配文件
     * @param in_file 压缩文件
     * @param resId 资源文件ID
     * @param filter 筛选器
     * @param out_dir 输出路径
     * @param password 密码
     * @param isExclude 是否排除匹配项
     * @param format 解压格式
     * @return
     */
    static bool Extract(const bit7z::tstring &in_file, const bit7z::tstring &filter, const bit7z::tstring &out_dir, const bit7z::tstring &password, bool isExclude, const bit7z::BitInFormat &format)
    {
        try
        {
            bit7z::BitFileExtractor extractor{piv::Archive::Get7zLib(), format};
            extractor.setPassword(password);
            extractor.extractMatching(in_file, filter, out_dir, isExclude ? bit7z::FilterPolicy::Exclude : bit7z::FilterPolicy::Include);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    static bool Extract(int32_t resId, const bit7z::tstring &filter, const bit7z::tstring &out_dir, const bit7z::tstring &password, bool isExclude, const bit7z::BitInFormat &format)
    {
        std::vector<bit7z::byte_t> buffer;
        if (!LoadResIdData(resId, buffer))
            return false;
        try
        {
            bit7z::BitMemExtractor extractor{piv::Archive::Get7zLib(), format};
            extractor.setPassword(password);
            extractor.extractMatching(buffer, filter, out_dir, isExclude ? bit7z::FilterPolicy::Exclude : bit7z::FilterPolicy::Include);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 解压匹配文件到字节集
     * @param in_file 压缩文件
     * @param resId 资源文件ID
     * @param filter 筛选器
     * @param out_buffer 输出数据
     * @param password 密码
     * @param out_file 输出文件名
     * @param isExclude 是否排除匹配项
     * @param format 解压格式
     * @return
     */
    static bool Extract(const bit7z::tstring &in_file, const bit7z::tstring &filter, CVolMem &out_buffer, const bit7z::tstring &password, CVolString &out_file, bool isExclude, const bit7z::BitInFormat &format)
    {
        out_file.Empty();
        try
        {
            if (filter.empty())
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::FilterNotSpecified));
            bit7z::BitFileExtractor extractor{piv::Archive::Get7zLib(), format};
            extractor.setPassword(password);
            bit7z::BitInputArchive in_archive(extractor, in_file);
            for (const auto &item : in_archive)
            {
                bool item_matches = bit7z::filesystem::fsutil::wildcard_match(filter, item.path());
                if (item_matches != isExclude)
                {
                    out_buffer.Alloc(static_cast<INT_P>(item.size()), true);
                    in_archive.extractTo(reinterpret_cast<bit7z::byte_t *>(out_buffer.GetPtr()), static_cast<std::size_t>(out_buffer.GetSize()), item.index());
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

    static bool Extract(int32_t resId, const bit7z::tstring &filter, CVolMem &out_buffer, const bit7z::tstring &password, CVolString &out_file, bool isExclude, const bit7z::BitInFormat &format)
    {
        out_file.Empty();
        std::vector<bit7z::byte_t> buffer;
        if (!LoadResIdData(resId, buffer))
            return false;
        try
        {
            if (filter.empty())
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::FilterNotSpecified));
            bit7z::BitMemExtractor extractor{piv::Archive::Get7zLib(), format};
            extractor.setPassword(password);
            bit7z::BitInputArchive in_archive(extractor, buffer);
            for (const auto &item : in_archive)
            {
                bool item_matches = bit7z::filesystem::fsutil::wildcard_match(filter, item.path());
                if (item_matches != isExclude)
                {
                    out_buffer.Alloc(static_cast<INT_P>(item.size()), true);
                    in_archive.extractTo(reinterpret_cast<bit7z::byte_t *>(out_buffer.GetPtr()), static_cast<std::size_t>(out_buffer.GetSize()), item.index());
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

    /**
     * @brief 解压正则匹配文件
     * @param in_file 压缩文件
     * @param resId 资源文件ID
     * @param regex 正则表达式
     * @param out_dir 输出路径
     * @param password 密码
     * @param isExclude 是否排除匹配项
     * @param format 解压格式
     * @return
     */
    static bool ExtractRegex(const bit7z::tstring &in_file, const bit7z::tstring &regex, const bit7z::tstring &out_dir, const bit7z::tstring &password, bool isExclude, const bit7z::BitInFormat &format)
    {
        try
        {
            bit7z::BitFileExtractor extractor{piv::Archive::Get7zLib(), format};
            extractor.setPassword(password);
            extractor.extractMatchingRegex(in_file, regex, out_dir, isExclude ? bit7z::FilterPolicy::Exclude : bit7z::FilterPolicy::Include);
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

    static bool ExtractRegex(int32_t resId, const bit7z::tstring &regex, const bit7z::tstring &out_dir, const bit7z::tstring &password, bool isExclude, const bit7z::BitInFormat &format)
    {
        std::vector<bit7z::byte_t> buffer;
        if (!LoadResIdData(resId, buffer))
            return false;
        try
        {
            bit7z::BitMemExtractor extractor{piv::Archive::Get7zLib(), format};
            extractor.setPassword(password);
            extractor.extractMatchingRegex(buffer, regex, out_dir, isExclude ? bit7z::FilterPolicy::Exclude : bit7z::FilterPolicy::Include);
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

    /**
     * @brief 解压正则匹配文件到字节集
     * @param in_file 压缩文件
     * @param resId 资源文件ID
     * @param regex 正则表达式
     * @param out_buffer 输出数据
     * @param password 密码
     * @param out_file 输出文件名
     * @param isExclude 是否排除匹配项
     * @param format 解压格式
     * @return
     */
    static bool ExtractRegex(const bit7z::tstring &in_file, const bit7z::tstring &regex, CVolMem &out_buffer, const bit7z::tstring &password, CVolString &out_file, bool isExclude, const bit7z::BitInFormat &format)
    {
        out_file.Empty();
        try
        {
            if (regex.empty())
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::FilterNotSpecified));
            bit7z::BitFileExtractor extractor{piv::Archive::Get7zLib(), format};
            extractor.setPassword(password);
            bit7z::BitInputArchive in_archive(extractor, in_file);
            const bit7z::tregex regex_filter(regex, bit7z::tregex::ECMAScript | bit7z::tregex::optimize);
            for (const auto &item : in_archive)
            {
                bool item_matches = std::regex_match(item.path(), regex_filter);
                if (item_matches != isExclude)
                {
                    out_buffer.Alloc(static_cast<INT_P>(item.size()), true);
                    in_archive.extractTo(reinterpret_cast<bit7z::byte_t *>(out_buffer.GetPtr()), static_cast<std::size_t>(out_buffer.GetSize()), item.index());
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

    static bool ExtractRegex(int32_t resId, const bit7z::tstring &regex, CVolMem &out_buffer, const bit7z::tstring &password, CVolString &out_file, bool isExclude, const bit7z::BitInFormat &format)
    {
        out_file.Empty();
        std::vector<bit7z::byte_t> buffer;
        if (!LoadResIdData(resId, buffer))
            return false;
        try
        {
            if (regex.empty())
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::FilterNotSpecified));
            bit7z::BitMemExtractor extractor{piv::Archive::Get7zLib(), format};
            extractor.setPassword(password);
            bit7z::BitInputArchive in_archive(extractor, buffer);
            const bit7z::tregex regex_filter(regex, bit7z::tregex::ECMAScript | bit7z::tregex::optimize);
            for (const auto &item : in_archive)
            {
                bool item_matches = std::regex_match(item.path(), regex_filter);
                if (item_matches != isExclude)
                {
                    out_buffer.Alloc(static_cast<INT_P>(item.size()), true);
                    in_archive.extractTo(reinterpret_cast<bit7z::byte_t *>(out_buffer.GetPtr()), static_cast<std::size_t>(out_buffer.GetSize()), item.index());
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

    /**
     * @brief 测试
     * @param in_file 压缩文件
     * @param password 密码
     * @param format 解压格式
     * @return
     */
    static bool Test(const bit7z::tstring &in_file, const bit7z::tstring &password, const bit7z::BitInFormat &format)
    {
        try
        {
            bit7z::BitFileExtractor extractor{piv::Archive::Get7zLib(), format};
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

/**
 * @brief 存档解压类
 */
class PivArchiveReader
{
private:
    std::unique_ptr<bit7z::BitArchiveReader> m_archive{nullptr};
    std::unique_ptr<std::vector<bit7z::byte_t>> m_buffer{nullptr};
    std::string m_error;
    uint64_t m_total_size = 0;
    int32_t m_ratio = 0;
    std::wstring m_progress_file;

    /**
     * @brief 置最后错误
     * @param Error 错误信息
     */
    inline void SetError(const CHAR *Error)
    {
        m_error = Error;
    }

    void TotalSizeCB(uint64_t total_size)
    {
        m_total_size = total_size;
    }

    void RatioCB(uint64_t input, uint64_t output)
    {
        if (output != 0)
            m_ratio = static_cast<int32_t>(100 * input / output);
    }

    bool ProgressCB(uint64_t progress_size)
    {
        if (progress_size == 0 || m_total_size == 0)
            return true;
        return !this->ExtractProgress(m_progress_file.c_str(), static_cast<int32_t>(100 * progress_size / m_total_size),
                                      static_cast<int64_t>(m_total_size), static_cast<int64_t>(progress_size), m_ratio);
    }

    void FileNameCB(bit7z::tstring fileName)
    {
        m_progress_file = fileName;
    }

public:
    PivArchiveReader() {}
    ~PivArchiveReader() {}

    inline bit7z::BitArchiveReader &data()
    {
        return *m_archive;
    }

    inline bit7z::BitArchiveReader *pdata()
    {
        return m_archive.get();
    }

    /**
     * @brief 是否为空
     * @return
     */
    inline bool IsNull()
    {
        return m_archive == nullptr;
    }

    /**
     * @brief 取最后错误
     * @return
     */
    inline CVolString GetError()
    {
        return CVolString(GetWideText(m_error.c_str(), CVolMem(), NULL));
    }

    /**
     * @brief 关闭
     */
    inline void CloseArchive()
    {
        m_buffer.reset();
        m_archive.reset();
    }

    /**
     * @brief 解压进度改变(虚拟方法)
     * @param progressFile 文件路径
     * @param progress 解压进度
     * @param totalSize 总解压大小
     * @param progress_size 已解压大小
     * @param ratio 整体压缩比
     * @return
     */
    virtual int32_t ExtractProgress(const wchar_t *progressFile, int32_t progress, int64_t totalSize, int64_t progress_size, int32_t ratio) { return 0; }

    /**
     * @brief 启用进度反馈
     */
    void EnableFeeback(bool enable)
    {
        if (!m_archive)
            return;
        if (enable)
        {
            m_archive->setTotalCallback(std::bind(&PivArchiveReader::TotalSizeCB, this, std::placeholders::_1));
            m_archive->setRatioCallback(std::bind(&PivArchiveReader::RatioCB, this, std::placeholders::_1, std::placeholders::_2));
            m_archive->setProgressCallback(std::bind(&PivArchiveReader::ProgressCB, this, std::placeholders::_1));
            m_archive->setFileCallback(std::bind(&PivArchiveReader::FileNameCB, this, std::placeholders::_1));
        }
        else
        {
            m_archive->setTotalCallback(nullptr);
            m_archive->setProgressCallback(nullptr);
            m_archive->setRatioCallback(nullptr);
            m_archive->setFileCallback(nullptr);
        }
    }

    /**
     * @brief 是否启动进度反馈
     */
    bool HasFeeback()
    {
        if (m_archive)
        {
            return !!m_archive->progressCallback();
        }
        return false;
    }

    /**
     * @brief 打开文件
     * @param in_file 压缩文件
     * @param feedback 是否启用进度反馈
     * @param password 密码
     * @param format 解压格式
     * @return
     */
    bool OpenArchive(const bit7z::tstring &in_file, bool feedback, const bit7z::tstring &password, const bit7z::BitInFormat &format)
    {
        CloseArchive();
        try
        {
            m_archive.reset(new bit7z::BitArchiveReader{piv::Archive::Get7zLib(), in_file, format, password});
            EnableFeeback(feedback);
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

    /**
     * @brief 打开内存输入流
     * @param in_data 压缩包数据
     * @param feedback 是否启用进度反馈
     * @param password 密码
     * @param format 解压格式
     * @return
     */
    bool OpenArchive(std::istream &in_stream, bool feedback, const bit7z::tstring &password, const bit7z::BitInFormat &format)
    {
        CloseArchive();
        try
        {
            m_archive.reset(new bit7z::BitArchiveReader{piv::Archive::Get7zLib(), in_stream, format, password});
            EnableFeeback(feedback);
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

    /**
     * @brief 打开vector
     * @param in_data 压缩包数据
     * @param feedback 是否启用进度反馈
     * @param password 密码
     * @param format 解压格式
     * @return
     */
    bool OpenArchive(std::vector<bit7z::byte_t> &in_buffer, bool feedback, const bit7z::tstring &password, const bit7z::BitInFormat &format)
    {
        CloseArchive();
        try
        {
            m_archive.reset(new bit7z::BitArchiveReader{piv::Archive::Get7zLib(), in_buffer, format, password});
            EnableFeeback(feedback);
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

    /**
     * @brief 打开字节集
     * @param in_data 压缩包数据
     * @param feedback 是否启用进度反馈
     * @param password 密码
     * @param format 解压格式
     * @return
     */
    bool OpenArchive(CVolMem &in_data, bool feedback, const bit7z::tstring &password, const bit7z::BitInFormat &format /*, bool copy_data*/)
    {
        CloseArchive();
        m_buffer.reset(new std::vector<bit7z::byte_t>{reinterpret_cast<bit7z::byte_t *>(in_data.GetPtr()),
                                                      reinterpret_cast<bit7z::byte_t *>(in_data.GetPtr()) + in_data.GetSize()});
        try
        {
            m_archive.reset(new bit7z::BitArchiveReader{piv::Archive::Get7zLib(), *m_buffer, format, password});
            EnableFeeback(feedback);
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

    /**
     * @brief 打开内存
     * @param mem_ptr 压缩包地址
     * @param mem_size 压缩包长度
     * @param feedback 是否启用进度反馈
     * @param password 密码
     * @param format 解压格式
     * @return
     */
    bool OpenArchive(ptrdiff_t mem_ptr, ptrdiff_t mem_size, bool feedback, const bit7z::tstring &password, const bit7z::BitInFormat &format /*, bool copy_data*/)
    {
        CloseArchive();
        m_buffer.reset(new std::vector<bit7z::byte_t>{reinterpret_cast<bit7z::byte_t *>(mem_ptr),
                                                      reinterpret_cast<bit7z::byte_t *>(mem_ptr) + mem_size});
        try
        {
            m_archive.reset(new bit7z::BitArchiveReader{piv::Archive::Get7zLib(), *m_buffer, format, password});
            EnableFeeback(feedback);
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

    /**
     * @brief 打开文件资源
     * @param resId 文件资源ID
     * @param feedback 是否启用进度反馈
     * @param password 密码
     * @param format 解压格式
     * @return
     */
    bool OpenArchive(int32_t resId, bool feedback, const bit7z::tstring &password, const bit7z::BitInFormat &format)
    {
        CloseArchive();
        if (resId == 0)
            return false;
        HMODULE hModule = g_objVolApp.GetInstanceHandle();
        HRSRC hSrc = ::FindResourceW(hModule, MAKEINTRESOURCE(static_cast<WORD>(resId)), RT_RCDATA);
        if (hSrc == NULL)
            return false;
        HGLOBAL resdata = ::LoadResource(hModule, hSrc);
        if (resdata == NULL)
            return false;
        bit7z::byte_t *data = reinterpret_cast<bit7z::byte_t *>(::LockResource(resdata));
        m_buffer.reset(new std::vector<bit7z::byte_t>{data, data + ::SizeofResource(hModule, hSrc)});
        try
        {
            m_archive.reset(new bit7z::BitArchiveReader{piv::Archive::Get7zLib(), *m_buffer, format, password});
            EnableFeeback(feedback);
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

    /**
     * @brief 解压到
     * @param out_dir 输出路径
     * @return
     */
    bool Extract(const bit7z::tstring &out_dir)
    {
        if (!m_archive)
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            m_archive->extractTo(out_dir);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 解压到内存
     * @param out_buffer 解压结果
     * @return
     */
    bool Extract(std::map<CVolString, CVolMem> &out_buffer)
    {
        out_buffer.clear();
        if (!m_archive)
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            std::map<bit7z::tstring, std::vector<bit7z::byte_t>> out_map;
            m_archive->extractTo(out_map);
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

    /**
     * @brief 解压到内存2
     * @param out_buffer 解压结果
     * @return
     */
    bool ExtractEx(std::map<CVolString, CVolMem> &out_buffer)
    {
        out_buffer.clear();
        if (!m_archive)
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            for (auto itr = m_archive->begin(); itr != m_archive->end(); itr++)
            {
                if (!itr->isDir())
                {
                    CVolString path;
                    path.SetText(itr->path().c_str());
                    out_buffer[path] = CVolMem();
                    out_buffer[path].Alloc((INT_P)itr->size(), true);
                    m_archive->extractTo(reinterpret_cast<bit7z::byte_t *>(out_buffer[path].GetPtr()), static_cast<std::size_t>(itr->size()), itr->index());
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

    /**
     * @brief 解压项目到字节集
     * @param idx 索引
     * @param out_buffer 输出数据
     * @param out_path 输出文件名
     * @return
     */
    bool Extract(int32_t idx, CVolMem &out_buffer, CVolString &out_path)
    {
        out_path.Empty();
        if (!m_archive)
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            if (idx < 0 || static_cast<uint32_t>(idx) > m_archive->itemsCount() - 1)
            {
                throw bit7z::BitException("Cannot extract item at the index " + std::to_string(idx), bit7z::make_error_code(bit7z::BitError::InvalidIndex));
            }
            out_buffer.Alloc(static_cast<ptrdiff_t>(m_archive->itemProperty(static_cast<uint32_t>(idx), bit7z::BitProperty::Size).getUInt64()), true);
            m_archive->extractTo(reinterpret_cast<bit7z::byte_t *>(out_buffer.GetPtr()), static_cast<std::size_t>(out_buffer.GetSize()), static_cast<uint32_t>(idx));
            out_path.SetText(m_archive->itemProperty(static_cast<uint32_t>(idx), bit7z::BitProperty::Path).getString().c_str());
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

    /**
     * @brief 解压多个项目到
     * @param idx_array 索引数组
     * @param out_dir 输出路径
     * @return
     */
    bool Extract(CMArray<int32_t> &idx_array, const bit7z::tstring &out_dir)
    {
        if (!m_archive)
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
            uint32_t n_items = m_archive->itemsCount();
            const auto find_res = std::find_if(indices.cbegin(), indices.cend(),
                                               [&n_items](const uint32_t &index) -> bool
                                               { return index >= n_items; });
            if (find_res != indices.cend())
            {
            }
            m_archive->extractTo(out_dir, indices);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 解压匹配文件
     * @param filter 筛选器
     * @param out_dir 输出路径
     * @param isExclude 是否排除匹配项
     * @return
     */
    bool Extract(const bit7z::tstring &filter, const bit7z::tstring &out_dir, bool isExclude)
    {
        if (!m_archive)
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            if (filter.empty())
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::FilterNotSpecified));
            std::vector<uint32_t> matched_indices;
            for (const auto &item : *m_archive)
            {
                bool item_matches = bit7z::filesystem::fsutil::wildcard_match(filter, item.path());
                if (item_matches != isExclude)
                    matched_indices.push_back(item.index());
            }
            if (matched_indices.empty())
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::NoMatchingItems));
            m_archive->extractTo(out_dir, matched_indices);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 解压匹配文件到字节集
     * @param filter 筛选器
     * @param out_buffer 输出数据
     * @param out_path 输出文件名
     * @param isExclude 是否排除匹配项
     * @return
     */
    bool Extract(const bit7z::tstring &filter, CVolMem &out_buffer, CVolString &out_path, bool isExclude)
    {
        out_path.Empty();
        if (!m_archive)
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            if (filter.empty())
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::FilterNotSpecified));
            for (const auto &item : *m_archive)
            {
                bool item_matches = bit7z::filesystem::fsutil::wildcard_match(filter, item.path());
                if (item_matches != isExclude)
                {
                    out_buffer.Alloc(static_cast<ptrdiff_t>(item.size()), true);
                    m_archive->extractTo(reinterpret_cast<bit7z::byte_t *>(out_buffer.GetPtr()), static_cast<std::size_t>(out_buffer.GetSize()), item.index());
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

    /**
     * @brief 解压正则匹配文件
     * @param regex 正则表达式
     * @param out_dir 输出路径
     * @param isExclude 是否排除匹配项
     * @return
     */
    bool ExtractRegex(const bit7z::tstring &regex, const bit7z::tstring &out_dir, bool isExclude)
    {
        if (!m_archive)
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            if (regex.empty())
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::FilterNotSpecified));
            std::vector<uint32_t> matched_indices;
            const bit7z::tregex regex_filter(regex, bit7z::tregex::ECMAScript | bit7z::tregex::optimize);
            for (const auto &item : *m_archive)
            {
                bool item_matches = std::regex_match(item.path(), regex_filter);
                if (item_matches != isExclude)
                    matched_indices.push_back(item.index());
            }
            if (matched_indices.empty())
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::NoMatchingItems));
            m_archive->extractTo(out_dir, matched_indices);
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

    /**
     * @brief 解压正则匹配文件到字节集
     * @param regex 正则表达式
     * @param out_buffer 输出数据
     * @param out_path 输出文件名
     * @param isExclude 是否排除匹配项
     * @return
     */
    bool ExtractRegex(const bit7z::tstring &regex, CVolMem &out_buffer, CVolString &out_path, bool isExclude)
    {
        out_path.Empty();
        if (!m_archive)
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            if (regex.empty())
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::FilterNotSpecified));
            std::vector<uint32_t> matched_indices;
            const bit7z::tregex regex_filter(regex, bit7z::tregex::ECMAScript | bit7z::tregex::optimize);
            for (const auto &item : *m_archive)
            {
                bool item_matches = std::regex_match(item.path(), regex_filter);
                if (item_matches != isExclude)
                {
                    out_buffer.Alloc(static_cast<ptrdiff_t>(item.size()), true);
                    m_archive->extractTo(reinterpret_cast<bit7z::byte_t *>(out_buffer.GetPtr()), static_cast<std::size_t>(out_buffer.GetSize()), item.index());
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

    /**
     * @brief 测试
     * @return
     */
    bool Test()
    {
        if (!m_archive)
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            m_archive->test();
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 测试项目
     * @param index 索引
     * @return
     */
    bool TestItem(int32_t index)
    {
        if (!m_archive)
        {
            SetError("未打开压缩文件.");
            return false;
        }
        try
        {
            m_archive->testItem(static_cast<uint32_t>(index));
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 压缩密码
     * @param password 密码
     */
    inline void SetPassword(const bit7z::tstring &password)
    {
        if (m_archive)
            m_archive->setPassword(password);
    }
    inline CVolString Password()
    {
        return m_archive == nullptr ? _CT("") : CVolString(m_archive->password().c_str());
    }

    /**
     * @brief 清除密码
     */
    inline void ClearPassword()
    {
        if (m_archive)
            m_archive->clearPassword();
    }

    /**
     * @brief 是否有密码
     * @return
     */
    inline bool IsPasswordDefined()
    {
        return m_archive == nullptr ? false : m_archive->isPasswordDefined();
    }

    /**
     * @brief 保留目录结构
     * @param retain 是否保留
     */
    inline void SetRetainDirectories(bool retain)
    {
        if (m_archive)
            m_archive->setRetainDirectories(retain);
    }
    inline bool RetainDirectories()
    {
        return m_archive == nullptr ? false : m_archive->retainDirectories();
    }

    /**
     * @brief 取压缩格式
     * @return
     */
    inline int8_t DetectedFormat()
    {
        return m_archive == nullptr ? 0 : static_cast<int8_t>(m_archive->detectedFormat().value());
    }

    /**
     * @brief 取压缩包属性表
     * @return
     */
    inline std::map<bit7z::BitProperty, bit7z::BitPropVariant> &ArchiveProperties()
    {
        return m_archive == nullptr ? std::map<bit7z::BitProperty, bit7z::BitPropVariant>() : m_archive->archiveProperties();
    }

    /**
     * @brief 取压缩包属性
     * @param property
     * @return
     */
    inline bit7z::BitPropVariant &ArchiveProperty(int32_t property)
    {
        return m_archive == nullptr ? bit7z::BitPropVariant() : m_archive->archiveProperty(static_cast<bit7z::BitProperty>(property));
    }

    /**
     * @brief 取项目信息表
     * @return
     */
    inline std::vector<bit7z::BitArchiveItemInfo> &Items()
    {
        return m_archive == nullptr ? std::vector<bit7z::BitArchiveItemInfo>() : m_archive->items();
    }

    /**
     * @brief 取成员属性
     * @param idx 索引
     * @param property 属性名
     * @return
     */
    inline bit7z::BitPropVariant &ItemProperty(int32_t idx, int32_t property)
    {
        return m_archive == nullptr ? bit7z::BitPropVariant() : m_archive->itemProperty(static_cast<uint32_t>(idx), static_cast<bit7z::BitProperty>(property));
    }

    /**
     * @brief 取项目数
     * @return
     */
    inline int32_t ItemsCount()
    {
        return m_archive == nullptr ? -1 : static_cast<int32_t>(m_archive->itemsCount());
    }

    /**
     * @brief 是否为目录项目
     * @param idx 项目索引
     * @return
     */
    inline bool IsItemFolder(int32_t idx)
    {
        return m_archive == nullptr ? false : m_archive->isItemFolder(static_cast<uint32_t>(idx));
    }

    /**
     * @brief 是否为加密项目
     * @param idx 项目索引
     * @return
     */
    inline bool IsItemEncrypted(int32_t idx)
    {
        return m_archive == nullptr ? false : m_archive->isItemEncrypted(static_cast<uint32_t>(idx));
    }

    /**
     * @brief 取压缩文件路径
     * @return
     */
    inline CVolString ArchivePath()
    {
        return m_archive == nullptr ? _CT("") : CVolString(m_archive->archivePath().c_str());
    }

    /**
     * @brief 取文件夹数量
     * @return
     */
    inline int32_t foldersCount()
    {
        return m_archive == nullptr ? -1 : static_cast<int32_t>(m_archive->foldersCount());
    }

    /**
     * @brief 取文件数量
     * @return
     */
    inline int32_t FilesCount()
    {
        return m_archive == nullptr ? -1 : static_cast<int32_t>(m_archive->filesCount());
    }

    /**
     * @brief 取总长度
     * @return
     */
    inline int64_t Size()
    {
        return m_archive == nullptr ? -1 : static_cast<int64_t>(m_archive->size());
    }

    /**
     * @brief 取打包长度
     * @return
     */
    inline int64_t PackSize()
    {
        return m_archive == nullptr ? -1 : static_cast<int64_t>(m_archive->packSize());
    }

    /**
     * @brief 是否有加密项目
     * @return
     */
    inline bool HasEncryptedItems()
    {
        return m_archive == nullptr ? false : m_archive->hasEncryptedItems();
    }

    /**
     * @brief 是否已加密
     * @return
     */
    inline bool IsEncrypted()
    {
        return m_archive == nullptr ? false : m_archive->isEncrypted();
    }

    /**
     * @brief 取分卷数量
     * @return
     */
    inline int32_t VolumesCount()
    {
        return m_archive == nullptr ? -1 : static_cast<int32_t>(m_archive->volumesCount());
    }

    /**
     * @brief 是否为分卷
     * @return
     */
    inline bool IsMultiVolume()
    {
        return m_archive == nullptr ? false : m_archive->isMultiVolume();
    }

    /**
     * @brief 是否为固实
     * @return
     */
    inline bool IsSolid()
    {
        return m_archive == nullptr ? false : m_archive->isSolid();
    }

    /**
     * @brief 取尾项目索引
     * @return
     */
    inline int32_t EndIndex()
    {
        return m_archive == nullptr ? -1 : static_cast<int32_t>(m_archive->end()->index() - 1);
    }

    /**
     * @brief 寻找项目索引
     * @param item_path 项目路径
     * @return
     */
    inline int32_t FindIndex(const bit7z::tstring &item_path)
    {
        if (!m_archive)
            return -1;
        uint32_t itemIndex = m_archive->find(item_path)->index();
        return (itemIndex != m_archive->end()->index()) ? static_cast<int32_t>(itemIndex) : -1;
    }

    /**
     * @brief 项目是否存在
     * @param item_path 项目路径
     * @return
     */
    inline bool IsContain(const bit7z::tstring &item_path)
    {
        return m_archive == nullptr ? false : m_archive->contains(item_path);
    }
}; // PivArchiveReader

/**
 * @brief 存档压缩类
 */
class PivArchiveWirter
{
protected:
    std::unique_ptr<bit7z::BitArchiveWriter> m_archive{nullptr};
    std::unique_ptr<std::vector<bit7z::byte_t>> m_buffer{nullptr};
    std::vector<std::vector<bit7z::byte_t>> m_buffer_array;
    std::string m_error;
    uint64_t m_total_size = 0;
    int32_t m_ratio = 0;
    std::wstring m_progress_file;

    /**
     * @brief 置最后错误
     * @param Error 错误信息
     */
    inline void SetError(const CHAR *Error)
    {
        m_error = Error;
    }

    void TotalSizeCB(uint64_t total_size)
    {
        m_total_size = total_size;
    }

    void RatioCB(uint64_t input, uint64_t output)
    {
        if (input != 0)
            m_ratio = static_cast<int32_t>(100 * output / input);
    }

    bool ProgressCB(uint64_t progress_size)
    {
        if (progress_size == 0 || m_total_size == 0)
            return true;
        return !this->WriterProgress(m_progress_file.c_str(), static_cast<int32_t>(100 * progress_size / m_total_size),
                                     static_cast<int64_t>(m_total_size), static_cast<int64_t>(progress_size), m_ratio);
    }

    void FileNameCB(bit7z::tstring fileName)
    {
        m_progress_file = fileName;
    }

public:
    PivArchiveWirter() {}
    ~PivArchiveWirter() {}

    inline bit7z::BitArchiveWriter &data()
    {
        return *m_archive;
    }

    inline bit7z::BitArchiveWriter *pdata()
    {
        return m_archive.get();
    }

    /**
     * @brief 关闭
     */
    inline void CloseArchive()
    {
        m_buffer_array.clear();
        m_buffer.reset();
        m_archive.reset();
    }

    /**
     * @brief 是否为空
     * @return
     */
    inline bool IsNull()
    {
        return m_archive == nullptr;
    }

    /**
     * @brief 取最后错误
     * @return
     */
    inline CVolString GetError()
    {
        return CVolString(GetWideText(m_error.c_str(), CVolMem(), NULL));
    }

    /**
     * @brief 压缩进度改变(虚拟方法)
     * @param progressFile 文件路径
     * @param progress 压缩进度
     * @param totalSize 总压缩大小
     * @param progress_size 已压缩大小
     * @param ratio 整体压缩比
     * @return
     */
    virtual int32_t WriterProgress(const wchar_t *progressFile, int32_t progress, int64_t totalSize, int64_t progress_size, int32_t ratio) { return 0; }

    /**
     * @brief 启用进度反馈
     */
    void EnableFeeback(bool enable)
    {
        if (!m_archive)
            return;
        if (enable)
        {
            m_archive->setTotalCallback(std::bind(&PivArchiveWirter::TotalSizeCB, this, std::placeholders::_1));
            m_archive->setProgressCallback(std::bind(&PivArchiveWirter::ProgressCB, this, std::placeholders::_1));
            m_archive->setRatioCallback(std::bind(&PivArchiveWirter::RatioCB, this, std::placeholders::_1, std::placeholders::_2));
            m_archive->setFileCallback(std::bind(&PivArchiveWirter::FileNameCB, this, std::placeholders::_1));
        }
        else
        {
            m_archive->setTotalCallback(nullptr);
            m_archive->setProgressCallback(nullptr);
            m_archive->setRatioCallback(nullptr);
            m_archive->setFileCallback(nullptr);
        }
    }

    /**
     * @brief 是否启用进度反馈
     */
    bool HasFeeback()
    {
        if (m_archive)
        {
            return !!m_archive->progressCallback();
        }
        return false;
    }

    /**
     * @brief 创建压缩包
     * @param feedback 是否启用进度反馈
     * @param format 压缩格式
     * @return
     */
    bool CreateArchive(bool feedback, const bit7z::BitInOutFormat &format)
    {
        CloseArchive();
        try
        {
            m_archive.reset(new bit7z::BitArchiveWriter{piv::Archive::Get7zLib(), format});
            EnableFeeback(feedback);
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

    /**
     * @brief 创建自文件
     * @param in_file 压缩文件
     * @param feedback 是否启用进度反馈
     * @param format 压缩格式
     * @param password 压缩密码
     * @return
     */
    bool CreateArchive(const bit7z::tstring &in_file, bool feedback, const bit7z::BitInOutFormat &format, const bit7z::tstring &password)
    {
        CloseArchive();
        try
        {
            m_archive.reset(new bit7z::BitArchiveWriter{piv::Archive::Get7zLib(), in_file, format, password});
            EnableFeeback(feedback);
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

    /**
     * @brief 创建自vector
     * @param in_buffer 压缩包数据
     * @param feedback 是否启用进度反馈
     * @param format 压缩格式
     * @param password 压缩密码
     * @return
     */
    bool CreateArchive(std::vector<bit7z::byte_t> &in_buffer, bool feedback, const bit7z::BitInOutFormat &format, const bit7z::tstring &password)
    {
        CloseArchive();
        try
        {
            m_archive.reset(new bit7z::BitArchiveWriter{piv::Archive::Get7zLib(), in_buffer, format, password});
            EnableFeeback(feedback);
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

    /**
     * @brief 创建自输入流
     * @param in_stream 压缩包流数据
     * @param feedback 是否启用进度反馈
     * @param format 压缩格式
     * @param password 压缩密码
     * @return
     */
    bool CreateArchive(std::istream &in_stream, bool feedback, const bit7z::BitInOutFormat &format, const bit7z::tstring &password)
    {
        CloseArchive();
        try
        {
            m_archive.reset(new bit7z::BitArchiveWriter{piv::Archive::Get7zLib(), in_stream, format, password});
            EnableFeeback(feedback);
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

    /**
     * @brief 创建自字节集
     * @param in_buffer 压缩包数据
     * @param feedback 是否启用进度反馈
     * @param format 压缩格式
     * @param password 压缩密码
     * @return
     */
    bool CreateArchive(CVolMem &in_buffer, bool feedback, const bit7z::BitInOutFormat &format, const bit7z::tstring &password)
    {
        CloseArchive();
        m_buffer.reset(new std::vector<bit7z::byte_t>{reinterpret_cast<bit7z::byte_t *>(in_buffer.GetPtr()),
                                                      reinterpret_cast<bit7z::byte_t *>(in_buffer.GetPtr()) + in_buffer.GetSize()});
        try
        {
            m_archive.reset(new bit7z::BitArchiveWriter{piv::Archive::Get7zLib(), *m_buffer, format, password});
            EnableFeeback(feedback);
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
    bool CreateArchive(size_t resid, bool feedback, const bit7z::BitInOutFormat &format, const bit7z::tstring &password)
    {
        CloseArchive();
        try
        {
            m_in_stream.OpenStream(resid);
            m_archive = new bit7z::BitArchiveWriter{piv::Archive::Get7zLib(), m_in_stream.is(), format, password};
            EnableFeeback(feedback);
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

    /**
     * @brief 添加项目
     * @param path_array 路径数组
     * @return
     */
    bool AddItems(std::vector<bit7z::tstring> &path_array)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addItems(path_array);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    bool AddItems(CMStringArray &path_array)
    {
        std::vector<bit7z::tstring> in_paths;
        for (INT_P i = 0; i < path_array.GetCount(); i++)
        {
            in_paths.push_back(bit7z::tstring{path_array.GetAt(i)});
        }
        return AddItems(in_paths);
    }

    /**
     * @brief 添加别名项目
     * @param path_map 路径键值对
     * @return
     */
    bool AddItems(std::map<bit7z::tstring, bit7z::tstring> &path_map)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addItems(path_map);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    bool AddItems(std::map<CVolString, CVolString> &path_map)
    {
        std::map<bit7z::tstring, bit7z::tstring> in_paths;
        for (auto iter = path_map.begin(); iter != path_map.end(); iter++)
        {
            in_paths[bit7z::tstring{iter->first.GetText()}] = bit7z::tstring{iter->second.GetText()};
        }
        return AddItems(in_paths);
    }

    /**
     * @brief 添加文件
     * @param in_path 文件路径
     * @param alias 别名
     * @return
     */
    bool AddFile(const bit7z::tstring &in_path, const bit7z::tstring &alias)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addFile(in_path, alias);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /* 添加内存流
    bool AddFile(std::istream &in_stream, const bit7z::tstring &path)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addFile(in_stream, path);
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

    /**
     * @brief 添加字节集
     * @param in_buffer 缓存数据
     * @param path 数据名称
     * @return
     */
    bool AddFile(CVolMem &in_buffer, const bit7z::tstring &path /* , bool copy_data*/)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_buffer_array.push_back(bit7z::buffer_t{reinterpret_cast<const bit7z::byte_t *>(in_buffer.GetPtr()), reinterpret_cast<const bit7z::byte_t *>(in_buffer.GetEndPtr())});
            m_archive->addFile(m_buffer_array.back(), path);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 添加内存
     * @param in_buffer 数据地址
     * @param buffer_len 数据长度
     * @param path 数据名称
     * @return
     */
    bool AddFile(const ptrdiff_t &in_buffer, const ptrdiff_t &buffer_len, const bit7z::tstring &path /*, bool copy_data*/)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_buffer_array.push_back(bit7z::buffer_t{reinterpret_cast<const bit7z::byte_t *>(in_buffer), reinterpret_cast<const bit7z::byte_t *>(in_buffer) + buffer_len});
            m_archive->addFile(m_buffer_array.back(), path);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /* 添加文件资源内存
    bool AddFile(size_t resid, const ptrdiff_t &buffer_len, const bit7z::tstring &path)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            PivMemoryInStream<char> in_stream;
            in_stream.OpenStream(resid);
            m_archive->addFile(in_stream.is(), path);
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

    /**
     * @brief 添加文件数组
     * @param in_files 文件数组
     * @return
     */
    bool AddFiles(std::vector<bit7z::tstring> &in_files)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addFiles(in_files);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    bool AddFiles(CMStringArray &in_files)
    {
        std::vector<bit7z::tstring> in_paths;
        for (INT_P i = 0; i < in_files.GetCount(); i++)
        {
            in_paths.push_back(bit7z::tstring{in_files.GetAt(i)});
        }
        return AddFiles(in_paths);
    }

    /**
     * @brief 添加匹配文件
     * @param in_dir 输入目录
     * @param filter 筛选器
     * @param isExclude 是否排除匹配项
     * @param recursion 递归子目录
     * @return
     */
    bool AddFiles(const bit7z::tstring &in_dir, const bit7z::tstring &filter, bool isExclude, bool recursion)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addFiles(in_dir, filter, isExclude ? bit7z::FilterPolicy::Exclude : bit7z::FilterPolicy::Include, recursion);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 添加目录
     * @param in_dir 目录路径
     * @return
     */
    bool AddDirectory(const bit7z::tstring &in_dir)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addDirectory(in_dir);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 压缩到文件
     * @param out_file 压缩包路径
     * @return
     */
    bool CompressTo(const bit7z::tstring &out_file)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->compressTo(out_file);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 压缩到字节集
     * @param out_buffer 输出字节集
     * @return
     */
    bool CompressTo(CVolMem &out_buffer)
    {
        out_buffer.Empty();
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            std::vector<bit7z::byte_t> buffer;
            m_archive->compressTo(buffer);
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

    /**
     * @brief 取压缩格式
     * @return
     */
    inline int8_t CompressionFormat()
    {
        return m_archive == nullptr ? 0 : static_cast<int8_t>(m_archive->compressionFormat().value());
    }

    /**
     * @brief 取项目数量
     * @return
     */
    inline int32_t ItemsCount()
    {
        return m_archive == nullptr ? 0 : static_cast<int32_t>(m_archive->itemsCount());
    }

    /**
     * @brief (设置/获取)压缩密码
     * @param password 新密码
     */
    inline void SetPassword(const bit7z::tstring &password)
    {
        if (m_archive)
            m_archive->setPassword(password);
    }
    inline CVolString Password()
    {
        return m_archive == nullptr ? _CT("") : CVolString(m_archive->password().c_str());
    }

    /**
     * @brief (设置/获取)压缩等级
     * @param level 新等级
     */
    inline void SetCompressionLevel(int32_t level)
    {
        if (m_archive)
            m_archive->setCompressionLevel(static_cast<bit7z::BitCompressionLevel>(level));
    }
    inline int32_t CompressionLevel()
    {
        return m_archive == nullptr ? 0 : static_cast<int32_t>(m_archive->compressionLevel());
    }

    /**
     * @brief (设置/获取)压缩方法
     * @param method 新方法
     */
    inline void SetCompressionMethod(bit7z::BitCompressionMethod method)
    {
        if (m_archive)
            m_archive->setCompressionMethod(method);
    }
    inline int32_t CompressionMethod()
    {
        return m_archive == nullptr ? 0 : static_cast<int32_t>(m_archive->compressionMethod());
    }

    /**
     * @brief (设置/获取)字典大小
     * @param dictionary_size 新字典大小
     */
    inline void SetDictionarySize(int32_t dictionary_size)
    {
        if (m_archive)
            m_archive->setDictionarySize(static_cast<uint32_t>(dictionary_size));
    }
    inline int32_t DictionarySize()
    {
        return m_archive == nullptr ? 0 : static_cast<int32_t>(m_archive->dictionarySize());
    }

    /**
     * @brief (设置/获取)单词大小
     * @param word_size 新单词大小
     */
    inline void SetWordSize(int32_t word_size)
    {
        if (m_archive)
            m_archive->setWordSize(static_cast<uint32_t>(word_size));
    }
    inline int32_t WordSize()
    {
        return m_archive == nullptr ? 0 : static_cast<int32_t>(m_archive->wordSize());
    }

    /**
     * @brief (设置/获取)固实压缩
     * @param solid_mode 是否启用
     */
    inline void SetSolidMode(bool solid_mode)
    {
        if (m_archive)
            m_archive->setSolidMode(solid_mode);
    }
    inline bool SolidMode()
    {
        return m_archive == nullptr ? false : m_archive->solidMode();
    }

    /**
     * @brief (设置/获取)更新方式
     * @param mode 新更新方式
     */
    inline void SetUpdateMode(int32_t mode)
    {
        if (m_archive)
            m_archive->setUpdateMode(static_cast<bit7z::UpdateMode>(mode));
    }
    inline int32_t UpdateMode()
    {
        return m_archive == nullptr ? 0 : static_cast<int32_t>(m_archive->updateMode());
    }

    /**
     * @brief (设置/获取)分卷大小
     * @param volume_size 新分卷大小
     */
    inline void SetVolumeSize(int32_t volume_size)
    {
        if (m_archive)
            m_archive->setVolumeSize(static_cast<uint32_t>(volume_size));
    }
    inline int32_t VolumeSize()
    {
        return m_archive == nullptr ? 0 : static_cast<int32_t>(m_archive->volumeSize());
    }

    /**
     * @brief (设置/获取)线程数
     * @param threads_count 新线程数
     */
    inline void SetThreadsCount(int32_t threads_count)
    {
        if (m_archive)
            m_archive->setThreadsCount(static_cast<uint32_t>(threads_count));
    }
    inline int32_t ThreadsCount()
    {
        return m_archive == nullptr ? 0 : static_cast<int32_t>(m_archive->threadsCount());
    }

    /**
     * @brief (设置/获取)存储符号链接
     * @param store_symlinks 是否存储符号链接
     */
    inline void SetStoreSymbolicLinks(bool store_symlinks)
    {
        if (m_archive)
            m_archive->setStoreSymbolicLinks(store_symlinks);
    }
    inline bool StoreSymbolicLinks()
    {
        return m_archive == nullptr ? false : m_archive->storeSymbolicLinks();
    }

    /**
     * @brief (设置/获取)保留目录结构
     * @param retain 是否保留目录结构
     */
    inline void SetRetainDirectories(bool retain)
    {
        if (m_archive)
            m_archive->setRetainDirectories(retain);
    }
    inline bool RetainDirectories()
    {
        return m_archive == nullptr ? false : m_archive->retainDirectories();
    }

    /**
     * @brief 设置密码
     * @param password 新密码
     * @param crypt_headers 加密文件头
     */
    inline void SetPassword(const bit7z::tstring &password, bool crypt_headers)
    {
        if (m_archive)
            m_archive->setPassword(password, crypt_headers);
    }

    /**
     * @brief 清除密码
     */
    inline void ClearPassword()
    {
        if (m_archive)
            m_archive->clearPassword();
    }

    /**
     * @brief 是否有密码
     * @return
     */
    inline bool IsPasswordDefined()
    {
        return m_archive == nullptr ? false : m_archive->isPasswordDefined();
    }

    /**
     * @brief 是否加密文件头
     * @return
     */
    inline bool CryptHeaders()
    {
        return m_archive == nullptr ? false : m_archive->cryptHeaders();
    }
}; // PivArchiveWirter

/**
 * @brief 存档编辑类
 */
class PivArchiveEditor
{
protected:
    std::unique_ptr<bit7z::BitArchiveEditor> m_archive{nullptr};
    std::unique_ptr<std::vector<bit7z::byte_t>> m_buffer{nullptr};
    std::vector<std::vector<bit7z::byte_t>> m_buffer_array;
    std::string m_error;
    uint64_t m_total_size = 0;
    int32_t m_ratio = 0;
    std::wstring m_progress_file;

    /**
     * @brief 置最后错误
     * @param Error 错误信息
     */
    inline void SetError(const CHAR *Error)
    {
        m_error = Error;
    }

    void RatioCB(uint64_t input, uint64_t output)
    {
        if (input != 0)
            m_ratio = static_cast<int32_t>(100 * output / input);
    }

    bool ProgressCB(uint64_t progress_size)
    {
        if (progress_size == 0 || m_total_size == 0)
            return true;
        return !this->WriterProgress(m_progress_file.c_str(), static_cast<int32_t>(100 * progress_size / m_total_size),
                                     static_cast<int64_t>(m_total_size), static_cast<int64_t>(progress_size), m_ratio);
    }

    void TotalSizeCB(uint64_t total_size)
    {
        m_total_size = total_size;
    }

    void FileNameCB(bit7z::tstring fileName)
    {
        m_progress_file = fileName;
    }

public:
    PivArchiveEditor() {}
    ~PivArchiveEditor() {}

    inline bit7z::BitArchiveEditor &data()
    {
        return *m_archive;
    }

    inline bit7z::BitArchiveEditor *pdata()
    {
        return m_archive.get();
    }

    /**
     * @brief 关闭
     */
    inline void CloseArchive()
    {
        m_buffer_array.clear();
        m_archive.reset();
    }

    /**
     * @brief 打开文件
     * @param in_file 压缩文件
     * @param feedback 是否启用进度反馈
     * @param format 压缩格式
     * @param update_mode 更新方式
     * @param password 压缩密码
     * @return
     */
    bool OpenFile(const bit7z::tstring &in_file, bool feedback, const bit7z::BitInOutFormat &format, int32_t update_mode, const bit7z::tstring &password)
    {
        CloseArchive();
        try
        {
            m_archive.reset(new bit7z::BitArchiveEditor{piv::Archive::Get7zLib(), in_file, format, password});
            m_archive->setUpdateMode(static_cast<bit7z::UpdateMode>(update_mode));
            EnableFeeback(feedback);
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

    /**
     * @brief 应用更改
     * @return
     */
    bool ApplyChanges()
    {
        if (!m_archive)
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            m_archive->applyChanges();
            m_buffer_array.clear();
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 索引项目更名
     * @param index 项目索引
     * @param new_path 新路径名称
     * @return
     */
    bool RenameItem(int32_t index, const bit7z::tstring &new_path)
    {
        if (!m_archive)
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            m_archive->renameItem(static_cast<uint32_t>(index), new_path);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 路径项目更名
     * @param old_path 旧路径名称
     * @param new_path 新路径名称
     * @return
     */
    bool RenameItem(const bit7z::tstring &old_path, const bit7z::tstring &new_path)
    {
        if (!m_archive)
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            m_archive->renameItem(old_path, new_path);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 更新索引项目自文件
     * @param index 项目索引
     * @param in_file 文件路径
     * @return
     */
    bool UpdateItem(int32_t index, const bit7z::tstring &in_file)
    {
        if (!m_archive)
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            m_archive->updateItem(static_cast<uint32_t>(index), in_file);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 更新索引项目自字节集
     * @param index 项目索引
     * @param in_buffer 数据内容
     * @return
     */
    bool UpdateItem(int32_t index, CVolMem &in_buffer)
    {
        if (!m_archive)
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            m_buffer_array.push_back(bit7z::buffer_t{reinterpret_cast<const bit7z::byte_t *>(in_buffer.GetPtr()), reinterpret_cast<const bit7z::byte_t *>(in_buffer.GetEndPtr())});
            m_archive->updateItem(static_cast<uint32_t>(index), m_buffer_array.back());
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /* 更新索引项目自内存流
    bool UpdateItem(int32_t index, std::istream &in_stream)
    {
        if (!m_archive)
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            m_archive->updateItem(static_cast<uint32_t>(index), in_stream);
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

    /**
     * @brief 更新路径项目自文件
     * @param item_path 项目路径
     * @param in_file 文件路径
     * @return
     */
    bool UpdateItem(const bit7z::tstring &item_path, const bit7z::tstring &in_file)
    {
        if (!m_archive)
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            m_archive->updateItem(item_path, in_file);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 更新路径项目自字节集
     * @param item_path 项目路径
     * @param in_buffer 数据内容
     * @return
     */
    bool UpdateItem(const bit7z::tstring &item_path, CVolMem &in_buffer /* , bool copy_data*/)
    {
        if (!m_archive)
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            m_buffer_array.push_back(bit7z::buffer_t{reinterpret_cast<const bit7z::byte_t *>(in_buffer.GetPtr()), reinterpret_cast<const bit7z::byte_t *>(in_buffer.GetEndPtr())});
            m_archive->updateItem(item_path, m_buffer_array.back());
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 更新路径项目自内存流
     * @param item_path 项目路径
     * @param in_stream 内存流数据
     * @return
     */
    bool UpdateItem(const bit7z::tstring &item_path, std::istream &in_stream)
    {
        if (!m_archive)
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            m_archive->updateItem(item_path, in_stream);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 删除索引项目
     * @param index 项目索引
     * @return
     */
    bool DeleteItem(int32_t index)
    {
        if (!m_archive)
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            m_archive->deleteItem(static_cast<uint32_t>(index));
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 删除路径项目
     * @param item_path 项目路径
     * @return
     */
    bool DeleteItem(const bit7z::tstring &item_path)
    {
        if (!m_archive)
        {
            SetError("未打开压缩文件!");
            return false;
        }
        try
        {
            m_archive->deleteItem(item_path);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 是否为空
     * @return
     */
    inline bool IsNull()
    {
        return m_archive == nullptr;
    }

    /**
     * @brief 取最后错误
     * @return
     */
    inline CVolString GetError()
    {
        return CVolString(GetWideText(m_error.c_str(), CVolMem(), NULL));
    }

    /**
     * @brief 压缩进度改变(虚拟方法)
     * @param progressFile 文件路径
     * @param progress 压缩进度
     * @param totalSize 总压缩大小
     * @param progress_size 已压缩大小
     * @param ratio 整体压缩比
     * @return
     */
    virtual int32_t WriterProgress(const wchar_t *progressFile, int32_t progress, int64_t totalSize, int64_t progress_size, int32_t ratio) { return 0; }

    /**
     * @brief 启用进度反馈
     */
    void EnableFeeback(bool enable)
    {
        if (!m_archive)
            return;
        if (enable)
        {
            m_archive->setTotalCallback(std::bind(&PivArchiveEditor::TotalSizeCB, this, std::placeholders::_1));
            m_archive->setProgressCallback(std::bind(&PivArchiveEditor::ProgressCB, this, std::placeholders::_1));
            m_archive->setRatioCallback(std::bind(&PivArchiveEditor::RatioCB, this, std::placeholders::_1, std::placeholders::_2));
            m_archive->setFileCallback(std::bind(&PivArchiveEditor::FileNameCB, this, std::placeholders::_1));
        }
        else
        {
            m_archive->setTotalCallback(nullptr);
            m_archive->setProgressCallback(nullptr);
            m_archive->setRatioCallback(nullptr);
            m_archive->setFileCallback(nullptr);
        }
    }

    /**
     * @brief 是否启动进度反馈
     */
    bool HasFeeback()
    {
        if (m_archive)
        {
            return !!m_archive->progressCallback();
        }
        return false;
    }
    /**
     * @brief 添加项目
     * @param path_array 路径数组
     * @return
     */
    bool AddItems(std::vector<bit7z::tstring> &path_array)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addItems(path_array);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    bool AddItems(CMStringArray &path_array)
    {
        std::vector<bit7z::tstring> in_paths;
        for (INT_P i = 0; i < path_array.GetCount(); i++)
        {
            in_paths.push_back(bit7z::tstring{path_array.GetAt(i)});
        }
        return AddItems(in_paths);
    }

    /**
     * @brief 添加别名项目
     * @param path_map 路径键值对
     * @return
     */
    bool AddItems(std::map<bit7z::tstring, bit7z::tstring> &path_map)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addItems(path_map);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    bool AddItems(std::map<CVolString, CVolString> &path_map)
    {
        std::map<bit7z::tstring, bit7z::tstring> in_paths;
        for (auto iter = path_map.begin(); iter != path_map.end(); iter++)
        {
            in_paths[bit7z::tstring{iter->first.GetText()}] = bit7z::tstring{iter->second.GetText()};
        }
        return AddItems(in_paths);
    }

    /**
     * @brief 添加文件
     * @param in_path 文件路径
     * @param alias 别名
     * @return
     */
    bool AddFile(const bit7z::tstring &in_path, const bit7z::tstring &alias)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addFile(in_path, alias);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /* 添加内存流
    bool AddFile(std::istream &in_stream, const bit7z::tstring &path)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addFile(in_stream, path);
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

    /**
     * @brief 添加字节集
     * @param in_buffer 缓存数据
     * @param path 数据名称
     * @return
     */
    bool AddFile(CVolMem &in_buffer, const bit7z::tstring &path /* , bool copy_data*/)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_buffer_array.push_back(bit7z::buffer_t{reinterpret_cast<const bit7z::byte_t *>(in_buffer.GetPtr()), reinterpret_cast<const bit7z::byte_t *>(in_buffer.GetEndPtr())});
            m_archive->addFile(m_buffer_array.back(), path);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 添加内存
     * @param in_buffer 数据地址
     * @param buffer_len 数据长度
     * @param path 数据名称
     * @return
     */
    bool AddFile(const ptrdiff_t &in_buffer, const ptrdiff_t &buffer_len, const bit7z::tstring &path /*, bool copy_data*/)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_buffer_array.push_back(bit7z::buffer_t{reinterpret_cast<const bit7z::byte_t *>(in_buffer), reinterpret_cast<const bit7z::byte_t *>(in_buffer) + buffer_len});
            m_archive->addFile(m_buffer_array.back(), path);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /* 添加文件资源内存
    bool AddFile(size_t resid, const ptrdiff_t &buffer_len, const bit7z::tstring &path)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            PivMemoryInStream<char> in_stream;
            in_stream.OpenStream(resid);
            m_archive->addFile(in_stream.is(), path);
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

    /**
     * @brief 添加文件数组
     * @param in_files 文件数组
     * @return
     */
    bool AddFiles(std::vector<bit7z::tstring> &in_files)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addFiles(in_files);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    bool AddFiles(CMStringArray &in_files)
    {
        std::vector<bit7z::tstring> in_paths;
        for (INT_P i = 0; i < in_files.GetCount(); i++)
        {
            in_paths.push_back(bit7z::tstring{in_files.GetAt(i)});
        }
        return AddFiles(in_paths);
    }

    /**
     * @brief 添加匹配文件
     * @param in_dir 输入目录
     * @param filter 筛选器
     * @param isExclude 是否排除匹配项
     * @param recursion 递归子目录
     * @return
     */
    bool AddFiles(const bit7z::tstring &in_dir, const bit7z::tstring &filter, bool isExclude, bool recursion)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addFiles(in_dir, filter, isExclude ? bit7z::FilterPolicy::Exclude : bit7z::FilterPolicy::Include, recursion);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 添加目录
     * @param in_dir 目录路径
     * @return
     */
    bool AddDirectory(const bit7z::tstring &in_dir)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addDirectory(in_dir);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 压缩到文件
     * @param out_file 压缩包路径
     * @return
     */
    bool CompressTo(const bit7z::tstring &out_file)
    {
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->compressTo(out_file);
            SetError("OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 压缩到字节集
     * @param out_buffer 输出字节集
     * @return
     */
    bool CompressTo(CVolMem &out_buffer)
    {
        out_buffer.Empty();
        if (!m_archive)
        {
            SetError("未打开或创建压缩包!");
            return false;
        }
        try
        {
            std::vector<bit7z::byte_t> buffer;
            m_archive->compressTo(buffer);
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

    /**
     * @brief 取压缩格式
     * @return
     */
    inline int8_t CompressionFormat()
    {
        return m_archive == nullptr ? 0 : static_cast<int8_t>(m_archive->compressionFormat().value());
    }

    /**
     * @brief 取项目数量
     * @return
     */
    inline int32_t ItemsCount()
    {
        return m_archive == nullptr ? 0 : static_cast<int32_t>(m_archive->itemsCount());
    }

    /**
     * @brief (设置/获取)压缩密码
     * @param password 新密码
     */
    inline void SetPassword(const bit7z::tstring &password)
    {
        if (m_archive)
            m_archive->setPassword(password);
    }
    inline CVolString Password()
    {
        return m_archive == nullptr ? _CT("") : CVolString(m_archive->password().c_str());
    }

    /**
     * @brief (设置/获取)压缩等级
     * @param level 新等级
     */
    inline void SetCompressionLevel(int32_t level)
    {
        if (m_archive)
            m_archive->setCompressionLevel(static_cast<bit7z::BitCompressionLevel>(level));
    }
    inline int32_t CompressionLevel()
    {
        return m_archive == nullptr ? 0 : static_cast<int32_t>(m_archive->compressionLevel());
    }

    /**
     * @brief (设置/获取)压缩方法
     * @param method 新方法
     */
    inline void SetCompressionMethod(bit7z::BitCompressionMethod method)
    {
        if (m_archive)
            m_archive->setCompressionMethod(method);
    }
    inline int32_t CompressionMethod()
    {
        return m_archive == nullptr ? 0 : static_cast<int32_t>(m_archive->compressionMethod());
    }

    /**
     * @brief (设置/获取)字典大小
     * @param dictionary_size 新字典大小
     */
    inline void SetDictionarySize(int32_t dictionary_size)
    {
        if (m_archive)
            m_archive->setDictionarySize(static_cast<uint32_t>(dictionary_size));
    }
    inline int32_t DictionarySize()
    {
        return m_archive == nullptr ? 0 : static_cast<int32_t>(m_archive->dictionarySize());
    }

    /**
     * @brief (设置/获取)单词大小
     * @param word_size 新单词大小
     */
    inline void SetWordSize(int32_t word_size)
    {
        if (m_archive)
            m_archive->setWordSize(static_cast<uint32_t>(word_size));
    }
    inline int32_t WordSize()
    {
        return m_archive == nullptr ? 0 : static_cast<int32_t>(m_archive->wordSize());
    }

    /**
     * @brief (设置/获取)固实压缩
     * @param solid_mode 是否启用
     */
    inline void SetSolidMode(bool solid_mode)
    {
        if (m_archive)
            m_archive->setSolidMode(solid_mode);
    }
    inline bool SolidMode()
    {
        return m_archive == nullptr ? false : m_archive->solidMode();
    }

    /**
     * @brief (设置/获取)更新方式
     * @param mode 新更新方式
     */
    inline void SetUpdateMode(int32_t mode)
    {
        if (m_archive)
            m_archive->setUpdateMode(static_cast<bit7z::UpdateMode>(mode));
    }
    inline int32_t UpdateMode()
    {
        return m_archive == nullptr ? 0 : static_cast<int32_t>(m_archive->updateMode());
    }

    /**
     * @brief (设置/获取)分卷大小
     * @param volume_size 新分卷大小
     */
    inline void SetVolumeSize(int32_t volume_size)
    {
        if (m_archive)
            m_archive->setVolumeSize(static_cast<uint32_t>(volume_size));
    }
    inline int32_t VolumeSize()
    {
        return m_archive == nullptr ? 0 : static_cast<int32_t>(m_archive->volumeSize());
    }

    /**
     * @brief (设置/获取)线程数
     * @param threads_count 新线程数
     */
    inline void SetThreadsCount(int32_t threads_count)
    {
        if (m_archive)
            m_archive->setThreadsCount(static_cast<uint32_t>(threads_count));
    }
    inline int32_t ThreadsCount()
    {
        return m_archive == nullptr ? 0 : static_cast<int32_t>(m_archive->threadsCount());
    }

    /**
     * @brief (设置/获取)存储符号链接
     * @param store_symlinks 是否存储符号链接
     */
    inline void SetStoreSymbolicLinks(bool store_symlinks)
    {
        if (m_archive)
            m_archive->setStoreSymbolicLinks(store_symlinks);
    }
    inline bool StoreSymbolicLinks()
    {
        return m_archive == nullptr ? false : m_archive->storeSymbolicLinks();
    }

    /**
     * @brief (设置/获取)保留目录结构
     * @param retain 是否保留目录结构
     */
    inline void SetRetainDirectories(bool retain)
    {
        if (m_archive)
            m_archive->setRetainDirectories(retain);
    }
    inline bool RetainDirectories()
    {
        return m_archive == nullptr ? false : m_archive->retainDirectories();
    }

    /**
     * @brief 设置密码
     * @param password 新密码
     * @param crypt_headers 加密文件头
     */
    inline void SetPassword(const bit7z::tstring &password, bool crypt_headers)
    {
        if (m_archive)
            m_archive->setPassword(password, crypt_headers);
    }

    /**
     * @brief 清除密码
     */
    inline void ClearPassword()
    {
        if (m_archive)
            m_archive->clearPassword();
    }

    /**
     * @brief 是否有密码
     * @return
     */
    inline bool IsPasswordDefined()
    {
        return m_archive == nullptr ? false : m_archive->isPasswordDefined();
    }

    /**
     * @brief 是否加密文件头
     * @return
     */
    inline bool CryptHeaders()
    {
        return m_archive == nullptr ? false : m_archive->cryptHeaders();
    }
}; // PivArchiveEditor

#endif // _PIV_BIT7Z_HPP
