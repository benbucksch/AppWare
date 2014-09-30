#include "n_includ.h"


////////////////////////////////////

#pragma argsused
extern "C" BOOL    ONmbr_lAdd(
	long 										value1, 
	long 										value2, 
	LPLONG 									result)
	{
	*result = value1 + value2;

	/*asm{
		mov	ax, value1
		mov	ebx, value2
		add	eax, ebx
	 	jo OVERFLOW
		les	bx,dword ptr [bp+6]
		mov	dword ptr es:[bx], eax
		}*/

	return TRUE;

/*OVERFLOW: 
	return FALSE;  	*/
	}

////////////////////////////////////

#pragma argsused
extern "C" BOOL    ONmbr_lSub(
	long 										value1, 
	long 										value2, 
	LPLONG 									result)
	{
	*result = value1 - value2;

	/*asm {
		mov	eax, value1
		mov	ebx, value2
		sub	eax, ebx
		jo OVERFLOW
		les	bx,dword ptr [bp+6]
		mov	dword ptr es:[bx], eax
		}		 */

	return TRUE;

/*OVERFLOW: 
	return FALSE;*/
	}

////////////////////////////////////

#pragma argsused
extern "C" BOOL    ONmbr_lMul(
	long 										value1, 
	long 										value2, 
	LPLONG 									result)
	{

	*result = value1 * value2;

/*	asm {
		mov	eax, value1
		mov	ebx, value2
		imul	ebx
		jo OVERFLOW
		les	bx,dword ptr [bp+6]
		mov	dword ptr es:[bx], eax
		}				 */

	return TRUE;

/*OVERFLOW: 
	return FALSE;*/
	}

////////////////////////////////////

#pragma argsused
extern "C" BOOL   ONmbr_lInc(
	LPLONG									value)
	{

	(*value)++;

	/*asm {
		les	bx,dword ptr [bp+6]
		mov	eax,dword ptr es:[bx]
		inc eax
		jo OVERFLOW
		mov	dword ptr es:[bx], eax
		}*/

	return TRUE;

/*OVERFLOW: 
	return FALSE;*/
	}

////////////////////////////////////

#pragma argsused
extern "C" BOOL   ONmbr_lDec(
	LPLONG									value)
	{
	(*value)--;

	/*asm {
		les	bx,dword ptr [bp+6]
		mov	eax, dword ptr es:[bx]
		dec eax
		jo OVERFLOW
		mov	dword ptr es:[bx], eax
		}*/

	return TRUE;

/*OVERFLOW: 
	return FALSE;*/
	}
