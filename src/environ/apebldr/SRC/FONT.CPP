#include "almsys.h"
#include <windowsx.h>

#include "font.h"
#include "progsys.h"
#include "oiputils.h"

//font size should be maintained in 72nds of an inch

#define FONT_SAMPLE 2000 //id of owner draw button

FONT::FONT(
	HWND owner,
	LPLOGFONT lfInit)
	{
	HDC hdc;
	cf.Flags = (DWORD)0;
	if(lfInit != NULL)
		{
		hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
		lf = *lfInit;
		lf.lfHeight = Y72ndsToPixels(hdc, lf.lfHeight);
		lf.lfWidth = X72ndsToPixels(hdc, lf.lfWidth);
		cf.Flags|=CF_INITTOLOGFONTSTRUCT;
		DeleteDC(hdc);
		}

	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.hwndOwner = owner;
	cf.hDC = NULL;
	cf.lpLogFont = (LPLOGFONT)&lf;
	cf.Flags|=(CF_BOTH|CF_EFFECTS|CF_ENABLETEMPLATE|CF_ENABLEHOOK);
	cf.lCustData = NULL;
	cf.lpfnHook = FONT::ChooseFontHook;
	cf.lpTemplateName = (LPSTR)"FONTDLG";
	cf.hInstance = PROGSYSTEM::hInst;
	cf.lpszStyle = NULL;
	cf.nFontType = NULL;
	cf.nSizeMin = NULL;
	cf.nSizeMax = NULL;
	}

/////////////////////////////////////////////////////////////
FONT::~FONT()
	{
	}

/////////////////////////////////////////////////////////////
BOOL FONT::ChooseFont(
	LPLOGFONT lfResult)
	{
	BOOL result;
	HDC hdc;

	result = ::ChooseFont((LPCHOOSEFONT)&cf);
	if(result)
		{
		*lfResult = *(cf.lpLogFont);
		hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
		lfResult->lfHeight = YPixelsTo72nds(hdc, lfResult->lfHeight);
		lfResult->lfWidth = XPixelsTo72nds(hdc, lfResult->lfWidth);
		DeleteDC(hdc);
		}
	return result;
	}

/////////////////////////////////////////////////////////////
UINT CALLBACK FONT::ChooseFontHook(
	HWND    hWnd,
	UINT    msg,
	WPARAM  wParam,
	LPARAM  lParam)
	{
	LPDRAWITEMSTRUCT di;

	switch(msg)
		{
		case WM_INITDIALOG:
			CenterDialog(hWnd, PROGSYSTEM::hWndFrame, FALSE);
			return TRUE;

		case WM_COMMAND:
			{
			WORD	wNotify	= GET_WM_COMMAND_CMD(wParam, lParam);

			if (wNotify == BN_CLICKED ||
				wNotify == CBN_SELCHANGE || wNotify == CBN_KILLFOCUS)
				{
				InvalidateRect(GetDlgItem(hWnd, FONT_SAMPLE), NULL, TRUE);
				}
			break;
			}

		case WM_DRAWITEM:
			di = (LPDRAWITEMSTRUCT)lParam;
			switch(di->CtlID)
				{
				case FONT_SAMPLE:
					DrawSampleText(hWnd, di);
					return TRUE;
				}
			break;

		}
	return FALSE;
	}


/////////////////////////////////////////////////////////////
VOID FONT::DrawSampleText(
	HWND								hWnd,
	LPDRAWITEMSTRUCT		di)
	{
	int								oldMode;
	LOGFONT						lf;
	HFONT							hFont, hFontOld;
	char							sampleText[51];
	COLORREF					oldColor;

	SendMessage(hWnd, WM_CHOOSEFONT_GETLOGFONT, 0, (LONG)&lf);
	hFont = CreateFontIndirect(&lf);

	hFontOld = SelectFont(di->hDC, hFont);
	oldColor = SetTextColor(di->hDC, GetSysColor(COLOR_WINDOWTEXT));
	oldMode = SetBkMode(di->hDC, TRANSPARENT);
	GetDlgItemText(hWnd, FONT_SAMPLE, sampleText, 50);
	DrawText(di->hDC, sampleText, lstrlen((LPSTR)sampleText),
		&di->rcItem, DT_CENTER|DT_VCENTER|DT_NOPREFIX);
	SelectObject(di->hDC, hFontOld);
	SetTextColor(di->hDC, oldColor);
	SetBkMode(di->hDC, oldMode);

	DeleteFont(hFont);
	}
