#include<iostream>
#include<fstream>
#include"bmp.h"
#include<bits/stdc++.h>

using namespace std;

const string PATH = "C://Users//Anton//Code//C++//BMP//Materials//";

int analysisalphabet(){
    BMP img;
    ofstream out;
    out.open(PATH + "analysis.csv");
    out << "N;weight;weightrel;x;y;x rel;y rel;ix;iy;ix rel;iy rel;\n";
    for (int i = 1; i < 34; ++i){
        img.inputfromfile(PATH + "GeneratedAlphabet//" + to_string(i) + ".bmp");
        img.binarization();
        out << to_string(i) + ";";
        auto analis = img.analysisthirdtask();
        for (auto oneline: analis)
            out << oneline << ";";
        out << "\n";
    }
    out.close();
    return 0;
}

void sep(string obj){
    BMP img("C://Users//Anton//Code//C++//BMP//BMP//" + obj + ".bmp");
    trio sep = img.separatecolors();
    sep.blue->outpputininfile("C://Users//Anton//Code//C++//BMP//BMP//" + obj + "blue.bmp");
    sep.red->outpputininfile("C://Users//Anton//Code//C++//BMP//BMP//" + obj + "red.bmp");
    sep.green->outpputininfile("C://Users//Anton//Code//C++//BMP//BMP//" + obj + "green.bmp");
}

void bord(string obj){
    BMP img("C://Users//Anton//Code//C++//BMP//BMP//" + obj + ".bmp");
    img.borderssobel();
    img.outpputininfile("C://Users//Anton//Code//C++//BMP//BMP//" + obj + "b.bmp");
}

void comb(string obj){
    BMP img1("C://Users//Anton//Code//C++//BMP//BMP//" + obj + "blueb.bmp");
    BMP img2("C://Users//Anton//Code//C++//BMP//BMP//" + obj + "redb.bmp");
    BMP img3("C://Users//Anton//Code//C++//BMP//BMP//" + obj + "greenb.bmp");
    trio mytrio;
    mytrio.blue = new BMP(img1);
    mytrio.red = new BMP(img2);
    mytrio.green = new BMP(img3);
    BMP img;
    img.combinecolors(mytrio);
    img.outpputininfile("C://Users//Anton//Code//C++//BMP//BMP//" + obj + "comb.bmp");
}

void splitbmplineintoletters(string obj)
{
    BMP img(PATH + obj);
    auto resultofsplit = img.split_into_letters_by_profiles(img.profiles());
    int i = 1;
    for (auto letter: resultofsplit)
    {
        cout << i++ << ": ";
        cout << "x1: " << letter.first.first << "; y1: " << letter.first.second;
        cout << "; x2: " << letter.second.first << "; y2: " << letter.second.second << ";\n";
    }
}

void profilesonletters()
{
    BMP img(PATH + "allalphabetinlinemono.bmp");
    auto split_result = img.split_into_letters_by_profiles(img.profiles());
    size_t count = split_result.size();
    for (size_t i = 1; i < count + 1; ++i)
    {
        BMP letprof = img.combine_img_part_and_profile(split_result.at(i - 1));
        letprof.outpputininfile(PATH + "GeneratedAlphbetWithProfiles//" + to_string(i) + ".bmp");
    }
}

void alpabetfromline()
{
    BMP img(PATH + "allalphabetinline.bmp");
    img.monochrome();
    auto split_result = img.split_into_letters_by_profiles(img.profiles());
    size_t count = split_result.size();
    for (size_t i = 1; i < count + 1; ++i)
    {
        BMP letprof = img.copypart(split_result.at(i - 1).first.first, split_result.at(i - 1).first.second, split_result.at(i - 1).second.first - split_result.at(i - 1).first.first + 1, split_result.at(i - 1).second.second - split_result.at(i - 1).first.second + 1);
        letprof.outpputininfile(PATH + "GeneratedAlphabet//" + to_string(i) + ".bmp");
    }
}

