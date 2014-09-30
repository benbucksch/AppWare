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
// FILE:  "tbl_mthd.cpp"
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
#include  "op_dview.h"
#include  <o_wnd.h>
#include  <intdebug.h>

#include  "tabl_row.h"

extern BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/  adhocSetColumnCount(
  AOBJECTID  tableObj,
  int       colCount);

extern int cxVS;
extern int cyVS;
extern int cxHS;
extern int cyHS;

#include  "tabl_hdr.h"
#include  "o_table.h"
#include  "table.h"

// global variables ////////////////////////////////////////////////////////

extern GLOBAL G;

extern long Tbl_AdjustNewRect(RECT *rect,LONG adjustFlag);

/**************************************************************************
WIP METHODS
***************************************************************************/
void ALMAPI TABLE::DataChanged(long details, long modifiers, long variation)
  {
  AOBJECTID    tableObject = m_oiObject;

  if(!modifiers)
      {
      RECT tmpRect;
      DestroyEditControl();
      tmpRect = m_rcItem;
      OWndLPtoDP(m_oiParent,(LPPOINT) &tmpRect, 2);
      UpdateRects(&tmpRect, TRUE);
      DrawScrollBars(NULL, StatusFlag());
      InvalidateTableRect();
      return;
      }

  if(modifiers & CHANGED_NUMCOLUMNS)
      {
      RECT tmpRect = m_rcItem;
      OWndLPtoDP(m_oiParent,(LPPOINT) &tmpRect, 2);

      DestroyEditControl();
      UpdateColumns();
      CheckFirstColumn();
      UpdateRects(&tmpRect, TRUE);
      UpdateHScrollArea();
      if(TotalNumCols()==0)
        NumRowsChanged();
      else
        InvalidateTableRect();
      }

  if(modifiers & CHANGED_COLUMNWIDTH)
      {
      UpdateHScrollArea();
      DestroyEditControl();
      InvalidateTableRect();
      }

  if(modifiers & CHANGED_COLUMNSELECTION)
      InvalidateTitleRect();

  if(modifiers & CHANGED_SCROLLTOSELECTION)
      ScrollToSelection();

  if(modifiers & CHANGED_NUMROWS)
      NumRowsChanged();

  if(modifiers & CHANGED_FULLREDRAW)
      InvalidateTableRect();

  if(modifiers & CHANGED_AUTOUPDATE)
      AutoUpdate();

  if(modifiers & CHANGED_KILLEDIT)
      EscKey();
  I_UNUSED(tableObject);
  I_UNUSED(details);
  I_UNUSED(variation);

  return; // don't even bother checking the other flags (none can be set)
  }

/*************************************************************************
ParentOpened
*************************************************************************/
void ALMAPI TABLE::ParentOpened(void)
  {
  // Put this code in the ParentShown function
  // store the DP rect

  // The following code will Init all the TextMetric info for diff columns
  // I have removed this code from the StartUp routine
  // SKJ May 13, 1994 
  I_ASSERT(m_oiParent);

  // convert the table to the correct version (if necessary)
  TABLE     table(m_oiObject);

  // initialize the column font metrics
  // This is done in the constructor

  table.CreateFonts();    // Create Runtime Fonts for the table
  table.InitFontMetrics();

  RUNTIMEINFO *rTime = (RUNTIMEINFO *)AObjGetRuntimeInfo(m_oiObject);
  rTime->m_fParentOpen = TRUE;

  visRect = m_rcItem;
  OWndLPtoDP(m_oiParent, (LPPOINT)&visRect, 2);
  CreateScrollBars(m_hwndParent, TRUE);
  m_hwndItem = hTable; // initialize the table window handle
  }

/************************************************************************
ParentClosing
************************************************************************/
void ALMAPI TABLE::ParentClosing(void)
  {
  // Put this code in the P
  DestroyScrollBars();
  m_hwndItem = hTable; // initialize the table window handle

  // Destroy the fonts created at ParentOpened()
  TABLE     table(m_oiObject);
  table.DestroyFonts();

  RUNTIMEINFO *rTime = (RUNTIMEINFO *)AObjGetRuntimeInfo(m_oiObject);
  rTime->m_fParentOpen = FALSE;
  }


//    Destroy the table visual instance that was created with WIA_STARTUP //
void ALMAPI TABLE::Destroy()
  {
  delete this;
  }


/*********************************************************
*
*
*********************************************************/
/////////////////////////////////////////////////////////////
// The AdjustNewRect Functions
/////////////////////////////////////////////////////////////
long ALMAPI TABLE::AdjustNewRect(
  RECT*                   newRect,
  LONG                    adjustFlag)
  {
  long                    rVal;
  rVal = OpHwndWndItem::AdjustNewRect(newRect, adjustFlag);
  visRect = *newRect;
  OWndLPtoDP(m_oiParent, (LPPOINT)&visRect, 2);
  return rVal;
  }

