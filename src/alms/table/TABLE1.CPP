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
// FILE:  "table1.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////////////////////

#define   STRICT
#define   ASTRICT

// id used by scroll window
#define HSCROLL_ID    100
#define VSCROLL_ID    101

#include  <windows.h>
#include  <string.h>
#include  <stdlib.h>

#include  <a_alm.h>
#include  <a_almutl.h>
#include  <intdebug.h>
#include  <op_wnd.h>
#include  <o_wnd.h>
#include  "op_dview.h"
#include  "o_table.h"


#include  "table.h"
#include  "tabl_hdr.h"  // for hInstance
#include  "tabl_rsc.h"
#include  "tabl_row.h"

extern int cxVS;
extern int cyVS;
extern int cxHS;
extern int cyHS;

// Externals ///////////////////////////////////////////////////////////////

extern GLOBAL G;

// TABLE::TABLE ////////////////////////////////////////////////////////////

TABLE::TABLE(
  AOBJECTID    objID,
  HWND        hwndParent,
  RECT*       rect)
  : OpHwndWndItem(objID,0L,rect,0L),
    TABLE_ATTRIBUTES(objID),
    COLUMN(objID)
  {
  theObject = objID;
  hVertScroll = NULL;
  hHorzScroll = NULL;
  hEdit = NULL;
  hTable = NULL;
  oldCursor = NULL;

  tracking = 0;

  firstColumn = TABLE_ATTRIBUTES::NumFixedColumns();
  firstRow = 0;

  TABLE_ATTRIBUTES::SetNumColumns(COLUMN::NumColumns());
  I_UNUSED(hwndParent);
  }

////////////////////////////////////////////////////////////////////////////

TABLE::TABLE(
  AOBJECTID  objID,
  AOBJECTID  oiParent,
  RECT*     rect,
  HWND      hWndParent,
  LONG      status)
  : OpHwndWndItem(objID, oiParent,rect,status),
  TABLE_ATTRIBUTES(objID),
  COLUMN(objID)
  {
  theObject =   objID;
  hParentWnd =  hWndParent;
  ParentID   =  oiParent;
  dblClkWait =  FALSE;

  hVertScroll = NULL;
  hHorzScroll = NULL;
  hEdit = NULL;
  hTable = NULL;
  oldCursor = NULL;

  firstColumn = TABLE_ATTRIBUTES::NumFixedColumns();
  firstRow = 0L;
  tracking = 0;

  TABLE_ATTRIBUTES::SetNumColumns(COLUMN::NumColumns());
  }

// TABLE::~TABLE ///////////////////////////////////////////////////////////

TABLE::~TABLE(
  )
  {
  if(hVertScroll)
    DestroyWindow(hVertScroll);
  if(hHorzScroll)
    DestroyWindow(hHorzScroll);
  if(hEdit)
    DestroyWindow(hEdit);
  if(hTable)
    DestroyWindow(hTable);
  }

// TABLE::InvalidateTableRect //////////////////////////////////////////////

void TABLE::InvalidateTableRect()

  {
  if(hTable)
    {
    InvalidateRect(hTable, &tblRect, FALSE);
    UpdateWindow(hTable);
    }
  }

// TABLE::InvalidateTitleRect //////////////////////////////////////////////

void TABLE::InvalidateTitleRect()

  {
  if(hTable && TABLE_ATTRIBUTES::ShowTitles())
    {
    RECT  rect = tblRect;
    rect.bottom = rect.top + TABLE_ATTRIBUTES::TitleHeight();
    InvalidateRect(hTable, &rect, FALSE);
    }
  }

// TABLE::UpdateRects //////////////////////////////////////////////////////

void TABLE::UpdateRects(
  RECT* rect,
  BOOL  runTime)

  {
  if(rect)
    visRect = *rect;
  else
    SetRect(&visRect, 0, 0, 0, 0);

  // set the visible table data portion
  if(TABLE_ATTRIBUTES::CanActivate())
    {
    if(hTable)
      {
      SetRect(&tblRect,
              TABLE_BORDERWIDTH,
              TABLE_BORDERWIDTH,
	      visRect.right - visRect.left - TABLE_BORDERWIDTH,
              visRect.bottom - visRect.top - TABLE_BORDERWIDTH);
      }
    else
      {
      SetRect(&tblRect,
              visRect.left + TABLE_BORDERWIDTH,
              visRect.top + TABLE_BORDERWIDTH,
              visRect.right - TABLE_BORDERWIDTH,
              visRect.bottom - TABLE_BORDERWIDTH);
      }
    }
  else
    {
    if(hTable)
      {
      SetRect(&tblRect,
              2,
              2,
	      visRect.right - visRect.left - 2,
              visRect.bottom - visRect.top - 2);
      }
    else
      {
      SetRect(&tblRect,
              visRect.left+2,
              visRect.top+2,
              visRect.right-2,
              visRect.bottom-2);
      }
    }
  if(hHorzScroll)
    tblRect.bottom -= (cyHS-1);
  if(hVertScroll)
    tblRect.right -= (cxVS-1);

  // set the number of visible rows and columns
  if(COLUMN::NumColumns()==0)
    {
    numVisRows = 0;

    if(runTime)
      TABLE_ATTRIBUTES::SetNumVisRows(numVisRows);
    }
  else
    {
    int   top = tblRect.top;
    int   rowHeight = TABLE_ATTRIBUTES::CellHeight();

    if(TABLE_ATTRIBUTES::RowLines())
      rowHeight++;
    if(TABLE_ATTRIBUTES::ShowTitles())
      {
      top = tblRect.top + TABLE_ATTRIBUTES::TitleHeight();
      if(TABLE_ATTRIBUTES::RowLines())
        top+=2;
      else
	top++;
      }
    if(rowHeight>0)
      numVisRows = (tblRect.bottom-top)/rowHeight;
    else
      numVisRows = 0;

    if(runTime)
      TABLE_ATTRIBUTES::SetNumVisRows(numVisRows);
    }

  // update the scroll ranges!
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  if(hVertScroll && (numVisRows < row->NumRows()))
    {
    SetScrollRange(hVertScroll, SB_CTL, 0, NumRowsToScrollRange(row->NumRows()), FALSE);
    SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
    EnableWindow(hVertScroll, TRUE);

    if(firstRow>(row->NumRows()-numVisRows))
      firstRow = row->NumRows()-numVisRows;
    }
  else if(hVertScroll)
    {
    firstRow = 0L;
    EnableWindow(hVertScroll, FALSE);
    }

  if(hHorzScroll)
    UpdateHScrollArea();
  }

// TABLE::Draw /////////////////////////////////////////////////////////////

void TABLE::WIDR_Draw(
  HDC   hDC,
  RECT* widrRect)

  {
  int cxhs = cxHS;
  int cxvs = cxVS;
  int cyhs = cyHS;
  int cyvs = cyVS;

  // if the rect is too small clip to the minimum size
  if(TABLE_ATTRIBUTES::VertScroll())
    {
    if((widrRect->right - widrRect->left) < (2 * cxhs + cxvs))
      widrRect->right = widrRect->left + (2 * cxhs + cxvs);
    }

  if(TABLE_ATTRIBUTES::HorzScroll())
    {
    if((widrRect->bottom - widrRect->top) < (2 * cyvs + cyhs))
      widrRect->bottom = widrRect->top + (2 * cyvs + cyhs);
    }

  UpdateRects(widrRect, FALSE);

  // set the status flag to visible to LAYOUT
  Draw(hDC, widrRect, TRUE, SF_VISIBLE);

  if(hVertScroll)
    EnableWindow(hVertScroll, FALSE);
  if(hHorzScroll)
    EnableWindow(hHorzScroll, FALSE);
  }



