#include <mem.h>
#include <dos.h>
#include "almsys.h"
#include "intalm.h"
#include "baserecb.h"
#include "serdef.h"
#include "debug.h"
#include "switches.h"
#include "ememstr.h"
#include "serblk.h"
#include "exserblk.h"
#include "system.h"
#include "sereclib.h"
#include "objdesc.h"
#include "funcdesc.h"
#include "methdesc.h"
#include "evntdesc.h"
#include "cfgdesc.h"
#include "rectypes.h"
#include "exeflow.h"
#include "objgroup.h"
#include "rtrec.h"
#include "rtreclib.h"

#ifdef __OIP__ // __SHELL__

#include "exelistb.h"
#include "exeevntl.h"
#include "exelibl.h"
#include "exeprocl.h"
#include "exebeadl.h"
#include "exerespl.h"
#include "exeobjs.h"
#include "exetypes.h"

#endif

#ifdef __PROGRAMMER__
#include "project.h"
#include "subject.h"
#include "funcinst.h"
#include "comment.h"
#include "flow.h"
#include "param.h"
#include "objinst.h"
#include "objtype.h"
#include "exeobjs.h"
#include "exetypes.h"
#include "exefunc.h"
#include "exehead.h"
#include "exeparam.h"
#include "exelistb.h"
#include "exeevntl.h"
#include "exelibl.h"
#include "exeprocl.h"
#include "exebeadl.h"
#include "exerespl.h"
#include "memblk.h"
#include "clipsjt.h"
#include "clipparm.h"
#include "clipcom.h"
#include "clipfunc.h"
#include "clipobj.h"
#include "clipflow.h"
#include "clipgrup.h"

#endif

