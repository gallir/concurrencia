package main

import (
    "fmt"
    "runtime"
    "time"
)

const (
    PROCS        = 4
    Philosophers = 5
    EatCount     = 100
)

type Empty struct{}
type Fork chan Empty
type Forks [Philosophers]Fork

func think(id int) {
    time.Sleep(50 * time.Millisecond)
}

func right(i int) int {
    return (i + 1) % Philosophers
}

func pick(id int, forks Forks) {
    if id < right(id) {
        <-forks[id]
        <-forks[right(id)]
    } else {
        <-forks[right(id)]
        <-forks[id]
    }
}

func eat(id int) {
    fmt.Printf("%d start eat\n", id)
    time.Sleep(100 * time.Millisecond)
    fmt.Printf("%d end eat\n", id)
}

func release(id int, forks Forks) {
    forks[id] <- Empty{}
    forks[right(id)] <- Empty{}
}

func philosopher(id int, done chan Empty, forks Forks) {
    for i := 0; i < EatCount; i++ {
        think(id)
        pick(id, forks)
        eat(id)
        release(id, forks)
    }
    done <- Empty{}
}

func fork(ch chan Empty) {
    for {
        ch <- Empty{}
        <-ch
    }
}

func main() {
    runtime.GOMAXPROCS(PROCS)
    done := make(chan Empty, 1)
    var forks Forks

    for i := range forks {
        forks[i] = make(chan Empty)
        go fork(forks[i])
    }

    for i := 0; i < Philosophers; i++ {
        go philosopher(i, done, forks)
    }

    for i := 0; i < Philosophers; i++ {
        <-done
    }

    fmt.Printf("End\n")
}
