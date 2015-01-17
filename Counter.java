public class Counter implements Runnable {
	static final int THREADS = 2;
	static final int MAX_COUNT = 10000000;
	volatile static int counter = 0;
	int n, id;

	public Counter(int id, int n) {
		this.id = id;
		this.n = n;
	}

	@Override
	public void run() {
		for (int i = 0; i < this.n; i++) {
			counter += 1;
		}
		
	}

	public static void main(String[] args) throws InterruptedException {
		Thread[] threads = new Thread[THREADS];
		int i;
		
		for (i=0; i< THREADS; i++) {
			threads[i] = new Thread(new Counter(i, MAX_COUNT/THREADS));
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

