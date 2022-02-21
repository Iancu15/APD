import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;

public class MapReduceImpl extends MapReduce<IntermediateOutput, Output> {
    private final Integer fragmentSize;
    private final List<String> fileList;

    public MapReduceImpl(Integer numberOfWorkers, Integer fragmentSize, List<String> fileList) {
        super(numberOfWorkers);
        this.fragmentSize = fragmentSize;
        this.fileList = fileList;
    }

    public List<Callable<IntermediateOutput>> createMapTasks() {
        List<Callable<IntermediateOutput>> mapTasks = new ArrayList<>();
        for (String file : fileList) {
            File f = new File(file);
            int offset;
            for (offset = 0; offset < f.length(); offset += fragmentSize) {
                mapTasks.add(new TaskMap(file, offset, Math.min(fragmentSize, (int) f.length() - offset)));
            }
        }

        return mapTasks;
    }

    public List<Callable<Output>> createReduceTasks(List<Future<IntermediateOutput>> intermediateOutputs) throws ExecutionException, InterruptedException {
        List<Callable<Output>> reduceTasks = new ArrayList<>();
        String file = null;
        List<Map<Integer, Integer>> partialMaps = null;
        List<List<String>> maxLengthWordsLists = null;
        for (Future<IntermediateOutput> intermediateOutput : intermediateOutputs) {
            IntermediateOutput intermediateOutputResult = intermediateOutput.get();

            // daca urmatorul output intermediar e de la alt fisier inseamna ca s-a trecut la procesarea
            // urmatorului fisier si se creeaza partialMaps si maxLengthWordLists pentru el
            if (!intermediateOutputResult.getFile().equals(file)) {
                if (file != null) {
                    reduceTasks.add(new TaskReduce(file, partialMaps, maxLengthWordsLists));
                }

                partialMaps = new ArrayList<>();
                maxLengthWordsLists = new ArrayList<>();
                file = intermediateOutputResult.getFile();
            }

            // adaug outputurile intermediare pentru fisierul curent
            partialMaps.add(intermediateOutputResult.getPartialMap());
            maxLengthWordsLists.add(intermediateOutputResult.getMaxLengthWordsList());
        }

        reduceTasks.add(new TaskReduce(file, partialMaps, maxLengthWordsLists));

        return reduceTasks;
    }
}
