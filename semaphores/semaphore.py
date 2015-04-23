#! /usr/bin/env python

import threading
import time

THREADS = 4
MAX_COUNT = 10000000

mutex = threading.Semaphore(1)
counter = 0

def thread():
    global counter

    print("Thread {}".format(threading.current_thread().name))

    for i in range(MAX_COUNT/THREADS):
        mutex.acquire()
        counter += 1
        mutex.release()

def main():
    threads = []

    for i in range(THREADS):
        t = threading.Thread(target=thread)
        threads.append(t)

    # Start all threads
    for t in threads:
        t.start()

    # Wait for all threads to complete
    for t in threads:
        t.join()

    print("Counter value: %d Expected: %d\n" % (counter, MAX_COUNT))

if __name__ == "__main__":
    main()
