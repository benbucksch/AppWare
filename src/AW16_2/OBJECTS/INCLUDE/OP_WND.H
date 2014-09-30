/* ****************************************************************
 *
 *  Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * **************************************************************** */

#ifndef OP_WND_H
#define OP_WND_H

/* ****************************************************************
 * PROTOCOLID for Window Item Protocol
 * **************************************************************** */

#define APROT_WND                     MAKEPROTOCOLID('P','w','i','n')

/* ****************************************************************
 * Typedefs and constant definitions
 * **************************************************************** */

/* Grab handle enumeration (these must be consecutive) */
#define OWND_POINT_IN_NONE            0
#define OWND_POINT_IN_TL              1
#define OWND_POINT_IN_TR              2
#define OWND_POINT_IN_BL              3
#define OWND_POINT_IN_BR              4
#define OWND_POINT_IN_T               5
#define OWND_POINT_IN_L               6
#define OWND_POINT_IN_B               7
#define OWND_POINT_IN_R               8

/* AdjustNewRect information */
#define OWND_ADJRECT_ANCHOR_T         0x00000001L
#define OWND_ADJRECT_ANCHOR_L         0x00000002L
#define OWND_ADJRECT_ANCHOR_B         0x00000004L
#define OWND_ADJRECT_ANCHOR_R         0x00000008L
#define OWND_ADJRECT_DEFAULT_W        0x00000010L
#define OWND_ADJRECT_DEFAULT_H        0x00000020L

/* AdjustNewRect return flags */
#define OWND_ADJRECT_SWAPPED_TB       0x00000001L
#define OWND_ADJRECT_SWAPPED_LR       0x00000002L
#define OWND_ADJRECT_CHANGED_T        0x00000010L
#define OWND_ADJRECT_CHANGED_L        0x00000020L
#define OWND_ADJRECT_CHANGED_B        0x00000040L
#define OWND_ADJRECT_CHANGED_R        0x00000080L
#define OWND_ADJRECT_NODEFAULT_W      0x00000100L
#define OWND_ADJRECT_NODEFAULT_H      0x00000200L

/* Activation information */
#define OWND_ACTIVATION_KEPT          1
#define OWND_ACTIVATION_PASS          2

#define OWND_ACTIVATED_BY_WINDOW      1
#define OWND_ACTIVATED_BY_MOUSE       2
#define OWND_ACTIVATED_BY_TAB         3
#define OWND_ACTIVATED_BY_CURSORKEYS  4
#define OWND_ACTIVATED_BY_OTHER       5

/* Position notification */
#define OWND_LOC_HSCROLL              1
#define OWND_LOC_VSCROLL              2
#define OWND_LOC_MOVED                3

/* ****************************************************************
 * ****************************************************************
 * Protocol definitions
 * ****************************************************************
 * **************************************************************** */

/* Forward protocol definitions */
APROT_DECLAREPROTOCOL(OpWndItemD);
APROT_DECLAREPROTOCOL(OpLiteWndItemD);
APROT_DECLAREPROTOCOL(OpLiteWndItem);
APROT_DECLAREPROTOCOL(OpHwndWndItemD);
APROT_DECLAREPROTOCOL(OpHwndWndItem);
APROT_DECLAREPROTOCOL(WINDOW);
APROT_DECLAREPROTOCOL(WINDOWRUNTIME);
APROT_DECLAREPROTOCOL(WINDOWAPI);
APROT_DECLAREPROTOCOL(WNDITEM_DATA);
APROT_DECLAREPROTOCOL(Items)
APROT_DECLAREPROTOCOL(OpGroupWndItem)

/* forward function definitions */
#ifdef __cplusplus
extern "C"
  {
  void ALMAPI OpGwiCreated(OpGroupWndItem far*, OpWndItemD far* far*, short);
  void ALMAPI OpGwiUngrouped(OpGroupWndItem far*);
  short ALMAPI OpGwiItemAdded(OpGroupWndItem far*, OpWndItemD far*);
  }
#endif

/* ****************************************************************
 * Base window item protocol
 * **************************************************************** */

