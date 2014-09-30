/*--------------------------------------------------------------------------*/
/* CmpFunc.h: Compare Functions.                                            */
/* An utility program for comparing two values of a specified type.         */
/* Serius Corporation, October 15, 1992.                                    */
/*--------------------------------------------------------------------------*/
#ifndef _CMPFUNC_H_
#define _CMPFUNC_H_

#include <windows.h>

int	CmpExact(char *, char *, int);
/* Returns -int, 0, or +int, if T < S, T = S, or T > S.  Respectively. */

int	CmpPart(char *, char *, int);
/* Returns -int, 0, or +int, if T < S, T = S, or T > S.  Respectively. */

int	(*(GetCmpExactFunc(int)))(char *, char *);
/* Returns pointer to the compare function returning int.
   The return value of -int, 0, or +int, if T < S, T = S, or T > S.
   Respectively.
*/

int	(*(GetCmpExactGEFunc(int)))(char *, char *);
/* Returns pointer to the compare function returning int.
   The return value of -int, or 0, if T < S, T >= S.  Respectively.
*/

int	(*(GetCmpExactLTFunc(int)))(char *, char *);
/* Returns pointer to the compare function returning int.
   The return value of 0, or +int, if T < S, T >= S.  Respectively.
*/

int	(*(GetCmpPartFunc(int)))(char *, char *);
/* Returns pointer to the compare function returning int.
   The return value of -int, 0, or +int, if T < S, T c= S, or T > S.
   Respectively.
*/

int	(*(GetCmpPartGEFunc(int)))(char *, char *);
/* Returns pointer to the compare function returning int.
   The return value of -int, or 0, if T < S, T c>= S.  Respectively.
*/

int	(*(GetCmpPartLEFunc(int)))(char *, char *);
/* Returns pointer to the compare function returning int.
   The return value of 0, or +int, if T <= S (T c S on text key), or T > S.
   Respectively.
*/

BOOL	IsWhiteSpace(char);
/* Returns TRUE, if C is white space, else return FALSE. */

int	LongCmp(const void *, const void *);
/* Returns -1, 0, or 1, if T < S, T = S, or T > S.  Respectively. */

#endif
