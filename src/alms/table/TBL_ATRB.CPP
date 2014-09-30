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
// FILE:  "tbl_atrb.cpp"
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

// include files

#include  <windows.h>

#include  <a_alm.h>
#include  <a_almutl.h>
#include  <op_wnd.h>
#include  <o_wnd.h>
#include  <string.h>
#include  <mem.h>

#include  "tabl_hdr.h"
#include  "tabl_rsc.h"
#include  "tbl_atrb.h"
#include  "tbl_clmn.h"
#include  "tabl_row.h"
#include  "table.h"

// TABLE_ATTRIBUTES constructor ////////////////////////////////////////////

TABLE_ATTRIBUTES::TABLE_ATTRIBUTES(
  AOBJECTID  objID)

  {
  theObject = objID;

  long  theSize = AObjGetDataSize(theObject, DATA_TABLEATTRIBUTES);
  long  count = AObjGetDataCount(theObject);

  if(count!=NUM_TABLEDATA)
    AObjSetDataCount(theObject, NUM_TABLEDATA);

  if(theSize==0) // only if not gone thro' OBJECT EDIT DLG
    {
    AObjResizeData(theObject, DATA_TABLEATTRIBUTES, sizeof(TATTRIB));
    attrib_data = (TATTRIB*)AObjLockData(theObject, DATA_TABLEATTRIBUTES);
    if(!attrib_data)
      return;

    Initialize();
    AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);

    return;
    }

  // translate the data as necessary -- version is the first item in the data
  attrib_data = (TATTRIB*)AObjLockData(theObject, DATA_TABLEATTRIBUTES);

  if((attrib_data->version == VERSION_OBJECT) && (theSize != sizeof(TATTRIB)))
    {
    AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);
    AObjResizeData(theObject, DATA_TABLEATTRIBUTES, sizeof(TATTRIB));
    attrib_data = (TATTRIB*)AObjLockData(theObject, DATA_TABLEATTRIBUTES);
    attrib_data->clairvoyant = TRUE;
    }

  if(attrib_data->version != VERSION_OBJECT)
    {
    int oldVersion;
    switch(attrib_data->version) // from previous lock
      {
      case 0:
        {
        oldTATTRIB *tmpPtr = (oldTATTRIB*)AObjLockData(theObject, DATA_TABLEATTRIBUTES);
        oldTATTRIB  old = *tmpPtr;
        AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);
        oldVersion = old.version;
        // the version is 0 -- translate the object data to the new version
        AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);
        AObjResizeData(theObject, DATA_TABLEATTRIBUTES, sizeof(TATTRIB));
        attrib_data = (TATTRIB*)AObjLockData(theObject, DATA_TABLEATTRIBUTES);


        attrib_data->version = VERSION_OBJECT;
        attrib_data->columnLines = old.drawColumns;
        attrib_data->rowLines = old.drawRows;
        attrib_data->showTitles = old.showTitles;
        attrib_data->hScroll = old.hScroll;
        attrib_data->vScroll = old.vScroll;
        attrib_data->multiSelections = old.multiSelections;
        attrib_data->canActivate = old.canActivate;
        attrib_data->canResize = TRUE;
        attrib_data->showSelection = TRUE;
        attrib_data->swapColors = FALSE;
        attrib_data->focusColumn = old.focusColumn;
        attrib_data->numOfCols = old.numOfCols;
        attrib_data->fixedColumns = (theSize==sizeof(oldTATTRIB)) ? old.leftPane : 0;
        attrib_data->activeColumn = old.activeColumn;
        attrib_data->activeRow = old.activeRow;
        attrib_data->editColumn = old.editColumn;
        attrib_data->editRow = old.editRow;
        attrib_data->lastWidthRB = IDD_RBPIXELS;
        attrib_data->fixedColor = COLOR_BLACK;
        attrib_data->lineColor = COLOR_DKGRAY;
        attrib_data->selFG = GetSysColor(COLOR_HIGHLIGHTTEXT);
        attrib_data->selBG = GetSysColor(COLOR_HIGHLIGHT);
        attrib_data->anchorRow = -1L;
        attrib_data->printDouble = TRUE;
        attrib_data->pageNumbers = TRUE;
        attrib_data->showActive = TRUE;
        attrib_data->clairvoyant = TRUE;

        // auto-fill stuff
        attrib_data->autoFill = old.isConnected;
        attrib_data->dataSource = NULL;
        attrib_data->indexedField = NULL;
        attrib_data->indexRef = old.indexRef;
        attrib_data->firstRow = 0L;
        attrib_data->numRows = 0L;
        attrib_data->recordIDs = NULL;
        attrib_data->maxStore = 50;

        break;
        }
      case 1:
        {
        // the version is 1 -- translate the object data to the new version
        // 'ADHOC' changes May 1994
        TATTRIBv1_0 *tmpPtr = (TATTRIBv1_0 *)AObjLockData(theObject, DATA_TABLEATTRIBUTES);
        TATTRIBv1_0  old = *tmpPtr;
        AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);
        oldVersion = old.version;

        AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);
        AObjResizeData(theObject, DATA_TABLEATTRIBUTES, sizeof(TATTRIB));
        attrib_data = (TATTRIB*)AObjLockData(theObject, DATA_TABLEATTRIBUTES);
        attrib_data->version = VERSION_OBJECT;

        // adhoc field added on April 29
        attrib_data->bAdhoc = FALSE;
    
        attrib_data->dataSource = old.dataSource;   // data source object
        attrib_data->indexedField = old.indexedField; // indexed field object
        attrib_data->indexRef = old.indexRef;     // object ID for the row index object
        attrib_data->firstRow = old.firstRow;     // first row loaded in memory
        attrib_data->numRows = old.numRows;     // number of rows loaded in memory
        attrib_data->totalNumRows = old.totalNumRows; // total number of rows for the auto-fill table
        attrib_data->recordIDs = NULL;    // array for loaded rows record IDs
        attrib_data->numVisRows = old.numVisRows;   // max number of visual rows for all visual instances
        attrib_data->maxStore = old.maxStore;     // maximum number of rows to store in memory
        attrib_data->clairvoyant = old.clairvoyant; // clairvoyant selection flag

        AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);
        InitDefColumn();
        attrib_data = (TATTRIB*)AObjLockData(theObject, DATA_TABLEATTRIBUTES);

        break;
        }

      default:
        break;
      }

    // update the columns to the new version
    Tabl_UpdateColumns(theObject, oldVersion, attrib_data->columnLines);

    // update the rows to the new version
    Tabl_UpdateRows(theObject, oldVersion);
    }

  attrib_data->recordIDs = NULL;
  attrib_data->numVisRows = 1L;
  
  AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);
  }

