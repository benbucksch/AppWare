////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//  WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//  OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//  HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:        OBJLIST.CPP
//
// AUTHOR:      Scott McCarty
//
// DESCRIPTION: Manages the object list window for window layout.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

#include <stdio.h>
#include <string.h>
#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>
#include <op_wnd.h>
#include <o_wnd.h>
#include <o_wndlay.h>

#include "wlay.h"
#include "heap.h"
#include "hierlist.h"
#include "objlist.h"
#include "layout.h"
#include "rc\wlay_rsc.h"
#include "objlisti.h"

//////////////// macros

#define SelectBitmap(hdc, hbm)  ((HBITMAP)SelectObject((hdc), (HGDIOBJ)(HBITMAP)(hbm)))
#define ALLTYPES                NULL
#define SZ_ALLTYPES             "<All Types>"

//////////////// Forward function prototypes

void CreateNewVI(Layout*, TYPEID, ApQueryInfo*, POINT);
HWND CreateObjectPaletteWindow(Layout*, HWND, POINT);
void CheckDropPoint(POINT* ptDrop, POINT* ptlOff, RECT* rcWnd);

//////////////// Global variables

extern HINSTANCE    g_hInstance;
extern const char*  c_szAppBuilderIni;

//////////////// Static variables

static ObjectPaletteWindow*   s_pObjPalette;    // main window (contains types&objs)
static short                  s_nResizeBar;     // 1/16"
static short                  s_nMinListSize;   // 1/2"

const LONG                    c_lObjectTypeItem     = 1;
const LONG                    c_lObjectObjectItem   = 2;

char          ObjectPaletteWindow::s_szWndClass[] = "ApwObjPal";
HINSTANCE     ObjectPaletteWindow::s_hInstance;

// ****************************************************************
//
// Overridden operator new and delete so we don't eat up selectors
// with small C++ objects.
//
// ****************************************************************

void*   operator new(
  size_t    s
  )
  {
  return MemAlloc(s);
  }

void    operator delete(
  void*     p
  )
  {
  if (p)
    MemFree(p);
  }

/* **************************************************************** */
/* Utility functions                                                */
/* **************************************************************** */

////////////////////////////////////////////////////////////////
//
// FUNCTION:  MaintainRatio
//
// DESCR:     Calculates the new first value given a ratio between
//            two other values.
//
////////////////////////////////////////////////////////////////

