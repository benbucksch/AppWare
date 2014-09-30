//======================================================================
// context.cpp -- task-specific information for OIP.DLL
//
//  jon 10-Jun-92 created
//  jon 23-Jun-92 added "sereclib.h" to solve(?) link problem with
//                GetObject(SERUID) ???
//  jon 24-Jun-92 don't attempt to get the pointer for <exeHead> if
//                the passed file object has a null SERID for it, ie
//                the EXE structures haven't been filled in yet because
//                we are pre-compile.
//  jon 24-Jun-92 overload new and delete operators, so we can cope
//                with crashed client tasks... i.e., any client that
//                doesn't exeUnregisterContext() before dying.  Now
//                the DLL will own the memory, not the client apps.
//  jon 24-Jun-92 changed documentation style to appease the master.
//  jon  9-Aug-92 added GetCopyOfExeInfo()
//  jon 24-Aug-92 if it's debug it *must* be runtime...
//  jon 27-Aug-92 added FillFromExeInfo() to aid debug world.
//  jon 12-Oct-92 REMOVED GetCopyOfExeInfo() [never used]
//======================================================================
#include "almsys.h"
#include <commdlg.h>

#include "context.h"
#include "sereclib.h"
#include "memlib.h"
#include "persdata.h"
#include "exeobjs.h"
#include "oiputils.h"
#include "oiptime.h"
#include "fnameobj.h"
#include "utility.h"
#include "a_almutl.h"

class   EXEEVENTLIST;
typedef EXEEVENTLIST  * LPEXEEVENTLIST;

#include "exetypes.h"
#include "eventqnd.i"
#include "eventqms.i"

//----------------------------------------------------------------------
//   APPCONTEXT::APPCONTEXT
//
// Instantiates the context.
//----------------------------------------------------------------------
APPCONTEXT::APPCONTEXT (LPEXEINFO theExeInfo)
	: exeInfo(&exeInfoAux)
  {
  this->next         = 0;
  this->priorityMode = AAPP_OBJECTPRIORITY;
  this->eventQueue   = 0;
  this->hIcon        = 0;
  this->eventQueueNodeMgr = NULL;
  this->eventQueueMsgMgr = NULL;

  this->Freshen(theExeInfo);
  }

//----------------------------------------------------------------------
//   APPCONTEXT::~APPCONTEXT
//
// Kills the queue, its node managers and the application icon.  Also
// writes out the persistent data if there is any.
//----------------------------------------------------------------------
APPCONTEXT::~APPCONTEXT()
  {
  delete this->eventQueue;
  delete this->eventQueueNodeMgr;
  delete this->eventQueueMsgMgr;

  if (this->hIcon)
    DestroyIcon(this->hIcon);

  }

//----------------------------------------------------------------------
//   APPCONTEXT::Freshen
//
// Whenever a context is Freshen'd, the caller should check for a
// fileObj of zero to determine an error has occurred.
//----------------------------------------------------------------------
BOOL APPCONTEXT::Freshen(
  LPEXEINFO  theExeInfo)
  {
  LPEXEEVENTLIST  eventList;

  this->FreshenExeInfo(theExeInfo);

  if (this->IsMicroApp() && !LoadMicroApp())
    return FALSE;

 // *** [ CAUTION! ] *** [ CAUTION! ] *** [ CAUTION! ] *************
 //
 //  The assumption here is that no EXEHEAD type serius records
 //  will be constructed or destroyed at runtime. If this is not
 //  the case, it is very likely this exeHead pointer will go stale!
 //
 // *****************************************************************
  if (this->IsExeStructures())
    {
    SERUID exeHeadUID = this->GetFileObject()->GetHeadExeRec();
    if (exeHeadUID == NULLSERUID)
      this->exeHead = 0;
    else
      this->exeHead = (LPEXEHEAD) this->GetFileObject()
                                  ->GetUIDTable()
                                  ->GetPointer(exeHeadUID);
    }
  else
    this->exeHead = 0;

  if (this->IsRuntime())
    {
    // Ensure a queue is instantiated
    if (!this->eventQueue)
      {
      this->eventQueueNodeMgr = new FASTRECMGR(32, sizeof(EVENTQUEUENODE));
      this->eventQueueMsgMgr = new FASTRECMGR(32, sizeof(EVENTQUEUEMSG));
      this->eventQueue = new EVENTQUEUE;
      eventList = (LPEXEEVENTLIST)
                  GetFileObject()
                  -> GetUIDTable()
                  -> GetPointer(exeHead->GetExeEventListUID());
      this->eventQueue->SetEventList(eventList);
      }

    // Are there any stops set? (Speeds function execution if
    // there are none)
    eventQueue->SetIsDebugging(this->IsDebug());

    if(TestSingleStepFlag())
    	eventQueue->SetSingleStepFlag();
    else
    	eventQueue->ClearSingleStepFlag();
    
    // Get the icon deposited by the compiler if we're standalone
    // else use serws's icon
    
    //IMPORTANT: 4002 is the value of IDIC_PROJECTEXE in resconst.h 
		// GetModuleHandle(NULL) will return HINSTANCE of module that
		// was used to create this process (i.e., either AppWare.exe or
		// almshell.exe).
    this->hIcon = LoadIcon(GetModuleHandle(NULL),
                           this->IsMicroApp()
                             ? "ShellIcon"
                             : MAKEINTRESOURCE(4002));
		}

  return TRUE;
  }

