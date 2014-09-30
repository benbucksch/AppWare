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
// FILE:  "tbl_atrb.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __TBL_ATRB_H  // prevent multiple includes
#define __TBL_ATRB_H

// includes ////////////////////////////////////////////////////////////////

#ifndef __TABL_HDR_H
#include "tabl_hdr.h"
#endif

#include "tbl_clmn.h"

#ifndef _O_WND_H
#include "op_wnd.h"
#include "o_wnd.h"
#endif

struct oldTATTRIB
  {
  int         version;        // internal version for future...
  int         drawColumns;
  BOOL        drawRows;
  BOOL        showTitles;
  BOOL        hScroll;
  BOOL        vScroll;
  BOOL        multiSelections;
  BOOL        doGrow;
  BOOL        canActivate;
  long        rowsStart;
  long        numRows;
  BOOL        isConnected;
  AOBJECTID    indexRef;
  long double hDiv;
  long double vDiv;
  long int    hMax;
  long int    vMax;
  long int    hVal;
  long int    vVal;
  long int    hCur;
  long int    vCur;
  int         passW;
  int         passH;
  int         focusColumn;
  int         editColumn;
  long int    editRow;
  int         activeColumn;
  long int    activeRow;
  int         numOfCols;
  int         oldIndex;
  int         leftPane; // added later, so may or may not be in object data
  };

// version 1.1  before April 27'94
struct TATTRIBv1_0
  {
  int       version;        // internal version for future...
  BOOL      columnLines;
  BOOL      rowLines;
  BOOL      showTitles;
  BOOL      hScroll;
  BOOL      vScroll;
  BOOL      multiSelections;
  BOOL      canActivate;
  BOOL      canResize;
  BOOL      showSelection;
  BOOL      swapColors;
  int       focusColumn;
  int       numOfCols;
  int       fixedColumns;
  int       activeColumn; // column for the active cell
  long      activeRow;    // row for the active cell
  int       editColumn;   // last edited column
  long      editRow;      // last edited row
  WPARAM    lastWidthRB;
  int       titleLeading; // leading includes both internal & external leading
  int       titleAscent;
  int       titleDescent;
  int       cellLeading;  // leading includes both internal & external leading
  int       cellAscent;
  int       cellDescent;
  COLORREF  fixedColor;   // color of the fixed lines (column header & fixed column) to be drawn
  COLORREF  lineColor;    // color of the line to be drawn
  COLORREF  selFG;        // foreground color for selections
  COLORREF  selBG;        // background color for selections
  long      anchorRow;    // anchor for selections....
  BOOL      printDouble;  // should I print double Lines?
  BOOL      pageNumbers;  // should page numbers be printed?
  BOOL      showActive;   // draw a active cell border ?

  // auto fill data
  BOOL      autoFill;
  AOBJECTID  dataSource;   // data source object
  AOBJECTID  indexedField; // indexed field object
  AOBJECTID  indexRef;     // object ID for the row index object
  long      firstRow;     // first row loaded in memory
  long      numRows;      // number of rows loaded in memory
  long      totalNumRows; // total number of rows for the auto-fill table
  long*     recordIDs;    // array for loaded rows record IDs
  long      numVisRows;   // max number of visual rows for all visual instances
  int       maxStore;     // maximum number of rows to store in memory
  BOOL      clairvoyant;  // clairvoyant selection flag
  };

// attributes structure -- actual object data structure ////////////////////

struct TATTRIB
  {
  int       version;        // internal version for future...
  BOOL      columnLines;
  BOOL      rowLines;
  BOOL      showTitles;
  BOOL      hScroll;
  BOOL      vScroll;
  BOOL      multiSelections;
  BOOL      canActivate;
  BOOL      canResize;
  BOOL      showSelection;
  BOOL      swapColors;
  int       focusColumn;
  int       numOfCols;
  int       fixedColumns;
  int       activeColumn; // column for the active cell
  long      activeRow;    // row for the active cell
  int       editColumn;   // last edited column
  long      editRow;      // last edited row
  WPARAM    lastWidthRB;
  int       titleLeading; // leading includes both internal & external leading
  int       titleAscent;
  int       titleDescent;
  int       cellLeading;  // leading includes both internal & external leading
  int       cellAscent;
  int       cellDescent;
  COLORREF  fixedColor;   // color of the fixed lines (column header & fixed column) to be drawn
  COLORREF  lineColor;    // color of the line to be drawn
  COLORREF  selFG;        // foreground color for selections
  COLORREF  selBG;        // background color for selections
  long      anchorRow;    // anchor for selections....
  BOOL      printDouble;  // should I print double Lines?
  BOOL      pageNumbers;  // should page numbers be printed?
  BOOL      showActive;   // draw a active cell border ?

  // auto fill data
  BOOL      autoFill;
  BOOL      bAdhoc;       // BOOL to indicate whether an auto-table is adhoc April27, 1994
  AOBJECTID  dataSource;   // data source object
  AOBJECTID  indexedField; // indexed field object
  AOBJECTID  indexRef;     // object ID for the row index object
  long      firstRow;     // first row loaded in memory
  long      numRows;      // number of rows loaded in memory
  long      totalNumRows; // total number of rows for the auto-fill table
  long*     recordIDs;    // array for loaded rows record IDs
  long      numVisRows;   // max number of visual rows for all visual instances
  int       maxStore;     // maximum number of rows to store in memory
  BOOL      clairvoyant;  // clairvoyant selection flag

  // SKJ April 28, 1994 Def added to handle adhoc tables
  TCOLS     defColumn;  // default attributes of COlumn

  // SKJ pad some extra bytes at the end
  BYTE      extraBytes[4]; // April 27. 1994
  };

