#ifndef __OBJECT_H
#define __OBJECT_H

#include "compuid.h"
#include "classdef.h"
#include "datasmgr.h"

#define OBJ_UNTYPED       0x00000001
#define OBJ_TEMPORARY     0x00000002
#define OBJ_UNINITIALIZED 0x00000004
#define OBJ_OK            0x00000008
#define OBJ_INVALIDTYPE   0x00000010
#define OBJ_PERSISTFLAG   0x00000020
#define OBJ_ALIASFLAG     0x00000040
#define OBJ_SHAREFLAG     0x00000080
#define OBJ_DESIGNTIME    0x00000100
#define OBJ_RUNTIME       0x00000200


class OBJECTRECORD:public DATASMGR
	{

  friend LPOBJECTRECORD GetObjectRecord(
    AOBJECTID  theObject);


        friend LONG ALMAPI  AObjEdit(
		AOBJECTID   theObject);

        friend BOOL ALMAPI  AObjExport(
		AOBJECTID   theObject);

        friend BOOL ALMAPI  AObjImport(
		AOBJECTID   theObject);

        friend LONG ALMAPI AObjGetName (
		AOBJECTID  theObject,
		LPSTR     theName,
		LONG      nameSize);

        friend ATYPEID ALMAPI AObjGetType (
		AOBJECTID  theObject);

        friend BOOL ALMAPI AObjCheckType(
		AOBJECTID  theObject,
		ATYPEID    theType);

        friend AOBJECTID ALMAPI AObjGetNextObject(
		ATYPEID typeID,
		LPLONG theGetKey);

	friend void dumpRuntimeInfo();


private:
  OBJECTRECORD(void);

	//Should only be called by DumpRunTimeInfo to force the use count
	//of Temporary objects to zero so they can be destroyed.
	void ClearUseCount()
		{
		useCount = 0;
		}

protected:
	COMPUID           typeID;
  SERUID            typeRecordUID;
	unsigned long     objectStatus;
	long              useCount;
  long              runTimeData;


	long GetDatasByteCount(
		long data);

	void DeleteAllDatas();

	void CreateDatasBlock(
		long numDatas);

public:

	OBJECTRECORD(
		COMPUID           objTypeID,
		LPRUNTIMERECLIST  pDatasRecList,
		SERUID            typeUID):
      DATASMGR(pDatasRecList),
			typeID(objTypeID),
      typeRecordUID(typeUID),
			useCount(0L)
		{
		objectStatus = (OBJ_UNINITIALIZED);

    runTimeData = 0;

		if (objTypeID == NULLCOMPUID)
			objectStatus |= OBJ_UNTYPED;
		}

  //This constuctor constructs a record read from disk
	OBJECTRECORD(
		LPRUNTIMERECLIST  pDatasRecList):
      DATASMGR(0, pDatasRecList)  //FALSE means only initialize recList
		{
    runTimeData = 0;
		}

	OBJECTRECORD(
    COMPUID           objTypeID,
		LPRUNTIMERECLIST  pDatasRecList,
		LPDATASMGR        pSrcDataMgr,
		SERUID		        typeUID):
      DATASMGR(pDatasRecList, pSrcDataMgr),
			typeID(objTypeID),
			typeRecordUID(typeUID),
			useCount(0L)
		{
		objectStatus = (OBJ_UNINITIALIZED);

    runTimeData = 0;

		if (objTypeID == NULLCOMPUID)
			objectStatus |= OBJ_UNTYPED;
		}


	~OBJECTRECORD(
		void)
		{
		}

	COMPUID GetTypeID()
		{
		return typeID;
		}

  SERUID GetTypeRecUID(void)
    {
    return this->typeRecordUID;
    }

  long GetRunTimeData(void)
    {
    return this->runTimeData;
    }

  BOOL SetRunTimeData(
    long newData)
    {
    this->runTimeData = newData;
    return TRUE;
    }

	long GetObjectStatus()
		{
		return objectStatus;
		}

	long GetUseCount()
		{
		return useCount;
		}

	long IncrementUseCount()
		{
		return ++useCount;
		}

	long DecrementUseCount()
		{
		return --useCount;
		}

	void SetTemporaryFlag(
		void)
		{
		objectStatus |= OBJ_TEMPORARY;
		}

	void ClearTemporaryFlag(
		void)
		{
		objectStatus &= (~OBJ_TEMPORARY);
		}

	BOOL TestTemporaryFlag(
		void)
		{
		return (0 != (objectStatus & OBJ_TEMPORARY));
		}

	void SetPersistentsFlag()
		{
		objectStatus |= OBJ_PERSISTFLAG;
		}

	void ClearPersistentsFlag()
		{
		objectStatus &= ~OBJ_PERSISTFLAG;
		}

	BOOL TestPersistentsFlag()
		{
		if (objectStatus & OBJ_PERSISTFLAG)
			return TRUE;
		else
			return FALSE;
		}

	void SetAliasFlag()
		{
		objectStatus |= OBJ_ALIASFLAG;
		}

	void ClearAliasFlag()
		{
		objectStatus &= ~OBJ_ALIASFLAG;
		}

	BOOL TestAliasFlag()
		{
		if (objectStatus & OBJ_ALIASFLAG)
			return TRUE;
		else
			return FALSE;
		}

	void SetShareFlag()
		{
		objectStatus |= OBJ_SHAREFLAG;
		}

	void ClearShareFlag()
		{
		objectStatus &= ~OBJ_SHAREFLAG;
		}

	BOOL TestShareFlag()
		{
		if (objectStatus & OBJ_SHAREFLAG)
			return TRUE;
		else
			return FALSE;
		}


  };

#endif