// TABLE_ATTRIBUTES constructor ////////////////////////////////////////////

TABLE_ATTRIBUTES::TABLE_ATTRIBUTES(
  AOBJECTID  objID,
  TATTRIB*  tAttrib)

  {
  int a_LockCount = 0;
  theObject = objID;
  attrib_data = tAttrib;  // store pointer

  long  theSize = AObjGetDataSize(theObject, DATA_TABLEATTRIBUTES);
  long  count = AObjGetDataCount(theObject);

  if(count!=NUM_TABLEDATA)
    AObjSetDataCount(theObject, NUM_TABLEDATA);

  if(theSize==0)
    Initialize();
  else
    {
    // get the version number
    int version = *(int*)AObjLockData(theObject, DATA_TABLEATTRIBUTES);
    AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);

    a_LockCount++;
    if((version == VERSION_OBJECT) && (theSize != sizeof(TATTRIB)))
      {
      a_LockCount--;
      AObjResizeData(theObject, DATA_TABLEATTRIBUTES, sizeof(TATTRIB));
      attrib_data = (TATTRIB*)AObjLockData(theObject, DATA_TABLEATTRIBUTES);
      a_LockCount++;
      attrib_data->clairvoyant = TRUE;
      AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);
      }

    if(version != VERSION_OBJECT)
      {
      int oldVersion = -1;
      switch(version)
        {
        case 0:
          {
          oldTATTRIB  old = *(oldTATTRIB*)AObjLockData(theObject, DATA_TABLEATTRIBUTES);
          oldVersion = old.version;
          AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);

          // the version is 0 -- translate the object data to the new version
          AObjResizeData(theObject, DATA_TABLEATTRIBUTES, sizeof(TATTRIB));
          attrib_data = (TATTRIB*)AObjLockData(theObject, DATA_TABLEATTRIBUTES);
          a_LockCount++;

          attrib_data->version = VERSION_OBJECT;
          attrib_data->columnLines = old.drawColumns;
          attrib_data->rowLines = old.drawRows;
          attrib_data->showTitles = old.showTitles;
          attrib_data->hScroll = old.hScroll;
          attrib_data->vScroll = old.vScroll;
          attrib_data->multiSelections = old.multiSelections;
          attrib_data->canActivate = old.canActivate;
          attrib_data->canResize = TRUE;
          attrib_data->showSelection = TRUE;
          attrib_data->swapColors = FALSE;
          attrib_data->focusColumn = old.focusColumn;
          attrib_data->numOfCols = old.numOfCols;
          attrib_data->fixedColumns = (theSize==sizeof(oldTATTRIB)) ? old.leftPane : 0;
          attrib_data->activeColumn = old.activeColumn;
          attrib_data->activeRow = old.activeRow;
          attrib_data->editColumn = old.editColumn;
          attrib_data->editRow = old.editRow;
          attrib_data->lastWidthRB = IDD_RBPIXELS;
          attrib_data->fixedColor = COLOR_BLACK;
          attrib_data->lineColor = COLOR_DKGRAY;
          attrib_data->selFG = GetSysColor(COLOR_HIGHLIGHTTEXT);
          attrib_data->selBG = GetSysColor(COLOR_HIGHLIGHT);
          attrib_data->anchorRow = -1L;
          attrib_data->printDouble = TRUE;
          attrib_data->pageNumbers = TRUE;
          attrib_data->showActive = TRUE;
          attrib_data->clairvoyant = TRUE;

          // auto-fill stuff
          attrib_data->autoFill = old.isConnected;
          attrib_data->dataSource = NULL;
          attrib_data->indexedField = NULL;
          attrib_data->indexRef = old.indexRef;
          attrib_data->firstRow = 0L;
          attrib_data->numRows = 0L;
          attrib_data->recordIDs = NULL;
          attrib_data->maxStore = 50;
          attrib_data->bAdhoc = FALSE;
          AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);

          // Init the def column attribues
          InitDefColumn();
          }
          break;
        case 1:
          {
          // the version is 1 -- translate the object data to the new version
          TATTRIBv1_0   old = *(TATTRIBv1_0*)AObjLockData(theObject, DATA_TABLEATTRIBUTES);
          oldVersion = old.version;
          AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);

          AObjResizeData(theObject, DATA_TABLEATTRIBUTES, sizeof(TATTRIB));
          attrib_data = (TATTRIB*)AObjLockData(theObject, DATA_TABLEATTRIBUTES);
          attrib_data->version = VERSION_OBJECT;
          // auto-fill stuff
          attrib_data->bAdhoc = FALSE;

          attrib_data->dataSource = old.dataSource;   // data source object
          attrib_data->indexedField = old.indexedField; // indexed field object
          attrib_data->indexRef = old.indexRef;     // object ID for the row index object
          attrib_data->firstRow = old.firstRow;     // first row loaded in memory
          attrib_data->numRows = old.numRows;     // number of rows loaded in memory
          attrib_data->totalNumRows = old.totalNumRows; // total number of rows for the auto-fill table
          attrib_data->recordIDs = NULL;    // array for loaded rows record IDs
          attrib_data->numVisRows = old.numVisRows;   // max number of visual rows for all visual instances
          attrib_data->maxStore = old.maxStore;     // maximum number of rows to store in memory
          attrib_data->clairvoyant = old.clairvoyant; // clairvoyant selection flag

          AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);
          InitDefColumn();

          break;
          }
        default:
          break;
        }

      // convert the columns to the new format
      Tabl_UpdateColumns(theObject, version, attrib_data->columnLines);

      // update the rows to the new version
      Tabl_UpdateRows(theObject, oldVersion);

      // update the local copy
      *tAttrib = *(TATTRIB*)attrib_data;
      }
    else {
      *tAttrib = *(TATTRIB*)AObjLockData(theObject, DATA_TABLEATTRIBUTES);

      AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);
      }
    }

  // make sure the pointer is correct
  attrib_data = tAttrib;
  attrib_data->recordIDs = NULL;
  attrib_data->numVisRows = 1L;
  }


