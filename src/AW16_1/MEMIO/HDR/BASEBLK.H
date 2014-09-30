#ifndef __BASEBLK_H__
#define __BASEBLK_H__

#include <memory.h>
#include "classdef.h"
#include "listmbr.h"
#include "rtrec.h"

struct BASEBLOCK_FILE
  {
  RUNTIMEUID  uid;
  LONG        byteCount;
  };

typedef BASEBLOCK_FILE far*  LPBASEBLOCK_FILE;

class BASEBLOCK:public RUNTIMEREC
  {
private:

	long int				  chunkID;
	long int				  byteCount;
	void far *			  theData;

  BASEBLOCK(void);

public:

  BASEBLOCK(
    LPRUNTIMERECLIST  pRecList,
    LPBASEBLOCK_FILE  pFileRec);

  BASEBLOCK(
    LPRUNTIMERECLIST  pRecList,
	  long int          size);

  ~BASEBLOCK(void);

	void far* ResizeBlock(
		long int	deltaCount);

	void far* ReplaceData(
		LPBASEBLOCK  pSourceBlock);

	void DeleteSubBlock(
		long int				start,
		long int				count);

	BOOL Write(
		int		hFile,
		WORD	&writeCount);

	BOOL Read(
		int  			hFile);

  static BOOL Read(
		int               hFile,
    LPRUNTIMERECLIST  pBlockList);

	long int GetByteCount(void)
    {
    return byteCount;
    }

	void far*	GetData(
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
    int               hFile,
    LPRUNTIMERECLIST  pRecList);

  static WriteBlockData(
    int               hFile,
    LPRUNTIMERECLIST  pRecList);

  static ReadBlockRecords(
    int               hFile,
    LPRUNTIMERECLIST  pRecList);

  static ReadBlockData(
    int               hFile,
    LPRUNTIMERECLIST  pRecList);


	};

typedef BASEBLOCK far*   LPBASEBLOCK;


#endif
