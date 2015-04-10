#! /usr/bin/env python

import threading
import time

THREADS = 4
MAX_COUNT = 10000000



class MyThread(threading.Thread):
    mutex = threading.Lock()
    counter = 0
    def __init__(self, threadID):
        super(MyThread, self).__init__()
        self.threadID = threadID

    def run(self):
        global counter
        print("Thread {}".format(self.threadID))

        for i in range(MAX_COUNT/THREADS):
            MyThread.mutex.acquire()
            MyThread.counter += 1
            MyThread.mutex.release()

def main():
    threads = []

    for i in range(THREADS):
        t = MyThread(i)
        threads.append(t)

    # Start all threads
    for t in threads:
        t.start()

    # Wait for all threads to complete
    for t in threads:
        t.join()

    print("Counter value: %d Expected: %d\n" % (MyThread.counter, MAX_COUNT))


if __name__ == "__main__":
    main()
