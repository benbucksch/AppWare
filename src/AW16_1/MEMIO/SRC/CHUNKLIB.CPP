#include <almsys.h>
#include <mem.h>
#include <dos.h>

#include "serdef.h"
#include "ememstr.h"
#include "system.h"
#include "chunklib.h"

#define SEGMENT_SIZE    65535L

#define MAX_IO_SIZE     0X8000

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        CheckBlock
//
// CLASS.......:        SERIUSBLOCK
//
// DESCRIPTION.:  This function checks to see that the blocks data does
//                not overlap any free space.
//
/////////////////////////////////////////////////////////////////////////////

void CheckBlock(
  long       chunkID,
  void far*  theData,
  long       byteCount)
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

	theMemory = pTheSystem->GetTheMemory();

  // search for chunk

	for (chunkIndex = 0; chunkIndex < theMemory->chunkCount; chunkIndex++)
		if (theMemory->chunks[chunkIndex].chunkID == chunkID)
			{
			currChunk = theMemory->chunks[chunkIndex];
			break;
			}

  // if chunk is found then check it

	if (chunkIndex < theMemory->chunkCount)
		{
		blockStart = LOWORD((unsigned long)theData);
		blockEnd = blockStart + byteCount - 1;

		for (freeIndex = 0; freeIndex<currChunk.freeBlockCount; freeIndex++)
			{
			freeStart = currChunk.freeBlocks[freeIndex].start;
			freeEnd = freeStart + currChunk.freeBlocks[freeIndex].extent - 1;
			if ((blockStart>=freeStart && blockStart<=freeEnd) ||
					(blockEnd>=freeStart && blockEnd<=freeEnd))
	      {
				MessageBox(NULL, "Block Overlaps chunk free space.",
									 "CheckBlock Error", MB_OK|MB_TASKMODAL);
	      DebugBreak();
	      }
			else if ((freeStart>=blockStart && freeStart<=blockEnd) ||
								 (freeEnd>=blockStart && freeEnd<=blockEnd))
	      {
				MessageBox(NULL, "Block Overlaps chunk free space.",
									 "CheckBlock Error", MB_OK|MB_TASKMODAL);
	      DebugBreak();
	      }
			}
		}
	else if (chunkID != -1)
    {
		MessageBox(NULL, "Block references memory chunk that does not exist.",
									 "CheckBlock Error", MB_OK|MB_TASKMODAL);
    DebugBreak();
    }

#else

	return;

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

void CheckFreeBlocks(
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
										 "CheckFreeBlocks Error", MB_OK|MB_TASKMODAL);
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
										 "CheckFreeBlocks Error", MB_OK|MB_TASKMODAL);
	  DebugBreak();
	  }
				else if ((tempFree->start>=free->start && tempFree->start<=end) ||
								 (tempEnd>=free->start && tempEnd<=end))
	  {
					MessageBox(NULL, "Free Space blocks overlap.",
										 "CheckFreeBlocks Error", MB_OK|MB_TASKMODAL);
	  DebugBreak();
	  }
				}
		}

	if (totalExtent != theChunk->freeSpace)
    {
		MessageBox(NULL, "Free Space total exceeds chunk space.",
							 "CheckFreeBlocks Error", MB_OK|MB_TASKMODAL);
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
//                free list of the chunk from which it came.  If the new free
//                block is adjacent to any other free blocks, the adjacent
//                blocks are combined into a single free block.
//
// PARAMETERS..:  theChunk - a pointer to the memory chunk to which the free
//                           memory belongs.
//
//                newFree - the freeblock record the describes the new free
//                          memory.
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

void CompactFree(
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
// FUNCTION....: AllocateBlock
//
// DESCRIPTION.:  This is the constructor normally used to create a new serius
//                block.
//
// PARAMETERS..:  count - the number of records/bytes of storage to allocate
//                        with the new block.
//
//                recordSize - the size in bytes of the records to be stored
//                   in the serius block.  If recordSize is zero, count is
//                   the number of bytes to be stored in the block, otherwise
//                   count is the number of records to be stored in the block.
//
// RETURN VALUE:  returns a pointer to the serius block record.
//
/////////////////////////////////////////////////////////////////////////////
void far* AllocateBlock(
	long    blockSize,
  long    &chunkID)
	{
	long int        found;
	MEMCHUNK        currChunk;
	short int       chunkIndex;
	short int       freeBlockIndex;
	unsigned long   newChunkSize;
	short int       i;
	LPSERIUSMEMORY  theMemory;
  void far*       theData;

	theMemory = pTheSystem->GetTheMemory();

	if (blockSize > 0)
		{
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


		if (found > 0)          //free block will only be partially used
			{
			theData = (char far *)currChunk.theChunk +
			 (U_PTRMATH_T)(currChunk.freeBlocks[freeBlockIndex].start);
			chunkID = currChunk.chunkID;
			(currChunk.freeBlocks[freeBlockIndex].extent) -= blockSize;
			currChunk.freeBlocks[freeBlockIndex].start += blockSize;
			currChunk.freeSpace -= blockSize;
			theMemory->chunks[chunkIndex] = currChunk;
			CheckFreeBlocks(&currChunk);
			}
		else if (found == 0L)   //free block will be completely used
			{
			theData = (char far *)currChunk.theChunk +
			 (U_PTRMATH_T)(currChunk.freeBlocks[freeBlockIndex].start);
			chunkID = currChunk.chunkID;
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
			theData = currChunk.theChunk;
			chunkID = currChunk.chunkID;
			CheckFreeBlocks(&currChunk);
			}
		}
	else
		{
		theData = 0;
		chunkID = -1;
		}

  CheckBlock(chunkID, theData, blockSize);

	return theData;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: AllocateChunck
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
MEMCHUNK far* AllocateChunk(
  long            minNumBytes)
  {
  long           chunkSize;
	MEMCHUNK far*  pChunk;
	LPSERIUSMEMORY  theMemory;

	theMemory = pTheSystem->GetTheMemory();

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
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void InitChunkFreeSpace(
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
// FUNCTION....: ReadChunk
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL ReadChunk(
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
		  MessageBox(NULL, "Error in reading Chunk Data", "File I/O Error", MB_OK|MB_TASKMODAL);
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
		    MessageBox(NULL, "Error in reading Block Data", "File I/O Error", MB_OK|MB_TASKMODAL);
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
		    MessageBox(NULL, "Error in reading Block Data", "File I/O Error", MB_OK|MB_TASKMODAL);
	      DebugBreak();
#endif
				result = FALSE;
	      }
      }
    }

  InitChunkFreeSpace(pChunk, byteCount);

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        ReleaseMemory
//
// DESCRIPTION.:  This function frees the memory used by the block.
//
/////////////////////////////////////////////////////////////////////////////
void ReleaseBlock(
	long int    blockMemID,
	long int    blockLength,
	void far *  theData)
	{
	MEMCHUNK                                currChunk;
	short int                               chunkIndex;
	FREEBLOCK                               newFreeBlock;
	short int                               i;
	LPSERIUSMEMORY  theMemory;

	theMemory = pTheSystem->GetTheMemory();

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
									 "Release Memory Error", MB_OK|MB_TASKMODAL);
    DebugBreak();
		}
#endif

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ResizeChunk
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void far * ResizeChunk(
  long  blockMemID,
  long  newCount)
  {
	short           chunkIndex;
	LPSERIUSMEMORY  theMemory;
  MEMCHUNK far *  theChunk;

	theMemory = pTheSystem->GetTheMemory();

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




