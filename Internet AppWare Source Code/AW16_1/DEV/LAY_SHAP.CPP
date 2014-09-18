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
// FILE:          "lay_shap.cpp"
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>
#include <op_wnd.h>
#include <o_wnd.h>
#include <o_wndlay.h>
#include <o_wndshp.h>

#include "wlay.h"
#include "lay_shap.h"
#include "lay_wnd.h"
#include "layout.h"
#include "rc\wlay_rsc.h"

// externs /////////////////////////////////////////////////////////////////

extern HINSTANCE    g_hInstance;

extern HBITMAP      g_hbmpArrow;
extern HBITMAP      g_hbmpReorder;
extern HBITMAP      g_hbmpTab;
extern HBITMAP      g_hbmpGroup;
extern HBITMAP      g_hbmpLock;
extern HBITMAP      g_hbmpVisibility;
extern HBITMAP      g_hbmpAnchor;
extern HBITMAP      g_hbmpSelectAll;
extern HBITMAP      g_hbmpWindowBG;

extern HCURSOR      g_hcurNmbr12;
extern HCURSOR      g_hcurNmbr21;
extern HCURSOR      g_hcurTab;
extern HCURSOR      g_hcurGroup;
extern HCURSOR      g_hcurLock;
extern HCURSOR      g_hcurVisibility;
extern HCURSOR      g_hcurAnchor;

extern const char*  c_szAppBuilderIni;
extern const char*  c_szAppWareIni;

// LayoutShapeAdmin ////////////////////////////////////////////////////////

