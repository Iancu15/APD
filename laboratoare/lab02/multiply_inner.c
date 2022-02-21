#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int N;
int P;
int **a;
int **b;
int **c;

typedef struct {
	int thread_id;
	pthread_mutex_t *mutex;
} StructArg;

void *thread_function(void *arg)
{
	StructArg* struct_arg = (StructArg*) arg;
	int thread_id = struct_arg->thread_id;

	int start = thread_id * ((double) N / P);
	int end = (thread_id + 1) * ((double) N / P);

	if (end > N) {
		end = N;
	} 

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			for (int k = start; k < end; k++) {
				int r = pthread_mutex_lock(struct_arg->mutex);
				if (r != 0) {
					printf("mutex lock err\n");
					exit(-1);
				}

				c[i][j] += a[i][k] * b[k][j];

				r = pthread_mutex_unlock(struct_arg->mutex);
				if (r != 0) {
					printf("mutex unlock err\n");
					exit(-1);
				}
			}
		}
	}

	pthread_exit(NULL);
}

void get_args(int argc, char **argv)
{
	if(argc < 3) {
		printf("Numar insuficient de parametri: ./program N P\n");
		exit(1);
	}

	N = atoi(argv[1]);
	P = atoi(argv[2]);
}

void init()
{
	a = malloc(sizeof(int *) * N);
	b = malloc(sizeof(int *) * N);
	c = malloc(sizeof(int *) * N);

	if (a == NULL || b == NULL || c == NULL) {
		printf("Eroare la malloc!");
		exit(1);
	}

	int i, j;
	for (i = 0; i < N; i++) {
		a[i] = malloc(sizeof(int) * N);
		b[i] = malloc(sizeof(int) * N);
		c[i] = malloc(sizeof(int) * N);

		if(a[i] == NULL || b[i] == NULL || c[i] == NULL) {
			printf("Eroare la malloc!");
			exit(1);
		}
	}

	for (i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			c[i][j] = 0;

			if(i <= j) {
				a[i][j] = 1;
				b[i][j] = 1;
			} else {
				a[i][j] = 0;
				b[i][j] = 0;
			}
		}
	}
}

void print(int **mat)
{
	int i, j;

	for (i = 0; i < N; i++) {
		for(j = 0; j < N; j++) {
			printf("%i\t", mat[i][j]);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	int i, r;

	get_args(argc, argv);
	init();

	pthread_t tid[P];
	pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
	r = pthread_mutex_init(mutex, NULL);
	if (r != 0) {
		printf("mutex init err\n");
		exit(-1);
	}

	for (i = 0; i < P; i++) {
		StructArg* struct_arg = (StructArg*) malloc(sizeof(StructArg));
		struct_arg->thread_id = i;
		struct_arg->mutex = mutex;
		pthread_create(&tid[i], NULL, thread_function, struct_arg);
	}

	for (i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);
	}

	print(c);

	r = pthread_mutex_destroy(mutex);
	if (r != 0) {
		printf("mutex destroy err\n");
		exit(-1);
	}

	return 0;
}
