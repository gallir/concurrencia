#! /usr/bin/env python

import threading
import time

TO_PRODUCE = 1000
BUFFER_SIZE = 10
PRODUCERS = 2
CONSUMERS = 2

buffer = []
mutex = threading.RLock()
notFull = threading.Semaphore(BUFFER_SIZE)
notEmpty = threading.Semaphore(0)


class Producer(threading.Thread):
    def run(self):
        global counter
        myName = threading.currentThread().getName()
        print("Producer {}".format(myName))

        for i in range(TO_PRODUCE):
            data = "{} i: {}".format(myName, i)
            notFull.acquire()
            with mutex:
                buffer.append(data)
            notEmpty.release()

class Consumer(threading.Thread):
    def run(self):
        global counter
        myName = threading.currentThread().getName()
        print("Consumer {}".format(myName))

        for i in range(TO_PRODUCE):
            notEmpty.acquire()
            with mutex:
                data = buffer.pop(0)
                print("{} read: {}".format(myName, data))
            notFull.release()

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
