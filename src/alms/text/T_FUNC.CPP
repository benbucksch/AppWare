#include "t_includ.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/////////////////////////////////////////////

BOOL GetLine(
  LPSTR   								szBuffer,
  LPUINT									ptrStartPos,
	LPUINT									ptrEndPos,
	UINT										uLineIndex,
	AOBJECTID								oiDestination)
	{
	LPSTR										chpStart, chpEnd;
	LPSTR										chpDestination;
	UINT										lenLine, i;
	BOOL										rVal=FALSE;

	//Backup untill we get a LF then go forward one
	chpStart = &szBuffer[*ptrStartPos];
	while(chpStart > szBuffer  &&  *chpStart != LF)
		chpStart--;
	if (*chpStart == LF)
		chpStart++;

	//Advance to line index
	i = 1;
	while(*chpStart && i<uLineIndex)
		{
		if (*chpStart == LF)
			i++;
		chpStart++;
		}
	
	if (*chpStart)
		{
		//Now, go forward untill we reach a CR or the end of the text
		chpEnd = chpStart;
		while (*chpEnd  &&  *chpEnd != CR)
			chpEnd++;

		lenLine = (UINT) (chpEnd-chpStart);
		if (ResizeTextString(oiDestination, lenLine+1))
			{
			chpDestination = LockTextString(oiDestination);
			I_ASSERT_STRING(chpDestination, "GetLine", return FALSE);
			CopyMemory(chpDestination, chpStart, lenLine);
			chpDestination[lenLine]=0;
			UnlockTextString(oiDestination);
			rVal = TRUE;
			}

		if (*chpEnd == CR)
			chpEnd++; //to go to the <LF>

  	*ptrStartPos = (UINT) (chpStart - szBuffer);
		*ptrEndPos = (UINT) (chpEnd - szBuffer) + 1;
			
		}

	return rVal;
  }

/////////////////////////////////

BOOL GetField(
  LPSTR   								szBuffer,
  LPUINT									ptrStartPos,
	LPUINT									ptrEndPos,
	UINT										uFieldIndex,
	char										delimeter1,
	char										delimeter2,
	AOBJECTID								oiDestination)
	{
	LPSTR										chpStart, chpEnd;
	LPSTR										chpDestination;
	UINT										lenField, i;
	BOOL										rVal=FALSE;

	//Backup untill we get a delimeter or then go forward one
	chpStart = &szBuffer[*ptrStartPos];
	while(chpStart > szBuffer  &&  *chpStart != delimeter1  &&  *chpStart != delimeter2)
		chpStart--;
	if (*chpStart && (*chpStart == delimeter1 || *chpStart == delimeter2))
		chpStart++;
	if (*chpStart == LF)//Ignore LFs
		chpStart++;

	//Advance to field index
	i = 1;
	while(*chpStart && i<uFieldIndex)
		{
		if (*chpStart == delimeter1 || *chpStart == delimeter2)
			i++;
		chpStart++;
		}
	if (*chpStart == LF)//Ignore LFs
		chpStart++;
	
	if (*chpStart)
		{
		//Now, go forward untill we reach a delimeter or the end of the text
		chpEnd = chpStart;
		while (*chpEnd  &&  *chpEnd != delimeter1 && *chpEnd != delimeter2)
			chpEnd++;

		lenField = (UINT)(chpEnd-chpStart);
		if (ResizeTextString(oiDestination, lenField+1))
			{
			chpDestination = LockTextString(oiDestination);
			I_ASSERT_STRING(chpDestination, "GetField", return FALSE);
			CopyMemory(chpDestination, chpStart, lenField);
			chpDestination[lenField]=0;
			UnlockTextString(oiDestination);
			rVal = TRUE;
			}

		if (*chpEnd == CR)//To go to the LF
			chpEnd++;

   	*ptrStartPos = (UINT)(chpStart - szBuffer);
		*ptrEndPos = (UINT)(chpEnd - szBuffer) + 1;
		}

	return rVal;
  }

/////////////////////////////////////////

BOOL IsWordDelimeter(
	LPSTR										szText,
	UINT										uPos)
	{
	char										chThis, chPrev, chNext;

	chThis = szText[uPos];

	if(!chThis)
		return TRUE;

	if (isspace(chThis))
		return TRUE;

	if (isalnum(chThis))
		return FALSE;

	chPrev = uPos>0 ? szText[uPos-1] : '\0';
	chNext = szText[uPos+1];
	
	if(chThis== '-'  ||  chThis == '\'') //a dash or apostrophy
		{
		if (!isalnum(chPrev)  ||  !isalnum(chNext))
			return TRUE;
		else
			return FALSE;
		}

	if (ispunct(chThis))
		{
		if (!isdigit(chPrev)  ||  !isdigit(chNext))
			return TRUE;
		else
			return FALSE;
		}

	return TRUE;
	}

////////////////////

BOOL GetWord(
  LPSTR   								szBuffer,
  LPUINT									ptrStartPos,
	LPUINT									ptrEndPos,
	UINT										uWordIndex,
	AOBJECTID								oiDestination)
	{
	UINT										uStartPos, uEndPos, uBufferLen, i, uLenWord;
	LPSTR										szDestination;
	BOOL										rVal=FALSE;
	BOOL										bOnDelimeter, bWasOnDelimeter;

	uStartPos = *ptrStartPos;
	uBufferLen = lstrlen(szBuffer);

	//Backtrack to the start of the word if not one a delimeters
	bOnDelimeter = IsWordDelimeter(szBuffer, uStartPos);
	while(uStartPos > 0  &&  !bOnDelimeter)
		{
		uStartPos--;
		bOnDelimeter = IsWordDelimeter(szBuffer, uStartPos);
		}
	// go forward to the start of the word
	while(uStartPos < uBufferLen  &&  IsWordDelimeter(szBuffer, uStartPos))
		uStartPos++;
	//Now, we should be at the start of the word

	//Count words from this point

	bWasOnDelimeter = bOnDelimeter = IsWordDelimeter(szBuffer, uStartPos);
	i = 1;
	while(uStartPos < uBufferLen  &&  i<uWordIndex)
		{
		uStartPos++;
		bOnDelimeter = IsWordDelimeter(szBuffer, uStartPos);
		if (bOnDelimeter != bWasOnDelimeter)
			{
			bWasOnDelimeter = bOnDelimeter;
			if (!bOnDelimeter)
				i++;
			}
		}

	if (uStartPos < uBufferLen)
		{//Well, we are at the start of the word
		uEndPos = uStartPos;
		while(uEndPos < uBufferLen  &&  !bOnDelimeter)
			{
			uEndPos++;
			bOnDelimeter = IsWordDelimeter(szBuffer, uEndPos);
			}

		uLenWord = (UINT)(uEndPos - uStartPos);
		if (ResizeTextString(oiDestination, uLenWord + 1))
			{
			szDestination = LockTextString(oiDestination);
			I_ASSERT_STRING(szDestination, "GetWord", return FALSE);
			CopyMemory(szDestination, &szBuffer[uStartPos], uLenWord);
			szDestination[uLenWord] = 0;
			UnlockTextString(oiDestination);
			*ptrStartPos = uStartPos;
			*ptrEndPos = uEndPos;
			rVal = TRUE;
			}
		}

  return rVal;
}

////////////////////////////////////////////////
////////////////////////////////////////////////
BOOL IsNumberPart(
	LPSTR										szText,
	UINT										uPos)
	{
	char										chThis, chPrev, chNext, chNextNext;

	chThis = szText[uPos];

	if (isdigit(chThis))
		return TRUE;

	chPrev = uPos > 0 ? szText[uPos-1] : '\0';
	chNext = szText[uPos+1];
	chNextNext = chNext ? szText[uPos+2] : '\0';

	if ( (chThis|0x20) == 'e'  ||  chThis == ',')
		{//These need to be surrounded by digits
		if (!isdigit(chPrev) || !isdigit(chNext))
			return FALSE;
		return TRUE;
		}

	if (chThis == '.')
		{
		if (isdigit(chNext))
			return TRUE;
		return FALSE;
		}

	if (chThis == '\%')
		{
		if (isdigit(chPrev))
			return TRUE;
		return FALSE;
		}

	if (chThis == '-'  ||  chThis == '+')
		{
		if (isdigit(chNext))
			return TRUE;
		if (chNext == '.'  &&  isdigit(chNextNext))
			return TRUE;
		return FALSE;
		}

	return FALSE;
	}



