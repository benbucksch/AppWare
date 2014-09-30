#include "stdlib.h"
#include "n_includ.h"

#define OTYPE_ANYTHING		0

//////////////////////////////////////////////////////////////

extern "C" void ALMAPI NmbrFunction_Add(
	pAEvtInfo								theEvent,
	pAObjMessage						theSystem)
	{
	AOBJECTID          			oiNum1, oiNum2, oiOut;
	LPNUMBER								num1, num2, numOut;
	long										lResult;

  I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiNum1 = AFuncGetTypedParameter(1, OTYPE_NUMBER);
	I_ASSERT_PARAMETER(oiNum1, 1, "NmbrFunction_Add", return);

	oiNum2 = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	I_ASSERT_PARAMETER(oiNum2, 2, "NmbrFunction_Add", return);

	oiOut = AObjCreate(OTYPE_NUMBER);
	I_ASSERT_PSUEDO(oiOut, "NmbrFunction_Add", return);

	num1 = LockNumberObject(oiNum1);
	I_ASSERT_NUM(num1, "NmbrFunction_Add", return);

	num2 = LockNumberObject(oiNum2);
	I_ASSERT_NUM(num2, "NmbrFunction_Add", return);

	numOut = LockNumberObject(oiOut);
	I_ASSERT_NUM(numOut, "NmbrFunction_Add", return);

	if (num1->HadRangeError()  ||  num2->HadRangeError() )
		numOut->SetRangeError(TRUE, DONT_POST_SIGNALS);
	else if (num1->IsInteger() && num2->IsInteger())
		{
		numOut->SetIsInteger();
		if (ONmbr_lAdd(num1->GetWholeValue(), num2->GetWholeValue(),	&lResult))
			numOut->SetValue(lResult, DONT_POST_SIGNALS); 
		else
			numOut->SetRangeError(TRUE, DONT_POST_SIGNALS);
		}
	else
		{
		numOut->SetIsReal();
		numOut->SetValue(num1->GetFloatValue() + num2->GetFloatValue(), DONT_POST_SIGNALS, DONT_ROUND_IF_INT);
		}

	UnlockNumberObject(oiNum1);
	UnlockNumberObject(oiNum2);
	UnlockNumberObject(oiOut);

	AFuncSetParameter(1, oiOut);
	}

/////////////////////////////////////////

extern "C" void ALMAPI NmbrFunction_Sub(
	pAEvtInfo		theEvent,
	pAObjMessage					theSystem)
	{
	AOBJECTID          			oiNum1, oiNum2, oiOut;
	LPNUMBER								num1, num2, numOut;
	long										lResult;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiNum1 = AFuncGetTypedParameter(1, OTYPE_NUMBER);
	I_ASSERT_PARAMETER(oiNum1, 1, "NmbrFunction_Sub", return);

	oiNum2 = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	I_ASSERT_PARAMETER(oiNum2, 2, "NmbrFunction_Sub", return);

	oiOut = AObjCreate(OTYPE_NUMBER);
	I_ASSERT_PSUEDO(oiOut, "NmbrFunction_Sub", return);

	num1 = LockNumberObject(oiNum1);
	I_ASSERT_NUM(num1, "NmbrFunction_Sub", return);

	num2 = LockNumberObject(oiNum2);
	I_ASSERT_NUM(num2, "NmbrFunction_Sub", return);

	numOut = LockNumberObject(oiOut);
	I_ASSERT_NUM(numOut, "NmbrFunction_Sub", return);

	if (num1->HadRangeError()  ||  num2->HadRangeError() )
		numOut->SetRangeError(TRUE, DONT_POST_SIGNALS);
	else if (num1->IsInteger() && num2->IsInteger())
		{
		numOut->SetIsInteger();
	 	if (ONmbr_lSub(num1->GetWholeValue(), num2->GetWholeValue(),	&lResult))
			numOut->SetValue(lResult, DONT_POST_SIGNALS);
		else
			numOut->SetRangeError(TRUE, DONT_POST_SIGNALS);
		}
	else
		{
		numOut->SetIsReal();
		numOut->SetValue(num1->GetFloatValue() - num2->GetFloatValue(), DONT_POST_SIGNALS, DONT_ROUND_IF_INT);
		}

	UnlockNumberObject(oiNum1);
	UnlockNumberObject(oiNum2);
	UnlockNumberObject(oiOut);

	AFuncSetParameter(1, oiOut);
	}

/////////////////////////////////////////


