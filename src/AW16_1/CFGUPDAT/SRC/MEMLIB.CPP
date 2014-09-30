#include "windows.h"
#include "memlib.h"
#include "math.h"

#define WINDOWSBYTEBOUNDARY				16
#define BYTEBOUNDARY     					16
#define MAXBLOCKSIZE							0xffff
#define MINBLOCKSIZE							4096
#define DESIREDRECORDS						10
#define STARTINGSIZE							BYTEBOUNDARY
#define ENDINGSIZE                1024


typedef struct BHTag{
	unsigned 				maxRecords;
	unsigned				currentRecords;
	unsigned				free;
	unsigned				recordSize;
	BHTag far* 			nextBlock;
	BHTag far* 			previousBlock;
	} blockHeader;

typedef blockHeader far* BHPtr;

typedef struct{
	unsigned	free;
	}	recordHeader;

typedef recordHeader far* RHPtr;


BHPtr NewBlock(unsigned size);
BHPtr far* CreateArrayOfHandles();

BHPtr far* arrayOfBlocks = NULL;
unsigned  arraySize;

/////////////////////////////////////////////////////////////////////////////
//   GetGlobalBlockArrayAddr
//
// Returns the current value of arrayOfBlocks for the system.  Needs to be
// passed to the OIP during init of the microApp.
/////////////////////////////////////////////////////////////////////////////
void far * GetGlobalBlockArrayAddr()
  {
  return arrayOfBlocks;
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
  arrayOfBlocks = (BHPtr far*)theAddr;
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
// FUNCTION....:	Initialize
//
// CLASS.......:  None
//
// DESCRIPTION.:  performs neccessary initialization to set up for the
//									memlib routines.
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
	if(!arrayOfBlocks)
		arrayOfBlocks=CreateArrayOfHandles();
	}



/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CreateArrayOfHandles
//
// CLASS.......:  None
//
// DESCRIPTION.:  Sets up the initial array of pointers to memory blocks.
//									all pointers are initially set to null.
//
// PARAMETERS..:
//
// RETURN VALUE:  BHPtr far*:  pointer to the array which was just
//									allocated.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BHPtr far* CreateArrayOfHandles()
	{
	BHPtr far*       		arrayPtr;
	HANDLE					arrayData;

	arraySize = unsigned(ceil((double)((double)(ENDINGSIZE - STARTINGSIZE) /
		BYTEBOUNDARY)))+1;
	arrayData = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
		 arraySize * sizeof(BHPtr));
	arrayPtr = (BHPtr far*)GlobalLock(arrayData);
	return arrayPtr;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	NewBlock
