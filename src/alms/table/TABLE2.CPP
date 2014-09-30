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
// FILE:  "table2.cpp"
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
#include  <op_wnd.h>
#include  <o_wnd.h>
#include  <o_text.h>
#include  <o_number.h>
#include  <o_time.h>
#include  <o_date.h>
#include  "o_datb.h"
#include  "o_table.h"

#include  "tabl_hdr.h"  // for hInstance
#include  "op_dview.h"
#include  "table.h"
#include  "tabl_row.h"

// Externals ///////////////////////////////////////////////////////////////

extern GLOBAL G;

// TABLE::ColumnIndex //////////////////////////////////////////////////////

int TABLE::ColumnIndex(
  AOBJECTID  colObject)

  {
  for(int i=0; i<COLUMN::NumColumns(); i++)
    {
    if(colObject == COLUMN::GetObjectID(i))
      return i;
    }

  if(!colObject)
    return TABLE_ATTRIBUTES::FocusColumn();
  else if(AObjCheckType(colObject, OTYPE_NUMBER))
    return (int)ONmbrGetInteger(colObject) - 1;
  else
    {
    AMEMBLOCKID  mem = AMemAllocate(0L);
    AObjConvertToText(colObject, mem);
    char*       txt = (char*)AMemLock(mem);
    if(txt)
      {
      i = (int)ONmbrConvertFromTextInteger(NULL, txt) - 1;
      AMemUnlock(mem);
      AMemFree(mem);
      return i;
      }
    else
      {
      AMemFree(mem);
      return -1;
      }
    }
  }

// TABLE::ActivateColumn ///////////////////////////////////////////////////

BOOL  TABLE::ActivateColumn(
  AOBJECTID  colObject)

  {
  BOOL rVal = FALSE;    // return FALSE if colObject is not valid
  for(int i=0; i<COLUMN::NumColumns(); i++)
    {
    if(colObject == COLUMN::GetObjectID(i))
      {
      if(i!=TABLE_ATTRIBUTES::FocusColumn())
        {
	TABLE_ATTRIBUTES::SetFocusColumn(i);
        if(TABLE_ATTRIBUTES::ShowSelection())
          PostObjectChanged(CHANGED_COLUMNSELECTION);
        }
      break;
      }
    rVal = TRUE;
    }
  return rVal;
  }

// TABLE::ActiveCell ///////////////////////////////////////////////////////

BOOL TABLE::ActiveCell(
  long*     rowIndex,
  AOBJECTID* colObject)

  {
  if(COLUMN::NumColumns()<=0)
    return FALSE;

  *rowIndex = TABLE_ATTRIBUTES::ActiveRow()+1;
  if(TABLE_ATTRIBUTES::ActiveColumn()>=0
       &&
     TABLE_ATTRIBUTES::ActiveColumn()<COLUMN::NumColumns())
    {
    *colObject = COLUMN::GetObjectID(TABLE_ATTRIBUTES::ActiveColumn());
    }
  else
    *colObject = NULL;

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();

  return row->IsSelected(*rowIndex - 1L);
  }

// TABLE::ActiveColumn /////////////////////////////////////////////////////

AOBJECTID TABLE::ActiveColumn()

  {
  if(COLUMN::NumColumns()<=0)
    return NULL;

  if(TABLE_ATTRIBUTES::FocusColumn()>=0
      &&
     TABLE_ATTRIBUTES::FocusColumn()<COLUMN::NumColumns())
    {
    return COLUMN::GetObjectID(TABLE_ATTRIBUTES::FocusColumn());
    }
  else
    return NULL;
  }

// TABLE::AddRow ///////////////////////////////////////////////////////////

void TABLE::AddRow(
  AOBJECTID  textObject,
  char*     text,
  AOBJECTID  indexObject,
  long      index)

  {
  if(COLUMN::NumColumns()==0)
    return;

  DestroyAllEditControls();

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();

  if(!indexObject)
    {
    // add from the current FIRST selection
    index = row->FirstSelection();
    }
  else
    {
    // add from the index (1==first row)
    index--;  // convert to zero-based rows
    if(index<0)
      index=0;
    if(index>row->NumRows())
      index = row->NumRows();
    }

  if(!textObject)
    {
    AMEMBLOCKID  hText = AMemAllocate(0L);
    AMEMBLOCKID  hObject = AMemAllocate(0L);
    char*       txtPtr;
    char*       objPtr;
    long        txtSize = 0L;
    long        objSize;
    long        zero = 0L;
    for(int i=0; i<COLUMN::NumColumns(); i++)
      {
      AObjConvertToText(COLUMN::GetObjectID(i), hObject);
      objPtr = (char*)AMemLock(hObject);
      if(objPtr)
        {
        objSize = lstrlen(objPtr)+1;
        AMemSetSize(hText, txtSize+objSize);
        txtPtr = (char*)AMemLock(hText);
        CopyMemory((char *)txtPtr + txtSize, objPtr, objSize);
        txtSize += objSize;
        *objPtr = 0;
        AMemUnlock(hText);
        }
      else
        {
        AMemSetSize(hText, txtSize+1);
        txtPtr = (char*)AMemLock(hText);
	*((char *)txtPtr + txtSize++) = 0;
        AMemUnlock(hText);
        }

      AMemUnlock(hObject);
      }
    row->InsertData(index, 1, txtPtr, (UINT)txtSize, &zero);
    AMemFree(hObject);
    AMemFree(hText);
    }
  else
    row->AddRow(index, text, COLUMN::NumColumns(), FALSE);

  if(hVertScroll && (row->NumRows()>numVisRows))
    {
    SetScrollRange(hVertScroll, SB_CTL, 0, NumRowsToScrollRange(row->NumRows()), TRUE);
    EnableWindow(hVertScroll, TRUE);
    }

  PostObjectChanged(CHANGED_NUMROWS);
  }

// TABLE::CellEdited ///////////////////////////////////////////////////////

BOOL TABLE::CellEdited(
  long*     rowIndex,
  AOBJECTID* colObject)

  {
  if(COLUMN::NumColumns()<=0)
    return FALSE;

  *rowIndex = TABLE_ATTRIBUTES::EditRow()+1L;
  if(*rowIndex>0L)
    {
    int index = TABLE_ATTRIBUTES::EditColumn();
    if(index>=0 && index<COLUMN::NumColumns())
      *colObject = COLUMN::GetObjectID(index);
    else
      *colObject = NULL;
    }

  return (*rowIndex>0L);
  }

// TABLE::ClearSelections //////////////////////////////////////////////////

void TABLE::ClearSelections()

  {
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();

  row->ClearSelections();
  PostObjectChanged(CHANGED_FULLREDRAW);
  }

// TABLE::ColumnCalculation ////////////////////////////////////////////////

