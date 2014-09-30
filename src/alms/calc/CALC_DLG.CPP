///////////////////////////////////////////////////////////////
//
//	(c) 1993 Novell, Inc.  All rights reserved.
//
//	THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//	TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//	COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//	CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//	TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//	NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//	AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//	LIABILITY.
//
///////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//
// Object Group:	Essentials
// Object:		  		Calculation
//
// Filename:	"calc_dlg.cpp"
//
// Author:	Kevin Smart
//   Start:		July 13, 1992
//   Finish:	July xx, 1992
//
// Description:
//	CALCULATION class functions for object management
//
//
////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <windowsx.h>

#include <helpids.h>
#include <o_number.h>

#include "calc_obj.h"
#include "strclass.h"
#include "calc.h"
#include "listdata.h"
#include "calc_dlg.h"

////////////////////////////////////////////////////////////////////////////

long CALLBACK NewEditControlProc(HWND hWnd, UINT message, UINT wParam, LONG lParam)

{
	extern	GLOBAL	G;

	FARPROC	oldProc = G.OldProc();

	switch(message) {
///		case WM_DESTROY :
///				RemoveProp(hWnd, "Shift");
///       break;
    case WM_CHAR : {
///				WORD start = LOWORD(SendMessage(hWnd, EM_GETSEL, 0, 0L));
				WORD start = LOWORD(Edit_GetSel(hWnd)); 
				if(start==0 && isdigit(wParam)) {
				  MessageBeep(-1);
				  return 0L;
          }
				if(!(isalnum(wParam) || wParam=='_' || wParam=='\b')) {
					MessageBeep(-1);
				  return 0L;
				  }
				break;
				}

    case WM_PASTE : {
///				if(wParam==VK_SHIFT) SetProp(hWnd, "Shift", TRUE);
///				else if(GetProp(hWnd, "Shift") && wParam==VK_INSERT) {
 			  if(OpenClipboard(hWnd)) {
          HANDLE hText;
  			  if((hText = GetClipboardData(CF_TEXT)) != NULL) {
    			  char* text = (char*)GlobalLock(hText);
///      			WORD start = LOWORD(SendMessage(hWnd, EM_GETSEL, 0, 0L)); 
      			WORD start = LOWORD(Edit_GetSel(hWnd)); 
      			if(start==0 && isdigit(text[0])) {
							// can't start an alias with a number!!!
							CloseClipboard();
							GlobalUnlock(hText);
              return 0L;
              }
			      for(int i=0; i<lstrlen(text); i++) {
    			    if(!(isalnum(text[i]) || text[i]=='_')) {
	  						// don't paste if there is an invalid character
                 CloseClipboard();
	 							GlobalUnlock(hText);
							  return 0L;
							  }
			        } // end...for(int i=0; ...

              GlobalUnlock(hText);
             } // end...if((hText = ...

            CloseClipboard();
           }
///				  } // end...else if(G.Shift ...)
				break;
        }

///    case WM_KEYUP :
///				if(wParam==VK_SHIFT) SetProp(hWnd, "Shift", FALSE);
///				break;

    default : break;
    }

	return CallWindowProc((WNDPROC)oldProc, hWnd, message, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK CalcDlg(HWND hDlg, UINT message,
																UINT wParam, LONG lParam)

{
	extern	GLOBAL	G;
	CALCULATION*	calc;
	char	szBuffer[OBJECTNAMESIZE];
	char	szType[256];
	HWND	hAlias;

  switch(message) {
    case WM_INITDIALOG:
				//AUtlSetTitleFont(OTYPE_EXPRESSION, NULL, GetDlgItem(hDlg, IDD_OBJECT));
				AUtlCenterDialog(hDlg, 0);
				SetWindowLong(hDlg, DWL_USER, lParam);
				calc = (CALCULATION*)lParam;
				calc->InitDlgControls(hDlg);
				ATypeGetName(OTYPE_EXPRESSION, szType, sizeof(szType));
				strcat(szType, ": ");
				AObjGetName(calc->TheObject(), szBuffer, OBJECTNAMESIZE);
				strcat(szType, szBuffer);
				SetWindowText(hDlg, szType/*szBuffer*/);

				// subclass the edit control to limit input to alpha-numeric chars
				hAlias = GetDlgItem(hDlg, IDD_ALIAS);
				G.SetOldProc((FARPROC)GetWindowLong(hAlias, GWL_WNDPROC));
				SetWindowLong(hAlias, GWL_WNDPROC, (long)NewEditControlProc);
				SetProp(hAlias, "Shift", FALSE);

				return TRUE;

    case WM_COMMAND: 
				calc = (CALCULATION*)GetWindowLong(hDlg, DWL_USER);

				switch(GET_WM_COMMAND_ID(wParam, lParam)) {
					case IDOK:
							SetFocus(GetDlgItem(hDlg, IDOK));
							calc->UpdateObject(hDlg);
							if (!G.ParseError()) {
							  calc->DeleteListData(hDlg);
							  EndDialog(hDlg, 1);
								return TRUE;
							  }
							G.SetParseError(FALSE);
							return TRUE;

				  case IDCANCEL:
							SetFocus(GetDlgItem(hDlg, IDCANCEL));
							calc->DeleteListData(hDlg);
							EndDialog(hDlg, 0);
							return TRUE;

					case IDD_EVALUATE:
							calc->OBJD_Evaluate(hDlg);
							return TRUE;

					case IDHELP:
           		WinHelp(hDlg, "awuscalc.hlp", HELP_CONTEXT, HELPID_OBJD);
							return TRUE;

				  case IDD_ADD:
							calc->Add(hDlg);
							return TRUE;

				  case IDD_REMOVE:
							calc->Remove(hDlg);
							calc->UpdateRemoveButton(hDlg);
							calc->UpdateEditButton(hDlg);
							calc->UpdateAliasControl(hDlg);
							return TRUE;

				  case IDD_EDIT:
							calc->Edit(hDlg);
							return TRUE;

				  case IDD_ALIAS:
							switch(GET_WM_COMMAND_CMD(wParam, lParam)) {
							  //case EN_SETFOCUS :
							  //    PostMessage(LOWORD(lParam), EM_SETSEL, 1, MAKELONG(0, 32767)); //!!PORT!!
							  //    return FALSE;
							  //
							  case EN_KILLFOCUS :
										calc->Alias(hDlg);
										return TRUE;
							  }
              break;

				  case IDD_EXPRESSION:
							if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE) {
							  ;  // Check this at end dialog OK
		            }
							return TRUE;

				  case IDD_OBJECTS:
							if (GET_WM_COMMAND_CMD(wParam, lParam) == LBN_SELCHANGE) {
							  calc->UpdateAddButton(hDlg);
							  }

							if (GET_WM_COMMAND_CMD(wParam, lParam) == LBN_DBLCLK) {
								calc->Add(hDlg);
                }
							return TRUE;

				  case IDD_VARIABLES:
							if (GET_WM_COMMAND_CMD(wParam, lParam) == LBN_SELCHANGE) {
							  calc->UpdateRemoveButton(hDlg);
							  calc->UpdateEditButton(hDlg);
							  calc->UpdateAliasControl(hDlg);
							  }

							if (GET_WM_COMMAND_CMD(wParam, lParam) == LBN_DBLCLK) {
							  calc->Edit(hDlg);
                }
							return TRUE;
				}
				break;

		default: return FALSE;
    }

	return FALSE;
}

