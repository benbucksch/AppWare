/////////////////////////////////////////////////////////////////////////////
// File Name:  BrwsFunc.CPP
// 		Copyright 1989-92 Serius Corporation
// Author:  Paul Ruben
// Date:  8/19/92
//
// Description.:
//	Contains all Serius functions associated with the Serius Browser object:
//		CancelFunc
/////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <a_alm.h>
#include "browser.h"

///////////////////////////////////////////////////////////////////////
// FUNCTION:  CancelFunc
// DESCRIPTION:  Sets the Cancel flag in the Browser's datas so that
//	the methods normally performed by the browser button are cancelled.
//  Note: Since to Browser object input is provided to this function
//  this function enumerates all Browser objects and cancels their
//  default buttons behavior.  This is not currently a problem.  But it
//  may be a problem on a preemtive multitasking platform.
///////////////////////////////////////////////////////////////////////
void _export FAR PASCAL CancelFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	OBJECTID oiBrowser ;
	BrowserInfo far *lpBrowser ;
	long key ;

	key = AOBJ_GETFIRSTKEY ;
	while((oiBrowser = AObjGetNextObject((TYPEID)OTYPE_BROWSER, &key)) != NULL)
		{
		lpBrowser = (BrowserInfo far*)AObjLockData(oiBrowser, INFO_INDEX) ;
		lpBrowser->cancel = TRUE ;
		AObjUnlockData(oiBrowser, INFO_INDEX) ;
		}
	AFuncReportResult(theSystem, 1, "") ;
	}
