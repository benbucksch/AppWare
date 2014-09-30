#include "almsys.h"
#include <math.h>

#include "compstat.h"
#include "progsys.h"
#include "oiputils.h"

#define COMPSTAT_ICON 2000
#define COMPSTAT_OPERATION 	(COMPSTAT_ICON + 2)
#define COMPSTAT_FILENAME  	(COMPSTAT_ICON + 4)
#define COMPSTAT_STATUSBAR 	(COMPSTAT_ICON + 6)

#define COMPSTAT_DONEINTERVAL 700

COMPILESTATUS * 	COMPILESTATUS::thisPtr;
HBITMAP           COMPILESTATUS::upsfPict=NULL;

COMPILESTATUS::COMPILESTATUS(
	HWND owner,
	LPCSTR operation,
	LPCSTR fileName,
	long maxPoints,
	long initStatus,
	LPCSTR title,
	long	typeStatus)
	{
	oldEdge = -1;
	hDlgCompStat = NULL;
	totalPoints = maxPoints;
	if(totalPoints < 1)
		totalPoints = 1;
	currentPoints = initStatus;
	thisPtr = this;
	this->operation = operation;
	this->fileName = fileName;
	if(title != NULL)
		this->title = title;
	else
		this->title = (LPSTR)""; //use default title

	status = typeStatus;
	if(status & COMPSTAT_COMPILE)
		{
		hDlgCompStat = CreateDialog(PROGSYSTEM::hInst, "COMPILESTATUS",
			owner, (DLGPROC)COMPILESTATUS::CompStatProc);
		}
	else if(status & COMPSTAT_UPSF)
		{
		//We need to load the picture before creating the dialog so the
		//dialog has something to draw in itself.
		upsfPict = LoadBitmap(PROGSYSTEM::hInst, (LPSTR)"upsf_PICTURE");
		hDlgCompStat = CreateDialog(PROGSYSTEM::hInst, "UPSFSTATUS",
			owner, (DLGPROC)COMPILESTATUS::CompStatProc);
		}
	}

COMPILESTATUS::~COMPILESTATUS()
	{
	if(hDlgCompStat)
		DestroyWindow(hDlgCompStat);
	if((status & COMPSTAT_UPSF) && upsfPict)
		{
		DeleteObject(upsfPict);
		upsfPict = NULL;
		}
	}


void COMPILESTATUS::UpdateStatus(
	long points)
	{
	HWND ctrl;

	if(hDlgCompStat)
		{
		currentPoints+=points;
		ctrl = GetDlgItem(hDlgCompStat, COMPSTAT_STATUSBAR);
		InvalidateRect(ctrl, NULL, FALSE);
		UpdateWindow(ctrl);
		}
	}

void COMPILESTATUS::FillToEnd()
	{
	HWND ctrl;

	if(hDlgCompStat)
		{
		currentPoints = totalPoints;
		ctrl = GetDlgItem(hDlgCompStat, COMPSTAT_STATUSBAR);
		InvalidateRect(ctrl, NULL, FALSE);
		UpdateWindow(ctrl);
		}
	}

void COMPILESTATUS::ChangeName(
	LPCSTR newOperation,
	LPCSTR newFileName)
	{
	if(hDlgCompStat)
		{
		operation = newOperation;
		fileName = newFileName;
		SetDlgItemText(hDlgCompStat, COMPSTAT_OPERATION, operation);
		UpdateWindow(GetDlgItem(hDlgCompStat, COMPSTAT_OPERATION));
		SetDlgItemText(hDlgCompStat, COMPSTAT_FILENAME, fileName);
		UpdateWindow(GetDlgItem(hDlgCompStat, COMPSTAT_FILENAME));
		}
	}

void COMPILESTATUS::ShowFinish()
	{
	if(hDlgCompStat)
		{
		ChangeName("Done!", fileName);
		SleepEx(COMPSTAT_DONEINTERVAL, FALSE);
		}
	}

void COMPILESTATUS::ChangeSize(
	long newSize)
	{
	if(hDlgCompStat)
		{
		totalPoints = newSize;
		if(totalPoints < 1)
			totalPoints = 1;
		}
	}

