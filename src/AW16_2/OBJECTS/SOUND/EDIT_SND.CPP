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
// FILE:          "edit_snd.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>

#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>

#include "xp_macro.h"
#include "edit_snd.h"
#include "recsound.h"
#include "soundres.h"
#include "resstrng.h"
#include "opendlg.h"

// global variables ////////////////////////////////////////////////////////

extern HINSTANCE  g_hinstDLL;
static UINT       s_nWaveInDev   = waveInGetNumDevs();
static UINT       s_nWaveOutDev  = waveOutGetNumDevs();

////////////////////////////////////////////////////////////////////////////

EditSound::EditSound
  (
  const Sound*  pSound
  ) : Sound(*pSound),
      m_fChanged(FALSE),
      m_hwnd(NULL)

  {
  }

////////////////////////////////////////////////////////////////////////////

EditSound::~EditSound
  (
  )

  {
  if(m_fOpen)
    Close(TRUE);
  }

////////////////////////////////////////////////////////////////////////////

BOOL ALMCALLBACK EditSound::DlgProc
  (
  HWND    hwnd,
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  BOOL        fRet;
  EditSound*  pEdit;

  switch (uMsg)
    {
    case WM_INITDIALOG:
      SetWindowLong(hwnd, DWL_USER, lParam);
      pEdit = (EditSound*)lParam;
      fRet = pEdit->InitDialog(hwnd);
      break;

    case WM_COMMAND:
      pEdit = (EditSound*)GetWindowLong(hwnd, DWL_USER);
      I_ASSERT(pEdit);
      HANDLE_WM_COMMAND(hwnd, wParam, lParam, pEdit->Command);
      fRet = TRUE;
      break;

    default:
      fRet = FALSE;
      break;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////

BOOL EditSound::InitDialog
  (
  HWND  hwnd
  )

  {
  char        szBuffer[255];
  BOOL        fRet;

  m_hwnd = hwnd;

  AUtlCenterDialog(m_hwnd, 0);
  AUtlSetTitleFont(OTYPE_SOUND, NULL, GetDlgItem(m_hwnd, ST_TYPENAME));
  AObjGetName(ObjectID(), szBuffer, sizeof(szBuffer));
  SetWindowText(m_hwnd, szBuffer);

  CheckDlgButton(m_hwnd, RB_EXTERNAL, External());
  CheckDlgButton(m_hwnd, RB_INTERNAL, Internal());

  EnableWindow(GetDlgItem(m_hwnd, ST_SOUNDFILE), External());
  EnableWindow(GetDlgItem(m_hwnd, ED_SOUNDFILE), External());
  Edit_LimitText(GetDlgItem(m_hwnd, ED_SOUNDFILE), sizeof(szBuffer)-1);
  if(External())
    {
    SetDlgItemText(m_hwnd, ED_SOUNDFILE, GetFilename());
    SetFocus(GetDlgItem(m_hwnd, ED_SOUNDFILE));
    Edit_SetSel(GetDlgItem(m_hwnd, ED_SOUNDFILE), 0, -1);
    fRet = FALSE;
    }
  else
    fRet = TRUE;

  // only allow the record dialog to display if the driver/hardware exists
  EnableWindow(GetDlgItem(m_hwnd, PB_RECORD), s_nWaveInDev!=0);

  // only allow the playback to display if the driver/hardware exists
  EnableWindow(GetDlgItem(m_hwnd, BB_FF),     FALSE);
  EnableWindow(GetDlgItem(m_hwnd, BB_REWIND), FALSE);

  // set up the playback buttons
  DlgStateChanged();

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////

void EditSound::Command
  (
  HWND  hwnd,
  int   idCtl,
  HWND  hwndCtl,
  UINT  uNtfyCode
  )

  {
  I_UNUSED(hwndCtl);

  switch (idCtl)
    {
    case IDOK:
      EndDialog(hwnd, Update());
      break;

    case IDCANCEL:
      EndDialog(hwnd, FALSE);
      break;

    case IDHELP:
      break;

    case ED_SOUNDFILE:
      if(uNtfyCode == EN_KILLFOCUS)
        DlgStateChanged();
      break;

    case PB_RECORD:
      RecordDlg();
      break;

    case PB_LOAD:
      LoadDlg();
      break;

    case PB_CLEARSOUND:
      if(m_fOpen)
        Close(TRUE);
      Clear();
      if(External())
        SetDlgItemText(hwnd, ED_SOUNDFILE, "");
      m_fChanged = TRUE;
      DlgStateChanged();
      break;

    case RB_EXTERNAL:
    case RB_INTERNAL:
      DlgStateChanged();
      break;

    case BB_PLAY:
      BBPlay();
      break;

    case BB_PAUSE:
      BBPause();
      break;

    case BB_STOP:
      BBStop();
      break;

    case BB_REWIND:
      break;

    case BB_FF:
      break;

    default:
      break;
    }
  }

////////////////////////////////////////////////////////////////////////////

void EditSound::DlgStateChanged
  (
  )

  {
  Update();
  EnableWindow(GetDlgItem(m_hwnd, ST_SOUNDFILE), External());
  EnableWindow(GetDlgItem(m_hwnd, ED_SOUNDFILE), External());
  SetDlgItemText(m_hwnd, PB_RECORD, External() ? "&Record To..." : "&Record...");

  if(m_fOpen)
    Close(TRUE);

  if(s_nWaveOutDev && m_szFile[0])
    {
    OpenMCI open;
    open.lpstrElementName = m_szFile;

    if(0 == Open(MCI_WAIT | MCI_OPEN_ELEMENT, &open))
      EnableWindow(GetDlgItem(m_hwnd, BB_PLAY),  TRUE);
    else
      EnableWindow(GetDlgItem(m_hwnd, BB_PLAY),  FALSE);
    }
  else
    EnableWindow(GetDlgItem(m_hwnd, BB_PLAY),   FALSE);

  EnableWindow(GetDlgItem(m_hwnd, BB_PAUSE),  FALSE);
  EnableWindow(GetDlgItem(m_hwnd, BB_STOP),   FALSE);    
  }

////////////////////////////////////////////////////////////////////////////

BOOL EditSound::Update
  (
  )

  {
  m_fChanged = SetInternal(IsDlgButtonChecked(m_hwnd, RB_INTERNAL)) || m_fChanged;

  if(External())
    {
    char  szBuffer[MAXPATH];
    GetDlgItemText(m_hwnd, ED_SOUNDFILE, szBuffer, sizeof(szBuffer));
    m_fChanged = SetFilename(szBuffer) || m_fChanged;
    }

  return m_fChanged;
  }

////////////////////////////////////////////////////////////////////////////

BOOL EditSound::RecordDlg
  (
  )

  {
  RecordSound rs(*this);
  BOOL        fLaunchDlg  = TRUE;

  m_fChanged = SetInternal(IsDlgButtonChecked(m_hwnd, RB_INTERNAL)) || m_fChanged;

  if(External())
    {
    // get a filename
    SaveDialog  sd(GetActiveWindow());
    ResString   rsLoadSound(STR_RECORDSOUND);

    sd.SetTitle(rsLoadSound.String());
    sd.SetInitDir(GetInitDir());
    fLaunchDlg = sd.SaveDlg();

    if(fLaunchDlg)
      rs.SetFilename(sd.FullPath());
    }

  if(fLaunchDlg && m_fOpen)
    Close(TRUE);

  if(fLaunchDlg && DialogBoxParam(g_hinstDLL, ID(DLG_RECORDSOUND), m_hwnd, RecordSound::DlgProc, (LPARAM)&rs))
    {
    m_fChanged = TRUE;
    CopyObject(rs, TRUE);

    if(External())
      SetDlgItemText(m_hwnd, ED_SOUNDFILE, m_szFile);
    }

  if(fLaunchDlg)
    DlgStateChanged();

  return TRUE;
  }

////////////////////////////////////////////////////////////////////////////

BOOL EditSound::LoadDlg
  (
  )

  {
  OpenDialog  od(GetActiveWindow());
  ResString   rsLoadSound(STR_LOADSOUND);

  m_fChanged = SetInternal(IsDlgButtonChecked(m_hwnd, RB_INTERNAL)) || m_fChanged;

  od.SetTitle(rsLoadSound.String());
  od.SetInitDir(GetInitDir());
  if(od.FileDlg())
    {
    if(Internal())
      {
      SetSoundFromFile(od.FullPath());
      m_fChanged = TRUE;
      }
    else
      m_fChanged = SetFilename(od.FullPath()) || m_fChanged;

    if(External())
      SetDlgItemText(m_hwnd, ED_SOUNDFILE, m_szFile);

    DlgStateChanged();
    }

  return FALSE;
  }

////////////////////////////////////////////////////////////////////////////

BOOL EditSound::BBPlay
  (
  )

  {
  if(m_fPaused)
    BBPause();
  else
    {
    PlayMCI play;

    EnableWindow(GetDlgItem(m_hwnd, BB_PLAY),  FALSE);
    EnableWindow(GetDlgItem(m_hwnd, BB_PAUSE), TRUE);
    EnableWindow(GetDlgItem(m_hwnd, BB_STOP),  TRUE);
    SetFocus(GetDlgItem(m_hwnd, BB_STOP));

    Seek(MCI_WAIT | MCI_SEEK_TO_START, 0);
    Play(MCI_NOTIFY, &play);
    }

  return TRUE;
  }

////////////////////////////////////////////////////////////////////////////

BOOL EditSound::BBPause
  (
  )

  {
  m_fPaused ? Resume(TRUE) : Pause(TRUE);
  m_fPaused = !m_fPaused;

  EnableWindow(GetDlgItem(m_hwnd, BB_PLAY),  m_fPaused);
  EnableWindow(GetDlgItem(m_hwnd, BB_PAUSE), TRUE);
  EnableWindow(GetDlgItem(m_hwnd, BB_STOP),  TRUE);
  SetFocus(GetDlgItem(m_hwnd, BB_PAUSE));

  return TRUE;
  }

////////////////////////////////////////////////////////////////////////////

BOOL EditSound::BBStop
  (
  )

  {
  Stop(TRUE);

  EnableWindow(GetDlgItem(m_hwnd, BB_PLAY),  TRUE);
  EnableWindow(GetDlgItem(m_hwnd, BB_PAUSE), FALSE);
  EnableWindow(GetDlgItem(m_hwnd, BB_STOP),  FALSE);

  SetFocus(GetDlgItem(m_hwnd, BB_PLAY));

  return TRUE;
  }

////////////////////////////////////////////////////////////////////////////

void EditSound::Notify
  (
  UINT  uNotifyCode
  )

  {
  if((uNotifyCode == MCI_NOTIFY_FAILURE) && m_fOpen)
    Close(TRUE);
  else if(uNotifyCode != MCI_NOTIFY_FAILURE)
    {
    EnableWindow(GetDlgItem(m_hwnd, BB_PLAY),  m_fOpen);
    EnableWindow(GetDlgItem(m_hwnd, BB_PAUSE), FALSE);
    EnableWindow(GetDlgItem(m_hwnd, BB_STOP),  FALSE);

    if(m_fOpen)
      SetFocus(GetDlgItem(m_hwnd, BB_PLAY));
    }
  }

////////////////////////////////////////////////////////////////////////////
