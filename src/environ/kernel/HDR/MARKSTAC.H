// markstac.h -- a stack for the marks during atOnce processing.

#ifndef __MARKSTACK_H
#define __MARKSTACK_H

struct MARKNODE
  {
    MARKNODE      *next;
    LPEVENTQUEUENODE  mark;
  };

typedef MARKNODE  *LPMARKNODE;

class MARKSTACK
  {
  public:  // canonical
    MARKSTACK() { head = 0; };
    ~MARKSTACK();

  public:  // cpp
    BOOL             Push(LPEVENTQUEUENODE newMark);
    LPEVENTQUEUENODE Pop();
    BOOL             Replace(LPEVENTQUEUENODE oldMark, LPEVENTQUEUENODE newMark);

  private:
    LPMARKNODE   head;
  };

#endif // __MARKSTACK_H
