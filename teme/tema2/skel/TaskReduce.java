import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.Callable;

public class TaskReduce implements Callable<Output> {
    private final String file;
    private final List<Map<Integer, Integer>> partialMaps;
    private final List<List<String>> maxLengthWordsLists;
    private final Map<Integer, Integer> fileMap;
    private final List<String> fileMaxLengthWordList;
    private Integer fileLengthOfBiggestWord;
    private Integer numberOfMaxLengthWords;
    private Float rank;

    public TaskReduce(String file, List<Map<Integer, Integer>> partialMaps, List<List<String>> maxLengthWordsLists) {
        this.file = file;
        this.partialMaps = partialMaps;
        this.maxLengthWordsLists = maxLengthWordsLists;
        this.fileMap = new HashMap<>();
        this.fileMaxLengthWordList = new ArrayList<>();
    }

    private void combine() {
        int maxLength = maxLengthWordsLists.stream().map(a -> {
            if (a == null) {
                return 0;
            }

            return a.get(0).length();
        }).max(Integer::compareTo).get();

        for (List<String> maxLengthWordsList : maxLengthWordsLists) {
            if (maxLengthWordsList != null && maxLengthWordsList.get(0).length() == maxLength) {
                fileMaxLengthWordList.addAll(maxLengthWordsList);
            }
        }

        for (Map<Integer, Integer> partialMap : partialMaps) {
            if (partialMap == null) {
                continue;
            }

            for (Map.Entry<Integer, Integer> entry : partialMap.entrySet()) {
                fileMap.merge(entry.getKey(), entry.getValue(), Integer::sum);
            }
        }
    }

    private void process() {
        numberOfMaxLengthWords = fileMaxLengthWordList.size();
        fileLengthOfBiggestWord = fileMaxLengthWordList.get(0).length();

        int[] fibonacciSequence = new int[fileLengthOfBiggestWord + 2];
        fibonacciSequence[0] = 0;
        fibonacciSequence[1] = 1;
        for (int i = 2; i < fileLengthOfBiggestWord + 2; i++) {
            fibonacciSequence[i] = fibonacciSequence[i - 1] + fibonacciSequence[i - 2];
        }

        float sum = 0;
        float totalNumberOfWords = 0;
        for (Map.Entry<Integer, Integer> entry : fileMap.entrySet()) {
            sum += fibonacciSequence[entry.getKey() + 1] * entry.getValue();
            totalNumberOfWords += entry.getValue();
        }

        rank = sum / totalNumberOfWords;
    }

    @Override
    public Output call() throws Exception {
        combine();
        process();
        return new Output(numberOfMaxLengthWords, rank, fileLengthOfBiggestWord, file);
    }
}
