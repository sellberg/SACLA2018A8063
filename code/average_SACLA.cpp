#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string.h>

#define std_cal 1

using namespace std;

void File_number();
void Read_file_name();

void Q_number();
void Chi_read();
void sorting();
void make_output();

//void bad_image_plot();

char **chi_file_name_front;
char **chi_file_name_side;

int number_of_files=0;
int q_line=0;
int q_line_side=0;

int number_of_water=0;
int number_of_background=0;
int number_of_ice=0;

double **intensity_front;
double **intensity_side;
double *summary_front[8]; // q 2theta av_water std_water av_back std_back av_ice std_ice
double *summary_side[8];
double *each_image[2];

double integ_water=0;
double integ_ice=0;

double criteria1 = 100000;
double criteria2 = 10;

double varience_front[3] = {0,0,0};
double varience_side[3] = {0,0,0};
double std_front[3] = {0,0,0};
double std_side[3] ={0,0,0};


int main(){
    system("ls ./chi_front/*.chi > chi_file_name_front.txt");
    system("ls ./chi_side/*.chi >chi_file_name_side.txt");
    system("mkdir average");

    File_number();

    cout<<"File_number_OK   "<<number_of_files<<endl;

    Read_file_name();

    cout<<"real_file_name_OK    "<<chi_file_name_front[5]<<endl;

    Q_number();

    cout<<"q_number_OK"<<endl;

    intensity_front = new double*[number_of_files+2];
    for(int i=0;i<number_of_files+2;i++)
        intensity_front[i] = new double[q_line];

    intensity_side = new double*[number_of_files+2];
    for(int i=0;i<number_of_files+2;i++)
        intensity_side[i] = new double[q_line_side];

    for(int i=0;i<2;i++)
        each_image[i] = new double[number_of_files];

    for (int i=0;i<8;i++)
    {
        summary_front[i] = new double[q_line];
        summary_side[i] = new double[q_line_side];
    }

    cout<<"before_chi_read_OK"<<endl;
    Chi_read();
    cout<<"chi_read_OK"<<endl;
    cout<<intensity_front[0][100]<<"  "<<intensity_front[1][100]<<" "<<intensity_front[2][100]<<endl;
    sorting();
    cout<<"sorting_OK"<<endl;

    make_output();
    cout<<"make_ouotput OK"<<endl;

//    bad_image_plot();
   
    cout<<number_of_files<<endl;
    cout<<q_line<<endl;
    cout<<q_line_side<<endl;

    cout<<endl<<endl<<"***** In summary *****"<<endl;
    cout<<"- Total # of images: "<<number_of_files<<" images"<<endl;
    cout<<"- Water: "<<number_of_water<<" images ("<<(double)number_of_water/number_of_files*100<<" %)"<<endl;
    cout<<"- Background: "<<number_of_background<<" images ("<<(double)number_of_background/number_of_files*100<<" %)"<<endl;
    cout<<"- Ice: "<<number_of_ice<<" images ("<<(double)number_of_ice/number_of_files*100<<" %)"<<endl;


}


void File_number()
{
    char temp[255];
    ifstream in;
    in.open("chi_file_name_front.txt");
    while (true){
        in.getline(temp,255,'\n');
        if(strlen(temp)<5)break;
        number_of_files++;
    }
    in.close();
}

void Read_file_name()
{
    chi_file_name_front = new char*[number_of_files];
    chi_file_name_side = new char*[number_of_files];

    for (int i=0;i<number_of_files;i++)
    {
        chi_file_name_front[i] = new char[100];
        chi_file_name_side[i] = new char[100];
    }

    ifstream in2;
    in2.open("chi_file_name_front.txt");
    for (int i=0;i<number_of_files;i++)
    {
        in2.getline(chi_file_name_front[i],100,'\n');
//        cout<<bin_file_name_temp[i]<<endl;
    }
    cout<<chi_file_name_front[5]<<endl;
    in2.close();

    ifstream in2_2;
    in2_2.open("chi_file_name_side.txt");
    for (int i=0;i<number_of_files;i++)
    {
        in2_2.getline(chi_file_name_side[i],100,'\n');
    }
    in2_2.close();
}

