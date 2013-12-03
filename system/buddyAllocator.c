/**
 * @file buddyAllocater.c
 * @author Blake Gross
 * Manages the memory that is allocated
 *
 */

#include <stddef.h> 
#include <buddymemory.h>
#include <stdlib.h>

struct buddynode* rootNode;
uint maxDepth;                  // Value of the farthest depth, can be calculated based on Page Size

void buddyInit()
{
	// To be implemented fully. Temporary allocation made
	// For now allocating new memory for each node. We should fix in the future
	rootNode = (struct buddynode*)malloc(sizeof(struct buddynode));
	rootNode->memRegion = malloc( BUDDY_PAGE_SIZE );
	rootNode->isUsed = FALSE;
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
                struct buddyNode* bestPossibleFit;              // Location of the Node that best suites the memory to allocate
                int desiredDepth = -1;                          // The desired depth for memory to be allocated to
                
                // Calculate Desired Depth
                uint depthSize = BUDDY_PAGE_SIZE;
                while( depthSize >= numBytes )
                {
                        depthSize >> 1;
                        ++desiredDepth;
                }
                
                // Negative desiredDepth means - Amount to allocate is larger than BUDDY_PAGE_SIZE                
                // Make sure desiredDepth doesn't pass maxDepth
                if( desiredDepth > maxDepth || desiredDepth == -1 )
                {
                        break;
                }                
                
                // Acquire the Node that best fits the allocated amount
                bestPossibleFit = buddyBestFit( desiredDepth, rootNode );
                
                // Split Node until desired depth is met.
                struct buddyNode* currentNode = bestPossibleFit;
                while( currentNode->depth != desiredDepth )
                {
                        buddySplit( currentNode );
                        currentNode = currentNode->leftNode;
                }
                
                // Finalize memory requested
                currentNode->isUsed = TRUE;
                memorySection = currentNode->memRegion;
                
        }
        
	return memorySection;
}



/**
* BuddySplit - Splits a Node into two parts
* @param  Node - the node being split
*/
void buddySplit(struct buddyNode* node)
{
        if( node )
        {
                // Allocate memory for the Nodes
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
                        childNode->memRegion = node->memRegion + offset;
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
struct buddynode* buddyBestFit(uint desiredDepth, struct buddyNode* node)
{
   struct buddyNode* bestNode = 0;
   struct buddyNode* returnedNode = 0;
   
   if( node )
   {        
        // Check to see if it is the best solution
        if( !node->isUsed )
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
                        if( !bestNode || ( returnedNode && returnedNode->depth > bestNode->depth && returnedNode->depth <= desiredDepth ) )
                        {
                                if( returnedNode->!isUsed )
                                {
                                        bestNode = returnedNode;
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
                                
                                // Swap bestNode if applicable
                                if( !bestNode || returnedNode->depth > bestNode->depth && returnedNode->depth <= desiredDepth )
                                {
                                        if( returnedNode->!isUsed )
                                        {
                                                bestNode = returnedNode;
                                        }
                                }
                        }
                }
        }   
   }   

   return bestNode;
}