#include "almsys.h"

#include "datasmgr.h"
#include "serblk.h"
#include "sereclib.h"
#include "rtreclib.h"
#include "intdebug.h"

struct DATAINFO
  {
	RUNTIMEUID  serblkUID;
  short       lockCount;
  };

typedef DATAINFO *  LPDATAINFO;


//----------------------------------------------------------------------
//   DATASMGR constructor
//----------------------------------------------------------------------
DATASMGR::DATASMGR(
  LPRUNTIMERECLIST  pDatasRecList):
		datasUID(0)
  {

  this->pDatasRecList = pDatasRecList;

  this->datasCount = 0;

  }

//----------------------------------------------------------------------
//   DATASMGR constructor
//
//   Copys the src datas which are assumed to be from a design time
//   object and places them in exeserius blocks.
//
//----------------------------------------------------------------------
DATASMGR::DATASMGR(
  LPRUNTIMERECLIST  pDatasRecList,
  DATASMGR *     pSrcDatasMgr)
  {
  LPBASEBLOCK  pSourceIDArrayBlock;
  LPDATAINFO   pSourceIDArray;
  LPBASEBLOCK  pDestIDArrayBlock;
  LPDATAINFO   pDestIDArray;
  LPBASEBLOCK  pSourceBlock;
  LPBASEBLOCK  pDestBlock;
  BYTE*  			 pSourceData;
	BYTE*  			 pDestData;
  SERUID       exeBlocksID;
  long         dataSize;
  short        i;

  this->datasCount = pSrcDatasMgr->datasCount;
  this->pDatasRecList = pDatasRecList;

  // Copy the object's datas

  if (this->datasCount > 0)
		{

		exeBlocksID = GetUID(pTheSystem->GetFileObject()->GetExeSeriusBlocks());

		// Get the source ID array info

		pSourceIDArrayBlock = (LPBASEBLOCK)GetObject(pSrcDatasMgr->datasUID);
		pSourceIDArray = (LPDATAINFO)pSourceIDArrayBlock->GetData();

		// Create a destination ID array block

		pDestIDArrayBlock =
		 new BASEBLOCK(pDatasRecList, pSourceIDArrayBlock->GetByteCount());
		pDestIDArray = (LPDATAINFO)pDestIDArrayBlock->GetData();
		this->datasUID = GetUID(pDestIDArrayBlock);

		// Duplicate the individual datas

		for (i=0; i<datasCount; i++)
			{
			pSourceBlock = (LPBASEBLOCK)GetObject(pSourceIDArray[i].serblkUID);
			dataSize = pSourceBlock->GetByteCount();
			pSourceData = (LPBYTE)pSourceBlock->GetData();
			pDestBlock = new BASEBLOCK(pDatasRecList, dataSize);
			pDestData = (LPBYTE)pDestBlock->GetData();

			if (pSourceData && pDestData)
				{
				CopyMemory(pDestData, pSourceData, dataSize);
				}
        
			pDestIDArray[i].serblkUID = GetUID(pDestBlock);
      pDestIDArray[i].lockCount = 0;
			}
		}

  }


//----------------------------------------------------------------------
//   DATASMGR constructor
//
//   This constructor is called when the object is read from disk. It
//   initializes the lockCount for all datas to 0.
//----------------------------------------------------------------------
DATASMGR::DATASMGR(
	void *         /*objPtr*/,
  LPRUNTIMERECLIST  pDatasRecList)
  {
  LPDATAINFO		pInfoArray;
  short					i;

  this->pDatasRecList = pDatasRecList;

	if (datasCount > 0)
		{
		pInfoArray = (LPDATAINFO)((LPBASEBLOCK)GetObject(datasUID))->GetData();
		for (i=0; i<datasCount; i++)
			{
			pInfoArray[i].lockCount = 0;
			}
		}

  }

//----------------------------------------------------------------------
//   ~DATASMGR destructor
//----------------------------------------------------------------------
DATASMGR::~DATASMGR(
  void)
  {
	LPDATAINFO		pInfoArray;
	LPBASEBLOCK	pBlock;
	short					i;

	if (datasCount > 0)
		{
		// Get the source ID array info

		pInfoArray = (LPDATAINFO)((LPBASEBLOCK)GetObject(datasUID))->GetData();

		// delete the individual datas

		for (i=0; i<datasCount; i++)
			{
			pBlock = (LPBASEBLOCK)GetObject(pInfoArray[i].serblkUID);
			delete pBlock;
			}

		//delete the Datas Array

		delete (LPBASEBLOCK)GetObject(datasUID);
		}

  }

