#include <windows.h>
#include "fileobj.h"
#include "system.h"
#include "sereclib.h"
#include "rectypes.h"
#include "memblk.h"
#include "io.h"

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        FILEHEADER
//
// CLASS.......:        FILEHEADER
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
FILEHEADER::FILEHEADER(
	LPUIDTABLE      uids):
		blockBlocks(),
		seriusBlocksPrefix(uids->GenerateUID(&seriusBlocks), NULLSERUID),
		seriusBlocks(sizeof(SERIUSBLOCK), RT_SERIUSBLOCK),
		recordBlocksPrefix(uids->GenerateUID(&recordBlocks), NULLSERUID),
		recordBlocks(sizeof(RECORDBLOCK), RT_RECORDBLOCK)
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
// PARAMETERS..:
//
// RETURN VALUE:
//
// ASSUMPTIONS.:
//
// SIDE-EFFECTS:
/////////////////////////////////////////////////////////////////////////////
FILEHEADER::FILEHEADER(
	int  hFile)
	{
	//Read the fileheader from the file

  _lread(hFile, (LPSTR)this, sizeof(FILEHEADER));

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        FILEOBJECT
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
FILEOBJECT::FILEOBJECT(
	void):
		fileStrings(1024),
		uids(),
		fileHdr(&uids)
	{

	SYSTEM::SetFileObject(this);
	status = FILESTAT_OK|FILESTAT_UNINIT;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        FILEOBJECT
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
FILEOBJECT::FILEOBJECT(
	int     hFile):
		fileStrings(),
		name(""),
		filePath(""),
		uids(),
		fileHdr(hFile)
	{
	LPSERIUSBLOCKBLOCK      tempBlockBlock;
	LPSERIUSBLOCK           tempBlock;
	LPRECORDBLOCK           tempRecordBlock;

	SYSTEM::SetFileObject(this);
	uids.InitPointer(fileHdr.seriusBlocksPrefix.UID, &fileHdr.seriusBlocks);
	uids.InitPointer(fileHdr.recordBlocksPrefix.UID, &fileHdr.recordBlocks);

	//Begin reading Blocks

	fileHdr.blockBlocks.Read(hFile);

	SERIUSBLOCKBLOCK::InitUIDTable(&fileHdr.blockBlocks, &uids);

	tempBlockBlock = NULL;
	while (0 != (tempBlockBlock =
				       SERIUSBLOCKBLOCK::NextRec(&fileHdr.blockBlocks, tempBlockBlock)))
		tempBlockBlock->Read(hFile);

	fileHdr.seriusBlocks.InitUIDTable();

	tempBlock = NULL;
	while (0 != (tempBlock = (LPSERIUSBLOCK)fileHdr.seriusBlocks.NextRec(tempBlock)))
		tempBlock->Read(hFile);

	fileHdr.recordBlocks.InitUIDTable();

	//read in the data base strings

	fileStrings.Read(hFile);

	tempRecordBlock = NULL;
	while (0 != (tempRecordBlock =
				       (LPRECORDBLOCK)fileHdr.recordBlocks.NextRec(tempRecordBlock)))
		tempRecordBlock->InitUIDTable();

  uids.BuildFreeList();

	status = FILESTAT_OK;

	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:        ~FILEOBJECT
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
FILEOBJECT::~FILEOBJECT()
	{

	}