extern "C" void ALMAPI NmbrFunction_Mult(
	pAEvtInfo		theEvent,
	pAObjMessage					theSystem)
	{
	AOBJECTID          			oiNum1, oiNum2, oiOut;
	LPNUMBER								num1, num2, numOut;
	long										lResult;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiNum1 = AFuncGetTypedParameter(1, OTYPE_NUMBER);
	I_ASSERT_PARAMETER(oiNum1, 1, "NmbrFunction_Mult", return);

	oiNum2 = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	I_ASSERT_PARAMETER(oiNum2, 2, "NmbrFunction_Mult", return);

	oiOut = AObjCreate(OTYPE_NUMBER);
	I_ASSERT_PSUEDO(oiOut, "NmbrFunction_Mult", return);

	num1 = LockNumberObject(oiNum1);
	I_ASSERT_NUM(num1, "NmbrFunction_Mult", return);

	num2 = LockNumberObject(oiNum2);
	I_ASSERT_NUM(num2, "NmbrFunction_Mult", return);

	numOut = LockNumberObject(oiOut);
	I_ASSERT_NUM(numOut, "NmbrFunction_Mult", return);

	if (num1->HadRangeError()  ||  num2->HadRangeError() )
		numOut->SetRangeError(TRUE, DONT_POST_SIGNALS);
	else if (num1->IsInteger() && num2->IsInteger())
		{
		numOut->SetIsInteger();
		if (ONmbr_lMul(num1->GetWholeValue(), num2->GetWholeValue(),	&lResult))
			numOut->SetValue(lResult, DONT_POST_SIGNALS);
		else
			numOut->SetRangeError(TRUE, DONT_POST_SIGNALS);
		}
	else
		{
		numOut->SetIsReal();
		numOut->SetValue(num1->GetFloatValue() * num2->GetFloatValue(), DONT_POST_SIGNALS, DONT_ROUND_IF_INT);
		}

	UnlockNumberObject(oiNum1);
	UnlockNumberObject(oiNum2);
	UnlockNumberObject(oiOut);

	AFuncSetParameter(1, oiOut);
	}

/////////////////////////////////////////


extern "C" void ALMAPI NmbrFunction_Div(
	pAEvtInfo								theEvent,
	pAObjMessage						theSystem)
	{
	AOBJECTID          			oiNum1, oiNum2, oiOut;
	LPNUMBER								num1, num2, numOut;
	long double							fl;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiNum1 = AFuncGetTypedParameter(1, OTYPE_NUMBER);
	I_ASSERT_PARAMETER(oiNum1, 1, "NmbrFunction_Div", return);

	oiNum2 = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	I_ASSERT_PARAMETER(oiNum2, 2, "NmbrFunction_Div", return);

	oiOut = AObjCreate(OTYPE_NUMBER);
	I_ASSERT_PSUEDO(oiOut, "NmbrFunction_Div", return);

	num1 = LockNumberObject(oiNum1);
	I_ASSERT_NUM(num1, "NmbrFunction_Div", return);

	num2 = LockNumberObject(oiNum2);
	I_ASSERT_NUM(num2, "NmbrFunction_Div", return);

	numOut = LockNumberObject(oiOut);
	I_ASSERT_NUM(numOut, "NmbrFunction_Div", return);

	if (num1->HadRangeError()  ||  num2->HadRangeError() )
		numOut->SetRangeError(TRUE, DONT_POST_SIGNALS);
	else if (num2->GetFloatValue() != 0.0)
		{
		fl = num1->GetFloatValue() / num2->GetFloatValue();

		if (num1->IsInteger() && num2->IsInteger())
			{
			numOut->SetIsInteger();
			fl = (fl>0.0 ? floorl(fl) : ceill(fl));  //drop the decimal part
			if (fl < I_LARGEST_LONG  &&  fl >I_SMALLEST_LONG)
				numOut->SetValue( (long)fl, DONT_POST_SIGNALS);
			else
				numOut->SetRangeError(TRUE, DONT_POST_SIGNALS);
			}
		else //atleast one float number
			{
			numOut->SetIsReal();
			numOut->SetValue(fl, DONT_POST_SIGNALS, DONT_ROUND_IF_INT);
			}
		}
	else
		{	//Divide by zero
		numOut->SetRangeError(TRUE, DONT_POST_SIGNALS);
		}


	UnlockNumberObject(oiNum1);
	UnlockNumberObject(oiNum2);
	UnlockNumberObject(oiOut);

	AFuncSetParameter(1, oiOut);
	}

