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
// FILE:          "launcher.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   Launcher class methods.
//
// CHANGES:
//
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT

////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <dir.h>
#include <ctype.h>
#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>
#include <o_upsf.h>

#include "launcher.h"
#include "launc_rc.h"

// Global Variables ////////////////////////////////////////////////////////

extern HINSTANCE  g_hinstDLL;

// constants ///////////////////////////////////////////////////////////////

static const char* const k_szBlank        = "";
static const char* const k_szVersion      = "VERSION";
static const char* const k_szMinimized    = "MINIMIZD";
static const char* const k_szCommandLine  = "CMDLINE";
static const char* const k_szStartupDir   = "STRTUPDR";

////////////////////////////////////////////////////////////////////////////

Launcher::Launcher
  (
  OBJECTID  oiLaunch
  ) : m_oiLauncher(oiLaunch)
  
  {
  short*  pVer;
  
  if(AObjGetDataCount(m_oiLauncher) < DATA_NUMDATABLOCKS)
    AObjSetDataCount(m_oiLauncher, DATA_NUMDATABLOCKS);
    
  m_szCmdLine  = (const char*)AObjLockData(m_oiLauncher, DATA_COMMANDLINE);
  m_szStartDir = (const char*)AObjLockData(m_oiLauncher, DATA_STARTDIRECTORY);
  m_pfRunMin    = (BOOL*)AObjLockData(m_oiLauncher, DATA_RUNMINIMIZED);
  pVer         = (short*)AObjLockData(m_oiLauncher, DATA_VERSION);
  
  if(!m_szCmdLine)
    {
    AObjSetData(m_oiLauncher, DATA_COMMANDLINE, (void*)k_szBlank, 1);
    m_szCmdLine = (const char*)AObjLockData(m_oiLauncher, DATA_COMMANDLINE);
    }
  
  if(!m_szStartDir)
    {
    AObjSetData(m_oiLauncher, DATA_STARTDIRECTORY, (void*)k_szBlank, 1);
    m_szStartDir = (const char*)AObjLockData(m_oiLauncher, DATA_STARTDIRECTORY);
    }
  
  if(!m_pfRunMin)
    {
    AObjResizeData(m_oiLauncher, DATA_RUNMINIMIZED, sizeof(BOOL));
    m_pfRunMin = (BOOL*)AObjLockData(m_oiLauncher, DATA_RUNMINIMIZED);
    if(m_pfRunMin)
      *m_pfRunMin = FALSE;
    }
    
  if(!pVer)
    {
    AObjResizeData(m_oiLauncher, DATA_VERSION, sizeof(short));
    pVer = (short*)AObjLockData(m_oiLauncher, DATA_VERSION);
    if(pVer)
      *pVer = VERSION_LAUNCHOBJECT;
    }

  I_ASSERT(pVer && (*pVer == VERSION_LAUNCHOBJECT));

  if(pVer)
    AObjUnlockData(m_oiLauncher, DATA_VERSION);
  }

////////////////////////////////////////////////////////////////////////////

Launcher::~Launcher
  (
  )
  
  {
  AObjUnlockData(m_oiLauncher, DATA_COMMANDLINE);
  AObjUnlockData(m_oiLauncher, DATA_STARTDIRECTORY);
  AObjUnlockData(m_oiLauncher, DATA_RUNMINIMIZED);
  }

////////////////////////////////////////////////////////////////////////////

BOOL Launcher::SetCommandLine
  (
  const char* szCmdLine
  )
  
  {
  BOOL  fRet = (strcmp(szCmdLine, m_szCmdLine) != 0); 
  
  if(fRet)
    {
    AObjUnlockData(m_oiLauncher, DATA_COMMANDLINE);
    AObjSetData(m_oiLauncher, DATA_COMMANDLINE, (void*)szCmdLine, strlen(szCmdLine)+1);
    m_szCmdLine = (const char*)AObjLockData(m_oiLauncher, DATA_COMMANDLINE);
    }
    
  return fRet;
  }
  
////////////////////////////////////////////////////////////////////////////

BOOL Launcher::SetStartDirectory
  (
  const char* szStartDir
  )
  
  {
  BOOL  fRet = (strcmp(szStartDir, m_szStartDir) != 0); 
  
  if(fRet)
    {
    AObjUnlockData(m_oiLauncher, DATA_STARTDIRECTORY);
    AObjSetData(m_oiLauncher, DATA_STARTDIRECTORY, (void*)szStartDir, strlen(szStartDir)+1);
    m_szStartDir = (const char*)AObjLockData(m_oiLauncher, DATA_STARTDIRECTORY);
    }
    
  return fRet;
  }

////////////////////////////////////////////////////////////////////////////

BOOL Launcher::SetRunMinimized
  (
  BOOL  fMin
  )
  
  {
  BOOL  fVal = (fMin != FALSE);
  BOOL  fRet = (*m_pfRunMin != fVal);
  
  if(fRet)
    *m_pfRunMin = fVal;
    
  return fRet;
  }

