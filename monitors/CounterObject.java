public class CounterObject implements Runnable {
    static final int THREADS = 4;
    static final int MAX_COUNT = 10000000;

    public volatile int counter = 0;
    Object lock = new Object();


    @Override
    public void run() {
        int max = MAX_COUNT/THREADS;
        System.out.printf("Thread %d\n", Thread.currentThread().getId());
        for (int i =0; i < max; i++) {
            synchronized (lock) {
                counter += 1;
            }
        }
    }

    public static void main(String[] args) throws InterruptedException {
        Thread[] threads = new Thread[THREADS];
        int i;

        CounterObject c = new CounterObject();

        for (i=0; i< THREADS; i++) {
            threads[i] = new Thread(c);
            threads[i].start();
        }

        for (i=0; i< THREADS; i++) {
            threads[i].join();
        }
        System.out.printf("Counter value: %d Expected: %d\n", c.counter, MAX_COUNT);
    }
}
