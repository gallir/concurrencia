/* Implements mutual exclusion using synchronous channels
 * WARN: it works but it's not recommended, it has more overhead that sync/mutex
 */

package main

import (
    "fmt"
    "runtime"
)

const (
    PROCS      = 4
    MaxCount   = 10000000
    Goroutines = 4
)

type Empty struct{}
type Mutex chan Empty

func NewMutex() Mutex {
    m := make(Mutex)
    go func() {
        for {
            m <- Empty{}
            <-m
        }
    }()
    return m
}

func (m Mutex) Lock() {
    <-m
}

func (m Mutex) Unlock() {
    m <- Empty{}
}

var counter = 0

func run(id, counts int, done chan Empty, mutex Mutex) {
    for i := 0; i < counts; i++ {
        mutex.Lock()
        counter++
        mutex.Unlock()
    }
    fmt.Printf("End %d counter: %d\n", id, counter)
    done <- Empty{}
}

func main() {
    runtime.GOMAXPROCS(PROCS)
    done := make(chan Empty, 1)
    mutex := NewMutex()

    for i := 0; i < Goroutines; i++ {
        go run(i, MaxCount/Goroutines, done, mutex)
    }

    for i := 0; i < Goroutines; i++ {
        <-done
    }

    fmt.Printf("Counter value: %d Expected: %d\n", counter, MaxCount)
}
