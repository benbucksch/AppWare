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
// FILE:  "tabl_row.cpp"
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
#define   MAXSEL            16000

#include <windows.h>
#include <string.h>
#include <stdlib.h>

#include <a_alm.h>
#include <intdebug.h>
#include <op_wnd.h>
#include "op_dview.h"
#include <o_wnd.h>
#include <o_number.h>
#include "o_table.h"

#include "table.h"
#include "tabl_hdr.h"
#include "tbl_clmn.h"
#include "tabl_row.h"

///////////////////////////////////////////////////////////////////////////
// A set of oip Data Management functions that lock/unlock/set/resize datas.
// This is to provide one-point control for all the data operations for the
// following datas:
//                      1.  DATA_CELLDATA
//                      2.  DATA_ROWOFFSETS
//                      3.  DATA_SELECTIONS
///////////////////////////////////////////////////////////////////////////
//
// A runtime Class that is unique for each oid.  It holds the updated
// Data ptrs at any given time
///////////////////////////////////////////////////////////////////////////
RUNTIMEINFO::RUNTIMEINFO(AOBJECTID oid)
  {
  cellData        = NULL;
  selections      = NULL;
  rowOffsets      = NULL;
  rowObjCount     = 0;  // number of ROW class instantiated
  lpHCFont        = NULL;
  lpHTFont        = NULL;
  MemhCFont       = NULL;
  MemhTFont       = NULL;
  m_nFontUseCount = 0;
  m_row           = NULL;
  m_fParentOpen   = FALSE;
  AObjSetRuntimeInfo(oid, (LONG)this);
  m_row           = new ROW(oid);
  }

RUNTIMEINFO::~RUNTIMEINFO()
  {
  delete m_row;
  }

// A new ROW obj is being instantiated. Increment ref. count
int RUNTIMEINFO::AddUser(LPLONG lpRow)
  {
  if(!m_row)
    m_row = (ROW*)lpRow;
  else
    I_ASSERT(m_row == (ROW*)lpRow);
  rowObjCount += 1;
  I_UNUSED(lpRow);
  return rowObjCount;
  }

// Remove a ROW ref count.
int RUNTIMEINFO::RemoveUser(LPLONG lpRow)
  {
  rowObjCount -= 1;
  I_UNUSED(lpRow);
  return rowObjCount;
  }


BOOL RUNTIMEINFO::RTimeResize(AOBJECTID theObject, LONG dataIndex, LONG lNewSize)
  {
  BOOL rVal;
  switch (dataIndex)
    {
    case DATA_CELLDATA:
      if (cellData)
        {
        AObjUnlockData(theObject, dataIndex);
	cellData = NULL;
	}
      rVal = AObjResizeData(theObject, dataIndex, lNewSize);
      break;
    case DATA_ROWOFFSETS:
      if (rowOffsets)
	{
	AObjUnlockData(theObject, dataIndex);
	rowOffsets = NULL;
	}
      rVal = AObjResizeData(theObject, dataIndex, lNewSize);
      break;
    case DATA_SELECTIONS:
      if (selections)
	{
	AObjUnlockData(theObject, dataIndex);
        selections = NULL;
        }
      rVal = AObjResizeData(theObject, dataIndex, lNewSize);
      break;
    default:
      rVal = FALSE;
    }
  return rVal;
  }



///////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////

BOOL RUNTIMEINFO::RTimeSetData(
  AOBJECTID theObject,
  LONG     dataIndex,
  LPVOID   theData,
  LONG     lSize)
  {
  BOOL rVal;
  switch (dataIndex)
    {
    case DATA_CELLDATA:
      if (cellData)
	{
	AObjUnlockData(theObject, dataIndex);
	cellData = NULL;
	}
      rVal = AObjSetData(theObject, dataIndex, theData, lSize);
      break;
    case DATA_ROWOFFSETS:
      if (rowOffsets)
	{
	AObjUnlockData(theObject, dataIndex);
	rowOffsets = NULL;
	}
      rVal = AObjSetData(theObject, dataIndex, theData, lSize);
      break;
    case DATA_SELECTIONS:
      if (selections)
	{
	AObjUnlockData(theObject, dataIndex);
	selections = NULL;
	}
      rVal = AObjSetData(theObject, dataIndex, theData, lSize);
      break;
    default:
      rVal = FALSE;
    }
  return rVal;
  }


///////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////

BOOL RUNTIMEINFO::RTimeUnlock(AOBJECTID theObject, LONG dataIndex)
  {
  BOOL rVal;
  switch (dataIndex)
    {
    case DATA_CELLDATA:
      {
      if ((cellData) &&  (rowObjCount == 0))
	{
	rVal = AObjUnlockData(theObject, dataIndex);
	cellData = NULL;
        }
      else
        rVal = TRUE;
      }
      break;
    case DATA_ROWOFFSETS:
      if ((rowOffsets) &&  (rowObjCount == 0))
        {
        rVal = AObjUnlockData(theObject, dataIndex);
        rowOffsets = NULL;
        }
      else
        rVal = TRUE;
      break;
    case DATA_SELECTIONS:
      if ((selections) && (rowObjCount == 0))
        {
        rVal = AObjUnlockData(theObject, dataIndex);
	selections  = NULL;
        }
      else
        rVal = TRUE;
      break;
    default:
      rVal = FALSE;
    }
  return rVal;
  }


///////////////////////////////////////////////////////////////////
// SetHCFont :  This function will set the Column Font for the
//              index-th column
//
///////////////////////////////////////////////////////////////////

BOOL RUNTIMEINFO::SetHCFont(int index, HFONT hFont)
  {
  BOOL rVal = FALSE;
  
  if (index >= 0)
    {
    lpHCFont[index] = hFont;
    rVal = TRUE;
    }
  return rVal;

  }


///////////////////////////////////////////////////////////////////
// SetHTFont :  This function will set the Title Font for the
//              index-th column//
// 
///////////////////////////////////////////////////////////////////
BOOL  RUNTIMEINFO::SetHTFont(int index, HFONT hFont)
  {
  BOOL rVal = FALSE;
  if (index >= 0)
    {
    lpHTFont[index]= hFont;
    rVal = TRUE;
    }
  return rVal;
  }


///////////////////////////////////////////////////////////////////
// GetHCFont :  This function will Get the Column Font for the
//              index-th column//
// 
///////////////////////////////////////////////////////////////////
HFONT RUNTIMEINFO::GetHCFont(int index)
  {
  HFONT hFont = lpHCFont[index];
  return hFont;
  }


///////////////////////////////////////////////////////////////////
// GetHCFont :  This function will Get the Title Font for the
//              index-th column//
// 
///////////////////////////////////////////////////////////////////
HFONT RUNTIMEINFO::GetHTFont(int index)
  {
  HFONT hFont = lpHTFont[index];
  return hFont;
  }



  
/**********************************************************************
ROW clas implementation
***********************************************************************/
BOOL ROW::IsPtrValid(long dataIndex)
  {
  BOOL bPtrIsValid = FALSE;
  switch ((long)dataIndex)
    {
    case DATA_CELLDATA:
        if (*cellData)
          bPtrIsValid = TRUE;
        break;
    case DATA_ROWOFFSETS:
        if (*rowOffsets)
          bPtrIsValid = TRUE;
        break;
    case DATA_SELECTIONS:
        if (*selections)
          bPtrIsValid = TRUE;
        break;
    default:
	bPtrIsValid = FALSE;
        break;
    }
  return bPtrIsValid;
  }

/////////////////////////////////////////////////////////////////////
LPVOID ROW::FocalLockData(AOBJECTID theObject, long dataIndex)
  {
  LPVOID rVal;
  RUNTIMEINFO *rTime = (RUNTIMEINFO *) AObjGetRuntimeInfo(theObject);

  switch (dataIndex)
    {
    case DATA_CELLDATA:
          cellData = rTime->GetCellData(theObject);
          rVal = (LPVOID)*cellData;
        break;
    case DATA_ROWOFFSETS:
	  rowOffsets = rTime->GetRowoffsets(theObject);
	  rVal = (LPVOID)*rowOffsets;
	break;
    case DATA_SELECTIONS:
	  selections = rTime->GetSelections(theObject);
	  rVal = (LPVOID)*selections;
	break;
    default:
	rVal = NULL;
    }
  return rVal;
  }


BOOL ROW::FocalUnlockData(AOBJECTID theObject, long dataIndex)
  {
  BOOL rVal;
  RUNTIMEINFO *rTime = (RUNTIMEINFO *)AObjGetRuntimeInfo(theObject);

  switch (dataIndex)
      {
      case DATA_CELLDATA:
        ADBG_ASSERT_E(cellData, return FALSE, "Invalid CellData ptr");
        rVal = rTime->RTimeUnlock(theObject, dataIndex);
        cellData = NULL;
        break;
      case DATA_ROWOFFSETS:
        ADBG_ASSERT_E(rowOffsets, return FALSE, "Invalid RowOffsets ptr");
        rVal = rTime->RTimeUnlock(theObject, dataIndex);
        rowOffsets = NULL;

        break;
      case DATA_SELECTIONS:
        ADBG_ASSERT_E(selections, return FALSE, "Invalid Selections ptr");
        rVal = rTime->RTimeUnlock(theObject, dataIndex);
        selections = NULL;

        break;
      default:
        rVal = FALSE;
      }
  return rVal;
  }

