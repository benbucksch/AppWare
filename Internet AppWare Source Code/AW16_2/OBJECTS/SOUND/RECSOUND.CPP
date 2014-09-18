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
// FILE:          "recsound.cpp"
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

// include files ///////////////////////////////////////////////////////////

//#include <string.h>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>

#include <a_alm.h>
#include <a_almutl.h>
#include <intdebug.h>

#include "soundres.h"
#include "sound.h"
#include "recsound.h"

// class variables /////////////////////////////////////////////////////////

WAVEINCAPS  RecordSound::sm_wic             = {0, 0, 0, {0}, 0, 0};
BOOL        RecordSound::sm_fWICInitialized = FALSE;

////////////////////////////////////////////////////////////////////////////

RecordSound::RecordSound
  (
  const Sound&  srcSound
  ) : Sound(srcSound, FALSE),
      m_fPaused(FALSE),
      m_hwnd(NULL)

  {
  m_szFile[0] = '\0';

  if(!sm_fWICInitialized)
    {
    sm_fWICInitialized = TRUE;

    // get the recording capabilities
    waveInGetDevCaps(WAVE_MAPPER, &sm_wic, sizeof(sm_wic));
    }
  }

////////////////////////////////////////////////////////////////////////////

RecordSound::~RecordSound
  (
  )

  {
  if(m_fOpen)
    Close(TRUE);
  }

////////////////////////////////////////////////////////////////////////////