///////////////////////////////////////////////////////////

TABLE_ATTRIBUTES::~TABLE_ATTRIBUTES()
  {
  if (attrib_data->recordIDs)
    AObjUnlockData(theObject, DATA_RECORDIDS);
  }

///////////////////////////////////////////////////////////
void TABLE_ATTRIBUTES::InitDefColumn()
  {
  // translate the data as necessary -- version is the first item in the data
  attrib_data = (TATTRIB*)AObjLockData(theObject, DATA_TABLEATTRIBUTES);

  if (attrib_data == NULL)
    return; 

  HDC         hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
  TEXTMETRIC  tm;
  HFONT       hFont, oldFont;

  // set everything to zero
  FillMemory((char *)&attrib_data->defColumn, sizeof(TCOLS), 0);

  lstrcpy(attrib_data->defColumn.title, COLUMN_DEFAULTTITLE);
  attrib_data->defColumn.bCanEdit         = FALSE;
  attrib_data->defColumn.align            = ALIGN_LEFT;
  attrib_data->defColumn.titleAlign       = ALIGN_TITLECENTER;
  attrib_data->defColumn.colWidth         = COLUMN_DEFAULTWIDTH;

  // title font stuff
  lstrcpy(attrib_data->defColumn.lfTitle.lfFaceName, COLUMN_FONT);
  attrib_data->defColumn.lfTitle.lfWeight = FW_BOLD;
  attrib_data->defColumn.titlePtSize      = COLUMN_TITLEPTSIZE * 10;
  attrib_data->defColumn.lfTitle.lfHeight = - MulDiv(COLUMN_TITLEPTSIZE, GetDeviceCaps(hDC, LOGPIXELSY), 720);
  attrib_data->defColumn.titleFG          = WINDOW_FOREGROUND;
  attrib_data->defColumn.titleBG          = WINDOW_BACKGROUND;

  hFont = AUtlCreateFontIndirect(&attrib_data->defColumn.lfTitle);
  oldFont = (HFONT)SelectObject(hDC, hFont);
  GetTextMetrics(hDC, &tm);
  SelectObject(hDC, oldFont);
  AUtlDeleteFont(hFont);

  attrib_data->defColumn.titleLeading   = tm.tmInternalLeading;
  attrib_data->defColumn.titleAscent    = tm.tmAscent;
  attrib_data->defColumn.titleDescent   = tm.tmDescent;
  attrib_data->defColumn.titleCharWidth = tm.tmAveCharWidth;

  // column font stuff
  lstrcpy(attrib_data->defColumn.lfCell.lfFaceName, COLUMN_FONT);
  attrib_data->defColumn.lfCell.lfWeight  = FW_NORMAL;
  attrib_data->defColumn.cellPtSize       = COLUMN_ITEMPTSIZE * 10;
  attrib_data->defColumn.lfCell.lfHeight  = - MulDiv(COLUMN_ITEMPTSIZE, GetDeviceCaps(hDC, LOGPIXELSY), 720);
  attrib_data->defColumn.cellFG           = WINDOW_FOREGROUND;
  attrib_data->defColumn.cellBG           = WINDOW_BACKGROUND;

  hFont = AUtlCreateFontIndirect(&attrib_data->defColumn.lfCell);
  oldFont = (HFONT)SelectObject(hDC, hFont);
  GetTextMetrics(hDC, &tm);
  SelectObject(hDC, oldFont);
  AUtlDeleteFont(hFont);

  attrib_data->defColumn.cellLeading    = tm.tmInternalLeading;
  attrib_data->defColumn.cellAscent     = tm.tmAscent;
  attrib_data->defColumn.cellDescent    = tm.tmDescent;
  attrib_data->defColumn.cellCharWidth  = tm.tmAveCharWidth;

  // reserved fields
  attrib_data->defColumn.Reserved1        = 0;
  attrib_data->defColumn.Reserved2        = 0;

  DeleteDC(hDC);
  // translate the data as necessary -- version is the first item in the data
  AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);

  }
