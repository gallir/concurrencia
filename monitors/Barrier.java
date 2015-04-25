import java.util.Random;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class Barrier implements Runnable {
    static final int THREADS = 10;
    static final int PHASES  = 20;

    static volatile int arrived = 0;

    synchronized void barrier(int n) {
        arrived++;
        if (arrived == n) {
            arrived = 0;
            notifyAll();
        } else {
            try {
                wait();
            } catch (InterruptedException e) {}
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
