#include"bmp.h"


BMP::BMP(){
    empty = true;
    bfh = {66, 77, 55, 0, 0, 54};
    bih = {40, 1, 1, 1, 8, 0, 0, 0, 0, 0, 0};
    palette = nullptr;
    imagep = nullptr;
    imagergb = nullptr;
}

BMP::~BMP(){
    empty = true;
    bfh = {0,0,0,0,0,0};
    bih = {0,0,0,0,0,0,0,0,0,0,0};
    if (palette != nullptr){
        delete [] palette;
        palette = nullptr;
    }
    if (imagep != nullptr){
        for (int i = 0; i < int(bih.biHeight); ++i)
            delete [] imagep[i];
        delete [] imagep;
        imagep = nullptr;
    }
    if (imagergb != nullptr){
        for (int i = 0; i < int(bih.biHeight); ++i)
            delete [] imagergb[i];
        delete [] imagergb;
        imagergb = nullptr;
    }
}


int BMP::__inputfromfile(string path){
    FILE * bmpfile = fopen(path.c_str(), "r");
    if (!bmpfile)
        return 1; //not existed file


    if (fread(&bfh.bft1, 1, 1, bmpfile) != 1 ||
            fread(&bfh.bft2, 1, 1, bmpfile) != 1||
            fread(&bfh.bfSize, 4, 1, bmpfile) != 1 ||
            fread(&bfh.bfReserved1, 2, 1, bmpfile) != 1 ||
            fread(&bfh.bfReserved2, 2, 1, bmpfile) != 1 ||
            fread(&bfh.bfOffBits, 4, 1, bmpfile) != 1)
        return 2;

    if (fread(&bih.biSize, 4, 1, bmpfile) != 1 ||
            fread(&bih.biWidth, 4, 1, bmpfile) != 1||
            fread(&bih.biHeight, 4, 1, bmpfile) != 1||
            fread(&bih.biPlanes, 2, 1, bmpfile) != 1||
            fread(&bih.biBitCount, 2, 1, bmpfile) != 1||
            fread(&bih.biCompression, 4, 1, bmpfile) != 1||
            fread(&bih.biSizeImage, 4, 1, bmpfile) != 1||
            fread(&bih.biXPelsPerMeter, 4, 1, bmpfile) != 1||
            fread(&bih.biYPelsPerMeter, 4, 1, bmpfile) != 1||
            fread(&bih.biClrUsed, 4, 1, bmpfile) != 1||
            fread(&bih.biClrImportant, 4, 1, bmpfile) != 1)
        return 3;
    if (bih.biSize - 40 > 0){
        unsigned char *data = new unsigned char[bih.biSize - 40];
        fread(data, 1, bih.biSize - 40, bmpfile);
        delete [] data;
    }

//    if (fread( data, 1, 4, bmpfile) != 4)
//        return 3;//error while reading info header length

//    char32_t n = lechar32(data);

//    delete [] data;

    if (bih.biCompression != 0){
        return 10;//compression!!!
    }


//    data = new unsigned char[n];

//    if (fread(&data[4], 1, n - 4, bmpfile) != n - 4)
//        return 3;//error while reading info header

//    bih = {40, lechar32(&data[4]), lechar32(&data[8]), lechar16(&data[12]), lechar16(&data[14]), lechar32(&data[16]), lechar32(&data[20]), lechar32(&data[24]), lechar32(&data[28]), lechar32(&data[32]), lechar32(&data[36])};

//    bfh.bfSize -= n - 40;



    if (bih.biBitCount <= 8){
        int count = (bih.biClrUsed?int(bih.biClrUsed):int(pow(2, bih.biBitCount)));
        palette = new RGB[count];
        for (int i = 0; i < count; ++i){
            if (fread(&palette[i], 1, 4, bmpfile) != 4)
                return 5;// error while reading palette
        }
    }

    if (bih.biBitCount == 1){
        imagep = new unsigned char*[bih.biHeight];
        for (unsigned int i = 0; i < bih.biHeight; ++i)
            imagep[i] = new unsigned char[bih.biWidth];
        unsigned char c;
        for (unsigned int i = 0; i < bih.biHeight; ++i){
            unsigned int j = 0;
            for (; j < (!bool(bih.biWidth%8) ? bih.biWidth/8 : bih.biWidth/8 + 1); ++j){
                if (fread(&c, 1, 1, bmpfile) != 1)
                        return 6;//error while reading image
                for (unsigned int k = 0; k < 8 && char32_t(j)*8 + k < bih.biWidth; ++k)
                    imagep[i][j*8 + k] = (c&pow(2, uint32_t(7 - k)))/pow(2, uint32_t(7 - k));
            }
            if (j%4 != 0){
                unsigned char e[3];
                fread(&e, 1, 4 - (j - 1)%4 - 1, bmpfile);
            }
        }
    }
    if (bih.biBitCount == 2){
        imagep = new unsigned char*[bih.biHeight];
        for (unsigned int i = 0; i < bih.biHeight; ++i)
            imagep[i] = new unsigned char[bih.biWidth];
        unsigned char c;
        for (unsigned int i = 0; i < bih.biHeight; ++i){
            unsigned int j = 0;
            for (; j < (!bool(bih.biWidth%4) ? bih.biWidth/4 : bih.biWidth/4 + 1); ++j){
                if (fread(&c, 1, 1, bmpfile) != 1)
                        return 6;//error while reading image
                for (unsigned int k = 0; k < 4 && char32_t(j)*4 + k < bih.biWidth; ++k)
                    imagep[i][j*4 + k] = (c&(pow(2, uint32_t(7 - k*2)) + pow(2, uint32_t(7 - k*2 - 1))))/pow(2, uint32_t(7 - k*2 - 1));
            }
            if (j%4 != 0){
                unsigned char e[3];
                fread(&e, 1, 4 - (j - 1)%4 - 1, bmpfile);
            }
        }
    }
    if (bih.biBitCount == 4){
        imagep = new unsigned char*[bih.biHeight];
        for (unsigned int i = 0; i < bih.biHeight; ++i)
            imagep[i] = new unsigned char[bih.biWidth];
        unsigned char c;
        for (unsigned int i = 0; i < bih.biHeight; ++i)
            for (unsigned int j = 0; j < (!bool(bih.biWidth%2) ? bih.biWidth/2 : bih.biWidth/2 + 1); ++j){
                if (fread(&c, 1, 1, bmpfile) != 1)
                        return 6;//error while reading image
                imagep[i][2*j] = c&240/16;
                if (2*j + 1 < bih.biWidth)
                    imagep[i][2*j + 1] = c&15;
            }
    }
    if (bih.biBitCount == 8){
        imagep = new unsigned char*[bih.biHeight];
        for (int i = 0; i < int(bih.biHeight); ++i)
            imagep[i] = new unsigned char[bih.biWidth];

        for (int i = 0; i < int(bih.biHeight); ++i){
            for (int j = 0; j < int(bih.biWidth); ++j){
                if (fread(&imagep[i][j], 1, 1, bmpfile) != 1)
                        return 6;//error while reading image
            }
            if (bih.biWidth%4 != 0){
                unsigned char e[3];
                fread(&e, 1, 4 - (bih.biWidth - 1)%4 - 1, bmpfile);
            }
        }
    }
    if (bih.biBitCount == 16){
        imagergb = new RGB*[bih.biHeight];
        for (int i = 0; i < int(bih.biHeight); ++i)
            imagergb[i] = new RGB[bih.biWidth];
        unsigned char c[2];
        for (int i = 0; i < int(bih.biHeight); ++i){
            for (int j = 0; j < int(bih.biWidth); ++j){
                if (fread(&c, 1, 2, bmpfile) != 2)
                    return 6;//error while reading image
                imagergb[i][j].blue = uint8_t(c[1]&31);
                imagergb[i][j].green = uint8_t((c[1]&224)/32 + (c[0]&3)*8);
                imagergb[i][j].red = uint8_t(c[0]&124/8);
                imagergb[i][j].reserved = 255;
            }
            if (bih.biWidth%4 != 0)
                fread(&c, 1, 2, bmpfile);
        }
    }
    if (bih.biBitCount == 24){
        imagergb = new RGB*[bih.biHeight];
        for (int i = 0; i < int(bih.biHeight); ++i)
            imagergb[i] = new RGB[bih.biWidth];
        unsigned char c[3];
        for (int i = 0; i < int(bih.biHeight); ++i){
            for (int j = 0; j < int(bih.biWidth); ++j){
                if (fread(&c, 1, 3, bmpfile) != 3)
                        return 6;//error while reading image
                imagergb[i][j].blue = c[0];
                imagergb[i][j].green = c[1];
                imagergb[i][j].red = c[2];
                imagergb[i][j].reserved = 255;
            }
            if (bih.biWidth%4 != 0)
                fread(&c, 1, 4 - (bih.biWidth*3 - 1)%4 - 1, bmpfile);
        }
    }
    if (bih.biBitCount == 32){
        imagergb = new RGB*[bih.biHeight];
        for (int i = 0; i < int(bih.biHeight); ++i)
            imagergb[i] = new RGB[bih.biWidth];
        unsigned char c[4];
        for (int i = 0; i < int(bih.biHeight); ++i)
            for (int j = 0; j < int(bih.biWidth); ++j){
                if (fread(&c, 1, 4, bmpfile) != 4)
                        return 6;//error while reading image
                imagergb[i][j].blue = c[0];
                imagergb[i][j].green = c[1];
                imagergb[i][j].red = c[2];
                imagergb[i][j].reserved = c[3];
            }
    }
    fclose(bmpfile);
    empty = false;
    return 0;
}

