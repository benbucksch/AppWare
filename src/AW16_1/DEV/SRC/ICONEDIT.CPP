#include <string.h>
#include "almsys.h"
#include <windowsx.h>

#include "a_almutl.h"
#include "resconst.h"
#include "position.h"
#include "iconedit.h"
#include "colorpal.h"
#include "winlist.h"
#include "openfile.h"
#include "pixelwin.h"
#include "sericon.h"

#define  ICONEDITORWNDCLASS	"SERIUS_ICON_EDITOR"

#define IDC_RIGHTBUTTON	10
#define IDC_LEFTBUTTON	20
#define IDC_CLEARICON		102
#define IDC_RESTOREICON	101
#define IDC_IMPORTICON	103
#define IDC_EXPORTICON  104

#define CAPTION		"ICON EDITOR"
#define CLASSNAME "DIALOG"
#define MENUNAME  "IconEditorMenu"

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

typedef struct {
	long  dtStyle;
	BYTE  dtItemCount;
	int   dtX;
	int   dtY;
	int   dtCX;
	int   dtCY;
//char  dtMenuName[];
//char  dtClassName[];
//char  dtCaptionText[];
	} DLGTEMPLATE, far* LPDLGTEMPLATE;


// Allocate static data members of the class

HINSTANCE					ICONEDITOR::hAppInstance = NULL;
short							ICONEDITOR::colorCount;

HWND	   			 		ICONEDITOR::hWndParent;
HWND					 		ICONEDITOR::hWndIconEditor;
PIXELWINDOW far*	ICONEDITOR::pixelWindow;
COLORPALETTE far*	ICONEDITOR::colorPalette;
short int					ICONEDITOR::currentColor;
HWND							ICONEDITOR::hLeftButton;
HWND							ICONEDITOR::hRightButton;
BOOL							ICONEDITOR::haveFileInfo;
LPSERIUSICON			ICONEDITOR::theIcon;
HINSTANCE					ICONEDITOR::hPrevInstance;
short int					ICONEDITOR::iconType;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ICONEDITOR
//
// CLASS.......: ICONEDITOR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////

