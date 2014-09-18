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
// FILE:          "wlay.h"
//
// AUTHOR:        
//
// DESCRIPTION:   
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef WLAY_H
#define WLAY_H

// forward class definitions ///////////////////////////////////////////////

class Layout;

// function prototypes /////////////////////////////////////////////////////

// functions in "lay_util.cpp"
void GetPalettePositions(LPSTR szBuffer, int width, int height, LPPOINT lppt, BOOL* lpbShow);
void InitPaletteMenuString(BOOL fVisible, HMENU hmenu, UINT nMenuCmd, LPCSTR lpcszPalName);
void SubclassHwndItem(HWND hwndItem);
void UnsubclassHwndItem(HWND hwndItem);
void MakeRealRect(LPRECT lprcDst, LPRECT lprcSrc);

LRESULT CALLBACK _export AWLayoutWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK _export AWFrameWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK _export AWClientWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK _export AWMenuWndProc(HWND, UINT, WPARAM, LPARAM);

// typedefs for the window layout //////////////////////////////////////////

typedef BOOL (far pascal *GETPLACEPROC)(HWND, LPWINDOWPLACEMENT);

// defines for the window layout ///////////////////////////////////////////

// signature for the Novell window layout
#define LAYOUT_SIGNATURE          MAKEID('w', 'L', 'a', 'y')
#define LAYOUT_VERSION            0x0100

#define WM_ENDLAYOUT              (WM_USER + 100)

// special define for point in item
#define OWND_POINT_IN_ITEM        (-1)

// defines for extra bytes in the windows
#define I_GWL_LAYOUT              0                 // valid for layout window
#define I_GWL_WINDOW              0                 // valid for window obj windows
#define I_GWL_SHAPES              sizeof(void*)     // valid for client window
#define I_GWL_MENUDATA            sizeof(void*)     // valid for menu window

// defines for menu indexes
#define MENU_FILE                 0
#define MENU_EDIT                 1
#define MENU_POSITION             2
#define MENU_ARRANGE              3
#define MENU_TOOLS                4
#define MENU_PALETTES             5
#define MENU_HELP                 6

// TRACK_XXX flags
#define TRACK_NONE                0x00000000L

// mutually exclusive tracking states
#define TRACK_RESIZETL            0x00000001L
#define TRACK_RESIZETR            0x00000002L
#define TRACK_RESIZEBL            0x00000003L
#define TRACK_RESIZEBR            0x00000004L
#define TRACK_RESIZET             0x00000005L
#define TRACK_RESIZEL             0x00000006L
#define TRACK_RESIZEB             0x00000007L
#define TRACK_RESIZER             0x00000008L
#define TRACK_RESIZEOTHER         0x00000009L
#define TRACK_RESIZEMASK          0x0000000FL

#define TRACK_MOVE                0x00000010L
#define TRACK_SELECT              0x00000020L
#define TRACK_NEWSHAPE            0x00000040L

#define TRACK_WAITFORDELTA        0x00001000L
#define TRACK_DELTA               10    // one millimeter

#ifdef OP_WND_H
#if ((TRACK_RESIZETL != OWND_POINT_IN_TL) \
              ||                          \
     (TRACK_RESIZETR != OWND_POINT_IN_TR) \
              ||                          \
     (TRACK_RESIZEBL != OWND_POINT_IN_BL) \
              ||                          \
     (TRACK_RESIZEBR != OWND_POINT_IN_BR) \
              ||                          \
     (TRACK_RESIZEL  != OWND_POINT_IN_L)  \
              ||                          \
     (TRACK_RESIZET  != OWND_POINT_IN_T)  \
              ||                          \
     (TRACK_RESIZER  != OWND_POINT_IN_R)  \
              ||                          \
     (TRACK_RESIZEB  != OWND_POINT_IN_B))

#error TRACK_RESIZEx must be the same as OWND_POINT_IN_x
#endif
#endif

// SetPoint ////////////////////////////////////////////////////////////////

inline void SetPoint
  (
  POINT FAR*  lppt,
  int         nX,
  int         nY
  )

  {
  lppt->x = nX;
  lppt->y = nY;
  }

// CopyPoint ///////////////////////////////////////////////////////////////

inline void CopyPoint
  (
  POINT FAR*        lppt1,
  const POINT FAR*  lppt2
  )

  {
  lppt1->x = lppt2->x;
  lppt1->y = lppt2->y;
  }

// EqualPoint //////////////////////////////////////////////////////////////

inline BOOL EqualPoint
  (
  const POINT FAR*  lppt1,
  const POINT FAR*  lppt2
  )

  {
  return ((lppt1->x==lppt2->x) && (lppt1->y==lppt2->y));
  }

// SetPointEmpty ///////////////////////////////////////////////////////////

inline void SetPointEmpty
  (
  POINT FAR*  lppt
  )

  {
  lppt->x = 0;
  lppt->y = 0;
  }

// nMin ////////////////////////////////////////////////////////////////////

inline int nMin
  (
  int n1,
  int n2
  )

  {
  return (n1<n2) ? n1 : n2;
  }

// nMax ////////////////////////////////////////////////////////////////////

inline int nMax
  (
  int n1,
  int n2
  )

  {
  return (n1>n2) ? n1 : n2;
  }

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef WLAY_H
