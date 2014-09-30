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
// FILE:  "table.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////////////////////

#ifndef TABLE_H // prevent multiple includes
#define TABLE_H

#ifndef __TBL_ATRB_H
#include "tbl_atrb.h"
#endif

#ifndef __TBL_CLMN_H
#include "tbl_clmn.h"
#endif

#ifndef INTDEBUG_H
#include <intdebug.h>
#endif

#ifndef OP_WND_H
#include <op_wnd.h>
#endif

#ifndef O_WND_H
#include <o_wnd.h>
#endif

#ifndef O_PRINT_H
#include <o_print.h>
#endif

#ifndef __TABL_ROW_H
class ROW;
#endif

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned size_t;
#endif

// Message posted by the EDIT control when it receives the keydown
#define EDITM_KEYDOWN   (WM_USER + 100)

// RUNTIMEINFO class definition ////////////////////////////////////////////

class RUNTIMEINFO {
  private:
    ROW*        m_row;

    // These are locks to datas of the table that are kept
    // around -> ref. by ROW class members
    char *  cellData;
    long *  rowOffsets;
    long *  selections;
    AMEMBLOCKID  MemhCFont;
    AMEMBLOCKID  MemhTFont;
    HFONT       *lpHCFont;  // Column Cell hFonts  SKJ May 19, 1994
    HFONT       *lpHTFont;  // Column Title hFonts SKJ May 19, 1994

  public:
    int         m_nFontUseCount;
    BOOL        m_fParentOpen;

  public:
                RUNTIMEINFO(AOBJECTID oid);
               ~RUNTIMEINFO();

    int         AddUser(LPLONG lpRow);
    int         RemoveUser(LPLONG lpRow);
    BOOL        RTimeResize (AOBJECTID theObject, LONG dataIndex, LONG lNewSize);
    BOOL        RTimeSetData(AOBJECTID theObject, LONG dataIndex, LPVOID lpData, LONG lNewSize);
    BOOL        RTimeUnlock (AOBJECTID theObject, LONG dataIndex);

    inline ROW* GetRow()
              {
              return m_row;
              }

    inline char ** GetCellData(AOBJECTID theObject)
              {
              if (cellData == NULL)
                cellData = (char *)AObjLockData(theObject, DATA_CELLDATA);
              return &cellData;
              }
    inline long ** GetRowoffsets(AOBJECTID theObject)
              {
              if (rowOffsets == NULL)
                rowOffsets = (long *)AObjLockData(theObject, DATA_ROWOFFSETS);
              return &rowOffsets;
              }

    inline long ** GetSelections(AOBJECTID theObject)
              {
              if (selections == NULL)
                selections= (long *)AObjLockData(theObject, DATA_SELECTIONS);
              return &selections;
              }

    int         rowObjCount; // Count of Row class instance referencing RTIME

    BOOL        SetHCFont(int index, HFONT hFont);
    BOOL        SetHTFont(int index, HFONT hFont);
    HFONT       GetHCFont(int index);
    HFONT       GetHTFont(int index);

    friend  class COLUMN;
  };

// TABLE class definition //////////////////////////////////////////////////

