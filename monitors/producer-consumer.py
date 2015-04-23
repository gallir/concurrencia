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
        self.notFull = threading.Condition(self.mutex)
        self.notEmpty = threading.Condition(self.mutex)

    def append(self, data):
        with self.mutex:
            while len(self.buffer) == self.buffer.maxlen:
                self.notFull.wait()
            self.buffer.append(data)
            self.notEmpty.notify()

    def take(self):
        with self.mutex:
            while not self.buffer:
                self.notEmpty.wait()
            data = self.buffer.popleft()
            self.notFull.notify()
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
