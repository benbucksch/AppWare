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
// FILE:          HIERLIST.H
//
// AUTHOR:        Scott McCarty
//
// DESCRIPTION:   Definition of hierarchical list of items.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef HIERLIST_H
#define HIERLIST_H

//////////////// Forward class definitions

class SListItem;

////////////////////////////////////////////////////////////////
//
// CLASS:   SItemListWindow
//
// DESCR:   This class manages a list of items similarly to a listbox.
//          However, the mouse interaction is more like the file manager.
//          The items can be hierarchical in nature, though very few
//          assumptions are made about this.
//
////////////////////////////////////////////////////////////////

class SItemListWindow
  {
  private:
    // Static methods
    static LRESULT ALMAPI WndProc(HWND, UINT, WPARAM, LPARAM);
    static short          SetMouseFlags(POINT&, BOOL, WPARAM, LPARAM);

    // Static fields
    static BOOL           s_fInitOK;
    static HINSTANCE      s_hInstance;

    // Private state information
    enum                                      // for Mouse{Down,Move,Up}
      {
      leftButton          = 0x01,
      rightButton         = 0x02,
      shiftKey            = 0x04,
      ctrlKey             = 0x08
      };

    enum                                      // for m_lStatus
      {
      trackingMouse       = 0x001,            // we need to release capture
      partialLine         = 0x002,            // not all of the bottom line shows
      unselectOnUp        = 0x004,            // unselect pMouseDownItem
      unselectOthersOnUp  = 0x008,            // unselect all but pMouseDownItem
      wasDragged          = 0x010,            // drag occurred
      destroyCursor       = 0x020,            // destroy cursor if wasDragged
      singleItemDrag      = 0x040,            // only one item was dragged
      fDisabled           = 0x080,            // window is disabled
      selectionChange     = 0x100,            // call SelectionChange() up button up
      nonDraggable        = 0x200             // cannot drag item
      };

    // Private member fields
    HWND                  m_hListWindow;      // Windows HWND

    LONG                  m_lStatus;          // runtime info
    LONG                  m_lOptions;         // user options

    SListItem*            m_pRootItem;        // marked as root
    short                 m_nFullCount;       // flattened count of items

    SListItem*            m_pAnchorItem;      // item that was last selected w/o shift
    SListItem*            m_pMouseDownItem;   // item that received mouse down
    POINT                 m_ptDown;           // where mouse down occurred
    HCURSOR               m_hDragCursor;      // shown during drag in list window
    HCURSOR               m_hOldCursor;       // original during drag

    HFONT                 m_hFont;            // user-supplied font for text
    HFONT                 m_hBoldFont;        // for drawing items with children
    short                 m_nFontHeight;      // font height in pixels

    short                 m_cxImages;         // width of bitmaps (should be the same)
    short                 m_cyImages;         // height of bitmaps (should be the same)
    short                 m_nImageSpace;      // amount (pixels) to space text from iamge
    short                 m_nItemHeight;      // same for all items; in pixels
    short                 m_nMaxDisplay;      // # of items that are visible (last might be partial)
    short                 m_nTopIndex;        // index (0-based) of top flattened item
    short                 m_nMaxTopIndex;     // max index (0-based) of top flattened item
    
    HWND*                 m_apDragTargets;    // array of HWNDs for drag target
    short*                m_apDragTargFlags;  // flags for above HWNDs
    short                 m_apDragTargetCount;// # of items in array
    short                 m_apDragTargetSize; // size of items array

    // Private methods
                          SItemListWindow(void);  // disabled

    void                  Flatten(void);
    void                  UpdateItemHeight(void);
    void                  UpdateBoldFont(HDC);  // of font in HDC
    void                  UpdateScroll(void);
    short                 FindTargetWindow(HWND);
    HWND                  FindDragWindow(POINT*);
    void                  ChangeCursor(HCURSOR, BOOL);

    LRESULT               MouseDown(POINT*, short);
    LRESULT               MouseDblClick(POINT*, short);
    LRESULT               MouseMove(POINT*, short);
    LRESULT               MouseUp(POINT*, short);
    LRESULT               Resize(HWND, UINT, int, int);
    LRESULT               Draw(HDC, RECT*);
    LRESULT               VScroll(HWND, HWND, UINT, int);

  protected:
    enum                  // for GetCursor
      {
      sourceOfDrag      = 1,
      targetForDrag,
      invalidTarget
      };

    virtual BOOL              GetCursor(short, SListItem*, HCURSOR&);
    virtual void              DragRelease(HWND, POINT);
    virtual void              SelectionChange(void);    // reported on buttonup
    virtual void              ItemDblClicked(SListItem*);

  public:

    // Static methods
    static BOOL               Init(HINSTANCE);

                              SItemListWindow(HWND, LONG, DWORD);
    virtual                  ~SItemListWindow(void);


    // Instance methods
    short                     ImageWidth(void)      {return m_cxImages;}
    short                     ItemHeight(void)      {return m_nItemHeight;}
    short                     ImageSpace(void)      {return m_nImageSpace;}
    HWND                      Hwnd(void)            {return m_hListWindow;}
    HFONT                     BoldFont(void)        {return m_hBoldFont;}
    BOOL                      ItemIsVisible(short);
    BOOL                      SingleItemDrag(void)  {return ((m_lStatus & singleItemDrag) != 0);}

    BOOL                      SetFont(LOGFONT*);  // defaults to system otherwise

    short                     MapFlatPositionToY(short);
    short                     MapYToFlatPosition(short);

    BOOL                      AddItem(SListItem*, LONG*, short);  // n-1 LONGs for UID path
    void                      ExpandAll(BOOL);
    void                      DeleteAll(void);
    void                      ForceSelectionChange(void)  {m_lStatus |= selectionChange;}

    BOOL                      AddDragTarget(HWND, short);
    BOOL                      RemoveDragTarget(HWND);
    SListItem*                EnumItems(short&, short);

    void                      Enable(BOOL);

    enum                      // options to ::Create()
      {
      allowDragDrop           = 0x01,
      sortItems               = 0x02
      };

    enum                      // options to AddDragTarget
      {
      useParent               = 0x01              // use parent window
      };

  };

