#! /usr/bin/env python

import threading
import time
import random

THREADS =   10
PHASES  =   20

class Barrier(object):
    def __init__(self, n):
        self.arrived = 0
        self.n = n
        self.mutex = threading.Lock()
        self.allArrived = threading.Condition(self.mutex)

    def barrier(self):
        with self.mutex:
            self.arrived += 1

            if self.arrived == self.n:
                self.arrived = 0
                self.allArrived.notify_all()
            else:
                self.allArrived.wait()


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
