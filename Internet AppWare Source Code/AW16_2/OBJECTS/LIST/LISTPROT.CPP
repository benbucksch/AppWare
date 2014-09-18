#include <windows.h>
#include <mem.h>
#include <string.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <op_wnd.h>
#include <o_wnd.h>
#include <o_text.h>
#include "attribut.h"
#include "list.h"

LONG FAR PASCAL NewListboxProc (HWND hWnd, UINT message, UINT wParam, LONG lParam) ;
LONG FAR PASCAL NewComboboxProc (HWND hWnd, UINT message, UINT wParam, LONG lParam) ;

///////////////////////////////////////////////////////
OpWndItemD* FAR PASCAL CreateListVI(OBJECTID oiList, OBJECTID oiParent, RECT* rcItem, long status, long special)
	{
	LPLIST lst = (LPLIST)AObjLockData(oiList, LIST_INFO) ;
	LONG style = lst->m_style ;
	AObjUnlockData(oiList, LIST_INFO) ;

	if (style & LF_COMBOS) // if this object is a combo box
		return (LIST_VI*) new LIST_VI_COMBOBOX(oiList, oiParent, rcItem, status, special) ;
	else  // if this object is a list box
		return (LIST_VI*) new LIST_VI_LISTBOX(oiList, oiParent, rcItem, status, special) ;
	}
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
LIST_VI::LIST_VI(OBJECTID oiList, OBJECTID oiParent, RECT* rcItem, long status, long)
		: OpHwndWndItem(oiList, oiParent, rcItem, status)
	{
	OWndSetItemActivatable(this, TRUE);
	OWndSetItemEnabled(this, TRUE);
	OWndSetItemTransparent(this, FALSE);
//	OWndSetItemUsesDefaultKey(this, (txt->IsEditable() && txt->UsesEnter()) );
//	OWndSetItemUsesTab(this, (txt->IsEditable() && txt->UsesTab()) );
	OWndSetItemUsesCursorKeys(this, TRUE);
	}

void ALMAPI LIST_VI::Destroy()
	{
	delete this;
	}

////////////////////////////////
void ALMAPI LIST_VI_LISTBOX::ParentOpened()
	{
	LPLIST lst = (LPLIST)AObjLockData(m_oiObject, LIST_INFO) ;

	m_hfont = lst->m_attributes.CreateTheFont(NULL, TRUE);
	m_background = lst->m_attributes.GetBackground() ;
	m_hbrush = AUtlCreateSolidBrush(m_background) ;
	m_foreground = lst->m_attributes.GetForeground() ;

	LONG styleBits = WS_CHILD | WS_BORDER | WS_VSCROLL |
			LBS_OWNERDRAWFIXED | LBS_NOTIFY ;
	if (lst->m_style & LF_MULTISELECT)
		styleBits |= (LBS_EXTENDEDSEL|LBS_MULTIPLESEL);

	m_hwndItem = OWndCreateWindowEx(
			this,
			0,
			"ALM Listbox",
			"",
			styleBits,
			hInstance,
			this);

	lst->FillListBox(m_hwndItem) ;
	lst->SetSelections(m_hwndItem) ;
	AObjUnlockData(m_oiObject, LIST_INFO) ;
	}
