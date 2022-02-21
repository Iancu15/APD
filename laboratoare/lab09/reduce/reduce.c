#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define MASTER 0

int main (int argc, char *argv[])
{
    int procs, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int value = rank;
    int value_recv;
    MPI_Status status;
    for (int i = 2; i <= procs; i *= 2) {
        if (rank % i == 0) {
            MPI_Recv(&value_recv, 1, MPI_INT, rank + (i / 2), MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            value += value_recv;
        } else if (rank % (i / 2) == 0) {
            MPI_Send(&value, 1, MPI_INT, rank - (i / 2), 0, MPI_COMM_WORLD);
        }
    }

    if (rank == MASTER) {
        printf("Result = %d\n", value);
    }

    MPI_Finalize();

}

