#include "l_includ.h"
#include "l_dialog.h"


LRESULT far pascal _export List_SetupDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

extern LONG EditListObject(
	OBJECTID 								oiList)
	{

	return DialogBoxParam(g_hInstance, MAKEINTRESOURCE(LIST_SETUP_DIALOG),
			GetActiveWindow(), (DLGPROC)List_SetupDlg, (LONG)oiList) ? A_MODIFIED : A_OK ;
	}

///////////
LRESULT far pascal _export List_SetupDlg(
	HWND 										hwnd, 
	UINT 										msg,
	WPARAM 									wParam, 
	LPARAM 									lParam)
	{
	LPEDIT_LIST           	el;
	short										i, len, numItems;
	LPSTR										str;
	LPINT 									intArray;

	switch(msg)
		{
		case WM_INITDIALOG:
			el = new EDIT_LIST( (OBJECTID) lParam, hwnd);
			SetWindowLong(hwnd, DWL_USER, (long)el);
			el->InitDialog();
			SetFocus( GetDlgItem(hwnd, ED_ENTRY) );
			SendDlgItemMessage(hwnd, ED_ENTRY, EM_SETSEL, 0, MAKELONG(0, -1));
			AUtlSetTitleFont(OTYPE_LIST, NULL, GetDlgItem(hwnd, ST_OBJECTNAME) );
			AUtlCenterDialog(hwnd, 0);
			return FALSE;

		case WM_DESTROY:
			el = (LPEDIT_LIST) GetWindowLong(hwnd, DWL_USER);
			delete el;
			return FALSE;

		case WM_DRAWITEM:
			if (wParam == LB_STARTUPLIST)
				{
				el = (LPEDIT_LIST) GetWindowLong(hwnd, DWL_USER);
				I_VERIFY_POINTER(el, break);
				el->DrawListItem((LPDRAWITEMSTRUCT)lParam) ;
				}
			return TRUE;

		case WM_COMMAND:
			el = (LPEDIT_LIST) GetWindowLong(hwnd, DWL_USER);
			I_VERIFY_POINTER(el, break);
			switch(wParam)
				{
				case CB_MULTISELECT:
					el->CheckBoxHit(wParam, I_LF_MULTISELECT);
					break;

				case RB_LISTBOX:
					if (HIWORD(lParam) == BN_CLICKED)
						if (SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L))
							el->RadioButtonClicked(I_LF_LISTBOX, I_LF_LIST_STYLES);
					break;

				case RB_SIMPLECOMBO:
					if (HIWORD(lParam) == BN_CLICKED)
						if (SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L))
							el->RadioButtonClicked(I_LF_SIMPLECOMBO, I_LF_LIST_STYLES);
					break;

				case RB_DROPDOWNCOMBO:
					if (HIWORD(lParam) == BN_CLICKED)
						if (SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L))
							el->RadioButtonClicked(I_LF_DROPDOWNCOMBO, I_LF_LIST_STYLES);
					break;

				case RB_DROPDOWNLIST:
					if (HIWORD(lParam) == BN_CLICKED)
						if (SendDlgItemMessage(hwnd, wParam, BM_GETCHECK, 0, 0L))
							el->RadioButtonClicked(I_LF_DROPDOWNLIST, I_LF_LIST_STYLES);
					break;

				case DD_TEXTOBJECTS:
					if (HIWORD(lParam) == CBN_SELCHANGE)
						{
						i = (short)SendDlgItemMessage(hwnd, DD_TEXTOBJECTS, CB_GETCURSEL, 0, 0L);
						el->SetTextObject( (OBJECTID) SendDlgItemMessage(hwnd, DD_TEXTOBJECTS, CB_GETITEMDATA, i, 0L) );
						}
					break;

				case ED_ENTRY:
					if (HIWORD(lParam) == EN_SETFOCUS)
						SendDlgItemMessage(hwnd, ED_ENTRY, EM_SETSEL, TRUE, MAKELONG(0,-1) );
					break;

				case PB_ADD:
					len = (short)SendDlgItemMessage(hwnd, ED_ENTRY, WM_GETTEXTLENGTH, 0, 0L)+1;
					str = (LPSTR)farmalloc(len+1);
					SendDlgItemMessage(hwnd, ED_ENTRY, WM_GETTEXT, len, (LPARAM)str);
					numItems = (short)SendDlgItemMessage(hwnd, LB_STARTUPLIST, LB_GETCOUNT, 0, 0L);
					el->AddItem(str, numItems);
					farfree(str);
					SetFocus( GetDlgItem(hwnd, ED_ENTRY) );
					SendDlgItemMessage(hwnd, ED_ENTRY, EM_SETSEL, TRUE, MAKELONG(0,-1) );
					break;

				case PB_REMOVE:
					numItems = (int)SendDlgItemMessage(hwnd, LB_STARTUPLIST, LB_GETSELCOUNT, 0, 0L);
					intArray = (LPINT)farmalloc(sizeof(int)*numItems);
					SendDlgItemMessage(hwnd, LB_STARTUPLIST, LB_GETSELITEMS, numItems, (LONG)intArray);
					el->RemoveMultItems(intArray, numItems);
					farfree(intArray);
					break;

				case PB_OK:
					el->Store();
					EndDialog(hwnd, TRUE);
					break;

				case PB_CANCEL:
					EndDialog(hwnd, FALSE);
					break;

				case PB_FONTS:
					el->EditFont();
					break;
				}
			break;

		}

	return FALSE;
	}

