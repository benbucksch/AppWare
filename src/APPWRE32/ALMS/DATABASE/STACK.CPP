/*--------------------------------------------------------------------------*\
 | FILE		: STACK.CPP                                                 |
 | DESCRIPTION	: Stack.                                                    |
 | COPYRIGHT	: 1993 Novell, Inc.                                         |
 | EDIT LOG	:                                                           |
 |   v1.00.00	  mw	01-Sep-1993  Initial Edit                           |
\*--------------------------------------------------------------------------*/
#include "stack.h"
#include "safemem.h"


Stack::Stack			/* ---- Stack ----------------------------- */
( void
)
/* The constructor. */
{
  stack = 0;
} // Stack

Stack::~Stack			/* ---- Stack ----------------------------- */
( void
)
/* The destructor. */
{ StackNode		*node;		// stack node

  while (stack)
  { node = stack;
    stack = stack->next;
    MemFree(node);
  } // while
} // ~Stack

void *Stack::Pop		/* ---- Pop ------------------------------- */
( void
)
/* Returns the value removed from the stack. */
{ void			*val;		// value
  StackNode		*node;		// stack node

  if (!stack)
    return 0;
  val = stack->val;
  node = stack;
  stack = stack->next;
  MemFree(node);
  return val;
} // Pop

void Stack::Push		/* ---- Push ------------------------------ */
( void			*val
)
/* Pushes the value into the stack. */
{ StackNode		*node;		// stack node

  node = (StackNode *) MemAlloc(sizeof(StackNode));
  node->val = val;
  node->next = stack;
  stack = node;
} // Push
