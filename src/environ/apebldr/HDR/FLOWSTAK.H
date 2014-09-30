#ifndef __FLOWSTAK_H
#define __FLOWSTAK_H

#include "classdef.h"
#include "list.h"
#include "listmbr.h"
#include "memclass.h"

//class whose dataPtr will be the flow pointer
class VOIDSTACKMEMBER:public LISTMEMBER, public MEMORY
	{
	public:
		void * dataPtr;

		VOIDSTACKMEMBER(
			void * voidPtr):dataPtr(voidPtr)
			{
			};

	};

typedef VOIDSTACKMEMBER * LPVOIDSTACKMEMBER;

//class to maintain a stack of flow pointers.
class VOIDSTACK:public STACK
	{
  public:
		VOIDSTACK()
			{
			};

		BOOL Push(
			void * voidPtr)
			{
			LPVOIDSTACKMEMBER voidMember;

			voidMember = new VOIDSTACKMEMBER(voidPtr);
			return STACK::Push(voidMember);
			}

		void * Pop()
			{
			LPVOIDSTACKMEMBER voidMember;
			void * voidPtr = NULL;

			if(NULL != (voidMember = (LPVOIDSTACKMEMBER)STACK::Pop()))
				{
				voidPtr = voidMember->dataPtr;
				delete voidMember;
				}

			return (void *)voidPtr;
			}
  };

//////////////////////////////////////////////////////////////////////
typedef VOIDSTACKMEMBER FLOWSTACKMEMBER;
typedef FLOWSTACKMEMBER * LPFLOWSTACKMEMBER;

class FLOWSTACK:public VOIDSTACK
	{
	public:
		FLOWSTACK()
			{
			}

		LPFLOW Pop()
			{
			return (LPFLOW)VOIDSTACK::Pop();
			}		
	};

#endif
