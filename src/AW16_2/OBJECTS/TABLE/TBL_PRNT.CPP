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
// FILE:  "tbl_prnt.cpp"
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

// Include files ///////////////////////////////////////////////////////////

#include <windows.h>
#include <string.h>

#include <a_alm.h>
#include <a_almutl.h>
#include <op_wnd.h>
#include <o_wnd.h>
#include <o_print.h>

#include  "tabl_hdr.h"
#include  "table.h"
#include  "tabl_row.h"

extern int cxVS;
extern int cyVS;
extern int cxHS;
extern int cyHS;

// TABLE::WIPR_Draw ////////////////////////////////////////////////////////

void TABLE::WIPR_Draw(
  HDC     hDC,
  LPRECT  lpRect,
  int     pclPrinter)

  {
  int         oldTitleHeight = TABLE_ATTRIBUTES::TitleHeight();
  int         oldCellHeight = TABLE_ATTRIBUTES::CellHeight();
  POINT       DPI;

  InitPrinterMetrics(hDC);
  DPI.x = GetDeviceCaps(hDC, LOGPIXELSX);
  DPI.y = GetDeviceCaps(hDC, LOGPIXELSY);
  long double   xScale = (long double)DPI.x / (long double)VGA_RESOLUTION;
  long double   yScale = (long double)DPI.y / (long double)VGA_RESOLUTION;
  RECT          rect = *lpRect;
  RECT          cellRect; 
  int           col;
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  HFONT         hFont, oldFont;
  int           xScreen;
  int           yScreen;
  BOOL          dbAccessed = FALSE;
  int           width;

  if(hVertScroll)
    rect.right -= (int)(cxVS * xScale);

  if(TABLE_ATTRIBUTES::CanActivate())
    xScreen = 2;
  else
    xScreen = 3;

  rect.left += (int)(xScreen * xScale);
  rect.top += (int)(xScreen * yScale);
  rect.right -= (int)(xScreen * xScale);
  rect.bottom -= (int)(xScreen * yScale);
  cellRect = rect;

  // make sure the colors are black on white
  COLORREF    oldFGColor = SetTextColor(hDC, COLOR_BLACK);
  COLORREF    oldBGColor = SetBkColor(hDC, COLOR_WHITE);

  for(int i=0; i<NumColumns() && cellRect.left<rect.right; i++)
    {
    col = (i < TABLE_ATTRIBUTES::NumFixedColumns()) ? i : (i - TABLE_ATTRIBUTES::NumFixedColumns() + firstColumn);
    if(COLUMN::ColumnWidth(col)==0)
      {
      if(i==(TABLE_ATTRIBUTES::NumFixedColumns()-1))
        {
        if(cellRect.left != rect.left)
          {
          xScreen++;
          cellRect.left++;
          }
        }
      continue;
      }

    xScreen += COLUMN::ColumnWidth(col);
    if(i==(TABLE_ATTRIBUTES::NumFixedColumns()-1))
      xScreen+=2;
    else if(TABLE_ATTRIBUTES::ColumnLines())
      xScreen++;

    cellRect.right = lpRect->left + (int)(xScale * (long double)xScreen);
    if(i==(TABLE_ATTRIBUTES::NumFixedColumns()-1))
      cellRect.right-=2;
    else if(TABLE_ATTRIBUTES::ColumnLines())
      cellRect.right--;

    // get the proper offsets for WYSIWYG printing
    if(TABLE_ATTRIBUTES::CanActivate())
      yScreen = 2;
    else
      yScreen = 3;
    cellRect.top = lpRect->top + (int)((long double)yScreen * yScale);

    if(TABLE_ATTRIBUTES::ShowTitles())
      {
      yScreen += oldTitleHeight;
      cellRect.bottom = lpRect->top + (int)((long double)yScreen * yScale);

      width = (int)((long double)COLUMN::TitleCharWidth(col) * (long double)DPI.y / (long double)VGA_RESOLUTION);
      hFont = AUtlCreateFontIndirect(COLUMN::TitleFont(col, DPI.y, width));
      oldFont = (HFONT)SelectObject(hDC, hFont);

      PrintCell(hDC, &cellRect, COLUMN::TitleAscent(col),
               (int)((long double)COLUMN::CellCharWidth(col) * (long double)DPI.y / (long double)VGA_RESOLUTION),
               2*TABLE_ATTRIBUTES::TitleLeading()/3 + TABLE_ATTRIBUTES::TitleAscent() + cellRect.top,
               COLUMN::Title(col), COLUMN::DT_FlagsTitle(col), &rect, TRUE);

      SelectObject(hDC, oldFont);
      AUtlDeleteFont(hFont);

      if(TABLE_ATTRIBUTES::RowLines())
        yScreen += 2;
      else
        yScreen += 1;
      }

    width = (int)((long double)COLUMN::CellCharWidth(col) * (long double)DPI.y / (long double)VGA_RESOLUTION);
    hFont = AUtlCreateFontIndirect(COLUMN::CellFont(col, DPI.y, width));
    oldFont = (HFONT)SelectObject(hDC, hFont);

    for(long j=firstRow; j<row->NumRows() && cellRect.top<rect.bottom; j++)
      {
      cellRect.top = lpRect->top + (int)((long double)yScreen * yScale);
      yScreen += oldCellHeight;
      cellRect.bottom = lpRect->top + (int)((long double)yScreen * yScale);

      PrintCell(hDC, &cellRect, COLUMN::CellAscent(col),
               width,
               2*TABLE_ATTRIBUTES::CellLeading()/3 + TABLE_ATTRIBUTES::CellAscent() + cellRect.top,
               row->GetCell(j, col, TRUE, &dbAccessed), COLUMN::DT_Flags(col), &rect, FALSE);

      if(TABLE_ATTRIBUTES::RowLines())
        yScreen++;
      }

    SelectObject(hDC, oldFont);
    AUtlDeleteFont(hFont);

    cellRect.left = cellRect.right;
    if(i==(TABLE_ATTRIBUTES::NumFixedColumns()-1))
      cellRect.left+=2;
    else if(TABLE_ATTRIBUTES::ColumnLines())
      cellRect.left++;
    }

  // restore the colors the what they were previously
  SetTextColor(hDC, oldFGColor);
  SetBkColor(hDC, oldBGColor);

  rect = *lpRect;
  DrawWIPRBorder(&rect, hDC, xScale, yScale, pclPrinter);
  PrintWIPRLines(hDC, lpRect->left, lpRect->top, &rect, xScale, yScale, oldTitleHeight, oldCellHeight, pclPrinter);

  // reset the font metrics for the screen
  InitFontMetrics();

  if(dbAccessed)
    AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);
  }

