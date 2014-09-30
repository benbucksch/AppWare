#include "almsys.h"
#include <windowsx.h>
#include <stdlib.h>

#include "objgroup.h"
#include "rectypes.h"
#include "subject.h"
#include "progsys.h"
#include "recblk.h"
#include "cfglibp.h"
#include "compuid.h"
#include "zlist.h"
#include "serblk.h"
#include "objinst.h"
#include "sericon.h"
#include "mdiclass.h"
#include "navcls.h"
#include "palette.h"
#include "project.h"
#include "apbutil.h"
#include "basewin.h"
#include "list.h"
#include "oiputils.h"
#include "sjtwin.h"
#include "intdebug.h"
#include "objdesc.h"
#include "resconst.h"
#include "upsfname.h"

#define OBJGROUP_BORDER 1            	//width of border around group

//make sure the height is even so we can have a center two pixels hight
// with equal pixels above and below the center
#define OBJGROUP_NIPPLEHEIGHT		 10

//the nipple width should be half the height
#define OBJGROUP_NIPPLEWIDTH	OBJGROUP_NIPPLEHEIGHT/2

#define OBJGROUP_TEXTBORDER 4        	//space between icon and text

#define OBJGROUP_VOBJECTSEPARATION 2  //vertical space between objects - must

#define OBJGROUP_HOBJECTBORDER 2    	//horizontal space between icon and
																			//either pref icon or left border.

#define OBJGROUP_BORDERTOLERANCE 3

#define OBJGROUP_SMALLFONTSIZE 16
#define OBJGROUP_CAPTIONFONTSIZE 16
#define OBJGROUP_LARGEFONTSIZE 18

//distance between the object's right side and the rightmost side of the
//box which is drawn around the object to signal it's eligbile for
//connection.
#define OBJGROUP_BOXBORDER OBJGROUP_NIPPLEWIDTH


//the tolerance should be greater than half the height
#define OBJGROUP_NIPPLETOLERANCE 3

//width of selection rectangle
#define OBJGROUP_SELRECTWIDTH 4

//used for animating the addition of an object to a group
#define OBJGROUP_ADDANIMATESTEPS 15
#define OBJGROUP_ADDANIMATEDELAY 10L

#define OBJGROUP_EDITCONTROL -1 //ID of edit control

#define OBJGROUP_MAXOBJNAME 64

#define OBJGROUP_MAXPARTIALREFRESH 2 	//number of objects that can change
																			//selection state before all the objects
																			//need to be redrawn instead of just the
                                      //changed objects.

#define OBJGROUP_GROUPNAMEHITTOLERANCE	5 //the amount after the last character
																			//in the name you can be withing and
																			//still call it a text hit instead of
																			//an icon hit.

#define WM_USER_DOEDITKILL (WM_USER + 100)

//insertMarkPen must be set to NULL
HPEN	OBJGROUP::insertMarkPen=NULL;
//must set groupCount to 0
long	OBJGROUP::groupCount = 0;

HFONT 	OBJGROUP::small_normalFont;
HFONT 	OBJGROUP::small_sharedFont;
HFONT 	OBJGROUP::small_aliasFont;
HFONT 	OBJGROUP::large_normalFont;
HFONT 	OBJGROUP::large_sharedFont;
HFONT 	OBJGROUP::large_aliasFont;
HFONT 	OBJGROUP::small_persistentNormalFont;
HFONT 	OBJGROUP::small_persistentSharedFont;
HFONT 	OBJGROUP::small_persistentAliasFont;
HFONT 	OBJGROUP::large_persistentNormalFont;
HFONT 	OBJGROUP::large_persistentSharedFont;
HFONT 	OBJGROUP::large_persistentAliasFont;
HFONT 	OBJGROUP::captionFont;
HBITMAP OBJGROUP::nippleNormal;
HBITMAP OBJGROUP::nippleInverted;
HBITMAP OBJGROUP::nippleMask;
HBITMAP OBJGROUP::nippleSelected;
HBITMAP OBJGROUP::selRectPattern;
HBRUSH	OBJGROUP::selRectBrush;
HWND		OBJGROUP::editField=NULL;
WNDPROC	OBJGROUP::oldEditProc;
int 		OBJGROUP::largeSlot;
int 		OBJGROUP::smallSlot;
int 		OBJGROUP::largeVBorder;
int 		OBJGROUP::smallVBorder;
int			OBJGROUP::smallTextBorder;
int			OBJGROUP::largeTextBorder;
SERUID	OBJGROUP::editObject = NULLSERUID;
char		OBJGROUP::nameBuffer[OBJGROUP_MAXNAMELENGTH + 1];
WNDPROC OBJGROUP::oldObjValueProc;

//NOTES:  objBounds does not include the nipple width

OBJGROUP::OBJGROUP(
	SERUID objUID,
	POINT  newLocation,  // SKJ
	SERUID ownerSubject,
	LPCSTR newName):FUNCWINOBJ(ownerSubject, newLocation),
		groupName(newName)
	{
	LPSUBJECT subjectPtr;
	RECT winBounds;

  //groupInfo must be created before any objects are created.
	groupInfo = new GROUPINFO();

	SetObjGroupFlag();

	subjectPtr = GetSubjectPtr();
	//Create the record block to hold the objects
	objIndexes = GetUID(new SERIUSBLOCK((long)0));

	objCount = 0;
	selObjCount = 0;

	// unused1 = unused2 = 0;  SKJ

	//must initialize before calculating the captions size
	if(!groupCount)
		Initialize();

	//Set the group status based on defaults
	groupStatus = 0L;

	if(subjectPtr->TestBigIconFlag())
		SetLargeIconsFlag();

	//calculate the objBounds by assigning default width 1/6 the width of
	//the funcwindow.
	subjectPtr->GetSubjectClientBounds((LPRECT)&winBounds);
	objBounds.left = location.x;
	//set the top of the objBounds.
	objBounds.top = location.y + GetCaptionHeight();

	objBounds.right = objBounds.left + (winBounds.right - winBounds.left)/6 +
	 OBJGROUP_BORDER;

	objBounds.bottom = objBounds.top;

  if(objUID != NULLSERUID)
		//add the object to the group.
		AddObject(objUID);

	groupCount++;
	}

////////////////////////////////////////////////////////////////////////////
//constructor for creating a group from a clipboard group.
OBJGROUP::OBJGROUP(
	SERUID ownerSubject,
	POINT newLocation,
	long baseStatus, // SKJ
	long groupStats,
	int width,
	int objectCount,
	LPCSTR newName,
	LPGROUPINFO infoPtr):FUNCWINOBJ(ownerSubject, newLocation),
		objCount(objectCount),
		groupStatus(groupStats),
		groupName(newName)
	{
	//must initialize before calculating the captions size
	if(!groupCount)
		Initialize();

	objIndexes = GetUID(new SERIUSBLOCK(sizeof(SERUID) * objCount));

	groupInfo = new GROUPINFO();
	groupInfo->mark = infoPtr->mark;

	status = baseStatus; 	//assign here since in FUNCWINOBJ constructor it was
												//set to zero.
	objBounds.left = location.x;
	objBounds.top = location.y+GetCaptionHeight();
  objBounds.right = objBounds.left + width - 1; 

	selObjCount = 0;

	CalculateBounds();

	groupCount++;
  }

////////////////////////////////////////////////////////////////////////////
OBJGROUP::OBJGROUP(
	void * thisPtr):FUNCWINOBJ(thisPtr), groupName(&groupName)
	{
	groupInfo = new GROUPINFO();
	if(!groupCount)
		Initialize();

	groupCount++;
	//default the mark
  groupInfo->mark = GetTopSelectedIndex();
  }

/////////////////////////////////////////////////////////////////////////////
OBJGROUP::~OBJGROUP()
	{
  if(GetUID(this) == GetSubjectPtr()->GetActiveGroup())
  	{
    GetSubjectPtr()->SetActiveGroup(NULLSERUID);
    }
    
	delete (LPSERIUSBLOCK)GetObject(objIndexes);
	groupCount--;
	if(!groupCount)
		UnInitialize();

	delete groupInfo;
	}


////////////////////////////////////////////////////////////////////////////
BOOL OBJGROUP::Initialize()
	{
	BOOL result = TRUE;
	HDC hdc;
	HDC hdcMem;
	POINT poly[4];
	TEXTMETRIC tm;

	if(!insertMarkPen)
		{
		insertMarkPen = CreatePen(PS_DOT, 1, PROGSYSTEM::highlightColor);
		if(!insertMarkPen)
			result = FALSE;

		small_normalFont = CreateFont(OBJGROUP_SMALLFONTSIZE,
			0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		 ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		 VARIABLE_PITCH|FF_SWISS, "Arial");
		small_sharedFont = CreateFont(OBJGROUP_SMALLFONTSIZE,
			0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		 ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		 VARIABLE_PITCH|FF_SWISS, "Arial");
		small_aliasFont = CreateFont(OBJGROUP_SMALLFONTSIZE,
			0, 0, 0, FW_NORMAL, TRUE, FALSE, FALSE,
		 ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		 VARIABLE_PITCH|FF_SWISS, "Arial");

		if(!small_normalFont || !small_sharedFont || !small_aliasFont)
			result = FALSE;

		large_normalFont = CreateFont(OBJGROUP_LARGEFONTSIZE,
			0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		 ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		 VARIABLE_PITCH|FF_SWISS, "Arial");
		large_sharedFont = CreateFont(OBJGROUP_LARGEFONTSIZE,
			0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		 ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		 VARIABLE_PITCH|FF_SWISS, "Arial");
		large_aliasFont = CreateFont(OBJGROUP_LARGEFONTSIZE,
			0, 0, 0, FW_NORMAL, TRUE, FALSE, FALSE,
		 ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		 VARIABLE_PITCH|FF_SWISS, "Arial");

		if(!large_normalFont || !large_sharedFont || !large_aliasFont)
			result = FALSE;

		small_persistentNormalFont = CreateFont(OBJGROUP_SMALLFONTSIZE,
			0, 0, 0, FW_NORMAL, FALSE, TRUE, FALSE,
		 ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		 VARIABLE_PITCH|FF_SWISS, "Arial");
		small_persistentSharedFont = CreateFont(OBJGROUP_SMALLFONTSIZE,
			0, 0, 0, FW_BOLD, FALSE, TRUE, FALSE,
		 ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		 VARIABLE_PITCH|FF_SWISS, "Arial");
		small_persistentAliasFont = CreateFont(OBJGROUP_SMALLFONTSIZE,
			0, 0, 0, FW_NORMAL, TRUE, TRUE, FALSE,
		 ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		 VARIABLE_PITCH|FF_SWISS, "Arial");

		if(!small_persistentNormalFont || !small_persistentSharedFont ||
			!small_persistentAliasFont)
			result = FALSE;

		large_persistentNormalFont = CreateFont(OBJGROUP_LARGEFONTSIZE,
			0, 0, 0, FW_NORMAL, FALSE, TRUE, FALSE,
		 ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		 VARIABLE_PITCH|FF_SWISS, "Arial");
		large_persistentSharedFont = CreateFont(OBJGROUP_LARGEFONTSIZE,
			0, 0, 0, FW_BOLD, FALSE, TRUE, FALSE,
		 ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		 VARIABLE_PITCH|FF_SWISS, "Arial");
		large_persistentAliasFont = CreateFont(OBJGROUP_LARGEFONTSIZE,
			0, 0, 0, FW_NORMAL, TRUE, TRUE, FALSE,
		 ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		 VARIABLE_PITCH|FF_SWISS, "Arial");

		if(!large_persistentNormalFont || !large_persistentSharedFont ||
			!large_persistentAliasFont)
			result = FALSE;

		captionFont = CreateFont(OBJGROUP_CAPTIONFONTSIZE, 0, 0, 0, FW_MEDIUM, FALSE, FALSE,
			FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				VARIABLE_PITCH|FF_SWISS, "Arial");

		if(!captionFont)
			result = FALSE;

		//Create the nipple bitmaps
		hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
		hdcMem = CreateCompatibleDC(hdc);

		nippleMask = CreateCompatibleBitmap(hdc, OBJGROUP_NIPPLEWIDTH,
			OBJGROUP_NIPPLEHEIGHT);

		nippleNormal = CreateCompatibleBitmap(hdc, OBJGROUP_NIPPLEWIDTH,
			OBJGROUP_NIPPLEHEIGHT);

		nippleInverted = CreateCompatibleBitmap(hdc, OBJGROUP_NIPPLEWIDTH,
			OBJGROUP_NIPPLEHEIGHT);

		nippleSelected = CreateCompatibleBitmap(hdc, OBJGROUP_NIPPLEWIDTH,
			OBJGROUP_NIPPLEHEIGHT);

		if(!nippleMask || !nippleInverted || !nippleNormal || !nippleSelected)
			result = FALSE;
		else
			{
      POINT oldLoc;
			//poly[0] is the top point
				//poly[1] is center point 1
				//poly[2] is center point 2
			//poly[3] is the bottom point

			poly[0].x = 0;
			poly[0].y = 0;

			//subtract 1 since everything is zero based
			poly[1].x = OBJGROUP_NIPPLEWIDTH-1;
			poly[1].y = OBJGROUP_NIPPLEHEIGHT/2 - 1;

			poly[2].x = poly[1].x;
			poly[2].y = poly[1].y+1;


			poly[3].x = poly[0].x;
			poly[3].y = OBJGROUP_NIPPLEHEIGHT -1;


			//create the mask with everything white but the solid nipple
			SelectObject(hdcMem, nippleMask);
			BitBlt(hdcMem, 0, 0, OBJGROUP_NIPPLEWIDTH, OBJGROUP_NIPPLEHEIGHT,
				NULL, 0, 0, WHITENESS);
			SelectObject(hdcMem, GetStockObject(BLACK_PEN));
			SelectObject(hdcMem, GetStockObject(BLACK_BRUSH));

			Polygon(hdcMem, (LPPOINT)&poly, 4);

			//create the nipple for highilghted objects with everything black
			//except the nipple which is drawn the normal way.
			SelectObject(hdcMem, nippleSelected);
			BitBlt(hdcMem, 0, 0, OBJGROUP_NIPPLEWIDTH, OBJGROUP_NIPPLEHEIGHT,
				NULL, 0, 0, BLACKNESS);

			SelectObject(hdcMem, PROGSYSTEM::framePen);
			SelectObject(hdcMem, PROGSYSTEM::windowBrush);

			Polygon(hdcMem, (LPPOINT)poly, 4);

			//now cause the vertical border to be the frame color
			SelectObject(hdcMem, PROGSYSTEM::framePen);
			MoveToEx(hdcMem, poly[0].x, poly[0].y+1, &oldLoc);
			LineTo(hdcMem, poly[3].x, poly[3].y);

			//now create the normal nipple with the vertical border the
			//window color;
			SelectObject(hdcMem, nippleNormal);
			BitBlt(hdcMem, 0, 0, OBJGROUP_NIPPLEWIDTH, OBJGROUP_NIPPLEHEIGHT,
				NULL, 0, 0, BLACKNESS);

			SelectObject(hdcMem, PROGSYSTEM::framePen);
			SelectObject(hdcMem, PROGSYSTEM::windowBrush);

			Polygon(hdcMem, (LPPOINT)poly, 4);

			//now cause the vertical border to be the highlight color
			SelectObject(hdcMem, PROGSYSTEM::windowPen);
			MoveToEx(hdcMem, poly[0].x, poly[0].y+1, NULL);
			LineTo(hdcMem, poly[3].x, poly[3].y);     

			//create the inverted nipple with everything black excepth the nipple
			//which is drawn the highlight color
			SelectObject(hdcMem, nippleInverted);
			BitBlt(hdcMem, 0, 0, OBJGROUP_NIPPLEWIDTH, OBJGROUP_NIPPLEHEIGHT,
				NULL, 0, 0, BLACKNESS);
			SelectObject(hdcMem, PROGSYSTEM::highlightPen);
			SelectObject(hdcMem, PROGSYSTEM::highlightBrush);

			Polygon(hdcMem, (LPPOINT)poly, 4);
			}

		//create selection rect pattern
		selRectPattern = LoadBitmap(PROGSYSTEM::hInst,
			(LPSTR)"GROUPSELRECTPATTERN");
		if(!selRectPattern)
			result = FALSE;

		selRectBrush = CreatePatternBrush(selRectPattern);
		if(!selRectBrush)
			result = FALSE;

		//determine the slot height of objects as well as the
		//height of the vborder between the object and border.
		//Also, figure the border between the top of the text rectangle and
		//the objects border - this could be different than the vborder if
		//the text is much smaller than the icon size. This is important
		//because we need to make sure the textRectangle is centered and only
    //as big as the text.

		//get font characteristics
		GetTextMetrics(hdc, &tm);

		//figure slot and border for small size
		SelectObject(hdc, small_normalFont);
    GetTextMetrics(hdc, (LPTEXTMETRIC)&tm);
		if(tm.tmHeight > OBJGROUP_SMALLICONSIZE)
			{
			//we need to increase the separation size so the text will
			//fit.
      //Add 1 to insure that we round up
			smallVBorder = (tm.tmHeight - OBJGROUP_SMALLICONSIZE + 1)/2;
      smallTextBorder = smallVBorder;
			}
		else
			{
			smallVBorder = OBJGROUP_VOBJECTSEPARATION;
			smallTextBorder = (OBJGROUP_SMALLICONSIZE - tm.tmHeight + 1)/2;
			//if the text border is zero, the text rect will overdraw the bottom border
			//of the group.
			if(smallTextBorder == 0)
				smallTextBorder = 1;
			}

    smallSlot = OBJGROUP_SMALLICONSIZE + (2 * smallVBorder);

		//figure slot and border for large objects using same logic as for
    //small icons.
		SelectObject(hdc, large_normalFont);
		GetTextMetrics(hdc, (LPTEXTMETRIC)&tm);
		if(tm.tmHeight > OBJGROUP_LARGEICONSIZE)
			{
			largeVBorder = (tm.tmHeight - OBJGROUP_LARGEICONSIZE + 1)/2;
      largeTextBorder = largeVBorder;
			}
		else
			{
			largeVBorder = OBJGROUP_VOBJECTSEPARATION;
			largeTextBorder = (OBJGROUP_LARGEICONSIZE - tm.tmHeight + 1)/2;
      }

		largeSlot = OBJGROUP_LARGEICONSIZE + (2 * largeVBorder);



		DeleteDC(hdcMem);
		DeleteDC(hdc);

		}
	return result;
	}