////////////////////////////////////////////////////////////////////////////

long Launcher::Assign
  (
  const Launcher* pSource
  )

  {
  SetCommandLine(pSource->CommandLine());
  SetStartDirectory(pSource->StartDirectory());
  SetRunMinimized(pSource->RunMinimized());
  
  return A_OK;
  }
  
////////////////////////////////////////////////////////////////////////////

long Launcher::Edit
  (
  BOOL  fRuntime
  )
  
  {
  EditLauncher  el      = {this, fRuntime};
  LPCSTR        lpszDlg = fRuntime ? ID(DLG_RUNTIMELAUNCH) : ID(DLG_DESIGNLAUNCH);
  long          lRet;
  int           nRes;
  
  
  nRes = DialogBoxParam(g_hinstDLL, lpszDlg, GetActiveWindow(), LauncherEditDlg, (LPARAM)&el);
  
  if(nRes == A_WARNING) // dialog cancelled
    lRet = A_OK;
  else if((nRes == A_MODIFIED) || fRuntime)
    lRet = A_MODIFIED;
  else
    lRet = A_OK;        // OK button pressed, but no changes were made

  return lRet;
  }

////////////////////////////////////////////////////////////////////////////

long Launcher::Export
  (
  ) const
  
  {
  ExportShort(k_szVersion,     0L, VERSION_LAUNCHOBJECT);
  ExportBool (k_szMinimized,   0L, RunMinimized());
  ExportText (k_szCommandLine, 0L, CommandLine(), strlen(CommandLine()));
  ExportText (k_szStartupDir,  0L, StartDirectory(), strlen(StartDirectory()));
  
  return A_OK;
  }
  
////////////////////////////////////////////////////////////////////////////

long Launcher::Import
  (
  )

  {
  MEMBLOCKID  miText = AMemAllocate(0L);
  char*       szText;
  
  ImportBool(k_szMinimized, 0L, m_pfRunMin);
    
  if(ImportText(k_szCommandLine,  0L, miText) == AUPSF_NOERROR)
    {
    szText = (char*)AMemLock(miText);
    SetCommandLine(szText);
    AMemUnlock(miText);
    }
    
  if(ImportText(k_szStartupDir, 0L, miText) == AUPSF_NOERROR)
    {
    szText = (char*)AMemLock(miText);
    SetStartDirectory(szText);
    AMemUnlock(miText);
    }
    
  AMemFree(miText);
  
  return A_OK;
  }
  
////////////////////////////////////////////////////////////////////////////

long Launcher::LaunchApplication
  (
  char* szError,
  int   nErrorBufLen
  )
  
  {
  char        szCWDir[256];
  int         nDrive;
  const char* szStartDir    = StartDirectory();
  UINT        uRes;
  long        lRet;
  
  // Store the current working directory and drive -- restore after executing
  getcwd(szCWDir, sizeof(szCWDir));
  nDrive = getdisk();

  // set the start directory and drive  
  if(strlen(szStartDir)>0)
    {
    if(szStartDir[1]==':')
      {
      int nNewDrive = toupper(szStartDir[0]) - 'A';
      setdisk(nNewDrive);
      }
    chdir(szStartDir);
    }
    
  // execute the application
  uRes = WinExec(CommandLine(), CommandShow());
  if(uRes < 32)
    {
    // error occurred -- get the description
    if(0 == LoadString(g_hinstDLL, uRes, szError, nErrorBufLen))
      LoadString(g_hinstDLL, STR_UNKNOWNERROR, szError, nErrorBufLen);
      
    lRet = 1L;  // error condition ---- fix later.....
    }
  else
    lRet = 1L;

  // restore the working directory and drvie
  setdisk(nDrive);
  chdir(szCWDir);
  
  return lRet;
  }
  
////////////////////////////////////////////////////////////////////////////

void Launcher::SetApplication
  (
  OBJECTID  oiText
  )
  
  {
  MEMBLOCKID  miText = AMemAllocate(0L);
  char*       pText;
  char*       pRes;

  // get the command line text
  AObjConvertToText(oiText, miText);
  pText = (char*)AMemLock(miText);

  // set the command line
  SetCommandLine(pText);
  
  // figure out the startup directory
  if(!(pRes = strrchr(pText, '\\')))
    {
    if(!(pRes = strrchr(pText, ':')))
      SetStartDirectory("");
    else
      {
      // no path information set the start directory to "<Drive>:"
      pRes[1] = '\0';
      SetStartDirectory(pText);
      }
    }
  else
    {
    if(*(pRes-1)==':')
      pRes[1]=0; // leave the "\" if the root directory
    else
      pRes[0]=0; // remove the "\" if not root directory

    SetStartDirectory(pText);
    }

  // don't allow the application to run minimized
  SetRunMinimized(FALSE);

  // free allocated memory
  AMemUnlock(miText);
  AMemFree(miText);
  }
  
////////////////////////////////////////////////////////////////////////////
