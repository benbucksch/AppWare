/*-----------------------------------------------------------------------------
 Program Name: NWXLAT.DLL
 Filename    : O_NWXLAT.H

 (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.

 No part of this file may be duplicated, revised, translated, localized or
 modified in any manner or compiled, linked, uploaded or downloaded to or
 from any computer system without the prior written consent of Novell, Inc.
-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
                               Macro Definitions
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                                   Typedefs
-----------------------------------------------------------------------------*/
typedef enum {DS_CLASS_NAME,DS_ATTR_NAME} DS_NAME_TYPE;

/*-----------------------------------------------------------------------------
                              External Variables
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                              Function Prototypes
-----------------------------------------------------------------------------*/
/* disable C++ name mangling for all exported functions */
#ifdef __cplusplus
extern "C" {
#endif
BOOL FAR PASCAL LoadTranslationTables(void);
void FAR PASCAL UnloadTranslationTables(void);
void FAR PASCAL DSALMGetLocalFromSchema(char *nameIn,char *nameOut,DS_NAME_TYPE nameType);
void FAR PASCAL DSALMGetSchemaFromLocal(char *nameIn,char *nameOut,DS_NAME_TYPE nameType);
#ifdef __cplusplus
}
#endif
