#include "almsys.h"

#include "funcinst.h"
#include "palette.h"
#include "subject.h"
#include "seruid.h"
#include "fwinobj.h"


//////////////////////////////////////////////////////////
FUNCWINOBJ::FUNCWINOBJ(
	SERUID theSubjectUID,
	POINT newLocation)
	{
	status = 0;
	location = newLocation;
	subjectUID = theSubjectUID;

	}

//////////////////////////////////////////////////////////
FUNCWINOBJ::FUNCWINOBJ(
	void * /*thisPtr*/)
	{
	}

//////////////////////////////////////////////////////////
SUBJECT * FUNCWINOBJ::GetSubjectPtr()
	{
	return (SUBJECT *)GetObject(subjectUID);
	}

//////////////////////////////////////////////////////////
void FUNCWINOBJ::UnSelect(
	HDC hdc)
	{
	if(IsSelected())
		{
		SetSelected(FALSE);
		DrawSelection(hdc);
		}
	}

//////////////////////////////////////////////////////////
void FUNCWINOBJ::Select(
	HDC hdc)
	{
	if(!IsSelected())
		{
		SetSelected(TRUE);
		DrawSelection(hdc);
		}
	}

//////////////////////////////////////////////////////////
BOOL FUNCWINOBJ::IntersectPoint(
	POINT pt)
	{
	RECT bounds;

	GetRect((LPRECT)&bounds);
	return(PtInRect((LPRECT)&bounds, pt));
	}

//////////////////////////////////////////////////////////
void FUNCWINOBJ::GetSelectedRect(
	LPRECT bounds)
	{
	GetRect(bounds);
	bounds->left-=BORDER;
	bounds->top-=BORDER;
	bounds->bottom+=BORDER;
	bounds->right+=BORDER;
	}


//////////////////////////////////////////////////////////
BOOL FUNCWINOBJ::ObjectIntersectsRect(
	LPRECT bigRect)
	{
	RECT bounds;
	RECT temp;

	GetRect((LPRECT)&bounds);
	return(IntersectRect((LPRECT)&temp, (LPRECT)&bounds, bigRect));
	}

//if intersectRect intersects the selected Rect, the union of the
//the selected Rect and unionRect is returned in unionRect.
void FUNCWINOBJ::GetUnionWithSelectRect(
	LPRECT intersectRect,
	LPRECT unionRect)
	{
	RECT selectRect;
	RECT tempRect;

	GetMaxScreenSize((LPRECT)&selectRect);
	if(IntersectRect((LPRECT)&tempRect, intersectRect, (LPRECT)&selectRect))
		UnionRect(unionRect, unionRect, (LPRECT)&selectRect);


	}


