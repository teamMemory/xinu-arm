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

void * pool;			//points to pool of memory to allocate from
struct Node root;		//will always point to the root node
//void * current;		//will point to varying nodes as we traverse
//void * nextChunk;	//points to the next chunk of unallocated memory
//void * lastChunk;	//the previous chunk allocated.

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
/*
void * initList(unsigned int userBytes)
{
	
	int nodeSize, totalSize;
	struct Node firstNode;
	//allocate a pool of memory
	root = malloc(POOL_SIZE);
	printf("	Root Address: %p\n" , root);
	nodeSize = sizeof(struct Node);
	printf("	Node Size: %i\n" , nodeSize);
	printf("	USER MEM SIZE: %i\n" , userBytes);
	totalSize = nodeSize + userBytes;
	
		//root = (struct Node *) malloc(sizeof(struct Node));
		//root->mem = malloc(nbytes);
		//root->prev = NULL;
		//root->next = NULL;
	firstNode.mem = &root + nodeSize;
	printf("	USER MEM ADDRESS: %p\n" , firstNode.mem);

	firstNode.prev = NULL;
	firstNode.next = NULL;
	firstNode.len = totalSize;
	firstNode.taken = TRUE;

	//copy the firstNode into the memory pool
	memcpy(&root,&firstNode,nodeSize);
	
	current = &root;
	//set next chunk to the address after the initial chunk
	nextChunk = &root + totalSize;

	//set the lastChunk *
	lastChunk = &root;

	//return the root location plus the offset of the struct
	//to get the user's memory location
	return &root + nodeSize;
}*/
void * initList(unsigned int userBytes)
{

}


/*
*	Inserts a node at the end of the linked list 
*/
void * insertNode(unsigned int userBytes)
{
	/*struct Node * tempPrev;
	current = root;
	while(current->next != NULL)
	{
		current = current->next;
	}
	
	current->next = (struct Node *) malloc(sizeof(struct Node));
	//store the address of the node before the new node
	tempPrev = current;
	//advance 1 node
	current = current->next;
	current->mem = malloc(nbytes);
	current->prev = tempPrev;
	current->next = NULL;
	return current;*/

	struct Node newNode;
	int nodeSize = sizeof(struct Node);
	int totalSize = nodeSize + userBytes;
	//now we need to see if their is an open
	//chunk left by a previous node (taken = 0)
	current = &root;
	//taken should be the first 4 byte chunk stored
		
	newNode.mem = &nextChunk + nodeSize;
	
}

/*
*	This Function takes in a pointer to a location in memory
*	it then has to that structures location free
*/
void removeNode(void * loc)
{
	/*
	struct Node * tempPrev;
	struct Node * tempNext;
	current = root;
	while(current->next != NULL)
	{
		if(current->mem == loc)
		{
			break;
		}
		else
		{
			current = current->next;
		}
	}
	// loc is not allocated
	if(current->mem != loc)
	{
		return;
	}
	//at the point the current is the correct node
	//store address for prev of the node ahead of the one we are removing
	tempPrev = current->prev;
	tempNext = current->next;
	tempPrev->next = tempNext;
	
	if(current->next != NULL)
	{
		tempNext->prev = tempPrev;
	}
	//root to be removed
	if(root->mem == loc)
	{
		root = root->next;
	}
	
	//linked list is intact
	//deallocate user memory
	free(loc);
	//free the associated Node struct
	free(current);
	//set current to root
	current = root;
	*/
}

/*
*	This is a memory manager which wrapps XINU's malloc
*	function.
*/
void * linkedListMalloc(unsigned int nbytes)
{	
	struct Node * n;
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
		n = (struct Node *)insertNode(nbytes);
		//reset the current to the root
		current = root;
		//return ptr to user memory address
		return n->mem;
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
