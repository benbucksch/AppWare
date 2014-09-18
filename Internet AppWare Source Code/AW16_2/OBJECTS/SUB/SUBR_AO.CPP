////////////////////////////////////////////////////////////////////////////
//
// (c) Serius Corporation, 1989-1992
//
// Object Group:	Essentials
// Object:		  	Subroutine
//
// Filename:	"subr_ao.cpp"
//
// Author:	Kevin Smart
//   Start:		August 10, 1992
//   Finish:	August 11, 1992
//
// Description:
//	Subroutine Object ADMN, OBJD,  and DLL entry point
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include	<a_alm.h>
#include	<a_almutl.h>
#include	<string.h>
#include	<helpids.h>

#include	"sub.h"

////////////////////////////Global Container////////////////////////////////

GLOBAL			G;
extern int	_WinAllocFlag;

/////////////////////Function Prototypes for this File//////////////////////

void	CopyDataPtr	(OBJECTID, OBJECTID, long);
void	DoAssign		(OBJECTID, OBJECTID);

////////////////////////////////////////////////////////////////////////////

#pragma	argsused	//disable warning: "Parameter '' never used"

int far pascal LibMain(
	HINSTANCE	hInstance,
	WORD			wDataSeg,
	WORD			wHeapSize,
	LPSTR			lpszCmdLine)

	{
  G.hInst = hInstance;
  _WinAllocFlag = GMEM_SHARE;

	if(wHeapSize>0)
		UnlockData(0);

  return 1;
	}

////////////////////////////////////////////////////////////////////////////

long ALMCALLBACK SubroutineAdmn(
	OBJECTID				theObject,
	pAObjMessage		theSystem)
	{
	LONG					lRet		= A_OK;
  SUBROUTINE*		sub;

	switch(theSystem->message1)
		{
		case AOBJ_CREATED :
			sub = new SUBROUTINE(theObject);
			delete sub;
			break;

		case AOBJ_ASSIGNOBJECT :
			DoAssign(theObject, (OBJECTID) (theSystem->message3));
			break;

		case AOBJ_EXPORTOBJECT :
			sub = new SUBROUTINE(theObject);
			sub->ExportObject();
      delete sub;
			break;

		case AOBJ_IMPORTOBJECT :
			sub = new SUBROUTINE(theObject);
			sub->ImportObject();
			delete sub;
      break;

		case AOBJ_READ:
			sub = new SUBROUTINE(theObject);
			if(sub->Version()!=VERSION_OBJECT)
				{
				lRet = A_ERROR;
				AObjReportError(theObject, AOBJ_READ, A_ERROR, "Subroutine\tser_subr.dll",
												VERSION_OBJECT);
				}
			delete sub;
			break;

    case AOBJ_EDITOBJECT :
			sub = new SUBROUTINE(theObject);
			DialogBoxParam(G.hInst, "SubDlg", GetActiveWindow(), SUBROUTINE::SubroutineDlg, (long)sub);
			delete sub;
			lRet = A_MODIFIED;
			break;

    case AOBJ_VALIDATEOBJECTIDS:
			break;

		case AOBJ_BUILDSIGNALS:
    	{
			MEMBLOCKID	theBlock = (MEMBLOCKID)theSystem->message2;
			AMemSetSize(theBlock, NUMSIGNALS*sizeof(AObjSignalRecord));

			pAObjSignalRecord	theArray = (pAObjSignalRecord)AMemLock(theBlock);

			theArray[SUBR_CALLED].theID = SUBR_CALLED;
			lstrcpy(theArray[SUBR_CALLED].theName, "Called");
			break;
      }

		case AOBJ_CHECKOBJECT:
			// the subroutine object will always be ok, so leave lRet as A_OK
			break;

		default:
			lRet = A_NOTHANDLED;
    	break;
		}  

  return lRet;
}

////////////////////////////////////////////////////////////////////////////

void CopyDataPtr(
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

	AObjUnlockData(dstObj, index);
	AObjUnlockData(srcObj, index);
	}

////////////////////////////////////////////////////////////////////////////

void	DoAssign(
	OBJECTID	srcObject,
	OBJECTID	dstObject)
	{
  long			dataPtrCnt;
  long 			ptrIndex;
  TYPEID		srcType,	dstType;

  srcType	= AObjGetType(srcObject);
  dstType	= AObjGetType(dstObject);

	if(srcObject==NULL || dstObject==NULL)
		{
    MessageBox(GetActiveWindow(), "Assign Failed; Null Object Value", NULL,
    	       	 MB_OK | MB_ICONEXCLAMATION);
    return;
    }

	if(srcType!=OTYPE_SUB || dstType!=OTYPE_SUB)
		{
    MessageBox(GetActiveWindow(), "Assign Failed; Invalid Object Type", NULL,
    	       	 MB_OK | MB_ICONEXCLAMATION);
    return;
    }

  dataPtrCnt = AObjGetDataCount(srcObject);
  for(ptrIndex=0; ptrIndex<dataPtrCnt; ptrIndex++)
    CopyDataPtr(dstObject, srcObject, ptrIndex);
	}

