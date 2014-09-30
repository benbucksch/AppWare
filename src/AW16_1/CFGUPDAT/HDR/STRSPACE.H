#ifndef __STRSPACE_H
#define __STRSPACE_H

#include <dos.h>
#include "strngstr.h"

class STRINGSPACE
	{
	friend union DBSTRING;

private:
	STRINGBLOCK far*	pBlockArray;
	GLOBALHANDLE			hBlockArray;
	unsigned short		blockSize;        //the number of bytes in a block.
	short							blockCount;       //the number of allocated blocks,
																			// always <= maxBlocks.
	short							maxBlocks;				//the current size of the block
																			// array.
	void InitBlock(
		STRINGBLOCK	far& theBlock);

	void InitStringMemory(
		unsigned short	blockSize);

public:

	STRINGSPACE(
		void);

	STRINGSPACE(
		WORD	blockSize);

	void Read(
		int	hFIle);

	LPSTR StringAlloc(
		int 	      size,
		short	far & blockIndex);

	BOOL StringFree(
		char far*		pString);

	void CheckFreeBlocks(
		LPBLOCKHEADER	blkHdr);

	void FreeStringMemory(
		void);

	BOOL StringFree(
		WORD  blockIndex,
		WORD	offSet);

	BOOL Write(
		int hFile);

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
LPSTR GetPointer(
	WORD blockIndex,
	WORD offset)
	{

	return (LPSTR)MK_FP(FP_SEG(pBlockArray[blockIndex].pBlock), offset);

	}


	}; //end class definition


typedef STRINGSPACE far*	LPSTRINGSPACE;

#endif
