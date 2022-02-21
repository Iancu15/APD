package doubleVectorElements;

import bug1.Main;

public class MyThread implements Runnable {
    public int id;
    public int N;
    public int[] v;
    public int P;

    public MyThread(int id, int[] v, int N, int P) {
        this.id = id;
        this.v = v;
        this.N = N;
        this.P = P;
    }

    @Override
    public void run() {
        int start = (int) (id * Math.ceil(N / P));
        int end = Math.min((((id + 1) * (int) Math.ceil(N / P))), N);
        if (id == P - 1 && end < N) {
            end = N;
        }

        for (int i = start; i < end; ++i) {
            v[i] *= 2;
        }
    }
}
