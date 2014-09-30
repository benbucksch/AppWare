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
// FILE:          "launcher.h"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   Launcher class and object definition.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#ifndef LAUNCHER_H
#define LAUNCHER_H

// Make sure dependencies are included /////////////////////////////////////

#ifndef A_ALM_H
#include <a_alm.h>
#endif // #ifndef A_ALM_H

// defines /////////////////////////////////////////////////////////////////

// type id
#define OTYPE_LAUNCHER          ATYPE_MAKEID('A','p','p','l')

// data
#define DATA_COMMANDLINE        0L
#define DATA_STARTDIRECTORY     1L
#define DATA_RUNMINIMIZED       2L
#define DATA_VERSION            3L
#define DATA_NUMDATABLOCKS      4L

// version
#define VERSION_LAUNCHOBJECT    0x0100

// Launcher class definition ///////////////////////////////////////////////

class Launcher
  {
  public:
                Launcher(AOBJECTID oiLaunch);
               ~Launcher();

    // check to see if the object constructed correctly
    BOOL        Valid() const                     {return m_szStartDir && m_szCmdLine && m_pfRunMin;}

    // accessor functions         
    AOBJECTID    ObjectID() const                  {return m_oiLauncher;}
    const char* CommandLine() const               {return m_szCmdLine;}
    const char* StartDirectory() const            {return m_szStartDir;}
    BOOL        RunMinimized() const              {return *m_pfRunMin;}

    // functions to set data
    BOOL        SetCommandLine(const char* szCmdLine);
    BOOL        SetStartDirectory(const char* szStartDir);
    BOOL        SetRunMinimized(BOOL fMin);

    // administration function messages             
    long        Assign(const Launcher* pSource);
    long        Edit(BOOL fRuntime = FALSE);
    long        Export() const;
    long        Import();

    // Launcher ALM functions
    long        LaunchApplication(char* szError, int nErrorBufLen);
    void        SetApplication(AOBJECTID oiText);

  private:
    AOBJECTID    m_oiLauncher;

    const char* m_szCmdLine;
    const char* m_szStartDir;
    BOOL*       m_pfRunMin;

    UINT        CommandShow() {return ((RunMinimized()) ? SW_MINIMIZE : SW_SHOW);}

    // unused methods for protection against incorrect use of this class
                Launcher(const Launcher&);
    Launcher&   operator=(const Launcher&);
  };

////////////////////////////////////////////////////////////////////////////

#endif  // #ifndef LAUNCHER_H