#define MIN_BLOCKID_ARRAY_SIZE	8
#define SEGMENT_SIZE	65535L

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	BASERECORDBLOCK (constructor)
//
// CLASS.......:	BASERECORDBLOCK
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
BASERECORDBLOCK::BASERECORDBLOCK(
  LPRUNTIMERECLIST  pRecList,
	long int  	      recordSize,
	long int		      type):
    RUNTIMEREC(pRecList)
	{
	this->recordSize = recordSize;
	this->type = type;
	this->count = 0;
	this->blockCount = 0;
	this->blockIDs = NULLSERUID;
  this->flags = 0;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	BASERECORDBLOCK (constructor)
//
// CLASS.......:	BASERECORDBLOCK
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
BASERECORDBLOCK::BASERECORDBLOCK(
  LPRUNTIMERECLIST  pRecList,
	long int  	      recordSize,
	long int		      type,
  long int          recordCount):
    RUNTIMEREC(pRecList)
	{

	this->recordSize = recordSize;
	this->type = type;
	this->count = 0;

  this->PreAllocate(recordCount);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	BASERECORDBLOCK (constructor)
//
// CLASS.......:	BASERECORDBLOCK
//
// DESCRIPTION.:  This is the constructor used to create a record from disk.
//
// RETURN VALUE:  returns a pointer to the serius block record.
//
/////////////////////////////////////////////////////////////////////////////
BASERECORDBLOCK::BASERECORDBLOCK(
  LPRUNTIMERECLIST        pRecList,
  LPBASERECORDBLOCK_FILE  pFileRec):
    RUNTIMEREC(pRecList, pFileRec->selfID)
  {

  this->type       = pFileRec->type;
  this->flags      = pFileRec->flags;
  this->count      = pFileRec->count;
  this->recordSize = pFileRec->recordSize;
  this->blockCount = pFileRec->blockCount;
  this->blockIDs   = pFileRec->blockIDs;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	PreAllocate
//
// CLASS.......:	BASERECORDBLOCK
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
BOOL BASERECORDBLOCK::PreAllocate(
  long  recordCount)
  {
  int           blockIdx;
  long          lastBlockSize;
  LPBASEBLOCK   pArrayBlock;
  LPRUNTIMEUID  pUIDArray;

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
    if (type < 0)
			this->blockIDs = ::GetUID(new EXESERIUSBLOCK(lastBlockSize));
		else
			this->blockIDs = ::GetUID(new SERIUSBLOCK(lastBlockSize));
    }
  else
    {
    if (type < 0)
			pArrayBlock = new EXESERIUSBLOCK(this->blockCount*sizeof(SERUID));
		else
			pArrayBlock = new SERIUSBLOCK(this->blockCount*sizeof(SERUID));
    this->blockIDs = ::GetUID(pArrayBlock);
    pUIDArray = (LPRUNTIMEUID)pArrayBlock->GetData();
    for (blockIdx = 0; blockIdx < this->blockCount-1; blockIdx++)
      {
      if (type < 0)
			  pUIDArray[blockIdx] = ::GetUID(new EXESERIUSBLOCK(SEGMENT_SIZE));
		  else
			  pUIDArray[blockIdx] = ::GetUID(new SERIUSBLOCK(SEGMENT_SIZE));
      }
    if (type < 0)
			pUIDArray[this->blockCount-1] =
       ::GetUID(new EXESERIUSBLOCK(lastBlockSize));
		else
			pUIDArray[this->blockCount-1] =
       ::GetUID(new SERIUSBLOCK(lastBlockSize));
    }

  return TRUE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	~BASERECORDBLOCK (destructor)
//
// CLASS.......:	BASERECORDBLOCK
//
// DESCRIPTION.:  If the block has allocated any memory, that memory is
//								released.
//
// ASSUMPTIONS.:
//
/////////////////////////////////////////////////////////////////////////////
BASERECORDBLOCK::~BASERECORDBLOCK(
	void)
	{

  this->FreeMemory();

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	FreeMemory
//
// CLASS.......:	BASERECORDBLOCK
//
// DESCRIPTION.:  If the block has allocated any memory, that memory is
//								released.
//
// ASSUMPTIONS.:
//
/////////////////////////////////////////////////////////////////////////////
void BASERECORDBLOCK::FreeMemory(
	void)
	{
	int 					blockIndex;
	LPBASEBLOCK   pBlock;
	LPRUNTIMEUID	arrayPtr;

	if (blockIDs != NULLSERUID)
		{
		pBlock = (LPBASEBLOCK)GetObject(blockIDs);
		if (blockCount > 1)
			{
			arrayPtr = (LPRUNTIMEUID)pBlock->GetData();
			for (blockIndex=0; blockIndex<blockCount; blockIndex++)
			  delete (LPBASEBLOCK)GetObject(arrayPtr[blockIndex]);

      //need to refresh block pointer after deletes above
      pBlock = (LPBASEBLOCK)GetObject(blockIDs);
			}
		delete pBlock;
		}

	count = 0;
	blockCount = 0;
	blockIDs = NULLSERUID;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	AddRecord
//
// CLASS.......:	BASERECORDBLOCK (public)
//
// DESCRIPTION.:  Deletes a single record from a block that contains fixed
//
// PARAMETERS..:	objPtr - a pointer to the record to be deleted.
//
// RETURN VALUE:	Returns TRUE if the record is deleted. Otherwise, FALSE
//								is returned.
//
/////////////////////////////////////////////////////////////////////////////
void far*	BASERECORDBLOCK::AddRecord(
	size_t	size)
	{
	int 						blockIndex;
	LPBASEBLOCK		  pBlock;
	LPBASEBLOCK		  pArrayBlock;
	void far*				recPtr = NULL;
	LPRECORDPREFIX	prefixPtr;
	RUNTIMEUID			tempuid;
	LPRUNTIMEUID		pUidArray;

  if (this->flags & RB_PREALLOCATED)
    return AddPreallocatedRecord(size);

	if ((size != 0) && (size == recordSize))
		{
		if (blockIDs == NULLSERUID)
			{
      if (type < 0)
				pBlock = new EXESERIUSBLOCK();
			else
				pBlock = new SERIUSBLOCK();
			blockIDs = ::GetUID(pBlock);
			blockCount += 1;
			}
		else if (blockCount == 1)
			{
			pBlock = (LPBASEBLOCK)GetObject(blockIDs);
			if (pBlock->GetByteCount()+recordSize+sizeof(RECORDPREFIX) >
					SEGMENT_SIZE)
				{
				tempuid = blockIDs;
        if (type < 0)
				  pArrayBlock = new EXESERIUSBLOCK(2*sizeof(SERUID));
				else
					pArrayBlock = new SERIUSBLOCK(2*sizeof(SERUID));
				blockIDs = ::GetUID(pArrayBlock);
				pUidArray = (LPRUNTIMEUID)pArrayBlock->GetData();
				pUidArray[0] = tempuid;
        if (type < 0)
				  pBlock = new EXESERIUSBLOCK();
				else
					pBlock = new SERIUSBLOCK();
				pUidArray[1] = ::GetUID(pBlock);
				blockCount = 2;
				}
			}
		else
			{
			pArrayBlock = (LPBASEBLOCK)GetObject(blockIDs);
			pUidArray = (LPRUNTIMEUID)pArrayBlock->GetData();
			for (blockIndex=0; blockIndex<blockCount; blockIndex++)
				{
				pBlock = (LPBASEBLOCK)GetObject(pUidArray[blockIndex]);
				if (pBlock->GetByteCount()+recordSize+sizeof(RECORDPREFIX) <
						SEGMENT_SIZE)
					break;
				}
			if (blockIndex == blockCount)
				{
				blockCount++;
        if (type < 0)
				  pBlock = new EXESERIUSBLOCK();
				else
					pBlock = new SERIUSBLOCK();
				pArrayBlock = (LPBASEBLOCK)GetObject(blockIDs);
				pUidArray = (LPRUNTIMEUID)pArrayBlock->ResizeBlock(sizeof(SERUID));
				*pUidArray = ::GetUID(pBlock);
				}
			}
		prefixPtr = (LPRECORDPREFIX)pBlock->
		 ResizeBlock(recordSize + sizeof(RECORDPREFIX));
		recPtr = prefixPtr + 1;
		prefixPtr->UID =
     pTheSystem->GetFileObject()->GetUIDTable()->GenerateUID(recPtr, type);
		prefixPtr->recBlockUID = ::GetUID(this);
		UpdateUIDTable(pBlock, (U_PTRMATH_T)(recordSize+sizeof(RECORDPREFIX)));
		count += 1;
#if (DEBUG_FLAG == 1)
		}
	else
		{
		MessageBox(NULL, "New record size not equal to current size.",
			"BASERECORDBLOCK::AddRecord", MB_OK|MB_TASKMODAL);
#endif
		}

	return recPtr;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	AddPreallocatedRecord
//
// CLASS.......:	BASERECORDBLOCK (public)
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
void far*	BASERECORDBLOCK::AddPreallocatedRecord(
	size_t	size)
	{
	U_PTRMATH_T			blockIndex;
	LPBASEBLOCK		  pBlock;
	LPRECORDPREFIX	prefixPtr;
	SERUID					tempuid;
	LPRUNTIMEUID		pUidArray;
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
        pBlock = (LPBASEBLOCK)GetObject(this->blockIDs);
      else
        {
        pUidArray =
         (LPRUNTIMEUID)((LPBASEBLOCK)GetObject(this->blockIDs))->GetData();
        pBlock = (LPBASEBLOCK)GetObject(pUidArray[blockIndex]);
        }

      if (pBlock && (pBlock->GetByteCount() >= byteOffset+grossRecSize))
        {
        this->count++;
        prefixPtr =
         (LPRECORDPREFIX)((char far*)pBlock->GetData() + byteOffset) ;
        recPtr = prefixPtr + 1;
		    prefixPtr->UID =
         pTheSystem->GetFileObject()->GetUIDTable()->GenerateUID(recPtr, type);
		    prefixPtr->recBlockUID = ::GetUID(this);
        pTheSystem->GetFileObject()->GetUIDTable()->
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
void BASERECORDBLOCK::InitUIDTable(
	void)
	{
	LPBASEBLOCK				pBlock;
	int								blockIndex;
	U_PTRMATH_T				size;
	int 							i;
	U_PTRMATH_T				count;
	LPRECORDPREFIX		objPtr;
	UIDTABLE far*			uidTable;
	LPRUNTIMEUID			pUidArray;

	uidTable = pTheSystem->GetFileObject()->GetUIDTable();
	size = (U_PTRMATH_T)(recordSize + sizeof(RECORDPREFIX));

	if (blockCount == 1)
		{
		pBlock = (LPBASEBLOCK)GetObject(blockIDs);
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
		pUidArray = (LPRUNTIMEUID)((LPBASEBLOCK)GetObject(blockIDs))->GetData();
		for (blockIndex=0; blockIndex<blockCount; blockIndex++)
			{
			pBlock = (LPBASEBLOCK)GetObject(pUidArray[blockIndex]);
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
// CLASS.......:	BASERECORDBLOCK
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
void BASERECORDBLOCK::PurgeBlock(
	void)
	{
	LPBASEBLOCK				pBlock;
	int								blockIndex;
	U_PTRMATH_T				size;
	int 							i;
  long              numRecsInBlock;
	LPRECORDPREFIX		objPtr;
	LPRUNTIMEUID			pUidArray;
  short             recType;

  this->flags |= RB_PURGE;

	size = (U_PTRMATH_T)(recordSize + sizeof(RECORDPREFIX));

  recType = this->type;

  //

	if (blockCount == 1)
		{
		pBlock = (LPBASEBLOCK)GetObject(blockIDs);
		objPtr = (LPRECORDPREFIX)pBlock->GetData()+1;
		for (i=0; i<this->count; i++)
			{
			CallDestructor(objPtr, recType);
			objPtr = (LPRECORDPREFIX)((LPSTR)objPtr + size);
			}
		}
	else if (blockCount > 1)
		{
		pUidArray = (LPRUNTIMEUID)((LPBASEBLOCK)GetObject(blockIDs))->GetData();
		for (blockIndex=0; blockIndex<blockCount; blockIndex++)
			{
			pBlock = (LPBASEBLOCK)GetObject(pUidArray[blockIndex]);
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
void BASERECORDBLOCK::UpdateUIDTable(
	LPBASEBLOCK	 theBlock,
	size_t			 size)
	{
	long int 						i;
	long int						count;
	RECORDPREFIX far*		objPtr;
	UIDTABLE far*				uidTable;

	count = theBlock->GetByteCount()/size;
	objPtr = (LPRECORDPREFIX)theBlock->GetData();
	uidTable = pTheSystem->GetFileObject()->GetUIDTable();

	for (i=0; i<count; i++)
		{
		uidTable->SetPointer(objPtr->UID, objPtr+1);
		objPtr = (LPRECORDPREFIX)((LPSTR)objPtr + size);
		}

	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	DeleteRecord
//
// CLASS.......:	BASERECORDBLOCK (public)
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

BOOL BASERECORDBLOCK::DeleteRecord(
	void far*	objPtr,
	size_t		size)
	{
	LPRECORDPREFIX  	prefix;
	LPBASEBLOCK       pBlock;
	int								i;
	void far*					theData;
	WORD							recSeg;
	WORD							recOff;
	BOOL							result = TRUE;
	LPRUNTIMEUID			pUidArray;
	LPBASEBLOCK			  pArrayBlock;

 	prefix = (RECORDPREFIX far*)objPtr - 1;

  //if this block is being purged from memory, don't actually remove records
  //from memory,  This flag is set in purge block.  This feature was added
  //to increase performance when deleting all the records in a block.

  if (this->flags & RB_PURGE)
    {
		pTheSystem->GetFileObject()->GetUIDTable()->ReleaseUID(prefix->UID);
    return TRUE;
    }

	recSeg = FP_SEG(prefix);
	recOff = FP_OFF(prefix);

	if (blockIDs == NULLSERUID)
		{
#if (DEBUG_FLAG == 1)
		MessageBox(NULL, "Error: Record block has a null blockID.",
		 "BASERECORDBLOCK::DeleteRecord()", MB_OK|MB_TASKMODAL);
#endif
		result = FALSE;
		}
	else if (blockCount == 1)
		{
		pBlock = (LPBASEBLOCK)GetObject(blockIDs);
		theData = pBlock->GetData();
		if ((FP_SEG(theData) != recSeg) ||
				(recOff < FP_OFF(theData)) ||
				(recOff > FP_OFF(theData)+pBlock->GetByteCount()-size))
			{
#if (DEBUG_FLAG == 1)
			MessageBox(NULL, "Error: Record block has a null blockID.",
			 "BASERECORDBLOCK::DeleteRecord()", MB_OK|MB_TASKMODAL);
#endif
			result = FALSE;
			}

		}
	else
		{
		pArrayBlock = (LPBASEBLOCK)GetObject(blockIDs);
		pUidArray = (LPRUNTIMEUID)pArrayBlock->GetData();
		for (i=0; i<blockCount; i++)
			{
			pBlock = (LPBASEBLOCK)GetObject(pUidArray[i]);
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
			 "BASERECORDBLOCK::DeleteRecord()", MB_OK|MB_TASKMODAL);
#endif
			result = FALSE;
			}
		}

	if (result)
		{
		pTheSystem->GetFileObject()->GetUIDTable()->ReleaseUID(prefix->UID);
		pBlock->DeleteSubBlock(recOff - FP_OFF(theData) + 1,
		 size+sizeof(RECORDPREFIX));
		count -= 1;
		UpdateUIDTable(pBlock, size+sizeof(RECORDPREFIX));
		if ((blockCount > 1) && (pBlock->GetByteCount() == 0))
			{
			delete pBlock;
      //refresh the pointer to the array block - required after the delete
      pArrayBlock = (LPBASEBLOCK)GetObject(blockIDs);
			pArrayBlock->DeleteSubBlock(i*sizeof(SERUID)+1, sizeof(SERUID));
			blockCount -= 1;
			if (blockCount == 1)
				{
				pUidArray = (LPRUNTIMEUID)pArrayBlock->GetData();
				blockIDs = pUidArray[0];
				delete pArrayBlock;
				}
			}
		}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	CallConstructor
//
// CLASS.......:	BASERECORDBLOCK
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
void BASERECORDBLOCK::CallConstructor(
	LPBASEBLOCK		theBlock)
	{
	U_PTRMATH_T			i;
	U_PTRMATH_T			recordCount;
	LPRECORDPREFIX  objPtr;
	LPSTR		   			theData;
	LPUIDTABLE			uidTable;

	recordCount = (U_PTRMATH_T)(theBlock->GetByteCount()/(recordSize+sizeof(RECORDPREFIX)));
	theData = (LPSTR)theBlock->GetData();
	uidTable = pTheSystem->GetFileObject()->GetUIDTable();

	for (i=0; i<recordCount; i++)
		{
		objPtr = (LPRECORDPREFIX)(theData + (U_PTRMATH_T)(i*(recordSize+sizeof(RECORDPREFIX))));
		uidTable->SetPointer(objPtr->UID, objPtr+1);
		objPtr++;
		switch (type)
			{
#ifndef __OIP__

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
#endif

#ifdef __OIP__ // __SHELL__

			case RT_EXEBEADLIST:
				new(objPtr) EXEBEADLIST(objPtr);
				break;

			case RT_EXELIBLIST:
				new(objPtr) EXELIBLIST(objPtr);
				break;

			case RT_EXEEVENTLIST:
				new(objPtr) EXEEVENTLIST(objPtr);
				break;

			case RT_EXEPROCLIST:
				new(objPtr) EXEPROCLIST(objPtr);
				break;

			case RT_EXERESPONSELIST:
				new(objPtr) EXERESPONSELIST(objPtr);
				break;

			case RT_EXEOBJINST:
				new(objPtr) EXEOBJINST(objPtr);
				break;

			case RT_EXEOBJTYPE:
				new(objPtr) EXEOBJTYPE(objPtr);
				break;

			case RT_EXEFLOW:
				new(objPtr) EXEFLOW(objPtr);
				break;

#endif

#ifdef __PROGRAMMER__

			case RT_PROJECT:
				new(objPtr) PROJECT(objPtr);
				break;

			case RT_SUBJECT:
				new(objPtr) SUBJECT(objPtr);
				break;

			case RT_OBJGROUP:
				new(objPtr) OBJGROUP(objPtr);
				break;

			case RT_OBJINST:
				new(objPtr) OBJINST(objPtr);
				break;

			case RT_OBJTYPE:
				new(objPtr) OBJTYPE(objPtr);
				break;

			case RT_SUBTYPE:
				new(objPtr) SUBTYPE(objPtr);
        break;

			case RT_FUNCINST:
				new(objPtr) FUNCINST(objPtr);
				break;

			case RT_COMMENT:
				new(objPtr) COMMENT(objPtr);
				break;

			case RT_FLOW:
				new(objPtr) FLOW(objPtr);
				break;

			case RT_PARAM:
				new(objPtr) PARAM(objPtr);
				break;
#endif

			default:
				break;
			}
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	CallDestructor
//
// CLASS.......:	BASERECORDBLOCK
//
//
/////////////////////////////////////////////////////////////////////////////
void BASERECORDBLOCK::CallDestructor(
	void far* objPtr,
  short     recType)
	{

	switch (recType)
		{
#ifndef __OIP__

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
#endif

#ifdef __OIP__ // __SHELL__

		case RT_EXEBEADLIST:
			delete (LPEXEBEADLIST)objPtr;
			break;

		case RT_EXELIBLIST:
			delete (LPEXELIBLIST)objPtr;
			break;

		case RT_EXEEVENTLIST:
			delete (LPEXEEVENTLIST)objPtr;
			break;

		case RT_EXEPROCLIST:
			delete (LPEXEPROCLIST)objPtr;
			break;

		case RT_EXERESPONSELIST:
			delete (LPEXERESPONSELIST)objPtr;
			break;

#endif

#ifdef __PROGRAMMER__

		case RT_PROJECT:
			delete (PROJECT far*)objPtr;
			break;

		case RT_SUBJECT:
			delete (SUBJECT far*)objPtr;
			break;

		case RT_OBJGROUP:
			delete (LPOBJGROUP)objPtr;
			break;

		case RT_OBJINST:
			delete (LPOBJINST)objPtr;
			break;

		case RT_OBJTYPE:
			delete (LPOBJTYPE)objPtr;
			break;

		case RT_FUNCINST:
			delete (FUNCINST far*)objPtr;
			break;

		case RT_COMMENT:
			delete (COMMENT far*)objPtr;
			break;

		case RT_FLOW:
			delete (FLOW far*)objPtr;
			break;

		case RT_PARAM:
			delete (PARAM far*)objPtr;
			break;

    case RT_EXEOBJINST:
      delete (EXEOBJINST far*)objPtr;
			break;

    case RT_EXEOBJTYPE:
      delete (EXEOBJTYPE far*)objPtr;
			break;

    case RT_EXEFUNCINST:
      delete (EXEFUNCINST far*)objPtr;
			break;

    case RT_EXEPARAM:
      delete (EXEPARAM far*)objPtr;
			break;

    case RT_EXEFLOW:
      delete (EXEFLOW far*)objPtr;
			break;

    case RT_EXEHEADREC:
      delete (EXEHEAD far*)objPtr;
			break;

		case RT_EXEBEADLIST:
			delete (LPEXEBEADLIST)objPtr;
			break;

		case RT_EXELIBLIST:
			delete (LPEXELIBLIST)objPtr;
			break;

		case RT_EXEEVENTLIST:
			delete (LPEXEEVENTLIST)objPtr;
			break;

		case RT_EXEPROCLIST:
			delete (LPEXEPROCLIST)objPtr;
			break;

		case RT_EXERESPONSELIST:
			delete (LPEXERESPONSELIST)objPtr;
			break;

    case RT_MEMBLOCK:
			delete (LPMEMBLOCK)objPtr;
			break;

		case RT_SUBTYPE:
			delete (LPSUBTYPE)objPtr;
			break;

    //clipboard types
		case RT_CLIPSJT:
			delete (LPCLIPSJT)objPtr;
			break;

		case RT_CLIPOBJ:
			delete (LPCLIPOBJ)objPtr;
			break;

		case RT_CLIPGROUP:
			delete (LPCLIPGROUP)objPtr;
			break;

		case RT_CLIPFUNC:
			delete (LPCLIPFUNC)objPtr;
			break;

		case RT_CLIPPARAM:
			delete (LPCLIPPARAM)objPtr;
			break;

		case RT_CLIPFLOW:
			delete (LPCLIPFLOW)objPtr;
			break;

		case RT_CLIPCOMMENT:
			delete (LPCLIPCOMMENT)objPtr;
      break;


#endif

		default:
			break;
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	operator []
//
// CLASS.......:	BASERECORDBLOCK
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

void far * BASERECORDBLOCK::operator[](
	long int index)
	{
	LPBASEBLOCK    theBlock;
	int						 i;
	void far*			 theData = NULL;
	long int			 recordOffset;
	LPRUNTIMEUID	 pUidArray;

	if ((index < count) && (index >= 0) && (blockIDs != NULLSERUID))
		{
		recordOffset = index * (recordSize + sizeof(RECORDPREFIX));
		if (blockCount == 1)
			{
			theBlock = (LPBASEBLOCK)GetObject(blockIDs);
			if (theBlock->GetByteCount() > recordOffset)
				{
				theData = theBlock->GetData();
				theData = (LPRECORDPREFIX)((LPSTR)theData + (U_PTRMATH_T)(recordOffset))+1;
				}
			}
		else
			{
			pUidArray = (LPRUNTIMEUID)((LPBASEBLOCK)GetObject(blockIDs))->GetData();
			for (i=0; i<blockCount; i++)
				{
				theBlock = (LPBASEBLOCK)GetObject(pUidArray[i]);
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
// FUNCTION....:  NextRec
//
// CLASS.......:	BASERECORDBLOCK
//
// DESCRIPTION.:	Writes the contents of a serius block to the indicated
//								file.  The data is written starting at the current file
//								position.
//
// RETURN VALUE:
//
/////////////////////////////////////////////////////////////////////////////
void far*	BASERECORDBLOCK::NextRec(
	void far*		currentRec)
	{
	void far*			nextRec = 0;
	LPBASEBLOCK   theBlock;
	int						i;
	int						j;
	void far*			theData;
	long					recSeg;
	long					recOff;
	LPRUNTIMEUID	pUidArray;

	if (currentRec == NULL)
		{
		if (blockCount == 1)
			{
      theBlock = (LPBASEBLOCK)GetObject(blockIDs);
      theData = theBlock->GetData();
			if ((0 != theData) && (sizeof(RECORDPREFIX) < theBlock->GetByteCount()))
				nextRec = (LPRECORDPREFIX)theData + 1;
			}
		else if (blockCount > 1)
			{
			pUidArray = (LPRUNTIMEUID)((LPBASEBLOCK)GetObject(blockIDs))->GetData();
			for (i=0; i<blockCount; i++)
        {
        theBlock = (LPBASEBLOCK)GetObject(pUidArray[i]);
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
			theBlock = (LPBASEBLOCK)GetObject(blockIDs);
			theData = theBlock->GetData();
			if (((long)FP_OFF(currentRec)+(recordSize+sizeof(RECORDPREFIX))) <=
					FP_OFF(theData)+theBlock->GetByteCount()-recordSize)
			  nextRec = (LPSTR)currentRec + ((U_PTRMATH_T)(recordSize+sizeof(RECORDPREFIX)));
			}
		else
			{
			recSeg = FP_SEG((LPRECORDPREFIX)currentRec - 1);
			recOff = FP_OFF((LPRECORDPREFIX)currentRec - 1);
			pUidArray = (LPRUNTIMEUID)((LPBASEBLOCK)GetObject(blockIDs))->GetData();
			for (i=0; i<blockCount; i++)
				{
				theBlock = (LPBASEBLOCK)GetObject(pUidArray[i]);
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
              theData = ((LPBASEBLOCK)GetObject(pUidArray[j]))->GetData();
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
// CLASS.......:	BASERECORDBLOCK
//
// DESCRIPTION.:	Writes the contents of a serius block to the indicated
//								file.  The data is written starting at the current file
//								position.
//
// RETURN VALUE:
//
/////////////////////////////////////////////////////////////////////////////
void far*	BASERECORDBLOCK::pNextRec(
	void far*		currentRec)
	{
	void far*		 nextRec = 0;
	LPBASEBLOCK  theBlock;
	int					 i;
	int					 j;
	void far*		 theData;
	WORD				 recSeg;
	WORD				 recOff;
	LPRUNTIMEUID pUidArray;

	if (currentRec == NULL)
		{
		if (blockCount == 1)
			{
			if (0 != (theData = ((LPBASEBLOCK)blockIDs.ptr)->GetData()))
				nextRec = (LPRECORDPREFIX)theData + 1;
			}
		else if (blockCount > 1)
			{
			pUidArray = (LPRUNTIMEUID)((LPBASEBLOCK)blockIDs.ptr)->GetData();
			for (i=0; i<blockCount; i++)
				if (0 != (theData = ((LPBASEBLOCK)pUidArray[i].ptr)->GetData()))
					break;
			if (theData)
				nextRec = (LPRECORDPREFIX)theData + 1;
			}
		}
	else
		{
		if (blockCount == 1)
			{
			theBlock = (LPBASEBLOCK)blockIDs.ptr;
			theData = theBlock->GetData();
			if (((long)FP_OFF(currentRec)+(recordSize+sizeof(RECORDPREFIX))) <=
					FP_OFF(theData)+theBlock->GetByteCount()-recordSize)
				nextRec = (LPSTR)currentRec + ((U_PTRMATH_T)(recordSize+sizeof(RECORDPREFIX)));
			}
		else
			{
			recSeg = FP_SEG((LPRECORDPREFIX)currentRec - 1);
			recOff = FP_OFF((LPRECORDPREFIX)currentRec - 1);
			pUidArray = (LPRUNTIMEUID)((LPBASEBLOCK)blockIDs.ptr)->GetData();
			for (i=0; i<blockCount; i++)
				{
				theBlock = (LPBASEBLOCK)pUidArray[i].ptr;
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
							if (0 != (theData = ((LPBASEBLOCK)pUidArray[j].ptr)->GetData()))
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
// CLASS.......:	BASERECORDBLOCK
//
// DESCRIPTION.:	Frees the BASEBLOCKS held by the record blocks.
//
/////////////////////////////////////////////////////////////////////////////
void BASERECORDBLOCK::Free(
	void)
	{
	int 					blockIndex;
	LPBASEBLOCK		pBlock;
	LPRUNTIMEUID	arrayPtr;

	if (blockIDs != NULLSERUID)
		{
		pBlock = (LPBASEBLOCK)GetObject(blockIDs);
		if (blockCount > 1)
			{
			arrayPtr = (LPRUNTIMEUID)pBlock->GetData();
			for (blockIndex=0; blockIndex<blockCount; blockIndex++)
				delete (LPBASEBLOCK)GetObject(arrayPtr[blockIndex]);
			}
		delete pBlock;
		}

	count = 0;
	blockCount = 0;
	blockIDs = NULLSERUID;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  WriteReocrdBlockRecords
//
// CLASS.......:  BASERECORDBLOCK
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL BASERECORDBLOCK::WriteRecordBlockRecords(
  int               hFile,
  LPRUNTIMERECLIST  pRecList)
  {
  LPBASERECORDBLOCK     pRec = 0;
  UINT                  writeCount;
  BASERECORDBLOCK_FILE  fileRec;
  BOOL                  result = TRUE;

  while (0 != (pRec=(LPBASERECORDBLOCK)pRecList->GetNextMember(pRec)))
    {
    fileRec.selfID     = pRec->GetUID();
    fileRec.type       = pRec->type;
    fileRec.flags      = pRec->flags;
    fileRec.count      = pRec->count;
    fileRec.recordSize = pRec->recordSize;
    fileRec.blockCount = pRec->blockCount;
    fileRec.blockIDs   = pRec->blockIDs;

  	writeCount =
     _lwrite(hFile, (LPSTR)&fileRec, sizeof(BASERECORDBLOCK_FILE));

    if (writeCount != sizeof(BASERECORDBLOCK_FILE))
      result = FALSE;
    }

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ReadRecordBlockRecords
//
// CLASS.......:  BASEBLOCK
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL BASERECORDBLOCK::ReadRecordBlockRecords(
  int               hFile,
  LPRUNTIMERECLIST  pRecList)
  {
  long                  i;
  UINT                  readCount;
  BASERECORDBLOCK_FILE  fileRec;
  BOOL                  result = TRUE;

  for (i=0; i<pRecList->GetCount(); i++)
    {
  	readCount =
     _lread(hFile, (LPSTR)&fileRec, sizeof(BASERECORDBLOCK_FILE));
    if (readCount != sizeof(BASERECORDBLOCK_FILE))
      result = FALSE;
    else
      {
      new BASERECORDBLOCK(pRecList, &fileRec);
      }
    }

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ConvertToPointers
//
// CLASS.......:	BASERECORDBLOCK
//
// DESCRIPTION.:	Writes the contents of a serius block to the indicated
//								file.  The data is written starting at the current file
//								position.
//
// RETURN VALUE:
//
/////////////////////////////////////////////////////////////////////////////
void BASERECORDBLOCK::ConvertToPointers(
	void)
	{
	LPRUNTIMEUID	pUidArray;
	int				i;

	if (blockIDs == NULLSERUID)
		blockIDs.ptr = NULL;
	else
		{
		blockIDs.ptr = GetObject(blockIDs);
		if (blockCount > 1)
			{
			pUidArray = (LPRUNTIMEUID)((LPBASEBLOCK)blockIDs.ptr)->GetData();
			for (i=0; i<blockCount; i++)
				pUidArray[i].ptr = GetObject(pUidArray[i]);
			}
		}

	}