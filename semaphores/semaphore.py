#! /usr/bin/env python

import threading
import time

THREADS = 4
MAX_COUNT = 10000000


class Sem(threading.Thread):
    mutex = threading.Semaphore(1)
    counter = 0

    def __init__(self, threadID):
        super(Sem, self).__init__()
        self.threadID = threadID

    def run(self):
        print("Thread {}".format(self.threadID))

        for i in range(MAX_COUNT/THREADS):
            Sem.mutex.acquire()
            Sem.counter += 1
            Sem.mutex.release()

def main():
    threads = []

    for i in range(THREADS):
        t = Sem(i)
        threads.append(t)

    # Start all threads
    for t in threads:
        t.start()

    # Wait for all threads to complete
    for t in threads:
        t.join()

    print("Counter value: %d Expected: %d\n" % (Sem.counter, MAX_COUNT))

if __name__ == "__main__":
    main()