////////////////////////////////////////////////////////////////////////////
BOOL OBJGROUP::UnInitialize()
	{
	if(insertMarkPen)
		{
		DeleteObject(insertMarkPen);
		//must set insertMarkPen to null to enable re-creation
		insertMarkPen = NULL;
		}

	if(small_normalFont)
		DeleteObject(small_normalFont);
	if(small_sharedFont)
		DeleteObject(small_sharedFont);
	if(small_aliasFont)
		DeleteObject(small_aliasFont);

	if(large_normalFont)
		DeleteObject(large_normalFont);
	if(large_sharedFont)
		DeleteObject(large_sharedFont);
	if(large_aliasFont)
		DeleteObject(large_aliasFont);

	if(small_persistentNormalFont)
		DeleteObject(small_persistentNormalFont);
	if(small_persistentSharedFont)
		DeleteObject(small_persistentSharedFont);
	if(small_persistentAliasFont)
		DeleteObject(small_persistentAliasFont);

	if(large_persistentNormalFont)
		DeleteObject(large_persistentNormalFont);
	if(large_persistentSharedFont)
		DeleteObject(large_persistentSharedFont);
	if(large_persistentAliasFont)
		DeleteObject(large_persistentAliasFont);

	if(captionFont)
		DeleteObject(captionFont);


	if(nippleNormal)
		DeleteObject(nippleNormal);

	if(nippleInverted)
		DeleteObject(nippleInverted);

	if(nippleMask)
		DeleteObject(nippleMask);

	if(nippleSelected)
		DeleteObject(nippleSelected);

	if(selRectPattern)
		DeleteObject(selRectPattern);

	if(selRectBrush)
		DeleteObject(selRectBrush);

	return TRUE; //signal no error
	}

////////////////////////////////////////////////////////////////////////////
//Causes the entire list to redraw
void OBJGROUP::Draw(
	HDC hdc)
	{
	// the group will be drawn offscreen.
	Draw(hdc, NULL);
	}

////////////////////////////////////////////////////////////////////////////
//Only draws stuff that intersects drawRect without drawing the selection rect.
//If drawRect is NULL, then the entire group is drawn
BOOL OBJGROUP::DrawGroupBody(
	HDC hdc,
	LPRECT drawRect,
	BOOL drawOffScreen)
	{
	HDC hdcMem;
	HBITMAP bmp;
	int bitmapWidth = objBounds.right - objBounds.left + OBJGROUP_NIPPLEWIDTH;
	int bitmapHeight = objBounds.bottom - location.y;
	HDC drawDC;
	RECT rc;

	//check to see if anything needs to be redrawn.
	if(drawRect && !ObjectIntersectsRect(drawRect))
		return FALSE;

	if(drawOffScreen)
		{
    POINT oldLoc; // old origin
		//setup offscreen drawing.
		bmp = CreateCompatibleBitmap(hdc, bitmapWidth, bitmapHeight);
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, bmp);

		//color the bitmap the screen color
		rc.left = rc.top = 0;
		rc.right = bitmapWidth;
		rc.bottom = bitmapHeight;
		FillRect(hdcMem, (LPRECT)&rc, PROGSYSTEM::windowBrush);

		//Set the origin so everthing will just draw normally
		SetWindowOrgEx(hdcMem, location.x, location.y, &oldLoc);

		//point drawDC to the memory dc.
		drawDC = hdcMem;
		}
	else
		drawDC = hdc;

	//we need to make sure that after saving the DC, we don't return until
	//the DC is restored.
	DrawCaption(drawDC);

  //pass FALSE so another bitmap isn't created to draw offscreen;
	DrawAllObjects(drawDC, drawRect, FALSE);

	if(drawOffScreen)
  	{
		BitBlt(hdc, location.x, location.y, bitmapWidth, bitmapHeight, hdcMem,
			location.x, location.y, SRCCOPY);
		DeleteDC(hdcMem);
		DeleteObject(bmp);
		}

	return TRUE;

	}

///////////////////////////////////////////////////////////////////////
//Draws the body plus selection rect if necessary.
BOOL OBJGROUP::Draw(
	HDC hdc,
	LPRECT drawRect,
	BOOL drawOffScreen)
	{
	DrawGroupBody(hdc, drawRect, drawOffScreen);
	if(status & FWO_SELECTED)
		DrawSelectionRect(hdc);

	return TRUE;
	}

////////////////////////////////////////////////////////////////////////////////
//Only draws stuff that intersects drawRect
//If drawRect is NULL, then the entire group is drawn
void OBJGROUP::DrawAllObjects(
	HDC hdc,
	LPRECT drawRect,
	BOOL drawOffScreen)
	{
	LPSERUID objIndexPtr;
	LPOBJINST objPtr;
  int i;
	RECT rc;
	HDC drawDC;
	HDC hdcMem;
  int oldDC;
	HBITMAP bmp;
	int slotHeight;
	RECT temp;
	int bitmapWidth = objBounds.right - objBounds.left + OBJGROUP_NIPPLEWIDTH;
	int bitmapHeight = objBounds.bottom - objBounds.top;

	if(drawOffScreen)
		{
    POINT oldLoc;
		//setup offscreen drawing.
		bmp = CreateCompatibleBitmap(hdc, bitmapWidth, bitmapHeight);
		hdcMem = CreateCompatibleDC(hdc);
		SelectObject(hdcMem, bmp);

		//color the bitmap the screen color
		rc.left = rc.top = 0;
		rc.right = bitmapWidth;
		rc.bottom = bitmapHeight;
		FillRect(hdcMem, (LPRECT)&rc, PROGSYSTEM::windowBrush);

		//set the window origin so things can just draw normally.
		SetWindowOrgEx(hdcMem, objBounds.left, objBounds.top, &oldLoc);

		drawDC = hdcMem;
		}
	else
		{
		oldDC = SaveDC(hdc);
		drawDC = hdc;
		}

 	//draw the border around the objects and nipples as well
	//add 1 to the right to force the border drawn to correspond to the
	//actual border
	SelectObject(drawDC, PROGSYSTEM::framePen);
	SelectObject(drawDC, GetStockObject(NULL_BRUSH));
	Rectangle(drawDC, objBounds.left, objBounds.top, objBounds.right+1,
		objBounds.bottom);


	//draw objects
	//set up the initial bounds for calculating each object's rectangle
	rc.left = objBounds.left + 1;
	rc.right = objBounds.right + OBJGROUP_NIPPLEWIDTH;
	rc.bottom = objBounds.top;
  slotHeight = GetSlotHeight();
	objIndexPtr = GetIndexesPtr();

	for(i=0;i<objCount;i++)
		{
		objPtr = (LPOBJINST)GetObject(objIndexPtr[i]);
		rc.top = rc.bottom ;
		rc.bottom += slotHeight;

		if(!drawRect || IntersectRect((LPRECT)&temp, (LPRECT)&rc, drawRect))
			{
			//we should already be drawing to a bitmap so make sure the object
      //doesn't draw offscreen as well.
			rc.right-=OBJGROUP_NIPPLEWIDTH;
			DrawObject(drawDC, objPtr, (LPRECT)&rc, FALSE);
			rc.right+=OBJGROUP_NIPPLEWIDTH;
      }
		}

	if(drawOffScreen)
		{
		BitBlt(hdc, objBounds.left, objBounds.top, bitmapWidth, bitmapHeight,
			drawDC, objBounds.left, objBounds.top, SRCCOPY);
		DeleteDC(hdcMem);
		DeleteObject(bmp);
		}
	else
		RestoreDC(hdc, oldDC);

	}


////////////////////////////////////////////////////////////////////////////
void OBJGROUP::DrawCaption(
	HDC hdc)
	{
	RECT rc;
	int oldDC = SaveDC(hdc);

	//set up the text rectangle
	rc.left = location.x+1;
	rc.right = objBounds.right;
	rc.top = location.y +1;
	rc.bottom = objBounds.top -1;

	//Add 1 to objBounds.right to force the drawn border to correspond to the
	//actual border.
	if(TestActiveFlag())
		{
		SelectObject(hdc, PROGSYSTEM::captionBrush);
		SetTextColor(hdc, PROGSYSTEM::captionTextColor);
		}
	else
		{
		SelectObject(hdc, PROGSYSTEM::inactiveCaptionBrush);
		SetTextColor(hdc, PROGSYSTEM::inactiveCaptionTextColor);
		}

	SelectObject(hdc, PROGSYSTEM::framePen);

	Rectangle(hdc, location.x, location.y, objBounds.right+1,
		objBounds.top);
	SetBkMode(hdc, TRANSPARENT);
	SelectObject(hdc, captionFont);
	DrawText(hdc, groupName, groupName.strlen(), (LPRECT)&rc,
		DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_NOPREFIX);
	RestoreDC(hdc, oldDC);
	}

////////////////////////////////////////////////////////////////////////////
void OBJGROUP::DrawSelection(
	HDC hdc)
	{
	if(!(status & FWO_SELECTED))
		DrawSelectionRect(hdc);  //need to erase selection rect
	Draw(hdc);
	}


////////////////////////////////////////////////////////////////////////////
void OBJGROUP::GetRect(
	LPRECT rect)
	{
	rect->left = location.x - OBJGROUP_SELRECTWIDTH;
	rect->top = location.y - OBJGROUP_SELRECTWIDTH;
	rect->right = objBounds.right + OBJGROUP_NIPPLEWIDTH +
		OBJGROUP_SELRECTWIDTH;
	rect->bottom = objBounds.bottom + OBJGROUP_SELRECTWIDTH;
	}

////////////////////////////////////////////////////////////////////////////
void OBJGROUP::GetTotalBounds(
	LPRECT bounds)
	{
	LPSERUID dataPtr;
	int i;

	I_ASSERT(bounds);

	GetRect(bounds);

	dataPtr = GetIndexesPtr();
	for(i=0;i<objCount;i++)
		((LPOBJINST)GetObject(dataPtr[i]))->GetUnionFlowBounds(bounds);
		
	}


////////////////////////////////////////////////////////////////////////////
void OBJGROUP::DrawDrag(
	HDC hdc,
	POINT offset)
	{
	POINT pt = location;
	int oldMode;
	COLORREF oldColor;
	COLORREF oldBkColor;
	HPEN oldPen;
	HBRUSH oldBrush;
	HDC parentDC;
	HWND parent;
	POINT windowOrg; // SKJ

	pt.x+=offset.x;
	pt.y+=offset.y;

	oldMode = SetROP2(hdc, R2_XORPEN);
	oldColor = SetTextColor(hdc, PROGSYSTEM::dragColor);
	oldBkColor = SetBkColor(hdc, PROGSYSTEM::windowColor);
	oldPen = SelectPen(hdc, GetStockObject(NULL_PEN));

	//MICORSOFT really sucks fix!!!!!!
	//What microsoft failed to mention is that for a device context
	//associated with a window, the brush origin is set according to
	//screen coordinates.  However, if the DC is a memory DC, then the
	//brush origin is set releative to the upper left corner of the
	//offscreen drawing area.  So what we want to do, is convert the point
	//to client coordinates of the offscreen drawing area.
	parent = GetSubjectPtr()->GetWindowPtr()->GetWindowHandle();   // SKJ When did this happen
	parentDC = GetDC(parent);

	//if the DC belongs to the parent window.
	if(parentDC ==hdc)
		{
		LPtoDP(parentDC, (LPPOINT)&pt, 1);
		ClientToScreen(parent, (LPPOINT)&pt);
		}
	else
		{
    // SKJ
		if (GetWindowOrgEx(hdc, &windowOrg))
			{
			pt.x-= (short)windowOrg.x;
			pt.y-=(short)windowOrg.y;
      }
		}

	ReleaseDC(parent, parentDC);
	UnrealizeObject(selRectBrush);
	SetBrushOrgEx(hdc, pt.x, pt.y, NULL);
	oldBrush = SelectBrush(hdc, selRectBrush);

	DrawTheSelectionRects(hdc, (LPPOINT)&offset);
  
	//clean up
	SetROP2(hdc, oldMode);
	SetTextColor(hdc, oldColor);
	SetBkColor(hdc, oldBkColor);
	SelectPen(hdc, oldPen);
	SelectBrush(hdc, oldBrush);
	}

////////////////////////////////////////////////////////////////////////////
void OBJGROUP::Move(
	POINT offset)
	{
	location.x+=offset.x;
	location.y+=offset.y;
	OffsetRect((LPRECT)&objBounds, offset.x, offset.y);
	}

////////////////////////////////////////////////////////////////////////////
//Can only be called after location has an initial value
void OBJGROUP::SetLocation(
	POINT p)
	{
	POINT offset;

	offset.x = p.x - location.x;
	offset.y = p.y - location.y;

	Move(offset);
	}


////////////////////////////////////////////////////////////////////////////
BOOL OBJGROUP::AddObjectToZlist(
	LPRECT area,
	LPZLIST zlistPtr)
	{
	RECT rect;

	if(ObjectIntersectsRect(area))
		{
		GetSelectedRect((LPRECT)&rect);
		zlistPtr->Add((LPRECT)&rect, Z_OBJGROUP, GetUID(this));
		return TRUE;
		}
	return FALSE;
	}


////////////////////////////////////////////////////////////////////////////
BOOL OBJGROUP::SetSysColors()
	{
	if(groupCount)
		{
		UnInitialize();

		if(!Initialize())
			return FALSE;
		}

	return TRUE;

	}


////////////////////////////////////////////////////////////////////////////
//adds the UID of an object to the end of the group unless a location
//is specified.  If a location is specified, the object is inserted.
//The index of the object in the group is returned.
//locationPtr is an optional parameter that defaults to NULL
int OBJGROUP::AddObject(
	SERUID objUID,
	LPPOINT locationPtr)
	{
	int index = objCount+1; //default to being inserted at end

	if(locationPtr)
		{
		//find out where to insert the object
		index = GetNewObjIndex(locationPtr->y);
		//if the index is to small or too large, don't worry.  InsertObject
		//will automatically insert the object at the correct place.
		}

	return InsertObject(objUID, index);
	}


int OBJGROUP::InsertObject(
	SERUID objUID,
	int index)
	{
	LPOBJINST objPtr;

	VerifyInsertIndex(&index);

  //if there are any selected objects, unselect them without redrawing.
	if(selObjCount)
		UnselectObjects(NULL, FALSE, NULLSERUID);

  index = DoInsert(objUID, index);
	CalculateBounds(objCount == 1);
	objPtr = (LPOBJINST)GetObject(objUID);
	objPtr->SetGroupUID(GetUID(this));
	objPtr->SetIndex(index);
	objPtr->SetSelectFlag();
	groupInfo->mark = index;
	selObjCount++;
	return index;
	}

