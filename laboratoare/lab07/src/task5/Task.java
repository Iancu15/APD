package task5;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveTask;

public class Task extends RecursiveTask<Void> {
    int[] colors;
    int step;
    ForkJoinPool fjp;

    public Task(int[] colors, int step, ForkJoinPool fjp) {
        this.colors = colors;
        this.step = step;
        this.fjp = fjp;
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
    protected Void compute() {
        if (step == Main.N) {
            printColors(colors);
            return null;
        }

        List<Task> tasks = new ArrayList<>();

        // for the node at position step try all possible colors
        for (int i = 0; i < Main.COLORS; i++) {
            int[] newColors = colors.clone();
            newColors[step] = i;
            if (verifyColors(newColors, step)) {
                Task task = new Task(newColors, step + 1, fjp);
                tasks.add(task);
                task.fork();
            }
        }

        tasks.forEach(Task::join);

        return null;
    }
}
