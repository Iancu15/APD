package task2;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class Task implements Runnable {
    int step;
    int[] colors;
    ExecutorService executorService;
    AtomicInteger submitted;

    public Task(int step, int[] colors, ExecutorService executorService) {
        this.step = step;
        this.colors = colors;
        this.submitted = new AtomicInteger(1);
        this.executorService = executorService;
    }

    public Task(int step, int[] colors, ExecutorService executorService, AtomicInteger submitted) {
        this.step = step;
        this.colors = colors;
        this.executorService = executorService;
        this.submitted = submitted;
        this.submitted.incrementAndGet();
    }

    private boolean verifyColors(int[] colors, int step) {
        for (int i = 0; i < step; i++) {
            if (colors[i] == colors[step] && isEdge(i, step))
                return false;
        }
        return true;
    }

    private boolean isEdge(int a, int b) {
        for (int[] ints : Main.graph) {
            if (ints[0] == a && ints[1] == b)
                return true;
        }
        return false;
    }

    void printColors(int[] colors) {
        StringBuilder aux = new StringBuilder();
        for (int color : colors) {
            aux.append(color).append(" ");
        }
        System.out.println(aux);
    }

    @Override
    public void run() {
        if (step == Main.N) {
            printColors(colors);

            if (submitted.decrementAndGet() == 0) {
                executorService.shutdown();
            }

            return;
        }

        // for the node at position step try all possible colors
        for (int i = 0; i < Main.COLORS; i++) {
            int[] newColors = colors.clone();
            newColors[step] = i;
            if (verifyColors(newColors, step)) {
                executorService.submit(new Task(step + 1, newColors, executorService, submitted));
            }
        }

        if (submitted.decrementAndGet() == 0) {
            executorService.shutdown();
        }
    }
}
