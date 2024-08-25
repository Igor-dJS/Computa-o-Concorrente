# Descrição
O código C 'soma_vetores_conc.c' exibe a soma sequencial invertida, soma sequencial em dois blocos, soma concorrente com n threads definido na linha de comando, a soma original (feita de forma sequencial e salva no arquivo) e a variação relativa. O código é uma adaptação do código da atividade 2 com a finalidade de calcular o produto interno entre os dois vetores.

O código C 'soma_vetores_conc_alt.c' é adaptado de forma que, na linha de comando, ao executar o programa objeto seja necessário passar o nome do arquivo e um modo (um número entre 0 e 3) que dentre eles são:
- Modo 0
    - Calcula a soma sequencial invertida dos produtos e a soma em 2 blocos sequencial dos produtos entre os dois vetores e exibe o resultado.
- Modo 1
    - Calcula a soma sequencial invertida dos produtos entre os vetores e exibe o tempo de execução em segundos
- Modo 2
    - Calcula a soma em 2 blocos sequencial dos produtos entre os vetores e exibe o tempo de execução em segundos
- Modo 3
    - Calcula o produto interno utilizando threads, dividindo as tarefas em blocos onde cada thread irá fazer os produtos entre os valores dos vetores e as somas respectivas ao seu bloco. As n threads são especificadas pelo usuário ao selecionar esse modo e ser solicitado o número de threads via entrada padrão.

# Vetores teste
Foram gerados 3 arquvios teste com o código 'gera_vets_rand.c' sendo
- vets_1000.bin
- vets_100_000.bin
- vets_10_000_000.bin

Onde o número signfica o tamhno de cada vetor. Por exemplo, em 'vets_1000.bin' temos dois vetores de tamanho 1000, com o tamanho do vetor especificado no início com tipo long int e a soma sequencial especificada no fim do arquivo no tipo double.