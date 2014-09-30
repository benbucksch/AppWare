/////////////////////////////////////////////////////////////////////////////
// File Name:  FileFunc.CPP
// 		Copyright 1989-92 Serius Corporation
// Author:  Paul Ruben
// Date:  4/22/92
//
// Description.:
//	Contains all Serius functions associated with the Serius File object:
//		FileNameFunc
//      NewFileFunc
//      SaveAsFileFunc
//		SaveFileFunc
//		LoadFileFunc
//		OpenFileFunc
//
//	Also contains the following support modules:
//		FileNameDlgProc: a dialog procedure for specifying a file name
//		lstrchr: returns a pointer to the first occurance of a character in a string
//		lstrrchr: returns a pointer to the last occurance of a character in a string
//		LibMain: saves the instance ID of the DLL
//		WEP: called when a DLL is removed from memory
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <commdlg.h>
#include <mem.h>
#include <dir.h>
#include <a_alm.h>
#include <o_text.h>
#include <string.h>
#include "fileobj.h"

BOOL LoadFile(char far* lpFullPathname, OBJECTID far* lpObjects, long nObjectsAnticipated ) ;
BOOL SaveFile(char far* lpFullPathname, OBJECTID far* lpObjects, long nObjects ) ;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  FileNameFunc
// DESCRIPTION:  Parses the path name and file name of a Serius File object.
// INPUT 1: a File Object
// OUTPUT 1: a Text Object = file name
// OUTPUT 2: a Text Object = full path name including the file name
// FLOW 1: this is the only option currently available
/////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export FileNameFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	OBJECTID oiText ; // Handle to the Text object
	OBJECTID oiFile ; // Handle to the File object
	FileInfo far* lpFileInfo ;
	char drive[MAXDRIVE];
	char dir[MAXDIR];
	char file[MAXFILE];
	char ext[MAXEXT];
	char buffer[MAXDRIVE + MAXDIR + MAXFILE + MAXEXT] ;

	oiFile = AFuncGetTypedParameter(1, OTYPE_FILE) ;  // get the ID for the File object
	lpFileInfo = (FileInfo far*)AObjLockData(oiFile, INFO_INDEX) ; // get the file information for this record
	if (lpFileInfo == NULL)
		return ;

	fnsplit(lpFileInfo->lpstrFile, drive, dir, file, ext);

  if (!strcmp(file, "NONAME"))	  // File doesn't exit
		theSystem->message1 = 2 ;											// doesn't exit flow
	else
		theSystem->message1 = 1 ;

	// store file name in text object in parameter 1
	lstrcpy(buffer, file) ;
	lstrcat(buffer, ext) ;
	oiText = AObjCreate(OTYPE_TEXT) ; // get a ID to a new text object
	OTextSetValueFromBuffer(oiText, buffer) ;
	AFuncSetParameter(1, oiText) ;

	// store the path name excluding the file name in parameter 2
	lstrcpy(buffer, drive) ;
	lstrcat(buffer, dir) ;
	oiText = AObjCreate(OTYPE_TEXT) ;
	OTextSetValueFromBuffer(oiText, buffer) ;
	AFuncSetParameter(2, oiText) ;

	// store the full path name in parameter 3
	lstrcpy(buffer, lpFileInfo->lpstrFile) ;
	oiText = AObjCreate(OTYPE_TEXT) ;
	OTextSetValueFromBuffer(oiText, buffer) ;
	AFuncSetParameter(3, oiText) ;

	AObjUnlockData(oiFile, INFO_INDEX) ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  NewFileFunc
