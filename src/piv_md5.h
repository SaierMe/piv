#ifndef VOL_PIV_MD5_H
#define VOL_PIV_MD5_H
#pragma once
#include <sys\\base\\libs\\win_base\\vol_base.h>

CVolString GetDataMd5(const void* dat, size_t len, BOOL upper);
CVolString GetDataMd5(CVolMem &dat, BOOL upper);
CVolString GetFileMd5(FILE* file, BOOL upper);
CVolString GetFileMd5(const wchar_t* filename, BOOL upper);
CVolString GetDataMd5sum6(const void* dat, size_t len);
CVolString GetDataMd5sum6(CVolMem &dat);

#endif // end of VOL_PIV_MD5_H
