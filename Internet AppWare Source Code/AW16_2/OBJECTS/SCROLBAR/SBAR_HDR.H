////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//  A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//  THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//  ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          "sbar_hdr.h"
//
// AUTHOR:
//
// DESCRIPTION:   General header file for the Scroll Bar object
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef   SBAR_HDR_H  // prevent multiple includes
#define   SBAR_HDR_H

#ifndef   A_ALM_H
#include  <a_alm.h>
#endif

#ifndef O_NUMBER_H
#include <o_number.h>
#endif

// constants ///////////////////////////////////////////////////////////////

// version constants
#define VERSION_OBJECT              0
#define VERSION_DLL                 0L

// help constants
#define SERIUS_HELPFILE             "serhelp.hlp"
#define HELPID_OBJD_Sbar            10000

// data constants
#define DATA_SBARDATA               0L
#define NUM_SBARDATA                1L

// object changed constants
#define SCROLLBAR_OBJECTCHANGED     0x00000000L
#define CHANGED_THUMBPOSITION       0x00000001L
#define CHANGED_SCROLLRANGE         0x00000002L
#define CHANGED_ENABLEDSTATE        0x00000004L

// signal constants
#define SIGNAL_LINEUP               0L
#define SIGNAL_LINEDOWN             1L
#define SIGNAL_PAGEUP               2L
#define SIGNAL_PAGEDOWN             3L
#define SIGNAL_TOP                  4L
#define SIGNAL_BOTTOM               5L
#define SIGNAL_THUMBTRACK           6L
#define SIGNAL_THUMBPOSITION        7L
#define SIGNAL_ACTIVATED            8L
#define SIGNAL_DEACTIVATED          9L
#define SIGNAL_CHANGED              10L
#define SIGNAL_NUMSIGNALS           11L

#define SIGNAME_VLINEUP             "Scroll Line Up"
#define SIGNAME_VLINEDOWN           "Scroll Line Down"
#define SIGNAME_VPAGEUP             "Scroll Page Up"
#define SIGNAME_VPAGEDOWN           "Scroll Page Down"
#define SIGNAME_VTOP                "Scroll Top"
#define SIGNAME_VBOTTOM             "Scroll Bottom"
#define SIGNAME_VTHUMBTRACK         "Slider Track"
#define SIGNAME_VTHUMBPOSITION      "Slider Position"
#define SIGNAME_VACTIVATED          "Activated"
#define SIGNAME_VDEACTIVATED        "Deactivated"
#define SIGNAME_VCHANGED            "User Changed"

#define SIGNAME_HLINEUP             "Scroll Line Left"
#define SIGNAME_HLINEDOWN           "Scroll Line Right"
#define SIGNAME_HPAGEUP             "Scroll Page Left"
#define SIGNAME_HPAGEDOWN           "Scroll Page Right"
#define SIGNAME_HTOP                "Scroll Full Left"
#define SIGNAME_HBOTTOM             "Scroll Full Right"
#define SIGNAME_HTHUMBTRACK         "Slider Track"
#define SIGNAME_HTHUMBPOSITION      "Slider Position"
#define SIGNAME_HACTIVATED          "Activated"
#define SIGNAME_HDEACTIVATED        "Deactivated"
#define SIGNAME_HCHANGED            "User Changed"

#define scrollModifiers             (AEVT_KILLPARTICULARS | AEVT_ATTAIL)

// scroll bar type constants
#define scrollBarTypeID             MAKETYPEID('S', 'b', 'a', 'r')

// scroll bar event ids
#define OSBAR_EVT_MAYBEPOSTSIGNALS  MAKEEVENTID('s', 'b', 'm', 'P')

// Global container class for the scroll bar object ////////////////////////

class GLOBAL
  {
  private:

  public:
    HINSTANCE hInstance;
  };

// function prototypes /////////////////////////////////////////////////////

extern "C"
  {
  // functions included in "sbr_func.cpp"
  void far pascal _export Sbar_Disable(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Sbar_Enable(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Sbar_GetIncrement(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Sbar_GetPosition(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Sbar_GetRange(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Sbar_IsEnabled(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Sbar_SetIncrement(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Sbar_SetPosition(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Sbar_SetRange(pAEvtInfo theEvent, pAObjMessage theSystem);

  // functions included in "sbr_evnt.cpp"
  void far pascal _export Sbar_objectChanged(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Sbar_startupEvent(pAEvtInfo theEvent, pAObjMessage theSystem);
  void far pascal _export Sbar_MaybePostSignals(pAEvtInfo theEvent, pAObjMessage theSystem);

  // functions included in "sbr_main.cpp"
  int far pascal _export  LibMain(HINSTANCE hInstance, WORD wDataSeg, WORD wHeapSize, LPSTR lpszCmdLine);
  int far pascal _export  WEP(int nParameter);
  long far pascal _export GetSerVersion(TYPEID theType);
  LONG far pascal _export Sbar_Admn(OBJECTID theEvent, pAObjMessage theSystem);
  LONG far pascal _export Sbar_ObjD(OBJECTID theObject, pAObjMessage theSystem);
  }

// functions in "sbr_main.cpp"
LONG Sbar_doAssign(OBJECTID dstObject, OBJECTID srcObject);
LONG Sbar_doVersionCheck(OBJECTID theObject, long& version, long& status, char* error);
LONG Sbar_editObject(OBJECTID theObject);
void Sbar_buildSketchSignals(OBJECTID theObject, MEMBLOCKID theBlock);
BOOL far pascal _export Sbar_ObjDDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef SBAR_HDR_H
