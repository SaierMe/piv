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

#include "bit7z.hpp"

#include <mutex>
#include <memory>
#include <atomic>

namespace piv
{
    namespace Archive
    {
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
            case 0x0E:
                return bit7z::BitFormat::Zstd;
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

/**
 * @brief 加载与释放7z.dll动态库(单例对象)
 */
class Piv7zLib
{
private:
    std::atomic<bit7z::Bit7zLibrary *> m_7zLib = nullptr;
    std::mutex m_mutex;
    std::wstring m_error{L"OK"};

    Piv7zLib() {}
    ~Piv7zLib()
    {
        this->Free();
    }
    Piv7zLib(const Piv7zLib &) = delete;
    Piv7zLib(Piv7zLib &&) = delete;
    Piv7zLib &operator=(const Piv7zLib &) = delete;
    Piv7zLib &operator=(Piv7zLib &&) = delete;

public:
    static Piv7zLib &data()
    {
        static Piv7zLib inst;
        return inst;
    }

    /**
     * @brief 获取7z动态库实例
     */
    bit7z::Bit7zLibrary &Get()
    {
        bit7z::Bit7zLibrary *p = m_7zLib.load(std::memory_order_acquire);
        if (p == nullptr)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            p = m_7zLib.load(std::memory_order_acquire);
            if (p == nullptr)
            {
                p = new bit7z::Bit7zLibrary{bit7z::kDefaultLibrary};
                m_7zLib.store(p, std::memory_order_release);
                p->setLargePageMode();
            }
        }
        return *p;
    }

    /**
     * @brief 加载静态库
     * @param library_path 7z.dll的路径
     * @return 是否加载成功
     */
    bool Load(const bit7z::tstring &library_path = bit7z::kDefaultLibrary)
    {
        try
        {
            bit7z::Bit7zLibrary *p = m_7zLib.load(std::memory_order_acquire);
            if (p == nullptr)
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                p = m_7zLib.load(std::memory_order_acquire);
                if (p == nullptr)
                {
                    p = new bit7z::Bit7zLibrary{library_path};
                    m_7zLib.store(p, std::memory_order_release);
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
    void Free()
    {
        bit7z::Bit7zLibrary *p = m_7zLib.load(std::memory_order_acquire);
        if (p != nullptr)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            p = m_7zLib.load(std::memory_order_acquire);
            if (p != nullptr)
            {
                delete p;
                m_7zLib.store(nullptr, std::memory_order_release);
            }
        }
    }

    /**
     * @brief 取最后错误
     * @return
     */
    inline CWString GetLastError()
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        CVolMem buf;
        return CWString{m_error.c_str()};
    }

    /**
     * @brief 置错误信息
     * @param msg
     */
    inline void SetError(const char *msg)
    {
        CVolMem buf;
        std::lock_guard<std::mutex> guard(m_mutex);
        m_error = GetWideText(msg, buf, NULL);
    }

    inline void SetError(const wchar_t *msg)
    {
        std::lock_guard<std::mutex> guard(m_mutex);
        m_error = msg;
    }

}; // Piv7zLib

/**
 * @brief 存档读写类
 */
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
    inline static CWString GetLastError()
    {
        return Piv7zLib::data().GetLastError();
    }

    /**
     * @brief 置错误信息
     * @param msg
     */
    static void SetError(const char *msg)
    {
        return Piv7zLib::data().SetError(msg);
    }

