#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <search.h>
#include "genetic_algorithm.h"

int read_input(sack_object **objects, int *object_count, int *sack_capacity, int *generations_count, int *num_of_threads, int argc, char *argv[])
{
	FILE *fp;

	if (argc < 4) {
		fprintf(stderr, "Usage:\n\t./tema1 in_file generations_count number_of_threads\n");
		return 0;
	}

	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		return 0;
	}

	if (fscanf(fp, "%d %d", object_count, sack_capacity) < 2) {
		fclose(fp);
		return 0;
	}

	if (*object_count % 10) {
		fclose(fp);
		return 0;
	}

	sack_object *tmp_objects = (sack_object *) calloc(*object_count, sizeof(sack_object));

	for (int i = 0; i < *object_count; ++i) {
		if (fscanf(fp, "%d %d", &tmp_objects[i].profit, &tmp_objects[i].weight) < 2) {
			free(objects);
			fclose(fp);
			return 0;
		}
	}

	fclose(fp);

	*generations_count = (int) strtol(argv[2], NULL, 10);
    *num_of_threads = (int) strtol(argv[3], NULL, 10);
	
	if (*generations_count == 0) {
		free(tmp_objects);

		return 0;
	}

	*objects = tmp_objects;

	return 1;
}

void print_objects(const sack_object *objects, int object_count)
{
	for (int i = 0; i < object_count; ++i) {
		printf("%d %d\n", objects[i].weight, objects[i].profit);
	}
}

void print_generation(const individual *generation, int limit)
{
	for (int i = 0; i < limit; ++i) {
		for (int j = 0; j < generation[i].chromosome_length; ++j) {
			printf("%d ", generation[i].chromosomes[j]);
		}

		printf("\n%d - %d\n", i, generation[i].fitness);
	}
}

void print_best_fitness(const individual *generation)
{
	printf("%d\n", generation[0].fitness);
}

void compute_fitness_function(const sack_object *objects, individual *generation, int start, int end, int sack_capacity)
{
	int weight;
	int profit;

    // the number of objects in the sack of the individual
    int count;

	for (int i = start; i < end; ++i) {
		weight = 0;
		profit = 0;
        count = 0;

		for (int j = 0; j < generation[i].chromosome_length; ++j) {
			if (generation[i].chromosomes[j]) {
				weight += objects[j].weight;
				profit += objects[j].profit;

                // if the chromosome is active it means that the object is in the sack
                count++;
			}
		}

		generation[i].fitness = (weight <= sack_capacity) ? profit : 0;

        // set the calculated count in the individual struct
        generation[i].count = count;
	}
}

int cmpfunc(const void *a, const void *b) {
	individual *first = (individual *) a;
	individual *second = (individual *) b;

	int res = second->fitness - first->fitness; // decreasing by fitness
	if (res == 0) {
		res = first->count - second->count; // increasing by number of objects in the sack
		if (res == 0) {
			return second->index - first->index;
		}
	}

	return res;
}

void mutate_bit_string_1(const individual *ind, int generation_index)
{
	int i, mutation_size;
	int step = 1 + generation_index % (ind->chromosome_length - 2);

	if (ind->index % 2 == 0) {
		// for even-indexed individuals, mutate the first 40% chromosomes by a given step
		mutation_size = ind->chromosome_length * 4 / 10;
		for (i = 0; i < mutation_size; i += step) {
			ind->chromosomes[i] = 1 - ind->chromosomes[i];
		}
	} else {
		// for even-indexed individuals, mutate the last 80% chromosomes by a given step
		mutation_size = ind->chromosome_length * 8 / 10;
		for (i = ind->chromosome_length - mutation_size; i < ind->chromosome_length; i += step) {
			ind->chromosomes[i] = 1 - ind->chromosomes[i];
		}
	}
}

void mutate_bit_string_2(const individual *ind, int generation_index)
{
	int step = 1 + generation_index % (ind->chromosome_length - 2);

	// mutate all chromosomes by a given step
	for (int i = 0; i < ind->chromosome_length; i += step) {
		ind->chromosomes[i] = 1 - ind->chromosomes[i];
	}
}

