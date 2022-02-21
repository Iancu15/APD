#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct thread_arg {
    int thread_id;
    int P;
    int sleep_duration;
    int *is_it_done;
    int *was_already_done;
} ThreadArg;

void *thread_function(void *arg) {
    ThreadArg *t_arg = (ThreadArg*) arg;
    int thread_id = t_arg->thread_id;
    int P = t_arg->P;
    int sleep_duration = t_arg->sleep_duration;
    int *is_it_done = t_arg->is_it_done;
    int *was_already_done = t_arg->was_already_done;

    if (thread_id == 0) {
        int still_working_threads;
        do {
            still_working_threads = 0;
            for (int i = 0; i < P - 1; i++) {
                if (!is_it_done[i]) {
                    still_working_threads = 1;
                } else {
                    if (!was_already_done[i]) {
                        printf("Thread-ul %d a terminat\n", i + 1);
                        was_already_done[i] = 1;
                    }
                }
            }
        } while (still_working_threads);
    } else {
        sleep(sleep_duration);
        is_it_done[thread_id - 1] = 1;
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int P = atoi(argv[1]);
    int *is_it_done = calloc(P - 1, sizeof(int));
    int *was_already_done = calloc(P - 1, sizeof(int));

    ThreadArg thread_args[P];
    pthread_t threads[P];

    // se creeaza thread-urile
    for (int id = 0; id < P; id++) {
        thread_args[id].thread_id = id;
        thread_args[id].P = P;
        thread_args[id].is_it_done = is_it_done;
        thread_args[id].was_already_done = was_already_done;
        if (id == 0) {
            thread_args[id].sleep_duration = 0;
        } else {
            thread_args[id].sleep_duration = atoi(argv[id + 1]);
        }

        pthread_create(&threads[id], NULL, thread_function, &thread_args[id]);
    }

    // se asteapta thread-urile
    for (int i = 0; i < P; i++) {
        pthread_join(threads[i], NULL);
    }
}