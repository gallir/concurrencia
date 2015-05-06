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
    PROCS    = 4
    MaxCount = 10000000
    Nodes    = 4
    None     = -1
)

var counter = 0

func node(id, counts int, done chan Empty, requests [Nodes]chan Message, replies [Nodes]chan Empty) {
    var parent int
    var holding bool
    deferred := -1

    if id == 0 {
        holding = true
        parent = None
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
            if parent == None {
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
            parent = None
            <-replies[id]
        }
        holding = false
        mutex.Unlock()
    }

    unlock := func() {
        mutex.Lock()
        if deferred != None {
            replies[deferred] <- Empty{}
            deferred = None
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
    runtime.GOMAXPROCS(PROCS)
    done := make(chan Empty, 1)

    var requests [Nodes]chan Message
    var replies [Nodes]chan Empty

    for i := range replies {
        requests[i] = make(chan Message)
        replies[i] = make(chan Empty)
    }

    for i := 0; i < Nodes; i++ {
        go node(i, MaxCount/Nodes, done, requests, replies)
    }

    for i := 0; i < Nodes; i++ {
        <-done
    }

    fmt.Printf("Counter value: %d Expected: %d\n", counter, MaxCount)
}
