/* ***********************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * *********************************************************************** */

#ifndef O_TEXT_H
#define O_TEXT_H

/* ***********************************************************************
 * ATYPEID for Text Object
 * *********************************************************************** */

#define OTYPE_TEXT              ATYPE_MAKEID('T', 'e', 'x', 't')

/* ***********************************************************************
 * enum for Extended APIs to ignore <CR>'s or <LF>'s
 * *********************************************************************** */
typedef enum 
  {
  OTEXT_CR,
  OTEXT_LF,
  OTEXT_CRLF
  } OTEXT_CRLF_TYPE;

#ifdef __cplusplus
extern "C" {
#endif

/* ***********************************************************************
 * Getting and setting from normal representations
 * *********************************************************************** */

int ALMAPI OTextGetToBuffer(AOBJECTID oiText, LPSTR szBuffer, int bufferLen);
int ALMAPI OTextGetToMemblock(AOBJECTID oiText, AMEMBLOCKID miBuffer);

BOOL ALMAPI OTextSetValueFromBuffer(AOBJECTID oiText, LPSTR szBuffer);
BOOL ALMAPI OTextSetValueFromMemblock(AOBJECTID oiText, AMEMBLOCKID miBuffer);


/* ***********************************************************************
 * Getting and setting from non-Windows representations
 * *********************************************************************** */

int ALMAPI OTextGetToBufferEx(AOBJECTID oiText, LPSTR szBuffer, int bufferLen, OTEXT_CRLF_TYPE fCrLf);
int ALMAPI OTextGetToMemblockEx(AOBJECTID oiText, AMEMBLOCKID miBuffer, OTEXT_CRLF_TYPE fCrLf);

BOOL ALMAPI OTextSetValueFromBufferEx(AOBJECTID oiText, LPSTR szBuffer);
BOOL ALMAPI OTextSetValueFromMemblockEx(AOBJECTID oiText, AMEMBLOCKID miBuffer);


/* ***********************************************************************
 * Miscellaneous
 * *********************************************************************** */

int  ALMAPI OTextGetLength(AOBJECTID oiText);
int ALMAPI OTextCompareWithBuffer(AOBJECTID oiText, LPSTR szBuffer);
int ALMAPI OTextCompareWithMemblock(AOBJECTID oiText, AMEMBLOCKID miBuffer);



#ifdef __cplusplus
}     /* extern "C" */
#endif

#endif  /* !O_TEXT_H */
