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
// FILE:					KBDESC.CPP
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/

#define STRICT
#define ASTRICT
#include <memory.h>
#include <a_alm.h>
#include <a_almutl.h>

#include "keyboard.h"
#include "helpids.h"

#define OBJECTNAMESIZE  80
extern int  _WinAllocFlag;

HINSTANCE  g_hInstance;

////////////////////////////////////////////////////////////////
//
// FUNCTION:	LibMain
//
// DESC:
//
////////////////////////////////////////////////////////////////

// Every DLL has an entry point LibMain and an exit point WEP.
int FAR PASCAL LibMain(
  HINSTANCE  hInstance,
  WORD    /*-- wDataSegment --*/,
  WORD    wHeapSize,
  LPSTR   /*-- lpszCmdLine --*/)
 {
  _WinAllocFlag = GMEM_SHARE;

  // The startup code for the DLL initializes the local heap (if there
  // is one) with a call to LocalInit which locks the data segment.
  if ( wHeapSize != 0 )
    UnlockData( 0 );

  g_hInstance = hInstance;

  return 1;   // Indicate that the DLL was initialized successfully.
  }


/****************************************************************************

    FUNCTION: KeyDlgProc(HWND, unsigned, WORD, LONG)

    PURPOSE: Let user select a file, and return.  Open code not provided.

****************************************************************************/

BOOL FAR PASCAL _export KeyDlgProc(
  HWND			hDlg,
  UINT			message,
  WPARAM		wParam,
  LPARAM		lParam)
  {
  static OBJECTID   objectID;

	KBRECORD*		pKBRec;

  switch (message)
    {
    case WM_INITDIALOG:
      {
      char title[OBJECTNAMESIZE];

			AUtlSetTitleFont(OTYPE_KEYBOARD, NULL, GetDlgItem(hDlg, ID_OBJECT));
			AUtlCenterDialog(hDlg, 0);

      objectID = (OBJECTID) lParam;
      pKBRec = (KBRECORD*) AObjLockData(objectID, DATA0);

      if (pKBRec->active)
        SendDlgItemMessage(hDlg, ACTIVEID, BM_SETCHECK, 1, 0);

      AObjGetName((OBJECTID)lParam, (LPSTR)title, OBJECTNAMESIZE);
      SetWindowText(hDlg, (LPSTR)title);

			AObjUnlockData(objectID, DATA0);

      return TRUE;
      }

    case WM_COMMAND:
      switch (wParam)
        {
        case IDOK:
          pKBRec = (KBRECORD*) AObjLockData(objectID, DATA0);

          if (SendDlgItemMessage(hDlg, ACTIVEID, BM_GETCHECK, 0, 0L) > 0)
            pKBRec->active = TRUE;
          else
            pKBRec->active = FALSE;
          EndDialog(hDlg, IDOK);

					AObjUnlockData(objectID, DATA0);

          return TRUE;

        case IDCANCEL:
          EndDialog(hDlg, IDCANCEL);
          break;

				case IDHELP: {

					WinHelp(hDlg, "vabhelp.hlp", HELP_CONTEXT, HELPID_OBJD_Keyb);

					}
					break;
        default:
          break;
        }
      break;

    default:
      break;
    }

  return FALSE;
  }


////////////////////////////////////////////////////////////////
//
// FUNCTION:	
//
// DESC:
//
////////////////////////////////////////////////////////////////

BOOL EditKey(
  OBJECTID objectID)
  {
  HWND          hWnd;
	int						idHit;

  hWnd = GetActiveWindow();
  idHit = DialogBoxParam(g_hInstance, "KEYBOARD", hWnd, KeyDlgProc, (LONG) objectID);

	return (idHit == IDOK);
  }

////////////////////////////////////////////////////////////////////////
//   doBuildSignals (local)
//
// Fills in the memblock specified in the passed SYSTEM record with
// the signal array for the keyboard object.
////////////////////////////////////////////////////////////////////////