////////////////////////////////////////////////
/// Finds a number starting at startPos and
/// then sets startPos to the end of the number
///////////////////////////////////////////
BOOL GetNumber(
  LPSTR   								szBuffer,
  LPUINT   								ptrStartPos, 
  LPUINT   								ptrEndPos,  
	UINT										uNumberIndex,
	long double*						num)
	{
	UINT										uStartPos, uEndPos, uBufferLen, i, iStrlen;
	BOOL										bOnNumber, bWasOnNumber;
	char										szNumber[100];
	BOOL										rVal=FALSE;

	uStartPos = *ptrStartPos;
	uBufferLen = lstrlen(szBuffer);

	//Backup to the start of the number
	while(uStartPos >0  &&  IsNumberPart(szBuffer, uStartPos))
		uStartPos--;

	//Go forward to the start of the number
	while(uStartPos < uBufferLen  &&  !IsNumberPart(szBuffer, uStartPos))
		uStartPos++;

	bWasOnNumber = IsNumberPart(szBuffer, uStartPos);
	i = 1;
	while(uStartPos < uBufferLen  &&  i<uNumberIndex)
		{
		uStartPos++;
		bOnNumber = IsNumberPart(szBuffer, uStartPos);
		if (bOnNumber != bWasOnNumber)
			{
			bWasOnNumber = bOnNumber;
			if (bOnNumber)
				i++;
			}
		}

	if (uStartPos < uBufferLen)
		{//Well, we are at the start of the number
		uEndPos = uStartPos;
		while(uEndPos < uBufferLen  &&  IsNumberPart(szBuffer, uEndPos))
			uEndPos++;

		rVal = TRUE;

		//change the string to fit what _atold will accept (strip commas)
		iStrlen = min(sizeof(szNumber), uEndPos-uStartPos+1);
		MoveMemory(szNumber, &szBuffer[uStartPos], iStrlen);
		szNumber[iStrlen-1]=0;
		for(i=0; i<iStrlen-1; i++)
			{
			if (szNumber[i] == ',')
				{
				MoveMemory(&szNumber[i], &szNumber[i+1], iStrlen-i);
				iStrlen--;
				}
			}
				
//		*num = _atold(szNumber);
		*num = atof(szNumber);
		*ptrStartPos = uStartPos;
		*ptrEndPos = uEndPos;
		}

  return rVal;
  }

//////////////////////////////////////////////																													 
																													 

BOOL FindTheText(
	LPSTR									szWholeBuffer,
	LPUINT								ptrStartPos,  				//zero-based
	LPSTR									szPattern,
	BOOL									bCaseSensitive)
	{
	LPSTR									chpBuffer, chpPattern;
	LPSTR									szBuffer;
	BOOL									bEqual;
	BOOL									rVal = FALSE;
	UINT									uPatternLen;

	szBuffer = &szWholeBuffer[*ptrStartPos];
	chpBuffer = szBuffer;
	chpPattern = szPattern;
	uPatternLen = lstrlen(szPattern);

	while(*chpBuffer && *chpPattern)
		{
		if (bCaseSensitive)
			bEqual = (*chpBuffer == *chpPattern);
		else
			bEqual = ( toupper(*chpBuffer) == toupper(*chpPattern) );
			
		if(!bEqual)
			{
			chpBuffer -= (UINT)(chpPattern-szPattern)-1;
			chpPattern = szPattern;
			}
		else
			{
			chpBuffer++;
			chpPattern++;
			}
		}

	if(!*chpPattern)
		{
		(*ptrStartPos) += (UINT)(chpBuffer - szBuffer) - uPatternLen;
		rVal = TRUE;
		}

	return rVal;
	}

////////////////////////////////////////////

BOOL ReplaceText( 
	AOBJECTID								oiSource,
	AOBJECTID								oiReplace,
	UINT										position,
	UINT										count)
	{
	LPSTR										szSource, szReplace;
	int											lenReplace, lenSource;
	BOOL										rVal = FALSE;

	szSource = LockTextString(oiSource);
	I_ASSERT_STRING(szSource, "ReplaceText", return FALSE);
	lenSource = lstrlen(szSource);
	UnlockTextString(oiSource);

	szReplace = LockTextString(oiReplace);
	I_ASSERT_STRING(szReplace, "ReplaceText", return FALSE);
	lenReplace = lstrlen(szReplace);

	if (count > lenReplace)
		{ // replace with a smaller string.
		szSource = LockTextString(oiSource);
		I_ASSERT_STRING(szSource, "ReplaceText", return FALSE);
		
		CopyMemory(&szSource[position], szReplace, lenReplace);
		MoveMemory(&szSource[position+lenReplace],
						&szSource[position+count],
						lenSource-position-count+1); 

		UnlockTextString(oiSource);
		ResizeTextString(oiSource, lenSource-count+lenReplace+1);  //resizing to smaller -- should never fail
		rVal = TRUE;
		}

	else if (count < lenReplace)
		{        // replace with a larger string.
		if (ResizeTextString(oiSource, lenSource-count+lenReplace+1))
			{
			szSource = LockTextString(oiSource);
			I_ASSERT_STRING(szSource, "ReplaceText", return FALSE);

			MoveMemory(&szSource[position+lenReplace],
							&szSource[position+count], lenSource-position-count+1);
			CopyMemory(&szSource[position], szReplace, lenReplace);

			UnlockTextString(oiSource);
			rVal = TRUE;
			}
		I_ASSERT(rVal);  //did it work
		}

	else
		{
		szSource = LockTextString(oiSource);
		I_ASSERT_STRING(szSource, "ReplaceText", return FALSE);

		CopyMemory(&szSource[position], szReplace, lenReplace);

		UnlockTextString(oiSource);
		rVal = TRUE;
		}

	UnlockTextString(oiReplace);

	return rVal;	
	}

////////////////////////////////////////////

extern "C" void ALMAPI TextFunction_AppendReturn(
  pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
  {
  static char  						eolMark[] = "\r\n";
  AOBJECTID     						oiText;
  LPSTR        						szText;
  int         						len;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;    //only one possible outflow

	oiText = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiText, 1, "TextFunction_AppendReturn", return);

	len = GetTextStringSize(oiText) + lstrlen(eolMark);
	if (ResizeTextString(oiText, len))
		{
	  szText = LockTextString(oiText);
		lstrcat(szText, eolMark);
	  UnlockTextString(oiText);
		PostObjectChanged(oiText, 0L, DONTPOSTEDITED, NODELAY);
		}
	}

/////////////////////////////////////////

extern "C" void ALMAPI TextFunction_AppendTab(
  pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
  {
	static char  						tabMark[] = "\t";
	AOBJECTID     						oiText;
  LPSTR        						szText;
  int         						len;

	I_UNUSED(theEvent);
	
	theSystem->message1 = 1;    //only one possible outflow

	oiText = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiText, 1, "TextFunction_AppendTab", return);

	len = GetTextStringSize(oiText) + lstrlen(tabMark);
	if (ResizeTextString(oiText, len))
		{
	  szText = LockTextString(oiText);
		lstrcat(szText, tabMark);
	  UnlockTextString(oiText);
		PostObjectChanged(oiText, 0L, DONTPOSTEDITED, NODELAY);
		}
	}

