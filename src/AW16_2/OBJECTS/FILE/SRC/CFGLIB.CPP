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
#include "fnameobj.h"
#include "utility.h"
#include "regdef.h"
#include "a_almutl.h"

static LIST	         cfgsList;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:	cfgGetDatabaseVersion
//
// DESCRIPTION.:  Returns the database version of a file.  Assumes that the
//                file is a valid serius config file of some database
//                version.
//
/////////////////////////////////////////////////////////////////////////////
WORD cfgGetDatabaseVersion(
	HANDLE 		hFile)
	{
	FILEIDINFO	idInfo;
	DWORD				nBytesRead;

	ReadFile(hFile, &idInfo, sizeof(idInfo), &nBytesRead, NULL);
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

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
static char CFGPREF_CONFIGFILES[] = {REG_CONFIGFILES};

void cfgReadConfigs(
	void)
	{
	int						nTmpSize			= SFilePath::MaxLength();
	SFilePath			objectsDir;
	char*					pszTmp;
	HANDLE				hFileFind;
	int						nPathChars;
	HANDLE				hFile;
	LPFILEOBJECT	fileObject;
	TSTRING				savePath;
  WORD          dbVersion;
	WIN32_FIND_DATA	fileInfo;
	BOOL					fContinue;
  HKEY					dirKey = AUtlOpenProductSubKey(HKEY_CURRENT_USER, AUTL_KEY_DIRECTORIES);
  HKEY					configKey=NULL;

	pszTmp = new char[nTmpSize];

	nPathChars = AUtlGetKeyString(dirKey, CFGPREF_CONFIGFILES,
	 pszTmp, nTmpSize,".");

	if(dirKey)
  	AUtlCloseProductSubKey(dirKey);

	if (nPathChars >=	nTmpSize)
		{
		MessageBox(NULL, "Directory path for config files is too long.",
		 "Info", MB_OK);
		objectsDir = ".";
		}
	else
		objectsDir = pszTmp;

	savePath = (const char*) objectsDir;

	objectsDir.AddToEnd("*.cfg");

	hFileFind = FindFirstFile(objectsDir, &fileInfo);
	fContinue = (hFileFind != INVALID_HANDLE_VALUE);

	while (fContinue)
		{
		objectsDir.SetFullName(fileInfo.cFileName);

		hFile = CreateFile(objectsDir, GENERIC_READ, 0, NULL,
												OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFile != INVALID_HANDLE_VALUE)
			{
			if (cfgCheckConfigFile(hFile, fileInfo.cFileName))
				{
        dbVersion = cfgGetDatabaseVersion(hFile);
        if (dbVersion == DATABASE_VERSION)
          {
				  fileObject = new FILEOBJECT(hFile, savePath, fileInfo.cFileName);
					cfgsList.Add(fileObject);
          }
        else
          {
          fileObject = NULL;
					MessageBox(GetActiveWindow(), "Invalid configuration file.",
										"Read Config File", MB_OK);
          }
				}
			CloseHandle(hFile);
			}
		fContinue = FindNextFile(hFileFind, &fileInfo);
		}

	if (hFileFind != INVALID_HANDLE_VALUE)
		FindClose(hFileFind);

	delete pszTmp;
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
	HANDLE	hFile,
	LPCSTR	filePath)
	{
	DWORD					status;
	DWORD					bytesRead;
	FILEIDINFO		idInfo;

	ReadFile(hFile, &idInfo, sizeof(idInfo), &bytesRead, NULL);

	if (bytesRead < sizeof(idInfo))
		status = FILESTAT_SHORT;
	else if (lstrcmp(idInfo.idString, ID_STRING ) != 0)
		status = FILESTAT_NOTSERIUSFILE;
	else if (idInfo.fileType != FILETYPE_CONFIG)
		status = FILESTAT_WRONGTYPE;
	else
		status = FILESTAT_OK;

	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

	if (status & FILESTAT_OK)
		return idInfo.databaseVersion;
	else
		{
		SFormattedMsg	msg;

		msg.Format("%s is not a valid configuration file", filePath);
		MessageBeep(0);
		MessageBox(NULL, msg, "Open File Error", 
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
	int						itemID;

	while (0 != (currFileObj = (LPFILEOBJECT)cfgsList.GetNextMember(currFileObj)))
		{
		pTheSystem->PushContext(currFileObj);
		currCFGDesc = (LPCFGDESC)GetObject(currFileObj->GetHeadRec());
		currObjDesc = NULL;
		while (0 != (currObjDesc = currCFGDesc->NextObjectDesc(currObjDesc)))
			{
			itemID = ComboBox_AddString(hComboBox, (LPCSTR) currObjDesc->GetName());
			 
			ComboBox_SetItemData(hComboBox, itemID, currObjDesc->GetUID());
			}
		pTheSystem->PopContext();
		}

	if (pszGeneralText == NULL)
		pszGeneralText = "General";

	ComboBox_InsertString(hComboBox, 0, pszGeneralText);
	ComboBox_SetItemData(hComboBox, itemID, COMPUID_GENERAL);
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
	int						itemID;

	while (0 != (currFileObj = (LPFILEOBJECT)cfgsList.GetNextMember(currFileObj)))
		{
		pTheSystem->PushContext(currFileObj);
		currCFGDesc = (LPCFGDESC)GetObject(currFileObj->GetHeadRec());
		currEventDesc = NULL;
		while (0 != (currEventDesc = currCFGDesc->NextEventDesc(currEventDesc)))
			{
			itemID = ComboBox_AddString(hComboBox, (LPCSTR)currEventDesc->GetName());
			ComboBox_SetItemData(hComboBox, itemID, currEventDesc->GetUID());
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
	HANDLE				hFile,
	LPFUNCDESC		pFuncDesc)
	{
	LPPARAMDESCARRAY  pParamArray;
	LPPARAMDESC       pParam;
	LPFLOWDESCARRAY   pFlowArray;
	LPFLOWDESC        pFlow;
	char							buffer[128];
	int               paramCount;
	int               flowCount;
	char              uidBuffer[8];
	char              label[9];
	int               i;
	COMPUID           tempUID;
	DWORD							nBytesWritten;

	tempUID = pFuncDesc->GetUID();

	CopyMemory(uidBuffer, &tempUID, sizeof(COMPUID));
	uidBuffer[4] = '\0';
	// these two lines swap the outer bytes and inner bytes
	// to reverse the uid character sequence.
	uidBuffer[0]^=uidBuffer[3]^=uidBuffer[0]^=uidBuffer[3];
	uidBuffer[1]^=uidBuffer[2]^=uidBuffer[1]^=uidBuffer[2];

	wsprintf(buffer, "  Function: %-20.20s  %-7.7s\r\n",
	 (LPCSTR)pFuncDesc->GetName(), (LPSTR)uidBuffer);

	WriteFile(hFile, buffer, lstrlen(buffer), &nBytesWritten, NULL);

	//write input parameters

	pParamArray = pFuncDesc->GetInputs();
	paramCount = pParamArray->GetCount();

	lstrcpy(label, "Inputs:");

	if (paramCount == 0)
		{
		wsprintf(buffer, "  %-10.10snone\r\n", label);
		WriteFile(hFile, buffer, lstrlen(buffer), &nBytesWritten, NULL);
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
				CopyMemory(uidBuffer, &pParam->type, sizeof(COMPUID));
				uidBuffer[4] = '\0';
				// these two lines swap the outer bytes and inner bytes
				// to reverse the uid character sequence.
				uidBuffer[0]^=uidBuffer[3]^=uidBuffer[0]^=uidBuffer[3];
				uidBuffer[1]^=uidBuffer[2]^=uidBuffer[1]^=uidBuffer[2];
				}

			wsprintf(buffer, "  %-10.10s%-20.20s  %-7.7s  %-8.8s  %-8.8s\r\n",
				label,
				(LPCSTR) pParam->name,
				uidBuffer,
				pParam->TestNoTempObjectsFlag()?" ":"TEMPS",
				pParam->TestOptionalFlag()?"OPTIONAL":" ");
			WriteFile(hFile, buffer, lstrlen(buffer), &nBytesWritten, NULL);
			lstrcpy(label, " ");
			}
		}

	// write output parameters

	pParamArray = pFuncDesc->GetOutputs();
	paramCount = pParamArray->GetCount();

	lstrcpy(label, "Outputs:");

	if (paramCount == 0)
		{
		wsprintf(buffer, "  %-10.10snone\r\n", label);
		WriteFile(hFile, buffer, lstrlen(buffer), &nBytesWritten, NULL);
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
				CopyMemory(uidBuffer, &pParam->type, sizeof(COMPUID));
				uidBuffer[4] = '\0';
				// these two lines swap the outer bytes and inner bytes
				// to reverse the uid character sequence.
				uidBuffer[0]^=uidBuffer[3]^=uidBuffer[0]^=uidBuffer[3];
				uidBuffer[1]^=uidBuffer[2]^=uidBuffer[1]^=uidBuffer[2];
				}

			wsprintf(buffer, "  %-10.10s%-20.20s  %-7.7s  %-8.8s  %-8.8s\r\n",
				label,
				(LPCSTR) pParam->name,
				uidBuffer,
				pParam->TestNoTempObjectsFlag()?" ":"TEMPS",
				pParam->TestOptionalFlag()?"OPTIONAL":" ");
			WriteFile(hFile, buffer, lstrlen(buffer), &nBytesWritten, NULL);
			lstrcpy(label, " ");
			}
		}

	//write flows

	pFlowArray = pFuncDesc->GetFlows();
	flowCount = pFlowArray->GetCount();

	lstrcpy(label, "Flows:");

	if (flowCount == 0)
		{
		wsprintf(buffer, "  %-10.10snone\r\n", label);
		WriteFile(hFile, buffer, lstrlen(buffer), &nBytesWritten, NULL);
		}
	else
		{
		for (i=0; i<flowCount; i++)
			{
			pFlow = &(*pFlowArray)[i];

			wsprintf(buffer, "  %-10.10s%s\r\n",
				label,
				(LPCSTR) pFlow->name);
			WriteFile(hFile, buffer, lstrlen(buffer), &nBytesWritten, NULL);
			lstrcpy(label, " ");
			}
		}

	//write DLL and Entry Point

	wsprintf(buffer, "  DLL:      %-12.12s  Entry Point: %s\r\n",
	 pFuncDesc->GetLibName(), pFuncDesc->GetEntryPointName());
	WriteFile(hFile, buffer, lstrlen(buffer), &nBytesWritten, NULL);


	wsprintf(buffer, "\r\n");
	WriteFile(hFile, buffer, lstrlen(buffer), &nBytesWritten, NULL);
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
	HANDLE			hFile,
	LPOBJDESC		pObjDesc,
	char*				buffer)
	{
	char              uidBuffer[8];
	COMPUID           tempUID;
	DWORD							nBytesWritten;

	tempUID = pObjDesc->GetUID();

	CopyMemory(uidBuffer, &tempUID, sizeof(COMPUID));
	uidBuffer[4] = '\0';
	// these two lines swap the outer bytes and inner bytes
	// to reverse the uid character sequence.
	uidBuffer[0]^=uidBuffer[3]^=uidBuffer[0]^=uidBuffer[3];
	uidBuffer[1]^=uidBuffer[2]^=uidBuffer[1]^=uidBuffer[2];

	wsprintf(buffer, "  Object:   %-20.20s  %-7.7s\r\n",
	 (LPCSTR)pObjDesc->GetName(), (LPSTR)uidBuffer);
	WriteFile(hFile, buffer, lstrlen(buffer), &nBytesWritten, NULL);
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
	HANDLE				hFile)
	{
	LPFILEOBJECT	pFileObj = NULL;
	LPCFGDESC			pCFGDesc;
	LPEVENTDESC		pEventDesc;
	LPFUNCDESC    pFuncDesc;
	LPOBJDESC     pObjDesc;
	SFormattedMsg	msg;
	BOOL          atLeastOne;
	DWORD					nBytesWritten;

	while (0 != (pFileObj = (LPFILEOBJECT)cfgsList.GetNextMember(pFileObj)))
		{
		pTheSystem->PushContext(pFileObj);
		pCFGDesc = (LPCFGDESC)GetObject(pFileObj->GetHeadRec());

		msg.Format("Config File: %s%s\r\n\r\n", (LPCSTR) pFileObj->GetFilePath(),
		 (LPCSTR) pFileObj->GetName());

		WriteFile(hFile, msg, msg.Length(), &nBytesWritten, NULL);

		//Write out Objects

		msg.Format("Objects:\r\n");
		WriteFile(hFile, msg, msg.Length(), &nBytesWritten, NULL);
		atLeastOne = FALSE;
		pObjDesc = NULL;
		while (0 != (pObjDesc = pCFGDesc->NextObjectDesc(pObjDesc)))
			{
			atLeastOne = TRUE;
			}
		if (atLeastOne == FALSE)
			{
			msg.Format("    none\r\n\r\n");
			WriteFile(hFile, msg, msg.Length(), &nBytesWritten, NULL);
			}

		//Write out Functions

		msg.Format("Functions:\r\n");
		WriteFile(hFile, msg, msg.Length(), &nBytesWritten, NULL);
		atLeastOne = FALSE;
		pFuncDesc = NULL;
		while (0 != (pFuncDesc = pCFGDesc->NextFunctionDesc(pFuncDesc)))
			{
			atLeastOne = TRUE;
			cfgWriteFunction(hFile, pFuncDesc);
			}
		if (atLeastOne == FALSE)
			{
			msg.Format("    none\r\n\r\n");
			WriteFile(hFile, msg, msg.Length(), &nBytesWritten, NULL);
			}

		//Write out Events

		msg.Format("Events:\r\n");
		WriteFile(hFile, msg, msg.Length(), &nBytesWritten, NULL);
		atLeastOne = FALSE;
		pEventDesc = NULL;
		while (0 != (pEventDesc = pCFGDesc->NextEventDesc(pEventDesc)))
			{
			atLeastOne = TRUE;
			}
		if (atLeastOne == FALSE)
			{
			msg.Format("    none\r\n\r\n");
			WriteFile(hFile, msg, msg.Length(), &nBytesWritten, NULL);
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
  void *						pBits;
  long              nBitmapSize;
  int		            nColorBits;
  long              nColorTableSize;
  long              nTotalSize;
  HGLOBAL           hgbl;
  HANDLE            hFile;
	DWORD							nWritten;

  GetObject(hbmp, sizeof(BITMAP), (LPSTR)&bmp);
  nColorBits      = bmp.bmBitsPixel * bmp.bmPlanes;
  nColorTableSize = (nColorBits<24) ? (0x00000001L << nColorBits) : 0;
  nBitmapSize     = (((DWORD)bmp.bmWidth * nColorBits + 31)/8) * bmp.bmHeight;
  nTotalSize      = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
                    (nColorTableSize * sizeof(RGBQUAD)) + nBitmapSize;

  hgbl = GlobalAlloc(GPTR, nTotalSize);
  pBF   = (BITMAPFILEHEADER*)GlobalLock(hgbl);
  pBI   = (BITMAPINFOHEADER*)((char*)pBF + sizeof(BITMAPFILEHEADER));
  pBits = (void *)((char *)pBI + (sizeof(BITMAPINFOHEADER) + (nColorTableSize * sizeof(RGBQUAD))));

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
	hFile = CreateFile(szPath, GENERIC_WRITE, 0, NULL,
										CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
  if (hFile != INVALID_HANDLE_VALUE)
    {
		WriteFile(hFile, pBF, nTotalSize, &nWritten, NULL);

    if (nWritten != nTotalSize)
      fRet = FALSE;

		CloseHandle(hFile);
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
  int		            nInputs     = pFuncDesc->GetInputsCount();
  int		            nOutputs    = pFuncDesc->GetOutputsCount();
  int		            nFlows      = pFuncDesc->GetFlowsCount();

  int		            nHeight     = 32 + (nInputs + nOutputs)*12;
  int		            nWidth      = 32;
  int		            nMaxFlowW   = 0;
  int		            nIconTop    = nInputs * 12;

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

      LPCSTR szName = pFlowDesc->name;

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

      LPCSTR szName = pParamDesc->name;

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

      LPCSTR szName = pParamDesc->name;

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
    DrawText(hdcMem, pParamDesc->name, -1, &rc, 0);
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
    DrawText(hdcMem, pParamDesc->name, -1, &rc, 0);
    SelectObject(hdcMem, hfontOld);
    }

  // draw the flows
  if(nFlows>0)
    {
    pFlowArray = pFuncDesc->GetFlows();

    // draw the bounding rectangle
    BitBlt(hdcMem, 32, nIconTop + 12, 4, 8, hdcMemParam, 0, 0, SRCCOPY);
    MoveToEx(hdcMem, 32, nIconTop + 15, NULL);
    LineTo(hdcMem, 106, nIconTop + 15);
    MoveToEx(hdcMem, 32, nIconTop + 16, NULL);
    LineTo(hdcMem, 106, nIconTop + 16);
    Rectangle(hdcMem, 106, nIconTop - nFlows * 6 + 14, rc.right, nIconTop + nFlows * 6 + 18);

    // draw each flow name
    hfontOld = SelectFont(hdcMem, hfontReq);
    rc.left = 106;
    for(i=0; i<nFlows; i++)
      {
      rc.top = nIconTop + i * 12 - nFlows * 6 + 16;
      pFlowDesc = &(*pFlowArray)[i];
      DrawText(hdcMem, pFlowDesc->name, -1, &rc, DT_CENTER);
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
	SFilePath					path;
	char							szTmp[32];		// holds temp file name
  char              szUID[5];
  COMPUID           uidTemp;
  HDC               hdc           = CreateIC("DISPLAY", NULL, NULL, NULL);
  HDC               hdcMem        = CreateCompatibleDC(hdc);
  HBITMAP           hbmp;
  int               i;
  HANDLE						hFile;


  uidTemp = pFuncDesc->GetUID();
  CopyMemory(szUID, &uidTemp, sizeof(COMPUID));
  szUID[4] = '\0';
  // these two lines swap the outer bytes and inner bytes
  // to reverse the uid character sequence.
  szUID[0]^=szUID[3]^=szUID[0]^=szUID[3];
  szUID[1]^=szUID[2]^=szUID[1]^=szUID[2];

  // get the file name for the icon bitmap
	path = szDir;
	wsprintf(szTmp, "i_%s.bmp", szUID);
	path.SetFullName(szTmp);
  i=0;
  while((hFile = CreateFile(path, GENERIC_READ, 0, NULL,
														OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0))
									!= INVALID_HANDLE_VALUE)
    {
    CloseHandle(hFile);
		
    wsprintf(szTmp, "i_%s%02i.bmp", szUID, i);
		path.SetFullName(szTmp);
    i++;
    }
  // get the icon bitmap and save it
  hbmp = cfgCreateIconBitmap(hdc, hdcMem, pFuncDesc);
  fRet = cfgSaveBitmap(path, hdc, hbmp);
  DeleteObject(hbmp);

  // get the file name for the sample function bitmap and save it
  wsprintf(szTmp, "f_%s.bmp", szUID);
	path.SetFullName(szTmp);
  i=0;
  while((hFile = CreateFile(path, GENERIC_READ, 0, NULL,
														OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0))
									!= INVALID_HANDLE_VALUE)
    {
    CloseHandle(hFile);
    wsprintf(szTmp, "f_%s%02i.bmp", szUID, i);
		path.SetFullName(szTmp);
    i++;
    }
  if(fRet)
    {
    hbmp = cfgCreateFunctionBitmap(hdc, hdcMem, pFuncDesc);
    fRet = cfgSaveBitmap(path, hdc, hbmp);
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
	int							nTmpSize					= SFilePath::MaxLength();
  LPFILEOBJECT    pFileObj					= NULL;
	char*						pszTmp;
	SFilePath				path;
  LPCFGDESC       pCFGDesc;
  LPFUNCDESC      pFuncDesc;
  BOOL            fOK;

	pszTmp = new char[nTmpSize];

	if ((GetEnvironmentVariable("BMPS", pszTmp, nTmpSize) == 0) &&
			(GetEnvironmentVariable("TEMP", pszTmp, nTmpSize) == 0))
		path.MakeFromCurrentDirectory();
	else
		path = pszTmp;

  while (0 != (pFileObj = (LPFILEOBJECT)cfgsList.GetNextMember(pFileObj)))
    {
    pTheSystem->PushContext(pFileObj);
    pCFGDesc = (LPCFGDESC)GetObject(pFileObj->GetHeadRec());

    //Write out Functions
    pFuncDesc = NULL;
    fOK = TRUE;
    while ((0 != (pFuncDesc = pCFGDesc->NextFunctionDesc(pFuncDesc))) && fOK)
      fOK = cfgWriteFuncBitmaps(pFuncDesc, pszTmp);

    pTheSystem->PopContext();
    }

	delete pszTmp;
  }

#endif // #ifdef AW_I_DEBUG