// TABLE_ATTRIBUTES::Initialize ////////////////////////////////////////////

void  TABLE_ATTRIBUTES::Initialize(
  )

  {
  attrib_data->version          = VERSION_OBJECT;
  attrib_data->columnLines      = FALSE;
  attrib_data->rowLines         = FALSE;
  attrib_data->showTitles       = TRUE;
  attrib_data->hScroll          = FALSE;
  attrib_data->vScroll          = TRUE;
  attrib_data->multiSelections  = TRUE;
  attrib_data->canActivate      = TRUE;
  attrib_data->canResize        = TRUE;
  attrib_data->showSelection    = TRUE;
  attrib_data->swapColors       = FALSE;
  attrib_data->focusColumn      = 0;
  attrib_data->numOfCols        = 0;
  attrib_data->fixedColumns     = 0;
  attrib_data->activeColumn     = -1;
  attrib_data->activeRow        = -1;
  attrib_data->editColumn       = -1;
  attrib_data->editRow          = -1;
  attrib_data->lastWidthRB      = IDD_RBPIXELS;
  attrib_data->titleLeading     = -1;
  attrib_data->titleAscent      = -1;
  attrib_data->titleDescent     = -1;
  attrib_data->cellLeading      = -1;
  attrib_data->cellAscent       = -1;
  attrib_data->cellDescent      = -1;
  attrib_data->fixedColor       = COLOR_BLACK;
  attrib_data->lineColor        = COLOR_DKGRAY;
  attrib_data->selFG            = GetSysColor(COLOR_HIGHLIGHTTEXT);
  attrib_data->selBG            = GetSysColor(COLOR_HIGHLIGHT);
  attrib_data->anchorRow        = -1L;
  attrib_data->printDouble      = TRUE;
  attrib_data->pageNumbers      = TRUE;
  attrib_data->showActive       = TRUE;
  attrib_data->clairvoyant      = TRUE;

  // auto-fill stuff
  attrib_data->autoFill         = FALSE;
  attrib_data->bAdhoc           = FALSE;

  attrib_data->dataSource       = NULL;
  attrib_data->indexedField     = NULL;
  attrib_data->indexRef         = NULL;
  attrib_data->firstRow         = 0L;
  attrib_data->numRows          = 0L;
  attrib_data->recordIDs        = NULL;
  attrib_data->maxStore         = 50;
  attrib_data->numVisRows       = 1L;

  // Init the def column attribues
  InitDefColumn();
  }