///////////////////////////////////////////////////////////////////////////////////
// INPUTS	--		1) The text object
//							2) The one-based position -- defaults to 1
//							3) The count (can be negitive) -- defaults to the length of the text
///////////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI TextFunction_Clear(
  pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
  {
 	AOBJECTID     						oiText, oiPos, oiCount;
  LPSTR        						szText;
	long										lPos, lCount;
	UINT										iPos, iCount, iLength;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;    //only one possible outflow

	oiText = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiText, 1, "TextFunction_Clear", return);
	szText = LockTextString(oiText);
	I_ASSERT_STRING(szText, "TextFunction_Clear", return);
	
	oiPos = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	if (oiPos)
		lPos = ONmbrGetInteger(oiPos);
	else 
		lPos = 1L;

	oiCount = AFuncGetTypedParameter(3, OTYPE_NUMBER);
	if (oiCount)
		lCount = ONmbrGetInteger(oiCount);
	else
		lCount = ParameterStringLength(szText);

	if (lCount < 0)
		{  //A negitive count really just moves back the starting position
		lPos += lCount + 1;
		lCount *= -1L;
		}

	iPos = (int)lPos;
	iCount = (int)lCount;

	ParameterPosAndCountToActual(szText, &iPos, &iCount);

	if (iCount)
		{
		lstrcpy(&szText[iPos], &szText[iPos+iCount]);
		iLength = lstrlen(szText)+1;
	  UnlockTextString(oiText);
		ResizeTextString(oiText, iLength);  //shrinking -- shouldn't fail
		PostObjectChanged(oiText, 0L, DONTPOSTEDITED, NODELAY);
		}
	else
		UnlockTextString(oiText);
	}

///////////////////////////////////////////////////////////////////////////////////
// INPUTS	--		1) The first text object
//							2) The second text object
//							3) "Case" or "No Case" -- defaults to "No Case"
// FLOWS	--		1=less than 2=equal 3=greater than
///////////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI TextFunction_Compare(
  pAEvtInfo								theEvent,
  pAObjMessage    				theSystem)
  {
  AOBJECTID  							oiText1, oiText2, oiCase;
  LPSTR     							szText1, szText2, szCase;
  int       							i;
	BOOL										bCaseSensitive=FALSE;

	I_UNUSED(theEvent);

  theSystem->message1 = 2;    //default to equal

	oiText1 = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiText1, 1, "TextFunction_Compare", return);

	oiText2 = AFuncGetTypedParameter(2, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiText2, 2, "TextFunction_Compare", return);

	if ( (oiCase = AFuncGetTypedParameter(3, OTYPE_TEXT)) != 0)
		{		 
		szCase = LockTextString(oiCase);
		I_ASSERT_STRING(szCase, "TextFunction_Compare", return);
		bCaseSensitive = IsCaseSensitive(szCase);
		UnlockTextString(oiCase);
		}

	szText1 = LockTextString(oiText1);
	I_ASSERT_STRING(szText1, "TextFunction_Compare", return);

	szText2 = LockTextString(oiText2);
	I_ASSERT_STRING(szText2, "TextFunction_Compare", UnlockTextString(oiText1);return);

	if (bCaseSensitive)
		i = lstrcmp(szText1, szText2);
	else
		i = lstrcmpi(szText1, szText2);

	UnlockTextString(oiText1);
	UnlockTextString(oiText2);

  if (i < 0)
    theSystem->message1 = 1;
  else if (i > 0)
    theSystem->message1 = 3;
  }

///////////////////////////////////////////////////////////////////////////////////
// INPUTS	--		1) The first text object
//							2) The second text object
// OUTPUTS	--	1) The result text
///////////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI TextFunction_Concat(
  pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
	{
  AOBJECTID   							oiText1, oiText2, oiTextOut;
  LPSTR      							szText1, szText2, szTextOut;
  int       							len;

	I_UNUSED(theEvent);

  theSystem->message1 = 1;   // "There's only one!"

	oiText1 = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiText1, 1, "TextFunction_Concat", return);

	oiText2 = AFuncGetTypedParameter(2, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiText2, 2, "TextFunction_Concat", return);

	oiTextOut = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiTextOut, "TextFunction_Concat", return);

	szText1 = LockTextString(oiText1);
	I_ASSERT_STRING(szText1, "TextFunction_Concat", return);

	szText2 = LockTextString(oiText2);
	I_ASSERT_STRING(szText2, "TextFunction_Concat", UnlockTextString(oiText1);return);

	len = lstrlen(szText1) + lstrlen(szText2) + 1;

	if (ResizeTextString(oiTextOut, len))
		{
		szTextOut = LockTextString(oiTextOut);
		I_ASSERT_STRING(szTextOut, "TextFunction_Concat", UnlockTextString(oiText1);UnlockTextString(oiText2);return);

	  lstrcpy(szTextOut, szText1);
		lstrcat(szTextOut, szText2);
		UnlockTextString(oiTextOut);
		}

	UnlockTextString(oiText1);
	UnlockTextString(oiText2);

	AFuncSetParameter(1, oiTextOut);
  }

///////////////////////////////////////////////////////////////////////////////////
// INPUTS	--		1) The source text object
//							2) The text to find
//							3) The replacement text -- optional
//							4) The start position
//							5) "Case" or "No Case" -- defaults to "No Case"
// OUTPUTS --		1) The start position of the found text
//							2) The next position
// FLOWS --			1=After Getting 2=If not found
///////////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI TextFunction_FindReplace(
  pAEvtInfo								theEvent,
  pAObjMessage    				theSystem)
  {
	AOBJECTID      					oiSource, oiFind, oiReplace, oiPos, oiCase;
	AOBJECTID								oiStartPos, oiNextPos;
	LPSTR         					szSource, szFind, szCase;
	UINT										uPos, findLen;
	BOOL										bFound, bCaseSensitive=FALSE;
  
  I_UNUSED(theEvent);

	theSystem->message1 = 2;    //default to not found

	oiSource = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiSource, 1, "TextFunction_FindReplace", return);

	oiFind = AFuncGetTypedParameter(2, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiFind, 2, "TextFunction_FindReplace", return);

	oiReplace = AFuncGetTypedParameter(3, OTYPE_TEXT);

	if ( (oiPos = AFuncGetTypedParameter(4, OTYPE_NUMBER)) != 0)
		uPos = (UINT)ONmbrGetInteger(oiPos);
	else
		uPos = 1;

	if ( (oiCase = AFuncGetTypedParameter(5, OTYPE_TEXT)) != 0)
		{		 
		szCase = LockTextString(oiCase);
		I_ASSERT_STRING(szCase, "TextFunction_FindReplace", return);
		bCaseSensitive = IsCaseSensitive(szCase);
		UnlockTextString(oiCase);
		}

	oiStartPos = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiStartPos, "TextFunction_FindReplace", return);
  
	oiNextPos = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiNextPos, "TextFunction_FindReplace", return);
  
	szSource = LockTextString(oiSource);
	I_ASSERT_STRING(szSource, "TextFunction_FindReplace", return);
 
 	szFind = LockTextString(oiFind);
	I_ASSERT_STRING(szFind, "TextFunction_FindReplace", UnlockTextString(oiSource);return);
	findLen = lstrlen(szFind);

	if (ParameterPosAndCountToActual(szSource, &uPos, NULL) )
		bFound = FindTheText(szSource, &uPos, szFind, bCaseSensitive);
	else
		bFound = FALSE;

	UnlockTextString(oiSource);
	UnlockTextString(oiFind);

	if (bFound)
		{
		if (oiReplace)
    	{
			if (ReplaceText(oiSource, oiReplace, uPos, findLen))
				PostObjectChanged(oiSource, 0L, DONTPOSTEDITED, NODELAY);
			}

		szSource = LockTextString(oiSource);
		I_ASSERT_STRING(szSource, "TextFunction_FindReplace", return);
		ActualPosAndCountToParameter(szSource, &uPos, &findLen);
		UnlockTextString(oiSource);

		ONmbrSetInteger(oiStartPos, uPos, TRUE);
		ONmbrSetInteger(oiNextPos, uPos + findLen, TRUE);
		theSystem->message1 = 1;    //found
		}

	AFuncSetParameter(1, oiStartPos);
	AFuncSetParameter(2, oiNextPos);
	}