static void   MaintainRatio(
  short         nFirstSize,         
  short         nSecondSize,
  short         nNewTotalSize,      // total thing to split
  short&        nNewFirstSize       // corresponds to nFirstSize
  )
  {
  nNewFirstSize = MulDiv(nNewTotalSize, nFirstSize,
                          (nFirstSize + nSecondSize));
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:  GetTypeBitmap
//
// DESCR:     Returns a small version of the type's icon displayed
//            on the current window background color.
//
////////////////////////////////////////////////////////////////

static HBITMAP  GetTypeBitmap(
  TYPEID          tType,
  short           cxDest,             // desired width of bitmap
  short           cyDest              // desired height of bitmap
  )
  {
  HDC             hScreenDC   = GetDC(NULL);
  int             cxIconSize  = GetSystemMetrics(SM_CXICON);
  int             cyIconSize  = GetSystemMetrics(SM_CYICON);
  HBITMAP         hSrcBmp;
  HBITMAP         hDstBmp;
  HDC             hSrcDC;
  HDC             hDstDC;
  HICON           hTypeIcon;
  HBRUSH          hBackground;
  HBITMAP         hOldSrcBmp;
  HBITMAP         hOldDstBmp;
  RECT            rcSrc;

  I_ASSERT(tType != NULL);
  I_ASSERT(cxDest > 0 && cyDest > 0);

  hTypeIcon = ATypeGetIcon(tType);

  I_ASSERT(hTypeIcon);

  // Create icon-sized and desired-size bitmaps and DCs to put them in
  hSrcBmp = CreateCompatibleBitmap(hScreenDC, cxIconSize, cyIconSize);
  hDstBmp = CreateCompatibleBitmap(hScreenDC, cxDest,     cyDest);
  hSrcDC  = CreateCompatibleDC(hScreenDC);
  hDstDC  = CreateCompatibleDC(hScreenDC);

  hOldSrcBmp = SelectBitmap(hSrcDC, hSrcBmp);
  hOldDstBmp = SelectBitmap(hDstDC, hDstBmp);

  // Fill full-sized bitmap with window background color
  hBackground = CreateSolidBrush(GetSysColor(COLOR_WINDOW));

  rcSrc.left   = 0;
  rcSrc.top    = 0;
  rcSrc.bottom = cyIconSize;
  rcSrc.right  = cxIconSize;

  FillRect(hSrcDC, &rcSrc, hBackground);
  DeleteObject(hBackground);

  // Draw the icon to the full-sized bitmap
  DrawIcon(hSrcDC, 0, 0, hTypeIcon);

  // Now stretch-blt the bit bitmap to the small bitmap
  StretchBlt(hDstDC, 0, 0, cxDest, cyDest,
              hSrcDC, 0, 0, cxIconSize, cyIconSize, SRCCOPY);

  // Release and delete all temporary handles
  SelectBitmap(hSrcDC, hOldSrcBmp);
  SelectBitmap(hDstDC, hOldDstBmp);

  DeleteDC(hDstDC);
  DeleteDC(hSrcDC);

  DeleteObject(hSrcBmp);

  DestroyIcon(hTypeIcon);
  ReleaseDC(NULL, hScreenDC);

  return hDstBmp;
  }

/* **************************************************************** */
/* ObjectTypeItem methods                                           */
/* **************************************************************** */

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectTypeItem::ObjectTypeItem
//
// DESCR:   Initializes and inserts the type item into the object list.
//
////////////////////////////////////////////////////////////////

ObjectTypeItem::ObjectTypeItem(
  TYPEID            tType,
  SItemListWindow*  pMaster
  ) :
    SListItem(pMaster, c_lObjectTypeItem),
    m_type(tType)
  {
  int         nBmpSize          = GetSystemMetrics(SM_CXICON) / 2;
  char        szTypeName[32];

  ADBG_ASSERT_E(pMaster != NULL, ;, "Invalid pMaster passed to ObjectTypeItem");

  if (tType == ALLTYPES)
    {
    SetGreedy();
    SetNoDrag();
    strcpy(szTypeName, SZ_ALLTYPES);
    }
  else
    ATypeGetName(tType, szTypeName, sizeof(szTypeName));

  SetTitle(szTypeName);
  SetUID((LONG) tType);

  if (tType != NULL)
    m_hImage = GetTypeBitmap(tType, nBmpSize, nBmpSize);
  else
    m_hImage = NULL;

  if (m_hImage)
    SetImage(m_hImage);

  pMaster->AddItem(this, NULL, 0);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectTypeItem::~ObjectTypeItem
//
// DESCR:   Cleans up an ObjectTypeItem by freeing its resources
//
////////////////////////////////////////////////////////////////

ObjectTypeItem::~ObjectTypeItem(
  void
  )
  {
  if (m_hImage)
    DeleteObject(m_hImage);
  }

/* **************************************************************** */
/* ObjectItem methods                                               */
/* **************************************************************** */

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectItem::ObjectItem
//
// DESCR:   Initializes and inserts the object item into the object list.
//
////////////////////////////////////////////////////////////////

ObjectItem::ObjectItem(
  OBJECTID          oiObject,
  ApQueryInfo*      pQPInfo,
  SItemListWindow*  pMaster
  ) :
    SListItem(pMaster, c_lObjectObjectItem),
    m_oiObject(oiObject)
  {
  int         nBmpSize          = GetSystemMetrics(SM_CXICON) / 2;
  char        szObjectName[32];
  TYPEID      tType;

  ADBG_ASSERT_E(oiObject != 0, ;, "Invalid object passed to ObjectType");
  ADBG_ASSERT_E(pMaster != NULL, ;, "Invalid pMaster passed to ObjectType");
  ADBG_ASSERT_E(pQPInfo != NULL, ;, "Invalid pQPInfo passed to ObjectType");

  m_QueryInfo = *pQPInfo;

  AObjGetName(oiObject, szObjectName, sizeof(szObjectName));
  SetTitle(szObjectName);

  tType = AObjGetType(oiObject);
  m_hImage = GetTypeBitmap(tType, nBmpSize, nBmpSize);
  SetImage(m_hImage);

  pMaster->AddItem(this, NULL, 0);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectItem::~ObjectItem
//
// DESCR:   Cleans up an object item object
//
////////////////////////////////////////////////////////////////

ObjectItem::~ObjectItem(
  void
  )
  {
  if (m_hImage)
    DeleteObject(m_hImage);
  }

/* **************************************************************** */
/* ObjectListWindow methods                                         */
/* **************************************************************** */

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectListWindow::ObjectListWindow
//
// DESCR:   Initializes the new object list window.
//
////////////////////////////////////////////////////////////////

ObjectListWindow::ObjectListWindow(
  ObjectPaletteWindow*  pParent,
  Layout*               pLayout,
  HWND                  hParent,
  LONG                  lOptions,
  DWORD                 dwWindowStyle,
  BOOL                  fTypes
  ) :
    SItemListWindow(hParent, lOptions, dwWindowStyle),
    m_pLayout(pLayout),
    m_fTypes(fTypes),
    m_pParent(pParent)
  {
  I_ASSERT(pParent != NULL);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectListWindow::~ObjectListWindow
//
// DESCR:   Cleans up an object list window object.
//
////////////////////////////////////////////////////////////////

ObjectListWindow::~ObjectListWindow(
  void
  )
  {
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectListWindow::GetCursor
//
// DESCR:   Returns UI standard cursors for drag operations.
//
////////////////////////////////////////////////////////////////

BOOL    ObjectListWindow::GetCursor(
  short       nMsg,
  SListItem*  pItem,                  // non-null for single item drag
  HCURSOR&    hCursor
  )
  {
  I_UNUSED(pItem);

  switch (nMsg)
    {
    case sourceOfDrag:
      hCursor = LoadCursor(g_hInstance, ID(TMP_SRCCURSOR));
      break;

    case targetForDrag:
      hCursor = LoadCursor(g_hInstance, ID(TMP_DSTCURSOR));
      break;

    case invalidTarget:
      hCursor = LoadCursor(g_hInstance, ID(TMP_INVCURSOR));
    }

  return TRUE;                        // tells SItemListWindow to destroy them
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectListWindow::MouseDblClick
//
// DESCR:   Handles an item in the list being double clicked.
//
////////////////////////////////////////////////////////////////

void ObjectListWindow::ItemDblClicked(
  SListItem*  pHitItem
  )
  {
  I_UNUSED(pHitItem);

  Window*           pWnd        = m_pLayout->ActiveWindow();
  short             nKey;
  SListItem*        pItem;
  ObjectTypeItem*   pTypeItem;
  ObjectItem*       pObjItem;
  ApQueryInfo       queryInfo;        // used only for type adds

  SetActiveWindow(m_pLayout->WndHandle());
  for (nKey = -1; ;)
    {
    pItem = EnumItems(nKey, SListItem::selected);
    if (pItem == NULL)
      break;

    switch (pItem->UserType())
      {
      case c_lObjectTypeItem:
        pTypeItem = (ObjectTypeItem*) pItem;
        queryInfo.object = 0;
        CreateNewVI(m_pLayout, pTypeItem->TypeID(), &queryInfo, pWnd->GetNextAddPoint());

        // Force a reload of the object list
        ForceSelectionChange();
        break;

      case c_lObjectObjectItem:
        pObjItem = (ObjectItem*) pItem;
        if(AObjCheckType(pObjItem->ObjectID(), 0))
          CreateNewVI(m_pLayout, 0, pObjItem->ProtocolInfo(), pWnd->GetNextAddPoint());
        break;

      default:
        I_ASSERT(0);
        break;
      }
    }
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectListWindow::DragRelease
//
// DESCR:   Handles the actual drop of the objects.
//
////////////////////////////////////////////////////////////////

void    ObjectListWindow::DragRelease(
  HWND    hDropWnd,
  POINT   ptDrop
  )
  {
  POINT             ptlOff    = {DROP_OFFSETX, DROP_OFFSETY}; // logical points
  Window*           pWnd      = (Window*) GetWindowLong(hDropWnd, I_GWL_WINDOW);
  short             nKey;
  SListItem*        pItem;
  ObjectTypeItem*   pTypeItem;
  ObjectItem*       pObjItem;
  ApQueryInfo       queryInfo;        // used only for type drags
  RECT              rcWnd;

  m_pLayout->ActivateWindow(pWnd);
  SetActiveWindow(m_pLayout->WndHandle());

  I_UNUSED(hDropWnd);

  GetClientRect(pWnd->ClientWnd(), &rcWnd);
  if(rcWnd.right>0)
    rcWnd.right--;
  if(rcWnd.bottom>0)
    rcWnd.bottom--;
  OWndDPtoLP(pWnd->ObjectID(), (LPPOINT)&rcWnd, 2);

  if (pWnd->SnapToGrid())
    {
    POINT ptGrid;

    // make sure the original drop point is on a grid point
    OWndDPtoLP(pWnd->ObjectID(), &ptDrop, 1);
    pWnd->SnapPointToGrid(&ptDrop);

    // force a maximum offset of (DROP_OFFSETX, DROP_OFFSETY) unless the grid is set bigger
    pWnd->GetGrid(&ptGrid);
    if (ptGrid.x>0)
      {
      ptlOff.x = ptGrid.x;
      while((ptlOff.x + ptGrid.x) <= DROP_OFFSETX)
        ptlOff.x += ptGrid.x;
      }
    if (ptGrid.y>0)
      {
      ptlOff.y = ptGrid.y;
      while ((ptlOff.y + ptGrid.y) <= DROP_OFFSETY)
        ptlOff.y += ptGrid.y;
      }

    // make sure the drop point is visible
    CheckDropPoint(&ptDrop, &ptlOff, &rcWnd);
    OWndLPtoDP(pWnd->ObjectID(), &ptDrop, 1);
    }

  for (nKey = -1; ;)
    {
    pItem = EnumItems(nKey, SListItem::selected);
    if (pItem == NULL)
      break;

    switch (pItem->UserType())
      {
      case c_lObjectTypeItem:
        pTypeItem = (ObjectTypeItem*) pItem;
        queryInfo.object = 0;
        CreateNewVI(m_pLayout, pTypeItem->TypeID(), &queryInfo, ptDrop);

        // Force a reload of the object list
        ForceSelectionChange();
        break;

      case c_lObjectObjectItem:
        pObjItem = (ObjectItem*) pItem;
        if(AObjCheckType(pObjItem->ObjectID(), 0))
          CreateNewVI(m_pLayout, 0, pObjItem->ProtocolInfo(), ptDrop);
        break;

      default:
        I_ASSERT(0);
        break;
      }

    // offset each item so all dropped items are visible
    OWndDPtoLP(pWnd->ObjectID(), &ptDrop, 1);
    pWnd->SnapPointToGrid(&ptDrop);  // no-op if no snap
    ptDrop.x += ptlOff.x;
    ptDrop.y += ptlOff.y;
    CheckDropPoint(&ptDrop, &ptlOff, &rcWnd);
    OWndLPtoDP(pWnd->ObjectID(), &ptDrop, 1);
    }
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectListWindow::SelectionChange
//
// DESCR:   Refills the object list if the selection of the types
//          changes.  This is called after the button is released.
//
////////////////////////////////////////////////////////////////

void    ObjectListWindow::SelectionChange(
  void
  )
  {
  if (m_fTypes)
    m_pParent->FillObjectList();
  }

/* **************************************************************** */
/* ObjectPaletteWindow methods                                      */
/* **************************************************************** */

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::Init
//
// DESCR:   Initializes the palette window globals, etc.
//
////////////////////////////////////////////////////////////////

BOOL  ObjectPaletteWindow::Init(
  HINSTANCE   hInstance
  )
  {
  WNDCLASS      wc;
  BOOL          fInitOK;
  HDC           hDC;

  s_hInstance = hInstance;

  wc.style          = CS_BYTEALIGNWINDOW | CS_DBLCLKS;
  wc.lpfnWndProc    = WndProc;
  wc.cbClsExtra     = 0;
  wc.cbWndExtra     = sizeof(ObjectPaletteWindow*);
  wc.hInstance      = s_hInstance;
  wc.hIcon          = NULL;
  wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground  = (HBRUSH) (COLOR_WINDOW + 1);
  wc.lpszMenuName   =  NULL;
  wc.lpszClassName  = s_szWndClass;

  fInitOK = RegisterClass(&wc);

  hDC = GetDC(NULL);      // get screen dc

  if (hDC)
    {
    s_nMinListSize = GetDeviceCaps(hDC, LOGPIXELSX) / 2;  // 1/2"
    s_nResizeBar   = s_nMinListSize / 15;                 // 1/20"

    ReleaseDC(NULL, hDC);
    }
  else
    {
    s_nResizeBar   = 96 / 30;     // 1/20"
    s_nMinListSize = 96 / 2;      // 1/2"
    }

  return fInitOK;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::WndProc
//
// DESCR:   This is the WindowProc for the palette window.   It
//          forwards necessary messages on to the corresponding methods
//          on the object.
//
////////////////////////////////////////////////////////////////

LRESULT CALLBACK _export    ObjectPaletteWindow::WndProc(
  HWND                          hWnd,
  UINT                          nMsg,
  WPARAM                        wParam,
  LPARAM                        lParam
  )
  {
  LRESULT                 lRes          = 0;
  BOOL                    fCallDefault  = FALSE;
  ObjectPaletteWindow*    pList;

  pList = (ObjectPaletteWindow*) GetWindowLong(hWnd, 0);

  switch (nMsg)
    {
    case WM_CREATE:
      pList = (ObjectPaletteWindow*) (((LPCREATESTRUCT) lParam)->lpCreateParams);
      SetWindowLong(hWnd, 0, (LONG) pList);
      break;

    case WM_GETMINMAXINFO:
      if (pList != NULL)
        {
        // set up default information (could do it in method, but
        // I'd have to pass wParam in)
        DefWindowProc(hWnd, nMsg, wParam, lParam);
        lRes = pList->GetMinMaxInfo((MINMAXINFO*) lParam);
        }
      else
        fCallDefault = TRUE;
      break;

    case WM_SIZE:
      I_ASSERT(pList != NULL);

      lRes = pList->Resize((short) LOWORD(lParam), (short) HIWORD(lParam));
      break;

    case WM_LBUTTONDOWN:
      {
      POINT   ptDown;

      I_ASSERT(pList != NULL);

      ptDown.x = LOWORD(lParam);
      ptDown.y = HIWORD(lParam);

      lRes = pList->MouseDown(&ptDown);
      break;
      }

    // Kind of a funny overload, but they need to do the same stuff
    case WM_LBUTTONUP:
    case WM_CANCELMODE:
      {
      POINT   ptUp;

      I_ASSERT(pList != NULL);

      ptUp.x = LOWORD(lParam);
      ptUp.y = HIWORD(lParam);

      lRes = pList->MouseUp(&ptUp, (nMsg == WM_LBUTTONUP));
      break;
      }

    case WM_MOUSEMOVE:
      {
      POINT   ptMove;

      I_ASSERT(pList != NULL);

      ptMove.x = LOWORD(lParam);
      ptMove.y = HIWORD(lParam);

      lRes = pList->MouseMove(&ptMove);
      break;
      }

    case WM_PAINT:
      {
      PAINTSTRUCT   ps;
      HDC           hDC;

      I_ASSERT(pList != NULL);

      hDC = BeginPaint(hWnd, &ps);

      if (hDC)
        lRes = pList->Draw(hDC, &ps.rcPaint);

      EndPaint(hWnd, &ps);

      break;
      }

    case WM_SYSCOMMAND:
      if (wParam == SC_CLOSE)
        ShowWindow(hWnd, SW_HIDE);
      else
        fCallDefault = TRUE;
      break;

    default:
      fCallDefault = TRUE;
      break;
    }

  if (fCallDefault)
    lRes = DefWindowProc(hWnd, nMsg, wParam, lParam);

  return lRes;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::ObjectPaletteWindow
//
// DESCR:   Creates and initializes the object list window.
//
////////////////////////////////////////////////////////////////

ObjectPaletteWindow::ObjectPaletteWindow(
  Layout*         pLayout,
  HWND            hParent
  ) :
    m_hWnd(NULL),
    m_nTypeSize(1),
    m_nObjSize(1),
    m_pTypeList(NULL),
    m_pObjList(NULL),
    m_nTrackLine(0)
  {
  LONG            lTypeKey;
  TYPEID          tType;
  LONG            lTypeQPStatus;
  ObjectTypeItem* pType;

  ADBG_ASSERT_E(hParent != NULL, return, "Invalid hParent in CreateObjectPaletteWindow");

  m_hWnd = CreateWindow(s_szWndClass, NULL,
                        (WS_BORDER | WS_THICKFRAME | WS_POPUP |
                         WS_CAPTION | WS_SYSMENU),
                        0, 0, 0, 0,
                        hParent, NULL, s_hInstance, this);

  if (m_hWnd != NULL)
    {
    m_pTypeList = new ObjectListWindow(this, pLayout, m_hWnd,
                                    SItemListWindow::allowDragDrop |
                                    SItemListWindow::sortItems,
                                    WS_VISIBLE | WS_CHILD | WS_BORDER,
                                    TRUE);

    m_pObjList  = new ObjectListWindow(this, pLayout, m_hWnd,
                                    SItemListWindow::allowDragDrop,
                                    WS_VISIBLE | WS_CHILD | WS_BORDER,
                                    FALSE);

    // Must have total success
    if (m_pTypeList == NULL || m_pObjList == NULL)
      {
      delete m_pTypeList;
      delete m_pObjList;

      DestroyWindow(m_hWnd);
      m_hWnd = NULL;
      }
    else
      {
      HMENU       hSysMenu;

      hSysMenu = GetSystemMenu(m_hWnd, FALSE);

      // Keep positions 1,2 and 6 (MOVE, SIZE, and CLOSE); delete in reverse order
      DeleteMenu(hSysMenu, 8, MF_BYPOSITION);
      DeleteMenu(hSysMenu, 7, MF_BYPOSITION);
      DeleteMenu(hSysMenu, 5, MF_BYPOSITION);
      DeleteMenu(hSysMenu, 4, MF_BYPOSITION);
      DeleteMenu(hSysMenu, 3, MF_BYPOSITION);
      DeleteMenu(hSysMenu, 0, MF_BYPOSITION);

      lTypeKey = AOBJ_GETFIRSTKEY;

      // Add a special case of "<All Types>" at the top.  The other
      // half of the special case is within the constructor (yuck!)
      pType = new ObjectTypeItem(ALLTYPES, m_pTypeList);

      if (pType)
        pType->Select(TRUE);

      while ((tType = ATypeGetNextCfg(&lTypeKey)) != 0)
        {
        // Check if the type supports the Window Item Protocol
        lTypeQPStatus = ATypeQueryProtocol(tType, APROT_WND);

        if (lTypeQPStatus & AP_SUPPORTED)
          {
          // The creation of the object also inserts it into the list.
          // The insertion of the item into the list also transfers
          // ownership, so it will be automatically deleted when the
          // ObjectListWindow is deleted.
          new ObjectTypeItem(tType, m_pTypeList);
          }
        }

      FillObjectList();

      SetFont();
      SetCaption();
      }
    }
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::~ObjectPaletteWindow
//
// DESCR:   Cleans up a object palette window.
//
////////////////////////////////////////////////////////////////

ObjectPaletteWindow::~ObjectPaletteWindow(
  void
  )
  {
  if (m_fFreeCursor)
    DestroyCursor(m_hResizeCursor);

  delete m_pTypeList;
  delete m_pObjList;

  if (m_hWnd != NULL)           // delete after lists, since lists do DestroyWindow()
    DestroyWindow(m_hWnd);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::Initialized
//
// DESCR:   Returns TRUE if the palette window was created successfully.
//
////////////////////////////////////////////////////////////////

BOOL  ObjectPaletteWindow::Initialized(
  void
  ) const
  {
  return (m_hWnd != NULL && m_pTypeList != NULL && m_pObjList != NULL);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::GetResizeCursor
//
// DESCR:   Returns the resize cursor (loads it if necessary).
//
////////////////////////////////////////////////////////////////

HCURSOR   ObjectPaletteWindow::GetResizeCursor(
  void
  )
  {
  if (m_hResizeCursor == NULL)
    {
    m_hResizeCursor = LoadCursor(s_hInstance, MAKEINTRESOURCE(IDC_VSIZEBAR));

    if (m_hResizeCursor == NULL)
      {
      m_hResizeCursor = LoadCursor(NULL, IDC_SIZENS);
      m_fFreeCursor   = FALSE;
      }
    else
      m_fFreeCursor = TRUE;
    }

  return m_hResizeCursor;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::MouseDown
//
// DESCR:   Handles the mouse going down in the palette window.
//
////////////////////////////////////////////////////////////////

LONG    ObjectPaletteWindow::MouseDown(
  POINT*    ptDown
  )
  {
  I_ASSERT(m_nTypeSize <= ptDown->y && ptDown->y <= m_nTypeSize + s_nResizeBar);

  m_fResizing = TRUE;
  SetCapture(Hwnd());

  m_nTrackLine = ConstrainListHeight(ptDown->y);
  DrawTrackLine();

  return 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::MouseMove
//
// DESCR:   Handles the mouse moving in the palette window.
//
////////////////////////////////////////////////////////////////

LONG    ObjectPaletteWindow::MouseMove(
  POINT*    ptMove
  )
  {
  short     nTrack;

  I_ASSERT(m_fResizing ||
    (m_nTypeSize <= ptMove->y && ptMove->y <= m_nTypeSize + s_nResizeBar));

  SetCursor(GetResizeCursor());

  nTrack = ConstrainListHeight(ptMove->y);

  if (m_fResizing && (nTrack != m_nTrackLine))
    {
    DrawTrackLine();                // erase
    m_nTrackLine = nTrack;
    DrawTrackLine();                // draw
    }

  return 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::MouseUp
//
// DESCR:   Handles the mouse going up in the palette window.
//
////////////////////////////////////////////////////////////////

LONG    ObjectPaletteWindow::MouseUp(
  POINT*    ptUp,
  BOOL      fMoving           // else canceling
  )
  {
  if (m_fResizing)
    {
    ReleaseCapture();
    m_fResizing = FALSE;

    DrawTrackLine();              // erase

    if (fMoving)
      {
      m_nTrackLine = ConstrainListHeight(ptUp->y);
      ResizeTypes(m_nTrackLine);
      }
    }

  return 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::GetMinMaxInfo
//
// DESCR:   Returns minimum tracking information.  This is done to
//          provide a minimum height that will accomodate 2 lists.
//
////////////////////////////////////////////////////////////////

LONG    ObjectPaletteWindow::GetMinMaxInfo(
  MINMAXINFO*   pMinMax
  )
  {
  // pMinMax is already setup when this is called.  Just modify min.y
  pMinMax->ptMinTrackSize.y = s_nMinListSize * 2 +
                                GetSystemMetrics(SM_CYCAPTION) +
                                2 * GetSystemMetrics(SM_CYFRAME);

  return 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::Resize
//
// DESCR:   Handles resizing of the window.
//
////////////////////////////////////////////////////////////////

LONG    ObjectPaletteWindow::Resize(
  short   cx,
  short   cy
  )
  {
  RECT    rcClient;
  short   cyClient;
  short   cyTypes;

  I_UNUSED(cx);
  I_UNUSED(cy);

  if (Hwnd() != NULL)       // this is called before returning from CreateWindow()
    {
    // Do this all the time to record the client width
    GetClientRect(Hwnd(), &rcClient);

    m_cxClient = rcClient.right;
    m_cyClient = rcClient.bottom;

    // This is called during the create of the window, so we need to
    // check if we have the objects first...
    if (m_pTypeList != NULL)
      {
      // Use existing ratio to determine real sizes
      cyClient = m_cyClient - s_nResizeBar;

      MaintainRatio(m_nTypeSize, m_nObjSize, cyClient, cyTypes);

      ResizeTypes(cyTypes);
      }
    }

  return 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::Draw
//
// DESCR:   Draws the resize bar (thick black line).
//
////////////////////////////////////////////////////////////////

LONG    ObjectPaletteWindow::Draw(
  HDC     hDC,
  RECT*   prcInvalid
  )
  {
  I_UNUSED(hDC);
  I_UNUSED(prcInvalid);
//!!  HBRUSH  hOldBrush   = (HBRUSH) SelectObject(hDC, GetStockObject(BLACK_BRUSH));

//!!  Rectangle(hDC, prcInvalid->left, prcInvalid->top,
//!!            prcInvalid->right, prcInvalid->bottom);

//!!  SelectObject(hDC, hOldBrush);

  return 0;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::DrawTrackLine
//
// DESCR:   Draws a thick horizontal line at the position given by
//          m_nTrackLine.  It is XORed.
//
////////////////////////////////////////////////////////////////

void    ObjectPaletteWindow::DrawTrackLine(
  void
  )
  {
  HDC       hDC   = GetDC(Hwnd());

  if (hDC)
    {
    HPEN      hLine   = CreatePen(PS_SOLID, 2, RGB(0, 0, 0)); // black
    HPEN      hOldPen;
    int       nROP;

    if (hLine)
      {
      hOldPen = (HPEN) SelectObject(hDC, hLine);
      nROP    = SetROP2(hDC, R2_NOTXORPEN);

      MoveTo(hDC, 0, m_nTrackLine);
      LineTo(hDC, m_cxClient, m_nTrackLine);

      SetROP2(hDC, nROP);
      SelectObject(hDC, hOldPen);

      DeleteObject(hLine);
      }

    ReleaseDC(Hwnd(), hDC);
    }
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::ConstrainListHeight
//
// DESCR:   Applies limit testing to the desired track line.
//
////////////////////////////////////////////////////////////////

short   ObjectPaletteWindow::ConstrainListHeight(
  short     nNewHeight
  )
  {
  if (nNewHeight < s_nMinListSize)
    nNewHeight = s_nMinListSize;
  else if (nNewHeight > m_cyClient - s_nMinListSize)
    nNewHeight = m_cyClient - s_nMinListSize;

  return nNewHeight;
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::ResizeTypes
//
// DESCR:   Resizes the types list to the given size.  Also resizes
//          the object list to fit in the remaining area.
//
////////////////////////////////////////////////////////////////

void    ObjectPaletteWindow::ResizeTypes(
  short     nNewHeight
  )
  {
  nNewHeight = ConstrainListHeight(nNewHeight);

  m_nTypeSize = nNewHeight;
  m_nObjSize  = m_cyClient - nNewHeight - s_nResizeBar;

  MoveWindow(m_pTypeList->Hwnd(), 0, 0, m_cxClient, m_nTypeSize, TRUE);
  MoveWindow(m_pObjList->Hwnd(),
            0, nNewHeight + s_nResizeBar, m_cxClient, m_nObjSize,
            TRUE);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::SetFont
//
// DESCR:   Sets the font of the object list window to something nicer
//          than the system font.
//
////////////////////////////////////////////////////////////////

void  ObjectPaletteWindow::SetFont(
  void
  )
  {
  LOGFONT     lfFont;

  lfFont.lfHeight         = MulDiv(12, 96, 72);   //!! 12 points @ 96dpi
  lfFont.lfWidth          = 0;
  lfFont.lfEscapement     = 0;
  lfFont.lfOrientation    = 0;
  lfFont.lfWeight         = FW_NORMAL;
  lfFont.lfItalic         = 0;
  lfFont.lfUnderline      = 0;
  lfFont.lfStrikeOut      = 0;
  lfFont.lfCharSet        = ANSI_CHARSET;
  lfFont.lfOutPrecision   = OUT_DEFAULT_PRECIS;
  lfFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
  lfFont.lfQuality        = DEFAULT_QUALITY;
  lfFont.lfPitchAndFamily = (DEFAULT_PITCH | FF_SWISS);

  strncpy(lfFont.lfFaceName, "MS Sans Serif", LF_FACESIZE);

  m_pTypeList->SetFont(&lfFont);
  m_pObjList->SetFont(&lfFont);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::SetCaption
//
// DESCR:   Sets the caption on the object list window.
//
////////////////////////////////////////////////////////////////

void  ObjectPaletteWindow::SetCaption(
  void
  )
  {
  SetWindowText(Hwnd(), "Object List");
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::SetPosition
//
// DESCR:   Positions the window and sizes the subwindows.
//
////////////////////////////////////////////////////////////////

void  ObjectPaletteWindow::SetPosition(
  POINT   ptPos,                  // upper-left corner of palette window
  short   cx,                     // width of palette window
  short   cy,                     // height of palette window
  short   cyTypes,                // height of types subwindow
  short   cyObjs                  // height of objects subwindow
  )
  {
  // Move palette window
  MoveWindow(Hwnd(), ptPos.x, ptPos.y, cx, cy, TRUE);

  m_nTypeSize = cyTypes;
  m_nObjSize  = cyObjs;

  // Resize child windows.
  Resize(cx, cy);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::FillObjectList
//
// DESCR:   Fills the object list proper (as compared to the type list)
//          with the objects that can be inserted.  The current list
//          is cleared before filling it.
//
////////////////////////////////////////////////////////////////

void  ObjectPaletteWindow::FillObjectList(
  void
  )
  {
  LONG              lObjKey;
  short             nTypeEnum;
  ObjectTypeItem*   pType;
  TYPEID            tType;
  OBJECTID          oiObject;
  ApQueryInfo       queryInfo;
  LONG              lObjQPStatus;
  short             nStatus;

  m_pObjList->DeleteAll();

  nTypeEnum = -1;

  nStatus = SListItem::selected;

  // The following assumes that the special object type item of
  // "<All Types>" is FIRST in the type list!!
  for (;;)
    {
    pType = (ObjectTypeItem*) m_pTypeList->EnumItems(nTypeEnum, nStatus);

    if (pType == NULL)            // no more types
      break;

    tType = pType->TypeID();

    // If the special item of "<All Types>" is selected, then select all
    if (tType == ALLTYPES)
      nStatus = SListItem::exists;
    else
      {
      lObjKey = AOBJ_GETFIRSTKEY;

      while ((oiObject = AObjGetNextObject(tType, &lObjKey)) != 0)
        {
#ifdef ADBG_DEBUG
        LONG        lTypeQPStatus = ATypeQueryProtocol(tType, APROT_WND);
#endif
        lObjQPStatus = AObjQueryProtocol(oiObject, &queryInfo, APROT_WND);

#ifdef ADBG_DEBUG
        ADBG_ASSERT_E(((lTypeQPStatus & AP_DYNAMIC) ||
                       (lObjQPStatus & AP_SUPPORTED)), ;,
                      "Type always supports WIP, but object of type doesn't");
#endif

        if (lObjQPStatus & AP_SUPPORTED)
          {
          // The creation of the object also inserts it into the list.
          // The insertion of the item into the list also transfers
          // ownership.
          new ObjectItem(oiObject, &queryInfo, m_pObjList);
          }
        }
      }
    }
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::AddDragTarget
//
// DESCR:   Sets the drag target for the object list window.
//
////////////////////////////////////////////////////////////////

void  ObjectPaletteWindow::AddDragTarget(
  HWND    hDragTarget,
  short   nStatus
  )
  {
  m_pTypeList->AddDragTarget(hDragTarget, nStatus);
  m_pObjList->AddDragTarget(hDragTarget, nStatus);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::RemoveDragTarget
//
// DESCR:   Removes the drag target from the object list window.
//
////////////////////////////////////////////////////////////////

void  ObjectPaletteWindow::RemoveDragTarget(
  HWND    hDragTarget
  )
  {
  m_pTypeList->RemoveDragTarget(hDragTarget);
  m_pObjList->RemoveDragTarget(hDragTarget);
  }

////////////////////////////////////////////////////////////////
//
// METHOD:  ObjectPaletteWindow::Enable
//
// DESCR:   Enables the windows.
//
////////////////////////////////////////////////////////////////

void  ObjectPaletteWindow::Enable(
  BOOL    fEnable
  )
  {
  EnableWindow(m_hWnd, fEnable);

  m_pTypeList->Enable(fEnable);
  m_pObjList->Enable(fEnable);
  }

/* **************************************************************** */
/* External functions                                               */
/* **************************************************************** */

////////////////////////////////////////////////////////////////
//
// FUNCTION:  RegisterObjectPalette
//
// DESCR:     Registers the object list window class.
//
////////////////////////////////////////////////////////////////

BOOL    RegisterObjectPalette(
  void
  )
  {
  return (SItemListWindow::Init(g_hInstance) &&
          ObjectPaletteWindow::Init(g_hInstance));
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:  InitObjectListPalette
//
// DESCR:     Creates and positions the object list palette
//
////////////////////////////////////////////////////////////////

HWND InitObjectListPalette
  (
  HWND    hwndOwner,
  Layout* pLayout,
  BOOL    fPrstPos
  )
  
  {
  char    szBuffer[80];
  char    szDefault[80];
  POINT   pt;
  BOOL    fShow;
  int     width;
  int     height;
  int     typesHeight;
  int     objsHeight;
  double  x;
  double  y;
  HWND    hwndRet;

  // create the object list palette
  x = 100.;
  y = 100.;

  sprintf(szDefault, "%lf, %lf, %hi", x, y, 1);

  if (fPrstPos)
    {
    GetPrivateProfileString("Layout Palettes", "Object List", szDefault, szBuffer, 80, c_szAppBuilderIni);
    width = GetPrivateProfileInt("Layout Palettes", "Object List Width", OBJLIST_WIDTH, c_szAppBuilderIni);
    height = GetPrivateProfileInt("Layout Palettes", "Object List Height", OBJLIST_HEIGHT, c_szAppBuilderIni);
    typesHeight = GetPrivateProfileInt("Layout Palettes", "Object Types Height", 50, c_szAppBuilderIni);
    objsHeight = GetPrivateProfileInt("Layout Palettes", "Object Objects Height", 50, c_szAppBuilderIni);
    }
  else
    {
    strcpy(szBuffer, szDefault);
    width = OBJLIST_WIDTH;
    height = OBJLIST_HEIGHT;
    typesHeight = height / 2;
    objsHeight = height / 2;
    }

  if (width > GetSystemMetrics(SM_CXSCREEN))
    width = GetSystemMetrics(SM_CXSCREEN);

  if (height > GetSystemMetrics(SM_CYSCREEN))
    height = GetSystemMetrics(SM_CYSCREEN);

  GetPalettePositions(szBuffer, width, height, &pt, &fShow);

  // We can operate on only one instance of the window.
  if (s_pObjPalette != NULL)
    DestroyObjectListWindow(FALSE);

  s_pObjPalette = new ObjectPaletteWindow(pLayout, hwndOwner);

  if ((s_pObjPalette != NULL) && s_pObjPalette->Initialized())
    {
    hwndRet = s_pObjPalette->Hwnd();
    s_pObjPalette->SetPosition(pt, width, height, typesHeight, objsHeight);
    ShowWindow(hwndRet, fShow ? SW_SHOWNA : SW_HIDE);
    }
  else
    {
    delete s_pObjPalette;

    s_pObjPalette = NULL;
    hwndRet       = NULL;
    }

  return hwndRet;
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:  DestroyObjectListWindow
//
// DESCR:     Destroys the object list window.
//
////////////////////////////////////////////////////////////////

void    DestroyObjectListWindow
  (
  BOOL  fPrstPos
  )
  {
  if (s_pObjPalette != NULL)
    {
    if (fPrstPos)
      {
      BOOL    nVisible = IsWindowVisible(s_pObjPalette->Hwnd()) ? 1 : 0;
      RECT    rc;
      double  x;
      double  y;
      char    szBuffer[80];
      int     width;
      int     height;

      GetWindowRect(s_pObjPalette->Hwnd(), &rc);

      x = 100. * (double)rc.left / (double)GetSystemMetrics(SM_CXSCREEN);
      y = 100. * (double)rc.top  / (double)GetSystemMetrics(SM_CYSCREEN);
      sprintf(szBuffer, "%.3lf, %.3lf, %hi", x, y, nVisible);
      WritePrivateProfileString("Layout Palettes", "Object List", szBuffer, c_szAppBuilderIni);

      width  = rc.right  - rc.left;
      sprintf(szBuffer, "%i", width);
      WritePrivateProfileString("Layout Palettes", "Object List Width", szBuffer, c_szAppBuilderIni);

      height = rc.bottom - rc.top;
      sprintf(szBuffer, "%i", height);
      WritePrivateProfileString("Layout Palettes", "Object List Height", szBuffer, c_szAppBuilderIni);

      sprintf(szBuffer, "%i", s_pObjPalette->TypeListHeight());
      WritePrivateProfileString("Layout Palettes", "Object Types Height", szBuffer, c_szAppBuilderIni);

      sprintf(szBuffer, "%i", s_pObjPalette->TypeListHeight());
      WritePrivateProfileString("Layout Palettes", "Object Objects Height", szBuffer, c_szAppBuilderIni);
      }

    delete s_pObjPalette;
    s_pObjPalette = NULL;
    }
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:  SetObjectListDragTarget
//
// DESCR:     Sets the drag target for the object list window.
//
////////////////////////////////////////////////////////////////

void    SetObjectListDragTarget(
  HWND    hDragTarget
  )
  {
  ADBG_ASSERT_E(hDragTarget != NULL, return, "Invalid hDragTarget in SetObjectListDragTarget");

  if (s_pObjPalette)
    s_pObjPalette->AddDragTarget(hDragTarget, 0);
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:  SetObjectListDragIgnore
//
// DESCR:     Tells the object list to ignore the window and use its parent
//            for the drag/drop stuff.
//
////////////////////////////////////////////////////////////////

void    SetObjectListDragIgnore(
  HWND    hDragIgnore
  )
  {
  ADBG_ASSERT_E(hDragIgnore != NULL, return, "Invalid hDragIgnore in SetObjectListDragIgnore");

  if (s_pObjPalette)
    s_pObjPalette->AddDragTarget(hDragIgnore, SItemListWindow::useParent);
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:  RemoveObjectListDragIgnore
//
// DESCR:     Removes the window from the ignore list
//
////////////////////////////////////////////////////////////////

void    RemoveObjectListDragIgnore(
  HWND    hDragIgnore
  )
  {
  ADBG_ASSERT_E(hDragIgnore != NULL, return, "Invalid hDragIgnore in RemoveObjectListDragIgnore");

  if (s_pObjPalette)
    s_pObjPalette->RemoveDragTarget(hDragIgnore);
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:  EnableObjectListWindow
//
// DESCR:     Enables or disables the object list window.  If the window
//            is disabled, then all attempts to activate it with the
//            mouse will cause a message beep.
//
////////////////////////////////////////////////////////////////

void    EnableObjectListWindow(
  BOOL    fEnable
  )
  {
  if (s_pObjPalette)
    s_pObjPalette->Enable(fEnable);
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:  ReloadObjectListWindow
//
// DESCR:     Reloads the objects in the object list part of the
//            objectlistwindow.  This should be called when an object
//            is deleted or renamed.
//
////////////////////////////////////////////////////////////////

void    ReloadObjectListWindow(
  void
  )
  {
  if (s_pObjPalette)
    s_pObjPalette->FillObjectList();
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:  CreateNewVI
//
// DESCR:     Creates a new visual instance for the object or type.  If
//            a type is specified, then a new object of that type is
//            created.
//
////////////////////////////////////////////////////////////////

void  CreateNewVI(
  Layout*       pLayout,
  TYPEID        tNewType,         // non-0 to create one
  ApQueryInfo*  pqi,              // query info (filled in for type create)
  POINT         ptInsert          // point to insert at
  )
  {
  BOOL          bCreated    = FALSE;
  RECT          rc          = {0, 0, 0, 0};
  OpWndItemD*   wi;
#ifdef ADBG_DEBUG
  long          rVal;
#endif
  Window*       pWnd = pLayout->ActiveWindow();
  HCURSOR       hcur;
  LONG          fAdjust = OWND_ADJRECT_ANCHOR_L  | OWND_ADJRECT_ANCHOR_T |
                          OWND_ADJRECT_DEFAULT_W | OWND_ADJRECT_DEFAULT_H;

  if (tNewType != 0)
    {
    // verify that the object has not been created
    I_ASSERT(pqi->object == 0);

    hcur = SetCursor(LoadCursor(NULL, IDC_WAIT));
    pqi->object = AObjCreateWithProtocol(tNewType, APROT_WND);

#ifdef ADBG_DEBUG
    rVal = 
#endif
    AObjQueryProtocol(pqi->object, pqi, APROT_WND);
    ADBG_ASSERT_RES_E((rVal & AP_SUPPORTED), return, g_hInstance, ERROR_NOPROTSUPPORT);

    bCreated = TRUE;
    SetCursor(hcur);
    }

  if (pqi->object)
    {
    hcur = SetCursor(LoadCursor(NULL, IDC_WAIT));
    wi = OWndLayCreateNewItem(pWnd->ObjectID(), pqi->object, (PWICreateFN)pqi->pfnCreate, &rc, NULL, NULL);

    if (wi != NULL)
      {
#ifdef ADBG_DEBUG
      {
      // Try to test the vtable for validity
      WORD    wSel    = SELECTOROF(*((void**) wi));
      DWORD   dwSize  = GetSelectorLimit(wSel);
      WORD    wOff    = OFFSETOF(*((void**) wi));

      if (dwSize == 0 || dwSize < wOff)
        {
        ADbgPrint(ADBG_ERROR, 0, "Bad virtual table in Window Item");
        return;
        }
      }
#endif
      OWndDPtoLP(pWnd->ObjectID(), &ptInsert, 1);
      pWnd->SnapPointToGrid(&ptInsert);

      rc.left   = ptInsert.x;
      rc.top    = ptInsert.y;
      rc.right  = ptInsert.x + ITEM_DEFAULT_WIDTH;
      rc.bottom = ptInsert.y + ITEM_DEFAULT_HEIGHT;

      (pWnd->Items())->AddNewItems(&wi, &pqi->object, &rc, &bCreated, &fAdjust, 1);
      }

    SetCursor(hcur);
    }
  }

////////////////////////////////////////////////////////////////
//
// FUNCTION:  CheckDropPoint
//
// DESCR:     Makes sure that the given drop point is visible.
//
////////////////////////////////////////////////////////////////

void CheckDropPoint
  (
  POINT*  ptDrop,
  POINT*  ptlOff,
  RECT*   rcWnd
  )

  {
  if(ptDrop->y < 0)
    {
    ptDrop->y = 0;
    ptlOff->y = -ptlOff->y;
    }

  if(ptDrop->y >= rcWnd->bottom)
    {
    ptDrop->y -= ptlOff->y;
    ptlOff->y = -ptlOff->y;
    }

  if(ptDrop->x < 0)
    {
    ptDrop->x = 0;
    ptlOff->x = -ptlOff->x;
    }

  if(ptDrop->x >= rcWnd->right)
    {
    ptDrop->x -= ptlOff->x;
    ptlOff->x = -ptlOff->x;
    }
  }
