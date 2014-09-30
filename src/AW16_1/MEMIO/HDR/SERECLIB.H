#ifndef __SERECLIB_H__
#define __SERECLIB_H__

#include "switches.h"
#include "seruid.h"
#include "system.h"
#include "fileobj.h"
#include "baserecb.h"
#include "rtreclib.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	GetObject
//
// CLASS.......:	(global)
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
inline void far *	GetObject(
	SERUID	uid)
	{

	return (pTheSystem->GetFileObject()->GetUIDTable()->GetPointer(uid));

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	GetSeriusObject
//
// CLASS.......:	(global)
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
inline void far *	GetSeriusObject(
	SERUID	uid)
	{

	return (pTheSystem->GetFileObject()->GetUIDTable()->GetSeriusObject(uid));

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	GetObject
//
// CLASS.......:	(global)
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
inline void far *	GetObject(
	SERUID    recBlockID,
	int far & index)
	{

	return (*(LPBASERECORDBLOCK)GetObject(recBlockID))[index++];

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: GetUID
//
// CLASS.......: (global)
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
inline SERUID GetUID(
	void far*	objPtr)
	{

	return ((RECORDPREFIX far*)objPtr - 1)->UID;

	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: GetUIDType
//
// CLASS.......: (global)
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
inline long GetUIDType(
	SERUID	 uid)
	{
 LPRECORDPREFIX  pRecPrefix;
 long            uidType;
 
 pRecPrefix = (LPRECORDPREFIX)pTheSystem->GetFileObject()->GetUIDTable()->
  GetNonRuntimeObject(uid); 
  
 if (pRecPrefix)
   {
   pRecPrefix--;
	  uidType = ((LPBASERECORDBLOCK)GetObject(pRecPrefix->recBlockUID))->GetType();
   }
 else
   uidType = 0;
   
 return uidType;  

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: CheckUID
//
// CLASS.......: (global)
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
inline BOOL CheckUID(
	SERUID	 uid,
  long     recordType)
	{
  LPRECORDPREFIX pPrefix;

  pPrefix =
   (LPRECORDPREFIX)pTheSystem->GetFileObject()->GetUIDTable()->CheckUID(uid);

  if (pPrefix)
    {
    pPrefix--;

	  if (((LPBASERECORDBLOCK)GetObject(pPrefix->recBlockUID))->GetType() ==
                                                                  recordType)
      return TRUE;
    else
      return FALSE;
    }
  else
    return FALSE;

	}

#endif
