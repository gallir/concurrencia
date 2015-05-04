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
        <-a
        b <- true
        fmt.Println("B finished phase", i)
    }
    fmt.Println("Finished thread B")
    done <- true
}

func main() {
    done := make(chan bool)
    a := make(chan bool)
    b := make(chan bool)

    go A(done, a, b)
    go B(done, a, b)

    <-done
    <-done

    fmt.Printf("End\n")
}
