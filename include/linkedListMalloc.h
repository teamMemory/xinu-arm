/*
*	@file LinkedListMalloc.h
*	@author Joe Cuffney 
*	header for linked list memory allocator which uses the kernals malloc
*/

#ifndef _LINKEDLISTMALLOC_H_
#define _LINKEDLISTMALLOC_H_

struct Node
{
	void * mem;
	struct Node * next;
};


void * linkedListMalloc(uint nbytes);
void * insertNode(uint nbytes);
void * removeNode(uint idx);

#endif //_LINKEDLISTMALLOC_H_
