//////////////////////////////////////////////////////////////
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
//--------------------------------------------------------------
//
//  "tabl_api.cpp"
//
// Table object API entry points for other objects
//    1.  OTableAddRow
//    2.  OTableColumnTitle
//    3.  OTableGetCell
//    4.  OTableGetColumn
//    5.  OTableGetColumnCount
//    6.  OTableGetRow
//    7.  OTableGetRowCount
//    8.  OTableSetCell
//    9.  OTableSetColumn
//   10.  OTableSetColumnCount
//   11.  OTableSetRow
//   12.  OTableSetRowCount
//   13.  OTableFindCell
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////

/* GENERAL NOTE:
 *  BOOL  return values:
 *    TRUE means the function was successful.
 *    FALSE means an error occurred (e.g. the row or column index is out of
 *          range).
 *
 *  INDEX inputs:
 *    ALL INDICIES are zero based (i.e. the first row is row 0 and the first
 *          column is column 0). 
 */

#define   STRICT
#define   ASTRICT

#include  <windows.h>
#include  <a_alm.h>
#include  <o_table.h>

#include  "tabl_hdr.h"
#include  "tabl_row.h"
#include  "table.h"

// OTableAddRow ///////////////////////////////////////////////////////////////

BOOL ALMAPI OTableAddRow(
  OBJECTID  tableObj,
  char*     rowText,
  long      startRow)

  {
  if(!AObjCheckType(tableObj, OTYPE_TABLE))
    {
    ADBG_PRINT_E("Invalid Table Object in 'OTableAddRow.'");
    return FALSE;
    }

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(tableObj);
  ROW*          row = rtime->GetRow();

  if(rowText && startRow>=0 && startRow<=row->NumRows())
    {
    TABLE table(tableObj);
    table.DestroyAllEditControls();
    row->AddRow(startRow, rowText, table.TotalNumCols(), FALSE);
    table.PostObjectChanged(CHANGED_NUMROWS);
    return TRUE;
    }
  else
    return FALSE;
  }

// OTableColumnTitle //////////////////////////////////////////////////////////

BOOL ALMAPI OTableColumnTitle(
  OBJECTID  tableObj,
  char*     colName,
  int       colIndex)

  {
  if(!AObjCheckType(tableObj, OTYPE_TABLE))
    {
    ADBG_PRINT_E("Invalid Table Object in 'OTableColumnTitle.'");
    return FALSE;
    }

  COLUMN  col(tableObj);
  if(colIndex<0 || colIndex>col.NumColumns())
    return FALSE;

  TABLE   table(tableObj);
  if(colIndex==col.NumColumns())
    {
    if (table.AutoFill())
      return FALSE;
    table.DestroyAllEditControls();

    RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(tableObj);
    ROW*          row = rtime->GetRow();
    row->InsertColumn(colIndex, colIndex);
    col.AddColumn(NULL, colName, colIndex);
    table.PostObjectChanged(CHANGED_NUMCOLUMNS);
    }
  else
    {
    col.SetTitle(colIndex, colName);
    table.PostObjectChanged(CHANGED_COLUMNSELECTION);
    }

  return TRUE;
  }

/// OTableGetCell //////////////////////////////////////////////////////////////

BOOL ALMAPI OTableGetCell(
  OBJECTID    tableObj,
  MEMBLOCKID  cellBlock,
  int         colIndex,
  long        rowIndex)

  {
  if(!AObjCheckType(tableObj, OTYPE_TABLE))
    {
    ADBG_PRINT_E("Invalid Table Object in 'OTableGetCell.'");
    return FALSE;
    }

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(tableObj);
  ROW*          row = rtime->GetRow();

  if(colIndex<0 || colIndex>=row->GetNumCols())
    return FALSE;

  if(rowIndex<0 || rowIndex>=row->NumRows())
    return FALSE;

  BOOL  dbAccessed = FALSE;
  HPSTR text = row->GetCell(rowIndex, colIndex, FALSE, &dbAccessed);
  long  length = hstrlen(text) + 1;
  AMemSetSize(cellBlock, length);
  hmemcpy(AMemLock(cellBlock), text, length);
  AMemUnlock(cellBlock);
  return TRUE;
  }

