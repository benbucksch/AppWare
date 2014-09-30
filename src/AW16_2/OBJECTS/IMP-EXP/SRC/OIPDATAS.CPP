////////////////////////////////////////////////////////////////////////
// oipdatas.cpp -- OIP manipulators of serius object data blocks
//
// jon 16-Aug-92 documented
////////////////////////////////////////////////////////////////////////

#include "almsys.h"

#include "recblk.h"
#include "object.h"
#include "system.h"
#include "sereclib.h"
#include "objinst.h"
#include "exeobjs.h"
#include "tempobj.h"
#include "rectypes.h"
#include "exetypes.h"
#include "project.h"
#include "oiputils.h"
#include "oipdebug.h"

//bit for who stored the datas
#define A_DATAS_STOREDBYVAB		0x00000001L

//tag identifing datas block
#define A_DATAS_BLOCKTAG MAKEID('D', 'B', 'T', 'G')

//current version of datas block
#define A_DATAS_VERSION_1      100L

struct A_DATAS_MASTERBLOCKHDR
	{
	long tag;
	long info;
	long version;
	TYPEID typeID;
	};

////////////////////////////////////////////////////////////////////////
//  OIP functions to be exported
////////////////////////////////////////////////////////////////////////

extern "C" {

//----------------------------------------------------------------------
//   AObjAddData
//----------------------------------------------------------------------
BOOL ALMAPI	AObjAddData(
  OBJECTID  theObject,
  LPVOID    theData,
  LONG      dataSize)
  {
  LPOBJECTRECORD pObject;
  BOOL           result = FALSE;

  pTheSystem->GetCurrentContext();

  pObject = GetObjectRecord(theObject);

  if (pObject)
    result = pObject->AddData(theData, dataSize);

  return result;

  }

//----------------------------------------------------------------------
//   AObjInsertData
//----------------------------------------------------------------------
BOOL ALMAPI	AObjInsertData(
  OBJECTID  theObject,
  LONG      dataIndex,
  LPVOID    theData,
  LONG      dataSize)
  {
  LPOBJECTRECORD  pObject;
  BOOL            result;

  pTheSystem->GetCurrentContext();

  pObject = GetObjectRecord(theObject);

  if (pObject)
    result = pObject->InsertData(dataIndex, theData, dataSize);

  return result;

  }


//----------------------------------------------------------------------
//   AObjLockData
//----------------------------------------------------------------------

// #undef macro if in debug mode
#ifdef AObjLockData
#undef AObjLockData
#endif

LPVOID ALMAPI	AObjLockData(
  OBJECTID  theObject,
  LONG      dataIndex)
  {
  LPVOID          theData = 0;
  LPOBJECTRECORD  pObject;

  pTheSystem->GetCurrentContext();

  pObject = GetObjectRecord(theObject);

  if (pObject)
    theData = pObject->LockDataPtr(dataIndex);

  return theData;

  }

//----------------------------------------------------------------------
//   AObjUnlockData
//----------------------------------------------------------------------

#ifdef AObjUnlockData
#undef AObjUnlockData
#endif

BOOL ALMAPI	AObjUnlockData(
  OBJECTID  theObject,
  LONG      dataIndex)
  {
  LPOBJECTRECORD  pObject;
  BOOL            result = FALSE;

  pTheSystem->GetCurrentContext();

  pObject = GetObjectRecord(theObject);

  if (pObject)
		{
		ADBG_EVAL(
			if (pObject->GetDataLockCount(dataIndex) <= 0)
				ADbgPrint(ADBG_WARNING, 0, "Attempt to unlock data that is not locked.");
			);
    result = pObject->UnlockDataPtr(dataIndex);
		}

  return result;

  }

//----------------------------------------------------------------------
//   AObjGetDataCount
//----------------------------------------------------------------------
LONG ALMAPI	AObjGetDataCount(
  OBJECTID  theObject)
  {
  LPOBJECTRECORD  pObject;

  pTheSystem->GetCurrentContext();

  pObject = GetObjectRecord(theObject);

  if (pObject)
    return pObject->GetDatasCount();
  else
    return 0;

  }

//----------------------------------------------------------------------
//   AObjGetDataLockCount
//----------------------------------------------------------------------
LONG ALMAPI	AObjGetDataLockCount(
  OBJECTID  theObject,
	LONG			theData)
  {
  LPOBJECTRECORD  pObject;

  pTheSystem->GetCurrentContext();

  pObject = GetObjectRecord(theObject);

  if (pObject)
    return pObject->GetDataLockCount(theData);
  else
    return 0;
  }

//----------------------------------------------------------------------
//   AObjGetDataSize
//----------------------------------------------------------------------
LONG ALMAPI	AObjGetDataSize(
  OBJECTID theObject,
  LONG     dataIndex)
  {
  long            byteCount;
  LPOBJECTRECORD  pObject;

  pTheSystem->GetCurrentContext();

  pObject = GetObjectRecord(theObject);

  if (pObject)
    byteCount = pObject->GetDataSize(dataIndex);
  else
    byteCount = 0;

  return byteCount;
  }


//----------------------------------------------------------------------
//   AObjDeleteData
//----------------------------------------------------------------------
BOOL ALMAPI	AObjDeleteData(
  OBJECTID  theObject,
  LONG      dataIndex)
  {
  LPOBJECTRECORD pObject;
  BOOL           result = FALSE;

  pTheSystem->GetCurrentContext();

  pObject = GetObjectRecord(theObject);

  if (pObject)
		{
		ADBG_EVAL(
			if (pObject->GetDataLockCount(dataIndex) != 0)
				{
				PrintObjDataInfo(theObject, dataIndex);
				ADbgPrint(ADBG_ERROR, 0, "Attempt to delete locked data block");
				}
			);
    result = pObject->RemoveData(dataIndex);
		}

  return result;

  }


//----------------------------------------------------------------------
//   AObjResizeData
//----------------------------------------------------------------------
BOOL ALMAPI	AObjResizeData(
  OBJECTID  theObject,
  LONG      dataIndex,
  LONG      dataSize)
  {
  LPOBJECTRECORD  pObject;
  BOOL            result = FALSE;

  pTheSystem->GetCurrentContext();

  pObject = GetObjectRecord(theObject);

  if (pObject)
		{
		ADBG_EVAL(
			if (pObject->GetDataLockCount(dataIndex) != 0)
				{
				PrintObjDataInfo(theObject, dataIndex);
				ADbgPrint(ADBG_ERROR, 0, "Attempt to resize locked data block");
				}
			);
    result = pObject->ResizeData(dataIndex, dataSize);
		}

  return result;

	}

//----------------------------------------------------------------------
//   AObjSetData
//----------------------------------------------------------------------
BOOL ALMAPI	AObjSetDataFromMemBlock(
	OBJECTID    theObject,
	LONG        dataIndex,
	MEMBLOCKID  theMemBlock)
	{
	LPOBJECTRECORD  pObject;
	BOOL            result = FALSE;

	pTheSystem->GetCurrentContext();

	pObject = GetObjectRecord(theObject);

	if (pObject)
		{
		ADBG_EVAL(
			if (pObject->GetDataLockCount(dataIndex) != 0)
				{
				PrintObjDataInfo(theObject, dataIndex);
				ADbgPrint(ADBG_ERROR, 0,
				 "Attempt to set data from MemBlock on locked data block");
				}
			);
		result = pObject->SetDataFromMemBlock(dataIndex, theMemBlock);
		}

	return result;

	}

//----------------------------------------------------------------------
//   AObjSetData
//----------------------------------------------------------------------
BOOL ALMAPI	AObjSetData(
  OBJECTID  theObject,
  LONG      dataIndex,
  LPVOID    theData,
  LONG      dataSize)
  {
  LPOBJECTRECORD  pObject;
  BOOL            result = FALSE;

  pTheSystem->GetCurrentContext();

  pObject = GetObjectRecord(theObject);

  if (pObject)
		{
		ADBG_EVAL(
			if (pObject->GetDataLockCount(dataIndex) != 0)
				{
				PrintObjDataInfo(theObject, dataIndex);
				ADbgPrint(ADBG_ERROR, 0, "Attempt to set data on locked data block");
				}
			);
    result = pObject->SetData(dataIndex, theData, dataSize);
		}

  return result;

  }

//----------------------------------------------------------------------
//   AObjSetDataCount
//----------------------------------------------------------------------
BOOL ALMAPI	AObjSetDataCount(
  OBJECTID  theObject,
  LONG      numDatas)
  {
  LPOBJECTRECORD pObject;
  BOOL           result = FALSE;

  pTheSystem->GetCurrentContext();

  pObject = GetObjectRecord(theObject);

  if (pObject)
    result = pObject->SetDatasCount(numDatas);

  return result;

  }

//----------------------------------------------------------------------
//   AObjGetRuntimeInfo
//----------------------------------------------------------------------
LONG ALMAPI	AObjGetRuntimeInfo(
  OBJECTID  theObject)
  {
  LPOBJECTRECORD	pObject;
	LONG						lData;

  if (theObject == 0)
    return 0;

  pTheSystem->GetCurrentContext();

  pObject = GetObjectRecord(theObject);

  if (pObject)
    lData = pObject->GetRunTimeData();
  else
    lData = 0;

  return lData;

  }

//----------------------------------------------------------------------
//   AObjSetRuntimeInfo
//----------------------------------------------------------------------
LONG ALMAPI	AObjSetRuntimeInfo(
  OBJECTID  theObject,
	LONG			lTheData)
  {
  LPOBJECTRECORD	pObject;
	LONG						lData;

  if (theObject == 0)
    return 0;

  pTheSystem->GetCurrentContext();

  pObject = GetObjectRecord(theObject);

  if (pObject)
		{
    lData = pObject->GetRunTimeData();
		pObject->SetRunTimeData(lTheData);
		}
  else
		{
    lData = 0;
		}

  return lData;

  }

//----------------------------------------------------------------------
//   AObjWriteDatas
//----------------------------------------------------------------------
BOOL ALMAPI	AObjWriteDatas(
  OBJECTID  theObject,
	HANDLE		hFile)
  {
  LPOBJECTRECORD 	pObject;
  BOOL           	result = FALSE;
	long 						msg = 0;
	A_DATAS_MASTERBLOCKHDR dh;  
  
  pTheSystem->GetCurrentContext();

	if (pTheSystem->IsExeStructures())
		{
		pObject = GetObjectRecord(theObject);

		if (pObject)
			{
			SendMessageToObject(AOBJ_PREWRITE, msg, msg, msg, theObject);

			dh.tag = A_DATAS_BLOCKTAG;
			dh.info = A_DATAS_STOREDBYVAB;
			dh.version = A_DATAS_VERSION_1;
			dh.typeID = AObjGetType(theObject);

			ADBG_VALIDATE(dh.typeID != 0, return FALSE, "Invalid object type");

ADBG_EVAL(
			{
			DWORD		nWritten;
			WriteFile(hFile, &dh, sizeof(dh), &nWritten, NULL);
			if (nWritten != sizeof(dh))
				{
				ADbgPrint(ADBG_ERROR, 0, "Error writing data's master block header");
				return FALSE;
				}
			}
			)

			result = pObject->StoreDatasToFile(hFile);

			SendMessageToObject(AOBJ_POSTWRITE, msg, msg, msg, theObject);
			}
		}

  return result;
  }

//----------------------------------------------------------------------
//   AObjCopyDatas
//----------------------------------------------------------------------
BOOL ALMAPI	AObjCopyDatas(
  OBJECTID  theObject,
	MEMBLOCKID memBlck)
  {
  LPOBJECTRECORD 	pObject;
  BOOL           	result = FALSE;
	long 						msg = 0;
	A_DATAS_MASTERBLOCKHDR dh;  
  LPSTR memBuffer;

  pTheSystem->GetCurrentContext();

	if (pTheSystem->IsExeStructures())
		{
		pObject = GetObjectRecord(theObject);

		if (pObject)
			{
			//check the type before copying the datas.
			dh.typeID = AObjGetType(theObject);

			ADBG_VALIDATE(dh.typeID != 0, return FALSE, "Invalid object type");

			SendMessageToObject(AOBJ_PREWRITE, msg, msg, msg, theObject);

			//the second parameter is the offset of where to start
			//copying the datas.
			result = pObject->StoreDatasToMemBlock(memBlck, sizeof(dh));

			if(result)
				{
				dh.tag = A_DATAS_BLOCKTAG;
				dh.info = A_DATAS_STOREDBYVAB;
				dh.version = A_DATAS_VERSION_1;

				memBuffer = (LPSTR)AMemLock(memBlck);

				CopyMemory(memBuffer, (LPVOID)&dh, sizeof(dh));

				AMemUnlock(memBlck);

				SendMessageToObject(AOBJ_POSTWRITE, msg, msg, msg, theObject);
				}
			}
		}

  return result;
  }

//----------------------------------------------------------------------
//   AObjReadDatas
//----------------------------------------------------------------------
BOOL ALMAPI	AObjReadDatas(
  OBJECTID  theObject,
	HANDLE		hFile)
  {
  BOOL           					result = FALSE;
	A_DATAS_MASTERBLOCKHDR 	dh;
  
  pTheSystem->GetCurrentContext();

	if (pTheSystem->IsExeStructures())
		{
ADBG_EVAL(
			{
			DWORD		nRead;
			ReadFile(hFile, &dh, sizeof(dh), &nRead, NULL);
			if (nRead != sizeof(dh))
				{
				ADbgPrint(ADBG_ERROR, 0, "Could not read in datas' master header");
				return FALSE;
				}
			}
			)

		ADBG_VALIDATE((dh.tag == A_DATAS_BLOCKTAG) && (dh.version ==
				A_DATAS_VERSION_1), return FALSE, "Inalid master datas header.");

		if (dh.info == A_DATAS_STOREDBYVAB)
			{
			long 			msg					= 0;
			OBJECTID	tmpObjID;
			TYPEID 		typeID;  

			typeID = AObjGetType(theObject);

			ADBG_VALIDATE(typeID == dh.typeID, return FALSE,
					"Trying to read datas for the wrong type of object.");

			tmpObjID = AObjCreate(typeID);

			// It is safe to restore the datas to this temp object because
			// the temp object can not have received any events/messages
			// that could cause it to associate runtime data with the
			// object.
			if (tmpObjID != NULL)
				{
				LPOBJECTRECORD	pTmpObject = GetObjectRecord(tmpObjID);

				if (pTmpObject != NULL)
					{
					SendMessageToObject(AOBJ_ASLEEP, msg, msg, msg, tmpObjID);

					result = pTmpObject->LoadDatasFromFile(hFile);

					if (result)
						{
						//?? Temporary object is getting these messages?
						SendMessageToObject(AOBJ_READ, msg, msg, msg, tmpObjID);
						SendMessageToObject(AOBJ_AWAKENED, msg, msg, msg, tmpObjID);

						AObjAssign(tmpObjID, theObject);
						}
					}

				AObjDestroy(tmpObjID);
				}
			}
		else
			{
			ADBG_PRINT_W("Datas weren't written by AppWare");
			}
		}

  return result;
  }

//----------------------------------------------------------------------
//   AObjRestoreDatas
//----------------------------------------------------------------------
BOOL ALMAPI	AObjRestoreDatas(
  OBJECTID  theObject,
	LPVOID memBlockPtr)
  {
  BOOL           	result = FALSE;
  LPSTR 					memPtr = (LPSTR)memBlockPtr;
	A_DATAS_MASTERBLOCKHDR * dhPtr;  

  pTheSystem->GetCurrentContext();

	if (pTheSystem->IsExeStructures())
		{
		dhPtr = (A_DATAS_MASTERBLOCKHDR *)memPtr;

		memPtr += sizeof(A_DATAS_MASTERBLOCKHDR);

		ADBG_VALIDATE((dhPtr->tag == A_DATAS_BLOCKTAG) && (dhPtr->version ==
				A_DATAS_VERSION_1), return FALSE, "Inalid master datas header.");

		if (dhPtr->info == A_DATAS_STOREDBYVAB)
			{
			long 			msg					= 0;
			OBJECTID	tmpObjID;
			TYPEID 		typeID;  

			typeID = AObjGetType(theObject);

			ADBG_VALIDATE(typeID == dhPtr->typeID, return FALSE,
					"Trying to restore datas to the wrong type of object.");

			tmpObjID = AObjCreate(typeID);

			// It is safe to restore the datas to this temp object because
			// the temp object can not have received any events/messages
			// that could cause it to associate runtime data with the
			// object.
			if (tmpObjID != NULL)
				{
				LPOBJECTRECORD	pTmpObject = GetObjectRecord(tmpObjID);

				if (pTmpObject != NULL)
					{
					SendMessageToObject(AOBJ_ASLEEP, msg, msg, msg, tmpObjID);

					result = pTmpObject->LoadDatasFromMemory(memPtr);

					if (result)
						{
						//?? Temporary object is getting these messages?
						SendMessageToObject(AOBJ_READ, msg, msg, msg, tmpObjID);
						SendMessageToObject(AOBJ_AWAKENED, msg, msg, msg, tmpObjID);

						AObjAssign(tmpObjID, theObject);
						}
					}

				AObjDestroy(tmpObjID);
				}
			}
		else
			{
			ADBG_PRINT_W("Datas weren't written by AppWare");
			}
		}

  return result;
  }


} // extern "C"