// TABLE::DrawWIPRBorder ///////////////////////////////////////////////////

void TABLE::DrawWIPRBorder(
  RECT*       rect,
  HDC         hDC,
  long double xScale,
  long double yScale,
  int         pcl)

  {
  for(int j=0; j<TABLE_ATTRIBUTES::PrintDoubleLines(); j++)
    {
    if(j!=1 && pcl)
      {
      PRECT_STRUCT  prect;
      prect.prPosition.x = rect->left;
      prect.prPosition.y = rect->top;
      prect.prSize.x = (int)xScale;
      prect.prSize.y = (int)yScale;
      prect.prStyle = 0;
      prect.prPattern = 100;

      // left line
      prect.prSize.y = rect->bottom - rect->top;
      Escape(hDC, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);

      // right line
      prect.prPosition.x = rect->right - (int)xScale;
      Escape(hDC, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);
  
      // top line
      prect.prPosition.x = rect->left;
      prect.prSize.x = rect->right - rect->left;
      prect.prSize.y = (int)yScale;
      Escape(hDC, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);

      // bottom line
      prect.prPosition.y = rect->bottom - (int)yScale;
      Escape(hDC, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);
      }
    else if(j!=1)
      {
      HPEN  hPen = CreatePen(PS_SOLID, 1, COLOR_BLACK);
      HPEN  oldPen = (HPEN)SelectObject(hDC, hPen);
      for(int i=0; i<(int)xScale; i++)
        {
        // left line
        MoveTo(hDC, rect->left+i, rect->top);
        LineTo(hDC, rect->left+i, rect->bottom);

        // right line
        MoveTo(hDC, rect->right-i, rect->top);
        LineTo(hDC, rect->right-i, rect->bottom);
        }

      for(i=0; i<(int)yScale; i++)
        {
        // top line
        MoveTo(hDC, rect->left, rect->top+i);
        LineTo(hDC, rect->right, rect->top+i);

        // bottom line
        MoveTo(hDC, rect->left, rect->bottom-i);
        LineTo(hDC, rect->right, rect->bottom-i);
        }
      SelectObject(hDC, oldPen);
      DeleteObject(hPen);
      }

    rect->left += (int)xScale;
    rect->top += (int)yScale;
    rect->right -= (int)xScale;
    rect->bottom -= (int)yScale;
    }
  }

