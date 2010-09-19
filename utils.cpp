#include "utils.h"
#include <iomanip>
#include <string>
#include <fstream>
#include <iostream>


void byte2String(char *strBits, unsigned long long byte,int tamanho)
{
	char  mask = 0x01;  //get LSbit
	char  bit;
	int i;
	
	for (i = (tamanho-1); i >= 0; i--)
	{		
		bit = byte & mask;
		strBits[i] = bit + 48; 
		byte = byte >> 1;
	}
	strBits[tamanho]='\0';
}

void msg2ai (int *strBits, unsigned long byte,int tamanho)
{
	int mask = 0x01;  //get LSbit
	int bit;
	int i;
	
	for (i = (tamanho-1); i >= 0; i--)
	{		
		bit = byte & mask;
		strBits[i] = bit;
		byte = byte >> 1;
	}	
}

/*
 * Converts the contents of a file into a string
 */
string FileToString(const string fileName)
{
    ifstream f(fileName.c_str(), ifstream::in | ifstream::binary);

    try
    {
        size_t size;
        char*  str;
        string s;

        if(f.is_open())
        {
            size_t fileSize;
            f.seekg(0, ifstream::end);
            size = fileSize = f.tellg();
            f.seekg(0, ifstream::beg);

            str = new char[size+1];
            if (!str) throw(string("Could not allocate memory"));

            f.read(str, fileSize);
            f.close();
            str[size] = '\0';

            s = str;
            delete [] str;
            return s;
        }
    }
    catch(std::string msg)
    {
        cerr << "Exception caught in FileToString(): " << msg << endl;
        if(f.is_open())
            f.close();
    }
    catch(...)
    {
        cerr << "Exception caught in FileToString()" << endl;
        if(f.is_open())
            f.close();
    }
    string errorMsg = "FileToString()::Error: Unable to open file "
                            + fileName;
    throw(errorMsg);
}

size_t shrRoundUp(int group_size,int global_size){
	int r = global_size % group_size;
	if (r==0)
		return global_size;
	else
		return global_size+group_size - r;
}
