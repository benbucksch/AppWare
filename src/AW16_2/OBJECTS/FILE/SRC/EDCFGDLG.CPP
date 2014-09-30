#include "almsys.h"
#include <windowsx.h>
#include "edcfgdlg.h"
#include "cfglib.h"
#include "mdiapp.h"
#include "fileobj.h"
#include "fnameobj.h"

BOOL CALLBACK EditConfigDlgProc(
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
	BOOL		 result;

	result = DialogBoxParam(hAppInstance, "EDIT_CONFIG", hWndParent,
														EditConfigDlgProc, (LPARAM) editConfigData);


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

BOOL CALLBACK EditConfigDlgProc(
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
			{
			WORD		wNotify	= GET_WM_COMMAND_CMD(wParam, lParam);
			WORD		wID			= GET_WM_COMMAND_ID(wParam, lParam);

			switch (wID)
				{
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					break;

				case IDOK:
					{
					HWND	hListBox	= GetDlgItem(hDlg, LB_CONFIGLIST);

					itemID = ListBox_GetCurSel(hListBox);
					if (itemID != LB_ERR)
						{
						data->fileObject = (LPFILEOBJECT)ListBox_GetItemData(
																						hListBox, itemID);
						EndDialog(hDlg, TRUE);
						}
					else
						{
						data->fileObject = NULL;
						EndDialog(hDlg, FALSE);
						}
					break;
					}

				case LB_CONFIGLIST:
					switch (wNotify)
						{
						case LBN_DBLCLK:
							{
							HWND		hListBox	= GetDlgItem(hDlg, LB_CONFIGLIST);

							itemID = ListBox_GetCurSel(hListBox);

							if (itemID != LB_ERR)
								{
								data->fileObject = (LPFILEOBJECT)ListBox_GetItemData(
																			hListBox, itemID);
								EndDialog(hDlg, TRUE);
								}
							break;
							}

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
			}


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
	SFilePath			path;
	HWND					hCfgList;

	fileObject = NULL;
	hCfgList = GetDlgItem(hDlg, LB_CONFIGLIST);


	while (0 != (fileObject = cfgGetNextFileObject(fileObject)))
		{
		path = (const char*) fileObject->GetFilePath();
		path.AddToEnd(fileObject->GetName());
		itemID = ListBox_AddString(hCfgList, (const char*) path);
		ListBox_SetItemData(hCfgList, itemID, fileObject);
		ListBox_SetCurSel(hCfgList, 0);
		}
	}
