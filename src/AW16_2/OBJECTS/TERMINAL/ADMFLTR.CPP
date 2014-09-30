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
// FILE:"admfltr.cpp"
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

#include <stdio.h>
#include <mem.h>
#include "filetrns.h"

#ifdef __BORLANDC__
extern int		_WinAllocFlag;
#endif

void	  DoAssign (OBJECTID, OBJECTID, TYPEID) ;
BOOL    EditObject (OBJECTID) ;
void	  CopyDataPtr(OBJECTID, OBJECTID, long) ;
HGLOBAL SaveObjData (OBJECTID, long) ;
void    RestoreObjData (OBJECTID, HGLOBAL, long) ;
HINSTANCE hInstance ;

//////////////////////////////////////////////////////////////////////////
//File Transfer Object Admn
//////////////////////////////////////////////////////////////////////////
extern "C" LONG ALMAPI FileTransferAdmn (
	OBJECTID	theObject,
	pAObjMessage	theSystem)
{
	FILETRANSFER* ftns ;
	LONG	lRet			= A_OK;

	switch(theSystem->message1) 
		{
		case AOBJ_AWAKENED:
    	//ftns = new FILETRANSFER (theObject) ;
			//AObjSetRuntimeInfo(theObject, (long) ftns); 
			lRet = A_CONTINUE;
			break;

		case AOBJ_ASLEEP:
			//ftns =(FILETRANSFER*) AObjGetRuntimeInfo (theObject) ;
			//if (ftns)
			//	delete ftns ;
			break ;
		case AOBJ_CREATED:
			ftns = new FILETRANSFER (theObject) ;
			delete ftns ;
			break;

	 	case AOBJ_ASSIGNOBJECT :
				DoAssign(theObject, (OBJECTID) (theSystem->message3), OTYPE_FILETRANSFER);
				break;

		/*case AOBJ_EXPORTOBJECT :
    		ftns = new FILETRANSFER (theObject) ;
				ftns->ExportObject() ;
        delete ftns ;
				break;

		case AOBJ_IMPORTOBJECT :
    		ftns = new FILETRANSFER (theObject) ;
				ftns->ImportObject() ;
        delete ftns ;
				break; */

		case AOBJ_READ :
    		ftns = new FILETRANSFER (theObject) ;
				//ftns->CheckVersion() ;
        delete ftns ;
				break;

		case AOBJ_EDITOBJECT :
    		BOOL ok ;
				ok = EditObject (theObject) ;
        if (ok)
					lRet = A_MODIFIED;
				break;

		case AOBJ_VALIDATEOBJECTIDS:
				ftns = new FILETRANSFER (theObject) ;
				ftns->DoValidateObjectIDs(theSystem) ;
				delete ftns ;
				break;

		case AOBJ_BUILDSIGNALS:
				ftns = new FILETRANSFER (theObject) ;
				ftns->DoSignalIDs (theSystem) ;
				delete ftns ;
				break;

		case AOBJ_CHECKOBJECT:
				// the loop object will always be healthy; leave lRet as A_OK
				break;

		default:
				lRet = A_NOTHANDLED;
				break;
		}

	return lRet;
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
BOOL EditObject (
	OBJECTID theObject)  
{
	FILETRANSFER* ftns ;
	BOOL          ok ;
	HGLOBAL       hData0 ;

	hData0 = SaveObjData (theObject, FTNSDATA) ;
	ftns = new FILETRANSFER (theObject);
	ok = DialogBoxParam(hInstance, "file_dialog", GetActiveWindow(), (DLGPROC)FileTransferDlg, (LPARAM)ftns);
	delete ftns;
	if (!ok)
		RestoreObjData (theObject, hData0, FTNSDATA) ;
	GlobalFree (hData0) ;

	return(ok);
}

void	CopyDataPtr(
	OBJECTID dstObj, 
	OBJECTID srcObj, 
	long index)

{
  long	srcSize= AObjGetDataSize(srcObj, index);
  void*	srcPtr = (void*)AObjLockData(srcObj, index);
  void*	dstPtr;

	AObjResizeData(dstObj, index, srcSize);
	dstPtr =(void*)AObjLockData(dstObj, index);

  if(srcPtr!=NULL && dstPtr!=NULL) {
    CopyMemory(dstPtr, srcPtr, (size_t)srcSize);
    }

	AObjUnlockData(srcObj, index);
	AObjUnlockData(dstObj, index);
}
HGLOBAL SaveObjData (
	OBJECTID oiObj, 
	long index)
{
	HGLOBAL hData = NULL ;
	long	dataSize= AObjGetDataSize(oiObj, index);
	void* tmpData ;
  void* objData ;

	hData = GlobalAlloc (GHND, (DWORD)dataSize) ;
	if (hData)
  	{
		tmpData = (void*)GlobalLock (hData) ;
		if (tmpData)
    	{
			objData =(void*)AObjLockData(oiObj, index);
			if (objData)
      	{
				CopyMemory(tmpData, objData, (size_t)dataSize);
				AObjUnlockData (oiObj, index) ;
				}
			GlobalUnlock(hData) ;
      }
		}
	return (hData) ;
}
void RestoreObjData (
	OBJECTID oiObj, 
	HGLOBAL hData, 
	long index)
{
	long  dataSize = GlobalSize (hData) ;
	void* tmpData ;

	if (hData)
		{
		AObjResizeData (oiObj, index, dataSize) ;
		tmpData = (void*)GlobalLock(hData) ;
		if (tmpData)
			AObjSetData (oiObj, index, tmpData, dataSize) ;
		}
}
//
//
//
int CALLBACK LibMain/*!!PORT!! Replace with DllEntryPoint */ (
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


