/*
================================================================================
SequeLnk.h: SequeLink Client Published Definitions
  Copyright(c) GnOsIs nv, 1990 - 1994  All rights reserved
  Version 2.04-00

Description
  This file is the published interface file for the SequeLink Client
  part. It contains all the necessary definitions to be used when developing
  SequeLink Client applications as well as Environment Modules.

  This file should be included in the Client application or Environment
  Module.

================================================================================
Compile Options:

macintosh 	- Must be defined for compilation on the Macintosh platform
_WINDOWS		- Must be defined for compilation on the Windows platform
VAX					- Recommended define for building VAX Calls, may be needed in future releases.
UNIX				- Recommended define for building Unix Calls, may be needed in future releases.

NOPROTOTYPE	- Must be defined for compilers that do not support prototypes.
*/


#ifndef __SEQUELINK__								/* To avoid double includes... */
#define __SEQUELINK__

#ifndef Version
#define Version "2.4.0"							/* Module Source Code Version Number */
#endif


/******************************************************************************
  Some Implementation Constraints
 ******************************************************************************/
#define MaxDecimalLen       34
#define MaxNameLen          80
#define MaxStringLen       255
#define MaxnDefVars        256
#define MaxCharLen        4096


/******************************************************************************
  typedefs and the like
 ******************************************************************************/
#ifndef FALSE
#define FALSE               (1 == 0)
#endif

#ifndef TRUE
#define TRUE                (1 == 1)
#endif

#ifndef NULL
#define NULL                0L
#endif

/*----------------------------------------------------------------------*
  SequeLink constants
 *----------------------------------------------------------------------*/
/* The OK return code */
#ifndef SSP_OK
#define SSP_OK              (SSPErr) 0		
#endif
#define SSP_AllConnections 	(SSPCount) 0		
#define SSP_AllVars 				(SSPCount) 0		
#define SSP_ByName	 				(SSPRefNum) 0		
#define SSP_AllRows	 				(SSPCount) 0		

/* TransLit switches */
#define SSP_TrOn	 						1		
#define SSP_TrOff	 						0

/* Nullable constants */
#define SSP_IS_NULLABLE				(char) 0
#define SSP_NOT_NULLABLE			(char) 1
#define SSP_NULL_UNKNOWN			(char) 0xFF


/*----------------------------------------------------------------------*
  Data Types used by the SequeLink Client
 *----------------------------------------------------------------------*/
typedef short           SSPRefNum;      /* Context reference numbers */
typedef char *          SSPName;        /* C string of max 80 chars long */
typedef char *          SSPString;      /* C string of max 255 chars long */
typedef char *          SSPStmt;        /* C string of max 4096 chars long */
typedef char *          SSPData;        /* Unlimited byte buffer */
typedef long            SSPErr;         /* SequeLink status return code v2.0 */
typedef long            SSPSrvRtn;      /* server status return code v2.0 */
typedef short           SSPParse;       /* Parser Offset */
typedef unsigned long		SSPSize;        /* data size */
typedef short           SSPIndicator;   /* data presence/NULL value indicator */
typedef long  					SSPCount;       /* number of rows affected/fetched */
typedef void *          SSPHook;        /* Generic user hook */
typedef short						SSPEnum;				/* SequeLink enumeration */
typedef SSPEnum					SSPType;				/* SequeLink type (SSPLong, ...)*/
typedef SSPEnum					SSPKind;				/* SequeLink kind (bind, select, ...) */ 

typedef struct BOB                      /* Binary data type */
  {
	  SSPSize len;
		SSPHook dta;
	} BinaryDta;



/*Typedefs for Transliteration Procedure, supplied by the user */
/*only applicable for UNIX & VAX, but doesn't hurt other platforms*/
typedef enum Translates
  {
    None, 
		TrInit, 
		TrDone, 
		TrClientToServer, 
		TrServerToCLient
  } TrOpCode;

#ifndef NOPROTOTYPES
typedef void (*SSPTransLit)(TrOpCode Ops, 
														char *from, 
														short flen,
                            char *to, 
														short *tlen, 
														SSPErr *status);
#else
typedef void (*SSPTransLit)();
#endif

typedef struct TransLitResTab
  {
    char TranslitName[MaxNameLen + 1];
    SSPTransLit TranslitRes;
  } SSPTransLitTab;


/*----------------------------------------------------------------------*
	The different network protocols supported
 *----------------------------------------------------------------------*/
