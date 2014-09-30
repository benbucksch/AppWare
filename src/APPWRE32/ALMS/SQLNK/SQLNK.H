/*
===============================================================================

SQLNK.h: SequeLink Client definition compatibility
	file of v.2.3 & v.2.4
  Copyright(c) GnOsIs nv, 1993  All rights reserved
  Version 1.00.00

Description
  This file contains definitions & typedefs which allows programs using
  the SequeLink 2.3 interface defines & typedefs (except structs SSPVarDesc
  and SSPDescribeRec).
  It contains a macro to use SQTransliterate instead of SQTransLit.
  Finally contains a macro to use the old MakeLinkParam functions
	with the new one.

Note
  This file should be included, by the Client application or Client
  Environment module, AFTER any Operating System header files or
  standard C header files.

===============================================================================

Compile Options:

macintosh 	- Must be defined for compilation on the Macintosh platform
_OS2 		- Must be defined for compilation on the OS/2 platform
_WINDOWS 	- Must be defined for compilation on the Windows platform
__DLL		- Must be defined to build the code as an OS/2 or Windows DLL along with the _OS2 or _WINDOWS define
_USEDLL		- Must be defined to build a module which Dynamically Links the SQCalls as a DLL
_VAX		- Recommended define for building VAX Calls, may be needed in future releases.
_UNIX		- Recommended define for building Unix Calls, may be needed in future releases.

NOPROTOTYPE	- Must be defined for compilers that do not support prototypes.
*/
#include "SequeLnk.h"

#ifndef _SequeLinkHdr
#define _SequeLinkHdr

#define _SSPHdrVersion "1.00.09"     /* Module Source Code Version Number */

/*****************************************************************************
  SequeLink #defines for pointers on an Intel machine
 *****************************************************************************/

#define  SSPnoErr           SSP_OK

/* setup #defines for an Intel machine */


#if defined(_OS2)  ||  defined(_WINDOWS)
#define  SSPFptr       *
#else
#define  SSPFptr          *
#endif

/*----------------------------------------------------------------------*
  Data Types used by the SequeLink Client
 *----------------------------------------------------------------------*/
typedef unsigned char         SSPByte;
typedef SSPErr                SSPrc;          /* return code from SequeLink */
typedef SSPParse         			SSPOffset;      /* Parser Offset */
typedef void          SSPFptr SSPUserParam;   /* address of a              */
                                              /*   user provided parameter */
typedef unsigned short        SSPBoolean;


/*----------------------------------------------------------------------*
  the various "Kind"s of Variables used by Define
 *----------------------------------------------------------------------*/

#define   kSSPSelectKind    SelectKind    /* Select or output variable */
#define   kSSPBindKind      BindKind    	/* Bind or input variable */
#define   kSSPRef_Select    Ref_Select    /* Referential Select variable */
#define   kSSPRef_Bind      Ref_Bind    	/* Referential Bind variable */

/*----------------------------------------------------------------------*
  the various data "Type"s (of variables) used by Define
 *----------------------------------------------------------------------*/

#define    kSSPBinary       SSPBinary 		/* Unformatted bytes */
#define    kSSPLong         SSPLong				/* Long Integer */
#define    kSSPChar         SSPChar				/* C string */
#define    kSSPFloat        SSPFloat			/* IEEE extended precision float */
#define    kSSPDecimal      SSPDecimal    

#define kSSPNameLength	MaxNameLen

typedef struct {
	SSPData VData;
	SSPString VName;
	short *IData;
	SSPString IName;
	short 			FieldType;
	unsigned long 	RecCount;
	unsigned long 	DataLen;
/* The following two fields are for Fetching Only */
	SSPByte	EndFldChar;
	SSPByte EndColChar;
} SSPCBRecord, *SSPCBRecPtr;
	
	
#define SQTransliterate		SQTransLit
#define SSPMakeConnectParamTCP(host_name, service, host_logon,host_password, connect_param) \
MakeLinkParams(TCP, host_name, (SSPName)NULL, service, (SSPName)NULL, host_logon,host_password, connect_param)
#define SSPMakeConnectParamSPX(host_name, service, host_logon, host_password, connect_param) \
MakeLinkParams(SPX, host_name, (SSPName)NULL, service, (SSPName)NULL, host_logon,host_password, connect_param)
#define SSPMakeConnectParamDECnet(node_name, service, host_logon, host_password, connect_param) \
MakeLinkParams(DECnet, node_name, (SSPName)NULL, service, (SSPName)NULL, host_logon, host_password, connect_param)
#define SSPMakeConnectParamNetBIOS(node_name, client_name, sqlnk_service, adapter_num, host_username, host_password, connect_param) \
MakeLinkParams(NetBIOS, node_name, (SSPName)NULL, sqlnk_service, (SSPName)NULL, client_name, adapter_num, connect_param)
#define SSPMakeConnectParamNetware(node_name, client_name, sqlnk_service, adapter_num, host_username, host_password, connect_param) \
MakeLinkParams(NetBIOS, node_name, (SSPName)NULL, sqlnk_service, (SSPName)NULL, client_name, adapter_num, connect_param)
#define SSPMakeConnectParamAPPC(local_LU, partner_LU, host_TP, host_mode, host_username, host_password, connect_param) \
MakeLinkParams(APPC, local_LU, partner_LU, host_TP, host_mode, host_username, host_password, connect_param)
#define SSPMakeConnectParamASP(appletalk_zone, service, host_logon, host_password, connect_param) \
MakeLinkParams(AppleTalk,	appletalk_zone, (SSPName)NULL, service, (SSPName)NULL, host_logon, host_password, connect_param)
#define SSPMakeConnectParamADSP(appletalk_zone, service, host_logon, host_password, connect_param) \
MakeLinkParams(ADSP, appletalk_zone, (SSPName)NULL, service, (SSPName)NULL, host_logon, host_password, connect_param)


#endif /* #endif for _SequeLinkHdr */
