#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_THREADS 8

void *f(void *arg) {
  	long id = (long)arg;
	int i;
	for (i = 1 ; i <= 100; i++) {
		printf("Hello World din iteratia %d a thread-ului %ld!\n", i, id);
	}

  	pthread_exit(NULL);
}

void *g(void *arg) {
  	long id = (long)arg;
	int i;
	for (i = 1 ; i <= 100; i++) {
		printf("Hello Romania din iteratia %d a thread-ului %ld!\n", i, id);
	}

  	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	long cores = sysconf(_SC_NPROCESSORS_CONF);
	pthread_t threads[cores];
  	int r;
  	long id;
  	void *status;

  	for (id = 0; id < NUM_THREADS; id++) {
		if (id % 2 == 0) {
			r = pthread_create(&threads[id], NULL, f, (void *)id);
		} else {
			r = pthread_create(&threads[id], NULL, g, (void *)id);
		}

		if (r) {
	  		printf("Eroare la crearea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}

  	for (id = 0; id < NUM_THREADS; id++) {
		r = pthread_join(threads[id], &status);

		if (r) {
	  		printf("Eroare la asteptarea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}

  	pthread_exit(NULL);
}
