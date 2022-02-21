import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class MyThread implements Runnable {
    private int id;
    private int N;
    private int P;
    private int K;
    private CyclicBarrier barrier;

    public MyThread(int id, int N, int P, int K, CyclicBarrier barrier) {
        this.id = id;
        this.N = N;
        this.P = P;
        this.K = K;
        this.barrier = barrier;
    }

    @Override
    public void run() {
        int start = (int) (id * Math.ceil((float) N / P));
        int end = Math.min((((id + 1) * (int) Math.ceil((float) N / P))), N);
        for (int iter = 0; iter < K - 1; iter++) {
            for (int i = start; i < end; i++) {
                for (int j = 0; j < N; j++) {
                    for (int k = 0; k < N; k++) {
                        Testapd.auxMatrix[i][j] += Testapd.matrix[i][k] * Testapd.initialMatrix[k][j];
                    }
                }
            }

            try {
                barrier.await();
            } catch (InterruptedException | BrokenBarrierException e) {
                e.printStackTrace();
            }

            if (id == P - 1) {
                Testapd.matrix = Testapd.auxMatrix;
                Testapd.auxMatrix = new long[N][N];
            }

            try {
                barrier.await();
            } catch (InterruptedException | BrokenBarrierException e) {
                e.printStackTrace();
            }
        }

        if (id == P - 1) {
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    System.out.print(Testapd.matrix[i][j] + " ");
                }

                System.out.println();
            }
        }
    }
}
