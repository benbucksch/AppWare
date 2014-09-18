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
    OBJECTID  theObject;
    TATTRIB   tblAttrib;
    HGLOBAL   hCols;
    int       numCols;
    friend BOOL far pascal _export  Tabl_DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

    public:
    TABLE_OBJD(OBJECTID objID);
    ~TABLE_OBJD() {if(hCols) GlobalFree(hCols);}

    // inline methods
    OBJECTID  GetObject()                   {return theObject;}
    int       NumColumns()                  {return numCols;}
    void      DeleteColumn(int index)       {COLUMN::DeleteColumn(index, hCols, numCols);}
    OBJECTID  RowObject()                   {return TABLE_ATTRIBUTES::RowIndexObject();}
    void      SetRowObject(OBJECTID rowObj) {TABLE_ATTRIBUTES::SetRowIndexObject(rowObj);}

    // methods
    void      GetDefaults();
    void      InitDialogControls(HWND hDlg);
    void      FillAvailableObjects(HWND hDlg, WPARAM wParam);
    void      SetAddEnableState(HWND hDlg);
    void      SetEnabledState(HWND hDlg);
    void      UpdateObjectData();
    void      FillColumnsInTable(HWND hDlg);
    void      EditableHit(HWND hDlg);
    void      ChangeTitleAlignmentRB(HWND hDlg, WPARAM wParam, BOOL changeData);
    void      ChangeAlignmentRadioButton(HWND hDlg, WPARAM wParam, BOOL changeData);
    void      ChangeWidthRadioButton(HWND hDlg, WPARAM wParam);
    void      Add(HWND hDlg);
    void      Remove(HWND hDlg);
    void      SetColumnAttributes(HWND hDlg);
    LONG      Edit(HWND hDlg);
    BOOL      ListBoxCommand(HWND hDlg, WPARAM wParam, LPARAM lParam);
    BOOL      EditControlCommand(HWND hDlg, WPARAM wParam, LPARAM lParam);
    BOOL      FontDialog(HWND hDlg, WPARAM wParam);
    OBJECTID  GetColumnObjectID(int index);
    void      SetColumnObjectID(int index, OBJECTID colObj);

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