/////////////////////////////////
void ALMAPI LIST_VI_COMBOBOX::ParentOpened()
	{
	LPLIST lst = (LPLIST)AObjLockData(m_oiObject, LIST_INFO) ;

	m_hfont = lst->m_attributes.CreateTheFont(NULL, TRUE);
	m_background = lst->m_attributes.GetBackground() ;
	m_hbrush = AUtlCreateSolidBrush(m_background) ;
	m_foreground = lst->m_attributes.GetForeground() ;

	LONG styleBits = WS_CHILD | WS_BORDER | WS_VSCROLL |
			CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | CBS_AUTOHSCROLL ;
//			CBS_OWNERDRAWFIXED | CBS_AUTOHSCROLL ;
	if (lst->m_style & LF_SIMPLECOMBO)
		styleBits |= CBS_SIMPLE;
	else if (lst->m_style & LF_DROPDOWNCOMBO)
		styleBits |= CBS_DROPDOWN;
	else
		styleBits |= CBS_DROPDOWNLIST;

	m_hwndItem = OWndCreateWindowEx(
			this,
			0,
			"ALM Combobox",
			"",
			styleBits,
			hInstance,
			this);
	lst->FillListBox(m_hwndItem) ;
	lst->SetSelections(m_hwndItem) ;
	SendMessage(m_hwndItem, WM_SETFONT, m_hfont, 0);
	AObjUnlockData(m_oiObject, LIST_INFO) ;
	}
////////////////////////////////////////////////
void ALMAPI LIST_VI::ParentClosing()
	{
	AUtlDeleteFont(m_hfont) ;
	AUtlDeleteBrush(m_hbrush) ;
	DestroyWindow(m_hwndItem);
	m_hwndItem = NULL;
	}
////////////////////////////////////////////////
LRESULT ALMAPI LIST_VI::ParentNotification(HWND, UINT message, UINT, LPARAM lParam)
	{
	switch(message)
		{
		case WM_MEASUREITEM :
			return MeasureItem((MEASUREITEMSTRUCT*)lParam) ;

		case WM_DRAWITEM :
			return DrawItem((DRAWITEMSTRUCT*)lParam) ;

		case WM_COMMAND :
			switch(HIWORD(lParam))
				{
				case LBN_DBLCLK:
					AEvtPostSignalAtTail(m_oiObject, DBLCLICKSIGNAL);
					return TRUE ;
				case LBN_SELCHANGE: // replaces WindMouseUp
					return selectionChanged() ;
				case LBN_ERRSPACE:
					return FALSE ;
				case LBN_SELCANCEL:
					return FALSE ;
				case LBN_SETFOCUS:
					AEvtPostSignalAtTail(m_oiObject, ACTIVATEDSIGNAL) ;
					return TRUE ;
				case LBN_KILLFOCUS:
					AEvtPostSignalAtTail(m_oiObject, DEACTIVATEDSIGNAL) ;
					return TRUE ;
				case CBN_DROPDOWN:  // from combobox
				case CBN_CLOSEUP:
					GetWindowRect(m_hwndItem, &m_rcItem);
//					RemapWindowPoints(HWND_DESKTOP, m_hwndParent, (LPPOINT)&m_rcItem, 2);
				}
			break ;
		}
	return FALSE ;
	}
////////////////////////////////////
BOOL ALMAPI LIST_VI::MeasureItem(MEASUREITEMSTRUCT* measureItem)
	{
	TEXTMETRIC tm;

	HDC hdc = CreateDC("Display", NULL, NULL, NULL);
	HFONT hFontOld = SelectObject(hdc, m_hfont) ;
	GetTextMetrics(hdc, &tm);
	SelectObject(hdc, hFontOld);
	DeleteDC(hdc) ;
	measureItem->itemHeight = tm.tmHeight ;
	return TRUE ;
	}

BOOL ALMAPI LIST_VI::DrawItem(DRAWITEMSTRUCT* drawItem)
	{
	if (drawItem->itemAction & ODA_FOCUS)
		{
		DrawFocusRect(drawItem->hDC, &drawItem->rcItem) ;
		return TRUE ;
		}

//	LPLIST list = (LPLIST)AObjLockData(m_oiObject, LIST_INFO);
	LPLIST_TABLE table = (LPLIST_TABLE)AObjLockData(m_oiObject, TABLE_INFO);
	LPSTR strings = (LPSTR)AObjLockData(m_oiObject, STRINGS_INFO);
	LPSTR str = GetListString(drawItem->itemID, table, strings);
	COLORREF oldTextColor, oldBackColor;
	HBRUSH brush;
	RECT rect;

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
		oldTextColor = SetTextColor(drawItem->hDC, m_foreground);
		oldBackColor = SetBkColor(drawItem->hDC, m_background);
		FillRect(drawItem->hDC, &drawItem->rcItem, m_hbrush );
		}

