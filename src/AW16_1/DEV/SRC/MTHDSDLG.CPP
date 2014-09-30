#include <memory.h>
#include "almsys.h"

#include "mthdsdlg.h"
#include "cfglib.h"
#include "methdesc.h"
#include "objdesc.h"
#include "sereclib.h"

BOOL I_CALLBACK MethodDescDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	);

#define	MAXNAMECHARS	80
#define	MAXUIDCHARS		4

#define SLE_NAME	   105
#define SLE_DLL			 126
#define SLE_FUNCTION 127

#define CB_OBJECT		 125

#define ST_UID       104


static HINSTANCE		H_APP_INSTANCE;
static HINSTANCE		H_PREV_INSTANCE;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MethodDescDlgBox
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
BOOL MethodDescDlgBox(
	HINSTANCE			hAppInstance,
	HINSTANCE			hPrevInstance,
	HWND					hWndParent,
	SERUID				objUID)
	{
	DLGPROC	 lpProc;
	BOOL		 result;

	H_APP_INSTANCE = hAppInstance;
	H_PREV_INSTANCE = hPrevInstance;

	lpProc = (DLGPROC) MakeProcInstance((FARPROC)MethodDescDlgProc, hAppInstance);
	result = DialogBoxParam(hAppInstance, "METHOD_DESC_DLG", hWndParent,
	 lpProc, (DWORD)GetObject(objUID));
	FreeProcInstance((FARPROC) lpProc);

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: MethodDescDlgProc
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

BOOL I_CALLBACK MethodDescDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
	{
	static LPMETHODDESC	method;
	BOOL								msgHandled = TRUE;
	char 								buffer[MAXNAMECHARS+1];
	COMPUID							compUID;
	COMPUID							tempUID;
	short								itemIndex;

	switch (message)
		{
		case WM_INITDIALOG:
			method = (LPMETHODDESC)lParam;
			SendDlgItemMessage(hDlg, SLE_NAME, EM_LIMITTEXT, MAXNAMECHARS, 0L);
			SendDlgItemMessage(hDlg, SLE_FUNCTION, EM_LIMITTEXT, MAXNAMECHARS, 0L);
			SendDlgItemMessage(hDlg, CB_OBJECT, CB_LIMITTEXT, 0, 0L);
			SetDlgItemText(hDlg, SLE_NAME, method->GetName());
			SetDlgItemText(hDlg, SLE_DLL, method->GetDLLName());
			SetDlgItemText(hDlg, SLE_FUNCTION, method->GetFunctionName());

			compUID = method->GetUID();
			_fmemcpy(buffer, &compUID, sizeof(COMPUID));
			buffer[4] = '\0';
			SetDlgItemText(hDlg, ST_UID, buffer);

			SendDlgItemMessage(hDlg, SLE_NAME, EM_SETSEL, 0, 0L);

			cfgInitObjectList(GetDlgItem(hDlg, CB_OBJECT));

			compUID = method->GetTypeID();
			itemIndex = 0;
			for (;;)
				{
				tempUID =
				 SendDlgItemMessage(hDlg, CB_OBJECT, CB_GETITEMDATA, itemIndex, 0L);
				if (compUID == tempUID)
					{
					SendDlgItemMessage(hDlg, CB_OBJECT, CB_SETCURSEL, itemIndex, 0L);
					break;
					}
				else if (tempUID == CB_ERR)
					break;
				else
					itemIndex++;
				}
			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					break;

				case IDOK:
					GetDlgItemText(hDlg, SLE_NAME, buffer, MAXNAMECHARS);
					method->SetName(buffer);
					GetDlgItemText(hDlg, SLE_FUNCTION, buffer, MAXNAMECHARS);
					method->SetFunctionName(buffer);
					GetDlgItemText(hDlg, SLE_DLL, buffer, MAXNAMECHARS);
					method->SetDLLName(buffer);
					compUID = SendDlgItemMessage(hDlg, CB_OBJECT, CB_GETITEMDATA,
					 SendDlgItemMessage(hDlg, CB_OBJECT, CB_GETCURSEL, 0, 0L), 0L);
					if (compUID != CB_ERR)
						method->SetTypeID(compUID);
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


