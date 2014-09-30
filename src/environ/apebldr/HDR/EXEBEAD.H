// exebead.h -- descriptor of a runtime bead

#ifndef __EXEBEAD_H
#define __EXEBEAD_H

#include "seruid.h"
#include "kernel.h"

class EXEBEAD
  {

friend class EXEBEADLIST;

private:

  LONG    nextIdx;  // index of next bead on this strand

  SERUID  uid;      // a UID of a EXEFUNCINST, EXEOBJINST, or
                    // null if this bead is for a EXERESPONSE

  ATYPEID  typeID;   // typeID for event tieback,
                    // ignored if this->uid != NULLSERUID

  AEVENTID eventID;  // eventID for event tieback,
                    // ignored if this->uid != NULLSERUID

public:
  void SetNextIdx(
    LONG theNext)
    {
    nextIdx = theNext;
    }

  LONG GetNextIdx()
    {
    return nextIdx;
    }

  void SetUID(
    SERUID theUID)
    {
    uid = theUID;
    }

  SERUID GetUID()
    {
    return uid;
    }

  void SetTypeID(
    ATYPEID theTypeID)
    {
    typeID = theTypeID;
    }

  ATYPEID GetTypeID()
    {
    return typeID;
    }

  void SetEventID(
    AEVENTID theEventID)
    {
    eventID = theEventID;
    }

  AEVENTID GetEventID()
    {
    return eventID;
    }

  };

#endif  // __EXEBEAD_H
