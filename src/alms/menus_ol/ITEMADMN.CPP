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
// FILE:					ITEMADMN.CPP
//
// AUTHOR:				Brian Brown
//                Russ Graves     28 March 1994    UPSF 
//
// DESCRIPTION:		Administration routine for menu item objects
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <windows.h>
#include <mem.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <a_alm.h>
#include <intdebug.h>
#include <op_wnd.h>
#include <o_wnd.h>
#include <o_menus.h>

#include "modinfo.h"
#include "itemlib.h"
#include "itemobj.h"
#include "itemdlg.h"
#include "hmenumgr.h"


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DoCreated
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
static long DoCreated(
  AOBJECTID  	objID)
  {
	char*	      default_name = "Menu Item";
  LPITEMINFO  pItemInfo;
  HMENUMGR*		pHMenuMgr = new	HMENUMGR(0L);	

  AObjSetRuntimeInfo(objID, (long)pHMenuMgr);

	AObjSetDataCount(objID, ITEM_DATAS_CNT);

  AObjResizeData(objID, ITEM_INFO_DATA, sizeof(ITEMINFO));

	pItemInfo = (LPITEMINFO)AObjLockData(objID, ITEM_INFO_DATA);

  pItemInfo->version		    = ITEM_VERSION;
  pItemInfo->status         = ITEM_NEVER_EDITED;
  pItemInfo->accelKey       = 0;
  pItemInfo->accelModifier  = 0;
  pItemInfo->itemID         = 0;

  AObjUnlockData(objID, ITEM_INFO_DATA);
  
	AObjSetData(objID, ITEM_TITLE_DATA, default_name, lstrlen(default_name)+1);

  return A_OK;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DoDestroyed
//
// DESCRIPTION.: create the runtime object to manage the menu.
//
/////////////////////////////////////////////////////////////////////////////
static long DoDestroyed(
  AOBJECTID  objID)
  {
  HMENUMGR* pHMenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(objID);

  delete pHMenuMgr;
  AObjSetRuntimeInfo(objID, 0L);	
  return A_OK;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DoAwakened
//
// DESCRIPTION.: create the runtime object to manage the menu.
//
/////////////////////////////////////////////////////////////////////////////
static long DoAwakened(
  AOBJECTID  objID)
  {
  HMENUMGR*	pHMenuMgr = new	HMENUMGR(0L);	

  AObjSetRuntimeInfo(objID, (long)pHMenuMgr);
  return A_CONTINUE;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DoAsleep
//
// DESCRIPTION.: delete runtime info
//
/////////////////////////////////////////////////////////////////////////////
static long DoAsleep(
  AOBJECTID  objID)
  {
  HMENUMGR* pHMenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(objID);

  delete pHMenuMgr;
  AObjSetRuntimeInfo(objID, 0L);	
  return A_CONTINUE;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DoAssign
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
static long DoAssign(
  AOBJECTID  destID,
  AOBJECTID  srcID)
  {
  void *   tempPtr;
  LPITEMINFO  pItemInfo;

  //Copy datinfo
  AObjSetData(destID, ITEM_INFO_DATA,
              tempPtr = AObjLockData(srcID, ITEM_INFO_DATA),
              sizeof(ITEMINFO));

  if (0 != tempPtr)
    AObjUnlockData(srcID, ITEM_INFO_DATA);

  pItemInfo = (LPITEMINFO)AObjLockData(destID, ITEM_INFO_DATA);

  pItemInfo->status &= (ITEM_NEVER_EDITED);

  AObjUnlockData(destID, ITEM_INFO_DATA);

  //copy  title string
  AObjSetData(destID, ITEM_TITLE_DATA,
              tempPtr = AObjLockData(srcID, ITEM_TITLE_DATA),
              AObjGetDataSize(srcID, ITEM_TITLE_DATA));

  if (0 != tempPtr)
    AObjUnlockData(srcID, ITEM_TITLE_DATA);


  //copy toggle title string
  AObjSetData(destID, ITEM_TOGGLE_TITLE_DATA,
              tempPtr = AObjLockData(srcID, ITEM_TOGGLE_TITLE_DATA),
              AObjGetDataSize(srcID, ITEM_TOGGLE_TITLE_DATA));

  if (0 != tempPtr)
    AObjUnlockData(srcID, ITEM_TOGGLE_TITLE_DATA);

	AEvtPostStandard(destID, AEVENT_OBJECTCHANGED, 0,
   AEvtGetObjectChangedPriority(AEVT_KILLPARTICULARS));

  return A_OK;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ExportObject
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
static long ExportObject (
  AOBJECTID  objID)
  {
  long        result = A_OK;
  LPITEMINFO  pItemInfo;
  LPSTR       szItemTitle;
  LPSTR       szItemTogTitle;
  BOOL        enabled;
  BOOL        togType;
  BOOL        togCheck;
  char        cmdKey[21] = "";
  char        accelKey[1];
  
  pItemInfo = (LPITEMINFO)AObjLockData(objID, ITEM_INFO_DATA);
  szItemTitle = (LPSTR)AObjLockData(objID, ITEM_TITLE_DATA);
  szItemTogTitle = (LPSTR)AObjLockData(objID, ITEM_TOGGLE_TITLE_DATA);

  enabled = !(pItemInfo->status & ITEM_DISABLED);
  togType = (BOOL)pItemInfo->status & ITEM_TOGGLE_TYPE;
  togCheck = (BOOL)pItemInfo->status & ITEM_SHOW_CHECKMARK;
  accelKey[1] = pItemInfo->accelKey;

  if (pItemInfo->accelModifier & OWND_SHIFT)
    lstrcat(cmdKey, "Shift+");

  if(pItemInfo->accelModifier & OWND_CTRL)
    lstrcat(cmdKey, "Ctrl+");

  lstrcat(cmdKey, g_cmdKeyToken[accelKey[1]]);


  I_ALWAYSASSERT(AUpsfExportField("VERSION", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, 
                 &pItemInfo->version, sizeof(int)) == AUPSF_NOERROR, result = A_ERROR);

	I_ALWAYSASSERT(AUpsfExportField("ITEMTITL", 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, 
	               szItemTitle, lstrlen(szItemTitle)) == AUPSF_NOERROR, result = A_ERROR);

	I_ALWAYSASSERT(AUpsfExportField("ENABLE", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_INTEGER, 
	               &enabled, sizeof(BOOL)) == AUPSF_NOERROR, result = A_ERROR);

	I_ALWAYSASSERT(AUpsfExportField("TOGTYPE", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_INTEGER, 
	               &togType, sizeof(BOOL)) == AUPSF_NOERROR, result = A_ERROR);

	if (szItemTogTitle != NULL)
		{
		I_ALWAYSASSERT(AUpsfExportField("TOGTEXT", 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, 
	  	             szItemTogTitle, lstrlen(szItemTogTitle)) == AUPSF_NOERROR, result = A_ERROR);
		}
	else
		{
		I_ALWAYSASSERT(AUpsfExportField("TOGTEXT", 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, 
	  	             "", lstrlen("")) == AUPSF_NOERROR, result = A_ERROR);
		}

	I_ALWAYSASSERT(AUpsfExportField("TOGCHECK", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_INTEGER, 
	               &togCheck, sizeof(BOOL)) == AUPSF_NOERROR, result = A_ERROR);

	I_ALWAYSASSERT(AUpsfExportField("CMNDKEY", 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, 
	               &cmdKey, lstrlen(cmdKey)) == AUPSF_NOERROR, result = A_ERROR);

  AObjUnlockData(objID, ITEM_INFO_DATA);
  if (szItemTitle)
    AObjUnlockData(objID, ITEM_TITLE_DATA);
  if (szItemTogTitle)
    AObjUnlockData(objID, ITEM_TOGGLE_TITLE_DATA);

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ImportObject
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
static long ImportObject(
  AOBJECTID  objID)
  {


	char      	buffer[256];
  long        result = A_OK;
  long        status = 0L;
  LPITEMINFO  pItemInfo;
  BOOL        enabled;
  BOOL        togType;
  BOOL        togCheck;
  
  pItemInfo = (LPITEMINFO)AObjLockData(objID, ITEM_INFO_DATA);

  I_ALWAYSASSERT(AUpsfImportField("VERSION", 0L, AUPSF_NATV_INT,  
                 &pItemInfo->version, sizeof(int)) == AUPSF_NOERROR, result = A_ERROR);

	I_ALWAYSASSERT(AUpsfImportField("ITEMTITL", 0L, AUPSF_NATV_STRING,  
	               buffer, 255) == AUPSF_NOERROR, result = A_ERROR);
  AObjSetData(objID, ITEM_TITLE_DATA, buffer, sizeof(buffer));

	I_ALWAYSASSERT(AUpsfImportField("ENABLE", 0L, AUPSF_NATV_BOOL, 
	               &enabled, sizeof(BOOL)) == AUPSF_NOERROR, result = A_ERROR);
  if (!enabled)                 
    status |= ITEM_DISABLED;
    
	I_ALWAYSASSERT(AUpsfImportField("TOGTYPE", 0L, AUPSF_NATV_BOOL,  
	               &togType, sizeof(BOOL)) == AUPSF_NOERROR, result = A_ERROR);
  if (togType)
    status |= ITEM_TOGGLE_TYPE;

	I_ALWAYSASSERT(AUpsfImportField("TOGTEXT", 0L, AUPSF_NATV_STRING, 
	               buffer, 255) == AUPSF_NOERROR, result = A_ERROR);
  AObjSetData(objID, ITEM_TOGGLE_TITLE_DATA, buffer, sizeof(buffer));

	I_ALWAYSASSERT(AUpsfImportField("TOGCHECK", 0L, AUPSF_NATV_BOOL, 
	               &togCheck, sizeof(BOOL)) == AUPSF_NOERROR, result = A_ERROR);
  if (togCheck)
    status |= ITEM_SHOW_CHECKMARK;
    
  pItemInfo->status = status;
  
	I_ALWAYSASSERT(AUpsfImportField("CMNDKEY", 0L, AUPSF_NATV_STRING, 
	               buffer, 255) == AUPSF_NOERROR, result = A_ERROR);

  ParseUpsf(buffer, pItemInfo->accelKey, pItemInfo->accelModifier); 

  AObjUnlockData(objID, ITEM_INFO_DATA);

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DoBuildSignals
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
static long DoBuildSignals(
  AOBJECTID    objID,
  AMEMBLOCKID  signalBlock)
  {
  LPITEMINFO         pItemInfo;
  long               blockSize;
  pAObjSignalRecord  pSignals;


  pItemInfo = (LPITEMINFO)AObjLockData(objID, ITEM_INFO_DATA);

  if ((pItemInfo->status & ITEM_TOGGLE_TYPE) == ITEM_TOGGLE_TYPE)
    blockSize = 3*sizeof(AObjSignalRecord);
  else
    blockSize = sizeof(AObjSignalRecord);

  AMemSetSize(signalBlock, blockSize);

  pSignals = (pAObjSignalRecord)AMemLock(signalBlock);

  pSignals[0].theID = SIGNAL_ITMCHOSEN;
  lstrcpy(pSignals[0].theName, "Chosen");

  if ((pItemInfo->status & ITEM_TOGGLE_TYPE) == ITEM_TOGGLE_TYPE)
    {
	  pSignals[1].theID = SIGNAL_ITMTOGON ;
	  lstrcpy(pSignals[1].theName, "Toggled On");

	  pSignals[2].theID = SIGNAL_ITMTOGOFF ;
	  lstrcpy(pSignals[2].theName, "Toggled Off");
    }

  AMemUnlock(signalBlock);

  AObjUnlockData(objID, ITEM_INFO_DATA);

  return A_OK;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: VerifyVersion
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
static long VerifyVersion(
  AOBJECTID     objID,
  pAObjMessage systemPtr)
	{
  LPITEMINFO  pItemInfo;
  long        result = A_OK;

	I_UNUSED(systemPtr);

  pItemInfo = (LPITEMINFO)AObjLockData(objID, ITEM_INFO_DATA);

	if(pItemInfo->version > ITEM_VERSION)
		{
		result = AObjReportError(objID, AOBJ_READ, A_ERROR,
     "Menu Item object \tser_menu.dll", 0);
		}

	if(pItemInfo->version < ITEM_VERSION)
		pItemInfo->version = ITEM_VERSION;

  AObjUnlockData(objID, ITEM_INFO_DATA);

  return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DoCheckObject
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
static long DoCheckObject(
  AOBJECTID  objID)
	{

  ValidateName(objID);

  return A_OK;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ItemAdministration
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
extern "C" LONG ALMAPI ItemAdministration(
  AOBJECTID     theObject,
  pAObjMessage systemPtr )
  {
  LONG    result;

	switch(systemPtr->message1)
		{
		case AOBJ_CREATED:
      result = DoCreated(theObject);
			break;

		case AOBJ_DESTROYED:
      result = DoDestroyed(theObject);
			break;
							 
		case AOBJ_AWAKENED:
			result= DoAwakened(theObject);
			break;

		case AOBJ_ASLEEP:
			result= DoAsleep(theObject);
			break;

		case AOBJ_ASSIGNOBJECT:
			result = DoAssign(theObject, (AOBJECTID)systemPtr->message3);
			break;

		case AOBJ_EXPORTOBJECT:
			result = ExportObject(theObject);
			break;

		case AOBJ_IMPORTOBJECT:
			result = ImportObject(theObject);
      break;

		case AOBJ_READ:
			result = VerifyVersion(theObject, systemPtr);
			break;

	  case AOBJ_EDITOBJECT:
      result = EditMenuItemObject(theObject);
		  break;

	  case AOBJ_BUILDSIGNALS:
      result = DoBuildSignals(theObject, (AMEMBLOCKID)systemPtr->message2);
		  break;

	  case AOBJ_CHECKOBJECT:
      result = DoCheckObject(theObject);
		  break;

    default:
      result = A_NOTHANDLED;
      break;
		}

  return result;

  }