/////////////////////////////////////////////////////////////////////
//insert objects which belonged to the same group or to another group
//removes the object from the old group
//all objects will be selected
//purges the insertObjects list	and the groupList
//resets all the groups obj indexes and deletes groups whose objcount went to zero.
BOOL OBJGROUP::InsertObjects(
	LPLIST groupList,
	LPLIST insertObjects,
	int insertIndex,
	BOOL	moveFunctions)
	{
	POINT newNipple;
	LPINSERTOBJDATA insertData;
	LPOBJINST objPtr;
	LPOBJGROUP groupPtr;
	LPSERUID dataPtr;
	LPUIDLIST member;
	int addCount=0;
  SERUID thisUID = GetUID(this);
	BOOL result = TRUE; //assume success
	int delCount;

	if(groupList)
		{ 
		member = NULL;
		while(NULL != (member = (LPUIDLIST)groupList->GetNextMember(member)))
			{
			groupPtr = (LPOBJGROUP)GetObject(member->uid);
			if(member->uid == thisUID)
				//objects are going to be deleted so the selectionIndex needs adjusting
				delCount = groupPtr->RemoveAllSelObjects(FALSE, &insertIndex);
			else
				delCount = groupPtr->RemoveAllSelObjects(FALSE);

			if(delCount == -1)
				{
				result = FALSE;
				break;
				}
			else
				addCount+=delCount;
			}
		}
	else
		{
		//get the add count from the insertObjects
		insertData = NULL;
		while(NULL != (insertData = (LPINSERTOBJDATA)insertObjects->GetNextMember(insertData)))
			addCount++;
    }

	if(result)
  	{
		VerifyInsertIndex(&insertIndex);

		//make sure all objects are unselected before adding new objects
		//Objects could be selected if the group was selected.
		if(selObjCount)
			UnselectAllObjects(NULL);

		((LPSERIUSBLOCK)GetObject(objIndexes))->ResizeBlock(sizeof(SERUID) * addCount);
		dataPtr = GetIndexesPtr();
		if(dataPtr)
			{
			if(insertIndex < objCount)
				MoveMemory((dataPtr + insertIndex + addCount), (dataPtr + insertIndex),
					sizeof(SERUID) * (objCount - insertIndex));

			//now shove in the new objects and purge the drag list
			while(NULL != (insertData = (LPINSERTOBJDATA)insertObjects->GetNextMember(NULL)))
				{
				dataPtr[insertIndex] = insertData->uid;
        objPtr = (LPOBJINST)GetObject(dataPtr[insertIndex]);
				objPtr->SetSelectFlag();
				objPtr->SetGroupUID(thisUID);
				selObjCount++;
				objCount++;

				if(moveFunctions)
					{
					//go through all functions which are connected to the object and move
					//any selected functions so they maintain their relative position.
					objPtr->SetIndex(insertIndex); //so we can get the new nipple point
					GetNippleTip((LPPOINT)&newNipple, objPtr);
					objPtr->MoveFunctionChainRelative(newNipple, insertData->oldNipple);
				 }
				insertIndex++;
				insertObjects->Remove(insertData);
				delete insertData;
				}

			ResetObjIndexes();
			CalculateBounds();

			if(moveFunctions)
				GetSubjectPtr()->ClearAllChainFlags();

			//now go through and purge the groupList - delete any groups which
			//no longer contain any objects

			if(groupList)
      	{
				while(NULL != (member = (LPUIDLIST)groupList->GetNextMember(NULL)))
					{
					groupPtr = (LPOBJGROUP)GetObject(member->uid);
					//make sure the group has objects in it
					if(groupPtr->GetObjCount() == 0)
						delete groupPtr;

					//delete the group from the groupList
					groupList->Remove(member);
					delete member;
					}
				}
			}
		else
			result = FALSE;
    }

  return result;
	}

//Insert's an objects based on the old indexes.
//Will crash unless called as a
//result of an undo operation.
int OBJGROUP::InsertUndoneObject(
	LPOBJINST objPtr)
	{
	LPSERUID dataPtr;
	 int i;
  LPOBJINST tempObj;
  int actualIndex=objCount; //default to end of list

	((LPSERIUSBLOCK)GetObject(objIndexes))->ResizeBlock(sizeof(SERUID));
	dataPtr = GetIndexesPtr();

	//find where the object actually goes
	for(i=0;i<objCount;i++)
		{
		tempObj = (LPOBJINST)GetObject(dataPtr[i]);
		if(tempObj->GetOldIndex() > objPtr->GetOldIndex())
			{
			actualIndex = i;
			break;
			}
		}

	objCount++;
	selObjCount++;
	if(actualIndex < objCount - 1)
		{
		//need make room to insert the object
		MoveMemory((dataPtr + actualIndex + 1), (dataPtr + actualIndex),
			sizeof(SERUID) * (objCount - 1 - actualIndex));
		}

	dataPtr[actualIndex] = GetUID(objPtr);
	objPtr->SetIndex(actualIndex);
	objPtr->SetGroupUID(GetUID(this));

	if(actualIndex < objCount - 1)
		{
		//we need to update the object Indexes which were moved
		for(i = actualIndex+1;i<objCount;i++)
			((LPOBJINST)GetObject(dataPtr[i]))->SetIndex(i);

		}
	CalculateBounds();

	return actualIndex;
	}


//////////////////////////////////////////////////////////////////////////////
//does not recalculate the bounds or adjust the obj indexes
int OBJGROUP::OnlyRemoveObject(
	LPOBJINST objPtr)
	{
	LPSERUID dataPtr = GetIndexesPtr();
	int index = objPtr->GetIndex();

	//make sure the object exists
	if(index >= 0 && index < objCount)
  	{
		if(dataPtr[index] == GetUID(objPtr))
			{
			//we found the object

      //shift all records after the deleted object to the left
      if(index < objCount)
				MoveMemory((dataPtr + index), (dataPtr + index + 1), sizeof(SERUID) * (objCount - 1 - index));

			objCount--;
			if(objPtr->TestSelectFlag())
				selObjCount--;

      if(objCount > 0)
				((LPSERIUSBLOCK)GetObject(objIndexes))->ResizeBlock(-((long)sizeof(SERUID)));

			}
		else
			I_ASSERT("Could not obtain an object pointer");
		}
	else
		I_ASSERT("Invalid object index");

	return objCount;
	}

//////////////////////////////////////////////////////////////////////////////
//adjusts the indexes and recalculates the bounds
//returns the number of objects removed.
//insertIndex is optional and defaults to NULL, if it is not null, the index
//is adjusted based on how many objects are deleted before it
int OBJGROUP::RemoveAllSelObjects(
  BOOL deleteObj,
	int * insertIndex)
	{
	LPSERUID dataPtr = GetIndexesPtr();
	int i;
	int delCount = 0;
  LPOBJINST objPtr;

	//need to go through all objects to make sure the indexes will be reset
  for(i=0;i<objCount; i++)
		{
		objPtr = (LPOBJINST)GetObject(dataPtr[i]);
		if(objPtr->TestSelectFlag())
			{
			if(deleteObj)
				DeleteObjInst(objPtr);

			if(i < objCount-1)
			//shift all records after the deleted object to the left
				MoveMemory((dataPtr + i), (dataPtr + i + 1), sizeof(SERUID) * (objCount - 1 - i));

			if(insertIndex && (i < *insertIndex))
				(*insertIndex)--;

			objCount--;
			selObjCount--;
			i--; //we just lost a record to dec i to look at the record which was shifted down;
			delCount++;
			}
		else if(delCount > 0)
			//adjust the index for the previously deleted objects
    	objPtr->SetIndex(objPtr->GetIndex() - delCount);
		}

	if(objCount > 0)
  	{
		((LPSERIUSBLOCK)GetObject(objIndexes))->ResizeBlock(-((long)sizeof(SERUID) * delCount));
		CalculateBounds();
		}

  if(selObjCount == 0)
		{
		groupInfo->mark = OBJGROUP_NOMARKSET;
		return delCount;
		}
	else
		return -1;
	}


/////////////////////////////////////////////////////////////
// FUNCTION:			OBJGROUP::InsertAllBlockObjects
//
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL OBJGROUP::InsertAllBlockObjects(
	RUNTIMEUID objectBlock)
	{
	LPSERUID dataPtr;
	int i=0;
	LPOBJINST objPtr;
	SERUID groupUID = GetUID(this);

	objCount = (int)((LPRECORDBLOCK)GetObject(objectBlock))->GetCount();

	((LPSERIUSBLOCK)GetObject(objIndexes))->ResizeBlock(sizeof(SERUID) * objCount);
	dataPtr = GetIndexesPtr();	
	
	while(0 != (objPtr = (LPOBJINST)GetObject(objectBlock, i)))
		{
		dataPtr[i-1] = GetUID(objPtr);
		objPtr->SetGroupUID(groupUID);
		objPtr->SetIndex(i-1);
		}
			
	CalculateBounds();
	
	return TRUE; //signal no error	
	} 

																			 

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  DeleteObjInst
//
// CLASS.......:  OBJGROUP
//
// DESCRIPTION.:
//
/////////////////////////////////////////////////////////////////////////////
void OBJGROUP::DeleteObjInst(
  OBJINST * pObjInst)
  {
	LONG			 lTmp			= 0;
  LPSUBJECT  subjectPtr = GetSubjectPtr();

	if((!pObjInst->TestAliasFlag()) || (pObjInst->GetSharedObjectUID() == 0))
		SendMessageToObject(AOBJ_DESTROYED, lTmp, lTmp, lTmp, ::GetUID(pObjInst).id);

	subjectPtr->DecTypeCount(pObjInst->GetTypeID(), !pObjInst->TestAliasFlag());
	delete pObjInst;
	subjectPtr->DecObjectsCount();
  subjectPtr->GetParentProject()->DecObjectsCount();
	}


/////////////////////////////////////////////////////////////////////////////
int OBJGROUP::DeleteObjects(
	BOOL deleteAll)
	{
	int numDeleted = 0;
	LPSERUID dataPtr;
	int i;
	LPOBJINST objPtr;
	int topIndex;

	if(deleteAll || (selObjCount == objCount))
		{
		dataPtr = GetIndexesPtr();
		for(i=0;i<objCount;i++)
			DeleteObjInst((LPOBJINST)GetObject(dataPtr[i]));

		numDeleted = objCount;
    objCount = selObjCount = 0;
		}
	else if(selObjCount > 0)
		{
		topIndex = GetTopSelectedIndex();
		numDeleted = RemoveAllSelObjects(TRUE);
		if(objCount)
    	{
			if(topIndex >= objCount)
				topIndex = objCount-1;

			objPtr = ObjectFromIndex(topIndex);
			objPtr->SetSelectFlag();
			groupInfo->mark = topIndex;
			selObjCount++;
			}
    }
	return numDeleted;
  }

////////////////////////////////////////////////////////////////////////////
void OBJGROUP::ResetObjIndexes()
	{
	int i;

	LPSERUID dataPtr = GetIndexesPtr();
	for(i=0;i<objCount;i++)
		((LPOBJINST)GetObject(dataPtr[i]))->SetIndex(i);

  }
////////////////////////////////////////////////////////////////////////////
//Private routine which inserts the object and returns the index - does not
//recalculate the bounds or select the object or anything like that.
int OBJGROUP::DoInsert(
	SERUID objUID,
	int index)
	{
	LPSERUID dataPtr;
	int i;

	objCount++;
	if(index >= objCount)
		index = objCount - 1; //insert at end of list
	else if(index < 0)
		index = 0; //insert at beginning

	((LPSERIUSBLOCK)GetObject(objIndexes))->ResizeBlock(sizeof(SERUID));
	dataPtr = GetIndexesPtr();
	if(index < objCount - 1)
		{
		//need make room to insert the object
		MoveMemory((dataPtr + index + 1), (dataPtr + index),
			sizeof(SERUID) * (objCount - 1 - index));
		}

	dataPtr[index] = objUID;
	if(index < objCount - 1)
		{
		//we need to update the object Indexes which were moved
		for(i = index+1;i<objCount;i++)
			((LPOBJINST)GetObject(dataPtr[i]))->SetIndex(i);

		}
	return index;
	}

////////////////////////////////////////////////////////////////////////////
//returns the index of where a new object should be inserted.
//The index could be negative or much greater than the actual number
//of objects.
int OBJGROUP::GetNewObjIndex(
	int y)
	{
	int slotHeight = GetSlotHeight();
	return (y - objBounds.top + (slotHeight / 2) ) / slotHeight;
	}

////////////////////////////////////////////////////////////////////////////
void OBJGROUP::CalculateBounds(
	BOOL sizeToHoldObject)
	{
	//the height of each object is the icon size + the object separation.
	//Then add the half separation for the top and bottom.
	objBounds.bottom = objBounds.top + (objCount *
		GetSlotHeight()) + OBJGROUP_BORDER;

	if(sizeToHoldObject)
		SizeGroupToFitObject();

	}


	/////////////////////////////////////////////////////////////
// FUNCTION:			OBJGROUP::SizeGroupToFitObject
//						 
// PARAMETERS:
//
// DESCRIPTION:		Enlarges group if necessary to hold the object name
//
// RETURNS:
/////////////////////////////////////////////////////////////
void OBJGROUP::SizeGroupToFitObject()
	{
	LPSERUID dataPtr = GetIndexesPtr();
	int objWidth = 0;
	int i;
	HDC hdc;
	LPOBJINST objPtr;
	HFONT oldFont;
	int testWidth = 0;
	RECT objRect;
	RECT textRect;

	hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	I_ASSERT(hdc);
	
	I_ASSERT(objCount > 0);

	//get largest object width
	for(i=0;i<objCount;i++)
		{
    SIZE textSize; // SKJ
		objPtr = (LPOBJINST)GetObject(dataPtr[i]);
		I_ASSERT(objPtr);

		oldFont = SelectFont(hdc, GetObjFont(objPtr));
		
		//add in the width of an 'I' so the name is not jammed against the
		//left edge.
		if (GetTextExtentPoint32(hdc, "I", 1, &textSize))
			testWidth = textSize.cx;

		if (GetTextExtentPoint32(hdc, objPtr->GetName(),
			lstrlen(objPtr->GetName()), &textSize))
			{
			testWidth += textSize.cx; // LOWORD(
			}

		if(testWidth > objWidth)
			objWidth = testWidth;

		SelectFont(hdc, oldFont);
		}

	//now find out how big the group is
	GetObjectRect(objPtr->GetIndex(), &objRect);
	GetObjTextRect(&objRect, &textRect);

	if(textRect.right - textRect.left + 1 < objWidth)
		objBounds.right += (objWidth - (textRect.right - textRect.left + 1));
	
	
	DeleteDC(hdc);
	} 

																			 

////////////////////////////////////////////////////////////////////////////
//There are three DrawObject routines for efficiency reasons.
//A routine is provided in case the objects rect needs to be calculated and
//a routine is provide for when the object's rect is already known.
//And a routine is provided if only the object is known
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//drawOffScreen is optional which defaults to TRUE
void OBJGROUP::DrawObject(
	LPOBJINST objPtr,
	BOOL drawOffScreen)
	{
	HDC hdc;
	HWND hwnd;

	hwnd = GetSubjectPtr()->GetWindowPtr()->GetWindowHandle();
	hdc = GetDC(hwnd);
	DrawObject(hdc, objPtr, drawOffScreen);
	ReleaseDC(hwnd, hdc);
	}


////////////////////////////////////////////////////////////////////////////
//drawOffScreen is optional which defaults to TRUE
void OBJGROUP::DrawObject(
	HDC hdc,
	LPOBJINST objPtr,
	BOOL drawOffScreen)
	{
	RECT rt;

	GetObjectRect(objPtr->GetIndex(), (LPRECT)&rt);
	DrawObject(hdc, objPtr, (LPRECT)&rt, drawOffScreen);
  }

