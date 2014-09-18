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
// FILE:  "tbl_clmn.cpp"
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
#include  <intdebug.h>
#include  <op_wnd.h>
#include  <o_wnd.h>

#include  "table.h"
#include  "tabl_hdr.h"
#include  "tbl_clmn.h"
#include  "tabl_row.h"

// Tabl_UpdateColumns //////////////////////////////////////////////////////

void Tabl_UpdateColumns(
  OBJECTID  theObject,
  int       oldVersion,
  BOOL      colLines)

  {
  switch(oldVersion)
    {
    case 0:
      {
      long  theSize = AObjGetDataSize(theObject, DATA_COLUMN);
      int   numColumns = (int)(theSize / sizeof(oldTCOLS));
      if(numColumns<=0)
        return; // no columns! -- no conversion!

      HGLOBAL   hCols = GlobalAlloc(GMEM_MOVEABLE, (DWORD)numColumns*sizeof(TCOLS));
      TCOLS*    colPtr = (TCOLS*)GlobalLock(hCols);
      if(!colPtr)
        return;
      oldTCOLS* old = (oldTCOLS*)AObjLockData(theObject, DATA_COLUMN);

      for(int i=0; i<numColumns; i++)
        {
        // do the conversions for each column
        hmemcpy(&colPtr[i].title, &old[i].title, COLUMN_TITLEBUFSIZE);
        colPtr[i].objRef        = old[i].theRef;
        colPtr[i].bCanEdit      = old[i].bcanEdit;
        colPtr[i].align         = (old[i].align==309) ? ALIGN_LEFT : ((old[i].align==310) ? ALIGN_RIGHT : ALIGN_CENTER);
        colPtr[i].colWidth      = (old[i].width==0) ? 0 : old[i].width + colLines ? 9 : 10;
        hmemcpy(&colPtr[i].lfTitle, &old[i].lfTitle, sizeof(LOGFONT));
        colPtr[i].titlePtSize   = old[i].TiPtSize;
        colPtr[i].titleFG       = old[i].tColor;
        colPtr[i].titleBG       = WINDOW_BACKGROUND;
        colPtr[i].titleCharWidth= -1;
        colPtr[i].titleLeading  = -1;
        colPtr[i].titleAscent   = -1;
        colPtr[i].titleDescent  = -1;
        hmemcpy(&colPtr[i].lfCell, &old[i].lfColumn, sizeof(LOGFONT));
        colPtr[i].cellPtSize    = old[i].CiPtSize;
        colPtr[i].cellFG        = old[i].itemColor;
        colPtr[i].cellBG        = WINDOW_BACKGROUND;
        colPtr[i].cellCharWidth = -1;
        colPtr[i].cellLeading   = -1;
        colPtr[i].cellAscent    = -1;
        colPtr[i].cellDescent   = -1;
        colPtr[i].titleAlign    = (char)(colPtr[i].align-308);
        colPtr[i].Reserved1     = 0;
        colPtr[i].Reserved2     = 0;
        }
      if (old)
        AObjUnlockData(theObject, DATA_COLUMN);
      TCOLS*  newCols;
      AObjResizeData(theObject, DATA_COLUMN, (long)numColumns*sizeof(TCOLS));
      newCols = (TCOLS*)AObjLockData(theObject, DATA_COLUMN);
      if(newCols)
        {
        // copy the updated data directly to the object data
        hmemcpy(newCols, colPtr, (long)numColumns*sizeof(TCOLS));
        AObjUnlockData(theObject, DATA_COLUMN);
        }
      if (colPtr)
        {
        GlobalUnlock(hCols);
        GlobalFree(hCols);
        }
      break;
      }

    default:  // conversion not supported
      break;
    }
  }

// COLUMN constructor //////////////////////////////////////////////////////

COLUMN::COLUMN(
  OBJECTID  objID)

  {
  theObject = objID;
  numColumns = (int)AObjGetDataSize(theObject, DATA_COLUMN) / sizeof(TCOLS);

  if(numColumns > 0)
    {
    column_data = (TCOLS*)AObjLockData(theObject, DATA_COLUMN);
    if(column_data[0].titleAlign==0)
      {
      for(int i=0; i<numColumns; i++)
        column_data[i].titleAlign = column_data[i].align - 308;
      }
    AObjUnlockData(theObject, DATA_COLUMN);

    }
  else
    column_data = NULL;
  }

// COLUMN::InitColumn ///////////////////////////////////////////////////////

void COLUMN::InitColumn(
  OBJECTID  objID)

  {
  theObject = objID;
  numColumns = (int)AObjGetDataSize(theObject, DATA_COLUMN) / sizeof(TCOLS);

  if(numColumns > 0) {
    
    column_data = (TCOLS*)AObjLockData(theObject, DATA_COLUMN);
    // SKJ-> table should never keep a ptr to its datas
    // This is being called from a VI method (which may be at runtime
    // or at layout time). But at layout time an object may revert to EDIT mode
    // at which point the COLUMN DATAS may get reinit.  So
    // keeping a lock on datas is never desired.
    AObjUnlockData(theObject, DATA_COLUMN);
    }
  else
    column_data = NULL;
  }

// COLUMN constructor -- objd constructor //////////////////////////////////

COLUMN::COLUMN(
  OBJECTID  objID,
  HGLOBAL&  colHandle,
  int&      numCols)

  {
  colHandle = NULL;
  theObject = objID;
  numColumns = (int)AObjGetDataSize(theObject, DATA_COLUMN) / sizeof(TCOLS);

  if(numColumns > 0)
    {
    column_data = (TCOLS*)AObjLockData(theObject, DATA_COLUMN);
    if(column_data[0].titleAlign==0)
      {
      for(int i=0; i<numColumns; i++)
        column_data[i].titleAlign = column_data[i].align - 308;
      }

    // get a copy of the object data
    colHandle = GlobalAlloc(GMEM_MOVEABLE, (DWORD)numColumns*sizeof(TCOLS));
    TCOLS*  temp = (TCOLS*)GlobalLock(colHandle);
    if(column_data && temp)
      hmemcpy(temp, column_data, (long)numColumns*sizeof(TCOLS));
    GlobalUnlock(colHandle);
    AObjUnlockData(theObject, DATA_COLUMN);
    }
  else
    column_data = NULL;

  numCols = numColumns;
  }

// COLUMN::AddColumn ///////////////////////////////////////////////////////

int COLUMN::AddColumn(
  OBJECTID  objID,
  char*     title,
  int       index)

  {
  numColumns++;
  AObjResizeData(theObject, DATA_COLUMN, (long)numColumns*sizeof(TCOLS));
  column_data =  (TCOLS*) AObjLockData(theObject, DATA_COLUMN);

  if(numColumns>1)
    {
    // copy the last column information into the new column
    int i = numColumns-1;
    // move the column information
    while(index<i)
      {
      column_data[i] = column_data[i-1];
      i--;
      }
    if(index!=(numColumns-1))
      column_data[index] = column_data[numColumns-1];
    else
      column_data[index] = column_data[numColumns-2];
    }
  else
    // use the default column information
    SetDefaults((column_data + index), NULL);

  SetTitle(index, column_data, title);

  AddColFont(index, numColumns);

  column_data[index].objRef = objID;
  AObjUnlockData(theObject, DATA_COLUMN);
  return index;
  }