long double TABLE::ColumnCalculation(
  int       operation,
  int       colIndex,
  long      start,
  long      stop,
  AOBJECTID  resultObject)

  {
  BOOL          dbAccessed = FALSE;
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();

  if(row->NumRows()==0 || COLUMN::NumColumns()==0)
    return 0.0;

  if((colIndex<0) || (colIndex>=COLUMN::NumColumns()))
    colIndex = TABLE_ATTRIBUTES::ActiveColumn();

  if(!resultObject || !AObjCheckType(COLUMN::GetObjectID(colIndex), OTYPE_NUMBER))
    return 0.0;

  if(start<0L)
    start=0L;
  if(start>=row->NumRows())
    start=row->NumRows()-1;

  if(stop<0L)
    stop=0L;
  if(stop>=row->NumRows())
    stop=row->NumRows()-1;

  if(start>stop)
    {
    long  temp = start;
    start = stop;
    stop = temp;
    }

  long double result = 0.0;
  long double val;

  // guarantee that number is a floating point number
  ONmbrSetType(resultObject, ONMBR_REAL);

  AMEMBLOCKID  hText = AMemAllocate(0L);
  char*       text;
  HPSTR       cellText;
  long        i;

  switch(operation)
    {
    case OP_SUM:
      for(i=start; i<=stop; i++)
        {
        cellText = row->GetCell(i, colIndex, TRUE, &dbAccessed);
        AMemSetSize(hText, lstrlen(cellText)+1);
        text = (char*)AMemLock(hText);
        CopyMemory(text, cellText, lstrlen(cellText)+1);
        result += ONmbrConvertFromTextReal(NULL, text);
        AMemUnlock(hText);
        }
      break;

    case OP_AVERAGE:
      for(i=start; i<=stop; i++)
        {
        cellText = row->GetCell(i, colIndex, TRUE, &dbAccessed);
        AMemSetSize(hText, lstrlen(cellText)+1);
        text = (char*)AMemLock(hText);
        CopyMemory(text, cellText, lstrlen(cellText)+1);
        result += ONmbrConvertFromTextReal(NULL, text);
        AMemUnlock(hText);
        }
      result = result / (stop - start + 1.0);
      break;

    case OP_MIN:
      for(i=start; i<=stop; i++)
        {
        cellText = row->GetCell(i, colIndex, TRUE, &dbAccessed);
        AMemSetSize(hText, lstrlen(cellText)+1);
        text = (char*)AMemLock(hText);
        CopyMemory(text, cellText, lstrlen(cellText)+1);
        val = ONmbrConvertFromTextReal(NULL, text);
        if((result > val) || (i==start))
          result = val;
        AMemUnlock(hText);
        }
      break;

    case OP_MAX:
      for(i=start; i<=stop; i++)
        {
        cellText = row->GetCell(i, colIndex, TRUE, &dbAccessed);
        AMemSetSize(hText, lstrlen(cellText)+1);
        text = (char*)AMemLock(hText);
        CopyMemory(text, cellText, lstrlen(cellText)+1);
        val = ONmbrConvertFromTextReal(NULL, text);
        if((result < val) || (i==start))
          result = val;
        AMemUnlock(hText);
        }
      break;

    default:
      AMemFree(hText);
      return 0.0;
    }

  AMemFree(hText);
  ONmbrSetReal(resultObject, result, TRUE);

  if(dbAccessed)
    AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);

  return result;
  }

// TABLE::DeleteColumn /////////////////////////////////////////////////////

void TABLE::DeleteColumn(
  int colIndex)

  {
  if(COLUMN::NumColumns()<=0)
    return;

  DestroyAllEditControls();

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();

  row->DeleteColumn(colIndex, COLUMN::NumColumns());
  COLUMN::DeleteColumn(colIndex);
  TABLE_ATTRIBUTES::SetNumColumns(COLUMN::NumColumns());

  PostObjectChanged(CHANGED_NUMCOLUMNS);
  }

// TABLE::DeleteRow ////////////////////////////////////////////////////////

void TABLE::DeleteRow(
  AOBJECTID  indexObject,
  long      index)

  {
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();

  DestroyAllEditControls();

  if(indexObject)
    {
    if(row->IsSelected(index))
      AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
    row->DeleteRow(index);
    }
  else
    {
    row->DeleteSelectedRows();
    AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
    }

  PostObjectChanged(CHANGED_NUMROWS);
  }

// TABLE::Match ////////////////////////////////////////////////////////////

BOOL TABLE::Match(
  AOBJECTID    oiColumn,
  ATYPEID      colType,
  AOBJECTID    valID,
  char*       value,
  long        valLen,
  AMEMBLOCKID  hTmp,
  AOBJECTID    tmpObj,
  char *  cellPtr,
  long        length,
  BOOL        exact)

  {
  if(colType==OTYPE_NUMBER)
    {
    char*       pCell;
    long        iTmp;
    long double fTmp;
    long        iVal = ONmbrGetInteger(valID);
    long double fVal;

    AMemSetSize(hTmp, length+1);
    pCell = (char*)AMemLock(hTmp);
    CopyMemory(pCell, cellPtr, length+1);
    ONmbrParseFormatedNumber(oiColumn, &fTmp, pCell);
    iTmp = (long)fTmp;
    AMemUnlock(hTmp);

    ONmbrGetReal(valID, &fVal);

    if (ONmbrGetType(valID) == ONMBR_INTEGER)
      {
      if(ONmbrGetType(tmpObj) == ONMBR_INTEGER)
        {
        if (iTmp == iVal)
          return TRUE;
        }
      else
        {
        if(exact)
          {
          if (fTmp == (long double)iVal)
            return TRUE; 
          }
        else
          {
          if ((long)fTmp == iVal)
            return TRUE;
          }
        }
      }
    else
      {
      if (ONmbrGetType(tmpObj) == ONMBR_INTEGER)
        {
        if(exact)
          {
          if ((long double)iTmp == fVal)
            return TRUE;
          }
        else
          {
          if (iTmp == fVal)
            return TRUE;
          }
        }
      else
        {
        if (fTmp == fVal)
          return TRUE;
        }
      }
    }
  else if(colType==OTYPE_DATE)
    {
    AMemSetSize(hTmp, length+1);
    CopyMemory(AMemLock(hTmp), cellPtr, length+1);
    AMemUnlock(hTmp);
    AObjConvertFromText(tmpObj, hTmp);
    if (ODateGetValue(tmpObj) == ODateGetValue(valID))
      return TRUE;
    }
  else if(colType==OTYPE_TIME)
    {
    AMemSetSize(hTmp, length+1);
    CopyMemory(AMemLock(hTmp), cellPtr, length+1);
    AMemUnlock(hTmp);
    AObjConvertFromText(tmpObj, hTmp);
    if (OTimeGetValue(tmpObj) == OTimeGetValue(valID))
      return TRUE;
    }
  else
    {
    if(exact)
      {
      if(!stricmp(value, cellPtr))
        return TRUE;
      }
    else
      {
      if(!memicmp(value, cellPtr, min(valLen, length)))
        return TRUE;
      }
    }

  return FALSE;
  }

// TABLE::Find /////////////////////////////////////////////////////////////

int TABLE::Find(
  int       colIndex,
  ATYPEID    colType,
  long      startRow,
  AOBJECTID  valObject,
  AOBJECTID  resultObject,
  BOOL      exact)

  {
  if(!valObject || COLUMN::NumColumns()<=0)
    return FOUND_NOTFOUND;

  BOOL          dbAccessed = FALSE;
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();

  if(startRow < 0)
    return FOUND_INVALIDROW;
  else if(startRow >= row->NumRows())
    return FOUND_INVALIDROW;

  AOBJECTID    tmpObj = AObjCreate(colType);
  if(colType==OTYPE_NUMBER || colType==OTYPE_DATE || colType==OTYPE_TIME)
    {
    if(!tmpObj || !valObject)
      return FOUND_NOTFOUND;
    }

  AMEMBLOCKID  hVal = AMemAllocate(0L);
  AObjConvertToText(valObject, hVal);
  char*       value = (char*)AMemLock(hVal);
  long        valLen = (long)lstrlen(value);
  AMEMBLOCKID  hTmp = AMemAllocate(0L);
  char *  cellPtr;
  long        length;

  for(long i=startRow; i<row->NumRows(); i++)
    {
    cellPtr = row->GetCell(i, colIndex, TRUE, &dbAccessed);
    length = lstrlen(cellPtr);
    if(Match(ColObjectID(colIndex), colType, valObject, value, valLen, hTmp, tmpObj, cellPtr, length, exact))
      break;
    }

  AObjDestroy(tmpObj);
  AMemFree(hVal);
  AMemFree(hTmp);

  if(dbAccessed)
    AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);

  if(i<row->NumRows())
    {
    Tbl_SetNmbrObj(resultObject, i + 1);

    return FOUND_FOUND;
    }
  else
    return FOUND_NOTFOUND;
  }

// TABLE::GetCell //////////////////////////////////////////////////////////

void TABLE::GetCell(
  AOBJECTID    columnObject,
  AOBJECTID    indexObject,
  long        index,
  AMEMBLOCKID  theText)

  {
  if(COLUMN::NumColumns()<=0)
    return;

  BOOL          dbAccessed = FALSE;
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();

  if(!indexObject)
    index = row->FirstSelection();
  else
    index--;

  // make sure the row and column are valid.
  if(index<0 || index>=row->NumRows())
    return;

  int colIndex = ColumnIndex(columnObject);
  if(colIndex>=COLUMN::NumColumns() || colIndex<0)
    colIndex = TABLE_ATTRIBUTES::FocusColumn();

  HPSTR text = row->GetCell(index, colIndex, FALSE, &dbAccessed);
  long  length = lstrlen(text) + 1;
  AMemSetSize(theText, length);
  CopyMemory(AMemLock(theText), text, length);
  AMemUnlock(theText);

  if(dbAccessed)
    AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);
  }

