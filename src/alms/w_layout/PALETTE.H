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
// FILE:          PALETTE.H
//
// AUTHOR:        Scott McCarty
//
// DESCRIPTION:   Contains palette window and item definitions.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef PALETTE_H
#define PALETTE_H

//////////////// Forward class declarations

class PaletteItem;

////////////////////////////////////////////////////////////////
//
// CLASS:   PaletteWindow
//
// DESCR:   Maintains a collection of buttons to choose in a window.
//
////////////////////////////////////////////////////////////////

class PaletteWindow
  {
  private:
    // Static methods
    static LRESULT ALMAPI WndProc(HWND, UINT, WPARAM, LPARAM);

    static BOOL             s_fInitOK;
    static HINSTANCE        s_hInstance;

    enum                                            // button enumerations
      {
      leftButton,
      rightButton
      };

    enum                                            // m_lStatus enumerations
      {
      fTrackingMouse  = 0x01L,
      fMouseIn        = 0x02L,
      fEndAddCalled   = 0x04L
      };

    // Private member fields
    HWND                    m_hPalette;             // HWND of created window
    LONG                    m_lStatus;
    LONG                    m_lOptions;

    PaletteItem**           m_apItems;              // array of paletteItems
    short                   m_apItemCount;          // # of valid items
    short                   m_apItemSize;           // size of array

    short                   m_nCurrentRow;          // (unitres) current row for adding
    short                   m_nCurrentCol;          // (unitres) current column for adding
    short                   m_nCols;                // # of columns (absolute, not unitres)

    short                   m_cxUnit;               // pixels
    short                   m_cyUnit;               // pixels
    short                   m_nUnitRes;             // resolution of units (2 = 1/2, 3 = 1/3, ...)

    PaletteItem*            m_pCurrentItem;         // item mouse went down in

    // Private methods
    LRESULT                 CancelMode(void);
    LRESULT                 MouseDown(POINT*, short nButton);
    LRESULT                 MouseMove(POINT*);
    LRESULT                 MouseUp(POINT*, short nButton);
    LRESULT                 Draw(HDC, RECT*);
    void 		    ReturnPrompt(WPARAM, POINT*);
    void                    ChangeRadioButton(PaletteItem*);

                            PaletteWindow(void);

  protected:
    // Overridables
    virtual void            Closing(void);          // when sysmenu/close is chosen
    virtual void            ItemChosen(PaletteItem*);


  public:

    static BOOL             Init(HINSTANCE);
    static void             Cleanup(void);

                            PaletteWindow(LONG, HWND);
    virtual                ~PaletteWindow(void);

    HWND                    Hwnd(void)              {return m_hPalette;}

    void                    SetColumns(short nCols) {m_nCols = nCols;}
    void                    SetUnitSize(short, short, short = 1);

    BOOL                    AddItem(PaletteItem*);
    void                    EndAdd(void);           // when no more items are to be added

    void                    SetItemState(short, BOOL);
    void                    Enable(BOOL);

    enum                    // options for constructor
      {
      popupPalette          = 0x01L,                // adds WS_POPUP
      titleBar              = 0x02L                 // adds WS_CAPTION|SYSMENU
      };
  };

#define Palette_allowedOptions      (PaletteWindow::popupPalette | PaletteWindow::titleBar)

////////////////////////////////////////////////////////////////
//
// CLASS:   PaletteItem
//
// DESCR:   Manages one item within a palette window.
//
////////////////////////////////////////////////////////////////

