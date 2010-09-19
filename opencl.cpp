/*
 * opencl.cpp
 *
 *      Author: bgouveia
 */

#include "opencl.h"
#include "DES.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Create Context, Device list, Command Queue
 * Load CL file, compile, link CL source
 * Build program and kernel objects
 */
void InitCL(DeviceType device)
{
    cl_int resultCL;

    contextCL = NULL;
    devicesCL = NULL;
    queueCL = NULL;
    programCL = NULL;

    size_t deviceListSize;

	cl_platform_id platforms[100];
	cl_uint platforms_n = 0;

	resultCL=clGetPlatformIDs(100, platforms, &platforms_n);

	if(resultCL!=CL_SUCCESS)
		throw (string("InitCL() Erro não foi encontrado plataforma OpenCL"));

	//codigo para obter info OpenCL
	if(DEBUG){
		printf("=== %d OpenCL platform(s) found: ===\n", platforms_n);
		for (unsigned int i=0; i<platforms_n; i++)
		{
			char buffer[10240];
			printf("  -- %d --\n", i);
			clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE, 10240, buffer, NULL);
			printf("  PROFILE = %s\n", buffer);
			clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, 10240, buffer, NULL);
			printf("  VERSION = %s\n", buffer);
			clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 10240, buffer, NULL);
			printf("  NAME = %s\n", buffer);
			clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, 10240, buffer, NULL);
			printf("  VENDOR = %s\n", buffer);
			clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, 10240, buffer, NULL);
			printf("  EXTENSIONS = %s\n", buffer);
		}

		cl_device_id devicesfound[100];
		cl_uint devices_n = 0;
		clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 100, devicesfound, &devices_n);

		printf("=== %d OpenCL device(s) found on platform:\n", platforms_n);
		for (unsigned int i=0; i<devices_n; i++)
		{
			char buffer[10240];
			cl_uint buf_uint;
			cl_ulong buf_ulong;
			printf("  -- %d --\n", i);
			clGetDeviceInfo(devicesfound[i], CL_DEVICE_NAME, sizeof(buffer), buffer, NULL);
			printf("  DEVICE_NAME = %s\n", buffer);
			clGetDeviceInfo(devicesfound[i], CL_DEVICE_VENDOR, sizeof(buffer), buffer, NULL);
			printf("  DEVICE_VENDOR = %s\n", buffer);
			clGetDeviceInfo(devicesfound[i], CL_DEVICE_VERSION, sizeof(buffer), buffer, NULL);
			printf("  DEVICE_VERSION = %s\n", buffer);
			clGetDeviceInfo(devicesfound[i], CL_DRIVER_VERSION, sizeof(buffer), buffer, NULL);
			printf("  DRIVER_VERSION = %s\n", buffer);
			clGetDeviceInfo(devicesfound[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(buf_uint), &buf_uint, NULL);
			printf("  DEVICE_MAX_COMPUTE_UNITS = %u\n", (unsigned int)buf_uint);
			clGetDeviceInfo(devicesfound[i], CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(buf_uint), &buf_uint, NULL);
			printf("  DEVICE_MAX_CLOCK_FREQUENCY = %u\n", (unsigned int)buf_uint);
			clGetDeviceInfo(devicesfound[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);
			printf("  DEVICE_GLOBAL_MEM_SIZE = %llu\n", (unsigned long long)buf_ulong);
			clGetDeviceInfo(devicesfound[i], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);
			printf("  DEVICE_MAX_MEM_ALLOC_SIZE = %llu\n", (unsigned long long)buf_ulong);
			clGetDeviceInfo(devicesfound[i], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);
			printf("  DEVICE_LOCAL_MEM_SIZE = %llu\n", (unsigned long long)buf_ulong);
		}

	}

    /////////////////////////////////////////////////////////////////
    // Create an OpenCL context
	/////////////////////////////////////////////////////////////////

    cl_context_properties cps[3] =
    {
        CL_CONTEXT_PLATFORM,
        (cl_context_properties)platforms[0],
        0
    };

    /* Use NULL for backward compatibility */
    cl_context_properties* cprops = (NULL == platforms[0]) ? NULL : cps;


    //escolhe entre varios tipos de device
	if(device==GPU)
		contextCL = clCreateContextFromType(cprops,
				CL_DEVICE_TYPE_GPU,
				NULL,
				NULL,
				&resultCL);
	if(device==CPU)
		contextCL = clCreateContextFromType(cprops,
				CL_DEVICE_TYPE_CPU,
				NULL,
				NULL,
				&resultCL);
	if(device==ACCELERATOR)
		contextCL = clCreateContextFromType(cprops,
				CL_DEVICE_TYPE_ACCELERATOR,
				NULL,
				NULL,
				&resultCL);

	string dispositivo;
	switch (device){
	case GPU:dispositivo="GPU";break;
	case CPU:dispositivo="CPU";break;
	case ACCELERATOR:dispositivo="ACCELERATOR";break;
	default:
		dispositivo="ERRO";
	}

	if ((resultCL != CL_SUCCESS) || (contextCL == NULL))
		throw (string("InitCL() Erro não foi encontrado dispositivo OpenCL de tipo "+dispositivo));


    /////////////////////////////////////////////////////////////////
    // Detect OpenCL devices
    /////////////////////////////////////////////////////////////////
    /* First, get the size of device list */
    resultCL = clGetContextInfo(contextCL,
                                CL_CONTEXT_DEVICES,
                                0,
                                NULL,
                                &deviceListSize);

    if (resultCL != CL_SUCCESS)
        throw(string("InitCL() Erro: Erro ao pedir informação de contexto."));
    if (deviceListSize == 0)
        throw(string("InitCL() Erro: O programa não encontrou dispositivos."));

    /* Now, allocate the device list */
    devicesCL = (cl_device_id *)malloc(deviceListSize);

    if (devicesCL == 0)
        throw(string("InitCL() Erro: Ao allocar memoria."));

    /* Next, get the device list data */
    resultCL = clGetContextInfo(contextCL,
                                CL_CONTEXT_DEVICES,
                                deviceListSize,
                                devicesCL,
                                NULL);

    if (resultCL != CL_SUCCESS)
        throw(string("InitCL() Erro: Erro ao pedir informação de contexto(device list, clGetContextInfo)"));


    cl_ulong buf_ulong;
    //determinar maxbuffersize
    resultCL=clGetDeviceInfo(devicesCL[0], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(buf_ulong), &buf_ulong, NULL);

    if (resultCL != CL_SUCCESS)
        throw(string("InitCL() Erro: Erro ao determinar CL_DEVICE_MAX_MEM_ALLOC_SIZE"));


    //maxbuffersize multiplo de 64bits
    if( buf_ulong % 8 )
    	maxbuffersize = buf_ulong;
    else{
    	maxbuffersize = buf_ulong - (buf_ulong%8);
    }

    //////////////////////////////////////////////////////////////////
    // Create an OpenCL command queue
    /////////////////////////////////////////////////////////////////

    queueCL = clCreateCommandQueue(contextCL,
                                            devicesCL[0],
                                            0,
                                            &resultCL);

    if ((resultCL != CL_SUCCESS) || (queueCL == NULL))
        throw(string("InitCL() Erro: Erro ao criar Command Queue. (clCreateCommandQueue)"));


    /////////////////////////////////////////////////////////////////
    // Load CL file, build CL program object, create CL kernel object
    /////////////////////////////////////////////////////////////////

    std::string  sourceStr = FileToString(kernelFileCL);
    const char * source    = sourceStr.c_str();
    size_t sourceSize[]    = { strlen(source) };


    programCL = clCreateProgramWithSource(contextCL,
                                                    1,
                                                    &source,
                                                    sourceSize,
                                                    &resultCL);

    if ((resultCL != CL_SUCCESS) || (programCL == NULL))
        throw(string("InitCL() Erro: Ao carregar o binario no cl_program. (clCreateProgramWithBinary)"));


    resultCL = clBuildProgram(programCL, 1, devicesCL, NULL, NULL, NULL);


    if ((resultCL != CL_SUCCESS) || (programCL == NULL))
    {
        cerr << "InitCL() Erro: no clBuildProgram" << endl;

		size_t length;
        resultCL = clGetProgramBuildInfo(programCL,
                                        devicesCL[0],
                                        CL_PROGRAM_BUILD_LOG,
                                        0,
                                        NULL,
                                        &length);
        if(resultCL != CL_SUCCESS)
            throw(string("InitCL() Erro: Ao pedir o Program build info(clGetProgramBuildInfo)"));

		char* buffer = (char*)malloc(length);
        resultCL = clGetProgramBuildInfo(programCL,
                                        devicesCL[0],
                                        CL_PROGRAM_BUILD_LOG,
                                        length,
                                        buffer,
                                        NULL);
        if(resultCL != CL_SUCCESS)
            throw(string("InitCL() Erro: Ao pedir o Program build info(clGetProgramBuildInfo)"));

		cerr << buffer << endl;
        free(buffer);

        throw(string("InitCL() Erro: Building Program (clBuildProgram)"));
    }


    //cirar o kernel DES
    /* get a kernel object handle for a kernel with the given name */
    cl_kernel kernel = clCreateKernel(programCL,
    		"DES",
    		&resultCL);

    if ((resultCL != CL_SUCCESS) || (kernel == NULL))
    {
    	string errorMsg = "InitCL() Erro: Ao criar o Kernel (clCreateKernel) DES";
    	throw(errorMsg);
    }

    kernelCL.push_back(kernel);

}

