/*********************************************\
 * 火山视窗PIV模块 - MD5校验类               *
 * 作者: Xelloss                             *
 * 网站: https://piv.ink                     *
 * 邮箱: xelloss@vip.qq.com                  *
 * 版本: 2023/02/23                          *
\*********************************************/

#ifndef PIV_MD5_HPP
#define PIV_MD5_HPP

// 包含火山视窗基本类,它在火山里的包含顺序比较靠前(全局-110)
#ifndef __VOL_BASE_H__
#include <sys/base/libs/win_base/vol_base.h>
#endif
#include "piv_string.hpp"

namespace piv
{
    class MD5Checksum
    {
    public:
#define PIV_MD5_FF(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define PIV_MD5_GG(x, y, z) ((y) ^ ((z) & ((x) ^ (y))))
#define PIV_MD5_HH(x, y, z) ((x) ^ (y) ^ (z))
#define PIV_MD5_II(x, y, z) ((y) ^ ((x) | ~(z)))
#define PIV_MD5_STEP(f, a, b, c, d, x, t, s)                 \
    (a) += f((b), (c), (d)) + (x) + (t);                     \
    (a) = (((a) << (s)) | (((a)&0xffffffff) >> (32 - (s)))); \
    (a) += (b);

#if defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#define PIV_MD5_SET(n) \
    (*(uint32_t *)&ptr[(n)*4])
#define PIV_MD5_GET(n) \
    PIV_MD5_SET(n)
#else
#define PIV_MD5_SET(n)                      \
    (ctx->block[(n)] =                      \
         (uint32_t)ptr[(n)*4] |             \
         ((uint32_t)ptr[(n)*4 + 1] << 8) |  \
         ((uint32_t)ptr[(n)*4 + 2] << 16) | \
         ((uint32_t)ptr[(n)*4 + 3] << 24))
#define PIV_MD5_GET(n) \
    (ctx->block[(n)])
#endif
        struct MD5_CTX
        {
            uint32_t lo, hi;
            uint32_t a, b, c, d;
            unsigned char buffer[64];
            uint32_t block[16];
        };

    private:
        MD5_CTX ctx;

