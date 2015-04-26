public class CounterSynchronized implements Runnable {
    static final int THREADS = 4;
    static final int MAX_COUNT = 10000000;
    volatile static int counter = 0;
    static Object sync = new Object();
    int n, id;

    public CounterSynchronized(int id, int n) {
        this.id = id;
        this.n = n;
    }

    @Override
    public void run() {
        for (int i = 0; i < this.n; i++) {
            synchronized (sync) {
                counter += 1;
            }
        }
    }

    public static void main(String[] args) throws InterruptedException {
        Thread[] threads = new Thread[THREADS];
        int i;

        for (i=0; i< THREADS; i++) {
            threads[i] = new Thread(new CounterSynchronized(i, MAX_COUNT/THREADS));
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
    boolean lock = false;

    synchronized void lock() {
        while (lock) {
            try {
                wait();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        lock = true;
    }

    synchronized void unlock() {
        lock = false;
        notify();
    }
}
