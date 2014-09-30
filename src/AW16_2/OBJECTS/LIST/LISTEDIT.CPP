#include <windows.h>
#include <mem.h>
#include <string.h>
#include <a_alm.h>
#include <op_wnd.h>
#include <o_text.h>
#include "attribut.h"
#include "dialogs.h"
#include "list.h"
//////////////////
BOOL validCopyObjectData(OBJECTID oiList, LONG dataBlock, MEMBLOCKID targetMID)
	{
	LONG dataSize = AObjGetDataSize(oiList, dataBlock);
	if (!dataSize)
		return FALSE;

	if (!(AMemSetSize(targetMID, dataSize)) )
		return FALSE;

	LPVOID lpDestination = AMemLock(targetMID) ;
	if (lpDestination == NULL )
		{
		AMemUnlock(targetMID) ;
		return FALSE;
		}
	LPVOID lpSource = AObjLockData(oiList, dataBlock);
	if(lpSource == NULL)
		{
		_fmemset(lpDestination, 0, (int)dataSize);
		AMemUnlock(targetMID) ;
		AObjUnlockData(oiList, dataBlock) ;
		return FALSE;
		}
	hmemcpy(lpDestination, lpSource, dataSize);
	AMemUnlock(targetMID) ;
	AObjUnlockData(oiList, dataBlock) ;
	return TRUE;
	}

EDIT_LIST::EDIT_LIST(OBJECTID oiList)
	{
	AObjSetDataCount(oiList, NUM_LIST_DATAS);
	m_curFocus = 0;
	m_lstMID = AMemAllocate(sizeof(LIST) );
	m_lst = (LPLIST) AMemLock(m_lstMID);
	LPLIST lpList = (LPLIST)AObjLockData(oiList, LIST_INFO);
	_fmemcpy(m_lst, lpList, sizeof(LIST) );
	AObjUnlockData(oiList, LIST_INFO);

	//Make sure one of the list styles is set DEFAULT= LF_LISTBOX 0x0004
	if (!(m_lst->m_style & LF_LIST_STYLES) )
		m_lst->m_style |= LF_LISTBOX;

	m_tableMID = AMemAllocate(0) ;
	if (!validCopyObjectData(oiList, TABLE_INFO, m_tableMID))
		{
		LONG tableSize = AObjGetDataSize(oiList, TABLE_INFO);
		if (tableSize < sizeof(LIST_TABLE))
			tableSize = sizeof(LIST_TABLE) ;
		AMemSetSize(m_tableMID, tableSize);
		LPLIST_TABLE table = (LPLIST_TABLE) AMemLock(m_tableMID);
		_fmemset(table, 0, (int)tableSize);
		if (table)
			AMemUnlock(m_tableMID) ;
		}

	m_stringsMID = AMemAllocate(0) ;
	if (!validCopyObjectData(oiList, STRINGS_INFO, m_stringsMID))
		{
		LONG stringsSize = AObjGetDataSize(oiList, STRINGS_INFO);
		AMemSetSize(m_stringsMID, stringsSize);
		LPSTR strings = (LPSTR) AMemLock(m_stringsMID);
		_fmemset(strings, 0, (int)stringsSize);
		if (strings)
			AMemUnlock(m_stringsMID) ;
		}
	}

/////////////////////

EDIT_LIST::~EDIT_LIST()
	{
	AMemUnlock(m_lstMID) ;
	AMemFree(m_lstMID);
	AMemFree(m_tableMID);
	AMemFree(m_stringsMID);
	}

////////////////////
void EDIT_LIST::CheckBoxHit(
	int                     item,
	LONG                    bitMask)
	{
	if (SendDlgItemMessage(m_hWndDlg, item, BM_GETCHECK, 0, 0L))
		m_lst->m_style |= bitMask;
	else
		m_lst->m_style &= ~bitMask;
	}

/////////////////

void EDIT_LIST::RadioButtonClicked(
	LONG 			setMask,
	LONG			groupMask)  //The | of all masks for the group
	{
	m_lst->m_style &= ~groupMask;			//Clear the masks of the groups
	m_lst->m_style |= setMask;				//Set the one bit of the clicked button

	EnableProperControls();
	}

/////////////////

void EDIT_LIST::EnableProperControls()
	{
	EnableWindow(GetDlgItem(m_hWndDlg, CB_MULTISELECT), (BOOL)(m_lst->m_style & LF_LISTBOX)) ;
	}

/////////////////

