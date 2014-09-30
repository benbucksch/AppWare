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
// FILE:  "table4.cpp"
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

#include  <windows.h>
#include  <stdlib.h>

#include  <a_alm.h>
#include  <intdebug.h>
#include  <intutl.h>
#include  <op_wnd.h>
#include  <o_wnd.h>

#include  "tabl_hdr.h"  // for hInstance
#include  "tabl_rsc.h"
#include  "table.h"
#include  "tabl_row.h"

// dfault sizes for buttons
#define I_DEFAULT_HEIGHT   300
#define I_DEFAULT_WIDTH    300
#define I_MIN_WIDTH        50
#define I_MIN_HEIGHT       50

#define I_DOT_SIZE         16 //1.6mm
#define I_DOT_SIZEH        8

// Externals ///////////////////////////////////////////////////////////////

extern GLOBAL G;

////////////////////////////////////////////////////////////////////////////
// ConvertToParentPt
// SKJ -> this function will take a POINT in table client space and convert it to
//        a POINT in table's Parent's client space  (i.e. WINDOW object's)
///////////////////////////////////////////////////////////////////////////
BOOL ConvertToParentPt(HWND hTable, POINT *pt)
  {
  POINT tmp;
  tmp = *pt;

  ClientToScreen(hTable, &tmp);
  ScreenToClient(GetParent(hTable), &tmp);

  *pt = tmp;
  return TRUE;
  }
// TABLE::Clicked //////////////////////////////////////////////////////////

void TABLE::Clicked(
  HWND    hWnd,
  WPARAM  wParam,
  LPARAM  lParam)

  {
  if(hWnd!=hTable)
    return;

  POINT point;

  A_MAKEPOINT(point, lParam);

  if(PtInRect(&tblRect, point))
    {
    AEvtPostSignalAtMark(theObject, SIGNAL_ROWCLICKED);

    // find the hit cell
    RECT  cellRect;
    BOOL  notFound = TRUE;
    cellRect.left = tblRect.left;
    for(int i=0; i<=NumColumns() && notFound && cellRect.left < tblRect.right; i++)
      {
      if(ColumnWidth(i)==0)
        {
        if((i == TABLE_ATTRIBUTES::NumFixedColumns()-1)
            &&
           (cellRect.left != tblRect.left))
          {
          cellRect.left++;
          }
        continue;
        }

      cellRect.top = tblRect.top;
      cellRect.right = cellRect.left + ColumnWidth(i);
      if(TABLE_ATTRIBUTES::ColumnLines())
        cellRect.right++;
      if((TABLE_ATTRIBUTES::NumFixedColumns()-1) == i)
        cellRect.right++;

      if(TABLE_ATTRIBUTES::ShowTitles())
        {
        cellRect.bottom = cellRect.top + TABLE_ATTRIBUTES::TitleHeight();
        if(TABLE_ATTRIBUTES::RowLines())
          cellRect.bottom+=2;
        else
          cellRect.bottom++;

        if(PtInRect(&cellRect, point))
          {
          if(TABLE_ATTRIBUTES::ColumnLines())
            {
            if(TABLE_ATTRIBUTES::CanResize() && abs(point.x - cellRect.left) <= COLUMN_RESIZEHITRANGE)
              {
              if(i<TABLE_ATTRIBUTES::NumFixedColumns())
                resizeColumn = i-1;
              else if(i!=TABLE_ATTRIBUTES::NumFixedColumns())
                resizeColumn = firstColumn + (i - TABLE_ATTRIBUTES::NumFixedColumns()) - 1;
              else
                {
                resizeColumn = firstColumn + (i - TABLE_ATTRIBUTES::NumFixedColumns()) - 1;
                if(COLUMN::ColumnWidth(resizeColumn)!=0)
                  resizeColumn = TABLE_ATTRIBUTES::NumFixedColumns()-1;
                }

              if(!(i==0 && COLUMN::ColumnWidth(resizeColumn)) && resizeColumn!=-1)
                {
                colLineX = cellRect.left;
                startX = point.x;
                DrawNewColLine(startX, FALSE);
                StartTracking(TRACK_COLUMNRESIZE);
                SetCursor(G.hResizeCursor);
                }
              return;
              }
            if(TABLE_ATTRIBUTES::CanResize() && abs(point.x - cellRect.right) <= COLUMN_RESIZEHITRANGE)
              {
              if(i<TABLE_ATTRIBUTES::NumFixedColumns())
                resizeColumn = i;
              else
                resizeColumn = firstColumn + (i - TABLE_ATTRIBUTES::NumFixedColumns()); 

              startX = point.x;
              colLineX = cellRect.right;
              DrawNewColLine(startX, FALSE);
              StartTracking(TRACK_COLUMNRESIZE);
              SetCursor(G.hResizeCursor);
              return;
              }
            }

          if(i!=NumColumns())
            ActivateColumn(i, TRUE);
          notFound = FALSE;
          break;
          } // end...if(PtInRect()...)

        cellRect.top = cellRect.bottom;
        } // end ... if(TABLE_ATTRIBUTES::ShowTitles())

      for(int j=0; cellRect.top < tblRect.bottom; j++)
        {
        cellRect.bottom = cellRect.top + TABLE_ATTRIBUTES::CellHeight();
        if(TABLE_ATTRIBUTES::RowLines())
          cellRect.bottom++;

        if(PtInRect(&cellRect, point))
          {
          RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
          ROW*          row = rtime->GetRow();
          long          sel = j + firstRow;
          if(sel >= row->NumRows())
            {
            notFound = FALSE;
            break;
            }

          if((tblRect.bottom - cellRect.top)<TABLE_ATTRIBUTES::CellHeight())
            {
            firstRow++;
            if(hVertScroll)
              SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
            }

          int col = (i<TABLE_ATTRIBUTES::NumFixedColumns()) ? i : (firstColumn + i - TABLE_ATTRIBUTES::NumFixedColumns());
          if(i==NumColumns())
            col = COLUMN::NumColumns()-1;

          if(hEdit && (sel!=TABLE_ATTRIBUTES::ActiveRow() || col!=TABLE_ATTRIBUTES::ActiveColumn()))
            DestroyEditControl();

          if(hEdit && (sel==TABLE_ATTRIBUTES::ActiveRow() && col==TABLE_ATTRIBUTES::ActiveColumn()))
            {
            notFound = FALSE;
            break;
            }

          if(TABLE_ATTRIBUTES::MultiSelect())
            {
            if(wParam & MK_SHIFT)
              {
              numOldSelections = (int)AObjGetDataSize(theObject, DATA_SELECTIONS) / sizeof(long);
              AObjResizeData(theObject, DATA_OLDSELECTIONS, numOldSelections*sizeof(long));
              oldSelections = (long *)AObjLockData(theObject, DATA_OLDSELECTIONS);
              if (oldSelections)
                {
                long *newSelections = (long *)AObjLockData(theObject, DATA_SELECTIONS);
                CopyMemory(oldSelections, newSelections, numOldSelections*sizeof(long));
                if (newSelections)
                  AObjUnlockData(theObject, DATA_OLDSELECTIONS);

                AObjUnlockData(theObject, DATA_SELECTIONS);
                }

              long  anchorRow = TABLE_ATTRIBUTES::AnchorRow();
              if((anchorRow<0) || (anchorRow>=row->NumRows()))
                {
                TABLE_ATTRIBUTES::SetAnchorRow(sel);
                row->SetSelection(1, &sel);
                }
              else
                row->RangeSelection(anchorRow, sel);

              if(!hEdit)
                openControl = TRUE;
              else
                openControl = FALSE;
              lastRow = sel;  // set the last row the cursor was in
              TABLE_ATTRIBUTES::SetActiveRow(sel);
              StartTracking(TRACK_SHIFTHIT);
              }
            else if(wParam & MK_CONTROL)
              {
              TABLE_ATTRIBUTES::SetAnchorRow(sel);
              row->ToggleSelection(sel);

              numOldSelections = (int)AObjGetDataSize(theObject, DATA_SELECTIONS) / sizeof(long);
              AObjResizeData(theObject, DATA_OLDSELECTIONS, numOldSelections*sizeof(long));
              oldSelections = (long*)AObjLockData(theObject, DATA_OLDSELECTIONS);

              if (oldSelections)
                {
                CopyMemory(oldSelections, AObjLockData(theObject, DATA_SELECTIONS), numOldSelections*sizeof(long));
                AObjUnlockData(theObject, DATA_OLDSELECTIONS);
                AObjUnlockData(theObject, DATA_SELECTIONS);
                }

              openControl = FALSE;
              lastRow = sel;  // set the last row the cursor was in
              TABLE_ATTRIBUTES::SetActiveRow(sel);
              StartTracking(TRACK_CONTROLHIT);
              }
            else
              {
              numOldSelections = (int)AObjGetDataSize(theObject, DATA_SELECTIONS) / sizeof(long);
              AObjResizeData(theObject, DATA_OLDSELECTIONS, numOldSelections*sizeof(long));
              oldSelections = (long*)AObjLockData(theObject, DATA_OLDSELECTIONS);
              if (oldSelections)
                {
                CopyMemory(oldSelections, AObjLockData(theObject, DATA_SELECTIONS), numOldSelections*sizeof(long));
                AObjUnlockData(theObject, DATA_OLDSELECTIONS);
                AObjUnlockData(theObject, DATA_SELECTIONS);
                }

              TABLE_ATTRIBUTES::SetAnchorRow(sel);
              row->SetSelection(1, &sel);
              if(i!=NumColumns())
                activeColumn = (i<TABLE_ATTRIBUTES::NumFixedColumns()) ? i : (firstColumn + i - TABLE_ATTRIBUTES::NumFixedColumns());
              else
                activeColumn = COLUMN::NumColumns()-1;
              TABLE_ATTRIBUTES::SetActiveColumn(activeColumn);
              TABLE_ATTRIBUTES::SetActiveRow(sel);
              if(!hEdit)
                openControl = TRUE;
              else
                openControl = FALSE;
              lastRow = sel;  // set the last row the cursor was in
              StartTracking(TRACK_SHIFTHIT);
              }
            }
          else
            {
            if(wParam & MK_CONTROL)
              {
              if(row->FirstSelection()!=sel)
                row->ToggleSelection(row->FirstSelection());
              row->ToggleSelection(sel);
              openControl = FALSE;
              AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
              PostObjectChanged(CHANGED_FULLREDRAW);
              }
            else
              {
              numOldSelections = (int)AObjGetDataSize(theObject, DATA_SELECTIONS) / sizeof(long);
              AObjResizeData(theObject, DATA_OLDSELECTIONS, numOldSelections*sizeof(long));
              oldSelections = (long*)AObjLockData(theObject, DATA_OLDSELECTIONS);
              if (oldSelections)
                {
                CopyMemory(oldSelections, AObjLockData(theObject, DATA_SELECTIONS), numOldSelections*sizeof(long));
                AObjUnlockData(theObject, DATA_OLDSELECTIONS);
                AObjUnlockData(theObject, DATA_SELECTIONS);
                }

              lastRow = sel;  // set the last row the cursor was in
              row->SetSelection(1, &sel);
              if(i!=NumColumns())
                activeColumn = (i<TABLE_ATTRIBUTES::NumFixedColumns()) ? i : (firstColumn + i - TABLE_ATTRIBUTES::NumFixedColumns());
              else
                activeColumn = COLUMN::NumColumns()-1;
              TABLE_ATTRIBUTES::SetActiveRow(sel);
              if(!hEdit)
                openControl = TRUE;
              else
                openControl = FALSE;
              StartTracking(TRACK_CELLHIT);
              }
            }

          // draw the selection for this visual instance
          InvalidateTableRect();

          notFound = FALSE;
          if(!hEdit)
            {
            dblClkWait = TRUE;
            PostDoubleClickWait();
            }
          break;
          }

        cellRect.top = cellRect.bottom;
        }

      cellRect.left = cellRect.right;
      }
    }
  }

