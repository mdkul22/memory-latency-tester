//#define _GNU_SOURCE
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "cachetime.h"
using namespace std;
///////////// extern declarations ///////////////

uint32_t nThreads;
uint32_t reps;
uint32_t modify;
uint32_t isSequential;
uint32_t lg2Array;

/////////////////////////////////////////////////



void usage(){
	fprintf(stderr, "Usage: ./cachetime <options> \n");
	fprintf(stderr, " Options:\n");
	fprintf(stderr, " --help			Print this message\n");
	fprintf(stderr, " -t 			Threads (default=1)\n");
	fprintf(stderr, " -s 			Sequential Accesses\n");
	fprintf(stderr, " -r			Random Accesses and\n");
	fprintf(stderr, " -m 			Modify\n");
	fprintf(stderr, " --reps 			No. of iterations\n");
	fprintf(stderr, " --log2array 		log2 of data array size\n");
}

int handleOption(char *arg)
{
	if(!strncmp(arg, "-t:", 3)) {
		sscanf(arg+3, "%u", &nThreads);
		//cout<<"Threads: "<<nThreads<<endl;
	} else if(!strncmp(arg, "-s", 2)) {
		isSequential = 1;
		//cout<<"isSequential: "<<isSequential<<endl;
	}  else if(!strncmp(arg, "-r", 2)) {
		isSequential = 0;
		//cout<<"isSequential: "<<isSequential<<endl;
	}   else if(!strncmp(arg, "-m", 2)) {
		modify = 1;
	}	else if(!strncmp(arg, "--log2array:", 12)) {
		sscanf(arg+12, "%u", &lg2Array);
		//cout<<"lg2array: "<<lg2Array<<endl;
	}	else if(!strncmp(arg, "--reps:", 7)) {
		sscanf(arg+7, "%u", &reps);
		//cout<<"reps: "<<reps<<endl;
	}	else{
		return 0;
	}
	return 1;
}

void setDefaults()
{
	isSequential = 0;
	nThreads = 1;
	modify = 0;
	lg2Array = 13;
	reps = 10000;
}

timespec diff(timespec start, timespec end)
{
    timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

void randomize(Node **nodes, uint32_t N)
{
	// shuffling 10 times across array Count for added measure :p
	for(uint32_t i=1; i < N-1; ++i)
	{
		uint32_t swp_id = rand() % (N-1);
		Node *tmp = nodes[swp_id];
		nodes[swp_id] = nodes[i];
		nodes[i] = tmp;
	}
}

int main(int argc, char *argv[])
{
	bool fileEmpty = false;
	ifstream checkFile;
	ofstream writeToFile;
	checkFile.open("latencyLog2.csv");
	if(checkFile.peek() == ifstream::traits_type::eof()) fileEmpty = true;
	checkFile.close();
	if(fileEmpty) {
		writeToFile.open("latencyLog2.csv");
		writeToFile << "size,cores,latency,Sequential,modify\n";
	} else {
		writeToFile.open("latencyLog2.csv", ios::app);
	}
	// Initialization
	//setDefaults();
	// parsing arguments passed from python file
	for(int i=0; i<argc; i++)
	{
		if(!strcmp(argv[i], "--help")) {
			usage();
			exit(0);
		} else if (!strncmp(argv[i], "-", 1)){
			if (!handleOption(argv[i])) {
				usage();
				exit(0);
			}
		} 
	}
	if(nThreads > 0) {
		double* etList = (double*)malloc(nThreads*sizeof(double));
		pthread_t* threadList = (pthread_t*)malloc(nThreads*sizeof(pthread_t));
		for(int i=0; i < nThreads; i++) pthread_create(threadList+i, NULL, testLatency, (void*)(etList+i));
		for(int i=0; i < nThreads; i++) pthread_join(*(threadList+i), NULL);
		free(threadList);
		double accum = 0;
		for(int i=0; i<nThreads; i++) accum += *(etList+i);
		writeToFile<<(1<<lg2Array)<<","<<nThreads<<","<<accum/nThreads<<","<<isSequential<<","<<modify<<"\n";	
		writeToFile.close();
	}
}

void* testLatency(void *ptr)
{
	// reps used to average out memory latency
	uint32_t repCount = reps;
	// got all the values from cmdline
	uint32_t arrCount = (1<<lg2Array); // get array size
	// cout<<"arrCount: "<<arrCount<<endl;
	// using linked list to do memory accesses
	// generating continuous memory to nodes
	Node* memory = (Node*)malloc(arrCount*sizeof(Node));
	// generating node pointers and initializing
	Node **nodes = (Node**)malloc(arrCount*sizeof(Node*));
	// assigning each node pointer to a node
	for(uint32_t i=0; i < arrCount; i++) nodes[i] = &memory[i];
	
	if(!isSequential) randomize(nodes, arrCount);
	// link up them nodes
	for(uint32_t i=0; i < arrCount-1; i++)
	{
		nodes[i]->next = nodes[i+1];
		nodes[i]->data = 0;
	}
	nodes[arrCount-1]->next = NULL;

	Node* firstNode = nodes[0];

	if(modify)
	{
		//timespec time_start, time_end;
		//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_start);
		// reps for getting average latency value
		uint64_t start = clock();
		volatile uint64_t y;
		for(uint32_t i=0; i < repCount; i++) 
		{	
			Node *tempNode = firstNode;
			uint64_t x = tempNode->data ^ 0xF7F7EEEE;
			while(tempNode)
			{  
				x = x ^ 0x7777FFFF;
				tempNode = tempNode->next;
				if(tempNode) tempNode->data = x;
			}
		}
		uint64_t end = clock();	
		//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_end);
		y = firstNode->data;
		free(nodes);
		uint64_t dur = end - start;
		double ns = 1e9 * dur / CLOCKS_PER_SEC;
		//cout<<"size: "<<arrCount<<"\t"<<"latency: "<<(double)(diff(time_start,time_end).tv_nsec/(reps*arrCount))<<" ns"<<endl;
		cout<<"size: "<<arrCount<<"\t"<<"latency: "<<ns/(reps*arrCount)<<" ns"<<endl;
		//cout<<"seconds taken: "<<diff(time_start,time_end).tv_sec<<endl;
		if(ptr) *(double *)ptr = ns/(reps*arrCount);
	} else { 
		timespec time_start, time_end;
		//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_start);
		uint64_t start = clock();
		for(uint32_t i=0; i < repCount; i++) 
		{	
			Node *tempNode = firstNode;
			while(tempNode) tempNode = tempNode->next;
		}
		uint64_t end = clock();			
		//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_end);
		free(nodes);
		uint64_t dur = end - start;
		double ns = 1e9 * dur / CLOCKS_PER_SEC;
		//cout<<"size: "<<arrCount<<"\t"<<"latency: "<<(double)(diff(time_start,time_end).tv_nsec/(reps*arrCount))<<" ns"<<endl;
		cout<<"size: "<<arrCount<<"\t"<<"latency: "<<ns/(reps*arrCount)<<" ns"<<endl;
		//cout<<"seconds taken: "<<diff(time_start,time_end).tv_sec<<endl;
		if(ptr) *(double *)ptr = ns/(reps*arrCount);
	}		
}