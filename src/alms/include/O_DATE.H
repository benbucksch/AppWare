/* ***********************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * *********************************************************************** */

#ifndef O_DATE_H
#define O_DATE_H

/* ***********************************************************************
 * ATYPEID for Date Object
 * *********************************************************************** */

#define OTYPE_DATE                    ATYPE_MAKEID('D','a','t','e')
#define ODATE_EVT_DATEUPDATE          AEVENT_MAKEID('d','t','u','P')


/* ***********************************************************************
 * Date Object API Functions
 * *********************************************************************** */

#define ODATE_MAX_TEXT_LENGTH         35

#define ODATE_MONTH_SUPPLIED          0x00000001L
#define ODATE_DAY_SUPPLIED            0x00000002L
#define ODATE_YEAR_SUPPLIED           0x00000004L

#define ODATE_STYLE_SHORT             0x00000001L
#define ODATE_STYLE_LONG              0x00000002L
#define ODATE_STYLE_KEEPCURRENT       0x00000004L

typedef long ODATE;

typedef struct ODATEYMD
  {
  int         year;
  int         month;
  int         day;
  int         dayOfWeek;            /*1-7=Sun-Sat*/
  } ODATEYMD;
typedef ODATEYMD far *pODATEYMD;

#ifdef __cplusplus
extern "C" {
#endif

/* Date_TextToDate */
ODATE ALMAPI    ODateStringToDate(
                      LPSTR           szString);

/* Date_ObjectToDate */
ODATE ALMAPI    ODateGetValue(
                      AOBJECTID        oiDate);

/* Date_ObjectToText */
int ALMAPI    ODateConvertToString(
                      AOBJECTID        oiDate,
                      LPSTR           szBuffer,
                      short           bufferLen);

/* Date_DateToText */
int ALMAPI    ODateFormatDate(
                      AOBJECTID        oiDate,  /*Can be 0 -- will use a default format*/
                      ODATE           theDate,
                      LPSTR           szBuffer,
                      short           bufferLen);

/* Date_TextToObject */
BOOL  ALMAPI    ODateConvertFromString(   /*return FALSE if the date cannot be set*/
                      AOBJECTID        oiDate,
                      LPSTR           szString);

/* Date_DateToObject */
BOOL  ALMAPI    ODateSetValue(            /*return FALSE if the date cannot be set*/
                      AOBJECTID        oiDate,
                      ODATE           theDate);

void  ALMAPI    ODateToYMD(
                      ODATE           theDate,
                      pODATEYMD       ymdDate);

ODATE ALMAPI    ODateFromYMD(
                      pODATEYMD       ymdDate);

long  ALMAPI    ODateGetStyle(            /*returns ODATE_STYLE_<SHORT,LONG,KEEPCURRENT>*/
                      AOBJECTID        oiDate);

long  ALMAPI    ODateParseString(         /*returns ODATE_<MONTH,DAY,YEAR>_SUPPLIED */
                      LPSTR           szString);
                              
#ifdef __cplusplus
}     /* extern "C" */
#endif

#endif  /* O_DATE_H */
