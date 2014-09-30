/*                                                                        *\
   Copyright (c) 1993-1994 Novell, Inc.  All rights reserved.

   THIS WORK IS SUBJECT TO U.S.  AND  INTERNATIONAL  COPYRIGHT  LAWS  AND
   TREATIES.  NO PART OF THIS WORK MAY  BE  USED,  PRACTICED,  PERFORMED,
   COPIED,   DISTRIBUTED,   REVISED,   MODIFIED,   TRANSLATED,  ABRIDGED,
   CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,  TRANSFORMED
   OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY  USE
   OR EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD  SUBJECT  THE
   PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
\*                                                                        */

/*------------------------------------------------------------------------*\
 | FILE         : O_SQL.H                                                 |
 | DESCRIPTION  : AppWare SQL Objects Library Include File.               |
 | COPYRIGHT    : 1993-1994 Novell, Inc.                                  |
\*------------------------------------------------------------------------*/
#ifndef O_SQL_H                         /* SQL Object */
#define O_SQL_H

#ifdef A_ALM_H                          /* AppWare Loadable Module */


#ifdef OCIDFN                           /* Oracle Call Interface */

#include "ocidfn.h"


/* ---- TYPEID for Oracle Connection Object ---- */
#define OTYPE_OCNC      MAKETYPEID('O','c','n','c')

#ifdef __cplusplus
extern "C" {                            /* Assumes C declarations for C++ */
#endif /* __cplusplus */

BOOL ALMAPI OOcncGetLDA (
    OBJECTID            ocncObj,        /* I: Oracle Connection object */
    Lda_Def             *lda);          /* O: Logon Data Area */
/* Returns TRUE or FALSE as the result of getting the LDA. */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OCIDFN */


#endif /* A_ALM_H */

#endif /* O_SQL_H */
