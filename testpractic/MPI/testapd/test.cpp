#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
using namespace std;

int main (int argc, char *argv[])
{
    int  numtasks, rank, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Get_processor_name(hostname, &len);
    int number = 2 * numtasks - 2 * rank;
    int recv_number;
    for (int i = 0; i < numtasks; i++) {
        if (i % 2 == 0) {
            if (rank % 2 == 0) {
                if (rank != numtasks - 1) {
                    MPI_Send(&number, 1, MPI_INT, rank + 1, i, MPI_COMM_WORLD);
                    MPI_Recv(&recv_number, 1, MPI_INT, rank + 1, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    if (recv_number < number) {
                        number = recv_number;
                    }
                }
            } else {
                MPI_Send(&number, 1, MPI_INT, rank - 1, i, MPI_COMM_WORLD);
                MPI_Recv(&recv_number, 1, MPI_INT, rank - 1, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if (recv_number > number) {
                    number = recv_number;
                }
            }
        } else {
            if (rank % 2 == 1) {
                if (rank != numtasks - 1) {
                    MPI_Send(&number, 1, MPI_INT, rank + 1, i, MPI_COMM_WORLD);
                    MPI_Recv(&recv_number, 1, MPI_INT, rank + 1, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    if (recv_number < number) {
                        number = recv_number;
                    }
                }
            } else if (rank != 0) {
                MPI_Send(&number, 1, MPI_INT, rank - 1, i, MPI_COMM_WORLD);
                MPI_Recv(&recv_number, 1, MPI_INT, rank - 1, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if (recv_number > number) {
                    number = recv_number;
                }
            }
        }
    }

    printf("%d - %d\n", rank, number);

    if (rank != 0) {
        MPI_Send(&number, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
        for (int i = numtasks - 1; i > rank; i--) {
            MPI_Recv(&recv_number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&recv_number, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
        }
    }

    if (rank == 0) {
        int *v = (int*) malloc(sizeof(int) * numtasks);
        v[0] = number;
        for (int i = 1; i < numtasks; i++) {
            MPI_Recv(&recv_number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            v[i] = recv_number;
        }

        for (int i = 0; i < numtasks; i++) {
            printf("%d ", v[i]);
        }
        printf("\n");

        free(v);
    }

    MPI_Finalize();
}