///////////


EDIT_LIST::EDIT_LIST(
	OBJECTID 								oiList,
	HWND										hwnd)
	{
	LPLIST									sourceList;
	LPLIST_TABLE						sourceTable, destTable;
	LPSTR										sourceStrings, destStrings;
	long										lSize;

	m_hWndDlg = hwnd;

	//Copy in the source list
	m_miList = AMemAllocate(sizeof(LIST) );
	m_list = (LPLIST) AMemLock(m_miList);
	sourceList = LockListObject(oiList);
	_fmemcpy(m_list, sourceList, sizeof(LIST) );
	UnlockListObject(oiList);

	//Copy in the source listTable
	lSize = AObjGetDataSize(oiList, I_DATA_TABLE);
	m_miTable = AMemAllocate(lSize);
	if (lSize)
		{
		destTable = (LPLIST_TABLE)AMemLock(m_miTable);
		sourceTable = LockListTable(oiList);
		hmemcpy( (void huge*)destTable, (void huge*)sourceTable, lSize);
		UnlockListTable(oiList);
		AMemUnlock(m_miTable);
		}

	//Copy in the source listStrings
	lSize  = AObjGetDataSize(oiList, I_DATA_STRINGS);
	m_miStrings	= AMemAllocate(lSize);
	if (lSize)
		{
		destStrings = (LPSTR)AMemLock(m_miStrings);
		sourceStrings = LockListStrings(oiList);
		hmemcpy( (void huge*)destStrings, (void huge*)sourceStrings, lSize);
		UnlockListStrings(oiList);
		AMemUnlock(m_miStrings);
		}

	}

/////////////////////

EDIT_LIST::~EDIT_LIST()
	{
	AMemUnlock(m_miList);
	AMemFree(m_miList);
	AMemFree(m_miTable);
	AMemFree(m_miStrings);
	}

////////////////////

void EDIT_LIST::CheckBoxHit(
	int                     item,
	LONG                    bitMask)
	{
	if (SendDlgItemMessage(m_hWndDlg, item, BM_GETCHECK, 0, 0L))
		m_list->m_style |= bitMask;
	else
		m_list->m_style &= ~bitMask;
	}

/////////////////

void EDIT_LIST::RadioButtonClicked(
	LONG 										setMask,
	LONG										groupMask)  //The | of all masks for the group
	{
	m_list->m_style &= ~groupMask;			//Clear the masks of the groups
	m_list->m_style |= setMask;				//Set the one bit of the clicked button

	EnableProperControls();
	}

/////////////////

void EDIT_LIST::EnableProperControls()
	{
	EnableWindow(GetDlgItem(m_hWndDlg, CB_MULTISELECT), m_list->GetType() == OLIST_LISTBOX);
	}

/////////////////

