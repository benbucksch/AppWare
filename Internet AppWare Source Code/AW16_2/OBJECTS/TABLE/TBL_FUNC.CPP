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
// FILE:  "tbl_func.cpp"
//
// AUTHOR:
//
// DESCRIPTION:
//     1. Tabl_ActivateColumn
//     2. Tabl_ActiveCell
//     3. Tabl_ActiveColumn
//     4. Tabl_AddRow -- manual table only
//     5. Tabl_CellEdited -- manual table only
//     6. Tabl_ClearSelections
//     7. Tabl_ClearTable -- manual table only
//     8. Tabl_ColumnCalculation
//     9. Tabl_CountRows
//    10. Tabl_DeleteColumn -- manual table only
//    11. Tabl_DeleteRow -- manual table only
//    12. Tabl_Find -- manual table only
//    13. Tabl_FindExact -- manual table only
//    14. Tabl_GetCell
//    15. Tabl_GetColumn
//    16. Tabl_GetColumnTitle
//    17. Tabl_GetColumnWidth
//    18. Tabl_GetDataSource -- auto table only
//    19. Tabl_GetIndexedField -- auto table only
//    20. Tabl_GetRecordID -- auto table only
//    21. Tabl_GetSelection
//    22. Tabl_InsertColumn -- manual table only
//    23. Tabl_RefreshAutoTable -- auto table only
//    24. Tabl_RowCalculation
//    25. Tabl_RowToObjects
//    26. Tabl_RowToText
//    27. Tabl_ScrollToSelection
//    28. Tabl_SetCell -- manual table only
//    29. Tabl_SetColumn -- manual table only
//    30. Tabl_SetColumnTitle
//    31. Tabl_SetColumnWidth
//    32. Tabl_SetDataSource -- auto table only
//    33. Tabl_SetIndexedField -- auto table only
//    34. Tabl_SetRow -- manual table only
//    35. Tabl_SetSelection
//    36. Tabl_SortAscending -- manual table only
//    37. Tabl_SortDescending -- manual table only
//    38. Tabl_SortRowAscending -- manual table only
//    39. Tabl_SortRowDescending -- manual table only
//    40. Tabl_UpdateAutoTable -- auto table only
//
// CHANGES:
//    41. Tabl_ActivateCell
//
//////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

//////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdlib.h>
#include <string.h>

#include <a_alm.h>
#include <intdebug.h>
#include <op_wnd.h>
#include <o_wnd.h>
#include <o_text.h>
#include <o_number.h>
#include <o_table.h>

#include "tabl_hdr.h"  // for hInstance
#include "table.h"
#include "tabl_row.h"

// Externals ///////////////////////////////////////////////////////////////
extern BOOL Tbl_SetNmbrObj(OBJECTID n_oid, long double ldVal);
extern long double  Tbl_GetNmbrObj(OBJECTID n_oid);

extern GLOBAL G;

// Tabl_ActivateColumn /////////////////////////////////////////////////////
//  Input 1: Table -- table object
//  Input 2: Column Object -- set active column to this column (if it is a column)
//  Flow  1: After activating ....
 
void far pascal _export Tabl_ActivateColumn(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;

  OBJECTID  columnObject = AFuncGetParameter(2L);

  TABLE     table(tableObject);
  if (table.ActivateColumn(columnObject))
    AFuncReportResult(theSystem, 1L, NULL);
  else
    AFuncReportResult(theSystem, 2L, NULL);
  }

// Tabl_ActiveCell /////////////////////////////////////////////////////////
//  Input   1:  Table
//  Output  1:  Column Object
//  Output  2:  Row Index
//  Flow    1:  After getting
//  Flow    2:  If none

void far pascal _export Tabl_ActiveCell(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;

  TABLE     table(tableObject);
  OBJECTID  columnObj;
  long      row;

  if(table.ActiveCell(&row, &columnObj))
    {
    OBJECTID  indexObj = AObjCreate(OTYPE_NUMBER);
    if(indexObj)
      {
      Tbl_SetNmbrObj(indexObj, (long)row); // table.lib util func. SKJ
      }
    AFuncSetParameter(1L, columnObj);
    AFuncSetParameter(2L, indexObj);
    AFuncReportResult(theSystem, 1L, NULL);
    }
  else
    AFuncReportResult(theSystem, 2L, NULL);
  }

// Tabl_ActiveColumn ///////////////////////////////////////////////////////
//  Input 1: Table -- table to get the active column from
//  Ouput 1: Column Object -- the object that is the current column
//  Flow  1: After getting ...

void far pascal _export Tabl_ActiveColumn(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;

  TABLE     table(tableObject);
  OBJECTID  columnObject = table.ActiveColumn();

  AFuncSetParameter(1L, columnObject);
  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_AddRow /////////////////////////////////////////////////////////////
//  Input 1:  Table Object -- the table where the row will be added
//  Input 2:  Text -- The text for the row (optional)
//  Input 3:  Row Index -- The index for the first row to add
//  Flow  1:  After adding

void far pascal _export Tabl_AddRow(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE       table(tableObject);
  if(table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Add Row\" function is valid for manual-fill tables only.");
    return;
    }

  OBJECTID    textObject = AFuncGetTypedParameter(2L, OTYPE_TEXT);
  OBJECTID    indexObject = AFuncGetTypedParameter(3L, OTYPE_NUMBER);

  MEMBLOCKID  theText = NULL;
  char*       text = NULL;
  long        index = -1L;

  if(textObject)
    {
    theText = AMemAllocate(0L);
    AObjConvertToText(textObject, theText);
    text = (char*)AMemLock(theText);
    }
  if(indexObject)
    {
    index = ONmbrGetInteger(indexObject);
    }

  table.AddRow(textObject, text, indexObject, index);

  if(textObject) {
    AMemUnlock(theText);
    AMemFree(theText);
    }

  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_CellEdited /////////////////////////////////////////////////////////
//  Input   1:  Table
//  Output  1:  Column Object
//  Output  2:  Row Index
//  Flow    1:  After getting
//  Flow    2:  If none

void far pascal _export Tabl_CellEdited(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE     table(tableObject);
  if(table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Cell Edited\" function is valid for manual-fill tables only.");
    return;
    }

  OBJECTID  columnObj;
  long      row;

  if(table.CellEdited(&row, &columnObj))
    {
    OBJECTID  indexObj = AObjCreate(OTYPE_NUMBER);
    if(indexObj)
      Tbl_SetNmbrObj(indexObj, (long double)row);
    AFuncSetParameter(1L, columnObj);
    AFuncSetParameter(2L, indexObj);
    AFuncReportResult(theSystem, 1L, NULL);
    }
  else
    AFuncReportResult(theSystem, 2L, NULL);
  }


