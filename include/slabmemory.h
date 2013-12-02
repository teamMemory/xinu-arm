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

struct dataNode
{
	void* mem;
	bool isfree;
	struct dataNode *next;
};

struct dataType
{
	int size;
	struct dataNode *memList;
};

struct dataType *dataCache;

// malloc and free operations
void* slabMalloc(uint numBytes);
void slabFree(void* base);

// accounting operations
void slabInit(uint numEl);
void* slabAlloc(uint numBytes);
void slabFree(void* base);

#endif  /*_SLABMEMORY_H_*/
