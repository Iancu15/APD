#include "mpi.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
using namespace std;

int main (int argc, char *argv[])
{
    int  numtasks, rank, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks); // Total number of processes.
    MPI_Comm_rank(MPI_COMM_WORLD,&rank); // The current process ID / Rank.
    MPI_Get_processor_name(hostname, &len);

    if (rank == 0) {
        fstream file;
        file.open(argv[1], ios::in);
        string line;
        if (file.is_open()) {
            getline(file, line);
            int number_of_lines = stoi(line);
            int current_mapper = 1;
            for (int i = 0; i < number_of_lines; i++) {
                getline(file, line);
                MPI_Send(line.c_str(), 15, MPI_CHAR, current_mapper, 0, MPI_COMM_WORLD);
                current_mapper++;
                if (current_mapper > 3) {
                    current_mapper = 1;
                }
            }

            for (int mapper_rank = 1; mapper_rank < 4; mapper_rank++) {
                MPI_Send("done", 15, MPI_CHAR, mapper_rank, 0, MPI_COMM_WORLD);
            }
        }

        file.close();
    }

    if (rank > 0 && rank < 4) {
        int number_of_consonants = 0;
        int number_of_vowels = 0;
        while (1) {
            char* line = (char*) malloc(sizeof(15) * sizeof(char));
            MPI_Recv(line, 15, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (strcmp(line, "done") == 0) {
                break;
            }

            printf("[%d] %s\n", rank, line);
            int len = strlen(line);
            for (int i = 0; i < len; i++) {
                if (strchr("aeiouAEIOU", line[i]) != NULL) {
                    number_of_vowels++;
                } else {
                    number_of_consonants++;
                }
            }

            free(line);
        }

        printf("[%d] v=%d c=%d\n", rank, number_of_vowels, number_of_consonants);
        MPI_Send(&number_of_vowels, 1, MPI_INT, 4, 1, MPI_COMM_WORLD);
        MPI_Send(&number_of_consonants, 1, MPI_INT, 5, 1, MPI_COMM_WORLD);
    }

    if (rank == 4 || rank == 5) {
        int number = 0;
        int recv_number;
        for (int i = 0; i < 3; i++) {
            MPI_Recv(&recv_number, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            number += recv_number;
        }

        printf("[%d] ", rank);
        if (rank == 4) {
            cout << "Vowels";
        } else {
            cout << "Consonants";
        }

        printf(": %d\n", number);
    }

    MPI_Finalize();
}