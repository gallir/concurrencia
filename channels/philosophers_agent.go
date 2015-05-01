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

func think(id int) {
    time.Sleep(50 * time.Millisecond)
}

func eat(id int) {
    fmt.Printf("%d start eat\n", id)
    time.Sleep(100 * time.Millisecond)
    fmt.Printf("%d end eat\n", id)
}

func pick(id int, table chan PhiloInfo, myChan chan Empty) {
    table <- PhiloInfo{id: id, c: myChan, state: HUNGRY}
    <-myChan
}

func release(id int, table chan PhiloInfo, myChan chan Empty) {
    table <- PhiloInfo{id: id, c: myChan, state: THINKING}
}

func philosopher(id int, done chan Empty, table chan PhiloInfo) {
    myChan := make(chan Empty)

    for i := 0; i < EAT_COUNT; i++ {
        think(id)
        pick(id, table, myChan)
        eat(id)
        release(id, table, myChan)
    }
    done <- Empty{}
}

func table(channel chan PhiloInfo) {
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
    runtime.GOMAXPROCS(GOROUTINES)
    done := make(chan Empty, 1)
    tableChan := make(chan PhiloInfo)

    go table(tableChan)

    for i := 0; i < PHILOSOPHERS; i++ {
        go philosopher(i, done, tableChan)
    }

    for i := 0; i < PHILOSOPHERS; i++ {
        <-done
    }

    fmt.Printf("End\n")
}
