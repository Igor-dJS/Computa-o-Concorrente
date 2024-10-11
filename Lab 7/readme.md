# Descrição
Utilizando como texto de entrada o Capítulo IX - Baleia do livro Vidas Secas de Graciliano Ramos temos que a Thread 1 é responsável por ler os caracteres do arquivo de entrada e os coloca no buffer1, a Thread 2 é responsável por ler os caracteres no buffer2 e processá-los de acordo com os seguintes requisitos: acrescentar um caractere de final de linha a cada 2n + 1 caracteres lidos (n = 0, 1, 2..10) e deposita os caracteres no buffer2. Depois de ‘n’ atingir o valor 10, o caracter de final de linha deve ser inserido a cada bloco de 10 caracteres lidos. Por fim, a Thread 3 é responsável por ler os caracteres do buffer2 e imprimir na saída padrão (geralmente o terminal).

Foi utilizado a implementação de produtor e consumidor com semáforos entre a thread 1 e a thread 2 e entre a thread 2 e a thread 3.

# Compilação
`gcc ler_arquivo.c -o ler_arquivo -Wall`

# Execução
`./ler_arquivo`
