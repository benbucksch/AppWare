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
// FILE:  dt_class.h
//
// AUTHOR: Ksai Liang
//
// DESCRIPTION: Date class definition
//
// CHANGES:
//
//////////////////////////////////////////////////////////////*/
#include "dt_glob.h"


#ifndef DT_CLASS_H
#define DT_CLASS_H

#include <dos.h>

#define VERSION										200

///////////////////////////////////////////////
//   Date Object Signals. DT is date abbrev. //
///////////////////////////////////////////////
#define DT_SignalChanged					0L
#define DT_SignalDayChanged				1L
#define DT_SignalMonthChanged			2L
#define DT_SignalYearChanged			3L
#define DT_SignalActivated   			4L
#define DT_SignalDeactivated   		5L
#define DT_NumSignals   	     	  6L

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
#define DT_FRAME	          0x00000400L
#define DT_EDITABLE   	    0x00000800L
#define DT_KEEPCURRENT 	    0x00001000L
#define DT_SHORTDATE 	      0x00020000L
#define DT_LONGDATE   	    0x00040000L
#define DT_SELECTALL	      0x01000000L
#define DT_TEMPOBJECT		    0x02000000L
#define DT_HASCHANGED		    0x04000000L  //set if the value has changed while editing. if true, objectchanged will be sent on <Enter> or deacti
#define DT_DISPLAYEMPTY		  0x08000000L
#define DT_ACTIVATABLE		  0x10000000L

#define DT_DATETYPE_BITS	  (DT_SHORTDATE | DT_LONGDATE)
#define DT_DATEDEFAULT      (DT_EDITABLE | DT_SELECTALL | DT_SHORTDATE | DT_FRAME | DT_ACTIVATABLE )
//#define DT_DATEHWNSTYLE     (DT_EDITABLE | DT_SELECTALL | DT_ACTIVATABLE | DT_FRAME)
#define DT_DATEHWNSTYLE     (DT_EDITABLE | DT_ACTIVATABLE | DT_FRAME)



///////////////////////////////////////////////
// String length Definition									 //
///////////////////////////////////////////////
#define DT_STRING_LEN			35 // Max. no of the date string

typedef struct date DATE;
typedef struct time TIME;

///////////////////////////////////////////////
// Date Class Definition                     //
///////////////////////////////////////////////
class EditDate;
class Date
{
	protected:
		int 					   m_version;
		char		  			 m_szDate[DT_STRING_LEN];
		OBJECTID  			 m_oiSelf;
		LONG           	 m_style;
		LONG						 m_date;
		ATTRIBUTES     	 m_attributes;    // font, foreground, background

	private:
		Date();
		LPVOID operator new(size_t size) {return NULL;}

		BOOL LocalSetDateValue(long date, BOOL bPostSignals);

	public:
		Date(Date  *ptr);	// setup virtual table
    LPVOID operator new(size_t size, Date  *ptr);

		Date(OBJECTID id, BOOL bIsTemp);                   
		LPVOID operator new(size_t size, OBJECTID id);
		void operator delete(LPVOID	ptr);

		inline BOOL IsEditable()			      {return m_style & DT_EDITABLE ? TRUE:FALSE;}
		inline BOOL IsActivatable()		      {return m_style & (DT_ACTIVATABLE|DT_EDITABLE) ? TRUE:FALSE;}
		inline BOOL SelectsAll()			      {return m_style & DT_SELECTALL ? TRUE:FALSE;}
		inline BOOL DisplaysEmpty()		      {return m_style & DT_DISPLAYEMPTY ? TRUE:FALSE;}
		inline BOOL HasFrame()				      {return m_style & DT_FRAME ? TRUE:FALSE;}
		inline BOOL HasChanged()			      {return m_style & DT_HASCHANGED ? TRUE:FALSE;}
		inline BOOL IsKeepCurrent()		      {return m_style & DT_KEEPCURRENT ? TRUE:FALSE;}
		inline BOOL	IsShortDate()	  	      {return m_style & DT_SHORTDATE ? TRUE:FALSE;}
		inline BOOL	IsLongDate()	  	      {return m_style & DT_LONGDATE ? TRUE:FALSE;}
		inline BOOL	IsTempObject()	        {return m_style & DT_TEMPOBJECT ? TRUE:FALSE;}
		inline BOOL VisualInstanceIsHwnd()  {return m_style & DT_DATEHWNSTYLE ? TRUE:FALSE;}

