// Base64/85/91 en/decoders, still C-String, TSV, XML and JSON friendly.
// - rlyeh 2011..2016, zlib/libpng licensed.

// Some notes {
// - [x] Base64: canonical implementation.
// - [x] Base85: 17% to 08% smaller than Base64, still C-String friendly. Custom Z85 variant.
// - [x] Base91: 19% to 10% smaller than Base64, still JSON, XML and TSV friendly. Custom basE91 variant.
// - [x] Encoded data can be "quoted", splitted with tabs, spaces, linefeeds and carriages.
// }

// Extra licensing {
// - Base64 is based on code by René Nyffenegger (zlib/libpng licensed)
// - Base91 is based on code by Joachim Henke {
//     Copyright (c) 2000-2006 Joachim Henke
//     http://base91.sourceforge.net/ (v0.6.0)
//     Redistribution and use in source and binary forms, with or without
//     modification, are permitted provided that the following conditions are met:
//      - Redistributions of source code must retain the above copyright notice,
//        this list of conditions and the following disclaimer.
//      - Redistributions in binary form must reproduce the above copyright notice,
//        this list of conditions and the following disclaimer in the documentation
//        and/or other materials provided with the distribution.
//      - Neither the name of Joachim Henke nor the names of his contributors may
//        be used to endorse or promote products derived from this software without
//        specific prior written permission.
//     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//     AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//     IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//     ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
//     LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//     CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//     SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//     INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//     CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//     ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//     POSSIBILITY OF SUCH DAMAGE.
// } }

#pragma once

namespace {

    /* Public API */

    template<unsigned N> struct base {
        static CVolString encode( const unsigned char * bytes_to_encode, size_t length);
        static CVolString encode( CVolMem &data);
        static CVolString encode( bool Padding, CVolMem &data );
        static CVolMem decode( const wchar_t * text, size_t length );
        static CVolMem decode( bool Padding, const wchar_t * text, size_t length );
        static CVolMem decode( bool Padding, CVolString &text );
        static CVolMem decode( CVolString &text );
    };

    typedef base<85> base85;
    typedef base<91> base91;

    /* API details */

    template<>
    CVolString base<91>::encode( const unsigned char * bytes_to_encode, size_t length ) {

        const unsigned char enctab[91] = {
            /* // Henke's original
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', //00..12
            'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', //13..25
            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', //26..38
            'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', //39..51
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '!', '#', '$', //52..64
            '%', '&', '(', ')', '*', '+', ',', '.', '/', ':', ';', '<', '=', //65..77
            '>', '?', '@', '[', ']', '^', '_', '`', '{', '|', '}', '~', '"'  //78..90 */
            // // rlyeh's modification
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', //00..12
            'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', //13..25
            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', //26..38
            'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', //39..51
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '!', '#', '$', //52..64
            '%', '&', '(', ')', '*', '+', ',', '.', '/', ':', ';', '-', '=', //65..77
            '\\','?', '@', '[', ']', '^', '_', '`', '{', '|', '}', '~', '\'' //78..90
        };

        CVolString out;
        out.SetNumAlignChars (length * 6 / 4);
        const unsigned char *ib = (unsigned char *) bytes_to_encode;
        unsigned long queue = 0;
        unsigned int nbits = 0;

        for( size_t len = length; len--; ) {
            queue |= *ib++ << nbits;
            nbits += 8;
            if (nbits > 13) {   /* enough bits in queue */
                unsigned int val = queue & 8191;

                if (val > 88) {
                    queue >>= 13;
                    nbits -= 13;
                } else {    /* we can take 14 bits */
                    val = queue & 16383;
                    queue >>= 14;
                    nbits -= 14;
                }
                out.AddChar( enctab[val % 91] );
                out.AddChar( enctab[val / 91] );
            }
        }

        /* process remaining bits from bit queue; write up to 2 bytes */
        if (nbits) {
            out.AddChar( enctab[queue % 91] );
            if (nbits > 7 || queue > 90)
                out.AddChar( enctab[queue / 91] );
        }

        return out;
    }

    template<>
    CVolMem base<91>::decode( const wchar_t * text, size_t length ) {

        const unsigned char dectab[256] = {
            /* // Henke's original
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, //000..015
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, //016..031
            91, 62, 90, 63, 64, 65, 66, 91, 67, 68, 69, 70, 71, 91, 72, 73, //032..047
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 74, 75, 76, 77, 78, 79, //048..063
            80,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, //064..079
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 81, 91, 82, 83, 84, //080..095
            85, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, //096..111
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 86, 87, 88, 89, 91, //112..127
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, //128..143
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, //144..159
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, //160..175
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, //176..191
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, //192..207
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, //208..223
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, //224..239
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91  //240..255 */
            // // rlyeh's modification
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, //000..015
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, //016..031
            91, 62, 91, 63, 64, 65, 66, 90, 67, 68, 69, 70, 71, 76, 72, 73, //032..047 // @34: ", @39: ', @45: -
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 74, 75, 91, 77, 91, 79, //048..063 // @60: <, @62: >
            80,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, //064..079
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 81, 78, 82, 83, 84, //080..095 // @92: slash
            85, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, //096..111
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 86, 87, 88, 89, 91, //112..127
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, //128..143
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, //144..159
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, //160..175
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, //176..191
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, //192..207
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, //208..223
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, //224..239
            91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91  //240..255
        };

        CVolMem out;
        out.SetMemAlignSize (length * 6 / 4);
        const wchar_t *ib = (wchar_t *)text;

        unsigned long queue = 0;
        unsigned int nbits = 0;
        int val = -1;

        for( size_t len = length; len--; ) {
            unsigned int d = dectab[*ib++];
            if (d == 91)
                continue;   /* ignore non-alphabet chars */
            if (val == -1)
                val = d;    /* start next value */
            else {
                val += d * 91;
                queue |= val << nbits;
                nbits += (val & 8191) > 88 ? 13 : 14;
                do {
                    out.AddU8Char( U8CHAR( queue ) );
                    queue >>= 8;
                    nbits -= 8;
                } while (nbits > 7);
                val = -1;   /* mark value complete */
            }
        }

        /* process remaining bits; write at most 1 byte */
        if (val != -1)
            out.AddU8Char( U8CHAR( queue | val << nbits ) );

        return out;
    }

