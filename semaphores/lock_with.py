#! /usr/bin/env python

import threading
import time

THREADS = 4
MAX_COUNT = 10000000


class Lock(threading.Thread):
    counter = 0
    mutex = threading.Lock()
    def __init__(self, threadID):
        super(Lock, self).__init__()
        self.threadID = threadID

    def run(self):
        print("Thread {}".format(self.threadID))

        for i in range(MAX_COUNT/THREADS):
            with Lock.mutex:
                Lock.counter += 1

def main():
    threads = []

    for i in range(THREADS):
        # Create new threads
        t = Lock(i)
        threads.append(t)

    # Start all threads
    for t in threads:
        t.start()

    # Wait for all threads to complete
    for t in threads:
        t.join()

    print("Counter value: %d Expected: %d\n" % (Lock.counter, MAX_COUNT))


if __name__ == "__main__":
    main()
