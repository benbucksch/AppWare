////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//  THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//  PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//  ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL EMPLOYEES WHO HAVE
//  A NEED TO KNOW TO PERFORM THEIR TASKS WITHIN THE SCOPE OF
//  THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL WHO HAVE
//  ENTERED INTO APPROPRIATE LICENCE AGREEMENTS.  NO PART OF THIS
//  WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//  REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//  COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//  WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL.  ANY USE OR
//  EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//  THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------------------
//
// FILE:          "balloon.cpp"
//
// AUTHOR:        Lee E. Thayer
//
// DESCRIPTION:   API routines for balloon server
//	
////////////////////////////////////////////////////////////////////////////

#define STRICT
#define ASTRICT


#include <windows.h>

#include <a_alm.h>
#include <intdebug.h>

#include "balloon.h"
#include "server.h"
#include "util.h"
#include "init.h"


extern HINSTANCE  	hInstance;
extern HWND 		hwndBalloon;
extern const char*	szBalloonClass;


////////////////////////////////////////////////////////////////////
//			
//			           A P I   R O U T I N E S		
//
// 	The following routines are all of the inteface or API
// 	routines that Balloon Clients can use.
//
//
////////////////////////////////////////////////////////////////////


//****************************************************************
//
//	Return values:
//
//		TRUE = Window registered
//
//****************************************************************
extern "C" int APIENTRY Balloon_RegisterClient(HWND hwndClient)
{
	I_ASSERT(hwndClient);
	if(!hwndClient)
		return FALSE;

	if (hwndBalloon)
	{
		Balloon *balloon = GetBalloon(hwndBalloon);
		return(balloon->RegisterClient(hwndClient));
	}
	else
	// Balloon server hasn't been started yet; this is the 
	// first client registration.
	{
		if(!RegisterBalloonClass())
		{
			I_PRINT("Balloon class failed Windows registration");
			return FALSE;
		}

		hwndBalloon = CreateWindowEx(WS_EX_TOPMOST,
				  					szBalloonClass,
				  				    NULL,
				  				    WS_POPUP,
				  			    	0, 0, 1, 1,
				  			    	HWND_DESKTOP,
				  				    NULL,
				  				    hInstance,
				  				  	NULL);

		if(hwndBalloon)
		{	
			// Instantiate the balloon class only if we have
			// a balloon window.  This will avoid problems
			// in the balloon constructor where the mouse hook
			// procedure is created. This hook procedure will try to
			// send messages to hwndBalloon.  In other words, we don't
			// want to implement a mouse hook unless we have a window
			// to send messages to.
			Balloon	*balloon = new Balloon;
			StoreBalloon(hwndBalloon, balloon);
			balloon->hwnd = hwndBalloon;

			if(balloon->Init())
				return(balloon->RegisterClient(hwndClient));
			else
			{
				delete[] balloon;
				return FALSE;
			}
		}
		else
		{
			I_PRINT("Balloon window failed to create.");
			return FALSE;
		}
	}
}


//***************************************************************
//
//	If we have a register, we must have a un-register just for
//	symmetry at least.  Plus, somebody may have a need for this
// at some point. 
//
//	Return Values:
//
//		TRUE  = Window successfully unregistered.
//		FALSE = Did not unregister window.  This is
//				because the handle passed in was not
//				a valid registered window handle.
//
//***************************************************************
extern "C" int APIENTRY Balloon_UnregisterClient(HWND hwndClient)
{
	I_ASSERT(hwndClient);
	if(!hwndClient)
		return FALSE;

	if(hwndBalloon)
	{
		Balloon *balloon = GetBalloon(hwndBalloon);
		return(balloon->UnregisterClient(hwndClient));
	}
	else
	// All clients are unregistered.  There is nothing more
	// to unregister.
	{
		return FALSE;
	}		
}


extern "C" BOOL APIENTRY Balloon_ShutdownServer()
{
	I_ASSERT(hwndBalloon);
	if(!hwndBalloon)
		return FALSE;

	DestroyWindow(hwndBalloon);
	hwndBalloon = NULL;
	return TRUE;
}


//***************************************************************
//
//	If the application or client window has a setting for 
//	turning balloon prompts on and off, this will provide 
// an easy interface into the balloon server to do that.
//
//	This will not unregister any balloon clients that have been 
// registered.
//
//	The default prompt state when the balloon server is started
//	is on.
//
//***************************************************************
extern "C" BOOL APIENTRY Balloon_ActivatePrompts()
{
	I_ASSERT(hwndBalloon);
	if(!hwndBalloon)
		return FALSE;

	Balloon *balloon = GetBalloon(hwndBalloon);

	if(balloon)
		return(balloon->ActivatePrompts());
	else
		return FALSE;
}


