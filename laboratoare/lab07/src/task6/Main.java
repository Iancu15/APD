package task6;

import java.util.concurrent.ForkJoinPool;

public class Main {
    public static int N = 4;

    public static void main(String[] args) {
        ForkJoinPool forkJoinPool = new ForkJoinPool(8);
        forkJoinPool.invoke(new Task(0, new int[N]));
        forkJoinPool.shutdown();
    }
}