BOOL ROW::FocalResizeData(AOBJECTID theObject, long dataIndex, long lNewSize, BOOL init)
  {
  LPVOID  lpPtr;
  BOOL    rVal;
  RUNTIMEINFO *rTime = (RUNTIMEINFO *)AObjGetRuntimeInfo(theObject);
  if ((rVal = FocalUnlockData(theObject, dataIndex)) == TRUE)
    {
    rTime->RTimeResize(theObject, dataIndex, lNewSize);
    lpPtr = FocalLockData(theObject, dataIndex);
    if (!lpPtr && (lNewSize > 0))
      rVal = FALSE;

    if(init && rVal)
      {
      switch(dataIndex)
        {
        case DATA_CELLDATA:
          dataLength = lNewSize;
          break;

        case DATA_ROWOFFSETS:
	  numRows = lNewSize / sizeof(long);
          break;

        case DATA_SELECTIONS:
          numSelections = lNewSize / sizeof(long);
          break;
        }
      }
    }
  ADBG_ASSERT_E(rVal,;, "Failed on Resize");
  return rVal;
  }

BOOL ROW::FocalSetData   (
  AOBJECTID  theObject,
  LONG      dataIndex,
  LPVOID    theData,
  LONG      lSize)
  {
  BOOL rVal;
  RUNTIMEINFO *rTime = (RUNTIMEINFO *)AObjGetRuntimeInfo(theObject);
  if ((rVal = FocalUnlockData(theObject, dataIndex)) == TRUE)
    {
    rTime->RTimeSetData(theObject, dataIndex, theData, lSize);
    if (FocalLockData(theObject, dataIndex))
      rVal = TRUE;
    else 
      rVal = FALSE;
    }
  ADBG_ASSERT_E(rVal,;, "Failed on SET");
  return rVal;
  }

////////////////////////////////////////////////////////////////////////
// Centralized Mem-Management for hRow handle
////////////////////////////////////////////////////////////////////////
LPVOID ROW::hRowMemLockData()
  {
  rowPtr = (char *)AMemLock(hRow);
  return rowPtr;
  }

BOOL ROW::hRowMemUnlockData()
  {
  BOOL rVal;
  if (rowPtr)
    {
    rVal = AMemUnlock(hRow);
    if (rVal)
      rowPtr = NULL;
    }
  else
    rVal = TRUE; // the ptr is not locked
  return rVal;
  }

BOOL ROW::hRowMemSetSize(
  long lNewSize)
  {
  BOOL  rVal = TRUE;
  if (rowPtr)
    rVal = AMemUnlock(hRow);
  if (rVal)
    AMemSetSize(hRow, lNewSize);
  rowPtr = (char *) AMemLock(hRow);
  ADBG_ASSERT_E(rVal, ;, "MEM Set Size Failed");
  return rVal;
  }


/**** End of Focal Data Management functions *******/
///////////////////////////////////////////////////////////////////////////

// function prototypes /////////////////////////////////////////////////////

int longCmp(const void* val1, const void* val2);

// Tabl_UpdateRows /////////////////////////////////////////////////////////

void Tabl_UpdateRows(
  AOBJECTID  theObject,
  int       oldVersion)
  {
  switch(oldVersion)
    {
    case 0:
      {
      COLUMN        col(theObject);
      RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
      rtime->GetRow()->UpdateObject(col.NumColumns());
      break;
      }

    default:
      break;
    }
  }

// ROW::ROW -- constructor /////////////////////////////////////////////////

ROW::ROW(
  AOBJECTID  objID)
  : TABLE_ATTRIBUTES(objID),
    m_lRecID(-1)
  {
  theObject =   objID;
  // SKJ, for every new instance of ROW, increment count
  // Set the data pointers to NULL only when no more ROW instances
  RUNTIMEINFO *rTime = (RUNTIMEINFO *)AObjGetRuntimeInfo(objID);
  rTime->AddUser((LPLONG)this);

  // SKJ, OCT 15, Keep these datas Locked until need to resize
  FocalLockData(theObject, DATA_CELLDATA);
  FocalLockData(theObject, DATA_ROWOFFSETS);
  FocalLockData(theObject, DATA_SELECTIONS);

  dataLength =  AObjGetDataSize(theObject, DATA_CELLDATA);
  numRows =     AObjGetDataSize(theObject, DATA_ROWOFFSETS) / sizeof(long);
  numSelections = AObjGetDataSize(theObject, DATA_SELECTIONS) / sizeof(long);

  // make sure the selections are in range!
  if(!TABLE_ATTRIBUTES::AutoFill())
    {
    while(numSelections && (FirstSelection() > numRows))
      {
      for(long j=0; j<numSelections; j++)
	*(*selections+j) = *(*selections+(j+1));
      FocalResizeData(theObject, DATA_SELECTIONS, (long)numSelections*sizeof(long));
      }

    while(numSelections && (LastSelection() > numRows))
      {
      FocalResizeData(theObject, DATA_SELECTIONS, (long)numSelections*sizeof(long));
      }
    }

  hRow = AMemAllocate(0L);
  rowPtr = NULL;

  if(numRows==0 && TABLE_ATTRIBUTES::AutoFill())
    {
    TABLE_ATTRIBUTES::SetFirstRow(0L);
    TABLE_ATTRIBUTES::SetNumLoadedRows(0L);
    }
  }

ROW::~ROW()
  {
  AMemUnlock(hRow);
  AMemFree(hRow);
  // SKJ, for every new instance of ROW, increment count
  // Set the data pointers to NULL only when no more ROW instances
  RUNTIMEINFO *rTime = (RUNTIMEINFO *)AObjGetRuntimeInfo(theObject);
  rTime->RemoveUser((LPLONG)this);
  FocalUnlockData(theObject, DATA_CELLDATA);
  FocalUnlockData(theObject, DATA_ROWOFFSETS);
  FocalUnlockData(theObject, DATA_SELECTIONS);
  }

// ROW::NumRows ////////////////////////////////////////////////////////////

long ROW::NumRows()

  {
  if(TABLE_ATTRIBUTES::AutoFill())
    return TABLE_ATTRIBUTES::TotalNumRows();
  else
    return numRows;
  }

// ROW::InsertData /////////////////////////////////////////////////////////

void ROW::InsertData(
  long  index,
  int   addNumRows,
  HPSTR newData,
  UINT  size,
  long* offsets)
  {
  //  // Free Cell, RowOffset and Selction ptr

  if(index<0)
    return;

  if(index>numRows)
    return;
  // update the *selections
  UpdateSelections(index, addNumRows);

  // make the row offsets copy
  HGLOBAL hOffsets;
  long*   rows;
  if(numRows!=index)
    {
    hOffsets = GlobalAlloc(GMEM_MOVEABLE, sizeof(long)*(numRows-index));
    if(!hOffsets)
      return;

    rows = (long*)GlobalLock(hOffsets);
    if(!rows)
      {
      GlobalFree(hOffsets);
      return;
      }

    CopyMemory(rows, *rowOffsets+index, sizeof(long)*(numRows-index));
    }

  if(!FocalResizeData(theObject, DATA_ROWOFFSETS, (numRows+addNumRows)*sizeof(long)))
    {
    if(numRows!=index)
      GlobalFree(hOffsets);

    return;
    }

  if(numRows==index)
    *(*rowOffsets+index) = dataLength;

  for(int i=1; i<addNumRows; i++)
    *(*rowOffsets+(index+i)) = *(*rowOffsets+index) + offsets[i];

  if(numRows!=index)
    {
    CopyMemory((*rowOffsets+(index+i)), rows, sizeof(long)*(numRows-index));
    GlobalUnlock(hOffsets);
    GlobalFree(hOffsets);
    for(long j=(index+addNumRows); j<(numRows+addNumRows); j++)
      *(*rowOffsets+j) += size;
    }

  // make the row data copy
  HGLOBAL     hData;
  char *  data;
  if(numRows!=index)
    {
    hData = GlobalAlloc(GMEM_MOVEABLE, dataLength-*(*rowOffsets+index));
    if(!hData)
      return;

    data = (char *)GlobalLock(hData);
    if(!data)
      {
      GlobalFree(hData);
      return;
      }
    CopyMemory(data, *cellData+*(*rowOffsets+index), dataLength-*(*rowOffsets+index));
    }

  BOOL    fSuccess;
  if(FocalResizeData(theObject, DATA_CELLDATA, dataLength+size))
    {
    CopyMemory(*cellData+*(*rowOffsets+index), newData, size);
    fSuccess = TRUE;
    }
  else
    fSuccess = FALSE;

  if(numRows!=index)
    {
    CopyMemory(*cellData+(*(*rowOffsets+index)+size), data, dataLength-*(*rowOffsets+index));
    GlobalUnlock(hData);
    GlobalFree(hData);
    }

  if(fSuccess)
    {
    numRows += addNumRows;
    dataLength += size;
    }
  }

// ROW::AddRow /////////////////////////////////////////////////////////////

