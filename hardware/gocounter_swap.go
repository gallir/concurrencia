/* Implements mutual exclusion with atomic.SwapInt32 */

package main

import (
	"fmt"
	"runtime"
	"sync/atomic"
)

const (
	MAX_COUNT  = 10000000
	GOROUTINES = 4
)

var (
	counter int
	mutex int32 
)

func lock() {
	for atomic.SwapInt32(&mutex, 1) == 1 {} // While it returns 1
}

func unlock() {
	mutex = 0
}

func run(id, counts int, done chan bool) {
	for i := 0; i < counts; i++ {
		lock()
		counter++
		unlock()
	}
	fmt.Printf("End %d counter: %d\n", id, counter)
	done <- true
}

func main() {
	runtime.GOMAXPROCS(GOROUTINES)
	done := make(chan bool, 1)

	for i := 0; i < GOROUTINES; i++ {
		go run(i, MAX_COUNT/GOROUTINES, done)
	}

	for i := 0; i < GOROUTINES; i++ {
		<-done
	}

	fmt.Printf("Counter value: %d Expected: %d\n", counter, MAX_COUNT);
}