void profilesonletters2()
{
    BMP img(PATH + "checktext.bmp");
    img.monochrome();
    auto split_result = img.split_into_lines_by_profiles(img.profiles());
    try{
        system(("cd " + PATH + "&rmdir /S /Q TextLines").c_str());
    }
    catch(...){
        cout << "Папка не существует.";
    }
    system(("cd " + PATH + "&mkdir TextLines&cd TextLines").c_str());
    for (size_t i = 1; i < split_result.size() + 1; ++i)
    {
        BMP letprof = img.copypart(split_result.at(i - 1).first.first, split_result.at(i - 1).second.second, split_result.at(i - 1).second.first - split_result.at(i - 1).first.first + 1, split_result.at(i - 1).first.second - split_result.at(i - 1).second.second + 1);
        letprof.outpputininfile(PATH + "TextLines//" + to_string(i) + ".bmp");
        auto split_result2 = letprof.split_into_letters_by_profiles(letprof.profiles());
        system(("cd " + PATH + "TextLines&mkdir line" + to_string(i) + "&cd line" + to_string(i)).c_str());
        for (size_t j = 1; j < split_result2.size() + 1; ++j)
        {
            BMP letprof2 = letprof.combine_img_part_and_profile(split_result2.at(j - 1));
            letprof2.outpputininfile(PATH + "TextLines//line" + to_string(i) + "//" + to_string(j) + ".bmp");
        }
    }
}

double matrixAXM(vector<vector<double>> matrix)
{
    double result = 0;
    for (uint i = 0; i < matrix.size(); ++i)
    {
        for (uint j = 0; j < matrix.at(i).size(); ++j)
        {
            result += matrix.at(i).at(j)*matrix.at(i).at(j);
        }
    }
    return result;
}

double matrixSRE(vector<vector<uint>> matrix)
{
    double result = 0;
    uint K = 0;
    for (uint i = 0; i < matrix.size(); ++i)
    {
        for (uint j = 1; j < matrix.at(i).size(); ++j)
        {
            result += double(matrix.at(i).at(j))/j/j;
            K += matrix.at(i).at(j);
        }
    }
    return result/K;
}

template <typename T>
vector<vector<uchar>>& matrixto256(vector<vector<T>>& matrix)
{
    vector<vector<uchar>> *result = new vector<vector<uchar>>(matrix.size(), vector<uchar>(matrix.at(0).size(), 0));
    double maxval = 0;
    for (uint i = 0; i < matrix.size(); ++i)
        for (uint j = 0; j < matrix.at(i).size(); ++j)
            if (matrix.at(i).at(j) > maxval)
                maxval = matrix.at(i).at(j);

    for (uint i = 0; i < matrix.size(); ++i)
    {
        for (uint j = 1; j < matrix.at(i).size(); ++j)
        {
            result->at(i).at(j) = uchar(matrix.at(i).at(j)/maxval*255);
        }
    }
    return *result;
}

void zadanie7()
{
    BMP img(PATH + "texture4.bmp");
    img.monochrome();
    auto matrix = img.normal_matrix_of_spatial_adjacency();
    auto matrixlegth = img.matrix_of_series_length();
    double ASM = matrixAXM(matrix);
    double SRE = matrixSRE(matrixlegth);
    auto matrixin256 = matrixto256(matrix);
    auto matrixlengthin256 = matrixto256(matrixlegth);
    BMP adjeacency(matrixin256);
    BMP length(matrixlengthin256);
    adjeacency = adjeacency.mirror_vertical();
    length = length.mirror_vertical();
    adjeacency.outpputininfile(PATH + "adj.bmp");
    length.outpputininfile(PATH + "length.bmp");
    cout << ASM << endl << SRE;
}

void zadanie8()
{
    BMP img(PATH + "logimp.bmp");
    img.monochrome();
    img.outpputininfile(PATH + "logimpmono.bmp");
    img = img.gisteq();
    img.outpputininfile(PATH + "logimpresult.bmp");
}