//	FillRect(drawItem->hDC, &drawItem->rcItem, brush );
//	DeleteObject(brush);

	HFONT oldFont = SelectObject(drawItem->hDC, m_hfont);
	CopyRect(&rect, &drawItem->rcItem); rect.left++;
	DrawText(drawItem->hDC, str, strlen(str), &rect, DT_LEFT|DT_VCENTER);
	SetTextColor(drawItem->hDC, oldTextColor);
	SetBkColor(drawItem->hDC, oldBackColor);
	SelectObject(drawItem->hDC, oldFont);

	AObjUnlockData(m_oiObject, STRINGS_INFO);
	AObjUnlockData(m_oiObject, TABLE_INFO);
//	AObjUnlockData(m_oiObject, LIST_INFO);
	return TRUE ;
	}

LRESULT LIST_VI::selectionChanged()
	{
	LPLIST lst = (LPLIST)AObjLockData(m_oiObject, LIST_INFO);
	lst->GetSelections(m_hwndItem) ;
	lst->SendSelectionToText() ;
//	lst->SetSelections(m_hwndItem) ;
	AObjUnlockData(m_oiObject, LIST_INFO) ;
	AEvtPostSignalAtTail(m_oiObject, CHANGEDSIGNAL) ;
	PostObjectChanged(m_oiObject, MAKELONG(LST_APPERANCE_CHANGED, m_hwndItem)) ;
	return TRUE ;
	}

long ALMAPI LIST_VI::AdjustNewRect(RECT* newRect, LONG adjustFlag)
	{
	LONG flags = OpHwndWndItem::AdjustNewRect(newRect, adjustFlag) ;
	if (flags & OWND_ADJRECT_DEFAULT_H)
		{
		TEXTMETRIC tm;

		HDC hdc = CreateDC("Display", NULL, NULL, NULL);

		LPLIST lst = (LPLIST)AObjLockData(m_oiObject, LIST_INFO) ;
		HFONT hFontNew = lst->m_attributes.CreateTheFont(hdc, TRUE);
		AObjUnlockData(m_oiObject, LIST_INFO) ;

		HFONT hFontOld = SelectObject(hdc, hFontNew) ;
		GetTextMetrics(hdc, &tm);
		SelectObject(hdc, hFontOld);
		AUtlDeleteFont(hFontNew) ;
		DeleteDC(hdc) ;
		POINT minSize ;
		minSize.x = 0 ;
		minSize.y = 3 * tm.tmHeight ;
		OWndDPtoLP(m_oiParent, &minSize, 1) ;

		if (minSize.y > (newRect->bottom - newRect->top))
			{
			newRect->bottom = newRect->top + minSize.y ;
			}
		}
	return flags ;
	}

void ALMAPI LIST_VI::Move(RECT* newRect)
	{
	OWndInvalidateLogicalRect(m_oiParent, &m_rcItem, TRUE) ;
	OpHwndWndItem::Move(newRect) ;
	}

