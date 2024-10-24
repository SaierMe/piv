/*********************************************\
 * 火山视窗PIV模块 - xxHash                  *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef PIV_XXHASH_HPP
#define PIV_XXHASH_HPP

#include "../piv_encoding.hpp"

#ifndef XXH_INLINE_ALL
#define XXH_INLINE_ALL
#endif
#include "xxhash.h"

namespace piv
{
    namespace hash
    {
        /**
         * @brief 生成密码
         * @param secretSize 密码长度
         * @param customSeed 自定义种子
         * @return 返回所生成的密码
         */
        static CVolMem Generate_Secret(size_t secretSize, const CVolMem &customSeed)
        {
            CVolMem secret;
            secret.Alloc(secretSize, TRUE);
            if (XXH3_generateSecret(secret.GetPtr(), secretSize, customSeed.GetPtr(), static_cast<size_t>(customSeed.GetSize())) == XXH_OK)
                return secret;
            return CVolMem();
        }

        /**
         * @brief 生成密码_192
         * @param seed 种子值
         * @return 返回所生成的密码
         */
        static CVolMem Generate_Secret_fromSeed(uint64_t seed)
        {
            CVolMem secret;
            secret.Alloc(192, TRUE);
            XXH3_generateSecret_fromSeed(secret.GetPtr(), seed);
            return secret;
        }

        /**
         * @brief 取数据XXH128
         * @tparam T 返回类型(字节集类、文本型)
         * @param input 所欲摘要的数据
         * @param len 数据字节长度
         * @param seed 种子
         * @param rethash 返回的hash值
         * @return hash值
         */
        template <typename T>
        T &&Get_XXH128(const void *input, size_t len, uint64_t seed = 0, T &&rethash = T{})
        {
            return std::forward<T>(piv::encoding::value_to_hex(XXH3_128bits_withSeed(input, len, seed), rethash));
        }

        template <typename T>
        T &&Get_XXH128(const ptrdiff_t &input, size_t len, uint64_t seed = 0, T &&rethash = T{})
        {
            return std::forward<T>(piv::encoding::value_to_hex(XXH3_128bits_withSeed(reinterpret_cast<const void *>(input), len, seed), rethash));
        }

        template <typename T>
        T &&Get_XXH128(const CVolMem &input, size_t len = (size_t)-1, uint64_t seed = 0, T &&rethash = T{})
        {
            return std::forward<T>(piv::encoding::value_to_hex(XXH3_128bits_withSeed(input.GetPtr(),
                                                                                     len == (size_t)-1 ? static_cast<size_t>(input.GetSize()) : len, seed),
                                                               rethash));
        }

        template <typename T>
        T &&Get_XXH128(const CWString &input, size_t len = (size_t)-1, uint64_t seed = 0, T &&rethash = T{})
        {
            return std::forward<T>(piv::encoding::value_to_hex(XXH3_128bits_withSeed(input.GetText(),
                                                                                     len == (size_t)-1 ? static_cast<size_t>(input.GetLength() * 2) : len, seed),
                                                               rethash));
        }

        template <typename T, typename CharT>
        T &&Get_XXH128(const std::basic_string<CharT> &input, size_t len = (size_t)-1, uint64_t seed = 0, T &&rethash = T{})
        {
            return std::forward<T>(piv::encoding::value_to_hex(XXH3_128bits_withSeed(input.data(),
                                                                                     len == (size_t)-1 ? input.size() * sizeof(CharT) : len, seed),
                                                               rethash));
        }

        template <typename T, typename CharT>
        T &&Get_XXH128(const basic_string_view<CharT> &input, size_t len = (size_t)-1, uint64_t seed = 0, T &&rethash = T{})
        {
            return std::forward<T>(piv::encoding::value_to_hex(XXH3_128bits_withSeed(input.data(),
                                                                                     len == (size_t)-1 ? input.size() * sizeof(CharT) : len, seed),
                                                               rethash));
        }

        /**
         * @brief 取数据HMAC_XXH128
         * @tparam T 返回类型(字节集类、文本型)
         * @param input 所欲摘要的数据
         * @param len 数据字节长度
         * @param secret 密码
         * @param seed 种子
         * @param rethash 返回的hash值
         * @return hash值
         */
        template <typename T>
        T &&Get_XXH128_withSecret(const void *input, size_t len, const CVolMem &secret = CVolMem{}, uint64_t seed = 0, T &&rethash = T{})
        {
            if (secret.GetSize() > 0)
                return std::forward<T>(piv::encoding::value_to_hex(XXH3_128bits_withSecretandSeed(input, len, secret.GetPtr(), static_cast<size_t>(secret.GetSize()), seed), rethash));
            else
                return std::forward<T>(piv::encoding::value_to_hex(XXH3_128bits_withSeed(input, len, seed), rethash));
        }

        template <typename T>
        T &&Get_XXH128_withSecret(const ptrdiff_t &input, size_t len, const CVolMem &secret = CVolMem{}, uint64_t seed = 0, T &&rethash = T{})
        {
            return std::forward<T>(Get_XXH128_withSecret(reinterpret_cast<const void *>(input), len, secret, seed, rethash));
        }

        template <typename T>
        T &&Get_XXH128_withSecret(const CVolMem &input, size_t len = (size_t)-1, const CVolMem &secret = CVolMem{}, uint64_t seed = 0, T &&rethash = T{})
        {
            return std::forward<T>(Get_XXH128_withSecret(input.GetPtr(), len == (size_t)-1 ? static_cast<size_t>(input.GetSize()) : len, secret, seed, rethash));
        }

        template <typename T>
        T &&Get_XXH128_withSecret(const CWString &input, size_t len = (size_t)-1, const CVolMem &secret = CVolMem{}, uint64_t seed = 0, T &&rethash = T{})
        {
            return std::forward<T>(Get_XXH128_withSecret(input.GetText(), len == (size_t)-1 ? input.GetLength() * 2 : len, secret, seed, rethash));
        }

        template <typename T, typename CharT>
        T &&Get_XXH128_withSecret(const std::basic_string<CharT> &input, size_t len = (size_t)-1, const CVolMem &secret = CVolMem{}, uint64_t seed = 0, T &&rethash = T{})
        {
            return std::forward<T>(Get_XXH128_withSecret(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, secret, seed, rethash));
        }

        template <typename T, typename CharT>
        T &&Get_XXH128_withSecret(const basic_string_view<CharT> &input, size_t len = (size_t)-1, const CVolMem &secret = CVolMem{}, uint64_t seed = 0, T &&rethash = T{})
        {
            return std::forward<T>(Get_XXH128_withSecret(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, secret, seed, rethash));
        }

        /**
         * @brief 取文件_XXH128
         * @tparam T 返回类型(字节集类、文本型)
         * @param filename 文件名
         * @param off 文件偏移
         * @param len 数据长度
         * @param secret 密码
         * @param seed 种子
         * @param rethash 返回的hash值
         * @return hash值
         */
        template <typename T>
        T &&GetFile_XXH128(const wchar_t *filename, int64_t off, uint64_t len, const CVolMem &secret = CVolMem{}, uint64_t seed = 0, T &&rethash = T{})
        {
            XXH128_hash_t hash = {0};
            FILE *file = NULL;
            file = _wfopen(filename, L"rb");
            if (file)
            {
                if (off > 0)
                    _fseeki64(file, off, SEEK_SET);
                XXH3_state_t *state = XXH3_createState();
                if (secret.GetSize() >= 136)
                    XXH3_128bits_reset_withSecretandSeed(state, secret.GetPtr(), static_cast<size_t>(secret.GetSize()), seed);
                else
                    XXH3_128bits_reset_withSeed(state, seed);
                unsigned char buff[65536];
                size_t rsize = 0;
                while ((rsize = fread(buff, 1, 65536, file)) > 0)
                {
                    if (rsize > len)
                    {
                        XXH3_128bits_update(state, buff, static_cast<size_t>(len));
                        break;
                    }
                    XXH3_128bits_update(state, buff, rsize);
                    len -= rsize;
                }
                hash = XXH3_128bits_digest(state);
                XXH3_freeState(state);
                fclose(file);
            }
            return std::forward<T>(piv::encoding::value_to_hex(hash, rethash));
        }

        /**
         * @brief 取资源_XXH128
         * @tparam T 返回类型(字节集类、文本型)
         * @param resId 文件资源ID
         * @param off 文件偏移
         * @param len 数据长度
         * @param secret 密码
         * @param seed 种子
         * @param rethash 返回的hash值
         * @return hash值
         */
        template <typename T>
        T &&GetRes_XXH128(int32_t resId, int64_t off, uint64_t len, const CVolMem &secret = CVolMem{}, uint64_t seed = 0, T &&rethash = T{})
        {
            XXH128_hash_t hash = {0};
            if (resId == 0)
                return std::forward<T>(piv::encoding::value_to_hex(hash, rethash));
            HMODULE hModule = g_objVolApp.GetInstanceHandle();
            HRSRC hSrc = ::FindResourceW(hModule, MAKEINTRESOURCE(static_cast<WORD>(resId)), RT_RCDATA);
            if (hSrc == NULL)
                return std::forward<T>(piv::encoding::value_to_hex(hash, rethash));
            HGLOBAL resdata = ::LoadResource(hModule, hSrc);
            if (resdata == NULL)
                return std::forward<T>(piv::encoding::value_to_hex(hash, rethash));
            const uint8_t *buffer = reinterpret_cast<const uint8_t *>(::LockResource(resdata));
            size_t _size = ::SizeofResource(hModule, hSrc);
            XXH3_state_t *state = XXH3_createState();
            if (secret.GetSize() >= 136)
                XXH3_128bits_reset_withSecretandSeed(state, secret.GetPtr(), static_cast<size_t>(secret.GetSize()), seed);
            else
                XXH3_128bits_reset_withSeed(state, seed);
            if (off > 0 && buffer != nullptr)
            {
                size_t fileOff = static_cast<size_t>(off);
                buffer = (fileOff < _size) ? (buffer + fileOff) : (buffer + _size);
                _size = (fileOff < _size) ? _size - fileOff : 0;
            }
            XXH3_128bits_update(state, buffer, _size > static_cast<size_t>(len) ? static_cast<size_t>(len) : _size);
            hash = XXH3_128bits_digest(state);
            XXH3_freeState(state);
            return std::forward<T>(piv::encoding::value_to_hex(hash, rethash));
        }

        /**
         * @brief 取数据XXH3(64位)
         * @param input 所欲摘要的数据
         * @param len 数据字节长度
         * @param seed 种子
         * @return hash值
         */
        static XXH64_hash_t Get_XXH3(const void *input, size_t len, uint64_t seed = 0)
        {
            return XXH3_64bits_withSeed(input, len, seed);
        }

        static XXH64_hash_t Get_XXH3(const ptrdiff_t &input, size_t len, uint64_t seed = 0)
        {
            return XXH3_64bits_withSeed(reinterpret_cast<const void *>(input), len, seed);
        }

        static XXH64_hash_t Get_XXH3(const CVolMem &input, size_t len = (size_t)-1, uint64_t seed = 0)
        {
            return XXH3_64bits_withSeed(input.GetPtr(), len == (size_t)-1 ? static_cast<size_t>(input.GetSize()) : len, seed);
        }

        static XXH64_hash_t Get_XXH3(const CWString &input, size_t len = (size_t)-1, uint64_t seed = 0)
        {
            return XXH3_64bits_withSeed(input.GetText(), len == (size_t)-1 ? input.GetLength() * 2 : len, seed);
        }

        template <typename CharT>
        static XXH64_hash_t Get_XXH3(const std::basic_string<CharT> &input, size_t len = -1, uint64_t seed = 0)
        {
            return XXH3_64bits_withSeed(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, seed);
        }

        template <typename CharT>
        XXH64_hash_t Get_XXH3(const basic_string_view<CharT> &input, size_t len = -1, uint64_t seed = 0)
        {
            return XXH3_64bits_withSeed(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, seed);
        }

        /**
         * @brief 取数据HMAC_XXH3(64位)
         * @param input 所欲摘要的数据
         * @param len 数据字节长度
         * @param secret 密码
         * @param seed 种子
         * @return hash值
         */
        static XXH64_hash_t Get_XXH3_withSecret(const void *input, size_t len, const CVolMem &secret = CVolMem{}, uint64_t seed = 0)
        {
            if (secret.GetSize() > 0)
                return XXH3_64bits_withSecretandSeed(input, len, secret.GetPtr(), static_cast<size_t>(secret.GetSize()), seed);
            else
                return XXH3_64bits_withSeed(input, len, seed);
        }

        static XXH64_hash_t Get_XXH3_withSecret(const ptrdiff_t &input, size_t len = (size_t)-1, const CVolMem &secret = CVolMem{}, uint64_t seed = 0)
        {
            return Get_XXH3_withSecret(reinterpret_cast<const void *>(input), len, secret, seed);
        }

        static XXH64_hash_t Get_XXH3_withSecret(const CVolMem &input, size_t len = (size_t)-1, const CVolMem &secret = CVolMem{}, uint64_t seed = 0)
        {
            return Get_XXH3_withSecret(input.GetPtr(), len == (size_t)-1 ? static_cast<size_t>(input.GetSize()) : len, secret, seed);
        }

        static XXH64_hash_t Get_XXH3_withSecret(const CWString &input, size_t len = (size_t)-1, const CVolMem &secret = CVolMem{}, uint64_t seed = 0)
        {
            return Get_XXH3_withSecret(input.GetText(), len == (size_t)-1 ? input.GetLength() * 2 : len, secret, seed);
        }

        template <typename CharT>
        static XXH64_hash_t Get_XXH3_withSecret(const std::basic_string<CharT> &input, size_t len = (size_t)-1, const CVolMem &secret = CVolMem{}, uint64_t seed = 0)
        {
            return Get_XXH3_withSecret(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, secret, seed);
        }

        template <typename CharT>
        XXH64_hash_t Get_XXH3_withSecret(const basic_string_view<CharT> &input, size_t len = (size_t)-1, const CVolMem &secret = CVolMem{}, uint64_t seed = 0)
        {
            return Get_XXH3_withSecret(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, secret, seed);
        }

        /**
         * @brief 取文件_XXH3(64位)
         * @param filename 文件名
         * @param off 文件偏移
         * @param len 数据长度
         * @param secret 密码
         * @param seed 种子
         * @return hash值
         */
        static XXH64_hash_t GetFile_XXH3(const wchar_t *filename, int64_t off, uint64_t len, const CVolMem &secret = CVolMem{}, uint64_t seed = 0)
        {
            XXH64_hash_t hash = 0;
            FILE *file = NULL;
            file = _wfopen(filename, L"rb");
            if (file)
            {
                if (off > 0)
                    _fseeki64(file, off, SEEK_SET);
                XXH3_state_t *state = XXH3_createState();
                if (secret.GetSize() >= 136)
                    XXH3_64bits_reset_withSecretandSeed(state, secret.GetPtr(), static_cast<size_t>(secret.GetSize()), seed);
                else
                    XXH3_64bits_reset_withSeed(state, seed);
                unsigned char buff[65536];
                size_t rsize = 0;
                while ((rsize = fread(buff, 1, 65536, file)) > 0)
                {
                    if (rsize > len)
                    {
                        XXH3_64bits_update(state, buff, static_cast<size_t>(len));
                        break;
                    }
                    XXH3_64bits_update(state, buff, rsize);
                    len -= rsize;
                }
                hash = XXH3_64bits_digest(state);
                XXH3_freeState(state);
                fclose(file);
            }
            return hash;
        }

        /**
         * @brief 取资源_XXH3(64位)
         * @param resId 文件资源ID
         * @param off 文件偏移
         * @param len 数据长度
         * @param secret 密码
         * @param seed 种子
         * @return hash值
         */
        static XXH64_hash_t GetRes_XXH3(int32_t resId, int64_t off, uint64_t len, const CVolMem &secret = CVolMem{}, uint64_t seed = 0)
        {
            XXH64_hash_t hash = 0;
            if (resId == 0)
                return hash;
            HMODULE hModule = g_objVolApp.GetInstanceHandle();
            HRSRC hSrc = ::FindResourceW(hModule, MAKEINTRESOURCE(static_cast<WORD>(resId)), RT_RCDATA);
            if (hSrc == NULL)
                return hash;
            HGLOBAL resdata = ::LoadResource(hModule, hSrc);
            if (resdata == NULL)
                return hash;
            const uint8_t *buffer = reinterpret_cast<const uint8_t *>(::LockResource(resdata));
            size_t _size = ::SizeofResource(hModule, hSrc);
            XXH3_state_t *state = XXH3_createState();
            if (secret.GetSize() >= 136)
                XXH3_64bits_reset_withSecretandSeed(state, secret.GetPtr(), static_cast<size_t>(secret.GetSize()), seed);
            else
                XXH3_64bits_reset_withSeed(state, seed);
            if (off > 0 && buffer != nullptr)
            {
                size_t fileOff = static_cast<size_t>(off);
                buffer = (fileOff < _size) ? (buffer + fileOff) : (buffer + _size);
                _size = (fileOff < _size) ? _size - fileOff : 0;
            }
            XXH3_64bits_update(state, buffer, _size > static_cast<size_t>(len) ? static_cast<size_t>(len) : _size);
            hash = XXH3_64bits_digest(state);
            XXH3_freeState(state);
            return hash;
        }

        /**
         * @brief 取数据XXH64
         * @param input 所欲摘要的数据
         * @param len 数据字节长度
         * @param seed 种子
         * @return hash值
         */
        static XXH64_hash_t Get_XXH64(const void *input, size_t len, uint64_t seed = 0)
        {
            return XXH64(input, len, seed);
        }

        static XXH64_hash_t Get_XXH64(const ptrdiff_t &input, size_t len, uint64_t seed = 0)
        {
            return XXH64(reinterpret_cast<const void *>(input), len, seed);
        }

        static XXH64_hash_t Get_XXH64(const CVolMem &input, size_t len = (size_t)-1, uint64_t seed = 0)
        {
            return XXH64(input.GetPtr(), len == (size_t)-1 ? static_cast<size_t>(input.GetSize()) : len, seed);
        }

        static XXH64_hash_t Get_XXH64(const CWString &input, size_t len = (size_t)-1, uint64_t seed = 0)
        {
            return XXH64(input.GetText(), len == (size_t)-1 ? input.GetLength() * 2 : len, seed);
        }

        template <typename CharT>
        static XXH64_hash_t Get_XXH64(const std::basic_string<CharT> &input, size_t len = -1, uint64_t seed = 0)
        {
            return XXH64(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, seed);
        }

        template <typename CharT>
        XXH64_hash_t Get_XXH64(const basic_string_view<CharT> &input, size_t len = -1, uint64_t seed = 0)
        {
            return XXH64(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, seed);
        }

        /**
         * @brief 取文件_XXH64
         * @param filename 文件名
         * @param off 文件偏移
         * @param len 数据长度
         * @param seed 种子
         * @return hash值
         */
        static XXH64_hash_t GetFile_XXH64(const wchar_t *filename, int64_t off, uint64_t len, uint64_t seed = 0)
        {
            XXH64_hash_t hash = 0;
            FILE *file = NULL;
            file = _wfopen(filename, L"rb");
            if (file)
            {
                if (off > 0)
                    _fseeki64(file, off, SEEK_SET);
                XXH64_state_t *state = XXH64_createState();
                XXH64_reset(state, seed);
                unsigned char buff[65536];
                size_t rsize = 0;
                while ((rsize = fread(buff, 1, 65536, file)) > 0)
                {
                    if (rsize > len)
                    {
                        XXH64_update(state, buff, static_cast<size_t>(len));
                        break;
                    }
                    XXH64_update(state, buff, rsize);
                    len -= rsize;
                }
                hash = XXH64_digest(state);
                XXH64_freeState(state);
                fclose(file);
            }
            return hash;
        }

        /**
         * @brief 取资源_XXH64
         * @param resId 文件资源ID
         * @param off 文件偏移
         * @param len 数据长度
         * @param seed 种子
         * @return hash值
         */
        static XXH64_hash_t GetRes_XXH64(int32_t resId, int64_t off, uint64_t len, uint64_t seed = 0)
        {
            XXH64_hash_t hash = 0;
            if (resId == 0)
                return hash;
            HMODULE hModule = g_objVolApp.GetInstanceHandle();
            HRSRC hSrc = ::FindResourceW(hModule, MAKEINTRESOURCE(static_cast<WORD>(resId)), RT_RCDATA);
            if (hSrc == NULL)
                return hash;
            HGLOBAL resdata = ::LoadResource(hModule, hSrc);
            if (resdata == NULL)
                return hash;
            const uint8_t *buffer = reinterpret_cast<const uint8_t *>(::LockResource(resdata));
            size_t _size = ::SizeofResource(hModule, hSrc);
            XXH64_state_t *state = XXH64_createState();
            XXH64_reset(state, seed);
            if (off > 0 && buffer != nullptr)
            {
                size_t fileOff = static_cast<size_t>(off);
                buffer = (fileOff < _size) ? (buffer + fileOff) : (buffer + _size);
                _size = (fileOff < _size) ? _size - fileOff : 0;
            }
            XXH64_update(state, buffer, _size > static_cast<size_t>(len) ? static_cast<size_t>(len) : _size);
            hash = XXH64_digest(state);
            XXH64_freeState(state);
            return hash;
        }

        /**
         * @brief 取数据XXH32
         * @param input 所欲摘要的数据
         * @param len 数据字节长度
         * @param seed 种子
         * @return hash值
         */
        static XXH32_hash_t Get_XXH32(const void *input, size_t len, uint32_t seed = 0)
        {
            return XXH32(input, len, seed);
        }

        static XXH32_hash_t Get_XXH32(const ptrdiff_t &input, size_t len, uint32_t seed = 0)
        {
            return XXH32(reinterpret_cast<const void *>(input), len, seed);
        }

        static XXH32_hash_t Get_XXH32(const CVolMem &input, size_t len = (size_t)-1, uint32_t seed = 0)
        {
            return XXH32(input.GetPtr(), len == (size_t)-1 ? static_cast<size_t>(input.GetSize()) : len, seed);
        }

        static XXH32_hash_t Get_XXH32(const CWString &input, size_t len = (size_t)-1, uint32_t seed = 0)
        {
            return XXH32(input.GetText(), len == (size_t)-1 ? input.GetLength() * 2 : len, seed);
        }

        template <typename CharT>
        static XXH32_hash_t Get_XXH32(const std::basic_string<CharT> &input, size_t len = -1, uint32_t seed = 0)
        {
            return XXH32(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, seed);
        }

        template <typename CharT>
        XXH32_hash_t Get_XXH32(const basic_string_view<CharT> &input, size_t len = -1, uint32_t seed = 0)
        {
            return XXH32(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, seed);
        }

        /**
         * @brief 取文件_XXH32
         * @param filename 文件名
         * @param off 文件偏移
         * @param len 数据长度
         * @param seed 种子
         * @return hash值
         */
        static XXH32_hash_t GetFile_XXH32(const wchar_t *filename, int64_t off, uint64_t len, uint32_t seed = 0)
        {
            XXH32_hash_t hash = 0;
            FILE *file = NULL;
            file = _wfopen(filename, L"rb");
            if (file)
            {
                if (off > 0)
                    _fseeki64(file, off, SEEK_SET);
                XXH32_state_t *state = XXH32_createState();
                XXH32_reset(state, seed);
                unsigned char buff[65536];
                size_t rsize = 0;
                while ((rsize = fread(buff, 1, 65536, file)) > 0)
                {
                    if (rsize > len)
                    {
                        XXH32_update(state, buff, static_cast<size_t>(len));
                        break;
                    }
                    XXH32_update(state, buff, rsize);
                    len -= rsize;
                }
                hash = XXH32_digest(state);
                XXH32_freeState(state);
                fclose(file);
            }
            return hash;
        }

        /**
         * @brief 取资源_XXH32
         * @param resId 文件资源ID
         * @param off 文件偏移
         * @param len 数据长度
         * @param seed 种子
         * @return hash值
         */
        static XXH32_hash_t GetRes_XXH32(int32_t resId, int64_t off, uint64_t len, uint32_t seed = 0)
        {
            XXH32_hash_t hash = 0;
            if (resId == 0)
                return hash;
            HMODULE hModule = g_objVolApp.GetInstanceHandle();
            HRSRC hSrc = ::FindResourceW(hModule, MAKEINTRESOURCE(static_cast<WORD>(resId)), RT_RCDATA);
            if (hSrc == NULL)
                return hash;
            HGLOBAL resdata = ::LoadResource(hModule, hSrc);
            if (resdata == NULL)
                return hash;
            const uint8_t *buffer = reinterpret_cast<const uint8_t *>(::LockResource(resdata));
            size_t _size = ::SizeofResource(hModule, hSrc);
            XXH32_state_t *state = XXH32_createState();
            XXH32_reset(state, seed);
            if (off > 0 && buffer != nullptr)
            {
                size_t fileOff = static_cast<size_t>(off);
                buffer = (fileOff < _size) ? (buffer + fileOff) : (buffer + _size);
                _size = (fileOff < _size) ? _size - fileOff : 0;
            }
            XXH32_update(state, buffer, _size > static_cast<size_t>(len) ? static_cast<size_t>(len) : _size);
            hash = XXH32_digest(state);
            XXH32_freeState(state);
            return hash;
        }
    } // namespace hash

} // namespace piv

#endif // PIV_XXHASH_HPP