// TABLE::DoubleClicked ////////////////////////////////////////////////////

void TABLE::DoubleClicked(
  HWND    hWnd,
  LPARAM  lParam)

  {
  if(hWnd!=hTable)
    return;

  POINT point;

  A_MAKEPOINT(point, lParam);

  if(PtInRect(&tblRect, point))
    {
    AEvtPostSignalAtMark(theObject, SIGNAL_ROWDOUBLECLICKED);

    // find the hit cell
    RECT  cellRect;
    BOOL  notFound = TRUE;
    cellRect.left = tblRect.left;
    for(int i=0; i<=NumColumns() && notFound && cellRect.left < tblRect.right; i++)
      {
      if(ColumnWidth(i)==0)
        {
        if((i == TABLE_ATTRIBUTES::NumFixedColumns()-1)
            &&
           (cellRect.left != tblRect.left))
          {
          cellRect.left++;
          }
	continue;
        }

      cellRect.top = tblRect.top;
      cellRect.right = cellRect.left + ColumnWidth(i);
      if(TABLE_ATTRIBUTES::ColumnLines() && ColumnWidth(i)>0)
        cellRect.right++;
      if(TABLE_ATTRIBUTES::NumFixedColumns() == (i-1))
        cellRect.right++;

      if(TABLE_ATTRIBUTES::ShowTitles())
        {
        cellRect.bottom = cellRect.top + TABLE_ATTRIBUTES::TitleHeight();
        if(TABLE_ATTRIBUTES::RowLines())
          cellRect.bottom+=2;
        else
          cellRect.bottom++;

        if(PtInRect(&cellRect, point))
	  {
          notFound = FALSE;
          break;
          }

        cellRect.top = cellRect.bottom;
        }

      for(int j=0; cellRect.top < tblRect.bottom; j++)
        {
        cellRect.bottom = cellRect.top + TABLE_ATTRIBUTES::CellHeight();
        if(TABLE_ATTRIBUTES::RowLines())
          cellRect.bottom++;

        if(PtInRect(&cellRect, point))
          {
          notFound = FALSE;

          RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
	  ROW*          row = rtime->GetRow();
          long          sel = j + firstRow;
          if(sel >= row->NumRows())
            break;

          if(hEdit)
            {
            int   col = (i<TABLE_ATTRIBUTES::NumFixedColumns()) ? i : (firstColumn + i - TABLE_ATTRIBUTES::NumFixedColumns());
            long  sel = j + firstRow;
            if(sel >= row->NumRows())
              break;

            if(sel==TABLE_ATTRIBUTES::ActiveRow() && col==TABLE_ATTRIBUTES::ActiveColumn())
              EscKey();
            }
          if(dblClkWait)
            DestroyEditControl();
          dblClkWait = FALSE;
          openControl = FALSE;
	  break;
          }

        cellRect.top = cellRect.bottom;
        }

      cellRect.left = cellRect.right;
      }
    }
  }

// TABLE::DoubleClickWait //////////////////////////////////////////////////

void TABLE::DoubleClickWait()

  {
  if(dblClkWait)
    dblClkWait = FALSE;
  }

// TABLE::MouseUp //////////////////////////////////////////////////////////

void TABLE::MouseUp(
  LPARAM  lParam)

  {
  POINT point;

  A_MAKEPOINT(point, lParam);

  ConvertToParentPt( m_hwndItem, &point);

  point.x -= visRect.left;
  point.y -= visRect.top;

  if(tracking & TRACK_COLUMNRESIZE)
    {
    StopTracking(TRACK_COLUMNRESIZE);
    DrawNewColLine(prevLine - colLineX + startX + 1, FALSE);

    if(point.x > (tblRect.right + startX - colLineX + 1))
      point.x = tblRect.right + startX - colLineX + 1;
    int newWidth = COLUMN::Pixels(resizeColumn) + (point.x - startX);
    if(newWidth<0)
      newWidth = 0;

    if(newWidth != COLUMN::Pixels(resizeColumn))
      {
      if(hEdit)
        DestroyEditControl();

      if(resizeColumn<firstColumn && resizeColumn>=TABLE_ATTRIBUTES::NumFixedColumns())
        firstColumn--;

      COLUMN::SetPixels(resizeColumn, newWidth);
      PostObjectChanged(CHANGED_COLUMNWIDTH);
      }
    }

  if(tracking & TRACK_CELLHIT)
    {
    StopTracking(TRACK_CELLHIT);
    TABLE_ATTRIBUTES::SetActiveColumn(activeColumn);
    TABLE_ATTRIBUTES::SetActiveRow(lastRow);
    DrawAllInstances();
    if(openControl)
      CreateEditControl();

    if(SelectionsDifferent())
      AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
    }

  if(tracking & TRACK_SHIFTHIT)
    {
    StopTracking(TRACK_SHIFTHIT);

    RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
    ROW*          row = rtime->GetRow();
    TABLE_ATTRIBUTES::SetActiveRow(lastRow);
    if(openControl && row->NumSelections()==1)
      {
      DrawAllInstances();
      CreateEditControl();
      }
    else
      PostObjectChanged(CHANGED_FULLREDRAW);

    if(SelectionsDifferent())
      AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
    }

  if(tracking & TRACK_CONTROLHIT)
    {
    StopTracking(TRACK_CONTROLHIT);
    TABLE_ATTRIBUTES::SetActiveRow(lastRow);
    PostObjectChanged(CHANGED_FULLREDRAW);

    AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
    }
  }