int BMP::inputfromfile(string path, ostream & out){
    int err = __inputfromfile(path);
    switch (err)
    {
        case 0:
            out << "File successfully readed.\n";
            return 0;
        case 1:
            out << "File not exist.\n";
            break;
        case 2:
            out << "Can't read file header.\n";
            break;
        case 3:
            out << "Can't read info header.\n";
            break;
        case 5:
            out << "Can't read palette of image.\n";
            break;
        case 6:
            out << "Can't read image.\n";
            break;
        case 10:
            out << "Fale has compression.\n";
            break;
        default:
            out << "Unknown error.\n";
            break;
    }
    return 1;
}

int BMP::outpputininfile(string path){
    FILE * bmpfile = fopen(path.c_str(), "w");

    //output of file header
    fwrite(&bfh.bft1, 1, 1, bmpfile);
    fwrite(&bfh.bft2, 1, 1, bmpfile);
    fwrite(&bfh.bfSize, 4, 1, bmpfile);
    fwrite(&bfh.bfReserved1, 2, 1, bmpfile);
    fwrite(&bfh.bfReserved2, 2, 1, bmpfile);
    fwrite(&bfh.bfOffBits, 4, 1, bmpfile);

    //output of info header
    fwrite(&bih.biSize, 4, 1, bmpfile);
    fwrite(&bih.biWidth, 4, 1, bmpfile);
    fwrite(&bih.biHeight, 4, 1, bmpfile);
    fwrite(&bih.biPlanes, 2, 1, bmpfile);
    fwrite(&bih.biBitCount, 2, 1, bmpfile);
    fwrite(&bih.biCompression, 4, 1, bmpfile);
    fwrite(&bih.biSizeImage, 4, 1, bmpfile);
    fwrite(&bih.biXPelsPerMeter, 4, 1, bmpfile);
    fwrite(&bih.biYPelsPerMeter, 4, 1, bmpfile);
    fwrite(&bih.biClrUsed, 4, 1, bmpfile);
    fwrite(&bih.biClrImportant, 4, 1, bmpfile);

    unsigned char* zeros = new unsigned char[bih.biSize - 40]();
    fwrite(zeros, 1, bih.biSize - 40, bmpfile);

    if (palette != nullptr){
        unsigned int counter = (int(bih.biClrUsed)?bih.biClrUsed:pow(2, bih.biBitCount));
        for (unsigned int i = 0; i < counter; ++i){
            char32_t wr = rgbtochar32(palette[i]);
            fwrite(&wr, 4, 1, bmpfile);
        }
    }

    if (imagep != nullptr && imagergb != nullptr)
        return 1;

    if (imagep != nullptr){
        if (bih.biBitCount == 8)
            for (int i = 0; i < int(bih.biHeight); ++i){
                for (int j = 0; j < int(bih.biWidth); ++j){
                    fwrite(&imagep[i][j], 1, 1, bmpfile);
                }
                unsigned char d [3] = {0, 0, 0};
                if (bih.biWidth%4 != 0){
                    fwrite(&d, 1, 4 - (bih.biWidth - 1)%4 - 1, bmpfile);
                }
            }
        if (bih.biBitCount == 4)
            for (unsigned int i = 0; i < bih.biHeight; ++i){
                for (unsigned int j = 0; j < bih.biWidth/2 + bih.biWidth%2; ++j){
                    unsigned char od;
                    od = imagep[i][2*j]*16 + (2*j+1<bih.biWidth? imagep[i][2*j + 1] :0);
                    fwrite(&od, 1, 1, bmpfile);
                }
                unsigned char d [3] = {0, 0, 0};
                if ((int(bih.biWidth)/2 + int(bih.biWidth)%2)%4 != 0){
                    fwrite(&d, 1, 4 - (int(bih.biWidth)/2 + int(bih.biWidth)%2 - 1)%4 - 1, bmpfile);
                }
            }
        if (bih.biBitCount == 2){
            for (unsigned int i = 0; i < bih.biHeight; ++i){
                unsigned int j = 0;
                for (; j < bih.biWidth/4 + uint32_t(bih.biWidth%4>0); ++j){
                    unsigned char out = 0;
                    for (unsigned int k = 0; k < 4 && j*4 + k < bih.biWidth; ++k)
                        out += imagep[i][j*4 + k] * pow(2, 6 - k*2);
                    fwrite(&out, 1, 1, bmpfile);
                }
                unsigned char d [3] = {0, 0, 0};
                --j;
                if (j%4 > 0)
                    fwrite(&d, 1, 4 - (j - 1)%4 - 1, bmpfile);
            }
        }
        if (bih.biBitCount == 1){
            for (unsigned int i = 0; i < bih.biHeight; ++i){
                unsigned int j = 0;
                for (; j < bih.biWidth/8 + (bih.biWidth%8>0?1:0); ++j){
                    unsigned char out = 0;
                    for (unsigned int k = 0; k < 8 && j*8 + k < bih.biWidth; ++k)
                        out += imagep[i][j*8 + k] * pow(2, 7 - k);
                    fwrite(&out, 1, 1, bmpfile);
                }
                unsigned char d [3] = {0, 0, 0};
                if (j%4 > 0)
                    fwrite(&d, 1, 4 - (j - 1)%4 - 1, bmpfile);
            }
        }
    }
    if (imagergb != nullptr){
        if (bih.biBitCount == 16){
            for (int i = 0; i < int(bih.biHeight); ++i){
                for (int j = 0; j < int(bih.biWidth); ++j){
                    char16_t od = rgbtochar16(imagergb[i][j]);
                    fwrite(&od, 2, 1, bmpfile);
                }
                if (bih.biWidth%4 != 0){
                    unsigned char z[2] = {0, 0};
                    fwrite(&z, 1, 2, bmpfile);
                }
            }
        }
        if (bih.biBitCount == 24){
            for (int i = 0; i < int(bih.biHeight); ++i){
                for (int j = 0; j < int(bih.biWidth); ++j){
                    unsigned char * od = &rgbtochar24(imagergb[i][j]);
                    fwrite(od, 1, 3, bmpfile);
                }
                if (bih.biWidth%4 != 0){
                    unsigned char c[3] = {0, 0, 0};
                    fwrite(&c, 1, 4 - (bih.biWidth*3 - 1)%4 - 1, bmpfile);
                }
            }
        }
        if (bih.biBitCount == 32){
            for (int i = 0; i < int(bih.biHeight); ++i)
                for (int j = 0; j < int(bih.biWidth); ++j){
                    char32_t od = rgbtochar32(imagergb[i][j]);
                    fwrite(&od, 4, 1, bmpfile);
                }
        }
    }
    if (empty){
        unsigned char c = 0;
        fwrite(&c, 1, 1, bmpfile);
    }
    fclose(bmpfile);
    return 0;
}

