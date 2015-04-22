#! /usr/bin/env python

import threading
import time
import random

THREADS =   10
PHASES  =   20

class Barrier(threading.Thread):
    arrived = 0
    mutex = threading.Lock()
    allArrived = threading.Condition(mutex)

    def __init__(self, threadID):
        super(Barrier, self).__init__()
        self.threadID = threadID

    def barrier(self, n):
        with Barrier.mutex:
            Barrier.arrived += 1

            if Barrier.arrived == n:
                Barrier.arrived = 0
                Barrier.allArrived.notify_all()
            else:
                Barrier.allArrived.wait()


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
