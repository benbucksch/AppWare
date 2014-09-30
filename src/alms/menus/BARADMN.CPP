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
// FILE:					BARADMN.CPP
//
// AUTHOR:				Brian Brown
//
// DESCRIPTION:		Administration routine for menu bar object
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
#include <o_menus.h>

#include "modinfo.h"
#include "barobj.h"
#include "itemobj.h"
#include "bardlg.h"
#include "barlib.h"
#include "menulib.h"
#include "hmenumgr.h"


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: CopyDataPtr
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void CopyDataPtr(
  AOBJECTID dstObj,
  AOBJECTID srcObj,
  long     index)
  {
	LONG 	  srcSize;
	LPVOID	srcPtr;

	srcPtr  = AObjLockData(srcObj, index);
	srcSize = AObjGetDataSize(srcObj, index);

	AObjSetData(dstObj, index, srcPtr, srcSize);

  AObjUnlockData(srcObj, index);

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DoAssign
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static long DoAssign(
  AOBJECTID  destID,
  AOBJECTID  srcID)
  {
	LPBARINFO	 pSrcInfo;
  LPBARINFO	 pDestInfo;

	pSrcInfo = (LPBARINFO)AObjLockData(srcID, BAR_INFO_DATA);
	pDestInfo = (LPBARINFO)AObjLockData(destID, BAR_INFO_DATA);

	pDestInfo->status = pSrcInfo->status;
	pDestInfo->menuCount = pSrcInfo->menuCount;
	pDestInfo->testWinHz = pSrcInfo->testWinHz;

	AObjUnlockData(srcID, BAR_INFO_DATA);
	AObjUnlockData(destID, BAR_INFO_DATA);

	CopyDataPtr(destID, srcID, BAR_INFO_DATA);

	AEvtPostStandard(destID, AEVENT_OBJECTCHANGED, 0,
   AEvtGetObjectChangedPriority(AEVT_KILLPARTICULARS));

  return A_OK;

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ExportObject
//
// DESCRIPTION.: Exports the data fields in UPSF.
//
/////////////////////////////////////////////////////////////////////////////
static long ExportObject(
  AOBJECTID  objID)
  {

	UINT    	  i;
  long        result = A_OK;
  BOOL        enabled;
  LPBARINFO   pBarInfo;
  LPMENUENTRY pMenuList = 0;
  
  pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);
  if (pBarInfo->menuCount)
    pMenuList = (LPMENUENTRY)AObjLockData(objID, BAR_MENULIST_DATA);
  
  I_ALWAYSASSERT(AUpsfExportField("VERSION", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
                 &pBarInfo->version, sizeof(int)) == AUPSF_NOERROR, result = A_ERROR);

  enabled = !(pBarInfo->status & BAR_DISABLED); 
 
	I_ALWAYSASSERT(AUpsfExportField("ENABLE", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_INTEGER,
	               &enabled, sizeof(BOOL)) == AUPSF_NOERROR, result = A_ERROR);

	for (i = 0; i < pBarInfo->menuCount; i++)
    {
    I_ALWAYSASSERT(AUpsfExportField("MENULIST", i, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, 
                 &pMenuList[i].id, sizeof(AOBJECTID)) == AUPSF_NOERROR, result = A_ERROR);
 	  }

  AObjUnlockData(objID, BAR_INFO_DATA);
  if (pMenuList)
    AObjUnlockData(objID, BAR_MENULIST_DATA);
  
  return result;

  }


////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ImportObject
//
// DESCRIPTION.: Does the UPSF import function of the data fields.
//
/////////////////////////////////////////////////////////////////////////////
static long ImportObject(
  AOBJECTID  objID)
  {

	UINT		    i;
//	long	    	count;
  long        result = A_OK;
  BOOL        enabled;
  LPBARINFO   pBarInfo;
  LPMENUENTRY pMenuList;
 
  pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);

	pBarInfo->version = 0;

  I_ALWAYSASSERT(AUpsfImportField("VERSION", 0L, AUPSF_NATV_INT, 
                 &pBarInfo->version, sizeof(int)) == AUPSF_NOERROR, result = A_ERROR);

 	I_ALWAYSASSERT(AUpsfImportField("ENABLE", 0L, AUPSF_NATV_BOOL,
	               &enabled, sizeof(BOOL)) == AUPSF_NOERROR, result = A_ERROR);

  if (enabled)
    pBarInfo->status &= (~BAR_DISABLED);
  else
    pBarInfo->status |= (BAR_DISABLED);                 

  if(0 != (pBarInfo->menuCount = (WORD)AUpsfFieldCount("MENULIST")))
    {
	  AObjResizeData(objID, BAR_MENULIST_DATA, pBarInfo->menuCount*sizeof(MENUENTRY));
    pMenuList = (LPMENUENTRY)AObjLockData(objID, BAR_MENULIST_DATA);
  
		for (i = 0; i < pBarInfo->menuCount; i++)
		  {
      I_ALWAYSASSERT(AUpsfImportField("MENULIST", i, AUPSF_NATV_OBJECTREF, &pMenuList[i].id, 
                     sizeof(AOBJECTID)) == AUPSF_NOERROR, result = A_ERROR);
                     
      pMenuList[i].status = 0;                     
		  }
    AObjUnlockData(objID, BAR_MENULIST_DATA);
    }

  AObjUnlockData(objID, BAR_INFO_DATA);
    
  return result;

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: BAROBJ::Read
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static long VerifyVersion(
  AOBJECTID  objID)
	{
  LPBARINFO  pBarInfo;
  long       result = A_OK;

  pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);

	if(pBarInfo->version > MENU_VERSION)
		{
		result = AObjReportError(objID, AOBJ_READ, A_ERROR,
     "Menu Bar object \tser_menu.dll", 0);
		}
	else if (pBarInfo->version < MENU_VERSION)
    {
		pBarInfo->version = MENU_VERSION;
    }

  AObjUnlockData(objID, BAR_INFO_DATA);

  return result;

	}


