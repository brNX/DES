#include "DES.h"
#include "utils.h"
#include <iostream>

//forward declaration
unsigned char* encryptDES_CL(unsigned char* inByteArray, long dim, unsigned long long key, int type,DeviceType device);


/**tabelas de permutação**/
int pc1[] = {57,49,41,33,25,17,9,1,58,50,42,34,26,18,10,2,59,51,43,35,27,19,11,3,60,52,44,36,63,55,47,39,31,23,15,7,62,54,46,38,30,22,14,6,61,53,45,37,29,21,13,5,28,20,12,4};
int pc2[] = {14,17,11,24,1,5,3,28,15,6,21,10,23,19,12,4,26,8,16,7,27,20,13,2,41,52,31,37,47,55,30,40,51,45,33,48,44,49,39,56,34,53,46,42,50,36,29,32};
int ip[] = {58,50,42,34,26,18,10,2,60,52,44,36,28,20,12,4,62,54,46,38,30,22,14,6,64,56,48,40,32,24,16,8,57,49,41,33,25,17,9,1,59,51,43,35,27,19,11,3,61,53,45,37,29,21,13,5,63,55,47,39,31,23,15,7};
int ip_1[] = {40,8,48,16,56,24,64,32,39,7,47,15,55,23,63,31,38,6,46,14,54,22,62,30,37,5,45,13,53,21,61,29,36,4,44,12,52,20,60,28,35,3,43,11,51,19,59,27,34,2,42,10,50,18,58,26,33,1,41,9,49,17,57,25};
int e[] = {32,1,2,3,4,5,4,5,6,7,8,9,8,9,10,11,12,13,12,13,14,15,16,17,16,17,18,19,20,21,20,21,22,23,24,25,24,25,26,27,28,29,28,29,30,31,32,1};
int p[] = {16,7,20,21,29,12,28,17,1,15,23,26,5,18,31,10,2,8,24,14,32,27,3,9,19,13,30,6,22,11,4,25};

//Tabela 3: As S-Boxes do DES.
int s[8][4][16] = {
	//S1
	14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7,
	0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8,
	4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0,
	15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13,
	//S2
	15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10,
	3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5,
	0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15,
	13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9,
	//S3
	10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8,
	13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1,
	13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7,
	1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12,
	//S4
	7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15,
	13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9,
	10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4,
	3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14,
	//S5
	2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9,
	14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6,
	4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14,
	11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3,
	//S6
	12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11,
	10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8,
	9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6,
	4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13,
	//S7
	4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1,
	13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6,
	1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2,
	6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12,
	//S8
	13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7,
	1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2,
	7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8,
	2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11
};


//encryption function
int DES (const char* inFileName, unsigned long long key,DeviceType device)
{
	return DESgeneral(inFileName, key, 0,device);
}


//decryption function
int unDES (const char* inFileName, unsigned long long key,DeviceType device)
{
	return DESgeneral(inFileName, key, 1,device);
}


