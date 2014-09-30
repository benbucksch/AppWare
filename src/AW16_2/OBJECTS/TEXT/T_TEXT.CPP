
#include "t_includ.h"


TEXT::TEXT(
	OBJECTID								id, 
	BOOL 										bIsTemp):	m_attributes(!bIsTemp) //If temp object don't createStuff in attributes
	{
	m_version = I_THE_VERSION;
  m_oiSelf = id;
	m_style = I_TF_DEFTEXTSTYLE;
	m_limit = 32000; 
	m_horzLimit = 500;
	m_tabSize = 8;
	}

LPVOID TEXT::operator new(
	size_t 									size,
	OBJECTID 								id)
	{
  LPSTR									  str;
	AObjSetDataCount(id, I_NUM_DATAS);
	AObjResizeData(id, I_DATA_STYLE, size);

	//Put an empty string in the second data block
	ResizeTextString(id, 1);
	str = (LPSTR)AObjLockData(id, I_DATA_STRING);
	*str = 0;
	AObjUnlockData(id, I_DATA_STRING);

	return AObjLockData(id, I_DATA_STYLE);
	}

////////////////////////

void TEXT::operator delete(
	LPVOID								ptr)
	{
	AObjUnlockData( ((LPTEXT)ptr)->m_oiSelf, I_DATA_STYLE);
	}

////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////

UINT TEXT::GetDrawTextStyle()
	{
  UINT 															drawFlags = DT_NOPREFIX | DT_EXPANDTABS;


	if (IsMultiline() || IsStatic())	drawFlags |= DT_WORDBREAK;
	else															drawFlags |= DT_SINGLELINE;

	if (JustifiesLeft()) 							drawFlags |= DT_LEFT;
	else if (JustifiesCenter())				drawFlags |= DT_CENTER;
	else															drawFlags |= DT_RIGHT;

	return drawFlags;
	}

///////////////////////////////////////////

LONG TEXT::GetCreateWindowStyle()
	{
	LONG      							s = WS_CHILD;

	if (IsMultiline())
  	{
																	s |= ES_MULTILINE;
																	s |= ES_AUTOVSCROLL;
		if (HasVScroll())      				s |= WS_VSCROLL;
		if (HasHScroll())        			s |= (WS_HSCROLL | ES_AUTOHSCROLL);
		}
	else														s |= ES_AUTOHSCROLL;

	if (JustifiesCenter())  				s |= ES_CENTER;
	else if (JustifiesRight())			s |= ES_RIGHT;
	else														s |= ES_LEFT;

	if (IsUppercase())      				s |= ES_UPPERCASE;
	else if (IsLowercase())  		 		s |= ES_LOWERCASE;

	if (HasFrame())									s |= WS_BORDER;

	if (!IsEditable())							s |= ES_READONLY;

	if (IsPassword())								s |= ES_PASSWORD;

	if (KeepsSelection())						s |= ES_NOHIDESEL;

	return s;
	}

///////////////////////////

void TEXT::CopyAttributes(
	LPATTRIBUTES						attTarget)
	{
	_fmemcpy(attTarget, &m_attributes, sizeof(ATTRIBUTES));
  }

//////////////////////////////////

void TEXT::ValidateObjectIDs(
	OBJECTID 								oiNew)
	{
  m_oiSelf = oiNew;
	}

////////////////////////////////////

void TEXT::BuildSignals(
	MEMBLOCKID 							miSignals)
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

		signal[signalNum].theID = I_SIG_ACTIVATED;
		lstrcpy(signal[signalNum++].theName, "Activated");

		signal[signalNum].theID = I_SIG_DEACTIVATED;
		lstrcpy(signal[signalNum++].theName, "Deactivated");
  	}

	signal[signalNum].theID = I_SIG_LENGTHERROR;
	lstrcpy(signal[signalNum++].theName, "Too Long");

	if (IsEditable())
		{
		signal[signalNum].theID = I_SIG_ENTERKEY;
		lstrcpy(signal[signalNum++].theName, "Enter Key");
   	}

	AMemSetSize(miSignals, signalNum * sizeof(AObjSignalRecord));
	AMemUnlock(miSignals);
	}


///////////////////////////////////////////

