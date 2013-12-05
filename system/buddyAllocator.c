/**
 * @file buddyAllocater.c
 * Manages the memory that is allocated
 */

#include <stddef.h> 
#include "buddymemory.h"
#include <stdlib.h>

struct buddynode* rootNode;
int maxDepth;                  // Farthest possible depth, can be calculated based on Page Size
int pageSize = 0;

struct buddynode* nodePool;
uint nodePoolCount = 0;

/**
* BuddyInit - Initalize the Page allocation and Pool of Nodes
* @param  buddyPageSize - the size of the page requested to allocate
*/
void buddyInit(uint buddyPageSize)
{
	/// Set before - Note should be based off of BuddyPageSize
	maxDepth = 0;

	// Max depth is two depths less than max possible
	uint depthSize = buddyPageSize;

	// Keep going until page can't be split
	while( depthSize % 2 == 0 )
	{
		depthSize -= depthSize  >> 1;
		++maxDepth;
	}
	maxDepth -= MAX_DEPTH_OFFSET;

	if( maxDepth > 0 )
	{
		// Allocate Node Pool
		buddyAllocNodePool( maxDepth );

		// To be implemented fully. Temporary allocation made
		// For now allocating new memory for each node. We should fix in the future
		rootNode = buddyNodeFromPool();
		rootNode->memRegion = malloc( buddyPageSize );
		pageSize = buddyPageSize;
	}
	else
	{
		// Buddy Page doesn't split evenly enough
	}
}

/**
* BuddyAlloc - Allocate the amount of bytes requested
* @param  numBytes - number of bytes to allocate
* @return memorySection - the address to the memory requested
*/
void* buddyAlloc(uint numBytes)
{
	void* memorySection = 0;        // The Location of the memory allocated
        
	// Allocate the root if nonexistant
	if( rootNode == NULL )
	{
		buddyInit(numBytes);
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

        struct buddynode* childNode;            // Node to manage children attributes
        int childDepth = node->depth + 1;

        // Left Child
        childNode = buddyNodeFromPool();
        if( childNode )
        {
            // Set Attributes
            childNode->memRegion = node->memRegion;
            childNode->isUsed = FALSE;
            childNode->depth = childDepth;

            // Null Pointers
            childNode->leftNode = 0;
            childNode->rightNode = 0;

			node->leftNode = childNode;
        }
        // Else error

        // Right Child
        childNode = buddyNodeFromPool();
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

			node->rightNode = childNode;
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

		// Left Node
		// Check to see if search is needed
		if( !bestNode || bestNode->depth != desiredDepth )
		{
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

/**
* BuddyAllocNodePool - Based on the max depth of the tree, create a pool of nodes
* @param  maxDepth - the max depth of the tree
*/
void buddyAllocNodePool(uint maxDepth)
{
	// Setup Node pool - based on maxdepth
		uint nodesToAllocate = 0;				// Amount of nodes to alloc

		// Based on half the maxdepth + half-maxdepth of one side

		// Half max depth
		uint halfMaxDepth = maxDepth / 2;
		nodesToAllocate = 1 << ( halfMaxDepth + 1 );	// 2^(depth) * 2 - 1;
		nodesToAllocate -= 1;

		// Calculate (Full - Half amount) divide by 2 ( one side )
		uint MaxNodesToAllocate = 1 << ( maxDepth + 1 );	// 2^(depth) * 2 - 1;
		MaxNodesToAllocate -= 1;
		
		// Subtract half divde by half
		MaxNodesToAllocate -= nodesToAllocate;
		MaxNodesToAllocate >>= 1;

		nodesToAllocate += MaxNodesToAllocate;

		nodePoolCount = nodesToAllocate;
		nodePool = (struct buddynode*)malloc( sizeof(struct buddynode) * nodesToAllocate );

		// Clear all Nodes to default
		memset( nodePool, 0, sizeof(struct buddynode) * nodesToAllocate );
}

/**
* BuddyNodeFromPool - Get a free node if any
*/
struct buddynode* buddyNodeFromPool()
{
	if( nodePool )
	{
		// Go through pool until Node memRegion is 0 ( Node is Free )
		// Then return that Nodes address
		for( int i = 0; i < nodePoolCount; ++i )
		{
			// Node Free
			if( !nodePool[i].memRegion )
			{
				return ( nodePool + i );
			}
		}
	}
	// No node found, pool is full
	return NULL;
}

/**
* BuddyFree - Free the memory used
* @param memoryAddress - The location in memroy of the memory being used
*/
void buddyFree(void* base)
{
	// Decend through the tree using the address locaction as the basis
	// inregards to the route of decending

	struct buddynode* parentNode = 0;			// This is the parent Node of the child which has the memory allocated	
	struct buddynode* memoryNode = 0;			// This is the address of the Node with the memory


	uint segmentSize = pageSize;				// The size of the current memory segment
	memoryNode = rootNode;

	int baseAddress = (int)base;
	int rootNodeAddress = (int)rootNode;
	int nodesMemRegion;

	int memorySegementSize = 0;

	// Check if the address is in Bounds of the Page
	if( (int)rootNode->memRegion <= baseAddress && (int)rootNode->memRegion + pageSize >= baseAddress )
	{
		// Traverse tree based on address, and check if it is valid
		while( memoryNode )
		{
			nodesMemRegion = (int)memoryNode->memRegion;

			// Memory address almost found
			if( nodesMemRegion == baseAddress )
			{
				// Check to see if node is allocated

				// Parent Node to child - head left
				if( !memoryNode->isUsed )
				{
					parentNode = memoryNode;
					memoryNode = parentNode->leftNode;

					// Memory was freed already
					if( !memoryNode )
					{
						printf("Memory already freed");
					}
				}
				
				// Child Node Found
				else
				{
					// Determine Side - Left is equal to

					// Clear memory allocated
					uint sizeOfMemoryChunk = pageSize >> memoryNode->depth;		// Get size of memory Chunk to clear out
					memset( memoryNode->memRegion, 0, sizeOfMemoryChunk );
					memoryNode->isUsed = FALSE;

					// Check if Node can be UnSplit ( child nodes are unused )
					struct buddynode* parentLeftNode = parentNode->leftNode;
					struct buddynode* parentRightNode = parentNode->rightNode;

					if( !(parentLeftNode->isUsed) && !(parentLeftNode->leftNode) && !(parentLeftNode->rightNode) && 
						!(parentRightNode->isUsed) && !(parentRightNode->leftNode) && !(parentRightNode->rightNode) )
					{
						// Clear memory Nodes
						memset( parentNode->leftNode, 0, sizeof(struct buddynode) );
						memset( parentNode->rightNode, 0, sizeof(struct buddynode) );

						parentNode->leftNode = NULL;
						parentNode->rightNode = NULL;
					}
					memoryNode = NULL;		// Break loop
				}
			}


			// Left - left of the right childs memory
			else if( memoryNode->rightNode && (int)memoryNode->rightNode->memRegion > baseAddress )
			{
				parentNode = memoryNode;
				memoryNode = parentNode->leftNode;
			}
			
			// Right - to the left of the edge of the segment
			else if(memoryNode->leftNode && (int)memoryNode->leftNode->memRegion + segmentSize >= baseAddress )
			{
				parentNode = memoryNode;
				memoryNode = parentNode->rightNode;
			}

			// Bad address
			else
			{
				// Bad address given
				printf("Bad address or Already freed");
				memoryNode = NULL;
			}

			segmentSize >>= 1;			// Prepare size for next section
		}
	}

	// Address was outside of the Page
	else
	{
		printf("Address is outside bounds of the page");
	}
}
