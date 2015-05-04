package main

import (
    "fmt"
)

const (
    PHASES = 20
)

func A(done, a, b chan bool) {
    for i := 0; i < PHASES; i++ {
        a <- true
        fmt.Println("A finished phase", i)
        <-b
    }
    fmt.Println("Finished thread A")
    done <- true
}

func B(done, a, b chan bool) {
    for i := 0; i < PHASES; i++ {
        b <- true
        fmt.Println("B finished phase", i)
        <-a
    }
    fmt.Println("Finished thread B")
    done <- true
}

func main() {
    done := make(chan bool)
    a := make(chan bool, 1)
    b := make(chan bool, 1)

    go A(done, a, b)
    go B(done, a, b)

    <-done
    <-done

    fmt.Printf("End\n")
}
