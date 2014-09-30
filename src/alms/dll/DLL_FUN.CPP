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
//	"dll_main.cpp"
//
// DLL ALM entry point.
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

#include	<o_number.h>
#include	<o_text.h>
#include "dll.h"


BOOL callCFunc();
BOOL callPascalFunc();


LONG			numArgs;				// number of arguments in function call + 2.
AOBJECTID	theObjectID;
FARPROC		dllFunctEntryPnt;			// dll function entry point.


/////////////////////////////////////////

extern "C" CALLBACK /*!!PORT!! was _far_ _pascal_*/  call_DLL(
  pAEvtInfo  theEvent,
  pAObjMessage       theSystem)
{
	int				languageType;
	HANDLE			dllHandle;
	char*				newSysPtr;
	BOOL				retVal = FALSE;				// assume that function call will fail.
	DLL_FUNCNAME*	aPtr;
	ATYPEID			inParamType;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;    //only one possible outflow
	theObjectID = AFuncGetParameter(1);

	inParamType = AObjGetType(theObjectID);
	if (inParamType == (ATYPEID)dllTypeID) {
		numArgs = AObjGetDataCount(theObjectID);
		aPtr = (DLL_FUNCNAME*)AObjLockData(theObjectID, DLLNAMEINDEX);
		languageType = aPtr->languageType;
		if((lstrlen(aPtr->DLLName) != 0) && (lstrlen(aPtr->DLLFunction) != 0)) {	// init DLL and function name.
			dllHandle = LoadLibrary(aPtr->DLLName);
			if ((long)dllHandle >= (long)HINSTANCE_ERROR) {
				dllFunctEntryPnt = GetProcAddress((HINSTANCE)dllHandle, (LPSTR)(aPtr->DLLFunction));
				if (dllFunctEntryPnt) {				// push values on stack...
					switch (languageType) {
						case C_FUNCTION:
							retVal = callCFunc();
						break;

						case PASCAL_FUNCTION:
							retVal = callPascalFunc();
							break;
        	
						default:				// should never get here.
							break;
					}			// switch (languageType)...
				}			// if (dllFunctEntryPnt)...
				FreeLibrary((HINSTANCE)dllHandle);
			}			// if ((long)dllHandle >= (long)HINSTANCE_ERROR)...
			else {
				lstrcpy(theSystem->errorMessage, " DLL could not be loaded:  ");
				newSysPtr = (char*)((long)(theSystem->errorMessage) + (long)lstrlen(theSystem->errorMessage));
				lstrcpy(newSysPtr, aPtr->DLLName);
				newSysPtr = (char*)((long) theSystem->errorMessage + (long)lstrlen(theSystem->errorMessage));
				lstrcpy(newSysPtr, " - function - ");
				newSysPtr = (char*)((long)(theSystem->errorMessage) + (long)lstrlen(theSystem->errorMessage));
				lstrcpy(newSysPtr, aPtr->DLLFunction);
				newSysPtr = (char*)((long)(theSystem->errorMessage) + (long)lstrlen(theSystem->errorMessage));
				lstrcpy(newSysPtr, " - could not be found.");
				newSysPtr = (char*)((long)(theSystem->errorMessage) + (long)lstrlen(theSystem->errorMessage));
				lstrcpy(newSysPtr, "  error id: ");
				newSysPtr = (char*)((long)(theSystem->errorMessage) + (long)lstrlen(theSystem->errorMessage));
				char tmpError[25];
				ltoa((long)dllHandle, tmpError, 4);
				lstrcpy(newSysPtr, tmpError);
			}
		}
		AObjUnlockData(theObjectID, DLLNAMEINDEX);
	}			// if (inParamType == (ATYPEID)dllTypeID...
	else {
		lstrcpy(theSystem->errorMessage, " Invalid Object passed to Call DLL function. ");
		newSysPtr = (char*)((long)(theSystem->errorMessage) + (long)lstrlen(theSystem->errorMessage));
	}
  return retVal;
}			// call_DLL(...

 
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  callCFunct
// DESCRIPTION: Will push the arguments for the DLL function call on the
//			stack, it will call the DLL function and will return after its
//			completation. Arguments in C are pushed from right to left unlike
//			the rest of the world.  In C we will clean up the stack after
//			the return of the DLL function call.
//
//	Return value:
//			TRUE				funtion was called.
//			FALSE				function could not be called  
/////////////////////////////////////////////////////////////////////////////

BOOL callCFunc()

