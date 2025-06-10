#ifndef _PIV_STB_CLASS_H
#define _PIV_STB_CLASS_H

#include <cstddef>
#include <cstdint>

class PivImage
{
public:
    PivImage();
    ~PivImage();

    void close();
    unsigned char* load(const wchar_t* filename, int desired_channels);
    unsigned char* load(unsigned char* buffer, int len, int desired_channels);

public:
    unsigned char* data = nullptr;
    int x = 0;
    int y = 0;
    int channels = 0;
};

#endif // _PIV_STB_CLASS_H
