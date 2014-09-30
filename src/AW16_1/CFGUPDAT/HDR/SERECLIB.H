#ifndef __SERECLIB_H__
#define __SERECLIB_H__

#include "serrec.h"
#include "recblk.h"
#include "uidtable.h"
#include "system.h"
#include "switches.h"

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

	return (SYSTEM::GetFileObject()->GetUIDTable()->GetPointer(uid));

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

	return (SYSTEM::GetFileObject()->GetUIDTable()->GetSeriusObject(uid));

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
	long int						count;

	return (*(LPRECORDBLOCK)GetObject(recBlockID))[index++];

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

	return ((LPRECORDBLOCK)GetObject(((LPRECORDPREFIX)GetObject(uid) - 1)->
	 recBlockUID))->type;

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
   (LPRECORDPREFIX)SYSTEM::GetFileObject()->GetUIDTable()->CheckUID(uid);

  if (pPrefix)
    {
    pPrefix--;

	  if (((LPRECORDBLOCK)GetObject(pPrefix->recBlockUID))->type == recordType)
      return TRUE;
    else
      return FALSE;
    }
  else
    return FALSE;

	}

#endif
