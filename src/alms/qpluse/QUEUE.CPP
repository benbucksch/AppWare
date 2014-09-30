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
 | FILE		: QUEUE.CPP                                               |
 | DESCRIPTION	: Queue.                                                  |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Jan-1994  Initial Edit                         |
\*------------------------------------------------------------------------*/
#include "queue.h"
#include "safemem.h"


Queue::Queue			/* ---- Queue --------------------------- */
( void
)
/* The constructor. */
{
  front = rear = 0;
} // Queue

Queue::~Queue			/* ---- Queue --------------------------- */
( void
)
/* The destructor. */
{ QueueNode	*node;			// node

  while (front)
  { node = front;
    front = front->next;
    MemFree(node);
  } // while
} // ~Queue

void *Queue::Deque		/* ---- Deque --------------------------- */
( void
)
/* Returns the value removed from the queue. */
{ void		*val;			// value
  QueueNode	*node;			// node

  if (front == 0)
    return 0;
  val = front->val;
  node = front;
  front = front->next;
  MemFree(node);
  return val;
} // Deque

void Queue::Enque		/* ---- Enque---------------------------- */
( void		*val			// value
)
/* Adds the value into the queue. */
{ QueueNode	*node;			// node

  node = (QueueNode *) MemAlloc(sizeof(QueueNode));
  node->val = val;
  node->next = 0;
  if (rear)
    rear->next = node;
  else
    front = node;
  rear = node; 
} // Enque
