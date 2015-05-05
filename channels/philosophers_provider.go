package main

import (
    "fmt"
    "runtime"
    "time"
)

const (
    PROCS        = 4
    PHILOSOPHERS = 5
    EAT_COUNT    = 100

    THINKING = iota
    HUNGRY
    EATING
)

type Empty struct{}

type PhiloInfo struct {
    c     chan Empty
    id    int
    state int
}

func philosopher(id int, done chan Empty, provider chan PhiloInfo) {
    myChan := make(chan Empty)

    think := func() {
        time.Sleep(50 * time.Millisecond)
    }

    eat := func() {
        fmt.Printf("%d start eat\n", id)
        time.Sleep(100 * time.Millisecond)
        fmt.Printf("%d end eat\n", id)
    }

    pick := func() {
        provider <- PhiloInfo{id: id, c: myChan, state: HUNGRY}
        <-myChan
    }

    release := func() {
        provider <- PhiloInfo{id: id, c: myChan, state: THINKING}
    }

    for i := 0; i < EAT_COUNT; i++ {
        think()
        pick()
        eat()
        release()
    }
    done <- Empty{}
}

func provider(channel chan PhiloInfo) {
    var philosophers [PHILOSOPHERS]PhiloInfo

    right := func(i int) int {
        return (i + 1) % PHILOSOPHERS
    }

    left := func(i int) int {
        return (i + PHILOSOPHERS - 1) % PHILOSOPHERS
    }

    canEat := func(id int) {
        r := right(id)
        l := left(id)
        if philosophers[id].state == HUNGRY &&
            philosophers[l].state != EATING &&
            philosophers[r].state != EATING {
            philosophers[id].state = EATING
            philosophers[id].c <- Empty{}
        }
    }

    for i := range philosophers {
        philosophers[i].state = THINKING
    }

    for {
        m := <-channel
        philosophers[m.id] = m
        if m.state == HUNGRY {
            canEat(m.id)
        } else {
            canEat(left(m.id))
            canEat(right(m.id))
        }
    }
}

func main() {
    runtime.GOMAXPROCS(PROCS)
    done := make(chan Empty, 1)
    providerChan := make(chan PhiloInfo)

    go provider(providerChan)

    for i := 0; i < PHILOSOPHERS; i++ {
        go philosopher(i, done, providerChan)
    }

    for i := 0; i < PHILOSOPHERS; i++ {
        <-done
    }

    fmt.Printf("End\n")
}