APROT_BEGINPROTOCOL(OpWndItemD, ApObjectD)

  APROT_BEGINSERVICES(OpWndItemD, ApObjectD)

  APROT_SERVICE(LONG, Edit,          (APROT_THIS(OpWndItemD) BOOL bEditExtra))
  APROT_SERVICE(void, Cut,           (APROT_VOID(OpWndItemD) ))
  APROT_SERVICE(void, Copy,          (APROT_VOID(OpWndItemD) ))
  APROT_SERVICE(void, Paste,         (APROT_VOID(OpWndItemD) ))
  APROT_SERVICE(void, Undo,          (APROT_VOID(OpWndItemD) ))
  APROT_SERVICE(void, DataChanged,   (APROT_THIS(OpWndItemD) long details,
                                      long modifiers, long variation))
  APROT_SERVICE(void, ParentOpened,  (APROT_VOID(OpWndItemD) ))
  APROT_SERVICE(void, ParentClosing, (APROT_VOID(OpWndItemD) ))
  APROT_SERVICE(void, Print,         (APROT_THIS(OpWndItemD) HDC hdc,
                                      LPRECT printRect, int pclPrinter))
  APROT_SERVICE(void, AccelKeyHit,   (APROT_VOID(OpWndItemD) ))
  APROT_SERVICE(BOOL, GetSelection,  (APROT_THIS(OpWndItemD) long far* start,
                                      long far* count))
  APROT_SERVICE(BOOL, SetSelection,  (APROT_THIS(OpWndItemD) long start,
                                      long count))
  APROT_SERVICE(void, SetDefaultItem,(APROT_THIS(OpWndItemD) BOOL set))
  APROT_SERVICE(void, MouseEntry,    (APROT_THIS(OpWndItemD) BOOL inside))
  APROT_SERVICE(long, Activate,      (APROT_THIS(OpWndItemD) int causedBy,
                                      BOOL mustKeep))
  APROT_SERVICE(void, Deactivated,   (APROT_VOID(OpWndItemD) ))
  APROT_SERVICE(void, Show,          (APROT_VOID(OpWndItemD) ))
  APROT_SERVICE(void, Hide,          (APROT_VOID(OpWndItemD) ))
  APROT_SERVICE(void, Move,          (APROT_THIS(OpWndItemD) LPRECT newRect))
  APROT_SERVICE(void, Enable,        (APROT_THIS(OpWndItemD) BOOL bEnable))
  APROT_SERVICE(void, ScreenLocationChanged, (APROT_THIS(OpWndItemD) int causedBy))
  APROT_SERVICE(long, AdjustNewRect, (APROT_THIS(OpWndItemD) LPRECT newRect,
                                      LONG adjustFlag))
  APROT_SERVICE(void, InvalidateItem, (APROT_THIS(OpWndItemD) BOOL bErase))
  APROT_SERVICE(BOOL, PointInItem,   (APROT_THIS(OpWndItemD) LPPOINT pt))
  APROT_SERVICE(BOOL, ItemInRect,    (APROT_THIS(OpWndItemD) LPRECT lprc,
                                      BOOL fReqFullEnclosure))
  APROT_SERVICE(BOOL, ItemInRegion,  (APROT_THIS(OpWndItemD) HRGN hRegion,
                                      BOOL bDeviceUnits))
  APROT_SERVICE(void, RemoveItemFromRegion,  (APROT_THIS(OpWndItemD) HRGN hRegion,
                                              BOOL bDeviceUnits))
  APROT_SERVICE(void, DrawTrackingRect,      (APROT_THIS(OpWndItemD) HDC hdc,
                                              LPRECT rect))
  APROT_SERVICE(void, DrawGrabHandles,       (APROT_THIS(OpWndItemD) HDC hdc))
  APROT_SERVICE(void, InvalidateGrabHandles, (APROT_THIS(OpWndItemD) BOOL bErase))
  APROT_SERVICE(long, PointInGrabHandle,     (APROT_THIS(OpWndItemD) LPPOINT pt,
                                              HCURSOR far* phCursor))
  APROT_SERVICE(MEMBLOCKID, GetExtraData,    (APROT_VOID(OpWndItemD) ))
  APROT_SERVICE(void, ExportExtraData,    (APROT_VOID(OpWndItemD) ))
  APROT_SERVICE(void, ImportExtraData,    (APROT_VOID(OpWndItemD) ))

  APROT_ENDSERVICES(OpWndItemD, ApObjectD)

  /* Data fields */
  APROT_PUBLIC
    RECT                m_rcItem;
    OBJECTID            m_oiParent;
    HWND                m_hwndParent;
  APROT_PRIVATE
    long                m_status;
    int                 m_grow;
    long                m_uidItem;
    char                m_accelVKey;
    char                m_accelModifier;
    int                 m_index;

#ifdef __cplusplus
  friend void localInit(OpWndItemD far* wi, OBJECTID oiItem, OBJECTID oiParent, LPRECT rcItem, long status);
  friend OBJECTID localObject(OpWndItemD far* wi);
  friend void ALMAPI OpGwiCreated(OpGroupWndItem far*, OpWndItemD far* far*, short);
  friend void ALMAPI OpGwiUngrouped(OpGroupWndItem far*);
  friend short ALMAPI OpGwiItemAdded(OpGroupWndItem far*, OpWndItemD far*);
  friend WINDOW;
  friend WINDOWRUNTIME;
  friend WINDOWAPI;
  friend WNDITEM_DATA;
  friend Items;
  friend OpGroupWndItem;
