package task3;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Main {
    public static int N = 4;

    public static void main(String[] args) {
        ExecutorService exc = Executors.newFixedThreadPool(8);
        exc.submit(new Task(0, new int[N], exc));
    }
}
