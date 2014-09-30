////////////////////////////////////////////////////////
// File Name: SERDLG.CPP
//		Copyright 1989-92 Serius Corporation
// Author:	Paul Ruben
// Date: 5/4/92
// 		 8/20/92	DrawTitle added
// Description:
///////////////////////////////////////////////////////////

#define STRICT
#include <windows.h>
#include <string.h>

#include "almsys.h"
#include "system.h"
#include "exeinfo.h"
#include "oiputils.h"

////////////////

#define	POINT_SIZE	18

//////////////// Forward declarations

LRESULT I_CALLBACK		SetFontSubclass(WNDPROC, HWND, UINT, WPARAM, LPARAM);

#define WNDPROCREF(n)				FontSubclass##n
#define WNDPROCFORWARD(n)		LRESULT I_CALLBACK FontSubclass##n(HWND, UINT, WPARAM, LPARAM);

#define WNDPROCBODY(n)																						\
														LRESULT I_CALLBACK 	FontSubclass##n(	\
															HWND								hWnd,										\
															UINT								uMsg,										\
															WPARAM							wParam,									\
															LPARAM							lParam									\
															)																						\
															{																						\
															return SetFontSubclass(g_pOldProcs[n], hWnd, uMsg, wParam, lParam);	\
															}

WNDPROCFORWARD(0);
WNDPROCFORWARD(1);
WNDPROCFORWARD(2);
WNDPROCFORWARD(3);
WNDPROCFORWARD(4);
WNDPROCFORWARD(5);
WNDPROCFORWARD(6);
WNDPROCFORWARD(7);

//////////////// Static variables

static int					g_nFontUseCount;
static HFONT				g_hTitleFont;

const int						g_nMaxSubclasses								= 10;

static WNDPROC			g_pOldProcs[g_nMaxSubclasses];
static WNDPROC			g_pWndProcs[g_nMaxSubclasses]		= {
																												WNDPROCREF(0),
																												WNDPROCREF(1),
																												WNDPROCREF(2),
																												WNDPROCREF(3),
																												WNDPROCREF(4),
																												WNDPROCREF(5),
																												WNDPROCREF(6),
																												WNDPROCREF(7)
																											};

WNDPROCBODY(0);
WNDPROCBODY(1);
WNDPROCBODY(2);
WNDPROCBODY(3);
WNDPROCBODY(4);
WNDPROCBODY(5);
WNDPROCBODY(6);
WNDPROCBODY(7);

////////////////////////////////////////////////////////////////
//
// FUNCTION:
//
////////////////////////////////////////////////////////////////

LRESULT I_CALLBACK	SetFontSubclass(
	WNDPROC							oldWndProc,
	HWND								hWnd,
	UINT								uMsg,
	WPARAM							wParam,
	LPARAM							lParam
	)
	{
	if (uMsg == WM_DESTROY)
		{
		g_nFontUseCount--;
		if (g_nFontUseCount == 0)
			{
			DeleteObject(g_hTitleFont);
			g_hTitleFont = NULL;
			}
		}

	return CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
	}

////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////

extern "C"
void ALMAPI		AUtlSetTitleFont(
	TYPEID					tidType,
	LPSTR						pszText,
	HWND						hCtl
	)
	{
	BOOL					fSetData				= TRUE;
	HDC						hdc;
	LOGFONT				lf;
	HFONT					hFont;
	HFONT					hOldFont;
	char					szText[128];
	DWORD					dwSize;
	RECT					rcItem;

	ADBG_ASSERT_W(hCtl != NULL, return, "NULL hWnd passed to AUtlSetTitleFont");

	hdc = GetDC(NULL);

	if (g_nFontUseCount == 0)
		{
		lf.lfHeight			    = - MulDiv(POINT_SIZE, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		lf.lfWidth			    = 0;		// use the default for proper aspect ratio
		lf.lfEscapement	    = 0;		// use standard location
		lf.lfOrientation    = 0;		// not used
		lf.lfWeight			    = FW_BOLD;	// bold font desired
		lf.lfItalic			    = 1;		// italics desired
		lf.lfUnderline      = 0;		// not underlined
		lf.lfStrikeOut      = 0;		// not struckout
		lf.lfCharSet        = ANSI_CHARSET;
		lf.lfOutPrecision   = OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
		lf.lfQuality        = PROOF_QUALITY;
		lf.lfPitchAndFamily	= DEFAULT_PITCH | FF_SWISS;

		lstrcpy(lf.lfFaceName, "Arial Bold Italic");

		g_hTitleFont = CreateFontIndirect(&lf);
		}

	if (g_nFontUseCount < g_nMaxSubclasses)
		{
		g_nFontUseCount++;

		SendMessage(hCtl, WM_SETFONT, (UINT) g_hTitleFont, 0);

		// Get the font just in case the create failed
		hFont = (HFONT) SendMessage(hCtl, WM_GETFONT, 0, 0);

		if (pszText)
			strncpy(szText, pszText, sizeof(szText));
		else if (tidType != 0)
			ATypeGetName(tidType, szText, sizeof(szText));
		else
			fSetData = FALSE;

		if (fSetData)
			{
			hOldFont = (HFONT) SelectObject(hdc, hFont);

			// HACK HACK!!!
			// Change only the height so the width will look good in Borland-like
			// dialogs
			GetClientRect(hCtl, &rcItem);
			dwSize = GetTextExtent(hdc, szText, strlen(szText));
			SetWindowPos(hCtl, NULL, 0, 0, rcItem.right, HIWORD(dwSize),
										SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

			SelectObject(hdc, hOldFont);

			SetWindowText(hCtl, szText);
			}

		// Now subclass the window in order to look for WM_DESTROY
		g_pOldProcs[g_nFontUseCount - 1] = (WNDPROC) GetWindowLong(hCtl, GWL_WNDPROC);
		SetWindowLong(hCtl, GWL_WNDPROC, (LONG) g_pWndProcs[g_nFontUseCount - 1]);
		}

	ReleaseDC(NULL, hdc);
	}

////////////////////////////////////////////////////////////////
//
// FUNC:  AUtlCenterDialog
//
////////////////////////////////////////////////////////////////

extern "C"
void ALMAPI		AUtlCenterDialog(
	HWND					hDlg,
	UINT					nReserved			// must be zero (later:  l/r,t/b options)
	)
	{
	if (hDlg != NULL)
		{
		EXEINFOAUX		auxInfo;
		EXEINFO				exeInfo(&auxInfo);
		LPAPPCONTEXT	pContext;

		pContext = pTheSystem->GetCurrentContext();

		// Because of the non-protection of pTheSystem from no contexts being
		// registered, we need to verify that we have a context before we
		// ask the system if it is runmode.
		if (pContext != NULL &&							// == NULL is possible if called from ALMBLDR.EXE
				!pTheSystem->IsExeStructures())	// don't do anything at runtime
			{
			pContext->GetCopyOfExeInfo(&exeInfo);

			// Passing in NULL for "auxInfo.hVABFrame" is okay, since it will
			// center over the active window.  This will happen at breakpoints
			// in VAB application.

			// Until we allow centering options, we'll just use internal centering
			// routine.
			CenterDialog(hDlg, auxInfo.hVABFrame, FALSE);
			}
		}
	}

