#ifndef __UIDTABLE_H__
#define __UIDTABLE_H__

#include "seruid.h"
#include "rtuid.h"

#define MAXTABLESIZE	1024

struct UIDENTRY
	{
	unsigned char	flags;
	void  *			pointer;
	};

typedef UIDENTRY  * LPUIDENTRY;

#define FLAG_USED       0X01
#define FLAG_OBJECT     0X02
#define FLAG_WASOBJECT  0X04
#define FLAG_RUNTIMEREC 0X08

class UIDTABLE
	{
	friend SERIUSRECORD;

	friend void  *	GetObject(
		SERUID	uid);

private:

	long 	 									free;
	long 	 									tableCount;
	HANDLE 									tablesHandle;
	UIDENTRY * * 							UIDTables;
	WORD										tableSize;

	void AllocateTable(
		void);

	void FreeTableMemory();

public:

	UIDTABLE(void);

  UIDTABLE(
	 UIDTABLE *  pSrc)
	 {
		free         = pSrc->free;
		tableCount   = pSrc->tableCount;
		tablesHandle = pSrc->tablesHandle;
		UIDTables    = pSrc->UIDTables;
	  tableSize    = pSrc->tableSize;
    }

	~UIDTABLE();

	SERUID GenerateUID(
		void *	pointer,
		long      rectype = 0);

	RUNTIMEUID GenerateUID(
		LPRUNTIMEREC	pointer,
		long          rectype = 0)
    {
    return (RUNTIMEUID)GenerateUID((void *)pointer, rectype).id;
    }

	BOOL ReleaseUID(
		SERUID	uid);

	BOOL ReleaseUID(
		RUNTIMEUID	uid)
    {
    return ReleaseUID((SERUID)uid.id);
    }

	BOOL SetPointer(
		SERUID		uid,
		void  *	thePointer);

	BOOL SetPointer(
		RUNTIMEUID	  uid,
		LPRUNTIMEREC	thePointer)
    {
    return SetPointer((SERUID)uid.id, (void  *)thePointer);
    }

	BOOL InitPointer(
		SERUID		uid,
		void *	thePointer,
		long      rectype = 0);

	BOOL InitPointer(
		RUNTIMEUID    uid,
		LPRUNTIMEREC	thePointer,
		long          rectype = 0)
    {
    return InitPointer((SERUID)uid.id, (void *)thePointer, rectype);
    }

	void BuildFreeList(
		void);

	BOOL SetTableSize(
		WORD	newTableSize);

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	GetObject
//
// CLASS.......:	UIDTABLE
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

LPRUNTIMEREC	CheckUID(
	RUNTIMEUID	uid)
  {
  return (LPRUNTIMEREC)CheckUID((SERUID)uid.id);
  }


void  *	CheckUID(
	SERUID	uid)
	{
	UINT  tableIndex;
	UINT  pointerIndex;

	if ((uid.id > (tableCount * tableSize)) || (uid.id <= 0))
		{
		return 0;
		}

	tableIndex   = (UINT)(uid.id/MAXTABLESIZE);
	pointerIndex = (UINT)(uid.id%MAXTABLESIZE);

	if (UIDTables[tableIndex][pointerIndex].flags & FLAG_USED)
		return UIDTables[tableIndex][pointerIndex].pointer;
	else
		{
		return 0;
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GetPointer
//
// CLASS.......:	UIDTABLE
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
LPRUNTIMEREC GetPointer(
	RUNTIMEUID	uid)
  {
  return (LPRUNTIMEREC)GetPointer((SERUID)uid.id);
  }



void *	GetPointer(
	SERUID	uid)
	{
	UINT  tableIndex;
	UINT  pointerIndex;

	if ((uid.id > (tableCount * tableSize)) || (uid.id <= 0))
		{
#ifdef DEBUG
		MessageBox(NULL, "Uid out of range in call to GetPointer",
		 "Yo, Programmer Dude!", MB_OK);
		DebugBreak();
#endif
		return 0;
		}

	tableIndex   = (UINT)(uid.id/MAXTABLESIZE);
	pointerIndex = (UINT)(uid.id%MAXTABLESIZE);

	if (UIDTables[tableIndex][pointerIndex].flags & FLAG_USED)
		return UIDTables[tableIndex][pointerIndex].pointer;
	else
		{
#ifdef DEBUG
		MessageBox(NULL, "Unused (free) uid in call to GetPointer",
		 "Yo, Programmer Dude!", MB_OK);
		DebugBreak();
#endif
		return 0;
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: GetNonRuntimeObject
//
// CLASS.......:	UIDTABLE
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void * GetNonRuntimeObject(
  SERUID  uid)
  {
  UIDENTRY *  pEntry = 0;
  void *      pObject = 0;

  if ((uid.id > (tableCount * tableSize)) || (uid.id <= 0))
		 {
#ifdef DEBUG
		 MessageBox(NULL, "Uid out of range in call to GetNonRuntimeObject",
		   "Internal Error", MB_OK);
      DebugBreak();
#endif
		 }
  else
    {
    pEntry = 
     &UIDTables[(UINT)(uid.id/MAXTABLESIZE)][(UINT)(uid.id%MAXTABLESIZE)];
	   if ((pEntry->flags & FLAG_USED) &&
	       !(pEntry->flags & FLAG_RUNTIMEREC))
      {
      if (!(pEntry->flags & FLAG_RUNTIMEREC))
        pObject = pEntry->pointer;
      }
    }

  return pObject;
    
  }
   
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	GetSeriusObject
//
// CLASS.......:	UIDTABLE
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void *	UIDTABLE::GetSeriusObject(
	RUNTIMEUID	uid)
  {
  return GetSeriusObject((SERUID)uid.id);
  }


void *	UIDTABLE::GetSeriusObject(
	SERUID	uid)
	{
  UIDENTRY *  pEntry;

	if ((uid.id > (tableCount * tableSize)) || (uid.id <= 0))
		{
#ifdef DEBUG
		MessageBox(NULL, "Uid out of range in call to GetSeriusPointer",
		 "Yo, Programmer Dude!", MB_OK);
    DebugBreak();
#endif
		return 0;
		}
  else
    pEntry = &UIDTables[(UINT)(uid.id/MAXTABLESIZE)][(UINT)(uid.id%MAXTABLESIZE)];

	if (pEntry->flags & FLAG_USED)
    {
    if (pEntry->flags &  FLAG_OBJECT)
      return pEntry->pointer;
	  else
		  {
#ifdef DEBUG
		  MessageBox(NULL, "Non-object uid in call to GetSeriusPointer",
		   "Yo, Programmer Dude!", MB_OK);
      DebugBreak();
#endif
		  return 0;
      }
    }
	else
		{
#ifdef DEBUG
		MessageBox(NULL, "Unused (free) uid in call to GetSeriusPointer",
		 "Yo, Programmer Dude!", MB_OK);
    DebugBreak();
#endif
		return 0;
		}

	}

	}; //end of class UIDTABLE

#endif