void ROW::AddRow(
  long  index,
  char* text,
  int   numColumns,
  BOOL  setRows)
  {
  numColumns = TABLE_ATTRIBUTES::NumColumns();
  UINT        used = 0;
  HGLOBAL     hTemp = GlobalAlloc(GMEM_MOVEABLE, 0x0000FFFFL);
  char*       copy = (char*)GlobalLock(hTemp);
  HGLOBAL     hOff = GlobalAlloc(GMEM_MOVEABLE, sizeof(long)*8);
  long*       offsets = (long*)GlobalLock(hOff);
  int i=0;
  int rows = 1;
  int column = 0;
  offsets[0]=0;
  while((text[i]!=0) && (used<(0xFFF0-numColumns+column)))
    {
    if((text[i]!='\t') && (text[i]!='\r') && (column<numColumns))
      copy[used++] = text[i];
    else if((text[i]=='\t') && (column<numColumns))
      {
      copy[used++] = 0;
      column++;
      }
    else if((text[i]=='\r') && (text[i+1]=='\n'))
      {
      while(column<numColumns)
        {
        // put NULL strings for the remaining columns
        copy[used++]=0;
        column++;
        }
      if(rows%8 == 0)
        {
        GlobalUnlock(hOff);
        hOff = GlobalReAlloc(hOff, (rows+8)*sizeof(long), GMEM_MOVEABLE);
        offsets = (long*)GlobalLock(hOff);
	}
      offsets[rows++] = used;
      column=0;
      i++;
      }

    i++;
    }

  while(column<numColumns)
    {
    // put NULL strings for the remaining columns
    copy[used++]=0;
    column++;
    }

  long* sel;
  if(setRows)
    {
    // get a selection list & delete existing rows
    sel = new long[rows];
    for(int j=0; j<rows && index<numRows; j++)
      sel[j] = FALSE;
    for(j=0; j<rows && index<numRows; j++)
      {
      sel[j] = IsSelected(index);
      DeleteRow(index);
      }
    }

  InsertData(index, rows, copy, used, offsets);

  if(setRows)
    {
    // restore the *selections ...
    for(int j=0; j<rows && (j+index)<numRows; j++)
      {
      if(sel[j])
	ToggleSelection(j+index);
      }
    delete sel;
    }

  GlobalUnlock(hTemp);
  GlobalFree(hTemp);
  GlobalUnlock(hOff);
  GlobalFree(hOff);
  }

// ROW::DeleteSelectedRows /////////////////////////////////////////////////

void ROW::DeleteSelectedRows()
  {

  for(long i=(numSelections-1L); i>=0; i--)
    DeleteRow(*(*selections+i));

  }

// ROW::DeleteRow //////////////////////////////////////////////////////////

void ROW::DeleteRow(
  long  row)
  {
  if(row<0 || row>=numRows)
    return;

  if(IsSelected(row))
    ToggleSelection(row); // unselect the row....


  // shift the *selections as necessary
  for(int i=0; i<numSelections; i++)
    {
    if(*(*selections+i) > row)
      *(*selections+i) -= 1;
    }


  if(row==(numRows-1))
    {
    numRows--;

    FocalResizeData(theObject, DATA_CELLDATA, *(*rowOffsets+numRows));
    dataLength = *(*rowOffsets+numRows);

    FocalResizeData(theObject, DATA_ROWOFFSETS, numRows*sizeof(long));
    }
  else
    {


    long  cutSize = *(*rowOffsets+(row+1)) - *(*rowOffsets+row);
    CopyMemory(*cellData+*(*rowOffsets+row), *cellData+*(*rowOffsets+(row+1)), dataLength - *(*rowOffsets+(row+1)));
    CopyMemory(*rowOffsets+row, *rowOffsets+(row+1), (numRows - (row+1))*sizeof(long));
    numRows--;
    dataLength -= cutSize;
    for(long i=row; i<numRows; i++)
      *(*rowOffsets+i) -= cutSize;



    FocalResizeData(theObject, DATA_CELLDATA, dataLength);
    FocalResizeData(theObject, DATA_ROWOFFSETS, numRows*sizeof(long));
    }
  }

// ROW::GetCell ////////////////////////////////////////////////////////////

HPSTR ROW::GetCell(
  long  row,
  int   col,
  BOOL  load,
  BOOL* dbAccessed)

  {
  HPSTR rVal;
  if(row<0 || row>=NumRows())
    return "";
  if(TABLE_ATTRIBUTES::AutoFill())
    {
    GetAutoRow(row, load, dbAccessed);
    if(row>=TABLE_ATTRIBUTES::FirstRow() && row<(TABLE_ATTRIBUTES::FirstRow()+TABLE_ATTRIBUTES::NumLoadedRows()))
      {
      long        rowOff  = row-TABLE_ATTRIBUTES::FirstRow();
      char * temp    = *cellData + *(*rowOffsets+rowOff);

      for(int i=0; i<col; i++)
	temp += lstrlen(temp)+1;

      rVal = temp;
      }
    else
      {
      long  offset=0L;
      int   i=0;
      while(i<col)
	{
	offset += (lstrlen(rowPtr+offset)+1L);
	i++;
	}

      rVal= (HPSTR)(rowPtr+offset);
      }
    }
  else
    {


    char * temp = *cellData + *(*rowOffsets+row);
    for(int i=0; i<col; i++)
      temp += lstrlen(temp)+1;

    rVal =  temp;

    }
  //
  //
  //
  return (HPSTR)rVal;
  }

// ROW::SetCell ////////////////////////////////////////////////////////////

void ROW::SetCell(
  long  row,
  int   col,
  char* text)
  {
  if(row<0 || row>=numRows)
    return;

   // Init RowOffsets

  long  oldRowLength = (row==(numRows-1)) ? dataLength - *(*rowOffsets+row) : *(*rowOffsets+(row+1)) - *(*rowOffsets+row);
   // SKJ OCt. 15, 1993

  int   oldCellLength = (int)lstrlen(GetCell(row, col, FALSE, NULL)) + 1;
  int   newCellLength = (int)lstrlen(text)+1;
  long  newRowLength = oldRowLength-oldCellLength+newCellLength;

  if(oldCellLength==newCellLength)
    {
    CopyMemory(GetCell(row, col, FALSE, NULL), text, newCellLength);
    return;
    }

  HGLOBAL     hRow = GlobalAlloc(GMEM_MOVEABLE, newRowLength);
  char *  newRow = (char *)GlobalLock(hRow);
  long        offset = 0;
  UINT        length = 0;
  int         i;

   // SKJ Oct. 15, 1993

  char *  temp = *cellData + *(*rowOffsets+row);
  for(i=0; i<col; i++)
    length += (int)(lstrlen(temp+length)+1);
  CopyMemory(newRow, *cellData + *(*rowOffsets+row), length);

  offset += length;
  CopyMemory(newRow + offset, text, newCellLength);
  offset += newCellLength;
  length += (int)(lstrlen(temp+length)+1);
  CopyMemory(newRow + offset, temp + length, oldRowLength - length);



  long  zero = 0L;
  BOOL  wasSelected = IsSelected(row);
  DeleteRow(row);
  InsertData(row, 1, (char*)newRow, (UINT)newRowLength, &zero);
  if(wasSelected)
    ToggleSelection(row);

  GlobalUnlock(hRow);
  GlobalFree(hRow);

  }

// ROW::UpdateSelections ///////////////////////////////////////////////////

void ROW::UpdateSelections(
  long  firstNewRow,
  int   numRows)

  {

  for(long i=0; i<numSelections; i++)
    {
    if(*(*selections+i) >= firstNewRow)
      *(*selections+i) += numRows;
    }

  }

// ROW::FirstSelection /////////////////////////////////////////////////////

long ROW::FirstSelection()

  {
  long lVal;
  if(numSelections < 1)
    return NumRows();

  lVal = **selections;

  return lVal;
  }

// ROW::LastSelection //////////////////////////////////////////////////////

long ROW::LastSelection()

  {
  long lVal;
  if(numSelections < 1)
    return NumRows();


  lVal = *(*selections+(numSelections-1));

  return lVal;
  }

// ROW::ClearSelections ////////////////////////////////////////////////////

void ROW::ClearSelections()

  {
  
  if(numSelections > 0)
    {
    FocalResizeData(theObject, DATA_SELECTIONS, 0L);
    numSelections = 0;
    }
  }

// ROW::IsSelected /////////////////////////////////////////////////////////

BOOL ROW::IsSelected(
  long  row)

  {
  BOOL bVal = FALSE;

  for(long i=0; i<numSelections; i++)
    {
    if(*(*selections+i)==row)
      bVal = TRUE;
    }

  return bVal;
  }

// ROW::GetSelection ///////////////////////////////////////////////////////

long ROW::GetSelection(
  long index)

  {
  long lVal;

  if(index>=0 && index<numSelections)
    lVal = *(*selections+index);
  else
    lVal = -1L;


  return lVal;
  }


// ROW::SetSelection ///////////////////////////////////////////////////////

void ROW::SetSelection(
  int   num,
  long* sel)

  {

  FocalResizeData(theObject, DATA_SELECTIONS, (long)num*sizeof(long));

  CopyMemory(*selections, sel, (long)num*sizeof(long));
  numSelections = num;

  }

// ROW::RangeSelection /////////////////////////////////////////////////////

void ROW::RangeSelection(
  long  anchor,
  long  sel)

  {
  if(numSelections==0L)
    SetSelection(1, &sel);

  if(sel > anchor)
    {
    int num = (int)(sel - anchor);
    if(IsSelected(anchor))
      num++;

    if(num<=0 || num>MAXSEL)
      num = MAXSEL;   // maximum number of *selections

    long* temp = new long[num];
    if(IsSelected(anchor))
      temp[0] = anchor;

    for(int i=(IsSelected(anchor) ? 1 : 0); i<num; i++)
      temp[i] = sel - num + i + 1;
    SetSelection(num, temp);
    delete temp;
    }
  else if(sel < anchor)
    {
    int num = (int)(anchor - sel);
    if(IsSelected(anchor))
      num++;

    if(num<=0 || num>MAXSEL)
      num = MAXSEL;   // maximum number of *selections

    long* temp = new long[num];
    if(IsSelected(anchor))
      temp[num-1] = anchor;

    for(int i=0; i<(IsSelected(anchor) ? (num-1) : num); i++)
      temp[i] = sel + i;
    SetSelection(num, temp);
    delete temp;
    }
  else
    {
    if(IsSelected(anchor))
      SetSelection(1, &anchor);
    else
      ClearSelections();
    }
  }