// Table Attributes managment class -- for each visual instance ////////////

class TABLE_ATTRIBUTES
  {
  private:
    AOBJECTID  theObject;
    TATTRIB*  attrib_data;

    // private methods
    void      Initialize();                         

  public:
    TABLE_ATTRIBUTES(AOBJECTID objID);
    TABLE_ATTRIBUTES(AOBJECTID objID, TATTRIB* tAttrib);
    ~TABLE_ATTRIBUTES();

    // inline methods
    int       Version()         {return attrib_data->version;}
    int       FocusColumn()     {return attrib_data->focusColumn;}
    int       ActiveColumn()    {return attrib_data->activeColumn;}
    long      ActiveRow()       {return attrib_data->activeRow;}
    int       EditColumn()      {return attrib_data->editColumn;}
    long      EditRow()         {return attrib_data->editRow;}
    int       NumColumns()      {return attrib_data->numOfCols;}
    int       NumSignals()      {return attrib_data->autoFill ? NUM_SIGNALS_CONNECTED : NUM_SIGNALS_NOCONNECT;}
    BOOL      AutoFill()        {return attrib_data->autoFill;}
    BOOL      AdHoc()           {return attrib_data->bAdhoc;}
    BOOL      MultiSelect()     {return attrib_data->multiSelections;}
    BOOL      ShowTitles()      {return attrib_data->showTitles;}
    BOOL      CanActivate()     {return attrib_data->canActivate;}
    BOOL      RowLines()        {return attrib_data->rowLines;}
    BOOL      ColumnLines()     {return attrib_data->columnLines;}
    BOOL      VertScroll()      {return attrib_data->vScroll;}
    BOOL      HorzScroll()      {return attrib_data->hScroll;}
    AOBJECTID  RowIndexObject()  {return attrib_data->indexRef;}
    int       NumFixedColumns() {return (attrib_data->fixedColumns >= 0) ? attrib_data->fixedColumns : 0;}
    WPARAM    LastWidthRB()     {return attrib_data->lastWidthRB;}
    int       TitleLeading()    {return attrib_data->titleLeading;}
    int       TitleAscent()     {return attrib_data->titleAscent;}
    int       TitleDescent()    {return attrib_data->titleDescent;}
    int       TitleHeight()     {return attrib_data->titleAscent +  attrib_data->titleDescent + attrib_data->titleLeading;}
    int       CellLeading()     {return attrib_data->cellLeading;}
    int       CellAscent()      {return attrib_data->cellAscent;}
    int       CellDescent()     {return attrib_data->cellDescent;}
    int       CellHeight()      {return attrib_data->cellAscent + attrib_data->cellDescent + attrib_data->cellLeading;}
    COLORREF  FixedColor()      {return attrib_data->fixedColor;}
    COLORREF  LineColor()       {return attrib_data->lineColor;}
    BOOL      CanResize()       {return attrib_data->canResize;}
    BOOL      ShowSelection()   {return attrib_data->showSelection;}
    BOOL      SwapColors()      {return attrib_data->swapColors;}
    COLORREF  SelectionFG()     {return attrib_data->selFG;}
    COLORREF  SelectionBG()     {return attrib_data->selBG;}
    long      AnchorRow()       {return attrib_data->anchorRow;}
    BOOL      PrintDoubleLines(){return (attrib_data->canActivate && attrib_data->printDouble) ? 3 : 1;}
    int       PrintPageNums()   {return attrib_data->pageNumbers;}
    BOOL      ShowActiveCell()  {return attrib_data->showActive;}
    AOBJECTID  DataSource()      {return attrib_data->dataSource;}
    AOBJECTID  IndexedField()    {return attrib_data->indexedField;}
    long      FirstRow()        {return attrib_data->firstRow;}
    long      NumLoadedRows()   {return attrib_data->numRows;}
    long      TotalNumRows()    {return attrib_data->totalNumRows;}
    long      NumVisRows()      {return attrib_data->numVisRows;}
    int       MaxStore()        {return attrib_data->maxStore;}
    BOOL      Clairvoyant()     {return attrib_data->clairvoyant;}

    void      ClearNumVisRows()               {attrib_data->numVisRows = 1L;}
    void      SetFocusColumn(int col)         {attrib_data->focusColumn = col;}
    void      SetActiveColumn(int col)        {attrib_data->activeColumn = col;}
    void      SetActiveRow(long row)          {attrib_data->activeRow = row;}
    void      SetEditColumn(int col)          {attrib_data->editColumn = col;}
    void      SetEditRow(long row)            {attrib_data->editRow = row;}
    void      SetNumColumns(int numCol)       {attrib_data->numOfCols = numCol;}
    void      SetAutoFill(BOOL b)             {attrib_data->autoFill = b;}
    void      SetAdHoc(BOOL b)                {attrib_data->bAdhoc = b;}
    void      SetMultiSelect(BOOL b)          {attrib_data->multiSelections = b;}
    void      SetShowTitles(BOOL b)           {attrib_data->showTitles = b;}
    void      SetCanActivate(BOOL b)          {attrib_data->canActivate = b;}
    void      SetRowLines(BOOL b)             {attrib_data->rowLines = b;}
    void      SetColumnLines(BOOL b)          {attrib_data->columnLines = b;}
    void      SetVertScroll(BOOL b)           {attrib_data->vScroll = b;}
    void      SetHorzScroll(BOOL b)           {attrib_data->hScroll = b;}
    void      SetRowIndexObject(AOBJECTID obj) {attrib_data->indexRef = obj;}
    void      SetNumFixedColumns(int num)     {attrib_data->fixedColumns = num;}
    void      SetLastWidthRB(WPARAM last)     {attrib_data->lastWidthRB = last;}
    void      SetTitleLeading(int leading)    {attrib_data->titleLeading = leading;}
    void      SetTitleAscent(int ascent)      {attrib_data->titleAscent = ascent;}
    void      SetTitleDescent(int descent)    {attrib_data->titleDescent = descent;}
    void      SetCellLeading(int leading)     {attrib_data->cellLeading = leading;}
    void      SetCellAscent(int ascent)       {attrib_data->cellAscent = ascent;}
    void      SetCellDescent(int descent)     {attrib_data->cellDescent = descent;}
    void      SetFixedColor(COLORREF color)   {attrib_data->fixedColor = color;}
    void      SetLineColor(COLORREF color)    {attrib_data->lineColor = color;}
    void      SetCanResize(BOOL b)            {attrib_data->canResize = b;}
    void      SetShowSelection(BOOL b)        {attrib_data->showSelection = b;}
    void      SetSwapColors(BOOL b)           {attrib_data->swapColors = b;}
    void      SetSelectionFG(COLORREF color)  {attrib_data->selFG = color;}
    void      SetSelectionBG(COLORREF color)  {attrib_data->selBG = color;}
    void      SetAnchorRow(long row)          {attrib_data->anchorRow = row;}
    void      SetShowActiveCell(BOOL b)       {attrib_data->showActive = b;}
    void      SetDataSource(AOBJECTID obj)     {attrib_data->dataSource = obj;}
    void      SetIndexedField(AOBJECTID obj)   {attrib_data->indexedField = obj;}
    void      SetFirstRow(long row)           {attrib_data->firstRow = row;}
    void      SetNumLoadedRows(long num)      {attrib_data->numRows = num;}
    void      SetTotalNumRows(long tot)       {attrib_data->totalNumRows = tot;}
    void      SetMaxStore(int max)            {attrib_data->maxStore = max;}
    void      SetClairvoyant(BOOL b)          {attrib_data->clairvoyant = b;}

    // methods
    BOOL      Export();
    BOOL      Import();
    long      GetRecordID(long row);
    void      SetRecordID(long row, long ID);
    void      SetNumVisRows(long num);
    void      InitDefColumn(void);
  };
typedef TABLE_ATTRIBUTES *   LPTABLE_ATTRIBUTES;

////////////////////////////////////////////////////////////////////////////

#endif  //#ifndef __TBL_ATRB_H
