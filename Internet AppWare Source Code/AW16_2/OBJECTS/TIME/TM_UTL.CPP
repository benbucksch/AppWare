#define STRICT

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <alloc.h>

#include "tm_incld.h"
#include "tm_dialg.h"
#include "tm_utl.h"


LONG GetDlgEditTime(HWND	hWnd,	int	item,	LONG style)
{
	int				len = (int)SendDlgItemMessage(hWnd, item, WM_GETTEXTLENGTH, 0, 0L) + 1;
	LPSTR			str = (LPSTR)farmalloc(len);
	LONG			time;

	SendDlgItemMessage(hWnd, item, WM_GETTEXT, len, (LONG)str);

	time = ParseTime(str);
	farfree(str);

	SetDlgEditTime(hWnd, item, time, style);

	return time;
}

void SetDlgEditTime(HWND hWnd, int item, LONG	time,	LONG style)
{
	char			str[100];

  FormatTime(time, style, str, sizeof(str) );
	SendDlgItemMessage(hWnd, item, WM_SETTEXT, 0, (LONG)str);
}

LONG ParseTime(LPSTR str)
{
	BOOL 					isPM = FALSE;
	TIME					time = {0,0,0};
	LPSTR					cp = str;

	if (str)
  {
		if ( SearchForSubstring(str, g_pmProfileSting) )
			isPM = TRUE;

		cp = GoToNextNumber(cp, FALSE);
		time.ti_hour = atoi(cp);

		if (time.ti_hour == 12  &&  !isPM)
		{//if the string is "12:--" and no "am" or "pm" entered, default to pm
			if (!SearchForSubstring(str, g_amProfileSting) )
			isPM = TRUE;
		}

		cp = GoToNextNumber(cp, TRUE);
		time.ti_min = atoi(cp);

		cp = GoToNextNumber(cp, TRUE);
		time.ti_sec = atoi(cp);

		///  Validate the time

		if (time.ti_sec > 59)
		{
			time.ti_min += time.ti_sec/60;
			time.ti_sec %= 60;
		}
		if (time.ti_min > 59)
		{
			time.ti_hour += time.ti_min/60;
			time.ti_min %= 60;
			if (time.ti_hour == 12) isPM = TRUE;
		}

		// Noon == 12:00 pm //

		if (isPM && time.ti_hour<12 && time.ti_hour>0 )
			time.ti_hour += 12;
		if (!isPM && time.ti_hour == 12)
			time.ti_hour = 0;
		time.ti_hour %= 24;
	}
	
	return TimeToSec(time);
}

void FormatTime(LONG time, LONG	style, LPSTR str,	int	len)
{
	TIME			theTime = SecToTime(time);
	LPSTR 		cp = str;
	BOOL			isAM = FALSE;
	BOOL			doSec = ((style & TM_SECONDS)!=0);
	BOOL			is24;

	I_UNUSED(len); 

	if (style & TM_DEFHOUR)
		is24 = g_is24hourProfileBool;
	else is24 = ((style & TM_24HOURS)!=0);

	if (!is24)
	{
		if (theTime.ti_hour == 0)
		{
			theTime.ti_hour = 12;
			isAM = TRUE;
		}
		else if (theTime.ti_hour>12)
			theTime.ti_hour %= 12;
		else if (theTime.ti_hour<12)
			isAM = TRUE;
	}

	if (g_leadingZeroProfileBool)	
		sprintf(cp, "%02d", theTime.ti_hour);
	else												
		sprintf(cp, "%d", theTime.ti_hour);

	cp += lstrlen(cp);
	lstrcat(cp, g_timeSepProfileChar);
	cp += lstrlen(cp);

	sprintf(cp, "%02d", theTime.ti_min);
	cp += lstrlen(cp);

	if (doSec)
	{
		lstrcat(cp, g_timeSepProfileChar);
		cp += lstrlen(cp);
		sprintf(cp, "%02d", theTime.ti_sec);
		cp += lstrlen(cp);
	}

	if (!is24)
	{
		*(cp++) = ' '; *cp = 0;
		if (isAM)		
			lstrcat(cp, g_amProfileSting);
		else				
			lstrcat(cp, g_pmProfileSting);
	}
}

LONG TimeToSec(TIME	time)
{
	LONG   sec;

	sec =  (LONG)time.ti_sec;
	sec += (LONG)time.ti_min * 60L;
	sec += (LONG)time.ti_hour * 3600L;

	return sec;
}

TIME SecToTime(LONG	sec)
{
	TIME		time;
	LONG		hours, minutes;

	if (sec < 0)
	{
		sec = 86400L + sec;	// if there's negatice seconds, add 24 hours 
	}

	hours		= sec/3600L;				sec -= hours * 3600L;
	minutes = sec/60L;					sec -= minutes * 60L;

	time.ti_hour		= (unsigned char) hours;
	time.ti_min			= (unsigned char) minutes;
	time.ti_sec			= (unsigned char) sec;
	time.ti_hund 		= 0;

	return time;
}

LPSTR SearchForSubstring(LPSTR s1,	LPSTR	s2)
{
	LPSTR		cp1=s1;
	LPSTR		cp2=s2;
	LPSTR		cpReturn=NULL;

	while(*cp1)
	{
		if (!*cp2) return cpReturn;

		if ( (*cp1|0x20) == (*cp2|0x20) )
		{
			if (!cpReturn) cpReturn = cp1;
			cp1++;
			cp2++;
		}
		else
		{
			cpReturn = NULL;
			cp1++;
			cp2 = s2;
		}
	}

	return cpReturn;
}

LPSTR GoToNextNumber(LPSTR str,	BOOL incFirst)  //increment first
{
	if (incFirst)
	{
		if (*str) 
			str = IncrementToNonQuote(str);
		while ( isdigit(*str) && *str) 
			str = IncrementToNonQuote(str);
	}

	while( !isdigit(*str) && *str) 
		str = IncrementToNonQuote(str); //go to next number

	return str;
}

LPSTR	IncrementToNonQuote(LPSTR	str)
{
	if (*str == '\'')
	{
		if (*str) str++;
		while ( *str != '\'' && *str) str++;
		if (*str) str++;
	}
	else 
		str++;

	return str;
}

LONG ParseTimeString(LPSTR str, LPLONG	whatsupplied)
{
	long  rVal=FALSE;
	LPSTR cp=str;

	*whatsupplied = 0;
	if (str)
	{
		cp = GoToNextNumber(cp, FALSE);
		if (cp)
			*whatsupplied |= OTIME_HOUR_SUPPLIED;

		cp = GoToNextNumber(cp, TRUE);
		if (cp)
			*whatsupplied |= OTIME_MIN_SUPPLIED;

		cp = GoToNextNumber(cp, TRUE);
		if (cp)
			*whatsupplied |= OTIME_SEC_SUPPLIED;

		rVal = TRUE;
	}

	return rVal;

}


