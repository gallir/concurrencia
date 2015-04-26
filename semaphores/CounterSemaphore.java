import java.util.concurrent.Semaphore;

public class CounterSemaphore implements Runnable {
    static final int THREADS = 4;
    static final int MAX_COUNT = 10000000;
    static volatile int counter = 0;
    static Semaphore mutex = new Semaphore(1);
    int id;

    public CounterSemaphore(int id) {
        this.id = id;
    }

    @Override
    public void run() {
        int max = MAX_COUNT/THREADS;
        System.out.printf("Thread %d\n", id);
        for (int i = 0; i < max; i++) {
            try {
                mutex.acquire();
            } catch (InterruptedException e) {}
            counter += 1;
            mutex.release();
        }
    }

    public static void main(String[] args) throws InterruptedException {
        Thread[] threads = new Thread[THREADS];
        int i;

        for (i=0; i< THREADS; i++) {
            threads[i] = new Thread(new CounterSemaphore(i));
            threads[i].start();
        }

        for (i=0; i< THREADS; i++) {
            threads[i].join();
        }
        System.out.printf("Counter value: %d Expected: %d\n", counter, MAX_COUNT);
    }
}
