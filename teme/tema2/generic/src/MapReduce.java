import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.*;
import java.util.stream.Collectors;

public abstract class MapReduce<IO, O> {
    private final Integer numberOfWorkers;

    public MapReduce(Integer numberOfWorkers) {
        this.numberOfWorkers = numberOfWorkers;
    }

    public abstract List<Callable<IO>> createMapTasks();

    public abstract List<Callable<O>> createReduceTasks(List<Future<IO>> intermediateOs) throws ExecutionException, InterruptedException;

    public List<O> execute() throws InterruptedException, ExecutionException {
        ExecutorService executorService = Executors.newFixedThreadPool(this.numberOfWorkers);
        List<Callable<IO>> mapTasks = createMapTasks();

        List<Future<IO>> intermediateOs = executorService.invokeAll(mapTasks);

        List<Callable<O>> reduceTasks = createReduceTasks(intermediateOs);

        // map pentru a transforma List<Future<O>> in List<O>
        List<O> outputs = executorService.invokeAll(reduceTasks).stream().map(x -> {
            try {
                return x.get();
            } catch (InterruptedException | ExecutionException e) {
                e.printStackTrace();
            }

            return null;
        }).collect(Collectors.toList());

        executorService.shutdown();
        return outputs;
    }
}
