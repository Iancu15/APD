package multipleProducersMultipleConsumers;

import java.util.concurrent.ArrayBlockingQueue;

public class Buffer {
	ArrayBlockingQueue<Integer> arrayBlockingQueue = new ArrayBlockingQueue<>(100);

	void put(int value) {
		try {
			arrayBlockingQueue.put(value);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

	int get() {
		try {
			return arrayBlockingQueue.take();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}

		return 0;
	}
}
