/**
 * @file buddyAllocater.c
 * Manages the memory that is allocated
 */

#include <stddef.h> 
#include "buddymemory.h"
#include <stdlib.h>

struct buddynode* rootNode;
uint maxDepth;                  // Farthest possible depth, can be calculated based on Page Size

void buddyInit()
{
	// To be implemented fully. Temporary allocation made
	// For now allocating new memory for each node. We should fix in the future
	rootNode = (struct buddynode*)malloc(sizeof(struct buddynode));
	rootNode->memRegion = malloc( BUDDY_PAGE_SIZE );

	rootNode->depth = 0;
	rootNode->isUsed = FALSE;
	rootNode->leftNode = NULL;
	rootNode->rightNode = NULL;

	/// Set before - Note should be based off of BuddyPageSize
	maxDepth = 20;
}

/**
* BuddyAlloc - Allocate the amount of bytes requested
* @param  numBytes - number of bytes to allocate
* @return memorySection - the address to the memory requested
*/
void* buddyAlloc(uint numBytes)
{
	void* memorySection = 0;        // The Location of the memory allocated
        
	if( rootNode == NULL )
	{
		buddyInit();
		memorySection = rootNode->memRegion;
	}
    else
    {
        struct buddynode* bestPossibleFit;              // Location of the Node that best suites the memory to allocate
        int desiredDepth = MEMORY_DEPTH_REQUEST_ERROR;	// The desired depth for memory to be allocated

        // Calculate Desired Depth
        uint depthSize = BUDDY_PAGE_SIZE;
        while( depthSize >= numBytes )
        {
                depthSize >>= 1;
                ++desiredDepth;
        }

        // Make sure desiredDepth doesn't pass maxDepth
        if( (uint)desiredDepth > maxDepth )
        {   
			// Memory request is to small
		}

		// Negative desiredDepth means - Amount to allocate is larger than BUDDY_PAGE_SIZE   
		else if( desiredDepth == MEMORY_DEPTH_REQUEST_ERROR )
		{
			// Memory request is larger than page size
		}
		else
		{
			// Acquire the Node that best fits the allocated amount
			bestPossibleFit = buddyBestFit( desiredDepth, rootNode );

			if( bestPossibleFit )
			{
				// Split Node until desired depth is met.
				struct buddynode* currentNode = bestPossibleFit;
				while( currentNode->depth != desiredDepth )
				{
						buddySplit( currentNode );
						currentNode = currentNode->leftNode;
				}

				// Finalize memory requested
				currentNode->isUsed = TRUE;
				memorySection = currentNode->memRegion;
			}
			else
			{
				// No approriate memory segment found
			}
		}
	}
	return memorySection;
}

/**
* BuddySplit - Splits a Node into two parts
* @param  Node - the node being split
*/
void buddySplit(struct buddynode* node)
{
    if( node )
    {
        // Allocate memory for the Nodes
		// Get From Node Pool ----------------------------------------------

        node->leftNode  = (struct buddynode*)malloc(sizeof(struct buddynode));
        node->rightNode = (struct buddynode*)malloc(sizeof(struct buddynode));

        struct buddynode* childNode;            // Node to manage children attributes
        int childDepth = node->depth + 1;

        // Left Child
        childNode = node->leftNode;
        if( childNode )
        {
            // Set Attributes
            childNode->memRegion = node->memRegion;
            childNode->isUsed = FALSE;
            childNode->depth = childDepth;

            // Null Pointers
            childNode->leftNode = 0;
            childNode->rightNode = 0;
        }
        // Else error

        // Right Child
        childNode = node->rightNode;
        if( childNode )
        {
            uint offset = (uint)(BUDDY_PAGE_SIZE) >> childDepth;        // Offset from left to right memRegion

            // Set Attributes 
			childNode->memRegion = (char*)node->memRegion + offset;
            childNode->isUsed = FALSE;
            childNode->depth = childDepth;

            // Null Pointers
            childNode->leftNode = 0;
            childNode->rightNode = 0;
        }
        // Else error
    }
}

/**
* BuddyBestFit - Goes through the Tree and Acquires the Node that would be best for this request
* @param  desiredDepth - the desired depth of the memory requested
* @param  node - the current Node being accessed
* @return bestNode - the current best node
*/
struct buddynode* buddyBestFit(uint desiredDepth, struct buddynode* node)
{
	struct buddynode* bestNode = 0;
	struct buddynode* returnedNode = 0;
   
	if( node && !node->isUsed && (uint)node->depth <= desiredDepth)
	{
		// Check to see if it is the best solution
		if( !node->leftNode && !node->rightNode )
		{
				bestNode = node;
		}

		// Check to see if search is needed
		if( !bestNode || bestNode->depth != desiredDepth )
		{
			// Left Node
			if( node->leftNode )
			{
				returnedNode = buddyBestFit( desiredDepth, node->leftNode );
                        
				// Swap bestNode if applicable
				if( returnedNode )
				{

					if( !bestNode || ( returnedNode->depth > bestNode->depth && (uint)returnedNode->depth <= desiredDepth ) )
					{
						if( !returnedNode->isUsed )
						{
							bestNode = returnedNode;
						}
					}
				}
			}

			// Right Node
			// Check to see if search is needed
			if( !bestNode || bestNode->depth != desiredDepth )
			{
				if( node->rightNode )
				{
					returnedNode = buddyBestFit( desiredDepth, node->rightNode );
                                
					if( returnedNode )
					{
						// Swap bestNode if applicable
						if( !bestNode || returnedNode->depth > bestNode->depth && (uint)returnedNode->depth <= desiredDepth )
						{
							if( !returnedNode->isUsed )
							{
								bestNode = returnedNode;
							}
						}
					}
				}
			}
		}
	}
	return bestNode;
}