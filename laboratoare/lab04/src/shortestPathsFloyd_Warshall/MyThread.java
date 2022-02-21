package shortestPathsFloyd_Warshall;

public class MyThread implements Runnable {
    public int id;
    public int[][] graph;

    public MyThread(int id, int[][] graph) {
        this.id = id;
        this.graph = graph;
    }

    @Override
    public void run() {
        int k = id;
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                graph[i][j] = Math.min(graph[i][k] + graph[k][j], graph[i][j]);
            }
        }
    }
}