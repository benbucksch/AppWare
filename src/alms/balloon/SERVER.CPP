//				  									
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//  A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//  THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//  ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          "server.cpp"
//
// AUTHOR:        Lee E. Thayer
//
// DESCRIPTION:   Server for balloon prompts
//
//	
//	This is a balloon server that provides all of the services and 
// support for balloon prompts for any window that registers itself
// as a balloon client (see section below on "REGISTERING A BALLOON 
//	CLIENT").
//
//	STARTING THE BALLOON SERVER
//
//		The way to start the balloon server is via the 
//		Balloon_RegisterClient() API call.  If it is the first client
///	to be registered, the balloon server will automatically be
// 	started up.  (It will not "re-startup" for each subsequent
//		client registration since it is already running).
//
//		Once the balloon server is started, it will create
// 	a balloon window.  As long as the balloon server is running, 
//		this balloon window will always exist.  It's default state 
//		will be invisible with no text.  However, as the user moves 
//		the mouse over registered client windows, the balloon window 
//		may become activated (according to timing and movement behavior) 
//		and its text will change cooresponding to what object it is over.
//
//
//	CLOSING DOWN THE BALLOON SERVER
//
//		The way to close the balloon server is through calling the
//		Balloon_ShutdownServer() API.
//
//	REGISTERING A BALLOON CLIENT
//
//		There are three qualifications to become a balloon client:
//
//		1)  Include "balloon.h" in the file where the client window's 
//			window procedure is defined.  "Client here means a 
// 		window that uses the services of the balloon server.  
//
//		2) 	Register the balloon client with the balloon server using 
//			the "Balloon_RegisterClient()" API call. 
//
//		3) 	Properly handle the BALLOON_REQUESTPROMPT message.
//
//				The balloon server will send balloon clients
//				this message when it detects that the mouse has been 
//				over the client for a certain amount of time.  As 
//				parameters, the balloon server will pass the client 
//				its window handle (in WPARAM) and the mouse 
//				coordinates (in LPARAM).
//
//				Upon receiving this message, the balloon client, must
//				determine what object the mouse is over (ie. bitmap, 
//				etc.).  Once the proper object and string have been 
//				identified by the balloon client, it will need to 
//				pack up the string into the WPARAM paramenter and 
//				return it to the balloon server via the 
//				BALLOON_UPDATEPROMPT message.
//
//				The following sample code represents one way that this 
//				could be done:
//
//					char *c = new char[strlen(szSampleString)];
//					strcpy (c, szSampleString);
//		
//					SendMessage((HWND)wParam, 
//									BALLOON_UPDATEPROMPT, 
//									(WPARAM)c, 
//									TRUE);
//
//				Note:	If memory is allocated to get the string, then
//						pass a "TRUE" is lParam.  This will tell
//						the balloon server to free the memory once
//						its done with the string.  Otherwise, pass
//						FALSE.
//
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <stdlib.h>
#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>
#include <o_wndshp.h>

#include "balloon.h"
#include "server.h"
#include "util.h"
#include "init.h"


// Global variables
HHOOK	mouseHook;
HWND 	hwndBalloon;


// Internal prototypes
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);

const char *szDefaultFont = "MS Sans Serif";			

#define DEFAULT_BKCOLOR	RGB(241, 255, 105)	// canary yellow
#define DEFAULT_TEXTCOLOR	RGB(0, 0, 0)		// black
#define DEFAULT_BRDCOLOR	RGB(0, 0, 0)		// black
#define DEFAULT_BRDWIDTH	1
#define DEFAULT_DELAY		400					// 4/1Oths of a second
#define DEFAULT_POINTSIZE	10
#define DEFAULT_WEIGHT		400					// light


////////////////////////////////////////////////////////////////////
//			
//			          BALLOON (INTERNAL) ROUTINES
//
// 	The following are balloon server routines that are internal
// 	and are not called directly by external clients as APIs.
//
//
////////////////////////////////////////////////////////////////////

