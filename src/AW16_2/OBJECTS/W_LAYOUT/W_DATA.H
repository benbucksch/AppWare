/////////////////////////////////////////////////////
// Private Header file for the window object  ///////
/////////////////////////////////////////////////////
#ifndef W_DATA_H
#define W_DATA_H

#ifndef   OP_WND_H
#include  "op_wnd.h"
#endif

#ifndef   __MEM_H
#include  <mem.h>
#endif

#ifndef   O_WND_H
#include  "o_wnd.h"
#endif

#define I_THE_VERSION               201


#define OWND_EVT_WINDOWSTARTUP          MAKEEVENTID('s','t','r','W')
#define OWND_EVT_OPENWINDOWOBJECT       MAKEEVENTID('o','p','n','W')
#define OWND_EVT_CLOSEWINDOWOBJECT      MAKEEVENTID('c','l','s','W')


/////////////////////////
// statusFlag defines  //
/////////////////////////
#define I_SF_VISIBLE            0x00000001L
#define I_SF_ENABLED            0x00000002L
#define I_SF_ACTIVATABLE        0x00000004L
#define I_SF_TRANSPARENT        0x00000008L
#define I_SF_TABSTOP            0x00000010L
#define I_SF_GROUPSTOP          0x00000020L
#define I_SF_USESCURSORKEYS     0x00000040L
#define I_SF_USESTAB            0x00000080L
#define I_SF_USESDEFAULTKEY     0x00000100L
#define I_SF_USESCANCELKEY      0x00000200L
#define I_SF_NEEDSTOBEDRAWN     0x00000400L
#define I_SF_LOCKED             0x00000800L
#define I_SF_LITEITEM           0x10000000L
#define I_SF_HWNDITEM           0x20000000L
#define I_SF_SHAPE              0x40000000L
#define I_SF_GROUP              0x80000000L

#define I_SF_ITEMTYPES          (I_SF_LITEITEM | I_SF_HWNDITEM)

/////////////////////////
// Grows Flag defines  //
/////////////////////////
#define I_GROW_TOP              1
#define I_GROW_LEFT             2
#define I_GROW_BOTTOM           4
#define I_GROW_RIGHT            8

//////////////////////////////////
////////  Window Flag Defines ////
//////////////////////////////////
#define I_WF_TITLEBAR           0x00000001L
#define I_WF_SYSMENU            0x00000002L
#define I_WF_MAXBUTTON          0x00000004L
#define I_WF_MINBUTTON          0x00000008L
#define I_WF_HORZSCROLL         0x00000010L
#define I_WF_VERTSCROLL         0x00000020L
#define I_WF_RESIZE             0x00000040L
#define I_WF_DIALOGFRAME        0x00000080L
#define I_WF_MODAL              0x00000100L
#define I_WF_POPUP              0x00000200L
#define I_WF_INITVISIBLE        0x00000400L
#define I_WF_DRAWSMOOTH         0x00000800L
#define I_WF_OPEN               0x00002000L
#define I_WF_MODAL_PRESENT      0x00004000L  
#define I_WF_QUIT_ON_CLOSE      0x00008000L  

#define I_DEFAULT_WINDOW_STYLE (I_WF_SYSMENU|I_WF_TITLEBAR|I_WF_MAXBUTTON|I_WF_MINBUTTON|I_WF_RESIZE|I_WF_INITVISIBLE|I_WF_QUIT_ON_CLOSE);

/////////////////////////////
#define I_WINDOW_FOREGROUND     0xFE000000L
#define I_WINDOW_BACKGROUND     0xFF000000L

///////////////////////////
#define I_TITLE_LENGTH          255

//////Window State Flags //////////////////
#define I_WSF_CLOSING           0x0001
#define I_WSF_OPENING           0x0002
#define I_WSF_ICONIC            0x0004
#define I_WSF_HSCROLL           0x0010
#define I_WSF_VSCROLL           0x0020
#define I_WSF_MENUDOWN          0x0040

///////////////////////////////
//  Window Object Signals 
///////////////////////////

#define I_SIG_SELECTED              0
#define I_SIG_OPENED                1
#define I_SIG_CLOSED                2
#define I_SIG_MOVED                 3
#define I_SIG_RESIZED               4
#define I_SIG_ICONIFIED             5
#define I_SIG_ICONRESTORED          6
#define I_SIG_ABOUTTOCLOSE          20
#define I_SIG_NUM_BASE_SIGNALS      8    //The number of signals a window will have not counting the one off its items

#define I_SIG_INOFFSET              0
#define I_SIG_OUTOFFSET             1

///////////////////////////
/// Data Block Defines  //
////////////////////////////

#define I_DATA_WINDOW               0L
#define I_DATA_LAYOUT               1L
#define I_DATA_ICON                 2L
#define I_DATA_RESERVED             3L
#define I_DATA_ITEMS_OFFSET         4L

///////////////////////////

#define I_ITEM_ID_OFFSET            1000
#define I_NOTHING_STRING            "\1\0"
#define I_EXTRABYTES_ID_OFFSET      0 //The offset of the object id in the window extrabytes

///////////////////////////