BOOL EDIT_LIST::AddItem(
	LPSTR 									szItem, 
	int 										idxItem)
	{
	long										lTableSize;
	long										lPotentialStringsSize;
	int											iStrlenItem;	//doesn't include the '\0'
	LPLIST_TABLE						listTable;
	LPSTR										szListStrings;
	BOOL										rVal = FALSE;

	iStrlenItem = lstrlen(szItem);
	lPotentialStringsSize = m_list->m_lStringsSize + (LONG)iStrlenItem + 1L;

	if (lPotentialStringsSize < 65536L)
		{ 
		m_list->m_nItems++;

		lTableSize = m_list->m_nItems * sizeof(LIST_TABLE);

		m_list->m_lStringsSize = lPotentialStringsSize;

		AMemSetSize(m_miTable, lTableSize);
		AMemSetSize(m_miStrings, m_list->m_lStringsSize);

		listTable = (LPLIST_TABLE) AMemLock(m_miTable);
		memset(&listTable[m_list->m_nItems-1], 0, sizeof(LIST_TABLE) );

		szListStrings = (LPSTR) AMemLock(m_miStrings);

		rVal = InsertListStrings(szItem, iStrlenItem, idxItem, m_list->m_nItems, listTable, szListStrings, m_list->m_lStringsSize);

		SendDlgItemMessage(m_hWndDlg, LB_STARTUPLIST, LB_ADDSTRING, 0, 0L);
		InvalidateRect( GetDlgItem(m_hWndDlg, LB_STARTUPLIST), NULL, TRUE);

		AMemLock(m_miTable);
		AMemLock(m_miStrings);
		}

	return rVal;
	}

/////////////////////////////////////////

BOOL EDIT_LIST::RemoveMultItems( 	//Takes a list of sort indexs to remove
	LPINT										array,
	int											num)
	{
	int											i;

	//Delete in reverse order
	for(i=num-1; i>=0; i--)
		RemoveItem(array[i]);

	return TRUE;
	}

/////////////////////////

BOOL EDIT_LIST::RemoveItem(
	int 										idx)
	{
	LPLIST_TABLE table = (LPLIST_TABLE)AMemLock(m_miTable);
	LPSTR strings = (LPSTR)AMemLock(m_miStrings);

	m_list->m_lStringsSize -= deleteListString(idx, m_list->m_nItems, table, strings, m_list->m_lStringsSize);
	m_list->m_nItems--;

	AMemUnlock(m_miTable);
	AMemUnlock(m_miStrings);

	AMemSetSize(m_miTable, sizeof(LIST_TABLE)*m_list->m_nItems);
	AMemSetSize(m_miStrings, m_list->m_lStringsSize);

	SendDlgItemMessage(m_hWndDlg, LB_STARTUPLIST, LB_DELETESTRING, 0, 0L);
	SendDlgItemMessage(m_hWndDlg, LB_STARTUPLIST, LB_SETSEL, FALSE, MAKELONG(-1, 0) );
	InvalidateRect( GetDlgItem(m_hWndDlg, LB_STARTUPLIST), NULL, TRUE);
	return TRUE;
	}

/////////////////

void EDIT_LIST::Store()
	{
	LPLIST_TABLE 						listTable;
	LPSTR 									listStrings;

	if (m_list->GetType() != OLIST_LISTBOX)
		m_list->SetDoesMultipleSelections(FALSE);

	listTable = (LPLIST_TABLE) AMemLock(m_miTable);
	listStrings = (LPSTR) AMemLock(m_miStrings);

	AObjSetData(m_list->m_oiSelf, I_DATA_STYLE, m_list, sizeof(LIST) );
	AObjSetData(m_list->m_oiSelf, I_DATA_TABLE, listTable, AMemGetSize(m_miTable));
	AObjSetData(m_list->m_oiSelf, I_DATA_STRINGS, listStrings, AMemGetSize(m_miStrings) );

	AMemUnlock(m_miTable);
	AMemUnlock(m_miStrings);
	}

///////////////////////

void EDIT_LIST::InitDialog()
	{
	char										szObjectName[80];

	AObjGetName(m_list->m_oiSelf, szObjectName, 80);
  SetWindowText(m_hWndDlg, szObjectName);

	SendDlgItemMessage(m_hWndDlg, CB_MULTISELECT, BM_SETCHECK, m_list->DoesMultipleSelections(), 0L);

	switch(m_list->GetType())
		{
		case OLIST_SIMPLECOMBO:			SendDlgItemMessage(m_hWndDlg, RB_SIMPLECOMBO, BM_SETCHECK, 1, 0L); break;
		case OLIST_DROPDOWNCOMBO:		SendDlgItemMessage(m_hWndDlg, RB_DROPDOWNCOMBO, BM_SETCHECK, 1, 0L); break;
		case OLIST_DROPDOWNLIST:		SendDlgItemMessage(m_hWndDlg, RB_DROPDOWNLIST, BM_SETCHECK, 1, 0L); break;
		default:										m_list->SetType(OLIST_LISTBOX); //fall through
		case OLIST_LISTBOX:					SendDlgItemMessage(m_hWndDlg, RB_LISTBOX, BM_SETCHECK, 1, 0L); break;

		}

	m_list->FillListBox(GetDlgItem(m_hWndDlg, LB_STARTUPLIST), TRUE);
	UpdateWindow(GetDlgItem(m_hWndDlg, LB_STARTUPLIST));

	FillTextObjectsList();
	EnableProperControls();
	}

