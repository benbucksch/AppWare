///////////////////////////////////////////////////////////////
//
//	(c) 1993, 1994 Novell, Inc.  All rights reserved.
//
//	THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//	TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//	COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//	CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//	TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//	NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//	AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//	LIABILITY.
//
//////////////////////////////////////////////////////////////*/

////////////////////////////////////////////////////////////////////////////
//
//	"scrp_main.cpp"
//
// scrp ALM entry point.
//
////////////////////////////////////////////////////////////////////////////

#define EMUL
#define ASTRICT
#define STRICT

#include	<a_alm.h>
#include <stdlib.h>
#include <string.h>
#include <intdebug.h>
#include <math.h>
#include <io.h>

#include	<o_number.h>
#include	<o_text.h>
#include "scrp_bas.h"


BOOL setGlobalVariables();
BOOL setStructureMembers();
BOOL callPascalFunc();
LONG cnvrt_to_file(OBJECTID, BOOL);
BOOL getRetValue();
BOOL setFunctionArgs();


LONG			numArgs;						// number of args in function call.
OBJECTID		theObjectID;
OBJECTID		RESObjectID;				// result argument Object ID.
FARPROC		scrpFunctEntryPnt;		// scrp function entry point.

SBL_LPMEMBER_INFO	typeMemberInfoPtr;	// ptr to structure member info.
int			numberOfMembers;			// number of members in structure.
MEMBLOCKID	typeMemberInfoBlkID;		// mem block ID for member infos.


/**************************************************************************
 *	SBL basic iinit structure...
 *************************************************************************/

extern SBI_INIT	SBLInit;				// initialize structure.
extern SBI_LPINIT	pSBLInit;
extern HSBI			SBIHandle;
HSBL					SBLHandle = NULL;	// Handle to sbl instance.
//SBL_PVOID			retValuePtr;		// pointer to return value.
char *				retValuePtr;		// pointer to return value.
MEMBLOCKID			retObjBlksId;		// Memory block ID for return value.

//SBL_PVOID SBL_FAR*	ppArgs[50];
SBL_PVOID 	ppArgs[50];
//SBL_PVOID SBL_FAR*	ppArgs[50];
MEMBLOCKID				callingArgsMemBlkID[50];




short*	basIntTypePtr;
long*		basLongTypePtr;
float*	basSingleTypePtr;
double*	basDoubleTypePtr;
//double*	basCurrencyTypePtr;
SBL_LPCY	basCurrencyTypePtr;
SBL_LPSTRING_DESCR	stringDesCrPtr;

double fuckingDouble;
static char mycur[] = {0,0,0,0,0,0,0,0,0};
static char myretcur[] = {0,0,0,1,2,7,5,0,0};
SBL_PVOID		p[2];


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  start_environment
// DESCRIPTION: Will setup a basic script environment, including the
//		registration of all global variables, structures and their definitions.
//
//	Return value:
//			NONE
//
/////////////////////////////////////////////////////////////////////////////

extern "C" far pascal _export start_environment(
  pAEvtInfo  		theEvent,
  pAObjMessage    theSystem)
{
	BOOL				retVal = FALSE;		// assume that function call will fail.
	OBJECTID			errorObjectID;			// ERROR object ID.
	TYPEID			theOutputType;			// alm type of the output param.

	SBL_DATATYPE		dataType;
	SCRP_FUNCNAME*		aPtr;
	long  				ecjretval = 0;
	SBL_STATUS 			status;
	SCRP_FUNCNAME*		scrpFuncName;		// scrp PTR to data0.
	long					theObjGetKey;
	int					defErr;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;    //only one possible outflow
	theObjectID = AFuncGetParameter(1);
	errorObjectID = AFuncGetOutput(1, &theOutputType);

	if(SBIHandle == NULL) {
		pSBLInit = &SBLInit;
		SBIDefaults(pSBLInit, sizeof(SBLInit));
//		SBLInit.hWndParent = parentWin;
		strcpy(SBLInit.szWndName, "AppWare Basic Script Language");
		SBIInit(pSBLInit, sizeof(SBLInit), &SBIHandle);	// Create SBI instance.

	}			// if(SBIHandle != NULL...
	SBLHandle = SBIhSbl(SBIHandle);
// create thread..

	theObjGetKey = (long)AOBJ_GETFIRSTKEY;			// start the enumeration of objects.
	while ((theObjectID = AObjGetNextObject((TYPEID)scrpTypeID, (LPLONG) &theObjGetKey)) != 0 ) {
		scrpFuncName = (SCRP_FUNCNAME*)AObjLockData(theObjectID, SCRP_INFO_DBINDEX);
		if (scrpFuncName->variables)
			retVal = setGlobalVariables();
		if (scrpFuncName->structure) {
			setStructureMembers();			// go setup the elememnts of the structure.
			typeMemberInfoPtr = (SBL_LPMEMBER_INFO)AMemLock(typeMemberInfoBlkID);
			dataType = SblRegisterType(SBLHandle, &scrpFuncName->scrpFunction[0], numberOfMembers, typeMemberInfoPtr);
			if (SBL_DT_TYPE_OF(dataType) == SBL_DATATYPE_ILLEGAL) {
				defErr = SBL_DT_INDEX_OF(dataType);
			}
			else {
				status = SblRegisterVar(SBLHandle, &scrpFuncName->scrpFunction[0], dataType, SBL_SCOPE_GLOBAL);
			}
			AObjUnlockData(theObjectID, SCRP_INFO_DBINDEX);
		}			// if (scrpFuncName->structure)...
	}					// while (theOBJ !=0)...
}			// start_environment(...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  end_environment
// DESCRIPTION: Will terminate a basic script environment, including the
//		registration of all global variables, structures and their definitions.
//
//	Return value:
//			NONE
//
/////////////////////////////////////////////////////////////////////////////

