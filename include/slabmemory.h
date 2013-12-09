/**
 * @file slabmemory.h
 * @author Artur Braga; Blake Gross
 * Defintions for slab memory management system that wraps the kernel's malloc implementation.
 *
 */
 
#ifndef _SLABMEMORY_H_
#define _SLABMEMORY_H_

#include <stddef.h>
#include <memFrag.h>

//typedef enum {false,true}bool;
#define TOTAL_MEMORY 10240	// 10 kb
#define PAGE_SIZE 1024
#define BUFFER 256
// structure which will be used to keep track of free memory areas and occupied memory areas
// structure which will be used to keep track of free memory areas and occupied memory areas
struct MemRange
{
	struct MemRange *pNext;
	struct MemRange *pPrev;
	void * base;
	uint nbBytes;
	//uint nbPages;
	struct Slab *pSlab;
};

// Cache Structure containing the list of slabs
struct SlabCacheList
{
	struct SlabCacheList *pPrev;
	struct SlabCacheList *pNext;
	uint objSize; // size of the object requested by the user
	uint allocSize; //size really allocated by an object
	uint slabObjCnt; // number of objects allocated by slab
	uint freeObj; // number of free  objects 
	//uint slabPagesCnt; //number of allocated pages by slab
	struct Slab *pSlabList;
};
//Slab structure containing the list of buffers
struct Slab
{
	struct Slab *pPrev;
	struct Slab *pNext;
	uint nbFree;
	uint nbTotal;
	struct BufferList *pFree;
	void *firstObj; //memory address of the first element in the slab
	struct MemRange *pRange;
	struct SlabCacheList *pCache;
};
//List of actual objects contained in the Slab
struct BufferList
{
	struct BufferList *pPrev;
	struct BufferList *pNext;
	void *pObject;
};

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
void slabInit();
struct SlabCacheList *createCache(uint objSize);
uint cacheDestroy(struct SlabCacheList *pCache);
void *CacheAlloc(struct SlabCacheList * pCache);
uint cacheFree(void *addr);
void* slabAlloc(uint elSize);
uint slabFree(void* base);
uint alignMemory(uint objSize);
struct BufferList* createBuffer(void *base,uint objSize);
struct Slab * createNewSlab(struct  SlabCacheList *cache);
uint slabDestroy(struct Slab* slab);
void slabCleanup();

// Fragmentation Checking Functions
struct MemFragFloat* calculateFragmentation();
int calculateInternalFragmentation();
int calculateExternalFragmentation();
#endif  /*_SLABMEMORY_H_*/