// TABLE_ATTRIBUTES::Export ////////////////////////////////////////////////

BOOL  TABLE_ATTRIBUTES::Export(
  )

  {
  if (AUpsfCreateScope("tableSco", 0L)==AUPSF_NOERROR)
    {
    AUpsfExportField("VERSION ", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &attrib_data->version, sizeof(int));
    AUpsfExportField("DRAWCOLU",  0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &attrib_data->columnLines, sizeof(BOOL));
    AUpsfExportField("DRAWROWS",  0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &attrib_data->rowLines, sizeof(BOOL));
    AUpsfExportField("SHOWTITL",  0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &attrib_data->showTitles, sizeof(BOOL));
    AUpsfExportField("HSCROL  ", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &attrib_data->hScroll, sizeof(BOOL));
    AUpsfExportField("VSCROLL ", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &attrib_data->vScroll, sizeof(BOOL));
    AUpsfExportField("MULTISEL", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &attrib_data->multiSelections, sizeof(BOOL));
    AUpsfExportField("CANACTIV", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &attrib_data->canActivate, sizeof(BOOL));
    AUpsfExportField("CANRESIZ", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &attrib_data->canResize, sizeof(BOOL));
    AUpsfExportField("SHOWSLTN", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &attrib_data->showSelection, sizeof(BOOL));
    AUpsfExportField("SWAPCOLR", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &attrib_data->swapColors, sizeof(BOOL));
    AUpsfExportField("ISCONNEC", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &attrib_data->autoFill, sizeof(BOOL));
    AUpsfExportField("INDEXREF", 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &attrib_data->indexRef, sizeof(AOBJECTID));
    AUpsfExportField("ACTIVECO", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &attrib_data->activeColumn, sizeof(int));
    AUpsfExportField("ACTIVERO", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &attrib_data->activeRow, sizeof(int));
    AUpsfExportField("LEFTPANE", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &attrib_data->fixedColumns, sizeof(int));
    AUpsfExportField("LINECOLR", 0L, AUPSF_NATV_LONG, AUPSF_UNIV_INTEGER, &attrib_data->lineColor, sizeof(COLORREF));
    AUpsfExportField("TITLECLR", 0L, AUPSF_NATV_LONG, AUPSF_UNIV_INTEGER, &attrib_data->fixedColor, sizeof(COLORREF));
    AUpsfExportField("SELFGCLR", 0L, AUPSF_NATV_RGBCOLOR, AUPSF_UNIV_INTEGER, &attrib_data->selFG, sizeof(COLORREF));
    AUpsfExportField("SELBGCLR", 0L, AUPSF_NATV_RGBCOLOR, AUPSF_UNIV_INTEGER, &attrib_data->selBG, sizeof(COLORREF));
    AUpsfExportField("DBLLINES", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &attrib_data->printDouble, sizeof(BOOL));
    AUpsfExportField("PAGENUMS", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &attrib_data->pageNumbers, sizeof(BOOL));
    AUpsfExportField("SHOWACTV", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &attrib_data->showActive, sizeof(BOOL));
    AUpsfExportField("CLAIRVOY", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &attrib_data->clairvoyant, sizeof(BOOL));
    AUpsfExportField("DATASRC ", 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &attrib_data->dataSource, sizeof(AOBJECTID));
    AUpsfExportField("IDXFIELD", 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &attrib_data->indexedField, sizeof(AOBJECTID));
    AUpsfExportField("MAXSTORE", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &attrib_data->maxStore, sizeof(int));

    // For Adhoc Table --> def Column attributes
    if (attrib_data->version == VERSION_OBJECT)
      {
      HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
      AUpsfExportField("ISADHOC", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &attrib_data->bAdhoc, sizeof(BOOL));
      COLUMN::ExportFont("DEFTFONTATR", 0L, &attrib_data->defColumn.lfTitle, hDC);
      COLUMN::ExportFont("DEFCFONTATR", 0L, &attrib_data->defColumn.lfCell, hDC);

      BOOL true = TRUE;
      if(attrib_data->defColumn.titleFG>0x00FFFFFFL)
        AUpsfExportField("TDEFFGCL", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &true, sizeof(BOOL));
      else
        AUpsfExportField("TITLCOLR", 0L, AUPSF_NATV_RGBCOLOR, AUPSF_UNIV_COLOR, &attrib_data->defColumn.titleFG, sizeof(COLORREF));
      if(attrib_data->defColumn.titleBG>0x00FFFFFFL)
        AUpsfExportField("TDEFBGCL", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &true, sizeof(BOOL));
      else
        AUpsfExportField("TBGCOLOR", 0L, AUPSF_NATV_RGBCOLOR, AUPSF_UNIV_COLOR, &attrib_data->defColumn.titleBG, sizeof(COLORREF));
      if(attrib_data->defColumn.cellFG>0x00FFFFFFL)
        AUpsfExportField("CDEFFGCL", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &true, sizeof(BOOL));
      else
        AUpsfExportField("ITEMCOLR", 0L, AUPSF_NATV_RGBCOLOR, AUPSF_UNIV_COLOR, &attrib_data->defColumn.cellFG, sizeof(COLORREF));
      if(attrib_data->defColumn.cellBG>0x00FFFFFFL)
        AUpsfExportField("CDEFBGCL", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &true, sizeof(BOOL));
      else
        AUpsfExportField("CBGCOLOR", 0L, AUPSF_NATV_RGBCOLOR, AUPSF_UNIV_COLOR, &attrib_data->defColumn.cellBG, sizeof(COLORREF));

      AUpsfExportField("WIDTH   ", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &attrib_data->defColumn.colWidth, sizeof(int));
      AUpsfExportField("BCANEDIT", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &attrib_data->defColumn.bCanEdit, sizeof(BOOL));
      AUpsfExportField("ALIGN   ", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &attrib_data->defColumn.align, sizeof(int));
      AUpsfExportField("TITLEAGN", 0L, AUPSF_NATV_CHAR, AUPSF_UNIV_INTEGER, &attrib_data->defColumn.titleAlign, sizeof(char));

      DeleteDC(hDC);
      }

    AUpsfCloseScope();
    }
  else
    return FALSE;

  return TRUE;
  }

