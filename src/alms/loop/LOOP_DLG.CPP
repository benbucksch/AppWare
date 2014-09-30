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
// FILE:          "loop_dlg.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <string.h>

#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>
#include <o_number.h>

#include "loop.h"
#include "loop_rc.h"
#include "helpids.h"

// function prototypes /////////////////////////////////////////////////////

BOOL UpdateLoop(HWND hwnd);
void InitDialog(HWND hwnd);
void FixLongValue(HWND hwndCtl);

////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK  LoopDlg
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )
  {
  BOOL  fRet		= FALSE;
	HWND	hStart;
  
  switch(uMsg)
    {
    case WM_INITDIALOG:
			hStart = GetDlgItem(hwnd, ED_START);

      //AUtlSetTitleFont(OTYPE_LOOP, NULL, GetDlgItem(hwnd, ST_TYPENAME));
	  AUtlCenterDialog(hwnd, 0);
      SetWindowLong(hwnd, DWL_USER, lParam);
      InitDialog(hwnd);
      SetFocus(hStart);
			Edit_SetSel(hStart, 0, -1);
      fRet = FALSE;
      break;

    case WM_COMMAND:
			{
			WORD		wID			= GET_WM_COMMAND_ID(wParam, lParam);
			WORD		wNotify	= GET_WM_COMMAND_CMD(wParam, lParam);
			HWND		hCtl		= GET_WM_COMMAND_HWND(wParam, lParam);

      switch(wID)
        {
        case IDOK:
          EndDialog(hwnd, UpdateLoop(hwnd));
          fRet = TRUE;
          break;

        case IDCANCEL:
          EndDialog(hwnd, FALSE);
          fRet = TRUE;
          break;

        case IDHELP:
					WinHelp (hwnd, "awusloop.hlp", HELP_CONTEXT, HELPID_OBJD) ;
          break;
          
        case ED_START:
        case ED_LENGTH:
          switch(wNotify)
            {
            case EN_KILLFOCUS:
              FixLongValue(hCtl);
              fRet = TRUE;
              break;
              
            default:
              break;
            }
          break;

        default:
          break;
        }
      break;
			}
      
    default:
      break;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////

void InitDialog
  (
  HWND  hwnd
  )

  {
  Loop*     pLoop           = (Loop*)GetWindowLong(hwnd, DWL_USER);
  long      lGetKey         = AOBJ_GETFIRSTKEY;
	HWND			hConnect;
  char      szBuffer[80];
  char		szType[256];
  AOBJECTID  oi;
  long      index;
  
	hConnect = GetDlgItem(hwnd, CB_CONNECT);

  // set the dialog title
  ATypeGetName(OTYPE_LOOP, szType, sizeof(szType));
  strcat(szType, ": ");
  AObjGetName(pLoop->ObjectID(), szBuffer, sizeof(szBuffer));
  strcat(szType, szBuffer);
  SetWindowText(hwnd, szType/*Buffer*/);

  // fill the values
  wsprintf(szBuffer, "%li", pLoop->StartValue());
  Edit_SetText(GetDlgItem(hwnd, ED_START), szBuffer);
  wsprintf(szBuffer, "%li", pLoop->Length());
  Edit_SetText(GetDlgItem(hwnd, ED_LENGTH), szBuffer);
  
  // check the radio buttons
  Button_SetCheck(GetDlgItem(hwnd, RB_TIGHT), !pLoop->MultiTask());
  Button_SetCheck(GetDlgItem(hwnd, RB_MULTITASK), pLoop->MultiTask());

  // empty the connection combo box
  ComboBox_ResetContent(hConnect);

  // add the "(None)" string
  index = ComboBox_InsertString(hConnect, 0, "(None)");
  ComboBox_SetItemData(hConnect, index, 0L);

  if(pLoop->ConnectionObject() == NULL)
    ComboBox_SetCurSel(hConnect, index);

  // add all number objects to the combobox
  while((oi=AObjGetNextObject(OTYPE_NUMBER, &lGetKey)) != NULL)
    {
    AObjGetName(oi, szBuffer, 256);
    index = ComboBox_AddString(hConnect, szBuffer);
    ComboBox_SetItemData(hConnect, index, oi);

    // select the item if it is the current connection
    if(pLoop->ConnectionObject() == oi)
      ComboBox_SetCurSel(hConnect, index);
    }
  }

////////////////////////////////////////////////////////////////////////////

BOOL UpdateLoop
  (
  HWND  hwnd
  )

  {
  Loop*   pLoop			= (Loop*)GetWindowLong(hwnd, DWL_USER);
  BOOL    fRet			= FALSE;
	HWND		hConnect	= GetDlgItem(hwnd, CB_CONNECT);

  if(pLoop)
    {
    char      szBuffer[256];
    int		    index;
    AOBJECTID  oi;
    
    Edit_GetText(GetDlgItem(hwnd, ED_START), szBuffer, sizeof(szBuffer));
    fRet = pLoop->SetStartValue(strtol(szBuffer, NULL, 0));

    Edit_GetText(GetDlgItem(hwnd, ED_LENGTH), szBuffer, sizeof(szBuffer));
    fRet = pLoop->SetLength(strtol(szBuffer, NULL, 0)) | fRet;

    fRet = pLoop->SetMultiTask(Button_GetCheck(GetDlgItem(hwnd, RB_MULTITASK))) ||
									fRet;

    index = ComboBox_GetCurSel(hConnect);
    if(index==(int)CB_ERR)
      fRet = pLoop->SetConnectionObject(NULL) || fRet;
    else
      {
      oi = (AOBJECTID)ComboBox_GetItemData(hConnect, index);
      fRet = pLoop->SetConnectionObject((oi == (AOBJECTID)CB_ERR) ? NULL : oi) | fRet;
      }
    }
    
  return fRet;
  }

////////////////////////////////////////////////////////////////////////////

void FixLongValue
  (
  HWND  hwndCtl
  )
  
  {
  char  szBuffer[256];
  long  lVal;

  GetWindowText(hwndCtl, szBuffer, 256);
  lVal = strtol(szBuffer, NULL, 0);
  wsprintf(szBuffer, "%li", lVal);
  Edit_SetText(hwndCtl, szBuffer);
  }
  
////////////////////////////////////////////////////////////////////////////
