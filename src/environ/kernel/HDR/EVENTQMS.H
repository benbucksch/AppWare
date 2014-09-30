//  *IMPORTANT* Include eventqms.i somewhere after the include of this
//              file.


#ifndef _EVENTQUEUEMSG_H
#define _EVENTQUEUEMSG_H

class EVENTQUEUEMSG
  {

  friend class EVENTQUEUE;

private:

   MSG  msg;

public:

  void * operator new(
    size_t  size);

  void operator delete(
    void *  deadObject,
    size_t     size);

  };

typedef EVENTQUEUEMSG  *LPEVENTQUEUEMSG;

#endif // _EVENTQUEUEMSG_H