extern "C" far pascal _export end_environment(
  pAEvtInfo  theEvent,
  pAObjMessage       theSystem)
{
	if(SBIHandle != NULL) {
		SblFreeAllGlobals(SBIHandle);
		SBITerm(SBIHandle);
		SBIHandle = NULL;
		SBLHandle = NULL;
	}
}			// end_environment(...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  call_SCRP
// DESCRIPTION: Will execute a basic script if a global  environment does
//		not extist it will create and terminate a local environment for the
//		script.  It will register all global variables, structures and their
//		definitions.
//
//	Return value:
//			NONE
//
/////////////////////////////////////////////////////////////////////////////

extern "C" far pascal _export call_SCRP(
  pAEvtInfo  theEvent,
  pAObjMessage       theSystem)
{
	int					languageType;
	HANDLE				scrpHandle;
	char*					newSysPtr;
	BOOL					retVal = FALSE;				// FALSE: function call will fail.
	SCRP_FUNCNAME*		aPtr;
	TYPEID				inParamType;
	OBJECTID				errorObjectID;					// ERROR object ID.
	TYPEID				theOutputType;					// alm type of the output param.
	BOOL					localEnvironment = FALSE;	// TRUE: local script environment.
	int					retParamType;
	long  				ecjretval = 0;
	SBL_STATUS 			status;
	short					ShortType = 0;
	SCRP_ARGUMENTS*	dataBlockPtr = NULL;

	SBL_LPSHORT	tempPtr;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;    //only one possible outflow
	theObjectID = AFuncGetParameter(1);
	errorObjectID = AFuncGetOutput(1, &theOutputType);

	unlink ("ecjtest0.sbl");

	if(SBIHandle == NULL) {
		pSBLInit = &SBLInit;
		SBIDefaults(pSBLInit, sizeof(SBLInit));
		if (!SBIInit(pSBLInit, sizeof(SBLInit), &SBIHandle)) {	// Create SBI instance.
			SBLHandle = SBIhSbl(SBIHandle);
			localEnvironment = TRUE;
		}			// if (!SBIInit(...
	}			// if(SBIHandle == NULL...

	if (SBLHandle != NULL){
		if (cnvrt_to_file(theObjectID, TRUE)){
//			retVal = setGlobalVariables();
			retVal = setFunctionArgs();
			retValuePtr = (char*)AMemLock(retObjBlksId);
			aPtr = (SCRP_FUNCNAME*)AObjLockData(theObjectID, SCRP_INFO_DBINDEX);
				// parameters for SBICall: instant handle, INVOKE DEBBUGER,
				// macro file name, subroutine to run, no signature check,
				// no trigger to store, return value
/* temp stuff to test currency...
			p[0] = myretcur;
			p[1] = mycur;
//			(double)mycur[0] = (double)12.25;
			strcpy(mycur, "12.75");
			status = SBIRun(SBIHandle,SBI_F_DEBUG, "ecjtest0",
								&aPtr->scrpFunction[0], NULL, NULL, p);
// end of currency temp stuff...
*/

			if (aPtr->debuggEnable) {
//				status = SBICall(SBIHandle, SBI_F_DEBUG, "ecjtest0",
//								&aPtr->scrpFunction[0], NULL, NULL, retValuePtr);
				status = SBIRun(SBIHandle, SBI_F_DEBUG, "ecjtest0",
								&aPtr->scrpFunction[0], NULL, NULL, ppArgs);
			}			// if (aPtr->debuggEnable...
			else {
//				status = SBICall(SBIHandle, 0, "ecjtest0", &aPtr->scrpFunction[0],
//										NULL, NULL, retValuePtr);
				status = SBIRun(SBIHandle, 0, "ecjtest0",
								&aPtr->scrpFunction[0], NULL, NULL, ppArgs);
			}			// if (aPtr->debuggEnable... else...

			unlink ("ecjtest0.sbl");
//			getRetValue();
			ONmbrSetInteger(errorObjectID, (long) status, FALSE);	// set error id.
			if (status != 0)
				theSystem->message1 = 2;    //error flow...
			AObjUnlockData(theObjectID, 0);
			if (status != 0)
				retVal = A_ERROR;
			else
				retVal = A_OK;
		}			// if(cnvrt_to_file(...
	}		// if (SBLHandle != NULL...
	if (localEnvironment) {
		SblFreeAllGlobals(SBIHandle);
		SBITerm(SBIHandle);
		SBIHandle = NULL;
		SBLHandle = NULL;
	}			// if (localEnvironment)...

	dataBlockPtr = (SCRP_ARGUMENTS*)AObjLockData(theObjectID, SCRP_PARAM_DBINDEX);		// get return onject ID.
	RESObjectID = dataBlockPtr->theArgObject;
	retParamType = dataBlockPtr->paramType;
	AObjUnlockData(theObjectID,SCRP_PARAM_DBINDEX);
//	ONmbrSetInteger(RESObjectID, retValuePtr, FALSE);

				// call the function and pop the return value off the stack.
	switch (retParamType) {
		case BASTYPE_INTEGER:							// 16 bits...
			basIntTypePtr = (short*)ppArgs[0];
			ONmbrSetInteger(RESObjectID, *basIntTypePtr, FALSE);
			break;

		case BASTYPE_LONG:							// 32 bits...
			basLongTypePtr = (long*)ppArgs[0];
			ONmbrSetInteger(RESObjectID, *basLongTypePtr, FALSE);
			break;

		case BASTYPE_SINGLE:							// 32 bits...
			basSingleTypePtr = (float*)ppArgs[0];
			ONmbrSetReal(RESObjectID, (long double) *basSingleTypePtr, FALSE);
			break;

		case BASTYPE_DOUBLE:						// 64 bits...
			basDoubleTypePtr = (double*)ppArgs[0];
			ONmbrSetReal(RESObjectID, (long double) *basDoubleTypePtr, FALSE);
			break;

		case BASTYPE_CURRENCY:						// 80 bits...
			basDoubleTypePtr = (double*)ppArgs[0];
			ONmbrSetReal(RESObjectID, *basDoubleTypePtr, FALSE);
//					*basCurrencyTypePtr = (double)LongDoubleType;
			break;

//		case BASTYPE_FIXEDSTR:
//			OTextSetValueFromBuffer(RESObjectID, retValuePtr);
//			break;

		case BASTYPE_DYNAMICSTR:
//		char* temp;
//		SBL_LPSTRING_DESCR shit;

//			shit = (SBL_LPSTRING_DESCR)retValuePtr;
//			temp = (char*)(shit->pch);
			stringDesCrPtr = (SBL_LPSTRING_DESCR)ppArgs[0];
			OTextSetValueFromBuffer(RESObjectID, stringDesCrPtr->pch);
			break;

		default:
			break;
	}			// switch (languageType)...
	AMemUnlock(retObjBlksId);
	AMemFree(retObjBlksId);
	return retVal;
}			// call_SCRP(...