/////////////////////////////////////////


extern "C" void ALMAPI NmbrFunction_Inc(
	pAEvtInfo								theEvent,
	pAObjMessage						theSystem)
	{
	AOBJECTID          			oiNum;
	LPNUMBER								num;	 
	LONG										lValue;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiNum = AFuncGetTypedParameter(1, OTYPE_NUMBER);
	I_ASSERT_PARAMETER(oiNum, 1, "NmbrFunction_Div", return);

	num = LockNumberObject(oiNum);
	I_ASSERT_NUM(num, "NmbrFunction_Inc", return);

	if (num->HadRangeError() )
		num->SetRangeError(TRUE, DONT_POST_SIGNALS);
	else if (num->IsInteger())
		{
		lValue = num->GetWholeValue();
		if (ONmbr_lInc(&lValue))
			num->SetValue(lValue, POST_SIGNALS);
		else
			num->SetRangeError(TRUE, POST_SIGNALS);
		}
	else
		num->SetValue(num->GetFloatValue()+1.0, POST_SIGNALS, DONT_ROUND_IF_INT);

	UnlockNumberObject(oiNum);
	}

/////////////////////////////////////////


extern "C" void ALMAPI NmbrFunction_Dec(
	pAEvtInfo								theEvent,
	pAObjMessage						theSystem)
	{
	AOBJECTID          			oiNum;
	LPNUMBER								num;
	LONG										lValue;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiNum = AFuncGetTypedParameter(1, OTYPE_NUMBER);
	I_ASSERT_PARAMETER(oiNum, 1, "NmbrFunction_Dec", return);

	num = LockNumberObject(oiNum);
	I_ASSERT_NUM(num, "NmbrFunction_Dec", return);

	if (num->HadRangeError() )
		num->SetRangeError(TRUE, DONT_POST_SIGNALS);
	else if (num->IsInteger())
		{
		lValue = num->GetWholeValue();
		if (ONmbr_lDec(&lValue))
			num->SetValue(lValue, POST_SIGNALS);
		else
			num->SetRangeError(TRUE, POST_SIGNALS);
		}
	else
		num->SetValue(num->GetFloatValue()-1.0, POST_SIGNALS, DONT_ROUND_IF_INT);

	UnlockNumberObject(oiNum);
	}

/////////////////////////////////////////


extern "C" void ALMAPI NmbrFunction_Comp(
	pAEvtInfo								theEvent,
	pAObjMessage						theSystem)
	{
	AOBJECTID          			oiNum1, oiNum2;
	LPNUMBER							  num1, num2;

	I_UNUSED(theEvent);
	theSystem->message1 = 2;		//Equals is the default

	oiNum1 = AFuncGetTypedParameter(1, OTYPE_NUMBER);
	I_ASSERT_PARAMETER(oiNum1, 1, "NmbrFunction_Comp", return);

	oiNum2 = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	I_ASSERT_PARAMETER(oiNum2, 2, "NmbrFunction_Comp", return);

  num1 = LockNumberObject(oiNum1);
	I_ASSERT_NUM(num1, "NmbrFunction_Comp", return);

  num2 = LockNumberObject(oiNum2);
	I_ASSERT_NUM(num2, "NmbrFunction_Comp", return);

	if (num1->GetFloatValue() < num2->GetFloatValue())					theSystem->message1 = 1;
	else if (num1->GetFloatValue() > num2->GetFloatValue())			theSystem->message1 = 3;

	UnlockNumberObject(oiNum1);
	UnlockNumberObject(oiNum2);

	return;
	}

/////////////////////////////////////////

