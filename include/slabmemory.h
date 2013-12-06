/**
 * @file slabmemory.h
 * @author Artur Braga; Blake Gross
 * Defintions for slab memory management system that wraps the kernel's malloc implementation.
 *
 */
 
#ifndef _SLABMEMORY_H_
#define _SLABMEMORY_H_

#include <stddef.h>


//typedef enum {false,true}bool;
#define TOTAL_MEMORY 10240	// 10 kb

// structure which will be used to keep track of free memory areas and occupied memory areas
struct MemRange
{
	struct MemRange *pNext;
	struct MemRange *pPrev;
	uint NbPages;
	struct Slab *pSlab;
}
// Cache Structure containing the list of slabs
struct SlabCacheList
{
	struct SlabCacheList *pPrev;
	struct SlabCacheList *pNext;
	uint objSize;
	uint allocSize;
	uint slabObjCnt;
	uint freeObj;
	struct Slab *pSlabList;
}
//Slab structure containing the list of buffers
struct Slab
{
	struct Slab *pPrev;
	struct Slab *pNext;
	uint NbFree;
	struct BufferList *pFree;
	void *firstObj; //memory address of the first element in the slab
	struct MemRange *pRange;
	struct SlabCacheList *pCache;
}
//List of actual objects contained in the Slab
struct BufferList
{
	struct BufferList *pPrev;
	struct BufferList *pNext;
	void *pObject;
}

/*struct dataNode
{
	void* mem;
	bool isTaken;
	struct dataNode *next;
};

struct dataType
{
	uint size;
	struct dataNode *memList;
};*/

// accounting operations
void slabInit(uint numEl);
struct SlabCacheList *createCache(uint size);
uint cacheDestroy(struct SlabCacheList *pCache);

void* slabAlloc(uint elSize);
void slabFree(void* base);

#endif  /*_SLABMEMORY_H_*/
