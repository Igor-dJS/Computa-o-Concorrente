#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h> 

//variaveis globais
//tamanho do vetor
long int n;
//vetor de elementos
float *vet1, *vet2;
//numero de threads
int nthreads;

//funcao executada pelas threads
//estrategia de divisao de tarefas: blocos de n/nthreads elementos
void *SomaVetores (void *tid) {
    long int id = (long int) tid; //identificador da thread
    int ini, fim, bloco; //auxiliares para divisao do vetor em blocos
    float soma_local, *ret; //somatorio local

    bloco = n/nthreads; //tamanho do bloco de dados de cada thread
    ini = id*bloco; //posicao inicial do vetor
    fim = ini + bloco; //posicao final do vetor
    if (id==(nthreads-1)) fim = n; //a ultima thread trata os elementos restantes no caso de divisao nao exata  
    //soma os valores 
    for(int i=ini; i<fim; i++) {
       soma_local += (vet1[i]*vet2[i]);
    }   
    //retorna o resultado da soma
    ret = malloc(sizeof(float));
    if (ret!=NULL){ 
        *ret = soma_local;
    }else { 
        printf("--ERRO: malloc() thread\n");
    } 

    pthread_exit((void*) ret);
}

void soma_sequencial_invertida(float* soma){
  //soma sequencial
  for(int t=n-1; t>=0; t--) {
     *soma += (vet1[t] * vet2[t]);
  }
}

void soma_sequencial_blocos(float* soma){
  //soma sequencial bloco (== soma com 2 threads)
  float soma1, soma2;

  soma1=0;
  for(int t=0; t<n/2; t++) {
     soma1 += (vet1[t] * vet2[t]);
  }
  soma2=0;
  for(int t=n/2; t<n; t++) {
     soma2 += (vet1[t] * vet2[t]);
  }

  *soma = (soma1 + soma2);
}

//funcao principal do programa
int main(int argc, char *argv[]) {
  FILE *arq; //arquivo de entrada
  size_t ret; //retorno da funcao de leitura no arquivo de entrada
	      
  double var_relativa; // variável para guardar a varição relativa entre o cáclulo concorrente e sequencial
  double soma_ori; //soma registrada no arquivo (feita de forma sequencial)
  float soma_seq, soma_seq_blocos, soma_par_global; //resultados das somas
  float *soma_retorno_threads; //auxiliar para retorno das threads

  pthread_t *tid_sistema; //vetor de identificadores das threads no sistema

  //valida e recebe os valores de entrada
  if(argc < 3) { printf("Use: %s <arquivo de entrada> <numero de threads> \n", argv[0]); exit(-1); }

  //abre o arquivo de entrada com os valores para serem somados
  arq = fopen(argv[1], "rb");
  if(arq==NULL) { printf("--ERRO: fopen()\n"); exit(-1); }

  //le o tamanho do vetor (primeira linha do arquivo)
  ret = fread(&n, sizeof(long int), 1, arq);
  if(!ret) {
     fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
     return 3;
  }

  //aloca espaco de memoria e carrega os vetores de entrada
  vet1 = malloc (sizeof(float) * n);
  if(vet1==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }

  vet2 = malloc(sizeof(float) * n);
  if(vet2==NULL) {printf("--ERRO: malloc()\n"); exit(-1);}

  ret = fread(vet1, sizeof(float), n, arq);
  ret = fread(vet2, sizeof(float), n, arq);

  if(ret < n) {
     fprintf(stderr, "Erro de leitura dos elementos do vetor\n");
     return 4;
  }

  soma_sequencial_invertida(&soma_seq);
  soma_sequencial_blocos(&soma_seq_blocos);

  printf("soma_seq (invertida)         = %.26f\n\n", soma_seq);
  printf("soma_seq_blocos (2 blocos)   = %.26f\n\n", soma_seq_blocos);

  //le o numero de threads da entrada do usuario 
  nthreads = atoi(argv[2]);
  //limita o numero de threads ao tamanho do vetor
  if(nthreads>n) nthreads = n;
  printf("Qtde de Threads: %d\n", nthreads);
  printf("Tam de N: %ld\n", n);

  //aloca espaco para o vetor de identificadores das threads no sistema
  tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
  if(tid_sistema==NULL) { printf("--ERRO: malloc()\n"); exit(-1); }

  //cria as threads
  for(long int i=0; i<nthreads; i++) {
    if (pthread_create(&tid_sistema[i], NULL, SomaVetores, (void*) i)) {
       printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }
 
  //espera todas as threads terminarem e calcula a soma total das threads
  //retorno = (float*) malloc(sizeof(float));
  soma_par_global=0;
  for(int i=0; i<nthreads; i++) {
     if (pthread_join(tid_sistema[i], (void *) &soma_retorno_threads)) {
        printf("--ERRO: pthread_join()\n"); exit(-1);
     }
     soma_par_global += *soma_retorno_threads;
     free(soma_retorno_threads);
  }

  //imprime os resultados
  printf("\n");
  printf("soma_concorrente             = %.26f\n", soma_par_global);
 
  //le o somatorio registrado no arquivo
  ret = fread(&soma_ori, sizeof(double), 1, arq); 
  printf("\nSoma-original              = %.26lf\n", soma_ori);

  // Imprime varição relativa
  var_relativa = (soma_ori - soma_par_global) / soma_ori;
  if (var_relativa < 0) {var_relativa *= -1;}
  printf("\nVariacao relativa            = %.26lf\n", var_relativa);

  //desaloca os espacos de memoria
  free(vet1);
  free(vet2);
  free(tid_sistema);
  //fecha o arquivo
  fclose(arq);
  return 0;
}
