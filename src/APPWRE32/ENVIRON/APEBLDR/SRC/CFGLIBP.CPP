#include "almsys.h"

#include "cfglibp.h"
#include "version.h"
#include "fileobj.h"
#include "devsys.h"
#include "list.h"
#include "compuid.h"
#include "sereclib.h"
#include "cfgdesc.h"
#include "funcdesc.h"
#include "objdesc.h"
#include "objinst.h"
#include "fnameobj.h"
#include "utility.h"
#include "a_almutl.h"
#include "regdef.h"

// holds the list of all opened configuration files.
static LIST	cfgsList;

#ifdef ADBG_DEBUG      //define code for dumping config info

/////////////////////////////////////////////////////////////////////////////
// CLASS....: SORTEDLIST
//
// DESCRIPTION.: This class is used simply to create a sorted list of
//               open configuration files.  Its is not intended to be a
//               a generalized sorted list manager.
//
/////////////////////////////////////////////////////////////////////////////
class SORTEDLIST:public MEMORY
  {

  struct LISTMEMBER:public MEMORY
    {
    LISTMEMBER *  next;
    LPSTR            key;
    long             data;

    LISTMEMBER(void)
      {
      next = NULL;
      key = NULL;
      data = 0;
      }

    };

private:
  LISTMEMBER *    m_head;

public:
  SORTEDLIST(void);

  ~SORTEDLIST(void);

  void Insert(
    LPSTR  pKey,
    long   data);

  long GetNextData(
     void *    &currRec,
     long         &data);

  };

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SORTEDLIST::SORTEDLIST
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
SORTEDLIST::SORTEDLIST(void)
  {
  m_head = NULL;
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SORTEDLIST::~SORTEDLIST
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
SORTEDLIST::~SORTEDLIST(void)
  {
  LISTMEMBER *  temp;

  while (m_head != NULL)
    {
    temp = m_head;
    m_head = m_head->next;
    delete temp;
    }
  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SORTEDLIST::Insert
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void SORTEDLIST::Insert(
  LPSTR  pKey,
  long   data)
  {
  LISTMEMBER *  newMember;
  LISTMEMBER *  currRec;
  LISTMEMBER *  prevRec;

  newMember = new LISTMEMBER();
  newMember->key = pKey;
  newMember->data = data;

  if (m_head == NULL)
    m_head = newMember;
  else
    {
    currRec = m_head;
    prevRec = 0;
    while((currRec != 0) &&
          (lstrcmp(currRec->key, newMember->key) >= 0))
      {
      prevRec = currRec;
      currRec = currRec->next;
      }
    if (prevRec == 0)
      {
      newMember->next = m_head;
      m_head = newMember;
      }
    else
      {
      newMember->next = prevRec->next;
      prevRec->next = newMember;
      }
    }

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: SORTEDLIST::GetNextData
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
long SORTEDLIST::GetNextData(
   void *    &currRec,
   long         &data)
   {
   LISTMEMBER *  nextRec;
   BOOL             result;

   if (currRec == NULL)
     nextRec = m_head;
   else
     nextRec = ((LISTMEMBER *)currRec)->next;

   if (nextRec != NULL)
     {
     data = nextRec->data;
     currRec = nextRec;
     result = TRUE;
     }
   else
     {
     currRec = 0;
     data = 0;
     result = FALSE;
     }

   return result;

   }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DumpThisFunction
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void DumpThisFunction(
  HANDLE         hFile,
  LPSTR       buffer,
  LPAFUNCDESC  pFuncDesc)
  {
	char              uidBuffer[9];
  char              nameBuf[81];
	int               i;
	COMPUID           tempUID;
  LPPARAMDESC       pParam;
	LPFLOWDESC        pFlow;
	int               count;
  DWORD							bc;

	tempUID = pFuncDesc->GetUID();

	CopyMemory(uidBuffer, &tempUID, (DWORD)sizeof(COMPUID));
	uidBuffer[4] = '\0';
	// these two lines swap the outer bytes and inner bytes
	// to reverse the uid character sequence.
	uidBuffer[0]^=uidBuffer[3]^=uidBuffer[0]^=uidBuffer[3];
	uidBuffer[1]^=uidBuffer[2]^=uidBuffer[1]^=uidBuffer[2];

	wsprintf(buffer, "Function:%s:%s\r\n",
   pFuncDesc->pGetName(), (LPSTR)uidBuffer);

	I_VERIFY(WriteFile(hFile, buffer, lstrlen(buffer), &bc, NULL));

	//write input parameters

	count = pFuncDesc->GetInputsCount();

	for (i=0; i<count; i++)
		{
		pParam = pFuncDesc->GetInput(i);
		if (pParam->type == COMPUID_GENERAL)
			lstrcpy(uidBuffer, "untyped");
		else
			{
			CopyMemory(uidBuffer, &pParam->type, (DWORD)sizeof(COMPUID));
			uidBuffer[4] = '\0';
			// these two lines swap the outer bytes and inner bytes
			// to reverse the uid character sequence.
			uidBuffer[0]^=uidBuffer[3]^=uidBuffer[0]^=uidBuffer[3];
			uidBuffer[1]^=uidBuffer[2]^=uidBuffer[1]^=uidBuffer[2];
			}

    lstrcpy(nameBuf,(LPSTR)pParam->name.pString);
    AnsiUpper(nameBuf);

		wsprintf(buffer, "Input:%s:%s:%s:%s\r\n",
			(LPSTR)nameBuf,
			(LPSTR)uidBuffer,
			pParam->TestOptionalFlag()?(LPSTR)"Optional":(LPSTR)"Not Optional",
			pParam->TestNoTempObjectsFlag()?(LPSTR)"Not Pseudo":(LPSTR)"Pseudo");

		WriteFile(hFile, buffer, lstrlen(buffer), &bc, NULL);
		}

	//write output parameters

	count = pFuncDesc->GetOutputsCount();

	for (i=0; i<count; i++)
		{
		pParam = pFuncDesc->GetOutput(i);
		if (pParam->type == COMPUID_GENERAL)
			lstrcpy(uidBuffer, "untyped");
		else
			{
			CopyMemory(uidBuffer, &pParam->type, (DWORD)sizeof(COMPUID));
			uidBuffer[4] = '\0';
			// these two lines swap the outer bytes and inner bytes
			// to reverse the uid character sequence.
			uidBuffer[0]^=uidBuffer[3]^=uidBuffer[0]^=uidBuffer[3];
			uidBuffer[1]^=uidBuffer[2]^=uidBuffer[1]^=uidBuffer[2];
			}

    lstrcpy(nameBuf,(LPSTR)pParam->name.pString);
    AnsiUpper(nameBuf);

		wsprintf(buffer, "Output:%s:%s:%s:%s\r\n",
			(LPSTR)nameBuf,
			(LPSTR)uidBuffer,
			pParam->TestOptionalFlag()?(LPSTR)"Optional":(LPSTR)"Not Optional",
			pParam->TestNoTempObjectsFlag()?(LPSTR)"Not Pseudo":(LPSTR)"Pseudo");

		WriteFile(hFile, buffer, lstrlen(buffer), &bc, NULL);
		}

  //Write Flows

	count = pFuncDesc->GetFlowsCount();

	for (i=0; i<count; i++)
		{
		pFlow = pFuncDesc->GetFlow(i);

    lstrcpy(nameBuf, (LPSTR)pFlow->name.pString);
    AnsiUpper(nameBuf);

		wsprintf(buffer, "Flow:%s:%d\r\n", (LPSTR)nameBuf, i+1);
		WriteFile(hFile, buffer, lstrlen(buffer), &bc, NULL);
		}

	wsprintf(buffer, "\r\n");
	WriteFile(hFile, buffer, lstrlen(buffer), &bc, NULL);


  }
/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DumpSignals
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void DumpSignals(
  HANDLE     hFile,
  LPSTR      buffer,
  LPOBJDESC  pObjDesc)
  {
	LPOBJINST         objPtr;
	AMEMBLOCKID        signalBlock;
	pAObjSignalRecord signals;
	LONG              signalSize;
	UINT       i;
	long              maxSignals;
  AOBJECTID          objID;
  long              getKey;
  DWORD							bc;

  getKey = AOBJ_GETFIRSTKEY;

  objID = AObjGetNextObject(pObjDesc->GetUID(), &getKey);

  if (objID)
    {
 	  objPtr = (LPOBJINST)GetObject((SERUID)objID);
		signalBlock = AMemAllocate(0);
		objPtr->GetSignals((RUNTIMEUID)signalBlock, AOBJ_BUILDSIGNALS);
		signals = (pAObjSignalRecord)AMemLock(signalBlock);
		signalSize = AMemGetSize(signalBlock);
		maxSignals = signalSize / sizeof(AObjSignalRecord);
		for(i=0 ; i < maxSignals ; i++)
			{
//      AnsiUpper(signals[i].theName);
	    wsprintf(buffer, "Signal:%s:%d\r\n",
       (LPSTR)signals[i].theName, signals[i].theID);
	    WriteFile(hFile, buffer, lstrlen(buffer), &bc, NULL);
      }
    AMemUnlock(signalBlock);
    AMemFree(signalBlock);
    }

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DumpThisObject
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void DumpThisObject(
  HANDLE        hFile,
  LPSTR      buffer,
  LPOBJDESC  pObjDesc)
  {
	char              uidBuffer[sizeof(COMPUID) + 1];
	COMPUID           tempUID;
	DWORD							bc;
  
	tempUID = pObjDesc->GetUID();

	CopyMemory(uidBuffer, &tempUID, (DWORD)sizeof(COMPUID));
	uidBuffer[4] = '\0';
	// these two lines swap the outer bytes and inner bytes
	// to reverse the uid character sequence.
	uidBuffer[0]^=uidBuffer[3]^=uidBuffer[0]^=uidBuffer[3];
	uidBuffer[1]^=uidBuffer[2]^=uidBuffer[1]^=uidBuffer[2];

	wsprintf(buffer, "Object:%s:%s\r\n",
   pObjDesc->pGetName(), (LPSTR)uidBuffer);

	WriteFile(hFile, buffer, lstrlen(buffer), &bc, NULL);

  DumpSignals(hFile, buffer, pObjDesc);

	wsprintf(buffer, "\r\n");
	WriteFile(hFile, buffer, lstrlen(buffer), &bc, NULL);

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: DumpThisConfig
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
static void DumpThisConfig(
  HANDLE       hFile,
  LPCFGDESC pCfgDesc)
  {
  char             buffer[100];
  SORTEDLIST *  pList;
	LPOBJDESC		     pObjDesc;
  LPAFUNCDESC       pFuncDesc;
  void  *       pNode;
  long             data;
  DWORD					bc;

	wsprintf(buffer, "File:%s\r\n", pCfgDesc->pGetFuncGroupName());
	WriteFile(hFile, buffer, lstrlen(buffer), &bc, NULL);


	wsprintf(buffer, "\r\n");
	WriteFile(hFile, buffer, lstrlen(buffer), &bc, NULL);

  //sort and dump objects

  pList = new SORTEDLIST();

	pObjDesc = NULL;
	while (0 != (pObjDesc = pCfgDesc->pNextObjectDesc(pObjDesc)))
    {
    pList->Insert(pObjDesc->pGetName(), (long)pObjDesc);
    }

  pNode = 0;
  while (pList->GetNextData(pNode, data))
    DumpThisObject(hFile, buffer, (LPOBJDESC)data);

  delete pList;

  //sort and dump objects

  pList = new SORTEDLIST();

	pFuncDesc = NULL;
	while (0 != (pFuncDesc = pCfgDesc->pNextFunctionDesc(pFuncDesc)))
    {
    pList->Insert(pFuncDesc->pGetName(), (long)pFuncDesc);
    }

  pNode = 0;
  while (pList->GetNextData(pNode, data))
    DumpThisFunction(hFile, buffer, (LPAFUNCDESC)data);

  delete pList;

  }

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: cfgDumpConfigInfo
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void cfgDumpConfigInfo(
  HANDLE hFile)
  {
  SORTEDLIST *  sortedConfigs;
	LPFILEOBJECT	   pFileObj;
	LPCFGDESC			   pCFGDesc;
  long             data;
  void  *       pNode;

  sortedConfigs = new SORTEDLIST();

  pFileObj = NULL;

	while (0 != (pFileObj = (LPFILEOBJECT)cfgsList.GetNextMember(pFileObj)))
    {
    pCFGDesc = (LPCFGDESC)pFileObj->GetHeadRecPtr();
    sortedConfigs->Insert(pCFGDesc->pGetFuncGroupName(), (long)pCFGDesc);
    }

  pNode = 0;
  while (sortedConfigs->GetNextData(pNode, data))
    DumpThisConfig(hFile, (LPCFGDESC)data);

  delete sortedConfigs;

  }
#endif  //#ifdef ADBG_DEBUG,  end of code for dumping config info


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
	int						nPathChars;
	HANDLE				hFile;
	LPFILEOBJECT	fileObject;
  HANDLE				firstFile;
  BOOL					doLoop;
	TSTRING				savePath;
	WIN32_FIND_DATA wfd;
  HKEY					dirKey = AUtlOpenProductSubKey(HKEY_CURRENT_USER, AUTL_KEY_DIRECTORIES);
//  HKEY					configKey=NULL;


	pszTmp = new char[nTmpSize];

	nPathChars = AUtlGetKeyString(dirKey, CFGPREF_CONFIGFILES,
	 pszTmp, (WORD)nTmpSize,".");

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

	objectsDir.AddToEnd("*.alm");

  firstFile = FindFirstFile(objectsDir, &wfd);
  doLoop = (firstFile != INVALID_HANDLE_VALUE)?TRUE:FALSE;

	while (doLoop)
		{
		objectsDir.SetFullName(wfd.cFileName);

		hFile = CreateFile(objectsDir, GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
			{
			if (cfgCheckConfigFile(hFile, wfd.cFileName))
				{
				fileObject = new FILEOBJECT(hFile, savePath, wfd.cFileName);
				cfgsList.Add(fileObject);
				((LPCFGDESC)GetObject(fileObject->GetHeadRec()))->
				 ConvertToPointers();
				fileObject->ConvertToPointers();
				}

				I_VERIFY(CloseHandle(hFile));
			}
			
		if(!FindNextFile(firstFile, &wfd))
    	{
      if(GetLastError()	!= ERROR_NO_MORE_FILES)
      	{
        ADBG_PRINT_E("File error reading in config files.");
        }
        
      doLoop = FALSE;
      }
		}

	if(firstFile != INVALID_HANDLE_VALUE)
  	I_VERIFY(FindClose(firstFile));
  	
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
	LPSTR   fileName)
	{
	DWORD					status;
	DWORD					bytesRead;
	FILEIDINFO		idInfo;
	SFormattedMsg	msg;

	if(ReadFile(hFile, (LPSTR)&idInfo, sizeof(FILEIDINFO), &bytesRead, NULL))
  	{
		if (bytesRead < sizeof(FILEIDINFO))
			status = FILESTAT_SHORT;
		else if (lstrcmp(idInfo.idString, ID_STRING ) != 0)
			status = FILESTAT_NOTSERIUSFILE;
		else if (idInfo.fileType != FILETYPE_CONFIG)
			status = FILESTAT_WRONGTYPE;
		else if (idInfo.databaseVersion  != DATABASE_VERSION)
			status = FILESTAT_OLDDB;
		else
			status = FILESTAT_OK;

		I_VERIFY(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) != 0xffffffff);
  	}
  else
  	status = FILESTAT_SHORT; //didn't read any bytes

	if (status & FILESTAT_OK)
		return TRUE;
	else
		{
		msg.Format("%s is not a valid configuration file.", fileName);
		MessageBeep(0);
		MessageBox(NULL, msg, "Open File Error", 
			MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
		return FALSE;
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
// FUNCTION....: cfgGetNextObjDesc
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
		nextObjDesc = currCFGDesc->pNextObjectDesc(currObjDesc);
		}

	if (nextObjDesc == NULL)
		while (0 != (currFileObj = (LPFILEOBJECT)cfgsList.GetNextMember(currFileObj))
              &&
					 0 != (currCFGDesc = (LPCFGDESC)currFileObj->GetHeadRecPtr()))
			{
			nextObjDesc = currCFGDesc->pNextObjectDesc(NULL);
			if (nextObjDesc)
				break;
			}

	return nextObjDesc;

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
		nextEventDesc = currCFGDesc->pNextEventDesc(currEventDesc);
		}

	if (nextEventDesc == NULL)
		while (0 != (currFileObj = (LPFILEOBJECT)cfgsList.GetNextMember(currFileObj))
              &&
					 0 != (currCFGDesc = (LPCFGDESC)currFileObj->GetHeadRecPtr()))
			{
			nextEventDesc = currCFGDesc->pNextEventDesc(NULL);
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
// FUNCTION....: cfgGetObjectDesc
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
LPOBJDESC cfgGetObjectDesc(
	COMPUID	objectType)
	{
	LPFILEOBJECT	currFileObj = NULL;
	LPCFGDESC			currCFGDesc;
	LPOBJDESC			currDesc;

	while (0 != (currFileObj = (LPFILEOBJECT)cfgsList.GetNextMember(currFileObj)))
		{
		currCFGDesc = (LPCFGDESC)currFileObj->GetHeadRecPtr();
		currDesc = NULL;
		while (0 != (currDesc = currCFGDesc->pNextObjectDesc(currDesc)))
			if (currDesc->GetUID() == objectType)
				return currDesc;
		}

	return NULL;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: cfgGetFunctionDesc
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
LPAFUNCDESC cfgGetFunctionDesc(
	COMPUID	functionType)
	{
	LPFILEOBJECT	currFileObj = NULL;
	LPCFGDESC			currCFGDesc;
	LPAFUNCDESC			currDesc;

	while (0 != (currFileObj = (LPFILEOBJECT)cfgsList.GetNextMember(currFileObj)))
		{
		currCFGDesc = (LPCFGDESC)currFileObj->GetHeadRecPtr();
		currDesc = NULL;
		while (0 != (currDesc = currCFGDesc->pNextFunctionDesc(currDesc)))
			if (currDesc->GetUID() == functionType)
				return currDesc;
		}

	return NULL;

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: cfgGetFunctionCategoryName
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
LPSTR cfgGetFunctionCategoryName(
	COMPUID	functionType)
	{
	LPFILEOBJECT	currFileObj = NULL;
	LPCFGDESC			currCFGDesc;
	LPAFUNCDESC			currDesc;

	while (0 != (currFileObj = (LPFILEOBJECT)cfgsList.GetNextMember(currFileObj)))
		{
		currCFGDesc = (LPCFGDESC)currFileObj->GetHeadRecPtr();
		currDesc = NULL;
		while (0 != (currDesc = currCFGDesc->pNextFunctionDesc(currDesc)))
			if (currDesc->GetUID() == functionType)
				return currCFGDesc->pGetFuncGroupName();
		}

	return NULL;

	}