// OTableGetColumn ////////////////////////////////////////////////////////////

BOOL ALMAPI OTableGetColumn(
  OBJECTID    tableObj,
  MEMBLOCKID  colBlock,
  int         colIndex)

  {
  if(!AObjCheckType(tableObj, OTYPE_TABLE))
    {
    ADBG_PRINT_E("Invalid Table Object in 'OTableGetColumn.'");
    return FALSE;
    }

  TABLE table(tableObj);
  
  if(colBlock && (table.TotalNumCols()>0))
    {
    // Put this check so as prevent invalid column index
    if (colIndex<0 || colIndex>=table.TotalNumCols())
      return FALSE;

    if(table.GetFullColumn(colIndex, colBlock))
      return TRUE;
    }

  return FALSE;
  }

// OTableGetColumnCount ///////////////////////////////////////////////////////

int ALMAPI OTableGetColumnCount(
  OBJECTID  tableObj)

  {
  if(!AObjCheckType(tableObj, OTYPE_TABLE))
    {
    ADBG_PRINT_E("Invalid Table Object in 'OTableGetColumnCount.'");
    return -1;
    }

  TABLE table(tableObj);

  return table.TotalNumCols();
  }

// OTableGetRow ///////////////////////////////////////////////////////////////

BOOL ALMAPI OTableGetRow(
  OBJECTID    tableObj,
  MEMBLOCKID  rowBlock,
  long        startRow,
  int         numRows)

  {
  if(!AObjCheckType(tableObj, OTYPE_TABLE))
    {
    ADBG_PRINT_E("Invalid Table Object in 'OTableGetRow.'");
    return FALSE;
    }

  TABLE table(tableObj);

  if((numRows>0) && rowBlock)
    {
    table.RowToText(startRow, startRow+numRows-1, rowBlock);
    return TRUE;
    }
  else
    return FALSE;
  }

// OTableGetRowCount //////////////////////////////////////////////////////////

long ALMAPI OTableGetRowCount(
  OBJECTID  tableObj)

  {
  if(!AObjCheckType(tableObj, OTYPE_TABLE))
    {
    ADBG_PRINT_E("Invalid Table Object in 'OTableGetRowCount.'");
    return -1;
    }

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(tableObj);
  ROW*          row = rtime->GetRow();

  return row->NumRows();
  }

// OTableSetCell //////////////////////////////////////////////////////////////

BOOL ALMAPI OTableSetCell(
  OBJECTID  tableObj,
  char*     cellText,
  int       colIndex,
  long      rowIndex)

  {
  if(!AObjCheckType(tableObj, OTYPE_TABLE))
    {
    ADBG_PRINT_E("Invalid Table Object in 'OTableSetCell.'");
    return FALSE;
    }

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(tableObj);
  ROW*          row = rtime->GetRow();

  if(colIndex<0 || colIndex>=row->GetNumCols())
    return FALSE;

  if(rowIndex<0 || rowIndex>=row->NumRows())
    return FALSE;

  TABLE table(tableObj);
  // do not allow Table API on AutoTable
  if (table.AutoFill())
    return FALSE;

  table.DestroyAllEditControls();
  row->SetCell(rowIndex, colIndex, cellText);
  table.PostObjectChanged(CHANGED_FULLREDRAW | CHANGED_KILLEDIT);
  
  return TRUE;
  }

// OTableSetColumn ////////////////////////////////////////////////////////////

