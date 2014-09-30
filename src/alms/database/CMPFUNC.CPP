/*--------------------------------------------------------------------------*/
/* CmpFunc.C: Compare Functions.                                            */
/* An utility program for comparing two values of a specified type.         */
/* Serius Corporation, October 15, 1992.                                    */
/*--------------------------------------------------------------------------*/
#include <a_alm.h>
#include "cmpfunc.h"
#include "dbkey.h"

int LngCmp(char *, char *);
/* Returns -1, 0, or 1, if T < S, T = S, or T > S.  Respectively. */

int LngCmpLE(char *, char *);
/* Returns 0 or 1, if T <= S, or T > S.  Respectively. */

int LngCmpLT(char *, char *);
/* Returns 0, or 1, if T < S, or T >= S.  Respectively. */

int LDbCmp(char *, char *);
/* Returns -1, 0, or 1, if T < S, T = S, or T > S.  Respectively. */

int LDbCmpLE(char *, char *);
/* Returns 0 or 1, if T <= S or T > S.  Respectively. */

int LDbCmpLT(char *, char *);
/* Returns 0, or 1, if T < S, or T >= S.  Respectively. */

int StrCmpExact(char *, char *);
/* Returns -int, 0, or +int, if T < S, T = S, or T > S.  Respectively. */

int StrCmpExactGE(char *, char *);
/* Returns -1, or 0, if T < S, or T >= S.  Respectively. */

int StrCmpExactLT(char *, char *);
/* Returns 0, or 1, if T < S, or T >= S.  Respectively. */

int StrCmpPart(char *, char *);
/* Returns -int, 0, or +int, if T < S, T c S, or T > S.  Respectively. */

int StrCmpPartLE(char *, char *);
/* Returns 0, or 1, if T <c S, or T > S.  Respectively. */



int LongCmp			/* ---- Long Compare ---------------------- */
( const void	*t,			// target string
  const void	*s			// source string
)
/* Returns -1, 0, or 1, if T < S, T = S, or T > S.  Respectively. */
{
  return *(long *) t < *(long *) s? -1 : *(long *) t == *(long *) s? 0 : 1;
} // LngCmp

int LngCmp			/* ---- Long Compare ---------------------- */
( char		*t,			// target string
  char		*s			// source string
)
/* Returns -1, 0, or 1, if T < S, T = S, or T > S.  Respectively. */
{
  return *(long *) t < *(long *) s? -1 : *(long *) t == *(long *) s? 0 : 1;
} // LngCmp

int LngCmpGE			/* ---- Long Compare Greater or Equal ----- */
( char		*t,			// target string
  char		*s			// source string
)
/* Returns -1, or 0, if T < S, or T >= S.  Respectively. */
{
  return *(long *) t < *(long *) s? -1 : 0;
} // LngCmpGE

int LngCmpLE			/* ---- Long Compare Less or Equal -------- */
( char		*t,			// target string
  char		*s			// source string
)
/* Returns 0 or 1, if T <= S or T > S.  Respectively. */
{
  return *(long *) t <= *(long *) s? 0 : 1;
} // LngCmpLE

int LngCmpLT			/* ---- Long Compare Less Than ------------ */
( char		*t,			// target string
  char		*s			// source string
)
/* Returns 0, or 1, if T < S, or T >= S.  Respectively. */
{
  return *(long *) t < *(long *) s? 0 : 1;
} // LngCmpLT

int LDbCmp			/* ---- Long Double Compare --------------- */
( char		*t,			// target string
  char		*s			// source string
)
/* Returns -1, 0, or 1, if T < S, T = S, or T > S.  Respectively. */
{
  return *(long double *) t < *(long double *) s? -1 :
	   *(long double *) t == *(long double *) s? 0 : 1;
} // LDbCmp

int LDbCmpGE			/* ---- Long Double Compare '>=' -----------*/
( char		*t,			// target string
  char		*s			// source string
)
/* Returns -1, or 0, if T < S, or T >= S.  Respectively. */
{
  return *(long double *) t < *(long double *) s? -1 : 0;
} // LDbCmpExactLT

int LDbCmpLE			/* ---- Long Double Compare '<=' ---------- */
( char		*t,			// target string
  char		*s			// source string
)
/* Returns 0 or 1, if T <= S or T > S.  Respectively. */
{
  return *(long double *) t <= *(long double *) s? 0 : 1;
} // LDbCmpLE

