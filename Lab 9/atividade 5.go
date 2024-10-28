package main

import (
	"fmt"
	"math"
	"sync"
	"os"
	"strconv"
)

// Função das GoRoutines
func ehPrimo(n int64) bool {
	if n <= 1 {
		return false
	}
	if n == 2 {
		return true
	}
	if n%2 == 0 {
		return false
	}
	for i := int64(3); i <= int64(math.Sqrt(float64(n))); i += 2 {
		if n%i == 0 {
			return false
		}
	}
	return true
}

func main() {
	// Verifica argumentos
	if len(os.Args) < 3 {
		fmt.Println("Por favor, forneça o número de GoRoutines e o valor de N.")
		fmt.Println("go run atividade\\ 5.go <num de goroutines> <valor de N>")
		return
	}

	var M int
	var N int64

	// Converte M recebido por linha de comando para int
	M, err1 := strconv.Atoi(os.Args[1])
	if err1 != nil {
		fmt.Printf("Erro ao converter o argumento para inteiro: %v\n", err1)
		return
	}

	// Converte N recebido por linha de comando para int64
	N, err2 := strconv.ParseInt(os.Args[2], 10, 64) // ParseInt com argumento para base 10 e btisize 64
	if err2 != nil {
		fmt.Printf("Erro ao converter o argumento para inteiro: %v\n", err2)
		return
	}

	var wg sync.WaitGroup

	// Cria canal para as GoRoutines receberem o argumento e canal para a main receber resultado das GoRoutines que verificam a primalidade
	numeros := make(chan int64, N)
	resultados := make(chan bool)

	// Cria as Goroutines para verificar a primalidade
	for i := 0; i < M; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			for n := range numeros {
				resultados <- ehPrimo(n)
			}
		}()
	}

	// GoRoutine que irá esperar as GoRoutines de primalidade terminarem para depois fechar o canal de resultados
	go func() {
		wg.Wait()
		close(resultados)
	}()

	// Passa argumento para as GoRoutines via canal "numeros". Como executam todos a mesma tarefa (verificar a primalidade) não importa qual GoRoutine irá receber qual argumento.
	for i := int64(1); i <= N; i++ {
		numeros <- i
	}
	close(numeros)

	// Contar os números primos encontrados. A função main executará em conjunto com as outras GoRoutines e consumirá do canal 
	// toda vez que uma GoRoutine escrever no canal e incrementará o contador quando o resultado lido for 'true'
	count := 0
	for res := range resultados {
		if res {
			count++
		}
	}

	// Imprime quantidade de primos encontrados
	fmt.Printf("Total de números primos encontrados: %d\n", count)
}
