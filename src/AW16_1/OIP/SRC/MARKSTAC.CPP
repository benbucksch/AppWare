// markstac.cpp -- a stack for the marks during atOnce processing.

	#include "almsys.h"

  #include "eventqnd.h"
  #include "markstac.h"

MARKSTACK::~MARKSTACK()
  {
  LPMARKNODE  p, q;

 // wipe off the mark stack
  p = head;
  while (p)
    {
    q = p->next;
    delete p;
    p = q;
    }
  }

BOOL MARKSTACK::Push(LPEVENTQUEUENODE newMark)
  {
  LPMARKNODE newNode = (LPMARKNODE) new MARKNODE;
  newNode->next = head;
  newNode->mark = newMark;
  head = newNode;
  return TRUE;
  }

LPEVENTQUEUENODE MARKSTACK::Pop()
  {
  LPEVENTQUEUENODE retMark;
  LPMARKNODE       newHead;
  if (head)
    {
    retMark = head->mark;
    newHead = head->next;
    delete head;
    head = newHead;
    }
  else
    retMark = 0;

  return retMark;
  }

BOOL  MARKSTACK::Replace(LPEVENTQUEUENODE oldMark, LPEVENTQUEUENODE newMark)
  {
  LPMARKNODE  p;

  for (p = head ; p ; p = p->next)
    if (p->mark == oldMark)
      p->mark = newMark;

  return TRUE;
  }