void TABLE::Draw(
  HDC   hDC,
  RECT* drawRect,
  BOOL  widr,
  long  statusFlag)

  {
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  BOOL          dbAccessed = FALSE;

  if(firstRow>(row->NumRows() - numVisRows))
    firstRow = row->NumRows() - numVisRows;
  if(firstRow<0L)
    firstRow = 0L;

  if(firstColumn<TABLE_ATTRIBUTES::NumFixedColumns())
    firstColumn = TABLE_ATTRIBUTES::NumFixedColumns();

  if(AutoFill()
        &&
     TABLE_ATTRIBUTES::DataSource()
        &&
     (firstRow==0 || firstRow==(row->NumRows()-1L)))
    {
    // check the scroll range
    long  num = -1L;
    if (TABLE_ATTRIBUTES::AdHoc())
      AEvtCallMethod(theObject, TABLE_ATTRIBUTES::DataSource(), NULL,AEVENT_DVIEWGETROWCOUNT, (long)&num);
    else
      AEvtCallMethod(theObject, TABLE_ATTRIBUTES::DataSource(), NULL, autoTableRecordCount, (long)&num);
    if(num<0L)
      num = 0L;

    if(TABLE_ATTRIBUTES::TotalNumRows()!=num)
      {
      TABLE_ATTRIBUTES::SetTotalNumRows(num);

      PostObjectChanged(CHANGED_NUMROWS);
      return;
      }
    }

  // make sure the drawRect is a good region
  RECT      memRect, rect, cellRect, junkRect; // relative coordinates
  rect = *drawRect;

  HPEN      hPen, oldPen;
  HBRUSH    hBrush, oldBrush;
  LOGBRUSH  lb;
  HFONT     hFont, oldFont;

  // draw the frame and scroll bars
  DrawFrame(hDC, statusFlag);
  DrawScrollBars(hDC, statusFlag);

  if(COLUMN::NumColumns()<=0)
    {
    // clear the background
    hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW));
    oldPen = (HPEN)SelectObject(hDC, hPen);
    lb.lbStyle = BS_SOLID;
    lb.lbColor = GetSysColor(COLOR_WINDOW);
    lb.lbHatch = 0;
    hBrush = CreateBrushIndirect(&lb);
    oldBrush = (HBRUSH)SelectObject(hDC, hBrush);
    Rectangle(hDC, tblRect.left, tblRect.top, tblRect.right, tblRect.bottom);
    SelectObject(hDC, oldBrush);
    DeleteObject(hBrush);
    SelectObject(hDC, oldPen);
    DeleteObject(hPen);
    return;
    }
  else if (hTable && !(statusFlag & SF_VISIBLE))  // OWndIsItemVisible(this)))
    {
    ShowWindow(hTable, SW_HIDE);
    return;
    }
  else if(hTable)
    ShowWindow(hTable, SW_SHOW);

  // prepare the memory device for drawing
  SetRect(&memRect, 0, 0, tblRect.right - tblRect.left, tblRect.bottom - tblRect.top);
  CopyRect(&cellRect, &memRect);
  HDC       hMemDC = CreateCompatibleDC(hDC);
  HBITMAP   hBitmap = CreateCompatibleBitmap(hDC, memRect.right, memRect.bottom);
  HBITMAP   oldBitmap =  (HBITMAP)SelectObject(hMemDC, hBitmap);

  // draw the whole background
  hPen = CreatePen(PS_SOLID, 1, COLUMN::CellBGColor(COLUMN::NumColumns()-1));
  oldPen = (HPEN)SelectObject(hMemDC, hPen);
  lb.lbStyle = BS_SOLID;
  lb.lbColor = COLUMN::CellBGColor(COLUMN::NumColumns()-1);
  lb.lbHatch = 0;
  hBrush = CreateBrushIndirect(&lb);
  oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
  Rectangle(hMemDC, memRect.left, memRect.top, memRect.right, memRect.bottom);
  SelectObject(hMemDC, oldBrush);
  DeleteObject(hBrush);
  SelectObject(hMemDC, oldPen);
  DeleteObject(hPen);

  hPen = CreatePen(PS_SOLID, 1, COLUMN::TitleBGColor(COLUMN::NumColumns()-1));
  oldPen = (HPEN)SelectObject(hMemDC, hPen);
  lb.lbStyle = BS_SOLID;
  lb.lbColor = COLUMN::TitleBGColor(COLUMN::NumColumns()-1);
  lb.lbHatch = 0;
  hBrush = CreateBrushIndirect(&lb);
  oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
  Rectangle(hMemDC, memRect.left, memRect.top, memRect.right, TABLE_ATTRIBUTES::TitleHeight()+memRect.top);
  SelectObject(hMemDC, oldBrush);
  DeleteObject(hBrush);
  SelectObject(hMemDC, oldPen);
  DeleteObject(hPen);

  // adjust the 'rect' values -- translate the coordinates
  rect.left -= tblRect.left;
  rect.top -= tblRect.top;
  rect.right -= tblRect.left;
  rect.bottom -= tblRect.top;

  cellRect.left = memRect.left;
  cellRect.right = memRect.left;
  for(int i=0; i<NumColumns() && (cellRect.left < rect.right); i++)
    {
    if(ColumnWidth(i)==0)
      {
      if((i == (TABLE_ATTRIBUTES::NumFixedColumns()-1))
          &&
         (cellRect.left != memRect.left))
        {
        cellRect.left++;
        }
      continue;
      }

    cellRect.top = memRect.top;
    cellRect.right = cellRect.left + ColumnWidth(i);

    if(cellRect.right < rect.left)
      {
      cellRect.left += ColumnWidth(i);
      if(TABLE_ATTRIBUTES::ColumnLines())
        cellRect.left++;
      if((TABLE_ATTRIBUTES::NumFixedColumns()-1) == i)
        cellRect.left++;
      continue;
      }

    if(TABLE_ATTRIBUTES::ShowTitles())
      {
      cellRect.bottom = cellRect.top + TABLE_ATTRIBUTES::TitleHeight();

      if(IntersectRect(&junkRect, &cellRect, &rect))
        {
        if(IsColumnActive(i)
            &&
           TABLE_ATTRIBUTES::ShowSelection()
            &&
           TABLE_ATTRIBUTES::SwapColors()
            &&
           !widr)
          {
          hPen = CreatePen(PS_SOLID, 1, TitleFGColor(i));
          oldPen = (HPEN)SelectObject(hMemDC, hPen);
          lb.lbStyle = BS_SOLID;
          lb.lbColor = TitleFGColor(i);
          lb.lbHatch = 0;
          hBrush = CreateBrushIndirect(&lb);
          oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
          SetBkColor(hMemDC, TitleFGColor(i));
          SetTextColor(hMemDC, TitleBGColor(i));
          }
        else
          {
          hPen = CreatePen(PS_SOLID, 1, TitleBGColor(i));
          oldPen = (HPEN)SelectObject(hMemDC, hPen);
          lb.lbStyle = BS_SOLID;
          lb.lbColor = TitleBGColor(i);
          lb.lbHatch = 0;
          hBrush = CreateBrushIndirect(&lb);
          oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
          SetBkColor(hMemDC, TitleBGColor(i));
          SetTextColor(hMemDC, TitleFGColor(i));
          }

        RUNTIMEINFO *rTime = (RUNTIMEINFO *)AObjGetRuntimeInfo(theObject);

        if(i < TABLE_ATTRIBUTES::NumFixedColumns())
          hFont = rTime->GetHTFont(i);
        else
          hFont = rTime->GetHTFont((i - TABLE_ATTRIBUTES::NumFixedColumns()) + firstColumn);

        oldFont = (HFONT)SelectObject(hMemDC, hFont);

        Rectangle(hMemDC, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom);
        DrawCell(hMemDC, &cellRect, TitleAscent(i),
                 (DT_FlagsTitle(i) & DT_CENTER) ? 1 : CellCharWidth(i),
                 2*TABLE_ATTRIBUTES::TitleLeading()/3 + TABLE_ATTRIBUTES::TitleAscent() + cellRect.top,
                 Title(i), DT_FlagsTitle(i), TRUE);

        SelectObject(hMemDC, oldFont);
        SelectObject(hMemDC, oldBrush);
        DeleteObject(hBrush);
        SelectObject(hMemDC, oldPen);
        DeleteObject(hPen);

        if(IsColumnActive(i)
            &&
           TABLE_ATTRIBUTES::ShowSelection()
            &&
           !widr)
          {
          lb.lbStyle = BS_PATTERN;
          lb.lbColor = 0L;
          lb.lbHatch = (int)G.hBitmap;
          hBrush = CreateBrushIndirect(&lb);
          oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
          RECT  tempRect = cellRect;
          tempRect.top = tempRect.bottom-1;
          FrameRect(hMemDC, &tempRect, hBrush);
          SelectObject(hMemDC, oldBrush);
          DeleteObject(hBrush);
          }
        }

      cellRect.top = cellRect.bottom;
      if(TABLE_ATTRIBUTES::RowLines())
        cellRect.top += 2;
      else
        cellRect.top++;
      } // end...if(TABLE_ATTRIBUTES::ShowTitles())

    // draw the rows
    hPen = CreatePen(PS_SOLID, 1, CellBGColor(i));
    oldPen = (HPEN)SelectObject(hMemDC, hPen);
    lb.lbStyle = BS_SOLID;
    lb.lbColor = CellBGColor(i);
    lb.lbHatch = 0;
    hBrush = CreateBrushIndirect(&lb);
    oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
    RUNTIMEINFO *rTime = (RUNTIMEINFO *)AObjGetRuntimeInfo(theObject);
    if(i < TABLE_ATTRIBUTES::NumFixedColumns())
      hFont = rTime->GetHCFont(i);
    else
      hFont = rTime->GetHCFont((i - TABLE_ATTRIBUTES::NumFixedColumns()) + firstColumn);
    oldFont = (HFONT)SelectObject(hMemDC, hFont);
    Rectangle(hMemDC, cellRect.left, cellRect.top, cellRect.right, memRect.bottom);

    SetBkColor(hMemDC, CellBGColor(i));
    SetTextColor(hMemDC, CellFGColor(i));

    if(widr)
      {
      for(int j=0; j<10 && cellRect.top<memRect.bottom; j++)
        {
        cellRect.bottom = cellRect.top + TABLE_ATTRIBUTES::CellHeight();

        if(IntersectRect(&junkRect, &cellRect, &rect))
          {
          DrawCell(hMemDC, &cellRect, CellAscent(i),
                   CellCharWidth(i),
                   2*TABLE_ATTRIBUTES::CellLeading()/3 + TABLE_ATTRIBUTES::CellAscent() + cellRect.top,
                   "AaBbYyZz", DT_Flags(i), FALSE);
          }

        cellRect.top = cellRect.top + TABLE_ATTRIBUTES::CellHeight();
        if(TABLE_ATTRIBUTES::RowLines())
          cellRect.top++;
        }
      }
    else  // actually run-time
      {
      for(long j=firstRow; j<row->NumRows() && cellRect.top<memRect.bottom && j>=0; j++)
        {
        cellRect.bottom = cellRect.top + TABLE_ATTRIBUTES::CellHeight();

        if(row->IsSelected(j))
          {
          HBRUSH    hBrush1, oldBrush1;
          HPEN      hPen1, oldPen1;
          hPen1 = CreatePen(PS_SOLID, 1, TABLE_ATTRIBUTES::SelectionBG());
          oldPen1 = (HPEN)SelectObject(hMemDC, hPen1);
          LOGBRUSH  lb1 = {BS_SOLID, TABLE_ATTRIBUTES::SelectionBG(), 0};
          hBrush1 = CreateBrushIndirect(&lb1);
          oldBrush1 = (HBRUSH)SelectObject(hMemDC, hBrush1);

          if(j>0 && row->IsSelected(j-1))
            Rectangle(hMemDC, cellRect.left, cellRect.top, memRect.right, cellRect.bottom+1);
          else
            Rectangle(hMemDC, cellRect.left, cellRect.top-1, memRect.right, cellRect.bottom+1);

          SelectObject(hMemDC, oldBrush1);
          DeleteObject(hBrush1);
          SelectObject(hMemDC, oldPen1);
          DeleteObject(hPen1);

          if(hEdit && (TABLE_ATTRIBUTES::ActiveColumn()==((i<TABLE_ATTRIBUTES::NumFixedColumns()) ? i : i+firstColumn-TABLE_ATTRIBUTES::NumFixedColumns())))
            {
            hPen1 = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW));
            oldPen1 = (HPEN)SelectObject(hMemDC, hPen1);
            LOGBRUSH  lb2 = {BS_SOLID, GetSysColor(COLOR_WINDOW), 0};
            hBrush1 = CreateBrushIndirect(&lb2);
            oldBrush1 = (HBRUSH)SelectObject(hMemDC, hBrush1);

            Rectangle(hMemDC, cellRect.left, cellRect.top, cellRect.right, cellRect.bottom);

            SelectObject(hMemDC, oldBrush1);
            DeleteObject(hBrush1);
            SelectObject(hMemDC, oldPen1);
            DeleteObject(hPen1);

            cellRect.top = cellRect.top + TABLE_ATTRIBUTES::CellHeight();
            if(TABLE_ATTRIBUTES::RowLines())
              cellRect.top++;
            continue;
            }

          SetBkColor(hMemDC, TABLE_ATTRIBUTES::SelectionBG());
          SetTextColor(hMemDC, TABLE_ATTRIBUTES::SelectionFG());
          }

        if(IntersectRect(&junkRect, &cellRect, &rect))
          {
          DrawCell(hMemDC, &cellRect, CellAscent(i),
                   CellCharWidth(i),
                   2*TABLE_ATTRIBUTES::CellLeading()/3 + TABLE_ATTRIBUTES::CellAscent() + cellRect.top,
                   row->GetCell(j, (i<TABLE_ATTRIBUTES::NumFixedColumns()) ? i : i+firstColumn-TABLE_ATTRIBUTES::NumFixedColumns(), TRUE, &dbAccessed),
                   DT_Flags(i), FALSE);
          }

        if(row->IsSelected(j))
          {
          if(!TABLE_ATTRIBUTES::AutoFill()
              &&
             TABLE_ATTRIBUTES::CanActivate()
              &&
             TABLE_ATTRIBUTES::ShowActiveCell()
              &&
             IsActiveCell(j, i))
            {
            lb.lbStyle = BS_PATTERN;
            lb.lbColor = 0L;
            lb.lbHatch = (int)G.hBitmap;
            HBRUSH  hBrush1 = CreateBrushIndirect(&lb);
            HBRUSH  oldBrush1 = (HBRUSH)SelectObject(hMemDC, hBrush1);
            FrameRect(hMemDC, &cellRect, hBrush1);
            SelectObject(hMemDC, oldBrush1);
            DeleteObject(hBrush1);
            }

          SetBkColor(hMemDC, CellBGColor(i));
          SetTextColor(hMemDC, CellFGColor(i));
          }

        cellRect.top = cellRect.top + TABLE_ATTRIBUTES::CellHeight();
        if(TABLE_ATTRIBUTES::RowLines())
          cellRect.top++;
        }
      }

    SelectObject(hMemDC, oldFont);
    SelectObject(hMemDC, oldBrush);
    DeleteObject(hBrush);
    SelectObject(hMemDC, oldPen);
    DeleteObject(hPen);

    // reset the localRect
    cellRect.top = memRect.top;
    cellRect.left += ColumnWidth(i);
    if(TABLE_ATTRIBUTES::ColumnLines())
      cellRect.left++;
    if((TABLE_ATTRIBUTES::NumFixedColumns()-1) == i)
      cellRect.left++;
    } // end...for(int i=0; i<numColumns(); ....

  DrawLines(hMemDC, &memRect, &memRect);

  // do the actual display of the table
  BitBlt(hDC, drawRect->left, drawRect->top, rect.right - rect.left, rect.bottom - rect.top,
         hMemDC, rect.left, rect.top, SRCCOPY);
  SelectObject(hMemDC, oldBitmap);
  DeleteObject(hBitmap);
  DeleteDC(hMemDC);

  if(dbAccessed)
    AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);
  }