// TABLE::KeyDown //////////////////////////////////////////////////////////

void TABLE::KeyDown(
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam,
  BOOL*   dispatched)
  {
  BOOL shift =  GetAsyncKeyState(VK_SHIFT) & 0x8000;
  BOOL control = GetAsyncKeyState(VK_CONTROL)& 0x8000; // (theEvent->modifiers & isCtrl) != 0;

  WPARAM  keyCode;

  keyCode = wParam;
  switch(keyCode)
    {
    case VK_ESCAPE:
      EscKey();
      return;

    case VK_RETURN:
      ReturnKey();
      return;

    case VK_TAB:
      TabKey(shift);
      return;

    case VK_UP:
      if(UpKey(shift))
        {
        if(hEdit)
          *dispatched=TRUE;
        return;
        }
      break;

    case VK_DOWN:
      if(DownKey(shift))
        {
        if(hEdit)
          *dispatched=TRUE;
        return;
        }
      break;

    case VK_LEFT:
      if(LeftKey(shift))
        return;
      break;

    case VK_RIGHT:
      if(RightKey(shift))
        return;
      break;

    case VK_PRIOR:
      if(PageUpKey(control, shift))
        {
        if(hEdit)
          *dispatched=TRUE;
        return;
        }
      break;

    case VK_NEXT:
      if(PageDownKey(control, shift))
        {
        if(hEdit)
          *dispatched=TRUE;
        return;
        }
      break;

    case VK_HOME:
      if(HomeKey(control, shift))
        return;
      break;

    case VK_END:
      if(EndKey(control, shift))
        return;
      break;

    case VK_BACK:
    case VK_DELETE:
      if(DeleteKey())
        return;
      break;

    default:
      break;
    }

  WPARAM Ascii = wParam ; // charCodeMask & modifiers;
  if(hEdit)
    {
    return;
    }
  else if(Ascii && !TABLE_ATTRIBUTES::AutoFill() && TABLE_ATTRIBUTES::Clairvoyant() && (COLUMN::NumColumns()>0))
    {
    // clairvoyant selection
    RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
    ROW*          row = rtime->GetRow();
    if(row->NumRows()>1)
      {
      long  i = 1L;
      long  first = row->FirstSelection();
      if(row->NumSelections()<1)
        first = -1L;

      long  idx;
      char  typedChar = (char)((long)AnsiUpper((char*)(char)Ascii));
      int   col = TABLE_ATTRIBUTES::ActiveColumn();
      if(col<0 || col>=COLUMN::NumColumns())
        TABLE_ATTRIBUTES::SetActiveColumn(0);
        
      col = TABLE_ATTRIBUTES::FocusColumn();
      if(col<0 || col>=COLUMN::NumColumns())
        col = 0;

      while((first+i)<row->NumRows())
        {
        char cellChar = *row->GetCell((first+i), col, FALSE, NULL);
        if((char)((long)AnsiUpper((char*)cellChar)) == typedChar)
          {
          idx = first+i;
          row->SetSelection(1, &idx);
          TABLE_ATTRIBUTES::SetActiveRow(first+i);
          ScrollToSelection(FALSE);
          PostObjectChanged(CHANGED_FULLREDRAW);
          AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
          return;
          }
        i++;
        }

      i = 0L;
      while(i<first)
        {
        char cellChar = *row->GetCell(i, col, FALSE, NULL);
        if((char)((long)AnsiUpper((char*)cellChar)) == typedChar)
          {
          row->SetSelection(1, &i);
          TABLE_ATTRIBUTES::SetActiveRow(i);
          ScrollToSelection(FALSE);
          PostObjectChanged(CHANGED_FULLREDRAW);
          AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
          return;
          }
        i++;
        }
      }
    }
  I_UNUSED(uMsg);
  I_UNUSED(lParam);
  }

// TABLE::StartTracking ////////////////////////////////////////////////////

void TABLE::StartTracking(
  int trackType)

  {
  if(!trackType)
    return;

  if(!(tracking & (TRACK_HIT | TRACK_COLUMNRESIZE))
      &&
     (trackType & (TRACK_HIT | TRACK_COLUMNRESIZE)))
    {
    OWndSetCaptureItem(this);
    }

  tracking |= trackType;

  if(tracking & TRACK_HIT)
    {
    AEvtInfo  event;
    event.sourceObject = theObject;
    event.targetObject = theObject;
    event.when         = AAppGetTime()+3;
    event.how          = AEVT_KILLALLQUEUES | AEVT_KILLPARTICULARS | AEVT_KILLSOURCE | AEVT_ATTAIL | AEVT_DIRECT;
    event.what         = (AEVENTID)trackCursor;
    event.details      = (long)this;
    AEvtPost(&event);
    }
  }

// TABLE::StopTracking /////////////////////////////////////////////////////

void TABLE::StopTracking(
  int trackType)

  {
  if(!trackType)
    return;

  tracking &= (~trackType);

  if((trackType & (TRACK_HIT | TRACK_COLUMNRESIZE))
      &&
     !(tracking & (TRACK_HIT | TRACK_COLUMNRESIZE)))
    {
    OWndReleaseCapture();
    }
  }

// TABLE::TrackCursor //////////////////////////////////////////////////////

void TABLE::TrackCursor(
  WPARAM  wParam,
  LPARAM  lParam)

  {
  I_UNUSED(lParam);

  POINT point;

  if(tracking & TRACK_TABLEACTIVE)
    {
    GetCursorPos(&point);           // point in Screen coords
    ScreenToClient(hTable, &point); // SKJ --> convert from screen to Parent coords
    if(CursorInResizeArea(point))
      {
      if(!oldCursor)
        {
        OWndSetCursor(G.hResizeCursor, FALSE);
        oldCursor = (HCURSOR)TRUE;
        }
      }
    else
      {
      if(oldCursor)
        {
        OWndReleaseCursor();
        oldCursor = NULL;
        }
      }
    }

  if((wParam & MK_LBUTTON) == FALSE) // PWR 7/14
    return ;

  GetCursorPos(&point);     // SKJ Jan. 20 , 1994
  ScreenToClient(m_hwndParent, &point);  // SKJ Jan.20, 1994

  // process the tracking as necessary
  point.x -= visRect.left;
  point.y -= visRect.top;
  if(tracking & TRACK_CELLHIT)
    {
    RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
    ROW*          row = rtime->GetRow();
    long          currentRow = RowFromPoint(point, row->NumRows());
    if((currentRow >=0) && (currentRow<row->NumRows()))
      {
      lastRow = currentRow;
      if(!row->IsSelected(currentRow))
        {
        row->SetSelection(1, &currentRow);

        // redraw the current visual instance
        TABLE_ATTRIBUTES::SetActiveRow(currentRow);
        InvalidateTableRect();
        }
      }
    }
  else if((tracking & TRACK_SHIFTHIT) && (wParam & MK_SHIFT))
    {
    RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
    ROW*          row = rtime->GetRow();
    long          currentRow = RowFromPoint(point, row->NumRows());
    if((currentRow >=0)
          &&
       (currentRow<row->NumRows())
          &&
       (currentRow!=lastRow))
      {
      lastRow = currentRow;
      row->RangeSelection(TABLE_ATTRIBUTES::AnchorRow(), currentRow);

      // redraw the current visual instance
      TABLE_ATTRIBUTES::SetActiveRow(currentRow);
      InvalidateTableRect();
      }
    }
  else if((tracking & TRACK_CONTROLHIT) && (wParam & MK_CONTROL))
    {
    RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
    ROW*          row = rtime->GetRow();
    long          currentRow = RowFromPoint(point, row->NumRows());
    if((currentRow >=0)
          &&
       (currentRow<row->NumRows())
          &&
       (currentRow!=lastRow))
      {
      long  i;
      long  anchor = TABLE_ATTRIBUTES::AnchorRow();
      BOOL  selected = row->IsSelected(anchor);
      long  start;
      long  stop;
      if(currentRow<=anchor)
        {
        start = currentRow;
        stop = anchor;
        }
      else
        {
        start = anchor+1;
        stop = currentRow+1;
        }

      if(selected)
        {
        row->SetSelection(numOldSelections, oldSelections);
        for(i=start; i<stop; i++)
          {
          if(!WasSelected(i))
            row->ToggleSelection(i);
          }
        }
      else
        {
        row->SetSelection(numOldSelections, oldSelections);
        for(i=start; i<stop; i++)
          {
          if(WasSelected(i))
            row->ToggleSelection(i);
          }
        }

      // redraw the current visual instance
      lastRow = currentRow;
      TABLE_ATTRIBUTES::SetActiveRow(currentRow);
      InvalidateTableRect();
      }
    }
  else if(tracking & TRACK_COLUMNRESIZE)
    {
    DrawNewColLine(point.x, TRUE);
    }
  }

