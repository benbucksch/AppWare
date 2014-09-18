////////////////////////////////////////////////////////////////
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
// FILE:"admtmnl.cpp"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <commDlg.h>
#include <stdio.h>
#include <mem.h>
#include "terminal.h"

#ifdef __BORLANDC__
extern int		_WinAllocFlag;
#endif

void DoAssign (OBJECTID, OBJECTID, TYPEID) ;
void CopyDataPtr(OBJECTID, OBJECTID, long) ;
void SaveDataBlock0 (OBJECTID, TERMDATA0*) ;
void RestoreDataBlock0 (OBJECTID, TERMDATA0*) ;
extern "C" OpWndItemD* ALMAPI vTerminalCreate(OBJECTID, OBJECTID, RECT* rcItem,long status,long special) ;

HINSTANCE hInstance ;

extern "C" LONG ALMAPI TerminalAdmn (
	OBJECTID	theObject,
	pAObjMessage	theSystem)
{
	TERMINAL* tmnl ;
	LONG	lRet			= A_OK;

	switch(theSystem->message1) {
		case AOBJ_CREATED :
    		tmnl = new TERMINAL (theObject) ;
				delete tmnl ;
				break;

		case AOBJ_ASSIGNOBJECT :
				DoAssign(theObject, (OBJECTID) (theSystem->message3), OTYPE_TERMINAL);
				break;

		case AOBJ_EXPORTOBJECT :
    		tmnl = new TERMINAL (theObject) ;
				tmnl->ExportObject() ;
        delete tmnl ;
				break;

		case AOBJ_IMPORTOBJECT :
    		tmnl = new TERMINAL (theObject) ;
				tmnl->ImportObject() ;
        delete tmnl ;
				break;

		case AOBJ_READ :
    		tmnl = new TERMINAL (theObject) ;
				tmnl->CheckVersion(theSystem) ;
				delete tmnl ;
				break;

		case AOBJ_EDITOBJECT :
        {
				TERMDATA0 tmpData ;
				SaveDataBlock0(theObject, &tmpData) ;
				tmnl = new TERMINAL(theObject);
				BOOL ok = DialogBoxParam(hInstance, "tml_dialog", GetActiveWindow(), (DLGPROC)TerminalDialog, (LPARAM)tmnl);
				delete tmnl ;
				if (ok)
					lRet = A_MODIFIED;
				else
					RestoreDataBlock0(theObject, &tmpData) ;
				break;
        }

		case AOBJ_QUERYPROTOCOL:
        {
				ApQueryInfo *pQueryInfo = (ApQueryInfo *) theSystem->message4;
				if (pQueryInfo->protocol == APROT_WND)  // if the window protocol is supported
					{
				// system->message3 = APROT_STATIC;
					pQueryInfo->pfnCreate = (APPCFN) vTerminalCreate; // create function for
					lRet = AP_SUPPORTED;
					}
				break;
        }

		case AOBJ_VALIDATEOBJECTIDS:
				tmnl = new TERMINAL (theObject) ;
				tmnl->DoValidateObjectIDs(theSystem) ;
				delete tmnl ;
				break;

    case AOBJ_BUILDSIGNALS:
				break;

		case AOBJ_CHECKOBJECT:
				tmnl = new TERMINAL (theObject) ;
        tmnl->DoCheckConnObj (theObject) ;
        delete tmnl ;
				break;

		default:
				lRet = A_NOTHANDLED;
				break;
		}

	return lRet;
}
void SaveDataBlock0(OBJECTID oiTmnl, TERMDATA0 *tmpData)
{
	TERMDATA0 *data0 ;

 	data0 = (TERMDATA0*)AObjLockData (oiTmnl, TMNLDATA) ;
 	if (data0)
  	{
 		*tmpData = *data0 ;
 		AObjUnlockData (oiTmnl, TMNLDATA) ;
		}
	else
	  {
	  tmpData->version        = 0 ;
		tmpData->oiConn         = 0 ;	
		tmpData->cacheText      = 0 ;	
    tmpData->cacheTextObj   = NULL ;   
	  tmpData->Emulation      = VT100 ;  
		tmpData->Mode          |= LINEWRAP; //|SOUND ;	
    tmpData->curShape       = BLOCK ;	
    tmpData->CRorLF         = INBOUND&OUTBOUND ;	
    tmpData->columns        = Columns80 ;	
		tmpData->bufferSize     = 100 ;	
		tmpData->scrollBar      = VSCROLL ;	
		tmpData->translate      = 0 ;	
		tmpData->activate       = TRUE ;	
		HFONT hFont = (HFONT) GetStockObject (SYSTEM_FIXED_FONT) ;	
    GetObject (hFont, sizeof(LOGFONT), &tmpData->logFont) ;  
	  }
}
void RestoreDataBlock0(OBJECTID oiTmnl, TERMDATA0 *tmpData)
{
	if (tmpData)
		AObjSetData (oiTmnl, TMNLDATA, tmpData, sizeof(TERMDATA0)) ;
	}
void	DoAssign(
	OBJECTID		srcObject,
	OBJECTID		dstObject,
	TYPEID      oType)
{
  long		dataPtrCnt;
  long 		ptrIndex;
  TYPEID	srcType,	dstType;

  srcType	= AObjGetType(srcObject);
  dstType	= AObjGetType(dstObject);

  if(srcObject==NULL || dstObject==NULL) {
    MessageBox(GetActiveWindow(), "Assign Failed; Null Object Value", NULL,
							 MB_OK | MB_ICONEXCLAMATION);
    return;
    }

	if(srcType!=oType || dstType!=oType) {
    MessageBox(GetActiveWindow(), "Assign Failed; Invalid Object Type", NULL,
							 MB_OK | MB_ICONEXCLAMATION);
    return;
    }

  dataPtrCnt = AObjGetDataCount(srcObject);
  for(ptrIndex=0; ptrIndex<dataPtrCnt; ptrIndex++) {
    CopyDataPtr(dstObject, srcObject, ptrIndex);
    }
}

OpWndItemD* ALMAPI vTerminalCreate(
	OBJECTID oiItem,
	OBJECTID oiParent,
	RECT*    rcItem,
	long     status,
	long     special)
{
	OpWndItemD* rVal;

	I_UNUSED(special);
	rVal = (OpWndItemD *)new TMNL_VI (oiItem, oiParent, rcItem, status);
	return rVal;
}

void	CopyDataPtr(OBJECTID dstObj, OBJECTID srcObj, long index)

{
  long	srcSize= AObjGetDataSize(srcObj, index);
  void*	srcPtr = (void*)AObjLockData(srcObj, index);
  void*	dstPtr;

	AObjResizeData(dstObj, index, srcSize);
	dstPtr =(void*)AObjLockData(dstObj, index);

  if(srcPtr!=NULL && dstPtr!=NULL) {
    memcpy(dstPtr, srcPtr, (size_t)srcSize);
    }

	AObjUnlockData(srcObj, index);
	AObjUnlockData(dstObj, index);

}
//
//
//
int CALLBACK LibMain (
	HINSTANCE hInst, 
	WORD wDataSeg,
	WORD wHeapSize, 
	LPSTR lpszCmdLine)
{
	I_UNUSED(wDataSeg);
	I_UNUSED(lpszCmdLine);

	hInstance = hInst;
  _WinAllocFlag = GMEM_SHARE;

  if(wHeapSize>0) UnlockData(0);
	return 1;
}


