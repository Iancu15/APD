package oneProducerOneConsumer;

public class Buffer {
    private int a;
    private boolean hasValue = false;

    public synchronized void put(int value) {
        if (hasValue) {
            try {
                this.wait();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        a = value;
        hasValue = true;
        this.notifyAll();
    }

    public synchronized int get() {
        if (!hasValue) {
            try {
                this.wait();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        int returnValue = a;
        hasValue = false;
        this.notifyAll();
        return returnValue;
    }
}