BOOL ALMAPI OTableSetColumn(
  OBJECTID    tableObj,
  char huge*  colText,
  int         colIndex)

  {
  if(!AObjCheckType(tableObj, OTYPE_TABLE))
    {
    ADBG_PRINT_E("Invalid Table Object in 'OTableSetColumn.'");
    return FALSE;
    }
 
  TABLE         table(tableObj);
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(tableObj);
  ROW*          row = rtime->GetRow();

  if (colIndex<0 || colIndex>=table.TotalNumCols() || (hstrlen(colText)>0x0000FFFF))
    return FALSE;

  // do not allow Table API on AutoTable
  if (table.AutoFill() == FALSE)
    {
    table.DestroyAllEditControls();
    row->SetColumn(colText, colIndex, table.NumColumns(), TRUE);
    table.SetColumnWidth(colIndex, table.GetMaxWidth(colIndex));
    table.PostObjectChanged(CHANGED_NUMROWS);
    return TRUE;
    }
  else
    return FALSE;
  }

///////////////////////////////////////////////////////////////////

BOOL ALMAPI OTableSetColumnCount(
  OBJECTID  tableObj,
  int       colCount)

  {
  if(!AObjCheckType(tableObj, OTYPE_TABLE))
    {
    ADBG_PRINT_E("Invalid Table Object in 'OTableSetColumnCount.'");
    return FALSE;
    }

  COLUMN        col(tableObj);
  int           numCol = col.NumColumns();
  int           oldnCol = numCol;
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(tableObj);
  ROW*          row = rtime->GetRow();
  TABLE         table(tableObj);

  // Do not allow autofill to change its Column Count
  if (table.AutoFill())
    return FALSE;

  table.DestroyAllEditControls();

  if(colCount==numCol)
    return TRUE;
  else if(colCount<numCol)
    {
    while(numCol>colCount)
      {
      int zeroIndex = numCol -1;
      row->DeleteColumn(zeroIndex, numCol);
      col.DeleteColumn(--numCol);
      table.SetNumColumns(numCol);
      }
    }
  else
    {
    // update the columns
    while(numCol < colCount)
      {
      // call Row::InsertCol first before calling Col.AddCol
      col.AddColumn(NULL, "New Column", numCol);
      row->InsertColumn(numCol, colCount);
      table.SetNumColumns(++numCol);
      }
    }

  // if there were no columns before, set some font metrics for new cols.
  if ((oldnCol == 0) && (colCount > 0))
    {
    table.InitFontMetrics();
    }

  // update the visual instances
  table.PostObjectChanged(CHANGED_NUMCOLUMNS);

  return TRUE;
  }

// OTableSetColEditAttrib /////////////////////////////////////////////////////
//  Input 1:  Table
//  Input 2:  Column index
//  Input 3:  bCanEdit

BOOL ALMAPI OTableSetColEditAttrib(
  OBJECTID tableObj,
  int   colIndex,     // specific col
  BOOL  bCanEdit
  )
  {
  if(!AObjCheckType(tableObj, OTYPE_TABLE))
    {
    ADBG_PRINT_E("Invalid Table Object in 'OTableSetColEditAttrib.'");
    return FALSE;
    }

  TABLE   table(tableObj);
  if (colIndex<0 || colIndex>=table.TotalNumCols() || table.AutoFill())
    return FALSE;

  return table.SetColumnEditAttrib(colIndex, bCanEdit);
  }


// OTableSetColumnWidth /////////////////////////////////////////////////////
//  Input 1:  Table
//  Input 2:  Column Object
//  Input 3:  Width (pixels)

BOOL ALMAPI OTableSetColumnWidth(
  OBJECTID tableObj,
  BOOL  doAll,        // All columns
  int   colIndex,     // specific col
  int   width
  )
  {
  if(!AObjCheckType(tableObj, OTYPE_TABLE))
    {
    ADBG_PRINT_E("Invalid Table Object in 'OTableSetColumnWidth.'");
    return FALSE;
    }

  TABLE   table(tableObj);

  // if we have a specific column we want to set
  if ((doAll == FALSE) && (colIndex<0 || colIndex>=table.TotalNumCols()))
    {
    return FALSE;
    }
  if (table.AutoFill())
    return FALSE;

  if(doAll)
    {
    for(int i=0; i<table.TotalNumCols(); i++)
      {
      if(width == -1)
        width = table.GetMaxWidth(i);

      table.SetColumnWidth(i, width);
      }
    }
  else
    table.SetColumnWidth(colIndex, width);
  return TRUE;
  }


