import java.util.concurrent.CyclicBarrier;

public class Testapd {
    public static long[][] initialMatrix;
    public static long[][] matrix;
    public static long[][] auxMatrix;

    public static void main(String[] args) {
        int N = Integer.parseInt(args[0]);
        int P = Integer.parseInt(args[1]);
        int K = Integer.parseInt(args[2]);

        matrix = new long[N][N];
        auxMatrix = new long[N][N];
        initialMatrix = new long[N][N];
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matrix[i][j] = 1 + (long) i * N + j;
                initialMatrix[i][j] = matrix[i][j];
            }
        }

        Thread[] threads = new Thread[P];
        CyclicBarrier barrier = new CyclicBarrier(P);
        for (int id = 0; id < P; id++) {
            threads[id] = new Thread(new MyThread(id, N, P, K, barrier));
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