// TABLE::DrawFrame ////////////////////////////////////////////////////////

void TABLE::DrawFrame(
  HDC     hDC,
  long    statusFlag)

  {
  BOOL    isActive = (StatusFlag() & SF_ACTIVE)? TRUE:FALSE;

  // draw the window frame
  BOOL  newDC = FALSE;
  if(!hDC && hTable)
    {
    newDC = TRUE;
    hDC = GetDC(hTable);
    }
  else if(!hDC)
    {
    newDC = TRUE;
    hDC = GetDC(hParentWnd);
    }

  HBRUSH  hBrush;
  RECT    tmpRect = tblRect;
  tmpRect.left--;
  tmpRect.right++;
  tmpRect.top--;
  tmpRect.bottom++;

  if(statusFlag & SF_VISIBLE)
    hBrush = CreateSolidBrush(COLOR_BLACK);
  else
    hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
  FrameRect(hDC, &tmpRect, hBrush);

  if(TABLE_ATTRIBUTES::CanActivate())
    {
    tmpRect.left-=2;
    tmpRect.right+=2;
    tmpRect.top-=2;
    tmpRect.bottom+=2;
    FrameRect(hDC, &tmpRect, hBrush);

    tmpRect.left++;
    tmpRect.right--;
    tmpRect.top++;
    tmpRect.bottom--;
    if(!isActive && (statusFlag & SF_VISIBLE))
      { 
      DeleteObject(hBrush);
      hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
      }
    if(statusFlag)
      FrameRect(hDC, &tmpRect, hBrush);
    }
  else
    {
    tmpRect.left--;
    tmpRect.right++;
    tmpRect.top--;
    tmpRect.bottom++;
    FrameRect(hDC, &tmpRect, hBrush);
    }

  DeleteObject(hBrush);
  if(newDC && hTable)
    ReleaseDC(hTable, hDC);
  else if(newDC)
    ReleaseDC(hParentWnd, hDC);
  }