////////////////////////////////////////////////////////////////////////////
//drawOffScreen is optional which defaults to TRUE
//IMPORTANT: hdc must not be a memory DC if drawOffScreen is TRUE
void OBJGROUP::DrawObject(
	HDC hdc,
	LPOBJINST objPtr,
	LPRECT bounds,
	BOOL drawOffScreen)
	{
	HBRUSH bkBrush;
	COLORREF textColor;
	HFONT textFont;
	int oldDC;
	int iconSize;
	HDC iconMemDC;
	HBITMAP iconBmp;
	RECT rc;
	POINT nip;
	HDC drawMemDC;
	HBITMAP drawBmp;
	HDC drawDC;
	int bitmapWidth = bounds->right - bounds->left + OBJGROUP_NIPPLEWIDTH;
	int bitmapHeight = bounds->bottom - bounds->top;

	if(drawOffScreen)
		{
    //set up a bitmap for offscreen drawing
		drawMemDC = CreateCompatibleDC(hdc);
		drawBmp = CreateCompatibleBitmap(hdc, bitmapWidth, bitmapHeight);
		SelectBitmap(drawMemDC, drawBmp);

		//fill the bitmap to the window color to make sure the nipple will have
    //a proper background.
		rc.left = rc.top = 0;
		rc.right = bitmapWidth;
		rc.bottom = bitmapHeight;
		FillRect(drawMemDC, (LPRECT)&rc, PROGSYSTEM::windowBrush);

		//here we have to put in a slight kluge - since we just erased
		//the left border with the fillrect call above, we have to redraw
		//the border - we only need to do this if the object is being specifically
		//drawn offscreen.

		SelectObject(drawMemDC, PROGSYSTEM::framePen);
		MoveToEx(drawMemDC, bitmapWidth - OBJGROUP_NIPPLEWIDTH, 0, NULL);
		LineTo(drawMemDC, bitmapWidth - OBJGROUP_NIPPLEWIDTH, bitmapHeight);

    //Set the window origin so things can draw normally
		SetWindowOrgEx(drawMemDC, bounds->left, bounds->top, NULL);

		drawDC = drawMemDC;
		}
	else
  	{
		oldDC = SaveDC(hdc);
		drawDC = hdc;
		}

	if(objPtr->TestSelectFlag())
		{
		bkBrush = PROGSYSTEM::highlightBrush;
		textColor = PROGSYSTEM::highlightTextColor;
		}
	else
		{
		bkBrush = PROGSYSTEM::windowBrush;
		textColor = PROGSYSTEM::normalTextColor;
		}

	iconSize = GetIconSize();
  textFont = GetObjFont(objPtr);

	SelectObject(drawDC, textFont);

	//create a memory hdc and bitmap on which to draw the icon - the icon
	//always appears on a backgroup the color of the window background.
	iconMemDC = CreateCompatibleDC(hdc);
	iconBmp = CreateCompatibleBitmap(hdc, OBJGROUP_LARGEICONSIZE,
		OBJGROUP_LARGEICONSIZE);
	SelectObject(iconMemDC, iconBmp);
	rc.left = rc.top = 0;
	rc.right = rc.bottom = OBJGROUP_LARGEICONSIZE + 1;
	FillRect(iconMemDC, (LPRECT)&rc, PROGSYSTEM::windowBrush);
	objPtr->GetIcon()->Draw(iconMemDC, 0, 0);


	//now set up the rectangle for the text
	GetObjTextRect(bounds, (LPRECT)&rc);

	if(objPtr->TestEditFlag())
		{
		//select the pen for the connecting line below.
		SelectObject(drawDC, PROGSYSTEM::highlightPen);

		FrameRect(drawDC, bounds, PROGSYSTEM::highlightBrush);

		//draw line to box in the icon
		MoveToEx(drawDC, bounds->left + OBJGROUP_HOBJECTBORDER + iconSize + 2, bounds->top + 1, NULL);
		LineTo(drawDC,bounds->left + OBJGROUP_HOBJECTBORDER + iconSize + 2, bounds->bottom - 1);
		}
	else
  	{
		SetBkMode(drawDC, TRANSPARENT);
		SetTextColor(drawDC, textColor);

		//draw the background
		FillRect(drawDC, bounds, bkBrush);

		//print the text - NOTE: the text must be drawn at the top of
		//the text rectangle since that is where the edit field draws the
		//text.
		DrawText(drawDC, objPtr->GetName(), lstrlen(objPtr->GetName()),
			(LPRECT)&rc, DT_SINGLELINE|DT_LEFT|DT_TOP|DT_NOPREFIX);

		}

	//bitblt the icons hdc to the destination hdc
	if(TestLargeIconsFlag())
		BitBlt(drawDC, bounds->left + OBJGROUP_HOBJECTBORDER, bounds->top +
			GetVBorder(), iconSize, iconSize, iconMemDC, 0, 0, SRCCOPY);
	else
		StretchBlt(drawDC, bounds->left + OBJGROUP_HOBJECTBORDER, bounds->top +
			GetVBorder(), iconSize, iconSize, iconMemDC, 0, 0,
				OBJGROUP_LARGEICONSIZE, OBJGROUP_LARGEICONSIZE, SRCCOPY);

	//Draw the nipple
	GetNipplePoint((LPPOINT)&nip, objPtr);
	//FALSE means don't invert
	DrawNipple(drawDC, nip, FALSE, objPtr->TestSelectFlag());

	if(drawOffScreen)
		{
		BitBlt(hdc, bounds->left, bounds->top, bitmapWidth, bitmapHeight,
			drawMemDC, bounds->left, bounds->top, SRCCOPY);
		DeleteDC(drawMemDC);
		DeleteObject(drawBmp);
		}
	else
		RestoreDC(hdc, oldDC);
	
	//cleanup
	DeleteDC(iconMemDC);
	DeleteObject(iconBmp);
	}


////////////////////////////////////////////////////////////////////////////
HFONT OBJGROUP::GetObjFont(
	LPOBJINST objPtr)
	{
	HFONT objFont;
	LPOBJINST sharedObjPtr=NULL;

	//if we're an alias, then we'll need to chack the shared object to see whether or not
  //to show persistence.
	if(objPtr->TestAliasFlag() && (objPtr->GetSharedObjectUID() != NULLSERUID))
		sharedObjPtr = (LPOBJINST)GetObject(objPtr->GetSharedObjectUID());


	if(TestLargeIconsFlag())
		{
		if((sharedObjPtr && sharedObjPtr->TestPersistentsFlag()) ||
			(!sharedObjPtr && objPtr->TestPersistentsFlag()))
			{
			if(objPtr->TestShareFlag())
				objFont = large_persistentSharedFont;
			else if(objPtr->TestAliasFlag())
				objFont = large_persistentAliasFont;
			else
				objFont = large_persistentNormalFont;
			}
		else
			{
			if(objPtr->TestShareFlag())
				objFont = large_sharedFont;
			else if(objPtr->TestAliasFlag())
				objFont = large_aliasFont;
			else
				objFont = large_normalFont;
			}
		}
	else
		{
		if((sharedObjPtr && sharedObjPtr->TestPersistentsFlag()) ||
			(!sharedObjPtr && objPtr->TestPersistentsFlag()))
			{
			if(objPtr->TestShareFlag())
				objFont = small_persistentSharedFont;
			else if(objPtr->TestAliasFlag())
				objFont = small_persistentAliasFont;
			else
				objFont = small_persistentNormalFont;
			}
		else
    	{
			if(objPtr->TestShareFlag())
				objFont = small_sharedFont;
			else if(objPtr->TestAliasFlag())
				objFont = small_aliasFont;
			else
				objFont = small_normalFont;
			}
		}
	return objFont;
  }

////////////////////////////////////////////////////////////////////////////
void OBJGROUP::GetObjTextRect(
	LPRECT const objRect,
	LPRECT textRect)
	{
	int iconSize = GetIconSize();
	int textSpacing;
	int vBorder = GetTextBorder();

	//figure distance between the left edge and the start of the text
	textSpacing = OBJGROUP_HOBJECTBORDER + iconSize + OBJGROUP_TEXTBORDER;

	textRect->left = objRect->left + textSpacing;
	textRect->right = objRect->right - 1;
	//add 1 to the top because it just seems to make things look better.
	textRect->top = objRect->top + vBorder + 1;
	textRect->bottom = objRect->bottom - vBorder;
  }

////////////////////////////////////////////////////////////////////////////
void OBJGROUP::DrawSelectionRect(
	HDC hdc)
	{
	POINT pt = location;
	int oldMode;
	COLORREF oldColor;
	COLORREF oldBkColor;
	HPEN oldPen;
	HBRUSH oldBrush;
	HWND parent;
	HDC parentDC;
	POINT windowOrg;

	oldMode = SetROP2(hdc, R2_NOTXORPEN);
	oldColor = SetTextColor(hdc, PROGSYSTEM::dragColor);
	oldBkColor = SetBkColor(hdc, PROGSYSTEM::windowColor);
	oldPen = SelectPen(hdc, GetStockObject(NULL_PEN));

	//MICORSOFT really sucks fix!!!!!!
	//What microsoft failed to mention is that for a device context
	//associated with a window, the brush origin is set according to
	//screen coordinates.  However, if the DC is a memory DC, then the
	//brush origin is set releative to the upper left corner of the
	//offscreen drawing area.  So what we want to do, is convert the point
	//to client coordinates of the offscreen drawing area.
	parent = GetSubjectPtr()->GetWindowPtr()->GetWindowHandle();
	parentDC = GetDC(parent);

	//if the DC belongs to the parent window.
	if(parentDC ==hdc)
		{
		LPtoDP(parentDC, (LPPOINT)&pt, 1);
		ClientToScreen(parent, (LPPOINT)&pt);
		}
	else
		{
		GetWindowOrgEx(hdc, &windowOrg);

		pt.x-= (short)windowOrg.x;
		pt.y-= (short)windowOrg.y;
		}

	ReleaseDC(parent, parentDC);

	UnrealizeObject(selRectBrush);
	SetBrushOrgEx(hdc, pt.x, pt.y, NULL);
	oldBrush = SelectBrush(hdc, selRectBrush);

	DrawTheSelectionRects(hdc);
  
	//clean up
	SetROP2(hdc, oldMode);
	SetTextColor(hdc, oldColor);
	SetBkColor(hdc, oldBkColor);
	SelectPen(hdc, oldPen);
	SelectBrush(hdc, oldBrush);
	}

////////////////////////////////////////////////////////////////////////////
//offset is optional and defaults to NULL
void OBJGROUP::DrawTheSelectionRects(
	HDC hdc,
	LPPOINT offset)
	{
	RECT selRect;
	RECT newBounds;
	POINT newLocation;

	//NOTE: the rect gotten by GetRect surrounds the objBounds
	GetRect((LPRECT)&selRect);
	//inflate the selRect by 1 since no border will be drawn around the
	//rectangle because we're using a null pen.
//	InflateRect((LPRECT)&selRect, 1, 1);

	if(offset)
  	{
		OffsetRect((LPRECT)&selRect, offset->x, offset->y);
		newBounds.left = objBounds.left + offset->x;
		newBounds.right = objBounds.right + offset->x;
		newBounds.bottom = objBounds.bottom + offset->y;
		newLocation.y = location.y + offset->y;
		}
	else
  	{
		newBounds = objBounds;
		newLocation.y = location.y;
		}

	//left
	Rectangle(hdc, selRect.left, selRect.top, newBounds.left+1,
		selRect.bottom+1);

	//top
	Rectangle(hdc, newBounds.left, selRect.top,
	newBounds.right + OBJGROUP_NIPPLEWIDTH+1, newLocation.y+1);

	//right
	Rectangle(hdc, newBounds.right + OBJGROUP_NIPPLEWIDTH,
	selRect.top, selRect.right+1, selRect.bottom+1);

	//bottom
	Rectangle(hdc, newBounds.left, newBounds.bottom,
	newBounds.right + OBJGROUP_NIPPLEWIDTH+1,	selRect.bottom + 1);
	}


////////////////////////////////////////////////////////////////////////////
//expand the selection rect to account for trying to select the borders or
//nipples.
void OBJGROUP::GetSelectedRect(
	LPRECT bounds)
	{
	bounds->left = objBounds.left - OBJGROUP_BORDERTOLERANCE;
	bounds->right = objBounds.right + OBJGROUP_NIPPLEWIDTH +
		OBJGROUP_NIPPLETOLERANCE;
	bounds->top = location.y - OBJGROUP_BORDERTOLERANCE;
	bounds->bottom =objBounds.bottom + OBJGROUP_BORDERTOLERANCE;
	}

///////////////////////////////////////////////////////////////////////////
WORD OBJGROUP::OnResizeBorder(
	POINT mouse)
	{
	WORD result;
  LPOBJINST tempObj;

	result = GetHitArea(mouse, &tempObj);

	if(!(result & (OBJGROUP_LEFTBORDERHIT | OBJGROUP_RIGHTBORDERHIT)))
		result = 0;


	return result;
  }

/////////////////////////////////////////////////////////////
// FUNCTION:			OBJGROUP::GetHitArea
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
WORD OBJGROUP::GetHitArea(
	POINT mouse,
	LPOBJINST * objPtr)
	{
	RECT selRect;
	WORD hit = OBJGROUP_NOHIT;
	LPOBJINST tempObj;
	HDC hdc;
	int length;
	HFONT oldFont;

	*objPtr = NULL;
	GetSelectedRect((LPRECT)&selRect);
	if(PtInRect((LPRECT)&selRect, mouse))
		{
		if(mouse.y < objBounds.top)
			{
			//we're either in the caption or the border
			if(mouse.x < objBounds.left + OBJGROUP_BORDERTOLERANCE)
				//we're in the left border
				hit = OBJGROUP_LEFTBORDERHIT;
			else if(mouse.x >= objBounds.right + OBJGROUP_NIPPLEWIDTH - OBJGROUP_BORDERTOLERANCE)
				{
				//right border
				hit = OBJGROUP_RIGHTBORDERHIT;
				}
			else if(mouse.y <= location.y)
				hit = OBJGROUP_DRAGBORDERHIT;
			else //in the caption
				hit = OBJGROUP_CAPTIONHIT;

			}
		else if(mouse.y >= objBounds.bottom)
			{
			//we're either on the left, bottom or right border
			if(mouse.x < objBounds.left + OBJGROUP_BORDERTOLERANCE)
				//we're in the left border
				hit = OBJGROUP_LEFTBORDERHIT;
			else if(mouse.x >= objBounds.right + OBJGROUP_NIPPLEWIDTH - OBJGROUP_BORDERTOLERANCE)
				{
				//right border
				hit = OBJGROUP_RIGHTBORDERHIT;
				}
			else
				hit = OBJGROUP_DRAGBORDERHIT;
			}
		else
			{
			//we're somewhere below the caption
			if(mouse.x < objBounds.left + (TRUE == IsSelected())?OBJGROUP_BORDERTOLERANCE:0)
				//we're in the left border
				hit = OBJGROUP_LEFTBORDERHIT;
			else if(mouse.x >= objBounds.right - OBJGROUP_BORDERTOLERANCE)
				{
				//right border or nipples
				//see if we're on a nipple only if we're not selected.
				if(	IsSelected())
					{
					if(mouse.x >= objBounds.right + OBJGROUP_NIPPLEWIDTH - OBJGROUP_BORDERTOLERANCE)
						{
						hit = OBJGROUP_RIGHTBORDERHIT;
						}
					else
						{
						*objPtr = ObjectFromY(mouse.y);
            hit = OBJGROUP_ICONHIT; //so the object can be dragged.
            }
          }
				else
					{
					//see if we're on a nipple
					if(OnNipple(mouse))
						{
						//we're on a  nipple
						tempObj = ObjectFromY(mouse.y);
						*objPtr = tempObj;
							hit = OBJGROUP_NIPPLEHIT;
						}
         }
				}
			else if(mouse.x < objBounds.left + GetIconSize() +
				OBJGROUP_HOBJECTBORDER)
				{
				//we're on an object icon
				tempObj = ObjectFromY(mouse.y);
				if(tempObj)
					{
					*objPtr = tempObj;
					hit = OBJGROUP_ICONHIT;
					}
				}
			else 
				{
				//see if we're inside the text otherwise treat the hit
				//as hitting an icon
				tempObj = ObjectFromY(mouse.y);
				if(tempObj)
					{
					SIZE size;
					*objPtr = tempObj;
					hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
					oldFont = SelectFont(hdc, GetObjFont(tempObj));
					if (GetTextExtentPoint32(hdc,	tempObj->GetName(),
						lstrlen(tempObj->GetName()), &size) )
						{
						length = size.cx;
						}
					SelectFont(hdc, oldFont);
					DeleteDC(hdc);

					if(mouse.x > objBounds.left + GetIconSize() + length +
						OBJGROUP_GROUPNAMEHITTOLERANCE + OBJGROUP_TEXTBORDER)
						hit = OBJGROUP_ICONHIT;
					else
						hit = OBJGROUP_TEXTHIT;
					}
				}
			}
		}

	//we don't care about the borders being hit if we're not selected
	if(!IsSelected() && (hit & OBJGROUP_BORDERHIT))
		hit = OBJGROUP_NOHIT;


	return hit;
	}

////////////////////////////////////////////////////////////////////////////
//object based on y coordinate only
LPOBJINST OBJGROUP::ObjectFromY(
	int y)
	{
	LPSERUID indexPtr;
	int i;

	if(y <= objBounds.top || y >= objBounds.bottom)
		return NULL;

	i = (y - objBounds.top) / GetSlotHeight();

	//because of border between the last object and the bottom border, the
	//math may calculate to i to be objCount which is 1 too many for our zero
	//based index.
	if(i==objCount)
		i = objCount-1;

	indexPtr = GetIndexesPtr();

	return (LPOBJINST)GetObject(indexPtr[i]);
	}

////////////////////////////////////////////////////////////////////////////
//object based on both x and y coordinates
LPOBJINST OBJGROUP::ObjectFromPoint(
	POINT pt)
	{
	if(IntersectPoint(pt))
		{
		return ObjectFromY(pt.y);
		}
	return NULL;
	}


////////////////////////////////////////////////////////////////////////////
void OBJGROUP::DrawObjectBox(
	HDC hdc,
	LPOBJINST objPtr)
	{
	HPEN oldPen;
	HBRUSH oldBrush;
	int oldMode;
	RECT rt;

	oldPen = SelectPen(hdc, PROGSYSTEM::highlightPen);
	oldBrush = SelectBrush(hdc, GetStockObject(NULL_BRUSH));
	oldMode = SetROP2(hdc, R2_NOTXORPEN);

	GetObjectBox(&rt, objPtr);

	Rectangle(hdc, rt.left, rt.top, rt.right, rt.bottom);

	SelectBrush(hdc, oldBrush);
	SelectPen(hdc, oldPen);
	SetROP2(hdc, oldMode);
	}


