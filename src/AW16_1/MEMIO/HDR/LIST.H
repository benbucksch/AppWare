#ifndef __LIST_H
#define __LIST_H

#include "classdef.h"
#include "memclass.h"

class LIST:public MEMORY
	{

	protected:
		LPLISTMEMBER head;
		LPLISTMEMBER tail;

	public:
		LIST();

		BOOL Add(
			LPLISTMEMBER memberPtr);

		BOOL Remove(
			LPLISTMEMBER memberPtr);

		LPLISTMEMBER GetNextMember(
			LPLISTMEMBER memberPtr);

		LPLISTMEMBER GetPrevMember(
			LPLISTMEMBER memberPtr);

		BOOL IsListEmpty();

    VOID Initialize()
      {
      head = tail = 0;
      }

		unsigned int GetMemberCount();
	};

class STACK:public LIST
	{
	public:
		STACK()
			{
			};

		BOOL Push(
			LPLISTMEMBER memberPtr);

		LPLISTMEMBER Pop();

		BOOL IsStackEmpty()
			{
			return IsListEmpty();
			}
	};


#endif
