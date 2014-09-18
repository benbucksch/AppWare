#include "l_include.h"
#include "stdlib.h"

////////////////////////////////////////////////////////

LIST_VI::LIST_VI(
	OBJECTID 								oiList, 
	OBJECTID 								oiParent, 
	RECT* 									rcItem, 
	long 										status, 
	long													)	: OpHwndWndItem(oiList, oiParent, rcItem, status), m_attributes(FALSE)
	{
	LPLIST									list;

	list = LockListObject(Object());
	I_ASSERT_POINTER(list, return);
	list->CopyAttributes(&m_attributes);
	UnlockListObject(Object());

	m_ptLastClick.x = m_ptLastClick.y = 0;
	m_lTimeLastClick = 0L;

	OWndSetItemActivatable(this, TRUE);
	OWndSetItemEnabled(this, TRUE);
	OWndSetItemTransparent(this, FALSE);
	OWndSetItemUsesCursorKeys(this, TRUE);
	}

///////////

LIST_VI_LISTBOX::LIST_VI_LISTBOX(
	OBJECTID 								oiList, 
	OBJECTID 								oiParent, 
	RECT* 									rcItem, 
	long 										status, 
	long										special)	: LIST_VI(oiList, oiParent, rcItem, status, special)	
	{
	OWndSetItemTransparent(this, FALSE);
	}

///////////

LIST_VI_COMBOBOX::LIST_VI_COMBOBOX(
	OBJECTID 								oiList, 
	OBJECTID 								oiParent, 
	RECT* 									rcItem, 
	long 										status, 
	long										special)	: LIST_VI(oiList, oiParent, rcItem, status, special)	
	{
	OWndSetItemTransparent(this, TRUE);
	}

////////////////////////////////

void ALMAPI LIST_VI_LISTBOX::ParentOpened()
	{
	LPLIST 									list;
	LONG										style;
	HDC											hdc;
	
	list = LockListObject(Object());
	I_ASSERT_POINTER(list, return);

	style = WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_OWNERDRAWFIXED | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT;
	if (list->DoesMultipleSelections())
		style |= (LBS_EXTENDEDSEL|LBS_MULTIPLESEL);

	m_hwndItem = OWndCreateWindowEx(
			this,
			0,
			g_listboxObjectClassName,
			"",
			style,
			g_hInstance,
			this);

	list->FillListBox(m_hwndItem);
	SetSelections();
	UnlockListObject(Object());

	I_ASSERT_WINDOW(m_hwndItem, return);

	hdc = GetDC(m_hwndItem);
	m_attributes.AttrCreateFont(hdc);
	ReleaseDC(m_hwndItem, hdc);
	m_attributes.AttrCreateBrush();
	m_attributes.AttrSetFont(m_hwndItem);
	}

/////////////////////////////////

void ALMAPI LIST_VI_COMBOBOX::ParentOpened()
	{
	LPLIST									list;
	LONG										style;
	HDC											hdc;

	list = LockListObject(Object());
	I_ASSERT_POINTER(list, return);

	style = WS_CHILD | WS_BORDER | WS_VSCROLL | CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL | CBS_NOINTEGRALHEIGHT ;
	switch (list->GetType())
		{
		case OLIST_SIMPLECOMBO:						
			style |= CBS_SIMPLE; 											 
			break;
		case OLIST_DROPDOWNCOMBO:					
			style |= CBS_DROPDOWN; 										 
			break;
		case OLIST_DROPDOWNLIST:					
			style |= CBS_DROPDOWNLIST; 								
			break;
		}

	m_hwndItem = OWndCreateWindowEx(
			this,
			0,
			g_comboboxObjectClassName,
			"",
			style,
			g_hInstance,
			this);

	list->FillListBox(m_hwndItem);
	SetSelections();
	UnlockListObject(Object());

	I_ASSERT_WINDOW(m_hwndItem, return);

	hdc = GetDC(m_hwndItem);
	m_attributes.AttrCreateFont(hdc);
	ReleaseDC(m_hwndItem, hdc);
	m_attributes.AttrCreateBrush();
	m_attributes.AttrSetFont(m_hwndItem);
	}

