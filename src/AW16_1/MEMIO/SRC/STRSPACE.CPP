#include <dos.h>
#include "almsys.h"

#include "strngstr.h"
#include "strspace.h"

#define INITIALBLOCKARRAYSIZE 10
#define BLOCKISUSED	'\0'
#define BLOCKISFREE 1
#define MINIMUMBYTES ((short)(sizeof(FREEHEAD) + sizeof(FREETAIL)))


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
	int	hFile)
	{
	STRINGSFILEHDR	stringsHdr;
	int 						i;

	if (sizeof(STRINGSFILEHDR) ==
			_lread(hFile, (LPSTR)&stringsHdr, sizeof(STRINGSFILEHDR)))
		{
		blockSize = stringsHdr.blockSize;
		blockCount = stringsHdr.blockCount;
		maxBlocks = blockCount + blockCount%INITIALBLOCKARRAYSIZE;
		hBlockArray = GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,
		 maxBlocks*sizeof(STRINGBLOCK));
		pBlockArray = (LPSTRINGBLOCK)GlobalLock(hBlockArray);
		for (i=0; i<blockCount; i++)
			{
			pBlockArray[i].hBlock = GlobalAlloc(GMEM_MOVEABLE, blockSize);
			pBlockArray[i].pBlock =
			 (LPBLOCKHEADER)GlobalLock(pBlockArray[i].hBlock);
			if (blockSize != _lread(hFile, (LPSTR)pBlockArray[i].pBlock, blockSize))
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
	int         size,
	short	far & blockIndex)
	{
	LPFREEHEAD 			currFree;
	LPFREEHEAD  		previousHead;
	LPFREEHEAD  		nextHead;
	LPFREEHEAD			newFree;
	LPBLOCKHEADER  	blkHdr;
	int							i;
	unsigned short	segment;
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
			segment = FP_SEG(blkHdr);
			currFree = (LPFREEHEAD)MK_FP(segment, blkHdr->firstFree);
			do
				{
				found = -currFree->size - size;
				if (found >= MINIMUMBYTES)
					break;
				currFree = (LPFREEHEAD)MK_FP(segment, currFree->next);
				}
			while (FP_OFF(currFree) != blkHdr->firstFree);
			}
		if (found >= MINIMUMBYTES)
			break;
		}

	if (found < MINIMUMBYTES) 							//allocate a new block
		{
		if (blockCount == maxBlocks)
			{
			maxBlocks += 10;
			GlobalUnlock(hBlockArray);
			hBlockArray = GlobalReAlloc(hBlockArray,
																	maxBlocks*sizeof(STRINGBLOCK), 0);
			pBlockArray = (LPSTRINGBLOCK)GlobalLock(hBlockArray);
			}
		InitBlock(pBlockArray[blockCount]);
		blkHdr = pBlockArray[blockCount].pBlock;
		segment = FP_SEG(blkHdr);
		currFree = (LPFREEHEAD)MK_FP(segment, pBlockArray[i].pBlock->firstFree);
		blockIndex = blockCount;
		blockCount++;
		found = -currFree->size - size;
		}
	else
		blockIndex = i;

	if (found > MINIMUMBYTES) //only part of block will be used
		{
		newFree = (LPFREEHEAD)((LPSTR)currFree + size);

		if (currFree->previous == FP_OFF(currFree))
			previousHead = newFree;
		else
			previousHead = (LPFREEHEAD)MK_FP(segment, currFree->previous);

		if (currFree->next == FP_OFF(currFree))
			nextHead = newFree;
		else
			nextHead = (LPFREEHEAD)MK_FP(segment, currFree->next);

		*newFree = *currFree;
		newFree->size = currFree->size + size;

		((LPFREETAIL)((LPSTR)newFree - newFree->size)-1)->size =
		 -newFree->size;

		previousHead->next = nextHead->previous = FP_OFF(newFree);
		if (blkHdr->firstFree == FP_OFF(currFree))
			blkHdr->firstFree = FP_OFF(newFree);
		if (blkHdr->lastFree == FP_OFF(currFree))
			blkHdr->lastFree = FP_OFF(newFree);
		blkHdr->freeSpace -= size;
		((LPSTRHEAD)currFree)->size = size;
		((LPSTRHEAD)currFree)->referenceCount = 1;
		*((LPSTR)currFree + (size - 1)) = BLOCKISUSED;
		}
	else           //all of block will be used
		{
		if (currFree->previous == FP_OFF(currFree) ||
				currFree->next == FP_OFF(currFree) )
			{
			blkHdr->firstFree = blkHdr->lastFree = 0;
			}
		else
			{
			previousHead = (LPFREEHEAD)MK_FP(segment, currFree->previous);
			nextHead = (LPFREEHEAD)MK_FP(segment, currFree->next);
			previousHead->next = FP_OFF(nextHead);
			nextHead->previous = FP_OFF(previousHead);
			if (blkHdr->firstFree == FP_OFF(currFree))
				blkHdr->firstFree = FP_OFF(nextHead);
			if (blkHdr->lastFree == FP_OFF(currFree))
				blkHdr->lastFree = FP_OFF(previousHead);
			}
		blkHdr->freeBlockCnt -= 1;
		blkHdr->freeSpace -= (-currFree->size);
		((LPSTRHEAD)currFree)->size = -currFree->size;
		((LPSTRHEAD)currFree)->referenceCount = 1;
		*((LPSTR)currFree + ((LPSTRHEAD)currFree)->size-1) = BLOCKISUSED;
		}

