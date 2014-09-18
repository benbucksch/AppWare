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
// FILE:          SHP_ATTR.H
//
// AUTHOR:        Kevin Smart
//
// DESCRIPTION:   Contains definition for shape attributes and implementation
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef SHP_ATTR_H
#define SHP_ATTR_H

// required includes ///////////////////////////////////////////////////////

#ifndef RCINVOKED

#ifndef A_ALM_H
#include  <a_alm.h>
#endif

#ifndef OP_WND_H
#include  <op_wnd.h>
#endif

#endif  // #ifndef RCINVOKED

// defines for the attributes //////////////////////////////////////////////

#ifndef RCINVOKED

#define COLOR_WINDOWFOREGROUND  0xFE000000L
#define COLOR_WINDOWBACKGROUND  0xFF000000L
#define COLOR_TRANSPARENT       0xFD000000L

#define BMP_BRUSHSIZE           8
#define DEF_TWIPSIZE            160

#define ATTR_VERSION            0x0100

#define UNITS_ENGLISH_IN        0       // inches
#define UNITS_METRIC_MM         1       // millimiters
#define UNITS_POINTS_PT         2       // true points (1 pt == 1/72 in)
#define UNITS_POINTS_TWIPS      3       // twips (1 pt == 20 twips)

#define PAT_FOREGROUND          0
#define PAT_BACKGROUND          19

#endif  // #ifndef RCINVOKED

// resource constants //////////////////////////////////////////////////////

#ifndef ID

#ifndef RCINVOKED
#define ID(x)       MAKEINTRESOURCE(x)
#else
#define ID(x)       x
#endif

#endif

#define DLG_LINEEDIT            100
#define DLG_FILLEDIT            200

#define CB_LINEFGCOLOR          110
#define CB_LINEBGCOLOR          111
#define CB_LINESTYLE            112
#define PB_SAMPLELINE           120
#define ED_LINEWIDTH            130
#define SB_LINEWIDTH            140
#define RB_HAIRLINE             150
#define RB_LINEWIDTH            151
#define ST_LINESTYLE            160

#define CB_FILLFGCOLOR          210
#define CB_FILLBGCOLOR          211
#define CB_FILLPATTERN          212
#define PB_SAMPLEFILL           220

// forward class definition ////////////////////////////////////////////////

#ifndef RCINVOKED

class WndData;

// attribute structures for file storage ///////////////////////////////////

struct ATTR_LINE
  {
  short           sVer;
  COLORREF        clrFG;
  COLORREF        clrBG;
  short           nWidth;
  short           nStyle;
  };

struct ATTR_FILL
  {
  short           sVer;
  COLORREF        clrFG;
  COLORREF        clrBG;
  short           nPattern;
  short           pattern[8];
  };

struct ATTR_FONT
  {
  short           sVer;
  COLORREF        clrFG;
  COLORREF        clrBG;
  LOGFONT         lfFont;
  short           nTwips;
  };

// static structure for fill attributes ////////////////////////////////////

struct FillBrushInfo
  {
  int     pattern[8];
  int     nUseCount;
  HBITMAP hbmp;
  HBRUSH  hbr;
  };

// attribute classes for shapes ////////////////////////////////////////////

class ColorAttrib
  {
  private:
    static COLORREF   sm_clr[];
    static char*      sm_szClrName[];
    COLORREF        m_clrFG;
    COLORREF        m_clrBG;

  protected:
    friend WndData;
    inline COLORREF GetTrueFGColor()    {return m_clrFG;}
    inline COLORREF GetTrueBGColor()    {return m_clrBG;}

  public:
                    ColorAttrib(COLORREF clrFG = COLOR_WINDOWFOREGROUND,
                                COLORREF clrBG = COLOR_WINDOWBACKGROUND);
                   ~ColorAttrib();
                
    BOOL            SetFGColor(COLORREF clrFG);
    BOOL            SetBGColor(COLORREF clrBG);
    COLORREF        FGColor();
    COLORREF        BGColor();

    inline BOOL     SetFGTransparent()  {return SetFGColor(COLOR_TRANSPARENT);}
    inline BOOL     SetBGTransparent()  {return SetBGColor(COLOR_TRANSPARENT);}
    inline BOOL     IsFGTransparent()   {return (m_clrFG==COLOR_TRANSPARENT);}
    inline BOOL     IsBGTransparent()   {return (m_clrBG==COLOR_TRANSPARENT);}

    static BOOL     FillColorComboBox(HWND hwndCombo, COLORREF clr, BOOL fFG, BOOL fWantTransparent);
    static void     DrawColorComboBox(HWND hwnd, LPDRAWITEMSTRUCT lpdi);
    static void     MeasureComboBox(HWND hwnd, LPMEASUREITEMSTRUCT lpmi);
  };