// COLUMN::DeleteColumn ////////////////////////////////////////////////////

void COLUMN::DeleteColumn(
  int index)

  {
  if(index<0 || index>=numColumns)
    return;

  // move the upper columns and resize the object data
  int i = index;
  while(i<(numColumns-1))
    {
    column_data[i] = column_data[i+1];
    i++;
    }

  numColumns--;

  RemoveColFont(index, numColumns);
  AObjResizeData(theObject, DATA_COLUMN, (long)numColumns*sizeof(TCOLS));
  }

// COLUMN::Export //////////////////////////////////////////////////////////

BOOL  COLUMN::Export(
  )

  {
  HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
  column_data = (TCOLS*)AObjLockData(theObject, DATA_COLUMN);

  for (int i=0; i<numColumns; i++)
    {
    if(AUpsfCreateScope("columnSc", (long)i)==AUPSF_NOERROR)
      {
      AUpsfExportField("TITLE   ", 0L, AUPSF_NATV_STRING, AUPSF_UNIV_STRING, column_data[i].title, lstrlen(column_data[i].title));
      AUpsfExportField("THEREF  ", 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &column_data[i].objRef, sizeof(OBJECTID));
      ExportFont("TFONTATR", 0L, &column_data[i].lfTitle, hDC);
      ExportFont("CFONTATR", 0L, &column_data[i].lfCell, hDC);

      BOOL  true = TRUE;
      if(column_data[i].titleFG>0x00FFFFFFL)
        AUpsfExportField("TDEFFGCL", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &true, sizeof(BOOL));
      else
        AUpsfExportField("TITLCOLR", 0L, AUPSF_NATV_RGBCOLOR, AUPSF_UNIV_COLOR, &column_data[i].titleFG, sizeof(COLORREF));
      if(column_data[i].titleBG>0x00FFFFFFL)
        AUpsfExportField("TDEFBGCL", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &true, sizeof(BOOL));
      else
        AUpsfExportField("TBGCOLOR", 0L, AUPSF_NATV_RGBCOLOR, AUPSF_UNIV_COLOR, &column_data[i].titleBG, sizeof(COLORREF));
      if(column_data[i].cellFG>0x00FFFFFFL)
        AUpsfExportField("CDEFFGCL", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &true, sizeof(BOOL));
      else
        AUpsfExportField("ITEMCOLR", 0L, AUPSF_NATV_RGBCOLOR, AUPSF_UNIV_COLOR, &column_data[i].cellFG, sizeof(COLORREF));
      if(column_data[i].cellBG>0x00FFFFFFL)
        AUpsfExportField("CDEFBGCL", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &true, sizeof(BOOL));
      else
        AUpsfExportField("CBGCOLOR", 0L, AUPSF_NATV_RGBCOLOR, AUPSF_UNIV_COLOR, &column_data[i].cellBG, sizeof(COLORREF));

      AUpsfExportField("WIDTH   ", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &column_data[i].colWidth, sizeof(int));
      AUpsfExportField("BCANEDIT", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &column_data[i].bCanEdit, sizeof(BOOL));
      AUpsfExportField("ALIGN   ", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &column_data[i].align, sizeof(int));
      AUpsfExportField("TITLEAGN", 0L, AUPSF_NATV_CHAR, AUPSF_UNIV_INTEGER, &column_data[i].titleAlign, sizeof(char));
      AUpsfCloseScope();
      } // end...if(AUpsfCreateScope("columnSc", i)==AUPSF_NOERROR)
    } // end...for(int i=0; i<numColumns; i++)

  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);

  DeleteDC(hDC);
  return TRUE;
  }

// COLUMN::Import //////////////////////////////////////////////////////////

BOOL COLUMN::Import()
  {
  HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
  numColumns = (int)AUpsfFieldCount("columnSc");

  // allocate the space for the new object data...
  AObjResizeData(theObject, DATA_COLUMN, (long)numColumns*sizeof(TCOLS));
  column_data = (TCOLS*)AObjLockData(theObject, DATA_COLUMN);
  for (int i=0; i<numColumns; i++)
    {
    // set defaults in case something isn't imported....
    SetDefaults(&column_data[i], NULL, hDC);

    if(AUpsfOpenScope("columnSc", i)==AUPSF_NOERROR)
      {
      AUpsfImportField("TITLE   ", 0L, AUPSF_NATV_STRING, column_data[i].title, 32);
      AUpsfImportField("THEREF  ", 0L, AUPSF_NATV_OBJECTREF, &column_data[i].objRef, sizeof(OBJECTID));

      // use the old UPSF stuff for old projects
      if(AUpsfFieldInfo("COLFONT", 0L, AUPSF_NATV_FONT, NULL, NULL) == AUPSF_NOERROR)
        {
        // use defaults except for height, weight, and face name
        memset(&column_data[i].lfTitle, 0, sizeof(LOGFONT));
        AUpsfImportField("COLFONT ", 0L, AUPSF_NATV_FONT, &column_data[i].lfTitle.lfFaceName, LF_FACESIZE);
        AUpsfImportField("TITLEPTS", 0L, AUPSF_NATV_INT, &column_data[i].lfTitle.lfHeight, sizeof(int));
        column_data[i].lfTitle.lfWeight = FW_BOLD;
        }
      else
        ImportFont("TFONTATR", 0L, &column_data[i].lfTitle, hDC);

      if(AUpsfFieldInfo("ITEMFONT", 0L, AUPSF_NATV_FONT, NULL, NULL) == AUPSF_NOERROR)
        {
        // old UPSFed project -- support as best as possible
        memset(&column_data[i].lfCell, 0, sizeof(LOGFONT));

        AUpsfImportField("ITEMFONT", 0L, AUPSF_NATV_FONT, &column_data[i].lfCell.lfFaceName, LF_FACESIZE);
        AUpsfImportField("COLPTSIZ", 0L, AUPSF_NATV_INT, &column_data[i].lfCell.lfHeight, sizeof(int));
        column_data[i].lfCell.lfWeight = FW_NORMAL;
        }
      else
        ImportFont("CFONTATR", 0L, &column_data[i].lfCell, hDC);

      // store the point sizes in decipoints
      column_data[i].titlePtSize = - MulDiv(column_data[i].lfTitle.lfHeight, 720, GetDeviceCaps(hDC, LOGPIXELSY));
      column_data[i].cellPtSize = - MulDiv(column_data[i].lfCell.lfHeight, 720, GetDeviceCaps(hDC, LOGPIXELSY));

      // ignore errors for the colors -- if error, use the default!
      AUpsfImportField("TITLCOLR", 0L, AUPSF_NATV_RGBCOLOR, &column_data[i].titleFG, sizeof(COLORREF));
      AUpsfImportField("ITEMCOLR", 0L, AUPSF_NATV_RGBCOLOR, &column_data[i].cellFG, sizeof(COLORREF));
      AUpsfImportField("TBGCOLOR", 0L, AUPSF_NATV_RGBCOLOR, &column_data[i].titleBG, sizeof(COLORREF));
      AUpsfImportField("CBGCOLOR", 0L, AUPSF_NATV_RGBCOLOR, &column_data[i].cellBG, sizeof(COLORREF));

      AUpsfImportField("WIDTH   ", 0L, AUPSF_NATV_INT, &column_data[i].colWidth, sizeof(int));
      AUpsfImportField("BCANEDIT", 0L, AUPSF_NATV_BOOL, &column_data[i].bCanEdit , sizeof(BOOL));
      AUpsfImportField("ALIGN   ", 0L, AUPSF_NATV_INT, &column_data[i].align , sizeof(int));
      AUpsfImportField("TITLEAGN", 0L, AUPSF_NATV_CHAR, &column_data[i].titleAlign, sizeof(char));

      AUpsfCloseScope();
      } // end...if(AUpsfOpenScope("columnSc", i)==AUPSF_NOERROR)
    } // end...for(int i=0; i<numColumns; i++)

  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN); // unlock old data
  DeleteDC(hDC);
  return TRUE;
  }