#define I_TF_HWND_STYLEBITS_FOR_200  0x7030 //(I_TF_EDITABLE | I_TF_ACTIVATABLE | I_TF_FRAME | I_TF_HORZBAR | I_TF_VERTBAR)

long TEXT::DoVersionCheck()
	{
	long										rVal = A_OK;

	if (m_version == 200)
		{//up it from when we didn't have the static style
		if (!(m_style & I_TF_HWND_STYLEBITS_FOR_200) )
			SetIsStatic();
		else if (!(m_style & I_TF_MULTILINE))
			SetIsSingleline();
		else
			SetIsMultiline();
		m_version = 201;
		rVal = A_MODIFIED;
		}
	else if (m_version > 201)
		rVal = A_ERROR;

	return rVal;
	}


//////////////////////////////////////////
////  UPSF Support      //////////////////
//////////////////////////////////////////

LPSTR szUpsfVersion						= "VERSION";
LPSTR szUpsfSelectAll					= "SLECTALL";
LPSTR szUpsfJustify						= "JUSTIFY";
LPSTR szUpsfCaseLimit					= "CASELIMT";
LPSTR szUpsfEditable					= "EDITABLE";
LPSTR szUpsfVScroll						= "VSCROLL";
LPSTR szUpsfHScroll						= "HSCROLL";
LPSTR szUpsfFrame							= "FRAME";
LPSTR szUpsfSingleline				= "SINGLELINE";
LPSTR szUpsfMultiline					= "MULTILINE";
LPSTR szUpsfStatic						= "STATIC";
LPSTR szUpsfUseEnter					= "USEENTER";
LPSTR szUpsfUseTabs						= "USETABS";
LPSTR szUpsfTabWidth					= "TABWIDTH";
LPSTR szUpsfLimit							= "LIMIT";
LPSTR szUpsfText							= "TEXT";
LPSTR szUpsfKeepsSelection		= "KEEPSEL";
LPSTR szUpsfPassword					= "PASSWORD";

///////////////////////
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

void LogError(
	LPSTR										szMember)
	{
	static char							szComment[256];

	wsprintf(szComment, "Error importing \"%s\" -- using default.", szMember);
	AUpsfLogComment(szComment);
	}

void LogInfo(
	LPSTR										szMember)
	{
	static char							szComment[256];

	wsprintf(szComment, "Info \"%s\" field missing -- using default.", szMember);
	AUpsfLogComment(szComment);
	}


///////////////////////

long TEXT::Export()
	{
	LPSTR					szText;

	ExportShort(szUpsfVersion,				0L, m_version);
	ExportBool(szUpsfSelectAll,				0L, SelectsAll());
	ExportShort(szUpsfJustify,				0L, JustifiesLeft() ? 0 : JustifiesCenter() ? 1 : 2);
	ExportShort(szUpsfCaseLimit,			0L, IsLowercase() ? 2 : IsUppercase() ? 1 : 0);
	ExportBool(szUpsfEditable,				0L, IsEditable());
	ExportBool(szUpsfVScroll,					0L, HasVScroll());
	ExportBool(szUpsfHScroll,					0L, HasHScroll());
	ExportBool(szUpsfFrame,						0L, HasFrame());
	ExportBool(szUpsfSingleline,			0L, IsSingleline() );
	ExportBool(szUpsfMultiline,				0L, IsMultiline() );
	ExportBool(szUpsfStatic,	 				0L, IsStatic() );
	ExportBool(szUpsfUseEnter,				0L, UsesEnter());
	ExportBool(szUpsfUseTabs,					0L, UsesTab());
	ExportShort(szUpsfTabWidth ,			0L, m_tabSize);
	ExportShort(szUpsfLimit,					0L, m_limit);
	ExportBool(szUpsfKeepsSelection,	0L, KeepsSelection() );
	ExportBool(szUpsfPassword,				0L, IsPassword() );
	
	(GetAttributes())->Export();

	szText = LockTextString(m_oiSelf);
	I_ASSERT_STRING(szText, "TEXT::Export", return A_ERROR);

	AUpsfExportField(szUpsfText,	0L, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, szText, lstrlen(szText));

	return A_OK;
	}
									 
///////////////////////

