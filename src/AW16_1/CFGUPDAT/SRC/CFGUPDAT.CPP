#include <windows.h>

#include "cfgupdat.h"
#include "fileobj.h"
#include "system.h"
#include "sereclib.h"


//----------------------------------------------------------------------
//   DoUpdate
//----------------------------------------------------------------------
void DoUpdate(
  LPFILEOBJECT       pFileObject,
  LPSTR              pathName,
  LPSTR              fileName,
  CFGUPDATECALLBACK  pCallBack)
  {
  RECORDBLOCKDATA  recBlkData;
  SERIUSBLOCKDATA  serBlkData;
  FILEOBJECTDATA   fileObjectData;

  LPSERIUSBLOCK    tempBlock;
	LPRECORDBLOCK    tempRecordBlock;
  LPRECORDBLOCK    pBlockList;

	lstrcpy(fileObjectData.idString, pFileObject->GetIdString());
	fileObjectData.majorVersion = pFileObject->GetMajorVersion();
	fileObjectData.minorVersion = pFileObject->GetMinorVersion();
	fileObjectData.fileType = pFileObject->GetFileType();
  fileObjectData.pathName = pathName;
  fileObjectData.fileName = fileName;
  fileObjectData.pUidTable = pFileObject->GetUIDTable();
  fileObjectData.pStringSpace = pFileObject->GetStringSpace();
  fileObjectData.headRec = pFileObject->GetHeadRec().id;
  fileObjectData.headBlock = pFileObject->GetHeadBlock().id;

  (pCallBack)(CFGUPD_DOFILEOBJECT, &fileObjectData);

  pBlockList = pFileObject->GetSeriusBlocks();
  tempBlock = (LPSERIUSBLOCK)pBlockList->NextRec(0);
	while (0 != tempBlock)
    {
    serBlkData.selfID     = GetUID(tempBlock).id;
  	serBlkData.blockMemID = tempBlock->GetBlockMemID();
	  serBlkData.byteCount  = tempBlock->GetByteCount();
	  serBlkData.theData    = tempBlock->GetData();

    (*pCallBack)(CFGUPD_DOSERIUSBLOCK, &serBlkData);

    tempBlock = (LPSERIUSBLOCK)pBlockList->NextRec(tempBlock);
    }


  pBlockList = pFileObject->GetRecordBlocks();
	tempRecordBlock = (LPRECORDBLOCK)pBlockList->NextRec(0);
	while (0 != tempRecordBlock)
    {
    recBlkData.selfID        = GetUID(tempRecordBlock).id;
    recBlkData.type          = tempRecordBlock->GetType();
		recBlkData.count         = tempRecordBlock->GetCount();
		recBlkData.recordSize    = tempRecordBlock->GetRecordSize();
		recBlkData.blockCount    = tempRecordBlock->GetBlockCount();
	  recBlkData.blockID       = tempRecordBlock->GetBlockID();

    (*pCallBack)(CFGUPD_DORECORDBLOCK, &recBlkData);

		tempRecordBlock  = (LPRECORDBLOCK)pBlockList->NextRec(tempRecordBlock);
    }

  }


extern "C"{

//----------------------------------------------------------------------
//   ConvertFile
//----------------------------------------------------------------------
BOOL far pascal _export ConvertFile(
  int                hFile,
  LPSTR              pathName,
  LPSTR              fileName,
  LPSERIUSMEMORY     pTheMemory,
  LPSTRINGSPACE      pStringSpace,
  CFGUPDATECALLBACK  pCallBack)
  {
  LPFILEOBJECT   pFileObject;

  SYSTEM::SetTheMemory(pTheMemory);
  SYSTEM::SetTempStringSpace(pStringSpace);

  pFileObject = new FILEOBJECT(hFile);

  DoUpdate(pFileObject, pathName, fileName, pCallBack);

  pFileObject->GetUIDTable()->BuildFreeList();

  return TRUE;

  }

} // extern "C"


