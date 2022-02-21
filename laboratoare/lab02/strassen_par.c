#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREADS 7

int N;
int **a;
int **b;
int **c;
int **M1;
int **AUXM11;
int **AUXM12;
int **M2;
int **AUXM21;
int **M3;
int **AUXM31;
int **M4;
int **AUXM41;
int **M5;
int **AUXM51;
int **M6;
int **AUXM61;
int **AUXM62;
int **M7;
int **AUXM71;
int **AUXM72;

void get_args(int argc, char **argv)
{
	if(argc < 2) {
		printf("Numar insuficient de parametri: ./program N\n");
		exit(1);
	}

	N = atoi(argv[1]);
}

int **alloc_matrix(int n, int m)
{
	int **matrix;
	matrix = malloc(sizeof(int *) * n);
	if(matrix == NULL) {
		printf("Eroare la malloc");
		exit(1);
	}

	int i;
	for (i = 0; i < N; i++) {
		matrix[i] = malloc(sizeof(int) * N);
		if (matrix[i] == NULL) {
			printf("Eroare la malloc");
			exit(1);
		}
	}

	return(matrix);
}

void init()
{
	int i, j;
	a = alloc_matrix(N, N);
	b = alloc_matrix(N, N);
	c = alloc_matrix(N, N);

	M1 = alloc_matrix(N, N);
	AUXM11 = alloc_matrix(N, N);
	AUXM12 = alloc_matrix(N, N);

	M2 = alloc_matrix(N, N);
	AUXM21 = alloc_matrix(N, N);

	M3 = alloc_matrix(N, N);
	AUXM31 = alloc_matrix(N, N);

	M4 = alloc_matrix(N, N);
	AUXM41 = alloc_matrix(N, N);

	M5 = alloc_matrix(N, N);
	AUXM51 = alloc_matrix(N, N);

	M6 = alloc_matrix(N, N);
	AUXM61 = alloc_matrix(N, N);
	AUXM62 = alloc_matrix(N, N);

	M7 = alloc_matrix(N, N);
	AUXM71 = alloc_matrix(N, N);
	AUXM72 = alloc_matrix(N, N);

	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			c[i][j] = 0;
			
			M1[i][j] = 0;
			AUXM11[i][j] = 0;
			AUXM12[i][j] = 0;
			
			M2[i][j] = 0;
			AUXM21[i][j] = 0;
			
			M3[i][j] = 0;
			AUXM31[i][j] = 0;
			
			M4[i][j] = 0;
			AUXM41[i][j] = 0;
			
			M5[i][j] = 0;
			AUXM51[i][j] = 0;
			
			M6[i][j] = 0;
			AUXM61[i][j] = 0;
			AUXM62[i][j] = 0;
			
			M7[i][j] = 0;
			AUXM71[i][j] = 0;
			AUXM72[i][j] = 0;
			
			if (i <= j) {
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

void mul_matrix(int **C,  int startCi, int startCj, int **A, int startAi, int startAj, int **B, int startBi, int startBj) 
{
	// NU paralelizati aceasta functie
	int i, j, k;
	for (i = 0; i < N/2; i++)
		for (j = 0; j < N/2; j++)
			for (k = 0; k < N/2; k++)
				C[startCi + i][startCj + j] += A[startAi + i][startAj + k] * B[startBi + k][startBj + j];
}

void add_matrix(int **C, int startCi, int startCj, int **A, int startAi, int startAj, int **B, int startBi, int startBj) 
{
	// NU paralelizati aceasta functie
	int i, j;
	for( i = 0; i < N/2; i++)
		for (j = 0; j < N/2; j++)
			C[startCi + i][startCj + j] = A[startAi + i][startAj + j] + B[startBi + i][startBj + j];
}

void sub_matrix(int **C, int startCi, int startCj,  int **A, int startAi, int startAj, int **B, int startBi, int startBj) 
{
	// NU paralelizati aceasta functie
	int i, j;
	for (i = 0; i < N/2; i++)
		for (j = 0; j < N/2; j++)
			C[startCi + i][startCj + j] = A[startAi + i][startAj + j] - B[startBi + i][startBj + j];
}

void *calcM1(void *arg) {
    add_matrix(AUXM11, 0, 0, a, 0, 0, a, N/2, N/2);
	add_matrix(AUXM12, 0, 0, b, 0, 0, b, N/2, N/2);
	mul_matrix(M1, 0, 0, AUXM11, 0, 0, AUXM12, 0, 0);

    pthread_exit(NULL);
}

void *calcM2(void *arg) {
    add_matrix(AUXM21, 0,0, a, N/2, 0, a, N/2, N/2);
	mul_matrix(M2, 0, 0, AUXM21, 0, 0, b, 0, 0);

    pthread_exit(NULL);
}

void *calcM3(void *arg) {
    sub_matrix(AUXM31, 0, 0, b, 0, N/2, b, N/2, N/2);
	mul_matrix(M3, 0, 0, a, 0, 0, AUXM31, 0, 0);

    pthread_exit(NULL);
}

void *calcM4(void *arg) {
    sub_matrix(AUXM41, 0, 0, b, N/2, 0, b, 0, 0);
	mul_matrix(M4, 0, 0, a, N/2, N/2, AUXM41, 0, 0);

    pthread_exit(NULL);
}

void *calcM5(void *arg) {
    add_matrix(AUXM51, 0,0, a, 0, 0, a, 0, N/2);
	mul_matrix(M5, 0, 0, AUXM51, 0, 0, b, N/2, N/2);

    pthread_exit(NULL);
}

void *calcM6(void *arg) {
    sub_matrix(AUXM61, 0, 0, a, N/2, 0, a, 0, 0);
	add_matrix(AUXM62, 0, 0, b, 0, 0, b, 0, N/2);
	mul_matrix(M6, 0, 0, AUXM61, 0, 0, AUXM62, 0, 0);

    pthread_exit(NULL);
}

void *calcM7(void *arg) {
    sub_matrix(AUXM71, 0, 0, a, 0, N/2, a, N/2, N/2);
	add_matrix(AUXM72, 0, 0, b, N/2, 0, b, N/2, N/2);
	mul_matrix(M7, 0, 0, AUXM71, 0, 0, AUXM72, 0, 0);

    pthread_exit(NULL);
}

void *calcC11(void *arg) {
    pthread_mutex_t *mutex = (pthread_mutex_t*) arg;
    int r = pthread_mutex_lock(mutex);
	if (r != 0) {
		printf("mutex lock err\n");
		exit(-1);
	}

    add_matrix(c, 0, 0, M1, 0, 0, M4, 0, 0);
	sub_matrix(c, 0, 0, c, 0, 0, M5, 0, 0);
	add_matrix(c, 0, 0, c, 0, 0, M7, 0, 0);

    r = pthread_mutex_unlock(mutex);
	if (r != 0) {
		printf("mutex unlock err\n");
		exit(-1);
	}

    pthread_exit(NULL);
}

void *calcC12(void *arg) {
    pthread_mutex_t *mutex = (pthread_mutex_t*) arg;
    int r = pthread_mutex_lock(mutex);
	if (r != 0) {
		printf("mutex lock err\n");
		exit(-1);
	}

    add_matrix(c, 0, N/2, M3, 0, 0, M5, 0, 0);

    r = pthread_mutex_unlock(mutex);
	if (r != 0) {
		printf("mutex unlock err\n");
		exit(-1);
	}

    pthread_exit(NULL);
}

void *calcC21(void *arg) {
    pthread_mutex_t *mutex = (pthread_mutex_t*) arg;
    int r = pthread_mutex_lock(mutex);
	if (r != 0) {
		printf("mutex lock err\n");
		exit(-1);
	}

    add_matrix(c, N/2, 0, M2, 0, 0, M4, 0, 0);

    r = pthread_mutex_unlock(mutex);
	if (r != 0) {
		printf("mutex unlock err\n");
		exit(-1);
	}

    pthread_exit(NULL);
}

void *calcC22(void *arg) {
    pthread_mutex_t *mutex = (pthread_mutex_t*) arg;
    int r = pthread_mutex_lock(mutex);
	if (r != 0) {
		printf("mutex lock err\n");
		exit(-1);
	}

    sub_matrix(c, N/2, N/2, M1, 0, 0, M2, 0, 0);
	add_matrix(c, N/2, N/2, c, N/2, N/2, M3, 0, 0);
	add_matrix(c, N/2, N/2, c, N/2, N/2, M6, 0, 0);

    r = pthread_mutex_unlock(mutex);
	if (r != 0) {
		printf("mutex unlock err\n");
		exit(-1);
	}

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	get_args(argc, argv);
	init();
    int r;
    void *status;
    pthread_t threads[NUM_THREADS];

	// calculul matricii M1
    r = pthread_create(&threads[0], NULL, calcM1, NULL);
    if (r) {
        printf("Eroare la crearea thread-ului %d\n", 0);
        exit(-1);
    }

	// calculul matricii M2
    r = pthread_create(&threads[1], NULL, calcM2, NULL);
    if (r) {
        printf("Eroare la crearea thread-ului %d\n", 1);
        exit(-1);
    }

	// calculul matricii M3
    r = pthread_create(&threads[2], NULL, calcM3, NULL);
    if (r) {
        printf("Eroare la crearea thread-ului %d\n", 2);
        exit(-1);
    }

	// calculul matricii M4
    r = pthread_create(&threads[3], NULL, calcM4, NULL);
    if (r) {
        printf("Eroare la crearea thread-ului %d\n", 3);
        exit(-1);
    }

	// calculul matricii M5
    r = pthread_create(&threads[4], NULL, calcM5, NULL);
    if (r) {
        printf("Eroare la crearea thread-ului %d\n", 4);
        exit(-1);
    }

	// calculul matricii M6
    r = pthread_create(&threads[5], NULL, calcM6, NULL);
    if (r) {
        printf("Eroare la crearea thread-ului %d\n", 5);
        exit(-1);
    }

	// calculul matricii M7
    r = pthread_create(&threads[6], NULL, calcM7, NULL);
    if (r) {
        printf("Eroare la crearea thread-ului %d\n", 6);
        exit(-1);
    }

    // n-am nevoie de bariera ca astept sa se termine toate thread-urile aici
    for (int i = 0; i < NUM_THREADS; i++) {
		r = pthread_join(threads[i], &status);

		if (r) {
			printf("Eroare la asteptarea thread-ului %d\n", i);
			exit(-1);
		}
	}

    // initializare mutex
    // folosesc mutex pentru ca operatiile de adunare si scadere
    // de matrici nu sunt atomice si astfel se face race condition
    // pe matricea c
    pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
	r = pthread_mutex_init(mutex, NULL);
	if (r != 0) {
		printf("mutex init err\n");
		exit(-1);
	}

	// calculul submatricii C1,1
    r = pthread_create(&threads[0], NULL, calcC11, mutex);
    if (r) {
        printf("Eroare la crearea thread-ului %d\n", 0);
        exit(-1);
    }

	// calculul submatricii C1,2
    r = pthread_create(&threads[1], NULL, calcC12, mutex);
    if (r) {
        printf("Eroare la crearea thread-ului %d\n", 1);
        exit(-1);
    }

	// calculul submatricii C2,1
    r = pthread_create(&threads[2], NULL, calcC21, mutex);
    if (r) {
        printf("Eroare la crearea thread-ului %d\n", 2);
        exit(-1);
    }

	// calculul submatricii C2,2
    r = pthread_create(&threads[3], NULL, calcC22, mutex);
    if (r) {
        printf("Eroare la crearea thread-ului %d\n", 3);
        exit(-1);
    }

	for (int i = 0; i < 4; i++) {
		r = pthread_join(threads[i], &status);

		if (r) {
			printf("Eroare la asteptarea thread-ului %d\n", i);
			exit(-1);
		}
	}

	print(c);

    // distrugere mutex
    r = pthread_mutex_destroy(mutex);
	if (r != 0) {
		printf("mutex destroy err\n");
		exit(-1);
	}

	return 0;
}