BOOL RecordSound::SetFilename
  (
  LPCSTR  szFile
  )

  {
  BOOL  fRet = (0 != lstrcmpi(szFile, m_szFile));

  if(fRet)
    lstrcpyn(m_szFile, szFile, sizeof(m_szFile));

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////

BOOL ALMCALLBACK RecordSound::DlgProc
  (
  HWND    hwnd, 
  UINT    uMsg,
  WPARAM  wParam,
  LPARAM  lParam
  )

  {
  BOOL          fRet;
  RecordSound*  pRS;

  switch (uMsg)
    {
    case WM_INITDIALOG:
      SetWindowLong(hwnd, DWL_USER, lParam);
      pRS = (RecordSound*)lParam;
      I_ASSERT(pRS);
      fRet = pRS->InitDialog(hwnd);
      break;

    case WM_COMMAND:
      pRS = (RecordSound*)GetWindowLong(hwnd, DWL_USER);
      I_ASSERT(pRS);
      HANDLE_WM_COMMAND(hwnd, wParam, lParam, pRS->Command);
      break;

    default:
      fRet = FALSE;
      break;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////

void RecordSound::Command
  (
  HWND  hwnd,
  int   idCtl,
  HWND  hwndCtl,
  UINT  uNtfyCode
  )

  {
  I_UNUSED(hwndCtl);
  I_UNUSED(uNtfyCode);

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

    case BB_RECORD:
      BBRecord();
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

    default:
      break;
    }
  }

////////////////////////////////////////////////////////////////////////////

void RecordSound::GetRecordInfo
  (
  long&   lSR,
  short&  sBPS,
  BOOL&   fStereo
  )

  {
  // get the recording quality
  if(IsDlgButtonChecked(m_hwnd, RB_QUALITY_RADIO))
    lSR = k_lSR_11025_Hz;
  else if(IsDlgButtonChecked(m_hwnd, RB_QUALITY_TAPE))
    lSR = k_lSR_22050_Hz;
  else
    lSR = k_lSR_44100_Hz;

  // get the stereo flag
  fStereo = IsDlgButtonChecked(m_hwnd, XB_STEREO);

  // get the bits per sample
  if(IsDlgButtonChecked(m_hwnd, RB_BPS_4))
    sBPS = k_sBPS_4;
  else if(IsDlgButtonChecked(m_hwnd, RB_BPS_8))
    sBPS = k_sBPS_8;
  else
    sBPS = k_sBPS_16;
  }

////////////////////////////////////////////////////////////////////////////

BOOL RecordSound::Check11K
  (
  long&   lSR,
  short&  sBPS,
  BOOL&   fStereo
  )

  {
  BOOL  fRet = FALSE;

  if(sm_wic.dwFormats & WAVE_FORMAT_11K)
    {
    if(sBPS == k_sBPS_8)
      {
      if(sm_wic.dwFormats & WAVE_FORMAT_11K_08)
        {
        if(fStereo && !(sm_wic.dwFormats & WAVE_FORMAT_1S08))
          fStereo = FALSE;
        else if(!fStereo && !(sm_wic.dwFormats & WAVE_FORMAT_1M08))
          fStereo = TRUE;
        }
      else
        sBPS = k_sBPS_16;
      }
    if(sBPS == k_sBPS_16)
      {
      if(sm_wic.dwFormats & WAVE_FORMAT_11K_16)
        {
        if(fStereo && !(sm_wic.dwFormats & WAVE_FORMAT_1S16))
          fStereo = FALSE;
        else if(!fStereo && !(sm_wic.dwFormats & WAVE_FORMAT_1M16))
          fStereo = TRUE;
        }
      else
        {
        sBPS = k_sBPS_8;
        fRet = TRUE;
        }
      }
    }
  else
    {
    if(sm_wic.dwFormats & WAVE_FORMAT_22K)
      {
      lSR = k_lSR_22050_Hz;
      fRet = TRUE;
      }
    else if(sm_wic.dwFormats & WAVE_FORMAT_44K)
      {
      lSR = k_lSR_44100_Hz;
      fRet = TRUE;
      }
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////

BOOL RecordSound::Check22K
  (
  long&   lSR,
  short&  sBPS,
  BOOL&   fStereo
  )

  {
  BOOL  fRet = FALSE;

  if(sm_wic.dwFormats & WAVE_FORMAT_22K)
    {
    if(sBPS == k_sBPS_8)
      {
      if(sm_wic.dwFormats & WAVE_FORMAT_22K_08)
        {
        if(fStereo && !(sm_wic.dwFormats & WAVE_FORMAT_2S08))
          fStereo = FALSE;
        else if(!fStereo && !(sm_wic.dwFormats & WAVE_FORMAT_2M08))
          fStereo = TRUE;
        }
      else
        sBPS = k_sBPS_16;
      }
    if(sBPS == k_sBPS_16)
      {
      if(sm_wic.dwFormats & WAVE_FORMAT_22K_16)
        {
        if(fStereo && !(sm_wic.dwFormats & WAVE_FORMAT_2S16))
          fStereo = FALSE;
        else if(!fStereo && !(sm_wic.dwFormats & WAVE_FORMAT_2M16))
          fStereo = TRUE;
        }
      else
        {
        sBPS = k_sBPS_8;
        fRet = TRUE;
        }
      }
    }
  else
    {
    if(sm_wic.dwFormats & WAVE_FORMAT_44K)
      {
      lSR = k_lSR_44100_Hz;
      fRet = TRUE;
      }
    else if(sm_wic.dwFormats & WAVE_FORMAT_11K)
      {
      lSR = k_lSR_11025_Hz;
      fRet = TRUE;
      }
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////

BOOL RecordSound::Check44K
  (
  long&   lSR,
  short&  sBPS,
  BOOL&   fStereo
  )

  {
  BOOL  fRet = FALSE;

  if(sm_wic.dwFormats & WAVE_FORMAT_44K)
    {
    if(sBPS == k_sBPS_8)
      {
      if(sm_wic.dwFormats & WAVE_FORMAT_44K_08)
        {
        if(fStereo && !(sm_wic.dwFormats & WAVE_FORMAT_4S08))
          fStereo = FALSE;
        else if(!fStereo && !(sm_wic.dwFormats & WAVE_FORMAT_4M08))
          fStereo = TRUE;
        }
      else
        sBPS = k_sBPS_16;
      }
    if(sBPS == k_sBPS_16)
      {
      if(sm_wic.dwFormats & WAVE_FORMAT_44K_16)
        {
        if(fStereo && !(sm_wic.dwFormats & WAVE_FORMAT_4S16))
          fStereo = FALSE;
        else if(!fStereo && !(sm_wic.dwFormats & WAVE_FORMAT_4M16))
          fStereo = TRUE;
        }
      else
        {
        sBPS = k_sBPS_8;
        fRet = TRUE;
        }
      }
    }
  else
    {
    if(sm_wic.dwFormats & WAVE_FORMAT_22K)
      {
      lSR = k_lSR_22050_Hz;
      fRet = TRUE;
      }
    else if(sm_wic.dwFormats & WAVE_FORMAT_11K)
      {
      lSR = k_lSR_11025_Hz;
      fRet = TRUE;
      }
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////

void RecordSound::ValidateRecordState
  (
  long&   lSR,
  short&  sBPS,
  BOOL&   fStereo
  )

  {
  BOOL  fRecurse = FALSE;

  if(lSR == k_lSR_11025_Hz)
    fRecurse = Check11K(lSR, sBPS, fStereo);

  if(!fRecurse && (lSR == k_lSR_22050_Hz))
    fRecurse = Check22K(lSR, sBPS, fStereo);

  if(!fRecurse && (lSR == k_lSR_44100_Hz))
    fRecurse = Check44K(lSR, sBPS, fStereo);

  if(fRecurse)
    ValidateRecordState(lSR, sBPS, fStereo);
  }

////////////////////////////////////////////////////////////////////////////

BOOL RecordSound::InitDialog
  (
  HWND  hwnd
  )

  {
  long  lSR     = SamplingRate();
  BOOL  fStereo = RecordInStereo();
  short sBPS    = BitsPerSample();

  ValidateRecordState(lSR, sBPS, fStereo);

  // save the window handle
  m_hwnd = hwnd;

  // center the dialog
  AUtlCenterDialog(m_hwnd, 0);

  // set the recording quality
  CheckDlgButton(m_hwnd, RB_QUALITY_RADIO, lSR==k_lSR_11025_Hz);
  CheckDlgButton(m_hwnd, RB_QUALITY_TAPE,  lSR==k_lSR_22050_Hz);
  CheckDlgButton(m_hwnd, RB_QUALITY_CD,    lSR==k_lSR_44100_Hz);

  // set the stereo flag
  CheckDlgButton(m_hwnd, XB_STEREO, fStereo);

  // set the bits per sample
  CheckDlgButton(m_hwnd, RB_BPS_4,  sBPS==k_sBPS_4);
  CheckDlgButton(m_hwnd, RB_BPS_8,  sBPS==k_sBPS_8);
  CheckDlgButton(m_hwnd, RB_BPS_16, sBPS==k_sBPS_16);

  // set the enable status for the bitmap buttons
  EnableWindow(GetDlgItem(m_hwnd, BB_RECORD), TRUE);
  EnableWindow(GetDlgItem(m_hwnd, BB_PLAY),   FALSE);
  EnableWindow(GetDlgItem(m_hwnd, BB_PAUSE),  FALSE);
  EnableWindow(GetDlgItem(m_hwnd, BB_STOP),   FALSE);

  // set the focus to the first item
  return TRUE;
  }

////////////////////////////////////////////////////////////////////////////

BOOL RecordSound::Update
  (
  )

  {
  BOOL  fRet;
  long  lSR;
  short sBPS;
  BOOL  fStereo;

  // get the information from the dialog
  GetRecordInfo(lSR, sBPS, fStereo);
  ValidateRecordState(lSR, sBPS, fStereo);

  // set the object data
  fRet = SetRecordInStereo(fStereo);
  fRet = SetSamplingRate(lSR)       || fRet;
  fRet = SetBitsPerSample(sBPS)     || fRet;

  if(m_fOpen)
    {
    // save the recorded sound -- if the sound was somehow modified
    SaveMCI sm;

    if(!m_szFile[0])
      GetTmpSoundFileName(SoundResource::waveform, m_szFile);

    sm.lpfilename = m_szFile;
    if(0 == Save(MCI_WAIT | MCI_SAVE_FILE, &sm))
      fRet = TRUE;
    }

  return fRet;
  }

////////////////////////////////////////////////////////////////////////////

BOOL RecordSound::BBRecord
  (
  )

  {
  long  lSR;
  short sBPS;
  BOOL  fStereo;

  // get the information from the dialog
  GetRecordInfo(lSR, sBPS, fStereo);
  ValidateRecordState(lSR, sBPS, fStereo);

  // close the open device
  if(m_fOpen)
    {
    Close(TRUE);
    m_fPaused = FALSE;
    }

  // open the device for recording
  WaveOpenMCI wom;
  wom.lpstrDeviceType   = "WaveAudio";
  wom.lpstrElementName  = "";
  if(0 == Open(MCI_WAIT | MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, &wom))
    {
    // set the recording state
    WaveSetMCI  wsm;
    wsm.nSamplesPerSec  = lSR;
    wsm.wBitsPerSample  = sBPS;
    wsm.nChannels       = fStereo ? 2 : 1;
    Set(MCI_WAIT | MCI_WAVE_SET_CHANNELS | MCI_WAVE_SET_SAMPLESPERSEC
        | MCI_WAVE_SET_BITSPERSAMPLE, &wsm);

    // set the enable state for the other buttons
    EnableWindow(GetDlgItem(m_hwnd, BB_RECORD), FALSE);
    EnableWindow(GetDlgItem(m_hwnd, BB_PLAY),   FALSE);
    EnableWindow(GetDlgItem(m_hwnd, BB_PAUSE),  FALSE);
    EnableWindow(GetDlgItem(m_hwnd, BB_STOP),   TRUE);
    SetFocus(GetDlgItem(m_hwnd, BB_STOP));

    // record
    Record(MCI_NOTIFY, 0, 0);
    }

  return TRUE;
  }

////////////////////////////////////////////////////////////////////////////

BOOL RecordSound::BBPlay
  (
  )

  {
  if(m_fPaused)
    BBPause();
  else
    {
    I_ASSERT(m_fOpen);
    PlayMCI pm;

    // set the other buttons' enable state
    EnableWindow(GetDlgItem(m_hwnd, BB_RECORD), FALSE);
    EnableWindow(GetDlgItem(m_hwnd, BB_PLAY),   FALSE);
    EnableWindow(GetDlgItem(m_hwnd, BB_PAUSE),  TRUE);
    EnableWindow(GetDlgItem(m_hwnd, BB_STOP),   TRUE);
    SetFocus(GetDlgItem(m_hwnd, BB_STOP));

    // play the sound from the beginning
    Seek(MCI_WAIT | MCI_SEEK_TO_START, 0);
    Play(MCI_NOTIFY, &pm);
    }

  return TRUE;
  }

////////////////////////////////////////////////////////////////////////////

BOOL RecordSound::BBPause
  (
  )

  {
  m_fPaused ? Resume(TRUE) : Pause(TRUE);
  m_fPaused = !m_fPaused;

  EnableWindow(GetDlgItem(m_hwnd, BB_RECORD), m_fPaused);
  EnableWindow(GetDlgItem(m_hwnd, BB_PLAY),   m_fPaused);
  EnableWindow(GetDlgItem(m_hwnd, BB_PAUSE),  TRUE);
  EnableWindow(GetDlgItem(m_hwnd, BB_STOP),   TRUE);
  SetFocus(GetDlgItem(m_hwnd, BB_PAUSE));

  return TRUE;
  }

////////////////////////////////////////////////////////////////////////////

BOOL RecordSound::BBStop
  (
  )

  {
  if(m_fOpen)
    Stop(TRUE);
  m_fPaused = FALSE;

  // set the enable state for the buttons
  EnableWindow(GetDlgItem(m_hwnd, BB_RECORD), TRUE);
  SetFocus(GetDlgItem(m_hwnd, BB_RECORD));
  EnableWindow(GetDlgItem(m_hwnd, BB_PLAY),   m_fOpen);
  EnableWindow(GetDlgItem(m_hwnd, BB_STOP),   FALSE);
  EnableWindow(GetDlgItem(m_hwnd, BB_PAUSE),  FALSE);

  return TRUE;
  }

////////////////////////////////////////////////////////////////////////////

void RecordSound::Notify
  (
  UINT  uNotifyCode
  )

  {
  if((MCI_NOTIFY_FAILURE == uNotifyCode) && m_fOpen)
    Close(TRUE);
  else if((MCI_NOTIFY_FAILURE != uNotifyCode)
                  &&
          ((m_dwLastNtfyCmd == MCI_RECORD) || (m_dwLastNtfyCmd == MCI_PLAY)))
    {
    EnableWindow(GetDlgItem(m_hwnd, BB_RECORD), TRUE);
    EnableWindow(GetDlgItem(m_hwnd, BB_PLAY),   m_fOpen);
    SetFocus(GetDlgItem(m_hwnd, (m_dwLastNtfyCmd == MCI_RECORD) ? BB_RECORD : BB_PLAY));
    EnableWindow(GetDlgItem(m_hwnd, BB_STOP),   FALSE);
    EnableWindow(GetDlgItem(m_hwnd, BB_PAUSE),  FALSE);
    }
  }

////////////////////////////////////////////////////////////////////////////
