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
// FILE:  "table3.cpp"
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
#include  <string.h>

#include  <a_alm.h>
#include  <a_almutl.h>
#include  <o_number.h>

#include  "tabl_hdr.h"  // for hInstance
#include  "table.h"
#include  "tabl_row.h"

// Externals ///////////////////////////////////////////////////////////////

extern GLOBAL G;

// EditControlProc /////////////////////////////////////////////////////////

long CALLBACK /*!!PORT!! was _far_ _pascal_*/  EditControlProc(
  HWND    hWnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam)

  {
  TABLE*  table = (TABLE*)MAKELONG((UINT)GetProp(hWnd, "loPtr"), (UINT)GetProp(hWnd, "hiPtr"));
  BOOL    number = (BOOL)GetProp(hWnd, "number");
  BOOL    destroy = FALSE;

  switch(uMsg)
    {
    case WM_KEYDOWN:
      // The parent table never gets the keydown messages when they are 
      // being intercepted by the edit control.  So I need to post the
      // keydown message back to the parent window (the table window) 
      PostMessage(GetParent(hWnd), EDITM_KEYDOWN, wParam, lParam);
      break;

    case WM_CHAR:
      if(wParam==VK_TAB)
        return 0L;
      else if(number)
        {
        switch(wParam)
          {
          case '-':
          case '+':
          case 'e':
          case 'E':
          case '.':
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
          case VK_BACK:
          case VK_CLEAR:
          case VK_HOME:
          case VK_END:
            break;

          default:
            MessageBeep(-1);
            return 0L;
          }
        }
      break;

    case WM_LBUTTONDBLCLK:
      if(table->dblClkWait)
        {
        AEvtPostSignalAtMark(table->theObject, SIGNAL_ROWDOUBLECLICKED);
        destroy = TRUE;
        }
      break;

    default:
      break;
    }

  if(destroy)
    {
    table->DestroyEditControl();
    return 0L;
    }
  else
    {
    #ifdef STRICT
    return CallWindowProc((WNDPROC)(table->oldEditControlProc), hWnd, uMsg, wParam, lParam);
    #else
    return CallWindowProc((FARPROC)table->oldEditControlProc, hWnd, uMsg, wParam, lParam);
    #endif
    }
  }

// TABLE::CreateEditControl ////////////////////////////////////////////////