// ROW::ToggleSelection ////////////////////////////////////////////////////

void ROW::ToggleSelection(
  long  row)

  {
  if(row>=numRows)
    return;


  for(long i=0; i<numSelections; i++)
    {
    if(*(*selections+i) == row)
      {
      numSelections--;
      for(long j=i; j<numSelections; j++)
        *(*selections+j) = *(*selections+(j+1));


      FocalResizeData(theObject, DATA_SELECTIONS, numSelections*sizeof(long));
      return;
      }
    }

  if(numSelections>=MAXSEL) // gracefully? fail if there are too many *selections
    {

    return;
    }
  numSelections++;

  FocalResizeData(theObject, DATA_SELECTIONS, numSelections*sizeof(long));

  *(*selections+(numSelections-1)) = row;

  // keep the *selections in sorted order
  qsort(*selections, (size_t)numSelections, sizeof(long), longCmp);

  }

// longCmp /////////////////////////////////////////////////////////////////

int longCmp(
  const void* val1,
  const void* val2)

  {
  return (*(long*)val1>*(long*)val2) ? 1 : ((*(long*)val1<*(long*)val2) ? -1 : 0);
  }

// ROW::MoveRow ////////////////////////////////////////////////////////////

void ROW::MoveRow(
  long  dst,
  long  src)

  {
  
  if(src == dst)
    return;

  UINT  size = (UINT)(((src==(numRows-1)) ? dataLength : *(*rowOffsets+(src+1))) - *(*rowOffsets+src));
  long  zero = 0L;

  if(src > dst)
    {
    InsertData(dst, 1, GetCell(src, 0, FALSE, NULL), size, &zero);

    if(IsSelected(src+1L))
      ToggleSelection(dst);

    if(TABLE_ATTRIBUTES::ActiveRow()==src)
      TABLE_ATTRIBUTES::SetActiveRow(dst);

    DeleteRow(src+1L);
    }
  else
    {
    InsertData(dst, 1, GetCell(src, 0, FALSE, NULL), size, &zero);

    if(IsSelected(src))
      ToggleSelection(dst);

    if(TABLE_ATTRIBUTES::ActiveRow()==src)
      TABLE_ATTRIBUTES::SetActiveRow(dst-1L);

    DeleteRow(src);
    }
  }

// ROW::SwapRows ///////////////////////////////////////////////////////////

void ROW::SwapRows(
  long  row1,
  long  row2)

  {
  if(row1 == row2)
    return;

  if(row1<row2)
    {
    MoveRow(row1, row2);
    MoveRow(row2, row1+1L);
    }
  else
    {
    MoveRow(row2, row1);
    MoveRow(row1, row2+1L);
    }
  }

// ROW::RowLength //////////////////////////////////////////////////////////

long ROW::RowLength(
  long  row,
  BOOL  load,
  BOOL* dbAccessed)

  {
  long rVal;

  if(TABLE_ATTRIBUTES::AutoFill())
    {
    GetAutoRow(row, load, dbAccessed);
    if(row>=TABLE_ATTRIBUTES::FirstRow() && row<(TABLE_ATTRIBUTES::FirstRow()+TABLE_ATTRIBUTES::NumLoadedRows()))
      {

      long  rowOff = row-TABLE_ATTRIBUTES::FirstRow();
      if(rowOff==(TABLE_ATTRIBUTES::NumLoadedRows()-1)) {
	rVal  = dataLength - *(*rowOffsets+rowOff);
	}
      else {
	rVal = *(*rowOffsets+(rowOff+1)) - *(*rowOffsets+rowOff);
	}

      }
    else
      {
      long  length = 0L;

      for(int i=0; i<TABLE_ATTRIBUTES::NumColumns(); i++)
	length += lstrlen(rowPtr+length)+1L;
      rVal = length;

      }
    }
  else
    {

    if(row==(numRows-1))
      rVal = dataLength - *(*rowOffsets+row);
    else
      rVal = *(*rowOffsets+(row+1)) - *(*rowOffsets+row);

    }
  return rVal;
  }

// ROW::GetRow /////////////////////////////////////////////////////////////

void ROW::GetRow(
  long        row,
  char     *  dst,
  long        length,
  BOOL        addTabs,
  BOOL        load,
  BOOL*       dbAccessed)

  {
  long  rowLength = RowLength(row, load, dbAccessed);
  long  i;

  while(length>rowLength)
    *(dst+--length) = 0;

  if(TABLE_ATTRIBUTES::AutoFill())
    {
    CopyMemory(dst, *cellData+*(*rowOffsets+(row-TABLE_ATTRIBUTES::FirstRow())), length);
    }
  else
    CopyMemory(dst, *cellData+*(*rowOffsets+row), length);

  if(addTabs)
    {
    for(i=0; i<length; i++)
      {
      if(*(dst+i) == 0)
        *(dst+i) = '\t';
      }
    }

  }

// ROW::ReorderRows ////////////////////////////////////////////////////////

BOOL ROW::ReorderRows(
  long *  list)

  {
  BOOL    ret = FALSE;
  HGLOBAL hTemp = GlobalAlloc(GMEM_MOVEABLE, dataLength);
  if(!hTemp)
    return ret;

  HGLOBAL hOff = GlobalAlloc(GMEM_MOVEABLE, numRows*sizeof(long));
  if(!hOff)
    {
    GlobalFree(hTemp);
    return ret;
    }

  char *  temp = (char *)GlobalLock(hTemp);
  long *  newOffsets = (long *)GlobalLock(hOff);

  for(long i=0; i<numRows; i++)
    {
    if(!ret && (*(list+i) != i))
      ret = TRUE;

    if(i==0)
      *(newOffsets+i) = 0L;
    else
      *(newOffsets+i) = *(newOffsets+(i-1)) + RowLength(*(list+(i-1)), FALSE, NULL);

    CopyMemory(temp + *(newOffsets+i), GetCell(*(list+i), 0, FALSE, NULL), RowLength(*(list+i), FALSE, NULL));
    }

  // update the object data

  CopyMemory(*rowOffsets, newOffsets, numRows*sizeof(long));
  CopyMemory(*cellData, temp, dataLength);

  // free the temporary memory
  GlobalUnlock(hTemp);
  GlobalUnlock(hOff);
  GlobalFree(hTemp);
  GlobalFree(hOff);

  return ret;
  }

// ROW::UpdateObject ///////////////////////////////////////////////////////

void ROW::UpdateObject(
  int numCols)

  {
  numCols = TABLE_ATTRIBUTES::NumColumns();
  HGLOBAL hTemp = GlobalAlloc(GMEM_MOVEABLE, dataLength);
  if(!hTemp)
    return;

  long    oldNumRows = numRows;
  long    oldDataLength = dataLength;
  HGLOBAL hOff = GlobalAlloc(GMEM_MOVEABLE, numRows*sizeof(long));
  if(!hOff)
    {
    GlobalFree(hTemp);
    return;
    }

  char *  temp = (char *)GlobalLock(hTemp);
  long *  oldOffsets = (long *)GlobalLock(hOff);

  // make local copies of old data

  CopyMemory(temp, *cellData, dataLength);
  CopyMemory(oldOffsets, *rowOffsets, numRows*sizeof(long));


  // delete old information
  FocalResizeData(theObject, DATA_CELLDATA, 0L);
  FocalResizeData(theObject, DATA_ROWOFFSETS, 0L);
  dataLength = 0L;
  numRows = 0L;

  for(long i=0; i<oldNumRows; i++)
    {
    int   length = (int)lstrlen(temp+*(oldOffsets+i)) + 1;
    if((*(oldOffsets+i)+length)>oldDataLength)
      break;

    if(i<(oldNumRows-1) && (*(oldOffsets+i)+length)>*(oldOffsets+i+1))
      {
      *(temp+(*(oldOffsets+i+1)-1)) = '\0';
      length = (int)lstrlen(temp+*(oldOffsets+i)) + 1;
      }

    char* junk = new char[length];
    CopyMemory(junk, temp+*(oldOffsets+i), length);
    AddRow(i, (char*)(temp+*(oldOffsets+i)), numCols, FALSE);
    delete junk;
    }

  // free the temporary memory
  GlobalUnlock(hTemp);
  GlobalUnlock(hOff);
  GlobalFree(hTemp);
  GlobalFree(hOff);
  }

// ROW::DeleteColumn ///////////////////////////////////////////////////////

