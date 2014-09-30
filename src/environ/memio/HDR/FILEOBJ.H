#ifndef __FILEOBJ_H__
#define __FILEOBJ_H__

#include "classdef.h"
#include "version.h"
#include "recprefx.h"
#include "list.h"
#include "listmbr.h"
#include "rtreclst.h"
#include "strspace.h"
#include "uidtable.h"
#include "tstring.h"
#include "splshdis.h"

#include "memclass.h"

typedef long FILEIDSTAMP;

struct FILEIDINFO
	{
	char				idString[ID_STRING_LEN+1];
	WORD				majorVersion;
	WORD				minorVersion;
	WORD				databaseVersion;
	WORD				fileType;
	};

struct FILEHEADER
	{
	FILEIDINFO			idInfo;

	RECORDPREFIX		exeSeriusBlocksPrefix;
	RUNTIMERECLIST  exeSeriusBlocks;
	RECORDPREFIX		exeRecordBlocksPrefix;
	RUNTIMERECLIST  exeRecordBlocks;

	RECORDPREFIX		memBlocksPrefix;
	RUNTIMERECLIST  memBlocks;

	RECORDPREFIX		seriusBlocksPrefix;
	RUNTIMERECLIST	seriusBlocks;
	RECORDPREFIX		recordBlocksPrefix;
	RUNTIMERECLIST  recordBlocks;
	RUNTIMEUID			headBlock;
	SERUID					headRec;
	RUNTIMEUID			headExeBlock;
	SERUID					headExeRec;

	FILEHEADER(
		LPUIDTABLE	uids);

	FILEHEADER(
		HANDLE  hFile,
    BOOL initExeRecords);

	};

#define	FILESTAT_OK								0X00000001
#define FILESTAT_UNINIT						0X00000002
#define FILESTAT_SHORT						0X00000004
#define FILESTAT_OLDDB						0X00000008
#define FILESTAT_WRONGTYPE				0X00000010
#define FILESTAT_NEW							0X00000020
#define FILESTAT_NOTSERIUSFILE		0X00000040
#define FILESTAT_SAVEAS           0X00000080
#define FILESTAT_DIRTY            0X00000100

