# Compilação
`gcc -o geraMatrizBinario.out geraMatrizBinario.c -Wall`

`gcc -o gera_vet_rand.out gera_vet_rand.c -Wall`

`gcc -o mult_matriz_vet_seq.out mult_matriz_vet_seq.c -Wall`

`gcc -o mult_matriz_vet_conc.out mult_matriz_conc_seq.c -Wall`

# Modo de Execução
Nos arquivos .c de multiplicação de matriz por vetor caso ESCREVE esteja definido nos headers será pedido um nome de arquivo a fim de salvar os tempos de execução em formato texto. Os arquivos são abertos em modo de adição, logo é possível salvar num mesmo arquivo vários tempos de execução se esse mesmo arquivo for passado em diferentes execuções.

Os geradores foram obtidos dos programas exemplos da professora

Gerador de Matriz aleatória -> `./geraMatrizBinario.out <linhas> <colunas> <arquivo saida>`

Gerador de Vetor aleatório -> `./gera_vet_rand.out <dimensao> <arquivo saida>`

Obs: dimensão e tamanho são sinônimos em relação aos vetores

Sem ESCREVE definido

Multiplicação sequencial matriz vetor -> `./mult_matriz_vet_seq.out <arquivo matriz de entrada> <arquivo vetor de entrada> <arquivo vetor de saída>`

Multiplicação concorrente matriz vetor -> `./mult_matriz_vet_conc.out <numero de threads> <arquivo matriz de entrada> <arquivo vetor de entrada> <arquivo vetor de saída>`

Com ESCREVE definido

Multiplicação sequencial matriz vetor -> `./mult_matriz_vet_seq.out <arquivo matriz de entrada> <arquivo vetor de entrada> <arquivo vetor de saída> <arquivo de escrita dos tempos de execução>`

Multiplicação concorrente matriz vetor -> `./mult_matriz_vet_conc.out <numero de threads> <arquivo matriz de entrada> <arquivo vetor de entrada> <arquivo vetor de saída> <arquivo de escrita dos tempos de execução>`

# Como estão salvos os arquivos binários?
Matrizes -> linhas(int), colunas(int), matriz(float*) nessa sequência
Vetores -> tamanho(long int), vetor(float*) nessa sequência

Obs: (float*) signfica que há uma sequência continua de floats, onde para matrizes há linhas x colunas floats e para os vetores a quntidade de floats é o tamanho do vetor

# Arquivos de testes

Há matrizes geradas de forma aleatória onde seu nome tem o tamanho da sua diemnsão (são amtrizes quadradas)

- matriz500.bin
- matriz1000.bin
- matriz2000.bin

Há vetores geradas de forma aleatória onde seu nome tem o seu tamanho ou diemnsão

- vet500.bin
- vet1000.bin
- vet2000.bin

Os vetores resultado da operação de multiplicação segue a lógica de nomeaçãp: `vet<tam>_<tipo_execução>_out.bin`

Os arquivos gerados com os tempos de execução seguem a seguinte lógica: `tempos_<tipo_execução>_<dimensão da matriz e vetor>_<num threads>th.txt`,</br>
Cada arquivo possui tuplas, onde cada tupla signfica `(<tempo inicialização>, <tempo multiplicação>, <tempo finalização>)`

Por fim, há um jupyter notebook utilzando Python e bibliotecas como Pandas, Matplotlib, Seaborn e Numpy para cálculo dos tempos médios de execução de cada caso (sequêncial e em quantidades diferentes de threads) para as diferentes dimensões (500,1000 e 2000). E finalmente as métricas de Aceleração e Eficiência. 