import java.util.Random;
import java.util.concurrent.Semaphore;
import java.util.concurrent.locks.ReentrantLock;

public class Barrier implements Runnable {
    static final int THREADS = 10;
    static final int PHASES  = 20;

    static volatile int arrived = 0, counter = 0;
    static ReentrantLock mutex = new ReentrantLock();
    static Semaphore arrival = new Semaphore(1);
    static Semaphore departure = new Semaphore(0);

    void barrier(int n) {
        try {
            arrival.acquire();
        } catch (InterruptedException e1) {}
        mutex.lock();
        counter++;
        mutex.unlock();
        if (counter < n) {
            arrival.release();
        } else {
            departure.release();
        }
        try {
            departure.acquire();
        } catch (InterruptedException e) {}
        mutex.lock();
        counter--;
        mutex.unlock();
        if (counter > 0) {
            departure.release();
        } else {
            arrival.release();
        }
    }


    @Override
    public void run() {
        long id = Thread.currentThread().getId();
        Random rnd = new Random();

        System.out.printf("Thread %d\n", id);

        for (int i = 0; i < PHASES; i++) {
            try {
                Thread.sleep(rnd.nextInt(1000));
            } catch (InterruptedException e) {}

            barrier(THREADS);
            System.out.printf("%d finished phase %d\n", id, i);
        }
        barrier(THREADS);
        System.out.printf("Finished thread %d\n", id);

    }

    public static void main(String[] args) throws InterruptedException {
        Thread[] threads = new Thread[THREADS];
        int i;

        Barrier c = new Barrier();

        for (i=0; i< THREADS; i++) {
            threads[i] = new Thread(c);
            threads[i].start();
        }

        for (i=0; i< THREADS; i++) {
            threads[i].join();
        }
    }
}
