package main

import (
    "fmt"
)

const (
    GOROUTINES = 2
)

func run(id int, done chan bool) {
    fmt.Printf("Hi, I'm thread %d\n", id)
    done <- true
}

func main() {
    done := make(chan bool, 1)

    for i := 0; i < GOROUTINES; i++ {
        go run(i, done)
    }

    for i := 0; i < GOROUTINES; i++ {
        <-done
    }
}
