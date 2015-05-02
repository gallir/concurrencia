package main

import (
    "fmt"
    "runtime"
)

const (
    GOROUTINES  = 4
    TO_PRODUCE  = 1000
    PRODUCERS   = 2
    CONSUMERS   = 2
    BUFFER_SIZE = 10
)

type Empty struct{}

func consumer(id int, done chan Empty, buffer chan string) {
    for i := 0; i < TO_PRODUCE; i++ {
        data := <-buffer
        fmt.Printf("%d read: %s\n", id, data)
    }
    done <- Empty{}
}

func producer(id int, done chan Empty, buffer chan string) {
    for i := 0; i < TO_PRODUCE; i++ {
        data := fmt.Sprintf("%d i: %d", id, i)
        buffer <- data
    }
}

func main() {
    runtime.GOMAXPROCS(GOROUTINES)
    done := make(chan Empty, 1)
    buffer := make(chan string, BUFFER_SIZE)

    for i := 0; i < PRODUCERS; i++ {
        go producer(i, done, buffer)
    }

    for i := 0; i < CONSUMERS; i++ {
        go consumer(i, done, buffer)
    }

    for i := 0; i < CONSUMERS; i++ {
        <-done
    }

    fmt.Printf("End\n")
}