//****************************************************************
//
//  Constructor
//
//****************************************************************
Balloon::Balloon()
{
	// Allocate memory for substructures and strings
	this->text        = new BLN_TEXT;
	this->border      = new BLN_BORDER;
	this->pos         = new BLN_POS;
	this->text->font  = new BLN_FONT;

	// Start capturing mouse messages
	mouseHook = SetWindowsHookEx(WH_MOUSE, MouseProc,
									(HINSTANCE)NULL, GetCurrentThreadId());

	// Necessary default to allow server to run without GPFing.
	// However, additional default settings in Balloon::Init are 
	// necessary for desirable balloon prompts.
	bClientIndex   = 0;		
	hwndFocus      = 0;		
	fbMakeBalloons = FALSE;
	fbShowBalloons = FALSE;

	border->nStyle = PS_SOLID;
	border->bWidth = 1;
	border->color  = RGB(0, 0, 0);

	lstrcpyn(text->font->szFaceName, szDefaultFont, LF_FACESIZE);

	GetClientRect(GetDesktopWindow(), &deskRc);	
}


//****************************************************************
//
//  Destructor
//
//****************************************************************
Balloon::~Balloon()
{
	UnhookWindowsHookEx(mouseHook);

	if(hPen)
		DeleteObject(hPen);

	if(hFont)
		DeleteObject(hFont);

	if(hBrush)
		DeleteObject(hBrush);

	UnregisterBalloonClass();


	if(text)
		delete[] text;

	if(border)
		delete[] border;

	if(pos)
		delete[] pos; 
};		


//**********************************************************************
//
// This controls the extra space that comes before and after the text
// in the balloon (ie. the balloon needs to be a little taller and wider
// than the text or it will look real funny.
//
// Also, this controls the relative position of the balloon to the 
// cursor.  It will be shifted a hair to the right and a little down
// from the point where the cursor was when the prompt was triggered.
// This doesn't guarantee that the balloon will always be a certain
// distance away from the cursor, because the user will likely keep
// moving the cursor and depending on which direction the cursor
// is being moved, the balloon prompt could look too close or too 
// far from the cursor.  (This is out of our control).
//
// This also controls the x and y positioning of the text in the balloon.
// The text doesn't start at the topleft corner of the balloon (or it
// would look real funny).
//
//**********************************************************************
BOOL Balloon::SetPositionProperties()
{
	BOOL	fRet = FALSE;


	if(text->bPoints)	// prevent divide by 0
	{
		text->font->bHeight = 
				(BYTE)(MulDiv((int)text->bPoints,		
					  			96, 	// @96 dpi (dots per inch)
								72));   // points per inch

		if(CreateFont())
		{
			pos->bExtraWidth  = text->font->bHeight;
			pos->bExtraHeight = text->font->bHeight * 0.60;
			pos->bXOffset     = 5;
			pos->bYOffset     = 16;
			text->bXPos		  = (BYTE)(pos->bExtraWidth / 2);
			text->bYPos		  = (BYTE)(pos->bExtraHeight / 2);
			fRet = TRUE;
		}
	}

	return fRet;
}


void Balloon::SetText(const char *szText, BOOL fUpdate)
{
	if(*szText)
	{
		strcpy(text->szBuf, szText);
		text->wLen = (WORD)(lstrlen(text->szBuf));
	}
	else
	{
		strcpy(text->szBuf,"");
		text->wLen  = 0;
	}		

	if(fUpdate)
		RedrawBalloon();
}


// Upper limit of 255 for point size
BOOL Balloon::SetPointSize(int	nPtSize, BOOL fUpdate)
{
	if(nPtSize > 255)
		return FALSE;

	text->bPoints = (BYTE)nPtSize;

	if(SetPositionProperties())
	{
		if(fUpdate)
			RedrawBalloon();
		return TRUE;
	}
	else
		return FALSE;
}


BOOL Balloon::SetTextColor(COLORREF color, BOOL fUpdate)
{
	text->color = color;

	if(fUpdate)
		RedrawBalloon();
	return TRUE;
}


BOOL Balloon::SetBackgroundColor(COLORREF color, BOOL fUpdate)
{
	text->backgroundColor = color;

	hBrush = CreateSolidBrush(text->backgroundColor);	

	if(hBrush)
	{
		if(fUpdate)
			RedrawBalloon();
	
		return TRUE;
	}
	else
		return FALSE;
}


BOOL Balloon::CreatePen()
{
	LOGPEN		lpPen;


	if(hPen)
		DeleteObject(hPen);

	// Create a pen for the balloon border
	lpPen.lopnStyle   = border->nStyle;
	lpPen.lopnWidth.x = border->bWidth;
	lpPen.lopnWidth.y = border->bWidth;
	lpPen.lopnColor   = border->color;

	hPen = CreatePenIndirect(&lpPen);

	if(hPen)
		return TRUE;
	else
		return FALSE;
}


