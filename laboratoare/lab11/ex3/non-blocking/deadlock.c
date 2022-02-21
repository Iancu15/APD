#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define SIZE 500000
 
int main (int argc, char *argv[])
{
    int  numtasks, rank, len, flag;
 
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks); // Total number of processes.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // The current process ID / Rank.
 
    srand(42);
    int num1[SIZE], num2[SIZE];
    MPI_Status status[2];
    MPI_Request request[2];
    if (rank == 0) {
        for (int i = 0; i < SIZE; i++) {
            num1[i] = 100;
        }
        MPI_Isend(&num1, SIZE, MPI_INT, 1, 0, MPI_COMM_WORLD, &request[0]);
        MPI_Irecv(&num2, SIZE, MPI_INT, 1, 0, MPI_COMM_WORLD, &request[1]);
        MPI_Waitall(2, request, status);
    } else {
        for (int i = 0; i < SIZE; i++) {
            num2[i] = 200;
        }
        MPI_Isend(&num2, SIZE, MPI_INT, 1, 0, MPI_COMM_WORLD, &request[0]);
        MPI_Irecv(&num1, SIZE, MPI_INT, 1, 0, MPI_COMM_WORLD, &request[1]);
        MPI_Waitall(2, request, status);
    }

    MPI_Finalize();
 
}