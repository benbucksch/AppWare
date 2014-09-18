////////////////////////////////////////////////////////////////////////////////
//	LISTMETH.CPP
//  Methods used by the List object
////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <alloc.h>
#include <a_alm.h>
#include <op_wnd.h>
//#include <o_datb.h>
#include "attribut.h"
#include "list.h"
///////////////////////////////////////////////////
EXTERNC StartupMeth(pAEvtInfo, pAObjMessage)
	{
	}
/////////////////////////
EXTERNC ShutdownMeth(pAEvtInfo, pAObjMessage)
	{
	}
///////////////////////////
EXTERNC ObjectToTextMeth(pAEvtInfo theEvent, pAObjMessage)
	{
	OBJECTID oiList = theEvent->targetObject ;
	LPLIST list = (LPLIST)AObjLockData(oiList, LIST_INFO);
	if (list == NULL)
		return ;

	MEMBLOCKID textMID = theEvent->details;

	LPSTR text ;
	if (list->m_nItems) // if the list has any items
        {
		LPLIST_TABLE table = (LPLIST_TABLE)AObjLockData(oiList, TABLE_INFO);
		LPSTR strings = (LPSTR)AObjLockData(oiList, STRINGS_INFO);
		if (table && strings)
        	{
			LONG len = table[list->m_nItems-1].charIdx + list->m_nItems; //for the linefeeds

			AMemSetSize(textMID, len);
			text = (LPSTR)AMemLock(textMID);

			for (int i = 0; i < list->m_nItems; i++)
				{
				LPSTR cp = GetListString(i, table, strings);
				lstrcpy(text, cp);
				lstrcat(text, "\r\n");
				text += lstrlen(text);
				}
			text -=2;	//remove the last <CR><LF>
			}
        if (table)
			AObjUnlockData(oiList, TABLE_INFO);
        if (strings)
			AObjUnlockData(oiList, STRINGS_INFO);
		}
	else   // if the list is empty
		{
		AMemSetSize(textMID, 1);
		text = (LPSTR)AMemLock(textMID);
		}
	*text = 0; // null terminate the string
	AMemUnlock(textMID) ;

	AObjUnlockData(oiList, LIST_INFO);
	}
///////////////////////////
EXTERNC TextToObjectMeth(pAEvtInfo theEvent, pAObjMessage)
	{
	OBJECTID oiList = theEvent->targetObject ;
	LPLIST list = (LPLIST)AObjLockData(oiList, LIST_INFO);
	MEMBLOCKID memText = theEvent->details;
	LPSTR lpText = (LPSTR)AMemLock(memText);

	//Empty the current list
	list->m_curSelection = 0;
	list->m_nItems = 0L;
	list->m_stringsSize = 0L;
	AObjResizeData(oiList, STRINGS_INFO, 0);
	AObjResizeData(oiList, TABLE_INFO, 0);

	UINT end = 0 ;
	int row = 0 ;
	for (UINT i = 0; GetNextLine(lpText, i, end); i = end+1, row++)
		{
		if (lpText[end] == 0)
			{
			list->addItem(&lpText[i], row);
			break ;
			}
		else
			{
			lpText[end] = 0 ;
			list->addItem(&lpText[i], row);
			}
		if (end > 32767)
			break ;
		}
	AMemUnlock(memText) ;
//	AMemFree(memText) ;  PWR 12/15/93
	AObjUnlockData(oiList, LIST_INFO);  // PWR 2/7/94
	PostObjectChanged(oiList, MAKELONG(LST_ITEMS_CHANGED, 0));
	}

