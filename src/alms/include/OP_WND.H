/* ****************************************************************
 *
 *  Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * **************************************************************** */

#ifndef OP_WND_H
#define OP_WND_H

#ifdef OWND_DEFAULTIMPLEMENTATION
#define METHOD_BODY(a)
#else
#define METHOD_BODY(a) a
#endif

#define PROTOCOL_VERSION		(short)1


/* ****************************************************************
 * APROTOCOLID for Window Item Protocol
 * **************************************************************** */

#define APROT_WND                     APROTOCOL_MAKEID('P','w','i','n')

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
class OpWndItemD;
class OpLiteWndItemD;
class OpLiteWndItem;
class OpHwndWndItemD;
class OpHwndWndItem;
class WINDOW;
class WINDOWRUNTIME;
class WINDOWAPI;
class WNDITEM_DATA;
class Items;
class OpGroupWndItem;

extern "C" {
void 				 ALMAPI OpGwiCreated(OpGroupWndItem *, OpWndItemD * *, int);
void				 ALMAPI OpGwiUngrouped(OpGroupWndItem *);
short 			 ALMAPI OpGwiItemAdded(OpGroupWndItem *, OpWndItemD *);
}


typedef OpWndItemD * (ALMAPI* PWICreateFN)(AOBJECTID oiItem, AOBJECTID oiParent, LPRECT rcItem, long status, AMEMBLOCKID extraData);
typedef BOOL (ALMAPI* CALLVIPROC)(OpWndItemD *, long, long);

