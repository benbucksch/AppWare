#include "almsys.h"
#include <windowsx.h>

#include	"printer.h"
#include  "progsys.h"
#include "font.h"
#include "resconst.h"
#include "oiputils.h"

#define PRT_TAB 2

#define PRT_FONTSBUTTON 1026 	//must coincide with the fonts button
															//identifier in printdlg.h

typedef UINT ((CALLBACK* PRINT_CALLBACK)(HWND, UINT, WPARAM, LPARAM));

BOOL PRINTER::continueOK;
HWND PRINTER::abortDlg = NULL;
HANDLE	PRINTER::hDevMode=NULL;
HANDLE	PRINTER::hDevNames=NULL;
BOOL    PRINTER::createFont = FALSE;
LOGFONT PRINTER::lf;
BOOL   PRINTER::collate = FALSE;

////////////////////////////////////////////////////////////////////////////

PRINTER::PRINTER(
	HWND hWnd)

	{
	hWndOwner = hWnd;

	pd.lStructSize		= sizeof(PRINTDLG);
	pd.hwndOwner			= hWndOwner;
	pd.hDevMode = hDevMode;
	pd.hDevNames = hDevNames;
	pd.hDC			= NULL;
	pd.Flags			= PD_RETURNDC|PD_ALLPAGES|PD_ENABLEPRINTHOOK|
		PD_NOPAGENUMS | PD_NOSELECTION | PD_ENABLEPRINTTEMPLATE |
		PD_USEDEVMODECOPIES;
	if(collate)
		pd.Flags|=PD_COLLATE;
	pd.nFromPage			= -1;
	pd.nToPage			= -1;
	pd.nMinPage			= 0;
	pd.nMaxPage			= 9999;
	pd.nCopies			= 1;
	pd.hInstance			= PROGSYSTEM::hInst;
	pd.lCustData			= 0L;
	pd.lpfnPrintHook		= PrintHook;
	pd.lpfnSetupHook		= NULL;
	pd.lpPrintTemplateName	= "PRINTERDLG";
	pd.lpSetupTemplateName	= NULL;
	pd.hPrintTemplate		= NULL;
	pd.hSetupTemplate		= NULL;
}

////////////////////////////////////////////////////////////////////////////

PRINTER::~PRINTER()
	{
	if(PRINTER::abortDlg != NULL)
		{
		DestroyWindow(PRINTER::abortDlg);
		PRINTER::abortDlg = NULL;
		}
	if(pd.Flags & PD_RETURNDC)
		DeleteDC();
	if(font)
		DeleteObject(font);
	}


BOOL PRINTER::PrintDlg()
	{
	BOOL result;
	int oldHeight;
	int oldWidth;
	int actualCopies;
	int numCopies;
	LPDEVMODE devModePtr;

	result = ::PrintDlg(&pd);
	PRINTER::abortDlg = NULL;
	if(result != 0)
		{
		hDevMode = pd.hDevMode;
		hDevNames = pd.hDevNames;
		if(createFont)
			{
			oldHeight = PRINTER::lf.lfHeight;
			oldWidth = PRINTER::lf.lfWidth;
			PRINTER::lf.lfHeight = FONT::Y72ndsToPixels(pd.hDC, oldHeight);
			PRINTER::lf.lfWidth = FONT::X72ndsToPixels(pd.hDC, oldWidth);
			if(PRINTER::lf.lfHeight>0)
				PRINTER::lf.lfHeight = -PRINTER::lf.lfHeight;
			font = CreateFontIndirect((LPLOGFONT)&PRINTER::lf);
			PRINTER::lf.lfHeight = oldHeight;
			PRINTER::lf.lfWidth = oldWidth;
			}
		else
			{
			font = NULL;
			}
		if(hDevMode)
			{
			devModePtr = (LPDEVMODE)GlobalLock(pd.hDevMode);
			if(pd.Flags & PD_COLLATE)
				collate = TRUE;
			else
				collate = FALSE;
			if((pd.Flags & PD_COLLATE) && pd.nCopies == 1 &&
				devModePtr->dmCopies > 1)
				{
				//since we need to collate, set # copies to 1
				numCopies = 1;
				Escape(pd.hDC, SETCOPYCOUNT, sizeof(int), (LPSTR)&numCopies,
					(LPSTR)&actualCopies);
				}

			GlobalUnlock(pd.hDevMode);
			}
		CalculatePageSize();
		}
	return result;
	}

BOOL PRINTER::SetUp()
	{
	BOOL result;
	pd.Flags = PD_PRINTSETUP;
	result = ::PrintDlg(&pd);
	if(result)
		{
		hDevMode = pd.hDevMode;
		hDevNames = pd.hDevNames;
		}
	return result;
	}

////////////////////////////////////////////////////////////////////////////
#pragma argsused        // disable warning:  "Parameter '' never used"
UINT CALLBACK PRINTER::PrintHook(
	HWND		hDlg,
	UINT		message,
	WPARAM	wParam,
	LPARAM	lParam
	)

