#ifndef BMP_H
#define BMP_H

#include<string>
#include<cstdio>
#include<cmath>
#include<iostream>
#include<limits>
#include<math.h>

using namespace std;

struct BITMAPFILEHEADER
 {
   unsigned char bft1;
   unsigned char bft2;
   char32_t bfSize;
   char16_t bfReserved1;
   char16_t bfReserved2;
   char32_t bfOffBits;
 };

struct BITMAPINFOHEADER
 {
   char32_t biSize;
   char32_t biWidth;
   char32_t biHeight;
   char16_t biPlanes;
   char16_t biBitCount;
   char32_t biCompression;
   char32_t biSizeImage;
   char32_t biXPelsPerMeter;
   char32_t biYPelsPerMeter;
   char32_t biClrUsed;
   char32_t biClrImportant;
 };

struct RGB
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char reserved;

    RGB& operator=(RGB& d){
        red = d.red;
        green = d.green;
        blue = d.blue;
        reserved = d.reserved;
        return *this;
    }
};

class BMP{
private:
    bool empty;
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
    RGB *palette;
    unsigned char **imagep;
    RGB **imagergb;

    char16_t lechar16(unsigned char *data){
        return char16_t(int(data[0]) + int(data[1])*256);
    }

    char32_t lechar32(unsigned char *data){
        return char32_t(int(data[0]) + int(data[1])*256 + int(data[2])*65536 + int(data[3])*16777216);
    }

    char32_t rgbtochar32(RGB data){
        char32_t res = data.reserved;
        res = res*256+data.red;
        res = res*256+data.green;
        res = res*256+data.blue;
        return res;
    }

    char16_t rgbtochar16(RGB data){
        char16_t res = char16_t(int(double(data.red)/255*31));
        res = res*32 + char16_t(int(double(data.green)/255*31));
        res = res*32 + char16_t(int(double(data.blue)/255*31));
        return res;
    }

    unsigned char & rgbtochar24(RGB data){
        unsigned char * res = new unsigned char[3];
        res[0] = data.blue;
        res[1] = data.green;
        res[2] = data.red;
        return *res;
    }

    int __inputfromfile(string path);

    unsigned int pow (unsigned int a, unsigned int b){
        if (b==0)
            return 1;
        unsigned int res = a;
        for (unsigned int i = 0; i < b-1; ++i)
            res*=a;
        return res;
    }

public:
    BMP();
    ~BMP();

    int inputfromfile(string path, ostream & out = cout);
    int outpputininfile(string path);

    int writeimage(RGB **image, unsigned int height, unsigned int width, unsigned int bitpercolor = 24);
    int writeimage(unsigned char **image, unsigned int height, unsigned int width, RGB * palette, unsigned int pallen, unsigned int bitpercolor = 8);

    int upsampling(unsigned int m);
    int downsampling(unsigned int n);
    int resizewithupanddown(unsigned int m, unsigned int n);
    int resize(unsigned int m, unsigned int n);

    int monochrome();
};

#endif // BMP_H
