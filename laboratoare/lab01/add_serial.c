#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
    schelet pentru exercitiul 5
*/

int* arr;
int array_size;

typedef struct {
    long id;
    int number_of_threads;
    int array_size;
    int* array;
} ArgStruct;

void *f(void *arg) {
    printf("%p\n", arg);
  	ArgStruct* structure = (ArgStruct*)arg;
    long ID = structure->id;
    int P = structure->number_of_threads;
    int N = structure->array_size;
    int start = ID * (double)N / P;
    int end = (ID + 1) * (double)N / P;
    if (end > N) {
        end = N;
    }

    int i;
	for (i = start ; i < end; i++) {
        structure->array[i] += 100;
        //sleep(1);
	}

  	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Specificati dimensiunea array-ului\n");
        exit(-1);
    }

    int array_size = atoi(argv[1]);
    int number_of_threads = atoi(argv[2]);
    pthread_t threads[number_of_threads];

    arr = malloc(array_size * sizeof(int));
    for (int i = 0; i < array_size; i++) {
        arr[i] = i;
    }

    for (int i = 0; i < array_size; i++) {
        printf("%d", arr[i]);
        if (i != array_size - 1) {
            printf(" ");
        } else {
            printf("\n");
        }
    }

    for (long id = 0; id < number_of_threads; id++) {
        ArgStruct* structure = (ArgStruct*) malloc(sizeof(ArgStruct));
        structure->id = id;
        structure->number_of_threads = number_of_threads;
        structure->array_size = array_size;
        structure->array = arr;
        int r = pthread_create(&threads[id], NULL, f, (void *)structure);

		if (r) {
	  		printf("Eroare la crearea thread-ului %ld\n", id);
	  		exit(-1);
		}
  	}

    for (int i = 0; i < array_size; i++) {
        printf("%d", arr[i]);
        if (i != array_size - 1) {
            printf(" ");
        } else {
            printf("\n");
        }
    }

  	pthread_exit(NULL);
}
