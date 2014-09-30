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
// FILE:          "opendlg.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   OpenDialog class definition.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef OPENDLG_H
#define OPENDLG_H

// Make sure dependencies are included /////////////////////////////////////

#ifndef COMMDLG_H
#include <commdlg.h>
  #ifndef COMMDLG_H
  #define COMMDLG_H
  #endif
#endif

#ifndef DIR_H
#include <dir.h>
  #ifndef DIR_H
  #define DIR_H
  #endif
#endif

#if defined(__FLAT__)
#define MAXFILENAME         MAXFILE
#else
#define MAXFILENAME         (MAXFILE + MAXEXT - 1)
#endif

// OpenDialog class definition /////////////////////////////////////////////

class OpenDialog
  {
  public:
                  OpenDialog(HWND hwnd = NULL);
                 ~OpenDialog()                      {}

    int           FileOffset() const                {return m_ofn.nFileOffset;}
    void          SetFlags(DWORD Flags)             {m_ofn.Flags = Flags;}
    const char*   FullPath() const                  {return m_szFullPath;}
    void          SetInitDir(char* p)               {m_ofn.lpstrInitialDir = p;}
    BOOL          FileDlg(const char* p = NULL);
    void          SetTitle(const char* szTitle);

  private:
    static char*  sm_szFilter;
    char          m_szDlgTitle[80];
    char          m_szFullPath[MAXPATH];
    char          m_szFilename[MAXFILENAME];
    OPENFILENAME  m_ofn;

    // unused methods for protection against incorrect use of this class
                  OpenDialog(const OpenDialog&);
    OpenDialog&   operator=(const OpenDialog&);
  };

// SaveDialog class definition /////////////////////////////////////////////

class SaveDialog
  {
  public:
                  SaveDialog(HWND hwnd = NULL);
                 ~SaveDialog()                      {}

    int           FileOffset() const                {return m_ofn.nFileOffset;}
    void          SetFlags(DWORD Flags)             {m_ofn.Flags = Flags;}
    const char*   FullPath() const                  {return m_szFullPath;}
    void          SetInitDir(char* p)               {m_ofn.lpstrInitialDir = p;}
    BOOL          SaveDlg(const char* p = NULL);
    void          SetTitle(const char* szTitle);

  private:
    static char*  sm_szFilter;
    char          m_szDlgTitle[80];
    char          m_szFullPath[MAXPATH];
    char          m_szFilename[MAXFILENAME];
    OPENFILENAME  m_ofn;

    // unused methods for protection against incorrect use of this class
                  SaveDialog(const SaveDialog&);
    SaveDialog&   operator=(const SaveDialog&);
  };

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef OPENDLG_H
