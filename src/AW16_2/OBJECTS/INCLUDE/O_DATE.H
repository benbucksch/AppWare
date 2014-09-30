/* ***********************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * *********************************************************************** */

#ifndef O_DATE_H
#define O_DATE_H

/* ***********************************************************************
 * TYPEID for Date Object
 * *********************************************************************** */

#define OTYPE_DATE                    MAKETYPEID('D','a','t','e')
#define ODATE_EVT_DATEUPDATE          MAKEEVENTID('d','t','u','P')


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
  short         year;
  short         month;
  short         day;
  short         dayOfWeek;            /*1-7=Sun-Sat*/
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
                      OBJECTID        oiDate);

/* Date_ObjectToText */
short ALMAPI    ODateConvertToString(
                      OBJECTID        oiDate,
                      LPSTR           szBuffer,
                      short           bufferLen);

/* Date_DateToText */
short ALMAPI    ODateFormatDate(
                      OBJECTID        oiDate,  /*Can be 0 -- will use a default format*/
                      ODATE           theDate,
                      LPSTR           szBuffer,
                      short           bufferLen);

/* Date_TextToObject */
BOOL  ALMAPI    ODateConvertFromString(   /*return FALSE if the date cannot be set*/
                      OBJECTID        oiDate,
                      LPSTR           szString);

/* Date_DateToObject */
BOOL  ALMAPI    ODateSetValue(            /*return FALSE if the date cannot be set*/
                      OBJECTID        oiDate,
                      ODATE           theDate);

void  ALMAPI    ODateToYMD(
                      ODATE           theDate,
                      pODATEYMD       ymdDate);

ODATE ALMAPI    ODateFromYMD(
                      pODATEYMD       ymdDate);

long  ALMAPI    ODateGetStyle(            /*returns ODATE_STYLE_<SHORT,LONG,KEEPCURRENT>*/
                      OBJECTID        oiDate);

long  ALMAPI    ODateParseString(         /*returns ODATE_<MONTH,DAY,YEAR>_SUPPLIED */
                      LPSTR           szString);
                              
#ifdef __cplusplus
}     /* extern "C" */
#endif

#endif  /* O_DATE_H */
