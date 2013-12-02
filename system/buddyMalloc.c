/**
 * @file buddyMalloc.c
 * @author Blake Gross
 * Allocate memory using a buddy system
 *
 */

#include <stddef.h> 
#include <buddymemory.h>

void* buddyMalloc(uint numBytes)
{
	if( numBytes < 0 )
	{
		return NULL;
	}
	
	return buddyAlloc( numBytes );
} 