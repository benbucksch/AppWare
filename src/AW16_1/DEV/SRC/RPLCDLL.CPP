#include <string.h>
#include "almsys.h"

#include "compuid.h"
#include "rplcdll.h"
#include "cfglib.h"
#include "devsys.h"
#include "cfgdesc.h"
#include "objdesc.h"
#include "funcdesc.h"

/////////////////////////////////////////////////////////////////////////////
//      Function prototypes
/////////////////////////////////////////////////////////////////////////////

BOOL I_CALLBACK ReplaceDLLDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	);

BOOL CheckUID(
	COMPUID compUID,
	long    compType);


/////////////////////////////////////////////////////////////////////////////
//      Local Constant Definitions
/////////////////////////////////////////////////////////////////////////////
#define MAXDLLCHARS    12

#define SLE_CURRENTDLL  101
#define SLE_NEWDLL      102
#define BN_REPLACE      103

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ReplaceDLLDlgBox
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL ReplaceDLLDlgBox(
	HINSTANCE   hAppInstance,
	HWND	      hWndParent,
  LPCFGDESC   pcfg)
	{
	DLGPROC  lpProc;
	BOOL     result;

	lpProc = (DLGPROC) MakeProcInstance((FARPROC)ReplaceDLLDlgProc, hAppInstance);

	result = DialogBoxParam(hAppInstance, "REPLACE_DLL_DLG", hWndParent,
   lpProc, (DWORD)pcfg);

	FreeProcInstance((FARPROC) lpProc);

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DoReplace
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL DoReplace(
  HWND       hDlg,
  LPCFGDESC  pcfg)
	{
	BOOL            replaceCount = 0;
  char            currentDLL[MAXDLLCHARS+1];
  char            newDLL[MAXDLLCHARS+1];
  char            msgBuffer[50];
  LPOBJDESC       pObjDesc;
  LPFUNCDESC      pFuncDesc;
  LPRESPONSEARRAY pResponseArray;
  short           responseCnt;
  int             i;

  GetDlgItemText(hDlg, SLE_CURRENTDLL, currentDLL, MAXDLLCHARS+1);
  GetDlgItemText(hDlg, SLE_NEWDLL, newDLL, MAXDLLCHARS+1);

  //do object descriptions

  pObjDesc = 0;
  while (0 != (pObjDesc = pcfg->NextObjectDesc(pObjDesc)))
    {
    if (0 == _fstricmp(currentDLL, pObjDesc->GetDLL(ADMIN_ROUTINE_INDEX)))
      {
      pObjDesc->SetDLL(ADMIN_ROUTINE_INDEX, newDLL);
      replaceCount++;
      }
    if (0 == _fstricmp(currentDLL, pObjDesc->GetDLL(DEF_ROUTINE_INDEX)))
      {
      pObjDesc->SetDLL(DEF_ROUTINE_INDEX, newDLL);
      replaceCount++;
      }
    pResponseArray = pObjDesc->GetResponseArray();
    responseCnt = pResponseArray->GetCount();
    for (i=0; i<responseCnt; i++)
      {
      if (0 == _fstricmp(currentDLL, (*pResponseArray)[i].GetDll()))
        {
        replaceCount++;
        (*pResponseArray)[i].SetDll(newDLL);
        }
      }
    }

  pFuncDesc = 0;
  while (0 != (pFuncDesc = pcfg->NextFunctionDesc(pFuncDesc)))
    {
    if (0 == _fstricmp(currentDLL, pFuncDesc->GetLibName()))
      {
      pFuncDesc->SetLibName(newDLL);
      replaceCount++;
      }
    }

  if (replaceCount > 0)
    wsprintf(msgBuffer, "%i items were change", replaceCount,
     (LPSTR)newDLL);
  else
    wsprintf(msgBuffer, "There are no items referencing %s.",
     (LPSTR)currentDLL);

  MessageBox(hDlg, msgBuffer, "Change DLL Names.", MB_OK);

  return replaceCount;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ReplaceDLLDlgProc
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

BOOL I_CALLBACK ReplaceDLLDlgProc(
	HWND     hDlg,
	UINT		 message,
	WPARAM   wParam,
	LPARAM   lParam)
	{
	BOOL                    msgHandled = TRUE;
  static BOOL             buttonSaysClose;
	static LPCFGDESC        pcfg;

	switch (message)
		{
		case WM_INITDIALOG:
			SendDlgItemMessage(hDlg, SLE_CURRENTDLL, EM_LIMITTEXT, MAXDLLCHARS, 0L);
			SendDlgItemMessage(hDlg, SLE_NEWDLL, EM_LIMITTEXT, MAXDLLCHARS, 0L);
			SetDlgItemText(hDlg, SLE_CURRENTDLL, "");
			SetDlgItemText(hDlg, SLE_NEWDLL, "");
      buttonSaysClose = FALSE;
      pcfg = (LPCFGDESC)lParam;
			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					break;

				case IDOK:
					EndDialog(hDlg, FALSE);
					break;

        case BN_REPLACE:
          if (DoReplace(hDlg, pcfg) && !buttonSaysClose)
            {
 		      	pTheSystem->GetFileObject()->SetDirtyFlag();
   			    SetDlgItemText(hDlg, IDCANCEL, "&Close");
            buttonSaysClose = TRUE;
            }
          break;

				default:
					msgHandled = FALSE;
					break;
				}

		default:
			msgHandled = FALSE;
			break;

		}

	return msgHandled;

	}