// TABLE::PIDR_Draw ////////////////////////////////////////////////////////

void TABLE::PIDR_Draw(
  PIDR_BLOCK* pidr)

  {
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  long double   xScale = (long double)pidr->DPI.x / (long double)VGA_RESOLUTION;
  long double   yScale = (long double)pidr->DPI.y / (long double)VGA_RESOLUTION;
  long          j;
  int           i, k;
  int           colWidth;
  POINT         dim, offset;
  int           printSpace;
  int           rowHeight;
  int           numPages;
  int           colFactor;
  int           cxPos;
  RECT          cellRect;
  HFONT         hFont, oldFont;
  long          startLine;
  int           linesPerPage;
  BOOL          dbAccessed = FALSE;
  int           width;

  // make sure the fonts are correct for the printer
  InitPrinterMetrics(pidr->hDC);

  // set the margins
  if(Escape(pidr->hDC, GETPHYSPAGESIZE, NULL, NULL, (char*)&dim)>0)
    {
    if(Escape(pidr->hDC, GETPRINTINGOFFSET, NULL, NULL, (char*)&offset)>0)
      {
      RECT rect;
      rect.left   = pidr->DPI.x*MAR_LEFT - offset.x;
      rect.top    = pidr->DPI.y*MAR_TOP  - offset.y;
      rect.right  = dim.x - pidr->DPI.x*MAR_RIGHT  - offset.x;
      rect.bottom = dim.y - pidr->DPI.y*MAR_BOTTOM - offset.y;

      // reassign the rectangle as necessary
      if(pidr->rect.left   < rect.left)   pidr->rect.left   = rect.left;
      if(pidr->rect.top    < rect.top)    pidr->rect.top    = rect.top;
      if(pidr->rect.right  > rect.right)  pidr->rect.right  = rect.right;
      if(pidr->rect.bottom > rect.bottom) pidr->rect.bottom = rect.bottom;
      }
    }

  printSpace = pidr->rect.bottom - pidr->rect.top;
  if(COLUMN::NumColumns()==0)
    {
    if(pidr->numPages)
      pidr->toPage = 0;
    goto quitLabel;
    }

  if(TABLE_ATTRIBUTES::ShowTitles())
    printSpace -= (TABLE_ATTRIBUTES::TitleHeight()+2*(int)yScale);

  if(TABLE_ATTRIBUTES::PrintPageNums())
    printSpace -= (TABLE_ATTRIBUTES::CellHeight()*2);

  rowHeight = TABLE_ATTRIBUTES::CellHeight();
  if(TABLE_ATTRIBUTES::RowLines())
    rowHeight++;

  // number of pages for length only!
  linesPerPage = (int)(printSpace / rowHeight);
  numPages = (int)(row->NumRows() / linesPerPage) + 1;

  // determine the multiplication factor for column widths!
  colFactor = 1;
  cxPos = pidr->rect.left + 2 * (int)xScale;
  for(i=0; i<COLUMN::NumColumns(); i++)
    {
    if(COLUMN::ColumnWidth(i)==0)
      {
      if((i==(TABLE_ATTRIBUTES::NumFixedColumns()-1))
          &&
         (cxPos!=(pidr->rect.left + 2 * (int)xScale))
          &&
         (colFactor!=1))
        {
        cxPos+=2;
        }       
      }

    cxPos += (int)(xScale * (long double)COLUMN::ColumnWidth(i));
    if(i==(TABLE_ATTRIBUTES::NumFixedColumns()-1))
      cxPos+=2;
    else if(TABLE_ATTRIBUTES::ColumnLines())
      cxPos++;
    if(cxPos > pidr->rect.right)
      {
      cxPos = pidr->rect.left;
      colFactor++;
      }
    }

  if(pidr->numPages)
    {
    pidr->toPage = numPages;
    goto quitLabel;
    }

  // do the actual printing...
  for(i=(pidr->fromPage-1); i<pidr->toPage && i<numPages; i++)
    {
    char  szBuffer[30];
    wsprintf(szBuffer, "Table:  Page %i of %i", (i+1), numPages);
    SetWindowText(pidr->hMsgWnd, szBuffer);

    cellRect.left = pidr->rect.left + 2*(int)xScale;
    cellRect.top = pidr->rect.top + 2*(int)yScale;
    startLine = i*(long)linesPerPage;
    int numLines = linesPerPage;
    if(row->NumRows()<(startLine+linesPerPage))
      numLines = (int)(row->NumRows()-startLine);
    int vHeight = rowHeight*numLines;
    if(TABLE_ATTRIBUTES::ShowTitles())
      vHeight += (TABLE_ATTRIBUTES::TitleHeight()+2*(int)yScale);
    if(TABLE_ATTRIBUTES::RowLines())
      vHeight++;

    for(k=0; k<COLUMN::NumColumns(); k++)
      {
      colWidth = (int)(xScale * (long double)COLUMN::ColumnWidth(k));
      cellRect.right = cellRect.left + colWidth;
      if(colWidth==0)
        continue;

      int leftFudge = 0;
      int rightFudge = 0;
      if(k==0)
        leftFudge = 2 * (int)xScale;
      if(k==(COLUMN::NumColumns()-1))
        rightFudge = 2 * (int)xScale;

      if(TABLE_ATTRIBUTES::ColumnLines() && !rightFudge)
        PrintDoubleHLines(pidr->hDC, pidr->pclPrinter, (int)yScale, cellRect.left-leftFudge, pidr->rect.top, colWidth+leftFudge+rightFudge+1);
      else
        PrintDoubleHLines(pidr->hDC, pidr->pclPrinter, (int)yScale, cellRect.left-leftFudge, pidr->rect.top, colWidth+leftFudge+rightFudge);

      if(k==0)
        PrintDoubleVLines(pidr->hDC, pidr->pclPrinter, (int)xScale, pidr->rect.left, cellRect.top, vHeight);
      else if(k==(COLUMN::NumColumns()-1))
        PrintDoubleVLines(pidr->hDC, pidr->pclPrinter, (int)xScale, cellRect.right, cellRect.top, vHeight);

      if(TABLE_ATTRIBUTES::ColumnLines())
        PrintVLine(pidr->hDC, pidr->pclPrinter, cellRect.right, cellRect.top, vHeight);

      if(TABLE_ATTRIBUTES::ShowTitles())
        {
        cellRect.bottom = cellRect.top + TABLE_ATTRIBUTES::TitleHeight();

        width = (int)((long double)COLUMN::TitleCharWidth(k) * (long double)pidr->DPI.y / (long double)VGA_RESOLUTION);
        hFont = AUtlCreateFontIndirect(COLUMN::TitleFont(k, pidr->DPI.y, width));
        oldFont = (HFONT)SelectObject(pidr->hDC, hFont);

        PrintCell(pidr->hDC, &cellRect, COLUMN::TitleAscent(k),
                 (int)((long double)COLUMN::CellCharWidth(k) * (long double)pidr->DPI.y / (long double)VGA_RESOLUTION),
                 2*TABLE_ATTRIBUTES::TitleLeading()/3 + TABLE_ATTRIBUTES::TitleAscent() + cellRect.top,
                 COLUMN::Title(k), COLUMN::DT_FlagsTitle(k), &pidr->rect, TRUE);

        SelectObject(pidr->hDC, oldFont);
        AUtlDeleteFont(hFont);

        PrintDoubleHLines(pidr->hDC, pidr->pclPrinter, (int)yScale, cellRect.left, cellRect.bottom, colWidth);
        cellRect.top = cellRect.bottom + 2*(int)yScale;
        }

      width = (int)((long double)COLUMN::CellCharWidth(k) * (long double)pidr->DPI.y / (long double)VGA_RESOLUTION);
      hFont = AUtlCreateFontIndirect(COLUMN::CellFont(k, pidr->DPI.y, width));
      oldFont = (HFONT)SelectObject(pidr->hDC, hFont);

      for(j=startLine;j<(startLine + linesPerPage); j++)
        {
        if(j==row->NumRows())
          {
          int leftFudge = 0;
          int rightFudge = 0;
          if(k==0)
            leftFudge = 2 * (int)xScale;
          if(k==(COLUMN::NumColumns()-1))
            rightFudge = 2 * (int)xScale;
          if(TABLE_ATTRIBUTES::ColumnLines() && !rightFudge)
            PrintDoubleHLines(pidr->hDC, pidr->pclPrinter, (int)yScale, cellRect.left-leftFudge, cellRect.bottom, colWidth+leftFudge+rightFudge+1);
          else
            PrintDoubleHLines(pidr->hDC, pidr->pclPrinter, (int)yScale, cellRect.left-leftFudge, cellRect.bottom, colWidth+leftFudge+rightFudge);
          break;
          }

        cellRect.bottom = cellRect.top + TABLE_ATTRIBUTES::CellHeight();

        PrintCell(pidr->hDC, &cellRect, COLUMN::CellAscent(k),
                 width,
                 2*TABLE_ATTRIBUTES::CellLeading()/3 + TABLE_ATTRIBUTES::CellAscent() + cellRect.top,
                 row->GetCell(j, k, TRUE, &dbAccessed), COLUMN::DT_Flags(k), &pidr->rect, FALSE);

        if(TABLE_ATTRIBUTES::RowLines())
          {
          PrintHLine(pidr->hDC, pidr->pclPrinter, cellRect.left, cellRect.bottom+1, colWidth);
          cellRect.bottom++;
          }
        cellRect.top = cellRect.bottom;
        }

      SelectObject(pidr->hDC, oldFont);
      AUtlDeleteFont(hFont);
      cellRect.left = cellRect.right;
      if(TABLE_ATTRIBUTES::ColumnLines())
        cellRect.left++;
      cellRect.top = pidr->rect.top + 2*(int)yScale;
      }

    if(TABLE_ATTRIBUTES::PrintPageNums())
      {
      cellRect = pidr->rect;
      cellRect.top = pidr->rect.bottom - TABLE_ATTRIBUTES::CellHeight();
      width = (int)((long double)COLUMN::CellCharWidth(0) * (long double)pidr->DPI.y / (long double)VGA_RESOLUTION);
      hFont = AUtlCreateFontIndirect(COLUMN::CellFont(0, pidr->DPI.y, width));
      oldFont = (HFONT)SelectObject(pidr->hDC, hFont);
      DrawText(pidr->hDC, &szBuffer[8], -1, &cellRect, DT_SINGLELINE | DT_BOTTOM | DT_CENTER);
      SelectObject(pidr->hDC, oldFont);
      AUtlDeleteFont(hFont);
      }

    if(i==(numPages-1) || i==(pidr->toPage-1))
      break;

    // eject the page
    short ret = Escape(pidr->hDC, NEWFRAME, NULL, NULL, NULL);
    if(ret<=0)
      {
      switch(ret)
        {
        case SP_APPABORT: // don't report this error
          break;

        case SP_OUTOFDISK:
          lstrcpy(pidr->error, "Not enough disk space is currently available for spooling, and no more space will become available.");
          break;

        case SP_OUTOFMEMORY:
          lstrcpy(pidr->error, "Not enough memory is available for spooling.");
          break;

        case SP_USERABORT:
          lstrcpy(pidr->error, "User terminated the job through Print Manager.");
          break;

        case SP_ERROR:
        default:
          lstrcpy(pidr->error, "General error.");
          break;
        }
      break;
      }
    }

  // reset the font metrics for the screen
  quitLabel:
  InitFontMetrics();

  if(dbAccessed)
    AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);
  }

