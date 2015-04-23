#! /usr/bin/env python

import threading
import time

THREADS = 4
MAX_COUNT = 10000000

class ReaderWriterFair(object):
    def __init__(self):
        self.writer = threading.Lock()
        self.mx = threading.Lock()
        self.entry = threading.Lock()
        self.readers = 0

    def reader_lock(self):
        self.entry.acquire()
        self.mx.acquire()
        self.readers += 1
        if self.readers == 1:
            self.writer.acquire()
        self.mx.release()
        self.entry.release()

    def reader_unlock(self):
        self.mx.acquire()
        self.readers -= 1
        if self.readers == 0:
            self.writer.release()
        self.mx.release()

    def writer_lock(self):
        self.entry.acquire()
        self.writer.acquire()

    def writer_unlock(self):
        self.writer.release()
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
    rw = ReaderWriterFair()

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