// DESCRIPTION:  Brings up a dialog that allows the user to create a new file
//			if a new name was not provided in input parameter 2.
//			The newly created file will contain no object data.
// INPUT 1: a File Object
// INPUT 2 (OPTIONAL): a Text Object = new file name
// FLOW 1: the file has been created
// FLOW 2: the user canceled or an error occured
/////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export NewFileFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	OBJECTID oiFile ; // ID of the File object
	OBJECTID oiFileName ; // ID of the Text object
	FileInfo far* lpFileInfo ;
	OPENFILENAME ofn ;
	char oldFileName[256] ;
	char customFilter[40] ; //

	oiFile = AFuncGetTypedParameter(1, OTYPE_FILE) ; // get the ID for the File object
	lpFileInfo = (FileInfo far*)AObjLockData(oiFile, INFO_INDEX) ; // get the file information for this record

	oiFileName = AFuncGetTypedParameter(2, OTYPE_TEXT) ;
	if (oiFileName)
		OTextGetToBuffer(oiFileName, lpFileInfo->lpstrFile, sizeof(lpFileInfo->lpstrFile)) ;
	else
		{
		// Set all structure members to zero
		lstrcpy(oldFileName, lpFileInfo->lpstrFile) ;
		lstrcpy(lpFileInfo->lpstrFile, "untitled") ;
		_fmemset (&ofn, 0, sizeof(OPENFILENAME)) ;
		ofn.lStructSize = sizeof(OPENFILENAME) ; // Specifies the length of the structure in bytes
		ofn.hwndOwner = GetActiveWindow() ; // Identifies the window that owns the dialog box.
		ofn.lpstrFilter = lpstrFilter ; // filter strings
		ofn.nFilterIndex = lpFileInfo->nFilterIndex ; // selects the first filter in the szFilter buffer
		ofn.lpstrFile = lpFileInfo->lpstrFile ;
		ofn.nMaxFile = sizeof (lpFileInfo->lpstrFile) ; // should be at least 256
		ofn.lpstrTitle = "Create File" ;
		ofn.Flags =  OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR | OFN_HIDEREADONLY ;
		wsprintf(customFilter,"[*.%s] *.%s", lpFileInfo->lpstrDefExt, lpFileInfo->lpstrDefExt) ;
		customFilter[lstrlen(customFilter)+1] = '\0' ;
		customFilter[lstrlen(lpFileInfo->lpstrDefExt)+4] = '\0' ;
		ofn.lpstrCustomFilter = customFilter ;
		ofn.nMaxCustFilter = sizeof(customFilter) ;
		ofn.lpstrDefExt = lpFileInfo->lpstrDefExt ;

		if (GetSaveFileName (&ofn) == FALSE)
			{
			AObjUnlockData(oiFile, INFO_INDEX) ;
			lstrcpy(lpFileInfo->lpstrFile, oldFileName) ;
			theSystem->message1 = 2 ;
			return ;
			}
		lpFileInfo->nFilterIndex = (int)ofn.nFilterIndex  ; // saves the first filter in the szFilter buffer
		}
	if (lpFileInfo->hFile > 0) // if a file is open, close it
		lpFileInfo->hFile = _lclose(lpFileInfo->hFile) ;
	if ((lpFileInfo->hFile = _lcreat(lpFileInfo->lpstrFile, 0)) == -1)
		{
		AObjUnlockData(oiFile, INFO_INDEX) ;
		ADBG_PRINT_E("File Could Not Be Created") ;
		theSystem->message1 = 2 ;
		return ;
		}
	_lclose(lpFileInfo->hFile) ;
	lpFileInfo->hFile = 0 ;

//	AEvtCallMethod( oiFile, oiFile, OTYPE_FILE, saveFile, 0 ) ;
	OBJECTID far* lpObjects = (OBJECTID far*)AObjLockData(oiFile, OBJECT_INDEX) ;
	long nObjects = AObjGetDataSize(oiFile, OBJECT_INDEX) / sizeof(OBJECTID) ; ;
	BOOL success = SaveFile(lpFileInfo->lpstrFile, lpObjects, nObjects ) ;
	AObjUnlockData(oiFile, OBJECT_INDEX) ;
	AObjUnlockData(oiFile, INFO_INDEX) ;

	if (success)
		theSystem->message1 = 1 ;
	else
		theSystem->message1 = 2 ;
	return ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  SaveAsFileFunc
