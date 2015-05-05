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
type Semaphore struct {
    value chan int
    queue chan Empty
}

func NewSemaphore(value int) Semaphore {
    var s Semaphore

    s.value = make(chan int, 1)
    s.queue = make(chan Empty, 1)
    s.value <- value
    return s
}

func (s Semaphore) Wait() {
    v := <-s.value
    v--
    s.value <- v
    if v < 0 {
        <-s.queue
    }
}

func (s Semaphore) Signal() {
    v := <-s.value
    v++
    if v <= 0 {
        s.queue <- Empty{}
    }
    s.value <- v
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