// Tabl_ClearSelections ////////////////////////////////////////////////////
//  Input 1:  Table for which the selections should be cleared
//  Flow  1:  After clearing ...

void far pascal _export Tabl_ClearSelections(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;

  TABLE     table(tableObject);

  table.ClearSelections();
  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_ClearTable /////////////////////////////////////////////////////////
//  Input 1:  Table
//  Flow  1:  After clearing

void far pascal _export Tabl_ClearTable(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE       table(tableObject);
  if(table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Clear Table\" function is valid for manual-fill tables only.");
    return;
    }

  table.DestroyAllEditControls();

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(tableObject);
  rtime->GetRow()->Clear();

  table.PostObjectChanged(CHANGED_FULLREDRAW);
  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_ColumnCalculation //////////////////////////////////////////////////
//  Input   1:  Table -- table object
//  Input   2:  Operation -- text string: 'Sum', 'Average', 'Min', 'Max'
//  Input   3:  Column Object -- column for the calculation
//  Input   4:  Start Row -- row to start with
//  Input   5:  End Row -- row to finish with
//  Output  1:  Result -- the result of the calculation

void far pascal _export Tabl_ColumnCalculation(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE       table(tableObject);

  OBJECTID    opObject = AFuncGetTypedParameter(2L, OTYPE_TEXT);
  MEMBLOCKID  hText = AMemAllocate(0L);
  AObjConvertToText(opObject, hText);
  char*       text = (char*)AMemLock(hText);
  if(!text)
    {
    AMemFree(hText);
    return;
    }

  int         operation = OP_INVALID;
  if(!memicmp(text, "sum", 3))
    operation = OP_SUM;
  else if(!memicmp(text, "ave", 3))
    operation = OP_AVERAGE;
  else if(!memicmp(text, "min", 3))
    operation = OP_MIN;
  else if(!memicmp(text, "max", 3))
    operation = OP_MAX;

  AMemUnlock(hText);
  AMemFree(hText);
  if(operation==OP_INVALID)
    {
    AFuncReportResult(theSystem, 1L, "Invalid Operation for the Column Calcualtion Function.");
    return;
    }

  OBJECTID    colObject = AFuncGetParameter(3L);
  int         colIndex = table.ColumnIndex(colObject);

  OBJECTID    startObj = AFuncGetTypedParameter(4L, OTYPE_NUMBER);
  long        start = -1L;
  if(startObj)
    start = (long)Tbl_GetNmbrObj(startObj) -1;

  OBJECTID    stopObj = AFuncGetTypedParameter(5L, OTYPE_NUMBER);
  long        stop = 0x7FFFFFFFL;
  if(stopObj)
    {
    stop = Tbl_GetNmbrObj(stopObj) - 1;
    }

  OBJECTID    resultObject = AObjCreate(OTYPE_NUMBER);
  if(resultObject)
    table.ColumnCalculation(operation, colIndex, start, stop, resultObject);

  AFuncSetParameter(1L, resultObject);
  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_CountRows //////////////////////////////////////////////////////////
//  Input   1:  Table -- count the rows and selections for this table
//  Output  1:  Rows -- the number of rows in the table
//  Output  2:  Selections -- the number of selected rows in the table
//  Flow    1:  After counting...

void far pascal _export Tabl_CountRows(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(tableObject);
  ROW*          row = rtime->GetRow();

  OBJECTID    rowObject = AObjCreate(OTYPE_NUMBER);
  OBJECTID    selObject = AObjCreate(OTYPE_NUMBER);
  if(rowObject)
    Tbl_SetNmbrObj(rowObject, (long double)row->NumRows());
  if(selObject)
    Tbl_SetNmbrObj(selObject, (long double)row->NumSelections());

  AFuncSetParameter(1L, rowObject);
  AFuncSetParameter(2L, selObject);
  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_DeleteColumn ///////////////////////////////////////////////////////
//  Input 1:  Table -- table to remove a column from
//  Input 2:  Column Object -- column (or index) to delete
//  Flow  1:  After deleting
//  Flow  2:  Invalid column

void far pascal _export Tabl_DeleteColumn(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE     table(tableObject);
  if(table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Delete Column\" function is valid for manual-fill tables only.");
    return;
    }

  OBJECTID  colObject = AFuncGetParameter(2L);
  int       colIndex = table.ColumnIndex(colObject);
  if(colIndex<0 || colIndex>=table.TotalNumCols())
    {
    // invalid column flow.....
    AFuncReportResult(theSystem, 2L, NULL);
    return;
    }

  table.DeleteColumn(colIndex);
  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_DeleteRow //////////////////////////////////////////////////////////
//  Input 1:  Table -- table to delete row(s)
//  Input 2:  Row Index -- row to delete (delete selected rows if no input)
//  Flow  1:  After deleting

void far pascal _export Tabl_DeleteRow(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE       table(tableObject);
  if(table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Delete Row\" function is valid for manual-fill tables only.");
    return;
    }

  OBJECTID    indexObject = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
  long        index = -1L;

  if(indexObject)
    index = Tbl_GetNmbrObj(indexObject);

  table.DeleteRow(indexObject, index-1);
  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_Find ///////////////////////////////////////////////////////////////
//  Input 1:  Table
//  Input 2:  Column Object
//  Input 3:  Value -- search value
//  Input 4:  Start Row -- row to start search from
//  Flow  1:  Found
//  Flow  2:  Not found
//  Flow  3:  Invalid column
//  Flow  4:  Invalid row

void far pascal _export Tabl_Find(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE       table(tableObject);
  if(table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Find\" function is valid for manual-fill tables only.");
    return;
    }

  OBJECTID    columnObject = AFuncGetParameter(2L);
  int         index = table.ColumnIndex(columnObject);
  if(index<0 || index>=table.TotalNumCols())
    {
    AFuncReportResult(theSystem, 3L, NULL);
    return;
    }

  TYPEID      colType = AObjGetType(table.ColObjectID(index));
  OBJECTID    valueObject;
  if(colType == OTYPE_NUMBER)
    {
    MEMBLOCKID  mi = AMemAllocate(0L);
    char*       pNum;
    long double ld;
    long        lNumType;

    valueObject = AFuncGetTypedParameter(3L, OTYPE_TEXT);
    OTextGetToMemblock(valueObject, mi);
    pNum = (char*)AMemLock(mi);
    lNumType = ONmbrParseFormatedNumber(table.ColObjectID(index), &ld, pNum);
    AMemUnlock(mi);

    valueObject = AObjCreate(OTYPE_NUMBER);
    if(lNumType == ONMBR_REAL)
      ONmbrSetReal(valueObject, ld, TRUE);
    else
      ONmbrSetInteger(valueObject, (long)ld, TRUE);
    }
  else if(colType)
    valueObject = AFuncGetTypedParameter(3L, colType);
  else
    valueObject = AFuncGetTypedParameter(3L, OTYPE_TEXT);
  if(!valueObject)
    valueObject = table.ColObjectID(index);

  OBJECTID    startRowObject = AFuncGetTypedParameter(4L, OTYPE_NUMBER);
  long        startRow;
  if (startRowObject)
    startRow = Tbl_GetNmbrObj(startRowObject) - 1;
  else
    startRow = 0L;

  OBJECTID  resultObject = AObjCreate(OTYPE_NUMBER);

  int       res = table.Find(index, colType, startRow, valueObject, resultObject, FALSE);
  switch(res)
    {
    case FOUND_FOUND:
      AFuncReportResult(theSystem, 1L, NULL);
      break;

    case FOUND_NOTFOUND:
      AFuncReportResult(theSystem, 2L, NULL);
      break;

    default:
      if(!startRowObject)
        AFuncReportResult(theSystem, 2L, NULL);
      else
        AFuncReportResult(theSystem, 4L, NULL);
      break;
    }

  if((colType == OTYPE_NUMBER) && valueObject)
    AObjDestroy(valueObject);

  AFuncSetParameter(1L, resultObject);
  }

// Tabl_FindExact //////////////////////////////////////////////////////////
//  Input 1:  Table
//  Input 2:  Column Object
//  Input 3:  Value -- search value
//  Input 4:  Start Row -- row to start search from
//  Flow  1:  Found
//  Flow  2:  Not found
//  Flow  3:  Invalid column
//  Flow  4:  Invalid row

void far pascal _export Tabl_FindExact(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return; 
  TABLE       table(tableObject);
  if(table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Find Exact\" function is valid for manual-fill tables only.");
    return;
    }

  OBJECTID    columnObject = AFuncGetParameter(2L);
  int         index = table.ColumnIndex(columnObject);
  if(index<0 || index>=table.TotalNumCols())
    {
    AFuncReportResult(theSystem, 3L, NULL);
    return;
    }

  TYPEID      colType = AObjGetType(table.ColObjectID(index));
  OBJECTID    valueObject;
  if(colType)
    valueObject = AFuncGetTypedParameter(3L, colType);
  else
    valueObject = AFuncGetTypedParameter(3L, OTYPE_TEXT);
  if(!valueObject)
    valueObject = table.ColObjectID(index);

  OBJECTID    startRowObject = AFuncGetTypedParameter(4L, OTYPE_NUMBER);
  long        startRow;
  if (startRowObject)
    startRow = (long)Tbl_GetNmbrObj(startRowObject) - 1;
  else
    startRow = 0L;
            
  OBJECTID  resultObject = AObjCreate(OTYPE_NUMBER);

  int       res = table.Find(index, colType, startRow, valueObject, resultObject, TRUE);
  switch(res)
    {
    case FOUND_FOUND:
      AFuncReportResult(theSystem, 1L, NULL);
      break;

    case FOUND_NOTFOUND:
      AFuncReportResult(theSystem, 2L, NULL);
      break;

    default:
      if(!startRowObject)
        AFuncReportResult(theSystem, 2L, NULL);
      else
        AFuncReportResult(theSystem, 4L, NULL);
      break;
    }

  AFuncSetParameter(1L, resultObject);
  }

// Tabl_GetCell ////////////////////////////////////////////////////////////
//  Input   1:  Table -- table to get the cell from
//  Input   2:  Row Index -- index to get (or first selected one)
//  Input   3:  Column Object -- column for the desired cell (or active one)
//  Output  1:  Cell Data -- the text from the cell
//  Flow    1:  After getting

void far pascal _export Tabl_GetCell(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;

  OBJECTID    indexObject = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
  OBJECTID    columnObject = AFuncGetParameter(3L);
  TABLE       table(tableObject);

  MEMBLOCKID  theText = AMemAllocate(0L);
  long        index = -1L;

  if(!columnObject)
    columnObject = table.ActiveColumn();

  if(indexObject)
    index = (long)Tbl_GetNmbrObj(indexObject);

  table.GetCell(columnObject, indexObject, index, theText);

  OBJECTID  outputObject = AObjCreate(OTYPE_TEXT);
  if(AMemGetSize(theText)>0L)
    AObjConvertFromText(outputObject, theText);
  AMemFree(theText);

  AFuncSetParameter(1L, outputObject);
  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_GetColumn //////////////////////////////////////////////////////////
//  Input   1:  Table
//  Input   2:  Column Object
//  Output  1:  Column Text
//  Flow    1:  After getting
//  Flow    2:  Invalid column

void far pascal _export Tabl_GetColumn(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE     table(tableObject);

  OBJECTID  columnObject = AFuncGetParameter(2L);
  int       colIndex = table.ColumnIndex(columnObject);

  if(colIndex<0 || colIndex>=table.TotalNumCols())
    AFuncReportResult(theSystem, 2L, NULL);
  else
    {
    MEMBLOCKID  theText = AMemAllocate(32000L);
    OBJECTID    textObject = AObjCreate(OTYPE_TEXT);

    table.GetColumn(columnObject, theText);
    AObjConvertFromText(textObject, theText);
    AMemFree(theText);

    AFuncSetParameter(1L, textObject);
    AFuncReportResult(theSystem, 1L, NULL);
    }
  }


// Tabl_GetColumnTitle /////////////////////////////////////////////////////
//  Input   1:  Table
//  Input   2:  Column Object
//  Output  1:  Title
//  Flow    1:  After getting
//  Flow    2:  Invalid column

void far pascal _export Tabl_GetColumnTitle(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE     table(tableObject);

  OBJECTID  columnObject = AFuncGetParameter(2L);
  int       index = table.ColumnIndex(columnObject);
  if(index<0 || index>=table.TotalNumCols())
    AFuncReportResult(theSystem, 2L, NULL);
  else
    {
    OBJECTID  resultObject = AObjCreate(OTYPE_TEXT);
    table.GetColumnTitle(index, resultObject);
    AFuncReportResult(theSystem, 1L, NULL);
    AFuncSetParameter(1L, resultObject);
    }
  }

// Tabl_GetColumnWidth /////////////////////////////////////////////////////
//  Input   1:  Table
//  Input   2:  Column Object
//  Output  1:  Width (pixels)
//  Flow    1:  After getting
//  Flow    2:  Invalid column

void far pascal _export Tabl_GetColumnWidth(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE     table(tableObject);

  OBJECTID  columnObject = AFuncGetParameter(2L);
  int       colIndex = table.ColumnIndex(columnObject);

  if(colIndex<0 || colIndex>=table.TotalNumCols())
    AFuncReportResult(theSystem, 2L, NULL);
  else
    {
    int         width = table.GetColumnWidth(columnObject);
    OBJECTID    widthObj = AObjCreate(OTYPE_NUMBER);
    Tbl_SetNmbrObj(widthObj, (long double)width);
    AFuncSetParameter(1L, widthObj);
    AFuncReportResult(theSystem, 1L, NULL);
    }
  }

// Tabl_GetDataSource //////////////////////////////////////////////////////
//  Input   1:  Table
//  Output  1:  Data Source
//  Flow    1:  After getting
//  Flow    2:  If none

void far pascal _export Tabl_GetDataSource(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID          tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE_ATTRIBUTES  table(tableObject);
  if(!table.AutoFill())
    AFuncReportResult(theSystem, 1L, "The \"Get Data Source\" function is valid for auto-fill tables only.");
  else if(table.DataSource())
    {
    AFuncSetParameter(1L, table.DataSource());
    AFuncReportResult(theSystem, 1L, NULL);
    }
  else
    AFuncReportResult(theSystem, 2L, NULL);
  }

// Tabl_GetIndexedField ////////////////////////////////////////////////////
//  Input   1:  Table
//  Output  1:  Indexed Field
//  Flow    1:  After getting
//  Flow    2:  If none

void far pascal _export Tabl_GetIndexedField(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID          tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE_ATTRIBUTES  table(tableObject);
  if(!table.AutoFill())
    AFuncReportResult(theSystem, 1L, "The \"Get Indexed Field\" function is valid for auto-fill tables only.");
  else if(table.IndexedField())
    {
    AFuncSetParameter(1L, table.IndexedField());
    AFuncReportResult(theSystem, 1L, NULL);
    }
  else
    AFuncReportResult(theSystem, 2L, NULL);
  }

// Tabl_GetRecordID ////////////////////////////////////////////////////////
//  Input   1:  Table
//  Input   2:  Row Index
//  Output  1:  Record ID
//  Flow    1:  After getting
//  Flow    2:  Invalid row

void far pascal _export Tabl_GetRecordID(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID          tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE_ATTRIBUTES  table(tableObject);
  if(!table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Get Record ID\" function is valid for auto-fill tables only.");
    return;
    }

  OBJECTID          indexObject = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
  if(!indexObject)
    { // grab the first selected row
    RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(tableObject);
    ROW*          row = rtime->GetRow();
    if(row->NumSelections()==0)
      AFuncReportResult(theSystem, 2L, NULL);
    else
      {
      OBJECTID  resultObject = AObjCreate(OTYPE_NUMBER);
      long      index = row->FirstSelection();
      ONmbrSetInteger(resultObject, table.GetRecordID(index), TRUE);
      AFuncReportResult(theSystem, 1L, NULL);
      AFuncSetParameter(1L, resultObject);
      }
    }
  else
    {
    long index = ONmbrGetInteger(indexObject) - 1;
    if(index<0L || index>=table.TotalNumRows())
      AFuncReportResult(theSystem, 2L, NULL);
    else
      {
      OBJECTID  resultObject = AObjCreate(OTYPE_NUMBER);
      ONmbrSetInteger(resultObject, table.GetRecordID(index), TRUE);
      AFuncReportResult(theSystem, 1L, NULL);
      AFuncSetParameter(1L, resultObject);
      }
    }
  }

// Tabl_GetSelection ///////////////////////////////////////////////////////
//  Input   1:  Table -- table from which a selection is desired
//  Input   2:  Selection Index -- get this selection (or the first)
//  Output  1:  Row Index -- row index of the selection

void far pascal _export Tabl_GetSelection(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;

  OBJECTID  indexObject = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
  OBJECTID  outputObject = AObjCreate(OTYPE_NUMBER);
  long      index = -1L;
  TABLE     table(tableObject);

  if(indexObject)
    index = (long) Tbl_GetNmbrObj(indexObject);

  if(table.tGetSelection(indexObject, index, outputObject))
    {
    AFuncSetParameter(1L, outputObject);
    AFuncReportResult(theSystem, 1L, NULL);
    }
  else
    {
    AObjDestroy(outputObject);
    AFuncReportResult(theSystem, 2L, NULL);
    }
  }

// Tabl_InsertColumn ///////////////////////////////////////////////////////
//  Input 1:  Table
//  Input 2:  Index
//  Input 3:  Title
//  Input 4:  Column Object
//  Flow  1:  After inserting
//  Flow  2:  Invalid index

void far pascal _export Tabl_InsertColumn(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE     table(tableObject);
  if(table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Insert Column\" function is valid for manual-fill tables only.");
    return;
    }

  OBJECTID  indexObject = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
  OBJECTID  titleObject = AFuncGetTypedParameter(3L, OTYPE_TEXT);
  OBJECTID  columnObject = AFuncGetParameter(4L);

  int     index;
  if(indexObject)
    index = Tbl_GetNmbrObj(indexObject) - 1;
  else
    index = table.TotalNumCols();

  if(index<0 || index>table.TotalNumCols())
    AFuncReportResult(theSystem, 2L, NULL);
  else
    {
    table.InsertColumn(index, titleObject, columnObject);
    AFuncReportResult(theSystem, 1L, NULL);
    }
  }

