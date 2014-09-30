

#include "t_includ.h"


extern "C" int ALMAPI OTextGetToBuffer(
	AOBJECTID 								oiText,
	LPSTR 									szBuffer,
	int 										bufferLen)
	{
	LPSTR										string;

	I_VERIFY_TYPE_FOR_API(oiText, "OTextGetToBuffer", return 0);
  I_VERIFY_FOR_API(bufferLen>0, "OTextGetToBuffer", return 0);

	string = LockTextString(oiText);
	I_ASSERT_STRING(string, "OTextGetToBuffer", return 0);

	lstrcpyn(szBuffer, string, bufferLen);
	szBuffer[bufferLen-1] = 0;

	UnlockTextString(oiText);

  return lstrlen(szBuffer)+1;
	}

///////////////////////////////////////////////////

extern "C" int ALMAPI OTextGetToMemblock(
	AOBJECTID 								oiText,
	AMEMBLOCKID 							miBuffer)
	{
	LPSTR										string, szBuffer;
	int											len;
  int											rVal;

	I_VERIFY_TYPE_FOR_API(oiText, "OTextGetToMemblock", return 0);
  I_VERIFY_FOR_API(miBuffer!=0, "OTextGetToMemblock", return 0);

	string = LockTextString(oiText);
	I_ASSERT_STRING(string, "OTextGetToMemblock", return 0);

	len = lstrlen(string)+1;
	if (AMemSetSize(miBuffer, len))
		{
		szBuffer = (LPSTR)AMemLock(miBuffer);
		lstrcpy(szBuffer, string);
		AMemUnlock(miBuffer);
		rVal = len;
		}
	else
		rVal = 0;

	UnlockTextString(oiText);

  return rVal;
	}

///////////////////////////////////////////////////

extern "C" BOOL ALMAPI OTextSetValueFromBuffer(
	AOBJECTID 								oiText,
	LPSTR 									szBuffer)
	{
  LPSTR										string;
	int											len;
  BOOL										rVal;

	I_VERIFY_TYPE_FOR_API(oiText, "OTextSetValueFromBuffer", return FALSE);
  I_VERIFY_FOR_API(szBuffer!=NULL, "OTextSetValueFromBuffer", return 0);


	if (OTextCompareWithBuffer(oiText, szBuffer))
		{	//Well, since they are not the same, make them the same
		len = lstrlen(szBuffer)+1;
		if (ResizeTextString(oiText, len))
			{
			string = LockTextString(oiText);
			I_ASSERT_STRING(string, "OTextSetValueFromBuffer", return FALSE);
			lstrcpy(string, szBuffer);
			UnlockTextString(oiText);

			PostObjectChanged(oiText, 0L, DONTPOSTEDITED, NODELAY);
	    rVal = TRUE;
			}
		else
  		rVal = FALSE;
    }
	else
 		rVal = FALSE;


	return rVal;
  }

///////////////////////////////////////////////////

extern "C" BOOL ALMAPI OTextSetValueFromMemblock(
	AOBJECTID 								oiText,
	AMEMBLOCKID 							miBuffer)
	{
	LPSTR										szBuffer;
  BOOL										rVal;

	I_VERIFY_TYPE_FOR_API(oiText, "OTextSetValueFromMemblock", return FALSE);
  I_VERIFY_FOR_API(miBuffer!=0, "OTextSetValueFromMemblock", return 0);

	szBuffer =(LPSTR)AMemLock(miBuffer);
	if (szBuffer)
		rVal = OTextSetValueFromBuffer(oiText, szBuffer);
	else
		rVal = FALSE;

	AMemUnlock(miBuffer);
	return rVal;
	}

///////////////////////////////////////////////////

int ALMAPI OTextGetToBufferEx(
	AOBJECTID 								oiText, 
	LPSTR 									szBuffer, 
	int 										bufferLen, 
	OTEXT_CRLF_TYPE 				fCrLf)
	{
	LPSTR										string;
	LPSTR										chpSource, chpDest;
	int											i, rVal;
	char										chSkip;

	I_VERIFY_TYPE_FOR_API(oiText, "OTextGetToBufferEx", return 0);
  I_VERIFY_FOR_API(bufferLen>0, "OTextGetToBufferEx", return 0);
  I_VERIFY_FOR_API(szBuffer, "OTextGetToBufferEx", return 0);

	string = LockTextString(oiText);
	I_ASSERT_STRING(string, "OTextGetToBufferEx", return 0);


	if (fCrLf == OTEXT_CR)
		chSkip = LF;
	else if (fCrLf == OTEXT_LF)
		chSkip = CR;
	else
		chSkip = 0;

	i=1;
	chpSource = string;
	chpDest = szBuffer;

	while( i < bufferLen  &&  *chpSource)
		{
		if (*chpSource != chSkip)
			{
			*chpDest = *chpSource;
			chpDest++;
			i++;
			}
		chpSource++;
		}
	*chpDest = 0;
	rVal = i;

	UnlockTextString(oiText);

  return rVal;
	}

//////////////////////////////////////////////////

int ALMAPI OTextGetToMemblockEx(
	AOBJECTID 								oiText, 
	AMEMBLOCKID 							miBuffer, 
	OTEXT_CRLF_TYPE 				fCrLf)
	{
	LPSTR										string, szDest;
	LPSTR										chpSource;
	int											rVal=0;
	int											memblockLen;
	char										chSkip;

	I_VERIFY_TYPE_FOR_API(oiText, "OTextGetToMemblockEx", return 0);

	string = LockTextString(oiText);
	I_ASSERT_STRING(string, "OTextGetToMemblockEx", return 0);

	if (fCrLf == OTEXT_CR)
		chSkip = LF;
	else if (fCrLf == OTEXT_LF)
		chSkip = CR;
	else
		chSkip = 0;

	///First, figure out how big the memblock will have to be
	memblockLen=1;
	chpSource=string;
	while(*chpSource)
		{
		if (*chpSource != chSkip)
			memblockLen++;
		chpSource++;
		}

	//Second, resize the memblock and get a pointer to it
	if (AMemSetSize(miBuffer, memblockLen))
		{
		szDest = (LPSTR)AMemLock(miBuffer);
		rVal = OTextGetToBufferEx(oiText, szDest, memblockLen, fCrLf);
		AMemUnlock(miBuffer);
		}

	UnlockTextString(oiText);

  return rVal;
	}


