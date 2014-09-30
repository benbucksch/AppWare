#ifndef __SCOPSTAK_H
#define __SCOPSTAK_H

#include "memclass.h"
#include "winlist.h"
#include "classdef.h"

#define ST_FULL 1.0
#define ST_HALF 0.5
#define ST_QUARTER 0.25
#define ST_3QUARTER 0.75
#define ST_1TENTH		0.1
#define ST_9TENTHS  0.9

class SCOPERECORD: public MEMORY
	{
	public:
		SCOPERECORD(
			long newSize,
			long initDrawn,
			double newPercent):size(newSize), amountDrawn(initDrawn),
				percent(newPercent)
			{
			}

		long size;
		long amountDrawn;
		double percent;
	};

typedef SCOPERECORD * LPSCOPERECORD;

class SCOPESTACK:public WINDOWLIST
	{
	private:
		COMPILESTATUS * compStatPtr;
		double scopePercent;
		long totalSize;

	public:
		SCOPESTACK(
			HWND owner,
			LPSTR operation,
			LPSTR fileName,
			LPSTR title,
			double prePercent,
			double postPercent,
			long typeStatus);

		~SCOPESTACK();

		void PushScope(
			double percentage);

		BOOL PopScope(
			BOOL drawToEnd);

		void Draw(
			long size);

		void ErrorCleanup();

	};
#endif
