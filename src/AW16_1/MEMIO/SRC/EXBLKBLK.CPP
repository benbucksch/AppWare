#include "almsys.h"

#include "serdef.h"
#include "exblkblk.h"
#include "recprefx.h"
#include "system.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator new
//
// CLASS.......:	EXESERIUSBLOCKBLOCK
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
void far * EXESERIUSBLOCKBLOCK::operator new(
	size_t				size)
	{
	RECORDPREFIX far*		objPtr;
	LPSERIUSBLOCK 			currentBlock;
	LPUIDTABLE					uidTable;

	currentBlock = pTheSystem->GetFileObject()->GetExeBlockBlocks();

	objPtr = (RECORDPREFIX far*)currentBlock->
	 ResizeBlock(size+sizeof(RECORDPREFIX));

	uidTable = pTheSystem->GetFileObject()->GetUIDTable();

	objPtr->UID = uidTable->GenerateUID(objPtr + 1);

	UpdateUIDTable(currentBlock, uidTable);

	return (objPtr + 1);

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator new
//
// CLASS.......:	EXESERIUSBLOCKBLOCK
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
void far * EXESERIUSBLOCKBLOCK::operator new(
	size_t			/*size*/,
	void far*		objPtr)
	{

	return (objPtr);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator delete
//
// CLASS.......:	EXESERIUSBLOCKBLOCK
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
void EXESERIUSBLOCKBLOCK::operator delete(
	void far *	objPtr)
	{
	RECORDPREFIX far*	prefix;
	LPSERIUSBLOCK 		theBlock;
	long int					byteStart;
	long int				  recSize;
	long int				  originalLength;
	void far*					oldData;
	long int					oldBlockMemID;
	long int					oldCount;

	theBlock = pTheSystem->GetFileObject()->GetExeBlockBlocks();

	prefix = (RECORDPREFIX far*)objPtr - 1;

	pTheSystem->GetFileObject()->GetUIDTable()->ReleaseUID(prefix->UID);

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
		_fmemcpy(theBlock->theData, oldData, (U_PTRMATH_T)byteStart);
		_fmemcpy((char far *)theBlock->theData+(U_PTRMATH_T)byteStart,
		 (char far *)oldData+(U_PTRMATH_T)(recSize+byteStart),
		 (U_PTRMATH_T)(originalLength-recSize-byteStart));
		}

	ReleaseMemory(oldBlockMemID, oldCount, oldData);

	UpdateUIDTable(theBlock, pTheSystem->GetFileObject()->GetUIDTable());

	}

