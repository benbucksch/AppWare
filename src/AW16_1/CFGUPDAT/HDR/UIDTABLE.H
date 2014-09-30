#ifndef __UIDTABLE_H__
#define __UIDTABLE_H__

#include "serdef.h"
#include "seruid.h"

#define MAXTABLESIZE	1024

struct UIDENTRY
	{
	unsigned char	flags;
	void far *		pointer;
	};

typedef UIDENTRY far * LPUIDENTRY;

#define	FLAG_USED       0X01
#define FLAG_OBJECT     0X02
#define FLAG_WASOBJECT  0X04

class SERIUSRECORD;

class UIDTABLE
	{
	friend SERIUSRECORD;

	friend void far *	GetObject(
		SERUID	uid);

private:

	long 	 									free;
	long 	 									tableCount;
	HANDLE 									tablesHandle;
	UIDENTRY far* far* 			UIDTables;
	WORD										tableSize;

  void AllocateTable(
    void);

public:

	UIDTABLE(void);

	SERUID GenerateUID(
		void far*	pointer,
    long      rectype = 0);

	BOOL ReleaseUID(
		SERUID	uid);

	BOOL SetPointer(
		SERUID		uid,
		void far *	thePointer);

	BOOL InitPointer(
		SERUID		uid,
		void far*	thePointer,
    long      rectype = 0);

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
void far *	CheckUID(
	SERUID	uid)
	{
	U_PTRMATH_T  tableIndex;
	U_PTRMATH_T  pointerIndex;

	if ((uid.id > (tableCount * tableSize)) || (uid.id <= 0))
		{
		return 0;
		}

  tableIndex   = (U_PTRMATH_T)(uid.id/MAXTABLESIZE);
  pointerIndex = (U_PTRMATH_T)(uid.id%MAXTABLESIZE);

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
void far*	GetPointer(
	SERUID	uid)
	{
	U_PTRMATH_T  tableIndex;
	U_PTRMATH_T  pointerIndex;

	if ((uid.id > (tableCount * tableSize)) || (uid.id <= 0))
		{
#ifdef DEBUG
		MessageBox(NULL, "Uid out of range in call to GetPointer",
		 "Yo, Programmer Dude!", MB_OK);
    DebugBreak();
#endif
		return 0;
		}

  tableIndex   = (U_PTRMATH_T)(uid.id/MAXTABLESIZE);
  pointerIndex = (U_PTRMATH_T)(uid.id%MAXTABLESIZE);

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
// FUNCTION....: 	GetSeriusObject
//
// CLASS.......:	UIDTABLE
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void far*	UIDTABLE::GetSeriusObject(
	SERUID	uid)
	{
  UIDENTRY far*  pEntry;

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
    pEntry = &UIDTables[(U_PTRMATH_T)(uid.id/MAXTABLESIZE)][(U_PTRMATH_T)(uid.id%MAXTABLESIZE)];

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

typedef UIDTABLE far*	LPUIDTABLE;

#endif
