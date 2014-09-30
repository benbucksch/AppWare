/*                                                                        *\
    (c) 1993-1995, Novell, Inc.  All rights reserved.

    THIS  WORK  IS  AN  UNPUBLISHED  WORK  AND  CONTAINS   CONFIDENTIAL,
    PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.  ACCESS TO
    THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE A  NEED  TO
    KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE  OF  THEIR  ASSIGNMENTS
    AND  (II)  ENTITIES  OTHER  THAN  NOVELL  WHO  HAVE   ENTERED   INTO
    APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS WORK MAY  BE  USED,
    PRACTICED,  PERFORMED,  COPIED,  DISTRIBUTED,   REVISED,   MODIFIED,
    TRANSLATED,  ABRIDGED,  CONDENSED,  EXPANDED,  COLLECTED,  COMPILED,
    LINKED, RECAST, TRANSFORMED, OR ADAPTED WITHOUT  THE  PRIOR  WRITTEN
    CONSENT OF NOVELL.  ANY USE OR EXPLOITATION  OF  THIS  WORK  WITHOUT
    AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO  CRIMINAL  AND  CIVIL
    LIABILITY.
\*                                                                        */

/*--------------------------------------------------------------------------*\
 | FILE		: QSQLDATC.CPP                                              |
 | DESCRIPTION	: Q+E SQL Object data conversion from, to Q+E.              |
 | COPYRIGHT	: 1993-1995 Novell, Inc.                                    |
 | EDIT LOG	:                                                           |
 |   v1.00.00	  mw	01-Nov-1993	Initial Edit.                       |
 |		  mw	08-Jun-1995	Win32 Convertions.                  |
\*--------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "qsqldc.h"
#include "safemem.h"
#include "memblock.h"


void *QSqlDatC::Alloc		/* ---- Allocate -------------------------- */
( long		bytes			// number of bytes
)
/* Allocates spaces. */
{
  Free();
  if (bytes <= sizeof(str))
    return str;
  return (void *) strPtr = MBAlloc(bytes);
} // Alloc

void QSqlDatC::Free		/* ---- Free ------------------------------ */
( void
)
/* Frees spaces. */
{
  if (strPtr == NULL)
    return;
  MBFree(strPtr);
  strPtr = NULL;
} // Free


/* ---- Q+E Data to String ------------------------------------------------ */
char *QSqlDatC::QEDateToDateStr	/* ---- Q+E Date To Date String ----------- */
( char		*qeDate			// q+e date-time
)
/* Returns pointer to the date string w/ value from the Q+E date string. */
{
  str[0] = qeDate[5];			// month
  str[1] = qeDate[6];
  str[2] = '/';
  str[3] = qeDate[8];			// day
  str[4] = qeDate[9];
  str[5] = '/';
  str[6] = qeDate[0];			// year
  str[7] = qeDate[1];
  str[8] = qeDate[2];
  str[9] = qeDate[3];
  str[10] = '\0';
  return str;
} // QEDateToDateStr

char *QSqlDatC::QETimeToTimeStr	/* ---- Q+E Time To Time String ----------- */
( char		*qeTime			// q+e date-time
)
/* Returns pointer to the time string w/ value from the Q+E time string. */
{
  str[0] = qeTime[11];			// hour
  str[1] = qeTime[12];
  str[2] = ':';
  str[3] = qeTime[14];			// minute
  str[4] = qeTime[15];
  str[5] = ':';
  str[6] = qeTime[17];			// second
  str[7] = qeTime[18];
  str[9] = '\0';
  return str;
} // QETimeToTimeStr


QSqlDatC::QSqlDatC		/* ---- QSqlDatC -------------------------- */
( void
)
/* The constructor. */
{
  strPtr = NULL;
} // QSqlDatC

QSqlDatC::~QSqlDatC		/* ---- QSqlDatC -------------------------- */
( void
)
/* The destructor. */
{
  Free();
} // ~QSqlDatC

/* ---- Object to Q+E String ---------------------------------------------- */

char *QSqlDatC::ObjToQEStr	/* ---- Object To Q+E String -------------- */
( AOBJECTID	obj,			// object
  ATYPEID	type			// type
)
/* Returns pointer to the Q+E data string with value from the object. */
{
  if (type == NULL)
    type = AObjGetType(obj);
  switch ((long) type)
  { case OTYPE_TEXT:
      return TextToQEStr(obj);
    case OTYPE_NUMBER:
      return NumberToQEStr(obj);
    case OTYPE_DATE:
      return DateToQEStr(obj);
    case OTYPE_TIME:
      return TimeToQEStr(obj);
    default:
      return NULL;
  } // switch
} // ObjToQEStr

