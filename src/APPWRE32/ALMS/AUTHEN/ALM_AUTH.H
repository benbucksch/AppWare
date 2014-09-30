/*
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 * 
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked, uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 */

#ifndef __ALM_AUTH_H__
#define __ALM_AUTH_H__
/*
 * ======================================================================
 * Macro Definitions
 * ======================================================================
 */

#define VERSION						DSALMGetMessage(VERSION_STRING);

#define DEFAULT_LOGIN				0
#define NDS_LOGIN					1
#define BINDERY_LOGIN				2

#define SUCCESS						0			/* Successful						*/
#define DS_SUCCESS					1			/* Successful DS authenticate	*/
#define BINDERY_SUCCESS			2			/* Successful BINDERY attach	*/
#define	DS_SUCCESS_PE				3
#define	BINDERY_SUCCESS_PE		4

#define FAILURE						-1

#define MAXPRINTERS					10
#define MAXDRIVES					26			/* Maximum number of drives	*/

#define FILENAME_SCRIPT			1
#define SYSTEM_SCRIPT				1
#define PROFILE_SCRIPT				2
#define USER_SCRIPT					4
#define ALL_SCRIPTS					(SYSTEM_SCRIPT|PROFILE_SCRIPT|USER_SCRIPT)

#define DEFAULT_LOGOUTTREE		FALSE
#define MAX_SERVER_LIST			2048

/*
 * ======================================================================
 *	AUTHENTICATION module prototypes
 * ======================================================================
 */

/* Prototype for main ALM function */
#ifdef __cplusplus

extern "C" {

#endif

void CALLBACK /*!!PORT!! was _far_ _pascal_*/ Authenticate(pAEvtInfo, pAObjMessage theSystem);
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ PromptedAuthenticate(pAEvtInfo, pAObjMessage theSystem);
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ Logout(pAEvtInfo theEvent, pAObjMessage theSystem);
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ MapDrive(pAEvtInfo theEvent, pAObjMessage theSystem);
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ MapDrives(pAEvtInfo theEvent, pAObjMessage theSystem);
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ DeleteMappedDrive(pAEvtInfo theEvent,pAObjMessage theSystem);
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ GetMappedDrives(pAEvtInfo theEvent,pAObjMessage theSystem);
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ MapNext(pAEvtInfo theEvent,pAObjMessage theSystem);
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ ExecuteScript(pAEvtInfo theEvent,pAObjMessage theSystem);
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ Login(pAEvtInfo,pAObjMessage theSystem);
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ PromptedLogin(pAEvtInfo theEvent,pAObjMessage theSystem);
void CALLBACK /*!!PORT!! was _far_ _pascal_*/ ExecuteLoginScripts(pAEvtInfo, pAObjMessage theSystem);


#ifdef __cplusplus

}

#endif

#endif