// TABLE::DrawCell /////////////////////////////////////////////////////////

void TABLE::DrawCell(
  HDC     hDC,
  RECT*   rect,       // cell rectangle -- text may not fit
  int     fontAscent,
  int     charWidth,
  int     baseLine,
  HPCSTR  text,       // cell string
  UINT    flags,
  BOOL    title)

  {
  SIZE  size;			// string size
  RECT  textRect;
  SetRect(&textRect, rect->left + charWidth, baseLine - fontAscent,
          rect->right - charWidth, rect->bottom);

  int   max = textRect.right - textRect.left;
  int   elipLen = 0;
  int   i = 0;
  int   length = (int)lstrlen(text);
  int   start = 0;
  char* txt = (char*)text;
  BOOL  segProb = FALSE;

  if(!title)
  { GetTextExtentPoint32(hDC, "...", 3, &size);
    elipLen = size.cx;
  } // if
  if(elipLen>max)
    return;

#ifdef UNDEF
  if(SELECTOROF(text) != SELECTOROF(text+(length+1)))
    {
    segProb = TRUE;
    txt = new char[length+1];
    CopyMemory(txt, text, length+1);
    }
#endif

  if(!title)
    {
    GetTextExtentPoint32(hDC, (char*)(txt+start), length, &size);
    while(size.cx+((i==0) ? 0 : elipLen)>max)
      {
      if(flags & DT_RIGHT)
	start++;
      else if(flags & DT_CENTER)
	{
	if(i & 0x0001)
	  start++;
	}

      i++;
      length--;
      GetTextExtentPoint32(hDC, (char*)(txt+start), length, &size);
      }
    }

  char* txtPtr = (char*)txt;
  if(length!=lstrlen(text))
    {
    txtPtr = new char[length+4];
    CopyMemory(txtPtr, text+start, length);
    txtPtr[length]='.';
    txtPtr[length+1]='.';
    txtPtr[length+2]='.';
    txtPtr[length+3]=0;
    GetTextExtentPoint32(hDC, txtPtr, length+3, &size);
    while(size.cx>max)
      {
      length--;
      txtPtr[length]='.';
      txtPtr[length+3]=0;
      GetTextExtentPoint32(hDC, txtPtr, length+3, &size);  
      }
    }

  DrawText(hDC, txtPtr, -1, &textRect, flags);

  if(length!=lstrlen(text))
    delete txtPtr;

  if(segProb)
    delete txt;
  }


// TABLE::CreateFonts()/////////////////////////////////////////////////////
void TABLE::CreateFonts()
  {
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);

  if(0 == rtime->m_nFontUseCount)
    {
    HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
    // make sure the font metrics are valid
    COLUMN::CreateTitleFonts(hDC, VGA_RESOLUTION, FALSE);
    COLUMN::CreateCellFonts(hDC, VGA_RESOLUTION, FALSE);
    DeleteDC(hDC);
    }

  rtime->m_nFontUseCount++;
  }

// TABLE::DestroyFonts //////////////////////////////////////////////////

