/*
*	linkedList Memory Allocator
*	@file linkedListMalloc.c
*	@author Joe Cuffney
*/

/////////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////////


#include <linkedListMalloc.h>
#include <stdlib.h>
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
void * initList(uint nbytes)
{
	root = malloc(sizeof(struct Node));
	//initialize the root of the tree iff
	//this is the first chunk allocated
	root->mem = malloc(nbytes);
	root->prev = NULL;
	root->next = NULL;
	//set the currect to point to the root
	current = &root;
	//return the location of the memory the program can use
	return &current;
}


/*
*	Inserts a node at the end of the linked list 
*/
void * insertNode(uint nbytes)
{
	while(current->next != NULL)
	{
		current = current->next;
	}
	current->next = malloc(sizeof(struct Node));
	//store the address of the node before the new node
	void * tempPrev = &current;
	//advance 1 node
	current = currenty->next;
	current->prev = tempPrev;
	current->next = NULL;
}

/*
*	This Function takes in a pointer to a location in memory
*	it then has to that structures location free
*/
void removeNode(void * loc)
{
	while(current->mem != loc)
	{
		//avoid dereferencing null pointer
		if(current->next != NULL)
		{
			current = current->next;
		}
	}
	//at the point the current->next is the correct node
	//store address for prev of the node ahead of the one we are removing
	void tempPrev = &current;
	//advance to following node
	current = current->next;
	void * tempNext;
	if(current->next == NULL)
	{
		//iff end of list
		tempNext = NULL;
	}
	else
	{
		current = current->next;
		tempNext = &current;
		current->prev = tempPrev;
		//move back to node before 
		//the one we are removing
		current = &current->prev;
		current = &current->prev;
		//set the next value
		current->next = tempNext;
		//advance to node to remove;
		current = current->next;
	}
	//linked list is intact
	//deallocate user memory
	free(loc);
	//free the associated Node struct
	free(current);
	//set current to root
	current = &root;
}

/*
*	This is a memory manager which wrapps XINU's malloc
*	function.
*/
void * linkedListMalloc(uint nbytes)
{	
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
		void * loc = insertNode(nbytes);
		//reset the current to the root
		current = &root;
		//return ptr to user memory address
		return &loc;
	}
}
