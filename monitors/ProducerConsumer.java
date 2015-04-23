import java.util.Deque;
import java.util.LinkedList;


public class ProducerConsumer {
    static final int PRODUCERS = 2;
    static final int CONSUMERS = 2;


    public static void main(String[] args) {
        Thread[] threads = new Thread[PRODUCERS+CONSUMERS];
        int t = 0, i;
        PCMonitor monitor = new PCMonitor();


        System.out.printf("Counter");

        for (i = 0; i < CONSUMERS; i++) {
            threads[t] = new Thread(new Consumer(monitor));
            threads[t].start();
            t++;
        }

        for (i = 0; i < PRODUCERS; i++) {
            threads[t] = new Thread(new Producer(monitor));
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
    static final int TO_PRODUCE = 1000;
    PCMonitor monitor;

    public Producer(PCMonitor monitor) {
        this.monitor = monitor;
    }

    @Override
    public void run() {
        long id = Thread.currentThread().getId();
        System.out.printf("Producer %d\n", id);

        for (int i = 0; i < TO_PRODUCE; i++ ) {
            monitor.append(i);
            System.out.printf("%d i: %d\n", id, i);
        }

        System.out.printf("Finished producer %d\n", id);
    }

}

class Consumer implements Runnable {
    static final int TO_CONSUME = 1000;

    PCMonitor monitor;
    public Consumer(PCMonitor monitor) {
        this.monitor = monitor;
    }

    @Override
    public void run() {
        long id = Thread.currentThread().getId();
        Integer data;

        System.out.printf("Consumer %d\n", id);

        for (int i = 0; i < TO_CONSUME; i++ ) {
            data = monitor.take();
            System.out.printf("%d read %d\n", id, data);
        }

        System.out.printf("Finished consumer %d\n", id);
    }

}

class PCMonitor {
    static final int BUFFER_SIZE = 10;

    int count = 0, in = 0, out = 0;
    Deque<Integer> buffer = new LinkedList<Integer>();

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
        while (buffer.size() == BUFFER_SIZE) {
            try {
                wait();
            } catch (InterruptedException e) {}
        }
        buffer.add(data);
        notifyAll();
    }
}
