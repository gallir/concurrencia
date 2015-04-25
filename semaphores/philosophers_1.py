#! /usr/bin/env python

import threading
import time

PHILOSOPHERS = 5
EAT_COUNT = 100

class Philosopher(threading.Thread):
    forks = []
    count = 0

    def __init__(self):
        super(Philosopher, self).__init__()
        self.id = Philosopher.count
        Philosopher.count += 1
        Philosopher.forks.append(threading.Lock())

    def right(self):
        return (self.id + 1) % PHILOSOPHERS

    def pick(self):
        if self.id < self.right():
            Philosopher.forks[self.id].acquire()
            Philosopher.forks[self.right()].acquire()
        else:
            Philosopher.forks[self.right()].acquire()
            Philosopher.forks[self.id].acquire()

    def release(self):
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
            self.pick()
            self.eat()
            self.release()

def main():
    philosophers = []

    for i in range(PHILOSOPHERS):
        philosophers.append(Philosopher())

    # Start all threads
    for p in philosophers:
        p.start()

    # Wait for all threads to complete
    for p in philosophers:
        p.join()


if __name__ == "__main__":
    main()
