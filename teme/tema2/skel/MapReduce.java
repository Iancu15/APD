import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.stream.Collectors;

public class MapReduce {
    private final Integer numberOfWorkers;
    private final Integer fragmentSize;
    private final List<String> fileList;
    private final List<Integer> numberOfMaxLengthWordsList;
    private final List<Integer> rankList;

    public MapReduce(Integer numberOfWorkers, Integer fragmentSize, List<String> fileList) {
        this.numberOfWorkers = numberOfWorkers;
        this.fragmentSize = fragmentSize;
        this.fileList = fileList;
        this.numberOfMaxLengthWordsList = new ArrayList<>();
        this.rankList = new ArrayList<>();
    }

    public List<Output> execute() throws InterruptedException, ExecutionException {
        ExecutorService executorService = Executors.newFixedThreadPool(this.numberOfWorkers);
        List<TaskMap> mapTasks = new ArrayList<>();
        for (String file : this.fileList) {
            File f = new File(file);
            int offset;
            for (offset = 0; offset < f.length(); offset += this.fragmentSize) {
                mapTasks.add(new TaskMap(file, offset, Math.min(fragmentSize, (int) f.length() - offset)));
            }
        }

        List<Future<IntermediateOutput>> intermediateOutputs = null;
        intermediateOutputs = executorService.invokeAll(mapTasks);

        List<TaskReduce> reduceTasks = new ArrayList<>();
        String file = null;
        List<Map<Integer, Integer>> partialMaps = null;
        List<List<String>> maxLengthWordsLists = null;
        for (Future<IntermediateOutput> intermediateOutput : intermediateOutputs) {
            IntermediateOutput intermediateOutputResult = intermediateOutput.get();
            if (!intermediateOutputResult.getFile().equals(file)) {
                if (file != null) {
                    reduceTasks.add(new TaskReduce(file, partialMaps, maxLengthWordsLists));
                }

                partialMaps = new ArrayList<>();
                maxLengthWordsLists = new ArrayList<>();
                file = intermediateOutputResult.getFile();
            }

            partialMaps.add(intermediateOutputResult.getPartialMap());
            maxLengthWordsLists.add(intermediateOutputResult.getMaxLengthWordsList());
        }

        reduceTasks.add(new TaskReduce(file, partialMaps, maxLengthWordsLists));

        List<Output> outputs = executorService.invokeAll(reduceTasks).stream().map(x -> {
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
