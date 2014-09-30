

#include "n_includ.h"

NUMBER::NUMBER(
	AOBJECTID								id, 
	BOOL 										bIsTemp):	m_attributes(!bIsTemp) //If temp object don't createStuff in attributes
	{
	m_version = I_THE_VERSION;
  m_oiSelf = id;
	m_style = I_NF_DEFNUMBERSTYLE;
	SetTempObject(bIsTemp);

	if (IsInteger())
		{
		lstrcpy(m_szFormat, g_szIntegerNumberFormat);
		CopyMemory(&m_formatInfo, &g_integerNumberFormatInfo, sizeof(NFORM_INFO));
		}
	else 
		{
		lstrcpy(m_szFormat, g_szRealNumberFormat);
		CopyMemory(&m_formatInfo, &g_realNumberFormatInfo, sizeof(NFORM_INFO));
		}

	m_floatValue = 0.0;
	m_wholeValue = 0;
  FormatNumber();
	}

LPVOID NUMBER::operator new(
	size_t 									size,
	AOBJECTID 								id)
	{
	AObjSetDataCount(id, I_NUM_DATAS);
	AObjResizeData(id, I_DATA_STYLE, size);

	return AObjLockData(id, I_DATA_STYLE);
	}

///////////////////////
NUMBER::NUMBER(	//Unsed to setup the virtual table
	NUMBER  *						ptr):	m_attributes(FALSE) //If setting up vTable don't createStuff in attributes
	{
	I_UNUSED(ptr);
	}

LPVOID NUMBER::operator new(
	size_t									size, 
	NUMBER  							*ptr)
	{
	I_UNUSED(size);
	return ptr;
	}

////////////////////////

void NUMBER::operator delete(
	LPVOID								ptr)
	{
	AObjUnlockData( ((LPNUMBER)ptr)->m_oiSelf, I_DATA_STYLE);
	}

////////////////////////////////////////

void NUMBER::SetRangeError(
	BOOL										set,
	I_POST_TYPE 						fPostSignals)
	{
  AEvtInfo   							event;

	if (set && !(m_style & I_NF_RANGE_ERR) )
		{
		m_style |= I_NF_RANGE_ERR;
		m_wholeValue = 0xffffffffL;
		m_floatValue = HUGE_VAL;
		FormatNumber();
		SetHasChanged(TRUE);
		if (fPostSignals == POST_SIGNALS)
			{
			MaybePostChangedStuff(); //Post the changed stuff, cause if range err set, the value changed

	    //Post the range err signal
			event.sourceObject = event.targetObject = m_oiSelf;
			event.what         = AEVENT_SIGNAL;
			event.details      = I_SIG_RANGEERROR;
			event.modifiers    = 0;
			event.when         = AAppGetTime();
			event.how          = AEvtGetObjectChangedPriority(AEVT_KILLALLQUEUES | AEVT_KILLWHAT | 
																												AEVT_KILLSOURCE | AEVT_KILLDETAILS);
			event.variation		 = 0;
			AEvtPost(&event);
      }
		}

	else if (!set && (m_style & I_NF_RANGE_ERR) )
  	{
		m_style &= ~I_NF_RANGE_ERR;
		if (fPostSignals == POST_SIGNALS)
    	{
      //Kill the range err signals
			event.sourceObject = event.targetObject = m_oiSelf;
			event.what         = AEVENT_SIGNAL;
			event.details      = I_SIG_RANGEERROR;
			event.modifiers    = 0;
			event.when         = AAppGetTime();
			event.how          = AEVT_ATMARK | AEVT_KILLSELF | AEVT_KILLALLQUEUES | AEVT_KILLWHAT | 
																												AEVT_KILLSOURCE | AEVT_KILLDETAILS;
			event.variation		 = 0;
			AEvtPost(&event);
      }
		}

	}

///////////////////

UINT NUMBER::GetDrawTextStyle()
	{
	UINT										drawFlags = DT_NOPREFIX | DT_SINGLELINE;

	if (JustifiesRight())			drawFlags |= DT_RIGHT;
	else											drawFlags |= DT_LEFT;

	return drawFlags;
	}

///////////////////////////////////////////

LONG NUMBER::GetCreateWindowStyle()
	{
	LONG      							s = WS_CHILD | ES_MULTILINE | ES_AUTOHSCROLL;

	if (JustifiesRight())							s |= ES_RIGHT;
	else															s |= ES_LEFT;

	if (HasFrame())										s |= WS_BORDER;

	if (!IsEditable())								s |= ES_READONLY;

	return s;
	}