// TABLE_ATTRIBUTES::Import ////////////////////////////////////////////////

BOOL  TABLE_ATTRIBUTES::Import(
  )

  {
  if (AUpsfOpenScope("tableSco", 0L)==AUPSF_NOERROR) 
    {
    AUpsfImportField("DRAWCOLU", 0L, AUPSF_NATV_BOOL, &attrib_data->columnLines, sizeof(BOOL));
    AUpsfImportField("DRAWROWS", 0L, AUPSF_NATV_BOOL, &attrib_data->rowLines, sizeof(BOOL));
    AUpsfImportField("SHOWTITL", 0L, AUPSF_NATV_BOOL, &attrib_data->showTitles, sizeof(BOOL));
    AUpsfImportField("HSCROL  ", 0L, AUPSF_NATV_BOOL, &attrib_data->hScroll, sizeof(BOOL));
    AUpsfImportField("VSCROLL ", 0L, AUPSF_NATV_BOOL, &attrib_data->vScroll, sizeof(BOOL));
    AUpsfImportField("MULTISEL", 0L, AUPSF_NATV_BOOL, &attrib_data->multiSelections, sizeof(BOOL));
    AUpsfImportField("CANACTIV", 0L, AUPSF_NATV_BOOL, &attrib_data->canActivate, sizeof(BOOL));
    AUpsfImportField("CANRESIZ", 0L, AUPSF_NATV_BOOL, &attrib_data->canResize, sizeof(BOOL));
    AUpsfImportField("SHOWSLTN", 0L, AUPSF_NATV_BOOL, &attrib_data->showSelection, sizeof(BOOL));
    AUpsfImportField("SWAPCOLR", 0L, AUPSF_NATV_BOOL, &attrib_data->swapColors, sizeof(BOOL));
    AUpsfImportField("ISCONNEC", 0L, AUPSF_NATV_BOOL, &attrib_data->autoFill, sizeof(BOOL));
    AUpsfImportField("INDEXREF", 0L, AUPSF_NATV_OBJECTREF, &attrib_data->indexRef, sizeof(AOBJECTID));
    AUpsfImportField("ACTIVECO", 0L, AUPSF_NATV_INT, &attrib_data->activeColumn, sizeof(int));
    AUpsfImportField("ACTIVERO", 0L, AUPSF_NATV_INT, &attrib_data->activeRow, sizeof(int));
    AUpsfImportField("LEFTPANE", 0L, AUPSF_NATV_INT, &attrib_data->fixedColumns, sizeof(int));
    AUpsfImportField("LINECOLR", 0L, AUPSF_NATV_LONG, &attrib_data->lineColor, sizeof(COLORREF));
    AUpsfImportField("TITLECLR", 0L, AUPSF_NATV_LONG, &attrib_data->fixedColor, sizeof(COLORREF));
    AUpsfImportField("SELFGCLR", 0L, AUPSF_NATV_LONG, &attrib_data->selFG, sizeof(COLORREF));
    AUpsfImportField("SELBGCLR", 0L, AUPSF_NATV_LONG, &attrib_data->selBG, sizeof(COLORREF));
    AUpsfImportField("DBLLINES", 0L, AUPSF_NATV_BOOL, &attrib_data->printDouble, sizeof(BOOL));
    AUpsfImportField("PAGENUMS", 0L, AUPSF_NATV_BOOL, &attrib_data->pageNumbers, sizeof(BOOL));
    AUpsfImportField("SHOWACTV", 0L, AUPSF_NATV_BOOL, &attrib_data->showActive, sizeof(BOOL));
    AUpsfImportField("CLAIRVOY", 0L, AUPSF_NATV_BOOL, &attrib_data->clairvoyant, sizeof(BOOL));
    AUpsfImportField("DATASRC ", 0L, AUPSF_NATV_OBJECTREF, &attrib_data->dataSource, sizeof(AOBJECTID));
    AUpsfImportField("IDXFIELD", 0L, AUPSF_NATV_OBJECTREF, &attrib_data->indexedField, sizeof(AOBJECTID));
    AUpsfImportField("MAXSTORE", 0L, AUPSF_NATV_INT, &attrib_data->maxStore, sizeof(int));

    if (attrib_data->version == VERSION_OBJECT)
      {
      HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
      AUpsfImportField("ISADHOC", 0L, AUPSF_NATV_BOOL, &attrib_data->bAdhoc, sizeof(BOOL));
      COLUMN::ImportFont("DEFTFONTATR", 0L, &attrib_data->defColumn.lfTitle, hDC);
      COLUMN::ImportFont("DEFCFONTATR", 0L, &attrib_data->defColumn.lfCell, hDC);

      // store the point sizes in decipoints
      attrib_data->defColumn.titlePtSize = - MulDiv(attrib_data->defColumn.lfTitle.lfHeight, 720, GetDeviceCaps(hDC, LOGPIXELSY));
      attrib_data->defColumn.cellPtSize = - MulDiv(attrib_data->defColumn.lfCell.lfHeight, 720, GetDeviceCaps(hDC, LOGPIXELSY));

      // ignore errors for the colors -- if error, use the default!
      AUpsfImportField("TITLCOLR", 0L, AUPSF_NATV_RGBCOLOR, &attrib_data->defColumn.titleFG, sizeof(COLORREF));
      AUpsfImportField("ITEMCOLR", 0L, AUPSF_NATV_RGBCOLOR, &attrib_data->defColumn.cellFG, sizeof(COLORREF));
      AUpsfImportField("TBGCOLOR", 0L, AUPSF_NATV_RGBCOLOR, &attrib_data->defColumn.titleBG, sizeof(COLORREF));
      AUpsfImportField("CBGCOLOR", 0L, AUPSF_NATV_RGBCOLOR, &attrib_data->defColumn.cellBG, sizeof(COLORREF));

      AUpsfImportField("WIDTH   ", 0L, AUPSF_NATV_INT, &attrib_data->defColumn.colWidth, sizeof(int));
      AUpsfImportField("BCANEDIT", 0L, AUPSF_NATV_BOOL, &attrib_data->defColumn.bCanEdit , sizeof(BOOL));
      AUpsfImportField("ALIGN   ", 0L, AUPSF_NATV_INT, &attrib_data->defColumn.align , sizeof(int));
      AUpsfImportField("TITLEAGN", 0L, AUPSF_NATV_CHAR, &attrib_data->defColumn.titleAlign, sizeof(char));

      DeleteDC(hDC);
      }

    AUpsfCloseScope();
    }
  else
    return FALSE;

  attrib_data->numOfCols = (int)AUpsfFieldCount("columnSc");

  return TRUE;
  }

