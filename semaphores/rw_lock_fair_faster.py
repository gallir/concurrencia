#! /usr/bin/env python

import threading
import time

THREADS = 4
MAX_COUNT = 10000000

class ReaderWriterFaster(object):
    def __init__(self):
        self.writer = threading.Lock()
        self.mx = threading.Lock()
        self.entry = threading.Lock()
        self.exit = threading.Lock()
        self.readers_in = 0
        self.readers_out = 0
        self.wait = False

    def reader_lock(self):
        self.entry.acquire()
        self.readers_in += 1
        self.entry.release()

    def reader_unlock(self):
        self.exit.acquire()
        self.readers_out += 1
        if self.wait and self.readers_in == self.readers_out:
            self.writer.release()
        self.exit.release()

    def writer_lock(self):
        self.entry.acquire()
        self.exit.acquire()
        if self.readers_in == self.readers_out:
            self.exit.release()
        else:
            self.wait = True
            self.exit.release()
            self.writer.acquire()
            self.wait = False

    def writer_unlock(self):
        self.entry.release()


counter = 0

def thread(rw):
    global counter

    print("Thread {}".format(threading.current_thread().name))

    for i in range(MAX_COUNT/THREADS):
        if i % 10:
            rw.reader_lock()
            c = counter
            rw.reader_unlock()
        else:
            rw.writer_lock()
            counter += 1
            rw.writer_unlock()

def main():
    threads = []
    rw = ReaderWriterFaster()

    for i in range(THREADS):
        t = threading.Thread(target=thread, args=(rw,))
        threads.append(t)

    # Start all threads
    for t in threads:
        t.start()

    # Wait for all threads to complete
    for t in threads:
        t.join()

    print("Counter value: %d Expected: %d\n" % (counter, int(MAX_COUNT/10)))


if __name__ == "__main__":
    main()
