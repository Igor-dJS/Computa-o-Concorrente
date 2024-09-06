/* Multiplicação de matrizes*/
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"
#define ESCREVE

float *mat1; //matriz 1 de entrada 
float *mat2; //matriz 2 de entrada
float *mat_saida; //matriz de saída

//funcao que multiplica as matrizes e salva na amtriz de saída
void mult(float* mat1, float* mat2, int linhas, int colunas, int dim) {
   for(int i=0; i<linhas; i++){
      for(int j=0; j<colunas; j++){
         mat_saida[i*colunas + j] = 0;
         for(int k=0; k<dim; k++){
            mat_saida[i*colunas + j] += mat1[i*dim + k] * mat2[k*colunas + j]; 
         }
      }
   }
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


// Fluxo principal
int main(int argc, char* argv[]) {
   int linhas1, colunas1, linhas2, colunas2, tam; // linhas e colunas a serem obtidas ao ler matriz e cálculo do seu tamanho 
   FILE *arq_matrix_out; //descritores dos arquivos de entrada e do arquivo de saída
   size_t ret;
   double inicio, fim, delta;
   
   GET_TIME(inicio);
   //leitura e avaliacao dos parametros de entrada
   #ifdef ESCREVE
   if(argc<5) {
      printf("Digite: %s <arquivo matriz 1 de entrada> <arquivo matriz 2 de entrada> <arquivo matriz de saída> <arquivo de escrita dos tempos de execução>\n", argv[0]);
      return 1;
   }
   #else
   if(argc<4) {
      printf("Digite: %s <arquivo matriz 1 de entrada> <arquivo matriz 2 de entrada> <arquivo matriz de saída>\n", argv[0]);
      return 1;
   }
   #endif

   ler_matriz(argv[1], &mat1, &linhas1, &colunas1); // ler primeira matriz
   ler_matriz(argv[2], &mat2, &linhas2, &colunas2); // ler segunda matriz 


   // verifica se é possível fazer a multiplicação
   if (colunas1 != linhas2){ // caso verdadeiro não é possível multiplicar a matriz pelo vetor
        printf("Num de colunas da matriz 1 não bate com num de linhas da matriz 2\n");
        return 1;
   }
   tam = linhas1*colunas2;

   // aloca vetor que guardará o resultado
   mat_saida = (float *) malloc(sizeof(float) * tam);
   if (mat_saida == NULL) {printf("ERRO--malloc\n"); return 2;}
  

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
   fprintf(arq_tempos, "0, %d, %f,", colunas1,delta);
   #endif

   //multiplicacao da matriz pelo vetor
   GET_TIME(inicio);

   mult(mat1, mat2, linhas1, colunas2, linhas2); // o argumento relacionado ao parâmetro dim poderia ser colunas1 (referente a matriz1) pois nesse ponto do código colunas1 e linhas2 tem o mesmo valor já que foi verificado que é possível multiplicar as matrizes 
 
   GET_TIME(fim)   
   delta = fim - inicio;
   printf("Tempo multiplicacao sequencial (linhas %d) (colunas %d): %lf\n", linhas1, colunas2, delta);
   // Escreve tempo de multiplicação no arquivo de tempos de execução
   #ifdef ESCREVE
   fprintf(arq_tempos, " %f,", delta);
   #endif

   //abre o arquivo para escrita binaria
   arq_matrix_out = fopen(argv[3], "wb");
   if(!arq_matrix_out) {
      fprintf(stderr, "Erro de abertura do arquivo da matriz de saída\n");
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

   // Escrita de arquivo e liberação da memória
   GET_TIME(inicio);
   free(mat1);
   free(mat2);
   free(mat_saida);
   GET_TIME(fim)   
   delta = fim - inicio;
   printf("Tempo finalizacao:%lf\n", delta);
   // Escreve tempo de finalização no arquivo de tempos de execução
   #ifdef ESCREVE
   fprintf(arq_tempos, " %f\n", delta);
   #endif

   return 0;
}
