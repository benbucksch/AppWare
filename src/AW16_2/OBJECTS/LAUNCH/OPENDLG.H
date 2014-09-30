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

#include <commdlg.h>

// OpenDialog class definition /////////////////////////////////////////////

class OpenDialog
  {
  public:
                  OpenDialog(HWND hwnd = NULL, BOOL = FALSE);
                 ~OpenDialog()          {}
                 
    int           FileOffset() const    {return m_ofn.nFileOffset;}
    void          SetFlags(DWORD Flags) {m_ofn.Flags = Flags;}
    const char*   FullPath() const      {return m_szFullPath;}
    void          SetInitDir(char* p)   {m_ofn.lpstrInitialDir = p;}
    BOOL          FileDlg(const char* p = NULL);
  
  private:
    static char*  sm_szFilter;
    static char*  sm_szDlgTitle;    
    char          m_szFullPath[256];
    char          m_szFilename[13];
    OPENFILENAME  m_ofn;
    
    // unused methods for protection against incorrect use of this class
                  OpenDialog(const OpenDialog&);
    OpenDialog&   operator=(const OpenDialog&);
  };

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef OPENDLG_H