//----------------------------------------------------------------------
//   AddData
//----------------------------------------------------------------------
BOOL DATASMGR::AddData(
  LPVOID       theData,
  LONG         dataSize)
	{
	RUNTIMEUID   newDatasID;
	LPBASEBLOCK  pInfoArrayBlock;
	LPDATAINFO   pDataInfo;
	LPBASEBLOCK  pNewDataBlock;
	LPVOID       newData;

	pNewDataBlock = new BASEBLOCK(this->pDatasRecList, dataSize);

	newData = pNewDataBlock->GetData();

	if (newData && theData && (dataSize > 0))
		{
		CopyMemory(newData, theData, dataSize);
		}

	newDatasID = GetUID(pNewDataBlock);

	if (this->datasCount <= 0)
		{
		pInfoArrayBlock = new BASEBLOCK(pDatasRecList, sizeof(DATAINFO));
		pDataInfo = (LPDATAINFO)pInfoArrayBlock->GetData();
		this->datasUID = GetUID(pInfoArrayBlock);
		this->datasCount = 1;
		}
	else
		{
		pInfoArrayBlock =
		 (LPBASEBLOCK)GetObject(this->datasUID);
		pDataInfo = (LPDATAINFO)pInfoArrayBlock->ResizeBlock(sizeof(DATAINFO));
		this->datasCount += 1;
		}

	pDataInfo->serblkUID = newDatasID;
  pDataInfo->lockCount = 0;

	return TRUE;

	}


//----------------------------------------------------------------------
//   InsertData
//----------------------------------------------------------------------
BOOL DATASMGR::InsertData(
	LONG         dataIndex,
	LPVOID       theData,
	LONG         dataSize)
	{
  LPBASEBLOCK   pInfoArrayBlock;
  LPDATAINFO    pInfoArray;
  LPBASEBLOCK   pNewDataBlock;
  RUNTIMEUID    newDatasID;
  LPVOID        newData;
  UINT   i;
  BOOL          result = TRUE;


  if ((this->datasCount > 0) && (dataIndex >= 0) &&
      (dataIndex <= this->datasCount))
    {

    if (dataIndex == this->datasCount)
      result = this->AddData(theData, dataSize);
    else
      {
      pNewDataBlock = new BASEBLOCK(this->pDatasRecList, dataSize);
			newData = pNewDataBlock->GetData();
      if (theData && (dataSize > 0))
        {
        CopyMemory(newData, theData, dataSize);
        }
      newDatasID = GetUID(pNewDataBlock);
      pInfoArrayBlock = (LPBASEBLOCK)GetObject(this->datasUID);
      pInfoArrayBlock->ResizeBlock(sizeof(DATAINFO));
      pInfoArray = (LPDATAINFO)pInfoArrayBlock->GetData();
      for (i = (UINT)(this->datasCount-1); i > dataIndex; i--)
         pInfoArray[i].serblkUID = pInfoArray[i-1].serblkUID;
			this->datasCount += 1;
      }
    }
  else
    result = FALSE;

  return result;

  }


//----------------------------------------------------------------------
//   LockData
//----------------------------------------------------------------------
LPVOID DATASMGR::LockDataPtr(
  long      dataIndex)
  {
  LPVOID      theData;
  LPDATAINFO  pInfoArray;

  if ((this->datasCount <= 0) || (dataIndex < 0) ||
      (dataIndex >= this->datasCount))
    theData = NULL;
  else
    {
    pInfoArray = (LPDATAINFO)
		 ((LPBASEBLOCK)GetObject(this->datasUID))->GetData();
    theData = ((LPBASEBLOCK)GetObject(
     pInfoArray[(UINT)dataIndex].serblkUID))->GetData();
    //lock the data only if the data pointer is NOT null
    if (0 != theData)
      pInfoArray[(UINT)dataIndex].lockCount += (short)1;
    }

  return theData;

  }

