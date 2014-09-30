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
//		LibMain/*!!PORT!! Replace with DllEntryPoint */: saves the instance ID of the DLL
//		WEP/*!!PORT!! Replace with DllEntryPoint */: called when a DLL is removed from memory
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <commdlg.h>
#include <mem.h>
#include <dir.h>
#include <a_alm.h>
#include <o_text.h>
#include <string.h>
#include "fileobj.h"

BOOL LoadFile(char * lpFullPathname, AOBJECTID * lpObjects, long nObjectsAnticipated ) ;
BOOL SaveFile(char * lpFullPathname, AOBJECTID * lpObjects, long nObjects ) ;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  FileNameFunc
// DESCRIPTION:  Parses the path name and file name of a Serius File object.
// INPUT 1: a File Object
// OUTPUT 1: a Text Object = file name
// OUTPUT 2: a Text Object = full path name including the file name
// FLOW 1: this is the only option currently available
/////////////////////////////////////////////////////////////////////////////
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  FileNameFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	AOBJECTID oiText ; // Handle to the Text object
	AOBJECTID oiFile ; // Handle to the File object
	FileInfo * lpFileInfo ;
	char drive[MAXDRIVE];
	char dir[MAXDIR];
	char file[MAXFILE];
	char ext[MAXEXT];
	char buffer[MAXDRIVE + MAXDIR + MAXFILE + MAXEXT] ;

	oiFile = AFuncGetTypedParameter(1, OTYPE_FILE) ;  // get the ID for the File object
	lpFileInfo = (FileInfo *)AObjLockData(oiFile, INFO_INDEX) ; // get the file information for this record
	if (lpFileInfo == NULL)
		return ;

	fnsplit(lpFileInfo->lpstrFile, drive, dir, file, ext);

  if (!lstrcmp(file, "NONAME"))	  // File doesn't exit
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
void CALLBACK NewFileFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	AOBJECTID oiFile ; // ID of the File object
	AOBJECTID oiFileName ; // ID of the Text object
	FileInfo * lpFileInfo ;
	OPENFILENAME ofn ;
	char oldFileName[256] ;
	char customFilter[40] ; //

	oiFile = AFuncGetTypedParameter(1, OTYPE_FILE) ; // get the ID for the File object
	lpFileInfo = (FileInfo *)AObjLockData(oiFile, INFO_INDEX) ; // get the file information for this record

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
	if (lpFileInfo->hFile!=INVALID_HANDLE_VALUE) // if a file is open, close it
		CloseHandle(lpFileInfo->hFile) ;

	lpFileInfo->hFile = CreateFile(
																lpFileInfo->lpstrFile,
																GENERIC_WRITE, 
																0,
																NULL, 
																CREATE_ALWAYS, 
																FILE_ATTRIBUTE_NORMAL,
																NULL);	
	if (lpFileInfo->hFile==INVALID_HANDLE_VALUE) 
		{
		AObjUnlockData(oiFile, INFO_INDEX) ;
		ADBG_PRINT_E("File Could Not Be Created") ;
		theSystem->message1 = 2 ;
		return ;
		}
	CloseHandle(lpFileInfo->hFile) ;
	lpFileInfo->hFile = INVALID_HANDLE_VALUE ;

	AOBJECTID * lpObjects = (AOBJECTID *)AObjLockData(oiFile, OBJECT_INDEX) ;
	long nObjects = AObjGetDataSize(oiFile, OBJECT_INDEX) / sizeof(AOBJECTID) ; ;
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
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  SaveAsFileFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	AOBJECTID oiFile ; // ID to the File object
	FileInfo * lpFileInfo ;
	OPENFILENAME ofn ;
	char customFilter[40] ;

	oiFile = AFuncGetTypedParameter(1, OTYPE_FILE) ; // get the object reference the File object
	lpFileInfo = (FileInfo *)AObjLockData(oiFile, INFO_INDEX) ; // get the file information for this record

	// Set all structure membe148rs to zero
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

	if (lpFileInfo->hFile !=INVALID_HANDLE_VALUE) // if a file is open, close it
		CloseHandle(lpFileInfo->hFile) ;
	lpFileInfo->hFile = CreateFile(
																lpFileInfo->lpstrFile,
																GENERIC_WRITE, 
																0,
																NULL, 
																CREATE_ALWAYS, 
																FILE_ATTRIBUTE_NORMAL,
																NULL);	
	if (lpFileInfo->hFile==INVALID_HANDLE_VALUE)
		{
		AObjUnlockData(oiFile, INFO_INDEX) ;
		ADBG_PRINT_E("File Could Not Be Created") ;
		theSystem->message1 = 2 ;
		return ;
		}
	CloseHandle(lpFileInfo->hFile) ;
	lpFileInfo->hFile = 0 ;