extern "C" BOOL APIENTRY Balloon_SuppressPrompts()
{
	I_ASSERT(hwndBalloon);
	if(!hwndBalloon)
		return FALSE;

	Balloon *balloon = GetBalloon(hwndBalloon);

	if(balloon)
		return(balloon->SuppressPrompts());
	else
		return FALSE;
}


extern "C" BOOL APIENTRY Balloon_SetPointSize(int nPtSize, BOOL fUpdate)
{
	I_ASSERT(hwndBalloon);
	if(!hwndBalloon)
		return FALSE;

	Balloon *balloon = GetBalloon(hwndBalloon);

	if(balloon)
		return(balloon->SetPointSize(nPtSize, fUpdate));
	else
		return FALSE;
}


extern "C" BOOL APIENTRY Balloon_SetItalics(BOOL fItalic, BOOL fUpdate)
{
	I_ASSERT(hwndBalloon);
	if(!hwndBalloon)
		return FALSE;

	Balloon *balloon = GetBalloon(hwndBalloon);

	if(balloon)
		return(balloon->SetItalics(fItalic, fUpdate));
	else
		return FALSE;
}


extern "C" BOOL APIENTRY Balloon_SetUnderline(BOOL fUnderline, BOOL fUpdate)
{
	I_ASSERT(hwndBalloon);
	if(!hwndBalloon)
		return FALSE;

	Balloon *balloon = GetBalloon(hwndBalloon);

	if(balloon)
		return(balloon->SetUnderline(fUnderline, fUpdate));
	else
		return FALSE;
}


extern "C" BOOL APIENTRY Balloon_SetFontWeight(int nWeight, BOOL fUpdate)
{
	I_ASSERT(hwndBalloon);
	if(!hwndBalloon)
		return FALSE;

	Balloon *balloon = GetBalloon(hwndBalloon);

	if(balloon)
		return(balloon->SetFontWeight(nWeight, fUpdate));
	else
		return FALSE;
}

extern "C" BOOL APIENTRY Balloon_SetFont(const char *szFaceName, BOOL fUpdate)
{
	I_ASSERT(hwndBalloon);
	if(!hwndBalloon)
		return FALSE;

	Balloon *balloon = GetBalloon(hwndBalloon);

	if(balloon)
		return(balloon->SetFont(szFaceName, fUpdate));
	else
		return FALSE;
}


extern "C" BOOL APIENTRY Balloon_SetTextColor(COLORREF color, BOOL fUpdate)
{	
	I_ASSERT(hwndBalloon);
	if(!hwndBalloon)
		return FALSE;

	Balloon *balloon = GetBalloon(hwndBalloon);

	if(balloon)
		return(balloon->SetTextColor(color, fUpdate));
	else
		return FALSE;
}


// Not enabled yet
extern "C" BOOL APIENTRY Balloon_SetBackgroundColor(COLORREF color, BOOL fUpdate)
{
	I_ASSERT(hwndBalloon);
	if(!hwndBalloon)
		return FALSE;

	Balloon *balloon = GetBalloon(hwndBalloon);

	if(balloon)
		return(balloon->SetBackgroundColor(color, fUpdate));
	else
		return FALSE;
}


extern "C" BOOL APIENTRY Balloon_SetBorderColor(COLORREF color, BOOL fUpdate)
{
	I_ASSERT(hwndBalloon);
	if(!hwndBalloon)
		return FALSE;

	Balloon *balloon = GetBalloon(hwndBalloon);

	if(balloon)
		return(balloon->SetBorderColor(color, fUpdate));
	else
		return(FALSE);
}


extern "C" BOOL APIENTRY Balloon_SetBorderStyle(int nStyle, BOOL fUpdate)
{
	I_ASSERT(hwndBalloon);
	if(!hwndBalloon)
		return FALSE;

	Balloon *balloon = GetBalloon(hwndBalloon);

	if(balloon)
		return(balloon->SetBorderStyle(nStyle, fUpdate));
	else
		return FALSE;
}


extern "C" BOOL APIENTRY Balloon_SetBorderWidth(int nWidth, BOOL fUpdate)
{	
	I_ASSERT(hwndBalloon);
	if(!hwndBalloon)
		return FALSE;

	Balloon *balloon = GetBalloon(hwndBalloon);

	if(balloon)
		return(balloon->SetBorderWidth(nWidth, fUpdate));
	else
		return FALSE;
}


extern "C" BOOL APIENTRY Balloon_SetDelayRate(WORD wRate)
{
	I_ASSERT(hwndBalloon);
	if(!hwndBalloon)
		return FALSE;

	Balloon *balloon = GetBalloon(hwndBalloon);

	if(balloon)
		return(balloon->SetDelayRate(wRate));
	else
		return FALSE;		
}