char *QSqlDatC::DateToQEStr	/* ---- Date Object To Q+E String --------- */
( AOBJECTID	dateObj			// date object
)
/* Returns pointer to the Q+E date string with value from the date object. */
{ ODATE		dateVal;		// date value
  ODATEYMD	date;			// date structure
  int		sLen;			// string length
  char		s[17];			// string

  dateVal = ODateGetValue(dateObj);
  ODateToYMD(dateVal, &date);

  str[0] = '[';
  str[1] = 'd';
  str[2] = '\'';

  /* ---- date ---- */
  /* ---- year ---- */
  itoa(date.year, s, 10);
  sLen = lstrlen(s);
  str[3] = sLen - 4 >= 0? s[sLen - 4] : '0';
  str[4] = sLen - 3 >= 0? s[sLen - 3] : '0';
  str[5] = sLen - 2 >= 0? s[sLen - 2] : '0';
  str[6] = sLen - 1 >= 0? s[sLen - 1] : '0';
  str[7] = '-';
  /* ---- month --- */
  itoa(date.month, s, 10);
  sLen = lstrlen(s);
  str[8] = sLen - 2 >= 0? s[sLen - 2] : '0';
  str[9] = sLen - 1 >= 0? s[sLen - 1] : '0';
  str[10] = '-';
  /* ---- day ---- */
  itoa(date.day, s, 10);
  sLen = lstrlen(s);
  str[11] = sLen - 2 >= 0? s[sLen - 2] : '0';
  str[12] = sLen - 1 >= 0? s[sLen - 1] : '0';
  str[13] = ' ';

  /* ---- time --- */
  /* ---- hour  ---- */
  str[14] = '0';
  str[15] = '0';
  str[16] = ':';

  /* ---- minute ---- */
  str[17] = '0';
  str[18] = '0';
  str[19] = ':';

  /* ---- second ---- */
  str[20] = '0';
  str[21] = '0';
  str[22] = '.';
  str[23] = '0';
  str[24] = '0';
  str[25] = '0';
  str[26] = '0';
  str[27] = '0';
  str[28] = '0';

  str[29] = '\'';
  str[30] = ']';
  str[31] = '\0';
  return str;
} // DateToQEStr

char *QSqlDatC::NumberToQEStr	/* ---- Number Object To Q+E String ------- */
( AOBJECTID	numberObj		// number object
)
/* Returns pointer to the text string with value from the number object. */
{ long double	realVal;		// real value

  if (ONmbrGetType(numberObj) == ONMBR_INTEGER)
    sprintf(str, "%ld", ONmbrGetInteger(numberObj));
  else
  { ONmbrGetReal(numberObj, &realVal);
    sprintf(str, "%Lf", realVal);
  } // else
  return str;
} // NumberToQEStr

char *QSqlDatC::TextToQEStr	/* ---- Text Object To Q+E String --------- */
( AOBJECTID	textObj			// text object
)
/* Returns pointer to the Q+E text string with value from the text object. */
{ char		*strPtr;		// string pointer
  long		textSize;		// text size
  char		*text;			// text
  long		expSize;		// expantion size
  char		*exp;			// expantion

  textSize = OTextGetLength(textObj) + 1;

  /* ---- allocate spaces ---- */
  strPtr = (char *) Alloc(textSize + 2);// 2 quotes
  text = strPtr + 1;
  OTextGetToBuffer(textObj, (char *) text, textSize);

  /* ---- calculates expantion size ---- */
  expSize = 0;
  while (*text != NULL)
  { if (*text == '\'')
      expSize++;
    text++;
  } // while

  /* ---- no expantion ---- */
  if (expSize == 0)
  { *text++ = '\'';			// open quote
    *text = NULL;

    *strPtr = '\'';			// close quote
    return strPtr;
  } // if

  if (textSize + expSize + 2 > sizeof(str))
  { /* ---- reallocates spaces ---- */
    strPtr = (char *) Alloc(textSize + expSize + 2);
    text = strPtr + 1;
    OTextGetToBuffer(textObj, (char *) text, textSize);
  } // if
  else
    text = strPtr + 1;
  text += textSize;
  exp = text + expSize;

  /* ---- expands ---- */
  text -= 2;
  *exp-- = NULL;
  *exp-- = '\'';
  while (expSize > 0)
  { if (*text == '\'')
    { expSize--;
      *exp-- = '\'';
    } // if
    *exp-- = *text--;
  } // while

  *strPtr = '\'';
  return strPtr;
} // TextToQEStr

