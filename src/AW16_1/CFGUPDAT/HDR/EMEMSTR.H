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
	void far *			theChunk;
	HANDLE					chunkHandle;
	long int				chunkExtent;
	long int 				freeSpace;
	FREEBLOCK far * freeBlocks;
	HANDLE					freeBlocksHandle;
	short int				freeBlockCount;
	long int 				chunkID;
	BOOL						dirtyBlock;
	BOOL						unusedFlag;
	};

struct SERIUSMEMORY
	{
	HANDLE					chunksHandle;
	MEMCHUNK far *	chunks;
	HANDLE					unused;
	short int				chunkCount;
	long int				chunkIDSeed;
	};

typedef SERIUSMEMORY far*		LPSERIUSMEMORY;

#endif