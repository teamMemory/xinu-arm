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
	int taken;
	int len;
	void * mem;
	struct Node * next;
};


void * linkedListMalloc(unsigned int nbytes);
void * insertNode(unsigned int nbytes);
void removeNode(void * loc);
void * initList(unsigned int nbytes);
#endif //_LINKEDLISTMALLOC_H_
