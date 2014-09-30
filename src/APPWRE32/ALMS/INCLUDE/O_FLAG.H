/* ***********************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * *********************************************************************** */

#ifndef O_FLAG_H                  /* prevent multiple includes */
#define O_FLAG_H

#ifndef A_ALM_H
#include "a_alm.h"
#endif

#define   OTYPE_FLAG               ATYPE_MAKEID('F', 'l', 'a', 'g')


/* function prototypes *************************************************** */

#ifdef __cplusplus
extern "C"
  {
#endif

  BOOL ALMAPI OFlagGet(AOBJECTID oiFlag);
  void ALMAPI OFlagSet(AOBJECTID oiFlag, BOOL fBool);
#ifdef __cplusplus
  }
#endif

/************************************************************************* */

#endif  /* #ifndef O_FLAG_H */
