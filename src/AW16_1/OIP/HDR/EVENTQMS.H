//  *IMPORTANT* Include eventqms.i somewhere after the include of this
//              file.


#ifndef _EVENTQUEUEMSG_H
#define _EVENTQUEUEMSG_H

#include  <memory.h>

class EVENTQUEUEMSG
  {

  friend class EVENTQUEUE;

private:

   MSG  msg;

public:

  void far* operator new(
    size_t  size);

  void operator delete(
    void far*  deadObject,
    size_t     size);

  };

typedef EVENTQUEUEMSG far *LPEVENTQUEUEMSG;

#endif // _EVENTQUEUEMSG_H
