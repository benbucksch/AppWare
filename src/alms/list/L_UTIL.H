#ifndef L_UTIL_H
#define L_UTIL_H


BOOL InsertListStrings(LPSTR str, int strLength, int idx, int numItems, LPLIST_TABLE table, LPSTR strings, LONG stringsSize);
int deleteListString(int idx, int numItems, LPLIST_TABLE table, LPSTR strings, LONG stringsSize);
void fixTable(LPLIST_TABLE table, LPSTR strings, int numItems);
void SortList(LPLIST_TABLE table, LPSTR strings, int numItems);

BOOL GetNextLine(LPSTR str, LPLONG start, LPLONG end);


inline LPSTR GetListString(int idx, LPLIST_TABLE table,	LPSTR strings)
	{
	if (idx < 0)
		return "" ;
	if (idx == 0)
		return strings ;
	else
		return &strings[(int)table[idx-1].lCharIdx] ;
	}


#endif
