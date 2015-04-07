#! /usr/bin/env python

import threading
import time

PHILOSOPHERS = 5
EAT_COUNT = 100

forks = []
philosophers = []

class Philosopher(threading.Thread):
    def __init__(self, id):
        super(Philosopher, self).__init__()
        self.id = id

    def right(self):
        return (self.id + 1) % PHILOSOPHERS

    def pickForks(self):
        if self.id < self.right():
            forks[self.id].acquire()
            forks[self.right()].acquire()
        else:
            forks[self.right()].acquire()
            forks[self.id].acquire()

    def releaseForks(self):
        forks[self.id].release()
        forks[self.right()].release()


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
        forks.append(threading.Lock())
        philosophers.append(Philosopher(i))

    # Start all threads
    for p in philosophers:
        p.start()

    # Wait for all threads to complete
    for p in philosophers:
        p.join()


if __name__ == "__main__":
    main()
