#include <almsys.h>
#include "fhelpdlg.h"

BOOL I_CALLBACK FuncHelpDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	);



#define	MAXSYNTAXCHARS	100

#define VISIBLE_ITEMS_COUNT	10


#define MLE_HELP  	125


static HINSTANCE		H_APP_INSTANCE;
static HINSTANCE		H_PREV_INSTANCE;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: FuncHelpDlgBox
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
BOOL FuncHelpDlgBox(
	HINSTANCE			hAppInstance,
	HINSTANCE			hPrevInstance,
	HWND					hWndParent,
	LPTSTRING			pHelp)
	{
	DLGPROC	 lpProc;
	BOOL		 result;

	H_APP_INSTANCE = hAppInstance;
	H_PREV_INSTANCE = hPrevInstance;

	lpProc = (DLGPROC) MakeProcInstance((FARPROC)FuncHelpDlgProc, hAppInstance);
	result = DialogBoxParam(hAppInstance, "FUNC_HELP_DLG", hWndParent, lpProc,
	 (long)pHelp);
	FreeProcInstance((FARPROC) lpProc);

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: FuncDescDlgProc
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

BOOL I_CALLBACK FuncHelpDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
	{
	static LPTSTRING	pHelp;
	BOOL							msgHandled = TRUE;

	switch (message)
		{
		case WM_INITDIALOG:
			pHelp = (LPTSTRING)lParam;
			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					break;

				case IDOK:
					EndDialog(hDlg, TRUE);
					break;


				default:
					msgHandled = FALSE;
					break;
				}
			break;

		default:
			msgHandled = FALSE;
			break;

		}

	return msgHandled;

	}


