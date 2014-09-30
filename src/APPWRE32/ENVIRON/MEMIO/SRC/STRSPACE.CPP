#include "almsys.h"
#include "intdebug.h"

#include "strngstr.h"
#include "strspace.h"

#define INITIALBLOCKARRAYSIZE 10
#define BLOCKISUSED	'\0'
#define BLOCKISFREE 1
#define MINIMUMBYTES ((short)(sizeof(FREEHEAD) + sizeof(FREETAIL)))

// SKJ 1-09 : Create a pointer from a handle and offset
#define STR_PTR(Ptr, offset) ((void*)(((LPSTR)Ptr) + offset))

// Get the Offset within the committed default heap block.
// if the offset calculation yields a value > 64K, announce an error.
// We should look into changing memory manager implementation to handle offsets into
// blocks of committed memory that is not constricted to archaic 64K limitation.
// - SKJ 01-10-95
#ifdef ADBG_DEBUG
unsigned short	STR_OFFSET(
	void*		segPtr,
	void*		offptr
	)
	{
	unsigned long		nDiff	= ((LPSTR)offptr - (LPSTR)segPtr);

	I_ASSERT(segPtr < offptr);
	I_ASSERT(nDiff <= 0xffff);

  return (unsigned short) nDiff;
	}
#else
#define STR_OFFSET(segPtr, offptr) (LOWORD(((LPSTR)offptr) - ((LPSTR)segPtr))) // get the lower 16 but value
#endif

