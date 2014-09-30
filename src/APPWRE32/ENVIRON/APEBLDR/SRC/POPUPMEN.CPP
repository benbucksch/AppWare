#include "almsys.h"
#include <windowsx.h>

#include "popupmen.h"
#include "math.h"
#include "progsys.h"
#include "winlist.h"
#include "objdesc.h"
#include "funcdesc.h"
#include "sericon.h"

#define PM_BORDER 2
#define PM_CLASS "PMCLS"
#define PM_ARROWHEIGHT 8
#define PM_ARROWWIDTH 8

#define PM_FORMAT ( (((WORD)((BYTE)4))<<8) + \
	(WORD)(DT_SINGLELINE|DT_VCENTER|DT_LEFT |/*DT_NOPREFIX|DT_NOCLIP|*/ \
		DT_EXPANDTABS|DT_TABSTOP))

#define PM_TABPIXELS 20

#define PM_MESSES_UP_SELECTION 0x0118

#define PM_UPARROW 1
#define PM_DOWNARROW 2


#define PM_TIMER 100 
#define PM_TIMERINTERVAL 27  

#define PM_MAX_ITEM_LENGTH 256

#define PM_VERTTOLERANCE PM_ARROWHEIGHT

#define PM_ICONSIZE 20

#define POPUP_ICONS (POPUP_FUNCICONS | POPUP_OBJICONS)

BOOL            POPUPMENU::registered = FALSE;
WINDOWLIST * POPUPMENU::menuList = NULL;
WINDOWLIST * POPUPMENU::listboxList = NULL;
int							POPUPMENU::count=0;



/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  POPUPMENU
//
// CLASS.......:  POPUPMENU
//
// DESCRIPTION.:  Constructor: create popup window which contains the listbox
//									and create the listbox.  Both windows are initially
//									invisible.  The popupwindow is created with some default
//									size.  This size doesn't matter since the window will
//									be resized before it is shown.  The popup menu is owned
//									by the desktop.
//
// PARAMETERS..:  font:  handle to font used by menu
//								owner: window who receives a notification message when
//											 the popup menu is closed.  The owner window is
//											 responsible for deleting the popupmenu object.
//								type:  optional which defaults to PARENT_NOTIFY which
//											 means the parent window gets a message when the
//											 popup closes.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:  owner can only be NULL when the type is POPUP_MESSAGEHOG
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

