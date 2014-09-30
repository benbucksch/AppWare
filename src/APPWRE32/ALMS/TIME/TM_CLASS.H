/*//////////////////////////////////////////////////////////////
//
//	Copyright (c) Unpublished Work of Novell, Inc.  All rights reserved.
//
//	THIS WORK IS AN UNPUBLISHED WORK AND CONTAINS CONFIDENTIAL,
//	PROPRIETARY, AND TRADE SECRET INFORMATION OF NOVELL, INC.
//	ACCESS TO THIS WORK IS RESTRICTED TO (I) NOVELL, INC. EMPLOYEES
//	WHO HAVE A NEED TO KNOW HOW TO PERFORM THEIR TASKS WITHIN THE SCOPE
//	OF THEIR ASSIGNMENTS AND (II) ENTITIES OTHER THAN NOVELL, INC. WHO
//	HAVE ENTERED INTO APPROPRIATE LICENSE AGREEMENTS.  NO PART OF THIS
//	WORK MAY BE USED, PRACTICED, PERFORMED, COPIED, DISTRIBUTED,
//	REVISED, MODIFIED, TRANSLATED, ABRIDGED, CONDENSED, EXPANDED,
//	COLLECTED, COMPILED, LINKED, RECAST, TRANSFORMED, OR ADAPTED
//	WITHOUT THE PRIOR WRITTEN CONSENT OF NOVELL, INC.  ANY USE OR
//	EXPLOITATION OF THIS WORK WITHOUT AUTHORIZATION COULD SUBJECT
//	THE PERPETRATOR TO CRIMINAL AND CIVIL LIABILITY.
//
//--------------------------------------------------------------
//
// FILE:  tm_class.h
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION: Time class definition
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#ifndef TM_CLASS_H
#define TM_CLASS_H

#include <dos.h>
#include "tm_glob.h"
          
#define VERSION										200

///////////////////////////////////////////////
//   Date Object Signals. TM is time abbrev. //
///////////////////////////////////////////////
#define TM_SignalChanged					0L
#define TM_SignalSecondChanged  	1L
#define TM_SignalMinuteChanged		2L
#define TM_SignalHourChanged			3L
#define TM_SignalActivated   			4L
#define TM_SignalDeactivated   		5L
#define TM_NumSignals   	     	  6L

#define TICKS_PER_DAY		 5184000L
#define TICKS_PER_HOUR	 216000L
#define TICKS_PER_MIN		 3600

///////////////////////////////////////////////
// Data block index defines                  //
///////////////////////////////////////////////
#define ZeroBase									0L

#define NumData										1L
////////////////////////////////////////////
//  Date style flags                      //
////////////////////////////////////////////
#define TM_FRAME	          0x00000400L
#define TM_EDITABLE   	    0x00000800L
#define TM_KEEPCURRENT 	    0x00001000L
#define TM_SELECTALL	      0x01000000L
#define TM_SECONDS          0x00100000L
#define TM_24HOURS          0x00200000L
#define TM_DEFHOUR          0x00400000L
#define TM_TEMPOBJECT		    0x02000000L
#define TM_HASCHANGED		    0x04000000L  //set if the value has changed while editing. if true, objectchanged will be sent on <Enter> or deacti
#define TM_DISPLAYEMPTY		  0x08000000L
#define TM_ACTIVATABLE		  0x10000000L

#define TM_TIMETYPE_BITS	  (TM_24HOUR | TM_DEFHOUR)
#define TM_TIMEDEFAULT      (TM_EDITABLE | TM_SELECTALL | TM_SECONDS | TM_FRAME)

///////////////////////////////////////////////
// String length Definition									 //
///////////////////////////////////////////////
#define TM_STRING_LEN			35 // Max. no of the date string


// This stuff isn't in dos.h anymore.  Why ????  3/22/96  MRG
struct  time    {
    unsigned char   ti_min;     // Minutes 
    unsigned char   ti_hour;    // Hours 
    unsigned char   ti_hund;    // Hundredths of seconds 
    unsigned char   ti_sec;     // Seconds 
};
// Where did this go to ?


struct time_l
{
    long   ti_min;     /* Minutes */
    long   ti_hour;    /* Hours */
    long   ti_hund;    /* Hundredths of seconds */
    long   ti_sec;     /* Seconds */
};


typedef struct time_l TIME;
typedef struct time   TIME_DOS;

///////////////////////////////////////////////
// Time Class Definition                     //
///////////////////////////////////////////////
class EditTime;
class Time
{
	protected:
		int 					   m_version;
		char		  			 m_szTime[TM_STRING_LEN];
		AOBJECTID  			 m_oiSelf;
		LONG           	 m_style;
		LONG						 m_time;
		ATTRIBUTES     	 m_attributes;    // font, foreground, background

	private:
		Time();
		LPVOID operator new(size_t size) {return NULL;}

    BOOL LocalSetTimeValue(long time, BOOL	bPostSignals);
	public:
		Time(Time  *ptr);	// setup virtual table
    LPVOID operator new(size_t size, Time  *ptr);

		Time(AOBJECTID id, BOOL bIsTemp);                   
		LPVOID operator new(size_t size, AOBJECTID id);
		void operator delete(LPVOID	ptr);