int BMP::writeimage(RGB **image, unsigned int height, unsigned int width, unsigned int bitpercolor){
    if (imagergb != nullptr){
        for (int i = 0; i < int(bih.biHeight); ++i)
            delete [] imagergb[i];
        delete [] imagergb;
        imagergb = nullptr;
    }
    if (imagep != nullptr){
        for (int i = 0; i < int(bih.biHeight); ++i)
            delete [] imagep[i];
        delete [] imagep;
        imagep = nullptr;
    }

    empty = false;
    bfh.bfSize = char32_t(54 + (bitpercolor/8*width + (bitpercolor/8*width)%4)*height);
    bih.biWidth = char32_t(width);
    bih.biHeight = char32_t(height);
    bih.biBitCount = char16_t(bitpercolor);

    imagergb = new RGB*[height];
    for (unsigned int i = 0; i < height; ++i)
        imagergb[i] = new RGB[width];
    for (unsigned int i = 0; i < height; ++i){
        for (unsigned int j = 0; j < width; ++j){
            imagergb[i][j] = image[i][j];
        }
    }
    return 0;
}

int BMP::writeimage(unsigned char **image, unsigned int height, unsigned int width, RGB * palette, unsigned int pallen, unsigned int bitpercolor){
    empty = false;


    if (pallen > 0)
        this->palette = new RGB[pallen];
    for (unsigned int i = 0; i < pallen; ++i)
        this->palette[i] = palette[i];

    if (imagergb != nullptr){
        for (int i = 0; i < int(bih.biHeight); ++i)
            delete [] imagergb[i];
        delete [] imagergb;
        imagergb = nullptr;
    }
    if (imagep != nullptr){
        for (int i = 0; i < int(bih.biHeight); ++i)
            delete [] imagep[i];
        delete [] imagep;
        imagep = nullptr;
    }

    bfh.bfSize = char32_t(54 + pallen*4 + (uint32_t(width*(double(bitpercolor)/8)) + 4 - (uint32_t(width*(double(bitpercolor)/8)) - 1)%4 - 1)*height);
    bih.biWidth = char32_t(width);
    bih.biHeight = char32_t(height);
    bih.biBitCount = char16_t(bitpercolor);
    bih.biClrUsed = pallen;
    bfh.bfOffBits = 54 + pallen * 4;

    imagep = new unsigned char*[height];
    for (unsigned int i = 0; i < height; ++i)
        imagep[i] = new unsigned char[width];
    for (unsigned int i = 0; i < pallen; ++i){
        this->palette[i] = palette[i];
    }
    for (unsigned int i = 0; i < height; ++i)
        for (unsigned int j = 0; j < width; ++j)
            imagep[i][j] = image[i][j];
    return 0;
}


