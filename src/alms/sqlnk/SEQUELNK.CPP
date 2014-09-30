/* Edit Log:
	- Rename 'sequelnk.c' to 'sequelnk.cpp'. 
	- Add '#include "sqlnklib.h"'.
	- Replace 'SQLDispatch' with '(*sql_dispatch)'.
*/
#include "sqlnklib.h"			



/*
=================================================================================
SequeLnk.c : SequeLink Client implementation for Mac & Win3
	Copyright(c) GnOsIs nv, 1991-1994 All rights reserved.
	Version 2.04-00

Description
  This file is the implements published the SequeLink Client
  part on Macintosh and Window 3.x platforms. It implements all 
	the necessary calls to be used when developing SequeLink
  Client applications as well as Environment Modules except the MakeLinkParams
	& ConcatLinkParams call.
	
  This file should be linked in the Client application
  or Environment Module.
	
================================================================================
*/


#ifdef _WINDOWS
#include "windows.h"
#endif

#ifdef macintosh		/* MAC INCLUDES */
#include <Memory.h>
#include <Files.h>
#include <Devices.h>
#include <String.h>
#include <Types.h>
#endif							/* MAC INCLUDES */

#include <StdLib.h>
#include "SequeLnk.h"


/******************************************************************************
  Some Mac types, used in the SequeLink data stuctures
 ******************************************************************************/
#ifdef _WINDOWS			/* WIN DEFINITIONS */
typedef short OSErr;
typedef char   * Ptr;
typedef short ( *ProcPtr)();
typedef unsigned char  * StringPtr; //,  ** StringHandle

typedef struct QElem
{
	struct QElem	*qLink;
	short					dummy1;
	char					dummy2;
} QElem, *QElemPtr;

#define ParamBlockHeader \
	QElemPtr			dummy3;    			\
	short					dummy4;					\
	short					dummy5;					\
	Ptr						dummy6;    			\
	ProcPtr				dummy7;					\
	OSErr					ioResult;				/*result code*/\
	StringPtr			dummy10;				\
	short					dummy11;

typedef struct
{
	ParamBlockHeader
} CntrlParam;
#endif							/* WIN DEFINITIONS */


/******************************************************************************
	SequeLink types and structures
 ******************************************************************************/
/*==================== Global constants ====================*/
#define Copyright_Notice		    "SequeLink C Interface Implementation" \
																"(c)GnOsIs nv 1990-1993. All rights reserved\n" \
																"For further information contact :\n"); \
																"                GnOsIs nv                \n" \
																"             Sorghvliet 17               \n" \
																"       B-2860 Sint Katelijne Waver       \n" \
																"                 BELGIUM                 \n" \
																"tfn: +32 15 299 711  fax: +32 15 200 663 \n"

#define CURSOR_v24					0x0300

/*---- Common SequeLink Param block header definition -----*/
#define SLParamBlockHeader \
ParamBlockHeader\
	short			ioRefNum;		/*refNum for I/O operation*/\
	short			csCode;			/*the SequeLink routine to be execued*/


/*==================== SSP Client Error codes ====================*/
#define nDefMemError        -2516 	/*Memory Error in SQnDefine*/
#define cntnDescribeErr     -2714 	/*param not correct in SQnDescribe*/
#define nDefNullVarsErr			-2515		/*Vars var == NULL*/

/*========== cntrlRecord =============*/
typedef struct
{
	SLParamBlockHeader
	short			Action;
	Ptr				ParamList;
} cntrlRecord;

/*========== connectRecord =============*/
typedef struct
{
	SLParamBlockHeader
	char			*LinkName;
	char 			*LinkParam;
	short  		SerRefNum;
	Ptr       ConName;
} connectRecord;

/*=========== logonRecord ==============*/
typedef struct
{
	SLParamBlockHeader
	short 		SerRefNum;
	char 			*UserId;
	char 			*PassWord;
	short			LogRefNum;
	short			SerRetCode;
	Ptr       LogName;
} logonRecord;

/*============== openRecord =============*/
typedef struct
{
	SLParamBlockHeader
	short			LogRefNum;
	short			CursRefNum;
	short			SerRetCode;
	Ptr       CursName;
	short			flgs;
} openRecord;

/*========== associateRecord ==============*/
typedef struct
{
	SLParamBlockHeader
	short			CursRefNum;
	char 			*SqlStatement;
	short			ParseOffset;
	short			SerRetCode;
} associateRecord;

