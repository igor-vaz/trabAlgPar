#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <cilk/cilk.h>
#include "timer.h"


int *A, *in, *out;
int **B,**	C;
int n;

// radix sort com soma de prefixo

void imprimeMatriz(int **matriz)
{
	int i,j;

	for(i=0;i<n;i++)
	{
		for(j=0;j<log(n);j++)
		{
			printf("%d ",matriz[i][j]);
		}
		printf("\n");
	}
}

int* SomaPrefix(int vetor[], int tamanho){
	int i,sum;
	i = 0;
	int *ret = (int *)calloc(tamanho, sizeof(int));
	sum = vetor[0];
	ret[0] = sum;
	while(i < tamanho){
		i+=1;
		sum +=vetor[i];
		ret[i] = sum;
	}
	return ret;
}

void ParSomaPrefix(){
	int h,j;
	
	for(h = 1; h < n; h++)
		B[0][h] = A[h];

	
	for (h = 1; h < log(n); h++)
		for (j = 1; j < (int)(n/pow(2,h)); j++){
			B[h][(int)j] = B[h-1][(int)(2j-1)]*B[h-1][(int)(2j)];
		}

	for (h = log(n); h < 1 ; h--)
		for (j = 1; j < (n/pow(2,h)); j++)
			if(j % 2 == 0)
				C[h][(int)j] = C[h+1][(int)(j/2)];
			else if(j == 1)
				C[h][1] = B[h][1];
			else if ((j%2 != 0) && j > 1)
				C[h][(int)j]=C[h+1][(int)((j-1)/2)]*B[h][(int)j];
}


int *get_bit(int n, int bitswanted){
  int *bits = malloc(sizeof(int) );
  int thebit;
  int k;
  for(k=0; k<bitswanted; k++){
    int mask =  1 << k;
    int masked_n = n & mask;
    thebit = masked_n >> k;
    bits[0] = thebit;
  }

  return bits;
}

void radixsort(int vetor[], int tamanho) {
    int i;
    int *b;
    int maior = vetor[0];
    int exp = 1;

    b = (int *)calloc(tamanho, sizeof(int));

    for (i = 0; i < tamanho; i++) {
        if (vetor[i] > maior)
    	    maior = vetor[i];
    }
 
    while (maior/exp > 0) {
        int bucket[10] = { 0 };
    	for (i = 0; i < tamanho; i++)
    	    bucket[(vetor[i] / exp) % 10]++; 
    	for (i = 1; i < 10; i++)
    	    bucket[i] += bucket[i - 1];
    	for (i = tamanho - 1; i >= 0; i--)
    	    b[--bucket[(vetor[i] / exp) % 10]] = vetor[i];
    	for (i = 0; i < tamanho; i++)
    	    vetor[i] = b[i];
    	exp *= 10;
    }

    free(b);
}


void radixsort2(int vetor[],int tamanho, int nbits){
	int i,j,k;
	int *bit= (int *)calloc(tamanho, sizeof(int));
	int marcabit[tamanho];
	int aux[tamanho];
	int *prefix= (int *)calloc(tamanho, sizeof(int));
	int nUns;

	for (i = 1; i <= nbits; i++){
		for(j = 0; j < tamanho; j++){
			bit = get_bit(vetor[j], i);
			printf("valor = %d ",vetor[j] );
			printf("getbit #%d = %d ",i,bit[0]);
			if( bit[0] == 0){
				marcabit[i] = 1;
				printf("marcabit %d\n ", marcabit[i]);
			}
			else{
				marcabit[i] = 0;
				printf("marcabit %d\n ", marcabit[i]);
			}
		}
		prefix = SomaPrefix(marcabit, tamanho);
		nUns = prefix[tamanho-1];
		// for (j = 0; j < tamanho; j++)
		// 	if(marcabit[j] == 0)
		// 		aux[prefix[j]] = vetor[j];
		// 	else
		// 		aux[j+nUns-prefix[j]] = vetor[j];
	}
	printf("prefix: ");
	for (i = 0; i < tamanho; i++)
		printf("%d ", prefix[i]);

	printf("nUns: %d\n", nUns);


}

int main(int argc, char const *argv[]){
	int i,j;
	// double inicio,fim;
	// n = 10;
	// int*bit;
	int vetor[10]={10,9,8,7,6,5,4,3,2,1};
	// for(j = 0; j < 10; j++){
	// 	bit = get_bit(vetor[j], 1);
	// 	printf("%d\n", bit[0]);
	// }
	// in = (int*) malloc(n*sizeof(int));
	// out = (int*) malloc(n*sizeof(int));
	// A = (int*) malloc(n*sizeof(int));
	// B = (int**) malloc(n*sizeof(int*));
	// C = (int**) malloc(n*sizeof(int*));
	// for(i=0;i<n;i++)
	// {
	//  	*(B+i)=(int*) malloc(log(n)*sizeof(int));
	//  	*(C+i)=(int*) malloc(log(n)*sizeof(int));
	// 	A[i] = 1;
	// }
	// for(i=0;i<n;i++)
	// 	for(j=0;j<log(n);j++){
	//   		B[i][(int)j] = 0;
	//   		C[i][(int)j] = 0;
	//   	}

	// for(i=0;i<n;i++)
	// 	in[i] = i+1;
	
	// GET_TIME(inicio);
	// //SomaPrefix();
	// //cilk_spawn ParSomaPrefix();
	// //radixsort(vetor,10);
	// radixsort2(vetor,10);
	// GET_TIME(fim);
	// printf("tempo: %lf\n", fim-inicio);
	// // printf("vetor:");
	// for(i=0;i<n;i++){
	//  	printf("%d ", vetor[i]);
	// }
	// printf("\n");
  // int n=7;

  // int  bitswanted = 3;

  // int *bits = get_bit(n, bitswanted);

  // printf("%d = ", n);

  //for(i=bitswanted-1; i>=0;i--){
//    printf("%d ", bits[0]);
  //}

  // printf("\n");
  // int *b;
  // b = SomaPrefix(vetor,10);
  // printf("prefixo:");	
  // for (i = 0; i < n; i++)
  // {
  // 	printf("%d ", b[i]); 
  // }
 radixsort2(vetor,10,5);
  printf("\n");
	// printf("out:");
	// for(i=0;i<n;i++){
	// 	printf("%d ", out[i]);
	// }
	// printf("\n");
	//cilk_sync;
	//imprimeMatriz(C);
	return 0;
}