public class CounterMutex implements Runnable {
    static final int THREADS = 4;
    static final int MAX_COUNT = 10000000;
    volatile static int counter = 0;
    static Mutex mutex = new Mutex();
    int n, id;

    public CounterMutex(int id, int n) {
        this.id = id;
        this.n = n;
    }

    @Override
    public void run() {
        for (int i = 0; i < this.n; i++) {
            mutex.lock();
            counter += 1;
            mutex.unlock();
        }
    }

    public static void main(String[] args) throws InterruptedException {
        Thread[] threads = new Thread[THREADS];
        int i;

        for (i=0; i< THREADS; i++) {
            threads[i] = new Thread(new CounterMutex(i, MAX_COUNT/THREADS));
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

class Mutex {
    int lock = 0;

    synchronized void lock() {
        while (lock != 0) {
            try {
                wait();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        lock = 1;
    }

    synchronized void unlock() {
        lock = 0;
        notify();
    }
}
