#! /usr/bin/env python

import threading
import time
import random

THREADS =   10
PHASES  =   20

""" Note we use mutex not only for incrementing and decrementing counter
    but also for printing, so the text don't get mangled in the console """

class Barrier(object):
    def __init__(self, n):
        self.n = n
        self.counter = 0
        self.mutex = threading.Lock()
        self.arrival = threading.Semaphore(1)
        self.departure = threading.Semaphore(0)

    def barrier(self):
        self.arrival.acquire()
        with self.mutex:
            self.counter += 1

        if self.counter < self.n:
            self.arrival.release()
        else:
            self.departure.release()

        self.departure.acquire()

        with self.mutex:
            self.counter -= 1

        if self.counter > 0:
            self.departure.release()
        else:
            self.arrival.release()



def thread(barrier):
    id = threading.current_thread().name
    print("Start thread {}".format(id))

    for i in range(PHASES):
        time.sleep(random.random())
        barrier.barrier()
        print("{} finished phase {}".format(id, i))

    barrier.barrier()
    print("Finished thread {}".format(id))

def main():
    threads = []
    barrier = Barrier(THREADS)

    for i in range(THREADS):
        # Create new threads
        t = threading.Thread(target=thread, args=(barrier,))
        threads.append(t)

    # Start all threads
    for t in threads:
        t.start()

    # Wait for all threads to complete
    for t in threads:
        t.join()



if __name__ == "__main__":
    main()
