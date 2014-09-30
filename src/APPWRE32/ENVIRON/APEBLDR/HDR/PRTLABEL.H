#ifndef __PRINTLABEL_H
#define __PRINTLABEL_H

#include "classdef.h"
#include "winlist.h"

class PRINTLABEL:public WINDOWLIST
	{
	private:
		int label;

	public:

		PRINTLABEL():WINDOWLIST()
			{
			label = 0;
			}

		~PRINTLABEL()
			{
			PurgeList();
			}

		void NewLabel(
			FUNCINST * funcPtr)
			{
			label++;
			Add((HWND) label, funcPtr);
			}

		int RetrieveLabel(
			FUNCINST * funcPtr)
			{
			return (int) GetWindow(funcPtr);
			}

	};

#endif
