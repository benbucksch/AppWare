#include "fastrecm.h"

  inline void far* EVENTQUEUENODE::operator new(
    size_t    size)
    {
    if (size != sizeof(EVENTQUEUENODE))
      return ::new char[size];

    return pTheSystem->GetCurrentContext()->GetEventQueueNodeMgr()->AllocateRecord();
    }

  inline void EVENTQUEUENODE::operator delete(
    void far*  deadObject,
    size_t     size)
    {
    if (size != sizeof(EVENTQUEUENODE))
      ::delete [] deadObject;

    pTheSystem->GetCurrentContext()->GetEventQueueNodeMgr()->FreeRecord(deadObject);
    }

