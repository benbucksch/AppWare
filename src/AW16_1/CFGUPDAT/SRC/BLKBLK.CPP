#include <windows.h>
#include "serdef.h"
#include "blkblk.h"
#include "recprefx.h"
#include "system.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator new
//
// CLASS.......:	SERIUSBLOCKBLOCK
//
// DESCRIPTION.:  Allocates SERIUSBLOCK records from the serius block
//								contained within the file object of the current context.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void far * SERIUSBLOCKBLOCK::operator new(
	size_t				size)
	{
	RECORDPREFIX far*		objPtr;
	LPSERIUSBLOCK 			currentBlock;
	LPUIDTABLE					uidTable;

	currentBlock = SYSTEM::GetFileObject()->GetBlockBlocks();

	objPtr = (RECORDPREFIX far*)currentBlock->
	 ResizeBlock(size+sizeof(RECORDPREFIX));

	uidTable = SYSTEM::GetFileObject()->GetUIDTable();

	objPtr->UID = uidTable->GenerateUID(objPtr + 1);

	UpdateUIDTable(currentBlock, uidTable);

	return (objPtr + 1);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator new
//
// CLASS.......:	SERIUSBLOCKBLOCK
//
// DESCRIPTION.:  Allocates SERIUSBLOCK records from the serius block
//								contained within the file object of the current context.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void far * SERIUSBLOCKBLOCK::operator new(
	size_t				size,
	LPSERIUSBLOCK	blockBlocks,
	LPUIDTABLE		uids)
	{
	RECORDPREFIX far*		objPtr;

	objPtr = (RECORDPREFIX far*)blockBlocks->
	 ResizeBlock(size+sizeof(RECORDPREFIX));

	objPtr->UID = uids->GenerateUID(objPtr + 1);

	UpdateUIDTable(blockBlocks, uids);

	return (objPtr + 1);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator new
//
// CLASS.......:	SERIUSBLOCKBLOCK
//
// DESCRIPTION.:	This version of the new operator is used to initialize
//							  a record read from disk.
//
// PARAMETERS..:  size -  the size of the seriusblock record.
//								objPtr - a pointer to the record  read from disk.  This
//									pointer is returned by the operator.  No memory is
//									allocated.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void far * SERIUSBLOCKBLOCK::operator new(
	size_t			/*size*/,
	void far*		objPtr)
	{

	return (objPtr);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator delete
//
// CLASS.......:	SERIUSBLOCKBLOCK
//
// DESCRIPTION.:  This operator deletes a SERIUSBLOCK record from the
//								serius block  contained within the FILEOBJECT record of
//								current context.
//
// PARAMETERS..:  objPtr - a pointer to the object to be deleted.
//
// ASSUMPTIONS.:	The objPtr must point to a SERIUSBLOCK record within the
//								current context.
//
// SIDE EFFECTS:	The block is resized and thus moved in memory.
/////////////////////////////////////////////////////////////////////////////
void SERIUSBLOCKBLOCK::operator delete(
	void far *	objPtr)
	{
	RECORDPREFIX far*	prefix;
	LPSERIUSBLOCK 		theBlock;
	long int					byteStart;
	long int			  	recSize;
	long int					originalLength;
	void far*					oldData;
	long int					oldBlockMemID;
	long int					oldCount;

	theBlock = SYSTEM::GetFileObject()->GetBlockBlocks();

	prefix = (RECORDPREFIX far*)objPtr - 1;

	SYSTEM::GetFileObject()->GetUIDTable()->ReleaseUID(prefix->UID);

	byteStart =
   LOWORD((unsigned long)prefix) - LOWORD((unsigned long)theBlock->theData);

	originalLength = theBlock->byteCount;

	oldData = theBlock->theData;
	oldBlockMemID = theBlock->blockMemID;
	oldCount = theBlock->byteCount;

  recSize = sizeof(SERIUSBLOCK) + sizeof(RECORDPREFIX);

	theBlock->AllocateBlock(theBlock->byteCount - recSize);

	if (theBlock->theData)
		{
    //copy bytes preceding the record
		_fmemcpy(theBlock->theData, oldData, (U_PTRMATH_T)byteStart);

    //copy bytes after the record
		_fmemcpy((char far *)theBlock->theData+(U_PTRMATH_T)byteStart,
		 (char far *)oldData+(U_PTRMATH_T)(recSize+byteStart),
		 (U_PTRMATH_T)(originalLength-recSize-byteStart));
		}

	ReleaseMemory(oldBlockMemID, oldCount, oldData);

	UpdateUIDTable(theBlock, SYSTEM::GetFileObject()->GetUIDTable());

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	UpdateUIDTable
//
// CLASS.......:	SERIUSBLOCKBLOCK
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void SERIUSBLOCKBLOCK::UpdateUIDTable(
	LPSERIUSBLOCK 	theBlock,
	LPUIDTABLE			uidTable)
	{
	long int				    i;
	long int						count;
	RECORDPREFIX far*		objPtr;
	char far*						theData;
	size_t							size;

	size = sizeof(SERIUSBLOCKBLOCK) + sizeof(RECORDPREFIX);

	count = theBlock->GetByteCount()/size;

	theData = (char far*)theBlock->GetData();

	for (i=0; i<count; i++)
		{
		objPtr = (RECORDPREFIX far*)(theData + (U_PTRMATH_T)(i*size));
		uidTable->SetPointer(objPtr->UID, objPtr+1);
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	InitUIDTable
//
// CLASS.......:	SERIUSBLOCKBLOCK
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void SERIUSBLOCKBLOCK::InitUIDTable(
	LPSERIUSBLOCK	pBlock,
	LPUIDTABLE		uidTable)
	{
	long int				    i;
	long int						count;
	RECORDPREFIX far*		objPtr;
	char far*						theData;
	size_t							size;

	size = sizeof(SERIUSBLOCKBLOCK) + sizeof(RECORDPREFIX);

	count = pBlock->GetByteCount()/size;

	theData = (char far*)pBlock->GetData();

	for (i=0; i<count; i++)
		{
		objPtr = (RECORDPREFIX far*)(theData + (U_PTRMATH_T)(i*size));
		uidTable->InitPointer(objPtr->UID, objPtr+1);
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	NextRec
//
// CLASS.......:	SERIUSBLOCKBLOCK
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
LPSERIUSBLOCKBLOCK SERIUSBLOCKBLOCK::NextRec(
	LPSERIUSBLOCK				theBlock,
	LPSERIUSBLOCKBLOCK	currentRec)
	{
	void far*		 nextRec;

  if (theBlock->theData == 0)
    nextRec = 0;
  else if (currentRec == 0)
    {
    if (sizeof(RECORDPREFIX) < theBlock->byteCount)
  		nextRec = (LPRECORDPREFIX)theBlock->theData + 1;
    else
      nextRec = 0;
    }
  //ensure next rec will not be beyond the end of the block.
  else if (((long)FP_OFF(currentRec) + sizeof(SERIUSBLOCKBLOCK) +
            sizeof(RECORDPREFIX))  <=
			     ((long)FP_OFF(theBlock->theData) + theBlock->byteCount -
            sizeof(SERIUSBLOCKBLOCK)))
		nextRec = ((LPRECORDPREFIX)(currentRec+1) + 1);
  else
		nextRec = 0;

	return (LPSERIUSBLOCKBLOCK)nextRec;

	}