BOOL Balloon::SetBorderColor(COLORREF color, BOOL fUpdate)
{
	border->color = color;

	if(CreatePen())
	{
		if(fUpdate)
			RedrawBalloon();
		return TRUE;
	}
	else
		return FALSE;
}


//*********************************************************
//
// PS_SOLID	
// PS_DASH		
// PS_DOT
// PS_DASHDOT
// PS_DASHDOTDOT
//
//
//*********************************************************
BOOL Balloon::SetBorderStyle(int nStyle, BOOL fUpdate)
{
	BOOL	fRet = FALSE;

	switch(nStyle)
	{
		case PS_SOLID:
		case PS_DASH:
		case PS_DOT:
		case PS_DASHDOT:
		case PS_DASHDOTDOT:
			border->nStyle = nStyle;
			if(CreatePen())
			{
				if(fUpdate)
					RedrawBalloon();
				fRet = TRUE;
			}
			break;
	}

	return (fRet);
}


// Upper width limit of 255
BOOL Balloon::SetBorderWidth(int nWidth, BOOL fUpdate)
{
	if (nWidth > 255)
		nWidth = 255;

	border->bWidth = (BYTE)nWidth;

	if(CreatePen())
	{
		if(fUpdate)
			RedrawBalloon();
		return TRUE;
	}
	else
		return FALSE;
}


//**************************************************************
//
//	Limit is 5 seconds (anything more than that is not for
// the purpose of balloon prompts)
//
//**************************************************************
BOOL Balloon::SetDelayRate(WORD wRate)
{
	if (wRate > 5000)
		return FALSE;

	wDelayRate = wRate;
	return TRUE;
}


BOOL Balloon::SetItalics(BOOL fItalic, BOOL fUpdate)
{
	text->font->fbItalic = (BYTE)fItalic;

   if(CreateFont())
	{
		if(fUpdate)
			RedrawBalloon();
		return TRUE;
	}
	else
		return FALSE;
}


BOOL Balloon::SetUnderline(BOOL fUnderline, BOOL fUpdate)
{
	text->font->fbUnderline = (BYTE)fUnderline;

	if(fUpdate)
		RedrawBalloon();

   if(CreateFont())
	{
		if(fUpdate)
			RedrawBalloon();
		return TRUE;
	}
	else
		return FALSE;
}


//****************************************************************
// 
// The range is  0 through 1000.
// 
//      700 is bold and 400 is normal
//
//****************************************************************
BOOL Balloon::SetFontWeight(int nWeight, BOOL fUpdate)
{
	if(nWeight>1000)
		return FALSE;

	if(nWeight<0)
		return FALSE;

	text->font->wWeight = (WORD)nWeight;

   if(CreateFont())
	{
		if(fUpdate)
			RedrawBalloon();
		return TRUE;
	}
	else
		return FALSE;
}


BOOL Balloon::SetFont(const char *szFaceName, BOOL fUpdate)
{
	if(!*szFaceName)
		return FALSE;

	strcpy (text->font->szFaceName, szFaceName);

   if(CreateFont())
	{
		if(fUpdate)
			RedrawBalloon();
		return TRUE;
	}
	else
		return FALSE;
}		


//*************************************************************
//
// Currently, only ten balloon clients can be registered at 
//	one time.  (This is probably more than sufficient).  
//
//*************************************************************
int Balloon::RegisterClient(HWND hwnd)
{
	if (bClientIndex + 1 < 10)
	{
		hwndClient[bClientIndex++] = hwnd;
		return TRUE;
	}
	else
		return FALSE;
}


int Balloon::UnregisterClient(HWND hwnd)
{
	for (BYTE i=0; i < bClientIndex; i++)
	{
		if (hwnd == hwndClient[i])
		{
			if(bClientIndex==0)			// Should never happen
				break;

			bClientIndex = (BYTE)(bClientIndex - 1);

			// Fill the space by shifting the remainder
			// of the array up one
			for (BYTE j=i; j < bClientIndex; j++)
				hwndClient[j] = hwndClient[j+1];

			hwndClient[bClientIndex] = 0;
			return TRUE;
		}
	}
	return FALSE;
}


BOOL Balloon::ActivatePrompts()
{
	fbShowBalloons = TRUE;
	return TRUE;
}


BOOL Balloon::SuppressPrompts()
{
	fbShowBalloons = FALSE;
	return TRUE;
}