///////////////////////////

void NUMBER::CopyAttributes(
	LPATTRIBUTES						attTarget)
	{
	CopyMemory(attTarget, &m_attributes, sizeof(ATTRIBUTES));
  }

//////////////////////////////////////////////////////////

long NUMBER::GetWholeValue(
	long 										minVal, //DEFAULT=LONG_MIN 
	long 										maxVal) //DEFAULT=LONG_MAX
	{ 
	long double							fl;

	if (IsReal())
		{
		if (m_floatValue > I_LARGEST_LONG)
			fl = I_LARGEST_LONG;
		else if (m_floatValue < I_SMALLEST_LONG)
			fl = I_SMALLEST_LONG;
		else
			fl = m_floatValue;

		m_wholeValue = (long)(fl>0.0 ? floorl(fl+0.5):ceill(fl-0.5));
		}

	if (m_wholeValue > maxVal) return maxVal; 
	if (m_wholeValue < minVal) return minVal; 

	return m_wholeValue;
	}

//////////////////////////////////////////////////////////

long double NUMBER::GetFloatValue() 
	{
	if (IsInteger())
		m_floatValue = m_wholeValue;
	
	return m_floatValue;
	}

//////////////////////////////////////////////////////////

BOOL NUMBER::SetValue(
	long										lValue,
	I_POST_TYPE 						fPostSignals)
	{
	BOOL										rVal = FALSE;

	if (IsInteger())
		{
		if (m_wholeValue != lValue)
			{
			rVal = TRUE;
			m_wholeValue = lValue;
			}
		}

	else //float number
		{
		if (m_floatValue != (long double) lValue)
			{
			rVal = TRUE;
			m_floatValue = (long double) lValue;
			}
		}

	SetRangeError(FALSE, fPostSignals);

	if (rVal)
		{
		FormatNumber();
		SetHasChanged(TRUE);
		if (fPostSignals == POST_SIGNALS)
			MaybePostChangedStuff();
		}

	if ( (!rVal || fPostSignals == DONT_POST_SIGNALS)  &&  DisplaysEmpty() == BLANK_IF_EMPTY_STRING)
		{ //No signals were posted, but it should not be blank anymore
		SetDisplaysEmpty(DONT_BLANK_IF_EMPTY_STRING);
		OWndCallEachVisualInstance(m_oiSelf, VisualInstanceIsHwnd() ? HwndNumberItemForceRedraw : LiteNumberItemForceRedraw, 0L,	0L);
		}
	else
		SetDisplaysEmpty(DONT_BLANK_IF_EMPTY_STRING);

	return rVal;
	}

//////////////////////////////////////////////////////////

BOOL NUMBER::LocalSetValue(							//Returns TRUE if the value changed
	long double 						flValue,
	I_POST_TYPE 						fPostSignals,
	I_ROUND_TYPE 						fRoundIfInteger)
	{
	BOOL										rVal = FALSE;


	if (IsInteger())
		{
		if (fRoundIfInteger == ROUND_IF_INT)
			flValue = (flValue>0.0 ? floorl(flValue+0.5) : ceill(flValue-0.5));

		if (flValue > I_LARGEST_LONG  ||  flValue < I_SMALLEST_LONG)
			{
			SetRangeError(TRUE, fPostSignals);
			rVal = TRUE;
			}
		else if (m_wholeValue != (long)flValue)
			{
			m_wholeValue = (long)flValue;
			SetRangeError(FALSE, fPostSignals);
			rVal = TRUE;
			}
		}

	else //Its a float number
		{
		if (m_floatValue != flValue)
			{
			m_floatValue = flValue;
			rVal = TRUE;
			}

		//Set/Clear the range error...
		SetRangeError(m_floatValue==HUGE_VAL ? TRUE:FALSE, fPostSignals);
		}


	if (rVal)
		{
		FormatNumber();
		SetHasChanged(TRUE);
		if (fPostSignals == POST_SIGNALS)
			MaybePostChangedStuff();
		}
	
	return rVal;
	}

//////////////////////////////