// COLUMN::ExportFont //////////////////////////////////////////////////////

BOOL  COLUMN::ExportFont(
  LPCSTR    theScope,
  long      scopeIndex,
  LOGFONT*  lf,
  HDC       hdc)

  {
  HDC hDC = hdc ? hdc : CreateDC("DISPLAY", NULL, NULL, NULL);

  if(AUpsfCreateScope((LPSTR)theScope, scopeIndex)==AUPSF_NOERROR)
    {
    BOOL  bold = (lf->lfWeight >= FW_BOLD) ? TRUE : FALSE;
    BOOL  italic = lf->lfItalic ? TRUE : FALSE;
    BOOL  under = lf->lfUnderline ? TRUE : FALSE;
    BOOL  strike = lf->lfStrikeOut ? TRUE : FALSE;
    int   deciPointSize;

    if(lf->lfHeight > 0)
      {
      HFONT       oldFont;
      HFONT       hFont = AUtlCreateFontIndirect(lf);
      TEXTMETRIC  tm;

      // get the text metrics for the DC
      oldFont = (HFONT)SelectObject(hDC, hFont);
      GetTextMetrics(hDC, &tm);
      SelectObject(hDC, oldFont);
      AUtlDeleteFont(hFont);

      deciPointSize = MulDiv(lf->lfHeight - tm.tmInternalLeading, 720, GetDeviceCaps(hDC, LOGPIXELSY));
      }
    else
      deciPointSize = - MulDiv(lf->lfHeight, 720, GetDeviceCaps(hDC, LOGPIXELSY));

    AUpsfExportField("FACENAME", 0L, AUPSF_NATV_FONT, AUPSF_UNIV_FONT, lf->lfFaceName, lstrlen(lf->lfFaceName));
    AUpsfExportField("BOLD", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &bold, sizeof(BOOL));
    AUpsfExportField("ITALIC", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &italic, sizeof(BOOL));
    AUpsfExportField("UNDRLINE", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &under, sizeof(BOOL));
    AUpsfExportField("STRIKOUT", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &strike, sizeof(BOOL));
    AUpsfExportField("DECIPTSZ", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &deciPointSize, sizeof(int));

    // export the logfont structure for native import-export!
    AUpsfExportField("wLOGFONT", 0L, NATV_LOGFONT, NATV_LOGFONT, lf, sizeof(LOGFONT));

    AUpsfCloseScope();
    }
  else
    {
    if(!hdc)
      DeleteDC(hDC);
    return FALSE;
    }

  if(!hdc)
    DeleteDC(hDC);
  return TRUE;
  }

// COLUMN::ImportFont //////////////////////////////////////////////////////

BOOL  COLUMN::ImportFont(
  LPCSTR    theScope,
  long      scopeIndex,
  LOGFONT*  lf,
  HDC       hdc)

  {
  HDC hDC = hdc ? hdc : CreateDC("DISPLAY", NULL, NULL, NULL);

  if(AUpsfOpenScope((LPSTR)theScope, scopeIndex)==AUPSF_NOERROR)
    {
    BOOL  bold;
    BOOL  italic;
    BOOL  under;
    BOOL  strike;
    int   deciPointSize;

    memset(lf, 0, sizeof(LOGFONT));

    // import the logfont structure for native import-export!
    long  ret = AUpsfImportField("wLOGFONT", 0L, NATV_LOGFONT, lf, sizeof(LOGFONT));

    if(ret!=AUPSF_NOERROR)
      {
      bold = TRUE;
      italic = FALSE;
      under = FALSE;
      strike = FALSE;

      // do the standard import from Mac
      AUpsfImportField("FACENAME", 0L, AUPSF_NATV_FONT, lf->lfFaceName, LF_FACESIZE);
      AUpsfImportField("BOLD", 0L, AUPSF_NATV_BOOL, &bold, sizeof(BOOL));
      AUpsfImportField("ITALIC", 0L, AUPSF_NATV_BOOL, &italic, sizeof(BOOL));
      AUpsfImportField("UNDRLINE", 0L, AUPSF_NATV_BOOL, &under, sizeof(BOOL));
      AUpsfImportField("STRIKOUT", 0L, AUPSF_NATV_BOOL, &strike, sizeof(BOOL));
      AUpsfImportField("DECIPTSZ", 0L, AUPSF_NATV_INT, &deciPointSize, sizeof(int));

      lf->lfWeight = bold ? FW_BOLD : FW_NORMAL;
      lf->lfItalic = italic ? 1 : 0;
      lf->lfUnderline = under ? 1 : 0;
      lf->lfStrikeOut = strike ? 1 : 0;
      lf->lfHeight = - MulDiv(deciPointSize, GetDeviceCaps(hDC, LOGPIXELSY), 720);
      }

    AUpsfCloseScope();
    }
  else
    {
    if(!hdc)
      DeleteDC(hDC);
    return FALSE;
    }

  if(!hdc)
    DeleteDC(hDC);
  return TRUE;
  }

// COLUMN::SetDefaults /////////////////////////////////////////////////////

void  COLUMN::SetDefaults(
  TCOLS*  newColumn,
  TCOLS * defColumn,
  HDC     hdc
  )

  {
  I_UNUSED(hdc);

  // return if an invalid pointer!
  if(!newColumn)
    return;

  if (defColumn)
    {
    _fmemcpy((char *)newColumn, (char *)defColumn, sizeof(TCOLS));
    return;
    }
  else {
    TATTRIB *attrib_data = (TATTRIB*)AObjLockData(theObject, DATA_TABLEATTRIBUTES);
    _fmemcpy((char *)newColumn, (char *)&(attrib_data->defColumn), sizeof(TCOLS));
    AObjUnlockData(theObject, DATA_TABLEATTRIBUTES);
    return;
    }
  }

// COLUMN::AddColumn -- objd AddColumn /////////////////////////////////////

