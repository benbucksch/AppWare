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
// FILE:					MENUADMN.CPP
//
// AUTHOR:				Brian Brown
//                Russ Graves     29 March 1994    UPSF
//
// DESCRIPTION:   Menu object administration routine
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <windows.h>
#ifdef __BORLANDC__
	#include "mem.h"
#else
	#include	"memory.h"
#endif
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <a_alm.h>
#include <intdebug.h>
#include <op_wnd.h>

#include "menuobj.h"
#include "menulib.h"
#include "menudlg.h"
#include "o_menus.h"
#include "menu_vi.h"
#include "hmenumgr.h"


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void SetupTypeData(
  void)
  {
  MENUTYPEINFO  typeInfo;

  if (ATypeGetDataCount(OTYPE_MENU) == 0)
    {
    typeInfo.idSeed = 0;
    ATypeAddData(OTYPE_MENU, &typeInfo, sizeof(MENUTYPEINFO));
    }

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DoCreate
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static long DoCreated(
  AOBJECTID    objID,
  APROTOCOLID  protID)
  {
  LPMENUINFO  pMenuInfo;
 	HMENUMGR*	pHMenuMgr = new	HMENUMGR(0L);	

  AObjSetRuntimeInfo(objID, (long)pHMenuMgr);

  SetupTypeData();

	AObjSetDataCount(objID, MENU_DATAS_CNT);

  AObjResizeData(objID, MENU_INFO_DATA, (long)sizeof(MENUINFO));

  pMenuInfo = (LPMENUINFO)AObjLockData(objID, MENU_INFO_DATA);

  pMenuInfo->status = MENU_NEVER_EDITED;

	pMenuInfo->version		= MENU_VERSION;
	pMenuInfo->itemCount  = 0;
	pMenuInfo->sendText   = 0L;
	pMenuInfo->sendIndex  = 0L;
	pMenuInfo->popIndex		= 0L;

  AObjUnlockData(objID, MENU_INFO_DATA);
  
  if(APROT_WND == protID)
    { // create as a popup menu
    pMenuInfo->status |= MENU_POPUP;    
    }
 
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
// FUNCTION....: DoDestroyed
//
// DESCRIPTION.: delete runtime info and set data count to zero
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
  AOBJECTID  dstObject,
  AOBJECTID  srcObject)
  {
  UINT		    i, j;
	UINT		    copyCount;
	LPMENUINFO	srcInfo;
	LPMENUINFO	dstInfo;
  LPITEMENTRY srcItemList;
  LPITEMENTRY dstItemList;
  LPSTR       szSrcTitle;

	srcInfo = (LPMENUINFO)AObjLockData(srcObject, 0L);
	dstInfo = (LPMENUINFO)AObjLockData(dstObject, 0L);

	dstInfo->popIndex = srcInfo->popIndex;
	dstInfo->sendText = srcInfo->sendText;
	dstInfo->sendIndex = srcInfo->sendIndex;

  szSrcTitle = (LPSTR)AObjLockData(srcObject, MENU_TITLE_DATA);

  if (0 != szSrcTitle)
    {
    AObjSetData(dstObject, MENU_TITLE_DATA, szSrcTitle, lstrlen(szSrcTitle)+1);
    AObjUnlockData(srcObject, MENU_TITLE_DATA);
    }

	// copy all children items except text only

	srcItemList = (LPITEMENTRY)AObjLockData(srcObject, MENU_ITEMLIST_DATA);

  copyCount = 0;
	for (i=0; i<srcInfo->itemCount; i++)
	  {
		if ((srcItemList[i].status & ITEMENTRY_TEXTONLY) != ITEMENTRY_TEXTONLY)
      copyCount++;
	  }

	AObjResizeData(dstObject, MENU_ITEMLIST_DATA, copyCount*sizeof(ITEMENTRY));

  dstItemList = (LPITEMENTRY)AObjLockData(dstObject, MENU_ITEMLIST_DATA);

	for (i = 0, j = 0; i < srcInfo->itemCount; i++)
	  {
		if ((srcItemList[i].status & ITEMENTRY_TEXTONLY) != ITEMENTRY_TEXTONLY)
		  dstItemList[j++] = srcItemList[i];
    }

	dstInfo->itemCount = (WORD)copyCount;

	AEvtPostStandard(dstObject, AEVENT_OBJECTCHANGED, 0,
   AEvtGetObjectChangedPriority(AEVT_KILLPARTICULARS));

	AObjUnlockData(srcObject, MENU_INFO_DATA);
	AObjUnlockData(dstObject, MENU_INFO_DATA);
  if (0 != srcItemList)
    AObjUnlockData(srcObject, MENU_ITEMLIST_DATA);
  if (0 != dstItemList)
    AObjUnlockData(dstObject, MENU_ITEMLIST_DATA);

  return A_OK;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ExportObject
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
static long ExportObject (
  AOBJECTID    objID)
  {
  AOBJECTID    itemObjID;
  UINT 		    i;
	long		    result = A_OK;
  BOOL        enabled;
  BOOL        popup;
  LPMENUINFO  pMenuInfo;
  LPSTR       pTitle;
  LPITEMENTRY pItemEntryList;
  
  
  
  pMenuInfo = (LPMENUINFO)AObjLockData(objID, MENU_INFO_DATA);
  pTitle = (LPSTR)AObjLockData(objID, MENU_TITLE_DATA);
  
  enabled = (pMenuInfo->status & MENU_DISABLED) ? FALSE : TRUE;
  popup = (pMenuInfo->status & MENU_POPUP) ? TRUE : FALSE;  

  I_ALWAYSASSERT(AUpsfExportField("VERSION", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, 
                 &pMenuInfo->version, sizeof(int)) == AUPSF_NOERROR, result = A_ERROR);

  I_ALWAYSASSERT(AUpsfExportField("MENUTITL", 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT,
                 pTitle, lstrlen(pTitle)) == AUPSF_NOERROR, result = A_ERROR);

  I_ALWAYSASSERT(AUpsfExportField("ENABLE", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_INTEGER, 
                 &enabled, sizeof(BOOL)) == AUPSF_NOERROR, result = A_ERROR);

  I_ALWAYSASSERT(AUpsfExportField("POPTYPE", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_INTEGER,
                 &popup, sizeof(BOOL)) == AUPSF_NOERROR, result = A_ERROR);

  I_ALWAYSASSERT(AUpsfExportField("SENDTEXT", 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF,
                 &pMenuInfo->sendText, sizeof(AOBJECTID)) == AUPSF_NOERROR, result = A_ERROR);

  I_ALWAYSASSERT(AUpsfExportField("SENDINDX", 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF,
                 &pMenuInfo->sendIndex, sizeof(AOBJECTID)) == AUPSF_NOERROR, result = A_ERROR);

  if (pMenuInfo->itemCount)
    {
    pItemEntryList = (LPITEMENTRY)AObjLockData(objID, MENU_ITEMLIST_DATA);  

  	for (i = 0; i < pMenuInfo->itemCount; i++)
      {
      itemObjID = pItemEntryList[i].id;
  		if (pItemEntryList[i].status & ITEMENTRY_SEPARATOR)
  		 	itemObjID = 0L;

		  I_ALWAYSASSERT(AUpsfExportField("OBJLIST", i, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, 
		                 &itemObjID, sizeof(AOBJECTID)) == AUPSF_NOERROR, result = A_ERROR);  
                                          
	    }
    AObjUnlockData(objID, MENU_ITEMLIST_DATA);
    }

  AObjUnlockData(objID, MENU_INFO_DATA);
  AObjUnlockData(objID, MENU_TITLE_DATA);
  
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
  UINT 		    i;
	long		    result = A_OK;
  BOOL        enabled;
  BOOL        popup;
  LPMENUINFO  pMenuInfo;
  LPITEMENTRY pItemEntryList;
	char		    buffer[256];
	long		    count;

  pMenuInfo = (LPMENUINFO)AObjLockData(objID, MENU_INFO_DATA);

  I_ALWAYSASSERT(AUpsfImportField("VERSION", 0L, AUPSF_NATV_UINT, 
                 &pMenuInfo->version, sizeof(UINT)) == AUPSF_NOERROR, result = A_ERROR);

	I_ALWAYSASSERT(AUpsfImportField("MENUTITL", 0L, AUPSF_NATV_STRING,
	               buffer, 255) == AUPSF_NOERROR, result = A_ERROR);
	AObjSetData(objID, MENU_TITLE_DATA, buffer, lstrlen((LPSTR)buffer)+1);

	I_ALWAYSASSERT(AUpsfImportField("ENABLE", 0L, AUPSF_NATV_BOOL, 
	               &enabled, sizeof(BOOL)) == AUPSF_NOERROR, result = A_ERROR);

	I_ALWAYSASSERT(AUpsfImportField("POPTYPE", 0L, AUPSF_NATV_UINT, 
	               &popup, sizeof(BOOL)) == AUPSF_NOERROR, result = A_ERROR);

	I_ALWAYSASSERT(AUpsfImportField("SENDTEXT", 0L, AUPSF_NATV_OBJECTREF,
	               &pMenuInfo->sendText, sizeof(AOBJECTID)) == AUPSF_NOERROR, result = A_ERROR);

	I_ALWAYSASSERT(AUpsfImportField("SENDINDX", 0L, AUPSF_NATV_OBJECTREF,
	               &pMenuInfo->sendIndex, sizeof(AOBJECTID)) == AUPSF_NOERROR, result = A_ERROR);
                 
  pMenuInfo->status = 0L;
  if (popup)
    pMenuInfo->status |= MENU_POPUP;
  if (!enabled)
    pMenuInfo->status |= MENU_DISABLED;
  
	count = AUpsfFieldCount("OBJLIST");
	pMenuInfo->itemCount = (WORD)count;

	if (count > 0)
    {
    AObjResizeData(objID, MENU_ITEMLIST_DATA, (count+1)*sizeof(ITEMENTRY));
    pItemEntryList = (LPITEMENTRY)AObjLockData(objID, MENU_ITEMLIST_DATA);
		for (i = 0; i < count; i++)
		  {
			I_ALWAYSASSERT(AUpsfImportField("OBJLIST", i, AUPSF_NATV_OBJECTREF,
			               &pItemEntryList[i].id, sizeof(AOBJECTID)) == AUPSF_NOERROR, result = A_ERROR);

      pItemEntryList[i].status = 0;
      
			if (pItemEntryList[i].id == 0)
			  pItemEntryList[i].status |= ITEMENTRY_SEPARATOR ;
        
      else if (OTYPE_MENUITEM == AObjGetType(pItemEntryList[i].id))
			  pItemEntryList[i].status |= ITEMENTRY_MENUITEM ;


      else  
			  pItemEntryList[i].status |= ITEMENTRY_MENU ;

      pItemEntryList[i].menuItemID = MenuGenerateItemID();			  		  
		  }
    AObjUnlockData(objID, MENU_ITEMLIST_DATA);
	  }
  AObjUnlockData(objID, MENU_INFO_DATA);

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DoBuildSignals
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
static long DoBuildSignals(
  AMEMBLOCKID  signalBlock)
  {
	pAObjSignalRecord  signalPtr;

	AMemSetSize(signalBlock, 2 * sizeof(AObjSignalRecord));

	signalPtr = (pAObjSignalRecord)AMemLock(signalBlock);

	signalPtr[0].theID = SIGNAL_ITMCHOSEN ;
	lstrcpy(signalPtr[0].theName, "Menu Item Chosen");

	signalPtr[1].theID = SIGNAL_NOITEMCHOSEN ;
	lstrcpy(signalPtr[1].theName, "No Item Chosen");

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
  LPMENUINFO  pMenuInfo;
  long        result = A_OK;

  I_UNUSED(systemPtr);

  pMenuInfo = (LPMENUINFO)AObjLockData(objID, MENU_INFO_DATA);

	if(pMenuInfo->version > MENU_VERSION)
		{
		//using an old DLL
		result = AObjReportError(objID, AOBJ_READ, A_ERROR,
     "Menu object requires new version of alm_menu.dll", 0);
		}

	if(pMenuInfo->version < MENU_VERSION)
		pMenuInfo->version = MENU_VERSION;

  AObjUnlockData(objID, MENU_INFO_DATA);

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
	char 	      text[MENU_MAX_TEXT+1];
  LPMENUINFO  pMenuInfo;

	MenuCheckObjectIDs(objID);

  pMenuInfo = (LPMENUINFO)AObjLockData(objID, MENU_INFO_DATA);

	if ((pMenuInfo->status & MENU_NEVER_EDITED) == MENU_NEVER_EDITED)
		{
		//change the name to the name in the subject's objlist if the objd
		//was never brought up
		AObjGetName(objID, text, MENU_MAX_TEXT);
		AObjSetData(objID, MENU_TITLE_DATA, text, (LONG)lstrlen(text)+1);
		}

  AObjUnlockData(objID, MENU_INFO_DATA);

  return A_OK;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DoValidateIDs
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
long DoValidateIDs(
  AOBJECTID      objID,
  pAObjMessage  sys)
  {
  LPMENUINFO  pMenuInfo;
  LPITEMENTRY pItemEntryList;

	BOOL	changed;
	int		i;

  pMenuInfo = (LPMENUINFO)AObjLockData(objID, MENU_INFO_DATA);

  if (pMenuInfo->itemCount > 0)
    {
    pItemEntryList = (LPITEMENTRY)AObjLockData(objID, MENU_ITEMLIST_DATA);
	  for (i = 0; i < pMenuInfo->itemCount; i++)
		  {
		  if (pItemEntryList[i].status & (ITEMENTRY_MENUITEM|ITEMENTRY_MENU))
        {
		    changed = AObjValidateID(&pItemEntryList[i].id, sys);
	     	if (changed && (pItemEntryList[i].id == 0L))
			    pItemEntryList[i].status |= ITEMENTRY_DELETEENTRY;
        }
      }
    pMenuInfo->itemCount -=
     DeleteItemEntries(pItemEntryList, pMenuInfo->itemCount);
    AObjUnlockData(objID, MENU_ITEMLIST_DATA);
    AObjResizeData(objID, MENU_ITEMLIST_DATA,
     pMenuInfo->itemCount*sizeof(ITEMENTRY));
    }

	if (pMenuInfo->sendText != 0)
	  AObjValidateID(&pMenuInfo->sendText, sys);

	if (pMenuInfo->sendIndex != 0)
		AObjValidateID(&pMenuInfo->sendIndex, sys);

  AObjUnlockData(objID, MENU_INFO_DATA);

  return A_OK;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MenuAdministration
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
extern "C" LONG ALMAPI MenuAdministration(
  AOBJECTID     theObject,
  pAObjMessage systemPtr )
  {
  pApQueryInfo  pQueryInfo;
  long          result;

	switch(systemPtr->message1)
		{
		case AOBJ_CREATED:
			result = DoCreated(theObject, (APROTOCOLID)systemPtr->message2);
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
      MenuCheckObjectIDs(theObject);
      result = EditMenuObject(theObject);
		  break;

	  case AOBJ_VALIDATEOBJECTIDS:
		  result = DoValidateIDs(theObject, systemPtr);
		  break;

	  case AOBJ_BUILDSIGNALS:
      result = DoBuildSignals((AMEMBLOCKID)systemPtr->message2);
		  break;

	  case AOBJ_CHECKOBJECT:
      result = DoCheckObject(theObject);
		  break;

    case AOBJ_QUERYPROTOCOL:
      if (theObject)   // theObject <> 0, so querying about an object.
        {              
        LPMENUINFO pMenuInfo;
        pMenuInfo = (LPMENUINFO)AObjLockData(theObject, MENU_INFO_DATA);

        if (pMenuInfo->status & MENU_POPUP)
          {
          pQueryInfo = (ApQueryInfo*) systemPtr->message4;
          result = AP_SUPPORTED;      // popup menu, so yes it's a window item
          pQueryInfo->pfnCreate = (APPCFN)MenuPopupCreate;
          }
        else
          result = AP_NOTSUPPORTED;   // not a popup menu, so not a window item

        AObjUnlockData(theObject, MENU_INFO_DATA);  
        }
      else             // theObject == 0, so querying about a type.
        {
        result = (AP_SUPPORTED | AP_DYNAMIC);    // answer is : some times it's a window item
        }
        break;
        
    default:
      result = A_NOTHANDLED;
      break;

		}

  return result;

  }

