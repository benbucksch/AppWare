/*--------------------------------------------------------------------------*\
 | FILE		: QSQLDATC.H                                                |
 | DESCRIPTION	: Q+E SQL Object data conversion from, to Q+E Include File. |
 | COPYRIGHT	: 1993 Novell, Inc.                                         |
 | EDIT LOG	:                                                           |
 |   v1.00.00	  mw	01-Nov-1993	Initial Edit                        |
\*--------------------------------------------------------------------------*/
#ifndef QSQLDATC_H
#define QSQLDATC_H

#include "qsql_fnc.h"


class QSqlDatC			/* ---- Q+E SQL Object Data Conversion ---- */
{ char			*strPtr;	// string pointer
  char			str[MXSTRSIZE];	// string

  void *Alloc(long);
  /* Allocates spaces. */

  void Free(void);
  /* Frees spaces. */

protected:
  /* ---- Q+E Data to String --------------------------------------------- */
  char *QEDateToDateStr(char *);
  /* Returns pointer to the date string w/ value from the Q+E date string. */

  char *QETimeToTimeStr(char *);
  /* Returns pointer to the time string w/ value from the Q+E time string. */

public:
  QSqlDatC(void);
  /* Constructor. */

  ~QSqlDatC(void);
  /* Destructor. */

  /* ---- Object to Q+E String -------------------------------------------- */
  char *ObjToQEStr(OBJECTID, TYPEID = 0);
  /* Returns pointer to the Q+E data string with value from the object. */

  char *DateToQEStr(OBJECTID);
  /* Returns pointer to the Q+E date string w/ value from the date object. */

  char *NumberToQEStr(OBJECTID);
  /* Returns pointer to the Q+E number str w/ value from the number object. */

  char *TextToQEStr(OBJECTID);
  /* Returns pointer to the Q+E text string w/ value from the text object. */

  char *TimeToQEStr(OBJECTID);
  /* Returns pointer to the Q+E time string w/ value from the time object. */

  /* ---- Q+E Data to Object Value ---------------------------------------- */
  void *QEDataToDateVal(qeHANDLE, long, Col, void *);
  /* Returns pointer to the date value (long) converted from the Q+E data. */

  void *QEDataToRealVal(qeHANDLE, long, Col, void *);
  /* Returns pointer to the real number value (long double) converted from
     the Q+E data.
  */

  void *QEDataToTextVal(qeHANDLE, long, Col, void *);
  /* Returns pointer to the text value (str) converted from the Q+E data. */

  void *QEDataToTimeVal(qeHANDLE, long, Col, void *);
  /* Returns pointer to the time value (long) converted from the Q+E data. */

  void *QEDataToWholeVal(qeHANDLE, long, Col, void *);
  /* Returns pointer to the whole number value (long) converted from
     the Q+E data.
  */
};

#endif /* QSQLDATC_H */
