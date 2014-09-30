#ifndef __EXEOBJS_H
#define __EXEOBJS_H

#include "exeflwmg.h"
#include "object.h"
#include "exstring.h"
#include "ownerlst.h"
#include "tstring.h"
#include "system.h"

#define OBJ_PSEUDO 0x0001

#define EXEOBJ_TAG AMAKEID('O','B','P','F')

struct OBJECTHDR
	{
	long tag;
	SERUID objectID;
	unsigned int numDatas;
	};

struct DATAHDR
	{
	long length;
	};

class EXEOBJINST:public OBJECTRECORD, public EXEFLOWMGR
	{

  friend LPOWNERLIST GetOwnerList(     //from oipowner.cpp
    AOBJECTID  theObject);

private:
  EXESTRING objectName;
	SERUID    typeRecord;
	long 	    status;
  OWNERLIST ownerList;

public:

  EXEOBJINST(
    ATYPEID      typeID,
    TSTRING     objectName,
    long        objectStatus,
    long        useCount,
		LPDATASMGR  pSrcDatasMgr,
		SERUID			typeUID);

  EXEOBJINST(
    void *  objPtr):
      EXEFLOWMGR(objPtr),
      OBJECTRECORD(pTheSystem->GetFileObject()->GetExeSeriusBlocks()),
      objectName(&this->objectName),
      ownerList()
    {
    }

	~EXEOBJINST();

	void SetTemporaryFlag()
		{
		status|=OBJ_PSEUDO;
		}

	BOOL TestTemporaryFlag()
		{
		return (status & OBJ_PSEUDO)?TRUE:FALSE;
		}

	void UpdateName(
		LPSTR           newName)
		{
		objectName = newName;
		}

  TSTRING GetName()
		{
		return (const char*) objectName;
		}

	BOOL StoreAsPersistent(
		HANDLE hFile);

	BOOL LoadPersistentDataBlock(
    HANDLE  hFile,
    long		theDataIndex);
	};

#endif
