/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define		STRICT
#define		ASTRICT
#define   ICONEDIT_MODULE  1  

#include <windows.h>
#include <string.h>
#include "resconst.h"
#include "position.h"
#include "iconedit.h"
#include "colorpal.h"
#include "winlist.h"
#include "a_alm.h"
#include "cursor.h"
#include "cursordl.h"
#include "commdlg.h"
#include "complex.h"


#define  ICONEDITORWNDCLASS	"SERIUS_ICON_EDITOR"

#define IDC_RIGHTBUTTON	10
#define IDC_LEFTBUTTON	20
#define FRAMELEFT PIXELWNDLEFT+PIXELWNDWIDTH-xCtrlExt+18

extern void FlipIcon(SERIUSICON *);
extern int iScanLineBytes;
extern int mScanLineBytes;
extern LPLONG 	GetOBJDPtr(HWND hWnd);
extern HANDLE Gbl_hCursor;
HFONT hHelvFont;
extern int xCtrlExt;
extern int yCtrlExt;


extern int dlgleft;
extern int dlgtop;

struct ICOFILEHDR
	{
	WORD	icoReserved;
	WORD	icoResourceType;
	WORD	icoResourceCount;
	};

struct ICODIRENTRY
	{
	BYTE		width;
	BYTE		height;
	BYTE		colorCount;
	BYTE		reserved1;
	WORD		reserved2;
	WORD		reserved3;
	DWORD		icoDIBSize;
	DWORD		icoDIBOffset;
	};


extern "C" BOOL FAR PASCAL OpenFileDlgProc(
	HWND hDlg,
	unsigned message,
	WORD wParam,
	LONG lParam
	);

BOOL bOPTIONKEY ;
void SaveIconInDB(HWND, int, CURSOROBJ*);
/////////////////////////////////////////////////////////////////////////////
// Allocate and initialize static class members
/////////////////////////////////////////////////////////////////////////////

HANDLE	ICONEDITOR::hAppInstance = NULL;

WINDOWLIST far* ICONEDITOR::windowList = NULL;

short ICONEDITOR::colorCount;

extern SERIUSICON seriusIcon;





/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: IconEditorWndProc
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.: Assumes the static windowList pointer in  class is
//								initialized.
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
long FAR PASCAL _export IconEditorWndProc(
	HWND   hWnd,
	UINT   message,
	WPARAM wParam,
	LPARAM lParam)
	{
	ICONEDITOR far* iconEditor;
	long						result;

	iconEditor = (ICONEDITOR far*)ICONEDITOR::windowList->GetObject(hWnd);

	if (iconEditor != 0)
		{
		result = iconEditor->WindowProc(hWnd, message, wParam, lParam);
		if (message == WM_DESTROY)
			{
			ICONEDITOR::windowList->Remove(hWnd);
			DeleteObject(hHelvFont);
			delete iconEditor;
			}
		}
	else
		switch (message)
			{
			case WM_CREATE:
				{
				LPCREATESTRUCT  lpcs;
				lpcs = (LPCREATESTRUCT) lParam;
				iconEditor = (ICONEDITOR far*)(lpcs->lpCreateParams);
				ICONEDITOR::windowList->Add(hWnd, (LPSTR)iconEditor);
				result = iconEditor->WindowProc(hWnd, message, wParam, lParam);
				return 0;
				}
			case WM_SETCURSOR:
				{
				SetCursor((HICON)Gbl_hCursor); // set it to be the new Cursor Frame
				return TRUE;
				}
			default:
				result = DefWindowProc(hWnd, message, wParam, lParam);
		}
	return result;
	}