////////////////////////////////////////////////

void ALMAPI LIST_VI::ParentClosing()
	{
	I_ASSERT_WINDOW(m_hwndItem, return);

	m_attributes.AttrDeleteFont();
	m_attributes.AttrDeleteBrush();
	DestroyWindow(m_hwndItem);
	m_hwndItem = 0;
	}

//////////////////////////

void ALMAPI LIST_VI::DataChanged(
	long, 
	long, 
	long										)
	{
	LPLIST 									list;

	if (m_hwndItem)
		{
		list = LockListObject(Object());
		I_ASSERT_POINTER(list, return);

		list->FillListBox(m_hwndItem);
		SetSelections();
	
		UnlockListObject(Object());
		}

	}

////////////////////////////////

void ALMAPI LIST_VI::Destroy()
	{
	delete this;
	}

////////////////////////////////////

BOOL ALMAPI LIST_VI::MeasureItem(
	MEASUREITEMSTRUCT* 			measureItem)
	{
	measureItem->itemHeight = m_attributes.AttrGetFontCellHeight();

	return TRUE;
	}

////////////////////////////////////

BOOL ALMAPI LIST_VI::DrawItem(DRAWITEMSTRUCT* drawItem)
	{
	LPLIST_TABLE 						table;
	LPSTR 									strings;
	LPSTR 									str;
	COLORREF 								oldTextColor, oldBackColor;
	HBRUSH 									brush;
	RECT 										rect;
	HFONT 									hFontOld;

	if (drawItem->itemAction & ODA_FOCUS)
		DrawFocusRect(drawItem->hDC, &drawItem->rcItem);
	else if (SizeofListTable(Object())/*not an empty list*/ )
		{
		table = LockListTable(Object());
		I_VERIFY_POINTER(table, return FALSE);

		strings = LockListStrings(Object());
		I_VERIFY_POINTER(strings, return FALSE);

		str = GetListString(drawItem->itemID, table, strings);

		if (drawItem->itemState & ODS_SELECTED)
			{
			oldTextColor = SetTextColor(drawItem->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT) );
			brush = CreateSolidBrush( GetSysColor(COLOR_HIGHLIGHT) );
			oldBackColor = SetBkColor(drawItem->hDC, GetSysColor(COLOR_HIGHLIGHT) );
			FillRect(drawItem->hDC, &drawItem->rcItem, brush );
			DeleteObject(brush);
			}
		else
			{
			oldTextColor = SetTextColor(drawItem->hDC, m_attributes.AttrGetForeground());
			oldBackColor = SetBkColor(drawItem->hDC, m_attributes.AttrGetBackground());
			FillRect(drawItem->hDC, &drawItem->rcItem, m_attributes.AttrGetBrushHandle() );
			}


		hFontOld = m_attributes.AttrSelectFont(drawItem->hDC);

		CopyRect(&rect, &drawItem->rcItem); 
		rect.left++;

		DrawText(drawItem->hDC, str, -1, &rect, DT_LEFT|DT_VCENTER);
		SetTextColor(drawItem->hDC, oldTextColor);
		SetBkColor(drawItem->hDC, oldBackColor);
		SelectObject(drawItem->hDC, hFontOld);

		UnlockListTable(Object());
		UnlockListStrings(Object());
		}

	return TRUE;
	}

/////////////////////////////

long ALMAPI LIST_VI::AdjustNewRect(
	RECT* 									newRect, 
	LONG 										adjustFlag)
	{
	LONG 										flags;

	//First, call the default method
	flags = OpHwndWndItem::AdjustNewRect(newRect, adjustFlag);

	if (flags & OWND_ADJRECT_DEFAULT_H)
		newRect->bottom = newRect->top + m_attributes.AttrGetHeight()*3;

	return flags;
	}

////////////////////////////////////////////////
 