#endif

APROT_ENDPROTOCOL(OpWndItemD, ApObjectD);

typedef OpWndItemD far* (WINAPI* PWICreateFN)(OBJECTID oiItem, OBJECTID oiParent,
                                              LPRECT rcItem, long status,
                                              MEMBLOCKID extraData);

/* ****************************************************************
 * HWND Window item protocol definition
 * **************************************************************** */

APROT_BEGINPROTOCOL(OpHwndWndItemD, OpWndItemD)

  APROT_BEGINSERVICES(OpHwndWndItemD, OpWndItemD)
  APROT_SERVICE(LRESULT, ParentNotification, (APROT_THIS(OpHwndWndItemD) HWND hwnd,
                                              UINT message, WPARAM wParam, LPARAM lParam))
  APROT_ENDSERVICES(OpHwndWndItemD, OpWndItemD)

  /* Data fields */
  APROT_PUBLIC
    HWND                m_hwndItem;

APROT_ENDPROTOCOL(OpHwndWndItemD, OpWndItemD);

/* ****************************************************************
 * Lite Window item protocol definition
 * **************************************************************** */

APROT_BEGINPROTOCOL(OpLiteWndItemD, OpWndItemD)

  APROT_BEGINSERVICES(OpLiteWndItemD, OpWndItemD)

  APROT_SERVICE(void, Draw,         (APROT_THIS(OpLiteWndItemD) HDC hdc,
                                      LPRECT invalidRect))
  APROT_SERVICE(void, KeyDown,      (APROT_THIS(OpLiteWndItemD) int vKey,
                                      char ascii, long keyData))
  APROT_SERVICE(void, KeyUp,        (APROT_THIS(OpLiteWndItemD) int vKey,
                                      char ascii, long keyData))
  APROT_SERVICE(void, MouseMove,    (APROT_THIS(OpLiteWndItemD) LPPOINT where,
                                      int keysDown))
  APROT_SERVICE(void, MouseDown,    (APROT_THIS(OpLiteWndItemD) LPPOINT where,
                                      int keysDown, int button))
  APROT_SERVICE(void, DoubleClicked,(APROT_THIS(OpLiteWndItemD) LPPOINT where,
                                      int keysDown, int button))
  APROT_SERVICE(void, MouseUp,      (APROT_THIS(OpLiteWndItemD) LPPOINT where,
                                      int keysDown, int button))
  APROT_SERVICE(void, CancelMode,   (APROT_VOID(OpWndItemD) ))

  APROT_ENDSERVICES(OpLiteWndItemD, OpWndItemD)

APROT_ENDPROTOCOL(OpLiteWndItemD, OpWndItemD);


/* ****************************************************************
 * ****************************************************************
 * Default implementations for above protocol definitions
 * ****************************************************************
 * **************************************************************** */

/* 'C' APIs for default implementation */
APROT_BEGINAPI

