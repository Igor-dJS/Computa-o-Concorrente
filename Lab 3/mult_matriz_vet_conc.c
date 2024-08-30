/* Multiplicacao de matriz-vetor (considerando matrizes quadradas) */
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"
#define ESCREVE

float *mat; //matriz de entrada
float *vet; //vetor de entrada
float *saida; //vetor de saida
int nthreads; //numero de threads

typedef struct{
   int id; //identificador do elemento que a thread ira processar
   int linhas; // num de linhas da matriz de entrada
   int colunas; // num de colunas da matriz de entrada
} tArgs;

//funcao que as threads executarao
void * tarefa(void *arg) {
   tArgs *args = (tArgs*) arg;
   for(int i=args->id; i<args->linhas; i+=nthreads)
      for(int j=0; j<args->colunas; j++) 
         saida[i] += mat[i*(args->colunas) + j] * vet[j];
   pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char* argv[]) {
   long int tam_vet; //tamanho do vetor lido
   int linhas, colunas, tam; // linhas e colunas a serem obtidas ao ler matriz e cálculo do seu tamanho 
   pthread_t *tid; //identificadores das threads no sistema
   tArgs *args; //identificadores locais das threads e dimensao
   FILE *arq_matrix, *arq_vet_in, *arq_vet_out; //descritores dos arquivos de entrada e do arquivo de saída
   size_t ret; //retorno da funcao de leitura no arquivo de entrada
   double inicio, fim, delta;
   
   GET_TIME(inicio);
   //leitura e avaliacao dos parametros de entrada
   #ifdef ESCREVE
   if(argc<6) {
      printf("Digite: %s <numero de threads> <arquivo matriz de entrada> <arquivo vetor de entrada> <arquivo vetor de saída> <arquivo de escrita dos tempos de execução>\n", argv[0]);
      return 1;
   }
   #else
   if(argc<5) {
      printf("Digite: %s <numero de threads> <arquivo matriz de entrada> <arquivo vetor de entrada> <arquivo vetor de saída>\n", argv[0]);
      return 1;
   }
   #endif
   nthreads = atoi(argv[1]);

   arq_matrix = fopen(argv[2], "rb");
   if(!arq_matrix) {
      fprintf(stderr, "Erro de abertura do arquivo da Matriz\n");
      return 2;
   } 

   arq_vet_in = fopen(argv[3], "rb");
   if(!arq_vet_in) {
      fprintf(stderr, "Erro de abertura do arquivo de Vetor\n");
      return 2;
   } 

   ret = fread(&linhas, sizeof(int), 1, arq_matrix);
   if (!ret) {fprintf(stderr, "Erro de leitura do num de linhas da matriz no arquivo \n"); return 3;}

   ret = fread(&colunas,sizeof(int), 1, arq_matrix);
   if (!ret) {fprintf(stderr, "Erro de leitura do num de colunas da matriz no arquivo \n"); return 3;}

   tam = linhas * colunas;
   if (nthreads > linhas) nthreads=tam; // se a quantidade de threads for maior que o número de linhas da matriz então nthreadas é limitado ao número de linhas
   

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
   FILE *arq_tempos = fopen(argv[5], "a");
    if (arq_tempos == NULL) {
        perror("Não foi possível abrir o arquivo");
        return 1;
    }
   fprintf(arq_tempos, "(%f,", delta);
   #endif

   //multiplicacao da matriz pelo vetor
   GET_TIME(inicio);
   //alocacao das estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}

   args = (tArgs*) malloc(sizeof(tArgs)*nthreads);
   if(args==NULL) {puts("ERRO--malloc"); return 2;}

   //criacao das threads
   for(int i=0; i<nthreads; i++) {
      (args+i)->id = i;
      (args+i)->linhas = linhas;
      (args+i)->colunas = colunas;
      if(pthread_create(tid+i, NULL, tarefa, (void*) (args+i))){
         puts("ERRO--pthread_create"); return 3;
      }
   } 
   //espera pelo termino da threads
   for(int i=0; i<nthreads; i++) {
      pthread_join(*(tid+i), NULL);
   }
   GET_TIME(fim)   
   delta = fim - inicio;
   printf("Tempo multiplicacao (linhas %d) (colunas %d) (nthreads %d): %lf\n", linhas, colunas, nthreads, delta);
   // Escreve tempo de multiplicação no arquivo de tempos de execução
   #ifdef ESCREVE
   fprintf(arq_tempos, " %f,", delta);
   #endif

   //abre o arquivo para escrita binaria
   arq_vet_out = fopen(argv[4], "wb");
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
   free(args);
   free(tid);
   GET_TIME(fim)   
   delta = fim - inicio;
   printf("Tempo finalizacao:%lf\n", delta);
   // Escreve tempo de finalização no arquivo de tempos de execução
   #ifdef ESCREVE
   fprintf(arq_tempos, " %f), ", delta);
   #endif

   return 0;
}
