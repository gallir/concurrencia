#! /usr/bin/env python

import threading
import time

PHILOSOPHERS = 5
EAT_COUNT = 100

THINKING = 0
HUNGRY = 1
EATING = 2

class Philosopher(threading.Thread):
    mutex = threading.Lock()
    status = []
    sync = []
    philosophers = []
    count = 0

    def __init__(self):
        super(Philosopher, self).__init__()
        self.id = Philosopher.count
        Philosopher.count += 1
        Philosopher.status.append(THINKING)
        Philosopher.sync.append(threading.Semaphore(0))

    def right(self, i):
        return (i - 1) % PHILOSOPHERS

    def left(self, i):
        return (i + 1) % PHILOSOPHERS

    def checkNeighbors(self, i):
        if Philosopher.status[i] == HUNGRY and Philosopher.status[self.left(i)] != EATING and Philosopher.status[self.right(i)] != EATING:
            Philosopher.status[i] = EATING
            Philosopher.sync[i].release()

    def pickForks(self):
        Philosopher.mutex.acquire()
        Philosopher.status[self.id] = HUNGRY
        self.checkNeighbors(self.id)
        Philosopher.mutex.release()
        Philosopher.sync[self.id].acquire()

    def releaseForks(self):
        Philosopher.mutex.acquire()
        Philosopher.status[self.id] = THINKING
        self.checkNeighbors(self.left(self.id))
        self.checkNeighbors(self.right(self.id))
        Philosopher.mutex.release()

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
