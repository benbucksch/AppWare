/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:					ITEMDLG.CPP
//
// AUTHOR:				Brian Brown
//
// DESCRIPTION:		Menu item object editing routines
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#define STRICT
#define ASTRICT

#include <windows.h>
#include <bwcc.h>
#include <mem.h>
#include <string.h>

#include "a_alm.h"
#include "a_almutl.h"
#include "op_wnd.h"
#include "o_wnd.h"
//#include "helpids.h"
#include "modinfo.h"
#include "itemobj.h"
#include "itemlib.h"
#include "o_menus.h"


#define IDD_OBJECT		201

// Item Dialog
#define BTN_TYPE_NORMAL      102
#define BTN_TYPE_TOGGLE      103
#define BTN_STATUS_ENABLED   106
#define BTN_STATUS_DISABLED  107
#define SLE_TITLE            108
#define SLE_TITLE_TOGGLE     110
#define BTN_CHECKMARK        112
#define SLE_CMDKEY           113

#define BTN_ICON             199

#define TXT_TOGGLE_TEXT      115     // Static text descriptor

static WNDPROC g_fpOrigWndProc = 0;
static char g_accelKey         = 0;
static char g_accelModifier    = 0;

/*
char* g_cmdKeyToken[] =
  {
  "",         //   0
  "A",        //   1
  "B",        //   2
  "C",        //   3
  "D",        //   4
  "E",        //   5
  "F",        //   6
  "G",        //   7
  "H",        //   8
  "I",        //   9
  "J",        //  10
  "K",        //  11
  "L",        //  12
  "M",        //  13
  "N",        //  14
  "O",        //  15
  "P",        //  16
  "Q",        //  17
  "R",        //  18
  "S",        //  19
  "T",        //  20
  "U",        //  21
  "V",        //  22
  "W",        //  23
  "X",        //  24
  "Y",        //  25
  "Z",        //  26
  NULL,       //  27
  NULL,       //  28
  NULL,       //  29
	NULL,       //  30
	NULL,       //  31
	NULL,       //  32
  NULL,       //  33
  NULL,       //  34
  NULL,       //  35
  NULL,       //  36
  NULL,       //  37
  NULL,       //  38
  NULL,       //  39
  NULL,       //  40
  NULL,       //  41
  NULL,       //  42
  NULL,       //  43
  NULL,       //  44
  "Insert",      //  45
  "Delete",      //  46
  NULL,       //  47
  NULL,       //  48
  NULL,       //  49
  NULL,       //  50
  NULL,       //  51
  NULL,       //  52
  NULL,       //  53
  NULL,       //  54
  NULL,       //  55
  NULL,       //  56
  NULL,       //  57
  NULL,       //  58
  NULL,       //  59
  NULL,       //  60
  NULL,       //  61
  NULL,       //  62
  NULL,       //  63
  NULL,       //  64
  NULL,       //  65
  NULL,       //  66
  NULL,       //  67
  NULL,       //  68
  NULL,       //  69
  NULL,       //  70
  NULL,       //  71
  NULL,       //  72
  NULL,       //  73
  NULL,       //  74
  NULL,       //  75
  NULL,       //  76
  NULL,       //  77
  NULL,       //  78
  NULL,       //  79
  NULL,       //  80
  NULL,       //  81
  NULL,       //  82
  NULL,       //  83
  NULL,       //  84
  NULL,       //  85
  NULL,       //  86
  NULL,       //  87
  NULL,       //  88
  NULL,       //  89
  NULL,       //  90
  NULL,       //  91
  NULL,       //  92
  NULL,       //  93
  NULL,       //  94
  NULL,       //  95
  "NPAD0",    //  96
  "NPAD0",    //  97
  "NPAD0",    //  98
  "NPAD0",    //  99
  "NPAD0",    // 100
  "NPAD0",    // 101
  "NPAD0",    // 102
  "NPAD0",    // 103
  "NPAD0",    // 104
  "NPAD0",    // 105
  NULL,       // 106
  NULL,       // 107
  NULL,       // 108
  NULL,       // 109
  NULL,       // 110
  NULL,       // 111
  "F1",       // 112
  "F2",       // 113
  "F3",       // 114
  "F4",       // 115
  "F5",       // 116
  "F6",       // 117
  "F7",       // 118
  "F8",       // 119
  "F9",       // 120
  "F10",      // 121
  "F11",      // 122
  "F12",      // 123
  "F13",      // 124
  "F14",      // 125
  "F15",      // 126
  "F16",      // 127
  NULL,       // 128
  NULL,       // 129
  NULL,       // 130
  NULL,       // 131
  NULL,       // 132
  NULL,       // 133
  NULL,       // 134
  NULL,       // 135
  NULL,       // 136
  NULL,       // 137
  NULL,       // 138
  NULL,       // 139
  NULL,       // 140
  NULL,       // 141
  NULL,       // 142
  NULL,       // 143
  NULL,       // 144
  "Alt+",     // 145
  "Ctrl+",    // 146
  "Shift+",   // 147
  "+"         // 148
  };    */


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SetCommandKeyText
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
static void SetCommandKeyText(
  HWND  hDlg)
  {
  char buffer[21] = "";

  if (g_accelModifier & OWND_SHIFT)
    lstrcat(buffer, "Shift+");

  if(g_accelModifier & OWND_CTRL)
    lstrcat(buffer, "Ctrl+");


  lstrcat(buffer, g_cmdKeyToken[g_accelKey]);


  SetDlgItemText(hDlg, SLE_CMDKEY, buffer);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ShowControl
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
static void HideControl(
  HWND hwnd)
  {

  EnableWindow(hwnd, FALSE);
  UpdateWindow(hwnd);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ShowControl
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
static void ShowControl(
  HWND hwnd)
  {

  EnableWindow(hwnd, TRUE);
  UpdateWindow(hwnd);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: HidShowControls
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
static void HideShowControls(
  HWND  hDlg,
  int   cntrl)
  {
    // Hide show controls according to control ID
  switch ( cntrl )
    {
	  case BTN_TYPE_NORMAL:
		  HideControl(GetDlgItem(hDlg, TXT_TOGGLE_TEXT));
      HideControl(GetDlgItem(hDlg, SLE_TITLE_TOGGLE));
      HideControl(GetDlgItem(hDlg, BTN_CHECKMARK));
      break;

	  case BTN_TYPE_TOGGLE:
		  ShowControl(GetDlgItem(hDlg, TXT_TOGGLE_TEXT));
      ShowControl(GetDlgItem(hDlg, SLE_TITLE_TOGGLE));
      ShowControl(GetDlgItem(hDlg, BTN_CHECKMARK));
		  break;
    }
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SubClassCmndKey
//
// DESCRIPTION.: Subclass Edit Control On Menu Item Dialog
//
////////////////////////////////////////////////////////////////////////////
LONG FAR PASCAL _export SubClassCmndKey(
  HWND hWnd,
  WORD wMsg,
  WORD wParam,
  LONG lParam)
  {
	HWND         hDlg;
  long         result;

  hDlg = GetParent(hWnd);

  switch (wMsg)
    {
    case WM_CHAR:
		  if (wParam == VK_SPACE)
		    {
			  g_accelKey = 0;
        g_accelModifier = 0;
        SetCommandKeyText(hDlg);
		    }
      else if (wParam >= 1 && wParam <= 26)
        {
			  g_accelKey = wParam;
			  g_accelModifier = OWND_CTRL;
        SetCommandKeyText(hDlg);
		    }
		  else if (wParam != VK_SHIFT && wParam != VK_CONTROL && wParam != VK_MENU)
			  MessageBeep(-1);
      result = 0;
      break;

    case WM_KEYDOWN:
		  if ( (wParam >= VK_F1 && wParam <= VK_F12) ||
           wParam == VK_INSERT || wParam == VK_DELETE)
        {
        if ( GetKeyState(VK_SHIFT) < 0 )
          {
				  g_accelModifier = OWND_SHIFT;
          }
			  else if (GetKeyState(VK_CONTROL) < 0 )
			    {
				  g_accelModifier = OWND_CTRL;
			    }
			  else
			    {
          g_accelModifier = 0;
			    }
        g_accelKey = (char)wParam;
        SetCommandKeyText(hDlg);
        }
		  else if (wParam != VK_SHIFT && wParam != VK_CONTROL &&
			         wParam != VK_MENU &&  wParam != VK_BACK &&
			         !(wParam >= 65 && wParam <= 90) )
			  MessageBeep(-1);
      result = 0;
      break;

    default:
      result = CallWindowProc(g_fpOrigWndProc, hWnd, wMsg, wParam, lParam);

    }

  return result;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: InitDialog
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
void InitDialog(
  HWND       hDlg,
  OBJECTID   objID)
  {
	char 	      text[ITEM_MAX_TEXT+1];
  LPITEMINFO  pItemInfo;
  LPSTR       pText;
  HWND        hCmdKeyText;

	AObjGetName(objID, text, ITEM_MAX_TEXT);
	SetWindowText(hDlg, text);

  pItemInfo = (LPITEMINFO)AObjLockData(objID, ITEM_INFO_DATA);

	if ((pItemInfo->status & ITEM_NEVER_EDITED) == ITEM_NEVER_EDITED)
		AObjSetData(objID, ITEM_TITLE_DATA, text, lstrlen(text)+1);

	g_accelKey = pItemInfo->accelKey;
  g_accelModifier = pItemInfo->accelModifier;

    // Provisions for subclassing
  hCmdKeyText = GetDlgItem(hDlg, SLE_CMDKEY);
  g_fpOrigWndProc = (WNDPROC)GetWindowLong(hCmdKeyText, GWL_WNDPROC);
  SetWindowLong(hCmdKeyText, GWL_WNDPROC, (LONG)SubClassCmndKey);

    // Set Check Boxes
  CheckDlgButton(hDlg, BTN_CHECKMARK,
   (pItemInfo->status & ITEM_SHOW_CHECKMARK) == ITEM_SHOW_CHECKMARK);

  if ((pItemInfo->status & ITEM_DISABLED) == ITEM_DISABLED)
    {
	  CheckRadioButton(hDlg, BTN_STATUS_ENABLED, BTN_STATUS_DISABLED,
     BTN_STATUS_DISABLED);
    }
  else
    {
	  CheckRadioButton(hDlg, BTN_STATUS_ENABLED, BTN_STATUS_DISABLED,
     BTN_STATUS_ENABLED);
    }

  if ((pItemInfo->status & ITEM_TOGGLE_TYPE) == ITEM_TOGGLE_TYPE)
    {
    HideShowControls(hDlg, BTN_TYPE_TOGGLE);
    CheckRadioButton(hDlg, BTN_TYPE_NORMAL, BTN_TYPE_TOGGLE,
     BTN_TYPE_TOGGLE);
    }
  else
    {
    HideShowControls(hDlg, BTN_TYPE_NORMAL);
    CheckRadioButton(hDlg, BTN_TYPE_NORMAL, BTN_TYPE_TOGGLE,
     BTN_TYPE_NORMAL);
    }

  AObjUnlockData(objID, ITEM_INFO_DATA);

    // Set Text Fields

	if (0 != (pText = (LPSTR)AObjLockData(objID, ITEM_TITLE_DATA)))
    {
		SetDlgItemText(hDlg, SLE_TITLE, (LPSTR)pText);
    AObjUnlockData(objID, ITEM_TITLE_DATA);
    }

	if (0 != (pText = (LPSTR)AObjLockData(objID, ITEM_TOGGLE_TITLE_DATA)))
    {
		SetDlgItemText(hDlg, SLE_TITLE_TOGGLE, (LPSTR)pText);
    AObjUnlockData(objID, ITEM_TOGGLE_TITLE_DATA);
    }

  SetCommandKeyText(hDlg);

	SetFocus(GetDlgItem(hDlg, SLE_TITLE));

	SendDlgItemMessage(hDlg, SLE_TITLE, EM_SETSEL, 0, MAKELONG(0, -1));

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: UpdateObject
//
// DESCRIPTION.:
//
////////////////////////////////////////////////////////////////////////////
static void UpdateObject(
  HWND      hDlg,
  OBJECTID  objID)
  {
  LPITEMINFO  pItemInfo;
  char        text[ITEM_MAX_TEXT+1];

  pItemInfo = (LPITEMINFO)AObjLockData(objID, ITEM_INFO_DATA);

	pItemInfo->accelKey = g_accelKey;
  pItemInfo->accelModifier = g_accelModifier;

    // set status bits

	pItemInfo->status &= (~ITEM_NEVER_EDITED);

  if (TRUE == IsDlgButtonChecked(hDlg, BTN_STATUS_DISABLED))
    pItemInfo->status |= ITEM_DISABLED;
  else
    pItemInfo->status &= (~ITEM_DISABLED);

	if (TRUE == IsDlgButtonChecked(hDlg, BTN_TYPE_TOGGLE))
    {
	  pItemInfo->status |= ITEM_TOGGLE_TYPE;
	  if (TRUE == IsDlgButtonChecked(hDlg, BTN_CHECKMARK))
      pItemInfo->status |= ITEM_SHOW_CHECKMARK;
    }
  else
    {
    pItemInfo->status &= (~ITEM_TOGGLE_TYPE);
    pItemInfo->status &= (~ITEM_SHOW_CHECKMARK);
    }

    // Data 1 Startup Text
  GetDlgItemText(hDlg, SLE_TITLE, LPSTR(text), ITEM_MAX_TEXT);
  AObjSetData(objID, ITEM_TITLE_DATA, LPSTR(text), strlen(text)+1);

	if ((pItemInfo->status & ITEM_TOGGLE_TYPE) == ITEM_TOGGLE_TYPE)
    {
    GetDlgItemText(hDlg, SLE_TITLE_TOGGLE, LPSTR(text), ITEM_MAX_TEXT);
    AObjSetData(objID, ITEM_TOGGLE_TITLE_DATA, LPSTR(text), lstrlen(text)+1);
	  }
  else
		AObjSetData(objID, ITEM_TOGGLE_TITLE_DATA, "", 2);

  AObjUnlockData(objID, ITEM_INFO_DATA);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: ItemDialogProc
//
// DESCRIPTION.: Subclass Edit Control On Menu Item Dialog
//
////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK _export ItemDialogProc(
  HWND    hDlg,
  UINT    message,
  WPARAM  wParam,
  LPARAM  lParam)
  {
	static OBJECTID   objID;
  BOOL              result =  TRUE;
  BOOL              msgProcessed = TRUE;
	LPDRAWITEMSTRUCT  drawInfo;

	switch (message)
		{
		case WM_INITDIALOG:
      objID = (OBJECTID)lParam;
      AUtlSetTitleFont(OTYPE_MENUITEM, NULL, GetDlgItem(hDlg, IDD_OBJECT));
			AUtlCenterDialog(hDlg, 0);
			InitDialog(hDlg, objID);
		  result = FALSE;
      break;

		case WM_COMMAND:
      switch ( wParam )
        {
	      case IDOK:
		      SetFocus(GetDlgItem(hDlg, IDOK));
			    UpdateObject(hDlg, objID);
          EndDialog(hDlg, TRUE);
          objID = 0;
          break;

        case IDCANCEL:
			    SetFocus(GetDlgItem(hDlg, IDCANCEL));
			    EndDialog(hDlg, FALSE);
          objID = 0;
          break;

		    case IDHELP:
//        WinHelp (hDlg, "serhelp.hlp", HELP_CONTEXT, HELPID_OBJD_Mitm) ;
          break;

		    case BTN_TYPE_NORMAL:
			    HideShowControls(hDlg, BTN_TYPE_NORMAL);
          break;

		    case BTN_TYPE_TOGGLE:
			    HideShowControls(hDlg, BTN_TYPE_TOGGLE);
          break;

        default:
          msgProcessed = FALSE;
          break;
        }
      break;

		case WM_DRAWITEM:
			drawInfo = (LPDRAWITEMSTRUCT)lParam;
			if (drawInfo->CtlID == BTN_ICON)
				{
        HICON hIcon = ATypeGetIcon(OTYPE_MENUITEM);
				if (hIcon != NULL)
					DrawIcon(drawInfo->hDC, drawInfo->rcItem.left,
					 drawInfo->rcItem.top, hIcon);
        }
      msgProcessed = TRUE;
      break;

    default:
      msgProcessed = FALSE;
      break;
	  }

  if (TRUE == msgProcessed)
    return result;
  else
    return FALSE;

  }

////////////////////////////////////////////////////////////////////////////
// FUNCTION....: EditMenuItemObject
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
long EditMenuItemObject(
  OBJECTID theObject)
  {
  long result;

  if (TRUE == DialogBoxParam(MODULEINFO::hInstance, "DLGITEM",
       GetActiveWindow(), ItemDialogProc, (LPARAM)theObject))
    result = A_MODIFIED;
  else
    result = A_OK;

  return result;

  }
