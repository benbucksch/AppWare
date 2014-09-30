#include <windows.h>
#include <mem.h>
#include <dos.h>
#include "serdef.h"
#include "debug.h"
#include "switches.h"
#include "ememstr.h"
#include "serblk.h"
#include "blkblk.h"
#include "system.h"
#include "sereclib.h"
#include "objdesc.h"
#include "funcdesc.h"
#include "methdesc.h"
#include "evntdesc.h"
#include "cfgdesc.h"
#include "rectypes.h"


#define MIN_BLOCKID_ARRAY_SIZE	8
#define SEGMENT_SIZE	65535L

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator new
//
// CLASS.......:	RECORDBLOCK
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
void far * RECORDBLOCK::operator new(
	size_t	size)
	{

	return SYSTEM::GetFileObject()->GetRecordBlocks()->AddRecord(size);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator new
//
// CLASS.......:	RECORDBLOCK
//
// DESCRIPTION.:	This version of the new operator is used to initialize
//								the virtual table pointers of a record read from disk.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void far * RECORDBLOCK::operator new(
	size_t			/*size*/,
	void far*		objPtr)
	{

	return (objPtr);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	operator delete
//
// CLASS.......:	RECORDBLOCK
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
void RECORDBLOCK::operator delete(
	void far *	objPtr,
	size_t			size)
	{

	SYSTEM::GetFileObject()->GetRecordBlocks()->DeleteRecord(objPtr, size);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: RECORDBLOCK (constructor)
//
// CLASS.......: RECORDBLOCK
//
// DESCRIPTION.: This is the default constructor.  No data space is allocated
//							 by this constructor.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
RECORDBLOCK::RECORDBLOCK(
	void)
	{

	count = 0;
	recordSize = 0;
	type =	0;
	blockCount = 0;
	blockIDs = NULLSERUID;
  flags = 0;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	RECORDBLOCK (constructor)
//
// CLASS.......:	RECORDBLOCK
//
// DESCRIPTION.:  This is the constructor normally used to create a new serius
//								block.
//
// PARAMETERS..:	recordSize - the size in bytes of the records to be stored
//									in the serius block.  If recordSize is zero, count is
//									the number of bytes to be stored in the block, otherwise
//									count is the number of records to be stored in the block.
//
//								type - the type constant for the record type.
//
// 							  count - the number of records/bytes of storage to allocate
//									with the new block.
//
// RETURN VALUE:  returns a pointer to the serius block record.
//
/////////////////////////////////////////////////////////////////////////////
RECORDBLOCK::RECORDBLOCK(
	long int  	recordSize,
	long int		type)
	{
	this->recordSize = recordSize;
	this->type = type;
	this->count = 0;
	this->blockCount = 0;
	this->blockIDs = NULLSERUID;
  this->flags = 0;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	RECORDBLOCK (constructor)
//
// CLASS.......:	RECORDBLOCK
//
// DESCRIPTION.:  This is the constructor normally used to create a new serius
//								block.
//
// PARAMETERS..:	recordSize - the size in bytes of the records to be stored
//									in the serius block.  If recordSize is zero, count is
//									the number of bytes to be stored in the block, otherwise
//									count is the number of records to be stored in the block.
//
//								type - the type constant for the record type.
//
// 							  count - the number of records/bytes of storage to allocate
//									with the new block.
//
// RETURN VALUE:  returns a pointer to the serius block record.
//
/////////////////////////////////////////////////////////////////////////////
RECORDBLOCK::RECORDBLOCK(
	long int  	recordSize,
	long int		type,
  long int    recordCount)
	{

	this->recordSize = recordSize;
	this->type = type;
	this->count = 0;

  this->PreAllocate(recordCount);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	PreAllocate
//
// CLASS.......:	RECORDBLOCK
//
// DESCRIPTION.:  This is the constructor normally used to create a new serius
//								block.
//
// PARAMETERS..:	recordSize - the size in bytes of the records to be stored
//									in the serius block.  If recordSize is zero, count is
//									the number of bytes to be stored in the block, otherwise
//									count is the number of records to be stored in the block.
//
//								type - the type constant for the record type.
//
// 							  count - the number of records/bytes of storage to allocate
//									with the new block.
//
// RETURN VALUE:  returns a pointer to the serius block record.
//
/////////////////////////////////////////////////////////////////////////////
BOOL RECORDBLOCK::PreAllocate(
  long  recordCount)
  {
  int           blockIdx;
  long          lastBlockSize;
  LPSERIUSBLOCK pArrayBlock;
  LPSERUID      pUIDArray;

	if ((this->count != 0) || (recordCount == 0))
    return FALSE;

  this->flags = RB_PREALLOCATED;

	this->blockCount =
   (recordCount * (recordSize + sizeof(RECORDPREFIX))) / SEGMENT_SIZE;

  lastBlockSize =
   (recordCount * (recordSize + sizeof(RECORDPREFIX))) % SEGMENT_SIZE;

  if (lastBlockSize)
    this->blockCount++;

	if (this->blockCount == 0)
    this->blockIDs = NULLSERUID;
  else if (this->blockCount == 1)
    {
		if (type == RT_SERIUSBLOCK)
			this->blockIDs = GetUID(new SERIUSBLOCKBLOCK(lastBlockSize));
		else
			this->blockIDs = GetUID(new SERIUSBLOCK(lastBlockSize));
    }
  else
    {
		if (type == RT_SERIUSBLOCK)
			pArrayBlock = new SERIUSBLOCKBLOCK(this->blockCount*sizeof(SERUID));
		else
			pArrayBlock = new SERIUSBLOCK(this->blockCount*sizeof(SERUID));
    this->blockIDs = GetUID(pArrayBlock);
    pUIDArray = (LPSERUID)pArrayBlock->GetData();
    for (blockIdx = 0; blockIdx < this->blockCount-1; blockIdx++)
      {
		  if (type == RT_SERIUSBLOCK)
			  pUIDArray[blockIdx] = GetUID(new SERIUSBLOCKBLOCK(SEGMENT_SIZE));
		  else
			  pUIDArray[blockIdx] = GetUID(new SERIUSBLOCK(SEGMENT_SIZE));
      }
		if (type == RT_SERIUSBLOCK)
			pUIDArray[this->blockCount-1] =
       GetUID(new SERIUSBLOCKBLOCK(lastBlockSize));
		else
			pUIDArray[this->blockCount-1] =
       GetUID(new SERIUSBLOCK(lastBlockSize));
    }

  return TRUE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	~RECORDBLOCK (destructor)
//
// CLASS.......:	RECORDBLOCK
//
// DESCRIPTION.:  If the block has allocated any memory, that memory is
//								released.
//
// ASSUMPTIONS.:
//
/////////////////////////////////////////////////////////////////////////////
RECORDBLOCK::~RECORDBLOCK(
	void)
	{

  this->FreeMemory();

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	FreeMemory
//
// CLASS.......:	RECORDBLOCK
//
// DESCRIPTION.:  If the block has allocated any memory, that memory is
//								released.
//
// ASSUMPTIONS.:
//
/////////////////////////////////////////////////////////////////////////////
void RECORDBLOCK::FreeMemory(
	void)
	{
	int 						blockIndex;
	LPSERIUSBLOCK		pBlock;
	LPSERUID				arrayPtr;

	if (blockIDs != NULLSERUID)
		{
		pBlock = (LPSERIUSBLOCK)GetObject(blockIDs);
		if (blockCount > 1)
			{
			arrayPtr = (LPSERUID)pBlock->GetData();
			for (blockIndex=0; blockIndex<blockCount; blockIndex++)
			  if (type == RT_SERIUSBLOCK)
				  delete (LPSERIUSBLOCKBLOCK)GetObject(arrayPtr[blockIndex]);
			  else
				  delete (LPSERIUSBLOCK)GetObject(arrayPtr[blockIndex]);

      //need to refresh block pointer after deletes above
      pBlock = (LPSERIUSBLOCK)GetObject(blockIDs);
			}
		if (type == RT_SERIUSBLOCK)
			delete (LPSERIUSBLOCKBLOCK)pBlock;
		else
			delete (LPSERIUSBLOCK)pBlock;
		}

	count = 0;
	blockCount = 0;
	blockIDs = NULLSERUID;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	AddRecord
//
// CLASS.......:	RECORDBLOCK (public)
//
// DESCRIPTION.:  Deletes a single record from a block that contains fixed
//
// PARAMETERS..:	objPtr - a pointer to the record to be deleted.
//
// RETURN VALUE:	Returns TRUE if the record is deleted. Otherwise, FALSE
//								is returned.
//
/////////////////////////////////////////////////////////////////////////////
void far*	RECORDBLOCK::AddRecord(
	size_t	size)
	{
	int 						blockIndex;
	LPSERIUSBLOCK		pBlock;
	LPSERIUSBLOCK		pArrayBlock;
	void far*				recPtr = NULL;
	LPRECORDPREFIX	prefixPtr;
	SERUID					tempuid;
	LPSERUID				pUidArray;

  if (this->flags & RB_PREALLOCATED)
    return AddPreallocatedRecord(size);

	if ((size != 0) && (size == recordSize))
		{
		if (blockIDs == NULLSERUID)
			{
			if (type == RT_SERIUSBLOCK)
				pBlock = new SERIUSBLOCKBLOCK();
			else
				pBlock = new SERIUSBLOCK();
			blockIDs = GetUID(pBlock);
			blockCount += 1;
			}
		else if (blockCount == 1)
			{
			pBlock = (LPSERIUSBLOCK)GetObject(blockIDs);
			if (pBlock->GetByteCount()+recordSize+sizeof(RECORDPREFIX) >
					SEGMENT_SIZE)
				{
				tempuid = blockIDs;
				if (type == RT_SERIUSBLOCK)
					pArrayBlock = new SERIUSBLOCKBLOCK(2*sizeof(SERUID));
				else
					pArrayBlock = new SERIUSBLOCK(2*sizeof(SERUID));
				blockIDs = GetUID(pArrayBlock);
				pUidArray = (LPSERUID)pArrayBlock->GetData();
				pUidArray[0] = tempuid;
				if (type == RT_SERIUSBLOCK)
					pBlock = new SERIUSBLOCKBLOCK();
				else
					pBlock = new SERIUSBLOCK();
				pUidArray[1] = GetUID(pBlock);
				blockCount = 2;
				}
			}
		else
			{
			pArrayBlock = (LPSERIUSBLOCK)GetObject(blockIDs);
			pUidArray = (LPSERUID)pArrayBlock->GetData();
			for (blockIndex=0; blockIndex<blockCount; blockIndex++)
				{
				pBlock = (LPSERIUSBLOCK)GetObject(pUidArray[blockIndex]);
				if (pBlock->GetByteCount()+recordSize+sizeof(RECORDPREFIX) <
						SEGMENT_SIZE)
					break;
				}
			if (blockIndex == blockCount)
				{
				blockCount++;
				if (type == RT_SERIUSBLOCK)
					pBlock = new SERIUSBLOCKBLOCK();
				else
					pBlock = new SERIUSBLOCK();
				pArrayBlock = (LPSERIUSBLOCK)GetObject(blockIDs);
				pUidArray = (LPSERUID)pArrayBlock->ResizeBlock(sizeof(SERUID));
				*pUidArray = GetUID(pBlock);
				}
			}
		prefixPtr = (LPRECORDPREFIX)pBlock->
		 ResizeBlock(recordSize + sizeof(RECORDPREFIX));
		recPtr = prefixPtr + 1;
		prefixPtr->UID =
     SYSTEM::GetFileObject()->GetUIDTable()->GenerateUID(recPtr, type);
		prefixPtr->recBlockUID = GetUID(this);
		UpdateUIDTable(pBlock, (U_PTRMATH_T)(recordSize+sizeof(RECORDPREFIX)));
		count += 1;
#if (DEBUG_FLAG == 1)
		}
	else
		{
		MessageBox(NULL, "New record size not equal to current size.",
			"RECORDBLOCK::AddRecord", MB_OK);
#endif
		}

	return recPtr;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	AddPreallocatedRecord
//
// CLASS.......:	RECORDBLOCK (public)
//
// DESCRIPTION.:  Deletes a single record from a block that contains fixed
//
// PARAMETERS..:	objPtr - a pointer to the record to be deleted.
//
// RETURN VALUE:	Returns TRUE if the record is deleted. Otherwise, FALSE
//								is returned.
//
// ASSUMPTIONS.:	Assumes that the block holds fixed length records.  Assumes
//                that all allocated space will be used before any records
//                are deleted.  When the space for the last preallocated
//                record is used, the RB_PREALLOCATED bit is cleared so this
//                routine will not be used again for adding records to this
//                record block.
//
/////////////////////////////////////////////////////////////////////////////
void far*	RECORDBLOCK::AddPreallocatedRecord(
	size_t	size)
	{
	U_PTRMATH_T			blockIndex;
	LPSERIUSBLOCK		pBlock;
	LPRECORDPREFIX	prefixPtr;
	SERUID					tempuid;
	LPSERUID				pUidArray;
  U_PTRMATH_T     grossRecSize;
	void far*				recPtr = 0;
  U_PTRMATH_T     byteOffset;

	if ((size != 0) && (size == recordSize) && (this->blockCount > 0))
		{
    grossRecSize = (U_PTRMATH_T)(this->recordSize+sizeof(RECORDPREFIX));
    blockIndex =
     (U_PTRMATH_T)((this->count*grossRecSize) / SEGMENT_SIZE);
    byteOffset =
     (U_PTRMATH_T)((this->count*grossRecSize) % SEGMENT_SIZE);
    if (blockIndex < this->blockCount)
      {
      if (this->blockCount == 1)
        pBlock = (LPSERIUSBLOCK)GetObject(this->blockIDs);
      else
        {
        pUidArray =
         (LPSERUID)((LPSERIUSBLOCK)GetObject(this->blockIDs))->GetData();
        pBlock = (LPSERIUSBLOCK)GetObject(pUidArray[blockIndex]);
        }

      if (pBlock && (pBlock->GetByteCount() >= byteOffset+grossRecSize))
        {
        this->count++;
        prefixPtr =
         (LPRECORDPREFIX)((char far*)pBlock->GetData() + byteOffset) ;
        recPtr = prefixPtr + 1;
		    prefixPtr->UID =
         SYSTEM::GetFileObject()->GetUIDTable()->GenerateUID(recPtr, type);
		    prefixPtr->recBlockUID = GetUID(this);
        SYSTEM::GetFileObject()->GetUIDTable()->
         SetPointer(prefixPtr->UID, recPtr);
        if ((pBlock->GetByteCount() == byteOffset+grossRecSize)  &&
            (blockIndex == this->blockCount-1))
          this->flags &= (~RB_PREALLOCATED);
        }
      }
    }

	return recPtr;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	InitUIDTable
//
// CLASS.......:	SERIUSBLOCK
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
void RECORDBLOCK::InitUIDTable(
	void)
	{
	LPSERIUSBLOCK				pBlock;
	int									blockIndex;
	U_PTRMATH_T					size;
	int 								i;
	U_PTRMATH_T					count;
	LPRECORDPREFIX			objPtr;
	UIDTABLE far*				uidTable;
	LPSERUID						pUidArray;

	uidTable = SYSTEM::GetFileObject()->GetUIDTable();
	size = (U_PTRMATH_T)(recordSize + sizeof(RECORDPREFIX));

	if (blockCount == 1)
		{
		pBlock = (LPSERIUSBLOCK)GetObject(blockIDs);
		count = (U_PTRMATH_T)(pBlock->GetByteCount()/size);
		objPtr = (LPRECORDPREFIX)pBlock->GetData();
		for (i=0; i<count; i++)
			{
			uidTable->InitPointer(objPtr->UID, objPtr+1, type);
			objPtr = (LPRECORDPREFIX)((LPSTR)objPtr + size);
			}
		CallConstructor(pBlock);
		}
	else if (blockCount > 1)
		{
		pUidArray = (LPSERUID)((LPSERIUSBLOCK)GetObject(blockIDs))->GetData();
		for (blockIndex=0; blockIndex<blockCount; blockIndex++)
			{
			pBlock = (LPSERIUSBLOCK)GetObject(pUidArray[blockIndex]);
			count = (U_PTRMATH_T)(pBlock->GetByteCount()/size);
			objPtr = (LPRECORDPREFIX)pBlock->GetData();
			for (i=0; i<count; i++)
				{
				uidTable->InitPointer(objPtr->UID, objPtr+1, type);
				objPtr = (LPRECORDPREFIX)((LPSTR)objPtr + size);
				}
			CallConstructor(pBlock);
			}
		}
  else
    {
    //since NULLSERUID change from -1 to 0, make sure blockIDs is 0
    if (blockIDs == -1)
      blockIDs = NULLSERUID;
    }

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	PurgeBlock
//
// CLASS.......:	RECORDBLOCK
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:  The this pointer may become invalid after the first call
//                to CallDestructor.
/////////////////////////////////////////////////////////////////////////////
void RECORDBLOCK::PurgeBlock(
	void)
	{
	LPSERIUSBLOCK				pBlock;
	int									blockIndex;
	U_PTRMATH_T					size;
	int 								i;
  long                numRecsInBlock;
	LPRECORDPREFIX			objPtr;
	LPSERUID						pUidArray;
  short               recType;

  this->flags |= RB_PURGE;

	size = (U_PTRMATH_T)(recordSize + sizeof(RECORDPREFIX));

  recType = this->type;

  //

	if (blockCount == 1)
		{
		pBlock = (LPSERIUSBLOCK)GetObject(blockIDs);
		objPtr = (LPRECORDPREFIX)pBlock->GetData()+1;
		for (i=0; i<this->count; i++)
			{
			CallDestructor(objPtr, recType);
			objPtr = (LPRECORDPREFIX)((LPSTR)objPtr + size);
			}
		}
	else if (blockCount > 1)
		{
		pUidArray = (LPSERUID)((LPSERIUSBLOCK)GetObject(blockIDs))->GetData();
		for (blockIndex=0; blockIndex<blockCount; blockIndex++)
			{
			pBlock = (LPSERIUSBLOCK)GetObject(pUidArray[blockIndex]);
			numRecsInBlock = pBlock->GetByteCount()/size;
			objPtr = (LPRECORDPREFIX)pBlock->GetData() + 1;
			for (i=0; i < numRecsInBlock; i++)
				{
				CallDestructor(objPtr, recType);
				objPtr = (LPRECORDPREFIX)((LPSTR)objPtr + size);
        this->count =- 1;
        if (this->count == 0)
          return;
				}
			}
		}

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	UpdateUIDTable
//
// CLASS.......:	SERIUSBLOCK
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
void RECORDBLOCK::UpdateUIDTable(
	SERIUSBLOCK far*	theBlock,
	size_t						size)
	{
	long int 						i;
	long int						count;
	RECORDPREFIX far*		objPtr;
	UIDTABLE far*				uidTable;

	count = theBlock->GetByteCount()/size;
	objPtr = (LPRECORDPREFIX)theBlock->GetData();
	uidTable = SYSTEM::GetFileObject()->GetUIDTable();

	for (i=0; i<count; i++)
		{
		uidTable->SetPointer(objPtr->UID, objPtr+1);
		objPtr = (LPRECORDPREFIX)((LPSTR)objPtr + size);
		}

	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	DeleteRecord
//
// CLASS.......:	RECORDBLOCK (public)
//
// DESCRIPTION.:  Deletes a single record from a block that contains fixed
//								length records.
//
// PARAMETERS..:	objPtr - a pointer to the record to be deleted.
//
//								size - the size of the record to be deleted.  If the value
//									of size is not equal to the recordSize stored in the
//									block the record will not be deleted.
//
// RETURN VALUE:	Returns TRUE if the record is deleted. Otherwise, FALSE
//								is returned.
//
// ASSUMPTIONS.:	Assumes that the block holds fixed length records.  In
//								other words, recordSize must be non-zero.
//
// SIDE-EFFECTS:	The block's data is moved to a new location in memory. This
//								invalidates any externally saved pointers to the block's
//								data.
/////////////////////////////////////////////////////////////////////////////

BOOL RECORDBLOCK::DeleteRecord(
	void far*	objPtr,
	size_t		size)
	{
	RECORDPREFIX far*	prefix;
	SERIUSBLOCK far*  pBlock;
	int								i;
	void far*					theData;
	WORD							recSeg;
	WORD							recOff;
	BOOL							result = TRUE;
	LPSERUID					pUidArray;
	LPSERIUSBLOCK			pArrayBlock;

 	prefix = (RECORDPREFIX far*)objPtr - 1;

  //if this block is being purged from memory, don't actually remove records
  //from memory,  This flag is set in purge block.  This feature was added
  //to increase performance when deleting all the records in a block.

  if (this->flags & RB_PURGE)
    {
		SYSTEM::GetFileObject()->GetUIDTable()->ReleaseUID(prefix->UID);
    return TRUE;
    }

	recSeg = FP_SEG(prefix);
	recOff = FP_OFF(prefix);

	if (blockIDs == NULLSERUID)
		{
#if (DEBUG_FLAG == 1)
		MessageBox(NULL, "Error: Record block has a null blockID.",
		 "RECORDBLOCK::DeleteRecord()", MB_OK);
#endif
		result = FALSE;
		}
	else if (blockCount == 1)
		{
		pBlock = (LPSERIUSBLOCK)GetObject(blockIDs);
		theData = pBlock->GetData();
		if ((FP_SEG(theData) != recSeg) ||
				(recOff < FP_OFF(theData)) ||
				(recOff > FP_OFF(theData)+pBlock->GetByteCount()-size))
			{
#if (DEBUG_FLAG == 1)
			MessageBox(NULL, "Error: Record block has a null blockID.",
			 "RECORDBLOCK::DeleteRecord()", MB_OK);
#endif
			result = FALSE;
			}

		}
	else
		{
		pArrayBlock = (LPSERIUSBLOCK)GetObject(blockIDs);
		pUidArray = (LPSERUID)pArrayBlock->GetData();
		for (i=0; i<blockCount; i++)
			{
			pBlock = (LPSERIUSBLOCK)GetObject(pUidArray[i]);
			theData = pBlock->GetData();
			if ((FP_SEG(theData) == recSeg)  &&
					(recOff >= FP_OFF(theData)) &&
					(recOff < FP_OFF(theData)+pBlock->GetByteCount()))
				break;
			}
		if (i == blockCount)
			{
#if (DEBUG_FLAG == 1)
			MessageBox(NULL, "Error: record not contained within blocks.",
			 "RECORDBLOCK::DeleteRecord()", MB_OK);
#endif
			result = FALSE;
			}
		}

	if (result)
		{
		SYSTEM::GetFileObject()->GetUIDTable()->ReleaseUID(prefix->UID);
		pBlock->DeleteSubBlock(recOff - FP_OFF(theData) + 1,
		 size+sizeof(RECORDPREFIX));
		count -= 1;
		UpdateUIDTable(pBlock, size+sizeof(RECORDPREFIX));
		if ((blockCount > 1) && (pBlock->GetByteCount() == 0))
			{
			if (type == RT_SERIUSBLOCK)
				delete (LPSERIUSBLOCKBLOCK)pBlock;
			else
				delete pBlock;
      //refresh the pointer to the array block - required after the delete
      pArrayBlock = (LPSERIUSBLOCK)GetObject(blockIDs);
			pArrayBlock->DeleteSubBlock(i*sizeof(SERUID)+1, sizeof(SERUID));
			blockCount -= 1;
			if (blockCount == 1)
				{
				pUidArray = (LPSERUID)pArrayBlock->GetData();
				blockIDs = pUidArray[0];
				if (type == RT_SERIUSBLOCK)
					delete (LPSERIUSBLOCKBLOCK)pArrayBlock;
				else
					delete pArrayBlock;
				}
			}
		}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	CallConstructor
//
// CLASS.......:	RECORDBLOCK
//
// DESCRIPTION.:  This is the constructor is used to create a serious block
//								read from a file.
//
// PARAMETERS..:  count - the number of records/bytes of storage to allocate
//									with the new block.
//
//								recordSize - the size in bytes of the records to be stored
//									in the serius block.  If recordSize is zero, count is
//									the number of bytes to be stored in the block, otherwise
//									count is the number of records to be stored in the block.
//
// RETURN VALUE:  returns a pointer to the serius block record.
//
/////////////////////////////////////////////////////////////////////////////
void RECORDBLOCK::CallConstructor(
	LPSERIUSBLOCK		theBlock)
	{
	U_PTRMATH_T			i;
	U_PTRMATH_T			recordCount;
	LPRECORDPREFIX  objPtr;
	LPSTR		   			theData;
	LPUIDTABLE			uidTable;

	recordCount = (U_PTRMATH_T)(theBlock->GetByteCount()/(recordSize+sizeof(RECORDPREFIX)));
	theData = (LPSTR)theBlock->GetData();
	uidTable = SYSTEM::GetFileObject()->GetUIDTable();

	for (i=0; i<recordCount; i++)
		{
		objPtr = (LPRECORDPREFIX)(theData + (U_PTRMATH_T)(i*(recordSize+sizeof(RECORDPREFIX))));
		uidTable->SetPointer(objPtr->UID, objPtr+1);
		objPtr++;
		switch (type)
			{
			case RT_CFGDESC:
				new(objPtr) CFGDESC(objPtr);
				break;

			case RT_OBJDESC:
				new(objPtr) OBJDESC(objPtr);
				break;

			case RT_FUNCDESC:
				new(objPtr) FUNCDESC(objPtr);
				break;

			case RT_EVENTDESC:
				new(objPtr) EVENTDESC(objPtr);
				break;

			case RT_METHODDESC:
				new(objPtr) METHODDESC(objPtr);
				break;

			default:
				break;
			}
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	CallDestructor
//
// CLASS.......:	RECORDBLOCK
//
//
/////////////////////////////////////////////////////////////////////////////
void RECORDBLOCK::CallDestructor(
	void far* objPtr,
  short     recType)
	{

	switch (recType)
		{

		case RT_CFGDESC:
			delete (LPCFGDESC)objPtr;
			break;

		case RT_OBJDESC:
			delete (LPOBJDESC)objPtr;
			break;

		case RT_FUNCDESC:
			delete (LPFUNCDESC)objPtr;
			break;

		case RT_EVENTDESC:
			delete (LPEVENTDESC)objPtr;
			break;

		case RT_METHODDESC:
			delete (LPMETHODDESC)objPtr;
			break;

		default:
			break;
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	operator []
//
// CLASS.......:	RECORDBLOCK
//
// DESCRIPTION.:	This function overides the array subscripting operator.
//								it allows you to treat the a block as an array of
//								fixed length records.  The operator returns a pointer to
//								the record specified by index.  The first record has index
//								zero.  The function may crash if index >= count.  If
//								recordSize is zero, this function will always produce a
//								pointer to the first byte of the block's data, regardless
//								of the value of index.
//
// RETURN VALUE:	a pointer to the indicated record.
//
/////////////////////////////////////////////////////////////////////////////

void far * RECORDBLOCK::operator[](
	long int index)
	{
	SERIUSBLOCK far*  theBlock;
	int								i;
	void far*					theData = NULL;
	long int					recordOffset;
	LPSERUID					pUidArray;

	if ((index < count) && (index >= 0) && (blockIDs != NULLSERUID))
		{
		recordOffset = index * (recordSize + sizeof(RECORDPREFIX));
		if (blockCount == 1)
			{
			theBlock = (LPSERIUSBLOCK)GetObject(blockIDs);
			if (theBlock->GetByteCount() > recordOffset)
				{
				theData = theBlock->GetData();
				theData = (LPRECORDPREFIX)((LPSTR)theData + (U_PTRMATH_T)(recordOffset))+1;
				}
			}
		else
			{
			pUidArray = (LPSERUID)((LPSERIUSBLOCK)GetObject(blockIDs))->GetData();
			for (i=0; i<blockCount; i++)
				{
				theBlock = (LPSERIUSBLOCK)GetObject(pUidArray[i]);
				if (theBlock->GetByteCount() > recordOffset)
					{
					theData = theBlock->GetData();
					theData = (LPRECORDPREFIX)((LPSTR)theData + (U_PTRMATH_T)(recordOffset))+1;
					break;
					}
				else
					recordOffset -= theBlock->GetByteCount();
				}
			}
		}

	return theData;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	GetRecordSize
//
// CLASS.......:	RECORDBLOCK
//
// DESCRIPTION.:	Returns the size of the records in the block.
//
// RETURN VALUE:	The size of the records in the block.
//
/////////////////////////////////////////////////////////////////////////////
long int RECORDBLOCK::GetRecordSize(
	void)
	{

	return recordSize;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GetCount
//
// CLASS.......:	RECORDBLOCK
//
// DESCRIPTION.:	Returns a count of the number of bytes/records in the
//				 				block
//
// RETURN VALUE:  The number of bytes/records in the block
//
/////////////////////////////////////////////////////////////////////////////
long int RECORDBLOCK::GetCount(
	void)
	{

	return count;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  NextRec
//
// CLASS.......:	RECORDBLOCK
//
// DESCRIPTION.:	Writes the contents of a serius block to the indicated
//								file.  The data is written starting at the current file
//								position.
//
// RETURN VALUE:
//
/////////////////////////////////////////////////////////////////////////////
void far*	RECORDBLOCK::NextRec(
	void far*		currentRec)
	{
	void far*					nextRec = 0;
	SERIUSBLOCK far*  theBlock;
	int								i;
	int								j;
	void far*					theData;
	long							recSeg;
	long							recOff;
	LPSERUID					pUidArray;

	if (currentRec == NULL)
		{
		if (blockCount == 1)
			{
      theBlock = (LPSERIUSBLOCK)GetObject(blockIDs);
      theData = theBlock->GetData();
			if ((0 != theData) && (sizeof(RECORDPREFIX) < theBlock->GetByteCount()))
				nextRec = (LPRECORDPREFIX)theData + 1;
			}
		else if (blockCount > 1)
			{
			pUidArray = (LPSERUID)((LPSERIUSBLOCK)GetObject(blockIDs))->GetData();
			for (i=0; i<blockCount; i++)
        {
        theBlock = (LPSERIUSBLOCK)GetObject(pUidArray[i]);
        theData = theBlock->GetData();
				if ((theData != 0) && (sizeof(RECORDPREFIX) < theBlock->GetByteCount()))
          {
          nextRec = (LPRECORDPREFIX)theData + 1;
					break;
          }
        }
			}
		}
	else
		{
		if (blockCount == 1)
			{
			theBlock = (LPSERIUSBLOCK)GetObject(blockIDs);
			theData = theBlock->GetData();
			if (((long)FP_OFF(currentRec)+(recordSize+sizeof(RECORDPREFIX))) <=
					FP_OFF(theData)+theBlock->GetByteCount()-recordSize)
			  nextRec = (LPSTR)currentRec + ((U_PTRMATH_T)(recordSize+sizeof(RECORDPREFIX)));
			}
		else
			{
			recSeg = FP_SEG((LPRECORDPREFIX)currentRec - 1);
			recOff = FP_OFF((LPRECORDPREFIX)currentRec - 1);
			pUidArray = (LPSERUID)((LPSERIUSBLOCK)GetObject(blockIDs))->GetData();
			for (i=0; i<blockCount; i++)
				{
				theBlock = (LPSERIUSBLOCK)GetObject(pUidArray[i]);
				theData = theBlock->GetData();
				if ((FP_SEG(theData) == recSeg) &&
						(recOff >= FP_OFF(theData)) &&
						(recOff <=
							(long)FP_OFF(theData)+theBlock->GetByteCount()-recordSize))
					{
			    if (((long)FP_OFF(currentRec)+(recordSize+sizeof(RECORDPREFIX))) <=
					    FP_OFF(theData)+theBlock->GetByteCount()-recordSize)
			      nextRec = (LPSTR)currentRec + ((U_PTRMATH_T)(recordSize+sizeof(RECORDPREFIX)));
					else
						{
            theData = 0;
						for (j=i+1; j<blockCount; j++)
              {
              theData = ((LPSERIUSBLOCK)GetObject(pUidArray[j]))->GetData();
							if (theData != 0)
								break;
              }
						if (theData && (sizeof(RECORDPREFIX) < theBlock->GetByteCount()))
							nextRec = (LPRECORDPREFIX)theData + 1;
						else
							nextRec = NULL;
						}
					break;
					}
				}
			}
		}

	return nextRec;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: pNextRec
//
// CLASS.......:	RECORDBLOCK
//
// DESCRIPTION.:	Writes the contents of a serius block to the indicated
//								file.  The data is written starting at the current file
//								position.
//
// RETURN VALUE:
//
/////////////////////////////////////////////////////////////////////////////
void far*	RECORDBLOCK::pNextRec(
	void far*		currentRec)
	{
	void far*					nextRec = 0;
	SERIUSBLOCK far*  theBlock;
	int								i;
	int								j;
	void far*					theData;
	WORD							recSeg;
	WORD							recOff;
	LPSERUID					pUidArray;

	if (currentRec == NULL)
		{
		if (blockCount == 1)
			{
			if (0 != (theData = ((LPSERIUSBLOCK)blockIDs.ptr)->GetData()))
				nextRec = (LPRECORDPREFIX)theData + 1;
			}
		else if (blockCount > 1)
			{
			pUidArray = (LPSERUID)((LPSERIUSBLOCK)blockIDs.ptr)->GetData();
			for (i=0; i<blockCount; i++)
				if (0 != (theData = ((LPSERIUSBLOCK)pUidArray[i].ptr)->GetData()))
					break;
			if (theData)
				nextRec = (LPRECORDPREFIX)theData + 1;
			}
		}
	else
		{
		if (blockCount == 1)
			{
			theBlock = (LPSERIUSBLOCK)blockIDs.ptr;
			theData = theBlock->GetData();
			if (((long)FP_OFF(currentRec)+(recordSize+sizeof(RECORDPREFIX))) <=
					FP_OFF(theData)+theBlock->GetByteCount()-recordSize)
			  nextRec = (LPSTR)currentRec + ((U_PTRMATH_T)(recordSize+sizeof(RECORDPREFIX)));
			}
		else
			{
			recSeg = FP_SEG((LPRECORDPREFIX)currentRec - 1);
			recOff = FP_OFF((LPRECORDPREFIX)currentRec - 1);
			pUidArray = (LPSERUID)((LPSERIUSBLOCK)blockIDs.ptr)->GetData();
			for (i=0; i<blockCount; i++)
				{
				theBlock = (LPSERIUSBLOCK)pUidArray[i].ptr;
				theData = theBlock->GetData();
				if ((FP_SEG(theData) == recSeg) &&
						(recOff >= FP_OFF(theData)) &&
						(recOff <=
							FP_OFF(theData)+theBlock->GetByteCount()-recordSize))
					{
			    if (((long)FP_OFF(currentRec)+(recordSize+sizeof(RECORDPREFIX))) <=
					    FP_OFF(theData)+theBlock->GetByteCount()-recordSize)
			      nextRec = (LPSTR)currentRec + ((U_PTRMATH_T)(recordSize+sizeof(RECORDPREFIX)));
					else
						{
            theData = 0;
						for (j=i+1; j<blockCount; j++)
							if (0 != (theData = ((LPSERIUSBLOCK)pUidArray[j].ptr)->GetData()))
								break;
						if (theData)
							nextRec = (LPRECORDPREFIX)theData + 1;
						else
							nextRec = NULL;
						}
					break;
					}
				}
			}
		}

	return nextRec;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	Free
//
// CLASS.......:	RECORDBLOCK
//
// DESCRIPTION.:	Writes the contents of a serius block to the indicated
//								file.  The data is written starting at the current file
//								position.
//
// RETURN VALUE:
//
/////////////////////////////////////////////////////////////////////////////
void RECORDBLOCK::Free(
	void)
	{
	int 						blockIndex;
	LPSERIUSBLOCK		pBlock;
	LPSERUID				arrayPtr;

	if (blockIDs != NULLSERUID)
		{
		pBlock = (LPSERIUSBLOCK)GetObject(blockIDs);
		if (blockCount > 1)
			{
			arrayPtr = (LPSERUID)pBlock->GetData();
			for (blockIndex=0; blockIndex<blockCount; blockIndex++)
				delete (LPSERIUSBLOCK)GetObject(arrayPtr[blockIndex]);
			}
		delete pBlock;
		}

	count = 0;
	blockCount = 0;
	blockIDs = NULLSERUID;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ConvertToPointers
//
// CLASS.......:	RECORDBLOCK
//
// DESCRIPTION.:	Writes the contents of a serius block to the indicated
//								file.  The data is written starting at the current file
//								position.
//
// RETURN VALUE:
//
/////////////////////////////////////////////////////////////////////////////
void RECORDBLOCK::ConvertToPointers(
	void)
	{
	LPSERUID	pUidArray;
	int				i;

	if (blockIDs == NULLSERUID)
		blockIDs.ptr = NULL;
	else
		{
		blockIDs.ptr = GetObject(blockIDs);
		if (blockCount > 1)
			{
			pUidArray = (LPSERUID)((LPSERIUSBLOCK)blockIDs.ptr)->GetData();
			for (i=0; i<blockCount; i++)
				pUidArray[i].ptr = GetObject(pUidArray[i]);
			}
		}

	}