/* The Neilsen-Mizuno token passing distributed algorithm */

package main

import (
    "fmt"
    "runtime"
    "sync"
)

type Empty struct{}

type Message struct {
    source     int
    originator int
}

const (
    MAX_COUNT  = 10000000
    NODES      = 4
    GOROUTINES = 4
    NONE       = -1
)

var counter = 0

func node(id, counts int, done chan Empty, requests [NODES]chan Message, replies [NODES]chan Empty) {
    var parent int
    var holding bool
    deferred := -1

    if id == 0 {
        holding = true
        parent = NONE
    } else {
        holding = false
        parent = 0
    }
    mutex := new(sync.Mutex)

    /* This is the asynchronous thread to receive requests from othe nodes*/
    receiver := func() {
        for {
            m := <-requests[id]
            mutex.Lock()
            if parent == NONE {
                if holding {
                    replies[m.originator] <- Empty{}
                    holding = false
                } else {
                    deferred = m.originator
                }
            } else {
                requests[parent] <- Message{source: id, originator: m.originator}
            }
            parent = m.source
            mutex.Unlock()
        }
    }

    // Launch the receiver
    go receiver()

    lock := func() {
        mutex.Lock()
        if !holding {
            requests[parent] <- Message{id, id}
            parent = NONE
            <-replies[id]
        }
        holding = false
        mutex.Unlock()
    }

    unlock := func() {
        mutex.Lock()
        if deferred != NONE {
            replies[deferred] <- Empty{}
            deferred = NONE
        } else {
            holding = true
        }
        mutex.Unlock()
    }

    for i := 0; i < counts; i++ {
        lock()
        counter++
        unlock()
    }

    fmt.Printf("End %d counter: %d\n", id, counter)
    done <- Empty{}
}

func main() {
    runtime.GOMAXPROCS(GOROUTINES)
    done := make(chan Empty, 1)

    var requests [NODES]chan Message
    var replies [NODES]chan Empty

    for i := range replies {
        requests[i] = make(chan Message)
        replies[i] = make(chan Empty)
    }

    for i := 0; i < NODES; i++ {
        go node(i, MAX_COUNT/NODES, done, requests, replies)
    }

    for i := 0; i < NODES; i++ {
        <-done
    }

    fmt.Printf("Counter value: %d Expected: %d\n", counter, MAX_COUNT)
}