// OTableSetRow ///////////////////////////////////////////////////////////////

BOOL ALMAPI OTableSetRow(
  OBJECTID  tableObj,
  char*     rowText,
  long      startRow)

  {
  if(!AObjCheckType(tableObj, OTYPE_TABLE))
    {
    ADBG_PRINT_E("Invalid Table Object in 'OTableSetRow.'");
    return FALSE;
    }

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(tableObj);
  ROW*          row = rtime->GetRow();

  if(rowText && startRow>=0 && startRow<=row->NumRows())
    {
    TABLE table(tableObj);
    if (table.AutoFill())
      return FALSE;
    table.DestroyAllEditControls();
    row->AddRow(startRow, rowText, table.TotalNumCols(), TRUE);
    table.PostObjectChanged(CHANGED_NUMROWS);
    return TRUE;
    }
  else
    return FALSE;
  }

// OTableSetRowCount //////////////////////////////////////////////////////////

BOOL ALMAPI OTableSetRowCount(
  OBJECTID  tableObj,
  long      rowCount)

  {
  if(!AObjCheckType(tableObj, OTYPE_TABLE))
    {
    ADBG_PRINT_E("Invalid Table Object in 'OTableSetRowCount.'");
    return FALSE;
    }

  TABLE   table(tableObj);
  if (table.AutoFill())
    return FALSE;

  table.DestroyAllEditControls();

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(tableObj);
  ROW*          row = rtime->GetRow();
  if(rowCount==row->NumRows())
    return TRUE;
  else
    row->SetRowCount(rowCount);

  table.PostObjectChanged(CHANGED_NUMROWS);
  return TRUE;
  }

////////////////////////////////////////////////////////////////////////////

struct FindCellStruct
  {
  TABLE*  pTable;
  BOOL    fSuccess;
  POINT   pt;
  long    lRow;
  int     nCol;
  };

BOOL ALMCALLBACK TableFindCellProc
  (
  OpWndItemD* wi,
  long        lUser1,
  long        lUser2
  )

  {
  I_UNUSED(lUser2);

  FindCellStruct* fcs = (FindCellStruct*)lUser1;

  fcs->pTable = (TABLE*)wi;
  fcs->fSuccess = fcs->pTable->FindCell(&fcs->pt, &fcs->lRow, &fcs->nCol);

  return !fcs->fSuccess;
  }

////////////////////////////////////////////////////////////////////////////

BOOL ALMAPI OTableFindCell
  (
  OBJECTID  oiTable,
  LPPOINT   lppt,
  LPLONG    plRow,
  LPINT     pnCol
  )

  {
  FindCellStruct  fcs;
  fcs.pTable    = NULL;
  fcs.fSuccess  = FALSE;
  fcs.pt        = *lppt;
  fcs.lRow      = -1L;
  fcs.nCol      = -1;

  if(oiTable)
    {
    if(!lppt || !AObjCheckType(oiTable, OTYPE_TABLE))
      {
      ADBG_PRINT_E("Invalid Table Object in 'OTableSetRowCount.'");
      return FALSE;
      }

    OWndCallEachVisualInstance(oiTable, TableFindCellProc, (long)&fcs, 0L);

    // cache the last visual instance used
    ATypeSetRuntimeInfo(OTYPE_TABLE, (long)fcs.pTable);
    }
  else
    {
    // use the cached visual instance
    fcs.pTable = (TABLE*)ATypeGetRuntimeInfo(OTYPE_TABLE);

    if(fcs.pTable)
      fcs.fSuccess = fcs.pTable->FindCell(&fcs.pt, &fcs.lRow, &fcs.nCol);
    }

  if(fcs.fSuccess)
    {
    if(plRow)
      *plRow = fcs.lRow;

    if(pnCol)
      *pnCol = fcs.nCol;
    }

  return fcs.fSuccess;
  }

////////////////////////////////////////////////////////////////////////////