//----------------------------------------------------------------------
//   UnlockData
//----------------------------------------------------------------------
BOOL DATASMGR::UnlockDataPtr(
  long      dataIndex)
  {
  LPDATAINFO  pInfoArray;
  BOOL        result = TRUE;

  if ((this->datasCount <= 0) || (dataIndex < 0) ||
      (dataIndex >= this->datasCount))
    result = FALSE;
  else
    {
    pInfoArray =
     (LPDATAINFO)((LPBASEBLOCK)GetObject(this->datasUID))->GetData();

    if (pInfoArray[(UINT)dataIndex].lockCount > 0)
      pInfoArray[(UINT)dataIndex].lockCount -= (short)1;
    else
      {
      result = FALSE;
      }
    }

  return result;

  }

//----------------------------------------------------------------------
//   GetDataLockCount
//----------------------------------------------------------------------
short DATASMGR::GetDataLockCount(
	long      dataIndex)
	{
	LPDATAINFO  pInfoArray;
	short       count;

	if ((this->datasCount <= 0) || (dataIndex < 0) ||
			(dataIndex >= this->datasCount))
		count = -1;
	else
		{
		pInfoArray =
		 (LPDATAINFO)((LPBASEBLOCK)GetObject(this->datasUID))->GetData();
		count = pInfoArray[(UINT)dataIndex].lockCount;
		}

	return count;

	}
//----------------------------------------------------------------------
//   GetDataSize
//----------------------------------------------------------------------
LONG DATASMGR::GetDataSize(
  LONG     dataIndex)
  {
  long         byteCount;
  LPDATAINFO   pInfoArray;

  if ((this->datasCount <= 0) || (dataIndex < 0) ||
      (dataIndex >= this->datasCount))
    byteCount = 0;
  else
    {
    pInfoArray = (LPDATAINFO)
     ((LPBASEBLOCK)GetObject(this->datasUID))->GetData();
    byteCount = ((LPBASEBLOCK)GetObject(
     pInfoArray[(UINT)dataIndex].serblkUID))->GetByteCount();
    }

  return byteCount;

  }


//----------------------------------------------------------------------
//   RemoveData
//----------------------------------------------------------------------
BOOL DATASMGR::RemoveData(
  LONG      dataIndex)
  {
  LPBASEBLOCK  pInfoArrayBlock;
  LPDATAINFO       pInfoArray;
  BOOL           result = TRUE;

  if ((this->datasCount <= 0) || (dataIndex < 0) ||
      (dataIndex >= this->datasCount))
    result = FALSE;
  else
    {
    pInfoArrayBlock = (LPBASEBLOCK)GetObject(this->datasUID);
    pInfoArray = (LPDATAINFO)pInfoArrayBlock->GetData();
    if (pInfoArray[(UINT)dataIndex].lockCount != 0)
      {
      result = FALSE;
      }
    else
      {
      delete (LPBASEBLOCK)GetObject(pInfoArray[(UINT)dataIndex].serblkUID);
      pInfoArrayBlock = (LPBASEBLOCK)GetObject(this->datasUID);
      pInfoArrayBlock->
       DeleteSubBlock(dataIndex*sizeof(DATAINFO)+1, sizeof(DATAINFO));
      this->datasCount -= 1;
      }
    }

  return result;

  }


//----------------------------------------------------------------------
//   ResizeData
//----------------------------------------------------------------------
BOOL DATASMGR::ResizeData(
  LONG      dataIndex,
  LONG      dataSize)
  {
  LPDATAINFO      pInfoArray;
  LPBASEBLOCK   pBlock;
  BOOL            result = TRUE;

  if ((this->datasCount <= 0) || (dataIndex < 0) ||
      (dataIndex >= this->datasCount))
    result = FALSE;
  else
    {
    pInfoArray = (LPDATAINFO)
     ((LPBASEBLOCK)GetObject(this->datasUID))->GetData();
    if (pInfoArray[(UINT)dataIndex].lockCount != 0)
      {
      result = FALSE;
      }
    else
      {
      pBlock =
       (LPBASEBLOCK)GetObject(pInfoArray[(UINT)dataIndex].serblkUID);
      if (pBlock->GetByteCount() != dataSize)
        pBlock->ResizeBlock(dataSize - pBlock->GetByteCount());
      }
    }

  return result;

	}

