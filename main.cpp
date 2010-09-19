#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include "DES.h"

int erro;
unsigned long long key=0x0123456789ABCDEFLLU;
bool encripta=false;
DeviceType device;
string filename1,filename2;
stringstream convert;


//help
void printhelp(){
	printf("Usage: DES [-d|-e] file key [-gpu|-cpu|-openmp]\n\n");
	printf("\t-e:\t\tencrypt file\n");
	printf("\t-d:\t\tdecrypt file\n");
	printf("\t-gpu:\t\tuse OpenCL gpu (if available)\n");
	printf("\t-cpu:\t\tuse OpenCL cpu (if available)\n");
	printf("\t-a:\t\tuse OpenCL accelerator (if available)\n");
	printf("\t-openmp:\tuse OpenMP \n");
	printf("key in hexadecimal format ex: 0x0123456789ABCDEF\n");
}

//command line parse
void parsecmdline(int argc,char * argv[]){

	if(argc != 5){
		printhelp();
		exit (-1);
	}else{


		if(strcmp(argv[1],"-d")==0)
			encripta=false;
		else
		{
			if(strcmp(argv[1],"-e")==0)
				encripta=true;
			else
			{
				printhelp();
				exit(-1);
			}
		}


		convert.str(string(argv[3]));
		convert >> std::hex >> key;

		if(strcmp(argv[4],"-gpu")==0)
			device=GPU;
		else
		{
			if(strcmp(argv[4],"-cpu")==0)
				device=CPU;
			else
				if(strcmp(argv[4],"-a")==0)
					device=ACCELERATOR;
				else
					if(strcmp(argv[4],"-openmp")==0)
						device=OPENMP;
					else
					{
						printhelp();
						exit (-1);
					}
		}

	}
}


int main(int argc, char * argv[])
{

	if(argc==1){
		filename1 = "README";
		filename2 = "README.DES";
		//key =

		//encrypting
		printf("encrypting....\n");
		erro = DES(filename1.c_str(), key,OPENMP);
		if (erro != 0)
			return erro;

		//decrypting
		printf("decrypting....\n");
		erro = unDES(filename2.c_str(), key,OPENMP);
	}else{

		parsecmdline(argc,argv);

		if(encripta){
			printf("a encriptar....\n");
			erro = DES(argv[2], key,device);
		}else{
			//decriptação
			printf("a desencriptar....\n");
			erro = unDES(argv[2], key,device);
		}

	}

	return erro;
}
