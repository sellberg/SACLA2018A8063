#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string.h>

using namespace std;

void File_number();
void Read_file_name();

void Q_number();
void Chi_read();
void initialize();
void check_hits();
void do_normalization();
void bad_check();
void do_average();
void make_output();
void take_difference();
void take_average();

char **chi_file_name;
char **chi_file_name_temp;

int number_of_files=0;
int q_line=0;

int *hits;
int *onoff;
int *timing[3];
double *normalization;
int *bad_image;
double *bad_sum;
double *bad_sum_ice;
int *bad_image_ice;
int *bad_image_sort;

double *dark;

double energy=12.13957;

double **intensity_full;
double **difference;
double *average;//q, number of delay(para[1])
int *summary;//#of delay, 6

double *delay_av;//q_line x #of delay+2
double *neg_av;
double *miss_av;
double *neg_temp;
double *neg_temp2;
int neg_number=0;
int run_number = 0;

double *sort;
int *pairs;
int *tags;
int *tags_time;
double *delay_true;
double *delay_no;
int *delay_code;
int number_of_time;
double *shot_intensity;
double *shot_energy;
char tag_file_name[255] = "time.txt";
char log4[250];
char command_line[250];
char summary_file_name[250];
char av_file_name[250];
char bad_file_name[250];
char test_file_name[250];
char diff_file_name[250];

double hit_criteria = 10000;
double high_criteria = 1280000;
double bad_criteria = 11120;
double bad_criteria_sort = 11300;
double ice_criteria = 3.0;
int num_of_pool = 1000;

int main(int argc, char* argv[]){

	if (argc != 3)
	{
		cout<<"run number of num of pool not specified."<<endl;
		return 0;
	}
	run_number = atoi(argv[1]);
	num_of_pool = atoi(argv[2]);

        sprintf (log4, "/UserData/fperakis/2018_6/codes/log_chi/chi_file_name_%d.txt",run_number);
	sprintf (summary_file_name, "/UserData/fperakis/2018_6/run%d/output/summary.dat",run_number);
	sprintf (av_file_name, "/UserData/fperakis/2018_6/run%d/output/av.dat",run_number);
	sprintf (bad_file_name, "/UserData/fperakis/2018_6/run%d/output/bad_sum.dat",run_number);
	sprintf (test_file_name, "/UserData/fperakis/2018_6/run%d/output/test.dat",run_number);
	sprintf (diff_file_name, "/UserData/fperakis/2018_6/run%d/output/diff.dat",run_number);

	sprintf (command_line, "ls /UserData/fperakis/2018_6/run%d/chi/*.chi > %s",run_number,log4);
        system(command_line);
	sprintf (command_line, "mkdir /UserData/fperakis/2018_6/run%d/output",run_number);
        system(command_line);

    File_number();
cout<<"file number OK"<<endl;
    Read_file_name();
cout<<"read file OK"<<endl;
    Q_number();
cout<<"q number OK"<<endl;
	initialize();
cout<<"initialize OK"<<endl;
    Chi_read();
cout<<"chi read OK"<<endl;
    cout<<number_of_files<<endl;
    cout<<chi_file_name_temp[0]<<endl;
    cout<<chi_file_name_temp[10]<<endl;
    cout<<q_line<<endl;
	check_hits();
cout<<"check hits OK"<<endl;
	take_difference();
cout<<"take difference OK"<<endl;
	bad_check();
cout<<"bad_check OK"<<endl;
	do_normalization();
cout<<"normalization OK"<<endl;
	take_average();
cout<<"take average OK"<<endl;
	make_output();
cout<<"make output OK"<<endl;

cout<<neg_number<<endl;
//	system("gnuplot plot");
}