// DESCRIPTION:  Brings up a dialog that allows the user to create a new file
//			and save the data in the objects to the new file.
// INPUT 1: a File Object
// FLOW 1: the file has been created and the data saved
// FLOW 2: the user canceled or an error occured
/////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export SaveAsFileFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	OBJECTID oiFile ; // ID to the File object
	FileInfo far* lpFileInfo ;
	OPENFILENAME ofn ;
	char customFilter[40] ;

	oiFile = AFuncGetTypedParameter(1, OTYPE_FILE) ; // get the object reference the File object
	lpFileInfo = (FileInfo far*)AObjLockData(oiFile, INFO_INDEX) ; // get the file information for this record

	// Set all structure members to zero
	_fmemset (&ofn, 0, sizeof(OPENFILENAME)) ;
	ofn.lStructSize = sizeof(OPENFILENAME) ; // Specifies the length of the structure in bytes
	ofn.hwndOwner = GetActiveWindow() ; // Identifies the window that owns the dialog box.
	ofn.lpstrFilter = lpstrFilter ; // filter strings
	ofn.nFilterIndex = lpFileInfo->nFilterIndex ; // selects the first filter in the szFilter buffer
	ofn.lpstrFile = lpFileInfo->lpstrFile ;
	ofn.nMaxFile = sizeof (lpFileInfo->lpstrFile) ; // should be at least 256
	ofn.lpstrTitle = "Save As" ;
	ofn.Flags =  OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR | OFN_HIDEREADONLY ;
	wsprintf(customFilter,"[*.%s] *.%s", lpFileInfo->lpstrDefExt, lpFileInfo->lpstrDefExt) ;
	customFilter[lstrlen(customFilter)+1] = '\0' ;
	customFilter[lstrlen(lpFileInfo->lpstrDefExt)+4] = '\0' ;
	ofn.lpstrCustomFilter = customFilter ;
	ofn.nMaxCustFilter = sizeof(customFilter) ;
	ofn.lpstrDefExt = lpFileInfo->lpstrDefExt ;

	if (GetSaveFileName (&ofn) == FALSE)
		{
		AObjUnlockData(oiFile, INFO_INDEX) ;
		ADBG_PRINT_I("User canceled the 'Save As' dialog box") ;
		theSystem->message1 = 2 ;
		return ;
		}
	lpFileInfo->nFilterIndex = (int)ofn.nFilterIndex  ; // saves the first filter in the szFilter buffer

	if (lpFileInfo->hFile > 0) // if a file is open, close it
		lpFileInfo->hFile = _lclose(lpFileInfo->hFile) ;
	if ((lpFileInfo->hFile = _lcreat(lpFileInfo->lpstrFile, 0)) == -1)
		{
		AObjUnlockData(oiFile, INFO_INDEX) ;
		ADBG_PRINT_E("File Could Not Be Created") ;
		theSystem->message1 = 2 ;
		return ;
		}
	_lclose(lpFileInfo->hFile) ;
	lpFileInfo->hFile = 0 ;

//	AEvtCallMethod( oiFile, oiFile, OTYPE_FILE, saveFile, 0 ) ;
	OBJECTID far* lpObjects = (OBJECTID far*)AObjLockData(oiFile, OBJECT_INDEX) ;
	long nObjects = AObjGetDataSize(oiFile, OBJECT_INDEX) / sizeof(OBJECTID) ; ;
	BOOL success = SaveFile(lpFileInfo->lpstrFile, lpObjects, nObjects ) ;
	AObjUnlockData(oiFile, OBJECT_INDEX) ;
	AObjUnlockData(oiFile, INFO_INDEX) ;

	if (success)
		theSystem->message1 = 1 ;
	else
		theSystem->message1 = 2 ;
	return ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  SaveFileFunc
// DESCRIPTION:  Saves the data in the objects to the file.
// INPUT 1: a File Object
// FLOW 1: the file has been opened and the data saved
// FLOW 2: the user canceled or an error occured
/////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export SaveFileFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	OBJECTID oiFile = AFuncGetTypedParameter(1, OTYPE_FILE) ; // get the object reference the File object

//	AEvtCallMethod( oiFile, oiFile, OTYPE_FILE, saveFile, 0 ) ;
	FileInfo far* lpFileInfo = (FileInfo far*)AObjLockData(oiFile, INFO_INDEX) ; // get the file information for this record
	OBJECTID far* lpObjects = (OBJECTID far*)AObjLockData(oiFile, OBJECT_INDEX) ;
	long nObjects = AObjGetDataSize(oiFile, OBJECT_INDEX) / sizeof(OBJECTID) ; ;
	BOOL success = SaveFile(lpFileInfo->lpstrFile, lpObjects, nObjects ) ;
	AObjUnlockData(oiFile, OBJECT_INDEX) ;
	AObjUnlockData(oiFile, INFO_INDEX) ;

	if (success)
		theSystem->message1 = 1 ;
	else
		theSystem->message1 = 2 ;
	return ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  LoadFileFunc
