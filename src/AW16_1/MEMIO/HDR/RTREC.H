#ifndef __RTREC_H
#define	__RTREC_H

#include "rtreclst.h"
#include "system.h"

class RUNTIMEREC:public LISTMEMBER, public MEMORY
	{
private:
  LPRUNTIMERECLIST  pRecList;
  RUNTIMEUID        uid;

	RUNTIMEREC(
		void)
    {
    }

public:

	RUNTIMEREC(
		LPRUNTIMERECLIST  pRecList)
    {
    this->pRecList = pRecList;
    pRecList->Add(this);
    this->uid = pTheSystem->GetFileObject()->GetUIDTable()->
     GenerateUID(this, pRecList->GetType());
    }

	RUNTIMEREC(
		LPRUNTIMERECLIST  pRecList,
    RUNTIMEUID        uid,
    BOOL              databaseUpdate = FALSE)
    {
    this->pRecList = pRecList;
    this->uid = uid;

    if (databaseUpdate == TRUE)
      pRecList->Add(this);
    else
      {
      pTheSystem->GetFileObject()->GetUIDTable()->
       InitPointer(uid, this, pRecList->GetType());
      pRecList->AddFromFile(this);
      }

    }

	virtual ~RUNTIMEREC(
		void)
    {
    pTheSystem->GetFileObject()->GetUIDTable()->
     ReleaseUID(uid);
    this->pRecList->Remove(this);
    }

  RUNTIMEUID GetUID(void)
    {
    return uid;
    }

  short GetType(void)
    {
    return this->pRecList->GetType();
    }

  static void UpdateUIDTable(
    LPRUNTIMERECLIST  pRecList);

	};

#endif