typedef enum SupportedNets
  {
    AppleTalk, DECnet, TCP, ADSP, APPC, NetBIOS, SPX, CPIC, PCS
  } SSPNetDriver;


/*----------------------------------------------------------------------*
  Different Kinds of Variables used by Define
 *----------------------------------------------------------------------*/
#define   SelectKind    1    /* Select or output variable */
#define   BindKind      2    /* Bind or input variable */
#define   Ref_Select    3    /* Referential Select variable */
#define   Ref_Bind      4    /* Referential Bind variable */

/*----------------------------------------------------------------------*
  Different Data Types of Variables used by Define
 *----------------------------------------------------------------------*/
#define    SSPBinary		1    /* Unformatted bytes */
#define    SSPLong			3    /* Long Integer */
#define    SSPChar			5    /* C string */
#define    SSPFloat			7    /* IEEE extended precision float */
#define    SSPDecimal		9    /* Binary Coded Decimal */


/*----------------------------------------------------------------------*
  Element of a List of Variable Definitions used in SQnDefine, GetVarDef
	and PutVarDta.

  REMARK: The Client programmer has to define himself the size
    of his array he wants to use, as this structure is purely for input
		when used in the SQnDefine.
  Exp:
    SSPDefList myVars[45];
 *----------------------------------------------------------------------*/
typedef struct DefArrayElem
  {
    SSPKind Kind;
    SSPType Type;
    SSPSize Size;
    SSPData Buffer;
    SSPName VarName;
    SSPIndicator *Indicator;
    SSPName IndName;
    SSPRefNum RelCursor;
  } SSPDefList;

/*----------------------------------------------------------------------*
  Element of a List of Variable Definitions returned by Describe
  REMARK: The Client programmer has to define himself the size
    of his array he wants to use. If a NULL pointer is passed, an
		appropriate amount of memory will be allocated, as given in the Count
		parameter. The user then has to deallocate before leaving...
  Exp:
    SSPDescList myVars[45];
 *----------------------------------------------------------------------*/
typedef struct DescArrayElem
  {
    long dbType;
    SSPType SqlnkType;
    SSPSize Size;
		char scale;
		char nullable;
    char VarName[MaxNameLen + 1];
  } SSPDescList;



#ifdef __cplusplus
extern "C" {
#endif

/**********************************************************************
  Synchronous Call Interface
**********************************************************************/
/*----------------------------------------------------------------------*
  SQConnect is a function returning a value of type SSPErr.
  The parameters are :
    SSPName NetWorkLink         I: Type of Network Link
    SSPString LinkParam         I: Connection Parameters
    SSPName ConName             I: Logical Connection Name
    SSPRefNum *SerRefNum        O: Connection RefNum
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQConnect(SSPName 		NetWorkLink,  	/*  I: Type of Network Link */
                 SSPString 	LinkParam,    	/*  I: Connection Parameters */
                 SSPName 		ConName,      	/*  I: Logical Connection Name */
                 SSPRefNum 	*SerRefNum);  	/*  O: Connection RefNum */
#else
SSPErr SQConnect();
#endif


/*----------------------------------------------------------------------*
  SQLogon is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum SerRefNum         I: Connection RefNum
    SSPString LogParam1         I: Logon Parameter 1
    SSPString LogParam2         I: Logon Parameter 2
    SSPName LogName             I: Logical Session Name
    SSPRefNum *LogRefNum        O: Logon/Session RefNum
    SSPSrvRtn *SerRetCode       O: Server Return Code
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQLogon(SSPRefNum SerRefNum,         /*  I: Connection RefNum */
               SSPString LogParam1,         /*  I: Logon Parameter 1 */
               SSPString LogParam2,         /*  I: Logon Parameter 2 */
               SSPName LogName,             /*  I: Logical Session Name */
               SSPRefNum *LogRefNum,        /*  O: Logon/Session RefNum */
               SSPSrvRtn *SerRetCode);      /*  O: Server Return Code */
#else
SSPErr SQLogon();
#endif


/*----------------------------------------------------------------------*
  SQOpen is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum LogRefNum         I: Logon/Session RefNum
    SSPName CursName            I: Logical Cursor Name
    SSPRefNum *CursRefNum       O: Cursor RefNum
    SSPSrvRtn *SerRetCode       O: Server Return Code
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQOpen(SSPRefNum LogRefNum,          /*  I: Logon/Session RefNum */
              SSPName CursName,             /*  I: Logical Cursor Name */
              SSPRefNum *CursRefNum,        /*  O: Cursor RefNum */
              SSPSrvRtn *SerRetCode);       /*  O: Server Return Code */
