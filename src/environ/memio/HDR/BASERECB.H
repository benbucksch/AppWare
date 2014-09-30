#ifndef __BASERECB_H__
#define __BASERECB_H__

#include "rtrec.h"
#include "listmbr.h"
#include "seruid.h"
#include "classdef.h"

#define RB_PURGE          0X0001
#define RB_PREALLOCATED   0X0002

struct BASERECORDBLOCK_FILE
  {
  RUNTIMEUID      selfID;
	short				    type;
  unsigned short  flags;
	long int				count;
	long int				recordSize;
	short						blockCount;
	RUNTIMEUID			blockIDs;
  };

typedef BASERECORDBLOCK_FILE *   LPBASERECORDBLOCK_FILE;

class BASERECORDBLOCK:public RUNTIMEREC
	{

private:

	short				    type;
  unsigned short  flags;
	long int				count;
	long int				recordSize;
	short						blockCount;
	RUNTIMEUID			blockIDs;

	BASERECORDBLOCK(void);

  void *	AddPreallocatedRecord(
	  size_t	size);

	void CallConstructor(
		LPBASEBLOCK		theBlock);

  static void CallDestructor(
	  void * objPtr,
    short     recType);

	static void UpdateUIDTable(
		LPBASEBLOCK 	theBlock,
		size_t				size);

public:

	BASERECORDBLOCK(
    LPRUNTIMERECLIST  pRecList,
		long int          recordSize,
		long int	        type);

  BASERECORDBLOCK(
    LPRUNTIMERECLIST  pRecList,
	  long int  	      recordSize,
	  long int		      type,
    long int          recordCount);

  BASERECORDBLOCK(
    LPRUNTIMERECLIST        pRecList,
    LPBASERECORDBLOCK_FILE  pFileRec);

	virtual ~BASERECORDBLOCK(void);

	void *	AddRecord(
		size_t	size = 0);

	BOOL DeleteRecord(
		void *	objPtr,
		size_t		size);

	void InitUIDTable(
		void);

  void PurgeBlock(void);

  void FreeMemory(void);

  BOOL PreAllocate(
    long  recordCount);

	void * operator [](long int index);

	long int GetCount(void)
    {
    return count;
    }

	long int GetRecordSize(void)
    {
    return recordSize;
    }

  long int GetType(
    void)
    {
    return type;
    }

  static BOOL WriteRecordBlockRecords(
    HANDLE               hFile,
    LPRUNTIMERECLIST  pRecList);

  static BOOL ReadRecordBlockRecords(
    HANDLE               hFile,
    LPRUNTIMERECLIST  pRecList);

	BOOL Write(
		HANDLE		hFile,
		WORD	&writeCount);

	BOOL Read(
		HANDLE	hFile);

	void *	NextRec(
		void *		currentRec);

	void *	pNextRec(
		void *		currentRec);

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

#endif