{
	switch(message)
		{
		case WM_INITDIALOG:
			CenterDialog(hDlg, PROGSYSTEM::hWndFrame, FALSE);
			return TRUE;

		case WM_COMMAND:
			{
			WORD		wID		= GET_WM_COMMAND_ID(wParam, lParam);

			if(wID == PRT_FONTSBUTTON)
				{
				LPFONT fontPtr;
				if(PRINTER::createFont)
					fontPtr = new FONT(hDlg, &PRINTER::lf);
				else
					fontPtr = new FONT(hDlg, NULL);
				if(fontPtr->ChooseFont(&PRINTER::lf))
					PRINTER::createFont = TRUE;
				delete fontPtr;
				return TRUE;
				}
			break;
			}
		}

	return FALSE;
}

void PRINTER::CalculatePageSize()
	{
	TEXTMETRIC tm;

	Escape(pd.hDC, GETPHYSPAGESIZE, NULL, NULL, (LPSTR)&pageSize);
	Escape(pd.hDC, GETPRINTINGOFFSET, NULL, NULL, (LPSTR)&offset);

	bounds.left = 0;
	bounds.top = 0;
	bounds.right = GetDeviceCaps(pd.hDC, HORZRES);
	bounds.bottom = GetDeviceCaps(pd.hDC, VERTRES);

	if(font)
		SelectObject(pd.hDC, font);
	GetTextMetrics(pd.hDC, (LPTEXTMETRIC)&tm);
	fontHeight = tm.tmHeight;
	tabPixels = PRT_TAB * tm.tmAveCharWidth;
	numLines = (pageSize.y - 2*offset.y) / fontHeight;
	}

BOOL PRINTER::StartJob(
	LPCSTR jobName)
	{
	currentLine = 0;
	PRINTER::continueOK = TRUE;

	if(PRINTER::abortDlg == NULL)
		{
		PRINTER::abortDlg = CreateDialogParam(PROGSYSTEM::hInst, "PRINTABORT",
			hWndOwner, (DLGPROC)PRINTER::AbortDlgProc, (LONG)jobName);
		}
	if(Escape(pd.hDC, SETABORTPROC, NULL, (LPSTR)PRINTER::AbortProc, NULL) <= 0)
		return FALSE;

	if(Escape(pd.hDC, STARTDOC, lstrlen(jobName), jobName, NULL) <= 0)
		return FALSE;

	if(font)
		SelectObject(pd.hDC, font);
	pageNum = 0;

	return TRUE;
	}

void PRINTER::EndJob()
	{
	if(currentLine != 0)
		{
		if(pageNum != 0)
			PrintPageNum();
		Escape(pd.hDC, NEWFRAME, NULL, NULL, NULL);
		}
	Escape(pd.hDC, ENDDOC, NULL, NULL, NULL);
	if(PRINTER::abortDlg != NULL)
		{
		DestroyWindow(PRINTER::abortDlg);
		PRINTER::abortDlg = NULL;
		}
	}

void PRINTER::PrintLine(
	int indent,
	LPCSTR string)
	{
	SetTextAlign(pd.hDC, TA_LEFT|TA_TOP);
	TextOut(pd.hDC, indent * tabPixels,
		currentLine * fontHeight, string, lstrlen(string));
	SkipLine();
	}

void PRINTER::CenterLine(
	LPCSTR string)
	{
	SetTextAlign(pd.hDC, TA_CENTER|TA_TOP);
	TextOut(pd.hDC, (pageSize.x - 2*offset.x)/2,
		currentLine * fontHeight, string, lstrlen(string));
	SkipLine();
	}

void PRINTER::PrintPageNum()
	{
	char buffer[16];

	pageNum++;
	wsprintf((LPSTR)buffer, "Page %i", pageNum);
	SetTextAlign(pd.hDC, TA_CENTER|TA_TOP);
	TextOut(pd.hDC, (pageSize.x - 2*offset.x)/2,
		(numLines - 1) * fontHeight, (LPSTR)buffer, lstrlen((LPSTR)buffer));
	}

void PRINTER::DoubleLine(
	int indent)
	{
	LPSTR buffer;
	int currentIndex = 1;
	int stopLine = pageSize.x - (2 * offset.x) - (indent * tabPixels);
	int size;
	SIZE	textSize;

	//allocate an extra arbitrary # of bytes (8) for security
	GetTextExtentPoint32(pd.hDC, "=", 1, &textSize);

	size = stopLine / textSize.cx + 8;
	buffer = (LPSTR) GlobalAlloc(GPTR, size);
	if (buffer == NULL)
		return;

	buffer[0] = '=';
	buffer[1] = '\0';
	while(GetTextExtentPoint32(pd.hDC, buffer, lstrlen(buffer), &textSize),
		textSize.cx < stopLine && currentIndex < size-1)
		{
		buffer[currentIndex] = '=';
		currentIndex++;
		buffer[currentIndex] = '\0';
		}
	PrintLine(indent, buffer);
	GlobalFree((HGLOBAL) buffer);
	}

