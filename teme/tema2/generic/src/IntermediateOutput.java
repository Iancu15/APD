import java.util.List;
import java.util.Map;

public class IntermediateOutput {
    private final String file;
    private final Map<Integer, Integer> partialMap;
    private final List<String> maxLengthWordsList;

    public IntermediateOutput(String file, Map<Integer, Integer> partialMap, List<String> maxLengthWordsList) {
        this.file = file;
        this.partialMap = partialMap;
        this.maxLengthWordsList = maxLengthWordsList;
    }

    public Map<Integer, Integer> getPartialMap() {
        return partialMap;
    }

    public List<String> getMaxLengthWordsList() {
        return maxLengthWordsList;
    }

    public String getFile() {
        return file;
    }
}
