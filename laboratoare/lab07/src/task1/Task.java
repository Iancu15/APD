package task1;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class Task implements Runnable {
    List<Integer> partialPath;
    int destination;
    ExecutorService tpe;
    AtomicInteger submitted;

    public Task(List<Integer> partialPath, int destination, ExecutorService tpe) {
        this.partialPath = partialPath;
        this.destination = destination;
        this.tpe = tpe;
        this.submitted = new AtomicInteger(1);
    }

    public Task(List<Integer> partialPath, int destination, ExecutorService tpe, AtomicInteger submitted) {
        this.partialPath = partialPath;
        this.destination = destination;
        this.tpe = tpe;
        submitted.incrementAndGet();
        this.submitted = submitted;
    }

    @Override
    public void run() {
        if (partialPath.get(partialPath.size() - 1) == destination) {
            System.out.println(partialPath);

            if (submitted.decrementAndGet() == 0) {
                tpe.shutdown();
            }

            return;
        }

        // se verifica nodurile pentru a evita ciclarea in graf
        int lastNodeInPath = partialPath.get(partialPath.size() - 1);
        for (int[] ints : Main.graph) {
            if (ints[0] == lastNodeInPath) {
                if (partialPath.contains(ints[1]))
                    continue;
                ArrayList<Integer> newPartialPath = new ArrayList<>(partialPath);
                newPartialPath.add(ints[1]);
                tpe.submit(new Task(newPartialPath, destination, tpe, submitted));
            }
        }

        if (submitted.decrementAndGet() == 0) {
            tpe.shutdown();
        }
    }
}
