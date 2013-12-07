/**
 * @file buddymemory.h
 * Definitions for buddy memory management system that wraps the kernel's malloc implementation.
 */
 
#ifndef _BUDDYMEMORY_H_
#define _BUDDYMEMORY_H_

#include <memory.h>
#include <stdio.h>
#include <stddef.h>
#include "MemFrag.h"

#define BUDDY_PAGE_SIZE 1024
#define MEMORY_DEPTH_REQUEST_ERROR -1	// Depth request error

#define MAX_DEPTH_OFFSET 2		// Max depth = max possible depth of page size - offset


struct buddynode
{
        int isUsed;
	struct buddynode* leftNode;
	struct buddynode* rightNode;
	void* memRegion;
	int depth;
        uint allocationSize;
};

// malloc and free operations
void* buddyMalloc(uint numBytes);
void buddyDealloc();

// accounting operations
void buddyInit(uint buddyPageSize);
void* buddyAlloc(uint numBytes);
void buddyFree(void* base);


void buddyAllocNodePool(uint maxDepth);
struct buddynode* buddyNodeFromPool(uint offsetFromNext = 0);
bool buddySplit(struct buddynode* node);
struct buddynode* buddyBestFit(uint desiredDepth, struct buddynode* node);
MemFrag buddyFragmentationAmount();
MemFrag buddyFragmentationAmount2(struct buddynode* node);

#endif  /*_BUDDYMEMORY_H_*/