void TABLE::DestroyFonts()
  {
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);

  rtime->m_nFontUseCount--;

  if(0 == rtime->m_nFontUseCount)
    {
    COLUMN::DestroyTitleFonts();
    COLUMN::DestroyCellFonts();
    }
  }

// TABLE::InitFontMetrics //////////////////////////////////////////////////

void TABLE::InitFontMetrics()

  {
  HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);

  // make sure the font metrics are valid
  COLUMN::TitleMetrics(hDC, VGA_RESOLUTION, FALSE);
  COLUMN::CellMetrics(hDC, VGA_RESOLUTION, FALSE);

  DeleteDC(hDC);

  // initialize the TABLE_ATTRIBUTES font metrics info
  TABLE_ATTRIBUTES::SetTitleLeading(COLUMN::MaxTitleLeading());
  TABLE_ATTRIBUTES::SetTitleAscent(COLUMN::MaxTitleAscent());
  TABLE_ATTRIBUTES::SetTitleDescent(COLUMN::MaxTitleDescent());
  TABLE_ATTRIBUTES::SetCellLeading(COLUMN::MaxCellLeading());
  TABLE_ATTRIBUTES::SetCellAscent(COLUMN::MaxCellAscent());
  TABLE_ATTRIBUTES::SetCellDescent(COLUMN::MaxCellDescent());
  }

// TABLE::CreateScrollBars /////////////////////////////////////////////////

void TABLE::CreateScrollBars(
  HWND  hWndParent,
  BOOL  bRunTime)
  {
  itemIndex = OWndGetItemControlID(this);

  if(bRunTime)       // both during layout and bRunTime
    {
    // stuff this pointer in the window extra bytes
    hTable = CreateWindow(
        "SeriusTable", NULL,
        WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        visRect.left, visRect.top,
        visRect.right - visRect.left,
        visRect.bottom - visRect.top,
        hWndParent, (HMENU)itemIndex, G.hInstance, (LPVOID)this);

    tblRect.left += TABLE_ATTRIBUTES::CanActivate() ? 3 : 2;
    tblRect.top += TABLE_ATTRIBUTES::CanActivate() ? 3 : 2;

    SetProp(hTable, "loPtr", (HANDLE)LOWORD((DWORD)this));
    SetProp(hTable, "hiPtr", (HANDLE)HIWORD((DWORD)this));
    }

  if (VertScroll())
    {
    if(hTable)
      {
      hVertScroll = CreateWindow(
          "scrollbar", NULL,
          WS_CHILD | WS_VISIBLE | SBS_VERT | SBS_RIGHTALIGN,
          0, 0,
          visRect.right - visRect.left,
          HorzScroll() ? (visRect.bottom - visRect.top - cyHS) : (visRect.bottom - visRect.top),
          hTable, (HMENU)HSCROLL_ID, G.hInstance, NULL);
        }
    else
      {
      hVertScroll = CreateWindow(
          "scrollbar", NULL,
          WS_CHILD | SBS_VERT |WS_VISIBLE |SBS_RIGHTALIGN,
          visRect.left, visRect.top,
          visRect.right - visRect.left,
          HorzScroll() ? (visRect.bottom - visRect.top - cyHS) : (visRect.bottom - visRect.top),
          hWndParent, (HMENU)HSCROLL_ID, G.hInstance, NULL);
      }
    }

  if(HorzScroll())
    {
    if(hTable)
      {
      hHorzScroll = CreateWindow(
          "scrollbar", NULL,
          WS_CHILD | WS_VISIBLE | SBS_HORZ | SBS_BOTTOMALIGN,
          0, 0,
          VertScroll() ? (visRect.right - visRect.left - cxVS) : (visRect.right - visRect.left),
          visRect.bottom - visRect.top,
          hTable, (HMENU)2, G.hInstance, NULL);
      }
    else
      {
       hHorzScroll = CreateWindow(
          "scrollbar", NULL,
          WS_CHILD |WS_VISIBLE | SBS_HORZ | SBS_BOTTOMALIGN,
          visRect.left, visRect.top,
          VertScroll() ? (visRect.right - visRect.left - cxVS) : (visRect.right - visRect.left),
          visRect.bottom - visRect.top,
          hWndParent, (HMENU)HSCROLL_ID, G.hInstance, NULL);
      }
    }

  UpdateRects(&visRect, bRunTime);
  if(bRunTime)
    {
    RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
    ROW*          row = rtime->GetRow();

    if(hVertScroll && row->NumRows()<=numVisRows)
      EnableWindow(hVertScroll, FALSE);

    UpdateHScrollArea();
    }
  }

// TABLE::DestroyScrollBars ////////////////////////////////////////////////

void TABLE::DestroyScrollBars()

  {
  if(hVertScroll)
    {
    DestroyWindow(hVertScroll);
    hVertScroll = NULL;
    }

  if(hHorzScroll)
    {
    DestroyWindow(hHorzScroll);
    hHorzScroll = NULL;
    }

  if(hTable)
    {
    DestroyWindow(hTable);
    hTable = NULL;
    }

  UpdateRects(&visRect, TRUE);
  }

// TABLE::DrawScrollBars ///////////////////////////////////////////////////