#else
SSPErr SQOpen();
#endif


/*----------------------------------------------------------------------*
  SQAssociate is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum CursRefNum        I: Cursor RefNum
    SSPStmt SqlStatement        I: SQL Statement
    SSPParse *ParseOffset       O: Parser return
    SSPSrvRtn *SerRetCode       O: Server Return Code
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQAssociate(SSPRefNum CursRefNum,    /*  I: Cursor RefNum */
                   SSPStmt Statement,       /*  I: Statement */
                   SSPParse *ParseOffset,   /*  O: Parser return */
                   SSPSrvRtn *SerRetCode);  /*  O: Server Return Code */
#else
SSPErr SQAssociate();
#endif


/*----------------------------------------------------------------------*
  SQnDescribe is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum CursRefNum           I: Cursor RefNum
    SSPKind Kind,                  I: 1,3 = Select; 2,4 = Bind (reduced to 1,2)
    SSPCount *Count,              IO: # of variables requested
    SSPDescList (*(*DescList))[], IO: Address of Data Buffer
    SSPSrvRtn *SerRetCode          O: Server Return Code

  If DescLst is supplied as a NULL value - ths to be allocated by the core
  the Count is only an output value indicating the number of vars.
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQnDescribe(SSPRefNum CursRefNum,        /*  I: Cursor RefNum */
                   SSPKind Kind,                /*  I: 1 = Select; 2 = Bind */
                   SSPCount *Count,             /* IO: # of variables requested */
                   SSPDescList (*(*DescList))[],/* IO: Address of Data Buffer */
                   SSPSrvRtn *SerRetCode);      /*  O: Server Return Code */
#else
SSPErr SQnDescribe();
#endif


/*----------------------------------------------------------------------*
  SQDefine is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum CursRefNum        I: Cursor RefNum
    SSPKind Kind                I: 1, 3 = Select; 2, 4 = Bind
    SSPType Type                I: Data Type
    SSPSize Size                I: Size in bytes
    SSPData Buffer              I: Address of Data Buffer
    SSPName VarName             I: Variable Name
    SSPIndicator *Indicator     I: Address of Indicator
    SSPName IndicatorName       I: Indicator Name
    SSPRefNum RelCursor         I: Related Cursor RefNum
    SSPSrvRtn *SerRetCode       O: Server Return Code
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQDefine(SSPRefNum CursRefNum,       /*  I: Cursor RefNum */
                SSPKind Kind,               /*  I: 1,3 = Select; 2,4 = Bind */
                SSPType Type,               /*  I: Data Type */
                SSPSize Size,               /*  I: Size in bytes */
                SSPData Buffer,             /*  I: Address of Data Buffer */
                SSPName VarName,            /*  I: Variable Name */
                SSPIndicator *Indicator,    /*  I: Address of Indicator */
                SSPName IndName,            /*  I: Indicator Name */
                SSPRefNum RelCursor,        /*  I: Related Cursor RefNum */
                SSPSrvRtn *SerRetCode);     /*  O: Server Return Code */
#else
SSPErr SQDefine();
#endif


/*----------------------------------------------------------------------*
  SQnDefine is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum CursRefNum        I: Cursor RefNum
    SSPCount nrVar              I: Number of variables
    SSPDefList Vars[]           I: repeated variable struct
    SSPSrvRtn *SerRetCode       O: Server Return Code
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQnDefine(SSPRefNum CursRefNum,      /*  I: Cursor RefNum */
                 SSPCount nrVar,            /*  I: Number of variables */
                 SSPDefList Vars[],         /*  I: repeated variable struct */
                 SSPSrvRtn *SerRetCode);    /*  O: Server Return Code */
#else
SSPErr SQnDefine();
#endif


/*----------------------------------------------------------------------*
  SQBind is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum CursRefNum        I: Cursor RefNum
    SSPSrvRtn *SerRetCode       O: Server Return Code
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQBind(SSPRefNum CursRefNum,         /*  I: Cursor RefNum */
              SSPSrvRtn *SerRetCode);       /*  O: Server Return Code */
#else
SSPErr SQBind();
#endif