///////////////////////////////////////////////////////////////////////////////////
// INPUTS	--		1) The text object
//							2) The starting character position (one-based)  default=1
//							3) The field index (one-based)									default=1
//							4) The deleimters in the form ascii,ascii				default=13,09 (<CR>,<TAB>)
// OUTPUTS --		1) The field text
//							2) The start position of the field
//							3) The next position
// FLOWS --			1=After Getting 2=If not found
///////////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI TextFunction_GetField(
  pAEvtInfo								theEvent,
	pAObjMessage  					theSystem)
  {
  AOBJECTID      					oiSource, oiPos, oiIndex, oiDelimeters, oiTextOut, oiStartOut, oiNextOut;
  LPSTR         					szSource, szDelimeters;
	UINT										iPos=1, iIndex=1, iNext;
	int											delimeter1 = CR;
	int											delimeter2 = TAB;

	I_UNUSED(theEvent);

	theSystem->message1 = 2;    //default to not found

		//Get the source string
	oiSource = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiSource, 1, "TextFunction_GetField", return);

	if ( (oiPos = AFuncGetTypedParameter(2, OTYPE_NUMBER)) != 0)
		iPos = (UINT)ONmbrGetInteger(oiPos);

	if ( (oiIndex = AFuncGetTypedParameter(3, OTYPE_NUMBER)) != 0)
		iIndex = (UINT)ONmbrGetInteger(oiIndex);

	if ( (oiDelimeters = AFuncGetTypedParameter(4, OTYPE_TEXT)) != 0)
		{
		szDelimeters = LockTextString(oiDelimeters);
		I_ASSERT_STRING(szDelimeters, "TextFunction_GetField", return);
		delimeter1 = delimeter2 = 0; 
		sscanf(szDelimeters, "%d,%d", &delimeter1, &delimeter2);
		UnlockTextString(oiDelimeters);
		}

	oiTextOut = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiTextOut, "TextFunction_GetField", return);
	oiStartOut = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiStartOut, "TextFunction_GetField", return);
	oiNextOut = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiNextOut, "TextFunction_GetField", return);

  szSource = LockTextString(oiSource);
	I_ASSERT_STRING(szSource, "TextFunction_GetField", return);

	if ( ParameterPosAndCountToActual(szSource, &iPos, NULL)  &&  
		   GetField(szSource, &iPos, &iNext, iIndex, (char)delimeter1, (char)delimeter2, oiTextOut))
		{
		ActualPosAndCountToParameter(szSource, &iPos, NULL);
		ActualPosAndCountToParameter(szSource, &iNext, NULL);
		ONmbrSetInteger(oiStartOut, iPos, TRUE);
		ONmbrSetInteger(oiNextOut, iNext, TRUE);
		theSystem->message1 = 1;    //found
		}

	UnlockTextString(oiSource);

	AFuncSetParameter(1, oiTextOut);
	AFuncSetParameter(2, oiStartOut);
	AFuncSetParameter(3, oiNextOut);
	}


///////////////////////////////////////////////////////////////////////////////////
// INPUTS	--		1) The text object
//							2) The starting character position (one-based)  default=1
//							3) The line index (one-based)										default=1
// OUTPUTS --		1) The line text
//							2) The start position of the line
//							3) The next position
// FLOWS --			1=After Getting 2=If not found
///////////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI TextFunction_GetLine(
	pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
  {
  AOBJECTID      					oiSource, oiPos, oiIndex, oiTextOut, oiStartOut, oiNextOut;
  LPSTR         					szSource;
	UINT										iPos=1, iIndex=1, iNext;

	I_UNUSED(theEvent);

	theSystem->message1 = 2;    //default to not found

		//Get the source string
	oiSource = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiSource, 1, "TextFunction_GetLine", return);

	if ( (oiPos = AFuncGetTypedParameter(2, OTYPE_NUMBER)) != 0)
		iPos = (UINT)ONmbrGetInteger(oiPos);

	if ( (oiIndex = AFuncGetTypedParameter(3, OTYPE_NUMBER)) != 0)
		iIndex = (UINT)ONmbrGetInteger(oiIndex);

	oiTextOut = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiTextOut, "TextFunction_GetLine", return);
	oiStartOut = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiStartOut, "TextFunction_GetLine", return);
	oiNextOut = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiNextOut, "TextFunction_GetLine", return);

  szSource = LockTextString(oiSource);
	I_ASSERT_STRING(szSource, "TextFunction_GetLine", return);

	if ( ParameterPosAndCountToActual(szSource, &iPos, NULL)  &&
	     GetLine(szSource, &iPos, &iNext, iIndex, oiTextOut))
		{
		ActualPosAndCountToParameter(szSource, &iPos, NULL);
		ActualPosAndCountToParameter(szSource, &iNext, NULL);
		ONmbrSetInteger(oiStartOut, iPos, TRUE);
		ONmbrSetInteger(oiNextOut, iNext, TRUE);
		theSystem->message1 = 1;    //found
		}

	UnlockTextString(oiSource);

	AFuncSetParameter(1, oiTextOut);
	AFuncSetParameter(2, oiStartOut);
	AFuncSetParameter(3, oiNextOut);
	}

///////////////////////////////////////////////////////////////////////////////////
// INPUTS	--		1) The text object
//							2) The starting character position (one-based)  default=1
//							3) The number index (one-based)										default=1
// OUTPUTS --		1) The number
//							2) The start position of the number
//							3) The next position
// FLOWS --			1=After Getting 2=If not found
///////////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI TextFunction_GetNumber(
  pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
  {
  AOBJECTID      					oiSource, oiPos, oiIndex, oiNumberOut, oiStartOut, oiNextOut;
  LPSTR         					szSource;
	UINT										iPos=1, iIndex=1, iNext;
	long double							flNumber;

	I_UNUSED(theEvent);

	theSystem->message1 = 2;    //default to not found

  //Get the source string
	oiSource = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiSource, 1, "TextFunction_GetNumber", return);

	if ((oiPos = AFuncGetTypedParameter(2, OTYPE_NUMBER)) != 0)
		iPos = (UINT)ONmbrGetInteger(oiPos);

	if ((oiIndex = AFuncGetTypedParameter(3, OTYPE_NUMBER)) != 0)
		iIndex = (UINT)ONmbrGetInteger(oiIndex);
	
	oiNumberOut = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiNumberOut, "TextFunction_GetNumber", return);

	oiStartOut = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiStartOut, "TextFunction_GetNumber", return);

	oiNextOut = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiNextOut, "TextFunction_GetNumber", return);

	szSource = LockTextString(oiSource);
	I_ASSERT_STRING(szSource, "TextFunction_GetNumber", return);

	if ( ParameterPosAndCountToActual(szSource, &iPos, NULL)  &&
	     GetNumber(szSource, &iPos, &iNext, iIndex, &flNumber))
		{
		ActualPosAndCountToParameter(szSource, &iPos, NULL);
		ActualPosAndCountToParameter(szSource, &iNext, NULL);
		ONmbrSetInteger(oiStartOut, iPos, TRUE);
		ONmbrSetInteger(oiNextOut, iNext, TRUE);
    ONmbrSetReal(oiNumberOut, flNumber, TRUE);
   	theSystem->message1 = 1;    //found
		}

  UnlockTextString(oiSource);

	AFuncSetParameter(1, oiNumberOut);
	AFuncSetParameter(2, oiStartOut);
	AFuncSetParameter(3, oiNextOut);
	}