/////////////////////////////////////////////////////////////
// The Move Functions
/////////////////////////////////////////////////////////////

void ALMAPI TABLE::Move(LPRECT newRect)
  {
  if (EqualRect(newRect, &(m_rcItem)) == FALSE)
      {
      OWndMoveWindow(this, newRect, TRUE);
      m_rcItem = *newRect;
      visRect = *newRect;
      // store the DP rect
      OWndLPtoDP(m_oiParent, (LPPOINT)&visRect, 2);
      UpdateRects(&visRect, TRUE); // SKJ Mar 28, 1994
      }
  }


// void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tabl_activateWindItem(pAEvtInfo theEvent, pAObjMessage theSystem);
//    Draw the solid frame around the table ////////////////////////////////
long ALMAPI TABLE::Activate(int causedBy, BOOL mustKeep)
  {
  long statusFlag = StatusFlag();
  AEvtPostSignalAtMark(m_oiObject, SIGNAL_TABLEACTIVATED);
  // SKJ, set the Active bit to check during DRAW call
  statusFlag |= SF_ACTIVE;
  SetStatusFlag(statusFlag);

  if (OWndIsItemVisible(this))
    {
    statusFlag |= SF_VISIBLE;
    DrawFrame(NULL, StatusFlag());
    }

  // SKJ, set the focus.  Oct 26, 1993
  SetFocus(m_hwndItem);
  I_UNUSED(mustKeep);
  I_UNUSED(causedBy);
  return OWND_ACTIVATION_KEPT;
  }
   
// void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tabl_deactivateWindItem(pAEvtInfo theEvent, pAObjMessage theSystem);
//    Draw the blank frame around the table ////////////////////////////////
void ALMAPI TABLE::Deactivated(void)
  {
  long statusFlag = StatusFlag();
  // SKJ, set the Active bit to check during DRAW call
  statusFlag &= ~SF_ACTIVE;
  // Destroy or DrawFrame may have changed the status bits
  SetStatusFlag(statusFlag);

  AEvtPostSignalAtMark(m_oiObject, SIGNAL_TABLEDEACTIVATED);
  if(OWndIsItemVisible(this)) {
    DrawFrame(NULL, StatusFlag());
    }                          

  DestroyEditControl();
  }

// Tabl_windItemMouseEntry /////////////////////////////////////////////////
void ALMAPI TABLE::MouseEntry(BOOL inside)
  {
  BOOL        enter = (BOOL)inside;

  if(enter)
    {
    if(ShowTitles() && CanResize())
      {
      StartTracking(TRACK_TABLEACTIVE);
      TrackCursor(0, 0L);
      }
    }
  else
    {
    if(ShowTitles() && CanResize())
      {
      TrackCursor(0, 0L);
      StopTracking(TRACK_TABLEACTIVE);
      }
    }
  }

////////////////////////////////////////////////////////////////////////
// Draw : Called from WM_PAINT: Tbl_WndProc
///////////////////////////////////////////////////////////////////////
void ALMAPI TABLE::Tabl_windItemDraw(HDC hDC, RECT *clipRect)
  {
  RECT tmpRect;
  if (OWndIsItemVisible(this)) {
    SetStatusFlag(StatusFlag()|SF_VISIBLE);
    DrawFrame(NULL, StatusFlag());
    }
  else {
    long status = StatusFlag();
    status &= ~SF_VISIBLE;
    SetStatusFlag(status);
    }
  tmpRect = m_rcItem;
  OWndLStoDS(m_oiParent, (LPPOINT)&tmpRect, 2);
  tmpRect = *clipRect;
  Draw(hDC, &tmpRect, FALSE, StatusFlag()); // drawRect
  I_UNUSED(clipRect);
  }

////////////////////////////////////////////////////////////////////////////
// void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tabl_windItemPrintRoutine(pAEvtInfo theEvent, pAObjMessage theSystem);
////////////////////////////////////////////////////////////////////////////
void  ALMAPI TABLE::Print(HDC hdc, LPRECT rect, int pclPrinter)
  {
  WIPR_Draw(hdc, rect, pclPrinter);
  }

// Tabl_editCut  ///////////////////////////////////////////////////////////

void ALMAPI TABLE::Cut()
  {
  tCut();
  }

  // Tabl_editCopy ///////////////////////////////////////////////////////////
void ALMAPI TABLE::Copy()
  {
  tCopy();
  }

  // Tabl_editPaste //////////////////////////////////////////////////////////
