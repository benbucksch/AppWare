////////////////////////////////////////////////////////////////////////////
//
// Copyright 1993, Serius Corp.
//	"flop_ini.cpp"
//
//	Written by: Kevin J. Smart
//
////////////////////////////////////////////////////////////////////////////

#include	<windows.h>
#include	<string.h>

#include	<a_alm.h>
#include 	<o_text.h>
#include	"flop.h"

// Flop_ReadIni ////////////////////////////////////////////////////////////
//
// Function: rIni
//
// Input 1: Full Pathname for the *.ini file
// Input 2: Section name
// Input 3: Entry name
// Input 4: Default Name
//
// Output 1: Profile string
//
// Flow 1: After reading
// Flow 2: If error

void far pascal _export Flop_ReadIni(pAEvtInfo,	pAObjMessage theSystem)

	{
	OBJECTID pathObj = AFuncGetTypedParameter(1L, OTYPE_TEXT);
	OBJECTID sectObj = AFuncGetTypedParameter(2L, OTYPE_TEXT);
	OBJECTID etryObj = AFuncGetTypedParameter(3L, OTYPE_TEXT);
	OBJECTID dfltObj = AFuncGetTypedParameter(4L, OTYPE_TEXT);

	MEMBLOCKID pathID = NULL;
	MEMBLOCKID sectID = NULL;
	MEMBLOCKID etryID = NULL;
	MEMBLOCKID dfltID = NULL;

	char* path = NULL;
	char* section = NULL;
	char* entry = NULL;
	char* defalt = NULL;

	BOOL error = FALSE;

	if(sectObj)
		{
		sectID = AMemAllocate(0L);
		OTextGetToMemblock(sectObj, sectID);
		section = (char*)AMemLock(sectID);
		}
	else
		{ // ERROR!!! this is a required parameter!
		ADBG_PRINT_E("The 'Section' parameter is required in the Read *.ini functions") ;
		theSystem->message1 = 2 ;
		return;
		}

	if(pathObj)
		{
		pathID = AMemAllocate(0L);
		OTextGetToMemblock(pathObj, pathID);
		path = (char*)AMemLock(pathID);
		}

	if(etryObj)
		{
		etryID = AMemAllocate(0L);
		OTextGetToMemblock(etryObj, etryID);
		entry = (char*)AMemLock(etryID);
		}

	if(dfltObj)
		{
		dfltID = AMemAllocate(0L);
		OTextGetToMemblock(dfltObj, dfltID);
		defalt = (char*)AMemLock(dfltID);
		}

	// do the actual reading of *.ini file.
	MEMBLOCKID	rsltID = AMemAllocate(0x00001000L);
	char* result = (char*)AMemLock(rsltID);
	int	size;
	if(path)
		{
		if(defalt)
			size = GetPrivateProfileString(section, entry, defalt, result, 0x1000, path);
		else
			{
			size = GetPrivateProfileString(section, entry, "{¶¶¤¤§§}", result, 0x1000, path);

			if(!strcmp(result, "{¶¶¤¤§§}"))
			error = TRUE;
			}
		}
	else
		{
		// use the win.ini file
		if(defalt)
			size = GetProfileString(section, entry, defalt, result, 0x1000);
		else
			{
			size = GetProfileString(section, entry, "{¶¶¤¤§§}", result, 0x1000);

			if(!strcmp(result, "{¶¶¤¤§§}"))
			error = TRUE;
			}
		}

	if (!size)
		error = TRUE;

	if((!error) && (!entry))
		{
		
	// a whole section was retrieved
		MEMBLOCKID	tmpBlock = AMemAllocate(2L * size);
		char* tmpStr = (char*)AMemLock(tmpBlock);

		int	i = 0;
		int	j = 0;
		while(i<(size-1))
			{
			if(*(result+i))
				*(tmpStr+i+j) = *(result + i);
			else
				{
				*(tmpStr+i+j) = '\r';
				j++;
				*(tmpStr+i+j) = '\n';
				}
			i++;
			}

		*(tmpStr+i+j) = 0;
		AMemUnlock(rsltID) ;
		AMemFree(rsltID);
		rsltID = tmpBlock;
		result = tmpStr;
	
		}

	if(error)
		theSystem->message1 = 2 ;
	else
		{
		OBJECTID theResult = AObjCreate(OTYPE_TEXT);
		OTextSetValueFromBuffer(theResult, result);
		AFuncSetParameter(1L, theResult);
		theSystem->message1 = 1 ;
		}

	if(rsltID)
		{
		AMemUnlock(rsltID) ;
		AMemFree(rsltID);
		}
	if(pathID)
		{
		AMemUnlock(pathID) ;
		AMemFree(pathID);
		}
	if(sectID)
		{
		AMemUnlock(sectID) ;
		AMemFree(sectID);
		}
	if(etryID)
		{
		AMemUnlock(etryID) ;
		AMemFree(etryID);
		}
	if(dfltID)
		{
		AMemUnlock(dfltID) ;
		AMemFree(dfltID);
		}
	}

