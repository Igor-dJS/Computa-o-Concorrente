#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "list_int.h"
#include "timer.h"

#define QTDE_OPS 500// quantidade de operacoes sobre a lista (insercao, remocao, consulta)
#define QTDE_INI 100      // quantidade de insercoes iniciais na lista
#define MAX_VALUE 100     // valor maximo a ser inserido

// lista compartilhada iniciada
struct list_node_s *head_p = NULL;
// qtde de threads no programa
int nthreads, leit, escr;
int solicitacao;

// rwlock de exclusao mutua
pthread_mutex_t mutex;
pthread_cond_t cond_leit, cond_escr;

void EntraLeitura(long int id)
{
    pthread_mutex_lock(&mutex);
    // se já tiver thread escrevendo ou thread solicitado escrita aguarda
    while ((escr > 0) || solicitacao)
    {
        if (escr > 0){
            printf("Thread %ld esperando escrita terminar.\n", id);
        } else {
            printf("Thread %ld leitora esperando escrita solicitada.\n", id);
        }
        pthread_cond_wait(&cond_leit, &mutex);
    }
    leit++;
    pthread_mutex_unlock(&mutex);
}

void SaiLeitura(long int id)
{
    pthread_mutex_lock(&mutex);
    leit--;
    // se threads leitoras terminaram de ler libera thread escritora
    if (leit == 0)
    {
        pthread_cond_signal(&cond_escr);
    }
    // caso tenha uma solicitação de escita ativa imprime log
    if (solicitacao != 0){
        printf("Thread %ld terminou a leitura. Leitores = %d.\n", id, leit);
    }
    pthread_mutex_unlock(&mutex);
}

void EntraEscrita(long int id)
{
    pthread_mutex_lock(&mutex);
    solicitacao = 1; // solicita escrita
    printf("Thread %ld solicitou escrita.\n", id); // log de solicitação
    // se há leitores lendo ou escritor escrevendo aguarda
    while ((leit > 0) || (escr > 0))
    {
        if (leit > 0){
            printf("Thread %ld aguardando leitores terminarem. Leitores = %d\n", id, leit);
        } else {
            printf("Thread %ld aguardando outro escritor terminar.\n", id);
        }
        pthread_cond_wait(&cond_escr, &mutex);
    }
    escr++;
    pthread_mutex_unlock(&mutex);
}

void SaiEscrita(long int id)
{
    pthread_mutex_lock(&mutex);
    escr--;
    solicitacao = 0; // solicitação já atendida, então desativa
    printf("Thread %ld terminou escrita.\n", id);
    pthread_cond_signal(&cond_escr);
    pthread_cond_broadcast(&cond_leit);
    pthread_mutex_unlock(&mutex);
}
// TO DO

void *tarefa(void *args)
{
    long int id = (long int)args;
    int op;
    int in, out, read;
    in = out = read = 0;

    // TO DO
    // realiza operacoes de consulta (98%), insercao (1%) e remocao (1%)
    for (long int i = id; i < QTDE_OPS; i += nthreads)
    {
        op = rand() % 100;
        if (op < 98)
        {
            EntraLeitura(id);
            Member(i % MAX_VALUE, head_p); /* Ignore return value */
            SaiLeitura(id);
            read++;
        }
        else if (98 <= op && op < 99)
        {
            EntraEscrita(id);
            Insert(i % MAX_VALUE, &head_p); /* Ignore return value */
            SaiEscrita(id);
            in++;
        }
        else if (99 <= op)
        {
            EntraEscrita(id);
            Delete(i % MAX_VALUE, &head_p); /* Ignore return value */
            SaiEscrita(id);
            out++;
        }
    }

    // registra a qtde de operacoes realizadas por tipo
    printf("Thread %ld: in=%d out=%d read=%d\n", id, in, out, read);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t *tid;
    double ini, fim, delta;

    // verifica se o numero de threads foi passado na linha de comando
    if (argc < 2)
    {
        printf("Digite: %s <numero de threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[1]);

    // insere os primeiros elementos na lista
    for (int i = 0; i < QTDE_INI; i++)
        Insert(i % MAX_VALUE, &head_p); /* Ignore return value */

    // aloca espaco de memoria para o vetor de identificadores de threads no sistema
    tid = malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL)
    {
        printf("--ERRO: malloc()\n");
        return 2;
    }

    // tomada de tempo inicial
    GET_TIME(ini);
    // inicializa a variavel mutex
    pthread_mutex_init(&mutex, NULL);

    // cria as threads
    for (long int i = 0; i < nthreads; i++)
    {
        if (pthread_create(tid + i, NULL, tarefa, (void *)i))
        {
            printf("--ERRO: pthread_create()\n");
            return 3;
        }
    }

    // aguarda as threads terminarem
    for (int i = 0; i < nthreads; i++)
    {
        if (pthread_join(*(tid + i), NULL))
        {
            printf("--ERRO: pthread_join()\n");
            return 4;
        }
    }

    // tomada de tempo final
    GET_TIME(fim);
    delta = fim - ini;
    printf("Tempo: %lf\n", delta);

    // libera o mutex
    pthread_mutex_destroy(&mutex);
    // libera o espaco de memoria do vetor de threads
    free(tid);
    // libera o espaco de memoria da lista
    Free_list(&head_p);

    return 0;
} /* main */