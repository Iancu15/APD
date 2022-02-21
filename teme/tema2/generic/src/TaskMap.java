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
        // daca primul caracter e separator, atunci primul cuvant din fragment nu are parti
        // lipsa in fragmentul precedent
        if (regex.indexOf(fragment.charAt(0)) != -1) {
            return;
        }

        // daca ultimul caracter din fragmentul precedent nu e separator, atunci primul
        // cuvant din fragment are parti lipsa in fragmentul precedent si il elimin din fragmentul curent
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
        // daca ultimul caracter e separator, atunci nu sunt parti lipsa in urmatorul fragment
        if (regex.indexOf(fragment.charAt(fragmentSize - 1)) != -1) {
            return;
        }

        // altfel adaug caractere din urmatorul fragment pana intalnesc un separator pentru a
        // completa ultimul cuvant din fragment (daca primul caracter din urmatorul fragment e separator
        // nu se adauga nimic)
        int ch = br.read();
        StringBuilder fragmentBuffer = new StringBuilder(fragment);
        while (ch != -1 && regex.indexOf(ch) == -1) {
            fragmentBuffer.append((char) ch);
            ch = br.read();
        }

        fragment = fragmentBuffer.toString();
    }

    private void createResult() {
        String[] words = fragment.split(regex);
        try {
            // elimin cuvintele goale si ce ramane grupez dupa lungime, folosesc listele
            // de cuvinte de aceeasi lungime si le mapez intr-o intrare intr-un map cu cheia lungimea
            // cuvintelor si valoarea dimensiunea listei de cuvinte de aceeasi lungime
            partialMap = Arrays.stream(words).filter(a -> a.length() > 0)
                    .collect(Collectors.groupingBy(String::length)).values()
                    .stream().collect(Collectors.toMap(a -> a.get(0).length(), List::size));

            // aflu lungimea maxima a unui cuvant si filtrez cuvintele care au acea lungime
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

        // pun un mark la litera de dinainte de fragment pentru removeExcessPart
        if (offset > 0) {
            long nrOfCharsSkipped = br.skip(offset - 1);
            if (nrOfCharsSkipped < offset - 1) {
                throw new IllegalArgumentException("Skipping offset - 1 buffering reader");
            }

            br.mark((int) f.length() - offset + 2);
            nrOfCharsSkipped = br.skip(1);
            if (nrOfCharsSkipped < 1) {
                throw new IllegalArgumentException("Skipping 1 buffering reader");
            }
        }

        char[] fragmentArr = new char[fragmentSize];
        int nrOfCharsRead = br.read(fragmentArr, 0, fragmentSize);
        if (nrOfCharsRead < fragmentSize) {
            throw new IllegalArgumentException("Reading buffering reader");
        }

        fragment = new String(fragmentArr);

        // primul fragment nu are cuvinte de inceput in exces si ultimul fragment
        // nu are cuvinte incomplete la final
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