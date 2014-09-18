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
// FILE:          "opendlg.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   OpenDialog class methods.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

////////////////////////////////////////////////////////////////////////////

#include <memory.h>
#include <windows.h>

#include "xp_macro.h"
#include "opendlg.h"
#include "resstrng.h"

////////////////////////////////////////////////////////////////////////////

char* OpenDialog::sm_szFilter   = "Sound Files\0*.wav;*.mid;*.rmi\0Waveform Files\0 *.wav\0Midi Files\0*.mid;*.rmi\0All Files (*.*)\0*.*\0";
char* SaveDialog::sm_szFilter   = "Waveform Files\0*.wav\0";

////////////////////////////////////////////////////////////////////////////

OpenDialog::OpenDialog
  (
  HWND  hwnd
  )

  {
  ResString   rsLoadSound(STR_LOADSOUND);

  m_szFullPath[0] = '\0';
  m_szFilename[0] = '\0';
  lstrcpy(m_szDlgTitle, rsLoadSound.String());

  FILLMEMORY(&m_ofn, 0, sizeof(OPENFILENAME));

  m_ofn.lStructSize     = sizeof(OPENFILENAME);
  m_ofn.hwndOwner       = hwnd;
  m_ofn.lpstrFilter     = sm_szFilter;
  m_ofn.nFilterIndex    = 1;
  m_ofn.lpstrFile       = m_szFullPath;
  m_ofn.nMaxFile        = sizeof(m_szFullPath);
  m_ofn.lpstrTitle      = m_szDlgTitle;
  m_ofn.lpstrFileTitle  = m_szFilename;
  m_ofn.nMaxFileTitle   = sizeof(m_szFilename);
  m_ofn.Flags           = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;
  }

////////////////////////////////////////////////////////////////////////////

BOOL OpenDialog::FileDlg
  (
  const char* p
  )

  {
  if (p != NULL)
    m_ofn.lpstrFilter = p;

  return GetOpenFileName(&m_ofn);
  }

////////////////////////////////////////////////////////////////////////////

void OpenDialog::SetTitle
  (
  const char* szTitle
  )

  {
  lstrcpyn(m_szDlgTitle, szTitle, sizeof(m_szDlgTitle));
  }

////////////////////////////////////////////////////////////////////////////

SaveDialog::SaveDialog
  (
  HWND  hwnd
  )

  {
  ResString   rsLoadSound(STR_LOADSOUND);

  m_szFullPath[0] = '\0';
  m_szFilename[0] = '\0';
  lstrcpy(m_szDlgTitle, rsLoadSound.String());

  FILLMEMORY(&m_ofn, 0, sizeof(OPENFILENAME));

  m_ofn.lStructSize     = sizeof(OPENFILENAME);
  m_ofn.hwndOwner       = hwnd;
  m_ofn.lpstrFilter     = sm_szFilter;
  m_ofn.nFilterIndex    = 1;
  m_ofn.lpstrFile       = m_szFullPath;
  m_ofn.nMaxFile        = sizeof(m_szFullPath);
  m_ofn.lpstrTitle      = m_szDlgTitle;
  m_ofn.lpstrFileTitle  = m_szFilename;
  m_ofn.lpstrDefExt     = "wav";
  m_ofn.nMaxFileTitle   = sizeof(m_szFilename);
  m_ofn.Flags           = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR |
                          OFN_NOREADONLYRETURN | OFN_HIDEREADONLY;
  }

////////////////////////////////////////////////////////////////////////////

BOOL SaveDialog::SaveDlg
  (
  const char* p
  )

  {
  if (p != NULL)
    m_ofn.lpstrFilter = p;

  return GetSaveFileName(&m_ofn);
  }

////////////////////////////////////////////////////////////////////////////

void SaveDialog::SetTitle
  (
  const char* szTitle
  )

  {
  lstrcpyn(m_szDlgTitle, szTitle, sizeof(m_szDlgTitle));
  }

////////////////////////////////////////////////////////////////////////////
