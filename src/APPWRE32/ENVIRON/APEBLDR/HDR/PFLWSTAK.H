#ifndef __PRINTFLOWSTACK_H
#define __PRINTFLOWSTACK_H

#include "classdef.h"
#include "winlist.h"

class PRINTFLOWSTACK:public WINDOWLIST
	{
	public:

		PRINTFLOWSTACK():WINDOWLIST()
			{
			}

		void PushFlow(
			int indent,
			FLOW* flowPtr)
			{
			Add((HWND) indent, flowPtr);
			}

		FLOW * PopFlow(
			int * indent)
			{
			void * ptr;

			if((ptr = GetTailObject((HWND *)indent))==NULL)
				return NULL;
			else
				{
				RemoveTail();
				return (FLOW *)ptr;
				}
			}

	};

#endif
