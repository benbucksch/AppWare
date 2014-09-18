#include "l_includ.h"

///////////////////////////////////////////////////////
//	Function: InsertListStrings
//	Description: Inserts new item's string in the string data block.
//  	Before calling this function, nItems should have been incremented,
//		and table and strings should have been resized.
///////////////////////////////////////////////////////
BOOL InsertListStrings(
	LPSTR										szNewItem, 
	int 										nItemLength, 	 //doesn't include the '\0'
	int 										index, 
	int 										nItems,
	LPLIST_TABLE 						table,	
	LPSTR 									strings,
	LONG										stringsSize)
	{
	int 										i;
	UINT 										sourcePos, destinationPos, bytesToMove;

	I_ALWAYSASSERT(index >= 0 && index < nItems, return FALSE);
	I_ALWAYSASSERT(table && strings && szNewItem, return FALSE);
	I_ALWAYSASSERT((nItems <= 1) || (table[nItems-2].lCharIdx + nItemLength <= stringsSize), return FALSE);
	I_ALWAYSASSERT((nItems != 1) || (nItemLength <= stringsSize), return FALSE);
	
 	//Shift the table info
	for	(i = nItems-1; i >= index && i > 0; i--)
		{
		table[i].lCharIdx = table[i-1].lCharIdx + nItemLength + 1;
		table[i].bIsSelected = table[i-1].bIsSelected ;
		}
	if (index == 0)
		table[0].lCharIdx = nItemLength + 1;

	table[index].bIsSelected = FALSE;
	//Shift the strings
	sourcePos = index ? (UINT)table[index-1].lCharIdx : 0;
	if (index < nItems-1)
		{   // if this item is not being appended to the end of the list
		destinationPos = sourcePos + nItemLength + 1;
		bytesToMove = (UINT)stringsSize - destinationPos;
		memmove(&strings[destinationPos], &strings[sourcePos], bytesToMove);
		}
	memmove(&strings[sourcePos], szNewItem, nItemLength);
	strings[sourcePos+nItemLength]=0;

	return TRUE;
	}

///////////////////////////////////////////////////////////
//	Function: deleteListString
//	Description:  After calling this function, table and strings
//		should be shrunk, and the nItems should be decremented.
//		The return value is the number of character	deleted.
//		This is the value to shrink strings by.
////////////////////////////////////////////////////////////
int deleteListString(
	int 										index,	
	int 										nItems, 
	LPLIST_TABLE 						table,
	LPSTR 									strings, 
	LONG										stringsSize)
	{
	UINT 										destinationPos, sourcePos, bytesToMove;
	int 										nItemLength;

	if (index == 0)
		destinationPos = 0;
	else
		destinationPos = (UINT)table[index-1].lCharIdx;

	nItemLength = (UINT)table[index].lCharIdx - destinationPos ;
	//shift the strings
	if (index < nItems-1)
		{ // if this item is not the last item in the list
		sourcePos = (UINT)table[index].lCharIdx;
		bytesToMove = (UINT)stringsSize - sourcePos;
		memmove(&strings[destinationPos], &strings[sourcePos], bytesToMove);
		}

	for(int i = index; i < nItems-1; i++)
		{
		table[i].lCharIdx = table[i+1].lCharIdx - nItemLength;
		table[i].bIsSelected = table[i+1].bIsSelected;
		}

	return nItemLength;
	}
///////////////////////////////////////////////////////////
//	Function: fixTable
//	Description: Recalculates the lCharIdx fields of the LPLIST_TABLE
//		elements of the table data block.
////////////////////////////////////////////////////////////
void fixTable(
	LPLIST_TABLE 						table, 
	LPSTR 									strings, 
	int 										nItems)
	{
	int 										nStringLength;
	UINT 										nAccumulatedLength = 0;

	for (int i = 0; i < nItems; i++)
		{
		nStringLength = lstrlen(&strings[nAccumulatedLength]) + 1 ;
		nAccumulatedLength += nStringLength ;
		table[i].lCharIdx = nAccumulatedLength ;
		}

	}

///////////////////////////////////////////////////////////
//	Function: SortList
//	Description: Sorts the list.
////////////////////////////////////////////////////////////
void SortList(
	LPLIST_TABLE 						table, 
	LPSTR 									strings, 
	int 										nItems)
	{
	MEMBLOCKID 							miSort, miTmp;
	LPINT 									sortIdx;
	UINT 										totalChars, curTotal;
	LPSTR 									szTmp, cp;
	int											span, i, upperRung, idxNow, lowerRung;

	miSort = AMemAllocate(nItems * sizeof(int));
	sortIdx = (LPINT) AMemLock(miSort);
	totalChars = (UINT)table[nItems-1].lCharIdx;
	miTmp = AMemAllocate(totalChars);
	szTmp = (LPSTR) AMemLock(miTmp);

	//Set up indexes
	for (i = 0; i < nItems; i++)
		sortIdx[i] = i ;

	//Set up span
	span = 1 ;
	if (nItems > 13)
		{
		while (span < nItems)
			span = (3*span)+1;
		span /= 9;
		}

	//Shell sort indexes

	for( ; span > 0; span/=3)
		{
		for (upperRung = span; upperRung < nItems; upperRung++)
			{
			idxNow = sortIdx[upperRung];
			lowerRung = upperRung - span;
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
	cp = szTmp ;
	for (i = 0; i < nItems; i++, cp+=lstrlen(cp)+1)
		lstrcpy(cp, GetListString(sortIdx[i], table, strings) );

	//Now copy the tmpStr to the real McCoy
	_fmemcpy(strings, szTmp, totalChars);

	//And restructure the table
	curTotal = 0 ;
	for(i = 0; i < nItems; i++)
		{
		curTotal += lstrlen(&szTmp[curTotal]) + 1;
		table[i].lCharIdx = curTotal;
		table[i].bIsSelected = FALSE;
		}

	//Free up tmp stuff
	AMemUnlock(miSort) ;
	AMemFree(miSort) ;
	AMemUnlock(miTmp) ;
	AMemFree(miTmp) ;
	}

////////////////////////////////////////
////////////////////////////////////////

BOOL GetNextLine(
	LPSTR 									str, 
	unsigned short& 				start, 
	unsigned short& 				end)
	{
	BOOL										rVal=FALSE;

	if (str  &&  *str)
		{
		while (str[start] == '\r' || str[start] == '\n')
			start++;

		end = start;

		if (str[start])
			{
			while ((str[end] != '\r') && (str[end] != '\n') && (str[end] != 0))
				end++;
			rVal = TRUE;
			}
		}

	return rVal;
	}
