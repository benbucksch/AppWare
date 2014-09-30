#ifndef __RECPREFX__H__
#define __RECPREFX__H__

#include "seruid.h"

struct RECORDPREFIX
	{
	SERUID	recBlockUID;
	SERUID	UID;

	RECORDPREFIX(
		void)
		{
		UID = NULLSERUID;
		recBlockUID = NULLSERUID;
		};

	RECORDPREFIX(
		SERUID 	recUID,
		SERUID  blockUID)
		{
		UID = recUID;
		recBlockUID = blockUID;
		}

	};

typedef RECORDPREFIX far*	LPRECORDPREFIX;

#endif