/*========== describeRecord ================*/
typedef struct
{
	SLParamBlockHeader
	short				CursRefNum;
	short				Kind;
	short				ElemSize;
	short				Index;
	short				Count;
	short				Usrflags;
	short				SerRetCode;
	SSPDescList (*nDescArr)[];
} describeRecord;

/*============ defineRecord ==================*/
typedef struct
{
	SLParamBlockHeader
	short			CursRefNum;
	short			reserved;
	short			Type;
	short			Kind;
	short			Size;
	ProcPtr   CallBack;
	char 			*buffer;
	char 			*FieldName;
	short  		*Indicator;
	char 			*IndName;
	short			SerRetCode;
	short			FetchCRN;
} defineRecord;

/*============ ndefineRecord =================*/
typedef struct
{
	SLParamBlockHeader
	short 		CursRefNum;
	short    	Quantity;
	short 		*FieldNum; 
	short 		*Type;
	short 		*Kind;
	short 		*Size;
	ProcPtr 	CallBackF;
	ProcPtr 	CallBackB;
	char 			**buffer;
	char 			**FieldName;
	short  		**Indicator;
	char 			**IndName;
	short 		SerRetCode;
	short  		*FetchCRN;
} ndefineRecord;

/*=============== bindRecord ===================*/
typedef struct
{
	SLParamBlockHeader
	short			CursRefNum;
	long      cbParam;
} bindRecord;

/*============== executeRecord =================*/
typedef struct
{
	SLParamBlockHeader
	short			CursRefNum;
	short			RowCount;
	short			SerRetCode;
	short			flgs;
} executeRecord;

/*============= immediateRecord ==================*/
typedef struct
{
	SLParamBlockHeader
	short			LogRefNum;
	char *		SqlStatement;
	short			ParseOffset;
	long			RowCount;
	short			SerRetCode;
} immediateRecord;

/*=============== fetchRecord ===================*/
typedef struct
{
	SLParamBlockHeader
	short			CursRefNum;
	short			RowCount;
	short			SerRetCode;
	long      cbParam;
} fetchRecord;

/*=============== cancelRecord ==================*/
typedef struct
{
	SLParamBlockHeader
	short			CursRefNum;
} cancelRecord;

/*=============== commitRecord ===================*/
typedef struct
{
	SLParamBlockHeader
	short			LogRefNum;
	short 		SerRetCode;
} commitRecord;
		
/*=============== rollbackRecord ==================*/
typedef struct
{
	SLParamBlockHeader
	short			LogRefNum;
	short 		SerRetCode;
} rollbackRecord;

/*================ errmesRecord ===================*/
typedef struct
{
	SLParamBlockHeader
	short			SerRefNum;
	short			SerRetCode;
	char 			*MessBuf;
} errmesRecord;

/*================= closeRecord ====================*/
typedef struct
{
	SLParamBlockHeader
	short     CursRefNum;
	short 		SerRetCode;
} closeRecord;

/*================= logoffRecord ====================*/
typedef struct
{
	SLParamBlockHeader
	short			LogRefNum;
	short 		SerRetCode;
} logoffRecord;

/*=============== disconnectRecord ===================*/
typedef struct
{
	SLParamBlockHeader
	short 		SerRefNum;
} disconnectRecord;


/*================ setVarDefRecord ====================*/
typedef struct
{
	SLParamBlockHeader
	short     CursRefNum;
	short     FieldNum; 
	short     Type;
	short     Kind;
	short     Size;
	char      *Buffer;
	char      *FieldName;
	short     *Indicator;
	char      *IndName;
} setVarDefRecord;

/*================ getVarDefRecord ====================*/
typedef struct
{
	SLParamBlockHeader
	short     CursRefNum;
	short     FieldNum;
	short     Type;
	short     Kind;
	short     Size;
	char      *Buffer;
	char      *FieldName;
	short     *Indicator;
	char      *IndName;
} getVarDefRecord;
	
	
/*================ csCode(SPFunctions) =================*/
enum sspFunctions
{
	SQLLogon = 10,
	SQLOpen,
	SQLAssociate,
	SQLDefine,
	SQLExec, 
	SQLFetch,
	SQLBreak,
	SQLCancel,
	SQLCommit,
	SQLRollback,
	SQLErrorMessage,
	SQLClose,
	SQLLogoff,
	SQLBind,
	SQLnDefine = 25,
	SQLImmediate,
	SQLnDescribe,
	SQLConnect = 31,
	SQLDisconnect,
	SQLSetVarDef = 40,
	SQLGetVarDef,
	SQLControl=107
};


