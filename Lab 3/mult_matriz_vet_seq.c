/* Multiplicacao de matriz-vetor (considerando matrizes quadradas) */
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"
#define ESCREVE

float *mat; //matriz de entrada
float *vet; //vetor de entrada
float *saida; //vetor de saida

//funcao que as threads executarao
void mult(float* mat, float* vet, int linhas, int colunas) {
   for(int i=0; i<linhas; i++)
      for(int j=0; j<colunas; j++) 
         saida[i] += mat[i*colunas + j] * vet[j];
}

//fluxo principal
int main(int argc, char* argv[]) {
   long int tam_vet; //tamanho do vetor lido
   int linhas, colunas, tam; // linhas e colunas a serem obtidas ao ler matriz e cálculo do seu tamanho 
   FILE *arq_matrix, *arq_vet_in, *arq_vet_out; //descritores dos arquivos de entrada e do arquivo de saída
   size_t ret; //retorno da funcao de leitura no arquivo de entrada
   double inicio, fim, delta;
   
   GET_TIME(inicio);
   //leitura e avaliacao dos parametros de entrada
   #ifdef ESCREVE
   if(argc<5) {
      printf("Digite: %s <arquivo matriz de entrada> <arquivo vetor de entrada> <arquivo vetor de saída> <arquivo de escrita dos tempos de execução>\n", argv[0]);
      return 1;
   }
   #else
   if(argc<4) {
      printf("Digite: %s <arquivo matriz de entrada> <arquivo vetor de entrada> <arquivo vetor de saída>\n", argv[0]);
      return 1;
   }
   #endif

   arq_matrix = fopen(argv[1], "rb");
   if(!arq_matrix) {
      fprintf(stderr, "Erro de abertura do arquivo da Matriz\n");
      return 2;
   } 

   arq_vet_in = fopen(argv[2], "rb");
   if(!arq_vet_in) {
      fprintf(stderr, "Erro de abertura do arquivo de Vetor\n");
      return 2;
   } 

   ret = fread(&linhas, sizeof(int), 1, arq_matrix);
   if (!ret) {fprintf(stderr, "Erro de leitura do num de linhas da matriz no arquivo \n"); return 3;}

   ret = fread(&colunas,sizeof(int), 1, arq_matrix);
   if (!ret) {fprintf(stderr, "Erro de leitura do num de colunas da matriz no arquivo \n"); return 3;}

   tam = linhas * colunas;
   

   // --    Leitura da Matriz   --
   //alocacao de memoria para as estruturas de dados da matriz
   mat = (float *) malloc(sizeof(float) * tam);
   if (mat == NULL) {printf("ERRO--malloc da mat\n"); return 2;}

   ret = fread(mat, sizeof(float), tam, arq_matrix);
   if (!ret) {fprintf(stderr, "Erro de leitura da matriz no arquivo \n"); return 3;}
   // --    Fim Leitura da Matriz   --

   ret = fread(&tam_vet, sizeof(long int), 1, arq_vet_in);
   if (!ret) {fprintf(stderr, "Erro de leitura do tamanho do vetor no arquivo \n"); return 3;}

   // --    Leitura do Vetor   --
   //alocacao de memoria para as estruturas de dados da matriz
   vet = (float *) malloc(sizeof(float) * tam_vet);
   if (vet == NULL) {printf("ERRO--malloc do vet\n"); return 2;}

   ret = fread(vet, sizeof(float), tam_vet, arq_vet_in);
   if (!ret) {fprintf(stderr, "Erro de leitura do vetor no arquivo \n"); return 3;}
   // --    Fim Leitura do Vetor   --

   // verifica se é possível fazer a multiplicação
   if (colunas != tam_vet){ // caso verdadeiro não é possível multiplicar a matriz pelo vetor
        printf("Num de colunas da matriz não bate com tamanho do vetor\n");
        return 1;
   }

   // aloca vetor que guardará o resultado
   saida = (float *) malloc(sizeof(float) * tam_vet);
   if (saida == NULL) {printf("ERRO--malloc\n"); return 2;}
  

   GET_TIME(fim);
   delta = fim - inicio;
   printf("Tempo inicializacao:%lf\n", delta);
   // Escreve tempo de inicizalização no arquivo de tempos de execução
   #ifdef ESCREVE
   FILE *arq_tempos = fopen(argv[4], "a");
    if (arq_tempos == NULL) {
        perror("Não foi possível abrir o arquivo");
        return 1;
    }
   fprintf(arq_tempos, "(%f,", delta);
   #endif

   //multiplicacao da matriz pelo vetor
   GET_TIME(inicio);

   mult(mat, vet, linhas, colunas); 
 
   GET_TIME(fim)   
   delta = fim - inicio;
   printf("Tempo multiplicacao sequencial (linhas %d) (colunas %d): %lf\n", linhas, colunas, delta);
   // Escreve tempo de multiplicação no arquivo de tempos de execução
   #ifdef ESCREVE
   fprintf(arq_tempos, " %f,", delta);
   #endif

   //abre o arquivo para escrita binaria
   arq_vet_out = fopen(argv[3], "wb");
   if(!arq_vet_out) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 3;
   }

   //escreve tamanho do vetor
   ret = fwrite(&tam_vet, sizeof(long int), 1, arq_vet_out);
   //escreve os elementos do vetor
   ret = fwrite(saida, sizeof(float), tam_vet, arq_vet_out);
   if(ret < tam_vet) {
      fprintf(stderr, "Erro de escrita no arquivo do vetor de saída\n");
      return 4;
   }

   // Escrita de arquivo e liberacao da memoria
   GET_TIME(inicio);
   free(mat);
   free(vet);
   free(saida);
   GET_TIME(fim)   
   delta = fim - inicio;
   printf("Tempo finalizacao:%lf\n", delta);
   // Escreve tempo de finalização no arquivo de tempos de execução
   #ifdef ESCREVE
   fprintf(arq_tempos, " %f), ", delta);
   #endif

   return 0;
}