////////////////////////////////////////////////////////////////////////////
void OBJGROUP::GetObjectBox(
	LPRECT box,
	LPOBJINST objPtr)
	{
	int slotHeight = GetSlotHeight();
	
	box->left = objBounds.left + OBJGROUP_HOBJECTBORDER;
	box->right = objBounds.right + OBJGROUP_BOXBORDER;

	box->top = objBounds.top + (objPtr->GetIndex() * slotHeight);

	box->bottom = box->top + slotHeight;
	}


////////////////////////////////////////////////////////////////////////////
int OBJGROUP::GetSlotHeight()
	{
	if(TestLargeIconsFlag())
		return largeSlot;
	else
		return smallSlot;
	}

////////////////////////////////////////////////////////////////////////////
BOOL OBJGROUP::OnNipple(
	POINT pt)
	{
	RECT rt;
	LPOBJINST objPtr;
  int height;

	//determine rectangle of nipple tolerance est the point
	objPtr = ObjectFromY(pt.y);
	if(!objPtr)
		return FALSE;

	GetNipplePoint((LPPOINT)&rt.left, objPtr);
	height = GetSlotHeight();

	rt.top-=(OBJGROUP_NIPPLEHEIGHT/2 + height/2); //OBJGROUP_NIPPLETOLERANCE);
	rt.bottom = rt.top + OBJGROUP_NIPPLEHEIGHT + height;//2*OBJGROUP_NIPPLETOLERANCE;
 	rt.left -= OBJGROUP_NIPPLETOLERANCE;
 	rt.right = rt.left + OBJGROUP_NIPPLEWIDTH + 2 * OBJGROUP_NIPPLETOLERANCE;

	return PtInRect((LPRECT)&rt, pt);
	}


////////////////////////////////////////////////////////////////////////////
void OBJGROUP::GetNipplePoint(
	LPPOINT nip,
	LPOBJINST objPtr)
	{
	int objHeight = GetSlotHeight();

	nip->y =  objBounds.top + (objPtr->GetIndex() * objHeight) + (objHeight/2);

	nip->x = objBounds.right;
	}

////////////////////////////////////////////////////////////////////////////
void OBJGROUP::GetNippleTip(
	LPPOINT nip,
	LPOBJINST objPtr)
	{
	GetNipplePoint(nip, objPtr);
	nip->x+=OBJGROUP_NIPPLEWIDTH+1; //extend 1 past the tip so the flows are drawn
																	//properly.
	}

////////////////////////////////////////////////////////////////////////////
void OBJGROUP::DrawNipple(
	HDC hdc,
	POINT nip,
	BOOL invert,
	BOOL selecteded)
	{
	HDC hdcMem = CreateCompatibleDC(hdc);

	SelectObject(hdcMem, nippleMask);
	BitBlt(hdc, nip.x, nip.y - OBJGROUP_NIPPLEHEIGHT/2,
		OBJGROUP_NIPPLEWIDTH, OBJGROUP_NIPPLEHEIGHT, hdcMem, 0, 0, SRCAND);

	if(invert)
		SelectObject(hdcMem, nippleInverted);
	else if(selecteded)
		SelectObject(hdcMem, nippleSelected);
	else
		SelectObject(hdcMem, nippleNormal);

	BitBlt(hdc, nip.x, nip.y - OBJGROUP_NIPPLEHEIGHT/2,
		OBJGROUP_NIPPLEWIDTH, OBJGROUP_NIPPLEHEIGHT, hdcMem, 0, 0, SRCINVERT);

	DeleteDC(hdcMem);
	}

////////////////////////////////////////////////////////////////////////////
void OBJGROUP::HighlightConnection(
	HDC hdc,
	WORD operations,
	LPOBJINST objPtr)
	{
	POINT nip;

	//if we're erasing, we want to draw the box before the nipple
	if(operations & OBJGROUP_ERASEBOX)
		DrawObjectBox(hdc, objPtr);

	if(operations & (OBJGROUP_NORMALNIPPLE | OBJGROUP_INVERTNIPPLE))
		{
		GetNipplePoint((LPPOINT)&nip, objPtr);
		DrawNipple(hdc, nip, (operations & OBJGROUP_INVERTNIPPLE),
			objPtr->TestSelectFlag());
		}

	if(operations & OBJGROUP_DRAWBOX)
		DrawObjectBox(hdc, objPtr);
	}

////////////////////////////////////////////////////////////////////////////
//made friend by project.h so we can access PROJECT::MakeClean
BOOL OBJGROUP::EditObject(
	LPOBJINST objPtr)
	{
	char       buffer[256];
	LPOBJINST	 pObjInst = objPtr;
	AMEMBLOCKID theText;
//  LPSTR      lpText;
	SERUID     realObjID;
	LPSUBJECT  subjectPtr = GetSubjectPtr();
	LPPROJECT  projectPtr = subjectPtr->GetParentProject();
	BOOL result = FALSE; //assume no changes made
	long editResult;
	char appNameBuffer[32];
  LPSTR valueString;
	BOOL dirtyState;

	if((subjectPtr->fileObjPtr->GetFileIDStamp() == 
		PROGSYSTEM::GetRunTimeFileID()) && PROGSYSTEM::TestRunTimeFlag())
		{
		if(pObjInst->TestAliasFlag() && (pObjInst->GetSharedObjectUID() != NULLSERUID))
			{
			//set pObjInst to be the real object
			//First link the alias just in case the user changed it's name
			//during the debug.  Pass TRUE errors are shown if the object can't
			//be linked.
			pObjInst = (LPOBJINST)GetObject(pObjInst->GetSharedObjectUID());
			}

		if(pObjInst)
    	{
			theText = AMemAllocate(0);
			if(AObjConvertToText(pObjInst->copyUID.id, theText))
				{
				valueString = (LPSTR)AMemLock(theText);
				if(valueString)
					ObjectValueDialog(valueString);
				else
					{
					lstrcpy(buffer,
					 "An error occurred while fetching the text value for the "
					 "highlighted object.");
					MessageBox(NULL, buffer, "Examine Object", MB_OK|MB_ICONINFORMATION|
						MB_TASKMODAL);
					}
				}
			else
				{
				lstrcpy(buffer, "That object can't be examined at run time.");
				MessageBox(NULL, buffer, "Examine Object", MB_OK|MB_ICONHAND|
					MB_TASKMODAL);
				}
			AMemFree(theText);
			}
		}
	else
		{
		subjectPtr->LinkAllAliasObjects(FALSE);
		if(pObjInst->TestAliasFlag())
			{

			realObjID = pObjInst->GetSharedObjectUID();
			if(realObjID.id != 0)
				((LPOBJINST)GetObject(realObjID))->GetSubjectPtr()->
				 ShowObject(realObjID);
			else
				{
				wsprintf(buffer,
				 "The alias object \"%s\" is not linked to a shared object."
				 "  An alias object can only link to a shared object with the"
				 " same type and name as the alias object.",
				 (LPSTR)pObjInst->GetName());
				MessageBox(NULL, buffer, "Edit Object", MB_OK|MB_TASKMODAL);
				}
			}
		else
			{
			EnableWindow(MDI::GetNavigatorPtr()->GetNavigatorWindow(), FALSE);
			EnableWindow(MDI::GetPalettePtr()->GetPaletteHandle(), FALSE);

			dirtyState = projectPtr->ProjectIsDirty();
			projectPtr->MakeClean();
			
			editResult = AObjEdit(GetUID(pObjInst).id);
			
			if(editResult == A_NOTHANDLED)
				{
				//put up message box saying the object can't be edited.
				lstrcpy(buffer, pObjInst->objDesc->pGetName());
				lstrcat(buffer, " objects cannot be edited.");
				MessageBeep(0);
				LoadString(PROGSYSTEM::hInst, STR_FRAMENAME, (LPSTR)appNameBuffer, 32);
				MessageBox(NULL, buffer, appNameBuffer, MB_OK|MB_TASKMODAL|MB_ICONINFORMATION);
				}

			//if the object wasn't edited but the project was dirty, make sure to
			//set the dirty flag.
			if(!projectPtr->ProjectIsDirty())
				{
				if(dirtyState)
					projectPtr->MakeDirty();
				}
			else
				{
				//object was edited
				result = TRUE;
				PROGSYSTEM::ClearUndoFlag(); 
				}
				
			}
		}
	EnableWindow(MDI::GetNavigatorPtr()->GetNavigatorWindow(), TRUE);
	EnableWindow(MDI::GetPalettePtr()->GetPaletteHandle(), TRUE);
	return result;
	}

////////////////////////////////////////////////////////////////////////////
void OBJGROUP::UnSelect(
	HDC hdc)
	{
	ClearActiveFlag();
	if(selObjCount)
  	{
		//don't redraw the objects since they'll be drawn in the FUNCWINOBJ::UnSelect routine
		UnselectObjects(hdc, FALSE, NULLSERUID);
		FUNCWINOBJ::UnSelect(hdc);
		}
	else
		{
		//do quicker unselection since no objects are selected.
		EraseSelection(hdc);
		DrawCaption(hdc);
    }
	}

////////////////////////////////////////////////////////////////////////////
//Set's the subject's active group flag as well
void OBJGROUP::Select(
	HDC hdc)
	{
	SetActiveFlag();
	FUNCWINOBJ::Select(hdc);
	}

//////////////////////////////////////////////////////////////////////////
void OBJGROUP::UnSelectAndLeaveActive(
	HDC hdc)
	{
	RectUnSelect(hdc);
	}

////////////////////////////////////////////////////////////////////////////
//With these routines, we want to set and clear the selection flag while
//leaving the active flag alone.
void OBJGROUP::RectUnSelect(
	HDC hdc)
	{
	EraseSelection(hdc);
	}

////////////////////////////////////////////////////////////////////////////
void OBJGROUP::RectSelect(
	HDC hdc)
	{
	FUNCWINOBJ::Select(hdc);
	}


////////////////////////////////////////////////////////////////////////////
//See if the drop POINT lies within the group
BOOL OBJGROUP::DropScoredHit(
	LPRECT cursorRect)
	{
	return ObjectIntersectsRect(cursorRect);
	}

////////////////////////////////////////////////////////////////////////////
BOOL OBJGROUP::ObjectIntersectsRect(
	LPRECT bigRect)
	{
	RECT bounds;
	RECT temp;

	GetSelectedRect((LPRECT)&bounds);

	return(IntersectRect((LPRECT)&temp, (LPRECT)&bounds, bigRect));
	}

////////////////////////////////////////////////////////////////////////////
//idea is to convert the cursor into the new location of the object
//The hdc passed in should be the hdc of the subjectwindow.
void OBJGROUP::AnimateObjectAdd(
	HDC hdc,
	LPOBJINST objPtr,
	LPRECT cursorRect)
	{
	RECT objRect;
	HDC newDC;
	HWND hwnd;
	RECT rc = *cursorRect;

	GetObjectRect(objPtr->GetIndex(), (LPRECT)&objRect);
	//If the object was double clicked, then the cursor was over the palette which means
	//that if we draw from the cursor, we will be drawing off the subjectwindow's client
	//area - so, get a screen DC and convert all coordinates to that DC just to be safe.
  hwnd = GetSubjectPtr()->GetWindowPtr()->GetWindowHandle();
	newDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	LPtoDP(hdc, (LPPOINT)&objRect, 2);
	LPtoDP(hdc, (LPPOINT)&rc, 2);
	ClientToScreen(hwnd, (LPPOINT)&objRect.left);
	ClientToScreen(hwnd, (LPPOINT)&objRect.right);
	ClientToScreen(hwnd, (LPPOINT)&rc.left);
	ClientToScreen(hwnd, (LPPOINT)&rc.right);

	 
	//now convert iconRect to the object rect through animation steps
	AnimateRectToRect(newDC, (LPRECT)&rc, (LPRECT)&objRect,
		PROGSYSTEM::highlightPen, OBJGROUP_ADDANIMATESTEPS,
		OBJGROUP_ADDANIMATEDELAY);

	DeleteDC(newDC);
	}

////////////////////////////////////////////////////////////////////////////
void OBJGROUP::GetObjectRect(
	int index,
	LPRECT rt)
	{
	int slotHeight = GetSlotHeight();

	rt->left = objBounds.left + 1;
	rt->right = objBounds.right;
	rt->top = objBounds.top + (index * slotHeight);
	rt->bottom = rt->top + slotHeight;
  }


////////////////////////////////////////////////////////////////////////////
//returns TRUE if the object identified by staySelectedUID is
//already selected - note: this object will not be unselected.
//If redraw is FALSE then hdc can be NULL
BOOL OBJGROUP::UnselectObjects(
	HDC hdc,
	BOOL redraw,
	SERUID staySelectedUID)
	{
	LPSERUID dataPtr;
	LPOBJINST objPtr;
	int i;
	BOOL result = FALSE; 	//assume that the object identified by staySelected
												//is not already selected.

	dataPtr = GetIndexesPtr();
	for(i=0;i<objCount && selObjCount > 0;i++)
		{
		objPtr = (LPOBJINST)GetObject(dataPtr[i]);
		if(objPtr->TestSelectFlag())
			{
			if(dataPtr[i] == staySelectedUID)
				result = TRUE;
			else
      	{
				objPtr->ClearSelectFlag();
        selObjCount--;
				if(redraw)
					DrawObject(hdc, objPtr);
				}
			}
		}
	groupInfo->mark = OBJGROUP_NOMARKSET;
	return result;
	}


void OBJGROUP::HandleSelectionForArrowKeys(
	HDC hdc,
	LPOBJINST objPtr,
	BOOL unselect)
	{
	BOOL drawAll = FALSE;
	BOOL drawObj = TRUE;

	if(unselect && selObjCount)
		{
		//we only want to draw everything if more than 1 object is selected or if
		//the only object selected is not the one to remain selected.
		if(selObjCount > 2)
    	{
			UnselectObjects(NULL, FALSE, GetUID(objPtr));
			drawAll = TRUE;
			}
		else if(UnselectObjects(hdc, (hdc != NULL)?TRUE:FALSE, GetUID(objPtr)))
			drawObj = FALSE;
		}

	if(!objPtr->TestSelectFlag())
		{
		objPtr->SetSelectFlag();
		selObjCount++;
		}
	else
		drawObj = FALSE;

	if(TestEditFieldFlag())
		{
		KillEditField(hdc != NULL, TRUE, FALSE); //don't redraw, change name, keep selection
		
		if(hdc)
			drawAll = TRUE;
		}

	if(hdc)
		{
		if(drawAll)
			DrawAllObjects(hdc, NULL, TRUE);
    else if(drawObj)
			DrawObject(hdc, objPtr);
		}

	groupInfo->mark = objPtr->GetIndex();

	}

////////////////////////////////////////////////////////////////////////////
//no objects are drawn if the hdc is null
//Also kills the edit field if necessary.
void OBJGROUP::SelectObj(
	HDC hdc,
	LPOBJINST objPtr,
	WPARAM wParam)
	{
	SERUID tempUID;
	BOOL drawObj = TRUE; //assumes the object will need to be drawn
	BOOL selectObj=TRUE; //assumes that objPtr not already selected.
	BOOL editFieldDied;
	int min;
	int max;

	if(TestEditFieldFlag())
		{
		KillEditField(FALSE, TRUE, FALSE); //don't redraw, change name, keep selection
		editFieldDied = TRUE;
		}
	else
		editFieldDied = FALSE;

	if(wParam & MK_SHIFT)
		{
		//we're selecting objects in a range
		if(groupInfo->mark != OBJGROUP_NOMARKSET)
			{
			min = MIN(groupInfo->mark, objPtr->GetIndex());
			max = MAX(groupInfo->mark, objPtr->GetIndex());

			I_ASSERT(min <= max);

			if(min <= max)
				SelectObjRange(hdc, min, max, OBJGROUP_TOTALREFRESH);

			drawObj = FALSE;
			selectObj = FALSE;
			}
    }
	else if(wParam & MK_CONTROL)
		{
		//unselect if the object is already selected else select.

		//if we need to kill the edif field, only redraw if the edit field
    //lies on an object other the objPtr.
		if(TestEditFieldFlag())
			KillEditField(!objPtr->TestEditFlag(), TRUE, FALSE);

		if(objPtr->TestSelectFlag())
			{
			objPtr->ClearSelectFlag();
			selObjCount--;
      selectObj = FALSE;
			}
    }
	 else if(objPtr->TestEditFlag() || !objPtr->TestSelectFlag())
		{
		//if the objects edit flag is set or if the object isn't selected
		//we need to unselect everything and redraw.

		//if True is passed in, all the objects will be redrawn
		//If the object is being edited, we want to make sure it is
		//redrawn, otherwise pass in the objects uid so that it won't
    //be redrawn when the objects are unselected.

    //if the edit field flag is set, then kill it.
		if(TestEditFieldFlag())
    	KillEditField(FALSE, TRUE, FALSE);

		//If the groups edit flag is not set but the objects edit flag is set, that
		//means don't kill the edit field but we're setting up for the creation of
    //the edit field.

 		if(selObjCount)
			if(UnselectObjects(hdc, (hdc != NULL)?TRUE:FALSE, GetUID(objPtr)))
      	{
				selectObj = FALSE;
				drawObj = FALSE;
        }
				//note: the below expression should replace tempUID as the
        //third parameter but the Borland compiler croaks.
				//(((objPtr->TestEditFlag()) ? NULLSERUID : GetUID(objPtr))));

		if(objPtr->TestEditFlag() && objPtr->TestSelectFlag())
			{
			//if the edit flag is set, the object could already be selected so make sure
			//we don't unneccesarily increment the selObjCount at the end of this routine.
			selectObj = FALSE;
			drawObj = TRUE;
			}

		}
	 else if(objPtr->TestSelectFlag())
		{
		selectObj = FALSE;
    if(!editFieldDied)
			drawObj = FALSE;
    }

	if(selectObj)
		{
		objPtr->SetSelectFlag();
		selObjCount++;
		}

	if(hdc)
  	{
		if(drawObj)
			DrawObject(hdc, objPtr);
		}

	if(selectObj || !(wParam & MK_SHIFT))
		groupInfo->mark = objPtr->GetIndex();
	}