class LineAttrib :
  public ColorAttrib

  {
  private:
    static int  sm_nLineStyle[];

    int         m_nWidth; // stored in tenths of a milimeter
    int         m_nStyle;

  public:
                LineAttrib(COLORREF clrFG = COLOR_WINDOWFOREGROUND,
                           COLORREF clrBG = COLOR_TRANSPARENT,
                           int      nWidth = 0,
                           int      nStyle = PS_SOLID);
                LineAttrib(ATTR_LINE* line);
               ~LineAttrib();

    static void         InitLineWidth(HWND hwnd, int);
    static int          GetLineWidthFromDlg(HWND hwnd);
    static void         SetEditFocus(HWND hwnd, UINT idCtl, BOOL fSelectAll);
    static void         WidthVScrollMsg(HWND hwnd, WPARAM wParam, LPARAM lParam, UINT idSampleCtl);
    static void         SetItemValue(HWND hwnd, UINT idCtrl, int nVal);
    static int          GetItemValue(HWND hwnd, UINT idCtrl);
    static void         FillLineStyleComboBox(HWND hwndCombo, int nStyle);
    static void         DrawLineStyleComboBox(HWND hwnd, LPDRAWITEMSTRUCT lpdi);
    static void         DrawSampleLine(HWND hwnd, LPDRAWITEMSTRUCT lpdi);
    static BOOL WINAPI  EditDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    inline int  LineWidth()                 {return m_nWidth;}
    inline int  LineStyle()                 {return m_nStyle;}

    BOOL        SetLineWidth(int nWidth);
    BOOL        SetLineStyle(int nStyle);
    HPEN        CreatePen(OBJECTID oiWnd = NULL);
    BOOL        DeletePen(HPEN hpen);
    BOOL        EditLine(HWND   hwndParent,
                         short  nCurrentUnits, 
                         LPSTR  szTitle = NULL,
                         BOOL   fShowBGColor = TRUE);
    void        PrepForFile(ATTR_LINE* pLine);
  };


class FillAttrib :
  public ColorAttrib

  {
  private:
    static FillBrushInfo  sm_nFillPattern[];

    int           m_nPattern;     // pattern index -- negative if user pattern
    FillBrushInfo m_fbUser;       // user pattern, bitmap, and brush.

  public:
                  FillAttrib(COLORREF clrFG = COLOR_WINDOWFOREGROUND,
                             COLORREF clrBG = COLOR_WINDOWBACKGROUND,
                             int      nPattern = 0,
                             int*     pattern = NULL);
                  FillAttrib(ATTR_FILL* fill);
                 ~FillAttrib();

    static int*         GetFillPattern(int nPattern, FillAttrib* fill=NULL);
    static void         FillPatternComboBox(HWND hwndCombo, int nPattern);
    static void         DrawPatternComboBox(HWND hwnd, LPDRAWITEMSTRUCT lpdi);
    static void         DrawSampleFill(HWND hwnd, LPDRAWITEMSTRUCT lpdi);
    static BOOL WINAPI  EditDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    BOOL          SetFillPattern(int nPattern, int* pattern = NULL);
    HBRUSH        CreateBrush();
    BOOL          DeleteBrush(HBRUSH hbr);
    BOOL          EditFill(HWND   hwndParent,
                           LPSTR  szTitle = NULL);
    void          PrepForFile(ATTR_FILL* pFill);
  };


class FontAttrib :
  public ColorAttrib

  {
  private:
    LOGFONT     m_lfFont;       // store the full logfont structure for simplicity
    int         m_nTwips;       // store the font height in twips 1/20 of a point

  public:
                FontAttrib(LOGFONT* lf = NULL,
                           COLORREF clrFG = COLOR_WINDOWFOREGROUND,
                           COLORREF clrBG = COLOR_TRANSPARENT,
                           int      nTwips = DEF_TWIPSIZE);
                FontAttrib(ATTR_FONT* font);
               ~FontAttrib();

    int         operator==(FontAttrib& font);
    FontAttrib& operator=(FontAttrib& font);

    inline int  FontHeight()                {return m_nTwips;}
    inline void SetFontHeight(int nHeight)  {m_nTwips = nHeight;}
    inline void GetLogfont(LOGFONT* lf)     {if(lf) *lf = m_lfFont;}
    inline void SetLogfont(LOGFONT* lf)     {if(lf) m_lfFont = *lf;}

    HFONT       CreateFont(OBJECTID oiWnd=NULL);  // this will default to the DISPLAY pixels per logical inch
    BOOL        DeleteFont(HFONT hfont);
    BOOL        EditFont(HWND   hwndParent,
                         LPSTR  szTitle = "Edit Font", 
                         LPSTR  szSample = "AaBbYyZz123",
                         BOOL   bShowBG = TRUE,
                         BOOL   bDoEffects = TRUE,
                         BOOL   bPrinterFonts = FALSE);
    void        PrepForFile(ATTR_FONT* pFont);
  };

// structures for dialogs //////////////////////////////////////////////////

struct ATTR_LINEDLG
  {
  LPSTR       szTitle;
  short       nCurrentUnits;
  BOOL        fShowBGColor;
  LineAttrib* line;
  };

struct ATTR_FILLDLG
  {
  LPSTR       szTitle;
  FillAttrib* fill;
  };

#endif  // #ifndef RCINVOKED

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef SHP_ATTR_H