void TABLE::CreateEditControl()

  {
  if(hEdit || TABLE_ATTRIBUTES::AutoFill() || !TABLE_ATTRIBUTES::CanActivate())
    return;

  long  activeRow = TABLE_ATTRIBUTES::ActiveRow();
  if(activeRow<0)
    return;

  if((activeRow<firstRow) || (activeRow>(firstRow+numVisRows-1)))
    return;

  int   activeCol = TABLE_ATTRIBUTES::ActiveColumn();
  if(!COLUMN::CanEdit(activeCol))
    return;

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  if(!row->IsSelected(activeRow))
    return;

  int width = COLUMN::ColumnWidth(activeCol);
  if(width==0)
    return;

  int height = TABLE_ATTRIBUTES::CellHeight();
  if(TABLE_ATTRIBUTES::RowLines())
    height++;
  int cellY = tblRect.top + ((int)(activeRow - firstRow))*height;
  if(TABLE_ATTRIBUTES::ShowTitles())
    {
    cellY += (TABLE_ATTRIBUTES::TitleHeight()+1);
    if(TABLE_ATTRIBUTES::RowLines())
      cellY++;
    }

  if(TABLE_ATTRIBUTES::RowLines())
    height--;

  int cellX = tblRect.left;
  for(int i=0; i<activeCol; i++)
    {
    if((i>=TABLE_ATTRIBUTES::NumFixedColumns()) && (i<firstColumn))
      continue;

    if(COLUMN::ColumnWidth(i)==0)
      {
      if((i==(TABLE_ATTRIBUTES::NumFixedColumns()-1))
          &&
        (cellX!=tblRect.left))
        cellX++;
      continue;
      }

    cellX += COLUMN::ColumnWidth(i);
    if(TABLE_ATTRIBUTES::ColumnLines())
      cellX++;
    if(i==(TABLE_ATTRIBUTES::NumFixedColumns()-1))
      cellX++;
    }

  HDC       hDC = GetDC(hTable);
  HPEN      hPen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW));
  LOGBRUSH  lb = {BS_SOLID, GetSysColor(COLOR_WINDOW), 0};
  HPEN      oldPen = (HPEN)SelectObject(hDC, hPen);
  HBRUSH    hBrush = CreateBrushIndirect(&lb);
  HBRUSH    oldBrush = (HBRUSH)SelectObject(hDC, hBrush);
  int       right = cellX+width;
  if(right>tblRect.right)
    right = tblRect.right;

  Rectangle(hDC, cellX, cellY, right, cellY+height);
  SelectObject(hDC, oldPen);
  DeleteObject(hPen);
  SelectObject(hDC, oldBrush);
  DeleteObject(hBrush);
  ReleaseDC(hParentWnd, hDC);

  cellX += COLUMN::CellCharWidth(activeCol);
  width -= 2*COLUMN::CellCharWidth(activeCol);
  if((cellX+width)>tblRect.right)
    width = tblRect.right - cellX;

  int offset = 2*TABLE_ATTRIBUTES::CellLeading()/3 + TABLE_ATTRIBUTES::CellAscent() - COLUMN::CellAscent(activeCol);
  cellY = cellY + offset;
  height = TABLE_ATTRIBUTES::CellHeight() - offset;

  editRow = activeRow;
  editCol = activeCol;

  char *  cellPtr = row->GetCell(activeRow, activeCol, FALSE, NULL);
  UINT  length = (UINT)lstrlen(cellPtr) + 1;
  char* cellStr = new char[length];
  CopyMemory(cellStr, cellPtr, length);
  hEdit = CreateWindow("EDIT", cellStr,
            WS_CHILD | ES_AUTOHSCROLL,
            cellX, cellY,
            width, height,
            hTable, (HMENU)3, G.hInstance, NULL);
  delete cellStr;

  // subclass the edit control
  SetProp(hEdit, "loPtr", (HANDLE)LOWORD((DWORD)this));
  SetProp(hEdit, "hiPtr", (HANDLE)HIWORD((DWORD)this));
  SetProp(hEdit, "number", (HANDLE)AObjCheckType(COLUMN::GetObjectID(activeCol), OTYPE_NUMBER));
  oldEditControlProc = (WNDPROC)GetWindowLong(hEdit, GWL_WNDPROC);
  SetWindowLong(hEdit, GWL_WNDPROC, (long)EditControlProc);
  ShowWindow(hEdit, SW_SHOW);

  hEditFont =CreateFontIndirect(COLUMN::CellLogFont(activeCol));
  SendMessage(hEdit, WM_SETFONT, (WPARAM)hEditFont, MAKELPARAM(TRUE, 0));
  SetFocus(hEdit);
  SendMessage(hEdit, EM_SETSEL, (WPARAM)FALSE, MAKELPARAM(0, -1)); //!!PORT!!

  // wipPreventThieves(ParentID, TRUE); SKJ --> need replacement
  AEvtPostSignalAtMark(theObject, SIGNAL_STARTEDITING);
  }

// TABLE::DestroyEditControl ///////////////////////////////////////////////

void TABLE::DestroyEditControl()

  {
  if(hEdit)
    {
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

    // get the text and update the cell as necessary
    int  len = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0L)+1;
    char* buffer = new char[len];
    SendMessage(hEdit, WM_GETTEXT, (WPARAM)(len+1), (LPARAM)buffer);
    RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
    ROW*          row = rtime->GetRow();
    if(lstrcmp(row->GetCell(editRow, editCol, FALSE, NULL), buffer))
      {
      row->SetCell(editRow, editCol, buffer);
      TABLE_ATTRIBUTES::SetEditRow(editRow);
      TABLE_ATTRIBUTES::SetEditColumn(editCol);
      PostObjectChanged(CHANGED_FULLREDRAW);
      AEvtPostSignalAtMark(theObject, SIGNAL_TABLEEDITED);
      }
    delete buffer;

    // destroy the window
    DestroyWindow(hEdit);
    hEdit = NULL;
    DeleteObject(hEditFont);

    AEvtPostSignalAtMark(theObject, SIGNAL_STOPEDITING);

    // make sure the proper region is redrawn
    InvalidateRect(hTable, &cellRect, FALSE);
    UpdateWindow(hTable);

    // wipPreventThieves(ParentID, FALSE); SKJ need replacement
    }
  }