// TABLE::GetColumn ////////////////////////////////////////////////////////

void TABLE::GetColumn(
  AOBJECTID    columnObject,
  AMEMBLOCKID  theText)

  {
  if(COLUMN::NumColumns()<=0)
    return;

  BOOL        dbAccessed = FALSE;
  int         colIndex = ColumnIndex(columnObject);
  if(colIndex>=COLUMN::NumColumns() || colIndex<0)
    colIndex = TABLE_ATTRIBUTES::FocusColumn();

  char*       text = (char*)AMemLock(theText);
  if(!text)
    return;

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  UINT          numBytesUsed = 0;
  char *    cell;
  int           cellLength;
  for(long j=0; j<row->NumRows(); j++)
    {
    cell = row->GetCell(j, colIndex, TRUE, &dbAccessed);
    cellLength = (int)lstrlen(cell);
    if(j!=0)
      {
      if(numBytesUsed < (32000 - 3))
        {
        CopyMemory(text+numBytesUsed, G.CRLF, 2);
        numBytesUsed += 2;
        }
      else
        {
        *(text+numBytesUsed) = 0;

        if(dbAccessed)
          AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);

        AMemUnlock(theText);
        return;
        }
      }

    if(numBytesUsed < (32000 - cellLength - 1))
      {
      CopyMemory(text+numBytesUsed, cell, cellLength);
      numBytesUsed += cellLength;
      }
    else
      {
      *(text+numBytesUsed) = 0;

      if(dbAccessed)
        AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);

      AMemUnlock(theText);
      return;
      }
    }

  *(text+numBytesUsed) = 0;

  AMemUnlock(theText);

  if(dbAccessed)
    AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);
  }

// TABLE::GetFullColumn ////////////////////////////////////////////////////

BOOL TABLE::GetFullColumn(
  int         colIndex,
  AMEMBLOCKID  theText)

  {
  if(COLUMN::NumColumns()<=0)
    return FALSE;

  BOOL          dbAccessed = FALSE;

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  long          length = 0L;
  char *    cell;
  long          cellLength;

  for(long j=0; j<row->NumRows(); j++)
    {
    cell = row->GetCell(j, colIndex, TRUE, &dbAccessed);
    length += (lstrlen(cell) + 1);
    }

  AMemSetSize(theText, length);
  char *  text = (char *)AMemLock(theText);
  if(!text)
    return FALSE;
  length = 0L;

  for(j=0; j<row->NumRows(); j++)
    {
    if(j!=0)
      {
      // append a tab after each line
      length++;
      *(text+length-1) = '\t';
      }
    cell = row->GetCell(j, colIndex, TRUE, &dbAccessed);
    cellLength = lstrlen(cell);
    CopyMemory(text+length, cell, cellLength+1);
    length += cellLength;
    }

  AMemUnlock(theText);
  if(dbAccessed)
    AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);
  return TRUE;
  }

// TABLE::GetColumnTitle ///////////////////////////////////////////////////

void TABLE::GetColumnTitle(
  int       index,
  AOBJECTID  resultObject)

  {
  if(COLUMN::NumColumns()<=0)
    return;

  AMEMBLOCKID  theBlock = AMemAllocate(COLUMN_TITLEBUFSIZE);
  lstrcpy((char*)AMemLock(theBlock), COLUMN::Title(index));

  AObjConvertFromText(resultObject, theBlock);
  AMemUnlock(theBlock);
  AMemFree(theBlock);
  }

// TABLE::GetColumnWidth ///////////////////////////////////////////////////

int TABLE::GetColumnWidth(
  AOBJECTID  columnObject)

  {
  if(COLUMN::NumColumns()<=0)
    return 0;

  int colIndex = ColumnIndex(columnObject);
  if(colIndex>=COLUMN::NumColumns() || colIndex<0)
    colIndex = TABLE_ATTRIBUTES::FocusColumn();

  return COLUMN::ColumnWidth(colIndex);
  }

// TABLE::GetRow ///////////////////////////////////////////////////////////

void TABLE::GetRow(
  AOBJECTID  indexObject,
  long      index)

  {
  if(COLUMN::NumColumns()<=0)
    return;

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  AMEMBLOCKID    theText;
  char *    text;
  long          length;
  BOOL          dbAccessed = FALSE;

  if(!indexObject)
    index = row->FirstSelection();
  else
    index--;

  // make sure the row is valid.
  if(index<0 || index>=row->NumRows())
    return;

  theText = AMemAllocate(0L);
  for(int i=0; i<COLUMN::NumColumns(); i++)
    {
    text = row->GetCell(index, i, FALSE, &dbAccessed);
    length = lstrlen(text) + 1;
    AMemSetSize(theText, length);
    CopyMemory(AMemLock(theText), text, length);
    AObjConvertFromText(COLUMN::GetObjectID(i), theText);
    AMemUnlock(theText);
    }
  AMemFree(theText);

  if(dbAccessed)
    AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);
  }

// TABLE::GetSelection /////////////////////////////////////////////////////

BOOL TABLE::tGetSelection(
  AOBJECTID  indexObject,
  long      index,
  AOBJECTID  outputObject)

  {
  if(COLUMN::NumColumns()<=0)
    return FALSE;

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();

  if(!indexObject)
    index = 0L;
  else
    index--;

  // make sure the row is valid.
  if(index<0 || index>=row->NumSelections())
    return FALSE;

  long  rowIndex = row->GetSelection(index);
  if(rowIndex<0 || rowIndex>=row->NumRows())
    return FALSE;

  if(outputObject)
    {
    Tbl_SetNmbrObj(outputObject, rowIndex + 1);

    return TRUE;
    }

  return FALSE;
  }

// TABLE::InsertColumn /////////////////////////////////////////////////////

void TABLE::InsertColumn(
  int       index,
  AOBJECTID  titleObject,
  AOBJECTID  columnObject)

  {
  DestroyAllEditControls();

  if(index<0)
    index=0;

  // get the title for the new column
  AMEMBLOCKID  theBlock = AMemAllocate(11L);
  char*       title;
  if(titleObject)
    {
    AObjConvertToText(titleObject, theBlock);
    title = (char*)AMemLock(theBlock);
    }
  else
    {
    if(columnObject)
      {
      AObjConvertToText(columnObject, theBlock);
      title = (char*)AMemLock(theBlock);
      }
    else
      {
      title = (char*)AMemLock(theBlock);
      lstrcpy(title, "New Column");
      }
    }

  // update the columns
  COLUMN::AddColumn(columnObject, title, index);
  TABLE_ATTRIBUTES::SetNumColumns(COLUMN::NumColumns());
  AMemUnlock(theBlock);
  AMemFree(theBlock);

  // update the rows
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  row->InsertColumn(index, COLUMN::NumColumns()-1);

  if(COLUMN::NumColumns()==1)
    {
    // update the TABLE_ATTRIBUTES
    InitFontMetrics();
    }

  // update the visual instances
  PostObjectChanged(CHANGED_NUMCOLUMNS);
  }

// TABLE::RowCalculation ///////////////////////////////////////////////////

