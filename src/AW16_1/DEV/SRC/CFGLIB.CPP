#include <dos.h>
#include <stdlib.h>
#include "almsys.h"
#include <windowsx.h>

#include "cfglib.h"
#include "version.h"
#include "fileobj.h"
#include "devsys.h"
#include "list.h"
#include "compuid.h"
#include "sereclib.h"
#include "cfgdesc.h"
#include "funcdesc.h"
#include "objdesc.h"
#include "evntdesc.h"
#include "serblk.h"
#include "recblk.h"
#include "mdiapp.h"
#include "inifile.h"


#define	MAXDIRLENGTH 100

static LIST	         cfgsList;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	cfgGetDatabaseVersion
//
// DESCRIPTION.:  Returns the database version of a file.  Assumes that the
//                file is a valid serius config file of some database
//                version.
//
/////////////////////////////////////////////////////////////////////////////
BOOL cfgGetDatabaseVersion(
	int 		hFile)
	{
	FILEIDINFO	idInfo;

	_lread(hFile, (LPSTR)&idInfo, sizeof(FILEIDINFO));

	_llseek(hFile, 0, 0);

	return idInfo.databaseVersion;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: cfgReadConfigs
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void cfgReadConfigs(
	void)
	{
	char					objectsDir[MAXDIRLENGTH+1];
	char					filePath[MAXDIRLENGTH+15];
	find_t  			fileInfo;
	int						nPathChars;
	int						hFile;
	LPFILEOBJECT	fileObject;
	int						foundFile;
	TSTRING				savePath;
  WORD          dbVersion;

	nPathChars = GetPrivateProfileString("Configuration Files", "Directory",
	 ".", objectsDir, 100, INI_FILE_NAME);

	if (nPathChars >=	MAXDIRLENGTH)
		{
		MessageBox(NULL, INI_FILE_NAME " -- Directory path for config files is too long.",
		 "Info", MB_OK);
		lstrcpy(objectsDir, ".");
		lstrcpy(filePath, ".\\");
		}
	else
		{
		lstrcpy(filePath, objectsDir);
		lstrcat(filePath, "\\");
		}

	savePath = filePath;

	lstrcat(objectsDir, "\\*.cfg");

	foundFile = _dos_findfirst(objectsDir, _A_NORMAL, &fileInfo);

	while (foundFile == 0)
		{
		lstrcpy(filePath+nPathChars+1, fileInfo.name);
		hFile = _lopen(filePath, OF_READ|OF_SHARE_COMPAT);
		if (hFile != -1)
			{
			if (cfgCheckConfigFile(hFile, fileInfo.name))
				{
        dbVersion = cfgGetDatabaseVersion(hFile);
        if (dbVersion == DATABASE_VERSION)
          {
				  fileObject = new FILEOBJECT(hFile, savePath, fileInfo.name);
					cfgsList.Add(fileObject);
          }
        else
          {
          fileObject = NULL;
					MessageBox(GetActiveWindow(), "Invalid configuration file.",
										"Read Config File", MB_OK);
          }
				}
			_lclose(hFile);
			}
		foundFile = _dos_findnext(&fileInfo);
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	cfgCheckConfigFile
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL cfgCheckConfigFile(
	int 		hFile,
	LPSTR   fileName)
	{
	DWORD 			status;
	int 				bytesRead;
	FILEIDINFO	idInfo;
	char				buffer[256];

	bytesRead = _lread(hFile, (LPSTR)&idInfo, sizeof(FILEIDINFO));

	if (bytesRead < sizeof(FILEIDINFO))
		status = FILESTAT_SHORT;
	else if (lstrcmp(idInfo.idString, ID_STRING ) != 0)
		status = FILESTAT_NOTSERIUSFILE;
	else if (idInfo.fileType != FILETYPE_CONFIG)
		status = FILESTAT_WRONGTYPE;
	else
		status = FILESTAT_OK;

	_llseek(hFile, 0, 0);

	if (status & FILESTAT_OK)
		return idInfo.databaseVersion;
	else
		{
		wsprintf(buffer, "%s is not a valid configuration file", fileName);
		MessageBeep(0);
		MessageBox(NULL, buffer, "Open File Error", 
			MB_OK|MB_ICONEXCLAMATION | MB_TASKMODAL);
		return 0;
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: cfgCleanUp
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void cfgCleanUp(
	void)
	{

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: cfgAddConfig
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL cfgAddConfig(
	LPFILEOBJECT	fileObject)
	{

	return cfgsList.Add(fileObject);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: cfgGetNextObjDef
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
LPOBJDESC cfgGetNextObjDesc(
	LPOBJDESC	currObjDesc)
	{
	static	LPFILEOBJECT	currFileObj;
	static  LPCFGDESC			currCFGDesc;
	LPOBJDESC							nextObjDesc;

	if ((currObjDesc == NULL))
		{
		nextObjDesc = NULL;
		currFileObj = NULL;
		currCFGDesc = NULL;
		}
	else
		{
		pTheSystem->PushContext(currFileObj);
		nextObjDesc = currCFGDesc->NextObjectDesc(currObjDesc);
		pTheSystem->PopContext();
		}

	if (nextObjDesc == NULL)
		while ((0 != (currFileObj=(LPFILEOBJECT)cfgsList.GetNextMember(currFileObj))) &&
					 pTheSystem->PushContext(currFileObj) &&
					 (0 != (currCFGDesc = (LPCFGDESC)GetObject(currFileObj->GetHeadRec()))))
			{
			nextObjDesc = currCFGDesc->NextObjectDesc(NULL);
			pTheSystem->PopContext();
			if (nextObjDesc)
				break;
			}

	return nextObjDesc;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: cfgGetNextFuncDesc
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
LPFUNCDESC cfgGetNextFuncDesc(
	LPFUNCDESC	currFuncDesc)
	{
	static	LPFILEOBJECT	currFileObj;
	static  LPCFGDESC			currCFGDesc;
	LPFUNCDESC						nextFuncDesc;

	if ((currFuncDesc == NULL))
		{
		nextFuncDesc = NULL;
		currFileObj = NULL;
		currCFGDesc = NULL;
		}
	else
		{
		pTheSystem->PushContext(currFileObj);
		nextFuncDesc = currCFGDesc->NextFunctionDesc(currFuncDesc);
		pTheSystem->PopContext();
		}

	if (nextFuncDesc == NULL)
		while ((0 != (currFileObj=(LPFILEOBJECT)cfgsList.GetNextMember(currFileObj))) &&
					 pTheSystem->PushContext(currFileObj) &&
					 (0 != (currCFGDesc = (LPCFGDESC)GetObject(currFileObj->GetHeadRec()))))
			{
			nextFuncDesc = currCFGDesc->NextFunctionDesc(NULL);
			pTheSystem->PopContext();
			if (nextFuncDesc)
				break;
			}

	return nextFuncDesc;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: cfgGetNextEventDesc
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
LPEVENTDESC cfgGetNextEventDesc(
	LPEVENTDESC	currEventDesc)
	{
	static	LPFILEOBJECT	currFileObj;
	static  LPCFGDESC			currCFGDesc;
	LPEVENTDESC						nextEventDesc;

	if ((currEventDesc == NULL))
		{
		nextEventDesc = NULL;
		currFileObj = NULL;
		currCFGDesc = NULL;
		}
	else
		{
		pTheSystem->PushContext(currFileObj);
		nextEventDesc = currCFGDesc->NextEventDesc(currEventDesc);
		pTheSystem->PopContext();
		}

	if (nextEventDesc == NULL)
		while ((0 != (currFileObj=(LPFILEOBJECT)cfgsList.GetNextMember(currFileObj))) &&
					 pTheSystem->PushContext(currFileObj)  &&
					 (0 != (currCFGDesc = (LPCFGDESC)GetObject(currFileObj->GetHeadRec()))))
			{
			nextEventDesc = currCFGDesc->NextEventDesc(NULL);
			pTheSystem->PopContext();
			if (nextEventDesc)
				break;
			}

	return nextEventDesc;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: cfgGetNextFileObject
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
LPFILEOBJECT cfgGetNextFileObject(
	LPFILEOBJECT	currentFileObject)
	{

	return (LPFILEOBJECT)cfgsList.GetNextMember(currentFileObject);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	cfgInitObjectList
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void cfgInitObjectList(
	HWND	hComboBox,
	LPSTR	pszGeneralText)			// = NULL
	{
	LPFILEOBJECT	currFileObj = NULL;
	LPCFGDESC			currCFGDesc;
	LPOBJDESC			currObjDesc;
	short					itemID;

	while (0 != (currFileObj = (LPFILEOBJECT)cfgsList.GetNextMember(currFileObj)))
		{
		pTheSystem->PushContext(currFileObj);
		currCFGDesc = (LPCFGDESC)GetObject(currFileObj->GetHeadRec());
		currObjDesc = NULL;
		while (0 != (currObjDesc = currCFGDesc->NextObjectDesc(currObjDesc)))
			{
			itemID = SendMessage(hComboBox, CB_ADDSTRING, 0,
			 (DWORD)(LPSTR)currObjDesc->GetName());
			SendMessage(hComboBox, CB_SETITEMDATA, itemID, currObjDesc->GetUID());
			}
		pTheSystem->PopContext();
		}

	if (pszGeneralText == NULL)
		pszGeneralText = "General";

	itemID = SendMessage(hComboBox, CB_INSERTSTRING, 0,
	 (LPARAM) pszGeneralText );
	SendMessage(hComboBox, CB_SETITEMDATA, itemID, COMPUID_GENERAL);

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	cfgInitEventList
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
void cfgInitEventList(
	HWND	hComboBox)
	{
	LPFILEOBJECT	currFileObj = NULL;
	LPCFGDESC			currCFGDesc;
	LPEVENTDESC		currEventDesc;
	short					itemID;

	while (0 != (currFileObj = (LPFILEOBJECT)cfgsList.GetNextMember(currFileObj)))
		{
		pTheSystem->PushContext(currFileObj);
		currCFGDesc = (LPCFGDESC)GetObject(currFileObj->GetHeadRec());
		currEventDesc = NULL;
		while (0 != (currEventDesc = currCFGDesc->NextEventDesc(currEventDesc)))
			{
			itemID = SendMessage(hComboBox, CB_ADDSTRING, 0,
			 (DWORD)(LPSTR)currEventDesc->GetName());
			SendMessage(hComboBox, CB_SETITEMDATA, itemID, currEventDesc->GetUID());
			}
		pTheSystem->PopContext();
		}

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	cfgCheckObjectUID
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL cfgCheckObjectUID(
	COMPUID	compUID)
	{
	BOOL result = TRUE;
	LPFILEOBJECT	currFileObj = NULL;
	LPCFGDESC			currCFGDesc;
	LPOBJDESC			currDesc;

	while (0 != (currFileObj = (LPFILEOBJECT)cfgsList.GetNextMember(currFileObj)))
		{
		pTheSystem->PushContext(currFileObj);
		currCFGDesc = (LPCFGDESC)GetObject(currFileObj->GetHeadRec());
		currDesc = NULL;
		while (0 != (currDesc = currCFGDesc->NextObjectDesc(currDesc)))
			{
			if (currDesc->GetUID() == compUID)
				{
				result = FALSE;
				break;
				}
			}
		pTheSystem->PopContext();
		if (result == FALSE)
			break;
		}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	cfgCheckFunctionUID
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL cfgCheckFunctionUID(
	COMPUID	compUID)
	{
	BOOL result = TRUE;
	LPFILEOBJECT	currFileObj = NULL;
	LPCFGDESC			currCFGDesc;
	LPFUNCDESC		currDesc;

	while (0 != (currFileObj = (LPFILEOBJECT)cfgsList.GetNextMember(currFileObj)))
		{
		pTheSystem->PushContext(currFileObj);
		currCFGDesc = (LPCFGDESC)GetObject(currFileObj->GetHeadRec());
		currDesc = NULL;
		while (0 != (currDesc = currCFGDesc->NextFunctionDesc(currDesc)))
			{
			if (currDesc->GetUID() == compUID)
				{
				result = FALSE;
				break;
				}
			}
		pTheSystem->PopContext();
		if (result == FALSE)
			break;
		}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	cfgCheckEventUID
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
BOOL cfgCheckEventUID(
	COMPUID	compUID)
	{
	BOOL result = TRUE;
	LPFILEOBJECT	currFileObj = NULL;
	LPCFGDESC			currCFGDesc;
	LPEVENTDESC		currDesc;

	while (0 != (currFileObj = (LPFILEOBJECT)cfgsList.GetNextMember(currFileObj)))
		{
		pTheSystem->PushContext(currFileObj);
		currCFGDesc = (LPCFGDESC)GetObject(currFileObj->GetHeadRec());
		currDesc = NULL;
		while (0 != (currDesc = currCFGDesc->NextEventDesc(currDesc)))
			{
			if (currDesc->GetUID() == compUID)
				{
				result = FALSE;
				break;
				}
			}
		pTheSystem->PopContext();
		if (result == FALSE)
			break;
		}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	cfgGetEventName
//
// CLASS.......:
//
// DESCRIPTION.:
//
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
TSTRING cfgGetEventName(
	COMPUID		eventID)
	{
	TSTRING				name = "";
	BOOL          foundIt = FALSE;
	LPFILEOBJECT	currFileObj = NULL;
	LPCFGDESC			currCFGDesc;
	LPEVENTDESC		currDesc;

	while (0 != (currFileObj = (LPFILEOBJECT)cfgsList.GetNextMember(currFileObj)))
		{
		pTheSystem->PushContext(currFileObj);
		currCFGDesc = (LPCFGDESC)GetObject(currFileObj->GetHeadRec());
		currDesc = NULL;
		while (0 != (currDesc = currCFGDesc->NextEventDesc(currDesc)))
			{
			if (currDesc->GetUID() == eventID)
				{
				name = currDesc->GetName();
				foundIt = TRUE;
				break;
				}
			}
		pTheSystem->PopContext();
		if (foundIt == TRUE)
			break;
		}

	return name;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	cfgWriteFunction
//
// CLASS.......:
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void cfgWriteFunction(
	int  				hFile,
	LPFUNCDESC  pFuncDesc,
	char far *  buffer)
	{
	LPPARAMDESCARRAY  pParamArray;
	LPPARAMDESC       pParam;
	LPFLOWDESCARRAY   pFlowArray;
	LPFLOWDESC        pFlow;
	int               paramCount;
	int               flowCount;
	char              uidBuffer[8];
	char              label[9];
	int               i;
	COMPUID           tempUID;


	tempUID = pFuncDesc->GetUID();

	_fmemcpy(uidBuffer, &tempUID, sizeof(COMPUID));
	uidBuffer[4] = '\0';
	// these two lines swap the outer bytes and inner bytes
	// to reverse the uid character sequence.
	uidBuffer[0]^=uidBuffer[3]^=uidBuffer[0]^=uidBuffer[3];
	uidBuffer[1]^=uidBuffer[2]^=uidBuffer[1]^=uidBuffer[2];

	wsprintf(buffer, "  Function: %-20.20s  %-7.7s\r\n",
	 (LPSTR)pFuncDesc->GetName(), (LPSTR)uidBuffer);
	_lwrite(hFile, buffer, lstrlen(buffer));


	//write input parameters

	pParamArray = pFuncDesc->GetInputs();
	paramCount = pParamArray->GetCount();

	lstrcpy(label, "Inputs:");

	if (paramCount == 0)
		{
		wsprintf(buffer, "  %-10.10snone\r\n", (LPSTR)label);
		_lwrite(hFile, buffer, lstrlen(buffer));
		}
	else
		{
		for (i=0; i<paramCount; i++)
			{
			pParam = &(*pParamArray)[i];

			if (pParam->type == COMPUID_GENERAL)
				lstrcpy(uidBuffer, "untyped");
			else
				{
				_fmemcpy(uidBuffer, &pParam->type, sizeof(COMPUID));
				uidBuffer[4] = '\0';
				// these two lines swap the outer bytes and inner bytes
				// to reverse the uid character sequence.
				uidBuffer[0]^=uidBuffer[3]^=uidBuffer[0]^=uidBuffer[3];
				uidBuffer[1]^=uidBuffer[2]^=uidBuffer[1]^=uidBuffer[2];
				}

			wsprintf(buffer, "  %-10.10s%-20.20s  %-7.7s  %-8.8s  %-8.8s\r\n",
				(LPSTR)label,
				(LPSTR)pParam->name,
				(LPSTR)uidBuffer,
				pParam->TestNoTempObjectsFlag()?(LPSTR)" ":(LPSTR)"TEMPS",
				pParam->TestOptionalFlag()?(LPSTR)"OPTIONAL":(LPSTR)" ");
			_lwrite(hFile, buffer, lstrlen(buffer));
			lstrcpy(label, " ");
			}
		}

	// write output parameters

	pParamArray = pFuncDesc->GetOutputs();
	paramCount = pParamArray->GetCount();

	lstrcpy(label, "Outputs:");

	if (paramCount == 0)
		{
		wsprintf(buffer, "  %-10.10snone\r\n", (LPSTR)label);
		_lwrite(hFile, buffer, lstrlen(buffer));
		}
	else
		{
		for (i=0; i<paramCount; i++)
			{
			pParam = &(*pParamArray)[i];

			if (pParam->type == COMPUID_GENERAL)
				lstrcpy(uidBuffer, "untyped");
			else
				{
				_fmemcpy(uidBuffer, &pParam->type, sizeof(COMPUID));
				uidBuffer[4] = '\0';
				// these two lines swap the outer bytes and inner bytes
				// to reverse the uid character sequence.
				uidBuffer[0]^=uidBuffer[3]^=uidBuffer[0]^=uidBuffer[3];
				uidBuffer[1]^=uidBuffer[2]^=uidBuffer[1]^=uidBuffer[2];
				}

			wsprintf(buffer, "  %-10.10s%-20.20s  %-7.7s  %-8.8s  %-8.8s\r\n",
				(LPSTR)label,
				(LPSTR)pParam->name,
				(LPSTR)uidBuffer,
				pParam->TestNoTempObjectsFlag()?(LPSTR)" ":(LPSTR)"TEMPS",
				pParam->TestOptionalFlag()?(LPSTR)"OPTIONAL":(LPSTR)" ");
			_lwrite(hFile, buffer, lstrlen(buffer));
			lstrcpy(label, " ");
			}
		}

	//write flows

	pFlowArray = pFuncDesc->GetFlows();
	flowCount = pFlowArray->GetCount();

	lstrcpy(label, "Flows:");

	if (flowCount == 0)
		{
		wsprintf(buffer, "  %-10.10snone\r\n", (LPSTR)label);
		_lwrite(hFile, buffer, lstrlen(buffer));
		}
	else
		{
		for (i=0; i<flowCount; i++)
			{
			pFlow = &(*pFlowArray)[i];

			wsprintf(buffer, "  %-10.10s%s\r\n",
				(LPSTR)label,
				(LPSTR)pFlow->name);
			_lwrite(hFile, buffer, lstrlen(buffer));
			lstrcpy(label, " ");
			}
		}

	//write DLL and Entry Point

	wsprintf(buffer, "  DLL:      %-12.12s  Entry Point: %s\r\n",
	 pFuncDesc->GetLibName(), pFuncDesc->GetEntryPointName());
	_lwrite(hFile, buffer, lstrlen(buffer));


	wsprintf(buffer, "\r\n");
	_lwrite(hFile, buffer, lstrlen(buffer));


	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	cfgWriteObject
//
// CLASS.......:
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void cfgWriteObject(
	int  				hFile,
	LPOBJDESC  pObjDesc,
	char far *  buffer)
	{
	char              uidBuffer[8];
	COMPUID           tempUID;


	tempUID = pObjDesc->GetUID();

	_fmemcpy(uidBuffer, &tempUID, sizeof(COMPUID));
	uidBuffer[4] = '\0';
	// these two lines swap the outer bytes and inner bytes
	// to reverse the uid character sequence.
	uidBuffer[0]^=uidBuffer[3]^=uidBuffer[0]^=uidBuffer[3];
	uidBuffer[1]^=uidBuffer[2]^=uidBuffer[1]^=uidBuffer[2];

	wsprintf(buffer, "  Object:   %-20.20s  %-7.7s\r\n",
	 (LPSTR)pObjDesc->GetName(), (LPSTR)uidBuffer);
	_lwrite(hFile, buffer, lstrlen(buffer));

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	cfgWrite
//
// CLASS.......:
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void cfgWrite(
	int   hFile)
	{
	LPFILEOBJECT	pFileObj = NULL;
	LPCFGDESC			pCFGDesc;
	LPEVENTDESC		pEventDesc;
	LPFUNCDESC    pFuncDesc;
	LPOBJDESC     pObjDesc;
	char          buffer[128];
	BOOL          atLeastOne;

	while (0 != (pFileObj = (LPFILEOBJECT)cfgsList.GetNextMember(pFileObj)))
		{
		pTheSystem->PushContext(pFileObj);
		pCFGDesc = (LPCFGDESC)GetObject(pFileObj->GetHeadRec());

		wsprintf(buffer, "Config File: %s%s\r\n\r\n", pFileObj->GetFilePath(),
		 pFileObj->GetName());

		_lwrite(hFile, buffer, lstrlen(buffer));

		//Write out Objects

		wsprintf(buffer, "Objects:\r\n");
		_lwrite(hFile, buffer, lstrlen(buffer));
		atLeastOne = FALSE;
		pObjDesc = NULL;
		while (0 != (pObjDesc = pCFGDesc->NextObjectDesc(pObjDesc)))
			{
			atLeastOne = TRUE;
      cfgWriteObject(hFile, pObjDesc,	buffer);
			}
		if (atLeastOne == FALSE)
			{
			wsprintf(buffer, "    none\r\n\r\n");
			_lwrite(hFile, buffer, lstrlen(buffer));
			}

		//Write out Functions

		wsprintf(buffer, "Functions:\r\n");
		_lwrite(hFile, buffer, lstrlen(buffer));
		atLeastOne = FALSE;
		pFuncDesc = NULL;
		while (0 != (pFuncDesc = pCFGDesc->NextFunctionDesc(pFuncDesc)))
			{
			atLeastOne = TRUE;
			cfgWriteFunction(hFile, pFuncDesc, buffer);
			}
		if (atLeastOne == FALSE)
			{
			wsprintf(buffer, "    none\r\n\r\n");
			_lwrite(hFile, buffer, lstrlen(buffer));
			}

		//Write out Events

		wsprintf(buffer, "Events:\r\n");
		_lwrite(hFile, buffer, lstrlen(buffer));
		atLeastOne = FALSE;
		pEventDesc = NULL;
		while (0 != (pEventDesc = pCFGDesc->NextEventDesc(pEventDesc)))
			{
			atLeastOne = TRUE;
			}
		if (atLeastOne == FALSE)
			{
			wsprintf(buffer, "    none\r\n\r\n");
			_lwrite(hFile, buffer, lstrlen(buffer));
			}

		pTheSystem->PopContext();
		}

	}

#ifdef AW_I_DEBUG
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  cfgSaveBitmaps
//
// CLASS.......:
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL cfgSaveBitmap(
  LPCSTR    szPath,
  HDC       hdc,
  HBITMAP   hbmp)

  {
  BOOL              fRet              = TRUE;
  BITMAP            bmp;
  BITMAPFILEHEADER* pBF;
  BITMAPINFOHEADER* pBI;
  void huge*        pBits;
  long              nBitmapSize;
  short             nColorBits;
  long              nColorTableSize;
  long              nTotalSize;
  HGLOBAL           hgbl;
  HFILE             hfile;

  GetObject(hbmp, sizeof(BITMAP), (LPSTR)&bmp);
  nColorBits      = bmp.bmBitsPixel * bmp.bmPlanes;
  nColorTableSize = (nColorBits<24) ? (0x00000001L << nColorBits) : 0;
  nBitmapSize     = (((DWORD)bmp.bmWidth * nColorBits + 31)/8) * bmp.bmHeight;
  nTotalSize      = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
                    (nColorTableSize * sizeof(RGBQUAD)) + nBitmapSize;

  hgbl = GlobalAlloc(GPTR, nTotalSize);
  pBF   = (BITMAPFILEHEADER*)GlobalLock(hgbl);
  pBI   = (BITMAPINFOHEADER*)((char*)pBF + sizeof(BITMAPFILEHEADER));
  pBits = (void huge*)((char huge*)pBI + (sizeof(BITMAPINFOHEADER) + (nColorTableSize * sizeof(RGBQUAD))));

  // set up the bitmap file header
  pBF->bfType       = 0x4D42;
  pBF->bfSize       = nTotalSize;
  pBF->bfReserved1  = 0L;
  pBF->bfReserved2  = 0L;
  pBF->bfOffBits    = nTotalSize - nBitmapSize;

  // set up the bitmap info header
  pBI->biWidth          = bmp.bmWidth;
  pBI->biHeight         = bmp.bmHeight;
  pBI->biPlanes         = 1;
  pBI->biSize           = sizeof(BITMAPINFOHEADER);
  pBI->biBitCount       = nColorBits;
  pBI->biCompression    = BI_RGB;
  pBI->biXPelsPerMeter  = 0;
  pBI->biYPelsPerMeter  = 0;
  pBI->biSizeImage      = nBitmapSize;
  pBI->biClrUsed        = 0;
  pBI->biClrImportant   = 0;

  // get the di bits and the color table
  GetDIBits(hdc, hbmp, 0, bmp.bmHeight, pBits, (BITMAPINFO*)pBI, DIB_RGB_COLORS);

  // write the bitmap to the file
  hfile = _lcreat(szPath, 0);
  if(hfile != HFILE_ERROR)
    {
    if(_hwrite(hfile, pBF, nTotalSize) != nTotalSize)
      {
      fRet = FALSE;
      _lclose(hfile);
      }
    else
      _lclose(hfile);
    }
  else
    fRet = FALSE;

  GlobalUnlock(hgbl);
  GlobalFree(hgbl);
  return fRet;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  cfgCreateIconBitmap
//
// CLASS.......:
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////

HBITMAP cfgCreateIconBitmap(
  HDC         hdc,
  HDC         hdcMem,
  LPFUNCDESC  pFuncDesc)
  {
  HBITMAP hbmp    = CreateCompatibleBitmap(hdc, 32, 32);
  HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcMem, hbmp);
  RECT    rc      = {0, 0, 32, 32};

  FillRect(hdcMem, &rc, GetStockBrush(WHITE_BRUSH));
  pFuncDesc->GetIcons()->DrawColor(hdcMem, 0, 0);
  SelectObject(hdcMem, hbmpOld);

  return hbmp;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  cfgCreateFunctionBitmap
//
// CLASS.......:
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////

HBITMAP cfgCreateFunctionBitmap(
  HDC         hdc,
  HDC         hdcMem,
  LPFUNCDESC  pFuncDesc)

  {
  HBITMAP           hbmp;
  HBITMAP           hbmpOld;
  short             nInputs     = pFuncDesc->GetInputsCount();
  short             nOutputs    = pFuncDesc->GetOutputsCount();
  short             nFlows      = pFuncDesc->GetFlowsCount();

  short             nHeight     = 32 + (nInputs + nOutputs)*12;
  short             nWidth      = 32;
  short             nMaxFlowW   = 0;
  short             nIconTop    = nInputs * 12;

  HFONT             hfontReq    = NULL;
  HFONT             hfontOpt    = NULL;
  HFONT             hfontOld    = NULL;

  LPPARAMDESCARRAY  pParamArray;
  LPFLOWDESCARRAY   pFlowArray;
  LPPARAMDESC       pParamDesc;
  LPFLOWDESC        pFlowDesc;

  SIZE              size;
  RECT              rc;
  int               i;

  LPSTR             szName;

  HDC               hdcMemParam   = CreateCompatibleDC(hdc);
  HBITMAP           hbmpParam     = LoadBitmap(DEVSYSTEM::GetAppInstance(), "PARMARROWMASK");
  HBITMAP           hbmpOldParam  = SelectBitmap(hdcMemParam, hbmpParam);

  if((nInputs + nOutputs + nFlows) > 0)
    {
    hfontReq = CreateFont(12, 0, 0, 0, 400, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH, "Small Fonts");
    hfontOpt = CreateFont(12, 0, 0, 0, 400, 1, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH, "Small Fonts");
    }


  if(nFlows > 0)
    {
    // adjust the width of the bitmap to handle the widest flow name
    hfontOld  = SelectFont(hdcMem, hfontReq);
    pFlowArray = pFuncDesc->GetFlows();
    for(i=0; i<nFlows; i++)
      {
      pFlowDesc  = &(*pFlowArray)[i];
      szName = (LPSTR)pFlowDesc->name;
      GetTextExtentPoint(hdcMem, szName, lstrlen(szName), &size);
      nMaxFlowW = (size.cx > nMaxFlowW) ? size.cx : nMaxFlowW;
      }
    SelectObject(hdcMem, hfontOld);

    nWidth += (80 + nMaxFlowW);

    // adjust the height, if necessary
    if((nIconTop - nFlows * 6 + 14) < 0)
      {
      nIconTop = nFlows * 6 - 14;
      nHeight  = nIconTop + 32 + nOutputs * 12;
      }

    if(nHeight < (nIconTop + 18 + nFlows * 6))
      nHeight = nIconTop + 18 + nFlows * 6;
    }

  // make sure the bitmap is wide enough for the input strings
  if(nInputs > 0)
    {
    int   nMax = 0;

    pParamArray = pFuncDesc->GetInputs();
    for(i=0; i<nInputs; i++)
      {
      pParamDesc = &(*pParamArray)[i];
      szName = (LPSTR)pParamDesc->name;
      hfontOld = SelectFont(hdcMem, pParamDesc->TestOptionalFlag() ? hfontOpt : hfontReq);
      GetTextExtentPoint(hdcMem, szName, lstrlen(szName), &size);
      SelectObject(hdcMem, hfontOld);
      nMax = (size.cx > nMax) ? size.cx : nMax;
      }

    nWidth = ((nMax + 5) > nWidth) ? (nMax + 5) : nWidth;
    }

  // make sure the bitmap is wide enough for the output strings
  if(nOutputs > 0)
    {
    int   nMax = 0;

    pParamArray = pFuncDesc->GetOutputs();
    for(i=0; i<nOutputs; i++)
      {
      pParamDesc = &(*pParamArray)[i];
      szName = (LPSTR)pParamDesc->name;
      hfontOld = SelectFont(hdcMem, pParamDesc->TestOptionalFlag() ? hfontOpt : hfontReq);
      GetTextExtentPoint(hdcMem, szName, lstrlen(szName), &size);
      SelectObject(hdcMem, hfontOld);
      nMax = (size.cx > nMax) ? size.cx : nMax;
      }

    nWidth = ((nMax + 5) > nWidth) ? (nMax + 5) : nWidth;
    }

  // create the bitmap to the correct dimensions
  hbmp    = CreateCompatibleBitmap(hdc, nWidth, nHeight);
  hbmpOld = (HBITMAP)SelectObject(hdcMem, hbmp);

  // draw a white background for the bitmap
  SetRect(&rc, 0, 0, nWidth, nHeight);
  FillRect(hdcMem, &rc, GetStockBrush(WHITE_BRUSH));
  rc.left = 5;

  // draw the inputs
  pParamArray = pFuncDesc->GetInputs();
  for(i=0; i<nInputs; i++)
    {
    rc.top = nIconTop - (nInputs - i) * 12;
    pParamDesc = &(*pParamArray)[i];

    // draw the parameter icon
    BitBlt(hdcMem, 0, rc.top + 2, 4, 8, hdcMemParam, 0, 0, SRCCOPY);

    // draw the parameter text
    hfontOld = SelectFont(hdcMem, pParamDesc->TestOptionalFlag() ? hfontOpt : hfontReq);
    DrawText(hdcMem, (LPSTR)pParamDesc->name, -1, &rc, 0);
    SelectObject(hdcMem, hfontOld);
    }

  // draw the icon
  pFuncDesc->GetIcons()->DrawColor(hdcMem, 0, nIconTop);

  // draw the outputs
  pParamArray = pFuncDesc->GetOutputs();
  for(i=0; i<nOutputs; i++)
    {
    rc.top = nIconTop + 33 + i * 12;
    pParamDesc = &(*pParamArray)[i];

    // draw the parameter icon
    BitBlt(hdcMem, 0, rc.top + 2, 4, 8, hdcMemParam, 0, 0, SRCCOPY);

    // draw the parameter text
    hfontOld = SelectFont(hdcMem, pParamDesc->TestOptionalFlag() ? hfontOpt : hfontReq);
    DrawText(hdcMem, (LPSTR)pParamDesc->name, -1, &rc, 0);
    SelectObject(hdcMem, hfontOld);
    }

  // draw the flows
  if(nFlows>0)
    {
    pFlowArray = pFuncDesc->GetFlows();

    // draw the bounding rectangle
    BitBlt(hdcMem, 32, nIconTop + 12, 4, 8, hdcMemParam, 0, 0, SRCCOPY);
    MoveTo(hdcMem, 32, nIconTop + 15);
    LineTo(hdcMem, 106, nIconTop + 15);
    MoveTo(hdcMem, 32, nIconTop + 16);
    LineTo(hdcMem, 106, nIconTop + 16);
    Rectangle(hdcMem, 106, nIconTop - nFlows * 6 + 14, rc.right, nIconTop + nFlows * 6 + 18);

    // draw each flow name
    hfontOld = SelectFont(hdcMem, hfontReq);
    rc.left = 106;
    for(i=0; i<nFlows; i++)
      {
      rc.top = nIconTop + i * 12 - nFlows * 6 + 16;
      pFlowDesc = &(*pFlowArray)[i];
      DrawText(hdcMem, (LPSTR)pFlowDesc->name, -1, &rc, DT_CENTER);
      }
    SelectObject(hdcMem, hfontOld);
    }

  // free up the allocated resources
  SelectObject(hdcMem, hbmpOld);
  if(hfontReq)
    DeleteObject(hfontReq);
  if(hfontOpt)
    DeleteObject(hfontOpt);
  SelectObject(hdcMemParam, hbmpOldParam);
  DeleteObject(hbmpParam);
  DeleteDC(hdcMemParam);

  return hbmp;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  cfgWriteBitmaps
//
// CLASS.......:
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL cfgWriteFuncBitmaps(
  LPFUNCDESC  pFuncDesc,
  LPCSTR      szDir)

  {
  BOOL              fRet;
  char              szPath[256];
  char              szUID[5];
  COMPUID           uidTemp;
  HDC               hdc           = CreateIC("DISPLAY", NULL, NULL, NULL);
  HDC               hdcMem        = CreateCompatibleDC(hdc);
  HBITMAP           hbmp;
  int               i;
  HFILE             hfile;


  uidTemp = pFuncDesc->GetUID();
  _fmemcpy(szUID, &uidTemp, sizeof(COMPUID));
  szUID[4] = '\0';
  // these two lines swap the outer bytes and inner bytes
  // to reverse the uid character sequence.
  szUID[0]^=szUID[3]^=szUID[0]^=szUID[3];
  szUID[1]^=szUID[2]^=szUID[1]^=szUID[2];

  // get the file name for the icon bitmap
  wsprintf(szPath, "%s\\i_%s.bmp", szDir, szUID);
  i=0;
  while((hfile = _lopen(szPath, READ)) != HFILE_ERROR)
    {
    _lclose(hfile);
    wsprintf(szPath, "%s\\i_%s%02i.bmp", szDir, szUID, i);
    i++;
    }
  // get the icon bitmap and save it
  hbmp = cfgCreateIconBitmap(hdc, hdcMem, pFuncDesc);
  fRet = cfgSaveBitmap(szPath, hdc, hbmp);
  DeleteObject(hbmp);

  // get the file name for the sample function bitmap and save it
  wsprintf(szPath, "%s\\f_%s.bmp", szDir, szUID);
  i=0;
  while((hfile = _lopen(szPath, READ)) != HFILE_ERROR)
    {
    _lclose(hfile);
    wsprintf(szPath, "%s\\f_%s%02i.bmp", szDir, szUID, i);
    i++;
    }
  if(fRet)
    {
    hbmp = cfgCreateFunctionBitmap(hdc, hdcMem, pFuncDesc);
    fRet = cfgSaveBitmap(szPath, hdc, hbmp);
    DeleteObject(hbmp);
    }


  // free up the remaining resources
  DeleteDC(hdcMem);
  DeleteDC(hdc);
  return fRet;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  cfgWriteBitmaps
//
// CLASS.......:
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void cfgWriteBitmaps(
  void)

  {
  LPFILEOBJECT    pFileObj = NULL;
  LPCFGDESC       pCFGDesc;
  LPFUNCDESC      pFuncDesc;
  char            szDir[128];
  char*           szTemp;
  BOOL            fOK;

  szTemp = getenv("BMPS");
  if(!szTemp)
    szTemp = getenv("TEMP");
  lstrcpyn(szDir, szTemp ? szTemp : "c:", 128);

  while (0 != (pFileObj = (LPFILEOBJECT)cfgsList.GetNextMember(pFileObj)))
    {
    pTheSystem->PushContext(pFileObj);
    pCFGDesc = (LPCFGDESC)GetObject(pFileObj->GetHeadRec());

    //Write out Functions
    pFuncDesc = NULL;
    fOK = TRUE;
    while ((0 != (pFuncDesc = pCFGDesc->NextFunctionDesc(pFuncDesc))) && fOK)
      fOK = cfgWriteFuncBitmaps(pFuncDesc, szDir);

    pTheSystem->PopContext();
    }
  }

#endif // #ifdef AW_I_DEBUG