///////////////////////////////////////////////////////////////////////////////////
// INPUTS	--		1) The text object
//							2) The starting character position (one-based)  default=1
//							3) The word index (one-based)										default=1
// OUTPUTS --		1) The word text
//							2) The start position of the word
//							3) The next position
// FLOWS --			1=After Getting 2=If not found
///////////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI TextFunction_GetWord(
  pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
  {
  AOBJECTID      					oiSource, oiPos, oiIndex, oiWordOut, oiStartOut, oiNextOut;
  LPSTR         					szSource;
	UINT										iPos=1, iIndex=1, iNext;

	I_UNUSED(theEvent);

  theSystem->message1 = 2;    //default to not found

	oiSource = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiSource, 1, "TextFunction_GetWord", return);

	if ((oiPos = AFuncGetTypedParameter(2, OTYPE_NUMBER)) != 0)
		iPos = (UINT)ONmbrGetInteger(oiPos);

	if ((oiIndex = AFuncGetTypedParameter(3, OTYPE_NUMBER)) != 0)
		iIndex = (UINT)ONmbrGetInteger(oiIndex);

	oiWordOut = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiWordOut, "TextFunction_GetWord", return);

	oiStartOut = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiStartOut, "TextFunction_GetWord", return);

	oiNextOut = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiNextOut, "TextFunction_GetWord", return);

	szSource = LockTextString(oiSource);
	I_ASSERT_STRING(szSource, "TextFunction_GetWord", return);

	if ( ParameterPosAndCountToActual(szSource, &iPos, NULL)  &&
	     GetWord(szSource, &iPos, &iNext, iIndex, oiWordOut))
		{
		ActualPosAndCountToParameter(szSource, &iPos, NULL);
		ActualPosAndCountToParameter(szSource, &iNext, NULL);
		ONmbrSetInteger(oiStartOut, iPos, TRUE);
		ONmbrSetInteger(oiNextOut, iNext, TRUE);
		theSystem->message1 = 1;    //found
    }

  UnlockTextString(oiSource);

	AFuncSetParameter(1, oiWordOut);
	AFuncSetParameter(2, oiStartOut);
	AFuncSetParameter(3, oiNextOut);
	}

///////////////////////////////////////////////////////////////////////////////////
// INPUTS	--		1) The destination text object (text to insert into)
//							2) The text to insert
//							3) The character position index (one-based)		default=1
///////////////////////////////////////////////////////////////////////////////////
extern "C" void ALMAPI TextFunction_InsertText(
  pAEvtInfo  							theEvent,
	pAObjMessage       			theSystem)
  {
	AOBJECTID        				oiSource, oiTarget, oiPos;
	LPSTR           				szSource, szTarget;
	UINT										uPos=1;
	UINT										uLenSource, uLenTarget;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;

	oiTarget = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiTarget, 1, "TextFunction_InsertText", return);

	oiSource = AFuncGetTypedParameter(2, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiSource, 2, "TextFunction_InsertText", return);

	if ((oiPos = AFuncGetTypedParameter(3, OTYPE_NUMBER)) != 0)
		uPos = (UINT)ONmbrGetInteger(oiPos);

	szSource = LockTextString(oiSource);
	I_ASSERT_STRING(szSource, "TextFunction_InsertText", return);
  uLenSource = lstrlen(szSource);

	szTarget = LockTextString(oiTarget);
	I_ASSERT_STRING(szTarget, "TextFunction_InsertText", UnlockTextString(oiSource);return);
	uLenTarget = lstrlen(szTarget);
	UnlockTextString(oiTarget);

	ParameterPosAndCountToActual(szTarget, &uPos, NULL);
	uPos = min(uPos, uLenTarget);

	if (ResizeTextString(oiTarget, uLenTarget+uLenSource+1))
		{
	  szTarget = LockTextString(oiTarget);
		I_ASSERT_STRING(szTarget, "TextFunction_InsertText", UnlockTextString(oiSource);return);

		MoveMemory(&szTarget[uPos+uLenSource], &szTarget[uPos], uLenTarget-uPos+1); // get the null terminator
		MoveMemory(&szTarget[uPos], szSource, uLenSource);
	  UnlockTextString(oiTarget);
		PostObjectChanged(oiTarget, 0L, DONTPOSTEDITED, NODELAY);
		}

	UnlockTextString(oiSource);
  }

////////////////////////////////////

extern "C" void ALMAPI TextFunction_Left(
  pAEvtInfo								theEvent,
	pAObjMessage    				theSystem)
  {
  AOBJECTID      					oiSource, oiCount, oiOut;
	LPSTR         					chp, szSource, szOut;
  UINT										u, uCount;
	long										lCount;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;    //only one exit flow

  ///Get the source text object
	oiSource = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiSource, 1, "TextFunction_Left", return);

	//Get the count (number of character to get)
	oiCount = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	I_VERIFY_PARAMETER(oiCount, 2, "TextFunction_Left", return);
	lCount = ONmbrGetInteger(oiCount);
	uCount = (lCount > 0L && lCount < 65536L) ? (UINT)lCount : 0;

	oiOut = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiOut, "TextFunction_Left", UnlockTextString(oiSource);return);

	szSource = LockTextString(oiSource);
	I_ASSERT_STRING(szSource, "TextFunction_Left", return);

	chp = szSource;
	u = 0;
	while(*chp  &&  u<uCount)
		{
		if (*chp == CR)
			uCount++;
		chp++;
		u++;
		}

	if (ResizeTextString(oiOut, uCount+1))
		{
		szOut = LockTextString(oiOut);
		I_ASSERT_STRING(szOut, "TextFunction_Left", UnlockTextString(oiSource);return);

		if (uCount)
///			lstrcpyn(szOut, szSource, uCount); // lstrcpyn is broken, it copies one less char than it should.
			strncpy(szOut, szSource, uCount);

		szOut[uCount] = 0;

		UnlockTextString(oiOut);
		}

	UnlockTextString(oiSource);

  AFuncSetParameter(1, oiOut);
	}

////////////////////////////////////

extern "C" void ALMAPI TextFunction_Load(
  pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
  {
	TEXTRUNTIME*						tr;
	LPTEXTOBJECT						txt;
	AOBJECTID   							oiPathIn, oiTextOut, oiPathOut;
  LPSTR      							szPathIn, szTextOut, szPathOut;
  HANDLE     							hf;
  DWORD       						dwFileSize, dwNumRead;
	OPENFILENAME    				of;

	I_UNUSED(theEvent);
  theSystem->message1 = 2;    //default to canceled

	tr = GetTextRuntime() ;
	I_ASSERT_TR(tr, "TextFunction_Load", return);

	//Create the text object for the output text
	oiTextOut = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiTextOut, "TextFunction_Load", return);

	//Create the text object for the path out
	oiPathOut = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiPathOut, "TextFunction_Load", return);

	//Get the input path
	oiPathIn = AFuncGetTypedParameter(1, OTYPE_TEXT);
	if (oiPathIn)
		{
		szPathIn = LockTextString(oiPathIn);
		I_ASSERT_STRING(szPathIn, "TextFunction_Load", return);

		if (ResizeTextString(oiPathOut, lstrlen(szPathIn)+1))
			{
			szPathOut = LockTextString(oiPathOut);
			lstrcpy(szPathOut, szPathIn);
			}
		else 
			szPathOut = NULL;

    UnlockTextString(oiPathIn);
		}
	else
		{//They didn't geva a path -- use the file picker
		if (ResizeTextString(oiPathOut, 129))
			{
			szPathOut = LockTextString(oiPathOut);

			FillMemory(&of, sizeof(OPENFILENAME), 0);
	    of.lStructSize        = sizeof(OPENFILENAME);
	    of.hwndOwner          = GetActiveWindow();
			of.lpstrFilter        = tr->GetFileFilter();
	    of.nFilterIndex       = 1;
	    of.lpstrFile          = szPathOut;
	    of.nMaxFile           = 128;
	    of.lpstrDefExt        = "txt";
	    of.Flags              = OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;
	    GetOpenFileName(&of);
			}
		else
			szPathOut = NULL;
		}

	if (szPathOut && *szPathOut)
  	{
		hf = CreateFile(szPathOut, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hf != INVALID_HANDLE_VALUE)
			{
			txt = LockTextStyle(oiTextOut);
			I_ASSERT_TXT(txt, "", return);

			dwFileSize = max((DWORD)txt->GetMaxNumChars(), SetFilePointer(hf, 0L, NULL, FILE_END));
			UnlockTextStyle(oiTextOut);
		
			if (ResizeTextString(oiTextOut, dwFileSize+1))
				{
				szTextOut = LockTextString(oiTextOut);
				I_ASSERT_STRING(szTextOut, "TextFunction_Load", UnlockTextString(oiPathOut);return);

				SetFilePointer(hf, 0L, NULL, FILE_BEGIN);
				ReadFile(hf, szTextOut, dwFileSize, &dwNumRead, NULL);
				szTextOut[dwNumRead] = 0;

				theSystem->message1 = 1;    //sucess

	      UnlockTextString(oiTextOut);
				}
			else
				theSystem->message1 = 3;    //error -- unable to resize the data

			CloseHandle(hf);
			}
		else
			theSystem->message1 = 3;    //error -- unable to open the file

	  UnlockTextString(oiPathOut);
		}

	AFuncSetParameter(1, oiTextOut);
	AFuncSetParameter(2, oiPathOut);
	}