POPUPMENU::POPUPMENU(
	HWND owner,
	HFONT font,
	long type):hFont(font), hWndOwner(owner), popupType(type)
	{
	WNDCLASS wc;
	HDC hdc;
	TEXTMETRIC tm;

	flags = 0;
	if(menuList == NULL)
		menuList = new WINDOWLIST;

	if(!listboxList)
		listboxList = new WINDOWLIST;  //list to keep track of all open menus

	count++;

	if(!registered)
		{
		wc.style=CS_SAVEBITS;
		wc.lpfnWndProc = POPUPMENU::WndProc;
		wc.hInstance = PROGSYSTEM::hInst;
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = GetStockBrush(WHITE_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = (LPSTR)PM_CLASS;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		RegisterClass((LPWNDCLASS)&wc);
		registered=TRUE;
		}


	longestItem = 0;
	hWndMenu = CreateWindow((LPSTR)PM_CLASS, NULL,
		WS_BORDER|WS_POPUP|WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		PROGSYSTEM::hInst,
		(LPSTR)this);

	//we need to know the itemHeight before creating the listbox
	hdc = GetDC(hWndMenu);
	SelectObject(hdc, hFont);
	GetTextMetrics(hdc, (LPTEXTMETRIC)&tm);
	ReleaseDC(hWndMenu, hdc);

	m_itemHeight = tm.tmHeight;
	m_textOffset.x = PM_BORDER;
	m_textOffset.y = 0;

	if((popupType & POPUP_ICONS) && (m_itemHeight < PM_ICONSIZE))
		{
		m_textOffset.x += PM_ICONSIZE;
		m_textOffset.y = (PM_ICONSIZE - m_itemHeight)/2;
		m_itemHeight = PM_ICONSIZE;
		}

	hListMenu = CreateWindow("listbox", NULL,
		WS_CHILD|LBS_OWNERDRAWFIXED|LBS_HASSTRINGS|
			(popupType & POPUP_SORTED ? LBS_SORT : 0L),
		CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,
		hWndMenu,
		(HMENU) POPUP_MENU_CODE,
		PROGSYSTEM::hInst,
		NULL);

	if(hFont)
		SendMessage(hListMenu, WM_SETFONT, (WPARAM) hFont, FALSE);
	
	hTimer =NULL;

	}

/////////////////////////////////////////////////////////////////////////
void POPUPMENU::PurgeList()
	{
	I_ASSERT(hTimer == NULL);

	longestItem = 0;

	ListBox_ResetContent(hListMenu);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ~POPUPMENU
//
// CLASS.......:  POPUPMENU
//
// DESCRIPTION.:  Destructor
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
POPUPMENU::~POPUPMENU()
	{
	if(hTimer)
		KillTimer(hListMenu, PM_TIMER);

	DestroyWindow(hWndMenu);

	count--;
	if(!count)
		{
		delete listboxList;
		delete menuList;
		listboxList = NULL;
		menuList = NULL;
		}

	}



/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  AppendItem
//
// CLASS.......:  POPUPMENU
//
// DESCRIPTION.:  Adds an item to the end of the menu.  The menu is still
//									not shown.
//
// PARAMETERS..:  item: 		pointer to the name of the menu item.
//								data:			data to be associated with the menu item
//								ordinal:  if set to NO_ORDINAL then only the item is shown,
//									else an ordianl number is associated with the item.  The
//									ordianl number is zero based.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void POPUPMENU::AppendItem(
	LPCSTR item,
	long  data,
	int ordinal)
	{
	char buffer[PM_MAX_ITEM_LENGTH];
	int index;
	HDC hdc;
	DWORD length;
	int tab=PM_TABPIXELS;

	if(ordinal != NO_ORDINAL)
		{
		wsprintf(buffer, "%lu%s%s",data,"\t",item);
		}
	else
		lstrcpy((LPSTR)buffer, item);

	hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	SelectObject(hdc, hFont);
	length = GetTabbedTextExtent(hdc, (LPSTR)buffer, lstrlen((LPSTR)buffer),
		1, (LPINT)&tab);
	DeleteDC(hdc);
	if(LOWORD(length) > longestItem)
		longestItem = LOWORD(length);
	index = (int)ListBox_AddString(hListMenu, buffer);
	ListBox_SetItemData(hListMenu, index, data);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ShowPopup
//
// CLASS.......:  POPUPMENU
//
// DESCRIPTION.:  Resizes the popupmenu to just be big enough to hold the
//									items and then shows the menu.  Note: it their are so
//									many items that they will extend off the screen, then
//									the outer window is created with enough extra vertical
//									space to hold directional arrows.
//
// PARAMETERS..:  leftPt:		upper left corner of the menu given in screen
//														coordinates.
//                selected: index of the item which should start out
//														highlighted.
//
// RETURN VALUE: returns TRUE if successful and if the style is
//							 message hog and a selection was made.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

BOOL POPUPMENU::ShowPopup(
	POINT leftPt,          //must be in screen coordinates
	long selected)
	{
	POINT location;
	int height;
	int xBorder = GetSystemMetrics(SM_CXBORDER);
	int yBorder = GetSystemMetrics(SM_CYBORDER);
	int xMaxSize = GetSystemMetrics(SM_CXSCREEN);
	int yMaxSize = GetSystemMetrics(SM_CYSCREEN);
	RECT clientRect;
	int y;
	int adjust;
	int space = 0;
	int maxCount = (int)ListBox_GetCount(hListMenu);
	MSG msg;
	long result;
	BOOL returnValue;
	int itemWidth = longestItem;
	int i;
  
	if(maxCount > 0)
		{
		returnValue =TRUE;

	if(popupType & POPUP_ICONS)
		itemWidth+=PM_ICONSIZE;

		//used to initialize selectedItem to selected but it seemed to make more
		//sense to initialize it to zero in case selected has some bogus value
		selectedItem = 0;
		selectedData = NULL;
		if(!(popupType & POPUP_MAXSIZE))
			{
			for(i=0; i<maxCount; i++)
				{
				if(selected == ListBox_GetItemData(hListMenu, i))
					{
					selectedItem = i;
					break;
					}
				}
			}

		height = (int)ListBox_GetCount(hListMenu) *
			m_itemHeight + (2 * (yBorder + PM_ARROWHEIGHT));

		location.x = leftPt.x-xBorder;

		if(popupType & POPUP_HCENTER)
			{
			location.x -= (itemWidth/2);
			if(location.x < 0)
				location.x = 0;
			}

		if(location.x + itemWidth > xMaxSize)
			location.x -= (location.x + itemWidth - xMaxSize);
		else if(location.x < 0)
			location.x  = 0;

		//get initial top point
		if(popupType & POPUP_MAXSIZE)
			{
			location.y = PM_BORDER;

			if(location.y > leftPt.y)
				location.y = leftPt.y;
			else if(location.y + height < leftPt.y)
				location.y = leftPt.y - height;
			}
		else
			location.y = leftPt.y - (m_itemHeight * selectedItem)- yBorder -
				PM_ARROWHEIGHT;

		y = PM_BORDER - location.y;

		if(y > 0)
			{
			//make sure top of menu on the screen
			adjust = (int)ceil((double)y / (double)m_itemHeight);
			location.y+=(adjust * m_itemHeight);
			height=height - (adjust * m_itemHeight);
			ListBox_SetTopIndex(hListMenu, adjust);
			flags|=PM_UPARROW;
			}

		y = location.y + height - (yMaxSize - PM_BORDER);

		if(y>0)
			{
			adjust = (int)ceil((double)y / (double)m_itemHeight);
			height = height - (adjust * m_itemHeight);
			flags|=PM_DOWNARROW;
			}
		if((!(flags & PM_DOWNARROW)) && (!(flags & PM_UPARROW)))
			{
			height-=(2 * PM_ARROWHEIGHT);
			location.y+=PM_ARROWHEIGHT;
			}
		else
			space = PM_ARROWHEIGHT;

		MoveWindow(hWndMenu, location.x, location.y,
			2 * (PM_BORDER + xBorder) + itemWidth ,
			height, FALSE);
		GetClientRect(hWndMenu, (LPRECT)&clientRect);
		MoveWindow(hListMenu, 0, space, clientRect.right,
			clientRect.bottom - space, FALSE);
		listboxList->Add(hListMenu, this);
		ScreenToClient(hListMenu, (LPPOINT)&leftPt);
		leftPt.x += (PM_BORDER + 1);
		leftPt.y += PM_BORDER;
		
		GetClientRect(hListMenu, (LPRECT)&clientRect);
		
		SendMessage(hListMenu, WM_MOUSEACTIVATE, (WPARAM) hWndMenu, MAKELPARAM(HTCLIENT,
			WM_LBUTTONDOWN));

		// send mouse down to message so it will think you clicked in its
		// client area

		if(!PtInRect((LPRECT)&clientRect, leftPt))
			{
			// no item selected so first simulate clicking mouse in menu and then
			// moving mouse out of menu
			SendMessage(hListMenu, WM_LBUTTONDOWN, NULL, MAKELPARAM(leftPt.x,
				clientRect.top + PM_BORDER));
			SendMessage(hListMenu, WM_MOUSEMOVE, NULL, MAKELPARAM(leftPt.x, leftPt.y));
			}
		else //add 1 to force it in the client area
			SendMessage(hListMenu, WM_LBUTTONDOWN, NULL, MAKELPARAM(leftPt.x, leftPt.y));

    //Subclass after we send the initiale mouse down otherwise the message
    //we'll be ignored if we're a right mouse popup.
		oldProc = (WNDPROC)SetWindowLong(hListMenu, GWL_WNDPROC, (DWORD)POPUPMENU::ListBoxWndProc);
		
		SetCapture(hListMenu);
		ShowWindow(hWndMenu, SW_SHOW);
		ShowWindow(hListMenu, SW_SHOW);

		okToScroll = TRUE;

		if(popupType & POPUP_EXTENDED)
    	{
		 	m_validSelect = POPUP_NOSELECTED;
			m_startSelect = -1;
			m_startX = leftPt.x;
			m_startTime = GetTickCount();			      
      }
		
		if(popupType & POPUP_MESSAGEHOG) //take control of message loop
			{
			while (GetMessage((LPMSG)&msg,NULL,0,0))
				{
				if(!TranslateMDISysAccel(PROGSYSTEM::hWndClient, (LPMSG)&msg))
					{
					TranslateMessage((LPMSG)&msg);
					//if the popup is being destroyed, break out of loop
					result = DispatchMessage((LPMSG)&msg);
					if(result == POPUP_DESTROY)
						break;
					else if(result == POPUP_OUTOFBOUNDS)
						{
						returnValue = FALSE;
						break;
						}
					}
				}

			if(hTimer)
				{
				I_VERIFY(KillTimer(hListMenu, PM_TIMER));
				hTimer = NULL;
				}

			ShowWindow(hWndMenu, SW_HIDE);
			}
		}
	else
		{
		MessageBeep(0);
		returnValue = FALSE;
		}

	return (returnValue && (selectedItem != POPUP_NOSELECTED));
	}

////////////////////////////////////////////////////////////////
int POPUPMENU::ShowPopupExt(
	POINT leftPt,          //must be in screen coordinates
	long selected,
	LPRECT selLocation)
	{
	selectionStatus = POPUP_EXT_NOSELECTION;
	I_ASSERT(popupType & POPUP_EXTENDED);
	if(popupType & POPUP_EXTENDED)
		{
	
		if(ShowPopup(leftPt, selected))
			*selLocation = selectedLocation;
		}

	return selectionStatus;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  DrawItem
//
// CLASS.......:  POPUPMENU
//
// DESCRIPTION.:  Draws item identified by index.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void POPUPMENU::DrawItem(
	HDC hdc,
	LPRECT bounds,
	int index)
	{
	char item[PM_MAX_ITEM_LENGTH];
	long result;
	int tab=PM_TABPIXELS;

	result = ListBox_GetText(hListMenu, index, item);
	Rectangle(hdc, bounds->left, bounds->top, bounds->right, bounds->bottom);
	
	if(result != LB_ERR)
		{
		if(popupType & POPUP_ICONS)
			{
			HBRUSH brush = CreateSolidBrush(GetBkColor(hdc));
			RECT rt;
			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP bmp = CreateCompatibleBitmap(hdc, 32, 32);
			
			SelectObject(hdcMem, bmp);

			rt.left = 0;
			rt.top = 0;
			rt.bottom = 32;
			rt.right = 32;

			FillRect(hdcMem, (LPRECT)&rt, brush);
			
			if(popupType & POPUP_OBJICONS)
				((LPOBJDESC)ListBox_GetItemData(hListMenu, index))->
					GetIcons()->Draw(hdcMem, 0, 0);
			else if(popupType & POPUP_FUNCICONS)
				((LPAFUNCDESC)ListBox_GetItemData(hListMenu, index))->
					GetIcons()->Draw(hdcMem, 0, 0);

			StretchBlt(hdc, bounds->left+2, bounds->top + 2, 16,16,
				hdcMem, 0, 0, 32, 32, SRCCOPY);

			DeleteObject(brush);
			DeleteObject(hdcMem);
			DeleteObject(bmp);
			}

		//DrawText(hdc, (LPSTR)item, lstrlen((LPSTR)item), bounds, PM_FORMAT);
		TabbedTextOut(hdc, bounds->left + m_textOffset.x, bounds->top + m_textOffset.y,
			(LPSTR)item, lstrlen((LPSTR)item),1, (LPINT)&tab, bounds->left);
	
		}
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  OuterWinProc
//
// CLASS.......:  POPUPMENU
//
// DESCRIPTION.:  Class window proc for the outer window
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

LRESULT POPUPMENU::OuterWinProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	LPMEASUREITEMSTRUCT ms;
	HDC hdc;
	LPDRAWITEMSTRUCT ds;
	int oldMode;
	COLORREF newColor, oldColor;
	HBRUSH newBrush, oldBrush;
	HPEN   newPen, oldPen;
	PAINTSTRUCT ps;
	POINT poly[3];
	RECT clientRect;

	switch(message)
		{
		case WM_PAINT:
			hdc = BeginPaint(hWnd, (LPPAINTSTRUCT)&ps);
			oldPen = SelectPen(hdc, GetStockPen(BLACK_PEN));
			oldBrush = SelectBrush(hdc, GetStockBrush(BLACK_BRUSH));
			GetClientRect(hWnd, (LPRECT)&clientRect);

			poly[0].x = clientRect.right - PM_BORDER;
			poly[1].x = poly[0].x - PM_ARROWWIDTH;
			poly[2].x = (poly[0].x + poly[1].x)/2;

			// draw directional arrows if necessary
			if(flags & PM_UPARROW)
				{
				poly[2].y = clientRect.top + PM_BORDER;
				poly[1].y = poly[2].y + PM_ARROWHEIGHT - PM_BORDER;
				poly[0].y = poly[1].y;
				Polygon(hdc, (LPPOINT)&poly, 3);
				}
			if(flags & PM_DOWNARROW)
				{
				poly[2].y = clientRect.bottom - PM_BORDER;
				poly[1].y = poly[2].y - PM_ARROWHEIGHT + PM_BORDER;
				poly[0].y = poly[1].y;
				Polygon(hdc, (LPPOINT)&poly, 3);
				}
			SelectPen(hdc, oldPen);
			SelectBrush(hdc, oldBrush);
			EndPaint(hWnd, (LPPAINTSTRUCT)&ps);
			return TRUE;

		case WM_MEASUREITEM:
			ms = (LPMEASUREITEMSTRUCT)lParam;
			ms->itemHeight = m_itemHeight;
			return FALSE;

		case WM_DRAWITEM:
			ds = (LPDRAWITEMSTRUCT)lParam;
			if(ds->itemAction & (ODA_DRAWENTIRE | ODA_SELECT))
				{
				if(ds->itemState & ODS_SELECTED)
					{
					newColor = RGB(255,255,255);
					newBrush = GetStockBrush(BLACK_BRUSH);
					newPen = GetStockPen(BLACK_PEN);
					}
				else
					{
					newColor = RGB(0,0,0);
					newBrush = GetStockBrush(WHITE_BRUSH);
					newPen = GetStockPen(WHITE_PEN);
					}
				oldMode = SetBkMode(ds->hDC, TRANSPARENT);
				oldPen = SelectPen(ds->hDC, newPen);
				oldBrush = SelectBrush(ds->hDC, newBrush);
				oldColor = SetTextColor(ds->hDC, newColor);
				DrawItem(ds->hDC, (LPRECT)&ds->rcItem, ds->itemID);
				SetBkMode(ds->hDC, oldMode);
				SelectPen(ds->hDC, oldPen);
				SelectBrush(ds->hDC, oldBrush);
				SetTextColor(ds->hDC, oldColor);
				}
			return TRUE;


		}
	return DefWindowProc(hWnd, message, wParam, lParam);
	}




/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ListProc
//
// CLASS.......:  POPUPMENU
//
// DESCRIPTION.:  class listbox window proc.  If mouse is moved horizontally
//									outside of menu bounds then the default item is selected
//									If the mouse is moved vertically outside of window bounds
//									then the listbox scrolls if it can else it does nothing.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS: if POPUP_OUTOFBOUNDS || POPUP_DESTROY is returned then
//							 the capture must be released.
/////////////////////////////////////////////////////////////////////////////
LRESULT POPUPMENU::ListProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	int selected;
	RECT clientRect;
	POINT mouse;
	int count;
	int topIndex;
	int boxCount;
	MSG msg;
	RECT rt;
	BOOL done=FALSE;
	int draw = 0;
	int erase = 0;
	BOOL post = FALSE;
	BOOL result;
	POINTS mouseS;

	#define PMUP 1
	#define PMDOWN 2
	
	#define PM_MIN_HORIZONTAL		50
	#define PM_MIN_SELECTTIME		100
	switch(message)
		{
		case WM_TIMER:
			if(wParam == PM_TIMER)
				HandleTimer();
			else
				I_ASSERT(FALSE);
			
			return 0;

		case WM_KEYUP:
			if((wParam == VK_ESCAPE) && (popupType & POPUP_EXTENDED))
				return SendMessage(hWnd, WM_RBUTTONUP, NULL, NULL);
			break;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			//break if the button message doesn't match the popup type
			if((message == WM_LBUTTONDOWN && (popupType & POPUP_RIGHTBUTTON)) ||
				(message == WM_RBUTTONDOWN && (popupType & POPUP_LEFTBUTTON)))
				return 0;	//we don't want any default processing
			
			break;

		// record selected item and post message to owner window that the menu
		// has been closed.
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			//break if the button message doesn't match the popup type
			if((message == WM_LBUTTONUP && (popupType & POPUP_RIGHTBUTTON)) ||
				(message == WM_RBUTTONUP && (popupType & POPUP_LEFTBUTTON)))
				{
				//see if we want' to cancel an extended menu
				if(popupType & POPUP_EXTENDED)
					{
					selectedItem = POPUP_NOSELECTED;
					goto popup_CLEANUP;
					}

				return 0;	//we don't want any default processing
				}

			ReleaseCapture();
			selected = (int)ListBox_GetCurSel(hWnd);
			if(selected != LB_ERR)
				{
				selectedItem = selected;
				selectedData = ListBox_GetItemData(hWnd, selected);

				if(popupType & POPUP_EXTENDED) 
					{								
					if(selectionStatus == POPUP_EXT_NOSELECTION)
						selectionStatus = POPUP_EXT_NORMALSELECTION;
					else
						{
						//make sure the drag selection was valid
						if(m_validSelect == POPUP_NOSELECTED)
							{
							selectedItem = POPUP_NOSELECTED;
							selectedData = NULL;
							}
						else
							{
							if(m_validSelect == LB_ERR)
								{
								selectedItem = POPUP_NOSELECTED;
								selectedData = NULL;
								}
							else
								{
								selectedItem = m_validSelect;
								selectedData = ListBox_GetItemData(hWnd, selectedItem);
								I_ASSERT(selectedData != LB_ERR);
								}
							}
						}

					if(selectedItem != LB_ERR && selectedItem != POPUP_NOSELECTED)
						{
						//get location of selection
						ListBox_GetItemRect(hWnd, selectedItem, &selectedLocation);
						ClientToScreen(hWnd, (LPPOINT)&selectedLocation.left);
						ClientToScreen(hWnd, (LPPOINT)&selectedLocation.right);
						}
					}
				}
			else	
				{
				selectedItem = POPUP_NOSELECTED;
				if(popupType & POPUP_EXTENDED)
					selectionStatus = POPUP_EXT_NOSELECTION;
				}

popup_CLEANUP:
			SetWindowLong(hWnd, GWL_WNDPROC, (DWORD)oldProc);
			//if we're a message hog then return that the popup is to be destroyed,
			//else post this message to the parent window
			if(popupType & POPUP_MESSAGEHOG)
				{
				result = POPUP_DESTROY;
				}
			else
				{
				PostMessage(hWndOwner, POPUP_DESTROY, NULL, NULL);
				result = 0;
				}

			//we're no longer calling into the subclassed list proc so this
			//is the best place to remove the hWnd from the list.
			POPUPMENU::listboxList->Remove(hWnd);

			return result;

		// determine if scrolling action is necessary.
		case WM_MOUSEMOVE:
			GetClientRect(hWnd, (LPRECT)&clientRect);
			//shrink rect to make scrolling easier
			clientRect.top++;
			clientRect.bottom--;
			mouseS = MAKEPOINTS(lParam);
      mouse.x = mouseS.x;
      mouse.y = mouseS.y;
      
			if(!PtInRect((LPRECT)&clientRect, mouse))
				{
				topIndex = (int)ListBox_GetTopIndex(hWnd);
				count = (int)ListBox_GetCount(hWnd);
				boxCount = (clientRect.bottom - clientRect.top + 1) /
					m_itemHeight;
				if(mouse.x >= clientRect.left && mouse.x<= clientRect.right)
					{
          //we're out of the bounds vertically so possibly scroll vertically
					
					if(!done)
						{
						if(count <= boxCount)
							{
							//we can't scroll so see if we're vertically beyond the
							//tolerance
							if(mouse.y < clientRect.top - PM_VERTTOLERANCE ||
								mouse.y > clientRect.bottom + PM_VERTTOLERANCE)
								{
								done=FALSE; //so the selected item will be cleared
								if(popupType & POPUP_NOOUTOFBOUNDS)
									{
									ReleaseCapture();
									return POPUP_OUTOFBOUNDS;
									}
								}
							else
								done=TRUE; //so the selected item won't be cleared
							}
						else
							{
		          //we're out of the bounds vertically so possibly scroll vertically
							if(!okToScroll)
								{
								if(PeekMessage((LPMSG)&msg, hWndMenu, WM_TIMER, WM_TIMER,
									PM_REMOVE))
									{
									HandleTimer();
									}
								}

							if(okToScroll)
								{
								if(mouse.y <= clientRect.top)
									{
									if(topIndex > 0)
										{
										topIndex--;
										ListBox_SetTopIndex(hWnd, topIndex);
										ListBox_SetCurSel(hWnd, topIndex);
										UpdateWindow(hWnd);
										if(!PeekMessage((LPMSG)&msg, hWnd, WM_MOUSEFIRST,
											WM_MOUSELAST, PM_NOREMOVE))
											{
											post = TRUE;
											}
										}
									}
								else if(topIndex + boxCount < count)
									{
									ListBox_SetTopIndex(hWnd, topIndex+1);
									ListBox_SetCurSel(hWnd, topIndex + boxCount);
									UpdateWindow(hWnd);
									if(!PeekMessage((LPMSG)&msg, hWnd, WM_MOUSEFIRST,
										WM_MOUSELAST, PM_NOREMOVE))
										{
										post = TRUE;
										}
									}

								if(!(popupType &  POPUP_ICONS))
									{
									//Set scrolling timer
									if(hTimer == NULL)
										{
										if((hTimer = SetTimer(hWnd, PM_TIMER, PM_TIMERINTERVAL, NULL)) != NULL)
											okToScroll = FALSE;
										}
									}
								}
							else
								{
								if((!PeekMessage((LPMSG)&msg, hWnd, WM_MOUSEFIRST,
									WM_MOUSELAST, PM_NOREMOVE)))
									{
									post = TRUE;
									}
								}
							done=TRUE;
							}
						}
					}
				else
					{
					//we're horizontally outofbounds
					if(popupType & POPUP_NOOUTOFBOUNDS)
						{
						ReleaseCapture();
						return POPUP_OUTOFBOUNDS;
						}
					else if(popupType & POPUP_EXTENDED)
						{
						selectionStatus = POPUP_EXT_DRAGSELECTION;
						return SendMessage(hWnd, WM_LBUTTONUP, wParam, lParam);
						}
					}

        if(!done)
					{
					ListBox_SetCurSel(hWnd, -1);
					m_validSelect = POPUP_NOSELECTED;
					}

				if(topIndex > 0)
					{
					if(!(flags & PM_UPARROW))
						{
						flags|=PM_UPARROW;
						draw|=PMUP;
						}
					}
				else if(flags & PM_UPARROW)
					{
					flags&= ~PM_UPARROW;
					draw|=PMUP;
					erase|=PMUP;
					}
				if(topIndex+boxCount < count)
					{
					if(!(flags & PM_DOWNARROW))
						{
						flags|=PM_DOWNARROW;
						draw|=PMDOWN;
						}
					}
				else if(flags & PM_DOWNARROW)
					{
					flags&= ~PM_DOWNARROW;
					draw|=PMDOWN;
					erase|=PMDOWN;
					}

				if(draw)
					{
					GetClientRect(hWndMenu, (LPRECT)&clientRect);
					rt.left = clientRect.left;
					rt.right = clientRect.right;
					if(draw & PMUP)
						{
						rt.top = clientRect.top;
						rt.bottom = rt.top + PM_ARROWHEIGHT;
						InvalidateRect(hWndMenu, (LPRECT)&rt, (erase&PMUP)?TRUE:FALSE );
						}
					if(draw & PMDOWN)
						{
						rt.bottom = clientRect.bottom;
						rt.top = clientRect.bottom - PM_ARROWHEIGHT;
						InvalidateRect(hWndMenu, (LPRECT)&rt, (erase&PMDOWN)?TRUE:FALSE);
						}
					UpdateWindow(hWndMenu);
					}

				if(post)
					{
					PostMessage(hWnd, message, wParam, lParam);
					}

				return FALSE;
				}

			//we're in the client bounds
			if(popupType & POPUP_EXTENDED)
				{
				int curSel = (int)ListBox_GetCurSel(hWnd);

				//make sure its OK to change selection				
				if(curSel != m_startSelect)
					{
					m_startTime = GetTickCount();
					m_startSelect = curSel;
					m_startX = (short)LOWORD(lParam);
					}
				else
					{
					if(GetTickCount() - m_startTime > PM_MIN_SELECTTIME)
						m_validSelect = curSel;

					if(abs(m_startX - (short)LOWORD(lParam)) > PM_MIN_HORIZONTAL)
						{
						if(curSel != LB_ERR)
							{
							selectionStatus |= POPUP_EXT_DRAGSELECTION;
							m_validSelect = curSel;
							return SendMessage(hWnd, WM_LBUTTONUP, wParam, lParam);
							}
						}
					}
				}
			break;

		case WM_DESTROY:
			if(hTimer)
				I_VERIFY(KillTimer(hWnd, PM_TIMER));
			break;
		}
	return CallWindowProc((FARPROC)oldProc, hWnd, message, wParam, lParam);
	}



/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  WndProc
//
// CLASS.......:  POPUPMENU
//
// DESCRIPTION.:  Window's window procedure for the outer window which
//									directs messages to the appropiate class window method
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK POPUPMENU::WndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	POPUPMENU *				 		menuPtr;
	LPCREATESTRUCT 						cs;

	if (POPUPMENU::menuList)
		{
		menuPtr = (POPUPMENU *)(POPUPMENU::menuList->GetObject(hWnd));
		if (!menuPtr)
			{
			if (message == WM_CREATE)
				{
				cs = (LPCREATESTRUCT)lParam;
				POPUPMENU::menuList->Add(hWnd, menuPtr =
					((POPUPMENU *)cs->lpCreateParams));
				return menuPtr->OuterWinProc(hWnd, message, wParam, lParam);
				}
			else
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		else
			{
			if(message == WM_DESTROY)
				POPUPMENU::menuList->Remove(hWnd);
			return menuPtr->OuterWinProc(hWnd, message, wParam, lParam);
			}
		}
	else
		return DefWindowProc(hWnd, message, wParam, lParam);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ListBoxWndProc
//
// CLASS.......:  POPUPMENU
//
// DESCRIPTION.:  Window's subclassed listbox procedure which directs listbox
//									messages to the appropriate class listbox method.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK POPUPMENU::ListBoxWndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	POPUPMENU *				 		menuPtr;

	if (POPUPMENU::listboxList)
		{
		menuPtr = (POPUPMENU *)(POPUPMENU::listboxList->GetObject(hWnd));
		if (menuPtr)
			{
			if(message != PM_MESSES_UP_SELECTION)
				return menuPtr->ListProc(hWnd, message, wParam, lParam);
			else
				return FALSE;
			}
		}
	return DefWindowProc(hWnd, message, wParam, lParam);
	}

///////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  HandleTimer
//
// CLASS.......:  POPUPMENU
//
// DESCRIPTION.:  Kills the timer and sets the okToScroll flag which enables
//									scrolling
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void POPUPMENU::HandleTimer()
	{
	okToScroll = TRUE;
	if(hTimer)
		{
		I_VERIFY(KillTimer(hListMenu, PM_TIMER));
		hTimer = NULL;
		}
	}

////////////////////////////////////////////////////////////
POPUPMENU& POPUPMENU::operator= (const POPUPMENU& rhs)
	{
	int maxCount = (int)ListBox_GetCount(rhs.hListMenu);
	char buffer[256];
	long data;
	int i;

	if(maxCount != LB_ERR)
		{
		for(i=0;i<maxCount;i++)
			{
			data = ListBox_GetItemData(rhs.hListMenu, i);
			if(data != LB_ERR)
				{
				if((WORD)ListBox_GetTextLen(rhs.hListMenu, i) < sizeof(buffer))
					{
					if(ListBox_GetText(rhs.hListMenu, i, buffer) != LB_ERR)
						{
						AppendItem(buffer, data, NO_ORDINAL);
						}
					}
				}
			}
		}

	return *this;
	}

