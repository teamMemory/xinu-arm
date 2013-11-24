/**
 * @file buddymemory.h
 * @author Blake Gross
 * Defintions for buddy memory management system that wraps the kernel's malloc implementation.
 *
 */
 
#ifndef _BUDDYMEMORY_H_
#define _BUDDYMEMORY_H_

#include <stddef.h>

#define BUDDY_PAGE_SIZE 1024

struct buddynode
{
	int isFull;
	struct buddynode* leftNode;
	struct buddynode* rightNode;
	void* memRegion;
	int depth;
};

// malloc and free operations
void* buddyMalloc(uint numBytes);
void buddyFree(void* base);

// accounting operations
void buddyInit(void);
void* buddyAlloc(uint numBytes);
void buddyFree(void* base);

#endif  /*_BUDDYMEMORY_H_*/