#include <math.h>
#include "almsys.h"

#include "memlib.h"
#include "intdebug.h"

#define WINDOWSBYTEBOUNDARY                             16
#define BYTEBOUNDARY                                            16
#define MAXBLOCKSIZE                                                    0xffff
#define MINBLOCKSIZE                                                    4096
#define DESIREDRECORDS                                          10
#define STARTINGSIZE                                                    BYTEBOUNDARY
#define ENDINGSIZE                1024


I_ENABLE_ASSERT;

struct blockHeader{
	unsigned                                maxRecords;
	unsigned                                currentRecords;
	unsigned                                free;           //points to the next never before used record in the block
	unsigned                                recordSize;
	blockHeader far*                        nextBlock;
	blockHeader far*                        previousBlock;
	};

typedef blockHeader far* BHPtr;

struct recordHeader
	{
	recordHeader far* nextFree;
	};

typedef recordHeader far* RHPtr;


struct listHeader{
	BHPtr firstBlock;
	BHPtr firstFreeBlock;
	RHPtr freeList;
	};

typedef listHeader far* LHPtr;


//Prototypes
BHPtr NewBlock(unsigned size);
LHPtr CreateArrayOfHandles();
void DeleteBlock(BHPtr blockPtr);

struct ARRAYHEADER{
	LHPtr arrayOfBlocks;
	long blockCount;
	};

typedef ARRAYHEADER far* AHPtr;

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

#define GETBLOCKPTR(x) ((BHPtr)((long)x & 0xFFFF0000L))

/////////////////////////////////////////////////////////////////////////////
//   GetGlobalBlockArrayAddr
//
// Returns the current value of arrayOfBlocks for the system.  Needs to be
// passed to the OIP during init of the microApp.
/////////////////////////////////////////////////////////////////////////////
void far * GetGlobalBlockArrayAddr()
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
  void far * theAddr)
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
void InitializeBlocks()
	{
	if(!GLOBALPOINTER)
		{
		CREATEGLOBALSTRUCTURE;
		
		ARRAYOFBLOCKS=CreateArrayOfHandles();
		
		#ifdef ADBG_DEBUG
		GLOBALPOINTER->blockCount = 0;
		#endif
		}
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
// RETURN VALUE:  BHPtr far*:  pointer to the array which was just
//                                                                      allocated.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
LHPtr CreateArrayOfHandles()
	{
	LHPtr                   arrayPtr;
	HANDLE                                  arrayData;

	arraySize = unsigned(ceil((double)((double)(ENDINGSIZE - STARTINGSIZE) /
		BYTEBOUNDARY)))+1;
	arrayData = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
		 arraySize * sizeof(listHeader));
	arrayPtr = (LHPtr)GlobalLock(arrayData);
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
	GLOBALHANDLE            blockHandle;
	BHPtr                                           blockPtr;
	unsigned                                recordSize;
	unsigned long           blockSize = 0L;
	unsigned                                numberRecords;

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
	blockHandle = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT, blockSize);
	blockPtr = (BHPtr) GlobalLock(blockHandle);
	blockPtr->free =sizeof(blockHeader);
	blockPtr->recordSize = recordSize;
	blockPtr->maxRecords = numberRecords;

	#ifdef ADBG_DEBUG
	GLOBALPOINTER->blockCount++;
	#endif

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
// RETURN VALUE:  void far*: pointer to the new record.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
#ifndef VABSHELL
void far* Allocate(
	unsigned size)
	{
	BHPtr                        blockPtr;
	RHPtr                        newPtr = NULL;
	unsigned                     offset = 0;
	HANDLE                       handle;
	WORD                         selector;

	if(!GLOBALPOINTER)
		InitializeBlocks();

	if(size > STARTINGSIZE)
		offset =(int)((size-STARTINGSIZE-1)/BYTEBOUNDARY) + 1;

	if(offset >= arraySize)
		{
		selector = SELECTOROF(ARRAYOFBLOCKS);
		
		handle = (HANDLE) LOWORD(GlobalHandle(selector));
		GlobalUnlock(handle);
		handle = GlobalReAlloc(handle, (offset+1) * sizeof(listHeader),
			GMEM_MOVEABLE | GMEM_ZEROINIT);
		
		ARRAYOFBLOCKS= (LHPtr)GlobalLock(handle);
		
		arraySize = offset+1;
		}
		
	if(ARRAYOFBLOCKS[offset].freeList)
		{
		newPtr = ARRAYOFBLOCKS[offset].freeList;
		ARRAYOFBLOCKS[offset].freeList = newPtr->nextFree;
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

		newPtr =(RHPtr)MAKELONG(blockPtr->free, HIWORD((long)blockPtr));
		blockPtr->currentRecords++;

		if (blockPtr->currentRecords == blockPtr->maxRecords)
			{
			blockPtr->free = 0;
			}
		else if (!newPtr->nextFree)
			{
			//we just added a previously unused record so link it up
			blockPtr->free = sizeof(blockHeader) +
			blockPtr->currentRecords * blockPtr->recordSize;
			}               
		}

	I_ASSERT(newPtr);

	return newPtr;
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
// PARAMETERS..:  void far* recordPtr:  record to be freed.
//
// RETURN VALUE:        non-zero if successful else zero.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
#ifndef VABSHELL
BOOL Deallocate(
	void far* recordPtr)
	{
	BHPtr                                                   blockPtr;
	unsigned                                        i;
	unsigned                                        offset = 0;

	if(GLOBALPOINTER)
		{
		blockPtr = GETBLOCKPTR(recordPtr);

		i = blockPtr->recordSize - sizeof(recordHeader);
		if(i > STARTINGSIZE)
			offset =(unsigned) ((i-STARTINGSIZE-1)/ BYTEBOUNDARY)+1;

		//link up the free list
		((RHPtr)recordPtr)->nextFree = ARRAYOFBLOCKS[offset].freeList;
		ARRAYOFBLOCKS[offset].freeList = (RHPtr)recordPtr;
		
		return TRUE;
		}
	else
		return FALSE;
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
	HANDLE                                 handle;
	WORD                                   selector;

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

		selector = SELECTOROF((long)ARRAYOFBLOCKS);
		handle = (HANDLE) LOWORD(GlobalHandle(selector));
		GlobalUnlock(handle);
		GlobalFree(handle);
		
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
	HANDLE                                handle;
	WORD                                  selector;

	selector = SELECTOROF((long)blockPtr);
	handle = (HANDLE) LOWORD(GlobalHandle(selector));
	GlobalUnlock(handle);
	GlobalFree(handle);

	#ifdef ADBG_DEBUG
	GLOBALPOINTER->blockCount--;    
	#endif
	}