BOOL EDIT_LIST::AddItem(LPSTR str, int idx)
	{
	m_lst->m_nItems++;

	LONG tableSize = m_lst->m_nItems * sizeof(LIST_TABLE);
	int	strLength = strlen(str)+1 ;
	m_lst->m_stringsSize += strLength ;

	AMemSetSize(m_tableMID, tableSize);
	AMemSetSize(m_stringsMID, m_lst->m_stringsSize);

	LPLIST_TABLE table = (LPLIST_TABLE) AMemLock(m_tableMID);
	if (table != NULL)
		memset(&table[m_lst->m_nItems-1], 0, sizeof(LIST_TABLE) );
	LPSTR strings = (LPSTR) AMemLock(m_stringsMID);

	if (table == NULL ||
		strings == NULL ||
		!insertListString(str, strLength, idx, m_lst->m_nItems, table, strings, m_lst->m_stringsSize) )
		{
		AMemLock(m_tableMID);
		AMemLock(m_stringsMID);
		return FALSE;
		}
	SendDlgItemMessage(m_hWndDlg, LB_STARTUPLIST, LB_ADDSTRING, 0, 0L);
	InvalidateRect( GetDlgItem(m_hWndDlg, LB_STARTUPLIST), NULL, TRUE);
	AMemLock(m_tableMID);
	AMemLock(m_stringsMID);
	return TRUE;
	}

/////////////////////////////////////////

BOOL EDIT_LIST::RemoveMultItems( 	//Takes a list of sort indexs to remove
	LPINT						array,
	int							num)
	{
	int							i;

	//Delete in reverse order
	for(i=num-1; i>=0; i--)
		RemoveItem(array[i]);

	return TRUE;
	}

/////////////////////////

BOOL EDIT_LIST::RemoveItem(int idx)
	{
	LPLIST_TABLE table = (LPLIST_TABLE)AMemLock(m_tableMID);
	LPSTR strings = (LPSTR)AMemLock(m_stringsMID);

	m_lst->m_stringsSize -= deleteListString(idx, m_lst->m_nItems, table, strings, m_lst->m_stringsSize);
	m_lst->m_nItems--;

	AMemUnlock(m_tableMID);
	AMemUnlock(m_stringsMID);

	AMemSetSize(m_tableMID, sizeof(LIST_TABLE)*m_lst->m_nItems);
	AMemSetSize(m_stringsMID, m_lst->m_stringsSize);

	SendDlgItemMessage(m_hWndDlg, LB_STARTUPLIST, LB_DELETESTRING, 0, 0L);
	SendDlgItemMessage(m_hWndDlg, LB_STARTUPLIST, LB_SETSEL, FALSE, MAKELONG(-1, 0) );
	InvalidateRect( GetDlgItem(m_hWndDlg, LB_STARTUPLIST), NULL, TRUE);
	return TRUE;
	}

/////////////////

void EDIT_LIST::Store()
	{
	if (m_lst->m_style & LF_COMBOS)
		m_lst->m_style &= ~LF_MULTISELECT;

	LPLIST_TABLE table = (LPLIST_TABLE) AMemLock(m_tableMID);
	LPSTR strings = (LPSTR) AMemLock(m_stringsMID);

	AObjSetData(m_lst->m_oiList, LIST_INFO, m_lst, sizeof(LIST) );
	AObjSetData(m_lst->m_oiList, TABLE_INFO, table, AMemGetSize(m_tableMID));
	AObjSetData(m_lst->m_oiList, STRINGS_INFO, strings, AMemGetSize(m_stringsMID) );

	AMemUnlock(m_tableMID);
	AMemUnlock(m_stringsMID);
	}

///////////////////////

void EDIT_LIST::InitDialog()
	{
	SendDlgItemMessage(m_hWndDlg, CB_MULTISELECT, BM_SETCHECK, (int)(m_lst->m_style & LF_MULTISELECT), 0L);

	if (m_lst->m_style & LF_SIMPLECOMBO )
		SendDlgItemMessage(m_hWndDlg, RB_SIMPLECOMBO, BM_SETCHECK, 1, 0L);
	else if (m_lst->m_style & LF_DROPDOWNCOMBO)
		SendDlgItemMessage(m_hWndDlg, RB_DROPDOWNCOMBO, BM_SETCHECK, 1, 0L);
	else if (m_lst->m_style & LF_DROPDOWNLIST)
		SendDlgItemMessage(m_hWndDlg, RB_DROPDOWNLIST, BM_SETCHECK, 1, 0L);
	else
		SendDlgItemMessage(m_hWndDlg, RB_LISTBOX, BM_SETCHECK, 1, 0L);

	m_lst->FillListBox(GetDlgItem(m_hWndDlg, LB_STARTUPLIST), TRUE);
	UpdateWindow(GetDlgItem(m_hWndDlg, LB_STARTUPLIST));

	FillTextObjectsList();
	EnableProperControls();
	}

