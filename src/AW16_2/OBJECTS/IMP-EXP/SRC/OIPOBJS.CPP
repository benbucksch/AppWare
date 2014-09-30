////////////////////////////////////////////////////////////////////////
// oipobjs.cpp -- OIP object/type manipulators
//
// jon 16-Aug-92 documented
// jon 26-Aug-92 AObjEdit now recognizes an errorMessage back
//               from called OBJD.
// jon 26-Oct-92 added oip<Export,Import>Object
////////////////////////////////////////////////////////////////////////

	#include "almsys.h"
	#include "intdebug.h"

	#include "fileobj.h"
	#include "exehead.h"
	#include "subject.h"
	#include "project.h"
	#include "compuid.h"
	#include "compdesc.h"
	#include "system.h"
	#include "rectypes.h"
	#include "objinst.h"
	#include "exeobjs.h"
	#include "exetypes.h"
	#include "tempobj.h"
	#include "flow.h"
	#include "exeinfo.h"
	#include "oipexe.h"
	#include "seruid.h"
	#include "objtype.h"
	#include "exeflow.h"
	#include "oiputils.h"
	#include "objgroup.h"

	static char  TXT_EDITING  [] = "Editing";
	static char  TXT_EXPORTING[] = "Exporting";
	static char  TXT_IMPORTING[] = "Importing";

////////////////////////////////////////////////////////////////////////
//  Local (static) functions
////////////////////////////////////////////////////////////////////////

 //--------------------------------------------------------------------
 //   GetSubjectPtr (local)
 //--------------------------------------------------------------------
	static SUBJECT * GetSubjectPtr()
		{
		PROJECT * pProjPtr;

		pTheSystem->GetCurrentContext();
		pProjPtr = (PROJECT *)GetObject(pTheSystem->GetFileObject()->GetHeadRec());

		return pProjPtr->activeSubject;
		}

	// helper routine which retrieves the type pointer if the type identified
	// by typeID is present in the active subject.
	static LPOBJTYPE GetSubjectTypePtr(
		TYPEID typeID)
		{
		LPSUBJECT pSubject;
		LPSUBTYPE pSubType;
		LPOBJTYPE pObjType=NULL;
		int i=0;

		pSubject = GetSubjectPtr();
		while (0 != (pSubType = (LPSUBTYPE)GetObject(pSubject->theSubTypes, i)))
			{
			if (pSubType->typeID == typeID)
				{
				pObjType = (LPOBJTYPE)GetObject(pSubType->objTypeUID);
				break;
				}
			}

		return pObjType;
		}

	static LPOBJTYPE GetProjectTypePtr(
		TYPEID typeID)
		{
		int					i									= 0;
		LPOBJTYPE		pObjType;
		LPPROJECT		pProject;
		RUNTIMEUID	recordBlock;

		pProject = (PROJECT *)GetObject(pTheSystem->GetFileObject()->GetHeadRec());
		recordBlock = pProject->GetTypeBlock();

		for (;;)
			{
			pObjType = (LPOBJTYPE)GetObject(recordBlock, i);

			if (pObjType == NULL || (typeID == pObjType->GetTypeID()))
				{
				if(pObjType)
					{
					if(pObjType->TestBadTypeFlag() ||
					(pObjType->GetAdmnProc() == NULL))
						{
						pObjType = NULL;
						}
					}
				break;
				}
			}

		return pObjType;
		}

 //--------------------------------------------------------------------
 //   splatErrorBox (local)
 //--------------------------------------------------------------------
	static void splatErrorBox(
		OBJECTID  theObject,
		LPSTR     messageText,
		LPSTR     action,
		LPSTR     optCap = 0)
		{
		char   cap[60];
		char   nam[33];
		long   cnt;

		if (optCap)
			lstrcpyn(cap, optCap, sizeof(cap));
		else if (theObject)
			{
			cnt = AObjGetName(theObject, nam, sizeof(nam));

			if (cnt+1 == sizeof(nam))
				{
				nam[sizeof(nam)-4] = '\0';
				lstrcat(nam, "...");
				}

			wsprintf(cap, "Error While %s Object ""%s""", action, (LPSTR)nam);
			}
		else
			wsprintf(cap, "Error While %s an Object", action);

		MessageBox(GetActiveWindow(),
							 messageText,
							 cap,
							 MB_ICONEXCLAMATION | MB_OK | MB_TASKMODAL);

		return;
		}

////////////////////////////////////////////////////////////////////////
//  Exported OIP functions
////////////////////////////////////////////////////////////////////////

