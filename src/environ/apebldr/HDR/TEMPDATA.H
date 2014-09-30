#ifndef __TEMPDATAS_H__
#define __TEMPDATAS_H__

#include "system.h"
#include "serblk.h"
#include "fastrecm.h"
#include "sereclib.h"

class TEMPDATAS:public SERIUSBLOCK
  {

public:

  void * operator new(
    size_t    /*size*/)
    {
    return pTheSystem->GetFileObject()->GetTempDatas()->AllocateRecord();
    }

  void operator delete(
    void *  pObject)
    {
    pTheSystem->GetFileObject()->GetTempDatas()->FreeRecord(pObject);
    }

  TEMPDATAS(
    void):
      SERIUSBLOCK()
    {
    }

  TEMPDATAS(
		long int	count):
      SERIUSBLOCK(count)
    {
    }

  static TEMPDATAS * GetObject(
    long   uid)
    {
    LPFASTRECMGR  fastRecMgr;

    fastRecMgr = pTheSystem->GetFileObject()->GetTempDatas();

    if (fastRecMgr)
      return (TEMPDATAS *)fastRecMgr->GetObject(uid);
    else
      return 0;

    }

  static TEMPDATAS * GetObject(
    SERUID   uid)
    {
    LPFASTRECMGR  fastRecMgr;

    fastRecMgr = pTheSystem->GetFileObject()->GetTempDatas();

    if (fastRecMgr)
      return (TEMPDATAS *)fastRecMgr->GetObject(uid.id);
    else
      return 0;

    }

  }; //end of TEMPDATAS class

#endif