//----------------------------------------------------------------------
//   APPCONTEXT::FreshenExeInfo
//
// Copies the specified EXEINFO into this->exeInfo.
//----------------------------------------------------------------------
void APPCONTEXT::FreshenExeInfo(LPEXEINFO theExeInfo)
  {
	this->exeInfo.version    = 0;
	this->exeInfo.flags      = 0;

	this->exeInfo.lUnused1   =
		this->exeInfo.lUnused2 =
		this->exeInfo.lUnused3 = 0;

	FillMemory(this->exeInfo.pAuxInfo, sizeof(EXEINFOAUX), 0);

  switch (theExeInfo->version)
    {
		// We do NOT need to check before version 102 since we renamed
		// DLLs in going to AppWare and therefore required a recompile.

		case EXEINFO_VERSION_102:
    default:
			this->exeInfo.version = theExeInfo->version;
			this->exeInfo.flags   = theExeInfo->flags;

			*(this->exeInfo.pAuxInfo) = *(theExeInfo->pAuxInfo);
      break;
    }

  // Make sure Troy's MemLib globals are set up...
  this->SetGlobals();
  }

//----------------------------------------------------------------------
//   APPCONTEXT::GetCopyOfExeInfo
//
// Fetches a copy of the current EXEINFO for this context.
//----------------------------------------------------------------------
BOOL APPCONTEXT::GetCopyOfExeInfo(LPEXEINFO copy)
  {
	copy->version = this->exeInfo.version;
	copy->flags   = this->exeInfo.flags;

	*(copy->pAuxInfo) = *(this->exeInfo.pAuxInfo);

  return TRUE;
  }

//----------------------------------------------------------------------
//   APPCONTEXT::LoadMicroApp
//
// Build up the FILEOBJECT for this app.  We do it here to reduce the
// amount of code in the microapp stub.
//----------------------------------------------------------------------
BOOL APPCONTEXT::LoadMicroApp()
  {
  HANDLE     hExeFile;
  long       startOfExe;
  HANDLE     hPersFile;
  LPEXEHEAD  headRec;
	DWORD				nBytesRead;

#ifdef AW_I_TIME
	long			nLoadTime	= HiResTime();
	char			szMsg[80];
#endif

  hExeFile = OpenExeFile();

  SetFileObject(0);

  if (hExeFile == INVALID_HANDLE_VALUE)
    return FALSE;

  // Start of app def is at offset spec'd in last four bytes of the exe
	SetFilePointer(hExeFile, -sizeof(startOfExe), NULL, FILE_END);
	ReadFile(hExeFile, &startOfExe, sizeof(startOfExe), &nBytesRead, NULL);

  // Go to the start of the app def
	SetFilePointer(hExeFile, startOfExe, NULL, FILE_BEGIN);

  SetFileObject(new FILEOBJECT(hExeFile));

	CloseHandle(hExeFile);

#ifdef AW_I_TIME
	nLoadTime	= HiResTime() - nLoadTime;
	wsprintf(szMsg, "Load time (ms):  %ld\r\n", nLoadTime);
	OutputDebugString(szMsg);
#endif

  if (!GetFileObject()->TestOkFlag())
    goto BadBye;

  // --- Get persistence info.

  headRec = (LPEXEHEAD)GetObject(GetFileObject()->GetHeadExeRec());
  if (headRec->HasPersistentData())
    {
#ifdef AW_I_TIME
		nLoadTime	= HiResTime();
#endif

    if (!OpenPersistents(&hPersFile, headRec->GetIniFileName()))
      goto BadBye;

    if (hPersFile != INVALID_HANDLE_VALUE)
      {
      BOOL  persOK;

      persOK = ReadPersistents(hPersFile);
			CloseHandle(hPersFile);
      if (!persOK)
        goto BadBye;
      }
#ifdef AW_I_TIME
		nLoadTime	= HiResTime() - nLoadTime;
		wsprintf(szMsg, "Persistent load time (ms):  %ld\r\n", nLoadTime);
		OutputDebugString(szMsg);
#endif
    }

  return TRUE;

BadBye:

  delete GetFileObject();
  SetFileObject(0);
  return FALSE;

  }


