/* Parallel Matrix Multiplication using synchronous channels
 * as described by M. Ben-Ari in Principles of Concurrent and Distributed Programming
 * http://goo.gl/uwXu5H
 */

package main

import (
    "fmt"
    "runtime"
)

const (
    PROCS = 4
    DIM   = 3
)

type Empty struct{}
type Row [DIM]int
type Matrix [DIM]Row
type RowResult struct {
    i   int
    row Row
}

func matrix1() [3]Row {
    /* First matrix */
    var m [DIM]Row
    m[0] = Row{1, 2, 3}
    m[1] = Row{4, 5, 6}
    m[2] = Row{7, 8, 9}
    return m
}

func matrix2() [3]Row {
    /* Second matrix */
    var m [DIM]Row
    m[0] = Row{1, 0, 2}
    m[1] = Row{0, 1, 2}
    m[2] = Row{1, 0, 0}
    return m
}

func zero(west chan int) {
    for i := 0; i < DIM; i++ {
        west <- 0
    }
}

func sink(north chan int) {
    for i := 0; i < DIM; i++ {
        <-north
    }
}

func result(rowNum int, east chan int, done chan RowResult) {
    var row [DIM]int

    for i := 0; i < DIM; i++ {
        row[i] = <-east
    }
    done <- RowResult{rowNum, row}
}

func source(row Row, south chan int) {
    for i := range row {
        south <- row[i]
    }
}

func multiplier(first int, north, east, south, west chan int) {
    for i := 0; i < DIM; i++ {
        second := <-north
        sum := <-east
        sum = sum + first*second
        west <- sum
        south <- second
    }
}

func main() {
    runtime.GOMAXPROCS(PROCS)
    done := make(chan RowResult, 1)

    var north [DIM][DIM]chan int
    var east [DIM][DIM]chan int
    var south [DIM][DIM]chan int
    var west [DIM][DIM]chan int

    m1 := matrix1()
    m2 := matrix2()

    /* Create the required channels */
    for i := 0; i < DIM; i++ {
        for j := 0; j < DIM; j++ {
            if i == 0 {
                /* First row elements need a new a north channel */
                north[i][j] = make(chan int)
            } else {
                /* This' north if the south of the above element */
                north[i][j] = south[i-1][j]
            }

            if j == 0 {
                /* First column's elements need a new west channel */
                west[i][j] = make(chan int)
            } else {
                /* otherwise reuse the east channel of the its western element */
                west[i][j] = east[i][j-1]
            }

            east[i][j] = make(chan int)
            south[i][j] = make(chan int)
        }
    }

    /* Now we start the different processes
     * (in a differente traversals just for clarity
     * furthermore the channeld could be reused for
     * additional multiplications )
     */
    for i := 0; i < DIM; i++ {
        for j := 0; j < DIM; j++ {
            /* The source of the second matrix's rows */
            if i == 0 {
                go source(m2[j], north[i][j])
            }

            /* The recipient for the row result */
            if j == 0 {
                go result(i, west[i][j], done)
            }

            /* Last column's elements need a zero sum "provider" */
            if j == DIM-1 {
                go zero(east[i][j])
            }

            /* And last row's elements need a sink to avoid deadlock */
            if i == DIM-1 {
                go sink(south[i][j])
            }

            /* This is the one doing the real job */
            go multiplier(m1[i][j], north[i][j], east[i][j], south[i][j], west[i][j])
        }
    }

    /* Wait for the results */
    var results [DIM]Row
    for i := 0; i < DIM; i++ {
        r := <-done
        results[r.i] = r.row
    }

    for i := range results {
        fmt.Printf("Result[%d]: [", i)
        for j := range results[i] {
            fmt.Printf("%6d", results[i][j])
        }
        fmt.Printf("]\n")
    }
}