void initialize()
{
	hits = new int[number_of_files];
        onoff = new int[number_of_files];
        for (int i=0;i<3;i++)
	{
        	timing[i] = new int[number_of_files];
	}
        normalization = new double[number_of_files];
	bad_image = new int[number_of_files];
	bad_image_ice = new int[number_of_files];
	bad_sum = new double[number_of_files];
	bad_sum_ice = new double[number_of_files];
	bad_image_sort = new int[number_of_files];

	tags = new int[number_of_files];
	delay_no = new double[number_of_files];
	delay_true = new double[number_of_files];
	delay_code = new int[number_of_files];
	sort = new double[number_of_files];
	pairs = new int[number_of_files];

        for (int i=0;i<number_of_files;i++)
        {
                hits[i] = 0;
                onoff[i] = 0;
                for (int j=0;j<3;j++)
		{
                	timing[j][i] = 0;
		}
                normalization[i] = 0;
		bad_sum[i] = 0;
		bad_sum_ice[i] = 0;
		bad_image[i] = 0;
		bad_image_ice[i]=1;
		bad_image_sort[i] = 0;

		tags[i] = 0;
		delay_no[i] = 0;
		delay_true[i] =0;
		delay_code[i] = 0;
		sort[i] = 500000;
		pairs[i] = 0;
        }

	tags_time = new int[number_of_time];
	shot_intensity = new double[number_of_time];
	shot_energy = new double[number_of_time];
	for (int i=0;i<number_of_time;i++)
	{
		tags_time[i] = 0;
		shot_intensity[i]=0;
		shot_energy[i] = 0;
	}

	intensity_full = new double*[q_line];
        difference = new double*[q_line];

	neg_temp = new double[q_line];
	neg_temp2 = new double[q_line];

	for (int i=0;i<q_line;i++)
	{
		intensity_full[i] = new double[number_of_files+2];
                difference[i] = new double[number_of_files+2];
	}
	for (int i=0;i<q_line;i++)
	{
		neg_temp[i] = 0;
		neg_temp2[i] = 0;
		for (int j=0;j<number_of_files+2;j++)
		{
			intensity_full[i][j] = 0;
			difference[i][j] = 0;
		}
	}

	average = new double[q_line];
	for (int i=0;i<q_line;i++)
	{
		average[i] = 0;
	}
	summary = new int[7];
	for (int i=0;i<7;i++)
	{
		summary[i] = 0;
	}

	delay_av = new double[q_line];
	for (int k=0;k<q_line;k++)
	{
		delay_av[k] =0;
	}

        neg_av = new double[q_line];
	miss_av = new double[q_line];
        for (int k=0;k<q_line;k++)
        {
        	neg_av[k] =0;
		miss_av[k] = 0;
        }


}

void take_difference()
{
	int o=0;
	int p=0;

	for (int i=0;i<number_of_files;i++)
	{
		if (hits[i] == 1 && onoff[i] == 0)
		{
			neg_number++;
		}

		if (hits[i] == 1 && onoff[i] == 1)
		{
			for (int j=0;j<num_of_pool;j++)
			{
				o=((int)(i/num_of_pool))*num_of_pool + j;
				if (o<number_of_files)
				{
				if (hits[o] == 1 && onoff[o] != 1)
				{
/*
					if (sqrt((shot_energy[i] - shot_energy[o])*(shot_energy[i] - shot_energy[o]))<sort[i])
					{
						sort[i] = sqrt((shot_energy[i] - shot_energy[o])*(shot_energy[i] - shot_energy[o]));
						p = o;
					}
*/

					if (sqrt((normalization[i] - normalization[o])*(normalization[i] - normalization[o]))<sort[i])                                                                              {                                                       sort[i] = sqrt((normalization[i] - normalization[o])*(normalization[i] - normalization[o]));                                        
                                                p = o;
                                        }
				}
				}
			}
			pairs[i] = p;

			for (int k=0;k<q_line;k++)
			{
				neg_temp[k] = intensity_full[k][p+2];
			}
				
			for (int k=0;k<q_line;k++)
			{
				difference[k][i+2] = 10000*(intensity_full[k][i+2]/normalization[i] - neg_temp[k]/normalization[p]);
			}
		}		
	}
}



void take_average()
{
	int miss_number =0;
	neg_number = 0;
	for (int i=0;i<number_of_files;i++)
	{
		if (hits[i] ==1 && onoff[i] ==1)
                {
			if (bad_image[i] ==1 && bad_image_sort[i] ==1)
			{
				if(bad_image_ice[i] == 1)
				{
					summary[3]++;
					for (int k=0;k<q_line;k++)
					{
                                	delay_av[k] = delay_av[k] +  difference[k][i+2];
					average[k] = average[k] + intensity_full[k][i+2];
					}
				}
			}
		}
		else if (hits[i] ==1 && onoff[i] ==0)
		{
			neg_number++;
			for (int k=0;k<q_line;k++)
			{
				neg_av[k] = neg_av[k] + intensity_full[k][i+2];
			}
		}
		else if (hits[i] ==0)
		{
			miss_number++;
			for (int k=0;k<q_line;k++)
			{
				miss_av[k] += intensity_full[k][i+1];
			}
		}
	}

	for (int k=0;k<q_line;k++)
	{
		delay_av[k] = delay_av[k]/summary[3];
		average[k] = average[k]/summary[3];
		neg_av[k] = neg_av[k]/neg_number;
		miss_av[k] = miss_av[k]/miss_number;
	}
cout<<"#of images: "<<summary[3]<<endl;
cout<<"neg_number: "<<neg_number<<endl;

}


