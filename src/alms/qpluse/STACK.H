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
 | FILE		: STACK.H                                                 |
 | DESCRIPTION	: Pushdown Stack Include File.                            |
 | COPYRIGHT	: 1993 Novell, Inc.                                       |
 | EDIT LOG	:                                                         |
 |   v1.00.00	  mw	04-Jan-1994  Initial Edit                         |
\*------------------------------------------------------------------------*/
#ifndef STACK_H
#define STACK_H

typedef struct stacknode	StackNode;
struct stacknode		/* ---- Stack Node ---------------------- */
{ void		*val;			// value
  StackNode	*next;			// next
};

class Stack			/* ---- Stack --------------------------- */
{ StackNode	*top;			// top

public:
  Stack(void);
  /* The constructor. */

  ~Stack(void);
  /* The destructor. */

  void *Pop();
  /* Returns the value removed from the stack. */

  void Push(void *val);
  /* Pushes the value into the stack. */
};

#endif /* STACK_H */
