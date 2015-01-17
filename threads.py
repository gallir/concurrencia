import threading
import time

THREADS = 2

class myThread(threading.Thread):
	def __init__(self, threadID):
		threading.Thread.__init__(self)
		self.threadID = threadID

	def run(self):
		print("Hi, I'm thread {}".format(self.threadID))

threads = []

for i in range(THREADS):
	# Create new threads
	t = myThread(i)
	threads.append(t)
	t.start() # start the thread

# Wait for all threads to complete
for t in threads:
	t.join()