void     ALMAPI OpLwiInit(OpLiteWndItem far* lwi, OBJECTID oiItem, OBJECTID oiParent, LPRECT rcItem, long status);
void     ALMAPI OpHwiInit(OpHwndWndItem far* hwi, OBJECTID oiItem, OBJECTID oiParent, LPRECT rcItem, long status);
void     ALMAPI OpLwiDestroy(OpLiteWndItem far* lwi);
void     ALMAPI OpHwiDestroy(OpHwndWndItem far* hwi);
long     ALMAPI OpLwiQueryProtocol(OpLiteWndItem far* lwi, ApQueryInfo far* qpi, PROTOCOLID pid);
long     ALMAPI OpHwiQueryProtocol(OpHwndWndItem far* hwi, ApQueryInfo far* qpi, PROTOCOLID pid);
OBJECTID ALMAPI OpLwiObject(OpLiteWndItem far* lwi);
OBJECTID ALMAPI OpHwiObject(OpHwndWndItem far* hwi);
TYPEID   ALMAPI OpLwiType(OpLiteWndItem far* lwi);
TYPEID   ALMAPI OpHwiType(OpHwndWndItem far* hwi);
LONG     ALMAPI OpLwiEdit(OpLiteWndItem far* lwi, BOOL bEditExtra);
LONG     ALMAPI OpHwiEdit(OpHwndWndItem far* hwi, BOOL bEditExtra);
void     ALMAPI OpLwiCut(OpLiteWndItem far* lwi);
void     ALMAPI OpHwiCut(OpHwndWndItem far* hwi);
void     ALMAPI OpLwiCopy(OpLiteWndItem far* lwi);
void     ALMAPI OpHwiCopy(OpHwndWndItem far* hwi);
void     ALMAPI OpLwiPaste(OpLiteWndItem far* lwi);
void     ALMAPI OpHwiPaste(OpHwndWndItem far* hwi);
void     ALMAPI OpLwiUndo(OpLiteWndItem far* lwi);
void     ALMAPI OpHwiUndo(OpHwndWndItem far* hwi);
void     ALMAPI OpLwiDataChanged(OpLiteWndItem far* lwi, long details, long modifiers, long variation);
void     ALMAPI OpHwiDataChanged(OpHwndWndItem far* hwi, long details, long modifiers, long variation);
void     ALMAPI OpLwiParentOpened(OpLiteWndItem far* lwi);
void     ALMAPI OpHwiParentOpened(OpHwndWndItem far* hwi);
void     ALMAPI OpLwiParentClosing(OpLiteWndItem far* lwi);
void     ALMAPI OpHwiParentClosing(OpHwndWndItem far* hwi);
void     ALMAPI OpLwiPrint(OpLiteWndItem far* lwi, HDC hdc, LPRECT rect, int pclPrinter);
void     ALMAPI OpHwiPrint(OpHwndWndItem far* hwi, HDC hdc, LPRECT rect, int pclPrinter);
void     ALMAPI OpLwiAccelKeyHit(OpLiteWndItem far* lwi);
void     ALMAPI OpHwiAccelKeyHit(OpHwndWndItem far* hwi);
BOOL     ALMAPI OpLwiGetSelection(OpLiteWndItem far* lwi, long far* start, long far* count);
BOOL     ALMAPI OpHwiGetSelection(OpHwndWndItem far* hwi, long far* start, long far* count);
BOOL     ALMAPI OpLwiSetSelection(OpLiteWndItem far* lwi, long start, long count);
BOOL     ALMAPI OpHwiSetSelection(OpHwndWndItem far* hwi, long start, long count);
void     ALMAPI OpLwiSetDefaultItem(OpLiteWndItem far* lwi, BOOL set);
void     ALMAPI OpHwiSetDefaultItem(OpHwndWndItem far* hwi, BOOL set);
void     ALMAPI OpLwiMouseEntry(OpLiteWndItem far* lwi, BOOL inside);
void     ALMAPI OpHwiMouseEntry(OpHwndWndItem far* hwi, BOOL inside);
long     ALMAPI OpLwiActivate(OpLiteWndItem far* lwi, int causedBy, BOOL mustKeep);
long     ALMAPI OpHwiActivate(OpHwndWndItem far* hwi, int causedBy, BOOL mustKeep);
void     ALMAPI OpLwiDeactivated(OpLiteWndItem far* lwi);
void     ALMAPI OpHwiDeactivated(OpHwndWndItem far* hwi);
void     ALMAPI OpLwiShow(OpLiteWndItem far* lwi);
void     ALMAPI OpHwiShow(OpHwndWndItem far* hwi);
void     ALMAPI OpLwiHide(OpLiteWndItem far* lwi);
void     ALMAPI OpHwiHide(OpHwndWndItem far* hwi);
void     ALMAPI OpLwiMove(OpLiteWndItem far* lwi, LPRECT newRect);
void     ALMAPI OpHwiMove(OpHwndWndItem far* hwi, LPRECT newRect);
void     ALMAPI OpLwiEnable(OpLiteWndItem far* lwi, BOOL bEnable);
void     ALMAPI OpHwiEnable(OpHwndWndItem far* hwi, BOOL bEnable);
void     ALMAPI OpLwiScreenLocationChanged(OpLiteWndItem far* lwi, int causedBy);
void     ALMAPI OpHwiScreenLocationChanged(OpHwndWndItem far* lwi, int causedBy);
long     ALMAPI OpLwiAdjustNewRect(OpLiteWndItem far* lwi, LPRECT newRect, LONG adjustFlag);
long     ALMAPI OpHwiAdjustNewRect(OpHwndWndItem far* hwi, LPRECT newRect, LONG adjustFlag);
void     ALMAPI OpLwiInvalidateItem(OpLiteWndItem far* lwi, BOOL bErase);
void     ALMAPI OpHwiInvalidateItem(OpHwndWndItem far* hwi, BOOL bErase);
BOOL     ALMAPI OpLwiPointInItem(OpLiteWndItem far* lwi, LPPOINT pt);
BOOL     ALMAPI OpHwiPointInItem(OpHwndWndItem far* hwi, LPPOINT pt);
BOOL     ALMAPI OpLwiItemInRect(OpLiteWndItem far* lwi, LPRECT rect, BOOL fReqFullEnclosure);
BOOL     ALMAPI OpHwiItemInRect(OpHwndWndItem far* hwi, LPRECT rect, BOOL fReqFullEnclosure);
BOOL     ALMAPI OpLwiItemInRegion(OpLiteWndItem far* lwi, HRGN hRegion, BOOL bDeviceUnits);
BOOL     ALMAPI OpHwiItemInRegion(OpHwndWndItem far* hwi, HRGN hRegion, BOOL bDeviceUnits);
void     ALMAPI OpLwiRemoveItemFromRegion(OpLiteWndItem far* lwi, HRGN hRegion, BOOL bDeviceUnits);
void     ALMAPI OpHwiRemoveItemFromRegion(OpHwndWndItem far* hwi, HRGN hRegion, BOOL bDeviceUnits);
void     ALMAPI OpLwiDrawTrackingRect(OpLiteWndItem far* lwi, HDC hdc, LPRECT rect);
void     ALMAPI OpHwiDrawTrackingRect(OpHwndWndItem far* hwi, HDC hdc, LPRECT rect);
void     ALMAPI OpLwiDrawGrabHandles(OpLiteWndItem far* lwi, HDC hdc);
void     ALMAPI OpHwiDrawGrabHandles(OpHwndWndItem far* hwi, HDC hdc);
void     ALMAPI OpLwiInvalidateGrabHandles(OpLiteWndItem far* lwi, BOOL bErase);
void     ALMAPI OpHwiInvalidateGrabHandles(OpHwndWndItem far* hwi, BOOL bErase);
long     ALMAPI OpLwiPointInGrabHandle(OpLiteWndItem far* lwi, LPPOINT pt, HCURSOR far* phCursor);
long       ALMAPI OpHwiPointInGrabHandle(OpHwndWndItem far* hwi, LPPOINT pt, HCURSOR far* phCursor);
MEMBLOCKID ALMAPI OpLwiGetExtraData(OpLiteWndItem far* lwi);
MEMBLOCKID ALMAPI OpHwiGetExtraData(OpHwndWndItem far* hwi);
void ALMAPI OpLwiExportExtraData(OpLiteWndItem far* lwi);
void ALMAPI OpHwiExportExtraData(OpHwndWndItem far* hwi);
void ALMAPI OpLwiImportExtraData(OpLiteWndItem far* lwi);
void ALMAPI OpHwiImportExtraData(OpHwndWndItem far* hwi);