// TABLE::DrawNewColLine ///////////////////////////////////////////////////

void TABLE::DrawNewColLine(
  int   x,
  BOOL  undoPrevious)

  {
  int tmp = colLineX + x - startX - 1;
  if(tmp>tblRect.right)
    tmp = tblRect.right;
  if(tmp < (colLineX - COLUMN::Pixels(resizeColumn) - 2))
    tmp = colLineX - COLUMN::Pixels(resizeColumn) - 2;

  if(undoPrevious && tmp==prevLine)
    return;

  HDC   hDC = GetDC(hTable);
  RECT  rect = tblRect;
  if(undoPrevious)
    {
    rect.left = rect.right = prevLine;
    rect.right++;
    InvertRect(hDC, &rect);
    }

  prevLine = tmp;
  rect.left = rect.right = prevLine;
  rect.right++;
  InvertRect(hDC, &rect);
  ReleaseDC(hTable, hDC);
  }

// TABLE::CursorInResizeArea ///////////////////////////////////////////////

BOOL TABLE::CursorInResizeArea(
  POINT point)

  {
  RECT  titleRect = tblRect;
  titleRect.bottom = titleRect.top + TABLE_ATTRIBUTES::TitleHeight();

  if(PtInRect(&titleRect, point))
    {
    // find the hit cell
    RECT  cellRect;
    cellRect.left = tblRect.left;
    for(int i=0; i<(NumColumns()+1) && cellRect.left < tblRect.right; i++)
      {
      if(ColumnWidth(i)==0)
        {
        if(TABLE_ATTRIBUTES::ColumnLines()
            &&
           (i == (TABLE_ATTRIBUTES::NumFixedColumns()-1))
            &&
           (cellRect.left != tblRect.left))
          {
          cellRect.left++;
          }
        continue;
        }

      cellRect.top = tblRect.top;
      cellRect.right = cellRect.left + ColumnWidth(i);
      if(TABLE_ATTRIBUTES::ColumnLines())
        {
        cellRect.right++;
        if((TABLE_ATTRIBUTES::NumFixedColumns()-1) == i)
          cellRect.right++;
        }

      if(TABLE_ATTRIBUTES::ShowTitles())
        {
        cellRect.bottom = cellRect.top + TABLE_ATTRIBUTES::TitleHeight();
        if(TABLE_ATTRIBUTES::RowLines())
          cellRect.bottom+=2;

        if(PtInRect(&cellRect, point))
          {
          if(TABLE_ATTRIBUTES::ColumnLines() && TABLE_ATTRIBUTES::CanResize())
            {
            if(abs(point.x - cellRect.left) <= COLUMN_RESIZEHITRANGE)
              {
              int column;
              if(i<TABLE_ATTRIBUTES::NumFixedColumns())
                column = i-1;
              else
                column = firstColumn + (i - TABLE_ATTRIBUTES::NumFixedColumns()) - 1;
              if(column==-1)
                return FALSE;

              if(i==0 && COLUMN::Pixels(column))
                return FALSE;

              return TRUE;
              }

            if(abs(point.x - cellRect.right) <= COLUMN_RESIZEHITRANGE)
              return TRUE;
            }
          }
        }

      cellRect.left = cellRect.right;
      }
    }

  return FALSE;
  }

// TABLE::RowFromPoint /////////////////////////////////////////////////////

long TABLE::RowFromPoint(
  POINT point,
  long  numRows)

  {
  int   top = tblRect.top;
  long  currentRow = firstRow;
  int   rowHeight = TABLE_ATTRIBUTES::CellHeight();

  if(TABLE_ATTRIBUTES::RowLines())
    rowHeight++;

  if(TABLE_ATTRIBUTES::ShowTitles())
    {
    top = tblRect.top + TABLE_ATTRIBUTES::TitleHeight();
    if(TABLE_ATTRIBUTES::RowLines())
      top+=2;
    }

  if((point.y < top) && (firstRow != 0))
    {
    firstRow--;
    top -= rowHeight;
    while((point.y < top) && (firstRow!=0))
      {
      firstRow--;
      top -= rowHeight;
      }

    if(hVertScroll)
      SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(numRows), TRUE);

    return firstRow;
    }

  top += rowHeight;
  while((top<tblRect.bottom) && (currentRow<numRows))
    {
    if(top>point.y)
      return currentRow;

    top += rowHeight;
    currentRow++;
    }

  if(currentRow >= numRows)
    return (numRows-1L);
  else
    {
    firstRow++;
    while((top<point.y) && (firstRow<(numRows-numVisRows)))
      {
      firstRow++;
      top += rowHeight;
      currentRow++;
      }

    if(hVertScroll)
      SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(numRows), TRUE);

    return currentRow;
    }   
  }

// TABLE::IsActiveCell /////////////////////////////////////////////////////

BOOL TABLE::IsActiveCell(
  long  row,
  int   column)

  {
  int trueCol = (column<TABLE_ATTRIBUTES::NumFixedColumns()) ? column : (firstColumn - TABLE_ATTRIBUTES::NumFixedColumns() + column);

  if((TABLE_ATTRIBUTES::ActiveRow()==row)
      &&
     (TABLE_ATTRIBUTES::ActiveColumn()==trueCol))
    return TRUE;
  else
    return FALSE;
  }

// TABLE::SelectionsDifferent //////////////////////////////////////////////

BOOL  TABLE::SelectionsDifferent()

  {
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();

  if(row->NumSelections()!=numOldSelections)
    return TRUE;

  for(int i=0; i<numOldSelections; i++)
    {
    if(row->GetSelection(i) != oldSelections[i])
      return TRUE;
    }

  return FALSE;
  }

// TABLE::WasSelected //////////////////////////////////////////////////////

BOOL TABLE::WasSelected(
  long  row)

  {
  for(int i=0; i<numOldSelections; i++)
    {
    if(oldSelections[i] == row)
      return TRUE;

    if(oldSelections[i] > row)
      return FALSE;
    }

  return FALSE;
  }

// TABLE::EscKey ///////////////////////////////////////////////////////////

void TABLE::EscKey()

  {
  if(!hEdit)
    return;

  // Get the rectangle of the cell for intelligent update
  RECT  cellRect;
  GetWindowRect(hEdit, &cellRect);
  ScreenToClient(hTable, (POINT*)&cellRect);
  ScreenToClient(hTable, (POINT*)&cellRect.right);
  cellRect.left -= COLUMN::CellCharWidth(editCol);
  cellRect.top = cellRect.bottom - TABLE_ATTRIBUTES::CellHeight();
  cellRect.right += COLUMN::CellCharWidth(editCol);

  // un-subclass the control in preparation for destroy
  RemoveProp(hEdit, "loPtr");
  RemoveProp(hEdit, "hiPtr");
  RemoveProp(hEdit, "number");
  SetWindowLong(hEdit, GWL_WNDPROC, (long)oldEditControlProc);

  // destroy the window
  DestroyWindow(hEdit);
  hEdit = NULL;
  DeleteObject(hEditFont);

  AEvtPostSignalAtMark(theObject, SIGNAL_STOPEDITING);

  // make sure the proper region is redrawn
  InvalidateTableRect();

  // wipPreventThieves(ParentID, FALSE); WM_SYSTEMKEY message
  }

// TABLE::ReturnKey ////////////////////////////////////////////////////////

