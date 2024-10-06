# Implementação de leitor e escritor com prioridade para operações de escrita

## Lógica
- Sempre que uma operação de escrita for solicitada, novas operações de leitura não podem começar (mesmo que outras operações de leitura já estejam acontecendo), até que a operação de escrita seja atendida.
- A quantidade de operações foi reduzida para 500 com a finalidade de ser possível verificar uma quantidade menor de mensagens de log.

## Compilação

`gcc list_int.c rwlock_priority_wr.c -o rwlock_wr -Wall`

## Execução

`./rwlock_wr <número de threads>`

## Mensagens de log

### Para threads entrando em leitura
- Se houver uma thread escrevendo a mensagem "Thread \<id\> esperando escrita terminar." será exibida.
- Se houver uma thread escritora que já tenha solicitado escrita a mensagem "Thread \<id\> leitora esperando escrita solicitada." será exibida.

### Para threads saindo de leitura
- Se houver uma thread escritora que já tenha solicitado escrita a mensagem "Thread \<id\> terminou a leitura. Leitores = \<leit\>." será exibida. Essa mensagem é importante pois quando há uma solicitação de escrita as threads que ainda estão lendo precisam terminar suas leituras e informar a quantidade de threads que estão lendo restantes.


### Para threads entrando em escrita
- Se houver threads lendo a mensagem "Thread \<id\> aguardando leitores terminarem. Leitores = \<leit\>" será exibida.
- Se houver outro escritor escrevendo a mensagem "Thread \<id\> aguardando outro escritor terminar." será exibida.

### Para threads saindo da escrita
- A mensagem "Thread \<id\> terminou escrita." será exibida.


# Respostas questões

## Atividade 1

2.
Poderia sim. Há muitos problemas em que você pode resolver utilizando uma estrutura de dados de tipo abstrato de dados, pois esse tipo pode conter valores e operações diversas quanto se queira para se adaptar a um problema que se quer resolver. Portanto, a forma como as threads utilizam essa estrutura de dados é por meio das funções que simplificam e abstraem a implementação dessa estrutura de dados.

4.
Podemos ter inconsistências se operações de leitura e escrita acontecerem simultaneamente ocasionando corrida de dados e violação de atomicidade do código.

6.
O tempo de execução tende a aumentar. Isso acontece porque estamos utilizando exclusão mútua e o overhead causado pela estrutura de controle da implementação da exclusão mútua demanda mais tempo de execução, o que é diferente quando apenas uma thread está executando de forma sequencial e não é interrompida por outra thread que tenta obter o mutex e não tem sucesso em obtê-lo. Tal cenário quando repetido muitas vezes representa um tempo em que o código não estava de fato executando as operações na lista, mas apenas alternando e verificando estados entre as threads.

## Atividade 2

4.
Em cenários em que o número de leituras (consulta) representa uma porcentagem muito maior que de escritas (inserção e remoção).

