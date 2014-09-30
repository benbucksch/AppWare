/*//////////////////////////////////////////////////////////////
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
// FILE: dt_main.cpp
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION:	Date object Main routine
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define DT_MAIN

#include "dt_incld.h"
#include "dt_dialg.h"

extern int _WinAllocFlag;

void RegisterEditSuperClass();

extern "C" int far pascal LibMain (HINSTANCE hIns, WORD	wDataSeg,	WORD cbHeapSize, LPSTR lpCmdLine)
{

	int 	month;
	int   day;

	I_UNUSED(wDataSeg);
	I_UNUSED(cbHeapSize);
	I_UNUSED(lpCmdLine);

	_WinAllocFlag = GMEM_SHARE;

	g_hInstance = (HINSTANCE)hIns;
	
	RegisterEditSuperClass();

	// Load month string for both long and short date 
	for(month=0; month<12; month++)
	{
		LoadString(g_hInstance, LONGMONTH_NAMES+month, g_lMonthNames[month], DAY_MONTH_STRING_LEN);
		LoadString(g_hInstance, SHORTMONTH_NAMES+month, g_sMonthNames[month], DAY_MONTH_STRING_LEN);
	}

	// load day string 
	for (day=0; day<7; day++)
	{
		LoadString(g_hInstance, LONGDAY_NAMES+day, g_lDayNames[day], DAY_MONTH_STRING_LEN);
		LoadString(g_hInstance, SHORTDAY_NAMES+day, g_sDayNames[day], DAY_MONTH_STRING_LEN);
	}


	GetProfileString("intl", "sShortDate", "m/dd/yyyy", g_sDateProfileString, sizeof(g_sDateProfileString) );
	GetProfileString("intl", "sLongDate", "dddd MMMM dd, yyyy", g_lDateProfileString, sizeof(g_lDateProfileString) );

	return TRUE;
}


extern "C" int far pascal _export WEP(int	nParameter)
{
	I_UNUSED(nParameter);

	UnregisterClass(g_dateObjectClassName, g_hInstance);
	return 1;
}


static void RegisterEditSuperClass()
{
	WNDCLASS	wc;

	GetClassInfo(NULL, "Edit", &wc);

	g_iThisPointerOffset = wc.cbWndExtra;
	g_defEditProc = wc.lpfnWndProc;

	wc.cbWndExtra += sizeof(LPVOID);
	wc.lpfnWndProc = AppwareDateObjectProc;
  wc.hInstance = g_hInstance;
	wc.lpszClassName = g_dateObjectClassName;

  RegisterClass(&wc);
}

