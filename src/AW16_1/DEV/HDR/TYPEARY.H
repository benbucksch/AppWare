#ifndef __TYPEARY_H__
#define __TYPEARY_H__

#include "array.h"

class TYPEIDARRAY : public SERIUSARRAY
	{
public:

	TYPEIDARRAY();

	TYPEIDARRAY(
		const TYPEIDARRAY far&	srcAry);

	TYPEIDARRAY(
		void far*	objPtr):
			SERIUSARRAY(objPtr)
		{
		}

	void		SetElement(
		short		nIdx,
		TYPEID	t);

	TYPEID	GetElement(
		short		nIdx) const;

	TYPEID	pGetElement(
		short		nIdx) const;
	};

#endif