/*----------------------------------------------------------------------*
  SQExec is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum CursRefNum        I: Cursor RefNum
    SSPCount *RowCount          O: Nr of Rows affected
    SSPSrvRtn *SerRetCode       O: Server Return Code
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQExec(SSPRefNum CursRefNum,         /*  I: Cursor RefNum */
              SSPCount *RowCount,           /*  O: Nr of Rows affected */
              SSPSrvRtn *SerRetCode);       /*  O: Server Return Code */
#else
SSPErr SQExec();
#endif


/*----------------------------------------------------------------------*
  SQImmediate is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum LogRefNum         I: Logon/Session RefNum
    SSPStmt Statement           I: Statement
    SSPCount *RowCount          O: Nr of Rows affected
    SSPParse *ParseOffset       O: Parser return
    SSPSrvRtn *SerRetCode       O: Server Return Code
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQImmediate(SSPRefNum LogRefNum,     /*  I: Logon/Session RefNum */
                   SSPStmt Statement,       /*  I: Statement */
                   SSPCount *RowCount,      /*  O: Nr of Rows affected */
                   SSPParse *ParseOffset,   /*  O: Parser return */
                   SSPSrvRtn *SerRetCode);  /*  O: Server Return Code */
#else
SSPErr SQImmediate();
#endif


/*----------------------------------------------------------------------*
  SQFetch is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum CursRefNum        I: Cursor RefNum
    SSPCount *RowCount         IO: Nr of Rows affected
    SSPSrvRtn *SerRetCode       O: Server Return Code
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQFetch(SSPRefNum CursRefNum,        /*  I: Cursor RefNum */
               SSPCount *RowCount,          /* IO: Nr of Rows affected */
               SSPSrvRtn *SerRetCode);      /*  O: Server Return Code */
#else
SSPErr SQFetch();
#endif


/*----------------------------------------------------------------------*
  SQCommit is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum LogRefNum         I: Logon/Session RefNum
    SSPSrvRtn *SerRetCode       O: Server Return Code
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQCommit(SSPRefNum LogRefNum,        /*  I: Logon/Session RefNum */
                SSPSrvRtn *SerRetCode);     /*  O: Server Return Code */
#else
SSPErr SQCommit();
#endif


/*----------------------------------------------------------------------*
  SQRollback is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum LogRefNum         I: Logon/Session RefNum
    SSPSrvRtn *SerRetCode       O: Server Return Code
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQRollback(SSPRefNum LogRefNum,      /*  I: Logon/Session RefNum */
                  SSPSrvRtn *SerRetCode);   /*  O: Server Return Code */
#else
SSPErr SQRollback();
#endif


/*----------------------------------------------------------------------*
  SQCancel is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum CursRefNum        I: Cursor RefNum
    SSPSrvRtn *SerRetCode       O: Server Return Code
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQCancel(SSPRefNum CursRefNum,       /*  I: Cursor RefNum */
                SSPSrvRtn *SerRetCode);     /*  O: Server Return Code */
#else
SSPErr SQCancel();
#endif


/*----------------------------------------------------------------------*
  SQErrorMessage is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum SerRefNum         I: Connection RefNum
    SSPSrvRtn MsgNumber         I: Server Error Code
    SSPString MsgBuffer         O: Error Message text
    SSPSrvRtn *SerRetCode       O: Server Return Code
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQErrorMessage(SSPRefNum SerRefNum,    /*  I: Connection RefNum */
                      SSPSrvRtn MsgNumber,    /*  I: Error Code */
                      SSPString MessBuf,      /*  O: Error Message text */
                      SSPSrvRtn *SerRetCode); /*  O: Server Return Code */
#else
SSPErr SQErrorMessage();
#endif



/*----------------------------------------------------------------------*
  SQTransLit is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum SerRefNum         I: Connection RefNum
    SSPName 	TransLitResource	I: Name of transliteration resouce/DLL/function
    short			Action            I: Indication to switch transliteration on 
																	  (SSP_TrOn) or off (SSP_TrOff)
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQTransLit(SSPRefNum SerRefNum,      	/*  I: Connection RefNum */
                  SSPName		TransLitResource,	/*  I: Name of transliteration resouce/DLL/function */
                  short			Action);          /*  I: Indication to switch transliteration on or off */
#else
SSPErr SQTransLit();
#endif