// DESCRIPTION:  Loads the file data into the application's objects.
// INPUT 1: a File Object
// INPUT 2: a Text Object = name of the file to be opened
// FLOW 1: the file has been opened and the data saved
// FLOW 2: the user canceled or an error occured
/////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export LoadFileFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	OBJECTID oiFile = AFuncGetTypedParameter(1, OTYPE_FILE) ; // get the object reference the File object
	FileInfo far* lpFileInfo = (FileInfo far*)AObjLockData(oiFile, INFO_INDEX) ; // get the file information for this record

	OBJECTID oiFileName = AFuncGetTypedParameter(2, OTYPE_TEXT) ; // get the object reference the File object
	OTextGetToBuffer(oiFileName, lpFileInfo->lpstrFile, sizeof(lpFileInfo->lpstrFile)) ;

	OBJECTID far* lpObjects = (OBJECTID far*)AObjLockData(oiFile, OBJECT_INDEX) ;
	long nObjects = AObjGetDataSize(oiFile, OBJECT_INDEX) / sizeof(OBJECTID) ; ;
	BOOL success = LoadFile(lpFileInfo->lpstrFile, lpObjects, nObjects) ;
	AObjUnlockData(oiFile, OBJECT_INDEX) ;
	AObjUnlockData(oiFile, INFO_INDEX) ;

	if (success)
		theSystem->message1 = 1 ;
	else
		theSystem->message1 = 2 ;
	return ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  OpenFileFunc
// DESCRIPTION:  Brings up a dialog that allows the user to find the file
// 			to open and loads the file data into the application's objects.
// INPUT 1: a File Object
// FLOW 1: the file has been opened and the data saved
// FLOW 2: the user canceled or an error occured
/////////////////////////////////////////////////////////////////////////////
void FAR PASCAL _export OpenFileFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	OBJECTID oiFile ; // ID to the File object
	FileInfo far* lpFileInfo ;
	OPENFILENAME ofn ;
	char customFilter[40] ;

	oiFile = AFuncGetTypedParameter(1, OTYPE_FILE) ; // get the object reference the File object
	lpFileInfo = (FileInfo far*)AObjLockData(oiFile, INFO_INDEX) ; // get the file information for this record

	// Set all structure members to zero
	_fmemset (&ofn, 0, sizeof(OPENFILENAME)) ;
	ofn.lStructSize = sizeof(OPENFILENAME) ; // Specifies the length of the structure in bytes
	ofn.hwndOwner = GetActiveWindow() ; // Identifies the window that owns the dialog box.
	ofn.lpstrFilter = lpstrFilter ; // filter strings
	ofn.nFilterIndex = lpFileInfo->nFilterIndex ; // selects the first filter in the szFilter buffer
	ofn.lpstrFile = lpFileInfo->lpstrFile ;
	ofn.nMaxFile = sizeof (lpFileInfo->lpstrFile) ; // should be at least 256
	ofn.lpstrTitle = "Open" ;
	ofn.Flags =  OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ;
	wsprintf(customFilter,"[*.%s] *.%s", lpFileInfo->lpstrDefExt, lpFileInfo->lpstrDefExt) ;
	customFilter[lstrlen(customFilter)+1] = '\0' ;
	customFilter[lstrlen(lpFileInfo->lpstrDefExt)+4] = '\0' ;
	ofn.lpstrCustomFilter = customFilter ;
	ofn.nMaxCustFilter = sizeof(customFilter) ;
	ofn.lpstrDefExt = lpFileInfo->lpstrDefExt ;

	if (GetOpenFileName (&ofn) == FALSE)
		{
		theSystem->message1 = 2 ;
		return ;
		}
	lpFileInfo->nFilterIndex = (int)ofn.nFilterIndex  ; // saves the first filter in the szFilter buffer

	if (lpFileInfo->hFile > 0) // if a file is open, close it
		lpFileInfo->hFile = _lclose(lpFileInfo->hFile) ;

	OBJECTID far* lpObjects = (OBJECTID far*)AObjLockData(oiFile, OBJECT_INDEX) ;
	long nObjects = AObjGetDataSize(oiFile, OBJECT_INDEX) / sizeof(OBJECTID) ; ;
	BOOL success = LoadFile(lpFileInfo->lpstrFile, lpObjects, nObjects) ;
	AObjUnlockData(oiFile, OBJECT_INDEX) ;
	AObjUnlockData(oiFile, INFO_INDEX) ;

	if (success)
		theSystem->message1 = 1 ;
	else
		theSystem->message1 = 2 ;
	return ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  LoadFile
