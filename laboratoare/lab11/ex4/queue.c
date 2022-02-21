#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>

typedef struct {
    int size;
    int arr[1000];
} queue;

int main (int argc, char *argv[]) {
    int numtasks, rank;

    queue q;
    // TODO: declare the types and arrays for offsets and block counts
    MPI_Datatype customtype, oldtypes[2];
    int blockcounts[2];
    MPI_Aint offsets[2];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // TODO: create the MPI data type, using offsets and block counts, and commit the data type
    // campul size
    offsets[0] = offsetof(queue, size);
    oldtypes[0] = MPI_INT;
    blockcounts[0] = 1;

    // campul arr
    offsets[1] = offsetof(queue, arr);
    oldtypes[1] = MPI_INT;
    blockcounts[1] = 1000;

    MPI_Type_create_struct(2, blockcounts, offsets, oldtypes, &customtype);
    MPI_Type_commit(&customtype);

    srand(time(NULL));
 
    // First process starts the circle.
    if (rank == 0) {
        // First process starts the circle.
        // Generate a random number and add it in queue.
        // Sends the queue to the next process.
        for (int i = 0; i < q.size; i++) {
            q.arr[i] = 0;
        }

        int rnd = rand();
        q.arr[rank] = rnd;
        MPI_Send(&q, 1, customtype, (rank + 1) % numtasks, 0, MPI_COMM_WORLD);

        MPI_Recv(&q, 1, customtype, numtasks - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    } else {
        // Middle process.
        // Receives the queue from the previous process.
        // Generate a random number and add it in queue.
        // Sends the queue to the next process.
        MPI_Recv(&q, 1, customtype, rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        int rnd = rand();
        q.arr[rank] = rnd;
        MPI_Send(&q, 1, customtype, (rank + 1) % numtasks, 0, MPI_COMM_WORLD);
    }

    // TODO: Process 0 prints the elements from queue
    if (rank == 0) {
        for (int i = 0; i < q.size; i++) {
            printf("%d ", q.arr[i]);
        }
        
        printf("\n");
    }
    
    // TODO: free the newly created MPI data type
    MPI_Type_free(&customtype);

    MPI_Finalize();
}