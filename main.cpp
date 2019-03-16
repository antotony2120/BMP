#include<iostream>
#include<fstream>
#include"bmp.h"

using namespace std;

int main(){
    BMP img;
    img.inputfromfile("clip.bmp");

//    unsigned int n = 4;
//    img.downsampling(n);5
    //img.resize(3,4);
    //img.monochrome();
    //img.resizewithupanddown(8,3);
    img.downsampling(2);
    img.outpputininfile("clip.bmp");
    return 0;
}