void crossover(individual *parent1, individual *child1, int generation_index)
{
	individual *parent2 = parent1 + 1;
	individual *child2 = child1 + 1;
	int count = 1 + generation_index % parent1->chromosome_length;

	memcpy(child1->chromosomes, parent1->chromosomes, count * sizeof(int));
	memcpy(child1->chromosomes + count, parent2->chromosomes + count, (parent1->chromosome_length - count) * sizeof(int));

	memcpy(child2->chromosomes, parent2->chromosomes, count * sizeof(int));
	memcpy(child2->chromosomes + count, parent1->chromosomes + count, (parent1->chromosome_length - count) * sizeof(int));
}

void copy_individual(const individual *from, const individual *to)
{
	memcpy(to->chromosomes, from->chromosomes, from->chromosome_length * sizeof(int));
}

void free_generation(individual *generation, int object_count)
{
	int i;

	for (i = 0; i < object_count; ++i) {
		free(generation[i].chromosomes);
		generation[i].chromosomes = NULL;
		generation[i].fitness = 0;
	}
}

/**
 * Struct contains the start, middle and end used in the merge function during mergesort
 */
typedef struct merge_struct {
    int start;
    int middle;
    int end;
} MergeStruct;

/**
 * Struct contains:
 * -the merge matrix used during each iteration of mergesort
 * -the number of stages in the mergesort algorithm corresponding to the number of objects
 * -the length of each stage which is equal to the number of merges done during the stage
 */
typedef struct merge_sort_matrix {
    MergeStruct** matrix;
    int number_of_stages;
    int* stage_length;
} MergeSortMatrix;

/**
 * Struct send as an argument to a thread
 */
typedef struct pthread_arg {
	const sack_object *objects;
	int object_count;
	int generations_count;
	int sack_capacity;
	int thread_id;
	int num_of_threads;

    // I send the generations as double pointers to have the interchange happen on all threads
	individual **current_generation;
	individual **next_generation;

	pthread_barrier_t *barrier;
    MergeSortMatrix* merge_matrix;
} PthreadArg;

int min(int a, int b) {
	if (a < b) {
		return a;
	}

	return b;
}

/**
 * Calculates the start of a for that needs to be run on a thread based on its id
 * @param thread_id
 * @param dim   the number of iterations
 * @param num_of_threads
 * @return  the start of the for
 */
int calculate_start(int thread_id, int dim, int num_of_threads) {
	return thread_id * ceil((double) dim / num_of_threads);
}

/**
 * Calculates the end of a for that needs to be run on a thread based on its id
 * @param thread_id
 * @param dim   the number of iterations
 * @param num_of_threads
 * @return  the end of the for
 */
int calculate_end(int thread_id, int dim, int num_of_threads) {
	return min((thread_id + 1) * ceil((double) dim / num_of_threads), dim);
}

/**
 * Calls pthread_barrier_wait and checks its result for errors
 * @param barrier   the barrier on which pthread_barrier_wait is called
 */
void barrier_wait(pthread_barrier_t *barrier) {
	int r = pthread_barrier_wait(barrier);
	if (r != 0 && r != PTHREAD_BARRIER_SERIAL_THREAD) {
		printf("barrier wait err\n");
		exit(-1);
	}
}

/**
 * The merge algorithm taken from the skel of lab 3
 * @param source    the source of the subarrays to be merged
 * @param start     the start of the first subarray
 * @param mid       the end of the first subarray and start of the second
 * @param end       the end of the second subarray
 * @param destination   the destination in which the result of the merged subarray is written
 */
void merge(individual *source, int start, int mid, int end, individual *destination) {
    int iA = start;
    int iB = mid;
    int i;

    for (i = start; i < end; i++) {
        if (end == iB || (iA < mid && cmpfunc(&source[iA], &source[iB]) <= 0)) {
            // both fitness and count need to be copied because they're used in cmpfunc during the next
            // stages in mergesort
            copy_individual(&source[iA], &destination[i]);
            destination[i].fitness = source[iA].fitness;
            destination[i].count = source[iA].count;
            iA++;
        } else {
            copy_individual(&source[iB], &destination[i]);
            destination[i].fitness = source[iB].fitness;
            destination[i].count = source[iB].count;
            iB++;
        }
    }

    // put the two sorted combined subarrays back in the source
    for (i = start; i < end; i++) {
        copy_individual(&destination[i], &source[i]);
        source[i].fitness = destination[i].fitness;
        source[i].count = destination[i].count;
    }
}

