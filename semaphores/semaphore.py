#! /usr/bin/env python

import threading
import time

THREADS = 4
MAX_COUNT = 10000000

counter = 0
mutex = threading.Semaphore(1)

class MyThread(threading.Thread):
    def __init__(self, threadID):
        super(MyThread, self).__init__()
        self.threadID = threadID

    def run(self):
        global counter
        print("Thread {}".format(self.threadID))

        for i in range(MAX_COUNT/THREADS):
            mutex.acquire()
            counter += 1
            mutex.release()
        

# Create new threads
threads = [MyThread(i) for i in range(THREADS)]

# Start threads
for t in threads: t.start()

# Wait for all threads to complete
for t in threads: t.join()

print("Counter value: %d Expected: %d\n" % (counter, MAX_COUNT))
