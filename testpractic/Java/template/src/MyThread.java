import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class MyThread implements Runnable {
    private int id;
    private int N;
    private int P;
    private CyclicBarrier barrier;

    public MyThread(int id, int N, int P, CyclicBarrier barrier) {
        this.id = id;
        this.N = N;
        this.P = P;
        this.barrier = barrier;
    }

    @Override
    public void run() {
        int start = (int) (id * Math.ceil((float) N / P));
        int end = Math.min((((id + 1) * (int) Math.ceil((float) N / P))), N);
        for (int i = start; i < end; i++) {
            // stuff
        }

        try {
            barrier.await();
        } catch (InterruptedException | BrokenBarrierException e) {
            e.printStackTrace();
        }
    }
}