void Balloon::PopBalloon()
{
	fbMakeBalloons = FALSE;
	KillBalloonTimer();
	SetText("", TRUE);
	ShowWindow(hwnd, SW_HIDE);
}


void Balloon::MakeBalloon()
{
	fbMakeBalloons = TRUE;
	ShowWindow(hwnd, SW_SHOW);
}


//*************************************************************
//
// The balloon client, will respond to this request by sending 
// a BALLOON_UPDATEPROMPT message.  If it doesn't respond, then 
// we never put up a prompt.  The server drives the balloon 
// prompts, but if clients don't respond they get no service.  
// (This architecture was chosen because clients know best about 
// their objects, bitmaps, what they want prompts for, and what 
// those prompts are.)
//
//*************************************************************
void Balloon::RequestPrompt(LPARAM lParam)
{
	lParam = ConvertToClientPoint(hwndFocus, lParam);
	SendMessage(hwndFocus, BALLOON_REQUESTPROMPT, (WPARAM)hwnd, lParam);
}


LPARAM Balloon::ConvertToClientPoint(HWND hwnd, LPARAM lParam)
{									 
	LPMOUSEHOOKSTRUCT 	lpMouseInfo;  
	WINDOWPLACEMENT		wndPos;
	POINT				pPoint;		  
	RECT				rc, cl;

	
	lpMouseInfo = (LPMOUSEHOOKSTRUCT)(lParam);
	pPoint.x    = lpMouseInfo->pt.x;		 
	pPoint.y    = lpMouseInfo->pt.y;		 

	// Put point in relation to the client window 
	GetWindowPlacement(hwnd, &wndPos);
	pPoint.x -= wndPos.rcNormalPosition.left;
	pPoint.y -= wndPos.rcNormalPosition.top;

	// Subtract out the height of the title bar (if exists)
	GetWindowRect(hwnd, &rc);
	GetClientRect(hwnd, &cl);
	pPoint.y -= ((rc.bottom - rc.top) - (cl.bottom - cl.top));

	// Max sure it's a positive number
	pPoint.x = max((int)pPoint.x, 0);
	pPoint.y = max((int)pPoint.y, 0);

	return (POINTTOPOINTS(pPoint));
}


void Balloon::MouseMove(LPARAM lParam)
{
	LPMOUSEHOOKSTRUCT 	lpMouseInfo;
	HWND				hwndNewFocus = NULL;
	HWND 				hHitWindow;

	
	lpMouseInfo = (LPMOUSEHOOKSTRUCT)lParam; 				
	hHitWindow  = WindowFromPoint(lpMouseInfo->pt);

	// See if mouse is over a registered balloon client window
	if (hHitWindow == hwnd)
		return;		// ignore if got over the balloon prompt itself

	if(hwnd==GetActiveWindow())
		return;

	for (int i=0; i < bClientIndex; i++)
	{
		if (hHitWindow == hwndClient[i])
		{
			if(hHitWindow == GetActiveWindow())
				hwndNewFocus = hwndClient[i];
			break;
		}
	}

	if(!hwndNewFocus)					
	{
		if(hwndFocus)
		{
			hwndFocus = NULL;
			PopBalloon();
		}
	}
	else if (hwndNewFocus!=hwndFocus)		// new window
	{
		if (hwndFocus)
			PopBalloon();

		hwndFocus = hwndNewFocus;
		RequestPrompt(lParam);
	}
	else
	{
		// Same window, but could be over a different button, bitmap,
		// or other object, so we need to keep requesting the window 
		// for new prompts.
		RequestPrompt(lParam);
	}
}


void Balloon::NCMouseMove()
{
	if(hwndFocus)
	{
		hwndFocus = NULL;
		PopBalloon();
	}
}


void Balloon::StartBalloonTimer()
{
	timerID = SetTimer(hwnd, 1, wDelayRate, NULL);
}


void Balloon::KillBalloonTimer()
{
	KillTimer(hwnd, timerID);
}


void Balloon::UpdatePrompt(WPARAM wParam)
{
	const char *szText = (char *)wParam;


	if(!szText)		// don't access a NULL pointer
		return;

	if (fbMakeBalloons)
	{
		if(*szText)
		{
			if (strcmp(text->szBuf, szText)!=0)
				SetText(szText, TRUE);
		}
		else
			SetText("", TRUE);
	}
	else
	{
		// Timer hasn't gone off yet.
		// Prevent balloons from coming up when moving quickly 					
		// over palette for an extended period.
		if (strcmp(text->szBuf, szText)!=0)
		{
			SetText(szText, FALSE);
			KillBalloonTimer();
			StartBalloonTimer();
		}
	}
}