////////////////////////////////////

extern "C" void ALMAPI TextFunction_Mid(
  pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
  {
	AOBJECTID      					oiSource, oiPos, oiCount, oiTextOut;
  LPSTR         					szSource, szTextOut;
	long										lCount, lPos;
	UINT										uPos, uCount;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;    //only one exit flow

	///Get the source text object
	oiSource = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiSource, 1, "TextFunction_Mid", return);

	//Get the starting position
	oiPos = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	I_VERIFY_PARAMETER(oiPos, 2, "TextFunction_Mid", return);

	oiCount = AFuncGetTypedParameter(3, OTYPE_NUMBER);
	I_VERIFY_PARAMETER(oiCount, 3, "TextFunction_Mid", return);
	
	oiTextOut = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiTextOut, "TextFunction_Mid", return);

	lPos = ONmbrGetInteger(oiPos);
	lCount = ONmbrGetInteger(oiCount);

	szSource = LockTextString(oiSource);
	I_ASSERT_STRING(szSource, "TextFunction_Mid", return);

	if (lCount < 0 )
		{
		lPos += lCount + 1;
		lCount *= -1;
		}

	uPos = (UINT)lPos;
	uCount = (UINT)lCount;

	ParameterPosAndCountToActual(szSource, &uPos, &uCount);

	if (ResizeTextString(oiTextOut, uCount+1))
		{
		szTextOut = LockTextString(oiTextOut);
		I_ASSERT_STRING(szTextOut, "TextFunction_Mid", UnlockTextString(oiSource);return);
		
		if (uCount)
///		  lstrcpyn(szTextOut, &szSource[uPos], uCount);	lstrcpyn not working properly.
		  strncpy(szTextOut, &szSource[uPos], uCount);
		szTextOut[uCount] = 0;

		UnlockTextString(oiTextOut);
		}

  UnlockTextString(oiSource);

	AFuncSetParameter(1, oiTextOut);
	}

////////////////////////////////////

extern "C" void ALMAPI TextFunction_Right(
  pAEvtInfo								theEvent,
	pAObjMessage  					theSystem)
  {
	AOBJECTID      					oiSource, oiCount, oiOut;
	LPSTR         					chp, szSource, szOut;
  UINT										u, uCount;
	long										lCount;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;    //only one exit flow

  ///Get the source text object
	oiSource = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiSource, 1, "TextFunction_Right", return);

	//Get the count (number of character to get)
	oiCount = AFuncGetTypedParameter(2, OTYPE_NUMBER);
	I_VERIFY_PARAMETER(oiCount, 2, "TextFunction_Right", return);
	lCount = ONmbrGetInteger(oiCount);
	uCount = (lCount > 0L && lCount < 65536L) ? (UINT)lCount : 0;

	oiOut = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiOut, "TextFunction_Right", return);

	szSource = LockTextString(oiSource);
	I_ASSERT_STRING(szSource, "TextFunction_Right", return);

	chp = &szSource[lstrlen(szSource)];
	u = 0;
	while(chp>szSource  &&  u<uCount)
		{
		chp--;
		if (*chp == LF)
			uCount++;
		u++;
		}

	if (ResizeTextString(oiOut, uCount+1))
		{
		szOut = LockTextString(oiOut);
		I_ASSERT_STRING(szOut, "TextFunction_Right", UnlockTextString(oiSource);return);
		
		lstrcpy(szOut, chp);
		
	  UnlockTextString(oiOut);
		}

	UnlockTextString(oiSource);

  AFuncSetParameter(1, oiOut);
	}

////////////////////////////////////

extern "C" void ALMAPI TextFunction_Save(
  pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
  {
	TEXTRUNTIME*						tr;
	AOBJECTID   							oiSource, oiPathIn, oiPathOut;
  LPSTR      							szSource, szPathIn, szPathOut;
  HANDLE      						hf;
  DWORD      							dwFileSize, dwNumWritten;
	OPENFILENAME    				of;

	I_UNUSED(theEvent);
  theSystem->message1 = 2;    //default to canceled

	tr = GetTextRuntime() ;
	I_ASSERT_TR(tr, "TextFunction_Save", return);

		///Get the source text object
	oiSource = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiSource, 1, "TextFunction_Save", return);
	szSource = LockTextString(oiSource);
	I_ASSERT_STRING(szSource, "TextFunction_Save", return);

	//Create the text object for the path out
	oiPathOut = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiPathOut, "TextFunction_Save", UnlockTextString(oiSource);return);

	//Get the input path
	oiPathIn = AFuncGetTypedParameter(3, OTYPE_TEXT);
	if (oiPathIn)
		{
		szPathIn = LockTextString(oiPathIn);
		I_ASSERT_STRING(szPathIn, "TextFunction_Save", UnlockTextString(oiSource);return);

		if (ResizeTextString(oiPathOut, lstrlen(szPathIn)+1))
			{
			szPathOut = LockTextString(oiPathOut);
			lstrcpy(szPathOut, szPathIn);
			}
		else
			szPathOut = NULL;

    UnlockTextString(oiPathIn);
		}
	else
		{//They didn't give a path -- use the file picker
		if (ResizeTextString(oiPathOut, 129))
			{
			szPathOut = LockTextString(oiPathOut);
			I_ASSERT_STRING(szPathOut, "TextFunction_Save", UnlockTextString(oiSource);return);

			FillMemory(&of, sizeof(OPENFILENAME), 0);
	    of.lStructSize        = sizeof(OPENFILENAME);
	    of.hwndOwner          = GetActiveWindow();
			of.lpstrFilter        = tr->GetFileFilter();
	    of.nFilterIndex       = 1;
	    of.lpstrFile          = szPathOut;
	    of.nMaxFile           = 128;
	    of.lpstrDefExt        = "txt";
	    of.Flags              = OFN_HIDEREADONLY|OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT;
	    GetSaveFileName(&of);
			}
		else
			szPathOut = NULL;
		}

  //Now, save the text..................

	if (szPathOut && *szPathOut)
  	{
		hf = CreateFile(szPathOut, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
		if (hf != INVALID_HANDLE_VALUE)
			{
			dwFileSize = lstrlen(szSource)+1;
			WriteFile(hf, szSource, dwFileSize, &dwNumWritten, NULL);
			CloseHandle(hf);
			theSystem->message1 = 1;    //sucess
			}
		else
			theSystem->message1 = 3;    //error

		UnlockTextString(oiPathOut);
		}

	UnlockTextString(oiSource);

	AFuncSetParameter(1, oiPathOut);
	}

