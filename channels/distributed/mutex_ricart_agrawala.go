/* The Ricart-Agrwala distributed mutual exclusion algorithm */

package main

import (
    "fmt"
    "runtime"
    "sync"
)

type Empty struct{}

type Message struct {
    source int
    number int
}

const (
    Procs    = 4
    MaxCount = 10000000
    Nodes    = 4
)

var counter = 0

func node(id, counts int, done chan Empty, requests, replies [Nodes]chan Message) {
    myNumber := 0
    deferred := make(chan int, Nodes)
    highestNum := 0
    requestCS := false
    mutex := new(sync.Mutex)

    /* This is the asynchronous thread to receive requests from othe nodes*/
    receiver := func() {
        for {
            m := <-requests[id]
            mutex.Lock()
            if m.number > highestNum {
                highestNum = m.number
            }
            if !requestCS || (m.number < myNumber ||
                (m.number == myNumber && m.source < id)) {
                mutex.Unlock()
                replies[m.source] <- Message{source: id}
            } else {
                deferred <- m.source
                mutex.Unlock()
            }
        }
    }

    // Launch the receiver
    go receiver()

    lock := func() {
        mutex.Lock()
        requestCS = true
        myNumber = highestNum + 1
        mutex.Unlock()
        for i := range requests {
            if i == id {
                continue
            }
            requests[i] <- Message{source: id, number: myNumber}
        }
        for i := 0; i < Nodes-1; i++ {
            <-replies[id]
        }
    }

    unlock := func() {
        requestCS = false
        mutex.Lock()
        n := len(deferred)
        mutex.Unlock()
        for i := 0; i < n; i++ {
            src := <-deferred
            replies[src] <- Message{source: id}
        }
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
    runtime.GOMAXPROCS(Procs)
    done := make(chan Empty, 1)

    var requests, replies [Nodes]chan Message

    for i := range replies {
        requests[i] = make(chan Message)
        replies[i] = make(chan Message)
    }

    for i := 0; i < Nodes; i++ {
        go node(i, MaxCount/Nodes, done, requests, replies)
    }

    for i := 0; i < Nodes; i++ {
        <-done
    }

    fmt.Printf("Counter value: %d Expected: %d\n", counter, MaxCount)
}
