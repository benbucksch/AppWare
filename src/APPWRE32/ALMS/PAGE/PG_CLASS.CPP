///////////////////////////////////////////////////////////////
//
//  (c) 1993 Novell, Inc.  All rights reserved.
//
//  THIS WORK IS SUBJECT TO U.S. AND INTERNATIONAL COPYRIGHT LAWS AND
//  TREATIES.  NO PART OF THIS WORK MAY BE USED, PRACTICED, PERFORMED,
//  COPIED, DISTRIBUTED, REVISED, MODIFIED, TRANSLATED, ABRIDGED,
//  CONDENSED, EXPANDED, COLLECTED, COMPILED, LINKED, RECAST,
//  TRANSFORMED OR ADAPTED WITHOUT THE PRIOR WRITTEN CONSENT OF
//  NOVELL, INC.  ANY USE OR EXPLOITATION OF THIS WORK WITHOUT
//  AUTHORIZATION COULD SUBJECT THE PERPETRATOR TO CRIMINAL AND CIVIL
//  LIABILITY.
//
///////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
//
// Object Group:  Essentials
// Object:        Page
//
// Filename:  "pg_class.cpp"
//
// Description:
//  Page Object Class
//
////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>

#include "a_alm.h"
#include "intdebug.h"
#include "a_almutl.h"
#include "o_wnd.h"
#include "o_wndlay.h"
#include "o_print.h"
#include "pg_class.h"
#include "pg_macro.h"
#include "pg_dialg.h"
#include "pg_glob.h"

PAGE::PAGE ( AOBJECTID id )
{
	HDC hdc;

	hActive = GetActiveWindow();

	objectId  = id;

	hdc = GetDC(hActive);

	winpixinch_x = GetDeviceCaps(hdc, LOGPIXELSX);  // default for monitor pixel per inch
	winpixinch_y = GetDeviceCaps(hdc, LOGPIXELSY);

	aspectx = GetDeviceCaps(hdc, ASPECTX);
	aspecty = GetDeviceCaps(hdc, ASPECTY);

	ReleaseDC(hActive, hdc);

	if (AObjGetDataCount(objectId) == 0)
		AObjSetDataCount(objectId, DATACOUNT);

//  AObjResizeData(objectId, 0L, sizeof(DATA0));
	data0 = LockPageData0(objectId);

	if (data0 == NULL)
	{
		AObjResizeData(objectId, 0L, sizeof(DATA0));
		I_ASSERT(objectId);
		data0 = LockPageData0(objectId);
		I_ASSERT(objectId);

		data0->version      = VERSION_OBJECT;
		data0->count        = 0;    // count of DATA1 instances
		data0->nextWindow   = 0;    // index into data1
		data0->spooled      = 0;    // spooled windows
		data0->pages        = 0;    // total pages spooled
		data0->hFile        = (HANDLE)-1;   // file handle

		data0->spoolFile[0] = 0;    // spool file name
		data0->snap         = 0;
	}
	if(data0->version == 101)
		data0->version = VERSION_OBJECT;

	UnlockPageData0(objectId);

}


// Runtime Constructor
PAGE::PAGE ( AOBJECTID id, pAEvtInfo eventptr, pAObjMessage systemptr)
{
	HDC hdc;                                                                    

	hActive = GetActiveWindow();

	objectId  = id;

	hdc = GetDC(hActive);

	winpixinch_x = GetDeviceCaps(hdc, LOGPIXELSX);  // default vga value
	winpixinch_y = GetDeviceCaps(hdc, LOGPIXELSY);

	eventPtr = eventptr;
	systemPtr = systemptr;

}