/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  setFunctionArgs
// DESCRIPTION: Will setup arguments to run the scrp function.
//
//	Return value:
//			TRUE				All arguments are registered.
//			FALSE				Something when wrong, could not register arguments.
/////////////////////////////////////////////////////////////////////////////

BOOL setFunctionArgs()

{

	short					basIntType = 0;
	long					basLongType = 0;
	float					basSingleType =0;
	double				basDoubleType = 0;
	SBL_CY				basCurrencyType;
//	SBL_LPSTRING_DESCR	stringDesCrPtr;


	long					fixedSize;
	long double			LongDoubleType = 0;
	OBJECTID				argObjectID = 0;
	MEMBLOCKID			objBlksId;			// memblock id for text object.
	char*		 			memBlkPtr;
	SCRP_ARGUMENTS*	dataBlockPtr = NULL;
	SCRP_ARGUMENTS*	dataBlockResPtr = NULL;
	BOOL					retVal = TRUE;				// assume that function will make it.
	int 					i = 0;
	LPSTR					argNamePtr;	// argument name.
	SBL_STATUS			status;
//	long		shit;


//SBL_PVOID SBL_FAR*	ppArgs[50];
//MEMBLOCKID				callingArgsMemBlkID[50];
//		char* temp;
//		SBL_LPSTRING_DESCR shit;

//			shit = (SBL_LPSTRING_DESCR)retValuePtr;
//			temp = (char*)(shit->pch);

				// skip DATA 0,1,2 remember that DATA3 is return parameter.
	numArgs = AObjGetDataCount(theObjectID)-1;
	for (i=(int)(numArgs); i>SCRP_PARAM_DBINDEX-1; --i) {
		dataBlockPtr = (SCRP_ARGUMENTS*)AObjLockData(theObjectID, i);
		argObjectID = dataBlockPtr->theArgObject;
		argNamePtr = &(dataBlockPtr->theArgObjectName[0]);
		switch (dataBlockPtr->paramType) {
			case BASTYPE_INTEGER:				// 16 bits...
				callingArgsMemBlkID[i-SCRP_PARAM_DBINDEX] = AMemAllocate(sizeof(SBL_SHORT));
				ppArgs[i-SCRP_PARAM_DBINDEX] = AMemLock(callingArgsMemBlkID[i-SCRP_PARAM_DBINDEX]);
				basIntTypePtr = (short*)ppArgs[i-SCRP_PARAM_DBINDEX];
				if (dataBlockPtr->constant) 		// if so then pass the constant.
					*basIntTypePtr = (short)dataBlockPtr->constVal;
				else		// if so, pass contants of arg.
					*basIntTypePtr = (short)ONmbrGetInteger(dataBlockPtr->theArgObject);
				break;

			case BASTYPE_LONG:				// 32 bits, large numbers...
				callingArgsMemBlkID[i-SCRP_PARAM_DBINDEX] = AMemAllocate(sizeof(SBL_LONG));
				ppArgs[i-SCRP_PARAM_DBINDEX] = AMemLock(callingArgsMemBlkID[i-SCRP_PARAM_DBINDEX]);
				basLongTypePtr = (long*)ppArgs[i-SCRP_PARAM_DBINDEX];
				if (dataBlockPtr->constant) 		// if so then pass the constant.
					*basLongTypePtr = (long)dataBlockPtr->constVal;
				else		// if so, pass contants of arg.
					*basLongTypePtr = (long)ONmbrGetInteger(dataBlockPtr->theArgObject);
				break;

			case BASTYPE_SINGLE:			// 32 bits, float, scientific 7-digit precision...
				callingArgsMemBlkID[i-SCRP_PARAM_DBINDEX] = AMemAllocate(sizeof(float));
				ppArgs[i-SCRP_PARAM_DBINDEX] = AMemLock(callingArgsMemBlkID[i-SCRP_PARAM_DBINDEX]);
				basSingleTypePtr = (float*)ppArgs[i-SCRP_PARAM_DBINDEX];
				if (dataBlockPtr->constant) 		// if so then pass the constant.
					*basSingleTypePtr = (float)dataBlockPtr->constVal;
				else {	// if so, pass contants of arg.
					ONmbrGetReal(dataBlockPtr->theArgObject, (long double*)&LongDoubleType);
					*basSingleTypePtr = (float) LongDoubleType;
				}
				break;

			case BASTYPE_DOUBLE:			// 64 bits, double,  scientific 15-digit precision...
				callingArgsMemBlkID[i-SCRP_PARAM_DBINDEX] = AMemAllocate(sizeof(double));
				ppArgs[i-SCRP_PARAM_DBINDEX] = AMemLock(callingArgsMemBlkID[i-SCRP_PARAM_DBINDEX]);
				basDoubleTypePtr = (double*)ppArgs[i-SCRP_PARAM_DBINDEX];
				if (dataBlockPtr->constant) 		// if so then pass the constant.
					*basDoubleTypePtr = (double)dataBlockPtr->constVal;
				else {	// if so, pass contants of arg.
					ONmbrGetReal(dataBlockPtr->theArgObject, (long double*)&LongDoubleType);
					*basDoubleTypePtr = (long double) LongDoubleType;
				}
				break;

			case BASTYPE_CURRENCY:			// 64 bits, double...
				long fraction, integer;
//				SBL_VARIANT theCurrency;

				callingArgsMemBlkID[i-SCRP_PARAM_DBINDEX] = AMemAllocate(sizeof(double)+1);
				ppArgs[i-SCRP_PARAM_DBINDEX] = AMemLock(callingArgsMemBlkID[i-SCRP_PARAM_DBINDEX]);
//				ppArgs[i-SCRP_PARAM_DBINDEX] = &fuckingDouble;
				basCurrencyTypePtr = (SBL_LPCY)ppArgs[i-SCRP_PARAM_DBINDEX];
//				basCurrencyTypePtr = (double*)ppArgs[i-SCRP_PARAM_DBINDEX];
				if (dataBlockPtr->constant) {		// if so then pass the constant.
//					*basCurrencyTypePtr = dataBlockPtr->constVal;
				}
				else {	// if so, pass contants of arg.
					ONmbrGetReal(dataBlockPtr->theArgObject, &LongDoubleType);
//					(long double)*basCurrencyTypePtr = LongDoubleType;

					fraction = (int)modfl(LongDoubleType, (long double*)&integer);
//					fuckingDouble = (double)LongDoubleType;
//					basCurrencyType.Hi = 5;
//					basCurrencyType.Lo = .25;
//					fuckingDouble = 5.25;
//					basCurrencyTypePtr->Hi = (SBL_LONG)LongDoubleType*10000;
					basCurrencyTypePtr->Hi = (SBL_LONG)integer;
					basCurrencyTypePtr->Lo = (SBL_LONG)fraction;
//					basCurrencyTypePtr->Hi = (double)LongDoubleType*10000;

					//					basCurrencyTypePtr->Hi = integer;
//					basCurrencyTypePtr->Lo = fraction;

//					basCurrencyTypePtr->vt = SBL_VT_CY;
//					basCurrencyTypePtr->cyVal = basCurrencyType;
				}
				break;

			case BASTYPE_FIXEDSTR:
				fixedSize = OTextGetLimit(dataBlockPtr->theArgObject);
				callingArgsMemBlkID[i-SCRP_PARAM_DBINDEX] = AMemAllocate(fixedSize);
				if (OTextGetLength(dataBlockPtr->theArgObject)) {
					objBlksId = AMemAllocate(0);
					OTextGetToMemblock(dataBlockPtr->theArgObject, objBlksId);
					memBlkPtr = (char*)AMemLock(objBlksId);
//					if (AMemGetSize(objBlksId) > 0)
					status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_MAKE_DT_FIXEDSTRING(fixedSize), memBlkPtr, FALSE);
					AMemUnlock(objBlksId);
					AMemFree(objBlksId);
				}			// if (OTextGetLength(dataBlockPtr->theArgObject))...
				else
					status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_MAKE_DT_FIXEDSTRING(fixedSize), "", FALSE);
				ppArgs[i] = (SBL_PVOID SBL_FAR*)AMemLock(callingArgsMemBlkID[i]);
				break;

			case BASTYPE_DYNAMICSTR:
				callingArgsMemBlkID[i-SCRP_PARAM_DBINDEX] = AMemAllocate(sizeof(SBL_STRING_DESCR));
				ppArgs[i-SCRP_PARAM_DBINDEX] = AMemLock(callingArgsMemBlkID[i-SCRP_PARAM_DBINDEX]);
				stringDesCrPtr = (SBL_LPSTRING_DESCR)ppArgs[i-SCRP_PARAM_DBINDEX];
 //				stringDesCrPtr = (SBL_LPSTRING_DESCR)AMemLock(callingArgsMemBlkID[i]);

				objBlksId = AMemAllocate(0);
				OTextGetToMemblock(dataBlockPtr->theArgObject, objBlksId);
				stringDesCrPtr->type = 0;
				stringDesCrPtr->pch = (SBL_LPBYTE)AMemLock(objBlksId);
				stringDesCrPtr->cch = AMemGetSize(objBlksId);
//				status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_STRING, &stringDesCrPtr, FALSE);
//xxx						status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_FIXEDSTRING, memBlkPtr, FALSE);

				AMemUnlock(objBlksId);
				break;

			default:
				break;
		}			// switch (languageType)...
		AObjUnlockData(theObjectID, i);
	}			// for (i=0; numArgs...
	return retVal;
}			// setFunctionArgs(...








