#ifndef __FASTRECM_H__
#define __FASTRECM_H__

#include "classdef.h"
#include "rtuiddf.h"
#include "memclass.h"
#include "serdef.h"

#define DEFBLOCKSIZE	64    //default number of records in a block

struct BLOCKINFO
  {
  RUNTIMEUID blockUID;
  char far*  pData;
  };

typedef BLOCKINFO far *  LPBLOCKINFO;

class FASTRECMGR;

typedef FASTRECMGR far*  LPFASTRECMGR;

struct PREFIX
	{
	unsigned char	flags;
  long          uid;
  LPFASTRECMGR  pRecMgr;
  long          nextFree;
	};

typedef PREFIX far * LPPREFIX;

//Flags for PREFIX::flags

#define	FLAG_USED       0X01

class FASTRECMGR:public MEMORY
	{

private:

	long 	 		   freeList;        //index of first free
	long 	 		   blockCount;      //number of blocks in block array
	RUNTIMEUID 	 blockTableUID;   //uid of block array
  LPBLOCKINFO  blockInfoArray;
	WORD			   blockSize;       //number of records in table
  WORD         recordSize;      //number of bytes in the record

  void AllocateTable(
    void);

  void FreeBlocks(
    void);

public:

	FASTRECMGR(void);

  FASTRECMGR(
	  WORD  blockRecordCount,
    long  recordSize);

  ~FASTRECMGR(void);

  void far* AllocateRecord(
    void);

  BOOL FreeRecord(
	  void far *  pRecord);

  static long GetUID(
    void far* pRecord)
    {
    return ((LPPREFIX)pRecord - 1)->uid;
    }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetObject
//
// CLASS.......:  FASTRECMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void far* GetObject(
  long  uid)
	{
	U_PTRMATH_T  blockIndex;
	long int	   recordByteOffset;
  LPPREFIX     pPrefix;

  uid = -uid;

  if ((uid <= 0) || (uid > blockCount*blockSize))
    {
#ifdef DEBUG
		MessageBox(NULL, "Uid out of range in call to FASTRECMGR::GetObject",
		 "Yo, Programmer Dude!", MB_OK);
    DebugBreak();
#endif
		return 0;
    }

	blockIndex = (U_PTRMATH_T)(uid/blockSize);
	recordByteOffset = uid%blockSize * (recordSize+sizeof(PREFIX));

  pPrefix = (LPPREFIX)(blockInfoArray[blockIndex].pData + (int)recordByteOffset);

	if (pPrefix->flags & FLAG_USED)
    return (pPrefix + 1);
  else
    {
#ifdef DEBUG
		MessageBox(NULL, "Free Uid in call to FASTRECMGR::GetObject",
		 "Yo, Programmer Dude!", MB_OK);
    DebugBreak();
#endif
		return 0;
    }

	}


	LPSTR GetNextUsedRecord(
		LPSTR prevRecord,
		long far &blockKey);


	}; //end of class FASTRECMGR

#endif
