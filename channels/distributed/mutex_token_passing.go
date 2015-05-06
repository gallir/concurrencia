/* The Ricart-Agrawala token passing distributed  algorithm */

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

func node(id, counts int, done chan Empty, requests [Nodes]chan Message, replies [Nodes]chan [Nodes]int) {
    var requested [Nodes]int
    var granted [Nodes]int
    myNumber := 0
    inCS := false
    var haveToken bool

    if id == 0 {
        haveToken = true
    } else {
        haveToken = false
    }
    mutex := new(sync.Mutex)

    sendToken := func() {
        for i := range requested {
            if requested[i] > granted[i] {
                replies[i] <- granted
                haveToken = false
                return
            }
        }
    }

    /* This is the asynchronous thread to receive requests from othe nodes*/
    receiver := func() {
        for {
            m := <-requests[id]
            if m.number > requested[m.source] {
                requested[m.source] = m.number
            }
            mutex.Lock()
            if haveToken && !inCS {
                sendToken()
            }
            mutex.Unlock()
        }
    }

    // Launch the receiver
    go receiver()

    lock := func() {
        mutex.Lock()
        if !haveToken {
            myNumber++
            mutex.Unlock()
            for i := range requests {
                if i == id {
                    continue
                }
                requests[i] <- Message{source: id, number: myNumber}
            }
            granted = <-replies[id]
            mutex.Lock()
            haveToken = true
        }
        inCS = true
        mutex.Unlock()
    }

    unlock := func() {
        mutex.Lock()
        granted[id] = myNumber
        inCS = false
        sendToken()
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
    runtime.GOMAXPROCS(Procs)
    done := make(chan Empty, 1)

    var requests [Nodes]chan Message
    var replies [Nodes]chan [Nodes]int

    for i := range replies {
        requests[i] = make(chan Message)
        replies[i] = make(chan [Nodes]int)
    }

    for i := 0; i < Nodes; i++ {
        go node(i, MaxCount/Nodes, done, requests, replies)
    }

    for i := 0; i < Nodes; i++ {
        <-done
    }

    fmt.Printf("Counter value: %d Expected: %d\n", counter, MaxCount)
}
