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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define TRUE 1
#define FALSE 0
#define POOL_SIZE 1024

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
	printf("	Root Address: %p\n" , root);
	nodeSize = sizeof(struct Node);
	printf("	Node Size: %i\n" , nodeSize);
	printf("	USER MEM SIZE: %i\n" , userBytes);
	totalSize = nodeSize + userBytes;
	
	firstNode = (struct Node *) root;

	firstNode->mem = firstNode + nodeSize;
	printf("	USER MEM ADDRESS: %p\n" , firstNode->mem);

	firstNode->next = NULL;
	firstNode->len = userBytes;
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
	void * cur = &root;
	int sizeStruct = sizeof(struct Node);
	int poolBytesUsed = 0;
	while(cur.next! != NULL)
	{
		//cast cur to (struct *)?
		
		if(!cur.taken && cur.len >= userBytes)
		{
			return cur.mem;
		}
		else
		{
			poolBytesUsed += (sizeStruct + cur.len);
			cur = cur.next;	
		}
	}
	//cur points to the last existing node
	int sizeNeeded = userBytes + sizeStruct;
	//only allocate a new Node if we have the space to do so
	if((sizeNeeded + poolBytesUsed) < POOL_SIZE)
	{
		struct Node newNode;
		newNode.next = NULL;
		newNode.taken = TRUE;
		newNode.len = userBytes;
		newNode.mem = newNode += cur + sizeof(struct Node);
		//now we need to copy the newNode into the pool
		//first advance to the next chunk in the pool
		cur += structSize + cur.len;
		memcpy(&cur, &newNode, sizeof(struct Node);
		//the memory is now in the pool.	
		return &cur;
	}
	else
	{
		//in this case we can't allocate the memory because
		//we don't have sufficient space in the pool.
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

int main()
{
	void * loc;
	printf("Allocating memory\n");
	loc = linkedListMalloc(56);
	printf("Memory allocated\n");
	printf("Deallocating memory\n");
	removeNode(loc);
	printf("Memory deallocated\n");
}