/**
 * A bottom up implementation of the mergesort algorithm
 * @param merge_matrix  contains the merges needed to be done during all stages
 * @param current_generation    the generation that needs to be sorted
 * @param new_generation    the generation used as temporary storage destination in the merge function
 * @param barrier
 * @param thread_id
 * @param num_of_threads
 */
void mergesort_par(MergeSortMatrix* merge_matrix, individual** current_generation, individual** new_generation, pthread_barrier_t *barrier, int thread_id, int num_of_threads) {
    // goes from the last stage to the first (bottom up)
    // each thread does the merges, corresponding to the merge structs, assigned to them during a stage
    for (int stage = merge_matrix->number_of_stages - 1; stage >= 0; stage--) {
        int start = calculate_start(thread_id, merge_matrix->stage_length[stage], num_of_threads);
        int end = calculate_end(thread_id, merge_matrix->stage_length[stage], num_of_threads);
        for (int i = start; i < end; i++) {
            int start_merge = merge_matrix->matrix[stage][i].start;
            int middle_merge = merge_matrix->matrix[stage][i].middle;
            int end_merge = merge_matrix->matrix[stage][i].end;
            merge(*current_generation, start_merge, middle_merge, end_merge, *new_generation);
        }

        // waits for all barriers to do all their assigned merges during the current stage
        barrier_wait(barrier);
    }
}

