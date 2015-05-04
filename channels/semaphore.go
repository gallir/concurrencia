package main

import (
    "fmt"
    "runtime"
)

const (
    PROCS      = 4
    MAX_COUNT  = 10000000
    GOROUTINES = 4
)

type Empty struct{}
type Semaphore chan Empty

func NewSemaphore(value int) Semaphore {
    s := make(Semaphore, 256) // Max
    for i := 0; i < value; i++ {
        s <- Empty{}
    }
    return s
}

func (s Semaphore) Wait() {
    <-s
}

func (s Semaphore) Signal() {
    s <- Empty{}
}

var counter = 0

func run(id, counts int, done chan Empty, sem Semaphore) {
    for i := 0; i < counts; i++ {
        sem.Wait()
        counter++
        sem.Signal()
    }
    fmt.Printf("End %d counter: %d\n", id, counter)
    done <- Empty{}
}

func main() {
    runtime.GOMAXPROCS(PROCS)
    done := make(chan Empty, 1)
    sem := NewSemaphore(1)

    for i := 0; i < GOROUTINES; i++ {
        go run(i, MAX_COUNT/GOROUTINES, done, sem)
    }

    for i := 0; i < GOROUTINES; i++ {
        <-done
    }

    fmt.Printf("Counter value: %d Expected: %d\n", counter, MAX_COUNT)
}
