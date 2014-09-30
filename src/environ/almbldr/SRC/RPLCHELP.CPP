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

BOOL CALLBACK ReplaceHelpFileDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	);

/////////////////////////////////////////////////////////////////////////////
//      Local Constant Definitions
/////////////////////////////////////////////////////////////////////////////
#define SLE_CURRENTHELPFILE  	101
#define SLE_NEWHELPFILE      	102
#define BN_REPLACE      			103

static SFilePath lastCurrentFile;
static SFilePath lastNewFile;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ReplaceHelpFileDlgBox
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL ReplaceHelpFileDlgBox(
	HINSTANCE   hAppInstance,
	HWND        hWndParent,
  LPCFGDESC   pcfg)
	{
	BOOL     result;

	result = DialogBoxParam(hAppInstance, "REPLACE_HELPFILE_DLG", hWndParent,
														(DLGPROC)ReplaceHelpFileDlgProc, (DWORD)pcfg);
	return result;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DoReplace
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static BOOL DoReplace(
  HWND       hDlg,
  LPCFGDESC  pcfg)
	{
	BOOL            replaceCount		= 0;
	int							nMaxFNameLen		= SFilePath::MaxLength();
  char*           currentHelpFile;
  char*           newHelpFile;
  char            msgBuffer[50];
  LPOBJDESC       pObjDesc;
  LPAFUNCDESC      pFuncDesc;

	currentHelpFile = new char[nMaxFNameLen];
	newHelpFile = new char[nMaxFNameLen];

  Edit_GetText(GetDlgItem(hDlg, SLE_CURRENTHELPFILE), currentHelpFile, nMaxFNameLen);
  Edit_GetText(GetDlgItem(hDlg, SLE_NEWHELPFILE), newHelpFile, nMaxFNameLen);

  //do object descriptions

  pObjDesc = 0;
  while (0 != (pObjDesc = pcfg->NextObjectDesc(pObjDesc)))
    {
    if (0 == lstrcmpi(currentHelpFile, pObjDesc->GetHelpFileName()))
      {
      pObjDesc->SetHelpFileName(newHelpFile);
      replaceCount++;
      }
   	}

  pFuncDesc = 0;
  while (0 != (pFuncDesc = pcfg->NextFunctionDesc(pFuncDesc)))
    {
    if (0 == lstrcmpi(currentHelpFile, pFuncDesc->GetHelp()))
      {
      pFuncDesc->SetHelp(newHelpFile);
      replaceCount++;
      }
    }

  if (replaceCount > 0)
		{
		if(replaceCount == 1)
			lstrcpy(msgBuffer, "1 item was changed.");
		else
    	wsprintf(msgBuffer, "%i items were changed.", replaceCount,
     		(LPSTR)newHelpFile);
  	}
  else
    wsprintf(msgBuffer, "There are no items referencing %s.",
     (LPSTR)currentHelpFile);

  MessageBox(hDlg, msgBuffer, "Change Help File Names.", MB_OK);

	lastCurrentFile = currentHelpFile;
	lastNewFile     = newHelpFile;

	delete currentHelpFile;
	delete newHelpFile;

  return replaceCount;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ReplaceHelpFileDlgProc
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

BOOL CALLBACK ReplaceHelpFileDlgProc(
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
			HWND			hCurrent	= GetDlgItem(hDlg, SLE_CURRENTHELPFILE);
			HWND			hNew			= GetDlgItem(hDlg, SLE_NEWHELPFILE);
			int				nMax			= SFilePath::MaxLength();

			Edit_LimitText(hCurrent, nMax);
			Edit_LimitText(hNew, nMax);
			Edit_SetText(hCurrent, (const char*) lastCurrentFile);
			Edit_SetText(hNew, (const char*) lastNewFile);
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