void ROW::DeleteColumn(
  int colIndex,
  int numCols)

  {
  numCols = TABLE_ATTRIBUTES::NumColumns();
  if(numCols<=0)
    return;

  HGLOBAL hTemp = GlobalAlloc(GMEM_MOVEABLE, dataLength);
  if(!hTemp)
    return;

  HGLOBAL hOff = GlobalAlloc(GMEM_MOVEABLE, sizeof(long)*numRows);
  if(!hOff)
    {
    GlobalFree(hTemp);
    return;
    }

  char *  temp = (char *)GlobalLock(hTemp);
  long *  newOffsets = (long *)GlobalLock(hOff);
  if(!temp || !newOffsets)
    {
    GlobalUnlock(hTemp);
    GlobalUnlock(hOff);
    GlobalFree(hTemp);
    GlobalFree(hOff);
    return;
    }
  *newOffsets = 0L;

  long  len;
  long  rowLen;
  long  totalLen = 0L;
  int   col;
  for(long i=0; i<numRows; i++)
    {
    rowLen = RowLength(i, FALSE, NULL) - (lstrlen(GetCell(i, colIndex, FALSE, NULL))+1);
    if(rowLen==0L)
      break;

    if(i<(numRows-1))
      *(newOffsets+(i+1)) = *(newOffsets+i) + rowLen;

    if(colIndex==0)
      CopyMemory(temp+*(newOffsets+i), GetCell(i, 1, FALSE, NULL), rowLen);
    else if(colIndex==(numCols-1))
      CopyMemory(temp+*(newOffsets+i), GetCell(i, 0, FALSE, NULL), rowLen);
    else
      {
      len = 0L;

      for(col=0; col<colIndex; col++)
	len += (lstrlen(*cellData+(*(*rowOffsets+i)+len))+1);


      CopyMemory(temp+*(newOffsets+i), GetCell(i, 0, FALSE, NULL), len);
      CopyMemory(temp+(*(newOffsets+i)+len), GetCell(i, colIndex+1, FALSE, NULL), rowLen-len);
      }

    totalLen += rowLen;
    }

  FocalResizeData(theObject, DATA_CELLDATA, totalLen);


  CopyMemory(*cellData, temp, totalLen);
  dataLength = totalLen;
  if(totalLen==0L || numCols==1)
    {

    FocalResizeData(theObject, DATA_ROWOFFSETS, 0L);

    numRows = 0L;

    FocalResizeData(theObject, DATA_SELECTIONS, 0L);

    numSelections = 0L;
    }
  else
    CopyMemory(*rowOffsets, newOffsets, sizeof(long)*numRows);

  GlobalUnlock(hTemp);
  GlobalUnlock(hOff);
  GlobalFree(hTemp);
  GlobalFree(hOff);


  if(TABLE_ATTRIBUTES::ActiveColumn()>=colIndex)
    TABLE_ATTRIBUTES::SetActiveColumn(TABLE_ATTRIBUTES::ActiveColumn()-1);
  }

// ROW::InsertColumn ///////////////////////////////////////////////////////

void ROW::InsertColumn(
  int colIndex,
  int numCols)

  {
  numCols = TABLE_ATTRIBUTES::NumColumns()-1;
  HGLOBAL hTemp = GlobalAlloc(GMEM_MOVEABLE, dataLength+numRows);
  if(!hTemp)
    return;

  HGLOBAL hOff = GlobalAlloc(GMEM_MOVEABLE, sizeof(long)*numRows);
  if(!hOff)
    {
    GlobalFree(hTemp);
    return;
    }

  char *  temp = (char *)GlobalLock(hTemp);
  long *  newOffsets = (long *)GlobalLock(hOff);
  if(!temp || !newOffsets)
    {
    GlobalUnlock(hTemp);
    GlobalUnlock(hOff);
    GlobalFree(hTemp);
    GlobalFree(hOff);
    return;
    }
  *newOffsets = 0L;

  long  len;
  long  rowLen;
  long  totalLen = 0L;
  int   col;
  for(long i=0; i<numRows; i++)
    {
    rowLen = RowLength(i, FALSE, NULL) + 1;

    if(i<(numRows-1))
      *(newOffsets+(i+1)) = *(newOffsets+i) + rowLen;

    if(colIndex==0)
      {
      *(temp+*(newOffsets+i)) = 0;
      CopyMemory(temp+(*(newOffsets+i)+1), GetCell(i, 0, FALSE, NULL), rowLen-1);
      }
    else if(colIndex==numCols)
      {
      CopyMemory(temp+*(newOffsets+i), GetCell(i, 0, FALSE, NULL), rowLen-1);
      *(temp+(*(newOffsets+i)+rowLen-1)) = 0;
      }
    else
      {
      len = 0L;
       // SKJ, OCt. 15, 1993

      for(col=0; col<colIndex; col++)
	len += (lstrlen(*cellData+(*(*rowOffsets+i)+len))+1);

       // SKJ, OCt. 15, 1993

      CopyMemory(temp+*(newOffsets+i), GetCell(i, 0, FALSE, NULL), len);
      *(temp+(*(newOffsets+i)+len)) = 0;
      CopyMemory(temp+(*(newOffsets+i)+len+1), GetCell(i, colIndex, FALSE, NULL), rowLen-len-1);
      }

    totalLen += rowLen;
    }

  FocalResizeData(theObject, DATA_CELLDATA, totalLen);

  CopyMemory(*cellData, temp, totalLen);
  dataLength = totalLen;
  CopyMemory(*rowOffsets, newOffsets, sizeof(long)*numRows);


  GlobalUnlock(hTemp);
  GlobalUnlock(hOff);
  GlobalFree(hTemp);
  GlobalFree(hOff);

  if(TABLE_ATTRIBUTES::ActiveColumn()>=colIndex)
    TABLE_ATTRIBUTES::SetActiveColumn(TABLE_ATTRIBUTES::ActiveColumn()+1);
  }

// ROW::SetColumn //////////////////////////////////////////////////////////

void ROW::SetColumn(
  char     *  text,
  int         colIndex,
  int         numCols,
  BOOL        tabDelimited)

  {
  numCols = TABLE_ATTRIBUTES::NumColumns();

  char *  token = text;

  long  rowCount = 0L;
  if(*token)
    rowCount++;

  if(tabDelimited)
    {
    while(*token)
      {
      if(*token=='\t')
        rowCount++;

      token++;
      }
    }
  else
    {
    while(*token)
      {
      if(*token=='\r' && *(token+1)=='\n')
        {
	rowCount++;
        token+=2;
        }
      else
        token++;
      }
    }

  if(rowCount>numRows)
    { // add empty new rows
    if((numCols*(rowCount - numRows)) > 0x7FFF)
      return;

    UINT  size = (UINT)(numCols*(rowCount - numRows));
    char* rows = new char[size];
    if(!rows)
      return;
    FillMemory(rows, size, 0);
    long* off = new long[(int)(rowCount - numRows)];
    if(!off)
      {
      delete rows;
      return;
      }
    *off = 0L;
    for(int i=1; i<(int)(rowCount - numRows); i++)
      off[i] = off[i-1] + numCols;

    InsertData(numRows, (int)(rowCount - numRows), rows, size, off);
    delete rows;
    delete off;
    }

  HGLOBAL hTemp = GlobalAlloc(GMEM_MOVEABLE, dataLength+lstrlen(text));
  if(!hTemp)
    return;

  HGLOBAL hOff = GlobalAlloc(GMEM_MOVEABLE, sizeof(long)*numRows);
  if(!hOff)
    {
    GlobalFree(hTemp);
    return;
    }

  char *  temp = (char *)GlobalLock(hTemp);
  long *  newOffsets = (long *)GlobalLock(hOff);
  if(!temp || !newOffsets)
    {
    GlobalUnlock(hTemp);
    GlobalUnlock(hOff);
    GlobalFree(hTemp);
    GlobalFree(hOff);
    return;
    }
  *newOffsets = 0L;

  long  len;
  long  rowLen;
  long  totalLen = 0L;
  int   col;
  long  i=0;
  long  cellLen;
  long  tokLen;

  token = myStrtok(text, tabDelimited);
  while(i<numRows)
    {
    if(!token)
      token = "";

    tokLen = lstrlen(token) + 1L;
    cellLen = lstrlen(GetCell(i, colIndex, FALSE, NULL))+1;
    rowLen = RowLength(i, FALSE, NULL) - cellLen + tokLen;

    if(i<(numRows-1))
      *(newOffsets+(i+1)) = *(newOffsets+i) + rowLen;

    if(colIndex==0)
      {
      CopyMemory(temp+*(newOffsets+i), token, tokLen);
      CopyMemory(temp+(*(newOffsets+i)+tokLen), GetCell(i, 1, FALSE, NULL), rowLen-tokLen);
      }
    else if(colIndex==(numCols-1))
      {
      CopyMemory(temp+*(newOffsets+i), GetCell(i, 0, FALSE, NULL), rowLen-tokLen);
      CopyMemory(temp+(*(newOffsets+i)+rowLen-tokLen), token, tokLen);
      }
    else
      {
      len = 0L;

      for(col=0; col<colIndex; col++) {
        // SKJ --> if there is no previous cellData than len should be set to 0.
        // This way data can start at the beginning of the buffer. -- April 26, 1994
        long oldLen = lstrlen(*cellData+(*(*rowOffsets+i)+len));
        len += oldLen +1;

        /*if (oldLen > 0)
          len += oldLen +1;
	else
          len = 0;
        */
        }

      CopyMemory(temp+*(newOffsets+i), GetCell(i, 0, FALSE, NULL), len);
      CopyMemory(temp+(*(newOffsets+i)+len), token, tokLen);
      if (rowLen-len-tokLen > 0) // SKJ
	CopyMemory(temp+(*(newOffsets+i)+len+tokLen), GetCell(i, colIndex+1, FALSE, NULL), rowLen-len-tokLen);
      }

    token = myStrtok(NULL, tabDelimited);
    totalLen += rowLen;
    i++;
    }

  if(FocalResizeData(theObject, DATA_CELLDATA, totalLen))
    {
    CopyMemory(*cellData, temp, totalLen);
    dataLength = totalLen;
    CopyMemory(*rowOffsets, newOffsets, sizeof(long)*numRows);
    }

  GlobalUnlock(hTemp);
  GlobalUnlock(hOff);
  GlobalFree(hTemp);
  GlobalFree(hOff);
  }

