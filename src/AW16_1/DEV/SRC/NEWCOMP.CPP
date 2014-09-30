#include <almsys.h>
#include "compuid.h"
#include "newcomp.h"
#include "cfglib.h"
#include "rectypes.h"

/////////////////////////////////////////////////////////////////////////////
//      Function prototypes
/////////////////////////////////////////////////////////////////////////////

BOOL I_CALLBACK NewCompDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	);

BOOL CheckUID(
	COMPUID compUID,
	long            compType);


/////////////////////////////////////////////////////////////////////////////
//      Local Constant Definitions
/////////////////////////////////////////////////////////////////////////////
#define MAXNAMECHARS    80
#define MAXUIDCHARS     4

#define SLE_NAME     101
#define ST_NAME      103
#define SLE_UID      102
#define ST_UID       104

/////////////////////////////////////////////////////////////////////////////
//      Static variables
/////////////////////////////////////////////////////////////////////////////
static char  DEFAULT_NAME[MAXNAMECHARS+1];
static char  TYPE_NAME[MAXNAMECHARS+1];
static long  COMP_TYPE;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ObjDescDlgBox
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
BOOL NewCompDlgBox(
	HINSTANCE     hAppInstance,
	HWND	        hWndParent,
	TSTRING       defaultName,
	TSTRING       typeName,
	long          compType,
	LPNEWCOMPDATA compData)
	{
	DLGPROC  lpProc;
	BOOL     result;

	lstrcpy(DEFAULT_NAME, defaultName);
	lstrcpy(TYPE_NAME, typeName);
	COMP_TYPE = compType;

	lpProc = (DLGPROC) MakeProcInstance((FARPROC)NewCompDlgProc, hAppInstance);
	result = DialogBoxParam(hAppInstance, "NEW_COMP_DLG", hWndParent, lpProc,
	 (DWORD)compData);
	FreeProcInstance((FARPROC) lpProc);

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: NewCompDlgProc
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

BOOL I_CALLBACK NewCompDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
	{
	BOOL                    msgHandled = TRUE;
	static LPNEWCOMPDATA    compData;
	COMPUID                 compUID;
	char                    buffer[MAXNAMECHARS+1];

	switch (message)
		{
		case WM_INITDIALOG:
			compData = (LPNEWCOMPDATA)lParam;
			SendDlgItemMessage(hDlg, SLE_NAME, EM_LIMITTEXT, MAXNAMECHARS, 0L);
			SendDlgItemMessage(hDlg, SLE_UID, EM_LIMITTEXT, MAXUIDCHARS, 0L);
			SetDlgItemText(hDlg, SLE_NAME, DEFAULT_NAME);
			SetDlgItemText(hDlg, SLE_UID, "");

			lstrcpy(buffer, TYPE_NAME);
			lstrcat(buffer, " Name:");
			SetDlgItemText(hDlg, ST_NAME, buffer);

			lstrcpy(buffer, TYPE_NAME);
			lstrcat(buffer, " ID:");
			SetDlgItemText(hDlg, ST_UID, buffer);

			lstrcpy(buffer, "New ");
			lstrcat(buffer, TYPE_NAME);
			SendMessage(hDlg, WM_SETTEXT, 0, (DWORD)(LPSTR)buffer);

			SendDlgItemMessage(hDlg, SLE_NAME, EM_SETSEL, 0, MAKELONG(0, 32767));

			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					break;

				case IDOK:
					GetDlgItemText(hDlg, SLE_UID, buffer, MAXUIDCHARS+1);
							if (lstrlen(buffer) != MAXUIDCHARS)
						{
						SendDlgItemMessage(hDlg, SLE_UID, EM_SETSEL, 0,
						 MAKELONG(0, 32767));
						SetFocus(GetDlgItem(hDlg, SLE_UID));
						MessageBeep(0);
						MessageBox(hDlg,
						 "A component ID must contain exactly four Characters",
						 "Invalid Component ID", MB_OK);
						}
					else
						{
						// these two lines swap the outer bytes and inner bytes
						// to reverse the uid character sequence.
						buffer[0]^=buffer[3]^=buffer[0]^=buffer[3];
						buffer[1]^=buffer[2]^=buffer[1]^=buffer[2];

						compUID = *(LPCOMPUID)buffer;
						if (CheckUID(compUID, COMP_TYPE))
							{

							compData->compUID = compUID;
							GetDlgItemText(hDlg, SLE_NAME, buffer, MAXNAMECHARS+1);
							compData->compName = buffer;
							EndDialog(hDlg, TRUE);
									}
						else
							{
							SendDlgItemMessage(hDlg, SLE_UID, EM_SETSEL, 0,
							 MAKELONG(0, 32767));
							SetFocus(GetDlgItem(hDlg, SLE_UID));
							MessageBeep(0);
							MessageBox(hDlg,
							 "The specified component ID has already been used",
							 "Duplicate Component ID", MB_OK);
							}
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


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: CheckUID
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
BOOL CheckUID(
	COMPUID compUID,
	long            compType)
	{
	BOOL    result;

	switch (compType)
		{
		case RT_OBJDESC:
			result = cfgCheckObjectUID(compUID);
			break;
		case RT_FUNCDESC:
			result = cfgCheckFunctionUID(compUID);
			break;
		case RT_EVENTDESC:
			result = cfgCheckEventUID(compUID);
			break;
		default:
			result = FALSE;
		}

	return result;

	}
