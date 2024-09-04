# Compilação

## Pasta matrizes
`gcc -o geraMatrizBinario.out geraMatrizBinario.c -Wall`

## Pasta Lab3 (raiz)

`gcc -o mult_matrizes_seq.out mult_matrizes_seq.c -Wall`

`gcc -o mult_matrizes_conc.out mult_matrizes_conc.c -Wall`

# Modo de Execução
Nos arquivos .c de multiplicação de matrizes caso ESCREVE esteja definido nos headers será pedido um nome de arquivo a fim de salvar o número de threads (zero para sequencial), a dimensão das matrizes (número de colunas da primeira e número de linhas da segunda caso sejam igauis) e os tempos de execução em formato texto. Os arquivos são abertos em modo de adição, logo é possível salvar num mesmo arquivo vários tempos de execução se esse mesmo arquivo for passado em diferentes execuções.

O gerador de matriz aleatória foi obtido do programa de exemplo da professora

Gerador de Matriz aleatória -> `./geraMatrizBinario.out <linhas> <colunas> <arquivo saida>`

Sem ESCREVE definido

Multiplicação sequencial de matrizes -> `./mult_matrizes_seq.out <arquivo matriz 1 de entrada> <arquivo matriz 2 de entrada> <arquivo matriz de saída> <arquivo matriz de saída>`

Multiplicação concorrente de matrizes -> `./mult_matrizes_conc.out <numero de threads> <arquivo matriz 1 de entrada> <arquivo matriz 2 de entrada> <arquivo matriz de saída>`

Com ESCREVE definido

Multiplicação sequencial de matrizes -> `./mult_matrizes_seq.out <arquivo matriz 1 de entrada> <arquivo matriz 2 de entrada> <arquivo matriz de saída> <arquivo de escrita dos tempos de execução>`

Multiplicação concorrente de matrizes -> `./mult_matrizes_conc.out <numero de threads> <numero de threads> <arquivo matriz 1 de entrada> <arquivo matriz 2 de entrada> <arquivo matriz de saída> <arquivo de escrita dos tempos de execução>`

# Como estão salvos os arquivos binários?
Matrizes -> linhas(int), colunas(int), matriz(float*) nessa sequência

Obs: (float*) signfica que há uma sequência continua de floats, onde que para matrizes há (linhas x colunas) valores floats.

# Arquivos de testes

Há matrizes geradas de forma aleatória onde no seu nome tem o tamanho da sua dimensão (são matrizes quadradas)

- matriz_500_A.bin
- matriz_500_B.bin
- matriz_1000_A.bin
- matriz_1000_B.bin
- matriz_2000_A.bin
- matriz_2000_B.bin

Por fim, há um jupyter notebook utilzando Python e bibliotecas como Pandas, Matplotlib, Seaborn e Numpy para cálculo dos tempos médios de execução de cada caso (sequêncial e em quantidades diferentes de threads) para as diferentes dimensões (500, 1000 e 2000). E finalmente as métricas de Aceleração e Eficiência. 