/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  setStructureMembers
// DESCRIPTION: Will set members for a structure.
//
//	Return value:
//			TRUE				All elements are registered.
//			FALSE				Something when wrong, could not register arguments.
/////////////////////////////////////////////////////////////////////////////

BOOL setStructureMembers()

{
	SBL_STRING_DESCR	stringDesCr;
	long					fixedSize;

	OBJECTID			argObjectID = 0;
	MEMBLOCKID		objBlksId;			// memblock id for text object.
	char*		 		memBlkPtr;

	SCRP_ARGUMENTS*	dataBlockPtr = NULL;
	SCRP_ARGUMENTS*	dataBlockResPtr = NULL;
	BOOL					retVal = TRUE;				// assume that function will make it.
	int 					i = 0;
	LPSTR					argNamePtr;	// argument name.
	SBL_STATUS			status;
	int					memberOffset, j;

				// skip DATA 0,1,2 remember that DATA3 is return parameter.
	numArgs = AObjGetDataCount(theObjectID);
	typeMemberInfoBlkID = AMemAllocate(numArgs * sizeof(SBL_MEMBER_INFO));
	typeMemberInfoPtr = (SBL_LPMEMBER_INFO)AMemLock(typeMemberInfoBlkID);
	memberOffset = j = 0;
	for (i=SCRP_PARAM_DBINDEX+1; i<numArgs; ++i) {
		dataBlockPtr = (SCRP_ARGUMENTS*)AObjLockData(theObjectID, i);
		argObjectID = dataBlockPtr->theArgObject;
		argNamePtr = &(dataBlockPtr->theArgObjectName[0]);
		switch (dataBlockPtr->paramType) {
			case BASTYPE_INTEGER:				// 16 bits...
				strcpy(typeMemberInfoPtr->szName, argNamePtr);
				typeMemberInfoPtr->type = BASTYPE_INTEGER;
				typeMemberInfoPtr->offset = memberOffset;
				memberOffset = memberOffset + sizeof(BASTYPE_INTEGER);
				break;

			case BASTYPE_LONG:				// 32 bits, large numbers...
				strcpy(typeMemberInfoPtr->szName, argNamePtr);
				typeMemberInfoPtr->type = BASTYPE_LONG;
				typeMemberInfoPtr->offset = memberOffset;
				memberOffset = memberOffset + sizeof(BASTYPE_LONG);
				break;

			case BASTYPE_SINGLE:			// 32 bits, float, scientific 7-digit precision...
				strcpy(typeMemberInfoPtr->szName, argNamePtr);
				typeMemberInfoPtr->type = BASTYPE_SINGLE;
				typeMemberInfoPtr->offset = memberOffset;
				memberOffset = memberOffset + sizeof(BASTYPE_SINGLE);
				break;

			case BASTYPE_DOUBLE:			// 64 bits, double,  scientific 15-digit precision...
				strcpy(typeMemberInfoPtr->szName, argNamePtr);
				typeMemberInfoPtr->type = BASTYPE_DOUBLE;
				typeMemberInfoPtr->offset = memberOffset;
				memberOffset = memberOffset + sizeof(BASTYPE_DOUBLE);
				break;

			case BASTYPE_CURRENCY:			// 64 bits, double...
				strcpy(typeMemberInfoPtr->szName, argNamePtr);
				typeMemberInfoPtr->type = BASTYPE_CURRENCY ;
				typeMemberInfoPtr->offset = memberOffset;
				memberOffset = memberOffset + sizeof(BASTYPE_CURRENCY);
				break;

			case BASTYPE_FIXEDSTR:
				fixedSize = OTextGetLimit(dataBlockPtr->theArgObject);
				strcpy(typeMemberInfoPtr->szName, argNamePtr);
				typeMemberInfoPtr->type = SBL_MAKE_DT_FIXEDSTRING(fixedSize) ;
				typeMemberInfoPtr->offset = memberOffset;
				memberOffset = memberOffset + fixedSize;
				break;

			case BASTYPE_DYNAMICSTR:
				break;

			default:
				break;
		}			// switch (languageType)...
		j++;
		typeMemberInfoPtr = (SBL_LPMEMBER_INFO)((long)typeMemberInfoPtr + sizeof(SBL_MEMBER_INFO));
		AObjUnlockData(theObjectID, i);
	}			// for (i=0; numArgs...
	numArgs = numberOfMembers = numArgs - SCRP_PARAM_DBINDEX - 1;

//	dataBlockResPtr = (SCRP_ARGUMENTS*)AObjLockData(theObjectID, SCRP_PARAM_DBINDEX);		// get return onject ID.
//	RESObjectID = dataBlockResPtr->theArgObject;
//	retParamType = dataBlockResPtr->paramType;
//	AObjUnlockData(theObjectID,SCRP_PARAM_DBINDEX);





// will the work...
/*
	SBL_DATATYPE		dataType;
	SCRP_FUNCNAME*		scrpFuncName;


	AMemUnlock(typeMemberInfoBlkID);
	typeMemberInfoPtr = (SBL_LPMEMBER_INFO)AMemLock(typeMemberInfoBlkID);

	scrpFuncName = (SCRP_FUNCNAME*)AObjLockData(theObjectID, SCRP_INFO_DBINDEX);
	dataType = SblRegisterType(SBLHandle, &scrpFuncName->scrpFunction[0], numberOfMembers, typeMemberInfoPtr);
	if (SBL_DT_TYPE_OF(dataType) == SBL_DATATYPE_ILLEGAL) {
		int defErr;
		defErr = SBL_DT_INDEX_OF(dataType);
	}
	else {
		status = SblRegisterVar(SBLHandle, &scrpFuncName->scrpFunction[0], dataType, SBL_SCOPE_GLOBAL);
//		status = SblRegisterVar(SBLHandle, "hospitalRec", dataType, SBL_SCOPE_GLOBAL);
	}
	AObjUnlockData(theObjectID, SCRP_INFO_DBINDEX);
	AMemUnlock(typeMemberInfoBlkID);
*/
// did it work...

	return retVal;
}			// setStructureMembers(...












