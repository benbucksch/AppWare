#ifndef __SERBLK_H__
#define __SERBLK_H__

#include <mem.h>
#include "ememstr.h"
#include "serrec.h"

class RECORDBLOCK;

class SERIUSBLOCK
	{
	friend class SERIUSBLOCKBLOCK;
	friend class EXESERIUSBLOCKBLOCK;
	friend class DATASBLOCKBLOCK;

  static MEMCHUNK far* AllocateChunk(
    long            numBytesUsed);

  static void InitChunkFreeSpace(
    MEMCHUNK far *  pChunk,
    long            numUsedBytes);

  static BOOL ReadChunk(
	  int            hFile,
    MEMCHUNK far*  pChunk,
    long           byteCount);


protected:

	long int				blockFileID;
	long int				blockMemID;
	long int				byteCount;
	void far *			theData;

	static void ReleaseMemory(
		long int				blockMemID,
		long int				blockLength,
		void far *			theData);

	void static CompactFree(
		MEMCHUNK far *	 theChunk,
		FREEBLOCK				 newFree);

	void far* AllocateBlock(
		long int 	blockSize);

  void far * ResizeChunk(
    long  blockMemID,
    long  newCount);

public:

	static void InitMemory(void);

	void far * operator new(
		size_t	size);

	void far * operator new(
		size_t			size,
		void far*		objPtr);

	void far * operator new(
		size_t			size,
		SERUID      recBlkID);

	void operator delete(
		void far *	objPtr,
		size_t			size);

	SERIUSBLOCK(void);

	SERIUSBLOCK(
		long int	count);

	~SERIUSBLOCK(void);

	void far* ResizeBlock(
		long int	deltaCount);

	void DeleteSubBlock(
		long int				start,
		long int				count);

	long int GetByteCount(void);

	void far*	GetData(
		void);

  long GetBlockMemID(void)
    {
    return blockMemID;
    }

	BOOL Write(
		int		hFile,
		WORD	&writeCount);

	BOOL Read(
		int  			hFile);

  static BOOL Read(
	  int                 hFile,
    RECORDBLOCK far *  theBlocks);

	void CheckBlock(
		void);

	static void CheckFreeBlocks(
		MEMCHUNK far *	 theChunk);

	};

typedef SERIUSBLOCK far*	LPSERIUSBLOCK;


#endif