/*
 * Release OpenCL resources
 */
void ReleaseCL()
{
    bool errorFlag = false;

    for (unsigned int nKernel = 0; nKernel < kernelCL.size(); nKernel++)
    {
        if (kernelCL[nKernel] != NULL)
        {
            cl_int resultCL = clReleaseKernel(kernelCL[nKernel]);
            if (resultCL != CL_SUCCESS)
            {
                cerr << "ReleaseCL() Erro: No clReleaseKernel" << endl;
                errorFlag = true;
            }
            kernelCL[nKernel] = NULL;
        }
        kernelCL.clear();
    }

    if (programCL != NULL)
    {
        cl_int resultCL = clReleaseProgram(programCL);
        if (resultCL != CL_SUCCESS)
        {
            cerr << "ReleaseCL() Erro: No clReleaseProgram" << endl;
            errorFlag = true;
        }
        programCL = NULL;
    }

    if (queueCL != NULL)
    {
        cl_int resultCL = clReleaseCommandQueue(queueCL);
        if (resultCL != CL_SUCCESS)
        {
            cerr << "ReleaseCL() Erro: No clReleaseCommandQueue" << endl;
            errorFlag = true;
        }
        queueCL = NULL;
    }

    if (devicesCL != NULL)
    {
    	free(devicesCL);
    	devicesCL=NULL;
    }

    if (contextCL != NULL)
    {
        cl_int resultCL = clReleaseContext(contextCL);
        if (resultCL != CL_SUCCESS)
        {
            cerr << "ReleaseCL() Erro: No clReleaseContext" << endl;
            errorFlag = true;
        }
        contextCL = NULL;
    }

    if (errorFlag) throw(string("ReleaseCL() Erro encontrado."));
}