LRESULT CALLBACK BalloonWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT		ps;
	HDC           	hdc;
	Balloon			*balloon;


	switch(uMsg)
	{
		case BALLOON_UPDATEPROMPT:
			balloon = GetBalloon(hwnd);
			if (balloon->fbShowBalloons)
				balloon->UpdatePrompt(wParam);
			break;

		case BALLOON_MOUSEMOVE:
			balloon = GetBalloon(hwnd);
			if (balloon->fbShowBalloons)
				balloon->MouseMove(lParam);
			break;

		case BALLOON_NCMOUSEMOVE:
			balloon = GetBalloon(hwnd);
			if (balloon->fbShowBalloons)
				balloon->NCMouseMove();
			break;

		case WM_PAINT:
			balloon = GetBalloon(hwnd);
			if (balloon->fbShowBalloons)
			{
				hdc = BeginPaint(hwnd, &ps);

				if (hdc)
					balloon->Paint(hdc);
			
			 	EndPaint(hwnd, &ps);
			}
			break;

		case WM_TIMER:
			balloon = GetBalloon(hwnd);
			if (balloon->fbShowBalloons)
			{
				balloon->MakeBalloon();
				balloon->KillBalloonTimer();
			}
			break;

		case WM_DESTROY:
			balloon = GetBalloon(hwnd);

			// Don't use delete[] so we can allow the destructor
			// to get at it's data and free up what it needs to
			if(balloon)
				delete balloon;	
			break;
	}

	return (DefWindowProc(hwnd, uMsg, wParam, lParam));
}


/****************************************************************
  WH_MOUSE hook procedure

 ****************************************************************/
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0)  	// do not process the message 
        return CallNextHookEx(mouseHook, nCode, wParam, lParam);

	if (wParam == WM_MOUSEMOVE)
		SendMessage (hwndBalloon, BALLOON_MOUSEMOVE, wParam, lParam);

	else if (wParam = WM_NCMOUSEMOVE, wParam, lParam)
		SendMessage (hwndBalloon, BALLOON_NCMOUSEMOVE, wParam, lParam);

	return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}


void Balloon::RedrawBalloon()
{
	RECT	rc;
	GetClientRect(hwnd, &rc);
	InvalidateRect(hwnd, &rc, TRUE);
}


BOOL Balloon::CreateFont()
{
	LOGFONT     lfFont;
	BOOL		fRet = FALSE;


	if(hFont)
		DeleteObject(hFont);

	// Create the font for the balloon text
	lfFont.lfHeight         = text->font->bHeight;
	lfFont.lfWidth          = 0;
	lfFont.lfEscapement     = 0;
	lfFont.lfOrientation    = 0;
	lfFont.lfWeight         = text->font->wWeight;		//400 is normal
	lfFont.lfItalic         = text->font->fbItalic;
	lfFont.lfUnderline      = text->font->fbUnderline;
	lfFont.lfStrikeOut      = 0;
	lfFont.lfCharSet        = ANSI_CHARSET;
	lfFont.lfOutPrecision   = OUT_DEFAULT_PRECIS;
	lfFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
	lfFont.lfQuality        = DEFAULT_QUALITY;
	lfFont.lfPitchAndFamily = (DEFAULT_PITCH | FF_SWISS);

	if(*text->font->szFaceName)
		lstrcpyn(lfFont.lfFaceName, text->font->szFaceName, LF_FACESIZE);
	else
		lstrcpyn(lfFont.lfFaceName, szDefaultFont, LF_FACESIZE);

	hFont = CreateFontIndirect(&lfFont);

	if(hFont)
		fRet = TRUE;
	else
	{
		lstrcpyn(lfFont.lfFaceName, szDefaultFont, LF_FACESIZE);
		hFont = CreateFontIndirect(&lfFont);

		if(hFont)
			fRet = TRUE;
	}
	
	return fRet;
}


