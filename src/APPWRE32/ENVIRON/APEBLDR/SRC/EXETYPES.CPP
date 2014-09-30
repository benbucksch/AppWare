#include "almsys.h"
#include "intalm.h"

#include "rectypes.h"
#include "exetypes.h"
#include "exeobjs.h"
#include "exehead.h"
#include "system.h"
#include "sereclib.h"
#include "exrecblk.h"
#include "exeobjs.h"
#include "rtreclib.h"

////////////////////////////////////////////////////////////////////////
//   EXEOBJTYPE::EXEOBJTYPE (constructor)
////////////////////////////////////////////////////////////////////////
EXEOBJTYPE::EXEOBJTYPE(
	LPBASETYPE srcTypePtr,
	long	  instCount):
		BASETYPE(srcTypePtr, instCount,
             pTheSystem->GetFileObject()->GetExeSeriusBlocks())
	{
  LPEXERECORDBLOCK  pRecordBlock;

	pRecordBlock = new EXERECORDBLOCK(sizeof(EXEOBJINST), RT_EXEOBJINST);

  pRecordBlock->PreAllocate(instCount);

	exeObjInstList = pRecordBlock->GetUID();

	SetAdmnProc(srcTypePtr->GetAdmnProc());
	}

////////////////////////////////////////////////////////////////////////
//   EXEOBJTYPE::~EXEOBJTYPE (destructor)
////////////////////////////////////////////////////////////////////////
EXEOBJTYPE::~EXEOBJTYPE()
	{

  // Kill the instances of this type

  ((LPEXERECORDBLOCK)GetObject(exeObjInstList))->PurgeBlock();

	delete (LPEXERECORDBLOCK)GetObject(exeObjInstList);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetObjectType
//
// CLASS.......:  EXEOBJTYPE
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:  pointer to the type record of passed type ID
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
LPEXEOBJTYPE EXEOBJTYPE::GetObjectType(
	ATYPEID typeID)
	{
	int					 i=0;
	LPEXEOBJTYPE pExeObjType;
	SERUID 			 headExeRec;
	RUNTIMEUID 	 recordBlock;

	headExeRec = pTheSystem->GetFileObject()->GetHeadExeRec();
	recordBlock = ((LPEXEHEAD)GetObject(headExeRec))->GetExeObjTypeList();
	while (0 != (pExeObjType = (LPEXEOBJTYPE)GetObject(recordBlock, i)))
		if (pExeObjType->GetTypeID() == typeID)
			return pExeObjType;
	return NULL;
	}

//----------------------------------------------------------------------
//   StorePersistenObjects
//----------------------------------------------------------------------
BOOL EXEOBJTYPE::StorePersistentObjects(
	HANDLE hFile)
	{
	BOOL					fStoreOk;
	int						i						= 0;
	LPEXEOBJINST	objPtr;
	AOBJECTID			objId;
  ADMNPROC			admnProc;
	AObjMessage		systemRec;

	systemRec.message3 =
		systemRec.message4 = 0;

	admnProc = GetAdmnProc();

	while((objPtr = (LPEXEOBJINST)GetObject(exeObjInstList, i))!=NULL)
    {
		if (objPtr->TestPersistentsFlag())
			{
			objId = (LONG) ::GetUID(objPtr).id;

			systemRec.message1 = AOBJ_PREWRITE;
			if (admnProc != NULL)
				admnProc(objId, &systemRec);

			fStoreOk = objPtr->StoreAsPersistent(hFile);

			systemRec.message1 = AOBJ_POSTWRITE;
			if (admnProc != NULL)
				admnProc(objId, &systemRec);

			if (!fStoreOk)
				return FALSE;
			}
		}
	return TRUE;
	}

//----------------------------------------------------------------------
//   CheckObjectVersions
//----------------------------------------------------------------------
BOOL EXEOBJTYPE::CheckObjectVersions(
	void)
	{
	int i;
	AObjMessage sysRec;
	BOOL         result = TRUE;
	LPEXEOBJINST pObject;
	LONG				 lStatus;

	i = 0;
	while (0 != (pObject = (LPEXEOBJINST)GetObject(exeObjInstList, i)))
		{
		sysRec.message1 = AOBJ_READ;
		sysRec.message2 = 0;
		sysRec.message3 = 0;
		sysRec.message4 = 0;
		sysRec.errorMessage[0] = '\0';

		lStatus = this->GetAdmnProc()(::GetUID(pObject).id, &sysRec);

		ADBG_EVAL(
			if (lStatus == A_NOTHANDLED)
				ADbgPrint(ADBG_WARNING, 0, "Object Type: \"%c%c%c%c\"] should handle AOBJ_READ message",
					HIBYTE(HIWORD(typeID)),
					LOBYTE(HIWORD(typeID)),
					HIBYTE(LOWORD(typeID)),
					LOBYTE(LOWORD(typeID)));
			);

		if (lStatus == A_ERROR)
			{
			result = FALSE;
			break;
			}
		}

	return result;

	}

