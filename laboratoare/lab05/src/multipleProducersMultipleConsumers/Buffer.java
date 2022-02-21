package multipleProducersMultipleConsumers;

public class Buffer {
    private int a;
    private boolean hasValue = false;

    public synchronized void put(int value) {
        while (hasValue) {
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
        while (!hasValue) {
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