/*
 * Bind variables and CL buffers to kernel arguments
 */
void SetKernelArgs(int vezes)
{
    cl_int   resultCL;

    /* input file */
    resultCL = clSetKernelArg(kernelCL[0],
                                0,
                                sizeof(cl_mem),
                                (void *)&inputCL);

    if (resultCL != CL_SUCCESS)
        throw(string("SetKernelArgs() Erro: Ao definir um argumento do  kernel."));

    /* subkeys */
    resultCL = clSetKernelArg(kernelCL[0],
                                1,
                                sizeof(cl_mem),
                                (void *)&subkeysCL);

    if (resultCL != CL_SUCCESS)
    	throw(string("SetKernelArgs() Erro: Ao definir um argumento do  kernel."));

    /*numero a percorrer (nvidia ....)*/
    resultCL = clSetKernelArg(kernelCL[0],
                                2,
                                sizeof(cl_int),
                                (void *)&vezes);

    if (resultCL != CL_SUCCESS)
    	throw(string("SetKernelArgs() Erro: Ao definir um argumento do  kernel."));
}


/////////////////////////////////////////////////////////////////
// CL Buffers
/////////////////////////////////////////////////////////////////

void InitCLBuffers(long dim,unsigned long long * subkeys)
{
    cl_int resultCL;

    inputCL = NULL;
    subkeysCL = NULL;

    /////////////////////////////////////////////////////////////////
    // Create OpenCL memory buffers
    /////////////////////////////////////////////////////////////////
    inputCL = clCreateBuffer(contextCL,
                                    CL_MEM_READ_WRITE,
                                    sizeof(cl_ulong) * dim,
                                    NULL,
                                    &resultCL);

    if ((resultCL != CL_SUCCESS) || (inputCL == NULL))
        throw(string("InitCLBuffers() Erro: clCreateBuffer (inputCL)"));


    subkeysCL = clCreateBuffer(contextCL,
                                    CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                    sizeof(cl_ulong) * 17,
                                    subkeys,
                                    &resultCL);

    if ((resultCL != CL_SUCCESS) || (subkeysCL == NULL))
        throw(string("InitCLBuffers() Erro: clCreateBuffer (subkeysCL)"));

}

//actualização do buffer de input e do tamanho a processar
void RefreshBuffer(unsigned char* inByteArray,long dim){

	cl_int   resultCL;

	resultCL=clEnqueueWriteBuffer(queueCL, inputCL,CL_TRUE, 0, sizeof(cl_ulong) * dim,inByteArray, 0, NULL, NULL);

	if (resultCL != CL_SUCCESS)
		throw(string("RefreshBuffer() Erro: ao actualizar o input buffer. (clEnqueueWriteBuffer)"));


    /*numero a percorrer (nvidia ....)*/
    resultCL = clSetKernelArg(kernelCL[0],
                                2,
                                sizeof(cl_int),
                                (void *)&dim);

    if (resultCL != CL_SUCCESS)
        throw(string("RefreshBuffer() Erro: Ao definir um argumento do  kernel."));

}