extern "C" {

//----------------------------------------------------------------------
//   AObjGetObjectCount
//----------------------------------------------------------------------
long ALMAPI	AObjGetObjectCount(
	TYPEID typeID)
	{
	int             i=0;
	LPEXEOBJTYPE    pExeObjType;
	LPSUBTYPE       pSubType;
	SERUID          headExeRec;
	RUNTIMEUID      recordBlock;
	LPSUBJECT       pSubject;

	ADBG_VALIDATE(typeID != NULL, return 0, "Invalid type passed to AObjGetObjectCount");

	pTheSystem->GetCurrentContext();
	if (pTheSystem->IsExeStructures())
		{
		headExeRec = pTheSystem->GetFileObject()->GetHeadExeRec();
		recordBlock = ((LPEXEHEAD)GetObject(headExeRec))->GetExeObjTypeList();
		while (0 != (pExeObjType = (LPEXEOBJTYPE)GetObject(recordBlock, i)))
			if (typeID == pExeObjType->GetTypeID())
				return pExeObjType->GetTypeCount();
		}
	else
		{
		if (0 == (pSubject = GetSubjectPtr()))
			return 0L;
		recordBlock = pSubject->theSubTypes;
		while (0 != (pSubType = (LPSUBTYPE)GetObject(recordBlock, i)))
			if (typeID == pSubType->typeID)
				return pSubType->objCount;
		}
	return 0L;
	}


//----------------------------------------------------------------------
//   ATypeGetCount
//----------------------------------------------------------------------
long ALMAPI	ATypeGetCount(
	void
	)
  {
	LONG						lCount				= 0;
  SERUID          headExeRec;
  RUNTIMEUID      recordBlock;
	LPSUBJECT       pSubject;

  pTheSystem->GetCurrentContext();

	if (pTheSystem->IsExeStructures())
    {
		lCount = ATypeGetCountCfg();
    }
	else
		{
		if ((pSubject = GetSubjectPtr()) != NULL)
			{
			recordBlock = pSubject->theSubTypes;
			lCount = (long)((LPRECORDBLOCK)GetObject(recordBlock))->GetCount();
			}
    }

  return lCount;
  }

//----------------------------------------------------------------------
//   ATypeGetCountCfg
//----------------------------------------------------------------------
long ALMAPI	ATypeGetCountCfg(
	void
	)
  {
  SERUID          headExeRec;
  RUNTIMEUID      recordBlock;
	PROJECT *		pProjPtr;

  pTheSystem->GetCurrentContext();

	if (pTheSystem->IsExeStructures())
    {
    headExeRec = pTheSystem->GetFileObject()->GetHeadExeRec();
		recordBlock = ((LPEXEHEAD)GetObject(headExeRec))->GetExeObjTypeList();
    }
	else
		{
		pProjPtr = (PROJECT *)GetObject(pTheSystem->GetFileObject()->GetHeadRec());
		recordBlock = pProjPtr->GetTypeBlock();
    }

  return (long)((LPRECORDBLOCK)GetObject(recordBlock))->GetCount();

  }

//----------------------------------------------------------------------
//   AObjEdit
//
// Sets the fileobject's dirty flag if indeed edited.
//----------------------------------------------------------------------
LONG ALMAPI	AObjEdit(
	OBJECTID theObject)
	{
	LONG							lResult;
	LPOBJTYPE					pObjType;
	AObjMessage				theSystem;
	LPOBJECTRECORD		pObject;

	pTheSystem->GetCurrentContext();

	ADBG_VALIDATE(!pTheSystem->IsExeStructures(), return A_ERROR, "AObjEdit called at runtime");

	pObject = (LPOBJECTRECORD)(LPOBJINST)GetSeriusObject(theObject);

	ADBG_VALIDATE(pObject, return A_ERROR, "Object not found in AObjEdit");

	if (pObject->objectStatus & OBJ_ALIASFLAG)
		{
		splatErrorBox(theObject,
					 "This object is an alias."
					 " You can only edit the original shared object.", TXT_EDITING);
		return A_ERROR;
		}

	pObjType = GetSubjectTypePtr(pObject->typeID);

	ADBG_VALIDATE(pObjType, return A_ERROR, "Internal error:  cannot locate type record for object in AObjEdit");

	theSystem.message1 = AOBJ_EDITOBJECT;
	theSystem.message2 = 0;
	theSystem.message3 = 0;
	theSystem.message4 = 0;
	theSystem.errorMessage[0] = '\0';

	lResult = pObjType->admnProc(theObject, &theSystem);

	if(lResult == A_MODIFIED)
		pTheSystem->GetFileObject()->SetDirtyFlag();

	ADBG_EVAL(
		if (lResult == A_NOTHANDLED)
			ADbgPrint(ADBG_WARNING, 0, "Objects should handle AOBJ_EDITOBJECT");
		);

	return lResult;
	}

//----------------------------------------------------------------------
//   AObjExport
//
// Exports the specified object via ADMN/AOBJ_EXPORTOBJECT.
//----------------------------------------------------------------------
BOOL ALMAPI	AObjExport(
	OBJECTID theObject)
	{
	LPOBJTYPE					pObjType;
	AObjMessage				theSystem;
	LPOBJECTRECORD		pObject;
	LONG							lResult;

	pTheSystem->GetCurrentContext();

	ADBG_VALIDATE(!pTheSystem->IsExeStructures(), return FALSE, "AObjExport called at runtime");

	pObject = (LPOBJECTRECORD)(LPOBJINST)GetSeriusObject(theObject);

	ADBG_VALIDATE(pObject, return FALSE, "Object not found in AObjExport");

	if (pObject->objectStatus & OBJ_ALIASFLAG)
		{
		splatErrorBox(theObject,
					 "This object is an alias."
					 " You can't AObjExport() aliases.", TXT_EXPORTING);
		return FALSE;
		}

	pObjType = GetSubjectTypePtr(pObject->typeID);

	ADBG_VALIDATE(pObjType, return FALSE, "Internal error:  cannot find type record in AObjExport");

	I_ASSERT(pObjType->admnProc);

  theSystem.message1 = AOBJ_EXPORTOBJECT;
  theSystem.message2 = 0;
  theSystem.message3 = 0;
  theSystem.message4 = 0;
	theSystem.errorMessage[0] = '\0';

  lResult = pObjType->admnProc(theObject, &theSystem);

  return (lResult == A_OK || lResult == A_NOTHANDLED);
  }

//----------------------------------------------------------------------
//   AObjImport
//
// Imports the specified object via ADMN/AOBJ_IMPORTOBJECT.
//----------------------------------------------------------------------
BOOL ALMAPI	AObjImport(
	OBJECTID theObject)
  {
	LPOBJTYPE					pObjType;
	AObjMessage				theSystem;
	LPOBJECTRECORD		pObject;
	LONG							lResult;

  pTheSystem->GetCurrentContext();

	ADBG_VALIDATE(!pTheSystem->IsExeStructures(), return FALSE, "AObjImport called at runtime");

  pObject = (LPOBJECTRECORD)(LPOBJINST)GetSeriusObject(theObject);

	ADBG_VALIDATE(pObject, return FALSE, "Object not found in AObjExport");

	if (pObject->objectStatus & OBJ_ALIASFLAG)
		{
		splatErrorBox(theObject,
					 "This object is an alias."
					 " You can't AObjImport() aliases.", TXT_IMPORTING);
		return FALSE;
		}

	pObjType = GetSubjectTypePtr(pObject->typeID);

	ADBG_VALIDATE(pObjType, return FALSE, "Internal error:  cannot find type record in AObjImport");

	I_ASSERT(pObjType->admnProc);

  theSystem.message1 = AOBJ_IMPORTOBJECT;
  theSystem.message2 = 0;
  theSystem.message3 = 0;
  theSystem.message4 = 0;
	theSystem.errorMessage[0] = '\0';

  lResult = pObjType->admnProc(theObject, &theSystem);

  return (lResult == A_OK || lResult == A_NOTHANDLED);
	}

//----------------------------------------------------------------------
//   AObjGetNextObject
//----------------------------------------------------------------------
OBJECTID ALMAPI	AObjGetNextObject(
	TYPEID typeID,
	LPLONG theGetKey)
	{
	int           i;
	int						j;
	LPEXEOBJINST  pExeObjInst;
	LPEXEOBJTYPE  pExeObjType;
	LPOBJINST     pObjInst;
	SERUID        headExeRec;
	RUNTIMEUID    recordBlock;
	SERUID        objuid;
	LPSUBJECT     pSubject;
	LPOBJGROUP 		pGroup;
	LPSERUID			pObjBlock;

	ADBG_VALIDATE(typeID != NULL, return NULL, "Invalid TYPEID passed to AObjGetNextObject");
	ADBG_VALIDATE(theGetKey != NULL, return NULL, "Invalid getkey passed to AObjGetNextObject");

	pTheSystem->GetCurrentContext();
	if(pTheSystem->IsExeStructures())
		{
		i = 0;
		headExeRec = pTheSystem->GetFileObject()->GetHeadExeRec();
		recordBlock = ((LPEXEHEAD)GetObject(headExeRec))->GetExeObjTypeList();
		while (0 != (pExeObjType = (LPEXEOBJTYPE)GetObject(recordBlock, i)))
			{
			if(pExeObjType->GetTypeID() == typeID)
				{
				recordBlock = pExeObjType->GetExeObjInstList();

				if(*theGetKey == AOBJ_GETFIRSTKEY)
					i = 0;
				else
					i = (int)*theGetKey;

				if(0 != (pExeObjInst = (LPEXEOBJINST)GetObject(recordBlock, i)))
					{
					*theGetKey = i;
					return GetUID(pExeObjInst).id;
					}
				}
			}
		}
	else
		{
		//=****** the LOWORD of the key will be the group index
		//=****** the HIWORD of the key will be the object index in the group
		//This is safe because if you ever have more than 64K objects or groups,
		//you'll be hosed anyway.
		if(0 == (pSubject = GetSubjectPtr()))
			return 0L;
		
		//set i to the group index
		//set j to the object index
		if(*theGetKey == AOBJ_GETFIRSTKEY)
			{
			i = 0;
			j=0;
			}
		else
			{
			i = LOWORD (*theGetKey);
			j = HIWORD (*theGetKey);
			}

		/*!!! If the subject window is open, then we need to iterate
		 first through groups and then in object order in the group !!*/

		while(0 != (pGroup = (LPOBJGROUP)GetObject(pSubject->theObjGroups, i)))
			{
			I_ASSERT(j < pGroup->objCount);

			pObjBlock = 
				(LPSERUID)((LPSERIUSBLOCK)GetObject(pGroup->objIndexes))->GetData();
			
			I_ASSERT(pObjBlock);

			//Now search the group for the object
			//j should already be initialized to the object index
			for(;j<pGroup->objCount;j++)
				{
				pObjInst = (LPOBJINST)GetObject(pObjBlock[j]);

				I_ASSERT(pObjInst);

				if(pObjInst->typeID == typeID)
					{
					//j is zero based so we can only look at the next
					//record if j is less than objCount-1.
					if(j < pGroup->objCount - 1)
						{
						i--; //keep looking in the same group
						j++; //look at next item
						}
					else
						j=0; //start looking in the next group

					*theGetKey = MAKELONG(i, j);

					return GetUID(pObjInst).id;
					}
				}

			//if we reach here, we'll try looking at the next group so
			//reset j
			j = 0;
			}
		}
	return 0L;
	}

//----------------------------------------------------------------------
//   ATypeGetNext
//----------------------------------------------------------------------
TYPEID ALMAPI	ATypeGetNext(
	LPLONG theGetKey)
	{
	TYPEID				tNextType			= NULL;
	int           myKey;
	LPSUBTYPE     pSubType;
	RUNTIMEUID    recordBlock;

	ADBG_VALIDATE(theGetKey != NULL, return NULL, "Invalid getkey passed to ATypeGetNext");

	pTheSystem->GetCurrentContext();

	if (pTheSystem->IsExeStructures())
		{
		tNextType = ATypeGetNextCfg(theGetKey);
		}
	else
		{
		if (GetSubjectPtr())
			{
			recordBlock = GetSubjectPtr()->theSubTypes;
			if (*theGetKey == AOBJ_GETFIRSTKEY)
				myKey = 0;
			else
				myKey = (int)*theGetKey;
			if (0 != (pSubType = (LPSUBTYPE)GetObject(recordBlock, myKey)))
				{
				*theGetKey = myKey;
				tNextType = pSubType->typeID;
				}
			}
		}
	return tNextType;
	}

//----------------------------------------------------------------------
//   ATypeGetNextCfg
//----------------------------------------------------------------------
TYPEID ALMAPI	ATypeGetNextCfg(
	LPLONG theGetKey)
	{
	int           myKey;
	LPEXEOBJTYPE  pExeObjType;
	LPOBJTYPE			pObjType;
	SERUID        headExeRec;
	RUNTIMEUID    recordBlock;
	PROJECT *	pProjPtr;
	TYPEID				l_rval = 0;

	ADBG_VALIDATE(theGetKey != NULL, return NULL, "Invalid getkey passed to ATypeGetNextCfg");

	pTheSystem->GetCurrentContext();

	if (pTheSystem->IsExeStructures())
		{
		headExeRec = pTheSystem->GetFileObject()->GetHeadExeRec();
		recordBlock = ((LPEXEHEAD)GetObject(headExeRec))->GetExeObjTypeList();
		if (*theGetKey == AOBJ_GETFIRSTKEY)
			myKey = 0;
		else
			myKey = (int)*theGetKey;
		if (0 != (pExeObjType = (LPEXEOBJTYPE)GetObject(recordBlock, myKey)))
			{
			*theGetKey = myKey;
			l_rval = pExeObjType->GetTypeID();
			}
		}
	else
		{
		pProjPtr = (PROJECT *)GetObject(pTheSystem->GetFileObject()->GetHeadRec());
		recordBlock = pProjPtr->GetTypeBlock();

		if (*theGetKey == AOBJ_GETFIRSTKEY)
			myKey = 0;
		else
			myKey = (int)*theGetKey;
		while (0 != (pObjType = (LPOBJTYPE)GetObject(recordBlock, myKey)))
			{
			if((!pObjType->TestBadTypeFlag()) && 
				(pObjType->GetAdmnProc() != NULL))
		    {
		    l_rval = pObjType->GetTypeID();
				break;
				}
			}
		*theGetKey = myKey;
		}
	return l_rval;
	}


//----------------------------------------------------------------------
//   AObjGetName
//----------------------------------------------------------------------
long ALMAPI	AObjGetName(
	OBJECTID  theObject,
	LPSTR     theName,
	long      nameSize)
	{
	LPOBJINST    objPtr;

	ADBG_VALIDATE(theObject != NULL, return 0, "Invalid OBJECTID passed to AObjGetName");
	ADBG_VALIDATE(theName != NULL, return 0, "Invalid buffer passed to AObjGetName");
	ADBG_VALIDATE(nameSize > 0, return 0, "Invalid buffer size passed to AObjGetName");

	pTheSystem->GetCurrentContext();

	// Objects don't have names at run-time
	if (pTheSystem->IsExeStructures())
		{
		theName[0] = '\0';
		return 0;
		}

	objPtr = (LPOBJINST)GetSeriusObject(theObject);
	if (!objPtr)
		{
		ADBG_PRINT_W("Object not found in AObjGetName");
		theName[0] = '\0';
		return 0;
		}

	lstrcpyn(theName, objPtr->objectName, (size_t)nameSize);
	theName[(size_t)nameSize-1] = '\0';

	return lstrlen(theName);
	}


//----------------------------------------------------------------------
//   AObjGetType
//----------------------------------------------------------------------
TYPEID ALMAPI	AObjGetType(
	OBJECTID  theObject)
	{
	void  *      pObject;
	LPOBJECTRECORD  pObjectRecord;
	LPRECORDBLOCK   recblock;
	long            type;

	pTheSystem->GetCurrentContext();

  pObjectRecord = 0;

	if (theObject < 0)
    {
		pObjectRecord = TEMPOBJECT::GetObject(theObject);
		ADBG_VALIDATE(pObjectRecord, return 0,
                  "Invalid objectID passed to AObjGetType");
    }
	else
		{
		pObject = (LPRECORDPREFIX)GetSeriusObject((SERUID)theObject);

		ADBG_VALIDATE(pObject, return 0,
                  "Invalid objectID passed to AObjGetType");

    if (pObject != NULL)
      {
		  recblock = (LPRECORDBLOCK)GetObject(((LPRECORDPREFIX)pObject - 1)->recBlockUID);

		  ADBG_VALIDATE(recblock, return 0,
                    "Internal error:  cannot find type in AObjGetType");

		  if (recblock != 0)
        {
        type = recblock->GetType();
		    if (type == RT_EXEOBJINST)
			    pObjectRecord = (LPOBJECTRECORD)(LPEXEOBJINST)pObject;
		    else if (type == RT_OBJINST)
			    pObjectRecord = (LPOBJECTRECORD)(LPOBJINST)pObject;
        }
      }
		}

	return pObjectRecord ? pObjectRecord->typeID : 0;

	}

//----------------------------------------------------------------------
//   ATypeGetName
//----------------------------------------------------------------------
long ALMAPI	ATypeGetName(
	TYPEID  theType,
	LPSTR   theName,
	long    nameSize)
	{
	LPOBJTYPE  pObjType;
	SERUID     recordBlock;

	ADBG_VALIDATE(theName != NULL, return 0, "Invalid buffer passed to ATypeGetName");
	ADBG_VALIDATE(nameSize > 0, return 0, "Zero-size buffer passed to ATypeGetName");

	pTheSystem->GetCurrentContext();

	theName[0] = '\0';

	// Leave after theName[0] = '\0'
	ADBG_VALIDATE(theType != NULL, return 0, "Invalid TYPEID passed to ATypeGetName");

	if (pTheSystem->IsExeStructures())
		return 0L;

	pObjType = GetProjectTypePtr(theType);
	if(pObjType)
		{
		lstrcpyn(theName, pObjType->typeName, (size_t)nameSize);
		theName[(size_t)(nameSize-1)] = '\0';
		return lstrlen(theName);
		}

	return 0L;
	}


//----------------------------------------------------------------------
//   AObjCheckType
//----------------------------------------------------------------------
BOOL ALMAPI	AObjCheckType(
	OBJECTID theObject,
	TYPEID theType)
	{
	LPOBJECTRECORD  pObjectRecord;
	void  *      pObject;
	LPRECORDBLOCK   recblock;
	long            type;

	if (theObject < 0)
		pObjectRecord = TEMPOBJECT::GetObject(theObject);
	else
		{
		//Call GetObject rather then GetSeriusObject because we have to check the
		//uid type here anyway

		pObject = (LPRECORDPREFIX)GetObject((SERUID)theObject);

		if (!pObject)
			return FALSE;

	  recblock = (LPRECORDBLOCK)GetObject(((LPRECORDPREFIX)pObject - 1)->recBlockUID);
    if (!recblock)
      return FALSE;

		type = recblock->GetType();

		if (type == RT_EXEOBJINST)
			pObjectRecord = (LPOBJECTRECORD)(LPEXEOBJINST)pObject;
		else if (type == RT_OBJINST)
			{
			pObjectRecord = (LPOBJECTRECORD)(LPOBJINST)pObject;
			}
		else
			return FALSE;
		}

	if (!pObjectRecord)
		return FALSE;

	if (!theType)
		return TRUE;

	if (theType == pObjectRecord->typeID)
		return TRUE;

	return FALSE;
	}


//----------------------------------------------------------------------
//   AObjValidateID
//----------------------------------------------------------------------
BOOL ALMAPI	AObjValidateID(
	LPOBJECTID			pObjectID,
	pAObjMessage    pSystemRecord)
	{
	LPOBJINST		pObjInst;
	LPAPPCONTEXT		context;
	EXEINFOAUX	realAuxInfo, myAuxInfo;
	EXEINFO			realExeInfo(&realAuxInfo), myExeInfo(&myAuxInfo);
	int					i;
	BOOL				objectExists;

	ADBG_VALIDATE(pObjectID, return FALSE, "NULL LPOBJECTID passed to AObjValidateID");
	ADBG_VALIDATE(pSystemRecord, return FALSE, "NULL pAObjMessage passed to AObjValidateID");

	context = pTheSystem->GetCurrentContext();

	// ensure it's a valid system record and the passed objectID is the dying ID

	// If message4 is set to indicate compileTime, then zero out pObjectID if
	// the object does not exist. (This indicates that that particular object
	// was deleted.)

	if (pSystemRecord->message1 == AOBJ_VALIDATEOBJECTIDS)
		{
		if(pSystemRecord->message4 == validateProject)
			{
			//After pasting into a different subject, the pasted object must
			//validate any ids it's holding onto against the objects on the
			//clipBoard.
			//CAUTION: after setting up the current context as the context which
			// is passed in im message3, the only operations that are save are
			// to retrieve the clipboard object and examine its fields.
			//WARNING: no strings should be accesses or datas examined!

			//safety check to make sure a valid context was passed in
			if(pSystemRecord->message3 == 0)
				{
				*pObjectID=0;
				return TRUE;
				}

			context->GetCopyOfExeInfo(&realExeInfo);

			//register the clipboard object passed in in message3

			myExeInfo.flags                     = realExeInfo.flags;
			myExeInfo.pAuxInfo->fileObj         = (LPFILEOBJECT)pSystemRecord->message3;
			myExeInfo.pAuxInfo->seriusMem       = 0;
			myExeInfo.pAuxInfo->tempStringSpace = 0;

			exeRegisterContext(&myExeInfo);

			//cycle through the clipboard objects to see if an object can be found
			//whose old id matches the pObjectID
			objectExists = FALSE;
			i=0;
			while(0!=(pObjInst =(LPOBJINST)GetObject(
				(SERUID)pSystemRecord->message2, i)))
				{
				if(pObjInst->oldUID == *pObjectID)
					{
					//the clipboard object contains the object's new ID in its copyUID
					//field so go ahead and validate
					*pObjectID = pObjInst->copyUID.id;
					objectExists = TRUE; //the object exists on the clipboard
					break;
					}
				}
			exeRegisterContext(&realExeInfo);
			if(!objectExists)
				*pObjectID =0;

			//always return true since the ID will either be changed or zeroed out
			//Granted, there is a chance that the new ID will be the same as the
			//old ID but for this unusual case, who cares.
			return TRUE;
			}

		if (pSystemRecord->message4 == validateSubject)
			{
			//After a Paste make sure all referenced IDs are in the same subject
			pObjInst = (LPOBJINST)GetSeriusObject(*pObjectID);
			if ((!pObjInst) ||
					(pObjInst->subjectUID.id != pSystemRecord->message2))
				{
				*pObjectID = 0;
				return TRUE;
				}
			else
				return FALSE;
			}

		if (pSystemRecord->message4 == validateCompileTime)
			{
			//Check to see if the object exists - If not, zero out the ID since
			//the object has previously been deleted.
			//Note: since it's during compile time, we Know the id must point to
			//			an OBJINST. An object will only be told once to validate it's
			//			object id's after a compile.
			if (0 == (pObjInst = (LPOBJINST)GetSeriusObject((SERUID)*pObjectID)))
				{
				*pObjectID = 0;
				return TRUE;
				}

			//If the object is an alias but not a dummy alias,
			//set pObjInst to be the real object
			if(pObjInst->TestAliasFlag() && (pObjInst->sharedObjectUID != 0))
				{
				pObjInst = (LPOBJINST)GetSeriusObject(pObjInst->sharedObjectUID);
				if(!pObjInst)
					{
					*pObjectID = 0;
					return TRUE;
					}
				}
			*pObjectID = pObjInst->copyUID.id;
			return TRUE;
			}
		//Check to see the the object id is the id to be validated.
		//If so, change to the new ID.
		if (*pObjectID == pSystemRecord->message2)
			{
			*pObjectID = pSystemRecord->message3;
			return TRUE;
			}
		return FALSE;
		}
	else
		return FALSE;
	}

}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	AObjQueryProtocol
//
// DESC:			Sends a message to the admin proc for the type of the object.
//						The admin proc determines if the type supports the protocol.
//
////////////////////////////////////////////////////////////////