long TEXT::Import()
	{
	BOOL										bool;
	unsigned short					s;
	long										lTextLen;
	LPSTR										szText;

	if(AUpsfImportField(szUpsfSelectAll, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
		LogError(szUpsfSelectAll);
	else
		SetSelectsAll(bool);

	if(AUpsfImportField(szUpsfJustify, 0L, AUPSF_NATV_SHORT, &s, sizeof(unsigned short)) != AUPSF_NOERROR)
		LogError(szUpsfJustify);
	else
		{
		if (s==0)	SetJustifiesLeft();
		else if (s==1) SetJustifiesCenter();
		else SetJustifiesRight();
		}
		
	if(AUpsfImportField(szUpsfCaseLimit, 0L, AUPSF_NATV_SHORT, &s, sizeof(unsigned short)) != AUPSF_NOERROR)
		LogError(szUpsfCaseLimit);
	else
		{
		if (s==2)	SetLowercase();
		else if (s==1) SetUppercase();
		}
		
	if(AUpsfImportField(szUpsfEditable, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
		LogError(szUpsfEditable);
	else
		SetIsEditable(bool);

	if(AUpsfImportField(szUpsfVScroll, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
		LogError(szUpsfVScroll);
	else
		SetHasVScroll(bool);

	if(AUpsfImportField(szUpsfHScroll, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
		LogError(szUpsfHScroll);
	else
		SetHasHScroll(bool);

	if(AUpsfImportField(szUpsfFrame, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
		LogError(szUpsfFrame);
	else
		SetHasFrame(bool);

	//get the control type
	if(AUpsfImportField(szUpsfStatic, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
		{//Don't know if it is static
		LogInfo(szUpsfStatic);
		if (!HasFrame() && !IsEditable() && !HasVScroll())
			SetIsStatic();
		}
	else
		{
		if (bool)
			SetIsStatic();
		}
	if(AUpsfImportField(szUpsfSingleline, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
		LogError(szUpsfSingleline);
	else
		{
		if (bool  &&  !IsStatic()) //cause singleline could be true from the old way
			SetIsSingleline();			//singleline was all we used to export
		}
	if(AUpsfImportField(szUpsfMultiline, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
		LogInfo(szUpsfMultiline);
	else
		{
		if (bool)
			SetIsMultiline();
		}

	if(AUpsfImportField(szUpsfUseEnter, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
		LogError(szUpsfUseEnter);
	else
		SetUsesEnter(bool);

	if(AUpsfImportField(szUpsfUseTabs, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
		LogError(szUpsfUseTabs);
	else
		SetUsesTab(bool);

	if(AUpsfImportField(szUpsfTabWidth, 0L, AUPSF_NATV_SHORT, &m_tabSize, sizeof(unsigned short)) != AUPSF_NOERROR)
		LogError(szUpsfTabWidth);

	if(AUpsfImportField(szUpsfLimit, 0L, AUPSF_NATV_SHORT, &m_limit, sizeof(unsigned short)) != AUPSF_NOERROR)
		LogError(szUpsfLimit);

	if(AUpsfImportField(szUpsfKeepsSelection, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
		LogInfo(szUpsfKeepsSelection);
	else
		SetKeepsSelection(bool);

	if(AUpsfImportField(szUpsfPassword, 0L, AUPSF_NATV_BOOL, &bool, sizeof(BOOL)) != AUPSF_NOERROR)
		LogInfo(szUpsfPassword);
	else
		SetIsPassword(bool);

	(GetAttributes())->Import();

	if(AUpsfFieldInfo(szUpsfText, 0L, AUPSF_NATV_STRING, NULL, &lTextLen) == AUPSF_NOERROR)
		{
		lTextLen = min(lTextLen, m_limit+1L);
		ResizeTextString(m_oiSelf, lTextLen);
		szText = LockTextString(m_oiSelf);
		I_ASSERT_STRING(szText, "TEXT::Import", return A_ERROR);

		*szText = 0; // initialize to nothing.

		// ignore possible errors on importing the text -- text could be too long
		AUpsfImportField(szUpsfText, 0L, AUPSF_NATV_STRING, szText, lTextLen);
		szText[(UINT)lTextLen-1] = 0;
		UnlockTextString(m_oiSelf);
		}
	else
		LogError(szUpsfText);

	return A_OK;
	}
	
////////////////////////////////////////////////////////
/// Printer Support   //////////////////////////////////
////////////////////////////////////////////////////////
#define I_MAR_LEFT		0.5	//margins in inches
#define I_MAR_TOP			0.5
#define I_MAR_RIGHT		0.5
#define I_MAR_BOTTOM	0.5


static BOOL EjectPage(
	HDC										hdc,
	LPSTR									szError)
	{
	BOOL									rVal;

	switch( Escape(hdc, NEWFRAME, NULL, NULL, NULL) )
		{
		case SP_APPABORT:
			strcpy(szError, "Print job was terminated because the application's Abort function returned zero.");
			rVal = FALSE;
			break;

		case SP_OUTOFDISK:
			strcpy(szError, "Not enough disk space is currently available for print spooling.");
			rVal = FALSE;
			break;

		case SP_OUTOFMEMORY:
			strcpy(szError, "Not enough memory is available for print spooling.");
			rVal = FALSE;
			break;

		case SP_USERABORT:
			strcpy(szError, "User terminated the job through Print Manager.");
			rVal = FALSE;
			break;

		case SP_ERROR:
			strcpy(szError, "General printing error.");
			rVal = FALSE;
			break;

		default :
			rVal = TRUE;
			break;
		}
	return rVal;
	}

//////

WORD BreakLine(  //Returns horz the extent of the text
	HDC											hdc,
	LPSTR										str,
	int											tabWidth,
	int											width,
	int far *								ptrNumCharsToPrint,
	int far *								ptrNumCharsToIncBy)
	{
	int											maxNumChars, i;
  DWORD										extent;
	BOOL										bFoundBreak;

	bFoundBreak = FALSE;

	for(i=0; !bFoundBreak && i <= lstrlen(str); i++)
		{
		extent = GetTabbedTextExtent(hdc, str, i+1, 1, &tabWidth);
		if (LOWORD(extent) > width)
			break; //we've gone too far

		if (str[i] == CR  &&  str[i+1] == LF)
			{  //hit a <CR><LF> combo
			bFoundBreak = TRUE;
			*ptrNumCharsToPrint = i;
			*ptrNumCharsToIncBy = i+2;
			}
		else if (str[i] == 0)
			{
			bFoundBreak = TRUE;
			*ptrNumCharsToPrint = i;
			*ptrNumCharsToIncBy = i;
			}
		}

	maxNumChars = i-1;

	//If there was no <CR>LF> in time, backup to break the word.
	for( ; !bFoundBreak && i>0; i--)
		{
		if (isspace(str[i]))
			{
			bFoundBreak = TRUE;
			*ptrNumCharsToIncBy = i+1;
			*ptrNumCharsToPrint = i;
			}
		}

	if (!bFoundBreak)
		*ptrNumCharsToPrint = *ptrNumCharsToIncBy = maxNumChars; 

	extent = GetTabbedTextExtent(hdc, str, *ptrNumCharsToPrint, 1, &tabWidth);

	return LOWORD(extent);
	}

////

void TEXT::Print(
  LPPIDR_BLOCK						pidr)
	{
	LPSTR										szText;
	ATTRIBUTES							attPrinter(&m_attributes);
  POINT										ptDim, ptOffset;
	RECT										rcTemp;
	int											width, height;
	HFONT										hFontOld;
	COLORREF								fore, oldFore, back, oldBack;
  TEXTMETRIC							tm;
  int											yChar, nLinesPerPage;
  char 										szBuffer[128];
	int											currPage, lineNumber;
  LPSTR										chp;
	BOOL										bIsColor;
	int											tabWidth;
	int											xPos, yPos;
	int											extent, numCharsToPrint, numCharsToIncBy;

	szText = LockTextString(m_oiSelf);
	I_ASSERT_STRING(szText, "TEXT::Print", return);

  // set the margins
  if(Escape(pidr->hDC, GETPHYSPAGESIZE, NULL, NULL, &ptDim) > 0) 
  	{
    if(Escape(pidr->hDC, GETPRINTINGOFFSET, NULL, NULL, &ptOffset)>0) 
    	{
      rcTemp.left	  = pidr->DPI.x * I_MAR_LEFT - ptOffset.x;
      rcTemp.top	  = pidr->DPI.y * I_MAR_TOP  - ptOffset.y;
      rcTemp.right  = ptDim.x - pidr->DPI.x* I_MAR_RIGHT  - ptOffset.x;
      rcTemp.bottom = ptDim.y - pidr->DPI.y* I_MAR_BOTTOM - ptOffset.y;

      // reassign the rectangle as necessary
      if(pidr->rect.left   < rcTemp.left)   pidr->rect.left   = rcTemp.left;
      if(pidr->rect.top    < rcTemp.top)    pidr->rect.top    = rcTemp.top;
      if(pidr->rect.right  > rcTemp.right)  pidr->rect.right  = rcTemp.right;
      if(pidr->rect.bottom > rcTemp.bottom) pidr->rect.bottom = rcTemp.bottom;
      }
    }

  width  = pidr->rect.right  - pidr->rect.left;
  height = pidr->rect.bottom - pidr->rect.top;

	//Setup the dc
	attPrinter.AttrCreateFont(pidr->hDC);
	hFontOld = attPrinter.AttrSelectFont(pidr->hDC);

	bIsColor = GetDeviceCaps(pidr->hDC, NUMCOLORS) > 2;
	fore = bIsColor ? attPrinter.AttrGetForeground() : RGB(0,0,0); //black
	back = bIsColor ? attPrinter.AttrGetBackground() : RGB(255,255,255);//white

	oldFore = SetTextColor(pidr->hDC, fore);
	oldBack = SetBkColor(pidr->hDC, back);

  // Get the char height, number of lines per page, & num chars per line
  GetTextMetrics(pidr->hDC, &tm);

  // Get the char height, number of lines per page, & tab width
  yChar = tm.tmHeight + tm.tmExternalLeading;
  nLinesPerPage = (height) / yChar;
	tabWidth = m_tabSize * tm.tmAveCharWidth;

	/////////

	currPage = 0;
	lineNumber = nLinesPerPage + 1;
  chp = szText;

	yPos = pidr->rect.top +tm.tmExternalLeading; 

	while (*chp)
		{
    if(lineNumber > nLinesPerPage) 
    	{
      currPage++;
			yPos = pidr->rect.top +tm.tmExternalLeading; 

      // quit if all the requested pages have been printed
      if(!pidr->numPages && currPage > pidr->toPage)
      	break;

      // eject page and update window if printing
			if(!pidr->numPages &&
				 currPage>pidr->fromPage && currPage<=pidr->toPage)
				{
				if (!EjectPage(pidr->hDC, pidr->error))
					break; //failed

				// re-initialize the font and colors
				attPrinter.AttrSelectFont(pidr->hDC);
				SetTextColor(pidr->hDC, fore);
				SetBkColor(pidr->hDC, back);
				}

			// update window
			if(!pidr->numPages) 
				{
				if(currPage >= pidr->fromPage  &&  currPage <= pidr->toPage)
					wsprintf(szBuffer, "Printing page #%i.", currPage);
				else
					wsprintf(szBuffer, "Preparing Print Job.");
				SendMessage(pidr->hMsgWnd, WM_SETTEXT, 0, (long)szBuffer);
				}

			// reset line number for next Page
      lineNumber=1;
      }

	 extent = BreakLine(pidr->hDC, chp, tabWidth, width, &numCharsToPrint, &numCharsToIncBy);
	 if(!pidr->numPages)
	 	{
		if (JustifiesLeft())
			xPos = pidr->rect.left;
		else if (JustifiesRight())
			xPos = pidr->rect.left + width - extent;
		else //center
		  xPos = pidr->rect.left + (width - extent)/2;

   TabbedTextOut(pidr->hDC, xPos, yPos, chp, numCharsToPrint, 1, &tabWidth, xPos);
		}
	 chp += numCharsToIncBy;
   yPos += yChar;
   lineNumber++;
	 }

	//Done
	EjectPage(pidr->hDC, pidr->error);

	// restore font and colors to what they originally were
	SetBkColor(pidr->hDC, oldBack);
	SetTextColor(pidr->hDC, oldFore);
	SelectObject(pidr->hDC, hFontOld);
	attPrinter.AttrDeleteFont();

  if(pidr->numPages) 
  	{
    // return number of possible pages to print in toPage
    pidr->toPage = currPage;
    }

	UnlockTextString(m_oiSelf);
	}