BOOL Balloon::Init(void)
{
	// Text
	SetText("", FALSE);

	if(!SetPointSize(DEFAULT_POINTSIZE, FALSE))
		return FALSE;

	if(!SetTextColor(DEFAULT_TEXTCOLOR, FALSE))	 
		return FALSE;

	if(!SetBackgroundColor(DEFAULT_BKCOLOR, FALSE))	
		return FALSE;

	// Font
	if(!SetItalics(FALSE, FALSE))
		return FALSE;

	if(!SetUnderline(FALSE, FALSE))
		return FALSE;

	if(!SetFontWeight(DEFAULT_WEIGHT, FALSE))					   
		return FALSE;

	if(!SetFont(szDefaultFont, FALSE))
		return FALSE;

	// Border
	if(!SetBorderColor(DEFAULT_BRDCOLOR, FALSE))
		return FALSE;

	if(!SetBorderStyle(PS_SOLID, FALSE))
		return FALSE;

	if(!SetBorderWidth(DEFAULT_BRDWIDTH, TRUE))
		return FALSE;

	if(!ActivatePrompts())
		return FALSE;

	if(!SetDelayRate(DEFAULT_DELAY)) 
		return FALSE;

	return TRUE;
}


void Balloon::DrawBorder(HDC hdc)
{
	RECT	rc;

	SelectObject (hdc, hPen);
	GetClientRect(hwnd, &rc);

	MoveToEx(hdc, rc.left, rc.top, NULL);
	LineTo(hdc, rc.right-1, rc.top);
	LineTo(hdc, rc.right-1, rc.bottom-1);
	LineTo(hdc, rc.left, rc.bottom-1);
	LineTo(hdc, rc.left, rc.top);
}


void Balloon::Paint(HDC hdc)
{
	if(*text->szBuf && fbMakeBalloons)
	{
		SIZE	size;
		HRGN	hRgn;
		RECT	rc;

		SelectObject(hdc, hFont);
		SetBkMode(hdc, TRANSPARENT);
		GetTextExtentPoint32(hdc, text->szBuf, text->wLen, &size); 

/*
		if(hBrush)
		{
			GetClientRect(hwnd, &rc);
			hRgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
			FillRgn(hdc, hRgn, hBrush);
		}
*/

		MoveWindow(size);

		// Paint background color
		if(hBrush)
		{
			GetClientRect(hwnd, &rc);
			hRgn = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);
			FillRgn(hdc, hRgn, hBrush);
			DeleteObject(hRgn);
		}

		::SetTextColor(hdc, text->color);
		TextOut(hdc, text->bXPos, text->bYPos, text->szBuf, text->wLen);
		DrawBorder(hdc);

		SetActiveWindow(hwndFocus);
	}
}


void Balloon::MoveWindow(SIZE size)
{
	POINT	pPoint;
	int		x, y;
	int	 	width, height;


	GetCursorPos(&pPoint);

	x = pPoint.x + pos->bXOffset;
	y = pPoint.y + pos->bYOffset;
	width = size.cx  + (text->font->fbItalic ? (pos->bExtraWidth*1.5) : (pos->bExtraWidth));
	height = size.cy  + pos->bExtraHeight;

	// Make sure balloon prompt is fully visible 
	// within the screen and doen't get clipped
	if (x < 0)
		x = 0;
	else if (x+width > deskRc.right)
		x = deskRc.right - width;

	if (y < 0)
		y = 0;
	else if (y+height > deskRc.bottom)
		y = deskRc.bottom - height;
	

/*	SetWindowPos(hwnd, 
					HWND_TOP, 
					x, y, width, height, 
					SWP_SHOWWINDOW);

	GetClientRect(hwnd, &rc);
	InvalidateRect(hwnd, &rc, TRUE);
*/

	::MoveWindow(hwnd, x, y, width, height, TRUE);

}


/*
void Balloon::Move(HWND hwndClient, POINT  pMove)
{
	RECT	rc;
	int		x;
	int		y;


	if (!IsWindowVisible(hwnd))
		ShowWindow(hwnd, SW_SHOW);

	GetClientRect(hwndClient, &rc);


	// Position in relation to the client window's position
	x = rc.left + pMove.x;		
	y = rc.top  + pMove.y;

	GetClientRect(GetDesktopWindow(), &rc);	

	// Make sure balloon prompt is fully visible 
	// within the screen and doen't get clipped
	if (x < 0)
		x = 0;
	else if (x+width > rc.right)
		x = rc.right - width;

	if (y < 0)
		y = 0;
	else if (y+height > rc.bottom)
		y = rc.bottom - height;
	
	SetWindowPos(hwnd, 
					HWND_TOP, 
					x, y, width, height, 
					SWP_SHOWWINDOW);

	GetClientRect(hwnd, &rc);
	InvalidateRect(hwnd, &rc, TRUE);
}
*/