LRESULT ALMAPI LIST_VI_LISTBOX::ParentNotification(
	HWND, 									
	UINT 										message, 
	UINT 										wParam,
	LPARAM 									lParam)
	{
  LPARAM									rVal = 0L;

	switch(message)
		{
		case WM_COMMAND :
			switch(HIWORD(lParam))
				{
				//case LBN_DBLCLK is not used to send double-clicked signal -- it is done by SendClickSignal
				case LBN_SELCHANGE: 
					rVal = SelectionsMaybeChanged();
					break;

				case LBN_SETFOCUS:
					AEvtPostSignalAtTail(Object(), I_SIG_ACTIVATED);
					rVal = TRUE;
					break;

				case LBN_KILLFOCUS:
					AEvtPostSignalAtTail(Object(), I_SIG_DEACTIVATED);
					rVal = TRUE;
					break;
				}
			break;

		case WM_MEASUREITEM :
			rVal = MeasureItem((MEASUREITEMSTRUCT*)lParam);
			break;

		case WM_DRAWITEM :
			rVal = DrawItem((DRAWITEMSTRUCT*)lParam);
			break;

		case WM_CTLCOLOR:
			SetTextColor((HDC)wParam, m_attributes.AttrGetForeground());
			SetBkColor((HDC)wParam, m_attributes.AttrGetBackground());
			rVal = (LRESULT) m_attributes.AttrGetBrushHandle();
			break;
		}

	return rVal;
	}		
////////////////////////////////////////////////
 
LRESULT ALMAPI LIST_VI_COMBOBOX::ParentNotification(
	HWND, 									
	UINT 										message, 
	UINT 										wParam,
	LPARAM 									lParam)
	{
  LPARAM									rVal = 0L;

	switch(message)
		{
		case WM_COMMAND :
			switch(HIWORD(lParam))
				{
				//case LBN_DBLCLK is not used to send double-clicked signal -- it is done by SendClickSignal
				case CBN_SELCHANGE: 
					rVal = SelectionsMaybeChanged();
					break;

				case CBN_SETFOCUS:
					AEvtPostSignalAtTail(Object(), I_SIG_ACTIVATED);
					rVal = TRUE;
					break;

				case CBN_KILLFOCUS:
					AEvtPostSignalAtTail(Object(), I_SIG_DEACTIVATED);
					rVal = TRUE;
					break;

				case CBN_DROPDOWN:  // from combobox
					SelectionsMaybeChanged();
					AEvtPostSignalAtTail(Object(), I_SIG_DOWN);
					break;

				case CBN_CLOSEUP:
					AEvtPostSignalAtTail(Object(), I_SIG_UP);
					break;
				}
			break;

		case WM_MEASUREITEM :
			rVal = MeasureItem((MEASUREITEMSTRUCT*)lParam);
			break;

		case WM_DRAWITEM :
			rVal = DrawItem((DRAWITEMSTRUCT*)lParam);
			break;

		case WM_CTLCOLOR:
			SetTextColor((HDC)wParam, m_attributes.AttrGetForeground());
			SetBkColor((HDC)wParam, m_attributes.AttrGetBackground());
			rVal = (LRESULT) m_attributes.AttrGetBrushHandle();
			break;
		}

	return rVal;
	}		
	
	
//////////////////////////////////

void LIST_VI::AppearanceChanged(
	HWND										hwnd)
	{
	LPLIST									list;

	if (hwnd != m_hwndItem) //don't update the one who caused the change
		{
		list = LockListObject(Object());
		I_VERIFY_POINTER(list, return);

		SetSelections();

		UnlockListObject(Object());
		}
	}

//////////////////////////////////

void LIST_VI::EditControlChanged(
	HWND										hwnd)
	{

	if (hwnd != m_hwndItem) //don't update the one who caused the change
		SetSelections();

	}

////////////////////////////////////

LRESULT LIST_VI_LISTBOX::SelectionsMaybeChanged()
	{
	LPLIST									list;

	list = LockListObject(Object());
	I_ASSERT_POINTER(list, return 0L);

	if (GetSelections())
		{
		list->SendSelectionsToText();
		list->AppearanceChanged(m_hwndItem);
		AEvtPostSignalAtTail(Object(), I_SIG_SELECTION_CHANGED);
		}

	UnlockListObject(Object());

	return TRUE;
	}

