

#include "n_includ.h"


short ALMAPI ONmbrFormatInteger(      /* Returns the number of bytes copied into the buffer not including the null terminator */
	OBJECTID								oiNumber, 						/*if zero, uses the default whole number format */
	LONG										lnValue,
	LPSTR										szBuffer,
	short										cbMax)
	{
 	LPNUMBER								num;
	short										rVal;

	if (oiNumber)
  	{
		I_ASSERT_TYPEID(oiNumber, OTYPE_NUMBER, "ONmbrFormatInteger", return 0);
             
		num = LockNumberObject(oiNumber);
		I_ASSERT_NUM(num, "ONmbrFormatInteger", return 0);
		rVal = FormatFloatNumber( (long double)lnValue, num->GetFormatPtr(), szBuffer, cbMax, num->GetFormatInfoPtr());
		UnlockNumberObject(oiNumber);
		}
	else
		{
		rVal = FormatFloatNumber( (long double)lnValue, g_szIntegerNumberFormat, szBuffer, cbMax, &g_integerNumberFormatInfo);
    }

	return rVal;
	}

//////////////////////////////////////////////////////////

short ALMAPI ONmbrFormatReal(	/* Returns the number of bytes copied into the szBuffer not including the null terminator */
	OBJECTID								oiNumber, 		/*if zero, uses the default whole number format */
	long double							flValue,
	LPSTR										szBuffer,
	short										cbMax)
	{
	LPNUMBER								num;
  short										rVal;

	if (oiNumber)
		{           
		I_ASSERT_TYPEID(oiNumber, OTYPE_NUMBER, "ONmbrFormatReal", return 0);

		num = LockNumberObject(oiNumber);
		I_ASSERT_NUM(num, "ONmbrFormatReal", return 0);
		rVal = FormatFloatNumber(flValue, num->GetFormatPtr(), szBuffer, cbMax, num->GetFormatInfoPtr());
		UnlockNumberObject(oiNumber);
		}
	else
		rVal = FormatFloatNumber(flValue, g_szRealNumberFormat, szBuffer, cbMax, &g_realNumberFormatInfo);

	return rVal;
	}

////////////////

long ALMAPI ONmbrParseFormatedNumber(
	OBJECTID 								oiNumber, //the object with the format string
	long double far *				lpValue,  //the value
	LPCSTR 									szBuffer) //the buffer to parse
	{
	LPNUMBER								num;
  long										rVal=0L;

	if (oiNumber)
		{           
		I_ASSERT_TYPEID(oiNumber, OTYPE_NUMBER, "ONmbrParseFormatedNumber", return rVal);

		num = LockNumberObject(oiNumber);
		I_ASSERT_NUM(num, "ONmbrParseFormatedNumber", return rVal);
		*lpValue = ParseFormatedNumber(szBuffer, num->GetFormatPtr(), num->GetFormatInfoPtr());
		UnlockNumberObject(oiNumber);
		}
	else
		*lpValue = ParseFormatedNumber(szBuffer, g_szRealNumberFormat, &g_realNumberFormatInfo);

	if (DoesStringContainRealNumberThingies(szBuffer)  ||  *lpValue > I_LARGEST_LONG  ||  *lpValue < I_SMALLEST_LONG)
		rVal = ONMBR_REAL;
	else
		rVal = ONMBR_INTEGER;

	return rVal;
	}

//////////////////////////////////////////////////////////

short ALMAPI ONmbrConvertToString( /* Returns the number of bytes copied into the buffer not including the null terminator */
	OBJECTID								oiNumber,
	LPSTR										szBuffer,
	short										cbMax)
	{
	LPNUMBER								num;
  short										rVal;

	I_ASSERT_TYPEID(oiNumber, OTYPE_NUMBER, "ONmbrConvertToString", return 0);

	num = LockNumberObject(oiNumber);
	I_ASSERT_NUM(num, "ONmbrConvertToText", return 0);

	num->FormatNumber();
	lstrcpyn(szBuffer, num->GetStringPtr(), cbMax);
	szBuffer[cbMax-1] = '\0';
	rVal = lstrlen(szBuffer);

	UnlockNumberObject(oiNumber);

  return rVal;
	}

//////////////////////////////////////////////////////////////

long ALMAPI ONmbrConvertFromString(				/*returns ONMBR_RANGE_ERR, ONMBR_HAS_CHANGED*/
	OBJECTID								oiNumber,
	LPSTR										szBuffer,
	BOOL										bChangeType) 		/*Changes the type if it is a pseudo object*/
	{
	LPNUMBER								num;
  long										rVal;

	I_ASSERT_TYPEID(oiNumber, OTYPE_NUMBER, "ONmbrConvertFromText", return 0);

	num = LockNumberObject(oiNumber);
	I_ASSERT_NUM(num, "ONmbrConvertFromText", return 0);

	num->SetValue(szBuffer, 
								POST_SIGNALS, 
								bChangeType ? CHANGE_IF_PSEUDO  : DONT_CHANGE_IF_PSEUDO, 
								BLANK_IF_EMPTY_STRING);

	rVal = num->GetStatus();

  UnlockNumberObject(oiNumber);

	return rVal;
	}

