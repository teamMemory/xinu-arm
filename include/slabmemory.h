/**
 * @file slabmemory.h
 * @author Artur BRAGA
 * Defintions for slab memory management system that wraps the kernel's malloc implementation.
 *
 */
 
#ifndef _SLABMEMORY_H_
#define _SLABMEMORY_H_

#include <stddef.h>


//typedef enum {false,true}bool;
#define NBDATASTR 5
#define NUM_ELEMENTS 100
struct dataNode
{
	void* mem;
	bool isTaken;
	struct dataNode *next;
};

struct dataType
{
	uint size;
	struct dataNode *memList;
};

// accounting operations
void slabInit(uint numEl);
void* slabAlloc(uint elSize);
void slabFree(void* base);

#endif  /*_SLABMEMORY_H_*/
