#include "almsys.h"
#include <windowsx.h>

#include "navcls.h"
#include "resconst.h"
#include "winlist.h"
#include "subject.h"
#include "project.h"
#include "uidtable.h"
#include "seruid.h"
#include "comment.h"
#include "progsys.h"

#define  navigatorClass	"MDINavigator"

#define IDC_LISTBOX		1

#define NAV_CLICKONNAME		1
#define NAV_CLICKONARROW	2

#define NAV_DISPLAYED						0X0001
#define NAV_EXPANDED						0X0002
#define NAV_WAITINGFORDELETION	0x0004	 //signals that the item has been
																					//removed from the list box but
																					//has not been picked from the tree.

#define SUBJECTTAB "  "		// 2 SPACES
#define COMMENTTAB "    " // 4 SPACES
#define SUBJECT_TAB_SPACES 2
#define COMMENT_TAB_SPACES 4

/* NOTE: when setting aside memory to store the subject and comment names
	use SUBJECT_NAME_LENGTH + 1 since SUBJECT_NAME_LENGTH is set equal to 32
	and 1 must be added to make room for the null terminator */


LRESULT CALLBACK NAVIGATORWndProc(
	HWND hWnd,
	UINT	message,
	WPARAM wParam,
	LPARAM lParam
	);

LRESULT CALLBACK ListBoxWndProc(
	HWND 			hWnd,
	UINT    	message,
	WPARAM 		wParam,
	LPARAM 		lParam);


/////////////////////////////////////////////////////////////////////////////
// Allocate and initialize static class members
/////////////////////////////////////////////////////////////////////////////

HINSTANCE	NAVIGATOR::hAppInstance = NULL;

WINDOWLIST*		 NAVIGATOR::windowList = NULL;