// Tabl_RefreshAutoTable ///////////////////////////////////////////////////
//  Input 1:  Table
//  Flow  1:  After refreshing

void far pascal _export Tabl_RefreshAutoTable(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE     table(tableObject);
  if(!table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Refresh Auto Table\" function is valid for auto-fill tables only.");
    return;
    }

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(tableObject);
  ROW*          row = rtime->GetRow();
  row->ClearLoadedRows();
  row->ClearSelections();

  table.PostObjectChanged(CHANGED_FULLREDRAW);
  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_RowCalculation /////////////////////////////////////////////////////
//  Input   1:  Table
//  Input   2:  Operation -- "MIN", "MAX", "AVE", "SUM"
//  Input   3:  Start Column
//  Input   4:  End Column
//  Input   5:  Row Index
//  Output  1:  Result
//  Flow    1:  After calculating

void far pascal _export Tabl_RowCalculation(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE       table(tableObject);

  OBJECTID    opObject = AFuncGetTypedParameter(2L, OTYPE_TEXT);
  MEMBLOCKID  hText = AMemAllocate(0L);
  AObjConvertToText(opObject, hText);
  char*       text = (char*)AMemLock(hText);
  if(!text)
    {
    AMemFree(hText);
    return;
    }

  int         operation = OP_INVALID;
  if(!memicmp(text, "sum", 3))
    operation = OP_SUM;
  else if(!memicmp(text, "ave", 3))
    operation = OP_AVERAGE;
  else if(!memicmp(text, "min", 3))
    operation = OP_MIN;
  else if(!memicmp(text, "max", 3))
    operation = OP_MAX;

  AMemUnlock(hText);
  AMemFree(hText);
  if(operation==OP_INVALID)
    {
    AFuncReportResult(theSystem, 1L, "Invalid Operation for the Column Calcualtion Function.");
    return;
    }

  OBJECTID    startColObject = AFuncGetParameter(3L);
  int         startCol = table.ColumnIndex(startColObject);
  if(!startColObject)
    startCol = 0;

  OBJECTID    endColObject = AFuncGetParameter(4L);
  int         endCol = table.ColumnIndex(endColObject);
  if(!endColObject)
    endCol = 0x7FFF;

  OBJECTID    rowObj = AFuncGetTypedParameter(5L, OTYPE_NUMBER);
  long        index = -1L;
  if(rowObj)
    index = (long)Tbl_GetNmbrObj(rowObj) - 1;

  OBJECTID    resultObject = AObjCreate(OTYPE_NUMBER);
  if (resultObject)
   table.RowCalculation(operation, startCol, endCol, index, resultObject);

  AFuncSetParameter(1L, resultObject);
  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_RowToObjects ///////////////////////////////////////////////////////
//  Input 1:  Table -- table from which a row is to be retrieved
//  Input 2:  Row Index -- get this row (or the first selected one)
//  Flow  1:  After getting

void far pascal _export Tabl_RowToObjects(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;

  OBJECTID    indexObject = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
  long        index = -1L;
  TABLE       table(tableObject);

  if(indexObject)
    index = (long)Tbl_GetNmbrObj(indexObject);

  table.GetRow(indexObject, index);
  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_RowToText //////////////////////////////////////////////////////////
//  Input   1:  Table -- table from which a row is to be retrieved
//  Input   2:  Start Row Index
//  Input   3:  End Row Index
//  Output  1:  Row Text
//  Flow    1:  After getting

void far pascal _export Tabl_RowToText(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE       table(tableObject);

  OBJECTID    startObject = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
  long        startIndex = -1L;

  OBJECTID    stopObject = AFuncGetTypedParameter(3L, OTYPE_NUMBER);
  long        stopIndex = -1L;

  if(startObject)
    startIndex = (long)Tbl_GetNmbrObj(startObject) - 1;

  if(stopObject)
    stopIndex = (long)Tbl_GetNmbrObj(stopObject) - 1;

  MEMBLOCKID  hText = AMemAllocate(0L);
  table.RowToText(startIndex, stopIndex, hText);
  OBJECTID    textObject = AObjCreate(OTYPE_TEXT);
  if(textObject)
    {
    AObjConvertFromText(textObject, hText);
    AFuncSetParameter(1L, textObject);
    }
  AMemFree(hText);
  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_ScrollToSelection //////////////////////////////////////////////////
//  Input 1:  Table

void far pascal _export Tabl_ScrollToSelection(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;

  TABLE     table(tableObject);
  table.PostObjectChanged(CHANGED_SCROLLTOSELECTION);

  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_SetCell ////////////////////////////////////////////////////////////
//  Input 1:  Table
//  Input 2:  Row Index
//  Input 3:  Column Object
//  Input 4:  Cell Data
//  Flow  1:  After setting

void far pascal _export Tabl_SetCell(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE       table(tableObject);
  if(table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Set Cell\" function is valid for manual-fill tables only.");
    return;
    }

  OBJECTID    indexObject = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
  OBJECTID    columnObject = AFuncGetParameter(3L);
  OBJECTID    textObject = AFuncGetTypedParameter(4L, OTYPE_TEXT);
  long        index = -1L;
  MEMBLOCKID  theText = AMemAllocate(0L);

  if(indexObject)
    index = (long)Tbl_GetNmbrObj(indexObject);

  if(!columnObject)
    columnObject = table.ActiveColumn();

  table.SetCell(indexObject, index, columnObject, textObject, theText);

  AMemFree(theText);
  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_SetColumn //////////////////////////////////////////////////////////
//  Input 1:  Table
//  Input 2:  Column Object
//  Input 3:  Column Text
//  Flow  1:  After setting
//  Flow  2:  Invalid column

void far pascal _export Tabl_SetColumn(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE       table(tableObject);
  if(table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Set Column\" function is valid for manual-fill tables only.");
    }

  OBJECTID    columnObject = AFuncGetParameter(2L);
  int         colIndex = table.ColumnIndex(columnObject);

  OBJECTID    textObject = AFuncGetTypedParameter(3L, OTYPE_TEXT);

  if(colIndex<0 || colIndex>=table.TotalNumCols())
    AFuncReportResult(theSystem, 2L, NULL);
  else
    {
    MEMBLOCKID  theText = AMemAllocate(0L);

    AObjConvertToText(textObject, theText);
    table.SetColumn(columnObject, theText);
    AMemFree(theText);

    AFuncReportResult(theSystem, 1L, NULL);
    }
  }


// Tabl_SetColumnTitle /////////////////////////////////////////////////////
//  Input 1:  Table
//  Input 2:  Column Object
//  Input 3:  Title
//  Flow  1:  After setting
//  Flow  2:  Invalid column

void far pascal _export Tabl_SetColumnTitle(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return; 
  TABLE       table(tableObject);

  OBJECTID    columnObject = AFuncGetParameter(2L);
  int         index = table.ColumnIndex(columnObject);

  OBJECTID    titleObject = AFuncGetTypedParameter(3L, OTYPE_TEXT);
  MEMBLOCKID  theText = AMemAllocate(0L);
  if(titleObject)
    AObjConvertToText(titleObject, theText);
  else
    {
    AMemSetSize(theText, 20L);
    char* text = (char*)AMemLock(theText);
    wsprintf(text, "Column %i", index+1);
    AMemUnlock(theText);
    }

  if(index<0 || index>=table.TotalNumCols())
    AFuncReportResult(theSystem, 2L, NULL);
  else
    {
    table.SetColumnTitle(index, theText);
    AFuncReportResult(theSystem, 1L, NULL);
    }

  AMemFree(theText);
  }

// Tabl_SetColumnWidth /////////////////////////////////////////////////////
//  Input 1:  Table
//  Input 2:  Column Object
//  Input 3:  Width (pixels)
//  Flow  1:  After setting
//  Flow  2:  Invalid column

void far pascal _export Tabl_SetColumnWidth(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE       table(tableObject);

  BOOL        doAll = FALSE;
  OBJECTID    columnObject = AFuncGetParameter(2L);
  if(!columnObject)
    doAll = TRUE;

  int         colIndex = table.ColumnIndex(columnObject);
  if(colIndex<0 || colIndex>=table.TotalNumCols())
    {
    AFuncReportResult(theSystem, 2L, NULL);
    return;
    }

  OBJECTID    widthObject = AFuncGetTypedParameter(3L, OTYPE_NUMBER);
  int         width;

  if(widthObject)
    width = (int)Tbl_GetNmbrObj(widthObject);
  else
    {
    if(!doAll)
      width = table.GetMaxWidth(colIndex);
    }

  if(doAll)
    {
    for(int i=0; i<table.TotalNumCols(); i++)
      {
      if(!widthObject)
        width = table.GetMaxWidth(i);

      table.SetColumnWidth(i, width);
      }
    }
  else
    table.SetColumnWidth(colIndex, width);

  table.PostObjectChanged(CHANGED_FULLREDRAW);
  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_SetDataSource //////////////////////////////////////////////////////
//  Input 1:  Table
//  Input 2:  Data Source
//  Flow  1:  After setting
//  Flow  2:  Invalid source

void far pascal _export Tabl_SetDataSource(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID          tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE     table(tableObject);
  if(!table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Set Data Source\" function is valid for auto-fill tables only.");
    return;
    }

  OBJECTID  dataSourceObj = AFuncGetParameter(2L);
  TYPEID    theType = AObjGetType(dataSourceObj);
  if(dataSourceObj && !AEvtCheckForMethod(theType, autoTableReadRecords))
    AFuncReportResult(theSystem, 2L, NULL);
  else if(table.DataSource() != dataSourceObj)
    {
    AObjUnregisterOwner(tableObject, table.DataSource(), 0L);
    table.SetDataSource(dataSourceObj);
    AObjRegisterOwner(tableObject, table.DataSource(), 0L);

    if(!table.ValidIndexedField(table.FieldIndex()))
      table.SetFieldIndex(NULL);

    table.UpdateAutoTable(0L);
    AFuncReportResult(theSystem, 1L, NULL);
    }
  else
    AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_SetIndexedField ////////////////////////////////////////////////////
//  Input 1:  Table
//  Input 2:  Indexed Field
//  Flow  1:  After setting
//  Flow  2:  Invalid field

void far pascal _export Tabl_SetIndexedField(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE     table(tableObject);
  if(!table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Set Indexed Field\" function is valid for auto-fill tables only.");
    return;
    }

  OBJECTID  idxFieldObj = AFuncGetParameter(2L);
  if(table.ValidIndexedField(idxFieldObj))
    {
    if(table.FieldIndex()!=idxFieldObj)
      {
      table.SetFieldIndex(idxFieldObj);
      table.UpdateAutoTable(0L);
      }
    AFuncReportResult(theSystem, 1L, NULL);
    }
  else
    AFuncReportResult(theSystem, 2L, NULL);
  }

// Tabl_SetRow /////////////////////////////////////////////////////////////
//  Input 1:  Table
//  Input 2:  Text
//  Input 3:  Row Index
//  Flow  1:  After setting

void far pascal _export Tabl_SetRow(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE     table(tableObject);
  if(table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Set Row\" function is valid for manual-fill tables only.");
    return;
    }

  OBJECTID    textObject = AFuncGetTypedParameter(2L, OTYPE_TEXT);
  OBJECTID    indexObject = AFuncGetTypedParameter(3L, OTYPE_NUMBER);

  MEMBLOCKID  theText = NULL;
  char*       text = NULL;
  long        index = -1L;

  if(textObject)
    {
    theText = AMemAllocate(0L);
    AObjConvertToText(textObject, theText);
    text = (char*)AMemLock(theText);
    }
  if(indexObject)
    index = (long)Tbl_GetNmbrObj(indexObject);

  table.SetRow(textObject, text, indexObject, index);

  if(textObject)
    {
    AMemUnlock(theText);
    AMemFree(theText);
    }
  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_SetSelection ///////////////////////////////////////////////////////
//  Input 1:  Table
//  Input 2:  Start Row Index
//  Input 3:  End Row Index
//  Flow  1:  After setting

void far pascal _export Tabl_SetSelection(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;

  OBJECTID    startObject = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
  if(!startObject)
    {
    AFuncReportResult(theSystem, 1L, "Table Set Selection:  You must specify a valid starting row.");
    return;
    }

  OBJECTID    stopObject = AFuncGetTypedParameter(3L, OTYPE_NUMBER);
  long        start = -1L;
  long        stop = -1L;
  TABLE       table(tableObject);

  if (startObject)
    start =  (long)Tbl_GetNmbrObj(startObject);
  if(stopObject)
    stop =  (long)Tbl_GetNmbrObj(stopObject);

  table.tSetSelection(start, stopObject, stop);

  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_SortAscending //////////////////////////////////////////////////////
//  Input 1:  Table
//  Input 2:  Column Object
//  Flow  1:  After sorting
//  Flow  2:  Invalid column

void far pascal _export Tabl_SortAscending(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE     table(tableObject);
  if(table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Sort Ascending\" function is valid for manual-fill tables only.");
    return;
    }

  OBJECTID  columnObject = AFuncGetParameter(2L);

  int       index = table.ColumnIndex(columnObject);
  if(index<0 || index>=table.TotalNumCols())
    AFuncReportResult(theSystem, 2L, NULL);
  else
    {
    table.SortTable(columnObject, SORT_ASCENDING);
    AFuncReportResult(theSystem, 1L, NULL);
    }
  }

// Tabl_SortDescending /////////////////////////////////////////////////////
//  Input 1:  Table
//  Input 2:  Column Object
//  Flow  1:  After sorting
//  Flow  2:  Invalid column

void far pascal _export Tabl_SortDescending(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE     table(tableObject);
  if(table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Sort Descending\" function is valid for manual-fill tables only.");
    return;
    }

  OBJECTID  columnObject = AFuncGetParameter(2L);

  int       index = table.ColumnIndex(columnObject);
  if(index<0 || index>=table.TotalNumCols())
    AFuncReportResult(theSystem, 2L, NULL);
  else
    {
    table.SortTable(columnObject, SORT_DESCENDING);
    AFuncReportResult(theSystem, 1L, NULL);
    }
  }

// Tabl_SortRowAscending ///////////////////////////////////////////////////
//  Input 1:  Table
//  Input 2:  Column Object
//  Input 3:  Row Index
//  Flow  1:  After sorting
//  Flow  2:  Invalid column

void far pascal _export Tabl_SortRowAscending(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE     table(tableObject);
  if(table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Sort Row\" function is valid for manual-fill tables only.");
    return;
    }

  OBJECTID    columnObject = AFuncGetParameter(2L);
  OBJECTID    indexObject = AFuncGetTypedParameter(3L, OTYPE_NUMBER);
  long        index = -1;

  if(indexObject)
    index = (long)Tbl_GetNmbrObj(indexObject);

  int colIndex = table.ColumnIndex(columnObject);
  if(colIndex<0 || colIndex>=table.TotalNumCols())
    AFuncReportResult(theSystem, 2L, NULL);
  else
    {
    OBJECTID    resultObject = AObjCreate(OTYPE_NUMBER);
    Tbl_SetNmbrObj(resultObject, (long double)table.SortRow(columnObject, indexObject, index, SORT_ASCENDING) + 1);

    AFuncReportResult(theSystem, 1L, NULL);
    AFuncSetParameter(1L, resultObject);
    }
  }

// Tabl_SortRowDescending //////////////////////////////////////////////////
//  Input 1:  Table
//  Input 2:  Column Object
//  Input 3:  Row Index
//  Flow  1:  After sorting
//  Flow  2:  Invalid column

void far pascal _export Tabl_SortRowDescending(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE     table(tableObject);
  if(table.AutoFill())
    {
    AFuncReportResult(theSystem, 1L, "The \"Sort Row\" function is valid for manual-fill tables only.");
    return;
    }

  OBJECTID    columnObject = AFuncGetParameter(2L);
  OBJECTID    indexObject = AFuncGetTypedParameter(3L, OTYPE_NUMBER);
  long        index = -1;

  if(indexObject)
    index = (long)Tbl_GetNmbrObj(indexObject);

  int colIndex = table.ColumnIndex(columnObject);
  if(colIndex<0 || colIndex>=table.TotalNumCols())
    AFuncReportResult(theSystem, 2L, NULL);
  else
    {
    OBJECTID    resultObject = AObjCreate(OTYPE_NUMBER);
    Tbl_SetNmbrObj(resultObject, (long double) table.SortRow(columnObject, indexObject, index, SORT_DESCENDING) + 1);
    AFuncReportResult(theSystem, 1L, NULL);
    AFuncSetParameter(1L, resultObject);
    }
  }

// Tabl_UpdateAutoTable ////////////////////////////////////////////////////
//  Input 1:  Table
//  Input 2:  Count
//  Flow  1:  After updating
//  Flow  2:  Invalid count

void far pascal _export Tabl_UpdateAutoTable(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID    tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;
  TABLE     table(tableObject);

  if(!table.AutoFill())
    {

    AFuncReportResult(theSystem, 1L, "The \"Update Auto Table\" function is valid for auto-fill tables only.");
    return;
    }

  OBJECTID    indexObj = AFuncGetTypedParameter(2L, OTYPE_NUMBER);
  long        numRows = -1L;
  if (indexObj)
    numRows = (long)Tbl_GetNmbrObj(indexObj);
  if(!table.UpdateAutoTable(numRows))
    AFuncReportResult(theSystem, 2L, NULL);
  else
    AFuncReportResult(theSystem, 1L, NULL);
  }

////////////////////////////////////////////////////////////////////////////

void far pascal _export Tabl_DataLength(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObj = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObj)
    {
    AFuncReportResult(theSystem, 1L, "Invalid table object");
    return;
    }

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(tableObj);
  ROW*          row = rtime->GetRow();
  char          szBuffer[256];
  wsprintf(szBuffer, "num rows: %li\n\ndata length: %li", row->NumRows(), row->DataLength());
  AFuncReportResult(theSystem, 1L, szBuffer);
  }