void Q_number(){
    char temp[255];
    ifstream in3;
    in3.open(chi_file_name_front[1]);
    while (true){
        in3.getline(temp,255,'\n');
        if(strlen(temp)<5)break;
        q_line++;
    }
    q_line=q_line-4;
    in3.close();

    ifstream in3_2;
    in3_2.open(chi_file_name_side[1]);
    while (true){
        in3_2.getline(temp,255,'\n');
        if(strlen(temp)<5)break;
        q_line_side++;
    }
    q_line_side = q_line_side -4;
    in3_2.close();

    cout<<q_line<<endl;
    cout<<q_line_side<<endl;

}

void Chi_read()
{
    for (int i=0;i<number_of_files;i++)
    {
            ifstream in4;
            in4.open(chi_file_name_front[i]);

            char temp_read_line[255];

            in4.getline(temp_read_line,255,'\n');
            in4.getline(temp_read_line,255,'\n');
            in4.getline(temp_read_line,255,'\n');
            in4.getline(temp_read_line,255,'\n');

            for (int k=0;k<q_line;k++)
            {
                in4>>intensity_front[1][k];
                in4>>intensity_front[i+2][k];
//cout<<"file_reak_ok"<<endl;
                intensity_front[0][k] = 4*3.14159/(12.4/11)*sin(intensity_front[1][k]/2*3.14159/180);
            }
            in4.close();
    }
cout<<"q_cal_OK"<<endl;

    for (int i=0;i<number_of_files;i++)
    {
        ifstream in4_2;
        in4_2.open(chi_file_name_side[i]);

        char temp_read_line2[255];

        in4_2.getline(temp_read_line2,255,'\n');
        in4_2.getline(temp_read_line2,255,'\n');
        in4_2.getline(temp_read_line2,255,'\n');
        in4_2.getline(temp_read_line2,255,'\n');

        for (int k=0;k<q_line_side;k++)
        {
            in4_2>>intensity_side[1][k];
            in4_2>>intensity_side[i+2][k];
            intensity_side[0][k] = 4*3.14159/(12.4/11)*sin(intensity_side[1][k]/2*3.14159/180);
        }
        in4_2.close();
    }
}