//
// CLASS.......:  None
//
// DESCRIPTION.:  Allocates a big chunk of memory from Windows and
//									sets up the block header pointer.
//
// PARAMETERS..:  BHPtr:  points to the block which was just allocated.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BHPtr NewBlock(unsigned size)
	{
	GLOBALHANDLE		blockHandle;
	BHPtr						blockPtr;
	unsigned				recordSize;
	unsigned long		blockSize = 0L;
	unsigned				numberRecords;

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
	GlobalUnlock(blockHandle);
	return blockPtr;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Allocate
//
// CLASS.......:  None
//
// DESCRIPTION.:  Allocates a record, or section of memory, from one the
//									blocks in the arrayOfBlocks.
//
// PARAMETERS..:  unsigned size: size of record to allocate
//
// RETURN VALUE:  void far*: pointer to the new record.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void far* Allocate(
	unsigned size)
	{
	BHPtr								blockPtr;
	BHPtr								tempBlockPtr;
	RHPtr								tempPtr;
	void far* 					newPtr;
	unsigned					offset = 0;
	HANDLE				handle;
	HANDLE						selector;

	if(!arrayOfBlocks)
		arrayOfBlocks = CreateArrayOfHandles();
	if(size > STARTINGSIZE)
		offset =(int)((size-STARTINGSIZE-1)/BYTEBOUNDARY) + 1;

	if(offset >= arraySize)
		{
		selector = HIWORD((long)arrayOfBlocks);
		handle = GlobalHandle(selector);
		GlobalUnlock(handle);
		handle = GlobalReAlloc(handle, (offset+1) * sizeof(BHPtr),
			GMEM_MOVEABLE | GMEM_ZEROINIT);
		arrayOfBlocks = (BHPtr far*)GlobalLock(handle);
		arraySize = offset+1;
		}
	blockPtr =  *(arrayOfBlocks + offset);
	if(!blockPtr)
		{
		blockPtr =(NewBlock(STARTINGSIZE +offset * BYTEBOUNDARY));
		*(arrayOfBlocks + offset) = blockPtr;
		}
	while(blockPtr->nextBlock)
		{
		blockPtr =(BHPtr)blockPtr->nextBlock;
		}
	if(! blockPtr->free)
		{
		tempBlockPtr =blockPtr;
		blockPtr->nextBlock =NewBlock(blockPtr->recordSize -
			sizeof(recordHeader));
		blockPtr = blockPtr->nextBlock;
		blockPtr->previousBlock =tempBlockPtr;
		}

	newPtr =(void far*)MAKELONG(blockPtr->free, HIWORD((long)blockPtr));
	tempPtr =(RHPtr)newPtr;
	blockPtr->currentRecords++;

	if (blockPtr->currentRecords == blockPtr->maxRecords)
		{
		blockPtr->free = 0;
		}
	else if (! tempPtr->free)
		{
		blockPtr->free = sizeof(blockHeader) +
		blockPtr->currentRecords * blockPtr->recordSize;
		}
	else
		{
		blockPtr->free = tempPtr->free;
		}

	return newPtr;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Deallocate
//
// CLASS.......:  None
//
// DESCRIPTION.:  Free the memory in the block
//
// PARAMETERS..:  void far* recordPtr:	record to be freed.
//
// RETURN VALUE:	non-zero if successful else zero.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL Deallocate(
	void far* recordPtr)
	{
	BHPtr							blockPtr;
	unsigned 					tempFree;
	RHPtr							theRecord;
	unsigned 					 i;
	HANDLE 						handle;
	HANDLE 						selector;
	unsigned					offset = 0;

	if(arrayOfBlocks)
		{
		blockPtr = (BHPtr)((long)recordPtr & 0xFFFF0000L);
		theRecord = (RHPtr)recordPtr;
		tempFree = blockPtr->free;
		blockPtr->free = LOWORD((long)recordPtr);
		theRecord->free = tempFree;
		blockPtr->currentRecords--;
		if (!blockPtr->currentRecords)
			{
			if(blockPtr->nextBlock)
				((BHPtr)(blockPtr->nextBlock))->previousBlock =
					blockPtr->previousBlock;
			if (blockPtr->previousBlock)
				((BHPtr)(blockPtr->previousBlock))->nextBlock =
					blockPtr->nextBlock;
			else
				{
				i = blockPtr->recordSize - sizeof(recordHeader);
				if(i > STARTINGSIZE)
					offset =(unsigned) ((i-STARTINGSIZE-1)/ BYTEBOUNDARY)+1;
				*(arrayOfBlocks + offset) = blockPtr->nextBlock;
				}
			selector = HIWORD((long)recordPtr);
			handle = GlobalHandle(selector);
			GlobalUnlock(handle);
			GlobalFree(handle);
			}
		return TRUE;
		}
	else
		return FALSE;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CleanUpBlocks
//
// CLASS.......:  None
//
// DESCRIPTION.:  called before program termination to perform neccessary
//									cleanup.
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
	BHPtr						blockPtr;
	unsigned				i;
	HANDLE 					handle;
	HANDLE 					selector;

	if(arrayOfBlocks)
		{
		for(i=0;i<arraySize;i++)
			{
			blockPtr = (BHPtr)*(arrayOfBlocks + i);
			while(blockPtr)
				{
				selector = HIWORD((long)blockPtr);
				blockPtr = (BHPtr)blockPtr->nextBlock;
				handle = GlobalHandle(selector);
				GlobalUnlock(handle);
				GlobalFree(handle);
				}
			}
		selector = HIWORD((long)arrayOfBlocks);
		handle = GlobalHandle(selector);
		GlobalUnlock(handle);
		GlobalFree(handle);
		arrayOfBlocks = NULL;
		}
	}









