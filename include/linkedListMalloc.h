/*
*	@file LinkedListMalloc.h
*	@author Joe Cuffney 
*	@author Paul Aman
*	header for linked list memory allocator which uses the kernals malloc
*/

#ifndef _LINKEDLISTMALLOC_H_
#define _LINKEDLISTMALLOC_H_

#include "memFrag.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0 
#define POOL_SIZE 10240

struct Node
{
	int taken;
	int lenUsed;
	int lenAvail;
	void * mem;
	struct Node * next;
};


void * linkedListMalloc(unsigned int nbytes);
void * insertNode(unsigned int nbytes);
void removeNode(void * loc);
void * initList(unsigned int nbytes);
void freeMemory();
struct MemFrag getFrag();
#endif //_LINKEDLISTMALLOC_H_
