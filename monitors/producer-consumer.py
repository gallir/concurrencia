#! /usr/bin/env python

import threading
import time
import collections

TO_PRODUCE = 1000
BUFFER_SIZE = 10
PRODUCERS = 2
CONSUMERS = 2

buffer = collections.deque([], BUFFER_SIZE)
mutex = threading.Lock()
notFull = threading.Condition(mutex)
notEmpty = threading.Condition(mutex)


class Producer(threading.Thread):
    def append(self, data):
        with mutex:
            while len(buffer) == buffer.maxlen:
                notFull.wait()
            buffer.append(data)
            notEmpty.notify()

    def run(self):
        global counter
        myName = threading.currentThread().getName()
        print("Producer {}".format(myName))

        for i in range(TO_PRODUCE):
            data = "{} i: {}".format(myName, i)
            self.append(data)

class Consumer(threading.Thread):
    def take(self):
        with mutex:
            while not buffer:
                notEmpty.wait()
            data = buffer.popleft()
            notFull.notify()
        return data


    def run(self):
        global counter
        myName = threading.currentThread().getName()
        print("Consumer {}".format(myName))

        for i in range(TO_PRODUCE):
            data = self.take()
            print("{} read: {}".format(myName, data))

def main():
    threads = []

    for i in range(CONSUMERS):
        c = Consumer()
        threads.append(c)

    for i in range(PRODUCERS):
        p = Producer()
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