int LDbCmpLT			/* ---- Long Double Compare Less Than ----- */
( char		*t,			// target string
  char		*s			// source string
)
/* Returns 0, or 1, if T < S, or T >= S.  Respectively. */
{
  return *(long double *) t < *(long double *) s? 0 : 1;
} // LDbCmpLT

int StrCmpExact			/* ---- String Compare Exact -------------- */
( char		*t,			// target string
  char		*s			// source string
)
/* Returns -int, 0, or int, if t < s, t = s, or t > s.  Respecively. */
{
  return lstrcmpi(t, s);
} // StrCmpExact

int StrCmpExactGE		/* ---- String Compare Exact '>=' ----------*/
( char		*t,			// target string
  char		*s			// source string
)
/* Returns -1, or 0, if T < S, or T >= S.  Respectively. */
{
  return lstrcmpi(t, s) < 0? -1 : 0;
} // StrCmpExactGE

int StrCmpExactLT		/* ---- String Compare Exact '<' ---------- */
( char		*t,			// target string
  char		*s			// source string
)
/* Returns 0, or 1, if T < S, or T >= S.  Respectively. */
{
  return lstrcmpi(t, s) < 0? 0 : 1;
} // StrCmpExactLT

int StrCmpPart			/* ---- String Compare Partial ------------ */
( char		*t,			// target string
  char		*s			// source string
)
/* Returns -int, 0, or +int, if T < S, T c S, or T > S.  Respectively. */
{ char		tChar[8];		// target character
  char		sChar[8];		// source character
  char		*nextChar;		// next character
  int		i;			// counter

  do
  { for (i = 0, nextChar = AnsiNext(t); t < nextChar; tChar[i++] = *t++);
    tChar[i] = 0;
    for (i = 0, nextChar = AnsiNext(s); s < nextChar; sChar[i++] = *s++);
    sChar[i] = 0;
  } while ((i = lstrcmpi(tChar, sChar)) == 0 && *t);
  return i;
} // StrCmpPart

int StrCmpPartGE		/* ---- String Compare Partial '>=' ------- */
( char		*t,			// target string
  char		*s			// source string
)
/* Returns 0, or 1, if T c>= S, or t > s.  Respectively. */
{
  return StrCmpPart(t, s) < 0? -1 : 0;
} // StrCmpPartGE

int StrCmpPartLE		/* ---- String Compare Partial '<=' ------- */
( char		*t,			// target string
  char		*s			// source string
)
/* Returns 0, or 1, if T <c S, or t > s.  Respectively. */
{
  return StrCmpPart(t, s) <= 0? 0 : 1;
} // StrCmpPartLE



int CmpExact			/* ---- Compare Exact --------------------- */
( char		*t,			// target string
  char		*s,			// source string
  int		keyType			// key type
)
/* Returns -int, 0, or +int, if T < S, T = S, or T > S.  Respectively. */
{
  switch (keyType)
  { case ODATB_KEYDATE	    :
    case ODATB_KEYTIME	    :
    case ODATB_KEYLONG	    : return LngCmp(t, s);
    case ODATB_KEYLONGDOUBLE: return LDbCmp(t, s);
    case ODATB_KEYTEXT	    :
    default		    : return StrCmpExact(t, s);
  } // switch
} // CmpExact

int CmpPart			/* ---- Compare Part ---------------------- */
( char		*t,			// target string
  char		*s,			// source string
  int		keyType			// key type
)
/* Returns -int, 0, or +int, if T < S, T = S, or T > S.  Respectively. */
{
  switch (keyType)
  { case ODATB_KEYDATE	    :
    case ODATB_KEYTIME	    :
    case ODATB_KEYLONG	    : return LngCmp(t, s);
    case ODATB_KEYLONGDOUBLE: return LDbCmp(t, s);
    case ODATB_KEYTEXT	    :
    default		    : return StrCmpPart(t, s);
  } // switch
} // CmpPart

int (*(GetCmpExactFunc		/* ---- Get Compare Exact Function -------- */
( int	keyType				// key type
)))(char *, char *)
/* Returns pointer to the compare function returning int.
   The return value of -int, 0, or +int, if T < S, T = S, or T > S.
   Respectively.
*/
{
  switch (keyType)
  { case ODATB_KEYDATE	    :
    case ODATB_KEYTIME	    :
    case ODATB_KEYLONG	    : return (int (*)(char *, char *)) LngCmp;
    case ODATB_KEYLONGDOUBLE: return (int (*)(char *, char *)) LDbCmp;
    case ODATB_KEYTEXT	    :
    default		    : return (int (*)(char *, char *)) StrCmpExact;
  } // switch
} // GetCmpExactFunc

