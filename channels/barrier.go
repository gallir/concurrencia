package main

import (
    "fmt"
    "math/rand"
    "runtime"
    "time"
)

const (
    PROCS      = 4
    Phases     = 20
    Goroutines = 10
)

type Empty struct{}

type Barrier struct {
    arrival   chan int
    departure chan int
    n         int
}

func NewBarrier(value int) *Barrier {
    b := new(Barrier)
    b.arrival = make(chan int, 1)
    b.departure = make(chan int, 1)
    b.n = value

    b.arrival <- value
    return b
}

func (b *Barrier) Barrier() {
    var v int

    v = <-b.arrival
    if v > 1 {
        v--
        b.arrival <- v
    } else {
        b.departure <- b.n
    }

    v = <-b.departure
    if v > 1 {
        v--
        b.departure <- v
    } else {
        b.arrival <- b.n
    }
}

func run(id int, done chan Empty, b *Barrier) {
    for i := 0; i < Phases; i++ {
        time.Sleep(time.Duration(rand.Intn(1000)) * time.Millisecond)
        b.Barrier()
        fmt.Printf("%d finished phase %d\n", id, i)
    }

    b.Barrier()
    fmt.Printf("Finished thread %d\n", id)
    done <- Empty{}
}

func main() {
    runtime.GOMAXPROCS(PROCS)
    done := make(chan Empty, 1)
    barrier := NewBarrier(Goroutines)

    for i := 0; i < Goroutines; i++ {
        go run(i, done, barrier)
    }

    for i := 0; i < Goroutines; i++ {
        <-done
    }

    fmt.Printf("End\n")
}
