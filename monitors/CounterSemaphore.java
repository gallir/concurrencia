public class CounterSemaphores implements Runnable {
	static final int THREADS = 4;
	static final int MAX_COUNT = 10000000;
	volatile static int counter = 0;
	static Semaphore mutex = new Semaphore(1);
	int n, id;

	public CounterSemaphores(int id, int n) {
		this.id = id;
		this.n = n;
	}

	@Override
	public void run() {
		for (int i = 0; i < this.n; i++) {
			mutex.p();
			counter += 1;
			mutex.v();
		}
	}

	public static void main(String[] args) throws InterruptedException {
		Thread[] threads = new Thread[THREADS];
		int i;
		
		for (i=0; i< THREADS; i++) {
			threads[i] = new Thread(new CounterSemaphores(i, MAX_COUNT/THREADS));
		}

		for (i=0; i< THREADS; i++) {
			threads[i].start();
		}
		
		for (i=0; i< THREADS; i++) {
			threads[i].join();
		}
		
		System.out.printf("Counter value: %d Expected: %d\n", counter, MAX_COUNT);
	}
	
}

class Semaphore {
	int value;

	public Semaphore(int v) {
		value = v;
	}
	
	synchronized void p() {
		while (value == 0) {
			try {
				wait();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		value--;
	}

	synchronized void v() {
		value++;
		notify();
	}
}