void PAGE::ExportObject ()
{
	UINT  i;
	long  ret;
	POINT wndpoint;

	ret = AUpsfExportField("VERSION", 0L, AUPSF_NATV_UINT, AUPSF_UNIV_INTEGER, &data0->version, sizeof(UINT));
	if (ret != AUPSF_NOERROR)
		MessageBox(NULL, "Page Layout: Error On Export 'VERSION'", NULL, MB_OK | MB_ICONEXCLAMATION);

	data0 = LockPageData0(objectId);
	data1 = LockPageData1(objectId);
	for (i = 0; i < data0->count; i++)
	{
		// window object ids
		ret = AUpsfExportField("WNDLIST", i, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &data1[i].objid, sizeof(AOBJECTID));
		if (ret != AUPSF_NOERROR)
			MessageBox(NULL, "Page Layout: Error On Export 'WNDLIST'", NULL, MB_OK | MB_ICONEXCLAMATION);

		wndpoint.x = (float(data1[i].xy.x / float(data1[i].pixinch_x) * 720.0) );
		wndpoint.y = (float(data1[i].xy.y / float(data1[i].pixinch_y) * 720.0) );

		// window offset decipoints
		ret = AUpsfExportField("WNDPOINT", i, AUPSF_NATV_POINT, AUPSF_UNIV_POINT, &wndpoint, sizeof(POINT));
		if (ret != AUPSF_NOERROR)
			MessageBox(NULL, "Page Layout: Error On Export 'WNDPOINT'", NULL, MB_OK | MB_ICONEXCLAMATION);
	}

	// export auto update and result object
	AUpsfExportField("SNAP", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_INTEGER, &data0->snap, sizeof(BOOL));
	if (ret != AUPSF_NOERROR)
		MessageBox(NULL, "Page Layout: Error On Export 'AUTOUPDT'", NULL, MB_OK | MB_ICONEXCLAMATION);

	UnlockPageData0(objectId);
	UnlockPageData1(objectId);

}


void PAGE::ImportObject ()
{
	UINT    i;
	UINT    count;
	long    ret;

	POINT   wndpoint;
	AOBJECTID  objidTemp;

	data0 = LockPageData0(objectId);
	ret = AUpsfImportField("SNAP", 0L, AUPSF_NATV_UINT, &data0->snap, sizeof(BOOL));
	if (ret != AUPSF_NOERROR)
	{
		AUpsfLogComment("Error On Import 'SNAP'; Default to no snap");
		data0->snap = 0;
	}

	count = (UINT)AUpsfFieldCount("WNDLIST");
	AObjResizeData(objectId, 1L, (count+1)*sizeof(DATA1));
	data1 = LockPageData1(objectId);

	if (count > 0)
	{
		for (i = 0; i < count; i++)
		{
			ret = AUpsfImportField("WNDLIST", i, AUPSF_NATV_OBJECTREF, &objidTemp, sizeof(AOBJECTID));
			if (ret != AUPSF_NOERROR)
			{
				AUpsfLogComment("Error On Import 'WNDLIST'; List Terminated");
				break;
			}

			ret = AUpsfImportField("WNDPOINT", i, AUPSF_NATV_POINT, &wndpoint, sizeof(POINT));
			if (ret != AUPSF_NOERROR)
			{
				AUpsfLogComment("Error On Import 'WNDPOINT'; List Terminated");
				break;
			}

			data1[i].objid = objidTemp;
			data1[i].name = NULL;

			data1[i].width  = 0;
			data1[i].height = 0;

			data1[i].pixinch_x = winpixinch_x;
			data1[i].pixinch_y = winpixinch_y;

			wndpoint.x = (float(wndpoint.x) * float(winpixinch_x) / 720.0);
			wndpoint.y = (float(wndpoint.y) * float(winpixinch_y) / 720.0);

			data1[i].xy.x = wndpoint.x;
			data1[i].xy.y = wndpoint.y;
		}
		data1[i].objid = NULL;        // Set NULL terminal for the last window object
		data0->count = count;
	}
	UnlockPageData1(objectId);
	UnlockPageData0(objectId);

}

