package doubleVectorElements;

public class Main {

    public static void main(String[] args) {
        int N = 100000013;
        int[] v = new int[N];
        int NUMBER_OF_THREADS = 4;
        Thread[] t = new Thread[NUMBER_OF_THREADS];

        for (int i = 0; i < N; i++) {
            v[i] = i;
        }

        for (int tid = 0; tid < NUMBER_OF_THREADS; ++tid) {
            t[tid] = new Thread(new MyThread(tid, v, N, NUMBER_OF_THREADS));
            t[tid].start();
        }

        for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
            try {
                t[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        for (int i = 0; i < N; i++) {
            if (v[i] != i * 2) {
                System.out.println("Wrong answer");
                System.exit(1);
            }
        }
        System.out.println("Correct");
    }

}
