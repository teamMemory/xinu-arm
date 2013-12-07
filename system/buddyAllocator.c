/**
 * @file buddyAllocater.c
 * Manages the memory that is allocated
 */

#include <stddef.h> 
#include "buddymemory.h"
#include <stdlib.h>

bool showDebugMSGS = TRUE;		// Show the debug messages as they arise

struct buddynode* rootNode;		// The root node of the tree of Nodes managing the allocated memory
int maxDepth;				// Farthest possible depth, can be calculated based on Page Size
int pageSize = 0;			// The size of the page allocated

struct buddynode* nodePool;		// Pool of nodes for the tree
uint nodePoolCount = 0;

struct buddynode** nodeTraversalList;	// This is used to keep track of nodes during a traversal

/**
* BuddyInit - Initialize the Page allocation and Pool of Nodes
* @param  buddyPageSize - the size of the page requested to allocate
* @return allocatedCorrectly - returns the final status of initialization
*/
bool buddyInit(uint buddyPageSize)
{
	/// Set before - Note should be based off of BuddyPageSize
	maxDepth = 0;

	bool errorFree = TRUE;	// Keep track and see if the allocation hasnt broke

	// Max depth is two depths less than max possible
	uint depthSize = buddyPageSize;

	// Keep going until page can't be split
	while( depthSize % 2 == 0 )
	{
		depthSize -= depthSize  >> 1;
		++maxDepth;
	}
	maxDepth -= MAX_DEPTH_OFFSET;

	// Buddy Page can be split enough
	if( maxDepth > 0 )
	{
		// Allocate Node Pool
		errorFree = errorFree && buddyAllocNodePool( maxDepth );

		if( errorFree )
		{
			// To be implemented fully. Temporary allocation made
			// For now allocating new memory for each node. We should fix in the future
			rootNode = buddyNodeFromPool( 0 );

			if( rootNode )
			{
				rootNode->memRegion = malloc( buddyPageSize );
				pageSize = buddyPageSize;

				errorFree = errorFree && rootNode->memRegion;

				if( errorFree )
				{
					// Allocate node traversal list
					nodeTraversalList = ( struct buddynode** )malloc( sizeof( struct buddynode* ) * (maxDepth + 1) );
					errorFree = errorFree && nodeTraversalList;
				}
			}
			// Cannot find free node for the root
			else
			{
				errorFree = FALSE;
				if( showDebugMSGS ){ printf("Cannot find free node for the root\n"); }
			}

		}
	}

	// Buddy Page doesn't split evenly enough
	else
	{
		if( showDebugMSGS ){ printf("Buddy Page doesn't split evenly enough\n"); }
		maxDepth = 0;
	}

	// Error occurred during allocation clean up memory
	if( !errorFree )
	{
		if( showDebugMSGS ){ printf("Error occurred during Page Allocation\n"); }
		buddyDealloc();
	}

	return errorFree;
}

/**
* BuddyAlloc - Allocate the amount of bytes requested
* @param  numBytes - number of bytes to allocate
* @return memorySection - the address to the memory requested
*/
void* buddyAlloc(uint numBytes)
{
	void* memorySection = 0;        // The Location of the memory allocated
        
	// Allocate the root if non-existent
	if( rootNode == NULL )
	{
		if( buddyInit(numBytes) )
		{
			memorySection = rootNode->memRegion;
		}
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
        if( desiredDepth > maxDepth )
        {   
			desiredDepth = maxDepth;
		}

		// Negative desiredDepth means - Amount to allocate is larger than BUDDY_PAGE_SIZE   
		if( desiredDepth == MEMORY_DEPTH_REQUEST_ERROR )
		{
			if( showDebugMSGS ){ printf("Memory request is larger than page size.\n"); }
		}
		else
		{
			// Acquire the Node that best fits the allocated amount
			bestPossibleFit = buddyBestFit( desiredDepth, rootNode );

			bool splitWorked = TRUE;

			if( bestPossibleFit )
			{
				// Split Node until desired depth is met.
				struct buddynode* currentNode = bestPossibleFit;
				struct buddynode* previousNode = 0;
				while( splitWorked && currentNode->depth != desiredDepth )
				{
						splitWorked = buddySplit( currentNode );

						previousNode = currentNode;
						currentNode = currentNode->leftNode;
				}

				// Split worked, finish allocation
				if( splitWorked )
				{
					// Finalize memory requested
					currentNode->isUsed = TRUE;
					currentNode->allocationSize = numBytes;
					memorySection = currentNode->memRegion;
				}
				// Split Failed, revert process
				else
				{
					buddyFree( previousNode->memRegion );
				}
			}
			else
			{
				if( showDebugMSGS ){ printf("No appropriate memory segment found.\n"); }
			}
		}
	}
	return memorySection;
}

