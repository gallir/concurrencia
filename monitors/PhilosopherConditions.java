import java.util.ArrayList;
import java.util.Arrays;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class PhilosopherConditions implements Runnable {
    static final int PHILOSOPHERS = 5;
    static final int EAT_COUNT = 100;
    TableConditions table;
    int id;

    public PhilosopherConditions(int id, TableConditions table) {
        this.id = id;
        this.table = table;
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
            table.pick(id);
            eat();
            table.release(id);
        }
    }

    public static void main(String[] args) throws InterruptedException {
        Thread[] threads = new Thread[PHILOSOPHERS];
        TableConditions table = new TableConditions(PHILOSOPHERS);
        int i;
        for (i=0; i< PHILOSOPHERS; i++) {
            threads[i] = new Thread(new PhilosopherConditions(i, table));
            threads[i].start();
        }
        for (i=0; i< PHILOSOPHERS; i++) {
            threads[i].join();
        }
    }
}

class TableConditions {
    final Lock lock = new ReentrantLock();
    Integer n;
    Integer forks[];
    ArrayList<Condition> canEat = new ArrayList<Condition>();

    public TableConditions(int n) {
        this.n = n;
        forks = new Integer[n];
        Arrays.fill(forks, 2);
        for (int i = 0; i < n; i++) {
            canEat.add(lock.newCondition());
        }
    }

    int left(int i) {
        return (i + n - 1) % n;
    }

    int right(int i) {
        return (i + 1) % n;
    }

    void pick(int i) {
        lock.lock();
        try {
            while (forks[i] != 2) {
                canEat.get(i).await();
            }
            forks[left(i)]--;
            forks[right(i)]--;
        } catch (InterruptedException e) {}
        finally {
            lock.unlock();
        }
    }

    void release(int i) {
        lock.lock();
        try {
            forks[left(i)]++;
            forks[right(i)]++;
            if (forks[left(i)] == 2) {
                canEat.get(left(i)).signal();
            }
            if (forks[right(i)] == 2) {
                canEat.get(right(i)).signal();
            }
        } finally {
            lock.unlock();
        }
    }
}
