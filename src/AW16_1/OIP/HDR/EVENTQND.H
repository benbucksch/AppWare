/*//////////////////////////////////////////////////////////////
//
//											Copyright 1993, Novell, Inc.
//
//		<standard disclaimers here>
//
//
//--------------------------------------------------------------
//
// FILE:
//
// AUTHOR:
//
// DESCRIPTION:
//
// CHANGES:
//
//	7/22/93		Scott
//		Removed all isOSEvent stuff since we are no longer putting
//		Windows messages in the Serius queue.
//
//////////////////////////////////////////////////////////////*/


/*[ EVENTQND.H ]======================================================*\
|
|  An event queue node is the container for a AEvtInfo struct when
|  it is being manipulated in the shell.
|
|  *IMPORTANT* Include eventqnd.i somewhere after the include of this
|              file.
\*====================================================================*/
#ifndef _EVENTQUEUENODE_H
#define _EVENTQUEUENODE_H

  #include  "eventqms.h"

  #include  <memory.h>

class EVENTQUEUENODE
  {
friend class EVENTQUEUE;
typedef EVENTQUEUENODE far *LPEVENTQUEUENODE;

private:
  static long  nodesNow;
  static long  nodesMax;
  static long  nodesTotal;

  LPEVENTQUEUENODE  next;
  LONG              flags;
  AEvtInfo       event;

	#ifdef ADBG_DEBUG
	long		nodeID;
	#endif

public:

  void far* operator new(
    size_t     size);

  void operator delete(
    void far*  deadObject,
    size_t     size);

  EVENTQUEUENODE()
    {
    this->next = 0;
    EVENTQUEUENODE::nodesNow++;
    EVENTQUEUENODE::nodesTotal++;
    if (EVENTQUEUENODE::nodesNow > EVENTQUEUENODE::nodesMax)
      EVENTQUEUENODE::nodesMax++;

		#ifdef ADBG_DEBUG
		nodeID = EVENTQUEUENODE::nodesTotal;
		#endif

    }

  ~EVENTQUEUENODE()
    {
    EVENTQUEUENODE::nodesNow--;
    }

  };

typedef EVENTQUEUENODE far *LPEVENTQUEUENODE;

#endif // _EVENTQUEUENODE_H
