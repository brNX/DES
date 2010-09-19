#include <string>
using namespace std;


#ifndef UTILS_H_
#define UTILS_H_


void byte2String(char *strBits, unsigned long long byte,int tamanho);
void msg2ai (int *strBits, unsigned long byte,int tamanho);
string FileToString(const string fileName);
size_t shrRoundUp(int group_size,int global_size);

enum DeviceType { GPU, CPU, ACCELERATOR, OPENMP };

#endif