////////////////////////////////////////////////////////////////////////////
//refresh is optional which defaults to OBJGROUP_TOTALREFRESH.
void OBJGROUP::SelectObjRange(
	HDC hdc,
	int start,
	int end,
	WORD refresh)
	{
	LPSERUID dataPtr;
	int i;
	LPOBJINST objPtr;
	//now go through and select the objects in range and unselect all objects
	//out of range without redrawing.
	dataPtr = GetIndexesPtr();
	for(i=0;i<objCount;i++)
		{
		objPtr = ((LPOBJINST)GetObject(dataPtr[i]));
		if(i >= start && i<= end)
			{
			if(!objPtr->TestSelectFlag())
      	{
				objPtr->SetSelectFlag();
				selObjCount++;
				if(refresh & OBJGROUP_PARTIALREFRESH)
        	DrawObject(hdc, objPtr);
				}
			}
		else
			{
			if(objPtr->TestSelectFlag())
      	{
				objPtr->ClearSelectFlag();
				selObjCount--;
				if(refresh & OBJGROUP_PARTIALREFRESH)
        	DrawObject(hdc, objPtr);
				}
      }
    }

	if(refresh & OBJGROUP_TOTALREFRESH)
  	{
		//TRUE enables offscreen drawing
		DrawAllObjects(hdc, NULL, TRUE);
		}
	}

////////////////////////////////////////////////////////////////////////////
//If the object is active, clears the active flag and draws the
//caption.
void OBJGROUP::ClearActiveState(
	HDC hdc)
	{
	if(TestActiveFlag())
		{
		ClearActiveFlag();
		UnselectObjects(hdc, FALSE, NULLSERUID); //FALSE signalsd don't redraw
		Draw(hdc);  //draw the entire group because we want it on top of everything so the
    						//objects can draw themselves.
		}
	}


////////////////////////////////////////////////////////////////////////////
void OBJGROUP::SetActiveState(
	HDC hdc)
	{
	if(!TestActiveFlag())
		{
		SetActiveFlag();
		Draw(hdc);
		}
	}

////////////////////////////////////////////////////////////////////////////
int OBJGROUP::GetTopSelectedIndex()
	{
	LPSERUID dataPtr;
  LPOBJINST objPtr;
	int topIndex = OBJGROUP_NONESELECTED; //default none selected

	int i;

	if(selObjCount > 0)
		{
		dataPtr = GetIndexesPtr();
		for(i=0;i<objCount;i++)
			{
			objPtr = (LPOBJINST)GetObject(dataPtr[i]);
			if(objPtr->TestSelectFlag())
				{
				topIndex = i;
				break;
				}
      }
		}
  return topIndex;
	}

////////////////////////////////////////////////////////////////////////////////
//If index isn't a special value, then index is not changed.
void OBJGROUP::VerifyInsertIndex(
	int * index)
	{
	if(*index == OBJGROUP_BOTTOMINSERTION)
		*index = objCount;
	else if(*index == OBJGROUP_DEFAULTINSERTION)
		{
		*index = GetTopSelectedIndex();
		if(*index == OBJGROUP_NONESELECTED)
			*index = objCount;

		}
	else if(*index == OBJGROUP_AFTERSELECTEDINSERTION)
		{
		*index = GetTopSelectedIndex();
    if(*index == OBJGROUP_NONESELECTED)
			*index = objCount;
		else
			(*index)++;
		}
  }

////////////////////////////////////////////////////////////////////////////
LPOBJINST OBJGROUP::ObjectFromIndex(
	int index)
	{
	LPSERUID dataPtr;

	if(index < 0 || index >= objCount)
		return NULL;
	else
		{
		dataPtr = GetIndexesPtr();
		return (LPOBJINST)GetObject(dataPtr[index]);
    }
  }

////////////////////////////////////////////////////////////////////////////
void OBJGROUP::HandleKeyDown(
	WPARAM wParam,
	LPARAM lParam)
	{
	int topSelected;
	LPOBJINST objPtr;
	HDC hdc;
	HWND hwnd;
	RECT logRect;
  LPSUBJECT subjectPtr;
	RECT objRect;
	POINT scrollAmount;
	BOOL unselect;

	switch(wParam)
		{
		case VK_DOWN:
		case VK_UP:
    	subjectPtr = GetSubjectPtr();
			//determine where to move the highlight marking the selected object.
			topSelected = groupInfo->mark;
			if(topSelected == OBJGROUP_NOMARKSET)
      	{
				topSelected = GetTopSelectedIndex();
				if(topSelected == OBJGROUP_NONESELECTED)
					topSelected = 0;
				}

			else if(wParam == VK_UP)
				topSelected--;
      else
				topSelected++;

			if(topSelected >= objCount)
				topSelected = 0;
			else if(topSelected < 0)
      	topSelected = objCount-1;

			objPtr = ObjectFromIndex(topSelected);
			hwnd = subjectPtr->GetWindowPtr()->GetWindowHandle();

      //see if we need to scroll the object into view
			subjectPtr->GetSubjectLogicalBounds((LPRECT)&logRect);
			GetObjectRect(topSelected, (LPRECT)&objRect);

			scrollAmount.x = 0;

			if(objRect.top < logRect.top)
				scrollAmount.y = objRect.top - logRect.top;
			else if(objRect.bottom > logRect.bottom)
				scrollAmount.y = objRect.bottom - logRect.bottom;
			else
				scrollAmount.y = 0;

      //take care of object selection before scrolling
      hdc = GetDC(hwnd);
			if((0x8000 & GetKeyState(VK_CONTROL)) || (0x8000 & GetKeyState(VK_SHIFT)))
				unselect = FALSE;
			else
				unselect = TRUE;

			HandleSelectionForArrowKeys(hdc, objPtr, unselect);

			ReleaseDC(hwnd, hdc);

			if(scrollAmount.y)
				{
				//we need to scroll
				((LPSUBJECTWINDOW)subjectPtr->GetWindowPtr())->ScrollWorkbench(scrollAmount);
        }


			break;

		case VK_RETURN:
			if(OBJGROUP_NONESELECTED != (topSelected = GetTopSelectedIndex()))
				{
				if(NULL != (objPtr = ObjectFromIndex(topSelected)))
					EditObject(objPtr);
				}
			break;

		default:
			if(OBJGROUP_NONESELECTED != (topSelected = GetTopSelectedIndex()))
      	{
				if(NULL != (objPtr = ObjectFromIndex(topSelected)))
					EditObjTitle(objPtr, WM_KEYDOWN, wParam, lParam, FALSE, FALSE);
					//make sure FALSE was passed in since EditObjTitle expects this for
					//VK_LEFT and VK_RIGHT.
					//The last FALSE means don't redraw the entire group.
				}
			break;
		}
	}


////////////////////////////////////////////////////////////////////////////
//We can't do this from the HandleKeyDown because in the WM_KEYDOWN message,
//the virtual key code received always reflects an uppercase character.  In the
//WM_CHAR message however, a distinction is made between upper and lower case.
//Very important, HandleChar and HandleKeyDown should not both be called for the
//same instance of a key being pressed.
void OBJGROUP::HandleCharMessage(
	WPARAM wParam,
	LPARAM lParam)
	{
	int topSelected;
	LPOBJINST objPtr;

	if(OBJGROUP_NONESELECTED != (topSelected = GetTopSelectedIndex()))
		{
		if(NULL != (objPtr = ObjectFromIndex(topSelected)))
			EditObjTitle(objPtr, WM_CHAR, wParam, lParam, TRUE, FALSE);
			//make sure TRUE is passes in. This will result in the
			//text being selected before the char message is sent
			//and that will replace the selected text with the character.
      //FALSE means don't redraw the entire group.
		}
  }

////////////////////////////////////////////////////////////////////////////
//if message is not 0, then the message is sent to the editField.
//editType is optional and defaults to OBJGROUP_NORMALEDIT
BOOL OBJGROUP::EditObjTitle(
	LPOBJINST objPtr,
   UINT message,
	WPARAM wParam,
	LPARAM lParam,
	BOOL selectText,
	BOOL drawAll)
	{
	HDC hdc;
	HWND hwnd;
	HFONT editFont;
	POINTS mouse;  // SKJ
	RECT rc;
	WORD preMessage = 0;

	//let the object know its being edited.
  objPtr->SetEditFlag();

	//first make sure the object is the only object selected
	hwnd = GetSubjectPtr()->GetWindowPtr()->GetWindowHandle();
	hdc = GetDC(hwnd);

  //Don't select the group's edit flag until after the object is selected.
	if(drawAll)
		{
		SelectObj(NULL, objPtr, 0);
		Draw(hdc);
		}
  else
		SelectObj(hdc, objPtr, 0);

	//don't release the DC just yet.

	//create the editField;
	editField = CreateEditField(objPtr);

	if(!editField)
		{
    objPtr->ClearEditFlag();
		return FALSE;
		}

	PROGSYSTEM::SetActiveEditField(editField);

	if(message == WM_LBUTTONDOWN || message == WM_LBUTTONUP)
		{
		//adjust the mouse coordinates to be client coordinates of the
		//new edit field;
		GetClientRect(editField, (LPRECT)&rc);

		//now convert the edit field's upper left corner to screen coordinates.
		ClientToScreen(editField, (LPPOINT)&rc.left);

    //the mouse is in screen coordinates
		mouse = MAKEPOINTS(lParam);

		mouse.x -= (SHORT)rc.left;  // SKJ-> no explicit casting on purpose
		mouse.y -= (SHORT)rc.top;   // SKJ-> no explicit casting on purpose

		lParam = MAKELPARAM(mouse.x, mouse.y); // two Short values

		message = WM_LBUTTONUP;
		preMessage = WM_LBUTTONDOWN;

		}
	else if(message == WM_KEYDOWN)
		{
		if(wParam ==VK_LEFT)
			wParam = VK_HOME;
		else if(wParam == VK_RIGHT)
				wParam = VK_END;
		}

	//now release the DC.
	ReleaseDC(hwnd, hdc);

	editObject = GetUID(objPtr);

	//set the editfields text to the object name and limit the
	//size of the text.
	Edit_SetText(editField, (LPSTR) objPtr->GetName());
	Edit_LimitText(editField, OBJGROUP_MAXOBJNAME);

	//Set the font
	editFont = GetObjFont(objPtr);
	SetWindowFont(editField, editFont, FALSE);

	//select the text here so in the case of a WM_CHAR message, the
  //selected text will be replaced by the new character.
  if(selectText)
		Edit_SetSel(editField, 0, -1);
	else
		Edit_SetSel(editField, -1, -1);


	if(preMessage)
		SendMessage(editField, preMessage, wParam, lParam);

	if(message)
		SendMessage(editField, message, wParam, lParam);

	//show the edit field
	ShowWindow(editField, SW_SHOW);
	SetFocus(editField);

	//allow for undoing
	SendMessage(editField, EM_CANUNDO, 0, 0L);

	SetEditFieldFlag();

	return TRUE; //signal that the edit field was created
	}


////////////////////////////////////////////////////////////////////////////
HWND OBJGROUP::CreateEditField(
	LPOBJINST objPtr)
	{
	RECT rc;
	HWND hwnd;
	HWND parentHwnd;
  HDC hdc;

	if(editField)
		return NULL;

	//first obtain the objects rectangle
	GetObjectRect(objPtr->GetIndex(), (LPRECT)&rc);

  //now put the text rect into rc
	GetObjTextRect((LPRECT)&rc, (LPRECT)&rc);

	//convert the rect to device coordinates
	parentHwnd = GetSubjectPtr()->GetWindowPtr()->GetWindowHandle();
	hdc = GetDC(parentHwnd);
	LPtoDP(hdc, (LPPOINT)&rc, 2);
  ReleaseDC(parentHwnd, hdc);

	hwnd = CreateWindow((LPSTR)"EDIT", NULL,
		WS_CHILD|ES_LEFT|ES_AUTOHSCROLL,
		rc.left,
		rc.top, 
		rc.right - rc.left,
		rc.bottom - rc.top,
		parentHwnd,
		(HMENU) OBJGROUP_EDITCONTROL,
		PROGSYSTEM::hInst,
		NULL);

	//now subclass the edit control
	oldEditProc = (WNDPROC)SetWindowLong(hwnd, GWL_WNDPROC,(long)OBJGROUP::EditProc);

	return hwnd;
	}

////////////////////////////////////////////////////////////////////////////
//If there is an editfield and we own that edit field, then return
//the object being edited, else return NULL.
LPOBJINST OBJGROUP::GetEditObject()
	{
	if(editObject != NULLSERUID && TestEditFieldFlag())
		return (LPOBJINST)GetObject(editObject);

  return NULL;
	}

////////////////////////////////////////////////////////////////////////////
//Kills the edit control and Sets the project dirty flag if the object's
//name was changed.
// If setOBjName is
//false, the field is destroyed and the object's name is left intact.
//if killObjSelection is FALSE, the
//editable object retains it's selection.
//Returns TRUE if the MessageBox was brought up asking if alias names should
//be changed as well.
BOOL OBJGROUP::KillEditField(
	BOOL redrawObj,
	BOOL setObjName,
	BOOL killObjSelection)
	{
	char buffer[OBJGROUP_MAXOBJNAME + 1];
	LPOBJINST editObjectPtr;
  BOOL messageBoxUp = FALSE;

	//first make sure there is an edit control and the group is the owner.
	if(TestEditFieldFlag() && editField)
		{
		//get the text of the edit field.
		GetWindowText(editField, (LPSTR)buffer, OBJGROUP_MAXOBJNAME);
		editObjectPtr = GetEditObject();
		if(editObjectPtr)
			{
			//reset the edit object and clear flags
			editObject = NULLSERUID;
			editObjectPtr->ClearEditFlag();
			
			if(killObjSelection)
				{
				editObjectPtr->ClearSelectFlag();
				selObjCount--;
				}

			ClearEditFieldFlag();

			//see if the name was changed.
			if(setObjName && lstrcmp((LPSTR)buffer, editObjectPtr->GetName()))
				{
				messageBoxUp = DoObjectNameChange(editObjectPtr, buffer);
				}

			//reset the editFields original edit proc while placing the
			//subclassed editproc into oldEditProc
			SetWindowLong(editField, GWL_WNDPROC, (long)oldEditProc);

			DestroyWindow(editField);
			editField = NULL;

			PROGSYSTEM::SetActiveEditField(NULL);

			//validate the area uncovered by the edit field because we'll handle
      //the drawing ourselves.
			ValidateObjRect(editObjectPtr);

      if(redrawObj)
				DrawObject(editObjectPtr);
			}
		}

	return messageBoxUp;
  }


