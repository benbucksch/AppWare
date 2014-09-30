#include <almsys.h>
#include <memory.h>
#include "edcfgdlg.h"
#include "cfglib.h"
#include "mdiapp.h"
#include "fileobj.h"

BOOL I_CALLBACK EditConfigDlgProc(
	HWND   hDlg,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam
	);

static void InitDialog(
	HWND	hDlg);

#define	MAXNAMECHARS	80
#define	MAXUIDCHARS		4

#define LB_CONFIGLIST	101
#define BN_NEWFILE		103


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: EditConfigDlgBox
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
BOOL EditConfigDlgBox(
	HINSTANCE					hAppInstance,
	HWND							hWndParent,
	LPEDITCONFIGDATA	editConfigData)
	{
	DLGPROC	 lpProc;
	BOOL		 result;

	lpProc = (DLGPROC) MakeProcInstance((FARPROC)EditConfigDlgProc, hAppInstance);

	result = DialogBoxParam(hAppInstance, "EDIT_CONFIG", hWndParent, lpProc,
	 (LPARAM) editConfigData);

	FreeProcInstance((FARPROC) lpProc);

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: EditConfigDlgProc
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

BOOL I_CALLBACK EditConfigDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
	{
	static LPEDITCONFIGDATA	data;
	BOOL										msgHandled = TRUE;
	int											itemID;

	switch (message)
		{
		case WM_INITDIALOG:
			data = (LPEDITCONFIGDATA)lParam;
			InitDialog(hDlg);
			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					break;

				case IDOK:
					itemID = SendDlgItemMessage(hDlg, LB_CONFIGLIST, LB_GETCURSEL,
					 0, 0L);
					if (itemID != LB_ERR)
						{
						data->fileObject = (LPFILEOBJECT)SendDlgItemMessage(hDlg,
						 LB_CONFIGLIST,	LB_GETITEMDATA, itemID, 0L);
						EndDialog(hDlg, TRUE);
						}
					else
						{
						data->fileObject = NULL;
						EndDialog(hDlg, FALSE);
						}
					break;

				case LB_CONFIGLIST:
					switch (HIWORD(lParam))	//switch on notification code
						{
						case LBN_DBLCLK:
							itemID = SendDlgItemMessage(hDlg, LB_CONFIGLIST, LB_GETCURSEL,
							 0, 0L);
							if (itemID != LB_ERR)
								{
								data->fileObject = (LPFILEOBJECT)SendDlgItemMessage(hDlg,
								 LB_CONFIGLIST,	LB_GETITEMDATA, itemID, 0L);
								EndDialog(hDlg, TRUE);
								}
							break;

						default:
							msgHandled = FALSE;
							break;
						}
					break;

				case BN_NEWFILE:
					data->mdiApp->OpenConfigFile();
					EndDialog(hDlg, FALSE);
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


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InitDialog
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
static void InitDialog(
	HWND	hDlg)
	{
	LPFILEOBJECT	fileObject;
	int						itemID;
	char					buffer[256];
	LPSTR					pString;

	fileObject = NULL;

	while (0 != (fileObject = cfgGetNextFileObject(fileObject)))
		{
		if (0 != (pString = fileObject->GetFilePath()))
			lstrcpy(buffer, pString);
		else
			lstrcpy(buffer, "");
		lstrcat(buffer, fileObject->GetName());
		itemID = SendDlgItemMessage(hDlg, LB_CONFIGLIST, LB_ADDSTRING, 0,
		 (DWORD)buffer);
		SendDlgItemMessage(hDlg, LB_CONFIGLIST, LB_SETITEMDATA, itemID,
		 (DWORD)fileObject);
		SendDlgItemMessage(hDlg, LB_CONFIGLIST, LB_SETCURSEL, 0, 0L);
		}

	}