int COLUMN::AddColumn(
  OBJECTID  objID,
  HGLOBAL&  colHandle,
  int&      numCols,
  int       first,
  TCOLS *   defColumn)

  {
  // increment the number of columns
  numCols++;
  numColumns++;

  // allocate the memory
  if(colHandle)
    colHandle = GlobalReAlloc(colHandle, (long)numCols*sizeof(TCOLS), GMEM_MOVEABLE);
  else
    colHandle = GlobalAlloc(GMEM_MOVEABLE, (long)numCols*sizeof(TCOLS));
  TCOLS*  colPtr = (TCOLS*)GlobalLock(colHandle);

  if(numCols>1)
    {
    
    // copy the last column information into the new column
    int i = numCols-1;
    // move the column information
    while(first<i)
      {
      colPtr[i] = colPtr[i-1];
      i--;
      }

    if(first!=(numCols-1))
      colPtr[first] = colPtr[numCols-1];
    else
      colPtr[first] = colPtr[numCols-2];

    // use the default column information
    SetDefaults((colPtr + first), defColumn);
    }
  else
    // use the default column information
    SetDefaults((colPtr + first), defColumn);

  // always use the object name for the object title
  if(objID)
    AObjGetName(objID, colPtr[first].title, COLUMN_TITLEBUFSIZE);
  colPtr[first].objRef = objID;

  GlobalUnlock(colHandle);
  return first;
  }

// COLUMN::DeleteColumn -- objd DeleteColumn ///////////////////////////////

void COLUMN::DeleteColumn(
  int       index,
  HGLOBAL&  colHandle,
  int&      numCols)

  {
  // check for an invalid index
  if(index >= numCols)
    return;

  if(index!=(numCols-1))
    {
    // move every column above "index" down one spot
    TCOLS*  colPtr = (TCOLS*)GlobalLock(colHandle);
    hmemmove(&colPtr[index], &colPtr[index+1], (long)sizeof(TCOLS)*(numCols-1-index));
    GlobalUnlock(colHandle);
    }

  // resize the data block
  numCols--;
  numColumns--;
  if(numCols==0)
    {
    GlobalFree(colHandle);
    colHandle = NULL;
    }
  else
    colHandle = GlobalReAlloc(colHandle, (long)numCols*sizeof(TCOLS), GMEM_MOVEABLE);
  }

// COLUMN::TitleMetrics ////////////////////////////////////////////////////

void COLUMN::TitleMetrics(
  HDC   hDC,
  int   logPixelsY,
  BOOL  oldWidth)

  {
  TEXTMETRIC  tm;
  HFONT       hFont, oldFont;
  int         width;
  column_data = (TCOLS*)AObjLockData(theObject, DATA_COLUMN);

  for(int i=0; i<numColumns; i++)
    {
    if(oldWidth)
      width = (int)((long double)column_data[i].titleCharWidth * (long double)GetDeviceCaps(hDC, LOGPIXELSY) / (long double)VGA_RESOLUTION);
    else
      width = 0;

    // This can be called from the Print Routine,
    // So create new fonts if needed else Utl routine will return an existing font
    hFont = AUtlCreateFontIndirect(TitleFont(i, logPixelsY, width));
    oldFont = (HFONT)SelectObject(hDC, hFont);
    GetTextMetrics(hDC, &tm);
    SelectObject(hDC, oldFont);
    AUtlDeleteFont(hFont);

    column_data[i].titleLeading = tm.tmInternalLeading;
    column_data[i].titleAscent = tm.tmAscent;
    column_data[i].titleDescent = tm.tmDescent;

    if(!oldWidth)
      column_data[i].titleCharWidth = tm.tmAveCharWidth;
    }
  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);

  }

// COLUMN::CellMetrics /////////////////////////////////////////////////////

void COLUMN::CellMetrics(
  HDC   hDC,
  int   logPixelsY,
  BOOL  oldWidth)

  {
  TEXTMETRIC  tm;
  HFONT       hFont, oldFont;
  int         width;

  column_data = (TCOLS*)AObjLockData(theObject, DATA_COLUMN);

  for(int i=0; i<numColumns; i++)
    {
    if(oldWidth)
      width = (int)((long double)column_data[i].cellCharWidth * (long double)GetDeviceCaps(hDC, LOGPIXELSY) / (long double)VGA_RESOLUTION);
    else
      width = 0;

    // This can be called from the Print Routine,
    // So create new fonts if needed else Utl routine will return an existing font
    hFont = AUtlCreateFontIndirect(CellFont(i, logPixelsY, width));
    oldFont = (HFONT)SelectObject(hDC, hFont);
    GetTextMetrics(hDC, &tm);
    SelectObject(hDC, oldFont);
    AUtlDeleteFont(hFont);

    column_data[i].cellLeading = tm.tmInternalLeading;
    column_data[i].cellAscent = tm.tmAscent;
    column_data[i].cellDescent = tm.tmDescent;

    if(!oldWidth)
      column_data[i].cellCharWidth = tm.tmAveCharWidth;
    }
  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);

  }



// COLUMN::AddColFont ////////////////////////////////////////////////////
// Create fonts for all a column and hold on to them in
// RUNTIMEINFO instance.
// Resize the runtime->lpHTFonts if needed
//////////////////////////////////////////////////////////////////////////
void COLUMN::AddColFont(
  int   index,
  int   numCols
  )
  {
  HFONT         hFont;
  RUNTIMEINFO*  rTime     = (RUNTIMEINFO *)AObjGetRuntimeInfo(theObject);

  if(!rTime->m_fParentOpen)
    return;

  HDC           hDC       = CreateDC("DISPLAY", NULL, NULL, NULL);
  int           i         = 0;

  // Create a new title font //////////////////////////
  int           size      = (int)AMemGetSize(rTime->MemhTFont);
  if (size < 0)
    size = 0;
  MEMBLOCKID    tmpMemID  =   AMemAllocate(size + sizeof(HFONT));
  HFONT*        tmpHFont  =  (HFONT *)AMemLock(tmpMemID);

  column_data = (TCOLS*)AObjLockData(theObject, DATA_COLUMN);

  for (i = 0; i < index; i++)
    {
    tmpHFont[i] = rTime->lpHTFont[i];
    }
  for (i = index + 1; i < numCols; i++)
    {
    tmpHFont[i] = rTime->lpHTFont[i];
    }

  AMemUnlock(rTime->MemhTFont);
  AMemFree(rTime->MemhTFont);
  rTime->MemhTFont = tmpMemID;
  // Keep the ptr locked
  rTime->lpHTFont = tmpHFont;
  hFont = AUtlCreateFontIndirect(TitleFont(index,  VGA_RESOLUTION, 0));
  rTime->SetHTFont(index, hFont);

  // Create Cell Font ///////////////////////////////////////////
  size  = (int)AMemGetSize(rTime->MemhCFont);
  if (size < 0)
    size = 0;
  tmpMemID =  AMemAllocate(size + sizeof(HFONT));
  tmpHFont =  (HFONT *)AMemLock(tmpMemID);

  for (i = 0; i < index; i++)
    {
    tmpHFont[i] = rTime->lpHCFont[i];
    }
  // leave a hole in the 'index - th' position
  for (i = index + 1; i < numCols; i++)
    {
    tmpHFont[i] = rTime->lpHCFont[i - 1];
    }

  AMemUnlock(rTime->MemhCFont);
  AMemFree(rTime->MemhCFont);
  rTime->MemhCFont = tmpMemID;
  // Keep the ptr locked
  rTime->lpHCFont = tmpHFont;
  hFont = AUtlCreateFontIndirect(CellFont(index, VGA_RESOLUTION, 0));
  rTime->SetHCFont(index, hFont);

  //////////////////////////////////////////////////////////////////
  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);

  DeleteDC(hDC);
  }

