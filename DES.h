
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>
#include "utils.h"


#ifndef DES_H_
#define DES_H_

unsigned char* encryptDES(unsigned char* inByteArray, long dim, unsigned long long key, int type);
unsigned long long encryptDESplain(unsigned long long plain, unsigned long long* subKeys);
void DESKeySchedule(unsigned long long key,unsigned long long * subKeys);
int DESgeneral (const char* inFileName, unsigned long long key, int type,DeviceType device);

int DES (const char* inFileName, unsigned long long key,DeviceType device);
int unDES (const char* inFileName, unsigned long long key,DeviceType device);

unsigned long long permuta(unsigned long long b, int subs[],int tamanho, int blockSize);
unsigned long long rotateleft(unsigned long long value,int tamanho, int shift);
#endif /* DES_H_ */
