#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

// structure for an individual in the genetic algorithm; the chromosomes are an array corresponding to each sack
// object, in order, where 1 means that the object is in the sack, 0 that it is not
typedef struct _individual {
	int fitness;
	int *chromosomes;
    int chromosome_length;
	int index;

    // counts the number of objects in the sack of the individual (equivalent to the number of active
    // chromosome bits)
    int count;
} individual;

#endif