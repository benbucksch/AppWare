#include <math.h>
#include "almsys.h"

#include "memlib.h"
#include "intdebug.h"

#define WINDOWSBYTEBOUNDARY             16
#define BYTEBOUNDARY                    16
#define MAXBLOCKSIZE                    0xffff
#define MINBLOCKSIZE                    4096
#define DESIREDRECORDS                  10
#define STARTINGSIZE                    BYTEBOUNDARY
#define ENDINGSIZE                			1024


#define REC_SIG_VALUE (AMAKEID('T','J','W','s'))

struct blockHeader{
	unsigned int                            maxRecords;
	unsigned int                            currentRecords;
	unsigned int                            free;           //points to the next never before used record in the block
	unsigned int                            recordSize;
	blockHeader *                        nextBlock;
	blockHeader *                        previousBlock;
	};

typedef blockHeader * BHPtr;

struct recordHeader
	{
  BHPtr blockPtr;
	
	union
		{
  	long sig;
		recordHeader * nextFree;
    } link;
	};

typedef recordHeader * RHPtr;


struct listHeader{
	BHPtr firstBlock;
	BHPtr firstFreeBlock;
	RHPtr freeList;
	};

typedef listHeader * LHPtr;


//Prototypes
BHPtr NewBlock(unsigned size);
LHPtr CreateArrayOfHandles();
void DeleteBlock(BHPtr blockPtr);

struct ARRAYHEADER{
	LHPtr arrayOfBlocks;
	long blockCount;
	};

typedef ARRAYHEADER * AHPtr;

//globals
#ifdef ADBG_DEBUG
static AHPtr debugHeader = NULL;
#else
static AHPtr noDebugHeader = NULL;
#endif

static unsigned  arraySize;


//MACROS
#ifdef ADBG_DEBUG

#define ARRAYOFBLOCKS debugHeader->arrayOfBlocks
#define GLOBALPOINTER debugHeader
#define GLOBALTYPECAST AHPtr

#else

#define ARRAYOFBLOCKS noDebugHeader->arrayOfBlocks
#define GLOBALPOINTER noDebugHeader
#define GLOBALTYPECAST AHPtr

#endif

#define CREATEGLOBALSTRUCTURE GLOBALPOINTER = (GLOBALTYPECAST)new ARRAYHEADER
#define DESTROYGLOBALSTRUCTURE delete   GLOBALPOINTER

#define GETRECORDPTR(x)	 ((RHPtr)(((BYTE*)x) - sizeof(recordHeader)))
#define GETBLOCKPTR(x) GETRECORDPTR(x)->blockPtr
#define PTR_FROM_RECORDHDR(x) ((void *)(((BYTE*)x) + sizeof(recordHeader)))
#define RECORDPTR_FROM_OFFSET(blck, off) ((RHPtr)(((BYTE*)blck) + off))

////////////////////////////////////////////////////////////////////////////
BOOL ValidRecordPtr(void* ptr)
	{
  return (GETRECORDPTR(ptr)->link.sig == REC_SIG_VALUE)?TRUE:FALSE;
  }


/////////////////////////////////////////////////////////////////////////////
//   GetGlobalBlockArrayAddr
//
// Returns the current value of arrayOfBlocks for the system.  Needs to be
// passed to the OIP during init of the microApp.
/////////////////////////////////////////////////////////////////////////////
void  * GetGlobalBlockArrayAddr()
  {
  return GLOBALPOINTER;
  }

/////////////////////////////////////////////////////////////////////////////
//   SetGlobalBlockArrayAddr
//
// Sets the current value of arrayOfBlocks for the system.  Needs to be
// performed by the OIP during init of the microApp.
/////////////////////////////////////////////////////////////////////////////
void SetGlobalBlockArrayAddr(
  void  * theAddr)
  {
  GLOBALPOINTER = (GLOBALTYPECAST)theAddr;
  }

/////////////////////////////////////////////////////////////////////////////
//   GetGlobalBlockArraySize
//
// Returns the current value of arraySize for the system.  Needs to be
// passed to the OIP during init of the microApp.
/////////////////////////////////////////////////////////////////////////////
unsigned GetGlobalBlockArraySize()
  {
  return arraySize;
  }

