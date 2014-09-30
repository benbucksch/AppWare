#include "almsys.h"

#include "fastrecm.h"
#include "exserblk.h"
#include "sereclib.h"
#include "switches.h"


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: FASTRECMGR default constructor
//
// CLASS.......: FASTRECMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
FASTRECMGR::FASTRECMGR(
	void)
	{

	freeList = 1;    //don't use the zero id
	blockCount = 0;
	blockTableUID = ::GetUID(new EXESERIUSBLOCK(0));
  blockInfoArray = NULL;
	blockSize = DEFBLOCKSIZE;
  recordSize = 0;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: FASTRECMGR constructor
//
// CLASS.......: FASTRECMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
FASTRECMGR::FASTRECMGR(
	WORD  blockRecordCount,
  long  recSize)
	{

	freeList = 1;             //don't use the zero id
	blockCount = 0;

  //make sure the global GetUID is called instead of FASTRECM.GetUID
	blockTableUID = ::GetUID(new EXESERIUSBLOCK(0));
  blockInfoArray = NULL;
	blockSize = blockRecordCount;
	recordSize = recSize; // number of bytes in a record

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ~FASTRECMGR destructor
//
// CLASS.......: FASTRECMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
FASTRECMGR::~FASTRECMGR(
	void)
	{

	this->FreeBlocks();

  delete (LPEXESERIUSBLOCK)::GetObject(this->blockTableUID);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	AllocateRecord
//
// CLASS.......:  FASTRECMGR
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
void * FASTRECMGR::AllocateRecord(
  void)
	{
	UINT       blockIndex;
	UINT	      recordByteOffset;
  long int          recUID;
  LPPREFIX          pRecord;

  recUID = this->freeList;

	blockIndex = (UINT)(recUID/blockSize);
	recordByteOffset = (UINT)(recUID%blockSize * (recordSize+sizeof(PREFIX)));

	if (blockIndex >= this->blockCount)
    AllocateTable();

  pRecord = (LPPREFIX)(blockInfoArray[blockIndex].pData + recordByteOffset);

	if (pRecord->nextFree == 0)
	  this->freeList += 1;
  else
	  this->freeList = pRecord->nextFree;

	pRecord->flags |= FLAG_USED;
  pRecord->uid = -recUID;
  pRecord->pRecMgr = this;

	return (pRecord + 1);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	AllocateTable
//
// CLASS.......:  FASTRECMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void FASTRECMGR::AllocateTable(
  void)
  {
  LPEXESERIUSBLOCK  blockTable;
  LPEXESERIUSBLOCK  newBlock;
  char  *        pData;
  WORD              i;

  blockTable = (LPEXESERIUSBLOCK)::GetObject(blockTableUID);

  blockTable->ResizeBlock(sizeof(BLOCKINFO));

	this->blockCount += 1;

  blockInfoArray = (LPBLOCKINFO)blockTable->GetData();

  newBlock =
   new EXESERIUSBLOCK(blockSize * (recordSize+sizeof(PREFIX)));

  blockInfoArray[(UINT)(blockCount - 1)].blockUID = ::GetUID(newBlock);
  blockInfoArray[(UINT)(blockCount - 1)].pData = pData = (LPSTR)newBlock->GetData();

  // initialize the record prefixes in the block

  for (i=0; i<blockSize; i++)
    {
    ((LPPREFIX)(pData + (i*(recordSize+sizeof(PREFIX)))))->
     flags = 0;
    ((LPPREFIX)(pData + (i*(recordSize+sizeof(PREFIX)))))->
     nextFree = 0;
    }

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	FreeBlocks
//
// CLASS.......:  FASTRECMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void FASTRECMGR::FreeBlocks(
  void)
  {
  LPEXESERIUSBLOCK  blockTable;
  LPBLOCKINFO       blockInfoArray;
  WORD              i;

  blockTable = (LPEXESERIUSBLOCK)::GetObject(this->blockTableUID);

  blockInfoArray = (LPBLOCKINFO)blockTable->GetData();

  for (i=0; i<this->blockCount; i++)
     delete (LPEXESERIUSBLOCK)::GetObject(blockInfoArray[i].blockUID);

  // resize the block table to zero
  blockTable = (LPEXESERIUSBLOCK)::GetObject(this->blockTableUID);
  blockTable->ResizeBlock(-blockCount*sizeof(BLOCKINFO));

  this->blockCount = 0;

 }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	FreeRecord
//
// CLASS.......:  FASTRECMGR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL FASTRECMGR::FreeRecord(
	void  *  pRecord)
	{
	LPPREFIX  pPrefix;

  if (pRecord == 0)
    return TRUE;

  pPrefix = (LPPREFIX)pRecord - 1;

  if (((-pPrefix->uid) <= 0) ||
      ((-pPrefix->uid) > blockCount*blockSize) ||
      (!(pPrefix->flags & FLAG_USED)))
    {
#ifdef DEBUG
		MessageBox(NULL, "Uid out of range in call to FASTRECMGR::FreeRecord",
		 "Yo, Programmer Dude!", MB_OK|MB_TASKMODAL);
    DebugBreak();
#endif
		return FALSE;
    }

  pPrefix->nextFree =  pPrefix->pRecMgr->freeList;
  pPrefix->pRecMgr->freeList = -pPrefix->uid;
  pPrefix->flags &= (~FLAG_USED);

	return TRUE;

	}


/////////////////////////////////////////////////////////////
// FUNCTION:			FASTRECMGR::GetNextUsedRecord
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
LPSTR FASTRECMGR::GetNextUsedRecord(
	LPSTR prevRecord,
	long  &blockKey)
	{
	LPSTR record = NULL;
	LPPREFIX recPrefix=NULL;
	LPSTR    pBlockData;
	WORD blockIndex;
	WORD recordIndex;

	if(blockCount > 0)
		{
		//first set recPrefix to point to the first record to examine
		if(!prevRecord)
			{
			//initialize to point to the second record in first the block.
			//the first record is unused - see comment in the constructor
			blockKey = 0;
			blockIndex = 0;
			recordIndex = 1;
			}
		else
			{
			blockIndex = (WORD)(blockKey / (long)blockSize);
			recordIndex = (WORD)(blockKey % (long)blockSize);
			}

		//now loop through all blocks until we reach the end or a used record
		//is found. //recPrefix will point to the used record
		while(!recPrefix && (blockIndex < blockCount))
			{
			//blockTableUID is the uid of a seriusblock which contains an array of
			//blockInfo records. 
			pBlockData = ((LPBLOCKINFO)((LPEXESERIUSBLOCK)
				::GetObject(this->blockTableUID))->GetData())[blockIndex].pData;			

			while(recordIndex < blockSize)
				{
				recPrefix = (LPPREFIX)(pBlockData + 
					(recordIndex * (recordSize + sizeof(PREFIX))));

				recordIndex++; //set up for next time 
				
				if(recPrefix->flags & FLAG_USED)
					break;
				else
					recPrefix = NULL;

				}	//end inner while

			if(!recPrefix)
				{
				//set up too look at the next block
				recordIndex = 0;
				blockIndex++;
				}
			}	//end outer while

		if(recPrefix)
			{
			record = (LPSTR)recPrefix;
			record += sizeof(PREFIX);
			}

		blockKey = (long)((long)blockIndex * (long)blockSize) + (long)recordIndex;
		}

	return record;
	}


