#include "include.h"


extern "C" void ALMAPI _export PolyFunc_Compress(
	pAEvtInfo 							, 
	pAObjMessage 						theSystem)
	{
	I_UNUSED(theSystem);
	}

////////////////////////////////////////////////////
// Inputs  - Polybutton Object                    //
// Outputs - Number Object. the number of buttons //
// Flows		-	Done                                //
////////////////////////////////////////////////////
extern "C" void ALMAPI _export PolyFunc_CountRegions(
	pAEvtInfo 							, 
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiPoly, oiCount;
	LPPOLY									poly;

	theSystem->message1 = 1;		//Only one exit flow	

	oiPoly = AFuncGetTypedParameter(1, OTYPE_POLYBUTTON);
	I_VERIFY_PARAMETER(oiPoly, return);

	oiCount = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiCount, return);

	poly = LockPoly(oiPoly);
	I_VERIFY_POINTER(poly, return);

	ONmbrSetInteger(oiCount, (long)poly->GetNumberOfButtons(), TRUE);

	UnlockPoly(oiPoly);

	AFuncSetParameter(1, oiCount);
	}

/////////////////////////////////////////////////////
// Inputs  - PolyButton                            //
// Outputs - (number) index of last clicked button //
// 					(text) name of last clicked button     //
// Flows		- Done                                 //
// 					Failed -- no last ckicked button       //
/////////////////////////////////////////////////////
extern "C" void ALMAPI _export PolyFunc_GetClick(
	pAEvtInfo 							, 
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiPoly, oiNumber, oiText;
	USHORT									usLastClicked;
	char										szLastClicked[I_BUTTON_NAME_LENGTH];
	LPPOLY									poly;

	theSystem->message1 = 2;		//Default to failed flow	

	oiPoly = AFuncGetTypedParameter(1, OTYPE_POLYBUTTON);
	I_VERIFY_PARAMETER(oiPoly, return);

	oiNumber = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiNumber, return);

	oiText = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiText, return);

	poly = LockPoly(oiPoly);
	I_VERIFY_POINTER(poly, return);

	if (poly->GetLastClickedButton(&usLastClicked, szLastClicked, sizeof(szLastClicked)) )
		{
		ONmbrSetInteger(oiNumber, (long)usLastClicked+1, TRUE);
		OTextSetValueFromBuffer(oiText, szLastClicked);
		theSystem->message1 = 1;		//Done
		}	

	UnlockPoly(oiPoly);

	AFuncSetParameter(1, oiNumber);
	AFuncSetParameter(2, oiText);

	}




//////////////////////////
// Inputs  - Polybutton //
// Outputs - none       //
// Flows		- Edited    //
// 					Canceled    //
//////////////////////////
extern "C" void ALMAPI _export PolyFunc_Edit(
	pAEvtInfo 							, 
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiPoly;

	theSystem->message1 = 2;		//Default to cancled flow	

	oiPoly = AFuncGetTypedParameter(1, OTYPE_POLYBUTTON);
	I_VERIFY_PARAMETER(oiPoly, return);

	if (POLY::Edit(oiPoly) == A_MODIFIED)
		theSystem->message1 = 1;		//Edited flow	
	}


/////////////////////////////////////////////////
// Inputs  - Polybutton                        //
// 					(number)Button Index               //
// Outputs - (text)Region Name                 //
// Flows		- Done                             //
// 					Failed (region index out of range) //
/////////////////////////////////////////////////
extern "C" void ALMAPI _export PolyFunc_GetName(
	pAEvtInfo 							, 
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiPoly, oiIndex, oiName;
	LPPOLY									poly;
	USHORT									usIndex;
	char										szName[I_BUTTON_NAME_LENGTH];

	theSystem->message1 = 2;		//Default to failed flow	

	oiPoly = AFuncGetTypedParameter(1, OTYPE_POLYBUTTON);
	I_VERIFY_PARAMETER(oiPoly, return);

	oiIndex= AFuncGetTypedParameter(2, OTYPE_NUMBER);
	I_VERIFY_PARAMETER(oiIndex, return);

	oiName = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiName, return);

	usIndex = (USHORT)ONmbrGetInteger(oiIndex)-1;

	poly = LockPoly(oiPoly);
	I_VERIFY_POINTER(poly, return);

	if (poly->GetButtonNameByIndex(usIndex, szName, sizeof(szName)))
		{
		OTextSetValueFromBuffer(oiName, szName);
		theSystem->message1 = 1;		//Done flow	
		}

	UnlockPoly(oiPoly);

	AFuncSetParameter(1, oiName);
	}	



