#define STRICT

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>																
#include <alloc.h>

#include "dt_dialg.h"
#include "dt_incld.h"

inline isDateChar(char c) {c |= 0x20; return (c=='m' || c=='d' || c=='y'); }

LPSTR	IncrementToNonQuote(
	LPSTR		str)
	{
	if (*str == '\'')
		{
		if (*str) str++;
		while ( *str != '\'' && *str) str++;
		if (*str) str++;
		}
	else str++;
	return str;
	}

LPSTR GoToNextNumber(
	LPSTR			str,
	BOOL			incFirst)  //increment first
	{
	if (incFirst)
		{
		if (*str) str = IncrementToNonQuote(str);
		while ( isdigit(*str) && *str) str = IncrementToNonQuote(str);
		}
	while( !isdigit(*str) && *str) str = IncrementToNonQuote(str); //go to next number
	return str;
	}

LPSTR GoToNextDateChar(
	LPSTR			str,
	BOOL			incFirst)  //increment first
	{
	if (incFirst)
		{
		if (*str) str = IncrementToNonQuote(str);
		while ( isDateChar(*str) && *str) str = IncrementToNonQuote(str);
		}
	while( !isDateChar(*str) && *str) str = IncrementToNonQuote(str);//go to next char
	return str;
	}

///////////////////////////////////

LONG ValidateDate(
	LONG			date)
	{
	return DateToJulian( (JulianToDate(date) ) );
	}

///////////////////////////////////
DATE JulianToDate(
	LONG			jul)
	{
	DATE			date;
	LONG      mon, day, year;

	jul += 578041L;

	year = ((jul * 4) - 1) / 146097L;
	jul = ((jul * 4) - 1) - (year * 146097L);

	day = (jul / 4);
	jul = ((day * 4) + 3) / 1461;
	day = ((day * 4) + 3) - (jul * 1461);
	day = (day + 4) / 4;

	mon = ((day * 5) - 3) / 153;
	day = ((day * 5) - 3) - (mon * 153);
	day = (day + 5) / 5;

	year = (year * 100) + jul;

	if (mon < 10)
		mon = mon + 3;
	else
		{
		mon = mon - 9;
		year = year + 1;
		}

	date.da_year = (int)year;
	date.da_mon = (int)mon;
	date.da_day = (int)day;

 	return date;
	}

///////////////////////////////////

LONG DateToJulian(
	DATE			date)
	{
	LONG			cent;
	LONG      mon, day, year;
	LONG			jul;

	while (date.da_mon > 12)
	{
		date.da_mon -= 12;
		date.da_year++;
	}

	year = date.da_year;
	mon = date.da_mon;
	day = date.da_day;
	if (mon > 2)
		mon -= 3;
	else
		{
		mon += 9;
		year--;
		}

	cent = year/100;

	year -= cent*100;

	jul = ((cent * 146097L)/4) +
				((year * 1461)/4) +
				(((mon * 153) + 2) / 5) +
				(day - 578041L);

	return jul;
	}
//return the tick count at midnight of the given julian date
LONG JulianToTickCount(LONG	julian)
{

	DATE		dateToday;
	TIME		time;
	LONG		ln;
	LONG		ticksToMidnight;
	DOS_DATE	dosDate;

	getdate(&dosDate);
	dateToday.da_year = dosDate.da_year;
	dateToday.da_mon = dosDate.da_mon;
	dateToday.da_day = dosDate.da_day;

	gettime(&time);

	ticksToMidnight = ( (LONG) (23 - time.ti_hour) * TICKS_PER_HOUR);
	ticksToMidnight += ( (LONG) (59 - time.ti_min) * TICKS_PER_MIN);
	ticksToMidnight += ( (LONG) (60 - time.ti_sec) * 60);

	ln = julian - DateToJulian(dateToday) - 1;
	ln *= TICKS_PER_DAY;
	ln += ticksToMidnight;

	return ln + AAppGetTime();
	
}

///////////////
//return the tick count at midnight of the given date

LONG DateToTickCount(DATE	date)
{
	return JulianToTickCount( DateToJulian(date) );
}


/////////////////////////////////////////
/////////////////////////////////////////
/////////////////////////////////////////

LPSTR SearchForSubstring(
	LPSTR		s1,
	LPSTR		s2)
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


//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////

LONG GetDlgEditShortDate(
	HWND			hWnd,
	int				item)
	{
//	int				len = (int)SendDlgItemMessage(hWnd, item, WM_GETTEXTLENGTH, 0, 0L) + 1;
	int				len = (int)Static_GetTextLength(GetDlgItem(hWnd, item)) + 1;
	LPSTR			str = (LPSTR)farmalloc(len);
	LONG			date;

//	SendDlgItemMessage(hWnd, item, WM_GETTEXT, len, (LONG)str);
	Static_GetText(GetDlgItem(hWnd, item), str, len);

	date = ParseShortDate(str);
	farfree(str);

	SetDlgEditShortDate(hWnd, item, date);

	return date;
	}

