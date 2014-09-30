#include "Tm_class.h"
/*****************************************************/
/* Prototype of time function                        */
/*****************************************************/
LONG GetDlgEditTime(HWND hWnd, int item, LONG style);
LONG ParseTime(LPSTR str);
LONG TimeToSec(TIME time);
TIME SecToTime(LONG sec);
LPSTR GoToNextNumber(LPSTR str,	BOOL incFirst); 
LPSTR SearchForSubstring(LPSTR s1,	LPSTR	s2);
LPSTR	IncrementToNonQuote(LPSTR	str);
void FormatTime(LONG time, LONG	style, LPSTR str,	int	len);
void SetDlgEditTime(HWND hWnd, int item, LONG time, LONG style);
LONG ParseTimeString(LPSTR str, LPLONG	whatsupplied);
void gettime(struct time *timep);