void TABLE::DrawScrollBars(
  HDC   hDC,
  long  statusFlag)

  {
  if(!hDC)
    {
    if(hTable &&  (statusFlag & SF_VISIBLE))
      {
      ShowWindow(hTable, SW_SHOW);
      UpdateWindow(hTable);
      }
    else if(hTable)
      {
      ShowWindow(hTable, SW_HIDE);
      UpdateWindow(hTable);
      }
    return;
    }

  RECT  tmpRect = visRect;
  if(hTable)
    {
    HDC tmpDC = GetDC(hParentWnd);
    LPtoDP(tmpDC, (LPPOINT)&tmpRect, 2);
    ReleaseDC(hParentWnd, tmpDC);
    }
  else
    LPtoDP(hDC, (LPPOINT)&tmpRect, 2);

  // update the table position
  if(hTable)
    {
    if (statusFlag & SF_VISIBLE)
      ShowWindow(hTable, SW_SHOW);
    else
      ShowWindow(hTable, SW_HIDE);
    if (!hDC) // only update if it is not already in midst of a WM_PAINT
      UpdateWindow(hTable);

    SetRect(&tmpRect, 0, 0, tmpRect.right - tmpRect.left, tmpRect.bottom - tmpRect.top);
    }

  // update the scroll bars
  if(hHorzScroll)
    {
    int fixedWidth = 0;
    for(int i=0; i<TABLE_ATTRIBUTES::NumFixedColumns(); i++)
      {
      if(COLUMN::ColumnWidth(i)==0)
        continue;

      fixedWidth += ColumnWidth(i);
      if(TABLE_ATTRIBUTES::ColumnLines())
        fixedWidth++;
      }
    if((fixedWidth>0))
      fixedWidth++;

    if(fixedWidth>0)
      {
      fixedWidth += 3;
      if(!TABLE_ATTRIBUTES::CanActivate())
        fixedWidth--;
      if(!TABLE_ATTRIBUTES::ColumnLines())
        fixedWidth++;
      LOGBRUSH  lb = {BS_SOLID, GetSysColor(COLOR_SCROLLBAR), NULL};
      HBRUSH    hBrush = CreateBrushIndirect(&lb);
      HBRUSH    oldBrush = (HBRUSH)SelectObject(hDC, hBrush);
      if(hTable)
        Rectangle(hDC, 0, tmpRect.bottom - cyHS, tmpRect.left+fixedWidth-1, tmpRect.bottom);
      else
        {
        if((visRect.left+fixedWidth)>visRect.right)
          Rectangle(hDC, visRect.left, visRect.bottom - cyHS, visRect.right, visRect.bottom);
        else
          Rectangle(hDC, visRect.left, visRect.bottom - cyHS, visRect.left+fixedWidth-1, visRect.bottom);
        }
      SelectObject(hDC, oldBrush);
      DeleteObject(hBrush);
      if(!TABLE_ATTRIBUTES::ColumnLines())
        fixedWidth--;
      }
    else
      fixedWidth = 1;

    MoveWindow(
      hHorzScroll,
      tmpRect.left+fixedWidth-1,
      tmpRect.bottom-cyHS,
      hVertScroll ? (tmpRect.right - tmpRect.left - fixedWidth - cxVS + 2) : (tmpRect.right - tmpRect.left - fixedWidth + 1),
      cyHS,
      TRUE);
    }

  if(hVertScroll)
    {
    MoveWindow(
      hVertScroll,
      tmpRect.right-cxVS,
      tmpRect.top,
      cxVS,
      hHorzScroll ? (tmpRect.bottom - tmpRect.top - cyHS + 1) : (tmpRect.bottom - tmpRect.top),
      TRUE);
    }

  // draw the blank corner if both the scroll bars are visible
  if(hHorzScroll && hVertScroll && (statusFlag & SF_VISIBLE))
    {
    LOGBRUSH  lb = {BS_SOLID, GetSysColor(COLOR_SCROLLBAR), NULL};
    HBRUSH    hBrush = CreateBrushIndirect(&lb);
    HBRUSH    oldBrush = (HBRUSH)SelectObject(hDC, hBrush);
    if(hTable)
      Rectangle(hDC, tmpRect.right - cxVS, tmpRect.bottom - cyHS, tmpRect.right, tmpRect.bottom);
    else
      Rectangle(hDC, visRect.right - cxVS, visRect.bottom - cyHS, visRect.right, visRect.bottom);
    SelectObject(hDC, oldBrush);
    DeleteObject(hBrush);
    }
  I_UNUSED(statusFlag);
  }

// TABLE::DrawLines ////////////////////////////////////////////////////////

void TABLE::DrawLines(
  HDC   hDC,
  RECT* TblRect,
  RECT* drawRect)

  {
  POINT	point;
  if(COLUMN::NumColumns() < 1)
    return;

  // make sure the drawRect is a good region
  RECT  rect;
  IntersectRect(&rect, TblRect, drawRect);

  if(TABLE_ATTRIBUTES::ColumnLines())
    {
    // draw column lines
    int txPos = TblRect->left-1;
    for(int i=0; i<NumColumns(); i++)
      {
      if(ColumnWidth(i)==0)
        {
        if(TABLE_ATTRIBUTES::ColumnLines()
            &&
           (i == (TABLE_ATTRIBUTES::NumFixedColumns()-1))
	    &&
           (txPos != (TblRect->left-1)))
          {
          txPos++;
          }
        continue;
        }

      txPos += (ColumnWidth(i) + 1);
      if(txPos>rect.right)
        break;

      if(txPos<rect.left)
        continue;

      HPEN  hPen, oldPen;
      if(i == (TABLE_ATTRIBUTES::NumFixedColumns()-1))
        {
        txPos++;
	if(!TABLE_ATTRIBUTES::RowLines())
	  {
	  hPen = CreatePen(PS_SOLID, 2, TABLE_ATTRIBUTES::FixedColor());
	  oldPen = (HPEN)SelectObject(hDC, hPen);
          MoveToEx(hDC, txPos, rect.top + 1, &point);
          LineTo(hDC, txPos, rect.bottom - 1);
          SelectObject(hDC, oldPen);
          DeleteObject(hPen);
          }
        }
      else
        {
        hPen = CreatePen(PS_SOLID, 1, TABLE_ATTRIBUTES::LineColor());
        oldPen = (HPEN)SelectObject(hDC, hPen);
	MoveToEx(hDC, txPos, rect.top, &point);
	LineTo(hDC, txPos, rect.bottom);
	SelectObject(hDC, oldPen);
	DeleteObject(hPen);
	}
      } // end....for(int i=0; ...
    }

  if(TABLE_ATTRIBUTES::RowLines())
    {
    // draw row lines
    int tyPos = TblRect->top - 1;
    for(int i=0; TRUE; i++)
      {
      tyPos += (CellHeight(i) + 1);
      if(tyPos>rect.bottom)
	break;

      if(tyPos<rect.top)
	continue;

      HPEN  hPen, oldPen;
      if(i==0 && TABLE_ATTRIBUTES::ShowTitles())
	{
	tyPos++;
	hPen = CreatePen(PS_SOLID, 2, TABLE_ATTRIBUTES::FixedColor());
	oldPen = (HPEN)SelectObject(hDC, hPen);
	MoveToEx(hDC, rect.left + 1, tyPos, &point);
	LineTo(hDC, rect.right - 1, tyPos);
	SelectObject(hDC, oldPen);
	DeleteObject(hPen);
	}
      else
	{
	HPEN  hPen = CreatePen(PS_SOLID, 1, TABLE_ATTRIBUTES::LineColor());
	HPEN  oldPen = (HPEN)SelectObject(hDC, hPen);
	MoveToEx(hDC, rect.left, tyPos, &point);
	LineTo(hDC, rect.right, tyPos);
	SelectObject(hDC, oldPen);
	DeleteObject(hPen);
	}
      } // end....for(int i=0; ...
    }
  else if(TABLE_ATTRIBUTES::ShowTitles())
    {
    HPEN  hPen = CreatePen(PS_SOLID, 1, TABLE_ATTRIBUTES::FixedColor());
    HPEN  oldPen = (HPEN)SelectObject(hDC, hPen);
    MoveToEx(hDC, rect.left, TblRect->top+TABLE_ATTRIBUTES::TitleHeight(), &point);
    LineTo(hDC, rect.right, TblRect->top+TABLE_ATTRIBUTES::TitleHeight());
    SelectObject(hDC, oldPen);
    DeleteObject(hPen);
    }

  // draw the fixed column line
  if(TABLE_ATTRIBUTES::NumFixedColumns()>0)
    {
    int txPos = TblRect->left;
    for(int j=0; j<TABLE_ATTRIBUTES::NumFixedColumns(); j++)
      {
      if(ColumnWidth(j)==0)
        {
        if((j==(TABLE_ATTRIBUTES::NumFixedColumns()-1))
            &&
           (TABLE_ATTRIBUTES::ColumnLines()))
          {
          if(txPos==TblRect->left)
            break;
          txPos--;
          }
        else
          continue;
        }

      txPos += ColumnWidth(j);
      if(TABLE_ATTRIBUTES::ColumnLines())
        txPos++;

      if(txPos>rect.right)
        break;

      if(txPos<rect.left)
        continue;

      if(j!=(TABLE_ATTRIBUTES::NumFixedColumns()-1))
        continue;

      // draw the fixed line
      HPEN  hPen = CreatePen(PS_SOLID, 1, TABLE_ATTRIBUTES::FixedColor());
      HPEN  oldPen = (HPEN)SelectObject(hDC, hPen);
      if(TABLE_ATTRIBUTES::ColumnLines())
        {
	MoveToEx(hDC, --txPos, rect.top, &point);
	LineTo(hDC, txPos, rect.bottom);
	MoveToEx(hDC, ++txPos, rect.top, &point);
	LineTo(hDC, txPos, rect.bottom);
	}
      else
	{
        MoveToEx(hDC, txPos, rect.top, &point);
        LineTo(hDC, txPos, rect.bottom);
        }
      SelectObject(hDC, oldPen);
      DeleteObject(hPen);
      break;
      }
    }
  }

