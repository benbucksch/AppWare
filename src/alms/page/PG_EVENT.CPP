///////////////////////////////////////////////////////////////
//
//  (c) 1993 Novell, Inc.  All rights reserved.
//
//  THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//  TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//  COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//  CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//  TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//  NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//  AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//  LIABILITY.
//
///////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//
// Object Group:  Essentials
// Object:        Page
//
// Filename:  "pg_event.cpp"
//
// Description:
//  Page Event Routine
//
////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include "a_alm.h"
#include "pg_class.h"
#include "pg_macro.h"

#if (WINVER < 0x030a)
typedef int HFILE;
long  ReadFile(HFILE, void _huge*, long);
long  WriteFile(HFILE, const void _huge*, long);
#endif  // WINVER < 0x030a

extern "C" void ALMCALLBACK PageEvent_Startup(pAEvtInfo /*theEvent*/, pAObjMessage theSystem)
{
	AOBJECTID	pageObject;
	LONG			theKey;
	LPDATA0 	data0;

	theKey = AOBJ_GETFIRSTKEY;
	while (	(pageObject = AObjGetNextObject(OTYPE_PAGE, &theKey)) != 0)
	{
		data0 = LockPageData0(pageObject);
		if (data0 == NULL)
			continue;

		GetTempFileName(0, "spl", 0, data0->spoolFile);

//		data0->hFile = CreateFile(data0->spoolFile, 0);

		data0->hFile = CreateFile(data0->spoolFile, 
									GENERIC_READ | GENERIC_WRITE,
									0,
									NULL,
									CREATE_ALWAYS,
									FILE_ATTRIBUTE_NORMAL,
									NULL);

		if (data0->hFile == (HANDLE)-1)
		{
			data0->spoolFile[0] = 0;
			AEvtPostStandard(pageObject, AEVT_ATMARK, 0, AEVT_KILLDUPLICATES);
			AFuncReportResult(theSystem, 1L, "Cannot Open Spool File");
			return;
		}
		UnlockPageData0(pageObject);
	}
	AFuncReportResult(theSystem, 0L, NULL);
	return;
}


extern "C" void CALLBACK PageEvent_Shutdown(pAEvtInfo /*theEvent*/, pAObjMessage theSystem)
{
	AOBJECTID	pageObject;
	LONG			theKey;
	DATA0*		data0;

	theKey = AOBJ_GETFIRSTKEY;
	while (	(pageObject = AObjGetNextObject(OTYPE_PAGE, &theKey)) != 0)
	{
		data0 = LockPageData0(pageObject);
		if (data0 == NULL)
			continue;

		CloseHandle((HANDLE)data0->hFile);

		if (remove(data0->spoolFile) == -1)
		{
			data0->spoolFile[0] = 0;
			data0->hFile = 0;
			AFuncReportResult(theSystem, 0L, "Page Layout Cannot Remove Temporary File");
			return;
		}

		data0->spoolFile[0] = 0;
		data0->hFile = 0;

		UnlockPageData0(pageObject);
	}
	AFuncReportResult(theSystem, 0L, NULL);
	return;
}


extern "C" void CALLBACK PageEvent_PrintItem(pAEvtInfo theEvent, pAObjMessage theSystem)
{
	PAGE*	page;

	page = new PAGE(theEvent->targetObject, theEvent, theSystem);
	page->Pidr();
  delete page;
	
}