// TABLE_ATTRIBUTES::GetRecordID ///////////////////////////////////////////

long TABLE_ATTRIBUTES::GetRecordID(
  long  row)

  {
  long rval = 0;
  int offset = (int)(row - attrib_data->firstRow);
  if(offset<0 || offset>=attrib_data->numRows)
    {
    RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
    ROW*          pRow = rtime->GetRow();
    BOOL          dbAccessed = FALSE;
  
    pRow->GetCell(row, 0, FALSE, &dbAccessed);

    if(dbAccessed)
      AEvtPostSignalAtMark(theObject, SIGNAL_TABLEAUTOFILLED);

    rval = pRow->GetLastRecID();
    }
  else
    {
    attrib_data->recordIDs = (long*)AObjLockData(theObject, DATA_RECORDIDS);
    rval = attrib_data->recordIDs[offset];
    AObjUnlockData(theObject, DATA_RECORDIDS);
    attrib_data->recordIDs = NULL;
    }

  return rval;
  }

// TABLE_ATTRIBUTES::SetRecordID ///////////////////////////////////////////

void TABLE_ATTRIBUTES::SetRecordID(
  long  row,
  long  ID)

  {
  int   offset = (int)(row - attrib_data->firstRow);
  AObjResizeData(theObject, DATA_RECORDIDS, sizeof(long)*attrib_data->numRows);
  attrib_data->recordIDs = (long*)AObjLockData(theObject, DATA_RECORDIDS);
  if(attrib_data->recordIDs && (offset>=0 && offset<attrib_data->numRows))
    attrib_data->recordIDs[offset] = ID;
  if (attrib_data->recordIDs)
    {
    AObjUnlockData(theObject, DATA_RECORDIDS);
    attrib_data->recordIDs = NULL;
    }
  }

// TABLE_ATTRIBUTES::SetNumVisRows /////////////////////////////////////////

void TABLE_ATTRIBUTES::SetNumVisRows(
  long  num)

  {
  if(num>attrib_data->numVisRows)
    attrib_data->numVisRows = num;

  if(attrib_data->maxStore < (int)(2*attrib_data->numVisRows + 3))
    attrib_data->maxStore = (int)(2*attrib_data->numVisRows + 3);
  }

////////////////////////////////////////////////////////////////////////////
