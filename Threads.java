public class Threads implements Runnable {
	static final int THREADS = 2;
	int id;

	public Threads(int id) {
		this.id = id;
	}

	@Override
	public void run() {
		System.out.printf("Hi, I'm thread %d\n", id);
	}

	public static void main(String[] args) throws InterruptedException {
		Thread[] threads = new Thread[THREADS];
		int i;
		
		for (i=0; i< THREADS; i++) {
			threads[i] = new Thread(new Threads(i));
			threads[i].start();
		}

		for (i=0; i< THREADS; i++) {
			threads[i].join();
		}
		
	}
}

