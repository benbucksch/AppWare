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

BOOL I_CALLBACK ReplaceHelpFileDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	);

/////////////////////////////////////////////////////////////////////////////
//      Local Constant Definitions
/////////////////////////////////////////////////////////////////////////////
#define MAXFILECHARS    12

#define SLE_CURRENTHELPFILE  	101
#define SLE_NEWHELPFILE      	102
#define BN_REPLACE      			103

static char lastCurrentFile[MAXFILECHARS + 1] = {""};
static char lastNewFile[MAXFILECHARS + 1] = {""};

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
	DLGPROC  lpProc;
	BOOL     result;

	lpProc = (DLGPROC) MakeProcInstance((FARPROC)ReplaceHelpFileDlgProc, hAppInstance);

	result = DialogBoxParam(hAppInstance, "REPLACE_HELPFILE_DLG", hWndParent,
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
static BOOL DoReplace(
  HWND       hDlg,
  LPCFGDESC  pcfg)
	{
	BOOL            replaceCount = 0;
  char            currentHelpFile[MAXFILECHARS+1];
  char            newHelpFile[MAXFILECHARS+1];
  char            msgBuffer[50];
  LPOBJDESC       pObjDesc;
  LPFUNCDESC      pFuncDesc;

  GetDlgItemText(hDlg, SLE_CURRENTHELPFILE, currentHelpFile, MAXDLLCHARS+1);
  GetDlgItemText(hDlg, SLE_NEWHELPFILE, newHelpFile, MAXDLLCHARS+1);

  //do object descriptions

  pObjDesc = 0;
  while (0 != (pObjDesc = pcfg->NextObjectDesc(pObjDesc)))
    {
    if (0 == _fstricmp(currentHelpFile, pObjDesc->GetHelpFileName()))
      {
      pObjDesc->SetHelpFileName(newHelpFile);
      replaceCount++;
      }
   	}

  pFuncDesc = 0;
  while (0 != (pFuncDesc = pcfg->NextFunctionDesc(pFuncDesc)))
    {
    if (0 == _fstricmp(currentHelpFile, pFuncDesc->GetHelp()))
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

	lstrcpy(lastCurrentFile, currentHelpFile);
	lstrcpy(lastNewFile, newHelpFile);

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

BOOL I_CALLBACK ReplaceHelpFileDlgProc(
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
			SendDlgItemMessage(hDlg, SLE_CURRENTHELPFILE, EM_LIMITTEXT, MAXDLLCHARS, 0L);
			SendDlgItemMessage(hDlg, SLE_NEWHELPFILE, EM_LIMITTEXT, MAXDLLCHARS, 0L);
			SetDlgItemText(hDlg, SLE_CURRENTHELPFILE, lastCurrentFile);
			SetDlgItemText(hDlg, SLE_NEWHELPFILE, lastNewFile);
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