#define ItemList_allowedOptions   (SItemListWindow::allowDragDrop | \
                                   SItemListWindow::sortItems)
#define ItemList_allowedDragFlags (SItemListWindow::useParent)

////////////////////////////////////////////////////////////////
//
// CLASS:   SListItem
//
// DESCR:   This class is contained within the SItemListWindow class.
//          It is responsible for displaying, etc., the items.  It can
//          be the owner of more SListItems.
//
////////////////////////////////////////////////////////////////

class SListItem
  {
  private:
    SItemListWindow*        m_pMaster;          // owner of everything
    SListItem*              m_pParent;          // hierarchical parent of item
    LONG                    m_lUserType;        // user-supplied for convenience

    short                   m_fStatus;          // status of item
    LONG                    m_lUID;             // unique ID for insert/delete
    short                   m_nFlatPosition;    // flattened position

    SListItem**             m_apSubItems;       // array of owned SListItems
    short                   m_apSubItemCount;   // # of items in array
    short                   m_apSubItemSize;    // size of items array

    char*                   m_pszTitle;         // text to display
    HBITMAP                 m_hImage;           // optional
    short                   m_cxImage;          // width of image (pixels)
    short                   m_cyImage;          // height of image (pixels)
    short                   m_nXOffset;         // offset to start drawing from

    // Private methods
                            SListItem(void);    // disabled

    void                    Sort(void);
    void                    SetupDraw(void);

  public:
                            SListItem(SItemListWindow* pMaster, LONG lType);
    virtual                ~SListItem(void);

    LONG                    UserType(void)      {return m_lUserType;}
    short                   Status(void)        {return m_fStatus;}
    LONG                    UID(void)           {return m_lUID;}
    short                   FlatPosition(void)  {return m_nFlatPosition;}
    void                    SetSortable(void)   {m_fStatus |= sortItems;}
    void                    SetGreedy(void)     {m_fStatus |= greedySelect;}
    void                    SetNoDrag(void)     {m_fStatus |= nonDraggable;}

    short                   XOffset(void)       {return m_nXOffset;}
    short                   LeftEdge(void);
    HBITMAP                 Image(void)         {return m_hImage;}
    short                   ImageWidth(void)    {return m_cxImage;}
    short                   ImageHeight(void)   {return m_cyImage;}
    char*                   Title(void)         {return m_pszTitle;}

    void                    SetUID(LONG l)      {m_lUID = l;}
    void                    SetTitle(char*);    // copies text
    void                    SetImage(HBITMAP);  // doesn't transfer ownership!

    short                   Flatten(short);     // assign FlatOrder; return next flattened position
    SListItem*              FindItem(short);    // takes flat position
    void CDECL              VisitItems(short, LONG, ...);

    void                    DeleteChildren(void);
    BOOL                    Select(BOOL);       // selects or unselects item
    BOOL                    Expand(BOOL);       // returns TRUE if count changes

    BOOL                    Add(SListItem*, LONG*, short, SListItem*&);

    void                    Invalidate(BOOL);
    void                    Draw(HDC, short, short, RECT*);


    enum                    // for m_fStatus
      {
      exists        = 0x001,                  // always set
      hasChildren   = 0x002,                  // contains other SListItem's
      hasBitmap     = 0x004,                  // should display bitmap
      rootItem      = 0x008,                  // m_pParent is also NULL.
      selected      = 0x010,                  // draw with texthighlight/highlight
      focus         = 0x020,                  // draw with DrawFocusRect()
      expanded      = 0x040,                  // display children
      sortItems     = 0x080,                  // sort items
      greedySelect  = 0x100,                  // unselect others always when selected
      nonDraggable  = 0x200                   // cannot be dragged
      };


    enum                    // for short of VisitItems()
      {
      clearSelection    = 1,                  // clears and invalidates
      lostFocus,                              // redraws if necessary
      gotFocus,                               // redraws if necessary
      expandAll,                              // doesn't redraw (done by caller)
      collapseAll,                            // doesn't redraw (done by caller)
      testImageHeight,                        // verify bitmaps have same height
      testImageWidth,                         // verify bitmaps have same width
      syncDraw,                               // setup draw values
      count,                                  // count      
      clearGreedy                             // clear those that want to be the only ones selected
      };
  };

#endif  // !HIERLIST_H
