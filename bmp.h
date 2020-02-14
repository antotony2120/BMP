#ifndef BMP_H
#define BMP_H

#include<string>
#include<cstdio>
#include<cmath>
#include<iostream>
#include<limits>
#include<math.h>
#include<vector>

typedef unsigned int uint;
typedef unsigned char uchar;

using namespace std;

class BMP;

struct BITMAPFILEHEADER
 {
   uchar bft1;
   uchar bft2;
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
    uchar red;
    uchar green;
    uchar blue;
    uchar reserved;

    RGB& operator=(RGB& d){
        red = d.red;
        green = d.green;
        blue = d.blue;
        reserved = d.reserved;
        return *this;
    }
};

struct trio{
    BMP *blue;
    BMP *green;
    BMP *red;
};

class BMP{
private:
    bool empty;
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
    RGB *palette;
    uchar **imagep;
    RGB **imagergb;

    char16_t lechar16(uchar *data){
        return char16_t(int(data[0]) + int(data[1])*256);
    }

    char32_t lechar32(uchar *data){
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

    uchar & rgbtochar24(RGB data){
        uchar * res = new uchar[3];
        res[0] = data.blue;
        res[1] = data.green;
        res[2] = data.red;
        return *res;
    }

    int __inputfromfile(string path);

    uint pow (uint a, uint b){
        if (b==0)
            return 1;
        uint res = a;
        for (uint i = 0; i < b-1; ++i)
            res*=a;
        return res;
    }

public:
    BMP();
    BMP(BMP& bmpfile);
    BMP(string path);
    BMP(vector<vector<uchar> > &matrix);
    ~BMP();

    uint getHeight();
    uint getWidth();

    int inputfromfile(string path, ostream & out = cout);
    int outpputininfile(string path);

    int writematrix(vector<vector<uint>> &matrix);
    int writeimage(RGB **image, uint height, uint width, uint bitpercolor = 24);
    int writeimage(uchar **image, uint height, uint width, RGB * palette, uint pallen, uint bitpercolor = 8);

    int upsampling(uint m);
    int downsampling(uint n);
    int resizewithupanddown(uint m, uint n);
    int resize(uint m, uint n);

    int monochrome();
    int binarization(uchar threshold = 127);
    BMP& logicalfiltration();
    int borderssobel();
    vector<string> &analysisthirdtask();

    BMP& operator=(BMP& bmpfile);

    trio& separatecolors();
    void combinecolors(trio& colors);

    BMP& mirror_vertical();

    BMP& copypart(uint x, uint y, uint sizex=0, uint sizey=0);
    pair<vector<int64_t>, vector<int64_t>> &profiles(); //first vector in pair: horizontal profile; second: vertical profile
    vector<pair<pair<uint, uint>, pair<uint, uint>>>& split_into_lines_by_profiles(pair<vector<int64_t>, vector<int64_t>>& profiles);
    vector<pair<pair<uint, uint>, pair<uint, uint>>>& split_into_letters_by_profiles(pair<vector<int64_t>, vector<int64_t>>& profiles);

    BMP& combine_img_part_and_profile(uint x = 0, uint y = 0, uint sizex = uint(-1), uint sizey = uint(-1));
    BMP& combine_img_part_and_profile(pair<pair<uint, uint>, pair<uint, uint>>& border);

    vector<vector<uint>>& matrix_of_spatial_adjacency();
    vector<vector<double>>& normal_matrix_of_spatial_adjacency();

    vector<vector<uint>> &matrix_of_series_length();

    BMP& logarifmic_improvement();
    BMP& gisteq();
};

#endif // BMP_H