long double TABLE::RowCalculation(
  int       operation,
  int       startCol,
  int       endCol,
  long      rowIndex,
  AOBJECTID  resultObject)

  {
  if(COLUMN::NumColumns()<=0)
    return 0.0;

  BOOL  dbAccessed = FALSE;

  if(startCol<0)
    startCol = 0;
  else if(startCol>=COLUMN::NumColumns())
    startCol = COLUMN::NumColumns() - 1;

  if(endCol<0)
    endCol = 0;
  else if(endCol>=COLUMN::NumColumns())
    endCol = COLUMN::NumColumns() - 1;

  if(startCol>endCol)
    {
    int temp = endCol;
    endCol = startCol;
    startCol = temp;
    }

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  if(row->NumRows()==0 || COLUMN::NumColumns()==0 || !resultObject)
    return 0.0;

  if(rowIndex<0 || rowIndex>=row->NumRows())
    rowIndex = row->FirstSelection();

  if(rowIndex>=row->NumRows())
    return 0.0;

  // guarantee that number is a floating point number
  ONmbrSetType(resultObject, ONMBR_REAL);

  long double result = 0.0;
  long double val;
  AMEMBLOCKID  hText = AMemAllocate(0L);
  char*       text;
  HPSTR       cellText;
  int         i;
  int         noOp = 0;

  switch(operation)
    {
    case OP_SUM:
      for(i=startCol; i<=endCol; i++)
        {
        if(!AObjCheckType(COLUMN::GetObjectID(i), OTYPE_NUMBER))
          continue;

        cellText = row->GetCell(rowIndex, i, TRUE, &dbAccessed);
        AMemSetSize(hText, lstrlen(cellText)+1);
        text = (char*)AMemLock(hText);
        CopyMemory(text, cellText, lstrlen(cellText)+1);
        result += ONmbrConvertFromTextReal(NULL, text);
        AMemUnlock(hText);
        }
      break;

    case OP_AVERAGE:
      for(i=startCol; i<=endCol; i++)
        {
        if(!AObjCheckType(COLUMN::GetObjectID(i), OTYPE_NUMBER))
          {
          noOp++;
          continue;
          }

        cellText = row->GetCell(rowIndex, i, TRUE, &dbAccessed);
        AMemSetSize(hText, lstrlen(cellText)+1);
        text = (char*)AMemLock(hText);
        CopyMemory(text, cellText, lstrlen(cellText)+1);
        result += ONmbrConvertFromTextReal(NULL, text);
        AMemUnlock(hText);
        }
      if(noOp<(endCol-startCol+1))
        result = result / (endCol - startCol - noOp + 1.0);
      else
        result = 0.0;
      break;

    case OP_MIN:
      for(i=startCol; i<=endCol; i++)
        {
        if(!AObjCheckType(COLUMN::GetObjectID(i), OTYPE_NUMBER))
          {
          noOp++;
          continue;
          }

        cellText = row->GetCell(rowIndex, i, TRUE, &dbAccessed);
        AMemSetSize(hText, lstrlen(cellText)+1);
        text = (char*)AMemLock(hText);
        CopyMemory(text, cellText, lstrlen(cellText)+1);
        val = ONmbrConvertFromTextReal(NULL, text);
        AMemUnlock(hText);

        if((result > val) || ((i-noOp)==startCol))
          result = val;
        }
      break;

    case OP_MAX:
      for(i=startCol; i<=endCol; i++)
        {
        if(!AObjCheckType(COLUMN::GetObjectID(i), OTYPE_NUMBER))
          {
          noOp++;
          continue;
          }

        cellText = row->GetCell(rowIndex, i, TRUE, &dbAccessed);
        AMemSetSize(hText, lstrlen(cellText)+1);
        text = (char*)AMemLock(hText);
        CopyMemory(text, cellText, lstrlen(cellText)+1);
        val = ONmbrConvertFromTextReal(NULL, text);
        AMemUnlock(hText);

        if((result < val) || ((i-noOp)==startCol))
          result = val;
        }
      break;

    default:
      AMemFree(hText);
      return 0.0;
    }

  AMemFree(hText);
  ONmbrSetReal(resultObject, result, TRUE);

  if(dbAccessed)
    AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);

  return result;
  }

// TABLE::RowToText ////////////////////////////////////////////////////////

void TABLE::RowToText(
  long        start,
  long        stop,
  AMEMBLOCKID  theText)

  {
  if(COLUMN::NumColumns()<=0)
    return;

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  long          numRows;
  long          length;
  char *    clip;
  long          sel;
  long          offset=0;
  long          i;
  BOOL          dbAccessed = FALSE;

  // If there are no rows in the table return a NULL string
  if (row->NumRows() <= 0)
    {
    AMemSetSize(theText, 1L);
    *(char*)AMemLock(theText) = 0;
    AMemUnlock(theText);
    return;
    }


  if(start<0L && stop<0L)
    { // get the text from all selected rows
    numRows = row->NumSelections();
    length = numRows;
    if(numRows<1 || numRows>row->NumRows())
      {
      AMemSetSize(theText, 1L);
      *(char*)AMemLock(theText) = 0;
      AMemUnlock(theText);
      return;
      }

    for(i=0; i<numRows; i++)
      length += row->RowLength(row->GetSelection(i), TRUE, &dbAccessed);

    AMemSetSize(theText, length);
    clip = (char *)AMemLock(theText);
    if(!clip)
      {
      AMemSetSize(theText, 1L);
      *(char*)AMemLock(theText) = 0;
      AMemUnlock(theText);

      if(dbAccessed)
        AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);

      return;
      }

    for(i=0; i<numRows; i++)
      {
      sel = row->GetSelection(i);
      length = row->RowLength(sel, TRUE, &dbAccessed);
      row->GetRow(sel, clip+offset, length, TRUE, TRUE, &dbAccessed);
      if(i!=(numRows-1))
        {
        // give the cr-lf combination
        *(clip+(offset + length - 1)) = '\r';
        *(clip+(offset + length)) = '\n';
        length++;
        }
      else
        *(clip+(offset + length - 1)) = 0;

      offset+=length;
      }
    AMemUnlock(theText);
    }
  else if(start>=0L && stop<0L)
    { // get the text from start row
    if(start>=row->NumRows())
      {
      AMemSetSize(theText, 1L);
      *(char*)AMemLock(theText) = 0;
      AMemUnlock(theText);

      if(dbAccessed)
        AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);

      return;
      }

    length = row->RowLength(start, FALSE, &dbAccessed);
    AMemSetSize(theText, length);
    char *  clip = (char *)AMemLock(theText);
    if(!clip)
      {
      AMemSetSize(theText, 1L);
      *(char*)AMemLock(theText) = 0;
      AMemUnlock(theText);

      if(dbAccessed)
        AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);

      return;
      }

    row->GetRow(start, clip, length, TRUE, TRUE, &dbAccessed);
    *(clip+(length - 1)) = 0;
    AMemUnlock(theText);
    }
  else
    { // get a range selection
    if(stop>=row->NumRows())
      stop = row->NumRows()-1;

    if(start<0)
      start = 0;
    else if(start>=row->NumRows())
      start = row->NumRows()-1;

    if(stop<start)
      {
      long  temp = start;
      start = stop;
      stop = temp;
      }

    numRows = stop-start+1;
    length = numRows;

    for(i=start; i<=stop; i++)
      length += row->RowLength(i, TRUE, &dbAccessed);

    AMemSetSize(theText, length);
    clip = (char *)AMemLock(theText);
    if(!clip)
      {
      AMemSetSize(theText, 1L);
      clip = (char *)AMemLock(theText);
      if(clip)
        *clip = 0;
      AMemUnlock(theText);

      if(dbAccessed)
        AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);

      return;
      }

    for(i=start; i<=stop; i++)
      {
      length = row->RowLength(i, TRUE, &dbAccessed);
      row->GetRow(i, clip+offset, length, TRUE, TRUE, &dbAccessed);
      if(i!=stop)
        {
        // give the cr-lf combination
        *(clip+(offset + length - 1)) = '\r';
        *(clip+(offset + length)) = '\n';
        length++;
        }
      else
        *(clip+(offset + length - 1)) = 0;

      offset+=length;
      }
    AMemUnlock(theText);
    }

  if(dbAccessed)
    AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);
  }

// TABLE::ScrollToSelection ////////////////////////////////////////////////

BOOL TABLE::ScrollToSelection(
  BOOL  redraw)

  {
  if(COLUMN::NumColumns()<=0)
    return FALSE;

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  if(row->NumSelections()==0)
    return FALSE;

  long  first = row->FirstSelection();
  if(first>=firstRow && first<(firstRow+numVisRows))
    return FALSE;

  if(hEdit)
    DestroyEditControl();

  if(first<firstRow)
    firstRow = first;
  else
    firstRow = first - numVisRows + 1L;

  if(firstRow < 0L)
    firstRow = 0L;

  if(hVertScroll && row->NumRows()>numVisRows)
    {
    EnableWindow(hVertScroll, TRUE);
    SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
    }
  else if(hVertScroll)
    EnableWindow(hVertScroll, FALSE);

  if(redraw)
    InvalidateTableRect();

  return TRUE;
  }