/*----------------------------------------------------------------------*
  Prototypes
 *----------------------------------------------------------------------*/
#ifdef _WINDOWS
short WINAPI SQLDispatch(CntrlParam*);
#endif 
#ifdef macintosh					/* MAC PRIVATE FUNCTIONS */	
static SSPErr OpenSequeLinkDriver(short	*SSPdrvrRefNum);
static short SQLDispatch(CntrlParam* generalCtl);


/*----------------------------------------------------------------------*
  OpenSequeLinkDriver is a function returning a value of type SSPErr.
	It tries to open the SequeLink driver.
  The parameters are:
    short		*SSPdrvrRefNum					O: Driver reference number
 *----------------------------------------------------------------------*/
static SSPErr OpenSequeLinkDriver(short	*SSPdrvrRefNum)
{
	return((SSPErr)OpenDriver ("\p.SSP", SSPdrvrRefNum));			
}


/*----------------------------------------------------------------------*
  SQLDispatch is a function returning a value of type short representing
	the (SequeLink) error. This routine is added to provide compatibility 
	with Win3 implementation.
	The parameters are:
    CntrlParam		*generalCtl					I: Parameter block.
 *----------------------------------------------------------------------*/
short SQLDispatch(CntrlParam* generalCtl)
{
	SSPErr	DrvErr = 0;
	short	SSPdrvrRefNum = 0;
	
	DrvErr = OpenSequeLinkDriver(&SSPdrvrRefNum);

	if(DrvErr != SSP_OK)
		generalCtl->ioResult = (OSErr) DrvErr;
	else
		{
			generalCtl->ioCRefNum = SSPdrvrRefNum;
			generalCtl->ioVRefNum = 0;
			generalCtl->ioCompletion = NULL;
		
			PBControl((ParmBlkPtr)generalCtl, false);
			DrvErr = generalCtl->ioResult;
		}
		
	return(DrvErr);		
}
#endif							/* MAC PRIVATE FUNCTIONS */


/*----------------------------------------------------------------------*
  GetVarDef is a function returning a value of type SSPErr.
  The parameters are:
    SSPRefNum		StmtCurs        	I: Statement Cursor Reference Number
    SSPDefList	*Mask          		O: Variable Definition
    SSPRefNum 	Var								I: Variable Reference Number (Optional)
 *----------------------------------------------------------------------*/
SSPErr GetVarDef(SSPRefNum StmtCurs, 
									SSPDefList *Mask, 
									SSPRefNum Var)
{
	getVarDefRecord	getVarParam;
	
	getVarParam.csCode = SQLGetVarDef;
	getVarParam.CursRefNum = StmtCurs;
	getVarParam.FieldNum = Var;
	getVarParam.Kind = Mask->Kind;
	getVarParam.Type = Mask->Type;
	getVarParam.FieldName = Mask->VarName;
		
	(*sql_dispatch)((CntrlParam*)&getVarParam);

	Mask->Type = (SSPType) getVarParam.Type;
	Mask->Size = getVarParam.Size;
	Mask->Buffer = getVarParam.Buffer;
	Mask->VarName = getVarParam.FieldName;
	Mask->Indicator = getVarParam.Indicator;
	Mask->IndName = getVarParam.IndName;

	return((SSPErr)getVarParam.ioResult);
}


/*----------------------------------------------------------------------*
  PutVarDta is a function returning a value of type SSPErr.
  The parameters are:
    SSPRefNum		StmtCurs         I: Statement Cursor Reference Number
    SSPDefList	*Mask            I: Variable Definition
    SSPRefNum		Var              I: Variable Reference Number (Optional)
		
	Not all fields from the Mask will be copied into the Bind variable.
 *----------------------------------------------------------------------*/
SSPErr PutVarDta(SSPRefNum StmtCurs, 
									SSPDefList *Mask, 
									SSPRefNum Var)
{
	setVarDefRecord setVarParam;
	
	setVarParam.csCode = SQLSetVarDef;
	setVarParam.CursRefNum = StmtCurs;
	setVarParam.FieldNum = Var;
	setVarParam.Buffer = Mask->Buffer;
	setVarParam.Indicator = Mask->Indicator;
	setVarParam.Kind = Mask->Kind;
	setVarParam.Type = Mask->Type;
	setVarParam.FieldName = Mask->VarName;
	 
	return((SSPErr)(*sql_dispatch)((CntrlParam*)&setVarParam));
}


