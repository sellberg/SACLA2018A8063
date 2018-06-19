#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string.h>

using namespace std;

void File_number();
void Read_file_name();
void Fit2d_input(int i);

char **bin_file_name_front;
char **bin_file_name_side;

//char file_name_temp[100];
//char file_name[100];
int number_of_files=0;

int para[4] = 
{2399,
2399,
50,
50
};// #of x_pixel, #of y_pixel, x_pixel size, y_pixel size for fron detector
double para2[5] = 
{
1199.582,
1200.499,
62.5,
11.0,
0.1
};// beam center x, beam center y, detector distance, X-ray energy, 2theta steps for front detector
int para3[4] = 
{1024,
512,
50,
50
};//  #of x_pixel, #of y_pixel, x_pixel size, y_pixel size for side detector
double para4[5] =
{
1049.582,
245.499,
62.5,
11.0,
0.1
};// beam center x, beam center y, detector distance, X-ray energy, 2theta steps for side detector



int main(){
    system("ls ./bin_front/*.bin > bin_file_name_front.txt");
    system("ls ./bin_side/*.bin > bin_file_name_side.txt");
    system("mkdir chi_front");
    system("mkdir chi_side");

    File_number();
    Read_file_name();
   
    for (int i=0;i<number_of_files;i++)
    {
        Fit2d_input(i);
        system("./fit2d_old -key -nograph <fit2d_input_front.txt >log");
        system("./fit2d_old -key -nograph <fit2d_input_side.txt >log");
    }

}


void File_number(){
    char temp[255];
    ifstream in;
    in.open("bin_file_name_front.txt");
    while (true){
        in.getline(temp,255,'\n');
        if(strlen(temp)<5)break;
        number_of_files++;
    }
    in.close();
}

void Read_file_name(){

//    char temp[255];
    bin_file_name_front = new char*[number_of_files];
    bin_file_name_side = new char*[number_of_files];

    for (int i=0;i<number_of_files;i++)
    {
        bin_file_name_front[i] = new char[100];
        bin_file_name_side[i] = new char[100];
    }

    ifstream in2;
    in2.open("bin_file_name_front.txt");
    for (int i=0;i<number_of_files;i++)
    {
        in2.getline(bin_file_name_front[i],100,'\n');
        cout<<bin_file_name_front[i]<<endl;
    }
    in2.close();

    ifstream in20;
    in20.open("bin_file_name_side.txt");
    for (int i=0;i<number_of_files;i++)
    {
        in20.getline(bin_file_name_side[i],100,'\n');
        cout<<bin_file_name_side[i]<<endl;
    }



}

void Fit2d_input(int i){

    char output_file_name_front[50];

    if (i<9)
        sprintf (output_file_name_front, "./chi_front/0000%d.chi",i+1);
    if (i>8&&i<99)
        sprintf (output_file_name_front, "./chi_front/000%d.chi",i+1);
    if (i>98&&i<999)
        sprintf (output_file_name_front, "./chi_front/00%d.chi",i+1);
    if (i>998&&i<9999)
        sprintf (output_file_name_front, "./chi_front/0%d.chi",i+1);
    if (9998<i)
        sprintf (output_file_name_front, "./chi_front/%d.chi",i+1);


    cout<<"Now making  "<<output_file_name_front<<endl;

    ofstream fit2d;
    fit2d.open("fit2d_input_front.txt");
//    fit2d<<para[0]<<endl<<para[1]<<endl<<"no"<<endl<<"input"<<endl<<"tiff"<<endl<<bin_file_name_front[i]<<endl;
//    fit2d<<"1"<<endl<<"1"<<endl<<"1"<<endl<<"1"<<endl<<para[0]<<endl<<para[1]<<endl;

    fit2d<<para[0]<<endl<<para[1]<<endl<<"no"<<endl<<"input"<<endl<<"bin"<<endl<<bin_file_name_front[i]<<endl;
    fit2d<<para[0]<<endl<<para[1]<<endl<<"REAL (4-BYTE IEEE)"<<endl;
    fit2d<<"fit"<<endl<<"threshold"<<endl<<"yes"<<endl<<"1"<<endl;
    fit2d<<"powder diffraction"<<endl<<"yes"<<endl<<para2[0]<<endl<<para2[1]<<endl<<para[2]<<endl<<para[3]<<endl<<para2[2]<<endl<<"0"<<endl<<"0"<<endl<<"yes"<<endl<<"0.99"<<endl;
    fit2d<<"PARTIAL POWDER (2-THETA SCAN)"<<endl<<"yes"<<endl<<para2[4]<<endl<<"no"<<endl<<"no"<<endl<<"exit"<<endl;
    fit2d<<"exchange"<<endl<<"OUTPUT DATA"<<endl<<"CHIPLOT"<<endl<<output_file_name_front<<endl<<"yes"<<endl<<"1"<<endl<<"exit"<<endl<<"yes"<<endl;

    fit2d.close();


    char output_file_name_side[50];

    if (i<9)
    sprintf (output_file_name_side, "./chi_side/0000%d.chi",i+1);
    if (i>8&&i<99)
    sprintf (output_file_name_side, "./chi_side/000%d.chi",i+1);
    if (i>98&&i<999)
    sprintf (output_file_name_side, "./chi_side/00%d.chi",i+1);
    if (i>998&&i<9999)
    sprintf (output_file_name_side, "./chi_side/0%d.chi",i+1);
    if (9998<i)
    sprintf (output_file_name_side, "./chi_side/%d.chi",i+1);


    cout<<"Now making  "<<output_file_name_side<<endl;

    ofstream fit2d_side;
    fit2d_side.open("fit2d_input_side.txt");
//    fit2d_side<<para3[0]<<endl<<para3[1]<<endl<<"no"<<endl<<"input"<<endl<<"tiff"<<endl<<bin_file_name_side[i]<<endl;
//    fit2d_side<<"1"<<endl<<"1"<<endl<<"1"<<endl<<"1"<<endl<<para3[0]<<endl<<para3[1]<<endl;

    fit2d_side<<para3[0]<<endl<<para3[1]<<endl<<"no"<<endl<<"input"<<endl<<"bin"<<endl<<bin_file_name_side[i]<<endl;
    fit2d_side<<para3[0]<<endl<<para3[1]<<endl<<"REAL (4-BYTE IEEE)"<<endl;
    fit2d_side<<"fit"<<endl<<"threshold"<<endl<<"yes"<<endl<<"1"<<endl;
    fit2d_side<<"powder diffraction"<<endl<<"yes"<<endl<<para4[0]<<endl<<para4[1]<<endl<<para3[2]<<endl<<para3[3]<<endl<<para4[2]<<endl<<"0"<<endl<<"0"<<endl<<"yes"<<endl<<"0.99"<<endl;
    fit2d_side<<"PARTIAL POWDER (2-THETA SCAN)"<<endl<<"yes"<<endl<<para4[4]<<endl<<"no"<<endl<<"no"<<endl<<"exit"<<endl;
    fit2d_side<<"exchange"<<endl<<"OUTPUT DATA"<<endl<<"CHIPLOT"<<endl<<output_file_name_side<<endl<<"yes"<<endl<<"1"<<endl<<"exit"<<endl<<"yes"<<endl;

    fit2d_side.close();


}



