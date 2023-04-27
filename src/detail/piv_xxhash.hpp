/*********************************************\
 * 火山视窗PIV模块 - xxHash                  *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
\*********************************************/

#ifndef PIV_XXHASH_HPP
#define PIV_XXHASH_HPP

#include "../piv_string.hpp"

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
            if (XXH_INLINE_XXH3_generateSecret(secret.GetPtr(), secretSize, customSeed.GetPtr(), static_cast<size_t>(customSeed.GetSize())) == XXH_OK)
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
            XXH_INLINE_XXH3_generateSecret_fromSeed(secret.GetPtr(), seed);
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
        T &Get_XXH128(const void *input, const size_t &len, const uint64_t &seed = 0, T &rethash = T{})
        {
            return piv::encoding::value_to_hex(XXH_INLINE_XXH3_128bits_withSeed(input, len, seed), rethash);
        }
        template <typename T>
        T &Get_XXH128(const ptrdiff_t &input, const size_t &len, const uint64_t &seed = 0, T &rethash = T{})
        {
            return Get_XXH128(reinterpret_cast<const void *>(input), len, seed, rethash);
        }
        template <typename T>
        T &Get_XXH128(const CVolMem &input, const size_t &len = (size_t)-1, const uint64_t &seed = 0, T &rethash = T{})
        {
            return Get_XXH128(input.GetPtr(), len == (size_t)-1 ? static_cast<size_t>(input.GetSize()) : len, seed, rethash);
        }
        template <typename T>
        T &Get_XXH128(const CVolString &input, const size_t &len = (size_t)-1, const uint64_t &seed = 0, T &rethash = T{})
        {
            return Get_XXH128(input.GetText(), len == (size_t)-1 ? input.GetLength() * 2 : len, seed, rethash);
        }
        template <typename T, typename CharT>
        T &Get_XXH128(const std::basic_string<CharT> &input, const size_t &len = (size_t)-1, const uint64_t &seed = 0, T &rethash = T{})
        {
            return Get_XXH128(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, seed, rethash);
        }
        template <typename T, typename CharT>
        T &Get_XXH128(const basic_string_view<CharT> &input, const size_t &len = (size_t)-1, const uint64_t &seed = 0, T &rethash = T{})
        {
            return Get_XXH128(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, seed, rethash);
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
        T &Get_XXH128_withSecret(const void *input, const size_t &len, const CVolMem &secret = CVolMem{}, const uint64_t &seed = 0, T &rethash = T{})
        {
            if (secret.GetSize() > 0)
                return piv::encoding::value_to_hex(XXH_INLINE_XXH3_128bits_withSecretandSeed(input, len, secret.GetPtr(), static_cast<size_t>(secret.GetSize()), seed), rethash);
            else
                return piv::encoding::value_to_hex(XXH_INLINE_XXH3_128bits_withSeed(input, len, seed), rethash);
        }
        template <typename T>
        T &Get_XXH128_withSecret(const ptrdiff_t &input, const size_t &len, const CVolMem &secret = CVolMem{}, const uint64_t &seed = 0, T &rethash = T{})
        {
            return Get_XXH128_withSecret(reinterpret_cast<const void *>(input), len, secret, seed, rethash);
        }
        template <typename T>
        T &Get_XXH128_withSecret(const CVolMem &input, const size_t &len = (size_t)-1, const CVolMem &secret = CVolMem{}, const uint64_t &seed = 0, T &rethash = T{})
        {
            return Get_XXH128_withSecret(input.GetPtr(), len == (size_t)-1 ? static_cast<size_t>(input.GetSize()) : len, secret, seed, rethash);
        }
        template <typename T>
        T &Get_XXH128_withSecret(const CVolString &input, const size_t &len = (size_t)-1, const CVolMem &secret = CVolMem{}, const uint64_t &seed = 0, T &rethash = T{})
        {
            return Get_XXH128_withSecret(input.GetText(), len == (size_t)-1 ? input.GetLength() * 2 : len, secret, seed, rethash);
        }
        template <typename T, typename CharT>
        T &Get_XXH128_withSecret(const std::basic_string<CharT> &input, const size_t &len = (size_t)-1, const CVolMem &secret = CVolMem{}, const uint64_t &seed = 0, T &rethash = T{})
        {
            return Get_XXH128_withSecret(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, secret, seed, rethash);
        }
        template <typename T, typename CharT>
        T &Get_XXH128_withSecret(const basic_string_view<CharT> &input, const size_t &len = (size_t)-1, const CVolMem &secret = CVolMem{}, const uint64_t &seed = 0, T &rethash = T{})
        {
            return Get_XXH128_withSecret(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, secret, seed, rethash);
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
        T &GetFile_XXH128(const wchar_t *filename, int64_t off, uint64_t len, const CVolMem &secret = CVolMem{}, const uint64_t &seed = 0, T &rethash = T{})
        {
            XXH128_hash_t hash = {0};
            FILE *file = NULL;
            file = _wfopen(filename, L"rb");
            if (file)
            {
                if (off > 0)
                    _fseeki64(file, off, SEEK_SET);
                XXH3_state_t *state = XXH_INLINE_XXH3_createState();
                if (secret.GetSize() >= 136)
                    XXH_INLINE_XXH3_128bits_reset_withSecretandSeed(state, secret.GetPtr(), static_cast<size_t>(secret.GetSize()), seed);
                else
                    XXH_INLINE_XXH3_128bits_reset_withSeed(state, seed);
                unsigned char buff[65536];
                size_t rsize = 0;
                while ((rsize = fread(buff, 1, 65536, file)) > 0)
                {
                    if (rsize > len)
                    {
                        XXH_INLINE_XXH3_128bits_update(state, buff, static_cast<size_t>(len));
                        break;
                    }
                    XXH_INLINE_XXH3_128bits_update(state, buff, rsize);
                    len -= rsize;
                }
                hash = XXH_INLINE_XXH3_128bits_digest(state);
                XXH_INLINE_XXH3_freeState(state);
                fclose(file);
            }
            return piv::encoding::value_to_hex(hash, rethash);
        }

        /**
         * @brief 取数据XXH3(64位)
         * @param input 所欲摘要的数据
         * @param len 数据字节长度
         * @param seed 种子
         * @return hash值
         */
        static XXH64_hash_t Get_XXH3(const void *input, const size_t &len, const uint64_t &seed = 0)
        {
            return XXH_INLINE_XXH3_64bits_withSeed(input, len, seed);
        }
        static XXH64_hash_t Get_XXH3(const ptrdiff_t &input, const size_t &len, const uint64_t &seed = 0)
        {
            return XXH_INLINE_XXH3_64bits_withSeed(reinterpret_cast<const void *>(input), len, seed);
        }
        static XXH64_hash_t Get_XXH3(const CVolMem &input, const size_t &len = (size_t)-1, const uint64_t &seed = 0)
        {
            return XXH_INLINE_XXH3_64bits_withSeed(input.GetPtr(), len == (size_t)-1 ? static_cast<size_t>(input.GetSize()) : len, seed);
        }
        static XXH64_hash_t Get_XXH3(const CVolString &input, const size_t &len = (size_t)-1, const uint64_t &seed = 0)
        {
            return XXH_INLINE_XXH3_64bits_withSeed(input.GetText(), len == (size_t)-1 ? input.GetLength() * 2 : len, seed);
        }
        template <typename CharT>
        static XXH64_hash_t Get_XXH3(const std::basic_string<CharT> &input, const size_t &len = -1, const uint64_t &seed = 0)
        {
            return XXH_INLINE_XXH3_64bits_withSeed(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, seed);
        }
        template <typename CharT>
        XXH64_hash_t Get_XXH3(const basic_string_view<CharT> &input, const size_t &len = -1, const uint64_t &seed = 0)
        {
            return XXH_INLINE_XXH3_64bits_withSeed(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, seed);
        }

        /**
         * @brief 取数据HMAC_XXH3(64位)
         * @param input 所欲摘要的数据
         * @param len 数据字节长度
         * @param secret 密码
         * @param seed 种子
         * @return hash值
         */
        static XXH64_hash_t Get_XXH3_withSecret(const void *input, const size_t &len, const CVolMem &secret = CVolMem{}, const uint64_t &seed = 0)
        {
            if (secret.GetSize() > 0)
                return XXH_INLINE_XXH3_64bits_withSecretandSeed(input, len, secret.GetPtr(), static_cast<size_t>(secret.GetSize()), seed);
            else
                return XXH_INLINE_XXH3_64bits_withSeed(input, len, seed);
        }
        static XXH64_hash_t Get_XXH3_withSecret(const ptrdiff_t &input, const size_t &len = (size_t)-1, const CVolMem &secret = CVolMem{}, const uint64_t &seed = 0)
        {
            return Get_XXH3_withSecret(reinterpret_cast<const void *>(input), len, secret, seed);
        }
        static XXH64_hash_t Get_XXH3_withSecret(const CVolMem &input, const size_t &len = (size_t)-1, const CVolMem &secret = CVolMem{}, const uint64_t &seed = 0)
        {
            return Get_XXH3_withSecret(input.GetPtr(), len == (size_t)-1 ? static_cast<size_t>(input.GetSize()) : len, secret, seed);
        }
        static XXH64_hash_t Get_XXH3_withSecret(const CVolString &input, const size_t &len = (size_t)-1, const CVolMem &secret = CVolMem{}, const uint64_t &seed = 0)
        {
            return Get_XXH3_withSecret(input.GetText(), len == (size_t)-1 ? input.GetLength() * 2 : len, secret, seed);
        }
        template <typename CharT>
        static XXH64_hash_t Get_XXH3_withSecret(const std::basic_string<CharT> &input, const size_t &len = (size_t)-1, const CVolMem &secret = CVolMem{}, const uint64_t &seed = 0)
        {
            return Get_XXH3_withSecret(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, secret, seed);
        }
        template <typename CharT>
        XXH64_hash_t Get_XXH3_withSecret(const basic_string_view<CharT> &input, const size_t &len = (size_t)-1, const CVolMem &secret = CVolMem{}, const uint64_t &seed = 0)
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
        static XXH64_hash_t GetFile_XXH3(const wchar_t *filename, int64_t off, uint64_t len, const CVolMem &secret = CVolMem{}, const uint64_t &seed = 0)
        {
            XXH64_hash_t hash = 0;
            FILE *file = NULL;
            file = _wfopen(filename, L"rb");
            if (file)
            {
                if (off > 0)
                    _fseeki64(file, off, SEEK_SET);
                XXH3_state_t *state = XXH_INLINE_XXH3_createState();
                if (secret.GetSize() >= 136)
                    XXH_INLINE_XXH3_64bits_reset_withSecretandSeed(state, secret.GetPtr(), static_cast<size_t>(secret.GetSize()), seed);
                else
                    XXH_INLINE_XXH3_64bits_reset_withSeed(state, seed);
                unsigned char buff[65536];
                size_t rsize = 0;
                while ((rsize = fread(buff, 1, 65536, file)) > 0)
                {
                    if (rsize > len)
                    {
                        XXH_INLINE_XXH3_64bits_update(state, buff, static_cast<size_t>(len));
                        break;
                    }
                    XXH_INLINE_XXH3_64bits_update(state, buff, rsize);
                    len -= rsize;
                }
                hash = XXH_INLINE_XXH3_64bits_digest(state);
                XXH_INLINE_XXH3_freeState(state);
                fclose(file);
            }
            return hash;
        }

        /**
         * @brief 取数据XXH64
         * @param input 所欲摘要的数据
         * @param len 数据字节长度
         * @param seed 种子
         * @return hash值
         */
        static XXH64_hash_t Get_XXH64(const void *input, const size_t &len, const uint64_t &seed = 0)
        {
            return XXH_INLINE_XXH64(input, len, seed);
        }
        static XXH64_hash_t Get_XXH64(const ptrdiff_t &input, const size_t &len, const uint64_t &seed = 0)
        {
            return XXH_INLINE_XXH64(reinterpret_cast<const void *>(input), len, seed);
        }
        static XXH64_hash_t Get_XXH64(const CVolMem &input, const size_t &len = (size_t)-1, const uint64_t &seed = 0)
        {
            return XXH_INLINE_XXH64(input.GetPtr(), len == (size_t)-1 ? static_cast<size_t>(input.GetSize()) : len, seed);
        }
        static XXH64_hash_t Get_XXH64(const CVolString &input, const size_t &len = (size_t)-1, const uint64_t &seed = 0)
        {
            return XXH_INLINE_XXH64(input.GetText(), len == (size_t)-1 ? input.GetLength() * 2 : len, seed);
        }
        template <typename CharT>
        static XXH64_hash_t Get_XXH64(const std::basic_string<CharT> &input, const size_t &len = -1, const uint64_t &seed = 0)
        {
            return XXH_INLINE_XXH64(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, seed);
        }
        template <typename CharT>
        XXH64_hash_t Get_XXH64(const basic_string_view<CharT> &input, const size_t &len = -1, const uint64_t &seed = 0)
        {
            return XXH_INLINE_XXH64(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, seed);
        }

        /**
         * @brief 取文件_XXH64
         * @param filename 文件名
         * @param off 文件偏移
         * @param len 数据长度
         * @param seed 种子
         * @return hash值
         */
        static XXH64_hash_t GetFile_XXH64(const wchar_t *filename, int64_t off, uint64_t len, const uint64_t &seed = 0)
        {
            XXH64_hash_t hash = 0;
            FILE *file = NULL;
            file = _wfopen(filename, L"rb");
            if (file)
            {
                if (off > 0)
                    _fseeki64(file, off, SEEK_SET);
                XXH64_state_t *state = XXH_INLINE_XXH64_createState();
                XXH_INLINE_XXH64_reset(state, seed);
                unsigned char buff[65536];
                size_t rsize = 0;
                while ((rsize = fread(buff, 1, 65536, file)) > 0)
                {
                    if (rsize > len)
                    {
                        XXH_INLINE_XXH64_update(state, buff, static_cast<size_t>(len));
                        break;
                    }
                    XXH_INLINE_XXH64_update(state, buff, rsize);
                    len -= rsize;
                }
                hash = XXH_INLINE_XXH64_digest(state);
                XXH_INLINE_XXH64_freeState(state);
                fclose(file);
            }
            return hash;
        }

        /**
         * @brief 取数据XXH32
         * @param input 所欲摘要的数据
         * @param len 数据字节长度
         * @param seed 种子
         * @return hash值
         */
        static XXH32_hash_t Get_XXH32(const void *input, const size_t &len, const uint32_t &seed = 0)
        {
            return XXH_INLINE_XXH32(input, len, seed);
        }
        static XXH32_hash_t Get_XXH32(const ptrdiff_t &input, const size_t &len, const uint32_t &seed = 0)
        {
            return XXH_INLINE_XXH32(reinterpret_cast<const void *>(input), len, seed);
        }
        static XXH32_hash_t Get_XXH32(const CVolMem &input, const size_t &len = (size_t)-1, const uint32_t &seed = 0)
        {
            return XXH_INLINE_XXH32(input.GetPtr(), len == (size_t)-1 ? static_cast<size_t>(input.GetSize()) : len, seed);
        }
        static XXH32_hash_t Get_XXH32(const CVolString &input, const size_t &len = (size_t)-1, const uint32_t &seed = 0)
        {
            return XXH_INLINE_XXH32(input.GetText(), len == (size_t)-1 ? input.GetLength() * 2 : len, seed);
        }
        template <typename CharT>
        static XXH32_hash_t Get_XXH32(const std::basic_string<CharT> &input, const size_t &len = -1, const uint32_t &seed = 0)
        {
            return XXH_INLINE_XXH32(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, seed);
        }
        template <typename CharT>
        XXH32_hash_t Get_XXH32(const basic_string_view<CharT> &input, const size_t &len = -1, const uint32_t &seed = 0)
        {
            return XXH_INLINE_XXH32(input.data(), len == (size_t)-1 ? input.size() * sizeof(CharT) : len, seed);
        }

        /**
         * @brief 取文件_XXH32
         * @param filename 文件名
         * @param off 文件偏移
         * @param len 数据长度
         * @param seed 种子
         * @return hash值
         */
        static XXH32_hash_t GetFile_XXH32(const wchar_t *filename, int64_t off, uint64_t len, const uint32_t &seed = 0)
        {
            XXH32_hash_t hash = 0;
            FILE *file = NULL;
            file = _wfopen(filename, L"rb");
            if (file)
            {
                if (off > 0)
                    _fseeki64(file, off, SEEK_SET);
                XXH32_state_t *state = XXH_INLINE_XXH32_createState();
                XXH_INLINE_XXH32_reset(state, seed);
                unsigned char buff[65536];
                size_t rsize = 0;
                while ((rsize = fread(buff, 1, 65536, file)) > 0)
                {
                    if (rsize > len)
                    {
                        XXH_INLINE_XXH32_update(state, buff, static_cast<size_t>(len));
                        break;
                    }
                    XXH_INLINE_XXH32_update(state, buff, rsize);
                    len -= rsize;
                }
                hash = XXH_INLINE_XXH32_digest(state);
                XXH_INLINE_XXH32_freeState(state);
                fclose(file);
            }
            return hash;
        }
    } // namespace hash

} // namespace piv

#endif // PIV_XXHASH_HPP
