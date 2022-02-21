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
        // calculez lungimea maximala globala pe baza listelor de cuvinte maximale locale
        int maxLength = maxLengthWordsLists.stream().map(a -> {
            if (a == null) {
                return 0;
            }

            return a.get(0).length();
        }).max(Integer::compareTo).get();

        // creez lista cu cuvintele maximale globale
        for (List<String> maxLengthWordsList : maxLengthWordsLists) {
            if (maxLengthWordsList != null && maxLengthWordsList.get(0).length() == maxLength) {
                fileMaxLengthWordList.addAll(maxLengthWordsList);
            }
        }

        // adaug intrarile map-urilor partiale intr-un map comun pentru tot fisierul
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

        // calculez secventa de fibonacci cu programare dinamica
        int[] fibonacciSequence = new int[fileLengthOfBiggestWord + 2];
        fibonacciSequence[0] = 0;
        fibonacciSequence[1] = 1;
        for (int i = 2; i < fileLengthOfBiggestWord + 2; i++) {
            fibonacciSequence[i] = fibonacciSequence[i - 1] + fibonacciSequence[i - 2];
        }

        // aplic formula
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
