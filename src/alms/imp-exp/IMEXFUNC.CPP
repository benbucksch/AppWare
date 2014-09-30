/////////////////////////////////////////////////////////////////////////////
// File Name:  ImExFunc.CPP
// 		Copyright 1989-92 Serius Corporation
// Author:  Paul Ruben
// Date:
//
// Description.:
//	Contains all Serius functions associated with the Serius Import/Export object:
//		ChooseFileFunc
//		ConfigureExportFunc
//		ConfigureImportFunc
//		CreateExportFunc
//		ExportRecordFunc
//		ImportRecordFunc
//		StartImportExportFunc
//		StopImportExportFunc
//
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <memory.h>
#include <dos.h>
#include <commdlg.h>
#include <a_alm.h>
#include <o_text.h>
#include <o_time.h>
#include <o_number.h>
#include <o_date.h>
#include "imexobj.h"										 
#include "imexdlg.h"

#define MXSTRSIZE	256

extern "C" void CALLBACK /*!!PORT!! was _far_ _pascal_*/ ShutdownMeth( pAEvtInfo, pAObjMessage) ;
int LoadBuffer (RuntimeInfo * lpRTI) ;
//inline DWORD filelength (HANDLE hFile){return (SetFilePointer (hFile, 0L, NULL, FILE_END));}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  CheckWindowsQueue
// DESCRIPTION:  Checks for dialog box messages in the Windows message queue
/////////////////////////////////////////////////////////////////////////////
void CheckWindowsQueue(HWND StatusDlg)
	{
	MSG msg ;

	if (StatusDlg == 0)
		return ;
	while(PeekMessage(&msg, StatusDlg, NULL, NULL, PM_NOYIELD | PM_REMOVE))
		IsDialogMessage(StatusDlg, &msg) ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  PrepareForFirstImport
// DESCRIPTION:  This function is called during the first 'Import Record' function.
/////////////////////////////////////////////////////////////////////////////
void PrepareForFirstImport(AOBJECTID oiImportExport, ImpExpInfo * lpImportInfo)
	{
	RuntimeInfo * lpRTInfo ;

	lpRTInfo = lpImportInfo->lpRuntimeInfo ;
	lpImportInfo->Status = O_IMPORTING ; // dedicate this object to importing until the stop function is called

	// Create the Status dialog box.
	lpRTInfo->StatusDlg = 0 ;
	if (lpImportInfo->ImportConfig.ShowStatus)
		lpRTInfo->StatusDlg = CreateDialogParam (hInstance, "Status", GetActiveWindow(), (DLGPROC)StatusDlgProc, oiImportExport) ;

	// allocate memory for the import buffer
	lpRTInfo->lpBuffer = new char[BUFFERSIZE] ;
	if (lpRTInfo->lpBuffer == NULL)
		{
		ADBG_PRINT_E("The new operator failed when allocating memory for the import buffer.") ;
		lpRTInfo->wBytes = -1 ;
		return ;
		}
	lpRTInfo->lpBuffer[0] = '\0' ;

	if (LoadBuffer (lpRTInfo) < 0)
		return  ;
	lpRTInfo->lBlockBytes = 0 ;  // reset lBlockBytes to zero
	lpRTInfo->lRecords = 0 ; // reset record count to zero

	// If the first line lists field headings, skip the first line
	if (lpImportInfo->ImportConfig.Heading)
		{
		char currentChar = lpRTInfo->lpBuffer[lpRTInfo->wMark++] ;
		BOOL EndOfLine = (currentChar == VK_RETURN || lpRTInfo->EndOfFile) ;
		while (!EndOfLine)
			{
			currentChar = lpRTInfo->lpBuffer[lpRTInfo->wMark++] ;
			if (lpRTInfo->wMark == lpRTInfo->wBytes)
				if (LoadBuffer (lpRTInfo) < 0)
					return ;
			EndOfLine = (currentChar == VK_RETURN || lpRTInfo->EndOfFile) ;
			}
		if (lpRTInfo->lpBuffer[lpRTInfo->wMark] == '\n')
			{
			lpRTInfo->wMark++ ; // skip the new line '\n' character
			if (lpRTInfo->wMark == lpRTInfo->wBytes)
				if (LoadBuffer (lpRTInfo) < 0)
					return ;
			}
		}
	}

////////////////////////////////////////////////////////////////////////////
// FUNCTION	:  ReadImportRecord
// DESCRIPTION	:  Reads a line from a text file and tranfers the text to
//		   Field objects.
////////////////////////////////////////////////////////////////////////////
void ReadImportRecord
( AOBJECTID	oiImportExport,
  RuntimeInfo	 * lpRTInfo,
  char		delimiter
)
{ AOBJECTID	 *lpImportObjects;
  long	    	strFieldSize;		// string field size
  AMEMBLOCKID	strFieldMB;		// string field memory block
  char		   *strField;		// string field
  char		   *strFieldPtr;	// string field pointer		

  LPSTR lpBuffer = lpRTInfo->lpBuffer;
  char currentChar = lpBuffer[lpRTInfo->wMark];
  BOOL EndOfLine = (currentChar == VK_RETURN);
  if (EndOfLine)
    lpRTInfo->wMark++ ;
  int nObjects = (int) (AObjGetDataSize(oiImportExport, IMP_OBJ_INDEX) /
                        sizeof(AOBJECTID));
  lpImportObjects = (AOBJECTID *) AObjLockData(oiImportExport,
                                                 IMP_OBJ_INDEX);

  /* reads the record into the field objects */
  for (int i = 0; i < nObjects; i++)
  { int j = 0;
    BOOL EndOfField = FALSE;
    strFieldMB = AMemAllocate(MXSTRSIZE);
    strField = (char *) AMemLock(strFieldMB);
    strFieldPtr = strField;
    while (!EndOfField && !EndOfLine)
    { currentChar = lpBuffer[lpRTInfo->wMark++];
      if (lpRTInfo->wMark == lpRTInfo->wBytes)
        if (LoadBuffer (lpRTInfo) < 0)
	{ AObjUnlockData(oiImportExport, IMP_OBJ_INDEX);
	  return;
	} // if
      EndOfLine = currentChar == VK_RETURN;
      EndOfField = (currentChar == delimiter || EndOfLine);
      if (!EndOfField)
	if (j < MXSTRSIZE)
        { *strFieldPtr++ = currentChar;
  	  j++;
        } // if
        else
        { AMemUnlock(strFieldMB);
          strFieldSize = AMemGetSize(strFieldMB);
	  AMemSetSize(strFieldMB, strFieldSize + MXSTRSIZE);
	  strField = (char *) AMemLock(strFieldMB);
	  strFieldPtr = (char *) strField + strFieldSize;
          *strFieldPtr++ = currentChar;
	  j = 1; 
        } // else
      EndOfLine = (currentChar == VK_RETURN || lpRTInfo->EndOfFile);
    } // while
    *strFieldPtr = '\0';		// 0 terminated string

    if (lpImportObjects[i])
    { switch ((ATYPEID) AObjGetType(lpImportObjects[i]))
      { case OTYPE_TEXT:
	  OTextSetValueFromBuffer(lpImportObjects[i], strField);
          break;
        case OTYPE_NUMBER:
	  ONmbrConvertFromString(lpImportObjects[i], strField, FALSE);
	  break;
        case OTYPE_DATE:
	  ODateConvertFromString(lpImportObjects[i], strField);
          break;
        case OTYPE_TIME:
	  OTimeConvertFromString(lpImportObjects[i], strField);
	  break;
        default:
	  AObjConvertFromText(lpImportObjects[i], strFieldMB);
          break;
      } // switch
    } // if
    AMemUnlock(strFieldMB);
    AMemFree(strFieldMB);
  } // for

  if (nObjects > 0)
    AObjUnlockData(oiImportExport, IMP_OBJ_INDEX) ;

  /* if there are more fields than objects, scan through the remaining
     fields until you reach the end of line
  */
  while (!EndOfLine)
  { currentChar = lpBuffer[lpRTInfo->wMark++];
    if (lpRTInfo->wMark == lpRTInfo->wBytes)
      if (LoadBuffer (lpRTInfo) < 0)
        return;
    EndOfLine = (currentChar == VK_RETURN || lpRTInfo->EndOfFile);
  } // while

  /* skips new line character if there is one following the end of line */
  if (lpBuffer[lpRTInfo->wMark] == '\n')
  { lpRTInfo->wMark++ ;			// skip the new line '\n' character
    if (lpRTInfo->wMark == lpRTInfo->wBytes)
      if (LoadBuffer(lpRTInfo) < 0)
	return;
  } // if
} // ReadImportRecord

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  ImportRecordFunc
// DESCRIPTION:  Reads a record from a text file and writes the
//		text data to the chosen objects.
// INPUT 1: an Import-Export Object
// OUTPUT 1: a Number Object (current position)
// FLOW 1: after exporting the record
// FLOW 2: if no more records are available or the user cancels
// FLOW 3: if a file error occurs
/////////////////////////////////////////////////////////////////////////////
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  ImportRecordFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	RuntimeInfo  *lpRTInfo ; // pointer to Runtime Information structure
	ImpExpInfo  *lpImpExpInfo ; // general information related to the import/export object

	// Get the Import/Export object's identifier
	AOBJECTID oiImportExport = AFuncGetTypedParameter(1, OTYPE_IMPORTEXPORT) ;
	lpImpExpInfo = (ImpExpInfo  *) AObjLockData(oiImportExport, INFO_INDEX) ;
	lpRTInfo = lpImpExpInfo->lpRuntimeInfo ;
	if (lpImpExpInfo->Status == O_EXPORTING)
		{
		AObjUnlockData(oiImportExport, INFO_INDEX) ;
		ADBG_PRINT_W("Cannot import while exporting.") ;
		AFuncReportResult(theSystem, 3, "") ;
		return ;
		}
	if (lpImpExpInfo->Busy == FALSE)
		{
		AObjUnlockData(oiImportExport, INFO_INDEX) ;
		ADBG_PRINT_W("Must execute 'Start Import-Export' function before importing records.") ;
		AFuncReportResult(theSystem, 3, "") ;
		return ;
		}

	// Is this the first record being imported for this object?
	if (lpImpExpInfo->Status == O_UNCOMMITTED)
		PrepareForFirstImport(oiImportExport, lpImpExpInfo) ;

	if (lpRTInfo->wBytes < 0)
		{  // Error reading file
		AObjUnlockData(oiImportExport, INFO_INDEX) ;
		AFuncReportResult(theSystem, 3, "") ;
		return ;
		}

	CheckWindowsQueue(lpRTInfo->StatusDlg) ; // Check the Cancel button in the Status dialog box
	if (lpRTInfo->Abort || lpRTInfo->EndOfFile) // if the user issued a cancel or we have reached the EOF
		{
		AObjUnlockData(oiImportExport, INFO_INDEX) ;
		AFuncReportResult(theSystem, 2, "") ;
		return ;
		}

	ReadImportRecord(oiImportExport, lpRTInfo, lpImpExpInfo->ImportConfig.Delimiter) ;
	if (lpRTInfo->wBytes < 0)
		{ // Error reading file
		AObjUnlockData(oiImportExport, INFO_INDEX) ;
		AFuncReportResult(theSystem, 3, "") ;
		return ;
		}

	AOBJECTID oiNumber = AObjCreate(OTYPE_NUMBER) ;
	if (oiNumber != 0)
		{
		long status = lpImpExpInfo->lpRuntimeInfo->wMark + lpImpExpInfo->lpRuntimeInfo->lBlockBytes ;
		ONmbrSetInteger(oiNumber, status, FALSE) ;
		AFuncSetParameter(1, oiNumber) ;
		}

	// Update the Status dialog box
	if (lpRTInfo->StatusDlg)
		{
		lpRTInfo->lRecords++ ;
		SendMessage(lpRTInfo->StatusDlg, WM_USER, 0, lpRTInfo->lRecords) ;
		}

	AObjUnlockData(oiImportExport, INFO_INDEX) ;
	AFuncReportResult (theSystem, 1, "") ;
	}