/*----------------------------------------------------------------------*
  SQClose is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum CursRefNum        I: Cursor RefNum
    SSPSrvRtn *SerRetCode       O: Server Return Code
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQClose(SSPRefNum CursRefNum,        /*  I: Cursor RefNum */
               SSPSrvRtn *SerRetCode);      /*  O: Server Return Code */
#else
SSPErr SQClose();
#endif


/*----------------------------------------------------------------------*
  SQLogoff is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum LogRefNum         I: Logon/Session RefNum
    SSPSrvRtn *SerRetCode       O: Server Return Code
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQLogoff(SSPRefNum LogRefNum,        /*  I: Logon/Session RefNum */
                SSPSrvRtn *SerRetCode);     /*  O: Server Return Code */
#else
SSPErr SQLogoff();
#endif


/*----------------------------------------------------------------------*
  SQDisconnect is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum SerRefNum         I: Connection RefNum  or 0
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr SQDisconnect(SSPRefNum SerRefNum);   /*  I: Connection RefNum or 0 */
#else
SSPErr SQDisconnect();
#endif


/******************************************************************************
  Some Support Functions
 ******************************************************************************/
/*----------------------------------------------------------------------*
  MakeLinkParams is a function returning a SSPString.
  The parameters are:
    SSPNetDriver network        I: Type of Network Protocol
    SSPName host_node           I: host/node/zone name
    SSPName partner_lu_name     I: partner LU Name (APPC)
    SSPName service             I: SequeLink Service Name
    SSPName APPC_mode           I: APPC mode (APPC)
    SSPName host_logon          I: host Logon
    SSPName host_password       I: host password
		SSPString ConnectString     0: resulting connect parm string
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
void MakeLinkParams(SSPNetDriver network,				/* I: Type of Network Protocol */
                    SSPName host_node,					/* I: host/node/zone name */
                    SSPName partner_lu_name,		/* I: partner LU Name (APPC) */ 
                    SSPName service,						/* I: SequeLink Service Name */
                    SSPName APPC_mode,					/* I: APPC mode (APPC) */
                    SSPName host_logon,					/* I: host Logon */
                    SSPName host_password,			/* I: host password */
										SSPString connectString);		/* 0: resulting connect parm string */
#else
void MakeLinkParams();
#endif


/*------------------------------------------------------------*
	ConcatLinkParams concatenates the array if strings in Params in the
	format of a SequeLink connect string.
	The parameters are :
		SSPName 		Params[]				I: Array of pointers to strings  
											   					 that have to be concatenated
		SSPCount		nrParams 				I: Number of parameters
		SSPString		connectString		0: Connect string 
 *------------------------------------------------------------*/
#ifndef NOPROTOTYPES
void ConcatLinkParams(SSPName Params[], 				/* I: Array of pointers to strings  */
											   					 							/* that have to be concatenated */
											SSPCount nrParams,				/* I: Number of parameters */
											SSPString connectString);	/* 0: Connect string */
#else
void ConcatLinkParams();
#endif


/*----------------------------------------------------------------------*
  GetVarDef is a function returning a SSPErr.
	If Var = SSP_ByName, the Mask->VarName will be used search the variable.
  The parameters are:
    SSPRefNum		StmtCurs         I: Statement Cursor Reference Number
    SSPDefList	*Mask            O: Variable Definition
    SSPRefNum		Var              I: Variable Reference Number
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr GetVarDef(SSPRefNum StmtCurs,					/* I: Statement Cursor Reference Number */
									SSPDefList *Mask,						/* O: Variable Definition */
									SSPRefNum Var);							/* I: Variable Reference Number */
#else
SSPErr GetVarDef();
#endif


/*----------------------------------------------------------------------*
  PutVarDta is a function returning a SSPErr.
	If Var = SSP_ByName, the Mask->VarName will be used search the variable.
  The parameters are:
    SSPRefNum		StmtCurs         I: Statement Cursor Reference Number
    SSPDefList	*Mask            I: Variable Definition
    SSPRefNum 	Var              I: Variable Reference Number

  Not all fields from the Mask will be copied into the Bind variable.
 *----------------------------------------------------------------------*/
#ifndef NOPROTOTYPES
SSPErr PutVarDta(SSPRefNum StmtCurs,				/* I: Statement Cursor Reference Number */
									SSPDefList *Mask,					/* I: Variable Definition */
									SSPRefNum Var);						/* I: Variable Reference Number */
#else
SSPErr PutVarDta();
#endif



#ifdef __cplusplus
}
#endif

#endif		/* __SEQUELINK__ */