// TABLE::Cut //////////////////////////////////////////////////////////////

void TABLE::tCut()

  {
  if(TABLE_ATTRIBUTES::AutoFill())
    return;

  DestroyAllEditControls();
  Copy();

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  row->DeleteSelectedRows();
  PostObjectChanged(CHANGED_NUMROWS);
  }

// TABLE::Copy /////////////////////////////////////////////////////////////

void TABLE::tCopy()

  {
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  long          numRows = row->NumSelections();
  long          length = numRows;
  BOOL          dbAccessed = FALSE;

  if(numRows<1 || numRows>row->NumRows())
    return;

  for(long i=0; i<numRows; i++)
    length += row->RowLength(row->GetSelection(i), TRUE, &dbAccessed);

  HGLOBAL hClip = GlobalAlloc(GHND, length);
  if(!hClip)
    {
    if(dbAccessed)
      AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);

    return;
    }

  char *  clip = (char *)GlobalLock(hClip);
  long        sel;
  long        offset=0;
  for(i=0; i<numRows; i++)
    {
    sel = row->GetSelection(i);
    length = row->RowLength(sel, TRUE, &dbAccessed);
    row->GetRow(sel, clip+offset, length, TRUE, TRUE, &dbAccessed);
    if(i!=(numRows-1))
      {
      // give the cr-lf combination
      *(clip+(offset + length - 1)) = '\r';
      *(clip+(offset + length++)) = '\n';
      }
    else
      *(clip+(offset + length - 1)) = 0;

    offset+=length;
    }
  GlobalUnlock(hClip);

  OpenClipboard(hParentWnd);
  EmptyClipboard();
  SetClipboardData(CF_TEXT, hClip);
  CloseClipboard();

  if(dbAccessed)
    AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);
  }

// TABLE::Paste ////////////////////////////////////////////////////////////

void TABLE::tPaste()

  {
  if(TABLE_ATTRIBUTES::AutoFill())
    return;

  BOOL  available = IsClipboardFormatAvailable(CF_TEXT);
  if(!available)
    return;

  DestroyAllEditControls();

  OpenClipboard(hParentWnd);
  HGLOBAL hClipboard = GetClipboardData(CF_TEXT);
  if(!hClipboard)
    {
    CloseClipboard();
    return;
    }

  HGLOBAL hCopy = GlobalAlloc(GHND, GlobalSize(hClipboard));
  if(!hCopy)
    {
    CloseClipboard();
    return;
    }

  char* copy = (char*)GlobalLock(hCopy);
  char* clip = (char*)GlobalLock(hClipboard);
  CopyMemory(copy, clip, GlobalSize(hClipboard));
  GlobalUnlock(hClipboard);
  CloseClipboard();

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  long          first = row->FirstSelection();
  char*         token = (char*)myStrtok((char *)copy);
  while(token)
    {
    row->AddRow(first, token, TABLE_ATTRIBUTES::NumColumns(), FALSE);
    token = (char*)myStrtok(NULL);
    first++;
    }
  GlobalUnlock(hCopy);
  GlobalFree(hCopy);

  row->DeleteSelectedRows();
  PostObjectChanged(CHANGED_NUMROWS);
  }

// TABLE::Undo /////////////////////////////////////////////////////////////

void TABLE::tUndo()

  {
  }

////////////////////////////////////////////////////////////////////////////

// TABLE::SetColumnEditAttrib /////////////////////////////////////////////////////////////

BOOL TABLE::SetColumnEditAttrib(
  int columnIndex,
  BOOL bEditFlag)
  {

  if(TABLE_ATTRIBUTES::AutoFill() || !TABLE_ATTRIBUTES::CanActivate())
    return FALSE;

  COLUMN::SetEditAttrib(columnIndex, bEditFlag);
  return TRUE;
  }