typedef enum {CHECK_ENABLE_AND_VISIBLE_BIT=TRUE, DONT_CHECK_ENABLE_AND_VISIBLE_BIT=FALSE} BOOL_CHECK_ENABLE_AND_VISIBLE; //Used for WINDOWAPI::IsItemActivatable

///////////////////////////

LRESULT far pascal _export WndCallbackProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

///////////////////////////
///////////////////////////
class WINDOW;

class WNDITEM_DATA
  {
  private:
    short                   m_size;
    OBJECTID                m_oiItem;
    RECT                    m_rcItem;
    LONG                    m_uidItem;
    LONG                    m_itemStatus;
    short                   m_itemGrow;
    char                    m_szShapeDll[13];
    char                    m_unused[10];

  public:
    WNDITEM_DATA();
    WNDITEM_DATA(OpWndItemD* wi);

    BOOL Validate(pAObjMessage theSystem);
    BOOL Check(void); 
    void GetName(short idxItem, LPSTR szName, short cbLength);
    OpWndItemD* CreateVI(OBJECTID oiParent, long lDataBlockSize, int idxItem);
    OpWndItemD* CreateShapeVI(OBJECTID oiParent, short sVer, short sType, int nIdx);

    inline long GetUID(void) {return m_uidItem;}

    friend WINDOW;
  };

/////////////////////////////////

struct SHAPE_DLLS
  {
  TYPEID                  tiShape;
  char                    szDllName[13];
  HINSTANCE               hInstShape;
  OShapeAdmin             shapeAdminProc;
  };
typedef SHAPE_DLLS far * LPSHAPE_DLLS;

/////////////////////////////////
struct MENU_ACCEL_INFO
  {
  short                   itemID;
  char                    vKey;
  char                    modifier;
  };
typedef MENU_ACCEL_INFO far * LPMENU_ACCEL_INFO;

#define I_MODIFIER_FOR_MENU   0x80

/////////////////////////////////

