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

    def __init__(self, id):
        super(Philosopher, self).__init__()
        self.id = id

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

    # Initialize status', sync semaphores and philosophers threads
    for i in range(PHILOSOPHERS):
        Philosopher.status.append(THINKING)
        Philosopher.sync.append(threading.Semaphore(0))
        Philosopher.philosophers.append(Philosopher(i))

    # Start all threads
    for p in Philosopher.philosophers:
        p.start()

    # Wait for all threads to complete
    for p in Philosopher.philosophers:
        p.join()


if __name__ == "__main__":
    main()
