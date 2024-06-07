#include "dllist.h"
#include "system.h"
#include <stdio.h>

static unsigned int seed = 1204;
void srand (int newseed) {
    seed = (unsigned)newseed & 0x7fffffffU;
}

int rand(void) {
    seed = (seed * 1103515245U + 12345U) & 0x7fffffffU;
    return (int)seed;
}


// insert N items into L
extern void Insert(DLList *L, int N, int whichThread)
{
	for (int i = 0; i < N; ++i)
	{
		int key = rand() % 50;
		L->SortedInsert(NULL, key);
		printf("Thread %d inserts: %d\n", whichThread, key);
	}
}


// remove N items starting from the head of the list
//   and print out the removed items to the console
extern void Remove(DLList *L, int N, int whichThread)
{
	int key;
	int count = N;
	
	while (L->IsEmpty() && count)
	{
		L->Remove(&key);
		--count;
		printf("Thread %d removes: %d\n", whichThread, key);
	}
}