/**
* BuddySplit - Splits a Node into two parts
* @param  Node - the node being split
* @return splitWorked - returns if the split occurred
*/
bool buddySplit(struct buddynode* node)
{
	bool splitCompleted = FALSE;

    if( node )
    {
        // Allocate memory for the Nodes
		// Get From Node Pool ----------------------------------------------

        struct buddynode* leftNode = buddyNodeFromPool( 0 );			// Node to manage children attributes
		struct buddynode* rightNode = buddyNodeFromPool( 1 );			// Node to manage children attributes

        int childDepth = node->depth + 1;

		// Make sure there are enough nodes in the pool to use
		if( leftNode && rightNode )
		{
			// Left Child

			// Set Attributes
			leftNode->memRegion = node->memRegion;
			leftNode->isUsed = FALSE;
			leftNode->depth = childDepth;

			// Null Pointers
			leftNode->leftNode = 0;
			leftNode->rightNode = 0;

			node->leftNode = leftNode;

			// Right Child
			uint offset = (uint)(BUDDY_PAGE_SIZE) >> childDepth;        // Offset from left to right memRegion

			// Set Attributes 
			rightNode->memRegion = (char*)node->memRegion + offset;
			rightNode->isUsed = FALSE;
			rightNode->depth = childDepth;

			// Null Pointers
			rightNode->leftNode = 0;
			rightNode->rightNode = 0;

			node->rightNode = rightNode;

			splitCompleted = TRUE;
		}

		// Not enough free nodes in the pool
		else
		{
			if( showDebugMSGS ){ printf("Could not find enough Nodes in the pool to split.\n"); }
		}
    }

	// Split failed!, prepare it to cancel operation
	if( !splitCompleted )
	{
		node->isUsed = TRUE;
	}


	return splitCompleted;
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
						if( !bestNode || ( returnedNode->depth > bestNode->depth && (uint)returnedNode->depth <= desiredDepth ) )
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
* @return memoryAllocatedCorrectly - whether memory allocated correctly
*/
bool buddyAllocNodePool(uint maxDepth)
{
		// Set-up Node pool - based on maxdepth
		uint nodesToAllocate = 0;				// Amount of nodes to alloc

		bool memoryAllocatedCorrectly = FALSE;

		// Based on half the maxdepth + half-maxdepth of one side

		// Half max depth
		uint halfMaxDepth = maxDepth / 2;
		nodesToAllocate = 1 << ( halfMaxDepth + 1 );	// 2^(depth) * 2 - 1;
		nodesToAllocate -= 1;

		/*
		// Calculate (Full - Half amount) divide by 2 ( one side )
		uint MaxNodesToAllocate = 1 << ( maxDepth + 1 );	// 2^(depth) * 2 - 1;
		MaxNodesToAllocate -= 1;
		
		// Subtract half divide by half
		MaxNodesToAllocate -= nodesToAllocate;
		MaxNodesToAllocate >>= 1;

		nodesToAllocate += MaxNodesToAllocate;
		*/

		nodePoolCount = nodesToAllocate;
		nodePool = (struct buddynode*)malloc( sizeof(struct buddynode) * nodesToAllocate );
		memoryAllocatedCorrectly = nodePool;

		// Clear all Nodes to default
		if( nodePool ){ memset( nodePool, 0, sizeof(struct buddynode) * nodesToAllocate ); }

		return memoryAllocatedCorrectly;
}

/**
* BuddyNodeFromPool - Get a free node if any
* @param offsetFromNext - From the first free node find the nth after that
* @return freeNode
*/
struct buddynode* buddyNodeFromPool(uint offsetFromNext)
{
	if( nodePool )
	{
		// Go through pool until Node memRegion is 0 ( Node is Free )
		// Then return that Nodes address
		uint i = 0;
		for( i; i < nodePoolCount; ++i )
		{
			// Node Free
			if( !nodePool[i].memRegion )
			{
				if( !offsetFromNext )
				{
					return ( nodePool + i );
				}
				--offsetFromNext;
			}
		}
	}

	// No node found, pool is full
	return NULL;
}

/**
* BuddyFree - Free the memory used
* @param memoryAddress - The location in memory of the memory being used
*/
void buddyFree(void* base)
{
	// Descend through the tree using the address location as the basis
	// in regards to the route of descending

	struct buddynode* parentNode = 0;			// This is the parent Node of the child which has the memory allocated	
	struct buddynode* memoryNode = 0;			// This is the address of the Node with the memory


	uint segmentSize = pageSize;				// The size of the current memory segment
	memoryNode = rootNode;

	if( rootNode )
	{

		int baseAddress = (int)base;
		int rootNodeMemAddress = (int)rootNode->memRegion;
		int nodesMemRegion;

		bool previousMemoryCleared = FALSE;

		// Check if the address is in Bounds of the Page
		if( rootNodeMemAddress <= baseAddress && rootNodeMemAddress + pageSize >= baseAddress )
		{
			// Traverse tree based on address, and check if it is valid
			while( memoryNode )
			{
				nodesMemRegion = (int)memoryNode->memRegion;

				nodeTraversalList[memoryNode->depth] = memoryNode;

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
							if( showDebugMSGS ){ printf("Memory already freed"); }
						}
					}
				
					// Child Node Found
					else
					{
						int maxDepthReached = memoryNode->depth;

						int i = maxDepthReached;
						for( i; i >= 1; --i )
						{

							parentNode = nodeTraversalList[i - 1];
							memoryNode = nodeTraversalList[i];

							// Determine Side - Left is equal to
							if( ( memoryNode->memRegion || previousMemoryCleared ) && parentNode->memRegion )
							{
								// Clear memory allocated
								uint sizeOfMemoryChunk = pageSize >> memoryNode->depth;		// Get size of memory Chunk to clear out
								if( memoryNode->memRegion ){ memset( memoryNode->memRegion, 0, sizeOfMemoryChunk ); }
								memoryNode->isUsed = FALSE;

								// Check if Node can be UnSplit ( child nodes are unused )
								struct buddynode* parentLeftNode = parentNode->leftNode;
								struct buddynode* parentRightNode = parentNode->rightNode;

								if( !( (parentLeftNode->isUsed) || (parentLeftNode->leftNode) || (parentLeftNode->rightNode) || 
										(parentRightNode->isUsed) || (parentRightNode->leftNode) || (parentRightNode->rightNode) ) )
								{
									// Clear memory Nodes
									memset( parentNode->leftNode, 0, sizeof(struct buddynode) );
									memset( parentNode->rightNode, 0, sizeof(struct buddynode) );
									previousMemoryCleared = TRUE;

									parentNode->leftNode = NULL;
									parentNode->rightNode = NULL;
								}

								// Memory cannot be merged any more
								else
								{
									break;
								}
							}
						}
						memoryNode = NULL;		// Break loop
					}
				}


				// Left - left of the right child's memory
				else if( memoryNode->rightNode && (int)memoryNode->rightNode->memRegion > baseAddress )
				{
					parentNode = memoryNode;
					memoryNode = parentNode->leftNode;
				}
			
				// Right - to the left of the edge of the segment
				else if(memoryNode->leftNode && (int)memoryNode->leftNode->memRegion + (int)segmentSize >= baseAddress )
				{
					parentNode = memoryNode;
					memoryNode = parentNode->rightNode;
				}

				// Bad address
				else
				{
					// Bad address given
					if( showDebugMSGS ){ printf("Bad address or Already freed"); }
					memoryNode = NULL;
				}

				segmentSize >>= 1;			// Prepare size for next section
			}
		}

		// Address was outside of the Page
		else
		{
			if( showDebugMSGS ){ printf("Address is outside the bounds of the page"); }
		}
	}

	// No root node
	else
	{
		if( showDebugMSGS ){ printf("Cannot Free non allocated Page"); }
	}

	// Clear traversal list
	if( nodeTraversalList ){ memset(nodeTraversalList, 0, sizeof( struct buddynode* ) * maxDepth + 1); }
}