/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  setGlobalVariables
// DESCRIPTION: Will register arguments for the scrp function call as
//			global variables.
//
//	Return value:
//			TRUE				All arguments are registered.
//			FALSE				Something when wrong, could not register arguments.
/////////////////////////////////////////////////////////////////////////////

BOOL setGlobalVariables()

{

	short					basIntType = 0;
	long					basLongType = 0;
	float					basSingleType =0;
	double				basDoubleType = 0;
	double				basCurrencyType = 0;
	SBL_STRING_DESCR	stringDesCr;
	long					fixedSize;
	long double			LongDoubleType = 0;
	OBJECTID				argObjectID = 0;
	MEMBLOCKID			objBlksId;			// memblock id for text object.
	char*		 			memBlkPtr;
	SCRP_ARGUMENTS*	dataBlockPtr = NULL;
	SCRP_ARGUMENTS*	dataBlockResPtr = NULL;
	BOOL					retVal = TRUE;				// assume that function will make it.
	int 					i = 0;
	LPSTR					argNamePtr;	// argument name.
	SBL_STATUS			status;
	long		shit;

				// skip DATA 0,1,2 remember that DATA3 is return parameter.
	numArgs = AObjGetDataCount(theObjectID)-1;
	for (i=(int)(numArgs); i>SCRP_PARAM_DBINDEX-1; --i) {
		dataBlockPtr = (SCRP_ARGUMENTS*)AObjLockData(theObjectID, i);
		argObjectID = dataBlockPtr->theArgObject;
		argNamePtr = &(dataBlockPtr->theArgObjectName[0]);
		switch (dataBlockPtr->paramType) {
			case BASTYPE_INTEGER:				// 16 bits...
				if (dataBlockPtr->constant) {		// if so then pass the constant.
					basIntType = (short)dataBlockPtr->constVal;
					status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_INTEGER, &basIntType, TRUE);
				}
				else {	// if so, pass contants of arg.
					basIntType = (short)ONmbrGetInteger(dataBlockPtr->theArgObject);
					status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_INTEGER, &basIntType, FALSE);
				}
				break;

			case BASTYPE_LONG:				// 32 bits, large numbers...
				if (dataBlockPtr->constant) {		// if so then pass the constant.
					basLongType = (long)dataBlockPtr->constVal;
					status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_LONG, &basLongType, TRUE);
				}
				else {	// if so, pass contants of arg.
					basLongType = (long)ONmbrGetInteger(dataBlockPtr->theArgObject);
					status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_LONG, &basLongType, FALSE);
				}
				break;

			case BASTYPE_SINGLE:			// 32 bits, float, scientific 7-digit precision...
				if (dataBlockPtr->constant) {		// if so then pass the constant.
					basSingleType = (float)dataBlockPtr->constVal;
					status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_SINGLE, &basSingleType, TRUE);
				}
				else {	// if so, pass contants of arg.
					ONmbrGetReal(dataBlockPtr->theArgObject, (long double*)&LongDoubleType);
					basSingleType = (float) LongDoubleType;
					status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_SINGLE, &basSingleType, FALSE);
				}
				break;

			case BASTYPE_DOUBLE:			// 64 bits, double,  scientific 15-digit precision...
				if (dataBlockPtr->constant) {		// if so then pass the constant.
					basDoubleType = (double)dataBlockPtr->constVal;
					status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_DOUBLE, &basDoubleType, TRUE);
				}
				else {	// if so, pass contants of arg.
					ONmbrGetReal(dataBlockPtr->theArgObject, (long double*)&LongDoubleType);
					basDoubleType = (double) LongDoubleType;
					status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_DOUBLE, &basDoubleType, FALSE);
				}
				break;

			case BASTYPE_CURRENCY:			// 64 bits, double...
				if (dataBlockPtr->constant) {		// if so then pass the constant.
					basCurrencyType = (double)dataBlockPtr->constVal;
					status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_CURRENCY, &basCurrencyType, TRUE);
				}
				else {	// if so, pass contants of arg.
					ONmbrGetReal(dataBlockPtr->theArgObject, &LongDoubleType);
					basCurrencyType = (double)LongDoubleType;
					status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_CURRENCY, &basCurrencyType, FALSE);
				}
				break;

			case BASTYPE_FIXEDSTR:
				fixedSize = OTextGetLimit(dataBlockPtr->theArgObject);
				if (OTextGetLength(dataBlockPtr->theArgObject)) {
					objBlksId = AMemAllocate(0);
					OTextGetToMemblock(dataBlockPtr->theArgObject, objBlksId);
					memBlkPtr = (char*)AMemLock(objBlksId);
//					if (AMemGetSize(objBlksId) > 0)
					status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_MAKE_DT_FIXEDSTRING(fixedSize), memBlkPtr, FALSE);
					AMemUnlock(objBlksId);
					AMemFree(objBlksId);
				}			// if (OTextGetLength(dataBlockPtr->theArgObject))...
				else
					status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_MAKE_DT_FIXEDSTRING(fixedSize), "", FALSE);
				break;

			case BASTYPE_DYNAMICSTR:
				objBlksId = AMemAllocate(0);
				OTextGetToMemblock(dataBlockPtr->theArgObject, objBlksId);
				stringDesCr.type = 0;
				stringDesCr.pch = (SBL_LPBYTE)AMemLock(objBlksId);
				stringDesCr.cch = AMemGetSize(objBlksId);
				status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_STRING, &stringDesCr, FALSE);
