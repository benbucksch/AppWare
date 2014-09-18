///////////////////////////////////////////////////////////////
//
//  (c) 1993 Novell, Inc.  All rights reserved.
//
//  THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//  TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//  COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//  CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//  TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//  NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//  AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//  LIABILITY.
//
///////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
// Object Group:  Essentials
// Object:          Printer
//
// Filename:  "print.cpp"
//
// Author:  Kevin Smart
//   Start:   July 24, 1992
//   Finish:  July 24, 1992
//
// Description:
//  Code for PRINTER class that manages the printer object
//
////////////////////////////////////////////////////////////////////////////

#include  <a_alm.h>
#include  <a_almutl.h>
#include  <string.h>
#include  <intdebug.h>

#include  "prntobj.h"
#include  "print.hpp"

////////////////////////////////////////////////////////////////////////////

PRINTER::PRINTER(OBJECTID obj, HWND hWnd)

{
  theObject = obj;
  long  count = AObjGetDataCount(theObject);
  long  d1;
  long  d2;
  void* data1;
  void* data2;
  void* temp;


  // build the PRINTDLG structure handles
  if(count<3L) {
    if(!AObjSetDataCount(theObject, 3L)) {
      MessageBox(GetActiveWindow(), "Unable to create the necessary data for the printer object.",
                 NULL, MB_OK | MB_ICONEXCLAMATION);
      return;
    }
    if (AObjResizeData(theObject, 0L, sizeof(DATA0))) {
      data0 = (DATA0*)AObjLockData(theObject, 0L);
      data0->ver              = VERSION_OBJECT;
      data0->PersistentFlags  = 0L;
      data0->FromPage         = DFLT_FROMPAGE;
      data0->ToPage           = DFLT_TOPAGE;
      pd.hDevMode             = NULL;
      pd.hDevNames            = NULL;
    }     // if (AObjResizeData(theObject,...
  }  // if(count<3L)...
  else {
    data0 = (DATA0*)AObjLockData(theObject, 0L);
    if(!data0) {
      if (AObjResizeData(theObject, 0L, sizeof(DATA0))) {
        data0 = (DATA0*)AObjLockData(theObject, 0L);
        data0->ver              = VERSION_OBJECT;
        data0->PersistentFlags  = 0L;
        data0->FromPage         = DFLT_FROMPAGE;
        data0->ToPage           = DFLT_TOPAGE;
      }     // if (AObjResizeData(theObject,...
    }     // if(!data0)...
    data1 = AObjLockData(theObject, 1L);
    d1 = AObjGetDataSize(theObject, 1L);
    if(data1) {
      if((pd.hDevMode=GlobalAlloc(GMEM_MOVEABLE, d1))==NULL) {
        MessageBox(GetActiveWindow(), "Unable to allocate enough memory for the printer object.",
        NULL, MB_OK | MB_ICONEXCLAMATION);
        AObjUnlockData(theObject, 0L);
        AObjUnlockData(theObject, 1L);
        return;
      }     // if((pd.hDevMode=GlobalAlloc(...
      if((temp=GlobalLock(pd.hDevMode))==NULL) { // if memory locking failure
        MessageBox(GetActiveWindow(), "Unable to lock the memory for the printer object.",
                   NULL, MB_OK | MB_ICONEXCLAMATION);
        AObjUnlockData(theObject, 0L);
        AObjUnlockData(theObject, 1L);
        return;
      }     // if((temp=GlobalLock(pd....
      _fmemcpy(temp, data1, (size_t)d1);  // update info in global memory
      AObjUnlockData(theObject, 1L);
      GlobalUnlock(pd.hDevMode);          // unlock it.
    }  // if(data1)
    else
      pd.hDevMode = NULL;
    data2 = AObjLockData(theObject, 2L);
    d2 = AObjGetDataSize(theObject, 2L);
    if(data2) {
      if((pd.hDevNames=GlobalAlloc(GMEM_MOVEABLE, d2))==NULL) {
        MessageBox(GetActiveWindow(), "Unable to allocate enough memory for the printer object.",
                   NULL, MB_OK | MB_ICONEXCLAMATION);
        AObjUnlockData(theObject, 0L);
        AObjUnlockData(theObject, 2L);
        return;
      }
      if((temp=GlobalLock(pd.hDevNames))==NULL) {  // memory locking failure
        MessageBox(GetActiveWindow(), "Unable to lock the memory for the printer object.",
                   NULL, MB_OK | MB_ICONEXCLAMATION);
        AObjUnlockData(theObject, 0L);
        AObjUnlockData(theObject, 2L);
        return;
      }
      _fmemcpy(temp, data2, (size_t)d2);  // update info in global memory
      AObjUnlockData(theObject, 2L);
      GlobalUnlock(pd.hDevNames);         // unlock it.
    }  // if(data2)
    else
      pd.hDevNames = NULL;
  }  // if(count<3L)...else

  // fill in the rest of the PRINTDLG structure
  pd.lStructSize          = sizeof(PRINTDLG);
  pd.hwndOwner            = hWnd;
  pd.hDC                  = NULL;
  pd.Flags                = data0->PersistentFlags;
  pd.nFromPage            = data0->FromPage;
  pd.nToPage              = data0->ToPage;
  pd.nMinPage             = 1;
  pd.nMaxPage             = 9999;
  pd.nCopies              = 1;
  pd.hInstance            = 0;
  pd.lCustData            = 0L;
  pd.lpfnPrintHook        = (HOOKFUNC)PrintHook;
  pd.lpfnSetupHook        = NULL;
  pd.lpPrintTemplateName  = NULL;
  pd.lpSetupTemplateName  = NULL;
  pd.hPrintTemplate       = NULL;
  pd.hSetupTemplate       = NULL;
  AObjUnlockData(theObject, 0L);
}