// TABLE::InitPrinterMetrics ///////////////////////////////////////////////

void TABLE::InitPrinterMetrics(
  HDC hDC)

  {
  // make sure the font metrics are valid
  COLUMN::TitleMetrics(hDC, GetDeviceCaps(hDC, LOGPIXELSY), TRUE);
  COLUMN::CellMetrics(hDC, GetDeviceCaps(hDC, LOGPIXELSY), TRUE);

  // initialize the TABLE_ATTRIBUTES font metrics info
  TABLE_ATTRIBUTES::SetTitleLeading(COLUMN::MaxTitleLeading());
  TABLE_ATTRIBUTES::SetTitleAscent(COLUMN::MaxTitleAscent());
  TABLE_ATTRIBUTES::SetTitleDescent(COLUMN::MaxTitleDescent());
  TABLE_ATTRIBUTES::SetCellLeading(COLUMN::MaxCellLeading());
  TABLE_ATTRIBUTES::SetCellAscent(COLUMN::MaxCellAscent());
  TABLE_ATTRIBUTES::SetCellDescent(COLUMN::MaxCellDescent());
  }

// TABLE::PrintCell ////////////////////////////////////////////////////////

void TABLE::PrintCell(
  HDC       hDC,
  RECT*     rect,       // cell rectangle -- text may not fit
  int       fontAscent,
  int       charWidth,
  int       baseLine,
  HPCSTR    text,       // cell string
  UINT      flags,
  RECT*     limitRect,
  BOOL      title)

  {
  RECT  textRect;
  SetRect(&textRect, rect->left + charWidth, baseLine - fontAscent,
          rect->right - charWidth, rect->bottom);

  if(rect->bottom>limitRect->bottom)
    return;

  if(textRect.right > limitRect->right)
    textRect.right = limitRect->right;

  int length = (int)hstrlen(text);
  int max = textRect.right - textRect.left;
  int start = 0;
  int i=0;
  int elipLen = 0;
  if(!title)
    elipLen = LOWORD(GetTextExtent(hDC, " ...", 4));
  if(elipLen>max)
    return;

  while((LOWORD(GetTextExtent(hDC, (char*)(text+start), length))+((i==0) ? 0 : elipLen)) > max)
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
    }

  char* txtPtr = (char*)text;
  if(length!=hstrlen(text))
    {
    if(!title)
      {
      txtPtr = new char[length+4];
      hmemcpy(txtPtr, text+start, length);
      txtPtr[length]='.';
      txtPtr[length+1]='.';
      txtPtr[length+2]='.';
      txtPtr[length+3]=0;
      }
    else
      {
      txtPtr = new char[length+1];
      hmemcpy(txtPtr, text+start, length);
      txtPtr[length]=0;
      }
    }   

  DrawText(hDC, txtPtr, -1, &textRect, flags);

  if(length!=hstrlen(text))
    delete txtPtr;
  }