		inline BOOL IsEditable()			      {return m_style & TM_EDITABLE ? TRUE:FALSE;}
		inline BOOL IsActivatable()		      {return m_style & (TM_ACTIVATABLE|TM_EDITABLE) ? TRUE:FALSE;}
		inline BOOL SelectsAll()			      {return m_style & TM_SELECTALL ? TRUE:FALSE;}
		inline BOOL DisplaysEmpty()		      {return m_style & TM_DISPLAYEMPTY ? TRUE:FALSE;}
		inline BOOL HasFrame()				      {return m_style & TM_FRAME ? TRUE:FALSE;}
    inline BOOL HasSeconds()				    {return m_style & TM_SECONDS ? TRUE:FALSE;}
		inline BOOL HasChanged()			      {return m_style & TM_HASCHANGED ? TRUE:FALSE;}
 		inline BOOL Is24Hour()		          {return m_style & TM_24HOURS ? TRUE:FALSE;}
 		inline BOOL IsDefHour()		          {return m_style & TM_DEFHOUR ? TRUE:FALSE;}
		inline BOOL IsKeepCurrent()		      {return m_style & TM_KEEPCURRENT ? TRUE:FALSE;}
		inline BOOL	IsTempObject()	        {return m_style & TM_TEMPOBJECT ? TRUE:FALSE;}
		inline BOOL VisualInstanceIsHwnd()  { return m_style & TM_TIMEDEFAULT ? TRUE:FALSE;}

 		inline void SetIs24Hour(BOOL set)         {if (set) m_style |= TM_24HOURS; else m_style &= ~TM_24HOURS;}
 		inline void SetIs12Hour(BOOL set)         {if (set) m_style |= 0x00000000; else m_style &= ~0x00000000;}
 		inline void SetIsDefHour(BOOL set)        {if (set) m_style |= TM_DEFHOUR; else m_style &= ~TM_DEFHOUR;}

		inline void SetIsEditable(BOOL set)	      {if (set) m_style |= TM_EDITABLE; else m_style &= ~TM_EDITABLE;}
		inline void SetIsKeepCurrent(BOOL set)    {if (set) m_style |= TM_KEEPCURRENT; else m_style &= ~TM_KEEPCURRENT;}
		inline void SetIsActivatable(BOOL set)    {if (set) m_style |= TM_ACTIVATABLE; else m_style &= ~TM_ACTIVATABLE;}
		inline void SetSelectsAll(BOOL set)		    {if (set) m_style |= TM_SELECTALL; else m_style &= ~TM_SELECTALL;}
		inline void SetDisplaysEmpty(BOOL set)    {if (set) m_style |= TM_DISPLAYEMPTY; else m_style &= ~TM_DISPLAYEMPTY;}
		inline void SetHasFrame(BOOL set)			    {if (set) m_style |= TM_FRAME; else m_style &= ~TM_FRAME;}
		inline void SetSecond(BOOL set) 			    {if (set) m_style |= TM_SECONDS; else m_style &= ~TM_SECONDS;}
		inline void SetHasChanged(BOOL set)		    {if (set) m_style |= TM_HASCHANGED; else m_style &= ~TM_HASCHANGED;}
		inline void SetTempObject(BOOL set)	   	  {if (set) m_style |= TM_TEMPOBJECT; else m_style &= ~TM_TEMPOBJECT;}

		UINT GetDrawTimeStyle();	// Lite Item
		LONG GetCreateWindowStyle();// Windows Item

		void CopyAttributes(LPATTRIBUTES attTarget);

		BOOL SetTimeValue(LPSTR time, BOOL bPostSignals, BOOL bBlankOutIfEmptyString);
		BOOL SetTimeValue(long  time, BOOL bPostSignals);

		void PostChangedSignals();

		void EditFont(HWND owner = NULL,
									LPSTR str="AaBbYyZz123",
                  LPSTR tstr="Edit Attributes",
									BOOL doColors=TRUE,
									DWORD flags = CF_BOTH|CF_EFFECTS|CF_INITTOLOGFONTSTRUCT|CF_ENABLETEMPLATE|CF_ENABLEHOOK,
		
									BOOL doBackground=TRUE)	{m_attributes.AttrEditFont(owner, str, tstr, doColors, flags, doBackground);}

    void FormatTM();

		long GetType();     
    long GetTimeValue();
    long GetTimeStyle();

		void ValidateObjectIDs(AOBJECTID oiNumber);
		void BuildSignals(AMEMBLOCKID miSignals);

    inline const char  *GetTimeStringPtr(){return DisplaysEmpty() ? g_szEmpty : m_szTime;}
		inline LPATTRIBUTES GetAttributes()		{return &m_attributes;}
    inline int *GetTimeVer() {return &m_version;}

		BOOL MaybePostChangedStuff(LONG how = AEVT_ATMARK|AEVT_KILLDUPLICATES);

    friend EditTime;

};
typedef Time  * LPTime;
/*****************************************************/


/*****************************************************/
class EditTime:public Time
{
	private:
		HWND	m_hwndDlg;

	private:
		LPVOID operator new(size_t size, AOBJECTID id) {return NULL;}

	public:
		LPVOID operator new(size_t size) { return :: new char[size];}
		void operator delete(LPVOID ptr){::delete(ptr);}

		EditTime(AOBJECTID oi);
		~EditTime();

    inline const char  *GetTimeStringPtr() {return m_szTime;}
		inline void SetWindowHandle(HWND hwnd) {m_hwndDlg = hwnd;}

		void EnableProperControls();
		void InitDialog();
		void StoreAobj();		// store the edit object back to the kernal

    static BOOL WINAPI TimeEditDlg(HWND, UINT, WPARAM, LPARAM);
};
typedef EditTime  * LPEditTime;
///////////////////////////////////////////////


#endif
