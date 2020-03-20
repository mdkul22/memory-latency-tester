#ifndef CACHETIME_H
#define CACHETIME_H

#include <stdint.h>
#include <stdlib.h>

extern uint32_t 	nThreads;
extern uint32_t 	reps;
extern uint32_t		modify;
extern uint32_t		isSequential;
extern uint32_t		lg2Array;

typedef struct Node Node;
struct Node {
	uint64_t data; 
	Node *next; 
};
// Provides help details to the user when needed
void usage();
int handleOption(char *arg);
void setDefaults();
timespec diff(timespec start, timespec end);
void randomize(Node **, uint32_t N);
void* testLatency(void *ptr);
#endif