// TABLE::PrintWIPRLines ///////////////////////////////////////////////////

void TABLE::PrintWIPRLines(
  HDC         hDC,
  int         left,
  int         top,
  RECT*       rect,
  long double xScale,
  long double yScale,
  int         oldTitleHeight,
  int         oldCellHeight,
  int         pcl)

  {
  if(COLUMN::NumColumns() < 1)
    return;

  HPEN  hPen, oldPen;
  int   txPos, tyPos;
  int   xScreen;
  int   yScreen;
  PRECT_STRUCT  prect;
  prect.prStyle = 0;
  prect.prPattern = 100;

  if(TABLE_ATTRIBUTES::CanActivate())
    xScreen = 3;
  else
    xScreen = 2;

  if(TABLE_ATTRIBUTES::ColumnLines())
    {
    // draw column lines
    for(int i=0; i<NumColumns(); i++)
      {
      if(ColumnWidth(i)==0)
        {
        if((i==(TABLE_ATTRIBUTES::NumFixedColumns()-1))
            &&
           (xScreen>3))
          {
          xScreen++;
          }
        continue;
        }

      xScreen += ColumnWidth(i);
      if(i==(TABLE_ATTRIBUTES::NumFixedColumns()-1))
        xScreen+=2;
      else
        xScreen++;

      txPos = left + (int)(xScale * (long double)xScreen);
      if(i==(TABLE_ATTRIBUTES::NumFixedColumns()-1))
        txPos-=2;
      else
        txPos--;
      if(txPos>rect->right)
        break;

      if(i != (TABLE_ATTRIBUTES::NumFixedColumns()-1))
        {
        if(pcl)
          {
          prect.prPosition.x = txPos++;
          prect.prPosition.y = rect->top;
          prect.prSize.x = 1;
          prect.prSize.y = rect->bottom - rect->top;
          Escape(hDC, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);
          }
        else
          {
          hPen = CreatePen(PS_SOLID, 1, COLOR_BLACK);
          oldPen = (HPEN)SelectObject(hDC, hPen);
          MoveTo(hDC, txPos, rect->top);
          LineTo(hDC, txPos++, rect->bottom);
          SelectObject(hDC, oldPen);
          DeleteObject(hPen);
          }
        }
      else
        txPos+=2;
      } // end....for(int i=0; ...
    }

  if(TABLE_ATTRIBUTES::CanActivate())
    yScreen = 3;
  else
    yScreen = 2;

  if(TABLE_ATTRIBUTES::ShowTitles())
    {
    yScreen += oldTitleHeight;
    tyPos = top + (int)((long double)yScreen * yScale);
    if(pcl)
      {
      prect.prPosition.x = rect->left;
      prect.prPosition.y = ++tyPos;
      prect.prSize.x = rect->right - rect->left;
      prect.prSize.y = 2;
      Escape(hDC, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);
      tyPos++;
      }
    else
      {
      hPen = CreatePen(PS_SOLID, 1, COLOR_BLACK);
      oldPen = (HPEN)SelectObject(hDC, hPen);
      MoveTo(hDC, rect->left, ++tyPos);
      LineTo(hDC, rect->right, tyPos);
      MoveTo(hDC, rect->left, ++tyPos);
      LineTo(hDC, rect->right, tyPos);
      SelectObject(hDC, oldPen);
      DeleteObject(hPen);
      }
    if(TABLE_ATTRIBUTES::RowLines())
      yScreen += 2;
    else
      yScreen ++;
    }

  if(TABLE_ATTRIBUTES::RowLines())
    {
    // draw row lines
    for(int i=0; TRUE; i++)
      {
      yScreen += oldCellHeight;
      tyPos = top + (int)((long double)yScreen * yScale);
      if(tyPos>rect->bottom)
        break;

      if(pcl)
        {
        prect.prPosition.x = rect->left;
        prect.prPosition.y = ++tyPos;
        prect.prSize.x = rect->right - rect->left;
        prect.prSize.y = 1;
        Escape(hDC, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);
        }
      else
        {
        hPen = CreatePen(PS_SOLID, 1, COLOR_BLACK);
        oldPen = (HPEN)SelectObject(hDC, hPen);
        MoveTo(hDC, rect->left, ++tyPos);
        LineTo(hDC, rect->right, tyPos);
        SelectObject(hDC, oldPen);
        DeleteObject(hPen);
        }

      yScreen++;
      } // end....for(int i=0; ...
    }

  // draw the fixed column line
  if(TABLE_ATTRIBUTES::NumFixedColumns()>0)
    {
    if(TABLE_ATTRIBUTES::CanActivate())
      xScreen = 3;
    else
      xScreen = 2;

    for(int j=0; j<TABLE_ATTRIBUTES::NumFixedColumns(); j++)
      {
      if(ColumnWidth(j)==0)
        {
        if(j!=(TABLE_ATTRIBUTES::NumFixedColumns()-1))
          continue;

        if(xScreen<=3)
          break;
        }

      xScreen += ColumnWidth(j);
      if(j==(TABLE_ATTRIBUTES::NumFixedColumns()-1))
        xScreen+=2;
      else if(TABLE_ATTRIBUTES::ColumnLines())
        xScreen++;

      if(j!=(TABLE_ATTRIBUTES::NumFixedColumns()-1))
        continue;

      txPos = left + (int)(xScale * (long double)xScreen) - 2;
      if(txPos>rect->right)
        break;

      // draw the fixed line
      if(pcl)
        {
        prect.prPosition.x = txPos;
        prect.prPosition.y = rect->top;
        prect.prSize.x = 2;
        prect.prSize.y = rect->bottom - rect->top;
        Escape(hDC, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);
        tyPos++;
        }
      else
        {
        hPen = CreatePen(PS_SOLID, 1, COLOR_BLACK);
        oldPen = (HPEN)SelectObject(hDC, hPen);
        MoveTo(hDC, txPos, rect->top);
        LineTo(hDC, txPos, rect->bottom);
        MoveTo(hDC, ++txPos, rect->top);
        LineTo(hDC, txPos, rect->bottom);
        SelectObject(hDC, oldPen);
        DeleteObject(hPen);
        }
      }
    }
  }

