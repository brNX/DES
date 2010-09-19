/*
 * opencl.h
 *
 *      Author: bgouveia
 */


#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <vector>
#include <string>
#include "utils.h"


#ifndef OPENCL_H_
#define OPENCL_H_


#define DEBUG 0

//variaveis opencl
uint availableworkitems;
unsigned long maxbuffersize;

cl_context              contextCL;
cl_device_id            *devicesCL;
cl_command_queue        queueCL;
cl_program              programCL;
std::vector<cl_kernel>  kernelCL;
std::string kernelFileCL="DESKernel.cl";
int nTotalKernels=1;

cl_mem  inputCL;  //buffer de entrada
cl_mem  subkeysCL; //buffer para as subkeys do DES


void initCL(DeviceType device);
void ReleaseCL();
void SetKernelArgs(int vezes);
void ReleaseCLBuffers();
void InitCLBuffers(long dim,unsigned long long* subKeys);
void RefreshBuffer(unsigned char* inByteArray,long dim);
void RunCL(int vezes);
void EnqueueKernel(int vezes, bool bBlocking);
unsigned char* encryptDES_CL(unsigned char* inByteArray, long dim, unsigned long long key, int type,DeviceType device);


#endif /* OPENCL_H_ */
