#define ASTRICT
#define STRICT

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mem.h>
#include <bwcc.h>
#include <helpids.h>

#include	<a_alm.h>
#include	<a_almutl.h>
#include	<op_wnd.h>
#include	<o_wnd.h>
#include	<intdebug.h>

#include "dll.h"


#include "dllexehd.h"


WORD readDLLOldHeader(HFILE hFile, LONG lFileLen, PTLONG lPosNewHdr);
WORD readDLLNewHeader(HFILE	hFile, LONG lFileLen, LONG lPosNewHdr,
												PTLONG	plPosResTable, PTLONG plPosNonResTable);



////////////////////////////////////////////////////////
// openDLLFile: open the dll and extract the exported
//		functions from both the resident-name table and
//		the non-resident name table.
////////////////////////////////////////////////////////

void	DLL_OBJECT::openDLLFile(
	HWND hDlg,
  char* newFileNamePtr)
{
	HFILE			hFile;
  OFSTRUCT	ofFile;
	WORD			wResult;
	LONG			lPos;
	LONG			lPosNewHdr;
	LONG			lFileLen;
	LONG			lPosResTable;
	LONG			lPosNonResTable;
	FUNCNAME	funcNameRec;							// name of the function record.
	BOOL			readNames;

	wResult = IDERR_SUCCESS;
	hFile = OF_ERROR;
	if ((hFile = CreateFile(newFileNamePtr, &ofFile, OF_READ)) == OF_ERROR) {	// open file.
		wResult = IDERR_OPENFAIL;
		SendDlgItemMessage(hDlg, IDD_DLLFUNCNAME, CB_RESETCONTENT, 0, 0L) ;
		EnableWindow(GetDlgItem(hDlg, IDD_DLLFUNCNAME), FALSE);	// alm type.
		EnableWindow(GetDlgItem(hDlg, IDD_DLLCTYPE), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDD_DLLPASCALTYPE), FALSE);
	}
	if (wResult == IDERR_SUCCESS)
		if ((lFileLen = SetFilePointer(hFile, 0L, SEEK_END)) == -1)	// get file length.
			wResult = IDERR_READFAIL;

		// read old header, verify contents and get position of new header.
	if (wResult == IDERR_SUCCESS)
		wResult = readDLLOldHeader(hFile, lFileLen, &lPosNewHdr);

		// read new header, and get position of resident and
    // non-resident name tables.
	if (wResult == IDERR_SUCCESS)
		wResult = readDLLNewHeader(hFile, lFileLen, lPosNewHdr, &lPosResTable, &lPosNonResTable);

	if (wResult == IDERR_SUCCESS)
	{			// get list of functions, both resident and non-resident, add to dialog.
//		USHORT	cb;

		EnableWindow(GetDlgItem(hDlg, IDD_DLLFUNCNAME), TRUE);	// alm type.
		EnableWindow(GetDlgItem(hDlg, IDD_DLLCTYPE), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDD_DLLPASCALTYPE), TRUE);
			// clear all the entries from the function list combobox....
		SendDlgItemMessage(hDlg, IDD_DLLFUNCNAME, CB_RESETCONTENT, 0, 0L) ;

				// read the resident name table.
