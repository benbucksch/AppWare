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
// FILE:  "tabl_row.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __TABL_ROW_H  // prevent multiple instances
#define __TABL_ROW_H

#ifndef __TBL_ATRB_H
#include "tbl_atrb.h"
#endif

#ifndef __TBL_CLMN_H
class COLUMN;
#endif

// Row management class ////////////////////////////////////////////////////

class ROW :
  private TABLE_ATTRIBUTES
  {
  private:
    OBJECTID    theObject;
    char huge*  *cellData;
    long huge*  *rowOffsets;
    long huge*  *selections;
    long        dataLength;
    long        numRows;
    long        numSelections;
    MEMBLOCKID  hRow;
    long        m_lRecID;
    char huge*  rowPtr;


    ///////////////////////////////////////////////////////////
    // A set of oip Data Management functions that lock/unlock/set/resize datas.
    // This is to provide one control point for all the data operations for the
    // following datas:
    //                      1.  DATA_CELLDATA
    //                      2.  DATA_ROWOFFSETS
    //                      3.  DATA_SELECTIONS
    ///////////////////////////////////////////////////////////////////////////
    LPVOID  FocalLockData(OBJECTID theObject, long dataIndex);
    BOOL    FocalUnlockData(OBJECTID theObject, long dataIndex);
    BOOL    FocalResizeData(OBJECTID theObject, long dataIndex, long lNewSize, BOOL init=FALSE);
    BOOL    FocalSetData(
              OBJECTID  theObject,
              LONG      dataIndex,
              LPVOID    theData,
              LONG      lSize);
    // Centralized control for rowPtr field
    LPVOID  hRowMemLockData();
    BOOL    hRowMemUnlockData();
    BOOL    hRowMemSetSize(long lNewSize);
    BOOL    IsPtrValid(long dataIndex);
    BOOL    SaveRunTimeInfo(OBJECTID oid, LONG lIndex, LPVOID lVal);
    LPVOID *GetRunTimeInfo(OBJECTID oid,  LONG lIndex);

    friend BOOL Tabl_doAssign(OBJECTID dstObject, OBJECTID srcObject);

  public:
    ROW(OBJECTID objID);
    ~ROW();

    // inline methods
    long      NumSelections() {return numSelections;}
    long      DataLength()    {return dataLength;}
    int       GetNumCols()    {return TABLE_ATTRIBUTES::NumColumns();}
    long      GetLastRecID()  {return m_lRecID;}

    // other methods
    long      NumRows();
    void      InsertData(long index, int addNumRows, HPSTR newData, UINT size, long* offsets);
    void      AddRow(long index, char* text, int numColumns, BOOL setRows);
    void      DeleteSelectedRows();
    void      DeleteRow(long row);
    HPSTR     GetCell(long row, int col, BOOL load, BOOL* dbAccessed);
    void      SetCell(long row, int col, char* data);
    void      UpdateSelections(long firstNewRow, int numRows);
    long      FirstSelection();
    long      LastSelection();
    void      ClearSelections();
    BOOL      IsSelected(long row);
    long      GetSelection(long index);
    void      SetSelection(int num, long* sel);
    void      RangeSelection(long anchor, long sel);
    void      ToggleSelection(long row);
    void      MoveRow(long dst, long source);
    void      SwapRows(long row1, long row2);
    long      RowLength(long row, BOOL load, BOOL* dbAccessed);
    void      GetRow(long row, char huge* dst, long length, BOOL addTabs, BOOL load, BOOL* dbAccessed);
    BOOL      ReorderRows(long huge* list);
    void      UpdateObject(int numCols);
    void      DeleteColumn(int colIndex, int numCols);
    void      InsertColumn(int colIndex, int numCols);
    void      SetColumn(char huge* text, int colIndex, int numCols, BOOL tabDelimited=FALSE);
    void      GetAutoRow(long row, BOOL load, BOOL* dbAccessed);
    void      UseTableNeedsRow(long row, COLUMN* theCol);
    void      ClearLoadedRows();
    void      StoreTheRow(long row);
    void      StoreMultiRows(long firstRow, long rowCount);
    BOOL      SetRowCount(long rowCount);
    void      Clear();
  };
typedef ROW far*  LPROW;

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef __TABL_ROW_H
