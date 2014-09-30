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
// FILE:  "tabl_hdr.h"
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __TABL_HDR_H  // prevent multiple includes
#define __TABL_HDR_H

#define OTYPE_TABLE MAKETYPEID('T','a','b','l')

// General constants for the table object //////////////////////////////////

// data indexes
#define DATA_TABLEATTRIBUTES      0L
#define DATA_COLUMN               1L
#define DATA_ROWOFFSETS           2L
#define DATA_CELLDATA             3L
#define DATA_SELECTIONS           4L
#define DATA_OLDSELECTIONS        5L
#define DATA_RECORDIDS            6L
#define NUM_TABLEDATA             7L

// visual instance management data -- run-time only
#define DATA_VIMGR                7L

// signal indexes
#define SIGNAL_ROWCLICKED         0
#define SIGNAL_ROWDOUBLECLICKED   1
#define SIGNAL_SWITCHCOLUMNS      2
#define SIGNAL_SELECTIONCHANGED   3
#define SIGNAL_NEEDSROW           4
#define SIGNAL_TABLEAUTOFILLED    5
#define SIGNAL_RETURNPRESSED      6
#define SIGNAL_TABLEACTIVATED     7
#define SIGNAL_TABLEDEACTIVATED   8
#define SIGNAL_TABLEEDITED        9
#define SIGNAL_DELETEPRESSED      10
#define SIGNAL_STARTEDITING       11
#define SIGNAL_STOPEDITING        12
#define SIGNAL_STARTINGAUTOFILL   13
#define NUM_SIGNALS_CONNECTED     14
#define NUM_SIGNALS_NOCONNECT     11

// signal name constants
#define SIGNAME_ROWCLICKED        "Single-Click"
#define SIGNAME_ROWDOUBLECLICKED  "Double-Click"
#define SIGNAME_SWITCHCOLUMNS     "Column Activated"
#define SIGNAME_SELECTIONCHANGED  "Selection Changed"
#define SIGNAME_NEEDSROW          "Table Needs Row"
#define SIGNAME_TABLEAUTOFILLED   "Table Auto-filled"
#define SIGNAME_RETURNPRESSED     "Return/Enter Key"
#define SIGNAME_TABLEACTIVATED    "Table Activated"
#define SIGNAME_TABLEDEACTIVATED  "Table Deactivated"
#define SIGNAME_TABLEEDITED       "Table Field Edited"
#define SIGNAME_DELETEPRESSED     "Delete/Backspace Key"
#define SIGNAME_STARTEDITING      "Start Editing Field"
#define SIGNAME_STOPEDITING       "Stop Editing Field"
#define SIGNAME_STARTINGAUTOFILL  "Auto Fill Started"

// color constants
#define COLOR_BLACK               RGB(0, 0, 0)
#define COLOR_DKGRAY              RGB(128, 128, 128)
#define COLOR_LTGRAY              RGB(192, 192, 192)
#define COLOR_WHITE               RGB(255, 255, 255)

// method name contants
#define TRACKCURSOR               MAKEID('t', 'r', 'k', 'C')
#define trackCursor               TRACKCURSOR
#define DOUBLECLICKWAIT           MAKEID('d', 'b', 'c', 'W')
#define doubleClickWait           DOUBLECLICKWAIT

// mouse tracking flags
#define TRACK_TABLEACTIVE         0x0001
#define TRACK_COLUMNRESIZE        0x0002
#define TRACK_CELLHIT             0x0004
#define TRACK_SHIFTHIT            0x0008
#define TRACK_CONTROLHIT          0x0010
#define TRACK_HIT                 (TRACK_CELLHIT | TRACK_SHIFTHIT | TRACK_CONTROLHIT)

// AEVENT_OBJECTCHANGED event modifiers
#define CHANGED_COLUMNWIDTH       0x00000001L
#define CHANGED_COLUMNSELECTION   0x00000002L
#define CHANGED_ROWSELECTION      0x00000004L
#define CHANGED_NUMROWS           0x00000008L
#define CHANGED_NUMCOLUMNS        0x00000010L
#define CHANGED_SCROLLTOSELECTION 0x00000020L
#define CHANGED_FULLREDRAW        0x00000040L
#define CHANGED_AUTOUPDATE        0x00000080L
#define CHANGED_KILLEDIT          0x00000100L

// operation values
#define OP_INVALID                (-1)
#define OP_SUM                    1
#define OP_AVERAGE                2
#define OP_MIN                    3
#define OP_MAX                    4

// print margins in inches
#define MAR_LEFT                  0.5
#define MAR_TOP                   0.5
#define MAR_RIGHT                 0.5
#define MAR_BOTTOM                0.5

// found constants
#define FOUND_INVALIDROW          1
#define FOUND_FOUND               2
#define FOUND_NOTFOUND            3

// sort contants
#define SORT_ASCENDING            1
#define SORT_DESCENDING           2

// version constants
#define VERSION_DLL               2L
#define VERSION_OBJECT            2   // incremented for the 1993 maintence release

// Pre April 27, 1994 new table version
#define VERSION_DLL1_0            1L
#define VERSION_OBJECT1_0         1   // incremented for the 1994 Adhoc Table

// other constans
#define VGA_RESOLUTION            96
#define SERIUS_HELPFILE           "serhelp.hlp"
#define TABLE_BORDERWIDTH         3
#define TABLE_OBJECTCHANGED       0
#define COLUMN_RESIZEHITRANGE     5
#define WINDOW_BACKGROUND         0xFF000000L
#define WINDOW_FOREGROUND         0xFE000000L
#define AUTOFILL_MAXROWS          200


