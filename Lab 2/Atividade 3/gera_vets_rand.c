/* 
Programa auxiliar para gerar dois vetores salvando em arquivo binário o tamanho dos vetores,
seguido dos dois vetores e por último o resultado do produto interno entre eles, com multiplicação
e soma feita de forma sequencial do início para o fim
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 1000 //valor maximo de um elemento do vetor

float* aloca_vetor(int n){
    float* vet;

    vet = (float*) malloc(sizeof(float) * n);

    return vet;
}


int main(int argc, char*argv[]) {
   float *vetor1, *vetor2; //vetores que serão gerados
   long int n; //qtde de elementos dos vetores que serao gerados
   float elem1, elem2; //valores gerados para incluir no vetor
   double soma=0; //soma total do produto entre os valores de cada ínidce dos vetores (produto interno)
   int fator=1; //fator multiplicador para gerar números negativos

   FILE * descritorArquivo; //descritor do arquivo de saida
   size_t ret = 0; //retorno da funcao de escrita no arquivo de saida

   //recebe os argumentos de entrada
   if(argc < 3) {
      fprintf(stderr, "Digite: %s <dimensao> <arquivo saida>\n", argv[0]);
      return 1;
   }
   n = atoi(argv[1]);

   //aloca memoria para os vetores
   vetor1 = aloca_vetor(n);
   if(!vetor1) {
    fprintf(stderr, "Erro de alocao da memoria do vetor1\n");
    return 2;
   }

    vetor2 = aloca_vetor(n);
    if(!vetor2) {
        fprintf(stderr, "Erro de alocao da memoria do vetor2\n");
        return 2;
   }

   //preenche o vetor com valores float aleatorios
   srand(time(NULL));
   for(long int i=0; i<n; i++) {
        elem1 = (rand() % MAX)/3.0 * fator;
        elem2 = (rand() % MAX)/3.0 * fator;

        vetor1[i] = elem1;
        vetor2[i] = elem2;

        soma += (elem1 * elem2); //acumula o elemento na soma total
        fator*=-1;
   }

   //imprimir na saida padrao o vetor gerado
   #ifdef TEXTO
   fprintf(stdout, "%ld\n", n);
   for(long int i=0; i<n; i++) {
      fprintf(stdout, "%f ",vetor1[i]);
   }

   fprintf(stdout, "\n");

   for(long int i=0; i<n; i++) {
      fprintf(stdout, "%f ",vetor2[i]);
   }
   fprintf(stdout, "\n");
   fprintf(stdout, "soma=%lf\n", soma);
   #endif


   //escreve os vetores no arquivo
   //abre o arquivo para escrita binaria
   descritorArquivo = fopen(argv[2], "wb");
   if(!descritorArquivo) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 3;
   }

   //escreve a dimensao
   ret = fwrite(&n, sizeof(long int), 1, descritorArquivo);
   //escreve os elementos do vetor1 e do vetor2 respectivamente
   ret = fwrite(vetor1, sizeof(float), n, descritorArquivo);
   ret = fwrite(vetor2, sizeof(float), n, descritorArquivo);

   //escreve o somatorio
   ret = fwrite(&soma, sizeof(double), 1, descritorArquivo);
   if(ret < (1+2*n+1)) {
      fprintf(stderr, "Erro de escrita no  arquivo\n");
      return 4;
   }

   //finaliza o uso das variaveis
   fclose(descritorArquivo);
   free(vetor1);
   free(vetor2);

   return 0;
} 