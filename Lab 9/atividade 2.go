// introduz canais na aplicação Go
package main

import (
	"fmt"
	"time"
)

func tarefa(str1 chan string, str2 chan string) {
	var msg = ""

	//lê do canal st2
	msg = <-str2
	fmt.Println("Routine recebeu: ", msg)

	//escreve no canal str1
	str1 <- "Oi Main, bom dia, tudo bem?"

	//lê do canal st2
	msg = <-str2
	fmt.Println("Routine recebeu: ", msg)

	//escreve no canal str1
	str1 <- "Certo, entendido."

	// Para garantir que a Go Routine se bloqueie para imprimir a mensagem de finalização primeiro
	msg = <-str2

	fmt.Printf("Go Routine Finalizando...\n")
}

func main() {
	var msg = "" //string vazia

	//cria um canal de comunicacao nao-bufferizado
	str1 := make(chan string)
	str2 := make(chan string)

	//cria um goroutine que executará a função 'tarefa'
	go tarefa(str1, str2)

	str2 <- "Olá, Goroutine, bom dia!"

	//lê do canal st1
	msg = <-str1
	fmt.Println("Main recebeu: ", msg)

	//escreve no canal str2
	str2 <- "Tudo bem! Vou terminar tá?"

	//lê do canal st1
	msg = <-str1
	fmt.Println("Main recebeu: ", msg)

	// Para que a Go routine imrpima a sua mensagem de finalização primeiro
	str2 <- "Sincroniza"

	time.Sleep(time.Second) //bloqueio na main
	fmt.Println("Main Finalizando...")
}
