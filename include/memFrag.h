/*
*	@file fragStruct.h
*	@author Paul Aman
*	struct representing amount of fragmentation occurring for a memory pool
*/

#ifndef _FRAGSTRUCT_H_
#define _FRAGSTRUCT_H_

struct MemFrag
{
	unsigned int intFrag;
	unsigned int extFrag;
	unsigned int memSize;
};

struct MemFragFloat
{
	float intFragPercentage;
	float extFragPercentage;
};
#endif //_FRAGSTRUCT_H_
