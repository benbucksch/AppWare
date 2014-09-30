/*
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 * 
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked, uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 */

#ifndef __ERRORS_H__
#define __ERRORS_H__

/*-----------------------------------------------------------------------------
                               Macro Definitions
-----------------------------------------------------------------------------*/
#define  NO_ICON  -1

/*-----------------------------------------------------------------------------
                                   Typedefs
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                              External Variables
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                              Function Prototypes
-----------------------------------------------------------------------------*/
/* non-exported functions */
#ifdef __cplusplus
extern "C" {
#endif
extern int  DisplayIDMessage(unsigned msgno,int iconType);
extern int  DisplayTextMessage(char *message,int iconType);
extern char *DSALMGetMessage(unsigned msgno,char *messageStr, int size);
#ifdef __cplusplus
}
#endif

#endif

