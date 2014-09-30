#ifndef __DATASMGR_H__
#define __DATASMGR_H__

#include "seruid.h"
#include "rtreclst.h"

#define DATASMGR_DATABLOCKTAG AMAKEID('D', 'M', 'B', 'K')

struct DATASMGR_DATABLOCKHDR
	{
	long tag;
	unsigned int numDatas;
	};

struct DATASMGR_DATAHDR
	{
	long length;
	};



class DATASMGR
  {
private:
  LPRUNTIMERECLIST  pDatasRecList;

  RUNTIMEUID        datasUID;
  long              datasCount;

  BOOL AddDatas(
    LPRUNTIMERECLIST  pDatasRecList,
    long              deltaCnt);

  BOOL DeleteDatas(
    long   deltaCnt);

  DATASMGR(VOID)
    {
    }

public:

  DATASMGR(
    LPRUNTIMERECLIST  pDatasRecList);

  DATASMGR(
    LPRUNTIMERECLIST  pDatasRecList,
    DATASMGR *     pSrcDatasMgr);

  DATASMGR(
    void *         objPtr,
    LPRUNTIMERECLIST  pDatasRecList);

  ~DATASMGR(
    void);

  BOOL AddData(
    LPVOID       theData,
    long         dataSize);

  BOOL InsertData(
    long         dataIndex,
    LPVOID       theData,
    long         dataSize);

  LPVOID LockDataPtr(
    long      dataIndex);

  BOOL UnlockDataPtr(
    long      dataIndex);

	short GetDataLockCount(
		long  dataIndex);

  long GetDataSize(
    long     dataIndex);

  BOOL RemoveData(
    long      dataIndex);

	BOOL ResizeData(
		long      dataIndex,
		long      dataSize);

	BOOL SetDataFromMemBlock(
		LONG        dataIndex,
		AMEMBLOCKID  theMemBlock);

	BOOL SetData(
    long      dataIndex,
    LPVOID    theData,
    long      dataSize);

  BOOL SetDatasCount(
    long         numDatas);

	long GetDatasCount()
		{
		return datasCount;
		}

  LPRUNTIMERECLIST GetDatasRecList(
    void)
    {
    return pDatasRecList;
		}

	BOOL StoreDatasToFile(
  	HANDLE hFile);

 	BOOL StoreDatasToMemBlock(
  	AMEMBLOCKID memBlck,
  	unsigned int headerSize);
	
	BOOL LoadDatasFromFile(
  	HANDLE   hFile);

	BOOL LoadDatasFromMemory(
  	LPVOID memBuffer);


	};

typedef DATASMGR * LPDATASMGR;

#endif
