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
