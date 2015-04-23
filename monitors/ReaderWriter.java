
public class ReaderWriter implements Runnable{
    static final int THREADS = 4;
    static final int MAX_COUNT = 10000000;
    static volatile int counter = 0;
    RWMonitor monitor;
    int id;

    public ReaderWriter(int id, RWMonitor monitor) {
        this.id = id;
        this.monitor = monitor;
    }

    @Override
    public void run() {
        int max = MAX_COUNT/THREADS;
        int c;

        System.out.printf("Thread %d\n", id);
        for (int i = 0; i < max; i++) {
            if (i % 10 == 0) {
                monitor.writerLock();
                counter += 1;
                monitor.writerUnlock();
            } else {
                monitor.readerLock();
                c = counter;
                monitor.readerUnlock();
            }
        }
    }

    public static void main(String[] args) throws InterruptedException {
        Thread[] threads = new Thread[THREADS];
        RWMonitor monitor = new RWMonitor();

        int i;

        for (i=0; i< THREADS; i++) {
            threads[i] = new Thread(new ReaderWriter(i, monitor));
            threads[i].start();
        }

        for (i=0; i< THREADS; i++) {
            threads[i].join();
        }
        System.out.printf("Counter value: %d Expected: %d\n", counter, MAX_COUNT/10);
    }
}

class RWMonitor {
    volatile int readers = 0;
    volatile boolean writing = false;

    synchronized void readerLock() {
        while (writing) {
            try {
                wait();
            } catch (InterruptedException e) {}
        }
        readers++;
        notifyAll();
    }

    synchronized void readerUnlock() {
        readers--;
        if (readers == 0) {
            notifyAll();
        }
    }

    synchronized void writerLock() {
        while (writing || readers != 0) {
            try {
                wait();
            } catch (InterruptedException e) {}
        }
        writing = true;
    }

    synchronized void writerUnlock() {
        writing = false;
        notifyAll();

    }
}
