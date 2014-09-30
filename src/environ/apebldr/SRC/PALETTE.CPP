#include "almsys.h"
#include <windowsx.h>
#include <stdlib.h>
#include "basewin.h"
#include "mdiclass.h"
#include "winlist.h"
#include "palette.h"
#include "cfglibp.h"
#include "objdesc.h"
#include "funcdesc.h"
#include "tstring.h"
#include "helpids.h"
#include "progsys.h"
#include "compuid.h"
#include "cfgdesc.h"
#include "helpcls.h"
#include "resconst.h"
#include "subject.h"
#include "sjtwin.h"
#include "objinst.h"
#include "project.h"
#include "intdebug.h"
#include "apbutil.h"
#include "oiputils.h"
#include "a_almutl.h"
#include "intutl.h"

//only include the file for the demo version
#ifdef DEMO
#include "demo.h"
#endif

WINDOWLIST * PALETTE::list = NULL;
WNDPROC PALETTE::lpfnOldObj = NULL;
WNDPROC PALETTE::lpfnOldFunc = NULL;
WNDPROC PALETTE::lpfnOldCat = NULL;
WNDPROC PALETTE::lpfnOldNumCat = NULL;
HWND PALETTE::lastValidDropWindow=NULL;
WORD PALETTE::dropType=0;
HCURSOR PALETTE::dropCursor = NULL;
POINT PALETTE::lastDropPoint;
LPOBJDESC	PALETTE::pAddObjDef;
LPAFUNCDESC PALETTE::pAddFuncDef;
HBITMAP		PALETTE::upArrow=NULL;
HFONT 		PALETTE::tinyFont;

//status bits
#define PAL_REGION1 0x00000001L
#define PAL_REGION2 0x00000002L
#define PAL_REGION3 0x00000004L

#define PAL_ALLREGIONS (PAL_REGION1 | PAL_REGION2 | PAL_REGION3)

#define PAL_MINIMIZED				0x00000010L
#define PAL_OBJICONS				0x00000020L
#define PAL_FUNCICONS				0x00000040L

#define PAL_TURBOENABLED		0x00000100L

