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
// FILE:					ITEMFUNC.CPP
//
// AUTHOR:				Brian Brown
//
// DESCRIPTION:		Item object function implementations
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <windows.h>

#include "a_alm.h"
#include "intdebug.h"
#include "o_menus.h"
#include "itemobj.h"


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ItemDisable
//
// DESCRIPTION.: Param 1 - Menu Item Object
//
////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI ItemDisable(
  pAEvtInfo evt,
  pAObjMessage sys )
  {
  BOOL       goodParams = TRUE;
	AOBJECTID	 itemObject;
	LPITEMINFO pItemInfo;

  I_UNUSED(evt);

	itemObject = AFuncGetTypedParameter(1, OTYPE_MENUITEM);

	if (itemObject == 0)
    goodParams = FALSE;

  ADBG_EVAL(
	  if (FALSE == goodParams)
		  ADbgPrint(ADBG_WARNING, 0, "Bad Parameter: Item Disable");
  );

  if (TRUE == goodParams)
    {
	  pItemInfo = (LPITEMINFO)AObjLockData(itemObject, ITEM_INFO_DATA);
    pItemInfo->status |= ITEM_DISABLED;
    AObjUnlockData(itemObject, ITEM_INFO_DATA);
    AEvtPostStandard(itemObject, AEVENT_OBJECTCHANGED, 0,
     AEVT_ATTAIL|AEVT_KILLDUPLICATES);
    }

 	AFuncReportResult(sys, 1, "");

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ItemEnable
//
// DESCRIPTION.: Param 1 - Menu Item Object
//
////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI ItemEnable(
  pAEvtInfo evt,
  pAObjMessage sys )
  {
  BOOL       goodParams = TRUE;
	AOBJECTID	 itemObject;
	LPITEMINFO pItemInfo;

  I_UNUSED(evt);

	itemObject = AFuncGetTypedParameter(1, OTYPE_MENUITEM);

	if (itemObject == 0)
    goodParams = FALSE;

  ADBG_EVAL(
	  if (FALSE == goodParams)
		  ADbgPrint(ADBG_WARNING, 0, "Bad Parameter: Item Disable");
  );

  if (TRUE == goodParams)
    {
	  pItemInfo = (LPITEMINFO)AObjLockData(itemObject, ITEM_INFO_DATA);
    pItemInfo->status &= (~ITEM_DISABLED);
    AObjUnlockData(itemObject, ITEM_INFO_DATA);
    AEvtPostStandard(itemObject, AEVENT_OBJECTCHANGED, 0,
     AEVT_ATTAIL|AEVT_KILLDUPLICATES);
    }

 	AFuncReportResult(sys, 1, "");

  }



/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ItemToggleOn
//
// DESCRIPTION.: Param 1 - Menu Item Object
//
////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI ItemToggleOn(
  pAEvtInfo     evt,
  pAObjMessage  sys)
  {
  BOOL       goodParams = TRUE;
	AOBJECTID	 itemObject;
	LPITEMINFO pItemInfo;

  I_UNUSED(evt);

	itemObject = AFuncGetTypedParameter(1, OTYPE_MENUITEM);

	if (itemObject == 0)
    goodParams = FALSE;

  ADBG_EVAL(
	  if (FALSE == goodParams)
		  ADbgPrint(ADBG_WARNING, 0, "Bad Parameter: Item Disable");
  );

  if (TRUE == goodParams)
    {
	  pItemInfo = (LPITEMINFO)AObjLockData(itemObject, ITEM_INFO_DATA);
    if ((pItemInfo->status & ITEM_TOGGLE_TYPE) == ITEM_TOGGLE_TYPE)
      {
      pItemInfo->status |= ITEM_TOGGLED_ON;
      AEvtPostStandard(itemObject, AEVENT_OBJECTCHANGED, 0,
       AEVT_ATTAIL|AEVT_KILLDUPLICATES);
      }
    else
      {
      //report builder time debug error if not toggle type
      }
    AObjUnlockData(itemObject, ITEM_INFO_DATA);
    }
    
    //AEvtPostSignalAtMark(itemObject, SIGNAL_ITMTOGON);  Don't do this per Bob 25 Aug 94
   	AFuncReportResult(sys, 1, "");

  }
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ItemToggleOff
//
// DESCRIPTION.: Param 1 - Menu Item Object
//
////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI ItemToggleOff(
  pAEvtInfo     evt,
  pAObjMessage  sys)
  {
  BOOL       goodParams = TRUE;
	AOBJECTID	 itemObject;
	LPITEMINFO pItemInfo;

  I_UNUSED(evt);

	itemObject = AFuncGetTypedParameter(1, OTYPE_MENUITEM);

	if (itemObject == 0)
    goodParams = FALSE;

  ADBG_EVAL(
	  if (FALSE == goodParams)
		  ADbgPrint(ADBG_WARNING, 0, "Bad Parameter: Item Disable");
  );

  if (TRUE == goodParams)
    {
	  pItemInfo = (LPITEMINFO)AObjLockData(itemObject, ITEM_INFO_DATA);
    if ((pItemInfo->status & ITEM_TOGGLE_TYPE) == ITEM_TOGGLE_TYPE)
      {
      pItemInfo->status &= (~ITEM_TOGGLED_ON);
      AEvtPostStandard(itemObject, AEVENT_OBJECTCHANGED, 0,
       AEVT_ATTAIL|AEVT_KILLDUPLICATES);
      }
    else
      {
      //report builder-time debug error if not toggle type
      }
    AObjUnlockData(itemObject, ITEM_INFO_DATA);
    }

  //AEvtPostSignalAtMark(itemObject, SIGNAL_ITMTOGOFF);  Don't do this per Bob 25 Aug 94
 	AFuncReportResult(sys, 1, "");

  }