/////////////////////////////

LRESULT LIST_VI_COMBOBOX::SelectionsMaybeChanged()
	{
	LPLIST									list;

	list = LockListObject(Object());
	I_ASSERT_POINTER(list, return 0L);

	if (GetSelections())
		{
		list->SendSelectionsToText();
		SetEditControlText();
		list->AppearanceChanged(m_hwndItem);
		AEvtPostSignalAtTail(Object(), I_SIG_SELECTION_CHANGED);
		}

	UnlockListObject(Object());

	return TRUE;
	}

/////////////////////////////

BOOL LIST_VI_LISTBOX::GetSelections()
	{
	LPLIST									list;
	LPLIST_TABLE 						table;
	int											i;
	BOOL										b, bHasChanged=FALSE;

	list = LockListObject(Object());
	I_VERIFY_POINTER(list, return FALSE);

	if (list->NumItems())
		{
		table = LockListTable(Object());
		I_VERIFY_POINTER(table, UnlockListObject(Object());return FALSE);

		for (i = 0; i < list->NumItems(); i++)
			{
			b = SendMessage(m_hwndItem, LB_GETSEL, i, 0L) ? TRUE : FALSE;
			if (table[i].bIsSelected != b)
				{
				table[i].bIsSelected = b;
				bHasChanged = TRUE;
				}
			}

		UnlockListTable(Object());
		}

	UnlockListObject(Object());

	return bHasChanged;
	}

/////////////////////////////

BOOL LIST_VI_COMBOBOX::GetSelections()
	{
	LPLIST									list;
	LPLIST_TABLE 						table;
	LPSTR										szListStrings, szComboEdit, szItem;
	int											iComboSelection;
	unsigned short					usEditLength;
	int											i;
	BOOL										b, bHasChanged=FALSE;

	list = LockListObject(Object());
	I_VERIFY_POINTER(list, return FALSE);

	if (list->NumItems())
		{

		table = LockListTable(Object());
	
		iComboSelection = (int)SendMessage(m_hwndItem, CB_GETCURSEL, 0, 0L);

		if (iComboSelection == CB_ERR)
			{ //text typed into the edit control -- find a match
			usEditLength = (unsigned short)SendMessage(m_hwndItem, WM_GETTEXTLENGTH, 0, 0l)+1;
			if (!usEditLength)
				usEditLength = 1; //if drag, then release not in box, and nothing selected usEditLength will be 0
			ResizeListEdit(Object(), usEditLength);
			szComboEdit = LockListEdit(Object());
			SendMessage(m_hwndItem, WM_GETTEXT, usEditLength, (LONG)szComboEdit);
			bHasChanged = list->SelectedString(szComboEdit);
			UnlockListEdit(Object());
			list->EditControlChanged(m_hwndItem);
			}
		else //something is selected 
			{
			// get the text for the edit control
			szListStrings = LockListStrings(Object());
			szItem = GetListString(iComboSelection, table, szListStrings);
			ResizeListEdit(Object(), lstrlen(szItem)+1);

			szComboEdit = LockListEdit(Object());
			lstrcpy(szComboEdit, szItem);
			UnlockListEdit(Object());
			list->EditControlChanged(m_hwndItem);

			for (i = 0; i < list->NumItems(); i++)
				{
				b = (iComboSelection==i);
				if (table[i].bIsSelected != b)
					{
					table[i].bIsSelected = b;
					bHasChanged = TRUE;
					}
				}

			UnlockListStrings(Object());
			}
		}

	UnlockListTable(Object());

	return bHasChanged;
	}

/////////////////////////////////////////////////

void LIST_VI::ReturnKeyPressed()
	{
	AEvtPostSignalAtTail(Object(), I_SIG_ENTERKEY);
	AEvtPostSignalAtTail(Object(), I_SIG_RETURNKEY);
	AEvtPostSignalAtTail(Object(), I_SIG_EDITED);
	}

