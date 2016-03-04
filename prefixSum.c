#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cilk/cilk.h>
#include "timer.h"
#include <time.h>
#include <unistd.h>

// radix sort com soma de prefixo

int **B, **C;

int* SomaPrefix(int vetor[], int tamanho){
	int i,sum;
	int *ret = (int*)malloc(tamanho*sizeof(int));
	i = 0;
	sum = vetor[0];
	ret[0] = sum;
	while(i < tamanho){
		i+=1;
		sum +=vetor[i];
		ret[i] = sum;
	}
	return ret;
}

void ParSomaPrefix(int vetor[], int n, int* ret){
	int logn = (int)log2(n)+1;
	int i, j;

	cilk_for (i = 0; i < n; i++)
  	{
    	B[0][i] = vetor[i];
  	}

  	for (i = 1; i <= logn; i++)
  	{
    	int pot =(int) pow(2,i);
    	cilk_for (j = 0; j < (n/pot); j++)
    	{
      		B[i][j] = B[i-1][2*j] + B[i-1][(2*j)+1];
    	}
  	}

	for (i = logn; i >= 0 ; i--)
	{
		int pot =(int) pow(2,i);
	  	cilk_for (j = 0; j < (n/pot); j++)
	  	{
	    	if (j == 0)
	      	{
	        	C[i][0] = B[i][0];
	        	if (i == 0)
	        	{
	          		ret[j+1] = B[i][0];
	        	}
	      	}
	      	else if (j % 2 == 0)
	      	{
	        	C[i][j] = C[i+1][(j-1)/2] + B[i][j];
	        	if (i == 0)
	        	{
	          		ret[j+1] = C[i+1][(j-1)/2] + B[i][j];
	        	}
	      	}
	      	else if (j % 2 == 1)
	      	{
	        	C[i][j] = C[i+1][j/2];
	        	if (i == 0)
	        	{
	          	ret[j+1] = C[i+1][j/2];
	        	}
	      	}
	    }
	} 
}

int get_bit(int n, int bitswanted){
	int thebit;
	int k;
	for(k=0; k<bitswanted; k++){
		int mask =  1 << k;
		int masked_n = n & mask;
		thebit = masked_n >> k;
	}
	return thebit;
}

void radixsort(int vetor[], int tamanho) {
    int i;
    int *b;
    int maior = vetor[0];
    int exp = 1;

    b = (int*)malloc(tamanho*sizeof(int));

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
	int bit;
	int *marcabit = (int*)malloc(tamanho*sizeof(int));
	int *aux = (int*)malloc(tamanho*sizeof(int));
	int *prefix = (int*)malloc(tamanho*sizeof(int)+1);
	int nUns = 0;
	
	for (i = 1; i <= nbits; i++){
		cilk_for(j = 0; j < tamanho; j++){
			bit = get_bit(vetor[j], i);
			if( bit == 0){
				marcabit[j] = 1;
			}
			else{
				marcabit[j] = 0;
			}
		}
		ParSomaPrefix(marcabit, tamanho,prefix);
		nUns = prefix[tamanho];
		cilk_for (j = 0; j < tamanho; j++){
			if(marcabit[j] == 1){
		 		aux[prefix[j]] = vetor[j];
			}
		 	else{
		 		aux[j+nUns-prefix[j]] = vetor[j];
		 	}
		}
		cilk_for(k = 0; k < tamanho; k++)
			vetor[k] = aux[k];
	}
	free(prefix);
	free(marcabit);
	free(aux);
}

int main(int argc, char const *argv[]){
	
	if (argc < 2) {
		printf("Use: %s <tamanho do vetor>\n", argv[0]);
    	exit(EXIT_FAILURE);
	}
	
	int i,j;
	int*ret;
  	double inicio,fim;
	//int tamanho = (int)pow(2,25);
	int tamanho = atoi(argv[1]);
	long int nbits = (int)ceil(log2(tamanho))+1;
	int *vetor = (int*)malloc(tamanho*sizeof(int));
	srand(time(NULL));
	int logn = (int)log2(tamanho)+1;
	B = (int**)malloc(logn*sizeof(int*));
	C = (int**)malloc(logn*sizeof(int*));

	for(i = 0; i < logn; i++){
		*(B+i) = (int*)malloc(tamanho*sizeof(int));
		*(C+i) = (int*)malloc(tamanho*sizeof(int));		
	}

	for (i = 0; i < tamanho; i++)
	{
		vetor[i] = rand()%tamanho;
	}

	printf("executando com tamanho %d\n",tamanho);
	GET_TIME(inicio);
	radixsort(vetor,tamanho);
	GET_TIME(fim);
  	printf("tempo sequencial: %lf\n", fim-inicio);

	GET_TIME(inicio);
	radixsort2(vetor,tamanho,nbits);
	GET_TIME(fim);
  	printf("tempo paralelo: %lf\n", fim-inicio);
  	printf("\n");

  	for(i = 0; i < logn; i++){
		free(*(B+i));
		free(*(C+i));		
	}
	free(B);
	free(C);

	return 0;
}