/////////////////////////////////////////////////////////////////////////////
//   SetGlobalBlockArraySize
//
// Sets the current value of arraySize for the system.  Needs to be
// performed by the OIP during init of the microApp.
/////////////////////////////////////////////////////////////////////////////
void SetGlobalBlockArraySize(
  unsigned theSize)
  {
  arraySize = theSize;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        Initialize
//
// CLASS.......:  None
//
// DESCRIPTION.:  performs neccessary initialization to set up for the
//                                                                      memlib routines.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL InitializeBlocks()
	{
	if(!GLOBALPOINTER)
		{
		CREATEGLOBALSTRUCTURE;

		ARRAYOFBLOCKS=CreateArrayOfHandles();

		#ifdef ADBG_DEBUG
		GLOBALPOINTER->blockCount = 0;
		#endif
		}
    
  return (ARRAYOFBLOCKS != NULL)?TRUE:FALSE;
	}



/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        CreateArrayOfHandles
//
// CLASS.......:  None
//
// DESCRIPTION.:  Sets up the initial array of pointers to memory blocks.
//                                                                      all pointers are initially set to null.
//
// PARAMETERS..:
//
// RETURN VALUE:  BHPtr *:  pointer to the array which was just
//                                                                      allocated.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
LHPtr CreateArrayOfHandles()
	{
	LHPtr                   arrayPtr;

	arraySize = unsigned(ceil((double)((double)(ENDINGSIZE - STARTINGSIZE) /
		BYTEBOUNDARY)))+1;
	arrayPtr = (LHPtr)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,
		 arraySize * sizeof(listHeader));

	I_ASSERT(arrayPtr);
  
	return arrayPtr;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        NewBlock
//
// CLASS.......:  None
//
// DESCRIPTION.:  Allocates a big chunk of memory from Windows and
//                                                                      sets up the block header pointer.
//
// PARAMETERS..:  BHPtr:  points to the block which was just allocated.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
#ifndef VABSHELL
BHPtr NewBlock(unsigned size)
	{
	BHPtr                   blockPtr;
	unsigned                recordSize;
	unsigned long           blockSize;
	unsigned                numberRecords;

	recordSize = size + sizeof( recordHeader );
	numberRecords = DESIREDRECORDS;
	blockSize = recordSize * numberRecords + sizeof(blockHeader);
	if( blockSize < MINBLOCKSIZE )
		blockSize = MINBLOCKSIZE;
	else if( blockSize > MAXBLOCKSIZE )
			blockSize = MAXBLOCKSIZE;

	numberRecords =(unsigned)(((WORD)blockSize - sizeof(blockHeader))/
		recordSize);
	blockSize =(DWORD)(recordSize * numberRecords + sizeof(blockHeader));
	blockSize+=WINDOWSBYTEBOUNDARY - (blockSize % WINDOWSBYTEBOUNDARY);
	blockPtr = (BHPtr)GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT, blockSize);
	I_ASSERT(blockPtr);
  if(blockPtr)
  	{
		blockPtr->free =sizeof(blockHeader);
		blockPtr->recordSize = recordSize;
		blockPtr->maxRecords = numberRecords;
    
		#ifdef ADBG_DEBUG
		GLOBALPOINTER->blockCount++;
		#endif
		}
    
	return blockPtr;
	}
#endif

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        Allocate
//
// CLASS.......:  None
//
// DESCRIPTION.:  Allocates a record, or section of memory, from one the
//                                                                      blocks in the arrayOfBlocks.
//
// PARAMETERS..:  unsigned size: size of record to allocate
//
// RETURN VALUE:  void *: pointer to the new record.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
#ifndef VABSHELL
void * Allocate(
	unsigned size)
	{
	BHPtr                        blockPtr;
	RHPtr                        newPtr;
	unsigned                     offset = 0;

	if(!GLOBALPOINTER)
		InitializeBlocks();

	//figure offset into the array of blocks
	if(size > STARTINGSIZE)
		offset =(int)((size-STARTINGSIZE-1)/BYTEBOUNDARY) + 1;

	if(offset >= arraySize)
		{
    //we must specify the GMEM_MOVEABLE flag so the block can be repositioned
    //if it doesn't fit at its current location.
		ARRAYOFBLOCKS = (LHPtr)GlobalReAlloc((HGLOBAL)ARRAYOFBLOCKS, (offset+1) * sizeof(listHeader),
			GMEM_MOVEABLE | GMEM_ZEROINIT);

		arraySize = offset+1;
		}
	
	if(!ARRAYOFBLOCKS)
  	{
    I_ASSERT(FALSE);
    FatalAppExit(2, "Out of Memory - Terminating Application to prevent corruption.");    
    }
		
	if(ARRAYOFBLOCKS[offset].freeList)
		{
		newPtr = ARRAYOFBLOCKS[offset].freeList;
		ARRAYOFBLOCKS[offset].freeList = newPtr->link.nextFree;
		}
	else
		{
		//no free list so get record from the block
		if(!ARRAYOFBLOCKS[offset].firstBlock)
			{
			//create the first block
			blockPtr =(NewBlock(STARTINGSIZE + (offset * BYTEBOUNDARY)));
			ARRAYOFBLOCKS[offset].firstBlock = blockPtr;
			ARRAYOFBLOCKS[offset].firstFreeBlock = blockPtr;
			}
		else
			{
			//make sure the free block is not full
			if(!ARRAYOFBLOCKS[offset].firstFreeBlock->free)
				{
				//block full so create a new block
				blockPtr = NewBlock(STARTINGSIZE + (offset * BYTEBOUNDARY));

				ARRAYOFBLOCKS[offset].firstFreeBlock->nextBlock = blockPtr;
				blockPtr->previousBlock = ARRAYOFBLOCKS[offset].firstFreeBlock;
				ARRAYOFBLOCKS[offset].firstFreeBlock = blockPtr;
				}
			else
				{
				blockPtr = ARRAYOFBLOCKS[offset].firstFreeBlock;
				}
			}

		I_ASSERT(blockPtr);
		I_ASSERT(blockPtr->free);

    newPtr = RECORDPTR_FROM_OFFSET(blockPtr, blockPtr->free);
    newPtr->blockPtr = blockPtr;

 		blockPtr->currentRecords++;

		if (blockPtr->currentRecords == blockPtr->maxRecords)
			{
			blockPtr->free = 0;
			}
		else if (!newPtr->link.nextFree)
			{
			//we just added a previously unused record so link it up
			blockPtr->free = sizeof(blockHeader) +
			blockPtr->currentRecords * blockPtr->recordSize;
			}               
		}

	I_ASSERT(newPtr);

	newPtr->link.sig = REC_SIG_VALUE;
	
  //if blockPtr is NULL, we pulled the record off the free list so
  //it already has the blockPtr field set.

	return PTR_FROM_RECORDHDR(newPtr);
	}