//----------------------------------------------------------------------
//   OpenExeFile
//
// Opens the .EXE that is calling us.
//----------------------------------------------------------------------
HANDLE APPCONTEXT::OpenExeFile()
  {
  HANDLE         hFile;

	hFile = CreateFile(GetExeFileName(), GENERIC_READ, FILE_SHARE_READ, NULL,
											OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

  return hFile;
  }


//----------------------------------------------------------------------
//   APPCONTEXT::OpenPersistents
//
// Determines path to persistent data file, 'pdfPath', and opens the
// persistent data file.
//
// The pdfPath is saved into this context's FILEOBJECT [via the
// SetFilePath() method]. Fills in hPersFile with the DOS file
// handle to the persistent data file. A hPersFile of -1 indicates
// a persistent data file was not opened. Returns FALSE on error.
//----------------------------------------------------------------------
BOOL APPCONTEXT::OpenPersistents(
  HANDLE*  hPersFile,
  LPCSTR    iniFileName)
  {
	SFilePath		pdfPath;
  SFilePath		iniPath;
	SFilePath		exePath;
  BOOL        foundPathInIni;

	exePath = GetExeFileName();

  if (iniFileName)
    {
    //------------------------------------------------------------------
    //   A path to the .INI file for this app was passed in. Fetch
    //   the path specified in it.
    //------------------------------------------------------------------
    foundPathInIni = FetchPdfPathFromIni(iniFileName, pdfPath);
    }
  else
    {
    //------------------------------------------------------------------
    //   A .INI path was not passed in.  Search for a file named
    //   <app>.ini where <app> is the simple file name of this app.
    //   The pdfPath will be equal to noPathString as long as this
    //   app's .INI file is not found or there is no setting for the
    //   pdfPath in that .INI 
    //------------------------------------------------------------------

    // 1st -- Look for <app>.ini in startup directory...
		exePath.GetName(iniPath);
		iniPath.MakeFromCurrentDirectory();
		iniPath.SetExtension("INI");

    foundPathInIni = FetchPdfPathFromIni(iniPath, pdfPath);

    // 2nd -- Look by Windows rules [i.e., in Windows dir/System dir]
    if (!foundPathInIni)
      {
			iniPath.SetDirectory("");		// already has name.ini
      foundPathInIni = FetchPdfPathFromIni(iniPath, pdfPath);
      }

    // 3rd -- Look in home directory of app
    if (!foundPathInIni)
      {
			iniPath = exePath;
			iniPath.SetExtension("INI");
      foundPathInIni = FetchPdfPathFromIni(iniPath, pdfPath);
      }
    }

  if (foundPathInIni)
    {
    if (pdfPath.Length() == 0)
      {
      // The pdfPath was specified as a null string in the .INI file
      this->GetFileObject()->SetFilePath("");
      *hPersFile = INVALID_HANDLE_VALUE;
      return TRUE;
      }

    //------------------------------------------------------------------
    //   Ensure that the pdfPath from the .INI is valid
    //------------------------------------------------------------------

//!! This needs to be redone to account for no 8.3, UNC, and file locking !!

		if (GetFileTitle(pdfPath, NULL, 0) < 0)	// bad filename
      {
      // The user has a bad path in the .INI file...
      this->GetFileObject()->SetFilePath("");
      *hPersFile = INVALID_HANDLE_VALUE;

      return ComplainAboutPdfPath(pdfPath, iniPath);
      }
    }
  else
    {

    // The pdfPath was not specified in the INI. Look for a file
    // named <app>.pdt in the home directory of the executable.

		pdfPath = exePath;
		pdfPath.SetExtension(PERSDATA_EXTENSION);
    }

  *hPersFile = CreateFile(pdfPath, GENERIC_READ, 0, NULL,
													OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

  this->GetFileObject()->SetFilePath((const char*) pdfPath);

  return TRUE;
  }


//----------------------------------------------------------------------
//   APPCONTEXT::FetchPdfPathFromIni
//----------------------------------------------------------------------
BOOL APPCONTEXT::FetchPdfPathFromIni(
  LPCSTR , // iniPath,
  SFilePath& pdfPath)
  {
  static char noPathString[] = " \\ ";  // must be an invalid pathname char,,,
	char				szTmp[MAX_PATH];
	HKEY        appSetKey;
  
//!! Need to move away from .INI files to registry!!!
	appSetKey = AUtlOpenProductSubKey(HKEY_CURRENT_USER, AUTL_KEY_APPSETTINGS);
  
  AUtlGetKeyString(
    appSetKey, PERSDATA_INIKEY, szTmp, sizeof(szTmp), noPathString);

  if(appSetKey)
  	AUtlCloseProductSubKey(appSetKey);
  
  if (0 != lstrcmp(szTmp, noPathString))
    {
    pdfPath = szTmp;
    return TRUE;
    }

  return FALSE;
  }

//------------------------------------------------------------------
//    APPCONTEXT::ComplainAboutPdfPath
//
// Tell'em the pdfPath from the .INI is invalid. Returns TRUE if
// the user wants to continue without a persistent data file.
//------------------------------------------------------------------
BOOL APPCONTEXT::ComplainAboutPdfPath(
  SFilePath& pdfPath,
  SFilePath& iniPath)
  {
	SFormattedMsg		msgText;
	SFormattedMsg		msgCap;
  int       ans;

  // Scream and die...
  msgText.Format(
    "INI File:   %s\n"
    "Section:   [%s]\n"
    "Keyword:   \"%s=\"\n"
    "Specified path:   %s"
    "\n\n"
    "The specified path is invalid.  "
    "Do you want to continue without persistent data?",
    (const char*) iniPath,
    "Application Settings",
    PERSDATA_INIKEY,
    (const char*) pdfPath);

  msgCap.Format("Error in INI File: %s", (const char*) iniPath);

  ans = MessageBox(
           NULL,
           msgText,
           msgCap,
           MB_YESNO|MB_DEFBUTTON2|MB_ICONSTOP|MB_TASKMODAL);

  return (ans == IDYES);
  }

//----------------------------------------------------------------------
//   APPCONTEXT::ReadPersistents
//----------------------------------------------------------------------
BOOL APPCONTEXT::ReadPersistents(
  HANDLE  hFile)
  {
  PERSISTENTDATAHEADER    pdHdr;
  LPPERSISTENTDATAHEADER  exePdHdr;
  OBJECTHDR               poHdr;
  LPEXEOBJINST            poPtr;
  LPEXEHEAD               headRec;
  int                     idx;
  SERUID                  typeBlock;
  SFormattedMsg           msgText;
	DWORD										nRead;

  headRec = (LPEXEHEAD)GetObject(GetFileObject()->GetHeadExeRec());
  exePdHdr = headRec->GetPersistentDataHeader();

	ReadFile(hFile, &pdHdr, sizeof(pdHdr), &nRead, NULL);

  if (nRead != sizeof(pdHdr) || pdHdr.GetTag() != exePdHdr->GetTag())
    {
    msgText.Format("%s is not a persistent data file.  "
             "You need to delete it or rename it before"
             " you can run this application.",
             (LPCSTR) GetFileObject()->GetFilePath());

    goto BadBye;
    }

  if (memcmp(&pdHdr, exePdHdr, sizeof(pdHdr)) != 0)
    {
    // It's a valid persistent data file but it doesn't match
    // this app, so don't read it in!
    goto GoodBye;
    }

  for (;;)
    {
		ReadFile(hFile, &poHdr, sizeof(poHdr), &nRead, NULL);
		if (nRead != sizeof(poHdr))
			break;

    poPtr = (LPEXEOBJINST) GetSeriusObject(poHdr.objectID);
    if (!poPtr || !poPtr->TestPersistentsFlag() || poHdr.tag != EXEOBJ_TAG)
      {
      msgText.Format("%s has become corrupted.  "
               "You need to delete it or rename it before"
               " you can run this application.",
               (LPCSTR) GetFileObject()->GetFilePath());

      goto BadBye;
      }

    poPtr->SetDatasCount(poHdr.numDatas);

    for (idx = 0 ; idx < poHdr.numDatas ; idx++)
      {
      if (!poPtr->LoadPersistentDataBlock(hFile, idx))
        {
        msgText.Format("%s contains bad persistent data!  "
                 "You need to delete it or rename it before"
                 " you can run this application.",
                 (LPCSTR) GetFileObject()->GetFilePath());

        goto BadBye;
        }
      }
    }

GoodBye:

  return TRUE;

BadBye:

  MessageBox(NULL,
             msgText,
             "Invalid Persistent Data File",
             MB_OK|MB_ICONSTOP|MB_TASKMODAL);

  return FALSE;
  }

//----------------------------------------------------------------------
//   APPCONTEXT::SavePersistents
//----------------------------------------------------------------------
BOOL APPCONTEXT::SavePersistents(
	BOOL displayErrorsFlag)
	{
	LPCSTR                 pdPath	= GetFileObject()->GetFilePath();
	HANDLE                hPersFile = INVALID_HANDLE_VALUE;
	PERSISTENTDATAHEADER  pdh;
	int                   i;
	RUNTIMEUID            typeBlock;
	LPEXEOBJTYPE          typePtr;
	char                  msgText[300];
	DWORD									nBytesWritten;

	if (!this->exeHead->HasPersistentData())
		goto GoodBye;

	if (!pdPath || pdPath[0] == '\0')
		{
		// It's supposed to have persistent data ... but they
		// turned it off in the .INI file
		goto GoodBye;
		}

	hPersFile = CreateFile(GetFileObject()->GetFilePath(), GENERIC_WRITE, 0, NULL,
											 CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (hPersFile == INVALID_HANDLE_VALUE)
		{
		wsprintf(msgText, "Can't open %s for writing.",
											(LPCSTR) GetFileObject()->GetFilePath());
		goto BadBye;
		}

	WriteFile(hPersFile, this->exeHead->GetPersistentDataHeader(),
														 sizeof(PERSISTENTDATAHEADER), &nBytesWritten, NULL);


	typeBlock = this->exeHead->GetExeObjTypeList();

	i = 0;
	while (0 != (typePtr = (LPEXEOBJTYPE)GetObject(typeBlock, i)))
		if (!typePtr->StorePersistentObjects(hPersFile))
			{
			ATYPEID  typeID;
			char    typeAlphaID[5];

			typeID = typePtr->GetTypeID();
			typeAlphaID[0] = (char) ((0xFF000000L & typeID) >> 24);
			typeAlphaID[1] = (char) ((0x00FF0000L & typeID) >> 16);
			typeAlphaID[2] = (char) ((0x0000FF00L & typeID) >>  8);
			typeAlphaID[3] = (char) ((0x000000FFL & typeID));
			typeAlphaID[4] = '\0';

			wsprintf(
				msgText,
				"Can't save persistent info for a '%s' type object into %s.",
				typeAlphaID,
				(LPCSTR) GetFileObject()->GetFilePath());

			goto BadBye;
			}

GoodBye:
	if (hPersFile != INVALID_HANDLE_VALUE)
		CloseHandle(hPersFile);
	return TRUE;

BadBye:

	if (displayErrorsFlag == TRUE)
		MessageBox(NULL, msgText, "Error Saving Persistent Data",
			MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);

	if (hPersFile != INVALID_HANDLE_VALUE)
		CloseHandle(hPersFile);

	return FALSE;
	}

//----------------------------------------------------------------------
//   APPCONTEXT::SetAppPriorityMode
//
// Returns the old priority mode or zero if the newMode is
// invalid.
//----------------------------------------------------------------------
LONG APPCONTEXT::SetAppPriorityMode(LONG newMode)
  {
  if (newMode == AAPP_VISUALPRIORITY ||
      newMode == AAPP_FUNCTIONPRIORITY ||
      newMode == AAPP_OBJECTPRIORITY)
    {
    LONG hold = priorityMode;
    priorityMode = newMode;
    return hold;
    }
  else
    return 0;
  }

//----------------------------------------------------------------------
//   APPCONTEXT::SetGlobals
//
// Make sure Troy's MemLib globals are set up...
// Also, make sure the masterTypeList is set up.
//----------------------------------------------------------------------
void APPCONTEXT::SetGlobals()
  {
  ::SetGlobalBlockArrayAddr(this->GetGlobalBlockAddr());
  ::SetGlobalBlockArraySize((unsigned)(this->GetGlobalBlockSize()));

	pTheSystem->SetMasterTypeList(this->GetMasterTypeList());
  }

//----------------------------------------------------------------------
//   APPCONTEXT::EnterDebugger
//
// Call into the debugger using the address passed when this context
// was registered.
//----------------------------------------------------------------------
BOOL APPCONTEXT::EnterDebugger(LONG exeFlow)
  {
  return (this->GetDebugProc())(exeFlow);
  }



//----------------------------------------------------------------------
//   APPCONTEXT::MaybeWritePersistentData
//
// Writes the persistent data if it's a micro app
//----------------------------------------------------------------------
BOOL APPCONTEXT::MaybeWritePersistentData()
	{
	BOOL rval = FALSE;

  if (this->IsMicroApp())
		{
    if (this->exeHead->HasPersistentData() && this->GetFileObject())
			{
			this->SavePersistents();
			rval = TRUE;
			}
		}
	
	return rval;
	}