void doBuildSignals(
  pAObjMessage  system)
  {
        // m: "blah"  --> the signal name is "blah in serius/m
        // m+         --> serius/m only
        // w+         --> serius/w only
        // v?         --> There is a virtual key code defined by
        //                Windows, but what key is it?

  static const AObjSignalRecord keyboardSignals[] =
		{

				// -------------------------- // m+ "Return Key"
			{0, "Enter Key" },
			{2, "Spacebar"} ,
			{3, "Upper 1 Key"} ,
			{4, "Upper 2 Key"} ,
			{5, "Upper 3 Key"} ,
			{6, "Upper 4 Key"} ,
			{7, "Upper 5 Key"} ,
			{8, "Upper 6 Key"} ,
			{9, "Upper 7 Key"} ,
      
			{10, "Upper 8 Key"} ,
      
			{11, "Upper 9 Key"} ,
			{12, "Upper 0 Key"} ,

			{13, "Backspace Key"} ,       // m: "Delete Key"
			{14, "Escape Key"} ,

			{15, "F1 Key"} ,
			{16, "F2 Key"} ,
			{17, "F3 Key"} ,
			{18, "F4 Key"} ,
			{19, "F5 Key"} ,
			{20, "F6 Key"} ,
			{21, "F7 Key"} ,
			{22, "F8 Key"} ,
			{23, "F9 Key"} ,
			{24, "F10 Key"} ,
			{25, "F11 Key"} ,
			{26, "F12 Key"} ,
			{27, "F13 Key"} ,
			{28, "F14 Key"} ,
			{29, "F15 Key"} ,
			
			{0x7F, "F16 Key"} ,             // w+
			{0x80, "F17 Key"} ,             // w+
			{0x81, "F18 Key"} ,             // w+
			{0x82, "F19 Key"} ,             // w+
			{0x83, "F20 Key"} ,             // w+
			{0x84, "F21 Key"} ,             // w+
			{0x85, "F22 Key"} ,             // w+
			{0x86, "F23 Key"} ,             // w+
			{0x87, "F24 Key"} ,             // w+
      
			{31, "A Key"} ,
			{32, "B Key"} ,
			{33, "C Key"} ,
			{34, "D Key"} ,
			{35, "E Key"} ,
			{36, "F Key"} ,
			{37, "G Key"} ,
			{38, "H Key"} ,
			{39, "I Key"} ,
			{40, "J Key"} ,
			{41, "K Key"} ,
			{42, "L Key"} ,
			{43, "M Key"} ,
			{44, "N Key"} ,
			{45, "O Key"} ,
			{46, "P Key"} ,
			{47, "Q Key"} ,
			{48, "R Key"} ,
			{49, "S Key"} ,
			{50, "T Key"} ,
			{51, "U Key"} ,
			{52, "V Key"} ,
			{53, "W Key"} ,
			{54, "X Key"} ,
			{55, "Y Key"} ,
			{56, "Z Key"} ,

			{57, "Help Key"} ,            // v?

          // *** NOTE: the order of these directional key signals
					// ***       is different than on serius/m.  For example, ,
          // ***       the "Forward Delete Key" signal in serius/m
          // ***       appears in the midst of directional key signals.

			{58, "Home Key"} ,
			{59, "Page Up Key"} ,
			{60, "Delete Key"} ,          // m: "Forward Delete Key" in Serius/M
			{61, "End Key"} ,

			{62, "Page Down Key"} ,

			{63, "Left Arrow Key"} ,
			{64, "Right Arrow Key"} ,
			{65, "Up Arrow Key"} ,
			{66, "Down Arrow Key"} ,

			
			//{0x2D, "Insert Key"} ,          // w+
			{68, "Insert Key"},

			{ 67, "Clear Key"} ,           // v?

      // -------------------------- // m+ "Numeric = Key"

			{69, "Numeric / Key"} ,
			{70, "Numeric * Key"} ,
			{71, "Numeric - Key"} ,
			{72, "Numeric + Key"} ,
			{73, "Numeric . Key"} ,

			{74, "Numeric 0 Key"} ,
			{75, "Numeric 1 Key"} ,
			{76, "Numeric 2 Key"} ,
			{77, "Numeric 3 Key"} ,
			{78, "Numeric 4 Key"} ,
			{79, "Numeric 5 Key"} ,
			{80, "Numeric 6 Key"} ,
			{81, "Numeric 7 Key"} ,
			{82, "Numeric 8 Key"} ,
			{83, "Numeric 9 Key"} ,

			{85, "Shift Key"} ,
			{86, "Control Key"} ,
			{87, "Alt Key"} ,             // m: "Option Key"

      // -------------------------- // m+ "Command Key"

			{30, "Tab Key"} ,

 /**** Windows strays: ****/
 /* new ids assigned on Jan. 13, 1994 */
 
			{90, "Left Mouse Button"} ,   // w+
			{91, "Right Mouse Button"} ,  // w+
			{92, "Cancel Key"} ,          // w+, , v?
			{93, "Middle Mouse Button"} , // w+

			{94, "Pause Key"} ,           // w+
			{95, "Caps Lock Key"} ,       // w+

			{96, "Select Key"} ,          // w+, oldSignalID, v?

			{97, "Execute Key"} ,         // w+, , v?
			{98, "Print Screen Key"} ,    // w+

			{99, "Numeric Separator Key"} ,  // w+, , v?

			{100, "Num Lock Key"} ,        // w+
			{101, "Scroll Lock Key"    }  // w+
		
  };

  MEMBLOCKID sigsBlockID;
  pAObjSignalRecord theSigs;

	sigsBlockID = (MEMBLOCKID) system->message2;

  AMemSetSize(sigsBlockID, sizeof(keyboardSignals));
  theSigs = (pAObjSignalRecord)AMemLock(sigsBlockID);
  _fmemcpy(theSigs, &keyboardSignals, sizeof(keyboardSignals));
  }

