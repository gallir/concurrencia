/* Implements mutual exclusion with sync/mutex */

package main

import (
    "fmt"
    "runtime"
    "sync"
)

const (
    Procs      = 4
    MaxCount   = 10000000
    Goroutines = 4
)

var counter = 0

func run(id, counts int, done chan bool, mutex *sync.Mutex) {
    for i := 0; i < counts; i++ {
        mutex.Lock()
        counter++
        mutex.Unlock()
    }
    fmt.Printf("End %d counter: %d\n", id, counter)
    done <- true
}

func main() {
    runtime.GOMAXPROCS(Procs)
    done := make(chan bool, 1)
    mutex := new(sync.Mutex)

    for i := 0; i < Goroutines; i++ {
        go run(i, MaxCount/Goroutines, done, mutex)
    }

    for i := 0; i < Goroutines; i++ {
        <-done
    }

    fmt.Printf("Counter value: %d Expected: %d\n", counter, MaxCount)
}