////////////////////////////////////

extern "C" void ALMAPI TextFunction_SetFontSize(
  pAEvtInfo								theEvent,
	pAObjMessage  					theSystem)
  {
	AOBJECTID      					oiText, oiFace, oiSize;
	LPTEXTOBJECT			  					txt;
	char										szFace[LF_FACESIZE];
	LPATTRIBUTES						attr;
	long										height;
	long										changeFlag = 0L;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;    //only one exit flow

	oiText = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiText, 1, "TextFunction_SetFontSize", return);
	txt = LockTextStyle(oiText);
	I_ASSERT_TXT(txt, "TextFunction_SetFontSize", return);

	attr = txt->GetAttributes();

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
			oiText,
			txt->IsStatic() ? LiteTextItemChangeFont : HwndTextItemChangeFont,
			(LONG)attr,
			(LONG)changeFlag);
		}
	
	UnlockTextStyle(oiText);
	}

/////////////////////////////////////////

extern "C" void ALMAPI TextFunction_Length(
  pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
	{
	AOBJECTID      					oiText, oiLen;
	LPSTR         					szText;
  
	I_UNUSED(theEvent);

	theSystem->message1 = 1;    //only one exit flow

	oiText = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiText, 1, "TextFunction_Length", return);

	oiLen = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiLen, "TextFunction_Length", return);

	szText = LockTextString(oiText);
	I_ASSERT_STRING(szText, "TextFunction_Length", return);

	ONmbrSetInteger(oiLen, ParameterStringLength(szText), TRUE);

  UnlockTextString(oiText);
	AFuncSetParameter(1, oiLen);
  }

////////////////////////////////////

extern "C" void ALMAPI TextFunction_UpperCase(
  pAEvtInfo								theEvent,
  pAObjMessage  					theSystem)
  {
  AOBJECTID  							oiSource, oiOut;
  LPSTR     							szSource, szOut;

	I_UNUSED(theEvent);

	theSystem->message1 = 1;    //only one exit flow

  //get the input text object
	oiSource = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiSource, 1, "TextFunction_UpperCase", return);

	szSource = LockTextString(oiSource);
	I_ASSERT_STRING(szSource, "TextFunction_UpperCase", return);

  //create the text object to output
	oiOut = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiOut, "TextFunction_UpperCase", UnlockTextString(oiSource);return);

	if (ResizeTextString(oiOut, lstrlen(szSource)+1))
		{
	 	szOut = LockTextString(oiOut);
		I_ASSERT_STRING(szOut, "TextFunction_UpperCase", UnlockTextString(oiSource);return);

	  lstrcpy(szOut, szSource);
		AnsiUpper(szOut);
		UnlockTextString(oiOut);
		}

	UnlockTextString(oiSource);

  AFuncSetParameter(1, oiOut);
	}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
/////////////////////////////////////////////////
class TEXTLIST
	{
	private:
		int										m_numLines;
		AMEMBLOCKID  *			m_miLineArray;
		BOOL  *						m_bRemoveArray;
		TEXTLIST();

	public:
		TEXTLIST(AOBJECTID oiText);
		~TEXTLIST();

		BOOL RemoveDuplicatesWithinSelf();
		BOOL IsStringInLines(LPSTR string, int startingLine=0);

		BOOL RemoveLinesInOtherList(TEXTLIST  *otherList);
		BOOL RemoveLinesNotInOtherList(TEXTLIST  *otherList);

		BOOL AppendToMemblock(AMEMBLOCKID miBuffer);

		int CountLinesNotRemoved();
	};
typedef TEXTLIST  * LPTEXTLIST;

/////////

TEXTLIST::TEXTLIST(
	AOBJECTID								oiText)
	{
	LPSTR										szText, szLine, cpStart, cpEnd;
	int											lineNum, lineLen;
	BOOL										bDone;

	szText = LockTextString(oiText);
	I_ASSERT_STRING(szText, "TEXTLIST::TEXTLIST", ;);

	if (*szText)
		{
		for( cpStart=szText, m_numLines=1; *cpStart; cpStart++)
			{
			if( *cpStart=='\r'  &&  *(cpStart+1)=='\n'  &&  *(cpStart+2)!=0 )
				{
				m_numLines++;
				cpStart++;
				}
			}
	
	///	m_miLineArray = (AMEMBLOCKID  *)farmalloc(m_numLines*sizeof(AMEMBLOCKID) );
	///	m_bRemoveArray = (BOOL  *)farmalloc(m_numLines*sizeof(BOOL) );
		m_miLineArray = (AMEMBLOCKID  *)malloc(m_numLines*sizeof(AMEMBLOCKID) );
		m_bRemoveArray = (BOOL  *)malloc(m_numLines*sizeof(BOOL) );
	
		cpEnd = cpStart = szText;
		lineNum=0;
		bDone = FALSE;
	
		while(!bDone)
			{
			if (*cpEnd == '\r'  &&  *(cpEnd+1)=='\n')
				{
				lineLen = (int)(cpEnd-cpStart);
				m_miLineArray[lineNum] = AMemAllocate(lineLen+1);
				szLine = (LPSTR)AMemLock(m_miLineArray[lineNum]);
				CopyMemory(szLine, cpStart, lineLen);
				szLine[lineLen]=0;
				AMemUnlock(m_miLineArray[lineNum]);
				m_bRemoveArray[lineNum] = FALSE;
				lineNum++;
				cpEnd+=2;
				cpStart=cpEnd;
				}
	
			else if(*cpEnd==0)
				{
				lineLen = (int)(cpEnd-cpStart);
				if (lineLen)
					{
					m_miLineArray[lineNum] = AMemAllocate(lineLen+1);
					szLine = (LPSTR)AMemLock(m_miLineArray[lineNum]);
					CopyMemory(szLine, cpStart, lineLen);
					szLine[lineLen]=0;
					AMemUnlock(m_miLineArray[lineNum]);
					m_bRemoveArray[lineNum] = FALSE;
					lineNum++;
					}	 
				bDone = TRUE;
				}
			else
				cpEnd++;
	
			}
	
		ADBG_EVAL(
			if(lineNum != m_numLines)
				ADbgPrint(ADBG_ERROR, 0, "lineNum != m_numLines in TEXTLIST::TEXTLIST");
				)
		}
	else /*!*szText*/
		m_numLines=0;

	UnlockTextString(oiText);
	}

//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////

TEXTLIST::~TEXTLIST()
	{
	int											i;

	if (m_numLines)
		{
		for(i=0; i<m_numLines; i++)
			AMemFree(m_miLineArray[i]);

///		farfree(m_miLineArray);
///		farfree(m_bRemoveArray);
		free(m_miLineArray);
		free(m_bRemoveArray);
		}
	}

//////////////////////////////////////////////
BOOL TEXTLIST::IsStringInLines(
	LPSTR 									string, 
	int 										startingLine) //defaults to 0;
	{
	LPSTR										szLine;
	BOOL										rVal=FALSE;
	int											i;
	
	for(i=startingLine; i<m_numLines && !rVal; i++)
		{
		szLine = (LPSTR)AMemLock(m_miLineArray[i]);
		if (lstrcmp(szLine, string)==0)
			rVal=TRUE;
		AMemUnlock(m_miLineArray[i]);
		}

	return rVal;
	}
	
//////////////////////////////////////////////

BOOL TEXTLIST::RemoveDuplicatesWithinSelf()
	{
	int											i;
	LPSTR										szLine;

	for(i=0; i<m_numLines; i++)
		{
		if (!m_bRemoveArray[i])
			{
			szLine = (LPSTR)AMemLock(m_miLineArray[i]);
			if (IsStringInLines(szLine, i+1))
		 		m_bRemoveArray[i] = TRUE;
			AMemUnlock(m_miLineArray[i]);
			}
		}
	return TRUE;
	}

