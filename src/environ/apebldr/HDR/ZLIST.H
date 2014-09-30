#ifndef __ZLIST_H
#define __ZLIST_H

#include "seruid.h"
#include "memclass.h"
#include "classdef.h"

#define Z_EMPTY			0
#define Z_FUNC 			1
#define Z_PARAM   	2
#define Z_FLOW			3
#define Z_COMMENT 	4
#define Z_INFUNC		5
#define Z_OUTFUNC 	6
#define Z_OBJGROUP 	7


struct ZDATA
	{
	RECT rect;
	WORD type;
	SERUID uid;
	};

typedef ZDATA * LPZDATA;

class ZLIST:public MEMORY
	{
	private:
		HGLOBAL  hData;
		long		count;
		long 		maxRecords;

	public:

		ZLIST();
		~ZLIST();

		void ClearList();
		void Add(
			LPRECT bounds,
			WORD newType,
			SERUID uid);

		WORD TypeFromPoint(
			POINT pt,
			LPSERUID lpuid);

		LPZDATA GetNextMember(
			long * recNumber);

	};

#endif
