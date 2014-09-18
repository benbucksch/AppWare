/* ***********************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * *********************************************************************** */

#ifndef O_TIME_H
#define O_TIME_H

/* ***********************************************************************
 * TYPEID for Time Object
 * *********************************************************************** */

#define OTYPE_TIME                MAKETYPEID('T','i','m','e')
#define OTIME_EVT_TIMEUPDATE      MAKEEVENTID('t','m','u','P')

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
  short         hour;
  short         min;
  short         sec;
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
                  OBJECTID        oiTime);

/* Time_ObjectToText */
short ALMAPI OTimeConvertToString(
                  OBJECTID        oiTime,
                  LPSTR           szBuffer,
                  short           bufferLen);

/* Time_TimeToText */
short ALMAPI OTimeFormatTime(
                  OBJECTID        oiTime,
                  OTIME           theTime,
                  LPSTR           szBuffer,
                  short           bufferLen);

/* Time_TextToObject */
BOOL  ALMAPI OTimeConvertFromString( /*return FALSE if the time cannot be set*/
                  OBJECTID        oiTime,
                  LPSTR           szString);

/* Time_TimeToObject */
BOOL  ALMAPI OTimeSetValue(       /*return FALSE if the time cannot be set*/
                  OBJECTID        oiTime,
                  OTIME           theTime);

void  ALMAPI OTimeToHMS(
                  OTIME           theTime,
                  pOTIMEHMS       hmsTime);

OTIME ALMAPI OTimeFromHMS(
                  pOTIMEHMS       hmsTime);

long  ALMAPI OTimeGetStyle(       /*returns OTIME_STYLE_<SECONDS,KEEPCURRENT>*/
                  OBJECTID        oiTime);

long  ALMAPI OTimeParseString(    /*returns OTIME_<HOUR,MIN,SEC>_SUPPLIED  */
                  LPSTR           szString);

#ifdef __cplusplus
}     /* extern "C" */
#endif

#endif  /* O_TIME_H */
