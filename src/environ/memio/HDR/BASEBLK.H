#ifndef __BASEBLK_H__
#define __BASEBLK_H__

#include "classdef.h"
#include "listmbr.h"
#include "rtrec.h"

struct BASEBLOCK_FILE
  {
  RUNTIMEUID  uid;
  LONG        byteCount;
  };

typedef BASEBLOCK_FILE *  LPBASEBLOCK_FILE;

class BASEBLOCK:public RUNTIMEREC
  {
private:

	long int				  chunkID;
	long int				  byteCount;
	void  *			  theData;

  BASEBLOCK(void);

public:

  BASEBLOCK(
    LPRUNTIMERECLIST  pRecList,
    LPBASEBLOCK_FILE  pFileRec);

  BASEBLOCK(
    LPRUNTIMERECLIST  pRecList,
	  long int          size);

  ~BASEBLOCK(void);

	void * ResizeBlock(
		long int	deltaCount);

	void * ReplaceData(
		LPBASEBLOCK  pSourceBlock);

	void DeleteSubBlock(
		long int				start,
		long int				count);

	BOOL Write(
		HANDLE		hFile,
		DWORD	&writeCount);

	BOOL Read(
		HANDLE  			hFile);

  static BOOL Read(
		HANDLE               hFile,
	 LPRUNTIMERECLIST  pBlockList);

	long int GetByteCount(void)
	 {
	 return byteCount;
	 }

	void *	GetData(
		void)
    {
    return theData;
    }

	//assumes that the block is valid if it's zero sized in which it
	//has a -1 chunkID, or if it has a pointer to some data.
	BOOL ValidBlock()
		{
		return ((chunkID == -1) || theData)?TRUE:FALSE;
		}

  static WriteBlockRecords(
	 HANDLE               hFile,
	 LPRUNTIMERECLIST  pRecList);

  static WriteBlockData(
	 HANDLE               hFile,
	 LPRUNTIMERECLIST  pRecList);

  static ReadBlockRecords(
	 HANDLE               hFile,
	 LPRUNTIMERECLIST  pRecList);

  static ReadBlockData(
	 HANDLE               hFile,
	 LPRUNTIMERECLIST  pRecList);


	};

typedef BASEBLOCK *   LPBASEBLOCK;


#endif
