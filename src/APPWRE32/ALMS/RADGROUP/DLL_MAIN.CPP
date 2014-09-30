//////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////

#define		STRICT
#define		ASTRICT

#include <windows.h>
#include <stdlib.h>
#include <string.h>

#define _MAIN_MODULE
#include "a_alm.h"
#include "intdebug.h"
#include "globals.h"

//extern int _WinAllocFlag;

//extern "C" int CALLBACK /*!!PORT!! was _far_ _pascal_*/  LibMain/*!!PORT!! Replace with DllEntryPoint */ (HINSTANCE hIns, WORD wDataSeg, WORD cbHeapSize, LPSTR lpCmdLine);

//extern "C" int CALLBACK /*!!PORT!! was _far_ _pascal_*/  LibMain/*!!PORT!! Replace with DllEntryPoint */ (
//	HINSTANCE			hIns,
//	WORD 				wDataSeg,
//	WORD 				cbHeapSize,
//	LPSTR				lpCmdLine)
//	{
//	_WinAllocFlag = GMEM_SHARE;
//	hInstance = hIns;
//	// userDS = LoadLibrary("user.exe");
//	windowsVersion = LOWORD( (LONG)GetVersion() );
//	I_UNUSED(wDataSeg);
//	I_UNUSED(cbHeapSize);
//	I_UNUSED(lpCmdLine);
//
//	return TRUE;
//	}
//////////////////////////////////////////////////////////////////////

extern "C"
BOOL WINAPI   DllEntryPoint(
	HINSTANCE			hInstDLL,
	DWORD					dwReason,
	LPVOID				pReserved
	)
	{
	I_UNUSED(pReserved);

	hInstance = hInstDLL;
	windowsVersion = LOWORD( (LONG)GetVersion() );

	switch (dwReason)
		{
		case DLL_PROCESS_ATTACH:
			break;

		case DLL_PROCESS_DETACH:
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;

#ifdef AW_I_DEBUG
		default:
			I_ASSERT(0);
#endif
		}

	return TRUE;
	}



//////////////////////////
// Utility functions
////////////////////////// 
int Object_in_items_array(
  AOBJECTID      object,
  LPOBJECTID    items,
  int           numItems)
  {
  int   i;

  for(i=0; i<numItems; i++, items++)
    {
    if (*items == object) return i;
    }
  return -1;
	}

////////////////////////////////////////////////////
//  numItem -- number of items in the array       //
//  idx     -- index of the item to remove        //
//  size    -- size of a single item              //
//  array   -- the array                          //
//  return value -- the new pointer to the array  //
////////////////////////////////////////////////////
BOOL EraseItemFromArray(
  int     idx,
  int     size,
  int     max,
  LPVOID  array)
  {
  LPSTR  a = (LPSTR)array;

  if (idx > max)
    {
    MessageBox(NULL, "idx > max in EraseItemFromArray", "Logic Error", MB_OK);
    return FALSE;
    }

  if(!max)
    return FALSE;

  if (idx < max )
    CopyMemory( (LPVOID)&a[idx*size], (void  *)&a[(idx+1)*size], (max-idx)*size);

  return TRUE;
  }


////////////////////////

void PostObjectChanged(
  AOBJECTID     theObject,
	LONG         modifiers, //defaults to 0
	LONG				 how,       //defaults to 0
	LONG 				 time)      //defaults to 0
	{
	AEvtInfo   event;

  event.sourceObject = theObject;
  event.targetObject = theObject;
	event.when         = time ? time : AAppGetTime();
	event.how          = how ? how : AEvtGetObjectChangedPriority(AEVT_KILLDUPLICATES);
  event.what         = AEVENT_OBJECTCHANGED;
  event.details      = NULL;
  event.modifiers    = modifiers;

  AEvtPost(&event);
  return;
	}