void File_number(){
    char temp[255];
    ifstream in;
    in.open(log4);
    while (true){
        in.getline(temp,255,'\n');
        if(strlen(temp)<5)break;
        number_of_files++;
    }
    in.close();
}

void Read_file_name(){

    chi_file_name = new char*[number_of_files];
    chi_file_name_temp = new char*[number_of_files];

    for (int i=0;i<number_of_files;i++)
    {
        chi_file_name[i] = new char[250];
        chi_file_name_temp[i] = new char[250];
    }

    ifstream in2;
    in2.open(log4);
    for (int i=0;i<number_of_files;i++)
    {
        in2.getline(chi_file_name_temp[i],250,'\n');
    }
    in2.close();
}

void Q_number(){
    char temp[255];
    ifstream in3;
    in3.open(chi_file_name_temp[0]);
    while (true){
        in3.getline(temp,255,'\n');
        if(strlen(temp)<5)break;
        q_line++;
    }
    q_line=q_line-4;
    in3.close();

/*
	ifstream in33;
	in33.open(tag_file_name);
	while (true)
	{
		in33.getline(temp,255,'\n');
	        if(strlen(temp)<5)break;
	        number_of_time++;
	}
	in33.close();

	cout<<"number_of_time = "<<number_of_time<<endl;
*/
}

void Chi_read()
{
        for (int j=0;j<number_of_files;j++)
        {
            ifstream in4;
            in4.open(chi_file_name_temp[j]);

            char temp_read_line[250];

            in4.getline(temp_read_line,250,'\n');
            in4.getline(temp_read_line,250,'\n');
            in4.getline(temp_read_line,250,'\n');
            in4.getline(temp_read_line,250,'\n');

            for (int k=0;k<q_line;k++)
            {
                in4>>intensity_full[k][0];
                in4>>intensity_full[k][j+2];
            }
            in4.close();

        }

	for (int i=0;i<q_line;i++)
	{
		intensity_full[i][1] = 4*3.14159/(12.4/energy)*sin(intensity_full[i][0]/2*3.14159/180);
		difference[i][1] = 4*3.14159/(12.4/energy)*sin(intensity_full[i][0]/2*3.14159/180);
		difference[i][0] = intensity_full[i][0];
	}
/*
	ifstream in44;
	in44.open("tag_number.txt");
	for (int i=0;i<number_of_files;i++)
	{
		in44>>tags[i];
	}
	in44.close();
*/
/*
	double dummy2=0;
	ifstream in45;
	in45.open(tag_file_name);
	for (int i=0;i<number_of_time;i++)
	{
		in45>>tags[i];
		in45>>shot_energy[i];
		in45>>shot_intensity[i];
	}
*/
}

void check_hits()
{
	for (int i=0;i<number_of_files;i++)
	{
		for (int k=0;k<q_line;k++)
		{
			if (intensity_full[k][1] > 0.66 && intensity_full[k][1] < 3.26)
			{
				normalization[i] += intensity_full[k][i+2];
			}
		}
	}

	for (int i=0;i<number_of_files;i++)
	{
		if (normalization[i] > hit_criteria && normalization[i] < high_criteria)
		{
			hits[i] = 1;
		}
		else
		{
			hits[i] = 0;
		}
	}

	for (int i=0;i<number_of_files;i++)
	{
		if( i % 4 == 0)
		{
			onoff[i] = 0;
		}
		else
		{
			onoff[i] = 1;
		}
	}
}

void do_normalization()
{
	for (int i=0;i<number_of_files;i++)
	{
		for (int k=0;k<q_line;k++)
		{
			if (hits[i] == 1)
			{
				intensity_full[k][i+2] = intensity_full[k][i+2]/normalization[i]*10000;
			}
		}
	}
}

