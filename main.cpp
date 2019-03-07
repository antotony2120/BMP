#include<iostream>
#include"bmp.h"

using namespace std;

int main(){
    BMP img;
    img.inputfromfile("test.bmp");
//    unsigned int n = 4;
//    img.downsampling(n);
    img.resize(3,4);
    //img.monochrome();
    //img.downsampling(2);
    //img.resizewithupanddown(1,2);
    img.outpputininfile("clip.bmp");
    return 0;
}