/*------------------------------------------------------------*
	SQConnect is a function returning a value of type SSPErr.
	The parameters are :
		SSPName		NetWorkLink 			I: Type of Network Link
		SSPName		ConName 					I: Logical Connection Name
    SSPString	LinkParam 				I: Connection Parameters
    SSPRefNum	*SerRefNum 				O: Connection RefNum
 *------------------------------------------------------------*/
SSPErr SQConnect(SSPName NetWorkLink, 
									SSPString LinkParam, 
									SSPName ConName, 
									SSPRefNum *SerRefNum)
{
	connectRecord connectParam;
	
	connectParam.LinkName = NetWorkLink;
	connectParam.LinkParam = LinkParam;
	connectParam.csCode = SQLConnect;
	connectParam.ConName = ConName;
	(*sql_dispatch)((CntrlParam*)&connectParam);
		
	*SerRefNum = connectParam.SerRefNum;
	return((SSPErr)connectParam.ioResult);
}


/*------------------------------------------------------------*
	SQLogon is a function returning a value of type SSPErr.
	The parameters are :
		SSPRefNum		SerRefNum						I: Connection RefNum
		SSPName			LogParam1						I: Logon Parameter 1
		SSPName			LogParam2						I: Logon Parameter 2
		SSPName			LogName							I: Logical Session Name
		SSPRefNum		*LogRefNum					O: Logon/Session RefNum
		SSPSrvRtn		*SerRetCode					O: Server Return Code
 *------------------------------------------------------------*/
SSPErr SQLogon(SSPRefNum SerRefNum, 
								SSPString LogParam1, 
								SSPString LogParam2, 
								SSPName LogName, 
								SSPRefNum *LogRefNum, 
								SSPSrvRtn *SerRetCode)
{
	logonRecord 	logonParam;
	
	logonParam.SerRefNum = SerRefNum;
	logonParam.UserId = LogParam1;
	logonParam.PassWord = LogParam2;
	logonParam.csCode = SQLLogon;
	logonParam.LogName = LogName;
	 
	(*sql_dispatch)((CntrlParam*)&logonParam);
				
	*LogRefNum = logonParam.LogRefNum;
	*SerRetCode = (SSPSrvRtn)logonParam.SerRetCode;
	return((SSPErr)logonParam.ioResult);
}


/*------------------------------------------------------------*
	SQOpen is a function returning a value of type SSPErr.
	The parameters are :
		SSPRefNum		LogRefNum					I: Logon/Session RefNum
		SSPName			CursName					I: Logical Cursor Name
		SSPRefNum		*CursRefNum				O: Cursor RefNum
		SSPSrvRtn		*SerRetCode				O: Server Return Code
 *------------------------------------------------------------*/
SSPErr SQOpen(SSPRefNum LogRefNum, 
								SSPName CursName, 
								SSPRefNum *CursRefNum, 
								SSPSrvRtn *SerRetCode)
{
	openRecord		openParam;
	
	openParam.LogRefNum = LogRefNum;
	openParam.csCode = SQLOpen;
	openParam.CursName = CursName;
	openParam.flgs = 0;
	openParam.flgs = CURSOR_v24;
	 
	(*sql_dispatch)((CntrlParam*)&openParam);
	 
	*CursRefNum = openParam.CursRefNum;
	*SerRetCode = (SSPSrvRtn)openParam.SerRetCode;
	return((SSPErr)openParam.ioResult);
}


/*------------------------------------------------------------*
	SQAssociate is a function returning a value of type SSPErr.
	The parameters are :
		SSPRefNum		CursRefNum			  	I: Cursor RefNum
		SSPStmt			SqlStatement			  I: SQL Statement
		SSPParse		*ParseOffset				O: Parser return
		SSPSrvRtn		*SerRetCode					O: Server Return Code
 *------------------------------------------------------------*/
SSPErr SQAssociate(SSPRefNum CursRefNum, 
										SSPStmt SqlStatement, 
										SSPParse *ParseOffset, 
										SSPSrvRtn *SerRetCode)
{ 
	associateRecord	associateParam;
	
	associateParam.CursRefNum = CursRefNum;
	associateParam.SqlStatement = SqlStatement;
	associateParam.csCode	= SQLAssociate;
	 
	(*sql_dispatch)((CntrlParam*)&associateParam);
	 
	*ParseOffset = associateParam.ParseOffset;
	*SerRetCode = (SSPSrvRtn)associateParam.SerRetCode;
	return((SSPErr)associateParam.ioResult);
}