// ROW::GetAutoRow /////////////////////////////////////////////////////////

void ROW::GetAutoRow(
  long  row,
  BOOL  load,
  BOOL* dbAccessed)

  {
  int i;
  if(row<0 || row>TABLE_ATTRIBUTES::TotalNumRows())
    return;

  if(row>=TABLE_ATTRIBUTES::FirstRow() && row<(TABLE_ATTRIBUTES::FirstRow()+TABLE_ATTRIBUTES::NumLoadedRows()))
    {
    // the row is loaded -- do no more....
    return;
    }

  if(dbAccessed)
    *dbAccessed = TRUE;

  COLUMN  theCol(theObject);

  if(!load)
    {
    // get a single row   
    if(TABLE_ATTRIBUTES::DataSource())
      {
      AEvtPostStandard(theObject, AEVENT_SIGNAL, SIGNAL_STARTINGAUTOFILL, AEVT_ATONCE);

      if (TABLE_ATTRIBUTES::AdHoc())
        {
        long  firstRec = row + 1;
        ODViewColData rr;
        rr.recNmbr = firstRec + 1;  // 1 based
        rr.colDataMB = AMemAllocate(sizeof(AMEMBLOCKID)* TABLE_ATTRIBUTES::NumColumns());
        rr.recCount = 1;
        AEvtCallMethod(theObject, TABLE_ATTRIBUTES::DataSource(), NULL, AEVENT_DVIEWGETDATA, (long)&rr);

        if(rr.retCode == 1)  // was able to retreive 1 record
          {
          AMEMBLOCKID  hTemp = hRow;
          m_lRecID = -1L;
          hRow = rr.colDataMB;
          hRowMemLockData(); // rowPtr = (char huge*)AMemLock(hRow);
          rr.colDataMB = hTemp;
          TABLE table(theObject);

	  for(i=0; i< TABLE_ATTRIBUTES::NumColumns(); i++)
	    table.SetColumnWidth(i,table.GetMaxWidth(i));
	  }
	else if(rr.retCode == -1)  // was able to retreive 1 record
	  {
	  ADBG_PRINT_E("Error in Adhoc Query");
	  hRowMemSetSize(TABLE_ATTRIBUTES::NumColumns());
	  memset(rowPtr, 0, TABLE_ATTRIBUTES::NumColumns());
          }
        else if(rr.retCode == -2)  // DB is disconnected
          {
          ADBG_PRINT_E("Disconnected from Data Source");
          }
        AMemFree(rr.colDataMB);
        }
      else {
        TablREADRECORDS rr;
        rr.hFields = AMemAllocate(sizeof(AOBJECTID)*TABLE_ATTRIBUTES::NumColumns());
        rr.idxObj = TABLE_ATTRIBUTES::IndexedField();
        rr.relIdx = row;
        rr.readType = SEEK_SET;
        rr.recCount = 1L;
        rr.hData = AMemAllocate(0L);
        rr.hRecIDs = AMemAllocate(0L);
        rr.retCode = UNEXPECTEDERROR;
        rr.currRecIdx = 0L;

        AOBJECTID* fields = (AOBJECTID*)AMemLock(rr.hFields);
        for(int i=0; i<theCol.NumColumns(); i++)
          fields[i] = theCol.GetObjectID(i);

        AEvtCallMethod(theObject, TABLE_ATTRIBUTES::DataSource(), NULL, autoTableReadRecords, (long)&rr);

        if(rr.retCode==1L)
          {
          AMEMBLOCKID  hTemp = hRow;
          hRow = rr.hData;
          hRowMemLockData(); // rowPtr = (char huge*)AMemLock(hRow);
          rr.hData = hTemp;

          long*       plRecID = (long*)AMemLock(rr.hRecIDs);
          m_lRecID = plRecID ? *plRecID : -1L;
          if(plRecID)
            AMemUnlock(rr.hRecIDs);
          }
	else
	  {
	  hRowMemSetSize(TABLE_ATTRIBUTES::NumColumns());
	  memset(rowPtr, 0, TABLE_ATTRIBUTES::NumColumns());
          }

        AMemUnlock(rr.hFields);
        AMemFree(rr.hFields);
        AMemFree(rr.hData);
        AMemFree(rr.hRecIDs);
        }
      }
    else
      {
      // retrieve the row with a "table needs row" signal
      UseTableNeedsRow(row, &theCol);
      m_lRecID = row;
      }
    }
  else  // retrieve the row(s) into memory
    {
    if(TABLE_ATTRIBUTES::DataSource())
      {
      long  first;
      long  need;

      AEvtPostStandard(theObject, AEVENT_SIGNAL, SIGNAL_STARTINGAUTOFILL, AEVT_ATONCE);

      if(row<TABLE_ATTRIBUTES::FirstRow())
        {
        first = row - TABLE_ATTRIBUTES::MaxStore()/2;
        if(first<0)
          first = 0;

        need = TABLE_ATTRIBUTES::FirstRow() - first;
        if(need>TABLE_ATTRIBUTES::MaxStore())
          need = TABLE_ATTRIBUTES::MaxStore();
        }
      else
        {
        first = TABLE_ATTRIBUTES::FirstRow() + TABLE_ATTRIBUTES::NumLoadedRows();
	need = row - first + TABLE_ATTRIBUTES::MaxStore()/2;

        if(need>TABLE_ATTRIBUTES::MaxStore())
          {
          first = row - TABLE_ATTRIBUTES::MaxStore()/2;
          need = TABLE_ATTRIBUTES::MaxStore();
          }

        if((first+need - TABLE_ATTRIBUTES::FirstRow())<TABLE_ATTRIBUTES::MaxStore())
          need = TABLE_ATTRIBUTES::MaxStore() - first + TABLE_ATTRIBUTES::FirstRow(); 

        if((first+need)>TABLE_ATTRIBUTES::TotalNumRows())
          need = TABLE_ATTRIBUTES::TotalNumRows() - first;
        }

      if (TABLE_ATTRIBUTES::AdHoc())
        {
        ODViewColData rr;
        rr.recNmbr = first + 1;  // 1 based
	rr.colDataMB = AMemAllocate(sizeof(AMEMBLOCKID)* TABLE_ATTRIBUTES::NumColumns());
        rr.recCount = need;
        if(need>0)
          AEvtCallMethod(theObject, TABLE_ATTRIBUTES::DataSource(), NULL, AEVENT_DVIEWGETDATA, (long)&rr);
        else
          rr.retCode = 0;

        if(rr.retCode != need
          ||
          need==0
          ||
          first==0
          ||
          (first+need)==TABLE_ATTRIBUTES::TotalNumRows())
          {
          // check the scroll range
          long  num = -1L;
          AEvtCallMethod(theObject, TABLE_ATTRIBUTES::DataSource(), NULL,AEVENT_DVIEWGETROWCOUNT, (long)&num);
          if(num<0L)
	    num = 0L;

	  if(TABLE_ATTRIBUTES::TotalNumRows()!=num)
	    {
	    TABLE_ATTRIBUTES::SetTotalNumRows(num);
	    // post object changed.
	    AEvtInfo  event;
	    event.sourceObject = theObject;
	    event.targetObject = theObject;
	    event.when         = AAppGetTime();
	    event.how          = AEVT_KILLMODIFIERS | AEVT_KILLDUPLICATES | AEvtGetObjectChangedPriority(AEVT_ATTAIL);
	    event.what         = AEVENT_OBJECTCHANGED;
	    event.modifiers    = CHANGED_NUMROWS;
	    AEvtPost(&event);
	    }
	  }

	if(rr.retCode > 0 )
	  {
	  AMEMBLOCKID  hTemp = hRow;
	  hRow = rr.colDataMB;
	  //  SKJ Oct.15'93
	  hRowMemLockData(); // rowPtr = (char huge*)AMemLock(hRow);
	  rr.colDataMB = hTemp;
	  // first is 0 based
	  StoreMultiRows(first, rr.retCode);
	  TABLE table(theObject);
	  for(i=0; i< TABLE_ATTRIBUTES::NumColumns(); i++)
	    table.SetColumnWidth(i,table.GetMaxWidth(i));
	  }
	else if(rr.retCode == -1 )
	  {
	  ADBG_PRINT_E("Error in Adhoc Query");
	  hRowMemSetSize(TABLE_ATTRIBUTES::NumColumns()*TABLE_ATTRIBUTES::NumVisRows());
	  memset(rowPtr, 0, TABLE_ATTRIBUTES::NumColumns()*TABLE_ATTRIBUTES::NumVisRows());
	  StoreMultiRows(first, TABLE_ATTRIBUTES::NumVisRows());
	  }
	else if(rr.retCode == -2 )
	  {
	  ADBG_PRINT_E("Disconnected from Data Source");
	  hRowMemSetSize(TABLE_ATTRIBUTES::NumColumns()*TABLE_ATTRIBUTES::NumVisRows());
	  memset(rowPtr, 0, TABLE_ATTRIBUTES::NumColumns()*TABLE_ATTRIBUTES::NumVisRows());
	  StoreMultiRows(first, TABLE_ATTRIBUTES::NumVisRows());
	  }
	AMemFree(rr.colDataMB);
	}
      else {
	TablREADRECORDS rr;
	rr.hFields = AMemAllocate(sizeof(AOBJECTID)*TABLE_ATTRIBUTES::NumColumns());
	rr.idxObj = TABLE_ATTRIBUTES::IndexedField();
	rr.relIdx = first;
	rr.readType = SEEK_SET;
	rr.recCount = need;
	rr.hData = AMemAllocate(0L);
	rr.hRecIDs = AMemAllocate(0L);
	rr.retCode = UNEXPECTEDERROR;
	rr.currRecIdx = need;

	AOBJECTID* fields = (AOBJECTID*)AMemLock(rr.hFields);
	for(int i=0; i<theCol.NumColumns(); i++)
	  fields[i] = theCol.GetObjectID(i);

	if(need>0)
	  AEvtCallMethod(theObject, TABLE_ATTRIBUTES::DataSource(), NULL, autoTableReadRecords, (long)&rr);
	else
	  rr.retCode = 0;

	if(rr.retCode!=need
	  ||
	  need==0
	  ||
	  first==0
	  ||
	  (first+need)==TABLE_ATTRIBUTES::TotalNumRows())
	  {
	  // check the scroll range
	  long  num = -1L;
	  AEvtCallMethod(theObject, TABLE_ATTRIBUTES::DataSource(), NULL, autoTableRecordCount, (long)&num);
	  if(num<0L)
	    num = 0L;

	  if(TABLE_ATTRIBUTES::TotalNumRows()!=num)
	    {
	    TABLE_ATTRIBUTES::SetTotalNumRows(num);
	    // post object changed.
	    AEvtInfo  event;
	    event.sourceObject = theObject;
	    event.targetObject = theObject;
	    event.when         = AAppGetTime();
	    event.how          = AEVT_KILLMODIFIERS | AEVT_KILLDUPLICATES | AEvtGetObjectChangedPriority(AEVT_ATTAIL);
	    event.what         = AEVENT_OBJECTCHANGED;
	    event.modifiers    = CHANGED_NUMROWS;
	    AEvtPost(&event);
	    }
	  }

	if(rr.retCode>0L)
	  {
	  AMEMBLOCKID  hTemp = hRow;
	  hRow = rr.hData;
	  //  SKJ Oct.15'93
	  hRowMemLockData(); // rowPtr = (char huge*)AMemLock(hRow);
	  rr.hData = hTemp;
	  StoreMultiRows(first, rr.retCode);

	  long *  recID = (long *)AMemLock(rr.hRecIDs);
	  for(long j=0; j<rr.retCode; j++)
	    TABLE_ATTRIBUTES::SetRecordID(first+j, *(recID+j));
	  AMemUnlock(rr.hRecIDs);
	  }
	else
	  {
	  hRowMemSetSize(TABLE_ATTRIBUTES::NumColumns()*TABLE_ATTRIBUTES::NumVisRows());
	  memset(rowPtr, 0, TABLE_ATTRIBUTES::NumColumns()*TABLE_ATTRIBUTES::NumVisRows());
	  StoreMultiRows(first, TABLE_ATTRIBUTES::NumVisRows());
	  }

	AMemUnlock(rr.hFields);
        AMemFree(rr.hFields);
        AMemFree(rr.hData);
        AMemFree(rr.hRecIDs);
        }
      }  // end of if DataSource()
    else
      {
      long    firstRow = TABLE_ATTRIBUTES::FirstRow();

      if(row<firstRow)
        {
        long  max = firstRow;
        if(row<(firstRow-(TABLE_ATTRIBUTES::MaxStore() + TABLE_ATTRIBUTES::NumVisRows())))
          {
          max = row + 2*TABLE_ATTRIBUTES::NumVisRows() + 1;
          ClearLoadedRows();
          }

	// retrieve the row with a "table needs row" signal and store it in the data
        for(long i=(max-1); i>=row; i--)
          {
          UseTableNeedsRow(i, &theCol);
          StoreTheRow(i);
          TABLE_ATTRIBUTES::SetRecordID(row, row);
          }
        }
      else if(row>=firstRow)
        {
        long  first = firstRow + TABLE_ATTRIBUTES::NumLoadedRows();
        long  last = row + TABLE_ATTRIBUTES::NumVisRows();
        if((last - firstRow) > TABLE_ATTRIBUTES::MaxStore())
          {
          ClearLoadedRows();
          first = row - TABLE_ATTRIBUTES::NumVisRows();
          last = row + TABLE_ATTRIBUTES::NumVisRows();
          }

	if(last<first)
          last=first;

        if(last>=TABLE_ATTRIBUTES::TotalNumRows())
          last = TABLE_ATTRIBUTES::TotalNumRows()-1;

        // retrieve the row with a "table needs row" signal and store it in the data
        for(long i=first; i<=last; i++)
          {
          UseTableNeedsRow(i, &theCol);
          StoreTheRow(i);
          TABLE_ATTRIBUTES::SetRecordID(i, i);
          }
        }
      }
    }
  }