void* thread_function(void* arg) {
    // takes all arguments from the struct argument and stores them or their pointers in local variables
	PthreadArg *arg_struct = (PthreadArg*) arg;
	const sack_object *objects = arg_struct->objects;
	int generations_count = arg_struct->generations_count;
	int sack_capacity = arg_struct->sack_capacity;

	int thread_id = arg_struct->thread_id;
	int num_of_threads = arg_struct->num_of_threads;
	pthread_barrier_t *barrier = arg_struct->barrier;
    int object_count = arg_struct->object_count;
    MergeSortMatrix *merge_matrix = arg_struct->merge_matrix;

	individual **current_generation = arg_struct->current_generation;
	individual **next_generation = arg_struct->next_generation;
    int start = calculate_start(thread_id, object_count, num_of_threads);
    int end = calculate_end(thread_id, object_count, num_of_threads);

    // set initial generation (composed of object_count individuals with a single item in the sack)
	for (int i = start; i < end; ++i) {
        (*current_generation)[i].fitness = 0;
        (*current_generation)[i].chromosomes = (int*) calloc(object_count, sizeof(int));
        (*current_generation)[i].chromosome_length = object_count;
        (*current_generation)[i].index = i;
        (*current_generation)[i].chromosomes[i] = 1;

        (*next_generation)[i].fitness = 0;
        (*next_generation)[i].chromosomes = (int*) calloc(object_count, sizeof(int));
        (*next_generation)[i].index = i;
        (*next_generation)[i].chromosome_length = object_count;
	}

	barrier_wait(barrier);

    // iterate for each generation
	for (int k = 0; k < generations_count; ++k) {
		int cursor, count, partial_start, partial_end;

        // compute fitness for the current generation
        compute_fitness_function(objects, *current_generation, start, end, sack_capacity);
		barrier_wait(barrier);

        // sort the current generation by fitness
        mergesort_par(merge_matrix, current_generation, next_generation, barrier, thread_id, num_of_threads);
		barrier_wait(barrier);

        // keep first 30% children (elite children selection)
		count = object_count * 3 / 10;
		partial_start = calculate_start(thread_id, count, num_of_threads);
		partial_end = calculate_end(thread_id, count, num_of_threads);
        for (int i = partial_start; i < partial_end; ++i) {
            copy_individual(*current_generation + i, *next_generation + i);
        }

        // mutate first 20% children with the first version of bit string mutation
		cursor = count;
		count = object_count * 2 / 10;
		partial_start = calculate_start(thread_id, count, num_of_threads);
		partial_end = calculate_end(thread_id, count, num_of_threads);
        for (int i = partial_start; i < partial_end; ++i) {
            copy_individual(*current_generation + i, *next_generation + cursor + i);
            mutate_bit_string_1(*next_generation + cursor + i, k);
        }

        // mutate next 20% children with the second version of bit string mutation
		cursor += count;
        for (int i = partial_start; i < partial_end; ++i) {
            copy_individual(*current_generation + i + count, *next_generation + cursor + i);
            mutate_bit_string_2(*next_generation + cursor + i, k);
        }

        // crossover first 30% parents with one-point crossover
        // (if there is an odd number of parents, the last one is kept as such)
		cursor += count;
		count = object_count * 3 / 10;
		if (count % 2 == 1) {
			if (thread_id == num_of_threads - 1) {
				copy_individual(*current_generation + object_count - 1, *next_generation + cursor + count - 1);
			}

			count--;
		}

		partial_start = calculate_start(thread_id, count, num_of_threads);
		partial_end = calculate_end(thread_id, count, num_of_threads);
        for (int i = partial_start; i < partial_end; ++i) {
            if (i % 2 == 0) {
                crossover(*current_generation + i, *next_generation + cursor + i, k);
            }
        }

        // interchange the current and next generation
		barrier_wait(barrier);
        if (thread_id == num_of_threads - 1) {
            individual *tmp = NULL;
            tmp = *current_generation;
            *current_generation = *next_generation;
            *next_generation = tmp;
        }

		barrier_wait(barrier);
        for (int i = start; i < end; ++i) {
            (*current_generation)[i].index = i;
        }

		barrier_wait(barrier);

        // only the last thread prints the best fitness
		if (thread_id == num_of_threads - 1 && k % 5 == 0) {
            // I also copy the fitness function during merge therefore I need to clear the fitness
            // for the first print to be the same as in the sequential algorithm
            if (k == 0) {
                for (int i = 0; i < object_count; i++) {
                    (*current_generation)->fitness = 0;
                }
            }

			print_best_fitness(*current_generation);
		}
        barrier_wait(barrier);
	}

	compute_fitness_function(objects, *current_generation, start, end, sack_capacity);
    barrier_wait(barrier);
    mergesort_par(merge_matrix, current_generation, next_generation, barrier, thread_id, num_of_threads);
    barrier_wait(barrier);
	if (thread_id == num_of_threads - 1) {
		print_best_fitness(*current_generation);
	}

	pthread_exit(NULL);
}

/**
 * Creates the merge matrix used during each iteration of mergesort
 * @param object_count
 * @return
 */
