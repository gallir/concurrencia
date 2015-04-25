#! /usr/bin/env python

import threading
import time

PHILOSOPHERS = 5
EAT_COUNT = 100

class Philosopher(threading.Thread):
    mutex = threading.Lock()
    forks = [] #forks available for each philosopher
    canEat = []
    count = 0

    def __init__(self):
        super(Philosopher, self).__init__()
        self.id = Philosopher.count
        self.right = (self.id - 1) % PHILOSOPHERS
        self.left = (self.id + 1) % PHILOSOPHERS
        Philosopher.count += 1
        Philosopher.forks.append(2)
        Philosopher.canEat.append(threading.Condition(Philosopher.mutex))

    def pick(self):
        with Philosopher.mutex:
            while Philosopher.forks[self.id] != 2:
                Philosopher.canEat[self.id].wait()

            Philosopher.forks[self.left] -= 1
            Philosopher.forks[self.right] -= 1

    def release(self):
        with Philosopher.mutex:
            Philosopher.forks[self.left] += 1
            Philosopher.forks[self.right] += 1

            if Philosopher.forks[self.left] == 2:
                Philosopher.canEat[self.left].notify()
            if Philosopher.forks[self.right] == 2:
                Philosopher.canEat[self.right].notify()

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