char *QSqlDatC::TimeToQEStr	/* ---- Time Object To Q+E String --------- */
( AOBJECTID	timeObj			// time object
)
/* Returns pointer to the Q+E time string with value from the time object. */
{ OTIME		timeVal;		// time value
  OTIMEHMS	time;			// time structure
  int		sLen;			// string length
  char		s[17];			// string

  timeVal = OTimeGetValue(timeObj);
  OTimeToHMS(timeVal, &time);

  str[0] = '[';
  str[1] = 't';
  str[2] = '\'';

  /* ---- date ---- */
  /* ---- year ---- */
  str[3] = '0';
  str[4] = '0';
  str[5] = '0';
  str[6] = '0';
  str[7] = '-';
  /* ---- month --- */
  str[8] = '0';
  str[9] = '0';
  str[10] = '-';
  /* ---- day ---- */
  str[11] = '0';
  str[12] = '0';
  str[13] = ' ';

  /* ---- time ---- */
  /* ---- hour  ---- */
  itoa(time.hour, s, 10);
  sLen = lstrlen(s);
  str[14] = sLen - 2 >= 0? s[sLen - 2] : '0';
  str[15] = sLen - 1 >= 0? s[sLen - 1] : '0';
  str[16] = ':';
  /* ---- minute ---- */
  itoa(time.min, s, 10);
  sLen = lstrlen(s);
  str[17] = sLen - 2 >= 0? s[sLen - 2] : '0';
  str[18] = sLen - 1 >= 0? s[sLen - 1] : '0';
  str[19] = ':';
  /* ---- second ---- */
  itoa(time.sec, s, 10);
  sLen = lstrlen(s);
  str[20] = sLen - 2 >= 0? s[sLen - 2] : '0';
  str[21] = sLen - 1 >= 0? s[sLen - 1] : '0';
  str[22] = '.';
  str[23] = '0';
  str[24] = '0';
  str[25] = '0';
  str[26] = '0';
  str[27] = '0';
  str[28] = '0';

  str[29] = '\'';
  str[30] = ']';
  str[31] = '\0';
  return str;
} // TimeToQEStr

/* ---- Q+E Data to Object Value ------------------------------------------ */

void *QSqlDatC::QEDataToDateVal	/* ---- Q+E Data To Date Value ------------ */
( qeHANDLE	cursor,			// cursor
  long		selColIdx,		// select column index
  Col		selCol,			// select column
  void		*data			// data
)
/* Returns pointer to the date value (long) converted from the Q+E data. */
{ char		*ptr;			// pointer

  switch (selCol.type)
  { case qeCHAR:
    case qeVARCHAR:
    case qeDECIMAL:
    case qeINTEGER:
    case qeSMALLINT:
    case qeFLOAT:
    case qeDOUBLEPRECISION:
      ptr = (char *) QEDataToTextVal(cursor, selColIdx, selCol, data);
      break;
    case qeDATETIME:
      ptr = QEDateToDateStr((char *) data);
      break;
    default:				// in case of Q+E error
      *str = '\0';
      ptr = str;
      break;
  } // switch
  *(long *) str = ODateStringToDate(ptr);
  return str;
} // QEDataToDateVal

void *QSqlDatC::QEDataToRealVal	/* ---- Q+E Data To Real Value ---------- */
( qeHANDLE	cursor,			// cursor
  long		selColIdx,		// select column index
  Col		selCol,			// select column
  void		*data			// data
)
/* Returns real number value (long double) converted from the Q+E data. */
{ long double	longDblVal;		// long double value
  double	dblVal;			// double value

  switch (selCol.type)
  { case qeCHAR:
    case qeVARCHAR:
      longDblVal = *(*qe_CharToDouble)((char *) data, "", &dblVal);
      break;
    case qeDECIMAL:
      longDblVal = *(*qe_DecimalToDouble)((char *) data,
		     (*qe_ColPrecision)(cursor, (short) selColIdx),
		     (*qe_ColScale)(cursor, (short) selColIdx), &dblVal);
      break;
    case qeINTEGER:
      longDblVal = *(long *) data;
      break;
    case qeSMALLINT:
      longDblVal = *(short *) data;
      break;
    case qeFLOAT:
      longDblVal = *(float *) data;
      break;
    case qeDOUBLEPRECISION:
      longDblVal = *(double *) data;
      break;
    case qeDATETIME:
      longDblVal = *(*qe_DateToDouble)((char *) data, &dblVal);
      break;
    default:				// in case of Q+E error
      longDblVal = 0;
      break;
  } // switch
  *(long double *) str = longDblVal;
  return str;
} // QEDataToRealNumber