// ROW::UseTableNeedsRow ///////////////////////////////////////////////////

void ROW::UseTableNeedsRow(
  long    row,
  COLUMN* theCol)
  {
  if(TABLE_ATTRIBUTES::RowIndexObject())
    ONmbrSetInteger(TABLE_ATTRIBUTES::RowIndexObject(), row+1L, FALSE);
  // Nmbr_LongToObject(TABLE_ATTRIBUTES::RowIndexObject(), row+1L);

  AEvtPostStandard(theObject, AEVENT_SIGNAL, SIGNAL_NEEDSROW, AEVT_ATONCE);

  // get the row
  AMEMBLOCKID  hObject = AMemAllocate(0L);
  char*       objPtr;
  long        txtSize = 0L;
  long        objSize;

  hRowMemSetSize(0L);

  for(int i=0; i<theCol->NumColumns(); i++)
    {
    AObjConvertToText(theCol->GetObjectID(i), hObject);
    objPtr = (char*)AMemLock(hObject);
    if(objPtr)
      {
      objSize = lstrlen(objPtr)+1;
      hRowMemSetSize(txtSize+objSize);
      CopyMemory(rowPtr+txtSize, objPtr, objSize);
      txtSize += objSize;
      *objPtr = 0;
      }
    else
      {
      hRowMemSetSize(txtSize+objSize);
      *(rowPtr+txtSize++) = 0;
      }
    AMemUnlock(hObject);
    }

  AMemFree(hObject);
  }

// ROW::ClearLoadedRows ////////////////////////////////////////////////////

void ROW::ClearLoadedRows()

  {
  FocalResizeData(theObject, DATA_ROWOFFSETS, 0L);
  numRows = 0L;

  FocalResizeData(theObject, DATA_CELLDATA, 0L);
  dataLength = 0L;

  TABLE_ATTRIBUTES::SetFirstRow(0L);
  TABLE_ATTRIBUTES::SetNumLoadedRows(0L);
  TABLE_ATTRIBUTES::SetRecordID(-1L, 0L);
  }

// ROW::StoreTheRow ////////////////////////////////////////////////////////

void ROW::StoreTheRow(
  long  row)

  {
  long  rowLength = AMemGetSize(hRow);

  if(TABLE_ATTRIBUTES::NumLoadedRows()==0)
    {
    TABLE_ATTRIBUTES::SetFirstRow(row);
    TABLE_ATTRIBUTES::SetNumLoadedRows(1L);

    FocalResizeData(theObject, DATA_ROWOFFSETS, (numRows+1L)*sizeof(long));
    *(*rowOffsets+numRows) = dataLength;
    numRows++;
    FocalResizeData(theObject, DATA_CELLDATA, dataLength+rowLength);
    CopyMemory(*cellData+dataLength, rowPtr, rowLength);
    dataLength += rowLength;
    }
  else if(row<TABLE_ATTRIBUTES::FirstRow())
    {
    if(TABLE_ATTRIBUTES::NumLoadedRows()>=TABLE_ATTRIBUTES::MaxStore())
      { // free the last rows
      dataLength = *(*rowOffsets+numRows-TABLE_ATTRIBUTES::NumVisRows());
      numRows -= TABLE_ATTRIBUTES::NumVisRows();
      FocalResizeData(theObject, DATA_ROWOFFSETS, numRows*sizeof(long));
      FocalResizeData(theObject, DATA_CELLDATA, dataLength);
      }

    // add at the first position
    hRowMemSetSize(rowLength+dataLength);
    CopyMemory(rowPtr+rowLength, *cellData, dataLength);
    dataLength += rowLength;

    FocalResizeData(theObject, DATA_CELLDATA, dataLength);

    CopyMemory(*cellData, rowPtr, dataLength);

    FocalResizeData(theObject, DATA_ROWOFFSETS, (numRows+1L)*sizeof(long));

    long  i=numRows;

    while(i>0)
      {
      *(*rowOffsets+i) = *(*rowOffsets+(i-1)) + rowLength;
      i--;
      }
    numRows++;

    TABLE_ATTRIBUTES::SetFirstRow(row);
    TABLE_ATTRIBUTES::SetNumLoadedRows(numRows);
    }
  else
    {
    if(TABLE_ATTRIBUTES::NumLoadedRows()>=TABLE_ATTRIBUTES::MaxStore())
      { // free the first rows
      long  firstRow = TABLE_ATTRIBUTES::FirstRow();
      long  numVis = TABLE_ATTRIBUTES::NumVisRows();
      dataLength -= *(*rowOffsets+numVis);
      numRows -= numVis;
      firstRow += numVis;
      long  correction = *(*rowOffsets+numVis);

      long  i=0L;
      while(i<numRows)
        {
        *(*rowOffsets+i) = *(*rowOffsets+i+numVis) - correction;
        i++;
        }

      FocalResizeData(theObject, DATA_ROWOFFSETS, numRows*sizeof(long));

  
      i=0L;
      while(i<dataLength)
        {
        *(*cellData+i) = *(*cellData+(i+correction));
        i++;
        }

      FocalResizeData(theObject, DATA_CELLDATA, dataLength);

      TABLE_ATTRIBUTES::SetFirstRow(firstRow);
      }

    // append the row to the end

    FocalResizeData(theObject, DATA_ROWOFFSETS, (numRows+1L)*sizeof(long));

    *(*rowOffsets+numRows) = dataLength;
    numRows++;

    FocalResizeData(theObject, DATA_CELLDATA, dataLength+rowLength);


    CopyMemory(*cellData+dataLength, rowPtr, rowLength);
    dataLength += rowLength;
    TABLE_ATTRIBUTES::SetNumLoadedRows(numRows);

    }

  }