void bad_check()
{
//bad_check for heat
	for (int i=0;i<number_of_files;i++)
	{
	if(hits[i] == 1 && onoff[i] ==1)
	{
		for (int k=0; k<q_line;k++)
		{
			if(difference[k][1]>1.5 && difference[k][1]<3.5)
			{
				bad_sum[i] += sqrt(difference[k][i+2]*difference[k][i+2]);
			}
		}

		if (hits[i] == 1 && bad_sum[i] < bad_criteria)
		{
			bad_image[i] = 1;
		}
	}
	else
	{
		bad_image[i] = 0;
	}
	}

//bad_check for sorting
        for (int i=0;i<number_of_files;i++)
        {
        if(hits[i] == 1 && onoff[i] ==1)
        {
                if (hits[i] == 1 && sort[i] < bad_criteria_sort)
                {
                        bad_image_sort[i] = 1;
                }
        }
        else
        {
                bad_image_sort[i] = 0;
        }
        }

//bad_check for ice
	double temp_q_line=0;
	for (int i=0;i<number_of_files;i++)
	{
		if (hits[i] ==1)
		{
			for (int k=0;k<q_line;k++)
			{
				if (intensity_full[k][1] > 1.2 && intensity_full[k][1] <3.5)
				{
					bad_sum_ice[i] += intensity_full[k][i];
					temp_q_line++;
				}
			}
			bad_sum_ice[i] = bad_sum_ice[i]/temp_q_line;
			temp_q_line =0;
			for (int k=0;k<q_line;k++)
                        {
                                if (intensity_full[k][1] > 1.2 && intensity_full[k][1] <3.5)
                                {
                                        if (intensity_full[k][i] > bad_sum_ice[i]*ice_criteria)
					{
						bad_image_ice[i] = 0;
					}
                                }
			}
			
		}
	}
}

void make_output()
{
	summary[1] = number_of_files;
	for (int i=0;i<number_of_files;i++)
	{
		if (hits[i] == 1)
		{
			summary[2]++;
		}
		if (bad_image_ice[i] ==1)
		{
			summary[5]++;
		}
		if (hits[i] == 1 && onoff[i]==0)
		{
			summary[4]++;
		}
	}
    ofstream out3;
    out3.open(summary_file_name);
    out3<<"#_of_shots	#_of_hits	#_of_good	#_of_good_off	#_of_ice	hit_rate"<<endl;
	for (int j=1;j<7;j++)
	{
		out3<<summary[j]<<"	";
	}
	out3<<(double)summary[2]/(double)summary[1]<<endl;
    out3.close();


    ofstream out4;
    out4.open(av_file_name);
	out4<<"#q	diff_av	neg_av	pos_av	av)diff"<<endl;
	for (int k=0;k<q_line;k++)
	{
		out4<<intensity_full[k][1]<<"	"<<delay_av[k]<<"	"<<neg_av[k]<<"	"<<average[k]<<"	"<<(average[k]-neg_av[k])<<"	"<<miss_av[k]<<endl;
	}
	out4.close();

	ofstream out93;
	out93.open(diff_file_name);
	for (int k=0;k<q_line;k++)
	{	
		out93<<intensity_full[k][1];
		for (int i=0;i<30;i++)
		{
			out93<<"	"<<difference[k][i+2];
		}
		out93<<endl;
	}
	out93.close();

    ofstream out6;
    out6.open(bad_file_name);

    out6<<"#image#	normalization	bad_sum	bad	hits	onoff	sorting_value	bad_image_sort	pair_image_num	ice_nor	ice"<<endl;
    for (int i=0;i<number_of_files;i++)
    {
	if(sort[i] == 500000)
	sort[i] = 0;

	out6<<i<<"	"<<normalization[i]<<"	"<<bad_sum[i]<<"	"<<bad_image[i]<<"	"<<hits[i]<<"	"<<onoff[i]<<"	"<<sort[i]<<"	"<<bad_image_sort[i]<<"	"<<pairs[i]<<"	"<<bad_sum_ice[i]<<"	"<<bad_image_ice[i]<<endl;
    }
   
    out6.close();

	ofstream out7;
	out7.open(test_file_name);
	out7<<"#image_num	tag_number	shot_energy	shot_intensity"<<endl;
	for (int i=0;i<number_of_files;i++)
	{
		out7<<i<<"	"<<tags[i]<<"	"<<shot_energy[i]<<"	"<<shot_intensity[i]<<"	"<<endl;
	}
	out7.close();

	ofstream out10;
	out10.open("./plot");
        out10<<"plot '/UserData/fperakis/2018_6/run"<<run_number<<"/output/av.dat' u 1:3 t '"<<run_number<<" pos' w l, '' u 1:4 t '"<<run_number<<" neg' w l, '' u 1:5 t '"<<run_number<<" miss' w l"<<endl;
        out10<<"pause -1"<<endl;
	out10<<"plot '/UserData/fperakis/2018_6/run"<<run_number<<"/output/av.dat' u 1:2 t '"<<run_number<<" diff_av' w l, '' u 1:5 t '"<<run_number<<" av_diff' w l"<<endl;
        out10<<"pause -1"<<endl;
	out10.close();
}

