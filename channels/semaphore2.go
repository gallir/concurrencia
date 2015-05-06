package main

import (
    "fmt"
    "runtime"
)

const (
    Procs      = 4
    MaxCount   = 10000000
    Goroutines = 4
)

type Empty struct{}
type Semaphore struct {
    value chan int
    queue chan Empty
}

func NewSemaphore(value int) Semaphore {
    var s Semaphore

    if value < 0 {
        // Don't allow negative initialization
        value = 0
    }
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
    s.value <- v
    if v <= 0 {
        s.queue <- Empty{}
    }
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
    runtime.GOMAXPROCS(Procs)
    done := make(chan Empty, 1)
    sem := NewSemaphore(1)

    for i := 0; i < Goroutines; i++ {
        go run(i, MaxCount/Goroutines, done, sem)
    }

    for i := 0; i < Goroutines; i++ {
        <-done
    }

    fmt.Printf("Counter value: %d Expected: %d\n", counter, MaxCount)
}
