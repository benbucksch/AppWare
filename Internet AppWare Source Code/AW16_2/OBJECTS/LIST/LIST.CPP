#include <windows.h>
#include <mem.h>
#include <string.h>
#include <a_alm.h>
#include <op_wnd.h>
#include <o_wnd.h>
#include <o_text.h>
#include "attribut.h"
#include "list.h"

//////////////////////////////
LPVOID LIST::operator new(size_t size, OBJECTID id, LONG numDatas)
	{
	AObjSetDataCount(id, numDatas);
	AObjResizeData(id, 0, size);
	return AObjLockData(id, 0);
	}

LIST::LIST(OBJECTID id, LONG s)
	{
	m_style = s;
	m_nItems = 0L;
	m_stringsSize = 0L;
	m_oiList = id;
	m_oiText = 0;
	m_curSelection = 0;
	m_version = THE_VERSION;
	}

BOOL LIST::addItem(LPSTR itemString, int itemIndex)
	{
	m_nItems++;
	AObjResizeData(m_oiList, TABLE_INFO, m_nItems * sizeof(LIST_TABLE)) ;
	LPLIST_TABLE listTable = (LPLIST_TABLE) AObjLockData(m_oiList, TABLE_INFO) ;
	listTable[m_nItems-1].isSelected = FALSE ;

	int	strLength = strlen(itemString) + 1 ;
	m_stringsSize += strLength ;
	AObjResizeData(m_oiList, STRINGS_INFO, m_stringsSize) ;
	LPSTR strings = (LPSTR)AObjLockData(m_oiList, STRINGS_INFO) ;

	BOOL result = insertListString(itemString, strLength, itemIndex, m_nItems, listTable, strings, m_stringsSize) ;
	AObjUnlockData(m_oiList, TABLE_INFO) ;
	AObjUnlockData(m_oiList, STRINGS_INFO) ;
	return result ;
	}

////////

void LIST::RemoveItem(int itemIndex)
	{
	if (itemIndex >= m_nItems)
		{
		DebugBreak() ;
		return ;
		}
	LPLIST_TABLE listTable = (LPLIST_TABLE)AObjLockData(m_oiList, TABLE_INFO);

	LPSTR strings = (LPSTR)AObjLockData(m_oiList, STRINGS_INFO);
	m_stringsSize = AObjGetDataSize(m_oiList, STRINGS_INFO);

	int strLength = deleteListString(itemIndex, m_nItems, listTable, strings, m_stringsSize) ;
	AObjUnlockData(m_oiList, TABLE_INFO) ;
	AObjUnlockData(m_oiList, STRINGS_INFO) ;

	m_nItems--;
	if (m_nItems < m_curSelection)
		m_curSelection = m_nItems ;
	AObjResizeData(m_oiList, TABLE_INFO, sizeof(LIST_TABLE)*m_nItems) ;

	m_stringsSize -= strLength ;
	AObjResizeData(m_oiList, STRINGS_INFO, m_stringsSize) ;
	}

//////////////////////////////////////

void LIST::RemoveSelectedItems()
	{
	LPLIST_TABLE table = (LPLIST_TABLE)AObjLockData(m_oiList, TABLE_INFO);
	for (int i = m_nItems-1; i >= 0; i--)
		{
		if (table[i].isSelected)
			{
			AObjUnlockData(m_oiList, TABLE_INFO);
			RemoveItem(i);
			table = (LPLIST_TABLE)AObjLockData(m_oiList, TABLE_INFO);
			}
		}
	if (table)
		AObjUnlockData(m_oiList, TABLE_INFO);
	}

/////////////////////////////////////