#define AP_ALLMODES		(AP_NOTSUPPORTED | AP_DYNAMIC | AP_SUPPORTED)

extern "C"
LONG ALMAPI	AObjQueryProtocol(
	OBJECTID			oid,
	pApQueryInfo	pQueryInfo,
	PROTOCOLID		protocol
	)
	{
	LONG					lResult		= AP_NOTSUPPORTED;
  ADMNPROC			admnProc;
	AObjMessage		systemRec;
  LPBASETYPE		pType;

	ADBG_VALIDATE(oid != NULL,        return AP_NOTSUPPORTED, "Invalid object passed to AObjQueryProtocol");
	ADBG_VALIDATE(pQueryInfo != NULL, return AP_NOTSUPPORTED, "Invalid QueryInfo passed to AObjQueryProtocol");
	ADBG_VALIDATE(protocol != NULL,   return AP_NOTSUPPORTED, "Invalid Protocol passed to AObjQueryProtocol");

	// Ensure that we are set up for this task
	pTheSystem->GetCurrentContext();

	pQueryInfo->size      = sizeof(ApQueryInfo);
	pQueryInfo->object    = oid;
	pQueryInfo->type      = 0;				// init to 0 for failure case
	pQueryInfo->protocol  = protocol;
	pQueryInfo->pfnCreate = NULL;

	systemRec.message1 = AOBJ_QUERYPROTOCOL;
	systemRec.message2 = 0;
	systemRec.message3 = 0;
	systemRec.message4 = (LONG) pQueryInfo;

	pType = GetObjectsTypeRecord(oid);

	ADBG_ASSERT_E(pType != NULL, ;, "Type not found for objectid passed to AObjQueryProtocol");

	if ((pType != NULL)                             &&
			((admnProc = pType->GetAdmnProc()) != NULL) &&
			(admnProc != NULL))
		{
		pQueryInfo->type = pType->GetTypeID();

		// Call ADMN:AOBJ_QUERYPROTOCOL
		lResult = admnProc(oid, &systemRec);

		if (lResult == A_NOTHANDLED)
			lResult = AP_NOTSUPPORTED;

		ADBG_EVAL(
			if ((lResult & ~AP_ALLMODES) != 0)
        ADbgPrint(ADBG_WARNING, 0, "[Type: \"%c%c%c%c\"] Invalid return value from AOBJ_QUERYPROTOCOL",
                  HIBYTE(HIWORD(pQueryInfo->type)),
                  LOBYTE(HIWORD(pQueryInfo->type)),
                  HIBYTE(LOWORD(pQueryInfo->type)),
                  LOBYTE(LOWORD(pQueryInfo->type))
                  );
			);

		ADBG_EVAL(
			if ((lResult & (AP_SUPPORTED | AP_NOTSUPPORTED)) ==
					(AP_SUPPORTED | AP_NOTSUPPORTED))
				ADbgPrint(ADBG_WARNING, 0, "AP_SUPPORTED & AP_NOTSUPPORTED both specified");
			);

		if ((lResult & AP_ALLMODES) == AP_DYNAMIC)	// indicates supported
			lResult |= AP_SUPPORTED;
		}

	return lResult;
	}