int BMP::upsampling(unsigned int m){
    if (empty)
        return 404;
    if (imagep != nullptr && imagergb != nullptr)
        return 1;

    if (uint32_t(bih.biWidth) > numeric_limits<char32_t>::max()/m || bih.biHeight > numeric_limits<char32_t>::max()/m)
        return 2;

    unsigned int height = uint32_t(bih.biHeight)*m;
    unsigned int width = uint32_t(bih.biWidth)*m;

    if (imagep != nullptr){
        //new image initialisation
        unsigned char **interimage = new unsigned char*[height];
        for (unsigned int i = 0; i < height; ++i)
            interimage[i] = new unsigned char[width];

        //new image fill
        for (unsigned int i = 0; i < bih.biHeight; ++i){
            for (unsigned int j = 0; j < bih.biWidth; ++j){
                for (unsigned int k = 0; k < m; ++k){
                    for (unsigned int l = 0; l < m; ++l){
                        interimage[i*m + k][j*m + l] = imagep[i][j];
                    }
                }
            }
        }

        writeimage(interimage, height, width, this->palette, bih.biClrUsed, bih.biBitCount);
    }
    if (imagergb != nullptr){
        //new image initialisation
        RGB **interimage = new RGB*[height];
        for (unsigned int i = 0; i < height; ++i)
            interimage[i] = new RGB[width];

        //new image fill
        for (unsigned int i = 0; i < bih.biHeight; ++i){
            for (unsigned int j = 0; j < bih.biWidth; ++j){
                for (unsigned int k = 0; k < m; ++k){
                    for (unsigned int l = 0; l < m; ++l){
                        interimage[i*m + k][j*m + l] = imagergb[i][j];
                    }
                }
            }
        }

        writeimage(interimage, height, width, bih.biBitCount);
    }

    return 0;
}