////////////////////////////////////////////////////

void LIST_VI_COMBOBOX::ReturnKeyPressed()
	{
	LPLIST 									list;

	list = LockListObject(Object());
	I_ASSERT_POINTER(list, return);

 	if (GetSelections())
		{
		list->SendSelectionsToText();
		list->AppearanceChanged(NULL);
		AEvtPostSignalAtTail(Object(), I_SIG_SELECTION_CHANGED);
		}

	UnlockListObject(Object());
	LIST_VI::ReturnKeyPressed();
	}

////////////////////////////////////////////////////


LONG LIST_VI_LISTBOX::SetSelections()
	{
	LPLIST									list;
	LPLIST_TABLE 						table;
	int											i;
	int 										iNumSelected;
	MEMBLOCKID 							miSelections;
	LPINT 									iptrSelections;

	list = LockListObject(Object());
	I_VERIFY_POINTER(list, return FALSE);

	if (list->NumItems())
		{
		table = LockListTable(Object());
		I_VERIFY_POINTER(table, return FALSE);

		if (list->DoesMultipleSelections())
			{
			iNumSelected = (int)SendMessage(m_hwndItem, LB_GETSELCOUNT, 0, 0L) + 1; //for overflowbuffer
			miSelections = AMemAllocate(sizeof(int) * iNumSelected);
			iptrSelections = (LPINT) AMemLock(miSelections);

			SendMessage(m_hwndItem, LB_GETSELITEMS, iNumSelected-1, (LONG)iptrSelections);
			iptrSelections[iNumSelected-1] = -1;  //Nothing is equal to the overflow buffer item

			/// Add magic to descide whether or not to go forward or backward

			for (i = 0; i < list->NumItems(); i++)
				{
				if (*iptrSelections == i)
					{
					if (!table[i].bIsSelected)
						SendMessage(m_hwndItem, LB_SETSEL, FALSE, MAKELONG(i, 0) );
					iptrSelections++;
					}
				else if (table[i].bIsSelected)
					SendMessage(m_hwndItem, LB_SETSEL, TRUE, MAKELONG(i, 0) );
				}

			AMemUnlock(miSelections);
			AMemFree(miSelections);
			}
		else //a single selection listbox
			{
			for (i = 0; i < list->NumItems(); i++)
				{
				if (table[i].bIsSelected)
					{
					SendMessage(m_hwndItem, LB_SETCURSEL, i, 0L);
					break ;
					}
				}
			if (i == list->NumItems())
				SendMessage(m_hwndItem, LB_SETCURSEL, -1, 0L); //nothing is selected
			}

		UnlockListTable(Object());
		}

	UnlockListObject(Object());
	return TRUE;
	}

////////////////////////////

LONG LIST_VI_COMBOBOX::SetSelections()
	{
	LPLIST									list;
	LPLIST_TABLE 						table;
	LPSTR										szListStrings, szItem;
	int											i;

	list = LockListObject(Object());
	I_VERIFY_POINTER(list, return FALSE);

	SetEditControlText();

	if (list->NumItems())
		{
		table = LockListTable(Object());
		szListStrings = LockListStrings(Object());

		for (i = 0; i < list->NumItems(); i++)
			{
			if (table[i].bIsSelected)
				{
				PostMessage(m_hwndItem, CB_SETCURSEL, i, 0L);
				szItem = GetListString(i, table, szListStrings);
				SendMessage(m_hwndItem, WM_SETTEXT, 0, (LONG)szItem);
				break ;
				}
			}
		if (i == list->NumItems())
			PostMessage(m_hwndItem, CB_SETCURSEL, -1, 0L); //nothing is selected

		UnlockListTable(Object());
		UnlockListStrings(Object());
		}

	UnlockListObject(Object());
	return TRUE;
	}

/////////////