///////////////////////

void ValidateParsedDate(
	DATE *			date)
	{
	DATE					currentDate;
	DOS_DATE		dosDate;


	getdate(&dosDate);
	currentDate.da_year = dosDate.da_year;
	currentDate.da_mon = dosDate.da_mon;
	currentDate.da_day = dosDate.da_day;

	if (date->da_mon && !date->da_day && !date->da_year)
		{ // only one number entered, and we thought is was the month
		date->da_day = date->da_mon;
		date->da_mon = currentDate.da_mon;
		date->da_year = currentDate.da_year;
		}
	else if (date->da_mon && !date->da_day && date->da_year)
		{ // two numbers entered
		date->da_day = date->da_year;
		date->da_year = currentDate.da_year;
		}
	else
		{
		if (!date->da_mon) 	date->da_mon = currentDate.da_mon;
		if (!date->da_day)	date->da_day = currentDate.da_day;
		if (!date->da_year) date->da_year = currentDate.da_year;
		else if (date->da_year < 100) date->da_year += (currentDate.da_year / 100)*100;
		}
	}



////////////////////////

LONG ParseShortDate(
	LPSTR					str)
	{
	DATE					date = {0,0,0};
	LPSTR					ps, pp;

	if (str)
  	{
		ps = str;
		pp = g_sDateProfileString;

		while(*pp && *ps)
			{
			switch(*pp|0x20)
				{
				case 'm': date.da_mon = atoi(ps); break;
				case 'd': date.da_day = atoi(ps); break;
				case 'y': date.da_year = atoi(ps); break;
				default: DebugBreak();
				}
			ps = GoToNextNumber(ps, TRUE);
			pp = GoToNextDateChar(pp, TRUE);
			}
		}

	ValidateParsedDate(&date);
	return DateToJulian(date);
	}

LONG ParseDate(
	LPSTR					str,
	LPLONG				whatsupplied)
	{
	DATE					date = {0,0,0};
	LPSTR					ps, pp;
	int           month;

	*whatsupplied = 0;
	if (str)
  	{

		ps = str;

		pp = g_sDateProfileString;

		for(month=0; month<12; month++)
			{
			if ( SearchForSubstring(str, g_sMonthNames[month]) )
				{
				*whatsupplied |= ODATE_MONTH_SUPPLIED;
				pp = g_lDateProfileString;
				}
			}

			pp = GoToNextDateChar(pp, FALSE);

		while(*pp && *ps)
			{

			switch(*pp|0x20)
				{
				case 'm': *whatsupplied |= ODATE_MONTH_SUPPLIED;  break;
				case 'd': *whatsupplied |= ODATE_DAY_SUPPLIED;  break;
				case 'y': *whatsupplied |= ODATE_YEAR_SUPPLIED;  break;
				default: DebugBreak();
				}
			ps = GoToNextNumber(ps, TRUE);
			pp = GoToNextDateChar(pp, TRUE);
			}
		}

	ValidateParsedDate(&date);
	return DateToJulian(date);
	}


/////////////////////////////

void SetDlgEditShortDate(
	HWND			hWnd,
	int				item,
	LONG			date)
	{
	char			str[100];

	FormatShortDate(date, str, sizeof(str) );
//	SendDlgItemMessage(hWnd, item, WM_SETTEXT, 0, (LONG)str);
	Static_SetText(GetDlgItem(hWnd, item), str);
	}

void FormatShortDate(
	LONG			date,
	LPSTR			str,
	int				len)
	{
	LPSTR					pp;
	LPSTR					cp = str;
	int						i;
	DATE					theDate = JulianToDate(date);


	I_UNUSED(len);
	pp = g_sDateProfileString;

	strcpy (pp, "m/d/yy");

	while(*pp)
		{
		switch(*pp|0x20)	// make lowercase 
			{
			case 'm':
				for(i=0; ((*pp)|0x20)=='m'; i++, pp++);
				if (i>1)
					wsprintf(cp, "%02d%c", theDate.da_mon, *pp);
				else
					wsprintf(cp, "%0d%c", theDate.da_mon, *pp);

				if (!*pp) 
					return;
				cp += lstrlen(cp);
				pp++;
				break;
			case 'd':
				for(i=0; ((*pp)|0x20)=='d'; i++, pp++);
				if (i>1)
					wsprintf(cp, "%02d%c", theDate.da_day, *pp);
				else
					wsprintf(cp, "%0d%c", theDate.da_day, *pp);
				if (!*pp) 
					return;
				cp += lstrlen(cp);
				pp++;
				break;
			case 'y':
				for(i=0; ((*pp)|0x20)=='y'; i++, pp++);
				if (i<4)	
					wsprintf(cp, "%02d%c", theDate.da_year%100, *pp);
				else      
					wsprintf(cp, "%04d%c", theDate.da_year, *pp);
				if (!*pp) 
					return;
				cp += lstrlen(cp);
				pp++;
				break;
			}
		}
	}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

