public class Output implements Comparable<Output> {
    private final Integer numberOfMaxLengthWords;
    private final Float rank;
    private final Integer maxLength;
    private final String file;

    public Output(Integer numberOfMaxLengthWords, Float rank, Integer maxLength, String file) {
        this.numberOfMaxLengthWords = numberOfMaxLengthWords;
        this.rank = rank;
        this.maxLength = maxLength;
        this.file = file;
    }

    public Float getRank() {
        return rank;
    }

    public String getFile() {
        return file;
    }

    @Override
    public int compareTo(Output output) {
        return this.rank.compareTo(output.getRank());
    }

    @Override
    public String toString() {
        return file.split("/")[2] + "," + String.format("%.2f", rank) + "," + maxLength + "," + numberOfMaxLengthWords;
    }
}