/////////////////////////////////////////////////
void ALMAPI LIST_VI::DataChanged(long, long modifiers, long)
	{
	if (m_hwndItem == 0)
		return ;   // This visual instance is not yet visible

	LPLIST lst = (LPLIST)AObjLockData(m_oiObject, LIST_INFO);
	switch(LOWORD(modifiers))
		{
		case LST_ITEMS_CHANGED :
			lst->FillListBox(m_hwndItem) ;
			lst->SetSelections(m_hwndItem) ;
			break ;

		case LST_APPERANCE_CHANGED :
			if (HIWORD(modifiers) != m_hwndItem) //don't update the one who caused the change
				{
				lst->SetSelections(m_hwndItem);
//				lst->m_attributes.SetFont(m_hwndItem);
				}
			break ;

		case LST_EDITCONTROL_CHANGED :
			if (HIWORD(modifiers) != m_hwndItem) //don't update the one who caused the change
				{
				LPSTR pEdit = (LPSTR)AObjLockData(m_oiObject, EDIT_INFO);
				lst->SelectedString(pEdit);
				AObjUnlockData(m_oiObject, EDIT_INFO);
				lst->SetSelections(m_hwndItem);
				}
			break ;
		}
//	MaybeMoveWindow(m_hwndItem, m_hwndParent, &m_rcItem, TRUE);
//	SetWindowVisibility(m_hwndItem, wi->statusFlag);
//	SetWindowVisibility(this);
	AObjUnlockData(m_oiObject, LIST_INFO) ;
	}
//////////////////////////////////
LONG FAR PASCAL _export NewListboxProc (HWND hWnd, UINT message, UINT wParam, LONG lParam)
	{
	switch(message)
		{
		case WM_CREATE:
			{
			LIST_VI*	plistVI = (LIST_VI*)(((CREATESTRUCT*)lParam)->lpCreateParams);
			plistVI->m_hwndItem = hWnd;
			SetWindowLong(hWnd, listPointerOffset, (long)plistVI);
			SetWindowLong(hWnd, listObjectOffset, (long)plistVI->Object());
			}
			break ;
		case WM_CHAR:
			if (wParam == VK_RETURN)
				{
				LIST_VI* plistVI = (LIST_VI*)GetWindowLong(hWnd, listPointerOffset) ;
				plistVI->ReturnKeyPressed() ;
				}
			break ;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			OBJECTID oiList = (OBJECTID)GetWindowLong(hWnd, listObjectOffset) ;
			AEvtPostSignalAtTail(oiList, SINGLECLICKSIGNAL);
			break ;
		}
	return CallWindowProc(OldListboxProc, hWnd, message, wParam, lParam) ;
	}
//////////////////////////////////
LONG FAR PASCAL _export NewComboboxProc (HWND hWnd, UINT message, UINT wParam, LONG lParam)
	{
	switch(message)
		{
    case WM_CREATE:
			{
			LIST_VI*	plistVI = (LIST_VI*)(((CREATESTRUCT*)lParam)->lpCreateParams);
			plistVI->m_hwndItem = hWnd;
      SetWindowLong(hWnd, comboPointerOffset, (long)plistVI);
			SetWindowLong(hWnd, comboObjectOffset, (long)plistVI->Object());
			}
			break ;
		case WM_CHAR:
			if (wParam == VK_RETURN)
				{
				LIST_VI* plistVI = (LIST_VI*)GetWindowLong(hWnd, comboPointerOffset) ;
				plistVI->ReturnKeyPressed() ;
				}
			break ;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			OBJECTID oiList = (OBJECTID)GetWindowLong(hWnd, comboObjectOffset) ;
			AEvtPostSignalAtTail(oiList, SINGLECLICKSIGNAL);
			break ;
		}
	return CallWindowProc(OldComboboxProc, hWnd, message, wParam, lParam) ;
	}
/////////////////////////////////////////////////
void LIST_VI::ReturnKeyPressed()
	{
	LPLIST lst = (LPLIST)AObjLockData(m_oiObject, LIST_INFO);
	lst->SendSelectionToText();
	lst->SetSelections(m_hwndItem);
	AObjUnlockData(m_oiObject, LIST_INFO) ;

	PostObjectChanged(m_oiObject, MAKELONG(LST_EDITCONTROL_CHANGED, m_hwndItem)) ;

	AEvtPostSignalAtTail(m_oiObject, ENTERKEYSIGNAL);
	AEvtPostSignalAtTail(m_oiObject, RETURNKEYSIGNAL);
	AEvtPostSignalAtTail(m_oiObject, EDITEDSIGNAL);
	}