void read_file()
{
    vector<vector<string>> standart;
    standart.clear();
    ofstream out;
    ofstream out2;
    out2.open(PATH + "result_string2.txt");
    out.open(PATH + "letterpredict2.txt");
//    alpabetfromline();
    string alphabet[33]= {"а", "б", "в", "г", "д", "е", "ё", "ж", "з", "и", "й", "к", "л", "м", "н", "о", "п",
                          "р", "с", "т", "у", "ф", "х", "ц", "ч", "ш", "щ", "ъ", "ы", "ь", "э", "ю", "я"};
    for (int i = 0; i < 33; ++i)
    {
        BMP letter(PATH + "GeneratedAlphabet//" + to_string(i + 1) + ".bmp");
        letter.binarization(0);
        auto letteranalis = letter.analysisthirdtask();
        letteranalis.insert(letteranalis.begin(), alphabet[i]);
        standart.push_back(letteranalis);
    }
    BMP img(PATH + "text2.bmp");
    img.monochrome();
    auto split_result = img.split_into_lines_by_profiles(img.profiles());
    int counter = 1;
    string result_string = "";
    for (size_t i = 1; i < split_result.size() + 1; ++i)
    {
        BMP letprof = img.copypart(split_result.at(i - 1).first.first, split_result.at(i - 1).second.second, split_result.at(i - 1).second.first - split_result.at(i - 1).first.first + 1, split_result.at(i - 1).first.second - split_result.at(i - 1).second.second + 1);
        auto split_result2 = letprof.split_into_letters_by_profiles(letprof.profiles());
        for (size_t j = 1; j < split_result2.size() + 1; ++j)
        {
            BMP letprof2 = letprof.copypart(split_result2.at(j - 1).first.first, split_result2.at(j - 1).first.second, split_result2.at(j - 1).second.first - split_result2.at(j - 1).first.first + 1, split_result2.at(j - 1).second.second - split_result2.at(j - 1).first.second + 1);
            letprof2.binarization(0);
            vector<string> letterintext = letprof2.analysisthirdtask();
            pair<string, double> value[33];
            for (size_t k = 0; k < 33; ++k)
            {
                double distance = (stod(letterintext[4]) - stod(standart.at(k).at(5)))*(stod(letterintext[4]) - stod(standart.at(k).at(5)))
                        + (stod(letterintext[5]) - stod(standart.at(k).at(6)))*(stod(letterintext[5]) - stod(standart.at(k).at(6)))
                        + (stod(letterintext[1]) - stod(standart.at(k).at(2)))*(stod(letterintext[1]) - stod(standart.at(k).at(2)))
                        + (stod(letterintext[8]) - stod(standart.at(k).at(9)))*(stod(letterintext[8]) - stod(standart.at(k).at(9)))
                        + (stod(letterintext[9]) - stod(standart.at(k).at(10)))*(stod(letterintext[9]) - stod(standart.at(k).at(10)));
                value[k].first = standart.at(k).at(0);
                value[k].second = exp(-distance/4);
            }
            for (int k = 0; k < 33; ++k){
                for (int l = 0; l < 32; ++l){
                    if (value[l].second < value[l + 1].second){
                        pair<string, double> c = value[l];
                        value[l] = value[l+1];
                        value[l + 1]= c;
                    }
                }
            }
            out << counter++ << ": [";
            for (int k = 0; k < 33; ++k){
                out << "(\"" << value[k].first << ", " << value[k].second << ")";
                if (k < 32)
                    out << ", ";
            }
            out << "]\n";
            out2 << value[0].first;
            result_string += value[0].first;
        }
    }
    out.close();
    out2.close();
}

int main(){
//    alpabetfromline();
//    analysisalphabet();
//    comb("woman");
//    bord("womanblue");
//    bord("womangreen");
//    bord("womanred");
//    comb("woman");
//    splitbmplineintoletters("allalphabetinline.bmp");
//    BMP img(PATH + "text2.bmp");
//    img.monochrome();
//    img.split_into_lines_by_profiles(img.profiles());
//    profilesonletters2();
//    read_file();
//    zadanie7();
    zadanie8();
//    BMP img(PATH + "allalphabetinline.bmp");
//    img.monochrome();
//    pair<uint, uint> a(0, 0);
//    pair<uint, uint> b(-1, -1);
//    pair<pair<uint, uint>, pair<uint, uint>> c(a, b);
//    img = img.combine_img_part_and_profile(c);
//    img.outpputininfile(PATH + "testresult.bmp");
//    auto split_result = img.split_into_lines_by_profiles(img.profiles());
//    img = img.copypart(split_result.at(0).first.first, split_result.at(0).second.second, split_result.at(0).second.first - split_result.at(0).first.first + 1, split_result.at(0).first.second - split_result.at(0).second.second + 1);
//    img.outpputininfile(PATH + "allalphabetinline2.bmp");
    return 0;
}
