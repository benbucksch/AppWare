////////////////////////////////////////////////////////
/////////  List Object Definition Routines    ////////
////////////////////////////////////////////////////////
#include <windows.h>
#include <string.h>
#include <alloc.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <helpids.h>
#include <op_wnd.h>
#include "attribut.h"
#include "dialogs.h"
#include "list.h"

LONG EditListObject(OBJECTID oiList)
	{
	AObjResizeData(oiList, LIST_INFO, sizeof(LIST) );
	int done = DialogBoxParam(hInstance, MAKEINTRESOURCE(LIST_SETUP_DIALOG),
			GetActiveWindow(), (FARPROC)Lst_SetupDlg, oiList);
	if(done == TRUE)
		return A_MODIFIED ;
	else
		return A_OK ;
	}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
BOOL InitDialog(HWND hWnd, OBJECTID oiList)
	{
	LPEDIT_LIST	editList = new EDIT_LIST(oiList);
	SetWindowLong(hWnd, DWL_USER, (long)editList) ;
	editList->m_hWndDlg = hWnd;
	editList->InitDialog();
	char objectName[80];
	AObjGetName(oiList, objectName, sizeof(objectName));
	SetWindowText(hWnd, objectName);

	AUtlSetTitleFont(OTYPE_LIST, NULL, GetDlgItem(hWnd, ST_OBJECTNAME));

	SetFocus( GetDlgItem(hWnd, ED_ENTRY) );
	SendDlgItemMessage(hWnd, ED_ENTRY, EM_SETSEL, 0, MAKELONG(0, -1));
	return FALSE;
	}

BOOL Destroy(HWND hWnd)
	{
	LPEDIT_LIST	editList = (LPEDIT_LIST)GetWindowLong(hWnd, DWL_USER);
	delete editList;
//	RemoveDialogPtr1(hWnd);
	return FALSE;
	}

BOOL MeasureItem(UINT wParam, LPMEASUREITEMSTRUCT lpMeasItem)
	{
	TEXTMETRIC tm;

	if (wParam != LB_STARTUPLIST)
		return FALSE ;
	HDC hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	SelectObject(hdc, GetStockObject(SYSTEM_FONT));
	GetTextMetrics(hdc, &tm);
	DeleteDC(hdc);
	lpMeasItem->itemHeight = tm.tmHeight;
	return FALSE ;
	}

BOOL DrawItem(HWND hWnd, LPDRAWITEMSTRUCT lpDrawItem)
	{
	if (lpDrawItem->CtlID == LB_STARTUPLIST)
		{
		LPEDIT_LIST	editList = (LPEDIT_LIST)GetWindowLong(hWnd, DWL_USER) ;
		editList->DrawListItem(lpDrawItem);
		}
	return TRUE;
	}

LRESULT far pascal _export Lst_SetupDlg(HWND hWnd, UINT msg,
		WPARAM wParam, LPARAM lParam )
	{
	switch(msg)
		{
		case WM_INITDIALOG:
			return InitDialog(hWnd, lParam) ;

		case WM_DESTROY:
			return Destroy(hWnd) ;

		case WM_MEASUREITEM:
			return MeasureItem(wParam, (LPMEASUREITEMSTRUCT)lParam) ;

		case WM_DRAWITEM:
			return DrawItem(hWnd, (LPDRAWITEMSTRUCT)lParam) ;

		case WM_COMMAND:
			LPEDIT_LIST editList = (LPEDIT_LIST) GetWindowLong(hWnd, DWL_USER) ;
			switch(wParam)
				{
				case CB_MULTISELECT:
					editList->CheckBoxHit(wParam, LF_MULTISELECT);
					break;

				case RB_LISTBOX:
					if (HIWORD(lParam) == BN_CLICKED)
						if (SendDlgItemMessage(hWnd, wParam, BM_GETCHECK, 0, 0L))
							editList->RadioButtonClicked(LF_LISTBOX, LF_LIST_STYLES);
					break;
				case RB_SIMPLECOMBO:
					if (HIWORD(lParam) == BN_CLICKED)
						if (SendDlgItemMessage(hWnd, wParam, BM_GETCHECK, 0, 0L))
							editList->RadioButtonClicked(LF_SIMPLECOMBO, LF_LIST_STYLES);
					break;
				case RB_DROPDOWNCOMBO:
					if (HIWORD(lParam) == BN_CLICKED)
						if (SendDlgItemMessage(hWnd, wParam, BM_GETCHECK, 0, 0L))
							editList->RadioButtonClicked(LF_DROPDOWNCOMBO, LF_LIST_STYLES);
					break;
				case RB_DROPDOWNLIST:
					if (HIWORD(lParam) == BN_CLICKED)
						if (SendDlgItemMessage(hWnd, wParam, BM_GETCHECK, 0, 0L))
							editList->RadioButtonClicked(LF_DROPDOWNLIST, LF_LIST_STYLES);
					break;


				case DD_TEXTOBJECTS:
					if (HIWORD(lParam) != CBN_SELCHANGE) break;
					int i = (int)SendDlgItemMessage(hWnd, DD_TEXTOBJECTS, CB_GETCURSEL, 0, 0L);
					editList->m_lst->m_oiText = SendDlgItemMessage(hWnd, DD_TEXTOBJECTS, CB_GETITEMDATA, i, 0L);
					return TRUE;

				case ED_ENTRY:
					if (HIWORD(lParam) == EN_SETFOCUS)
						SendDlgItemMessage(hWnd, ED_ENTRY, EM_SETSEL, TRUE, MAKELONG(0,-1) );
					break;

				case PB_ADD:
					int len = (int)SendDlgItemMessage(hWnd, ED_ENTRY, WM_GETTEXTLENGTH, 0, 0L)+1;
					LPSTR str = (LPSTR)farmalloc(len+1);
					SendDlgItemMessage(hWnd, ED_ENTRY, WM_GETTEXT, len, (LPARAM)str);
					int numItems = (int)SendDlgItemMessage(hWnd, LB_STARTUPLIST, LB_GETCOUNT, 0, 0L);
					editList->AddItem(str, numItems);
					farfree(str);
					SetFocus( GetDlgItem(hWnd, ED_ENTRY) );
					break;

				case PB_REMOVE:
					numItems = (int)SendDlgItemMessage(hWnd, LB_STARTUPLIST, LB_GETSELCOUNT, 0, 0L);
					LPINT intArray = (LPINT)farmalloc(sizeof(int)*numItems);
					SendDlgItemMessage(hWnd, LB_STARTUPLIST, LB_GETSELITEMS, numItems, (LONG)intArray);
					editList->RemoveMultItems(intArray, numItems);
					farfree(intArray);
					break;

				case IDOK:
					editList->Store();
					EndDialog(hWnd, TRUE);
					break;

				case IDCANCEL:
					EndDialog(hWnd, FALSE);
					break;

				case PB_FONTS:
					editList->m_lst->m_attributes.EditFont(hWnd);
					break;

				case IDHELP:
					WinHelp(hWnd, "serhelp.hlp", HELP_CONTEXT, HELPID_OBJD_List);
					break;
				}
			break;
		}

	return FALSE;
	}