// COLUMN::RemoveColFont ////////////////////////////////////////////////////
// Remove fonts for a column from RUNTIMEINFO object
// Resize the runtime->lpHTFonts if needed
//////////////////////////////////////////////////////////////////////////
void COLUMN::RemoveColFont(
  int   index,
  int   numCols
  )
  {
  RUNTIMEINFO *rTime = (RUNTIMEINFO *)AObjGetRuntimeInfo(theObject);

  if(!rTime->m_fParentOpen)
    return;

  // Remove title font //////////////////////////
  int size  = (int)AMemGetSize(rTime->MemhTFont);
  if (size <= 0)
    {
    return;
    }
  column_data = (TCOLS*)AObjLockData(theObject, DATA_COLUMN);
  HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
  int i = 0;
  MEMBLOCKID tmpMemID =   AMemAllocate(size - sizeof(HFONT));
  HFONT      *tmpHFont =  (HFONT *)AMemLock(tmpMemID);

  for (i = 0; i < index; i++)
    {
    tmpHFont[i] = rTime->lpHTFont[i];
    }
  // remove 'index' font
  for (i = index; i < numCols; i++)
    {
    tmpHFont[i] = rTime->lpHTFont[i + 1];
    }

  AUtlDeleteFont(rTime->lpHTFont[index]);
  AMemUnlock(rTime->MemhTFont);
  AMemFree(rTime->MemhTFont);
  rTime->MemhTFont = tmpMemID;
  // Keep the ptr locked
  rTime->lpHTFont = tmpHFont;

  // Remove cell font //////////////////////////
  size  = (int)AMemGetSize(rTime->MemhCFont);
  if (size <= 0)
    return;
  tmpMemID =   AMemAllocate(size - sizeof(HFONT));
  tmpHFont =  (HFONT *)AMemLock(tmpMemID);

  for (i = 0; i < index; i++)
    {
    tmpHFont[i] = rTime->lpHCFont[i];
    }
  // remove 'index' font
  for (i = index; i < numCols; i++)
    {
    tmpHFont[i] = rTime->lpHCFont[i + 1];
    }

  AUtlDeleteFont(rTime->lpHCFont[index]);
  AMemUnlock(rTime->MemhCFont);
  AMemFree(rTime->MemhCFont);
  rTime->MemhCFont = tmpMemID;
  // Keep the ptr locked
  rTime->lpHCFont = tmpHFont;

  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);

  DeleteDC(hDC);
  }

// COLUMN::CreateTitleFonts ////////////////////////////////////////////////////
// Create fonts for all the column titles and hold on to them in
// RUNTIMEINFO instance.
//
//////////////////////////////////////////////////////////////////////////
void COLUMN::CreateTitleFonts(
  HDC   hDC,
  int   logPixelsY,
  BOOL  oldWidth)

  {
  HFONT       hFont;
  int         width;
  column_data = (TCOLS*)AObjLockData(theObject, DATA_COLUMN);
  RUNTIMEINFO *rTime = (RUNTIMEINFO *)AObjGetRuntimeInfo(theObject);
  rTime->MemhTFont =  AMemAllocate(numColumns * sizeof(HFONT));
  rTime->lpHTFont   = (HFONT *)AMemLock(rTime->MemhTFont);

  for(int i=0; i<numColumns; i++)
    {
    if(oldWidth)
      width = (int)((long double)column_data[i].titleCharWidth * (long double)GetDeviceCaps(hDC, LOGPIXELSY) / (long double)VGA_RESOLUTION);
    else
      width = 0;

    hFont = AUtlCreateFontIndirect(TitleFont(i, logPixelsY, width));
    rTime->SetHTFont(i, hFont);

    }
  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);
  }

// COLUMN::CreatecellFonts ////////////////////////////////////////////////////
// Create fonts for all the column Cells and hold on to them in
// RUNTIMEINFO instance.
//
//////////////////////////////////////////////////////////////////////////

void COLUMN::CreateCellFonts(
  HDC   hDC,
  int   logPixelsY,
  BOOL  oldWidth)

  {
  HFONT       hFont;
  int         width;

  column_data = (TCOLS*)AObjLockData(theObject, DATA_COLUMN);
  RUNTIMEINFO *rTime = (RUNTIMEINFO *)AObjGetRuntimeInfo(theObject);
  rTime->MemhCFont =  AMemAllocate(numColumns * sizeof(HFONT));
  rTime->lpHCFont   = (HFONT *)AMemLock(rTime->MemhCFont);

  for(int i=0; i<numColumns; i++)
    {
    if(oldWidth)
      width = (int)((long double)column_data[i].cellCharWidth * (long double)GetDeviceCaps(hDC, LOGPIXELSY) / (long double)VGA_RESOLUTION);
    else
      width = 0;

    hFont = AUtlCreateFontIndirect(CellFont(i, logPixelsY, width));
    rTime->SetHCFont(i, hFont);
    }
  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);
  }

// COLUMN::DestroyTitleFonts ////////////////////////////////////////////////////
void COLUMN::DestroyTitleFonts()
  {
  column_data = (TCOLS*)AObjLockData(theObject, DATA_COLUMN);
  RUNTIMEINFO *rTime = (RUNTIMEINFO *)AObjGetRuntimeInfo(theObject);

  for(int i=0; i<numColumns; i++)
    {
    AUtlDeleteFont (rTime->GetHTFont(i));
    }
  AMemUnlock(rTime->MemhTFont);
  AMemFree(rTime->MemhTFont);
  rTime->MemhTFont = 0;
  rTime->lpHTFont = 0;
  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);

  }

// COLUMN::CellFonts /////////////////////////////////////////////////////

void COLUMN::DestroyCellFonts()
  {
  column_data = (TCOLS*)AObjLockData(theObject, DATA_COLUMN);
  RUNTIMEINFO *rTime = (RUNTIMEINFO *)AObjGetRuntimeInfo(theObject);

  for(int i=0; i<numColumns; i++)
    {
    AUtlDeleteFont(rTime->GetHCFont(i));
    }
  AMemUnlock(rTime->MemhCFont);
  AMemFree(rTime->MemhCFont);
  rTime->MemhCFont = 0;
  rTime->lpHCFont = 0;

  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);
  }

// COLUMN::MaxTitleAscent //////////////////////////////////////////////////

int COLUMN::MaxTitleAscent()
  {
  int maxAscent = -1;
  column_data = (TCOLS*)AObjLockData(theObject, DATA_COLUMN);

  for(int i=0; i<numColumns; i++)
    {
    if(maxAscent < column_data[i].titleAscent)
      maxAscent = column_data[i].titleAscent;
    }
  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);

  return maxAscent;
  }

// COLUMN::MaxTitleDescent /////////////////////////////////////////////////

int COLUMN::MaxTitleDescent()
  {
  int maxDescent = -1;
  column_data =(TCOLS*) AObjLockData(theObject, DATA_COLUMN);
  for(int i=0; i<numColumns; i++)
    {
    if(maxDescent < column_data[i].titleDescent)
      maxDescent = column_data[i].titleDescent;
    }
  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);

  return maxDescent;
  }

// COLUMN::MaxTitleLeading /////////////////////////////////////////////////

