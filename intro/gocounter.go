package main

import (
    "fmt"
    "runtime"
)

const (
    Procs      = 2
    MaxCount   = 10000000
    Goroutines = 2
)

var counter = 0

func run(id, counts int, done chan bool) {
    for i := 0; i < counts; i++ {
        counter++
    }
    fmt.Printf("End %d counter: %d\n", id, counter)
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