int BMP::downsampling(unsigned int n){
    if (empty)
        return 404;
    if (imagep != nullptr && imagergb != nullptr)
        return 1;

    unsigned int height = bih.biHeight/n + (bih.biHeight%n?1:0);
    unsigned int width = bih.biWidth/n + (bih.biWidth%n?1:0);


    if (imagep != nullptr){

        unsigned char** downimage = new unsigned char *[height];
        for (unsigned int i = 0; i < height; ++i){
            downimage[i] = new unsigned char[width];
            for (unsigned int j = 0; j < width; ++j)
                downimage[i][j] = 0;
        }

        for (unsigned int i = 0; i < height; ++i)
            for (unsigned int j = 0; j < width; ++j)
                for (unsigned  int k = 0; k < n; ++k)
                    for (unsigned int l = 0; l < n; ++l)
                        downimage[i][j] += imagep[i*n+k][j*n+l]/(n*n);

        writeimage(downimage, height, width, this->palette, bih.biClrUsed, bih.biBitCount);
    }
    if (imagergb != nullptr){

        RGB** downimage = new RGB*[height];
        for (unsigned int i = 0; i < height; ++i){
            downimage[i] = new RGB[width];
            for (unsigned int j = 0; j < width; ++j){
                downimage[i][j].red = 0;
                downimage[i][j].green = 0;
                downimage[i][j].blue = 0;
                downimage[i][j].reserved= 0;
            }
        }

        for (unsigned int i = 0; i < height; ++i)
            for (unsigned int j = 0; j < width; ++j)
                for (unsigned  int k = 0; k < n; ++k)
                    for (unsigned int l = 0; l < n; ++l)
                        if (i*n+k < bih.biHeight && j*n+l < bih.biWidth){
                            downimage[i][j].red += imagergb[i*n+k][j*n+l].red/(n*n);
                            downimage[i][j].green += imagergb[i*n+k][j*n+l].green/(n*n);
                            downimage[i][j].blue += imagergb[i*n+k][j*n+l].blue/(n*n);
                            downimage[i][j].reserved += imagergb[i*n+k][j*n+l].reserved/(n*n);
                        }

        writeimage(downimage, height, width, bih.biBitCount);
    }
    return 0;
}

