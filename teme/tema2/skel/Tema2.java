import java.io.*;
import java.util.*;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;

public class Tema2 {
    public static void main(String[] args) throws IOException {
        if (args.length < 3) {
            System.err.println("Usage: Tema2 <workers> <in_file> <out_file>");
            return;
        }

        File inputFile = new File(args[1]);
        FileReader fr = new FileReader(inputFile);
        BufferedReader br = new BufferedReader(fr);
        String line = br.readLine();
        Integer fragmentSize = Integer.parseInt(line);

        line = br.readLine();
        Integer numberOfFiles = Integer.parseInt(line);
        List<String> files = new ArrayList<>();
        for (int i = 0; i < numberOfFiles; i++) {
            files.add(br.readLine());
        }

        Integer numberOfWorkers = Integer.parseInt(args[0]);
        MapReduce mapReduce = new MapReduce(numberOfWorkers, fragmentSize, files);
        List<Output> outputs = null;
        try {
            outputs = mapReduce.execute();
        } catch (InterruptedException | ExecutionException e) {
            e.printStackTrace();
        }

        assert outputs != null;
        Collections.sort(outputs, Collections.reverseOrder());
        FileWriter writer = new FileWriter(args[2]);
        boolean isFirst = true;
        for (Output output : outputs) {
            if (isFirst) {
                isFirst = false;
            } else {
                writer.write("\n");
            }

            writer.write(output.toString());
        }

        writer.close();
    }
}