    // base85 (z85): standard rfc size (multiples of 4/5)

    inline bool encode85( CVolString &out, const unsigned char *raw, size_t rawlen ) {
        if( rawlen % 4 ) {
            return false; // error: raw string size must be multiple of 4
        }
        // encode
        const char encoder[86] =
            "0123456789" "abcdefghij" "klmnopqrst" "uvwxyzABCD"             // 00..39
            "EFGHIJKLMN" "OPQRSTUVWX" "YZ.-:+=^!/" "*?&<>()[]{" "}@%$#";    // 40..84 // free chars: , ; _ ` | ~ \'
        out.SetNumAlignChars (rawlen * 5 / 4);
        wchar_t tmp[5];
        for( size_t o = 0; o < rawlen * 5 / 4; raw += 4, o += 5 ) {
            unsigned value = (raw[0] << 24) | (raw[1] << 16) | (raw[2] << 8) | raw[3];
            tmp[0] = encoder[ (value / 0x31C84B1) % 0x55 ];
            tmp[1] = encoder[   (value / 0x95EED) % 0x55 ];
            tmp[2] = encoder[    (value / 0x1C39) % 0x55 ];
            tmp[3] = encoder[      (value / 0x55) % 0x55 ];
            tmp[4] = encoder[               value % 0x55 ];
            out.AddText(tmp, 5);
        }
        return true;
    }

    inline bool decode85( CVolMem &out, const wchar_t *z85, size_t z85len ) {
        if( z85len % 5 ) {
            return false; // error: z85 string size must be multiple of 5
        }
        // decode
        const unsigned char decoder[128] = {
             0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x00..0x0F
             0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, // 0x10..0x1F
             0, 68,  0, 84, 83, 82, 72,  0, 75, 76, 70, 65,  0, 63, 62, 69, // 0x20..0x2F
             0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 64,  0, 73, 66, 74, 71, // 0x30..0x3F
            81, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, // 0x40..0x4F
            51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 77,  0, 78, 67,  0, // 0x50..0x5F
             0, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, // 0x60..0x6F
            25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 79,  0, 80,  0,  0, // 0x70..0x7F
        };
        out.SetMemAlignSize( z85len * 4 / 5 );
        for( size_t o = 0; o < z85len * 4 / 5; z85 += 5, o += 4 ) {
            unsigned value = decoder[z85[0]] * 0x31C84B1 + decoder[z85[1]] * 0x95EED +
                             decoder[z85[2]] *    0x1C39 + decoder[z85[3]] *    0x55 + decoder[z85[4]];
            out.AddU8Char( U8CHAR ((value >> 24) & 0xFF) );
            out.AddU8Char( U8CHAR ((value >> 16) & 0xFF) );
            out.AddU8Char( U8CHAR ((value >>  8) & 0xFF) );
            out.AddU8Char( U8CHAR ((value >>  0) & 0xFF) );
        }
        return true;
    }

    // base85 (z85): arbitrary size (this may lead up to four additional bytes)

    template<>
    CVolString base<85>::encode( bool Padding, CVolMem &raw ) {
        // create padding, if needed
        CVolString out;
        if (Padding) {
            CVolMem pad4 = raw;
            pad4.AddU8Char ('\1');
            pad4.AddManyU8Chars('\0', 4 - (raw.GetSize() + 1) % 4);
            return encode85( out, (const unsigned char *)raw.GetPtr(), (size_t)raw.GetSize() ) ? out : _CT ("");
        } else {
            return encode85( out, (const unsigned char *)raw.GetPtr(), (size_t)raw.GetSize() ) ? out : _CT ("");
        }
    }

    template<>
    CVolMem base<85>::decode( bool Padding, const wchar_t *z85, size_t z85len ) {
        CVolMem out;
        if( !decode85( out, z85, z85len ) ) {
            return CVolMem ();            
        } else {
            if (Padding) {
                // remove padding, if needed
                while( out.GetSize() && out.EndU8CharOf( '\0' ) ) out.Realloc( out.GetSize() - 1 );
                if( out.GetSize() && out.EndU8CharOf( '\1' ) ) out.Realloc( out.GetSize() - 1 );
            }
            return out;
        }
    }

    // aliases
    template<>
    CVolString base<91>::encode( CVolMem &data ) {
        return base<91>::encode( (const unsigned char*)data.GetPtr(), (size_t)data.GetSize() );
    }
    template<>
    CVolMem base<91>::decode( CVolString &text ) {
        return base<91>::decode( (const wchar_t*)text.GetText(), (size_t)text.GetLength() );
    }
    template<>
    CVolMem base<85>::decode( bool Padding, CVolString &text ) {
        return base<85>::decode( Padding, (const wchar_t*)text.GetText(), (size_t)text.GetLength() );
    }
}