void TABLE::ReturnKey()

  {
  long          currRow = TABLE_ATTRIBUTES::ActiveRow();
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();

  if(currRow>=0 && currRow<row->NumRows())
    {
    if(row->IsSelected(currRow) && row->NumSelections()>1)
      {
      row->SetSelection(1, &currRow);
      AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
      }
    }

  if(!row->IsSelected(currRow))
    {
    AEvtPostSignalAtMark(theObject, SIGNAL_RETURNPRESSED);
    return;
    }

  if(currRow<firstRow)
    {
    firstRow = currRow;
    if(hVertScroll)
      SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
    InvalidateTableRect();
    }
  else if(currRow>(firstRow+numVisRows-1))
    {
    firstRow = currRow - numVisRows + 1;
    if(firstRow<0L)
      firstRow = 0L;
    if(hVertScroll)
      SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
    InvalidateTableRect();
    }

  if(hEdit)
    DestroyEditControl();
  else
    {
    int   col = TABLE_ATTRIBUTES::ActiveColumn();
    BOOL  changed = FALSE;
    while(col<COLUMN::NumColumns() && COLUMN::ColumnWidth(col)==0)
      {
      col++;
      changed=TRUE;
      }

    if(col>=TABLE_ATTRIBUTES::NumFixedColumns() && col<firstColumn)
      {
      firstColumn = col;
      changed = TRUE;

      if(hHorzScroll)
        SetScrollPos(hHorzScroll, SB_CTL, FirstColumnToScrollPos(), TRUE);
      }
    else if(col>firstColumn)
      {
      int i = firstColumn;
      int width = COLUMN::ColumnWidth(i++);
      changed = TRUE;
      while(i<=col)
        {
        if(COLUMN::ColumnWidth(i)==0)
          {
          i++;
          continue;
          }

        if(width>0 && TABLE_ATTRIBUTES::ColumnLines())
          width++;

        width += COLUMN::ColumnWidth(i++);
        }

      if(width>freeSpace)
        { // get a new first column
        i=col;
        width = COLUMN::ColumnWidth(i);
        while(width<freeSpace)
          {
          i--;

          if(COLUMN::ColumnWidth(i)==0)
            continue;

          if(TABLE_ATTRIBUTES::ColumnLines())
            width++;

          width += COLUMN::ColumnWidth(i);
          if(width>freeSpace)
            i++;
          }

        firstColumn = i;

        if(hHorzScroll)
          SetScrollPos(hHorzScroll, SB_CTL, FirstColumnToScrollPos(), TRUE);
        }
      }

    if(changed)
      {
      TABLE_ATTRIBUTES::SetActiveColumn(col);
      DrawAllInstances();
      }
    CreateEditControl();
    }

  AEvtPostSignalAtMark(theObject, SIGNAL_RETURNPRESSED);
  }

// TABLE::TabKey ///////////////////////////////////////////////////////////

void TABLE::TabKey(
  BOOL  shift)

  {
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  if(row->NumSelections()==0)
    {
    if(shift)
      {
      OpWndItemD *nextItem = OWndGetPreviousItem(this, NULL);
      if(!nextItem)
        nextItem = OWndGetPreviousItem(NULL, m_oiParent);
      if (nextItem != this)
        OWndSetActiveItem(nextItem);
      }
    else
      {
      OpWndItemD *nextItem = OWndGetNextItem(this, NULL);
      if(!nextItem)
        nextItem = OWndGetNextItem(NULL, m_oiParent);
      if (nextItem != this)
        OWndSetActiveItem(nextItem);
      }

    return;
    }

  long  currRow = TABLE_ATTRIBUTES::ActiveRow();
  BOOL  changed = FALSE;

  if((row->NumSelections()!=1) || !row->IsSelected(currRow))
    {
    row->SetSelection(1, &currRow);
    AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
    changed = TRUE;
    }

  if(shift)
    {
    if(TABLE_ATTRIBUTES::ActiveColumn()==0)
      {
      if(row->NumRows()>0 && currRow>0)
        {
        currRow--;
        if(currRow<firstRow)
          {
          firstRow = currRow;
          if(hVertScroll)
            SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
          }
        else if(currRow>(firstRow+numVisRows-1))
          {
          firstRow = currRow - numVisRows + 1;
          if(firstRow<0L)
            firstRow = 0L;
          if(hVertScroll)
            SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
          }

        firstColumn = ScrollPosToFirstColumn(HScrollSpace);
        int col = COLUMN::NumColumns()-1;
        while(col>0 && COLUMN::ColumnWidth(col)==0)
          col--;

        if(hHorzScroll)
          SetScrollPos(hHorzScroll, SB_CTL, FirstColumnToScrollPos(), TRUE);

        TABLE_ATTRIBUTES::SetActiveColumn(col);
        TABLE_ATTRIBUTES::SetActiveRow(currRow);
        row->SetSelection(1, &currRow);
        if(!changed)
          AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);

        DestroyEditControl();
        DrawAllInstances();
        CreateEditControl();
        }
      else
        { // current row==0 && currentCol==0
        OpWndItemD *nextItem = OWndGetPreviousItem(this, NULL);
        if(!nextItem)
          nextItem = OWndGetPreviousItem(NULL, m_oiParent);
        if (nextItem)
          OWndSetActiveItem(nextItem);
        }
      }
    else
      {
      if(currRow<firstRow)
        {
        firstRow = currRow;
        if(hVertScroll)
          SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
        }
      else if(currRow>(firstRow+numVisRows-1))
        {
        firstRow = currRow - numVisRows + 1;
        if(firstRow<0L)
          firstRow = 0L;
        if(hVertScroll)
          SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
        }
      int col = TABLE_ATTRIBUTES::ActiveColumn()-1;
      while(col>0 && COLUMN::ColumnWidth(col)==0)
        col--;

      if(col>=TABLE_ATTRIBUTES::NumFixedColumns() && col<firstColumn)
        {
        firstColumn = col;

        if(hHorzScroll)
          SetScrollPos(hHorzScroll, SB_CTL, FirstColumnToScrollPos(), TRUE);
        }

      TABLE_ATTRIBUTES::SetActiveColumn(col);
      DestroyEditControl();
      DrawAllInstances();
      CreateEditControl();
      }
    }
  else // shift==FALSE !!!
    {
    int lastCol = COLUMN::NumColumns()-1;
    while(lastCol>0 && COLUMN::ColumnWidth(lastCol)==0)
      lastCol--;

    if(TABLE_ATTRIBUTES::ActiveColumn()>=lastCol)
      {
      if(row->NumRows()>0 && currRow<(row->NumRows()-1))
        {
        currRow++;
        if(currRow<firstRow)
          {
          firstRow = currRow;
          if(hVertScroll)
            SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
          }
        else if(currRow>(firstRow+numVisRows-1))
          {
          firstRow = currRow - numVisRows + 1;
          if(firstRow<0L)
            firstRow = 0L;
          if(hVertScroll)
            SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
          }

        int col = 0;
        while(col<COLUMN::NumColumns() && COLUMN::ColumnWidth(col)==0)
          col++;

        if(col>=TABLE_ATTRIBUTES::NumFixedColumns())
          firstColumn = ScrollPosToFirstColumn(0);

        if(hHorzScroll)
          SetScrollPos(hHorzScroll, SB_CTL, FirstColumnToScrollPos(), TRUE);

        TABLE_ATTRIBUTES::SetActiveColumn(col);
        TABLE_ATTRIBUTES::SetActiveRow(currRow);
        row->SetSelection(1, &currRow);
        if(!changed)
          AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);

        DestroyEditControl();
        DrawAllInstances();
        CreateEditControl();
        }
      else
        { // row==last row && column==last column
        OpWndItemD *nextItem = OWndGetNextItem(this, NULL);
        if(!nextItem)
          nextItem = OWndGetNextItem(NULL, m_oiParent);
        if (nextItem)
          OWndSetActiveItem(nextItem);
        }
      }
    else
      {
      if(currRow<firstRow)
        {
        firstRow = currRow;
        if(hVertScroll)
          SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
        }
      else if(currRow>(firstRow+numVisRows-1))
        {
        firstRow = currRow - numVisRows + 1;
        if(firstRow<0L)
          firstRow = 0L;
        if(hVertScroll)
          SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
        }
      int col = TABLE_ATTRIBUTES::ActiveColumn()+1;
      while(col<COLUMN::NumColumns() && COLUMN::ColumnWidth(col)==0)
        col++;

      if(col>=TABLE_ATTRIBUTES::NumFixedColumns() && col<firstColumn)
        {
        firstColumn = col;

        if(hHorzScroll)
          SetScrollPos(hHorzScroll, SB_CTL, FirstColumnToScrollPos(), TRUE);
        }
      else if(col>firstColumn)
        {
        int i = firstColumn;
        int width = COLUMN::ColumnWidth(i++);
        while(i<=col)
          {
          if(COLUMN::ColumnWidth(i)==0)
            {
            i++;
            continue;
            }

          if(width>0 && TABLE_ATTRIBUTES::ColumnLines())
            width++;

          width += COLUMN::ColumnWidth(i++);
          }

        if(width>freeSpace)
          { // get a new first column
          i=col;
          width = COLUMN::ColumnWidth(i);
          while(width<freeSpace)
            {
            i--;

            if(COLUMN::ColumnWidth(i)==0)
              continue;

            if(TABLE_ATTRIBUTES::ColumnLines())
              width++;

            width += COLUMN::ColumnWidth(i);
            if(width>freeSpace)
              i++;
            }

          firstColumn = i;

          if(hHorzScroll)
            SetScrollPos(hHorzScroll, SB_CTL, FirstColumnToScrollPos(), TRUE);
          }
        }

      TABLE_ATTRIBUTES::SetActiveColumn(col);
      DestroyEditControl();
      DrawAllInstances();
      CreateEditControl();
      }
    }
  }

