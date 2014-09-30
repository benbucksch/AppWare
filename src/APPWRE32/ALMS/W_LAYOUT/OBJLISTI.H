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
// FILE:          OBJLISTI.H
//
// AUTHOR:        Scott McCarty
//
// DESCRIPTION:   Internal-only class definitions, etc. for object palette
//                window.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef OBJLISTI_H
#define OBJLISTI_H

#ifndef HIERLIST_H
#include "hierlist.h"
#endif

class ObjectPaletteWindow;

////////////////////////////////////////////////////////////////////////////
//
// CLASS:   ObjectTypeItem
//
// DESCR:   Handles the insertion of a type into the type list window.
//
////////////////////////////////////////////////////////////////////////////

class ObjectTypeItem : public SListItem
  {
  private:
    ATYPEID              m_type;               // ATYPEID of the item
    HBITMAP             m_hImage;             // 1/2 size image of type

  public:
                        ObjectTypeItem(ATYPEID, SItemListWindow*);
                       ~ObjectTypeItem(void);

    ATYPEID              TypeID(void)          {return m_type;}
  };

////////////////////////////////////////////////////////////////////////////
//
// CLASS:   ObjectItem
//
// DESCR:   Handles the insertion of an object into the object list window.
//
////////////////////////////////////////////////////////////////////////////

class ObjectItem : public SListItem
  {
  private:
    AOBJECTID            m_oiObject;           // AOBJECTID of item
    ApQueryInfo         m_QueryInfo;          // from AObjQueryProtocol
    HBITMAP             m_hImage;             // 1/2 size image of type

  public:
                        ObjectItem(AOBJECTID, ApQueryInfo*, SItemListWindow*);
                       ~ObjectItem(void);

    inline  AOBJECTID    ObjectID()              {return m_oiObject;}
    ApQueryInfo*        ProtocolInfo(void)      {return &m_QueryInfo;}
  };

////////////////////////////////////////////////////////////////////////////
//
// CLASS:   ObjectListWindow
//
// DESCR:   Object list window class that uses SItemListWindow.  It manages
//          a list of either types or objects.
//
////////////////////////////////////////////////////////////////////////////

class ObjectListWindow : public SItemListWindow
  {
  private:
    Layout*               m_pLayout;          // communication to layout module
    BOOL                  m_fTypes;           // TRUE => holds types (not objects)
    ObjectPaletteWindow*  m_pParent;          // graphical owner of this window

  public:
    BOOL                GetCursor(short, SListItem*, HCURSOR&);
    void                DragRelease(HWND, POINT);
    void                SelectionChange(void);
    void                ItemDblClicked(SListItem*);


                        ObjectListWindow(ObjectPaletteWindow*, Layout*,
                                          HWND, LONG, DWORD, BOOL);
                       ~ObjectListWindow(void);
  };

////////////////////////////////////////////////////////////////////////////
//
// CLASS:   ObjectPaletteWindow
//
// DESCR:   Manages the two objectlist windows.
//
////////////////////////////////////////////////////////////////////////////

class ObjectPaletteWindow
  {
  private:
    static LRESULT CALLBACK EXPORT WndProc(HWND, UINT, WPARAM, LPARAM);
    static HINSTANCE      s_hInstance;      // module handle
    static char           s_szWndClass[32]; // unique class name

    HWND                  m_hWnd;           // HWND of window
    ObjectListWindow*     m_pTypeList;      // type list window
    ObjectListWindow*     m_pObjList;       // object list window
    short                 m_nTypeSize;      // height (pixels) of type list
    short                 m_nObjSize;       // height (pixels) of obj list
    short                 m_nTrackLine;     // current bar position
    short                 m_cxClient;       // width of client part of window
    short                 m_cyClient;       // height of client part of window
    BOOL                  m_fResizing;      // TRUE => resizing with bar
    HCURSOR               m_hResizeCursor;  // standard splitbar cursor
    BOOL                  m_fFreeCursor;    // TRUE => loaded hResizeCursor

    // From WM_* messages
    LRESULT               MouseDown(POINT*);
    LRESULT               MouseMove(POINT*);
    LRESULT               MouseUp(POINT*, BOOL);
    LRESULT               Resize(HWND hwnd, UINT state, int cx, int cy);
    LRESULT               Draw(HDC, RECT*);
    LRESULT               GetMinMaxInfo(MINMAXINFO*);

    void                  DrawTrackLine(void);
    void                  ResizeTypes(short);
    short                 ConstrainListHeight(short);
    HCURSOR               GetResizeCursor(void);

  public:
    static BOOL           Init(HINSTANCE);

                          ObjectPaletteWindow(Layout*, HWND);
                         ~ObjectPaletteWindow(void);

    BOOL                  Initialized(void)     const;
    HWND                  Hwnd(void)            const {return m_hWnd;}

    short                 TypeListHeight(void)  const {return m_nTypeSize;}
    short                 ObjsListHeight(void)  const {return m_nObjSize;}

    void                  FillObjectList(void);
    void                  SetFont(void);
    void                  SetCaption(void);
    void                  SetPosition(POINT, short, short, short, short);

    void                  AddDragTarget(HWND, short);
    void                  RemoveDragTarget(HWND);
    void                  Enable(BOOL);
  };


// ****************************************************************


#endif  // !OBJLISTI_H