WNDPROC NAVIGATOR::originalLBWindowProc = NULL;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ITEM
//
// CLASS.......: ITEM
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
ITEM::ITEM(
	LPCSTR	    				namePtr,
	LPFILEOBJECT        filePtr,
	SERUID							itemUID,
	short int						type):
    name(namePtr)
	{

	status = 0;
	next = NULL;
	first = NULL;

	this->type = type;
	itemContext = filePtr;
	this->itemUID = itemUID;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: FindItem
//
// CLASS.......: ITEM
//
// DESCRIPTION.: Called recursively to traverse the tree of navigator items
//               and return a pointer to the item whose itemUID and
//               itemContext match those specified.
//
/////////////////////////////////////////////////////////////////////////////
LPITEM ITEM::FindItem(
  SERUID        itemUID,
  LPFILEOBJECT  itemContext,
  short &       level)
  {
  LPITEM  currentItem;
  LPITEM  foundItem = 0;

  level += (short)1;

  currentItem = this->first;

  while (currentItem)
    {
    if ((itemUID == currentItem->itemUID) &&
        (itemContext == currentItem->itemContext))
      {
      foundItem = currentItem;
      break;
      }
    else if (0 ==
             (foundItem = currentItem->FindItem(itemUID, itemContext, level)))
      currentItem = currentItem->next;
    else
      break;
    }

  if (foundItem == 0)
    level -= (short)1;

  return foundItem;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: NAVIGATOR
//
// CLASS.......: NAVIGATOR
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

NAVIGATOR::NAVIGATOR(
	HWND				hWndParent,
	HINSTANCE 	hAppInstance,
	HINSTANCE 	hPrevInstance,
	int     left,
	int			top,
	int			width,
	int			height
	){
	BOOL		status;
	RECT		navigatorRect;
	char  	buffer[20];
	HMENU 	hMenu;
	SERUID	dummy;

	NAVIGATOR::hAppInstance = hAppInstance;

	headItem = new ITEM("", NULL, dummy, 0);
	headItem->status = NAV_EXPANDED;

	selectedIndex = LB_ERR;

	if (!windowList)
		windowList = new WINDOWLIST();

	if (!hPrevInstance)
		status = registerWindowClass();
	else
		status = TRUE;

	if (status)
		{
		this->hWndParent = hWndParent;

		LoadString(hAppInstance, STR_NAVIGATOR, (LPSTR)buffer, 20);

		hWndNavigator = CreateWindow((LPSTR)navigatorClass,
									(LPSTR)buffer,
									WS_OVERLAPPED|WS_SYSMENU|WS_CAPTION|WS_THICKFRAME,
									left, top, width, height,
									hWndParent,
									NULL,
									hAppInstance,
									(LPSTR)this);

		if (hWndNavigator)
			{
			hMenu = GetSystemMenu(hWndNavigator, NULL);
			DeleteMenu(hMenu, SC_TASKLIST, MF_BYCOMMAND);
			DeleteMenu(hMenu, GetMenuItemCount(hMenu)-1, MF_BYPOSITION);

			GetClientRect(hWndNavigator, (LPRECT)(&navigatorRect));

			hWndListBox = CreateWindow(	(LPSTR)"LISTBOX",
									NULL,
									WS_CHILD|LBS_USETABSTOPS|LBS_NOTIFY|WS_VSCROLL|
									LBS_OWNERDRAWFIXED,
									navigatorRect.left,
									navigatorRect.top,
									navigatorRect.right-navigatorRect.left,
									navigatorRect.bottom-navigatorRect.top,
									hWndNavigator,
									(HMENU) IDC_LISTBOX,
									hAppInstance,
									NULL);

			if (hWndListBox)
				{
				NAVIGATOR::windowList->Add(hWndListBox, this);

				NAVIGATOR::originalLBWindowProc =
					(WNDPROC)SetWindowLong(hWndListBox, GWL_WNDPROC, (long)ListBoxWndProc);
				}
			}
		}
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ~NAVIGATOR (destructor)
//
// CLASS.......: NAVIGATOR
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

NAVIGATOR::~NAVIGATOR(
	void)
	{
	LPITEM	projectItem;
	LPITEM	subjectItem;
	LPITEM	commentItem;
	LPITEM	tempItem;

	projectItem = headItem->first;

	while (projectItem)
		{
		subjectItem = projectItem->first;
		while (subjectItem)
			{
			commentItem = subjectItem->first;
			while (commentItem)
				{
				tempItem = commentItem;
				commentItem = commentItem->next;
				delete tempItem;
				}
			subjectItem->first = NULL;
			tempItem = subjectItem;
			subjectItem = subjectItem->next;
			delete tempItem;
			}
		tempItem = projectItem;
		projectItem = projectItem->next;
		delete tempItem;
		}

	delete headItem;

	if(IsWindow(hWndNavigator))
		DestroyWindow(hWndNavigator);

	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: registerWindowClass
//
// CLASS.......: NAVIGATOR
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
BOOL NAVIGATOR::registerWindowClass(
	void
	){
	WNDCLASS  	wc;
	BOOL				status;

	wc.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
	wc.lpfnWndProc = NAVIGATORWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(NAVIGATOR*);
	wc.hInstance = hAppInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_APPWORKSPACE+1);
	wc.lpszMenuName =  NULL;
	wc.lpszClassName = navigatorClass;

	status = RegisterClass((LPWNDCLASS)(&wc));

	return(status);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ListBoxWindowProc
//
// CLASS.......: NAVIGATOR
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

LRESULT NAVIGATOR::ListBoxWindowProc(
	HWND 				hWnd,
	UINT		 		message,
	WPARAM			wParam,
	LPARAM			lParam)
	{

	switch (message)
		{
		case WM_LBUTTONDBLCLK:
			if (LOWORD(lParam) <= 16)
				clickLocation = NAV_CLICKONARROW;
			else
				clickLocation = NAV_CLICKONNAME;
			break;

		case WM_KEYDOWN:
			if ((wParam == VK_RETURN) && (selectedIndex != LB_ERR))
				ActivateWindow();
			else if(wParam == VK_DELETE)
				{
				SendMessage(PROGSYSTEM::hWndFrame, WM_COMMAND,
											GET_WM_COMMAND_MPS(IDM_DODELETE, NULL, 0));
				return 0;
				}
			break;

		default:
			break;
		}
	return(CallWindowProc((FARPROC)NAVIGATOR::originalLBWindowProc, hWnd, message,
												wParam, lParam));

	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: WindowProc
//
// CLASS.......: NAVIGATOR
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

LRESULT NAVIGATOR::WindowProc(
	HWND 				hWnd,
	UINT		 		message,
	WPARAM			wParam,
	LPARAM			lParam
	){
	BOOL 				msgHandled = TRUE;

	switch (message)
		{
		case WM_SYSCOMMAND:
			switch(wParam & 0xFFF0)
				{
				case SC_CLOSE:
					hide();
					break;
				default:
					msgHandled = FALSE;
				}
			break;

		case WM_SIZE:
			resizeWindow(LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_ACTIVATE: 
			if (LOWORD(wParam) != WA_INACTIVE)					//window is getting the focus
				SetFocus(hWndListBox);
			else
				msgHandled = FALSE;
			break;

		case WM_COMMAND: 
			if ((GET_WM_COMMAND_HWND(wParam, lParam)!=0) && GET_WM_COMMAND_ID(wParam, lParam)==IDC_LISTBOX)
				{
				switch (GET_WM_COMMAND_CMD(wParam, lParam))	//switch on notification code
					{
					case LBN_DBLCLK:
						if (clickLocation == NAV_CLICKONARROW)
							ExpandOrContract();
						else if (clickLocation == NAV_CLICKONNAME)
							ActivateWindow();
						break;
					case LBN_SELCHANGE:
						selectedIndex = (short)ListBox_GetCurSel(hWndListBox);
						break;
					default:
						msgHandled = FALSE;
						break;
					}
				}
			else
				msgHandled = FALSE;
			break;

		case WM_MEASUREITEM:
			((MEASUREITEMSTRUCT *)lParam)->itemHeight = 16;
			break;

		case WM_DRAWITEM:
			drawItem((DRAWITEMSTRUCT *)lParam);
			break;

		case WM_DELETEITEM:
			break;

		case WM_KEYDOWN:
			if(wParam == VK_DELETE)
				{
				SendMessage(PROGSYSTEM::hWndFrame, WM_COMMAND,
											GET_WM_COMMAND_MPS(IDM_DODELETE, NULL, 0));
				return 0;
				}
			break;

    case WM_NCACTIVATE:
      msgHandled = FALSE;
      wParam = 0;
      if(PROGSYSTEM::TestActiveAppFlag())
        wParam = 1;
      break;

		default:
			msgHandled = FALSE;
		}

	if (msgHandled)
		return (NULL);
	else
		return DefWindowProc(hWnd, message, wParam, lParam);

	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Expand
//
// CLASS.......: NAVIGATOR
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
void NAVIGATOR::Expand(
	LPITEM 	list,
	short				index)
	{

	SendMessage(hWndListBox, WM_SETREDRAW, FALSE, 0L);

	while (list)
		{
		list->status = list->status | NAV_DISPLAYED;
		if (!list->next)
			SendMessage(hWndListBox, WM_SETREDRAW, TRUE, 0L);
	
	  ListBox_InsertString(hWndListBox, index, (LPSTR)list);
		index++;
		list = list->next;
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Contract
//
// CLASS.......: NAVIGATOR
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
void NAVIGATOR::Contract(
	LPITEM 	list,
	short				index)
	{
	static short	recursionCount;

	SendMessage(hWndListBox, WM_SETREDRAW, FALSE, 0L);

	while (list)
		{
		if ((list->status & NAV_EXPANDED) && list->first)
			{
			recursionCount++;
			Contract(list->first, (short)(index+1));
			recursionCount--;
			}
		if ((recursionCount == 0) && (!list->next))
			SendMessage(hWndListBox, WM_SETREDRAW, TRUE, 0L);
		list->status = list->status & ~(NAV_EXPANDED|NAV_DISPLAYED);
	
	  ListBox_DeleteString(hWndListBox, index);
		list = list->next;
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ExpandOrContract
//
// CLASS.......: NAVIGATOR
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
void NAVIGATOR::ExpandOrContract(
	void)
	{
	short 			index;
	LPITEM	itemPtr;
	LPITEM		child;

  // Get current selection ... no selection --> no work to do
	index = (short)ListBox_GetCurSel(hWndListBox);
  if (index == LB_ERR)
    return;

  // Get item-data for current selection ... no item-data --> no work to do
  itemPtr = (LPITEM)ListBox_GetItemData(hWndListBox, index);
	if (!itemPtr || (itemPtr == (LPITEM)LB_ERR))
    return;

  // Get the child for this selection ... no child --> no work to do
  child = itemPtr->first;
	if (!child)
    return;

  // Contract or expand accordingly
	if (itemPtr->status & NAV_EXPANDED)
		{
		itemPtr->status = itemPtr->status & ~NAV_EXPANDED;
		Contract(child, (short)(index+1));
		}
	else
		{
		index++;
		itemPtr->status = itemPtr->status | NAV_EXPANDED;
		Expand(child, index);
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ActivateWindow
//
// CLASS.......: NAVIGATOR
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
void NAVIGATOR::ActivateWindow(
	void)
	{
	LPITEM	item;
	void * object;

  // Ensure there is a window selected to activate
	if (selectedIndex == LB_ERR)
    return;

  // Ensure we can get the item-data for that item
	item = (LPITEM)ListBox_GetItemData(hWndListBox,  selectedIndex);
	if (!item || (item == (LPITEM)LB_ERR))
    return;

	//bail out if we're in a run state and the context of the item is 
	//different than the running project.
	if(PROGSYSTEM::TestRunTimeFlag() && 
		(item->itemContext->GetFileIDStamp() != 
		PROGSYSTEM::GetRunContext()->GetFileIDStamp()))
		{
		MessageBeep(0);
		return;
		}

	PROGSYSTEM::PushAndRegisterContext(item->itemContext);

  // Show the selected item
	object = GetObject(item->itemUID);
	switch (item->type)
		{
		case ITEM_PROJECT:
			((PROJECT *)object)->ShowItem();
			break;

		case ITEM_SUBJECT:
			((SUBJECT *)object)->ShowItem();
			break;

		case ITEM_COMMENT:
			((COMMENT *)object)->ShowItem();
			break;

		default:
			break;
		}

	PROGSYSTEM::PopAndRegisterContext();
	return;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: resizeWindow
//
// CLASS.......: NAVIGATOR
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
void NAVIGATOR::resizeWindow(
	short	width,
	short	height)
	{
	RECT        listBoxRect;
	RECT        navWindowRect;
	short				newHeight;

	MoveWindow(hWndListBox, 0, 0, width, height, TRUE);

	GetWindowRect(hWndListBox, (LPRECT)(&listBoxRect));
	GetWindowRect(hWndNavigator, (LPRECT)(&navWindowRect));

	newHeight = (short)(navWindowRect.bottom-navWindowRect.top-
	 (navWindowRect.bottom-listBoxRect.bottom)+GetSystemMetrics(SM_CYFRAME));

	if (newHeight != (navWindowRect.bottom-navWindowRect.top))
		MoveWindow(hWndNavigator, navWindowRect.left, navWindowRect.top,
		 navWindowRect.right-navWindowRect.left, newHeight, TRUE);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: drawItem
//
// CLASS.......: NAVIGATOR
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

void NAVIGATOR::drawItem(
	DRAWITEMSTRUCT*	drawItemInfo)
	{
	POINT				trianglePts[3];
	HBRUSH 			hBrush;
	HPEN				hPen;
	HPEN				hOldPen;
	HBRUSH			hOldBrush;
	int					currentROP;
	LPITEM	item;
	BOOL				drawTriangle = FALSE;

	if ((short)drawItemInfo->itemID == -1)
		{
		DrawFocusRect(drawItemInfo->hDC,(RECT  *)&drawItemInfo->rcItem);
		return;
		}

	item = (LPITEM)drawItemInfo->itemData;

	if (item->first)
		{
		drawTriangle = TRUE;
		if (item->status & NAV_EXPANDED)
			{
			trianglePts[0].x = drawItemInfo->rcItem.left+4;
			trianglePts[0].y = drawItemInfo->rcItem.top+4;

			trianglePts[1].x = drawItemInfo->rcItem.left+12;
			trianglePts[1].y = drawItemInfo->rcItem.top+4;

			trianglePts[2].x = drawItemInfo->rcItem.left+8;
			trianglePts[2].y = drawItemInfo->rcItem.top+12;
			}
		else
			{
			trianglePts[0].x = drawItemInfo->rcItem.left+4;
			trianglePts[0].y = drawItemInfo->rcItem.top+4;

			trianglePts[1].x = drawItemInfo->rcItem.left+4;
			trianglePts[1].y = drawItemInfo->rcItem.top+12;

			trianglePts[2].x = drawItemInfo->rcItem.left+12;
			trianglePts[2].y = drawItemInfo->rcItem.top+8;
			}
		}

	SetBkMode(drawItemInfo->hDC, TRANSPARENT);

	switch (drawItemInfo->itemAction)
		{
		case ODA_SELECT:

			hBrush = GetStockBrush(WHITE_BRUSH);
			hPen = GetStockPen(WHITE_PEN);
			if (hBrush && hPen)
				{
				hOldBrush = SelectBrush(drawItemInfo->hDC, hBrush);
				hOldPen = SelectPen(drawItemInfo->hDC, hPen);
				Rectangle(drawItemInfo->hDC, drawItemInfo->rcItem.left,
				 drawItemInfo->rcItem.top, drawItemInfo->rcItem.right,
				 drawItemInfo->rcItem.bottom);
				SelectBrush(drawItemInfo->hDC, hOldBrush);
				SelectPen(drawItemInfo->hDC, hOldPen);
				DeleteBrush(hBrush);
				DeletePen(hPen);
				}

			if (drawTriangle)
        {
				hBrush = GetStockBrush(BLACK_BRUSH);
        if (hBrush)
          {
				  hOldBrush = SelectBrush(drawItemInfo->hDC, hBrush);
				  Polygon(drawItemInfo->hDC, trianglePts, 3);
				  SelectBrush(drawItemInfo->hDC, hOldBrush);
				  DeleteBrush(hBrush);
				  }
        }

			TextOut(drawItemInfo->hDC,
        drawItemInfo->rcItem.left+16, drawItemInfo->rcItem.top,
        item->name, lstrlen(item->name));

			if (drawItemInfo->itemState & ODS_SELECTED)
        {
				hBrush = GetStockBrush(BLACK_BRUSH);
        if (hBrush)
				  {
				  currentROP = GetROP2(drawItemInfo->hDC);
				  SetROP2(drawItemInfo->hDC, R2_NOT);

				  Rectangle(drawItemInfo->hDC, drawItemInfo->rcItem.left,
            drawItemInfo->rcItem.top, drawItemInfo->rcItem.right,
            drawItemInfo->rcItem.bottom);

				  SetROP2(drawItemInfo->hDC, currentROP);
				  }
        }

			break;

		case ODA_FOCUS:
			DrawFocusRect(drawItemInfo->hDC,(RECT  *)&drawItemInfo->rcItem);
			break;

		case ODA_DRAWENTIRE:

			hBrush = GetStockBrush(WHITE_BRUSH);
			hPen = GetStockPen(WHITE_PEN);

			if (hBrush && hPen)
				{
				hOldBrush = SelectBrush(drawItemInfo->hDC, hBrush);
				hOldPen = SelectPen(drawItemInfo->hDC, hPen);

				Rectangle(drawItemInfo->hDC, drawItemInfo->rcItem.left,
          drawItemInfo->rcItem.top, drawItemInfo->rcItem.right,
          drawItemInfo->rcItem.bottom);

				SelectBrush(drawItemInfo->hDC, hOldBrush);
				SelectPen(drawItemInfo->hDC, hOldPen);

				DeleteBrush(hBrush);
				DeletePen(hPen);
				}

			if (drawTriangle)
        {
        hBrush = GetStockBrush(BLACK_BRUSH);
        if (hBrush)
          {
				  hOldBrush = SelectBrush(drawItemInfo->hDC, hBrush);
				  Polygon(drawItemInfo->hDC, trianglePts, 3);
				  SelectBrush(drawItemInfo->hDC, hOldBrush);
				  DeleteBrush(hBrush);
				  }
        }

			TextOut(drawItemInfo->hDC,
				drawItemInfo->rcItem.left+16, drawItemInfo->rcItem.top,
        item->name, lstrlen(item->name));

			if (drawItemInfo->itemState & ODS_SELECTED)
        {
				hBrush = GetStockBrush(BLACK_BRUSH);
        if (hBrush)
				  {
				  currentROP = GetROP2(drawItemInfo->hDC);
				  SetROP2(drawItemInfo->hDC, R2_NOT);

				  Rectangle(drawItemInfo->hDC, drawItemInfo->rcItem.left,
				    drawItemInfo->rcItem.top, drawItemInfo->rcItem.right,
				    drawItemInfo->rcItem.bottom);

				  SetROP2(drawItemInfo->hDC, currentROP);
				  }
        }
			break;

		default:
			break;
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: show
//
// CLASS.......: NAVIGATOR
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
void NAVIGATOR::show(
	void
	){
	RECT rect;

	if(IsWindow(hWndNavigator))
		{
		//we need to invalidate the screen rectangle under the navigator to force
		//the frame to redraw since for some reason  in windows 3.0 the frame
		//does not redraw once the navigator has been hidden and then shown for
		//the second time.
		GetWindowRect(hWndNavigator, (LPRECT)&rect);
		ScreenToClient(GetDesktopWindow(), (LPPOINT)&rect.left);
		ScreenToClient(GetDesktopWindow(), (LPPOINT)&rect.right);
		ShowWindow(hWndNavigator, SW_SHOW);
		InvalidateRect(GetDesktopWindow()	,(LPRECT)&rect,FALSE);
		ShowWindow(hWndListBox, SW_SHOW);
		UpdateWindow(hWndNavigator);
		UpdateWindow(hWndListBox);
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: hide
//
// CLASS.......: NAVIGATOR
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
void NAVIGATOR::hide(
	void
	)
	{
	if(IsWindow(hWndNavigator))
		ShowWindow(hWndNavigator, SW_HIDE);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: GetIndex
//
// CLASS.......: NAVIGATOR
//
// DESCRIPTION.: This function generates the index that a given item should
//							 have in the list box.  It is used to determine where a new
//							 item should be displayed.
//
// PARAMETERS..: item - a pointer to the item whose structure is to be
//							 calculated.
//
// RETURN VALUE: The index of the item in the list box.  Returns -1 if the
//							 item's parent is not displayed or the item is not found in
//							 the list.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

short NAVIGATOR::GetIndex(
	LPITEM 	item)
	{
	LPITEM projectItem;
	LPITEM subjectItem;
	LPITEM commentItem;
	short			 index = -1;

	projectItem = this->headItem->first;

	//count item as being in the listbox only if it hasn't already been deleted.
	while (projectItem)
		{
		if(!(projectItem->status & NAV_WAITINGFORDELETION))
			index++;
		
		if (projectItem == item)
			return (index);
		else if (projectItem->status & NAV_EXPANDED)
			{
			subjectItem = projectItem->first;
			while (subjectItem)
				{
				if(!(subjectItem->status & NAV_WAITINGFORDELETION))
					index++;
				
				if (subjectItem == item)
					return (index);
				else if (subjectItem->status & NAV_EXPANDED)
					{
					commentItem = subjectItem->first;
					while (commentItem)
						{
						if(!(commentItem->status & NAV_WAITINGFORDELETION))
							index++;
						if (commentItem == item)
							return (index);
						commentItem = commentItem->next;
						}
					}
				subjectItem = subjectItem->next;
				}
			}
		projectItem = projectItem->next;
		}

	return (-1);

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: findItem
//
// CLASS.......: NAVIGATOR
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

LPITEM NAVIGATOR::findItem(
	LPITEM	parentItem,
	LPFILEOBJECT fileObjPtr,
	SERUID itemUID)
	{
	LPITEM	currentItem;

	currentItem = parentItem->first;

	while (currentItem && (fileObjPtr != currentItem->itemContext ||
		itemUID != currentItem->itemUID))
			currentItem = currentItem->next;

	return (currentItem);

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: insertItem
//
// CLASS.......: NAVIGATOR
//
// DESCRIPTION.: This function creates a new item record and inserts it in
//							 the child list of the indicated parent.  The list is sorted
//							 alphabetically.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

void NAVIGATOR::insertItem(
	LPITEM						parentItem,
	const char  *			name,
	LPFILEOBJECT			fileObjPtr,
	SERUID						itemUID,
	short int					type)
	{
	LPITEM	currentItem;
	LPITEM	newItem;
	LPITEM	prevItem;
	short				index;

	if (!(parentItem->first))
		{
		parentItem->first = new ITEM(name, fileObjPtr, itemUID, type);
		if (parentItem->status & NAV_EXPANDED)
			{
			parentItem->first->status |= NAV_DISPLAYED;
			ListBox_InsertString(hWndListBox, GetIndex(parentItem->first), 
				(LPSTR)parentItem->first);
			}
		else if (parentItem->status & NAV_DISPLAYED)	//redraw to show arrow
			{
			index = GetIndex(parentItem);
			SendMessage(hWndListBox, WM_SETREDRAW, FALSE, 0L);
			ListBox_DeleteString(hWndListBox, index);
			SendMessage(hWndListBox, WM_SETREDRAW, TRUE, 0L);
			ListBox_InsertString(hWndListBox, index, (LPSTR)parentItem);
			}
		}
	else
		{
		prevItem = NULL;
		currentItem = parentItem->first;
		while (currentItem && (lstrcmp(name, currentItem->name) >= 0))
			{
			prevItem = currentItem;
			currentItem = currentItem->next;
			}
		newItem = new ITEM(name, fileObjPtr, itemUID, type);
		newItem->next = currentItem;
		if (prevItem)
			prevItem->next = newItem;
		else
			parentItem->first = newItem;
		if (parentItem->status & NAV_EXPANDED)
			{
			newItem->status |= NAV_DISPLAYED;
			ListBox_InsertString(hWndListBox, GetIndex(newItem),
				(LPSTR)newItem);
			}
		}

	return;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: addItem
//
// CLASS.......: NAVIGATOR
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

BOOL NAVIGATOR::AddItem(
	SERUID							projectUID,
	SERUID		 					subjectUID,
	SERUID							commentUID,
	LPFILEOBJECT				fileObjPtr,
	LPCSTR							itemName)
	{
	LPITEM		parentItem;
	LPSTR buffer;

	if (projectUID == NULL)
		return FALSE;

	if (subjectUID == NULL)		//must be a project
		{
		insertItem(headItem, itemName, fileObjPtr, projectUID, ITEM_PROJECT);
		return TRUE;
		}

	if (commentUID == NULL)		//must be a subject
		{
		//find the project which the subject belongs to
		parentItem = findItem(headItem, fileObjPtr, projectUID);
		if (!parentItem)
			return FALSE;

		buffer = (LPSTR)GlobalAlloc(GPTR, lstrlen(itemName) + 1 + SUBJECT_TAB_SPACES);

		lstrcpy(buffer, SUBJECTTAB);

		I_ASSERT(buffer);

		lstrcat(buffer, itemName);
		insertItem(parentItem, buffer, fileObjPtr, subjectUID, ITEM_SUBJECT);
		
		GlobalFree((HGLOBAL)buffer);
		return TRUE;
		}

	//must be a comment
	//find the project which contains the comment
	parentItem = findItem(headItem, fileObjPtr, projectUID);
	//find the subject which the comment belongs to
	parentItem = findItem(parentItem, fileObjPtr, subjectUID);
	if(!parentItem) //if we can't find the subject return false
		return FALSE;

	buffer = (LPSTR)GlobalAlloc(GPTR, lstrlen(itemName) + 1 + COMMENT_TAB_SPACES);

	I_ASSERT(buffer);

	lstrcpy(buffer, COMMENTTAB);

	lstrcat(buffer, itemName);
	insertItem(parentItem, buffer, fileObjPtr, commentUID, ITEM_COMMENT);

	GlobalFree((HGLOBAL)buffer);
	return TRUE;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: RemoveSubItems
//
// CLASS.......: NAVIGATOR
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
void NAVIGATOR::RemoveSubItems(
	LPITEM	subItemList)
	{
	LPITEM	currentItem;
	LPITEM	tempItem;

	currentItem = subItemList;

	while (currentItem)
		{
		if (currentItem->first)
			RemoveSubItems(currentItem->first);
		if (currentItem->status & NAV_DISPLAYED)
			{
			ListBox_DeleteString(hWndListBox, GetIndex(currentItem));
			currentItem->status |= NAV_WAITINGFORDELETION; 
			}
		tempItem = currentItem;
		currentItem = currentItem->next;
		delete tempItem;
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: RemoveItem
//
// CLASS.......: NAVIGATOR
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

BOOL NAVIGATOR::RemoveItem(
	LPITEM	parentItem,
	LPFILEOBJECT fileObjPtr,
	SERUID itemUID)
	{
	LPITEM	currentItem;
	LPITEM	prevItem = NULL;
	short				index;

	if(!parentItem->first)
		return FALSE;

	currentItem = parentItem->first;

	while (currentItem && (fileObjPtr != currentItem->itemContext ||
		itemUID != currentItem->itemUID))
		{
		prevItem = currentItem;
		currentItem = currentItem->next;
		}

	if (currentItem)
		{
		if (currentItem->status & NAV_DISPLAYED)
			{
			ListBox_DeleteString(hWndListBox, GetIndex(currentItem));
			currentItem->status |= NAV_WAITINGFORDELETION;
			}
		if (currentItem->first)
			RemoveSubItems(currentItem->first);
		if (!prevItem)						//is first item in list
			parentItem->first = currentItem->next;
		else
			prevItem->next = currentItem->next;
		delete currentItem;
		if ((parentItem->status & NAV_DISPLAYED) && (!parentItem->first))
			{
			index = GetIndex(parentItem);
			SendMessage(hWndListBox, WM_SETREDRAW, FALSE, 0L);
			ListBox_DeleteString(hWndListBox, index);
			SendMessage(hWndListBox, WM_SETREDRAW, TRUE, 0L);
			ListBox_InsertString(hWndListBox, index, (LPSTR)parentItem);
			}
		return TRUE;
		}

	return FALSE;

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DeleteItem
//
// CLASS.......: NAVIGATOR
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

BOOL NAVIGATOR::DeleteItem(
	LPFILEOBJECT					fileObjPtr,
	SERUID								projectUID,
	SERUID								subjectUID,
	SERUID								commentUID)
	{
	LPITEM parentItem;

	if(projectUID == NULLSERUID || !headItem->first)
		return FALSE;

	if(subjectUID == NULLSERUID)
		//must be a project
		return RemoveItem(headItem, fileObjPtr, projectUID);

	//Get owning project
	parentItem = findItem(headItem, fileObjPtr, projectUID);
	if(!parentItem || !parentItem->first)
		return FALSE;

	if(commentUID == NULLSERUID)
		//must be a subject
		return RemoveItem(parentItem, fileObjPtr, subjectUID);

	//must be a comment
	//get owning subject
	parentItem = findItem(parentItem, fileObjPtr, subjectUID);
	if(!parentItem || !parentItem->first)
		return FALSE;

	return RemoveItem(parentItem, fileObjPtr, commentUID);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SetItemName
//
// CLASS.......: NAVIGATOR
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
BOOL NAVIGATOR::SetItemName(
	SERUID       itemUID,
	LPFILEOBJECT itemContext,
	LPCSTR			 newName)
	{
	LPITEM  		foundItem;
	short       index;
	BOOL        result = FALSE;
	short       level = 0;
	LPSTR       buffer;

	foundItem = headItem->FindItem(itemUID, itemContext, level);

	if (foundItem)
		{
		buffer = (LPSTR)GlobalAlloc(GPTR, lstrlen(newName) + 1 + COMMENT_TAB_SPACES);

		I_ASSERT(buffer);

		if (level == 2)                   //must be a subject
			lstrcpy(buffer, SUBJECTTAB);
		else if (level == 3)              //must be a comment
			lstrcpy(buffer, COMMENTTAB);
		else
			buffer[0] = '\0';

		lstrcat(buffer, newName);
		foundItem->name = (LPSTR)buffer;
		if (foundItem->status & NAV_DISPLAYED)
			{
			index = GetIndex(foundItem);
			SendMessage(hWndListBox, WM_SETREDRAW, FALSE, 0L);
			ListBox_DeleteString(hWndListBox, index);
			SendMessage(hWndListBox, WM_SETREDRAW, TRUE, 0L);
			ListBox_InsertString(hWndListBox, index, (LPSTR)foundItem);
			}

		GlobalFree((HGLOBAL)buffer);
		result = TRUE;
		}

	return result;

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: NAVIGATORWndProc
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.: Assumes the static windowList pointer in NAVIGATOR class is
//								initialized.
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK NAVIGATORWndProc(
	HWND 			hWnd,
	UINT			message,
	WPARAM 		wParam,
	LPARAM 		lParam)
	{
	NAVIGATOR  * navigator;
	LPCREATESTRUCT  lpcs;

  navigator = (NAVIGATOR  *)(NAVIGATOR::windowList->GetObject(hWnd));
	if (navigator)
		return navigator->WindowProc(hWnd, message, wParam, lParam);
	else
		switch (message)
			{
			case WM_CREATE:
				{
				lpcs = (LPCREATESTRUCT) lParam;
				navigator = (NAVIGATOR *)(lpcs->lpCreateParams);
				NAVIGATOR::windowList->Add(hWnd, navigator);
				return (navigator->WindowProc(hWnd, message, wParam, lParam));
				}
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
      }

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ListBoxWndProc
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.: Assumes the static windowList pointer in NAVIGATOR class is
//								initialized.
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK ListBoxWndProc(
	HWND 			hWnd,
	UINT			message,
	WPARAM 		wParam,
	LPARAM 		lParam)
	{
	NAVIGATOR  * navigator;

  navigator = (NAVIGATOR  *)(NAVIGATOR::windowList->GetObject(hWnd));
	if (navigator)
		return(navigator->ListBoxWindowProc(hWnd, message, wParam, lParam));
	else
		return(CallWindowProc((FARPROC)NAVIGATOR::originalLBWindowProc, hWnd, message,
													wParam, lParam));

	}