int (*(GetCmpExactGEFunc	/* ---- Get Compare Exact '>=' Function --- */
( int		keyType			// key type
)))(char *, char *)
/* Returns pointer to the compare function returning int.
   The return value of -int, or 0, if T < S, T >= S.  Respectively.
*/
{
  switch (keyType)
  { case ODATB_KEYDATE      :
    case ODATB_KEYTIME      :
    case ODATB_KEYLONG      : return (int (*)(char *, char *)) LngCmpGE;
    case ODATB_KEYLONGDOUBLE: return (int (*)(char *, char *)) LDbCmpGE;
    case ODATB_KEYTEXT      :
    default	            : return (int (*)(char *, char *)) StrCmpExactGE;
  } // switch
} // GetCmpExactGEFunc

int (*(GetCmpExactLTFunc	/* ---- Get Compare Exact '<' Function ---- */
( int		keyType			// key type
)))(char *, char *)
/* Returns pointer to the compare function returning int.
   The return value of 0, or +int, if T < S, T >= S.  Respectively.
*/
{
  switch (keyType)
  { case ODATB_KEYDATE      :
    case ODATB_KEYTIME      :
    case ODATB_KEYLONG      : return (int (*)(char *, char *)) LngCmpLT;
    case ODATB_KEYLONGDOUBLE: return (int (*)(char *, char *)) LDbCmpLT;
    case ODATB_KEYTEXT      :
    default	            : return (int (*)(char *, char *)) StrCmpExactLT;
  } // switch
} // GetCmpExactLTFunc

int (*(GetCmpPartFunc		/* ---- Get Compare Partial Function ------ */
( int		keyType			// key type
)))(char *, char *)
/* Returns pointer to the compare function returning int.
   The return value of -int, 0, or +int, if T < S, T = S (T c S on text key),
   or T > S.  Respectively.
*/
{
  switch (keyType)
  { case ODATB_KEYDATE      :
    case ODATB_KEYTIME      :
    case ODATB_KEYLONG      : return (int (*)(char *, char *)) LngCmp;
    case ODATB_KEYLONGDOUBLE: return (int (*)(char *, char *)) LDbCmp;
    case ODATB_KEYTEXT      :
    default	            : return (int (*)(char *, char *)) StrCmpPart;
  } // switch
} // GetKeyCmpPartFunc

int (*(GetCmpPartGEFunc		/* ---- Get Compare Part '>=' Function ---- */
( int		keyType			// key type
)))(char *, char *)
/* Returns pointer to the compare function returning int.
   The return value of -int, or 0, if T < S, T c>= S.  Respectively.
*/
{
  switch (keyType)
  { case ODATB_KEYDATE      :
    case ODATB_KEYTIME      :
    case ODATB_KEYLONG      : return (int (*)(char *, char *)) LngCmpGE;
    case ODATB_KEYLONGDOUBLE: return (int (*)(char *, char *)) LDbCmpGE;
    case ODATB_KEYTEXT      :
    default	            : return (int (*)(char *, char *)) StrCmpPartGE;
  } // switch
} // GetCmpPartGEFunc

int (*(GetCmpPartLEFunc		/* ---- Get Compare Partial '<=' Function - */
( int		keyType			// key type
)))(char *, char *)
/* Returns pointer to the compare function returning int.
   The return value of 0, or +int, if T <= S (T c S on text key), or T > S.
   Respectively.
*/
{
  switch (keyType)
  { case ODATB_KEYDATE      :
    case ODATB_KEYTIME      :
    case ODATB_KEYLONG      : return (int (*)(char *, char *)) LngCmpLE;
    case ODATB_KEYLONGDOUBLE: return (int (*)(char *, char *)) LDbCmpLE;
    case ODATB_KEYTEXT      :
    default	            : return (int (*)(char *, char *)) StrCmpPartLE;
  } // switch
} // GetCmpPartLEFunc

BOOL IsWhiteSpace		/* ---- Is White Space? ------------------- */
( char          c			// a character
)
/* Returns TRUE, if C is white space, else return FALSE. */
{
  if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
    return TRUE;
  return FALSE;
} // IsWhiteSpace
