#ifndef __TYPEARY_H__
#define __TYPEARY_H__

#include "array.h"

class TYPEIDARRAY : public SERIUSARRAY
	{
public:

	TYPEIDARRAY();

	TYPEIDARRAY(
		const TYPEIDARRAY &	srcAry);

	TYPEIDARRAY(
		void *	objPtr):
			SERIUSARRAY(objPtr)
		{
		}

	void		SetElement(
		short		nIdx,
		ATYPEID	t);

	ATYPEID	GetElement(
		short		nIdx) const;

	ATYPEID	pGetElement(
		short		nIdx) const;
	};

#endif