        static const void *body(MD5_CTX *ctx, const void *data, uint32_t size)
        {
            const unsigned char *ptr;
            uint32_t a, b, c, d;
            uint32_t saved_a, saved_b, saved_c, saved_d;

            ptr = (const unsigned char *)data;

            a = ctx->a;
            b = ctx->b;
            c = ctx->c;
            d = ctx->d;

            do
            {
                saved_a = a;
                saved_b = b;
                saved_c = c;
                saved_d = d;

                PIV_MD5_STEP(PIV_MD5_FF, a, b, c, d, PIV_MD5_SET(0), 0xd76aa478, 7)
                PIV_MD5_STEP(PIV_MD5_FF, d, a, b, c, PIV_MD5_SET(1), 0xe8c7b756, 12)
                PIV_MD5_STEP(PIV_MD5_FF, c, d, a, b, PIV_MD5_SET(2), 0x242070db, 17)
                PIV_MD5_STEP(PIV_MD5_FF, b, c, d, a, PIV_MD5_SET(3), 0xc1bdceee, 22)
                PIV_MD5_STEP(PIV_MD5_FF, a, b, c, d, PIV_MD5_SET(4), 0xf57c0faf, 7)
                PIV_MD5_STEP(PIV_MD5_FF, d, a, b, c, PIV_MD5_SET(5), 0x4787c62a, 12)
                PIV_MD5_STEP(PIV_MD5_FF, c, d, a, b, PIV_MD5_SET(6), 0xa8304613, 17)
                PIV_MD5_STEP(PIV_MD5_FF, b, c, d, a, PIV_MD5_SET(7), 0xfd469501, 22)
                PIV_MD5_STEP(PIV_MD5_FF, a, b, c, d, PIV_MD5_SET(8), 0x698098d8, 7)
                PIV_MD5_STEP(PIV_MD5_FF, d, a, b, c, PIV_MD5_SET(9), 0x8b44f7af, 12)
                PIV_MD5_STEP(PIV_MD5_FF, c, d, a, b, PIV_MD5_SET(10), 0xffff5bb1, 17)
                PIV_MD5_STEP(PIV_MD5_FF, b, c, d, a, PIV_MD5_SET(11), 0x895cd7be, 22)
                PIV_MD5_STEP(PIV_MD5_FF, a, b, c, d, PIV_MD5_SET(12), 0x6b901122, 7)
                PIV_MD5_STEP(PIV_MD5_FF, d, a, b, c, PIV_MD5_SET(13), 0xfd987193, 12)
                PIV_MD5_STEP(PIV_MD5_FF, c, d, a, b, PIV_MD5_SET(14), 0xa679438e, 17)
                PIV_MD5_STEP(PIV_MD5_FF, b, c, d, a, PIV_MD5_SET(15), 0x49b40821, 22)
                PIV_MD5_STEP(PIV_MD5_GG, a, b, c, d, PIV_MD5_GET(1), 0xf61e2562, 5)
                PIV_MD5_STEP(PIV_MD5_GG, d, a, b, c, PIV_MD5_GET(6), 0xc040b340, 9)
                PIV_MD5_STEP(PIV_MD5_GG, c, d, a, b, PIV_MD5_GET(11), 0x265e5a51, 14)
                PIV_MD5_STEP(PIV_MD5_GG, b, c, d, a, PIV_MD5_GET(0), 0xe9b6c7aa, 20)
                PIV_MD5_STEP(PIV_MD5_GG, a, b, c, d, PIV_MD5_GET(5), 0xd62f105d, 5)
                PIV_MD5_STEP(PIV_MD5_GG, d, a, b, c, PIV_MD5_GET(10), 0x02441453, 9)
                PIV_MD5_STEP(PIV_MD5_GG, c, d, a, b, PIV_MD5_GET(15), 0xd8a1e681, 14)
                PIV_MD5_STEP(PIV_MD5_GG, b, c, d, a, PIV_MD5_GET(4), 0xe7d3fbc8, 20)
                PIV_MD5_STEP(PIV_MD5_GG, a, b, c, d, PIV_MD5_GET(9), 0x21e1cde6, 5)
                PIV_MD5_STEP(PIV_MD5_GG, d, a, b, c, PIV_MD5_GET(14), 0xc33707d6, 9)
                PIV_MD5_STEP(PIV_MD5_GG, c, d, a, b, PIV_MD5_GET(3), 0xf4d50d87, 14)
                PIV_MD5_STEP(PIV_MD5_GG, b, c, d, a, PIV_MD5_GET(8), 0x455a14ed, 20)
                PIV_MD5_STEP(PIV_MD5_GG, a, b, c, d, PIV_MD5_GET(13), 0xa9e3e905, 5)
                PIV_MD5_STEP(PIV_MD5_GG, d, a, b, c, PIV_MD5_GET(2), 0xfcefa3f8, 9)
                PIV_MD5_STEP(PIV_MD5_GG, c, d, a, b, PIV_MD5_GET(7), 0x676f02d9, 14)
                PIV_MD5_STEP(PIV_MD5_GG, b, c, d, a, PIV_MD5_GET(12), 0x8d2a4c8a, 20)
                PIV_MD5_STEP(PIV_MD5_HH, a, b, c, d, PIV_MD5_GET(5), 0xfffa3942, 4)
                PIV_MD5_STEP(PIV_MD5_HH, d, a, b, c, PIV_MD5_GET(8), 0x8771f681, 11)
                PIV_MD5_STEP(PIV_MD5_HH, c, d, a, b, PIV_MD5_GET(11), 0x6d9d6122, 16)
                PIV_MD5_STEP(PIV_MD5_HH, b, c, d, a, PIV_MD5_GET(14), 0xfde5380c, 23)
                PIV_MD5_STEP(PIV_MD5_HH, a, b, c, d, PIV_MD5_GET(1), 0xa4beea44, 4)
                PIV_MD5_STEP(PIV_MD5_HH, d, a, b, c, PIV_MD5_GET(4), 0x4bdecfa9, 11)
                PIV_MD5_STEP(PIV_MD5_HH, c, d, a, b, PIV_MD5_GET(7), 0xf6bb4b60, 16)
                PIV_MD5_STEP(PIV_MD5_HH, b, c, d, a, PIV_MD5_GET(10), 0xbebfbc70, 23)
                PIV_MD5_STEP(PIV_MD5_HH, a, b, c, d, PIV_MD5_GET(13), 0x289b7ec6, 4)
                PIV_MD5_STEP(PIV_MD5_HH, d, a, b, c, PIV_MD5_GET(0), 0xeaa127fa, 11)
                PIV_MD5_STEP(PIV_MD5_HH, c, d, a, b, PIV_MD5_GET(3), 0xd4ef3085, 16)
                PIV_MD5_STEP(PIV_MD5_HH, b, c, d, a, PIV_MD5_GET(6), 0x04881d05, 23)
                PIV_MD5_STEP(PIV_MD5_HH, a, b, c, d, PIV_MD5_GET(9), 0xd9d4d039, 4)
                PIV_MD5_STEP(PIV_MD5_HH, d, a, b, c, PIV_MD5_GET(12), 0xe6db99e5, 11)
                PIV_MD5_STEP(PIV_MD5_HH, c, d, a, b, PIV_MD5_GET(15), 0x1fa27cf8, 16)
                PIV_MD5_STEP(PIV_MD5_HH, b, c, d, a, PIV_MD5_GET(2), 0xc4ac5665, 23)
                PIV_MD5_STEP(PIV_MD5_II, a, b, c, d, PIV_MD5_GET(0), 0xf4292244, 6)
                PIV_MD5_STEP(PIV_MD5_II, d, a, b, c, PIV_MD5_GET(7), 0x432aff97, 10)
                PIV_MD5_STEP(PIV_MD5_II, c, d, a, b, PIV_MD5_GET(14), 0xab9423a7, 15)
                PIV_MD5_STEP(PIV_MD5_II, b, c, d, a, PIV_MD5_GET(5), 0xfc93a039, 21)
                PIV_MD5_STEP(PIV_MD5_II, a, b, c, d, PIV_MD5_GET(12), 0x655b59c3, 6)
                PIV_MD5_STEP(PIV_MD5_II, d, a, b, c, PIV_MD5_GET(3), 0x8f0ccc92, 10)
                PIV_MD5_STEP(PIV_MD5_II, c, d, a, b, PIV_MD5_GET(10), 0xffeff47d, 15)
                PIV_MD5_STEP(PIV_MD5_II, b, c, d, a, PIV_MD5_GET(1), 0x85845dd1, 21)
                PIV_MD5_STEP(PIV_MD5_II, a, b, c, d, PIV_MD5_GET(8), 0x6fa87e4f, 6)
                PIV_MD5_STEP(PIV_MD5_II, d, a, b, c, PIV_MD5_GET(15), 0xfe2ce6e0, 10)
                PIV_MD5_STEP(PIV_MD5_II, c, d, a, b, PIV_MD5_GET(6), 0xa3014314, 15)
                PIV_MD5_STEP(PIV_MD5_II, b, c, d, a, PIV_MD5_GET(13), 0x4e0811a1, 21)
                PIV_MD5_STEP(PIV_MD5_II, a, b, c, d, PIV_MD5_GET(4), 0xf7537e82, 6)
                PIV_MD5_STEP(PIV_MD5_II, d, a, b, c, PIV_MD5_GET(11), 0xbd3af235, 10)
                PIV_MD5_STEP(PIV_MD5_II, c, d, a, b, PIV_MD5_GET(2), 0x2ad7d2bb, 15)
                PIV_MD5_STEP(PIV_MD5_II, b, c, d, a, PIV_MD5_GET(9), 0xeb86d391, 21)

                a += saved_a;
                b += saved_b;
                c += saved_c;
                d += saved_d;

                ptr += 64;
            } while (size -= 64);

            ctx->a = a;
            ctx->b = b;
            ctx->c = c;
            ctx->d = d;

            return ptr;
        }