////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Validate
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static long Validate(
  AOBJECTID     objID,
  pAObjMessage pSystem)
	{
  LPBARINFO    pBarInfo;
  LPMENUENTRY  pMenuList;
	BOOL	       changed;
  int		       i;

  pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);

  if (0 < pBarInfo->menuCount)
    {
    pMenuList = (LPMENUENTRY)AObjLockData(objID, BAR_MENULIST_DATA);
	  for (i = 0; i < pBarInfo->menuCount; i++)
		  {
		  changed = AObjValidateID(&pMenuList[i].id, pSystem);
		  if (changed && pMenuList[i].id == 0L)
			  {
        pMenuList[i].status |= ITEMENTRY_DELETEENTRY;
			  }
/*		  else if(pMenuList[i].id != 0)
			  {
			//eliminate popups from the bars list
			  if (TRUE == OMenuIsPopup(pMenuList[i].id))
				  {
          pMenuList[i].status |= ITEMENTRY_DELETEENTRY;
				  }
			  }    */
		  }
    pBarInfo->menuCount -=
     DeleteMenuEntries(pMenuList, pBarInfo->menuCount);
    AObjUnlockData(objID, BAR_MENULIST_DATA);
    AObjResizeData(objID, BAR_MENULIST_DATA,
     pBarInfo->menuCount*sizeof(MENUENTRY));
    }

  AObjUnlockData(objID, BAR_INFO_DATA);

  return A_OK;

	}


