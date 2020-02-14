#include"bmp.h"


BMP::BMP(){
    empty = true;
    bfh = {66, 77, 55, 0, 0, 54};
    bih = {40, 1, 1, 1, 8, 0, 0, 0, 0, 0, 0};
    palette = nullptr;
    imagep = nullptr;
    imagergb = nullptr;
}

BMP::BMP(BMP& bmpfile):
    BMP()
{
    if (bmpfile.imagep != nullptr && bmpfile.imagergb != nullptr)
        throw 1;

    this->bih = bmpfile.bih;
    this->bfh = bmpfile.bfh;
    this->empty = bmpfile.empty;
    if (bmpfile.palette != nullptr){
        uint pallen = bih.biClrUsed?bih.biClrUsed:pow(2,bih.biBitCount);
        this->palette = new RGB[pallen];
        for (uint i = 0; i < pallen; ++i){
            this->palette[i] = bmpfile.palette[i];
        }
    }
    if (bmpfile.imagep != nullptr){
        this->imagep = new uchar*[getHeight()];
        for (uint i = 0; i < getHeight(); ++i){
            this->imagep[i] = new uchar[getWidth()];
            for (uint j = 0; j < getWidth(); ++j)
                this->imagep[i][j] = bmpfile.imagep[i][j];
        }
    }
    if (bmpfile.imagergb != nullptr){
        this->imagergb = new RGB*[getHeight()];
        for (uint i = 0; i < getHeight(); ++i){
            this->imagergb[i] = new RGB[getWidth()];
            for (uint j = 0; j < getWidth(); ++j)
                this->imagergb[i][j] = bmpfile.imagergb[i][j];
        }
    }
}

BMP::BMP(string path):
    BMP()
{
    this->__inputfromfile(path);
}

BMP::BMP(vector<vector<uchar>> &matrix):
    BMP()
{
    uchar **newimg = new uchar*[(uint(matrix.at(0).size())>0?uint(matrix.at(0).size()):0)];
    for (uint i = 0; i < matrix.size(); ++i)
        newimg[i] = new uchar[matrix.at(i).size()];
    for (uint i = 0; i < matrix.size(); ++i)
        for (uint j = 0; j < (uint(matrix.at(0).size())>0?uint(matrix.at(0).size()):0); ++j)
            newimg[i][j] = matrix.at(i).at(j);

    RGB *palette = new RGB[256];

    for (int i = 0; i < 256; ++i){
        palette[i].red = uint8_t(i);
        palette[i].green = uint8_t(i);
        palette[i].blue = uint8_t(i);
        palette[i].reserved = 0;
    }
    writeimage(newimg, uint(matrix.size()), (uint(matrix.at(0).size())>0?uint(matrix.at(0).size()):0), palette, 256);
    for (uint i = 0; i < matrix.size(); ++i)
        delete [] newimg[i];
    delete [] newimg;
    delete [] palette;
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
        for (int i = 0; i < int(getHeight()); ++i)
            delete [] imagep[i];
        delete [] imagep;
        imagep = nullptr;
    }
    if (imagergb != nullptr){
        for (int i = 0; i < int(getHeight()); ++i)
            delete [] imagergb[i];
        delete [] imagergb;
        imagergb = nullptr;
    }
}


uint BMP::getHeight(){
    return this->bih.biHeight;
}

uint BMP::getWidth(){
    return this->bih.biWidth;
}


