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
// FILE:        SHAPES.H
//
// AUTHOR:      Scott McCarty (converted from old Window Shapes)
//
// DESCRIPTION: Contains standard shapes class definition
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef SHAPES_H
#define SHAPES_H

#ifndef OP_WND_H
#include <op_wnd.h>
#endif  // !OP_WND_H

#ifndef ATTRIBUT_H
#include <attribut.h>
#endif  // !ATTRIBUT_H

// Because of the RC compiler's stupidity, we have to list the
// IDs explicitly.  We also define a useful ID() macro.

#ifndef RCINVOKED
#define ID(x)       MAKEINTRESOURCE(x)
#else
#define ID(x)       x
#endif

////////////////////////////////////////////////////////////////
//
// Resource ids
//
////////////////////////////////////////////////////////////////

#define DLG_TEXTENTRY         101
#define DLG_TEXT_ENTRY        10
#define CUR_LINEMOVE          5000

////////////////////////////////////////////////////////////////
//
// Supported shapes
//
////////////////////////////////////////////////////////////////

#ifndef RCINVOKED

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

//////////////// Forward class references

class ShapePalette;

////////////////////////////////////////////////////////////////
//
// STRUCT:  ShapeExtra
//
// DESCR:   Contains shape information that is to be saved by the Window
//          object.
//
////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////
//
// CLASS:   WindowShape
//
// DESCR:   Implements a Window Item that draws various shapes.
//
////////////////////////////////////////////////////////////////

class WindowShape : public OpLiteWndItem
  {
  private:
    short         m_iType;            // type of shape:  SHP_*
    WORD          m_wPenStyle;        // PS_* from windows.h
    WORD          m_wPenWidth;        // # of pixels
    WORD          m_wBrushStyle;      // BS_* from windows.h
    WORD          m_wBrushHatch;      // HS_* from windows.h
    int           m_nAlign;           // align options for DrawText()
    ATTRIBUTES    m_Attributes;       // fg, bg, font, ...
    union
      {
      char            szName[100];
      unsigned char   bPattern[100];
      }           m_uExtra;


    // Dialog methods
    static BOOL CALLBACK    TextEntryDlgProc(HWND, UINT, WPARAM, LPARAM);
    BOOL                    TextEntry(void);

    // Helper methods
    HBRUSH        CreateTheBrush(BOOL forceHollow = FALSE);
    HPEN          CreateThePen(BOOL fForceHollow, double rFactor);
    void          DrawAux(HDC hdc, double rFactor);

    // Protect them from being deleted outside of Destroy() method
    void          operator delete(void*)    {}
                 ~WindowShape(void)         {}

  public:

                  WindowShape(OBJECTID, OBJECTID, RECT*, long, MEMBLOCKID,
                                ShapePalette*);

    void          Init(WORD           nType,
                        WORD          wPenStyle,
                        WORD          wPenWidth,
                        WORD          wBrushStyle,
                        WORD          wBrushHatch,
                        WORD          nAlign,
                        LPATTRIBUTES  pAttribs,
                        LPVOID        pShapeExtra);

    //////////////// Window item accessor and setting methods

    int     GetType(void)             {return m_iType;}
    void    SetType(int t)            {m_iType = t;}

    WORD    GetPenStyle(void)         {return m_wPenStyle;}
    WORD    GetPenWidth(void)         {return m_wPenWidth;}

    void    SetPenStyle(WORD s)       {m_wPenStyle = s;}
    void    SetPenWidth(WORD w)       {m_wPenWidth = w;}

    int     GetBrushNum(void)         {return m_wBrushHatch;}
    int     GetPCLBrushNum(void)      {return GetBrushNum() + 1;}

    void    SetBrushNum(WORD n)       {m_wBrushHatch = n;}
    void    SetBrushType(WORD t)      {m_wBrushStyle = t;}
    void    SetBrushSolid(BOOL s)     {if (s) m_wBrushStyle = BS_SOLID;}

    BOOL    IsBrushSolid(void)        {return (m_wBrushStyle == BS_SOLID);}
    BOOL    IsShapeTransparent(void)  {return (m_iType != SHP_RECTFILL || !IsBrushSolid());}

    void    ShapeName(LPSTR, int);

    //////////////// Useful functions

    void    SizeToShape(void);                    // make min size for text

    //////////////// Overridden protocol services

    void ALMAPI       Destroy(void);            // MUST override destroy!
    LONG ALMAPI       Edit(BOOL);               // for editing group box & text
    BOOL ALMAPI       PointInItem(POINT* pt);
    BOOL ALMAPI       ItemInRect(RECT* rect, RECT* intersectRect);
    void ALMAPI       DrawTrackingRect(HDC hdc, RECT* rect);
    void ALMAPI       DrawGrabHandles(HDC hdc);
    void ALMAPI       InvalidateGrabHandles(void);
    long ALMAPI       PointInGrabHandle(POINT* pt, HCURSOR* phCursor);
    MEMBLOCKID ALMAPI GetExtraData(void);
    void ALMAPI       Draw(HDC hdc, RECT* invalidRect);
    long ALMAPI       AdjustNewRect(RECT* newRect, LONG adjustFlag);
    void ALMAPI       Move(RECT* newRect);
    BOOL ALMAPI       ItemInRegion(HRGN hRegion, BOOL bDeviceUnits);
    void ALMAPI       RemoveItemFromRegion(HRGN hRegion, BOOL bDeviceUnits);
    LONG ALMAPI       QueryProtocol(pApQueryInfo pqi, PROTOCOLID piProt);
  };

#endif  // !RCINVOKED

#endif  // !SHAPES_H
