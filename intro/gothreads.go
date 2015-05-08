package main

import (
    "fmt"
)

const (
    Goroutines = 2
)

func run(id int, done chan bool) {
    fmt.Printf("Hi, I'm thread %d\n", id)
    done <- true
}

func main() {
    done := make(chan bool, 1)

    for i := 0; i < Goroutines; i++ {
        go run(i, done)
    }

    for i := 0; i < Goroutines; i++ {
        <-done
    }
}