class FILEOBJECT:public LISTMEMBER, public MEMORY
	{
private:
	static FILEIDSTAMP stampSeed;

	STRINGSPACE			fileStrings;
	STRINGSPACE			exeStrings;
	UIDTABLE		 		uids;
	FILEHEADER   		fileHdr;
  LPFASTRECMGR    tempObjects;
	TSTRING			 		filePath;
	TSTRING			 		name;
	DWORD				 		status;
	FILEIDSTAMP			idStamp;
	SPLASHDISPLAY*	displaySplash;

  unsigned long WriteShellExe(
    HANDLE  hFile);

	void SetFileIdStamp()
		{
		idStamp = ++stampSeed;
		}

public:

	static BOOL CheckConfigFile(
		HANDLE 		hFile);

	static SERUID	CreateRecordBlock(
		long int	recordSize,
		long int 	type);

	FILEOBJECT(
		void);

	FILEOBJECT(
		TSTRING  name,
		WORD		 majorVersion,
		WORD		 minorVersion,
		WORD		 databaseVersion,
		WORD		 fileType);

	FILEOBJECT(
		HANDLE 		hFile,
		TSTRING	filePath,
		TSTRING	name);

  FILEOBJECT(
		HANDLE     hFile);

	~FILEOBJECT();

	LPRUNTIMERECLIST GetSeriusBlocks(
		void)
		{
		return &fileHdr.seriusBlocks;
		}

	LPRUNTIMERECLIST	GetRecordBlocks(
		void)
		{
		return &fileHdr.recordBlocks;
		}

	LPRUNTIMERECLIST	GetExeSeriusBlocks(
		void)
		{
		return &fileHdr.exeSeriusBlocks;
		}

	LPRUNTIMERECLIST	GetExeRecordBlocks(
		void)
		{
		return &fileHdr.exeRecordBlocks;
		}

	LPRUNTIMERECLIST GetMemBlocks(
		void)
		{
		return	&fileHdr.memBlocks;
		}

	LPFASTRECMGR GetTempObjects(
		void)
		{
		return	tempObjects;
		}

  BOOL SetTempObjects(
    LPFASTRECMGR newTempObjects)
    {
    tempObjects = newTempObjects;
    return TRUE;
		}

	LPUIDTABLE GetUIDTable(
		void)
		{
		return &uids;
		}

	LPSTRINGSPACE	GetStringSpace(
		void)
		{
		return &fileStrings;
		}

	LPSTRINGSPACE	GetExeStringSpace(
		void)
		{
		return &exeStrings;
		}

	TSTRING GetName(
		void)
		{
		return name;
		}

	BOOL SetName(
		const TSTRING& 	newName)
		{
		name = newName;
		return TRUE;
		}

	TSTRING GetFilePath(
		void)
		{
		return filePath;
		}

	BOOL SetFilePath(
		const TSTRING& 	newPath)
		{
		filePath = newPath;
		return TRUE;
		}

	RUNTIMEUID GetHeadBlock(
		void)
		{
		return fileHdr.headBlock;
		}

	SERUID GetHeadRec(
		void)
		{
		return fileHdr.headRec;
		}

	void * GetHeadRecPtr(
		void)
		{
		return fileHdr.headRec.ptr;
		}

	BOOL SetHeadBlock(
		RUNTIMEUID 	newHeadBlock)
		{
		fileHdr.headBlock = newHeadBlock;
		return TRUE;
		}

	BOOL SetHeadRec(
		SERUID	newHeadRec)
		{
		fileHdr.headRec = newHeadRec;
		return TRUE;
		}

	RUNTIMEUID GetHeadExeBlock(
		void)
		{
		return fileHdr.headExeBlock;
		}

	SERUID GetHeadExeRec(
		void)
		{
		return fileHdr.headExeRec;
		}

	void * GetHeadExeRecPtr(
		void)
		{
		return fileHdr.headExeRec.ptr;
		}

	BOOL SetHeadExeBlock(
		RUNTIMEUID 	newHeadBlock)
		{
		fileHdr.headExeBlock = newHeadBlock;
		return TRUE;
		}

	BOOL SetHeadExeRec(
		SERUID	newHeadRec)
		{
		fileHdr.headExeRec = newHeadRec;
		return TRUE;
		}

	BOOL Save(
		void);

	BOOL Write(
		HANDLE hFile);

  BOOL ExeWrite(
    HANDLE hFile);

	void ConvertToPointers(
		void);

	void SetDirtyFlag()
		{
		status|=FILESTAT_DIRTY;
		}

	void ClearDirtyFlag()
		{
		status&=(0XFFFFFFFFL ^ FILESTAT_DIRTY);
		}

	void SetSaveAsFlag()
		{
		status|=FILESTAT_SAVEAS;
		}

	void ClearSaveAsFlag()
		{
		status&=(0XFFFFFFFFL ^ FILESTAT_SAVEAS);
		}

	BOOL TestSaveAsFlag()
		{
		return (status & FILESTAT_SAVEAS)?TRUE:FALSE;
		}

	BOOL TestDirtyFlag()
		{
		return (status & FILESTAT_DIRTY)?TRUE:FALSE;
		}

	BOOL TestOkFlag()
		{
		return (status & FILESTAT_OK)?TRUE:FALSE;
		}

	void SetNewFlag()
		{
		status|=FILESTAT_NEW;
		}

	void ClearNewFlag()
		{
		status&=(0XFFFFFFFFL ^ FILESTAT_NEW);
		}

	BOOL TestNewFlag()
		{
		return (status & FILESTAT_NEW)?TRUE:FALSE;
		}

	FILEIDSTAMP GetFileIDStamp()
		{
		return idStamp;
    }

	void ClearSplashScreenFlag();

	void SetSplashScreenFlag();

	BOOL TestSplashScreenFlag();

	void SetSplashBits()
		{
		I_ASSERT(displaySplash);

		if(displaySplash->ValidSplash())
			SetSplashScreenFlag();
		else
			ClearSplashScreenFlag();
		}

	void ShowSplashScreen()
		{
		if(TestSplashScreenFlag())
			{
			I_ASSERT(displaySplash && displaySplash->ValidSplash());
			displaySplash->ShowSplashScreen();
			}
		}

	void HideSplashScreen()
		{
		if(TestSplashScreenFlag())
			{
			I_ASSERT(displaySplash && displaySplash->ValidSplash());
			displaySplash->HideSplashScreen();
			}
		}

	DIBITMAP* GetSplashPtr()
		{
		I_ASSERT(displaySplash);
		return displaySplash->GetSplashPtr();
		}

	WORD GetSplashTimeDelay()
		{
		I_ASSERT(displaySplash);
		return displaySplash->GetTimeDelay();
		}

	void SetSplashTimeDelay(WORD delay)
		{
		I_ASSERT(displaySplash);
		displaySplash->SetTimeDelay(delay);
		}

	};

#endif