//returns TRUE if a message box prompting for the alias names to be changed was
//brought up.
BOOL OBJGROUP::DoObjectNameChange(
	LPOBJINST objPtr,
	LPCSTR newName)
	{
	char messageBuffer[200];
	LPSUBJECT subjectPtr = GetSubjectPtr();
	LPPROJECT projectPtr = subjectPtr->GetParentProject();
  BOOL messageBoxUp = FALSE;
	int result = IDYES; //default to something other than IDCANCEL

	if(objPtr->TestShareFlag() &&  projectPtr->IsAliased(objPtr))
		{
		//see if we want to update the names of all aliased objects linked to the
		//shared object.
		wsprintf(messageBuffer,
		"Should the names of aliases connected to '%s' be"
		" changed to '%s' as well?",
		(LPSTR)objPtr->GetName(), newName);

		messageBoxUp = TRUE;

		result = MessageBox(NULL, messageBuffer,
			"Change Connected Aliases?",
			MB_TASKMODAL|MB_YESNOCANCEL|MB_ICONQUESTION);

		if(result == IDYES)
			{
			//update names of all linked aliases and parameters which hold onto those aliases.
			apb_ShowWaitCursor();
			projectPtr->RenameAliases(objPtr, newName);
			apb_RestoreOldCursor();
			}
		else if(result == IDNO)
			{
			//make sure all dummy aliases have datas.
			projectPtr->LinkAllAliases(FALSE);
			}
		}

	if(result != IDCANCEL)
	 	{	
		objPtr->UpdateName((LPSTR)newName);
		projectPtr->MakeDirty();

		if(objPtr->TestAliasFlag())
			SUBJECT::LinkAliasObject(objPtr, FALSE);

		//there will always be a valid windowPtr when this routine is called
		//FALSE means we're not a function.
		((LPSUBJECTWINDOW)subjectPtr->GetWindowPtr())->UpdateParams(
			GetUID(objPtr), FALSE, NULL);
		}

	return messageBoxUp;
	}

///////////////////////////////////////////////////////////////////////////
BOOL OBJGROUP::RenameObject(
	LPOBJINST objPtr,
	LPCSTR newName)
	{
	DoObjectNameChange(objPtr, newName);

	return TRUE; //signal no error
	}


////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK OBJGROUP::EditProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	LPOBJGROUP groupPtr;
	HWND possibleParent;

	switch(message)
		{
		case WM_KEYDOWN:
			switch(wParam)
				{
				case VK_UP:
				case VK_DOWN:
        	groupPtr = ((LPOBJINST)GetObject(editObject))->GetGroupPtr();
					groupPtr->HandleKeyDown(wParam, lParam);
					return 0;

				case VK_RETURN:
					((LPOBJINST)GetObject(editObject))->GetGroupPtr()->KillEditField(TRUE, TRUE, FALSE);
					return 0;

        case VK_ESCAPE:
					SendMessage(hwnd, WM_UNDO, 0, 0L);
					return 0;
				}
			break;

		//we'll never reach killfocus as a result of destroying the
		//edit field since the old wndproc would have previously been
		//restored and therefore there would be no way to reach this
		//code.
		case WM_ACTIVATE:
			if(wParam)
				break;

      wParam = lParam;
			//else fall through
			  
		case WM_KILLFOCUS:
			//make sure all other window's processing occurs before killing
			//the edit field.

			PostMessage(hwnd, WM_USER_DOEDITKILL, wParam, lParam);
			break;

		case WM_USER_DOEDITKILL:
			possibleParent = (HWND) wParam;

			//if the edit fiels parent is causing it to lose focus, don't kill it.  The
			//death will occur at more appropriate places.
      if(possibleParent != GetParent(hwnd) &&
				((editObject!= NULLSERUID) && 
      	((LPOBJINST)GetObject(editObject))->GetGroupPtr()->TestEditFieldFlag()))
				((LPOBJINST)GetObject(editObject))->GetGroupPtr()->KillEditField(TRUE, TRUE, FALSE);
			return 0; 

		case WM_UNDO:
			((LPOBJINST)GetObject(editObject))->GetGroupPtr()->KillEditField(TRUE, FALSE, FALSE);
			return 0;

		}

	return CallWindowProc((FARPROC)oldEditProc, hwnd, message, wParam, lParam);
	}


////////////////////////////////////////////////////////////////////////////
void OBJGROUP::InvalidateObjRect(
	LPOBJINST objPtr)
	{
	HDC hdc;
	HWND hwnd;
	RECT rc;

	hwnd = GetSubjectPtr()->GetWindowPtr()->GetWindowHandle();
	hdc = GetDC(hwnd);
	GetObjectRect(objPtr->GetIndex(), (LPRECT)&rc);
	LPtoDP(hdc, (LPPOINT)&rc, 2);
	ReleaseDC(hwnd, hdc);
	InvalidateRect(hwnd, (LPRECT)&rc, FALSE);
	}

void OBJGROUP::ValidateObjRect(
	LPOBJINST objPtr)
	{
	HDC hdc;
	HWND hwnd;
	RECT rc;

	hwnd = GetSubjectPtr()->GetWindowPtr()->GetWindowHandle();
	hdc = GetDC(hwnd);
	GetObjectRect(objPtr->GetIndex(), (LPRECT)&rc);
	LPtoDP(hdc, (LPPOINT)&rc, 2);
	ReleaseDC(hwnd, hdc);
	ValidateRect(hwnd, (LPRECT)&rc);
	}

/////////////////////////////////////////////////////////////////////////////
void OBJGROUP::SelectObjChain(
	HDC hdc,
	LPOBJINST objPtr,
	LPLIST selectedFuncs,
	LPLIST selectedFlows,
  WPARAM wParam,
	BOOL drawAll)
	{
	if(drawAll)
		{
		SelectObj(NULL, objPtr, wParam);
		Draw(hdc);
		}
  else
		SelectObj(hdc, objPtr, wParam);

	objPtr->SelectSignalChain(hdc, selectedFuncs, selectedFlows);
  }

/////////////////////////////////////////////////////////////////////////////
void OBJGROUP::SelectAllObjChains(
	HDC hdc,
	LPLIST selectedFuncs,
	LPLIST selectedFlows,
	BOOL drawAll)
	{
	int i;
	LPSERUID dataPtr;

	dataPtr = GetIndexesPtr();

	if(drawAll)
		{
		SelectObjRange(hdc, 0, objCount-1, OBJGROUP_NOREFRESH);
		Draw(hdc);
		}
  else
		SelectObjRange(hdc, 0, objCount-1, OBJGROUP_TOTALREFRESH);

	//now select all signal chains
	for(i=0;i<objCount;i++)
		{
		((LPOBJINST)GetObject(dataPtr[i]))->SelectSignalChain(hdc, selectedFuncs,
			selectedFlows);
		}
	}


//////////////////////////////////////////////////////////////////////////////
//returns TRUE if we can do something else returns FALSE
BOOL OBJGROUP::HandleInteriorDrag(
	HDC hdc,
	WPARAM wParam,
	POINT mouse)
	{
  int newSelected;
	LPOBJINST objPtr;
	BOOL result = TRUE; //assume we did something;
  WORD refresh;

	//Get the item that the mouse is over.
	objPtr = ObjectFromY(mouse.y);
	if(objPtr)
		{
		newSelected = objPtr->GetIndex();
		//if the shift or control key is down, then select from the top selected item to
		//the cursor.
		if(wParam & (MK_SHIFT | MK_CONTROL))
			{
			//decide whether to do a partial or total refresh - do a total only if the
			//number of selected objects is substantially greater than the range.
			if(labs(selObjCount - (groupInfo->mark - newSelected)) >
				OBJGROUP_MAXPARTIALREFRESH)
				refresh = OBJGROUP_PARTIALREFRESH;
			else
				refresh = OBJGROUP_TOTALREFRESH;

			SelectObjRange(hdc, MIN(groupInfo->mark, newSelected),
				MAX(groupInfo->mark, newSelected), refresh);
			}
		else //let select object handle everything
			{
			//see if we need to do anything
			if(GetTopSelectedIndex() != objPtr->GetIndex() || selObjCount)
				SelectObj(hdc, objPtr, wParam);
      }
		}
	else
  	result = FALSE;  //couldn't retrieve an object

	return result;
	}


///////////////////////////////////////////////////////////////////////////
//Unselects the group without unselecting the objects
//and does the minimal required to erase the selection.
void OBJGROUP::EraseSelection(
	HDC hdc)
	{
	if(IsSelected())
		{
		SetSelected(FALSE);
		DrawSelectionRect(hdc);
		}
	}

void OBJGROUP::DrawObjectDrag(
	HDC hdc,
	POINT offset)
	{
	LPOBJINST objPtr;
	LPSERUID dataPtr;
	int i;
	int numDrawn;
	RECT rc;

	if(selObjCount != 0)
		{
		numDrawn = 0;
		dataPtr = GetIndexesPtr();
		for(i=0;i<objCount && numDrawn <= selObjCount;i++)
			{
			objPtr = (LPOBJINST)GetObject(dataPtr[i]);
			if(objPtr->TestSelectFlag())
				{
				GetObjectRect(i, (LPRECT)&rc);
        OffsetRect((LPRECT)&rc, offset.x, offset.y);
				objPtr->DrawDrag(hdc, (LPRECT)&rc);
        numDrawn++;
        }
      }
		}	
	}

void OBJGROUP::DrawInsertMark(
	HDC hdc,
	int index)
	{
	int y;
	int oldMode = SetROP2(hdc, R2_NOTXORPEN);
	HPEN oldPen = SelectPen(hdc, insertMarkPen);

	y = objBounds.top + (index * GetSlotHeight());

	MoveToEx(hdc, objBounds.left, y, NULL);
	LineTo(hdc, objBounds.right, y);
	MoveToEx(hdc, objBounds.left, y+1, NULL);
	LineTo(hdc, objBounds.right, y+1);

	SelectPen(hdc, oldPen);
	SetROP2(hdc, oldMode);
	}

int OBJGROUP::GetHighlightIndex(
	POINT mouse)
	{
	int index = GetNewObjIndex(mouse.y);
	if(index < 0)
		index = 0;
	else if(index > objCount)
		index = objCount;

	return index;
	}

int OBJGROUP::GetCaptionHeight()
	{
	HDC hdc;
	TEXTMETRIC tm;

	hdc = CreateDC("DISPLAY", NULL, NULL, NULL);
	SelectObject(hdc, captionFont);
	GetTextMetrics(hdc, (LPTEXTMETRIC)&tm);
	DeleteDC(hdc);
	return tm.tmHeight + OBJGROUP_VOBJECTSEPARATION;
	}

LPOBJINST OBJGROUP::GetNextSelObj(
	int * selIndex,
	int * selCount)
	{
	LPOBJINST objPtr = NULL;
	LPOBJINST tempObj;
	LPSERUID dataPtr; 

	if(*selIndex >= 0 && *selIndex < objCount && *selCount <= selObjCount)
  	{
 		dataPtr = GetIndexesPtr();
 		while(*selIndex < objCount)
			{
			tempObj = (LPOBJINST)GetObject(dataPtr[*selIndex]);
			(*selIndex)++;
			if(tempObj->TestSelectFlag())
				{
				objPtr = tempObj;
				(*selCount)++;
        break;
				}
			}
		}
	return objPtr;
  }


BOOL OBJGROUP::EditGroupName(
	HDC hdc)
	{
	BOOL result;

	MDI::ModalizeVAB(TRUE);
	result = DialogBoxParam(PROGSYSTEM::hInst, "GROUPNAME", PROGSYSTEM::hWndFrame,
		(DLGPROC)OBJGROUP::ChangeGroupName, (long)(LPCSTR)groupName);
	MDI::ModalizeVAB(FALSE);
	if(result)
  	{
		groupName = nameBuffer;
		DrawCaption(hdc);
		}
		 
	return result;
  }

BOOL CALLBACK OBJGROUP::ChangeGroupName(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	HWND		hEdit;

	switch(message)
		{
		case WM_INITDIALOG:
			hEdit = GetDlgItem(hDlg, OBJGROUP_NAMEEDIT);
			CenterDialog(hDlg, PROGSYSTEM::hWndFrame, FALSE);
			Edit_SetText(hEdit, (LPSTR) lParam);
			Edit_LimitText(hEdit,	OBJGROUP_MAXNAMELENGTH);
			Edit_SetSel(hEdit, 0, 0);

			return(TRUE);

		case WM_COMMAND:
			switch(GET_WM_COMMAND_ID(wParam, lParam))
			{
			case IDOK:
				GetDlgItemText(hDlg, OBJGROUP_NAMEEDIT,
										 (LPSTR)OBJGROUP::nameBuffer, OBJGROUP_MAXNAMELENGTH);
				EndDialog(hDlg, TRUE);
				return(TRUE);

			case IDCANCEL:
				EndDialog(hDlg, FALSE);
				return(TRUE);

			}
			break;
	}
	return(FALSE);
}


BOOL OBJGROUP::ChangeSharedState(
	BOOL makeShared)
	{
	LPSERUID dataPtr;
	int numChanged = 0;
	int i=0;
  LPOBJINST objPtr;
	HDC hdc;
	HWND hWnd;

	if(selObjCount)
		{
		dataPtr = GetIndexesPtr();
		while(numChanged < selObjCount && i<objCount)
			{
			objPtr = (LPOBJINST)GetObject(dataPtr[i]);
			if(objPtr->TestSelectFlag() && !objPtr->TestAliasFlag())
				{
				if(makeShared)
					objPtr->SetShareFlag();
				else
					objPtr->ClearShareFlag();

				numChanged++;
      	}
			i++;
			}
		if(numChanged)
			{
			//draw the entire group to make sure that nothing is on top of it.
			hWnd = GetSubjectPtr()->GetWindowPtr()->GetWindowHandle();
			hdc = GetDC(hWnd);
			Draw(hdc);
			ReleaseDC(hWnd, hdc);
      }
		}

  return (numChanged > 0)?TRUE:FALSE;
	}


short OBJGROUP::PostReadInit()
	{
	int i=0;
	short result = PRJ_READNOERROR;
	short tempResult;
	LPOBJINST objPtr;
  LPSERUID dataPtr = GetIndexesPtr();


	while(!(result & PRJ_READERROR) && i<objCount)
		{
    objPtr = (LPOBJINST)GetObject(dataPtr[i]);
		//we must temporarily assign the results of PostReadInit to a temp
		//variable so we can tell each time if that particular object was bad.
		tempResult = objPtr->PostReadInit();
		if(tempResult & PRJ_READBADOBJECT)
			{
			objPtr->HandleBadFlows();
			OnlyRemoveObject(objPtr);
			DeleteObjInst(objPtr);
			ResetObjIndexes();
			CalculateBounds();
			dataPtr = GetIndexesPtr();
			//don't increment i since the array shrunk.
			}
		else
			i++;

		result |= tempResult;
		}

	return result;
	}

void OBJGROUP::CalculateObjFlows()
	{
	int i;
  LPSERUID dataPtr = GetIndexesPtr();

	for(i=0;i<objCount;i++)
		{
		((LPOBJINST)GetObject(dataPtr[i]))->RecalculateFlowPaths();
		}
	}

void OBJGROUP::DrawBorderDrag(
	HDC hdc,
	POINT offset,
	WORD dragBorder)
	{
	RECT oldObjBounds = objBounds;
	POINT oldLocation = location;

	DoGroupResize(offset, dragBorder);

	DrawSelectionRect(hdc);

  objBounds = oldObjBounds;
	location = oldLocation;
	}


void OBJGROUP::DoGroupResize(
	POINT offset,
	WORD dragBorder)
	{
	int minGroupWidth = GetMinGroupWidth();

	if(dragBorder & OBJGROUP_RIGHTBORDERHIT)
		{
		objBounds.right += offset.x;
		if(objBounds.right - objBounds.left + 1 < minGroupWidth)
			objBounds.right = objBounds.left + minGroupWidth;
		}
	else
		{
		objBounds.left += offset.x;
		location.x += offset.x;
		if(objBounds.right - objBounds.left + 1 < minGroupWidth)
			{
			objBounds.left = objBounds.right - minGroupWidth + 1;
			location.x = objBounds.left;
      }
		}
  }

void OBJGROUP::ResizeGroupHorizontally(
	POINT offset,
  POINT /*funcOffset*/, //adjusted for grid if neccesary
	WORD dragBorder)
	{
	POINT oldNipple;
	POINT newNipple;
	int i;
	LPSERUID objIndexes = GetIndexesPtr();

  GetNippleTip(&oldNipple, ObjectFromIndex(0));  
	DoGroupResize(offset, dragBorder);
	GetNippleTip(&newNipple, ObjectFromIndex(0));

	//the nipple POINT will tell how  the group moved horizontally
	for(i=0;i<objCount;i++)
		{
		((LPOBJINST)GetObject(objIndexes[i]))->MoveFunctionChainRelative(
			newNipple, oldNipple);
		}

	GetSubjectPtr()->ClearAllChainFlags();

	CalculateObjFlows();
	}


int OBJGROUP::GetMinGroupWidth()
	{
	return GetIconSize() + 2*OBJGROUP_BORDER + OBJGROUP_HOBJECTBORDER +
  	OBJGROUP_NIPPLEWIDTH;
	}


void OBJGROUP::SetGroupIconSize(
	WORD newSize)
	{
	if(newSize == OBJGROUP_SETLARGEICONS &&
		!TestLargeIconsFlag())
		{
		SetLargeIconsFlag();
    CalculateBounds();
		}
	else if(newSize == OBJGROUP_SETSMALLICONS &&
		TestLargeIconsFlag())
		{
		ClearLargeIconsFlag();
    CalculateBounds();
		}
	}

