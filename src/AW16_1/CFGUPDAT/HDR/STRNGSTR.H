////////////////////////////////////////////////////////////////////////////
// This file contains the declarations for the structures used to manage
// string memory.
////////////////////////////////////////////////////////////////////////////
#ifndef __STRNGSTR_H
#define __STRNGSTR_H

struct FREEHEAD								//header for a block of free storage.
	{
	short						size;       //the negative of the total number of bytes in
															// the free block.  This number is saved as a
															// negative value to flag the block as free.
	unsigned short	next;       //the offset of the next free block.
	unsigned short	previous;   //the offset of the previoius free block.
	};

typedef FREEHEAD far*				LPFREEHEAD;



struct FREETAIL               //the structure of the last bytes in the free
	{														// block
	short	size;                 //the number of bytes in the free block.
	char	usedFlag;             //a flag to indicate if the block is free
															// equal to BLOCKISFREE or BLOCKISUSED.
	};

typedef FREETAIL far*				LPFREETAIL;



struct STRHEAD                //header for an allocated string
	{
	short	size;                 //the total number of bytes allocated for the
															// character string.  Always >= MINIMUMBYTES.
															// Includes the header and terminating null
															// character.
	short	referenceCount;       //A count of the number of STRING objects that
															// point to the character string.
	};

typedef STRHEAD far*				LPSTRHEAD;



struct BLOCKHEADER                //The header for a block of string memory
	{
	unsigned short	freeBlockCnt;   //The number of free blocks.
	unsigned short	freeSpace;      //The total number of free bytes.
	unsigned short	firstFree;      //The offset of the first free block.
																	//Equal to zero if there are no free blocks
	unsigned short	lastFree;       //The offset of the last free block.
	};

typedef BLOCKHEADER far*		LPBLOCKHEADER;



struct STRINGBLOCK					 //structure of an entry in the block array.
	{
	LPBLOCKHEADER		pBlock;    //pointer to the string memory.
	GLOBALHANDLE		hBlock;    //handle of the string memory.
	};

typedef STRINGBLOCK far*		LPSTRINGBLOCK;

#endif