#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    int numtasks, rank, flag;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int recv_num;
    MPI_Status status;
    MPI_Request request;
    // First process starts the circle.
    if (rank == 0) {
        // First process starts the circle.
        // Generate a random number.
        // Send the number to the next process.
        srand(42);
        int rnd = rand();
        std::cout << "Sending[" << rank << "] " << rnd << "\r\n";
        MPI_Isend(&rnd, 1, MPI_INT, (rank + 1) % numtasks, 0, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);
        if (!flag) {
            MPI_Wait(&request, &status);
        }

        MPI_Irecv(&recv_num, 1, MPI_INT, numtasks - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);
        if (!flag) {
            MPI_Wait(&request, &status);
        }

        std::cout << "Received[" << rank << "] " << recv_num << "\r\n";
    } else {
        // Middle process.
        // Receives the number from the previous process.
        // Increments the number.
        // Sends the number to the next process.
        MPI_Irecv(&recv_num, 1, MPI_INT, rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
        MPI_Test(&request, &flag, &status);
        if (!flag) {
            MPI_Wait(&request, &status);
        }

        std::cout << "Received[" << rank << "] " << recv_num << "\r\n";

        recv_num += 2;
        std::cout << "Sending[" << rank << "] " << recv_num << "\r\n";
        MPI_Isend(&recv_num, 1, MPI_INT, (rank + 1) % numtasks, 0, MPI_COMM_WORLD, &request);
    }

    MPI_Finalize();

}