//	AEvtCallMethod( oiFile, oiFile, OTYPE_FILE, saveFile, 0 ) ;
	AOBJECTID * lpObjects = (AOBJECTID *)AObjLockData(oiFile, OBJECT_INDEX) ;
	long nObjects = AObjGetDataSize(oiFile, OBJECT_INDEX) / sizeof(AOBJECTID) ; ;
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
void CALLBACK SaveFileFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	AOBJECTID oiFile = AFuncGetTypedParameter(1, OTYPE_FILE) ; // get the object reference the File object

	FileInfo * lpFileInfo = (FileInfo *)AObjLockData(oiFile, INFO_INDEX) ; // get the file information for this record
	AOBJECTID * lpObjects = (AOBJECTID *)AObjLockData(oiFile, OBJECT_INDEX) ;
	long nObjects = AObjGetDataSize(oiFile, OBJECT_INDEX) / sizeof(AOBJECTID) ; ;
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
void CALLBACK LoadFileFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	AOBJECTID oiFile = AFuncGetTypedParameter(1, OTYPE_FILE) ; // get the object reference the File object
	FileInfo * lpFileInfo = (FileInfo *)AObjLockData(oiFile, INFO_INDEX) ; // get the file information for this record

	AOBJECTID oiFileName = AFuncGetTypedParameter(2, OTYPE_TEXT) ; // get the object reference the File object
	OTextGetToBuffer(oiFileName, lpFileInfo->lpstrFile, sizeof(lpFileInfo->lpstrFile)) ;

	AOBJECTID * lpObjects = (AOBJECTID *)AObjLockData(oiFile, OBJECT_INDEX) ;
	long nObjects = AObjGetDataSize(oiFile, OBJECT_INDEX) / sizeof(AOBJECTID) ; ;
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
void CALLBACK OpenFileFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	AOBJECTID oiFile ; // ID to the File object
	FileInfo * lpFileInfo ;
	OPENFILENAME ofn ;
	char customFilter[40] ;

	oiFile = AFuncGetTypedParameter(1, OTYPE_FILE) ; // get the object reference the File object
	lpFileInfo = (FileInfo *)AObjLockData(oiFile, INFO_INDEX) ; // get the file information for this record

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

	//if (lpFileInfo->hFile!=INVALID_HANDLE_VALUE) // if a file is open, close it
	//	CloseHandle(lpFileInfo->hFile) ;

	AOBJECTID * lpObjects = (AOBJECTID *)AObjLockData(oiFile, OBJECT_INDEX) ;
	long nObjects = AObjGetDataSize(oiFile, OBJECT_INDEX) / sizeof(AOBJECTID) ; ;
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
BOOL LoadFile(char * lpFullPathname, AOBJECTID * lpObjects, long nObjectsAnticipated )
	{
	char   buffer[MAX_PATH + 40] ;
	long   nObjectsActual = 0L ; // actual number of objects stored in the File object
	long   typeID ; // type ID of the object to be read from the File object
	HANDLE hFile ;  // handle of the open file
	DWORD  bytes ;

	// open file
	hFile = CreateFile(
										lpFullPathname,
										GENERIC_READ,
										0,
										NULL, 
										OPEN_EXISTING, 
										FILE_ATTRIBUTE_NORMAL,
										NULL);	
	if (hFile==INVALID_HANDLE_VALUE)
		{
		wsprintf (buffer, "Cannot open file %s", lpFullPathname) ;
		ADBG_PRINT_E(buffer) ;
		return FALSE ;
		}

	// check the number of objects stored in this file
	ReadFile (hFile, (LPSTR) &nObjectsActual, sizeof(nObjectsActual), &bytes, NULL) ;
	if (bytes != sizeof(nObjectsActual))   // read the first four bytes of the file
		{
		CloseHandle (hFile) ;
		wsprintf (buffer, "Cannot read the first four bytes of %s", lpFullPathname) ;
		ADBG_PRINT_E(buffer) ;
		return FALSE ;
		}

	if (nObjectsActual != nObjectsAnticipated)
		{
		CloseHandle (hFile) ;
		wsprintf (buffer, "Expected to read %ld objects, but %s contains %ld objects",
				nObjectsAnticipated, lpFullPathname, nObjectsActual) ;
		ADBG_PRINT_E(buffer) ;
		return FALSE ;
		}

	// read in the objects stored in the File object
	for (int i = 0; i < (int)nObjectsActual; i++)
		{
		ReadFile(hFile, (LPSTR) &typeID, sizeof(ATYPEID), &bytes, NULL);
		if (typeID != AObjGetType(lpObjects[i]))
		{
			CloseHandle(hFile);
			wsprintf(buffer, "Object type mismatch on object %d",i);
			ADBG_PRINT_E(buffer);
			return FALSE;
		}

//		AOBJECTID oiNew = AObjCreate(typeID) ;  // create a new object

		AObjReadDatas(lpObjects[i], hFile);
//		AObjAssign(oiNew, lpObjects[i] ) ;
//		AObjDestroy(oiNew) ;
		}
	CloseHandle (hFile) ;
	return TRUE ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  SaveFile
// DESCRIPTION:  Save data from the selected objects to a file.
/////////////////////////////////////////////////////////////////////////////
BOOL SaveFile(char * lpFullPathname, AOBJECTID * lpObjects, long nObjects )
	{
	HANDLE hFile ;  // handle of the open file
	char   buffer[MAX_PATH + 39] ;
	DWORD  bytes ;
	// open file
	hFile = CreateFile(
										lpFullPathname,
										GENERIC_WRITE, 
										0,
										NULL, 
										CREATE_ALWAYS, 
										FILE_ATTRIBUTE_NORMAL,
										NULL);	
	if (hFile==INVALID_HANDLE_VALUE)
		{
		wsprintf (buffer, "Cannot open file %s", lpFullPathname) ;
		ADBG_PRINT_E(buffer) ;
		return FALSE ;
		}

	// write the number of objects to be stored in this file
	WriteFile (hFile, (LPSTR) &nObjects, sizeof(nObjects), &bytes, NULL) ;
	if (bytes != sizeof(AOBJECTID))    // write the first four bytes of the file
		{
		CloseHandle (hFile) ;
		wsprintf (buffer, "Cannot write the first four bytes of %s", lpFullPathname) ;
		ADBG_PRINT_E(buffer) ;
		return FALSE ;
		}

	// write the objects to the File object
	for (int i = 0; i < (int)nObjects; i++)
		{
		long typeID = AObjGetType(lpObjects[i]);
		AOBJECTID oiSave = lpObjects[i] ;

		// write object type id
		WriteFile(hFile, (LPSTR) &typeID, sizeof(typeID), &bytes, NULL);

		// write data from the data block to the file
		AObjWriteDatas(oiSave, hFile);
		}
	CloseHandle (hFile) ; // close the file
	return TRUE ;
	}