		inline void SetIsEditable(BOOL set)	      {if (set) m_style |= DT_EDITABLE; else m_style &= ~DT_EDITABLE;}
		inline void SetIsKeepCurrent(BOOL set)    {if (set) m_style |= DT_KEEPCURRENT; else m_style &= ~DT_KEEPCURRENT;}
		inline void SetIsActivatable(BOOL set)    {if (set) m_style |= DT_ACTIVATABLE; else m_style &= ~DT_ACTIVATABLE;}
		inline void SetIsLongDate(BOOL set)	      {if (set) m_style |= DT_LONGDATE; else m_style &= ~DT_LONGDATE;}
		inline void SetIsShortDate(BOOL set)      {if (set) m_style |= DT_SHORTDATE; else m_style &= ~DT_SHORTDATE;}
		inline void SetSelectsAll(BOOL set)		    {if (set) m_style |= DT_SELECTALL; else m_style &= ~DT_SELECTALL;}
		inline void SetDisplaysEmpty(BOOL set)    {if (set) m_style |= DT_DISPLAYEMPTY; else m_style &= ~DT_DISPLAYEMPTY;}
		inline void SetHasFrame(BOOL set)			    {if (set) m_style |= DT_FRAME; else m_style &= ~DT_FRAME;}
		inline void SetHasChanged(BOOL set)		    {if (set) m_style |= DT_HASCHANGED; else m_style &= ~DT_HASCHANGED;}
		inline void SetTempObject(BOOL set)	   	  {if (set) m_style |= DT_TEMPOBJECT; else m_style &= ~DT_TEMPOBJECT;}

    inline const char  *GetDateStringPtr(){return DisplaysEmpty() ? g_szEmpty : m_szDate;}
		inline LPATTRIBUTES GetAttributes()		{return &m_attributes;}
    inline int *GetDateVer() {return &m_version;}

		UINT GetDrawDateStyle();	// Lite Item
		LONG GetCreateWindowStyle();// Windows Item

		BOOL SetDateValue(LPSTR str, BOOL bPostSignals, BOOL bBlankOutIfEmptyString);
		BOOL SetDateValue(long julian, BOOL bPostSignals);
		BOOL SetDateValue(DATE dt, BOOL bPostSignals);

		void PostChangedSignals();
    void CopyAttributes(LPATTRIBUTES attTarget);
		void FormatDate();

		void EditFont(HWND owner = NULL,
									LPSTR str="AaBbYyZz123",
                  LPSTR tstr="Edit Attributes",
									BOOL doColors=TRUE,
									DWORD flags = CF_BOTH|CF_EFFECTS|CF_INITTOLOGFONTSTRUCT|CF_ENABLETEMPLATE|CF_ENABLEHOOK,
									BOOL doBackground=TRUE)	{m_attributes.AttrEditFont(owner, str, tstr, doColors, flags, doBackground);}

    long GetDateValue();
    long GetDateStyle();

		void ValidateObjectIDs(OBJECTID oiNumber);
		void BuildSignals(MEMBLOCKID miSignals);

		BOOL MaybePostChangedStuff(LONG how = AEVT_ATMARK|AEVT_KILLDUPLICATES);

    friend EditDate;

};
typedef Date  * LPDate;
/*****************************************************/


/*****************************************************/
class EditDate:public Date
{
	private:
		HWND	m_hwndDlg;
		LPVOID operator new(size_t size, OBJECTID id) {return NULL;}

	public:
		LPVOID operator new(size_t size) { return :: new char[size];}
		void operator delete(LPVOID ptr){::delete(ptr);}

    inline const char  *GetDateStringPtr(){return m_szDate;}
		inline void SetWindowHandle(HWND hwnd) {m_hwndDlg = hwnd;}

		void EnableProperControls();
		void InitDialog();
		void StoreAobj();		// store the edit object back to the kernal

		EditDate(OBJECTID oi);
		~EditDate();

    static BOOL WINAPI DateEditDlg(HWND, UINT, WPARAM, LPARAM);
};
typedef EditDate  * LPEditDate;
///////////////////////////////////////////////
#endif