void sorting()
{
    for (int i=0;i<8;i++)
    {
        for (int k=0;k<q_line;k++)
            summary_front[i][k] = 0;
        for (int k=0;k<q_line_side;k++)
            summary_side[i][k] = 0;
    }

    for (int k=0;k<q_line;k++)
    {
        summary_front[0][k] = intensity_front[0][k];
        summary_front[1][k] = intensity_front[1][k];
    }

    for (int k=0;k<q_line_side;k++)
    {
        summary_side[0][k] = intensity_side[0][k];
        summary_side[1][k] = intensity_side[1][k];
    }

    for (int i=0;i<number_of_files;i++)
    {
        integ_water=0;
        integ_ice=0;
        for(int k=0;k<q_line_side;k++)
        {
            if(intensity_side[0][k]>1.8 && intensity_side[0][k]<3.3)
                integ_water = integ_water+intensity_side[i+2][k];
            if(intensity_side[0][k]>2.0 && intensity_side[0][k]<2.1)
                integ_ice = integ_ice+intensity_side[i+2][k];
        }

        if (integ_water > criteria1 && integ_water > integ_ice*criteria2) // water
        {
            for (int k=0;k<q_line;k++)
            {
                summary_front[2][k] = summary_front[2][k] + intensity_front[i+2][k];
            }
            for (int k=0;k<q_line_side;k++)
            {
                summary_side[2][k] = summary_side[2][k] + intensity_side[i+2][k];
            }
            number_of_water++;
            each_image[0][i] = i+1;
            each_image[1][i] = 1;
            cout<<each_image[1][i];
        }

        else if (integ_water > criteria1 && integ_water < integ_ice*criteria2) //ice
        {
            for (int k=0;k<q_line;k++)
            {
                summary_front[6][k] = summary_front[6][k] + intensity_front[i+2][k];
            }
            for (int k=0;k<q_line_side;k++)
            {
                summary_side[6][k] = summary_side[6][k] + intensity_side[i+2][k];
            }
            number_of_ice++;
            each_image[0][i] = i+1;
            each_image[1][i] = 3;
            cout<<each_image[1][i];
        }

        else //background
        {
            for (int k=0;k<q_line;k++)
            {
                summary_front[4][k] = summary_front[4][k] + intensity_front[i+2][k];
            }
                for (int k=0;k<q_line_side;k++)
            {
                summary_side[4][k] = summary_side[4][k] + intensity_side[i+2][k];
            }
            number_of_background++;
            each_image[0][i] = i+1;
            each_image[1][i] = 2;
            cout<<each_image[1][i];
        }

    }
    for (int k=0;k<q_line;k++)
    {
        summary_front[2][k] = summary_front[2][k]/number_of_water;
        summary_front[4][k] = summary_front[4][k]/number_of_background;
        summary_front[6][k] = summary_front[6][k]/number_of_ice;
    }
    for (int k=0;k<q_line_side;k++)
    {
        summary_side[2][k] = summary_side[2][k]/number_of_water;
        summary_side[4][k] = summary_side[4][k]/number_of_background;
        summary_side[6][k] = summary_side[6][k]/number_of_ice;
    }

    if (std_cal ==1)
    {
        for(int k=0;k<q_line;k++)
        {
            varience_front[0] = 0;
            std_front[0] = 0;
            for(int i=0;i<number_of_files;i++)
            {

            if (each_image[1][i] == 1) // water
            {
                varience_front[0] = varience_front[0] + (summary_front[2][k] - intensity_front[i+2][k])*(summary_front[2][k] - intensity_front[i+2][k]);
            }
            if (each_image[1][i] == 2) // background
            {
                varience_front[1] = varience_front[1] + (summary_front[4][k] - intensity_front[i+2][k])*(summary_front[4][k] - intensity_front[i+2][k]);
            }
            if (each_image[1][i] == 3) // ice
            {
                varience_front[2] = varience_front[2] + (summary_front[6][k] - intensity_front[i+2][k])*(summary_front[6][k] - intensity_front[i+2][k]);
            }
            }
            if(number_of_water>1)
            std_front[0] = sqrt(varience_front[0]/(number_of_water-1))/sqrt(number_of_water);
            if(number_of_background>1)
            std_front[1] = sqrt(varience_front[1]/(number_of_background-1))/sqrt(number_of_background);
            if(number_of_ice>1)
            std_front[2] = sqrt(varience_front[2]/(number_of_ice-1))/sqrt(number_of_ice);

            summary_front[3][k] = std_front[0];
            summary_front[5][k] = std_front[1];
            summary_front[7][k] = std_front[2];
        }
        for(int k=0;k<q_line_side;k++)
        {
            varience_side[0] = 0;
            std_side[0] = 0;
            for(int i=0;i<number_of_files;i++)
            {

            if (each_image[1][i] == 1) // water
            {
                varience_side[0] = varience_side[0] + (summary_side[2][k] - intensity_side[i+2][k])*(summary_side[2][k] - intensity_side[i+2][k]);
            }
            if (each_image[1][i] == 2) // background
            {
                varience_side[1] = varience_side[1] + (summary_side[4][k] - intensity_side[i+2][k])*(summary_side[4][k] - intensity_side[i+2][k]);
            }
            if (each_image[1][i] == 3) // ice
            {
                varience_side[2] = varience_side[2] + (summary_side[6][k] - intensity_side[i+2][k])*(summary_side[6][k] - intensity_side[i+2][k]);
            }
            }
            std_side[0] = sqrt(varience_side[0]/(number_of_water-1));
            std_side[1] = sqrt(varience_side[1]/(number_of_background-1));
            std_side[2] = sqrt(varience_side[2]/(number_of_ice-1));
            summary_side[3][k] = std_side[0];
            summary_side[5][k] = std_side[1];
            summary_side[7][k] = std_side[2];
        }
    }
}

