#ifndef __FILEOBJ_H__
#define __FILEOBJ_H__

#include "uidtable.h"
#include "serblk.h"
#include "recblk.h"
#include "blkblk.h"
#include "seruid.h"
#include "tstring.h"
#include "strspace.h"
#include "listmbr.h"
#include "version.h"

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

//	SERIUSBLOCK			exeBlockBlocks;
  char            dummy5[sizeof(SERIUSBLOCK)];

	RECORDPREFIX		exeSeriusBlocksPrefix;

//	EXERECORDBLOCK	exeSeriusBlocks;
  char            dummy4[sizeof(RECORDBLOCK)];

	RECORDPREFIX		exeRecordBlocksPrefix;

//	EXERECORDBLOCK	exeRecordBlocks;
  char            dummy3[sizeof(RECORDBLOCK)];

	RECORDPREFIX		memBlocksPrefix;
//	EXERECORDBLOCK  memBlocks;
  char            dummy2[sizeof(RECORDBLOCK)];


  RECORDPREFIX    runtimeDatasPrefix;

//  EXERECORDBLOCK  runtimeDatas;
  char            dummy1[sizeof(RECORDBLOCK)];

  //to account for space taken by now defunct tempObjects
//  char            dummy1[sizeof(RECORDPREFIX) + sizeof(EXERECORDBLOCK)];

  char            dummy[sizeof(RECORDPREFIX) + sizeof(SERIUSBLOCK) +
                        sizeof(RECORDBLOCK)];

	SERIUSBLOCK			blockBlocks;
	RECORDPREFIX		seriusBlocksPrefix;
	RECORDBLOCK			seriusBlocks;
	RECORDPREFIX		recordBlocksPrefix;
	RECORDBLOCK			recordBlocks;
	SERUID					headBlock;
	SERUID					headRec;
	SERUID					headExeBlock;
	SERUID					headExeRec;

	FILEHEADER(
		LPUIDTABLE	uids);

	FILEHEADER(
		int  hFile);

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

class FILEOBJECT:public LISTMEMBER
	{

private:
	STRINGSPACE			fileStrings;
	UIDTABLE		 		uids;
	FILEHEADER   		fileHdr;
	TSTRING			 		filePath;
	TSTRING			 		name;
	DWORD				 		status;

  unsigned long WriteShellExe(
    int  hFile);

public:

	static BOOL CheckConfigFile(
		int 		hFile);

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
		int     hFile);

	~FILEOBJECT();

	LPSERIUSBLOCK	GetBlockBlocks(
		void)
		{
		return &fileHdr.blockBlocks;
		}

	LPRECORDBLOCK	GetSeriusBlocks(
		void)
		{
		return &fileHdr.seriusBlocks;
		}

	LPRECORDBLOCK	GetRecordBlocks(
		void)
		{
		return &fileHdr.recordBlocks;
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

  LPSTR GetIdString(void)
    {
    return fileHdr.idInfo.idString;
    }

  WORD GetMajorVersion(void)
    {
    return fileHdr.idInfo.majorVersion;
    }

  WORD GetMinorVersion(void)
    {
    return fileHdr.idInfo.minorVersion;
    }

  WORD GetFileType(void)
    {
    return fileHdr.idInfo.fileType;
    }


	TSTRING GetName(
		void)
		{
		return name;
		}

	BOOL SetName(
		TSTRING 	newName)
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
		TSTRING 	newPath)
		{
		filePath = newPath;
		return TRUE;
		}

	SERUID GetHeadBlock(
		void)
		{
		return fileHdr.headBlock;
		}

	SERUID GetHeadRec(
		void)
		{
		return fileHdr.headRec;
		}

	void far* GetHeadRecPtr(
		void)
		{
		return fileHdr.headRec.ptr;
		}

	BOOL SetHeadBlock(
		SERUID 	newHeadBlock)
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

	};

typedef FILEOBJECT far*	LPFILEOBJECT;

#endif