// TABLE::UpKey ////////////////////////////////////////////////////////////

BOOL TABLE::UpKey(
  BOOL  shift)

  {
  long          activeRow = TABLE_ATTRIBUTES::ActiveRow();
  long          anchorRow = TABLE_ATTRIBUTES::AnchorRow();
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  if(activeRow<0L)
    {
    activeRow=0;
    if(TABLE_ATTRIBUTES::ActiveColumn()<0)
      TABLE_ATTRIBUTES::SetActiveColumn(0);
    }
  else
    activeRow--;

  if(activeRow<0 || activeRow>=row->NumRows())
    return FALSE;

  if(activeRow<firstRow)
    {
    firstRow = activeRow;
    if(hVertScroll)
      SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
    }
  else if(activeRow>(firstRow+numVisRows-1))
    {
    firstRow = activeRow - numVisRows + 1;
    if(firstRow<0L)
      firstRow = 0L;
    if(hVertScroll)
      SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
    }

  TABLE_ATTRIBUTES::SetActiveRow(activeRow);
  if(hEdit)
    {
    row->SetSelection(1, &activeRow);
    TABLE_ATTRIBUTES::SetAnchorRow(activeRow);
    DestroyEditControl();
    DrawAllInstances();
    CreateEditControl();
    }
  else if(shift)
    {
    row->RangeSelection(anchorRow, activeRow);
    PostObjectChanged(CHANGED_FULLREDRAW);
    }
  else
    {
    row->SetSelection(1, &activeRow);
    TABLE_ATTRIBUTES::SetAnchorRow(activeRow);
    PostObjectChanged(CHANGED_FULLREDRAW);
    }

  AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
  return TRUE;
  }

// TABLE::DownKey //////////////////////////////////////////////////////////

BOOL TABLE::DownKey(
  BOOL  shift)

  {
  long          activeRow = TABLE_ATTRIBUTES::ActiveRow();
  long          anchorRow = TABLE_ATTRIBUTES::AnchorRow();
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  if(activeRow<0L)
    {
    activeRow=0L;
    if(TABLE_ATTRIBUTES::ActiveColumn()<0)
      TABLE_ATTRIBUTES::SetActiveColumn(0);
    }
  else
    activeRow++;

  if(activeRow<0 || activeRow>=row->NumRows())
    return FALSE;

  if(activeRow<firstRow)
    {
    firstRow = activeRow;
    if(hVertScroll)
      SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
    }
  else if(activeRow>(firstRow+numVisRows-1))
    {
    firstRow = activeRow - numVisRows + 1;
    if(firstRow<0L)
      firstRow = 0L;
    if(hVertScroll)
      SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
    }

  TABLE_ATTRIBUTES::SetActiveRow(activeRow);
  if(hEdit)
    {
    row->SetSelection(1, &activeRow);
    TABLE_ATTRIBUTES::SetAnchorRow(activeRow);
    DestroyEditControl();
    DrawAllInstances();
    CreateEditControl();
    }
  else if(shift)
    {
    row->RangeSelection(anchorRow, activeRow);
    PostObjectChanged(CHANGED_FULLREDRAW);
    }
  else
    {
    row->SetSelection(1, &activeRow);
    TABLE_ATTRIBUTES::SetAnchorRow(activeRow);
    PostObjectChanged(CHANGED_FULLREDRAW);
    }

  AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
  return TRUE;
  }

// TABLE::LeftKey //////////////////////////////////////////////////////////

BOOL TABLE::LeftKey(
  BOOL  shift)

  {
  if(hEdit)
    return FALSE;

  long          currRow = TABLE_ATTRIBUTES::ActiveRow();
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  BOOL          changed = FALSE;

  if(!shift && (row->NumSelections()!=1) || !row->IsSelected(currRow))
    {
    row->SetSelection(1, &currRow);
    ScrollToSelection();
    AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
    changed = TRUE;
    }
  else if(shift && !row->IsSelected(currRow))
    {
    if(TABLE_ATTRIBUTES::MultiSelect())
      row->ToggleSelection(currRow);
    else
      row->SetSelection(1, &currRow);

    AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
    changed = TRUE;
    }

  if(TABLE_ATTRIBUTES::ActiveColumn()==0)
    {
    if(row->NumRows()>0 && currRow>0)
      {
      currRow--;
      if(currRow<firstRow)
        {
        firstRow = currRow;
        if(hVertScroll)
          SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
        }
      else if(currRow>(firstRow+numVisRows-1))
        {
        firstRow = currRow - numVisRows + 1;
        if(firstRow<0L)
          firstRow = 0L;
        if(hVertScroll)
          SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
        }
      firstColumn = ScrollPosToFirstColumn(HScrollSpace);
      int col = COLUMN::NumColumns()-1;
      while(col>0 && COLUMN::ColumnWidth(col)==0)
        col--;

      if(hHorzScroll)
        SetScrollPos(hHorzScroll, SB_CTL, FirstColumnToScrollPos(), TRUE);

      TABLE_ATTRIBUTES::SetActiveColumn(col);
      TABLE_ATTRIBUTES::SetActiveRow(currRow);
      if(shift && TABLE_ATTRIBUTES::MultiSelect())
        {
        if(!row->IsSelected(currRow))
          row->ToggleSelection(currRow);
        }
      else
        {
        row->SetSelection(1, &currRow);
        TABLE_ATTRIBUTES::SetAnchorRow(currRow);
        }
      if(!changed)
        AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);

      PostObjectChanged(CHANGED_FULLREDRAW);
      }
    }
  else
    {
    if(currRow<firstRow)
      {
      firstRow = currRow;
      if(hVertScroll)
        SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
      }
    else if(currRow>(firstRow+numVisRows-1))
      {
      firstRow = currRow - numVisRows + 1;
      if(firstRow<0L)
        firstRow = 0L;
      if(hVertScroll)
        SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
      }
    int col = TABLE_ATTRIBUTES::ActiveColumn()-1;
    while(col>0 && COLUMN::ColumnWidth(col)==0)
      col--;

    if(col>=TABLE_ATTRIBUTES::NumFixedColumns() && col<firstColumn)
      {
      firstColumn = col;

      if(hHorzScroll)
        SetScrollPos(hHorzScroll, SB_CTL, FirstColumnToScrollPos(), TRUE);
      }

    TABLE_ATTRIBUTES::SetActiveColumn(col);
    PostObjectChanged(CHANGED_FULLREDRAW);
    }

  return TRUE;
  }

// TABLE::RightKey /////////////////////////////////////////////////////////

