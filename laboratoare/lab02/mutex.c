#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 100

int a = 0;

void *f(void *arg)
{
	pthread_mutex_t *mutex = (pthread_mutex_t*) arg;
	int r = pthread_mutex_lock(mutex);
	if (r != 0) {
		printf("mutex lock err\n");
		exit(-1);
	}

	a += 2;

	r = pthread_mutex_unlock(mutex);
	if (r != 0) {
		printf("mutex unlock err\n");
		exit(-1);
	}

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	int i, r;
	void *status;
	pthread_t threads[NUM_THREADS];
	pthread_mutex_t mutex;
	r = pthread_mutex_init(&mutex, NULL);
	if (r != 0) {
		printf("mutex init err\n");
		exit(-1);
	}

	for (i = 0; i < NUM_THREADS; i++) {
		r = pthread_create(&threads[i], NULL, f, &mutex);

		if (r) {
			printf("Eroare la crearea thread-ului %d\n", i);
			exit(-1);
		}
	}

	for (i = 0; i < NUM_THREADS; i++) {
		r = pthread_join(threads[i], &status);

		if (r) {
			printf("Eroare la asteptarea thread-ului %d\n", i);
			exit(-1);
		}
	}

	printf("a = %d\n", a);
	
	r = pthread_mutex_destroy(&mutex);
	if (r != 0) {
		printf("mutex destroy err\n");
		exit(-1);
	}

	return 0;
}