int COLUMN::MaxTitleLeading()
  {
  int maxLeading = -1;
  column_data =(TCOLS*) AObjLockData(theObject, DATA_COLUMN);

  for(int i=0; i<numColumns; i++)
    {
    if(maxLeading < column_data[i].titleLeading)
      maxLeading = column_data[i].titleLeading;
    }
  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);

  return maxLeading;
  }

// COLUMN::MaxCellAscent ///////////////////////////////////////////////////

int COLUMN::MaxCellAscent()
  {
  int maxAscent = -1;
  column_data =(TCOLS*) AObjLockData(theObject, DATA_COLUMN);

  for(int i=0; i<numColumns; i++)
    {
    if(maxAscent < column_data[i].cellAscent)
      maxAscent = column_data[i].cellAscent;
    }
  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);

  return maxAscent;
  }

// COLUMN::MaxCellDescent //////////////////////////////////////////////////

int COLUMN::MaxCellDescent()
  {
  int maxDescent = -1;
  column_data =(TCOLS*) AObjLockData(theObject, DATA_COLUMN);

  for(int i=0; i<numColumns; i++)
    {
    if(maxDescent < column_data[i].cellDescent)
      maxDescent = column_data[i].cellDescent;
    }
  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);

  return maxDescent;
  }

// COLUMN::MaxCellLeading //////////////////////////////////////////////////

int COLUMN::MaxCellLeading()
  {
  int maxLeading = -1;
  column_data =(TCOLS*) AObjLockData(theObject, DATA_COLUMN);

  for(int i=0; i<numColumns; i++)
    {
    if(maxLeading < column_data[i].cellLeading)
      maxLeading = column_data[i].cellLeading;
    }
  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);

  return maxLeading;
  }

// COLUMN::TitleBGColor ////////////////////////////////////////////////////

COLORREF COLUMN::TitleBGColor(
  int index)

  {
  COLORREF rVal;
  column_data =(TCOLS*) AObjLockData(theObject, DATA_COLUMN);

  if(column_data[index].titleBG > 0x00FFFFFFL)
    rVal = GetSysColor(COLOR_WINDOW);
  else
    rVal = column_data[index].titleBG;

  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);
  return rVal;
  }

// COLUMN::TitleFGColor ////////////////////////////////////////////////////

COLORREF COLUMN::TitleFGColor(
  int index)

  {
  COLORREF rVal;
  column_data =(TCOLS*) AObjLockData(theObject, DATA_COLUMN);
  
  if(column_data[index].titleFG > 0x00FFFFFFL)
    rVal = GetSysColor(COLOR_WINDOWTEXT);
  else
    rVal = column_data[index].titleFG;
  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);

  return rVal;
  }

// COLUMN::CellBGColor /////////////////////////////////////////////////////

COLORREF COLUMN::CellBGColor(
  int index)

  {
  COLORREF rVal;
  column_data =(TCOLS*) AObjLockData(theObject, DATA_COLUMN);

  if(column_data[index].cellBG > 0x00FFFFFFL)
    rVal = GetSysColor(COLOR_WINDOW);
  else
    rVal = column_data[index].cellBG;

  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);
  return rVal;
  }

// COLUMN::CellFGColor /////////////////////////////////////////////////////

COLORREF COLUMN::CellFGColor(
  int index)

  {
  COLORREF rVal;
  column_data = (TCOLS*)AObjLockData(theObject, DATA_COLUMN);
  if(column_data[index].cellFG > 0x00FFFFFFL)
    rVal = GetSysColor(COLOR_WINDOWTEXT);
  else
    rVal = column_data[index].cellFG;
  if (column_data)
    AObjUnlockData(theObject, DATA_COLUMN);
  return rVal;

  }

// COLUMN::GetObjectID -- objd function ////////////////////////////////////

OBJECTID COLUMN::GetObjectID(
  int     index,
  HGLOBAL colHandle)

  {
  // check for an invalid index
  if(index >= numColumns)
    return NULL;

  TCOLS*    colPtr = (TCOLS*)GlobalLock(colHandle);
  if(!colPtr)
    return NULL;
  OBJECTID  objID = colPtr[index].objRef;
  GlobalUnlock(colHandle);

  return objID; 
  }

// COLUMN::Title -- objd Title /////////////////////////////////////////////

char* COLUMN::Title(
  int     index,
  TCOLS*  colPtr)

  {
  if ((index == 0 && index == numColumns) ||
    (index>=0 && index<numColumns && colPtr))
    return colPtr[index].title;
  else
    return NULL;
  }

// COLUMN::TitleAlignment -- objd TitleAlignment ///////////////////////////

int COLUMN::TitleAlignment(
  int     index,
  TCOLS*  colPtr)

  {
  if ((index == 0 && index == numColumns) ||
    (index>=0 && index<numColumns && colPtr))
    return colPtr[index].titleAlign;
  else
    return ALIGN_ERROR;
  }

// COLUMN::Alignment -- objd Alignment /////////////////////////////////////

int COLUMN::Alignment(
  int     index,
  TCOLS*  colPtr)

  {
  if ((index == 0 && index == numColumns) ||
    (index>=0 && index<numColumns && colPtr) )
    return colPtr[index].align;
  else
    return ALIGN_ERROR;
  }

// COLUMN::Editable -- objd Editable ///////////////////////////////////////

BOOL COLUMN::Editable(
  int     index,
  TCOLS*  colPtr)

  {
  if ((index == 0 && index == numColumns) ||
    (index>=0 && index<numColumns && colPtr) )
    return colPtr[index].bCanEdit;
  else
    return FALSE;
  }

// COLUMN::Pixels -- objd Pixels ///////////////////////////////////////////

int COLUMN::Pixels(
  int     index,
  TCOLS*  colPtr)

  {
  if ((index == 0 && index == numColumns) ||
    (index>=0 && index<numColumns && colPtr)  )
    return colPtr[index].colWidth;
  else
    return COLUMN_DEFAULTWIDTH;
  }

// COLUMN::Characters -- objd Characters ///////////////////////////////////

double COLUMN::Characters(
  int     index,
  TCOLS*  colPtr)

  {
  double      numChars;
  int         numPixels;
  TEXTMETRIC  tm;
  HDC         hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
  // This called during OBJD, so create new fonts
  HFONT       hFont = AUtlCreateFontIndirect(&colPtr[index].lfCell);

  HFONT       oldFont = (HFONT)SelectObject(hDC, hFont);

  if ((index == 0 && index == numColumns) ||
    (index>=0 && index<numColumns && colPtr))
    numPixels = colPtr[index].colWidth;
  else
    numPixels = COLUMN_DEFAULTWIDTH;
  

  GetTextMetrics(hDC, &tm);
  numChars = (double)numPixels / (double)tm.tmAveCharWidth; 

  SelectObject(hDC, oldFont);
  AUtlDeleteFont(hFont);
  DeleteDC(hDC);
  return numChars;
  }

// COLUMN::Inches -- objd Inches ///////////////////////////////////////////

