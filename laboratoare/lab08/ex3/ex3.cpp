#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#define MULTI 5 // chunk dimension
#define ROOT 0

int main (int argc, char *argv[])
{
    int  numtasks, rank, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Get_processor_name(hostname, &len);

    int num_elements = MULTI * numtasks; // total elements
    std::vector<int> v_send;
    std::vector<int> v_recv(MULTI);

    // ROOT process generates the values for the full vector.
    // Scatter the vector to all processes.
    if (rank == ROOT) {
        v_send = std::vector<int>(num_elements, 0);
    }

    MPI_Scatter(v_send.data(), MULTI, MPI_INT, v_recv.data(), MULTI, MPI_INT, ROOT, MPI_COMM_WORLD);

    /*
     * Prints the values received after scatter.
     * NOTE: If MULTI changed, also change this line.
     */
    printf("Process [%d]: have elements %d %d %d %d %d.\n", rank, v_recv[0],
            v_recv[1], v_recv[2], v_recv[3], v_recv[4]);

    // Each process increments the values of the partial vector received.
    // Gathers the values from all the processes.
    // The ROOT process prints the elements received.

    for (int &v : v_recv) {
        v += rank + 1;
    }

    MPI_Gather(v_recv.data(), MULTI, MPI_INT, v_send.data(), MULTI, MPI_INT, ROOT, MPI_COMM_WORLD);

    if (rank == ROOT) {
        std::cout << "Received: ";

        for (const int &v : v_send) {
            std::cout << v << " ";
        }

        std::cout << "\r\n";
    }

    MPI_Finalize();

}