// TABLE::IsColumnActive ///////////////////////////////////////////////////

BOOL TABLE::IsColumnActive(
  int col)

  {
  int actualColumn = (col < TABLE_ATTRIBUTES::NumFixedColumns()) ? col : (col - TABLE_ATTRIBUTES::NumFixedColumns() + firstColumn);
  if(actualColumn == TABLE_ATTRIBUTES::FocusColumn())
    return TRUE;
  else
    return FALSE;
  }

// TABLE::ActivateColumn ///////////////////////////////////////////////////

void TABLE::ActivateColumn(
  int   col,
  BOOL  clicked)

  {
  if(col >= NumColumns())
    return;

  int actualColumn = (col < TABLE_ATTRIBUTES::NumFixedColumns()) ? col : (col - TABLE_ATTRIBUTES::NumFixedColumns() + firstColumn);

  if(actualColumn == TABLE_ATTRIBUTES::FocusColumn())
    return;

  TABLE_ATTRIBUTES::SetFocusColumn(actualColumn);

  if(TABLE_ATTRIBUTES::ShowSelection())
    PostObjectChanged(CHANGED_COLUMNSELECTION);

  if(clicked)
    AEvtPostSignalAtMark(theObject, SIGNAL_SWITCHCOLUMNS);

  }

// TABLE::ColumnWidth //////////////////////////////////////////////////////

int TABLE::ColumnWidth(
  int col)

  {
  if(col >= NumColumns())
    return tblRect.right;

  if(col < TABLE_ATTRIBUTES::NumFixedColumns())
    return COLUMN::Pixels(col);
  else
    return COLUMN::Pixels((col - TABLE_ATTRIBUTES::NumFixedColumns()) + firstColumn);
  }

// TABLE::ColumnTitle //////////////////////////////////////////////////////

char* TABLE::ColumnTitle(
  int col)

  {
  if(col < TABLE_ATTRIBUTES::NumFixedColumns())
    return COLUMN::Title(col);
  else
    return COLUMN::Title((col - TABLE_ATTRIBUTES::NumFixedColumns()) + firstColumn);
  }

// TABLE::CellHeight ///////////////////////////////////////////////////////

int TABLE::CellHeight(
  long  row)

  {
  if(row==0L && TABLE_ATTRIBUTES::ShowTitles())
    return TABLE_ATTRIBUTES::TitleHeight();
  else
    return TABLE_ATTRIBUTES::CellHeight();
  }

// TABLE::NumColumns ///////////////////////////////////////////////////////

int TABLE::NumColumns()

  {
  return TABLE_ATTRIBUTES::NumFixedColumns() + (COLUMN::NumColumns() - firstColumn);
  }

// TABLE::DT_Flags /////////////////////////////////////////////////////////

UINT TABLE::DT_Flags(
  int col)

  {
  if(col < TABLE_ATTRIBUTES::NumFixedColumns())
    return COLUMN::DT_Flags(col);
  else
    return COLUMN::DT_Flags((col - TABLE_ATTRIBUTES::NumFixedColumns()) + firstColumn);
  }

// TABLE::DT_FlagsTitle ////////////////////////////////////////////////////

UINT TABLE::DT_FlagsTitle(
  int col)

  {
  if(col < TABLE_ATTRIBUTES::NumFixedColumns())
    return COLUMN::DT_FlagsTitle(col);
  else
    return COLUMN::DT_FlagsTitle((col - TABLE_ATTRIBUTES::NumFixedColumns()) + firstColumn);
  }

// TABLE::Title ////////////////////////////////////////////////////////////

char* TABLE::Title(
  int col)

  {
  if(col < TABLE_ATTRIBUTES::NumFixedColumns())
    return COLUMN::Title(col);
  else
    return COLUMN::Title((col - TABLE_ATTRIBUTES::NumFixedColumns()) + firstColumn);
  }

// TABLE::TitleBGColor /////////////////////////////////////////////////////

COLORREF TABLE::TitleBGColor(
  int col)

  {
  if(col < TABLE_ATTRIBUTES::NumFixedColumns())
    return COLUMN::TitleBGColor(col);
  else
    return COLUMN::TitleBGColor((col - TABLE_ATTRIBUTES::NumFixedColumns()) + firstColumn);
  }

// TABLE::TitleFGColor /////////////////////////////////////////////////////

COLORREF TABLE::TitleFGColor(
  int col)

  {
  if(col < TABLE_ATTRIBUTES::NumFixedColumns())
    return COLUMN::TitleFGColor(col);
  else
    return COLUMN::TitleFGColor((col - TABLE_ATTRIBUTES::NumFixedColumns()) + firstColumn);
  }

// TABLE::TitleFont ////////////////////////////////////////////////////////

LOGFONT* TABLE::TitleFont(
  int col,
  int logPixelsY,
  int width)

  {
  if(col < TABLE_ATTRIBUTES::NumFixedColumns())
    return COLUMN::TitleFont(col, logPixelsY, width);
  else
    return COLUMN::TitleFont(((col - TABLE_ATTRIBUTES::NumFixedColumns()) + firstColumn), logPixelsY, width);
  }

// TABLE::TitleAscent //////////////////////////////////////////////////////

int TABLE::TitleAscent(
  int col)

  {
  if(col < TABLE_ATTRIBUTES::NumFixedColumns())
    return COLUMN::TitleAscent(col);
  else
    return COLUMN::TitleAscent((col - TABLE_ATTRIBUTES::NumFixedColumns()) + firstColumn);
  }

// TABLE::TitleCharWidth ///////////////////////////////////////////////////

int TABLE::TitleCharWidth(
  int col)

  {
  if(col < TABLE_ATTRIBUTES::NumFixedColumns())
    return COLUMN::TitleCharWidth(col);
  else
    return COLUMN::TitleCharWidth((col - TABLE_ATTRIBUTES::NumFixedColumns()) + firstColumn);
  }

// TABLE::CellBGColor //////////////////////////////////////////////////////

COLORREF TABLE::CellBGColor(
  int col)

  {
  if(col < TABLE_ATTRIBUTES::NumFixedColumns())
    return COLUMN::CellBGColor(col);
  else
    return COLUMN::CellBGColor((col - TABLE_ATTRIBUTES::NumFixedColumns()) + firstColumn);
  }

// TABLE::CellFGColor //////////////////////////////////////////////////////

