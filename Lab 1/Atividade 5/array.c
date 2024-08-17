#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Estrutura para passar argumentos para as threads
typedef struct {
    int *array;
    int start;
    int end;
} ThreadArgs;

// Função executada pelas threads
void *increment_elements(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    int *array = args->array;
    int start = args->start;
    int end = args->end;

    for (int i = start; i < end; i++) {
        array[i]++;
    }
    pthread_exit(NULL);
}

// Função para inicializar o vetor
void initialize_array(int *array, int size) {
    for (int i = 0; i < size; i++) {
        array[i] = i; // Inicializa com valores 0, 1, 2, ..., size-1
    }
}

// Função para verificar se o vetor foi corretamente atualizado
void verify_array(int *array, int size) {
    for (int i = 0; i < size; i++) {
        if (array[i] != i + 1) {
            printf("Erro: array[%d] = %d, esperado = %d\n", i, array[i], i + 1);
            exit(1);
        }
    }
    printf("Vetor verificado com sucesso.\n");
}

int main(int argc, char *argv[]) {
    //verifica se os argumentos com a quantidade de threads e o tamanho do vetor foi passado
    if(argc < 3) {
        printf("--ERRO: informe a qtde de threads <%s> <nthreads> e o tamanho do vetor\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);
    int M = atoi(argv[2]);

    if (N <= 0 || M <= 0) {
        printf("N e M devem ser maiores que 0.\n");
        return 1;
    }

    if (M > N) {
        printf("Quantidade de Threads maior que o tamanho do vetor.\nDefinindo a quantidade de Threads para o tamanho do vetor!.\n");
        M = N;
    }

    int *array = malloc(N * sizeof(int));
    if (array == NULL) {
        fprintf(stderr, "Erro ao alocar memória.\n");
        return 1;
    }

    initialize_array(array, N); //Inicializando o vetor com os números correspondentes

    pthread_t threads[M];
    ThreadArgs args[M];
    int elementos_por_thread = N / M;
    int resto = N % M;

    int thread_inicial_resto = M - resto;

    // Cria as threads e distribui de forma balanceada
    for (int i = 0; i < M; i++) {
        args[i].array = array;

        if (i == thread_inicial_resto){
            args[i].start = i * elementos_por_thread;
            args[i].end = (i + 1) * elementos_por_thread + 1;
        } else if (i > thread_inicial_resto){
            args[i].start = i * elementos_por_thread + (i - thread_inicial_resto); 
            args[i].end = (i + 1) * elementos_por_thread + ((i + 1) - thread_inicial_resto);            
        }else{
            args[i].start = i * elementos_por_thread;
            args[i].end = (i + 1) * elementos_por_thread ;
        }

#ifdef TAMANHOS
        printf("Thread %d\n%d - %d\n", i, args[i].start, args[i].end);
#endif

        if (pthread_create(&threads[i], NULL, increment_elements, (void *) &args[i])) {
            fprintf(stderr, "Erro ao criar a thread %d.\n", i);
            free(array);
            return 1;
        }
    }

    // Aguarda o término das threads
    for (int i = 0; i < M; i++) {
        if (pthread_join(threads[i], NULL)) {
            fprintf(stderr, "Erro ao esperar a thread %d.\n", i);
            free(array);
            return 1;
        }
    }

    // Verifica o resultado
    verify_array(array, N);

    free(array);
    return 0;
}
