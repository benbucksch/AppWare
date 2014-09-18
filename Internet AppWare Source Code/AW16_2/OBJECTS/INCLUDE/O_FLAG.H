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

#define   OTYPE_FLAG               MAKETYPEID('F', 'l', 'a', 'g')


/* function prototypes *************************************************** */

#ifdef __cplusplus
extern "C"
  {
#endif

  BOOL ALMAPI OFlagGet(OBJECTID oiFlag);
  void ALMAPI OFlagSet(OBJECTID oiFlag, BOOL fBool);
#ifdef __cplusplus
  }
#endif

/************************************************************************* */

#endif  /* #ifndef O_FLAG_H */
