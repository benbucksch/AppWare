#include "almsys.h"

#include "ememstr.h"
#include "baseblk.h"
#include "chunklib.h"
#include "intdebug.h"

#define SEGMENT_SIZE    65535L

#define MAX_IO_SIZE     0X8000


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        BASEBLOCK (constructor)
//
// CLASS.......:        BASEBLOCK
//
// DESCRIPTION.:  This is the constructor normally used to create a new serius
//                                                              block.
//
// PARAMETERS..:  count - the number of records/bytes of storage to allocate
//                        with the new block.
//
//
/////////////////////////////////////////////////////////////////////////////
BASEBLOCK::BASEBLOCK(
  LPRUNTIMERECLIST  pRecList,
	long int          size):
    RUNTIMEREC(pRecList)
	{

	this->theData = AllocateBlock(size, this->chunkID);

	if (this->theData != 0)
    this->byteCount = size;
  else
    this->byteCount = 0;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        BASEBLOCK (constructor)
//
// CLASS.......:        BASEBLOCK
//
// DESCRIPTION.:  This is the constructor normally used to create a new serius
//                                                              block.
//
// PARAMETERS..:  count - the number of records/bytes of storage to allocate
//                        with the new block.
//
//
/////////////////////////////////////////////////////////////////////////////
BASEBLOCK::BASEBLOCK(
  LPRUNTIMERECLIST  pRecList,
	LPBASEBLOCK_FILE  pFileRec):
    RUNTIMEREC(pRecList, pFileRec->uid)
	{

	this->theData = 0;

	this->byteCount = pFileRec->byteCount;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  Read
//
// CLASS.......:  BASEBLOCK
//
// DESCRIPTION.:  Writes the contents of a serius block to the indicated
//                file.  The data is written starting at the current file
//                position.
//
/////////////////////////////////////////////////////////////////////////////
BOOL BASEBLOCK::Read(
	HANDLE             hFile,
	LPRUNTIMERECLIST pBlockList)
	{
  LPBASEBLOCK   pCurrBlock;
  LPBASEBLOCK   pNextBlock;
  long            byteCount;
  MEMCHUNK  *  pCurrChunk = 0;
  LPSTR           currentData;

  pCurrBlock = (LPBASEBLOCK)pBlockList->GetNextMember(0);

  byteCount = 0;

	while (0 != pCurrBlock)
    {
		pNextBlock = (LPBASEBLOCK)pBlockList->GetNextMember(pCurrBlock);
    if (!pCurrChunk)
      {
      pCurrChunk = AllocateChunk(pCurrBlock->byteCount);
      currentData = (LPSTR)pCurrChunk->theChunk;
      }
    if (pCurrBlock->byteCount != 0)
      {
      pCurrBlock->theData = currentData;
      pCurrBlock->chunkID = pCurrChunk->chunkID;
      currentData = currentData + (UINT)(pCurrBlock->byteCount);
      byteCount = byteCount + pCurrBlock->byteCount;
      }
		if ((pNextBlock != 0) &&
        ((byteCount + pNextBlock->byteCount) > SEGMENT_SIZE))
      {
			ReadChunk(hFile, pCurrChunk, byteCount);
      pCurrChunk = 0;
      byteCount = 0;
			}
    pCurrBlock = pNextBlock;
		}

  ReadChunk(hFile, pCurrChunk, byteCount);

  return TRUE;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        ~BASEBLOCK (destructor)
//
// CLASS.......:        BASEBLOCK
//
// DESCRIPTION.:  If the block has allocated any memory, that memory is
//                                                              released.
//
// ASSUMPTIONS.:        Assumes that chunkID is -1 if and only if no memory is
//                                                              currently allocated by the block.
//
/////////////////////////////////////////////////////////////////////////////
BASEBLOCK::~BASEBLOCK(
	void)
	{

	if (chunkID != -1)
		ReleaseBlock(chunkID, byteCount, theData);

  return;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        ResizeBlock
//
// CLASS.......:  BASEBLOCK
//
// DESCRIPTION.:        The resize block method increases or decreases the size
//                                                              of the block by the indicated records/bytes.
//
// PARAMETERS..:  deltaCount - the size in bytes/records that the block
//                                                                      size is to be changed.  A positive value will increase
//                                                                      the size and a negative value will decrease the size.
//
// RETURN VALUE:        The function returns a pointer to the beginning of the
//                                                              new memory if the size was increased, or a pointer to the
//                                                              beginning of the memory if the size was decreased.
//
// SIDE-EFFECTS:        The data stored in the block is moved to a different
//                                                              area of memory.  This invalidates any stored pointers to
//                                                              that memory.
//
// ASSUMPTIONS:         Assumes that AllocateBlock sets the value of byteCount.
//
/////////////////////////////////////////////////////////////////////////////
void * BASEBLOCK::ResizeBlock(
	long int  deltaCount)
	{
	long int   newSize;
	long int   oldSize;
	void *  dataPtr;
	void *  oldData;
	long int   oldChunkID;
	long int   oldCount;

	if (chunkID == -1)
		{
		if (deltaCount < 0)
			{
			byteCount = 0;
			this->theData = dataPtr = 0;
			}
		else
			{
			dataPtr = this->theData = AllocateBlock(deltaCount, this->chunkID);
      this->byteCount = deltaCount;
			}
		}
	else if (byteCount+deltaCount <= 0)
		{
		ReleaseBlock(chunkID, this->byteCount, this->theData);
		this->theData = dataPtr = 0;
		byteCount = 0;
		chunkID = -1;
		}
	else if ((byteCount > SEGMENT_SIZE) &&
		 (byteCount+deltaCount > SEGMENT_SIZE))
    {
    this->theData = ResizeChunk(this->chunkID, byteCount+deltaCount);
    if (this->theData != 0)
      {
      if (deltaCount > 0)
        dataPtr = ((char  *)this->theData) + this->byteCount;
			else
        dataPtr = this->theData;
      this->byteCount = this->byteCount + deltaCount;
      }
    else
      {
      dataPtr = 0;
      this->byteCount = 0;
			}
    }
	else
		{
		oldSize = byteCount;
		newSize = byteCount + deltaCount;
		oldData = theData;
		oldChunkID = chunkID;
		oldCount = byteCount;
		this->theData = AllocateBlock(newSize, this->chunkID);
		if (this->theData != 0)
      {
      if (newSize < oldSize)
        CopyMemory(theData, oldData, (UINT)(newSize));
      else
        CopyMemory(theData, oldData, (UINT)(oldSize));
		  if (deltaCount > 0)
			  dataPtr = (char *)theData + (UINT)(oldSize); //return pointer to new space
		  else
				dataPtr = this->theData;
			this->byteCount = newSize;
      }
    else
      {
      dataPtr = 0;
      this->byteCount = 0;
      }
		ReleaseBlock(oldChunkID, oldCount, oldData);
		}

	return dataPtr;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ReplaceData
//
// CLASS.......:  BASEBLOCK
//
// DESCRIPTION.:  This function replaces the data refereced by the block
//                with the data referenced by the source block.  The
//                previously referenced data is freed.  The source block will
//                will not reference any data when the function returns.
//
// RETURN VALUE:  This function returns a pointer to the new data.
//
// SIDE-EFFECTS:  The pSourceBlock is changed to not reference any data.
//
/////////////////////////////////////////////////////////////////////////////
void * BASEBLOCK::ReplaceData(
	LPBASEBLOCK  pSourceBlock)
	{

	I_ASSERT(pSourceBlock != 0);

	if (this->chunkID > -1)
		{
		ReleaseBlock(this->chunkID, this->byteCount, this->theData);
		this->theData = 0;
		this->byteCount = 0;
		this->chunkID = -1;
		}

	this->theData = pSourceBlock->theData;
	this->byteCount = pSourceBlock->byteCount;
	this->chunkID = pSourceBlock->chunkID;

	pSourceBlock->theData = 0;
	pSourceBlock->byteCount = 0;
	pSourceBlock->chunkID = -1;

	return this->theData;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        DeleteSubBlock
//
// CLASS.......:        BASEBLOCK
//
// DESCRIPTION.:  This function deletes a subblock of bytes or records from
//                                                              the block.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:        The block's data is moved to a new location in memory. This
//                                                              invalidates any externally saved pointers to the block's
//                                                              data.
/////////////////////////////////////////////////////////////////////////////

void BASEBLOCK::DeleteSubBlock(
	long int                                start,
	long int                                count)
	{
	long int                                originalLength;
	long int                                newSize;
	void *                               oldData;
	long int                                oldChunkID;
	long int                                oldCount;

	oldData = this->theData;
	oldChunkID = this->chunkID;
	oldCount = this->byteCount;

	originalLength = this->byteCount;
	newSize = this->byteCount - count;

	this->theData = AllocateBlock(newSize, this->chunkID);

  if (0 != this->theData)
    {
		this->byteCount = newSize;
		CopyMemory(this->theData, oldData, (UINT)(start-1));
		CopyMemory((char  *)this->theData+(UINT)(start-1),
			(char  *)oldData+(UINT)(start+count-1),
			(UINT)(originalLength-count-(start-1)));
		}
  else
    this->byteCount = 0;

	ReleaseBlock(oldChunkID, oldCount, oldData);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        Read
//
// CLASS.......:        BASEBLOCK
//
// DESCRIPTION.:  This is the constructor is used to create a serious block
//                                                              read from a file.
//
// PARAMETERS..:  count - the number of records/bytes of storage to allocate
//                                                                      with the new block.
//
//                                                              recordSize - the size in bytes of the records to be stored
//                                                                      in the serius block.  If recordSize is zero, count is
//                                                                      the number of bytes to be stored in the block, otherwise
//                                                                      count is the number of records to be stored in the block.
//
// RETURN VALUE:  returns a pointer to the serius block record.
//
/////////////////////////////////////////////////////////////////////////////
BOOL BASEBLOCK::Read(
	HANDLE  hFile)
	{
	BOOL         result = TRUE;
	char  *			 pData;
	long         readSize;

	this->theData = AllocateBlock(this->byteCount, this->chunkID);

	if (this->byteCount <= SEGMENT_SIZE)
		{
		DWORD dwBytesRead;
		ReadFile(hFile, (LPSTR)theData, this->byteCount, &dwBytesRead, NULL);
		if (this->byteCount != dwBytesRead)
			{
			I_PRINT("BASEBLOCK::Read() - Error in reading Block Data from File");
			result = FALSE;
			}
		}
	else
		{
		pData = (char  *)theData;
		readSize = this->byteCount;
		while ((readSize > MAX_IO_SIZE)  && (result == TRUE))
			{
			DWORD dwBytesRead;
			ReadFile(hFile, (LPSTR)pData, MAX_IO_SIZE, &dwBytesRead, NULL);

			if (MAX_IO_SIZE != dwBytesRead) // ReadFile(hFile, (LPSTR)pData, MAX_IO_SIZE))
				{
				I_PRINT("BASEBLOCK::Read() - Error in reading Block Data from File");
				result = FALSE;
				}
			readSize -= MAX_IO_SIZE;
			pData += MAX_IO_SIZE;
			}
		if (result == TRUE)
			{
			DWORD dwBytesRead;
			ReadFile(hFile, (LPSTR)pData, readSize, &dwBytesRead, NULL);

			if (readSize != dwBytesRead) // ReadFile(hFile, (LPSTR)pData, readSize))
				{
				I_PRINT("BASEBLOCK::Read() - Error in reading Block Data from File");
				result = FALSE;
				}
			}
		}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  Write
//
// CLASS.......:  BASEBLOCK
//
// DESCRIPTION.:  Writes the contents of a serius block to the indicated
//                file.  The data is written starting at the current file
//                position.
//
/////////////////////////////////////////////////////////////////////////////
BOOL BASEBLOCK::Write(
	HANDLE  hFile,
	DWORD &writeCount)
	{
	BOOL         result = TRUE;
	DWORD         tempCount;
//	char  *			 pData;
	long         writeSize;


	if (byteCount <= SEGMENT_SIZE)
		{

		WriteFile(hFile, (LPSTR)theData, byteCount, &tempCount, NULL); // SKJ
		if (byteCount != tempCount) //  = WriteFile(hFile, (LPSTR)theData, byteCount)))
			{
			I_PRINT("BASEBLOCK::Write() - Error in writing Block Data to File");
			result = FALSE;
			}
		writeCount += tempCount;
		}
	else
		{
//		pData = (char  *)theData;
		writeSize = byteCount;
		while ((writeSize > MAX_IO_SIZE)  && (result == TRUE))
			{

			WriteFile(hFile, (LPSTR)theData, MAX_IO_SIZE, &tempCount, NULL);
			if (MAX_IO_SIZE != tempCount) // SKJ (tempCount = WriteFile(hFile, (LPSTR)pData, MAX_IO_SIZE)))
				{
				I_PRINT("BASEBLOCK::Write() - Error in writing Block Data to File");
				result = FALSE;
				}
			writeSize -= tempCount;
//			pData += MAX_IO_SIZE;
			}
		if (result == TRUE)
			{
			WriteFile(hFile, (LPSTR)theData, writeSize, &tempCount, NULL); // SkJ	
			if (writeSize != tempCount) //  = WriteFile(hFile, (LPSTR)pData, writeSize)))
				{
				I_PRINT("BASEBLOCK::Write() - Error in writing Block Data to File");
				result = FALSE;
				}
			writeCount += tempCount;
			}
		}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  WriteBlockRecords
//
// CLASS.......:  BASEBLOCK
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL BASEBLOCK::WriteBlockRecords(
	HANDLE               hFile,
	LPRUNTIMERECLIST  pRecList)
  {
  LPBASEBLOCK     pBlock = 0;
  DWORD 				writeCount;
  BASEBLOCK_FILE  fileRec;
  BOOL            result = TRUE;


  while (0 != (pBlock = (LPBASEBLOCK)pRecList->GetNextMember(pBlock)))
    {
    fileRec.uid = pBlock->GetUID();
	 fileRec.byteCount = pBlock->byteCount;
	 WriteFile(hFile, (LPSTR)&fileRec, sizeof(BASEBLOCK_FILE), &writeCount, NULL); // SkJ

	 // writeCount = WriteFile(hFile, (LPSTR)&fileRec, sizeof(BASEBLOCK_FILE));
    if (writeCount != sizeof(BASEBLOCK_FILE))
      result = FALSE;
    }

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  WriteBlockData
//
// CLASS.......:  BASEBLOCK
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL BASEBLOCK::WriteBlockData(
  HANDLE               hFile,
  LPRUNTIMERECLIST  pRecList)
  {
  LPBASEBLOCK     pBlock = 0;
	DWORD            writeCount;
	BASEBLOCK_FILE  fileRec;
	BOOL            result = TRUE;

	while (0 != (pBlock = (LPBASEBLOCK)pRecList->GetNextMember(pBlock)))
		{
		if (FALSE == pBlock->Write(hFile, writeCount))
			result = FALSE;
		}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ReadBlockRecords
//
// CLASS.......:  BASEBLOCK
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL BASEBLOCK::ReadBlockRecords(
  HANDLE               hFile,
  LPRUNTIMERECLIST  pRecList)
  {
//  LPBASEBLOCK     pBlock;
  long            i;
  UINT            readCount;
  BASEBLOCK_FILE  fileRec;
  BOOL            result = TRUE;

  for (i=0; i<pRecList->GetCount(); i++)
		{
		DWORD dwBytesRead;
		ReadFile(hFile, (LPSTR)&fileRec, sizeof(BASEBLOCK_FILE), &dwBytesRead, NULL);

		readCount = dwBytesRead; // ReadFile(hFile, (LPSTR)&fileRec, sizeof(BASEBLOCK_FILE));
		if (readCount != sizeof(BASEBLOCK_FILE))
			result = FALSE;
		else
			{
			new BASEBLOCK(pRecList, &fileRec);
			}
		}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ReadBlockData
//
// CLASS.......:  BASEBLOCK
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL BASEBLOCK::ReadBlockData(
	HANDLE               hFile,
	LPRUNTIMERECLIST  pRecList)
	{
  LPBASEBLOCK     pBlock = 0;
//  WORD            writeCount;
  BASEBLOCK_FILE  fileRec;
  BOOL            result = TRUE;

  while (0 != (pBlock = (LPBASEBLOCK)pRecList->GetNextMember(pBlock)))
    {
    if (FALSE == pBlock->Read(hFile))
      result = FALSE;
    }

  return result;

  }