LONG GetDlgEditLongDate(
	HWND			hWnd,
	int				item)
	{
//	int				len = (int)SendDlgItemMessage(hWnd, item, WM_GETTEXTLENGTH, 0, 0L) + 1;
	int				len = (int)Static_GetTextLength(GetDlgItem(hWnd, item)) + 1;
	LPSTR			str = (LPSTR)farmalloc(len);
	LONG			date;

//	SendDlgItemMessage(hWnd, item, WM_GETTEXT, len, (LONG)str);
	Static_GetText(GetDlgItem(hWnd, item), str, len);

	date = ParseLongDate(str);
	farfree(str);

	SetDlgEditLongDate(hWnd, item, date);

	return date;
	}

////////////

LONG ParseLongDate(
	LPSTR					str)
	{
	DATE					date = {0,0,0};
	LPSTR					ps, pp;
	int						month;
	int						i;
	BOOL					incFirst = FALSE;

	if (str)
  	{
		pp = g_lDateProfileString;
		ps = str;

		for(month=0; month<12; month++)
			if ( SearchForSubstring(str, g_sMonthNames[month]))
					date.da_mon = month+1;

		pp = GoToNextDateChar(pp, FALSE);

		while(*pp && *ps)
			{
			switch(*pp)
				{
				case 'm':
				case 'M':
					if (!date.da_mon)
						{
						ps = GoToNextNumber(ps, incFirst); incFirst = TRUE;
						date.da_mon = atoi(ps);
						}
					pp = GoToNextDateChar(pp, TRUE);
					break;
				case 'd':
				case 'D':
					for(i=0; ((*pp)|0x20)=='d'; i++, pp++);
					if (i<3) {
						ps = GoToNextNumber(ps, incFirst); incFirst = TRUE;
						date.da_day = atoi(ps);
						}
					pp = GoToNextDateChar(pp, FALSE);
					break;
				case 'y':
				case 'Y':
					ps = GoToNextNumber(ps, incFirst); incFirst = TRUE;
					date.da_year = atoi(ps);
					pp = GoToNextDateChar(pp, TRUE);
					break;

				default: //Should never get here, but prevent infinite loop
					pp = GoToNextDateChar(pp, TRUE);
					break;
				}
			}
    }

	ValidateParsedDate(&date);
	return DateToJulian(date);
	}

/////////////////////////////////

void SetDlgEditLongDate(
	HWND			hWnd,
	int				item,
	LONG			date)
	{
	char			str[100];

	FormatLongDate(date, str, sizeof(str) );
//	SendDlgItemMessage(hWnd, item, WM_SETTEXT, 0, (LONG)str);
	Static_SetText(GetDlgItem(hWnd, item), str);
	}

////////////////////////////////

void FormatLongDate(
	LONG					date,
	LPSTR					str,
	int						len)
	{
	LPSTR					pp;
	LPSTR					cp = str;
	//LPSTR					max_cp = cp + len;
	int						i;
	DATE					theDate = JulianToDate(date);

	I_UNUSED(len);
	pp = g_lDateProfileString;

	while(*pp)
		{
		switch(*pp)
			{
			case 'm':
			case 'M':
				for(i=0; ((*pp)|0x20)=='m'; i++, pp++);
				switch(i)
					{
					case 1:	wsprintf(cp, "%d", theDate.da_mon); break;
					case 2: wsprintf(cp, "%02d", theDate.da_mon); break;
					case 3:	lstrcpy(cp, g_sMonthNames[theDate.da_mon-1]); break;
					case 4:	lstrcpy(cp, g_lMonthNames[theDate.da_mon-1]); break;
					}//switch i
				if (!*pp) return;
				cp += lstrlen(cp);
				break;

			case 'd':
			case 'D':
				for(i=0; ((*pp)|0x20)=='d'; i++, pp++);
				switch(i)
					{
					case 1:	wsprintf(cp, "%d", theDate.da_day); break;
					case 2: wsprintf(cp, "%02d", theDate.da_day); break;
					case 3: lstrcpy(cp, g_sDayNames[DayOfTheWeek(date)]); break;
					case 4:	lstrcpy(cp, g_lDayNames[DayOfTheWeek(date)]);	break;
					}//switch i
				if (!*pp) return;
				cp += lstrlen(cp);
				break;

			case 'y':
			case 'Y':
				for(i=0; ((*pp)|0x20)=='y'; i++, pp++);
				if (i<4)	wsprintf(cp, "%02d", theDate.da_year%100, *pp);
				else      wsprintf(cp, "%04d", theDate.da_year, *pp);
				if (!*pp) return;
				cp += lstrlen(cp);
				break;

			case '\'':
				pp++;
				while(*pp != '\'' && *pp)	*(cp++) = *(pp++);
				if (*pp) pp++;
				break;

			default:
				*(cp++) = *(pp++);
				break;
			}
		}//while
}