BOOL NUMBER::SetValue(							//Returns TRUE if the value changed
	long double 						flValue,
	I_POST_TYPE 						fPostSignals,
	I_ROUND_TYPE 						fRoundIfInteger)
	{
	BOOL										rVal;

	rVal = LocalSetValue(flValue, fPostSignals, fRoundIfInteger);

	if ( (!rVal || fPostSignals == DONT_POST_SIGNALS)  &&  DisplaysEmpty() == BLANK_IF_EMPTY_STRING)
		{ //No signals were posted, but it should not be blank anymore
		SetDisplaysEmpty(DONT_BLANK_IF_EMPTY_STRING);
		OWndCallEachVisualInstance(m_oiSelf, VisualInstanceIsHwnd() ? HwndNumberItemForceRedraw : LiteNumberItemForceRedraw, 0L,	0L);
		}
	else
		SetDisplaysEmpty(DONT_BLANK_IF_EMPTY_STRING);

	return rVal;
	}


///////////////////////////////////////

BOOL NUMBER::SetValue(              //Returns TRUE if the value changed
	LPCSTR 				 					str,
	I_POST_TYPE		 					fPostSignals,
	I_CHANGE_TYPE  					fChangeTypeIfPseudo,
	I_BLANK_TYPE 	 					fBlankOutIfEmptyString)
	{
	long double							flValue;
	BOOL										rVal;
	BOOL										bNeedsToCallEachVI;
	I_BLANK_TYPE						fShouldDisplayEmpty;

	flValue = ParseFormatedNumber(str, GetFormatPtr(), GetFormatInfoPtr());

	if (fChangeTypeIfPseudo == CHANGE_IF_PSEUDO  &&  IsTempObject() )
		{
		if (DoesStringContainRealNumberThingies(str) ||  flValue > I_LARGEST_LONG  ||  flValue < I_SMALLEST_LONG)
			SetIsReal();
		else
			SetIsInteger();
		}

	rVal = LocalSetValue(flValue, fPostSignals, ROUND_IF_INT);
	
	fShouldDisplayEmpty = (str[0] == '\0'  &&  fBlankOutIfEmptyString == BLANK_IF_EMPTY_STRING)  ? BLANK_IF_EMPTY_STRING  : DONT_BLANK_IF_EMPTY_STRING;

	if ( (!rVal || fPostSignals==DONT_POST_SIGNALS) && fBlankOutIfEmptyString == BLANK_IF_EMPTY_STRING && fShouldDisplayEmpty != DisplaysEmpty() )
		bNeedsToCallEachVI = TRUE;
	else
		bNeedsToCallEachVI = FALSE;

	SetDisplaysEmpty(fShouldDisplayEmpty);	
	//SetRangeError(FALSE, fPostSignals);

	if (bNeedsToCallEachVI)
		OWndCallEachVisualInstance(m_oiSelf, VisualInstanceIsHwnd() ? HwndNumberItemForceRedraw : LiteNumberItemForceRedraw, 0L,	0L);
		
	return rVal;
	}

///////////////////////////////////////////////

void NUMBER::FormatNumber()
	{

	if (HadRangeError())
		{
		lstrcpy(m_szNumber, "**err**");
		}
	else
		{
		if (IsInteger())
			FormatFloatNumber( (long double)m_wholeValue, GetFormatPtr(), m_szNumber, I_STRING_LEN, GetFormatInfoPtr());
		else
			FormatFloatNumber(m_floatValue, GetFormatPtr(), m_szNumber, I_STRING_LEN, GetFormatInfoPtr());
		}

	}


////////////////////////////////////////////////////
LPSTR NUMBER::GetFormatPtr()
	{
	if (IsTempObject())
		{
		if (IsInteger())
			return g_szIntegerNumberFormat;
		else
			return g_szRealNumberFormat;
		}
	else
		return m_szFormat;
	}
////////////////////////////////////////////////////
LPNFORM_INFO NUMBER::GetFormatInfoPtr() 
	{
	if (IsTempObject())
		{
		if (IsInteger())
			return &g_integerNumberFormatInfo;
		else
			return &g_realNumberFormatInfo;
		}
	else
		return &m_formatInfo;
	}

////////////////////////////////////////////////////
////////////////////////////////////////////////////

long NUMBER::GetType()    //returns ONMBR_INTEGER, ONMBR_REAL
	{

	if (IsInteger())
		return ONMBR_INTEGER;

	else if (IsReal())
		return ONMBR_REAL;

	else
  	return 0L;
	}

////////////////////////////////////////////////////

long NUMBER::GetStatus()  //returns ONMBR_RANGE_ERR, ONMBR_HAS_CHANGED
	{
	long										rVal=0L;

	if (HadRangeError())
		rVal |= ONMBR_RANGE_ERR;

	if (HasChanged())
		rVal |= ONMBR_HAS_CHANGED;


  return rVal;
	}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

void NUMBER::ValidateObjectIDs(
	AOBJECTID								oiNumber)
	{
	m_oiSelf = oiNumber;
	}

