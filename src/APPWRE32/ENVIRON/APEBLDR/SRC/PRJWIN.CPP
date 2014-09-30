#include <math.h>
#include "almsys.h"
#include <windowsx.h>

#include "resconst.h"
#include "prjwin.h"
#include "memlib.h"
#include "project.h"
#include "subject.h"
#include "mdiclass.h"
#include "serblk.h"
#include "progsys.h"
#include "system.h"
#include "fileobj.h"
#include "navcls.h"
#include "intutl.h"

#define S_EDITFIELD 600

char PROJECTWINDOW::className[] = "ProjectClass";
BOOL PROJECTWINDOW::registered = FALSE;
int	PROJECTWINDOW::minProjectHeight = 0;
int	PROJECTWINDOW::minProjectWidth = 0;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	PROJECTWINDOW
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  constructor: responsible for creating the MDI window,
//								 						 making menu changes to reflect an open
//														 project window, and make the neccessary
//														 initialization.
//
// PARAMETERS..:  PROJECT * creatorObject:  pointer to the PROJECT which
//																						 created this PROJECTWINDOW.
//								HANDLE hInstance: application instance.
//								HANDLE hPrevInstance:  appliation previous instance.
//								HANDLE hFrameWindow:   handle the MDI frame window.
//								HANDLE hClientWindow:  handle toe MDE client window.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
PROJECTWINDOW::PROJECTWINDOW(
	SERUID theProjectUID):BASEWINDOW(theProjectUID)
{
	RECT clientRect;
	MDICREATESTRUCT mdicreate;
	PROJECT * projPtr;

	projPtr = GetProjectPtr();
	if(!registered)
	{
		Register();
	}
	SetProjectFlag();
	projectSubjectWidth	= GetSystemMetrics(SM_CXSCREEN)/5;
	minProjectHeight =  100;
	minProjectWidth = projectSubjectWidth + 100;

	vScrollShowing = 0;
	hScrollShowing = 0;

	projPtr->GetWindowBounds(&clientRect);
	if(clientRect.bottom - clientRect.top < PROJECTWINDOW::minProjectHeight)
		clientRect.bottom = clientRect.top + PROJECTWINDOW::minProjectHeight;

	if(clientRect.right-clientRect.left < PROJECTWINDOW::minProjectWidth)
		clientRect.right = clientRect.left + PROJECTWINDOW::minProjectWidth;

	//set the window bounds in case they changed.
	projPtr->SetWindowBounds(&clientRect);

	mdicreate.szClass = className;
	mdicreate.szTitle = GetProjectPtr()->GetName();
	mdicreate.hOwner = PROGSYSTEM::hInst;
	mdicreate.x = clientRect.left;
	mdicreate.y = clientRect.top;
	mdicreate.cx = clientRect.right-clientRect.left;
	mdicreate.cy = clientRect.bottom-clientRect.top;
	mdicreate.style = WS_OVERLAPPEDWINDOW|WS_VISIBLE;
	mdicreate.lParam =(long)this;
	editField = NULL;
	if(projPtr->TestIconicFlag())
		mdicreate.style|=WS_MINIMIZE;
	else if(projPtr->TestMaximizeFlag())
		{
		mdicreate.style|=WS_MAXIMIZE;
		mdicreate.x = CW_USEDEFAULT;
		mdicreate.y = CW_USEDEFAULT;
		mdicreate.cx = CW_USEDEFAULT;
		mdicreate.cy = CW_USEDEFAULT;
		}

	SendMessage(PROGSYSTEM::hWndClient, WM_MDICREATE, 0, (LONG)&mdicreate);
	//If window is Iconic, need to reset the title since due to some
	//windows bug, the title doesn't appear under the icon initially.
	if(projPtr->TestIconicFlag())
		{
		SetWindowText(GetWindowHandle(), projPtr->GetName());
		UpdateWindow(GetWindowHandle());
		}

	draggingSubject =0;
	draggingSelectRect = 0;

}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	~PROJECTWINDOW
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  destructor
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
PROJECTWINDOW::~PROJECTWINDOW()
	{
	GetProjectPtr()->SetWindowPtr(NULL);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Register
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  registers window class
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::Register()
{
	WNDCLASS wc;

	wc.style = CS_VREDRAW | CS_HREDRAW|CS_DBLCLKS;;
	wc.lpfnWndProc = BASEWINDOW::BaseWndProc;
	wc.hInstance = PROGSYSTEM::hInst;
	wc.hIcon = WORKSHEET::GetProjectIcon();
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = (LPSTR)className;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	RegisterClass((LPWNDCLASS)&wc);
	registered = TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	WndProc
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  class window procedure
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
LRESULT PROJECTWINDOW::WndProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	RECT refreshRect;
	RECT clientRect;
	RECT tempRect;
	POINT mouse;
	LPPOINT		pt;
	HBRUSH		tempBrush;
	HMENU hMenu;
	char buffer[64];
	int messageAnswer;
	PROJECT * projPtr=GetProjectPtr();

	switch(message)
		{
		case WM_CREATE:
			SetWindowHandle(hwnd);
			CreateScrollBars();
 			return(0);

		case WM_GETMINMAXINFO:
			pt = LPPOINT(lParam);
			(pt+3)->x = minProjectWidth;
			(pt+3)->y = minProjectHeight;
			break;

		case WM_PAINT:
			PaintWindow();
			return(0);

		case WM_ERASEBKGND:
			GetUpdateRect(GetWindowHandle(),&refreshRect,FALSE);
			GetClientRect(hwnd, &clientRect);
			//We don't need to erase where the scroll bars are
			clientRect.right-=GetSystemMetrics(SM_CXVSCROLL);
			clientRect.bottom-=GetSystemMetrics(SM_CYHSCROLL);
			if(IntersectRect(&tempRect,&clientRect,
				&refreshRect))
				{
				//Increment the right and bottom edges since FillRect doesn't cover
				//these edges
				tempRect.right++;
				tempRect.bottom++;
				tempBrush = CreateSolidBrush(projPtr->GetWindowColor());
				FillRect((HDC) wParam, &tempRect, tempBrush);
				DeleteObject(tempBrush);
				}
			return(1);

		case WM_COMMAND:
			{
			WORD		wID			= GET_WM_COMMAND_ID(wParam, lParam);
			WORD		wNotify	= GET_WM_COMMAND_CMD(wParam, lParam);

			switch(wID)
				{
				case IDM_ARRANGESUBJECTS:
					projPtr->ArrangeSubjects();
//					projPtr->SnapLogBounds(HRANGE|VRANGE);
//					DetermineScrollBars();
//					SetScrollBars(HRANGE|HPOS|VRANGE|VPOS);
					return(0);

				case IDM_PROJECTGRID:
					hMenu = GetMenu(PROGSYSTEM::hWndFrame);
					if(projPtr->GridIsOn())
						{
						projPtr->SetGrid(FALSE);
						CheckMenuItem(hMenu, IDM_PROJECTGRID,
							MF_UNCHECKED|MF_BYCOMMAND);
						}
					else
						{
						projPtr->SetGrid(TRUE);
						CheckMenuItem(hMenu, IDM_PROJECTGRID,
							MF_CHECKED|MF_BYCOMMAND);
						}
					return 0;

				case S_EDITFIELD:
					if(wNotify==EN_UPDATE)
						ReshapeEditField();
					break;

				}
			break;
			}

		case WM_LBUTTONDBLCLK:
			FigureSelection(lParam, wParam, 1);
			return(0);

		case WM_LBUTTONDOWN:

			projPtr->GetClientBounds(&clientRect);
			A_MAKEPOINT(mouse, lParam);
			if(PtInRect(&clientRect, mouse))
				FigureSelection(lParam, wParam, 0);
			else
				{
				GetWindowRect(vScroll, &clientRect);
				ScreenToClient(hwnd, (LPPOINT)&clientRect.left);
				ScreenToClient(hwnd, (LPPOINT)&clientRect.right);
				if(PtInRect(&clientRect, mouse))
					MessageBeep(1);
				else
					{
					GetWindowRect(hScroll, &clientRect);
					ScreenToClient(hwnd, (LPPOINT)&clientRect.left);
					ScreenToClient(hwnd, (LPPOINT)&clientRect.right);
					if(PtInRect(&clientRect, mouse))
						MessageBeep(1);
					}
				}
			if(PROGSYSTEM::TestUndoFlag() && (PROGSYSTEM::TestPasteOpFlag() ||
				PROGSYSTEM::TestUndoDeleteOpFlag()) &&
				fileObjPtr->GetFileIDStamp() == PROGSYSTEM::GetUndoPrjID() &&
				PROGSYSTEM::GetUndoSjtUID() == NULLSERUID)
				PROGSYSTEM::ClearUndoFlag();

			return(0);

		case WM_LBUTTONUP:
			if(EndDrag(lParam))
				return 0;
			else
				break;

		case WM_VSCROLL:
			{
			WORD	nCode		= GET_WM_VSCROLL_CODE(wParam, lParam);
			HWND	hScroll	= GET_WM_VSCROLL_HWND(wParam, lParam);

			if(hScroll == vScroll)
				{
				if(editField != NULL)
					CloseEditField();
				switch(nCode)
					{
					case SB_LINEDOWN:
						ScrollProjectWindow(GODOWN);
						return(0);
					case SB_LINEUP:
						ScrollProjectWindow(GOUP);
						return(0);

					case SB_THUMBTRACK:
					case SB_THUMBPOSITION:
					case SB_BOTTOM:
					case SB_TOP:
					case SB_PAGEUP:
					case SB_PAGEDOWN:
						ArbitraryVScroll(wParam, lParam);
					return(0);
					}
				}
				break;
			}

		case WM_HSCROLL:
			{
			WORD	nCode		= GET_WM_VSCROLL_CODE(wParam, lParam);
			HWND	hScroll	= GET_WM_VSCROLL_HWND(wParam, lParam);

			if(hScroll == hScroll)
				{
				if(editField != NULL)
					CloseEditField();
				switch(nCode)
					{
					case SB_LINEDOWN:
						ScrollProjectWindow(GORIGHT);
						return(0);
					case SB_LINEUP:
						ScrollProjectWindow(GOLEFT);
						return(0);

					case SB_THUMBTRACK:
					case SB_THUMBPOSITION:
					case SB_BOTTOM:
					case SB_TOP:
					case SB_PAGEUP:
					case SB_PAGEDOWN:
						ArbitraryHScroll(wParam, lParam);
					return(0);
					}
				}
				break;
			}

		case WM_MOUSEMOVE:
			if (DragSubject(lParam))
				return(0);
			else
				break;

		case WM_MDIACTIVATE:
			{
			BOOL			fActivate	= GET_WM_MDIACTIVATE_FACTIVATE(hwnd, wParam, lParam);

			if(fActivate)
				{
				PROGSYSTEM::SetContext(projPtr);
				PROGSYSTEM::SetActiveProjectFlag();
				}
			else
				{
				PROGSYSTEM::ClearActiveProjectFlag();
				if(editField != NULL)
					CloseEditField();
				}
			break;
			}

		case WM_MOVE:
			if(!projPtr->TestIconicFlag() && !IsIconic(hwnd))
				{
				GetWindowRect(hwnd, &clientRect);
				ScreenToClient(PROGSYSTEM::hWndFrame, (LPPOINT)&clientRect);
				ScreenToClient(PROGSYSTEM::hWndFrame, (LPPOINT)&clientRect.right);
				projPtr->SetWindowBounds(&clientRect);
				}
			break;

		case WM_SIZE:
			switch(wParam)
				{
				case SIZEFULLSCREEN:
					projPtr->SetMaximizeFlag();
				case SIZENORMAL:
					if(wParam == SIZENORMAL)
						projPtr->ClearMaximizeFlag();
					projPtr->ClearIconicFlag();
					ResizeScrollBars();
					GrowOrShrinkLogBounds();
					projPtr->MakeDirty();
					break;

				case SIZEICONIC:
					projPtr->ClearMaximizeFlag();
					projPtr->SetIconicFlag();
					projPtr->MakeDirty();
					break;
				}
			break;

		// if the file is dirty, query the user if he would like to save.
		// if the user responds to the dialog box by hittin cancel, 0 is returned
		case WM_CLOSE:
		case WM_QUERYENDSESSION:
			if(PROGSYSTEM::TestRunTimeFlag() && 
				PROGSYSTEM::GetRunTimeFileID() ==
				GetProjectPtr()->fileObjPtr->GetFileIDStamp())
				{
				LoadString(PROGSYSTEM::hInst, STR_FRAMENAME, buffer, sizeof(buffer));

				MessageBeep(0);
			
				MessageBox(NULL, "Can not close project since this project\n"
					"is currently running.", buffer,
					MB_ICONINFORMATION|MB_OK|MB_TASKMODAL);

				return FALSE;
				}
			if(projPtr->ProjectIsDirty())
				{
				if(!projPtr->TestIconicFlag())
					BringWindowToTop(hwnd);
				if(editField != NULL)
					CloseEditField();
				wsprintf((LPSTR)buffer,
					(LPSTR)"Save changes to %s ?",
					(LPSTR)projPtr->GetName());
				MessageBeep(1);
				messageAnswer = MessageBox(PROGSYSTEM::hWndFrame,buffer,
					"WARNING",
					MB_YESNOCANCEL | MB_ICONEXCLAMATION |MB_TASKMODAL);
				if(messageAnswer == IDCANCEL)
					return FALSE;
				else if(messageAnswer == IDYES)
					if(!projPtr->Save())
						return FALSE;
				}
			//window is responsible for restoring itself from an iconisized state
			//before being destroyed.
			SendMessage(PROGSYSTEM::hWndClient, WM_MDIRESTORE, (WPARAM) hwnd, NULL);
			break;


		}
	return DefMDIChildProc(hwnd, message, wParam, lParam);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  PaintWindow
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  paints the neccessary window features
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::PaintWindow()
	{
	HDC hdc;
	RECT clientRect;
	RECT winRect;
	char buffer[16];
	PAINTSTRUCT ps;
	HWND hwnd;
	PROJECT * projPtr = GetProjectPtr();
	HBRUSH newBrush;

	hwnd = GetWindowHandle();
	hdc = BeginPaint(hwnd, (LPPAINTSTRUCT)&ps);
	GetClientRect(hwnd, &clientRect);
	SelectObject(hdc, GetStockObject(BLACK_PEN));
	SelectObject(hdc, GetStockObject(WHITE_BRUSH));
	SelectObject(hdc, WORKSHEET::numSubFont);
	winRect.left = clientRect.left-1; // -1 so scroll bar edge and box edge
																		// overlap.
	winRect.top = clientRect.bottom - GetSystemMetrics(SM_CYHSCROLL) + 1;
	winRect.right = clientRect.left + projectSubjectWidth;
	winRect.bottom = clientRect.bottom + 1;	// +1 so scroll bar edge and box
																					// edge overlap.
	Rectangle(hdc,
		winRect.left,
		winRect.top,
		winRect.right,
		winRect.bottom);
	if(projPtr->GetSubjectCount() == 1)
		wsprintf((LPSTR)buffer, (LPSTR)"%u Subject \0",
			projPtr->GetSubjectCount());
	else
		wsprintf((LPSTR)buffer, (LPSTR)"%u Subjects \0",
			projPtr->GetSubjectCount());
	SetBkMode(hdc, TRANSPARENT);
	DrawText(hdc, (LPSTR)buffer, lstrlen((LPSTR)buffer),
		&winRect, DT_VCENTER | DT_RIGHT | DT_NOPREFIX |
			DT_SINGLELINE);

	//Draw the lower righthand corner if necessary
	clientRect.left = clientRect.right - GetSystemMetrics(SM_CXVSCROLL);
	clientRect.top = clientRect.bottom - GetSystemMetrics(SM_CYHSCROLL);
	if(IntersectRect(&winRect, &clientRect,
		&ps.rcPaint))
		{
		//draw the corner
		newBrush = CreateSolidBrush(GetSysColor(COLOR_SCROLLBAR));
		FillRect(hdc, &winRect, newBrush);
		DeleteObject(newBrush);
		}

	//All window drawing should be done before drawing the subjects since
	//drawing the subjects will alter the clipping region for the
	//device context.
	DrawAllSubjects(hdc, &(ps.rcPaint));

	EndPaint(hwnd, (LPPAINTSTRUCT)&ps);
	if(!vScrollShowing)
		GreyScrollBar(vScroll);
	if(!hScrollShowing)
		GreyScrollBar(hScroll);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  GrowOrShrinkLogBounds
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  resizes the logBounds if neccessary when the window size
//									changes
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::GrowOrShrinkLogBounds()
	{
	RECT clientRect;
	RECT winRect;
	POINT logBounds;
	POINT originOffset;
	BOOL originChanged = FALSE;
	BOOL logChanged = FALSE;
	PROJECT * projPtr = GetProjectPtr();

	originOffset = projPtr->GetOriginOffset();
	logBounds = projPtr->GetLogBounds();
	GetWindowRect(GetWindowHandle(), &winRect);
	projPtr->GetClientBounds(&clientRect);
	if(logBounds.x - ( clientRect.right - clientRect.left) -
		originOffset.x <0)
		{
		logChanged = TRUE;
		logBounds.x = clientRect.right - clientRect.left + originOffset.x;
		if(logBounds.x > XMAXPAGE)
			{
			originChanged = TRUE;
			logBounds.x = XMAXPAGE;
			originOffset.x = logBounds.x - (clientRect.right - clientRect.left);
			}
		}
	if(logBounds.y - ( clientRect.bottom - clientRect.top) -
		originOffset.y <0)
		{
		logChanged = TRUE;
		logBounds.y = clientRect.bottom - clientRect.top + originOffset.y;
		if(logBounds.y > YMAXPAGE)
			{
			originChanged = TRUE;
			logBounds.y = YMAXPAGE;
			originOffset.y = logBounds.y - (clientRect.bottom - clientRect.top);
			}
		}
	if(logChanged)
		projPtr->SetLogBounds(logBounds);
	if(originChanged)
		projPtr->SetOriginOffset(originOffset);
	if(!IsIconic(GetWindowHandle()))
		projPtr->SetWindowBounds(&winRect);
	projPtr->SnapLogBounds(HRANGE|VRANGE);
	DetermineScrollBars();
	SetScrollBars(HRANGE|HPOS|VRANGE|VPOS);
	}


void PROJECTWINDOW::SetAllScrollBars()
	{
	DetermineScrollBars();
	SetScrollBars(HRANGE|HPOS|VRANGE|VPOS);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CreateScrollBars
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  creates window scroll bars
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::CreateScrollBars()
{
	RECT clientRect;

	GetClientRect(GetWindowHandle(), &clientRect);
	vScroll = CreateWindow("scrollbar",
							NULL,
							(long)(WS_CHILD| SBS_VERT),
							clientRect.right - GetSystemMetrics(SM_CXVSCROLL)+1,
							clientRect.top-1,
							GetSystemMetrics(SM_CXVSCROLL),
							clientRect.bottom - clientRect.top -
								GetSystemMetrics(SM_CYHSCROLL)+3,
							GetWindowHandle(),
							(HMENU) ID_VSCROLLBAR,
							PROGSYSTEM::hInst,
							NULL);

	hScroll = CreateWindow("scrollbar",
							NULL,
							(long)(WS_CHILD | SBS_HORZ),
							clientRect.left + projectSubjectWidth -1,
							clientRect.bottom - GetSystemMetrics(SM_CYHSCROLL)+1,
							clientRect.right - clientRect.left -
								projectSubjectWidth - GetSystemMetrics(SM_CXVSCROLL)+3,
							GetSystemMetrics(SM_CYHSCROLL),
							GetWindowHandle(),
							(HMENU) ID_HSCROLLBAR,
							PROGSYSTEM::hInst,
							NULL);
}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ResizeScrollBars
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  resizes scroll bars according to window size
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::ResizeScrollBars()
{
	RECT clientRect;

	GetClientRect(GetWindowHandle(), &clientRect);
	MoveWindow(	vScroll,
		clientRect.right -
		GetSystemMetrics(SM_CXVSCROLL)+1,
		clientRect.top-1,
		GetSystemMetrics(SM_CXVSCROLL),
		clientRect.bottom - clientRect.top - GetSystemMetrics(SM_CYHSCROLL)+3,
		TRUE);
	UpdateWindow(vScroll);

	MoveWindow(	hScroll,
		clientRect.left + projectSubjectWidth-1,
		clientRect.bottom - GetSystemMetrics(SM_CYHSCROLL)+1,
		clientRect.right - clientRect.left - projectSubjectWidth -
		GetSystemMetrics(SM_CXVSCROLL)+3,
		GetSystemMetrics(SM_CYHSCROLL),
		TRUE);
	UpdateWindow(hScroll);

}



/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ScrollProjectWindow
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  scrolls the window either horizontally or vertically or
//								both. The scroll distance is XSCROLLSTEP for horizontal or
//								YSCROLLSTEP for vertical.
//
// PARAMETERS..:  int direction: contains the mask of which directions to
//								scroll.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::ScrollProjectWindow(
	int direction)
	{
	RECT	winBounds;
	HDC		hdc;
	unsigned int snapResult = 0;
	POINT originOffset;
	POINT logBounds;
	PROJECT * projPtr = GetProjectPtr();

	logBounds = projPtr->GetLogBounds();
	originOffset = projPtr->GetOriginOffset();
	projPtr->GetClientBounds(&winBounds);
	hdc = GetDC(GetWindowHandle());
	if(direction & GOLEFT)
		{
		originOffset.x-=XSCROLLSTEP;
		if(originOffset.x < 0)
			originOffset.x = 0;
		}
	if(direction & GORIGHT)
		{
		originOffset.x+=XSCROLLSTEP;
		if(originOffset.x + winBounds.right > logBounds.x)
			originOffset.x= logBounds.x - winBounds.right;
		}
	if(direction & GOUP)
		{
		originOffset.y-=YSCROLLSTEP;
		if(originOffset.y < 0)
			originOffset.y = 0;
		}
	if(direction & GODOWN)
		{
		originOffset.y+=YSCROLLSTEP;
		if(originOffset.y + winBounds.bottom > logBounds.y)
			originOffset.y = logBounds.y - winBounds.bottom;
		}
	projPtr->SetOriginOffset(originOffset);
	if(!originOffset.x && (direction & GOLEFT))
		{
		snapResult |= projPtr->SnapLogBounds(HRANGE);
		}

	 if(!originOffset.y && (direction & GOUP))
		{
		snapResult |= projPtr->SnapLogBounds(VRANGE);
		}
	 if(snapResult)
		{
		SetScrollBars(snapResult);
		}

	if(direction & (GOLEFT | GORIGHT))
			SetScrollBars(HPOS);

	if(direction & (GOUP | GODOWN))
		SetScrollBars(VPOS);
	if(snapResult)
		DetermineScrollBars();

	DrawAllSubjects(hdc,&winBounds);
	ReleaseDC(GetWindowHandle(), hdc);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	DrawAllSubjects
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  draws every subject which is visible in the rectangle
//								pointed to by refreshRect.
//
// PARAMETERS..:  HDC hdc: device context.
//								LPRECT refreshRect: points to the rectangle which needs to
//								be redrawn.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::DrawAllSubjects(
	HDC hdc,
	LPRECT refreshRect)
	{
	HBRUSH	     tempBrush;
	HDC			     hdcMem;
	HBITMAP	     refreshBitmap;
	HDC			     holdDC;
	BOOL	       bitmapCreated = 0;
	POINT		     drawOffset;
	RECT		     clientRect;
	RECT	       drawRect;
	RECT		     windowClientRect;
	HBITMAP	     oldBmp;
	SUBJECT * temp;
	int		i=0;
	RUNTIMEUID   subjectList;
	PROJECT * projPtr = GetProjectPtr();

	projPtr->GetClientBounds(&windowClientRect);

	IntersectRect(&clientRect, &windowClientRect,
		refreshRect);

	hdcMem = CreateCompatibleDC(hdc);
	refreshBitmap = CreateCompatibleBitmap(hdc,
		clientRect.right - clientRect.left + 1,
		clientRect.bottom - clientRect.top + 1);

	if(refreshBitmap)
		{
		holdDC = hdcMem;
		oldBmp = SelectBitmap(holdDC, refreshBitmap);
		bitmapCreated = 1;
		drawOffset.x = clientRect.left;
		drawOffset.y = clientRect.top;
		}
	else
		{
		holdDC = hdc;
		drawOffset.x = 0;
		drawOffset.y = 0;
		}
	tempBrush = CreateSolidBrush(projPtr->GetWindowColor());
	SetClippingRegion(hdc);

	//increase the bottom and right sides by 1since FillRect doesn't cover
	//these sides.
	drawRect.top = clientRect.top-drawOffset.y;
	drawRect.bottom = clientRect.bottom - drawOffset.y + 1;
	drawRect.left = clientRect.left - drawOffset.x;
	drawRect.right = clientRect.right - drawOffset.x + 1;
	FillRect(holdDC, &drawRect, tempBrush);

	subjectList = projPtr->GetSubjectBlock();

	while((temp=(SUBJECT *)GetObject(subjectList,i))!=NULL)
		{
		if(!temp->IsSelected())
			{
			temp->DrawASubject(holdDC, &clientRect, drawOffset);
			}
		}
		i=0;
	while((temp=(SUBJECT *)GetObject(subjectList, i))!=NULL)
		{
		if(temp->IsSelected())
			{
			temp->DrawASubject(holdDC, &clientRect, drawOffset);
			}
		}

	if(bitmapCreated)
		{
		BitBlt(hdc,clientRect.left,clientRect.top,
			clientRect.right - clientRect.left,
			clientRect.bottom - clientRect.top,
			holdDC,
			0,0,
			SRCCOPY);
		SelectBitmap(holdDC, oldBmp);
		DeleteBitmap(refreshBitmap);
		}
	DeleteBitmap(tempBrush);
	DeleteDC(hdcMem);
	}



/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	FigureSelection
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  determines if the user selected a subject or clicked
//								in the window outside of the subjects.
//
// PARAMETERS..:  long lParam:  coordinates of where click took place
//								WORD lParam:  info about which keys were held down, if any
//															when click took place.
//								BOOL doubleClick:  zero if the function call was made
//																			as the result of a doubleclick.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::FigureSelection(
	LPARAM lParam,
	WPARAM wParam,
	BOOL doubleClick)
	{
	POINT         mouse;
	RECT	        textRect;
	SUBJECT *	newSelection = NULL;
	HDC 	        hdc;
	SUBJECT *  temp;
	int		        i = 0;
	RUNTIMEUID    subjectList;
	POINT         originOffset;
	HWND          hwnd;
	DWORD         length;
	char          buffer[SUBJECT_NAME_LENGTH];
	PROJECT *  projPtr = GetProjectPtr();

	if(PROGSYSTEM::TestUndoFlag() && 
		(PROGSYSTEM::GetUndoPrjID() == GetProjectPtr()->fileObjPtr->GetFileIDStamp()) 
		&& (PROGSYSTEM::GetUndoSjtUID() == NULLSERUID))
		PROGSYSTEM::ClearUndoFlag();
	hwnd = GetWindowHandle();
	subjectList = projPtr->GetSubjectBlock();
	autoScroll = 0;
	hdc = GetDC(hwnd);
	A_MAKEPOINT(mouse, lParam);
	SetCapture(hwnd);

	//first go through selected subjects

	while((temp=(SUBJECT *)GetObject(subjectList, i))!=NULL)
		{
		if(temp->IsSelected() && temp->SubjectIntersectPoint(hdc,&mouse))
			{
			newSelection = temp;
			break;
			}
		}

	if(!newSelection)
		{
		i=0;
		while((temp=(SUBJECT *)GetObject(subjectList, i))!=NULL)
			{
			if(!temp->IsSelected() && temp->SubjectIntersectPoint(hdc,&mouse))
				{
				newSelection = temp;
				break;
				}
			}
		}

	if(editField!=NULL)
		CloseEditField();
	if(newSelection)
		{
		if(newSelection->PointInTextRect(hdc, mouse) && editField == NULL)
			{
			SIZE		textSize;

			newSelection->GetTextRect(hdc, &textRect);
			SelectObject(hdc, WORKSHEET::subFont);
			GetTextExtentPoint32(hdc, newSelection->GetName(), lstrlen(
				newSelection->GetName()), &textSize);
			length = textSize.cx;
			editField = CreateWindow("EDIT",NULL,
				ES_MULTILINE|ES_AUTOHSCROLL|ES_LEFT|WS_CHILD,
				(int)(textRect.left + textRect.right - (int)LOWORD(length))/(int)2,
				textRect.top,
				LOWORD(length),
				textRect.bottom - textRect.top,
				GetWindowHandle(),
				(HMENU) S_EDITFIELD,
				PROGSYSTEM::hInst,
				NULL);
			SendMessage(editField, WM_SETFONT, (WPARAM) WORKSHEET::subFont,FALSE);
			SetWindowText(editField, newSelection->GetName());
			SendMessage(editField, EM_LIMITTEXT,
				SUBJECT_NAME_LENGTH, 0L);
			ClientToScreen(GetWindowHandle(), (LPPOINT)&mouse);
			ScreenToClient(editField, (LPPOINT)&mouse);

			ShowWindow(editField, SW_SHOW);

			//Send Message so cursor will appear in the edit field where the
			//user clicked on the name
			SendMessage(editField, WM_LBUTTONDOWN, NULL, MAKELPARAM(mouse.x,
				mouse.y));

		 //	CreateCaret(editField, NULL, 0, textRect.bottom - textRect.top);
		 //	ShowCaret(editField);
			oldEditProc = (WNDPROC) GetWindowLong(editField, GWL_WNDPROC);
			SetWindowLong(editField, GWL_WNDPROC, (long)PROJECTWINDOW::EditFieldProc);
			holdUID = GetUID(newSelection);
			oldName=(LPSTR)newSelection->GetName();
			buffer[0]='\0';
			newSelection->SetName((LPSTR)buffer);
			PROGSYSTEM::SetActiveEditField(editField);
			}
		else
			{
			if(doubleClick)
				{
				if(newSelection->IsSelected())
					{
					ReleaseCapture();
					newSelection->ShowObjectWindow();
					}
				}
			else
				{
				oldMouse = mouse;
				draggingSubject = 1;
				projPtr->ChangeSelection(hdc, GetUID(newSelection), wParam);
				projPtr->CalculateSelectedSubjectOffset(mouse);
				}
			}
		}
	else
		{
		originOffset = projPtr->GetOriginOffset();
		projPtr->UnSelectSubjects(hdc);
		selectRect.left = selectRect.right = mouse.x + originOffset.x;
		selectRect.top = selectRect.bottom = mouse.y + originOffset.y;
		draggingSelectRect = 1;
		DrawSelectRect(hdc,1);
		}

	ReleaseDC(hwnd, hdc);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  DragSubject
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  handles dragging of the selected subjects
//
// PARAMETERS..:  long lParam:  mouse coordinates
//
// RETURN VALUE:  non-zero if successful else zero
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////

BOOL PROJECTWINDOW::DragSubject(
	LPARAM lParam)
{
	RECT	clientRect;
	HDC		hdc;
	POINT mouse;
	RECT	refreshRect;
	POINT offset;
	POINT	adjustment;
	int		direction=0;
	POINT originOffset;
	HWND hwnd;
	PROJECT * projPtr = GetProjectPtr();

	A_MAKEPOINT(mouse, lParam);
	projPtr->GetClientBounds(&clientRect);
	hwnd = GetWindowHandle();
	if(draggingSubject)
		{
		projPtr->MakeDirty();
		if(autoScroll)
			{
			ResetOldMouse();
			autoScroll = FALSE;
			}
			hdc = GetDC(hwnd);
			if((oldMouse.x != mouse.x) || (oldMouse.y != mouse.y))
				{
				offset.x = mouse.x - oldMouse.x;
				offset.y = mouse.y - oldMouse.y;
				adjustment = VerifyOffset(offset);
				offset.x+=adjustment.x;
				offset.y+=adjustment.y;
				MoveAllSubjects(hdc,(LPPOINT)&offset, &refreshRect);
				oldMouse = mouse;
				oldMouse.x+=adjustment.x;
				oldMouse.y+=adjustment.y;
				}
			ReleaseDC(hwnd,hdc);
			if(mouse.x < clientRect.left)
				direction = GOLEFT;
			else if(mouse.x > clientRect.right)
				direction = GORIGHT;
			if(mouse.y < clientRect.top)
				direction|=GOUP;
			else if(mouse.y > clientRect.bottom)
				direction|=GODOWN;
			if(direction)
				AutoScroll(direction);
			return 1;
		}
	else if(draggingSelectRect)
		{
		originOffset = projPtr->GetOriginOffset();
		hdc = GetDC(hwnd);
		DrawSelectRect(hdc,0);
		selectRect.right = mouse.x + originOffset.x;
		selectRect.bottom = mouse.y + originOffset.y;
		DrawSelectRect(hdc,1);
		ReleaseDC(hwnd, hdc);
		if(mouse.x < clientRect.left)
			direction = GOLEFT;
		else if(mouse.x > clientRect.right)
			direction = GORIGHT;
		if(mouse.y < clientRect.top)
			direction|=GOUP;
		else if(mouse.y > clientRect.bottom)
			direction|=GODOWN;
		if(direction)
				AutoScrollSelectRect(direction);

			return 1;
		}
	else
		return 0;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	EndDrag
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  called when the mouse button is released to clean up after
//								a subject has been dragged or after the selection rectangle
//								has been dragged.
//
// PARAMETERS..:  long lParam: mouse coordinates.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL PROJECTWINDOW::EndDrag(
	LPARAM /*lParam*/)
	{
	HDC hdc;
	PROJECT * projPtr = GetProjectPtr();

	ReleaseCapture();
	if(draggingSubject)
		{
		draggingSubject = 0;
		if(projPtr->GridIsOn())
			SnapSubjectsToGrid(0);
		projPtr->AdjustLogBounds();
		projPtr->SnapLogBounds(VRANGE | HRANGE);
		projPtr->AdjustLastSubjectPosition();
		DetermineScrollBars();
		SetScrollBars(HRANGE | HPOS | VRANGE | VPOS);
		return 1;
		}
	else if(draggingSelectRect)
		{
		draggingSelectRect = 0;
		hdc = GetDC(GetWindowHandle());
		DrawSelectRect(hdc,0);
		ReleaseDC(GetWindowHandle(), hdc);
		if(projPtr->SnapLogBounds(HRANGE | VRANGE))
			{
			DetermineScrollBars();;
			SetScrollBars(HRANGE | HPOS | VRANGE | VPOS);
			}
		return 1;
		}
	else
		return 0;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	MoveAllSubjects
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  Moves all selected subjects relative to the logical bounds
//								as a result of a drag.
//
// PARAMETERS..:  HDC hdc:	device context.
//								LPPOINT offset:	amount to move.
//								LPRECT	unionRect:	here is where the union of the
//									rectangles of the subjects which were moved is returned.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::MoveAllSubjects(
	HDC 		hdc,
	LPPOINT offset,
	LPRECT	unionRect)
	{
	RECT 	       tempUnion;
	RECT	       holdUnion;
	SUBJECT * temp;
	int		       i=0;
	RUNTIMEUID   subjectList;

	subjectList = GetProjectPtr()->GetSubjectBlock();

	SetRectEmpty(unionRect);

	while((temp=(SUBJECT *)GetObject(subjectList, i))!=NULL)
		{
		if(temp->IsSelected())
			{
			temp->Move(hdc, offset, &tempUnion);
			holdUnion = *unionRect;
			UnionRect(unionRect, &holdUnion, &tempUnion);
			}
		}

	DrawAllSubjects(hdc, unionRect);

	}



/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SetClippingRegion
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  Sets the clipping region for dragging, scrolling and for
//									drawing the subjects.
//
// PARAMETERS..:  HDC hdc:  device context.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::SetClippingRegion(
	HDC hdc)
	{
	RECT tempRect;
	GetProjectPtr()->GetClientBounds(&tempRect);
	SetRectRgn(clipRegion,tempRect.left,tempRect.top,
		tempRect.right, tempRect.bottom);
	SelectClipRgn(hdc, clipRegion);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	AutoScroll
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  autoscrolls the projectwindow when the user drags outside
//								of the windows client area.
//
// PARAMETERS..:  int direction:  mask of which directions to scroll.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::AutoScroll(
	int direction)
	{
	RECT	winBounds;
	HDC		hdc;
	POINT	scrollAmount;
	int		hold;
	MSG		msg;
	unsigned scrollSet=0;
	POINT originOffset;
	POINT logBounds;
	BOOL 	logChanged = FALSE;
	HWND hwnd;
	PROJECT * projPtr = GetProjectPtr();

	hwnd = GetWindowHandle();
	msg.message = 0;
	projPtr->GetClientBounds(&winBounds);
	originOffset = projPtr->GetOriginOffset();
	logBounds = projPtr->GetLogBounds();
	hdc = GetDC(hwnd);
	do
		{
		scrollAmount.y=0;
		scrollAmount.x=0;
		if(direction & GOLEFT)
			{
			hold = originOffset.x;
			originOffset.x-=XSCROLLSTEP;
			if(originOffset.x < 0)
				{
				originOffset.x = 0;
				}
			scrollAmount.x = originOffset.x - hold;
			}
		else if(direction & GORIGHT)
			{
			hold = originOffset.x;
			originOffset.x+=XSCROLLSTEP;
			if(originOffset.x + winBounds.right > logBounds.x)
				{
				logBounds.x = winBounds.right + originOffset.x + XSCROLLSTEP;
				logChanged = TRUE;
				if(logBounds.x > XMAXPAGE)
					{
					logBounds.x = XMAXPAGE;
					originOffset.x = XMAXPAGE - (winBounds.right - winBounds.left)+1;
					}
				}
			scrollAmount.x = originOffset.x - hold;
			}

		if(direction & GOUP)
			{
			hold = originOffset.y;
			originOffset.y-=YSCROLLSTEP;
			if(originOffset.y < 0)
				{
				originOffset.y = 0;
				}
			scrollAmount.y = originOffset.y - hold;
			}
		else if(direction & GODOWN)
			{
			hold = originOffset.y;
			originOffset.y+=YSCROLLSTEP;
			if(originOffset.y + winBounds.bottom > logBounds.y)
				{
				logBounds.y = winBounds.bottom + originOffset.y + YSCROLLSTEP;
				logChanged = TRUE;
				if(logBounds.y > YMAXPAGE)
					{
					logBounds.y = YMAXPAGE;
					originOffset.y = YMAXPAGE - (winBounds.bottom - winBounds.top)+1;
					}
				}
			scrollAmount.y = originOffset.y - hold;
			}
		if(logChanged)
			projPtr->SetLogBounds(logBounds);
		projPtr->SetOriginOffset(originOffset);
		AdjustSelectedSubjectPosition(hdc, (LPPOINT)&scrollAmount);
		projPtr->AdjustLogBounds();
		if(direction & (GORIGHT | GOLEFT))
			{
			scrollSet|=HRANGE | HPOS;
			}
		if(direction & (GOUP | GODOWN))
			{
			scrollSet|=VRANGE | VPOS;
			}
		DetermineScrollBars();
		SetScrollBars(scrollSet);
		DrawAllSubjects(hdc,&winBounds);
		}
	while(!PeekMessage((LPMSG)&msg, hwnd, WM_MOUSEFIRST, WM_MOUSELAST,
			PM_NOREMOVE|PM_NOYIELD));
	ReleaseDC(hwnd, hdc);
	autoScroll = TRUE;

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	AjustSelectedSubjectPosition
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  adjust the offset to make sure selected subjects aren't
//									moved outside of the max and min page boundaries,
//									and then movse the selected subjects.
//
// PARAMETERS..:  HDC hdc: device context.
//								LPPOINT offset:  distance to move in the x and y direction.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::AdjustSelectedSubjectPosition(
	HDC	hdc,
	LPPOINT offset)
	{
	RECT 	       tempUnion;
	POINT		     adjustment;
	SUBJECT * temp;
	int		       i=0;
	RUNTIMEUID   subjectList;

	subjectList = GetProjectPtr()->GetSubjectBlock();
	adjustment = VerifyOffset(*offset);
	offset->x+=adjustment.x;
	offset->y+=adjustment.y;
	while((temp=(SUBJECT *)GetObject(subjectList,i))!=NULL)
		{
		if(temp->IsSelected())
			{
			temp->Move(hdc, offset, &tempUnion);
			}
		}
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	VerifyOffset
//
// CLASS.......:  PROEJECTWINDOW
//
// DESCRIPTION.:  verifys the offset in the x and y direction to make sure
//									that the selected subjects will not be moved outside of
//									the max and min page boundaries.  Computes an adjustment
//									for the offset if neccessary.
//
// PARAMETERS..:  POINT offset: distance to be moved.
//
// RETURN VALUE:  POINT adjustment to be made to the offset.
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
POINT PROJECTWINDOW::VerifyOffset(
	POINT offset)
	{
	SUBJECT * temp;
	int          i=0;
	POINT	       newOffset;
	POINT        adjustment;
	POINT        positionInProject;
	RUNTIMEUID   subjectList;

	newOffset = offset;
	subjectList = GetProjectPtr()->GetSubjectBlock();
	while((temp=(SUBJECT *)GetObject(subjectList, i))!=NULL)
		{
		if(temp->IsSelected())
			{
			positionInProject = temp->GetPositionInProject();
			if(positionInProject.x  + newOffset.x <= 0)
				{
				newOffset.x = -positionInProject.x;
				}
			else if(positionInProject.x + newOffset.x + SUBJECTWIDTH >= XMAXPAGE)
				{
				newOffset.x = XMAXPAGE - SUBJECTWIDTH - positionInProject.x;
				}
			if(positionInProject.y + newOffset.y <= 0)
				{
				newOffset.y = -positionInProject.y;
				}
			else if(positionInProject.y + newOffset.y + SUBJECTHEIGHT >= YMAXPAGE)
				{
				newOffset.y = YMAXPAGE - SUBJECTHEIGHT - positionInProject.y;
				}
			}
		}
	adjustment.x = newOffset.x - offset.x;
	adjustment.y = newOffset.y - offset.y;
	return adjustment;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ResetOldMouse
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  If the mouse is moved outside of the windows client area
//									and the subjects can't be moved any ther in that
//									direction, then the oldMouse position needs to be
//									adjusted so when the mouse is moved back into the client
//									area, it will keep its same position relative to the
//									selected subjects.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::ResetOldMouse()
	{
	SUBJECT *  temp;
	int           i=0;
	BOOL          xBreak = 0;
	BOOL          yBreak = 0;
	POINT         mouseOffset;
	POINT         originOffset;
	POINT         positionInProject;
	RUNTIMEUID    subjectList;
	PROJECT *  projPtr = GetProjectPtr();

	subjectList = projPtr->GetSubjectBlock();
	originOffset = projPtr->GetOriginOffset();
	while(((temp=(SUBJECT *)GetObject(subjectList, i))!=NULL) &&
		!(xBreak && yBreak))
		{
		if(temp->IsSelected())
			{
			positionInProject = temp->GetPositionInProject();
			mouseOffset = temp->GetMouseOffset();
			if(!xBreak)
				{
				if(oldMouse.x - mouseOffset.x<0)
					{
					oldMouse.x = positionInProject.x + mouseOffset.x - originOffset.x;
					xBreak=1;
					}
				else if(oldMouse.x - mouseOffset.x +SUBJECTWIDTH +
					originOffset.x >= XMAXPAGE)
					{
					oldMouse.x = positionInProject.x + mouseOffset.x - originOffset.x;
					xBreak=1;
					}
				}
			if(!yBreak)
				{
				if(oldMouse.y + mouseOffset.y + originOffset.y  <= 0)
					{
					oldMouse.y = mouseOffset.y  + positionInProject.y + originOffset.y;
					yBreak=1;
					}
				else if(oldMouse.y + mouseOffset.y +SUBJECTWIDTH +
					originOffset.y >= YMAXPAGE)
					{
					oldMouse.y = mouseOffset.y + positionInProject.y - originOffset.y;
					yBreak=1;
					}
				}
			}
		}
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ArbitraryHScroll
//
// CLASS.......:  ProjectWindow
//
// DESCRIPTION.:  scrolls an arbitrary amount in the x direction.
//
// PARAMETERS..:  long lParam:  contains the thumb position for thumb
//									tracking.
//								WORD wParam:	tells how much to scroll.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::ArbitraryHScroll(
	WPARAM wParam,
	LPARAM ) // lParam)
	{
	WORD		nCode		= GET_WM_VSCROLL_CODE(wParam, lParam);
	WORD		nPos		= GET_WM_VSCROLL_POS(wParam, lParam);
	RECT	clientRect;
	int		position;
	HDC 	hdc;
	unsigned int snapResult;
	POINT originOffset;
	POINT logBounds;
	PROJECT * projPtr = GetProjectPtr();

	logBounds = projPtr->GetLogBounds();
	originOffset = projPtr->GetOriginOffset();
	projPtr->GetClientBounds(&clientRect);
	switch(nCode)
		{
		case SB_BOTTOM:
			originOffset.x = logBounds.x - (clientRect.right - clientRect.left);
			break;

		case SB_TOP:
			originOffset.x = 0;
			break;

		case SB_PAGEUP:
			originOffset.x-=(int)((double) (clientRect.right -
				clientRect.left) * .75);
			if(originOffset.x < 0)
				originOffset.x = 0;
			break;

		case SB_PAGEDOWN:
			originOffset.x+=(int)((double)(clientRect.right -
				clientRect.left) * .75);
			if(originOffset.x + (clientRect.right - clientRect.left) >logBounds.x)
				originOffset.x = logBounds.x - (clientRect.right - clientRect.left);
			break;

		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			position = nPos;
			originOffset.x = position * XSCROLLSTEP;
			if(originOffset.x + (clientRect.right - clientRect.left) >logBounds.x)
				originOffset.x = logBounds.x -(clientRect.right - clientRect.left);
			break;

		}
	projPtr->SetOriginOffset(originOffset);
	hdc = GetDC(GetWindowHandle());
	if(!originOffset.x)
		{
		snapResult = projPtr->SnapLogBounds(HRANGE);
		if(snapResult)
			SetScrollBars(snapResult);
		}

	SetScrollBars(HPOS);
	if(snapResult)
		DetermineScrollBars();

	DrawAllSubjects(hdc, &clientRect);
	ReleaseDC(GetWindowHandle(), hdc);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ArbitraryVScroll
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  scrolls an arbitrary amount in the y direction.
//
// PARAMETERS..:  long lParam:  thumb position used for thumb tracking.
//								WORD wParam:  tells how  to scroll.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::ArbitraryVScroll(
	WPARAM wParam,
	LPARAM ) // lParam)
	{
	WORD		nCode		= GET_WM_VSCROLL_CODE(wParam, lParam);
	WORD		nPos		= GET_WM_VSCROLL_POS(wParam, lParam);
	RECT	clientRect;
	int		position;
	HDC 	hdc;
	unsigned int snapResult;
	POINT originOffset;
	POINT logBounds;
	PROJECT * projPtr = GetProjectPtr();

	logBounds = projPtr->GetLogBounds();
	originOffset = projPtr->GetOriginOffset();
	projPtr->GetClientBounds(&clientRect);
	switch(nCode)
		{
		case SB_BOTTOM:
			originOffset.y = logBounds.y - (clientRect.bottom - clientRect.top);
			break;

		case SB_TOP:
			originOffset.y = 0;
			break;

		case SB_PAGEUP:
			originOffset.y-=(int)((double) (clientRect.bottom -
				clientRect.top) * .75);
			if(originOffset.y < 0)
				originOffset.y = 0;
			break;

		case SB_PAGEDOWN:
			originOffset.y+=(int)((double)(clientRect.bottom -
				clientRect.top) * .75);
			if(originOffset.y + (clientRect.bottom - clientRect.top) >logBounds.y)
				originOffset.y = logBounds.y - (clientRect.bottom - clientRect.top);
			break;

		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			position = nPos;
			originOffset.y = position * YSCROLLSTEP;
			if(originOffset.y + (clientRect.bottom - clientRect.top) >logBounds.y)
				originOffset.y = logBounds.y  - (clientRect.bottom - clientRect.top);
			break;
		}
	projPtr->SetOriginOffset(originOffset);
	hdc = GetDC(GetWindowHandle());
	if(!originOffset.y)
		{
		snapResult = projPtr->SnapLogBounds(VRANGE);
		if(snapResult)
			SetScrollBars(snapResult);
		}

	SetScrollBars(VPOS);
	if(snapResult)
		DetermineScrollBars();

	DrawAllSubjects(hdc, &clientRect);
	ReleaseDC(GetWindowHandle(), hdc);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	DrawSelectRect
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  Draws the selection rectangle.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::DrawSelectRect(
	HDC hdc,
	BOOL selectTheSubjects)
	{
	HPEN oldPen;
	POINT originOffset;

	originOffset = GetProjectPtr()->GetOriginOffset();
	SetClippingRegion(hdc);
	if(selectTheSubjects)
		SelectSubjectsInRect(hdc);
	SetROP2(hdc, R2_NOTXORPEN);

	oldPen = SelectPen(hdc, PROJECT::dragPen);
	SelectBrush(hdc, GetStockBrush(HOLLOW_BRUSH));
	Rectangle(hdc, selectRect.left - originOffset.x,
		selectRect.top - originOffset.y,
		selectRect.right - originOffset.x,
		selectRect.bottom - originOffset.y);
	SelectPen(hdc, oldPen);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	AutoScrollSelectRect
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  auto scrolls the selection rectangle if the user drags
//									outside of the windows client area.
//
// PARAMETERS..:  int direction:  mask of which directions to scroll.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::AutoScrollSelectRect(
	int direction)
	{
	RECT	winBounds;
	HDC		hdc;
	MSG	msg;
	POINT	hold;
	POINT originOffset;
	POINT logBounds;
	PROJECT * projPtr = GetProjectPtr();

	logBounds = projPtr->GetLogBounds();
	originOffset = projPtr->GetOriginOffset();
	projPtr->GetClientBounds(&winBounds);
	hdc = GetDC(GetWindowHandle());
	do
		{
		hold = originOffset;
		if(direction & GOLEFT)
			{
			originOffset.x-=XSCROLLSTEP;
			if(originOffset.x < 0)
				originOffset.x = 0;
			}
		if(direction & GORIGHT)
			{
			originOffset.x+=XSCROLLSTEP;
			if(originOffset.x + winBounds.right > logBounds.x)
				originOffset.x= logBounds.x - winBounds.right;
			}
		if(direction & GOUP)
			{
			originOffset.y-=YSCROLLSTEP;
			if(originOffset.y<0)
				originOffset.y = 0;
			}
		if(direction & GODOWN)
			{
			originOffset.y+=YSCROLLSTEP;
			if(originOffset.y + winBounds.bottom > logBounds.y)
				originOffset.y = logBounds.y - winBounds.bottom;
			}
		hold.x = originOffset.x - hold.x;
		hold.y = originOffset.y - hold.y;
		if(hold.x || hold.y)
			{
			projPtr->SetOriginOffset(originOffset);
			if(direction & (GOLEFT | GORIGHT))
				SetScrollBars(HPOS);
			if(direction & (GOUP | GODOWN))
				SetScrollBars(VPOS);
			selectRect.right+=hold.x;
			selectRect.bottom+=hold.y;
			DrawAllSubjects(hdc,&winBounds);
			DrawSelectRect(hdc,1);
			}
		}
	while(!PeekMessage((LPMSG)&msg, GetWindowHandle(),
		WM_MOUSEFIRST, WM_MOUSELAST, PM_NOREMOVE|PM_NOYIELD));

	ReleaseDC(GetWindowHandle(), hdc);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SelectSubjectsInRect
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  selects all subjects which intersect the selection
//									rectangle.
//
// PARAMETERS..:  HDC hdc:	device context.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::SelectSubjectsInRect(
	HDC hdc)
	{
	RECT          clientRect;
	POINT         offset;
	SUBJECT *  temp;
	RUNTIMEUID    subjectList;
	PROJECT *  projPtr = GetProjectPtr();
	int           i=0;

	subjectList = projPtr->GetSubjectBlock();
	projPtr->GetClientBounds(&clientRect);
	offset.x=0;
	offset.y=0;
	while((temp=(SUBJECT *)GetObject(subjectList, i))!=NULL)
		{
		if(temp->SubjectIntersectsRect(hdc,&selectRect))
			{
			if(!temp->IsSelected())
				{
				temp->SetSelected(TRUE);
				temp->DrawASubject(hdc,&clientRect, offset);
				}
			}
		else if(temp->IsSelected())
			{
			temp->SetSelected(FALSE);
			temp->DrawASubject(hdc,&clientRect, offset);
			}
		}
	}

void PROJECTWINDOW::SelectAllSubjects()
	{
	HDC           hdc = GetDC(hWnd);
	PROJECT *  projPtr = GetProjectPtr();
	RECT          clientRect;
	RUNTIMEUID    subjectBlock = projPtr->GetSubjectBlock();
	SUBJECT *  subjectPtr;
	int           i=0;

	projPtr->GetClientBounds(&clientRect);
	while(0!=(subjectPtr = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		if(!subjectPtr->IsSelected())
			subjectPtr->SetSelected(TRUE);
		}
	DrawAllSubjects(hdc, &clientRect);
	ReleaseDC(hWnd, hdc);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SnapSubjectsToGrid
//
// CLASS.......:  ProjectWindow
//
// DESCRIPTION.:  Snaps all subjects to the project window grid.
//
// PARAMETERS..:  BOOL snapAll:  true if every subject should be snapped.
//									false if only the selected subjects should be snapped.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::SnapSubjectsToGrid(
	BOOL snapAll)
	{
	SUBJECT *  temp;
	RECT          clientRect;
	RUNTIMEUID    subjectList;
	PROJECT *  projPtr = GetProjectPtr();
	HDC           hdc;
	BOOL          snapped = 0;
	int           i=0;

	subjectList = projPtr->GetSubjectBlock();

	while((temp = (SUBJECT *)GetObject(subjectList, i))!=NULL)
		{
		if(temp->IsSelected() || snapAll)
			{
			snapped |= temp->AdjustToGrid();
			}
		}

	if(snapped)
		{
		hdc = GetDC(GetWindowHandle());
		projPtr->GetClientBounds(&clientRect);
		DrawAllSubjects(hdc, &clientRect);
		ReleaseDC(GetWindowHandle(), hdc);
		}
	}



/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	DetermineScrollBars
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  determines which scroll bars should be visible or greyed.
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::DetermineScrollBars()
	{
	RECT winRect;
	POINT logBounds;
	POINT originOffset;
	PROJECT * projPtr = GetProjectPtr();

	logBounds = projPtr->GetLogBounds();
	originOffset = projPtr->GetOriginOffset();
	projPtr->GetClientBounds(&winRect);
	if(originOffset.x == 0 && logBounds.x <= winRect.right)
		{
		if(hScrollShowing)
			{
			hScrollShowing = FALSE;
			ShowWindow(hScroll, SW_HIDE);
			GreyScrollBar(hScroll);
			}
		}
	else if(!hScrollShowing)
		{
		hScrollShowing = TRUE;
		ShowWindow(hScroll, SW_SHOWNA);
		UpdateWindow(hScroll);
		}

	if(originOffset.y == 0 && logBounds.y <= winRect.bottom)
		{
		if(vScrollShowing)
			{
			vScrollShowing = FALSE;
			ShowWindow(vScroll, SW_HIDE);
			}
		}
	else if(!vScrollShowing)
		{
		vScrollShowing = TRUE;
		ShowWindow(vScroll, SW_SHOWNA);
		UpdateWindow(vScroll);
		}
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SetScrollBars
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  Sets the appropriate scroll bar's range and position.
//
// PARAMETERS..:  int command:  mask of which scroll bar's range and position
//									should be set.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::SetScrollBars(
	int command)
	{
	RECT clientRect;
	POINT logBounds;
	POINT	originOffset;
	PROJECT * projPtr = GetProjectPtr();

	originOffset = projPtr->GetOriginOffset();
	logBounds = projPtr->GetLogBounds();
	projPtr->GetClientBounds(&clientRect);
	if(vScrollShowing)
		{
		if(command & VRANGE)
			SetScrollRange(vScroll, SB_CTL, 0,
				(int)ceil((double)(logBounds.y -
				(clientRect.bottom - clientRect.top))/(double)YSCROLLSTEP),
				FALSE);
		if(command & VPOS)
			SetScrollPos(vScroll, SB_CTL,
				(int)ceil((double)originOffset.y/(double)YSCROLLSTEP),
				TRUE);
		}

	if(hScrollShowing)
		{
		if(command & HRANGE)
			SetScrollRange(hScroll, SB_CTL, 0,
				(int)ceil((double)(logBounds.x -
				(clientRect.right - clientRect.left))/(double)XSCROLLSTEP),
				FALSE);
		if(command & HPOS)
			SetScrollPos(hScroll, SB_CTL,
				(int)ceil((double)originOffset.x/(double)XSCROLLSTEP),
				TRUE);
		}
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	GreyScrollBar
//
// CLASS.......:  PROJECTWINDOW
//
// DESCRIPTION.:  Greys the appropriate scroll bar
//
// PARAMETERS..:  HWND sBar:  handle of scroll bar to be greyed.
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void PROJECTWINDOW::GreyScrollBar(HWND sBar)
	{
	RECT barRect;
	HDC hdc;
	HDC hdcMem;
	HPEN thePen;
	HPEN oldPen;
	HBRUSH theBrush;
	int width;
	int height;
	HWND hwnd;

	hwnd = GetWindowHandle();
	GetWindowRect(sBar, &barRect);
	ScreenToClient(hwnd, (LPPOINT)&barRect.left);
	ScreenToClient(hwnd, (LPPOINT)&barRect.right);
	thePen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_SCROLLBAR));
	theBrush = CreateSolidBrush(GetSysColor(COLOR_SCROLLBAR));
	hdc = GetDC(hwnd);
	hdcMem = CreateCompatibleDC(hdc);
	oldPen = SelectPen(hdc,thePen);
	SelectBrush(hdc, theBrush);
	Rectangle(hdc,barRect.left,
		barRect.top,
		barRect.right,
		barRect.bottom);
	SelectBrush(hdc, GetStockBrush(HOLLOW_BRUSH));
	SetROP2(hdc, R2_NOTCOPYPEN);
	Rectangle(hdc, barRect.left,
		barRect.top,
		barRect.right,
		barRect.bottom);
	if(barRect.right - barRect.left > barRect.bottom - barRect.top)
		{
			width = GetSystemMetrics(SM_CXHSCROLL)-2;
			height = GetSystemMetrics(SM_CYHSCROLL)-2;
			SelectBitmap(hdcMem, WORKSHEET::leftScrollArrow);
			BitBlt(hdc,barRect.left+1,
				barRect.top + 1,
				width,
				height,
				hdcMem,
				0,0,
				SRCCOPY);
			SelectBitmap(hdcMem, WORKSHEET::rightScrollArrow);
			BitBlt(hdc, barRect.right - width-1,
				barRect.top + 1,
				width,
				height,
				hdcMem,
				0,0,
				SRCCOPY);
		}
	else
		{
			width = GetSystemMetrics(SM_CXVSCROLL)-2;
			height = GetSystemMetrics(SM_CYVSCROLL)-2;
			SelectBitmap(hdcMem, WORKSHEET::upScrollArrow);
			BitBlt(hdc,barRect.left+1,
				barRect.top + 1,
				width,
				height,
				hdcMem,
				0,0,
				SRCCOPY);
			SelectBitmap(hdcMem, WORKSHEET::downScrollArrow);
			BitBlt(hdc, barRect.left+1,
				barRect.bottom - height-1,
				width,
				height,
				hdcMem,
				0,0,
				SRCCOPY);
		}
	SelectPen(hdc, oldPen);
	ReleaseDC(hwnd, hdc);
	DeleteDC(hdcMem);
	DeleteBrush(theBrush);
	DeletePen(thePen);

	}


void PROJECTWINDOW::CloseEditField()
	{
	char buffer[SUBJECT_NAME_LENGTH+1];
	SUBJECT * subjectPtr;
	RECT textRect;
	HDC hdc = GetDC(GetWindowHandle());

	GetWindowText(editField, (LPSTR)buffer, SUBJECT_NAME_LENGTH+1);
	subjectPtr = (SUBJECT *)GetObject(holdUID);

	//Make sure the name is not blank.
	//If it is blank, then restore name to original
	//This idea came from the Mac system 7.0.1
	if(lstrlen((LPSTR)buffer) > 0)
		subjectPtr->SetName((LPSTR)buffer);
	else
		subjectPtr->SetName(oldName);

	subjectPtr->SetSubjectWindowName();

	// update name in navigator

	if (MDI::GetNavigatorPtr())
		MDI::GetNavigatorPtr()->
		 SetItemName(holdUID, GetProjectPtr()->fileObjPtr, buffer);

	SetWindowLong(editField, GWL_WNDPROC, (DWORD)oldEditProc);
	HideCaret(editField);
	DestroyWindow(editField);
	editField = NULL;
	PROGSYSTEM::SetActiveEditField(NULL);

	//The edit field does not seem to quite cover the entire text rect
	//so invalidate the area under the text rect to make sure that it
	//will be redrawn
	subjectPtr->GetTextRect(hdc, &textRect);
	ReleaseDC(hWnd, hdc);
	InvalidateRect(GetWindowHandle(), &textRect, FALSE);
	UpdateWindow(GetWindowHandle());
	}


//WARNING:  This subclass procedure destroys it's owning window by a call
//	made from the body of this proecture.  When CloseEditField is called,
//  the original window proc is replaced and the edit field is destroyed.
//  The only thing to do when we return is to clean the stack and that seems
//  to happen O.K.
//  MAKE SURE NOT TO ACCESS ANY DATA AFTER CALLING CloseEditField();
LRESULT CALLBACK PROJECTWINDOW::EditFieldProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	switch(message)
		{
		case WM_CHAR:
			if(wParam == VK_RETURN)
				{
				((PROJECTWINDOW *)PROGSYSTEM::GetCurrentProject()->GetWindowPtr())
					->CloseEditField();
				return(0);
				}
			break;

		case WM_UNDO:
			//Restore text to the original then close the edit field
    	SetWindowText(
			((PROJECTWINDOW *)PROGSYSTEM::GetCurrentProject()->GetWindowPtr())
				->editField, ((PROJECTWINDOW *)PROGSYSTEM::GetCurrentProject()->
				GetWindowPtr())->oldName);
			((PROJECTWINDOW *)PROGSYSTEM::GetCurrentProject()->GetWindowPtr())
				->CloseEditField();
			return(0);


		}
	return CallWindowProc((FARPROC)((PROJECTWINDOW *)(PROGSYSTEM::GetCurrentProject()->
		GetWindowPtr()))->oldEditProc, hwnd, message, wParam, lParam);
	}


void PROJECTWINDOW::ReshapeEditField()
{
	char buffer[SUBJECT_NAME_LENGTH + 2];
	RECT bounds;
	HDC hdc;
	DWORD length;
	HWND parent;
	SUBJECT * subjectPtr;
	RECT textRect;
	POINT caretPos;
	RECT refresh;
	SIZE	textSize;

	parent = GetWindowHandle();
	GetWindowText(editField, buffer, SUBJECT_NAME_LENGTH+1);
	hdc = GetDC(parent);
	SelectObject(hdc, WORKSHEET::subFont);
	GetTextExtentPoint32(hdc, buffer, lstrlen(buffer), &textSize);
	length = textSize.cx;
	ReleaseDC(parent, hdc);
	GetClientRect(editField, &bounds);
	if(LOWORD(length) != (bounds.right - bounds.left))
		{
		GetWindowRect(editField, &bounds);
		ScreenToClient(parent, (LPPOINT)&bounds);
		ScreenToClient(parent, (LPPOINT)&bounds.right);
		subjectPtr = (SUBJECT *)GetObject(holdUID);
		hdc = GetDC(parent);
		subjectPtr->GetTextRect(hdc, &textRect);
		ReleaseDC(parent,hdc);
		GetCaretPos((LPPOINT)&caretPos);
		HideCaret(editField);
		MoveWindow(editField,(int)(textRect.left + textRect.right -
			(int)LOWORD(length))/(int)2,
			bounds.top,
			LOWORD(length),
			bounds.bottom - bounds.top,
			TRUE);

		if(caretPos.x >= LOWORD(length))
			{
			SetCaretPos(LOWORD(length), 0);
			refresh.right = textRect.right;
			refresh.left = textRect.left + LOWORD(length);
			refresh.top = textRect.top;
			refresh.bottom = textRect.bottom;
			InvalidateRect(parent, &refresh, TRUE);
			}
		UpdateWindow(parent);
		ShowCaret(editField);
		}
	}

void PROJECTWINDOW::CenterSelection()
	{
	SUBJECT *  temp;
	PROJECT *  projectPtr = GetProjectPtr();
	RUNTIMEUID    subjectBlock = projectPtr->GetSubjectBlock();
	POINT         originOffset = projectPtr->GetOriginOffset();
	HDC           hdc = GetDC(hWnd);
	RECT          unionRect;
	RECT          subRect;
	RECT          clientRect;
	int           i = 0;

	SetRectEmpty(&unionRect);
	while(0!=(temp = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		if(temp->IsSelected())
			{
			temp->GetBoundingRectangle(hdc, &subRect);
			UnionRect(&unionRect, &unionRect, &subRect);
			}
		}
	if(unionRect.left != 0 || unionRect.right != 0 || unionRect.top != 0
		|| unionRect.bottom != 0)
		{
		//unionRect is already in client coordinates
		projectPtr->GetClientBounds(&clientRect);
		if(unionRect.right - unionRect.left > clientRect.right - clientRect.left
			|| unionRect.bottom - unionRect.top >
			clientRect.bottom - clientRect.top)
			{
			//place upper left corner of unionRect even the upper left corner
			//of the clientRect
			originOffset.x+=unionRect.left;
			originOffset.y+=unionRect.top;
			}
		else
			{
			//center unionRect in clientArea
			originOffset.x+=((unionRect.right + unionRect.left)/2 -
				(clientRect.right - clientRect.left)/2);
			originOffset.y+=((unionRect.bottom + unionRect.top)/2 -
				(clientRect.bottom - clientRect.top)/2);
			}
		projectPtr->SetOriginOffset(originOffset);
		GrowOrShrinkLogBounds();
		DrawAllSubjects(hdc,&clientRect);
		}
	ReleaseDC(hWnd, hdc);
	}

BOOL PROJECTWINDOW::Busy()
	{
	return draggingSubject | draggingSelectRect;
	}