        static unsigned char hb2hex(const unsigned char &hb, const bool &upper)
        {
            if (upper)
                return hb < 10 ? '0' + hb : hb - 10 + 'A';
            else
                return hb < 10 ? '0' + hb : hb - 10 + 'a';
        }

    public:
        MD5Checksum()
        {
            Init();
        }
        ~MD5Checksum() {}

        void Init()
        {
            ctx.a = 0x67452301;
            ctx.b = 0xefcdab89;
            ctx.c = 0x98badcfe;
            ctx.d = 0x10325476;

            ctx.lo = 0;
            ctx.hi = 0;
        }

        void Update(const void *data, const size_t &len)
        {
            uint32_t size = static_cast<uint32_t>(len);
            uint32_t saved_lo;
            unsigned long used, free;

            saved_lo = ctx.lo;
            if ((ctx.lo = (saved_lo + size) & 0x1fffffff) < saved_lo)
                ctx.hi++;
            ctx.hi += size >> 29;
            used = saved_lo & 0x3f;

            if (used)
            {
                free = 64 - used;
                if (size < free)
                {
                    memcpy(&ctx.buffer[used], data, size);
                    return;
                }

                memcpy(&ctx.buffer[used], data, free);
                data = (unsigned char *)data + free;
                size -= free;
                body(&ctx, ctx.buffer, 64);
            }

            if (size >= 64)
            {
                data = body(&ctx, data, size & ~(uint32_t)0x3f);
                size &= 0x3f;
            }

            memcpy(ctx.buffer, data, size);
        }

