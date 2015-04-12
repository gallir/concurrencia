#! /usr/bin/env python

import threading
import time
import random

THREADS =   10
PHASES  =   20

""" Note we use mutex not only for incrementing and decrementing counter
    but also for printing, so the text don't get mangled in the console """

class Barrier(threading.Thread):
    counter = 0
    mutex = threading.Lock()
    arrival = threading.Semaphore(1)
    departure = threading.Semaphore(0)

    def __init__(self, threadID):
        super(Barrier, self).__init__()
        self.threadID = threadID

    def barrier(self, n):
        Barrier.arrival.acquire()
        with Barrier.mutex:
            Barrier.counter += 1

        if Barrier.counter < n:
            Barrier.arrival.release()
        else:
            Barrier.departure.release()

        Barrier.departure.acquire()

        with Barrier.mutex:
            Barrier.counter -= 1

        if Barrier.counter > 0:
            Barrier.departure.release()
        else:
            Barrier.arrival.release()


    def run(self):
        with Barrier.mutex:
            print("Start thread {}".format(self.threadID))

        for i in range(PHASES):
            time.sleep(random.random())
            self.barrier(THREADS)
            with Barrier.mutex:
                print("{} finished phase {}".format(self.threadID, i))

        self.barrier(THREADS)
        with Barrier.mutex:
            print("Finished thread {}".format(self.threadID))


def main():
    threads = []

    for i in range(THREADS):
        # Create new threads
        t = Barrier(i)
        threads.append(t)

    # Start all threads
    for t in threads:
        t.start()

    # Wait for all threads to complete
    for t in threads:
        t.join()



if __name__ == "__main__":
    main()
