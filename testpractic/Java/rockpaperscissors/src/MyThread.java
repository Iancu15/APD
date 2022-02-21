import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class MyThread implements Runnable {
    private int id;
    private int P;

    public MyThread(int id, int P) {
        this.id = id;
        this.P = P;
    }

    @Override
    public void run() {
        int numberOfRounds = 0;
        for (int i = P; i > 1; i /= 2) {
            int currentId = id % i;
            Testapd.draws[currentId] = Testapd.rand.nextInt(3);
            numberOfRounds++;
            try {
                Testapd.barrier.await();
            } catch (InterruptedException | BrokenBarrierException e) {
                e.printStackTrace();
            }

            int enemyId = currentId + 1;
            boolean idIsSmaller = true;
            if (currentId % 2 == 1) {
                enemyId = currentId - 1;
                idIsSmaller = false;
            }

            int enemyDraw = Testapd.draws[enemyId];
            switch (Testapd.draws[currentId]) {
                case 0:
                    if (enemyDraw == 2 || (enemyDraw == 0 && idIsSmaller)) {
                        printRound(enemyId, i);
                        Testapd.standingThreads[id] = false;
                        return;
                    }

                    break;

                case 1:
                    if (enemyDraw == 0 || (enemyDraw == 1 && idIsSmaller)) {
                        printRound(enemyId, i);
                        Testapd.standingThreads[id] = false;
                        return;
                    }

                    break;

                case 2:
                    if (enemyDraw == 1 || (enemyDraw == 2 && idIsSmaller)) {
                        printRound(enemyId, i);
                        Testapd.standingThreads[id] = false;
                        return;
                    }

                    break;
            }

            synchronized (MyThread.class) {
                if (currentId != 0 && currentId != 1) {
                    try {
                        Testapd.standingThreads.wait();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                } else {
                    Testapd.barrier = new CyclicBarrier(i / 2);
                    Testapd.standingThreads.notifyAll();
                }
            }

            try {
                Testapd.barrier.await();
            } catch (InterruptedException | BrokenBarrierException e) {
                e.printStackTrace();
            }
        }

        System.out.println("Threadul " + id + " a castigat turneul dupa " + numberOfRounds + " runde");
    }

    private void printRound(int enemyId, int round) {
        for (int i = enemyId; i < P; i += (P / round) * 2) {
            if (Testapd.standingThreads[i]) {
                System.out.println("Threadul " + id + " care a ales " + drawAsString(Testapd.draws[id]) + " se bate cu threadul " +
                        i + " care a ales " + drawAsString(Testapd.draws[i]));
                break;
            }
        }
    }

    private String drawAsString(int draw) {
        switch (draw) {
            case 0:
                return "PIATRA";

            case 1:
                return "FOARFECA";

            case 2:
                return "HARTIE";
        }

        return "";
    }
}