void LIST_VI_COMBOBOX::SetEditControlText()
	{
	LPSTR										szComboEdit;

	if (SizeofListEdit(Object()))
		{
		szComboEdit = LockListEdit(Object());
		I_ASSERT_POINTER(szComboEdit, return);

	 	SendMessage(m_hwndItem, WM_SETTEXT, 0, (LONG)szComboEdit);
		UnlockListEdit(Object());
		}
	else
	 	SendMessage(m_hwndItem, WM_SETTEXT, 0, (LONG)"");
		
	
	}

//////////////////////////////////

void LIST_VI::SendClickSignal(
	POINT										pt)
	{
	LONG										lSignal = I_SIG_SINGLECLICKED;
	LONG										lTickCount;
	UINT										uTimeDiff;
	int											dx = pt.x - m_ptLastClick.x;
	int											dy = pt.y - m_ptLastClick.y;

	if (pt.x != -1  &&  pt.y != -1)
		{	//SetFocus for some kludge reason sends a WM_LBUTTONUP @{-1,-1} to the last active item -- thanks Micro$oft
		lTickCount = GetTickCount();
		uTimeDiff = (UINT)(lTickCount - m_lTimeLastClick);
		dx = abs(pt.x - m_ptLastClick.x);
		dy = abs(pt.y - m_ptLastClick.y);

		if (uTimeDiff < g_uDoubleClickTime  &&  dx < g_ptDoubleClick.x  &&  dy < g_ptDoubleClick.y)
			lSignal = I_SIG_DOUBLECLICKED;

		AEvtPostSignalAtTail(Object(), lSignal);

		m_lTimeLastClick = lTickCount;
		m_ptLastClick.x = pt.x;
		m_ptLastClick.y = pt.y;
		}
	}

//////////////////////////////////

void LIST_VI_COMBOBOX::SubclassComboboxPieces(
	HWND 										hwndChild)
	{
	LPLIST									list;
	char										szClassName[10];
	OLIST_TYPE							type;
				
	list = LockListObject(Object());
	I_VERIFY_POINTER(list, return);
	type = list->GetType();
	UnlockListObject(Object());

	GetClassName(hwndChild, szClassName, sizeof(szClassName));

	if (lstrcmp("Edit", szClassName)==0)
		{//only simple and drop down combo are subclassed -- they both subclass the edit piece to send edit signal
		g_defComboboxEditProc = (WNDPROC)GetWindowLong(hwndChild, GWL_WNDPROC); 
		SetWindowLong(hwndChild, GWL_WNDPROC, (long)NewComboboxEditProc);
		}
	else if (lstrcmp("ComboLBox", szClassName)==0  &&  type == OLIST_SIMPLECOMBO)
		{//only the simple combo sends click signals from the list piece
		g_defComboboxListProc = (WNDPROC)GetWindowLong(hwndChild, GWL_WNDPROC); 
		SetWindowLong(hwndChild, GWL_WNDPROC, (long)NewComboboxListProc);
		}
	}

//////////////////////////////////

LONG far pascal _export NewListboxProc (
	HWND 										hwnd, 
	UINT 										message, 
	UINT 										wParam, 
	LONG 										lParam)
	{
	LIST_VI_LISTBOX*				vi;

	switch(message)
		{
		case WM_CREATE:
			vi = (LIST_VI_LISTBOX*)(((CREATESTRUCT*)lParam)->lpCreateParams);
			vi->m_hwndItem = hwnd;
			SetListboxThisPointer(hwnd, vi);
			break;

		case WM_CHAR:
			if (wParam == VK_RETURN)
				{
				vi = GetListboxThisPointer(hwnd);
				vi->ReturnKeyPressed();
				}
			return FALSE;

		case WM_LBUTTONUP:
			vi = GetListboxThisPointer(hwnd);
			vi->SendClickSignal(MAKEPOINT(lParam));
			break;
		}

	return CallWindowProc(g_defListboxProc, hwnd, message, wParam, lParam);
	}

//////////////////////////////////

