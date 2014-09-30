/* ***********************************************************************
 *  
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * *********************************************************************** */

#ifndef O_BUTTON_H
#define O_BUTTON_H

/* ***********************************************************************
 * ATYPEID for Button Object
 * *********************************************************************** */

#define OTYPE_BUTTON              ATYPE_MAKEID('B','u','t','n')

/* ***********************************************************************
 * Number Object API Functions
 * *********************************************************************** */

/* For OButnGetType */
typedef enum
  {
  OBUTN_PUSH,
  OBUTN_CHECK,
  OBUTN_RADIO,
  OBUTN_INVISIBLE,
  OBUTN_BITMAP,
  OBUTN_UNKNOWN     = -1
  } OBUTN_TYPE;


#ifdef __cplusplus
extern "C" {
#endif

BOOL ALMAPI       OButnIsSelected(AOBJECTID oiButton);

BOOL ALMAPI       OButnSetSelected(AOBJECTID oiButton, BOOL bSelected);

OBUTN_TYPE ALMAPI OButnGetType(AOBJECTID oiButton);


#ifdef __cplusplus
}     /* extern "C" */
#endif

#endif  /* O_BUTTON_H */
