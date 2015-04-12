#! /usr/bin/env python

import threading
import time

THREADS = 4
MAX_COUNT = 10000000



class RW(threading.Thread):
    writer = threading.Lock()
    entry = threading.Lock()
    exit = threading.Lock()

    readers_in = 0
    readers_out = 0
    wait = False

    counter = 0
    def __init__(self, threadID):
        super(RW, self).__init__()
        self.threadID = threadID

    def reader_lock(self):
        RW.entry.acquire()
        RW.readers_in += 1
        RW.entry.release()

    def reader_unlock(self):
        RW.exit.acquire()
        RW.readers_out += 1
        if RW.wait and RW.readers_in == RW.readers_out:
            RW.writer.release()
        RW.exit.release()

    def writer_lock(self):
        RW.entry.acquire()
        RW.exit.acquire()
        if RW.readers_in == RW.readers_out:
            RW.exit.release()
        else:
            RW.wait = True
            RW.exit.release()
            RW.writer.acquire()
            RW.wait = False

    def writer_unlock(self):
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
