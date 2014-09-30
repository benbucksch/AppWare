//======================================================================
// exeevntl.h -- runtime event list
//
// jon 12-Jun-92 created
//======================================================================

#ifndef __EXEEVENTL_H
#define __EXEEVENTL_H

#include "classdef.h"
#include "kernel.h"
#include "exeevnt.h"
#include "exelistb.h"

class EXEEVENTLIST:public EXELISTBASE
  {

private:

  LONG GetNodeKey(
    LONG theNodeIdx)
    {
    return ((LPEXEEVENT)dataPtr + (int)theNodeIdx)->eventID;
    }

  LONG GetNodeKey(
    LPVOID theNodePtr)
    {
    return ((LPEXEEVENT)theNodePtr)->eventID;
    }

public:

  EXEEVENTLIST()
    : EXELISTBASE(sizeof(EXEEVENT))
    {
    isSortable = TRUE;
    }

  EXEEVENTLIST(
    void  * objPtr)
    : EXELISTBASE(objPtr)
    {
    }

  ~EXEEVENTLIST();

	LPEXERESPONSELIST GetResponseList(
    AEVENTID theEvent)
    {
    LPEXEEVENT  eventNow = FindNode(theEvent);
    if (eventNow)
      return eventNow->GetResponseList();
    else
      return 0;
    }

  LPEXEEVENT FindNode(
    AEVENTID theEvent)
    {
    return (LPEXEEVENT)EXELISTBASE::FindNode(theEvent);
    }

  LPEXEEVENT GetNodeFor(
    AEVENTID theEvent)
    {
    LPEXEEVENT  myNode;

    myNode = this->FindNode(theEvent);
    if (!myNode)
      {
      myNode = (LPEXEEVENT)(this->GetNewNodeAddr());
      myNode->Init(theEvent);
      }

    return myNode;
    }

  BOOL SetMethodAddr(
    PROC procAddr,
    ATYPEID  theTypeID,
    AEVENTID theEventID);

  };

#endif  // __EXEEVNTL_H
