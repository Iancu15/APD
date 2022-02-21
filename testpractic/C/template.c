#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct thread_arg {
    pthread_barrier_t *barrier;
    pthread_mutex_t *mutex;
    int thread_id;
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

void mutex_lock(pthread_mutex_t *mutex) {
    int r = pthread_mutex_lock(mutex);
    if (r != 0) {
        printf("mutex lock error\n");
        exit(-1);
    }
}

void mutex_unlock(pthread_mutex_t *mutex) {
    int r = pthread_mutex_unlock(mutex);
    if (r != 0) {
        printf("mutex lock error\n");
        exit(-1);
    }
}

void *thread_function(void *arg) {
    ThreadArg *t_arg = (ThreadArg*) arg;
    pthread_barrier_t *barrier = t_arg->barrier;
    pthread_mutex_t *mutex = t_arg->mutex;
    int thread_id = t_arg->thread_id;
    int N = t_arg->N;
    int P = t_arg->P;

    int start = thread_id * ceil((float) N / P);
    int end = min((thread_id + 1) * ceil((float) N / P), N);
    for (int i = start; i < end; i++) {
        // stuff
    }

    barrier_wait(barrier);
    mutex_lock(mutex);
    mutex_unlock(mutex);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int N = atoi(argv[1]);
    int P = atoi(argv[2]);

    pthread_barrier_t barrier;
    int r = pthread_barrier_init(&barrier, NULL, P);
    if (r != 0) {
        printf("barrier init error\n");
        exit(-1);
    }

    pthread_mutex_t mutex;
    r = pthread_mutex_init(&mutex, NULL);
    if (r != 0) {
        printf("mutex init error\n");
        exit(-1);
    }

    ThreadArg thread_args[P];
    pthread_t threads[P];

    // se creeaza thread-urile
    for (int id = 0; id < P; id++) {
        thread_args[id].barrier = &barrier;
        thread_args[id].mutex = &mutex;
        thread_args[id].thread_id = id;
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

    r = pthread_mutex_destroy(&mutex);
    if (r != 0) {
        printf("mutex destroy error\n");
        exit(-1);
    }
}