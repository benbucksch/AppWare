//----------------------------------------------------------------------
//   serdef.h
//
// Standard definitions for Serius environment and shell.
//
// jon 5-Feb-93
//----------------------------------------------------------------------

#ifndef __SERDEF_H
#define __SERDEF_H

  // For casting the integral portion of a (p + n) expression.
  // Use instead of 'size_t' or 'ptrdiff_t' so it's easy to grep
  // for if we need to.
  typedef int       PTRMATH_T;
  typedef unsigned  U_PTRMATH_T;

#endif // __SERDEF_H