double COLUMN::Inches(
  int     index,
  TCOLS*  colPtr)

  {
  double  numInches;
  int     numPixels;
  HDC     hDC = CreateDC("DISPLAY", NULL, NULL, NULL);

  if ((index == 0 && index == numColumns) ||
    (index>=0 && index<numColumns && colPtr))
    numPixels = colPtr[index].colWidth;
  else
    numPixels = COLUMN_DEFAULTWIDTH;


  numInches = (double)numPixels / (double)GetDeviceCaps(hDC, LOGPIXELSX);

  DeleteDC(hDC);
  return numInches;
  }

// COLUMN::TitleFont -- objd TitleFont /////////////////////////////////////

LOGFONT* COLUMN::TitleFont(
  int     index,
  TCOLS*  colPtr,
  int     logPixelsY,
  int     width)

  {
  // if the index is 0 and numCOl == 0, we are setting the font for the DEF column
  if ((index == 0 && index == numColumns) ||
    (index>=0 && index<numColumns && colPtr))
    {
    if(logPixelsY>0)
      colPtr[index].lfTitle.lfHeight = - MulDiv(colPtr[index].titlePtSize, logPixelsY, 720);
    else
      colPtr[index].lfTitle.lfHeight = - MulDiv(colPtr[index].titlePtSize, VGA_RESOLUTION, 720);

    if(width>0)
      colPtr[index].lfTitle.lfWidth = width;
    else
      colPtr[index].lfTitle.lfWidth = 0;

    return &colPtr[index].lfTitle;
    }
  else
    return NULL;
  }

// COLUMN::TitleFGColor -- objd TitleFGColor ///////////////////////////////

COLORREF COLUMN::TitleFGColor(
  int     index,
  TCOLS*  colPtr)

  {
  // if the index is 0 and numCOl == 0, we are setting the font for the DEF column
  if ((index == 0 && index == numColumns) ||
    (index>=0 && index<numColumns && colPtr))
    return colPtr[index].titleFG;
  else
    return WINDOW_FOREGROUND;

  }

// COLUMN::TitleBGColor -- objd TitleBGColor ///////////////////////////////

COLORREF COLUMN::TitleBGColor(
  int     index,
  TCOLS*  colPtr)

  {
  // if the index is 0 and numCOl == 0, we are setting the font for the DEF column
  if ((index == 0 && index == numColumns) ||
    (index>=0 && index<numColumns && colPtr))
    return colPtr[index].titleBG;
  else
    return WINDOW_BACKGROUND;
  }

// COLUMN::CellFont -- objd CellFont ///////////////////////////////////////

LOGFONT* COLUMN::CellFont(
  int     index,
  TCOLS*  colPtr,
  int     logPixelsY,
  int     width)

  {
  // if the index is 0 and numCOl == 0, we are setting the font for the DEF column
  if ((index == 0 && index == numColumns) ||
    (index>=0 && index<numColumns && colPtr) )
    {
    if(logPixelsY>0)
      colPtr[index].lfCell.lfHeight = - MulDiv(colPtr[index].cellPtSize, logPixelsY, 720);
    else
      colPtr[index].lfCell.lfHeight = - MulDiv(colPtr[index].titlePtSize, VGA_RESOLUTION, 720);

    if(width>0)
      colPtr[index].lfCell.lfWidth = width;
    else
      colPtr[index].lfCell.lfWidth = 0;

    return &colPtr[index].lfCell;
    }
  else
    return NULL;
  }

// COLUMN::CellFGColor -- objd CellFGColor /////////////////////////////////

COLORREF COLUMN::CellFGColor(
  int     index,
  TCOLS*  colPtr)

  {
  // if the index is 0 and numCOl == 0, we are setting the font for the DEF column
  if ((index == 0 && index == numColumns) ||
    (index>=0 && index<numColumns && colPtr))
    return colPtr[index].cellFG;
  else
    return WINDOW_FOREGROUND;
  }

// COLUMN::CellBGColor -- objd CellBGColor /////////////////////////////////

COLORREF COLUMN::CellBGColor(
  int     index,
  TCOLS*  colPtr)

  {
  // if the index is 0 and numCOl == 0, we are setting the font for the DEF column
  if ((index == 0 && index == numColumns) ||
    (index>=0 && index<numColumns && colPtr) )
    return colPtr[index].cellBG;
  else
    return WINDOW_BACKGROUND;
  }

// COLUMN::ObjectID -- objd ObjectID ///////////////////////////////////////

OBJECTID COLUMN::ObjectID(
  int     index,
  TCOLS*  colPtr)

  {
  // if the index is 0 and numCOl == 0, we are setting the font for the DEF column
  if ((index == 0 && index == numColumns) ||
    (index>=0 && index<numColumns && colPtr))
    return colPtr[index].objRef;
  else
    return NULL;
  }

// COLUMN::SetTitle ////////////////////////////////////////////////////////

void COLUMN::SetTitle(
  int     index,
  TCOLS*  colPtr,
  char*   newTitle)

  {
  if (((index == 0) && (index == numColumns)) ||
    (colPtr
      &&
    (index>=0)
      &&
    (index<numColumns)
      &&
    newTitle))
    {
    strncpy(colPtr[index].title, newTitle, COLUMN_TITLEBUFSIZE);

    // guarantee null termination
    colPtr[index].title[COLUMN_TITLEBUFSIZE-1] = 0;
    }
  }

// COLUMN::SetTitleAlignment -- objd SetTitleAlignment /////////////////////

void COLUMN::SetTitleAlignment(
  int     index,
  TCOLS*  colPtr,
  char    state)

  {
  if (((index == 0) && (index == numColumns)) ||
    (colPtr && (index>=0) && (index<numColumns)) )
    colPtr[index].titleAlign = state;
  }

// COLUMN::SetAlignment -- objd SetAlignment ///////////////////////////////

void COLUMN::SetAlignment(
  int     index,
  TCOLS*  colPtr,
  int     state)

  {
  if (((index == 0) && (index == numColumns)) ||
    (colPtr && (index>=0) && (index<numColumns)))
    colPtr[index].align = state;
  }

// COLUMN::SetEditable -- objd SetEditable /////////////////////////////////

void COLUMN::SetEditable(
  int     index,
  TCOLS*  colPtr,
  BOOL    state)

  {
  if ( ((index == numColumns) && (index == 0)) ||
     (colPtr && (index>=0) && (index<numColumns)) )
    colPtr[index].bCanEdit = state;
  }

// COLUMN::SetPixels -- objd SetPixels /////////////////////////////////////

int COLUMN::SetPixels(
  int     index,
  TCOLS*  colPtr,
  int     numPixels)

  {
  if (((index == 0) && (index == numColumns)) ||
    (colPtr && (index>=0) && (index<numColumns)))
    {
    if (numPixels>999)
      numPixels = 999;

    colPtr[index].colWidth = numPixels;
    }

  return numPixels;
  }

// COLUMN::SetCharacters -- objd SetCharacters /////////////////////////////

double COLUMN::SetCharacters(
  int     index,
  TCOLS*  colPtr,
  double  numChars)

  {
  if (((index == 0) && (index == numColumns)) ||
    (colPtr && (index>=0) && (index<numColumns)))
    {
    int         numPixels;
    TEXTMETRIC  tm;
    HDC         hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
    HFONT       hFont = AUtlCreateFontIndirect(&colPtr[index].lfCell);
    HFONT       oldFont = (HFONT)SelectObject(hDC, hFont);

    GetTextMetrics(hDC, &tm);
    numPixels = (int)(numChars * tm.tmAveCharWidth + 0.5);

    if (numPixels>999)
      numPixels = 999;
    colPtr[index].colWidth = numPixels;

    SelectObject(hDC, oldFont);
    DeleteDC(hDC);
    AUtlDeleteFont(hFont);
    return (double)numPixels / (double)tm.tmAveCharWidth;
    }
  else
    return 0.0;
  }