void make_output()
{
//cout<<"1"<<endl;
    char output_file_name[255] = "./average/av_front.dat";
//cout<<"2"<<endl;
    ofstream out3;
//cout<<"3"<<endl;
    out3.open(output_file_name);

//cout<<"what?"<<endl;
    out3<<"#q   2theta   av_water   std_water   av_bg   std_bg  av_ice  std_ice"<<endl;

//cout<<"testssss"<<endl;
        
    for (int k=0;k<q_line;k++)
    {
        for(int i=0;i<7;i++)
        {
//            cout<<"teset"<<endl;
            out3<<summary_front[i][k]<<"    ";
        }
        out3<<summary_front[7][k]<<endl;
//        cout<<summary_front[7][k]<<endl;
    }
    out3.close();   

    char output_file_name_side[255] = "./average/av_side.dat";
    ofstream out3_2;
    out3_2.open(output_file_name_side);
    out3_2<<"#q   2theta   av_water   std_water   av_bg   std_bg  av_ice  std_ice"<<endl;
    for (int k=0;k<q_line_side;k++)
    {
        for(int i=0;i<7;i++)
        {
            out3_2<<summary_side[i][k]<<"    ";
        }   
        out3_2<<summary_side[7][k]<<endl;
    }
    out3_2.close();

    ofstream out4;
    out4.open("./average/sorting.dat");
    out4<<"#***** In summary *****"<<endl;
    out4<<"#- Water: "<<number_of_water<<" images ("<<(double)number_of_water/number_of_files*100<<" %)"<<endl;
    out4<<"#- Background: "<<number_of_background<<" images ("<<(double)number_of_background/number_of_files*100<<" %)"<<endl;
    out4<<"#- Ice: "<<number_of_ice<<" images ("<<(double)number_of_ice/number_of_files*100<<" %)"<<endl<<endl;
    out4<<"#image#   sorting(1=water, 2=bg, 3=ice)"<<endl;
    for (int i=0;i<number_of_files;i++)
    {
        out4<<each_image[0][i]<<"   "<<each_image[1][i]<<endl;
    }
    out4.close();

}

/*
void bad_image_plot()
{
    int j = 0;
    char bad_file_name[255] = "bad_image_plot";
    ofstream bad;
    bad.open(bad_file_name);

    double water;

    bad<<"plot 'av.dat' u 1:5 t 'bg' w l";

    for (int i=1;i<number_of_files;i++)
    {

        water=0;

        char chi_file_name[255];

                if (i<9)
                sprintf (chi_file_name, "./chi/0000%d.chi",i+1);
                if (i>8&&i<99)
                sprintf (chi_file_name, "./chi/000%d.chi",i+1);
                if (i>98&&i<999)
                sprintf (chi_file_name, "./chi/00%d.chi",i+1);
                if (i>998&&i<9999)
                sprintf (chi_file_name, "./chi/0%d.chi",i+1);
                if (9998<i)
                sprintf (chi_file_name, "./chi/%d.chi",i+1);

        if (bad_number[j] == i+1)
            j++;
        else
            bad<<", '' u 1:"<<i+5<<" t '"<<i<<"th position' w l";

        for (int k=57;k<133;k++)
		water=water+intensity[i+4][k];
	jetscan[1][i-1] = water;

    }
    bad<<"; pause -1"<<endl;
    bad.close();

	ofstream scan;
	scan.open("scan.dat");
	for(int i=1;i<number_of_files;i++)
	{
		scan<<jetscan[0][i-1]<<"	"<<jetscan[1][i-1]<<endl;
	}
	scan.close();

}

*/



