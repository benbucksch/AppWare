////////////////////////////////////////////////////////////////////////////
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
// FILE:          "server.h"
//
// AUTHOR:        Lee E. Thayer
//
// DESCRIPTION:   Class definition for balloon server
//
////////////////////////////////////////////////////////////////////////////

#ifndef _SERVER_H
#define _SERVER_H


#define BALLOON_MOUSEMOVE     	(WM_USER + 201)
#define BALLOON_NCMOUSEMOVE   	(WM_USER + 202)


// These following values are based off values set to
// bTextPoints.  These are not to be accessed directly.
struct BLN_POS {
	BYTE		bExtraWidth;
	BYTE		bExtraHeight;
	BYTE		bXOffset;
	BYTE		bYOffset;
};

struct BLN_FONT {
	char		szFaceName[256];
	BYTE		bHeight;
	BYTE		fbItalic;
	BYTE		fbUnderline;
	WORD		wWeight;
};

struct BLN_BORDER {
	COLORREF	color;
	int			nStyle;			// PS_SOLID, PS_DASH, PS_DOT, PS_DASHDOT
	BYTE		bWidth;
};

struct BLN_TEXT {
	char		szBuf[128];		// Text in balloon
	WORD		wLen;			// Length of text in ballon (figure once to save cycles)
	BYTE		bPoints;
	COLORREF	color;
	COLORREF	backgroundColor;
	BYTE		bXPos;
	BYTE		bYPos;
	BLN_FONT	*font;
};


class Balloon 
{
	public:
	   	HWND	hwnd;			// Balloon's handle
		BYTE	fbShowBalloons;	// FALSE = Prompts will be suppressed

	private:
		BLN_TEXT	*text;
		BLN_BORDER	*border;
		BLN_POS		*pos;
		HBRUSH		hBrush;		// For painting background
		HWND		hwndClient[10];	// Client windows that can support prompts
		HWND		hwndFocus;		// Window over which the mouse is
		HPEN		hPen;			// For drawing black border around balloon
		HFONT		hFont;			// For drawing text in balloon
		UINT		timerID;
		WORD		wDelayRate;		// time required for prompt to come up
		BYTE		bClientIndex;	// Number of registered client windows
		BYTE   		fbMakeBalloons;	// TRUE = Timer has gone off, display balloon
		RECT		deskRc;		    // coordinates of desktop window

	public:
		Balloon();		// constructor
		~Balloon();		// destructor

		BOOL Init();

		int RegisterClient(HWND hwnd);
		int UnregisterClient(HWND hwnd);

		void UpdatePrompt(WPARAM wParam);
		void MouseMove(LPARAM lParam);
		void NCMouseMove();
		void MakeBalloon();
		void KillBalloonTimer();
		void Paint(HDC hdc);
		void SetText(const char *szNewText, BOOL fUpdate);

		BOOL ActivatePrompts();
		BOOL SuppressPrompts();
		BOOL SetPointSize(int nPtSize, BOOL fUpdate);
		BOOL SetItalics(BOOL fItalic, BOOL fUpdate);
		BOOL SetUnderline(BOOL fUnderline, BOOL fUpdate);
		BOOL SetFontWeight(int nWeight, BOOL fUpdate);		// 0 through 1000, 700 is bold, 400 is normal
		BOOL SetFont(const char *szFaceName, BOOL fUpdate);
		BOOL SetTextColor(COLORREF color, BOOL fUpdate);
		BOOL SetBackgroundColor(COLORREF color, BOOL fUpdate);
		BOOL SetBorderColor(COLORREF color, BOOL fUpdate);
		BOOL SetBorderStyle(int nStyle, BOOL fUpdate);
		BOOL SetBorderWidth(int nWidth, BOOL fUpdate);
		BOOL SetDelayRate(WORD wRate);

	private:
		void MoveWindow(SIZE size);
		BOOL CreatePen();
		BOOL CreateFont();
		BOOL SetPositionProperties();

		void PopBalloon();
		void StartBalloonTimer();

		void RequestPrompt(LPARAM lParam);

		void RedrawBalloon();
		void DrawBorder(HDC hdc);

		LPARAM ConvertToClientPoint(HWND hwnd, LPARAM lParam);
};

// Prototypes
LRESULT CALLBACK BalloonWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#endif
