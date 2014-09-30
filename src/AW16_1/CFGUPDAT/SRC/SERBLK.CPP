#include <windows.h>
#include <mem.h>
#include <dos.h>
#include "serdef.h"
#include "debug.h"
#include "ememstr.h"
#include "serblk.h"
#include "sereclib.h"
#include "system.h"
#include "limits.h"

#define SEGMENT_SIZE    65535L

#define MAX_IO_SIZE     0X8000

//#define DEBUGFLAG 1

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        operator new
//
// CLASS.......:        SERIUSBLOCK
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void far * SERIUSBLOCK::operator new(
	size_t  size)
	{

	return SYSTEM::GetFileObject()->GetSeriusBlocks()->AddRecord(size);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:operator new
//
// CLASS.......:SERIUSBLOCK
//
// DESCRIPTION.: This version of the new operator is used to initialize
//               the virtual table pointers of a record read from disk.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void far * SERIUSBLOCK::operator new(
	size_t                  /*size*/,
	void far*               objPtr)
	{

	return (objPtr);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        operator new
//
// CLASS.......:        SERIUSBLOCK
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void far * SERIUSBLOCK::operator new(
	size_t                  size,
	SERUID      recBlkID)
	{

	return ((LPRECORDBLOCK)GetObject(recBlkID))->AddRecord(size);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:operator delete
//
// CLASS.......:SERIUSBLOCK
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void SERIUSBLOCK::operator delete(
	void far *      objPtr,
	size_t                  size)
	{

	((LPRECORDBLOCK)GetObject(((RECORDPREFIX far*)objPtr - 1)->recBlockUID))->
	 DeleteRecord(objPtr, size);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SERIUSBLOCK (constructor)
//
// CLASS.......: SERIUSBLOCK
//
// DESCRIPTION.: This is the default constructor.  No data space is allocated
//                                                       by this constructor.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
SERIUSBLOCK::SERIUSBLOCK(
	void)
	{

	this->byteCount = 0;
	this->theData = NULL;
	this->blockFileID = -1;
	this->blockMemID = -1;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        SERIUSBLOCK (constructor)
//
// CLASS.......:        SERIUSBLOCK
//
// DESCRIPTION.:  This is the constructor normally used to create a new serius
//                                                              block.
//
// PARAMETERS..:  count - the number of records/bytes of storage to allocate
//                                                                      with the new block.
//
//                                                              recordSize - the size in bytes of the records to be stored
//                                                                      in the serius block.  If recordSize is zero, count is
//                                                                      the number of bytes to be stored in the block, otherwise
//                                                                      count is the number of records to be stored in the block.
//
// RETURN VALUE:  returns a pointer to the serius block record.
//
/////////////////////////////////////////////////////////////////////////////

SERIUSBLOCK::SERIUSBLOCK(
	long int        size)
	{

	this->theData = NULL;
	this->blockFileID = -1;
	this->blockMemID = -1;

	AllocateBlock(size);

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        AllocateBlock
//
// CLASS.......:        SERIUSBLOCK
//
// DESCRIPTION.:  This is the constructor normally used to create a new serius
//                                                              block.
//
// PARAMETERS..:  count - the number of records/bytes of storage to allocate
//                                                                      with the new block.
//
//                                                              recordSize - the size in bytes of the records to be stored
//                                                                      in the serius block.  If recordSize is zero, count is
//                                                                      the number of bytes to be stored in the block, otherwise
//                                                                      count is the number of records to be stored in the block.
//
// RETURN VALUE:  returns a pointer to the serius block record.
//
/////////////////////////////////////////////////////////////////////////////

void far* SERIUSBLOCK::AllocateBlock(
	long    blockSize)
	{
	long int                                found;
	MEMCHUNK                                currChunk;
	short int                               chunkIndex;
	short int                               freeBlockIndex;
	unsigned long           newChunkSize;
	short int                               i;
	LPSERIUSMEMORY  theMemory;

	theMemory = SYSTEM::GetTheMemory();

	if (blockSize > 0)
		{
		this->byteCount = blockSize;
		found = -1;
		for (chunkIndex = 0; chunkIndex < theMemory->chunkCount; chunkIndex++)
			{
			currChunk = theMemory->chunks[chunkIndex];
			if (blockSize <= currChunk.freeSpace)
				{
				for (freeBlockIndex = 0; freeBlockIndex < currChunk.freeBlockCount;
						 freeBlockIndex++)
					 {
					 found = currChunk.freeBlocks[freeBlockIndex].extent - blockSize;
					 if (found >= 0)
						 break;
					 }
				if (found >= 0)
					break;
				}
			}
		if (found > 0)                                  //free block will only be partially used
			{
			this->theData = (char far *)currChunk.theChunk +
			 (U_PTRMATH_T)(currChunk.freeBlocks[freeBlockIndex].start);
			this->blockMemID = currChunk.chunkID;
			(currChunk.freeBlocks[freeBlockIndex].extent) -= blockSize;
			currChunk.freeBlocks[freeBlockIndex].start += blockSize;
			currChunk.freeSpace -= blockSize;
			theMemory->chunks[chunkIndex] = currChunk;
			CheckFreeBlocks(&currChunk);
			}
		else if (found == 0L)                   //free block will be completely used
			{
			this->theData = (char far *)currChunk.theChunk +
			 (U_PTRMATH_T)(currChunk.freeBlocks[freeBlockIndex].start);
			this->blockMemID = currChunk.chunkID;
			currChunk.freeBlockCount -= 1;
			for (i = freeBlockIndex; i < currChunk.freeBlockCount; i++)
				currChunk.freeBlocks[i] = currChunk.freeBlocks[i+1];
			GlobalUnlock(currChunk.freeBlocksHandle);
      if (currChunk.freeBlockCount == 0)
	{
	GlobalFree(currChunk.freeBlocksHandle);
	currChunk.freeBlocksHandle = 0;
	currChunk.freeBlocks = 0;
	}
      else
	{
			  currChunk.freeBlocksHandle = GlobalReAlloc(currChunk.freeBlocksHandle,
			   sizeof(FREEBLOCK)*(currChunk.freeBlockCount), 0L);
			  currChunk.freeBlocks =
				 (FREEBLOCK far *)GlobalLock(currChunk.freeBlocksHandle);
	}
			currChunk.freeSpace -= blockSize;
			theMemory->chunks[chunkIndex] = currChunk;
			CheckFreeBlocks(&currChunk);
			}
		else                   //allocate a new chunk
			{
			if (blockSize <= SEGMENT_SIZE)
				newChunkSize = SEGMENT_SIZE;
			else
				newChunkSize = blockSize;
			currChunk.chunkHandle = GlobalAlloc(GMEM_MOVEABLE, newChunkSize);
			if (blockSize < SEGMENT_SIZE)
        currChunk.chunkExtent = GlobalSize(currChunk.chunkHandle);
      else
        //this assures that the chunk containing a block larger than
        //SEGMENT_SIZE will be freed when the block is freed.
        currChunk.chunkExtent = newChunkSize;
			currChunk.theChunk = GlobalLock(currChunk.chunkHandle);
			currChunk.freeSpace = currChunk.chunkExtent-blockSize;
			if (currChunk.freeSpace)
				{
				currChunk.freeBlocksHandle = GlobalAlloc(GMEM_MOVEABLE,
																								 sizeof(FREEBLOCK));
				currChunk.freeBlocks =
				 (FREEBLOCK far *)GlobalLock(currChunk.freeBlocksHandle);
				currChunk.freeBlocks[0].start = blockSize;
				currChunk.freeBlocks[0].extent = currChunk.freeSpace;
				currChunk.freeBlockCount = 1;
				}
			else
				{
				currChunk.freeBlocks = NULL;
				currChunk.freeBlocksHandle = 0;
				currChunk.freeBlockCount = 0;
				}
			currChunk.chunkID = (++theMemory->chunkIDSeed);
			currChunk.dirtyBlock = FALSE;
			currChunk.unusedFlag = TRUE;
			theMemory->chunkCount += 1;
			if (theMemory->chunksHandle == 0)
				theMemory->chunksHandle  = GlobalAlloc(GMEM_MOVEABLE,
				 theMemory->chunkCount*sizeof(MEMCHUNK));
			else
				theMemory->chunksHandle  = GlobalReAlloc(theMemory->chunksHandle,
				 theMemory->chunkCount*sizeof(MEMCHUNK), 0);
			theMemory->chunks = (MEMCHUNK far*)GlobalLock(theMemory->chunksHandle);
			theMemory->chunks[theMemory->chunkCount-1] = currChunk;
			this->theData = currChunk.theChunk;
			this->blockMemID = currChunk.chunkID;
			CheckFreeBlocks(&currChunk);
			}
		}
	else
		{
		this->byteCount = 0;
		this->theData = NULL;
		this->blockFileID = -1;
		this->blockMemID = -1;

		}

  CheckBlock();

	return this->theData;


	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: AllocateChunck
//
// CLASS.......: SERIUSBLOCK
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
MEMCHUNK far* SERIUSBLOCK::AllocateChunk(
  long            minNumBytes)
  {
  long           chunkSize;
	MEMCHUNK far*  pChunk;
	LPSERIUSMEMORY  theMemory;

	theMemory = SYSTEM::GetTheMemory();

  //Allocate a new chunk record

	theMemory->chunkCount += 1;

	if (theMemory->chunksHandle == 0)
		theMemory->chunksHandle  = GlobalAlloc(GMEM_MOVEABLE,
		 theMemory->chunkCount*sizeof(MEMCHUNK));
	else
		theMemory->chunksHandle  = GlobalReAlloc(theMemory->chunksHandle,
		 theMemory->chunkCount*sizeof(MEMCHUNK), 0);

	theMemory->chunks = (MEMCHUNK far*)GlobalLock(theMemory->chunksHandle);

	pChunk = &(theMemory->chunks[theMemory->chunkCount-1]);


  // now allocate the chunk

	if (minNumBytes <= SEGMENT_SIZE)
		chunkSize = SEGMENT_SIZE;
	else
		chunkSize = minNumBytes;

	pChunk->chunkHandle = GlobalAlloc(GMEM_MOVEABLE, chunkSize);

//	if (minNumBytes < SEGMENT_SIZE)
//     pChunk->chunkExtent = GlobalSize(pChunk->chunkHandle);
//  else
    //this assures that the chunk containing a block larger than
    //SEGMENT_SIZE will be freed when the block is freed.
     pChunk->chunkExtent = chunkSize;

	pChunk->theChunk = GlobalLock(pChunk->chunkHandle);

	pChunk->freeBlocks = NULL;
	pChunk->freeBlocksHandle = 0;
	pChunk->freeBlockCount = 0;
  pChunk->freeSpace = 0;

  pChunk->chunkID = (++theMemory->chunkIDSeed);
	pChunk->dirtyBlock = FALSE;
	pChunk->unusedFlag = TRUE;

	return pChunk;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InitChunkFreeSpace
//
// CLASS.......: SERIUSBLOCK
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SERIUSBLOCK::InitChunkFreeSpace(
  MEMCHUNK far *  pChunk,
  long            usedByteCount)
  {

	pChunk->freeSpace = pChunk->chunkExtent - usedByteCount;

	if (pChunk->freeSpace)
		{
		pChunk->freeBlocksHandle =
     GlobalAlloc(GMEM_MOVEABLE, sizeof(FREEBLOCK));
		pChunk->freeBlocks =
		 (FREEBLOCK far *)GlobalLock(pChunk->freeBlocksHandle);
		pChunk->freeBlocks[0].start = usedByteCount;
		pChunk->freeBlocks[0].extent = pChunk->freeSpace;
		pChunk->freeBlockCount = 1;
		}
	else
		{
		pChunk->freeBlocks       = NULL;
		pChunk->freeBlocksHandle = 0;
		pChunk->freeBlockCount   = 0;
		}

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  Read
//
// CLASS.......:  SERIUSBLOCK
//
// DESCRIPTION.:  Writes the contents of a serius block to the indicated
//                file.  The data is written starting at the current file
//                position.
//
// RETURN VALUE:
//
/////////////////////////////////////////////////////////////////////////////
BOOL SERIUSBLOCK::Read(
	int             hFile,
	LPRECORDBLOCK   theBlocks)
  {
  LPSERIUSBLOCK   pCurrBlock = 0;
  LPSERIUSBLOCK   pNextBlock;
  long            byteCount;
  MEMCHUNK far *  pCurrChunk = 0;
  LPSTR           currentData;


  pCurrBlock = (LPSERIUSBLOCK)theBlocks->NextRec(0);

  byteCount = 0;

	while (0 != pCurrBlock)
    {
		pNextBlock = (LPSERIUSBLOCK)theBlocks->NextRec(pCurrBlock);
    if (!pCurrChunk)
      {
      pCurrChunk = AllocateChunk(pCurrBlock->byteCount);
      currentData = (LPSTR)pCurrChunk->theChunk;
      }
    if (pCurrBlock->byteCount != 0)
      {
      pCurrBlock->theData = currentData;
      pCurrBlock->blockMemID = pCurrChunk->chunkID;
      currentData = currentData + (U_PTRMATH_T)(pCurrBlock->byteCount);
      byteCount = byteCount + pCurrBlock->byteCount;
      }
		if ((pNextBlock != 0) &&
        ((byteCount + pNextBlock->byteCount) > SEGMENT_SIZE))
      {
      ReadChunk(hFile, pCurrChunk, byteCount);
      InitChunkFreeSpace(pCurrChunk, byteCount);
      pCurrChunk = 0;
      byteCount = 0;
			}
    pCurrBlock = pNextBlock;
    }

  ReadChunk(hFile, pCurrChunk, byteCount);

  return TRUE;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ReadChunk
//
// CLASS.......: SERIUSBLOCK
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL SERIUSBLOCK::ReadChunk(
	int            hFile,
  MEMCHUNK far*  pChunk,
  long           byteCount)
	{
	BOOL         result = TRUE;
  char huge *  pData;
  long         readSize;

  if (byteCount <= SEGMENT_SIZE)
		{
	  if (byteCount != _lread(hFile, (LPSTR)pChunk->theChunk, byteCount))
		  {
#if (DEBUGFLAG == 1)
		  MessageBox(NULL, "Error in reading Chunk Data", "File I/O Error", MB_OK);
      DebugBreak();
#endif
			result = FALSE;
		  }
    }
  else
    {
    pData = (char huge *)pChunk->theChunk;
    readSize = byteCount;
    while ((readSize > SEGMENT_SIZE)  && (result == TRUE))
      {
      if (SEGMENT_SIZE != _lread(hFile, (LPSTR)pData, SEGMENT_SIZE))
	      {
#if (DEBUGFLAG == 1)
		    MessageBox(NULL, "Error in reading Block Data", "File I/O Error", MB_OK);
	      DebugBreak();
#endif
				result = FALSE;
	      }
      readSize -= SEGMENT_SIZE;
      pData += SEGMENT_SIZE;
      }
    if (result == TRUE)
      {
      if (readSize != _lread(hFile, (LPSTR)pData, readSize))
	      {
#if (DEBUGFLAG == 1)
		    MessageBox(NULL, "Error in reading Block Data", "File I/O Error", MB_OK);
	      DebugBreak();
#endif
				result = FALSE;
	      }
      }
    }

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        ~SERIUSBLOCK (destructor)
//
// CLASS.......:        SERIUSBLOCK
//
// DESCRIPTION.:  If the block has allocated any memory, that memory is
//                                                              released.
//
// ASSUMPTIONS.:        Assumes that blockMemID is -1 if and only if no memory is
//                                                              currently allocated by the block.
//
/////////////////////////////////////////////////////////////////////////////
SERIUSBLOCK::~SERIUSBLOCK(
	void)
	{
  CheckBlock();

	if (blockMemID != -1)
		ReleaseMemory(blockMemID, byteCount, theData);

  return;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        ReleaseMemory
//
// CLASS.......:        SERIUSBLOCK (private)
//
// DESCRIPTION.:  This function frees the memory used by the block.
//
/////////////////////////////////////////////////////////////////////////////

void SERIUSBLOCK::ReleaseMemory(
	long int                                blockMemID,
	long int                                blockLength,
	void far *                      theData)
	{
	MEMCHUNK                                currChunk;
	short int                               chunkIndex;
	FREEBLOCK                               newFreeBlock;
	short int                               i;
	LPSERIUSMEMORY  theMemory;

	theMemory = SYSTEM::GetTheMemory();

	for (chunkIndex = 0; chunkIndex < theMemory->chunkCount; chunkIndex++)
		if (theMemory->chunks[chunkIndex].chunkID == blockMemID)
			{
			currChunk = theMemory->chunks[chunkIndex];
			break;
			}

	if (chunkIndex < theMemory->chunkCount)
		{
		currChunk.freeSpace += blockLength;
		if (currChunk.chunkExtent == currChunk.freeSpace)               //free entire chunk
			{
			if (currChunk.freeBlocks)
				{
				GlobalUnlock(currChunk.freeBlocksHandle);
				GlobalFree(currChunk.freeBlocksHandle);
				}
			GlobalUnlock(currChunk.chunkHandle);
			GlobalFree(currChunk.chunkHandle);
			for (i = chunkIndex; i < theMemory->chunkCount-1; i++)
				theMemory->chunks[i] = theMemory->chunks[i+1];
			theMemory->chunkCount -= 1;
			if (theMemory->chunkCount == 0)
				{
				GlobalFree(theMemory->chunksHandle);
				theMemory->chunksHandle = 0;
				}
			else
				{
				GlobalUnlock(theMemory->chunksHandle);
				theMemory->chunksHandle = GlobalReAlloc(theMemory->chunksHandle,
				 theMemory->chunkCount*sizeof(MEMCHUNK), 0L);
				theMemory->chunks = (MEMCHUNK far *)GlobalLock(theMemory->chunksHandle);
				}
			}
		else
			{
			newFreeBlock.start = LOWORD((unsigned long)theData);
			newFreeBlock.extent = blockLength;
			CompactFree(&currChunk, newFreeBlock);
			theMemory->chunks[chunkIndex] = currChunk;
			}
		blockMemID = -1;
		}

#if (DEBUGFLAG == 1)
	else
    {
		MessageBox(NULL, "Block references memory chunk that does not exist.",
									 "Release Memory Error", MB_OK);
    DebugBreak();
		}
#endif

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        CheckBlock
//
// CLASS.......:        SERIUSBLOCK
//
// DESCRIPTION.:  This function checks to see that the blocks data does
//                                                              not overlap any free space.
//
/////////////////////////////////////////////////////////////////////////////

void SERIUSBLOCK::CheckBlock(
	void)
	{

#if (DEBUGFLAG == 1)

	MEMCHUNK        currChunk;
	short int       chunkIndex;
	short int       freeIndex;
	WORD            freeStart;
	WORD            freeEnd;
	WORD            blockStart;
	WORD            blockEnd;
	LPSERIUSMEMORY  theMemory;

	theMemory = SYSTEM::GetTheMemory();

	for (chunkIndex = 0; chunkIndex < theMemory->chunkCount; chunkIndex++)
		if (theMemory->chunks[chunkIndex].chunkID == this->blockMemID)
			{
			currChunk = theMemory->chunks[chunkIndex];
			break;
			}

	if (chunkIndex < theMemory->chunkCount)
		{
		blockStart = LOWORD((unsigned long)theData);
		blockEnd = blockStart + this->byteCount - 1;

		for (freeIndex = 0; freeIndex<currChunk.freeBlockCount; freeIndex++)
			{
			freeStart = currChunk.freeBlocks[freeIndex].start;
			freeEnd = freeStart + currChunk.freeBlocks[freeIndex].extent - 1;
			if ((blockStart>=freeStart && blockStart<=freeEnd) ||
					(blockEnd>=freeStart && blockEnd<=freeEnd))
	{
				MessageBox(NULL, "Block Overlaps chunk free space.",
									 "CheckBlock Error", MB_OK);
	DebugBreak();
	}
			else if ((freeStart>=blockStart && freeStart<=blockEnd) ||
								 (freeEnd>=blockStart && freeEnd<=blockEnd))
	{
				MessageBox(NULL, "Block Overlaps chunk free space.",
									 "CheckBlock Error", MB_OK);
	DebugBreak();
	}
			}
		}
	else if (this->blockMemID != -1)
    {
		MessageBox(NULL, "Block references memory chunk that does not exist.",
									 "CheckBlock Error", MB_OK);
    DebugBreak();
    }

#else

	return;

#endif

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        CompactFree
//
// CLASS.......:        SERIUSBLOCK
//
// DESCRIPTION.:  This function places a newly freed block of memory in the
//                                                              free list of the chunk from which it came.  If the new free
//                                                              block is adjacent to any other free blocks, the adjacent
//                                                              blocks are combined into a single free block.
//
// PARAMETERS..:        theChunk - a pointer to the memory chunk to which the free
//                                                                      memory belongs.
//
//                                                              newFree - the freeblock record the describes the new free
//                                                                      memory.
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

void SERIUSBLOCK::CompactFree(
	MEMCHUNK far *   theChunk,
	FREEBLOCK                                newFree)
	{
	short                                                   freeIndex;
	FREEBLOCK far   *               tempFree;
	FREEBLOCK far *   preAdjacentBlock = NULL;
	FREEBLOCK far *   postAdjacentBlock = NULL;
	short                                                   postAdjacentIndex;
	short                                                   i;

	for (freeIndex = 0; freeIndex<theChunk->freeBlockCount; freeIndex++)
		{
		tempFree = &theChunk->freeBlocks[freeIndex];
		if ((tempFree->start+tempFree->extent) == newFree.start)
			preAdjacentBlock = tempFree;
		else if (tempFree->start == (newFree.start+newFree.extent))
			{
			postAdjacentIndex = freeIndex;
			postAdjacentBlock = tempFree;
			}
		}

	if (preAdjacentBlock)
		{
		if (!postAdjacentBlock)
			preAdjacentBlock->extent += newFree.extent;
		else
			{
			preAdjacentBlock->extent += (newFree.extent+postAdjacentBlock->extent);
			for (i = postAdjacentIndex; i < theChunk->freeBlockCount-1; i++)
				theChunk->freeBlocks[i] = theChunk->freeBlocks[i+1];
			theChunk->freeBlockCount -= 1;
			GlobalUnlock(theChunk->freeBlocksHandle);
			theChunk->freeBlocksHandle = GlobalReAlloc(theChunk->freeBlocksHandle,
			 sizeof(FREEBLOCK)*(theChunk->freeBlockCount), 0L);
			theChunk->freeBlocks =
			 (FREEBLOCK far *)GlobalLock(theChunk->freeBlocksHandle);
			}
		}
	else if (postAdjacentBlock)
		{
		postAdjacentBlock->start = newFree.start;
		postAdjacentBlock->extent += newFree.extent;
		}
	else
		{
		theChunk->freeBlockCount += 1;
		if (theChunk->freeBlocksHandle == 0)
			{
			theChunk->freeBlocksHandle = GlobalAlloc(GMEM_MOVEABLE,
			 sizeof(FREEBLOCK)*(theChunk->freeBlockCount));
			}
		else
			{
			GlobalUnlock(theChunk->freeBlocksHandle);
			theChunk->freeBlocksHandle = GlobalReAlloc(theChunk->freeBlocksHandle,
			 sizeof(FREEBLOCK)*(theChunk->freeBlockCount), 0L);
			}
		theChunk->freeBlocks =
		 (FREEBLOCK far *)GlobalLock(theChunk->freeBlocksHandle);
		theChunk->freeBlocks[theChunk->freeBlockCount - 1] = newFree;
		}

#if (DEBUGFLAG == 1)
	CheckFreeBlocks(theChunk);
#endif

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        CheckFreeBlocks
//
// CLASS.......:        SERIUSBLOCK
//
// DESCRIPTION.:  This function places a newly freed block of memory in the
//                                                              free list of the chunk from which it came.  If the new free
//                                                              block is adjacent to any other free blocks, the adjacent
//                                                              blocks are combined into a single free block.
//
// PARAMETERS..:        theChunk - a pointer to the memory chunk to which the free
//                                                                      memory belongs.
//
//                                                              newFree - the freeblock record the describes the new free
//                                                                      memory.
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

#pragma argsused  //use because of debugging switch in the followin routine
		  //gets rid of never used parameter warning

void SERIUSBLOCK::CheckFreeBlocks(
	MEMCHUNK far * theChunk)
	{

#if (DEBUGFLAG == 1)

	short            freeIndex;
	FREEBLOCK far *  tempFree;
	short            i;
	long int         tempEnd;
	long int         end;
	FREEBLOCK far *  free;
	long int         totalExtent;

	totalExtent = 0;

	for (freeIndex = 0; freeIndex<theChunk->freeBlockCount; freeIndex++)
		{
		tempFree = &theChunk->freeBlocks[freeIndex];
		tempEnd = tempFree->start + tempFree->extent - 1;
		if (tempEnd > theChunk->chunkExtent)
			{
		  MessageBox(NULL, "Free end beyond chunk extent.",
										 "CheckFreeBlocks Error", MB_OK);
      DebugBreak();
      }
		totalExtent += tempFree->extent;
		for (i = 0; i<theChunk->freeBlockCount; i++)
			if (i != freeIndex)
				{
				free = &theChunk->freeBlocks[i];
				end = free->start + free->extent - 1;
				if ((free->start>=tempFree->start && free->start<=tempEnd) ||
						(end>=tempFree->start && end<=tempEnd))
	  {
					MessageBox(NULL, "Free Space blocks overlap.",
										 "CheckFreeBlocks Error", MB_OK);
	  DebugBreak();
	  }
				else if ((tempFree->start>=free->start && tempFree->start<=end) ||
								 (tempEnd>=free->start && tempEnd<=end))
	  {
					MessageBox(NULL, "Free Space blocks overlap.",
										 "CheckFreeBlocks Error", MB_OK);
	  DebugBreak();
	  }
				}
		}

	if (totalExtent != theChunk->freeSpace)
    {
		MessageBox(NULL, "Free Space total exceeds chunk space.",
							 "CheckFreeBlocks Error", MB_OK);
    DebugBreak();
    }

#else
	return;
#endif

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        ResizeBlock
//
// CLASS.......:  SERIUSBLOCK
//
// DESCRIPTION.:        The resize block method increases or decreases the size
//                                                              of the block by the indicated records/bytes.
//
// PARAMETERS..:  deltaCount - the size in bytes/records that the block
//                                                                      size is to be changed.  A positive value will increase
//                                                                      the size and a negative value will decrease the size.
//
// RETURN VALUE:        The function returns a pointer to the beginning of the
//                                                              new memory if the size was increased, or a pointer to the
//                                                              beginning of the memory if the size was decreased.
//
// SIDE-EFFECTS:        The data stored in the block is moved to a different
//                                                              area of memory.  This invalidates any stored pointers to
//                                                              that memory.
//
// ASSUMPTIONS:         Assumes that AllocateBlock sets the value of byteCount.
//
/////////////////////////////////////////////////////////////////////////////

void far* SERIUSBLOCK::ResizeBlock(
	long int  deltaCount)
	{
	long int   newSize;
	long int   oldSize;
	void far*  dataPtr;
	void far*  oldData;
	long int   oldBlockMemID;
	long int   oldCount;

  CheckBlock();

	if (blockMemID == -1)
		{
		if (deltaCount < 0)
			{
			byteCount = 0;
			this->theData = dataPtr = NULL;
			}
		else
			{
			AllocateBlock(deltaCount);
			dataPtr = this->theData;
			}
		}
	else if (byteCount+deltaCount <= 0)
		{
		ReleaseMemory(blockMemID, this->byteCount, this->theData);
		this->theData = dataPtr = 0;
		byteCount = 0;
		blockMemID = -1;
		}
  else if ((byteCount > SEGMENT_SIZE) &&
		 (byteCount+deltaCount > SEGMENT_SIZE))
    {
    this->theData = ResizeChunk(this->blockMemID, byteCount+deltaCount);
    if (this->theData != 0)
      {
      if (deltaCount > 0)
        dataPtr = ((char huge *)this->theData) + this->byteCount;
			else
        dataPtr = this->theData;
      }
    else
      dataPtr = 0;
    this->byteCount = this->byteCount + deltaCount;
    }
	else
		{
		oldSize = byteCount;
		newSize = byteCount + deltaCount;
		oldData = theData;
		oldBlockMemID = blockMemID;
		oldCount = byteCount;
		AllocateBlock(newSize);
		if (this->theData != 0)
      {
      if (newSize < oldSize)
        _fmemcpy(theData, oldData, (U_PTRMATH_T)(newSize));
      else
        _fmemcpy(theData, oldData, (U_PTRMATH_T)(oldSize));
		  if (deltaCount > 0)
			  dataPtr = (char far*)theData + (U_PTRMATH_T)(oldSize); //return pointer to new space
		  else
			  dataPtr = this->theData;
      }
    else
      dataPtr = 0;
		ReleaseMemory(oldBlockMemID, oldCount, oldData);
		}

  CheckBlock();

	return dataPtr;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ResizeChunk
//
// CLASS.......: SERIUSBLOCK
//
// DESCRIPTION.:
//
//
/////////////////////////////////////////////////////////////////////////////
void far * SERIUSBLOCK::ResizeChunk(
  long  blockMemID,
  long  newCount)
  {
	short           chunkIndex;
	LPSERIUSMEMORY  theMemory;
  MEMCHUNK far *  theChunk;

	theMemory = SYSTEM::GetTheMemory();

	for (chunkIndex = 0; chunkIndex < theMemory->chunkCount; chunkIndex++)
		if (theMemory->chunks[chunkIndex].chunkID == blockMemID)
			{
			theChunk = &theMemory->chunks[chunkIndex];
			break;
			}

	if (theChunk)
    {
    if (theChunk->freeBlocksHandle != 0)
      {
      GlobalFree(theChunk->freeBlocksHandle);
      theChunk->freeBlocks = NULL;
      }
		theChunk->freeBlocksHandle = 0;   //just to be sure
		theChunk->freeBlockCount = 0;      //just to be sure
		theChunk->freeSpace = 0;          //just to be sure
    theChunk->chunkHandle = GlobalReAlloc(theChunk->chunkHandle, newCount, 0L);

    //asssuming that a chunk larger than SEGMENT_SIZE will only contain a
    //single serius block,  set chunkExtent to the number of bytes in the
    //serius block instead of the number of bytes actually reallocated by
    //Windows.  This will prevent us from allocating any extra bytes to
    // another seriusblock and thus allow us to free the chunk as soon as the serius
    //block is sized to zero or deleted.
		theChunk->chunkExtent = newCount;  //don't use GlobalSize here.

		theChunk->theChunk = GlobalLock(theChunk->chunkHandle);
    }

  return theChunk->theChunk;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        DeleteSubBlock
//
// CLASS.......:        SERIUSBLOCK
//
// DESCRIPTION.:  This function deletes a subblock of bytes or records from
//                                                              the block.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:        The block's data is moved to a new location in memory. This
//                                                              invalidates any externally saved pointers to the block's
//                                                              data.
/////////////////////////////////////////////////////////////////////////////

void SERIUSBLOCK::DeleteSubBlock(
	long int                                start,
	long int                                count)
	{
	long int                                originalLength;
	long int                                newSize;
	void far*                               oldData;
	long int                                oldBlockMemID;
	long int                                oldCount;

  CheckBlock();

	oldData = this->theData;
	oldBlockMemID = this->blockMemID;
	oldCount = this->byteCount;

	originalLength = this->byteCount;
	newSize = this->byteCount - count;

	AllocateBlock(newSize);

	if (newSize > 0)
		{
		_fmemcpy(this->theData, oldData, (U_PTRMATH_T)(start-1));
		_fmemcpy((char far *)this->theData+(U_PTRMATH_T)(start-1),
			(char far *)oldData+(U_PTRMATH_T)(start+count-1),
			(U_PTRMATH_T)(originalLength-count-(start-1)));
		}

	ReleaseMemory(oldBlockMemID, oldCount, oldData);

  CheckBlock();

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetByteCount
//
// CLASS.......:        SERIUSBLOCK
//
// DESCRIPTION.:        Returns a count of the number of bytes/records in the
//                                                              block
//
// RETURN VALUE:  The number of bytes/records in the block
//
/////////////////////////////////////////////////////////////////////////////
long int SERIUSBLOCK::GetByteCount(
	void)
	{

	return byteCount;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetData
//
// CLASS.......:  SERIUSBLOCK
//
// DESCRIPTION.:  Returns a pointer to the block's data
//
// RETURN VALUE:  A pointer to the blocks data.
//
/////////////////////////////////////////////////////////////////////////////
void far*       SERIUSBLOCK::GetData(
	void)
	{

	return theData;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        Read
//
// CLASS.......:        SERIUSBLOCK
//
// DESCRIPTION.:  This is the constructor is used to create a serious block
//                                                              read from a file.
//
// PARAMETERS..:  count - the number of records/bytes of storage to allocate
//                                                                      with the new block.
//
//                                                              recordSize - the size in bytes of the records to be stored
//                                                                      in the serius block.  If recordSize is zero, count is
//                                                                      the number of bytes to be stored in the block, otherwise
//                                                                      count is the number of records to be stored in the block.
//
// RETURN VALUE:  returns a pointer to the serius block record.
//
/////////////////////////////////////////////////////////////////////////////
BOOL SERIUSBLOCK::Read(
	int  hFile)
	{
	BOOL         result = TRUE;
	char huge *  pData;
	long         readSize;

	AllocateBlock(byteCount);

	if (byteCount <= SEGMENT_SIZE)
		{
		if (byteCount != _lread(hFile, (LPSTR)theData, byteCount))
			{
#if (DEBUGFLAG == 1)
			MessageBox(NULL, "Error in reading Block Data", "File I/O Error", MB_OK);
			DebugBreak();
#endif
			result = FALSE;
			}
		}
	else
		{
		pData = (char huge *)theData;
		readSize = byteCount;
		while ((readSize > MAX_IO_SIZE)  && (result == TRUE))
			{
			if (MAX_IO_SIZE != _lread(hFile, (LPSTR)pData, MAX_IO_SIZE))
	{
#if (DEBUGFLAG == 1)
				MessageBox(NULL, "Error in reading Block Data", "File I/O Error", MB_OK);
	DebugBreak();
#endif
	result = FALSE;
	}
			readSize -= MAX_IO_SIZE;
			pData += MAX_IO_SIZE;
			}
		if (result == TRUE)
			{
      if (readSize != _lread(hFile, (LPSTR)pData, readSize))
	{
#if (DEBUGFLAG == 1)
		    MessageBox(NULL, "Error in reading Block Data", "File I/O Error", MB_OK);
	DebugBreak();
#endif
	result = FALSE;
	}
      }
    }

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  Write
//
// CLASS.......:  SERIUSBLOCK
//
// DESCRIPTION.:  Writes the contents of a serius block to the indicated
//                file.  The data is written starting at the current file
//                position.
//
// RETURN VALUE:
//
/////////////////////////////////////////////////////////////////////////////
BOOL SERIUSBLOCK::Write(
	int  hFile,
	WORD &writeCount)
	{
	BOOL         result = TRUE;
	WORD         tempCount;
	char huge *  pData;
	long         writeSize;


	if (byteCount <= SEGMENT_SIZE)
		{
		if (byteCount != (tempCount = _lwrite(hFile, (LPSTR)theData, byteCount)))
			{
#if (DEBUGFLAG == 1)
			MessageBox(NULL, "Error in writing Block Data", "File I/O Error", MB_OK);
			DebugBreak();
#endif
			result = FALSE;
			}
		writeCount += tempCount;
		}
	else
    {
    pData = (char huge *)theData;
    writeSize = byteCount;
		while ((writeSize > MAX_IO_SIZE)  && (result == TRUE))
      {
			if (MAX_IO_SIZE != (tempCount = _lwrite(hFile, (LPSTR)pData, MAX_IO_SIZE)))
	{
#if (DEBUGFLAG == 1)
		    MessageBox(NULL, "Error in writing Block Data", "File I/O Error", MB_OK);
	DebugBreak();
#endif
	result = FALSE;
	}
      writeSize -= tempCount;
			pData += MAX_IO_SIZE;
      }
    if (result == TRUE)
      {
      if (writeSize != (tempCount = _lwrite(hFile, (LPSTR)pData, writeSize)))
	{
#if (DEBUGFLAG == 1)
		    MessageBox(NULL, "Error in writing Block Data", "File I/O Error", MB_OK);
	DebugBreak();
#endif
	result = FALSE;
	}
      writeCount += tempCount;
			}
    }

	return result;

	}