////////////////////////////////////////////////////////////////
//
// FUNCTION:	ATypeQueryProtocol
//
// DESC:			Sends a message to the admin proc of the type.
//						The admin proc determines if the type supports the protocol.
//
////////////////////////////////////////////////////////////////

extern "C"
LONG ALMAPI	ATypeQueryProtocol(
	TYPEID				tid,
	PROTOCOLID		protocol
	)
	{
	LONG					lResult		= AP_NOTSUPPORTED;
  ADMNPROC			admnProc;
	AObjMessage		systemRec;
  LPBASETYPE		pType;
	ApQueryInfo		queryInfo;

	ADBG_VALIDATE(tid != NULL,      return AP_NOTSUPPORTED, "Invalid type passed to AObjQueryProtocol");
	ADBG_VALIDATE(protocol != NULL, return AP_NOTSUPPORTED, "Invalid Protocol passed to AObjQueryProtocol");

	// Ensure that we are set up for this task
	pTheSystem->GetCurrentContext();

	queryInfo.size      = sizeof(ApQueryInfo);
	queryInfo.object    = NULL;
	queryInfo.type      = tid;
	queryInfo.protocol  = protocol;
	queryInfo.pfnCreate = NULL;

	systemRec.message1 = AOBJ_QUERYPROTOCOL;
	systemRec.message2 = NULL;
	systemRec.message3 = 0;
	systemRec.message4 = (LONG) &queryInfo;

	pType = GetTypeRecord(tid);

	ADBG_ASSERT_E(pType != NULL, ;, "Type not found for typeid passed to ATypeQueryProtocol");

	if ((pType != NULL)                             &&
			((admnProc = pType->GetAdmnProc()) != NULL) &&
			(admnProc != NULL))
		{
		// Call ADMN:AOBJ_QUERYPROTOCOL
		lResult = admnProc(NULL, &systemRec);

		if (lResult == A_NOTHANDLED)
			lResult = AP_NOTSUPPORTED;

		ADBG_EVAL(
			if ((lResult & ~AP_ALLMODES) != 0)
        ADbgPrint(ADBG_WARNING, 0, "[Type: \"%c%c%c%c\"] Invalid return value from AOBJ_QUERYPROTOCOL",
                  HIBYTE(HIWORD(tid)),
                  LOBYTE(HIWORD(tid)),
                  HIBYTE(LOWORD(tid)),
                  LOBYTE(LOWORD(tid))
                  );
			);

		ADBG_EVAL(
			if ((lResult & (AP_SUPPORTED | AP_NOTSUPPORTED)) ==
					(AP_SUPPORTED | AP_NOTSUPPORTED))
				ADbgPrint(ADBG_WARNING, 0, "AP_SUPPORTED & AP_NOTSUPPORTED both specified");
			);

		if ((lResult & AP_ALLMODES) == AP_DYNAMIC)	// indicates supportedx
			lResult |= AP_SUPPORTED;
		}

	return lResult;
	}


