#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define N 20
#define MASTER 0

void compareVectors(int * a, int * b) {
	// DO NOT MODIFY
	int i;
	for(i = 0; i < N; i++) {
		if(a[i]!=b[i]) {
			printf("Sorted incorrectly\n");
			return;
		}
	}
	printf("Sorted correctly\n");
}

void displayVector(int * v) {
	// DO NOT MODIFY
	int i;
	int displayWidth = 2 + log10(v[N-1]);
	for(i = 0; i < N; i++) {
		printf("%*i", displayWidth, v[i]);
	}
	printf("\n");
}

int cmp(const void *a, const void *b) {
	// DO NOT MODIFY
	int A = *(int*)a;
	int B = *(int*)b;
	return A-B;
}
 
int main(int argc, char * argv[]) {
	int rank, i, j;
	int nProcesses;
	MPI_Init(&argc, &argv);
	int pos[N];
	int sorted = 0;
	int *v = (int*)malloc(sizeof(int)*N);
	int *vQSort = (int*)malloc(sizeof(int)*N);

	for (i = 0; i < N; i++)
		pos[i] = 0;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
	printf("Hello from %i/%i\n", rank, nProcesses);


	if(rank == MASTER) {
		for (i = 0; i < N; i++) {
			v[i] = random() % 200;
		}

		// DO NOT MODIFY
		displayVector(v);

		// make copy to check it against qsort
		// DO NOT MODIFY
		for(i = 0; i < N; i++)
			vQSort[i] = v[i];
		qsort(vQSort, N, sizeof(int), cmp);

		// sort the vector v
		for (i = 0; i < N; i++) {
			MPI_Send(&v[i], 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
			for (int j = 0; j < N; j++) {
				if (i != j) {
					MPI_Send(&v[j], 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
				}
			}
		}
		
        // recv the new pozitions
		MPI_Status status;
		for (i = 0; i < N; i++) {
			MPI_Recv(&v[i], 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		}

		displayVector(v);
		compareVectors(v, vQSort);
	} else {
		
        // compute the positions
		int number_smaller = 0;
		int elem, recv_value;
		MPI_Status status;

		MPI_Recv(&elem, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		for (int i = 0; i < N - 1; i++) {
			MPI_Recv(&recv_value, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			if (recv_value < elem) {
				number_smaller++;
			}
		}

        // send the new positions to process MASTER
		int can_send = 0;;
		if (number_smaller == 0) {
			can_send = 1;
		} else {
			while(!can_send) {
				int prev_number_smaller;
				MPI_Recv(&prev_number_smaller, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				if (prev_number_smaller + 1 == number_smaller) {
					can_send = 1;
				}
			}
		}

		if (can_send) {
			MPI_Send(&elem, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

			for (i = 1; i < N; i++) {
				MPI_Send(&number_smaller, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			}
		}
	}

	MPI_Finalize();
	return 0;
}