//////////////////////////////////////////////////

BOOL ALMAPI OTextSetValueFromBufferEx(
	AOBJECTID								oiText, 
	LPSTR 									szBuffer)
	{
	WORD										textLength;
	LPSTR										szText;
	LPSTR										chpSource, chpDest;
	char										prevChar, thisChar, nextChar;

	I_VERIFY_TYPE_FOR_API(oiText, "OTextSetValueFromBufferEx", return FALSE);
  I_VERIFY_FOR_API(szBuffer, "OTextSetValueFromBufferEx", return FALSE);

	//First, figure out how long the text object buffer will have to be
	textLength = 1;
	chpSource = szBuffer;
	prevChar = 0;
	thisChar = *chpSource;
	while(thisChar)
		{
		chpSource++;
		nextChar = *chpSource;

		if ( (thisChar == CR && nextChar != LF)  ||  (thisChar == LF && prevChar != CR) )
			textLength++;

		textLength++;

		prevChar = thisChar;
		thisChar = nextChar;
		}

	//Now we have the size of text object will have to be

	ResizeTextString(oiText, textLength);
	szText = LockTextString(oiText);
	I_ASSERT_STRING(szText, "OTextSetValueFromBufferEx", return FALSE);

	chpSource = szBuffer;
	chpDest = szText;
	prevChar = 0;
	thisChar = *chpSource;
	while(thisChar)
		{
		chpSource++;
		nextChar = *chpSource;

		if ( (thisChar == CR && nextChar != LF)  ||  (thisChar == LF && prevChar != CR) )
			{
			*chpDest = CR;
			chpDest++;
			*chpDest = LF;
			}
		else 
			*chpDest = thisChar;

		chpDest++;
		
		prevChar = thisChar;
		thisChar = nextChar;
		}
	*chpDest = 0;

	UnlockTextString(oiText);
	PostObjectChanged(oiText, 0L, DONTPOSTEDITED, NODELAY);
	
	return TRUE;
	}

//////////////////////////////////////////////////

BOOL ALMAPI OTextSetValueFromMemblockEx(
	AOBJECTID								oiText, 
	AMEMBLOCKID 							miBuffer)
	{
	LPSTR										szBuffer;
  BOOL										rVal;

	I_VERIFY_TYPE_FOR_API(oiText, "OTextSetValueFromMemblockEx", return FALSE);
  I_VERIFY_FOR_API(miBuffer!=0, "OTextSetValueFromMemblockEx", return FALSE);

	szBuffer =(LPSTR)AMemLock(miBuffer);
	if (szBuffer)
		rVal = OTextSetValueFromBufferEx(oiText, szBuffer);
	else
		rVal = FALSE;

	AMemUnlock(miBuffer);
	return rVal;
	}



///////////////////////////////////////////////////

extern "C" int ALMAPI OTextGetLength(
	AOBJECTID 								oiText)
	{
	LPSTR										string;
  int											rVal;

	I_VERIFY_TYPE_FOR_API(oiText, "OTextGetLength", return 0);

	string = LockTextString(oiText);
	I_ASSERT_STRING(string, "OTextGetLength", return 0);

	rVal = lstrlen(string);		// the len instead of the size
	UnlockTextString(oiText);
  return rVal;
	}


//////////////////////////////////////////////////
extern "C" int ALMAPI OTextGetLimit(
	AOBJECTID 								oiText)
	{
	TEXTOBJECT*										txt;
	int														rVal;

	I_VERIFY_TYPE_FOR_API(oiText, "OTextGetLimit", return 0);

	txt = LockTextStyle(oiText);
	I_ASSERT_TXT(txt, "OTextGetLimit", return 0);

	rVal = (WORD)txt->GetMaxNumChars();

	UnlockTextStyle(oiText);

  return rVal;
	}


extern "C" int ALMAPI OTextCompareWithBuffer(
	AOBJECTID 								oiText,
	LPSTR 									szBuffer)
	{
	LPSTR										string;
  int											rVal;

	I_VERIFY_TYPE_FOR_API(oiText, "OTextCompareWithBuffer", return 0);
  I_VERIFY_FOR_API(szBuffer!=NULL, "OTextCompareWithBuffer", return 0);


	string = LockTextString(oiText);
	I_ASSERT_STRING(string, "OTextCompareWithBuffer", return 0);

	rVal = lstrcmp(string, szBuffer);
	UnlockTextString(oiText);

  return rVal;
  }


extern "C" int ALMAPI OTextCompareWithMemblock(
	AOBJECTID								oiText,
	AMEMBLOCKID 							miBuffer)
	{
	LPSTR										szBuffer;
  int											rVal;

	I_VERIFY_TYPE_FOR_API(oiText, "OTextCompareWithMemblock", return 0);
  I_VERIFY_FOR_API(miBuffer!=0, "OTextCompareWithMemblock", return 0);

	szBuffer = (LPSTR)AMemLock(miBuffer);
	if (szBuffer)
		rVal = OTextCompareWithBuffer(oiText, szBuffer);
	else
		rVal = 0;

	AMemUnlock(miBuffer);
	return rVal;
  }
