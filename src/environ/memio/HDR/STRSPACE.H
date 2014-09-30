#ifndef __STRSPACE_H
#define __STRSPACE_H

#include "classdef.h"
#include "strngstr.h"

class STRINGSPACE
	{
	friend union DBSTRING;

private:
	STRINGBLOCK *	pBlockArray;
	GLOBALHANDLE			hBlockArray;
	unsigned short		blockSize;        //the number of bytes in a block.
	short							blockCount;       //the number of allocated blocks,
																			// always <= maxBlocks.
	short							maxBlocks;				//the current size of the block
																			// array.
	void InitBlock(
		STRINGBLOCK	& theBlock);

	void InitStringMemory(
		unsigned short	blockSize);

public:

	STRINGSPACE(
		void);

	STRINGSPACE(
		WORD	blockSize);

	STRINGSPACE(
		STRINGSPACE *	pSrc)
    {
    pBlockArray = pSrc->pBlockArray;
		hBlockArray = pSrc->hBlockArray;
		blockSize = pSrc->blockSize;
	  blockCount = pSrc->blockCount;
		maxBlocks = pSrc->maxBlocks;
    }

	~STRINGSPACE();

	void Read(
		HANDLE	hFIle);

	LPSTR StringAlloc(
		int 	   size,
		short	 & blockIndex,
		WORD   & offset);

	void CheckFreeBlocks(
		LPBLOCKHEADER	blkHdr);

	void FreeStringMemory(
		void);

	BOOL StringFree(
		WORD  blockIndex,
		WORD	offSet);

	BOOL Write(
		HANDLE hFile);

	LPSTR GetPointer(
		WORD blockIndex,
		WORD offset);

  short IncrementRefCount(
    WORD blockIndex,
    WORD offset);

	}; //end class definition


#endif