#if (DEBUGFLAG == 1)

	CheckFreeBlocks(blkHdr);

#endif

	return (LPSTR)currFree + sizeof(STRHEAD);

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

	return StringFree((LPSTR)MK_FP(FP_SEG(pBlockArray[blockIndex].pBlock), offset));

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
	LPSTR			pString)
	{
	LPFREEHEAD			preAdjacentHead;
	LPFREEHEAD			postAdjacentHead;
	LPFREETAIL 			preAdjacentTail;
	LPFREEHEAD			newFreeHead;
	LPFREEHEAD 			previousHead;
	LPFREEHEAD 			nextHead;
	LPSTRHEAD 			usedHead;
	LPBLOCKHEADER 	blkHdr;
	unsigned short	segment;
	short 					size;

	usedHead = (LPSTRHEAD)pString - 1;
	usedHead->referenceCount -= 1;

	if (usedHead->referenceCount > 0)
		return FALSE;

	size = usedHead->size;

	segment = FP_SEG(pString);

	blkHdr = (LPBLOCKHEADER)MK_FP(segment, 0);

	blkHdr->freeSpace += size;

	//check for preadjacent free block

	if (FP_OFF(usedHead) == sizeof(BLOCKHEADER))
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

	if (blockSize-(FP_OFF(usedHead)+size) < MINIMUMBYTES)
		postAdjacentHead = NULL;  							//string is at end of block
	else
		{
		postAdjacentHead = (LPFREEHEAD)(pString+(size-sizeof(STRHEAD)));
		if (postAdjacentHead->size >= 0)
			postAdjacentHead = NULL;              //string is followed by a string
		}

	if (preAdjacentHead && postAdjacentHead)
		{
		preAdjacentHead->size -= (size - postAdjacentHead->size);
		((LPFREETAIL)((LPSTR)postAdjacentHead - postAdjacentHead->size)-1)->size =
		 -preAdjacentHead->size;
		previousHead =
		 (LPFREEHEAD)MK_FP(segment, postAdjacentHead->previous);
		nextHead =
		 (LPFREEHEAD)MK_FP(segment, postAdjacentHead->next);
		previousHead->next = FP_OFF(nextHead);
		nextHead->previous = FP_OFF(previousHead);
		if (blkHdr->firstFree == FP_OFF(postAdjacentHead))
			blkHdr->firstFree = FP_OFF(nextHead);
		if (blkHdr->lastFree == FP_OFF(postAdjacentHead))
			blkHdr->lastFree = FP_OFF(previousHead);
		blkHdr->freeBlockCnt -= 1;
		}
	else if (preAdjacentHead)
		{
		preAdjacentHead->size -= size;
		preAdjacentTail =
		 (LPFREETAIL)((LPSTR)preAdjacentHead - preAdjacentHead->size) - 1;
		preAdjacentTail->size = -preAdjacentHead->size;
		preAdjacentTail->usedFlag = BLOCKISFREE;
		}
	else if (postAdjacentHead)
		{
		if (postAdjacentHead->next == FP_OFF(postAdjacentHead))
			{
			((LPFREEHEAD)usedHead)->next = blkHdr->lastFree = blkHdr->firstFree =
			 ((LPFREEHEAD)usedHead)->previous = FP_OFF(usedHead);
			}
		else
			{
			((LPFREEHEAD)usedHead)->next = postAdjacentHead->next;
			((LPFREEHEAD)usedHead)->previous = postAdjacentHead->previous;
			previousHead =
			 (LPFREEHEAD)MK_FP(segment, postAdjacentHead->previous);
			nextHead =
			 (LPFREEHEAD)MK_FP(segment, postAdjacentHead->next);
			previousHead->next = FP_OFF(usedHead);
			nextHead->previous = FP_OFF(usedHead);
			if (blkHdr->firstFree == FP_OFF(postAdjacentHead))
				blkHdr->firstFree = FP_OFF(usedHead);
			if (blkHdr->lastFree == FP_OFF(postAdjacentHead))
				blkHdr->lastFree = FP_OFF(usedHead);
			}
		((LPFREEHEAD)usedHead)->size = postAdjacentHead->size - size;
		((LPFREETAIL)((LPSTR)postAdjacentHead - postAdjacentHead->size)-1)->size=
		 -((LPFREEHEAD)usedHead)->size;
		}
	else
		{
		newFreeHead = (LPFREEHEAD)((LPSTRHEAD)pString - 1);
		newFreeHead->size = -size;
		((LPFREETAIL)((LPSTR)newFreeHead + size)-1)->size = size;
		((LPFREETAIL)((LPSTR)newFreeHead + size)-1)->usedFlag = BLOCKISFREE;
		blkHdr->freeBlockCnt += 1;
		if (blkHdr->firstFree == 0)
			{
			blkHdr->firstFree = blkHdr->lastFree = newFreeHead->previous =
			 newFreeHead->next = FP_OFF(newFreeHead);
			}
		else
			{
			previousHead =
			 (LPFREEHEAD)MK_FP(segment, blkHdr->lastFree);
			nextHead =
			 (LPFREEHEAD)MK_FP(segment, blkHdr->firstFree);
			previousHead->next = nextHead->previous = FP_OFF(newFreeHead);
			newFreeHead->previous = blkHdr->lastFree;
			blkHdr->lastFree = FP_OFF(newFreeHead);
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
	STRINGBLOCK	far& theBlock)
	{
	LPFREEHEAD	freeHead;
	LPFREETAIL	freeTail;

	theBlock.hBlock = GlobalAlloc(GMEM_MOVEABLE, blockSize);
	theBlock.pBlock = (LPBLOCKHEADER)GlobalLock(theBlock.hBlock);

	freeHead = (LPFREEHEAD)(theBlock.pBlock + 1);
	freeHead->previous = freeHead->next = FP_OFF(freeHead);
	freeHead->size = -(blockSize-sizeof(BLOCKHEADER));

	freeTail = ((LPFREETAIL)((LPSTR)theBlock.pBlock+blockSize)) - 1;
	freeTail->usedFlag = BLOCKISFREE;
	freeTail->size = -freeHead->size;

	theBlock.pBlock->freeBlockCnt = 1;
	theBlock.pBlock->firstFree = theBlock.pBlock->lastFree =
	 FP_OFF(freeHead);
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
	unsigned short	segment;
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

	segment = FP_SEG(blkHdr);

	currFree = (LPFREEHEAD)MK_FP(segment, blkHdr->firstFree);
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
		currFree = (LPFREEHEAD)MK_FP(segment, currFree->next);
		}
	while (FP_OFF(currFree) != blkHdr->firstFree);

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
	int			 hFile)
	{
	short	i;
	BOOL						result = TRUE;
	STRINGSFILEHDR	stringsHdr;

	stringsHdr.blockCount = blockCount;
	stringsHdr.blockSize = blockSize;

	_lwrite(hFile, (LPSTR)&stringsHdr, sizeof(STRINGSFILEHDR));

	for (i=0; i<blockCount; i++)
		{
		if (blockSize != _lwrite(hFile, (LPSTR)pBlockArray[i].pBlock, blockSize))
			MessageBox(NULL, "String Write Failure", "Could not write Block",
			 MB_OK|MB_TASKMODAL);
		}

	return result;

	}
#endif