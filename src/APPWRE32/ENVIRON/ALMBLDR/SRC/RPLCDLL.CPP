#include "almsys.h"
#include <windowsx.h>

#include "compuid.h"
#include "rplcdll.h"
#include "cfglib.h"
#include "devsys.h"
#include "cfgdesc.h"
#include "objdesc.h"
#include "funcdesc.h"
#include "fnameobj.h"

/////////////////////////////////////////////////////////////////////////////
//      Function prototypes
/////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK ReplaceDLLDlgProc(
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
	BOOL     result;

	result = DialogBoxParam(hAppInstance, "REPLACE_DLL_DLG", hWndParent,
								(DLGPROC)ReplaceDLLDlgProc, (DWORD)pcfg);
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
	int							nMaxFNameLen		= SFilePath::MaxLength();
	BOOL            replaceCount = 0;
  char*           currentDLL;
  char*           newDLL;
  char            msgBuffer[50];
  LPOBJDESC       pObjDesc;
  LPAFUNCDESC      pFuncDesc;
  LPRESPONSEARRAY pResponseArray;
  short           responseCnt;
  int             i;

	currentDLL = new char[nMaxFNameLen];
	newDLL     = new char[nMaxFNameLen];

  Edit_GetText(GetDlgItem(hDlg, SLE_CURRENTDLL), currentDLL, nMaxFNameLen);
  Edit_GetText(GetDlgItem(hDlg, SLE_NEWDLL), newDLL, nMaxFNameLen);

  //do object descriptions

  pObjDesc = 0;
  while (0 != (pObjDesc = pcfg->NextObjectDesc(pObjDesc)))
    {
    if (0 == lstrcmpi(currentDLL, pObjDesc->GetDLL(ADMIN_ROUTINE_INDEX)))
      {
      pObjDesc->SetDLL(ADMIN_ROUTINE_INDEX, newDLL);
      replaceCount++;
      }
    if (0 == lstrcmpi(currentDLL, pObjDesc->GetDLL(DEF_ROUTINE_INDEX)))
      {
      pObjDesc->SetDLL(DEF_ROUTINE_INDEX, newDLL);
      replaceCount++;
      }
    pResponseArray = pObjDesc->GetResponseArray();
    responseCnt = pResponseArray->GetCount();
    for (i=0; i<responseCnt; i++)
      {
      if (0 == lstrcmpi(currentDLL, (*pResponseArray)[i].GetDll()))
        {
        replaceCount++;
        (*pResponseArray)[i].SetDll(newDLL);
        }
      }
    }

  pFuncDesc = 0;
  while (0 != (pFuncDesc = pcfg->NextFunctionDesc(pFuncDesc)))
    {
    if (0 == lstrcmpi(currentDLL, pFuncDesc->GetLibName()))
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

	delete currentDLL;
	delete newDLL;

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

BOOL CALLBACK ReplaceDLLDlgProc(
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
			{
			HWND		hCurrent	= GetDlgItem(hDlg, SLE_CURRENTDLL);
			HWND		hNew			= GetDlgItem(hDlg, SLE_NEWDLL);
			int			nMax			= SFilePath::MaxLength();

			Edit_LimitText(hCurrent, nMax);
			Edit_LimitText(hNew, nMax);
			Edit_SetText(hCurrent, "");
			Edit_SetText(hNew, "");
      buttonSaysClose = FALSE;
      pcfg = (LPCFGDESC)lParam;
			break;
			}

		case WM_COMMAND:
			{
			WORD		wID		= GET_WM_COMMAND_ID(wParam, lParam);

			switch (wID)
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
   			    Button_SetText(GetDlgItem(hDlg, IDCANCEL), "&Close");
            buttonSaysClose = TRUE;
            }
          break;

				default:
					msgHandled = FALSE;
					break;
				}
			break;
			}

		default:
			msgHandled = FALSE;
			break;
		}

	return msgHandled;

	}
