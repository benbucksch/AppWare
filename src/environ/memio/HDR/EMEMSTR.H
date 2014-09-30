/////////////////////////////////////////////////////////////////////////////
//  This file contains the definitions of the structures used by the
//  runtime memory management routines.
/////////////////////////////////////////////////////////////////////////////
#ifndef __EMEMSTR_H__
#define __EMEMSTR_H__

struct FREEBLOCK
	{
	long int	start;
	long int	extent;
	};

struct MEMCHUNK
	{
	void  *			theChunk;
	HANDLE					chunkHandle;
	long int				chunkExtent;
	long int 				freeSpace;
	FREEBLOCK  * freeBlocks;
	HANDLE					freeBlocksHandle;
	short int				freeBlockCount;
	long int 				chunkID;
	BOOL						dirtyBlock;
	BOOL						unusedFlag;
	};

struct SERIUSMEMORY
	{
	HANDLE					chunksHandle;
	MEMCHUNK  *	chunks;
	HANDLE					unused;
	short int				chunkCount;
	long int				chunkIDSeed;
	};

#endif