class OpWndItemD : public ApObjectD 
	{
	public:
		RECT                m_rcItem;
		AOBJECTID            m_oiParent;
		HWND                m_hwndParent;
	
	private:
		long                m_status;
		short               m_grow;
		long                m_uidItem;
		char                m_accelVKey;
		char                m_accelModifier;
		int		             	m_index;

	public:
		short ALMAPI Version(void) { return PROTOCOL_VERSION;}
		virtual LONG ALMAPI Edit ( BOOL bEditExtra) = 0;
		virtual void ALMAPI Cut (void ) = 0;
		virtual void ALMAPI Copy (void ) = 0;
		virtual void ALMAPI Paste (void ) = 0;
		virtual void ALMAPI Undo (void ) = 0;

		virtual void ALMAPI DataChanged ( long details, long modifiers, long variation) = 0;
		virtual void ALMAPI ParentOpened (void ) = 0;
		virtual void ALMAPI ParentClosing (void ) = 0;

		virtual void ALMAPI Print ( HDC hdc, LPRECT printRect, int pclPrinter) = 0;
		virtual void ALMAPI AccelKeyHit (void ) = 0;

		virtual BOOL ALMAPI GetSelection ( LPINT start, LPINT count) = 0;

		virtual BOOL ALMAPI SetSelection ( int start, int count) = 0;
		virtual void ALMAPI SetDefaultItem ( BOOL set) = 0;
		virtual void ALMAPI MouseEntry ( BOOL inside) = 0;

		virtual long ALMAPI Activate ( int causedBy, BOOL mustKeep) = 0;
		virtual void ALMAPI Deactivated (void ) = 0;
		virtual void ALMAPI Show (void ) = 0;
		virtual void ALMAPI Hide (void ) = 0;
		virtual void ALMAPI Move ( LPRECT newRect) = 0;
		virtual void ALMAPI Enable ( BOOL bEnable) = 0;
		virtual void ALMAPI ScreenLocationChanged ( int causedBy) = 0;

		virtual long ALMAPI AdjustNewRect ( LPRECT newRect, LONG adjustFlag) = 0;
		virtual void ALMAPI InvalidateItem ( BOOL bErase) = 0;

		virtual BOOL ALMAPI PointInItem ( LPPOINT pt) = 0;
		virtual BOOL ALMAPI ItemInRect ( LPRECT lprc, BOOL fReqFullEnclosure) = 0;
		virtual BOOL ALMAPI ItemInRegion ( HRGN hRegion, BOOL bDeviceUnits) = 0;
		virtual void ALMAPI RemoveItemFromRegion ( HRGN hRegion, BOOL bDeviceUnits) = 0;
		virtual void ALMAPI DrawTrackingRect ( HDC hdc, LPRECT rect) = 0;
		virtual void ALMAPI DrawGrabHandles ( HDC hdc) = 0;
		virtual void ALMAPI InvalidateGrabHandles ( BOOL bErase) = 0;
		virtual long ALMAPI PointInGrabHandle ( LPPOINT pt, HCURSOR * phCursor) = 0;

		virtual AMEMBLOCKID ALMAPI GetExtraData (void ) = 0;
		virtual void ALMAPI ExportExtraData (void ) = 0;
		virtual void ALMAPI ImportExtraData (void ) = 0;

		virtual BOOL ALMAPI QueryDesiredPalette(AMEMBLOCKID miPalette) = 0;  //fill memblock with LOGPALETTE returns TRUE if handled
		virtual LRESULT ALMAPI ParentNotification ( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;

		virtual void EmptySlot0(void){};
		virtual void EmptySlot1(void){};
		virtual void EmptySlot2(void){};
		virtual void EmptySlot3(void){};
		virtual void EmptySlot4(void){};

		friend void localInit(OpWndItemD * wi, AOBJECTID oiItem, AOBJECTID oiParent, LPRECT rcItem, long status);
		friend AOBJECTID localObject(OpWndItemD * wi);
		friend void ALMAPI OpGwiCreated(OpGroupWndItem *, OpWndItemD * *, int);
		friend void ALMAPI OpGwiUngrouped(OpGroupWndItem *);
		friend short ALMAPI OpGwiItemAdded(OpGroupWndItem *, OpWndItemD *);
		friend WINDOW;
		friend WINDOWRUNTIME;
		friend WINDOWAPI;
		friend WNDITEM_DATA;
		friend Items;
		friend OpGroupWndItem;
	};

class OpHwndWndItemD : public OpWndItemD 
	{
	public:
	HWND                m_hwndItem;

	public:
		virtual void EmptySlot5(void){};
		virtual void EmptySlot6(void){};
		virtual void EmptySlot7(void){};
		virtual void EmptySlot8(void){};
		virtual void EmptySlot9(void){};
		

	};


class OpLiteWndItemD : public OpWndItemD 	
	{
	public:
		virtual void ALMAPI Draw ( HDC hdc, LPRECT invalidRect) = 0;
		virtual void ALMAPI KeyDown ( int vKey, char ascii, long keyData) = 0;
		virtual void ALMAPI KeyUp ( int vKey, char ascii, long keyData) = 0;
		virtual void ALMAPI MouseMove ( LPPOINT where, int keysDown) = 0;
		virtual void ALMAPI MouseDown ( LPPOINT where, int keysDown, int button) = 0;
		virtual void ALMAPI DoubleClicked ( LPPOINT where, int keysDown, int button) = 0;
		virtual void ALMAPI MouseUp ( LPPOINT where, int keysDown, int button) = 0;
		virtual void ALMAPI CancelMode (void ) = 0;
		virtual void EmptySlot10(void){};
		virtual void EmptySlot11(void){};
		virtual void EmptySlot12(void){};
		virtual void EmptySlot13(void){};
		virtual void EmptySlot14(void){};

	};


extern "C" {
void         ALMAPI OpLwiInit(OpLiteWndItem * lwi, AOBJECTID oiItem, AOBJECTID oiParent, LPRECT rcItem, long status);
void         ALMAPI OpHwiInit(OpHwndWndItem * hwi, AOBJECTID oiItem, AOBJECTID oiParent, LPRECT rcItem, long status);
void         ALMAPI OpLwiDestroy(OpLiteWndItem * lwi);
void         ALMAPI OpHwiDestroy(OpHwndWndItem * hwi);
long         ALMAPI OpLwiQueryProtocol(OpLiteWndItem * lwi, ApQueryInfo * qpi, APROTOCOLID pid);
long         ALMAPI OpHwiQueryProtocol(OpHwndWndItem * hwi, ApQueryInfo * qpi, APROTOCOLID pid);
AOBJECTID     ALMAPI OpLwiObject(OpLiteWndItem * lwi);
AOBJECTID     ALMAPI OpHwiObject(OpHwndWndItem * hwi);
ATYPEID       ALMAPI OpLwiType(OpLiteWndItem * lwi);
ATYPEID       ALMAPI OpHwiType(OpHwndWndItem * hwi);
LONG         ALMAPI OpLwiEdit(OpLiteWndItem * lwi, BOOL bEditExtra);
LONG         ALMAPI OpHwiEdit(OpHwndWndItem * hwi, BOOL bEditExtra);
void         ALMAPI OpLwiCut(OpLiteWndItem * lwi);
void         ALMAPI OpHwiCut(OpHwndWndItem * hwi);
void         ALMAPI OpLwiCopy(OpLiteWndItem * lwi);
void         ALMAPI OpHwiCopy(OpHwndWndItem * hwi);
void         ALMAPI OpLwiPaste(OpLiteWndItem * lwi);
void         ALMAPI OpHwiPaste(OpHwndWndItem * hwi);
void         ALMAPI OpLwiUndo(OpLiteWndItem * lwi);
void         ALMAPI OpHwiUndo(OpHwndWndItem * hwi);
void         ALMAPI OpLwiDataChanged(OpLiteWndItem * lwi, long details, long modifiers, long variation);
void         ALMAPI OpHwiDataChanged(OpHwndWndItem * hwi, long details, long modifiers, long variation);
void         ALMAPI OpLwiParentOpened(OpLiteWndItem * lwi);
void         ALMAPI OpHwiParentOpened(OpHwndWndItem * hwi);
void         ALMAPI OpLwiParentClosing(OpLiteWndItem * lwi);
void         ALMAPI OpHwiParentClosing(OpHwndWndItem * hwi);
void         ALMAPI OpLwiPrint(OpLiteWndItem * lwi, HDC hdc, LPRECT rect, int pclPrinter);
void         ALMAPI OpHwiPrint(OpHwndWndItem * hwi, HDC hdc, LPRECT rect, int pclPrinter);
void         ALMAPI OpLwiAccelKeyHit(OpLiteWndItem * lwi);
void         ALMAPI OpHwiAccelKeyHit(OpHwndWndItem * hwi);
BOOL         ALMAPI OpLwiGetSelection(OpLiteWndItem * lwi, LPINT start, LPINT count);
BOOL         ALMAPI OpHwiGetSelection(OpHwndWndItem * hwi, LPINT start, LPINT count);
BOOL         ALMAPI OpLwiSetSelection(OpLiteWndItem * lwi, int start, int count);
BOOL         ALMAPI OpHwiSetSelection(OpHwndWndItem * hwi, int start, int count);
void         ALMAPI OpLwiSetDefaultItem(OpLiteWndItem * lwi, BOOL set);
void         ALMAPI OpHwiSetDefaultItem(OpHwndWndItem * hwi, BOOL set);
void         ALMAPI OpLwiMouseEntry(OpLiteWndItem * lwi, BOOL inside);
void         ALMAPI OpHwiMouseEntry(OpHwndWndItem * hwi, BOOL inside);
long         ALMAPI OpLwiActivate(OpLiteWndItem * lwi, int causedBy, BOOL mustKeep);
long         ALMAPI OpHwiActivate(OpHwndWndItem * hwi, int causedBy, BOOL mustKeep);
void         ALMAPI OpLwiDeactivated(OpLiteWndItem * lwi);
void         ALMAPI OpHwiDeactivated(OpHwndWndItem * hwi);
void         ALMAPI OpLwiShow(OpLiteWndItem * lwi);
void         ALMAPI OpHwiShow(OpHwndWndItem * hwi);
void         ALMAPI OpLwiHide(OpLiteWndItem * lwi);
void         ALMAPI OpHwiHide(OpHwndWndItem * hwi);
void         ALMAPI OpLwiMove(OpLiteWndItem * lwi, LPRECT newRect);
void         ALMAPI OpHwiMove(OpHwndWndItem * hwi, LPRECT newRect);
void         ALMAPI OpLwiEnable(OpLiteWndItem * lwi, BOOL bEnable);
void         ALMAPI OpHwiEnable(OpHwndWndItem * hwi, BOOL bEnable);
void         ALMAPI OpLwiScreenLocationChanged(OpLiteWndItem * lwi, int causedBy);
void         ALMAPI OpHwiScreenLocationChanged(OpHwndWndItem * lwi, int causedBy);
long         ALMAPI OpLwiAdjustNewRect(OpLiteWndItem * lwi, LPRECT newRect, LONG adjustFlag);
long         ALMAPI OpHwiAdjustNewRect(OpHwndWndItem * hwi, LPRECT newRect, LONG adjustFlag);
void         ALMAPI OpLwiInvalidateItem(OpLiteWndItem * lwi, BOOL bErase);
void         ALMAPI OpHwiInvalidateItem(OpHwndWndItem * hwi, BOOL bErase);
BOOL         ALMAPI OpLwiPointInItem(OpLiteWndItem * lwi, LPPOINT pt);
BOOL         ALMAPI OpHwiPointInItem(OpHwndWndItem * hwi, LPPOINT pt);
BOOL         ALMAPI OpLwiItemInRect(OpLiteWndItem * lwi, LPRECT rect, BOOL fReqFullEnclosure);
BOOL         ALMAPI OpHwiItemInRect(OpHwndWndItem * hwi, LPRECT rect, BOOL fReqFullEnclosure);
BOOL         ALMAPI OpLwiItemInRegion(OpLiteWndItem * lwi, HRGN hRegion, BOOL bDeviceUnits);
BOOL         ALMAPI OpHwiItemInRegion(OpHwndWndItem * hwi, HRGN hRegion, BOOL bDeviceUnits);
void         ALMAPI OpLwiRemoveItemFromRegion(OpLiteWndItem * lwi, HRGN hRegion, BOOL bDeviceUnits);
void         ALMAPI OpHwiRemoveItemFromRegion(OpHwndWndItem * hwi, HRGN hRegion, BOOL bDeviceUnits);
void         ALMAPI OpLwiDrawTrackingRect(OpLiteWndItem * lwi, HDC hdc, LPRECT rect);
void         ALMAPI OpHwiDrawTrackingRect(OpHwndWndItem * hwi, HDC hdc, LPRECT rect);
void         ALMAPI OpLwiDrawGrabHandles(OpLiteWndItem * lwi, HDC hdc);
void         ALMAPI OpHwiDrawGrabHandles(OpHwndWndItem * hwi, HDC hdc);
void         ALMAPI OpLwiInvalidateGrabHandles(OpLiteWndItem * lwi, BOOL bErase);
void         ALMAPI OpHwiInvalidateGrabHandles(OpHwndWndItem * hwi, BOOL bErase);
long         ALMAPI OpLwiPointInGrabHandle(OpLiteWndItem * lwi, LPPOINT pt, HCURSOR * phCursor);
long         ALMAPI OpHwiPointInGrabHandle(OpHwndWndItem * hwi, LPPOINT pt, HCURSOR * phCursor);
AMEMBLOCKID   ALMAPI OpLwiGetExtraData(OpLiteWndItem * lwi);
AMEMBLOCKID   ALMAPI OpHwiGetExtraData(OpHwndWndItem * hwi);
void         ALMAPI OpLwiExportExtraData(OpLiteWndItem * lwi);
void         ALMAPI OpHwiExportExtraData(OpHwndWndItem * hwi);
void         ALMAPI OpLwiImportExtraData(OpLiteWndItem * lwi);
void         ALMAPI OpHwiImportExtraData(OpHwndWndItem * hwi);
BOOL 			 	 ALMAPI OpLwiQueryDesiredPalette(OpLiteWndItem * lwi, AMEMBLOCKID miPalette);
BOOL 			 	 ALMAPI OpHwiQueryDesiredPalette(OpHwndWndItem * hwi, AMEMBLOCKID miPalette);
LRESULT      ALMAPI OpLwiParentNotification(OpLiteWndItem * lwi, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT      ALMAPI OpHwiParentNotification(OpHwndWndItem * hwi, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void				 ALMAPI OpLwiDraw(OpLiteWndItem * lwi, HDC hdc, LPRECT invalidRect);
void				 ALMAPI OpLwiKeyDown(OpLiteWndItem * lwi, int vKey, char ascii, long keyData);
void				 ALMAPI OpLwiKeyUp(OpLiteWndItem * lwi, int vKey, char ascii, long keyData);
void				 ALMAPI OpLwiMouseMove(OpLiteWndItem * lwi, LPPOINT where, int keysDown);
void				 ALMAPI OpLwiMouseDown(OpLiteWndItem * lwi, LPPOINT where, int keysDown, int button);
void				 ALMAPI OpLwiDoubleClicked(OpLiteWndItem * lwi, LPPOINT where, int keysDown, int button);
void				 ALMAPI OpLwiMouseUp(OpLiteWndItem * lwi, LPPOINT where, int keysDown, int button);
void				 ALMAPI OpLwiCancelMode(OpLiteWndItem * lwi);
}


class OpLiteWndItem : public OpLiteWndItemD 
	{
	private:
		OpLiteWndItem();

	public:
		OpLiteWndItem(AOBJECTID oiItem, AOBJECTID oiParent, LPRECT rcItem, long status) { OpLwiInit(this, oiItem, oiParent, rcItem, status); }			
		void ALMAPI Destroy (void)
				METHOD_BODY({OpLwiDestroy(this);});
		long ALMAPI QueryProtocol (ApQueryInfo * qpi, APROTOCOLID pid)
				METHOD_BODY( {return OpLwiQueryProtocol(this, qpi, pid);});
		AOBJECTID ALMAPI Object (void)
				METHOD_BODY( {return OpLwiObject(this);});
		ATYPEID ALMAPI Type (void)
				METHOD_BODY( {return OpLwiType(this);});
		LONG ALMAPI Edit (BOOL bEditExtra)
				METHOD_BODY( {return OpLwiEdit(this, bEditExtra);});
		void ALMAPI Cut (void)
				METHOD_BODY( {OpLwiCut(this);});
		void ALMAPI Copy (void)
				METHOD_BODY( {OpLwiCopy(this);});
		void ALMAPI Paste (void)
				METHOD_BODY( {OpLwiPaste(this);});
		void ALMAPI Undo (void)
				METHOD_BODY( {OpLwiUndo(this);});
		void ALMAPI DataChanged (long details, long modifiers, long variation)
				METHOD_BODY( {OpLwiDataChanged(this, details, modifiers, variation);});
		void ALMAPI ParentOpened (void)
				METHOD_BODY( {OpLwiParentOpened(this);});
		void ALMAPI ParentClosing (void)
				METHOD_BODY( {OpLwiParentClosing(this);});
		void ALMAPI Print (HDC hdc, LPRECT printRect, int pclPrinter)
				METHOD_BODY( {OpLwiPrint(this, hdc, printRect, pclPrinter);});
		void ALMAPI AccelKeyHit (void)
				METHOD_BODY( {OpLwiAccelKeyHit(this);});
		BOOL ALMAPI GetSelection (LPINT start, LPINT count)
				METHOD_BODY( {return OpLwiGetSelection(this, start, count);});
		BOOL ALMAPI SetSelection (int start, int count)
				METHOD_BODY( {return OpLwiSetSelection(this, start, count);});
		void ALMAPI SetDefaultItem (BOOL set)
				METHOD_BODY( {OpLwiSetDefaultItem(this, set);});
		void ALMAPI MouseEntry (BOOL inside)
				METHOD_BODY( {OpLwiMouseEntry(this, inside);});
		long ALMAPI Activate (int causedBy, BOOL mustKeep)
				METHOD_BODY( {return OpLwiActivate(this, causedBy, mustKeep);});
		void ALMAPI Deactivated (void)
				METHOD_BODY( {OpLwiDeactivated(this);});
		void ALMAPI Show (void)
				METHOD_BODY( {OpLwiShow(this);});
		void ALMAPI Hide (void)
				METHOD_BODY( {OpLwiHide(this);});
		void ALMAPI Move (LPRECT newRect)
				METHOD_BODY( {OpLwiMove(this, newRect);});
		void ALMAPI Enable (BOOL bEnable)
				METHOD_BODY( {OpLwiEnable(this, bEnable);});
		void ALMAPI ScreenLocationChanged (int causedBy)
				METHOD_BODY( {OpLwiScreenLocationChanged(this, causedBy);});
		long ALMAPI AdjustNewRect (LPRECT newRect, LONG adjustFlag)
				METHOD_BODY( {return OpLwiAdjustNewRect(this, newRect, adjustFlag);});
		BOOL ALMAPI PointInItem (LPPOINT pt)
				METHOD_BODY( {return OpLwiPointInItem(this, pt);});
		void ALMAPI InvalidateItem (BOOL bErase)
				METHOD_BODY( {OpLwiInvalidateItem(this, bErase);});
		BOOL ALMAPI ItemInRect (LPRECT rect, BOOL fReqFullEnclosure)
				METHOD_BODY( {return OpLwiItemInRect(this, rect, fReqFullEnclosure);});
		BOOL ALMAPI ItemInRegion (HRGN hRegion, BOOL bDeviceUnits)
				METHOD_BODY( {return OpLwiItemInRegion(this, hRegion, bDeviceUnits);});
		void ALMAPI RemoveItemFromRegion (HRGN hRegion, BOOL bDeviceUnits)
				METHOD_BODY( {OpLwiRemoveItemFromRegion(this, hRegion, bDeviceUnits);});
		void ALMAPI DrawTrackingRect (HDC hdc, LPRECT rect)
				METHOD_BODY( {OpLwiDrawTrackingRect(this, hdc, rect);});
		void ALMAPI DrawGrabHandles (HDC hdc)
				METHOD_BODY( {OpLwiDrawGrabHandles(this, hdc);});
		void ALMAPI InvalidateGrabHandles (BOOL bErase)
				METHOD_BODY( {OpLwiInvalidateGrabHandles(this, bErase);});
		long ALMAPI PointInGrabHandle (LPPOINT pt, HCURSOR * phCursor)
				METHOD_BODY( {return OpLwiPointInGrabHandle(this, pt, phCursor);});
		AMEMBLOCKID ALMAPI GetExtraData (void)
				METHOD_BODY( {return OpLwiGetExtraData(this);});
		void ALMAPI ExportExtraData (void)
				METHOD_BODY( {OpLwiExportExtraData(this);});
		void ALMAPI ImportExtraData (void)
				METHOD_BODY( {OpLwiImportExtraData(this);});
		BOOL ALMAPI QueryDesiredPalette(AMEMBLOCKID miPalette)
				METHOD_BODY( {return OpLwiQueryDesiredPalette(this, miPalette);});
		LPARAM ALMAPI ParentNotification (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
				METHOD_BODY( {return OpLwiParentNotification(this, hwnd, message, wParam, lParam);});
		void ALMAPI Draw (HDC hdc, LPRECT invalidRect)
				METHOD_BODY( {OpLwiDraw(this, hdc, invalidRect);});
		void ALMAPI KeyDown (int vKey, char ascii, long keyData)
				METHOD_BODY( {OpLwiKeyDown(this, vKey, ascii, keyData);});
		void ALMAPI KeyUp (int vKey, char ascii, long keyData)
				METHOD_BODY( {OpLwiKeyUp(this, vKey, ascii, keyData);});
		void ALMAPI MouseMove (LPPOINT where, int keysDown)
				METHOD_BODY( {OpLwiMouseMove(this, where, keysDown);});
		void ALMAPI MouseDown (LPPOINT where, int keysDown, int button)
				METHOD_BODY( {OpLwiMouseDown(this, where, keysDown, button);});
		void ALMAPI DoubleClicked (LPPOINT where, int keysDown, int button)
				METHOD_BODY( {OpLwiDoubleClicked(this, where, keysDown, button);});
		void ALMAPI MouseUp (LPPOINT where, int keysDown, int button)
				METHOD_BODY( {OpLwiMouseUp(this, where, keysDown, button);});
		void ALMAPI CancelMode (void)
				METHOD_BODY( {OpLwiCancelMode(this);});
	};

class OpHwndWndItem : public OpHwndWndItemD 
	{
	private:
		OpHwndWndItem();
	
	public:
		OpHwndWndItem(AOBJECTID oiItem, AOBJECTID oiParent, LPRECT rcItem, long status) { OpHwiInit(this, oiItem, oiParent, rcItem, status); }
		void ALMAPI Destroy (void)
				METHOD_BODY( {OpHwiDestroy(this);});
		long ALMAPI QueryProtocol (ApQueryInfo * qpi, APROTOCOLID pid)
				METHOD_BODY( {return OpHwiQueryProtocol(this, qpi, pid);});
		AOBJECTID ALMAPI Object (void)
				METHOD_BODY( {return OpHwiObject(this);});
		ATYPEID ALMAPI Type (void)
				METHOD_BODY( {return OpHwiType(this);});
		LONG ALMAPI Edit (BOOL bEditExtra)
				METHOD_BODY( {return OpHwiEdit(this, bEditExtra);});
		void ALMAPI Cut (void)
				METHOD_BODY( {OpHwiCut(this);});
		void ALMAPI Copy (void)
				METHOD_BODY( {OpHwiCopy(this);});
		void ALMAPI Paste (void)
				METHOD_BODY( {OpHwiPaste(this);});
		void ALMAPI Undo (void)
				METHOD_BODY( {OpHwiUndo(this);});
		void ALMAPI DataChanged (long details, long modifiers, long variation)
				METHOD_BODY( {OpHwiDataChanged(this, details, modifiers, variation);});
		void ALMAPI ParentOpened (void)
				METHOD_BODY( {OpHwiParentOpened(this);});
		void ALMAPI ParentClosing (void)
				METHOD_BODY( {OpHwiParentClosing(this);});
		void ALMAPI Print (HDC hdc, LPRECT printRect, int pclPrinter)
				METHOD_BODY( {OpHwiPrint(this, hdc, printRect, pclPrinter);});
		void ALMAPI AccelKeyHit (void)
				METHOD_BODY( {OpHwiAccelKeyHit(this);});
		BOOL ALMAPI GetSelection (LPINT start, LPINT count)
				METHOD_BODY( {return OpHwiGetSelection(this, start, count);});
		BOOL ALMAPI SetSelection (int start, int count)
				METHOD_BODY( {return OpHwiSetSelection(this, start, count);});
		void ALMAPI SetDefaultItem (BOOL set)
				METHOD_BODY( {OpHwiSetDefaultItem(this, set);});
		void ALMAPI MouseEntry (BOOL inside)
				METHOD_BODY( {OpHwiMouseEntry(this, inside);});
		long ALMAPI Activate (int causedBy, BOOL mustKeep)
				METHOD_BODY( {return OpHwiActivate(this, causedBy, mustKeep);});
		void ALMAPI Deactivated (void)
				METHOD_BODY( {OpHwiDeactivated(this);});
		void ALMAPI Show (void)
				METHOD_BODY( {OpHwiShow(this);});
		void ALMAPI Hide (void)
				METHOD_BODY( {OpHwiHide(this);});
		void ALMAPI Move (LPRECT newRect)
				METHOD_BODY( {OpHwiMove(this, newRect);});
		void ALMAPI Enable (BOOL bEnable)
				METHOD_BODY( {OpHwiEnable(this, bEnable);});
		void ALMAPI ScreenLocationChanged (int causedBy)
				METHOD_BODY( {OpHwiScreenLocationChanged(this, causedBy);});
		long ALMAPI AdjustNewRect (LPRECT newRect, LONG adjustFlag)
				METHOD_BODY( {return OpHwiAdjustNewRect(this, newRect, adjustFlag);});
		void ALMAPI InvalidateItem (BOOL bErase)
				METHOD_BODY( {OpHwiInvalidateItem(this, bErase);});
		BOOL ALMAPI PointInItem (LPPOINT pt)
				METHOD_BODY( {return OpHwiPointInItem(this, pt);});
		BOOL ALMAPI ItemInRect (LPRECT rect, BOOL fReqFullEnclosure)
				METHOD_BODY( {return OpHwiItemInRect(this, rect, fReqFullEnclosure);});
		BOOL ALMAPI ItemInRegion (HRGN hRegion, BOOL bDeviceUnits)
				METHOD_BODY( {return OpHwiItemInRegion(this, hRegion, bDeviceUnits);});
		void ALMAPI RemoveItemFromRegion (HRGN hRegion, BOOL bDeviceUnits)
				METHOD_BODY( {OpHwiRemoveItemFromRegion(this, hRegion, bDeviceUnits);});
		void ALMAPI DrawTrackingRect (HDC hdc, LPRECT rect)
				METHOD_BODY( {OpHwiDrawTrackingRect(this, hdc, rect);});
		void ALMAPI DrawGrabHandles (HDC hdc)
				METHOD_BODY( {OpHwiDrawGrabHandles(this, hdc);});
		void ALMAPI InvalidateGrabHandles (BOOL bErase)
				METHOD_BODY( {OpHwiInvalidateGrabHandles(this, bErase);});
		long ALMAPI PointInGrabHandle (LPPOINT pt, HCURSOR * phCursor)
				METHOD_BODY( {return OpHwiPointInGrabHandle(this, pt, phCursor);});
		AMEMBLOCKID ALMAPI GetExtraData (void)
				METHOD_BODY( {return OpHwiGetExtraData(this);});
		void ALMAPI ExportExtraData (void)
				METHOD_BODY( {OpHwiExportExtraData(this);});
		void ALMAPI ImportExtraData (void)
				METHOD_BODY( {OpHwiImportExtraData(this);});
		BOOL ALMAPI QueryDesiredPalette(AMEMBLOCKID miPalette)
				METHOD_BODY( {return OpHwiQueryDesiredPalette(this, miPalette);});
		LPARAM ALMAPI ParentNotification (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
				METHOD_BODY( {return OpHwiParentNotification(this, hwnd, message, wParam, lParam);});
	};

#endif //OP_WND_H



