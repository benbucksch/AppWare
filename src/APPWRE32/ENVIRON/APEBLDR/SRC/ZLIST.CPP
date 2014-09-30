#include "almsys.h"

#include "zlist.h"
#include "sereclib.h"
#include "memblk.h"

#define Z_INITIALSIZE 300 //based on screensize divided by funcsize
#define	Z_INCREASE		50


ZLIST::ZLIST()
	{
	maxRecords = Z_INITIALSIZE;
	count = 0;
	hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(ZDATA) * Z_INITIALSIZE);
	}


ZLIST::~ZLIST()
	{
	//force the lockcount to zero so the block can be freed
	while(GMEM_LOCKCOUNT & GlobalFlags(hData))
		GlobalUnlock(hData);

	GlobalFree(hData);
	}

void ZLIST::ClearList()
	{
	count = 0;
	}

void ZLIST::Add(
	LPRECT bounds,
	WORD	newType,
	SERUID uid)
	{
	LPZDATA dataPtr;

	if(count == maxRecords)
		{
		maxRecords+=Z_INCREASE;
		hData = GlobalReAlloc(hData, sizeof(ZDATA) * maxRecords, GMEM_MOVEABLE);

		}

	dataPtr = (LPZDATA)GlobalLock(hData);
	dataPtr[(UINT)count].rect = *bounds;
	dataPtr[(UINT)count].type = newType;
	dataPtr[(UINT)count].uid = uid;
	GlobalUnlock(hData);
	count++;
	}


//searches in reverse order
WORD ZLIST::TypeFromPoint(
	POINT pt,
	LPSERUID lpuid)
	{
	WORD type = Z_EMPTY;
	LPZDATA dataPtr;
	int i;

	if(count > 0)
		{
		dataPtr = (LPZDATA)GlobalLock(hData);
		for(i=(UINT)(count-1);i>=0;i--)
			{
			if(PtInRect((LPRECT)&(dataPtr[i].rect), pt))
				{
				type = dataPtr[i].type;
				*lpuid = dataPtr[i].uid;
				break;
				}
			}
		GlobalUnlock(hData);
		}

	if(type == Z_EMPTY)
		*lpuid = NULLSERUID;

	return type;

	}

LPZDATA ZLIST::GetNextMember(
	long * recNumber)
	{
	LPZDATA dataPtr;

	if(*recNumber < 0 || *recNumber >= count)
		dataPtr = NULL;
	else
		{
		dataPtr = (LPZDATA)GlobalLock(hData);

		dataPtr = (dataPtr + (int)(*recNumber));
		(*recNumber)++;
		GlobalUnlock(hData);
		}
	return dataPtr;
	}
