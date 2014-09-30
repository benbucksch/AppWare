#include "almsys.h"

#include "fileobj.h"
#include "system.h"
#include "sereclib.h"
#include "rectypes.h"
#include "memblk.h"
#include "exeobjs.h"
#include "exserblk.h"
#include "io.h"
#include "tempobj.h"
#include "exrecblk.h"
#include "recblk.h"
#include "serblk.h"
#include "exserblk.h"
#include "intdebug.h"
#include "context.h"

I_ENABLE_ASSERT;

FILEIDSTAMP	FILEOBJECT::stampSeed = 0;

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        FILEHEADER
//
// CLASS.......:        FILEHEADER
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
FILEHEADER::FILEHEADER(
	LPUIDTABLE      uids):
		seriusBlocksPrefix(uids->GenerateUID(&seriusBlocks), NULLSERUID),
		seriusBlocks(RT_SERIUSBLOCK),
		recordBlocksPrefix(uids->GenerateUID(&recordBlocks), NULLSERUID),
		recordBlocks(RT_RECORDBLOCK),
		exeSeriusBlocksPrefix(uids->GenerateUID(&exeSeriusBlocks), NULLSERUID),
		exeSeriusBlocks(RT_SERIUSBLOCK),
		exeRecordBlocksPrefix(uids->GenerateUID(&exeRecordBlocks), NULLSERUID),
		exeRecordBlocks(RT_RECORDBLOCK),
		memBlocksPrefix(uids->GenerateUID(&memBlocks), NULLSERUID),
		memBlocks(RT_SERIUSBLOCK)
	{

	headBlock = NULLSERUID;
	headRec = NULLSERUID;
	headExeBlock = NULLSERUID;
	headExeRec = NULLSERUID;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        FILEHEADER
//
// CLASS.......:        FILEHEADER
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
FILEHEADER::FILEHEADER(
	int  hFile,
  BOOL initExeRecords):
		seriusBlocks(),
		recordBlocks(),
		exeSeriusBlocks(),
		exeRecordBlocks(),
		memBlocks(RT_MEMBLOCK)
	{
	//Read the fileheader from the file

  _lread(hFile, (LPSTR)this, sizeof(FILEHEADER));

  exeSeriusBlocks.Initialize();
  exeRecordBlocks.Initialize();
  seriusBlocks.Initialize();
  recordBlocks.Initialize();

  memBlocks.Initialize();
  memBlocks.ZeroCount();

  if (initExeRecords == TRUE)
    {
    exeSeriusBlocks.ZeroCount();
    exeRecordBlocks.ZeroCount();
    }
  else
    {
    seriusBlocks.ZeroCount();
    recordBlocks.ZeroCount();
    }

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        FILEOBJECT
//
// CLASS.......:        FILEOBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
FILEOBJECT::FILEOBJECT(
	void):
		fileStrings(1024),
		exeStrings(1024),
		uids(),
		fileHdr(&uids),
    displaySplash(NULL)
	{

	pTheSystem->SetFileObject(this);
	status = FILESTAT_OK|FILESTAT_UNINIT;

	SetFileIdStamp();
  
  if(!(fileHdr.idInfo.fileType & FILETYPE_CONFIG))
  	displaySplash = new SPLASHDISPLAY();
  	
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        FILEOBJECT
//
// CLASS.......:        FILEOBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
FILEOBJECT::FILEOBJECT(
	const TSTRING&  name,
	WORD             majorVersion,
	WORD             minorVersion,
	WORD             databaseVersion,
	WORD             fileType):
		fileStrings(1024),
		exeStrings(1024),
		uids(),
		fileHdr(&uids),
		name(name),
    displaySplash(NULL)
	{

	pTheSystem->SetFileObject(this);
	lstrcpy(fileHdr.idInfo.idString, ID_STRING);

	fileHdr.idInfo.majorVersion = majorVersion;
	fileHdr.idInfo.minorVersion = minorVersion;
	fileHdr.idInfo.fileType = fileType;
	fileHdr.idInfo.databaseVersion = databaseVersion;

	status = FILESTAT_NEW;

	SetFileIdStamp();

  if(!(fileHdr.idInfo.fileType & FILETYPE_CONFIG))
  	displaySplash = new SPLASHDISPLAY();  	
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        FILEOBJECT
//
// CLASS.......:        FILEOBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
FILEOBJECT::FILEOBJECT(
	int     hFile,
	const TSTRING& filePath,
	const TSTRING& name):
		fileStrings(),
		exeStrings(1024),
		name(name),
		filePath(filePath),
		uids(),
		fileHdr(hFile, TRUE),
    displaySplash(NULL)
	{
	LPRECORDBLOCK  pRecBlock;

	pTheSystem->SetFileObject(this);

  //Init uid table for records imbedded in the file header

	uids.InitPointer(fileHdr.seriusBlocksPrefix.UID, &fileHdr.seriusBlocks);
	uids.InitPointer(fileHdr.recordBlocksPrefix.UID, &fileHdr.recordBlocks);

	uids.InitPointer(fileHdr.exeSeriusBlocksPrefix.UID,
	 &fileHdr.exeSeriusBlocks);
	uids.InitPointer(fileHdr.exeRecordBlocksPrefix.UID,
	 &fileHdr.exeRecordBlocks);

	uids.InitPointer(fileHdr.memBlocksPrefix.UID, &fileHdr.memBlocks);

	//Begin reading Blocks

  SERIUSBLOCK::ReadBlockRecords(hFile, &fileHdr.seriusBlocks);

  SERIUSBLOCK::ReadBlockData(hFile, &fileHdr.seriusBlocks);

  RECORDBLOCK::ReadRecordBlockRecords(hFile, &fileHdr.recordBlocks);

	//read in the data base strings now so they are available when the
  //records are constructed during InitUIIDTable of BASERECORDBLOCK.

	fileStrings.Read(hFile);

	pRecBlock = NULL;
	while (0 != (pRecBlock =
				       (LPRECORDBLOCK)fileHdr.recordBlocks.GetNextMember(pRecBlock)))
		pRecBlock->InitUIDTable();

	uids.BuildFreeList();

	status = FILESTAT_OK;

	SetFileIdStamp();

  if(!(fileHdr.idInfo.fileType & FILETYPE_CONFIG))
  	{
  	displaySplash = new SPLASHDISPLAY();  	
    if(TestSplashScreenFlag())
    	{
    	if(!displaySplash->Read(hFile))
      	ClearSplashScreenFlag();
      }
    }
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        ~FILEOBJECT
//
// CLASS.......:        FILEOBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
FILEOBJECT::~FILEOBJECT()
	{
  I_ASSERT(!displaySplash || !(fileHdr.idInfo.fileType & FILETYPE_CONFIG));
  
	if(displaySplash)
  	delete displaySplash;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....: Write
//
// CLASS.......: FILEOBJECT
//
// DESCRIPTION.: Writes the project structures to a project file.
//
/////////////////////////////////////////////////////////////////////////////
BOOL FILEOBJECT::Write(
	int     hFile)
	{
	_lwrite(hFile, (LPSTR)&fileHdr, sizeof(FILEHEADER));

  SERIUSBLOCK::WriteBlockRecords(hFile, &fileHdr.seriusBlocks);

  SERIUSBLOCK::WriteBlockData(hFile, &fileHdr.seriusBlocks);

  RECORDBLOCK::WriteRecordBlockRecords(hFile, &fileHdr.recordBlocks);

	//write out the database strings

	fileStrings.Write(hFile);
  
	if(TestSplashScreenFlag())
  	{
    if(displaySplash->ValidSplash())
    	{
      I_VERIFY(displaySplash->Write(hFile));
      }
    else
    	{
      I_ASSERT(FALSE);
      }
    }
  	

	return TRUE;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        Save
//
// CLASS.......:        FILEOBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL FILEOBJECT::Save(
	void)
	{
	BOOL       result = FALSE;
	int        hFile = -1;
	OFSTRUCT   fileInfo;
	char       fullPath[256];

	lstrcpy(fullPath, this->filePath);
	lstrcat(fullPath, this->name);

	if (-1 == (hFile = OpenFile(fullPath, &fileInfo, OF_CREATE)))
		{
		MessageBeep(0);
		MessageBox(NULL, this->filePath, "Can't Open File",
				MB_OK|MB_SYSTEMMODAL|MB_ICONEXCLAMATION);
		}
	else
		{
		result = this->Write(hFile);

		if (result)
			ClearDirtyFlag();

		_lclose(hFile);
		}

	return result;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  FILEOBJECT
//
// CLASS.......:  FILEOBJECT
//
// DESCRIPTION.:  This constructor reads a serius executable from disk.
//
/////////////////////////////////////////////////////////////////////////////
FILEOBJECT::FILEOBJECT(
	int     hFile,
	HINSTANCE hPrevInst):
		fileStrings(),
		exeStrings(),
		name(""),
		filePath(""),
		uids(),
		fileHdr(hFile, FALSE),
    displaySplash(NULL)
	{
	LPRECORDBLOCK           pRecBlock;
	DWORD filePos;
  
	pTheSystem->SetFileObject(this);

  if(!((fileHdr.idInfo.fileType & FILETYPE_ONEINSTANCE) && hPrevInst))
    {  
	  if(!(fileHdr.idInfo.fileType & FILETYPE_CONFIG))
	  	{
	  	displaySplash = new SPLASHDISPLAY();  	
	    
	    if(TestSplashScreenFlag())
	    	{
	      //save file location
	      filePos = _llseek(hFile, 0, 1);
	      
	      //in an executable, a long is written following the splash
	      //screen data.
	    	if(!displaySplash->Read(hFile, sizeof(long)))
	      	ClearSplashScreenFlag();
	      else
	      	displaySplash->ShowSplashScreen();
	      
	      //seek back to original location  
	      _llseek(hFile, filePos, 0);
	      }
	    }
		
		uids.InitPointer(fileHdr.seriusBlocksPrefix.UID, &fileHdr.seriusBlocks);
		uids.InitPointer(fileHdr.recordBlocksPrefix.UID, &fileHdr.recordBlocks);

		uids.InitPointer(fileHdr.exeSeriusBlocksPrefix.UID,
		 &fileHdr.exeSeriusBlocks);
		uids.InitPointer(fileHdr.exeRecordBlocksPrefix.UID,
		 &fileHdr.exeRecordBlocks);

		uids.InitPointer(fileHdr.memBlocksPrefix.UID, &fileHdr.memBlocks);

		//Begin reading Blocks

	  EXESERIUSBLOCK::ReadBlockRecords(hFile, &fileHdr.exeSeriusBlocks);

	  EXESERIUSBLOCK::ReadBlockData(hFile, &fileHdr.exeSeriusBlocks);

	  EXERECORDBLOCK::ReadRecordBlockRecords(hFile, &fileHdr.exeRecordBlocks);

		//read in the data base strings now so they are available when the
	  //records are constructed during InitUIIDTable of BASERECORDBLOCK.

		exeStrings.Read(hFile);

		pRecBlock = NULL;

		while (0 != (pRecBlock =
					       (LPRECORDBLOCK)fileHdr.exeRecordBlocks.GetNextMember(pRecBlock)))
			pRecBlock->InitUIDTable();

		uids.BuildFreeList();

		status = FILESTAT_OK;

		tempObjects = new FASTRECMGR(64, sizeof(TEMPOBJECT));
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ExeWrite
//
// CLASS.......:  FILEOBJECT
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
BOOL FILEOBJECT::ExeWrite(
	int     hFile)
	{

	_lwrite(hFile, (LPSTR)&fileHdr, sizeof(FILEHEADER));

  EXESERIUSBLOCK::WriteBlockRecords(hFile, &fileHdr.exeSeriusBlocks);

  EXESERIUSBLOCK::WriteBlockData(hFile, &fileHdr.exeSeriusBlocks);

  EXERECORDBLOCK::WriteRecordBlockRecords(hFile, &fileHdr.exeRecordBlocks);

	//write out the exestrings

	exeStrings.Write(hFile);

	if(TestSplashScreenFlag())
  	{
    if(displaySplash->ValidSplash())
    	{
      I_VERIFY(displaySplash->Write(hFile));
      }
    else
    	{
      I_ASSERT(FALSE);
      }
    }
  	
	return TRUE;

	}


/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        ConvertToPointers
//
// CLASS.......:        FILEOBJECT
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
void FILEOBJECT::ConvertToPointers(
	void)
	{
	fileHdr.headRec.ptr = GetObject(fileHdr.headRec);
	}

////////////////////////////////////////////////////////////////////////////
void FILEOBJECT::ClearSplashScreenFlag()
	{
  I_ASSERT(!displaySplash->ValidSplash());
  fileHdr.idInfo.fileType &= ~FILETYPE_SPLASH;
  }

////////////////////////////////////////////////////////////////////////////
void FILEOBJECT::SetSplashScreenFlag()
	{
  I_ASSERT(displaySplash->ValidSplash());
  fileHdr.idInfo.fileType |= FILETYPE_SPLASH;
  }
  
////////////////////////////////////////////////////////////////////////////
BOOL FILEOBJECT::TestSplashScreenFlag()
	{
  return (fileHdr.idInfo.fileType & FILETYPE_SPLASH)?TRUE:FALSE;
  }