//general en/decryption function 
int DESgeneral (const char* inFileName, unsigned long long key, int type,DeviceType device)
{
	FILE* DESInFile;
	unsigned char* inByteArray=NULL;
	long inFileSize;
	unsigned char* encByteArray=NULL;
	char* outFileName=NULL;
	int write;
	char response;
	struct stat stFileInfo;
	FILE* DESOutFile=NULL;
	char suf[5];

	//abrir ficheiro e ler tamanho
	DESInFile = fopen(inFileName, "rb");
	if (DESInFile == NULL)
	{
		printf("Error opening file for reading. Exiting...\n");
		return 1;
	}
	fseek(DESInFile, 0L, SEEK_END);
	inFileSize = ftell(DESInFile);  //ignore EOF
	fseek(DESInFile, 0L, SEEK_SET);

	//read to array inByteArray	
	inByteArray = (unsigned char*) calloc(inFileSize, sizeof(unsigned char)); 
	fread(inByteArray, 1, inFileSize, DESInFile);

	//encrypting array
	switch(device)
	{
	case GPU: printf("a usar opencl na gpu\n");encByteArray = encryptDES_CL(inByteArray, inFileSize, key, type,device); break;
	case CPU: printf("a usar opencl na cpu\n");encByteArray = encryptDES_CL(inByteArray, inFileSize, key, type,device); break;
	case ACCELERATOR: printf("a usar opencl no accelerator \n");encByteArray = encryptDES_CL(inByteArray, inFileSize, key, type,device);break;
	case OPENMP:
	default:
		printf("a usar openmp\n");
		encByteArray = encryptDES(inByteArray, inFileSize, key, type);
	}


	if(encByteArray != NULL){

		//flush encByteArray to file

		if (type == 0)  
		{
			outFileName = (char*) calloc(strlen(inFileName) + 5, sizeof(char)); 
			strcpy(outFileName, inFileName);
			strcat(outFileName, ".DES");
		}
		else  
		{
			strcpy(suf, &inFileName[strlen(inFileName) - 4]);
			if (strcmp(suf, ".DES") == 0)
			{
				outFileName = (char*) calloc(strlen(inFileName) + 5, sizeof(char));
				strcpy(outFileName, "DES_");
				strcat(outFileName, inFileName);
				outFileName[strlen(outFileName) - 4] = 0;
			}
			else
			{
				outFileName = (char*) calloc(14, sizeof(char));
				strcpy(outFileName, "DES_decrypted");
			}

		}
		//create file
		write = 1;
		if(stat(outFileName, &stFileInfo) == 0) //see if file already exists
		{
			printf ("File already exists. Overwrite (y/n)?: ");
			response = getchar();
			if (response == 'n')
				write = 0;
			printf("\n");
			fflush(stdin);
		}

		if (write)
		{
			DESOutFile = fopen(outFileName, "wb");
			if (DESOutFile == NULL)
			{
				printf("Error opening file for writing. Exiting...\n");
				return -1;
			}
			fwrite(encByteArray, 1, inFileSize, DESOutFile);
			fclose(DESOutFile);
		}
	}
	//finalizações
	free(inByteArray);
	inByteArray=NULL;
	free(outFileName);
	outFileName=NULL;
	if(device==OPENMP)
		free(encByteArray);
	fclose(DESInFile);

	return 0;	
}


//encryption of inByteArray with size dim 
unsigned char* encryptDES(unsigned char* inByteArray, long dim, unsigned long long key, int type)
{	
	unsigned char* outByteArray;
	unsigned long long subKeys[17];
	int i,j;

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
	
	outByteArray = (unsigned char*) calloc(dim, sizeof(unsigned char)); 
	int vezes=dim/8;



	#pragma omp parallel for shared(vezes,outByteArray) private (i)
	for (i=0;i<vezes;i++)
	{
		unsigned long long plain=0, cipher;
		int indice=i*8;
		int x;

		for (x=0;x<8;x++)
			plain = plain | ((unsigned long long)inByteArray[indice+x] << (64 - 8*(x+1)));

		cipher = encryptDESplain(plain,subKeys);

		for (x=0;x<8;x++)
			outByteArray[indice+x] = (unsigned char) (cipher >> (56 - 8*(x)) & (0xFF));

	}

	//plain part
	i=vezes*8;
	while (i < dim)

		{
		unsigned long long plain = 0;
			j = i;
			while (j < i + 8 && j < dim)
			{
	 			plain = plain | ((unsigned long long)inByteArray[j] << (64 - 8*(j-i+1)));
				j++;
			}
			//guarda cifra no array de saida
			j = i;
			while (j < i + 8 && j < dim)
			{
				outByteArray[j] = (unsigned char) (plain >> (56 - 8*(j-i)) & (0xFF));
				j++;
			}
			i=j;
		}

	return outByteArray;
}