MergeSortMatrix* create_merge_sort_matrix(int object_count) {
    MergeSortMatrix* merge_matrix = malloc(sizeof(MergeSortMatrix));
    merge_matrix->number_of_stages = ((int) log2(object_count)) + 1;
    merge_matrix->matrix = (MergeStruct **) malloc(sizeof(MergeStruct *) * merge_matrix->number_of_stages);
    merge_matrix->stage_length = (int*) malloc(sizeof(int) * merge_matrix->number_of_stages);

    // create the first stage and the only merge struct of it
    merge_matrix->stage_length[0] = 1;
    merge_matrix->matrix[0] = (MergeStruct*) malloc(sizeof(MergeStruct));
    merge_matrix->matrix[0][0].start = 0;
    merge_matrix->matrix[0][0].end = object_count;
    merge_matrix->matrix[0][0].middle = object_count / 2;

    // for the rest of the stages I create the merge structs based upon the merge structs of their previous stage
    for (int stage = 1; stage < merge_matrix->number_of_stages; stage++) {
        merge_matrix->matrix[stage] = malloc(sizeof(MergeStruct) * merge_matrix->stage_length[stage - 1] * 2);
        merge_matrix->stage_length[stage] = 0;

        // break the merge structs of the previous stage in half based upon its middle and add the 2 resulted merge
        // structs to the current stage if they encompass more than one element
        for (int j = 0; j < merge_matrix->stage_length[stage - 1]; j++) {
            int start = merge_matrix->matrix[stage - 1][j].start;
            int end = merge_matrix->matrix[stage - 1][j].end;
            int middle = merge_matrix->matrix[stage - 1][j].middle;
            if (middle - start > 1) {
                merge_matrix->matrix[stage][merge_matrix->stage_length[stage]].start = start;
                merge_matrix->matrix[stage][merge_matrix->stage_length[stage]].end = middle;
                merge_matrix->matrix[stage][merge_matrix->stage_length[stage]].middle = start + (middle - start) / 2;
                merge_matrix->stage_length[stage]++;
            }

            if (end - middle > 1) {
                merge_matrix->matrix[stage][merge_matrix->stage_length[stage]].start = middle;
                merge_matrix->matrix[stage][merge_matrix->stage_length[stage]].end = end;
                merge_matrix->matrix[stage][merge_matrix->stage_length[stage]].middle = middle + (end - middle) / 2;
                merge_matrix->stage_length[stage]++;
            }
        }
    }

    return merge_matrix;
}

void run_genetic_algorithm(const sack_object *objects, int object_count, int generations_count, int sack_capacity, int num_of_threads) {
	individual *current_generation = (individual*) calloc(object_count, sizeof(individual));
	individual *next_generation = (individual*) calloc(object_count, sizeof(individual));

    // the pointers to the current and next generations are used for interchanging the two
    // generations across all threads
    individual **current_generation_ptr = (individual **) malloc(sizeof(individual*));
    individual **next_generation_ptr = (individual **) malloc(sizeof(individual*));
    current_generation_ptr = &current_generation;
    next_generation_ptr = &next_generation;

    // initialize barrier
	pthread_barrier_t *barrier = malloc(sizeof(pthread_barrier_t));
	int r = pthread_barrier_init(barrier, NULL, num_of_threads);
	if (r != 0) {
		printf("barrier init err\n");
		exit(-1);
	}

    pthread_t threads[num_of_threads];
    PthreadArg** structures = (PthreadArg**) calloc(num_of_threads, sizeof(PthreadArg*));
    MergeSortMatrix* merge_matrix = create_merge_sort_matrix(object_count);

    // create the threads
    // alloc, construct and send the required argument
	for (int id = 0; id < num_of_threads; id++) {
        structures[id] = (PthreadArg*) malloc(sizeof(PthreadArg));
        structures[id]->thread_id = id;
        structures[id]->current_generation = current_generation_ptr;
        structures[id]->generations_count = generations_count;
        structures[id]->next_generation = next_generation_ptr;
        structures[id]->num_of_threads = num_of_threads;
        structures[id]->object_count = object_count;
        structures[id]->objects = objects;
        structures[id]->sack_capacity = sack_capacity;
        structures[id]->barrier = barrier;
        structures[id]->merge_matrix = merge_matrix;

        r = pthread_create(&threads[id], NULL, thread_function, (void *)structures[id]);
		if (r) {
	  		printf("thread %d create err\n", id);
	  		exit(-1);
		}
  	}

    // join the threads
	for (int id = 0; id < num_of_threads; id++) {
		r = pthread_join(threads[id], NULL);
		if (r) {
	  		printf("thread %d join err\n", id);
	  		exit(-1);
		}
	}

    // destroy the barrier
	r = pthread_barrier_destroy(barrier);
	if (r != 0) {
		printf("barrier destroy err\n");
		exit(-1);
	}

    // free the barrier
    free(barrier);

    // free the structures used as arguments for the threads
    for (int id = 0; id < num_of_threads; id++) {
        free(structures[id]);
    }

    // free the array of structures
    free(structures);

	// free resources for old generation
	free_generation(current_generation, object_count);
	free_generation(next_generation, object_count);

	// free resources
	free(current_generation);
	free(next_generation);
}