/*----------------------------------------------------------------------*
  SQnDescribe is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum		CursRefNum         	I:	Cursor RefNum
    SSPKind			Kind             		I: 	1 = Select; 2 = Bind
    SSPCount		*Count              IO:	# of variables requested
    SSPDescList	(*(*DescList))[]		IO:	Address of Data Buffer
    SSPSrvRtn		*SerRetCode   			O:	Server Return Code
 *----------------------------------------------------------------------*/
SSPErr SQnDescribe(SSPRefNum CursRefNum, 
										SSPKind Kind,
										SSPCount *Count,
										SSPDescList (*(*DescList))[],
										SSPSrvRtn *SerRetCode)
{
	describeRecord	describeParam;
	
	describeParam.CursRefNum = CursRefNum;
	describeParam.csCode = SQLnDescribe;
	describeParam.Kind = Kind;
	describeParam.Index = 1;
	describeParam.Count = (short) *Count;
	describeParam.Usrflags = 0;
	describeParam.ElemSize = (short)sizeof(SSPDescList);
		
	if(DescList != NULL)
		{
			describeParam.nDescArr = *DescList;

			(*sql_dispatch)((CntrlParam*)&describeParam);

			*Count = (SSPCount) describeParam.Count;
			*DescList = describeParam.nDescArr;
			*SerRetCode = (SSPSrvRtn)describeParam.SerRetCode;
			return((SSPErr)describeParam.ioResult);
		}
	else
		return((SSPErr)cntnDescribeErr);
}


/*------------------------------------------------------------*
	SQDefine is a function returning a value of type SSPErr.
	The parameters are :
		SSPRefNum			CursRefNum				I: Cursor RefNum
		SSPKind				Kind							I: 1, 3 = Select; 2, 4 = Bind
		SSPType 			Type							I: Data Type
		SSPSize 			Size							I: Size in bytes
		SSPData				Buffer						I: Address of Data Buffer
		SSPName				VarName						I: Variable Name
		SSPIndicator	*Indicator				I: Address of Indicator
		SSPName				IndName						I: Indicator Name
		SSPRefNum			RelCursor				  I: Related Cursor RefNum
		SSPSrvRtn			*SerRetCode			  O: Server Return Code
 *------------------------------------------------------------*/
SSPErr SQDefine(SSPRefNum CursRefNum, 
									SSPKind Kind, 
									SSPType Type,
									SSPSize Size,
									SSPData Buffer,
									SSPName FieldName,
									SSPIndicator *Indicator,
									SSPName IndicatorName,
									SSPRefNum RelCursor,
									SSPSrvRtn *SerRetCode)
{ 
	defineRecord	defineParam;
	
	defineParam.CursRefNum = CursRefNum;
	defineParam.Kind = Kind;
	defineParam.Type = Type;
	defineParam.Size = (short)Size;
	defineParam.buffer = Buffer;
	defineParam.FieldName = FieldName;  
	defineParam.Indicator = Indicator;
	defineParam.IndName = IndicatorName;
	defineParam.CallBack = NULL;    /* Not used in this interface */
	defineParam.csCode = SQLDefine;
	defineParam.FetchCRN = RelCursor;
		
	(*sql_dispatch)((CntrlParam*)&defineParam);
		
	*SerRetCode = (SSPSrvRtn)defineParam.SerRetCode;
	return((SSPErr)defineParam.ioResult);
}


/*----------------------------------------------------------------------*
  SQnDefine is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum		CursRefNum       	I: Cursor RefNum
    SSPCount		nrVar           	I: Number of variables
    SSPDefList	Vars[]        		I: repeated variable struct
    SSPSrvRtn		*SerRetCode     	O: Server Return Code
 *----------------------------------------------------------------------*/