// TABLE::PrintDoubleVLines ////////////////////////////////////////////////

void TABLE::PrintDoubleVLines(
  HDC hDC,
  int pcl,
  int lWidth,
  int left,
  int top,
  int height)

  {
  if(pcl)
    {
    PRECT_STRUCT  prect;
    prect.prStyle = 0;
    prect.prPattern = 100;
    prect.prPosition.x = left;
    prect.prPosition.y = top;
    prect.prSize.x = 2*lWidth;
    prect.prSize.y = height;

    // draw the first line
    Escape(hDC, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);
    }
  else
    {
    HPEN  hPen = CreatePen(PS_SOLID, 1, COLOR_BLACK);
    HPEN  oldPen = (HPEN)SelectObject(hDC, hPen);

    for(int j=0; j<2*lWidth; j++, left++)
      {
      MoveTo(hDC, left, top);
      LineTo(hDC, left, top+height);
      }
    SelectObject(hDC, oldPen);
    DeleteObject(hPen);
    }
  }

// TABLE::PrintVLine ///////////////////////////////////////////////////////

void TABLE::PrintVLine(
  HDC hDC,
  int pcl,
  int left,
  int top,
  int height)

  {
  if(pcl)
    {
    PRECT_STRUCT  prect;
    prect.prStyle = 0;
    prect.prPattern = 100;
    prect.prPosition.x = left;
    prect.prPosition.y = top;
    prect.prSize.x = 1;
    prect.prSize.y = height;

    // draw the first line
    Escape(hDC, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);
    }
  else
    {
    HPEN  hPen = CreatePen(PS_SOLID, 1, COLOR_BLACK);
    HPEN  oldPen = (HPEN)SelectObject(hDC, hPen);
    MoveTo(hDC, left, top);
    LineTo(hDC, left, top+height);
    SelectObject(hDC, oldPen);
    DeleteObject(hPen);
    }
  }

