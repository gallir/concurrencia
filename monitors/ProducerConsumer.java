import java.util.Deque;
import java.util.LinkedList;


public class ProducerConsumer {
    static final int BUFFER_SIZE = 10;
    static final int PRODUCERS = 2;
    static final int CONSUMERS = 2;
    static final int TO_CONSUME = 1000;
    static final int TO_PRODUCE = 1000;

    public static void main(String[] args) {
        Thread[] threads = new Thread[PRODUCERS+CONSUMERS];
        int t = 0, i;
        PCMonitor monitor = new PCMonitor(BUFFER_SIZE);


        System.out.printf("Counter");

        for (i = 0; i < CONSUMERS; i++) {
            threads[t] = new Thread(new Consumer(monitor, TO_CONSUME));
            threads[t].start();
            t++;
        }

        for (i = 0; i < PRODUCERS; i++) {
            threads[t] = new Thread(new Producer(monitor, TO_PRODUCE));
            threads[t].start();
            t++;
        }

        for (i = 0; i < PRODUCERS+CONSUMERS; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {}
        }
    }

}

class Producer implements Runnable {
    PCMonitor monitor;
    int operations;

    public Producer(PCMonitor mon, int ops) {
        monitor = mon;
        operations = ops;
    }

    @Override
    public void run() {
        long id = Thread.currentThread().getId();
        System.out.printf("Producer %d\n", id);

        for (int i = 0; i < operations; i++ ) {
            monitor.append(i);
            System.out.printf("%d i: %d\n", id, i);
        }

        System.out.printf("Finished producer %d\n", id);
    }

}

class Consumer implements Runnable {
    PCMonitor monitor;
    int operations;

    public Consumer(PCMonitor mon, int ops) {
        monitor = mon;
        operations = ops;
    }

    @Override
    public void run() {
        long id = Thread.currentThread().getId();
        Integer data;

        System.out.printf("Consumer %d\n", id);

        for (int i = 0; i < operations; i++ ) {
            data = monitor.take();
            System.out.printf("%d read %d\n", id, data);
        }

        System.out.printf("Finished consumer %d\n", id);
    }

}

class PCMonitor {
    int size, count = 0, in = 0, out = 0;
    Deque<Integer> buffer = new LinkedList<Integer>();

    public PCMonitor(int size) {
        this.size = size;
    }

    synchronized public int take() {
        Integer data;

        while (buffer.isEmpty()) {
            try {
                wait();
            } catch (InterruptedException e) {}
        }

        data = buffer.remove();
        notifyAll();
        return data;
    }

    synchronized public void append(Integer data) {
        while (buffer.size() == size) {
            try {
                wait();
            } catch (InterruptedException e) {}
        }
        buffer.add(data);
        notifyAll();
    }
}