BOOL TABLE::RightKey(
  BOOL  shift)

  {
  if(hEdit)
    return FALSE;

  long          currRow = TABLE_ATTRIBUTES::ActiveRow();
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  BOOL          changed = FALSE;

  if(!shift && (row->NumSelections()!=1) || !row->IsSelected(currRow))
    {
    row->SetSelection(1, &currRow);
    ScrollToSelection();
    AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
    changed = TRUE;
    }
  else if(shift && !row->IsSelected(currRow))
    {
    if(TABLE_ATTRIBUTES::MultiSelect())
      row->ToggleSelection(currRow);
    else
      row->SetSelection(1, &currRow);

    AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
    changed = TRUE;
    }

  int lastCol = COLUMN::NumColumns()-1;
  while(lastCol>0 && COLUMN::ColumnWidth(lastCol)==0)
    lastCol--;

  if(TABLE_ATTRIBUTES::ActiveColumn()>=lastCol)
    {
    if(row->NumRows()>0 && currRow<(row->NumRows()-1))
      {
      currRow++;
      if(currRow<firstRow)
        {
        firstRow = currRow;
        if(hVertScroll)
          SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
        }
      else if(currRow>(firstRow+numVisRows-1))
        {
        firstRow = currRow - numVisRows + 1;
        if(firstRow<0L)
          firstRow = 0L;
        if(hVertScroll)
          SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
        }

      int col = 0;
      while(col<COLUMN::NumColumns() && COLUMN::ColumnWidth(col)==0)
        col++;

      if(col>=TABLE_ATTRIBUTES::NumFixedColumns())
        firstColumn = ScrollPosToFirstColumn(0);

      if(hHorzScroll)
        SetScrollPos(hHorzScroll, SB_CTL, FirstColumnToScrollPos(), TRUE);

      TABLE_ATTRIBUTES::SetActiveColumn(col);
      TABLE_ATTRIBUTES::SetActiveRow(currRow);
      if(shift && TABLE_ATTRIBUTES::MultiSelect())
        {
        if(!row->IsSelected(currRow))
          row->ToggleSelection(currRow);
        }
      else
        {
        row->SetSelection(1, &currRow);
        TABLE_ATTRIBUTES::SetAnchorRow(currRow);
        }
      if(!changed)
        AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);

      PostObjectChanged(CHANGED_FULLREDRAW);
      }
    }
  else
    {
    if(currRow<firstRow)
      {
      firstRow = currRow;
      if(hVertScroll)
        SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
      }
    else if(currRow>(firstRow+numVisRows-1))
      {
      firstRow = currRow - numVisRows + 1;
      if(firstRow<0L)
        firstRow = 0L;
      if(hVertScroll)
        SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
      }

    int col = TABLE_ATTRIBUTES::ActiveColumn()+1;
    while(col<COLUMN::NumColumns() && COLUMN::ColumnWidth(col)==0)
      col++;

    if(col>=TABLE_ATTRIBUTES::NumFixedColumns() && col<firstColumn)
      {
      firstColumn = col;

      if(hHorzScroll)
        SetScrollPos(hHorzScroll, SB_CTL, FirstColumnToScrollPos(), TRUE);
      }
    else if(col>firstColumn)
      {
      int i = firstColumn;
      int width = COLUMN::ColumnWidth(i++);
      while(i<=col)
        {
        if(COLUMN::ColumnWidth(i)==0)
          {
          i++;
          continue;
          }

        if(width>0 && TABLE_ATTRIBUTES::ColumnLines())
          width++;

        width += COLUMN::ColumnWidth(i++);
        }

      if(width>freeSpace)
        { // get a new first column
        i=col;
        width = COLUMN::ColumnWidth(i);
        while(width<freeSpace)
          {
          i--;

          if(COLUMN::ColumnWidth(i)==0)
            continue;

          if(TABLE_ATTRIBUTES::ColumnLines())
            width++;

          width += COLUMN::ColumnWidth(i);
          if(width>freeSpace)
            i++;
          }

        firstColumn = i;

        if(hHorzScroll)
          SetScrollPos(hHorzScroll, SB_CTL, FirstColumnToScrollPos(), TRUE);
        }
      }

    TABLE_ATTRIBUTES::SetActiveColumn(col);
    PostObjectChanged(CHANGED_FULLREDRAW);
    }

  return TRUE;
  }

// TABLE::PageUpKey ////////////////////////////////////////////////////////

BOOL TABLE::PageUpKey(
  BOOL  control,
  BOOL  shift)

  {
  if(control)
    return FALSE;

  long  activeRow = TABLE_ATTRIBUTES::ActiveRow();
  long  anchorRow = TABLE_ATTRIBUTES::AnchorRow();
  if(activeRow==0L)
    return FALSE;

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  if(activeRow==firstRow)
    activeRow -= numVisRows;
  else
    activeRow = firstRow;

  if(activeRow<0L)
    activeRow=0L;

  if(activeRow>=row->NumRows())
    return FALSE;

  if(activeRow<firstRow)
    {
    firstRow = activeRow;
    if(hVertScroll)
      SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
    }
  else if(activeRow>(firstRow+numVisRows-1))
    {
    firstRow = activeRow - numVisRows + 1;
    if(firstRow<0L)
      firstRow = 0L;
    if(hVertScroll)
      SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
    }

  TABLE_ATTRIBUTES::SetActiveRow(activeRow);
  if(shift)
    row->RangeSelection(anchorRow, activeRow);
  else
    {
    row->SetSelection(1, &activeRow);
    TABLE_ATTRIBUTES::SetAnchorRow(activeRow);
    }

  if(hEdit)
    {
    DestroyEditControl();
    DrawAllInstances();
    CreateEditControl();
    }
  else
    PostObjectChanged(CHANGED_FULLREDRAW);

  AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
  return TRUE;
  }

// TABLE::PageDownKey //////////////////////////////////////////////////////

BOOL TABLE::PageDownKey(
  BOOL  control,
  BOOL  shift)

  {
  if(control)
    return FALSE;

  long          activeRow = TABLE_ATTRIBUTES::ActiveRow();
  long          anchorRow = TABLE_ATTRIBUTES::AnchorRow();
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  if(activeRow==(row->NumRows()-1))
    return FALSE;

  if(activeRow==(firstRow+numVisRows-1))
    activeRow += numVisRows;
  else
    activeRow = firstRow + numVisRows - 1;

  if(activeRow>=row->NumRows())
    activeRow=row->NumRows()-1;

  if(activeRow<0L)
    return FALSE;

  if(activeRow<firstRow)
    {
    firstRow = activeRow;
    if(hVertScroll)
      SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
    }
  else if(activeRow>(firstRow+numVisRows-1))
    {
    firstRow = activeRow - numVisRows + 1;
    if(firstRow<0L)
      firstRow = 0L;
    if(hVertScroll)
      SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
    }

  TABLE_ATTRIBUTES::SetActiveRow(activeRow);
  if(shift)
    row->RangeSelection(anchorRow, activeRow);
  else
    {
    row->SetSelection(1, &activeRow);
    TABLE_ATTRIBUTES::SetAnchorRow(activeRow);
    }

  if(hEdit)
    {
    DestroyEditControl();
    DrawAllInstances();
    CreateEditControl();
    }
  else
    PostObjectChanged(CHANGED_FULLREDRAW);

  AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
  return TRUE;
  }

// TABLE::HomeKey //////////////////////////////////////////////////////////

BOOL TABLE::HomeKey(
  BOOL  control,
  BOOL  shift)

  {
  if(hEdit)
    return FALSE;

  long          activeRow = TABLE_ATTRIBUTES::ActiveRow();
  long          anchorRow = TABLE_ATTRIBUTES::AnchorRow();
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();

  // scroll the columns to the first column
  if(firstColumn != ScrollPosToFirstColumn(0))
    {
    firstColumn = ScrollPosToFirstColumn(0);
    PostObjectChanged(CHANGED_FULLREDRAW);

    if(hHorzScroll)
      SetScrollPos(hHorzScroll, SB_CTL, FirstColumnToScrollPos(), TRUE);
    }

  int col = 0;
  while(col<COLUMN::NumColumns() && COLUMN::ColumnWidth(col)==0)
    col++;

  if(col != TABLE_ATTRIBUTES::ActiveColumn())
    {
    TABLE_ATTRIBUTES::SetActiveColumn(col);
    PostObjectChanged(CHANGED_FULLREDRAW);
    }

  if(control)
    {
    if(activeRow<0 || activeRow>=row->NumRows())
      return FALSE;

    if(activeRow<firstRow)
      {
      firstRow = activeRow;
      if(hVertScroll)
        SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
      }
    else if(activeRow>(firstRow+numVisRows-1))
      {
      firstRow = activeRow - numVisRows + 1;
      if(firstRow<0L)
        firstRow = 0L;
      if(hVertScroll)
        SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
      }

    return TRUE;
    }

  if((activeRow==0)
      &&
     (firstRow==0)
      &&
     (TABLE_ATTRIBUTES::ActiveColumn()==0))
    return FALSE;

  activeRow = 0;

  if(activeRow<0L || activeRow>=row->NumRows())
    return FALSE;

  firstRow = 0;
  if(hVertScroll)
    SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);

  TABLE_ATTRIBUTES::SetActiveRow(activeRow);
  TABLE_ATTRIBUTES::SetActiveColumn(0);
  if(shift)
    row->RangeSelection(anchorRow, activeRow);
  else
    {
    row->SetSelection(1, &activeRow);
    TABLE_ATTRIBUTES::SetAnchorRow(activeRow);
    }

  AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
  PostObjectChanged(CHANGED_FULLREDRAW);
  return TRUE;
  }

// TABLE::EndKey ///////////////////////////////////////////////////////////