////////////////////////////////////////////////////////////////////////////
// FUNCTION....: CheckObjectIDs
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static long CheckObjectIDs(
  AOBJECTID  objID)
  {
  LPBARINFO    pBarInfo;
  LPMENUENTRY  pMenuList;
	BOOL	       valid;
  int		       i;

  pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);

  if (0 < pBarInfo->menuCount)
    {
    pMenuList = (LPMENUENTRY)AObjLockData(objID, BAR_MENULIST_DATA);
	  for (i = 0; i < pBarInfo->menuCount; i++)
		  {
		  valid = AObjCheckType(pMenuList[i].id, 0L);
		  if (!valid)
			  {
        pMenuList[i].status |= ITEMENTRY_DELETEENTRY;
			  }
		  else if (TRUE == OMenuIsPopup(pMenuList[i].id))
				{
        pMenuList[i].status |= ITEMENTRY_DELETEENTRY;
				}
		  }
    pBarInfo->menuCount -=
     DeleteMenuEntries(pMenuList, pBarInfo->menuCount);
    AObjUnlockData(objID, BAR_MENULIST_DATA);
    AObjResizeData(objID, BAR_MENULIST_DATA,
     pBarInfo->menuCount*sizeof(MENUENTRY));
    }

  AObjUnlockData(objID, BAR_INFO_DATA);

  return A_OK;

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ValidateIDs(void)
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void ValidateIDs(
  AOBJECTID  objID)
  {
  LPBARINFO    pBarInfo;
  LPMENUENTRY  pMenuList;
	BOOL	   valid;
	int		   i;

	// data 1
  pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);

  if (0 < pBarInfo->menuCount)
    {
    pMenuList = (LPMENUENTRY)AObjLockData(objID, BAR_MENULIST_DATA);
	  for (i = 0; i < pBarInfo->menuCount; i++)
		  {
		  valid = AObjCheckType(pMenuList[i].id, 0L);
		  if (!valid)
			  {
        pMenuList[i].status |= ITEMENTRY_DELETEENTRY;
			  }
		  else if (TRUE == OMenuIsPopup(pMenuList[i].id))
				{
        pMenuList[i].status |= ITEMENTRY_DELETEENTRY;
				}
      else
        {
        MenuCheckObjectIDs(pMenuList[i].id);
        }
			}
    pBarInfo->menuCount -=
     DeleteMenuEntries(pMenuList, pBarInfo->menuCount);
    AObjUnlockData(objID, BAR_MENULIST_DATA);
    AObjResizeData(objID, BAR_MENULIST_DATA,
     pBarInfo->menuCount*sizeof(MENUENTRY));
	  }

  AObjUnlockData(objID, BAR_INFO_DATA);

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ObjectHealthy
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static long ObjectHealthy(
  AOBJECTID  objID)
  {
  long       result = A_OK;
  LPBARINFO  pBarInfo;

	CheckObjectIDs(objID);

  pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);

	if (pBarInfo->menuCount <= 0)
    {
    result = AObjReportError(objID, AOBJ_CHECKOBJECT, A_WARNING,
     "The menu bar does not contain any menus.", 0);
	  }

  AObjUnlockData(objID, BAR_INFO_DATA);

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DoCreated
//
// DESCRIPTION.: Create and Initialize the datas and then create the
//               runtime object to manage the menu bar.
//
/////////////////////////////////////////////////////////////////////////////
static long DoCreated(
  AOBJECTID  objID)
  {
  LPBARINFO   pBarInfo;
  HMENUMGR*	pHMenuMgr = new	HMENUMGR(0L);	

  AObjSetRuntimeInfo(objID, (long)pHMenuMgr);

  AObjSetDataCount(objID, BAR_DATA_CNT);

  //Initialize BARINFO_DATA

  AObjResizeData(objID, BAR_INFO_DATA, sizeof(BARINFO));

  pBarInfo = (LPBARINFO)AObjLockData(objID, BAR_INFO_DATA);

  pBarInfo->version		= BAR_VERSION;
  pBarInfo->menuCount = 0;
  pBarInfo->testWinHz	= 430;
  pBarInfo->status    = 0;

  AObjUnlockData(objID, BAR_INFO_DATA);

  return A_OK;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DoAwakened
//
// DESCRIPTION.: create the runtime object to manage the menu bar.
//
/////////////////////////////////////////////////////////////////////////////
static long DoAwakened(
  AOBJECTID  theObject)
  {
  HMENUMGR*	pHMenuMgr = new	HMENUMGR(0L);	

  AObjSetRuntimeInfo(theObject, (long)pHMenuMgr);
  return A_CONTINUE;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DoAsleep
//
// DESCRIPTION.: delete runtime info
//
/////////////////////////////////////////////////////////////////////////////
static long DoAsleep(
  AOBJECTID  theObject)
  {
  HMENUMGR* pHMenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(theObject);

  delete pHMenuMgr;
  AObjSetRuntimeInfo(theObject, 0L);
  return A_CONTINUE;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DoDestroyed
//
// DESCRIPTION.: delete runtime info and set data count to zero
//
/////////////////////////////////////////////////////////////////////////////
static long DoDestroyed(
  AOBJECTID  theObject)
  {
  HMENUMGR* pHMenuMgr = (HMENUMGR*)AObjGetRuntimeInfo(theObject);

  delete pHMenuMgr;
  AObjSetRuntimeInfo(theObject, 0L);

  return A_OK;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DoEditObject
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static long DoEditObject(
  AOBJECTID  objID)
  {
  long result;

  ValidateIDs(objID);

  result = EditMenuBarObject(objID);

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: BarAdministration
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
extern "C" LONG ALMAPI BarAdministration(
  AOBJECTID     theObject,
  pAObjMessage systemPtr)
  {
  long    result;

	switch(systemPtr->message1)
		{
		case AOBJ_CREATED:
      result = DoCreated(theObject);
			break;

    case AOBJ_AWAKENED:
      result = DoAwakened(theObject);
      break;

    case AOBJ_ASLEEP:
      result = DoAsleep(theObject);
      break;

    case AOBJ_DESTROYED:
      result = DoDestroyed(theObject);
      break;

	  case AOBJ_EDITOBJECT:
      result = DoEditObject(theObject);
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
			result = VerifyVersion(theObject);
			break;

	  case AOBJ_VALIDATEOBJECTIDS:
		  result = Validate(theObject, systemPtr);
		  break;

	  case AOBJ_CHECKOBJECT:
		  result = ObjectHealthy(theObject);
			break;

    default:
      result = A_NOTHANDLED;
      break;

  	}

  return result;

  }






