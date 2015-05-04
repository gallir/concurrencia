/* Implements RW lock exclusion using messages */

package main

import (
    "fmt"
    "runtime"
)

const (
    PROCS      = 4
    MAX_COUNT  = 10000000
    GOROUTINES = 4
)

type Empty struct{}
type RWLock struct {
    writer  chan Empty
    readers chan int
}

func NewRWLock() RWLock {
    var lock RWLock
    lock.writer = make(chan Empty, 1)
    lock.readers = make(chan int, 1)
    lock.writer <- Empty{}
    lock.readers <- 0
    return lock
}

func (l RWLock) readerLock() {
    readers := <-l.readers
    readers++
    if readers == 1 {
        <-l.writer
    }
    l.readers <- readers
}

func (l RWLock) readerUnlock() {
    readers := <-l.readers
    readers--
    if readers == 0 {
        l.writer <- Empty{}
    }
    l.readers <- readers
}

func (l RWLock) writerLock() {
    <-l.writer
}

func (l RWLock) writerUnlock() {
    l.writer <- Empty{}
}

var counter = 0

func run(id, counts int, done chan Empty, rwlock RWLock) {
    for i := 0; i < counts; i++ {
        if i%10 > 0 {
            rwlock.readerLock()
            // c := counter
            rwlock.readerUnlock()
        } else {
            rwlock.writerLock()
            counter++
            rwlock.writerUnlock()
        }
    }
    fmt.Printf("End %d counter: %d\n", id, counter)
    done <- Empty{}
}

func main() {
    runtime.GOMAXPROCS(PROCS)
    done := make(chan Empty, 1)
    rwlock := NewRWLock()

    for i := 0; i < GOROUTINES; i++ {
        go run(i, MAX_COUNT/GOROUTINES, done, rwlock)
    }

    for i := 0; i < GOROUTINES; i++ {
        <-done
    }

    fmt.Printf("Counter value: %d Expected: %d\n", counter, MAX_COUNT/10)
}
