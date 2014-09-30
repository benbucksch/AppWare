/* ***********************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * *********************************************************************** */

#ifndef O_TEXT_H
#define O_TEXT_H

/* ***********************************************************************
 * TYPEID for Text Object
 * *********************************************************************** */

#define OTYPE_TEXT              MAKETYPEID('T', 'e', 'x', 't')

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

WORD ALMAPI OTextGetToBuffer(OBJECTID oiText, LPSTR szBuffer, WORD bufferLen);
WORD ALMAPI OTextGetToMemblock(OBJECTID oiText, MEMBLOCKID miBuffer);

BOOL ALMAPI OTextSetValueFromBuffer(OBJECTID oiText, LPSTR szBuffer);
BOOL ALMAPI OTextSetValueFromMemblock(OBJECTID oiText, MEMBLOCKID miBuffer);


/* ***********************************************************************
 * Getting and setting from non-Windows representations
 * *********************************************************************** */

WORD ALMAPI OTextGetToBufferEx(OBJECTID oiText, LPSTR szBuffer, WORD bufferLen, OTEXT_CRLF_TYPE fCrLf);
WORD ALMAPI OTextGetToMemblockEx(OBJECTID oiText, MEMBLOCKID miBuffer, OTEXT_CRLF_TYPE fCrLf);

BOOL ALMAPI OTextSetValueFromBufferEx(OBJECTID oiText, LPSTR szBuffer);
BOOL ALMAPI OTextSetValueFromMemblockEx(OBJECTID oiText, MEMBLOCKID miBuffer);


/* ***********************************************************************
 * Miscellaneous
 * *********************************************************************** */

WORD  ALMAPI OTextGetLength(OBJECTID oiText);
WORD  ALMAPI OTextGetLimit(OBJECTID oiText);
short ALMAPI OTextCompareWithBuffer(OBJECTID oiText, LPSTR szBuffer);
short ALMAPI OTextCompareWithMemblock(OBJECTID oiText, MEMBLOCKID miBuffer);


#ifdef __cplusplus
}     /* extern "C" */
#endif

#endif  /* !O_TEXT_H */
