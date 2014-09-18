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
// FILE:          SHP_PROT.H
//
// AUTHOR:        Kevin Smart
//
// DESCRIPTION:   Contains definition for shape items protocol
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef SHP_PROT_H
#define SHP_PROT_H

// required includes ///////////////////////////////////////////////////////

#ifndef RCINVOKED

#ifndef A_ALM_H
#include  <a_alm.h>
#endif

#ifndef OP_WND_H
#include  <op_wnd.h>
#endif

#ifndef O_WNDSHP_H
#include  <o_wndshp.h>
#endif

#ifndef XP_MACRO_H
#include "xp_macro.h"
#endif

#endif  // #ifndef RCINVOKED

#ifndef SHP_ATTR_H
#include  "shp_attr.h"
#endif

// forward class references ////////////////////////////////////////////////

#ifndef RCINVOKED
class ShapePalette;
#endif

// file structures for shapes //////////////////////////////////////////////

#ifndef RCINVOKED

struct SHAPE_RECT
  {
  OShpBase    base;
  ATTR_LINE   line;
  };

struct SHAPE_RECTFILL
  {
  OShpBase    base;
  ATTR_LINE   line;
  ATTR_FILL   fill;
  };

struct SHAPE_ROUNDRECT
  {
  OShpBase    base;
  ATTR_LINE   line;
  short       xRad;
  short       yRad;
  };

struct SHAPE_ROUNDRECTFILL
  {
  OShpBase    base;
  ATTR_LINE   line;
  ATTR_FILL   fill;
  short       xRad;
  short       yRad;
  };

struct SHAPE_ELLIPSE
  {
  OShpBase    base;
  ATTR_LINE   line;
  };

struct SHAPE_ELLIPSEFILL
  {
  OShpBase    base;
  ATTR_LINE   line;
  ATTR_FILL   fill;
  };

struct SHAPE_LINE
  {
  OShpBase    base;
  ATTR_LINE   line;
  };

struct SHAPE_TEXT
  {
  OShpBase    base;
  ATTR_FONT   font;
  short       nTextLen;
  WORD        nAlign;
  char        szText[1];      // actually this is a long as the text
  };

struct SHAPE_GROUPBOX
  {
  OShpBase    base;
  ATTR_FONT   font;
  ATTR_LINE   line;
  short       nTextLen;
  char        szText[1];      // actually this is a long as the text
  };

struct SHAPE_BITMAP
  {
  OShpBase    base;
  long        lLength;        // size of the bitmap
  BITMAPINFO  bi;             // a packed DIB
  };

#endif

// Resource ids ////////////////////////////////////////////////////////////

#define DLG_FILLSHAPE         300
#define PB_SAMPLE             310

#define DLG_TEXTENTRY         101
#define PB_FONT               200
#define ED_TEXT               210

#define CUR_LINEMOVE          5000

#define DLG_GROUPBOX          400

// Supported shapes ////////////////////////////////////////////////////////

#define SHP_LINE              1
#define SHP_TEXT              2
#define SHP_RECT              3
#define SHP_RECTFILL          4
#define SHP_ROUNDRECT         5
#define SHP_ROUNDRECTFILL     6
#define SHP_ELLIPSE           7
#define SHP_ELLIPSEFILL       8
#define SHP_BITMAP            9
#define SHP_GROUPBOX          10

// Shape versions //////////////////////////////////////////////////////////

#ifndef RCINVOKED
#define SHP_VERSION           0x100
#endif

// Shape status flags //////////////////////////////////////////////////////

#define SHP_STAT_CONSTRUCTOR  0x0001
#define SHP_STAT_CANCELLED    0x0002

////////////////////////////////////////////////////////////////////////////

#ifndef RCINVOKED