/**
* BuddyDealloc - Remove and clear all of the memory allocated
*/
void buddyDealloc(void)
{
	if( rootNode )
	{
		if( rootNode->memRegion )
		{
			memset( rootNode->memRegion, 0, pageSize );
			free( rootNode->memRegion );
		}

		if( nodePool )
		{
			memset( nodePool, 0, sizeof(struct buddynode) * nodePoolCount );
			free( nodePool );
		}

		if( nodeTraversalList )
		{
			memset( nodeTraversalList, 0, sizeof( struct buddynode* ) * (maxDepth + 1) );
			free( nodeTraversalList );
		}
	}

	// Clear variables
	rootNode = NULL;
	maxDepth = 0;
	pageSize = 0;

	nodePool = NULL;
	nodePoolCount = 0;

	nodeTraversalList = NULL;
}

/**
* BuddyFragmentationAmount - Go through the Nodes and get the fragmentation
* @return Fragmentation - Struct with amount allocated and amount possible
*/
struct MemFrag buddyFragmentationAmount(void)
{
	return buddyFragmentationAmount2(rootNode);
}

struct MemFrag buddyFragmentationAmount2(struct buddynode* node)
{
	struct MemFrag fragmentation;
	fragmentation.intFrag = 0;
	fragmentation.memSize = 0;

	if( node )
	{
		// Itself
		if( node->isUsed )
		{
			fragmentation.intFrag += node->allocationSize;
			fragmentation.memSize += pageSize >> node->depth;
		}
		else if( node->leftNode && node->rightNode )
		{
			// Left
			struct MemFrag leftFrag = buddyFragmentationAmount2( node->leftNode );
			fragmentation.intFrag += leftFrag.intFrag;
			fragmentation.memSize += leftFrag.memSize;

			// Right
			struct MemFrag rightFrag = buddyFragmentationAmount2( node->rightNode );
			fragmentation.intFrag += rightFrag.intFrag;
			fragmentation.memSize += rightFrag.memSize;
		}
	}

	return fragmentation;
}