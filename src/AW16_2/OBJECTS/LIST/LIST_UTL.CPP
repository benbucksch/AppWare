////////////////////////////////////////////////////////
/////////  List Object Utlitiy Routines    ////////
////////////////////////////////////////////////////////
#include <windows.h>
//#include <stdio.h>
#include <mem.h>
//#include <string.h>
//#include <math.h>

#include <a_alm.h>
//#include "seriusid.h"
#include <op_wnd.h>
#include "attribut.h"
#include "list.h"
///////////////////////////////////////////////////////
//	Function: insertListString
//	Description: Inserts new item's string in the string data block.
//  	Before calling this function, nItems should have been incremented,
//		and table and strings should have been resized.
///////////////////////////////////////////////////////
BOOL insertListString(LPSTR	lpNewItem, int nItemLength, int index, int nItems,
		LPLIST_TABLE table,	LPSTR strings, LONG	stringsSize)
	{
	if (index < 0 || index >= nItems)  //Should NEVER happen
		return FALSE ;  // Check for out of range indexes
	if (!table || !strings || !lpNewItem)
		return FALSE ;  // Check for NULL pointers
	if ((nItems > 1) && (table[nItems-2].charIdx + nItemLength > stringsSize))
		return FALSE ;  // Verify that stringsSize is large enough
	if ((nItems == 1) && (nItemLength > stringsSize))
    	return FALSE ;

	//Shift the table info
	for	(int i = nItems-1; i >= index && i > 0; i--)
		{
		table[i].charIdx = table[i-1].charIdx + nItemLength ;
		table[i].isSelected = table[i-1].isSelected ;
		}
	if (index == 0)
		table[0].charIdx = nItemLength ;

	table[index].isSelected = FALSE;
	//Shift the strings
	UINT sourcePos = index ? (UINT)table[index-1].charIdx : 0;
	if (index < nItems-1)
		{   // if this item is not being appended to the end of the list
		UINT destinationPos = sourcePos + nItemLength ;
		UINT bytesToMove = (UINT)stringsSize - destinationPos;
		memmove(&strings[destinationPos], &strings[sourcePos], bytesToMove);
		}
	memmove(&strings[sourcePos], lpNewItem, nItemLength);

	return TRUE;
	}
///////////////////////////////////////////////////////////
//	Function: deleteListString
//	Description:  After calling this function, table and strings
//		should be shrunk, and the nItems should be decremented.
//		The return value is the number of character	deleted.
//		This is the value to shrink strings by.
////////////////////////////////////////////////////////////
int deleteListString(int index,	int nItems, LPLIST_TABLE table,
		LPSTR strings, LONG	stringsSize)
	{
	UINT destinationPos ;
	if (index == 0)
		destinationPos = 0 ;
	else
		destinationPos =  (UINT)table[index-1].charIdx ;

	int nItemLength = (UINT)table[index].charIdx - destinationPos ;
	//shift the strings
	if (index < nItems-1)
		{ // if this item is not the last item in the list
		UINT sourcePos = (UINT)table[index].charIdx;
		UINT bytesToMove = (UINT)stringsSize - sourcePos;
		memmove(&strings[destinationPos], &strings[sourcePos], bytesToMove);
		}

	for(int i = index; i < nItems-1; i++)
		{
		table[i].charIdx = table[i+1].charIdx - nItemLength;
		table[i].isSelected = table[i+1].isSelected;
		}

	return nItemLength;
	}
///////////////////////////////////////////////////////////
//	Function: fixTable
//	Description: Recalculates the charIdx fields of the LPLIST_TABLE
//		elements of the table data block.
////////////////////////////////////////////////////////////
void fixTable(LPLIST_TABLE table, LPSTR strings, int nItems)
	{
	int nStringLength ;
	UINT nAccumulatedLength = 0 ;
	for (int i = 0; i < nItems; i++)
		{
		nStringLength = lstrlen(&strings[nAccumulatedLength]) + 1 ;
		nAccumulatedLength += nStringLength ;
		table[i].charIdx = nAccumulatedLength ;
		}
	}
///////////////////////////////////////////////////////////
//	Function: SortList
//	Description: Sorts the list.
////////////////////////////////////////////////////////////
void SortList(LPLIST_TABLE table, LPSTR strings, int nItems)
	{
	MEMBLOCKID sortMID = AMemAllocate(nItems * sizeof(int));
	LPINT sortIdx = (LPINT) AMemLock(sortMID);

	UINT totalChars = (UINT)table[nItems-1].charIdx;
	MEMBLOCKID tmpStrMID = AMemAllocate(totalChars);
	LPSTR tmpStr = (LPSTR) AMemLock(tmpStrMID);

	//Set up indexes
	for (int i = 0; i < nItems; i++)
		sortIdx[i] = i ;

	//Set up span
	int span = 1 ;
	if (nItems > 13)
		{
		while (span < nItems)
			span = (3*span)+1;
		span /= 9;
		}

	//Shell sort indexes

	for( ; span > 0; span/=3)
		{
		for (int upperRung = span; upperRung < nItems; upperRung++)
			{
			int idxNow = sortIdx[upperRung];
			int lowerRung = upperRung - span;
			while(lowerRung >= 0  &&
					(lstrcmp(GetListString(sortIdx[lowerRung], table, strings),
					GetListString(idxNow, table, strings)) > 0) )
				{
				sortIdx[lowerRung+span] = sortIdx[lowerRung];
				lowerRung -= span;
				}
			sortIdx[lowerRung+span] = idxNow;
			}
		}

	//Fill tmpStr with sorted strings
	LPSTR cp = tmpStr ;
	for (i = 0; i < nItems; i++, cp+=lstrlen(cp)+1)
		lstrcpy(cp, GetListString(sortIdx[i], table, strings) );

	//Now copy the tmpStr to the real McCoy
	_fmemcpy(strings, tmpStr, totalChars);

	//And restructure the table
	UINT curTotal = 0 ;
	for(i = 0; i < nItems; i++)
		{
		curTotal += lstrlen(&tmpStr[curTotal]) + 1;
		table[i].charIdx = curTotal;
		table[i].isSelected = FALSE;
		}

	//Free up tmp stuff
	AMemUnlock(sortMID) ;
	AMemFree(sortMID) ;
	AMemUnlock(tmpStrMID) ;
	AMemFree(tmpStrMID) ;
	}

