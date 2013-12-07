/*
*	linkedList Memory Allocator
*	@file linkedListMalloc.c
*	@author Joe Cuffney
*	@author Paul Aman
*/

/////////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////////


#include "linkedListMalloc.h"

/////////////////////////////////////////////////////
/// Globals
/////////////////////////////////////////////////////


//struct Node * root;	//will always point to the root node
//struct Node * current;//will point to varying nodes as we traverse

void * root;		//will always point to the root node

int init = FALSE;	//states whether the allocator has been initialized.

/////////////////////////////////////////////////////
/// Functions
/////////////////////////////////////////////////////

/*
*	Initializes the root node and sets the "current" Node
*	to that root
*
*	The strucure will be as follows:
*	|STUCT NODE|USER MEMORY|STRUCT NODE|USER MEMORY|...|
*
*/
void * initList(unsigned int userBytes)
{
	
	int nodeSize, totalSize;
	struct Node * firstNode;
	//allocate a pool of memory
	root = malloc(POOL_SIZE);
	
	nodeSize = sizeof(struct Node);
	if (userBytes + nodeSize > POOL_SIZE)
	{
		printf("Block too large to allocate.");
		return NULL;
	}
	printf("	Root Address: %p\n" , root);
	printf("	Node Size: %i\n" , nodeSize);
	printf("	USER MEM SIZE: %i\n" , userBytes);
	totalSize = nodeSize + userBytes;
	
	firstNode = (struct Node *) root;

	firstNode->mem = firstNode + nodeSize;
	printf("	USER MEM ADDRESS: %p\n" , firstNode->mem);

	firstNode->next = NULL;
	firstNode->lenUsed = userBytes;
	firstNode->lenAvail = userBytes;
	firstNode->taken = TRUE;

	//return the root location plus the offset of the struct
	//to get the user's memory location
	return firstNode->mem;
}


/*
*	Inserts a node at the end of the linked list 
*/
void * insertNode(unsigned int userBytes)
{
	int sizeStruct, poolBytesUsed, sizeNeeded;
	struct Node * newNode, * cur;
	cur = (struct Node *)root;
	sizeStruct = sizeof(struct Node);
	poolBytesUsed = 0;
	while(cur->next != NULL)
	{
		//cast cur to (struct *)?
		
		if(!cur->taken && cur->lenAvail >= userBytes)
		{
			cur->lenUsed = userBytes;
			cur->taken = TRUE;
			return cur->mem;
		}
		else
		{
			poolBytesUsed += (sizeStruct + cur->lenAvail);
			cur = cur->next;
		}
	}
	//cur points to the last existing node
	sizeNeeded = userBytes + sizeStruct;
	//only allocate a new Node if we have the space to do so
	if((sizeNeeded + poolBytesUsed) < POOL_SIZE)
	{
		cur->next = cur + sizeNeeded;
		newNode = cur->next;
		newNode->next = NULL;
		newNode->taken = TRUE;
		newNode->lenUsed = userBytes;
		newNode->lenAvail = userBytes;
		newNode->mem = cur + sizeof(struct Node);
		
		return newNode->mem;
	}
	else
	{
		//in this case we can't allocate the memory because
		//we don't have sufficient space in the pool.
		printf("Block too large to allocate.");
		return NULL;
	}

}


/*
*	This Function takes in a pointer to a location in memory
*	it then has to that structures location free
*/
void removeNode(void * loc)
{
	struct Node * curr = (struct Node *)root;
	while (curr->next != NULL && curr->mem != loc)
	{
		curr = curr->next;
	}
	if (curr->mem == loc && curr->taken == TRUE)
	{
		curr->taken = FALSE;
	}
	else 
	{
		printf("Loc hasn't been allocated.");
	}
}

/*
*	This is a memory manager which wrapps XINU's malloc
*	function.
*/
void * linkedListMalloc(unsigned int nbytes)
{	
	void * mem;
	if(nbytes <= 0)		//Do nothing for 0 or negative memory 
	{
		return NULL;
	}
	else if(!init)	//initialize root
	{
		init = TRUE;
		return initList(nbytes);
	}
	else			//root has already been initialized
	{
		//insert node and store user memory address
		mem = insertNode(nbytes);

		//return ptr to user memory address
		return mem;
	}
}

/*
*	Frees the memory pool
*/
void freeMemory()
{
	free(root);
}

/*
*	Returns the memory fragmentation of the pool
*/
struct MemFrag getFrag()
{

	struct MemFrag frag;
	struct Node * curr;
	unsigned int intFrag, allocatedMem;
	intFrag = 0;
	allocatedMem = 0;
	frag.memSize = POOL_SIZE;
	
	curr = (struct Node *)root;
	if (curr == NULL)
	{
		return frag;
	}
	do{
		intFrag += (curr->lenAvail - curr->lenUsed);
		allocatedMem += curr->lenAvail;
		curr = curr->next;
	}while (curr->next != NULL);

	frag.extFrag = POOL_SIZE - allocatedMem;
	frag.intFrag = intFrag;
	return frag;
}

