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
////////////////////////////////////////////////////////////////////////////
//
// Serius font object --- based on the "commdlg.dll" CHOOSEFONT routines
//  object methods
//
////////////////////////////////////////////////////////////////////////////

#include "almsys.h"
#include "a_almutl.h"
#include <windowsx.h>
#include <commdlg.h>

#include "chosfont.h"

// defines /////////////////////////////////////////////////////////////////

#define WINDOW_BACKGROUND     0xFF000000L
#define WINDOW_FOREGROUND     0xFE000000L

// special typedef /////////////////////////////////////////////////////////

typedef UINT (APIENTRY *HOOKFUNC)(HWND, UINT, WPARAM, LPARAM);

// extern //////////////////////////////////////////////////////////////////

extern HINSTANCE  hDLL;

// function prototypes /////////////////////////////////////////////////////

UINT CALLBACK serdlgEditFontHook(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void DrawSampleText(HWND hWnd, LPDRAWITEMSTRUCT di);
void DrawBackColorDropDown(HWND hWnd,LPDRAWITEMSTRUCT di);

// SERDLG_CUSTDATA structure ///////////////////////////////////////////////

struct SERDLG_CUSTDATA
  {
  BOOL      doColors;
  COLORREF  bgColor;
  BOOL      doEffects;
  LPCSTR    displayString;
  LPCSTR    dialogTitle;
  };

// serdlgEditFont -- uses a modified common font dialog ////////////////////

extern "C"
BOOL ALMAPI AUtlDlgEditFont(
  HWND      hWndOwner,
  LPCSTR    displayString,
  LPCSTR    dialogTitle,
  LOGFONT*  lf,
  int *			deciPointSize,
  BOOL      doColors,       // FALSE -- colors hidden, <fg,bg>Color ignored
  COLORREF* fgColor,        // NULL -- no foreground color options
  COLORREF* bgColor,        // NULL -- no background color options
  BOOL      doEffects,      // FALSE -- effects (underline, strikeout) hidden
  DWORD     flags,
  BOOL      printerFonts,
  HDC       hPrinterDC)

  {
  CHOOSEFONT  cf;
  COLORREF    foreground;
  COLORREF    background;
  LOGFONT     lfBackup = *lf;

  if (!doColors)
    {
    foreground = GetSysColor(COLOR_WINDOWTEXT);
    background = GetSysColor(COLOR_WINDOW);
    }
  else
    {
    foreground = fgColor ? *fgColor : -1L;  // flag that there should be no foreground option
    background = bgColor ? *bgColor : -1L;  // flag that there should be no background option
    }

  SERDLG_CUSTDATA custData = {doColors, background, doEffects, displayString, dialogTitle};

  flags |= (CF_ENABLEHOOK | CF_ENABLETEMPLATE);

  memset(&cf, 0, sizeof(CHOOSEFONT));
  cf.lStructSize    = sizeof(CHOOSEFONT);
  cf.hwndOwner      = hWndOwner;
  if(printerFonts)
    cf.hDC          = hPrinterDC;
  else
    cf.hDC          = NULL;
  cf.lpLogFont      = lf;
  if(printerFonts && hPrinterDC)
    cf.Flags        = flags | CF_PRINTERFONTS;
  else
    cf.Flags        = flags & ~CF_PRINTERFONTS;
  cf.rgbColors      = foreground;
  cf.lpszStyle      = NULL;
  cf.lCustData      = (LPARAM)&custData;
  cf.lpfnHook       = (HOOKFUNC)serdlgEditFontHook;
  cf.lpTemplateName = MAKEINTRESOURCE(CHOOSEFONT_DIALOG);
  cf.hInstance      = hDLL;

  if (ChooseFont(&cf))
    {
    if(doColors && fgColor)
      *fgColor = cf.rgbColors;
    if(doColors && bgColor)
      *bgColor = custData.bgColor;
    if(deciPointSize)
      *deciPointSize = cf.iPointSize;
    return TRUE;
    }
  else
    {
    *lf = lfBackup;
    return FALSE;
    }
  }

///////////////////////////////////////////////////////////

UINT CALLBACK serdlgEditFontHook(
  HWND              hWnd,
  UINT              message,
  WPARAM            wParam,
  LPARAM            lParam)

  {
  LPCHOOSEFONT      cf;
  SERDLG_CUSTDATA*  custData;
  LPDRAWITEMSTRUCT  di;
  int               i, selBack=0, selFore=0;
  AMEMBLOCKID        nameMID;
  LPSTR             name;
  int               nameLen;
  COLORREF          color;

  switch (message)
    {
    case WM_INITDIALOG:
			AUtlCenterDialog(hWnd, 0);		// dialog is called only at design time

      cf = (LPCHOOSEFONT)lParam;
      custData = (SERDLG_CUSTDATA*) cf->lCustData;
      if(custData->dialogTitle && lstrlen(custData->dialogTitle)>0)
        SetWindowText(hWnd, custData->dialogTitle);

      SetWindowLong(hWnd, DWL_USER, (long)custData); 
      SetDlgItemText(hWnd, IDD_SAMPLETEXT, custData->displayString);

			ComboBox_InsertString(GetDlgItem(hWnd, IDD_BACKCOLOR), 0, "Default");
      ComboBox_SetItemData(GetDlgItem(hWnd, IDD_BACKCOLOR), 0, WINDOW_BACKGROUND);
      ComboBox_InsertString(GetDlgItem(hWnd, IDD_FORECOLOR), 0, "Default");
      ComboBox_SetItemData(GetDlgItem(hWnd, IDD_FORECOLOR), 0, WINDOW_FOREGROUND);

      // fill the background color combo box
      nameMID = AMemAllocate(0);
      for(i=1; i<=16; i++)
        {
				nameLen = ComboBox_GetLBTextLen(GetDlgItem(hWnd, IDD_FORECOLOR), i);
        AMemSetSize(nameMID, nameLen+1);
        name = (LPSTR) AMemLock(nameMID);

				ComboBox_GetLBText(GetDlgItem(hWnd, IDD_FORECOLOR), i, name);
        ComboBox_AddString(GetDlgItem(hWnd, IDD_BACKCOLOR), name);

        color = ComboBox_GetItemData(GetDlgItem(hWnd, IDD_FORECOLOR), i);
        ComboBox_SetItemData(GetDlgItem(hWnd, IDD_BACKCOLOR), i, color);

        if (color == custData->bgColor)
          selBack = i;
        if (color == cf->rgbColors)
          selFore = i;
        }
      AMemFree(nameMID);

      ComboBox_SetCurSel(GetDlgItem(hWnd, IDD_FORECOLOR), selFore);
      ComboBox_SetCurSel(GetDlgItem(hWnd, IDD_BACKCOLOR), selBack);

      if(!custData->doEffects)
        {
        ShowWindow(GetDlgItem(hWnd, IDC_GROUPBOX2), SW_HIDE);
        ShowWindow(GetDlgItem(hWnd, IDD_STRIKEOUT), SW_HIDE);
        ShowWindow(GetDlgItem(hWnd, IDD_UNDERLINE), SW_HIDE);
        }

      if (!custData->doColors)
        {
        ShowWindow(GetDlgItem(hWnd, IDD_FORECOLOR), SW_HIDE);
        ShowWindow(GetDlgItem(hWnd, IDD_BACKCOLOR), SW_HIDE);
        ShowWindow(GetDlgItem(hWnd, IDD_STFOREGROUND), SW_HIDE);
        ShowWindow(GetDlgItem(hWnd, IDD_STBACKGROUND), SW_HIDE);
        }
      else
        {
        if(custData->bgColor==(COLORREF)-1)
          {
          ShowWindow(GetDlgItem(hWnd, IDD_BACKCOLOR), SW_HIDE);
          ShowWindow(GetDlgItem(hWnd, IDD_STBACKGROUND), SW_HIDE);
          }
        if(cf->rgbColors==(COLORREF)-1)
          {
          ShowWindow(GetDlgItem(hWnd, IDD_FORECOLOR), SW_HIDE);
          ShowWindow(GetDlgItem(hWnd, IDD_STFOREGROUND), SW_HIDE);
          }
        }
      SetFocus(GetDlgItem(hWnd, IDOK));
      return FALSE;

    case WM_DRAWITEM:
      di = (LPDRAWITEMSTRUCT)lParam;
      switch(di->CtlID)
        {
        case IDD_SAMPLETEXT:
          DrawSampleText(hWnd, di);
          break;

        case IDD_FORECOLOR:
        case IDD_BACKCOLOR:
          DrawBackColorDropDown(hWnd, di);
          break;

        default:
          return FALSE;
        }
      return TRUE;

    case WM_COMMAND:
			{
			WORD	wNotify	= GET_WM_COMMAND_CMD(wParam, lParam);
			WORD	wID			= GET_WM_COMMAND_ID(wParam, lParam);
			
      if (wNotify == BN_CLICKED || wNotify == CBN_SELCHANGE ||
					wNotify == CBN_KILLFOCUS)
        {
        InvalidateRect( GetDlgItem(hWnd, IDD_SAMPLETEXT), NULL, TRUE);
        if (wID == IDD_BACKCOLOR)
          {
          custData = (SERDLG_CUSTDATA*)GetWindowLong(hWnd, DWL_USER);
          selBack = ComboBox_GetCurSel(GetDlgItem(hWnd, IDD_BACKCOLOR));
          custData->bgColor = ComboBox_GetItemData(GetDlgItem(hWnd, IDD_BACKCOLOR), selBack);
          }
        }
      return FALSE;
			}

    default:
      break;
    }

  return 0;
  }

// DrawSampleText //////////////////////////////////////////////////////////

void DrawSampleText(
  HWND              hWnd,
  LPDRAWITEMSTRUCT  di)

  {
  int               oldMode;
  LOGFONT           lf;
  HFONT             hFont, hFontOld;
  HBRUSH            hBrush;
  COLORREF          oldFore, foreColor, backColor;
  int               i;
  char              sampleText[51];

  i = ComboBox_GetCurSel(GetDlgItem(hWnd, IDD_FORECOLOR));
  foreColor = ComboBox_GetItemData(GetDlgItem(hWnd, IDD_FORECOLOR), i);
  if ((foreColor == WINDOW_FOREGROUND) || (foreColor == (COLORREF) CB_ERR))
    foreColor = GetSysColor(COLOR_WINDOWTEXT);
  i = ComboBox_GetCurSel(GetDlgItem(hWnd, IDD_BACKCOLOR));
  backColor = ComboBox_GetItemData(GetDlgItem(hWnd, IDD_BACKCOLOR), i);
  if ((backColor == WINDOW_BACKGROUND) || (backColor == (COLORREF) CB_ERR))
    backColor = GetSysColor(COLOR_WINDOW);

  SendMessage(hWnd, WM_CHOOSEFONT_GETLOGFONT, 0, (LONG)&lf);
  hFont = CreateFontIndirect(&lf);
  hBrush = CreateSolidBrush(backColor);

  hFontOld = (HFONT)SelectObject(di->hDC, hFont);
  oldFore = SetTextColor(di->hDC, foreColor);
  oldMode = SetBkMode(di->hDC, TRANSPARENT);

  FillRect(di->hDC, &di->rcItem, hBrush);

  GetDlgItemText(hWnd, IDD_SAMPLETEXT, sampleText, 50);
  DrawText(di->hDC, sampleText, lstrlen(sampleText), &di->rcItem, DT_CENTER|DT_VCENTER|DT_NOPREFIX);

  SelectObject(di->hDC, hFontOld);
  SetTextColor(di->hDC, oldFore);
  SetBkMode(di->hDC, oldMode);

  DeleteObject(hFont);
  DeleteObject(hBrush);
  }

// DrawBackColorDropDown ///////////////////////////////////////////////////

void DrawBackColorDropDown(
  HWND              hWnd,
  LPDRAWITEMSTRUCT  di)

  {
  if (di->itemID >17) return;

  HBRUSH    hBrush, highlightBrush, oldBack;
  char*     name;
  int       nameLen;
  COLORREF  color;
  RECT      rect;
  COLORREF  oldTextColor, oldBackColor;

  nameLen = ComboBox_GetLBTextLen(GetDlgItem(hWnd, di->CtlID), di->itemID);
  name = new char[nameLen+1];
  ComboBox_GetLBText(GetDlgItem(hWnd, di->CtlID), di->itemID, name);

  color = ComboBox_GetItemData(GetDlgItem(hWnd, di->CtlID), di->itemID);
  if (color == WINDOW_BACKGROUND)
    color = GetSysColor(COLOR_WINDOW);
  else if (color == WINDOW_FOREGROUND)
    color = GetSysColor(COLOR_WINDOWTEXT);
  else if (color == (COLORREF) CB_ERR)
    color = di->CtlID==IDD_FORECOLOR ? GetSysColor(COLOR_WINDOWTEXT) : GetSysColor(COLOR_WINDOW);
  hBrush = CreateSolidBrush(color);

  rect.top = di->rcItem.top+1;    rect.bottom = di->rcItem.bottom-1;
  rect.left = di->rcItem.left+1;  rect.right = di->rcItem.left+25;

  if (di->itemState & (ODS_SELECTED|ODS_FOCUS) )
    {
    highlightBrush = CreateSolidBrush( GetSysColor(COLOR_HIGHLIGHT) );
    oldTextColor = SetTextColor(di->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT) );
    oldBackColor = SetBkColor(di->hDC, GetSysColor(COLOR_HIGHLIGHT) );
    }
  else
    {
    highlightBrush = CreateSolidBrush( GetSysColor(COLOR_WINDOW) );
    oldTextColor = SetTextColor(di->hDC, GetSysColor(COLOR_MENUTEXT) );
    oldBackColor = SetBkColor(di->hDC, GetSysColor(COLOR_WINDOW) );
    }

  FillRect(di->hDC, &di->rcItem, highlightBrush );
  oldBack = (HBRUSH)SelectObject(di->hDC, highlightBrush);

  FillRect(di->hDC, &rect, hBrush);
  FrameRect(di->hDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));

  rect.top = di->rcItem.top;  rect.bottom = di->rcItem.bottom;
  rect.left = rect.right+4;   rect.right = di->rcItem.right;
  DrawText(di->hDC, name, nameLen, &rect, DT_LEFT);

  delete name;
  SetTextColor(di->hDC, oldTextColor);
  SetBkColor(di->hDC, oldBackColor);
  SelectObject(di->hDC, oldBack);
  if (hBrush) DeleteObject(hBrush);
  if (highlightBrush) DeleteObject(highlightBrush);
  }

////////////////////////////////////////////////////////////////////////////