// Tabl_NumOfVisRows ///////////////////////////////////////////////////////
//  Input 1: Table -- table to get the # ofVisible Rows from
//  Flow  1: After getting ...

extern "C" void far pascal _export Tabl_CountVisibleRows(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;

  TABLE_ATTRIBUTES  table(tableObject);

  // update the scroll ranges!
  long  vRows = table.NumVisRows();

  OBJECTID    resultObject = AObjCreate(OTYPE_NUMBER);
  Tbl_SetNmbrObj(resultObject, (long double)vRows );

  AFuncSetParameter(1L, resultObject);
  AFuncReportResult(theSystem, 1L, NULL);
  }

// Tabl_NumOfVisRows ///////////////////////////////////////////////////////
//  Input 1: Table -- table to get the #of vis rows from
//  Flow  1: After getting ...

extern "C" void far pascal _export Tabl_CountRowsAbove(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)

  {
  OBJECTID  tableObject = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObject)
    return;

  TABLE     table(tableObject);
  // update the scroll ranges!
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(tableObject);
  ROW*          row = rtime->GetRow();
  int           nRows = table.FirstRowToScrollPos(row->NumRows());

  OBJECTID      resultObject = AObjCreate(OTYPE_NUMBER);
  Tbl_SetNmbrObj(resultObject, (long double)nRows);

  AFuncSetParameter(1L, resultObject);
  AFuncReportResult(theSystem, 1L, NULL);
  }