{
	short					ShortType = 0;
	unsigned short		UShortType = 0;
	long					LongType = 0;
	unsigned long		ULongType = 0;
	float					FloatType = 0;
	double				DoubleType = 0;
	long double			LongDoubleType = 0;

	LPVOID				tmpPtr = NULL;
	int			 		lenOfPtr = sizeof(tmpPtr);				// length of ptr for offset, in pass by referance.
	WORD					low16 = 0;
	WORD					high16 = 0;
	AOBJECTID				RESObjectID = 0;

	AMEMBLOCKID			objBlksId;						// memblock id for text object.
	AMEMBLOCKID			objBlksIds[MAXNUMOFARGS];	// memblock id for text object, we need to junk them.
	int					numberOfObjBlksIds=0;			// number of block ids allocated.
	char*					memBlkPtr;
	LPSTR					retTextPtr;	// return ptr to text string.

	int	ShortTypeLen = sizeof(ShortType);
	int	UShortTypeLen = sizeof(UShortType);
	int	LongTypeLen = sizeof(LongType);
	int	ULongTypeLen = sizeof(ULongType);
	int	FloatTypeLen = sizeof(FloatType);
	int	DoubleTypeLen = sizeof(DoubleType);
	int	LongDoubleTypeLen = sizeof(LongDoubleType);
	int	spOffset = 0;
	int	retParamType;

	DLL_ARGUMENTS*	dataBlockPtr = NULL;
	DLL_RETARG*		dataBlockResPtr = NULL;
	BOOL				retVal = TRUE;				// assume that function will make it.
	int 				i = 0;

	for (i=(int)(numArgs-1); i>DLLRETPARAMINDEX; --i) {		// copy data blocks to temp area.
		dataBlockPtr = (DLL_ARGUMENTS*)AObjLockData(theObjectID, i);
		switch (dataBlockPtr->paramType) {
			case CTYPE_SHORT:				// 16 bits...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
						ShortType = (short)dataBlockPtr->constVal;
						asm	{
							mov	ax,ShortType					// load the constant.
							push	ax										// push it on stack.
						}			// asm...
						spOffset = spOffset + ShortTypeLen;	// increment stack offset.
					}
					else if (dataBlockPtr->byValue) {	// if so, pass contants of arg.
						ShortType = (short)ONmbrGetInteger(dataBlockPtr->theArgObject);
						asm	{
							mov	ax,ShortType					// load the constant.
							push	ax										// push it on stack.
						}			// asm...
						spOffset = spOffset + ShortTypeLen;	// increment stack offset.
					}
					else {	// pass by reference, arg may be modified by function.
						ShortType = (short)ONmbrGetInteger(dataBlockPtr->theArgObject);
						tmpPtr = &ShortType;
						low16 = LOWORD(tmpPtr);
						high16 = HIWORD(tmpPtr);
						asm	{
							mov	ax,high16
							push	ax
							mov	ax,low16			// load ptr.
							push	ax								// push it on stack.
						}			// asm...
						spOffset = spOffset +	lenOfPtr;	// increment stack offset.
					}
				break;

			case CTYPE_USHORT:			// 16 bits...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
						UShortType = (unsigned short)dataBlockPtr->constVal;
						asm	{
							mov	ax,UShortType					// load the constant.
							push	ax										// push it on stack.
						}			// asm...
						spOffset = spOffset + UShortTypeLen;	// increment stack offset.
					}
					else if (dataBlockPtr->byValue) {	// if so, pass contants of arg.
						UShortType = (short)ONmbrGetInteger(dataBlockPtr->theArgObject);
						asm	{
							mov	ax,UShortType					// load the constant.
							push	ax										// push it on stack.
						}			// asm...
						spOffset = spOffset + UShortTypeLen;	// increment stack offset.
					}
					else {	// pass by reference, arg may be modified by function.
						UShortType = (short)ONmbrGetInteger(dataBlockPtr->theArgObject);
						tmpPtr = &UShortType;
						low16 = LOWORD(tmpPtr);
						high16 = HIWORD(tmpPtr);
						asm	{
							mov	ax,high16
							push	ax
							mov	ax,low16			// load ptr.
							push	ax						// push it on stack.
						}			// asm...
						spOffset = spOffset + lenOfPtr;	// increment stack offset.
					}
				break;

			case CTYPE_LONG:				// 32 bits...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
						LongType = (long)dataBlockPtr->constVal;
						low16 = LOWORD(LongType);
						high16 = HIWORD(LongType);
					}
					else if (dataBlockPtr->byValue) {	// if so, pass contants of arg.
						LongType = (long)ONmbrGetInteger(dataBlockPtr->theArgObject);
						low16 = LOWORD(LongType);
						high16 = HIWORD(LongType);
					}
					else {	// pass by reference, arg may be modified by function.
						LongType = (long)ONmbrGetInteger(dataBlockPtr->theArgObject);
						tmpPtr = &LongType;
						low16 = LOWORD(tmpPtr);
						high16 = HIWORD(tmpPtr);
					}
					asm	{				// push on the stack.
						mov		ax,high16
						push		ax
						mov		ax,low16			// load ptr.
						push		ax								// push it on stack.
					}			// asm...
					spOffset = spOffset + LongTypeLen;	// increment stack offset.
				break;

			case CTYPE_ULONG:				// 32 bits...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
						ULongType = (unsigned long)dataBlockPtr->constVal;
						low16 = LOWORD(ULongType);
						high16 = HIWORD(ULongType);
					}
					else if (dataBlockPtr->byValue) {	// if so, pass contants of arg.
						ULongType = (unsigned long)ONmbrGetInteger(dataBlockPtr->theArgObject);
						low16 = LOWORD(ULongType);
						high16 = HIWORD(ULongType);
					}
					else {	// pass by reference, arg may be modified by function.
						ULongType = (unsigned long)ONmbrGetInteger(dataBlockPtr->theArgObject);
						tmpPtr = &ULongType;
						low16 = LOWORD(tmpPtr);
						high16 = HIWORD(tmpPtr);
					}
					asm	{
						mov	ax,high16
						push	ax
						mov	ax,low16			// load ptr.
						push	ax								// push it on stack.
					}			// asm...
					spOffset = spOffset +	ULongTypeLen;	// increment stack offset.
				break;

			case CTYPE_FLOAT:			// 32 bits...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
						FloatType = (float)dataBlockPtr->constVal;
						asm	{				// push on the stack.
							fld	 	FloatType			// move value into floating point register.
							sub		sp,4
							push		bp							// save bp on the stack.
        	    			mov		bp,sp
							fstp		dword ptr[bp+2]
							mov		sp,bp
							pop		bp								// restore bp from the stack.
						}
						spOffset = spOffset +	FloatTypeLen;	// increment stack offset.
					}
					else if (dataBlockPtr->byValue) {	// if so, pass contants of arg.
						ONmbrGetReal(dataBlockPtr->theArgObject, (long double*)&LongDoubleType);
						FloatType = (float) LongDoubleType;
						asm	{				// push on the stack.
							fld	 	FloatType			// move value into floating point register.
							sub		sp,4
							push		bp							// save bp on the stack.
        	    			mov		bp,sp
							fstp		dword ptr[bp+2]
							mov		sp,bp
							pop		bp								// restore bp from the stack.
						}
						spOffset = spOffset +	FloatTypeLen;	// increment stack offset.
					}
					else {	// pass by reference, arg may be modified by function.
						ONmbrGetReal(dataBlockPtr->theArgObject, (long double*)&LongDoubleType);
						FloatType = (float) LongDoubleType;
						tmpPtr = &FloatType;
						low16 = LOWORD(tmpPtr);
						high16 = HIWORD(tmpPtr);
						asm	{
							mov		ax,high16
							push		ax
							mov		ax,low16			// load ptr.
 							push		ax								// push it on stack.
						}			// asm...
						spOffset = spOffset +	lenOfPtr;	// increment stack offset.
					}
				break;

			case CTYPE_DOUBLE:			// 64 bits...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
						DoubleType = (double)dataBlockPtr->constVal;
						asm	{				// push on the stack.
							fld	 	DoubleType			// move value into floating point register.
							sub		sp,8
							push		bp							// save bp on the stack.
        	    			mov		bp,sp
							fstp		qword ptr[bp+2]
							mov		sp,bp
							pop		bp								// restore bp from the stack.
						}
						spOffset = spOffset + DoubleTypeLen;	// increment stack offset.
					}
					else if (dataBlockPtr->byValue) {	// if so, pass contants of arg.
						ONmbrGetReal(dataBlockPtr->theArgObject, (long double*)&LongDoubleType);
						DoubleType = (double) LongDoubleType;
						asm	{				// push on the stack.
							fld	 	DoubleType			// move value into floating point register.
							sub		sp,8
							push		bp							// save bp on the stack.
        	    			mov		bp,sp
							fstp		qword ptr[bp+2]
							mov		sp,bp
							pop		bp								// restore bp from the stack.
						}
						spOffset = spOffset + DoubleTypeLen;	// increment stack offset.
					}
					else {	// pass by reference, arg may be modified by function.
						ONmbrGetReal(dataBlockPtr->theArgObject, (long double*)&LongDoubleType);
						DoubleType = (double) LongDoubleType;
						tmpPtr = &DoubleType;
						low16 = LOWORD(tmpPtr);
						high16 = HIWORD(tmpPtr);
						asm	{
							mov		ax,high16
							push		ax
							mov		ax,low16			// load ptr.
							push		ax								// push it on stack.
						}			// asm...
						spOffset = spOffset +	lenOfPtr;	// increment stack offset.
					}
				break;

			case CTYPE_LDOUBLE:			// 80 bits...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
					LongDoubleType = (long double)dataBlockPtr->constVal;
					asm	{				// push on the stack.
							fld	 	LongDoubleType			// move value into floating point register.
							sub		sp,10
							push		bp							// save bp on the stack.
      					mov		bp,sp
							fstp		tbyte ptr[bp+2]
							mov		sp,bp
							pop		bp								// restore bp from the stack.
						}
						spOffset = spOffset + LongDoubleTypeLen;	// increment stack offset.
					}
					else if (dataBlockPtr->byValue) {	// if so, pass contants of arg.
						ONmbrGetReal(dataBlockPtr->theArgObject, (long double*)&LongDoubleType);
						asm	{				// push on the stack.
							fld	 	LongDoubleType			// move value into floating point register.
							sub		sp,10
							push		bp							// save bp on the stack.
							mov		bp,sp
							fstp		tbyte ptr[bp+2]
							mov		sp,bp
							pop		bp								// restore bp from the stack.
						}
						spOffset = spOffset + LongDoubleTypeLen;	// increment stack offset.
					}
					else {	// pass by reference, arg may be modified by function.
						ONmbrGetReal(dataBlockPtr->theArgObject, (long double*)&LongDoubleType);
						tmpPtr = &LongDoubleType;
						low16 = LOWORD(tmpPtr);
						high16 = HIWORD(tmpPtr);
						asm	{
							mov		ax,high16
							push		ax
							mov		ax,low16			// load ptr.
							push		ax						// push it on stack.
						}			// asm...
						spOffset = spOffset + lenOfPtr;	// increment stack offset.
					}
				break;

			case CTYPE_CHRFARPTR:
						objBlksId = AMemAllocate(0);
						objBlksIds[numberOfObjBlksIds] = objBlksId;	// save memblock id; we need to delete it.
						numberOfObjBlksIds++;			// inc number of block ids allocated.
						OTextGetToMemblock(dataBlockPtr->theArgObject, objBlksId);
						memBlkPtr = (char*)AMemLock(objBlksId);
						low16 = LOWORD(memBlkPtr);
						high16 = HIWORD(memBlkPtr);
						asm	{
							mov		ax,high16
							push		ax
							mov		ax,low16			// load ptr.
							push		ax						// push it on stack.
						}			// asm...
						spOffset = spOffset + lenOfPtr;	// increment stack offset.
				break;

			default:
				break;
		}			// switch (languageType)...
		AObjUnlockData(theObjectID, i);
	}			// for (i=0; numArgs...

	dataBlockResPtr = (DLL_RETARG*)AObjLockData(theObjectID, DLLRETPARAMINDEX);		// get return object.
	RESObjectID = dataBlockResPtr->theObject;
	retParamType = dataBlockResPtr->paramType;
	AObjUnlockData(theObjectID,DLLRETPARAMINDEX);

				// call the function and pop the return value off the stack.
		switch (retParamType) {
			case CTYPE_SHORT:							// 16 bits...            
				ShortType = ((C_SHORT)dllFunctEntryPnt)();				// call the function.
				asm add	sp,spOffset;				// clean up stack
				ONmbrSetInteger(RESObjectID, (long) ShortType, FALSE);
				break;

			case CTYPE_USHORT:						// 16 bits...
				UShortType = ((C_USHORT)dllFunctEntryPnt)();				// call the function.
				asm add	sp,spOffset;				// clean up stack
				ONmbrSetInteger(RESObjectID, (long) UShortType, FALSE);
				break;

			case CTYPE_LONG:							// 32 bits...
				LongType = ((C_LONG)dllFunctEntryPnt)();				// call the function.
				asm add	sp,spOffset;				// clean up stack
				ONmbrSetInteger(RESObjectID, (long) LongType, FALSE);
				break;

			case CTYPE_ULONG:							// 32 bits...
				ULongType = ((C_ULONG)dllFunctEntryPnt)();				// call the function.
				asm add	sp,spOffset;				// clean up stack
				ONmbrSetInteger(RESObjectID, (long) ULongType, FALSE);
				break;

			case CTYPE_FLOAT:							// 32 bits...
				FloatType = ((C_FLOAT)dllFunctEntryPnt)();				// call the function.
				asm add	sp,spOffset;				// clean up stack
				LongDoubleType = (long double) FloatType;
				ONmbrSetReal(RESObjectID, LongDoubleType, FALSE);
				break;

			case CTYPE_DOUBLE:						// 64 bits...
				DoubleType = ((C_DOUBLE)dllFunctEntryPnt)();				// call the function.
				asm add	sp,spOffset;				// clean up stack
				LongDoubleType = (long double)DoubleType;
				ONmbrSetReal(RESObjectID, LongDoubleType, FALSE);
				break;

			case CTYPE_LDOUBLE:						// 80 bits...
				LongDoubleType = ((C_LONGDOUBLE)dllFunctEntryPnt)();				// call the function.
				asm add	sp,spOffset;				// clean up stack
				ONmbrSetReal(RESObjectID, LongDoubleType, FALSE);
				break;

			case CTYPE_CHRFARPTR:
				retTextPtr = ((C_TEXTPTR)dllFunctEntryPnt)();
   			asm add	sp,spOffset;				// clean up stack
				for (i=0; i<numberOfObjBlksIds; ++i) {		// delete allocate blocks.
					AMemUnlock(objBlksIds[i]);
					AMemFree(objBlksIds[i]);
				}			// for (...
				OTextSetValueFromBuffer(RESObjectID, retTextPtr);
				break;

			default:
				break;
		}			// switch (languageType)...

	return retVal;
}			// callCFunc(...

 
/////////////////////////////////////////////////////////////////////////////
// FUNCTION:  callPascalFunc
// DESCRIPTION: Will push the arguments for the DLL function call on the
//			stack, it will call the DLL function and will return after its
//			completation. Arguments in Pascal are pushed from left to right
//			like the rest of the world.  In Pascal we need not clean up the
//			stack, since the calling function is responsible for doing so.
//
//	Return value:
//			TRUE				funtion was called.
//			FALSE				function could not be called  
/////////////////////////////////////////////////////////////////////////////

