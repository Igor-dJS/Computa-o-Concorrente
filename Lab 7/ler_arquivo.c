#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 3
#define N 15

// vars globais
sem_t vazios1, cheios1, vazios2, cheios2;
char buffer1[N], buffer2[N];
int in1 = 0, out1 = 0, in2 = 0, out2 = 0;

void insere_buffer1(char c){
    // aguarda slot vazio
    sem_wait(&vazios1);

    buffer1[in1] = c; // salva caractere no primeiro buffer
    in1 = (in1 + 1) % N; // atualiza próxima posição de inserção


    // sinaliza que um slot ficou cheio para o consumidor
    sem_post(&cheios1);
}

char retira_buffer1(){
    char c;

    // aguarda slot cheio
    sem_wait(&cheios1);

    c = buffer1[out1]; // retira caractere do primeiro buffer
    out1 = (out1 + 1) % N; // atualiza próxima posição de remoção

    // sinaliza que um slot ficou vazio para o produtor
    sem_post(&vazios1);

    return c;
}

void insere_buffer2(char c){
    // aguarda slot vazio
    sem_wait(&vazios2);

    buffer2[in2] = c; // salva caractere no segundo buffer
    in2 = (in2 + 1) % N; // atualiza próxima posição de inserção

    // sinaliza que um slot ficou cheio para o consumidor
    sem_post(&cheios2);
}

char retira_buffer2(){
    char c;

    // aguarda slot cheio
    sem_wait(&cheios2);

    c = buffer2[out2]; // retira caractere do segundo buffer
    out2 = (out2 + 1) % N; // atualiza próxima posição de remoção

    // sinaliza que um slot ficou vazio para o produtor
    sem_post(&vazios2);

    return c;
}

// funções das threads
void *task1(void *args){
    FILE *arq; // var para arquivo
    int c; // var pata receber caractere como int 

    arq = fopen("baleia.txt", "r");
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo\n");
        pthread_exit(NULL);
    }

    // enquanto arquivo não acaba, continua lendo
    while((c = fgetc(arq)) != EOF){
        c = (char)c; // casting para char pois é necessário que c seja int pois fgetc retorna um inteiro
        insere_buffer1(c);
    }

    insere_buffer1('\0'); // para marcar o fim da leitura do buffer1, dessa forma a thread 2 saberá quando será a hora de terminar de ler os possíveis caracteres restantes

    fclose(arq); // fecha arquivo
    pthread_exit(NULL);
}

void *task2(void *args){
    int n = 0, count_caracteres = 0; // vars de controle para as quebras de linha
    char c;

    while (1) 
    {        
        if((count_caracteres == (2*n+1))){ // pular linha a cada 2*n+1 caracteres com n indo de 1 a 10 com n sendo incrementado de 1 a cada quebra de linha
            insere_buffer2('\n');
            count_caracteres = 0;
            n++;
        }else if((count_caracteres == 10) && (n > 10)){ // pular linha a cada 10 caracteres depois de n ser maior que 10
            insere_buffer2('\n');
            count_caracteres = 0;
        }else{ // caso não seja quebra de linha apenas retira do buffer1 e insere no buffer2 verificando se o buffer1 já terminou os caracteres ou não
            c = retira_buffer1();
            if(c == '\0'){
                break;
            }
            insere_buffer2(c);
            count_caracteres++;
        }
    }

    insere_buffer2('\0'); // para marcar o fim da leitura do buffer2, dessa forma a thread 3 saberá quando será a hora de terminar de ler os possíveis caracteres restantes

    pthread_exit(NULL);
}


void *task3(void *args){
    char c;

    // ler os caracteres do buffer e imprime na tela
    while (1)
    {
        c = retira_buffer2();
        if(c == '\0'){
            break;
        }
        printf("%c", c);
    }

    pthread_exit(NULL);
}
// fim funções das threads

int main(int argc, char *argv[])
{
    pthread_t thread_id[NTHREADS];

    // inicia os semáforos
    sem_init(&cheios1, 0, 0);
    sem_init(&vazios1, 0, N);

    sem_init(&cheios2, 0, 0);
    sem_init(&vazios2, 0, N);

    // cria as três threads
    if (pthread_create(&thread_id[0], NULL, task1, NULL)){
        printf("--ERRO: pthread_create()\n"); exit(-1);}
    printf("Thread 1 criada\n");

    if (pthread_create(&thread_id[1], NULL, task2, NULL)){
        printf("--ERRO: pthread_create()\n"); exit(-1);}
    printf("Thread 2 criada\n");
    

    if (pthread_create(&thread_id[2], NULL, task3, NULL)){
        printf("--ERRO: pthread_create()\n"); exit(-1);}
    printf("Thread 3 criada\n");


    //esperar as threads terminarem
    for(int i = 0 ; i < NTHREADS ; i++){
        if(pthread_join(thread_id[i], NULL)){printf("--ERRO: pthread_join() \n"); exit(-1);}
    }

    // finaliza os semáforos
    sem_destroy(&cheios1);
    sem_destroy(&vazios1);
    sem_destroy(&cheios2);
    sem_destroy(&vazios2);


    return 0;
}