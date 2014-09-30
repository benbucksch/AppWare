/*//////////////////////////////////////////////////////////////
//
//					Copyright 1993, Novell, Inc.  All rights reserved
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//	A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//	THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//	ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
// to force strict compile
#define STRICT
#define ASTRICT




#include "a_alm.h"
#include "windows.h"
#include "picture.h"
#include "string.h"
#include "commdlg.h"
#include "stdlib.h"
#include "o_wnd.h"
#include "op_wnd.h"
#include "o_text.h"

extern LPLONG GetPictObject(OBJECTID objID);



static char szFilter[256] = "\0";
static 	char szFile[256] = "\0";
int GetSaveFile(OBJECTID objID, LPSTR fName) {

	OPENFILENAME opn; //        lpFSChunk;
	char 					szDirName[256];
	static char 	szFileTitle[256];
  char          sztmpFile[256];
	LPSTR  lpext;

	int rVal;
	int size;

	// skj, 3-21-93 init from the datas
	PICTOBJECT *pObject =  (PICTOBJECT *)  GetPictObject(objID);
	memset((LPVOID)&opn, 0, sizeof(OPENFILENAME));
	// szFile[0] = '\0';
	if (pObject->GetPictType() == BITMAPPICT)
		{
		lstrcpy(szFilter, "BITMAP FILES (*.bmp) *.bmp");
		lstrcpy(sztmpFile, "*.bmp");
		}
	else if (pObject->GetPictType() == METAPICT)
		{
		lstrcpy(szFilter, "META FILES (*.wmf) *.wmf");
		lstrcpy(sztmpFile, "*.wmf");
		}
	
	lpext = strchr(szFilter, ')');
	size = strlen(szFilter);

	lpext[1] = '\0';
	szFilter[size + 1] = '\0';


	szDirName[0] = '\0';

  if (szFile[0] == '\0')
		{
		GetSystemDirectory(szDirName, sizeof(szDirName));

		strcpy(szFile, sztmpFile);

		}
	else
		{
		LPSTR lpOld;
		LPSTR lpTmp = new char[strlen(szFile) + 1];
		strcpy(lpTmp, szFile);
		lpOld = lpTmp;
		while ((lpTmp = strchr(lpTmp, '\\')) != (LPSTR) NULL)
			{
			lpOld = lpTmp;
			
			lpTmp++;


			}
		szFile[strlen(szFile) - strlen(lpOld)] = '\0';

		strcpy(szDirName, szFile);
		strcpy(szFile, sztmpFile);
		delete lpTmp;
		}
	opn.lStructSize       = sizeof(OPENFILENAME);
	opn.hwndOwner         = (HWND)GetActiveWindow(); // Oct. 15, 1993 SKJ do not use NULL,
																									 // for task-modal dialog box

	// opn.hInstance         = (HANDLE)NULL;
	opn.lpstrFilter       = szFilter;
	opn.nFilterIndex      = 1L;
	opn.lpstrFile         = szFile;
	opn.nMaxFile          = (DWORD)sizeof(szFile);
	opn.lpstrFileTitle    = szFileTitle;
	opn.nMaxFileTitle     = 80; //MAXFILETITLELEN;
	opn.lpstrInitialDir   = szDirName; //gszBuffer;
	opn.lpstrTitle        = (LPSTR)"Save Picture";
	opn.Flags             = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY
													| OFN_NOCHANGEDIR ;


  rVal = GetSaveFileName( &opn);

	lstrcpy(fName, (LPSTR) opn.lpstrFile);

  return rVal;
  }


extern "C" void FAR PASCAL _export SavePict(
  pAEvtInfo theEvent,
  pAObjMessage system) {
	POINT where;
	int rVal;
	OBJECTID  pObjectID, dObject;
	char fName[255];


  pObjectID = AFuncGetParameter(1); // the picture object to load New picture
				// into

  dObject = NULLOBJECTID;
  system->message1 = 1;
	if (AObjCheckType( pObjectID, (TYPEID)OTYPE_PICTURE))
    {

		// if a filename is supplied as input then load that file into the
    // pict object else open up a file-open dlg (W3.1 - commdlg)

		if ((dObject = AFuncGetTypedParameter(2, OTYPE_TEXT)) != 0L)
			{
			OTextGetToBuffer(dObject, fName, MAXNAME);
			
      //tPtr = (LPTEXTOBJECT)AObjLockData(dObject, 1);

      //_fmemcpy(fName, tPtr, AObjGetDataSize(dObject, 1));

			if (strcmp(fName, "\0") == 0)
				rVal = GetSaveFile(pObjectID, fName);

			// AObjUnlockData(dObject, 1);


      }
    else {
      fName[0] = '\0';


      where.x = 40;
      where.y = 40;
			rVal = GetSaveFile(pObjectID, fName);

     }


    if (rVal)
			{
			PICTOBJECT * pObject = (PICTOBJECT *)GetPictObject(pObjectID);
			rVal = pObject->SavePictFile((LPSTR)fName);
			if (!rVal)
				system->message1 = 2;

			}
		else
			system->message1 = 2;

    }
  else
    system->message1 = 2;

	I_UNUSED(where);
	I_UNUSED(theEvent);

	}