////////////////////////////////////////////////////////////////////////////

PRINTER::~PRINTER()

{
  void* temp;
  long  length;

  data0 = (DATA0*)AObjLockData(theObject, 0L);
  if(data0->PersistentFlags & PD_PAGENUMS) {
    data0->FromPage = pd.nFromPage;
    data0->ToPage   = pd.nToPage;
    }
  else {
    data0->FromPage = DFLT_FROMPAGE;
    data0->ToPage   = DFLT_TOPAGE;
    }

  if (pd.hDevMode) {
    length  = GlobalSize(pd.hDevMode);
    temp  = GlobalLock(pd.hDevMode);
     
    if(!temp && length) {
      // memory locking failure
      MessageBox(GetActiveWindow(), "Unable to lock the memory for the printer object.",
                 NULL, MB_OK | MB_ICONEXCLAMATION);
      AObjSetData(theObject, 1L, NULL, 0L);
      }
    else {
      AObjSetData(theObject, 1L, temp, length);
      }
    GlobalUnlock(pd.hDevMode);
    GlobalFree(pd.hDevMode);
    }  // end...if(pd.hDevMode)
  else {
    AObjSetData(theObject, 1L, NULL, 0L);
    }

  if (pd.hDevNames) {
    length  = GlobalSize(pd.hDevNames);
    temp  = GlobalLock(pd.hDevNames);
     
    if(!temp && length) {
      // memory locking failure
      MessageBox(GetActiveWindow(), "Unable to lock the memory for the printer object.",
                 NULL, MB_OK | MB_ICONEXCLAMATION);
      AObjSetData(theObject, 2L, NULL, 0L);
      }
    else {
      AObjSetData(theObject, 2L, temp, length);
      }
    GlobalUnlock(pd.hDevNames);
    GlobalFree(pd.hDevNames);
    }  // end...if(pd.hDevNames)
  else {
    AObjSetData(theObject, 2L, NULL, 0L);
    }
  AObjUnlockData(theObject, 0L);
}


////////////////////////////////////////////////////////////////////////////

UINT far pascal _export PRINTER::PrintHook(HWND hDlg, WORD message,
           WORD wParam, long lParam)
{
  I_UNUSED(wParam);
  I_UNUSED(lParam);

  switch(message) {
    case WM_INITDIALOG :
      ShowWindow(GetDlgItem(hDlg, 1024), SW_HIDE);
      break;

    default : break;
  }
 return FALSE;
}


////////////////////////////////////////////////////////////////////////////

BOOL  PRINTER::PrintDlg(DWORD flags)

{
  pd.Flags = flags;
  return ::PrintDlg(&pd);
}


////////////////////////////////////////////////////////////////////////////

UINT  PRINTER::FromPage()
{
  if (pd.Flags & PD_PAGENUMS)
    return pd.nFromPage;
  else
    return 1;
}


////////////////////////////////////////////////////////////////////////////

UINT  PRINTER::ToPage()
{
  if (pd.Flags & PD_PAGENUMS)
    return pd.nToPage;
  else
    return 9999;  // maximum number of pages in the Macintosh
}


////////////////////////////////////////////////////////////////////////////