void OBJGROUP::MakeObjectsPersistent()
	{
	LPSERUID dataPtr;
	int i;
	LPOBJINST objPtr;
	HDC hdc;
  HWND hWnd;
	char buffer[128];
	char title[32];
	BOOL showMessage = FALSE;

	if(selObjCount)
		{
		dataPtr = GetIndexesPtr();

		hWnd = GetSubjectPtr()->GetWindowPtr()->GetWindowHandle();
		hdc = GetDC(hWnd);

		for(i=0;i<objCount;i++)
			{
			objPtr = (LPOBJINST)GetObject(dataPtr[i]);
			if(objPtr->TestSelectFlag())
				{
				if(!objPtr->objDesc->TestCantBePersistentFlag())
					{
					if(!objPtr->TestPersistentsFlag())
						{
						objPtr->SetPersistentsFlag();
						DrawObject(hdc, objPtr, TRUE); //TRUE forces offscreen drawing
						}
					}
				else
					{
					if(selObjCount == 1)
						{
						lstrcpy(buffer, objPtr->objDesc->pGetName());
						lstrcat(buffer, " objects can't be persistent.");	
						}
					else if(!showMessage)
						{
						lstrcpyn(buffer, "Some selected objects can't be persistent.  "
							"However, objects which can be persistent were made persistent.",
							sizeof(buffer));

						buffer[sizeof(buffer)-1] = '\0';
						}

					showMessage = TRUE;
					}
				}
			}

		if(showMessage)
			{
			LoadString(PROGSYSTEM::hInst, STR_FRAMENAME, title, sizeof(title));
			MessageBox(NULL, buffer,title, MB_TASKMODAL|MB_ICONEXCLAMATION|MB_OK);						
			}

		ReleaseDC(hWnd, hdc);
    }
	}

void OBJGROUP::MakeObjectsNonPersistent()
	{
	LPSERUID dataPtr;
	int i;
	LPOBJINST objPtr;
	HDC hdc;
  HWND hWnd;

	if(selObjCount)
		{
		dataPtr = GetIndexesPtr();

		hWnd = GetSubjectPtr()->GetWindowPtr()->GetWindowHandle();
		hdc = GetDC(hWnd);

		for(i=0;i<objCount;i++)
			{
			objPtr = (LPOBJINST)GetObject(dataPtr[i]);
			if(objPtr->TestPersistentsFlag() && objPtr->TestSelectFlag() &&
				!objPtr->objDesc->TestMustBePersistentFlag())
				{
				objPtr->ClearPersistentsFlag();
				DrawObject(hdc, objPtr, TRUE); //TRUE forces offscreen drawing
				}
			}

		ReleaseDC(hWnd, hdc);
    }
	}

//removes a single object from the group and deletes the object
int OBJGROUP::DestroyObject(
	LPOBJINST objPtr)
	{
	int originalCount = objCount;

	OnlyRemoveObject(objPtr); //resets the objCount

  //if the before and after count are different, the obect was removed.
	if(originalCount != objCount)
		{
		DeleteObjInst(objPtr);
		ResetObjIndexes();
		CalculateBounds();
		}

	return  objCount;
  }

//Makes copies of all objects in sourceGroup and inserts them.
//objCount already be set and objIndexes block should already be sized.
BOOL OBJGROUP::CopyObjects(
	LPOBJGROUP sourceGroup,
	RUNTIMEUID objectBlock)
	{
	LPSERUID dstIndexes = GetIndexesPtr();
	LPSERUID srcIndexes = sourceGroup->GetIndexesPtr();
	LPOBJINST srcObj;
	LPOBJINST dstObj;
	int i;
	SERUID groupUID = GetUID(this);
	SERUID dstUID;
  SERUID srcUID;
	long dataCount;
	long dataSize;
	int j;
	LPVOID dataPtr;
	long mes2 = 0;
	long mes3 = 0;
	long mes4 = 0;
	LPSUBJECT subjectPtr = GetSubjectPtr();
  LPPROJECT projectPtr = subjectPtr->GetParentProject();
	long objStatus;
	TSTRING objName;
	long typeID;
	SERUID typeUID;

  selObjCount = 0;

	//go throught the srcgroup object list and make copies of all the objects
  //and insert them.
	for(i=0;i<objCount;i++)
		{
		srcObj = (LPOBJINST)GetObject(srcIndexes[i]);
		//store the source obj members in locals because the new for the OBJINST
		//constructor may move memory and cause the srcObj pointer to change.
		objStatus = srcObj->GetObjectStatus();
		objName = srcObj->GetName();
		typeID = srcObj->GetTypeID();

		typeUID = subjectPtr->IncTypeCount(typeID, !srcObj->TestAliasFlag());
		I_ASSERT(typeUID != NULLSERUID);

		dstObj = new(objectBlock) OBJINST(typeID, objName, objStatus,
			subjectUID, typeUID);

		//get the srcObj pointer again to make sure it's valid
		srcObj = (LPOBJINST)GetObject(srcIndexes[i]);

		if(srcObj->TestSelectFlag())
    	{
			dstObj->SetSelectFlag();
			selObjCount++;
			}

		dstObj->SetGroupUID(groupUID);
		dstObj->SetIndex(i);

		if((!srcObj->TestAliasFlag()) || (srcObj->GetSharedObjectUID() == 0))
			{
			//copy the datas

			srcUID = GetUID(srcObj);
			dstUID = GetUID(dstObj);

			SendMessageToObject(AOBJ_PREWRITE, mes2,mes3, mes4, srcUID.id);
			
			dataCount = AObjGetDataCount(srcUID.id);

			//Set the data count to avoid memory fragmentation caused by
			//AddData
			AObjSetDataCount(dstUID.id, dataCount);

			for(j=0;j<dataCount;j++)
				{
				dataPtr = AObjLockData(srcUID.id, j);

				dataSize = AObjGetDataSize(srcUID.id, j);
        
        AObjSetData(dstUID.id, j, dataPtr, dataSize);

        if(dataPtr)
					AObjUnlockData(srcUID.id, j);
				}

			SendMessageToObject(AOBJ_POSTWRITE, mes2, mes3, mes4, srcUID.id);
			SendMessageToObject(AOBJ_AWAKENED, mes2, mes3, mes4, dstUID.id);
			}
		else
			dstObj->SetSharedObjectUID(srcObj->GetSharedObjectUID());

		dstIndexes[i] = GetUID(dstObj);
		subjectPtr->IncObjectCount();
		projectPtr->IncObjectsCount();
		}

  return TRUE; //signal No Error
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ObjectValueDialog
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
void OBJGROUP::ObjectValueDialog(
	LPCSTR valueString)
	{
	MDI::ModalizeVAB(TRUE);

	DialogBoxParam(PROGSYSTEM::hInst, "OBJVALUE", PROGSYSTEM::hWndClient, 
								(DLGPROC)OBJGROUP::ObjValueDlgProc, (long)valueString);

	MDI::ModalizeVAB(FALSE);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ObjValueDlgProc
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK OBJGROUP::ObjValueDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{

	switch(message)
		{
		case WM_INITDIALOG:
			CenterDialog(hDlg, PROGSYSTEM::hWndFrame, FALSE);
			oldObjValueProc = (WNDPROC)SetWindowLong(GetDlgItem(hDlg, ID_OBJVALUE),
			 GWL_WNDPROC, (LONG)OBJGROUP::ObjValueEditProc);
			SetDlgItemText(hDlg, ID_OBJVALUE, (LPSTR)lParam);
			return FALSE;

		case WM_COMMAND:
			switch(GET_WM_COMMAND_ID(wParam, lParam))
				{
				case IDOK:
					EndDialog(hDlg, 0);
					return TRUE;
				}
			break;

		case WM_DESTROY:
			break;

		}
	return FALSE;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ObjValueEditProc
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK OBJGROUP::ObjValueEditProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{

	HideCaret(hwnd);

	switch(message)
		{
		case WM_CHAR:
		case WM_KEYDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_MOUSEMOVE:
			return 0;

		case WM_LBUTTONDOWN:
			if(wParam & MK_SHIFT)
				return 0;
			break;

		}

	return CallWindowProc((FARPROC)OBJGROUP::oldObjValueProc, hwnd, message, wParam,
	 lParam);

	}


/////////////////////////////////////////////////////////////
// FUNCTION:			OBJGROUP::ExportUPSF
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
WORD OBJGROUP::ExportUPSF()
	{
	int tempInt;
  BOOL tempBool;
	int exportCount= 0;
	int i;
	long result;
	WORD returnValue = EXPORT_NOERROR;
	LPSERUID dataPtr = GetIndexesPtr();
	LPSUBJECT subjectPtr = GetSubjectPtr();

	//export param name	and check to see if we're out of disk space
	I_ALWAYSASSERT(AUpsfExportField(GROUP_GROUPNAME, 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT,
		(LPVOID)((LPCSTR)groupName), (long)groupName.strlen()) ==AUPSF_NOERROR,
		returnValue |= EXPORT_GENERALERROR);


	//export x coordinate
	I_ALWAYSASSERT(AUpsfExportField(GROUP_LOCATION, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&location.x, (long)sizeof(int)) == AUPSF_NOERROR,
			returnValue|=EXPORT_GENERALERROR);

	//export y coordinate
	I_ALWAYSASSERT(AUpsfExportField(GROUP_LOCATION, 1L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&location.y, (long)sizeof(int)) == AUPSF_NOERROR,
			returnValue|=EXPORT_GENERALERROR);

	//export width
	tempInt = GetWidth();
	I_ALWAYSASSERT(AUpsfExportField(GROUP_WIDTH, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&tempInt, (long)sizeof(int)) == AUPSF_NOERROR,
			returnValue|=EXPORT_GENERALERROR);

  //export Icon size status
  tempBool = (TestLargeIconsFlag())?TRUE:FALSE;
  
  I_ALWAYSASSERT(AUpsfExportField(GROUP_LARGEICONS, 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL,
		(LPVOID)&tempBool, (long)sizeof(BOOL)) == AUPSF_NOERROR,
		returnValue|=EXPORT_GENERALERROR);


	I_ALWAYSASSERT((result = AUpsfCreateScope(GROUP_OBJECTS, 0L))
		== AUPSF_NOERROR, returnValue |= EXPORT_GENERALERROR);
		
	if(result == AUPSF_NOERROR)
		{
		for(i=0;i<objCount;i++)
			{

			tempInt = subjectPtr->UPSFObjToIndex(dataPtr[i]);
			
			I_ASSERT(tempInt != SJT_UPSFINDEXERROR);

			I_ALWAYSASSERT(AUpsfExportField(GROUP_FIELDOBJECTINDEX, exportCount, 
				AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, (LPVOID)&tempInt, 
				(long)sizeof(int)) == AUPSF_NOERROR, 
				returnValue |= EXPORT_GENERALERROR);

			exportCount++;   
			}

		AUpsfCloseScope();
		}

	result = AUpsfExportField(GROUP_OBJECTCOUNT, 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER,
		(LPVOID)&exportCount, (long)sizeof(int));

	I_ASSERT(result == AUPSF_NOERROR);

	if(result == AUPSF_OUTOFDISKSPACE)
		returnValue |= EXPORT_OUTOFDISK;
	else if(result != AUPSF_NOERROR)
		returnValue|=EXPORT_GENERALERROR;

	PROJECT::IncStatusBar();

	return returnValue;
	}


/////////////////////////////////////////////////////////////
// FUNCTION:			OBJGROUP::ImportUPSF
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL OBJGROUP::ImportUPSF()
	{
	char nameBuffer[64];
	char msgBuffer[128];
	long result;
	int tempInt;
  BOOL tempBool;
	int i;
	LPSERUID dataPtr;
	LPSUBJECT subjectPtr = GetSubjectPtr();
	int goodCount;
	SERUID groupUID = GetUID(this);
	LPOBJINST objPtr;

	I_ALWAYSASSERT((result = AUpsfImportField(GROUP_GROUPNAME, 0L, AUPSF_NATV_STRING,
		(LPVOID)nameBuffer, (long)sizeof(nameBuffer))) != AUPSF_MEMORYERROR,
		return FALSE);

	if(UPSF_GOODSTRINGRESULT(result))
		groupName = nameBuffer;
	else
		{
		GetSubjectPtr()->GetParentProject()->GetNewGroupName(nameBuffer);
		groupName = nameBuffer;
		
		wsprintf(msgBuffer, "group's name so using default name of: %s",
			nameBuffer);

		apb_LogUpsfError(result, msgBuffer);
		}

	I_ALWAYSASSERT((result = AUpsfImportField(GROUP_WIDTH, 0L, AUPSF_NATV_INT,
		(LPVOID)&tempInt, (long)sizeof(int))) != AUPSF_MEMORYERROR, 
		return FALSE);
		
	if(result == AUPSF_NOERROR)
		{
		I_ASSERT(tempInt > 0);

		if(tempInt < GetMinGroupWidth())
			AUpsfLogComment("Group width was too narrow so using default width.");
		else
			objBounds.right = objBounds.left + tempInt;
		}
	else
		apb_LogUpsfError(result, "group's width");


  I_ALWAYSASSERT((result = AUpsfImportField(GROUP_LARGEICONS, 0L, AUPSF_NATV_BOOL,
    (LPVOID)&tempBool, (long)sizeof(BOOL))) != AUPSF_MEMORYERROR,
    return FALSE);

  if(result == AUPSF_NOERROR)
    {
    if(tempBool)
      SetLargeIconsFlag();
    else
      ClearLargeIconsFlag();
    }
  else
    apb_LogUpsfError(result, "group's icon size");
		
	I_ALWAYSASSERT((result = AUpsfImportField(GROUP_OBJECTCOUNT, 0L,
		AUPSF_NATV_INT, (LPVOID)&tempInt, (long)sizeof(int))) == AUPSF_NOERROR,
		{apb_LogUpsfError(result, "group's object count", APB_UPSFFATALERROR);
		return FALSE;});
		
	I_ASSERT(tempInt > 0); 	//Group shouldn't have been exported if there
													//weren't any objects.
													
	objCount = tempInt;
	
	((LPSERIUSBLOCK)GetObject(objIndexes))->ResizeBlock(sizeof(SERUID)* objCount);
	dataPtr = GetIndexesPtr();
	
	goodCount = 0;

	
	I_ALWAYSASSERT((result = AUpsfOpenScope(GROUP_OBJECTS, 0L)) ==
		AUPSF_NOERROR,
		{apb_LogUpsfError(result, "group's object scope", 
			APB_UPSFOPEN | APB_UPSFFATALERROR); return FALSE;});

	PROJECT::PushScopeStack(ST_FULL);

	for(i=0;i<objCount;i++)
		{

		I_ALWAYSASSERT((result = AUpsfImportField(GROUP_FIELDOBJECTINDEX, (long)i,
			AUPSF_NATV_INT, (LPVOID)&tempInt, (long)sizeof(int))) == AUPSF_NOERROR,
			{apb_LogUpsfError(result, "group's object index", 
				APB_UPSFOPEN | APB_UPSFFATALERROR); return FALSE;});

		dataPtr[goodCount] = subjectPtr->UPSFIndexToObj(tempInt);

		if(dataPtr[goodCount] != NULLSERUID)
			{
			objPtr = (LPOBJINST)GetObject(dataPtr[goodCount]);
			
			I_ALWAYSASSERT(objPtr != NULL,
				{AUpsfLogComment("Fatal Error. Invalid object index in group.");
				return FALSE;});

			objPtr->SetGroupUID(groupUID);
			objPtr->SetIndex(goodCount);

			goodCount++;
			}
		else
			{
			wsprintf(msgBuffer, "Invalid object index found in group: %s.",
				(LPCSTR)groupName);

			AUpsfLogComment(msgBuffer);
			}			 
		}

	PROJECT::PopScopeStack();
	AUpsfCloseScope();
	
	if(goodCount != objCount)
		{
		((LPSERIUSBLOCK)GetObject(objIndexes))->ResizeBlock(-((long)sizeof(SERUID) * (goodCount - objCount)));
		objCount = goodCount;
		}

	CalculateBounds();
	
	return TRUE; //no error if we reach here.	
	} 

/////////////////////////////////////////////////////////////
// FUNCTION:			OBJGROUP::MoveNearRectangle
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////

//make sure the functions are  enough from the groups that the signals
//will be readable
#define OBJGROUP_DEFFUNCSPACE 64
void OBJGROUP::MoveNearRectangle(
	LPRECT rt)
	{
	POINT offset;

	offset.x = (rt->left - (objBounds.right - objBounds.left) - 
		OBJGROUP_DEFFUNCSPACE) - location.x;

	offset.y = rt->top - location.y;

	Move(offset);
	} 

																			 
