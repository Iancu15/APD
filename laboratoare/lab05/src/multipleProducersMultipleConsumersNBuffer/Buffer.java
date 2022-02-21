package multipleProducersMultipleConsumersNBuffer;

import java.util.Queue;
import java.util.concurrent.Semaphore;

public class Buffer {
    int BUFFER_SIZE = 4;
    Queue<Integer> queue;
    Semaphore producer;
    Semaphore consumer;
    
    public Buffer(int size) {
        queue = new LimitedQueue<Integer>(BUFFER_SIZE);
        producer = new Semaphore(BUFFER_SIZE);
        consumer = new Semaphore(0);
    }

	public void put(int value) {
        try {
            producer.acquire();
        } catch(InterruptedException e) {
            e.printStackTrace();
        }
        synchronized (queue) {
            queue.add(value);
        }

        consumer.release();
	}

	public int get() {
        int a = -1;
        try {
            consumer.acquire();
        } catch(InterruptedException e) {
            e.printStackTrace();
        }

        synchronized (queue) {
            Integer result = queue.poll();
            if (result != null) {
                a = result;
            }
        }

        producer.release();
        return a;
	}
}
