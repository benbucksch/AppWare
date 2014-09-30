#include "include.h"

////////

REGION::REGION(
	REGION_TYPE 						type, 
	COMBINE_MODE 						mode, 
	LPPOINT 								pPoints, 
	USHORT 									usNumPoints)
	{
	m_type						= type;
	m_combineMode			= mode; 
	m_usNumPoints			= usNumPoints;
	CopyMemory(m_aPoints, pPoints, sizeof(POINT) * usNumPoints);
	}

////////

size_t REGION::GetSize()
	{
	return sizeof(REGION) + sizeof(POINT) * m_usNumPoints;
	}

////////

int REGION::GetNativeCombineMode()
	{
	switch (m_combineMode)
		{
		case I_COMBINE_AND:			return RGN_AND;	
		case I_COMBINE_DIFF:		return RGN_DIFF;
		case I_COMBINE_OR:			return RGN_OR;	
		case I_COMBINE_XOR:			return RGN_XOR;	
		
		default:
		case I_COMBINE_INITIAL: return RGN_COPY;
		}
	}

////////


LPREGION REGION::CombineRegion(
	HRGN  * 							pHrgnPrev)
	{			
	HRGN										hrgnDest, hrgnThisOne;

	switch(m_type)
		{
		case I_RECT:
			I_ASSERT(m_usNumPoints==2);
			hrgnThisOne = CreateRectRgnIndirect( (LPRECT)m_aPoints);
			break;
		case I_ELLIPSE:
			I_ASSERT(m_usNumPoints==2);
			hrgnThisOne = CreateEllipticRgnIndirect( (LPRECT)m_aPoints);
			break;
		case I_POLYGON:
		default:
			hrgnThisOne = CreatePolygonRgn(m_aPoints, m_usNumPoints, WINDING);
			break;
		}

	hrgnDest = CreateRectRgn(0,0,0,0);

	if (*pHrgnPrev)
		{
		I_ASSERT(m_combineMode != I_COMBINE_INITIAL);
		CombineRgn(hrgnDest, *pHrgnPrev, hrgnThisOne, GetNativeCombineMode());
		DeleteObject(*pHrgnPrev);
		}
	else
		CombineRgn(hrgnDest, hrgnThisOne, NULL, RGN_COPY);

	DeleteObject(hrgnThisOne);
		
	*pHrgnPrev = hrgnDest;
	return (LPREGION) &m_aPoints[m_usNumPoints];
	}

void REGION::OffsetRegion(
	int 										dx, 
	int 										dy)
	{
	USHORT									us;

	for(us=0; us<m_usNumPoints; us++)
		{
		m_aPoints[us].x += dx;
		m_aPoints[us].y += dy;
		}
	}



void REGION::FrameRegion(
	HDC											hdc) //defaults to NULL
	{
	switch(m_type)
		{
		case I_RECT:
			Rectangle(hdc,	m_aPoints[0].x, 		m_aPoints[0].y, 
											m_aPoints[1].x, 		m_aPoints[1].y);
			break;

		case I_ELLIPSE:
			Ellipse(hdc,		m_aPoints[0].x, 		m_aPoints[0].y, 
											m_aPoints[1].x, 		m_aPoints[1].y);
			break;

		case I_POLYGON:
			Polygon(hdc, m_aPoints, m_usNumPoints);
			break;
		}

	}
