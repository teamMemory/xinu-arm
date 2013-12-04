/*
*	@file LinkedListMalloc.h
*	@author Joe Cuffney 
*	@author Paul Aman
*	header for linked list memory allocator which uses the kernals malloc
*/

#ifndef _LINKEDLISTMALLOC_H_
#define _LINKEDLISTMALLOC_H_

struct Node
{
	void * mem;
	struct Node * next;
	struct Node * prev;
};


void * linkedListMalloc(uint nbytes);
void * insertNode(uint nbytes);
void removeNode(void * loc);
void * initList(uint nbytes);
#endif //_LINKEDLISTMALLOC_H_