class ShpItem :
  public OpLiteWndItem

  {
  private:
    short m_nShapeType;
    short m_fStatus;

  public:
                  ShpItem(OBJECTID  oiItem,
                          OBJECTID  oiParent,
                          RECT*     rcItem,
                          long      lStatus,
                          short     nShapeType);

                 ~ShpItem();

    inline short  ShapeType()               {return m_nShapeType;}
    inline void   SetConstructor(BOOL fSet) {if(fSet) m_fStatus |= SHP_STAT_CONSTRUCTOR; else m_fStatus &= ~SHP_STAT_CONSTRUCTOR;}
    inline BOOL   InConstructor()           {return (m_fStatus & SHP_STAT_CONSTRUCTOR) ? TRUE : FALSE;}
    inline void   SetCancelCreate(BOOL fSet){if(fSet) m_fStatus |= SHP_STAT_CANCELLED; else m_fStatus &= ~SHP_STAT_CANCELLED;}
    inline BOOL   CancelCreate()            {return (m_fStatus & SHP_STAT_CANCELLED)   ? TRUE : FALSE;}

    // virtual functions for the shapes
    virtual void  SizeToShape();
    virtual void  ShapeName(LPSTR lpszName, int nBufLen) = 0;

    // overridden window item protocol methods general to all shapes
    virtual LONG ALMAPI   QueryProtocol(pApQueryInfo pqi, PROTOCOLID piProt);
    virtual TYPEID ALMAPI Type();
  };


class ShpItemFill :
  public ShpItem

  {
  public:
                  ShpItemFill(OBJECTID  oiItem,
                              OBJECTID  oiParent,
                              RECT*     rcItem,
                              long      lStatus,
                              short     nShapeType);

                 ~ShpItemFill();

    BOOL          LaunchDialog(HWND hwndParent, LPSTR szTitle);

    // static member functions
    static void         DrawSampleBox(HWND hwnd, LPDRAWITEMSTRUCT lpdi);
    static BOOL         SaveNewAttributes(HWND hwnd);
    static BOOL WINAPI  FilledShapeDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static void         FilledShapeCommand(HWND hwnd, int id, HWND hwndCtl, UINT uNtfyCode);

    // virtual functions for the filled shapes
    virtual void  GetAttributes(ATTR_LINE* line, ATTR_FILL* fill) = 0;
    virtual BOOL  SetAttributes(const ATTR_LINE* line, const ATTR_FILL* fill) = 0;
    virtual void  DrawSample(HWND hwnd, LPDRAWITEMSTRUCT lpdi, COLORREF clrFGPen, COLORREF clrBGPen, short nWidth, short nStyle) = 0;
  };


class ShpRectangleFill :
  public ShpItemFill

  {
  private:
    LineAttrib  m_line;
    FillAttrib  m_fill;
    HBRUSH      m_hbr;
    HPEN        m_hpen;

  public:       // creation from existing shape
                ShpRectangleFill(OBJECTID       oiItem,
                                 OBJECTID       oiParent,
                                 RECT*          rcItem,
                                 long           lStatus,
                                 ATTR_LINE*     line,
                                 ATTR_FILL*     fill);

                // destructor
               ~ShpRectangleFill();

    void        Draw3D(HDC hdc);

    // Overridden ShpItem services
    void        ShapeName(LPSTR lpszName, int nBufLen);

    // Overridden ShpItemFill methods
    void        GetAttributes(ATTR_LINE* line, ATTR_FILL* fill);
    BOOL        SetAttributes(const ATTR_LINE* line, const ATTR_FILL* fill);
    void        DrawSample(HWND hwnd, LPDRAWITEMSTRUCT lpdi, COLORREF clrFGPen, COLORREF clrBGPen, short nWidth, short nStyle);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       ExportExtraData();
    void ALMAPI       ImportExtraData();
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
    void ALMAPI       DrawGrabHandles(HDC hdc);
    void ALMAPI       InvalidateGrabHandles(BOOL fErase);
    void ALMAPI       Print(HDC hdc, RECT* lprcPrint, int nPCLMode);
  };


