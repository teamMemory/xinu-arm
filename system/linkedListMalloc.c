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
#define TRUE 1
#define FALSE 0

/////////////////////////////////////////////////////
/// Globals
/////////////////////////////////////////////////////


struct Node * root;	//will always point to the root node
struct Node * current;	//will point to varying nodes as we traverse

/////////////////////////////////////////////////////
/// Functions
/////////////////////////////////////////////////////

/*
*	Initializes the root node and sets the "current" Node
*	to that root
*/
void * initList(unsigned int nbytes)
{
	root = (struct Node *) malloc(sizeof(struct Node));
	//initialize the root of the tree iff
	//this is the first chunk allocated
	root->mem = malloc(nbytes);
	root->prev = NULL;
	root->next = NULL;
	//set the currect to point to the root
	current = root;
	//return the location of the memory the program can use
	return current;
}


/*
*	Inserts a node at the end of the linked list 
*/
void * insertNode(unsigned int nbytes)
{
	struct Node * tempPrev;
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
	return current;
}

/*
*	This Function takes in a pointer to a location in memory
*	it then has to that structures location free
*/
void removeNode(void * loc)
{
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
	else if(root == NULL)	//initialize root
	{
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
	printf("Allocating memory");
	loc = linkedListMalloc(56);
	printf("Memory allocated");
	printf("Deallocating memory");
	removeNode(loc);
	printf("Memory deallocated");
}
