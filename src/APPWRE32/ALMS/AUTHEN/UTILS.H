/*
 * (C) Unpublished Copyright of Novell, Inc.  All Rights Reserved.
 * 
 * No part of this file may be duplicated, revised, translated, localized or
 * modified in any manner or compiled, linked, uploaded or downloaded to or
 * from any computer system without the prior written consent of Novell, Inc.
 */
#ifndef __UTILS_H_
#define __UTILS_H_

#include "parse.h"

#define SQUESTION			'\xBF'
#define SPERIOD				'\xAE'
#define ASTERISK				'\x2A'
#define QUESTION				'\x3F'
#define SASTERISK			'\xAA'

/* Function prototypes */
extern WORD DSConnStatus(NWCONN_HANDLE connID,WORD *connStatus);
extern WORD WildMatch(BYTE *pattern, BYTE *string);
extern WORD IsWild(BYTE *s);
extern BYTE *Gettoken(BYTE *list, BYTE *token);
extern WORD AddToList(AMEMBLOCKID *list, BYTE *row);
extern WORD AddToTable(AOBJECTID oidTable, BYTE *column1);
extern WORD AddToTable(AOBJECTID oidTable, BYTE *column1, BYTE *column2);
extern WORD AddToTable(AOBJECTID oidTable, BYTE *column1, BYTE *column2,
		BYTE *column3);
extern void MyOutputList(AOBJECTID oiObject, AMEMBLOCKID *list,
		WORD Paramnum);
extern int ReadAttribute(char *objectName,
						char *attributeName,
						void **attrBuffer,
						uint32 *syntaxID);
extern void GetDefaultProfile(WORD connectionID, char *user, char *profile);
extern WORD getConnectionToTree(NWCONN_HANDLE *connectionID,
		char *server,char *tree);
extern WORD AttachtoServer(BYTE *ServerName, NWCONN_HANDLE *ConnectionID);
extern void FillPrimaryConnInfo(primaryConnectionStruct *p);
extern int CleanChars(char *data, int length);
extern WORD FirstUnusedDrive(void);
extern WORD 	isNDSConnection(NWCONN_HANDLE connectionID);
extern void UpdateLoginTime(WORD connID, char *name, PSPACE *p);
extern WORD LastUnusedDrive (void);

#endif