//----------------------------------------------------------------------
//   SetDataFromMemBlock
//----------------------------------------------------------------------
BOOL DATASMGR::SetDataFromMemBlock(
	LONG        dataIndex,
	AMEMBLOCKID  theMemBlock)
	{
	LPDATAINFO    pInfoArray;
	LPBASEBLOCK   pDataBlock;
	LPBASEBLOCK   pMemBlock;
	BOOL          result = TRUE;

	if ((this->datasCount <= 0) || (dataIndex < 0) ||
			(dataIndex >= this->datasCount))
		result = FALSE;
	else
		{
		pInfoArray = (LPDATAINFO)
		 ((LPBASEBLOCK)GetObject(this->datasUID))->GetData();
		if (pInfoArray[(UINT)dataIndex].lockCount != 0)
			{
			result = FALSE;
			}
		else
			{
			pDataBlock =
			 (LPBASEBLOCK)GetObject(pInfoArray[(UINT)dataIndex].serblkUID);
			pMemBlock =(LPBASEBLOCK)GetObject((RUNTIMEUID)theMemBlock);
			pDataBlock->ReplaceData(pMemBlock);
			}
		}

	return result;

	}

//----------------------------------------------------------------------
//   SetData
//----------------------------------------------------------------------
BOOL DATASMGR::SetData(
	LONG      dataIndex,
	LPVOID    theData,
	LONG      dataSize)
	{
	LPVOID          pData;
	LPDATAINFO      pInfoArray;
	LPBASEBLOCK   pBlock;
	BOOL            result = TRUE;

	if ((this->datasCount <= 0) || (dataIndex < 0) ||
			(dataIndex >= this->datasCount))
		result = FALSE;
	else
		{
		pInfoArray = (LPDATAINFO)
		 ((LPBASEBLOCK)GetObject(this->datasUID))->GetData();
		if (pInfoArray[(UINT)dataIndex].lockCount != 0)
			{
			result = FALSE;
			}
		else
			{
			pBlock =
			 (LPBASEBLOCK)GetObject(pInfoArray[(UINT)dataIndex].serblkUID);
      
      //pData must be set in the if/else clause
      if (dataSize != pBlock->GetByteCount())
        {
        pBlock->ResizeBlock(-pBlock->GetByteCount());  //size to zero so no copy
        pData = pBlock->ResizeBlock(dataSize);
        }
      else
        pData = pBlock->GetData();
      
      if (theData)
        {
        CopyMemory(pData, theData, dataSize);
        }
      }
    }

  return result;

  }

//----------------------------------------------------------------------
//   SetDatasCount
//----------------------------------------------------------------------
BOOL DATASMGR::SetDatasCount(
  LONG         numDatas)
  {
  LONG           currentCount;
  BOOL           result = TRUE;

  currentCount = this->datasCount;

  if (numDatas > currentCount)
    result = this->AddDatas(this->pDatasRecList, numDatas-currentCount);
  else if (numDatas < currentCount)
    {
    if (numDatas <= 0)
      result = this->DeleteDatas(currentCount);
    else
      result = this->DeleteDatas(currentCount - numDatas);
    }

  return result;

  }


 //--------------------------------------------------------------------
 //   AddDatas
 //--------------------------------------------------------------------
  BOOL DATASMGR::AddDatas(
    LPRUNTIMERECLIST  pDatasRecList,
    long              deltaCnt)
    {
    LPBASEBLOCK   pUIDArrayBlock;
    LPDATAINFO      pUIDArray;
    UINT     i;

    if (this->datasCount <= 0)
      {
      pUIDArrayBlock =
        new BASEBLOCK(pDatasRecList, deltaCnt*sizeof(DATAINFO));
      this->datasCount = deltaCnt;
      this->datasUID = GetUID(pUIDArrayBlock);
      }
    else
      {
      pUIDArrayBlock = (LPBASEBLOCK)GetObject(this->datasUID);
      pUIDArrayBlock->ResizeBlock(deltaCnt*sizeof(DATAINFO));
      this->datasCount += deltaCnt;
      }

    pUIDArray = (LPDATAINFO)pUIDArrayBlock->GetData();

    for (i = (this->datasCount-deltaCnt); i<this->datasCount; i++)
      {
      pUIDArray[i].serblkUID =
        (new BASEBLOCK(pDatasRecList, (long)0))->GetUID();
      pUIDArray[i].lockCount = 0;
      }

    return TRUE;

    }

 //----------------------------------------------------------------------
 //   DeleteDatas (local)
 //----------------------------------------------------------------------
  BOOL DATASMGR::DeleteDatas(
    long   deltaCnt)
    {
    LPBASEBLOCK   pUIDArrayBlock;
    LPDATAINFO      pUIDArray;
    long            currentCnt;
    UINT     i;
    BOOL            result = TRUE;

    currentCnt = this->datasCount;

    pUIDArrayBlock = (LPBASEBLOCK)GetObject(this->datasUID);
    pUIDArray = (LPDATAINFO)pUIDArrayBlock->GetData();

    //check for locked blocks
    for (i=(UINT)(currentCnt-deltaCnt); i < currentCnt; i++)
      {
      if (pUIDArray[i].lockCount != 0)
        {
        result = FALSE;
        break;
        }
      }

    if (result == TRUE)
      {
      for (i=(UINT)(currentCnt-deltaCnt); i < currentCnt; i++)
        delete (LPBASEBLOCK)GetObject(pUIDArray[i].serblkUID);
      this->datasCount = currentCnt-deltaCnt;

     // get good pointer after deleting data blocks
      pUIDArrayBlock =
       (LPBASEBLOCK)GetObject(this->datasUID);
      if (this->datasCount == 0)
        {
        delete pUIDArrayBlock;
        this->datasUID = NULLSERUID;
        }
      else
        {
				pUIDArrayBlock->ResizeBlock(-(sizeof(DATAINFO)*deltaCnt));
        }
      }

    return result;

    }