//////////////////////////////////////
// Inputs  - Polybutton             //
// 					(text) button name      //
// Outputs - (number) button index  //
// Flows		- Done                  //
// 					Failed (name not found) //
//////////////////////////////////////
extern "C" void ALMAPI _export PolyFunc_GetIndex(
	pAEvtInfo 							, 
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiPoly, oiIndex, oiName;
	LPPOLY									poly;
	USHORT									usIndex;
	char										szName[I_BUTTON_NAME_LENGTH];

	theSystem->message1 = 2;		//Default to failed flow	

	oiPoly = AFuncGetTypedParameter(1, OTYPE_POLYBUTTON);
	I_VERIFY_PARAMETER(oiPoly, return);

	oiName= AFuncGetTypedParameter(2, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiName, return);

	oiIndex = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiIndex, return);

	OTextGetToBuffer(oiName, szName, sizeof(szName));

	poly = LockPoly(oiPoly);
	I_VERIFY_POINTER(poly, return);

	if (poly->GetButtonIndexByName(&usIndex, szName))
		{
		ONmbrSetInteger(oiIndex, (long)usIndex+1, TRUE);
		theSystem->message1 = 1;		//Done flow	
		}

	UnlockPoly(oiPoly);

	AFuncSetParameter(1, oiIndex);
	}


////////////////////////////////////////////////
// Inputs  - Polybutton                       //
// 					(opt number) button index         //
// 					(opt text) button name            //
// Outputs - none                             //
// Flows		- done                            //
// 					failed -- name or index not valid //
////////////////////////////////////////////////
extern "C" void ALMAPI _export PolyFunc_SetRegion(
	pAEvtInfo 							, 
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiPoly, oiIndex, oiName;
	LPPOLY									poly;
	char										szName[I_BUTTON_NAME_LENGTH];

	theSystem->message1 = 2;		//Default to failed flow	

	oiPoly = AFuncGetTypedParameter(1, OTYPE_POLYBUTTON);
	I_VERIFY_PARAMETER(oiPoly, return);

	poly = LockPoly(oiPoly);
	I_VERIFY_POINTER(poly, return);

	if ( (oiIndex = AFuncGetTypedParameter(2, OTYPE_NUMBER)) != 0)
		{
		if (poly->SetButton((USHORT)ONmbrGetInteger(oiIndex)-1))
			theSystem->message1 = 1;		//Done flow	
		}
	else if ( (oiName = AFuncGetTypedParameter(3, OTYPE_TEXT)) != 0)
		{
		OTextGetToBuffer(oiName, szName, sizeof(szName));
		if (poly->SetButton(szName))
			theSystem->message1 = 1;		//Done flow	
		}

	UnlockPoly(oiPoly);
	}



////////////////
// Inputs  -  //
// Outputs -  //
// Flows		- //
////////////////
extern "C" void ALMAPI _export PolyFunc_GetPictures(
	pAEvtInfo 							, 
	pAObjMessage 						theSystem)
	{
	I_UNUSED(theSystem);
	}


////////////////////////////////////////////////////
// Inputs  - Polybutton                           //
// 					(number) button index                 //
// 					(text opt) new name (none=clear name) //
// Outputs - none                                 //
// Flows		- done                                //
// 					failed. index out of range            //
////////////////////////////////////////////////////
extern "C" void ALMAPI _export PolyFunc_SetName(
	pAEvtInfo 							, 
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiPoly, oiIndex, oiName;
	LPPOLY									poly;
	char										szName[I_BUTTON_NAME_LENGTH];

	theSystem->message1 = 2;		//Default to failed flow	

	oiPoly = AFuncGetTypedParameter(1, OTYPE_POLYBUTTON);
	I_VERIFY_PARAMETER(oiPoly, return);

	oiIndex = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	I_VERIFY_PARAMETER(oiIndex, return);

 	if ( (oiName = AFuncGetTypedParameter(3, OTYPE_TEXT)) != 0)
		OTextGetToBuffer(oiName, szName, sizeof(szName));
	else
		szName[0] = 0;

	poly = LockPoly(oiPoly);
	I_VERIFY_POINTER(poly, return);

	if (poly->SetButtonName((USHORT)ONmbrGetInteger(oiIndex)-1, szName))
		theSystem->message1 = 1;		//Done flow	

	UnlockPoly(oiPoly);
	}




