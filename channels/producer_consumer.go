package main

import (
    "fmt"
    "runtime"
)

const (
    Procs      = 4
    ToProduce  = 1000
    Producers  = 2
    Consumers  = 2
    BufferSize = 10
)

type Empty struct{}

func consumer(id int, done chan Empty, buffer chan string) {
    for i := 0; i < ToProduce; i++ {
        data := <-buffer
        fmt.Printf("%d read: %s\n", id, data)
    }
    done <- Empty{}
}

func producer(id int, done chan Empty, buffer chan string) {
    for i := 0; i < ToProduce; i++ {
        data := fmt.Sprintf("%d i: %d", id, i)
        buffer <- data
    }
}

func main() {
    runtime.GOMAXPROCS(Procs)
    done := make(chan Empty, 1)
    buffer := make(chan string, BufferSize)

    for i := 0; i < Producers; i++ {
        go producer(i, done, buffer)
    }

    for i := 0; i < Consumers; i++ {
        go consumer(i, done, buffer)
    }

    for i := 0; i < Consumers; i++ {
        <-done
    }

    fmt.Printf("End\n")
}