//		cb = 0;
		lPos = SetFilePointer(hFile, lPosResTable, SEEK_SET);
		if (lPos == -1 || lPos > lFileLen || lPos != lPosResTable)
			wResult = IDERR_READFAIL;
		readNames = TRUE;


			// skip over the dll name, WEP/*!!PORT!! Replace with DllEntryPoint */ and LIBMAIN functions.

		if (wResult == IDERR_SUCCESS) {			// the LIB name or dll.
			ReadFile(hFile, (LPSTR) &(funcNameRec.nameLen), sizeof(char));
			if (funcNameRec.nameLen  != 0) {
				ReadFile(hFile, (LPSTR) &(funcNameRec.funcName), funcNameRec.nameLen);
				ReadFile(hFile, (LPSTR) &(funcNameRec.ordinalNo), sizeof(int));
			}
/*
			ReadFile(hFile, (LPSTR) &(funcNameRec.nameLen), sizeof(char));
			if (funcNameRec.nameLen  != 0) {
				ReadFile(hFile, (LPSTR) &(funcNameRec.funcName), funcNameRec.nameLen);
				ReadFile(hFile, (LPSTR) &(funcNameRec.ordinalNo), sizeof(int));
			}
			ReadFile(hFile, (LPSTR) &(funcNameRec.nameLen), sizeof(char));
			if (funcNameRec.nameLen  != 0) {
				ReadFile(hFile, (LPSTR) &(funcNameRec.funcName), funcNameRec.nameLen);
				ReadFile(hFile, (LPSTR) &(funcNameRec.ordinalNo), sizeof(int));
			}
*/
		}


				// now for the real stuff.
		while (readNames == TRUE)
    	{
			if (wResult == IDERR_SUCCESS)
				ReadFile(hFile, (LPSTR) &(funcNameRec.nameLen), sizeof(char));
			if (funcNameRec.nameLen  == 0)
				readNames = FALSE;
			else {
				if (wResult == IDERR_SUCCESS)
					ReadFile(hFile, (LPSTR) &(funcNameRec.funcName), funcNameRec.nameLen);
				if (wResult == IDERR_SUCCESS) {				// add name to function list in dialog.
					funcNameRec.funcName[funcNameRec.nameLen]	= '\0';
					SendDlgItemMessage(hDlg, IDD_DLLFUNCNAME, CB_ADDSTRING, 0, (long)&(funcNameRec.funcName)) ;
        }
				if (wResult == IDERR_SUCCESS)
					ReadFile(hFile, (LPSTR) &(funcNameRec.ordinalNo), sizeof(int));
			}
		}				// while (readNames ...

				// read the non-resident name table.
//		cb = 0;
		lPos = SetFilePointer(hFile, lPosNonResTable, SEEK_SET);
		if (lPos == -1 || lPos > lFileLen || lPos != lPosNonResTable)
			wResult = IDERR_READFAIL;
		readNames = TRUE;

				// skip over the msg trash...
		if (wResult == IDERR_SUCCESS) {
			ReadFile(hFile, (LPSTR) &(funcNameRec.nameLen), sizeof(char));
			if (funcNameRec.nameLen  != 0) {
				ReadFile(hFile, (LPSTR) &(funcNameRec.funcName), funcNameRec.nameLen);
				ReadFile(hFile, (LPSTR) &(funcNameRec.ordinalNo), sizeof(int));
			}
		}



		while (readNames == TRUE)
    	{
			if (wResult == IDERR_SUCCESS)
				ReadFile(hFile, (LPSTR) &(funcNameRec.nameLen), sizeof(char));
			if (funcNameRec.nameLen  == 0)
				readNames = FALSE;
			else {
				if (wResult == IDERR_SUCCESS)
					ReadFile(hFile, (LPSTR) &(funcNameRec.funcName), funcNameRec.nameLen);			
				if (wResult == IDERR_SUCCESS)	{			// add name to function list in dialog.
					funcNameRec.funcName[funcNameRec.nameLen]	= '\0';
					if (lstrcmp(&(funcNameRec.funcName[0]), "WEP/*!!PORT!! Replace with DllEntryPoint */") != FALSE)
						SendDlgItemMessage(hDlg, IDD_DLLFUNCNAME, CB_ADDSTRING, 0, (long)&(funcNameRec.funcName)) ;
        }
				if (wResult == IDERR_SUCCESS)
					ReadFile(hFile, (LPSTR) &(funcNameRec.ordinalNo), sizeof(int));
			 }
		}				// while (readNames ...
	}
	if (hFile != OF_ERROR)
		CloseHandle(hFile);
}


////////////////////////////////////////////////////////
// readDLLOldHeader: This function reads the old header
//		from a DLL (.EXE) file, checks to make sure that
//		it is a valid header, and saves the position of
//		the files new header.
//
//				hFile					dll file handle.
//				lFileLen			length of file.
//				pPosNewHdr		pointer to new header.
//				
////////////////////////////////////////////////////////