int BMP::resizewithupanddown(unsigned int m, unsigned int n){
    if (upsampling(m))
        return 1;
    outpputininfile("clip0.bmp");
    if (downsampling(n))
        return 2;
    return 0;
}

int BMP::resize(unsigned int m, unsigned int n){
    if (empty)
        return 404;
    if (imagep != nullptr && imagergb != nullptr)
        return 1;
    if (uint32_t(bih.biWidth) > numeric_limits<char32_t>::max()/m*n || bih.biHeight > numeric_limits<char32_t>::max()/m*n)
        return 2;

    unsigned int a = m, b = n, lcm;

    while (a!=0 && b!=0)
        a>b ? a%=b : b%=a;

    lcm = a + b;//actually it is gcd
    lcm = m*n/lcm;

    unsigned int height = bih.biHeight*m/n + (bih.biHeight%n?1:0);
    unsigned int width = bih.biWidth*m/n + (bih.biWidth%n?1:0);

    if (imagep != nullptr){
        unsigned char **tempbuf = new unsigned char*[lcm];
        for (unsigned int i = 0; i < lcm; ++i)
            tempbuf[i] = new unsigned char[lcm];

        unsigned char **newimage = new unsigned char*[height];
        for (unsigned int i = 0; i < height; ++i)
            newimage[i] = new unsigned char[width]();

        for (unsigned int i = 0; i < bih.biHeight/n + (bih.biHeight%n?1:0); ++i)
            for (unsigned int j = 0; j < bih.biWidth/n + (bih.biWidth%n?1:0); ++j){
                //tempbuf fill
                for (unsigned int k = 0; k < n; ++k)
                    for (unsigned int l = 0; l < n; ++l)
                        for (unsigned int g = 0; g < m; ++g)
                            for (unsigned int p = 0; p < m; ++p)
                                if (i*n+k < bih.biHeight && j*n+l < bih.biWidth)
                                    tempbuf[k*m + g][l*m + p] = imagep[i*n+k][j*n+l];
                                else {
                                    tempbuf[k*m + g][l*m+p] = 127;
                                }

                for (unsigned int k = 0; k < m; ++k)
                    for (unsigned int l = 0; l < m; ++l)
                        for (unsigned int g = 0; g < n; ++g)
                            for (unsigned int p = 0; p < n; ++p)
                                newimage[i*m+k][j*m+l] += tempbuf[k*n+g][l*n+p]/n/n;
            }

        writeimage(newimage, height, width, this->palette, bih.biClrUsed, bih.biBitCount);
    }
    if (imagergb != nullptr){
        RGB **tempbuf = new RGB*[lcm];
        for (unsigned int i = 0; i < lcm; ++i)
            tempbuf[i] = new RGB[lcm];

        RGB **newimage = new RGB*[height];
        for (unsigned int i = 0; i < height; ++i)
            newimage[i] = new RGB[width]();

        for (unsigned int i = 0; i < bih.biHeight/n + (bih.biHeight%n?1:0); ++i)
            for (unsigned int j = 0; j < bih.biWidth/n + (bih.biWidth%n?1:0); ++j){
                //tempbuf fill
                for (unsigned int k = 0; k < n; ++k)
                    for (unsigned int l = 0; l < n; ++l)
                        for (unsigned int g = 0; g < m; ++g)
                            for (unsigned int p = 0; p < m; ++p)
                                if (i*n+k < bih.biHeight && j*n+l < bih.biWidth)
                                    tempbuf[k*m + g][l*m + p] = imagergb[i*n+k][j*n+l];
                                else {
                                    tempbuf[k*m + g][l*m+p].red = 127;
                                    tempbuf[k*m + g][l*m+p].green = 127;
                                    tempbuf[k*m + g][l*m+p].blue = 127;
                                    tempbuf[k*m + g][l*m+p].reserved = 255;
                                }

                for (unsigned int k = 0; k < m; ++k)
                    for (unsigned int l = 0; l < m; ++l)
                        for (unsigned int g = 0; g < n; ++g)
                            for (unsigned int p = 0; p < n; ++p){
                                if (i*m+k < height && j*m+l < width){
                                    newimage[i*m+k][j*m+l].red += tempbuf[k*n+g][l*n+p].red/n/n;
                                    newimage[i*m+k][j*m+l].green += tempbuf[k*n+g][l*n+p].green/n/n;
                                    newimage[i*m+k][j*m+l].blue += tempbuf[k*n+g][l*n+p].blue/n/n;
                                    newimage[i*m+k][j*m+l].reserved += tempbuf[k*n+g][l*n+p].reserved/n/n;
                                }
                            }
            }

        writeimage(newimage, height, width, bih.biBitCount);
    }

    return 0;
}

