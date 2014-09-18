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
// FILE:          "scroll.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   This is the source file for the Scroll class (ALM Object).
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <windows.h>
#include <windowsx.h>

#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>
#include <op_wnd.h>
#include <o_number.h>

#include "sbar_rsc.h"
#include "almobase.h"
#include "resstrng.h"
#include "scroll.h"

// global variables ////////////////////////////////////////////////////////

extern HINSTANCE  g_hinstDLL;
extern OpWndItemD* ALMAPI CreateScrollBarVI(OBJECTID    oiItem,
                                            OBJECTID    oiParent,
                                            LPRECT      rcItem,
                                            long        lStatus,
                                            MEMBLOCKID  miExtra);

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::Scroll(OBJECTID, BOOL) -- constructor
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

Scroll::Scroll
  (
  OBJECTID  oiScroll,
  BOOL      fTemp
  ) : ALMOBase(oiScroll, fTemp),
      m_lMin(1),
      m_lMax(100),
      m_lLine(1),
      m_lPage(10),
      m_fStatus(enabled | focus | vertical | defaultSize | (fTemp ? temporary : 0)),
      m_oiPosition(NULL),
      m_lCurrPos(1)

  {
  long  nData = AObjGetDataCount(ObjectID());

  if(k_lNumDataBlocks == nData)
    InitializeFromData();

  AObjSetDataCount(ObjectID(), 0L);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::~Scroll() -- destructor
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

Scroll::~Scroll
  (
  )

  {
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::CheckVersion(OBJECTID) -- static
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

long Scroll::CheckVersion
  (
  OBJECTID  oi
  )

  {
  long    lRet = A_ERROR;
  short*  psVer = (short*)AObjLockData(oi, k_lSBarData);

  if(psVer)
    {
    if(*psVer < k_sVersion)
      {
      OldSBarData*  osbd = (OldSBarData*)psVer;
      SBarData      sbd;

      // upgrade the old object
      sbd.sVer        = k_sVersion;
      sbd.lMin        = osbd->lMin;
      sbd.lMax        = osbd->lMax;
      sbd.lLine       = osbd->lLine;
      sbd.lPage       = osbd->lPage;
      sbd.lPos        = osbd->lPos;
      sbd.fStatus     = 0;
      sbd.oiPosition  = osbd->oiPosition;

      if(osbd->fEnabled)
        sbd.fStatus |= enabled;
      if(osbd->fVertical)
        sbd.fStatus |= vertical;
      if(osbd->fCanActivate)
        sbd.fStatus |= focus;
      if(osbd->fDefaultSize)
        sbd.fStatus |= defaultSize;

      // set the object data
      AObjUnlockData(oi, k_lSBarData);
      if(AObjSetData(oi, k_lSBarData, &sbd, sizeof(SBarData)))
        lRet = A_WARNING;
      }
    else if(*psVer == k_sVersion)
      {
      AObjUnlockData(oi, k_lSBarData);
      lRet = A_OK;
      }
    else
      {
      AObjUnlockData(oi, k_lSBarData);
      lRet = A_ERROR;
      }
    }

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::InitializeFromData()
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

void Scroll::InitializeFromData
  (
  )

  {
  SBarData* sbd = (SBarData*)AObjLockData(ObjectID(), k_lSBarData);

  if(sbd)
    {
    m_lMin        = sbd->lMin;
    m_lMax        = sbd->lMax;
    m_lLine       = sbd->lLine;
    m_lPage       = sbd->lPage;
    m_oiPosition  = sbd->oiPosition;
    m_lCurrPos    = sbd->lPos;
    m_fStatus     = (m_fStatus & temporary) | (sbd->fStatus & ~temporary);

    AObjUnlockData(ObjectID(), k_lSBarData);
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::Assign(const ALMOBase*)
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

long Scroll::Assign
  (
  const ALMOBase* pSrc
  )

  {
  const Scroll* pSrcScroll  = (const Scroll*)pSrc;
  long          lRet;

  if(pSrcScroll != this)
    {
    BOOL  fChanged;

    fChanged = SetMinimum(pSrcScroll->Minimum());
    fChanged = SetMaximum(pSrcScroll->Maximum())        || fChanged;
    fChanged = SetLineScroll(pSrcScroll->LineScroll())  || fChanged;
    fChanged = SetPageScroll(pSrcScroll->PageScroll())  || fChanged;
    fChanged = SetPosition(pSrcScroll->Position())      || fChanged;

    if(fChanged)
      AEvtPostSignalAtMark(ObjectID(), k_lSignalChanged);

    lRet = A_OK;
    }
  else
    lRet = A_ERROR;

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::BuildSignals(MEMBLOCKID) const
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

long Scroll::BuildSignals
  (
  MEMBLOCKID  mi
  ) const

  {
  /* Structure for storing the static signal table.
   */
  struct SignalInfo
    {
    /* This structure is used as a convenient way to store signal
     * names and ids, so they can all be looked at in one location.
     */
    long    lIdx;     /* signal id    */
    LPCSTR  szName;   /* signal name  */
    };

  // signal names
  static const ResString  k_rsSignalVLineUp(STR_SIGNAL_VLINEUP);
  static const ResString  k_rsSignalVLineDown(STR_SIGNAL_VLINEDOWN);
  static const ResString  k_rsSignalVPageUp(STR_SIGNAL_VPAGEUP);
  static const ResString  k_rsSignalVPageDown(STR_SIGNAL_VPAGEDOWN);
  static const ResString  k_rsSignalVTop(STR_SIGNAL_VTOP);
  static const ResString  k_rsSignalVBottom(STR_SIGNAL_VBOTTOM);
  static const ResString  k_rsSignalHLineUp(STR_SIGNAL_HLINEUP);
  static const ResString  k_rsSignalHLineDown(STR_SIGNAL_HLINEDOWN);
  static const ResString  k_rsSignalHPageUp(STR_SIGNAL_HPAGEUP);
  static const ResString  k_rsSignalHPageDown(STR_SIGNAL_HPAGEDOWN);
  static const ResString  k_rsSignalHTop(STR_SIGNAL_HTOP);
  static const ResString  k_rsSignalHBottom(STR_SIGNAL_HBOTTOM);
  static const ResString  k_rsSignalThumbTrack(STR_SIGNAL_THUMBTRACK);
  static const ResString  k_rsSignalChanged(STR_SIGNAL_CHANGED);
  static const ResString  k_rsSignalThumbPosition(STR_SIGNAL_THUMBPOSITION);
  static const ResString  k_rsSignalActivated(STR_SIGNAL_ACTIVATED);
  static const ResString  k_rsSignalDeactivated(STR_SIGNAL_DEACTIVATED);

  static const SignalInfo vSignals[] =
    {
    /* This is a list of all the vertical scroll bar object signals.
     */
      {k_lSignalLineUp,         k_rsSignalVLineUp.String()},
      {k_lSignalLineDown,       k_rsSignalVLineDown.String()},
      {k_lSignalPageUp,         k_rsSignalVPageUp.String()},
      {k_lSignalPageDown,       k_rsSignalVPageDown.String()},
      {k_lSignalTop,            k_rsSignalVTop.String()},
      {k_lSignalBottom,         k_rsSignalVBottom.String()},
      {k_lSignalThumbTrack,     k_rsSignalThumbTrack.String()},
      {k_lSignalChanged,        k_rsSignalChanged.String()},
      {k_lSignalThumbPosition,  k_rsSignalThumbPosition.String()},
      {k_lSignalActivated,      k_rsSignalActivated.String()},
      {k_lSignalDeactivated,    k_rsSignalDeactivated.String()}
    };

  static const SignalInfo hSignals[] =
    {
    /* This is a list of all the horizontal scroll bar object signals.
     */
      {k_lSignalLineUp,         k_rsSignalHLineUp.String()},
      {k_lSignalLineDown,       k_rsSignalHLineDown.String()},
      {k_lSignalPageUp,         k_rsSignalHPageUp.String()},
      {k_lSignalPageDown,       k_rsSignalHPageDown.String()},
      {k_lSignalTop,            k_rsSignalHTop.String()},
      {k_lSignalBottom,         k_rsSignalHBottom.String()},
      {k_lSignalThumbTrack,     k_rsSignalThumbTrack.String()},
      {k_lSignalChanged,        k_rsSignalChanged.String()},
      {k_lSignalThumbPosition,  k_rsSignalThumbPosition.String()},
      {k_lSignalActivated,      k_rsSignalActivated.String()},
      {k_lSignalDeactivated,    k_rsSignalDeactivated.String()}
    };

  // number of signals for the scroll bar object
  static int        nNumSignals = sizeof(vSignals)/sizeof(vSignals[0]);

  // all scroll bar objects have the first (nNumSignals - 2L) the same
  pAObjSignalRecord pSignals;
  int               nNumSigs = nNumSignals - (CanActivate() ? 0 : 2);
  long              lRet;

  if(AMemSetSize(mi, nNumSigs * sizeof(AObjSignalRecord)))
    {
    pSignals = (pAObjSignalRecord)AMemLock(mi);
    if(pSignals)
      {
      const SignalInfo* signals = Vertical() ? vSignals : hSignals;

      // copy the signals from the constant signal array (in "sound.h")
      for(int i=0; i<nNumSigs; i++)
        {
        pSignals[i].theID = signals[i].lIdx;
        lstrcpy(pSignals[i].theName, signals[i].szName);
        }

      lRet = A_OK;
      }
    else
      lRet = A_ERROR;
    }
  else
    lRet = A_ERROR;

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::Check() const
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

long Scroll::Check
  (
  ) const

  {
  // the scroll bar will have no errors
  return A_OK;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::Edit()
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

long Scroll::Edit
  (
  )

  {
  long  lRet;
  BOOL  fValid = AObjCheckType(m_oiPosition, 0L);

  if(!fValid)
    SetPositionObject(NULL);

  if(DialogBoxParam(g_hinstDLL, MAKEINTRESOURCE(DLG_EDITSCROLLBAR), GetActiveWindow(), (DLGPROC)EditDlgProc, (LPARAM)this))
    lRet = A_MODIFIED;
  else
    lRet = A_OK;

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::Export() const
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

long Scroll::Export
  (
  ) const

  {
  short sVer          = k_sVersion;
  BOOL  fEnabled      = Enabled();
  BOOL  fCanActivate  = CanActivate();
  BOOL  fVertical     = Vertical();
  BOOL  fDefaultSize  = DefaultSize();

  AUpsfExportField("VERSION ", 0L, AUPSF_NATV_SHORT,      AUPSF_UNIV_INTEGER,   (void*)&sVer,          (long)sizeof(short));
  AUpsfExportField("MINIMUM ", 0L, AUPSF_NATV_LONG,       AUPSF_UNIV_INTEGER,   (void*)&m_lMin,        (long)sizeof(long));
  AUpsfExportField("MAXIMUM ", 0L, AUPSF_NATV_LONG,       AUPSF_UNIV_INTEGER,   (void*)&m_lMax,        (long)sizeof(long));
  AUpsfExportField("LINESCRL", 0L, AUPSF_NATV_LONG,       AUPSF_UNIV_INTEGER,   (void*)&m_lLine,       (long)sizeof(long));
  AUpsfExportField("PAGESCRL", 0L, AUPSF_NATV_LONG,       AUPSF_UNIV_INTEGER,   (void*)&m_lPage,       (long)sizeof(long));
  AUpsfExportField("POSOBJCT", 0L, AUPSF_NATV_OBJECTREF,  AUPSF_UNIV_OBJECTREF, (void*)&m_oiPosition,  (long)sizeof(OBJECTID));
  AUpsfExportField("ENABLED ", 0L, AUPSF_NATV_BOOL,       AUPSF_UNIV_BOOL,      (void*)&fEnabled,      (long)sizeof(BOOL));
  AUpsfExportField("CANACTVT", 0L, AUPSF_NATV_BOOL,       AUPSF_UNIV_BOOL,      (void*)&fCanActivate,  (long)sizeof(BOOL));
  AUpsfExportField("VERTICAL", 0L, AUPSF_NATV_BOOL,       AUPSF_UNIV_BOOL,      (void*)&fVertical,     (long)sizeof(BOOL));
  AUpsfExportField("DFLTSIZE", 0L, AUPSF_NATV_BOOL,       AUPSF_UNIV_BOOL,      (void*)&fDefaultSize,  (long)sizeof(BOOL));

  return A_OK;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::Import()
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

long Scroll::Import
  (
  )

  {
  BOOL  fEnabled      = TRUE;
  BOOL  fCanActivate  = TRUE;
  BOOL  fVertical     = TRUE;
  BOOL  fDefaultSize  = TRUE;

  AUpsfImportField("MINIMUM ", 0L, AUPSF_NATV_LONG,       &m_lMin,        (long)sizeof(long));
  AUpsfImportField("MAXIMUM ", 0L, AUPSF_NATV_LONG,       &m_lMax,        (long)sizeof(long));
  AUpsfImportField("LINESCRL", 0L, AUPSF_NATV_LONG,       &m_lLine,       (long)sizeof(long));
  AUpsfImportField("PAGESCRL", 0L, AUPSF_NATV_LONG,       &m_lPage,       (long)sizeof(long));
  AUpsfImportField("POSOBJCT", 0L, AUPSF_NATV_OBJECTREF,  &m_oiPosition,  (long)sizeof(OBJECTID));
  AUpsfImportField("ENABLED ", 0L, AUPSF_NATV_BOOL,       &fEnabled,      (long)sizeof(BOOL));
  AUpsfImportField("CANACTVT", 0L, AUPSF_NATV_BOOL,       &fCanActivate,  (long)sizeof(BOOL));
  AUpsfImportField("VERTICAL", 0L, AUPSF_NATV_BOOL,       &fVertical,     (long)sizeof(BOOL));
  AUpsfImportField("DFLTSIZE", 0L, AUPSF_NATV_BOOL,       &fDefaultSize,  (long)sizeof(BOOL));

  SetEnabled(fEnabled);
  SetCanActivate(fCanActivate);
  SetVertical(fVertical);
  SetDefaultSize(fDefaultSize);

  return A_OK;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::PostWrite()
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

long Scroll::PostWrite
  (
  )

  {
  AObjSetDataCount(ObjectID(), 0L);

  return A_CONTINUE;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::PreWrite()
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

long Scroll::PreWrite
  (
  )

  {
  SBarData  sbd;

  sbd.sVer        = k_sVersion;
  sbd.lMin        = m_lMin;
  sbd.lMax        = m_lMax;
  sbd.lLine       = m_lLine;
  sbd.lPage       = m_lPage;
  sbd.lPos        = m_lCurrPos;
  sbd.fStatus     = (m_fStatus & ~temporary);
  sbd.oiPosition  = m_oiPosition;

  // set the object data
  AObjSetDataCount(ObjectID(), k_lNumDataBlocks);
  AObjSetData(ObjectID(), k_lSBarData, &sbd, sizeof(SBarData));

  return A_CONTINUE;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::QueryProtocol() -- static
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

long Scroll::QueryProtocol
  (
  OBJECTID      oi,
  pApQueryInfo  pQI
  )

  {
  I_UNUSED(oi);

  long  lRet;

  if(pQI->protocol == APROT_WND)  // if the window protocol is supported
    {
    pQI->pfnCreate = (APPCFN) CreateScrollBarVI;
    lRet = AP_SUPPORTED;
    }
  else
    lRet = AP_NOTSUPPORTED;

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::ValidateIDs(pAObjMessage)
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

long Scroll::ValidateIDs
  (
  pAObjMessage  pMsg
  )

  {
  AObjValidateID(&m_oiPosition, pMsg);

  return A_OK;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::SetPosition(long)
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

BOOL Scroll::SetPosition
  (
  long  lPos
  )

  {
  BOOL  fRet = (lPos != m_lCurrPos);

  if(fRet)
    {
    if(MinLTMax())
      {
      if(lPos < Minimum())
        lPos = Minimum();
      else if(lPos > Maximum())
        lPos = Maximum();
      }
    else
      {
      if(lPos < Maximum())
        lPos = Maximum();
      else if(lPos > Minimum())
        lPos = Minimum();
      }

    m_lCurrPos = lPos;
    AEvtPostStandard(ObjectID(), AEVENT_OBJECTCHANGED, 0, AEVT_KILLMODIFIERS | AEVT_KILLDUPLICATES | AEvtGetObjectChangedPriority(AEVT_ATTAIL));

    if(PositionObject())
      ONmbrSetInteger(PositionObject(), m_lCurrPos, FALSE);
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::EditDlgProc(HWND, UINT, WPARAM, LPARAM)
//                      -- static, private
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

BOOL ALMCALLBACK Scroll::EditDlgProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  Scroll* pScroll;
  BOOL    fRet;

  switch(uMsg)
    {
    case WM_INITDIALOG:
      pScroll = (Scroll*)lParam;
      SetWindowLong(hwnd, DWL_USER, (long)pScroll);
      pScroll->InitDlg(hwnd);
      fRet = TRUE;
      break;

    case WM_COMMAND:
      pScroll = (Scroll*)GetWindowLong(hwnd, DWL_USER);
      HANDLE_WM_COMMAND(hwnd, wParam, lParam, pScroll->Command);
      fRet = TRUE;
      break;

    default:
      fRet = FALSE;
      break;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::InitDlg(HWND) -- private
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

void Scroll::InitDlg
  (
  HWND  hwnd
  )

  {
  char      szBuffer[255];
  int       nIdx;
  long      lGetKey   = AOBJ_GETFIRSTKEY;
  OBJECTID  oiNumber  = NULL;
  BOOL      fSelected = FALSE;
  HWND      hwndList;

  AUtlSetTitleFont(OTYPE_SCROLLBAR, NULL, GetDlgItem(hwnd, ST_OBJECT));
  AUtlCenterDialog(hwnd, 0);
  AObjGetName(ObjectID(), szBuffer, sizeof(szBuffer));
  SetWindowText(hwnd, szBuffer);   // write object name to title bar

  // fill in the edit fields
  wsprintf(szBuffer, "%li", Minimum());
  SetDlgItemText(hwnd, ED_MINIMUM, szBuffer);
  wsprintf(szBuffer, "%li", Maximum());
  SetDlgItemText(hwnd, ED_MAXIMUM, szBuffer);
  wsprintf(szBuffer, "%li", LineScroll());
  SetDlgItemText(hwnd, ED_LINESCROLL, szBuffer);
  wsprintf(szBuffer, "%li", PageScroll());
  SetDlgItemText(hwnd, ED_PAGESCROLL, szBuffer);

  // initialize the check boxes and radio buttons
  CheckDlgButton(hwnd, XB_ENABLED, Enabled());
  CheckDlgButton(hwnd, XB_CANACTIVATE, CanActivate());
  CheckDlgButton(hwnd, RB_HORIZONTAL, Horizontal());
  CheckDlgButton(hwnd, RB_VERTICAL, Vertical());
  CheckDlgButton(hwnd, XB_DEFAULTSIZE, DefaultSize()); 
  if(IsDlgButtonChecked(hwnd, RB_VERTICAL))
    {
    SetDlgItemText(hwnd, ST_MINIMUM, "T&op:");
    SetDlgItemText(hwnd, ST_MAXIMUM, "&Bottom:");
    SetDlgItemText(hwnd, XB_DEFAULTSIZE, "&Default Width");
    }
  else
    {
    SetDlgItemText(hwnd, ST_MINIMUM, "Le&ft:");
    SetDlgItemText(hwnd, ST_MAXIMUM, "&Right:");
    SetDlgItemText(hwnd, XB_DEFAULTSIZE, "&Default Height");
    }

  // initialize the position object list
  hwndList = GetDlgItem(hwnd, CB_POSITIONOBJ);
  nIdx = ComboBox_AddString(hwndList, "(None)");
  ComboBox_SetItemData(hwndList, nIdx, 0L);

  while(NULL != (oiNumber = AObjGetNextObject(OTYPE_NUMBER, &lGetKey)))
    {
    AObjGetName(oiNumber, szBuffer, sizeof(szBuffer));
    nIdx = ComboBox_AddString(hwndList, szBuffer);
    ComboBox_SetItemData(hwndList, nIdx, (LPARAM)oiNumber);
    if(oiNumber == m_oiPosition)
      {
      ComboBox_SetCurSel(hwndList, nIdx);
      fSelected = TRUE;
      }
    }
  if(!fSelected)
    ComboBox_SetCurSel(hwndList, 0L);
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::Command(HWND, int, HWND, UINT) -- private
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

void Scroll::Command
  (
  HWND  hwnd,
  int   idCtl,
  HWND  hwndCtl,
  UINT  uNtfyCode
  )

  {
  I_UNUSED(hwndCtl);
  I_UNUSED(uNtfyCode);

  switch(idCtl)
    {               // buttons on right side of dialog
    case IDOK:
      EndDialog(hwnd, Update(hwnd));
      break;

    case IDCANCEL:
      EndDialog(hwnd, FALSE);
      break;

    case RB_HORIZONTAL:
    case RB_VERTICAL:
      if(IsDlgButtonChecked(hwnd, RB_VERTICAL))
        {
        SetDlgItemText(hwnd, ST_MINIMUM, "T&op:");
        SetDlgItemText(hwnd, ST_MAXIMUM, "&Bottom:");
        SetDlgItemText(hwnd, XB_DEFAULTSIZE, "&Default Width");
        }
      else
        {
        SetDlgItemText(hwnd, ST_MINIMUM, "Le&ft:");
        SetDlgItemText(hwnd, ST_MAXIMUM, "&Right:");
        SetDlgItemText(hwnd, XB_DEFAULTSIZE, "&Default Height");
        }
      break;

    default:
      break;
    }
  }

////////////////////////////////////////////////////////////////////////////
//
//  Method:       Scroll::Update(HWND) -- private
//
//  Description:  
//
////////////////////////////////////////////////////////////////////////////

BOOL Scroll::Update
  (
  HWND  hwnd
  )

  {
  BOOL      fRet;
  char      szBuffer[255];
  int       nIdx;
  OBJECTID  oi;

  // get the values in the edit fields
  GetDlgItemText(hwnd, ED_MINIMUM, szBuffer, sizeof(szBuffer));
  fRet = SetMinimum(strtol(szBuffer, NULL, 0));
  GetDlgItemText(hwnd, ED_MAXIMUM, szBuffer, sizeof(szBuffer));
  fRet = SetMaximum(strtol(szBuffer, NULL, 0))                      || fRet;
  GetDlgItemText(hwnd, ED_LINESCROLL, szBuffer, sizeof(szBuffer));
  fRet = SetLineScroll(strtol(szBuffer, NULL, 0))                   || fRet;
  GetDlgItemText(hwnd, ED_PAGESCROLL, szBuffer, sizeof(szBuffer));
  fRet = SetPageScroll(strtol(szBuffer, NULL, 0))                   || fRet;

  // update the flags from the check boxes and radio buttons
  fRet = SetEnabled(IsDlgButtonChecked(hwnd, XB_ENABLED))          || fRet;
  fRet = SetCanActivate(IsDlgButtonChecked(hwnd, XB_CANACTIVATE))  || fRet;
  fRet = SetVertical(IsDlgButtonChecked(hwnd, RB_VERTICAL))        || fRet;
  fRet = SetDefaultSize(IsDlgButtonChecked(hwnd, XB_DEFAULTSIZE))  || fRet;

  // update the position object
  nIdx = ComboBox_GetCurSel(GetDlgItem(hwnd, CB_POSITIONOBJ));
  oi = (OBJECTID)ComboBox_GetItemData(GetDlgItem(hwnd, CB_POSITIONOBJ), nIdx);
  fRet = SetPositionObject(((long)oi == CB_ERR) ? NULL : oi)        || fRet;

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