// TABLE::SetCell //////////////////////////////////////////////////////////

void TABLE::SetCell(
  AOBJECTID    indexObject,
  long        index,
  AOBJECTID    columnObject,
  AOBJECTID    textObject,
  AMEMBLOCKID  hText)

  {
  if(COLUMN::NumColumns()<=0)
    return;

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();

  if(!indexObject)
    index = row->FirstSelection();
  else
    index--;

  // make sure the row and column are valid.
  if(index<0 || index>=row->NumRows())
    return;

  int colIndex = ColumnIndex(columnObject);
  DestroySomeEditControls(index, colIndex);

  if(colIndex>=COLUMN::NumColumns() || colIndex<0)
    colIndex = TABLE_ATTRIBUTES::FocusColumn();

  if(!textObject)
    AObjConvertToText(columnObject, hText);
  else
    AObjConvertToText(textObject, hText);

  row->SetCell(index, colIndex, (char*)AMemLock(hText));
  AMemUnlock(hText);

  PostObjectChanged(CHANGED_FULLREDRAW);
  }

// TABLE::SetColumn ////////////////////////////////////////////////////////

void TABLE::SetColumn(
  AOBJECTID    columnObject,
  AMEMBLOCKID  theText)

  {
  if(COLUMN::NumColumns()<=0)
    return;

  DestroyAllEditControls();

  int colIndex = ColumnIndex(columnObject);
  if(colIndex>=COLUMN::NumColumns() || colIndex<0)
    colIndex = TABLE_ATTRIBUTES::FocusColumn();

  char* text = (char*)AMemLock(theText);
  if(!text)
    return;

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  row->SetColumn(text, colIndex, COLUMN::NumColumns());

  PostObjectChanged(CHANGED_NUMROWS);
  AMemUnlock(theText);
  }

// TABLE::SetColumnTitle ///////////////////////////////////////////////////

void TABLE::SetColumnTitle(
  int         index,
  AMEMBLOCKID  theTitle)

  {
  COLUMN::SetTitle(index, (char*)AMemLock(theTitle));
  AMemUnlock(theTitle);

  // redraw the column titles
  PostObjectChanged(CHANGED_COLUMNSELECTION);
  }

// TABLE::SetColumnWidth ///////////////////////////////////////////////////

void TABLE::SetColumnWidth(
  int colIndex,
  int width)

  {
  if(COLUMN::NumColumns()<=0)
    return;

  // force a valid width
  if(width<0)
    width = 0;
  if(width>999)
    width = 999;

  if(COLUMN::ColumnWidth(colIndex)!=width)
    {
    COLUMN::SetColumnWidth(colIndex, width);
    PostObjectChanged(CHANGED_COLUMNWIDTH);
    }
  }

// TABLE::SetRow ///////////////////////////////////////////////////////////

void TABLE::SetRow(
  AOBJECTID  textObject,
  char*     text,
  AOBJECTID  indexObject,
  long      index)

  {
  if(COLUMN::NumColumns()<=0)
    return;

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();

  DestroyAllEditControls();

  if(!indexObject)
    index = row->FirstSelection();
  else
    index--;  // convert to zero-based rows

  // make sure the row is valid.
  if(index<0 || index>=row->NumRows())
    return;

  if(!textObject)
    {
    AMEMBLOCKID  hText = AMemAllocate(0L);
    AMEMBLOCKID  hObject = AMemAllocate(0L);
    char*       txtPtr;
    char*       objPtr;
    long        txtSize = 0L;
    UINT        objSize;
    long        zero = 0L;
    for(int i=0; i<COLUMN::NumColumns(); i++)
      {
      AObjConvertToText(COLUMN::GetObjectID(i), hObject);
      objPtr = (char*)AMemLock(hObject);
      objSize = (UINT)lstrlen(objPtr)+1;
      AMemSetSize(hText, txtSize+objSize);
      txtPtr = (char*)AMemLock(hText);
      CopyMemory((char *)txtPtr + txtSize, objPtr, objSize);
      txtSize += objSize;
      AMemUnlock(hObject);
      AMemUnlock(hText);
      }

    // do the actual setting
    BOOL  wasSelected = row->IsSelected(index);
    row->DeleteRow(index);
    row->InsertData(index, 1, txtPtr, (UINT)txtSize, &zero);
    if(wasSelected)
      row->ToggleSelection(index);

    AMemFree(hObject);
    AMemFree(hText);
    }
  else
    row->AddRow(index, text, COLUMN::NumColumns(), TRUE);

  PostObjectChanged(CHANGED_NUMROWS);
  }

// TABLE::SetSelection /////////////////////////////////////////////////////

void TABLE::tSetSelection(
  long      start,
  AOBJECTID  stopObject,
  long      stop)

  {
  if(COLUMN::NumColumns()<=0)
    return;

  DestroyAllEditControls();

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  long          tmp = 0L;

  start--;
  if(start<0 || start>=row->NumRows())
    return;

  if(row->NumSelections()==0L)
    TABLE_ATTRIBUTES::SetActiveRow(start);

  if(TABLE_ATTRIBUTES::MultiSelect())
    {
    if(!row->IsSelected(start))
      {
      AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
      row->ToggleSelection(start);
      }
    }
  else
    {
    if(!row->IsSelected(start))
      {
      AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
      row->SetSelection(1, &start);
      TABLE_ATTRIBUTES::SetActiveRow(start);
      }
    }

  if(stopObject && TABLE_ATTRIBUTES::MultiSelect())
    {
    AEvtPostSignalAtMark(theObject, SIGNAL_SELECTIONCHANGED);
    stop--;
    if((stop - start) > 16000L)
      stop = start + 15999L;
    else if((start - stop) > 16000L)
      stop = start - 15999L;

    if(stop<0L)
      stop = 0L;
    else if(stop>=row->NumRows())
      stop = row->NumRows() - 1L;

    if(start>stop)
      {
      start--;
      while((start>=stop) && (row->NumSelections()<16000))
        {
        if(!row->IsSelected(start))
          row->ToggleSelection(start);

        start--;
        }
      }
    else if(start<stop)
      {
      start++;
      while((start<=stop) && (row->NumSelections()<16000))
        {
        if(!row->IsSelected(start))
          row->ToggleSelection(start);

        start++;
        }
      }
    }

  if(hEdit)
    {
    if((row->NumSelections()>1) || !row->IsSelected(editRow))
      tmp = CHANGED_KILLEDIT;
    }

  PostObjectChanged(CHANGED_FULLREDRAW | tmp);
  }

// TABLE::SortRow //////////////////////////////////////////////////////////

long TABLE::SortRow(
  AOBJECTID  columnObject,
  AOBJECTID  indexObject,
  long      index,
  int       sortType)

  {
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  long          ret = -1L;

  if(row->NumRows()<1)
    return -1L;
  else if(row->NumRows()==1)
    return 0L;

  DestroyAllEditControls();

  if(!indexObject)
    index = row->FirstSelection();
  else
    index--;

  if(index<0 || index>=row->NumRows() || COLUMN::NumColumns()<=0)
    return -1L;

  int colIndex = ColumnIndex(columnObject);
  if(colIndex>=COLUMN::NumColumns() || colIndex<0)
    colIndex = TABLE_ATTRIBUTES::FocusColumn();

  ATYPEID      theType = AObjGetType(COLUMN::GetObjectID(colIndex));
  AOBJECTID    obj1 = AObjCreate(theType);
  AOBJECTID    obj2 = AObjCreate(theType);
  AMEMBLOCKID  hText1 = AMemAllocate(255L);
  AMEMBLOCKID  hText2 = AMemAllocate(255L);

  for(long j=0L; j<row->NumRows(); j++)
    {
    if(j==index)
      continue;

    if(sortType==SORT_ASCENDING)
      {
      if(CellCompare(row, colIndex, theType, NULL, index, obj1, hText1, j, obj2, hText2) <= 0)
        {
        row->MoveRow(j, index);
        ret = j;
        break;
        }
      }
    else
      {
      if(CellCompare(row, colIndex, theType, NULL, index, obj1, hText1, j, obj2, hText2) >= 0)
        {
        row->MoveRow(j, index);
        ret = j;
        break;
        }
      }
    }
  AObjDestroy(obj1);
  AObjDestroy(obj2);
  AMemFree(hText1);
  AMemFree(hText2);

  if(j==row->NumRows())
    {
    row->MoveRow(j, index);
    ret = j;
    }

  if(ret>index)
    ret--;

  if(ret<0L)
    ret = index;

  PostObjectChanged(CHANGED_FULLREDRAW | CHANGED_KILLEDIT);
  return ret;
  }