// TABLE::PrintDoubleHLines ////////////////////////////////////////////////

void TABLE::PrintDoubleHLines(
  HDC hDC,
  int pcl,
  int lHeight,
  int left,
  int top,
  int width)

  {
  if(pcl)
    {
    PRECT_STRUCT  prect;
    prect.prStyle = 0;
    prect.prPattern = 100;
    prect.prPosition.x = left;
    prect.prPosition.y = top;
    prect.prSize.x = width;
    prect.prSize.y = 2*lHeight;

    // draw the first line
    Escape(hDC, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);
    }
  else
    {
    HPEN  hPen = CreatePen(PS_SOLID, 1, COLOR_BLACK);
    HPEN  oldPen = (HPEN)SelectObject(hDC, hPen);

    for(int j=0; j<2*lHeight; j++, top++)
      {
      MoveTo(hDC, left, top);
      LineTo(hDC, left+width, top);
      }
    SelectObject(hDC, oldPen);
    DeleteObject(hPen);
    }
  }

// TABLE::PrintHLine ///////////////////////////////////////////////////////

void TABLE::PrintHLine(
  HDC hDC,
  int pcl,
  int left,
  int top,
  int width)

  {
  if(pcl)
    {
    PRECT_STRUCT  prect;
    prect.prStyle = 0;
    prect.prPattern = 100;
    prect.prPosition.x = left;
    prect.prPosition.y = top;
    prect.prSize.x = width;
    prect.prSize.y = 1;

    // draw the first line
    Escape(hDC, DRAWPATTERNRECT, sizeof(PRECT_STRUCT), (LPSTR)&prect, NULL);
    }
  else
    {
    HPEN  hPen = CreatePen(PS_SOLID, 1, COLOR_BLACK);
    HPEN  oldPen = (HPEN)SelectObject(hDC, hPen);
    MoveTo(hDC, left, top);
    LineTo(hDC, left+width, top);
    SelectObject(hDC, oldPen);
    DeleteObject(hPen);
    }
  }

////////////////////////////////////////////////////////////////////////////
