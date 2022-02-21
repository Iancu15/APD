#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct thread_arg {
    pthread_barrier_t *barrier;
    int thread_id;
    int* v;
    int* v_result;
    int N;
    int P;
} ThreadArg;

int min(int a, int b) {
    if (a < b) {
        return a;
    }

    return b;
}

void barrier_wait(pthread_barrier_t *barrier) {
    int r = pthread_barrier_wait(barrier);
    if (r != 0 && r != PTHREAD_BARRIER_SERIAL_THREAD) {
        printf("barrier wait error\n");
        exit(-1);
    }
}

void *thread_function(void *arg) {
    ThreadArg *t_arg = (ThreadArg*) arg;
    pthread_barrier_t *barrier = t_arg->barrier;
    int thread_id = t_arg->thread_id;
    int* v = t_arg->v;
    int* v_result = t_arg->v_result;
    int N = t_arg->N;
    int P = t_arg->P;

    int start = thread_id * ceil((float) N / P);
    int end = min((thread_id + 1) * ceil((float) N / P), N);
    for (int i = start; i < end; i++) {
        int number_of_smaller_elems = 0;
        for (int j = 0; j < N; j++) {
            if (v[j] < v[i]) {
                number_of_smaller_elems++;
            }
        }

        v_result[number_of_smaller_elems] = v[i];
    }

    barrier_wait(barrier);

    if (thread_id == P - 1) {
        for (int i = 0; i < N; i++) {
            printf("%d ", v_result[i]);
        }

        printf("\n");
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int N = atoi(argv[1]);
    int P = atoi(argv[2]);
    int *v = malloc(sizeof(int) * N);
    int *v_result = calloc(N, sizeof(int));
    for (int i = 0; i < N; i++) {
        v[i] = N - i + 5;
    }

    pthread_barrier_t barrier;
    int r = pthread_barrier_init(&barrier, NULL, P);
    if (r != 0) {
        printf("barrier init error\n");
        exit(-1);
    }

    ThreadArg thread_args[P];
    pthread_t threads[P];

    // se creeaza thread-urile
    for (int id = 0; id < P; id++) {
        thread_args[id].barrier = &barrier;
        thread_args[id].thread_id = id;
        thread_args[id].v = v;
        thread_args[id].v_result = v_result;
        thread_args[id].N = N;
        thread_args[id].P = P;
        pthread_create(&threads[id], NULL, thread_function, &thread_args[id]);
    }

    // se asteapta thread-urile
    for (int i = 0; i < P; i++) {
        pthread_join(threads[i], NULL);
    }

    r = pthread_barrier_destroy(&barrier);
    if (r != 0) {
        printf("barrier destroy error\n");
        exit(-1);
    }

    free(v);
    free(v_result);
}