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
    GOROUTINES = 2
)

var counter = 0

func run(id, counts int, done, mutex chan bool) {
    for i := 0; i < counts; i++ {
        <-mutex
        counter++
        mutex <- true
    }
    fmt.Printf("End %d counter: %d\n", id, counter)
    done <- true
}

func main() {
    runtime.GOMAXPROCS(GOROUTINES)
    done, mutex := make(chan bool, 1), make(chan bool, 1)

    mutex <- true

    for i := 0; i < GOROUTINES; i++ {
        go run(i, MAX_COUNT/GOROUTINES, done, mutex)
    }

    for i := 0; i < GOROUTINES; i++ {
        <-done
    }

    fmt.Printf("Counter value: %d Expected: %d\n", counter, MAX_COUNT)
}