//////////////////////////////////////////
// Inputs  - Polybutton                 //
// 					(number opt) region index   //
// 					(text opt) region name      //
// Outputs - none                       //
// Flows		- Set                       //
// 					Not Set                     //
// 					Failed (index out of range) //
//////////////////////////////////////////
extern "C" void ALMAPI _export PolyFunc_IsRegionSet(
	pAEvtInfo 							, 
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiPoly, oiIndex, oiName;
	LPPOLY									poly;
	char										szName[I_BUTTON_NAME_LENGTH];
	BOOL										bSet;

	theSystem->message1 = 3;		//Default to failed flow	

	oiPoly = AFuncGetTypedParameter(1, OTYPE_POLYBUTTON);
	I_VERIFY_PARAMETER(oiPoly, return);

	poly = LockPoly(oiPoly);
	I_VERIFY_POINTER(poly, return);

	if ( (oiIndex = AFuncGetTypedParameter(2, OTYPE_NUMBER)) != 0)
		{
		if ( poly->IsButtonSet((USHORT)ONmbrGetInteger(oiIndex)-1, &bSet))
			{
			if (bSet)
				theSystem->message1 = 1;		//set flow	
			else
				theSystem->message1 = 2;		//not set flow	
			}
		}
	else if ( (oiName = AFuncGetTypedParameter(3, OTYPE_TEXT)) != 0)
		{
		OTextGetToBuffer(oiName, szName, sizeof(szName));
		if ( poly->IsButtonSet(szName, &bSet))
			{
			if (bSet)
				theSystem->message1 = 1;		//set flow	
			else
				theSystem->message1 = 2;		//not set flow	
			}
		}

	UnlockPoly(oiPoly);
	}


////////////////////////////////////////
// Inputs  - Polybutton               //
// 					(number opt) region index //
// 					(text opt) region name    //
// Outputs - none                     //
// Flows		- done                    //
// 					failed                    //
////////////////////////////////////////
extern "C" void ALMAPI _export PolyFunc_ClearRegion(
	pAEvtInfo 							, 
	pAObjMessage 						theSystem)
	{
	OBJECTID								oiPoly, oiIndex, oiName;
	LPPOLY									poly;
	char										szName[I_BUTTON_NAME_LENGTH];

	theSystem->message1 = 2;		//Default to failed flow	

	oiPoly = AFuncGetTypedParameter(1, OTYPE_POLYBUTTON);
	I_VERIFY_PARAMETER(oiPoly, return);

	poly = LockPoly(oiPoly);
	I_VERIFY_POINTER(poly, return);

	if ( (oiIndex = AFuncGetTypedParameter(2, OTYPE_NUMBER)) != 0)
		{
		if (poly->ClearButton((USHORT)ONmbrGetInteger(oiIndex)-1))
			theSystem->message1 = 1;		//Done flow	
		}
	else if ( (oiName = AFuncGetTypedParameter(3, OTYPE_TEXT)) != 0)
		{
		OTextGetToBuffer(oiName, szName, sizeof(szName));
		if (poly->ClearButton(szName))
			theSystem->message1 = 1;		//Done flow	
		}

	UnlockPoly(oiPoly);
	}

////////////////
// Inputs  -  //
// Outputs -  //
// Flows		- //
////////////////
extern "C" void ALMAPI _export PolyFunc_SetPictures(
	pAEvtInfo 							, 
	pAObjMessage 						theSystem)
	{
	theSystem->message1 = 1;		//Only one flow	
	}


