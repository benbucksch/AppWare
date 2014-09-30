// exeevnt.h --> EXEEVENT

#ifndef __EXEEVNT_H
#define __EXEEVNT_H

#include "classdef.h"
#include "kernel.h"
#include "exerespl.h"
#include "sereclib.h"

class EXEEVENT
  {

friend class EXEEVENTLIST;

private:

  AEVENTID  eventID;
  SERUID   responseListUID;

 ///////////////////////////////////////////////////////////////////////
 //   EXEEVENT::Init
 //
 // Initialize our memory space.
 ///////////////////////////////////////////////////////////////////////
  void Init(AEVENTID theEvent)
    {
    eventID = theEvent;
    responseListUID = GetUID(new EXERESPONSELIST);
    }

public:

  // A dummy delete to override default ..
  // .. since we never 'new' these things
  void operator delete(
    void *  /*theExeEvent*/,
	  size_t     /*theSize*/)
	  {
	  }

  ~EXEEVENT()
    {
    delete this->GetResponseList();
    }

  void  SetEventID(
    AEVENTID theEvent)
    {
    eventID = theEvent;
    }

  LONG  GetEventID()
    {
    return eventID;
    }

  LPEXERESPONSELIST  GetResponseList()
    {
    return ((LPEXERESPONSELIST)GetObject(responseListUID));
    }

  };

#endif  // __EXEEVNT_H
