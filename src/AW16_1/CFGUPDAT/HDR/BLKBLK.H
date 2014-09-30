#ifndef __BLKBLK_H__
#define __BLKBLK_H__

#include "serblk.h"
#include "uidtable.h"

class SERIUSBLOCKBLOCK:public SERIUSBLOCK
	{
protected:

	static void UpdateUIDTable(
		LPSERIUSBLOCK	pBlock,
		LPUIDTABLE		uidTable);

public:

	static SERIUSBLOCKBLOCK far* NextRec(
		LPSERIUSBLOCK					theBlock,
		SERIUSBLOCKBLOCK far*	currentRec);

	static void InitUIDTable(
		LPSERIUSBLOCK	pBlock,
		LPUIDTABLE		uidTable);

	void far * operator new(
		size_t				size);

	void far * operator new(
		size_t				size,
		LPSERIUSBLOCK	blockBlocks,
		LPUIDTABLE		uids);

	void far * operator new(
		size_t			size,
		void far*		objPtr);

	void operator delete(
		void far *	objPtr);

	SERIUSBLOCKBLOCK(
		void):SERIUSBLOCK()
		{
		}

	SERIUSBLOCKBLOCK(
		long 	count):SERIUSBLOCK(count)
		{
		}

	};

typedef SERIUSBLOCKBLOCK far * 	LPSERIUSBLOCKBLOCK;

#endif