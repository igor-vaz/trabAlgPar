#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cilk/cilk.h>
#include "timer.h"
#include <time.h>

// radix sort com soma de prefixo

void imprimeMatriz(int **matriz, int n)
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
//	printf("vetor de entrada: ");
//	for(i = 0; i<tamanho;i++)
//		printf("%d ",vetor[i] );
//	printf("\n");
	i = 0;
	int *ret = (int *)calloc(4*tamanho, sizeof(int));
	sum = vetor[0];
	ret[0] = sum;
	while(i < tamanho){
		i+=1;
		sum +=vetor[i];
		ret[i] = sum;
	}
	return ret;
}

int* ParSomaPrefix(int vetor[], int n){
  int logn = (int)log2(n);
  int B[logn][n];
  int C[logn][n];
  int i, j;
  int *ret = (int*)calloc(n,sizeof(int)+1);

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

  return ret;
}

int *get_bit(int n, int bitswanted){
  int *bits = malloc(4*sizeof(int) );
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

    b = (int *)calloc(4*tamanho, sizeof(int));

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
	int *bit= (int *)calloc(4*tamanho, sizeof(int));
	int marcabit[tamanho];
	int aux[tamanho];
	int *prefix= (int *)calloc(4*tamanho+1, sizeof(int));
	int nUns = 0;
	int swap = 0;
	for (k = 0; k < tamanho; k++)
		aux[k] = 0;

	//i=1;
	for (i = 1; i <= nbits; i++){
		cilk_for(j = 0; j < tamanho; j++){
			bit = get_bit(vetor[j], i);
		//	printf("valor = %d ",vetor[j] );
		//	printf("getbit #%d = %d ",i,bit[0]);
		//	printf("\n");
			if( bit[0] == 0){
				marcabit[j] = 1;
				// printf("marcabit %d\n ", marcabit[j]);
			}
			else{
				marcabit[j] = 0;
				// printf("marcabit %d\n ", marcabit[j]);
			}
		}
		prefix = ParSomaPrefix(marcabit, tamanho);
		nUns = prefix[tamanho];
		//printf("prefix: ");

		// printf("nUns: %d\n", nUns);
		// printf("vetor: ");


		// printf("prefix: ");
		// for (j = 0; j < tamanho; j++)
		// 	printf("%d ", prefix[j] );
		// printf("\n");

		cilk_for (j = 0; j < tamanho; j++){
			//bit = get_bit(vetor[j], i);
			
			int k;
			// for (k = 0; k < tamanho; k++)
			// 	printf("%d ", vetor[k] );
			// printf("\n");
			if(marcabit[j] == 1){
				//if(vetor[prefix[j]]<vetor[j]){
					// printf("prefix[%d] = %d \n", j,prefix[j]);
					// printf("vetor[%d] = %d \n", j,vetor[j]);
				// printf("j:%d\n",j );
		 		aux[prefix[j]] = vetor[j];
		 		//vetor[j] = aux[prefix[j]];
				// for (k = 0; k < tamanho; k++)
				// 	printf("%d ", aux[k] );
				// printf("\n");
					
					// swap = vetor[prefix[j]];
					// vetor[prefix[j]] = vetor[j];
					// vetor[j] = swap;
					
				//}
			}
		 	else{
		 		// printf("j:%d\n",j );
		 		aux[j+nUns-prefix[j]] = vetor[j];
		 	// 	for (k = 0; k < tamanho; k++)
				// 	printf("%d ", aux[k] );
				// printf("\n");
		 		//if (vetor[j+nUns-prefix[j]] < vetor[j]){
			 		// swap = vetor[j+nUns-prefix[j]];
			 		// vetor[j+nUns-prefix[j]] = vetor[j];
			 		// vetor[j] = swap;
		 			
		 		//}		 		
		 	}
		}
		 	cilk_for(k = 0; k < tamanho; k++)
		 		vetor[k] = aux[k];
		 // 	printf("vetor :");
		 // 	cilk_for(k = 0; k < tamanho; k++)
		 // 		printf("%d ", vetor[k]);
			// printf("\n");
	}
	// for (i = 0; i < tamanho; i++)
	// 	printf("%d ", aux[i]);
	// printf("\n");
	
		printf("%d ", vetor[tamanho-1]);
		
}

int main(int argc, char const *argv[]){
	int i,j;
  	double inicio,fim;
	int tamanho = 8000;
	int nbits = (int)ceil(log2(tamanho))+1;
	int *vetor = (int*)malloc(tamanho*sizeof(int));
	srand(time(NULL));
	
	for (i = 0; i < tamanho; i++)
	{
		vetor[i] = tamanho-i;
		// printf("%d ",vetor[i] );
	}
	// printf("\n");

	GET_TIME(inicio);
	radixsort2(vetor,tamanho,nbits);
	printf("\n");
	GET_TIME(fim);
  	printf("tempo: %lf\n", fim-inicio);

	return 0;
}