void ALMAPI TABLE::Paste()
  {
  tPaste();
  }
  // Tabl_editUndo ///////////////////////////////////////////////////////////
void ALMAPI TABLE::Undo()
  {
  
  tUndo();
  }

/*************************************************************************
*
*
// These methods are called from the Tabl_WndProc
*
*
*************************************************************************/
// Tabl_windItemDoubleClick ////////////////////////////////////////////////

void CALLBACK /*!!PORT!! was _far_ _pascal_*/ TABLE::Tabl_windItemDoubleClick(
  WPARAM wParam, LPARAM lParam)
  {
  DoubleClicked(m_hwndItem, lParam);
  I_UNUSED(wParam);
  }

// Tabl_windItemHit ////////////////////////////////////////////////////////

void CALLBACK /*!!PORT!! was _far_ _pascal_*/ TABLE::Tabl_windItemHit(
  WPARAM wParam, LPARAM lParam)
  {
  Clicked(m_hwndItem, wParam, lParam);
  }

// Tabl_windItemHScroll ////////////////////////////////////////////////////

void CALLBACK /*!!PORT!! was _far_ _pascal_*/ TABLE::Tabl_windItemHScroll(
  WPARAM wParam, LPARAM lParam)
  {
  HScroll(wParam,lParam);
  }

// Tabl_windItemVScroll ////////////////////////////////////////////////////

void CALLBACK /*!!PORT!! was _far_ _pascal_*/ TABLE::Tabl_windItemVScroll(
  WPARAM wParam, LPARAM lParam)
  {
  VScroll(wParam, lParam);
  }


// Tabl_windItemMouseMove //////////////////////////////////////////////////

void CALLBACK /*!!PORT!! was _far_ _pascal_*/ TABLE::Tabl_windItemMouseMove(
  WPARAM wParam, LPARAM lParam)
  {
  TrackCursor(wParam, lParam); // pwr 7/14
  }

// Tabl_windMouseUp ////////////////////////////////////////////////////////

void CALLBACK /*!!PORT!! was _far_ _pascal_*/ TABLE::Tabl_windMouseUp(
  WPARAM wParam, LPARAM lParam)
  {
  MouseUp(lParam);
  I_UNUSED(wParam);
  }

// Tabl_windItemKey ////////////////////////////////////////////////////////

void CALLBACK /*!!PORT!! was _far_ _pascal_*/ TABLE::Tabl_windItemKey(
  WPARAM wParam, LPARAM lParam)
  {
  int messageDispatched;
  int message = WM_KEYDOWN;
  KeyDown(message, wParam, lParam,&messageDispatched);
  }

/***************************************************************************
These methods are called by the shell to handle various events
***************************************************************************/
// Tabl_startupEvent ///////////////////////////////////////////////////////
//    1. Make sure all tables are the current version //////////////////////
//    2. Initialize the font metrics for the column fonts //////////////////
//    3. Initialize the double click speed

void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tabl_startupEvent(
  pAEvtInfo /*theEvent*/,
  pAObjMessage        /*theSystem*/)

  {
  AOBJECTID  tableID;
  long      tableKey = AOBJ_GETFIRSTKEY;

  while((tableID = AObjGetNextObject(OTYPE_TABLE, &tableKey))!=NULL)
    {
    // convert the table to the correct version (if necessary) 
    TABLE     table(tableID);

    // initialize the column font metrics
    // This is done in the constructor
    // table.InitFontMetrics();

    if(table.AutoFill())
      {
      // register the data source object for object-changes
      AObjRegisterOwner(tableID, table.DataSource(), 0L);

      // if an autofill table has an adhoc Data Source,
      // then inform the Source
      if (table.AdHoc())
        {

        // post DVIEWSET only to the Data source object
        AEvtInfo  event;
        event.sourceObject = tableID;
        event.targetObject = table.DataSource();
        event.when         = AAppGetTime();
	event.how          = AEVT_DIRECT | AEVT_ATONCE;
        event.what         = AEVENT_DVIEWSET;
        event.modifiers    = 0;
        AEvtPost(&event);

        // AEvtPostStandard(table.DataSource(), AEVENT_DVIEWSET,0, AEVT_ATONCE);
        }
      }

    // clear the number of visible rows
    table.ClearNumVisRows();
    table.SetTotalNumRows(0L);

    }

  // update the G.DblClkSpeed
  G.DblClkSpeed = MulDiv(GetProfileInt("windows", "DoubleClickSpeed", 550), 60, 1000);
  }

// Tabl_printItemDrawRoutine ///////////////////////////////////////////////
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tabl_printItemDrawRoutine(
  pAEvtInfo theEvent,
  pAObjMessage        /*theSystem*/)

  {
  AOBJECTID    tableObject = theEvent->targetObject;
  TABLE       table(tableObject);
  PIDR_BLOCK* pidr = (PIDR_BLOCK*)theEvent->details;

  table.PIDR_Draw(pidr);
  }