// encrypting 64 bits using subKeys
unsigned long long encryptDESplain(unsigned long long plain, unsigned long long* subKeys)
{

	int i;
	unsigned long long bufferIP;
	unsigned int  parteD,parteDA;
	unsigned int parteE,parteEA;
	unsigned long long parteDE;
	unsigned long long outtemp;
	
	unsigned long long  r48=0, xOR48=0;
	unsigned int s32=0, p32=0;
	int linhas, colunas;
	int j;

	//IP permutation
	bufferIP=permuta(plain,ip,64,64);
	
	//dividir a mensagem em 2 partes
	parteEA = bufferIP>>32;
	parteDA = bufferIP;

	//16 iterations
	for(i=1;i<=16;i++) {

		r48=0;
		xOR48=0;
		s32=0;
		p32=0;


		parteE = parteDA;


		r48=permuta(parteDA, e, 48,32);


		xOR48= r48 ^ subKeys[i];

		//S-Box transformation
		for (j=0; j<8; j++) {

			
			linhas = (xOR48>>(47-6*j-1) & 2) |  (xOR48>>(47-6*j-5) & 1);

			
			colunas = (xOR48>>(47-6*j-4) & 0xF);

			
			s32 |= s[j][linhas][colunas];

			if(j!=7)
				s32=s32<<4;
		}

		p32=permuta(s32, p, 32, 32);

		parteD = parteEA ^ p32;

		parteDA=parteD;
		parteEA=parteE;
	}
	

	parteDE = parteD;
	parteDE = parteDE<<32;
	parteDE = parteDE | parteE;

	outtemp=permuta(parteDE,ip_1,64,64);
	
	return outtemp;
}


// sub-keys generation
void DESKeySchedule(unsigned long long key,unsigned long long * subKeys)
{
	int i;
	unsigned long long chave56;
	unsigned int chaveC16[17];
	unsigned int chaveD16[17];
	unsigned long long chaveCD56[17];
	
	

	chave56=permuta(key, pc1, 56, 64);


    chaveD16[0] = chave56;
	chaveC16[0] = chave56>>28;


	for (i=0; i<=16; i++) {

		if (i==0) { 

			chaveCD56[i] = chaveC16[i];
			chaveCD56[i] = chaveCD56[i]<<28;
			chaveCD56[i] = chaveCD56[i] | chaveD16[i];
			continue;
		}
	
		if ( (i==1) || (i==2) || (i==9) || (i==16)) {


			chaveC16[i]=(rotateleft(chaveC16[i-1], 28,1))&0xFFFFFFF;

			chaveCD56[i] = chaveC16[i];
			chaveCD56[i]=chaveCD56[i]<<28;

			chaveD16[i]=(rotateleft(chaveD16[i-1],28,1))&0xFFFFFFF;

			chaveCD56[i] = chaveCD56[i]  | chaveD16[i];


		} else {
		
			chaveC16[i]=(rotateleft(chaveC16[i-1], 28,2))&0xFFFFFFF;

			chaveCD56[i] = chaveC16[i];
			chaveCD56[i]=chaveCD56[i]<<28;

			chaveD16[i]=(rotateleft(chaveD16[i-1],28,2))&0xFFFFFFF;;
			chaveCD56[i] = chaveCD56[i]  | chaveD16[i];
		}
	}


	for (i=1; i<=16; i++) {
		subKeys[i]=permuta(chaveCD56[i], pc2, 48, 56);

	}

}

unsigned long long permuta(unsigned long long b, int subs[],int tamanho, int blockSize) {

	unsigned long long bytes = 0;
	unsigned long long bit;
	int i;

	for (i = 0; i < tamanho; i++) {
		bit=0;

	
		bit= (b >> (blockSize - subs[i]))& 1;


		bytes = (bytes | bit);

		if(i!=(tamanho-1))
			bytes=bytes<<1;
	}
	return bytes;
}


unsigned long long rotateleft(unsigned long long value,int tamanho, int shift){
	return (value << shift) | (value >> (tamanho - shift));
}