////////////////////////////////////////////////////////////////////////////
//
//  FUNCTION:     Tabl_ActivateCell
//
//  DESCRIPTION:  This will activate the specified cell
//
//  INPUTS:       (1) Table Object
//                (2) Column
//                (3) Row
//
//  FLOWS:        (1) Activated
//                (2) Invalid Cell
//
////////////////////////////////////////////////////////////////////////////

extern "C" void ALMCALLBACK Tabl_ActivateCell
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);
  AFuncReportResult(pMsg, 1L, NULL);
  }

////////////////////////////////////////////////////////////////////////////
#ifdef TABLEAPIDEBUG
extern "C" void far pascal _export TestAPIs(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        theSystem)
  {
  int debug = 0;
  OBJECTID  tableObj = AFuncGetTypedParameter(1L, OTYPE_TABLE);
  if(!tableObj)
    {
    AFuncReportResult(theSystem, 1L, "Invalid table object");
    return;
    }

  int rowCount = tblGetColumnCount(tableObj); // (oiTable = 2729L)
  tblSetColumnCount(tableObj, 1);
  tblColumnTitle(tableObj, "Language", 0);
  tblSetRow(tableObj, "ITALIANO\r\nFRANCAIS\r\nESPANOL\r\nENGLISH\r\nDEUTSCH", 0);
  tblSetColumnCount(tableObj, 2);
  tblSetColumnCount(tableObj, 1);
  tblColumnTitle(tableObj, "Language", 0);

  AFuncReportResult(theSystem, 1L, NULL);

  }

#endif