void ReleaseCLBuffers()
{
    bool errorFlag = false;

    if (inputCL != NULL)
    {
        cl_int resultCL = clReleaseMemObject(inputCL);
        if (resultCL != CL_SUCCESS)
        {
            cerr << "ReleaseCLBuffers() Erro: clReleaseMemObject (inputBuffer)" << endl;
            errorFlag = true;
        }
        inputCL = NULL;
    }

    if (subkeysCL != NULL)
    {
        cl_int resultCL = clReleaseMemObject(subkeysCL);
        if (resultCL != CL_SUCCESS)
        {
            cerr << "ReleaseCLBuffers() Erro: clReleaseMemObject (subkeysBuffer)" << endl;
            errorFlag = true;
        }
        subkeysCL = NULL;
    }

    if (errorFlag)
        throw(string("ReleaseCLBuffers() Erro encontrado."));
}


void EnqueueKernel(int vezes, bool bBlocking = false)
{

    cl_event events[1];
    cl_int   resultCL;

    //definição do tamanho da global_work_size e local_work_size
    const size_t workdim = shrRoundUp(256,vezes); //multiplo de 256 (nvidia...)
    const size_t localdim = 256;


    resultCL = clEnqueueNDRangeKernel(queueCL,
                                     kernelCL[0],
                                     1,
                                     NULL,
                                     &workdim,
                                     &localdim,
                                     0, NULL,
                                     &events[0]);

    if (resultCL != CL_SUCCESS)
        throw(string("CallKernel() Erro: Ao por o kernel na command queue. (clEnqueueNDRangeKernel)"));

    /* wait for the kernel call to finish execution */
    if (bBlocking)
    {
        resultCL = clWaitForEvents(1, &events[0]);
        if (resultCL != CL_SUCCESS)
            throw(string("CallKernel() Erro: Ao esperar pela execução do kernel. (clWaitForEvents)"));
    }

}


unsigned char* encryptDES_CL(unsigned char* inByteArray, long dim, unsigned long long key, int type,DeviceType device){

	unsigned long long subKeys[17];
	int i,j;
	cl_int   resultCL;

	//obtem sub-keys (16 de comprimento 48)
	DESKeySchedule(key,subKeys);


	if (type == 1) //decrypt --> inverter subKeys
	{
		unsigned long long tempcpy;
		for(i=1,j=16;i<j;i++,j--){

			tempcpy=subKeys[i];
			subKeys[i]=subKeys[j];
			subKeys[j]=tempcpy;
		}
	}

	//determina os workitems(tamanho do ficheiro a dividir por 64bits)
	availableworkitems=dim/8;

	try{

		InitCL(device);

		InitCLBuffers(maxbuffersize/8,subKeys);

		SetKernelArgs(0);

		unsigned long tamanho,vezes=0;
		uint indice=0;

		//enquanto houver workitems envia para o buffer o processa
		while (availableworkitems>0){

			//determina o tamanho a copiar do ficheiro de input
		    if( availableworkitems * 8 <= maxbuffersize ){
		    	tamanho = availableworkitems * 8 ;
				availableworkitems=0;
		    }
		    else{
		    	tamanho = maxbuffersize;
		    	availableworkitems=availableworkitems-(maxbuffersize/8);
		    }

		    vezes=tamanho/8;

		    //faz refresh do buffer input no device
			RefreshBuffer(&inByteArray[indice],vezes);

			//corre o kernel e espera pela conclusão
		    EnqueueKernel(vezes);
		    clFinish(queueCL);

		    //le o buffer depois de processado no device e copia para o host
			resultCL=clEnqueueReadBuffer(queueCL, inputCL,CL_TRUE, 0, sizeof(cl_ulong) * vezes,&inByteArray[indice], 0, NULL, NULL);

			if (resultCL != CL_SUCCESS)
				throw(string("Erro : Ao ler o buffer. (clEnqueueReaderBuffer)"));

			//actualiza o indice do ficheiro de input
			indice+=tamanho;
		}


		ReleaseCLBuffers();
		ReleaseCL();


		//tratamento da parte não encriptada
		i=dim-dim%8;
		while (i < dim)

			{
			unsigned long long plain = 0;
				j = i;
				while (j < i + 8 && j < dim)
				{
		 			plain = plain | ((unsigned long long)inByteArray[j] << (64 - 8*(j-i+1)));
					j++;
				}
				j = i;
				while (j < i + 8 && j < dim)
				{
					inByteArray[j] = (unsigned char) (plain >> (56 - 8*(j-i)) & (0xFF));
					j++;
				}
				i=j;
			}


		return inByteArray;


	}catch(std::string msg)
	{
		cerr << endl << msg << endl << endl;
		ReleaseCL();
	}
	catch(...)
	{
		cerr << "Exception caught in encryptDES_CL()" << endl;
		ReleaseCL();
	}

	return NULL;
}