//////////////////////////////////////////////
BOOL TEXTLIST::RemoveLinesInOtherList(
	TEXTLIST  *					otherList)
	{
	int											i;
	LPSTR										szLine;

	for(i=0; i<m_numLines; i++)
		{
		if (!m_bRemoveArray[i])
			{
			szLine = (LPSTR)AMemLock(m_miLineArray[i]);
			if (otherList->IsStringInLines(szLine) )
		 		m_bRemoveArray[i] = TRUE; //string is in second list, so mark to remove
			AMemUnlock(m_miLineArray[i]);
			}
		}
	return TRUE;
	}

/////

BOOL TEXTLIST::RemoveLinesNotInOtherList(
	TEXTLIST  *					otherList)
	{
	int											i;
	LPSTR										szLine;

	for(i=0; i<m_numLines; i++)
		{
		if (!m_bRemoveArray[i])
			{
			szLine = (LPSTR)AMemLock(m_miLineArray[i]);
			if (!otherList->IsStringInLines(szLine) )
		 		m_bRemoveArray[i] = TRUE; //string is NOT in second list, so mark to remove
			AMemUnlock(m_miLineArray[i]);
			}
		}
	return TRUE;
	}

/////////////////////////////////////////

BOOL TEXTLIST::AppendToMemblock(
	AMEMBLOCKID							miBuffer)
	{
	int											spaceLeft = 0x7FFF; /*32k*/
	LPSTR										szBuffer;
	LPSTR										szLine;
	int											lenLine;
	int											i;
	BOOL										bZeroStringFirst=FALSE;

	if (AMemGetSize(miBuffer) == 0)
		bZeroStringFirst = TRUE;

	AMemSetSize(miBuffer, 0x7FFFL/*32K*/);

	szBuffer = (LPSTR)AMemLock(miBuffer);
	if (bZeroStringFirst)
		*szBuffer = 0;

	for(i=0; i<m_numLines && spaceLeft; i++)
		{
		if (!m_bRemoveArray[i])
			{
			if (!bZeroStringFirst)
				{
				strcat(szBuffer, "\r\n");
				spaceLeft -= 2;
				}
			bZeroStringFirst = FALSE;

			szLine = (LPSTR)AMemLock(m_miLineArray[i]);
			lenLine = lstrlen(szLine);
			if (spaceLeft - lenLine > 0)
				{
				strcat(szBuffer, szLine);
				spaceLeft -= lenLine;
				}
			else
				{
				szLine[spaceLeft-1]=0;
				strcat(szBuffer, szLine);
				spaceLeft=0;
				}
			AMemUnlock(m_miLineArray[i]);
			}
		}

	AMemUnlock(miBuffer);
	return TRUE;
	}

//////////////////////////////////

int TEXTLIST::CountLinesNotRemoved()
	{
	int											numLinesNotRemoved=0;
	int											i;

	for(i=0; i<m_numLines; i++)
		{
		if (!m_bRemoveArray[i])	
			numLinesNotRemoved++;
		}

	return numLinesNotRemoved;
	}

///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////

extern "C" void ALMAPI TextFunction_IntersectText(
  pAEvtInfo								theEvent,
	pAObjMessage    				theSystem)
	{
	AOBJECTID 								oiFirst, oiSecond, oiResult, oiCount;
	LPTEXTLIST							list1, list2;
	AMEMBLOCKID							miBuffer;

	theSystem->message1 = 1;    //only one possible outflow
	I_UNUSED(theEvent);			

	oiFirst = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiFirst, 1, "TextFunction_IntersectText", return);

	oiSecond = AFuncGetTypedParameter(2, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiSecond, 2, "TextFunction_IntersectText", return);

	oiResult = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiResult, "TextFunction_IntersectText", return);

	oiCount = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiCount, "TextFunction_IntersectText", return);

	list1 = new TEXTLIST(oiFirst);
	list2 = new TEXTLIST(oiSecond);

	list1->RemoveDuplicatesWithinSelf();
	list1->RemoveLinesNotInOtherList(list2);

	miBuffer = AMemAllocate(0);
	list1->AppendToMemblock(miBuffer);
	OTextSetValueFromMemblock(oiResult, miBuffer);
	AMemFree(miBuffer);

	ONmbrSetInteger(oiCount, list1->CountLinesNotRemoved(), TRUE);

	delete list1;
	delete list2;

	AFuncSetParameter(1L, oiResult);
	AFuncSetParameter(2L, oiCount);
	}

/////////////////////////////////////////////////

extern "C" void ALMAPI TextFunction_MergeText(
  pAEvtInfo								theEvent,
	pAObjMessage  					theSystem)
	{
	AOBJECTID								oiFirst, oiSecond, oiResult, oiCount;
	LPTEXTLIST							list1, list2;
	AMEMBLOCKID							miBuffer;

	theSystem->message1 = 1;    //only one possible outflow
	I_UNUSED(theEvent);

	oiFirst = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiFirst, 1, "TextFunction_MergeText", return);

	oiSecond = AFuncGetTypedParameter(2, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiSecond, 2, "TextFunction_MergeText", return);

	oiResult = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiResult, "TextFunction_MergeText", return);

	oiCount = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiCount, "TextFunction_MergeText", return);

	list1 = new TEXTLIST(oiFirst);
	list2 = new TEXTLIST(oiSecond);

	list1->RemoveDuplicatesWithinSelf();
	list2->RemoveDuplicatesWithinSelf();

	list2->RemoveLinesInOtherList(list1);
	
	miBuffer = AMemAllocate(0);
	list1->AppendToMemblock(miBuffer);
	list2->AppendToMemblock(miBuffer);
	OTextSetValueFromMemblock(oiResult, miBuffer);
	AMemFree(miBuffer);

	ONmbrSetInteger(oiCount, list1->CountLinesNotRemoved() + list2->CountLinesNotRemoved(), TRUE);

	delete list1;
	delete list2;

	AFuncSetParameter(1L, oiResult);
	AFuncSetParameter(2L, oiCount);
	}


/////////////////////////////////////////////////

extern "C" void ALMAPI TextFunction_OmitText(
  pAEvtInfo								theEvent,
	pAObjMessage    				theSystem)
	{
	AOBJECTID								oiFirst, oiSecond, oiResult, oiCount;
	LPTEXTLIST							list1, list2;
	AMEMBLOCKID							miBuffer;

	theSystem->message1 = 1;    //only one possible outflow
	I_UNUSED(theEvent);

	oiFirst = AFuncGetTypedParameter(1, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiFirst, 1, "TextFunction_OmitText", return);

	oiSecond = AFuncGetTypedParameter(2, OTYPE_TEXT);
	I_VERIFY_PARAMETER(oiSecond, 2, "TextFunction_OmitText", return);

	oiResult = AObjCreate(OTYPE_TEXT);
	I_VERIFY_PSUEDO(oiResult, "TextFunction_OmitText", return);

	oiCount = AObjCreate(OTYPE_NUMBER);
	I_VERIFY_PSUEDO(oiCount, "TextFunction_OmitText", return);

	list1 = new TEXTLIST(oiFirst);
	list2 = new TEXTLIST(oiSecond);

	list2->RemoveDuplicatesWithinSelf();

	list2->RemoveLinesInOtherList(list1);

	miBuffer = AMemAllocate(0);
	list2->AppendToMemblock(miBuffer);
	OTextSetValueFromMemblock(oiResult, miBuffer);
	AMemFree(miBuffer);
	
	ONmbrSetInteger(oiCount, list2->CountLinesNotRemoved(), TRUE);

	delete list1;
	delete list2;

	AFuncSetParameter(1L, oiResult);
	AFuncSetParameter(2L, oiCount);
	}

/////////////////////////////////////////////////