void *QSqlDatC::QEDataToTextVal	/* ---- Q+E Data To Text Value ------------ */
( qeHANDLE	cursor,			// cursor
  long		selColIdx,		// select column index
  Col		selCol,			// select column
  void		*data			// data
)
/* Returns pointer to the text string with value from the Q+E data. */
{
  switch (selCol.type)
  { case qeCHAR:
    case qeVARCHAR:
      return (char *) data;
    case qeDECIMAL:
      (*qe_DecimalToCharBuf)(str, (char *) data,
			     (*qe_ColPrecision)(cursor, (short) selColIdx),
			     (*qe_ColScale)(cursor, (short) selColIdx), "");
      break;
    case qeINTEGER:
      (*qe_LongToCharBuf)(str, *(long *) data, "");
      break;
    case qeSMALLINT:
      (*qe_IntToCharBuf)(str, *(short *) data, "");
      break;
    case qeFLOAT:
      (*qe_FloatToCharBuf)(str, *(float *) data, "");
      break;
    case qeDOUBLEPRECISION:
      (*qe_DoubleToCharBuf)(str, *(double *) data, "");
      break;
    case qeDATETIME:
      (*qe_DateToCharBuf)(str, (char *) data, "");
      break;
    default:				// in case of Q+E error
      *str = '\0';
      break;
  } // switch
  return str;
} // QEDataToTextVal

void *QSqlDatC::QEDataToTimeVal	/* ---- Q+E Data To Time Value ------------ */
( qeHANDLE	cursor,			// cursor
  long		selColIdx,		// select column index
  Col		selCol,			// select column
  void		*data			// data
)
/* Returns pointer to the time value (long) converted from the Q+E data. */
{ char		*ptr;			// pointer

  switch (selCol.type)
  { case qeCHAR:
    case qeVARCHAR:
    case qeDECIMAL:
    case qeINTEGER:
    case qeSMALLINT:
    case qeFLOAT:
    case qeDOUBLEPRECISION:
      ptr = (char *) QEDataToTextVal(cursor, (short) selColIdx, selCol,
				     data);
      break;
    case qeDATETIME:
      ptr = QETimeToTimeStr((char *) data);
      break;
    default:				// in case of Q+E error
      *str = '\0';
      ptr = str;
      break;
  } // switch
  *(long *) str = OTimeStringToTime(ptr);
  return str;
} // QEDataToTimeVal

void *QSqlDatC::QEDataToWholeVal/* ---- Q+E Data To Whole Value ----------- */
( qeHANDLE	cursor,			// cursor
  long		selColIdx,		// select column index
  Col		selCol,			// select column
  void		*data			// data
)
/* Returns pointer to the whole number value (long) converted from
   the Q+E data.
*/
{ long		wholeVal;		// whole value

  switch (selCol.type)
  { case qeCHAR:
    case qeVARCHAR:
      wholeVal = (*qe_CharToLong)((char *) data, "");
      break;
    case qeDECIMAL:
      wholeVal = (*qe_DecimalToLong)((char *) data,
		   (*qe_ColPrecision)(cursor, (short) selColIdx),
		   (*qe_ColScale)(cursor, (short) selColIdx));
      break;
    case qeINTEGER:
      wholeVal = *(long *) data;
      break;
    case qeSMALLINT:
      wholeVal = *(short *) data;
      break;
    case qeFLOAT:
      wholeVal = (long) *(float *) data;
      break;
    case qeDOUBLEPRECISION:
      wholeVal = (long) *(double *) data;
      break;
    case qeDATETIME:
      wholeVal = (*qe_DateToLong)((char *) data);
      break;
    default:				// in case of Q+E error
      wholeVal = 0;
      break;
  } // switch
  *(long *) str = wholeVal;
  return str;
} // QEDataToWholeVal
