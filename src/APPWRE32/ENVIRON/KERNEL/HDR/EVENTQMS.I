#ifndef __EVENTQUEUEMSG_I
#define __EVENTQUEUEMSG_I

  inline void * EVENTQUEUEMSG::operator new(
    size_t  size)
    {
    if (size != sizeof(EVENTQUEUEMSG))
      return ::new char[size];

    return pTheSystem->GetCurrentContext()->GetEventQueueMsgMgr()->AllocateRecord();
    }

  inline void EVENTQUEUEMSG::operator delete(
    void *  deadObject,
    size_t     size)
    {
    if (size != sizeof(EVENTQUEUEMSG))
      ::delete [] deadObject;

    pTheSystem->GetCurrentContext()->GetEventQueueMsgMgr()->FreeRecord(deadObject);
    }

#endif // __EVENTQUEUEMSG_I