class TABLE :
  public OpHwndWndItem,  // window Item Protocol class
  private TABLE_ATTRIBUTES,
  private COLUMN
  {
  private:
    AOBJECTID  theObject;

    // window handles
    HWND      hParentWnd;
    HWND      hEdit;
    HWND      hTable;

    AOBJECTID  ParentID;
    int       itemIndex;

  
    HCURSOR   oldCursor;

    // edit control fields
    BOOL      openControl;
    BOOL      dblClkWait;
    WNDPROC   oldEditControlProc;
    HFONT     hEditFont;
    int       editCol;
    long      editRow;

    // tracking flags
    int       tracking;

    // values for determining horizontal scrolling
    int       fixedSpace;   // width of the fixed columns
    int       freeSpace;    // width of the scrollable table space
    int       HScrollSpace; // number of scrolls possible

    // column resize variables
    int       resizeColumn;
    int       startX;
    int       colLineX;
    int       prevLine;


    // variables for selection management
    int       activeColumn;
    long      lastRow;
    int       numOldSelections;
    long*     oldSelections;

    // keep track of visual columns and rows for each visual instance
    // --if there are fixed columns, the first column will be the first
    //   after the fixed columns.... 
    int       firstColumn;
    long      firstRow;
    long      sFlag;

    friend long CALLBACK /*!!PORT!! was _far_ _pascal_*/  EditControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    friend long CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tabl_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  public:

    void      SetParentID  (long oid) { ParentID = (AOBJECTID)oid;}
    void      SetParentWnd(HWND hwnd) { hParentWnd = hwnd;}
    HWND      hHorzScroll;
    HWND      hVertScroll;

    RECT      visRect;  // rectangle for the table's visual instance
    RECT      tblRect;  // rectangle for the table data (visRect - border stuff)

    int       numVisRows; // number of visible rows for the current visual instance

    TABLE(AOBJECTID oiItem,HWND hParent, RECT* rcItem); // called from TABLE_WLAY
    TABLE(AOBJECTID  objID,AOBJECTID oiParent = 0L, RECT* rect=NULL, HWND hWndParent=NULL, long status=0L); // runtime constructor

    ~TABLE();

    BOOL      VertScroll()      {return TABLE_ATTRIBUTES::VertScroll(); }
    BOOL      HorzScroll()      {return TABLE_ATTRIBUTES::HorzScroll(); }
    BOOL      Temporary()       {return ((long)theObject < 0L); }

    // inline methods
    BOOL        AutoFill()                {return TABLE_ATTRIBUTES::AutoFill();}
    BOOL        ShowTitles()              {return TABLE_ATTRIBUTES::ShowTitles();}
    BOOL        CanResize()               {return TABLE_ATTRIBUTES::CanResize();}
    BOOL        CanActivate()             {return TABLE_ATTRIBUTES::CanActivate();}
    int         TotalNumCols()            {return COLUMN::NumColumns();}
    int         Tracking()                {return tracking;}
    int         GetMaxWidth(int i)        {return COLUMN::MaxWidth(i, AutoFill(), TABLE_ATTRIBUTES::FirstRow(), TABLE_ATTRIBUTES::NumLoadedRows());}
    AOBJECTID    ObjectID()                {return theObject;}
    void        UpdateColumns()           {COLUMN::InitColumn(theObject);}
    AOBJECTID    ColObjectID(int i)        {return COLUMN::GetObjectID(i);}
    void        ClearNumVisRows()         {TABLE_ATTRIBUTES::ClearNumVisRows();}
    void        SetTotalNumRows(long i)   {TABLE_ATTRIBUTES::SetTotalNumRows(i);}
    void        SetNumColumns(int i)      {TABLE_ATTRIBUTES::SetNumColumns(i); COLUMN::SetNumColumns(i);}
    void        SetDataSource(AOBJECTID o) {TABLE_ATTRIBUTES::SetDataSource(o);}
    AOBJECTID    FieldIndex()              {return TABLE_ATTRIBUTES::IndexedField();}
    void        SetFieldIndex(AOBJECTID o) {TABLE_ATTRIBUTES::SetIndexedField(o);}
    AOBJECTID    DataSource()              {return TABLE_ATTRIBUTES::DataSource();}
    BOOL        AdHoc()                   {return TABLE_ATTRIBUTES::AdHoc();}
    void        SetStatusFlag(long sf)    {sFlag = sf;}
    long        StatusFlag()              {return sFlag;}
    HWND        GetTableWndHandle()       {return hTable;}

    // methods included in "table1.cpp"
    void        InvalidateTableRect();
    void        InvalidateTitleRect();
    void        UpdateRects(RECT* rect, BOOL runTime);
    void        WIDR_Draw(HDC hDC, RECT* widrRect);
    void        Draw(HDC hDC, RECT* drawRect, BOOL widr, long statusFlag);
    void        DrawFrame(HDC hDC, long statusFlag);
    void        DrawCell(HDC hDC, RECT* rect, int fontAscent, int charWidth, int baseLine, HPCSTR text, UINT flags, BOOL title);
    void        InitFontMetrics();
    void        CreateScrollBars(HWND hWndParent, BOOL runTime);
    void        DestroyScrollBars();
    void        DrawScrollBars(HDC hDC, long statusFlag);
    void        DrawLines(HDC hDC, RECT* TblRect, RECT* drawRect);
    BOOL        IsColumnActive(int col);
    void        ActivateColumn(int col, BOOL clicked);
    int         ColumnWidth(int col);
    char*       ColumnTitle(int col);
    int         CellHeight(long row);
    char*       CellText(long row, int col);
    long        NumRows();
    int         NumColumns();
    UINT        DT_Flags(int col);
    UINT        DT_FlagsTitle(int col);
    char*       Title(int col);
    COLORREF    TitleBGColor(int col);
    COLORREF    TitleFGColor(int col);
    LOGFONT*    TitleFont(int col, int logPixelsY, int width);
    void        CreateFonts();
    void        DestroyFonts();
    int         TitleAscent(int col);
    int         TitleCharWidth(int col);
    COLORREF    CellBGColor(int col);
    COLORREF    CellFGColor(int col);
    LOGFONT*    CellFont(int col, int logPixelsY, int width);
    int         CellAscent(int col);
    int         CellCharWidth(int col);
    void        PostObjectChanged(long modifiers);
    void        PostDoubleClickWait();
    void        UpdateHScrollArea();
    int         FirstColumnToScrollPos();
    int         ScrollPosToFirstColumn(int pos);
    int         FirstRowToScrollPos(long numRows);
    long        ScrollPosToFirstRow(long numRows, int pos);
    int         NumRowsToScrollRange(long numRows);

    // functions in table2.cpp
    int         ColumnIndex(AOBJECTID colObject);
    BOOL        ActivateColumn(AOBJECTID colObject);
    BOOL        ActiveCell(long* rowIndex, AOBJECTID* colObject);
    AOBJECTID    ActiveColumn();
    void        AddRow(AOBJECTID textObject, char* text, AOBJECTID indexObject, long index);
    BOOL        CellEdited(long* rowIndex, AOBJECTID* colObject);
    void        ClearSelections();
    long double ColumnCalculation(int operation, int colIndex, long start, long stop, AOBJECTID resultObject);
    void        DeleteColumn(int colIndex);
    void        DeleteRow(AOBJECTID indexObject, long index);
    BOOL        Match(AOBJECTID oiColumn, ATYPEID colType, AOBJECTID valID, char* value, long valLen, AMEMBLOCKID hTemp, AOBJECTID tmpObj, char * cellPtr, long length, BOOL exact);
    int         Find(int colIndex, ATYPEID colType, long startRow, AOBJECTID valObject, AOBJECTID resultObject, BOOL exact);
    void        GetCell(AOBJECTID columnObject, AOBJECTID indexObject, long index, AMEMBLOCKID theText);
    void        GetColumn(AOBJECTID columnObject, AMEMBLOCKID theText);
    BOOL        GetFullColumn(int colIndex, AMEMBLOCKID theText);
    void        GetColumnTitle(int index, AOBJECTID resultObject);
    int         GetColumnWidth(AOBJECTID columnObject);
    void        GetRow(AOBJECTID indexObject, long index);
    BOOL        tGetSelection(AOBJECTID indexObject, long index, AOBJECTID outputObject);
    void        InsertColumn(int index, AOBJECTID titleObject, AOBJECTID columnObject);
    long double RowCalculation(int operation, int startCol, int endCol, long rowIndex, AOBJECTID resultObject);
    void        RowToText(long start, long stop, AMEMBLOCKID theText);
    BOOL        ScrollToSelection(BOOL redraw = TRUE);
    void        SetCell(AOBJECTID indexObject, long index, AOBJECTID columnObject, AOBJECTID textObject, AMEMBLOCKID hText);
    void        SetColumn(AOBJECTID columnObject, AMEMBLOCKID theText);
    void        SetColumnTitle(int index, AMEMBLOCKID theTitle);
    void        SetColumnWidth(int colIndex, int width);
    void        SetRow(AOBJECTID textObject, char* text, AOBJECTID indexObject, long index);
    void        tSetSelection(long start, AOBJECTID stopObject, long stop);
    long        SortRow(AOBJECTID columnObject, AOBJECTID indexObject, long index, int sortType);
    void        SortTable(AOBJECTID columnObject, int sortType);
    int         CellCompare(ROW* row, int column, ATYPEID theType, void* keys, long row1, AOBJECTID obj1, AMEMBLOCKID hText1, long row2, AOBJECTID obj2, AMEMBLOCKID hText2);
    BOOL        UpdateAutoTable(long  numRows);
    void        AutoUpdate();
    BOOL        ValidIndexedField(AOBJECTID idxObj);
    void        NumRowsChanged();
    void        HScroll(WPARAM wParam, LPARAM lParam);
    void        VScroll(WPARAM wParam, LPARAM lParam);
    void        UpdateVScrollBar(long numRows);
    void        CheckFirstColumn();
    void        DestroyAllEditControls();
    void        DestroySomeEditControls(long row, int colIndex); //PWR 7/13/93 used by SetCell function

    static ALMCALLBACK BOOL DestroyEditControls(OpWndItemD*, LPARAM, LPARAM);

    // functions in table3.cpp
    void        CreateEditControl();
    void        DestroyEditControl();
    void        tCut();
    void        tCopy();
    void        tPaste();
    void        tUndo();

    // methods in "table4.cpp"
    void        Clicked(HWND hWnd, WPARAM wParam, LPARAM lParam);
    void        DoubleClicked(HWND hWnd, LPARAM lParam);
    void        DoubleClickWait();
    void        MouseUp(LPARAM lParam);
    void        KeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL* dispatched);
    void        StartTracking(int trackType);
    void        StopTracking(int trackType);
    void        TrackCursor(WPARAM wParam, LPARAM lParam); // pwr 7/14
    void        DrawNewColLine(int x, BOOL undoPrevious);
    BOOL        CursorInResizeArea(POINT point);
    long        RowFromPoint(POINT point, long numRows);
    BOOL        IsActiveCell(long row, int column);
    BOOL        SelectionsDifferent();
    BOOL        WasSelected(long row);
    void        EscKey();
    void        ReturnKey();
    void        TabKey(BOOL shift);
    BOOL        UpKey(BOOL shift);
    BOOL        DownKey(BOOL shift);
    BOOL        LeftKey(BOOL shift);
    BOOL        RightKey(BOOL shift);
    BOOL        PageUpKey(BOOL control, BOOL shift);
    BOOL        PageDownKey(BOOL control, BOOL shift);
    BOOL        HomeKey(BOOL control, BOOL shift);
    BOOL        EndKey(BOOL control, BOOL shift);
    BOOL        DeleteKey();
    void        DrawAllInstances();
    static ALMCALLBACK BOOL RedrawInstance(OpWndItemD*, LPARAM, LPARAM);
    BOOL        FindCell(LPPOINT lppt, LPLONG plRow, LPINT pnCol);

    // methods in "tbl_prnt.cpp"
    void        WIPR_Draw(HDC hDC, LPRECT lpRect, int pclPrinter);
    void        DrawWIPRBorder(RECT* rect, HDC hDC, long double xScale, long double yScale, int pcl);
    void        PIDR_Draw(PIDR_BLOCK* pidr);
    void        InitPrinterMetrics(HDC hDC);
    void        PrintCell(HDC hDC, RECT* rect, int fontAscent, int charWidth, int baseLine, HPCSTR text, UINT flags, RECT* limitRect, BOOL title);
    void        PrintWIPRLines(HDC hDC, int left, int top, RECT* TblRect, long double xScale, long double yScale, int pcl, int oldTitleHeight, int oldCellHeight);
    void        PrintDoubleVLines(HDC hDC, int pcl, int lWidth, int left, int top, int height);
    void        PrintVLine(HDC hDC, int pcl, int left, int top, int height);
    void        PrintDoubleHLines(HDC hDC, int pcl, int lHeight, int left, int top, int width);
    void        PrintHLine(HDC hDC, int pcl, int left, int top, int width);

    BOOL        SetColumnEditAttrib(int columnIndex,BOOL bEditFlag);  // set the bcanEdit flag on column

    void ALMAPI OSEnvChanged(int style);
    void ALMAPI DataChanged(long details, long modifiers, long variation);
    void ALMAPI ParentOpened();
    void ALMAPI ParentClosing();
    long ALMAPI AdjustNewRect(LPRECT lpRect, LONG flag);
    void ALMAPI Move(LPRECT newRect);
    void ALMAPI Destroy();
    long ALMAPI Activate(int causedBy, BOOL mustKeep);
    void ALMAPI Deactivated(void);
    void ALMAPI MouseEntry(BOOL inside);
    void ALMAPI Cut();
    void ALMAPI Copy();
    void ALMAPI Paste();
    void ALMAPI Undo();
    void  ALMAPI  Print(HDC hdc, LPRECT rect, int pclPrinter);

    // WM_PAINT
    void ALMAPI Tabl_windItemDraw(HDC hDC, RECT *clipRect);
    // WM_HSCROLL: //!!PORT!!
    void CALLBACK /*!!PORT!! was _far_ _pascal_*/ Tabl_windItemHScroll(WPARAM wParam, long lParam);
    // WM_VSCROLL: //!!PORT!!
    void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tabl_windItemVScroll(WPARAM wParam, long lParam);
    // WM_MOUSEMOVE:
    void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tabl_windItemMouseMove(WPARAM wParam, long lParam);
    // WM_LBUTTONDOWN:
    void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tabl_windItemHit(WPARAM wParam, long lParam);
    // WM_LBUTTONUP:
    void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tabl_windMouseUp(WPARAM wParam, long lParam);
    // This should be handled in WM_KEYDOWN:
    void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tabl_windItemKey(WPARAM wParam, long lParam);
    // WM_LBUTTONDBLCLK:
    void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tabl_windItemDoubleClick(WPARAM wParam, long lParam);
    };

typedef TABLE *  LPTABLE;

struct ROWCOL
  {
  long    row;
  int     col;
  TABLE*  table;
  };

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef TABLE_H
