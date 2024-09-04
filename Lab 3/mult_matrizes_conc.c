/* Multiplicacao de matriz-vetor (considerando matrizes quadradas) */
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"
#define ESCREVE

float *mat1; //matriz 1 de entrada
float *mat2; //matriz 2 de entrada
float *mat_saida; //matriz de saída
int nthreads; //numero de threads

typedef struct{
   int id; //identificador do elemento que a thread ira processar
   int linhas; // num de linhas da matriz de saída
   int colunas; // num de colunas da matriz de saída
   int dim; // qtde de linhas de colunas da amtriz e linhas da matriz 2 que tem que ser iguais se a multiplicação for possível
} tArgs;

//funcao que as threads executarao
void * tarefa(void *arg) {
   tArgs *args = (tArgs*) arg;
   for(int i=args->id; i<args->linhas; i+=nthreads){
      for(int j=0; j<args->colunas; j++) {
         mat_saida[i*(args->colunas) + j] = 0;
         for(int k=0; k < args->dim; k++){
            mat_saida[i*(args->colunas) + j] += mat1[i*(args->dim) + k] * mat2[k*(args->colunas) + j];
         }
      }
   }
   pthread_exit(NULL);
}


int ler_matriz(const char *nome_arquivo, float **mat, int *linhas, int *colunas) {
    FILE *arq_matrix;
    size_t ret;
    int tam;

    // Abre o arquivo no modo binário
    arq_matrix = fopen(nome_arquivo, "rb");
    if (!arq_matrix) {
        fprintf(stderr, "Erro de abertura do arquivo da Matriz\n");
        return 2;
    }

    // Lê o número de linhas da matriz
    ret = fread(linhas, sizeof(int), 1, arq_matrix);
    if (!ret) {
        fprintf(stderr, "Erro de leitura do número de linhas da matriz no arquivo\n");
        fclose(arq_matrix);
        return 3;
    }

    // Lê o número de colunas da matriz
    ret = fread(colunas, sizeof(int), 1, arq_matrix);
    if (!ret) {
        fprintf(stderr, "Erro de leitura do número de colunas da matriz no arquivo\n");
        fclose(arq_matrix);
        return 3;
    }

    // Calcula o tamanho total da matriz
    tam = (*linhas) * (*colunas);

    // Aloca memória para a matriz
    *mat = (float *)malloc(sizeof(float) * tam);
    if (*mat == NULL) {
        fprintf(stderr, "Erro na alocação de memória para a matriz\n");
        fclose(arq_matrix);
        return 2;
    }

    // Lê a matriz do arquivo
    ret = fread(*mat, sizeof(float), tam, arq_matrix);
    if (!ret) {
        fprintf(stderr, "Erro de leitura da matriz no arquivo\n");
        free(*mat);
        fclose(arq_matrix);
        return 3;
    }

    // Fecha o arquivo
    fclose(arq_matrix);

    return 0; // Sucesso
}

//fluxo principal
int main(int argc, char* argv[]) {
   int linhas1, colunas1, linhas2, colunas2, tam; // linhas e colunas a serem obtidas ao ler matriz e cálculo do seu tamanho 
   pthread_t *tid; //identificadores das threads no sistema
   tArgs *args; //identificadores locais das threads e dimensao
   FILE *arq_matrix_out; //descritores dos arquivos de entrada e do arquivo de saída
   size_t ret; //retorno da funcao de leitura no arquivo de entrada
   double inicio, fim, delta;
   
   GET_TIME(inicio);
   //leitura e avaliacao dos parametros de entrada
   #ifdef ESCREVE
   if(argc<6) {
      printf("Digite: %s <numero de threads> <arquivo matriz 1 de entrada> <arquivo matriz 2 de entrada> <arquivo matriz de saída> <arquivo de escrita dos tempos de execução>\n", argv[0]);
      return 1;
   }
   #else
   if(argc<5) {
      printf("Digite: %s <numero de threads> <arquivo matriz 1 de entrada> <arquivo matriz 2 de entrada> <arquivo matriz de saída>\n", argv[0]);
      return 1;
   }
   #endif
   nthreads = atoi(argv[1]);

   ler_matriz(argv[2], &mat1, &linhas1, &colunas1); // ler primeira matriz
   ler_matriz(argv[3], &mat2, &linhas2, &colunas2); // ler segunda matriz 
   
   if (nthreads > linhas1) nthreads = linhas1; // se a quantidade de threads for maior que o número de linhas da matriz 1 então nthreadas é limitado ao número de linhas da primeira matriz
   tam = linhas1 * colunas2;

   // verifica se é possível fazer a multiplicação
   if (colunas1 != linhas2){ // caso verdadeiro não é possível multiplicar as matrizes
        return 1;
   }

   // aloca vetor que guardará o resultado
   mat_saida = (float *) malloc(sizeof(float) * tam);
   if (mat_saida == NULL) {printf("ERRO--malloc\n"); return 2;}
  

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
   fprintf(arq_tempos, "%d, %d, %f,", nthreads, colunas1,delta);
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
      (args+i)->linhas = linhas1;
      (args+i)->colunas = colunas2;
      (args+i)->dim = linhas2;
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
   printf("Tempo multiplicacao (linhas %d) (colunas %d) (nthreads %d): %lf\n", linhas1, colunas2, nthreads, delta);
   // Escreve tempo de multiplicação no arquivo de tempos de execução
   #ifdef ESCREVE
   fprintf(arq_tempos, " %f,", delta);
   #endif

   //abre o arquivo para escrita binaria
   arq_matrix_out = fopen(argv[4], "wb");
   if(!arq_matrix_out) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 3;
   }

   //escreve num de linhas e colunas da matriz de saída
   ret = fwrite(&linhas1, sizeof(int), 1, arq_matrix_out);
   if(!ret){fprintf(stderr, "Erro de escrita do num de linhas no arquivo da matriz de saída\n"); return 4;}
   ret = fwrite(&colunas2, sizeof(int), 1, arq_matrix_out);
   if(!ret){fprintf(stderr, "Erro de escrita do num de colunas no arquivo da matriz de saída\n"); return 4;}
   //escreve os elementos do vetor
   ret = fwrite(mat_saida, sizeof(float), tam, arq_matrix_out);
   if(ret < tam) {
      fprintf(stderr, "Erro de escrita dos valores no arquivo da amtriz de saída\n");
      return 4;
   }

   // Escrita de arquivo e liberacao da memoria
   GET_TIME(inicio);
   free(mat1);
   free(mat2);
   free(mat_saida);
   free(args);
   free(tid);
   GET_TIME(fim)   
   delta = fim - inicio;
   printf("Tempo finalizacao:%lf\n", delta);
   // Escreve tempo de finalização no arquivo de tempos de execução
   #ifdef ESCREVE
   fprintf(arq_tempos, " %f\n", delta);
   #endif

   return 0;
}
