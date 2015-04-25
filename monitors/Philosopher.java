import java.util.Arrays;

public class Philosopher implements Runnable {
    static final int PHILOSOPHERS = 5;
    static final int EAT_COUNT = 100;
    Table table;
    int id, leftFork, rightFork;

    public Philosopher(int id, Table table) {
        this.id = id;
        this.table = table;
        leftFork = id;
        rightFork = (id + 1) % PHILOSOPHERS;
    }

    void think() {
        try {
            Thread.sleep(50);
        } catch (InterruptedException e) {}
    }

    void eat() {
        System.out.printf("%d start eat\n", id);
        try {
            Thread.sleep(100);
        } catch (InterruptedException e) {}
        System.out.printf("%d end eat\n", id);

    }

    @Override
    public void run() {
        for (int i = 0; i < EAT_COUNT; i++) {
            think();
            table.pick(leftFork, rightFork);
            eat();
            table.release(leftFork, rightFork);
        }

    }

    public static void main(String[] args) throws InterruptedException {
        Thread[] threads = new Thread[PHILOSOPHERS];
        Table table = new Table(PHILOSOPHERS);
        int i;

        for (i=0; i< PHILOSOPHERS; i++) {
            threads[i] = new Thread(new Philosopher(i, table));
            threads[i].start();
        }

        for (i=0; i< PHILOSOPHERS; i++) {
            threads[i].join();
        }
    }
}

class Table {
    boolean forks[];

    public Table(int forks) {
        this.forks = new boolean[forks];
        Arrays.fill(this.forks, true);
    }

    synchronized void pick(int left, int right) {
        while (! forks[left] || ! forks[right]) {
            try {
                wait();
            } catch (InterruptedException e) {}
        }
        forks[left] = false;
        forks[right] = false;
    }

    synchronized void release(int left, int right) {
        forks[left] = true;
        forks[right] = true;
        notifyAll();
    }
}
