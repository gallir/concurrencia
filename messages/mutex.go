/* Implements mutual exclusion using messages
 * WARN: it works but it's not recommended, it has more overhead that sync/mutex
 */

package main

import (
    "fmt"
    "runtime"
)

const (
    MAX_COUNT  = 10000000
    GOROUTINES = 4
)

type Empty struct{}
type Mutex chan Empty

func NewMutex() Mutex {
    m := make(Mutex, 1)
    m <- Empty{}
    return m
}

func (m Mutex) Lock() {
    <- m
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
    runtime.GOMAXPROCS(GOROUTINES)
    done := make(chan Empty, 1)
    mutex := NewMutex()

    for i := 0; i < GOROUTINES; i++ {
        go run(i, MAX_COUNT/GOROUTINES, done, mutex)
    }

    for i := 0; i < GOROUTINES; i++ {
        <-done
    }

    fmt.Printf("Counter value: %d Expected: %d\n", counter, MAX_COUNT)
}