        void Final(unsigned char *result)
        {
            uint32_t used, free;
            used = ctx.lo & 0x3f;
            ctx.buffer[used++] = 0x80;
            free = 64 - used;

            if (free < 8)
            {
                memset(&ctx.buffer[used], 0, free);
                body(&ctx, ctx.buffer, 64);
                used = 0;
                free = 64;
            }

            memset(&ctx.buffer[used], 0, free - 8);

            ctx.lo <<= 3;
            ctx.buffer[56] = ctx.lo;
            ctx.buffer[57] = ctx.lo >> 8;
            ctx.buffer[58] = ctx.lo >> 16;
            ctx.buffer[59] = ctx.lo >> 24;
            ctx.buffer[60] = ctx.hi;
            ctx.buffer[61] = ctx.hi >> 8;
            ctx.buffer[62] = ctx.hi >> 16;
            ctx.buffer[63] = ctx.hi >> 24;
            body(&ctx, ctx.buffer, 64);
            result[0] = ctx.a;
            result[1] = ctx.a >> 8;
            result[2] = ctx.a >> 16;
            result[3] = ctx.a >> 24;
            result[4] = ctx.b;
            result[5] = ctx.b >> 8;
            result[6] = ctx.b >> 16;
            result[7] = ctx.b >> 24;
            result[8] = ctx.c;
            result[9] = ctx.c >> 8;
            result[10] = ctx.c >> 16;
            result[11] = ctx.c >> 24;
            result[12] = ctx.d;
            result[13] = ctx.d >> 8;
            result[14] = ctx.d >> 16;
            result[15] = ctx.d >> 24;
            memset(&ctx, 0, sizeof(ctx));
        }