// TABLE::SortTable ////////////////////////////////////////////////////////

void TABLE::SortTable(
  AOBJECTID  columnObject,
  int       sortType)

  {
  if(COLUMN::NumColumns()<=0)
    return;

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();

  DestroyAllEditControls();

  int colIndex = ColumnIndex(columnObject);
  if(colIndex>=COLUMN::NumColumns() || colIndex<0)
    colIndex = TABLE_ATTRIBUTES::FocusColumn();

  if(row->NumRows()>1)
    {
    ATYPEID      theType = COLUMN::GetObjectID(colIndex) ? AObjGetType(COLUMN::GetObjectID(colIndex)) : OTYPE_TEXT;
    AOBJECTID    obj1 = AObjCreate(theType);
    AOBJECTID    obj2 = AObjCreate(theType);
    AMEMBLOCKID  hText1 = AMemAllocate(255L);
    AMEMBLOCKID  hText2 = AMemAllocate(255L);
    HGLOBAL     hKeys = NULL;
    void*       keys = NULL;
    long        lKeySize;
    if((theType==OTYPE_NUMBER)
        ||
       (theType==OTYPE_DATE)
        ||
       (theType==OTYPE_TIME))
      {
      lKeySize = (theType==OTYPE_NUMBER) ? sizeof(long double) : sizeof(long);
      hKeys = GlobalAlloc(GMEM_MOVEABLE, lKeySize*row->NumRows());
      keys = GlobalLock(hKeys);

      char* text1 = (char*)AMemLock(hText1);
      for(long i=0; i<row->NumRows(); i++)
        {
        CopyMemory(text1, row->GetCell(i, colIndex, FALSE, NULL), min(255, lstrlen(row->GetCell(i, colIndex, FALSE, NULL))+1));
        text1[254] = 0;

        AObjConvertFromText(obj1, hText1);

        if(theType==OTYPE_NUMBER)
          {
          long double dVal = (long double)Tbl_GetNmbrObj(obj1);
          *((long double *)keys+i) = dVal;
          }
        else if(theType==OTYPE_DATE)
          *((long *)keys+i) = (long)ODateGetValue(obj1);
        else // theType == OTYPE_TIME
          *((long *)keys+i) = (long)OTimeGetValue(obj1);
        }
      AMemUnlock(hText1);
      }

    HGLOBAL     hRows = GlobalAlloc(GMEM_MOVEABLE, sizeof(long)*row->NumRows());
    long *  rows = (long *)GlobalLock(hRows);

    for(long index=0L; index<row->NumRows(); index++)
      *(rows+index) = index;

    // shell sort implementation
    //----------------------------------------------------------------------
    // The sort is based on Robert Sedgewick "Algorithms", p 97-99.  See
    // also Knuth, Vol. 2, Section 5.2.1, pp. 84-95.
    //----------------------------------------------------------------------
    // set the first span for the shellsort using these rules:
    //
    //   [1] Let h(1) = 1, h(s+1) = 3 * h(s) + 1 [ie: 1,4,13,40,121...]
    //   [2] Set span = h(t) where h(t+2) >= numNodes
    //
    // (per Knuth, Vol 2, Sec 5.2.1, item (8), p 95)
    //----------------------------------------------------------------------
    long  span = 1;
    long  upperRung;
    long  lowerRung;
    long  numNodes = row->NumRows();
    long  temp;
    int   ret;

    if (numNodes > 13)
      {
      while (span < numNodes)
        span = (3 * span) + 1;
      span = span / 9;
      }

    // -- shellsort it
    if(sortType==SORT_ASCENDING)
      {
      for ( ; span > 0 ; span /= 3)
        {
        for (upperRung = span ; upperRung < numNodes ; upperRung++)
          {
          temp = *(rows+upperRung);

          lowerRung = upperRung - span;
          while (lowerRung >= 0
                   &&
                 (ret=CellCompare(row, colIndex, theType, keys, *(rows+lowerRung), obj1, hText1, temp, obj2, hText2)) >= 0)
            {
            if(ret!=0 || (*(rows+lowerRung) > temp))
              {
              *(rows+(lowerRung+span)) = *(rows+lowerRung);
              lowerRung -= span;
              }
            else
              break;
            }

          *(rows+(lowerRung+span)) = temp;
          }
        }
      }
    else
      {
      for ( ; span > 0 ; span /= 3)
        {
        for (upperRung = span ; upperRung < numNodes ; upperRung++)
          {
          temp = *(rows+upperRung);

          lowerRung = upperRung - span;
          while (lowerRung >= 0
                   &&
                 (ret=CellCompare(row, colIndex, theType, keys, *(rows+lowerRung), obj1, hText1,temp, obj2, hText2)) <= 0)
            {
            if(ret!=0 || (*(rows+lowerRung) > temp))
              {
              *(rows+(lowerRung+span)) = *(rows+lowerRung);
              lowerRung -= span;
              }
            else
              break;
            }

          *(rows+(lowerRung+span)) = temp;
          }
        }
      }

    long  tmp = 0L;
    if(row->ReorderRows(rows))
      tmp = CHANGED_KILLEDIT;

    if(hKeys)
      {
      GlobalUnlock(hKeys);
      GlobalFree(hKeys);
      }

    GlobalUnlock(hRows);
    GlobalFree(hRows);
  
    AObjDestroy(obj1);
    AObjDestroy(obj2);
    AMemFree(hText1);
    AMemFree(hText2);

    PostObjectChanged(CHANGED_FULLREDRAW | tmp);
    }
  }

// TABLE::CellCompare //////////////////////////////////////////////////////

