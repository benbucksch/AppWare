////////////////////////////////////////////////////////////////////////
// oipowner.cpp -- OIP Routines to manage object owners
//
// bgb 07-Dec-92  created.
////////////////////////////////////////////////////////////////////////

	#include "almsys.h"

  #include "object.h"
  #include "system.h"
  #include "sereclib.h"
  #include "exeobjs.h"
  #include "rectypes.h"
  #include "recblk.h"


////////////////////////////////////////////////////////////////////////
//  Local (static) functions
////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
//   GetObjectRecord (local)
//--------------------------------------------------------------------
static LPEXEOBJINST GetExeObject(
  OBJECTID  theObject)
  {
  LPEXEOBJINST   pExeObject = 0;  //initialize return value
  void far *     pObject;
  long           type;

  if (theObject > 0)
    {
    //Call GetObject rather then GetSeriusObject because we have to check the
    //uid type here anyway
    pObject = GetObject((SERUID)theObject);
    if (pObject)
      {
      type = ((LPRECORDBLOCK)GetObject(((LPRECORDPREFIX)pObject - 1)->
	     recBlockUID))->GetType();
      if (type == RT_EXEOBJINST)
        pExeObject = (LPEXEOBJINST)pObject;
      }
    }

  return pExeObject;

  }

//--------------------------------------------------------------------
//   GetOwnerList (local)
//--------------------------------------------------------------------
static LPOWNERLIST GetOwnerList(
  OBJECTID  theObject)
  {
  LPOWNERLIST   pOwnerList = 0;  //initialize return value
  void far *    pObject;
  long          type;

  if (theObject > 0)
    {
    //Call GetObject rather then GetSeriusObject because we have to check the
    //uid type here anyway
    pObject = GetObject((SERUID)theObject);
    if (pObject)
      {
      type = ((LPRECORDBLOCK)GetObject(((LPRECORDPREFIX)pObject - 1)->
	     recBlockUID))->GetType();
      if (type == RT_EXEOBJINST)
        pOwnerList = &((LPEXEOBJINST)pObject)->ownerList;
      }
    }

  return pOwnerList;

  }

////////////////////////////////////////////////////////////////////////
//  OIP functions to be exported
////////////////////////////////////////////////////////////////////////

extern "C"

{

//----------------------------------------------------------------------
//   AObjRegisterOwner
//----------------------------------------------------------------------
BOOL ALMAPI	AObjRegisterOwner(
  OBJECTID  ownerObject,
  OBJECTID  ownedObject,
  LONG      ownerData)
  {
  LPOWNERLIST   pOwnerList;

  pTheSystem->GetCurrentContext();

  if (!GetExeObject(ownerObject))
   return FALSE;

  pOwnerList = GetOwnerList(ownedObject);

  if (!pOwnerList)
    return FALSE;

  return pOwnerList->AddOwner(ownerObject, ownerData);

  }

//----------------------------------------------------------------------
//   AObjUnregisterOwner
//----------------------------------------------------------------------
BOOL ALMAPI	AObjUnregisterOwner(
  OBJECTID  ownerObject,
  OBJECTID  ownedObject,
  LONG      ownerData)
  {
  LPOWNERLIST   pOwnerList;

  pTheSystem->GetCurrentContext();

  if (!GetExeObject(ownerObject))
   return FALSE;

  pOwnerList = GetOwnerList(ownedObject);

  if (!pOwnerList)
    return FALSE;

  return pOwnerList->RemoveOwner(ownerObject, ownerData);

  }

//----------------------------------------------------------------------
//   AObjGetNextOwner
//----------------------------------------------------------------------
BOOL ALMAPI	AObjGetNextOwner(
  TYPEID            ownerType,
  OBJECTID          ownedObject,
  pAObjOwnerInfo  pNextOwnerInfo)
  {
  LPOWNERLIST   pOwnerList;

  pTheSystem->GetCurrentContext();

  if (!pNextOwnerInfo)
    return FALSE;

  pOwnerList = GetOwnerList(ownedObject);

  if (!pOwnerList)
    return FALSE;

  return pOwnerList->GetNextOwner(ownerType, pNextOwnerInfo);

  }

}// end extern "C"
