//======================================================================
//   exeobjs.cpp -- runtime serius Object data
//
// jon 25-Aug-92 clear this->status at construct time
//======================================================================

#include "almsys.h"

#include "exeobjs.h"
#include "exserblk.h"
#include "sereclib.h"
#include "system.h"
#include "ownerlst.h"
#include "oiputils.h"
#include "intdebug.h"


//----------------------------------------------------------------------
// EXEOBJINST::EXEOBJINST (constructor)
//
// Creates a copy of the object's data blocks.
//----------------------------------------------------------------------
EXEOBJINST::EXEOBJINST(
  ATYPEID      typeID,
  TSTRING     objectName,
  long        objectStatus,
  long        useCount,
	LPDATASMGR  pSrcDatasMgr,
	SERUID			typeUID):
    OBJECTRECORD(typeID,
                 pTheSystem->GetFileObject()->GetExeSeriusBlocks(),
								 pSrcDatasMgr, typeUID),
    objectName(objectName),
    ownerList()
  {
 // Make sure the status field is initialized!
  this->status = 0;

	this->objectStatus = objectStatus;
  this->useCount = useCount;

  this->objectStatus &= (~OBJ_DESIGNTIME);
  this->objectStatus |= OBJ_RUNTIME;
	}


//----------------------------------------------------------------------
// EXEOBJINST::~EXEOBJINST (destructor)
//----------------------------------------------------------------------
EXEOBJINST::~EXEOBJINST()
  {
  }

//----------------------------------------------------------------------
// EXEOBJINST::StoreAsPersistent
//
// Writes the persistent info for an object to the specified file.
// Returns FALSE on error.
//----------------------------------------------------------------------
BOOL EXEOBJINST::StoreAsPersistent(
  HANDLE hFile)
  {
  OBJECTHDR  oh;
  DATAHDR    dh;
  int        i;
  LPSTR      dataPtr;
	DWORD				nWritten;

  if (!TestPersistentsFlag())
    {
    // We can always successfully store
    // persistent info for objects that aren't...
    return TRUE;
		}

  oh.tag = EXEOBJ_TAG;
  oh.objectID = GetUID(this);
  oh.numDatas = (unsigned int)GetDatasCount();
	WriteFile(hFile, &oh, sizeof(oh), &nWritten, NULL);

  if (nWritten != sizeof(oh))
    return FALSE;

  for (i=0 ; i<oh.numDatas ; i++)
    {
    dh.length = this->GetDataSize(i);
    dataPtr = (LPSTR)this->LockDataPtr(i);

		WriteFile(hFile, &dh, sizeof(dh), &nWritten, NULL);
    if (nWritten != sizeof(dh))
      return FALSE;

    if(dh.length > 0)
    	{
    	if(!WriteFile(hFile, dataPtr, dh.length, &nWritten, NULL) ||
    		(dh.length != nWritten))
      	{
      	I_ASSERT(FALSE);
      	return FALSE; //major error
      	}
      }
      
		this->UnlockDataPtr(i);
    }

  return TRUE;
  }

//----------------------------------------------------------------------
// EXEOBJINST::LoadPersistentDataBlock
//
// Reads in the persistent info for the specified data block from the
// specified file.  Returns FALSE on error.
//----------------------------------------------------------------------
BOOL EXEOBJINST::LoadPersistentDataBlock(
  HANDLE   hFile,
  long  theDataIndex)
  {
  DATAHDR  dh;
  LPSTR    dataPtr;
	DWORD			nRead;

	ReadFile(hFile, &dh, sizeof(DATAHDR), &nRead, NULL);
  if (nRead != sizeof(DATAHDR))
    return FALSE;

 //--------------------------------------------------------------------
 // Push the size down to zero first so the memory manager doesn't
 // carefully copy any data in the 'dh.length' resize, cuz we're
 // just gonna smash that on the read we're about to do!
 //--------------------------------------------------------------------
  this->ResizeData(theDataIndex, 0);
  this->ResizeData(theDataIndex, dh.length);

  // Can always read zero bytes successfully
  if (dh.length == 0)
    return TRUE;

  // Fetch the address to read into...
  dataPtr = (LPSTR)this->LockDataPtr(theDataIndex);
  if (!dataPtr)
    return FALSE;

  // ...And read away!
  if (dh.length > 0)
    {
    if (!ReadFile(hFile, dataPtr, dh.length, &nRead, NULL) ||
    	(dh.length != nRead))
      {
      I_ASSERT(FALSE);
      return FALSE;
      }
    }

  this->UnlockDataPtr(theDataIndex);

  return TRUE;
  }
