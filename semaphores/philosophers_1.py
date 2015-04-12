#! /usr/bin/env python

import threading
import time

PHILOSOPHERS = 5
EAT_COUNT = 100

class Philosopher(threading.Thread):
    forks = []
    philosophers = []

    def __init__(self, id):
        super(Philosopher, self).__init__()
        self.id = id

    def right(self):
        return (self.id + 1) % PHILOSOPHERS

    def pickForks(self):
        if self.id < self.right():
            Philosopher.forks[self.id].acquire()
            Philosopher.forks[self.right()].acquire()
        else:
            Philosopher.forks[self.right()].acquire()
            Philosopher.forks[self.id].acquire()

    def releaseForks(self):
        Philosopher.forks[self.id].release()
        Philosopher.forks[self.right()].release()


    def think(self):
        time.sleep(0.05)

    def eat(self):
        print("{} start eat".format(self.id))
        time.sleep(0.1)
        print("{} end eat".format(self.id))

    def run(self):
        for i in range(EAT_COUNT):
            self.think()
            self.pickForks()
            self.eat()
            self.releaseForks()

def main():

    # Initialize forks locks and philosophers threads
    for i in range(PHILOSOPHERS):
        Philosopher.forks.append(threading.Lock())
        Philosopher.philosophers.append(Philosopher(i))

    # Start all threads
    for p in Philosopher.philosophers:
        p.start()

    # Wait for all threads to complete
    for p in Philosopher.philosophers:
        p.join()


if __name__ == "__main__":
    main()
