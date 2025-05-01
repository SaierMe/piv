#include "piv_image.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#include "stb_image.h"

PivImage::PivImage()
{
}

PivImage::~PivImage()
{
   close();
}

void PivImage::close()
{
   if (data)
   {
      stbi_image_free(data);
      data = nullptr;
      x = 0;
      y = 0;
      channels = 0;
   }
}

unsigned char *PivImage::load(const wchar_t *filename, int desired_channels)
{
   close();
   FILE *file = _wfopen(filename, L"rb");
   if (!file)
      return false;
   data = stbi_load_from_file(file, &x, &y, &channels, desired_channels);
   fclose(file);
   return data;
}

unsigned char *PivImage::load(unsigned char *buffer, int len, int desired_channels)
{
   close();
   data = stbi_load_from_memory(buffer, len, &x, &y, &channels, desired_channels);
   return data;
}