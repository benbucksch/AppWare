// exeproc.h -- runtime DLL entry point descriptor

#ifndef __EXEPROC_H
#define __EXEPROC_H

#include "kernel.h"
#include "exstring.h"
#include "seruid.h"

class EXEPROC
  {

friend class EXEPROCLIST;

private:

  EXESTRING  name;         // the name of this entry point
  PROC    addr;         // (filled in at runtime via ::GetProcAddress)
  LONG       firstBeadIdx; // index of first bead on this proc's strand

  void SetFirstBeadIdx(
    LONG theIdx)
    {
    firstBeadIdx = theIdx;
    }

  LONG GetFirstBeadIdx()
    {
    return firstBeadIdx;
    }

public:

  EXEPROC()
    : name()
    {
    }

  // A dummy delete to override default ..
  // .. since we never 'new' EXEPROCs
  void operator delete(
    void *  /*theExeProc*/,
	  size_t     /*theSize*/)
	  {
	  }

  ~EXEPROC()
    {
    }

 ///////////////////////////////////////////////////////////////////////
 //   EXEPROC::Init
 //
 // Initialize our memory space.  Do not use if the 'name' data member
 // has already been init'd, since we won't be able to reclaim
 // EXESTRING space.
 ///////////////////////////////////////////////////////////////////////
  void Init()
    {
   // make sure the name has a nil EXESTRING value
    new(&name) EXESTRING;
    addr = (PROC)0;
    firstBeadIdx = -1;
    }

 ///////////////////////////////////////////////////////////////////////
 //   EXEPROC::SetName
 ///////////////////////////////////////////////////////////////////////
  void SetName(
    LPCSTR theName)
    {
    name = theName;
    }

  void SetAddress(
    PROC theAddr)
    {
    addr = theAddr;
    }

  LPCSTR GetName()
    {
    return name;
    }

  PROC GetAddress()
    {
    return addr;
    }

  BOOL AddBeadFor(
    SERUID  theUID)
    {
    return this->DoBindBead(theUID, 0, 0);
    }

  BOOL AddBeadFor(
    AEVENTID theEvent,
    ATYPEID  theType)
    {
    return this->DoBindBead(NULLSERUID, theEvent, theType);
    }

  BOOL EXEPROC::GetNextBead(
    LPEXEBEAD  * theNextBead,
    LONG       * theKey);

private:

  BOOL DoBindBead(
    SERUID  theUID,
    AEVENTID theEvent,
    ATYPEID  theType);

  };

#endif  // __EXEPROC_H
