#ifndef __RTRECLST_H__
#define __RTRECLST_H__

#include "list.h"

class RUNTIMERECLIST:public LIST
  {

private:

  short recType;
  long  count;

public:

  RUNTIMERECLIST(void):
      LIST()
    {

    }

  RUNTIMERECLIST(
    short type):
      LIST()
    {
    recType = type;
    count = 0;
    }

  BOOL ZeroCount(void)
    {
    if (IsListEmpty())
      {
      count = 0;
      return TRUE;
      }
    else
      return FALSE;
    }

  short GetType(void)
    {
    return recType;
    }

  long GetCount(void)
    {
    return count;
    }

  BOOL Add(
		LPLISTMEMBER memberPtr)
    {
    count++;
    return LIST::Add(memberPtr);
    }

  BOOL AddFromFile(
		LPLISTMEMBER memberPtr)
    {
    return LIST::Add(memberPtr);
    }

	BOOL Remove(
		LPLISTMEMBER memberPtr)
    {
    count--;
    return LIST::Remove(memberPtr);
    }

  void PurgeList(void);

  };

#endif