void LIST::FillListBox(HWND	hWnd, BOOL forceLB) //Defaults to FALSE
	{
	SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);
	int	topIdx;
	if (m_style & LF_LISTBOX)
		topIdx = (int)SendMessage(hWnd, LB_GETTOPINDEX, 0, 0L);
	SendMessage(hWnd, m_style & LF_LISTBOX ? LB_RESETCONTENT:CB_RESETCONTENT, 0, 0L);

	if (m_style & LF_LISTBOX || forceLB)
		{
		topIdx = (int)SendMessage(hWnd, LB_GETTOPINDEX, 0, 0L);
		SendMessage(hWnd, LB_RESETCONTENT, 0, 0L);
		for (int i = 0; i < m_nItems; i++)	//Insert entrees -- strings are user drawn
			SendMessage(hWnd, LB_ADDSTRING, 0, 0L);
		SendMessage(hWnd, LB_SETTOPINDEX, topIdx, 0L);
		}
	else
		{
		SendMessage(hWnd, CB_RESETCONTENT, 0, 0L);
/*		for (int i = 0; i < m_nItems; i++)	//Insert entrees -- strings are user drawn
			SendMessage(hWnd, CB_ADDSTRING, 0, 0L) ;*/
		if (m_nItems > 0)
			{
			LPLIST_TABLE table = (LPLIST_TABLE)AObjLockData(m_oiList, TABLE_INFO);
			LPSTR strings = (LPSTR )AObjLockData(m_oiList, STRINGS_INFO);
			for (int i = 0; i < m_nItems; i++)
				SendMessage(hWnd, CB_ADDSTRING, 0, (LONG)GetListString(i, table, strings) );
			AObjUnlockData(m_oiList, TABLE_INFO);
			AObjUnlockData(m_oiList, STRINGS_INFO);
			}
		}
	SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);
	}

/////////////////////////////////////////////////

void LIST::GetSelections(HWND hWnd)
	{
	LPLIST_TABLE table = (LPLIST_TABLE)AObjLockData(m_oiList, TABLE_INFO);
	LPSTR strings = (LPSTR )AObjLockData(m_oiList, STRINGS_INFO);

	if (m_style & LF_LISTBOX)
		{  // if this control is a list box
		for (int i = 0; i < m_nItems; i++)
			table[i].isSelected = SendMessage(hWnd, LB_GETSEL, i, 0L) ? TRUE : FALSE;
		}
	else
		{  // if this control is a combo box
		int comboSelection = (int)SendMessage(hWnd, CB_GETCURSEL, 0, 0L);
		if (comboSelection == CB_ERR)
			{ //text typed into the edit control -- find a match
			int size = (int)SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0L) + 1 ;
			AObjResizeData(m_oiList, EDIT_INFO, size) ;
			LPSTR editText = (LPSTR)AObjLockData(m_oiList, EDIT_INFO);
			SendMessage(hWnd, WM_GETTEXT, size, (LONG)editText);
			for (int i = 0; i < m_nItems; i++)
				{
				LPSTR str = GetListString(i, table, strings);
				table[i].isSelected = lstrcmpi(editText, str) ? FALSE:TRUE;
				}
			AObjUnlockData(m_oiList, EDIT_INFO);
			}
		else
			{//something is selected -- get the text for the edit control
			LPSTR str = GetListString(comboSelection, table, strings);
			AObjResizeData(m_oiList, EDIT_INFO, lstrlen(str)+1);
			LPSTR editText = (LPSTR)AObjLockData(m_oiList, EDIT_INFO);
			lstrcpy(editText, str) ;
			AObjUnlockData(m_oiList, EDIT_INFO);

			for (int i = 0; i < m_nItems; i++)
				table[i].isSelected = (comboSelection==i) ? TRUE : FALSE;
			}
		}
	AObjUnlockData(m_oiList, TABLE_INFO);
	AObjUnlockData(m_oiList, STRINGS_INFO);
	}

//////////////////////////////////////////

void LIST::SetSelections(HWND hWnd)
	{
	if (m_style & LF_LISTBOX)
		{   // if this is a list box control
		if (m_style & LF_MULTISELECT)
			SetMultiSelections(hWnd);
		else
			SetSingleSelections(hWnd);
		}
	else  // if this is a conbo box control
		SetComboSelections(hWnd);
	}

////////////////////////

void LIST::SetMultiSelections(HWND hWnd)
	{
	int numSelected = (int)SendMessage(hWnd, LB_GETSELCOUNT, 0, 0L) + 1; //for overflowbuffer
	MEMBLOCKID selMID = AMemAllocate(sizeof(int) * numSelected);
	LPINT selArray = (LPINT) AMemLock(selMID);

	SendMessage(hWnd, LB_GETSELITEMS, numSelected-1, (LONG)selArray);
	selArray[numSelected-1] = -1;  //Nothing is equal to the overflow buffer item

	/// Add magic to descide whether or not to go forward or backward

	LPLIST_TABLE table = (LPLIST_TABLE)AObjLockData(m_oiList, TABLE_INFO);
	for (int i = 0; i < m_nItems; i++, table++)
		{
		if (*selArray == i)
			{
			if (!table->isSelected)
				SendMessage(hWnd, LB_SETSEL, FALSE, MAKELONG(i, 0) );
			selArray++;
			}
		else if (table->isSelected)
			SendMessage(hWnd, LB_SETSEL, TRUE, MAKELONG(i, 0) );
		}
	AObjUnlockData(m_oiList, TABLE_INFO);
	AMemUnlock(selMID) ;
	AMemFree(selMID);
	}
