#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define ROOT 0

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    // Checks the number of processes allowed.
    if (numtasks != 2) {
        printf("Wrong number of processes. Only 2 allowed!\n");
        MPI_Finalize();
        return 0;
    }

    // How many numbers will be sent.
    int send_numbers = 10;

    if (rank == 0) {

        srand(42);
        for (int i = 0; i < send_numbers; i++) {
            // Generate the random numbers.
            int rnd = rand(); 

            // Generate the random tags.
            int tag = rand() % 10;

            // Sends the numbers with the tags to the second process.
            std::cout << "Sending message " << rnd << " with tag " << tag << "\r\n";
            MPI_Send(&rnd, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
        }

    } else {

        MPI_Status status;
        int recv_num;
        for (int i = 0; i < send_numbers; i++) {
            // Receives the information from the first process.
            MPI_Recv(&recv_num, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            // Prints the numbers with their corresponding tags.
            std::cout << "Received message " << recv_num << " with tag " << status.MPI_TAG << "\r\n";
        }

    }

    MPI_Finalize();

}

