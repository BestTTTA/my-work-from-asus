package main

import (
	"fmt"
	"time"
)

func main() {
	jobCh := make(chan int, 5)
	resultCh := make(chan int, 5)

	for i := range 5 {
		jobCh <- i + 1
	}
	close(jobCh)

	go double(jobCh, resultCh)
	go double(jobCh, resultCh)
	
	for range 5 {
		result := <-resultCh
		fmt.Println(result)
	}

}

func double(jobCh <-chan int, resultCh chan<- int) {
	for i := range jobCh {
		time.Sleep(1 * time.Second)
		resultCh <- i * 2
	}
}