////////////////////////////////////////////////////////////////
//
// FUNCTION:	AObjAddRequiredFile
//
// message1 should contain the code AOBJ_REPORTREQUIREDFILES
// message2 should contain the memblock ID of where the
// fileName will be added.  
// message3 should contain the code AOBJ_INT_REPORTREQUIREDFILES
// If either of the above conditions are
// false or if an error occurs, FALSE will be returned.
//
////////////////////////////////////////////////////////////////
extern "C"
BOOL  ALMAPI  AObjAddRequiredFile(pAObjMessage theSystem, LPCSTR fileName)
	{
	BOOL rVal = FALSE;
	char* dataPtr;
	pImplicit_Link linkPtr;
	
	if((theSystem->message1 == AOBJ_REPORTREQUIREDFILES) &&
		(theSystem->message3 == AOBJ_INT_REPORTREQUIREDFILES) &&
		(theSystem->message2 != 0))
		{
		if(AMemSetSize(theSystem->message2, AMemGetSize(theSystem->message2) +
			sizeof(Implicit_Link)))
			{
			dataPtr = (char*)AMemLock(theSystem->message2);
			if(dataPtr)
				{
				linkPtr = (pImplicit_Link)(dataPtr + 
					AMemGetSize(theSystem->message2) - sizeof(Implicit_Link));

				// Need to force the initialization of the filename since
				// just allocating memory won't do it
				new (&linkPtr->fileName) SFilePath;
				linkPtr->fileName = fileName;
				AMemUnlock(theSystem->message2);
				rVal = TRUE;
				}
			else
				I_ASSERT(FALSE);

			}
		else
			I_ASSERT(FALSE);
		}
	else
		ADBG_PRINT_W("AObjReportRequiredFile called at an invalid time.");

	return rVal;
	}