//xxx						status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_FIXEDSTRING, memBlkPtr, FALSE);
				AMemUnlock(objBlksId);
				break;

			default:
				break;
		}			// switch (languageType)...
		AObjUnlockData(theObjectID, i);
	}			// for (i=0; numArgs...
	return retVal;
}			// setGlobalVariables(...








/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  getRetValue
// DESCRIPTION: Will setup return arguments.
//
//	Return value:
//			TRUE				All arguments are registered.
//			FALSE				Something when wrong, could not register arguments.
/////////////////////////////////////////////////////////////////////////////

BOOL getRetValue()

{
	SBL_LPTID			theThreadID;

	short					basIntType = 0;
	long					basLongType = 0;
	float					basSingleType =0;
	double				basDoubleType = 0;
	double				basCurrencyType = 0;
	SBL_STRING_DESCR	stringDesCr;
	long					fixedSize;
	long double			LongDoubleType = 0;
	OBJECTID				retArgObjectID = 0;
	MEMBLOCKID			objBlksId;			// memblock id for text object.
	char*		 			memBlkPtr;
	SCRP_ARGUMENTS*	dataBlockPtr = NULL;
	SCRP_ARGUMENTS*	dataBlockResPtr = NULL;
	BOOL					retVal = TRUE;				// assume that function will make it.
	int 					i = 0;
	SBL_LPSTR					argNamePtr;	// argument name.
	SBL_STATUS			status;
	SBL_LPTID			pTid;
	SBL_VAR_INFO		varInfo;

				// skip DATA 0,1,2 remember that DATA3 is return parameter.

	numArgs = AObjGetDataCount(theObjectID)-1;
	if (!SblCreateThread(SBLHandle, pTid)) {

		dataBlockPtr = (SCRP_ARGUMENTS*)AObjLockData(theObjectID, SCRP_PARAM_DBINDEX);
		retArgObjectID = dataBlockPtr->theArgObject;
		argNamePtr = &(dataBlockPtr->theArgObjectName[0]);

		status = SblGetVarInfo(*pTid, (unsigned long)NULL, argNamePtr, SBL_ENUM_GLOBAL, 0,
											&varInfo, sizeof(SBL_VAR_INFO));

/*
		for (i=(int)(numArgs); i>SCRP_PARAM_DBINDEX-1; --i) {
			dataBlockPtr = (SCRP_ARGUMENTS*)AObjLockData(theObjectID, i);
			argObjectID = dataBlockPtr->theArgObject;
			argNamePtr = &(dataBlockPtr->theArgObjectName[0]);
			switch (dataBlockPtr->paramType) {
				case BASTYPE_INTEGER:				// 16 bits...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
						basIntType = (short)dataBlockPtr->constVal;
						status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_INTEGER, &basIntType, TRUE);
					}
					else {	// if so, pass contants of arg.
						basIntType = (short)ONmbrGetInteger(dataBlockPtr->theArgObject);
						status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_INTEGER, &basIntType, FALSE);
					}
				break;

				case BASTYPE_LONG:				// 32 bits, large numbers...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
						basLongType = (long)dataBlockPtr->constVal;
						status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_LONG, &basLongType, TRUE);
					}
					else {	// if so, pass contants of arg.
						basLongType = (long)ONmbrGetInteger(dataBlockPtr->theArgObject);
						status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_LONG, &basLongType, FALSE);
					}
				break;

				case BASTYPE_SINGLE:			// 32 bits, float, scientific 7-digit precision...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
						basSingleType = (float)dataBlockPtr->constVal;
						status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_SINGLE, &basSingleType, TRUE);
					}
					else {	// if so, pass contants of arg.
						ONmbrGetReal(dataBlockPtr->theArgObject, (long double*)&LongDoubleType);
						basSingleType = (float) LongDoubleType;
						status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_SINGLE, &basSingleType, FALSE);
					}
				break;

				case BASTYPE_DOUBLE:			// 64 bits, double,  scientific 15-digit precision...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
						basDoubleType = (double)dataBlockPtr->constVal;
						status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_DOUBLE, &basDoubleType, TRUE);
					}
					else {	// if so, pass contants of arg.
						ONmbrGetReal(dataBlockPtr->theArgObject, (long double*)&LongDoubleType);
						basDoubleType = (double) LongDoubleType;
						status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_DOUBLE, &basDoubleType, FALSE);
					}
				break;

				case BASTYPE_CURRENCY:			// 64 bits, double...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
						basCurrencyType = (double)dataBlockPtr->constVal;
						status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_CURRENCY, &basCurrencyType, TRUE);
					}
					else {	// if so, pass contants of arg.
						ONmbrGetReal(dataBlockPtr->theArgObject, &LongDoubleType);
						basCurrencyType = (double)LongDoubleType;
						status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_CURRENCY, &basCurrencyType, FALSE);
					}
				break;

				case BASTYPE_FIXEDSTR:
					fixedSize = OTextGetLimit(dataBlockPtr->theArgObject);
					if (OTextGetLength(dataBlockPtr->theArgObject)) {
						objBlksId = AMemAllocate(0);
						OTextGetToMemblock(dataBlockPtr->theArgObject, objBlksId);
						memBlkPtr = (char*)AMemLock(objBlksId);
//					if (AMemGetSize(objBlksId) > 0)
						status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_MAKE_DT_FIXEDSTRING(fixedSize), memBlkPtr, FALSE);
						AMemUnlock(objBlksId);
						AMemFree(objBlksId);
					}			// if (OTextGetLength(dataBlockPtr->theArgObject))...
					else
						status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_MAKE_DT_FIXEDSTRING(fixedSize), "", FALSE);
				break;

				case BASTYPE_DYNAMICSTR:
					objBlksId = AMemAllocate(0);
					OTextGetToMemblock(dataBlockPtr->theArgObject, objBlksId);
					stringDesCr.type = 0;
					stringDesCr.pch = (SBL_LPBYTE)AMemLock(objBlksId);
					stringDesCr.cch = AMemGetSize(objBlksId);
					status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_STRING, &stringDesCr, FALSE);
//xxx						status = SblCreateGlobal(SBLHandle, argNamePtr, SBL_DATATYPE_FIXEDSTRING, memBlkPtr, FALSE);
					AMemUnlock(objBlksId);
				break;

				default:
				break;
			}			// switch (languageType)...
			AObjUnlockData(theObjectID, i);
		}			// for (i=0; numArgs...
*/
		SblKillThread(*pTid);
	}			// if (SblCreateThread(SBLHandle, pTid))...
	return retVal;
}			// getRetValue(...









