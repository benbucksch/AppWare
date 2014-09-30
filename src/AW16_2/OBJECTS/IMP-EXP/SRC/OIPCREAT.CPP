////////////////////////////////////////////////////////////////////////
// oipcreat.cpp -- OIP object creation functions
//
// jon 16-Aug-92 documented
////////////////////////////////////////////////////////////////////////

#include "almsys.h"
#include "intalm.h"

#include "exetypes.h"
#include "exeobjs.h"
#include "sereclib.h"
#include "tempobj.h"
#include "oiputils.h"
#include "oipdebug.h"
#include "context.h"

extern "C" {

//----------------------------------------------------------------------
//   AObjCreate
//----------------------------------------------------------------------
OBJECTID ALMAPI	AObjCreate(
  TYPEID  theType)
	{
	return AObjCreateWithProtocol(theType, 0);
	}

//----------------------------------------------------------------------
//   AObjCreateWithProtocol
//----------------------------------------------------------------------
OBJECTID ALMAPI	AObjCreateWithProtocol(
  TYPEID			theType,
	PROTOCOLID	theProtocol)
  {
  OBJECTID      objectID;
  LPTEMPOBJECT  pObject;
  LPEXEOBJTYPE  pType;
  AObjMessage		systemRec;
  ADMNPROC      admnProc;
	LPAPPCONTEXT			pContext;
	OBJCREATEPROC	pfnObjCreate;

	// Ensure that we are set up for this task
	pContext = pTheSystem->GetCurrentContext();

	if ((pfnObjCreate = pContext->GetObjCreateProc()) != NULL)
		{
		objectID = (*pfnObjCreate)(theType, theProtocol);
		}
	else
		{
		// Available at runtime only
		if (!pTheSystem->IsExeStructures())
			{
			ADBG_PRINT_W("AObjCreate() failed at runtime");
			return 0;
			}

		// Get a pointer to the internal type record
		pType = EXEOBJTYPE::GetObjectType(theType);
		if (!pType)
			return 0;

		// Get the ADMN for this type
		admnProc = pType->GetAdmnProc();
		if (!admnProc)
			return 0;

		// Create the temporary object in the temporary objects block
		pObject = new TEMPOBJECT(theType, ::GetUID(pType));

		// Call ADMN:AOBJ_CREATED
		objectID = FASTRECMGR::GetUID(pObject);
		systemRec.message1 = AOBJ_CREATED;
		systemRec.message2 = theProtocol;
		systemRec.message3 = 0;
		systemRec.message4 = AOBJ_TEMPORARY;
		admnProc(objectID, &systemRec);
		}

	// Return the OBJECTID of the temp object
  return objectID;
  }

//---------------------------------------------------------------------------
//   AObjDestroy
//---------------------------------------------------------------------------
BOOL ALMAPI	AObjDestroy(
  OBJECTID  theObject)
  {
  LPTEMPOBJECT   pObject;
	LPAPPCONTEXT			pContext;
	OBJDESTROYPROC	pfnObjDestroy;

 // Ensure that we are set up for this task
  pContext = pTheSystem->GetCurrentContext();

	if ((pfnObjDestroy = pContext->GetObjDestroyProc()) != NULL)
		{
		if(!(*pfnObjDestroy)(theObject))
			return FALSE; //object couldn't be destroyed
		}
	if (pTheSystem->IsExeStructures())
    {
	 // Available at runtime only
	 //Must be TEMPOBJECT, ie. uid < 0
		if (theObject >= 0)
			return FALSE;

	 // Get the internal object record
		pObject = TEMPOBJECT::GetObject(theObject);
		if (!pObject)
			return FALSE;

	 // Make sure it was created with AObjCreate()
		if (!pObject->TestTemporaryFlag())
			return FALSE;

	 // Delete the object if it's really free
		if (pObject->GetUseCount() < 0)
			{
			ADBG_EVAL(
				MessageBox(GetActiveWindow(),
								 "An impossible use count...",
								 "Important message from: oipDeleteObject",
								 MB_OK);
				DebugBreak();
				);
			}
		else if (pObject->GetUseCount() == 0
							 ||
						 pObject->DecrementUseCount() == 0)
			{
			LONG		lTmp	= 0;

			SendMessageToObject(AOBJ_DESTROYED, lTmp, lTmp, lTmp, theObject);

			ADBG_EVAL(
				BOOL	fLocked	= FALSE;

				for (lTmp = 0; lTmp < pObject->GetDatasCount(); lTmp++)
					{
					if (pObject->GetDataLockCount(lTmp) != 0)
						{
						PrintObjDataInfo(theObject, lTmp);
						fLocked = TRUE;
						}
					}
				if (fLocked)
					ADbgPrint(ADBG_ERROR, 0, "Deleting object with locked datas");
				);

			delete pObject;
			}
		}

  return TRUE;
  }


//---------------------------------------------------------------------------
//   AObjSetName
//---------------------------------------------------------------------------
BOOL ALMAPI	AObjSetName(
	OBJECTID  theObject,
	LPCSTR newName)
  {
	LPAPPCONTEXT			pContext;
	OBJSETNAMEPROC	pfnObjSetName;
	BOOL result = FALSE;

 // Ensure that we are set up for this task
  pContext = pTheSystem->GetCurrentContext();

	if ((pfnObjSetName = pContext->GetObjSetNameProc()) != NULL)
		{
		result = (*pfnObjSetName)(theObject, newName);
		}

	return result;
	}

//----------------------------------------------------------------------
//   AObjAssign
//
// Returns TRUE if the transfer occurred.
//----------------------------------------------------------------------
BOOL ALMAPI	AObjAssign(
  OBJECTID  fromObject,
  OBJECTID  toObject)
  {
  TYPEID        toType;
  TYPEID        fromType;
  AObjMessage  systemRec;
  ADMNPROC      admnProc;
  MEMBLOCKID    bufferID;
  BOOL          itHappened;

 // Ensure that we are set up for this task
  pTheSystem->GetCurrentContext();

 // Available at runtime only
  if (!pTheSystem->IsExeStructures())
    return FALSE;

 // Get the FROM object's type
  fromType = AObjGetType(fromObject);
  if (!fromType)
    return FALSE;

 // Get the TO object's type
  toType = AObjGetType(toObject);
  if (!toType)
    return FALSE;

 //-----------------------------------------------------------------
 // The easy case: TO type == FROM type
 //
 //   Just use the type's ADMN:AOBJ_ASSIGNOBJECT to do the work.
 //-----------------------------------------------------------------

  if (toType == fromType)
    {
    admnProc = EXEOBJTYPE::GetObjectType(toType)->GetAdmnProc();
    if (!admnProc)
      return FALSE;

    systemRec.message1 = AOBJ_ASSIGNOBJECT;
    systemRec.message2 = 0;
    systemRec.message3 = fromObject;
    systemRec.message4 = 0;
    itHappened = admnProc(toObject, &systemRec);

    if(itHappened == A_OK || itHappened == A_WARNING)
			return TRUE;
		else
			return FALSE;
    }

 //-----------------------------------------------------------------
 // The hard case: TO type != FROM type
 //
 //   Attempt a transfer via the object-text interchange.  Will
 //   fail if either the TO type or the FROM type do not support
 //   object-text interchange.
 //-----------------------------------------------------------------

 // Grab a buffer to hold the TEXT version of the FROM object
  bufferID = AMemAllocate(0);
  if (!bufferID)
    return FALSE;

     //-------------------------------------------//
     // NOTE: all return statements from here on  //
     //       out must AMemFree the buffer...      //
     //-------------------------------------------//

 // Try to get the FROM object's TEXT representation...
 // ...then shove it to the TO object
  if (AObjConvertToText(fromObject, bufferID)
        &&
      AObjConvertFromText(toObject, bufferID))
    {
    itHappened = TRUE;
    }
  else
    {
    itHappened = FALSE;
    }

  AMemFree(bufferID);
  return itHappened;
  }


//----------------------------------------------------------------------
//   AObjConvertToText
//----------------------------------------------------------------------
BOOL ALMAPI		AObjConvertToText(
  OBJECTID    theObject,
  MEMBLOCKID  theText)
  {

 // Ensure that we are set up for this task
  pTheSystem->GetCurrentContext();

 // Available at runtime or stop-time only
  if (pTheSystem->IsExeStructures())
    return AEvtCallMethod(0, theObject, 0, AEVENT_OBJECTTOTEXT, theText);
  else
    return FALSE;
  }


//----------------------------------------------------------------------
//   AObjConvertFromText
//----------------------------------------------------------------------
BOOL ALMAPI AObjConvertFromText(
  OBJECTID    theObject,
  MEMBLOCKID  theText)
  {

 // Ensure that we are set up for this task
  pTheSystem->GetCurrentContext();

 // Available at runtime only
  if (!pTheSystem->IsExeStructures())
    return FALSE;

  return AEvtCallMethod(0, theObject, 0, AEVENT_TEXTTOOBJECT, theText);

  }

} // extern "C"
