/* Implements mutual exclusion with atomic.AddUint32 */

package main

import (
    "fmt"
    "runtime"
    "sync/atomic"
)

const (
    MAX_COUNT  = 10000000
    GOROUTINES = 4
)

var (
    counter int
    number  uint32
    turn    uint32 = 1 // AddUint32 returns the new value
)

func lock() {
    var current uint32

    current = atomic.AddUint32(&number, 1)
    /* We have to use LoadUint32 because there is no volatile vars
       and turn is read from the local cache */
    for current != atomic.LoadUint32(&turn) {
    }
}

func unlock() {
    atomic.AddUint32(&turn, 1)
}

func run(id, counts int, done chan bool) {
    for i := 0; i < counts; i++ {
        lock()
        counter++
        unlock()
    }
    fmt.Printf("End %d counter: %d\n", id, counter)
    done <- true
}

func main() {
    runtime.GOMAXPROCS(GOROUTINES)
    done := make(chan bool, 1)

    for i := 0; i < GOROUTINES; i++ {
        go run(i, MAX_COUNT/GOROUTINES, done)
    }

    for i := 0; i < GOROUTINES; i++ {
        <-done
    }

    fmt.Printf("Counter value: %d Expected: %d\n", counter, MAX_COUNT)
}