#define IDM_PAL_CONFIGURE 0x8000  //defined system commands follow the
																	//format of 0XFxx0 so begin with an
																	//8 so as not to conflict
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	PALETTE
//
// DESCRIPTION.:  constructor
/////////////////////////////////////////////////////////////////////////////
PALETTE::PALETTE(
	HWND 		hwnd)
	{
	static BOOL	registered=FALSE;
  HDC         hDC;
  TEXTMETRIC  tm;
	WORD type;;

	//set defaults before reading in preferences
	status = 0L;
	nWidth = GetSystemMetrics(SM_CXFULLSCREEN);

	hFont = CreateFont(13, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
    VARIABLE_PITCH|FF_SWISS, "Helv");

	tinyFont = CreateFont(10, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, 
		VARIABLE_PITCH, "Small Fonts");

  hDC = GetDC(hwnd);
  SelectObject(hDC, hFont);
  GetTextMetrics(hDC, &tm);
  ReleaseDC(hwnd, hDC);

	nHeight = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYHSCROLL) +
   GetSystemMetrics(SM_CYICON) + tm.tmHeight + 9;
	nLeft	=	0;
	nTop = GetSystemMetrics(SM_CYFULLSCREEN)+GetSystemMetrics(SM_CYMENU) -
   (nHeight-2);
  nObjEnd = nWidth/2;

	ReadPreferences();

  extraLine = 0;
	hInst = PROGSYSTEM::hInst;
	lastActiveWindow = NULL;

	if(!list)
		list = (WINDOWLIST *)new WINDOWLIST;

	if(!registered)
		{
		Register();
		registered = TRUE;
		}

	if(!upArrow)
		{
		upArrow = LoadBitmap(hInst, MAKEINTRESOURCE(IDBMP_UPARROW));
		I_ASSERT(upArrow);
		}

	//create the popup as a message hog so we won't exit this routine until the popup
	//has been closed.
	categoryPopupPtr = new POPUPMENU(NULL, hFont, 
		POPUP_HCENTER | POPUP_MESSAGEHOG | POPUP_MAXSIZE | POPUP_SORTED |
		POPUP_LEFTBUTTON);

	type = POPUP_HCENTER | POPUP_MESSAGEHOG | POPUP_MAXSIZE | POPUP_SORTED |
		POPUP_LEFTBUTTON | POPUP_EXTENDED;

	if(status & PAL_OBJICONS)
		type |= POPUP_OBJICONS;

	objPopupPtr = new POPUPMENU(NULL, hFont, type);

	type = POPUP_HCENTER | POPUP_MESSAGEHOG | POPUP_MAXSIZE | POPUP_SORTED |
		POPUP_LEFTBUTTON | POPUP_EXTENDED;

	if(status & PAL_FUNCICONS)
		type |= POPUP_FUNCICONS;

	funcPopupPtr = new POPUPMENU(NULL, hFont, type);


	creating = TRUE;
	
	CreatePaletteWindow(hwnd);

	
  DrawExtraLine(nObjEnd+2);
  MoveWindow(PROGSYSTEM::hWndFrame, 0, 0, GetSystemMetrics(SM_CXSCREEN),
   GetSystemMetrics(SM_CYSCREEN)-nHeight+1, TRUE);

	DrawMenuBar(hwndPalette);

	ActivateOldWindow();
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	~PALETTE
//
// DESCRIPTION.:  destructor
/////////////////////////////////////////////////////////////////////////////
PALETTE::~PALETTE(
	void)
	{
	WritePreferences();
  DeleteObject(hFont);
	DeleteObject(tinyFont);
	if(PALETTE::list->IsListEmpty())
		{
		delete PALETTE::list;
		PALETTE::list = NULL;
		}

	delete categoryPopupPtr;
	delete objPopupPtr;
	delete funcPopupPtr;
	if(upArrow)
		DeleteObject(upArrow);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  PaletteWndProc
//
// DESCRIPTION.:  window procedure for the palette
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK PaletteWndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	PALETTE * 		objectPtr;
	LPCREATESTRUCT 	cs;
	long 						success;

	if(PALETTE::list)
		{
		objectPtr = (PALETTE *)PALETTE::list->GetObject(hWnd);
		if(!objectPtr)
			{
			if(message == WM_CREATE)
				{
				cs = (LPCREATESTRUCT)lParam;
				PALETTE::list->Add(hWnd, (LPSTR)cs->lpCreateParams);
				objectPtr = (PALETTE *)PALETTE::list->GetObject(hWnd);
				return objectPtr->WndProc(hWnd, message, wParam, lParam);
				}
			else
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		else if(message == WM_DESTROY)
			{
			if(PALETTE::list->Remove(hWnd))
				{
				success = objectPtr->WndProc(hWnd, message, wParam, lParam);
				return success;
				}
			else
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		else
			return objectPtr->WndProc(hWnd, message, wParam, lParam);
		}
	else
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	Register
//
// DESCRIPTION.:  registers the palette class
/////////////////////////////////////////////////////////////////////////////
void PALETTE::Register()
	{
	WNDCLASS wc;

	wc.style 					= CS_VREDRAW|CS_HREDRAW|CS_DBLCLKS;
	wc.lpfnWndProc 		= PaletteWndProc;
	wc.cbClsExtra  		= 0;
	wc.cbWndExtra  		= 0;
	wc.hInstance 			= hInst;
	wc.hIcon 					= LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor 				= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground 	= GetStockBrush(WHITE_BRUSH);
	wc.lpszMenuName 	= NULL;
	wc.lpszClassName 	= "Palette";
	RegisterClass((LPWNDCLASS)&wc);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	PALETTE
//
// DESCRIPTION.:  window procedure
/////////////////////////////////////////////////////////////////////////////
LRESULT PALETTE::WndProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	static BOOL					bSizeIt;
	static short 				cyClient, nLastX=0, xpos, ypos;
	char								buffer[50];
	HDC									hDC;
	HPEN    						hPen;
	LPDRAWITEMSTRUCT		lpdis;
  LPAFUNCDESC	        funcDef;
	LPMEASUREITEMSTRUCT lpmis;
	LPOBJDESC						pObjDef;
	int								catCount, index, length;
  LPFILEOBJECT        pFileObject;
  LPCFGDESC           pConfig;
	RECT								rc;
	HPEN 								shadow;
	HBRUSH 							bkgrnd;
	HMENU								hMenu;

	switch (message)
		{
		case WM_SYSCOMMAND:
			if((wParam & 0xFFF0) == SC_CLOSE)
				{
				hide();
      	return 0;
				}
			else if((wParam	& 0xFFF0) == SC_MINIMIZE)
				{
				if(status & PAL_TURBOENABLED)
					{
					I_ALWAYSASSERT(!(status & PAL_MINIMIZED), return 0);

					MinimizeWindow();
					}
				return 0;
				}
			else if((wParam & 0xFFF0) == SC_MAXIMIZE)
				{
				if(status & PAL_TURBOENABLED)
					{
					I_ALWAYSASSERT(status & PAL_MINIMIZED, return 0);
				
					MaximizeWindow();
					}
				return 0;
				}
			else if((wParam & 0xFFF0) == IDM_PAL_CONFIGURE)
				{
				if(status & PAL_TURBOENABLED)
					{
					ConfigureTurboPalette();
					}
				return 0;
				}

			break;

		case WM_CREATE:
			SIZE size;
			hwndPalette = hwnd;
			//calculate required width of category list box
			hDC = GetDC(hwndPalette);
			SelectObject(hDC, hFont);
			GetTextExtentPoint32(hDC, "0 Categories", 12, &size);
			catWidth = size.cx;
			pFileObject = 0;
			while (0 != (pFileObject = cfgGetNextFileObject(pFileObject)))
				{
				pConfig = (LPCFGDESC)pFileObject->GetHeadRecPtr();
				if (lstrcmp("", pConfig->pGetFuncGroupName()) != 0)
					{
					lstrcpy(buffer, pConfig->pGetFuncGroupName());
					GetTextExtentPoint32(hDC, buffer, lstrlen(buffer), &size);
					length = size.cx;
					if(length > catWidth)
						catWidth = length;
					}
				}
			ReleaseDC(hwndPalette, hDC);
			catWidth += GetSystemMetrics(SM_CXVSCROLL)+5;
			catBegin = nWidth-catWidth;

			hwndObject = CreateWindow("listbox", NULL,
			 LBS_NOTIFY|LBS_OWNERDRAWFIXED|LBS_MULTICOLUMN|LBS_NOINTEGRALHEIGHT|
			 WS_BORDER|WS_HSCROLL|WS_CHILD|LBS_SORT|LBS_HASSTRINGS,
			 0, 0, nObjEnd, nHeight-GetSystemMetrics(SM_CYCAPTION)-1,
			 hwnd, (HMENU) ID_LISTBOX, hInst, NULL);
			lpfnOldObj = (WNDPROC)SetWindowLong(hwndObject, GWL_WNDPROC,
			 (LONG)PALETTE::ObjProc);

			hwndFunction = CreateWindow("listbox", NULL,
			 LBS_NOTIFY|LBS_OWNERDRAWFIXED|LBS_MULTICOLUMN|LBS_NOINTEGRALHEIGHT|
			 WS_BORDER|WS_HSCROLL|WS_CHILD|LBS_SORT|LBS_HASSTRINGS,
			 nObjEnd+4, 0, catBegin-nObjEnd-5,
			 nHeight-GetSystemMetrics(SM_CYCAPTION)-1,
			 hwnd, (HMENU) ID_LISTBOX2, hInst, NULL);
			lpfnOldFunc = (WNDPROC)SetWindowLong(hwndFunction, GWL_WNDPROC,
			 (LONG)PALETTE::FuncProc);

			catHeight = nHeight-GetSystemMetrics(SM_CYHSCROLL) -
			 GetSystemMetrics(SM_CYCAPTION);

		 catStatHeight = 2 * GetSystemMetrics(SM_CYHSCROLL);

			hwndCategory = CreateWindow("listbox", NULL,
			 LBS_NOTIFY|LBS_NOINTEGRALHEIGHT|WS_BORDER|WS_VSCROLL|WS_CHILD|
			 LBS_SORT,
			 catBegin, 0, catWidth-2, catHeight,
			 hwnd, (HMENU) ID_LISTBOX3, hInst, NULL);
			lpfnOldCat = (WNDPROC)SetWindowLong(hwndCategory, GWL_WNDPROC,
			 (LONG)PALETTE::CategoryProc);

			hwndCatStatus = CreateWindow("button", NULL,
				WS_BORDER|WS_CHILD|BS_OWNERDRAW|WS_CLIPSIBLINGS,
				0,0,0,0, hwnd, (HMENU) ID_CATSTAT, hInst, NULL);

			hwndNumCat = CreateWindow("BUTTON", NULL,
				ES_RIGHT|WS_CHILD|WS_BORDER|BS_OWNERDRAW|WS_CLIPSIBLINGS,
				catBegin-1, catHeight, catWidth, GetSystemMetrics(SM_CYHSCROLL)+2,
				hwnd, (HMENU) ID_NUMCAT, hInst, NULL);

			lpfnOldNumCat = (WNDPROC)SetWindowLong(hwndNumCat, GWL_WNDPROC,
			 (LONG)PALETTE::NumCatProc);

			SetWindowFont(hwndObject, hFont, TRUE);
			SetWindowFont(hwndFunction, hFont, TRUE);
			SetWindowFont(hwndCategory, hFont, TRUE);
			SetWindowFont(hwndNumCat, hFont, TRUE);
			SetWindowFont(hwndCatStatus, hFont, TRUE);

			//modify menu before minimizing or maximizing
			hMenu = GetSystemMenu(hwnd, NULL);
			DeleteMenu(hMenu, SC_TASKLIST, MF_BYCOMMAND);
			DeleteMenu(hMenu, GetMenuItemCount(hMenu)-1, MF_BYPOSITION);
			DeleteMenu(hMenu, SC_RESTORE, MF_BYCOMMAND);
			DeleteMenu(hMenu, SC_SIZE, MF_BYCOMMAND);

			if(!(status & PAL_TURBOENABLED))
				{
				DeleteMenu(hMenu, SC_MINIMIZE, MF_BYCOMMAND);
				DeleteMenu(hMenu, SC_MAXIMIZE, MF_BYCOMMAND);
				}

			if(status & PAL_MINIMIZED)
				MinimizeWindow();
			else
				MaximizeWindow();

			// Fill object list Box
			pObjDef = NULL;
			while(0 != (pObjDef = cfgGetNextObjDesc(pObjDef)))
				{
				if (!pObjDef->TestHiddenFlag())
					{
					index = ListBox_AddString(hwndObject, pObjDef->pGetName());
					ListBox_SetItemData(hwndObject, index, (long)pObjDef);

					//fill popup menu
					objPopupPtr->AppendItem(pObjDef->pGetName(),
							(long)pObjDef, NO_ORDINAL);
					}
				}

			// Fill category list box
			pFileObject = 0;
			catCount = 0;
			while(0 != (pFileObject = cfgGetNextFileObject(pFileObject)))
				{
				pConfig = (LPCFGDESC)pFileObject->GetHeadRecPtr();
				if(0 != lstrcmp("", pConfig->pGetFuncGroupName()))
					{
					funcDef = 0;
					if(0 != (funcDef = pConfig->pNextFunctionDesc(funcDef)))
						{
						index = ListBox_AddString(hwndCategory, pConfig->pGetFuncGroupName());
						ListBox_SetItemData(hwndCategory, index, (long)pConfig);
						catCount++;

						//fill popup menu
						categoryPopupPtr->AppendItem(pConfig->pGetFuncGroupName(),
							(long)pConfig, NO_ORDINAL);

						}
					}
				}

			//select the General category if Present else select top category
			index = ListBox_FindString(hwndCategory, -1, (long)"General");
			if(index == LB_ERR)
				index = 0;
			ListBox_SetCurSel(hwndCategory, index);
			ListBox_SetTopIndex(hwndCategory, (index<1?0:index-1));

			SetObjColumnWidth();

      //display category count
			wsprintf(buffer, "%d Categories", catCount);
			SetWindowText(hwndNumCat, (LPSTR)buffer);

			FillFuncList(
       (LPCFGDESC)SendMessage(hwndCategory, LB_GETITEMDATA, index, 0L));

			return 0;

    case WM_PAINT:
      if(extraLine)
        DrawExtraLine(extraLine);
      break;

		case WM_SIZE:
			cyClient = HIWORD(lParam);
			//fall through
		case WM_MOVE:
			GetWindowRect(hwnd, &rc);
			if(status & PAL_MINIMIZED)
				{
				turboPalLoc.x = rc.right;	//keep track of right edge
				turboPalLoc.y = rc.top;
				}
			else
				{
				palLoc.x = rc.left;
				palLoc.y = rc.top;
				}
			
			break;

		case WM_MOUSEMOVE:
			xpos = LOWORD(lParam);
			ypos = HIWORD(lParam);

			if(xpos>=nObjEnd-2 && xpos<=nObjEnd+6 &&	ypos>=0 && ypos<=cyClient)
				{
				bSizeIt = TRUE;
				SetCapture(hwnd);
				SetCursor(LoadCursor(hInst, "horzSplit"));
				}
			else if(!(wParam & MK_LBUTTON))
				{
				bSizeIt = FALSE;
				ReleaseCapture();
				SetCursor(LoadCursor(NULL, IDC_ARROW));
				}
			if(wParam & MK_LBUTTON && bSizeIt)
				{
				hDC = GetDC(hwnd);
				hPen = CreatePen(PS_DOT, 1, RGB(128,128,128));
				SetROP2(hDC, R2_NOTXORPEN);
				SelectObject(hDC, hPen);
				if(nLastX)
					{
					MoveToEx(hDC, nLastX, 0, NULL);
					LineTo(hDC, nLastX,	cyClient-GetSystemMetrics(SM_CYHSCROLL));
					}
				if(xpos > 0 && xpos < (nWidth - catWidth-4))
					{
					MoveToEx(hDC, xpos, 0, NULL);
					LineTo(hDC, xpos, cyClient-GetSystemMetrics(SM_CYHSCROLL));
					nLastX = xpos;
					}
				ReleaseDC(hwnd, hDC);
				DeleteObject(hPen);
				}
			break;

		case WM_LBUTTONDOWN:
			if(bSizeIt)
				{
				SetCapture(hwnd);
				SetCursor(LoadCursor(hInst, "horzSplit"));
        xpos = LOWORD(lParam);
				if(xpos > 0 && xpos < catBegin-4)
					{
					hDC = GetDC(hwnd);
					hPen = CreatePen(PS_DOT, 1, RGB(128,128,128));
					SelectObject(hDC, hPen);
					SetROP2(hDC, R2_NOTXORPEN);
					MoveToEx(hDC, xpos, 0, NULL);
					LineTo(hDC, xpos, cyClient-GetSystemMetrics(SM_CYHSCROLL));
					nLastX = xpos;
					ReleaseDC(hwnd, hDC);
  				DeleteObject(hPen);
					}
				}
			break;

		case WM_LBUTTONUP:
			if(bSizeIt)
				{
				nObjEnd = LOWORD(lParam);
				if(nObjEnd < 1)
					nObjEnd = 1;
				if(nObjEnd > catBegin-2)
					nObjEnd = catBegin-2;
				MoveWindow(hwndObject, -1, -1, nObjEnd,
				 nHeight-GetSystemMetrics(SM_CYCAPTION)+1, TRUE);
				MoveWindow(hwndFunction,nObjEnd+1,-1,catBegin-nObjEnd-1,
				 nHeight-GetSystemMetrics(SM_CYCAPTION)+1, TRUE);
        DrawExtraLine(nObjEnd);
				nLastX = 0;
				}
			ReleaseCapture();
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			break;


		case WM_COMMAND:
			{
			WORD		wID			= GET_WM_COMMAND_ID(wParam, lParam);
			WORD		wNotify	= GET_WM_COMMAND_CMD(wParam, lParam);

			if(wID == ID_LISTBOX3 && wNotify == LBN_SELCHANGE)
				{
				index = ListBox_GetCurSel(hwndCategory);
				FillFuncList(
         (LPCFGDESC)ListBox_GetItemData(hwndCategory, index));
				InvalidateRect(hwndFunction, NULL, TRUE);
				UpdateWindow(hwndFunction);
				InvalidateRect(hwndCatStatus, NULL, TRUE);
				UpdateWindow(hwndCatStatus);
				if(MDI::pHelpPtr->TestHelpFlag())
					{
					MDI::pHelpPtr->ClearHelpFlag();
          char catName[25];
					SendMessage(hwndCategory, LB_GETTEXT, index, (LONG)(LPSTR)catName);
					WinHelp(hwnd, MDI::pHelpPtr->GetHelpFileName(), HELP_CONTEXT,
					 MDI::pHelpPtr->GetCategoryContextId((LPSTR)catName));
					return TRUE;
					}
				}
			break;
			}

		case WM_SETCURSOR:
			if(MDI::pHelpPtr->TestHelpFlag())
				{
				SetCursor(MDI::pHelpPtr->GetHelpCursor());
				return TRUE;
				}
			break;

		case WM_DRAWITEM:
			lpdis = (LPDRAWITEMSTRUCT)lParam;
			if(wParam == ID_NUMCAT)
				{
				if(status & PAL_MINIMIZED)
					{
					//draw 3D button bar
					FillRect(lpdis->hDC, &rc, PROGSYSTEM::windowBrush);

					RECT rt;
					RECT textRt;
					int	delta;
					HDC hdcMem;
					HBITMAP bmp;
					int hold;
					BITMAP bmpInfo;
					HDC hdcMem2;
					int yDelta;

					GetObject(upArrow, sizeof(bmpInfo), &bmpInfo);

					bkgrnd = CreateSolidBrush(RGB(192,192,192));
					shadow = CreatePen(PS_SOLID, 1, RGB(128,128,128));

					GetClientRect(hwndNumCat, &rt);
					OffsetRect(&rt,0,-1); //just makes things look better
	 				bmp = CreateCompatibleBitmap(lpdis->hDC, rt.right+1, rt.bottom - rt.top + 1);
					hdcMem = CreateCompatibleDC(lpdis->hDC);
					hdcMem2 = CreateCompatibleDC(lpdis->hDC);
					I_ASSERT(bmp && hdcMem && hdcMem2);

					SelectObject(hdcMem2, upArrow);
					SetTextColor(hdcMem, PROGSYSTEM::normalTextColor);
					SetBkMode(hdcMem, TRANSPARENT);
					SelectObject(hdcMem, hFont);


					SelectObject(hdcMem, bmp);

					//draw the background
					FillRect(hdcMem, &rt, bkgrnd);

					delta = (rt.right + 1)/3;
					yDelta = ((rt.bottom - rt.top + 1) - (bmpInfo.bmHeight))/2;

					hold = rt.right;
					rt.right = rt.left + delta;
					textRt = rt;
					textRt.right = rt.right - 2 * bmpInfo.bmWidth;

					if(status & PAL_REGION1)
						apb_DoFrameDraw(hdcMem, &rt, GetStockPen(BLACK_PEN), shadow, GetStockPen(WHITE_PEN));
					else
						apb_DoFrameDraw(hdcMem, &rt, GetStockPen(BLACK_PEN), GetStockPen(WHITE_PEN), shadow);
					
					BitBlt(hdcMem, textRt.right, rt.top + yDelta, bmpInfo.bmWidth, bmpInfo.bmHeight,
						hdcMem2, 0, 0, SRCCOPY);
					DrawText(hdcMem, "Obj", 3, &textRt,	DT_CENTER|DT_VCENTER|DT_SINGLELINE);

					rt.left = rt.right;
					rt.right+=delta;
					textRt = rt;
					textRt.right = rt.right - 2 * bmpInfo.bmWidth;
					
					if(status & PAL_REGION2)
						apb_DoFrameDraw(hdcMem, &rt, GetStockPen(BLACK_PEN), shadow, GetStockPen(WHITE_PEN));
					else
						apb_DoFrameDraw(hdcMem, &rt, GetStockPen(BLACK_PEN), GetStockPen(WHITE_PEN), shadow);
					
					BitBlt(hdcMem, textRt.right, rt.top + yDelta, bmpInfo.bmWidth, bmpInfo.bmHeight,
						hdcMem2, 0, 0, SRCCOPY);
					DrawText(hdcMem, "Fnc", 3, &textRt,	DT_CENTER|DT_VCENTER|DT_SINGLELINE);

					rt.left = rt.right;
					rt.right = hold;
					textRt = rt;
					textRt.right = rt.right - 2 * bmpInfo.bmWidth;

					if(status & PAL_REGION3)
						apb_DoFrameDraw(hdcMem, &rt, GetStockPen(BLACK_PEN), shadow, GetStockPen(WHITE_PEN));
					else
						apb_DoFrameDraw(hdcMem, &rt, GetStockPen(BLACK_PEN), GetStockPen(WHITE_PEN), shadow);

					BitBlt(hdcMem, textRt.right, rt.top + yDelta, bmpInfo.bmWidth, bmpInfo.bmHeight,
						hdcMem2, 0, 0, SRCCOPY);
					DrawText(hdcMem, "Cat", 3, &textRt,	DT_CENTER|DT_VCENTER|DT_SINGLELINE);

					BitBlt(lpdis->hDC, 0, 0, rt.right+1, rt.bottom - rt.top + 1, hdcMem, 0, 0, SRCCOPY);

					DeleteDC(hdcMem);
					DeleteDC(hdcMem2);
					DeleteObject(bmp);
					DeleteObject(bkgrnd);
					DeleteObject(shadow);
					}
				else
					{
					GetWindowText(hwndNumCat, buffer, sizeof(buffer));

					SetTextColor(lpdis->hDC, PROGSYSTEM::normalTextColor);
					
					SetBkMode(lpdis->hDC, TRANSPARENT);

					rc = lpdis->rcItem;

					//offset since it seems to make the text appear better centered.
					OffsetRect(&rc, 0, -1);

					FillRect(lpdis->hDC, &rc, PROGSYSTEM::windowBrush);

					DrawText(lpdis->hDC, buffer, -1, &lpdis->rcItem,
						DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);
					}
				}
			else if(wParam == ID_CATSTAT)
				{
				RECT client;
				char buffer[128] = {"     "};
				int sel;

				GetClientRect(hwndCatStatus, &client);
			 	
			 	sel = ListBox_GetCurSel(hwndCategory);
				if(sel != LB_ERR)
					{
					if(ListBox_GetTextLen(hwndCategory, sel) < 
						(sizeof(buffer)- 1))	
						{
						ListBox_GetText(hwndCategory, sel, buffer);
						}
					else
						{
						I_ASSERT(FALSE);
						buffer[0] = '\0';
						}
					}
				else
					{
					buffer[0] = '\0';
					I_ASSERT(FALSE);
					}

				shadow = CreatePen(PS_SOLID, 1, RGB(128,128,128));
				bkgrnd = CreateSolidBrush(RGB(192,192,192));

				FillRect(lpdis->hDC, &client, bkgrnd);
				
				InflateRect(&client, -4, -4);
				apb_DoThinFrameDraw(lpdis->hDC, &client, GetStockPen(WHITE_PEN), shadow);
			 	SetTextColor(lpdis->hDC, RGB(0,0,0));
				SetBkMode(lpdis->hDC, TRANSPARENT);
			 		 
			 	DrawText(lpdis->hDC, buffer, lstrlen(buffer), &client,
			 		DT_NOPREFIX|DT_SINGLELINE|DT_CENTER|DT_VCENTER);
				
				DeleteObject(shadow);
				DeleteObject(bkgrnd);
				}
			else
				{
					
				if(lpdis->itemAction & (ODA_DRAWENTIRE | ODA_SELECT))	
					DrawEntireItem(lpdis);

	      if(lpdis->itemAction & ODA_FOCUS)
	        HandleFocus(lpdis);
				}
			return TRUE;

		case WM_MEASUREITEM:
			lpmis = (LPMEASUREITEMSTRUCT)lParam;
			lpmis->itemHeight = 55;
			break;


    case WM_NCACTIVATE:
      wParam = 0;
      if(PROGSYSTEM::TestActiveAppFlag())
        wParam = 1;
      break;

		case WM_ACTIVATE:
			{
			WORD		fActive	= GET_WM_ACTIVATE_STATE(wParam, lParam);
			HWND		hWnd		= GET_WM_ACTIVATE_HWND(wParam, lParam);

			if(fActive != WA_INACTIVE )
				if(creating)
					creating = FALSE;
				lastActiveWindow = hWnd;
			break;
			}

    case WM_KEYDOWN:
      switch(wParam)
        {
        case VK_DELETE:
          SendMessage(PROGSYSTEM::hWndFrame, WM_COMMAND,
											GET_WM_COMMAND_MPS(IDM_DODELETE, NULL, 0));
          return 0;

        }
      break;

		case WM_DESTROY:
			SetWindowLong(hwndNumCat, GWL_WNDPROC, (long)lpfnOldNumCat);

			break;
		}
	return DefWindowProc(hwnd, message, wParam, lParam);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	DrawEntireItem
//
// DESCRIPTION.:  draws each item for the owner draw list boxes
/////////////////////////////////////////////////////////////////////////////
void  PALETTE::DrawEntireItem(
	LPDRAWITEMSTRUCT lpdis)
	{
	char				 name[64];
	int     		 iconX, iconY;
	LPSERIUSICON pIcon;
	RECT    		 rc;
	COLORREF		 oldBkColor;
	COLORREF		 oldTColor;

	SetTextAlign(lpdis->hDC, TA_CENTER|TA_TOP);
	CopyRect(&rc, &lpdis->rcItem);
	if(lpdis->hwndItem == hwndObject)
		{
		pIcon = ((LPOBJDESC)lpdis->itemData)->GetIcons();
		lstrcpy(name, ((LPOBJDESC)lpdis->itemData)->pGetName());
		}
	if(lpdis->hwndItem == hwndFunction)
		{
		pIcon = ((LPAFUNCDESC)lpdis->itemData)->GetIcons();
		lstrcpy(name, ((LPAFUNCDESC)lpdis->itemData)->pGetName());
		}
	iconX = (rc.right-rc.left)/2 + rc.left - 16;
	iconY = rc.top+3;

	if((status & PAL_TURBOENABLED) && lpdis->itemState & ODS_SELECTED)
		{
		oldTColor = SetTextColor(lpdis->hDC, PROGSYSTEM::highlightTextColor);
		oldBkColor = SetBkColor(lpdis->hDC, PROGSYSTEM::highlightColor);
		}
	else
		{
		oldTColor = SetTextColor(lpdis->hDC, PROGSYSTEM::normalTextColor);
		oldBkColor = SetBkColor(lpdis->hDC, PROGSYSTEM::windowColor);
		}

	rc.right++;
  rc.bottom++;
	FillRect(lpdis->hDC, &rc, PROGSYSTEM::windowBrush);
  rc = lpdis->rcItem;
	
	pIcon->Draw(lpdis->hDC, iconX, iconY);
	TextOut(lpdis->hDC, (rc.right-rc.left)/2 + rc.left, rc.top+35, name,
	 lstrlen(name));

	SetTextColor(lpdis->hDC, oldTColor);
	SetBkColor(lpdis->hDC, oldBkColor);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	HandleFocus
//
// DESCRIPTION.:  draws box around item with focus in the list boxes
/////////////////////////////////////////////////////////////////////////////
void  PALETTE::HandleFocus(
	LPDRAWITEMSTRUCT	lpdis)
  {
	RECT	      rc;

	if(status & PAL_TURBOENABLED)
		{
		CopyRect(&rc, &lpdis->rcItem);
		//shift rect down because it makes the stupid thing look better
		OffsetRect(&rc, 0, 1);
		DrawFocusRect(lpdis->hDC, &rc);
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	ObjProc
//
// DESCRIPTION.:  subclass routine for the objects listbox
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK PALETTE::ObjProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	static BOOL			 dblClkFlag=FALSE;
//	OBJLIST *		 pObjList;
	PALETTE *		 pPalPtr;
	static short		 beep=0;
	int						   i, j;
	POINT 					 point;
	HDC							 hDC;
	BOOL             aliasFlag;
	LPSTR						 newName;
  PROJECT *		 projectPtr;
	SUBJECT * 		 subjectPtr;
	HWND overWindow;

	if(PALETTE::list == NULL)
		return CallWindowProc(
		 (FARPROC)PALETTE::lpfnOldObj, hwnd, message, wParam, lParam);

	pPalPtr = (PALETTE  *)PALETTE::list->GetObject(GetParent(hwnd));
	switch (message)
		{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			i = (int)SendMessage(hwnd, LB_GETTOPINDEX, 0, 0L);
			j = (LOWORD(lParam)/pPalPtr->objWidth+i);
			if(j >= SendMessage(hwnd, LB_GETCOUNT, 0, 0L))
				break;
			pAddObjDef = (LPOBJDESC)SendMessage(hwnd, LB_GETITEMDATA, j, 0L);
			if(message == WM_RBUTTONDOWN || MDI::pHelpPtr->TestHelpFlag())
				{
				if(!dropType)//make sure not in middle of drag and drop
					{
					MDI::pHelpPtr->ClearHelpFlag();
					WinHelp(hwnd, pAddObjDef->pGetHelpFileName(), HELP_CONTEXT,
				 	(DWORD)pAddObjDef->GetUID());
					}
				return TRUE;
				}
			else
				{
				hDC = GetDC(hwnd);
				SetCapture(hwnd);
        if(!dropCursor)
					dropCursor = pPalPtr->MakeTheCursor(hDC, pAddObjDef->GetIcons());
				SetCursor(dropCursor);
				dropType = APB_DRAGANDDROP_OBJECT;
				ReleaseDC(hwnd, hDC);
				}
			break;

		case WM_MOUSEMOVE:
			if((wParam & MK_LBUTTON) && dropType)
				{
				A_MAKEPOINT(point, lParam); // SKJ
        ClientToScreen(hwnd, (LPPOINT)&point);
				overWindow = WindowFromPoint(point);
				if(lastValidDropWindow)
					SendMessage(lastValidDropWindow, APB_CLEAROBJECTHIGHLIGHT, 0,
						MAKELPARAM(lastDropPoint.x, lastDropPoint.y));

				if(overWindow != pPalPtr->hwndPalette &&
					pPalPtr->hwndPalette != GetParent(overWindow))
					{
					if(SendMessage(overWindow, dropType, 0, 0L) == APB_DRAGANDDROP_OK)
						{
						SetCursor(dropCursor);

						//adjust the point to be the upper left corner
						point.x-=15;
						point.y-=15;

						//Fix changing lastValidDropWindow to overWindow - lastValidDropWindow
            //at this point is not necessarily the current window we're over.
						SendMessage(overWindow, APB_HIGHLIGHTOBJECTINSERT, 0,
							MAKELPARAM(point.x, point.y));
						//end of fix

						lastDropPoint = point;
						}
					else
						{
						SetCursor(LoadCursor(PROGSYSTEM::hInst, "INVALIDDROP"));
						overWindow = NULL;
						}
					}
				else
        	{
					SetCursor(dropCursor);
					overWindow = NULL;
          }

        lastValidDropWindow = overWindow;
				return 0;
				}
			break;

		case WM_RBUTTONUP:
			if(dropType)
				{
				//kill the drop without adding
				if(lastValidDropWindow)
					{
					SendMessage(lastValidDropWindow, APB_CLEAROBJECTHIGHLIGHT, 0,
						MAKELPARAM(lastDropPoint.x, lastDropPoint.y));
					}	
				ReleaseCapture();
				dropType = 0;
				lastValidDropWindow = NULL;

				if(dropCursor)
					{
					DestroyCursor(dropCursor);
					dropCursor = NULL;
					}

			SetCursor(LoadCursor(NULL, IDC_ARROW));
				}
			break;
		
		case WM_LBUTTONUP:
			if(dropType && !dblClkFlag)
				ReleaseCapture();

			if(lastValidDropWindow)
				{
				subjectPtr = (SUBJECT *)SendMessage(lastValidDropWindow, APB_GETSUBJECT, 0, 0L);
				if(HIWORD(subjectPtr) == 0)
					{
					MessageBeep(0);
					beep++;
					}
				else
					{
					//we should be a valid subject
					GetCursorPos(&point);
	//				pBasePtr = BASEWINDOW::GetMouseWindow(point);
 //					if(pBasePtr && pBasePtr->TestSubjectFlag())
#ifdef DEMO
					//only add an object if the number of objects in the subject
					//had not yet reached the maximum allowed in the demo.
					if(subjectPtr->GetObjectCount() >= DEMO_MAXOBJECTS ||
						subjectPtr->GetParentProject()->GetObjectCount() >=  DEMO_MAXPROJECTOBJECTS)
						{
						SendMessage(lastValidDropWindow, APB_CLEAROBJECTHIGHLIGHT, 0,
							MAKELPARAM(lastDropPoint.x, lastDropPoint.y));
						dropType = 0;
						lastValidDropWindow = NULL;
						demo_ObjectMessage();
						return 0;
						}
#endif
					SetCapture(hwnd);
					SetCursor(LoadCursor(NULL, IDC_WAIT));
					beep = 0;

					if(GetKeyState(VK_MENU) < 0)
						{
						aliasFlag = TRUE;
						newName = DEFAULT_ALIAS_NAME;
						}
					else
						{
						aliasFlag = FALSE;
						newName = NULL;
						}

					A_MAKEPOINT(point, lParam);
					//adjust point to be upper left corner of cursor
					point.x-=15;
					point.y-=15;

					ClientToScreen(hwnd, (LPPOINT)&point);
					subjectPtr->AddObjInst(pAddObjDef, aliasFlag, (LPPOINT)&point, newName, TRUE);
					//passing in TRUE means to use the location to insert
					ReleaseCapture();
					}
				}
			else if(dblClkFlag)
				{
				dblClkFlag = FALSE;
				projectPtr = PROGSYSTEM::GetCurrentProject();

				if(projectPtr && (NULL != (subjectPtr = projectPtr->GetActiveSubject())))
					{
#ifdef DEMO
					//only add an object if the number of objects in the subject
					//had not yet reached the maximum allowed in the demo.
					if(subjectPtr->
						GetObjectCount() >= DEMO_MAXOBJECTS || 
							projectPtr->GetObjectCount() >= DEMO_MAXPROJECTOBJECTS)
						{
 						ReleaseCapture();
						dropType = 0;
						lastValidDropWindow = NULL;
            demo_ObjectMessage();
           	return 0;
            }
#endif
         // put up the hourglass ...
          SetCapture(hwnd);
          SetCursor(LoadCursor(NULL, IDC_WAIT));

					if(GetKeyState(VK_MENU) < 0)
          	{
						aliasFlag = TRUE;
						newName = DEFAULT_ALIAS_NAME;
            }
					else
          	{
            aliasFlag = FALSE;
						newName = NULL;
						}


					A_MAKEPOINT(point, lParam);
					ClientToScreen(hwnd, (LPPOINT)&point);
					//adjust point to be upper left corner of cursor
					point.x-=15;
					point.y-=15;

				 //pass NULL for the location to do a default add.
					subjectPtr->AddObjInst(pAddObjDef, aliasFlag, (LPPOINT)&point, newName, FALSE);
					//passing in FALSE means to do a default add.  Still pass in the point since
          //this locatio is needed for our animation.

					ReleaseCapture();
					}
				 else
					MessageBeep(0);
				}
			else if(dropType)
				{
				dropType = 0;
				//if we are over the palette, then dont message beep
				A_MAKEPOINT(point, lParam);
				if(point.x < 0 || point.y < 0 || point.x > pPalPtr->nWidth ||
					point.y > pPalPtr->nHeight)
					{
					MessageBeep(0);
					beep++;
          }
        }

			if(beep > 2)
				{
				beep = 0;
				MessageBox(NULL, "Please place the object in a subject", NULL,
				 MB_ICONINFORMATION | MB_TASKMODAL | MB_OK);
				}

			SetCursor(LoadCursor(NULL, IDC_ARROW));

			//Fix do destroy the cursor we created.
			if(dropCursor)
				{
				DestroyCursor(dropCursor);
				dropCursor = NULL;
				}
			//end of fix

			dropType = 0;
			lastValidDropWindow = NULL;
      
      if(!(pPalPtr->status & PAL_TURBOENABLED))
      	return TRUE;
        
      break;
      
		case WM_LBUTTONDBLCLK:
			dblClkFlag = TRUE;
      break;

		case WM_KEYUP:
			if((pPalPtr->status & PAL_TURBOENABLED) && 
				!(pPalPtr->status & PAL_MINIMIZED) && !dropType && (wParam == VK_TAB))
				{
				SetFocus(pPalPtr->hwndFunction);
				}

			if(dropType && (wParam == VK_ESCAPE))
				return SendMessage(hwnd, WM_RBUTTONUP, NULL, NULL);

			//if(!(pPalPtr->status & PAL_TURBOENABLED))
			//	return 0;
			
			break;

    case WM_KEYDOWN:
      switch(wParam)
        {
        case VK_DELETE:
          SendMessage(PROGSYSTEM::hWndFrame, WM_COMMAND,
											GET_WM_COMMAND_MPS(IDM_DODELETE, NULL, 0));
          return 0;

        case VK_F1:
          if(GetKeyState(VK_SHIFT))
            SendMessage(PROGSYSTEM::hWndFrame, WM_COMMAND,
											GET_WM_COMMAND_MPS(ID_HELPCURSOR, NULL, 0));
          else
						WinHelp(PROGSYSTEM::hWndFrame, MDI::pHelpPtr->GetHelpFileName(), HELP_INDEX, 0L);
          break;

        case VK_ESCAPE:
          SendMessage(PROGSYSTEM::hWndFrame, WM_COMMAND,
											GET_WM_COMMAND_MPS(ID_ESCAPE, NULL, 0));
          break;
        }

			if(!(pPalPtr->status & PAL_TURBOENABLED))
				return 0;

      break;
		}
	return CallWindowProc((FARPROC)PALETTE::lpfnOldObj, hwnd, message, wParam, lParam);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	FuncProc
//
// DESCRIPTION.:  subclass routine for the functions listbox
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK PALETTE::FuncProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	static BOOL 				dblClkFlag = FALSE;
	static short				beep=0;
	int									i, j;
	HDC									hDC;
	PALETTE *				pPalPtr;
	POINT								loc;
	LPSUBJECT 					subjectPtr;
	LPPROJECT						projectPtr;
	RECT								rt;

  HWND overWindow;

	if(PALETTE::list == NULL)
		return CallWindowProc(
		 (FARPROC)PALETTE::lpfnOldFunc, hwnd, message, wParam, lParam);

	pPalPtr = (PALETTE  *)PALETTE::list->GetObject(GetParent(hwnd));
	switch (message)
		{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			i = (int)SendMessage(hwnd, LB_GETTOPINDEX, 0, 0L);
			j = (LOWORD(lParam)/pPalPtr->funcWidth+i);
			if(j >= SendMessage(hwnd, LB_GETCOUNT, 0, 0L))
				break;
			pAddFuncDef = (LPAFUNCDESC)SendMessage(hwnd, LB_GETITEMDATA, j, 0L);
			if(message == WM_RBUTTONDOWN || MDI::pHelpPtr->TestHelpFlag())
				{
				if(!dropType) //make sure not in middle of drag and drop
					{
					MDI::pHelpPtr->ClearHelpFlag();
					WinHelp(hwnd,pAddFuncDef->pGetHelpFileName(),HELP_CONTEXT,(DWORD)pAddFuncDef->GetUID());
					}
				return TRUE;
				}
			else
				{
				hDC = GetDC(hwnd);
				SetCapture(hwnd);
        if(!dropCursor)
					dropCursor = pPalPtr->MakeTheCursor(hDC, pAddFuncDef->GetIcons());
				SetCursor(dropCursor);
				dropType = APB_DRAGANDDROP_FUNCTION;
				ReleaseDC(hwnd, hDC);
				}
			break;

		case WM_MOUSEMOVE:
			if((wParam & MK_LBUTTON) && dropType)
				{
			  A_MAKEPOINT(loc, lParam);
				ClientToScreen(hwnd, (LPPOINT)&loc);
				overWindow = WindowFromPoint(loc);
				if(overWindow != pPalPtr->hwndPalette &&
					pPalPtr->hwndPalette != GetParent(overWindow))
					{
					if(SendMessage(overWindow, dropType, 0, 0L) == APB_DRAGANDDROP_OK)
						SetCursor(dropCursor);
					else
						{
            SetCursor(LoadCursor(PROGSYSTEM::hInst, "INVALIDDROP"));
						overWindow = NULL;
            }
					}
				else
					{
          SetCursor(dropCursor);
					overWindow = NULL;
					}

				lastValidDropWindow = overWindow;
        return 0;
				}
			break;

		case WM_LBUTTONUP:
      if(dropType && !dblClkFlag)
      	ReleaseCapture();

			if(lastValidDropWindow)
      	{
				subjectPtr = (LPSUBJECT)SendMessage(lastValidDropWindow, APB_GETSUBJECT, 0, 0L);
				if(HIWORD(subjectPtr) == 0)
					{
					MessageBeep(0);
					beep++;
					}
				else
					{
					A_MAKEPOINT(loc, lParam);
					SetCapture(hwnd);
					SetCursor(LoadCursor(NULL, IDC_WAIT));
					beep = 0;
					//adjust to allow for hotspot being in the middle
					loc.x-=15;
					loc.y-=15;
					ClientToScreen(hwnd, (LPPOINT)&loc);

					((LPSUBJECTWINDOW)subjectPtr->GetWindowPtr())->AddFunction(pAddFuncDef->GetUID(), loc);
					pPalPtr->ActivateOldWindow();
					ReleaseCapture();
					}
				}
			else if(dblClkFlag)
				{
				dblClkFlag = FALSE;
				
				if(pPalPtr->status & PAL_TURBOENABLED)
					{
					projectPtr = PROGSYSTEM::GetCurrentProject();

					if(projectPtr && (NULL != (subjectPtr = projectPtr->GetActiveSubject())))
						{
	         // put up the hourglass ...
	          SetCapture(hwnd);
	          apb_ShowWaitCursor();

						A_MAKEPOINT(loc, lParam);
						beep = 0;
						//adjust to allow for hotspot being in the middle
						loc.x-=15;
						loc.y-=15;
						ClientToScreen(hwnd, (LPPOINT)&loc);
						rt.left = loc.x;
						rt.top = loc.y;
						rt.right = rt.left + 32;
						rt.bottom = rt.top + 32;

						((LPSUBJECTWINDOW)subjectPtr->GetWindowPtr())
							->AddFunctionExt(pAddFuncDef->GetUID(), &rt);

						apb_RestoreOldCursor();
						ReleaseCapture();
						}
					 else
						MessageBeep(0);
					}
				}
			else if(dropType)
				{
				A_MAKEPOINT(loc, lParam); // SKJ
				if(loc.x < 0 || loc.y < 0 || loc.x > pPalPtr->nWidth || loc.y > pPalPtr->nHeight)
					{
					MessageBeep(0);
					beep++;
          }
        }
			if(beep > 2)
				{
				beep = 0;
				MessageBox(NULL, "Please place the function in a subject", NULL,
				 MB_ICONINFORMATION | MB_TASKMODAL | MB_OK);
				}
			SetCursor(LoadCursor(NULL, IDC_ARROW));

			//Fix for making sure the cursor is destroyed
			if(dropCursor)
				{
				DestroyCursor(dropCursor);
				dropCursor = NULL;
				}
      //end fix

			ReleaseCapture();
			dropType = 0;
			lastValidDropWindow = NULL;
      if(!(pPalPtr->status & PAL_TURBOENABLED))
      	return TRUE;
        
			break;

		case WM_LBUTTONDBLCLK:
			dblClkFlag = TRUE;
      break;

		case WM_RBUTTONUP:
			if(dropType)
				{
				//kill the drop without adding
				if(lastValidDropWindow)
					{
					SendMessage(lastValidDropWindow, APB_CLEAROBJECTHIGHLIGHT, 0,
						MAKELPARAM(lastDropPoint.x, lastDropPoint.y));
					}	
				ReleaseCapture();
				dropType = 0;
				lastValidDropWindow = NULL;
				if(dropCursor)
					{
					DestroyCursor(dropCursor);
					dropCursor = NULL;
					}

			SetCursor(LoadCursor(NULL, IDC_ARROW));
				}
			break;

		case WM_KEYUP:
			if((pPalPtr->status & PAL_TURBOENABLED) &&
				!(pPalPtr->status & PAL_MINIMIZED) && !dropType && (wParam == VK_TAB))
				{
				SetFocus(pPalPtr->hwndCategory);
				}

			if(dropType && (wParam == VK_ESCAPE))
				return SendMessage(hwnd, WM_RBUTTONUP, NULL, NULL);

			if(!(pPalPtr->status & PAL_TURBOENABLED))
				return 0;

			break;

		case WM_KEYDOWN:
			switch(wParam)
				{
				case VK_DELETE:
					SendMessage(PROGSYSTEM::hWndFrame, WM_COMMAND,
											GET_WM_COMMAND_MPS(IDM_DODELETE, NULL, 0));
					return 0;

				case VK_F1:
					if(GetKeyState(VK_SHIFT))
						SendMessage(PROGSYSTEM::hWndFrame, WM_COMMAND,
											GET_WM_COMMAND_MPS(ID_HELPCURSOR, NULL, 0));
          else
						WinHelp(PROGSYSTEM::hWndFrame, MDI::pHelpPtr->GetHelpFileName(), HELP_INDEX, 0L);
          break;

        case VK_ESCAPE:
          SendMessage(PROGSYSTEM::hWndFrame, WM_COMMAND,
											GET_WM_COMMAND_MPS(ID_ESCAPE, NULL, 0));
          break;
        }

			if(!(pPalPtr->status & PAL_TURBOENABLED))
				return 0;

      break;
		}
	return CallWindowProc((FARPROC)PALETTE::lpfnOldFunc, hwnd, message, wParam, lParam);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CategoryProc
//
// DESCRIPTION.:  subclass routine for the category listbox
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK PALETTE::CategoryProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{

	PALETTE *				pPalPtr;

	if(PALETTE::list == NULL)
		return CallWindowProc(
		 (FARPROC)PALETTE::lpfnOldCat, hwnd, message, wParam, lParam);

	pPalPtr = (PALETTE  *)PALETTE::list->GetObject(GetParent(hwnd));
	//just disable the help for the categories
	//until we can evaluate how the help should work.

/*  char      catName[25];
	int			  i, index;
	POINT		  point;
	RECT      rect;	 

	switch(message)
		{
		case WM_RBUTTONDOWN:
			/*
			A_MAKEPOINT(point, lParam);
			index = (int)SendMessage(hwnd, LB_GETTOPINDEX, 0, 0L);
			for(i=index;i<SendMessage(hwnd, LB_GETCOUNT, 0, 0L);i++)
				{
				SendMessage(hwnd, LB_GETITEMRECT, i, (LONG)&rect);
				if(PtInRect(&rect, point))
					{
					SendMessage(hwnd, LB_GETTEXT, i, (LONG)(LPSTR)catName);
					WinHelp(hwnd, MDI::pHelpPtr->GetHelpFileName(), HELP_CONTEXT,
					 MDI::pHelpPtr->GetCategoryContextId((LPSTR)catName));
				  return TRUE;
          }
        }	

       break;
    }	 */
	
	switch(message)
		{
		case WM_KEYUP:

			if(!(pPalPtr->status & PAL_MINIMIZED) && (wParam == VK_TAB))
				{
				SetFocus(pPalPtr->hwndObject);
				}

			break;
		}

	return CallWindowProc((FARPROC)PALETTE::lpfnOldCat, hwnd, message, wParam, lParam);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	CreatePaletteWindow
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
void PALETTE::CreatePaletteWindow(
	HWND parent)
	{
	DWORD style = WS_BORDER|WS_VISIBLE|WS_OVERLAPPED|WS_SYSMENU;

	if(status & PAL_TURBOENABLED)
		style |= WS_MINIMIZEBOX;

	CreateWindow("Palette", "Object & Function Palette", style,
	 nLeft, nTop,	nWidth,	nHeight, parent, NULL, hInst, (LPSTR)this);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	FillFuncList
//
// DESCRIPTION.:  puts in the items of the functions listbox for the cate-
//								gory selected
/////////////////////////////////////////////////////////////////////////////
void PALETTE::FillFuncList(
	LPCFGDESC  pConfig)
	{
	LPAFUNCDESC funcDef = 0;
	int			 index;

  if (pConfig != 0 && (long)pConfig != LB_ERR)  //probably because we have no config files.
    {
		funcPopupPtr->PurgeList();

	  SendMessage(hwndFunction, LB_RESETCONTENT, 0, 0L);
	  SendMessage(hwndFunction, WM_SETREDRAW, FALSE, 0L);
	  while (0 != (funcDef = pConfig->pNextFunctionDesc(funcDef)))
		  {
		  index = SendMessage(hwndFunction, LB_ADDSTRING, 0,
		   (LONG)funcDef->pGetName());
		  SendMessage(hwndFunction, LB_SETITEMDATA, index, (LONG)funcDef);

			funcPopupPtr->AppendItem(funcDef->pGetName(), 
				(long)funcDef, NO_ORDINAL);

		  }
	  SendMessage(hwndFunction, WM_SETREDRAW, TRUE, 0L);
    }

  SetFuncColumnWidth();

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SetObjColumnWidth
//
// CLASS.......:  PALETTE
//
/////////////////////////////////////////////////////////////////////////////
void PALETTE::SetObjColumnWidth()
	{
	HDC				hDC;
	int 			count, i, length;
	LPOBJDESC objDef;
	char			name[50];

	hDC = GetDC(hwndObject);
	SelectObject(hDC, hFont);
	count = (int)SendMessage(hwndObject, LB_GETCOUNT, 0, 0L);
	objWidth = 32 + 3;  //set to minimum of icon width + 3 pixels
	for (i=0; i<count; i++)
		{
    SIZE size;
		objDef = (LPOBJDESC)SendMessage(hwndObject, LB_GETITEMDATA, i, 0L);
		lstrcpy(name, objDef->pGetName());
		GetTextExtentPoint32(hDC, name, lstrlen(name), &size);
		length = size.cx;
		length += 3;
		if(length > objWidth)
			objWidth = length;
		}
	ReleaseDC(hwndObject, hDC);
	SendMessage(hwndObject, LB_SETCOLUMNWIDTH, objWidth, 0L);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	SetFuncColumnWidth
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
void PALETTE::SetFuncColumnWidth()
	{
	HDC				 hDC;
	int 			 count, i, length;
	LPAFUNCDESC funcDef;
	char			 name[50];

	hDC = GetDC(hwndFunction);
	SelectObject(hDC, hFont);
	count = (int)SendMessage(hwndFunction, LB_GETCOUNT, 0, 0L);
	funcWidth = 32 + 3;  //set to minimum of icon width + 3 pixels
	for (i=0; i<count; i++)
		{
		SIZE size;
		funcDef = (LPAFUNCDESC)SendMessage(hwndFunction, LB_GETITEMDATA, i, 0L);
		lstrcpy(name, funcDef->pGetName());
		GetTextExtentPoint32(hDC, name, lstrlen(name), &size);
		length = size.cx;
		length += 3;
		if(length > funcWidth)
			funcWidth = length;
		}
	ReleaseDC(hwndFunction, hDC);
	SendMessage(hwndFunction, LB_SETCOLUMNWIDTH, funcWidth, 0L);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	show
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
void PALETTE::show(
	void)
	{
	ShowWindow(hwndPalette, SW_SHOW);
  SendMessage(hwndPalette, WM_NCACTIVATE, 1, 0L);
  DrawExtraLine(extraLine);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	hide
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
void PALETTE::hide(
	void)
	{
	//Must set the focus to the frame window otherwise, hiding the palette
	//might cause some mystery window to become active and results in
	//not being able to Alt+Tab to VAB.
	SetFocus(PROGSYSTEM::hWndFrame);
	ShowWindow(hwndPalette, SW_HIDE);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	MakeTheCursor
//
// DESCRIPTION.:  creates a cursor to be used for dragging an icon from the
//								palette
/////////////////////////////////////////////////////////////////////////////
HCURSOR PALETTE::MakeTheCursor(
	HDC			 		 hDC,
	LPSERIUSICON pIcon)
	{
	HDC			hdcMem;
	HBITMAP	hBitmap;
	BYTE		byIMAGEBits[128], byMASKBits[128];

	//the zero bits will be the and mask and the xor mask will contain the
	//image.  This way, the cursor will appear as a white and black image
	//reguardless of the background color.

	//scan convert the icon moving in from the left looking for the first
	//visible color.  Everything up to the color will be transparent.
	//Then do the same thing from the right side.
	//If no colors are found in the entire icon, make the whole thing inverted.

	hdcMem = CreateCompatibleDC(hDC);
	hBitmap = CreateBitmap(32, 32, 1, 1, NULL);
	SelectObject(hdcMem, hBitmap);
	pIcon->DrawImage(hdcMem, 0, 0);
	GetBitmapBits(hBitmap, 128L, (LPSTR)byIMAGEBits);

	pIcon->DrawMask(hdcMem, 0, 0);
	GetBitmapBits(hBitmap, 128L, (LPSTR)byMASKBits);

	DeleteDC(hdcMem);
	DeleteObject(hBitmap);

	return CreateCursor(hInst, 15, 15, 32, 32,
	 (LPSTR)byMASKBits,(LPSTR)byIMAGEBits);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ActivateOldWindow
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
void PALETTE::ActivateOldWindow()
	{
	if(lastActiveWindow)
		{
		SetActiveWindow(lastActiveWindow);
		lastActiveWindow=NULL;
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  DrawExtraLine
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
void PALETTE::DrawExtraLine(
  int x)
  {
  HDC hDC = GetDC(hwndPalette);
	HPEN oldPen;

	SelectObject(hDC, GetStockObject(BLACK_PEN));
	MoveToEx(hDC, x, 0, NULL);
	LineTo(hDC, x, nHeight);
	//now draw a line the color of the windowBackGround just to the left
	//of the black line to make sure that that area will always be the color
	//of the windowbackground
	if(x>0)
		{
		oldPen = SelectPen(hDC, PROGSYSTEM::windowPen);
		MoveToEx(hDC, x-1,0, NULL);
		LineTo(hDC, x-1, nHeight);
		SelectPen(hDC, oldPen);
    }
	extraLine = x;
	ReleaseDC(hwndPalette, hDC);
	}

//Fills rt with the size that the cursor would be based on the refPt.
//refPt is assumed to be the upper left corner.
void PALETTE::GetCursorRect(
	LPRECT rt,
	POINT refPt)
	{
	rt->left = refPt.x;
	rt->top = refPt.y;
	rt->right = rt->left + 31;
	rt->bottom = rt->top + 31;
	}


void PALETTE::PopUpCategoryList(
	POINT startPoint)
	{
  LPCFGDESC           pConfig;
	DWORD								result;

	//show the popup
	//the second parameter will be unused because of the type
	//of popup
	if(categoryPopupPtr->ShowPopup(startPoint, 0))
		{
		pConfig = (LPCFGDESC)categoryPopupPtr->GetItemData();

		if(pConfig)
			{
			//change the selection;
			result = SendMessage(hwndCategory, LB_SELECTSTRING, -1, 
				(long)pConfig->pGetFuncGroupName());

			if(result != (DWORD) LB_ERR)
				{
				//cause the function list to re-select.
				SendMessage(hwndPalette, WM_COMMAND,
										GET_WM_COMMAND_MPS(ID_LISTBOX3, hwndFunction, LBN_SELCHANGE));
				}
			}
		}

	}


void PALETTE::PopupObjList(
	POINT startPoint)
	{
  LPOBJDESC           pConfig;
	LPSUBJECT						subjectPtr = NULL;
	LPPROJECT						projectPtr;
	POINT								point;
	int									result;
	RECT								selLocation;
	HDC									hdc;
	//show the popup
	//the second parameter will be unused because of the type
	//of popup
	if((result = objPopupPtr->ShowPopupExt(startPoint, 0, &selLocation))
		 != POPUP_EXT_NOSELECTION)
		{
		pConfig = (LPOBJDESC)objPopupPtr->GetItemData();
		
		if(result == POPUP_EXT_NORMALSELECTION)
			{

			if(pConfig)
				{

				projectPtr = PROGSYSTEM::GetCurrentProject();
				if(projectPtr)
					subjectPtr = projectPtr->GetActiveSubject();

					if(projectPtr && subjectPtr)
					{	

	#ifdef DEMO
						//only add an object if the number of objects in the subject
						//had not yet reached the maximum allowed in the demo.
						if(subjectPtr->
							GetObjectCount() >= DEMO_MAXOBJECTS || 
								projectPtr->GetObjectCount() >= DEMO_MAXPROJECTOBJECTS)
							{
	            demo_ObjectMessage();
	           	return;
	            }
	#endif
	 		
					 apb_ShowWaitCursor();
					 point.x = selLocation.left;
					 point.y = selLocation.top;

					 //pass NULL for the location to do a default add.
						subjectPtr->AddObjInst(pConfig, FALSE/*alias flag*/, (LPPOINT)&point, pConfig->pGetName(), FALSE, &selLocation);
						//passing in FALSE means to do a default add.  Still pass in the point since
	          //this locatio is needed for our animation.
				 	 apb_RestoreOldCursor();
				 }
				}
			}
		else if(result == POPUP_EXT_DRAGSELECTION)
			{
			//fool the palette into thinking we started dragging at the palette
			if(pConfig)
				{
				pAddObjDef = pConfig;
				hdc = GetDC(hwndObject);
				SetCapture(hwndObject);
				if(status & PAL_OBJICONS)
					dropCursor = MakeTheCursor(hdc, pConfig->GetIcons());
				else
					dropCursor = MakeTheNameCursor(hdc, pConfig->pGetName());
				SetCursor(dropCursor);
				dropType = APB_DRAGANDDROP_OBJECT;
				ReleaseDC(hwndObject, hdc);
				}
			}
		else
			I_ASSERT(FALSE);  //bad if we reach here.
		}

	}


void PALETTE::PopupFuncList(
	POINT startPoint)
	{
  LPAFUNCDESC          pConfig;
	int									result;
	RECT								selLocation;
	HDC									hdc;
	LPPROJECT						projectPtr;
	LPSUBJECT						subjectPtr;



	//show the popup
	//the second parameter will be unused because of the type
	//of popup
	if((result = funcPopupPtr->ShowPopupExt(startPoint, 0, &selLocation))
		 != POPUP_EXT_NOSELECTION)
		{
		pConfig = (LPAFUNCDESC)funcPopupPtr->GetItemData();
		
		if(result == POPUP_EXT_NORMALSELECTION)
			{

		 	if(pConfig)
			 	{
				projectPtr = PROGSYSTEM::GetCurrentProject();
				if(projectPtr)
					subjectPtr = projectPtr->GetActiveSubject();

				if(projectPtr && subjectPtr)
					{
					apb_ShowWaitCursor();	
					((LPSUBJECTWINDOW)subjectPtr->GetWindowPtr())->
						AddFunctionExt(pConfig->GetUID(), &selLocation);
			 		apb_RestoreOldCursor();
			 		}
			 	}	
			}
		else if(result == POPUP_EXT_DRAGSELECTION)
			{
			//fool the palette into thinking we started dragging at the palette
			if(pConfig)
				{
				pAddFuncDef = pConfig;
				hdc = GetDC(hwndFunction);
				SetCapture(hwndFunction);
        if(!dropCursor)
					{
					if(status & PAL_FUNCICONS)
						dropCursor = MakeTheCursor(hdc, pConfig->GetIcons());
					else
						dropCursor = MakeTheNameCursor(hdc, pConfig->pGetName());
					}
				SetCursor(dropCursor);
				dropType = APB_DRAGANDDROP_FUNCTION;
				ReleaseDC(hwndFunction, hdc);
				}
			}
		else
			I_ASSERT(FALSE);  //bad if we reach here.
		}

	}


LRESULT CALLBACK PALETTE::NumCatProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	long result = 1;
	PALETTE * pPalPtr;
	POINT pt;
	int section;
	RECT rt;


	pPalPtr = (PALETTE  *)PALETTE::list->GetObject(GetParent(hwnd));
	
	I_ALWAYSASSERT(pPalPtr, return result);

	//prevent the static windows from sending mouse messages back to the parent.
	//we also don't want to erase the background since the entire client area will
	//be drawn over anyway.
	if((message < WM_MOUSEFIRST || message > WM_MOUSELAST) && (message !=
  	WM_ERASEBKGND))
		result = CallWindowProc((FARPROC)lpfnOldNumCat, hwnd, message, wParam, lParam);
	else if(message == WM_LBUTTONDOWN)
		{
		GetClientRect(hwnd, &rt);
		A_MAKEPOINT(pt, lParam);

		if(pPalPtr->status & PAL_MINIMIZED)
			section = (pt.x * 3) / rt.right;	//sections go from 0 to 2
		else
			section = 2; //force category popup

		ClientToScreen(hwnd, (LPPOINT)&pt);

		InvalidateRect(pPalPtr->hwndNumCat, NULL, FALSE);

		if(section == 0)
			{
			pPalPtr->status |= PAL_REGION1;
			UpdateWindow(pPalPtr->hwndNumCat);
			pPalPtr->PopupObjList(pt);
			}
		else if(section == 1)
			{
			pPalPtr->status |= PAL_REGION2;
			UpdateWindow(pPalPtr->hwndNumCat);
			pPalPtr->PopupFuncList(pt);
			}
		else
			{
			pPalPtr->status |= PAL_REGION3;
			pPalPtr->PopUpCategoryList(pt);
			UpdateWindow(pPalPtr->hwndNumCat);
			}
		pPalPtr->status &= ~PAL_ALLREGIONS;
		InvalidateRect(pPalPtr->hwndNumCat, NULL, FALSE);
		UpdateWindow(pPalPtr->hwndNumCat);
		}

	return result;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	MakeTheNameCursor
//
// DESCRIPTION.:  creates a cursor to be used for dragging an icon from the
//								palette
/////////////////////////////////////////////////////////////////////////////
HCURSOR PALETTE::MakeTheNameCursor(
	HDC			 		 hDC,
	LPSTR 			 theName)
	{
	HDC			hdcMem;
	HBITMAP	hBitmap;
	BYTE		byIMAGEBits[128], byMASKBits[128];
	RECT		rt;
	RECT		text;
	UINT flags;
	//the zero bits will be the and mask and the xor mask will contain the
	//image.  This way, the cursor will appear as a white and black image
	//reguardless of the background color.

	//scan convert the icon moving in from the left looking for the first
	//visible color.  Everything up to the color will be transparent.
	//Then do the same thing from the right side.
	//If no colors are found in the entire icon, make the whole thing inverted.

	SetRect(&rt, 0,0,32,32);
	text = rt;
	
	hdcMem = CreateCompatibleDC(hDC);
	hBitmap = CreateBitmap(32, 32, 1, 1, NULL);
	SelectObject(hdcMem, hBitmap);
	SelectObject(hdcMem, tinyFont);
	

		flags = DT_NOPREFIX;
		if(_fstrrchr(theName, ' '))
			flags |= DT_WORDBREAK;
		else
			{
      SIZE size;
			flags|=DT_SINGLELINE|DT_VCENTER;
			GetTextExtentPoint32(hdcMem, theName, lstrlen(theName), &size);
			//if the name fits with function boundaries then horizontally center it
			if(size.cx < text.right)
				flags|=DT_CENTER;
			}

	//draw the image
	BitBlt(hdcMem,0,0,rt.right,rt.bottom,NULL,0,0,WHITENESS);
	SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
	SelectObject(hdcMem, GetStockObject(BLACK_PEN));
	
 //	Rectangle(hdcMem, text.left, text.top, text.right, text.bottom);
	SetBkMode(hdcMem, TRANSPARENT);
	SetTextColor(hdcMem, RGB(0,0,0));
//	DrawText(hdcMem, theName, lstrlen(theName),
//		&text, flags);
	GetBitmapBits(hBitmap, 128L, (LPSTR)byIMAGEBits);

	//draw the mask
	BitBlt(hdcMem,0,0,rt.right,rt.bottom,NULL,0,0,BLACKNESS);
	SelectObject(hdcMem, GetStockObject(WHITE_PEN));
	SetTextColor(hdcMem, RGB(255,255,255));
	Rectangle(hdcMem, text.left, text.top, text.right, text.bottom);
	DrawText(hdcMem, theName, lstrlen(theName),
		&text, flags);
	GetBitmapBits(hBitmap, 128L, (LPSTR)byMASKBits);
	
	DeleteDC(hdcMem);
	DeleteObject(hBitmap);

	return CreateCursor(hInst, 15, 15, 32, 32,
	 (LPSTR)byMASKBits,(LPSTR)byIMAGEBits);
	}


void PALETTE::MinimizeWindow()
	{
	RECT newPalLoc;
	RECT rt;
	RECT client;
	DWORD style;
	HMENU hMenu;

	status |= PAL_MINIMIZED;
	newPalLoc.right = turboPalLoc.x;
	newPalLoc.top = turboPalLoc.y;

	SetWindowText(hwndPalette, "Turbo Palette");

	newPalLoc.left = newPalLoc.right - catWidth + 1;
	newPalLoc.bottom = newPalLoc.top + nHeight - 2 - 
		(catHeight - catStatHeight);

	HideNeededWindows();

	hMenu = GetSystemMenu(hwndPalette, FALSE);
	AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hMenu,  MF_STRING | MF_ENABLED, IDM_PAL_CONFIGURE, "Configure Palette");  
	DrawMenuBar(hwndPalette);

	//replace minimize with maximize
	style = GetWindowLong(hwndPalette, GWL_STYLE);
	style &= ~WS_MINIMIZEBOX;
	style |= WS_MAXIMIZEBOX;
	SetWindowLong(hwndPalette, GWL_STYLE, style);

	MoveWindow(hwndPalette, newPalLoc.left, newPalLoc.top,
		newPalLoc.right - newPalLoc.left + 1,
		newPalLoc.bottom - newPalLoc.top + 1,
		TRUE);

	GetClientRect(hwndPalette, &client);
	
	GetWindowRect(hwndNumCat, &rt);

	MoveWindow(hwndCatStatus, 
		-1, 
		rt.bottom - rt.top - 3,
		client.right+2,
		catStatHeight,
		TRUE);

	MoveWindow(hwndNumCat, -2,-1, 
		client.right + 3,
		rt.bottom - rt.top,
		TRUE);

	ShowNeededWindows();
	}

void PALETTE::MaximizeWindow()
	{
	RECT newPalLoc;
	RECT rt;
	RECT client;
	DWORD style;
	HMENU hMenu;

	status &= ~PAL_MINIMIZED;
	newPalLoc.left = palLoc.x;
	newPalLoc.top = palLoc.y;

	newPalLoc.right = newPalLoc.left + GetSystemMetrics(SM_CXSCREEN) - 1;
	newPalLoc.bottom = newPalLoc.top + nHeight -1;

	HideNeededWindows();

	hMenu = GetSystemMenu(hwndPalette, FALSE);
	DeleteMenu(hMenu, IDM_PAL_CONFIGURE, MF_BYCOMMAND);
	if(GetMenuItemID(hMenu, GetMenuItemCount(hMenu) -1) == 0)
		{
		//remove the separator
		DeleteMenu(hMenu, GetMenuItemCount(hMenu) - 1, MF_BYPOSITION);
		}
	DrawMenuBar(hwndPalette);

	if(status & PAL_TURBOENABLED)
		{
		//replace minimize with maximize
		style = GetWindowLong(hwndPalette, GWL_STYLE);
		style &= ~WS_MAXIMIZEBOX;
			style |= WS_MINIMIZEBOX;
		SetWindowLong(hwndPalette, GWL_STYLE, style);
		}

	MoveWindow(hwndPalette, newPalLoc.left, newPalLoc.top,
		newPalLoc.right - newPalLoc.left + 1,
		newPalLoc.bottom - newPalLoc.top + 1,
		TRUE);

	GetClientRect(hwndPalette, &client);
	
	GetWindowRect(hwndNumCat, &rt);

	MoveWindow(hwndNumCat, catBegin-1, catHeight, 
		catWidth,
		rt.bottom - rt.top,
		TRUE);

	ShowNeededWindows();
	}


void PALETTE::ShowNeededWindows()
	{
	if(status & PAL_MINIMIZED)
		{
		ShowWindow(hwndCatStatus, SW_SHOWNA);
		ShowWindow(hwndNumCat, SW_SHOWNA);
		}
	else
		{
		ShowWindow(hwndObject, SW_SHOWNA);
		ShowWindow(hwndFunction, SW_SHOWNA);
		ShowWindow(hwndCategory, SW_SHOWNA);
		ShowWindow(hwndNumCat, SW_SHOWNA);
		}
	}

void PALETTE::HideNeededWindows()
	{
	if(status & PAL_MINIMIZED)
		{
		ShowWindow(hwndObject, SW_HIDE);
		ShowWindow(hwndFunction, SW_HIDE);
		ShowWindow(hwndCategory, SW_HIDE);
		}
	else
		{
		ShowWindow(hwndCatStatus, SW_HIDE);
		}
	}

///////////////////////////////////////////////////////////
void PALETTE::ConfigureTurboPalette()
	{
	LPPOPUPMENU tempObjPopup;
	LPPOPUPMENU tempFuncPopup;
	int type;
	BOOL swap;

	swap = FALSE;
	if(DialogBoxParam(PROGSYSTEM::hInst, "TURBOPALCONFIG", NULL, 
										(DLGPROC)PALETTE::TurboPalConfigProc, (LPARAM)this))
		{
		//set flags
		type = objPopupPtr->GetType();
		if(status & PAL_OBJICONS)
			{
			if(!(type & POPUP_OBJICONS))
				{
				type |= POPUP_OBJICONS;
				swap = TRUE;
				}
			}
		else if(type & POPUP_OBJICONS)
			{
			//create objPopup without icons
			type &= ~POPUP_OBJICONS;
			swap =  TRUE;
			}

		if(swap)
			{
			tempObjPopup = objPopupPtr;
			objPopupPtr = new POPUPMENU(NULL, hFont, type);
			*objPopupPtr = *tempObjPopup;
			delete tempObjPopup;
			}

		swap = FALSE;
		type = funcPopupPtr->GetType();
		if(status & PAL_FUNCICONS)
			{
			if(!(type & POPUP_FUNCICONS))
				{
				type |= POPUP_FUNCICONS;
				swap = TRUE;
				}
       }
		else if(type & POPUP_FUNCICONS)
			{
			type &= ~POPUP_FUNCICONS;
			swap = TRUE;
			}

		if(swap)
			{
			tempFuncPopup = funcPopupPtr;
			funcPopupPtr = new POPUPMENU(NULL, hFont, type);
			*funcPopupPtr = *tempFuncPopup;
			delete tempFuncPopup;
			}			
		}
	}

///////////////////////////////////////////////////////////
#define IDB_OBJICONS 	1000
#define IDB_FUNCICONS 1002

BOOL CALLBACK PALETTE::TurboPalConfigProc(
	HWND hDlg, 
	UINT msg, 
	WPARAM wParam, 
	LPARAM lParam)
	{
	int endVal;

	static LPPALETTE s_palPtr = NULL;

	switch(msg)
		{
		case WM_INITDIALOG:
			I_ASSERT(!s_palPtr);
			s_palPtr = (LPPALETTE)lParam;
			CenterDialog(hDlg, PROGSYSTEM::hWndFrame, FALSE);

			CheckDlgButton(hDlg, IDB_OBJICONS, (UINT)(s_palPtr->status & PAL_OBJICONS));
			CheckDlgButton(hDlg, IDB_FUNCICONS, (UINT)(s_palPtr->status & PAL_FUNCICONS));
			
			break;

		case WM_COMMAND:
			if(GET_WM_COMMAND_ID(wParam, lParam) == IDOK)
				{
				endVal = 0;	//no changes

				if(IsDlgButtonChecked(hDlg, IDB_OBJICONS))
					{
					if(!(s_palPtr->status & PAL_OBJICONS))
						{
						s_palPtr->status |= PAL_OBJICONS;
						endVal = 1;
						}
					}
				else
					{
					if(s_palPtr->status & PAL_OBJICONS)
						{
						s_palPtr->status &= ~PAL_OBJICONS;
						endVal = 1;
						}
					}

				if(IsDlgButtonChecked(hDlg, IDB_FUNCICONS))
					{
					if(!(s_palPtr->status & PAL_FUNCICONS))
						{
						s_palPtr->status |= PAL_FUNCICONS;
						endVal = 1;
						}
					}
				else
					{
					if(s_palPtr->status & PAL_FUNCICONS)
						{
						s_palPtr->status &= ~PAL_FUNCICONS;
						endVal = 1;
						}
					}
				
				EndDialog(hDlg, endVal);
				return TRUE;
				}
			else if(wParam == IDCANCEL)
				{
				EndDialog(hDlg, FALSE);
				return FALSE;
				}
			break;

		case WM_DESTROY:
			s_palPtr = NULL;
			break;
		}

	return 0;
	}

////////////////////////////////////////////////////////
// ***********  IMPORTANT	****************
//We keep	track of the right edge of the turbo palette for the following
//reason: If the location has never been saved, we can't calculate the
//left edge while reading in the preferences since we have no idea what the
//width is yet.

//preferences
static char PREF_ON[]				= {"on"};
static char PREF_OFF[]				= {"off"};
static char PREF_PALKEY[] = {"Object/Function Palette"};
static char PREF_PALLOCATION[] = {"Palette Location"};
static char PREF_TURBOPALLOCATION[] = {"Turbo Palette Location"};
static char PREF_OBJICONS[]	= {"Object Icons"};
static char PREF_FUNCICONS[] = {"Function Icons"};
static char PREF_TURBOPAL[]	= {"Turbo Palette"};
static char PREF_ENABLETURBO[] = {"Turbo Palette Backdoor"};
static char PREF_ET_ANSWER[] = {"opened"};
static char PREF_ET_DISABLE[] = {"closed"};

static char s_tokSeparator[] = {", "};

///////////////////////////////////////////////////////////////////
void PALETTE::ReadPreferences()
	{
	char buffer[64];
	LPSTR bufPtr;
	char defBuffer[16];
	HKEY vabKey;
  HKEY palKey = NULL;
  
	//get key to where enironment preferences go
	if((vabKey = AUtlOpenProductSubKey(HKEY_CURRENT_USER, AUTL_KEY_APEBUILDER)) != NULL)  	
 		{
 		RegOpenKeyEx(vabKey, PREF_PALKEY, 0, KEY_ALL_ACCESS, &palKey);
 		}


	AUtlGetKeyString(palKey, PREF_ENABLETURBO, buffer, sizeof(buffer), "");
	if(!lstrcmp(buffer, PREF_ET_ANSWER))
		status |= PAL_TURBOENABLED;
	else
		status &= ~PAL_TURBOENABLED;


	//read pal location
	wsprintf(defBuffer, "%u%s%u", nLeft, s_tokSeparator, nTop);
	
	if(status & PAL_TURBOENABLED)
		AUtlGetKeyString(palKey, PREF_PALLOCATION, buffer, 
			sizeof(buffer), defBuffer);
	else
		lstrcpy(buffer, defBuffer);

	bufPtr = _fstrtok(buffer, s_tokSeparator);
	if(bufPtr != NULL)
		palLoc.x = atoi(bufPtr);
	else
		{
		palLoc.x = 0;
		I_ASSERT(FALSE);
		}
	bufPtr = _fstrtok(NULL, s_tokSeparator);
	if(bufPtr)
		palLoc.y = atoi(bufPtr);
	else
		{
		palLoc.y = 0;
		I_ASSERT(FALSE);
		}

	//read turbo pal location
	wsprintf(defBuffer, "%u%s%u", nLeft + nWidth -1, s_tokSeparator, nTop);
	if(status & PAL_TURBOENABLED)
		AUtlGetKeyString(palKey, PREF_TURBOPALLOCATION,
			buffer, sizeof(buffer), defBuffer);
	else
		lstrcpy(buffer, defBuffer);

	bufPtr = _fstrtok(buffer, s_tokSeparator);
	if(bufPtr != NULL)
		turboPalLoc.x = atoi(bufPtr);
	else
		{
		turboPalLoc.x = 0;
		I_ASSERT(FALSE);
		}
	bufPtr = _fstrtok(NULL, s_tokSeparator);
	if(bufPtr)
		turboPalLoc.y = atoi(bufPtr);
	else
		{
		turboPalLoc.y = 0;
		I_ASSERT(FALSE);
		}


	if(status & PAL_TURBOENABLED)
		AUtlGetKeyString(palKey, PREF_OBJICONS, buffer, sizeof(buffer), PREF_OFF);
	else
		lstrcpy(buffer, PREF_OFF);

	if(!lstrcmp(buffer, PREF_OFF))
		status &= ~PAL_OBJICONS;
	else
		status |= PAL_OBJICONS;
		
	if(status & PAL_TURBOENABLED)
		AUtlGetKeyString(palKey, PREF_FUNCICONS,
			buffer, sizeof(buffer), PREF_OFF);
	else
		lstrcpy(buffer, PREF_OFF);

	if(!lstrcmp(buffer, PREF_OFF))
		status &= ~PAL_OBJICONS;
	else
		status |= PAL_OBJICONS;

	if(status & PAL_TURBOENABLED)
		AUtlGetKeyString(palKey, PREF_TURBOPAL,
			buffer, sizeof(buffer), PREF_OFF);
	else
		lstrcpy(buffer, PREF_OFF);

	if(!lstrcmp(buffer, PREF_OFF))
		status &= ~PAL_MINIMIZED;
	else
		status |= PAL_MINIMIZED;

	if(palKey)
  	RegCloseKey(palKey);
    
  if(vabKey)
  	AUtlCloseProductSubKey(vabKey);
	}


/////////////////////////////////////////////////////
void PALETTE::WritePreferences()
	{
	char buffer[64];
	HKEY vabKey;
  HKEY palKey = NULL;
  DWORD action;
  
	//get key to where enironment preferences go
	if((vabKey = AUtlOpenProductSubKey(HKEY_CURRENT_USER, AUTL_KEY_APEBUILDER)) != NULL)  	
		{
		RegCreateKeyEx(vabKey, PREF_PALKEY, 0, NULL,
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &palKey, &action);
		}

	if(status & PAL_TURBOENABLED)
    lstrcpy(buffer, PREF_ET_ANSWER);
  else
  	lstrcpy(buffer, PREF_ET_DISABLE);
  	 
	I_VERIFY(AUtlWriteKeyString(palKey, PREF_ENABLETURBO, buffer));

	if(status & PAL_TURBOENABLED)
		{
		//don't yet write out the normal palette location
		wsprintf(buffer, "%u%s%u", turboPalLoc.x, s_tokSeparator, turboPalLoc.y);
		I_VERIFY(AUtlWriteKeyString(palKey, PREF_TURBOPALLOCATION, buffer));

		if(status & PAL_OBJICONS)
			lstrcpy(buffer, PREF_ON);
		else
			lstrcpy(buffer, PREF_OFF);
		I_VERIFY(AUtlWriteKeyString(palKey, PREF_OBJICONS, buffer));

		if(status & PAL_FUNCICONS)
			lstrcpy(buffer, PREF_ON);
		else
			lstrcpy(buffer, PREF_OFF);
		I_VERIFY(AUtlWriteKeyString(palKey, PREF_FUNCICONS, buffer));

		if(status & PAL_MINIMIZED)
			lstrcpy(buffer, PREF_ON);
		else
			lstrcpy(buffer, PREF_OFF);
		I_VERIFY(AUtlWriteKeyString(palKey, PREF_TURBOPAL, buffer));
		
		}
    
	if(palKey)
  	RegCloseKey(palKey);
    
  if(vabKey)
  	AUtlCloseProductSubKey(vabKey);
	}
							
