/*
*	linkedList Memory Allocator
*	@author Joe Cuffney
*/

#include <linkedListMalloc.h>
#include <stdlib.h>
//#include <
#define TRUE 1
#define FALSE 0

struct Node * root;	//will always point to the root node
struct Node * current;	//will point to varying nodes as we traverse

/*
*
*	Initializes the root node and sets the "current" Node
*	to that root
*
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
*
*	Inserts a node either in the first
*
*/
void * insertNode(uint nbytes)
{
	while(current->next != NULL)
	{
		current = current->next;
	}
	current->next = malloc(sizeof(struct Node));
	void * tempPrev = &current;
	current = currenty->next;
	current->prev = tempPrev;
	current->next = NULL;
}

/*
*
*	This Function takes in a pointer to a location in memory
*	it then has to that structures location free
*
*/
void removeNode(void * loc)
{
	
	free(loc);
}

void * linkedListMalloc(uint nbytes)
{	
	//Do nothing if asks for 0 or negative memory 
	if(nbytes <= 0)	
	{
		return NULL;
	}
	if(root == NULL)	//initialize root
	{
		return initList(nbytes);
	}
	else	//root has already been initialized
	{
		void * loc = insertNode(nbytes);
		//reset the current to the beginning of the list
		//after each traversal
		current = &root;
	}
}
