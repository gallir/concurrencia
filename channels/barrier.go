package main

import (
    "fmt"
    "math/rand"
    "runtime"
    "time"
)

const (
    PROCS      = 4
    PHASES     = 20
    GOROUTINES = 10
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
    v--
    if v > 0 {
        b.arrival <- v
    } else {
        b.departure <- b.n
    }

    v = <-b.departure

    v--
    if v > 0 {
        b.departure <- v
    } else {
        b.arrival <- b.n
    }
}

func run(id int, done chan Empty, b *Barrier) {
    for i := 0; i < PHASES; i++ {
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
    barrier := NewBarrier(GOROUTINES)

    for i := 0; i < GOROUTINES; i++ {
        go run(i, done, barrier)
    }

    for i := 0; i < GOROUTINES; i++ {
        <-done
    }

    fmt.Printf("End\n")
}