    static void SetError(const wchar_t *msg)
    {
        return Piv7zLib::data().SetError(msg);
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
    static bool CompressPaths(const CMStringArray &paths, const bit7z::tstring &out_file, const bit7z::tstring &password, const bit7z::BitInOutFormat &format, bit7z::BitCompressionLevel level, bit7z::UpdateMode update_mode)
    {
        if (paths.IsEmpty())
        {
            SetError(L"未提供所欲压缩的文件或目录.");
            return false;
        }
        try
        {
            std::vector<bit7z::tstring> in_paths;
            for (INT_P i = 0; i < paths.GetCount(); i++)
            {
                in_paths.emplace_back(paths.GetAt(i));
            }
            bit7z::BitFileCompressor Compressor{Piv7zLib::data().Get(), format};
            Compressor.setPassword(password);
            Compressor.setCompressionLevel(level);
            Compressor.setUpdateMode(update_mode);
            Compressor.compress(in_paths, out_file);
            SetError(L"OK");
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
    static bool CompressPaths(const std::map<bit7z::tstring, bit7z::tstring> &in_paths, const bit7z::tstring &out_file, const bit7z::tstring &password, const bit7z::BitInOutFormat &format,
                              bit7z::BitCompressionLevel level, bit7z::UpdateMode update_mode)
    {
        if (in_paths.empty())
        {
            SetError(L"未提供所欲压缩的文件或目录.");
            return false;
        }
        try
        {
            bit7z::BitFileCompressor Compressor{Piv7zLib::data().Get(), format};
            Compressor.setPassword(password);
            Compressor.setCompressionLevel(level);
            Compressor.setUpdateMode(update_mode);
            Compressor.compress(in_paths, out_file);
            SetError(L"OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    static bool CompressPaths(const std::map<CWString, CWString> &path_pair, const bit7z::tstring &out_file, const bit7z::tstring &password, const bit7z::BitInOutFormat &format,
                              bit7z::BitCompressionLevel level, bit7z::UpdateMode update_mode)
    {
        if (path_pair.empty())
        {
            SetError(L"未提供所欲压缩的文件或目录.");
            return false;
        }
        std::map<bit7z::tstring, bit7z::tstring> in_paths;
        for (auto &pair : path_pair)
        {
            in_paths.emplace(std::piecewise_construct, std::forward_as_tuple(pair.first.GetText()), std::forward_as_tuple(pair.second.GetText()));
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
    static bool CompressFiles(const std::vector<bit7z::tstring> &in_paths, const bit7z::tstring &out_file, const bit7z::tstring &password, const bit7z::BitInOutFormat &format,
                              bit7z::BitCompressionLevel level, bit7z::UpdateMode update_mode)
    {
        if (in_paths.empty())
        {
            SetError(L"未提供所欲压缩的文件.");
            return false;
        }
        try
        {
            bit7z::BitFileCompressor Compressor{Piv7zLib::data().Get(), format};
            Compressor.setPassword(password);
            Compressor.setCompressionLevel(level);
            Compressor.setUpdateMode(update_mode);
            Compressor.compress(in_paths, out_file);
            SetError(L"OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    static bool CompressFiles(const CMStringArray &paths, const bit7z::tstring &out_file, const bit7z::tstring &password, const bit7z::BitInOutFormat &format,
                              bit7z::BitCompressionLevel level, bit7z::UpdateMode update_mode)
    {
        if (paths.IsEmpty())
        {
            SetError(L"未提供所欲压缩的文件.");
            return false;
        }
        std::vector<bit7z::tstring> in_paths;
        for (INT_P i = 0; i < paths.GetCount(); i++)
        {
            in_paths.emplace_back(paths.GetAt(i));
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
    static bool CompressFiles(const bit7z::tstring &in_dir, const bit7z::tstring &out_file, const bit7z::tstring &filter, bool recursive, const bit7z::tstring &password,
                              const bit7z::BitInOutFormat &format, bit7z::BitCompressionLevel level, bit7z::UpdateMode update_mode)
    {
        try
        {
            bit7z::BitFileCompressor Compressor{Piv7zLib::data().Get(), format};
            Compressor.setPassword(password);
            Compressor.setCompressionLevel(level);
            Compressor.setUpdateMode(update_mode);
            Compressor.compressFiles(in_dir, out_file, recursive, filter);
            SetError(L"OK");
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
    static bool CompressDirectory(const bit7z::tstring &in_dir, const bit7z::tstring &out_file, const bit7z::tstring &password, const bit7z::BitInOutFormat &format,
                                  bit7z::BitCompressionLevel level, bit7z::UpdateMode update_mode)
    {
        try
        {
            bit7z::BitFileCompressor Compressor{Piv7zLib::data().Get(), format};
            Compressor.setPassword(password);
            Compressor.setCompressionLevel(level);
            Compressor.setUpdateMode(update_mode);
            Compressor.compressDirectory(in_dir, out_file);
            SetError(L"OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 添加匹配目录到压缩包
     * @param in_dir 输入目录
     * @param out_file 输出文件名
     * @param password 压缩密码
     * @param format 压缩格式
     * @param level 压缩等级
     * @param update_mode 更新方式
     * @return
     */
    static bool CompressDirectoryContents(const bit7z::tstring &in_dir, const bit7z::tstring &out_file, const bit7z::tstring &filter, bool recursive, const bit7z::tstring &password,
        const bit7z::BitInOutFormat &format, bit7z::BitCompressionLevel level, bit7z::UpdateMode update_mode)
    {
        try
        {
            bit7z::BitFileCompressor Compressor{Piv7zLib::data().Get(), format};
            Compressor.setPassword(password);
            Compressor.setCompressionLevel(level);
            Compressor.setUpdateMode(update_mode);
            Compressor.compressDirectoryContents(in_dir, out_file, recursive, filter);
            SetError(L"OK");
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
            bit7z::BitFileExtractor extractor{Piv7zLib::data().Get(), format};
            extractor.setPassword(password);
            extractor.extract(in_file, out_dir);
            SetError(L"OK");
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
            bit7z::BitMemExtractor extractor{Piv7zLib::data().Get(), format};
            extractor.setPassword(password);
            extractor.extract(buffer, out_dir);
            SetError(L"OK");
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
    static bool Extract(const bit7z::tstring &in_file, std::map<CWString, CVolMem> &out_buffer, const bit7z::tstring &password, const bit7z::BitInFormat &format)
    {
        out_buffer.clear();
        try
        {
            bit7z::BitFileExtractor extractor{Piv7zLib::data().Get(), format};
            extractor.setPassword(password);
            bit7z::BitInputArchive in_archive{extractor, in_file, bit7z::ArchiveStartOffset::None};
            for (const auto &item : in_archive)
            {
                if (item.isDir())
                {
                    auto &it = out_buffer.emplace(std::piecewise_construct, std::forward_as_tuple(item.path().c_str()), std::forward_as_tuple()).first;
                    it->second.Alloc(static_cast<ptrdiff_t>(item.size()), TRUE);
                    in_archive.extractTo(reinterpret_cast<bit7z::byte_t *>(it->second.GetPtr()), static_cast<std::size_t>(item.size()), item.index());
                }
            }
            SetError(L"OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            out_buffer.clear();
            SetError(ex.what());
            return false;
        }
    }

    static bool Extract(int32_t resId, std::map<CWString, CVolMem> &out_buffer, const bit7z::tstring &password, const bit7z::BitInFormat &format)
    {
        out_buffer.clear();
        std::vector<bit7z::byte_t> buffer;
        if (!LoadResIdData(resId, buffer))
            return false;
        try
        {
            bit7z::BitMemExtractor extractor{Piv7zLib::data().Get(), format};
            extractor.setPassword(password);
            bit7z::BitInputArchive in_archive{extractor, buffer, bit7z::ArchiveStartOffset::None};
            for (const auto &item : in_archive)
            {
                if (item.isDir())
                {
                    auto &it = out_buffer.emplace(std::piecewise_construct, std::forward_as_tuple(item.path().c_str()), std::forward_as_tuple()).first;
                    it->second.Alloc(static_cast<ptrdiff_t>(item.size()), TRUE);
                    in_archive.extractTo(reinterpret_cast<bit7z::byte_t *>(it->second.GetPtr()), static_cast<std::size_t>(item.size()), item.index());
                }
            }
            SetError(L"OK");
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
            bit7z::BitFileExtractor extractor{Piv7zLib::data().Get(), format};
            extractor.setPassword(password);
            extractor.extractMatching(in_file, filter, out_dir, isExclude ? bit7z::FilterPolicy::Exclude : bit7z::FilterPolicy::Include);
            SetError(L"OK");
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
            bit7z::BitMemExtractor extractor{Piv7zLib::data().Get(), format};
            extractor.setPassword(password);
            extractor.extractMatching(buffer, filter, out_dir, isExclude ? bit7z::FilterPolicy::Exclude : bit7z::FilterPolicy::Include);
            SetError(L"OK");
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
    static bool Extract(const bit7z::tstring &in_file, const bit7z::tstring &filter, CVolMem &out_buffer, const bit7z::tstring &password, CWString &out_file, bool isExclude, const bit7z::BitInFormat &format)
    {
        out_file.Empty();
        try
        {
            if (filter.empty())
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::FilterNotSpecified));
            bit7z::BitFileExtractor extractor{Piv7zLib::data().Get(), format};
            extractor.setPassword(password);
            bit7z::BitInputArchive in_archive{extractor, in_file, bit7z::ArchiveStartOffset::None};
            for (const auto &item : in_archive)
            {
                bool item_matches = bit7z::filesystem::fsutil::wildcard_match(filter, item.path());
                if (item_matches != isExclude)
                {
                    out_buffer.Alloc(static_cast<ptrdiff_t>(item.size()), true);
                    in_archive.extractTo(reinterpret_cast<bit7z::byte_t *>(out_buffer.GetPtr()), static_cast<std::size_t>(out_buffer.GetSize()), item.index());
                    out_file.SetText(item.path().c_str());
                    SetError(L"OK");
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

    static bool Extract(int32_t resId, const bit7z::tstring &filter, CVolMem &out_buffer, const bit7z::tstring &password, CWString &out_file, bool isExclude, const bit7z::BitInFormat &format)
    {
        out_file.Empty();
        std::vector<bit7z::byte_t> buffer;
        if (!LoadResIdData(resId, buffer))
            return false;
        try
        {
            if (filter.empty())
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::FilterNotSpecified));
            bit7z::BitMemExtractor extractor{Piv7zLib::data().Get(), format};
            extractor.setPassword(password);
            bit7z::BitInputArchive in_archive{extractor, buffer, bit7z::ArchiveStartOffset::None};
            for (const auto &item : in_archive)
            {
                bool item_matches = bit7z::filesystem::fsutil::wildcard_match(filter, item.path());
                if (item_matches != isExclude)
                {
                    out_buffer.Alloc(static_cast<ptrdiff_t>(item.size()), true);
                    in_archive.extractTo(reinterpret_cast<bit7z::byte_t *>(out_buffer.GetPtr()), static_cast<std::size_t>(out_buffer.GetSize()), item.index());
                    out_file.SetText(item.path().c_str());
                    SetError(L"OK");
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
            bit7z::BitFileExtractor extractor{Piv7zLib::data().Get(), format};
            extractor.setPassword(password);
            extractor.extractMatchingRegex(in_file, regex, out_dir, isExclude ? bit7z::FilterPolicy::Exclude : bit7z::FilterPolicy::Include);
            SetError(L"OK");
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
            bit7z::BitMemExtractor extractor{Piv7zLib::data().Get(), format};
            extractor.setPassword(password);
            extractor.extractMatchingRegex(buffer, regex, out_dir, isExclude ? bit7z::FilterPolicy::Exclude : bit7z::FilterPolicy::Include);
            SetError(L"OK");
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
    static bool ExtractRegex(const bit7z::tstring &in_file, const bit7z::tstring &regex, CVolMem &out_buffer, const bit7z::tstring &password, CWString &out_file, bool isExclude, const bit7z::BitInFormat &format)
    {
        out_file.Empty();
        try
        {
            if (regex.empty())
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::FilterNotSpecified));
            bit7z::BitFileExtractor extractor{Piv7zLib::data().Get(), format};
            extractor.setPassword(password);
            bit7z::BitInputArchive in_archive{extractor, in_file, bit7z::ArchiveStartOffset::None};
            const bit7z::tregex regex_filter(regex, bit7z::tregex::ECMAScript | bit7z::tregex::optimize);
            for (const auto &item : in_archive)
            {
                bool item_matches = std::regex_match(item.path(), regex_filter);
                if (item_matches != isExclude)
                {
                    out_buffer.Alloc(static_cast<ptrdiff_t>(item.size()), true);
                    in_archive.extractTo(reinterpret_cast<bit7z::byte_t *>(out_buffer.GetPtr()), static_cast<std::size_t>(out_buffer.GetSize()), item.index());
                    out_file.SetText(item.path().c_str());
                    SetError(L"OK");
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

    static bool ExtractRegex(int32_t resId, const bit7z::tstring &regex, CVolMem &out_buffer, const bit7z::tstring &password, CWString &out_file, bool isExclude, const bit7z::BitInFormat &format)
    {
        out_file.Empty();
        std::vector<bit7z::byte_t> buffer;
        if (!LoadResIdData(resId, buffer))
            return false;
        try
        {
            if (regex.empty())
                throw bit7z::BitException("Cannot extract items", bit7z::make_error_code(bit7z::BitError::FilterNotSpecified));
            bit7z::BitMemExtractor extractor{Piv7zLib::data().Get(), format};
            extractor.setPassword(password);
            bit7z::BitInputArchive in_archive{extractor, buffer, bit7z::ArchiveStartOffset::None};
            const bit7z::tregex regex_filter(regex, bit7z::tregex::ECMAScript | bit7z::tregex::optimize);
            for (const auto &item : in_archive)
            {
                bool item_matches = std::regex_match(item.path(), regex_filter);
                if (item_matches != isExclude)
                {
                    out_buffer.Alloc(static_cast<ptrdiff_t>(item.size()), true);
                    in_archive.extractTo(reinterpret_cast<bit7z::byte_t *>(out_buffer.GetPtr()), static_cast<std::size_t>(out_buffer.GetSize()), item.index());
                    out_file.SetText(item.path().c_str());
                    SetError(L"OK");
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
            bit7z::BitFileExtractor extractor{Piv7zLib::data().Get(), format};
            extractor.setPassword(password);
            extractor.test(in_file);
            SetError(L"OK");
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
 * @brief 存档处理模板类
 * @tparam archive_t bit7z类
 */
template <class archive_t>
class PivArchiveHandler
{
protected:
    int32_t m_ratio = 0;
    uint64_t m_total_size = 0;
    std::unique_ptr<archive_t> m_archive{nullptr};
    std::unique_ptr<std::vector<bit7z::byte_t>> m_buffer{nullptr};
    std::function<int32_t(CWString &, int32_t, int64_t, int64_t, int32_t)> m_progress;
    std::wstring m_error;
    std::wstring m_progress_file;

    /**
     * @brief 置最后错误
     * @param Error 错误信息
     */
    inline void SetError(const char *Error)
    {
        CVolMem buf;
        m_error = GetWideText(Error, buf, NULL);
    }

    inline void SetError(const wchar_t *Error)
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
        if (m_progress)
            return !m_progress(CWString{m_progress_file.c_str()}, static_cast<int32_t>(100 * progress_size / m_total_size),
                               static_cast<int64_t>(m_total_size), static_cast<int64_t>(progress_size), m_ratio);
        return true;
    }

    void FileNameCB(bit7z::tstring fileName)
    {
        m_progress_file = fileName;
    }

public:
    PivArchiveHandler() {}
    ~PivArchiveHandler() {}

    template <typename Fun, typename... Args>
    inline void SetEvent(Fun &&fun, Args &&...args)
    {
        m_progress = std::bind(std::forward<Fun>(fun), std::forward<Args>(args)...);
    }

    /**
     * @brief 取最后错误
     * @return
     */
    inline CWString GetError() const
    {
        return CWString{m_error.c_str()};
    }

    inline archive_t &data()
    {
        return *m_archive;
    }

    inline archive_t *pdata()
    {
        return m_archive.get();
    }

    /**
     * @brief 是否为空
     * @return
     */
    inline bool IsNull() const
    {
        return m_archive == nullptr;
    }

    /**
     * @brief 关闭
     */
    virtual inline void CloseArchive()
    {
        m_buffer.reset();
        m_archive.reset();
    }

    /**
     * @brief 启用进度反馈
     */
    void EnableFeeback(bool enable)
    {
        if (!m_archive)
            return;
        if (enable)
        {
            m_archive->setTotalCallback(std::bind(&PivArchiveHandler::TotalSizeCB, this, std::placeholders::_1));
            m_archive->setProgressCallback(std::bind(&PivArchiveHandler::ProgressCB, this, std::placeholders::_1));
            m_archive->setRatioCallback(std::bind(&PivArchiveHandler::RatioCB, this, std::placeholders::_1, std::placeholders::_2));
            m_archive->setFileCallback(std::bind(&PivArchiveHandler::FileNameCB, this, std::placeholders::_1));
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
    bool HasFeeback() const
    {
        return m_archive ? !!m_archive->progressCallback() : false;
    }

    /**
     * @brief 设置压缩密码
     * @param password 密码
     */
    inline void SetPassword(const bit7z::tstring &password)
    {
        if (m_archive)
            m_archive->setPassword(password);
    }

    /**
     * @brief 获取压缩密码
     * @return
     */
    inline CWString Password() const
    {
        return m_archive ? CWString(m_archive->password().c_str()) : CWString(L"");
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
    inline bool IsPasswordDefined() const
    {
        return m_archive ? m_archive->isPasswordDefined() : false;
    }

    /**
     * @brief 设置保留目录结构
     * @param retain 是否保留目录结构
     */
    inline void SetRetainDirectories(bool retain)
    {
        if (m_archive)
            m_archive->setRetainDirectories(retain);
    }

    /**
     * @brief 是否保留目录结构
     * @return
     */
    inline bool RetainDirectories() const
    {
        return m_archive ? m_archive->retainDirectories() : false;
    }

    /**
     * @brief 取项目数
     * @return
     */
    inline int32_t ItemsCount() const
    {
        return m_archive ? static_cast<int32_t>(m_archive->itemsCount()) : -1;
    }

}; // PivArchiveHandler

/**
 * @brief 存档创建模板类
 * @tparam archive_t bit7z类
 */
template <class archive_t>
class PivArchiveCreator : public PivArchiveHandler<archive_t>
{
protected:
    std::vector<std::vector<bit7z::byte_t>> m_buffer_array;

public:
    PivArchiveCreator() {}
    ~PivArchiveCreator() {}

    /**
     * @brief 关闭
     */
    inline void CloseArchive() override
    {
        m_buffer_array.swap(std::vector<std::vector<bit7z::byte_t>>{});
        m_buffer.reset();
        m_archive.reset();
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
            SetError(L"未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addItems(path_array);
            SetError(L"OK");
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
            in_paths.emplace_back(path_array.GetAt(i));
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
            SetError(L"未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addItems(path_map);
            SetError(L"OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }
    bool AddItems(std::map<CWString, CWString> &path_map)
    {
        std::map<bit7z::tstring, bit7z::tstring> in_paths;
        for (const auto &pair : path_map)
        {
            in_paths.emplace(std::piecewise_construct, std::forward_as_tuple(pair.first.GetText()), std::forward_as_tuple(pair.second.GetText()));
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
            SetError(L"未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addFile(in_path, alias);
            SetError(L"OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 添加内存流
     * @param in_stream 内存流
     * @param path 路径
     * @return
     */
    bool AddFile(std::istream &in_stream, const bit7z::tstring &path)
    {
        if (!m_archive)
        {
            SetError(L"未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addFile(in_stream, path);
            SetError(L"OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

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
            SetError(L"未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_buffer_array.emplace_back(reinterpret_cast<const bit7z::byte_t *>(in_buffer.GetPtr()), reinterpret_cast<const bit7z::byte_t *>(in_buffer.GetEndPtr()));
            m_archive->addFile(m_buffer_array.back(), path);
            SetError(L"OK");
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
            SetError(L"未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_buffer_array.emplace_back(reinterpret_cast<const bit7z::byte_t *>(in_buffer), reinterpret_cast<const bit7z::byte_t *>(in_buffer) + buffer_len);
            m_archive->addFile(m_buffer_array.back(), path);
            SetError(L"OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 添加文件资源内存
     * @param resid
     * @param buffer_len
     * @param path
     * @return
     */
    bool AddFile(int32_t resid, const bit7z::tstring &path)
    {
        if (!m_archive)
        {
            SetError(L"未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_buffer_array.emplace_back();
            PivArchiveOperate::LoadResIdData(resid, m_buffer_array.back());
            m_archive->addFile(m_buffer_array.back(), path);
            SetError(L"OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 添加文件数组
     * @param in_files 文件数组
     * @return
     */
    bool AddFiles(std::vector<bit7z::tstring> &in_files)
    {
        if (!m_archive)
        {
            SetError(L"未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addFiles(in_files);
            SetError(L"OK");
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
            in_paths.emplace_back(in_files.GetAt(i));
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
            SetError(L"未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addFiles(in_dir, filter, isExclude ? bit7z::FilterPolicy::Exclude : bit7z::FilterPolicy::Include, recursion);
            SetError(L"OK");
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
            SetError(L"未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addDirectory(in_dir);
            SetError(L"OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 添加匹配目录
     * @param in_dir 目录路径
     * @param filter 筛选器
     * @param isExclude 是否排除匹配项
     * @param recursion 递归子目录
     * @return
     */
    bool addDirectoryContents(const bit7z::tstring &in_dir, const bit7z::tstring &filter, bool isExclude, bool recursion)
    {
        if (!m_archive)
        {
            SetError(L"未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->addDirectoryContents(in_dir, filter, isExclude ? bit7z::FilterPolicy::Exclude : bit7z::FilterPolicy::Include, recursion);
            SetError(L"OK");
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
            SetError(L"未打开或创建压缩包!");
            return false;
        }
        try
        {
            m_archive->compressTo(out_file);
            SetError(L"OK");
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
            SetError(L"未打开或创建压缩包!");
            return false;
        }
        try
        {
            std::vector<bit7z::byte_t> buffer;
            m_archive->compressTo(buffer);
            out_buffer.Append(buffer.data(), buffer.size());
            SetError(L"OK");
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
    inline int8_t CompressionFormat() const
    {
        return m_archive ? static_cast<int8_t>(m_archive->compressionFormat().value()) : 0;
    }

    /**
     * @brief (设置/获取)压缩等级
     * @param level 新等级
     */
    inline void SetCompressionLevel(bit7z::BitCompressionLevel level)
    {
        if (m_archive)
            m_archive->setCompressionLevel(level);
    }
    inline int32_t CompressionLevel() const
    {
        return m_archive ? static_cast<int32_t>(m_archive->compressionLevel()) : 0;
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
    inline int32_t CompressionMethod() const
    {
        return m_archive ? static_cast<int32_t>(m_archive->compressionMethod()) : 0;
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
    inline int32_t DictionarySize() const
    {
        return m_archive ? static_cast<int32_t>(m_archive->dictionarySize()) : 0;
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
    inline int32_t WordSize() const
    {
        return m_archive ? static_cast<int32_t>(m_archive->wordSize()) : 0;
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
    inline bool SolidMode() const
    {
        return m_archive ? m_archive->solidMode() : false;
    }

    /**
     * @brief (设置/获取)更新方式
     * @param mode 新更新方式
     */
    inline void SetUpdateMode(bit7z::UpdateMode mode)
    {
        if (m_archive)
            m_archive->setUpdateMode(mode);
    }
    inline bit7z::UpdateMode UpdateMode() const
    {
        return m_archive ? m_archive->updateMode() : bit7z::UpdateMode::None;
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
    inline int32_t VolumeSize() const
    {
        return m_archive ? static_cast<int32_t>(m_archive->volumeSize()) : 0;
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
    inline int32_t ThreadsCount() const
    {
        return m_archive ? static_cast<int32_t>(m_archive->threadsCount()) : 0;
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
    inline bool StoreSymbolicLinks() const
    {
        return m_archive ? m_archive->storeSymbolicLinks() : false;
    }

    /**
     * @brief 设置密码
     * @param password 新密码
     * @param crypt_headers 加密文件头
     */
    inline void SetPasswordEx(const bit7z::tstring &password, bool crypt_headers)
    {
        if (m_archive)
            m_archive->setPassword(password, crypt_headers);
    }

    /**
     * @brief 是否加密文件头
     * @return
     */
    inline bool CryptHeaders() const
    {
        return m_archive ? m_archive->cryptHeaders() : false;
    }
}; // PivArchiveCreator

/**
 * @brief 存档解压类
 */
class PivArchiveReader : public PivArchiveHandler<bit7z::BitArchiveReader>
{
public:
    PivArchiveReader() {}
    ~PivArchiveReader()
    {
        CloseArchive();
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
            m_archive.reset(new bit7z::BitArchiveReader{Piv7zLib::data().Get(), in_file, format, password});
            EnableFeeback(feedback);
            SetError(L"OK");
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
            m_archive.reset(new bit7z::BitArchiveReader{Piv7zLib::data().Get(), in_stream, format, password});
            EnableFeeback(feedback);
            SetError(L"OK");
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
            m_archive.reset(new bit7z::BitArchiveReader{Piv7zLib::data().Get(), in_buffer, format, password});
            EnableFeeback(feedback);
            SetError(L"OK");
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
            m_archive.reset(new bit7z::BitArchiveReader{Piv7zLib::data().Get(), *m_buffer, format, password});
            EnableFeeback(feedback);
            SetError(L"OK");
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
            m_archive.reset(new bit7z::BitArchiveReader{Piv7zLib::data().Get(), *m_buffer, format, password});
            EnableFeeback(feedback);
            SetError(L"OK");
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
        try
        {
            m_buffer.reset(new std::vector<bit7z::byte_t>{});
            PivArchiveOperate::LoadResIdData(resId, *m_buffer);
            m_archive.reset(new bit7z::BitArchiveReader{Piv7zLib::data().Get(), *m_buffer, format, password});
            EnableFeeback(feedback);
            SetError(L"OK");
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
            SetError(L"未打开压缩文件.");
            return false;
        }
        try
        {
            m_archive->extractTo(out_dir);
            SetError(L"OK");
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
    bool Extract(std::map<CWString, CVolMem> &out_buffer)
    {
        out_buffer.clear();
        if (!m_archive)
        {
            SetError(L"未打开压缩文件.");
            return false;
        }
        try
        {
            std::map<bit7z::tstring, std::vector<bit7z::byte_t>> out_map;
            m_archive->extractTo(out_map);
            for (auto &pair : out_map)
            {
                auto &it = out_buffer.emplace(std::piecewise_construct, std::forward_as_tuple(pair.first.c_str()), std::forward_as_tuple()).first;
                it->second.SetMemAlignSize(static_cast<ptrdiff_t>(pair.second.size()));
                it->second.Append(pair.second.data(), pair.second.size());
                pair.second.swap(std::vector<bit7z::byte_t>{});
            }
            SetError(L"OK");
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
    bool ExtractEx(std::map<CWString, CVolMem> &out_buffer)
    {
        out_buffer.clear();
        if (!m_archive)
        {
            SetError(L"未打开压缩文件.");
            return false;
        }
        try
        {
            for (auto &item : *m_archive)
            {
                if (item.isDir())
                {
                    auto &it = out_buffer.emplace(std::piecewise_construct, std::forward_as_tuple(item.path().c_str()), std::forward_as_tuple()).first;
                    it->second.Alloc(static_cast<ptrdiff_t>(item.size()), TRUE);
                    m_archive->extractTo(reinterpret_cast<bit7z::byte_t *>(it->second.GetPtr()), static_cast<std::size_t>(item.size()), item.index());
                }
            }
            SetError(L"OK");
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
    bool Extract(int32_t idx, CVolMem &out_buffer, CWString &out_path)
    {
        out_path.Empty();
        if (!m_archive)
        {
            SetError(L"未打开压缩文件.");
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
            SetError(L"OK");
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
            SetError(L"未打开压缩文件.");
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
            SetError(L"OK");
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
            SetError(L"未打开压缩文件.");
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
            SetError(L"OK");
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
    bool Extract(const bit7z::tstring &filter, CVolMem &out_buffer, CWString &out_path, bool isExclude)
    {
        out_path.Empty();
        if (!m_archive)
        {
            SetError(L"未打开压缩文件.");
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
                    SetError(L"OK");
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
            SetError(L"未打开压缩文件.");
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
            SetError(L"OK");
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
    bool ExtractRegex(const bit7z::tstring &regex, CVolMem &out_buffer, CWString &out_path, bool isExclude)
    {
        out_path.Empty();
        if (!m_archive)
        {
            SetError(L"未打开压缩文件.");
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
                    SetError(L"OK");
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
            SetError(L"未打开压缩文件.");
            return false;
        }
        try
        {
            m_archive->test();
            SetError(L"OK");
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
            SetError(L"未打开压缩文件.");
            return false;
        }
        try
        {
            m_archive->testItem(static_cast<uint32_t>(index));
            SetError(L"OK");
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
    inline int8_t DetectedFormat() const
    {
        return m_archive ? static_cast<int8_t>(m_archive->detectedFormat().value()) : 0;
    }

    /**
     * @brief 取压缩包属性表
     * @return
     */
    inline std::map<bit7z::BitProperty, bit7z::BitPropVariant> ArchiveProperties() const
    {
        return m_archive ? m_archive->archiveProperties() : std::map<bit7z::BitProperty, bit7z::BitPropVariant>{};
    }

    /**
     * @brief 取压缩包属性
     * @param property
     * @return
     */
    inline bit7z::BitPropVariant ArchiveProperty(bit7z::BitProperty property) const
    {
        return m_archive ? m_archive->archiveProperty(property) : bit7z::BitPropVariant{};
    }

    /**
     * @brief 取项目信息表
     * @return
     */
    inline std::vector<bit7z::BitArchiveItemInfo> Items()
    {
        return m_archive ? m_archive->items() : std::vector<bit7z::BitArchiveItemInfo>{};
    }

    /**
     * @brief 取成员属性
     * @param idx 索引
     * @param property 属性名
     * @return
     */
    inline bit7z::BitPropVariant ItemProperty(int32_t idx, bit7z::BitProperty property) const
    {
        return m_archive ? m_archive->itemProperty(static_cast<uint32_t>(idx), property) : bit7z::BitPropVariant{};
    }

    /**
     * @brief 是否为目录项目
     * @param idx 项目索引
     * @return
     */
    inline bool IsItemFolder(int32_t idx) const
    {
        return m_archive ? m_archive->isItemFolder(static_cast<uint32_t>(idx)) : false;
    }

    /**
     * @brief 是否为加密项目
     * @param idx 项目索引
     * @return
     */
    inline bool IsItemEncrypted(int32_t idx) const
    {
        return m_archive ? m_archive->isItemEncrypted(static_cast<uint32_t>(idx)) : false;
    }

    /**
     * @brief 取压缩文件路径
     * @return
     */
    inline CWString ArchivePath() const
    {
        return m_archive ? CWString{m_archive->archivePath().c_str()} : CWString(L"");
    }

    /**
     * @brief 取文件夹数量
     * @return
     */
    inline int32_t foldersCount() const
    {
        return m_archive ? static_cast<int32_t>(m_archive->foldersCount()) : -1;
    }

    /**
     * @brief 取文件数量
     * @return
     */
    inline int32_t FilesCount() const
    {
        return m_archive ? static_cast<int32_t>(m_archive->filesCount()) : -1;
    }

    /**
     * @brief 取总长度
     * @return
     */
    inline int64_t Size() const
    {
        return m_archive ? static_cast<int64_t>(m_archive->size()) : -1;
    }

    /**
     * @brief 取打包长度
     * @return
     */
    inline int64_t PackSize() const
    {
        return m_archive ? static_cast<int64_t>(m_archive->packSize()) : -1;
    }

    /**
     * @brief 是否有加密项目
     * @return
     */
    inline bool HasEncryptedItems() const
    {
        return m_archive ? m_archive->hasEncryptedItems() : false;
    }

    /**
     * @brief 是否已加密
     * @return
     */
    inline bool IsEncrypted() const
    {
        return m_archive ? m_archive->isEncrypted() : false;
    }

    /**
     * @brief 取分卷数量
     * @return
     */
    inline int32_t VolumesCount() const
    {
        return m_archive ? static_cast<int32_t>(m_archive->volumesCount()) : -1;
    }

    /**
     * @brief 是否为分卷
     * @return
     */
    inline bool IsMultiVolume() const
    {
        return m_archive ? m_archive->isMultiVolume() : false;
    }

    /**
     * @brief 是否为固实
     * @return
     */
    inline bool IsSolid() const
    {
        return m_archive ? m_archive->isSolid() : false;
    }

    /**
     * @brief 取尾项目索引
     * @return
     */
    inline int32_t EndIndex() const
    {
        return m_archive ? static_cast<int32_t>(m_archive->end()->index() - 1) : -1;
    }

    /**
     * @brief 寻找项目索引
     * @param item_path 项目路径
     * @return
     */
    inline int32_t FindIndex(const bit7z::tstring &item_path) const
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
    inline bool IsContain(const bit7z::tstring &item_path) const
    {
        return m_archive ? m_archive->contains(item_path) : false;
    }
}; // PivArchiveReader

/**
 * @brief 存档压缩类
 */
class PivArchiveWirter : public PivArchiveCreator<bit7z::BitArchiveWriter>
{
public:
    PivArchiveWirter() {}
    ~PivArchiveWirter()
    {
        CloseArchive();
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
            m_archive.reset(new bit7z::BitArchiveWriter{Piv7zLib::data().Get(), format});
            EnableFeeback(feedback);
            SetError(L"OK");
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
            m_archive.reset(new bit7z::BitArchiveWriter{Piv7zLib::data().Get(), in_file, format, password});
            EnableFeeback(feedback);
            SetError(L"OK");
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
            m_archive.reset(new bit7z::BitArchiveWriter{Piv7zLib::data().Get(), in_buffer, format, password});
            EnableFeeback(feedback);
            SetError(L"OK");
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
            m_archive.reset(new bit7z::BitArchiveWriter{Piv7zLib::data().Get(), in_stream, format, password});
            EnableFeeback(feedback);
            SetError(L"OK");
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
            m_archive.reset(new bit7z::BitArchiveWriter{Piv7zLib::data().Get(), *m_buffer, format, password});
            EnableFeeback(feedback);
            SetError(L"OK");
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
    bool CreateArchive(int32_t resid, bool feedback, const bit7z::BitInOutFormat &format, const bit7z::tstring &password)
    {
        CloseArchive();
        m_buffer.reset(new std::vector<bit7z::byte_t>{});
        PivArchiveOperate::LoadResIdData(resid, *m_buffer);
        try
        {
            m_archive.reset(new bit7z::BitArchiveWriter{Piv7zLib::data().Get(), *m_buffer, format, password});
            EnableFeeback(feedback);
            SetError(L"OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            CloseArchive();
            SetError(ex.what());
            return false;
        }
    }
}; // PivArchiveWirter

/**
 * @brief 存档编辑类
 */
class PivArchiveEditor : public PivArchiveCreator<bit7z::BitArchiveEditor>
{
public:
    PivArchiveEditor() {}
    ~PivArchiveEditor()
    {
        CloseArchive();
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
    bool OpenFile(const bit7z::tstring &in_file, bool feedback, const bit7z::BitInOutFormat &format, bit7z::UpdateMode update_mode, const bit7z::tstring &password)
    {
        CloseArchive();
        try
        {
            m_archive.reset(new bit7z::BitArchiveEditor{Piv7zLib::data().Get(), in_file, format, password});
            m_archive->setUpdateMode(update_mode);
            EnableFeeback(feedback);
            SetError(L"OK");
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
            SetError(L"未打开压缩文件!");
            return false;
        }
        try
        {
            m_archive->applyChanges();
            m_buffer_array.swap(std::vector<std::vector<bit7z::byte_t>>{});
            SetError(L"OK");
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
            SetError(L"未打开压缩文件!");
            return false;
        }
        try
        {
            m_archive->renameItem(static_cast<uint32_t>(index), new_path);
            SetError(L"OK");
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
            SetError(L"未打开压缩文件!");
            return false;
        }
        try
        {
            m_archive->renameItem(old_path, new_path);
            SetError(L"OK");
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
            SetError(L"未打开压缩文件!");
            return false;
        }
        try
        {
            m_archive->updateItem(static_cast<uint32_t>(index), in_file);
            SetError(L"OK");
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
            SetError(L"未打开压缩文件!");
            return false;
        }
        try
        {
            m_buffer_array.emplace_back(reinterpret_cast<const bit7z::byte_t *>(in_buffer.GetPtr()), reinterpret_cast<const bit7z::byte_t *>(in_buffer.GetEndPtr()));
            m_archive->updateItem(static_cast<uint32_t>(index), m_buffer_array.back());
            SetError(L"OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

    /**
     * @brief 更新索引项目自内存流
     * @param index 项目索引
     * @param in_stream 内存流
     * @return
     */
    bool UpdateItem(int32_t index, std::istream &in_stream)
    {
        if (!m_archive)
        {
            SetError(L"未打开压缩文件!");
            return false;
        }
        try
        {
            m_archive->updateItem(static_cast<uint32_t>(index), in_stream);
            SetError(L"OK");
            return true;
        }
        catch (const bit7z::BitException &ex)
        {
            SetError(ex.what());
            return false;
        }
    }

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
            SetError(L"未打开压缩文件!");
            return false;
        }
        try
        {
            m_archive->updateItem(item_path, in_file);
            SetError(L"OK");
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
            SetError(L"未打开压缩文件!");
            return false;
        }
        try
        {
            m_buffer_array.emplace_back(reinterpret_cast<const bit7z::byte_t *>(in_buffer.GetPtr()), reinterpret_cast<const bit7z::byte_t *>(in_buffer.GetEndPtr()));
            m_archive->updateItem(item_path, m_buffer_array.back());
            SetError(L"OK");
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
            SetError(L"未打开压缩文件!");
            return false;
        }
        try
        {
            m_archive->updateItem(item_path, in_stream);
            SetError(L"OK");
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
     * @param recurse_dirs 递归删除目录
     * @return
     */
    bool DeleteItem(int32_t index, bool recurse_dirs = false)
    {
        if (!m_archive)
        {
            SetError(L"未打开压缩文件!");
            return false;
        }
        try
        {
            m_archive->deleteItem(static_cast<uint32_t>(index), recurse_dirs ? bit7z::DeletePolicy::RecurseDirs : bit7z::DeletePolicy::ItemOnly);
            SetError(L"OK");
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
     * @param recurse_dirs 递归删除目录
     * @return
     */
    bool DeleteItem(const bit7z::tstring &item_path, bool recurse_dirs = false)
    {
        if (!m_archive)
        {
            SetError(L"未打开压缩文件!");
            return false;
        }
        try
        {
            m_archive->deleteItem(item_path, recurse_dirs ? bit7z::DeletePolicy::RecurseDirs : bit7z::DeletePolicy::ItemOnly);
            SetError(L"OK");
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