int BMP::__inputfromfile(string path){
    if (imagep != nullptr){
        for (uint i = 0; i < getHeight(); ++i)
            delete [] imagep[i];
        delete [] imagep;
        imagep = nullptr;
    }
    if (imagergb != nullptr){
        for (uint i = 0; i < getHeight(); ++i)
            delete [] imagergb[i];
        delete [] imagergb;
        imagergb = nullptr;
    }

    FILE * bmpfile = fopen(path.c_str(), "rb");
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
        uchar *data = new uchar[bih.biSize - 40];
        fread(data, 1, bih.biSize - 40, bmpfile);
        delete [] data;
    }

    if (bih.biCompression != 0){
        return 10;//compression!!!
    }

    if (bih.biBitCount <= 8){
        int count = (bih.biClrUsed?int(bih.biClrUsed):int(pow(2, bih.biBitCount)));
        palette = new RGB[count];
        for (int i = 0; i < count; ++i){
            if (fread(&palette[i], 1, 4, bmpfile) != 4)
                return 5;// error while reading palette
        }
    }

    if (bih.biBitCount == 1){
        imagep = new uchar*[getHeight()];
        for (uint i = 0; i < getHeight(); ++i)
            imagep[i] = new uchar[getWidth()];
        uchar c;
        for (uint i = 0; i < getHeight(); ++i){
            uint j = 0;
            for (; j < (!bool(getWidth()%8) ? getWidth()/8 : getWidth()/8 + 1); ++j){
                if (fread(&c, 1, 1, bmpfile) != 1)
                        return 6;//error while reading image
                for (uint k = 0; k < 8 && char32_t(j)*8 + k < getWidth(); ++k)
                    imagep[i][j*8 + k] = (c&pow(2, uint32_t(7 - k)))/pow(2, uint32_t(7 - k));
            }
            if (j%4 != 0){
                uchar e[3];
                fread(&e, 1, 4 - (j - 1)%4 - 1, bmpfile);
            }
        }
    }
    if (bih.biBitCount == 2){
        imagep = new uchar*[getHeight()];
        for (uint i = 0; i < getHeight(); ++i)
            imagep[i] = new uchar[getWidth()];
        uchar c;
        for (uint i = 0; i < getHeight(); ++i){
            uint j = 0;
            for (; j < (!bool(getWidth()%4) ? getWidth()/4 : getWidth()/4 + 1); ++j){
                if (fread(&c, 1, 1, bmpfile) != 1)
                        return 6;//error while reading image
                for (uint k = 0; k < 4 && char32_t(j)*4 + k < getWidth(); ++k)
                    imagep[i][j*4 + k] = (c&(pow(2, uint32_t(7 - k*2)) + pow(2, uint32_t(7 - k*2 - 1))))/pow(2, uint32_t(7 - k*2 - 1));
            }
            if (j%4 != 0){
                uchar e[3];
                fread(&e, 1, 4 - (j - 1)%4 - 1, bmpfile);
            }
        }
    }
    if (bih.biBitCount == 4){
        imagep = new uchar*[getHeight()];
        for (uint i = 0; i < getHeight(); ++i)
            imagep[i] = new uchar[getWidth()];
        uchar c;
        for (uint i = 0; i < getHeight(); ++i)
            for (uint j = 0; j < (!bool(getWidth()%2) ? getWidth()/2 : getWidth()/2 + 1); ++j){
                if (fread(&c, 1, 1, bmpfile) != 1)
                        return 6;//error while reading image
                imagep[i][2*j] = c&240/16;
                if (2*j + 1 < getWidth())
                    imagep[i][2*j + 1] = c&15;
            }
    }
    if (bih.biBitCount == 8){
        imagep = new uchar*[getHeight()];
        for (int i = 0; i < int(getHeight()); ++i)
            imagep[i] = new uchar[getWidth()];

        for (int i = 0; i < int(getHeight()); ++i){
            for (int j = 0; j < int(getWidth()); ++j){
                if (fread(&imagep[i][j], 1, 1, bmpfile) != 1)
                        return 6;//error while reading image
            }
            if (getWidth()%4 != 0){
                uchar e[3];
                fread(&e, 1, 4 - (getWidth() - 1)%4 - 1, bmpfile);
            }
        }
    }
    if (bih.biBitCount == 16){
        imagergb = new RGB*[getHeight()];
        for (int i = 0; i < int(getHeight()); ++i)
            imagergb[i] = new RGB[getWidth()];
        uchar c[2];
        for (int i = 0; i < int(getHeight()); ++i){
            for (int j = 0; j < int(getWidth()); ++j){
                if (fread(&c, 1, 2, bmpfile) != 2)
                    return 6;//error while reading image
                imagergb[i][j].blue = uint8_t(c[1]&31);
                imagergb[i][j].green = uint8_t((c[1]&224)/32 + (c[0]&3)*8);
                imagergb[i][j].red = uint8_t(c[0]&124/8);
                imagergb[i][j].reserved = 255;
            }
            if (getWidth()%4 != 0)
                fread(&c, 1, 2, bmpfile);
        }
    }
    if (bih.biBitCount == 24){
        imagergb = new RGB*[getHeight()];
        for (int i = 0; i < int(getHeight()); ++i)
            imagergb[i] = new RGB[getWidth()];
        uchar c[3];
        for (int i = 0; i < int(getHeight()); ++i){
            for (int j = 0; j < int(getWidth()); ++j){
                if (fread(&c, 1, 3, bmpfile) != 3){
                    fclose(bmpfile);
                    return 6;//error while reading image
                }
                imagergb[i][j].blue = c[0];
                imagergb[i][j].green = c[1];
                imagergb[i][j].red = c[2];
                imagergb[i][j].reserved = 255;
            }
            if (getWidth()%4 != 0)
                fread(&c, 1, 4 - (getWidth()*3 - 1)%4 - 1, bmpfile);
        }
    }
    if (bih.biBitCount == 32){
        imagergb = new RGB*[getHeight()];
        for (int i = 0; i < int(getHeight()); ++i)
            imagergb[i] = new RGB[getWidth()];
        uchar c[4];
        for (int i = 0; i < int(getHeight()); ++i)
            for (int j = 0; j < int(getWidth()); ++j){
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
    FILE * bmpfile = fopen(path.c_str(), "wb");

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

    uchar* zeros = new uchar[bih.biSize - 40]();
    fwrite(zeros, 1, bih.biSize - 40, bmpfile);

    if (palette != nullptr){
        uint counter = (int(bih.biClrUsed)?bih.biClrUsed:pow(2, bih.biBitCount));
        for (uint i = 0; i < counter; ++i){
            char32_t wr = rgbtochar32(palette[i]);
            fwrite(&wr, 4, 1, bmpfile);
        }
    }

    if (imagep != nullptr && imagergb != nullptr)
        return 1;

    if (imagep != nullptr){
        if (bih.biBitCount == 8)
            for (int i = 0; i < int(getHeight()); ++i){
                for (int j = 0; j < int(getWidth()); ++j){
                    fwrite(&imagep[i][j], 1, 1, bmpfile);
                }
                uchar d [3] = {0, 0, 0};
                if (getWidth()%4 != 0){
                    fwrite(&d, 1, 4 - (getWidth() - 1)%4 - 1, bmpfile);
                }
            }
        if (bih.biBitCount == 4)
            for (uint i = 0; i < getHeight(); ++i){
                for (uint j = 0; j < getWidth()/2 + getWidth()%2; ++j){
                    uchar od;
                    od = imagep[i][2*j]*16 + (2*j+1<getWidth()? imagep[i][2*j + 1] :0);
                    fwrite(&od, 1, 1, bmpfile);
                }
                uchar d [3] = {0, 0, 0};
                if ((int(getWidth())/2 + int(getWidth())%2)%4 != 0){
                    fwrite(&d, 1, 4 - (int(getWidth())/2 + int(getWidth())%2 - 1)%4 - 1, bmpfile);
                }
            }
        if (bih.biBitCount == 2){
            for (uint i = 0; i < getHeight(); ++i){
                uint j = 0;
                for (; j < getWidth()/4 + uint32_t(getWidth()%4>0); ++j){
                    uchar out = 0;
                    for (uint k = 0; k < 4 && j*4 + k < getWidth(); ++k)
                        out += imagep[i][j*4 + k] * pow(2, 6 - k*2);
                    fwrite(&out, 1, 1, bmpfile);
                }
                uchar d [3] = {0, 0, 0};
                --j;
                if (j%4 > 0)
                    fwrite(&d, 1, 4 - (j - 1)%4 - 1, bmpfile);
            }
        }
        if (bih.biBitCount == 1){
            for (uint i = 0; i < getHeight(); ++i){
                uint j = 0;
                for (; j < getWidth()/8 + (getWidth()%8>0?1:0); ++j){
                    uchar out = 0;
                    for (uint k = 0; k < 8 && j*8 + k < getWidth(); ++k)
                        out += imagep[i][j*8 + k] * pow(2, 7 - k);
                    fwrite(&out, 1, 1, bmpfile);
                }
                uchar d [3] = {0, 0, 0};
                if (j%4 > 0)
                    fwrite(&d, 1, 4 - (j - 1)%4 - 1, bmpfile);
            }
        }
    }
    if (imagergb != nullptr){
        if (bih.biBitCount == 16){
            for (int i = 0; i < int(getHeight()); ++i){
                for (int j = 0; j < int(getWidth()); ++j){
                    char16_t od = rgbtochar16(imagergb[i][j]);
                    fwrite(&od, 2, 1, bmpfile);
                }
                if (getWidth()%4 != 0){
                    uchar z[2] = {0, 0};
                    fwrite(&z, 1, 2, bmpfile);
                }
            }
        }
        if (bih.biBitCount == 24){
            for (int i = 0; i < int(getHeight()); ++i){
                for (int j = 0; j < int(getWidth()); ++j){
                    uchar * od = &rgbtochar24(imagergb[i][j]);
                    fwrite(od, 1, 3, bmpfile);
                }
                if (getWidth()%4 != 0){
                    uchar c[3] = {0, 0, 0};
                    fwrite(&c, 1, 4 - (getWidth()*3 - 1)%4 - 1, bmpfile);
                }
            }
        }
        if (bih.biBitCount == 32){
            for (int i = 0; i < int(getHeight()); ++i)
                for (int j = 0; j < int(getWidth()); ++j){
                    char32_t od = rgbtochar32(imagergb[i][j]);
                    fwrite(&od, 4, 1, bmpfile);
                }
        }
    }
    if (empty){
        uchar c = 0;
        fwrite(&c, 1, 1, bmpfile);
    }
    fclose(bmpfile);
    return 0;
}

int BMP::writeimage(RGB **image, uint height, uint width, uint bitpercolor){
    if (imagergb != nullptr){
        for (int i = 0; i < int(getHeight()); ++i)
            delete [] imagergb[i];
        delete [] imagergb;
        imagergb = nullptr;
    }
    if (imagep != nullptr){
        for (int i = 0; i < int(getHeight()); ++i)
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
    for (uint i = 0; i < height; ++i)
        imagergb[i] = new RGB[width];
    for (uint i = 0; i < height; ++i){
        for (uint j = 0; j < width; ++j){
            imagergb[i][j] = image[i][j];
        }
    }
    return 0;
}

int BMP::writeimage(uchar **image, uint height, uint width, RGB * palette, uint pallen, uint bitpercolor)
{
    empty = false;

    if (pallen == 0)
    {
        pallen = pow(2, bih.biBitCount);
    }
    if (pallen > 0)
        this->palette = new RGB[pallen];

    for (uint i = 0; i < pallen; ++i)
        this->palette[i] = palette[i];

    if (imagergb != nullptr){
        for (int i = 0; i < int(getHeight()); ++i)
            delete [] imagergb[i];
        delete [] imagergb;
        imagergb = nullptr;
    }
    if (imagep != nullptr){
        for (int i = 0; i < int(getHeight()); ++i)
            delete [] imagep[i];
        delete [] imagep;
        imagep = nullptr;
    }

    bfh.bfSize = char32_t(54 + pallen*4 + (uint32_t(width*(double(bitpercolor)/8)) + 4 - (uint32_t(width*(double(bitpercolor)/8)) - 1)%4 - 1)*height);
    bih.biWidth = char32_t(width);
    bih.biHeight = char32_t(height);
    bih.biBitCount = char16_t(bitpercolor);
    bih.biClrUsed = pallen<pow(2, bih.biBitCount)?pallen:0;
    bfh.bfOffBits = 14 + bih.biSize + pallen * 4;

    imagep = new uchar*[height];
    for (uint i = 0; i < height; ++i)
        imagep[i] = new uchar[width];
    for (uint i = 0; i < pallen; ++i){
        this->palette[i] = palette[i];
    }
    for (uint i = 0; i < height; ++i)
        for (uint j = 0; j < width; ++j)
            imagep[i][j] = image[i][j];
    return 0;
}


int BMP::upsampling(uint m){
    if (empty)
        return 404;
    if (imagep != nullptr && imagergb != nullptr)
        return 1;

    if (uint32_t(getWidth()) > numeric_limits<char32_t>::max()/m || getHeight() > numeric_limits<char32_t>::max()/m)
        return 2;

    uint height = uint32_t(getHeight())*m;
    uint width = uint32_t(getWidth())*m;

    if (imagep != nullptr){
        //new image initialisation
        uchar **interimage = new uchar*[height];
        for (uint i = 0; i < height; ++i)
            interimage[i] = new uchar[width];

        //new image fill
        for (uint i = 0; i < getHeight(); ++i){
            for (uint j = 0; j < getWidth(); ++j){
                for (uint k = 0; k < m; ++k){
                    for (uint l = 0; l < m; ++l){
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
        for (uint i = 0; i < height; ++i)
            interimage[i] = new RGB[width];

        //new image fill
        for (uint i = 0; i < getHeight(); ++i){
            for (uint j = 0; j < getWidth(); ++j){
                for (uint k = 0; k < m; ++k){
                    for (uint l = 0; l < m; ++l){
                        interimage[i*m + k][j*m + l] = imagergb[i][j];
                    }
                }
            }
        }

        writeimage(interimage, height, width, bih.biBitCount);
    }

    return 0;
}

int BMP::downsampling(uint n){
    if (empty)
        return 404;
    if (imagep != nullptr && imagergb != nullptr)
        return 1;

    uint height = getHeight()/n + (getHeight()%n?1:0);
    uint width = getWidth()/n + (getWidth()%n?1:0);


    if (imagep != nullptr){

        uchar** downimage = new uchar *[height];
        for (uint i = 0; i < height; ++i){
            downimage[i] = new uchar[width];
            for (uint j = 0; j < width; ++j)
                downimage[i][j] = 0;
        }

        for (uint i = 0; i < height; ++i)
            for (uint j = 0; j < width; ++j)
                for (unsigned  int k = 0; k < n; ++k)
                    for (uint l = 0; l < n; ++l)
                        downimage[i][j] += imagep[i*n+k][j*n+l]/(n*n);

        writeimage(downimage, height, width, this->palette, bih.biClrUsed, bih.biBitCount);
    }
    if (imagergb != nullptr){

        RGB** downimage = new RGB*[height];
        for (uint i = 0; i < height; ++i){
            downimage[i] = new RGB[width];
            for (uint j = 0; j < width; ++j){
                downimage[i][j].red = 0;
                downimage[i][j].green = 0;
                downimage[i][j].blue = 0;
                downimage[i][j].reserved= 0;
            }
        }

        for (uint i = 0; i < height; ++i)
            for (uint j = 0; j < width; ++j)
                for (unsigned  int k = 0; k < n; ++k)
                    for (uint l = 0; l < n; ++l)
                        if (i*n+k < getHeight() && j*n+l < getWidth()){
                            downimage[i][j].red += imagergb[i*n+k][j*n+l].red/(n*n);
                            downimage[i][j].green += imagergb[i*n+k][j*n+l].green/(n*n);
                            downimage[i][j].blue += imagergb[i*n+k][j*n+l].blue/(n*n);
                            downimage[i][j].reserved += imagergb[i*n+k][j*n+l].reserved/(n*n);
                        }

        writeimage(downimage, height, width, bih.biBitCount);
    }
    return 0;
}

int BMP::resizewithupanddown(uint m, uint n){
    if (upsampling(m))
        return 1;
    outpputininfile("clip0.bmp");
    if (downsampling(n))
        return 2;
    return 0;
}

int BMP::resize(uint m, uint n){
    if (empty)
        return 404;
    if (imagep != nullptr && imagergb != nullptr)
        return 1;
    if (uint32_t(getWidth()) > numeric_limits<char32_t>::max()/m*n || getHeight() > numeric_limits<char32_t>::max()/m*n)
        return 2;

    uint a = m, b = n, lcm;

    while (a!=0 && b!=0)
        a>b ? a%=b : b%=a;

    lcm = a + b;//actually it is gcd
    lcm = m*n/lcm;

    uint height = getHeight()*m/n + (getHeight()%n?1:0);
    uint width = getWidth()*m/n + (getWidth()%n?1:0);

    if (imagep != nullptr){
        uchar **tempbuf = new uchar*[lcm];
        for (uint i = 0; i < lcm; ++i)
            tempbuf[i] = new uchar[lcm];

        uchar **newimage = new uchar*[height];
        for (uint i = 0; i < height; ++i)
            newimage[i] = new uchar[width]();

        for (uint i = 0; i < getHeight()/n + (getHeight()%n?1:0); ++i)
            for (uint j = 0; j < getWidth()/n + (getWidth()%n?1:0); ++j){
                //tempbuf fill
                for (uint k = 0; k < n; ++k)
                    for (uint l = 0; l < n; ++l)
                        for (uint g = 0; g < m; ++g)
                            for (uint p = 0; p < m; ++p)
                                if (i*n+k < getHeight() && j*n+l < getWidth())
                                    tempbuf[k*m + g][l*m + p] = imagep[i*n+k][j*n+l];
                                else {
                                    tempbuf[k*m + g][l*m+p] = 127;
                                }

                for (uint k = 0; k < m; ++k)
                    for (uint l = 0; l < m; ++l)
                        for (uint g = 0; g < n; ++g)
                            for (uint p = 0; p < n; ++p)
                                newimage[i*m+k][j*m+l] += tempbuf[k*n+g][l*n+p]/n/n;
            }

        writeimage(newimage, height, width, this->palette, bih.biClrUsed, bih.biBitCount);
    }
    if (imagergb != nullptr){
        RGB **tempbuf = new RGB*[lcm];
        for (uint i = 0; i < lcm; ++i)
            tempbuf[i] = new RGB[lcm];

        RGB **newimage = new RGB*[height];
        for (uint i = 0; i < height; ++i)
            newimage[i] = new RGB[width]();

        for (uint i = 0; i < getHeight()/n + (getHeight()%n?1:0); ++i)
            for (uint j = 0; j < getWidth()/n + (getWidth()%n?1:0); ++j){
                //tempbuf fill
                for (uint k = 0; k < n; ++k)
                    for (uint l = 0; l < n; ++l)
                        for (uint g = 0; g < m; ++g)
                            for (uint p = 0; p < m; ++p)
                                if (i*n+k < getHeight() && j*n+l < getWidth())
                                    tempbuf[k*m + g][l*m + p] = imagergb[i*n+k][j*n+l];
                                else {
                                    tempbuf[k*m + g][l*m+p].red = 127;
                                    tempbuf[k*m + g][l*m+p].green = 127;
                                    tempbuf[k*m + g][l*m+p].blue = 127;
                                    tempbuf[k*m + g][l*m+p].reserved = 255;
                                }

                for (uint k = 0; k < m; ++k)
                    for (uint l = 0; l < m; ++l)
                        for (uint g = 0; g < n; ++g)
                            for (uint p = 0; p < n; ++p){
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

    imagep = new uchar*[getHeight()];
    for (uint i = 0; i < getHeight(); ++i)
        imagep[i] = new uchar[getWidth()];

    for (uint i = 0; i < getHeight(); ++i)
        for (uint j = 0; j < getWidth(); ++j){
            imagep[i][j] = imagergb[i][j].red/3 + imagergb[i][j].blue/3 + imagergb[i][j].green/3;
        }

    bih.biBitCount = 8;
    bfh.bfSize = char32_t(54 + 1024 + (getWidth() + 4 - (getWidth() - 1)%4 - 1)*getHeight());
    bih.biClrUsed = 0;
    bfh.bfOffBits += 1024;

    palette = new RGB[256];

    for (int i = 0; i < 256; ++i){
        palette[i].red = uint8_t(i);
        palette[i].green = uint8_t(i);
        palette[i].blue = uint8_t(i);
        palette[i].reserved = 0;
    }

    for (int i = 0; i < int(getHeight()); ++i)
        delete [] imagergb[i];
    delete [] imagergb;
    imagergb = nullptr;

    return 0;
}


int BMP::binarization(uchar threshold){
    if (empty)
        return 404;
    if (imagep != nullptr && imagergb != nullptr)
        return 1;

    if (bih.biBitCount > 8){
        monochrome();
    }
    uchar **newimage = new uchar*[getHeight()];
    for (uint i = 0; i < getHeight(); ++i)
        newimage[i] = new uchar[getWidth()];
    for (uint i = 0; i < getHeight(); ++i)
        for (uint j = 0; j < getWidth(); ++j){
            newimage[i][j] = (imagep[i][j] > threshold? 1: 0);
        }
    RGB *palet = new RGB[2];
    palet[0].blue = 0;
    palet[0].red = 0;
    palet[0].green = 0;
    palet[0].reserved = 255;
    palet[1].blue = 255;
    palet[1].red = 255;
    palet[1].green = 255;
    palet[1].reserved = 255;
    writeimage(newimage, getHeight(), getWidth(), palet, 2, 1);
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

    uchar **newimage = new uchar*[getHeight()];
    for (uint i = 0; i < getHeight(); ++i){
        newimage[i] = new uchar[getWidth()];
        for (uint j = 0; j < getWidth(); ++j)
            newimage[i][j] = 0;
    }
    for (long long i  = 1; i < getHeight() - 1; ++i)
        for (long long j = 1; j < getWidth() - 1; ++j){
            int c = 0;
            for (int k = -1; k < 2; ++k)
                for (int l = -1; l < 2; ++l)
                    if (k != 0 || l != 0)
                        c += imagep[i+k][j+l];

            if (c == 8)
                newimage[i][j] = 1;
            else
            {
                if (c == 0)
                    newimage[i][j] = 0;
                else
                    newimage[i][j] = imagep[i][j];
            }
        }
    for (uint i = 0;  i < getHeight(); ++i){
        newimage[i][0] = imagep[i][0];
        newimage[i][getWidth() - 1] = imagep[i][getWidth() - 1];
    }
    for (uint j = 1; j < getWidth() - 1; ++j){
        newimage[0][j] = imagep[0][j];
        newimage[getHeight() - 1][j] = imagep[getHeight() - 1][j];
    }
    BMP* xorimage = new BMP();

    uchar ** xorarray = new uchar*[getHeight()];
    for (uint i = 0; i < getHeight(); ++i)
        xorarray[i] = new uchar[getWidth()];

    for (uint i = 0; i < getHeight(); ++i)
        for (uint j = 0; j < getWidth(); ++j)
            xorarray[i][j] = (imagep[i][j] + newimage[i][j])%2;

    xorimage->writeimage(xorarray, getHeight(), getWidth(), this->palette, 2, 1);

    for (uint i = 0; i < getHeight(); ++i)
        delete [] imagep[i];
    delete [] imagep;
    imagep = newimage;

    return *xorimage;
}

int BMP::borderssobel(){
    if (empty)
        throw 404;

    if (imagergb != nullptr && imagep != nullptr)
        throw 1;


    if (bih.biBitCount > 8){
        RGB ** newimage = new RGB*[getHeight()];
        for (uint i = 0; i < getHeight(); ++i)
            newimage[i] = new RGB[getWidth()];
        unsigned short int ** blue = new unsigned short int*[getHeight()];
        unsigned short int ** red = new unsigned short int*[getHeight()];
        unsigned short int ** green = new unsigned short int*[getHeight()];
        for (uint i = 0; i < getHeight(); ++i){
            blue[i] = new unsigned short int[getWidth()];
            red[i] = new unsigned short int[getWidth()];
            green[i] = new unsigned short int[getWidth()];
        }
        for (uint i = 0; i < getHeight(); ++i){
            newimage[i][getWidth() - 1].blue = 0;
            newimage[i][getWidth() - 1].red = 0;
            newimage[i][getWidth() - 1].green = 0;
            newimage[i][getWidth() - 1].reserved = 255;
            newimage[i][0].blue = 0;
            newimage[i][0].red = 0;
            newimage[i][0].green = 0;
            newimage[i][0].reserved = 255;
        }
        for (uint i = 1; i < getWidth() - 1; ++i){
            newimage[getHeight() - 1][i].blue = 0;
            newimage[getHeight() - 1][i].red = 0;
            newimage[getHeight() - 1][i].green = 0;
            newimage[getHeight() - 1][i].reserved = 255;
            newimage[0][i].blue = 0;
            newimage[0][i].red = 0;
            newimage[0][i].green = 0;
            newimage[0][i].reserved = 255;
        }
        unsigned short int maximumb = 0;
        unsigned short int maximumg = 0;
        unsigned short int maximumr = 0;
        for (long long i = 1; i < getHeight() - 1; ++i)
            for (long long j = 1; j < getWidth() - 1; ++j){
                blue[i][j] = uint16_t(int(sqrt((-imagergb[i - 1][j - 1].blue - 2*imagergb[i - 1][j].blue - imagergb[i - 1][j + 1].blue + imagergb[i + 1][j - 1].blue + 2*imagergb[i + 1][j].blue + imagergb[i + 1][j + 1].blue)*
                        (-imagergb[i - 1][j - 1].blue - 2*imagergb[i - 1][j].blue - imagergb[i - 1][j + 1].blue + imagergb[i + 1][j - 1].blue + 2*imagergb[i + 1][j].blue + imagergb[i + 1][j + 1].blue) +
                        (-imagergb[i - 1][j - 1].blue - 2*imagergb[i][j - 1].blue - imagergb[i + 1][j - 1].blue + imagergb[i - 1][j + 1].blue + 2*imagergb[i][j + 1].blue + imagergb[i + 1][j + 1].blue)*
                        (-imagergb[i - 1][j - 1].blue - 2*imagergb[i][j - 1].blue - imagergb[i + 1][j - 1].blue + imagergb[i - 1][j + 1].blue + 2*imagergb[i][j + 1].blue + imagergb[i + 1][j + 1].blue))));
                red[i][j] = uint16_t(int(sqrt((-imagergb[i - 1][j - 1].red - 2*imagergb[i - 1][j].red - imagergb[i - 1][j + 1].red + imagergb[i + 1][j - 1].red + 2*imagergb[i + 1][j].red + imagergb[i + 1][j + 1].red)*
                        (-imagergb[i - 1][j - 1].red - 2*imagergb[i - 1][j].red - imagergb[i - 1][j + 1].red + imagergb[i + 1][j - 1].red + 2*imagergb[i + 1][j].red + imagergb[i + 1][j + 1].red) +
                        (-imagergb[i - 1][j - 1].red - 2*imagergb[i][j - 1].red - imagergb[i + 1][j - 1].red + imagergb[i - 1][j + 1].red + 2*imagergb[i][j + 1].red + imagergb[i + 1][j + 1].red)*
                        (-imagergb[i - 1][j - 1].red - 2*imagergb[i][j - 1].red - imagergb[i + 1][j - 1].red + imagergb[i - 1][j + 1].red + 2*imagergb[i][j + 1].red + imagergb[i + 1][j + 1].red))));
                green[i][j] = uint16_t(int(sqrt((-imagergb[i - 1][j - 1].green - 2*imagergb[i - 1][j].green - imagergb[i - 1][j + 1].green + imagergb[i + 1][j - 1].green + 2*imagergb[i + 1][j].green + imagergb[i + 1][j + 1].green)*
                        (-imagergb[i - 1][j - 1].green - 2*imagergb[i - 1][j].green - imagergb[i - 1][j + 1].green + imagergb[i + 1][j - 1].green + 2*imagergb[i + 1][j].green + imagergb[i + 1][j + 1].green) +
                        (-imagergb[i - 1][j - 1].green - 2*imagergb[i][j - 1].green - imagergb[i + 1][j - 1].green + imagergb[i - 1][j + 1].green + 2*imagergb[i][j + 1].green + imagergb[i + 1][j + 1].green)*
                        (-imagergb[i - 1][j - 1].green - 2*imagergb[i][j - 1].green - imagergb[i + 1][j - 1].green + imagergb[i - 1][j + 1].green + 2*imagergb[i][j + 1].green + imagergb[i + 1][j + 1].green))));
                newimage[i][j].reserved = 255;
                if (blue[i][j] > maximumb)
                    maximumb = blue[i][j];
                if (red[i][j] > maximumr)
                    maximumr = red[i][j];
                if (green[i][j] > maximumg)
                    maximumg = green[i][j];
            }
        for (long long i = 1; i < getHeight() - 1; ++i)
            for (long long j = 1; j < getWidth() - 1; ++j){
                newimage[i][j].blue = uint8_t(int(double(blue[i][j])/(maximumb?maximumb:1)*255));
                newimage[i][j].red = uint8_t(int(double(red[i][j])/(maximumr?maximumr:1)*255));
                newimage[i][j].green = uint8_t(int(double(green[i][j])/(maximumg?maximumg:1)*255));
            }

        for (uint i = 0; i < getHeight(); ++i)
            delete [] imagergb[i];
        delete [] imagergb;
        imagergb = newimage;
    }
    else{
        uchar ** newimage = new uchar *[getHeight()];
        for (uint i = 0; i < getHeight(); ++i)
            newimage[i] = new uchar[getWidth()];
        for (uint i = 0;  i < getHeight(); ++i){
            newimage[i][0] = 0;
            newimage[i][getWidth() - 1] = 0;
        }
        for (uint j = 1; j < getWidth() - 1; ++j){
            newimage[0][j] = 0;
            newimage[getHeight() - 1][j] = 0;
        }
        unsigned short int ** gray = new unsigned short int*[getHeight()];
        for (uint i = 0; i < getHeight(); ++i)
            gray[i] = new unsigned short int[getWidth()];
        unsigned short int maximum = 0;
        for (long long i = 1; i < getHeight() - 1; ++i)
            for (long long j = 1; j < getWidth() - 1; ++j){
                gray[i][j] = uint16_t(int(sqrt((-imagep[i - 1][j - 1] - 2*imagep[i - 1][j] - imagep[i - 1][j + 1] + imagep[i + 1][j - 1] + 2*imagep[i + 1][j] + imagep[i + 1][j + 1])*
                        (-imagep[i - 1][j - 1] - 2*imagep[i - 1][j] - imagep[i - 1][j + 1] + imagep[i + 1][j - 1] + 2*imagep[i + 1][j] + imagep[i + 1][j + 1]) +
                        (-imagep[i - 1][j - 1] - 2*imagep[i][j - 1] - imagep[i + 1][j - 1] + imagep[i - 1][j + 1] + 2*imagep[i][j + 1] + imagep[i + 1][j + 1])*
                        (-imagep[i - 1][j - 1] - 2*imagep[i][j - 1] - imagep[i + 1][j - 1] + imagep[i - 1][j + 1] + 2*imagep[i][j + 1] + imagep[i + 1][j + 1]))));
                if (gray[i][j] > maximum)
                    maximum = gray[i][j];
            }
        for (long long i = 1; i < getHeight() - 1; ++i)
            for (long long j = 1; j < getWidth() - 1; ++j){
                newimage[i][j] = uint8_t(int(double(gray[i][j])/(maximum?maximum:1) * 255));
            }
        RGB * palet = new RGB[256];
        for (int i = 0; i < 256; ++i){
            palet[i].blue = uint8_t(i);
            palet[i].red = uint8_t(i);
            palet[i].green = uint8_t(i);
            palet[i].reserved = 255;
        }
        writeimage(newimage, getHeight(), getWidth(), palet, 256, 8);
    }
    return 0;
}

vector<string>& BMP::analysisthirdtask(){
    if (empty)
        throw 404;
    if (bih.biBitCount > 8)
        throw 1;
    if (imagep == nullptr)
        throw 2;

    int black = 0;
    vector<string> *result = new vector<string>();
    for (uint i = 0; i < getHeight(); ++i)
        for (uint j = 0; j < getWidth(); ++j)
            black += (pow(2, bih.biBitCount) - imagep[i][j] - 1);

    result->push_back(to_string(black));
    result->push_back(to_string(double(black)/(getWidth()*getHeight())));
    double x = 0;
    double y = 0;
    for (uint i = 0; i < getHeight(); ++i)
        for (uint j = 0; j < getWidth(); ++j){
            y += double(i)*(pow(2, bih.biBitCount) - 1 - imagep[i][j])/black;
            x += double(j)*(pow(2, bih.biBitCount) - 1 - imagep[i][j])/black;
        }
    result->push_back(to_string(x));
    result->push_back(to_string(y));
    result->push_back(to_string((x - 1)/(getWidth() - 1)));
    result->push_back(to_string((y - 1)/(getHeight() - 1)));
    double ix = 0;
    double iy = 0;
    for (uint i = 0; i < getHeight(); ++i)
        for (uint j = 0; j < getWidth(); ++j){
            ix += (double(j) - x)*(double(j) - x)*(pow(2, bih.biBitCount) - 1 - imagep[i][j]);
            iy += (double(i) - y)*(double(i) - y)*(pow(2, bih.biBitCount) - 1 - imagep[i][j]);
        }
    result->push_back(to_string(int(ix)));
    result->push_back(to_string(int(iy)));
    result->push_back(to_string(ix/(getHeight()*getHeight() + getWidth()*getWidth())));
    result->push_back(to_string(iy/(getHeight()*getHeight() + getWidth()*getWidth())));
    return *result;
}

BMP& BMP::operator=(BMP &bmpfile){
    if (bmpfile.imagep != nullptr && bmpfile.imagergb != nullptr)
        throw 1;

    if (!this->empty)
        this->~BMP();

    this->bih = bmpfile.bih;
    this->bfh = bmpfile.bfh;
    this->empty = bmpfile.empty;
    if (bmpfile.palette != nullptr){
        uint pallen = bih.biClrUsed?bih.biClrUsed:pow(2,bih.biBitCount);
        this->palette = new RGB[pallen];
        for (uint i = 0; i < pallen; ++i){
            this->palette[i] = bmpfile.palette[i];
        }
    }
    if (bmpfile.imagep != nullptr){
        this->imagep = new uchar*[getHeight()];
        for (uint i = 0; i < getHeight(); ++i){
            this->imagep[i] = new uchar[getWidth()];
            for (uint j = 0; j < getWidth(); ++j)
                this->imagep[i][j] = bmpfile.imagep[i][j];
        }
    }
    if (bmpfile.imagergb != nullptr){
        this->imagergb = new RGB*[getHeight()];
        for (uint i = 0; i < getHeight(); ++i){
            this->imagergb[i] = new RGB[getWidth()];
            for (uint j = 0; j < getWidth(); ++j)
                this->imagergb[i][j] = bmpfile.imagergb[i][j];
        }
    }

    return *this;
}

trio& BMP::separatecolors(){
    if (this->bih.biBitCount <= 8)
        throw "No colors";

    trio * result = new trio;
    result->blue = new BMP(*this);
    result->blue->monochrome();
    result->green = new BMP(*result->blue);
    result->red = new BMP(*result->blue);
    for (uint i = 0; i < getHeight(); ++i)
        for (uint j = 0; j < getWidth(); ++j){
            result->blue->imagep[i][j] = this->imagergb[i][j].blue;
            result->green->imagep[i][j] = this->imagergb[i][j].green;
            result->red->imagep[i][j] = this->imagergb[i][j].red;
        }
    for (int i = 0; i < 256; ++i){
       result->blue->palette[i].blue = uint8_t(i);
       result->blue->palette[i].red = 0;
       result->blue->palette[i].green = 0;
       result->red->palette[i].red = uint8_t(i);
       result->red->palette[i].blue = 0;
       result->red->palette[i].green = 0;
       result->green->palette[i].green = uint8_t(i);
       result->green->palette[i].red = 0;
       result->green->palette[i].blue= 0;
    }
    return *result;
}

void BMP::combinecolors(trio& colors){
    RGB** newimage = new RGB*[colors.red->getHeight()];
    for (uint i = 0; i < colors.red->getHeight(); ++i)
        newimage[i] = new RGB[colors.red->getWidth()];
    for (uint i = 0; i < colors.red->getHeight(); ++i)
        for (uint j = 0; j < colors.red->getWidth(); ++j){
            newimage[i][j].blue = colors.blue->imagep[i][j];
            newimage[i][j].green = colors.green->imagep[i][j];
            newimage[i][j].red = colors.red->imagep[i][j];
        }
    this->writeimage(newimage, colors.red->getHeight(), colors.red->getWidth());
}

// отражение по вертикали
BMP& BMP::mirror_vertical(){
    BMP *mirrorimg = new BMP(*this);
    for (uint i = 0; i < getHeight(); ++i)
    {
        for (uint j = 0; j < getWidth(); ++j)
        {
            if (bih.biBitCount > 8)
                mirrorimg->imagergb[i][j] = this->imagergb[getHeight() - i - 1][j];
            else
                mirrorimg->imagep[i][j] = this->imagep[getHeight() - i - 1][j];
        }
    }
    return *mirrorimg;
}

// копирование части картинки
BMP& BMP::copypart(uint x, uint y, uint sizex, uint sizey)
{
    if (x > getWidth())
        throw 1;
    if (y > getHeight())
        throw 2;

    if (sizex == 0)
        sizex = getWidth() - x;
    if (sizey == 0)
        sizey = getHeight() - y;
    if (sizex + x > getWidth())
        sizex = getWidth() - x;
    if (sizey + y > getHeight())
        sizey = getHeight() - y;

    BMP *result = new BMP(*this);

    *result = result->mirror_vertical();

    if (this->bih.biBitCount > 8)
    {
        RGB **newimage = new RGB*[sizey];
        for (uint i = y; i < y + sizey; ++i)
        {
            newimage[i - y] = new RGB[sizex];
            for (uint j = x; j < x + sizex; ++j)
            {
                newimage[i-y][j-x] = result->imagergb[i][j];
            }
        }
        result->writeimage(newimage, sizey, sizex, bih.biBitCount);
    }
    else
    {
        uchar **newimage = new uchar*[sizey];
        for (uint i = y; i < y + sizey; ++i)
        {
            newimage[i - y] = new uchar[sizex];
            for (uint j = x; j < x + sizex; ++j)
            {
                newimage[i-y][j-x] = result->imagep[i][j];
            }
        }
        result->writeimage(newimage, sizey, sizex, this->palette, this->bih.biClrUsed, bih.biBitCount);
    }

    *result = result->mirror_vertical();

    return *result;
}

// рассчет профилей
pair<vector<int64_t>,vector<int64_t>>& BMP::profiles()
{
    pair<vector<int64_t>,vector<int64_t>> *result = new pair<vector<int64_t>,vector<int64_t>>;
    if (bih.biBitCount>8)
        return *result;

    BMP workimg = this->mirror_vertical();

    result->first = vector<int64_t>(getHeight());
    result->second = vector<int64_t>(getWidth());

    for (uint i = 0; i < getWidth(); ++i)
    {
        result->second.at(i) = 0;
    }

    for (uint i = 0; i < getHeight(); ++i)
    {
        result->first.at(i) = 0;
        for (uint j = 0; j < getWidth(); ++j)
        {
            result->first.at(i) += pow(2, bih.biBitCount) - workimg.imagep[i][j] - 1;
            result->second.at(j) += pow(2, bih.biBitCount) - workimg.imagep[i][j] - 1;
        }
    }
    return *result;
}

// разделение картинки на строки
vector<pair<pair<uint, uint>, pair<uint, uint>>>& BMP::split_into_lines_by_profiles(pair<vector<int64_t>, vector<int64_t>>& profiles)
{
    vector<pair<pair<uint, uint>, pair<uint, uint>>> *result = new vector<pair<pair<uint, uint>, pair<uint, uint>>>;
    pair<pair<uint,uint>,pair<uint,uint>> line;

    int64_t min = profiles.first.at(0);

    for (auto profcolumn : profiles.first)
    {
        if (min > profcolumn)
        {
            min = profcolumn;
        }
    }

    uint textstart = 0, textend = getHeight() - 1;

    while (textstart < getHeight() && profiles.first.at(textstart) < min + pow(2, bih.biBitCount))
    {
        ++textstart;
    }
    while (textend > 0 && profiles.first.at(textend) < min + pow(2, bih.biBitCount))
    {
        --textend;
    }

    int maxletterhigh = 0, currentlen = 0;
    for (uint i = textstart; i < textend + 1; ++i)
    {
        if (profiles.first.at(i) >= min + pow(2, bih.biBitCount))
        {
            ++currentlen;
        }
        else
        {
            if (currentlen > maxletterhigh)
            {
                maxletterhigh = currentlen;
            }
            currentlen = 0;
        }
    }
    if (currentlen > maxletterhigh)
    {
        maxletterhigh = currentlen;
    }

    profiles.first.insert(profiles.first.begin(), 0);

    line.first.first = 0;
    line.first.second = textend + 1;
    line.second.first = getWidth() - 1;
    line.second.second = textend + 1;

    for (int64_t i = textend + 1; i >= textstart; --i)
    {
        if (profiles.first.at(size_t(i)) < min + pow(2, bih.biBitCount))
        {
            if (line.first.second == line.second.second)
            {
                line.first.second = uint(i>0?i:1 - 1);
                line.second.second = uint(i>0?i:1 - 1);
            }
            else
            {
                bool isnotEOL = false;
                for (uint j = uint(i-1); j > 0 && j > i - maxletterhigh/8; --j)
                {
                    if (profiles.first.at(j) > pow(2, bih.biBitCount))
                    {
                        isnotEOL = true;
                    }
                }
                if (!isnotEOL)
                {
                    pair<vector<int64_t>,vector<int64_t>> lineprofile = copypart(0, line.second.second, 0, line.first.second - line.second.second + 1).profiles();
                    int64_t min2 = lineprofile.second.at(0);
                    int64_t max2 = 0;
                    for (auto profline : lineprofile.second)
                    {
                        if (min2 > profline)
                        {
                            min2 = profline;
                        }
                        if (max2 < profline)
                        {
                            max2 = profline;
                        }
                    }
                    if (min2 > max2/10)
                    {
                        min2 = max2/20;
                    }

                    while (line.second.first > 0 && lineprofile.second.at(line.second.first) < min + pow(2, bih.biBitCount))
                    {
                        --line.second.first;
                    }
                    while (line.first.first < getHeight() && lineprofile.second.at(line.first.first) < min + pow(2, bih.biBitCount))
                    {
                        ++line.first.first;
                    }
                    if (profiles.first.at(line.first.second + 1) < min + pow(2, bih.biBitCount))
                        --line.first.second;
                    result->push_back(line);
                    line.first.first = 0;
                    line.first.second = uint(i - 1);
                    line.second.first = getWidth() - 1;
                    line.second.second = uint(i - 1);
                }
                else
                {
                    line.second.second = uint(i - 1);
                }
            }
        }
        else
        {
            line.second.second = uint(i - 1);
        }
    }
    return *result;
}

// разделение строки на буквы
vector<pair<pair<uint, uint>, pair<uint, uint>>>& BMP::split_into_letters_by_profiles(pair<vector<int64_t>, vector<int64_t>>& profiles)
{
    vector<pair<pair<uint, uint>, pair<uint, uint>>> *result = new vector<pair<pair<uint, uint>, pair<uint, uint>>>;
    pair<pair<uint,uint>,pair<uint,uint>> letter;
    letter.first.first = 0;
    letter.first.second = 0;
    letter.second.first = 0;
    letter.second.second = getHeight() - 1;

    // нахождение минимального уровня шума в строчке
    int64_t min = profiles.second.at(0);
    for (auto profcolumn : profiles.second)
    {
        if (min > profcolumn)
        {
            min = profcolumn;
        }
    }

    profiles.second.push_back(0);

    for (uint i = 0; i < getWidth()+1; ++i)
    {
        if (profiles.second.at(i) < min + pow(2, bih.biBitCount)*1)
        {
            if (letter.first.first == letter.second.first)
            {
                letter.first.first = i;
                letter.second.first = i;
            }
            else
            {
                pair<vector<int64_t>,vector<int64_t>> letterprofile = copypart(letter.first.first,0,letter.second.first - letter.first.first + 1, 0).profiles();
                int64_t min2 = letterprofile.first.at(0);
                int64_t max2 = 0;

                // нахождение минимального уровня шума в букве и максимальное значение профиля для корректировки шума
                for (auto profline : letterprofile.first)
                {
                    if (min2 > profline)
                    {
                        min2 = profline;
                    }
                    if (max2 < profline)
                    {
                        max2 = profline;
                    }
                }
                if (min2 > max2/10)
                {
                    min2 = max2/20;
                }

                // нахождение верхней и нижней границ буквы
                while (letter.second.second > 0 && letterprofile.first.at(letter.second.second) < min2 + 256)
                {
                    --letter.second.second;
                }
                while (letter.first.second < getHeight() && letterprofile.first.at(letter.first.second) < min2 + 256)
                {
                    ++letter.first.second;
                }
                if (profiles.second.at(letter.first.first) < min + pow(2, bih.biBitCount)*1)
                    ++letter.first.first;
                result->push_back(letter);
                letter.first.first = i;
                letter.first.second = 0;
                letter.second.first = i;
                letter.second.second = getHeight() - 1;
            }
        }
        else
        {
            letter.second.first = i;
        }
    }
    return *result;
}

BMP& BMP::combine_img_part_and_profile(uint x, uint y, uint sizex, uint sizey)
{
    BMP * result = new BMP;
    pair<uint, uint> a(x, y);
    pair<uint, uint> b(sizex, sizey);
    pair<pair<uint, uint>, pair<uint, uint>> c(a, b);
    *result = this->combine_img_part_and_profile(c);
    return *result;
}

// совмещение части картини и ее профиля
BMP& BMP::combine_img_part_and_profile(pair<pair<uint, uint>, pair<uint, uint> > &border)
{
    if (this->bih.biBitCount > 8)
        throw 1;

    BMP part = copypart(border.first.first, border.first.second, border.second.first - border.first.first + 1, border.second.second - border.first.second + 1);
    BMP* result = new BMP(part);
    result->mirror_vertical();
    auto profiles = result->profiles();
    uint maxlen_horizontal = 0;
    uint maxlen_vertical = 0;
    // нахождение максимально высоких профилей
    for (auto horprof: profiles.first)
    {
        if (horprof/256 > maxlen_horizontal)
            maxlen_horizontal = uint(horprof/256);
    }
    for (auto vertprof: profiles.second)
    {
        if (vertprof/256 > maxlen_vertical)
            maxlen_vertical = uint(vertprof/256);
    }
    uint newHeight = part.getHeight() + maxlen_vertical;
    uint newWidth = part.getWidth() + maxlen_horizontal;
    // инициалзация комбинированной картинки
    unsigned char** newimage = new unsigned char*[newHeight];
    for (uint i = 0; i < newHeight; ++i)
    {
        newimage[i] = new unsigned char[newWidth];
        for (uint j = 0; j < newWidth; ++j)
        {
            if (i >= maxlen_vertical)
            {
                if (j < maxlen_horizontal)
                {
                    if (profiles.first[i - maxlen_vertical]/256 > j)
                        newimage[i][j] = 0;
                    else
                        newimage[i][j] = 255;
                }
                else
                    newimage[i][j] = part.imagep[i - maxlen_vertical][j - maxlen_horizontal];
            }
            else
                if (j >= maxlen_horizontal){
                    if (profiles.second[j - maxlen_horizontal]/256 > i)
                        newimage[i][j] = 0;
                    else
                        newimage[i][j] = 255;
                }
                else
                    newimage[i][j] = 255;
        }
    }
    result->writeimage(newimage, newHeight, newWidth, part.palette, part.bih.biClrUsed, part.bih.biBitCount);
    return *result;
}

vector<vector<uint>>& BMP::matrix_of_spatial_adjacency()
{
    if (bih.biBitCount > 8)
        throw 1;

    vector<vector<uint>> *result = new vector<vector<uint>>(pow(2, bih.biBitCount), vector<uint>(pow(2, bih.biBitCount), 0));
    for (uint i = 0; i < getHeight(); ++i)
    {
        for (uint j = 0; j < getWidth(); ++j)
        {
            if (i > 0) ++result->at(imagep[i][j]).at(imagep[i - 1][j]);
            if (j > 0) ++result->at(imagep[i][j]).at(imagep[i][j - 1]);
            if (i < getHeight() - 1) ++result->at(imagep[i][j]).at(imagep[i + 1][j]);
            if (j < getWidth() - 1) ++result->at(imagep[i][j]).at(imagep[i][j + 1]);
        }
    }
    return *result;
}

vector<vector<double>>& BMP::normal_matrix_of_spatial_adjacency()
{
    if (bih.biBitCount > 8)
        throw 1;

    vector<vector<double>> *result = new vector<vector<double>>(pow(2, bih.biBitCount), vector<double>(pow(2, bih.biBitCount), 0));
    auto matrix = matrix_of_spatial_adjacency();
    uint all_connections = 0;
    for (uint i = 0; i < pow(2, bih.biBitCount); ++i)
    {
        for (uint j = 0; j < pow(2, bih.biBitCount); ++j)
        {
            all_connections += matrix.at(i).at(j);
        }
    }
    for (uint i = 0; i < pow(2, bih.biBitCount); ++i)
    {
        for (uint j = 0; j < pow(2, bih.biBitCount); ++j)
        {
            result->at(i).at(j) = double(matrix.at(i).at(j))/all_connections;
        }
    }
    return *result;
}

vector<vector<uint>> &BMP::matrix_of_series_length()
{
    if (bih.biBitCount > 8)
        throw 1;

    vector<vector<uint>> *result = new vector<vector<uint>>(pow(2, bih.biBitCount), vector<uint>(max(getHeight(), getWidth()) + 1, 0));
    // поиск длинн по горизонтали
    for (uint i = 0; i < getHeight(); ++i)
    {
        uint color = 256;
        uint counter = 0;
        for (uint j = 0; j < getWidth(); ++j)
        {
            if (imagep[i][j] == color){
                ++counter;
            }
            else{
                if (color < 256 && counter > 1)
                {
                    ++result->at(color).at(counter);
                    color = imagep[i][j];
                    counter = 1;
                }
                else {
                    color = imagep[i][j];
                    counter = 1;
                }
            }
        }
        if (color < 256 && counter > 1)
        {
            ++result->at(color).at(counter);
        }
    }
    // поиск длинн по вертикали
    for (uint i = 0; i < getWidth(); ++i)
    {
        uint color = 256;
        uint counter = 0;
        for (uint j = 0; j < getHeight(); ++j)
        {
            if (imagep[j][i] == color){
                ++counter;
            }
            else{
                if (color < 256 && counter > 1)
                {
                    ++result->at(color).at(counter);
                    color = imagep[j][i];
                    counter = 1;
                }
                else {
                    color = imagep[j][i];
                    counter = 1;
                }
            }
        }
    }
    // поиск длинн по диагнали вниз направо
    for (uint i = 0; i < getHeight() + getWidth() - 2; ++i)
    {
        uint color = 256;
        uint counter = 0;
        uint x, y;
        if (i < getHeight())
        {
            y = 0;
            x = i;
        }
        else {
            y = i - getHeight() + 1;
            x = getHeight() - 1;
        }
        for (uint j = 0; j < min(
                 min(i + 1, getWidth() + getHeight() - 1 - i),
                 min(getWidth(), getHeight())); ++j)
        {
            if (imagep[x][y] == color){
                ++counter;
            }
            else{
                if (color < 256 && counter > 1)
                {
                    ++result->at(color).at(counter);
                    color = imagep[x][y];
                    counter = 1;
                }
                else {
                    color = imagep[x][y];
                    counter = 1;
                }
            }
            --x; ++y;
        }
    }
    // поиск длинн по диагнали вверх вправо
    for (uint i = 0; i < getHeight() + getWidth() - 2; ++i)
    {
        uint color = 256;
        uint counter = 0;
        uint x, y;
        if (i < getHeight())
        {
            y = 0;
            x = i;
        }
        else {
            y = i - getHeight() + 1;
            x = 0;
        }
        for (uint j = 0; x < getHeight() && y < getWidth(); ++j)
        {
            if (imagep[x][y] == color){
                ++counter;
            }
            else{
                if (color < 256 && counter > 1)
                {
                    ++result->at(color).at(counter);
                    color = imagep[x][y];
                    counter = 1;
                }
                else {
                    color = imagep[x][y];
                    counter = 1;
                }
            }
            ++x; ++y;
        }
    }
    return *result;
}

BMP& BMP::logarifmic_improvement()
{
    BMP *result = new BMP(*this);
    uchar fmax = 0;
    uchar fmin = 255;
    int64_t all = 0;
    uint L = pow(2, bih.biBitCount - 1);
    double mean = 0;
    for (uint i = 0; i < getHeight(); ++i)
        for (uint j = 0; j < getWidth(); ++j)
        {
            if (this->imagep[i][j] < fmin) fmin = this->imagep[i][j];
            if (this->imagep[i][j] > fmax) fmax = this->imagep[i][j];
            all += this->imagep[i][j];
        }
    mean = double(all)/getHeight()/getWidth();
    double PR = (2>fmax - mean?2:fmax - mean);
    double NR = (2>mean - fmin?2:mean - fmin);
    double PA = double(L)/log(PR);
    double NA = double(L)/log(NR);
    for (uint i = 0; i < getHeight(); ++i)
        for (uint j = 0; j < getWidth(); ++j){
            if (double(this->imagep[i][j]) - mean >= 1)
            {
                result->imagep[i][j] = uchar(int(mean + PA*log(double(this->imagep[i][j]) - mean)));
            }
            else if (double(this->imagep[i][j]) - mean <= -1) {
                result->imagep[i][j] = uchar(int(mean - NA*log(abs(double(this->imagep[i][j]) - mean))));
            }
            else {
                result->imagep[i][j] = uchar(int(mean));
            }
        }
    return *result;
}

BMP& BMP::gisteq()
{
    BMP *result = new BMP(*this);
    uint twopowl = pow(2, bih.biBitCount);
    uint N0 = getHeight()*getWidth()/twopowl;
    vector<uint> bright_levels(twopowl, 0);
    vector<uchar> change_bright_levels(twopowl, 0);
    vector<uint> new_bright_levels(twopowl, 0);
    for (uint i = 0; i < getHeight(); ++i)
        for (uint j = 0; j < getWidth(); ++j)
        {
            ++bright_levels.at(result->imagep[i][j]);
        }
    uchar counter = 0;
    for (uint i = 0; i < twopowl; ++i)
    {

            if (new_bright_levels[counter] + bright_levels[i] < N0*1.05)
            {
                new_bright_levels[counter] += bright_levels[i];
                change_bright_levels[i] = counter;
            }
            else {
                if (new_bright_levels[counter] > 0)
                    ++counter;
                new_bright_levels[counter] = bright_levels[i];
                change_bright_levels[i] = counter;
                counter += bright_levels[i]/N0;
            }

            if (new_bright_levels[counter] > N0*0.95)
                ++counter;

    }
    for (uint i = 0; i < getHeight(); ++i)
        for (uint j = 0; j < getWidth(); ++j)
        {
            result->imagep[i][j] = change_bright_levels.at(result->imagep[i][j]);
        }
    return *result;
}
