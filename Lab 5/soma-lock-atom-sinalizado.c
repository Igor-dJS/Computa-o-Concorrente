/*
A Thread extra imprime os 20 primeiros múltiplos de 10. 
Esse código é válido apenas para execução de uma thread principal com uma thread extra.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

long int soma = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_cond_t cond_extra;
int qtde_prints = 0;
int control = 0;

// Função executada pelas threads principais
void *ExecutaTarefa(void *arg){
    long int id = (long int) arg;
    printf("Thread : %ld esta executando...\n", id);
    control = 1;

    for(int i = 0; i< 100000; i++){
        //--entrada na SC
        pthread_mutex_lock(&mutex);
        //--SC (seção critica)
        if (!(soma % 10) && (qtde_prints < 20)){ //--controle da impressão dos 20 primeiros múltiplos de 10
            pthread_cond_signal(&cond_extra); //--libera thread de impressão (thread extra) se ela estiver bloqueada
            pthread_cond_wait(&cond, &mutex); //--aguarda sinal da thread extra para voltar a somar na var 'soma'
            qtde_prints++; //--aumenta contador de quantidade de saídas já realizadas pela thread extra
        }
        soma++; //incrementa a variavel compartilhada 
        //--saida da SC
        pthread_mutex_unlock(&mutex);
    }

    pthread_cond_signal(&cond_extra); //--libera thread de impressão (thread extra) se ela estiver bloqueada
    printf("Thread : %ld terminou!\n", id);
    pthread_exit(NULL);
}

//funcao executada pela thread de log
void *extra (void *args) {
    printf("Extra : esta executando...\n");

    //--esse trecho incial garante que se a thread extra começar primeiro não será impresso o valor 'soma = 0' duas vezes
    pthread_mutex_lock(&mutex); //--início SC crítica
    if(!control){ //--se a thread extra começar antes ela aguarda o início da outra thread verificando a variável 'control'. Se ela for 0 é porque a outra thread não começou (ela altera o valor para 1).
      pthread_cond_wait(&cond_extra, &mutex); //--aguarda sinal da thread principal para continuar iteração. Isso evita que a threa extra fique executando desnecessariamente.
    }
    pthread_mutex_unlock(&mutex); // fim SC crítica


    while(qtde_prints < 20){
        pthread_mutex_lock(&mutex); //--início SC crítica
        printf("soma = %ld \n", soma); //--print da var 'soma' 
        pthread_cond_signal(&cond); //--libera a thread principal para continuar suas iterações após realiza a saída no terminal
        pthread_cond_wait(&cond_extra, &mutex); //--aguarda sinal da thread principal para continuar iteração. Isso evita que a threa extra fique executando desnecessariamente.
        pthread_mutex_unlock(&mutex); // fim SC crítica
    }

    //pthread_cond_signal(&cond); //--libera a thread principal para continuar suas iterações após realiza a saída no terminal
    printf("Extra : terminou!\n");
    pthread_exit(NULL);  
}

//fluxo principal
int main(int argc, char *argv[]) {
   pthread_t *tid; //identificadores das threads no sistema
   int nthreads; //qtde de threads (passada linha de comando)

   //--le e avalia os parametros de entrada
   if(argc<2) {
      printf("Digite: %s <numero de threads>\n", argv[0]);
      return 1;
   }
   nthreads = atoi(argv[1]);

   //--aloca as estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*(nthreads+1));
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}

   //--inicilaiza o mutex (lock de exclusao mutua)
   pthread_mutex_init(&mutex, NULL);

   //--inicializa vars de condição
   pthread_cond_init (&cond, NULL);
   pthread_cond_init (&cond_extra, NULL);

   //--cria as threads
   for(long int t=0; t<nthreads; t++) {
     if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
       printf("--ERRO: pthread_create()\n"); exit(-1);
     }
   }

   //--cria thread de log
   if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
   }

   //--espera todas as threads terminarem
   for (int t=0; t<nthreads+1; t++) {
     if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
     } 
   } 

   //--finaliza o mutex
   pthread_mutex_destroy(&mutex);

   //--finaliza conds
   pthread_cond_destroy(&cond);
   pthread_cond_destroy(&cond_extra);

   
   printf("Valor de 'soma' = %ld\n", soma);

   return 0;
}