////////////////////////////////////////////////////////////////////////////

SUBROUTINE::SUBROUTINE(
	OBJECTID	oid,
	BOOL			/*runtimeConstructor*/)

	{
  theObject = oid;

	data0	= (DATA0*)AObjLockData(theObject, 0L);
	}

////////////////////////////////////////////////////////////////////////////

SUBROUTINE::SUBROUTINE(
	OBJECTID oid)

	{
  theObject = oid;

  if(!theObject || AObjGetType(theObject)!=OTYPE_SUB) return;

  if(AObjGetDataCount(theObject)==0L)
    AObjSetDataCount(theObject, 1L);

  data0	= (DATA0*)AObjLockData(theObject, 0L);

	if(!data0)
		{
    if (!AObjResizeData(theObject, 0L, sizeof(DATA0)) ||
				(data0 = (DATA0*)AObjLockData(theObject, 0L)) == NULL)
			return;

		memset(data0, 0, sizeof(DATA0));
		data0->ver = VERSION_OBJECT;
		data0->modal = TRUE;
    }
	}

////////////////////////////////////////////////////////////////////////////

SUBROUTINE::~SUBROUTINE(
	VOID)
	{
	AObjUnlockData(theObject, 0L);
	}

////////////////////////////////////////////////////////////////////////////

void	SUBROUTINE::SetParameters(
	OBJECTID	obj1,
	OBJECTID	obj2,
	OBJECTID	obj3)

	{
  if(!data0) return;

  data0->Obj1 = obj1;
  data0->Obj2 = obj2;
  data0->Obj3 = obj3;
	}

////////////////////////////////////////////////////////////////////////////

void	SUBROUTINE::GetParameters(
	OBJECTID*	o1,
	OBJECTID*	o2,
	OBJECTID*	o3)

	{
	if(!data0)
		{
    *o1 = *o2 = *o3 = NULL;
    return;
    }

  *o1 = data0->Obj1;
  *o2 = data0->Obj2;
  *o3 = data0->Obj3;
	}

////////////////////////////////////////////////////////////////////////////

BOOL far pascal _export	SUBROUTINE::SubroutineDlg(
	HWND		hDlg,
	UINT		message,
	WPARAM	wParam,
	LPARAM	lParam)

	{
  SUBROUTINE*	sub;
  char		szBuffer[OBJECTNAMESIZE];

	switch(message)
		{
    case WM_INITDIALOG:
			AUtlSetTitleFont(OTYPE_SUB, NULL, GetDlgItem(hDlg, IDD_OBJECT));
			SetWindowLong(hDlg, DWL_USER, lParam);
			sub = (SUBROUTINE*)lParam;
			sub->InitDlgControls(hDlg);
			AObjGetName(sub->theObject, szBuffer, OBJECTNAMESIZE);
			SetWindowText(hDlg, szBuffer);
			return FALSE;

    case WM_COMMAND:
			sub = (SUBROUTINE*)GetWindowLong(hDlg, DWL_USER);

			switch(wParam)
				{
				case IDOK:
					sub->UpdateData(hDlg);
          EndDialog(hDlg, TRUE);
					return TRUE;

				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					return TRUE;

				case IDHELP:
					WinHelp(hDlg, "serhelp.hlp", HELP_CONTEXT, HELPID_OBJD_Subr);
					return TRUE;

				default :
					break;
				}

			break;
    }

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////

void	SUBROUTINE::InitDlgControls(
	HWND	hDlg)

	{
  CheckRadioButton(hDlg, IDD_MODAL, IDD_NONMODAL, IDD_MODAL+(data0->modal ? 0 : 1));
	}

////////////////////////////////////////////////////////////////////////////

void	SUBROUTINE::UpdateData(
	HWND	hDlg)

	{
  data0->modal = IsDlgButtonChecked(hDlg, IDD_MODAL);
	}

////////////////////////////////////////////////////////////////////////////

void	SUBROUTINE::ExportObject()

	{
	// ignore errors
	AUpsfExportField("VERSION", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &data0->ver, sizeof(int));
	AUpsfExportField("MODAL", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_INTEGER, &data0->modal, sizeof(BOOL));
	}

////////////////////////////////////////////////////////////////////////////

void	SUBROUTINE::ImportObject()

	{
	if(AUpsfImportField("MODAL", 0L, AUPSF_NATV_BOOL, &data0->modal, sizeof(BOOL)) != AUPSF_NOERROR)
		{
		AUpsfLogComment("Defaulting to modal subroutine.");
		data0->modal = TRUE;
    }
	}
	
////////////////////////////////////////////////////////////////////////////

long far pascal _export GetSerVersion(
	TYPEID	theType)

	{
	if(theType==OTYPE_SUB)
		return VERSION_DLL;
	else
		return 0L;
  }

////////////////////////////////////////////////////////////////////////////
