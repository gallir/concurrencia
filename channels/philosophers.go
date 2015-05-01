package main

import (
    "fmt"
    "runtime"
    "time"
)

const (
    GOROUTINES   = 10
    PHILOSOPHERS = 5
    EAT_COUNT    = 100
)

type Empty struct{}

var forks [PHILOSOPHERS]chan Empty

func think(id int) {
    time.Sleep(50 * time.Millisecond)
}

func pick(id int) {
    right := (id + 1) % PHILOSOPHERS
    if id < right {
        <-forks[id]
        <-forks[right]
    } else {
        <-forks[right]
        <-forks[id]
    }
}

func eat(id int) {
    fmt.Printf("%d start eat\n", id)
    time.Sleep(100 * time.Millisecond)
    fmt.Printf("%d end eat\n", id)
}

func release(id int) {
    right := (id + 1) % PHILOSOPHERS
    forks[id] <- Empty{}
    forks[right] <- Empty{}
}

func philosopher(id int, done chan Empty) {
    for i := 0; i < EAT_COUNT; i++ {
        think(id)
        pick(id)
        eat(id)
        release(id)
    }
    done <- Empty{}
}

func main() {
    runtime.GOMAXPROCS(GOROUTINES)
    done := make(chan Empty, 1)

    for i := range forks {
        forks[i] = make(chan Empty, 1)
        forks[i] <- Empty{}
    }

    for i := 0; i < PHILOSOPHERS; i++ {
        go philosopher(i, done)
    }

    for i := 0; i < PHILOSOPHERS; i++ {
        <-done
    }

    fmt.Printf("End\n")
}
