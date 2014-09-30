#ifndef __CFGUPDAT_H__
#define __CFGUPDAT_H__

#include "ememstr.h"
#include "strspace.h"
#include "version.h"
#include "uidtable.h"

typedef void (CALLBACK*     CFGUPDATECALLBACK)(short, void far*);

#define  CFGUPD_DOFILEOBJECT    1
#define  CFGUPD_DOSERIUSBLOCK   2
#define  CFGUPD_DORECORDBLOCK   3

struct FILEOBJECTDATA
  {
	char				   idString[ID_STRING_LEN+1];
	WORD				   majorVersion;
	WORD				   minorVersion;
	WORD				   fileType;
  LPSTR          pathName;
  LPSTR          fileName;
  LPUIDTABLE     pUidTable;
  LPSTRINGSPACE  pStringSpace;
  long           headRec;
  long           headBlock;
  };

typedef FILEOBJECTDATA far*  LPFILEOBJECTDATA;

struct SERIUSBLOCKDATA
  {
  long        selfID;
	long 				blockMemID;
	long 				byteCount;
	void far *	theData;
  };

typedef SERIUSBLOCKDATA far* LPSERIUSBLOCKDATA;

struct RECORDBLOCKDATA
  {
  long            selfID;
 	short				    type;
	long int				count;
	long int				recordSize;
	short						blockCount;      //if blockCount is 1, blockID's holds the
	long 					  blockID;         //id of the block, if blockCount > 1
  };

typedef RECORDBLOCKDATA far* LPRECORDBLOCKDATA;

extern "C" {

BOOL far pascal _export ConvertFile(
  int                hFile,
  LPSTR              pathName,
  LPSTR              fileName,
  LPSERIUSMEMORY     pTheMemory,
  LPSTRINGSPACE      pStringSpace,
  CFGUPDATECALLBACK  pCallBack);

}

#endif