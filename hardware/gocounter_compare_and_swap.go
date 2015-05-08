/* Implements mutual exclusion with atomic.CompareAndSwapInt32 */

package main

import (
    "fmt"
    "runtime"
    "sync/atomic"
)

const (
    Procs      = 4
    MaxCount   = 10000000
    Goroutines = 4
)

var (
    counter       = 0
    mutex   int32 = 0
)

func lock() {
    for !atomic.CompareAndSwapInt32(&mutex, 0, 1) {
    }
}

func unlock() {
    mutex = 0
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
    runtime.GOMAXPROCS(Procs)
    done := make(chan bool, 1)

    for i := 0; i < Goroutines; i++ {
        go run(i, MaxCount/Goroutines, done)
    }

    for i := 0; i < Goroutines; i++ {
        <-done
    }

    fmt.Printf("Counter value: %d Expected: %d\n", counter, MaxCount)
}
