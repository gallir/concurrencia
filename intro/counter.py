#! /usr/bin/env python

import threading

THREADS = 2
MAX_COUNT = 10000000

counter = 0

def thread():
    global counter

    print("Thread {}".format(threading.current_thread().name))

    for i in range(MAX_COUNT/THREADS):
        counter += 1

def main():
    threads = []

    for i in range(THREADS):
        # Create new threads
        t = threading.Thread(target=thread)
        threads.append(t)
        t.start() # start the thread

    # Wait for all threads to complete
    for t in threads:
        t.join()

    print("Counter value: {} Expected: {}\n".format(counter, MAX_COUNT))

if __name__ == "__main__":
    main()
