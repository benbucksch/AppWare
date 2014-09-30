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
// FILE:          "lay_dlg.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef LAY_DLG_H
#define LAY_DLG_H

////////////////////////////////////////////////////////////////////////////

#ifndef XP_MACRO_H
#include "xp_macro.h"
#endif

////////////////////////////////////////////////////////////////////////////

// dialog utility routines
void SetItemValue(HWND hDlg, UINT idCtrl, short nVal, WORD fUnits);
short GetItemValue(HWND hDlg, UINT idCtrl, WORD fUnits);

// forward class definition
class OpWndItemD;
class Items;

////////////////////////////////////////////////////////////////////////////
// routines for the grid dialog
////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK EXPORT OWLayGridDlgProc(HWND, UINT, WPARAM, LPARAM);
void GridDlgCommand(HWND hwnd, int idCtl, HWND hwndCtl, UINT uNtfyCode);
void InitGridDlg(HWND hDlg);
BOOL UpdateGrid(HWND hDlg);

struct GRID_DLG
  {
  POINT     ptGrid;
  WORD      fUnits;
  AOBJECTID  oiWnd;
  BOOL      fShowGrid;
  BOOL      fSnapToGrid;
  };

////////////////////////////////////////////////////////////////////////////
// routines for the attach dialog
////////////////////////////////////////////////////////////////////////////
#define BUTTON_WIDTH                  80
#define BUTTON_HEIGHT                 57
#define BUTTON_WINDOWOFFSET_LEFT      6
#define BUTTON_WINDOWOFFSET_TOP       6
#define BUTTON_WINDOW_WIDTH           70
#define BUTTON_WINDOW_HEIGHT          47

#define ANCHOR_LEFT                   0x08
#define ANCHOR_TOP                    0x04
#define ANCHOR_RIGHT                  0x02
#define ANCHOR_BOTTOM                 0x01

// ATTACHBMP structure
struct ATTACHBMP
  {
  HBITMAP hbmpUp;
  HBITMAP hbmpDown;
  int     nUseCount;
  };

// class AnchorItem
class AnchorItem
  
  {
  private:
    static ATTACHBMP  sm_bmp[];

    BOOL    m_fMultipleItems;
    int     m_nOriginalState;
    int     m_nCurrentState;
    char*   m_szTitle;

  public:
            AnchorItem(int nOrigState, BOOL fMultipleItems, char* szTitle = NULL);
           ~AnchorItem();

    inline int  GetAnchorFlags()      {return m_nCurrentState;}

    static BOOL WINAPI EXPORT DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static void Command(HWND hwnd, int idCtl, HWND hwndCtl, UINT uNtfyCode);

    void    InitDialog(HWND hwnd);
    void    DrawAnchorBitmap(int nItem, LPDRAWITEMSTRUCT lpdis);
    void    DrawCurrentSample(LPDRAWITEMSTRUCT lpdis);
  };

////////////////////////////////////////////////////////////////////////////
// routines for the preferences dialog
////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK EXPORT OWLayPreferencesDlgProc(HWND, UINT, WPARAM, LPARAM);
void PreferencesDlgCommand(HWND hwnd, int idCtl, HWND hwndCtl, UINT uNtfyCode);
void InitPreferencesDlg(HWND hDlg);
BOOL UpdatePreferences(HWND hDlg);

struct PREF_DLG
  {
  short fLayPref;
  };

////////////////////////////////////////////////////////////////////////////
// routines for the item sizes dialog
////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK EXPORT OWLayItemSizesDlgProc(HWND, UINT, WPARAM, LPARAM);
void ItemSizesDlgCommand(HWND hwnd, int idCtl, HWND hwndCtl, UINT uNtfyCode);
void InitPosAndSizeDlg(HWND hDlg);
void UpdatePosAndSize(HWND hDlg);

struct POSSIZE_DLG
  {
  WORD        fUnits;
  int         nSelections;
  OpWndItemD* wi;
  Items*      pItems;
  };

////////////////////////////////////////////////////////////////////////////
// routines for the rename dialog
////////////////////////////////////////////////////////////////////////////
#define MAX_NAMELEN                   64

BOOL CALLBACK EXPORT OWLayRenameDlgProc(HWND, UINT, WPARAM, LPARAM);
void RenameDlgCommand(HWND hwnd, int idCtl, HWND hwndCtl, UINT uNtfyCode);
void InitRenameDlg(HWND hDlg);
BOOL UpdateName(HWND hDlg);

struct RENAME_DLG
  {
  AOBJECTID  oiItem;
  };

////////////////////////////////////////////////////////////////////////////
// routines for sizes dialog
////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK EXPORT OWLayWndSizesDlgProc(HWND, UINT, WPARAM, LPARAM);
void SizesDlgCommand(HWND hwnd, int idCtl, HWND hwndCtl, UINT uNtfyCode);
void InitSizesDlg(HWND hDlg);
BOOL UpdateSizes(HWND hDlg);

struct WNDSIZES_DLG
  {
  long      style;
  RECT      rcClient;
  POINT     ptMin;
  POINT     ptMax;
  POINT     ptScroll;
  BOOL      fLimit;
  WORD      fUnits;
  };

////////////////////////////////////////////////////////////////////////////
// routines for the style dialog
////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK EXPORT OWLayWndStyleDlgProc(HWND, UINT, WPARAM, LPARAM);
void StyleDlgCommand(HWND hwnd, int idCtl, HWND hwndCtl, UINT uNtfyCode);
void InitStyleDlg(HWND hDlg);
void FillMenuBarList(HWND hDlg);
BOOL UpdateWndStyle(HWND hDlg);
void DrawSampleWindow(HWND hDlg);
void PaintSampleWidow(HWND hwnd);
LRESULT CALLBACK EXPORT OWLaySampleWndProc(HWND, UINT, WPARAM, LPARAM);

struct WNDSTYLE_DLG
  {
  char      szTitle[256];
  long      style;
  AOBJECTID  oiMenu;
  };

////////////////////////////////////////////////////////////////////////////

#endif  // LAY_DLG_H