////////////////////////////////

void NUMBER::BuildSignals(
	AMEMBLOCKID 							miSignals)
	{
	int											signalNum = 0;
	pAObjSignalRecord				signal;

	AMemSetSize(miSignals, I_NUM_SIGNALS * sizeof(AObjSignalRecord));
	signal = (pAObjSignalRecord)AMemLock(miSignals);

	signal[signalNum].theID = I_SIG_CHANGED;
	lstrcpy(signal[signalNum++].theName, "Changed");

	if (IsEditable())
		{
		signal[signalNum].theID = I_SIG_EDITED;
		lstrcpy(signal[signalNum++].theName, "Edited");
		}

	if (IsActivatable())
		{
		signal[signalNum].theID = I_SIG_ACTIVATED;
		lstrcpy(signal[signalNum++].theName, "Activated");

		signal[signalNum].theID = I_SIG_DEACTIVATED;
		lstrcpy(signal[signalNum++].theName, "Deactivated");
  	}

	signal[signalNum].theID = I_SIG_RANGEERROR;
	lstrcpy(signal[signalNum++].theName, "Range error");


	AMemSetSize(miSignals, signalNum * sizeof(AObjSignalRecord));
	AMemUnlock(miSignals);
	}

//////////////////////////////////////////

long NUMBER::DoVersionCheck()
	{
	long										rVal = A_OK;

	if (m_version == 200)
		{
		m_version = 201;
		if (IsInteger())
			{
			m_floatValue = (m_floatValue>0.0 ? floorl(m_floatValue+0.5) : ceill(m_floatValue-0.5));

			if (m_floatValue > I_LARGEST_LONG  ||  m_floatValue < I_SMALLEST_LONG)
				{
				SetRangeError(TRUE, DONT_POST_SIGNALS);
				rVal = A_WARNING;
				}
			else
				m_wholeValue = (long)m_floatValue;
			}
		}

	return rVal;
	}

//////////////////////////////////////////

BOOL NUMBER::MaybePostChangedStuff()
	{
	AEvtInfo   							event;
  BOOL										rVal;

	if (HasChanged())
		{
		event.sourceObject = event.targetObject = m_oiSelf;
		event.when         = AAppGetTime();
		event.modifiers    = 0L;
		event.variation		 = 0;

		event.what         = AEVENT_OBJECTCHANGED;
		event.details      = 0;
		event.how          = AEvtGetObjectChangedPriority(AEVT_KILLALLQUEUES | AEVT_KILLWHAT | AEVT_KILLSOURCE);
		AEvtPost(&event);		//Post ObjectChanged...

		event.what         = AEVENT_SIGNAL;
		event.details      = I_SIG_CHANGED;
		event.how          = AEvtGetObjectChangedPriority(AEVT_KILLALLQUEUES | AEVT_KILLSOURCE | 
																											AEVT_KILLWHAT | AEVT_KILLDETAILS);
		AEvtPost(&event);		//Posts the changed signal

		SetHasChanged(FALSE);		//Clear the has changed bit
    rVal = TRUE;
		}
	else
		rVal = FALSE;

  return rVal;
	}

/////////////////////////////////
//  UPSF Support    /////////////
/////////////////////////////////
static long ExportBool(
	LPSTR										szName,
	LONG										idx,
	BOOL										bValue)
	{
	return AUpsfExportField(szName, idx, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &bValue, sizeof(BOOL));
  }

static long ExportShort(
	LPSTR										szName,
	LONG										idx,
	short										sValue)
	{
	return AUpsfExportField(szName, idx, AUPSF_NATV_SHORT, AUPSF_UNIV_INTEGER, &sValue, sizeof(short));
  }

void LogComment(
	LPSTR										szMember)
	{
	static char							szComment[256];

	wsprintf(szComment, "Error importing \"%s\" -- using default.", szMember);
	AUpsfLogComment(szComment);
	}

/////////////////////////////////
LPSTR szUpsfVersion				= "VERSION";
LPSTR szUpsfEditable			=	"EDITABLE";
LPSTR szUpsfActivatable			= "ACTIVE";
LPSTR szUpsfFrame					=	"FRAME";
LPSTR szUpsfJustify				=	"JUSTIFY";
LPSTR szUpsfFormat				=	"FORMAT";
LPSTR szUpsfFloat					=	"FLOAT";
LPSTR szUpsfDispEmpty			=	"DSEMPTY";
LPSTR szUpsfValue					=	"VALUE";


/////////////////////////////////