// COLUMN::SetInches -- objd SetInches /////////////////////////////////////

double COLUMN::SetInches(
  int     index,
  TCOLS*  colPtr,
  double  numInches)

  {
  if (((index == 0) && (index == numColumns)) ||
    (colPtr && index>=0 && (index<numColumns)))
    {
    int numPixels;
    HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
    int pixelsX = GetDeviceCaps(hDC, LOGPIXELSX);

    numPixels = (int)(numInches * pixelsX + 0.5);
    if (numPixels>999)
      numPixels = 999;
    colPtr[index].colWidth = numPixels;

    DeleteDC(hDC);
    return (double)numPixels / (double)pixelsX;
    }
  else
    {
    return 0.0;
    }
  }

// COLUMN::SetTitleFont -- objd SetTitleFont ///////////////////////////////

void COLUMN::SetTitleFont(
  int       index,
  TCOLS*    colPtr,
  LOGFONT*  lf,
  int       deciPointSize)

  {
  if(index<0 || (index == numColumns &&  index != 0) || (index>numColumns) || !colPtr || !lf)
    return;

  colPtr[index].lfTitle = *lf;
  colPtr[index].titlePtSize = deciPointSize;

  // fill in other font characteristics
  HDC         hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
  HFONT       hFont = AUtlCreateFontIndirect(lf);
  HFONT       oldFont = (HFONT)SelectObject(hDC, hFont);
  TEXTMETRIC  tm;
  GetTextMetrics(hDC, &tm);
  colPtr[index].titleCharWidth = tm.tmAveCharWidth;
  colPtr[index].titleLeading = tm.tmInternalLeading;
  colPtr[index].titleAscent = tm.tmAscent;
  colPtr[index].titleDescent = tm.tmDescent;
  SelectObject(hDC, oldFont);
  AUtlDeleteFont(hFont);
  DeleteDC(hDC);
  }

// COLUMN::SetTitleFGColor -- objd SetTitleFGColor /////////////////////////

void COLUMN::SetTitleFGColor(
  int       index,
  TCOLS*    colPtr,
  COLORREF  color)

  {
  if (((index == 0) && (index == numColumns)) ||
    (colPtr && (index>=0) && (index<numColumns)))
    colPtr[index].titleFG = color;
  else
    return;
  }

// COLUMN::SetTitleBGColor -- objd SetTitleBGColor /////////////////////////

void COLUMN::SetTitleBGColor(
  int       index,
  TCOLS*    colPtr,
  COLORREF  color)

  {
  if (((index == 0) && (index == numColumns)) ||
    (colPtr && (index>=0) && (index<numColumns)))
    colPtr[index].titleBG = color;
  else
    return;
  }

// COLUMN::SetCellFont -- objd SetCellFont /////////////////////////////////

void COLUMN::SetCellFont(
  int       index,
  TCOLS*    colPtr,
  LOGFONT*  lf,
  int       deciPointSize)

  {
  if(index<0 || (index != 0 && index == numColumns) || (index>numColumns) || !colPtr || !lf)
    return;

  colPtr[index].lfCell = *lf;
  colPtr[index].cellPtSize = deciPointSize;

  // fill in other font characteristics
  HDC         hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
  HFONT       hFont = AUtlCreateFontIndirect(lf);
  HFONT       oldFont = (HFONT)SelectObject(hDC, hFont);
  TEXTMETRIC  tm;
  GetTextMetrics(hDC, &tm);
  colPtr[index].cellCharWidth = tm.tmAveCharWidth;
  colPtr[index].cellLeading = tm.tmInternalLeading;
  colPtr[index].cellAscent = tm.tmAscent;
  colPtr[index].cellDescent = tm.tmDescent;
  SelectObject(hDC, oldFont);
  AUtlDeleteFont(hFont);
  DeleteDC(hDC);
  }

// COLUMN::SetCellFGColor -- objd SetCellFGColor ///////////////////////////

void COLUMN::SetCellFGColor(
  int       index,
  TCOLS*    colPtr,
  COLORREF  color)

  {
  if (((index == 0) && (index == numColumns)) ||
    (colPtr && (index>=0) && (index<numColumns)))
    colPtr[index].cellFG = color;
  }

// COLUMN::SetCellBGColor -- objd SetCellBGColor ///////////////////////////

void COLUMN::SetCellBGColor(
  int       index,
  TCOLS*    colPtr,
  COLORREF  color)

  {
  if (((index == 0) && (index == numColumns)) ||
    (colPtr && (index>=0) && (index<numColumns)))
    colPtr[index].cellBG = color;
  }

// COLUMN::SetObjectID -- objd SetObjectID /////////////////////////////////

void COLUMN::SetObjectID(
  int       index,
  TCOLS*    colPtr,
  OBJECTID  newID)

  {
  if (((index == 0) && (index == numColumns)) ||
    (colPtr && (index>=0) && (index<numColumns)))
    colPtr[index].objRef = newID;
  }

// COLUMN::MaxWidth ////////////////////////////////////////////////////////

int COLUMN::MaxWidth(
  int   index,
  BOOL  autoFill,
  long  first,
  long  numLoaded)

  {
  if ((numColumns<0) || (numColumns == 0 && index != numColumns))
    return 0;

  HDC   hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
  HFONT hFont = AUtlCreateFontIndirect(TitleLogFont(index));
  HFONT oldFont = (HFONT)SelectObject(hDC, hFont);
  int   max = LOWORD(GetTextExtent(hDC, Title(index), lstrlen(Title(index))));
  SelectObject(hDC, oldFont);
  AUtlDeleteFont(hFont);

  hFont = AUtlCreateFontIndirect(CellLogFont(index));
  oldFont = (HFONT)SelectObject(hDC, hFont);

  RUNTIMEINFO*  rtime = (RUNTIMEINFO*)AObjGetRuntimeInfo(theObject);
  ROW*          row = rtime->GetRow();

  if(!autoFill)
    {
    first = 0L;
    numLoaded = row->NumRows();
    }

  int         length;
  WORD        len;
  BOOL        segProb;
  char*       txt;
  char huge*  text;
  for(long i=first; i<numLoaded; i++)
    {
    segProb = FALSE;
    text = row->GetCell(i, index, FALSE, NULL);
    txt = (char*)text;
    length = (int)hstrlen(text);
    if(SELECTOROF(text) != SELECTOROF(text+(length+1)))
      {
      segProb = TRUE;
      txt = new char[length+1];
      hmemcpy(txt, text, length+1);
      }

    len = LOWORD(GetTextExtent(hDC, txt, length));
    if(len>max)
      max = len;

    if(segProb)
      delete txt;
    }
    
  SelectObject(hDC, oldFont);
  AUtlDeleteFont(hFont);
  DeleteDC(hDC);
  return (max + CellCharWidth(index)*2);
  }

////////////////////////////////////////////////////////////////////////////