BOOL TABLE::EndKey(
  BOOL  control,
  BOOL  shift)

  {
  if(hEdit)
    return FALSE;

  long          activeRow = TABLE_ATTRIBUTES::ActiveRow();
  long          anchorRow = TABLE_ATTRIBUTES::AnchorRow();
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();

  // scroll to the last column
  if(firstColumn != ScrollPosToFirstColumn(HScrollSpace))
    {
    firstColumn = ScrollPosToFirstColumn(HScrollSpace);
    PostObjectChanged(CHANGED_FULLREDRAW);

    if(hHorzScroll)
      SetScrollPos(hHorzScroll, SB_CTL, FirstColumnToScrollPos(), TRUE);
    }

  int col = COLUMN::NumColumns()-1;
  while(col>0 && COLUMN::ColumnWidth(col)==0)
    col--;

  if(col != TABLE_ATTRIBUTES::ActiveColumn())
    {
    TABLE_ATTRIBUTES::SetActiveColumn(col);
    PostObjectChanged(CHANGED_FULLREDRAW);
    }

  if(control)
    {
    if(activeRow<0 || activeRow>=row->NumRows())
      return FALSE;

    if(activeRow<firstRow)
      {
      firstRow = activeRow;
      if(hVertScroll)
        SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
      }
    else if(activeRow>(firstRow+numVisRows-1))
      {
      firstRow = activeRow - numVisRows + 1;
      if(firstRow<0L)
        firstRow = 0L;
      if(hVertScroll)
        SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
      }
    return TRUE;
    }

  if((activeRow==(row->NumRows() - 1))
      &&
     (firstRow==(row->NumRows() - numVisRows))
      &&
     (TABLE_ATTRIBUTES::ActiveColumn()==(COLUMN::NumColumns()-1)))
    return FALSE;

  activeRow=row->NumRows()-1;

  if(activeRow>(firstRow+numVisRows-1))
    {
    firstRow = activeRow - numVisRows + 1;
    if(firstRow<0L)
      firstRow = 0L;
    if(hVertScroll)
      SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
    }

  TABLE_ATTRIBUTES::SetActiveRow(activeRow);
  if(shift)
    row->RangeSelection(anchorRow, activeRow);
  else
    {
    row->SetSelection(1, &activeRow);
    TABLE_ATTRIBUTES::SetAnchorRow(activeRow);
    }

  AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
  PostObjectChanged(CHANGED_FULLREDRAW);
  return TRUE;
  }

// TABLE::DeleteKey ////////////////////////////////////////////////////////

BOOL TABLE::DeleteKey()

  {
  if(hEdit)
    return FALSE;
  else
    {
    AEvtPostSignalAtMark(theObject, SIGNAL_DELETEPRESSED);
    return TRUE;
    }
  }

// TABLE::DrawAllInstances /////////////////////////////////////////////////

void TABLE::DrawAllInstances()

  {
  OWndCallEachVisualInstance(theObject, RedrawInstance, 0L, 0L);
  }

////////////////////////////////////////////////////////////////////////////

BOOL ALMCALLBACK TABLE::RedrawInstance
  (
  OpWndItemD* wi,
  LPARAM      lParam1,
  LPARAM      lParam2
  )

  {
  I_UNUSED(lParam1);
  I_UNUSED(lParam2);

  ((TABLE*)wi)->InvalidateTableRect();

  return(TRUE);
  }

////////////////////////////////////////////////////////////////////////////
long Tbl_AdjustNewRect(
  RECT *newRect,
  LONG adjustFlag)
  {
  long        rVal = 0L;
  int         tmp;

  if (newRect->bottom < 0 )
    {
    newRect->bottom = newRect->top + I_DEFAULT_HEIGHT;
    rVal |= (OWND_ADJRECT_CHANGED_B | OWND_ADJRECT_DEFAULT_H);
    }
  else if (newRect->bottom < newRect->top)
    {
    tmp = newRect->bottom;
    newRect->bottom = newRect->top;
    newRect->top = tmp;
    rVal |= OWND_ADJRECT_SWAPPED_TB;
    if (adjustFlag & OWND_ADJRECT_ANCHOR_B)
      { //if we swap the sides, we need to swap the anchor points
      adjustFlag &= ~OWND_ADJRECT_ANCHOR_B;
      adjustFlag |= OWND_ADJRECT_ANCHOR_T;
      }
    else
      {
      adjustFlag &= ~OWND_ADJRECT_ANCHOR_T;
      adjustFlag |= OWND_ADJRECT_ANCHOR_B;
      }
    }

  if (newRect->right < 0)
    {
    newRect->right = newRect->left + I_DEFAULT_WIDTH;
    rVal |= (OWND_ADJRECT_CHANGED_R | OWND_ADJRECT_DEFAULT_W);
    }
  else if (newRect->right < newRect->left)
    {
    tmp = newRect->right;
    newRect->right = newRect->left;
    newRect->left = tmp;
    rVal |= OWND_ADJRECT_SWAPPED_LR;
    if (adjustFlag & OWND_ADJRECT_ANCHOR_R)
      { //if we swap the sides, we need to swap the anchor points
      adjustFlag &= ~OWND_ADJRECT_ANCHOR_R;
      adjustFlag |= OWND_ADJRECT_ANCHOR_L;
      }
    else
      {
      adjustFlag &= ~OWND_ADJRECT_ANCHOR_L;
      adjustFlag |= OWND_ADJRECT_ANCHOR_R;
      }
    }
  // At this point, the rectangle is not flipped inside-out
  // Make sure it is a minimum height and width
  if (newRect->bottom - newRect->top < I_MIN_HEIGHT)
    {
    if (adjustFlag & OWND_ADJRECT_ANCHOR_B)
      {
      newRect->top = newRect->bottom - I_MIN_HEIGHT;
      rVal |= OWND_ADJRECT_CHANGED_T;
      }
    else
      {
      newRect->bottom = newRect->top + I_MIN_HEIGHT;
      rVal |= OWND_ADJRECT_CHANGED_B;
      }
    }

  if (newRect->right - newRect->left < I_MIN_WIDTH)
    { 
    if (adjustFlag & OWND_ADJRECT_ANCHOR_R)
      {
      newRect->left = newRect->right - I_MIN_WIDTH;
      rVal |= OWND_ADJRECT_CHANGED_L;
      }
    else
      {
      newRect->right = newRect->left + I_MIN_WIDTH;
      rVal |= OWND_ADJRECT_CHANGED_R;
      }
    }
  return rVal;
  }

////////////////////////////////////////////////////////////////////////////

BOOL TABLE::FindCell
  (
  LPPOINT lppt,
  LPLONG  plRow,
  LPINT   pnCol
  )

  {
  POINT point = *lppt;
  long  lRow  = -1L;
  int   nCol  = -1;
  BOOL  fRet  = FALSE;

  if(hTable)
    MapWindowPoints(HWND_DESKTOP, hTable, &point, 1);

  if(hTable && PtInRect(&tblRect, point))
    {
    RECT  cellRect;
    BOOL  notFound = TRUE;
    cellRect.left = tblRect.left;
    for(int i=0; i<=NumColumns() && notFound && cellRect.left < tblRect.right; i++)
      {
      if(ColumnWidth(i)==0)
        {
        if((i == TABLE_ATTRIBUTES::NumFixedColumns()-1)
            &&
           (cellRect.left != tblRect.left))
          {
          cellRect.left++;
          }
        continue;
        }

      cellRect.top = tblRect.top;
      cellRect.right = cellRect.left + ColumnWidth(i);
      if(TABLE_ATTRIBUTES::ColumnLines() && ColumnWidth(i)>0)
        cellRect.right++;
      if(TABLE_ATTRIBUTES::NumFixedColumns() == (i-1))
        cellRect.right++;

      if(TABLE_ATTRIBUTES::ShowTitles())
        {
        cellRect.bottom = cellRect.top + TABLE_ATTRIBUTES::TitleHeight();
        if(TABLE_ATTRIBUTES::RowLines())
          cellRect.bottom+=2;
        else
          cellRect.bottom++;

        if(PtInRect(&cellRect, point))
          {
          notFound = FALSE;
          break;
          }

        cellRect.top = cellRect.bottom;
        }

      for(int j=0; cellRect.top < tblRect.bottom; j++)
        {
        cellRect.bottom = cellRect.top + TABLE_ATTRIBUTES::CellHeight();
        if(TABLE_ATTRIBUTES::RowLines())
          cellRect.bottom++;

        if(PtInRect(&cellRect, point))
          {
          notFound = FALSE;

          RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
          ROW*          row = rtime->GetRow();

          lRow = j + firstRow;
          nCol = (i<TABLE_ATTRIBUTES::NumFixedColumns()) ? i : (firstColumn + i - TABLE_ATTRIBUTES::NumFixedColumns());
          fRet = (lRow < row->NumRows());
          break;
          }

        cellRect.top = cellRect.bottom;
        }

      cellRect.left = cellRect.right;
      }
    }

  if(fRet)
    {
    if(plRow)
      *plRow = lRow;
    if(pnCol)
      *pnCol = nCol;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
