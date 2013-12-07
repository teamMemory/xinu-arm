/**
 * @file slabAllocater.c
 * @author Artur Braga; Blake Gross
 * Manages the memory that is allocated
 *
 */

#include <slabmemory.h>
#include <stdlib.h>
#include <stdio.h>

struct SlabCacheList *cacheHead;
void* memBlock;
void slabInit()
{
	//alocating the memory pull
	memBlock = malloc( TOTAL_MEMORY );
	
	//usedMem node is the first to be in the memory
	usedMem=(struct MemRange *)memBlock ;
	usedMem->pPrev=NULL;
	usedMem->pNext=NULL;
	usedMem->base=memBlock;
	//a node of usedMem + node of freeMem+ node of  the first cache
	usedMem->nbBytes=2*sizeof(struct MemRange)+sizeof(struct SlabCacheList);
	//usedMem->nbPages=1; //////////////To define!!!!!!!!!!!!!!!!!!!!///////////
	usedMem->pSlab=NULL;
	
	//alocating freeMem just after the used mem node
	freeMem=(struct MemRange *)((char *)memBlock+sizeof(struct MemRange));                       ////////////////////////////////Workaround////////////////////////////////
	freeMem->pPrev=NULL;
	freeMem->pNext=NULL;
	freeMem->base=(void * )((char *)usedMem->base+usedMem->nbBytes);
	freeMem->nbBytes=TOTAL_MEMORY-usedMem->nbBytes;
	//freeMem->nbPages=0; //////////////To define!!!!!!!!!!!!!!!!!!!!///////////
	freeMem->pSlab=NULL;
	

	cacheHead=(struct SlabCacheList *)((char * ) memBlock+ 2*sizeof(struct MemRange));
	cacheHead->pNext=NULL;
	cacheHead->pPrev=NULL;
	cacheHead->allocSize=-1;
	cacheHead->freeObj=-1;
	cacheHead->slabObjCnt=-1;
	cacheHead->objSize=-1;
}
struct SlabCacheList *createCache(uint objSize)
{
	struct SlabCacheList *lastCache;
	struct SlabCacheList *cacheEl;
	struct Slab *slabEl;
	struct MemRange *current,*usedrange;
	uint size;
	size=sizeof(struct SlabCacheList)+sizeof(struct Slab)+BUFFER;
	current=freeMem;
	
	//Going through the list of free memory chunks
	while( current->pNext != NULL )
	{
		if( current->nbBytes>= size)
		{
			break;
		}
		current = current->pNext;
	}
	if( current->nbBytes< size) 
		{
			printf("There is not enough memory to create the cache!!!\n");
	}else{
		lastCache=cacheHead;
		while(lastCache->pNext !=NULL) lastCache=lastCache->pNext;
		cacheEl=(struct SlabCacheList *)current->base;
		lastCache->pNext=cacheEl;
		cacheEl->pPrev=lastCache;
		cacheEl->pNext=NULL;
		cacheEl->objSize=objSize;
		cacheEl->allocSize=objSize;//alignMemory(objSize);
		cacheEl->slabObjCnt=(uint)(BUFFER/(sizeof(struct BufferList)+objSize));
		cacheEl->freeObj=cacheEl->slabObjCnt;


		slabEl=(struct Slab *)(char *)current->base+sizeof(struct SlabCacheList);
		cacheEl->pSlabList=slabEl;
		slabEl->pPrev=NULL;
		slabEl->pNext=NULL;
		slabEl->pCache=cacheEl;
		slabEl->pRange=current;
		

		slabEl->firstObj=(void *)((char *)current->base+sizeof(struct SlabCacheList)+sizeof(struct Slab));
		slabEl->pFree=createBuffer(slabEl->firstObj,objSize);
		slabEl->nbFree=cacheEl->slabObjCnt;

		usedrange=usedMem;
		while( usedrange->pNext != NULL )usedrange=usedrange->pNext;
		
		
		if(current->nbBytes-size>sizeof(struct MemRange))
		{
			struct MemRange *freeNode;
			freeNode=(struct MemRange *)((char *)current->base+size);
			freeNode->base=(char *)current->base+size;
			freeNode->nbBytes=current->nbBytes-size;
			freeNode->pSlab=NULL;
			freeNode->pNext=NULL;
			freeNode->pPrev=current->pPrev;

			if (current->pPrev!=NULL)current->pPrev->pNext=freeNode;
			

			current->pPrev=usedrange;
			usedrange->pNext=current;
			current->pNext=NULL;
			current->nbBytes=size;
			current->pSlab=slabEl;
		}

		//Unmap the used memory from the free list
		usedrange->pNext=current;


	}
	return cacheEl;
}
struct BufferList* createBuffer(void *base,uint objSize)
{
	void * addrSpace;
	int i;
	struct BufferList* prev; 
	uint nbrObjects = (int)(BUFFER/(sizeof(struct BufferList)+objSize));
	addrSpace=base;
	prev=(struct BufferList *)addrSpace;
	
	prev->pPrev=NULL;
	prev->pObject=(void *)((char*)addrSpace+(sizeof(struct BufferList)));
	addrSpace=(void *)((char *)prev->pObject+objSize);

	for( i=1;i<nbrObjects;++i)
	{
		struct BufferList* temp =(struct BufferList *)addrSpace;
		prev->pNext=temp;
		temp->pPrev=prev;
		temp->pObject=(void *)((char*)addrSpace+(sizeof(struct BufferList)));
		addrSpace=(void *)((char *)prev->pObject+objSize);
		prev=temp;
	}
	prev->pNext=(struct BufferList *)base;
	prev->pNext->pPrev=prev;
	return prev->pNext;
}
/*void slabInit()
{
	memBlock = malloc( TOTAL_MEMORY );
}*/

void* slabMalloc(uint elSize)
{
	int i,j;
	struct SlabCacheList* cacheToUse = cacheHead;
	while( cacheHead != NULL )
	{
		if( cacheToUse->objSize == elSize )
		{
			break;
		}
		cacheHead = cacheHead->pNext;
	}
	
	if( cacheToUse == NULL )
	{
		cacheToUse = createCache(elSize);
	}
	
	struct Slab* currentSlab = cacheHead->pSlabList;	// need to get way of getting this....
	
	
	void* memoryToReturn = NULL;
	while( currentSlab != NULL )
	{
		if( currentSlab-> NbFree > 0 )	// if the slab has free space, we can allocate in it
		{
			struct BufferList* headBuffer = currentSlab->pFree;
			memoryToReturn = headBuffer->pObject;		// first object on list is always going to be free, now we have to take if off the list (works like a queue)

			if( headBuffer->pNext != NULL )
			{
				headBuffer->pNext->pPrev = headBuffer->pPrev;	// reordering the list
				headBuffer->pNext = NULL;
				headBuffer->pPrev = NULL;
			}
			currentSlab->pFree = headBuffer->pNext;
			
			--NbFree;
			break;
		}
		
		currentSlab = currentSlab->pNext;
	}

	// for now just returning rootnodes memory region
	//return rootNode->memRegion;
	return memoryToReturn;
}

/*struct SlabCacheList *createCache(uint size)
{
	if( memBlock == NULL )
	{
		slabInit();
	}
	
	return 	newCache = (struct SlabCacheList*)(&memBlock->prev + sizeof(struct SlabCacheList);	// creates new cache from memory
}*/

//http://stackoverflow.com/questions/227897/solve-the-memory-alignment-in-c-interview-question-that-stumped-me
int alignAmount( int size )
{
	return size + 4 & ~0x03;
}