// ROW::StoreMultiRows /////////////////////////////////////////////////////

void ROW::StoreMultiRows(
  long  firstRow,
  long  rowCount)

  {
  
  if(rowCount<=0)
    return;

  long        rowsLength = AMemGetSize(hRow);
  HGLOBAL     hOff = GlobalAlloc(GMEM_MOVEABLE, sizeof(long)*rowCount);
  if(!hOff)
    return;

  long *  off = (long *)GlobalLock(hOff);
  long        size=0L;
  for(long i=0L; i<rowCount; i++)
    {
    *(off+i) = size;
    for(int j=0; j<TABLE_ATTRIBUTES::NumColumns(); j++)
      size += (lstrlen(rowPtr+size)+1L);
    }
  if(size!=rowsLength)
    rowsLength = size;


  if(TABLE_ATTRIBUTES::NumLoadedRows()==0)
    {
    TABLE_ATTRIBUTES::SetFirstRow(firstRow);
    TABLE_ATTRIBUTES::SetNumLoadedRows(rowCount);

    numRows = rowCount;

    FocalResizeData(theObject, DATA_ROWOFFSETS, numRows*sizeof(long));

    CopyMemory(*rowOffsets, off, rowCount*sizeof(long));

    // unlock as many times as locked
    FocalResizeData(theObject, DATA_CELLDATA, rowsLength);

    CopyMemory(*cellData, rowPtr, rowsLength);
    dataLength = rowsLength;
    }
  else if(firstRow<TABLE_ATTRIBUTES::FirstRow())
    {
    if((rowCount + TABLE_ATTRIBUTES::NumLoadedRows())>TABLE_ATTRIBUTES::MaxStore())
      { // free the last rows
      long  numToKill = rowCount + TABLE_ATTRIBUTES::NumLoadedRows() - TABLE_ATTRIBUTES::MaxStore();
      if(numToKill>numRows)
	numToKill = numRows;

      dataLength = *(*rowOffsets+numRows-numToKill);
      numRows -= numToKill;

      // unlock as many times as locked
      FocalResizeData(theObject, DATA_ROWOFFSETS, numRows*sizeof(long));
      FocalResizeData(theObject, DATA_CELLDATA, dataLength);

      long *  pRecID = (long *)AObjLockData(theObject, DATA_RECORDIDS);
      if(pRecID)
        {
        for(i = numRows-1; i>=0; i--)
          *(pRecID+i+numToKill) = *(pRecID+i);

	AObjUnlockData(theObject, DATA_RECORDIDS);
        }
      }

    // add at the first position
    hRowMemSetSize(rowsLength+dataLength);
    CopyMemory(rowPtr+rowsLength, *cellData, dataLength);
    
    dataLength += rowsLength;
    // unlock as many times as locked
    FocalResizeData(theObject, DATA_CELLDATA, dataLength);

    CopyMemory(*cellData, rowPtr, dataLength);

    FocalResizeData(theObject, DATA_ROWOFFSETS, (numRows+rowCount)*sizeof(long));

  
    long  i = numRows - 1L;
    while(i>=0)
      {
      *(*rowOffsets+i+rowCount) = *(*rowOffsets+i) + rowsLength;
      i--;
      }
    numRows+=rowCount;
    CopyMemory(*rowOffsets, off, rowCount*sizeof(long));

    TABLE_ATTRIBUTES::SetFirstRow(firstRow);
    TABLE_ATTRIBUTES::SetNumLoadedRows(numRows);
    }
  else
    {
    if(firstRow>(TABLE_ATTRIBUTES::FirstRow()+TABLE_ATTRIBUTES::NumLoadedRows()))
      {
      // free all loaded rows
      TABLE_ATTRIBUTES::SetFirstRow(firstRow);
      numRows=0L;

      FocalResizeData(theObject, DATA_ROWOFFSETS, numRows*sizeof(long));

      dataLength=0L;

      FocalResizeData(theObject, DATA_CELLDATA, dataLength);
      }
    else if((TABLE_ATTRIBUTES::NumLoadedRows()+rowCount)>TABLE_ATTRIBUTES::MaxStore())
      { // free the first rows
      long  numToKill = TABLE_ATTRIBUTES::NumLoadedRows()+rowCount - TABLE_ATTRIBUTES::MaxStore();
      long  first_Row = TABLE_ATTRIBUTES::FirstRow();
      long  correction = dataLength;
      if(numToKill!=numRows)
        correction = *(*rowOffsets+numToKill);
      dataLength -= correction;
      numRows -= numToKill;
      first_Row += numToKill;

      long  i=0L;
      while(i<numRows)
        {
	*(*rowOffsets+i) = *(*rowOffsets+(i+numToKill)) - correction;
        i++;
        }

      FocalResizeData(theObject, DATA_ROWOFFSETS, numRows*sizeof(long));

      i=0L;
      while(i<dataLength)
        {
        *(*cellData+i) = *(*cellData+(i+correction));
        i++;
        }

      FocalResizeData(theObject, DATA_CELLDATA, dataLength);

      long *  pRecID = (long *)AObjLockData(theObject, DATA_RECORDIDS);
      if(pRecID)
	{
	for(i = 0; i<numRows; i++)
	  *(pRecID+i) = *(pRecID+i+numToKill);

	AObjUnlockData(theObject, DATA_RECORDIDS);
	}

      TABLE_ATTRIBUTES::SetFirstRow(first_Row);
      }

    // append the rows to the end
    FocalResizeData(theObject, DATA_ROWOFFSETS, (numRows+rowCount)*sizeof(long));
    
    for(int i=0; i<rowCount; i++)
      *(*rowOffsets+numRows+i) = *(off+i)+dataLength;
    numRows+=rowCount;

    FocalResizeData(theObject, DATA_CELLDATA, dataLength+rowsLength);

    CopyMemory(*cellData+dataLength, rowPtr, rowsLength);
    dataLength += rowsLength;
    TABLE_ATTRIBUTES::SetNumLoadedRows(numRows);
    }

  GlobalUnlock(hOff);
  GlobalFree(hOff);
  }

// ROW::SetRowCount ////////////////////////////////////////////////////////

BOOL ROW::SetRowCount(
  long  rowCount)

  {
  if(rowCount>0x7FFF)
    return FALSE;

  if(numRows>rowCount)
    {
    dataLength = *(*rowOffsets+rowCount);
    numRows = rowCount;
    FocalResizeData(theObject, DATA_CELLDATA, dataLength);
    FocalResizeData(theObject, DATA_ROWOFFSETS, numRows*sizeof(long));
    }
  else
    {
    long    addNumRows = rowCount-numRows;
    if((addNumRows * TABLE_ATTRIBUTES::NumColumns()) > 0x7FFF)
      return FALSE;

    int     numChars = (int)addNumRows * TABLE_ATTRIBUTES::NumColumns();
    long*   offsets = new long[(int)addNumRows];
    for(int i=0; i<(int)addNumRows; i++)
      offsets[i] = i*TABLE_ATTRIBUTES::NumColumns();

    HGLOBAL hChars = GlobalAlloc(GPTR, numChars);
    if(hChars)
      {
      char*   newData = (char*)GlobalLock(hChars);
      if(newData) 
        {
        InsertData(numRows, (int)addNumRows, newData, (UINT)numChars, offsets);
        GlobalUnlock(hChars);
        }
      GlobalFree(hChars);
      }
    delete offsets;
    }

  return TRUE;
  }

// ROW::Clear //////////////////////////////////////////////////////////////

void ROW::Clear
  (
  )

  {
  FocalResizeData(theObject, DATA_CELLDATA,   0L);
  FocalResizeData(theObject, DATA_ROWOFFSETS, 0L);
  FocalResizeData(theObject, DATA_SELECTIONS, 0L);

  dataLength    = 0;
  numRows       = 0;
  numSelections = 0;

  if(TABLE_ATTRIBUTES::AutoFill())
    {
    TABLE_ATTRIBUTES::SetFirstRow(0L);
    TABLE_ATTRIBUTES::SetNumLoadedRows(0L);
    }
  }