#endif

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        Deallocate
//
// CLASS.......:  None
//
// DESCRIPTION.:  Free the memory in the block.  If the block is currently 
//                                                              not the first block on the free list, place the block on
//                                                              the free list.
//
// PARAMETERS..:  void * recordPtr:  record to be freed.
//
// RETURN VALUE:        non-zero if successful else zero.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
#ifndef VABSHELL
BOOL Deallocate(
	void * recordPtr)
	{
	BHPtr                                           blockPtr;
	unsigned                                        i;
	unsigned                                        offset = 0;
	RHPtr rhPtr;
  BOOL rVal = FALSE;
  
	if(GLOBALPOINTER)
		{
    if(ValidRecordPtr(recordPtr))
    	{
    
			blockPtr = GETBLOCKPTR(recordPtr);

			I_ASSERT(blockPtr);
      
			i = blockPtr->recordSize - sizeof(recordHeader);
			if(i > STARTINGSIZE)
				offset =(unsigned) ((i-STARTINGSIZE-1)/ BYTEBOUNDARY)+1;

			//link up the free list
			rhPtr = GETRECORDPTR(recordPtr);
			rhPtr->link.nextFree = ARRAYOFBLOCKS[offset].freeList;
			ARRAYOFBLOCKS[offset].freeList = rhPtr;
			
			rVal=TRUE;
      }
    else
    	{
      ADBG_PRINT_E("Attempting to Deallocate an invalid Pointer.");      
      }
		}
	else
		{
    I_ASSERT(FALSE);
    }
    
  return rVal;
	}
#endif

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        CleanUpBlocks
//
// CLASS.......:  None
//
// DESCRIPTION.:  called before program termination to perform neccessary
//                                                                      cleanup.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void CleanUpBlocks()
	{
	BHPtr                                  blockPtr;
	BHPtr                                  deleteBlock;
	unsigned                               i;


	if(GLOBALPOINTER)
		{
		for(i=0;i<arraySize;i++)
			{
			blockPtr = (BHPtr)ARRAYOFBLOCKS[i].firstBlock;
			while(blockPtr)
				{
				deleteBlock = blockPtr;
				blockPtr = deleteBlock->nextBlock;
				DeleteBlock(deleteBlock);
				}
			}

		GlobalFree((HGLOBAL)ARRAYOFBLOCKS);
		
		#ifdef ADBG_DEBUG
		if(GLOBALPOINTER->blockCount > 0)
			I_PRINT("All blocks have not been freed.");
		else if(GLOBALPOINTER->blockCount < 0)
			I_PRINT("Some blocks have been freed multiple times.");
		#endif

		DESTROYGLOBALSTRUCTURE;
		GLOBALPOINTER = NULL;
		}

	}


/////////////////////////////////////////////////////////////
// FUNCTION:                    DeleteBlock
//                                               
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void DeleteBlock(
	BHPtr blockPtr)
	{
	GlobalFree((HGLOBAL)blockPtr);

	#ifdef ADBG_DEBUG
	GLOBALPOINTER->blockCount--;    
	#endif
	}

