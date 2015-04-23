#! /usr/bin/env python

import threading
import time

TO_PRODUCE = 1000
PRODUCERS = 2
CONSUMERS = 2

buffer = []
mutex = threading.Lock()
notEmpty = threading.Semaphore(0)


def producer():
    id = threading.current_thread().name
    print("Producer {}".format(id))

    for i in range(TO_PRODUCE):
        data = "{} i: {}".format(id, i)
        with mutex:
            buffer.append(data)
        notEmpty.release()


def consumer():
    id = threading.current_thread().name
    print("Consumer {}".format(id))

    for i in range(TO_PRODUCE):
        notEmpty.acquire()
        with mutex:
            data = buffer.pop(0)
            print("{} read: {}".format(id, data))

def main():
    threads = []

    for i in range(CONSUMERS):
        c = threading.Thread(target=consumer)
        threads.append(c)

    for i in range(PRODUCERS):
        p = threading.Thread(target=producer)
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