int ICONEDITOR::Dialog(
	HWND	 				hWndParent,
	HINSTANCE			hAppInstance,
	HINSTANCE			hPrevInstance,
	LPSERIUSICON	theIcon,
	short int			iconType)
	{
	GLOBALHANDLE	hTemplate;
	LPDLGTEMPLATE	pTemplate;
	LPSTR					pString;
	DLGPROC				lpProc;
	int						result;

	ICONEDITOR::hAppInstance = hAppInstance;

	ICONEDITOR::hPrevInstance = hPrevInstance;

	ICONEDITOR::hWndParent = hWndParent;

	ICONEDITOR::theIcon = theIcon;

	ICONEDITOR::iconType = iconType;

	hTemplate = GlobalAlloc(GMEM_MOVEABLE,
	 sizeof(DLGTEMPLATE)+ lstrlen(MENUNAME) + lstrlen(CLASSNAME) +
	 lstrlen(CAPTION) + 3);

	pTemplate = (LPDLGTEMPLATE)GlobalLock(hTemplate);

	pTemplate->dtStyle = DS_MODALFRAME|WS_CAPTION|WS_SYSMENU;

	pTemplate->dtItemCount = 0;

	pTemplate->dtX = ICONEDITORLEFT;
	pTemplate->dtY = ICONEDITORTOP;
	pTemplate->dtCX = ICONEDITORWIDTH;
	pTemplate->dtCY = ICONEDITORHEIGHT;

	pString = (LPSTR)(pTemplate+1);

	lstrcpy(pString, MENUNAME);

	pString += (lstrlen(MENUNAME) + 1);

	lstrcpy(pString, CLASSNAME);

	pString += (lstrlen(CLASSNAME) + 1);

	lstrcpy(pString, CAPTION);

	GlobalUnlock(hTemplate);

	lpProc = (DLGPROC) MakeProcInstance((FARPROC)IconEditorDlgProc, hAppInstance);
	result = DialogBox(hAppInstance, "ICON_EDITOR", hWndParent, lpProc);
	FreeProcInstance((FARPROC) lpProc);

	GlobalFree(hTemplate);

	return result;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InitDialog
//
// CLASS.......: ICONEDITOR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////

void ICONEDITOR::InitDialog(
	HWND	hDlg)
	{
	short pixelsPerByte;

	hWndIconEditor = hDlg;

	hLeftButton = CreateWindow("Button",  "<",
																 BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE,
																 LEFTBUTTONLEFT, LEFTBUTTONTOP,
																 BUTTONWIDTH, BUTTONHEIGHT, hWndIconEditor,
																 (HMENU) IDC_LEFTBUTTON, hAppInstance, NULL);

	hRightButton = CreateWindow("Button", ">",
																 BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE,
																 RIGHTBUTTONLEFT, RIGHTBUTTONTOP,
																 BUTTONWIDTH, BUTTONHEIGHT, hWndIconEditor,
																 (HMENU) IDC_RIGHTBUTTON, hAppInstance, NULL);
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
			 new COLORPALETTE(hWndIconEditor, hAppInstance, hPrevInstance,
												pixelWindow, colorCount);

	if (theIcon)
		{
		theIcon = theIcon;
		pixelWindow->ReplaceIcon(theIcon);
		colorPalette->UpdateIndices();
		colorPalette->UpdateColors();
		}
	else
		theIcon = NULL;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: IconEditorDlgProc
//
// CLASS.......: ICONEDITOR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL I_CALLBACK ICONEDITOR::IconEditorDlgProc(
	HWND 				hDlg,
	UINT		 		message,
	WPARAM			wParam,
	LPARAM			lParam)
	{
	BOOL 				msgHandled = TRUE;

	switch (message)
		{
		case WM_INITDIALOG:
			AUtlCenterDialog(hDlg, 0);		// this is safe in both VAB and ALMBLDR
			InitDialog(hDlg);
			break;

		case WM_COMMAND:
			switch (wParam)
				{
				case IDC_CLEARICON:
					pixelWindow->ClearIcon();
					pixelWindow->Redraw();
					break;

				case IDC_RESTOREICON:
					if (theIcon)
						{
						pixelWindow->ReplaceIcon(theIcon);
						pixelWindow->Redraw();
            colorPalette->UpdateIndices();
            colorPalette->UpdateColors();
						colorPalette->Show();
						}
					break;

				case IDC_IMPORTICON:
					OpenIconFile();
					break;

				case IDC_EXPORTICON:
					SaveIconInFile();
					break;

				case IDC_LEFTBUTTON:
					if (HIWORD(lParam) == BN_CLICKED)
						pixelWindow->SetBackgroundColor(DECREMENTBACKGROUND);
					else
						msgHandled = FALSE;
					break;

				case IDC_RIGHTBUTTON:
					if (HIWORD(lParam) == BN_CLICKED)
						pixelWindow->SetBackgroundColor(INCREMENTBACKGROUND);
					else
						msgHandled = FALSE;
					break;

				case IDCANCEL:
					EndDialog(hDlg, FALSE);
					break;

				case IDOK:
					if (theIcon)
						pixelWindow->SaveIcon(theIcon);
					EndDialog(hDlg, TRUE);
					break;

				default:
					msgHandled = FALSE;
				}
			break;

		case WM_PAINT:
			Draw();
			break;

		default:
			msgHandled = FALSE;
		}

	return msgHandled;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SaveIconInFile
//
// CLASS.......: ICONEDITOR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL ICONEDITOR::SaveIconInFile(
	void)
	{
	char 					 filePath[256];
	char					 fileName[100];
	int						 hFile;
	BOOL					 result;

	hFile = SaveAsDlgBox(hAppInstance,
                       hWndIconEditor,
	                     "Icon Files (*.ico)\0*.ico\0",
                       1,
                       "icon.ico",
                       ".",
	                     "Save Icon to a File",
                       OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT,
	                     "ico",
                       filePath,
                       fileName);

	if(hFile != -1)
		{
		result = theIcon->Write(hFile);
		_lclose(hFile);
		}
	else
		result = FALSE;


	return result;


	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: OpenIconFile
//
// CLASS.......: ICONEDITOR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void ICONEDITOR::OpenIconFile(
	void)
	{
	char 					filePath[150];
	char					fileName[13];
	int						hFile;
	LPSERIUSICON	tempIcon;

	hFile = OpenFileDlgBox(hAppInstance, hWndIconEditor,
		"Icons (*.ico)\0*.ico\0",
		1, "", ".","Import Icon",
		OFN_PATHMUSTEXIST|OFN_SHOWHELP|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY,
		 "*.ico",
		filePath, fileName, (CHECKPROC)CheckIconFile);

	if(hFile != -1)
		{
		tempIcon = new SERIUSICON(hFile, colorCount);
		_lclose(hFile);
		pixelWindow->ReplaceIcon(tempIcon);
		pixelWindow->Redraw();
    colorPalette->UpdateIndices();
    colorPalette->UpdateColors();
    colorPalette->Show();
		delete tempIcon;
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: CheckIconFile
//
// CLASS.......: ICONEDITOR
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL ICONEDITOR::CheckIconFile(
	int	fileHandle,
	LPSTR /*fileName*/)
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
/////////////////////////////////////////////////////////////////////////////
BOOL ICONEDITOR::VerifyClose(
	void)
	{
	BOOL result = TRUE;
	int	 userResponse;

	if (pixelWindow->IsDirty())
		{
		userResponse = MessageBox(hWndParent, "Save Changes to Icon?",
		 "Icon Editor", MB_ICONQUESTION|MB_YESNOCANCEL|MB_DEFBUTTON1);
		switch (userResponse)
			{
			case IDYES:
				if (theIcon)
					pixelWindow->SaveIcon(theIcon);
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
/////////////////////////////////////////////////////////////////////////////
void ICONEDITOR::Draw(
	void)
	{
	PAINTSTRUCT		paintInfo;
	HDC						hDC;

	hDC = BeginPaint(hWndIconEditor, (LPPAINTSTRUCT)&paintInfo);

	pixelWindow->Show();
	pixelWindow->UpdateIcon(TRUE);
	colorPalette->Show();

	DrawCurrentColor(hDC);

	DrawLabel(hDC, "Current Color", CURCOLORLEFT, CURCOLORTOP, CURCOLORWIDTH);

	DrawLabel(hDC, "Color Palette", PALLEFT, PALTOP, PALWIDTH);

	DrawLabel(hDC, "Icon", ICONBACKLEFT, ICONBACKTOP, ICONBACKWIDTH);

	DrawLabel(hDC, "BackGround", LEFTBUTTONLEFT, LEFTBUTTONTOP,
	 ICONBACKWIDTH);

	EndPaint(hWndIconEditor, (LPPAINTSTRUCT)&paintInfo);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DrawCurrentColor
//
// CLASS.......: ICONEDITOR
//
// DESCRIPTION.:
//
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

	hPen = GetStockPen(BLACK_PEN);
	hOldPen = SelectPen(hDC, hPen);
	hBrush = CreateSolidBrush(pixelWindow->GetColor(currentColor));
	hOldBrush = SelectBrush(hDC, hBrush);

	Rectangle(hDC, CURCOLORLEFT, CURCOLORTOP, CURCOLORLEFT+CURCOLORWIDTH,
	 CURCOLORTOP+CURCOLORHEIGHT);

	if ((currentColor == SCREENCOLOR) || (currentColor == INVERTEDCOLOR))
		{
		SetBkMode(hDC, TRANSPARENT);
		hFont = CreateFont(13, 0, 0, 0, 10, FALSE, FALSE, FALSE, ANSI_CHARSET,
		 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		 VARIABLE_PITCH|FF_SWISS, "Helv");
		hOldFont = SelectFont(hDC, hFont);
		SetTextAlign(hDC, TA_CENTER);
		textHeight = HIWORD(GetTextExtent(hDC, "STRING", 6));
		if (currentColor == SCREENCOLOR)
			TextOut(hDC, CURCOLORLEFT+CURCOLORWIDTH/2,
			 CURCOLORTOP+CURCOLORHEIGHT/2-textHeight/2, "Screen", 6);
		else if (currentColor == INVERTEDCOLOR)
			TextOut(hDC, CURCOLORLEFT+CURCOLORWIDTH/2,
			 CURCOLORTOP+CURCOLORHEIGHT/2-textHeight/2, "Inverted Screen", 8);
		SetTextAlign(hDC, TA_LEFT|TA_TOP|TA_NOUPDATECP);
		SelectFont(hDC, hOldFont);
		DeleteFont(hFont);
		}

	SelectPen(hDC, hOldPen);
	SelectBrush(hDC, hOldBrush);

	DeletePen(hPen);
	DeleteBrush(hBrush);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SetCurrentColor
//
// CLASS.......: ICONEDITOR
//
// DESCRIPTION.:
//
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

	hPen = GetStockPen(BLACK_PEN);
	hOldPen = SelectPen(hDC, hPen);

	SetBkMode(hDC, TRANSPARENT);

	SetTextAlign(hDC, TA_CENTER|TA_BOTTOM);

	TextOut(hDC, left+width/2, top-LABELSPACE, text, lstrlen(text));

	SetTextAlign(hDC, TA_LEFT|TA_TOP|TA_NOUPDATECP);

	SelectPen(hDC, hOldPen);

	DeletePen(hPen);

	}