SSPErr SQnDefine(SSPRefNum CursRefNum,
									SSPCount nrVar,
									SSPDefList Vars[],
									SSPSrvRtn *SerRetCode)
{
	ndefineRecord	ndefineParam;   
	char * 				bufferPtr;
	SSPCount 			i;
	long					memsize;
#ifdef _WINDOWS
	HGLOBAL 			bufferHnd;
#endif

	if (Vars == NULL)	
		return((SSPErr)nDefNullVarsErr);

	memsize = nrVar * (sizeof(short)
										+ sizeof(short)
										+ sizeof(short)
										+ sizeof(short)
										+ sizeof(void *)
										+ sizeof(void *)
										+ sizeof(void *)
										+ sizeof(void *)
										+ sizeof(short));

#ifdef _WINDOWS
	bufferHnd = GlobalAlloc(GMEM_MOVEABLE,memsize);
	if (bufferHnd == NULL)
		return ((SSPErr)nDefMemError);
		bufferPtr = (char *) GlobalLock(bufferHnd);
#else				
		bufferPtr = NewPtr((Size) memsize);
		if (bufferPtr == NULL)
			return ((SSPErr)nDefMemError);
#endif	
	ndefineParam.FieldNum = (short *) bufferPtr;
	bufferPtr += (nrVar * sizeof(short));
	ndefineParam.Type = (short *) bufferPtr;
	bufferPtr += (nrVar * sizeof(short));
	ndefineParam.Kind = (short *) bufferPtr;
	bufferPtr += (nrVar * sizeof(short));
	ndefineParam.Size = (short *) bufferPtr;
	bufferPtr += (nrVar * sizeof(short));
	ndefineParam.buffer = (char **) bufferPtr;
	bufferPtr += (nrVar * sizeof(void *));
	ndefineParam.FieldName = (char **) bufferPtr;
	bufferPtr += (nrVar * sizeof(void *));
	ndefineParam.Indicator = (short **) bufferPtr;
	bufferPtr += (nrVar * sizeof(void *));
	ndefineParam.IndName = (char **) bufferPtr;
	bufferPtr += (nrVar * sizeof(void *));
	ndefineParam.FetchCRN = (short *) bufferPtr;

	bufferPtr = (char *)ndefineParam.FieldNum; 

	for(i = 0 ; i < nrVar ; i++)
		{
			ndefineParam.Type[i] = Vars[i].Type;
			ndefineParam.Kind[i] = Vars[i].Kind;
			ndefineParam.Size[i] = (short)Vars[i].Size;
			ndefineParam.buffer[i] = Vars[i].Buffer;
			ndefineParam.FieldName[i] = Vars[i].VarName;
			ndefineParam.Indicator[i] = Vars[i].Indicator;
			ndefineParam.IndName[i] = Vars[i].IndName;
			ndefineParam.FetchCRN[i] = Vars[i].RelCursor;
		}
			
	ndefineParam.CursRefNum = CursRefNum;
	ndefineParam.Quantity = (short) nrVar;
	ndefineParam.CallBackF = 0L;
	ndefineParam.CallBackB = 0L;
	ndefineParam.csCode = SQLnDefine;
	(*sql_dispatch)((CntrlParam*)&ndefineParam);

#ifdef _WINDOWS
	GlobalUnlock(bufferHnd);
	GlobalFree(bufferHnd);
#else
	DisposPtr(bufferPtr);
#endif
		
	*SerRetCode = (SSPSrvRtn)ndefineParam.SerRetCode;
	return((SSPErr)ndefineParam.ioResult);
}


/*------------------------------------------------------------*
	SQBind is a function returning a value of type SSPErr.
	The parameters are :
		SSPRefNum CursRefNum 				I: Cursor RefNum
		SSPSrvRtn *SerRetCode 			O: Server Return Code
 *------------------------------------------------------------*/
SSPErr SQBind(SSPRefNum CursRefNum, SSPSrvRtn *SerRetCode)
{
	bindRecord		bindParam;
	
	bindParam.CursRefNum = CursRefNum;
	bindParam.csCode = SQLBind;
	bindParam.cbParam = 0L;
		
	(*sql_dispatch)((CntrlParam*)&bindParam);
		
	*SerRetCode = (SSPSrvRtn)SSP_OK;
	return((SSPErr)bindParam.ioResult);
}


/*------------------------------------------------------------*
	SQExec is a function returning a value of type SSPErr.
	The parameters are :
		SSPRefNum	CursRefNum 				I: Cursor RefNum
		SSPCount	*RowCount 				O: Nr of Rows affected
		SSPSrvRtn	*SerRetCode 			O: Server Return Code
 *------------------------------------------------------------*/
SSPErr SQExec(SSPRefNum CursRefNum, 
								SSPCount *RowCount,
								SSPSrvRtn *SerRetCode)
{
	executeRecord	executeParam;

	executeParam.CursRefNum = CursRefNum;
	executeParam.csCode = SQLExec;
	executeParam.flgs = 0;

	(*sql_dispatch)((CntrlParam*)&executeParam);

	*RowCount = (SSPCount) executeParam.RowCount;
	*SerRetCode = (SSPSrvRtn)executeParam.SerRetCode;
	return((SSPErr)executeParam.ioResult);
}


