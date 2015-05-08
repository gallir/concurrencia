package main

import (
    "fmt"
    "runtime"
    "sync/atomic"
)

const (
    Procs      = 4
    MaxCount   = 10000000
    Goroutines = 4
)

var (
    counter int32
)

func run(id, counts int, done chan bool) {
    var c int32
    for i := 0; i < counts; i++ {
        c = atomic.AddInt32(&counter, 1)
    }
    fmt.Printf("End %d counter: %d\n", id, c)
    done <- true
}

func main() {
    runtime.GOMAXPROCS(Procs)
    done := make(chan bool, 1)

    for i := 0; i < Goroutines; i++ {
        go run(i, MaxCount/Goroutines, done)
    }

    for i := 0; i < Goroutines; i++ {
        <-done
    }

    fmt.Printf("Counter value: %d Expected: %d\n", counter, MaxCount)
}
