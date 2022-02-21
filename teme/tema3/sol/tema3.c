#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define COORD0 0
#define COORD1 1
#define COORD2 2

int is_coord(int rank) {
    if (rank < 3) {
        return 1;
    }

    return 0;
}

int is_worker(int rank) {
    if (rank > 2) {
        return 1;
    }

    return 0;
}

void print_msg(int sender, int reciever) {
    printf("M(%d,%d)\n", sender, reciever);
}

void print_topology(int number_of_workers0, int number_of_workers1, int number_of_workers2,
                    int* workers0, int* workers1, int* workers2, int rank) {
    printf("%d -> 0:", rank);
    for (int i = 0; i < number_of_workers0; i++) {
        if (i != 0) {
            printf(",");
        }

        printf("%d", workers0[i]);
    }

    printf(" 1:");
    for (int i = 0; i < number_of_workers1; i++) {
        if (i != 0) {
            printf(",");
        }

        printf("%d", workers1[i]);
    }

    printf(" 2:");
    for (int i = 0; i < number_of_workers2; i++) {
        if (i != 0) {
            printf(",");
        }

        printf("%d", workers2[i]);
    }

    printf("\n");
}

int main (int argc, char *argv[]) {
    int procs, rank;
    if (argc < 3) {
        printf("usage: ./tema3 <dimensiune_vector> <eroare_comunicatie>\n");
        exit(-1);
    }

    int vector_dim = atoi(argv[1]);
    int is_error = atoi(argv[2]);
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Status status;
    FILE* fp;
    int number_of_workers;
    int number_of_workers0;
    int number_of_workers1;
    int number_of_workers2;

    int* workers;
    int* workers0;
    int* workers1;
    int* workers2;
    int coord;

    MPI_Datatype workers0_vec;
    MPI_Datatype workers1_vec;
    MPI_Datatype workers2_vec;

    // open cluster file
    switch(rank) {
        case COORD0:
            fp = fopen("cluster0.txt", "r");
            if (fp == NULL) {
                printf("Error opening cluster0.txt\n");
                exit(-1);
            }

            break;
        
        case COORD1:
            fp = fopen("cluster1.txt", "r");
            if (fp == NULL) {
                printf("Error opening cluster1.txt\n");
                exit(-1);
            }

            break;
        
        case COORD2:
            fp = fopen("cluster2.txt", "r");
            if (fp == NULL) {
                printf("Error opening cluster2.txt\n");
                exit(-1);
            }

            break;
    }

    // read workers and inform them of their coord
    if (is_coord(rank)) {
        char* line = NULL;
        size_t len = 0;
        getline(&line, &len, fp);

        number_of_workers = atoi(line);
        workers = malloc(sizeof(int) * number_of_workers);
        for (int i = 0; i < number_of_workers; i++) {
            getline(&line, &len, fp);
            workers[i] = atoi(line);
            MPI_Send(&rank, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
            print_msg(rank, workers[i]);
        }
    }

    // workers discover who their coord is
    if (is_worker(rank)) {
        MPI_Recv(&coord, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    }

    // coords send the number of their workers to the other coords
    switch(rank) {
        case COORD0:
            number_of_workers0 = number_of_workers;
            if (!is_error) {
                MPI_Send(&number_of_workers, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
                print_msg(rank, 1);
            }

            MPI_Send(&number_of_workers, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
            print_msg(rank, 2);

            if (!is_error) {
                MPI_Recv(&number_of_workers1, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
            }

            MPI_Recv(&number_of_workers2, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
            if (is_error) {
                MPI_Recv(&number_of_workers1, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
            }

            break;
        
        case COORD1:
            number_of_workers1 = number_of_workers;
            if (!is_error) {
                MPI_Send(&number_of_workers, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
                print_msg(rank, 0);
            }

            MPI_Send(&number_of_workers, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
            print_msg(rank, 2);

            if (!is_error) {
                MPI_Recv(&number_of_workers0, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
            }

            MPI_Recv(&number_of_workers2, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
            if (is_error) {
                MPI_Recv(&number_of_workers0, 1, MPI_INT, 2, 1, MPI_COMM_WORLD, &status);
            }

            break;

        case COORD2:
            number_of_workers2 = number_of_workers;
            MPI_Send(&number_of_workers, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
            print_msg(rank, 0);
            MPI_Send(&number_of_workers, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
            print_msg(rank, 1);

            MPI_Recv(&number_of_workers0, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
            MPI_Recv(&number_of_workers1, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, &status);
            if (is_error) {
                MPI_Send(&number_of_workers0, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
                print_msg(rank, 1);
                MPI_Send(&number_of_workers1, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
                print_msg(rank, 0);
            }

            break;
    }

    // coords send the number of workers in each cluster to their workers
    if (is_coord(rank)) {
        for (int i = 0; i < number_of_workers; i++) {
            MPI_Send(&number_of_workers0, 1, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
            print_msg(rank, workers[i]);
            MPI_Send(&number_of_workers1, 1, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
            print_msg(rank, workers[i]);
            MPI_Send(&number_of_workers2, 1, MPI_INT, workers[i], 2, MPI_COMM_WORLD);
            print_msg(rank, workers[i]);
        }
    }

    // workers receive the number of workers in each cluster
    if (is_worker(rank)) {
        MPI_Recv(&number_of_workers0, 1, MPI_INT, coord, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&number_of_workers1, 1, MPI_INT, coord, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(&number_of_workers2, 1, MPI_INT, coord, 2, MPI_COMM_WORLD, &status);
    }

    // commit workers vectors
    MPI_Type_vector(number_of_workers0, 1, 1, MPI_INT, &workers0_vec);
    MPI_Type_commit(&workers0_vec);
    MPI_Type_vector(number_of_workers1, 1, 1, MPI_INT, &workers1_vec);
    MPI_Type_commit(&workers1_vec);
    MPI_Type_vector(number_of_workers2, 1, 1, MPI_INT, &workers2_vec);
    MPI_Type_commit(&workers2_vec);

    // coords send their worker vectors to other coords
    switch(rank) {
        case COORD0:
            workers0 = workers;
            if (!is_error) {
                MPI_Send(workers, 1, workers0_vec, 1, 3, MPI_COMM_WORLD);
                print_msg(rank, 1);
            }

            MPI_Send(workers, 1, workers0_vec, 2, 3, MPI_COMM_WORLD);
            print_msg(rank, 2);

            workers1 = malloc(sizeof(int) * number_of_workers1);
            if (!is_error) {
                MPI_Recv(workers1, 1, workers1_vec, 1, 3, MPI_COMM_WORLD, &status);
            }

            workers2 = malloc(sizeof(int) * number_of_workers2);
            MPI_Recv(workers2, 1, workers2_vec, 2, 3, MPI_COMM_WORLD, &status);
            if (is_error) {
                MPI_Recv(workers1, 1, workers1_vec, 2, 3, MPI_COMM_WORLD, &status);
            }

            break;
        
        case COORD1:
            workers1 = workers;
            if (!is_error) {
                MPI_Send(workers, 1, workers1_vec, 0, 3, MPI_COMM_WORLD);
                print_msg(rank, 0);
            }

            MPI_Send(workers, 1, workers1_vec, 2, 3, MPI_COMM_WORLD);
            print_msg(rank, 2);

            workers0 = malloc(sizeof(int) * number_of_workers1);
            if (!is_error) {
                MPI_Recv(workers0, 1, workers0_vec, 0, 3, MPI_COMM_WORLD, &status);
            }

            workers2 = malloc(sizeof(int) * number_of_workers2);
            MPI_Recv(workers2, 1, workers2_vec, 2, 3, MPI_COMM_WORLD, &status);
            if (is_error) {
                MPI_Recv(workers0, 1, workers0_vec, 2, 3, MPI_COMM_WORLD, &status);
            }

            break;


        case COORD2:
            workers2 = workers;
            MPI_Send(workers, 1, workers2_vec, 0, 3, MPI_COMM_WORLD);
            print_msg(rank, 0);
            MPI_Send(workers, 1, workers2_vec, 1, 3, MPI_COMM_WORLD);
            print_msg(rank, 1);

            workers0 = malloc(sizeof(int) * number_of_workers1);
            MPI_Recv(workers0, 1, workers0_vec, 0, 3, MPI_COMM_WORLD, &status);
            workers1 = malloc(sizeof(int) * number_of_workers2);
            MPI_Recv(workers1, 1, workers1_vec, 1, 3, MPI_COMM_WORLD, &status);

            if (is_error) {
                MPI_Send(workers0, 1, workers0_vec, 1, 3, MPI_COMM_WORLD);
                print_msg(rank, 1);
                MPI_Send(workers1, 1, workers1_vec, 0, 3, MPI_COMM_WORLD);
                print_msg(rank, 0);
            }

            break;
    }

    // coords print their topology and then send the topology to
    // their workers
    if (is_coord(rank)) {
        print_topology(number_of_workers0, number_of_workers1, number_of_workers2,
                        workers0, workers1, workers2, rank);

        for (int i = 0; i < number_of_workers; i++) {
            MPI_Send(workers0, 1, workers0_vec, workers[i], 4, MPI_COMM_WORLD);
            print_msg(rank, workers[i]);
            MPI_Send(workers1, 1, workers1_vec, workers[i], 4, MPI_COMM_WORLD);
            print_msg(rank, workers[i]);
            MPI_Send(workers2, 1, workers2_vec, workers[i], 4, MPI_COMM_WORLD);
            print_msg(rank, workers[i]);
        }
    }

    // workers get the topology from their coord and then print it
    if (is_worker(rank)) {
        workers0 = malloc(sizeof(int) * number_of_workers1);
        MPI_Recv(workers0, 1, workers0_vec, coord, 4, MPI_COMM_WORLD, &status);
        workers1 = malloc(sizeof(int) * number_of_workers2);
        MPI_Recv(workers1, 1, workers1_vec, coord, 4, MPI_COMM_WORLD, &status);
        workers2 = malloc(sizeof(int) * number_of_workers2);
        MPI_Recv(workers2, 1, workers2_vec, coord, 4, MPI_COMM_WORLD, &status);

        print_topology(number_of_workers0, number_of_workers1, number_of_workers2,
                        workers0, workers1, workers2, rank);
    }

    // commit the 2 possible worker vectors
    int total_number_of_workers = number_of_workers0 + number_of_workers1 + number_of_workers2;
    int iterations_per_worker = vector_dim / total_number_of_workers;
    MPI_Datatype worker_vec;
    MPI_Type_vector(iterations_per_worker, 1, 1, MPI_INT, &worker_vec);
    MPI_Type_commit(&worker_vec);
    MPI_Datatype worker_vec_with_leftover;
    MPI_Type_vector(iterations_per_worker + 1, 1, 1, MPI_INT, &worker_vec_with_leftover);
    MPI_Type_commit(&worker_vec_with_leftover);

    int vector_cluster_dim;
    MPI_Datatype cluster1_vec, cluster2_vec;
    int* vec;

    // coord0 creates the vector and then sends the assigned
    // vectors to coord1 and coord2
    if (rank == COORD0) {
        vec = malloc(sizeof(int) * vector_dim);
        for (int i = 0; i < vector_dim; i++) {
            vec[i] = i;
        }

        // calculate rounded assigned iterations
        vector_cluster_dim = iterations_per_worker * number_of_workers0;
        int vector_cluster_dim1 = iterations_per_worker * number_of_workers1;
        int vector_cluster_dim2 = iterations_per_worker * number_of_workers2;

        // the rest of the iterations are assigned 1 per worker in the limit of
        // leftover iterations
        int leftover_iterations = vector_dim - (vector_cluster_dim + vector_cluster_dim1 + vector_cluster_dim2);
        if (leftover_iterations < number_of_workers) {
            vector_cluster_dim += leftover_iterations;
            leftover_iterations = 0;
        } else {
            vector_cluster_dim += number_of_workers;
            leftover_iterations -= number_of_workers;
        }

        if (leftover_iterations != 0) {
            if (leftover_iterations < number_of_workers1) {
                vector_cluster_dim1 += leftover_iterations;
                leftover_iterations = 0;
            } else {
                vector_cluster_dim1 += number_of_workers1;
                leftover_iterations -= number_of_workers1;
            }
        }

        if (leftover_iterations != 0) {
            vector_cluster_dim2 += leftover_iterations;
        }

        // send the size of the cluster vector to coord 1 and 2
        if (!is_error) {
            MPI_Send(&vector_cluster_dim1, 1, MPI_INT, 1, 5, MPI_COMM_WORLD);
            print_msg(rank, 1);
        }

        MPI_Send(&vector_cluster_dim2, 1, MPI_INT, 2, 5, MPI_COMM_WORLD);
        print_msg(rank, 2);

        MPI_Type_vector(vector_cluster_dim1, 1, 1, MPI_INT, &cluster1_vec);
        MPI_Type_commit(&cluster1_vec);
        MPI_Type_vector(vector_cluster_dim2, 1, 1, MPI_INT, &cluster2_vec);
        MPI_Type_commit(&cluster2_vec);

        // send the assigned vectors to coord 1 and 2
        if (!is_error) {
            MPI_Send(&vec[vector_cluster_dim], 1, cluster1_vec, 1, 6, MPI_COMM_WORLD);
            print_msg(rank, 1);
        }

        MPI_Send(&vec[vector_cluster_dim + vector_cluster_dim1], 1, cluster2_vec, 2, 6, MPI_COMM_WORLD);
        print_msg(rank, 2);

        if (is_error) {
            MPI_Send(&vector_cluster_dim1, 1, MPI_INT, 2, 5, MPI_COMM_WORLD);
            print_msg(rank, 2);
            MPI_Send(&vec[vector_cluster_dim], 1, cluster1_vec, 2, 6, MPI_COMM_WORLD);

            print_msg(rank, 2);
        }
    }

    MPI_Datatype cluster_vec;

    // coord1 receives the cluster vector from coord0
    // if there is an error it recieves it from coord1 instead
    if (rank == COORD1) {
        if (!is_error) {
            MPI_Recv(&vector_cluster_dim, 1, MPI_INT, 0, 5, MPI_COMM_WORLD, &status);
        } else {
            MPI_Recv(&vector_cluster_dim, 1, MPI_INT, 2, 5, MPI_COMM_WORLD, &status);
        }

        MPI_Type_vector(vector_cluster_dim, 1, 1, MPI_INT, &cluster_vec);
        MPI_Type_commit(&cluster_vec);
        vec = malloc(sizeof(int) * vector_cluster_dim);

        if (!is_error) {
            MPI_Recv(vec, 1, cluster_vec, 0, 6, MPI_COMM_WORLD, &status);
        } else {
            MPI_Recv(vec, 1, cluster_vec, 2, 6, MPI_COMM_WORLD, &status);
        }
    }

    int vector_cluster_dim1;
    int* vec1;

    // coord2 receives the cluster vector from coord0
    // if there is an error it will also receive the cluster vector
    // of coord1 from coord0 and will then send it to coord1
    if (rank == COORD2) {
        MPI_Recv(&vector_cluster_dim, 1, MPI_INT, 0, 5, MPI_COMM_WORLD, &status);
        MPI_Type_vector(vector_cluster_dim, 1, 1, MPI_INT, &cluster_vec);
        MPI_Type_commit(&cluster_vec);

        vec = malloc(sizeof(int) * vector_cluster_dim);
        MPI_Recv(vec, 1, cluster_vec, 0, 6, MPI_COMM_WORLD, &status);

        if (is_error) {
            MPI_Recv(&vector_cluster_dim1, 1, MPI_INT, 0, 5, MPI_COMM_WORLD, &status);
            MPI_Type_vector(vector_cluster_dim1, 1, 1, MPI_INT, &cluster1_vec);
            MPI_Type_commit(&cluster1_vec);
            MPI_Send(&vector_cluster_dim1, 1, MPI_INT, 1, 5, MPI_COMM_WORLD);
            print_msg(rank, 1);

            vec1 = malloc(sizeof(int) * vector_cluster_dim1);
            MPI_Recv(vec1, 1, cluster1_vec, 0, 6, MPI_COMM_WORLD, &status);
            MPI_Send(vec1, 1, cluster1_vec, 1, 6, MPI_COMM_WORLD);
            print_msg(rank, 1);
        }
    }

    int leftover_iterations;

    // coords send the vectors of the workers to the workers alongside their size
    if (is_coord(rank)) {
        leftover_iterations = vector_cluster_dim - iterations_per_worker * number_of_workers;
        int worker_index = 0;
        for (int i = 0; i < number_of_workers; i++) {
            int worker_vec_size = iterations_per_worker;
            MPI_Datatype worker_vec_datatype = worker_vec;
            if (i < leftover_iterations) {
                worker_vec_size++;
                worker_vec_datatype = worker_vec_with_leftover;
            }

            MPI_Send(&worker_vec_size, 1, MPI_INT, workers[i], 7, MPI_COMM_WORLD);
            print_msg(rank, workers[i]);
            MPI_Send(&vec[worker_index], 1, worker_vec_datatype, workers[i], 8, MPI_COMM_WORLD);
            print_msg(rank, workers[i]);

            worker_index += worker_vec_size;
        }
    }

    int worker_vec_size;

    // workers get their vectors alongside their size, multiply the
    // elements of the vectors and then send it back to the coord
    if (is_worker(rank)) {
        MPI_Recv(&worker_vec_size, 1, MPI_INT, coord, 7, MPI_COMM_WORLD, &status);
        MPI_Datatype worker_vec_datatype = worker_vec;
        if (worker_vec_size == iterations_per_worker + 1) {
            worker_vec_datatype = worker_vec_with_leftover;
        }

        vec = malloc(sizeof(int) * worker_vec_size);
        MPI_Recv(vec, 1, worker_vec_datatype, coord, 8, MPI_COMM_WORLD, &status);
        for (int i = 0; i < worker_vec_size; i++) {
            vec[i] *= 2;
        }

        MPI_Send(vec, 1, worker_vec_datatype, coord, 9, MPI_COMM_WORLD);
    }

    // coords get the multiplied vectors back from the workers
    if (is_coord(rank)) {
        int worker_index = 0;
        for (int i = 0; i < number_of_workers; i++) {
            MPI_Datatype worker_vec_datatype = worker_vec;
            if (i < leftover_iterations) {
                worker_vec_datatype = worker_vec_with_leftover;
            }

            MPI_Recv(&vec[worker_index], 1, worker_vec_datatype, workers[i], 9, MPI_COMM_WORLD, &status);
            worker_index += status._ucount / sizeof(int);
        }
    }

    // coord1 sends back the multiplied vector to coord0
    // if there is an error it sends it to coord2 instead
    if (rank == COORD1) {
        if (!is_error) {
            MPI_Send(vec, 1, cluster_vec, 0, 10, MPI_COMM_WORLD);
            print_msg(rank, 0);
        } else {
            MPI_Send(vec, 1, cluster_vec, 2, 10, MPI_COMM_WORLD);
            print_msg(rank, 2);
        }
    }

    // coord1 sends back the multiplied vector to coord0
    // if there is an error it will also receive the multiplied
    // vector from coord1 and will send it to coord0
    if (rank == COORD2) {
        if (is_error) {
            MPI_Recv(vec1, 1, cluster1_vec, 1, 10, MPI_COMM_WORLD, &status);
            MPI_Send(vec1, 1, cluster1_vec, 0, 10, MPI_COMM_WORLD);
            print_msg(rank, 0);
            free(vec1);
        }

        MPI_Send(vec, 1, cluster_vec, 0, 10, MPI_COMM_WORLD);
        print_msg(rank, 0);
    }

    // coord0 recieves the multiplied vectors from the other clusters
    // and then prints the resulted vector
    if (rank == COORD0) {
        if (!is_error) {
            MPI_Recv(&vec[vector_cluster_dim], 1, cluster1_vec, 1, 10, MPI_COMM_WORLD, &status);
        } else {
            MPI_Recv(&vec[vector_cluster_dim], 1, cluster1_vec, 2, 10, MPI_COMM_WORLD, &status);
        }

        MPI_Recv(&vec[vector_cluster_dim + status._ucount / sizeof(int)], 1, cluster2_vec, 2, 10, MPI_COMM_WORLD, &status);

        printf("Rezultat:");
        for (int i = 0; i < vector_dim; i++) {
            printf(" %d", vec[i]);
        }

        printf("\n");
    }

    // coords free their worker vectors
    if (is_coord(rank)) {
        free(workers);
        if (rank != COORD0) {
            free(workers0);
        }

        if (rank != COORD1) {
            free(workers1);
        }

        if (rank != COORD2) {
            free(workers2);
        }
    }

    // workers free their worker vectors
    if (is_worker(rank)) {
        free(workers0);
        free(workers1);
        free(workers2);
    }

    // everyone frees their vector
    free(vec);

    MPI_Finalize();
}