///////////////////////////////////////
void LIST::SetSingleSelections(HWND	hWnd)
	{
	LPLIST_TABLE table = (LPLIST_TABLE)AObjLockData(m_oiList, TABLE_INFO);

	for (int i = 0; i < m_nItems; i++)
		{
		if (table[i].isSelected)
			{
			SendMessage(hWnd, LB_SETCURSEL, i, 0L);
			break ;
			}
		}
	if (i == m_nItems)
		SendMessage(hWnd, LB_SETCURSEL, -1, 0L); //nothing is selected

	if(table)
		AObjUnlockData(m_oiList, TABLE_INFO);
	}

///////////////////////////////////////

void LIST::SetComboSelections(HWND hWnd)
	{
	LPLIST_TABLE table = (LPLIST_TABLE)AObjLockData(m_oiList, TABLE_INFO);
	LPSTR strings = (LPSTR)AObjLockData(m_oiList, STRINGS_INFO);

	LPSTR editText = (LPSTR)AObjLockData(m_oiList, EDIT_INFO);
	if (editText) //stuff typed into the edit of a comboBox
		{
		SendMessage(hWnd, WM_SETTEXT, 0, (LONG)editText) ;
		AObjUnlockData(m_oiList, EDIT_INFO);
		}

	for (int i = 0; i < m_nItems; i++)
		{
		if (table[i].isSelected)
			{
			SendMessage(hWnd, CB_SETCURSEL, i, 0L);
			LPSTR str = GetListString(i, table, strings);
			SendMessage(hWnd, WM_SETTEXT, 0, (LONG)str);
			break ;
			}
		}
	if (i == m_nItems)
		SendMessage(hWnd, CB_SETCURSEL, -1, 0L); //nothing is selected

	AObjUnlockData(m_oiList, STRINGS_INFO);
	AObjUnlockData(m_oiList, TABLE_INFO);
	}
///////////////////////////////////////////
void LIST::SelectedString(LPSTR	str)
	{
	LPLIST_TABLE table = (LPLIST_TABLE)AObjLockData(m_oiList, TABLE_INFO);
	LPSTR strings = (LPSTR)AObjLockData(m_oiList, STRINGS_INFO);

	for (int i = 0; i < m_nItems; i++)
		table[i].isSelected =
			lstrcmp(GetListString(i, table, strings), str) ? FALSE : TRUE;

	AObjUnlockData(m_oiList, STRINGS_INFO);
	AObjUnlockData(m_oiList, TABLE_INFO);
	SendSelectionToText();
	}
//////////////////////////////////////////
void LIST::SendSelectionToText()
	{
	if (!m_oiText)
		return;

	LPLIST_TABLE table = (LPLIST_TABLE) AObjLockData(m_oiList, TABLE_INFO);
	LPSTR strings = (LPSTR) AObjLockData(m_oiList, STRINGS_INFO);

	MEMBLOCKID textMID = AMemAllocate(1);
	LPSTR text = (LPSTR) AMemLock(textMID);
	text[0] = 0;

	for (int i = 0; i < m_nItems; i++)
		{
		if (!table[i].isSelected) continue;
		LPSTR str = GetListString(i, table, strings);
		AMemUnlock(textMID);
		AMemSetSize(textMID, AMemGetSize(textMID)+strlen(str)+2);
		text = (LPSTR) AMemLock(textMID);
		lstrcat(text, str);
		text[lstrlen(text)+2] = 0;
		text[lstrlen(text)+1] = 10;
		text[lstrlen(text)] = 13;
		}
	if (text[0] != 0) // if at least one row is selected
		text[lstrlen(text)-2] = 0;
	OTextSetValueFromMemblock(m_oiText, textMID);
	AMemUnlock(textMID) ;
	AMemFree(textMID);
	AObjUnlockData(m_oiList, STRINGS_INFO);
	AObjUnlockData(m_oiList, TABLE_INFO);
	}