void COMPILESTATUS::DrawStatusBar(
	LPDRAWITEMSTRUCT di)
	{
	RECT clientRect;
	int marker;
	HDC hdcMem;
	HDC holdDC;
	HBITMAP bmp;
	char percent[8];
	RECT textRect;
	int  oldDC;
	SIZE textDimensions;


	GetClientRect(di->hwndItem, &clientRect);

	//make sure the percentage will no be greater than 100
	if(currentPoints > totalPoints)
		currentPoints = totalPoints;

	//determine how much is filled
	marker = (int)ceil((double)currentPoints *
		(double)clientRect.right / (double)totalPoints);

	if(marker != oldEdge)
		{
		oldDC = SaveDC(di->hDC);
		hdcMem = CreateCompatibleDC(di->hDC);
		bmp = CreateCompatibleBitmap(di->hDC, clientRect.right+1,
			clientRect.bottom+1);

		if(bmp)
			{
			SelectObject(hdcMem, bmp);
			holdDC = hdcMem;
			}
		else
			holdDC = di->hDC;

		oldEdge = marker;

		//draw filled part
		SelectObject(holdDC, GetStockObject(BLACK_BRUSH));
		SelectObject(holdDC, GetStockObject(BLACK_PEN));
		Rectangle(holdDC, clientRect.left, clientRect.top, marker,
			clientRect.bottom);

		//draw unfilled part
		SelectObject(holdDC, GetStockObject(WHITE_BRUSH));
		Rectangle(holdDC, marker, clientRect.top,clientRect.right,
			clientRect.bottom);

		//put the percentage text in percent
		wsprintf((LPSTR)percent, (LPSTR)"%u%%", (unsigned int)ceil(100 *
			(double)marker / (double)clientRect.right));

		I_VERIFY(GetTextExtentPoint32(holdDC, (LPSTR)percent,
			lstrlen((LPSTR)percent), &textDimensions));
		textRect = clientRect;
		SetTextAlign(holdDC, TA_CENTER|TA_TOP);

		SetBkMode(holdDC, TRANSPARENT);
		//draw text in filled part in white
		SetTextColor(holdDC, RGB(255,255,255));

		textRect.right = marker;
		ExtTextOut(holdDC, clientRect.right/2, (clientRect.bottom -
			textDimensions.cy)/2, ETO_CLIPPED, (LPRECT)&textRect,
			(LPSTR)percent, lstrlen((LPSTR)percent), 0);

		//draw text in the unfilled part in black
		SetTextColor(holdDC, RGB(0,0,0));
		textRect.right = clientRect.right-1;
		textRect.left = marker+1;
		ExtTextOut(holdDC, clientRect.right/2, (clientRect.bottom -
			textDimensions.cy)/2, ETO_CLIPPED, (LPRECT)&textRect,
			(LPSTR)percent, lstrlen((LPSTR)percent), 0);

		if(bmp)
			{
			BitBlt(di->hDC,0,0,clientRect.right,clientRect.bottom,holdDC,0,0,
				SRCCOPY);
			DeleteDC(hdcMem);
			DeleteObject(bmp);
			}

		RestoreDC(di->hDC, oldDC);
		}
	}


BOOL CALLBACK COMPILESTATUS::CompStatProc(
	HWND hDlg,
	UINT msg,
	WPARAM /*wParam*/,
	LPARAM lParam)
	{
	LPDRAWITEMSTRUCT di;
	HDC hdc;
	HDC hdcMem;
	RECT paintRect;
	PAINTSTRUCT ps;
	RECT childRect;
	BITMAP bmp;
	RECT clientRect;

	switch(msg)
		{
		case WM_INITDIALOG:
			CenterDialog(hDlg, PROGSYSTEM::hWndFrame, FALSE);
			if(thisPtr->title.strlen() > 0)
				SetWindowText(hDlg, thisPtr->title);
			SetDlgItemText(hDlg, COMPSTAT_OPERATION, thisPtr->operation);
			SetDlgItemText(hDlg, COMPSTAT_FILENAME, thisPtr->fileName);
			break;

		case WM_PAINT:
			if((thisPtr->status & COMPSTAT_UPSF) && upsfPict)
				{
				//bitblt in the UPSF picture
				hdc = BeginPaint(hDlg, (LPPAINTSTRUCT)&ps);
				GetWindowRect(GetDlgItem(hDlg, COMPSTAT_OPERATION), (LPRECT)&childRect);
				ScreenToClient(hDlg, (LPPOINT)&childRect.left);
				GetObject(upsfPict ,sizeof(BITMAP), (LPVOID)&bmp);
				GetClientRect(hDlg, (LPRECT)&clientRect);
				//center the bitmap between the caption and the fist text field
				paintRect.left = (clientRect.right - bmp.bmWidth)/2;
				paintRect.top = (childRect.top - bmp.bmHeight)/2;
				paintRect.right = paintRect.left + bmp.bmWidth -1;
				paintRect.bottom = paintRect.top + bmp.bmHeight-1;

				//Since we don't need clientRect anymore,
				//place the intersection of the rectangle where the picture should
				//go and the update rectangle into clientRect.
				if(IntersectRect((LPRECT)&clientRect, (LPRECT)&paintRect,
					(LPRECT)&ps.rcPaint))
					{
					hdcMem = CreateCompatibleDC(hdc);
					SelectObject(hdcMem, upsfPict);
					BitBlt(hdc,clientRect.left, clientRect.top,
						clientRect.right - clientRect.left+1,
						clientRect.bottom - clientRect.top+1,
						hdcMem,
						clientRect.left - paintRect.left,
						clientRect.top - paintRect.top,
						SRCCOPY);
					DeleteDC(hdcMem);
					}
				EndPaint(hDlg, (LPPAINTSTRUCT)&ps);
				return 0;
				}
			break;

		case WM_DRAWITEM:
			di = (LPDRAWITEMSTRUCT)lParam;
			switch(di->CtlID)
				{
				case COMPSTAT_STATUSBAR:
					thisPtr->DrawStatusBar(di);
					return TRUE;
				}
			break;
		}
	return FALSE;
	}