/////////////////////////////////////////

extern "C" far pascal _export get_error_info(
  pAEvtInfo			theEvent,
  pAObjMessage		theSystem)
{
	BOOL			retVal = FALSE;				// FALSE: function call will fail.
	char			sblErrTextBuff[255];
	long			errorID;							// runtime basic error id.
	OBJECTID		objErrorID;						// runtime basic error id.
	OBJECTID		outObjID;						// output object error id.
	TYPEID		theOutputType;					// alm type of the output param.
	BOOL			localEnvironment = FALSE;	// TRUE: local script environment.

	I_UNUSED(theEvent);
	theSystem->message1 = 1;    //only one possible outflow
	objErrorID = AFuncGetParameter(1);
	errorID = (long)ONmbrGetInteger(objErrorID);


	if(SBIHandle == NULL) {
		pSBLInit = &SBLInit;
		SBIDefaults(pSBLInit, sizeof(SBLInit));
		if (!SBIInit(pSBLInit, sizeof(SBLInit), &SBIHandle)) {	// Create SBI instance.
			SBLHandle = SBIhSbl(SBIHandle);
			localEnvironment = TRUE;
		}			// if (!SBIInit(...
	}			// if(SBIHandle == NULL...
	SblGetPlainErrorText(errorID, &sblErrTextBuff[0],255);
	outObjID = AFuncGetOutput(1, &theOutputType);
	OTextSetValueFromBuffer(outObjID, &sblErrTextBuff[0]);
	if (localEnvironment) {
		SBITerm(SBIHandle);
		SBIHandle = NULL;
		SBLHandle = NULL;
	}			// if (localEnvironment)...
	return retVal;
}			// get_error_info(...


