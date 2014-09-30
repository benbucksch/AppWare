/////////////////////////////////////////////////////////////////////////////
// File Name:  FlopFunc.CPP
//    Copyright 1989-92 Serius Corporation
// Author:  Paul Ruben
// Date:  4/22/92
//
// Description.:
//  File operation functions
/////////////////////////////////////////////////////////////////////////////
#define STRICT
#define ASTRICT

#include <stdio.h>
#include <io.h>
#include <sys\stat.h>
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <dlgs.h>
#include <time.h>

#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mem.h>
#include <dos.h>
#include <dir.h>
#include <ctype.h>

#include <a_alm.h>
#include <intdebug.h>
#include <o_text.h>
#include <o_number.h>
#include <o_date.h>
#include <o_time.h>

#include "flop.h"
#include "dos_util.h"

#define BUFFERSIZE 8192
#define DIR_FILE    1
#define DIR         2
#define FILE_CONST  3
#define CURRENTDATAVERSION 0

struct pathName
  {
  char  name[256];
  };

extern "C" long FAR PASCAL _export GetSerVersion(TYPEID) ;
void fillIndexedName(LPSTR pathname, OBJECTID oiTemp) ;
void fillAncestors(LPSTR pathname, OBJECTID oiTemp, BOOL directory) ;
void fillContents(LPSTR pathname, OBJECTID oiTemp, BOOL directory) ;
void OutPutParm(pathName *list, int noFile);

typedef UINT (CALLBACK *DLGHOOKPROC)(HWND, UINT, WPARAM, LPARAM);
UINT FAR PASCAL DirHook(HWND hDlg, UINT msg, WPARAM  wParam,  LPARAM lParam);

HINSTANCE hInstance ;
char lpstrFilter[256] ;
extern int _WinAllocFlag;