WORD readDLLOldHeader(
	HFILE	hFile,
	LONG	lFileLen,
	PTLONG	plPosNewHdr)
{
	LONG		lPos;
	USHORT	cb;
	EXEHDR	ehOldHeader;
	WORD		wResult;

	wResult = IDERR_SUCCESS;
	lPos = SetFilePointer(hFile, 0L, SEEK_SET);
	if (lPos == -1 || lPos != 0)
		wResult = IDERR_READFAIL;
	if (wResult == IDERR_SUCCESS)
	{
		cb = ReadFile(hFile, (LPSTR)&ehOldHeader, sizeof(ehOldHeader));

		if (cb != sizeof(ehOldHeader))
			wResult = IDERR_READFAIL;
		else if (ehOldHeader.ehSignature != OLDEXESIGNATURE)
			wResult = IDERR_FILETYPEBAD;
		else if (ehOldHeader.ehPosNewHdr < sizeof(EXEHDR))
			wResult = IDERR_EXETYPEBAD;
		else if (ehOldHeader.ehPosNewHdr > lFileLen - sizeof(NEWHDR))
			wResult = IDERR_EXETYPEBAD;
		else
			*plPosNewHdr = ehOldHeader.ehPosNewHdr;
	}
	return wResult;
}


////////////////////////////////////////////////////////////////
// readDLLNewHeader: This function reads the new header
//		from a DLL (.EXE) file, checks to make sure that
//		it is a valid header, and saves the position of
//		the files resident and non-resident name tables.
//
//				hFile							dll file handle.
//				lFileLen					length of file.
//				lPosNewHdr				new header.
//				plPosResTable			pointer to resident name table.
//				plPosNonResTable	pointer to non-resident name table
////////////////////////////////////////////////////////////////

WORD readDLLNewHeader(
	HFILE	hFile,
	LONG	lFileLen,
	LONG	lPosNewHdr,
	PTLONG	plPosResTable,
	PTLONG	plPosNonResTable)

{
	LONG		lPos;
	USHORT	cb;
	NEWHDR	nhNewHeader;
	WORD		wResult;

	wResult = IDERR_SUCCESS;
	lPos = SetFilePointer(hFile, lPosNewHdr, SEEK_SET);
	if (lPos == -1 || lPos > lFileLen || lPos != lPosNewHdr)
		wResult = IDERR_READFAIL;
	else
	{
		WORD	wVersion;

    wVersion = (GetVersion() >> 8) | (GetVersion() << 8);
		cb = ReadFile(hFile, (LPSTR)&nhNewHeader, sizeof(nhNewHeader));

		if (cb != sizeof(nhNewHeader))
			wResult = IDERR_READFAIL;
		else if (nhNewHeader.nhSignature != NEWEXESIGNATURE)
			wResult = IDERR_FILETYPEBAD;
		else if (nhNewHeader.nhExeType != WINDOWSEXE)		// make sure that it is a window dll.
			wResult = IDERR_EXETYPEBAD;
		else if (nhNewHeader.nhExpVer <0x0300 )		// check the window type
			wResult = IDERR_WINVERSIONBAD;
		else if (nhNewHeader.nhExpVer >	wVersion )
			wResult = IDERR_WINVERSIONBAD;

		else if (nhNewHeader.nhoffResNameTable == 0)
			wResult = IDERR_RESNAMETBLBAD;
		else if (nhNewHeader.nhoffNonResNameTable == 0)
			wResult = IDERR_NONRESNAMETBLBAD;
		else {
			*plPosResTable = lPosNewHdr + nhNewHeader.nhoffResNameTable;
//			*plPosNonResTable = lPosNewHdr + nhNewHeader.nhoffNonResNameTable - 3;
			*plPosNonResTable = nhNewHeader.nhoffNonResNameTable;
		}
	}
	return wResult;
}

