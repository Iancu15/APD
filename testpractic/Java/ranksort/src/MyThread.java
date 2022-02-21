import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class MyThread implements Runnable {
    private int id;
    private int N;
    private int[] v;
    private int[] vResult;
    private int P;
    private CyclicBarrier barrier;

    public MyThread(int id, int[] v, int[] vResult, int N, int P, CyclicBarrier barrier) {
        this.id = id;
        this.v = v;
        this.vResult = vResult;
        this.N = N;
        this.P = P;
        this.barrier = barrier;
    }

    @Override
    public void run() {
        int start = (int) (id * Math.ceil((float) N / P));
        int end = Math.min((((id + 1) * (int) Math.ceil((float) N / P))), N);
        for (int i = start; i < end; i++) {
            int numberOfSmallerElems = 0;
            for (int j = 0; j < N; j++) {
                if (v[j] < v[i]) {
                    numberOfSmallerElems++;
                }
            }

            vResult[numberOfSmallerElems] = v[i];
        }

        try {
            barrier.await();
        } catch (InterruptedException | BrokenBarrierException e) {
            e.printStackTrace();
        }

        if (id == P - 1) {
            for (int i = 0; i < N; i++) {
                System.out.print(vResult[i] + " ");
            }

            System.out.print("\n");
        }
    }
}
