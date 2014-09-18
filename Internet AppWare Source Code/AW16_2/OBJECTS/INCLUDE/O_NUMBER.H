/* ***********************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * *********************************************************************** */

#ifndef O_NUMBER_H
#define O_NUMBER_H

/* ***********************************************************************
 * TYPEID for Number Object
 * *********************************************************************** */

#define OTYPE_NUMBER                  MAKETYPEID('N', 'm', 'b', 'r')

/* ***********************************************************************
 * Number Object API Functions
 * *********************************************************************** */

#define ONMBR_MAX_TEXT_LENGTH         35

/* For ONmbrGetType() and ONmbrParseFormatedNumber*/
#define ONMBR_INTEGER                 1L
#define ONMBR_REAL                    2L

/* For ONmbrGetStatus() */
#define ONMBR_RANGE_ERR               0x00000001L
#define ONMBR_HAS_CHANGED             0x00000002L

#ifdef __cplusplus
extern "C" {
#endif

/* Borrows formatting information from object **************************** */

short ALMAPI    ONmbrFormatInteger(OBJECTID oiNumber, LONG lnValue,
                                   LPSTR szBuffer, short cbMax);

short ALMAPI    ONmbrFormatReal(OBJECTID oiNumber, long double flValue,
                                LPSTR szBuffer, short cbMax);

long ALMAPI     ONmbrParseFormatedNumber(OBJECTID oiNumber, long double far *lpValue, LPCSTR szBuffer);



/* Conversions from other representations ******************************** */

short ALMAPI    ONmbrConvertToString(OBJECTID oiNumber, LPSTR szBuffer,
                                     short cbMax);

long ALMAPI     ONmbrConvertFromString(OBJECTID oiNumber, LPSTR szBuffer,
                                       BOOL bChangeType);


/* Setting and getting from normal representations *********************** */

long ALMAPI     ONmbrGetInteger(OBJECTID oiNumber);

void ALMAPI     ONmbrGetReal(OBJECTID oiNumber, long double far* pValue);

long ALMAPI     ONmbrSetInteger(OBJECTID oiNumber, long lnValue,
                                BOOL bChangeType);

long ALMAPI     ONmbrSetReal(OBJECTID oiNumber, long double flValue,
                             BOOL bChangeType);


/* Miscellaneous ********************************************************* */

long ALMAPI     ONmbrGetType(OBJECTID oiNumber);

long ALMAPI     ONmbrSetType(OBJECTID oiNumber, LONG theType);

long ALMAPI     ONmbrGetStatus(OBJECTID oiNumber);


#ifdef __cplusplus
}     /* extern "C" */
#endif

#endif  /* O_NUMBER_H */
