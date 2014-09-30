#ifndef __FASTRECM_H__
#define __FASTRECM_H__

#include "classdef.h"
#include "rtuiddf.h"
#include "memclass.h"

#define DEFBLOCKSIZE	64    //default number of records in a block

struct BLOCKINFO
  {
  RUNTIMEUID blockUID;
  char *  pData;
  };

typedef BLOCKINFO  *  LPBLOCKINFO;

class FASTRECMGR;

typedef FASTRECMGR *  LPFASTRECMGR;

struct PREFIX
	{
	unsigned char	flags;
  long          uid;
  LPFASTRECMGR  pRecMgr;
  long          nextFree;
	};

typedef PREFIX  * LPPREFIX;

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
	int	        recordSize;      //number of bytes in the record

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

  void * AllocateRecord(
    void);

  BOOL FreeRecord(
	  void  *  pRecord);

  static long GetUID(
    void * pRecord)
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
void * GetObject(
  long  uid)
	{
	UINT  blockIndex;
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

	blockIndex = (UINT)(uid/blockSize);
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
		long  &blockKey);


	}; //end of class FASTRECMGR

#endif