struct STRINGSFILEHDR
	{
	unsigned short	blockSize;
	short  					blockCount;
	};

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	STRINGSPACE
//
// CLASS.......:  STRING
//
// DESCRIPTION.:  default constuctor.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
/////////////////////////////////////////////////////////////////////////////
STRINGSPACE::STRINGSPACE(
	void)
	{

	blockSize = 0;
	blockCount = 0;
	maxBlocks	 = 0;
	pBlockArray = NULL;
	hBlockArray = NULL;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	STRINGSPACE
//
// CLASS.......:  STRING
//
// DESCRIPTION.:  default constuctor.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
/////////////////////////////////////////////////////////////////////////////
STRINGSPACE::STRINGSPACE(
	WORD	blockSize)
	{

	InitStringMemory(blockSize);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	~STRINGSPACE
//
// CLASS.......:  STRING
//
// DESCRIPTION.:  destructor.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
/////////////////////////////////////////////////////////////////////////////
STRINGSPACE::~STRINGSPACE()
	{

	// Do NOT call FreeStringMemory() here because:  1)  it is being called
	// in SYSTEMMIN::Cleanup, and 2) freeing memory during cleanup code
	// of global objects (such as SYSTEMMIN) causes exception 13 (but only
	// if Winsight or Dr. Watson is present).

	}



/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Read
//
// CLASS.......:  STRINGSPACE
//
// DESCRIPTION.:  default constuctor.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
/////////////////////////////////////////////////////////////////////////////
#ifndef VABSHELL
void STRINGSPACE::Read(
	HANDLE	hFile)
	{
	STRINGSFILEHDR	stringsHdr;
	int 						i;
  DWORD readSize;
  
	ReadFile(hFile, &stringsHdr, sizeof(stringsHdr), &readSize, NULL);

	if (sizeof(STRINGSFILEHDR) == readSize)
		{
		blockSize = stringsHdr.blockSize;
		blockCount = stringsHdr.blockCount;
		maxBlocks = (short)(blockCount + blockCount%INITIALBLOCKARRAYSIZE);
		hBlockArray = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,
		 maxBlocks*sizeof(STRINGBLOCK));
		pBlockArray = (LPSTRINGBLOCK)GlobalLock(hBlockArray);
		for (i=0; i<blockCount; i++)
			{
			DWORD readSize;
			pBlockArray[i].hBlock = GlobalAlloc(GMEM_MOVEABLE, blockSize);
			pBlockArray[i].pBlock =
			 (LPBLOCKHEADER)GlobalLock(pBlockArray[i].hBlock);

			ReadFile(hFile, pBlockArray[i].pBlock, blockSize, &readSize, NULL);
			if (blockSize != readSize)
				MessageBox(NULL, "Error reading String Block.", "String I/0 Error",
				 MB_OK|MB_TASKMODAL);
			}
		}
	else
		{
		blockSize = 0;
		blockCount = 0;
		maxBlocks	 = 0;
		pBlockArray = NULL;
		hBlockArray = NULL;
		}

	}
#endif

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	StringAlloc
//
// CLASS.......:  STRING (private)
//
// DESCRIPTION.:  Allocates space to store a character string.  The size
//								of the space will be at least MINIMUMBYTES.  Space is
//								allocated for a string header in addition to the space
//								needed to store the character string.
//
// PARAMETERS..:	size - the number of characters in the character string
//                	including the null char.
//
// RETURN VALUE:  A pointer to the actual character string.
//
/////////////////////////////////////////////////////////////////////////////
#ifndef VABSHELL
LPSTR STRINGSPACE::StringAlloc(
	int      size,
	short	 & blockIndex,
	WORD   & offset)
	{
	LPFREEHEAD 			currFree;
	LPFREEHEAD  		previousHead;
	LPFREEHEAD  		nextHead;
	LPFREEHEAD			newFree;
	LPBLOCKHEADER  	blkHdr;
  LPBYTE          pTemp;
  LPSTR           pNewString;
	int							i;
	int							found = MINIMUMBYTES-1;

	size = size + sizeof(STRHEAD);

	if (size < MINIMUMBYTES)
		size = MINIMUMBYTES;

	for (i=0; i<blockCount; i++)
		{
		blkHdr =	pBlockArray[i].pBlock;
		if (blkHdr->freeSpace >= size)
			{
			if (blkHdr->firstFree == 0)
				continue;

			currFree = (LPFREEHEAD)STR_PTR(blkHdr, blkHdr->firstFree);
			do
				{
				found = -currFree->size - size;
				if (found >= MINIMUMBYTES)
					break;
				currFree = (LPFREEHEAD)STR_PTR(blkHdr, currFree->next);
				}
			while (currFree != (LPFREEHEAD)STR_PTR(blkHdr, blkHdr->firstFree));
			}
		if (found >= MINIMUMBYTES)
			break;
		}

	if (found < MINIMUMBYTES) 							//allocate a new block
		{
		if (blockCount == maxBlocks)
			{
			maxBlocks += (short)10;
			GlobalUnlock(hBlockArray);
			hBlockArray = GlobalReAlloc(hBlockArray,
																	maxBlocks*sizeof(STRINGBLOCK), GMEM_MOVEABLE);
			pBlockArray = (LPSTRINGBLOCK)GlobalLock(hBlockArray);
			}
		InitBlock(pBlockArray[blockCount]);
		blkHdr = pBlockArray[blockCount].pBlock;

		currFree = (LPFREEHEAD)STR_PTR(blkHdr, blkHdr->firstFree);
		blockIndex = blockCount;
		blockCount++;
		found = -currFree->size - size;
		}
	else
		blockIndex = (short)i;

	if (found > MINIMUMBYTES) //only part of block will be used
		{
		newFree = (LPFREEHEAD)STR_PTR(currFree, size);

    pTemp = (LPBYTE) STR_PTR(blkHdr, currFree->previous);
		if (pTemp == (LPBYTE)currFree) 
			previousHead = newFree;
		else
			previousHead = (LPFREEHEAD)pTemp; 

    pTemp = (LPBYTE) STR_PTR(blkHdr, currFree->next);
		if (pTemp == (LPBYTE)currFree)
			nextHead = newFree;
		else
			nextHead = (LPFREEHEAD)pTemp; // SKJ

		*newFree = *currFree;
		newFree->size = (short)(currFree->size + size);

		((LPFREETAIL)((LPSTR)newFree - newFree->size)-1)->size = (short)(
		 -newFree->size);

		previousHead->next = nextHead->previous  = STR_OFFSET(blkHdr, newFree);

		if (blkHdr->firstFree == STR_OFFSET(blkHdr, currFree)) // SKJ
			blkHdr->firstFree = STR_OFFSET(blkHdr, newFree);// SKJ
		if (blkHdr->lastFree == STR_OFFSET(blkHdr, currFree))// SKJ
			blkHdr->lastFree = STR_OFFSET(blkHdr, newFree); // SKJ
		blkHdr->freeSpace -= (short)size;
		((LPSTRHEAD)currFree)->size = (short)size;
		((LPSTRHEAD)currFree)->referenceCount = 1;
		*((LPSTR)currFree + (size - 1)) = BLOCKISUSED;
		}
	else           //all of block will be used
		{
		if (currFree->previous == STR_OFFSET(blkHdr, currFree) ||
				currFree->next == STR_OFFSET(blkHdr, currFree))    // SKJ
			{
			blkHdr->firstFree = blkHdr->lastFree = 0;
			}
		else
			{
			previousHead = (LPFREEHEAD)STR_PTR(blkHdr, currFree->previous);
			nextHead = (LPFREEHEAD)STR_PTR(blkHdr, currFree->next);
			previousHead->next = STR_OFFSET(blkHdr, nextHead);
			nextHead->previous = STR_OFFSET(blkHdr, previousHead);
			if (blkHdr->firstFree == STR_OFFSET(blkHdr, currFree))
				blkHdr->firstFree = STR_OFFSET(blkHdr, nextHead);
			if (blkHdr->lastFree == STR_OFFSET(blkHdr, currFree))
				blkHdr->lastFree = STR_OFFSET(blkHdr, previousHead);
			}
		blkHdr->freeBlockCnt -= (short)1;
		blkHdr->freeSpace -= (short)(-currFree->size);
		((LPSTRHEAD)currFree)->size = (short)(-currFree->size);
		((LPSTRHEAD)currFree)->referenceCount = 1;
		*((LPSTR)currFree + ((LPSTRHEAD)currFree)->size-1) = BLOCKISUSED;
		}

#if (DEBUGFLAG == 1)

	CheckFreeBlocks(blkHdr);

#endif

  pNewString = (LPSTR)currFree + sizeof(STRHEAD);

  offset = STR_OFFSET(blkHdr, pNewString);

	return pNewString;

	}
#endif

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	StringFree
//
// CLASS.......:  STRING (private)
//
// DESCRIPTION.:  Frees the space allocated for a string.  If the string is
//                adjacent to a free block the free space is added to the
//								free block.
//
// PARAMETERS..:  A pointer to the character string whose storage space is
//                to be freed.
//
// RETURN VALUE:	Returns TRUE if the string space was freed. Returns FALSE
//								if the reference count was simply decremented.
//
/////////////////////////////////////////////////////////////////////////////
#ifndef VABSHELL
BOOL STRINGSPACE::StringFree(
	WORD  blockIndex,
	WORD	offset)
	{
  LPSTR           pString;
	LPFREEHEAD			preAdjacentHead;
	LPFREEHEAD			postAdjacentHead;
	LPFREETAIL 			preAdjacentTail;
	LPFREEHEAD			newFreeHead;
	LPFREEHEAD 			previousHead;
	LPFREEHEAD 			nextHead;
	LPSTRHEAD 			usedHead;
	LPBLOCKHEADER 	blkHdr;
	short 					size;

	blkHdr = pBlockArray[blockIndex].pBlock;

  pString = (LPSTR) STR_PTR(blkHdr, offset);

	usedHead = (LPSTRHEAD)pString - 1;
	usedHead->referenceCount -= (short)1;

	if (usedHead->referenceCount > 0)
		return FALSE;

	size = usedHead->size;

	blkHdr->freeSpace += size;

	//check for preadjacent free block

	if (usedHead == STR_PTR(blkHdr, sizeof(BLOCKHEADER))) // SKJ used to FP_OFF
		preAdjacentHead = NULL;  						//string is at the start of the block
	else
		{
		preAdjacentTail = (LPFREETAIL)usedHead - 1;
		if (preAdjacentTail->usedFlag == BLOCKISFREE)
			preAdjacentHead =
			 (LPFREEHEAD)(((LPSTR)usedHead) - preAdjacentTail->size);
		else
			preAdjacentHead = NULL;
		}

	// check for postadjacent free block

	if (blockSize- (STR_OFFSET(blkHdr, usedHead)+ size) < MINIMUMBYTES) // SKJ
		postAdjacentHead = NULL;  							//string is at end of block
	else
		{
		postAdjacentHead = (LPFREEHEAD)(pString+(size-sizeof(STRHEAD)));
		if (postAdjacentHead->size >= 0)
			postAdjacentHead = NULL;              //string is followed by a string
		}

	if (preAdjacentHead && postAdjacentHead)
		{
		preAdjacentHead->size -= (short)(size - postAdjacentHead->size);
		((LPFREETAIL)((LPSTR)postAdjacentHead - postAdjacentHead->size)-1)->size =
		 (short)(-preAdjacentHead->size);
		previousHead =
		 (LPFREEHEAD)STR_PTR(blkHdr, postAdjacentHead->previous);
		nextHead =
		 (LPFREEHEAD)STR_PTR(blkHdr, postAdjacentHead->next);
		previousHead->next = STR_OFFSET(blkHdr, nextHead);
		nextHead->previous = STR_OFFSET(blkHdr, previousHead);
		if (blkHdr->firstFree == STR_OFFSET(blkHdr, postAdjacentHead))
			blkHdr->firstFree = STR_OFFSET(blkHdr, nextHead);
		if (blkHdr->lastFree == STR_OFFSET(blkHdr, postAdjacentHead))
			blkHdr->lastFree = STR_OFFSET(blkHdr, previousHead);
		blkHdr->freeBlockCnt -= (short)1;
		}
	else if (preAdjacentHead)
		{
		preAdjacentHead->size -= size;
		preAdjacentTail =
		 (LPFREETAIL)((LPSTR)preAdjacentHead - preAdjacentHead->size) - 1;
		preAdjacentTail->size = (short)(-preAdjacentHead->size);
		preAdjacentTail->usedFlag = BLOCKISFREE;
		}
	else if (postAdjacentHead)
		{
		if (postAdjacentHead->next == STR_OFFSET(blkHdr, postAdjacentHead) )
			{
			((LPFREEHEAD)usedHead)->next = blkHdr->lastFree = blkHdr->firstFree =
			 ((LPFREEHEAD)usedHead)->previous = STR_OFFSET(blkHdr, usedHead);
			}
		else
			{
			((LPFREEHEAD)usedHead)->next = postAdjacentHead->next;
			((LPFREEHEAD)usedHead)->previous = postAdjacentHead->previous;
			previousHead =
			 (LPFREEHEAD)STR_PTR(blkHdr, postAdjacentHead->previous);
			nextHead =
			 (LPFREEHEAD)STR_PTR(blkHdr, postAdjacentHead->next);
			previousHead->next = STR_OFFSET(blkHdr, usedHead);
			nextHead->previous = STR_OFFSET(blkHdr, usedHead);
			if (blkHdr->firstFree == STR_OFFSET(blkHdr, postAdjacentHead))
				blkHdr->firstFree = STR_OFFSET(blkHdr, usedHead);
			if (blkHdr->lastFree == STR_OFFSET(blkHdr, postAdjacentHead))
				blkHdr->lastFree  = STR_OFFSET(blkHdr, usedHead);
			}
		((LPFREEHEAD)usedHead)->size = (short)(postAdjacentHead->size - size);
		((LPFREETAIL)((LPSTR)postAdjacentHead - postAdjacentHead->size)-1)->size=
		 (short)(-((LPFREEHEAD)usedHead)->size);
		}
	else
		{
		newFreeHead = (LPFREEHEAD)((LPSTRHEAD)pString - 1);
		newFreeHead->size = (short)-size;
		((LPFREETAIL)((LPSTR)newFreeHead + size)-1)->size = size;
		((LPFREETAIL)((LPSTR)newFreeHead + size)-1)->usedFlag = BLOCKISFREE;
		blkHdr->freeBlockCnt += (short)1;
		if (blkHdr->firstFree == 0)
			{
			blkHdr->firstFree = blkHdr->lastFree = newFreeHead->previous =
			 newFreeHead->next = STR_OFFSET(blkHdr, newFreeHead);
			}
		else
			{
			previousHead =
			 (LPFREEHEAD)STR_PTR(blkHdr, blkHdr->lastFree);
			nextHead =
			 (LPFREEHEAD)STR_PTR(blkHdr, blkHdr->firstFree);
			previousHead->next = nextHead->previous = STR_OFFSET(blkHdr, newFreeHead);
			newFreeHead->previous = blkHdr->lastFree;
			blkHdr->lastFree = STR_OFFSET(blkHdr, newFreeHead);
			newFreeHead->next = blkHdr->firstFree;
			}
		}

#if (DEBUGFLAG == 1)

	CheckFreeBlocks(blkHdr);

#endif

	return TRUE;

	}
#endif

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	InitBlock
//
// CLASS.......:	STRINGSPACE (private)
//
// DESCRIPTION.:	allocates and initializes a block of string memory.  The
//								size of the block is set by the value of the static member
//                block size.
//
// PARAMETERS..:  theBlock - the block structure to be initialized.
//
/////////////////////////////////////////////////////////////////////////////
void STRINGSPACE::InitBlock(
	STRINGBLOCK	& theBlock)
	{
	LPFREEHEAD	freeHead;
	LPFREETAIL	freeTail;

	theBlock.hBlock = GlobalAlloc(GMEM_MOVEABLE, blockSize);
	theBlock.pBlock = (LPBLOCKHEADER)GlobalLock(theBlock.hBlock);

	freeHead = (LPFREEHEAD)(theBlock.pBlock + 1);
	freeHead->previous = freeHead->next = STR_OFFSET(theBlock.pBlock, freeHead);
	freeHead->size = (short)(-(blockSize-sizeof(BLOCKHEADER)));

	freeTail = ((LPFREETAIL)((LPSTR)theBlock.pBlock+blockSize)) - 1;
	freeTail->usedFlag = BLOCKISFREE;
	freeTail->size = (short)(-freeHead->size);

	theBlock.pBlock->freeBlockCnt = 1;
	theBlock.pBlock->firstFree = theBlock.pBlock->lastFree =
	 STR_OFFSET(theBlock.pBlock, freeHead);
	theBlock.pBlock->freeSpace = freeTail->size;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	InitStringMemory
//
// CLASS.......:  STRINGSPACE (public)
//
// DESCRIPTION.:	Initializes String Memory by allocating a single memory
//								block and initializing it.
//
// PARAMETERS..:  blockSize - sets the value of the static member blockSize
//									This the size of each new block of string memory.
//
/////////////////////////////////////////////////////////////////////////////
void STRINGSPACE::InitStringMemory(
	unsigned short blockSize)
	{

	this->blockSize = blockSize;

	hBlockArray = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,
														INITIALBLOCKARRAYSIZE*sizeof(STRINGBLOCK));

	pBlockArray = (LPSTRINGBLOCK)GlobalLock(hBlockArray);

	InitBlock(pBlockArray[0]);

	blockCount = 1;

	maxBlocks = INITIALBLOCKARRAYSIZE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	FreeStringMemory
//
// CLASS.......:	STRINGSPACE (public)
//
// DESCRIPTION.:  Frees all string memory.
//
/////////////////////////////////////////////////////////////////////////////
void STRINGSPACE::FreeStringMemory(
	void)
	{
	int i;

	if (hBlockArray != NULL)
		{
		for (i=0; i<blockCount; i++)
			{
			GlobalUnlock(pBlockArray[i].hBlock);
			GlobalFree(pBlockArray[i].hBlock);
			}
		blockCount = 0;
		GlobalUnlock(hBlockArray);
		GlobalFree(hBlockArray);
		pBlockArray = NULL;
		hBlockArray = NULL;
		blockCount = 0;
		blockSize = 0;
		maxBlocks = 0;
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CheckFreeBlocks
//
// CLASS.......:  STRINGSPACE (static)
//
// DESCRIPTION.:  runs a consitency check on the free blocks in a single
//								memory block.
//
/////////////////////////////////////////////////////////////////////////////
#ifndef VABSHELL
void STRINGSPACE::CheckFreeBlocks(
	LPBLOCKHEADER	blkHdr)
	{
	LPFREEHEAD			currFree;

	if (blkHdr->freeSpace > blockSize-sizeof(BLOCKHEADER))
		MessageBox(NULL, "freeSpace is larger than blockSize",
							 "STRING::CheckFreeBlocks", MB_OK|MB_TASKMODAL);

	if (blkHdr->firstFree>blockSize || blkHdr->firstFree<sizeof(BLOCKHEADER))
		MessageBox(NULL, "firstFree is out of range",
							 "STRING::CheckFreeBlocks", MB_OK|MB_TASKMODAL);

	if (blkHdr->lastFree>blockSize || blkHdr->lastFree<sizeof(BLOCKHEADER))
		MessageBox(NULL, "lastFree is out of range",
							 "STRING::CheckFreeBlocks", MB_OK|MB_TASKMODAL);

	if ((blkHdr->lastFree == blkHdr->firstFree) &&
			(blkHdr->firstFree != 0) && (blkHdr->freeBlockCnt > 1))
		MessageBox(NULL, "lastFree = firstFree, but freeBlockCnt > 1",
							 "STRING::CheckFreeBlocks", MB_OK|MB_TASKMODAL);

	if ((blkHdr->firstFree == 0) &&	(blkHdr->freeBlockCnt > 0))
		MessageBox(NULL, "firstFree=0, but freeBlockCnt > 0",
							 "STRING::CheckFreeBlocks", MB_OK|MB_TASKMODAL);


	currFree = (LPFREEHEAD)STR_PTR(blkHdr, blkHdr->firstFree);
	do
		{
		if (currFree->size > 0)
			MessageBox(NULL, "free block's size is > 0.",
								 "STRING::CheckFreeBlocks", MB_OK|MB_TASKMODAL);
		else if (-currFree->size > blkHdr->freeSpace)
			MessageBox(NULL, "free block size is > total free space.",
								 "STRING::CheckFreeBlocks", MB_OK|MB_TASKMODAL);
		else
			{
			if (-currFree->size !=
					((LPFREETAIL)((LPSTR)currFree-currFree->size) - 1)->size)
				MessageBox(NULL, "free head size != free tail size.",
									 "STRING::CheckFreeBlocks", MB_OK|MB_TASKMODAL);
			if (((LPFREETAIL)((LPSTR)currFree-currFree->size)-1)->usedFlag !=
					BLOCKISFREE)
				MessageBox(NULL, "FREE BLOCK NOT MARKED AS FREE",
									 "STRING::CheckFreeBlocks", MB_OK|MB_TASKMODAL);
			}
		currFree = (LPFREEHEAD)STR_PTR(blkHdr, currFree->next);
		}
	while ((long)((LPSTR)currFree  - (LPSTR)blkHdr) != blkHdr->firstFree);

	}
#endif

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  Write
//
// CLASS.......:	STRINGSPACE
//
// DESCRIPTION.:	Writes a string to the indicated file.  The string is
//                written starting at the current file position.  A two byte
//                count of the characters in the string (including the null
//								character) is written first.  This is followed by all of
//                the characters including the null character.
//
// RETURN VALUE:  Returns TRUE if the string is successfully written.
//								Otherwise, FALSE is returned.
//
/////////////////////////////////////////////////////////////////////////////
#ifndef VABSHELL
BOOL STRINGSPACE::Write(
	HANDLE			 hFile)
	{
	short	i;
	BOOL						result = TRUE;
	STRINGSFILEHDR	stringsHdr;
   DWORD				readSize;
	stringsHdr.blockCount = blockCount;
	stringsHdr.blockSize = blockSize;

	WriteFile(hFile, (LPSTR)&stringsHdr, sizeof(STRINGSFILEHDR), &readSize, NULL);

	for (i=0; i<blockCount; i++)
		{
      WriteFile(hFile, (LPSTR)pBlockArray[i].pBlock, blockSize, &readSize, NULL);
		if (blockSize != readSize)
			MessageBox(NULL, "String Write Failure", "Could not write Block",
			 MB_OK|MB_TASKMODAL);
		}

	return result;

	}
#endif

/////////////////////////////////////////////////////////////////////////////
//
// FUNCTION....:  GetPointer
//
// CLASS.......:	STRINGSPACE
//
// DESCRIPTION.:
//
// RETURN VALUE:
//
/////////////////////////////////////////////////////////////////////////////
LPSTR STRINGSPACE::GetPointer(
	WORD blockIndex,
	WORD offset)
	{

	return (LPSTR)STR_PTR(pBlockArray[blockIndex].pBlock, offset);

	}
/////////////////////////////////////////////////////////////////////////////
//
// FUNCTION....:  IncrementRefCount
//
// CLASS.......:	STRINGSPACE
//
// DESCRIPTION.:
//
// RETURN VALUE:
//
/////////////////////////////////////////////////////////////////////////////
short STRINGSPACE::IncrementRefCount(
	WORD blockIndex,
	WORD offset)
	{
	LPSTRHEAD		pHead	= (LPSTRHEAD) GetPointer(blockIndex, offset) - 1;

	pHead->referenceCount += (short)1;

	return pHead->referenceCount;
	}