class ShpRectangle :
  public ShpItem

  {
  private:
    LineAttrib  m_line;
    HPEN        m_hpen;

  public:       // creation from existing shape
                ShpRectangle(OBJECTID       oiItem,
                             OBJECTID       oiParent,
                             RECT*          rcItem,
                             long           lStatus,
                             ATTR_LINE*     line);

                // destructor
               ~ShpRectangle();
              
    void        Draw3D(HDC hdc);

    // Overridden ShpItem services
    void        ShapeName(LPSTR lpszName, int nBufLen);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       ExportExtraData();
    void ALMAPI       ImportExtraData();
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
    void ALMAPI       DrawGrabHandles(HDC hdc);
    void ALMAPI       InvalidateGrabHandles(BOOL fErase);
    BOOL ALMAPI       PointInItem(POINT* lppt);
    BOOL ALMAPI       ItemInRect(RECT* lprc, BOOL fReqFullEnclosure);
    void ALMAPI       Print(HDC hdc, RECT* lprcPrint, int nPCLMode);
  };


class ShpEllipseFill :
  public ShpItemFill

  {
  private:
    LineAttrib  m_line;
    FillAttrib  m_fill;
    HBRUSH      m_hbr;
    HPEN        m_hpen;

  public:       // creation from existing shape
                ShpEllipseFill(OBJECTID       oiItem,
                               OBJECTID       oiParent,
                               RECT*          rcItem,
                               long           lStatus,
                               ATTR_LINE*     line,
                               ATTR_FILL*     fill);

               ~ShpEllipseFill(); 

    void        Draw3D(HDC hdc);

    // Overridden ShpItem services
    void        ShapeName(LPSTR lpszName, int nBufLen);

    // Overridden ShpItemFill methods
    void        GetAttributes(ATTR_LINE* line, ATTR_FILL* fill);
    BOOL        SetAttributes(const ATTR_LINE* line, const ATTR_FILL* fill);
    void        DrawSample(HWND hwnd, LPDRAWITEMSTRUCT lpdi, COLORREF clrFGPen, COLORREF clrBGPen, short nWidth, short nStyle);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       ExportExtraData();
    void ALMAPI       ImportExtraData();
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       DrawTrackingRect(HDC hdc, RECT* rect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
    void ALMAPI       DrawGrabHandles(HDC hdc);
    void ALMAPI       InvalidateGrabHandles(BOOL fErase);
    BOOL ALMAPI       PointInItem(POINT* pt);
    BOOL ALMAPI       ItemInRect(RECT* lprc, BOOL fReqFullEnclosure);
    void ALMAPI       Print(HDC hdc, RECT* lprcPrint, int nPCLMode);
  };


class ShpEllipse :
  public ShpItem

  {
  private:
    LineAttrib  m_line;
    HPEN        m_hpen;

  public:       // creation from existing shape
                ShpEllipse(OBJECTID       oiItem,
                           OBJECTID       oiParent,
                           RECT*          rcItem,
                           long           lStatus,
                           ATTR_LINE*     line);

               ~ShpEllipse(); 

    void        Draw3D(HDC hdc);

    // Overridden ShpItem services
    void        ShapeName(LPSTR lpszName, int nBufLen);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       ExportExtraData();
    void ALMAPI       ImportExtraData();
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       DrawTrackingRect(HDC hdc, RECT* rect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
    void ALMAPI       DrawGrabHandles(HDC hdc);
    void ALMAPI       InvalidateGrabHandles(BOOL fErase);
    BOOL ALMAPI       PointInItem(POINT* pt);
    BOOL ALMAPI       ItemInRect(RECT* lprc, BOOL fReqFullEnclosure);
    void ALMAPI       Print(HDC hdc, RECT* lprcPrint, int nPCLMode);
  };


class ShpRoundRectFill :
  public ShpItemFill

  {
  private:
    LineAttrib  m_line;
    FillAttrib  m_fill;
    POINT       m_ptRadius;
    HBRUSH      m_hbr;
    HPEN        m_hpen;

  public:       // creation from existing shape
                ShpRoundRectFill(OBJECTID       oiItem,
                                 OBJECTID       oiParent,
                                 RECT*          rcItem,
                                 long           lStatus,
                                 ATTR_LINE*     line,
                                 ATTR_FILL*     fill,
                                 short          xRad = -1,  // changes with size
                                 short          yRad = -1); // changes with size

               ~ShpRoundRectFill();

    void        Draw3D(HDC hdc, LPPOINT lppt);

    // Overridden ShpItem services
    void        ShapeName(LPSTR lpszName, int nBufLen);

    // Overridden ShpItemFill methods
    void        GetAttributes(ATTR_LINE* line, ATTR_FILL* fill);
    BOOL        SetAttributes(const ATTR_LINE* line, const ATTR_FILL* fill);
    void        DrawSample(HWND hwnd, LPDRAWITEMSTRUCT lpdi, COLORREF clrFGPen, COLORREF clrBGPen, short nWidth, short nStyle);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       ExportExtraData();
    void ALMAPI       ImportExtraData();
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       DrawTrackingRect(HDC hdc, RECT* rect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
    void ALMAPI       DrawGrabHandles(HDC hdc);
    void ALMAPI       InvalidateGrabHandles(BOOL fErase);
    BOOL ALMAPI       ItemInRect(RECT* lprc, BOOL fReqFullEnclosure);
    void ALMAPI       Print(HDC hdc, RECT* lprcPrint, int nPCLMode);
  };


class ShpRoundRect :
  public ShpItem

  {
  private:
    LineAttrib  m_line;
    POINT       m_ptRadius;
    HPEN        m_hpen;

  public:       // creation from existing shape
                ShpRoundRect(OBJECTID       oiItem,
                             OBJECTID       oiParent,
                             RECT*          rcItem,
                             long           lStatus,
                             ATTR_LINE*     line,
                             short          xRad = -1,
                             short          yRad = -1);

               ~ShpRoundRect();

    void        Draw3D(HDC hdc, LPPOINT lppt);

    // Overridden ShpItem services
    void        ShapeName(LPSTR lpszName, int nBufLen);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       ExportExtraData();
    void ALMAPI       ImportExtraData();
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       DrawTrackingRect(HDC hdc, RECT* rect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
    void ALMAPI       DrawGrabHandles(HDC hdc);
    void ALMAPI       InvalidateGrabHandles(BOOL fErase);
    BOOL ALMAPI       PointInItem(POINT* pt);
    BOOL ALMAPI       ItemInRect(RECT* lprc, BOOL fReqFullEnclosure);
    void ALMAPI       Print(HDC hdc, RECT* lprcPrint, int nPCLMode);
  };


class ShpLine :
  public ShpItem

  {
  private:
    LineAttrib  m_line;
    HPEN        m_hpen;

  public:       // creation from existing shape
                ShpLine(OBJECTID       oiItem,
                        OBJECTID       oiParent,
                        RECT*          rcItem,
                        long           lStatus,
                        ATTR_LINE*     line);

               ~ShpLine();

    void        Draw3D(HDC hdc);

    // Overridden ShpItem services
    void        ShapeName(LPSTR lpszName, int nBufLen);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       ExportExtraData();
    void ALMAPI       ImportExtraData();
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       DrawTrackingRect(HDC hdc, RECT* rect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
    BOOL ALMAPI       PointInItem(POINT* pt);
    BOOL ALMAPI       ItemInRect(RECT* lprc, BOOL fReqFullEnclosure);
    void ALMAPI       DrawGrabHandles(HDC hdc);
    void ALMAPI       InvalidateGrabHandles(BOOL fErase);
    long ALMAPI       PointInGrabHandle(POINT* pt, HCURSOR* phCursor);
    long ALMAPI       AdjustNewRect(RECT* newRect, LONG adjustFlag);
    void ALMAPI       Move(RECT* newRect);
    BOOL ALMAPI       ItemInRegion(HRGN hRegion, BOOL bDeviceUnits);
    void ALMAPI       RemoveItemFromRegion(HRGN hRegion, BOOL bDeviceUnits);
    void ALMAPI       InvalidateItem(BOOL fErase);
    void ALMAPI       Print(HDC hdc, RECT* lprcPrint, int nPCLMode);
  };


class ShpText :
  public ShpItem

  {
  private:
    FontAttrib  m_font;
    short       m_nTextLen;
    WORD        m_nAlign;
    MEMBLOCKID  m_miText;
    char*       m_pText;
    HFONT       m_hfont;
              
  public:       // creation from existing shape
                ShpText(OBJECTID      oiItem,
                        OBJECTID      oiParent,
                        RECT*         rcItem,
                        long          lStatus,
                        ATTR_FONT*    font,
                        WORD          nAlign = DT_LEFT | DT_TOP | DT_WORDBREAK,
                        short         nTextLen = 5,
                        char*         pText = "Text",
                        BOOL          fEditText = TRUE);

               ~ShpText();

    static BOOL CALLBACK EXPORT   TextEntryDlgProc(HWND, UINT, WPARAM, LPARAM);
    static void                   TextEntryCommand(HWND hwnd, int idCtl, HWND hwndCtl, UINT uNtfyCode);

    // Overridden ShpItem services
    void        SizeToShape();
    void        ShapeName(LPSTR lpszName, int nBufLen);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       ExportExtraData();
    void ALMAPI       ImportExtraData();
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
    void ALMAPI       Print(HDC hdc, RECT* lprcPrint, int nPCLMode);
    void ALMAPI       AccelKeyHit();
  };


class ShpGroupBox :
  public ShpItem

  {
  private:
    FontAttrib  m_font;
    LineAttrib  m_line;
    short       m_nTextLen;
    MEMBLOCKID  m_miText;
    char*       m_pText;
    HFONT       m_hfont;
    HPEN        m_hpen;

    RECT        m_rcText;     // rectangle for the groupbox text
    RECT        m_rcFrame;    // rectangle for drawing the frame
    int         m_nLT;        // left top starting point for the frame
    int         m_nRT;        // right top ending point for the frame

  public:       // creation from existing shape
                ShpGroupBox(OBJECTID      oiItem,
                            OBJECTID      oiParent,
                            RECT*         rcItem,
                            long          lStatus,
                            ATTR_FONT*    font,
                            ATTR_LINE*    line,
                            short         nTextLen  = 10,
                            char*         pText     = "Group Box");

               ~ShpGroupBox();

    void        InitDialog(HWND hwnd);
    static BOOL SaveChanges(HWND hwnd);
    static void DrawSample(HWND hwnd, LPDRAWITEMSTRUCT lpdi);
    static BOOL CALLBACK EXPORT EditDlgProc(HWND, UINT, WPARAM, LPARAM);
    static void Command(HWND hwnd, int idCtl, HWND hwndCtl, UINT uNtfyCode);

    void        InitDrawValues();

    // Overridden ShpItem services
    void        ShapeName(LPSTR lpszName, int nBufLen);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       ExportExtraData();
    void ALMAPI       ImportExtraData();
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
    void ALMAPI       Move(RECT* rcNew);
    void ALMAPI       DrawGrabHandles(HDC hdc);
    void ALMAPI       InvalidateGrabHandles(BOOL fErase);
    BOOL ALMAPI       PointInItem(POINT* lppt);
    BOOL ALMAPI       ItemInRect(RECT* lprc, BOOL fReqFullEnclosure);
    void ALMAPI       Print(HDC hdc, RECT* lprcPrint, int nPCLMode);
    void ALMAPI       AccelKeyHit();
  };


class ShpBitmap :
  public ShpItem

  {
  private:
    MEMBLOCKID  m_miBitmap;     // packed DIB
    HBITMAP     m_hbmp;

  public:
                ShpBitmap(OBJECTID      oiItem,
                          OBJECTID      oiParent,
                          RECT*         rcItem,
                          long          lStatus,
                          MEMBLOCKID    miExtra);

               ~ShpBitmap();

    // Overridden ShpItem services
    void        ShapeName(LPSTR lpszName, int nBufLen);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       ExportExtraData();
    void ALMAPI       ImportExtraData();
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
    void ALMAPI       Print(HDC hdc, RECT* lprcPrint, int nPCLMode);
  };

#endif  // #ifndef RCINVOKED

// Miscellaneous structures ////////////////////////////////////////////////

#ifndef RCINVOKED

struct SHPFILL_EDIT
  {
  LPSTR         szTitle;
  short         nCurrentUnits;
  ShpItemFill*  shape;
  };

struct GROUPBOX_EDIT
  {
  ShpGroupBox*  pGroup;
  short         nCurrentUnits;
  };

#endif  // #ifndef RCINVOKED

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef SHP_PROT_H
