package synchronizedSortedList;

import java.util.Collections;
import java.util.List;
import java.util.concurrent.Semaphore;

public class Sort extends Thread {
    private final List<Integer> list;
    private final Semaphore readingSem;

    public Sort(List<Integer> list, Semaphore readingSem) {
        super();
        this.list = list;
        this.readingSem = readingSem;
    }

    @Override
    public void run() {
        try {
            readingSem.acquire();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        Collections.sort(list);
    }
}