////////////////////////////////////////////////////////////////////////////
// FUNCTION	:  LoadBuffer
// DESCRIPTION	:  Reads a block of data from a text file into the buffer
////////////////////////////////////////////////////////////////////////////
int LoadBuffer
( RuntimeInfo	 *lpRTInfo
)
{
  lpRTInfo->lBlockBytes += lpRTInfo->wBytes ;
  lpRTInfo->wMark = 0 ;
  //lpRTInfo->wBytes = ReadFile(lpRTInfo->hFile, lpRTInfo->lpBuffer,
  //                          BUFFERSIZE);
	BOOL b = ReadFile(
					lpRTInfo->hFile, 
					lpRTInfo->lpBuffer,
					BUFFERSIZE,
					(DWORD*)(&lpRTInfo->wBytes),
					NULL);  
	if (lpRTInfo->wBytes == 0)
    lpRTInfo->EndOfFile = TRUE;
  return lpRTInfo->wBytes;
} // LoadBuffer

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  WriteExportHeader
// DESCRIPTION:  Writes the field names to the first record
/////////////////////////////////////////////////////////////////////////////
void WriteExportHeader(AOBJECTID oiImportExport, RuntimeInfo  *lpRTInfo, char del)
	{
	AOBJECTID  *lpExportObjects ; // indexes used to identify objects for export
	char  *lpszNames ; // zero terminated strings containing field names for objects
	char delimiter[2] = {del, 0};
	char * lpBuffer = lpRTInfo->lpBuffer ;
	HANDLE hFile = lpRTInfo->hFile ;		
	DWORD  BytesWritten;

	int nObjects = (int)AObjGetDataSize(oiImportExport, EXP_OBJ_INDEX) / sizeof(AOBJECTID) ;
	lpExportObjects = (AOBJECTID *) AObjLockData (oiImportExport, EXP_OBJ_INDEX) ;
	lpszNames = (char *) AObjLockData (oiImportExport, NAMES_INDEX) ;

	for (int i = 0; i < nObjects; i++)
		{
		int k = (int)GetFieldIndex(oiImportExport, lpExportObjects[i]) ; // index of next field name
		if (k >= 0)
			{
			int j = 0 ;
			for (int m = 0; m < k; m++)
				j += lstrlen(&lpszNames[j]) + 1 ;
			lstrcat (lpBuffer, &lpszNames[j]) ;
			}
		if (i < (nObjects - 1))
			lstrcat (lpBuffer, delimiter) ;
		else
			lstrcat (lpBuffer, "\r\n") ; // append a return
		}
	if (nObjects > 0)
		{
		AObjUnlockData (oiImportExport, NAMES_INDEX) ;
		AObjUnlockData (oiImportExport, EXP_OBJ_INDEX) ;
		}
	WriteFile(hFile, lpBuffer, lstrlen(lpBuffer), &BytesWritten, NULL) ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  PrepareForFirstExport
// DESCRIPTION:  This function is called during the first 'Export Record' function.
/////////////////////////////////////////////////////////////////////////////
BOOL PrepareForFirstExport(AOBJECTID oiImportExport, ImpExpInfo * lpExportInfo)
	{
	RuntimeInfo * lpRTInfo ;

	lpRTInfo = lpExportInfo->lpRuntimeInfo ;
	lpExportInfo->Status = O_EXPORTING ; // dedicate this object to exporting until the stop function is called

	// Create the Status dialog box.
	if (lpExportInfo->ExportConfig.ShowStatus)
		lpRTInfo->StatusDlg = CreateDialogParam (hInstance, "Status", GetActiveWindow(), (DLGPROC)StatusDlgProc, oiImportExport) ;
	else
		lpRTInfo->StatusDlg = 0 ;

	// allocate memory for the export buffer
	lpRTInfo->lpBuffer = new char[BUFFERSIZE] ;
	if (lpRTInfo->lpBuffer == NULL)
		{
		ADBG_PRINT_E("The new operator failed when allocating memory for the export buffer.") ;
		return FALSE ;
		}
	lpRTInfo->lpBuffer[0] = '\0' ;

	long fileLength = GetFileSize (lpRTInfo->hFile, NULL) ; //filelength(lpRTInfo->hFile) ;
	if (fileLength == 0)
		{
		if (lpExportInfo->ExportConfig.Heading)
			WriteExportHeader(oiImportExport, lpRTInfo, lpExportInfo->ExportConfig.Delimiter) ;
		}
	else if (fileLength > 0)
		SetFilePointer (lpRTInfo->hFile, 0L, NULL, FILE_END) ;
		 //SetFilePointer(lpRTInfo->hFile, 0L, 2) ;
	else
		{
		ADBG_PRINT_E("File error detected during export.") ;
		return FALSE ;
		}
	return TRUE ;
	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  WriteExportRecord
// DESCRIPTION:  This function writes an export record to file.
/////////////////////////////////////////////////////////////////////////////
BOOL WriteExportRecord(AOBJECTID oiImportExport, RuntimeInfo * lpRTInfo, char del)
	{
	AOBJECTID * lpExportObjects ;
	LPSTR recordBuffer = lpRTInfo->lpBuffer ;
	char delimiter[2] = {del, 0};
	AMEMBLOCKID memBlockID = AMemAllocate(0) ;
	int nObjects = (int)AObjGetDataSize(oiImportExport, EXP_OBJ_INDEX) / sizeof(AOBJECTID) ;
	lpExportObjects = (AOBJECTID *) AObjLockData(oiImportExport, EXP_OBJ_INDEX) ;
	recordBuffer[0] = 0 ;
	int recordSize = 1 ;
	DWORD BytesWritten ;

	for (int i = 0; i < nObjects; i++)
		{
		int ObjectIndex = (int)GetFieldIndex(oiImportExport, lpExportObjects[i]) ; // index of next field name
		if (ObjectIndex >= 0)
			{
			long objectType = AObjGetType (lpExportObjects[i]);
			if (objectType == OTYPE_TEXT)
				OTextGetToMemblock(lpExportObjects[i], memBlockID);
/*			else if	(objectType == OTYPE_NUMBER)
				{
				ONmbrGetToMemblock(lpExportObjects[i],  memBlockID);
				}
			else if	(objectType == OTYPE_DATE)
				{
				ODateGetToMemBlock(lpExportObjects[i],  memBlockID, NULL, 0);
				}
			else if	(objectType == OTYPE_TIME)
				OTimeGetToMemBlock(lpExportObjects[i],  memBlockID, NULL, 0); */
			else  // for menus, buttons, etc.
				AObjConvertToText(lpExportObjects[i], memBlockID) ;

			LPSTR fieldBuffer = (LPSTR)AMemLock(memBlockID) ;
			if (fieldBuffer != NULL)
				{
				if ((recordSize += lstrlen(fieldBuffer)) < BUFFERSIZE - 2)
					lstrcat (recordBuffer, fieldBuffer) ;
				else
					{
					ADBG_PRINT_W("Export record length exceeds buffer size") ;
					lstrcat (recordBuffer, "\r\n") ; // append a return
					return FALSE ;
					}
				}
			AMemUnlock(memBlockID) ;
			}
		if (i < (nObjects - 1))
			lstrcat (recordBuffer, delimiter) ;
		else
			lstrcat (recordBuffer, "\r\n") ; // append a return
		}
	if (nObjects > 0)
		AObjUnlockData(oiImportExport, EXP_OBJ_INDEX) ;
	AMemFree(memBlockID) ;

	// write the record to the text file
	if (!WriteFile(lpRTInfo->hFile, recordBuffer, lstrlen(recordBuffer), &BytesWritten, NULL))
		{
		ADBG_PRINT_E("Error writing to export file") ;
		return FALSE ;
		}
	return TRUE ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  ExportRecordFunc
// DESCRIPTION:  Writes text data from the chosen objects to a line
//		in the selected text file
// INPUT 1: an Import-Export Object
// INPUT 2 (optional): a Number Object (current status)
// FLOW 1: after exporting the record
// FLOW 2: if the user cancels
// FLOW 3: if a file error occurs
/////////////////////////////////////////////////////////////////////////////
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  ExportRecordFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	RuntimeInfo  *lpRTInfo ; // pointer to Runtime Information structure
	ImpExpInfo  *lpExportInfo ; // general information related to the import/export object

	// Get the Import/Export object's identifier
	AOBJECTID oiImportExport = AFuncGetTypedParameter(1, OTYPE_IMPORTEXPORT) ;  // get the handle for the Import Export object
	lpExportInfo = (ImpExpInfo  *) AObjLockData(oiImportExport, INFO_INDEX) ;
	lpRTInfo = lpExportInfo->lpRuntimeInfo ;
	if (lpExportInfo->Status == O_IMPORTING)
		{
		AObjUnlockData(oiImportExport, INFO_INDEX) ;
		ADBG_PRINT_W("Cannot export while importing.") ;
		theSystem->message1 = 3 ; // signal flow index for errors
		return ;
		}
	if (lpExportInfo->Busy == FALSE)
		{
		AObjUnlockData(oiImportExport, INFO_INDEX) ;
		ADBG_PRINT_W("Must execute 'Start Import-Export' function before exporting records.") ;
		theSystem->message1 = 3 ; // signal flow index for errors
		return ;
		}
	// Is this the first record being exported for this object?
	if (lpExportInfo->Status == O_UNCOMMITTED)
		if (PrepareForFirstExport(oiImportExport, lpExportInfo) == FALSE)
			{
			AObjUnlockData(oiImportExport, INFO_INDEX) ;
			theSystem->message1 = 3 ; // signal flow index for file errors
			return ;
			}
	CheckWindowsQueue(lpRTInfo->StatusDlg) ; // Check the Cancel button in the Status dialog box
	if (lpRTInfo->Abort) // if the user issued a cancel from the status box
		{
		AObjUnlockData(oiImportExport, INFO_INDEX) ;
		theSystem->message1 = 2 ; // signal flow for cancel
		return ;
		}
	// write the text for each object of a records
	if (WriteExportRecord(oiImportExport, lpRTInfo, lpExportInfo->ExportConfig.Delimiter) == FALSE)
		{
		AObjUnlockData(oiImportExport, INFO_INDEX) ;
		theSystem->message1 = 3 ; // signal flow index for file errors
		return ;
		}
	if (lpRTInfo->StatusDlg)
		{  // get the handle for the Import Export object
		AOBJECTID oiCurrentStatus = AFuncGetTypedParameter(2, OTYPE_NUMBER) ;
		if(oiCurrentStatus)
			lpRTInfo->currentStatus = ONmbrGetInteger(oiCurrentStatus) ;

		lpRTInfo->lRecords++ ;
		SendMessage(lpRTInfo->StatusDlg, WM_USER, 0, lpRTInfo->lRecords) ;
		}
	AObjUnlockData(oiImportExport, INFO_INDEX) ;
	theSystem->message1 = 1 ; // record exported successfully
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  CreateExportFunc
// DESCRIPTION:  Allows you or the user of your application to create
//		a text file for exporting data.
// INPUT 1: an Import-Export Object
// INPUT 2 (optional): a Text Object (path name of the file to create)
// INPUT 3 (optional): a Text Object (default name of the file to be create)
// OUTPUT 1: a Text Object (path name of the file created)
// FLOW 1: the file has been created
// FLOW 2: the user canceled or an error occured
/////////////////////////////////////////////////////////////////////////////
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  CreateExportFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	ImpExpInfo  *lpImpExpInfo ;
	OPENFILENAME ofn ;
	HANDLE hFile ; // MS-DOS file handle

	AOBJECTID oiImportExport = AFuncGetTypedParameter(1, OTYPE_IMPORTEXPORT) ;  // get the handle for the Import Export object
	lpImpExpInfo = (ImpExpInfo  *)AObjLockData(oiImportExport, INFO_INDEX) ;

	AOBJECTID oiDefaultName = AFuncGetTypedParameter(3, OTYPE_TEXT) ;
	if(oiDefaultName != 0)  // get the object ID for the File object
		{
		OTextGetToBuffer(oiDefaultName, lpImpExpInfo->lpstrFile, sizeof(lpImpExpInfo->lpstrFile)-1) ;
		}

	AOBJECTID oiFileName = AFuncGetTypedParameter(2, OTYPE_TEXT) ;
	if(oiFileName)  // get the object ID for the File object
		OTextGetToBuffer(oiFileName, lpImpExpInfo->lpstrFile, sizeof(lpImpExpInfo->lpstrFile)-1) ;
	else
		{
		// Set all structure members to zero
		memset (&ofn, 0, sizeof(OPENFILENAME)) ;
		ofn.lStructSize = sizeof(OPENFILENAME) ; // Specifies the length of the structure in bytes
		ofn.hwndOwner = GetActiveWindow() ; // Identifies the window that owns the dialog box.
		ofn.lpstrFilter = lpstrFilter ; // filter strings
		ofn.nFilterIndex = lpImpExpInfo->nFilterIndex ; // selects the first filter in the szFilter buffer
		ofn.lpstrFile = lpImpExpInfo->lpstrFile ;
		ofn.nMaxFile = sizeof (lpImpExpInfo->lpstrFile) ; // should be at least 256
		ofn.lpstrTitle = "Create Export File" ;
		ofn.Flags =  OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR | OFN_HIDEREADONLY ;
		ofn.lpstrDefExt = lpImpExpInfo->lpstrDefExt ;

		if (GetSaveFileName (&ofn) == FALSE)
			{
			AObjUnlockData(oiImportExport, INFO_INDEX) ;
			theSystem->message1 = 2 ; // signal flow index for CANCEL
			return ;
			}
		lpImpExpInfo->nFilterIndex = (int)ofn.nFilterIndex  ; // saves the first filter in the szFilter buffer
		}
 	hFile = CreateFile (
	 										lpImpExpInfo->lpstrFile, 
	 										GENERIC_READ|GENERIC_WRITE, 
	 										0,
											NULL,
	 										CREATE_ALWAYS, 
	 										FILE_ATTRIBUTE_NORMAL,
											NULL) ;
	if(hFile==INVALID_HANDLE_VALUE)
		{
		AObjUnlockData(oiImportExport, INFO_INDEX) ;
		ADBG_PRINT_W("Could not create export file") ;
		theSystem->message1 = 2 ; // signal flow index for error
		return ;
		}
	CloseHandle(hFile) ;
	AOBJECTID oiPathName = AObjCreate(OTYPE_TEXT) ;
	OTextSetValueFromBuffer(oiPathName, lpImpExpInfo->lpstrFile) ;
	AFuncSetParameter(1, oiPathName) ;
	AObjUnlockData(oiImportExport, INFO_INDEX) ;
	theSystem->message1 = 1 ; // signal flow index for OKAY
	return ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  ChooseFileFunc
// DESCRIPTION:  Allows you or the user of your application to select
//		a text file for importing data.
// INPUT 1: an Import-Export Object
// INPUT 2 (optional): a Text Object (path name of the file to create)
// OUTPUT 1: a Text Object (path name of the file created)
// FLOW 1: the file has been created
// FLOW 2: the user canceled or an error occured
/////////////////////////////////////////////////////////////////////////////
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  ChooseFileFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	ImpExpInfo  *lpImpExpInfo ;
	OPENFILENAME ofn ;
	HANDLE hFile ; // MS-DOS file handle

	AOBJECTID oiImportExport = AFuncGetTypedParameter(1, OTYPE_IMPORTEXPORT) ;
	lpImpExpInfo = (ImpExpInfo  *)AObjLockData(oiImportExport, INFO_INDEX) ;

	AOBJECTID oiFileName = AFuncGetTypedParameter(2, OTYPE_TEXT) ;
	if(oiFileName)  // get the object ID for the File object
		{
		OTextGetToBuffer(oiFileName, lpImpExpInfo->lpstrFile, sizeof(lpImpExpInfo->lpstrFile)-1) ;
		//if ((hFile = CreateFile/*!!PORT!! was _lopen*/(lpImpExpInfo->lpstrFile, 0)) == -1)
		hFile = CreateFile (
	 										lpImpExpInfo->lpstrFile, 
	 										GENERIC_READ|GENERIC_WRITE, 
	 										0,
											NULL,
	 										OPEN_ALWAYS, 
	 										FILE_ATTRIBUTE_NORMAL,
											NULL) ;

		if(hFile ==INVALID_HANDLE_VALUE)
			{
			AObjUnlockData(oiImportExport, INFO_INDEX) ;
			ADBG_PRINT_W("Could not open import/export file") ;
			theSystem->message1 = 2 ; // signal flow index for CANCEL
			return ;
			}
		CloseHandle(hFile) ;
		}
	else
		{
		// Set all structure members to zero
		memset (&ofn, 0, sizeof(OPENFILENAME)) ;
		ofn.lStructSize = sizeof(OPENFILENAME) ; // Specifies the length of the structure in bytes
		ofn.hwndOwner = GetActiveWindow() ; // Identifies the window that owns the dialog box.
		ofn.lpstrFilter = lpstrFilter ; // filter strings
		ofn.nFilterIndex = lpImpExpInfo->nFilterIndex ; // selects the first filter in the szFilter buffer
		ofn.lpstrFile = lpImpExpInfo->lpstrFile ;
		ofn.nMaxFile = sizeof (lpImpExpInfo->lpstrFile) ; // should be at least 256
		ofn.lpstrTitle = "Choose Import File" ;
		ofn.Flags =  OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ;
		ofn.lpstrDefExt = lpImpExpInfo->lpstrDefExt ;

		if (GetOpenFileName (&ofn) == FALSE)
			{
			AObjUnlockData(oiImportExport, INFO_INDEX) ;
			theSystem->message1 = 2 ; // signal flow index for CANCEL
			return ;
			}
		lpImpExpInfo->nFilterIndex = (int)ofn.nFilterIndex  ; // saves the first filter in the szFilter buffer
		}
	AOBJECTID oiPathName = AObjCreate(OTYPE_TEXT) ;
	OTextSetValueFromBuffer(oiPathName, lpImpExpInfo->lpstrFile) ;
	AObjUnlockData(oiImportExport, INFO_INDEX) ;
	AFuncSetParameter( 1, oiPathName) ;
	theSystem->message1 = 1 ; // signal flow index for OKAY
	return ;
	}
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  StartImportExportFunc
// DESCRIPTION:  Opens a file for importing or exporting.
// INPUT 1: an Import-Export Object
// INPUT 2 (optional): a Number Object (a number corresponding to the end of the export: used for export status)
// OUTPUT 1: a Number Object (file size in bytes: used for import status)
// FLOW 1: the file has been created
// FLOW 2: an error has occured
/////////////////////////////////////////////////////////////////////////////
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  StartImportExportFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	ImpExpInfo  *lpImpExpInfo ;
	HANDLE hFile ; // MS-DOS file handle

	// Get the Import/Export object's identifier
	AOBJECTID oiImportExport = AFuncGetTypedParameter(1, OTYPE_IMPORTEXPORT) ;  // get the handle for the Import Export object
	lpImpExpInfo = (ImpExpInfo  *)AObjLockData(oiImportExport, INFO_INDEX) ;

	// Check the object's busy status
	if (lpImpExpInfo->Busy)
		{
		AObjUnlockData(oiImportExport, INFO_INDEX) ;
		ADBG_PRINT_W("The Import/Export object is already assigned to another transfer.") ;
		theSystem->message1 = 2 ;
		return ;
		}

	// open file
	//f (-1 == (hFile =/*!!PORT!! was _lopen*/ (lpImpExpInfo->lpstrFile, OF_READWRITE)))
	hFile = CreateFile (
	 										lpImpExpInfo->lpstrFile, 
	 										GENERIC_READ|GENERIC_WRITE, 
	 										0,
											NULL,
	 										OPEN_ALWAYS, 
	 										FILE_ATTRIBUTE_NORMAL,
											NULL) ;

	if(hFile==INVALID_HANDLE_VALUE)
	 	{
		AObjUnlockData(oiImportExport, INFO_INDEX) ;
		ADBG_PRINT_W("Cannot open the Import/Export file.") ;
		theSystem->message1 = 2 ;
		return ;
		}

	lpImpExpInfo->Busy = TRUE ; // Set the object's busy status
	long nBytes = GetFileSize (hFile, NULL) ;//filelength (hFile) ;
	lpImpExpInfo->FileSize = nBytes ; // Save file size
	if (-1 == nBytes)
		{
		AObjUnlockData(oiImportExport, INFO_INDEX) ;
		ADBG_PRINT_W("Cannot read the Import/Export file.") ;
		theSystem->message1 = 2 ;
		CloseHandle(hFile) ;
		return ;
		}

	// Create a number object to store the file's size
	AOBJECTID oiFileSize = AObjCreate (OTYPE_NUMBER) ;
	if (oiFileSize)
		{
		ONmbrSetInteger(oiFileSize, nBytes, FALSE) ;
		AFuncSetParameter(1, oiFileSize) ;
		}

	// initialize the runtime information record
	if (!(lpImpExpInfo->lpRuntimeInfo = new RuntimeInfo))
		{
		AObjUnlockData(oiImportExport, INFO_INDEX) ;
		ADBG_PRINT_W("Unable to allocate memory for Import/Export Runtime Information") ;
		theSystem->message1 = 2 ;
		CloseHandle(hFile) ;
		return ;
		}

	RuntimeInfo * lpRTInfo = lpImpExpInfo->lpRuntimeInfo ;
	// Was the optional status parameter provided?
	AOBJECTID oiFinalStatus = AFuncGetTypedParameter(2, OTYPE_NUMBER) ;
	if (oiFinalStatus)
		lpRTInfo->finalStatus = ONmbrGetInteger(oiFinalStatus) ;
	else
		lpRTInfo->finalStatus = 0 ;

	lpRTInfo->currentStatus = 0 ;
	lpRTInfo->lRecords = 0 ;
	lpRTInfo->lpBuffer = NULL ;
	lpRTInfo->wMark = 0 ;
	lpRTInfo->wBytes = 0 ;
	lpRTInfo->lBlockBytes = 0 ;
	lpRTInfo->EndOfFile = FALSE ;
	lpRTInfo->StatusDlg = 0 ;
	lpRTInfo->Abort = FALSE ;
	lpRTInfo->hFile = hFile ;

	AObjUnlockData(oiImportExport, INFO_INDEX) ;
	theSystem->message1 = 1 ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  ConfigureExportFunc
// DESCRIPTION:  Allows you or the user of your application to configure
//		the import/export object for exports.  In particular, it allows
//		specification of the delimiters, objects to export, assignment
//		of objects to fields, etc.
// INPUT 1: an Import-Export Object
// FLOW 1: the file has been created
// FLOW 2: the user canceled or an error occured
/////////////////////////////////////////////////////////////////////////////
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  ConfigureExportFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	AOBJECTID oiImportExport = AFuncGetTypedParameter(1, OTYPE_IMPORTEXPORT) ;
	if(DialogBoxParam( hInstance, "ExportConfiguration", NULL, (DLGPROC)ConfigExportDlgProc, (long)oiImportExport))
		theSystem->message1 = 1 ;  // OKAY
	else
		theSystem->message1 = 2 ;  // CANCELLED
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  ConfigureImportFunc
// DESCRIPTION:  Allows you or the user of your application to configure
//		the import/export object for imports.  In particular, it allows
//		specification of the delimiters, objects to export, assignment
//		of objects to fields, etc.
// INPUT 1: an Import-Export Object
// FLOW 1: the file has been created
// FLOW 2: the user canceled or an error occured
/////////////////////////////////////////////////////////////////////////////
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  ConfigureImportFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	AOBJECTID oiImportExport = AFuncGetTypedParameter(1, OTYPE_IMPORTEXPORT) ;
	if(DialogBoxParam( hInstance, "ImportConfiguration", NULL, (DLGPROC)ConfigImportDlgProc, (long)oiImportExport))
		theSystem->message1 = 1 ;  // OKAY
	else
		theSystem->message1 = 2 ;  // CANCELLED
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  StopImportExportFunc
// DESCRIPTION:  Writes text data from the chosen objects to a line
//		in the selected text file
// INPUT 1: an Import-Export Object
// FLOW 1: after stopping an import/export process
// NOTE: I think there should be a second flow for errors
/////////////////////////////////////////////////////////////////////////////
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  StopImportExportFunc(pAEvtInfo, pAObjMessage theSystem)
	{
	ImpExpInfo  *lpImpExpInfo ; // general information related to the import/export object

	AOBJECTID oiImportExport = AFuncGetTypedParameter(1, OTYPE_IMPORTEXPORT) ;
	lpImpExpInfo = (ImpExpInfo  *) AObjLockData(oiImportExport, INFO_INDEX) ;
	lpImpExpInfo->Busy = FALSE ;  // no longer busy
	lpImpExpInfo->Status = O_UNCOMMITTED ; // not committed to import or export
	lpImpExpInfo->FileSize = 0 ;

	RuntimeInfo * lpRTInfo = lpImpExpInfo->lpRuntimeInfo ;
	if(lpRTInfo)
		{
		if(lpRTInfo->StatusDlg)
			DestroyWindow(lpRTInfo->StatusDlg) ;

		if(lpRTInfo->lpBuffer)
			delete [] (lpRTInfo->lpBuffer) ;

		if(lpRTInfo->hFile)
			CloseHandle(lpRTInfo->hFile) ;

		delete(lpRTInfo) ;
		}
	lpImpExpInfo->lpRuntimeInfo = NULL ;
	AObjUnlockData(oiImportExport, INFO_INDEX) ;
	theSystem->message1 = 1 ;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  ShutdownMeth
// DESCRIPTION:  Frees memory and resources which may not have been freed
//		if the program was halted before "StopImportExport" was called.
/////////////////////////////////////////////////////////////////////////////
void CALLBACK /*!!PORT!! was _far_ _pascal_*/  ShutdownMeth( pAEvtInfo, pAObjMessage)
	{
	AOBJECTID oiImportExport ; // Handle to the Import Export object
	ImpExpInfo  *lpImpExpInfo ; // general information related to the import/export object

	long key = AOBJ_GETFIRSTKEY;			// initialization
	while((oiImportExport = AObjGetNextObject(OTYPE_IMPORTEXPORT, &key)) != 0)
		{
		lpImpExpInfo = (ImpExpInfo  *) AObjLockData(oiImportExport, INFO_INDEX) ;
		lpImpExpInfo->Busy = FALSE ;  // no longer busy
		lpImpExpInfo->Status = O_UNCOMMITTED ; // not committed to import or export
		lpImpExpInfo->FileSize = 0 ;

		RuntimeInfo * lpRTInfo = lpImpExpInfo->lpRuntimeInfo ;
		if(lpRTInfo)
			{
			if(lpRTInfo->StatusDlg)
				DestroyWindow(lpRTInfo->StatusDlg) ;

			if(lpRTInfo->lpBuffer)
				delete [] (lpRTInfo->lpBuffer) ;

			if(lpRTInfo->hFile)
				CloseHandle(lpRTInfo->hFile) ;

			delete(lpRTInfo) ;
			}
		lpImpExpInfo->lpRuntimeInfo = NULL ;
		AObjUnlockData(oiImportExport, INFO_INDEX) ;
		}
	}
