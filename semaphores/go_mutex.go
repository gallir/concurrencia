/* Implements mutual exclusion with sync/mutex */

package main

import (
    "fmt"
    "runtime"
    "sync"
)

const (
    PROCS      = 4
    MAX_COUNT  = 10000000
    GOROUTINES = 4
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
    runtime.GOMAXPROCS(PROCS)
    done := make(chan bool, 1)
    mutex := new(sync.Mutex)

    for i := 0; i < GOROUTINES; i++ {
        go run(i, MAX_COUNT/GOROUTINES, done, mutex)
    }

    for i := 0; i < GOROUTINES; i++ {
        <-done
    }

    fmt.Printf("Counter value: %d Expected: %d\n", counter, MAX_COUNT)
}
