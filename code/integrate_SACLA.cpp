#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string.h>
#include <sys/io.h>
//#include <unistd.h>

using namespace std;

void File_number();
void Read_file_name();
void Fit2d_input(int i);
void conf_read();
unsigned long length_check(char *check_file_name);
void matlab_gen();

char conf_file_name[250];
int num_pix_x = 2399;
int num_pix_y = 2399;
double size_pix_x = 50;
double size_pix_y = 50;
double center_x = 1199.058;
double center_y = 1199.507;
double detector_distance =76.095;
double energy = 12;
double threshold = 0.1;
int mask = 0;
int num_bins = 1000;


char **bin_file_name;
char **bin_file_name_temp;
char command_line[250];
char log1[250];
char log3[250];
char matlab_file_name[250];
char bin_check[250];
char exe_file_name[250];

char output_file_name[150];
char file_name_temp[100];
char file_name[100];
int number_of_files=0;

int run_number =0;
char *folder_name;

int para[4] = 
{2399,
2399,
50,
50
};// #of x_pixel, #of y_pixel, x_pixel size, y_pixel size
double para2[5] = 
{
    1199.058,
    1199.507,
    76.095,
    12.13957,
    0.1
};// beam center x, beam center y, detector distance, X-ray energy


int main(int argc, char* argv[]){

	folder_name = argv[1];
	run_number = atoi(argv[1]);

	conf_read();
	cout<<"conf read OK"<<endl;

	sprintf (matlab_file_name, "/UserData/fperakis/2018_6/codes/matlab_code/one_%d.m",run_number);
	sprintf (exe_file_name, "/UserData/fperakis/2018_6/codes/matlab_code/matlab_exe");
//	matlab_gen();
	sprintf (bin_check, "/UserData/fperakis/2018_6/run%d/bin/1000001.bin",run_number);
	if (access(bin_check,0)==-1)
	{
	matlab_gen();
	cout<<"generating bin files......"<<endl;
	sprintf (command_line, "chmod 777 ./matlab_code/matlab_exe");
        system(command_line);
	sprintf (command_line, "./matlab_code/matlab_exe");
	system(command_line);
	}
	else
	{
		cout<<"bin files are already there"<<endl;
	}

	sprintf (log1, "/UserData/fperakis/2018_6/codes/log_bin/bin_file_name_%d.txt",run_number);
	sprintf (log3, "/UserData/fperakis/2018_6/codes/log_fit2d/fit2d_input_%d.txt",run_number);
	sprintf (command_line, "ls /UserData/fperakis/2018_6/run%d/bin/*.bin > %s",run_number,log1);
	system(command_line);
	sprintf (command_line, "mkdir /UserData/fperakis/2018_6/run%d/chi",run_number);
	system(command_line);

    File_number();
    Read_file_name();
   
    for (int i=0;i<number_of_files;i++)
    {

        Fit2d_input(i);
	if(access(output_file_name, 0)==-1)
	{
		sprintf (command_line, "./fit2d_new -com -nogr < %s >log4.dat",log3);
        	system(command_line);
	}
	else if(length_check(output_file_name)<1000)
	{
		sprintf (command_line, "./fit2d_new -com -nogr < %s >log4.dat",log3);
		system(command_line);
	}

	if(access(output_file_name, 0)==-1)
	{
		i--;
	}
	else if(length_check(output_file_name)<1000)
//	if (access( output_file_name, 0)==-1)
	{
		i--;
	}

    }

	sprintf (command_line, "./heat.out %d %d",run_number,1000);
        system(command_line);

}

