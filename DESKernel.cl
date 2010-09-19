//tabelas de permutação

constant uint ip[] = {58,50,42,34,26,18,10,2,60,52,44,36,28,20,12,4,62,54,46,38,30,22,14,6,64,56,48,40,32,24,16,8,57,49,41,33,25,17,9,1,59,51,43,35,27,19,11,3,61,53,45,37,29,21,13,5,63,55,47,39,31,23,15,7};
constant uint ip_1[] = {40,8,48,16,56,24,64,32,39,7,47,15,55,23,63,31,38,6,46,14,54,22,62,30,37,5,45,13,53,21,61,29,36,4,44,12,52,20,60,28,35,3,43,11,51,19,59,27,34,2,42,10,50,18,58,26,33,1,41,9,49,17,57,25};
constant uint e[] = {32,1,2,3,4,5,4,5,6,7,8,9,8,9,10,11,12,13,12,13,14,15,16,17,16,17,18,19,20,21,20,21,22,23,24,25,24,25,26,27,28,29,28,29,30,31,32,1};
constant uint p[] = {16,7,20,21,29,12,28,17,1,15,23,26,5,18,31,10,2,8,24,14,32,27,3,9,19,13,30,6,22,11,4,25};

//Tabela 3: As S-Boxes do DES.
constant uint s[8][4][16] = {
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


#define BSWAP_64(x)     (((ulong)(x) << 56) | \
                        (((ulong)(x) << 40) & 0xff000000000000UL) | \
                        (((ulong)(x) << 24) & 0xff0000000000UL) | \
                        (((ulong)(x) << 8)  & 0xff00000000UL) | \
                        (((ulong)(x) >> 8)  & 0xff000000UL) | \
                        (((ulong)(x) >> 24) & 0xff0000UL) | \
                        (((ulong)(x) >> 40) & 0xff00UL) | \
                        ((ulong)(x)  >> 56))


ulong swap(ulong msg) {
   uint i,j;
   ulong plain=0;

   for (i=0;i<8;i++)
     for(j=0;j<8;j++){
        plain |= ((msg >> 8*i+7-j) & 0x1);
        if(!(i==7 && j==7))
            plain = plain << 1;
     }
  return plain;
}


//funçao que permuta o buffer de entrada seguindo a ordemdepermutação
ulong permuta(ulong b, constant uint subs[],uint tamanho, uint blockSize) {

	ulong bytes = 0;
	ulong bit;
	uint shifts;
	int i;
	for (i = 0; i < tamanho; i++) {
		bit=0;
		shifts=subs[i];
		bit = (b >> (blockSize - shifts))& 1;
		bytes = (bytes | bit);

		if(i!=(tamanho-1))
			bytes=bytes<<1;
	}
	return bytes;
}


__kernel void DES( __global ulong * pInput,
		__constant ulong * pSubkeys,
		int nNumElements)
{
	const uint indice = get_global_id(0);

	//por causa das nvidia...
	if(indice>=nNumElements)
		return;

	int i;
	ulong bufferIP,plain=0;
	uint parteDA,parteD;
	uint parteE,parteEA;
	ulong parteDE;

	plain = swap(pInput[indice]);

	//Permuta por IP
	bufferIP=permuta(plain,ip,64,64);

	//dividir a mensagem em 2 partes
	parteEA = bufferIP>>32;
	parteDA = bufferIP;

	//16 iterações para cada uma das chaves
	for(i=1;i<17;i++) {

		ulong  r48, xOR48;
		uint s32=0, p32;
		uint linhas, colunas;
		uint j;

		//Parte Esquerda -> Ln = Rn-1
		parteE = parteDA;

		//Permutação da parte Direita da Mensagem segundo a tabela E
		r48=permuta(parteDA, e, 48,32);

		//XOR -> xor da parte direita com cada uma das chaves
		xOR48= r48 ^ pSubkeys[i];


		//Transformação S-Boxes
		for (j=0; j<8; j++) {

			//junta o primeiro e sexto bit para encontrar a linha
			linhas = (xOR48>>(47-6*j-1) & 2) |  (xOR48>>(47-6*j-5) & 1);

			//junta os restantes bits (do meio) para encontrar a coluna
			colunas = (xOR48>>(47-6*j-4) & 0xF);

			s32 |= s[j][linhas][colunas];

			if(j!=7)
				s32=s32<<4;	
		}


		//Permutação por p
		p32=permuta(s32, p, 32, 32);


		parteD = parteEA ^p32;
		
		parteDA=parteD;
		parteEA=parteE;
	
	}

	//juntar invertendo a ordem
	parteDE = parteD;
	parteDE = parteDE<<32;
	parteDE = parteDE | parteE;

	//permuta parteDE[16] segundo a Inversa de IP
	plain=permuta(parteDE,ip_1,64,64);

	pInput[indice]=swap(plain);
	
}
