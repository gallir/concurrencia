#! /usr/bin/env python

import threading
import time

THREADS = 4
MAX_COUNT = 10000000



class RW(threading.Thread):
    writer = threading.Lock()
    mx = threading.Lock()
    entry = threading.Lock()
    readers = 0

    counter = 0
    def __init__(self, threadID):
        super(RW, self).__init__()
        self.threadID = threadID

    def reader_lock(self):
        RW.entry.acquire()
        RW.mx.acquire()
        RW.readers += 1
        if RW.readers == 1:
            RW.writer.acquire()
        RW.mx.release()
        RW.entry.release()

    def reader_unlock(self):
        RW.mx.acquire()
        RW.readers -= 1
        if RW.readers == 0:
            RW.writer.release()
        RW.mx.release()

    def writer_lock(self):
        RW.entry.acquire()
        RW.writer.acquire()

    def writer_unlock(self):
        RW.writer.release()
        RW.entry.release()

    def run(self):
        print("Thread {}".format(self.threadID))

        for i in range(MAX_COUNT/THREADS):
            if i % 10:
                self.reader_lock()
                c = RW.counter
                self.reader_unlock()
            else:
                self.writer_lock()
                RW.counter += 1
                self.writer_unlock()

def main():
    threads = []

    for i in range(THREADS):
        t = RW(i)
        threads.append(t)

    # Start all threads
    for t in threads:
        t.start()

    # Wait for all threads to complete
    for t in threads:
        t.join()

    print("Counter value: %d Expected: %d\n" % (RW.counter, int(MAX_COUNT/10)))


if __name__ == "__main__":
    main()
