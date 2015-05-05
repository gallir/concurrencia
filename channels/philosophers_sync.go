package main

import (
    "fmt"
    "runtime"
    "time"
)

const (
    PROCS        = 4
    GOROUTINES   = 10
    PHILOSOPHERS = 5
    EAT_COUNT    = 100
)

type Empty struct{}
type Fork chan Empty
type Forks [PHILOSOPHERS]Fork

func think(id int) {
    time.Sleep(50 * time.Millisecond)
}

func right(i int) int {
    return (i + 1) % PHILOSOPHERS
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
    for i := 0; i < EAT_COUNT; i++ {
        think(id)
        pick(id, forks)
        eat(id)
        release(id, forks)
    }
    done <- Empty{}
}

func fork(id int, ch chan Empty) {
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
        go fork(i, forks[i])
    }

    for i := 0; i < PHILOSOPHERS; i++ {
        go philosopher(i, done, forks)
    }

    for i := 0; i < PHILOSOPHERS; i++ {
        <-done
    }

    fmt.Printf("End\n")
}
