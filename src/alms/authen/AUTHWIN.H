#ifndef __ALMWIND_H_
#define __ALMWIND_H_

#include <constant.h>
#include <nwdsdefs.h>
#include <nwconnec.h>
#include "parse.h"
#include "alm_auth.h"

typedef struct awndextra
{
	BYTE			UserName[MAX_DN_BYTES];
	BYTE			ServerName[SERVER_NAME_SIZE];
	BYTE			Password[PASSWORD_BUFFER_SIZE];
	WORD			LoginType;
} AWNDEXTRA;

typedef struct lwndextra
{
	BYTE			UserName[MAX_DN_BYTES];
	BYTE			Script[MAX_DN_BYTES];
	BYTE			Profile[MAX_DN_BYTES];
	BYTE			ServerName[SERVER_NAME_SIZE];
	BYTE			Password[PASSWORD_BUFFER_SIZE];
	WORD			LoginType;
} LWNDEXTRA;

typedef struct swndextra
{
	BYTE ScriptFile[MAX_DN_BYTES];
	BYTE ScriptInputScript[MAX_DN_BYTES];
	BYTE ScriptInputProfile[MAX_DN_BYTES];
	BYTE ScriptUserName[MAX_DN_BYTES];
	WORD exitflag;
	WORD   doreturn;
	WORD Scripttype;
	AMEMBLOCKID *Errors;
	NWCONN_HANDLE ScriptconnID;
	PSPACE *pglobals;
} SWNDEXTRA;

typedef struct loutextra
{
	WORD logoutTree;
	WORD winreturn;
	BYTE SList[MAX_SERVER_LIST];
} LOUTEXTRA;

#endif