unsigned long length_check(char *check_file_name)
{
    FILE *f;
    f = fopen(check_file_name , "r");
    fseek(f, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(f);
//    printf("%ld\n", len);
    fclose(f);
	return len;
}

void File_number(){
    char temp[255];
    ifstream in;
    in.open(log1);
    while (true){
        in.getline(temp,255,'\n');
        if(strlen(temp)<5)break;
        number_of_files++;
    }
    in.close();
}

void Read_file_name(){

//    char temp[255];
    bin_file_name = new char*[number_of_files];
    bin_file_name_temp = new char*[number_of_files];

    for (int i=0;i<number_of_files;i++)
    {
        bin_file_name[i] = new char[250];
        bin_file_name_temp[i] = new char[250];
    }

    ifstream in2;
    in2.open(log1);
    for (int i=0;i<number_of_files;i++)
    {
        in2.getline(bin_file_name_temp[i],250,'\n');
//        cout<<bin_file_name_temp[i]<<endl;
    }
    in2.close();

}

void Fit2d_input(int i){

        sprintf (output_file_name, "/UserData/fperakis/2018_6/run%d/chi/%d.chi",run_number,i+1000001);


    cout<<output_file_name<<" is made...."<<endl;

    ofstream fit2d;
    fit2d.open(log3);
    fit2d<<"dim"<<endl<<num_pix_x<<endl<<num_pix_y<<endl<<"yes"<<endl<<"no"<<endl;
    fit2d<<"input"<<endl<<"bin"<<endl<<bin_file_name_temp[i]<<endl;
    fit2d<<num_pix_x<<endl<<num_pix_y<<endl<<"REAL (4-BYTE IEEE)"<<endl;

//    fit2d<<"EXCHANGE"<<endl<<"input"<<endl<<"fit2d"<<endl<<"./dark.f2d"<<endl<<"EXCHANGE"<<endl<<"SUBTRACT"<<endl;

    fit2d<<"fit"<<endl<<"threshold"<<endl<<"yes"<<endl<<threshold<<endl;
    fit2d<<"powder diffraction"<<endl<<"yes"<<endl<<center_x<<endl<<center_y<<endl<<size_pix_x<<endl<<size_pix_y<<endl<<detector_distance<<endl<<"0"<<endl<<"0"<<endl<<"yes"<<endl<<"0.99"<<endl;
    fit2d<<"PARTIAL POWDER (2-THETA SCAN)"<<endl<<"yes"<<endl<<"0.1"<<endl<<"no"<<endl<<"no"<<endl<<"exit"<<endl;
    fit2d<<"exchange"<<endl<<"OUTPUT DATA"<<endl<<"CHIPLOT"<<endl<<output_file_name<<endl<<"yes"<<endl<<"1"<<endl<<"exit"<<endl<<"yes"<<endl;

    fit2d.close();

}

void conf_read()
{
        sprintf (conf_file_name, "./SACLA.conf");
        char temp[255];
        ifstream in2;
        in2.open (conf_file_name);
cout<<"number of x pixels = "<<num_pix_x<<endl;
        in2.getline(temp,255,'\n');
        in2>>num_pix_x;
cout<<"number of x pixels = "<<num_pix_x<<endl;
        in2.getline(temp,255,'\n');
        in2.getline(temp,255,'\n');
        in2>>num_pix_y;
        in2.getline(temp,255,'\n');
        in2.getline(temp,255,'\n');
        in2>>size_pix_x;
        in2.getline(temp,255,'\n');
        in2.getline(temp,255,'\n');
        in2>>size_pix_y;
        in2.getline(temp,255,'\n');
        in2.getline(temp,255,'\n');
        in2>>center_x;
        in2.getline(temp,255,'\n');
        in2.getline(temp,255,'\n');
        in2>>center_y;
        in2.getline(temp,255,'\n');
        in2.getline(temp,255,'\n');
        in2>>detector_distance;
        in2.getline(temp,255,'\n');
        in2.getline(temp,255,'\n');
        in2>>energy;
        in2.getline(temp,255,'\n');
        in2.getline(temp,255,'\n');
        in2>>threshold;
cout<<"number of bins = "<<num_bins<<endl;
        in2.getline(temp,255,'\n');
        in2.getline(temp,255,'\n');
        in2>>num_bins;
        in2.getline(temp,255,'\n');
	in2.close();

cout<<"number of bins = "<<num_bins<<endl;
}

void matlab_gen()
{
        ofstream in;
        in.open(matlab_file_name);

in<<"    run_number = "<<run_number<<endl;
in<<"    num_skip = 1"<<endl;
in<<"    num_file = 1000000"<<endl;
in<<"for j=1:1"<<endl;
in<<"    file_name = sprintf('/UserData/fperakis/2018_6/run%d/%d_corrected.h5',run_number, run_number) %% change"<<endl;
in<<endl;
in<<"    file = H5F.open (file_name, 'H5F_ACC_RDONLY', 'H5P_DEFAULT'); %%read only mode"<<endl;
in<<endl;
in<<"    dset1_filename = sprintf('run_%d/event_info/tag_number_list', run_number)"<<endl;
in<<"    dset1 = H5D.open (file, dset1_filename);"<<endl;
in<<"    dataspace1 = H5D.get_space(dset1);"<<endl;
in<<"    [dim1,dim2] = H5S.get_simple_extent_dims(dataspace1);"<<endl;
in<<"    data1 = H5D.read(dset1, 'H5ML_DEFAULT', 'H5S_ALL', dataspace1, 'H5P_DEFAULT');"<<endl;
in<<"    H5D.close (dset1);"<<endl;
in<<"    H5S.close (dataspace1);"<<endl;
in<<endl;
in<<"    image_num = dim2/num_skip %%change"<<endl;
in<<endl;
in<<"    for i=1:image_num"<<endl;
in<<"        %% read CCD images"<<endl;
in<<"        tag_name = sprintf('/run_%d/detector_2d_assembled_1/tag_%d/detector_data',run_number, data1(i,1));%%change"<<endl;
in<<"        dset = H5D.open (file, tag_name);"<<endl;
in<<"        dataspace = H5D.get_space(dset);"<<endl;
in<<"        data = H5D.read(dset, 'H5ML_DEFAULT', 'H5S_ALL', dataspace, 'H5P_DEFAULT');"<<endl;
in<<"        H5D.close (dset);"<<endl;
in<<"        H5S.close (dataspace);"<<endl;
in<<"        num_file = num_file + 1;"<<endl;
in<<endl;
in<<"        %% make binary file of CCD images"<<endl;
in<<"        path_name = sprintf('/UserData/fperakis/2018_6/run%d/bin', run_number)"<<endl;
in<<"        mkdir(path_name);"<<endl;
in<<"        out_file_name = sprintf('/UserData/fperakis/2018_6/run%d/bin/%d.bin',run_number, num_file);%%change"<<endl;
in<<"        out_bin_file = fopen(out_file_name, 'w');"<<endl;
in<<"        fwrite(out_bin_file, data, 'float32');"<<endl;
in<<"        fclose(out_bin_file);"<<endl;
in<<endl;
in<<"        clear data;"<<endl;
in<<"        end"<<endl;
in<<endl;
in<<"H5F.close(file);"<<endl;
in<<"end"<<endl;
in<<"test = 'program finished'"<<endl;
        in.close();

	ofstream in3;
	in3.open(exe_file_name);
in3<<"#!/bin/csh -f"<<endl;
in3<<"unsetenv DISPLAY"<<endl;
in3<<"nohup matlab <"<<matlab_file_name<<"> ./matlab_code/log_"<<run_number<<" %"<<endl;
in3.close();



}

