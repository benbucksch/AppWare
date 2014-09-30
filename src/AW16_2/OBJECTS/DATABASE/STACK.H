/*--------------------------------------------------------------------------*\
 | FILE		: STACK.H                                                   |
 | DESCRIPTION	: Stack Include File.                                       |
 | COPYRIGHT	: 1993 Novell, Inc.                                         |
 | EDIT LOG	:                                                           |
 |   v1.00.00	  mw	01-Sep-1993  Initial Edit                           |
\*--------------------------------------------------------------------------*/
#ifndef STACK_H
#define STACK_H

typedef struct stacknode	StackNode;
struct stacknode		/* ---- Stack Node ------------------------ */
{ void			*val;		// value
  StackNode		*next;		// next
};

class Stack			/* ---- Stack ----------------------------- */
{ StackNode		*stack;		// stack

public:
  Stack(void);
  /* The constructor. */

  ~Stack(void);
  /* The destructor. */

  void *Pop();
  /* Returns the value removed from the stack. */

  void Push(void *);
  /* Pushes the value into the stack. */
};

#endif /* STACK_H */