LRESULT    ALMAPI OpHwiParentNotification(OpHwndWndItem far* hwi, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void ALMAPI OpLwiDraw(OpLiteWndItem far* lwi, HDC hdc, LPRECT invalidRect);
void ALMAPI OpLwiKeyDown(OpLiteWndItem far* lwi, int vKey, char ascii, long keyData);
void ALMAPI OpLwiKeyUp(OpLiteWndItem far* lwi, int vKey, char ascii, long keyData);
void ALMAPI OpLwiMouseMove(OpLiteWndItem far* lwi, LPPOINT where, int keysDown);
void ALMAPI OpLwiMouseDown(OpLiteWndItem far* lwi, LPPOINT where, int keysDown, int button);
void ALMAPI OpLwiDoubleClicked(OpLiteWndItem far* lwi, LPPOINT where, int keysDown, int button);
void ALMAPI OpLwiMouseUp(OpLiteWndItem far* lwi, LPPOINT where, int keysDown, int button);
void ALMAPI OpLwiCancelMode(OpLiteWndItem far* lwi);

APROT_ENDAPI

/* ****************************************************************
 * Re-include ap_macro.h to redefine some macros.
 * **************************************************************** */

#ifdef OWND_DEFAULTIMPLEMENTATION
#define APROT_DEFAULTIMPLEMENTATION
#include <ap_macro.h>
#endif

/* ****************************************************************
 * Default implementation for Lite window item
 * **************************************************************** */

APROT_BEGINPROTOCOLIMP(OpLiteWndItem, OpLiteWndItemD)
#ifdef __cplusplus
  private:
    OpLiteWndItem();
  public:
    OpLiteWndItem(OBJECTID oiItem, OBJECTID oiParent, LPRECT rcItem, long status)
    {
    OpLwiInit(this, oiItem, oiParent, rcItem, status);
    }
#endif

  APROT_BEGINSERVICESIMP(OpLiteWndItem, OpLiteWndItemD)

  APROT_SERVICEIMP(void, Destroy,  (void) )
    APROT_CPP({OpLwiDestroy(this);})

  APROT_SERVICEIMP(long, QueryProtocol,  (ApQueryInfo far* qpi, PROTOCOLID pid) )
    APROT_CPP({return OpLwiQueryProtocol(this, qpi, pid);})

  APROT_SERVICEIMP(OBJECTID, Object,  (void) )
    APROT_CPP({return OpLwiObject(this);})

  APROT_SERVICEIMP(TYPEID, Type,  (void) )
    APROT_CPP({return OpLwiType(this);})

  APROT_SERVICEIMP(LONG, Edit,  (BOOL bEditExtra) )
    APROT_CPP({return OpLwiEdit(this, bEditExtra);})

  APROT_SERVICEIMP(void, Cut,  (void) )
    APROT_CPP({OpLwiCut(this);})

  APROT_SERVICEIMP(void, Copy,  (void) )
    APROT_CPP({OpLwiCopy(this);})

  APROT_SERVICEIMP(void, Paste,  (void) )
    APROT_CPP({OpLwiPaste(this);})

  APROT_SERVICEIMP(void, Undo,  (void) )
    APROT_CPP({OpLwiUndo(this);})

  APROT_SERVICEIMP(void, DataChanged,  (long details, long modifiers, long variation) )
    APROT_CPP({OpLwiDataChanged(this, details, modifiers, variation);})

  APROT_SERVICEIMP(void, ParentOpened,  (void) )
    APROT_CPP({OpLwiParentOpened(this);})

  APROT_SERVICEIMP(void, ParentClosing,  (void) )
    APROT_CPP({OpLwiParentClosing(this);})

  APROT_SERVICEIMP(void, Print,  (HDC hdc, LPRECT printRect, int pclPrinter) )
    APROT_CPP({OpLwiPrint(this, hdc, printRect, pclPrinter);})

  APROT_SERVICEIMP(void, AccelKeyHit,  (void) )
    APROT_CPP({OpLwiAccelKeyHit(this);})

  APROT_SERVICEIMP(BOOL, GetSelection,  (long far* start, long far* count) )
    APROT_CPP({return OpLwiGetSelection(this, start, count);})

  APROT_SERVICEIMP(BOOL, SetSelection,  (long start, long count) )
    APROT_CPP({return OpLwiSetSelection(this, start, count);})

  APROT_SERVICEIMP(void, SetDefaultItem,  (BOOL set) )
    APROT_CPP({OpLwiSetDefaultItem(this, set);})

  APROT_SERVICEIMP(void, MouseEntry,  (BOOL inside) )
    APROT_CPP({OpLwiMouseEntry(this, inside);})

  APROT_SERVICEIMP(long, Activate,  (int causedBy, BOOL mustKeep) )
    APROT_CPP({return OpLwiActivate(this, causedBy, mustKeep);})

  APROT_SERVICEIMP(void, Deactivated,  (void) )
    APROT_CPP({OpLwiDeactivated(this);})

  APROT_SERVICEIMP(void, Show,  (void) )
    APROT_CPP({OpLwiShow(this);})

  APROT_SERVICEIMP(void, Hide,  (void) )
    APROT_CPP({OpLwiHide(this);})

  APROT_SERVICEIMP(void, Move,  (LPRECT newRect) )
    APROT_CPP({OpLwiMove(this, newRect);})

  APROT_SERVICEIMP(void, Enable,  (BOOL bEnable) )
    APROT_CPP({OpLwiEnable(this, bEnable);})

  APROT_SERVICEIMP(void, ScreenLocationChanged,  (int causedBy) )
    APROT_CPP({OpLwiScreenLocationChanged(this, causedBy);})

  APROT_SERVICEIMP(long, AdjustNewRect,  (LPRECT newRect, LONG adjustFlag) )
    APROT_CPP({return OpLwiAdjustNewRect(this, newRect, adjustFlag);})

  APROT_SERVICEIMP(BOOL, PointInItem,  (LPPOINT pt) )
    APROT_CPP({return OpLwiPointInItem(this, pt);})

  APROT_SERVICEIMP(void, InvalidateItem,  (BOOL bErase) )
    APROT_CPP({OpLwiInvalidateItem(this, bErase);})

  APROT_SERVICEIMP(BOOL, ItemInRect,  (LPRECT rect, BOOL fReqFullEnclosure) )
    APROT_CPP({return OpLwiItemInRect(this, rect, fReqFullEnclosure);})

  APROT_SERVICEIMP(BOOL, ItemInRegion,  (HRGN hRegion, BOOL bDeviceUnits) )
    APROT_CPP({return OpLwiItemInRegion(this, hRegion, bDeviceUnits);})

  APROT_SERVICEIMP(void, RemoveItemFromRegion,  (HRGN hRegion, BOOL bDeviceUnits) )
    APROT_CPP({OpLwiRemoveItemFromRegion(this, hRegion, bDeviceUnits);})

  APROT_SERVICEIMP(void, DrawTrackingRect,  (HDC hdc, LPRECT rect) )
    APROT_CPP({OpLwiDrawTrackingRect(this, hdc, rect);})

  APROT_SERVICEIMP(void, DrawGrabHandles,  (HDC hdc) )
    APROT_CPP({OpLwiDrawGrabHandles(this, hdc);})

  APROT_SERVICEIMP(void, InvalidateGrabHandles,  (BOOL bErase) )
    APROT_CPP({OpLwiInvalidateGrabHandles(this, bErase);})

  APROT_SERVICEIMP(long, PointInGrabHandle,  (LPPOINT pt, HCURSOR far* phCursor) )
    APROT_CPP({return OpLwiPointInGrabHandle(this, pt, phCursor);})

  APROT_SERVICEIMP(MEMBLOCKID, GetExtraData, (void) )
    APROT_CPP({return OpLwiGetExtraData(this);})

  APROT_SERVICEIMP(void, ExportExtraData, (void) )
    APROT_CPP({OpLwiExportExtraData(this);})

  APROT_SERVICEIMP(void, ImportExtraData, (void) )
    APROT_CPP({OpLwiImportExtraData(this);})

  APROT_SERVICEIMP(void, Draw, (HDC hdc, LPRECT invalidRect) )
    APROT_CPP({OpLwiDraw(this, hdc, invalidRect);})

  APROT_SERVICEIMP(void, KeyDown, (int vKey, char ascii, long keyData) )
    APROT_CPP({OpLwiKeyDown(this, vKey, ascii, keyData);})

  APROT_SERVICEIMP(void, KeyUp, (int vKey, char ascii, long keyData) )
    APROT_CPP({OpLwiKeyUp(this, vKey, ascii, keyData);})

  APROT_SERVICEIMP(void, MouseMove, (LPPOINT where, int keysDown))
    APROT_CPP({OpLwiMouseMove(this, where, keysDown);})

  APROT_SERVICEIMP(void, MouseDown, (LPPOINT where,  int keysDown, int button))
    APROT_CPP({OpLwiMouseDown(this, where, keysDown, button);})

  APROT_SERVICEIMP(void, DoubleClicked, (LPPOINT where,  int keysDown, int button))
    APROT_CPP({OpLwiDoubleClicked(this, where, keysDown, button);})

  APROT_SERVICEIMP(void, MouseUp, (LPPOINT where,  int keysDown, int button))
    APROT_CPP({OpLwiMouseUp(this, where, keysDown, button);})

  APROT_SERVICEIMP(void, CancelMode, (void) )
    APROT_CPP({OpLwiCancelMode(this);})

  APROT_ENDSERVICESIMP(OpLiteWndItem, OpLiteWndItemD)

APROT_ENDPROTOCOLIMP(OpLiteWndItem, OpLiteWndItemD);

/* ****************************************************************
 * Default implementation for HWND window item
 * **************************************************************** */

APROT_BEGINPROTOCOLIMP(OpHwndWndItem, OpHwndWndItemD)
#ifdef __cplusplus
  private:
    OpHwndWndItem();
  public:
    OpHwndWndItem(OBJECTID oiItem, OBJECTID oiParent, LPRECT rcItem, long status)
    {
    OpHwiInit(this, oiItem, oiParent, rcItem, status);
    }
#endif

  APROT_BEGINSERVICESIMP(OpHwndWndItem, OpHwndWndItemD)

  APROT_SERVICEIMP(void, Destroy,  (void) )
    APROT_CPP({OpHwiDestroy(this);})

  APROT_SERVICEIMP(long, QueryProtocol,  (ApQueryInfo far* qpi, PROTOCOLID pid) )
    APROT_CPP({return OpHwiQueryProtocol(this, qpi, pid);})

  APROT_SERVICEIMP(OBJECTID, Object,  (void) )
    APROT_CPP({return OpHwiObject(this);})

  APROT_SERVICEIMP(TYPEID, Type,  (void) )
    APROT_CPP({return OpHwiType(this);})

  APROT_SERVICEIMP(LONG, Edit,  (BOOL bEditExtra) )
    APROT_CPP({return OpHwiEdit(this, bEditExtra);})

  APROT_SERVICEIMP(void, Cut,  (void) )
    APROT_CPP({OpHwiCut(this);})

  APROT_SERVICEIMP(void, Copy,  (void) )
    APROT_CPP({OpHwiCopy(this);})

  APROT_SERVICEIMP(void, Paste,  (void) )
    APROT_CPP({OpHwiPaste(this);})

  APROT_SERVICEIMP(void, Undo,  (void) )
    APROT_CPP({OpHwiUndo(this);})

  APROT_SERVICEIMP(void, DataChanged,  (long details, long modifiers, long variation) )
    APROT_CPP({OpHwiDataChanged(this, details, modifiers, variation);})

  APROT_SERVICEIMP(void, ParentOpened,  (void) )
    APROT_CPP({OpHwiParentOpened(this);})

  APROT_SERVICEIMP(void, ParentClosing,  (void) )
    APROT_CPP({OpHwiParentClosing(this);})

  APROT_SERVICEIMP(void, Print,  (HDC hdc, LPRECT printRect, int pclPrinter) )
    APROT_CPP({OpHwiPrint(this, hdc, printRect, pclPrinter);})

  APROT_SERVICEIMP(void, AccelKeyHit,  (void) )
    APROT_CPP({OpHwiAccelKeyHit(this);})

  APROT_SERVICEIMP(BOOL, GetSelection,  (long far* start, long far* count) )
    APROT_CPP({return OpHwiGetSelection(this, start, count);})

  APROT_SERVICEIMP(BOOL, SetSelection,  (long start, long count) )
    APROT_CPP({return OpHwiSetSelection(this, start, count);})

  APROT_SERVICEIMP(void, SetDefaultItem,  (BOOL set) )
    APROT_CPP({OpHwiSetDefaultItem(this, set);})

  APROT_SERVICEIMP(void, MouseEntry,  (BOOL inside) )
    APROT_CPP({OpHwiMouseEntry(this, inside);})

  APROT_SERVICEIMP(long, Activate,  (int causedBy, BOOL mustKeep) )
    APROT_CPP({return OpHwiActivate(this, causedBy, mustKeep);})

  APROT_SERVICEIMP(void, Deactivated,  (void) )
    APROT_CPP({OpHwiDeactivated(this);})

  APROT_SERVICEIMP(void, Show,  (void) )
    APROT_CPP({OpHwiShow(this);})

  APROT_SERVICEIMP(void, Hide,  (void) )
    APROT_CPP({OpHwiHide(this);})

  APROT_SERVICEIMP(void, Move,  (LPRECT newRect) )
    APROT_CPP({OpHwiMove(this, newRect);})

  APROT_SERVICEIMP(void, Enable,  (BOOL bEnable) )
    APROT_CPP({OpHwiEnable(this, bEnable);})

  APROT_SERVICEIMP(void, ScreenLocationChanged,(int causedBy) )
    APROT_CPP({OpHwiScreenLocationChanged(this, causedBy);})

  APROT_SERVICEIMP(long, AdjustNewRect,  (LPRECT newRect, LONG adjustFlag) )
    APROT_CPP({return OpHwiAdjustNewRect(this, newRect, adjustFlag);})

  APROT_SERVICEIMP(void, InvalidateItem,  (BOOL bErase) )
    APROT_CPP({OpHwiInvalidateItem(this, bErase);})

  APROT_SERVICEIMP(BOOL, PointInItem,  (LPPOINT pt) )
    APROT_CPP({return OpHwiPointInItem(this, pt);})

  APROT_SERVICEIMP(BOOL, ItemInRect,  (LPRECT rect, BOOL fReqFullEnclosure) )
    APROT_CPP({return OpHwiItemInRect(this, rect, fReqFullEnclosure);})

  APROT_SERVICEIMP(BOOL, ItemInRegion,  (HRGN hRegion, BOOL bDeviceUnits) )
    APROT_CPP({return OpHwiItemInRegion(this, hRegion, bDeviceUnits);})

  APROT_SERVICEIMP(void, RemoveItemFromRegion,  (HRGN hRegion, BOOL bDeviceUnits) )
    APROT_CPP({OpHwiRemoveItemFromRegion(this, hRegion, bDeviceUnits);})

  APROT_SERVICEIMP(void, DrawTrackingRect,  (HDC hdc, LPRECT rect) )
    APROT_CPP({OpHwiDrawTrackingRect(this, hdc, rect);})

  APROT_SERVICEIMP(void, DrawGrabHandles,  (HDC hdc) )
    APROT_CPP({OpHwiDrawGrabHandles(this, hdc);})

  APROT_SERVICEIMP(void, InvalidateGrabHandles,  (BOOL bErase) )
    APROT_CPP({OpHwiInvalidateGrabHandles(this, bErase);})

  APROT_SERVICEIMP(long, PointInGrabHandle,  (LPPOINT pt, HCURSOR far* phCursor) )
    APROT_CPP({return OpHwiPointInGrabHandle(this, pt, phCursor);})

  APROT_SERVICEIMP(MEMBLOCKID, GetExtraData, (void) )
    APROT_CPP({return OpHwiGetExtraData(this);})

  APROT_SERVICEIMP(void, ExportExtraData, (void) )
    APROT_CPP({OpHwiExportExtraData(this);})

  APROT_SERVICEIMP(void, ImportExtraData, (void) )
    APROT_CPP({OpHwiImportExtraData(this);})

  APROT_SERVICEIMP(LPARAM, ParentNotification, (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) )
    APROT_CPP({return OpHwiParentNotification(this, hwnd, message, wParam, lParam);})

  APROT_ENDSERVICESIMP(OpHwndWndItem, OpHwndWndItemD)


APROT_ENDPROTOCOLIMP(OpHwndWndItem, OpHwndWndItemD);

#endif /* !OP_WND_H */
