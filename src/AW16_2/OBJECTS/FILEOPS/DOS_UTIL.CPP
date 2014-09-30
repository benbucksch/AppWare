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
// FILE:          "dos_util.cpp"
//
// AUTHOR:        Kevin J. Smart
//
// DESCRIPTION:   Special utilities for working with DOS files.  These
//                functions will work for DOS ONLY.
//
////////////////////////////////////////////////////////////////////////////

#include <dir.h>
#include <ctype.h>
#include <string.h>

#include "dos_util.h"

/////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:    GetCurrentDrive  (DOS only)
//
// DESCRIPTION: This function will get the current DOS drive letter if the
//              form of "X:", where 'X' is the drive letter.
//
/////////////////////////////////////////////////////////////////////////////

BOOL GetCurrentDrive
  (
  char* szDrive,
  int   nBufLen
  )

  {
  BOOL  fRet;
  
  if(nBufLen>=MAXDRIVE)
    {
    szDrive[0] = 'A' + getdisk();
    szDrive[1] = ':';
    szDrive[2] = '\0';
    
    fRet = TRUE;
    }
  else
    fRet = FALSE;
    
  return fRet;
  }

/////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:    GetCurrentDirectory  (DOS only)
//
// DESCRIPTION: This function will get the current DOS directory on the 
//              current drive.  The directory will include the first and
//              last PATHDELIMITER characters (unless the current directory
//              is the root directory).
//
/////////////////////////////////////////////////////////////////////////////

BOOL GetCurrentDirectory
  (
  int   nDrive,
  char* szDir,
  int   nBufLen
  )

  {
  BOOL  fRet  = FALSE;
  
  if(nBufLen>=MAXDIR)
    {
    int length;

    // always have the leading PATHDELIMITER character
    szDir[0] = PATHDELIMITER;
    if(getcurdir(nDrive, szDir+1)==0)
      {
      length = strlen(szDir);
      if(length > 1)
        {
        szDir[length] = PATHDELIMITER;
        szDir[length+1] = '\0';
        }
        
      fRet = TRUE;
      }
    }
    
  return fRet;
  }

/////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:    ConvertToRealDir  (DOS only)
//
// DESCRIPTION: This function will take a relative path and convert it to a
//              full path.
//
/////////////////////////////////////////////////////////////////////////////

BOOL ConvertToRealDir
  (
  int   nDrive,
  char* szDir,
  int   nBufLen
  )

  {
  BOOL  fRet;
  
  if((nBufLen>=MAXDIR) && (szDir[0]!=PATHDELIMITER))
    {
    // a relative path
    char  szCurDir[MAXPATH];
    int   offset = 0;

    fRet = GetCurrentDirectory(nDrive, szCurDir, sizeof(szCurDir));

    if(fRet && !memcmp(szDir, CURRENTDIRECTORY, CURDIRLEN))
      {
      // The path starts with the CURRENTDIRECTORY delimiter.  Set the 
      // offset to skip past the delimiter.
      offset = CURDIRLEN;
      }
    else if(fRet)
      {
      int   length  = strlen(szCurDir);
  
      // While the given path has the PREVIOUSDIRECTORY string, we need
      // to move the current directory back one level.  This loop will
      // also abort when the length of szCurDir gets to 1 (the PATHDELIMITER
      // size).
      while((length>1)
                &&
            !memcmp(&szDir[offset], PREVIOUSDIRECTORY, PREVDIRLEN))
        {
        length--;
        while((length>1) && (szCurDir[length-1] != PATHDELIMITER))
          length--;

        szCurDir[length] = '\0';
        offset += PREVDIRLEN;
        }
      }

    // We have now modified the current directory to skip the necessary
    // parent directories and have set the directory offset to skip all
    // relative path information.
    if(fRet)
      {
      if((strlen(szCurDir) + strlen(&szDir[offset]) + 1) < nBufLen)
        {
        strcat(szCurDir, &szDir[offset]);
        strcpy(szDir, szCurDir);
        }
      else
        fRet = FALSE;
      }
    }
  else
    fRet = FALSE;
    
  return fRet;
  }

/////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:    GetFullPath (DOS only)
//
// DESCRIPTION: This function will convert a relative path to a full path
//              with a drive letter and directories.
//
/////////////////////////////////////////////////////////////////////////////

BOOL GetFullPath
  (
  char* szPath,
  int   nBufLen
  )

  {
  BOOL  fRet;
  
  if(nBufLen>=MAXPATH)
    {
    char  szDrive[MAXDRIVE];
    char  szDir[MAXDIR];
    char  szFile[MAXFILE];
    char  szExt[MAXEXT];
    int   nRet;
    int   nDrive;

    // split the given path into its components
    nRet = fnsplit(szPath, szDrive, szDir, szFile, szExt);

    if(!(nRet & DRIVE))
      {
      // given path had no drive information -- use active drive
      fRet = GetCurrentDrive(szDrive, sizeof(szDrive));
      }
    else
      fRet = TRUE;

    // convert the drive letter to a one-based index
    if(fRet)
      nDrive = (int)(toupper(szDrive[0]) - 'A') + 1;

    if(fRet && !(nRet & DIRECTORY))
      {
      // there was no given directory -- use the current working dir.
      fRet = GetCurrentDirectory(nDrive, szDir, sizeof(szDir));
      }
    else if(fRet && (szDir[0] != PATHDELIMITER))
      {
      // the given directory was a relative path
      fRet = ConvertToRealDir(nDrive, szDir, sizeof(szDir));
      }

    // build the full path from the components
    if(fRet)
      {
      if((strlen(szDrive) + strlen(szDir) + strlen(szFile) + strlen(szExt) + 1) < nBufLen)
        {
        strcpy(szPath, szDrive);
        strcat(szPath, szDir);
        strcat(szPath, szFile);
        strcat(szPath, szExt);
        }
      else
        fRet = FALSE;
      }
    }
  else
    fRet = FALSE;
    
  return fRet;
  }

////////////////////////////////////////////////////////////////////////////
