package task4;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveTask;

public class Task extends RecursiveTask<Void> {
    int destination;
    List<Integer> partialPath;

    public Task(int destination, List<Integer> partialPath) {
        this.destination = destination;
        this.partialPath = partialPath;
    }

    @Override
    protected Void compute() {
        if (partialPath.get(partialPath.size() - 1) == destination) {
            System.out.println(partialPath);
            return null;
        }

        // se verifica nodurile pentru a evita ciclarea in graf
        int lastNodeInPath = partialPath.get(partialPath.size() - 1);
        List<Task> tasks = new ArrayList<>();
        for (int[] ints : Main.graph) {
            if (ints[0] == lastNodeInPath) {
                if (partialPath.contains(ints[1]))
                    continue;
                ArrayList<Integer> newPartialPath = new ArrayList<>(partialPath);
                newPartialPath.add(ints[1]);
                Task task = new Task(destination, newPartialPath);
                tasks.add(task);
                task.fork();
            }
        }

        tasks.forEach(Task::join);
        return null;
    }
}