///////////////////////////////////////////////
/////                                     /////
/////    T Y P E   D A T A   S T U F F    /////
/////                                     /////
///////////////////////////////////////////////


extern "C" {

//----------------------------------------------------------------------
//   ATypeAddData
//----------------------------------------------------------------------
BOOL ALMAPI	ATypeAddData(
  TYPEID  theType,
  LPVOID  theData,
  LONG    dataSize)
  {
  LPBASETYPE  pType = GetTypeRecord(theType);

  return pType
         ? pType->AddData(theData, dataSize)
         : FALSE;
  }

//----------------------------------------------------------------------
//   ATypeInsertData
//----------------------------------------------------------------------
BOOL ALMAPI	ATypeInsertData(
  TYPEID  theType,
  LONG    dataIndex,
  LPVOID  theData,
  LONG    dataSize)
  {
  LPBASETYPE  pType = GetTypeRecord(theType);

  return pType
         ? pType->InsertData(dataIndex, theData, dataSize)
         : FALSE;
  }


//----------------------------------------------------------------------
//   ATypeLockData
//----------------------------------------------------------------------
LPVOID ALMAPI	ATypeLockData(
  TYPEID  theType,
  LONG    dataIndex)
  {
  LPBASETYPE pType = GetTypeRecord(theType);

  return pType
         ? pType->LockDataPtr(dataIndex)
         : 0;
  }

//----------------------------------------------------------------------
//   ATypeUnlockData
//----------------------------------------------------------------------
BOOL ALMAPI	ATypeUnlockData(
  TYPEID  theType,
  LONG    dataIndex)
  {
  LPBASETYPE pType = GetTypeRecord(theType);

  return pType
         ? pType->UnlockDataPtr(dataIndex)
         : FALSE;
  }

//----------------------------------------------------------------------
//   ATypeGetDataCount
//----------------------------------------------------------------------
LONG ALMAPI	ATypeGetDataCount(
  TYPEID  theType)
  {
  LPBASETYPE pType = GetTypeRecord(theType);

  return pType
         ? pType->GetDatasCount()
         : 0;
  }

//----------------------------------------------------------------------
//   ATypeGetDataSize
//----------------------------------------------------------------------
LONG ALMAPI	ATypeGetDataSize(
  TYPEID  theType,
  LONG    dataIndex)
  {
  LPBASETYPE pType = GetTypeRecord(theType);

  return pType
         ? pType->GetDataSize(dataIndex)
         : 0;
  }

//----------------------------------------------------------------------
//   ATypeDeleteData
//----------------------------------------------------------------------
BOOL ALMAPI	ATypeDeleteData(
  TYPEID  theType,
  LONG    dataIndex)
  {
  LPBASETYPE pType = GetTypeRecord(theType);

  return pType
         ? pType->RemoveData(dataIndex)
         : FALSE;
  }

//----------------------------------------------------------------------
//   ATypeResizeData
//----------------------------------------------------------------------
BOOL ALMAPI	ATypeResizeData(
  TYPEID  theType,
  LONG    dataIndex,
  LONG    dataSize)
  {
  LPBASETYPE pType = GetTypeRecord(theType);

  return pType
         ? pType->ResizeData(dataIndex, dataSize)
         : FALSE;
  }


//----------------------------------------------------------------------
//   ATypeSetData
//----------------------------------------------------------------------
BOOL ALMAPI	ATypeSetData(
  TYPEID  theType,
  LONG    dataIndex,
  LPVOID  theData,
  LONG    dataSize)
  {
  LPBASETYPE pType = GetTypeRecord(theType);

  return pType
         ? pType->SetData(dataIndex, theData, dataSize)
         : FALSE;
  }

//----------------------------------------------------------------------
//   ATypeSetDataCount
//----------------------------------------------------------------------
BOOL ALMAPI	ATypeSetDataCount(
  TYPEID  theType,
  LONG    numDatas)
  {
  LPBASETYPE pType = GetTypeRecord(theType);

  return pType
         ? pType->SetDatasCount(numDatas)
         : FALSE;
  }

//----------------------------------------------------------------------
//   ATypeGetRuntimeInfo
//----------------------------------------------------------------------
LONG ALMAPI	ATypeGetRuntimeInfo(
  TYPEID  theType)
  {
  LPBASETYPE pType = GetTypeRecord(theType);

  return pType
         ? pType->GetRunTimeData()
         : 0;

  }

//----------------------------------------------------------------------
//   ATypeSetRuntimeInfo
//----------------------------------------------------------------------
LONG ALMAPI	ATypeSetRuntimeInfo(
  TYPEID    theType,
	LONG			lTheData)
  {
  LPBASETYPE pType = GetTypeRecord(theType);
	LONG			 lData;

  if (pType)
		{
    lData = pType->GetRunTimeData();
		pType->SetRunTimeData(lTheData);
		}
  else
		{
    lData = 0;
		}

  return lData;

  }

} // extern "C"