LONG far pascal _export NewComboboxProc (
	HWND 										hwnd, 
	UINT 										message, 
	UINT 										wParam, 
	LONG 										lParam)
	{
	LIST_VI_COMBOBOX*				vi;
	LONG										rVal;

	switch(message)
		{
    case WM_CREATE:
			vi = (LIST_VI_COMBOBOX*)(((CREATESTRUCT*)lParam)->lpCreateParams);
			vi->m_hwndItem = hwnd;
			SetComboboxThisPointer(hwnd, vi);
			break;

		case WM_LBUTTONUP:
				vi = GetComboboxThisPointer(hwnd);
				I_VERIFY_POINTER(vi, break);
				SendMessage(hwnd, WM_SETREDRAW, FALSE, 0L);
				rVal = CallWindowProc(g_defComboboxProc, hwnd, message, wParam, lParam);
				SendMessage(hwnd, WM_SETREDRAW, TRUE, 0L);
				vi->SetSelections();
				return rVal;
			
		case WM_PARENTNOTIFY:
			if (wParam == WM_CREATE)
				{
				vi = GetComboboxThisPointer(hwnd);
				I_VERIFY_POINTER(vi, break);
				vi->SubclassComboboxPieces((HWND)LOWORD(lParam));
				}
			return FALSE;
		}

	return CallWindowProc(g_defComboboxProc, hwnd, message, wParam, lParam);
	}

///////////////

LONG far pascal _export NewComboboxEditProc (
	HWND 										hwnd, 
	UINT 										message, 
	UINT 										wParam, 
	LONG 										lParam)
	{
	LIST_VI_COMBOBOX*				vi;

	if (message == WM_KEYDOWN  &&  wParam == VK_RETURN)
		{
		vi = GetComboboxThisPointer(GetParent(hwnd));
		if (vi)
			vi->ReturnKeyPressed();
		return FALSE;
		}
	else
		return CallWindowProc(g_defComboboxEditProc, hwnd, message, wParam, lParam);
	}

///////////////

LONG far pascal _export NewComboboxListProc (
	HWND 										hwnd, 
	UINT 										message, 
	UINT 										wParam, 
	LONG 										lParam)
	{
	LIST_VI_COMBOBOX*				vi;

	if (message == WM_LBUTTONUP)
		{
		vi = GetComboboxThisPointer(GetParent(hwnd));
		if (vi)
			vi->SendClickSignal(MAKEPOINT(lParam));
		}

	return CallWindowProc(g_defComboboxListProc, hwnd, message, wParam, lParam);
	}


////////////////////////////////////////////////////
///////////////////////////////////////////////////////

OpWndItemD* ALMAPI CreateListVI(
	OBJECTID 								oiList, 
	OBJECTID 								oiParent, 
	RECT* 									rcItem, 
	long 										status,
	long 										special)
	{
	LPLIST 									list;
	OpWndItemD*							wi;

	list = LockListObject(oiList);
	I_ASSERT_POINTER(list, return NULL);

	switch(list->GetType())
		{
		case OLIST_LISTBOX:
			wi = new LIST_VI_LISTBOX(oiList, oiParent, rcItem, status, special);
			break;
		case OLIST_SIMPLECOMBO:
		case OLIST_DROPDOWNCOMBO:
		case OLIST_DROPDOWNLIST:
			wi = new LIST_VI_COMBOBOX(oiList, oiParent, rcItem, status, special);
			break;
		case OLIST_UNKNOWN:
			wi = NULL;
			break;
		}

	UnlockListObject(oiList);

	I_ASSERT_POINTER(wi, );

	return wi;
	}

///////////////////////////////////////////////

BOOL ALMAPI ListViAppearanceChanged(
	OpWndItemD* 						wi, 
	long 										hwnd, 
	long 										)
	{
	((LIST_VI *)wi)->AppearanceChanged((HWND)hwnd);
	return TRUE;
	}

///////////////////////////////////////////////

BOOL ALMAPI ListViEditControlChanged(
	OpWndItemD* 						wi, 
	long 										hwnd, 
	long 										)
	{
	((LIST_VI *)wi)->EditControlChanged((HWND)hwnd);
	return TRUE;
	}

///////////////////////////////////////////////
