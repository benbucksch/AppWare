/* ***********************************************************************
 *
 * Copyright (c) 1989-1994 Novell, Inc.  All rights reserved.
 *
 * *********************************************************************** */

#ifndef O_NUMBER_H
#define O_NUMBER_H

/* ***********************************************************************
 * ATYPEID for Number Object
 * *********************************************************************** */

#define OTYPE_NUMBER                  ATYPE_MAKEID('N', 'm', 'b', 'r')

/* ***********************************************************************
 * Number Object API Functions
 * *********************************************************************** */

#define ONMBR_MAX_TEXT_LENGTH         35

/* For ONmbrGetType() */
#define ONMBR_INTEGER                 1L
#define ONMBR_REAL                    2L

/* For ONmbrGetStatus() */
#define ONMBR_RANGE_ERR               0x00000001L
#define ONMBR_HAS_CHANGED             0x00000002L

#ifdef __cplusplus
extern "C" {
#endif

/* Borrows formatting information from object **************************** */

short ALMAPI    ONmbrFormatInteger(AOBJECTID oiNumber, LONG lnValue,
                                   LPSTR szBuffer, short cbMax);

short ALMAPI    ONmbrFormatReal(AOBJECTID oiNumber, long double flValue,
                                LPSTR szBuffer, short cbMax);

/* Conversions from other representations ******************************** */

short ALMAPI    ONmbrConvertToString(AOBJECTID oiNumber, LPSTR szBuffer,
                                     short cbMax);

long ALMAPI     ONmbrConvertFromString(AOBJECTID oiNumber, LPSTR szBuffer,
                                       BOOL bChangeType);

long ALMAPI ONmbrParseFormatedNumber(AOBJECTID oiNumber, long double *lpValue, LPCSTR szBuffer);

/* Setting and getting from normal representations *********************** */

long ALMAPI     ONmbrGetInteger(AOBJECTID oiNumber);

void ALMAPI     ONmbrGetReal(AOBJECTID oiNumber, long double far* pValue);

long ALMAPI     ONmbrSetInteger(AOBJECTID oiNumber, long lnValue,
                                BOOL bChangeType);

long ALMAPI     ONmbrSetReal(AOBJECTID oiNumber, long double flValue,
                             BOOL bChangeType);


/* Miscellaneous ********************************************************* */

long ALMAPI     ONmbrGetType(AOBJECTID oiNumber);

long ALMAPI     ONmbrSetType(AOBJECTID oiNumber, LONG theType);

long ALMAPI     ONmbrGetStatus(AOBJECTID oiNumber);


#ifdef __cplusplus
}     /* extern "C" */
#endif

#endif  /* O_NUMBER_H */