int BMP::monochrome(){
    if (empty)
        return 404;

    if (imagergb == nullptr)
        return 1;

    if (imagep != nullptr)
        return 2;

    imagep = new unsigned char*[bih.biHeight];
    for (unsigned int i = 0; i < bih.biHeight; ++i)
        imagep[i] = new unsigned char[bih.biWidth];

    for (unsigned int i = 0; i < bih.biHeight; ++i)
        for (unsigned int j = 0; j < bih.biWidth; ++j){
            imagep[i][j] = imagergb[i][j].red/3 + imagergb[i][j].blue/3 + imagergb[i][j].green/3;
        }

    bih.biBitCount = 8;
    bfh.bfSize = char32_t(54 + 1024 + (bih.biWidth + 4 - (bih.biWidth - 1)%4 - 1)*bih.biHeight);
    bih.biClrUsed = 0;
    bfh.bfOffBits += 1024;

    palette = new RGB[256];

    for (int i = 0; i < 256; ++i){
        palette[i].red = uint8_t(i);
        palette[i].green = uint8_t(i);
        palette[i].blue = uint8_t(i);
        palette[i].reserved = 0;
    }

    for (int i = 0; i < int(bih.biHeight); ++i)
        delete [] imagergb[i];
    delete [] imagergb;
    imagergb = nullptr;

    return 0;
}