        CVolString &Final(CVolString &str = CVolString{}, const bool &upper = true)
        {
            unsigned char out[16];
            Final(out);
            str.Empty();
            for (size_t i = 0; i < 16; i++)
            {
                str.AddChar(hb2hex((out[i] >> 4) & 0xF, upper));
                str.AddChar(hb2hex(out[i] & 0xF, upper));
            }
            return str;
        }
        std::string &Final(std::string &str = std::string{}, const bool &upper = true)
        {
            unsigned char out[16];
            Final(out);
            str.clear();
            for (size_t i = 0; i < 16; i++)
            {
                str.push_back(hb2hex((out[i] >> 4) & 0xF, upper));
                str.push_back(hb2hex(out[i] & 0xF, upper));
            }
            return str;
        }
        std::wstring &Final(std::wstring &str = std::wstring{}, const bool &upper = true)
        {
            unsigned char out[16];
            Final(out);
            str.clear();
            for (size_t i = 0; i < 16; i++)
            {
                str.push_back(hb2hex((out[i] >> 4) & 0xF, upper));
                str.push_back(hb2hex(out[i] & 0xF, upper));
            }
            return str;
        }
    };

    static CVolString GetFileMd5(FILE *file, const bool &upper)
    {
        MD5Checksum md5;

        char buff[BUFSIZ];
        size_t len = 0;
        while ((len = fread(buff, sizeof(char), BUFSIZ, file)) > 0)
        {
            md5.Update(buff, len);
        }
        return md5.Final(CVolString{}, upper);
    }

    static CVolString GetFileMd5(const wchar_t *filename, const bool &upper)
    {
        FILE *file = NULL;
        file = _wfopen(filename, L"rb");
        CVolString res;
        if (file == NULL)
            return res;
        res = GetFileMd5(file, upper);
        fclose(file);
        return res;
    }

    static CVolString GetDataMd5(const void *data, size_t len, const bool &upper)
    {
        MD5Checksum md5;
        md5.Update(data, len);
        return md5.Final(CVolString{}, upper);
    }

    static CVolString GetDataMd5(CVolMem &data, const bool &upper)
    {
        return GetDataMd5(data.GetPtr(), static_cast<size_t>(data.GetSize()), upper);
    }

    /* Generate shorter md5sum by something like base62 instead of base16 or base10. 0~61 are represented by 0-9a-zA-Z */
    static CVolString GetDataMd5sum6(const void *data, size_t len)
    {
        static const char *tbl = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
        CVolString res;
        unsigned char out[16];
        MD5Checksum md5;
        md5.Update(data, len);
        md5.Final(out);
        for (size_t i = 0; i < 6; ++i)
        {
            res.AddChar(tbl[out[i] % 62]);
        }
        return res;
    }

    static CVolString GetDataMd5sum6(CVolMem &data)
    {
        return GetDataMd5sum6(data.GetPtr(), static_cast<size_t>(data.GetSize()));
    }

    template <typename CharT>
    std::basic_string<CharT> &GetStringMd5(const std::basic_string<CharT> &data, const bool &upper = true, std::basic_string<CharT> &ret = std::basic_string<CharT>{})
    {
        MD5Checksum md5;
        md5.Update(data.data(), data.size() * sizeof(CharT));
        return md5.Final(ret, upper);
    }
    template <typename CharT>
    std::basic_string<CharT> &GetStringMd5(const CharT *data, const size_t &len, const bool &upper = true, std::basic_string<CharT> &ret = std::basic_string<CharT>{})
    {
        MD5Checksum md5;
        md5.Update(data, len);
        return md5.Final(ret, upper);
    }
    template <typename CharT>
    std::basic_string<CharT> &GetStringMd5(const piv::basic_string_view<CharT> &data, const bool &upper = true, std::basic_string<CharT> &ret = std::basic_string<CharT>{})
    {
        MD5Checksum md5;
        md5.Update(data.data(), data.size() * sizeof(CharT));
        return md5.Final(ret, upper);
    }

} // namespace piv
#endif // PIV_MD5_HPP
