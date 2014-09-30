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
// FILE:"admconn.cpp"
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

#include <mem.h>
#include "commObj.h"

#ifdef __BORLANDC__
extern int		_WinAllocFlag;
#endif

extern "C" long ALMAPI GetSerVersion(TYPEID) ;
LONG VersionCheck(pAObjMessage theSystem) ;

void	DoAssign (OBJECTID, OBJECTID, TYPEID) ;
void	CopyDataPtr(OBJECTID, OBJECTID, long) ;
BOOL  EditObject (OBJECTID) ;
HINSTANCE hInstance ;

LONG ALMAPI ConnectAdmn (
	OBJECTID	theObject,
	pAObjMessage	theSystem)
{
	CONN* conn ;
	LONG	lRet = A_OK;

	switch(theSystem->message1) {
		case AOBJ_CREATED :
				conn = new CONN (theObject) ;
				delete conn ;
				break;

		case AOBJ_ASSIGNOBJECT :
				DoAssign(theObject, (OBJECTID) (theSystem->message3), OTYPE_CONN);
				break;

		/*case AOBJ_EXPORTOBJECT :
    		conn = new CONN (theObject) ;
				conn->ExportObject() ;
				delete conn ;
				break;

		case AOBJ_IMPORTOBJECT :
    		conn = new CONN (theObject) ;
				conn->ImportObject() ;
        delete conn ;
				break; */

		case AOBJ_READ :
    		conn = new CONN (theObject) ;
				lRet = conn->doCheckVersion(theSystem) ;
        delete conn ;
				break;

		case AOBJ_EDITOBJECT :
        {
    		BOOL ok = EditObject(theObject) ;
				if (ok)
					lRet = A_MODIFIED;
				break;
        }

		case AOBJ_VALIDATEOBJECTIDS:
				conn = new CONN (theObject) ;
				conn->doValidateObjectIDs(theSystem) ;
				delete conn ;
				break;

		case AOBJ_BUILDSIGNALS:
				conn = new CONN (theObject) ;
				conn->doSignalIDs(theSystem) ;
				delete conn ;
				break;

		case AOBJ_CHECKOBJECT:
				conn = new CONN (theObject) ;
				conn->doCheckObjectIDs() ;
				delete conn ;
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
HGLOBAL SaveObjData (OBJECTID oiObj, long index)
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
				memcpy(tmpData, objData, (size_t)dataSize);
				AObjUnlockData (oiObj, index) ;
				}
			GlobalUnlock(hData) ;
      }
		}
	return (hData) ;
}
void RestoreObjData (OBJECTID oiObj, HGLOBAL hData, long index)
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
BOOL EditObject (OBJECTID theObject)
{
	BOOL ok ;
	CONN *conn ;
	HGLOBAL hData0 ;
	HGLOBAL hSignalInfo ;
	HGLOBAL hSignalBlock ;

	hData0 = SaveObjData (theObject, CONNDATA) ;
	hSignalInfo = SaveObjData (theObject, SGNLINFO) ;
	hSignalBlock = SaveObjData (theObject, SGNLBLOCK) ;
	conn = new EDIT_SIGNAL(theObject) ;
	ok = DialogBoxParam(hInstance, "signals", GetActiveWindow(), (DLGPROC)ConnectionDialog, (LPARAM)conn);
	delete conn ;
	if (!ok)
  	{
		RestoreObjData (theObject, hData0, CONNDATA) ;
		RestoreObjData (theObject, hSignalInfo, SGNLINFO) ;
		RestoreObjData (theObject, hSignalBlock, SGNLBLOCK) ;
		}
	GlobalFree (hData0) ;
	GlobalFree (hSignalInfo) ;
	GlobalFree (hSignalBlock) ;
	return (ok) ;
}
//
//
//
extern "C" int far pascal _export LibMain 
	(
	HINSTANCE hInst, 
	WORD wDataSeg,
	WORD wHeapSize, 
	LPSTR lpszCmdLine
	)
{
	I_UNUSED(wDataSeg);
	I_UNUSED(lpszCmdLine);

	hInstance = hInst;
  _WinAllocFlag = GMEM_SHARE;

  if(wHeapSize>0) UnlockData(0);
	return 1;
}