// DESCRIPTION:  Reads the file and copies the data to the appropriate objects.
/////////////////////////////////////////////////////////////////////////////
BOOL LoadFile(char far* lpFullPathname, OBJECTID far* lpObjects, long nObjectsAnticipated )
	{
	char buffer[MAXPATH + 40] ;
	long nObjectsActual ; // actual number of objects stored in the File object
	long typeID ; // type ID of the object to be read from the File object
	int hFile ;  // handle of the open file

	// open file
	if (-1 == (hFile = _lopen (lpFullPathname, READ)))
		{
		wsprintf (buffer, "Cannot open file %s", lpFullPathname) ;
		ADBG_PRINT_E(buffer) ;
		return FALSE ;
		}

	// check the number of objects stored in this file
	long bytes = _lread (hFile, (LPSTR) &nObjectsActual, sizeof(nObjectsActual)) ;
	if (bytes != sizeof(nObjectsActual))   // read the first four bytes of the file
		{
		_lclose (hFile) ;
		wsprintf (buffer, "Cannot read the first four bytes of %s", lpFullPathname) ;
		ADBG_PRINT_E(buffer) ;
		return FALSE ;
		}

	if (nObjectsActual != nObjectsAnticipated)
		{
		_lclose (hFile) ;
		wsprintf (buffer, "Expected to read %ld objects, but %s contains %ld objects",
				nObjectsAnticipated, lpFullPathname, nObjectsActual) ;
		ADBG_PRINT_E(buffer) ;
		return FALSE ;
		}

	// read in the objects stored in the File object
	for (int i = 0; i < (int)nObjectsActual; i++)
		{
		_lread(hFile, (LPSTR) &typeID, sizeof(TYPEID));
		if (typeID != AObjGetType(lpObjects[i]))
		{
			_lclose(hFile);
			wsprintf(buffer, "Object type mismatch on object %d",i);
			ADBG_PRINT_E(buffer);
			return FALSE;
		}

//		OBJECTID oiNew = AObjCreate(typeID) ;  // create a new object

		AObjReadDatas(lpObjects[i], hFile);
//		AObjAssign(oiNew, lpObjects[i] ) ;
//		AObjDestroy(oiNew) ;
		}
	_lclose (hFile) ;
	return TRUE ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  SaveFile
// DESCRIPTION:  Save data from the selected objects to a file.
/////////////////////////////////////////////////////////////////////////////
BOOL SaveFile(char far* lpFullPathname, OBJECTID far* lpObjects, long nObjects )
	{
	int hFile ;  // handle of the open file
	char buffer[MAXPATH + 39] ;

	// open file
	if (-1 == (hFile = _lopen (lpFullPathname, WRITE)))
		{
		wsprintf (buffer, "Cannot open file %s", lpFullPathname) ;
		ADBG_PRINT_E(buffer) ;
		return FALSE ;
		}

	// write the number of objects to be stored in this file
	long bytes = _lwrite (hFile, (LPSTR) &nObjects, sizeof(nObjects)) ;
	if (bytes != sizeof(OBJECTID))    // write the first four bytes of the file
		{
		_lclose (hFile) ;
		wsprintf (buffer, "Cannot write the first four bytes of %s", lpFullPathname) ;
		ADBG_PRINT_E(buffer) ;
		return FALSE ;
		}

	// write the objects to the File object
	for (int i = 0; i < (int)nObjects; i++)
		{
		long typeID = AObjGetType(lpObjects[i]);
		OBJECTID oiSave = lpObjects[i] ;

		// write object type id
		_lwrite(hFile, (LPSTR) &typeID, sizeof(typeID));

		// write data from the data block to the file
		AObjWriteDatas(oiSave, hFile);
		}
	_lclose (hFile) ; // close the file
	return TRUE ;
	}
