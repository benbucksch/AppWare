#ifndef __RECBLK_H__
#define __RECBLK_H__

#include <mem.h>
#include "seruid.h"
#include "serblk.h"
#include "serrec.h"

#define RB_PURGE          0X0001
#define RB_PREALLOCATED   0X0002

class RECORDBLOCK
	{

	friend long GetUIDType(
		SERUID	 uid);

  friend BOOL CheckUID(
	  SERUID	 uid,
    long     recordType);

private:

	short				    type;
  unsigned short  flags;
	long int				count;
	long int				recordSize;
	short						blockCount;     	//if blockCount is 1, blockID's holds the
	SERUID					blockIDs;         //id of the block, if blockCount > 1

//if blockCount is 1, blockIDs holds the id of the block, if blockCount > 1
//blockIDs is  the id of a block containing an array of the block ids.

  void far*	AddPreallocatedRecord(
	  size_t	size);

	void CallConstructor(
		LPSERIUSBLOCK		theBlock);

  static void CallDestructor(
	  void far* objPtr,
    short     recType);

	static void UpdateUIDTable(
		SERIUSBLOCK far*	theBlock,
		size_t						size);

public:

	void InitUIDTable(
		void);

  void PurgeBlock(void);

  void FreeMemory(void);

  BOOL PreAllocate(
    long  recordCount);

	void far * operator new(
		size_t	size);

	void far * operator new(
		size_t			size,
		void far*		objPtr);

	void operator delete(
		void far *	objPtr,
		size_t			size);

	RECORDBLOCK(void);

	RECORDBLOCK(
		long int  recordSize,
		long int	type);

  RECORDBLOCK(
	  long int  	recordSize,
	  long int		type,
    long int    recordCount);

	~RECORDBLOCK(void);

	void far*	AddRecord(
		size_t	size = 0);

	BOOL DeleteRecord(
		void far*	objPtr,
		size_t		size);

	void far* operator [](long int index);

	long int GetCount(void);

	long int GetRecordSize(void);

  long int GetType(
    void)
    {
    return type;
    }

  long GetBlockID(void)
    {
    return blockIDs.id;
    }

  short GetBlockCount(void)
    {
    return blockCount;
    }

	BOOL Write(
		int		hFile,
		WORD	&writeCount);

	BOOL Read(
		int	hFile);

	void far*	NextRec(
		void far*		currentRec);

	void far*	pNextRec(
		void far*		currentRec);

	void Free(
		void);

	void ConvertToPointers(
		void);

  void SetPurgeFlag(void)
    {
    flags |= RB_PURGE;
    }

  void ClearPurgeFlag(void)
   {
   flags &= (~RB_PURGE);
   }

  BOOL TestClearFlag(void)
   {
   if (flags & RB_PURGE)
     return TRUE;
   else
     return FALSE;
   }

	};


typedef RECORDBLOCK far*	LPRECORDBLOCK;


#endif
