/*//////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE: dt_util.h
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION:	Date object function prototype
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
LPSTR IncrementToNonQuote(LPSTR str);
LPSTR GoToNextNumber(LPSTR str, BOOL incFirst);
LPSTR GoToNextDateChar(LPSTR str, BOOL incFirst);
LONG ValidateDate(LONG date);
DATE JulianToDate(LONG jul);
LONG DateToJulian(DATE date);
LONG JulianToTickCount(LONG julian);
LONG DateToTickCount(DATE date);
LPSTR SearchForSubstring(LPSTR s1/*in this*/, LPSTR s2/*for this*/);
LONG GetDlgEditShortDate(HWND hWnd, int item);
void ValidateParsedDate(LPDate date);
LONG ParseShortDate(LPSTR str);
void SetDlgEditShortDate(HWND hWnd, int item, LONG date);
void FormatShortDate(LONG date, LPSTR str, int len);
LONG GetDlgEditLongDate(HWND hWnd, int item);
LONG ParseLongDate(LPSTR str);
void FormatLongDate(LONG date, LPSTR str, int len);
void SetDlgEditLongDate(HWND hWnd, int item, LONG date);
LONG ParseDate(LPSTR str, LPLONG	whatsupplied);


class STRING_RES
{
	public:
		int         len;
		char        text[256];

		STRING_RES(HINSTANCE hInstance, WORD wID)
		{
			len = LoadString(hInstance, wID, text, 255);
		}

};
typedef STRING_RES far * LPSTRING_RES;

inline char DayOfTheWeek(unsigned long date)
{
	return (date+4) % 7;
}