void PAGE::Validate ( pAObjMessage sys )
{
	BOOL    changed;
	int     i;
	DATA1*  pageData1;

	// data 1
	data0 = LockPageData0(objectId);

	AObjResizeData(objectId, 1L, sizeof(DATA1)*(data0->count+1));
	data1 = LockPageData1(objectId);

	data1[data0->count].objid = NULL;
	data1[data0->count].name = NULL;

	for (i = 0; i < data0->count; i++)
	{
		pageData1 = data1+i;
		changed = AObjValidateID(&pageData1->objid, sys);
		if (changed && pageData1->objid == 0L)
		{
			DataLayoutDelete(data1+i);
			data0->count--;
			i--;
		}
	}

	UnlockPageData1(objectId);
	AObjResizeData(objectId, 1L, sizeof(DATA1)*(data0->count+1));

	UnlockPageData0(objectId);
}


void PAGE::CheckObjectID ( void )
{
	BOOL    valid;
	int     i;
	DATA1*  pageData1;

	// data 1
	data0 = LockPageData0(objectId);

	AObjResizeData(objectId, 1L, sizeof(DATA1)*(data0->count+1));
	data1 = LockPageData1(objectId);

	data1[data0->count].objid = NULL;
	data1[data0->count].name = NULL;

	for (i = 0; i < data0->count; i++)
	{
		pageData1 = data1+i;
		valid = AObjCheckType(pageData1->objid, 0L);
		if (!valid)
		{
			DataLayoutDelete(data1+i);
			data0->count--;
			i--;
		}

	}

	UnlockPageData1(objectId);
	AObjResizeData(objectId, 1L, sizeof(DATA1)*(data0->count+1));

	UnlockPageData0(objectId);

}

#pragma argsused
void PAGE::DataLayoutDelete ( DATA1* layoutPtr )
{
	DATA1* savedPtr;

	savedPtr = layoutPtr;           
	while ((layoutPtr+1)->objid)
	{
		*layoutPtr = *(layoutPtr+1);
		layoutPtr++;
	}
	 *layoutPtr = *(layoutPtr+1);
	 layoutPtr = savedPtr;
}


LONG PAGE::ObjectHealthy ( AObjInfo* infoRecordPtr )
{

	LONG   rVal;

	data0 = LockPageData0(objectId);
	if (data0->count == 0)
	{
		lstrcpy(infoRecordPtr->errorMessage, "Page Layout object must contain at least 1 window");
		rVal = A_WARNING;

	}
	else
	{
		rVal = A_OK;

	}
	UnlockPageData0(objectId);

	return rVal;
}