//This class is a friend to OpWndItemD and only contains
//static inline functions used to acces private members
//of OpWndItemD
class WINDOWAPI
  {
  private:
    WINDOWAPI();

  public:
    static inline void SetItemVisible(OpWndItemD* wi, BOOL set)
      { if(set) wi->m_status |= I_SF_VISIBLE; else wi->m_status &= ~I_SF_VISIBLE;}
    static inline BOOL IsItemVisible(OpWndItemD* wi)
      {return wi->m_status & I_SF_VISIBLE ? TRUE:FALSE;}

    static inline void SetItemEnabled(OpWndItemD* wi, BOOL set)
      { if(set) wi->m_status |= I_SF_ENABLED; else wi->m_status &= ~I_SF_ENABLED;}
    static inline BOOL IsItemEnabled(OpWndItemD* wi)
      {return wi->m_status & I_SF_ENABLED ? TRUE:FALSE;}

    static inline void SetItemActivatable(OpWndItemD* wi, BOOL set)
      { if(set) wi->m_status |= I_SF_ACTIVATABLE; else wi->m_status &= ~I_SF_ACTIVATABLE;}
    static inline BOOL IsItemActivatable(OpWndItemD* wi, BOOL_CHECK_ENABLE_AND_VISIBLE bCheckEnableAndVisible = CHECK_ENABLE_AND_VISIBLE_BIT)
      {return bCheckEnableAndVisible == CHECK_ENABLE_AND_VISIBLE_BIT ? 
        (wi->m_status & (I_SF_ACTIVATABLE|I_SF_ENABLED|I_SF_VISIBLE))==(I_SF_ACTIVATABLE|I_SF_ENABLED|I_SF_VISIBLE) : 
        (wi->m_status & I_SF_ACTIVATABLE ? TRUE:FALSE);
      }
      
    static inline void SetItemTransparent(OpWndItemD* wi, BOOL set)
      { if(set) wi->m_status |= I_SF_TRANSPARENT; else wi->m_status &= ~I_SF_TRANSPARENT;}
    static inline BOOL IsItemTransparent(OpWndItemD* wi)
      {return wi->m_status & I_SF_TRANSPARENT ? TRUE:FALSE;}

    static inline void SetItemTabstop(OpWndItemD* wi, BOOL set)
      { if(set) wi->m_status |= I_SF_TABSTOP; else wi->m_status &= ~I_SF_TABSTOP;}
    static inline BOOL IsItemTabstop(OpWndItemD* wi)
      {return wi->m_status & I_SF_TABSTOP ? TRUE:FALSE;}

    static inline void SetItemGroupstop(OpWndItemD* wi, BOOL set)
      { if(set) wi->m_status |= I_SF_GROUPSTOP; else wi->m_status &= ~I_SF_GROUPSTOP;}
    static inline BOOL IsItemGroupstop(OpWndItemD* wi)
      {return wi->m_status & I_SF_GROUPSTOP ? TRUE:FALSE;}

    static inline void SetItemUsesCursorKeys(OpWndItemD* wi, BOOL set)
      { if(set) wi->m_status |= I_SF_USESCURSORKEYS; else wi->m_status &= ~I_SF_USESCURSORKEYS;}
    static inline BOOL DoesItemUseCursorKeys(OpWndItemD* wi)
      {return wi->m_status & I_SF_USESCURSORKEYS ? TRUE:FALSE;}

    static inline void SetItemUsesTab(OpWndItemD* wi, BOOL set)
      { if(set) wi->m_status |= I_SF_USESTAB; else wi->m_status &= ~I_SF_USESTAB;}
    static inline BOOL DoesItemUseTab(OpWndItemD* wi)
      {return wi->m_status & I_SF_USESTAB ? TRUE:FALSE;}

    static inline void SetItemUsesDefaultKey(OpWndItemD* wi, BOOL set)
      { if(set) wi->m_status |= I_SF_USESDEFAULTKEY; else wi->m_status &= ~I_SF_USESDEFAULTKEY;}
    static inline BOOL DoesItemUseDefaultKey(OpWndItemD* wi)
      {return wi->m_status & I_SF_USESDEFAULTKEY ? TRUE:FALSE;}

    static inline void SetItemUsesCancelKey(OpWndItemD* wi, BOOL set)
      { if(set) wi->m_status |= I_SF_USESCANCELKEY; else wi->m_status &= ~I_SF_USESCANCELKEY;}
    static inline BOOL DoesItemUseCancelKey(OpWndItemD* wi)
      {return wi->m_status & I_SF_USESCANCELKEY ? TRUE:FALSE;}

    static inline void SetLiteItem(OpWndItemD* wi)
      { wi->m_status &= ~I_SF_ITEMTYPES; wi->m_status |= I_SF_LITEITEM;}
    static inline BOOL IsLiteItem(OpWndItemD* wi)
      {return wi->m_status & I_SF_LITEITEM ? TRUE:FALSE;}

    static inline void SetHwndItem(OpWndItemD* wi)
      {  wi->m_status &= ~I_SF_ITEMTYPES; wi->m_status |= I_SF_HWNDITEM;}
    static inline BOOL IsHwndItem(OpWndItemD* wi)
      {return wi->m_status & I_SF_HWNDITEM ? TRUE:FALSE;}

    static inline void SetItemNeedsToBeDrawn(OpWndItemD* wi, BOOL set)
      { if(set) wi->m_status |= I_SF_NEEDSTOBEDRAWN; else wi->m_status &= ~I_SF_NEEDSTOBEDRAWN;}
    static inline BOOL DoesItemNeedToBeDrawn(OpWndItemD* wi)
      {return wi->m_status & I_SF_NEEDSTOBEDRAWN ? TRUE:FALSE;}

    static inline void SetItemIsShape(OpWndItemD* wi, BOOL set)
      { if(set) wi->m_status |= I_SF_SHAPE; else wi->m_status &= ~I_SF_SHAPE;}
    static inline BOOL IsItemShape(OpWndItemD* wi)
      {return wi->m_status & I_SF_SHAPE ? TRUE:FALSE;}

    static inline void SetItemGrowflag(OpWndItemD* wi, int growflag)
      { wi->m_grow = growflag; }
    static inline int GetItemGrowflag(OpWndItemD* wi)
      {return wi->m_grow;}

    static inline int GetItemIndex(OpWndItemD* wi) {return wi->m_index;}

    static inline void SetItemIsGroup(OpWndItemD* wi, BOOL set)
      { if(set) wi->m_status |= I_SF_GROUP; else wi->m_status &= ~I_SF_GROUP;}
    static inline BOOL IsItemGroup(OpWndItemD* wi)
      {return wi->m_status & I_SF_GROUP ? TRUE:FALSE;}

    static inline LONG UIDFromItem(OpWndItemD* wi) {return wi->m_uidItem;}

    static inline void SetItemLocked(OpWndItemD* wi, BOOL fLocked)
      { if(fLocked) wi->m_status |= I_SF_LOCKED; else wi->m_status &= ~I_SF_LOCKED;}
    static inline BOOL IsItemLocked(OpWndItemD* wi)
      {return wi->m_status & I_SF_LOCKED ? TRUE:FALSE;}

  };


//////////////////////////////////

