#include <windows.h>
#include "syntxdlg.h"
#include "syntax.h"
#include "dbstring.h"

BOOL FAR PASCAL _export SyntaxDescDlgProc(
	HWND hDlg,
	unsigned message,
	WORD wParam,
	LONG lParam
	);

static void InitListbox(
	LPSYNTAX	syntax,
	HWND			hListbox);

static void UpdateSyntax(
	LPSYNTAX	syntax,
	HWND			hListbox);

#define	MAXSYNTAXCHARS	100

#define VISIBLE_ITEMS_COUNT	10

#define BN_ADD			101
#define BN_REMOVE		119
#define BN_UPDATE		121

#define SLE_SYNTAX  123

#define LB_SYNTAX		122


static HANDLE		H_APP_INSTANCE;
static HANDLE		H_PREV_INSTANCE;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SyntaxDescDlgBox
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
BOOL SyntaxDescDlgBox(
	HANDLE				hAppInstance,
	HANDLE				hPrevInstance,
	HANDLE				hWndParent,
	LPSYNTAX			syntax)
	{
	FARPROC	 lpProc;
	BOOL		 result;

	H_APP_INSTANCE = hAppInstance;
	H_PREV_INSTANCE = hPrevInstance;

	lpProc = MakeProcInstance((FARPROC)SyntaxDescDlgProc, hAppInstance);
	result = DialogBoxParam(hAppInstance, "SYNTAX_DESC_DLG", hWndParent,
	 lpProc, (long)syntax);
	FreeProcInstance(lpProc);

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SyntaxDescDlgProc
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

BOOL FAR PASCAL _export SyntaxDescDlgProc(
	HWND hDlg,
	unsigned message,
	WORD wParam,
	LONG lParam
	)
	{
	static LPSYNTAX		syntax;
	BOOL							msgHandled = TRUE;
	char 							buffer[MAXSYNTAXCHARS + 1];
	long							itemId;
	long							topItem;
	HWND							hListbox;

	switch (message)
		{
		case WM_INITDIALOG:
			syntax = (LPSYNTAX)lParam;
			SendDlgItemMessage(hDlg, SLE_SYNTAX, EM_LIMITTEXT, MAXSYNTAXCHARS, 0L);
			SetDlgItemText(hDlg, SLE_SYNTAX, "");
			InitListbox(syntax, GetDlgItem(hDlg, LB_SYNTAX));
			SetFocus(GetDlgItem(hDlg, SLE_SYNTAX));
			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					break;

				case IDOK:
					UpdateSyntax(syntax, GetDlgItem(hDlg, LB_SYNTAX));
					EndDialog(hDlg, TRUE);
					break;

				case BN_ADD:
					hListbox = GetDlgItem(hDlg, LB_SYNTAX);
					SendDlgItemMessage(hDlg, SLE_SYNTAX, WM_GETTEXT, MAXSYNTAXCHARS+1,
					 (DWORD)buffer);
					itemId = SendMessage(hListbox, LB_ADDSTRING, 0, (DWORD)buffer);
					topItem = SendMessage(hListbox, LB_GETTOPINDEX, 0, 0);
					if ((itemId - topItem - 1) > VISIBLE_ITEMS_COUNT)
						SendMessage(hListbox, LB_SETTOPINDEX,
						 itemId - (VISIBLE_ITEMS_COUNT/2), 0L);
					SendMessage(hListbox, LB_SETCURSEL, itemId, 0L);
					SetFocus(GetDlgItem(hDlg, SLE_SYNTAX));
					SendDlgItemMessage(hDlg, SLE_SYNTAX, EM_SETSEL, 0,
					 MAKELONG(0, 32767));
					break;

				case BN_REMOVE:
					hListbox = GetDlgItem(hDlg, LB_SYNTAX);
					itemId = SendMessage(hListbox, LB_GETCURSEL, 0, 0L);
					if (itemId != LB_ERR)
						SendMessage(hListbox, LB_DELETESTRING, itemId, 0L);
					SetFocus(GetDlgItem(hDlg, SLE_SYNTAX));
					SendDlgItemMessage(hDlg, SLE_SYNTAX, EM_SETSEL, 0,
					 MAKELONG(0, 32767));
					break;

				case BN_UPDATE:
					hListbox = GetDlgItem(hDlg, LB_SYNTAX);
					itemId = SendMessage(hListbox, LB_GETCURSEL, 0, 0L);
					if (itemId != LB_ERR)
						{
						SendDlgItemMessage(hDlg, SLE_SYNTAX, WM_GETTEXT, MAXSYNTAXCHARS+1,
						 (DWORD)buffer);
						SendMessage(hListbox, LB_DELETESTRING, itemId, 0L);
						itemId = SendMessage(hListbox, LB_INSERTSTRING, itemId,
						 (DWORD)buffer);
						topItem =
						 SendMessage(hListbox, LB_GETTOPINDEX, 0, 0);
						if ((itemId - topItem - 1) > VISIBLE_ITEMS_COUNT)
							SendMessage(hListbox, LB_SETTOPINDEX,
							 itemId - (VISIBLE_ITEMS_COUNT/2), 0L);
						SendMessage(hListbox, LB_SETCURSEL, itemId, 0L);
						SetFocus(GetDlgItem(hDlg, SLE_SYNTAX));
						SendDlgItemMessage(hDlg, SLE_SYNTAX, EM_SETSEL, 0,
						 MAKELONG(0, 32767));
						}
					break;

				case LB_SYNTAX:
					if (HIWORD(lParam) == LBN_SELCHANGE)
						// get selected text from listbox and place in edit field
						{
						itemId = SendMessage(LOWORD(lParam), LB_GETCURSEL, 0, 0L);
						SendMessage(LOWORD(lParam), LB_GETTEXT, itemId, (DWORD)buffer);
						SendDlgItemMessage(hDlg, SLE_SYNTAX, WM_SETTEXT, 0,
						 (DWORD)buffer);
						}
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
// FUNCTION....: InitListbox
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
static void InitListbox(
	LPSYNTAX	syntax,
	HWND			hListbox)
	{
	int count;
	int i;

	count = syntax->GetCount();

	for (i=0; i<count; i++)
		SendMessage(hListbox, LB_ADDSTRING, 0,
		 (long)(LPSTR)syntax->GetSyntax(i));

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateListbox
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
static void UpdateSyntax(
	LPSYNTAX	syntax,
	HWND			hListbox)
	{
	int		newSyntaxCount;
	int 	i;
	char	buffer[MAXSYNTAXCHARS+1];

	newSyntaxCount = SendMessage(hListbox, LB_GETCOUNT, 0, 0L);

	syntax->Resize(newSyntaxCount);

	for (i=0; i<newSyntaxCount; i++)
		{
		SendMessage(hListbox, LB_GETTEXT, i, (long)buffer);
		syntax->Update(buffer, i);
		}


	}