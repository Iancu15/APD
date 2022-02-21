import java.util.concurrent.CyclicBarrier;

public class Testapd {
    public static void main(String[] args) {
        int N = Integer.parseInt(args[0]);
        int P = Integer.parseInt(args[1]);

        int[] v = new int[N];
        int[] vResult = new int[N];
        for (int i = 0; i < N; i++) {
            v[i] = N - i + 5;
        }

        Thread[] threads = new Thread[P];
        CyclicBarrier barrier = new CyclicBarrier(P);
        for (int id = 0; id < P; id++) {
            threads[id] = new Thread(new MyThread(id, v, vResult, N, P, barrier));
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
