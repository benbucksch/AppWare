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

#include <string.h>
#include <windows.h>
#include <a_alm.h>
#include <a_almutl.h>

#include "opendlg.h"

////////////////////////////////////////////////////////////////////////////

char* OpenDialog::sm_szFilter   = "Programs\0*.exe;*.pif;*.com;*.bat\0All Files (*.*)\0*.*\0";
char* OpenDialog::sm_szDlgTitle = "Browse";

//////////////////////////////////////////////////

static
UINT		ALMCALLBACK OpenFileHook(
	HWND		hDlg,
	UINT		nMsg,
	WPARAM	wParam,
	LPARAM	lParam
	)
	{
	UINT		nRes;

	switch (nMsg)
		{
		case WM_INITDIALOG:
			AUtlCenterDialog(hDlg, 0);
			break;

		default:
			nRes = 0;
			break;
		}

	return nRes;
	}

////////////////////////////////////////////////////////////////////////////

OpenDialog::OpenDialog
  (
  HWND  hwnd,
	BOOL	fCenter
  )

  {
  m_szFullPath[0] = '\0';
  m_szFilename[0] = '\0';

  FillMemory(&m_ofn, 0, sizeof(OPENFILENAME));

  m_ofn.lStructSize     = sizeof(OPENFILENAME);
  m_ofn.hwndOwner       = hwnd;
  m_ofn.lpstrFilter     = sm_szFilter;
  m_ofn.nFilterIndex    = 1;
  m_ofn.lpstrFile       = m_szFullPath;
  m_ofn.nMaxFile        = sizeof(m_szFullPath);
  m_ofn.lpstrTitle      = sm_szDlgTitle;
  m_ofn.lpstrFileTitle  = m_szFilename;
  m_ofn.nMaxFileTitle   = sizeof(m_szFilename);
  m_ofn.Flags           = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST;

	if (fCenter)
		{
		m_ofn.Flags |= OFN_ENABLEHOOK;
		m_ofn.lpfnHook = OpenFileHook;
		}
  }

////////////////////////////////////////////////////////////////////////////

BOOL  OpenDialog::FileDlg
  (
  const char* p
  )

  {
  if (p != NULL)
    m_ofn.lpstrFilter = p;

  return GetOpenFileName(&m_ofn);
  }

////////////////////////////////////////////////////////////////////////////