/////////////////////////////////////////
///  Stuff for the Window Item Protocol
/////////////////////////////////////////
#define SF_VISIBLE            0x00000001L
#define SF_CANACTIVATE        0x00000002L
#define SF_ACTIVE             0x00000004L
#define SF_TABSTOP            0x00000008L
#define SF_STARTUP            0x00000010L
#define SF_USESCURSORKEYS     0x00000020L
#define SF_USESTAB            0x00000040L
#define SF_ENTERTHIEF         0x00000080L
#define SF_ENTERTHIEFPROOF    0x00000100L
#define SF_ENTERTHIEFCAPTOR   0x00000200L
#define SF_ENTERTHIEFLEADER   0x00000400L
#define SF_TRANSPARENT        0x00000800L
#define SF_DRAWME             0x00001000L


// constant to indicate extra Window Bytes
// Used to store C++ class instance (TABLE)
// SKJ- appware migration Aug.31, 1993
#define TABLE_OBJSIZE             4
#define LTABLE_INDEX              0

// typedefs ////////////////////////////////////////////////////////////////

typedef char _huge*               HPSTR;
typedef const char _huge*         HPCSTR;

// Global container class for the table object /////////////////////////////

class GLOBAL
  {
  private:

  public:
    HINSTANCE     hInstance;
    static char*  CRLF;
    int           cbByteOffset;
    HCURSOR       hResizeCursor;
    HBITMAP       hBitmap;
    int           DblClkSpeed;  // in 1/60 second increments
  };

// Event-handling Function prototypes //////////////////////////////////////

// Serius & API entry points need to be C declarations
extern "C"
  {
  // included in "tbl_admn.cpp"
  long far pascal _export Tabl_Admn(OBJECTID objectID, pAObjMessage theSystem);
  long far pascal _export GetSerVersion(TYPEID theType);

  // included in "tbl_func.cpp"
  void far pascal _export Tabl_ActivateColumn(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_ActiveCell(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_ActiveColumn(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_AddRow(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_CellEdited(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_ClearSelections(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_ClearTable(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_ColumnCalculation(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_CountRows(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_DeleteColumn(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_DeleteRow(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_Find(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_FindExact(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_GetCell(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_GetColumn(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_GetColumnTitle(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_GetColumnWidth(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_GetDataSource(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_GetIndexedField(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_GetRecordID(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_GetSelection(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_InsertColumn(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_RefreshAutoTable(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_RowCalculation(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_RowToObjects(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_RowToText(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_ScrollToSelection(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_SetCell(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_SetColumn(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_SetColumnTitle(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_SetColumnWidth(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_SetDataSource(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_SetIndexedField(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_SetRow(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_SetSelection(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_SortAscending(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_SortDescending(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_SortRowAscending(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_SortRowDescending(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_UpdateAutoTable(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_DataLength(pAEvtInfo theEvent, pAObjMessage theSystem);

  // included in "tbl_main.cpp"
  int far pascal _export  LibMain(HINSTANCE hInstance, WORD wDataSeg, WORD wHeapSize, LPSTR lpszCmdLine);
  int far pascal _export  WEP(int nParameter);
  
  // included in "tbl_mthd.cpp"
  void far pascal _export Tabl_startupEvent(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_trackCursor(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_editCut(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_editCopy(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_editPaste(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_editUndo(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_doubleClickWait(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_objectChanged(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Tabl_printItemDrawRoutine(pAEvtInfo theEvent, pAObjMessage theSystem);
  void ALMAPI Tabl_AdhocViewChanged(pAEvtInfo  theEvent, pAObjMessage     theSystem);
  }

// callback entry points -- C declarations not necessary
// included in "tbl_clmn.cpp"
void Tabl_UpdateColumns(OBJECTID theObject, int oldVersion, BOOL colLines);

// included in "tabl_row.cpp"
void Tabl_UpdateRows(OBJECTID theObject, int oldVersion);

// included in "tbl_main.cpp"
long far pascal _export WIDRTableWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
long far pascal _export VisualTableWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// included in "tbl_objd.cpp"
BOOL far pascal _export Tabl_DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL far pascal _export Tabl_AttribProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
long far pascal _export TestTableWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// included in "tabl_lib.cpp"
// huge pointer functions
#if (WINVER < 0x030a)
void hmemcpy(void _huge* dest, const void _huge* source, long size);
#endif  // WINVER < 0x030a
void hmemmove(void _huge* dest, const void _huge* source, long size);
long hstrlen(const char _huge* string);
int hstrcmp(const char _huge* str1, const char _huge* str2);
int hstricmp(const char _huge* str1, const char _huge* str2);
BOOL lfcmp(const LOGFONT* lf1, const LOGFONT* lf2);
int hmemicmp(const char _huge* str1, const char _huge* str2, long count);
void hmemset(void _huge* s, int c, long n);
char huge* myStrtok(char huge* source, BOOL tabDelimited=FALSE);

// inline functions
inline long min(long var1, long var2) {return (var1<var2) ? var1 : var2;}

// Set the number object
BOOL Tbl_SetNmbrObj(OBJECTID n_oid, long double ldVal);
long double  Tbl_GetNmbrObj(OBJECTID n_oid);
long double ONmbrConvertFromTextReal(OBJECTID oid,LPSTR txt);
long ONmbrConvertFromTextInteger(OBJECTID oid,LPSTR txt);

////////////////////////////////////////////////////////////////////////////
#endif  // end....#ifndef __TABL_HDR_H