LONG ALMAPI LayoutShapeAdmin
  (
  UINT          uMsg,
  AObjMessage*  pMsg
  )

  {
  long              lRet = A_OK;
  Shapes*           shapes = (Shapes*)pMsg->message1;
  OShpStdToolInfo*  pToolInfo;

  switch(uMsg)
    {
    case OSHP_MSG_SETACTIVETOOL:
      shapes->ChangeTool((TYPEID)pMsg->message2, (short)pMsg->message3);
      break;

    case OSHP_MSG_CREATESHAPE:
      break;

    case OSHP_MSG_GETSTDTOOLINFO:
      pToolInfo = (OShpStdToolInfo*)pMsg->message3;
      switch((short)pMsg->message2)
        {
        case OSHP_STDTOOL_ARROW:
          pToolInfo->hImage = g_hbmpArrow;
          pToolInfo->lStyle = OSHP_STDCAP_RADIO;
          break;

        case OSHP_STDTOOL_REORDER:
          pToolInfo->hImage = g_hbmpReorder;
          pToolInfo->lStyle = OSHP_STDCAP_RADIO;
          break;

        case OSHP_STDTOOL_TABSTOP:
          pToolInfo->hImage = g_hbmpTab;
          pToolInfo->lStyle = OSHP_STDCAP_RADIO;
          break;

        case OSHP_STDTOOL_GROUPSTOP:
          pToolInfo->hImage = g_hbmpGroup;
          pToolInfo->lStyle = OSHP_STDCAP_RADIO;
          break;

        case OSHP_STDTOOL_LOCK:
          pToolInfo->hImage = g_hbmpLock;
          pToolInfo->lStyle = OSHP_STDCAP_RADIO;
          break;

        case OSHP_STDTOOL_VISIBILITY:
          pToolInfo->hImage = g_hbmpVisibility;
          pToolInfo->lStyle = OSHP_STDCAP_RADIO;
          break;

        case OSHP_STDTOOL_ANCHOR:
          pToolInfo->hImage = g_hbmpAnchor;
          pToolInfo->lStyle = OSHP_STDCAP_RADIO;
          break;

        case OSHP_STDTOOL_SELECTALL:
          pToolInfo->hImage = g_hbmpSelectAll;
          pToolInfo->lStyle = OSHP_STDCAP_PUSH;
          break;

        case OSHP_STDTOOL_BACKGROUND:
          pToolInfo->hImage = g_hbmpWindowBG;
          pToolInfo->lStyle = OSHP_STDCAP_PUSH;
          break;

        default:
          lRet = A_ERROR;
          #ifdef ADBG_DEBUG
          ADbgPrint(ADBG_ERROR, 0, "Shape Library requesting information on an invalid standard tool (id = %hi).", (short)pMsg->message2);
          #endif
          break;
        }
      break;

    case OSHP_MSG_GETCURRENTUNITS:
      pMsg->message2 = shapes->GetCurrentUnits();
      break;

    case OSHP_MSG_ENABLEPALETTES:
      (shapes->m_layout)->EnablePalettes((BOOL)pMsg->message2);
      break;

    default:
      lRet = A_NOTHANDLED;
      break;
    }

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes constructor
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

Shapes::Shapes
  (
  Layout* layout
  )

  {
  m_layout = layout;

  // initialize the shape libraries
  m_nShapeLib = 0;
  m_miShapeLib = AMemAllocate(0L);
  m_pShapeLib = NULL;

  // set the default tool
  m_nTool = OSHP_STDTOOL_ARROW;
  m_fToolProp = 0;
  m_nToolLib = -1;
  m_hcursor = NULL;

  // make sure nothing is tracking
  m_fTrack = TRACK_NONE;
  m_wiShape = NULL;
  SetRectEmpty(&m_rcLast);
  SetPointEmpty(&m_ptStart);

  LoadShapeLibs();
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes denstructor
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////


Shapes::~Shapes
  (
  )

  {
  FreeShapeLibs();

  if(m_nShapeLib)
    AMemUnlock(m_miShapeLib);

  AMemFree(m_miShapeLib);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::FindShapeLibIndex (private)
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

int Shapes::FindShapeLibIndex
  (
  TYPEID  tiShape
  )

  {
  int rVal = -1;
  int i;

  if(tiShape)
    {
    for(i=0; i<m_nShapeLib; i++)
      {
      if(m_pShapeLib[i].type==tiShape)
        {
        rVal = i;
        break;
        }
      }
    }

  return rVal;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::BroadcastToolChange (private)
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::BroadcastToolChange
  (
  )

  {
  int         i;
  AObjMessage msg = 
    {
    0L,
    (long)((m_nToolLib>=0) ? m_pShapeLib[m_nToolLib].type : NULL),
    (long)m_nTool,
    OSHP_TS_ACTIVE,
    ""
    };

  for(i=0; i<m_nShapeLib; i++)
    {
    msg.message1 = m_pShapeLib[i].hPalette;
    m_pShapeLib[i].lpfnShapeAdmin(OSHP_SETTOOLSTATE, &msg);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::TrackShape (private)
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::TrackShape
  (
  Window* pWnd,
  HWND    hwnd,
  LPPOINT lppt,
  BOOL    fDraw,
  BOOL    fToggle
  )

  {
  RECT  rc;

  I_ASSERT((m_fTrack & TRACK_SELECT) || ((m_fTrack & TRACK_NEWSHAPE) && m_wiShape));
  I_ASSERT(fToggle || fDraw);

  I_ASSERT(lppt);

  // get the new tracking rect
  SetRect(&rc, m_ptStart.x, m_ptStart.y, (lppt->x>=0) ? lppt->x : 0, (lppt->y>=0) ? lppt->y : 0);
  if(m_fTrack & TRACK_NEWSHAPE)
    pWnd->Items()->AdjustNewRect(m_wiShape, &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
  else
    MakeRealRect(&rc, &rc);

  if((fToggle && !fDraw) || (fDraw && !fToggle) || !EqualRect(&rc, &m_rcLast))
    {
    HDC     hdc           = GetDC(hwnd);
    HPEN    hpen          = CreatePen(PS_DOT, 1, COLOR_WINDOWTEXT);
    HPEN    hpenOld       = (HPEN)SelectObject(hdc, hpen);
    HBRUSH  hbrOld        = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
    int     nOldDrawMode  = SetROP2(hdc, R2_NOTXORPEN);

    // clear the old tracking rect
    if(fToggle)
      {
      if(m_fTrack & TRACK_NEWSHAPE)
        pWnd->Items()->DrawTrackingRect(m_wiShape, hdc, &m_rcLast);
      else
        Rectangle(hdc, m_rcLast.left, m_rcLast.top, m_rcLast.right, m_rcLast.bottom);

      if(!fDraw)
        CopyRect(&m_rcLast, &rc);
      }

    // draw the new tracking rect
    if(fDraw)
      {
      if(m_fTrack & TRACK_NEWSHAPE)
        pWnd->Items()->DrawTrackingRect(m_wiShape, hdc, &rc);
      else
        Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

      CopyRect(&m_rcLast, &rc);
      }

    SelectObject(hdc, hpenOld);
    DeleteObject(hpen);
    SelectObject(hdc, hbrOld);
    SetROP2(hdc, nOldDrawMode);
    ReleaseDC(hwnd, hdc);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::CreateShape (private)
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

OpWndItemD* Shapes::CreateShape
  (
  Window* pWnd,
  LPPOINT lppt,
  BOOL    fLButtonDown
  )

  {
  OpWndItemD* wi = NULL;
  RECT        rc = {m_ptStart.x, m_ptStart.y, (lppt->x>0) ? lppt->x : 0, (lppt->y>0) ? lppt->y : 0};

  I_ASSERT(m_nToolLib>=0);

  // create the shape
  wi = OWndLayCreateNewItem(
          pWnd->ObjectID(),
          (OBJECTID)m_pShapeLib[m_nToolLib].type,
          m_pShapeLib[m_nToolLib].lpfnCreate,
          &rc,
          NULL,
          m_pShapeLib[m_nToolLib].szDLL);

  if(wi)
    {
    // make sure that the shape was created properly
    #ifdef ADBG_DEBUG
    I_ASSERT(wi);

    if(!wi)
      {
      ADbgPrint(ADBG_ERROR, 0, "Shape Lib: \"%s\"\tShape could not be created.", m_pShapeLib[m_nToolLib].szDLL);
      return NULL;
      }  

    #if !defined(__FLAT__)
    // Try to test the vtable for validity
    WORD  wSel   = SELECTOROF(*((void**) wi));
    DWORD dwSize = GetSelectorLimit(wSel);
    WORD  wOff   = OFFSETOF(*((void**) wi));

    if (dwSize == 0 || dwSize < wOff)
      {
      ADbgPrint(ADBG_ERROR, 0, "Shape Lib: \"%s\"\tBad virtual table in shape for current tool.", m_pShapeLib[m_nToolLib].szDLL);
      return NULL;
      }
    #endif

    if(!OWndLayIsItemShape(wi))
      {
      ADbgPrint(ADBG_ERROR, 0, "Shape Lib: \"%s\"\tDid not set the item as a shape.", m_pShapeLib[m_nToolLib].szDLL);
      OWndLaySetItemIsShape(wi, TRUE);
      }
    #endif

    pWnd->Items()->AdjustNewRect(wi, &rc, OWND_ADJRECT_ANCHOR_L | OWND_ADJRECT_ANCHOR_T);
    CopyRect(&wi->m_rcItem, &rc);

    // finish doing what is needed for creating a shape
    if(fLButtonDown && (m_fToolProp & OSHP_CAP_RESIZEONCREATE))
      {
      m_wiShape = wi;
      m_fTrack |= TRACK_NEWSHAPE;
      SetCapture(pWnd->ClientWnd());
      TrackShape(pWnd, pWnd->ClientWnd(), lppt, TRUE, FALSE);
      }
    else
      (pWnd->Items())->AddNewItems(&wi, NULL, NULL, NULL, NULL, 1);

    if(wi && (m_fToolProp & OSHP_CAP_NOTIFYONCREATE))
      {
      AObjMessage msg = {m_pShapeLib[m_nToolLib].hPalette, (long)wi, 0L, 0L, ""};

      m_pShapeLib[m_nToolLib].lpfnShapeAdmin(OSHP_SHAPECREATED, &msg);
      }
    }

  return wi;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::LoadShapeLibs
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::LoadShapeLibs
  (
  )

  {
  char  szDLLs[255];
  char* szLastLib;
  UINT  nOldErrorMode;

  // turn off the system error box when the shape libraries are trying to be loaded
  nOldErrorMode = SetErrorMode(SEM_NOOPENFILEERRORBOX);

  GetPrivateProfileString("Window Object", "Shape Libraries", "AW32SHAP.DLL", szDLLs, 255, c_szAppWareIni);

  szLastLib = strtok(szDLLs, ",; ");

  while(szLastLib)
    {
    #ifdef AW_I_DEBUG
    char  szTmp[255];

    wsprintf(szTmp, "shape library name = %s", szLastLib);
    ADBG_PRINT_I(szTmp);
    #endif

    LoadShapeLib(szLastLib);
    szLastLib = strtok(NULL, ",; ");
    }

  SetErrorMode(nOldErrorMode);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::LoadShapeLib
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::LoadShapeLib
  (
  char* szDLL
  )

  {
  AObjMessage msg = {0L, 0L, 0L, 0L, ""};
  HINSTANCE   hinstLib;

  hinstLib = LoadLibrary(szDLL);

  #if defined(__FLAT__)
  if(!hinstLib)
  #else
  if(hinstLib < HINSTANCE_ERROR)
  #endif
    {
    ADBG_EVAL(ADbgPrint(ADBG_ERROR, 0, "Unable to load shape library \"%s.\"", szDLL);)

    return;
    }

  if(m_pShapeLib || (m_nShapeLib>0))
    AMemUnlock(m_miShapeLib);

  m_nShapeLib++;
  AMemSetSize(m_miShapeLib, m_nShapeLib*sizeof(SHAPELIB));
  m_pShapeLib = (SHAPELIB*)AMemLock(m_miShapeLib);
  I_ASSERT(m_pShapeLib);

  m_pShapeLib[m_nShapeLib-1].hinstShapeLib = hinstLib;
  m_pShapeLib[m_nShapeLib-1].lpfnShapeAdmin = (OShapeAdmin)GetProcAddress(hinstLib, "ShapeLibAdmin");
  lstrcpyn(m_pShapeLib[m_nShapeLib-1].szDLL, szDLL, 13);
  m_pShapeLib[m_nShapeLib-1].szDLL[12] = 0;

  if(!m_pShapeLib[m_nShapeLib-1].lpfnShapeAdmin)
    {
    ADBG_EVAL(ADbgPrint(ADBG_ERROR, 0, "Unable to find the entry point \"ShapeLibAdmin\" in shape library \"%s.\"", szDLL);)

    m_nShapeLib--;
    AMemUnlock(m_miShapeLib);
    AMemSetSize(m_miShapeLib, m_nShapeLib*sizeof(SHAPELIB));
    m_pShapeLib = (m_nShapeLib==0) ? NULL : (SHAPELIB*)AMemLock(m_miShapeLib);

    FreeLibrary(hinstLib);
    }
  else
    {
    msg.message1 = (long)this;
    msg.message2 = (long)LayoutShapeAdmin;
    m_pShapeLib[m_nShapeLib-1].lpfnShapeAdmin(OSHP_SETLAYOUTINFO, &msg);

    m_pShapeLib[m_nShapeLib-1].lpfnShapeAdmin(OSHP_QUERYLIBINFO, &msg);
    m_pShapeLib[m_nShapeLib-1].lpfnCreate = (PWICreateFN)msg.message1;
    m_pShapeLib[m_nShapeLib-1].type = (TYPEID)msg.message2;
    m_pShapeLib[m_nShapeLib-1].lCaps = msg.message3;

    msg.message1 = (long)m_pShapeLib[m_nShapeLib-1].szName;
    msg.message2 = (long)LEN_SHAPELIBNAME;
    m_pShapeLib[m_nShapeLib-1].lpfnShapeAdmin(OSHP_MENUSTRING, &msg);
    m_pShapeLib[m_nShapeLib-1].hPalette = NULL;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::ValidateShapeLib
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::ValidateShapeLib
  (
  TYPEID  tiShape
  )

  {
  I_UNUSED(tiShape);

//  if(FindShapeLibIndex(tiShape)<0)
//    {
//    char  szDLL[13];
//    OWndLayGetShapeLibName(m_pItems[i].wi->Object(), szDLL, 13)
//    LoadShapeLib(szDLL);
//    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::FreeShapeLibs
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::FreeShapeLibs
  (
  )

  {
  int i;

  for(i=0; i<m_nShapeLib; i++)
    FreeLibrary(m_pShapeLib[i].hinstShapeLib);

  if(m_nShapeLib>0)
    AMemUnlock(m_miShapeLib);

  m_nShapeLib = 0;
  AMemSetSize(m_miShapeLib, 0L);
  m_pShapeLib = NULL;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::GetCurrentUnits
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

long Shapes::GetCurrentUnits
  (
  )

  {
  long  lRet = -1;

  if(m_layout->CurrUnitsInches())
    lRet = OSHP_UNITS_ENGLISH_IN;
  else if(m_layout->CurrUnitsMillimeters())
    lRet = OSHP_UINTS_METRIC_MM;
  else if(m_layout->CurrUnitsPoints())
    lRet = OSHP_UNITS_POINTS_PT;
  #ifdef AW_I_DEBUG
  else
    ADbgInternalPrint("Invalid units in the layout.", __FILE__, __LINE__);
  #endif

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::ChangeTool
//
//  DESCRIPTION:  This will change the current tool.
//
////////////////////////////////////////////////////////////////////////////

void Shapes::ChangeTool
  (
  TYPEID  tiShape,
  short   nTool
  )

  {
  if((tiShape!=CurrentLib()) || (nTool!=m_nTool))
    {
    BOOL  fOldFullInfo = FullInfoTool();

    switch(nTool)
      {
      case OSHP_STDTOOL_ANCHOR:
        I_ASSERT(tiShape==NULL);
        m_nTool = nTool;
        m_nToolLib = -1;
        m_layout->SwitchToFrameMode(FullInfoTool(), fOldFullInfo);
        m_hcursor = g_hcurAnchor;
        m_fToolProp = 0L;
        break;

      case OSHP_STDTOOL_ARROW:
        I_ASSERT(tiShape==NULL);
        m_nTool = nTool;
        m_nToolLib = -1;
        m_layout->SwitchToFrameMode(FullInfoTool(), fOldFullInfo);
        m_fToolProp = 0L;
        m_hcursor = NULL;
        break;

      case OSHP_STDTOOL_BACKGROUND:
        I_ASSERT(tiShape==NULL);
        m_layout->ActiveWindow()->EditBackground();
        break;

      case OSHP_STDTOOL_GROUPSTOP:
        I_ASSERT(tiShape==NULL);
        m_nTool = nTool;
        m_nToolLib = -1;
        m_layout->SwitchToFrameMode(FullInfoTool(), fOldFullInfo);
        m_hcursor = g_hcurGroup;
        m_fToolProp = 0L;
        break;

      case OSHP_STDTOOL_LOCK:
        I_ASSERT(tiShape==NULL);
        m_nTool = nTool;
        m_nToolLib = -1;
        m_layout->SwitchToFrameMode(FullInfoTool(), fOldFullInfo);
        m_hcursor = g_hcurLock;
        m_fToolProp = 0L;
        break;

      case OSHP_STDTOOL_REORDER:
        I_ASSERT(tiShape==NULL);
        if(m_layout->ActiveWindow()
              &&
           m_layout->ActiveWindow()->Items()->StartReorder())
          {
          m_nTool = nTool;
          m_nToolLib = -1;
          m_layout->SwitchToFrameMode(FullInfoTool(), fOldFullInfo);
          m_hcursor = g_hcurNmbr12;
          m_fToolProp = 0L;
          }
        else  // there must be at least two items to reorder....
          {
          m_nTool = OSHP_STDTOOL_ARROW;
          m_nToolLib = -1;
          m_layout->SwitchToFrameMode(FullInfoTool(), fOldFullInfo);
          m_fToolProp = 0L;
          m_hcursor = NULL;
          }
        break;

      case OSHP_STDTOOL_SELECTALL:
        I_ASSERT(tiShape==NULL);
        m_layout->ActiveWindow()->Items()->SelectAll(TRUE);
        break;

      case OSHP_STDTOOL_TABSTOP:
        I_ASSERT(tiShape==NULL);
        m_nTool = nTool;
        m_nToolLib = -1;
        m_layout->SwitchToFrameMode(FullInfoTool(), fOldFullInfo);
        m_hcursor = g_hcurTab;
        m_fToolProp = 0L;
        break;

      case OSHP_STDTOOL_VISIBILITY:
        I_ASSERT(tiShape==NULL);
        m_nTool = nTool;
        m_nToolLib = -1;
        m_layout->SwitchToFrameMode(FullInfoTool(), fOldFullInfo);
        m_hcursor = g_hcurVisibility;
        m_fToolProp = 0L;
        break;

      default:
        m_nTool = nTool;
        m_nToolLib = FindShapeLibIndex(tiShape);

        I_ASSERT((nTool>=OSHP_STDTOOL_USERTOOL) && tiShape);
        m_layout->SwitchToFrameMode(FullInfoTool(), fOldFullInfo);

        if(m_nToolLib>=0)
          {
          AObjMessage msg = {nTool, 0L, 0L, 0L, ""};
          #ifdef ADBG_DEBUG
          long lRet =
          #endif
          m_pShapeLib[m_nToolLib].lpfnShapeAdmin(OSHP_QUERYTOOLINFO, &msg);
          m_fToolProp = msg.message2;
          m_hcursor = (HCURSOR)msg.message3;

          #ifdef ADBG_DEBUG
          if(lRet != A_OK)
            ADbgPrint(ADBG_ERROR, 0, "Shape Lib: \"%s\"\tTool ID: %hi\tInvalid tool information.", m_pShapeLib[m_nToolLib].szDLL, m_nTool);
          if(!m_hcursor)
            ADbgPrint(ADBG_WARNING, 0, "Shape Lib: \"%s\"\tTool ID: %hi\tInvalid cursor supplied.", m_pShapeLib[m_nToolLib].szDLL, m_nTool);
          #endif
          }
        else
          {
          m_nTool = OSHP_STDTOOL_ARROW;
          m_nToolLib = -1;
          m_fToolProp = 0L;
          m_hcursor = NULL;
          }
        break;
      }

    BroadcastToolChange();
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::InitPalettes
//
//  DESCRIPTION:  This will cause all shape libraries with palettes to
//                create and display them.
//
////////////////////////////////////////////////////////////////////////////

void Shapes::InitPalettes
  (
  )

  {
  AObjMessage msg = {0L, 0L, 0L, 0L, ""};
  long        rVal;
  int         i;
  HMENU       hmenu = GetSubMenu(GetMenu(m_layout->WndHandle()), MENU_PALETTES);
  char        szBuffer[80];
  char        szDefault[80];
  char        szType[20];
  char*       szEntry;
  RECT        rcPal;
  double      x, y;
  BOOL        fShow;

  for(i=0; i<m_nShapeLib; i++)
    {
    if(m_pShapeLib[i].lCaps & OSHP_LIBCAP_HASPALETTE)
      {
      msg.message2 = (long)m_layout->WndHandle();
      rVal = m_pShapeLib[i].lpfnShapeAdmin(OSHP_CREATEPALETTE, &msg);
      if(rVal==A_OK)
        {
        // position and show the palette based on .ini defaults
        wsprintf(szType, "%lx", m_pShapeLib[i].type);
        sprintf(szDefault, "%.3lf, %.3lf, %hi", (double)(100.0 - 5.*i), (double)(5.*i), (short)1);
        if(m_layout->PrstWndPositions())
          GetPrivateProfileString("Layout Shape Libraries", szType, szDefault, szBuffer, 80, c_szAppBuilderIni);
        else
          lstrcpy(szBuffer, szDefault);

        x = strtod(szBuffer, &szEntry);
        y = strtod(++szEntry, &szEntry);
        fShow   = (strtol(++szEntry, &szEntry, 0)!=0) ? TRUE : FALSE;

        // adjust the palette position to make sure it is on screen
        rcPal.left = (int)(x * GetSystemMetrics(SM_CXSCREEN)/100.);
        if((rcPal.left+(int)msg.message3) > GetSystemMetrics(SM_CXSCREEN))
          rcPal.left = GetSystemMetrics(SM_CXSCREEN) - (int)msg.message3;
        if(rcPal.left<0)
          rcPal.left = 0;

        rcPal.top = (int)(y * GetSystemMetrics(SM_CYSCREEN)/100.);
        if((rcPal.top+(int)msg.message4) > GetSystemMetrics(SM_CYSCREEN))
          rcPal.top = GetSystemMetrics(SM_CYSCREEN) - (int)msg.message4;
        if(rcPal.top<0)
          rcPal.top = 0;

        // save the palette handle
        m_pShapeLib[i].hPalette = msg.message1;

        // set the palette's position
        msg.message2 = (long)&rcPal;
        m_pShapeLib[i].lpfnShapeAdmin(OSHP_SETPALETTEPOS, &msg);

        if(fShow)
          { // show the palette if desired
          msg.message1 = m_pShapeLib[i].hPalette;
          msg.message2 = (long)FALSE;   // show but don't activate
          m_pShapeLib[i].lpfnShapeAdmin(OSHP_SHOWPALETTE, &msg);
          }

        // add the palette name to the palettes menu....
        AppendMenu(hmenu, MF_STRING | MF_ENABLED | MF_UNCHECKED, IDM_USERPALETTE+i, m_pShapeLib[i].szName);
        }
      #ifdef ADBG_DEBUG
      else
        ADbgPrint(ADBG_ERROR, 0, "The shape library \"%s\" could not create its palette.", m_pShapeLib[i].szName);
      #endif
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::EnablePalettes
//
//  DESCRIPTION:  This will cause all shape libraries with palettes to
//                enable/disable them (for modal dialogs).
//
////////////////////////////////////////////////////////////////////////////

void Shapes::EnablePalettes
  (
  BOOL  fEnable
  )

  {
  AObjMessage msg = {0L, (long)fEnable, 0L, 0L, ""};
  int         i;

  for(i=0; i<m_nShapeLib; i++)
    {
    if((m_pShapeLib[i].lCaps & OSHP_LIBCAP_HASPALETTE) && m_pShapeLib[i].hPalette)
      {
      msg.message1 = m_pShapeLib[i].hPalette;
      m_pShapeLib[i].lpfnShapeAdmin(OSHP_ENABLEPALETTE, &msg);
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::DestroyPalettes
//
//  DESCRIPTION:  This will cause all shape libraries with palettes to
//                destroy their palettes.
//
////////////////////////////////////////////////////////////////////////////

void Shapes::DestroyPalettes
  (
  BOOL fPrstPos
  )

  {
  AObjMessage msg = {0L, 0L, 0L, 0L, ""};
  int         i;
  RECT        rcPal;
  long        lRet;
  short       nVisible;
  double      x, y;
  char        szType[20];
  char        szBuffer[80];

  for(i=0; i<m_nShapeLib; i++)
    {
    if(fPrstPos)
      {
      if((m_pShapeLib[i].lCaps & OSHP_LIBCAP_HASPALETTE) && m_pShapeLib[i].hPalette)
        {
        // get the current palette position
        msg.message1 = m_pShapeLib[i].hPalette;
        msg.message2 = (long)&rcPal;
        lRet = m_pShapeLib[i].lpfnShapeAdmin(OSHP_GETPALETTEPOS, &msg);
        if(lRet==A_OK)
          {
          // get the current palette status (visibility)
          msg.message1 = m_pShapeLib[i].hPalette;
          msg.message2 = 0L;
          lRet = m_pShapeLib[i].lpfnShapeAdmin(OSHP_GETPALETTESTATUS, &msg);
          nVisible = (short)(((lRet==A_OK) && !(msg.message2 & OSHP_PS_VISIBLE)) ? 0 : 1);

          // save the position and status in the appbldr.ini file
          x = 100. * (double)rcPal.left / (double)GetSystemMetrics(SM_CXSCREEN);
          y = 100. * (double)rcPal.top / (double)GetSystemMetrics(SM_CYSCREEN);
          wsprintf(szType, "%lx", m_pShapeLib[i].type);
          sprintf(szBuffer, "%.3lf, %.3lf, %hi", x, y, nVisible);
          WritePrivateProfileString("Layout Shape Libraries", szType, szBuffer, c_szAppBuilderIni);
          }
        }
      }

    if((m_pShapeLib[i].lCaps & OSHP_LIBCAP_HASPALETTE) && m_pShapeLib[i].hPalette)
      {
      msg.message1 = m_pShapeLib[i].hPalette;
      m_pShapeLib[i].lpfnShapeAdmin(OSHP_DESTROYPALETTE, &msg);
      m_pShapeLib[i].hPalette = 0L;
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::ShowAllPalettes
//
//  DESCRIPTION:  This will cause all shape libraries with palettes to
//                make their palettes visible.
//
////////////////////////////////////////////////////////////////////////////

void Shapes::ShowAllPalettes
  (
  BOOL  fShow
  )

  {
  AObjMessage msg = {0L, 0L, 0L, 0L, ""};
  long        rVal;
  BOOL        fVisible;
  int         i;

  for(i=0; i<m_nShapeLib; i++)
    {
    if((m_pShapeLib[i].lCaps & OSHP_LIBCAP_HASPALETTE) && (m_pShapeLib[i].hPalette))
      {
      msg.message1 = m_pShapeLib[i].hPalette;
      msg.message2 = 0L;
      rVal = m_pShapeLib[i].lpfnShapeAdmin(OSHP_GETPALETTESTATUS, &msg);
      if(rVal==A_OK)
        {
        fVisible = (msg.message2 & OSHP_PS_VISIBLE) ? TRUE : FALSE;

        if(fVisible != fShow)
          {
          msg.message1 = (long)m_pShapeLib[i].hPalette;
          msg.message2 = 0L;
          m_pShapeLib[i].lpfnShapeAdmin(fShow ? OSHP_SHOWPALETTE : OSHP_HIDEPALETTE, &msg);
          }
        }
      #ifdef ADBG_DEBUG
      else
        ADbgPrint(ADBG_ERROR, 0, "Shape Lib: \"%s\"\tUnable to get status.", m_pShapeLib[i].szDLL);
      #endif
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::InitPaletteMenu
//
//  DESCRIPTION:  This will cause all shape libraries with palettes to
//                add their string to the Palettes menu.
//
////////////////////////////////////////////////////////////////////////////

void Shapes::InitPaletteMenu
  (
  HMENU hmenuPopup
  )

  {
  int         i;
  AObjMessage msg = {0L, 0L, 0L, 0L, ""};
  long        rVal;

  for(i=0; i<m_nShapeLib; i++)
    {
    if((m_pShapeLib[i].lCaps & OSHP_LIBCAP_HASPALETTE) && m_pShapeLib[i].hPalette)
      {
      msg.message1 = m_pShapeLib[i].hPalette;
      msg.message2 = 0L;
      rVal = m_pShapeLib[i].lpfnShapeAdmin(OSHP_GETPALETTESTATUS, &msg);
      if(rVal==A_OK)
        InitPaletteMenuString((msg.message2 & OSHP_PS_VISIBLE) ? TRUE : FALSE, hmenuPopup, IDM_USERPALETTE+i, m_pShapeLib[i].szName);
      #ifdef ADBG_DEBUG
      else
        ADbgPrint(ADBG_ERROR, 0, "Unable to get status for shape library \"%s.\"", m_pShapeLib[i].szDLL);
      #endif
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::MenuPicked
//
//  DESCRIPTION:  This will cause all shape libraries with palettes to
//                add their string to the Palettes menu.
//
////////////////////////////////////////////////////////////////////////////

void Shapes::MenuPicked
  (
  WPARAM  wParam
  )

  {
  AObjMessage msg = {0L, 0L, 0L, 0L, ""};
  long        rVal;
  BOOL        fShow;
  int         idx = (int)(wParam - IDM_USERPALETTE);

  if(idx>=0 && idx<m_nShapeLib)
    {
    if((m_pShapeLib[idx].lCaps & OSHP_LIBCAP_HASPALETTE) && (m_pShapeLib[idx].hPalette))
      {
      msg.message1 = m_pShapeLib[idx].hPalette;
      msg.message2 = 0L;
      rVal = m_pShapeLib[idx].lpfnShapeAdmin(OSHP_GETPALETTESTATUS, &msg);
      if(rVal==A_OK)
        {
        // toggle the palette's visibility
        fShow = (msg.message2 & OSHP_PS_VISIBLE) ? FALSE : TRUE;
        msg.message1 = (long)m_pShapeLib[idx].hPalette;
        msg.message2 = fShow ? (long)TRUE : 0L;
        m_pShapeLib[idx].lpfnShapeAdmin(fShow ? OSHP_SHOWPALETTE : OSHP_HIDEPALETTE, &msg);
        }
      #ifdef ADBG_DEBUG
      else
        ADbgPrint(ADBG_ERROR, 0, "Unable to get status for shape library \"%s.\"", m_pShapeLib[idx].szDLL);
      #endif
      }
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::FullInfoTool
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

BOOL Shapes::FullInfoTool
  (
  )

  {
  return ((m_nTool==OSHP_STDTOOL_GROUPSTOP)
               ||
          (m_nTool==OSHP_STDTOOL_LOCK)
               ||
          (m_nTool==OSHP_STDTOOL_REORDER)
               ||
          (m_nTool==OSHP_STDTOOL_TABSTOP)
               ||
          (m_nTool==OSHP_STDTOOL_VISIBILITY));
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::GetShapeName
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::GetShapeName
  (
  OpWndItemD* wi,
  LPSTR       szName,
  int         nLen
  )

  {
  int i;

  // for shape items the objectid is actually the typeid
  if((i=FindShapeLibIndex((TYPEID)wi->Object()))>=0)
    {
    AObjMessage msg = {(long)wi, (long)szName, (long)nLen, 0L, ""};

    m_pShapeLib[i].lpfnShapeAdmin(OSHP_GETSHAPENAME, &msg);
    }
  else
    lstrcpyn(szName, "Invalid Shape Library", nLen);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::MouseMove
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::MouseMove
  (
  Window* pWnd,
  HWND    hwnd,
  int    fwKeys,
  short   xPos,
  short   yPos
  )

  {
  POINT pt = {xPos, yPos};
  OWndDPtoLP(pWnd->ObjectID(), &pt, 1);

  switch(m_nTool)
    {
    case OSHP_STDTOOL_ARROW:
      (pWnd->Items())->MouseMove(&pt, &m_hcursor, fwKeys);
      break;

    case OSHP_STDTOOL_REORDER:
      m_hcursor = (fwKeys & MK_SHIFT) ? g_hcurNmbr21 : g_hcurNmbr12;
      break;

    case OSHP_STDTOOL_ANCHOR:
      I_ASSERT(m_hcursor==g_hcurAnchor);
      break;

    case OSHP_STDTOOL_GROUPSTOP:
      I_ASSERT(m_hcursor==g_hcurGroup);
      break;

    case OSHP_STDTOOL_LOCK:
      I_ASSERT(m_hcursor==g_hcurLock);
      break;

    case OSHP_STDTOOL_TABSTOP:
      I_ASSERT(m_hcursor==g_hcurTab);
      break;

    case OSHP_STDTOOL_VISIBILITY:
      I_ASSERT(m_hcursor==g_hcurVisibility);
      break;

    default:
      // a shape library tool
      I_ASSERT(m_nToolLib>=0);
      pWnd->SnapPointToGrid(&pt);
      if((m_fTrack & TRACK_NEWSHAPE) || (m_fTrack & TRACK_SELECT))
        TrackShape(pWnd, hwnd, &pt, TRUE, TRUE);
      break;
    }

  SetCursor(m_hcursor);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::MouseLButtonDown
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::MouseLButtonDown
  (
  Window* pWnd,
  HWND    hwnd,
  int     fwKeys,
  short   xPos,
  short   yPos
  )

  {
  I_UNUSED(hwnd);

  POINT pt = {xPos, yPos};
  OWndDPtoLP(pWnd->ObjectID(), &pt, 1);

  switch(m_nTool)
    {
    case OSHP_STDTOOL_ARROW:
      I_ASSERT(m_nToolLib<0);
      (pWnd->Items())->LButtonDown(&pt, &m_hcursor, fwKeys);
      SetCursor(m_hcursor);
      break;

    case OSHP_STDTOOL_REORDER:
      I_ASSERT(m_nToolLib<0);
      (pWnd->Items())->Reorder(&pt, (fwKeys & MK_SHIFT) ? TRUE : FALSE);
      break;

    case OSHP_STDTOOL_ANCHOR:
      I_ASSERT(m_nToolLib<0);
      (pWnd->Items())->Attach(&pt);
      break;

    case OSHP_STDTOOL_GROUPSTOP:
      I_ASSERT(m_nToolLib<0);
      (pWnd->Items())->GroupStop(&pt);
      break;

    case OSHP_STDTOOL_LOCK:
      I_ASSERT(m_nToolLib<0);
      (pWnd->Items())->Lock(&pt);
      break;

    case OSHP_STDTOOL_TABSTOP:
      I_ASSERT(m_nToolLib<0);
      (pWnd->Items())->TabStop(&pt);
      break;

    case OSHP_STDTOOL_VISIBILITY:
      I_ASSERT(m_nToolLib<0);
      (pWnd->Items())->Visibility(&pt);
      break;

    default:
      // start tracking a new shape
      I_ASSERT(m_nToolLib>=0);
      (pWnd->Items())->SelectAll(FALSE);
      pWnd->SnapPointToGrid(&pt);
      CopyPoint(&m_ptStart, &pt);

      if(m_fToolProp & OSHP_CAP_CREATEONDOWN)
        {
        I_ASSERT(!m_wiShape);
        CreateShape(pWnd, &pt, TRUE);
        I_ASSERT(!(m_fToolProp & OSHP_CAP_RESIZEONCREATE) || m_wiShape);
        }
      else if(m_fToolProp & OSHP_CAP_CREATEONUP)
        {
        SetCapture(hwnd);
        m_fTrack |= TRACK_SELECT;
        TrackShape(pWnd, hwnd, &pt, TRUE, FALSE);
        }

      break;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::MouseLButtonDblClk
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::MouseLButtonDblClk
  (
  Window* pWnd,
  HWND    hwnd,
  int     fwKeys,
  short   xPos,
  short   yPos
  )

  {
  I_UNUSED(hwnd);

  POINT pt = {xPos, yPos};
  OWndDPtoLP(pWnd->ObjectID(), &pt, 1);

  switch(m_nTool)
    {
    case OSHP_STDTOOL_ARROW:
      I_ASSERT(m_nToolLib<0);
      (pWnd->Items())->EditItem(&pt, (fwKeys & MK_CONTROL) ? TRUE : FALSE);
      break;

    case OSHP_STDTOOL_REORDER:
    case OSHP_STDTOOL_TABSTOP:
    case OSHP_STDTOOL_GROUPSTOP:
    case OSHP_STDTOOL_LOCK:
    case OSHP_STDTOOL_VISIBILITY:
      I_ASSERT(m_nToolLib<0);
      MouseLButtonDown(pWnd, hwnd, fwKeys, xPos, yPos);
      break;

    case OSHP_STDTOOL_ANCHOR:
    default:
      break;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::MouseLButtonUp
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::MouseLButtonUp
  (
  Window* pWnd,
  HWND    hwnd,
  int     fwKeys,
  short   xPos,
  short   yPos
  )

  {
  I_UNUSED(hwnd);
  I_UNUSED(fwKeys);

  POINT pt = {xPos, yPos};
  OWndDPtoLP(pWnd->ObjectID(), &pt, 1);

  switch(m_nTool)
    {
    case OSHP_STDTOOL_ARROW:
      I_ASSERT(m_nToolLib<0);
      (pWnd->Items())->LButtonUp(&pt, &m_hcursor, fwKeys);
      SetCursor(m_hcursor);
      break;

    case OSHP_STDTOOL_ANCHOR:
    case OSHP_STDTOOL_GROUPSTOP:
    case OSHP_STDTOOL_LOCK:
    case OSHP_STDTOOL_REORDER:
    case OSHP_STDTOOL_TABSTOP:
    case OSHP_STDTOOL_VISIBILITY:
      I_ASSERT(m_nToolLib<0);
      break;

    default:
      // shape library tool
      I_ASSERT(m_nToolLib>=0);
      pWnd->SnapPointToGrid(&pt);

      if(m_fToolProp & OSHP_CAP_CREATEONDOWN)
        {
        if((m_fToolProp & OSHP_CAP_RESIZEONCREATE) && m_wiShape)
          {
          TrackShape(pWnd, pWnd->ClientWnd(), &pt, FALSE, TRUE);
          (pWnd->Items())->AddNewItems(&m_wiShape, NULL, &m_rcLast, NULL, NULL, 1);
          m_fTrack = 0;
          ReleaseCapture();
          m_wiShape = NULL;
          }

        SetPointEmpty(&m_ptStart);
        SetRectEmpty(&m_rcLast);
        }
      else if(m_fToolProp & OSHP_CAP_CREATEONUP)
        {
        TrackShape(pWnd, pWnd->ClientWnd(), &pt, FALSE, TRUE);
        CreateShape(pWnd, &pt, FALSE);
        m_fTrack = 0;
        ReleaseCapture();

        SetPointEmpty(&m_ptStart);
        SetRectEmpty(&m_rcLast);
        }

      break;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::MouseMButtonDown
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::MouseMButtonDown
  (
  Window* pWnd,
  HWND    hwnd,
  int     fwKeys,
  short   xPos,
  short   yPos
  )

  {
  I_UNUSED(pWnd);
  I_UNUSED(hwnd);
  I_UNUSED(fwKeys);
  I_UNUSED(xPos);
  I_UNUSED(yPos);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::MouseMButtonDblClk
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::MouseMButtonDblClk
  (
  Window* pWnd,
  HWND    hwnd,
  int     fwKeys,
  short   xPos,
  short   yPos
  )

  {
  I_UNUSED(pWnd);
  I_UNUSED(hwnd);
  I_UNUSED(fwKeys);
  I_UNUSED(xPos);
  I_UNUSED(yPos);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::MouseMButtonUp
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::MouseMButtonUp
  (
  Window* pWnd,
  HWND    hwnd,
  int     fwKeys,
  short   xPos,
  short   yPos
  )

  {
  I_UNUSED(pWnd);
  I_UNUSED(hwnd);
  I_UNUSED(fwKeys);
  I_UNUSED(xPos);
  I_UNUSED(yPos);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::MouseRButtonDown
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::MouseRButtonDown
  (
  Window* pWnd,
  HWND    hwnd,
  int     fwKeys,
  short   xPos,
  short   yPos
  )

  {
  I_UNUSED(hwnd);

  POINT pt = {xPos, yPos};

  OWndDPtoLP(pWnd->ObjectID(), &pt, 1);

  if(m_nTool==OSHP_STDTOOL_ARROW)
    {
    (pWnd->Items())->RButtonDown(&pt, &m_hcursor, fwKeys);
    SetCursor(m_hcursor);
    }
  else if(!m_wiShape)
    {
    ChangeTool(NULL, OSHP_STDTOOL_ARROW);
    (pWnd->Items())->MouseMove(&pt, &m_hcursor, fwKeys);
    SetCursor(m_hcursor);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::MouseRButtonDblClk
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::MouseRButtonDblClk
  (
  Window* pWnd,
  HWND    hwnd,
  int     fwKeys,
  short   xPos,
  short   yPos
  )

  {
  I_UNUSED(pWnd);
  I_UNUSED(hwnd);
  I_UNUSED(fwKeys);
  I_UNUSED(xPos);
  I_UNUSED(yPos);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::MouseRButtonUp
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::MouseRButtonUp
  (
  Window* pWnd,
  HWND    hwnd,
  int     fwKeys,
  short   xPos,
  short   yPos
  )

  {
  I_UNUSED(pWnd);
  I_UNUSED(hwnd);
  I_UNUSED(fwKeys);
  I_UNUSED(xPos);
  I_UNUSED(yPos);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::KeyDown
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::KeyDown
  (
  Window* pWnd,
  HWND    hwnd,
  int     wVKey,
  DWORD   dwKeyData
  )

  {
  I_UNUSED(hwnd);

  (pWnd->Items())->KeyDown(wVKey, dwKeyData);
  }

////////////////////////////////////////////////////////////////////////////
//
//  METHOD:       Shapes::KeyUp
//
//  DESCRIPTION:  
//
////////////////////////////////////////////////////////////////////////////

void Shapes::KeyUp
  (
  Window* pWnd,
  HWND    hwnd,
  int     wVKey,
  DWORD   dwKeyData
  )

  {
  I_UNUSED(hwnd);

 (pWnd->Items())->KeyUp(wVKey, dwKeyData);
  }

////////////////////////////////////////////////////////////////////////////