int TABLE::CellCompare(
  ROW*        row,
  int         column,
  ATYPEID      theType,
  void*       keys,
  long        row1,
  AOBJECTID    obj1,
  AMEMBLOCKID  hText1,
  long        row2,
  AOBJECTID    obj2,
  AMEMBLOCKID  hText2
  )

  {
  long double dTo1;
  long double dTo2;

  if(keys)
    {
    if(theType==OTYPE_NUMBER)
      {
      long double * nums = (long double *)keys;
      dTo1 = Tbl_GetNmbrObj(obj1);
      dTo2 = Tbl_GetNmbrObj(obj2);

      return (*(nums+row1)==*(nums+row2)) ? 0 : ((*(nums+row1)>*(nums+row2)) ? 1 : -1);
      }
    else if(theType==OTYPE_DATE || theType==OTYPE_TIME)
      {
      long *  nums = (long *)keys;
      if (theType == OTYPE_DATE)
	{
	dTo1 = ODateGetValue(obj1);
	dTo2 = ODateGetValue(obj2);
	}
      else // TIME
	{
	dTo1 = OTimeGetValue(obj1);
	dTo2 = OTimeGetValue(obj2);
	}
      return (*(nums+row1)==*(nums+row2)) ? 0 : ((*(nums+row1)>*(nums+row2)) ? 1 : -1);
      }
    }

  if(AEvtCheckForMethod(theType, ODATB_EVT_SENDKEY))
    {
    ODatbKey  Key = {ODATB_KEYQUERY, {0}}; // KeyStruct
    AEvtCallMethod(theObject, COLUMN::GetObjectID(column), 0, ODATB_EVT_SENDKEY, (long)&Key);

    if(Key.type & ODATB_KEYTEXT)
      return  stricmp(row->GetCell(row1, column, FALSE, NULL), row->GetCell(row2, column, FALSE, NULL));

    int       ret;
    char*     text1 = (char*)AMemLock(hText1);
    char*     text2 = (char*)AMemLock(hText2);
    ODatbKey  Key1 = {Key.type, {0}};
    ODatbKey  Key2 = {Key.type, {0}};
    CopyMemory(text1, row->GetCell(row1, column, FALSE, NULL), min(255, lstrlen(row->GetCell(row1, column, FALSE, NULL))+1));
    CopyMemory(text2, row->GetCell(row2, column, FALSE, NULL), min(255, lstrlen(row->GetCell(row2, column, FALSE, NULL))+1));
    text1[254]=0;
    text2[254]=0;

    switch(Key.type)
      {
      case ODATB_KEYLONG:
        {
	AObjConvertFromText(obj1, hText1);
        AObjConvertFromText(obj2, hText2);
        if(theType!=OTYPE_NUMBER)
          {
          AEvtCallMethod(theObject, obj1, 0, ODATB_EVT_GETKEY, (long)&Key1);
          AEvtCallMethod(theObject, obj2, 0, ODATB_EVT_GETKEY, (long)&Key2);
          long  diff = *((long*)&Key1.key) - *((long*)&Key2.key);
          ret = (diff==0) ? 0 : ((diff>0) ? 1 : -1);
          }
        else
          ret = ((long)dTo1== (long)dTo2) ? 0 : (((long)dTo1>(long)dTo2) ? 1 : -1);
        break;
        }

      case (ODATB_KEYLONG | ODATB_KEYLONGDOUBLE):
      case ODATB_KEYLONGDOUBLE:
        {
        AObjConvertFromText(obj1, hText1);
        AObjConvertFromText(obj2, hText2);
	if(theType==OTYPE_NUMBER)
          {
          long double val1, val2;
          val1 = dTo1;
          val2 = dTo2;
          ret = (val1==val2) ? 0 : ((val1>val2) ? 1 : -1);
          }
        else
          {
          Key1.type = ODATB_KEYLONGDOUBLE;
          Key2.type  = ODATB_KEYLONGDOUBLE;
          AEvtCallMethod(theObject, obj1, 0, ODATB_EVT_GETKEY, (long)&Key1);
          AEvtCallMethod(theObject, obj2, 0, ODATB_EVT_GETKEY, (long)&Key2);
          long double diff = *((long double*)&Key1.key) - *((long double*)&Key2.key);
          ret = (diff==0.) ? 0 : ((diff>0) ? 1 : -1);
          }
        break;
        }

      case ODATB_KEYDATE:
        {
        AObjConvertFromText(obj1, hText1);
        AObjConvertFromText(obj2, hText2);
        if(theType==OTYPE_DATE)
          ret = ((long)dTo1 == (long)dTo2) ? 0: (((long)dTo1 > (long)dTo2) ? 1 : -1);
        else
          {
          Key1.type = ODATB_KEYDATE;
          Key2.type = ODATB_KEYDATE;
          AEvtCallMethod(theObject, obj1, 0, ODATB_EVT_GETKEY, (long)&Key1);
          AEvtCallMethod(theObject, obj2, 0, ODATB_EVT_GETKEY, (long)&Key2);
          long  diff = *((long*)&Key1.key) - *((long*)&Key2.key);
          ret = (diff==0) ? 0 : ((diff>0) ? 1 : -1);
          }
        break;
        }

      case ODATB_KEYTIME:
        {
        AObjConvertFromText(obj1, hText1);
        AObjConvertFromText(obj2, hText2);
        if(theType==OTYPE_TIME)
          ret = ((long)dTo1== (long)dTo2) ? 0 : ((dTo1>dTo2) ? 1 : -1);
        else
          {
          AEvtCallMethod(theObject, obj1, 0, ODATB_EVT_GETKEY, (long)&Key1);
          AEvtCallMethod(theObject, obj2, 0, ODATB_EVT_GETKEY, (long)&Key2);
          long  diff = *((long*)&Key1.key) - *((long*)&Key2.key);
          ret = (diff==0.) ? 0 : ((diff>0) ? 1 : -1);
          }
        break;
        }

      default :
        ret = stricmp(row->GetCell(row1, column, FALSE, NULL), row->GetCell(row2, column, FALSE, NULL));
      }
    AMemUnlock(hText1);
    AMemUnlock(hText2);
    return ret;
    }
  else
    return stricmp(row->GetCell(row1, column, FALSE, NULL), row->GetCell(row2, column, FALSE, NULL));
  }

// TABLE::UpdateAutoTable //////////////////////////////////////////////////

BOOL TABLE::UpdateAutoTable(
  long  numRows)

  {
  if(COLUMN::NumColumns()<=0)
    return FALSE;

  if(!TABLE_ATTRIBUTES::DataSource() && numRows<0)
    return FALSE;

  if(TABLE_ATTRIBUTES::DataSource())
    {
    // get the row count from the data source object....
    long  num = -1L;
    if (AdHoc() == FALSE)
      {
      AEvtCallMethod(theObject, TABLE_ATTRIBUTES::DataSource(), NULL, autoTableRecordCount, (long)&num);
      }
    else
      {
      // AD Hoc table
      // get the row count from the data source object....
      AEvtCallMethod(theObject, TABLE_ATTRIBUTES::DataSource(), NULL, AEVENT_DVIEWGETROWCOUNT, (long)&num);
      }
    if(num<0L)
      num = 0L;
    TABLE_ATTRIBUTES::SetTotalNumRows(num);
    }
  else
    TABLE_ATTRIBUTES::SetTotalNumRows(numRows);

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  row->ClearSelections();
  row->ClearLoadedRows();

  TABLE_ATTRIBUTES::SetActiveColumn(-1);
  TABLE_ATTRIBUTES::SetActiveRow(-1);

  PostObjectChanged(CHANGED_AUTOUPDATE);
  return TRUE;
  }

// TABLE::AutoUpdate ///////////////////////////////////////////////////////

void TABLE::AutoUpdate()

  {
  firstRow = 0L;
  NumRowsChanged();
  }

// TABLE::ValidIndexedField ////////////////////////////////////////////////

BOOL TABLE::ValidIndexedField(
  AOBJECTID  idxObj)

  {
  if(!idxObj)
    return TRUE;
  else if(TABLE_ATTRIBUTES::DataSource())
    {
    AMEMBLOCKID  theBlock = AMemAllocate(0L);
    AEvtCallMethod(theObject, TABLE_ATTRIBUTES::DataSource(), NULL, autoTableGetIndexableFields, (long)theBlock);
    int         count = (int)AMemGetSize(theBlock) / sizeof(AOBJECTID);
    AOBJECTID*   objs = (AOBJECTID*)AMemLock(theBlock);
    for(int i=0; i<count; i++)
      {
      if(idxObj==objs[i])
        {
        AMemUnlock(theBlock);
        return TRUE;
        }
      }
    AMemUnlock(theBlock);
    return FALSE; 
    }
  else
    return FALSE;
  }

// TABLE::NumRowsChanged ///////////////////////////////////////////////////

void TABLE::NumRowsChanged()

  {
  if(hEdit) // close the edit control without changing the value
    EscKey();

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();

  if(hVertScroll && (numVisRows < row->NumRows()))
    {
    SetScrollRange(hVertScroll, SB_CTL, 0, NumRowsToScrollRange(row->NumRows()), FALSE);
    SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
    EnableWindow(hVertScroll, TRUE);

    if(firstRow>(row->NumRows()-numVisRows))
      firstRow = row->NumRows() - numVisRows;
    }
  else if(hVertScroll)
    {
    firstRow = 0L;
    EnableWindow(hVertScroll, FALSE);
    }

  InvalidateTableRect();
  }

// TABLE::HScroll //////////////////////////////////////////////////////////