class WINDOWRUNTIME
  {
  private:
    OBJECTID              m_oiActive;         //oi of the active window object
    OBJECTID              m_oiModal;          //oi of the active modal window object, if any

    OpWndItemD*           m_wiCapture;        //The window item that has the mouse capture
    OpWndItemD*           m_wiMouseIn;        //the window item that the mouse is in

    BOOL                  m_bAppIsActive;     //TRUE if the application is active

    HCURSOR               m_hCursor;          //Handle of the cursor to use.
    HCURSOR               m_hCursorMaster;    //Cursor to use when wipReleaseCursor is called.
    BOOL                  m_bArrowCursor;     //TRUE if the cursor to use is the arrow

    BOOL                  m_bIsColor;         //True is the color version should be used
    BOOL                  m_bIsLayoutTime;    //True is in the window layout

    HHOOK                 m_hNextMouseHook;
    HHOOK                 m_hNextKeyHook;
    int                   m_numWindowsTotal;        //The total number of window objects
    int                   m_numVisibleWindows;      //The number of visible windows

    MEMBLOCKID            m_miShapeDlls;

    MEMBLOCKID            m_miZorder;         //This memblock is left locked throughout the lifetime of this class
    OBJECTID far *        m_oiptrZorder;      
    BOOL                  m_bZorderDone;


  public:

    WINDOWRUNTIME(int numWindows, BOOL bLayoutTime);
    ~WINDOWRUNTIME();

    inline OBJECTID GetActiveWindowObject() {return m_oiActive;}
		HWND GetActiveWindowObjectHandle(void);
    BOOL ActivateWindow(OBJECTID oi);
    OpWndItemD* GetActiveItem();   //the active in the active window

    inline OBJECTID GetModalWindowObject() {return m_oiModal;}

    inline OpWndItemD* GetCaptureItem() {return m_wiCapture;}
    OpWndItemD* SetCaptureItem(OpWndItemD* wi);
    void ReleaseCapture();

    inline BOOL IsAppActive() {return m_bAppIsActive;}
    inline void SetAppIsActive(BOOL set) {m_bAppIsActive = set;}

    inline HCURSOR GetCursor() {return m_hCursor;}
    HCURSOR SetCursor(HCURSOR hCursor, BOOL isMaster);
    HCURSOR ReleaseCursor();
    inline BOOL UsingArrowCursor() {return m_bArrowCursor;}

    inline BOOL IsColor() {return m_bIsColor;}
    inline void SetIsColor(BOOL set){m_bIsColor = set;}

    inline BOOL IsLayoutTime() {return m_bIsLayoutTime;}

    void InstallHooks();
    void RemoveHooks();
    LRESULT CheckForAccelKey(WPARAM wParam, OBJECTID oiWindow);
    LRESULT MouseMoveHook(POINT pt);
    LRESULT MouseDownHook(HWND hwndTarget, POINT pt);
    LRESULT EnterKeyHook(WPARAM wParam, LPARAM lParam);
    LRESULT EscKeyHook(WPARAM wParam, LPARAM lParam);
    LRESULT TabKeyHook();
    LRESULT CursorKeyHook(WPARAM wParam);

    inline HHOOK GetNextMouseHook() {return m_hNextMouseHook;}
    inline HHOOK GetNextKeyHook() {return m_hNextKeyHook;}

    BOOL IsHwndAWindowObject(HWND hwnd);
    OBJECTID WindowObjectFromPoint(POINT pt);
    OBJECTID GetWindowObjectFromHwnd(HWND hwnd);
    OpLiteWndItemD* GetLiteItemFromPointOrCapture(HWND hwnd, POINT pt);
    OpHwndWndItemD* GetItemFromHwnd(HWND hwndItem);

    void SetZOrderList();

    LRESULT WmCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmActivate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmNcButton(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmMouseActivate(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmSysCommand(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmClose(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmGetMinMaxInfo(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmSetCursor(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmMove(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmSize(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmEraseBkgnd(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmPaint(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmQueryNewPalette(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmMenuChar(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmCommand(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmMenuSelect(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmVScroll(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmHScroll(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    LRESULT WmCompareItem(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmMeasureItem(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmDrawItem(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmCtlColor(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmVKeyToItem(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmCharToItem(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmDeleteItem(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    LRESULT WmKeyDown(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmKeyUp(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmMouseMove(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmMouseDown(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmMouseDblClicked(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT WmMouseUp(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    //////////////// SHAPE STUFF ////////////////////////
    PWICreateFN GetShapeCreateAndAddToList(LPSTR szDllName, TYPEID tiShape);  //Will load the dll if it is not there
    void FreeShapeLibrarys(); //Will free all dlls
    BOOL GetShapeLibraryName(TYPEID tiShape, LPSTR szName/*must be 13 chars long*/);  //Returns TRUE if type is found
    OShapeAdmin GetShapeAdmin(TYPEID tiShape);
  };


class WINDOW
  {
  private:
    int                   m_version;                        //version #
    OBJECTID              m_oiSelf;
    HWND                  m_hwnd;                           //handle of the window
    char                  m_szTitle[I_TITLE_LENGTH+1];      //title bar text
    char                  m_accelKeysUsed[256];             //contains the modifier bits for the key -- 0 if key is not accelerated
    LONG                  m_style;                          //window style WF_?????
    int                   m_nInitialCmdShow;                //initial window state....SW_RESTORE, SW_SHOWMAXIMIXED or SW_SHOWMINIMIZED
    RECT                  m_rclInitialClient;               //initial client rectangle in LOGICAL points
    RECT                  m_rclCurrentClient;               //current client rectangle in LOGICAL points
    RECT                  m_rcdCurrentWindow;               //current window rectangle in DEVICE points
    BOOL                  m_bLimitMaxSize;                  //TRUE if theres a max size limit
    POINT                 m_ptlMaxSize;                     //Maximum size of the window in LOGICAL points
    POINT                 m_ptlMinSize;                     //Miminum size of the window int LOGICAL points
    POINT                 m_ptlScrollSize;                  //Size of the scroll area in LOGICAL points
    int                   m_nItems;                         //Number of window items
    OpWndItemD*           m_wiActive;                       //The active (in the window) item
    OpWndItemD*           m_wiDefault;                      //The default window item
    OpWndItemD*           m_wiCancel;                       //The cancel window item
    HPALETTE              m_hPalette;
    short                 unused;
    BOOL                  m_bDefaultOn;                     //TRUE if the default item will get the 'Enter' key
    BOOL                  m_bCancelOn;                      //TRUE if the cancel item will get the 'Esc' key
    short                 m_fState;                         //Window state flag -- I_WSF_CLOSING | I_WSF_OPENING | I_WSF_ICONIC | I_WSF_MENUDOWN
    OBJECTID              m_oiMenu;                         //OBJECTID of the menu bar
    HRGN                  m_hrgnErase;
    COLORREF              m_clrBackground, m_clrForeground;
    COLORREF              m_bwBackground, m_bwForeground;
    short                 m_aPattern[8];
    HBRUSH                m_hbrushBackground;

    int                   m_mappingMode;
    POINT                 m_ptViewportExt;
    POINT                 m_ptWindowExt;
    POINT                 m_ptViewportOrg;
    POINT                 m_ptWindowOrg;

    long                  m_uidNext;                        //Initial 100.  Each time an object is add +=10.  mousein=100, mouseout=101, ?????=102....
    MEMBLOCKID            m_miWindItems;
    OpWndItemD**          m_aptrWndItem;

    COLORREF              m_background, m_foreground;       //The colors to use. If defaults gotten from control panel. BW/color looked at

    BYTE                  m_bVisualInstancesCreated;

    MEMBLOCKID            m_miMenuAccelKeys;

  public:

    WINDOW(OBJECTID id);
    LPVOID operator new(size_t size, OBJECTID id);
    void operator delete(LPVOID ptr);

    inline int GetVersion() { return m_version;}
    inline OBJECTID GetObjectID() { return m_oiSelf; }
    inline LPSTR GetTitle() {return m_szTitle;}
    int GetTitle(LPSTR buffer, int len);
    void ValidateTitle();
    void SetTitle(LPCSTR newTitle);

    inline int GetNumberOfItems()     { return m_nItems; }
    inline LONG GetStyle()            { return m_style; }
    inline void SetStyle(long style)  { m_style = style; }

    inline BOOL HasTitleBar()     { return m_style & I_WF_TITLEBAR ? TRUE:FALSE; }
    inline BOOL HasSysMenu()      { return m_style & I_WF_SYSMENU ? TRUE:FALSE; }
    inline BOOL HasMaxButton()    { return m_style & I_WF_MAXBUTTON ? TRUE:FALSE; }
    inline BOOL HasMinButton()    { return m_style & I_WF_MINBUTTON ? TRUE:FALSE; }
    inline BOOL HasDialogFrame()  { return m_style & I_WF_DIALOGFRAME ? TRUE:FALSE; }
    inline BOOL HasHorzScroll()   { return (!HasDialogFrame() && (m_style & I_WF_HORZSCROLL)) ? TRUE:FALSE; }
    inline BOOL HasVertScroll()   { return (!HasDialogFrame() && (m_style & I_WF_VERTSCROLL)) ? TRUE:FALSE; }
    inline BOOL HasResizeFrame()  { return (!HasDialogFrame() && (m_style & I_WF_RESIZE)) ? TRUE:FALSE; }
    inline BOOL IsModal()         { return m_style & I_WF_MODAL ? TRUE:FALSE; }
    inline BOOL IsPopup()         { return m_style & I_WF_POPUP ? TRUE:FALSE; }
    inline BOOL IsInitVisible()   { return m_style & I_WF_INITVISIBLE ? TRUE:FALSE; }
    inline BOOL DrawsSmooth()     { return m_style & I_WF_DRAWSMOOTH ? TRUE:FALSE; }
    inline BOOL QuitsOnClose()      { return m_style & I_WF_QUIT_ON_CLOSE ? TRUE:FALSE; }

    inline void SetHasTitleBar(BOOL set)    { if (set) m_style |= I_WF_TITLEBAR;    else m_style &= ~I_WF_TITLEBAR; }
    inline void SetHasSysMenu(BOOL set)     { if (set) m_style |= I_WF_SYSMENU;     else m_style &= ~I_WF_SYSMENU; }
    inline void SetHasMaxButton(BOOL set)   { if (set) m_style |= I_WF_MAXBUTTON;   else m_style &= ~I_WF_MAXBUTTON; }
    inline void SetHasMinButton(BOOL set)   { if (set) m_style |= I_WF_MINBUTTON;   else m_style &= ~I_WF_MINBUTTON; }
    inline void SetHasHorzScroll(BOOL set)  { if (set) m_style |= I_WF_HORZSCROLL;  else m_style &= ~I_WF_HORZSCROLL; }
    inline void SetHasVertScroll(BOOL set)  { if (set) m_style |= I_WF_VERTSCROLL;  else m_style &= ~I_WF_VERTSCROLL; }
    inline void SetHasResizeFrame(BOOL set) { if (set) m_style |= I_WF_RESIZE;      else m_style &= ~I_WF_RESIZE; }
    inline void SetHasDialogFrame(BOOL set) { if (set) m_style |= I_WF_DIALOGFRAME; else m_style &= ~I_WF_DIALOGFRAME; }
    inline void SetIsModal(BOOL set)        { if (set) m_style |= I_WF_MODAL;       else m_style &= ~I_WF_MODAL; }
    inline void SetIsPopup(BOOL set)        { if (set) m_style |= I_WF_POPUP;       else m_style &= ~I_WF_POPUP; }
    inline void SetIsInitVisible(BOOL set)  { if (set) m_style |= I_WF_INITVISIBLE; else m_style &= ~I_WF_INITVISIBLE; }
    inline void SetDrawsSmooth(BOOL set)    { if (set) m_style |= I_WF_DRAWSMOOTH;  else m_style &= ~I_WF_DRAWSMOOTH; }
    inline void SetQuitsOnClose(BOOL set)   { if (set) m_style |= I_WF_QUIT_ON_CLOSE; else m_style &= ~I_WF_QUIT_ON_CLOSE; }

    inline BOOL WindowIsOpening()             {return m_fState & I_WSF_OPENING ? TRUE : FALSE;}
    inline BOOL WindowIsClosing()             {return m_fState & I_WSF_CLOSING ? TRUE : FALSE;}
    inline BOOL WindowIsIconic()              {return m_fState & I_WSF_ICONIC  ? TRUE : FALSE;}
    inline BOOL HScrollVisible()              {return m_fState & I_WSF_HSCROLL ? TRUE : FALSE;}
    inline BOOL VScrollVisible()              {return m_fState & I_WSF_VSCROLL ? TRUE : FALSE;}
    inline BOOL MenuIsDown(void)              {return m_fState & I_WSF_MENUDOWN ? TRUE : FALSE;}
    inline void SetWindowIsOpening(BOOL set)  {if (set) m_fState |= I_WSF_OPENING; else m_fState &= ~I_WSF_OPENING;}
    inline void SetWindowIsClosing(BOOL set)  {if (set) m_fState |= I_WSF_CLOSING; else m_fState &= ~I_WSF_CLOSING;}
    inline void SetWindowIsIconic(BOOL set)   {if (set) m_fState |= I_WSF_ICONIC;  else m_fState &= ~I_WSF_ICONIC;}
    inline void SetHScrollVisible(BOOL set)   {if (set) m_fState |= I_WSF_HSCROLL; else m_fState &= ~I_WSF_HSCROLL;}
    inline void SetVScrollVisible(BOOL set)   {if (set) m_fState |= I_WSF_VSCROLL; else m_fState &= ~I_WSF_VSCROLL;}
    inline void SetMenuDown(BOOL set)         {if (set) m_fState |= I_WSF_MENUDOWN; else m_fState &= ~I_WSF_MENUDOWN;}

    inline HWND GetWindowHandle() {return m_hwnd;}
    inline void SetWindowHandle(HWND hwnd)  { m_hwnd = hwnd; }
    inline BOOL IsWindowOpen() {return (m_hwnd != 0);}
    void BringToTop();

    inline RECT GetInitialClientRect() {return m_rclInitialClient;}
    inline void SetInitialClientRect(int left, int top, int right, int bottom) {SetRect(&m_rclInitialClient, left, top, right, bottom);}
    inline void SetInitialClientRect(const LPRECT rect) {CopyRect(&m_rclInitialClient, rect);}

    inline int GetInitialWindowState()  {return m_nInitialCmdShow;}
    inline void SetInitialWindowState(int nCmdShow) {m_nInitialCmdShow = nCmdShow;}

    inline RECT GetCurrentWindowRect() {return m_rcdCurrentWindow;}
    inline void SetCurrentWindowRect(int left, int top, int right, int bottom) {SetRect(&m_rcdCurrentWindow, left, top, right, bottom);}
    inline void SetCurrentWindowRect(const LPRECT rect) {CopyRect(&m_rcdCurrentWindow, rect);}

    inline RECT GetCurrentClientRect() {return m_rclCurrentClient;}
    inline void SetCurrentClientRect(int left, int top, int right, int bottom) {SetRect(&m_rclCurrentClient, left, top, right, bottom);}
    inline void SetCurrentClientRect(const LPRECT rect) {CopyRect(&m_rclCurrentClient, rect);}

    inline BOOL PtInWindow(POINT ptd) {return PtInRect(&m_rcdCurrentWindow, ptd);}
    inline BOOL PtInClient(POINT ptl) {return PtInRect(&m_rclCurrentClient, ptl);}

    inline BOOL HasMaxSize() {return m_bLimitMaxSize;}
    inline void SetHasMaxSize(BOOL set) {m_bLimitMaxSize = set;}

    inline POINT GetMaxSize() {return m_ptlMaxSize;}
    inline void SetMaxSize(POINT pt) {m_ptlMaxSize = pt;}

    inline POINT GetMinSize() {return m_ptlMinSize;}
    inline void SetMinSize(POINT pt) {m_ptlMinSize = pt;}

    inline POINT GetScrollSize() {return m_ptlScrollSize;}
    inline void SetScrollSize(POINT pt) {m_ptlScrollSize = pt;}

    OBJECTID inline GetMenuObject() {return m_oiMenu;}
    inline void AssignMenuObject(OBJECTID oi) {m_oiMenu = oi;}
    inline BOOL HasMenuBar() {return (!HasDialogFrame() && m_oiMenu) ? TRUE:FALSE;}
    BOOL SetMenuHandle(); //uses m_oiMenu--returns true for sucess--releases current hmenu if it exists
    BOOL ReleaseMenuHandle(); //Release the hmenu associtated with m_hwnd if one exists
    BOOL RedrawMenu();

    void GetColorsAndPattern(COLORREF far * pclrCBackground, COLORREF far * pclrCForeground,
                             COLORREF far * pclrBWBackground, COLORREF far * pclrBWForeground,
                             short* ptrPattern,  UINT cbSize);
    void SetColorsAndPattern(COLORREF clrCBackground, COLORREF clrCForeground,
                             COLORREF clrBWBackground, COLORREF clrBWForeground,
                             const short* ptrPattern,  UINT cbSize);   
    void CreateBackgroundBrushAndColors(void);
    void FreeBackgroundBrushAndColors(void);
    

    BOOL SendAccelKey(OpWndItemD* wi,TYPEID type, char vKey, char modifier);
    BOOL SetItemAccelKey(OpWndItemD* wi, char vKey, char modifier);
    BOOL SetMenuAccelKey(short menuID, char vKey, char modifier);
    void RebuildAccelTable(char vKey);

    OpWndItemD* GetActiveItem() {return m_wiActive;}   //the active for the window
    OpWndItemD* SetActiveItem(OpWndItemD* wi, int causedBy, BOOL bForward=TRUE);
    OpWndItemD* ActivateItem(OpWndItemD* wi, int causedBy, BOOL bForward=TRUE);
    void ActivateItemCauseWindowWas();
    void DeactivateItemCauseWindowWas();

    OpWndItemD* GetDefaultItem() {return m_wiDefault;}
    void SetDefaultItem(OpWndItemD* wi) {m_wiDefault=wi;}

    OpWndItemD* GetCancelItem() {return m_wiCancel;}
    void SetCancelItem(OpWndItemD* wi) {m_wiCancel=wi;}

    OpHwndWndItemD* GetItemByControlID(int controlID);
    OpWndItemD* GetItemByObjectID(OBJECTID id);
    OpWndItemD* GetItemByUID(LONG uid);
    OpWndItemD* GetNextItem(OpWndItemD* wi, BOOL wrap);
    OpWndItemD* GetPrevItem(OpWndItemD* wi, BOOL wrap);
    OpWndItemD* GetNextItemWithinGroup(OpWndItemD* wi);//wraps within group
    OpWndItemD* GetPrevItemWithinGroup(OpWndItemD* wi);//wraps within group

    OpWndItemD* GetNextMatchingItem(OpWndItemD* wi, LONG statusBits); //wraps
    OpWndItemD* GetPrevMatchingItem(OpWndItemD* wi, LONG statusBits); //wraps
    OpWndItemD* GetNextMatchingItemWithinGroup(OpWndItemD* wi, LONG statusBits); //wraps within group
    OpWndItemD* GetPrevMatchingItemWithinGroup(OpWndItemD* wi, LONG statusBits); //wraps within group

    OpWndItemD* GetNextActivatableItem(OpWndItemD* wi=NULL) {if (!wi) wi=m_wiActive; return GetNextMatchingItem(wi, I_SF_ACTIVATABLE|I_SF_ENABLED|I_SF_VISIBLE);}
    OpWndItemD* GetPrevActivatableItem(OpWndItemD* wi=NULL) {if (!wi) wi=m_wiActive; return GetPrevMatchingItem(wi, I_SF_ACTIVATABLE|I_SF_ENABLED|I_SF_VISIBLE);}
    OpWndItemD* GetNextActivatableItemWithinGroup(OpWndItemD* wi=NULL)  {if (!wi) wi=m_wiActive; return GetNextMatchingItemWithinGroup(wi, I_SF_ACTIVATABLE|I_SF_ENABLED|I_SF_VISIBLE);}
    OpWndItemD* GetPrevActivatableItemWithinGroup(OpWndItemD* wi=NULL)  {if (!wi) wi=m_wiActive; return GetPrevMatchingItemWithinGroup(wi, I_SF_ACTIVATABLE|I_SF_ENABLED|I_SF_VISIBLE);}
    OpWndItemD* GetNextTabstop(OpWndItemD* wi=NULL) {if (!wi) wi=m_wiActive; return GetNextMatchingItem(wi, I_SF_TABSTOP|I_SF_ENABLED|I_SF_ACTIVATABLE|I_SF_VISIBLE);}
    OpWndItemD* GetPrevTabstop(OpWndItemD* wi=NULL) {if (!wi) wi=m_wiActive; return GetPrevMatchingItem(wi, I_SF_TABSTOP|I_SF_ENABLED|I_SF_ACTIVATABLE|I_SF_VISIBLE);}

    void Startup(OBJECTID oiWindow);
    BOOL MakeSureTheVisualInstancesAreCreated(void);
    inline BOOL AreTheVisualInstancesCreated(void) {return m_bVisualInstancesCreated ? TRUE:FALSE;}
    void Shutdown();

    long GetNativeStyle();
    long GetNativeExStyle();
    void RemoveTitleBar();

    void CreateTheWindow();
    static void CalculateMappingModeExtents(int mode, LPPOINT windowExt, LPPOINT viewportExt);
    void AssignMappingMode(int newMode);    //OWNDLAY_MM_DEPENDANT or OWNDLAY_MM_INDEPENDANT
    void SetMappingMode(HDC hdc);               
    void WndDPtoLP(LPPOINT pt, int numPoints);
    void WndLPtoDP(LPPOINT pt, int numPoints);
    void WndDStoLS(LPPOINT pt, int numPoints);
    void WndLStoDS(LPPOINT pt, int numPoints);

    void Open();
    void Close();
    void Hide();

    OpWndItemD* ItemFromLogicalPoint(POINT pt); //pt is in client local coords
    OpWndItemD* ItemFromScreenPoint(POINT pt);  //pt is in screen coords

    BOOL ShouldEnterKeyBeStolen() { return (m_wiDefault && m_bDefaultOn && WINDOWAPI::IsItemEnabled(m_wiDefault)  && WINDOWAPI::IsItemVisible(m_wiDefault));}
    BOOL ShouldEscKeyBeStolen() { return (m_wiCancel && m_bCancelOn && WINDOWAPI::IsItemEnabled(m_wiCancel)  && WINDOWAPI::IsItemVisible(m_wiCancel));}
    BOOL ShouldTabKeyBeStolen() { return (!m_wiActive || !WINDOWAPI::DoesItemUseTab(m_wiActive));}
    BOOL ShouldCursorKeysBeStolen() { return (!MenuIsDown() && (!m_wiActive || !WINDOWAPI::DoesItemUseCursorKeys(m_wiActive)));}

    BOOL Erase(HDC hdc);
    void Paint();
    void PaintIcon();
    HICON GetIcon();

    void ResizeWindowItems(const LPRECT rclNewSize);

    void SetEraseRegion();
    void RemoveEraseRegion();

    BOOL m_GetViewportOrgEx(LPPOINT lppt)   {*lppt = m_ptViewportOrg; return TRUE;}
    BOOL m_GetWindowOrgEx(LPPOINT lppt)     {*lppt = m_ptWindowOrg; return TRUE;}
    BOOL m_SetViewportOrgEx(int xOrigin, int yOrigin, LPPOINT lppt);
    BOOL m_SetWindowOrgEx(int nX, int nY, LPPOINT lppt);
    void RestorePortOrigins();
    void ValidateScrollBars();
    void DoWindowHScroll(WPARAM wParam, LPARAM lParam);
    void DoWindowVScroll(WPARAM wParam, LPARAM lParam);

    void Center(OBJECTID oiCenterIn);

    void MoveItemToBack(OpWndItemD* wi);
    void MoveItemToFront(OpWndItemD* wi);
    void ReorderItem(OpWndItemD* wi, int nPosition);

    void SetItemParent(OpWndItemD* wi, HWND hwndParent);
    void SetAllParents(HWND hwndParent);

    long ValidateObjectIDs(OBJECTID oiNew, pAObjMessage theSystem);
    long CheckObject();
    long Read();
    long BuildSignals(MEMBLOCKID miSignal);
    void RemoveItemDataBlock(int itemNumber);

    int CreateExistingItems(MEMBLOCKID miArray);  //Creates all items in datas.  Resizes the array. return number created
    OpWndItemD* CreateNewItem(OBJECTID oiItem, PWICreateFN createFunction, LPRECT lprcItem, MEMBLOCKID miExtra, LPSTR szShapeDllName);
    OpWndItemD* RecreateExistingItem(OpWndItemD* wiIn);

    inline BOOL PutItemsIntoDatas(){return PutItemsIntoDatas(m_miWindItems, m_nItems);}
    BOOL PutItemsIntoDatas(MEMBLOCKID miArray, int numItems);
    static BOOL DestroyAllItems(MEMBLOCKID miArray, int numItems);//Destroys all items in the array and resizes the array to 0.
    
    long Export(void);
    long Import(void);

    BOOL Print(LPPIDR_BLOCK pidr); 

    BOOL CreatePaletteHandle(void);
    BOOL DeletePaletteHandle(void);
    inline HPALETTE GetPaletteHandle(void) {return m_hPalette;}

    #ifdef ADBG_DEBUG
    static  WINDOW* LockWindowData(OBJECTID id);
    static  void    UnlockWindowData(OBJECTID id);
    #endif
  };

#ifdef ADBG_DEBUG
struct OWndRuntimeInfo
  {
  short   nLockCount;
  WINDOW* pWnd;
  };

inline WINDOW* LockWindowData
  (
  OBJECTID  oi
  )

  {
  return WINDOW::LockWindowData(oi);
  }

inline void UnlockWindowData
  (
  OBJECTID  oi
  )
  
  {
  WINDOW::UnlockWindowData(oi);
  }

#else
inline WINDOW* LockWindowData
  (
  OBJECTID  oi
  )

  {
  return ((WINDOW*)AObjLockData(oi, I_DATA_WINDOW));
  }

inline void UnlockWindowData
  (
  OBJECTID  oi
  )
  
  {
  AObjUnlockData(oi, I_DATA_WINDOW);
  }
#endif //#ifdef ADBG_DEBUG



#endif



