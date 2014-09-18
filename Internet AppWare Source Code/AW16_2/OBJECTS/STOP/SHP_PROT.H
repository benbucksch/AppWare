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

#endif  // #ifndef RCINVOKED

#ifndef SHP_ATTR_H
#include  "shp_attr.h"
#endif

// forward class references ////////////////////////////////////////////////

#ifndef RCINVOKED
class ShapePalette;
#endif

// old file structure DELETE ASAP!!!! //////////////////////////////////////

#ifndef RCINVOKED

#ifndef ATTRIBUT_H_
#include  "attribut.h"
#endif

typedef struct _ShapeExtra
  {
  short         nType;
  WORD          wPenStyle;
  WORD          wPenWidth;
  WORD          wBrushStyle;
  WORD          wBrushHatch;
  short         nAlign;
  ATTRIBUTES    Attributes;

  union
    {
    char            szName[100];    // text for text shape
    unsigned char   bPattern[100];  // bitmap for pattern data
    }           uExtra;             // extra shape-specific data

  union
    {
    }           uShapeSpecData;     // shape-specific information
  }   ShapeExtra;

#endif

// file structures for shapes //////////////////////////////////////////////

#ifndef RCINVOKED

struct SHAPE_BASE
  {
  short       sVer;
  short       sType;
  };

struct SHAPE_RECT
  {
  SHAPE_BASE  base;
  ATTR_LINE   line;
  };

struct SHAPE_RECTFILL
  {
  SHAPE_BASE  base;
  ATTR_LINE   line;
  ATTR_FILL   fill;
  };

struct SHAPE_ROUNDRECT
  {
  SHAPE_BASE  base;
  ATTR_LINE   line;
  POINT       ptRadius;
  };

struct SHAPE_ROUNDRECTFILL
  {
  SHAPE_BASE  base;
  ATTR_LINE   line;
  ATTR_FILL   fill;
  POINT       ptRadius;
  };

struct SHAPE_ELLIPSE
  {
  SHAPE_BASE  base;
  ATTR_LINE   line;
  };

struct SHAPE_ELLIPSEFILL
  {
  SHAPE_BASE  base;
  ATTR_LINE   line;
  ATTR_FILL   fill;
  };

struct SHAPE_LINE
  {
  SHAPE_BASE  base;
  ATTR_LINE   line;
  };

struct SHAPE_TEXT
  {
  SHAPE_BASE  base;
  ATTR_FONT   font;
  int         nTextLen;
  UINT        nAlign;
  char        szText[1];      // actually this is a long as the text
  };

struct SHAPE_GROUPBOX
  {
  SHAPE_BASE  base;
  ATTR_FONT   font;
  ATTR_LINE   line;
  int         nTextLen;
  char        szText[1];      // actually this is a long as the text
  };

struct SHAPE_BITMAP
  {
  SHAPE_BASE  base;
  long        lLength;        // size of the bitmap
  BITMAPINFO  bi;             // a packed DIB
  };

#endif

// Resource ids ////////////////////////////////////////////////////////////

#define DLG_FILLSHAPE         300
#define PB_SAMPLE             310

#define DLG_TEXTENTRY         101
#define DLG_FONT              200
#define DLG_TEXT_ENTRY        10
#define CUR_LINEMOVE          5000

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

////////////////////////////////////////////////////////////////////////////

#ifndef RCINVOKED