extern "C" void ALMAPI NmbrFunction_Rand(
	pAEvtInfo								theEvent,
	pAObjMessage 						theSystem)
	{
	AOBJECTID								oiMin, oiMax, oiOut;
	LPNUMBER								numMin, numMax, numOut;
	long double							ln, lnMin=0L, lnMax=65535L;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	if (  (oiMin = AFuncGetTypedParameter(1, OTYPE_NUMBER)) != 0)
		{
		numMin = LockNumberObject(oiMin);
		I_ASSERT_NUM(numMin, "NmbrFunction_Rand", return);
		lnMin = numMin->GetFloatValue();
    UnlockNumberObject(oiMin);
    }

	if (  (oiMax = AFuncGetTypedParameter(2, OTYPE_NUMBER)) != 0)
		{
		numMax = LockNumberObject(oiMax);
		I_ASSERT_NUM(numMax, "NmbrFunction_Rand", return);
		lnMax = numMax->GetFloatValue();
    UnlockNumberObject(oiMax);
    }
  
	if (lnMax < lnMin)
		{
		ln = lnMin;
		lnMin = lnMax;
		lnMax = ln;
		}

	oiOut = AObjCreate(OTYPE_NUMBER);
	I_ASSERT_PSUEDO(oiOut, "NmbrFunction_Rand", return);
	numOut = LockNumberObject(oiOut);
	I_ASSERT_NUM(numOut, "NmbrFunction_Rand", return);

	numOut->SetValue( (long double)((rand() * (lnMax-lnMin+1)) / RAND_MAX + lnMin), DONT_POST_SIGNALS, DONT_ROUND_IF_INT); //round 

  UnlockNumberObject(oiOut);

	AFuncSetParameter(1, oiOut);
	}

/////////////////////////

extern "C" void ALMAPI NmbrFunction_Randomize(
	pAEvtInfo								theEvent,
	pAObjMessage 						theSystem)
	{
	AOBJECTID          			oiSeed;
	UINT										seed;
  LPNUMBER								numSeed;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one flow

	if (  (oiSeed = AFuncGetTypedParameter(1, OTYPE_NUMBER)) != 0)
		{
		numSeed = LockNumberObject(oiSeed);
		I_ASSERT_NUM(numSeed, "NmbrFunction_Randomize", return);
		seed = (unsigned int)numSeed->GetFloatValue();	 //seed will lose presision, but so what -- it will still be random
    UnlockNumberObject(oiSeed);
		}
	else
		seed = (UINT)AAppGetTime();

	srand(seed);
	}

/////////////////////////////////////////////

extern "C" void ALMAPI NmbrFunction_SetFontSize(
	pAEvtInfo								theEvent,
	pAObjMessage						theSystem)
	{
	AOBJECTID      					oiNumber, oiFace, oiSize;
	LPNUMBER			  				num;
	char										szFace[LF_FACESIZE];
	LPATTRIBUTES						attr;
	long										height;
	long										changeFlag = 0L;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;    //only one exit flow

	oiNumber = AFuncGetTypedParameter(1, OTYPE_NUMBER);
	I_ASSERT_PARAMETER(oiNumber, 1, "NmbrFunction_SetFontSize", return);
	num = LockNumberObject(oiNumber);
	I_ASSERT_NUM(num, "NmbrFunction_SetFontSize", return);

	attr = num->GetAttributes();

	oiFace = AFuncGetTypedParameter(2, OTYPE_TEXT);
	if (oiFace)
		{
		OTextGetToBuffer(oiFace, szFace, sizeof(szFace));
		changeFlag |= ATTR_CHANGE_FACE;
		}
	else
		szFace[0] = '\0';

	oiSize = AFuncGetTypedParameter(3, OTYPE_NUMBER);
	if (oiSize)
		{
		height =  ONmbrGetInteger(oiSize);
		height *= 127L; //convert from points to TMM
		height /= 36L;
		changeFlag |= ATTR_CHANGE_HEIGHT;
    }
	else 
		height=0L;

	if (attr->AttrChangeFont(NULL, NULL, changeFlag, szFace, (int)height) )
		{
		OWndCallEachVisualInstance(
			oiNumber,
			num->VisualInstanceIsHwnd() ? HwndNumberItemChangeFont : LiteNumberItemChangeFont,
			(LONG)attr,
			(LONG)changeFlag);
		}

	UnlockNumberObject(oiNumber);
	}

///////////////////////////////////////////////////

extern "C" void ALMAPI NmbrFunction_OipTime(
	pAEvtInfo								theEvent,
	pAObjMessage						theSystem)
	{
	AOBJECTID          			oiNum;
  LPNUMBER								num;

	I_UNUSED(theEvent);
	theSystem->message1 = 1;		//Only one exit flow

	oiNum = AFuncGetTypedParameter(1, OTYPE_NUMBER);
	I_ASSERT_PARAMETER(oiNum, 1, "NmbrFunction_OipTime", return);

	num = LockNumberObject(oiNum);
	I_ASSERT_NUM(num, "NmbrFunction_OipTime", return);

	num->SetValue( AAppGetTime(), POST_SIGNALS, DONT_ROUND_IF_INT);

  UnlockNumberObject(oiNum);
	}

