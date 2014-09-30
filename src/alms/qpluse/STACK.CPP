/*                                                                        *\
    (c) 1993, Novell, Inc.  All rights reserved.

    THIS  WORK  IS  AN  UNPUBLISHED  WORK  AND  CONTAINS   CONFIDENTIAL,
    PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.  ACCESS TO
    THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE A  NEED  TO
    KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE  OF  THEIR  ASSIGNMENTS
    AND  (II)  ENTITIES  OTHER  THAN  NOVELL  WHO  HAVE   ENTERED   INTO
    APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS WORK MAY  BE  USED,
    PRACTICED,  PERFORMED,  COPIED,  DISTRIBUTED,   REVISED,   MODIFIED,
    TRANSLATED,  ABRIDGED,  CONDENSED,  EXPANDED,  COLLECTED,  COMPILED,
    LINKED, RECAST, TRANSFORMED, OR ADAPTED WITHOUT  THE  PRIOR  WRITTEN
    CONSENT OF NOVELL.  ANY USE OR EXPLOITATION  OF  THIS  WORK  WITHOUT
    AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO  CRIMINAL  AND  CIVIL
    LIABILITY.
\*                                                                        */

/*------------------------------------------------------------------------*\
 | FILE		: STACK.CPP                                               |
 | DESCRIPTION	: Pushdown Stack.                                         |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Jan-1994  Initial Edit                         |
\*------------------------------------------------------------------------*/
#include "stack.h"
#include "safemem.h"


Stack::Stack			/* ---- Stack --------------------------- */
( void
)
/* The constructor. */
{
  top = 0;
} // Stack

Stack::~Stack			/* ---- Stack --------------------------- */
( void
)
/* The destructor. */
{ StackNode	*node;			// node

  while (top)
  { node = top;
    top = top->next;
    MemFree(node);
  } // while
} // ~Stack

void *Stack::Pop		/* ---- Pop ----------------------------- */
( void
)
/* Returns the value removed from the stack. */
{ void		*val;			// value
  StackNode	*node;			// node

  if (top == 0)
    return 0;
  val = top->val;
  node = top;
  top = top->next;
  MemFree(node);
  return val;
} // Pop

void Stack::Push		/* ---- Push ---------------------------- */
( void		*val			// value
)
/* Pushes the value into the stack. */
{ StackNode	*node;			// node

  node = (StackNode *) MemAlloc(sizeof(StackNode));
  node->val = val;
  node->next = top;
  top = node;
} // Push