void PAGE::Pidr ( void )
{
	int           ret;
	int           totalSpool;
	int           width;
	int           height;
	ITEMTYPE      itemType;
	AOBJECTID      unspooledWindow;
	LPPIDR_BLOCK  pidr;
	int           toPage;
	RECT          rect;
	POINT         gripEdge;
	BOOL          fHorz;
	BOOL          fVert;
	POINT         ptScroll;


	pidr = (LPPIDR_BLOCK) eventPtr->details;
	toPage = pidr->toPage;
	data0 = LockPageData0(objectId);

	if (pidr->numPages)
	{
		pidr->toPage = data0->pages;
		UnlockPageData0(objectId);
		return;
	}

	ret = Escape(pidr->hDC, GETPRINTINGOFFSET, NULL, NULL, (char*)&gripEdge);
	if (ret <= 0)
	{
		// default to zero if error
		gripEdge.x = 0;
		gripEdge.y = 0;
	}

	// here we are computing the total windows to unspool
	if (pidr->toPage < data0->pages)
		totalSpool = data0->count * pidr->toPage;   // there are data0->count windows per page
	else
		totalSpool = data0->spooled;

	// start from beginning of file
	SetFilePointer(data0->hFile, 0, 0, FILE_BEGIN);

	for (int i = 0, j, currentPage; i < totalSpool; i++)
	{
		data1 = LockPageData1(objectId);

		j = i % data0->count;

		currentPage = (i / data0->count) + 1;
		unspooledWindow = UnSpoolObject(itemType);
		if (unspooledWindow == 0)
		{
			lstrcpy(pidr->error, "Page Layout: Error In Unspooling Window");
			UnlockPageData0(objectId);
			UnlockPageData1(objectId);
			return;
		}

		if (!(currentPage >= pidr->fromPage && currentPage <= pidr->toPage))
		{
			UnlockPageData1(objectId);
			continue;
		}

		if (unspooledWindow != data1[j].objid)
		{
			lstrcpy(pidr->error, "Page Layout: Objects Do Not Match In Spool File");
			UnlockPageData0(objectId);
			UnlockPageData1(objectId);
			return;
		}

		// build new print rect; use rect from window for size
		// use data1 layout point for offset
		fHorz = OWndLayHasHScroll(data1[j].objid);
		fVert = OWndLayHasVScroll(data1[j].objid);
		ptScroll = OWndLayGetScrollSize(data1[j].objid);

		// get window logical size(scroll area or physical size)
		OWndGetClientRect(data1[j].objid, &rect);

		if(fHorz)
			rect.right = rect.left + ptScroll.x;
		if(fVert)
			rect.bottom = rect.top + ptScroll.y;

		width  = rect.right - rect.left;    // in .1 mm units
		height = rect.bottom - rect.top;

		// use vga(screen) conversions for accuracy
		width  = (float)width  / 254. * (float)pidr->DPI.x;
		height = (float)height / 254. * (float)pidr->DPI.y;

		// this is from layout pixel origins
		rect.left = (float)data1[j].xy.x * (float)pidr->DPI.x / (float)data1[j].pixinch_x;
		rect.top  = (float)data1[j].xy.y * (float)pidr->DPI.y / (float)data1[j].pixinch_y;

		rect.left -= gripEdge.x;
		rect.top -= gripEdge.y;

		// this is from vga pixel origins
		rect.right  = rect.left + width;
		rect.bottom = rect.top + height;

		pidr->rect = rect;
//    MoveToEx(pidr->hDC, rect.left, rect.top);
//    LineTo(pidr->hDC, rect.right, rect.top);
//    LineTo(pidr->hDC, rect.right, rect.bottom);
//    LineTo(pidr->hDC, rect.left, rect.bottom);
//    LineTo(pidr->hDC, rect.left, rect.top);
		AEvtCallMethod(objectId, data1[j].objid, 0L, OPRINT_EVT_PRINTITEMDRAWROUTINE, long(pidr));
		pidr->toPage = toPage;

		// this is end of page test -- eject page except for last page
		if ( (i != totalSpool - 1) && (((i+1) % data0->count) == 0) )
		{
			if (!NewPage(pidr))
			{  
				UnlockPageData0(objectId);
				UnlockPageData1(objectId);
				AFuncReportResult(systemPtr, 0L, NULL);
				return;
			}
		}
		UnlockPageData1(objectId);
	}
	UnlockPageData0(objectId);
	AFuncReportResult(systemPtr, 0L, NULL);
}


AOBJECTID PAGE::UnSpoolObject ( ITEMTYPE& itype )
{
	OBJHEADER objHeader;
	long      ret;

	data0 = LockPageData0(objectId);

	ret = ReadFile(data0->hFile, &objHeader, sizeof(OBJHEADER), NULL, NULL);

	I_ASSERT(ret==sizeof(OBJHEADER));

	AObjReadDatas(objHeader.window_id, data0->hFile);

	for (int i=0; i<objHeader.numWndItem; i++)
	{
		ret = ReadFile(data0->hFile, &objHeader, sizeof(OBJHEADER), NULL, NULL);
		I_ASSERT(ret==sizeof(OBJHEADER));

		AObjReadDatas(objHeader.object_id, data0->hFile);
	}


	itype = objHeader.type;

	UnlockPageData0(objectId);

	return objHeader.window_id;
}