long NUMBER::Export()
	{
	long double							fl;

	fl = GetFloatValue();

	ExportShort(szUpsfVersion, 0L, m_version);
	ExportBool(szUpsfEditable, 0L, IsEditable() );

	if(IsEditable())
		ExportBool(szUpsfActivatable, 0L, TRUE);
	else if (IsActivatable())
		ExportBool(szUpsfActivatable, 0L, TRUE);
	else
		ExportBool(szUpsfActivatable, 0L, FALSE);

	ExportBool(szUpsfFrame, 0L, HasFrame() );
	ExportShort(szUpsfJustify, 0L, (short)(JustifiesLeft() ? 0 : 2) );
	ExportBool(szUpsfFloat, 0L, IsReal());
	ExportBool(szUpsfDispEmpty, 0L, DisplaysEmpty());
	AUpsfExportField(szUpsfFormat, 0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, m_szFormat, I_FORMAT_STRING_LENGTH);
	AUpsfExportField(szUpsfValue, 0L, AUPSF_NATV_LONGDOUBLE, AUPSF_UNIV_NUMBER, &fl, sizeof(long double));
	(GetAttributes())->Export();

	return A_OK;
	}


/////////////////////////////////

long NUMBER::Import()
	{
	BOOL										b;
	short										s;
	long double							fl;

	if(AUpsfImportField(szUpsfEditable, 0L, AUPSF_NATV_BOOL, &b, sizeof(BOOL)) != AUPSF_NOERROR)
		LogComment(szUpsfEditable);
	else
	{
		SetIsEditable(b);
		SetIsActivatable(b);
	}


	if(AUpsfImportField(szUpsfActivatable, 0L, AUPSF_NATV_BOOL, &b, sizeof(BOOL)) != AUPSF_NOERROR)
		LogComment(szUpsfActivatable);
	else
		SetIsActivatable(b);

	if(AUpsfImportField(szUpsfFrame, 0L, AUPSF_NATV_BOOL, &b, sizeof(BOOL)) != AUPSF_NOERROR)
		LogComment(szUpsfFrame);
	else
		SetHasFrame(b);
		
	if(AUpsfImportField(szUpsfJustify, 0L, AUPSF_NATV_SHORT, &s, sizeof(short)) != AUPSF_NOERROR)
		LogComment(szUpsfJustify);
	else
		{
		if (s == 2) SetJustifiesRight();
		else				SetJustifiesLeft();
		}
			
	if(AUpsfImportField(szUpsfFloat, 0L, AUPSF_NATV_BOOL, &b, sizeof(BOOL)) != AUPSF_NOERROR)
		LogComment(szUpsfFloat);
	else
		{
		if (b)	SetIsReal();
		else		SetIsInteger();
		}

	if(AUpsfImportField(szUpsfDispEmpty, 0L, AUPSF_NATV_BOOL, &b, sizeof(BOOL)) != AUPSF_NOERROR)
		LogComment(szUpsfDispEmpty);
	else
		SetDisplaysEmpty(b ? BLANK_IF_EMPTY_STRING : DONT_BLANK_IF_EMPTY_STRING);

	if(AUpsfImportField(szUpsfFormat, 0L, AUPSF_NATV_STRING, m_szFormat, I_FORMAT_STRING_LENGTH) != AUPSF_NOERROR)
		{
		if (IsInteger())
			lstrcpy(m_szFormat, g_szIntegerNumberFormat);
		else 
			lstrcpy(m_szFormat, g_szRealNumberFormat);
		LogComment(szUpsfFormat);
		}
	else
		{//we got a format
///		if (IsInteger()  &&  _fstrchr(GetFormatPtr(), g_chDecSep) ) //we got a format for an integer that has a decimal seperator
		if (IsInteger()  &&  strchr(GetFormatPtr(), g_chDecSep) ) //we got a format for an integer that has a decimal seperator
			lstrcpy(m_szFormat, g_szIntegerNumberFormat);	//it must have come from Workshop, so just use the default
		}
	ValidateNumberFormatString(GetFormatPtr(), GetFormatInfoPtr(), IsInteger()); //Validate the format string to setup m_formatInfo

	if(AUpsfImportField(szUpsfValue, 0L, AUPSF_NATV_LONGDOUBLE, &fl, sizeof(long double)) != AUPSF_NOERROR)
		LogComment(szUpsfValue);	
	else
		SetValue(fl, DONT_POST_SIGNALS, DONT_ROUND_IF_INT);

	(GetAttributes())->Import();

	return A_OK;
	}


