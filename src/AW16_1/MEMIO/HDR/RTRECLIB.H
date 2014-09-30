#ifndef __RTRECLIB_H__
#define __RTRECLIB_H__

#include "switches.h"
#include "system.h"
#include "fileobj.h"
#include "baserecb.h"
#include "rtrec.h"


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
inline LPRUNTIMEREC	GetObject(
	RUNTIMEUID	uid)
	{

	return (pTheSystem->GetFileObject()->GetUIDTable()->GetPointer(uid));

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
	RUNTIMEUID    recBlockID,
	int far &     index)
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
inline RUNTIMEUID GetUID(
  LPRUNTIMEREC	objPtr)
	{

	return objPtr->GetUID();

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
	RUNTIMEUID	 uid)
	{

	return ((LPBASERECORDBLOCK)GetObject(((LPRECORDPREFIX)GetObject(uid) - 1)->
	 recBlockUID))->GetType();

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
	RUNTIMEUID	 uid,
  long         recordType)
	{
  LPRUNTIMEREC  pRec;

  pRec =
   pTheSystem->GetFileObject()->GetUIDTable()->CheckUID(uid);

  if (pRec)
    {
	  if (pRec->GetType() == recordType)
      return TRUE;
    else
      return FALSE;
    }
  else
    return FALSE;

	}

#endif