BMP& BMP::logicalfiltration() // return xor of files
{
    if (empty)
        throw 404;

    if (imagergb != nullptr && imagep != nullptr)
        throw 1;

    if (bih.biBitCount != 1)
        throw 2;

    if (imagep == nullptr)
        throw 3;

    unsigned char **newim = new unsigned char*[bih.biHeight];
    for (unsigned int i = 0; i < bih.biHeight; ++i){
        newim[i] = new unsigned char[bih.biWidth];
        for (unsigned int j = 0; j < bih.biWidth; ++j)
            newim[i][j] = 0;
    }
    for (long long i  = 1; i < bih.biHeight - 1; ++i)
        for (long long j = 1; j < bih.biWidth - 1; ++j){
            int c = 0;
            for (int k = -1; k < 2; ++k)
                for (int l = -1; l < 2; ++l)
                    if (k != 0 || l != 0)
                        c += imagep[i+k][j+l];

            if (c == 8)
                newim[i][j] = 1;
            else
            {
                if (c == 0)
                    newim[i][j] = 0;
                else
                    newim[i][j] = imagep[i][j];
            }
        }
    for (unsigned int i = 0;  i < bih.biHeight; ++i){
        newim[i][0] = imagep[i][0];
        newim[i][bih.biWidth - 1] = imagep[i][bih.biWidth - 1];
    }
    for (unsigned int j = 1; j < bih.biWidth - 1; ++j){
        newim[0][j] = imagep[0][j];
        newim[bih.biHeight - 1][j] = imagep[bih.biHeight - 1][j];
    }
    BMP* xorimage = new BMP();

    unsigned char ** xorarray = new unsigned char*[bih.biHeight];
    for (unsigned int i = 0; i < bih.biHeight; ++i)
        xorarray[i] = new unsigned char[bih.biWidth];

    for (unsigned int i = 0; i < bih.biHeight; ++i)
        for (unsigned int j = 0; j < bih.biWidth; ++j)
            xorarray[i][j] = (imagep[i][j] + newim[i][j])%2;

    xorimage->writeimage(xorarray, bih.biHeight, bih.biWidth, this->palette, 2, 1);

    writeimage(newim, bih.biHeight, bih.biWidth, this->palette, 2, 1);

    return *xorimage;
}
