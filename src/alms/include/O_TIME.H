/* ***********************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * *********************************************************************** */

#ifndef O_TIME_H
#define O_TIME_H

/* ***********************************************************************
 * ATYPEID for Time Object
 * *********************************************************************** */

#define OTYPE_TIME                ATYPE_MAKEID('T','i','m','e')
#define OTIME_EVT_TIMEUPDATE      AEVENT_MAKEID('t','m','u','P')

/* ***********************************************************************
 * Time Object API Functions
 * *********************************************************************** */

#define OTIME_MAX_TEXT_LENGTH     35

#define OTIME_HOUR_SUPPLIED       0x00000001L
#define OTIME_MIN_SUPPLIED        0x00000002L
#define OTIME_SEC_SUPPLIED        0x00000004L

#define OTIME_STYLE_SEC           0x00000001L
#define OTIME_STYLE_KEEPCURRENT   0x00000004L

typedef long OTIME;

typedef struct OTIMEHMS
  {
  int           hour;
  int           min;
  int           sec;
  } OTIMEHMS;
typedef OTIMEHMS far *pOTIMEHMS;


#ifdef __cplusplus
extern "C" {
#endif

/* ***********************************************************************
 * Time Object API Functions
 * *********************************************************************** */

/* Time_TextToTime */
OTIME ALMAPI OTimeStringToTime(
                  LPSTR           szString);

/* Time_ObjectToTime */
OTIME ALMAPI OTimeGetValue(
                  AOBJECTID        oiTime);

/* Time_ObjectToText */
int ALMAPI OTimeConvertToString(
                  AOBJECTID        oiTime,
                  LPSTR           szBuffer,
                  short           bufferLen);

/* Time_TimeToText */
int ALMAPI OTimeFormatTime(
                  AOBJECTID        oiTime,
                  OTIME           theTime,
                  LPSTR           szBuffer,
                  short           bufferLen);

/* Time_TextToObject */
BOOL  ALMAPI OTimeConvertFromString( /*return FALSE if the time cannot be set*/
                  AOBJECTID        oiTime,
                  LPSTR           szString);

/* Time_TimeToObject */
BOOL  ALMAPI OTimeSetValue(       /*return FALSE if the time cannot be set*/
                  AOBJECTID        oiTime,
                  OTIME           theTime);

void  ALMAPI OTimeToHMS(
                  OTIME           theTime,
                  pOTIMEHMS       hmsTime);

OTIME ALMAPI OTimeFromHMS(
                  pOTIMEHMS       hmsTime);

long  ALMAPI OTimeGetStyle(       /*returns OTIME_STYLE_<SECONDS,KEEPCURRENT>*/
                  AOBJECTID        oiTime);

long  ALMAPI OTimeParseString(    /*returns OTIME_<HOUR,MIN,SEC>_SUPPLIED  */
                  LPSTR           szString);

#ifdef __cplusplus
}     /* extern "C" */
#endif

#endif  /* O_TIME_H */
