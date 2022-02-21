import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;
import java.util.concurrent.Callable;
import java.util.stream.Collectors;

public class TaskMap implements Callable<IntermediateOutput> {
    private final String file;
    private final Integer offset;
    private final Integer fragmentSize;
    private String fragment;
    private Map<Integer, Integer> partialMap;
    private List<String> maxLengthWordsList;
    private static final String regex = "[; :/\\ ̃.,><‘\\[\\]\\{\\}()!@#$%ˆ&\\-_+’=*”'|\n\r\t?]+";
    private BufferedReader br;

    public TaskMap(String file, Integer offset, Integer fragmentSize) {
        this.file = file;
        this.offset = offset;
        this.fragmentSize = fragmentSize;
        this.partialMap = new HashMap<>();
    }

    private void removeExcessPart() throws IOException {
        if (regex.indexOf(fragment.charAt(0)) != -1) {
            return;
        }

        br.reset();
        if (regex.indexOf(br.read()) == -1) {
            int i;
            for (i = 0; i < fragmentSize; i++) {
                if (regex.indexOf(fragment.charAt(i)) != -1) {
                    break;
                }
            }

            fragment = fragment.substring(i);
        }
    }

    private void addMissingPart() throws IOException {
        if (regex.indexOf(fragment.charAt(fragmentSize - 1)) != -1) {
            return;
        }

        int ch = br.read();
        StringBuffer fragmentBuffer = new StringBuffer(fragment);
        while (ch != -1 && regex.indexOf(ch) == -1) {
            fragmentBuffer.append((char) ch);
            ch = br.read();
        }

        fragment = fragmentBuffer.toString();
    }

    private void createResult() {
        String[] words = fragment.split(regex);
        try {
            partialMap = Arrays.stream(words).filter(a -> a.length() > 0)
                    .collect(Collectors.groupingBy(String::length)).values()
                    .stream().collect(Collectors.toMap(a -> a.get(0).length(), List::size));

            int maxLength = partialMap.keySet().stream().max(Comparator.naturalOrder()).get();
            maxLengthWordsList = Arrays.stream(words).filter(a -> a.length() == maxLength)
                    .collect(Collectors.toList());
        } catch (NoSuchElementException e) {
            partialMap = null;
            maxLengthWordsList = null;
        }
    }

    @Override
    public IntermediateOutput call() throws Exception {
        File f = new File(file);
        FileReader fr = new FileReader(f);
        br = new BufferedReader(fr);
        if (offset > 0) {
            br.skip(offset - 1);
            br.mark((int) f.length() - offset + 2);
            br.skip(1);
        }

        char[] fragmentArr = new char[fragmentSize];
        int nrOfCharsRead = br.read(fragmentArr, 0, fragmentSize);
        if (nrOfCharsRead < fragmentSize) {
            throw new IllegalArgumentException();
        }

        fragment = new String(fragmentArr);
        if (offset == 0) {
            addMissingPart();
        } else if (f.length() - offset == fragmentSize) {
            removeExcessPart();
        } else {
            addMissingPart();
            removeExcessPart();
        }

        createResult();
        br.close();
        return new IntermediateOutput(file, partialMap, maxLengthWordsList);
    }
}