COLORREF TABLE::CellFGColor(
  int col)

  {
  if(col < TABLE_ATTRIBUTES::NumFixedColumns())
    return COLUMN::CellFGColor(col);
  else
    return COLUMN::CellFGColor((col - TABLE_ATTRIBUTES::NumFixedColumns()) + firstColumn);
  }

// TABLE::CellFont /////////////////////////////////////////////////////////

LOGFONT* TABLE::CellFont(
  int col,
  int logPixelsY,
  int width)

  {
  if(col < TABLE_ATTRIBUTES::NumFixedColumns())
    return COLUMN::CellFont(col, logPixelsY, width);
  else
    return COLUMN::CellFont(((col - TABLE_ATTRIBUTES::NumFixedColumns()) + firstColumn), logPixelsY, width);
  }

// TABLE::CellAscent ///////////////////////////////////////////////////////

int TABLE::CellAscent(
  int col)

  {
  if(col < TABLE_ATTRIBUTES::NumFixedColumns())
    return COLUMN::CellAscent(col);
  else
    return COLUMN::CellAscent((col - TABLE_ATTRIBUTES::NumFixedColumns()) + firstColumn);
  }

// TABLE::CellCharWidth ////////////////////////////////////////////////////

int TABLE::CellCharWidth(
  int col)

  {
  if(col < TABLE_ATTRIBUTES::NumFixedColumns())
    return COLUMN::CellCharWidth(col);
  else
    return COLUMN::CellCharWidth((col - TABLE_ATTRIBUTES::NumFixedColumns()) + firstColumn);
  }

// TABLE::PostObjectChanged ////////////////////////////////////////////////

void TABLE::PostObjectChanged(
  long  modifiers)

  {
  AEvtInfo  event;
  FillMemory(&event, sizeof(AEvtInfo), 0);

  event.sourceObject = theObject;
  event.targetObject = theObject;
  event.when         = AAppGetTime();
  event.how          = AEVT_KILLMODIFIERS | AEVT_KILLDUPLICATES | AEvtGetObjectChangedPriority(AEVT_ATTAIL);
  event.what         = AEVENT_OBJECTCHANGED;
  event.modifiers    = modifiers;
  AEvtPost(&event);
  }

// TABLE::PostDoubleClickWait //////////////////////////////////////////////

void TABLE::PostDoubleClickWait()

  {
  AEvtInfo  event;
  event.sourceObject = theObject;
  event.targetObject = theObject;
  event.when         = AAppGetTime() + G.DblClkSpeed;
  event.how          = AEVT_KILLDUPLICATES | AEVT_ATTAIL | AEVT_DIRECT;
  event.what         = (AEVENTID)doubleClickWait;
  event.details      = (long)this;
  AEvtPost(&event);
  }

// TABLE::UpdateHScrollArea ////////////////////////////////////////////////

void TABLE::UpdateHScrollArea()

  {
  if(COLUMN::NumColumns()==0)
    {
    HScrollSpace=0;
    if(hHorzScroll)
      EnableWindow(hHorzScroll, FALSE);
    return;
    }

  // get the available space for the different parts of the table
  fixedSpace = 0;
  for(int i=0; i<TABLE_ATTRIBUTES::NumFixedColumns(); i++)
    {                                
    if(COLUMN::ColumnWidth(i)==0)
      {
      if((i==(TABLE_ATTRIBUTES::NumFixedColumns()-1))
          &&
         (fixedSpace!=0))
        {
        fixedSpace++;
        }
      continue;
      }

    fixedSpace += COLUMN::ColumnWidth(i);
    if(TABLE_ATTRIBUTES::ColumnLines())
      fixedSpace++;
    if(i==(TABLE_ATTRIBUTES::NumFixedColumns()-1))
      fixedSpace++;
    }
  freeSpace = (tblRect.right - tblRect.left) - fixedSpace;

  // determine the scroll range for the table
  HScrollSpace = COLUMN::NumColumns() - TABLE_ATTRIBUTES::NumFixedColumns();

  int widthSum = 0;
  for(i=COLUMN::NumColumns()-1; i>=TABLE_ATTRIBUTES::NumFixedColumns() && widthSum==0; i--)
    {
    HScrollSpace--;
    widthSum += COLUMN::ColumnWidth(i);
    } 

  for( ; i>=TABLE_ATTRIBUTES::NumFixedColumns(); i--)
    {
    if(COLUMN::ColumnWidth(i)==0)
      {
      HScrollSpace--;
      continue;
      }

    widthSum += COLUMN::ColumnWidth(i);
    if(TABLE_ATTRIBUTES::ColumnLines())
      widthSum++;

    if(widthSum<=freeSpace)
      HScrollSpace--;
    }

  // make sure firstColumn has width
  while(COLUMN::ColumnWidth(firstColumn)==0)
    {
    if(firstColumn==(COLUMN::NumColumns()-1))
      break;

    firstColumn++;
    }

  skipCounting:
  if(hHorzScroll && HScrollSpace>0)
    {
    EnableWindow(hHorzScroll, TRUE);
    SetScrollRange(hHorzScroll, SB_CTL, 0, HScrollSpace, FALSE);
    SetScrollPos(hHorzScroll, SB_CTL, FirstColumnToScrollPos(), TRUE);
    }
  else if(hHorzScroll)
    {
    firstColumn = 0;
    EnableWindow(hHorzScroll, FALSE);
    }
  }

// TABLE::FirstColumnToScrollPos ///////////////////////////////////////////

int TABLE::FirstColumnToScrollPos()

  {
  int pos=0;

  for(int i=TABLE_ATTRIBUTES::NumFixedColumns(); i<firstColumn; i++)
    {
    if(COLUMN::ColumnWidth(i)>0)
      pos++;
    }

  return pos;
  }

// TABLE::ScrollPosToFirstColumn ///////////////////////////////////////////

int TABLE::ScrollPosToFirstColumn(
  int pos)

  {
  int temp=0;
  for(int first=TABLE_ATTRIBUTES::NumFixedColumns(); firstColumn<COLUMN::NumColumns() && (temp!=pos || COLUMN::ColumnWidth(first)==0); first++)
    {
    if(COLUMN::ColumnWidth(first)==0)
      continue;

    temp++;
    }

  return first;
  }

// TABLE::FirstRowToScrollPos //////////////////////////////////////////////

int TABLE::FirstRowToScrollPos(
  long  numRows)

  {
  long  div = 1L;
  while(((numRows-numVisRows)/div)>0x00007FFFL)
    div++;

  return (int)(firstRow/div);
  }

// TABLE::ScrollPosToFirstRow //////////////////////////////////////////////

long TABLE::ScrollPosToFirstRow(
  long  numRows,
  int   pos)

  {
  long  div = 1L;
  while(((numRows-numVisRows)/div)>0x00007FFFL)
    div++;

  if((int)((numRows-numVisRows)/div)==pos)
    return (long)(numRows-numVisRows);
  else
    return (long)(div*pos);
  }

// TABLE::NumRowsToScrollRange /////////////////////////////////////////////

int TABLE::NumRowsToScrollRange(
  long  numRows)

  {
  long  div = 1L;
  while (((numRows-numVisRows)/div)>0x00007FFFL)
    div++;

  return (int)((numRows-numVisRows)/div);
  }


////////////////////////////////////////////////////////////////////////////