// Tabl_trackCursor ////////////////////////////////////////////////////////

void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tabl_trackCursor(
  pAEvtInfo theEvent,
  pAObjMessage        /*theSystem*/)

  {
  TABLE*  table = (TABLE*)theEvent->details;

  I_UNUSED(table);
  }


// Tabl_doubleClickWait ////////////////////////////////////////////////////
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tabl_doubleClickWait(
  pAEvtInfo theEvent,
  pAObjMessage        /*theSystem*/)

  {
  TABLE*  table = (TABLE*)theEvent->details;

  table->DoubleClickWait();
  }

// Tabl_osEnvChanged ///////////////////////////////////////////////////////


void CALLBACK /*!!PORT!! was _far_ _pascal_*/  OSEnvChanged( // Tbl_osEnvChanged
  int wMsg) 
  {
  if(wMsg != WM_WININICHANGE) // preferencesChanged)
    return;

  // update the G.DblClkSpeed
  G.DblClkSpeed = MulDiv(GetProfileInt("windows", "DoubleClickSpeed", 550), 60, 1000);
  }

// Tabl_objectChanged //////////////////////////////////////////////////////

void CALLBACK /*!!PORT!! was _far_ _pascal_*/  Tabl_objectChanged(
  pAEvtInfo theEvent,
  pAObjMessage        /*theSystem*/)

  {
  AOBJECTID       targetID = theEvent->targetObject;
  AOBJECTID       tableID;
  AObjOwnerInfo  ownerInfo;

  targetID = theEvent->targetObject;

  // Get each auto-fill table that owns this object
  ownerInfo.key = AOBJ_GETFIRSTKEY;
  while (AObjGetNextOwner(OTYPE_TABLE, targetID, &ownerInfo))
    {
    tableID = ownerInfo.ownerID;
    TABLE table(tableID);

    if(table.AutoFill())
      {
      if ((table.AdHoc()) || (table.DataSource()==targetID))
        table.UpdateAutoTable(0);
      }
    }
  }

////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/  adhocSetColumnCount(
  AOBJECTID  tableObj,
  int       colCount)

  {
  COLUMN        col(tableObj);
  int           numCol = col.NumColumns();
  int           oldnCol = numCol;
  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(tableObj);
  ROW*          row = rtime->GetRow();
  TABLE         table(tableObj);

  /**********************/
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
      table.SetNumColumns(numCol); // set col count SKJ
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
      table.SetNumColumns(++numCol);   // set the col count SKJ
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

/*
// The Following Methods are called to handle adhoc-table Data Query and acquisition
*/
// Tabl_///////////////////////////////////////////////////////


extern "C" void ALMAPI
Tabl_AdhocViewChanged(
  pAEvtInfo         theEvent,
  pAObjMessage      theSystem)

  {
  AOBJECTID  tableID;
  tableID = theEvent->targetObject;
  if ( AObjCheckType(tableID, OTYPE_TABLE) == TRUE)
    {
    // convert the table to the correct version (if necessary)
    TABLE     table(tableID);
    int       i;
    // Call Method on the data Source to get the data.

    if (table.DataSource())
      {
      ODViewColDesc  *dataDesc = (ODViewColDesc  *)(theEvent->details);

      table.DestroyAllEditControls();

      RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(tableID);
      rtime->GetRow()->Clear();

      adhocSetColumnCount(tableID, dataDesc->colCount); // mimics tblSetColumnCount
      
      for (i = 0; i < dataDesc->colCount; i++)
        {
        int sLen;
	HPSTR tmpStr = (HPSTR)dataDesc->colNames;
	int   j;
        for (j = 0; j < i; j++)
          {
	  sLen = (int)lstrlen(tmpStr);
          tmpStr = (tmpStr + ++sLen);
          }
        HPSTR colName = (HPSTR)tmpStr;
        tblColumnTitle(tableID,(LPSTR)colName,i);
        }

      // post object changed.
      AEvtInfo  event;
      event.sourceObject = tableID;
      event.targetObject = table.DataSource();
      event.when         = AAppGetTime();
      event.how          = AEVT_KILLMODIFIERS | AEVT_KILLDUPLICATES | AEvtGetObjectChangedPriority(AEVT_ATTAIL);
      event.what         = AEVENT_OBJECTCHANGED;
      event.modifiers    = CHANGED_FULLREDRAW;
      event.details      = dataDesc->rowCount;
      AEvtPost(&event);
      }
    }
  I_UNUSED(theSystem);
  I_UNUSED(theEvent);
  }
