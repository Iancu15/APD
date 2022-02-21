#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 2

typedef struct {
	int thread_id;
	pthread_barrier_t *barrier;
} StructArg;

void *f(void *arg)
{
	StructArg* struct_arg = (StructArg*) arg;
	int thread_id = struct_arg->thread_id;

	if (thread_id == 1) {
		printf("1\n");
	}

	int r = pthread_barrier_wait(struct_arg->barrier);
	if (r != 0 && r != PTHREAD_BARRIER_SERIAL_THREAD) {
		printf("barrier wait err\n");
		exit(-1);
	}

	if (thread_id == 0) {
		printf("2\n");
	}

	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	int i, r;
	void *status;
	pthread_t threads[NUM_THREADS];
	pthread_barrier_t *barrier = malloc(sizeof(pthread_barrier_t));
	r = pthread_barrier_init(barrier, NULL, NUM_THREADS);
	if (r != 0) {
		printf("barrier init err\n");
		exit(-1);
	}

	for (i = 0; i < NUM_THREADS; i++) {
		StructArg* struct_arg = (StructArg*) malloc(sizeof(StructArg));
		struct_arg->thread_id = i;
		struct_arg->barrier = barrier;
		r = pthread_create(&threads[i], NULL, f, struct_arg);

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

	r = pthread_barrier_destroy(barrier);
	if (r != 0) {
		printf("barrier destroy err\n");
		exit(-1);
	}

	return 0;
}
