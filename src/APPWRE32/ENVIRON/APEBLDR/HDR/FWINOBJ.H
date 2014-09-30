#ifndef FWINOBJ_H
#define FWINOBJ_H

#include "funcmask.h"
#include "classdef.h"
#include "serrec.h"

#define FUNCSIZE 32
#define BORDER 2

class FUNCWINOBJ:public SERIUSRECORD
	{
	private:

		virtual void DrawSelection(
			HDC hdc)=0;

	protected:
		SERUID subjectUID;
		POINT location;
		long	status;

		void SetFunctionFlag()
			{
			status &= ~(FWO_COMMENT|FWO_OBJGROUP);
			status |= FWO_FUNCTION;
			}

		void SetCommentFlag()
			{
			status &= ~(FWO_FUNCTION|FWO_OBJGROUP);
			status |= FWO_COMMENT;
			}

		void SetObjGroupFlag()
			{
			status &= ~(FWO_FUNCTION|FWO_COMMENT);
			status |= FWO_OBJGROUP;
			}


	public:

		FUNCWINOBJ(
			SERUID theSubjectUID,
			POINT newLocation);

		FUNCWINOBJ(
			void * thisPtr);

//		virtual BOOL Draw(
//			HDC hdc,
//			LPRECT drawRect);

		virtual void Draw(
			HDC hdc)=0;

		//////////////////////////////////////////////////////////
		void FUNCWINOBJ::SetSelected(
			BOOL set)
			{
			if(set)
				status|=FWO_SELECTED;
			else
				status&= ~FWO_SELECTED;
			}

		//////////////////////////////////////////////////////////
		BOOL FUNCWINOBJ::IsSelected()
			{
			return (status & FWO_SELECTED)?TRUE:FALSE;
			}

		BOOL IntersectPoint(
			POINT pt);

		//should return the rectangle which the object covers and therefore
		//which would need to be redrawn if the object was moved.
		virtual void GetRect(
			LPRECT bounds)=0;

		//should return the total union of all areas occupied by the object
		//and parts of the object such as flows.
		virtual void GetTotalBounds(
			LPRECT bounds)
			{
			GetRect(bounds);
			}

		virtual void DrawDrag(
			HDC hdc,
			POINT offset)=0;

		virtual void Move(
			POINT offset)=0;

		//////////////////////////////////////////////////////////
		POINT& FUNCWINOBJ::GetLocation()
			{
			return location;
			}

		virtual void SetLocation(POINT p)=0;

		virtual void UnSelect(
			HDC hdc);

		virtual void Select(
			HDC hdc);

		//Should check to see if any part of the object intersects the passed
		//in rectangle.  The default checks to see if the rectangle returned
		//by GetRect() intersects the passed in rectangle.
		virtual BOOL ObjectIntersectsRect(
			LPRECT bigRect);

		long GetStatus()
			{
			return status;
			}

		BOOL RectIntersectsSelectRect(
			LPRECT rect)
			{
			RECT temp;

			GetSelectedRect((LPRECT)&temp);
			return IntersectRect((LPRECT)&temp, (LPRECT)&temp, rect);
			}

		virtual BOOL AddObjectToZlist(
			LPRECT area,
			LPZLIST zlistPtr) = 0;

		BOOL TestFunctionFlag()
			{
			return (status & FWO_FUNCTION)?TRUE:FALSE;
			}

		BOOL TestCommentFlag()
			{
			return (status & FWO_COMMENT)?TRUE:FALSE;
			}

		BOOL TestObjGroupFlag()
			{
			return (status & FWO_OBJGROUP)?TRUE:FALSE;
			}

		long GetClass()
			{
			return (status & (FWO_COMMENT | FWO_FUNCTION | FWO_OBJGROUP));
			}

		void GetUnionWithSelectRect(
			LPRECT intersectRect,
			LPRECT unionRect);

		virtual void GetMaxScreenSize(
			LPRECT bounds)
			{
			GetSelectedRect(bounds);
			}

		//These routine select or unselect the object if it is being selected
		//or unselected with the selection rectangle.
		virtual void RectSelect(
			HDC hdc)
			{
			Select(hdc);
			}

		virtual void RectUnSelect(
			HDC hdc)
			{
			UnSelect(hdc);
			}

		void GetAnimateRect(
			LPRECT rect)
			{
			GetSelectedRect(rect);
			}

		///////////////////////////////////////////////////////////////////////////
		//Unselects the object and does the minimal required to erase the selection
		virtual void EraseSelection(
			HDC hdc)
			{
			UnSelect(hdc);
			}

		SUBJECT * GetSubjectPtr();

		virtual void GetSelectedRect(
			LPRECT bounds);

	};

#endif
