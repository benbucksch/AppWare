#ifndef __TEMPOBJ_H__
#define __TEMPOBJ_H__

#include "object.h"
#include "fastrecm.h"
#include "system.h"

class TEMPOBJECT:public OBJECTRECORD
  {

private:

  TEMPOBJECT(void);

public:

  void * operator new(
    size_t    size)
    {
    return pTheSystem->GetFileObject()->GetTempObjects()->AllocateRecord();
    }

  void operator delete(
    void *  pObject)
    {
    pTheSystem->GetFileObject()->GetTempObjects()->FreeRecord(pObject);
    }

	TEMPOBJECT(
		COMPUID objTypeID,
		SERUID  typeUID):
			OBJECTRECORD(objTypeID,
									 pTheSystem->GetFileObject()->GetExeSeriusBlocks(),
									 typeUID)
		{
		this->SetTemporaryFlag();
		}

	static TEMPOBJECT * GetObject(
		long   uid)
		{
		LPFASTRECMGR  fastRecMgr;

		fastRecMgr = pTheSystem->GetFileObject()->GetTempObjects();

		if (fastRecMgr)
			return (TEMPOBJECT *)fastRecMgr->GetObject(uid);
		else
			return 0;

		}

	~TEMPOBJECT()
		{
		}


	}; //end of TEMPOBJECT class


#endif