/*----------------------------------------------------------------------*
  SQImmediate is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum		LogRefNum      		I: Logon/Session RefNum
    SSPStmt			Statement       	I: Statement
    SSPCount		*RowCount        	O: Nr of Rows affected
    SSPParse		*ParseOffset    	O: Parser return
    SSPSrvRtn		*SerRetCode       O: Server Return Code
 *----------------------------------------------------------------------*/
SSPErr SQImmediate(SSPRefNum LogRefNum,
										SSPStmt Statement,
										SSPCount *RowCount,
										SSPParse *ParseOffset,
										SSPSrvRtn *SerRetCode)
{
	immediateRecord	immediateParam;

	immediateParam.csCode = SQLImmediate;
	immediateParam.LogRefNum = LogRefNum;
	immediateParam.SqlStatement = Statement;

	(*sql_dispatch)((CntrlParam*)&immediateParam);

	*RowCount = (SSPCount) immediateParam.RowCount;
	*SerRetCode = (SSPSrvRtn)immediateParam.SerRetCode;
	*ParseOffset = (SSPParse)immediateParam.ParseOffset;
	return((SSPErr)immediateParam.ioResult);
}


/*------------------------------------------------------------*
	SQFetch is a function returning a value of type SSPErr.
	The parameters are :
		SSPRefNum		CursRefNum				I:	Cursor RefNum
		SSPCount		*RowCount					IO: Nr of Rows affected
		SSPSrvRtn		*SerRetCode				O:	Server Return Code
 *------------------------------------------------------------*/
SSPErr SQFetch(SSPRefNum CursRefNum, 
								SSPCount	*RowCount,
								SSPSrvRtn	*SerRetCode)
{
	fetchRecord		fetchParam;

	fetchParam.CursRefNum = CursRefNum;
	fetchParam.RowCount = (short) *RowCount;
	fetchParam.csCode = SQLFetch;

	(*sql_dispatch)((CntrlParam*)&fetchParam);

	*RowCount = (SSPCount) fetchParam.RowCount;
	*SerRetCode = (SSPSrvRtn)fetchParam.SerRetCode;
	return((SSPErr)fetchParam.ioResult);
}


/*------------------------------------------------------------*
	SQCommit is a function returning a value of type SSPErr.
	The parameters are :
		SSPRefNum LogRefNum					I: Logon/Session RefNum
		SSPSrvRtn *SerRetCode				O: Server Return Code
 *------------------------------------------------------------*/
SSPErr SQCommit(SSPRefNum LogRefNum, SSPSrvRtn *SerRetCode)
{ 
	commitRecord	commitParam;

	commitParam.LogRefNum = LogRefNum;
	commitParam.csCode = SQLCommit;

	(*sql_dispatch)((CntrlParam*)&commitParam);
		
	*SerRetCode = (SSPSrvRtn)commitParam.SerRetCode;
	return((SSPErr)commitParam.ioResult);
}


/*------------------------------------------------------------*
	SQRollback is a function returning a value of type SSPErr.
	The parameters are :
		SSPRefNum LogRefNum				  I: Logon/Session RefNum
		SSPSrvRtn *SerRetCode			  O: Server Return Code
 *------------------------------------------------------------*/
SSPErr SQRollback(SSPRefNum LogRefNum, SSPSrvRtn *SerRetCode)
{
	rollbackRecord	rollbackParam;

	rollbackParam.LogRefNum = LogRefNum;
	rollbackParam.csCode = SQLRollback;

	(*sql_dispatch)((CntrlParam*)&rollbackParam);

	*SerRetCode = (SSPSrvRtn)rollbackParam.SerRetCode;
	return((SSPErr)rollbackParam.ioResult);
}


/*------------------------------------------------------------*
	SQCancel is a function returning a value of type SSPErr.
	The parameters are :
		SSPRefNum CursRefNum				I: Cursor RefNum
		SSPSrvRtn *SerRetCode				O: Server Return Code
 *------------------------------------------------------------*/
