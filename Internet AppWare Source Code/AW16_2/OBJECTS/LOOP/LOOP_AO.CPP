////////////////////////////////////////////////////////////////////////////
//
// (c) Serius Corporation, 1989-1992
//
// Object Group:	Essentials
// Object:		  	Loop
//
// Filename:	"loop_ao.cpp"
//
// Author:	Kevin Smart
//   Start:		August  7, 1992
//   Finish:	August  7, 1992
//
// Description:
//	Loop Object ADMN, OBJD,  and DLL entry point
//
////////////////////////////////////////////////////////////////////////////

#include	<a_alm.h>
#include	<intdebug.h>
#include	<string.h>

#include	"loop.h"

////////////////////////////Global Container////////////////////////////////

GLOBAL			G;
extern int	_WinAllocFlag;

/////////////////////Function Prototypes for this File//////////////////////

void	CopyDataPtr	(OBJECTID, OBJECTID, long);
void	DoAssign		(OBJECTID, OBJECTID);

////////////////////////////////////////////////////////////////////////////

int far pascal LibMain (HANDLE hInstance, WORD wDataSeg,
												WORD wHeapSize, LPSTR lpszCmdLine)

{
	I_UNUSED(wDataSeg);
	I_UNUSED(lpszCmdLine);

  G.hInst = hInstance;
  _WinAllocFlag = GMEM_SHARE;

  if(wHeapSize>0) UnlockData(0);

  return 1;
}

////////////////////////////////////////////////////////////////////////////

LONG ALMCALLBACK LoopAdmn	(OBJECTID	theObject,
					 pAObjMessage	theSystem)

{
	LONG			lRet			= A_OK;
  OBJECTID	tmpObj;
  LOOP*			loop;

	switch(theSystem->message1) {
		case AOBJ_CREATED :
				loop = new LOOP(theObject);
				delete loop;
				break;

		case AOBJ_ASSIGNOBJECT :
				DoAssign(theObject, (OBJECTID) (theSystem->message3));
				break;

		case AOBJ_EXPORTOBJECT :
				loop = new LOOP(theObject);
				loop->ExportObject();
				delete loop;
				break;

		case AOBJ_IMPORTOBJECT :
				loop = new LOOP(theObject);
				loop->ImportObject();
				delete loop;
				break;

		case AOBJ_READ :
				loop = new LOOP(theObject);
				if(loop->Version()!=VERSION_OBJECT)
					{
					lRet = A_ERROR;
					AObjReportError(theObject, AOBJ_READ, A_ERROR,
													"Loop\tser_loop.dll", VERSION_OBJECT);
					}
				delete loop;
				break;

    case AOBJ_EDITOBJECT :
				loop = new LOOP(theObject);
				tmpObj = loop->GetConnect();
				BOOL	valid = AObjCheckType(tmpObj, 0L);
        if(!valid) {
					loop->SetConnect(NULL);
          }
				DialogBoxParam(G.hInst, "LoopDlg", GetActiveWindow(), (FARPROC)LOOP::LoopDlg, (LPARAM)loop);
	 			delete loop;
				lRet = A_MODIFIED;
				break;

		case AOBJ_VALIDATEOBJECTIDS: {
				DATA0*	data0	= (DATA0*)AObjLockData(theObject, 0L);
				tmpObj = data0->connect;
        if(tmpObj) {
					AObjValidateID(&tmpObj, theSystem);
					data0->connect = tmpObj;
          }
				AObjUnlockData(theObject, 0L);
				break;
				}

    case AOBJ_BUILDSIGNALS:
				MEMBLOCKID	theBlock = (MEMBLOCKID)theSystem->message2;
				AMemSetSize(theBlock, NUMSIGNALS*sizeof(AObjSignalRecord));

				pAObjSignalRecord	theArray = (pAObjSignalRecord)AMemLock(theBlock);

				theArray[EACH_ITERATION].theID = EACH_ITERATION;
				lstrcpy(theArray[EACH_ITERATION].theName, "Each iteration");
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

// GetSerVersion ///////////////////////////////////////////////////////////

long ALMCALLBACK GetSerVersion(
	TYPEID	theType)

	{
	if(theType==OTYPE_LOOP)
		return VERSION_DLL;
	else
		return 0L;
	}

////////////////////////////////////////////////////////////////////////////

void	CopyDataPtr(OBJECTID dstObj, OBJECTID srcObj, long index)

{
  long	srcSize    = AObjGetDataSize(srcObj, index);
  void*	srcPtr     = AObjLockData(srcObj, index);
  void*	dstPtr;

	AObjResizeData(dstObj, index, srcSize);
	dstPtr = AObjLockData(dstObj, index);

  if(srcPtr!=NULL && dstPtr!=NULL) {
    memcpy(dstPtr, srcPtr, (size_t)srcSize);
    }

	AObjUnlockData(srcObj, index);
	AObjUnlockData(dstObj, index);

}

////////////////////////////////////////////////////////////////////////////

void	DoAssign(
	OBJECTID		srcObject,
	OBJECTID		dstObject)
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

	if(srcType!=OTYPE_LOOP || dstType!=OTYPE_LOOP) {
    MessageBox(GetActiveWindow(), "Assign Failed; Invalid Object Type", NULL,
							 MB_OK | MB_ICONEXCLAMATION);
    return;
    }

  dataPtrCnt = AObjGetDataCount(srcObject);
  for(ptrIndex=0; ptrIndex<dataPtrCnt; ptrIndex++) {
    CopyDataPtr(dstObject, srcObject, ptrIndex);
    }
}

////////////////////////////////////////////////////////////////////////////
