#! /usr/bin/env python

import threading
import time
import collections

TO_PRODUCE = 1000
BUFFER_SIZE = 10
PRODUCERS = 2
CONSUMERS = 2

class ProducerConsumer(object):
    def __init__(self, size):
        self.buffer = collections.deque([], size)
        self.mutex = threading.Lock()
        self.notFull = threading.Semaphore(size)
        self.notEmpty = threading.Semaphore(0)

    def append(self, data):
        self.notFull.acquire()
        with self.mutex:
            self.buffer.append(data)
        self.notEmpty.release()

    def take(self):
        self.notEmpty.acquire()
        with self.mutex:
            data = self.buffer.popleft()
        self.notFull.release()
        return data


def producer(buffer):
    id = threading.current_thread().name
    print("Producer {}".format(id))

    for i in range(TO_PRODUCE):
        data = "{} i: {}".format(id, i)
        buffer.append(data)

def consumer(buffer):
    id = threading.current_thread().name
    print("Consumer {}".format(id))

    for i in range(TO_PRODUCE):
        data = buffer.take()
        print("{} read: {}".format(id, data))


def main():
    threads = []

    buffer = ProducerConsumer(BUFFER_SIZE)
    for i in range(CONSUMERS):
        c = threading.Thread(target=consumer, args=(buffer,))
        threads.append(c)

    for i in range(PRODUCERS):
        p = threading.Thread(target=producer, args=(buffer,))
        threads.append(p)

    # Start all threads
    for t in threads:
        t.start()

    # Wait for all threads to complete
    for t in threads:
        t.join()

    print("End")


if __name__ == "__main__":
    main()