/////////////////////////////////////////////////////////////
// FUNCTION:			DATASMGR::StoreDatasToFile
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL DATASMGR::StoreDatasToFile(
  HANDLE hFile)
  {
  DATASMGR_DATABLOCKHDR  dbh;
  DATASMGR_DATAHDR    dh;
  int        i;
  LPSTR	     dataPtr;
  DWORD			 bc;

  dbh.tag = DATASMGR_DATABLOCKTAG;

  dbh.numDatas = (unsigned int)GetDatasCount();
  
  ADBG_VALIDATE(WriteFile(hFile, (LPSTR)&dbh, sizeof(dbh), &bc, NULL) &&
  						 (bc == (DWORD)sizeof(dbh)),
  	return FALSE, "Error writing datas' block header to file");

  for (i=0 ; i<dbh.numDatas ; i++)
    {
    dh.length = GetDataSize(i);
    dataPtr = (LPSTR)LockDataPtr(i);

    ADBG_VALIDATE(WriteFile(hFile, (LPSTR)&dh, sizeof(dh), &bc, NULL) &&
    							(bc == (DWORD)sizeof(dh)),
			return FALSE, "Error writing data's header to file");

    ADBG_VALIDATE(WriteFile(hFile, dataPtr, dh.length, &bc, NULL) &&
    							(bc == (DWORD)dh.length),
    	return FALSE, "Error writing data's data to file");
    
		UnlockDataPtr(i);
    }

  return TRUE;
  }


/////////////////////////////////////////////////////////////
// FUNCTION:			DATASMGR::StoreDatasToMemBlock
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// ASSSUMPTIONS:	memBlck must point to an area that does not
//								overlap the datas area. It wouldn't make sense
//								to break this restriction.
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL DATASMGR::StoreDatasToMemBlock(
  AMEMBLOCKID memBlck,
  unsigned int headerSize)
  {
  DATASMGR_DATABLOCKHDR  dbh;
  DATASMGR_DATAHDR    dh;
  int        i;
  LPSTR    	 dataPtr;
  long       totalSize;
	char*       memBuffer;

  dbh.tag = DATASMGR_DATABLOCKTAG;

  dbh.numDatas = (unsigned int)GetDatasCount();
  
	//figure how large to resize memBlck
	totalSize = (long)headerSize + (long)sizeof(dbh);

  for (i=0 ; i<dbh.numDatas ; i++)
 		totalSize += ((long)sizeof(dh) + GetDataSize(i));

	I_ALWAYSASSERT(AMemSetSize(memBlck, totalSize) != FALSE, return FALSE);

 	memBuffer = (char*)AMemLock(memBlck);

 	memBuffer+=headerSize;
 	
 	CopyMemory(memBuffer, (LPVOID)&dbh, sizeof(dbh));
	
	memBuffer+=sizeof(dbh);

  for (i=0 ; i<dbh.numDatas ; i++)
    {
    dh.length = GetDataSize(i);
   
    dataPtr = (LPSTR)LockDataPtr(i);

    CopyMemory(memBuffer, (LPVOID)&dh, sizeof(dh));
    memBuffer+=sizeof(dh);

    CopyMemory(memBuffer, dataPtr, dh.length);
    memBuffer+=dh.length;

		UnlockDataPtr(i);
		}

	AMemUnlock(memBlck);

  return TRUE;
  }