long FAR PASCAL _export GetSerVersion(TYPEID)
  {
  return 1L ;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  PutFileFunc (fOpf)
// DESCRIPTION: Displays a SaveAs dialog box which allows the user to choose
//  a name and directory for a file.  This function warns the user when
//  the file already exists.  It does not create a new file nor does it
//  write over an existing file.
// INPUT 1 (optional): a Text Object = the Initial File Name
// INPUT 2 (optional): a Text Object = the Dialog Box Prompt
// OUTPUT 1 (optional): a Text Object = full path name including the file name
// OUTPUT 2 (optional): a Text Object = full path name excluding
// OUTPUT 3 (optional): a Text Object = file name
// FLOW 1: Followed if the user selects a file.
// FLOW 2: Followed if the user cancels the file dialog.
/////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export PutFileFunc(pAEvtInfo, pAObjMessage theSystem)
  {
  OBJECTID oiInitialName ; // InitialName parameter
  OBJECTID oiPrompt ; // Dialog box text
  OBJECTID oiText ; // temporary object used to send output
//  LPSTR lpText ; // points to the object data
  char prompt[30] ;
  OPENFILENAME ofn ;
//  int size ;
//  char FileName[MAXPATH] ;
  char buffer[MAXPATH] ;
  char file[MAXFILE] ;
  char ext[MAXEXT];
  char drive[MAXDRIVE];
  char dir[MAXDIR];

//  AFuncReportResult (theSystem, 1, "") ;

  // Set all structure members to zero
  _fmemset (&ofn, 0, sizeof(OPENFILENAME)) ;
  ofn.lStructSize = sizeof(OPENFILENAME) ; // Specifies the length of the structure in bytes
  ofn.hwndOwner = GetActiveWindow() ; // Identifies the window that owns the dialog box.
  ofn.lpstrFilter = lpstrFilter ; // filter strings
  ofn.nFilterIndex = 4 ; // selects the first filter in the szFilter buffer

  oiInitialName = AFuncGetTypedParameter(1, OTYPE_TEXT) ; // get the optional File Name input parameter
  if (oiInitialName == 0)
    buffer[0] = '\0' ;
  else
    OTextGetToBuffer(oiInitialName, buffer, sizeof(buffer)) ;
  ofn.lpstrFile = buffer ;
  ofn.nMaxFile = sizeof (buffer) ; // should be at least 256

  oiPrompt = AFuncGetTypedParameter(2, OTYPE_TEXT) ; // get the optional Dialog Prompt input parameter
  if (oiPrompt == 0)
    ofn.lpstrTitle = "Put File" ;
  else
    OTextGetToBuffer(oiPrompt, prompt, sizeof(prompt)) ;

  ofn.Flags = OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST |
      OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ;
  if (GetSaveFileName (&ofn) == FALSE)
    {
    ADBG_PRINT_I("The User canceled the 'Save File' dialog box.") ;
    theSystem->message1 = 2 ;
    return ;
    }

  theSystem->message1 = 1 ;

  // store full path name in text object to output parameter 1
  oiText = AObjCreate(OTYPE_TEXT) ; // get a ID to a new text object
  OTextSetValueFromBuffer(oiText, buffer) ;
  AFuncSetParameter(1, oiText) ;

  fnsplit(buffer, drive, dir, file, ext);
  // store path name in text object to output parameter 2
  oiText = AObjCreate(OTYPE_TEXT) ; // get a ID to a new text object
  lstrcpy(buffer, drive) ;
  lstrcat(buffer, dir) ;
  OTextSetValueFromBuffer(oiText, buffer) ;
  AFuncSetParameter(2, oiText) ;

  // store file name in text object to output parameter 3
  oiText = AObjCreate(OTYPE_TEXT) ; // get a ID to a new text object
  lstrcpy(buffer, file) ;
  lstrcat(buffer, ext) ;
  OTextSetValueFromBuffer(oiText, buffer) ;
  AFuncSetParameter(3, oiText) ;
  return ;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  RenameFileFunc (fOrn)
// DESCRIPTION: Renames a file but leaves it otherwise unchanged.  The file
//    is not relocated.
// INPUT 1: a Text Object = Old file path name
// INPUT 2: a Text Object = New file name
// FLOW 1: Followed after the file has been copied.
// FLOW 3: Followed if file was not renamed.
/////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export RenameFileFunc(pAEvtInfo, pAObjMessage theSystem)
  {
  OBJECTID oiSource, oiDestination ;
	int length;
	int	res;
  char sourceName[MAXPATH] ;
  char destinationName[MAXFILE + MAXEXT + 1] ;
  char fullPathName[MAXPATH] ;
  char file[MAXFILE] ;
  char ext[MAXEXT];
	struct stat fStat;
	BOOL		fIsRoot;

  oiSource      = AFuncGetTypedParameter(1, OTYPE_TEXT) ;
  oiDestination = AFuncGetTypedParameter(2, OTYPE_TEXT) ;

  OTextGetToBuffer(oiSource, sourceName, sizeof(sourceName)) ;
  OTextGetToBuffer(oiDestination, destinationName, sizeof(destinationName)) ;

  length = lstrlen(destinationName) ;
  if (length > 12)
    {
    ADBG_PRINT_E("The destination file name must be less than 13 characters.") ;
    theSystem->message1 = 2 ;
    return ;
    }

  lstrcpy(fullPathName, sourceName) ;

  for(int i = lstrlen(fullPathName) - 1; i > 1; i--)
    if (fullPathName[i] == '\\')
      {
      fullPathName[i] = '\0' ; // remove the source file name
      break ;
      }

	if (lstrlen(fullPathName) < 3 && fullPathName[1] == ':')		// add trailing "\"
		{
		lstrcat(fullPathName, "\\");
		fIsRoot = TRUE;
		}
	else
		fIsRoot = FALSE;

	if (stat(fullPathName, &fStat) != 0)
		{
    ADBG_PRINT_W("The source directory is not a valid directory.") ;
    theSystem->message1 = 2 ;
    return ;
		}
		
	if (!(fStat.st_mode & S_IFDIR))		// not a directory
    {
    ADBG_PRINT_W("The source path is not a valid directory.") ;
    theSystem->message1 = 2 ;
    return ;
    }

	if (!fIsRoot)
		lstrcat(fullPathName, "\\") ;

  fnsplit(destinationName, NULL, NULL, file, ext) ;
  lstrcat(fullPathName, file) ;
  lstrcat(fullPathName, ext) ;

  if (lstrcmpi(fullPathName, sourceName) == 0)
    {
    ADBG_PRINT_I("Source and destination are identical.") ;
    theSystem->message1 = 1 ;  // source and destination are identical so return
    return ;
    }

	if (access(fullPathName, 0) == 0)
		{
		chmod(fullPathName, S_IWRITE);
		remove(fullPathName);
		}

	res = rename(sourceName, fullPathName);

	if (res == ENOENT)
    {
    ADBG_PRINT_E("The source file doesn't exist.") ;
    theSystem->message1 = 2 ;  // source file doesn't exit
    return ;
    }
		
  theSystem->message1 = 1 ;
  }
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  MoveFileFunc (fOmv)
// DESCRIPTION: Moves a file specified by full pathname to a new location
//    also specified by path name.
// INPUT 1: a Text Object = source Pathname
// INPUT 2: a Text Object = destination Pathname (filename ignored)
// FLOW 1: Followed if the file is successfully moved.
// FLOW 2: Followed if moving the file failed.
/////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export MoveFileFunc(pAEvtInfo, pAObjMessage theSystem)
  {
  OBJECTID oiSource, oiDestination ;
  char far *lpBuffer ;
  int hSource, hDestination, nRead, length ;
  char sourceName[MAXPATH] ;
  char destinationName[MAXPATH] ;
  char file[MAXFILE] ;
  char ext[MAXEXT];
  struct ffblk ffBlk ;

  oiSource = AFuncGetTypedParameter(1, OTYPE_TEXT) ;
  oiDestination = AFuncGetTypedParameter(2, OTYPE_TEXT) ;

  OTextGetToBuffer(oiSource, sourceName, sizeof(sourceName)) ;
  OTextGetToBuffer(oiDestination, destinationName, sizeof(destinationName)) ;

  length = lstrlen(destinationName) ;
  if (length >= (MAXPATH - 13))
    {
    ADBG_PRINT_E("The destination file name is too long.") ;
    theSystem->message1 = 2 ;
    return ;
    }
  if(!((length==2) && (destinationName[length-1] == ':'))
            &&
     ((length>0) && (destinationName[length-1] != '\\')))
    {
    destinationName[length] = '\\';
    length++;
    destinationName[length] = '\0';
    }
  if(!GetFullPath(destinationName, sizeof(destinationName)))
    {
    ADBG_PRINT_W("The destination path is not a valid directory.") ;
    theSystem->message1 = 2 ;  // the Destination path is not a directory
    return ;
    }
  if(!GetFullPath(sourceName, sizeof(sourceName)))
    {
    ADBG_PRINT_W("The source path is not a valid directory.") ;
    theSystem->message1 = 2 ;  // the Destination path is not a directory
    return ;
    }

  length = lstrlen(destinationName) ;
  if ((length>0) && (destinationName[length-1] == '\\'))  // and a backslash exists,
    destinationName[--length] = 0 ;  // remove the backslash.
  if (length > 2)  // if this is not the root directory
    {
    if (findfirst(destinationName, &ffBlk, FA_DIREC) || (!(ffBlk.ff_attrib & FA_DIREC)))
      {
      ADBG_PRINT_W("The destination path is not a valid directory.") ;
      theSystem->message1 = 2 ;  // the Destination path is not a directory
      return ;
      }
    }
  else // if this is the root directory
    {
    lstrcpy(file, destinationName) ;
    lstrcat(file, "\\*.*") ;
    if (findfirst(file, &ffBlk, FA_DIREC) || (!(ffBlk.ff_attrib & FA_DIREC)))
      {
      ADBG_PRINT_W("The destination path is not a valid directory.") ;
      theSystem->message1 = 2 ;  // the Destination path is not a directory
      return ;
      }
    }
  lstrcat(destinationName, "\\") ;
  fnsplit(sourceName, NULL, NULL, file, ext) ;
  lstrcat(destinationName, file) ;
  lstrcat(destinationName, ext) ;

  hSource = _lopen(sourceName, OF_READ) ;
  if (hSource == -1)
    {
    ADBG_PRINT_E("The source file doesn't exist.") ;
    theSystem->message1 = 2 ;  // source file doesn't exit
    return ;
    }
  if (lstrcmpi(destinationName, sourceName) == 0)
    {
    ADBG_PRINT_I("Source and destination are identical.") ;
    theSystem->message1 = 1 ;  // source and destination are identical so return
    return ;
    }
  if (!findfirst(destinationName, &ffBlk, FA_DIREC))  // if the destination file already exists, delete it
    remove(destinationName) ;
  hDestination = _lcreat(destinationName, 0) ;
  if (hDestination == -1)
    {
    _lclose(hSource) ;
    ADBG_PRINT_E("Couldn't create destination file.") ;
    theSystem->message1 = 2 ;
    return ;
    }
  lpBuffer = new char[BUFFERSIZE] ;
  do
    {
    nRead = _lread(hSource, lpBuffer, BUFFERSIZE) ;
    if (nRead > 0)
      _lwrite(hDestination, lpBuffer, nRead) ;
    } while (nRead == BUFFERSIZE) ;
  delete [] lpBuffer ;

  _lclose(hSource) ;
  remove(sourceName) ;
  _lclose(hDestination) ;
  theSystem->message1 = 1 ;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  GetFileNamesFunc (fOgn)
// DESCRIPTION: Analyzes a full pathname into path name (the path up
//  to but not including the file) and file name.
// INPUT 1: a Text Object = the Full Pathname
// OUTPUT 1 (optional): a Text Object = full path name excluding the file name
// OUTPUT 2 (optional): a Text Object = file name
// FLOW 1: Followed after the file names are return.
/////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export GetFileNamesFunc(pAEvtInfo, pAObjMessage theSystem)
  {
  OBJECTID oiFullPathName ; // the Full Path Name parameter
  OBJECTID oiText ; // temporary object used to send output
  char buffer[MAXPATH] ;
  char file[MAXFILE] ;
  char ext[MAXEXT];
  char drive[MAXDRIVE];
  char dir[MAXDIR];

  oiFullPathName = AFuncGetTypedParameter(1, OTYPE_TEXT) ; // get the optional File Name input parameter
  OTextGetToBuffer(oiFullPathName, buffer, sizeof(buffer)) ;
  fnsplit(buffer, drive, dir, file, ext);

  // store path name in text object to output parameter 1
  oiText = AObjCreate(OTYPE_TEXT) ; // get a ID to a new text object
  lstrcpy(buffer, drive) ;
  lstrcat(buffer, dir) ;
  OTextSetValueFromBuffer(oiText, buffer) ;
  AFuncSetParameter(1, oiText) ;

  // store file name in text object to output parameter 2
  oiText = AObjCreate(OTYPE_TEXT) ; // get a ID to a new text object
  lstrcpy(buffer, file) ;
  lstrcat(buffer, ext) ;
  OTextSetValueFromBuffer(oiText, buffer) ;
  AFuncSetParameter(2, oiText) ;

  theSystem->message1 = 1 ;
  }
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  ChooseFileFunc
// DESCRIPTION: Displays an open file dialog allowing the user to choose
//  a file and reports the file name and path name of the chosen file
// INPUT 1 (optional): a Text Object = file types filter
// OUTPUT 1: a Text Object = full path name including the file name
// OUTPUT 2: a Text Object = full path name excluding
// OUTPUT 3: a Text Object = file name
// FLOW 1: Followed if the user selects a file.
// FLOW 2: Followed if the user cancels the file dialog.
/////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export ChooseFileFunc(pAEvtInfo, pAObjMessage theSystem)
  {
  OBJECTID oiFilter ; // optional filter input
  OBJECTID oiText ; // temporary object used to send output
  char filter[13] ;
  char buffer[50] ;
  OPENFILENAME ofn ;
  int size ;
  char FileName[256] ;
  char file[MAXFILE] ;
  char ext[MAXEXT];
  char drive[MAXDRIVE];
  char dir[MAXDIR];

//  AFuncReportResult (theSystem, 1, "") ;
  FileName[0] = '\0' ;

  // Set all structure members to zero
  _fmemset (&ofn, 0, sizeof(OPENFILENAME)) ;
  ofn.lStructSize = sizeof(OPENFILENAME) ; // Specifies the length of the structure in bytes
  ofn.hwndOwner = GetActiveWindow() ; // Identifies the window that owns the dialog box.
  oiFilter = AFuncGetTypedParameter(1, OTYPE_TEXT) ; // get the optional input parameter
  if (oiFilter)
    {
    OTextGetToBuffer(oiFilter, filter, sizeof(filter)) ;
    wsprintf(buffer, "Default filter (%s)", filter ) ;
    size = lstrlen(buffer) ;
    lstrcat(buffer, filter) ;
    buffer[lstrlen(buffer)+1] = '\0' ;
    buffer[size-1] = '\0' ;
    ofn.lpstrCustomFilter = buffer ;
    ofn.nMaxCustFilter = sizeof(buffer) ;
    }
  ofn.lpstrFilter = lpstrFilter ; // filter strings
  ofn.nFilterIndex = 0 ; // selects the first filter in the szFilter buffer
  ofn.lpstrFile = FileName ;
  ofn.nMaxFile = sizeof (FileName) ; // should be at least 256
  ofn.lpstrTitle = "Choose File" ;
  ofn.Flags =  OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ;

  if (GetOpenFileName (&ofn) == FALSE)
    {
    theSystem->message1 = 2 ;
    return ;
    }
  ext[1] = '\0';     // Initialize the extention string
  fnsplit(FileName, drive, dir, file, ext);

  // store full path name in text object to output parameter 1
  oiText = AObjCreate(OTYPE_TEXT) ; // get a ID to a new text object
  OTextSetValueFromBuffer(oiText, FileName) ;
  AFuncSetParameter(1, oiText) ;

  // store path name in text object to output parameter 2
  oiText = AObjCreate(OTYPE_TEXT) ; // get a ID to a new text object
  lstrcpy(FileName, drive) ;
  lstrcat(FileName, dir) ;
  OTextSetValueFromBuffer(oiText, FileName) ;
  AFuncSetParameter(2, oiText) ;

  // store file name in text object to output parameter 3
  oiText = AObjCreate(OTYPE_TEXT) ; // get a ID to a new text object
  lstrcpy(FileName, file) ;
  lstrcat(FileName, ext) ;
  OTextSetValueFromBuffer(oiText, FileName) ;
  AFuncSetParameter(3, oiText) ;

  oiText = AObjCreate(OTYPE_TEXT) ; // get a ID to a new text object
  OTextSetValueFromBuffer(oiText, &ext[1]) ;
  AFuncSetParameter(4, oiText) ;

  theSystem->message1 = 1 ;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  CopyFileFunc
// DESCRIPTION: Duplicates a specified file to a specified location on your
//  disk or on a file sharing network.
// INPUT 1: a Text Object = Source file path name
// INPUT 2: a Text Object = Destination file path name
// OUTPUT 1: none
// FLOW 1: Followed after the file has been copied.
// FLOW 2: Followed if the destination file already exists.
// FLOW 3: Followed if source is bad or file in use.
/////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export CopyFileFunc(pAEvtInfo, pAObjMessage theSystem)
  {
  OBJECTID oiSource, oiDestination ;
  char sourceName[MAXPATH] ;
  char destinationName[MAXPATH] ;
  char far *lpBuffer ;
  int hSource, hDestination, nRead ;

  oiSource = AFuncGetTypedParameter(1, OTYPE_TEXT) ;
  oiDestination = AFuncGetTypedParameter(2, OTYPE_TEXT) ;

  OTextGetToBuffer(oiSource, sourceName, sizeof(sourceName)) ;
  OTextGetToBuffer(oiDestination, destinationName, sizeof(destinationName)) ;
//  lpSource = (LPSTR)AObjLockData(oiSource, 1) ;
//  lpDestination = (LPSTR)AObjLockData(oiDestination, 1) ;

  hSource = _lopen(sourceName, OF_READ) ;
  if (hSource == -1)
    {
    theSystem->message1 = 3 ;
    return ;
    }
  hDestination = _lopen(destinationName, OF_READ) ;
  if (hDestination != -1) // file already exists
    {
    _lclose(hSource) ;
    _lclose(hDestination);
    theSystem->message1 = 2 ;
    return ;
    }
  hDestination = _lcreat(destinationName, 0) ;
  if (hDestination == -1) // file already exists
    {
    _lclose(hSource) ;
    _lclose(hDestination);
    theSystem->message1 =  2 ;
    return ;
    }
  lpBuffer = new char[BUFFERSIZE] ;
  do
    {
    nRead = _lread(hSource, lpBuffer, BUFFERSIZE) ;
    if (nRead > 0)
      _lwrite(hDestination, lpBuffer, nRead) ;
    } while (nRead == BUFFERSIZE) ;
  delete [] lpBuffer ;

  _lclose(hSource) ;
  _lclose(hDestination) ;
  theSystem->message1 = 1 ;
  }
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  DeleteFileFunc
// DESCRIPTION: Deletes a file.
// INPUT 1 (optional): a Text Object = File path name
// INPUT 2 (optional): a Text Object = file search filter
// OUTPUT 1: none
// FLOW 1: Followed after the file has been deleted.
// FLOW 2: Followed if the user canceled or if error occurs.
/////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export DeleteFileFunc(pAEvtInfo, pAObjMessage theSystem)
  {
  OBJECTID oiFile, oiFilter ;
  pathName *list;
  OPENFILENAME ofn ;
  struct ffblk ffBlk ;
  char filter[13] ;
  int size, nFiles, isDir, ndFiles ;
  char pathname[256] ;
  char buffer[MAXPATH],buf[MAXPATH] ;
  char file[MAXFILE] ;
  char ext[MAXEXT];
  char drive[MAXDRIVE];
  char dir[MAXDIR];
  int  done;
  int  errFlag = 0;
  int /*i = 0 ,*/ j = 0;
  char FileName[256];

  oiFile = AFuncGetTypedParameter(1, OTYPE_TEXT) ;
  if (oiFile)
    {
    OTextGetToBuffer(oiFile, pathname, sizeof(pathname)) ;
    if (pathname[0] != 0)
      {
      buffer[0] = 0;

      fnsplit(pathname, drive, dir, file, ext);

      // count files and directories
      nFiles = 0 ;
      ndFiles = 0;  // files deleted

      lstrcat(buffer, drive);
      lstrcat(buffer, dir);

      done = findfirst(pathname, &ffBlk, FA_DIREC) ;
      if (done)   // file/direcotry doesn't exist
        {
        theSystem->message1 = 3;
        return;
        }
      else
        {
        while (!done)
          {
          isDir = ffBlk.ff_attrib & FA_DIREC ;
          if (!isDir)
            nFiles++ ;
          done = findnext(&ffBlk) ;
          }
        list = (pathName *)new char[nFiles*sizeof(pathName)] ;
        if (list == NULL) return ;
        done = findfirst(pathname, &ffBlk, FA_DIREC) ;
        while (!done /*&& i < nFiles*/)
          {
          isDir = ffBlk.ff_attrib & FA_DIREC ;
          if (!isDir)
            {
            lstrcpy(buf, buffer);
            lstrcat(buf, ffBlk.ff_name);
            if (remove(buf) == 0)
              {
              lstrcpy(list[j++].name, buf) ;
              ndFiles++;
              theSystem->message1 = 1 ;
              }
            else
              {
              theSystem->message1 = 3 ;
              errFlag = 1;
              }
            }
          else
            {
            if (rmdir(pathname) == 0)
              {
              lstrcpy(list[0].name, pathname) ;
              ndFiles++;
              theSystem->message1 = 1 ;
              }
            else
              {
              theSystem->message1 = 3 ;
              errFlag = 1;
              }
            }
          done = findnext(&ffBlk) ;
          }

        if (errFlag == 1)
          theSystem->message1 = 3;    // enforce the error flow though some files have been deleted
        }

      OutPutParm(list, ndFiles);
      delete [] list ;

      return;
      }
    }

    list = (pathName *)new char[sizeof(pathName)] ;
    if (list == NULL) return ;

    FileName[0] = '\0' ;

    // Set all structure members to zero
    _fmemset (&ofn, 0, sizeof(OPENFILENAME)) ;
    ofn.lStructSize = sizeof(OPENFILENAME) ; // Specifies the length of the structure in bytes
    ofn.hwndOwner = GetActiveWindow() ; // Identifies the window that owns the dialog box.
    oiFilter = AFuncGetTypedParameter(2, OTYPE_TEXT) ; // get the optional input parameter

    if (oiFilter)
      {
      OTextGetToBuffer(oiFilter, filter, sizeof(filter)) ;

      if (lstrcmpi(filter, "dir") == 0) // delete directory
        {

        char szDirName[MAXPATH];
        char temp[MAXPATH] ;

        szDirName[0] = '\0';
        ofn.lCustData = (LPARAM)szDirName;
        ofn.Flags =   OFN_ENABLEHOOK | OFN_ENABLETEMPLATE | OFN_NOCHANGEDIR;
        ofn.lpfnHook = (DLGHOOKPROC)DirHook;
        ofn.lpTemplateName = "DIRECTORY";
        ofn.hInstance = hInstance;

        ofn.lpstrFilter = lpstrFilter ; // filter strings
        ofn.nFilterIndex = 0 ; // selects the first filter in the szFilter buffer
        ofn.lpstrFile = FileName ;
        ofn.nMaxFile = sizeof (FileName) ; // should be at least 256
        ofn.lpstrTitle = "Delete Diretory" ;

        if (GetOpenFileName (&ofn) == FALSE)
          {
          theSystem->message1 = 2 ;
          return ;
          }

        lstrcpy(temp, szDirName);
        int length = lstrlen(temp) ;
        for (int i=length; i--; i>1)  // go up one level of the current directory
          {
          if (temp[i] == '\\')
            {
            if (i != 2)   // make sure not overwrite the \ for the root directory s.t. c:\
              temp[i]='\0';
            break;
            }
          else
            temp[i]='\0';
          }

        chdir(temp); // move up one level
        if (rmdir(szDirName) == 0)        // remove a selected directory
          {
          lstrcpy(list[0].name, szDirName) ;
          ndFiles++;
          theSystem->message1 = 1 ;
          }
        else
          theSystem->message1 = 3 ;

        OutPutParm(list, ndFiles);
        delete [] list;

        return;
        }
      else
        {
        if (filter[0] == '\0')
          lstrcpy(filter, "*.*");

        wsprintf(buffer,"Default filter (%s) ",filter) ;
        size = lstrlen(buffer) ;
        lstrcat(buffer, filter) ;
        buffer[lstrlen(buffer)+1] = '\0' ;
        buffer[size-1] = '\0' ;
        ofn.lpstrCustomFilter = buffer ;
        ofn.nMaxCustFilter = sizeof(buffer) ;
        ofn.lpstrFilter = lpstrFilter ; // filter strings
        ofn.nFilterIndex = 0 ; // selects the first filter in the szFilter buffer
        ofn.lpstrFile = FileName ;
        ofn.nMaxFile = sizeof (FileName) ; // should be at least 256
        ofn.lpstrTitle = "Delete File" ;
        ofn.Flags =  OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

        if (GetOpenFileName (&ofn) == FALSE)
          {
          theSystem->message1 = 2 ;
          return ;
          }

        if (remove(FileName) == 0)
          {
          lstrcpy(list[0].name, FileName) ;
          ndFiles++;
          theSystem->message1 = 1 ;
          }
        else
          theSystem->message1 = 3 ;
        }

      OutPutParm(list, ndFiles);
      delete [] list ;

      return;
      }

      ofn.lpstrFilter = lpstrFilter ; // filter strings
      ofn.nFilterIndex = 0 ; // selects the first filter in the szFilter buffer
      ofn.lpstrFile = FileName ;
      ofn.nMaxFile = sizeof (FileName) ; // should be at least 256

      ofn.lpstrTitle = "Delete File" ;
      ofn.Flags =  OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

      if (GetOpenFileName (&ofn) == FALSE)
        {
        theSystem->message1 = 2 ;
        return ;
        }
      if (remove(FileName) != 0)
        theSystem->message1 = 3 ;
      else
        {
        lstrcpy(list[0].name, FileName) ;
        ndFiles++;
        theSystem->message1 = 1 ;
        }

      OutPutParm(list, ndFiles);
      delete [] list ;

      return;
  }

void OutPutParm(pathName *list, int noFile)
  {

  OBJECTID  oiFilename = AObjCreate(OTYPE_TEXT) ; // get a ID to a new text object
  OBJECTID  oiFileno = AObjCreate(OTYPE_NUMBER);  // get a ID to a new number object

  char far* lpTemp = new char[(sizeof(pathName)+2)*noFile] ;

  lpTemp[0] = 0 ;
  for (int i = 0; i < noFile; i++)
    {
    lstrcat(lpTemp, list[i].name) ;
    lstrcat(lpTemp, "\r\n") ;
    }
  OTextSetValueFromBuffer(oiFilename, lpTemp) ;
  ONmbrSetInteger(oiFileno, noFile, TRUE);

  delete [] lpTemp ;

  AFuncSetParameter(1, oiFileno) ;
  AFuncSetParameter(2, oiFilename);


  }

UINT FAR PASCAL _export DirHook(HWND hDlg, UINT msg, WPARAM  wParam,  LPARAM lParam)
  {
  LPOPENFILENAME ofn;
  LPSTR szDirName;

  switch(msg)
    {
    case WM_INITDIALOG:
      ofn = ( LPOPENFILENAME)lParam;
      szDirName = (LPSTR) ofn->lCustData;
      SetWindowLong(hDlg, DWL_USER, (long)szDirName);
      SetDlgItemText(hDlg, edt1, "test.dat");
      return TRUE;

    case WM_COMMAND:
      if (wParam == IDOK)
        {
        (long)szDirName =  GetWindowLong(hDlg, DWL_USER);
        GetDlgItemText(hDlg, stc1, szDirName, MAXPATH-1) ;
        }
      return FALSE;

    default:
      break;
    }


  return FALSE;


 }



/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  VerifyPath
// DESCRIPTION: Verify that the Reference Path identifies a valid directory
//  or file.
/////////////////////////////////////////////////////////////////////////////
BOOL VerifyPath(char far* lpRefPath, BOOL *refDirectory)
  {
  char buffer[256] ;
  struct ffblk ffBlk ;
  int i, flags ;

  *refDirectory = FALSE ;
  // check for leading spaces in the reference path
  lstrcpy(buffer, lpRefPath) ;
  for (i = 0; i < sizeof(buffer); i++)
    {
    if (buffer[i] == 0)  // check for an "empty" reference path
      {
      lpRefPath[0] = 0 ;
      *refDirectory = TRUE ;
      return TRUE ;
      }
    if (!isspace(buffer[i]))
      break ;
    }

  if (i)  // remove leading spaces from the reference path
    {
    lstrcpy(buffer, &lpRefPath[i]) ;
    lstrcpy(lpRefPath, buffer) ;
    }

  // remove trailing spaces in the reference path
  for (i = lstrlen(lpRefPath)-1 ; i >= 0; i--)
    {
    if (!isspace(lpRefPath[i]))
      break ;
    else
      lpRefPath[i] = 0 ;
    }

  i = lstrlen(lpRefPath) - 1 ;
  if (lpRefPath[i] == '\\')
    lpRefPath[i] = 0 ;

  flags = fnsplit(lpRefPath, NULL, NULL, NULL, NULL) ;
  if (flags & WILDCARDS)
    return FALSE ;
  if (!(flags & DRIVE))
    return FALSE ;
  if (!(flags & DIRECTORY || flags & FILENAME))
    *refDirectory = TRUE ; // this is a root directory
  else
    {
    if (findfirst(lpRefPath, &ffBlk, FA_DIREC) == 0)  // this will only work in large model   {
      *refDirectory = ffBlk.ff_attrib & FA_DIREC ;
    else
      return FALSE ;
    }
  return TRUE ;
  }
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  ListDrives
/////////////////////////////////////////////////////////////////////////////
void ListDrives(OBJECTID oiTemp)
  {
  int disk, save ;
  char buffer[5] ;
  char disklist[5*26 + 1] ;

  save = getdisk() ;
  disklist[0] = 0 ;
  lstrcpy(buffer, "A:\\\r\n") ;
  for (disk = 0; disk < 26; disk++)
    {
    setdisk(disk) ;
    if (disk == getdisk())
      {
      buffer[0] = disk + 'A' ;
      lstrcat(disklist, buffer) ;
      }
    }
  OTextSetValueFromBuffer(oiTemp, disklist) ;
  setdisk(save) ;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION: sort_function
/////////////////////////////////////////////////////////////////////////////
int sort_function(const void *a, const void *b)
  {
  return (lstrcmp((char *)a, (char *)b)) ;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  FillContents
/////////////////////////////////////////////////////////////////////////////
void fillContents(LPSTR pathname, OBJECTID oiTemp, BOOL directory)
  {
  OBJECTID oiFilter, oiOpCode ;
  char filter[20] ;
  int done, originalSize, nFiles, nDirectories, i ;
  int opcode ;
  fileName *list ;
  struct ffblk ffBlk ;
  BOOL isDir ;
  int  sizeDir=1;   // sizeof the directory

  if (!directory)
    return ;
  if (pathname[0] == '\0')
    {
    ListDrives(oiTemp) ;
    return ;
    }
  oiOpCode = AFuncGetTypedParameter(4, OTYPE_NUMBER) ;
  if (oiOpCode)
    opcode = (int)ONmbrGetInteger(oiOpCode) ;
  else
    opcode = DIR_FILE ;
  if (opcode < DIR_FILE) opcode = DIR_FILE ;
  if (opcode > FILE_CONST) opcode = FILE_CONST ;

  originalSize = lstrlen(pathname) ;
  if ((opcode == DIR_FILE) || (opcode == DIR))
    {
    nDirectories = 0 ;
    lstrcat(pathname, "\\*.*") ;
    done = findfirst(pathname, &ffBlk, FA_DIREC) ;
    while (!done)
      {
      isDir = ffBlk.ff_attrib & FA_DIREC ;
      if (isDir)
        nDirectories++ ;
      done = findnext(&ffBlk) ;
      }
    list = (fileName *)new char[nDirectories*sizeof(fileName)] ;
    if (list == NULL) return ;
    i = 0 ;
    done = findfirst(pathname, &ffBlk, FA_DIREC) ;
    while (!done && i < nDirectories)
      {
      isDir = ffBlk.ff_attrib & FA_DIREC ;
      if (isDir)
        lstrcpy(list[i++].name, ffBlk.ff_name) ;
      done = findnext(&ffBlk) ;
      }
    qsort((void*)list, nDirectories, sizeof(fileName), sort_function) ;
    char far* lpTemp = new char[(sizeof(fileName)+2)*nDirectories] ;
    lpTemp[0] = 0 ;
    for (i = 0; i < nDirectories; i++)
      {
      if(list[i].name[0] == '.') // skip directories . and ..
        continue ;
      lstrcat(lpTemp, list[i].name) ;
      lstrcat(lpTemp, "\\\r\n") ;
      }
    sizeDir = lstrlen(lpTemp) + 1;
    OTextSetValueFromBuffer(oiTemp, lpTemp) ;

    delete [] lpTemp ;
    delete [] list ;
    }

  if ((opcode == DIR_FILE) || (opcode == FILE_CONST))
    {
    pathname[originalSize] = '\0' ;
    oiFilter = AFuncGetTypedParameter(3, OTYPE_TEXT) ;
    if (oiFilter)
      {
      OTextGetToBuffer(oiFilter, filter, sizeof(filter)) ;
//      lpFilter = (LPSTR)AObjLockData(oiFilter, 1) ;
      lstrcat(pathname, "\\") ;
      lstrcat(pathname, filter) ;
      }
    else
      lstrcat(pathname, "\\*.*") ;

    // count files and directories
    nFiles = 0 ;
    done = findfirst(pathname, &ffBlk, FA_DIREC) ;
    while (!done)
      {
      isDir = ffBlk.ff_attrib & FA_DIREC ;
      if (!isDir)
        nFiles++ ;
      done = findnext(&ffBlk) ;
      }
    list = (fileName *)new char[nFiles*sizeof(fileName)] ;
    if (list == NULL) return ;
    i = 0 ;
    done = findfirst(pathname, &ffBlk, 0) ;
    while (!done && i < nFiles)
      {
      lstrcpy(list[i++].name, ffBlk.ff_name) ;
      done = findnext(&ffBlk) ;
      }
    qsort((void*)list, nFiles, sizeof(fileName), sort_function) ;

//    lpTemp = (LPSTR)AObjResizeData(oiTemp, 1, size + (sizeof(fileName)+1)*nFiles) ;
    char far* lpTemp = new char[sizeDir+(sizeof(fileName)+2)*nFiles] ;
//    int  totSize = sizeDir+(lstrlen(fileName)+2)*nFiles);
    OTextGetToBuffer(oiTemp, lpTemp, (sizeDir+15*nFiles));
    for (i = 0; i < nFiles; i++)
      {
      lstrcat(lpTemp, list[i].name) ;
      lstrcat(lpTemp, "\r\n") ;
      }
    OTextSetValueFromBuffer(oiTemp, lpTemp) ;
    delete [] lpTemp ;
//    size = lstrlen(lpTemp) + 1 ;
//    AObjResizeData(oiTemp, 1, size) ;
    delete [] list ;
    }
  pathname[originalSize] = '\0' ;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  findDrive
/////////////////////////////////////////////////////////////////////////////
BOOL findDrive(char *pathname, int index)
  {
  int disk, diskIndex, save ;

  save = getdisk() ;
  diskIndex = 0 ;
  for (disk = 0; disk < 26; disk++)
    {
    setdisk(disk) ;
    if (disk == getdisk()) ;
      {
      diskIndex++ ;
      if (diskIndex == index)
        {
        lstrcpy(pathname, "A:") ;
        pathname[0] = disk + 'A' ;
        setdisk(save) ;
        return TRUE ;
        }
      }
    }
  setdisk(save) ;
  return FALSE ;
  }
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  findChild
/////////////////////////////////////////////////////////////////////////////
BOOL findChild(char *pathname, BOOL *directory, int index)
  {
//  OBJECTID oiFilter ;
  int done, originalSize, nFiles, nDirectories, i, totDirectories ;
//  LPSTR lpFilter ;
  char name[13] ;
  fileName *list ;
  struct ffblk ffBlk ;
  BOOL isDir ;

  if (pathname [0] == '\0')
    {
    *directory = TRUE ;
    return findDrive(pathname, index) ;
    }
  originalSize = lstrlen(pathname) ;
  // count directories
  nDirectories = 0 ;
  totDirectories=0;
  lstrcat(pathname, "\\*.*") ;
  done = findfirst(pathname, &ffBlk, FA_DIREC) ;
  while (!done)
    {
    isDir = ffBlk.ff_attrib & FA_DIREC ;
    if (isDir)
      if(ffBlk.ff_name[0] != '.') // skip directories . and ..
        nDirectories++ ;
    totDirectories++;             // total no. of directory includes . and ..
    done = findnext(&ffBlk) ;
    }
  if (index <= (nDirectories))
    {
    list = (fileName *)new char[nDirectories*sizeof(fileName)] ;
    if (list == NULL) return FALSE ;
    i = 0 ;
    done = findfirst(pathname, &ffBlk, FA_DIREC) ;
    while (!done && i < totDirectories)
      {
      isDir = ffBlk.ff_attrib & FA_DIREC ;
      if (isDir)
        if(ffBlk.ff_name[0] != '.') // skip directories . and ..
          lstrcpy(list[i++].name, ffBlk.ff_name) ;
      done = findnext(&ffBlk) ;
      }
    qsort((void*)list, nDirectories, sizeof(fileName), sort_function) ;

    if (index > 0)
      lstrcpy(name, list[index-1].name) ;

    delete [] list ;
    *directory = TRUE ;
    pathname[originalSize] = '\0' ;
    lstrcat(pathname, "\\") ;
    lstrcat(pathname, name) ;
    return TRUE ;
    }

  pathname[originalSize] = '\0' ;
  OBJECTID oiFilter = AFuncGetTypedParameter(3, OTYPE_TEXT) ;
  if (oiFilter)
    {
    char filter[15] ;
    OTextGetToBuffer(oiFilter, filter, sizeof(filter)) ;
    lstrcat(pathname, "\\") ;
    lstrcat(pathname, filter) ;
    }
  else
    lstrcat(pathname, "\\*.*") ;
  // count files
  nFiles = 0 ;
  done = findfirst(pathname, &ffBlk, FA_DIREC) ;
  while (!done)
    {
    isDir = ffBlk.ff_attrib & FA_DIREC ;
    if (!isDir)
      nFiles++ ;
    done = findnext(&ffBlk) ;
    }
//  if (index > (nDirectories-2 + nFiles))
  if (index > nFiles)
    return FALSE ;

  list = (fileName *)new char[nFiles*sizeof(fileName)] ;
  if (list == NULL) return FALSE ;
  i = 0 ;
  done = findfirst(pathname, &ffBlk, 0) ;
  while (!done && i < nFiles)
    {
    lstrcpy(list[i++].name, ffBlk.ff_name) ;
    done = findnext(&ffBlk) ;
    }
  qsort((void*)list, nFiles, sizeof(fileName), sort_function) ;
//  lstrcpy(name, list[index-1-(nDirectories-2)].name) ;
  if (index > 0)
    lstrcpy(name, list[index-1].name) ;

  delete [] list ;
  *directory = FALSE ;
  pathname[originalSize] = '\0' ;
  lstrcat(pathname, "\\") ;
  lstrcat(pathname, name) ;
  return TRUE ;
  }
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  FindIndexedPath
// DESCRIPTION: Determine the indexed path
/////////////////////////////////////////////////////////////////////////////
BOOL FindIndexedPath(char *lpRefPath, BOOL refDirectory, BOOL *directory, int index)
  {
  int i, j ;

  if (index == 0)
    {
    *directory = refDirectory ;
    return TRUE ;
    }

  if (index < 0)
    {
    *directory = TRUE ;
    j = lstrlen(lpRefPath)-1 ;
    for (i = 0; i > index; i--)
      {
      while (lpRefPath[j] != '\\' && j > 0) j--;
      lpRefPath[j] = '\0' ;
      }
    return TRUE ;
    }

  // at this point the index must be greater than 0
  if (!refDirectory) // if the reference path points to a file
    return FALSE ;

  return findChild(lpRefPath, directory, index) ;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  DirectoryFunc
// DESCRIPTION: Obtains information about directories and files found on
//  all disks.
// INPUT 1: Directory Path
// INPUT 2: Index (0 = current directory, <0 = ancestor, >0 items in directory
// INPUT 3: Selection filter
// OUTPUT 1: Full Path Name
// OUTPUT 2: Directory Contents
// OUTPUT 3: Ancestors
// OUTPUT 4: Child Name
// FLOW 1: Is Directory
// FLOW 2: Is File
// FLOW 3: Is Error
/////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export DirectoryFunc(pAEvtInfo, pAObjMessage theSystem)
  {
  char path[MAXPATH] ;
  char buffer[MAXPATH+2] ;
  BOOL refDirectory = FALSE ;
  BOOL directory = FALSE ;

//  AFuncReportResult(theSystem, 1, "") ;
  OBJECTID oiRefPath = AFuncGetTypedParameter(1, OTYPE_TEXT) ;
  OBJECTID oiIndex = AFuncGetTypedParameter(2, OTYPE_NUMBER) ;
  if (!oiRefPath || !oiIndex)
    {
    ADBG_PRINT_E("Bad Inputs to Directory Function") ;
    AFuncReportResult(theSystem, 3, "") ;
    return ;
    }
  if (AObjGetDataSize(oiRefPath, 1) > MAXPATH)
    {
    ADBG_PRINT_E("Directory path must be less than 79 characters") ;
    AFuncReportResult(theSystem, 3, "") ;
    return ;
    }

  OTextGetToBuffer(oiRefPath, path, sizeof(path)) ;
  if (!VerifyPath(path, &refDirectory))
    {
    ADBG_PRINT_E("Invalid path provided") ;
    AFuncReportResult(theSystem, 3, "") ;
    return ;
    }
  int index = (int)ONmbrGetInteger(oiIndex) ;

  if (!FindIndexedPath(path, refDirectory, &directory, index))
    {
    ADBG_PRINT_E("Invalid index provided") ;
    AFuncReportResult(theSystem, 3, "") ;
    return ;
    }

  OBJECTID oiTemp = AObjCreate(OTYPE_TEXT) ;
  lstrcpy(buffer, path) ;
  if (directory && (path[0] != 0))
    lstrcat(buffer, "\\") ;
  OTextSetValueFromBuffer(oiTemp, buffer) ;
  AFuncSetParameter(1, oiTemp) ;

  oiTemp = AObjCreate(OTYPE_TEXT) ; // get a ID to a new text object
  fillContents(path, oiTemp, directory) ;
  AFuncSetParameter(2, oiTemp) ;

  oiTemp = AObjCreate(OTYPE_TEXT) ; // get a ID to a new text object
  fillAncestors(path, oiTemp, directory) ;
  AFuncSetParameter(3, oiTemp) ;

  oiTemp = AObjCreate(OTYPE_TEXT) ; // get a ID to a new text object
  fillIndexedName(path, oiTemp) ;
  AFuncSetParameter(4, oiTemp) ;

  if (directory)
    AFuncReportResult(theSystem, 1, "") ;
  else
    AFuncReportResult(theSystem, 2, "") ;
  }
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  fillIndexedName
/////////////////////////////////////////////////////////////////////////////
void fillIndexedName(LPSTR pathname, OBJECTID oiTemp)
  {
  char file[MAXFILE + MAXEXT] ;
  char ext[MAXEXT] ;

  fnsplit(pathname, NULL, NULL, file, ext) ;
  lstrcat(file, ext) ;
  OTextSetValueFromBuffer(oiTemp, file) ;
  }
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  FillAncestors
/////////////////////////////////////////////////////////////////////////////
void fillAncestors(LPSTR pathname, OBJECTID oiTemp, BOOL directory)
  {
  char temp[MAXPATH + 1] ;
  char ancestors[MAXPATH*2] ; // allow extra space for "\r\n"

  if (pathname[0] == '\0')
    return ;

  lstrcpy(temp, pathname) ;
  if (directory)
    lstrcat(temp, "\\") ;

  int length = lstrlen(temp) ;
  ancestors[0] = 0 ;
  for (int j = 0 ; j < (length - 1); j++)
    {
    int i = j ;
    while (temp[j] != '\\' && j < length) j++ ;
    temp[j] = 0 ;
    lstrcat(ancestors, &temp[i]) ;
    if (j < length)
      lstrcat(ancestors, "\\") ;
    if (j < (length - 1))
      lstrcat(ancestors, "\r\n") ;
    }
  OTextSetValueFromBuffer(oiTemp, ancestors) ;
  }
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  DiskFunc
/////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export DiskLabelFunc(pAEvtInfo, pAObjMessage theSystem)
  {
//  struct ffblk ffBlk ;
  char   szDrive[MAXDRIVE];

//  ffBlk.ff_name[0] = '\0';   // Initialize the disk label name
  GetCurrentDrive(szDrive, sizeof(szDrive));
//  findfirst("*.*", &ffBlk, FA_LABEL) ;
  OBJECTID oiDriveLetter = AObjCreate(OTYPE_TEXT) ;

  OTextSetValueFromBuffer(oiDriveLetter, szDrive) ;
  AFuncSetParameter(1, oiDriveLetter) ;
  AFuncReportResult(theSystem, 1, "") ;
  }
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  CreateDirectoryFunc
// DESCRIPTION: Creates a new directory.
// INPUT 1: a Text Object = Directory name
// INPUT 2 (optional): a Text Object = path name
// OUTPUT 1: none
// FLOW 1: Followed after the directory has been created.
// FLOW 2: Followed if the directory couldn't be opened.
/////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export CreateDirectoryFunc(pAEvtInfo, pAObjMessage theSystem)
  {
  char directory[MAXFILE+MAXEXT] ;
  char path[MAXPATH], fullname[MAXPATH], tPathname[MAXPATH] ;
  int nSize ;
  int done;
  int dirExist=0;
  struct ffblk ffBlk ;
  BOOL isDir ;

  fullname[0] = '\0';
  OBJECTID oiDir = AFuncGetTypedParameter(1, OTYPE_TEXT) ;
  nSize = OTextGetToBuffer(oiDir, directory, sizeof(directory)) ;
  OBJECTID oiPath = AFuncGetTypedParameter(2, OTYPE_TEXT) ;
  if (oiPath)
    {

    OTextGetToBuffer(oiPath, path, sizeof(path) - nSize) ;
    if (path[0] == 0)        // pathname not provided
      {
      tPathname[0] = '\0';
      if(!GetFullPath(tPathname, sizeof(tPathname)))
        {
        ADBG_PRINT_W("The pathname is invalid directory.") ;
        theSystem->message1 = 3 ;  // the Destination path is invalid
        return ;
        }
      lstrcpy(fullname, tPathname);
      lstrcpy(path, tPathname);

      }
    else
      {
      int length = lstrlen(path);
      if (path[length] != '\\' && length !=3)  // avoid the drive only path s.t. c:\
        lstrcat(path, "\\") ;

      path[lstrlen(path)]='\0';

      lstrcpy(fullname, path);

      BOOL refDirectory = FALSE ;
      if (!VerifyPath(path, &refDirectory))
        {
        ADBG_PRINT_E("Invalid path provided") ;
        theSystem->message1 = 3;
        return ;
        }
      }

      lstrcat(fullname, directory) ;

    }
    else    // pathname is not provided
    {
    int length=0;
    tPathname[length] = '\0';
    if(!GetFullPath(tPathname, sizeof(tPathname)))
      {
      ADBG_PRINT_W("The pathname is invalid directory.") ;
      theSystem->message1 = 3 ;  // the Destination path is invalid
      return ;
      }
    lstrcpy(fullname, tPathname);
    lstrcpy(path, tPathname);
    lstrcat(fullname, directory) ;
    }


  lstrcat(path, "\\*.*");
  // Find out if the directory exists
  done = findfirst(path, &ffBlk, FA_DIREC) ;
  while (!done)
    {
    isDir = ffBlk.ff_attrib & FA_DIREC ;
    if (isDir)

      if(ffBlk.ff_name[0] != '.') // skip directories . and ..
        if ( lstrcmpi(ffBlk.ff_name, directory) == 0 )
          {
          dirExist = 1;
          break;
          }
    done = findnext(&ffBlk) ;
    }

  if (!dirExist)
    {
    if (mkdir(fullname) == 0)
      theSystem->message1 = 1;        // the directory is created
    else
      theSystem->message1 = 3;    // fail to create the directory
    }
  else                       //  directory exists
    theSystem->message1 = 2;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  PathNameFunc
// DESCRIPTION: Returns the path name of the current folder.
// INPUT 1: none
// OUTPUT 1: a text object = pathname of the current working directory
// FLOW 1: Followed after the current pathname is returned.
/////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export PathNameFunc(pAEvtInfo, pAObjMessage theSystem)
  {
  char path[MAXPATH] ;

  if (getcwd(path, sizeof(path)) != path)
    {
    ADBG_PRINT_E("The 'C' function getcwd returned an error") ;
    AFuncReportResult(theSystem, 1, "") ;
    }
  OBJECTID oiPath = AObjCreate(OTYPE_TEXT) ;
  if(lstrlen(path) > 3) // make sure "C:\" does not get an extra "\"
    lstrcat(path, "\\") ;
  OTextSetValueFromBuffer(oiPath, path) ;
  AFuncSetParameter(1, oiPath) ;
  AFuncReportResult(theSystem, 1, "") ;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  GetCreationInfoFunc
// DESCRIPTION: Returns the creation date and time of a specified file.
// INPUT 1: Pathname = specify the full pathname of the file.
// OUTPUT 1: Creation Date - report the file's creation date.
// OUTPUT 2: Creation Time - reports the file's creation time.
// FLOW 1: Retrieved.
// FLOW 2: Failed.
/////////////////////////////////////////////////////////////////////////////
void ALMCALLBACK GetCreationInfoFunc
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);

  struct stat   statBuf;
  char          pathname[MAXPATH];
  OBJECTID      oiPathname = AFuncGetTypedParameter(1, OTYPE_TEXT) ;

  OTextGetToBuffer(oiPathname, pathname, sizeof(pathname)) ;

  if (stat(pathname, &statBuf) != 0)
    pMsg->message1 = 2L;
  else
    {
    OBJECTID  oiDate = AObjCreate(OTYPE_DATE);
    OBJECTID  oiTime = AObjCreate(OTYPE_TIME);
    char      szDate[80];
    ODATE     od;
    OTIME     ot;
    OTIMEHMS  hms;
    struct tm tmFile;

    // extract the creation date
    tmFile = *localtime(&statBuf.st_ctime);
    
    // compose date
    sprintf(szDate, "%d/%d/%d", tmFile.tm_mon + 1, tmFile.tm_mday, tmFile.tm_year + 1900);
    od = ODateStringToDate(szDate);
    ODateSetValue(oiDate, od);
    AFuncSetParameter(1L, oiDate);

    // compose time
    hms.hour = tmFile.tm_hour;
    hms.min = tmFile.tm_min;
    hms.sec = tmFile.tm_sec;
    ot = OTimeFromHMS(&hms);
    OTimeSetValue(oiTime, ot);
    AFuncSetParameter(2L, oiTime);

    pMsg->message1 = 1L;
    }
  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  GetModificationInfoFunc
// DESCRIPTION: Returns the creation date and time of a specified file.
// INPUT 1: Pathname = specify the full pathname of the file.
// OUTPUT 1: Modification Date - report the file's Modification date.
// OUTPUT 2: Modification Time - reports the file's Modification time.
// FLOW 1: Retrieved.
// FLOW 2: Failed.
/////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export GetModificationInfoFunc
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);

  struct stat   statBuf;
  char          pathname[MAXPATH];
  OBJECTID      oiPathname = AFuncGetTypedParameter(1, OTYPE_TEXT) ;

  OTextGetToBuffer(oiPathname, pathname, sizeof(pathname)) ;

  if (stat(pathname, &statBuf) != 0)
    pMsg->message1 = 2L;
  else
    {
    OBJECTID  oiDate = AObjCreate(OTYPE_DATE);
    OBJECTID  oiTime = AObjCreate(OTYPE_TIME);
    char      szDate[80];
    ODATE     od;
    OTIME     ot;
    OTIMEHMS  hms;
    struct tm tmFile;

    // extract the modification date
    tmFile = *localtime(&statBuf.st_mtime);
    
    // compose date
    sprintf(szDate, "%d/%d/%d", tmFile.tm_mon + 1, tmFile.tm_mday, tmFile.tm_year + 1900);
    od = ODateStringToDate(szDate);
    ODateSetValue(oiDate, od);
    AFuncSetParameter(1L, oiDate);

    // compose time
    hms.hour = tmFile.tm_hour;
    hms.min = tmFile.tm_min;
    hms.sec = tmFile.tm_sec;
    ot = OTimeFromHMS(&hms);
    OTimeSetValue(oiTime, ot);
    AFuncSetParameter(2L, oiTime);

    pMsg->message1 = 1L;
    }
  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  GetSizeFunc
// DESCRIPTION: Reports the specified file's size.
// INPUT 1: Pathname = specify the full pathname of the file.
// OUTPUT 1: Size - report the file's size in bytes.
// FLOW 1: Retrieved.
// FLOW 2: Failed.
/////////////////////////////////////////////////////////////////////////////
void ALMCALLBACK GetSizeFunc
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);

  struct stat statBuf;
  char        pathname[MAXPATH];
  OBJECTID    oiPathname = AFuncGetTypedParameter(1L, OTYPE_TEXT);

  OTextGetToBuffer(oiPathname, pathname, sizeof(pathname));

  if (!stat(pathname, &statBuf))
    {
    OBJECTID oiSize = AObjCreate(OTYPE_NUMBER);

    ONmbrSetInteger(oiSize, statBuf.st_size, FALSE);
    AFuncSetParameter(1L, oiSize);

    pMsg->message1 = 1;
    }
  else
    pMsg->message1 = 2;
  }


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  GetTypesFunc
// DESCRIPTION: Reports the specified file's creator and file types.
// INPUT 1: Pathname = specify the full pathname of the file.
// OUTPUT 1: Creator Type
// OUTPUT 2: File Type - reports the file's file type ID
// FLOW 1: Retrieved.
// FLOW 2: Failed.
/////////////////////////////////////////////////////////////////////////////
void ALMCALLBACK GetTypesFunc
  (
  pAEvtInfo     pEvt,
  pAObjMessage  pMsg
  )

  {
  I_UNUSED(pEvt);

  OFSTRUCT  of;
  HFILE     hfile;
  OBJECTID  oiPathname = AFuncGetTypedParameter(1, OTYPE_TEXT);
  char      szPath[MAXPATH];

  OTextGetToBuffer(oiPathname, szPath, sizeof(szPath)) ;

  hfile = OpenFile(szPath, &of, OF_PARSE);
  if (hfile == HFILE_ERROR)
    pMsg->message1 = 2;
  else
    {
    char      szExt[MAXEXT];
    char      szType[80];
    OBJECTID  oiType = AObjCreate(OTYPE_TEXT);
    OBJECTID  oiFile = AObjCreate(OTYPE_TEXT);
    long      l = sizeof(szType);

    // get the file type (the file extension)
    fnsplit(szPath, NULL, NULL, NULL, szExt) ;
    OTextSetValueFromBuffer(oiFile, szExt);
    AFuncSetParameter(2L, oiFile);

    // get the creator type from the registration database
    if(RegQueryValue(HKEY_CLASSES_ROOT, szExt, szType, &l) != ERROR_SUCCESS)
      lstrcpy(szType, "");
    else
      {
      l = sizeof(szType);
      RegQueryValue(HKEY_CLASSES_ROOT, szType, szType, &l);
      }

    OTextSetValueFromBuffer(oiType, szType);
    AFuncSetParameter(1L, oiType) ;

    pMsg->message1 = 1;
    }
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  File Open?
// DESCRIPTION: Find out if the file is opened.
// INPUT 1: Pathname = specify the full pathname of the file.
// FLOW 1: Read
// FLOW 2: Write
// FLOW 3: Read/Write
// FLOW 4: Not Open
/////////////////////////////////////////////////////////////////////////////
//void FAR PASCAL _export FileOpenFunc(pAEvtInfo, pAObjMessage theSystem)
//  {
//  char   pathname[MAXPATH];
//  FILE   *fp;

//  OBJECTID oiPathname = AFuncGetTypedParameter(1, OTYPE_TEXT) ;
//  OTextGetToBuffer(oiPathname, pathname, sizeof(pathname)) ;

//  theSystem->message1 = 4;
//  if ((fp = fopen(pathname, "r")) != NULL)
//    theSystem->message1 = 1;
//  if ((fp = fopen(pathname, "a")) != NULL)
//    theSystem->message1 = 2;

//  fclose(fp);

//  }

/////////////////////////////////////////////////////////////////////////
//  LibMain
////////////////////////////////////////////////////////////////////////
int FAR PASCAL LibMain( HINSTANCE hInst, WORD /*wDataSegment*/,
                   WORD wHeapSize, LPSTR /*lpszCmdLine*/ )
  {
  _WinAllocFlag = GMEM_SHARE ;
  hInstance = (HINSTANCE) hInst ;
  if ( wHeapSize != 0 )
    UnlockData( 0 ) ;
  int cbString = LoadString (hInstance, IDS_FILTERSTRING, lpstrFilter, sizeof(lpstrFilter)) ;
  char chReplace = lpstrFilter[cbString - 1]; // retrieve wildcard character
  for (int i = 0; lpstrFilter[i] != '\0'; i++)
    if (lpstrFilter[i] == chReplace)
      lpstrFilter[i] = '\0' ;
  return 1;   // Indicate that the DLL was initialized successfully.
  }

////////////////////////////////////////////////////////////////////////
//  WEP
////////////////////////////////////////////////////////////////////////
int FAR PASCAL WEP ( int /*bSystemExit*/ )
  {
  return 1;
  }



