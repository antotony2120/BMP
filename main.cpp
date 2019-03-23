#include<iostream>
#include<fstream>
#include"bmp.h"

using namespace std;

int analysisalphabet(){
    BMP img;
    ofstream out;
    out.open("C://Users//Anton//Code//C++//BMP//BMP//analysis.csv");
    out << "N;weight;weightrel;x y;x y rel;ix;iy;ix iy rel;\n";
    for (int i = 1; i < 34; ++i){
        img.inputfromfile("C://Users//Anton//Code//C++//BMP//BMP//Alphabet//" + to_string(i) + ".bmp");
        out << to_string(i) + ";";
        out << img.analysisthirdtask();
    }
    out.close();
    return 0;
}

int main(){
    analysisalphabet();
    return 0;
}
