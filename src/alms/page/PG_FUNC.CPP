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
// Filename:  "pg_class.cpp"
//
// Description:
//  Page Layout control flow functions
//
////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>

#include "a_alm.h"
#include "o_number.h"
#include "pg_class.h"
#include "pg_macro.h"

#if (WINVER < 0x030a)
typedef int HFILE;
long  ReadFile(HFILE, void _huge*, long);
long  WriteFile(HFILE, const void _huge*, long);
#endif  // WINVER < 0x030a


// Param 1 Page Layout Object
extern "C" void ALMAPI ClearLayout ( pAEvtInfo /*evt*/, pAObjMessage sys )
{
	char 			errString[256];;
	AOBJECTID 	pageObject;
	DATA0*		data0;

	pageObject = AFuncGetTypedParameter(1, OTYPE_PAGE);
	if (pageObject == 0L)
	{
		AFuncReportResult(sys, 1, "Undefined Page Object");
		return;
	}

	data0 = LockPageData0(pageObject);

	data0->nextWindow 	= 0;
	data0->spooled 			= 0;
	data0->pages 				= 0;

	CloseHandle(data0->hFile);

//	if ( (data0->hFile = CreateFile/*!!PORT!! was _lcreat*/(data0->spoolFile, 0)) == -1)
	if ( (data0->hFile = CreateFile(data0->spoolFile, 
									GENERIC_READ | GENERIC_WRITE,
									0,
									NULL,
									CREATE_ALWAYS,
									FILE_ATTRIBUTE_NORMAL,
									NULL)) == (HANDLE)-1)

	{
		wsprintf(errString, "Cannot Create %s", data0->spoolFile);
		AFuncReportResult(sys, 1L, errString);
		return;
	}

	UnlockPageData0(pageObject);
	AFuncReportResult(sys, 1L, NULL);
}


// Param 1 Text Object
extern "C" void ALMAPI CountPages ( pAEvtInfo /*evt*/, pAObjMessage sys )
{
	DATA0*      data0;
	AOBJECTID		pageObject, numObject;

	if ( (pageObject = AFuncGetTypedParameter(1, OTYPE_PAGE)) != 0)
	{
		data0 = LockPageData0(pageObject);

		if ( (numObject = AObjCreate(OTYPE_NUMBER)) != 0)
		{
			ONmbrSetInteger(numObject, data0->pages, TRUE);
			AFuncSetParameter(1, numObject);
			AObjDestroy(numObject);
		}
	}
	UnlockPageData0(pageObject);

	AFuncReportResult(sys, 1, NULL);
}


// Param 1 Text Object
extern "C" void ALMAPI NextWindow ( pAEvtInfo /*evt*/, pAObjMessage sys )
{
	DATA0*    data0;
	DATA1*		data1;
	AOBJECTID	pageObject, windowObject;

	if ( (pageObject = AFuncGetTypedParameter(1, OTYPE_PAGE)) != 0)
	{
		data0 = (DATA0*)AObjLockData(pageObject, 0L);
		if (data0 == NULL)
		{
			AFuncReportResult(sys, 1, "Data 0 Of Page Layout NULL");
			return;
		}

		data1 = (DATA1*)AObjLockData(pageObject, 1L);
		if (data1 == NULL)
		{
			AFuncReportResult(sys, 1, "Data 1 Of Page Layout NULL");
			return;
		}
		windowObject = data1[data0->nextWindow].objid;
		AFuncSetParameter(1, windowObject);
	}
	UnlockPageData0(pageObject);
	UnlockPageData1(pageObject);

	AFuncReportResult(sys, 1, NULL);
}


extern "C" void ALMAPI SpoolWindow ( pAEvtInfo /*evt*/, pAObjMessage sys )
{
	AOBJECTID 	pageObject;
  PAGE*			page;

	pageObject = AFuncGetTypedParameter(1, OTYPE_PAGE);
	if (pageObject == 0L)
	{
		AFuncReportResult(sys, 1, "Undefined Page Object");
		return;
	}

	page = (PAGE*)new PAGE(pageObject);
	if (page->GetCount() == 0)
	{
		AFuncReportResult(sys, 1, "No Windows To Spool; Empty Window List");
  	return;
	}
	page->SpoolWindow(sys);
	page->IncrementNextWindow();
	page->IncrementPage();
	delete page;
}



// huge read/write information 
#if (WINVER < 0x030a) // use Windows API ReadFile and WriteFile for later versions

long  ReadFile(HFILE hFile, void _huge* buffer, long size)
{
	BYTE huge*  dest = (BYTE huge*)buffer;
  long				ret = 0L;

	while(size > 0x00008000)
	{
		ret 	+= (long)ReadFile(hFile, (BYTE*)dest, 0x8000);
   	size  -= 0x00008000;
   	dest  += 0x00008000;
  }
	ret += (long)ReadFile(hFile, (BYTE*)dest, (UINT)size);
  return ret;
}


long  WriteFile(HFILE hFile, const void _huge* buffer, long size)
{
	BYTE huge*  source = (BYTE huge*)buffer;
  long				ret = 0L;

	while(size > 0x00008000)
	{
   	ret 		+= (long)WriteFile(hFile, (BYTE*)source, 0x8000);
   	size    -= 0x00008000;
   	source  += 0x00008000;
  }
	ret += (long)WriteFile(hFile, (BYTE*)source, (UINT)size);
	return ret;
}

#endif  // WINVER < 0x030a