// Flop_WriteIni ///////////////////////////////////////////////////////////
//
// Function: wIni
//
// Input 1: Full Pathname for the *.ini file
// Input 2: Section name
// Input 3: Entry name
// Input 4: Profile String
//
// Flow 1: After writing
// Flow 2: If error

void far pascal _export Flop_WriteIni(pAEvtInfo, pAObjMessage theSystem)
	{
	OBJECTID pathObj = AFuncGetTypedParameter(1L, OTYPE_TEXT);
	OBJECTID sectObj = AFuncGetTypedParameter(2L, OTYPE_TEXT);
	OBJECTID etryObj = AFuncGetTypedParameter(3L, OTYPE_TEXT);
	OBJECTID profObj = AFuncGetTypedParameter(4L, OTYPE_TEXT);

	MEMBLOCKID pathID = NULL;
	MEMBLOCKID sectID = NULL;
	MEMBLOCKID etryID = NULL;
	MEMBLOCKID profID = NULL;

	char* path = NULL;
	char* section = NULL;
	char* entry = NULL;
	char* profile = NULL;

	int	result;

	if(sectObj)
		{
		sectID = AMemAllocate(0L);
		OTextGetToMemblock(sectObj, sectID);
		section = (char*)AMemLock(sectID);
		}
	else
		{ // ERROR!!! this is a required parameter!
		ADBG_PRINT_E("The 'Section' parameter is required in the Write *.ini functions") ;
		theSystem->message1 = 2 ;
		return;
		}

	if(pathObj)
		{
		pathID = AMemAllocate(0L);
		OTextGetToMemblock(pathObj, pathID);
		path = (char*)AMemLock(pathID);
		}

	if(etryObj)
		{
		etryID = AMemAllocate(0L);
		OTextGetToMemblock(etryObj, etryID);
		entry = (char*)AMemLock(etryID);
		}

	if(profObj)
		{
		profID = AMemAllocate(0L);
		OTextGetToMemblock(profObj, profID);
		profile = (char*)AMemLock(profID);
		}

	// do the actual writing to the *.ini file.
	if(path)
		result = WritePrivateProfileString(section, entry, profile, path);
	else	// use the win.ini file
		result = WriteProfileString(section, entry, profile);

	if(result==0)
		theSystem->message1 = 2 ; // error occurred
	else
		theSystem->message1 = 1 ; // successfull

	if(pathID)
		{
		AMemUnlock(pathID) ;
		AMemFree(pathID) ;
		}
	if(sectID)
		{
		AMemUnlock(sectID) ;
		AMemFree(sectID);
		}
	if(etryID)
		{
		AMemUnlock(etryID) ;
		AMemFree(etryID);
		}
	if(profID)
		{
		AMemUnlock(profID) ;
		AMemFree(profID);
		}
	}
////////////////////////////////////////////////////////////////////////////