BOOL PAGE::NewPage ( LPPIDR_BLOCK pidr )
{
	int ret;

	ret = Escape(pidr->hDC, NEWFRAME, NULL, NULL, NULL);

	if (ret <= 0)
	{
		switch ( ret )
		{
			case SP_ERROR:
				lstrcpy(pidr->error, "General printing error.");
				break;

			case SP_OUTOFDISK:
				lstrcpy(pidr->error, "Not enough disk space is currently available for print spooling.");
				break;

			case SP_OUTOFMEMORY:
				lstrcpy(pidr->error, "Not enough memory is available for spooling.");
				break;

			case SP_USERABORT:
				lstrcpy(pidr->error, "User terminated the job through Print Manager.");
				break;  
		}
		return FALSE;
	}
	return TRUE;
}


void PAGE::SpoolWindow ( pAObjMessage sys )
{
	long          ret;
	OBJHEADER     objHeader;
	AOBJECTID      oiWindow;
	OpWndItemD*   wi;
	int           numCnt=0;
	
	data0 = LockPageData0(objectId);
	data1 = LockPageData1(objectId);

	objHeader.numWndItem = 0;

	oiWindow = data1[data0->nextWindow].objid; // get window object id

	wi = OWndGetNextItem(NULL, oiWindow);
	while (wi)    // counting the number of window items
	{
		if (OWndIsItemObject(wi))
			objHeader.numWndItem++;
		wi = OWndGetNextItem(wi, oiWindow);
	}

	wi = OWndGetNextItem(NULL, oiWindow);

	objHeader.window_id = oiWindow;
	objHeader.object_id  = oiWindow;
	objHeader.type = (data0->spooled==0) ? FIRST : RUN;

	ret = WriteFile(data0->hFile, &objHeader, sizeof(objHeader), NULL, NULL); // write object header
	if (ret == HFILE_ERROR)
	{
		AFuncReportResult(sys, 1, "Page Layout: write data failed");
		return;
	}

	AObjWriteDatas(oiWindow, data0->hFile); //write window object

	while (wi)
	{
		if (OWndIsItemObject(wi))
		{
			objHeader.window_id = oiWindow;
			objHeader.object_id  = wi->Object();

			if (numCnt==objHeader.numWndItem-1)
				objHeader.type = LAST;
			else if (!numCnt)
				objHeader.type = FIRST;
			else
				objHeader.type = RUN;
	
			ret = WriteFile(data0->hFile, &objHeader, sizeof(objHeader), NULL, NULL); //write object header
			if (ret == HFILE_ERROR)
			{
				AFuncReportResult(sys, 1, "Page Layout: write data failed");
				return;
			}

			AObjWriteDatas(objHeader.object_id,  data0->hFile); //wirte window item 

			numCnt++;

		}   
		
		wi = OWndGetNextItem(wi, oiWindow);
	}

	UnlockPageData0(objectId);
	UnlockPageData1(objectId);
	AFuncReportResult(sys, 1, NULL);
}


void PAGE::IncrementNextWindow ( void )
{
	data0 = LockPageData0(objectId);

	data0->nextWindow++;

	if (data0->count == data0->nextWindow)
		data0->nextWindow = 0;

	UnlockPageData0(objectId);

}

void PAGE::IncrementPage ( void )
{

	data0 = LockPageData0(objectId);

	data0->spooled++;

	data0->pages = data0->spooled / data0->count;
	if (data0->spooled % data0->count)
		data0->pages++;
	else
		AEvtPostSignalAtMark(objectId, SIGNAL_PAGEFINISHED);

	UnlockPageData0(objectId);
}

void PAGE::DeleteListData ( void )
{
	UINT        i;
	LRESULT     count;
	HWND        hwindow;
	DATA1*      layout;

	DeleteObject(hPen1);
	DeleteObject(hPen2);

	count = SendDlgItemMessage(hDialog, PAGESELECT, LB_GETCOUNT, 0, 0L);

	for (i = 0; i < count; i++)
	{
		hwindow = (HWND)SendDlgItemMessage(hDialog, PAGESELECT, LB_GETITEMDATA, i, 0L);
		layout = (DATA1*)GetWindowLong(hwindow, 0L);

		DestroyWindow(hwindow);
		if (layout != NULL)
			delete layout;
	}
}