HANDLE SetWindowFont(HWND hWnd, char *fontName) {
	HDC hDC = GetDC(hWnd);
	int iLogPixsY = GetDeviceCaps(hDC, LOGPIXELSY);
	HFONT hFont;
	LOGFONT lf;
	lf.lfHeight = iLogPixsY * 8 / 72;
	lf.lfWidth  = 0;
	lf.lfEscapement  = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_BOLD; //400;
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfCharSet    = ANSI_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	lstrcpy(lf.lfFaceName, fontName);

	hFont = CreateFontIndirect(&lf);
	ReleaseDC(hWnd, hDC);
	return hFont;






	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ICONEDITOR
//
// CLASS.......: ICONEDITOR
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

ICONEDITOR::ICONEDITOR(
	HWND	 				hWndParent,
	HANDLE 				hAppInstance,
	HANDLE 				hPrevInstance,
	LPSERIUSICON	theIcon,
	short int			iconType)
	{
	BOOL	status;
	HWND  hTmp;

	short pixelsPerByte;

	ICONEDITOR::hAppInstance = hAppInstance;

	if (!hPrevInstance && !windowList)
		status = registerWindowClass();
	else
		status = TRUE;

	if (!windowList)
		windowList = new WINDOWLIST();

	if (status)
		{
		RECT rect;
    POINT tmpPoint;
		this->hWndParent = hWndParent;

		//hMenu = LoadMenu(hAppInstance, "IconEditorMenu");
		GetClientRect(hWndParent, &rect);

		int width = 478;
		int height = 300 ; //rect.bottom - 15;
		char fontName[30];
		TEXTMETRIC tm;
		strcpy(fontName, "helv");
		hHelvFont = (HFONT)SetWindowFont(hWndParent, fontName);
		// SendMessage(hWndParent, WM_SETFONT, hHelvFont, 0L);

		DWORD lDlgBaseUnits = GetDialogBaseUnits();
		int xDlgUnit = LOWORD(lDlgBaseUnits);  // number of pixels per dlg units
		int yDlgUnit = HIWORD(lDlgBaseUnits);

		HDC hDlgDC = GetDC(hWndParent);
		GetTextMetrics(hDlgDC, &tm);
		int CtrlDlgWt = 40;
		int CtrlDlgHt = 12;
		xCtrlExt = CtrlDlgWt * tm.tmAveCharWidth / 4; // LOWORD(lDlgBaseUnits)) / 4  ;
		yCtrlExt = CtrlDlgHt * tm.tmHeight / 8 ; //HIWORD(lDlgBaseUnits)) / 8   ;
		ReleaseDC(hWndParent, hDlgDC);
		hTmp = GetDlgItem(hWndParent, ID_GROUP);

		// SKJ , added this to fix the BORLAND dlg layout problem
    // Oct. 30, 1993
		GetWindowRect(hTmp, &rect);

		tmpPoint.x = rect.left;
		tmpPoint.y = rect.top;
		ScreenToClient(hWndParent,(LPPOINT)&tmpPoint);
    
		dlgleft = tmpPoint.x;  // 7 * LOWORD(lDlgBaseUnits) / 4  ;
		dlgtop 	= tmpPoint.y;  // 38 * HIWORD(lDlgBaseUnits) / 8 ;


		tmpPoint.x = rect.right;
		tmpPoint.y = rect.bottom;
		ScreenToClient(hWndParent,(LPPOINT)&tmpPoint);
		int dlgwidth = abs(tmpPoint.x -  dlgleft);
		int dlgheight 	= abs(tmpPoint.y - dlgtop);

	
		PIXELWIDTH = ((dlgwidth - SCROLLWIDTH) / PIXELCOUNT)  + 1;
		PIXELHEIGHT = (dlgheight/ PIXELCOUNT) + 1;


		hTmp = GetDlgItem(hWndParent, ID_BORDER);
		GetWindowRect(hTmp, &rect);
		POINT iPt;
		iPt.x = rect.right;
		iPt.y = rect.bottom;
		ScreenToClient(hWndParent,(LPPOINT)&iPt);
		ICONBACKTOP = iPt.y;
		dlgwidth = iPt.x - dlgleft;

		hWndIconEditor = CreateWindow((LPSTR)ICONEDITORWNDCLASS,
									(LPSTR)"ICON EDITOR",
									WS_OVERLAPPED|WS_CHILD,
									dlgleft,dlgtop,//ICONEDITORLEFT, ICONEDITORTOP,
									dlgwidth, ICONEDITORHEIGHT,  // ICONEDITORWIDTH
									hWndParent,
									hMenu,
									(HINSTANCE)hAppInstance,
									(LPSTR)this);

		SendMessage(hWndIconEditor, WM_SETFONT, (WORD)hHelvFont, 0L);

		hTmp = GetDlgItem(hWndParent, ID_BORDER);
		GetWindowRect(hTmp, &rect);
		iPt.x = rect.right;
		iPt.y = rect.bottom;
		ScreenToClient(hWndIconEditor,(LPPOINT)&iPt);
		ICONBACKTOP = iPt.y + 15;


		// hWndIconEditor = hWndParent;

		if (hWndIconEditor)
			{
			hNFrames = GetDlgItem(hWndParent, FRAMECOUNTID);
			hDelayTimeEdit = GetDlgItem(hWndParent,DELAYID);
			hDelButton =  GetDlgItem(hWndParent,DELICONID);


			currentColor = 0;
			haveFileInfo = FALSE;
			if (iconType == COLOR16)
				{
				colorCount = 16;
				pixelsPerByte = 2;
				}
			else if (iconType == BLACKANDWHITE)
				{
				colorCount = 2;
				pixelsPerByte = 8;
				}
			pixelWindow =
			 new PIXELWINDOW(hWndIconEditor, hAppInstance, hPrevInstance,
					 PIXELCOUNT, pixelsPerByte);
			colorPalette =
			 new COLORPALETTE(hWndIconEditor, hAppInstance, hPrevInstance, this,
					pixelWindow, colorCount);

			if (theIcon)
				{
				this->theIcon = theIcon;
				pixelWindow->ReplaceIcon(theIcon);
				}
			else
				this->theIcon = NULL;

			ShowWindow(hWndIconEditor, SW_SHOWNORMAL);
			UpdateWindow(hWndIconEditor);


			}
		I_UNUSED(height);
		I_UNUSED(width);
		}
	I_UNUSED(hTmp);
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ~ICONEDITOR (destructor)
//
// CLASS.......: ICONEDITOR
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

ICONEDITOR::~ICONEDITOR(
	void)
	{

	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: registerWindowClass
//
// CLASS.......: ICONEDITOR
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
BOOL ICONEDITOR::registerWindowClass(
	void
	){
	WNDCLASS  	wc;
	BOOL				status;

	wc.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;
	wc.lpfnWndProc = IconEditorWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(ICONEDITOR far*);
	wc.hInstance = (HINSTANCE)hAppInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = RGB(0, 0, 0) ; //COLOR_WHITE ; // COLOR_APPWORKSPACE+1;
	wc.lpszMenuName =  NULL;
	wc.lpszClassName = ICONEDITORWNDCLASS;

	status = RegisterClass((LPWNDCLASS)(&wc));

	return(status);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: WindowProc
//
// CLASS.......: ICONEDITOR
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

long ICONEDITOR::WindowProc(
	HWND 				hWnd,
	unsigned 		message,
	WORD 				wParam,
	LONG 				lParam
	){
	BOOL 				msgHandled = TRUE;
	static BOOL closing;


	switch (message)
		{
	       case WM_INITDIALOG: {


		  }
		  break;
		case WM_ACTIVATE:
			if (wParam == 0 && closing != TRUE)
				SetActiveWindow(hWnd);
			else
				{
				closing = FALSE;
				msgHandled = FALSE;
				}
			break;

		case WM_SYSCOMMAND:
			if (wParam == SC_CLOSE)
				msgHandled = VerifyClose();
			else
				{
				closing = TRUE;
				msgHandled = FALSE;
				}
			break;

		case WM_COMMAND:
			if (LOWORD(lParam) != 0)	//menu command
				switch (wParam)
					{
					case LOADICONID:
						// SendMessage(GetParent(hWnd), WM_COMMAND, wParam, lParam);
						// OpenIconFile();
						break;
					case IDM_SAVEICON:
					case NEWID:
						// if (theIcon)
						//	pixelWindow->SaveIcon(theIcon);
						// SendMessage(GetParent(hWnd), WM_COMMAND, wParam, lParam);

						break;
					case ID_TEST: {
						// SendMessage(GetParent(hWnd), WM_COMMAND, wParam, lParam);
						break;
						}
					case CLEARID:
						pixelWindow->ClearIcon();
						pixelWindow->Redraw();
						break;
					case DELICONID:
						pixelWindow->ClearIcon();
						pixelWindow->Redraw();
						// SendMessage(GetParent(hWnd), WM_COMMAND, wParam, lParam);

						break;


					case IDM_RESTOREICON:
						if (theIcon)
							{
							pixelWindow->ReplaceIcon(theIcon);
							pixelWindow->Redraw();
							}
						break;


					case IDM_IMPORT:    // Load a cursor from .CUR file

					case IDC_LEFTBUTTON:
								 {
								if (bOPTIONKEY == TRUE) {
						//theIcon.curXHotspot = 0;
						//theIcon.curYHotspot = 0;
								 }
								if (HIWORD(lParam) == BN_CLICKED)
						pixelWindow->SetBackgroundColor(DECREMENTBACKGROUND);
								else
						msgHandled = FALSE;
								 }
								 break;
					case IDC_RIGHTBUTTON:
								 {
								 if (HIWORD(lParam) == BN_CLICKED)
						 pixelWindow->SetBackgroundColor(INCREMENTBACKGROUND);
								 else
						 msgHandled = FALSE;
						}
								 break;
					default:
								 msgHandled = FALSE;
								 break;
					 }


		case WM_LBUTTONDOWN: {

			SendMessage(GetParent(hWnd), WM_LBUTTONDOWN, wParam, lParam);

			}
			break;


		case WM_PAINT:
			Draw();
			break;


		default:
			msgHandled = FALSE;
		}

	if (msgHandled)
		return (1);
	else

		return DefWindowProc(hWnd, message, wParam, lParam);

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: OpenIconFile
//
// CLASS.......: ICONEDITOR
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
void ICONEDITOR::OpenIconFile(
	int hFile)
	{
	LPSERIUSICON	tempIcon;
	if(hFile != -1)
		{
		long size = 32;
		tempIcon = new SERIUSICON(hFile, colorCount);
		_lclose(hFile);

		// seriusIcon = tempIcon;

		pixelWindow->ReplaceIcon(tempIcon);
		pixelWindow->Redraw();


		// seriusIcon = tempIcon;
		delete tempIcon;
		I_UNUSED(size);
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: CheckIconFile
//
// CLASS.......: ICONEDITOR
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
BOOL ICONEDITOR::CheckIconFile(
	int	fileHandle)
	{
	BOOL 							status = FALSE;
	ICOFILEHDR				fileHdr;
	ICODIRENTRY far*	iconDirectory;
	HANDLE						dirHandle;
	int								i;

	_lread(fileHandle, (LPSTR)&fileHdr, sizeof(ICOFILEHDR));

	if (fileHdr.icoResourceType == 1)
		{
		dirHandle = GlobalAlloc(GMEM_MOVEABLE,
														sizeof(ICODIRENTRY)*fileHdr.icoResourceCount);
		iconDirectory = (ICODIRENTRY far*)GlobalLock(dirHandle);
		_lread(fileHandle, (LPSTR)iconDirectory,
					 sizeof(ICODIRENTRY)*fileHdr.icoResourceCount);
		for (i=0; i<fileHdr.icoResourceCount; i++)
			{
			if ((iconDirectory[i].width == PIXELCOUNT) &&
					(iconDirectory[i].height == PIXELCOUNT) &&
					(iconDirectory[i].colorCount == colorCount))
				break;
			}

		if (i<fileHdr.icoResourceCount)
			{
			status = TRUE;
			_llseek(fileHandle, iconDirectory[i].icoDIBOffset, 0);
			}

		GlobalUnlock(dirHandle);
		GlobalFree(dirHandle);
		}

	return status;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: VerifyClose
//
// CLASS.......: ICONEDITOR
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
BOOL ICONEDITOR::VerifyClose(
	void)
	{
	BOOL result = TRUE;
	int	 userResponse;
  LPCURSOROBJ cursorObj = (LPCURSOROBJ)GetOBJDPtr(hWndParent);
	if (pixelWindow->IsDirty())
		{
		userResponse = MessageBox(hWndParent, "Save Changes to Icon?",
		 "Icon Editor", MB_ICONQUESTION|MB_YESNOCANCEL|MB_DEFBUTTON1);
		switch (userResponse)
			{
			case IDYES:
				if (theIcon)
					pixelWindow->SaveIcon(theIcon);
				SaveIconInDB(hWndParent, cursorObj->nCurrentFrame, cursorObj);
				result = FALSE;
				break;
			case IDNO:
				result = FALSE;
				break;
			case IDCANCEL:
				result = TRUE;
				break;
			default:
				result  = TRUE;
				break;
			}
		}
	else
		result = FALSE;

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Show
//
// CLASS.......: ICONEDITOR
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
void ICONEDITOR::Show(
	void
	){

	ShowWindow(hWndIconEditor, SW_SHOW);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Draw
//
// CLASS.......: ICONEDITOR
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
void ICONEDITOR::Draw(
	void)
	{
	PAINTSTRUCT		paintInfo;
	HDC						hDC;
	hDC = BeginPaint(hWndIconEditor, (LPPAINTSTRUCT)&paintInfo);

	pixelWindow->Show();
	colorPalette->Show();

	DrawCurrentColor(hDC);

	DrawLabel(hDC, "Current Color", CURCOLORLEFT, CURCOLORTOP, CURCOLORWIDTH + 10);

	DrawLabel(hDC, "Color Palette", PALLEFT, PALTOP, PALWIDTH + 10);


	DrawLabel(hDC, "Cursor", ICONBACKLEFT + ICONBACKWIDTH / 4, ICONBACKTOP + LABELSPACE + LABELSPACE, ICONBACKWIDTH);

	EndPaint(hWndIconEditor, (LPPAINTSTRUCT)&paintInfo);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DrawCurrentColor
//
// CLASS.......: ICONEDITOR
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
void ICONEDITOR::DrawCurrentColor(
	HDC	hDC)
	{
	HPEN					hPen;
	HPEN					hOldPen;
	HBRUSH				hBrush;
	HBRUSH				hOldBrush;
	HFONT					hFont;
	HFONT					hOldFont;
	short					textHeight;

	hPen = (HPEN)GetStockObject(BLACK_PEN);
	hOldPen = (HPEN) SelectObject(hDC, hPen);
	hBrush = CreateSolidBrush(pixelWindow->GetColor(currentColor));
	hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	Rectangle(hDC, CURCOLORLEFT, CURCOLORTOP, CURCOLORLEFT+xCtrlExt,
	 CURCOLORTOP+CURCOLORHEIGHT);

	if ((currentColor == SCREENCOLOR) || (currentColor == INVERTEDCOLOR))
		{
		SetBkMode(hDC, TRANSPARENT);
		hFont = CreateFont(13, 0, 0, 0, 10, FALSE, FALSE, FALSE, ANSI_CHARSET,
		 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		 VARIABLE_PITCH|FF_SWISS, "Helv");
		hOldFont = (HFONT)SelectObject(hDC, hFont);
		SetTextAlign(hDC, TA_LEFT);
		textHeight = HIWORD(GetTextExtent(hDC, "STRING", 6));
		if (currentColor == SCREENCOLOR)
			DrawLabel(hDC, "Screen", CURCOLORLEFT + 8, CURCOLORTOP+CURCOLORHEIGHT -10, CURCOLORWIDTH);

		else if (currentColor == INVERTEDCOLOR)
			DrawLabel(hDC, "Inverted", CURCOLORLEFT + 8, CURCOLORTOP+CURCOLORHEIGHT -10, CURCOLORWIDTH);

			// TextOut(hDC, CURCOLORLEFT+CURCOLORWIDTH/2,
			//  CURCOLORTOP+CURCOLORHEIGHT/2-textHeight/2, "Inverted Screen", 8);

			//TextOut(hDC, CURCOLORLEFT +CURCOLORWIDTH/2,
			//	CURCOLORTOP+CURCOLORHEIGHT/2-textHeight/2, "Screen", 6);


		SetTextAlign(hDC, TA_LEFT|TA_TOP|TA_NOUPDATECP);
		SelectObject(hDC, hOldFont);

		DeleteObject(hFont);
		}

	SelectObject(hDC, hOldPen);
	SelectObject(hDC, hOldBrush);

	DeleteObject(hPen);
	DeleteObject(hBrush);

	I_UNUSED(textHeight);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SetCurrentColor
//
// CLASS.......: ICONEDITOR
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
void ICONEDITOR::SetCurrentColor(
	short		colorIndex)
	{
	HDC			hDC;

	pixelWindow->SetCurrentColor(colorIndex);
	currentColor = colorIndex;

	hDC = GetDC(hWndIconEditor);
	DrawCurrentColor(hDC);
	ReleaseDC(hWndIconEditor, hDC);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DrawLabel
//
// CLASS.......: ICONEDITOR
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

void ICONEDITOR::DrawLabel(
	HDC			hDC,
	LPSTR		text,
	short		left,
	short		top,
	short		width)
	{
	HPEN					hPen;
	HPEN					hOldPen;

	// top -= ICONBACKTOP; // SKJ Nov. 5, 1994 Change TextOut to TA_TOP instead of TA_BOTTOM
	hPen = (HPEN)GetStockObject(BLACK_PEN);
	hOldPen = (HPEN)SelectObject(hDC, hPen);
	SelectObject(hDC, hHelvFont);
	SetBkMode(hDC, TRANSPARENT);

	SetTextAlign(hDC, TA_LEFT|TA_BOTTOM); // BOTTOM);

	TextOut(hDC, left, top - LABELSPACE, text, lstrlen(text)); // -LABELSPACE

	SetTextAlign(hDC, TA_LEFT|TA_TOP|TA_NOUPDATECP);

	SelectObject(hDC, hOldPen);

	DeleteObject(hPen);
	I_UNUSED(width);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: GetIconBitmap
//
// CLASS.......: ICONEDITOR
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
BOOL ICONEDITOR::GetIconBitmap(
	int	fileHandle)
	{
	BOOL 							status = FALSE;
	ICOFILEHDR				fileHdr;
	ICODIRENTRY far*	iconDirectory;
	HANDLE						dirHandle;
	HANDLE						bitmapHandle;
	LPBITMAPINFO      bitmapPtr;
	int								i;

	_lread(fileHandle, (LPSTR)&fileHdr, sizeof(ICOFILEHDR));

	if (fileHdr.icoResourceType == 1)
		{
		dirHandle = GlobalAlloc(GMEM_MOVEABLE,
														sizeof(ICODIRENTRY)*fileHdr.icoResourceCount);
		iconDirectory = (ICODIRENTRY far*)GlobalLock(dirHandle);
		_lread(fileHandle, (LPSTR)iconDirectory,
					 sizeof(ICODIRENTRY)*fileHdr.icoResourceCount);
		for (i=0; i<fileHdr.icoResourceType; i++)
			{
			if ((iconDirectory[i].width == PIXELCOUNT) &&
					(iconDirectory[i].height == PIXELCOUNT) &&
					(iconDirectory[i].colorCount == 16))
				break;
			}
		if (i<fileHdr.icoResourceCount)
			{
			status = TRUE;
			bitmapHandle = GlobalAlloc(GMEM_MOVEABLE,
			 sizeof(BITMAPINFOHEADER)+16*sizeof(RGBQUAD)+
			 iconDirectory[i].icoDIBSize);
			bitmapPtr = (LPBITMAPINFO)GlobalLock(bitmapHandle);
			_llseek(fileHandle, iconDirectory[i].icoDIBOffset, 0);
			_lread(fileHandle, (LPSTR)bitmapPtr, (int)(sizeof(BITMAPINFOHEADER)+
				16*sizeof(RGBQUAD)+iconDirectory[i].icoDIBSize));
//			pixelWindow->ReplaceIcon(bitmapPtr);
			colorPalette->UpdateIndices();
			pixelWindow->UpdateIcon(TRUE);
			GlobalUnlock(bitmapHandle);
			GlobalFree(bitmapHandle);
			}
		}

	return status;

	}