void TABLE::HScroll(
  WPARAM  wParam,
  LPARAM  lParam)

  {
  if(hEdit)
    DestroyEditControl();

  int pos = GetScrollPos(hHorzScroll, SB_CTL);

  switch(wParam)
    {
    case SB_TOP:
      if(firstColumn!=ScrollPosToFirstColumn(0))
        {
        firstColumn = ScrollPosToFirstColumn(0);
        RECT  tmp = tblRect;
        tmp.left += fixedSpace;

        InvalidateRect(hTable, &tmp, FALSE);
        }
      break;

    case SB_BOTTOM:
      if(firstColumn!=ScrollPosToFirstColumn(HScrollSpace))
        {
        firstColumn = ScrollPosToFirstColumn(HScrollSpace);
        RECT  tmp = tblRect;
        tmp.left += fixedSpace;

        InvalidateRect(hTable, &tmp, FALSE);
        }
      return;

    case SB_PAGEUP:
      if(pos>0)
        {
        int width = 0;
        int change = 0;
        while(width<freeSpace && pos>0)
          {
          pos--;

          if(TABLE_ATTRIBUTES::ColumnLines() && width>0)
            width++;

          width += COLUMN::ColumnWidth(ScrollPosToFirstColumn(pos));
          if(change!=0 && width>freeSpace)
            pos++;

          change++;
          }
        firstColumn = ScrollPosToFirstColumn(pos);
        RECT  tmp = tblRect;
        tmp.left += fixedSpace;

        InvalidateRect(hTable, &tmp, FALSE);
        }
      break;

    case SB_LINEUP:
      if(pos>0)
        {
        pos--;
        firstColumn = ScrollPosToFirstColumn(pos);
        RECT  tmp = tblRect;
        tmp.left += fixedSpace;

        InvalidateRect(hTable, &tmp, FALSE);
        }
      break;

    case SB_PAGEDOWN:
      if(pos<HScrollSpace)
        {
        int width = 0;
        int change = 0;
        while(width<freeSpace && pos<HScrollSpace)
          {
          if(TABLE_ATTRIBUTES::ColumnLines() && width>0)
            width++;

          width += COLUMN::ColumnWidth(ScrollPosToFirstColumn(pos));

          if(change==0 || width<freeSpace)
            pos++;
          change++;
          }
        firstColumn = ScrollPosToFirstColumn(pos);
        RECT  tmp = tblRect;
        tmp.left += fixedSpace;

        InvalidateRect(hTable, &tmp, FALSE);
        }
      break;

    case SB_LINEDOWN:
      if(pos<HScrollSpace)
        {
        pos++;
        firstColumn = ScrollPosToFirstColumn(pos);
        RECT  tmp = tblRect;
        tmp.left += fixedSpace;

        InvalidateRect(hTable, &tmp, FALSE);
        }
      break;

    case SB_THUMBPOSITION:
      {
      firstColumn = ScrollPosToFirstColumn((int)LOWORD(lParam));
      RECT  tmp = tblRect;
      tmp.left += fixedSpace;

      InvalidateRect(hTable, &tmp, FALSE);
      }
      break;

    default:
      break;
    }

  SetScrollPos(hHorzScroll, SB_CTL, FirstColumnToScrollPos(), TRUE);
  }

// TABLE::VScroll //////////////////////////////////////////////////////////

void TABLE::VScroll(
  WPARAM  wParam,
  LPARAM  lParam)

  {
  if(hEdit)
    DestroyEditControl();

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();
  switch(wParam)
    {
    case SB_TOP:
      firstRow = 0L;
      InvalidateTableRect();
      break;

    case SB_BOTTOM:
      firstRow = row->NumRows() - numVisRows;
      InvalidateTableRect();
      break;

    case SB_LINEUP:
      if(firstRow>0L)
        {
        firstRow--;
        int srcX = tblRect.left;
        int srcY = tblRect.top + (TABLE_ATTRIBUTES::ShowTitles() ? (TABLE_ATTRIBUTES::TitleHeight() + 1) : 0);
        int dstX = tblRect.left;
        int dstY = srcY + TABLE_ATTRIBUTES::CellHeight();
        int width = tblRect.right - tblRect.left;
        int height;
        if(TABLE_ATTRIBUTES::RowLines())
          {
          srcY++;
          dstY += 2;
          }
        height = tblRect.bottom - dstY;

        HDC hDC = GetDC(hTable);
        BitBlt(hDC, dstX, dstY, width, height, hDC, srcX, srcY, SRCCOPY);
        ReleaseDC(hTable, hDC);
        RECT  rect = {tblRect.left, srcY,
                      tblRect.right, srcY + TABLE_ATTRIBUTES::CellHeight() + (TABLE_ATTRIBUTES::RowLines() ? 1 : 0)};
        InvalidateRect(hTable, &rect, FALSE);
        UpdateWindow(hTable);
        }
      break;

    case SB_LINEDOWN:
      if(firstRow < (row->NumRows() - numVisRows))
        {
        firstRow++;
        int dstX = tblRect.left;
        int dstY = tblRect.top + (TABLE_ATTRIBUTES::ShowTitles() ? (TABLE_ATTRIBUTES::TitleHeight() + 1) : 0);
        int srcX = tblRect.left;
        int srcY = dstY + TABLE_ATTRIBUTES::CellHeight();
        int width = tblRect.right - tblRect.left;
        int height;
        if(TABLE_ATTRIBUTES::RowLines())
          {
          srcY += 2;
          dstY++;
          }
        height = tblRect.bottom - srcY;

        HDC hDC = GetDC(hTable);
        BitBlt(hDC, dstX, dstY, width, height, hDC, srcX, srcY, SRCCOPY);
        ReleaseDC(hTable, hDC);
        RECT  rect = {tblRect.left, tblRect.bottom - TABLE_ATTRIBUTES::CellHeight() - 1,
                      tblRect.right, tblRect.bottom};
        InvalidateRect(hTable, &rect, FALSE);
        UpdateWindow(hTable);
        }
      break;

    case SB_PAGEUP:
      if(firstRow > 0L)
        {
        firstRow -= numVisRows;
        if(firstRow < 0L)
          firstRow = 0L;
        InvalidateTableRect();
        }
      break;

    case SB_PAGEDOWN:
      if(firstRow < (row->NumRows() - numVisRows))
        {
        firstRow += numVisRows;
        if(firstRow > (row->NumRows() - numVisRows))
          firstRow = row->NumRows() - numVisRows;
        InvalidateTableRect();
        }
      break;

    case SB_THUMBPOSITION:
      firstRow = ScrollPosToFirstRow(row->NumRows(), (int)LOWORD(lParam));
      InvalidateTableRect();
      break;

    default:
      return;
    }

  SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(row->NumRows()), TRUE);
  }

// TABLE::UpdateVScrollBar /////////////////////////////////////////////////

void TABLE::UpdateVScrollBar(
  long  numRows)

  {
  if(hVertScroll)
    {
    if(numRows>numVisRows)
      {
      EnableWindow(hVertScroll, TRUE);
      SetScrollRange(hVertScroll, SB_CTL, 0, NumRowsToScrollRange(numRows), FALSE);
      SetScrollPos(hVertScroll, SB_CTL, FirstRowToScrollPos(numRows), TRUE);
      }
    else 
      {
      firstRow = 0L;
      EnableWindow(hVertScroll, FALSE);
      }
    }
  }

// TABLE::CheckFirstColumn /////////////////////////////////////////////////

void TABLE::CheckFirstColumn()

  {
  if(firstColumn>=COLUMN::NumColumns())
    firstColumn = COLUMN::NumColumns()-1;
  }

// TABLE::DestroyAllEditControls ///////////////////////////////////////////

void TABLE::DestroyAllEditControls()

  {
  OWndCallEachVisualInstance(theObject, DestroyEditControls, FALSE, 0L);
  }

// TABLE::DestroySomeEditControls ///////////////////////////////////////////

void TABLE::DestroySomeEditControls(
  long  row,
  int   colIndex)

  {
  ROWCOL  rct = {row, colIndex, this};

  OWndCallEachVisualInstance(theObject, DestroyEditControls, TRUE, (LPARAM)&rct);
  }
////////////////////////////////////////////////////////////////////////////

BOOL ALMCALLBACK TABLE::DestroyEditControls
  (
  OpWndItemD* wi,
  LPARAM      lParam1,
  LPARAM      lParam2
  )

  {
  TABLE*  table = (TABLE*)wi;
  ROWCOL* rct = (ROWCOL*)lParam2;

  if(lParam1 && rct)
    {
    if((rct->table)->hEdit && rct->row==table->editRow && rct->col==table->editCol)
      table->DestroyEditControl();
    }
  else
    table->DestroyEditControl();

  return(TRUE);  
  }

////////////////////////////////////////////////////////////////////////////
