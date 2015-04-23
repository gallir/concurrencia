#! /usr/bin/env python

import threading
import time

THREADS = 4
MAX_COUNT = 10000000

class ReaderWriter(object):
    def __init__(self):
        self.readers = 0
        self.writing = False
        self.mutex = threading.Lock()
        self.canRead = threading.Condition(self.mutex)
        self.canWrite = threading.Condition(self.mutex)


    def reader_lock(self):
        with self.mutex:
            while self.writing:
                self.canRead.wait()
            self.readers += 1
            self.canRead.notify()

    def reader_unlock(self):
        with self.mutex:
            self.readers -= 1
            if not self.readers:
                self.canWrite.notify()

    def writer_lock(self):
        with self.mutex:
            while self.writing or self.readers:
                self.canWrite.wait()
            self.writing = True

    def writer_unlock(self):
        with self.mutex:
            self.writing = False
            # we don't give priority to readers or writers
            self.canRead.notify()
            self.canWrite.notify()


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
    rw = ReaderWriter()

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