/////////////////////////////////////////////////////////////
// FUNCTION:			DATASMGR::LoadDatasFromFile
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL DATASMGR::LoadDatasFromFile(
  HANDLE   hFile)
  {
  DATASMGR_DATAHDR  dh;
	DATASMGR_DATABLOCKHDR dbh;
  LPSTR    dataPtr;
	int			 i;
	DWORD			bc;
  
  ADBG_VALIDATE(ReadFile(hFile, (LPSTR)&dbh, sizeof(dbh), &bc, NULL) &&
  							(bc == (DWORD)sizeof(dbh)), 
  	return FALSE, "Error reading in Data's block Header");

	ADBG_VALIDATE(dbh.tag == DATASMGR_DATABLOCKTAG, return FALSE,
		"Invalid datas file");
	
	if(GetDatasCount() != dbh.numDatas)
		SetDatasCount(dbh.numDatas);

	for(i = 0; i<dbh.numDatas; i++)
		{
  	ADBG_VALIDATE(ReadFile(hFile, (LPSTR)&dh, sizeof(dh), &bc, NULL) && 
  								(bc == (DWORD)sizeof(dh)), 
  		return FALSE, "Error reading Data's Header");

		if(dh.length != GetDataSize(i))
			{
  		ResizeData(i, 0);
  		ResizeData(i, dh.length);
			}

  	if (dh.length == 0)
    	continue;

  	// Fetch the address to read into...
  	dataPtr = (LPSTR)LockDataPtr(i);
  	if (!dataPtr)
    	return FALSE;

  	// ...And read away!
		ADBG_VALIDATE(ReadFile(hFile, dataPtr, dh.length, &bc, NULL) &&
									(bc == (DWORD)dh.length),
			return FALSE, "Error reading data");
		
		UnlockDataPtr(i);
		}	 

  return TRUE;
  }


/////////////////////////////////////////////////////////////
// FUNCTION:			EXEOBJINST::LoadDatasFromMemory
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL DATASMGR::LoadDatasFromMemory(
  LPVOID memBuffer)
  {
  DATASMGR_DATAHDR  * dhPtr;
	DATASMGR_DATABLOCKHDR * dbhPtr;
  LPVOID   dataPtr;
	int			 i;
	char*    memPtr = (char*)memBuffer;

	dbhPtr = (DATASMGR_DATABLOCKHDR *)memPtr;

	ADBG_VALIDATE(dbhPtr->tag == DATASMGR_DATABLOCKTAG, return FALSE,
		"Invalid datas file");

	memPtr+= sizeof(DATASMGR_DATABLOCKHDR);
	
	if(GetDatasCount() != dbhPtr->numDatas)
		SetDatasCount(dbhPtr->numDatas);

	for(i = 0; i < dbhPtr->numDatas; i++)
		{
		dhPtr = (DATASMGR_DATAHDR *)memPtr;
		memPtr+= sizeof(DATASMGR_DATAHDR);

		if(dhPtr->length != GetDataSize(i))
			{
  		ResizeData(i, 0);
  		ResizeData(i, dhPtr->length);
			}

  	if (dhPtr->length == 0)
    	continue;									

  	// Fetch the address to read into...
  	dataPtr = (LPSTR)LockDataPtr(i);
  	
  	if (!dataPtr)
    	return FALSE;

  	CopyMemory(dataPtr, memPtr, dhPtr->length);

		memPtr += dhPtr->length;

		UnlockDataPtr(i);
		}

  return TRUE;
  }