class ShpItem :
  public OpLiteWndItem

  {
  private:
    short m_nShapeType;

  public:
                  ShpItem(OBJECTID  oiItem,
                          OBJECTID  oiParent,
                          RECT*     rcItem,
                          long      lStatus,
                          short     nShapeType);

                 ~ShpItem();

    // virtual functions for the shapes
    virtual void  SizeToShape();
    virtual void  ShapeName(LPSTR lpszName, int nBufLen) = 0;

    // overridden window item protocol methods general to all shapes
    virtual LONG ALMAPI QueryProtocol(pApQueryInfo pqi, PROTOCOLID piProt);
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

    // virtual functions for the filled shapes
    virtual void  GetAttributes(ATTR_LINE* line, ATTR_FILL* fill) = 0;
    virtual BOOL  SetAttributes(const ATTR_LINE* line, const ATTR_FILL* fill) = 0;
    virtual void  DrawSample(HWND hwnd, LPDRAWITEMSTRUCT lpdi) = 0;
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

    // Overridden ShpItem services
    void        ShapeName(LPSTR lpszName, int nBufLen);

    // Overridden ShpItemFill methods
    void        GetAttributes(ATTR_LINE* line, ATTR_FILL* fill);
    BOOL        SetAttributes(const ATTR_LINE* line, const ATTR_FILL* fill);
    void        DrawSample(HWND hwnd, LPDRAWITEMSTRUCT lpdi);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
    void ALMAPI       DrawGrabHandles(HDC hdc);
    void ALMAPI       InvalidateGrabHandles(BOOL fErase);
//    void ALMAPI       WinIniChanged(char* section);
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
              
    // Overridden ShpItem services
    void        ShapeName(LPSTR lpszName, int nBufLen);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
    void ALMAPI       DrawGrabHandles(HDC hdc);
    void ALMAPI       InvalidateGrabHandles(BOOL fErase);
    BOOL ALMAPI       PointInItem(POINT* lppt);
    BOOL ALMAPI       ItemInRect(RECT* lprc, BOOL fReqFullEnclosure);
//    void ALMAPI       WinIniChanged(char* section);
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

    // Overridden ShpItem services
    void        ShapeName(LPSTR lpszName, int nBufLen);

    // Overridden ShpItemFill methods
    void        GetAttributes(ATTR_LINE* line, ATTR_FILL* fill);
    BOOL        SetAttributes(const ATTR_LINE* line, const ATTR_FILL* fill);
    void        DrawSample(HWND hwnd, LPDRAWITEMSTRUCT lpdi);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       DrawTrackingRect(HDC hdc, RECT* rect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
    void ALMAPI       DrawGrabHandles(HDC hdc);
    void ALMAPI       InvalidateGrabHandles(BOOL fErase);
    BOOL ALMAPI       PointInItem(POINT* pt);
    BOOL ALMAPI       ItemInRect(RECT* lprc, BOOL fReqFullEnclosure);
//    void ALMAPI       WinIniChanged(char* section);
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

    // Overridden ShpItem services
    void        ShapeName(LPSTR lpszName, int nBufLen);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       DrawTrackingRect(HDC hdc, RECT* rect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
    void ALMAPI       DrawGrabHandles(HDC hdc);
    void ALMAPI       InvalidateGrabHandles(BOOL fErase);
    BOOL ALMAPI       PointInItem(POINT* pt);
    BOOL ALMAPI       ItemInRect(RECT* lprc, BOOL fReqFullEnclosure);
//    void ALMAPI       WinIniChanged(char* section);
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
                                 int            xRad = -1,  // changes with size
                                 int            yRad = -1); // changes with size

               ~ShpRoundRectFill();

    // Overridden ShpItem services
    void        ShapeName(LPSTR lpszName, int nBufLen);

    // Overridden ShpItemFill methods
    void        GetAttributes(ATTR_LINE* line, ATTR_FILL* fill);
    BOOL        SetAttributes(const ATTR_LINE* line, const ATTR_FILL* fill);
    void        DrawSample(HWND hwnd, LPDRAWITEMSTRUCT lpdi);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       DrawTrackingRect(HDC hdc, RECT* rect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
    void ALMAPI       DrawGrabHandles(HDC hdc);
    void ALMAPI       InvalidateGrabHandles(BOOL fErase);
    BOOL ALMAPI       ItemInRect(RECT* lprc, BOOL fReqFullEnclosure);
//    void ALMAPI       WinIniChanged(char* section);
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
                             int            xRad = -1,
                             int            yRad = -1);

               ~ShpRoundRect();

    // Overridden ShpItem services
    void        ShapeName(LPSTR lpszName, int nBufLen);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       DrawTrackingRect(HDC hdc, RECT* rect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
    void ALMAPI       DrawGrabHandles(HDC hdc);
    void ALMAPI       InvalidateGrabHandles(BOOL fErase);
    BOOL ALMAPI       PointInItem(POINT* pt);
    BOOL ALMAPI       ItemInRect(RECT* lprc, BOOL fReqFullEnclosure);
//    void ALMAPI       WinIniChanged(char* section);
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

    // Overridden ShpItem services
    void        ShapeName(LPSTR lpszName, int nBufLen);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
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
//    void ALMAPI       WinIniChanged(char* section);
  };


class ShpText :
  public ShpItem

  {
  private:
    FontAttrib  m_font;
    short       m_nTextLen;
    UINT        m_nAlign;
    MEMBLOCKID  m_miText;
    char*       m_pText;
    HFONT       m_hfont;
              
  public:       // creation from existing shape
                ShpText(OBJECTID      oiItem,
                        OBJECTID      oiParent,
                        RECT*         rcItem,
                        long          lStatus,
                        ATTR_FONT*    font,
                        UINT          nAlign = DT_LEFT | DT_TOP | DT_WORDBREAK,
                        int           nTextLen = 5,
                        char*         pText = "Text",
                        BOOL          fEditText = TRUE);

               ~ShpText();

    static BOOL CALLBACK _export  TextEntryDlgProc(HWND, UINT, WPARAM, LPARAM);

    // Overridden ShpItem services
    void        SizeToShape();
    void        ShapeName(LPSTR lpszName, int nBufLen);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
  };


class ShpGroupBox :
  public ShpItem

  {
  private:
    FontAttrib  m_font;
    LineAttrib  m_line;
    int         m_nTextLen;
    MEMBLOCKID  m_miText;
    char*       m_pText;
    HFONT       m_hfont;
    HPEN        m_hpen;

  public:       // creation from existing shape
                ShpGroupBox(OBJECTID      oiItem,
                            OBJECTID      oiParent,
                            RECT*         rcItem,
                            long          lStatus,
                            ATTR_FONT*    font,
                            ATTR_LINE*    line,
                            int           nTextLen = 10,
                            char*         pText = "Group Box");

               ~ShpGroupBox();

    // Overridden ShpItem services
    void        ShapeName(LPSTR lpszName, int nBufLen);
    
    // Overridden window item protocol services
    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
    void ALMAPI       DrawGrabHandles(HDC hdc);
    void ALMAPI       InvalidateGrabHandles(BOOL fErase);
//    void ALMAPI       WinIniChanged(char* section);
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
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    void ALMAPI       ParentOpened();
    void ALMAPI       ParentClosing();
  };

#endif  // #ifndef RCINVOKED

// Miscellaneous structures ////////////////////////////////////////////////

#ifndef RCINVOKED

struct SHPFILL_EDIT
  {
  LPSTR         szTitle;
  int           nCurrentUnits;
  ShpItemFill*  shape;
  };

#endif  // #ifndef RCINVOKED

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef SHP_PROT_H
