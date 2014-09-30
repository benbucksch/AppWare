/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:					BARFUNC.CPP
//
// AUTHOR:				Brian Brown
//
// DESCRIPTION:		Bar object function implementations
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <windows.h>
#include <a_alm.h>
#include <intdebug.h>
#include <o_wnd.h>
#include <o_menus.h>

#include "barobj.h"
#include "hmenumgr.h"


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Func_BarEnable
//
// DESCRIPTION.: Param 1  menuBar, required
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI Func_BarEnable(
  pAEvtInfo     evt,
  pAObjMessage  sys)
  {
	AOBJECTID	barObject;
  HMENUMGR* hmenuMgr;
  HMENU     hMenu;
  LPBARINFO pBarInfo;
  int       count;
  int       i;

  I_UNUSED(evt);

	barObject = AFuncGetTypedParameter(1, OTYPE_MENUBAR);

	if (0 == barObject)
	  {
		barObject = OWndGetActiveMenuBar();
	  }

  if (0 != barObject)
    {
    hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(barObject);
    hMenu = hmenuMgr->GetMenuHandle();
    if (0 != hMenu)
      {
      count = GetMenuItemCount(hMenu);
      for (i=0; i<count; i++)
        EnableMenuItem(hMenu, i, MF_ENABLED|MF_BYPOSITION);
      }
    pBarInfo = (LPBARINFO)AObjLockData(barObject, BAR_INFO_DATA);
    pBarInfo->status &= (~BAR_DISABLED);
    AObjUnlockData(barObject, BAR_INFO_DATA);
    AEvtPostStandard(barObject, AEVENT_OBJECTCHANGED, 0,
     AEvtGetObjectChangedPriority(AEVT_KILLDUPLICATES));
    OWndRedrawMenuBar(barObject);
    }

	AFuncReportResult(sys, 1L, NULL);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Func_BarDisable
//
// DESCRIPTION.: Param 1  menuBar, optional
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI Func_BarDisable(
  pAEvtInfo     evt,
  pAObjMessage  sys)
  {
	AOBJECTID	barObject;
  HMENUMGR* hmenuMgr;
  HMENU     hMenu;
  LPBARINFO pBarInfo;
  int       count;
  int       i;

  I_UNUSED(evt);

	barObject = AFuncGetTypedParameter(1, OTYPE_MENUBAR);

	if (0 == barObject)
	  {
		barObject = OWndGetActiveMenuBar();
	  }

  if (0 != barObject)
    {
    hmenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(barObject);
    hMenu = hmenuMgr->GetMenuHandle();
    if (0 != hMenu)
      {
      count = GetMenuItemCount(hMenu);
      for (i=0; i<count; i++)
        EnableMenuItem(hMenu, i, MF_GRAYED|MF_BYPOSITION);
      }
    pBarInfo = (LPBARINFO)AObjLockData(barObject, BAR_INFO_DATA);
    pBarInfo->status |= BAR_DISABLED;
    AObjUnlockData(barObject, BAR_INFO_DATA);
    AEvtPostStandard(barObject, AEVENT_OBJECTCHANGED, 0,
     AEvtGetObjectChangedPriority(AEVT_KILLDUPLICATES));
    OWndRedrawMenuBar(barObject);
    }

	AFuncReportResult(sys, 1L, NULL);

  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Func_BarShow
//
// DESCRIPTION.: Param 1 Bar object, optional
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI Func_BarShow(
  pAEvtInfo     evt,
  pAObjMessage  sys )
  {
	AOBJECTID    	barObject;

  I_UNUSED(evt);

	barObject = AFuncGetTypedParameter(1, OTYPE_MENUBAR);

  OWndShowMenuBar(barObject);

	AFuncReportResult(sys, 1L, NULL);

  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Func_BarHide
//
// DESCRIPTION.: Param 1 Bar object, optional
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI Func_BarHide(
  pAEvtInfo     evt,
  pAObjMessage  sys)
  {
	AOBJECTID    	barObject;

  I_UNUSED(evt);

	barObject = AFuncGetTypedParameter(1, OTYPE_MENUBAR);

  OWndHideMenuBar(barObject);

	AFuncReportResult(sys, 1L, NULL);

  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Func_BarInstall
//
// DESCRIPTION.: Param 1 Bar object, required
//               Param 2 Window object, optional
//
/////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI Func_BarInstall(
  pAEvtInfo     evt,
  pAObjMessage  sys)
  {
	AOBJECTID	windowObject;
	AOBJECTID  barObject;

  I_UNUSED(evt);

	barObject 	 = AFuncGetTypedParameter(1, OTYPE_MENUBAR);
	windowObject = AFuncGetTypedParameter(2, OTYPE_WINDOW);

	AFuncReportResult(sys, 1L, NULL);

  ADBG_ASSERT_E(barObject!=0, return,
   "Install Menu Bar: could not get first parameter");

  OWndSetMenuBar(barObject, windowObject);

  }