//////////////////////////////////////////////////////////////

long ALMAPI ONmbrGetInteger(           /* returns the integer value*/
	OBJECTID  							oiNumber)
	{
	LPNUMBER								num;
  long										rVal;

	I_ASSERT_TYPEID(oiNumber, OTYPE_NUMBER, "ONmbrGetInteger", return 0);

	num = LockNumberObject(oiNumber);
	I_ASSERT_NUM(num, "ONmbrGetInteger", return 0);

  rVal = num->GetWholeValue();

	UnlockNumberObject(oiNumber);

	return rVal;
	}

////////////////////////////////////////////////////////////////

void ALMAPI ONmbrGetReal(              /* returns the real value*/
	OBJECTID  							oiNumber,
	long double*						pValue)
	{
	LPNUMBER								num;

	I_ASSERT_TYPEID(oiNumber, OTYPE_NUMBER, "ONmbrGetReal", if (pValue) *pValue=0; return);
	ADBG_ASSERT_E(pValue, return, "Invalid pointer passed to ONmbrGetReal");

	if (pValue != NULL)
		{
		num = LockNumberObject(oiNumber);
		I_ASSERT_NUM(num, "ONmbrGetReal", *pValue = 0; return);

		*pValue = num->GetFloatValue();

		UnlockNumberObject(oiNumber);
		}
	}

///////////////////////////////////////////////////////

long ALMAPI ONmbrSetInteger(           /* sets the value -- returns the status*/
	OBJECTID								oiNumber,
	long										lnValue,
	BOOL										bChangeType) /*Changes the type to integer if it is a pseudo object*/
	{
	LPNUMBER								num;
	long										rVal;

	I_ASSERT_TYPEID(oiNumber, OTYPE_NUMBER, "ONmbrSetInteger", return 0);

	num = LockNumberObject(oiNumber);
	I_ASSERT_NUM(num, "ONmbrSetInteger", return 0);

	if (bChangeType && num->IsTempObject())
		num->SetIsInteger();

	num->SetValue( (long double)lnValue, POST_SIGNALS, DONT_ROUND_IF_INT);

  rVal = num->GetStatus();

	UnlockNumberObject(oiNumber);
  return rVal;
	}

///////////////////////////////////////////////////////

long ALMAPI ONmbrSetReal(              /* sets the value - returns the status*/
	OBJECTID								oiNumber,
	long double							flValue,
	BOOL										bChangeType)  /*Changes the type to real if it is a pseudo object*/
  {
	LPNUMBER								num;
  long										rVal;

	I_ASSERT_TYPEID(oiNumber, OTYPE_NUMBER, "ONmbrSetReal", return 0);

	num = LockNumberObject(oiNumber);
	I_ASSERT_NUM(num, "ONmbrSetReal", return 0);

	if (bChangeType && num->IsTempObject())
		num->SetIsReal();

	num->SetValue(flValue, POST_SIGNALS, ROUND_IF_INT);

  rVal = num->GetStatus();

	UnlockNumberObject(oiNumber);
	return rVal;
	}


/* ****************************************************************
 * Miscellaneous
 * **************************************************************** */

long ALMAPI ONmbrGetType(   /* returns the type*/
	OBJECTID								oiNumber)
	{
	LPNUMBER								num;
	long										rVal;

	I_ASSERT_TYPEID(oiNumber, OTYPE_NUMBER, "ONmbrGetType", return 0);

	num = LockNumberObject(oiNumber);
	I_ASSERT_NUM(num, "ONmbrGetType", return 0);

	rVal = num->GetType();

	UnlockNumberObject(oiNumber);
	return rVal;
	}

///////////////////////////////////

long ALMAPI ONmbrSetType( /* returns the type*/
	OBJECTID								oiNumber, /*Only sets the type for pseudos*/
	LONG										theType)
	{
	LPNUMBER								num;
	long										rVal;

	I_ASSERT_TYPEID(oiNumber, OTYPE_NUMBER, "ONmbrGetType", return 0);

	num = LockNumberObject(oiNumber);
	I_ASSERT_NUM(num, "ONmbrGetType", return 0);

	if (num->IsTempObject())
		{
		if (theType == ONMBR_INTEGER)
			num->SetIsInteger();
		else if (theType == ONMBR_REAL)
    	num->SetIsReal();
		}
	else
		ADBG_EVAL(ADbgPrintRes(ADBG_WARNING, 0, g_hInstance, WARN_CANNOT_CHANGE_TYPE));

  rVal = num->GetStatus();

	UnlockNumberObject(oiNumber);
	return rVal;
	}

////////////////////////////////////

long ALMAPI ONmbrGetStatus( /* overflow, status*/
	OBJECTID								oiNumber)
	{
	LPNUMBER								num;
	long										rVal = 0L;

	I_ASSERT_TYPEID(oiNumber, OTYPE_NUMBER, "ONmbrGetStatus", return 0);

	num = LockNumberObject(oiNumber);
	I_ASSERT_NUM(num, "ONmbrGetType", return 0);

  rVal = num->GetStatus();

	UnlockNumberObject(oiNumber);

  return rVal;
	}