void PRINTER::SingleLine(
	int indent)
	{
	LPSTR buffer;
	int currentIndex = 1;
	int stopLine = 2 * (pageSize.x - (2 * offset.x))/3 - (indent * tabPixels);
	int size;
	SIZE	textSize;

	//allocate an extra arbitrary # of bytes (8) for security
	GetTextExtentPoint32(pd.hDC, "-", 1, &textSize);

	size = stopLine / textSize.cx + 8;

	buffer = (LPSTR) GlobalAlloc(GPTR, size);
	if (buffer == NULL)
		return;

	buffer[0] = '-';
	buffer[1] = '\0';

	while(GetTextExtentPoint32(pd.hDC, buffer, lstrlen(buffer), &textSize),
		textSize.cx < stopLine && currentIndex < size-1)
		{
		buffer[currentIndex] = '-';
		currentIndex++;
		buffer[currentIndex] = '\0';
		}

	PrintLine(indent, buffer);
	GlobalFree((LPSTR) buffer);
	}

void PRINTER::SkipLine()
	{
	currentLine++;
	int result;

	if(currentLine == numLines-2)
		{
		PrintPageNum();
		currentLine = 0;
		SelectObject(pd.hDC, GetStockObject(BLACK_BRUSH));
		result = Escape(pd.hDC, NEWFRAME, NULL, NULL, NULL);
		if(result <= 0)
			{
			if(result == SP_ERROR || result == SP_OUTOFDISK ||
				result == SP_OUTOFMEMORY)
				{
				MessageBox(PROGSYSTEM::hWndFrame,
					"Aborting print job.", "Printer Error", MB_ICONEXCLAMATION|
						MB_OK);
				Escape(pd.hDC, ABORTDOC,NULL,NULL,NULL);
				}
			}
		if(font)
			SelectObject(pd.hDC, font);
		}
	}

void PRINTER::PrintLabel(
	int indent,
	int label)
	{
	char string[6];

	wsprintf((LPSTR)string, "%i.", label);
	SetTextAlign(pd.hDC, TA_LEFT|TA_TOP);
	TextOut(pd.hDC, indent * tabPixels,
		currentLine * fontHeight, string, lstrlen(string));
	}

BOOL CALLBACK PRINTER::AbortDlgProc(
	HWND hdlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	LPSTR buffer;
	LPDEVNAMES dvPtr;
	LPSTR dataPtr;

	switch(message)
		{
		case WM_INITDIALOG:
			CenterDialog(hdlg, PROGSYSTEM::hWndFrame, FALSE);
			dvPtr = (LPDEVNAMES)GlobalLock(hDevNames);

			buffer = (LPSTR) GlobalAlloc(GPTR, dvPtr->wOutputOffset + 16);
			dataPtr = (LPSTR) dvPtr;

			wsprintf(buffer, "%s on port %s",
				(dataPtr + dvPtr->wDeviceOffset),
				(dataPtr + dvPtr->wOutputOffset));

			SetDlgItemText(hdlg, PRT_JOB, (LPSTR)lParam);
			SetDlgItemText(hdlg, PRT_PRINTER, buffer);
			UpdateWindow(GetDlgItem(hdlg, PRT_JOB));
			UpdateWindow(GetDlgItem(hdlg, PRT_PRINTER));
			GlobalUnlock(hDevNames);
			GlobalFree((HGLOBAL) buffer);
			break;

		case WM_COMMAND:
			{
			WORD	wID	= GET_WM_COMMAND_ID(wParam, lParam);

			if(wID == IDCANCEL)
				PRINTER::continueOK = FALSE;
			return TRUE;
			}
		}
	return FALSE;
	}

BOOL CALLBACK PRINTER::AbortProc(
	HDC   /*hdc*/,
	short /*code*/)
	{
	MSG msg;

	while(PRINTER::continueOK && PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
		if(!IsDialogMessage(PRINTER::abortDlg, &msg))
			{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			}
		}
	return PRINTER::continueOK;
	}

void PRINTER::PrinterCleanUp()
	{
	if(hDevMode != NULL)
		GlobalFree(hDevMode);
	if(hDevNames != NULL)
		GlobalFree(hDevNames);
	}

int PRINTER::GetNumCopies()
	{
	LPDEVMODE devModePtr;
	int result = 1;

	if(hDevMode)
		{
		if((pd.Flags & PD_COLLATE))
			{
			devModePtr = (LPDEVMODE)GlobalLock(pd.hDevMode);
			result = devModePtr->dmCopies;
			GlobalUnlock(pd.hDevMode);
			}
		}
	return result;
	}
