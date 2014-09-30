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
// FILE:          "lay_wdat.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef LAY_WDAT_H
#define LAY_WDAT_H

// make sure the necessary files are included //////////////////////////////

#ifndef A_ALM_H
#include <a_alm.h>
#endif

#ifndef SHP_ATTR_H
#include "shp_attr.h"
#endif

// defines for the window flags ////////////////////////////////////////////

#define WF_TITLEBAR               0x00000001L
#define WF_SYSMENU                0x00000002L
#define WF_MAXBUTTON              0x00000004L
#define WF_MINBUTTON              0x00000008L
#define WF_HORZSCROLL             0x00000010L
#define WF_VERTSCROLL             0x00000020L
#define WF_RESIZEBORDER           0x00000040L
#define WF_DIALOGFRAME            0x00000080L
#define WF_MODAL                  0x00000100L
#define WF_ALWAYSONTOP            0x00000200L
#define WF_INITVISIBLE            0x00000400L
#define WF_DRAWSMOOTH             0x00000800L
#define WF_QUITONCLOSE            0x00001000L

// palette structure ///////////////////////////////////////////////////////

struct  WndPalette
  {
  WORD          palVersion;
  WORD          palNumEntries;
  PALETTEENTRY  palPalEntry[256];
  };

////////////////////////////////////////////////////////////////////////////

class WndData

  {
  private:
    static WndPalette   sm_palette;   // default 256 color palette
  protected:
    static HPALETTE     sm_hpal;      // handle to default palette
  public:
    static inline HPALETTE  GetPaletteHandle()    {return sm_hpal;}
    static void             CreatePalette();
    static void             DestroyPalette();

  private:
    long        m_style;          // the style --- see WF_*

  protected:
    AOBJECTID    m_oiWnd;          // object id of the window object
    BOOL        m_fDirty;         // flag for window changes
    short       m_nChanged;       // number of times changes have been saved
    FillAttrib  m_fill;           // the window background fill attributes
    RECT        m_rcInitClient;   // initial client rectangle
    short       m_nInitCmdShow;   // initial window state
    BOOL        m_fLimitMaxSize;  // is the maximum size limited?
    POINT       m_ptMaxSize;      // maximum window size (if limited)
    POINT       m_ptMinSize;      // minimum window size
    POINT       m_ptScrollSize;   // scrolling area
    AOBJECTID    m_oiMenu;         // menu object id
    char        m_szTitle[256];   // the title of the window

    void        GetWindowData();
    void        SaveWindowData();
    DWORD       m_GetWindowStyle();
    DWORD       m_GetWindowStyleEx();

    // access to the m_style member
    inline long Style()           {return m_style;}
    inline void SetStyle(long s)  {m_style = s;}
    inline BOOL HasTitleBar()     {return (m_style & WF_TITLEBAR)     ? TRUE : FALSE;}
    inline BOOL HasSysMenu()      {return (m_style & WF_SYSMENU)      ? TRUE : FALSE;}
    inline BOOL HasMaxButton()    {return (m_style & WF_MAXBUTTON)    ? TRUE : FALSE;}
    inline BOOL HasMinButton()    {return (m_style & WF_MINBUTTON)    ? TRUE : FALSE;}
    inline BOOL HasDialogFrame()  {return (m_style & WF_DIALOGFRAME)  ? TRUE : FALSE;}
    inline BOOL IsModal()         {return (m_style & WF_MODAL)        ? TRUE : FALSE;}
    inline BOOL IsPopup()         {return (m_style & WF_ALWAYSONTOP)  ? TRUE : FALSE;}
    inline BOOL IsInitVisible()   {return (m_style & WF_INITVISIBLE)  ? TRUE : FALSE;}
    inline BOOL QuitOnClose()     {return (m_style & WF_QUITONCLOSE)  ? TRUE : FALSE;}

    // protected conditional styles
    inline BOOL HasResizeBorder() {return (!HasDialogFrame() && (m_style & WF_RESIZEBORDER)) ? TRUE : FALSE;}
    inline BOOL HasMenuBar()      {return (!HasDialogFrame() && m_oiMenu) ? TRUE : FALSE;}

  public:
                WndData(AOBJECTID oiWnd);
               ~WndData();

    // public conditional styles
    inline BOOL HasHorzScroll()   {return (!HasDialogFrame() && (m_style & WF_HORZSCROLL))   ? TRUE : FALSE;}
    inline BOOL HasVertScroll()   {return (!HasDialogFrame() && (m_style & WF_VERTSCROLL))   ? TRUE : FALSE;}
    inline AOBJECTID MenuID()      {return m_oiMenu;}
  };

////////////////////////////////////////////////////////////////////////////

#endif  // #ifdef LAY_WDAT_H
