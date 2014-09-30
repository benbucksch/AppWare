/*--------------------------------------------------------------------------*\
 | FILE		: STRUTL.H                                                  |
 | DESCRIPTION	: String Utilities Include File.                            |
 | COPYRIGHT	: Novell, Inc.                                              |
 | EDIT LOG	:                                                           |
 |   v1.00.00	  mw	01-Sep-1993  Initial Edit                           |
\*--------------------------------------------------------------------------*/
#ifndef STRUTL_H
#define STRUTL_H

#include <windows.h>


class StrUtl			/* ---- String Utility -------------------- */
{
public:
  BOOL IsVarChar(const unsigned char);
  /* Returns TRUE if the char is a variable character, else return FALSE. */

  BOOL IsWhiteSpace(char);
  /* Returns TRUE if the character is white space, else return FALSE. */

  void LTrim(char *);
  /* Trims leading white spaces of the string. */

  void LRTrim(char *);
  /* Trims leading, and trailing white spaces of the string. */

  void RTrim(char *);
  /* Trims trailing white spaces of the string. */

  void Trim(char *);
  /* Trims trailing white spaces of the string. */

  void TrimVar(char *);
  /* Trims invalid variable characters of the string. */
};

#endif /* STRUTL_H */