//////////

void EDIT_LIST::FillTextObjectsList()
	{
	OBJECTID object;
	char tmpBuffer[30];
	BOOL selectionSet=FALSE;

	SendDlgItemMessage(m_hWndDlg, DD_TEXTOBJECTS, CB_RESETCONTENT, 0, 0L);

	SendDlgItemMessage(m_hWndDlg, DD_TEXTOBJECTS, CB_ADDSTRING, 0, (LONG)"(None)");
	SendDlgItemMessage(m_hWndDlg, DD_TEXTOBJECTS, CB_SETITEMDATA, 0, 0L);

	LONG cookie = AOBJ_GETFIRSTKEY;
	for (int i = 1; (object = AObjGetNextObject(OTYPE_TEXT, &cookie)) != 0; i++)
		{
		AObjGetName(object, tmpBuffer, 30);
		SendDlgItemMessage(m_hWndDlg, DD_TEXTOBJECTS, CB_ADDSTRING, 0, (LONG)tmpBuffer);
		SendDlgItemMessage(m_hWndDlg, DD_TEXTOBJECTS, CB_SETITEMDATA, i, object);
		if (object == m_lst->m_oiText)
			{
			SendDlgItemMessage(m_hWndDlg, DD_TEXTOBJECTS, CB_SETCURSEL, i, 0L);
			selectionSet=TRUE;
			}
		}

	if (!selectionSet)
		SendDlgItemMessage(m_hWndDlg, DD_TEXTOBJECTS, CB_SETCURSEL, 0, 0L);
	}

//////////

void EDIT_LIST::DrawListItem(LPDRAWITEMSTRUCT di)
	{
	if (di->itemID >= m_lst->m_nItems)
		return;

	LPLIST_TABLE table = (LPLIST_TABLE)AMemLock(m_tableMID);
	LPSTR strings = (LPSTR)AMemLock(m_stringsMID);

	int	itemIdx = di->itemID;
	LPSTR str = itemIdx ? &strings[(int)table[itemIdx-1].charIdx] : strings;

	HBRUSH highlightBrush;
	COLORREF oldBackColor, oldTextColor;
	if (di->itemState & ODS_SELECTED)
		{
		highlightBrush = CreateSolidBrush( GetSysColor(COLOR_HIGHLIGHT) );
		oldTextColor = SetTextColor(di->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT) );
		oldBackColor = SetBkColor(di->hDC, GetSysColor(COLOR_HIGHLIGHT) );
		}
	else
		{
//		highlightBrush = CreateSolidBrush( GetSysColor(COLOR_WINDOW) );
//		oldTextColor = SetTextColor(di->hDC, GetSysColor(COLOR_WINDOWTEXT) );
//		oldBackColor = SetBkColor(di->hDC, GetSysColor(COLOR_WINDOW) );

    // the above lines are replaced with the following lines because we are
    // using the BORLAND custom dialogs.  The list background MUST be light gray
		highlightBrush = CreateSolidBrush( RGB(192, 192, 192) );
		oldTextColor = SetTextColor(di->hDC, GetSysColor(COLOR_WINDOWTEXT) );
		oldBackColor = SetBkColor(di->hDC, RGB(192, 192, 192) );
		}

	FillRect(di->hDC, &di->rcItem, highlightBrush );
	HBRUSH oldBack = SelectObject(di->hDC, highlightBrush);
	HFONT oldFont = SelectObject(di->hDC, GetStockObject(SYSTEM_FONT) );

	DrawText(di->hDC, str, strlen(str), &di->rcItem, DT_LEFT);

	if (di->itemState & ODS_FOCUS)
		{
		FrameRect(di->hDC, &di->rcItem, GetStockObject(GRAY_BRUSH) );
		m_curFocus = di->itemID;
		}
	SetTextColor(di->hDC, oldTextColor);
	SetBkColor(di->hDC, oldBackColor);
	SelectObject(di->hDC, oldBack);
	SelectObject(di->hDC, oldFont);
	DeleteObject(highlightBrush);

	AMemUnlock(m_tableMID);
	AMemUnlock(m_stringsMID);
	}
