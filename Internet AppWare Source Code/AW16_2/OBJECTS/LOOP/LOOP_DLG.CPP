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
#include <stdlib.h>

#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>
#include <o_number.h>

#include "loop.h"
#include "loop_rc.h"

// function prototypes /////////////////////////////////////////////////////

BOOL UpdateLoop(HWND hwnd);
void InitDialog(HWND hwnd);
void FixLongValue(HWND hwndCtl);

////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK _export LoopDlg
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  BOOL  fRet = FALSE;
  
  switch(uMsg)
    {
    case WM_INITDIALOG:
      AUtlSetTitleFont(OTYPE_LOOP, NULL, GetDlgItem(hwnd, ST_TYPENAME));
			AUtlCenterDialog(hwnd, 0);
      SetWindowLong(hwnd, DWL_USER, lParam);
      InitDialog(hwnd);
      SetFocus(GetDlgItem(hwnd, ED_START));
      SendDlgItemMessage(hwnd, ED_START, EM_SETSEL, TRUE, MAKELONG(0, -1));
      fRet = FALSE;
      break;

    case WM_COMMAND:
      switch(wParam)
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
          break;
          
        case ED_START:
        case ED_LENGTH:
          switch(HIWORD(lParam))
            {
            case EN_KILLFOCUS:
              FixLongValue((HWND)LOWORD(lParam));
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
  char      szBuffer[256];
  long      lGetKey         = AOBJ_GETFIRSTKEY;
  OBJECTID  oi;
  long      index;
  
  // set the dialog title
  AObjGetName(pLoop->ObjectID(), szBuffer, 256);
  SetWindowText(hwnd, szBuffer);

  // fill the values
  wsprintf(szBuffer, "%li", pLoop->StartValue());
  SetDlgItemText(hwnd, ED_START, szBuffer);
  wsprintf(szBuffer, "%li", pLoop->Length());
  SetDlgItemText(hwnd, ED_LENGTH, szBuffer);
  
  // check the radio buttons
  CheckDlgButton(hwnd, RB_TIGHT, !pLoop->MultiTask());
  CheckDlgButton(hwnd, RB_MULTITASK, pLoop->MultiTask());

  // empty the connection combo box
  SendDlgItemMessage(hwnd, CB_CONNECT, CB_RESETCONTENT, 0, 0L);

  // add the "(None)" string
  index = SendDlgItemMessage(hwnd, CB_CONNECT, CB_INSERTSTRING, 0, (long)"(None)");
  SendDlgItemMessage(hwnd, CB_CONNECT, CB_SETITEMDATA, (WPARAM)index, 0L);

  if(pLoop->ConnectionObject() == NULL)
    SendDlgItemMessage(hwnd, CB_CONNECT, CB_SETCURSEL, (WPARAM)index, 0L); 

  // add all number objects to the combobox
  while((oi=AObjGetNextObject(OTYPE_NUMBER, &lGetKey)) != NULL)
    {
    AObjGetName(oi, szBuffer, 256);
    index = SendDlgItemMessage(hwnd, CB_CONNECT, CB_ADDSTRING, 0, (long)szBuffer);
    SendDlgItemMessage(hwnd, CB_CONNECT, CB_SETITEMDATA, (WPARAM)index, (LPARAM)oi);

    // select the item if it is the current connection
    if(pLoop->ConnectionObject() == oi)
      SendDlgItemMessage(hwnd, CB_CONNECT, CB_SETCURSEL, (WPARAM)index, 0L); 
    }
  }

////////////////////////////////////////////////////////////////////////////

BOOL UpdateLoop
  (
  HWND  hwnd
  )

  {
  Loop*   pLoop   = (Loop*)GetWindowLong(hwnd, DWL_USER);
  BOOL    fRet    = FALSE;

  if(pLoop)
    {
    char      szBuffer[256];
    WPARAM    index;
    OBJECTID  oi;
    
    GetDlgItemText(hwnd, ED_START, szBuffer, 256);
    fRet = pLoop->SetStartValue(strtol(szBuffer, NULL, 0));

    GetDlgItemText(hwnd, ED_LENGTH, szBuffer, 256);
    fRet = pLoop->SetLength(strtol(szBuffer, NULL, 0)) | fRet;

    fRet = pLoop->SetMultiTask(IsDlgButtonChecked(hwnd, RB_MULTITASK)) | fRet;

    index = (WPARAM)SendDlgItemMessage(hwnd, CB_CONNECT, CB_GETCURSEL, 0, 0L);
    if(index==(WPARAM)CB_ERR)
      fRet = pLoop->SetConnectionObject(NULL) | fRet;
    else
      {
      oi = (OBJECTID)SendDlgItemMessage(hwnd, CB_CONNECT, CB_GETITEMDATA, index, 0L);
      fRet = pLoop->SetConnectionObject((oi == (OBJECTID)CB_ERR) ? NULL : oi) | fRet;
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
  SetWindowText(hwndCtl, szBuffer);
  }
  
////////////////////////////////////////////////////////////////////////////
