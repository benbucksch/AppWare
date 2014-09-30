/**************************************************************************\
|* UPSF_ENV header file (for the environment-only routines)               *|
|*  Copyright, Serius Corporation 1989-1992                               *|
|*                                                                        *|
|*  author: Kevin J. Smart                                                *|
|*                                                                        *|
|*  start date:   12/16/92                                                *|
|*  finish date:  12/16/92                                                *|
|*                                                                        *|
\**************************************************************************/

#ifndef __UPSF_ENV_H  /* only include this file once */
#define __UPSF_ENV_H

/* UPSF ENVIRONMENT-ONLY FUNCTION PROTOTYPES ******************************/

#ifdef __cplusplus
extern "C" {
#endif

/* Environment-Only Routines */
long APIENTRY upsfInitExport      (LPCSTR pathName);
long APIENTRY upsfInitImport      (LPCSTR pathName,   BOOL wantLogFile,
                                     LPCSTR logPathName);

long APIENTRY upsfCloseExport     (VOID);
long APIENTRY upsfCloseImport     (VOID);

long APIENTRY upsfClearObjectIDs  (VOID);
long APIENTRY upsfRegisterObjectID(OBJECTID nativeID, LONG upsfID);

long APIENTRY upsfScopeLength     (VOID);

long APIENTRY upsfValidateScope   (LPCSTR szScopeName, LONG lScopeIndex);

#ifdef __cplusplus
}
#endif

/* WINDOWS ENVIRONMENT-ONLY RETURN VALUES *********************************/
#define     AUPSF_SCOPEOK       1L

/* WINDOWS ENVIRONMENT-ONLY VARIABLE TYPES ********************************/

/* Native (Windows) Types */
#define     NATV_OBJECTID       MAKEID('O', 'b', 'I', 'D')

/**************************************************************************/

#endif  /* #ifdef __UPSF_ENV_H */

