/*--------------------------------------------------------------------------*\
 | FILE		: STRUTL.CPP                                                |
 | DESCRIPTION	: String Utilities.                                         |
 | COPYRIGHT	: Novell, Inc.                                              |
 | EDIT LOG	:                                                           |
 |   v1.00.00	  mw	01-Sep-1993  Initial Edit                           |
\*--------------------------------------------------------------------------*/
#include "strutl.h"


BOOL StrUtl::IsVarChar		/* ---- Is Variable Character? ---------- */
( const unsigned char c			// character
)
/* Returns true if the char is a variable character, else return false. */
{
  return !(
    c == ' '  || c == '\t' || c == '\r' || c == '\n' ||	// white spaces
    c == ':'  || c == ','  || c == '('  || c == ')'  ||	// resrvd characters
    c == '.'  || c == '"'  || c == '\'' || c == '@'  ||
    c == '+'  || c == '-'  || c == '*'  || c == '/'  ||	// operators
    c == '^'  || c == '%'  || c == '!'  || c == '~'  ||
    c == '%'  || c == '='  || c == '<'  || c == '>'  || // expressions
    c == '|'  || c == '&');
} // IsVarChar

BOOL StrUtl::IsWhiteSpace       /* ---- Is White Space? ------------------- */
( char			c               // character
)
/* Returns TRUE if the character is white space, else return FALSE. */
{
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
} // IsWhiteSpace

void StrUtl::LTrim		/* ---- Left Trim ------------------------- */
( char			*s		// string
)
/* Trims leading white spaces of the string. */
{ char			*p = s;		// pointer

  if (!IsWhiteSpace(*s))
    return;				// nothing to trim
  /* trims at lease 1 white space */
  while (IsWhiteSpace(*++s))
    /* Do Nothing */;
  lstrcpy(p, s);
} // LTrim

void StrUtl::LRTrim		/* ---- Left and Right Trim --------------- */
( char			*s		// string
)
/* Trims leading, and trailing white spaces of the string. */
{
  RTrim(s);
  LTrim(s);
} // Trim

void StrUtl::RTrim		/* ---- Right Trim ------------------------ */
( char			*s		// string
)
/* Trims trailing white spaces of the string. */
{ int			i;		// counter

  if ((i = lstrlen(s) - 1) < 0)
    return;				// nothing to trim
  if (!IsWhiteSpace(s[i]))
    return;				// nothing to trim
  /* trims at least 1 white space */
  for (i--; i >= 0 && IsWhiteSpace(s[i]); i--);
    /* Do Nothing */
  s[i + 1] = 0;
} // Rtrim

void StrUtl::Trim		/* ---- Trim ------------------------------ */
( char			*s		// string
)
/* Trims trailing white spaces of the string. */
{
  RTrim(s);
} // Trim

void StrUtl::TrimVar		/* ---- Trim Variable --------------------- */
( char			*s		// string
)
/* Trims invalid characters of the string. */
{ char			*p;		// pointer

  for (p = s; *p; p++)
    if (IsVarChar(*p))			// is valid character
    { if (s != p)
	*s = *p;			// trims character
      s++;
    } // if
  if (s != p)
    *s = 0;				// trims trailing characters
} // TrimVar
