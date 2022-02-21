import java.util.Random;
import java.util.concurrent.CyclicBarrier;

public class Testapd {
    /**
     * 0 - piatra
     * 1 - foarfeca
     * 2 - hartie
     */
    public static int[] draws;
    public static boolean[] standingThreads;
    public static Random rand;
    public static CyclicBarrier barrier;

    public static void main(String[] args) {
        int P = 2 * Integer.parseInt(args[0]);

        draws = new int[P];
        standingThreads = new boolean[P];
        rand = new Random();
        for (int i = 0; i < P; i++) {
            standingThreads[i] = true;
        }

        Thread[] threads = new Thread[P];
        barrier = new CyclicBarrier(P);
        for (int id = 0; id < P; id++) {
            threads[id] = new Thread(new MyThread(id, P));
            threads[id].start();
        }

        for (int i = 0; i < P; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