SSPErr SQCancel(SSPRefNum CursRefNum, SSPSrvRtn *SerRetCode)
{
	cancelRecord	cancelParam;

	cancelParam.CursRefNum = CursRefNum;
	cancelParam.csCode = SQLCancel;
	
	(*sql_dispatch)((CntrlParam*)&cancelParam);

	*SerRetCode = (SSPSrvRtn)SSP_OK;
	return((SSPErr)cancelParam.ioResult);
}


/*------------------------------------------------------------*
	SQErrorMessage is a function returning a value of type SSPErr.
	The parameters are :
		SSPRefNum SerRefNum					I: Connection RefNum
		SSPSrvRtn MsgNumber					I: Server Error Code
		SSPString MsgBuffer					O: Error Message text
		SSPSrvRtn *SerRetCode				O: Server Return Code
 *------------------------------------------------------------*/
SSPErr SQErrorMessage(SSPRefNum SerRefNum, 
												SSPSrvRtn MsgNumber,
												SSPString MsgBuffer,
												SSPSrvRtn *SerRetCode)
{
	errmesRecord	errmesParam;
	
	errmesParam.SerRefNum = SerRefNum;
	errmesParam.SerRetCode = (short)MsgNumber;
	errmesParam.csCode = SQLErrorMessage;
	errmesParam.MessBuf = MsgBuffer;

	(*sql_dispatch)((CntrlParam*)&errmesParam);

	*SerRetCode = (SSPSrvRtn)SSP_OK;
	return((SSPErr)errmesParam.ioResult);
}


/*----------------------------------------------------------------------*
  SQTransLit is a function returning a value of type SSPErr.
  The parameters are :
    SSPRefNum SerRefNum         I: Session RefNum
    SSPName		CodeName          I: TransLit Code Name
    short			Action            I: Indication to switch transliteration on 
																	  (SSP_TrOn) or off (SSP_TrOff)
 *----------------------------------------------------------------------*/
SSPErr SQTransLit(SSPRefNum SerRefNum, 
									SSPName TransLitResource, 
									short Action)
{
	long 					ParamList[3];
	cntrlRecord		ctlParam;

	if (Action)
		ParamList[0] = 1;
	else
		ParamList[0] = 0;

	ParamList[1] = SerRefNum;
	ParamList[2] = (long) TransLitResource;

	ctlParam.csCode	= SQLControl;
	ctlParam.Action	= 3;
	ctlParam.ParamList	= (char *) ParamList;
	return((SSPErr)(*sql_dispatch)((CntrlParam*)&ctlParam));
}


/*------------------------------------------------------------*
	SQClose is a function returning a value of type SSPErr.
	The parameters are :
		SSPRefNum CursRefNum				I: Cursor RefNum
		SSPSrvRtn *SerRetCode				O: Server Return Code
 *------------------------------------------------------------*/
SSPErr SQClose(SSPRefNum CursRefNum, SSPSrvRtn *SerRetCode)
	{
		closeRecord	closeParam;
	
		closeParam.CursRefNum = CursRefNum;
		closeParam.csCode = SQLClose;
	
		(*sql_dispatch)((CntrlParam*)&closeParam);
	 
		*SerRetCode = (SSPSrvRtn)closeParam.SerRetCode;
		return((SSPErr)closeParam.ioResult);
	}


/*------------------------------------------------------------*
	SQLogoff is a function returning a value of type SSPErr.
	The parameters are :
		SSPRefNum LogRefNum					I: Logon/Session RefNum
    SSPSrvRtn *SerRetCode				O: Server Return Code
 *------------------------------------------------------------*/
SSPErr SQLogoff(SSPRefNum LogRefNum, SSPSrvRtn *SerRetCode)
{ 
	logoffRecord	logoffParam;

	logoffParam.LogRefNum = LogRefNum;
	logoffParam.csCode = SQLLogoff;

	(*sql_dispatch)((CntrlParam*)&logoffParam);

	*SerRetCode = (SSPSrvRtn)logoffParam.SerRetCode;
	return((SSPErr)logoffParam.ioResult);
}


/*------------------------------------------------------------*
	SQDisconnect is a function returning a value of type SSPErr.
	The parameters are :
		SSPRefNum SerRefNum					I: Connection RefNum  or 0
 *------------------------------------------------------------*/
SSPErr SQDisconnect(SSPRefNum SerRefNum)
{
	disconnectRecord	disconnectParam;

	disconnectParam.SerRefNum = SerRefNum;
	disconnectParam.csCode = SQLDisconnect;
	return((SSPErr)(*sql_dispatch)((CntrlParam*)&disconnectParam));
}
