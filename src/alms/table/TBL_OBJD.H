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
// FILE:  "tbl_objd.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __TBL_OBJD_H  // prevent multiple includes
#define __TBL_OBJD_H


#ifndef __TBL_ATRB_H
#include <tbl_atrb.h>
#endif

#ifndef __TBL_CLMN_H
#include <tbl_clmn.h>
#endif

// TABLE_OBJD class -- for easy OBJD management ///////////////////////////

class TABLE_OBJD :
  private TABLE_ATTRIBUTES,
  private COLUMN
  {
  private:
    AOBJECTID  theObject;
    TATTRIB   tblAttrib;
    HGLOBAL   hCols;
    int       numCols;
    friend BOOL CALLBACK /*!!PORT!! was _far_ _pascal_*/   Tabl_DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

    public:
    TABLE_OBJD(AOBJECTID objID);
    ~TABLE_OBJD() {if(hCols) GlobalFree(hCols);}

    // inline methods
    AOBJECTID  GetObject()                   {return theObject;}
    int       NumColumns()                  {return numCols;}
    void      DeleteColumn(int index)       {COLUMN::DeleteColumn(index, hCols, numCols);}
    AOBJECTID  RowObject()                   {return TABLE_ATTRIBUTES::RowIndexObject();}
    void      SetRowObject(AOBJECTID rowObj) {TABLE_ATTRIBUTES::SetRowIndexObject(rowObj);}

    // methods
    void      GetDefaults();
    void      InitDialogControls(HWND hDlg);
    void      FillAvailableObjects(HWND hDlg, WORD wId);
    void      SetAddEnableState(HWND hDlg);
    void      SetEnabledState(HWND hDlg);
    void      UpdateObjectData();
    void      FillColumnsInTable(HWND hDlg);
    void      EditableHit(HWND hDlg);
    void      ChangeTitleAlignmentRB(HWND hDlg, WORD wCmd, BOOL changeData);
    void      ChangeAlignmentRadioButton(HWND hDlg, WORD wCmd, BOOL changeData);
    void      ChangeWidthRadioButton(HWND hDlg, WORD wCmd);
    void      Add(HWND hDlg);
    void      Remove(HWND hDlg);
    void      SetColumnAttributes(HWND hDlg);
    LONG      Edit(HWND hDlg);
    BOOL      ListBoxCommand(HWND hDlg, LPARAM lParam, WORD wCmd, WORD wId);
    BOOL      EditControlCommand(HWND hDlg, WORD wCmd, WORD wId);
    BOOL      FontDialog(HWND hDlg, WORD wCmd, WORD wId);
    AOBJECTID  GetColumnObjectID(int index);
    void      SetColumnObjectID(int index, AOBJECTID colObj);

    // methods for the attributes dialog
    void      InitAttributeControls(HWND hDlg);
    void      FillDataSource(HWND hDlg);
    void      FillIndexedField(HWND hDlg);
    void      FillRowIndex(HWND hDlg);
    void      EnableComboBoxes(HWND hDlg);
    void      UpdateTableAttributes(HWND hDlg);
  };

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef __TBL_OBJD_H