BOOL callPascalFunc()
{
	short					ShortType = 0;
	unsigned short		UShortType = 0;
	long					LongType = 0;
	unsigned long		ULongType = 0;
	float					FloatType = 0;
	double				DoubleType = 0;
	long double			LongDoubleType = 0;

	LPVOID				tmpPtr = NULL;
	int					lenOfPtr = sizeof(tmpPtr);				// length of ptr for offset, in pass by referance.
	WORD					low16 = 0;
	WORD					high16 = 0;
	AOBJECTID				RESObjectID = 0;
	AMEMBLOCKID			objBlksId;			// memblock id for text object.
	AMEMBLOCKID			objBlksIds[MAXNUMOFARGS];	// memblock id for text object, we need to junk them.
	int					numberOfObjBlksIds = 0;			// number of block ids allocated.
	char*					memBlkPtr;
	LPSTR					retTextPtr;			// return ptr to text string.

	int	ShortTypeLen = sizeof(ShortType);
	int	UShortTypeLen = sizeof(UShortType);
	int	LongTypeLen = sizeof(LongType);
	int	ULongTypeLen = sizeof(ULongType);
	int	FloatTypeLen = sizeof(FloatType);
	int	DoubleTypeLen = sizeof(DoubleType);
	int	LongDoubleTypeLen = sizeof(LongDoubleType);
	int	spOffset = 0;
	int	retParamType;

	DLL_ARGUMENTS*	dataBlockPtr = NULL;
	DLL_RETARG*			dataBlockResPtr = NULL;
	BOOL		retVal = TRUE;				// assume that function will make it.
	int i = 0;

	for (i=(int)DLLPARAMINDEX; i<numArgs; ++i) {		// copy data blocks to temp area.
		dataBlockPtr = (DLL_ARGUMENTS*)AObjLockData(theObjectID, i);
		switch (dataBlockPtr->paramType) {
			case CTYPE_SHORT:				// 16 bits...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
						ShortType = (short)dataBlockPtr->constVal;
						asm	{
							mov	ax,ShortType					// load the constant.
							push	ax										// push it on stack.
						}			// asm...
						spOffset = spOffset + ShortTypeLen;	// increment stack offset.
					}
					else if (dataBlockPtr->byValue) {	// if so, pass contants of arg.
						ShortType = (short)ONmbrGetInteger(dataBlockPtr->theArgObject);
						asm	{
							mov	ax,ShortType					// load the constant.
							push	ax										// push it on stack.
						}			// asm...
						spOffset = spOffset + ShortTypeLen;	// increment stack offset.
					}
					else {	// pass by reference, arg may be modified by function.
						ShortType = (short)ONmbrGetInteger(dataBlockPtr->theArgObject);
						tmpPtr = &ShortType;
						low16 = LOWORD(tmpPtr);
						high16 = HIWORD(tmpPtr);
						asm	{
							mov	ax,high16
							push	ax
							mov	ax,low16			// load ptr.
							push	ax								// push it on stack.
						}			// asm...
						spOffset = spOffset +	lenOfPtr;	// increment stack offset.
					}
				break;

			case CTYPE_USHORT:			// 16 bits...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
						UShortType = (unsigned short)dataBlockPtr->constVal;
						asm	{
							mov	ax,UShortType					// load the constant.
							push	ax										// push it on stack.
						}			// asm...
						spOffset = spOffset + UShortTypeLen;	// increment stack offset.
					}
					else if (dataBlockPtr->byValue) {	// if so, pass contants of arg.
						UShortType = (short)ONmbrGetInteger(dataBlockPtr->theArgObject);
						asm	{
							mov	ax,UShortType					// load the constant.
							push	ax										// push it on stack.
						}			// asm...
						spOffset = spOffset + UShortTypeLen;	// increment stack offset.
					}
					else {	// pass by reference, arg may be modified by function.
						UShortType = (short)ONmbrGetInteger(dataBlockPtr->theArgObject);
						tmpPtr = &UShortType;
						low16 = LOWORD(tmpPtr);
						high16 = HIWORD(tmpPtr);
						asm	{
							mov	ax,high16
							push	ax
							mov	ax,low16			// load ptr.
							push	ax						// push it on stack.
						}			// asm...
						spOffset = spOffset + lenOfPtr;	// increment stack offset.
					}
				break;

			case CTYPE_LONG:				// 32 bits...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
						LongType = (long)dataBlockPtr->constVal;
						low16 = LOWORD(LongType);
						high16 = HIWORD(LongType);
					}
					else if (dataBlockPtr->byValue) {	// if so, pass contants of arg.
						LongType = (long)ONmbrGetInteger(dataBlockPtr->theArgObject);
						low16 = LOWORD(LongType);
						high16 = HIWORD(LongType);
					}
					else {	// pass by reference, arg may be modified by function.
						LongType = (long)ONmbrGetInteger(dataBlockPtr->theArgObject);
						tmpPtr = &LongType;
						low16 = LOWORD(tmpPtr);
						high16 = HIWORD(tmpPtr);
					}
					asm	{				// push on the stack.
						mov		ax,high16
						push	ax
						mov		ax,low16			// load ptr.
						push	ax								// push it on stack.
					}			// asm...
					spOffset = spOffset + LongTypeLen;	// increment stack offset.
				break;

			case CTYPE_ULONG:				// 32 bits...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
						ULongType = (unsigned long)dataBlockPtr->constVal;
						low16 = LOWORD(ULongType);
						high16 = HIWORD(ULongType);
					}
					else if (dataBlockPtr->byValue) {	// if so, pass contants of arg.
						ULongType = (unsigned long)ONmbrGetInteger(dataBlockPtr->theArgObject);
						low16 = LOWORD(ULongType);
						high16 = HIWORD(ULongType);
					}
					else {	// pass by reference, arg may be modified by function.
						ULongType = (unsigned long)ONmbrGetInteger(dataBlockPtr->theArgObject);
						tmpPtr = &ULongType;
						low16 = LOWORD(tmpPtr);
						high16 = HIWORD(tmpPtr);
					}
					asm	{
						mov	ax,high16
						push	ax
						mov	ax,low16			// load ptr.
						push	ax								// push it on stack.
					}			// asm...
					spOffset = spOffset +	ULongTypeLen;	// increment stack offset.
				break;

			case CTYPE_FLOAT:			// 32 bits...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
						FloatType = (float)dataBlockPtr->constVal;
						asm	{				// push on the stack.
							fld	 FloatType			// move value into floating point register.
							sub		sp,4
							push	bp							// save bp on the stack.
        	    mov		bp,sp
							fstp	dword ptr[bp+2]
							mov		sp,bp
							pop		bp								// restore bp from the stack.
						}
						spOffset = spOffset +	FloatTypeLen;	// increment stack offset.
					}
					else if (dataBlockPtr->byValue) {	// if so, pass contants of arg.
						ONmbrGetReal(dataBlockPtr->theArgObject, (long double*)&LongDoubleType);
						FloatType = (float) LongDoubleType;
						asm	{				// push on the stack.
							fld	 FloatType			// move value into floating point register.
							sub		sp,4
							push	bp							// save bp on the stack.
        	    mov		bp,sp
							fstp	dword ptr[bp+2]
							mov		sp,bp
							pop		bp								// restore bp from the stack.
						}
						spOffset = spOffset +	FloatTypeLen;	// increment stack offset.
					}
					else {	// pass by reference, arg may be modified by function.
						ONmbrGetReal(dataBlockPtr->theArgObject, (long double*)&LongDoubleType);
						FloatType = (float) LongDoubleType;
						tmpPtr = &FloatType;
						low16 = LOWORD(tmpPtr);
						high16 = HIWORD(tmpPtr);
						asm	{
							mov	ax,high16
							push	ax
							mov		ax,low16			// load ptr.
 							push	ax								// push it on stack.
						}			// asm...
						spOffset = spOffset +	lenOfPtr;	// increment stack offset.
					}
				break;

			case CTYPE_DOUBLE:			// 64 bits...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
						DoubleType = (double)dataBlockPtr->constVal;
						asm	{				// push on the stack.
							fld	 	DoubleType			// move value into floating point register.
							sub		sp,8
							push	bp							// save bp on the stack.
        	    mov		bp,sp
							fstp	qword ptr[bp+2]
							mov		sp,bp
							pop		bp								// restore bp from the stack.
						}
						spOffset = spOffset + DoubleTypeLen;	// increment stack offset.
					}
					else if (dataBlockPtr->byValue) {	// if so, pass contants of arg.
						ONmbrGetReal(dataBlockPtr->theArgObject, (long double*)&LongDoubleType);
						DoubleType = (double) LongDoubleType;
						asm	{				// push on the stack.
							fld	 	DoubleType			// move value into floating point register.
							sub		sp,8
							push	bp							// save bp on the stack.
        	    mov		bp,sp
							fstp	qword ptr[bp+2]
							mov		sp,bp
							pop		bp								// restore bp from the stack.
						}
						spOffset = spOffset + DoubleTypeLen;	// increment stack offset.
					}
					else {	// pass by reference, arg may be modified by function.
						ONmbrGetReal(dataBlockPtr->theArgObject, (long double*)&LongDoubleType);
						DoubleType = (double) LongDoubleType;
						tmpPtr = &DoubleType;
						low16 = LOWORD(tmpPtr);
						high16 = HIWORD(tmpPtr);
						asm	{
							mov		ax,high16
							push	ax
							mov		ax,low16			// load ptr.
							push	ax								// push it on stack.
						}			// asm...
						spOffset = spOffset +	lenOfPtr;	// increment stack offset.
					}
				break;

			case CTYPE_LDOUBLE:			// 80 bits...
					if (dataBlockPtr->constant) {		// if so then pass the constant.
					LongDoubleType = (long double)dataBlockPtr->constVal;
					asm	{				// push on the stack.
							fld	 	LongDoubleType			// move value into floating point register.
							sub		sp,10
							push	bp							// save bp on the stack.
        	    mov		bp,sp
							fstp	tbyte ptr[bp+2]
							mov		sp,bp
							pop		bp								// restore bp from the stack.
						}
						spOffset = spOffset + LongDoubleTypeLen;	// increment stack offset.
					}
					else if (dataBlockPtr->byValue) {	// if so, pass contants of arg.
						ONmbrGetReal(dataBlockPtr->theArgObject, (long double*)&LongDoubleType);
						asm	{				// push on the stack.
							fld	 	LongDoubleType			// move value into floating point register.
							sub		sp,10
							push	bp							// save bp on the stack.
        	    mov		bp,sp
							fstp	tbyte ptr[bp+2]
							mov		sp,bp
							pop		bp								// restore bp from the stack.
						}
						spOffset = spOffset + LongDoubleTypeLen;	// increment stack offset.
					}
					else {	// pass by reference, arg may be modified by function.
						ONmbrGetReal(dataBlockPtr->theArgObject, (long double*)&LongDoubleType);
						tmpPtr = &LongDoubleType;
						low16 = LOWORD(tmpPtr);
						high16 = HIWORD(tmpPtr);
						asm	{
							mov		ax,high16
							push	ax
							mov		ax,low16			// load ptr.
							push	ax						// push it on stack.
						}			// asm...
						spOffset = spOffset + lenOfPtr;	// increment stack offset.
					}
				break;

			case CTYPE_CHRFARPTR:
						objBlksId = AMemAllocate(0);
						objBlksIds[numberOfObjBlksIds] = objBlksId;	// save memblock id; we need to delete it.
						numberOfObjBlksIds++;			// inc number of block ids allocated.
						OTextGetToMemblock(dataBlockPtr->theArgObject, objBlksId);
						memBlkPtr = (char*)AMemLock(objBlksId);
						low16 = LOWORD(memBlkPtr);
						high16 = HIWORD(memBlkPtr);
						asm	{
							mov		ax,high16
							push	ax
							mov		ax,low16			// load ptr.
							push	ax						// push it on stack.
						}			// asm...
						spOffset = spOffset + lenOfPtr;	// increment stack offset.
				break;

			default:
				break;
		}			// switch (languageType)...
		AObjUnlockData(theObjectID, i);
	}			// for (i=0; numArgs...

	dataBlockResPtr = (DLL_RETARG*)AObjLockData(theObjectID, DLLRETPARAMINDEX);		// get return onject ID.
	RESObjectID = dataBlockResPtr->theObject;
  retParamType = dataBlockResPtr->paramType;
	AObjUnlockData(theObjectID,DLLRETPARAMINDEX);

				// call the function and pop the return value off the stack.
		switch (retParamType) {
			case CTYPE_SHORT:							// 16 bits...            
				ShortType = ((P_SHORT)dllFunctEntryPnt)();				// call the function.
//				asm add	sp,spOffset;				// clean up stack
				ONmbrSetInteger(RESObjectID, (long) ShortType, FALSE);
				break;

			case CTYPE_USHORT:						// 16 bits...
				UShortType = ((P_USHORT)dllFunctEntryPnt)();				// call the function.
//				asm add	sp,spOffset;				// clean up stack
				ONmbrSetInteger(RESObjectID, (long) UShortType, FALSE);
				break;

			case CTYPE_LONG:							// 32 bits...
				LongType = ((P_LONG)dllFunctEntryPnt)();				// call the function.
//				asm add	sp,spOffset;				// clean up stack
				ONmbrSetInteger(RESObjectID, (long) LongType, FALSE);
				break;

			case CTYPE_ULONG:							// 32 bits...
				ULongType = ((P_ULONG)dllFunctEntryPnt)();				// call the function.
//				asm add	sp,spOffset;				// clean up stack
				ONmbrSetInteger(RESObjectID, (long) ULongType, FALSE);
				break;

			case CTYPE_FLOAT:							// 32 bits...
				FloatType = ((P_FLOAT)dllFunctEntryPnt)();				// call the function.
//				asm add	sp,spOffset;				// clean up stack
				LongDoubleType = (long double) FloatType;
				ONmbrSetReal(RESObjectID, LongDoubleType, FALSE);
				break;

			case CTYPE_DOUBLE:						// 64 bits...
				DoubleType = ((P_DOUBLE)dllFunctEntryPnt)();				// call the function.
//				asm add	sp,spOffset;				// clean up stack
				LongDoubleType = (long double)DoubleType;
				ONmbrSetReal(RESObjectID, LongDoubleType, FALSE);
				break;

			case CTYPE_LDOUBLE:						// 80 bits...
				LongDoubleType = ((P_LONGDOUBLE)dllFunctEntryPnt)();				// call the function.
//				asm add	sp,spOffset;				// clean up stack
				ONmbrSetReal(RESObjectID, LongDoubleType, FALSE);
				break;

			case CTYPE_CHRFARPTR:
				retTextPtr = ((P_TEXTPTR)dllFunctEntryPnt)();
//   			asm add	sp,spOffset;				// clean up stack
				for (i=0; i<numberOfObjBlksIds; ++i) {		// delete allocate blocks.
					AMemUnlock(objBlksIds[i]);
					AMemFree(objBlksIds[i]);
				}			// for (...
				OTextSetValueFromBuffer(RESObjectID, retTextPtr);
				break;

			default:
				break;
		}			// switch (languageType)...

	return retVal;
}			// callPascalFunc(...

/////////////////////////////////////////


