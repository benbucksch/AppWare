////////////////////////////////////////////////////////////////////////////
//
// (c) Serius Corporation, 1989-1992
//
// Object Group:	Essentials
// Object:		  		Launcher			
//
// Filename:	"launc_ao.cpp"
//
// Author:	Kevin Smart
//   Start:		August  5, 1992
//   Finish:	August  6, 1992
//
// Description:
//	Launch Object ADMN, OBJD,  and DLL entry point
//
////////////////////////////////////////////////////////////////////////////

#include	<a_alm.h>
#include	<intdebug.h>
#include	<string.h>

#include	"launch.h"

////////////////////////////Global Container////////////////////////////////

GLOBAL			G;
extern int	_WinAllocFlag;

/////////////////////Function Prototypes for this File//////////////////////

void	CopyDataPtr	(OBJECTID, OBJECTID, long);
void	DoAssign		(OBJECTID, OBJECTID);

////////////////////////////////////////////////////////////////////////////

int far pascal LibMain (
	HANDLE	hInstance,
	WORD		wDataSeg,
	WORD		wHeapSize,
	LPSTR		lpszCmdLine)

	{
	I_UNUSED(wDataSeg);
	I_UNUSED(lpszCmdLine);

  G.hInst = hInstance;
  _WinAllocFlag = GMEM_SHARE;

	if(wHeapSize>0)
		UnlockData(0);

  return 1;
	}

////////////////////////////////////////////////////////////////////////////

long ALMCALLBACK LaunchAdmn(
	OBJECTID				theObject,
	pAObjMessage		theSystem)

	{
	LONG			lRet		= A_OK;
	LAUNCH*		launch;

	switch(theSystem->message1)
		{
		case AOBJ_CREATED : 
		  launch = new LAUNCH(theObject);
			delete launch;
      break;

		case AOBJ_ASSIGNOBJECT : 
			DoAssign(theObject, (OBJECTID) (theSystem->message3));
			break;

		case AOBJ_EXPORTOBJECT : 
			launch = new LAUNCH(theObject);
			launch->ExportObject();
			delete launch;
      break;

		case AOBJ_IMPORTOBJECT :
			launch = new LAUNCH(theObject);
      launch->ImportObject();
			delete launch;
			break;

		case AOBJ_READ:
			launch = new LAUNCH(theObject);
			if(launch->Version()!=VERSION_OBJECT)
				{
				lRet = A_ERROR;
				AObjReportError(theObject, AOBJ_READ, A_ERROR, "Launcher\tser_appl.dll",
												VERSION_OBJECT);
				}
			delete launch;
			break;

    case AOBJ_EDITOBJECT :
			launch = new LAUNCH(theObject);
			launch->style = 1;
			DialogBoxParam(G.hInst, "LaunchDlg1", GetActiveWindow(), (FARPROC)LAUNCH::LaunchDlg, (LPARAM)launch);
			delete launch;
			lRet = A_MODIFIED;
			break;

		case AOBJ_CHECKOBJECT:
			// the loop object will always be ok; leave lRet as A_OK
			break;

		default:
			lRet = A_NOTHANDLED;
			break;
		}
  return lRet;
	}

// GetSerVersion ///////////////////////////////////////////////////////////

long far pascal _export GetSerVersion(
	TYPEID theType)

	{
	if(theType==OTYPE_LAUNCHER)
		return VERSION_DLL;
	else
  	return 0L;
  }

////////////////////////////////////////////////////////////////////////////

void	CopyDataPtr(
	OBJECTID	dstObj,
	OBJECTID	srcObj,
	long			index)

	{
  long	srcSize    = AObjGetDataSize(srcObj, index);
  void*	srcPtr     = AObjLockData(srcObj, index);
  void*	dstPtr;

  AObjResizeData(dstObj, index, srcSize);
  dstPtr = AObjLockData(dstObj, index);

	if(srcPtr!=NULL && dstPtr!=NULL)
		memcpy(dstPtr, srcPtr, (size_t)srcSize);

	AObjUnlockData(srcObj, index);
	AObjUnlockData(dstObj, index);
	}

////////////////////////////////////////////////////////////////////////////

void	DoAssign(
	OBJECTID		srcObject,
	OBJECTID		dstObject)
	{
  long			dataPtrCnt;
  long 			ptrIndex;
  TYPEID		srcType,	dstType;

  srcType	= AObjGetType(srcObject);
  dstType	= AObjGetType(dstObject);

	if(srcObject==NULL || dstObject==NULL)
		{
    MessageBox(GetActiveWindow(), "Assign Failed; Null Object Value", NULL, MB_OK | MB_ICONEXCLAMATION);
    return;
    }

	if(srcType!=OTYPE_LAUNCHER || dstType!=OTYPE_LAUNCHER)
		{
    MessageBox(GetActiveWindow(), "Assign Failed; Invalid Object Type", NULL, MB_OK | MB_ICONEXCLAMATION);
    return;
    }

  dataPtrCnt = AObjGetDataCount(srcObject);
	for(ptrIndex=0; ptrIndex<dataPtrCnt; ptrIndex++)
		CopyDataPtr(dstObject, srcObject, ptrIndex);
	}

////////////////////////////////////////////////////////////////////////////