class PaletteItem
  {
  private:
    PaletteWindow*          m_pOwner;
    LONG                    m_lStyle;

    UINT                    m_nID;                  // id
    UINT                    m_gID;                  // group id (0 => no group)
    BOOL                    m_fState;               // TRUE => down/selected

    HBITMAP                 m_hBitmap;              // face of button only
    short                   m_cxBitmap;             // pixels
    short                   m_cyBitmap;             // pixels
    short                   m_cxWidth;              // in parent units
    short                   m_cyHeight;             // in parent units

    char*		    szPrompt;
    DWORD		    dwHelpID;

    // Private methods
    void                    DrawBitmap(HDC, UINT);
    void                    DrawByState(BOOL fState);
    BOOL                    GetNewState(void);
    BOOL                    GetImageState(void);
    void                    ChangeState(BOOL fNewState);

  protected:
    RECT                    m_rcItem;               // rectangle of item

    // Overridables
    virtual void            DrawItem(HDC, BOOL);

  public:
                            PaletteItem(PaletteWindow*, UINT, UINT, LONG);
    virtual                ~PaletteItem(void);

    // Called by class PaletteWindow only
    BOOL                    PointInItem(POINT*);
    void                    MouseDown(void);        // mouse went down in item
    void                    MouseUp(BOOL);          // mouse went up
    void                    MouseIn(void);          // mouse is entering
    void                    MouseOut(void);         // mouse is leaving
    void                    SetState(BOOL);
    void                    SetPosition(RECT*);     // relative to parent
    void                    Draw(HDC);

    // Called by user of item
    void		    AddPrompt(char*);
    void		    AddHelpID(DWORD);
    void 		    ShowHelp();
    BOOL		    fPress;  // don't want to press button when right-mouse-button clicking for help

    char*		    GetPrompt();
    void                    SetImage(HBITMAP);      // might transfer ownership
    void                    SetSize(short, short);  // 

    PaletteWindow*          Owner(void)             {return m_pOwner;}
    BOOL                    State(void)             {return m_fState;}
    RECT*                   Rectangle(void)         {return &m_rcItem;}
    UINT                    GroupID(void)           {return m_gID;}
    UINT                    ItemID(void)            {return m_nID;}
    LONG                    Style(void)             {return m_lStyle;}
    void                    ItemSize(short&, short&);
    
    enum
      {
      // Image transition options (used only during tracking)
      toggleImage                   = 0x01,

      // State transition options
      toggleWhenActionDone          = 0x02,
      turnOnWhenActionDone          = 0x04,
      turnOffWhenActionDone         = 0x08,

      // When is action performed?  !!Are these mutually exclusive?  It would
      // be nice if they were not
      actionOnMouseDown             = 0x10,
      actionOnMouseUp               = 0x20,

      // Miscellaneous
      ownerDraw                     = 0x40,     // send message to draw image
      doesntOwnBitmap               = 0x80      // don't delete bitmap
      };

    // "Off" is the default state (corresponds to m_fState == FALSE).
    // The typical button will look like it is NOT depressed when it is
    // off.
    // Image will always match state when mouse is not being tracked.
    // Mouse is tracked when it goes down in an item with the actionOnMouseUp
    // style.
    // Image will be drawn according to state transition options when mouse is
    // tracked AS THOUGH THE MOUSE WERE RELEASED.  This is done by...
    // Whenever state changes, then the image area is invalidated.

    // In general, the palette will take care of signals, while the items
    // will use the mouseup/down/in/out to take care of internal state and
    // drawing.
  };

////////////////////////////////////////////////////////////////
//
// #defines for commonly used button types
//
//  PAL_PUSHBUTTON:   acts just like a Window's pushbutton
//  PAL_CHECKBOX:     button toggles between up and down
//  PAL_RADIOBUTTON:  when button goes ON, other buttons in group will go OFF
//  PAL_STATELESS:    doesn't change image or state when pressed
//
////////////////////////////////////////////////////////////////

#define PAL_PUSHBUTTON    (                                       \
                            PaletteItem::turnOffWhenActionDone |  \
                            PaletteItem::toggleImage           |  \
                            PaletteItem::actionOnMouseUp          \
                          )

#define PAL_CHECKBOX      (                                       \
                            PaletteItem::toggleWhenActionDone  |  \
                            PaletteItem::actionOnMouseUp          \
                          )

#define PAL_RADIOBUTTON   (                                       \
                            PaletteItem::turnOnWhenActionDone |   \
                            PaletteItem::actionOnMouseUp          \
                          )

#define PAL_STATELESS     (                                       \
                            PaletteItem::actionOnMouseDown        \
                          )

#endif  // !PALETTE_H
