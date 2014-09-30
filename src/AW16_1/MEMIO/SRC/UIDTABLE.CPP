#include <stddef.h>
#include "almsys.h"

#include "serdef.h"
#include "uidtable.h"
#include "rectypes.h"
#include "switches.h"


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UIDTABLE default constructor
//
// CLASS.......: UIDTABLE
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
UIDTABLE::UIDTABLE(
	void)
	{

	free = 1;    //don't use the zero id
	tableCount = 0;
	tablesHandle = 0;
	UIDTables = NULL;
	tableSize = MAXTABLESIZE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ~UIDTABLE destructor
//
// CLASS.......: UIDTABLE
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
UIDTABLE::~UIDTABLE()
	{
	FreeTableMemory();
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GenerateUID
//
// CLASS.......:  UIDTABLE
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
SERUID UIDTABLE::GenerateUID(
	void far*		pointer,
  long        recType)
	{
	SERUID			uid;
	U_PTRMATH_T	tableIndex;
	U_PTRMATH_T	pointerIndex;
  long int		prevFree;
  long int		currentFree;

	tableIndex = (U_PTRMATH_T)(this->free/tableSize);
	pointerIndex = (U_PTRMATH_T)(this->free%tableSize);

	if (tableIndex >= this->tableCount)
    {
  	uid.id = this->free;
    AllocateTable();
		this->free += 1;
    }
  else if ((recType != RT_OBJINST) && (recType != RT_EXEOBJINST))
    {
  	uid.id = this->free;
	  if (0 == (this->UIDTables[tableIndex])[pointerIndex].pointer)
		  this->free += 1;
	  else
		  this->free = (long)this->UIDTables[tableIndex][pointerIndex].pointer;
    }
  else  // don't reuse an object id for an object.
    {
    prevFree = -1;
    currentFree = this->free;
    while ((tableIndex < this->tableCount) &&
           (0 != (long)UIDTables[tableIndex][pointerIndex].pointer) &&
           (this->UIDTables[tableIndex][pointerIndex].flags & FLAG_WASOBJECT))
      {
      prevFree    = currentFree;
      currentFree = (long)UIDTables[tableIndex][pointerIndex].pointer;

      tableIndex   = (U_PTRMATH_T)(currentFree/tableSize);
	    pointerIndex = (U_PTRMATH_T)(currentFree%tableSize);
      }
    if (tableIndex >= this->tableCount)
      {
  	  uid.id = currentFree;
      AllocateTable();
		  UIDTables[(U_PTRMATH_T)(prevFree/MAXTABLESIZE)]
               [(U_PTRMATH_T)(prevFree%MAXTABLESIZE)].pointer =
       (void far *)(currentFree+1);
      }
    else
      {
  	  uid.id = currentFree;
      if (prevFree != -1)  //could not use the first free (this->free)
        {
	      if (0 == (this->UIDTables[tableIndex])[pointerIndex].pointer)
		      *((long*) &(UIDTables[(U_PTRMATH_T)(prevFree/MAXTABLESIZE)]
															 [(U_PTRMATH_T)(prevFree%MAXTABLESIZE)].pointer)) += 1;


        else
		      UIDTables[(U_PTRMATH_T)(prevFree/MAXTABLESIZE)]
                   [(U_PTRMATH_T)(prevFree%MAXTABLESIZE)].pointer =
           (void far *)(long)this->UIDTables[tableIndex][pointerIndex].pointer;
        }
      else                //used the first free
        {
	      if (0 == (this->UIDTables[tableIndex])[pointerIndex].pointer)
		      this->free = currentFree + 1;   //all frees are at end of table
        else
 		      this->free = (long)this->UIDTables[tableIndex][pointerIndex].pointer;
        }
      }
    }

	this->UIDTables[tableIndex][pointerIndex].pointer = pointer;
	this->UIDTables[tableIndex][pointerIndex].flags |= FLAG_USED;

 switch(recType)
   {
   case RT_OBJINST:
   case RT_EXEOBJINST:
     this->UIDTables[tableIndex][pointerIndex].flags |= FLAG_OBJECT;
     break;
   
   case RT_SERIUSBLOCK:
   case RT_RECORDBLOCK:
   case RT_EXESERIUSBLOCK:
   case RT_EXERECORDBLOCK:
   case RT_MEMBLOCK:
     this->UIDTables[tableIndex][pointerIndex].flags |= FLAG_RUNTIMEREC;
     break;
   
   default:
     break;
   }

	return uid;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	AllocateTable
//
// CLASS.......:  UIDTABLE
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
void UIDTABLE::AllocateTable(
  void)
  {

	this->tableCount += 1;

	if (this->tablesHandle)
		{
		GlobalUnlock(this->tablesHandle);
		this->tablesHandle = GlobalReAlloc(this->tablesHandle,
		 this->tableCount*sizeof(UIDENTRY), 0L);
		}
	else
		{
		this->tablesHandle = GlobalAlloc(GMEM_MOVEABLE,
		 this->tableCount*sizeof(UIDENTRY));
		}

	this->UIDTables = (UIDENTRY far* far* )GlobalLock(this->tablesHandle);
	this->UIDTables[(U_PTRMATH_T)(this->tableCount - 1)] =
	 (UIDENTRY far*)GlobalLock(GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,
		tableSize*sizeof(UIDENTRY)));

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ReleaseUID
//
// CLASS.......:  UIDTABLE
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
BOOL UIDTABLE::ReleaseUID(
	SERUID		uid)
	{
	U_PTRMATH_T		tableIndex;
	U_PTRMATH_T		pointerIndex;
  LPUIDENTRY		pUidEntry;

	if (uid.id > (tableCount * (long)tableSize))
		{
#ifdef DEBUG
		MessageBox(NULL, "Uid out of range in call to UIDTABLE::ReleaseUID",
		 "Yo, Programmer Dude!", MB_OK|MB_TASKMODAL);
    DebugBreak();
#endif
		return FALSE;
		}

	tableIndex   = (U_PTRMATH_T)(uid.id/tableSize);
	pointerIndex = (U_PTRMATH_T)(uid.id%tableSize);

  pUidEntry = &this->UIDTables[tableIndex][pointerIndex];

	if (!(pUidEntry->flags & FLAG_USED))
		{
#ifdef DEBUG
		MessageBox(NULL, "Unused (free) UID in call to UIDTABLE::ReleaseUID",
		 "Yo, Programmer Dude!", MB_OK|MB_TASKMODAL);
    DebugBreak();
#endif
		return FALSE;
		}

	pUidEntry->pointer = (void far *)this->free;
	pUidEntry->flags &= ~(FLAG_USED|FLAG_RUNTIMEREC);

  if (pUidEntry->flags & FLAG_OBJECT)
    {
    pUidEntry->flags |= FLAG_WASOBJECT;
    pUidEntry->flags &= (~FLAG_OBJECT);
    }
    
    

	this->free = uid.id;

	return TRUE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	SetPointer
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
BOOL UIDTABLE::SetPointer(
	SERUID			uid,
	void far *	thePointer)
	{
	U_PTRMATH_T 	tableIndex;
	U_PTRMATH_T	  pointerIndex;

	if (uid.id > (tableCount * (long)tableSize))
		{
#ifdef DEBUG
		MessageBox(NULL, "Uid out of range in call to UIDTABLE::SetPointer",
		 "Yo, Programmer Dude!", MB_OK|MB_TASKMODAL);
    DebugBreak();
#endif
		return FALSE;
		}

	tableIndex   = (U_PTRMATH_T)(uid.id/tableSize);
	pointerIndex = (U_PTRMATH_T)(uid.id%tableSize);

	if (!(UIDTables[tableIndex][pointerIndex].flags & FLAG_USED))
		{
#ifdef DEBUG
		MessageBox(NULL, "Unused (free) UID in call to UIDTABLE::SetPointer",
		 "Yo, Programmer Dude!", MB_OK|MB_TASKMODAL);
    DebugBreak();
#endif
		return FALSE;
		}

	this->UIDTables[tableIndex][pointerIndex].pointer = thePointer;

	return TRUE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: 	InitPointer
//
// CLASS.......:	UIDTABLE
//
// DESCRIPTION.:	This method is used to create and initialize the uid table
//								when a block of serius records are read from a file.  The
//								uid table is automatically grown to accommidate the value
//								of the specified uid.
//
// PARAMETERS..:  uid - the uid whose pointer value is to be initialized.
//								thePointer - the pointer value that corresponds to the uid.
//
// SIDE-EFFECTS:	This method does not affect the free list.  The free list
//                should be built after all pointers have been initialized.
/////////////////////////////////////////////////////////////////////////////
BOOL UIDTABLE::InitPointer(
	SERUID			uid,
	void far *	thePointer,
  long int    recType)
	{
	U_PTRMATH_T   tableIndex;
	U_PTRMATH_T	  pointerIndex;
  long int      cntNewTablesNeeded;
	long int      i;

	tableIndex   = (U_PTRMATH_T)(uid.id/tableSize);
	pointerIndex = (U_PTRMATH_T)(uid.id%tableSize);

	if (tableIndex >= this->tableCount)
    {
    cntNewTablesNeeded = tableIndex - this->tableCount + 1;
		if (this->tablesHandle)
			{
			GlobalUnlock(this->tablesHandle);
			this->tablesHandle = GlobalReAlloc(this->tablesHandle,
			 (tableIndex+1)*sizeof(UIDENTRY), 0L);
			}
		else
			{
			this->tablesHandle = GlobalAlloc(GMEM_MOVEABLE,
			 (tableIndex+1)*sizeof(UIDENTRY));
			}
		this->UIDTables = (UIDENTRY far* far* )GlobalLock(this->tablesHandle);
		for (i=this->tableCount; i < (cntNewTablesNeeded+this->tableCount); i++)
			{//allocate the new tables
			this->UIDTables[(U_PTRMATH_T)i] =
			 (UIDENTRY far*)GlobalLock(GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,
				tableSize*sizeof(UIDENTRY)));
			}
    this->tableCount = tableIndex +1;
    }
	else
		if (UIDTables[tableIndex][pointerIndex].flags & FLAG_USED)
			{
	#ifdef DEBUG
			MessageBox(NULL, "UID already used in call to UIDTABLE::InitPointer",
			 "Yo, Programmer Dude!", MB_OK|MB_TASKMODAL);
    DebugBreak();
	#endif
			return FALSE;
			}

	this->UIDTables[tableIndex][pointerIndex].pointer = thePointer;
	this->UIDTables[tableIndex][pointerIndex].flags |= FLAG_USED;

  switch(recType)
   {
   case RT_OBJINST:
   case RT_EXEOBJINST:
     this->UIDTables[tableIndex][pointerIndex].flags |= FLAG_OBJECT;
     break;
   
   case RT_SERIUSBLOCK:
   case RT_RECORDBLOCK:
   case RT_EXESERIUSBLOCK:
   case RT_EXERECORDBLOCK:
   case RT_MEMBLOCK:
     this->UIDTables[tableIndex][pointerIndex].flags |= FLAG_RUNTIMEREC;
     break;
   
   default:
     break;
   }

	return TRUE;

	}
 
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	BuildFreeList
//
// CLASS.......:  UIDTABLE
//
// DESCRIPTION.:	This routine is used to rebuild the list of free uids after
//                all uids have been initialized and all blocks of serius
//								records have been read from the file.
//
/////////////////////////////////////////////////////////////////////////////
void UIDTABLE::BuildFreeList(
	void)
	{
	U_PTRMATH_T   	tableIndex;
	U_PTRMATH_T   	pointerIndex;

  //mark the zero id as used so it won't get allocated.
  UIDTables[0][0].flags = FLAG_USED;

	free = (long)tableSize*tableCount;

	for (tableIndex=0; tableIndex<tableCount; tableIndex++)
		for (pointerIndex=0; pointerIndex<tableSize; pointerIndex++)
			{
			if (!(UIDTables[tableIndex][pointerIndex].flags & FLAG_USED))
				{
				UIDTables[tableIndex][pointerIndex].pointer = (void far *)free;
				free = (long)tableIndex*(long)tableSize + (long)pointerIndex;
				}
			}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SetTableSize
//
// CLASS.......:  UIDTABLE
//
// DESCRIPTION.:	This routine is used to rebuild the list of free uids after
//                all uids have been initialized and all blocks of serius
//								records have been read from the file.
//
/////////////////////////////////////////////////////////////////////////////
BOOL UIDTABLE::SetTableSize(
	WORD	newTableSize)
	{
	BOOL result = TRUE;

	if (tablesHandle == NULL)
		tableSize = newTableSize;
	else
		result = FALSE;

	return result;

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: FreeTableMemory
//
// CLASS.......: UIDTABLE
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void UIDTABLE::FreeTableMemory()
	{
	int i;
	HANDLE handle;

	for(i=0;i<tableCount;i++)
		{
		handle = (HANDLE) LOWORD(GlobalHandle(HIWORD(UIDTables[i])));
		GlobalUnlock(handle);
		GlobalFree(handle);
		}

	GlobalUnlock(tablesHandle);
	GlobalFree(tablesHandle);
	}