//////////

void EDIT_LIST::FillTextObjectsList()
	{
	OBJECTID 								oiText;
	char 										szTmp[30];
	BOOL 										bSelectionSet=FALSE;
	LONG 										cookie = AOBJ_GETFIRSTKEY;
	int											i;

	SendDlgItemMessage(m_hWndDlg, DD_TEXTOBJECTS, CB_RESETCONTENT, 0, 0L);

	SendDlgItemMessage(m_hWndDlg, DD_TEXTOBJECTS, CB_ADDSTRING, 0, (LONG)"(None)");
	SendDlgItemMessage(m_hWndDlg, DD_TEXTOBJECTS, CB_SETITEMDATA, 0, 0L);

	for (i = 1; (oiText = AObjGetNextObject(OTYPE_TEXT, &cookie)) != 0; i++)
		{
		AObjGetName(oiText, szTmp, sizeof(szTmp) );
		SendDlgItemMessage(m_hWndDlg, DD_TEXTOBJECTS, CB_ADDSTRING, 0, (LONG)szTmp);
		SendDlgItemMessage(m_hWndDlg, DD_TEXTOBJECTS, CB_SETITEMDATA, i, (LONG)oiText);
		if (oiText == m_list->m_oiText)
			{
			SendDlgItemMessage(m_hWndDlg, DD_TEXTOBJECTS, CB_SETCURSEL, i, 0L);
			bSelectionSet=TRUE;
			}
		}

	if (!bSelectionSet)
		SendDlgItemMessage(m_hWndDlg, DD_TEXTOBJECTS, CB_SETCURSEL, 0, 0L);		 
		
	}

//////////

void EDIT_LIST::DrawListItem(
	LPDRAWITEMSTRUCT 				di)
	{
	LPLIST_TABLE 						listTable;
	LPSTR 									szListStrings, szItem;
	int											idxItem;
	HBRUSH 									hBrushBack;
	COLORREF 								clrOldBack, clrOldText;

	if (di->itemID >= m_list->m_nItems)
		return;

	listTable = (LPLIST_TABLE)AMemLock(m_miTable);
	szListStrings = (LPSTR)AMemLock(m_miStrings);

	idxItem = di->itemID;
	szItem = GetListString(idxItem, listTable,	szListStrings);

	if (di->itemState & ODS_SELECTED)
		{
		hBrushBack = CreateSolidBrush( GetSysColor(COLOR_HIGHLIGHT) );
		clrOldText = SetTextColor(di->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT) );
		clrOldBack = SetBkColor(di->hDC, GetSysColor(COLOR_HIGHLIGHT) );
		}
	else
		{
    // Using the BORLAND custom dialogs.  The list background MUST be light gray
		hBrushBack = CreateSolidBrush( RGB(192, 192, 192) );
		clrOldText = SetTextColor(di->hDC, GetSysColor(COLOR_WINDOWTEXT) );
		clrOldBack = SetBkColor(di->hDC, RGB(192, 192, 192) );
		}

	FillRect(di->hDC, &di->rcItem, hBrushBack );
	hBrushBack = (HBRUSH)SelectObject(di->hDC, hBrushBack);

	DrawText(di->hDC, szItem, -1, &di->rcItem, DT_LEFT|DT_NOPREFIX);

	if (di->itemState & ODS_FOCUS)
		FrameRect(di->hDC, &di->rcItem, (HBRUSH)GetStockObject(GRAY_BRUSH) );

	SetTextColor(di->hDC, clrOldText);
	SetBkColor(di->hDC, clrOldBack);
	hBrushBack = (HBRUSH)SelectObject(di->hDC, hBrushBack);
	DeleteObject(hBrushBack);

	AMemUnlock(m_miTable);
	AMemUnlock(m_miStrings);
	}



