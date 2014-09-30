#include "l_includ.h"


//////////////////////////////

LPVOID LIST::operator new(
	size_t 									size, 
	OBJECTID 								id)
	{
	AObjSetDataCount(id, I_NUM_DATAS);
	AObjResizeData(id, I_DATA_STYLE, size);
	return AObjLockData(id, I_DATA_STYLE);
	}

//////////////////////////////

LIST::LIST(
	OBJECTID 								id, 
	LONG 										s)
	{
	m_style = s;
	m_nItems = 0L;
	m_lStringsSize = 0L;
	m_oiSelf = id;
	m_oiText = 0;
	m_curSelection = 0;
	m_version = I_VERSION;

	AObjUnlockData(id, I_DATA_STYLE);
	}

//////////////////////////////

LIST::LIST(	//Used to setup the virtual table
	LIST far *						ptr):	m_attributes(FALSE) //If setting up vTable don't createStuff in attributes
	{
	I_UNUSED(ptr);
	}

LPVOID LIST::operator new(
	size_t									size, 
	LIST far 								*ptr)
	{
	I_UNUSED(size);
	return ptr;
	}

/////////////////////////////////


BOOL LIST::SetDoesMultipleSelections(
	BOOL 										set)
	{
	BOOL										rVal = FALSE;

	if (DoesMultipleSelections() != set)
		{
		if (set)				m_style |= I_LF_MULTISELECT;
		else						m_style &= ~I_LF_MULTISELECT;
		rVal = TRUE;
		}

	return rVal;
	}

//////////////////////////////

BOOL LIST::DoesMultipleSelections()
	{
	return m_style & I_LF_MULTISELECT ? TRUE : FALSE;
	}

//////////////////////////////

OLIST_TYPE LIST::GetType()
	{
	OLIST_TYPE							rVal;

	if (m_style & I_LF_LISTBOX)							rVal = OLIST_LISTBOX;
	else if (m_style & I_LF_SIMPLECOMBO) 		rVal = OLIST_SIMPLECOMBO;
	else if (m_style & I_LF_DROPDOWNCOMBO) 	rVal = OLIST_DROPDOWNCOMBO;
	else if (m_style & I_LF_DROPDOWNLIST) 	rVal = OLIST_DROPDOWNLIST;
	else 																		rVal = OLIST_UNKNOWN;

	return rVal;
	}

//////////////////////////////

BOOL LIST::SetType(
	OLIST_TYPE 							type)
	{
	BOOL										rVal;

	if (GetType() != type)
		{
		m_style &= ~I_LF_LIST_STYLES; 

		switch(type)
			{
			case OLIST_LISTBOX:				m_style |= I_LF_LISTBOX;				break;
			case OLIST_SIMPLECOMBO:		m_style |= I_LF_SIMPLECOMBO;		break;
			case OLIST_DROPDOWNCOMBO:	m_style |= I_LF_DROPDOWNCOMBO;	break;
			case OLIST_DROPDOWNLIST:	m_style |= I_LF_DROPDOWNLIST; 	break;
			}
		rVal = TRUE;
		}
	else
		rVal = FALSE;

	return rVal;
	}

//////////////////////////////

BOOL LIST::addItem(
	LPSTR 									szItemString,
	int 										idxItem,
	int											cbString) //default=-1	DOESN'T include the '\0'
	{
	LPLIST_TABLE 						table;
	LPSTR										szListStrings;
	int											iStringLen;	//doesn't include the '\0'
	BOOL										rVal = FALSE;
	long										lPotentialStringsSize;
	
	iStringLen = (cbString == -1 ? lstrlen(szItemString) : cbString);
																															 	
	lPotentialStringsSize = m_lStringsSize + (LONG)iStringLen + 1L;
	if (lPotentialStringsSize < 65536L)
		{
		m_nItems++;

		SetNumberTableItems(m_oiSelf, m_nItems);

		table = LockListTable(m_oiSelf);
		I_VERIFY_POINTER(table, return FALSE);

		table[m_nItems-1].bIsSelected = FALSE;

		m_lStringsSize = lPotentialStringsSize;

		ResizeListStrings(m_oiSelf, m_lStringsSize);
		szListStrings = LockListStrings(m_oiSelf);

		rVal = InsertListStrings(szItemString, iStringLen, idxItem, m_nItems, table, szListStrings, m_lStringsSize);

		UnlockListTable(m_oiSelf);
		UnlockListStrings(m_oiSelf);
		}

	return rVal;
	}

////////

BOOL LIST::RemoveItem(
	int 										idxItem)
	{
	LPLIST_TABLE 						table;
	LPSTR										szListStrings;
	int											iDeleteItemStrLen;
	BOOL										bSelectionChanged;

	I_ALWAYSASSERT(idxItem < m_nItems, return FALSE) ;

	table = LockListTable(m_oiSelf);
	szListStrings = LockListStrings(m_oiSelf);

	I_ASSERT(m_lStringsSize == SizeofListStrings(m_oiSelf));

	bSelectionChanged = table[idxItem].bIsSelected;

	iDeleteItemStrLen = deleteListString(idxItem, m_nItems, table, szListStrings, m_lStringsSize) ;

	UnlockListTable(m_oiSelf);
	UnlockListStrings(m_oiSelf);

	m_nItems--;
	m_lStringsSize -= iDeleteItemStrLen;

	SetNumberTableItems(m_oiSelf, m_nItems);
	ResizeListStrings(m_oiSelf, m_lStringsSize);

	if (m_nItems < m_curSelection)
		m_curSelection = m_nItems ;

	return bSelectionChanged;	
	}

//////////////////////////////////////

BOOL LIST::RemoveSelectedItems()
	{
	LPLIST_TABLE 						table;
	int											i;
	BOOL										bRemovedSomething=FALSE;

	if (NumItems())
		table = LockListTable(m_oiSelf);

	for (i = NumItems()-1; i >= 0; i--)
		{
		if (table[i].bIsSelected)
			{
			UnlockListTable(m_oiSelf);
			RemoveItem(i);
			bRemovedSomething=TRUE;
			if (NumItems())
				table = LockListTable(m_oiSelf);
			I_VERIFY_POINTER(table, return FALSE);
			}
		}

	if (NumItems())
		UnlockListTable(m_oiSelf);

	if (bRemovedSomething)
		{
		SendSelectionsToText();
		ItemsChanged();
		AEvtPostSignalAtTail(m_oiSelf, I_SIG_SELECTION_CHANGED);
		}

	return TRUE;
	}

/////////////////////////////////////

BOOL LIST::RemoveAllItems()
	{
	BOOL										rVal = FALSE;

	if (m_nItems)
		{
		m_curSelection = 0;
		m_nItems = 0L;
		m_lStringsSize = 0L;
		SetNumberTableItems(m_oiSelf, 0);
		ResizeListStrings(m_oiSelf, 0);
		
		SendSelectionsToText();
		ItemsChanged();
		AEvtPostSignalAtTail(m_oiSelf, I_SIG_SELECTION_CHANGED);

		rVal = TRUE;
		}

	return rVal;
	}


/////////////////////////////////////

BOOL LIST::FillListBox(
	HWND										hWnd, 
	BOOL 										bForceListBox) //Defaults to FALSE. used during edit dialog
	{
	int											topIdx;

	I_ASSERT_WINDOW(hWnd, return FALSE);

	SendMessage(hWnd, WM_SETREDRAW, FALSE, 0);

	if (bForceListBox  ||  GetType() == OLIST_LISTBOX)
		{
		topIdx = (int)SendMessage(hWnd, LB_GETTOPINDEX, 0, 0L);
		SendMessage(hWnd, LB_RESETCONTENT, 0, 0L);
		for (int i = 0; i < m_nItems; i++)	//Insert entrees -- strings are user drawn
			SendMessage(hWnd, LB_ADDSTRING, 0, 0L);
		SendMessage(hWnd, LB_SETTOPINDEX, topIdx, 0L);
		}
	else
		{
		SendMessage(hWnd, CB_RESETCONTENT, 0, 0L);
		for (int i = 0; i < m_nItems; i++)	//Insert entrees -- strings are user drawn
			SendMessage(hWnd, CB_ADDSTRING, 0, 0L) ;
		}

	SendMessage(hWnd, WM_SETREDRAW, TRUE, 0);

	return TRUE;
	}


//////////////////////////////////////////

BOOL LIST::SelectedString(
	LPSTR										szToSelect)
	{
	LPLIST_TABLE 						table;
	LPSTR										szListStrings;
	int											i;
	BOOL										b, bHasChanged=FALSE;

	if (NumItems())
		{
		table = LockListTable(m_oiSelf);
		szListStrings = LockListStrings(m_oiSelf);

		for (i = 0; i < m_nItems; i++)
			{
			b = lstrcmp(GetListString(i, table, szListStrings), szToSelect) ? FALSE : TRUE;
			if (table[i].bIsSelected != b)
				{
				table[i].bIsSelected = b;
				bHasChanged = TRUE;
				}
			}

		UnlockListTable(m_oiSelf);
		UnlockListStrings(m_oiSelf);
		}

	return bHasChanged;
	}

//////////////////////////////////////////

void LIST::SendSelectionsToText()
	{
	LPLIST_TABLE 						table;
	LPSTR										szListStrings, szItem, szText;
	MEMBLOCKID							miText;
	long										lTextLen;
	int											i;
	BOOL										bSomethingSelected;


	if (m_oiText)
		{
		if (m_nItems)
			{
			table = LockListTable(m_oiSelf);
			szListStrings = LockListStrings(m_oiSelf);

			//First pass -- count the number of characters needed for miText
			lTextLen = 1; //null byte
			bSomethingSelected = FALSE;
			for (i = 0; i < m_nItems; i++)
				{
				if (table[i].bIsSelected)
					{
					szItem = GetListString(i, table, szListStrings);
					lTextLen += lstrlen(szItem); //space for the item
					lTextLen += 2; //space for the <CR><LF>
					bSomethingSelected = TRUE;
					}
				}

			//Now, allocate and fill the memblock

			miText = AMemAllocate(lTextLen);
			szText = (LPSTR)AMemLock(miText);

			szText[0] = 0;

			if (bSomethingSelected)
				{
				for (i = 0; i < m_nItems; i++)
					{
					if (table[i].bIsSelected)
						{
						szItem = GetListString(i, table, szListStrings);
						lstrcat(szText, szItem);
						lstrcat(szText, "\r\n");
						}
					}
				szText[lstrlen(szText)-2] = 0; //get rid of the last <CR><LF>
				}

			AMemUnlock(miText);

			OTextSetValueFromMemblock(m_oiText, miText);
			AMemFree(miText);

			UnlockListTable(m_oiSelf);
			UnlockListStrings(m_oiSelf);
			} //if (m_nItems)
		else
			OTextSetValueFromBuffer(m_oiText, "");
		}

	}

//////////////////////

void LIST::ObjectToText(
	MEMBLOCKID 							miText)
	{
	LPLIST_TABLE 						table;
	LPSTR 									szListStrings, szText, szItem;
	long										lSize;
	int											i;

	if (NumItems()) // if the list has any items
	  {
		table = LockListTable(m_oiSelf);
		I_ASSERT_POINTER(table, return);

		szListStrings = LockListStrings(m_oiSelf);
		I_ASSERT_POINTER(szListStrings, UnlockListTable(m_oiSelf);return);

		lSize = table[m_nItems-1].lCharIdx + m_nItems/*for the <LF>*/ - 1 /*cause the last one doesn't get a <LF>*/; 

		AMemSetSize(miText, lSize);
		szText = (LPSTR)AMemLock(miText);
		I_ASSERT_POINTER(szText, UnlockListTable(m_oiSelf);UnlockListStrings(m_oiSelf);return);

		for (i=0; i < m_nItems - 1; i++)
			{	//do all but the last one
			szItem = GetListString(i, table, szListStrings);
			lstrcpy(szText, szItem);
			lstrcat(szText, "\r\n");
			szText += lstrlen(szText);
			}	 
		// Now, the last one
		szItem = GetListString(i, table, szListStrings);
		lstrcpy(szText, szItem);

		UnlockListTable(m_oiSelf);
		UnlockListStrings(m_oiSelf);
		}
	else   // if the list is empty
		{
		AMemSetSize(miText, 1);
		szText = (LPSTR)AMemLock(miText);
		*szText = 0; // null terminate the string
		}

	AMemUnlock(miText);
	}

//////////////////////

void LIST::TextToObject(
	MEMBLOCKID 							miText)
	{
	LPSTR 									szText;
	unsigned short					u, uEnd, uRow;
	BOOL										bSomethingWasSelected;


	bSomethingWasSelected = OListCountSelections(m_oiSelf) ? TRUE:FALSE;

	szText = (LPSTR)AMemLock(miText);
	I_ASSERT_POINTER(szText, return);

	//Empty the current list
	RemoveAllItems();

	for (uEnd = uRow = u = 0; GetNextLine(szText, u, uEnd) && uEnd < 32767; u = uEnd+1, uRow++)
		{
		if (szText[uEnd] == 0)
			{
			addItem(&szText[u], uRow);
			break ;
			}
		else
			{
			szText[uEnd] = 0 ;
			addItem(&szText[u], uRow);
			}
		}
	AMemUnlock(miText);

	if(bSomethingWasSelected)
		{
		SendSelectionsToText();
		AEvtPostSignalAtTail(m_oiSelf, I_SIG_SELECTION_CHANGED);
		}
	ItemsChanged();

	}

//////////////////////

void LIST::ItemsChanged()
	{
	AEvtInfo   							event;

	event.sourceObject = m_oiSelf;
	event.targetObject = m_oiSelf;
	event.when         = 0;
	event.how          = AEvtGetObjectChangedPriority(AEVT_ATMARK | AEVT_KILLALLQUEUES | AEVT_KILLWHAT | AEVT_KILLSOURCE);
	event.what         = AEVENT_OBJECTCHANGED;
	event.details      = NULL;
	event.modifiers    = 0;

	AEvtPost(&event);

	AEvtPostSignalAtTail(m_oiSelf, I_SIG_CHANGED);
	}

//////////////////////

void LIST::AppearanceChanged(
	HWND 										hwnd)
	{
	OWndCallEachVisualInstance(m_oiSelf, ListViAppearanceChanged, (LONG)hwnd, 0L);
	}

//////////////////////

void LIST::EditControlChanged(
	HWND 										hwnd)
	{
	OWndCallEachVisualInstance(m_oiSelf, ListViEditControlChanged, (LONG)hwnd, 0L);
	}

//////////////////////

BOOL LIST::Copy(
	OBJECTID								oiSource)
	{
	LPLIST									sourceList;
	LPLIST_TABLE						sourceTable;
	LPSTR										sourceStrings;
	
	sourceList = LockListObject(oiSource);
	I_VERIFY_POINTER(sourceList, return FALSE);

	m_nItems = sourceList->NumItems();
	m_curSelection = 0;
	m_lStringsSize = sourceList->m_lStringsSize;

	UnlockListObject(oiSource);
	
	////////

	if (NumItems())
		{
		sourceTable = LockListTable(oiSource);
		I_VERIFY_POINTER(sourceTable, return FALSE);

		AObjSetData(m_oiSelf, I_DATA_TABLE, sourceTable, SizeofListTable(oiSource));
		UnlockListTable(oiSource);

		////////
		
		sourceStrings = LockListStrings(oiSource);
		I_VERIFY_POINTER(sourceStrings, return FALSE);

		AObjSetData(m_oiSelf, I_DATA_STRINGS, sourceStrings, SizeofListStrings(oiSource));
		UnlockListStrings(oiSource);
		}

	ItemsChanged();

	return TRUE;
	}

//////////////////////////////////////////////////
////////  UPSF Support   /////////////////////////
//////////////////////////////////////////////////
LPSTR szUpsfVersion 						= "VERSION";
LPSTR szUpsfItemString					= "ITEMSTRG";
LPSTR szUpsfSelectionObject			= "SELCTOBJ";
LPSTR szUpsfMultiSelects				= "MULTSLCT";
LPSTR szUpsfListType						= "TYPE";

static long ExportBool(LPSTR szName, LONG idx, BOOL bValue)
	{
	return AUpsfExportField(szName, idx, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &bValue, sizeof(BOOL));
	}

static long ExportShort(LPSTR szName, LONG idx, short sValue)
	{
	return AUpsfExportField(szName, idx, AUPSF_NATV_SHORT, AUPSF_UNIV_INTEGER, &sValue, sizeof(short));
	}

void LogComment(LPSTR szMember)
	{
	static char							szComment[256];

	wsprintf(szComment, "Error importing \"%s\" -- using default.", szMember);
	AUpsfLogComment(szComment);
	}

///////////////////////

LONG LIST::Export()
	{																															
	LPLIST_TABLE						table;
	LPSTR 									strings, str;
	int											i;

	ExportShort(szUpsfVersion,		0L, m_version);
	AUpsfExportField(szUpsfSelectionObject, 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &m_oiText, sizeof(OBJECTID));
	ExportBool(szUpsfMultiSelects, 0L, DoesMultipleSelections() );
	ExportShort(szUpsfListType, 0L, 
		GetType()==OLIST_SIMPLECOMBO ? 1 :
		GetType()==OLIST_DROPDOWNCOMBO ? 2 :
		GetType()==OLIST_DROPDOWNLIST ? 3 : 0);

	(GetAttributes())->Export();

	if (NumItems())
		{
		table = LockListTable(m_oiSelf);
		I_VERIFY_POINTER(table, return A_ERROR);

		strings = LockListStrings(m_oiSelf);
		I_VERIFY_POINTER(strings, return A_ERROR);

		for(i = 0; i <NumItems(); i++)
			{
			str = GetListString(i, table, strings);
			AUpsfExportField(szUpsfItemString, i, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, str, lstrlen(str));
			}
		UnlockListTable(m_oiSelf);
		UnlockListStrings(m_oiSelf);
		}

	return A_OK ;
	}

///////////////////

LONG LIST::Import()

	{
	BOOL										b;
	short										type;
	long										lNumItems, lItemLength, lNumErrors, i;
	MEMBLOCKID							miItem;
	LPSTR										szItem;

  // clear the list if necessary
	while(NumItems() > 0)
		RemoveItem(0);

	if(AUpsfImportField(szUpsfSelectionObject, 0L, AUPSF_NATV_OBJECTREF, &m_oiText, sizeof(OBJECTID)))
		LogComment(szUpsfSelectionObject);

	if(AUpsfImportField(szUpsfMultiSelects, 0L, AUPSF_NATV_BOOL, &b, sizeof(BOOL)))
		LogComment(szUpsfMultiSelects);
	else
		SetDoesMultipleSelections(b);

	type = 0;
	if(AUpsfImportField(szUpsfListType, 0L, AUPSF_NATV_SHORT, &type, sizeof(short)) != AUPSF_NOERROR)
		LogComment(szUpsfListType);
	switch(type)
		{
		case 0: SetType(OLIST_LISTBOX); break;
		case 1:	SetType(OLIST_SIMPLECOMBO); break;
		case 2: SetType(OLIST_DROPDOWNCOMBO); break;
		case 3: SetType(OLIST_DROPDOWNLIST); break;
		}

	(GetAttributes())->Import();


	// import the strings
	lNumItems = AUpsfFieldCount(szUpsfItemString);
	miItem = AMemAllocate(0L);

	for(lNumErrors=i=0L; i < lNumItems; i++)
		{
		if(AUpsfFieldInfo(szUpsfItemString, i, AUPSF_NATV_STRING, NULL, &lItemLength) == AUPSF_NOERROR)
			{
			AMemSetSize(miItem, lItemLength);
			szItem  = (LPSTR)AMemLock(miItem);
			I_VERIFY_POINTER(szItem, AUpsfLogComment("Error importing the strings.");	break);

			if(AUpsfImportField(szUpsfItemString, i, AUPSF_NATV_STRING, szItem, lItemLength)==AUPSF_NOERROR)
				addItem(szItem, (int) (i-lNumErrors) );
			else
				lNumErrors++;

			AMemUnlock(miItem);
			}
		else
			lNumErrors++;
		}
	AMemFree(miItem);

	if (lNumErrors)
		AUpsfLogComment("Error importing one or more of the strings.");

	return A_OK ;
	}

///////////////

LONG LIST::ValidateObjectIDs(
	pAObjMessage 						theSystem)
	{
	AObjValidateID(&m_oiSelf, theSystem);
	AObjValidateID(&m_oiText, theSystem);
	return A_OK ;
	}

///////////////

LONG LIST::CheckObject()
	{
	if (!AObjCheckType(m_oiText, 0))
		m_oiText = 0;

	return A_OK;
	}

///////////////

LONG LIST::BuildSignals(
	pAObjMessage						theSystem)
	{							 
	MEMBLOCKID							miSignals = (MEMBLOCKID)theSystem->message2;
	AObjSignalRecord far* 	lpSignal;
	BOOL 										bEditable, bClickSignals, bUpDownSignals;
	short										nSignals, i;

	switch(GetType())
		{
		case OLIST_LISTBOX:				bEditable =	FALSE;	bClickSignals = TRUE;	bUpDownSignals = FALSE;	break;
		case OLIST_SIMPLECOMBO:		bEditable =	TRUE;		bClickSignals = TRUE;	bUpDownSignals = FALSE;	break;
		case OLIST_DROPDOWNCOMBO:	bEditable = TRUE;		bClickSignals = FALSE;bUpDownSignals = TRUE;	break;
		case OLIST_DROPDOWNLIST:	bEditable = FALSE;	bClickSignals = FALSE;bUpDownSignals = TRUE;	break;
		case OLIST_UNKNOWN:				return A_ERROR;
		}

	nSignals = I_NUM_BASE_SIGNALS;
	if (bEditable)
		nSignals += 1;
	if (bClickSignals)
		nSignals += 2;
	if (bUpDownSignals)
		nSignals += 2;

	AMemSetSize(miSignals, nSignals*sizeof(AObjSignalRecord)) ;
	
	lpSignal = (AObjSignalRecord far*)AMemLock(miSignals);
	I_VERIFY_POINTER(lpSignal, return A_ERROR);

	i = 0;

	if (bClickSignals)
		{
		lpSignal[i].theID = I_SIG_DOUBLECLICKED;	lstrcpy(lpSignal[i].theName, "Double-click");		i++;
		lpSignal[i].theID = I_SIG_SINGLECLICKED;	lstrcpy(lpSignal[i].theName, "Single-click");		i++;
		}

	lpSignal[i].theID = I_SIG_CHANGED;						lstrcpy(lpSignal[i].theName, "Changed");						i++;
	lpSignal[i].theID = I_SIG_ENTERKEY;						lstrcpy(lpSignal[i].theName, "Enter Key");					i++;
	lpSignal[i].theID = I_SIG_RETURNKEY;					lstrcpy(lpSignal[i].theName, "Return Key");					i++;
	lpSignal[i].theID = I_SIG_ACTIVATED;					lstrcpy(lpSignal[i].theName, "Activated");					i++;
	lpSignal[i].theID = I_SIG_DEACTIVATED;				lstrcpy(lpSignal[i].theName, "Deactivated");				i++;
	lpSignal[i].theID = I_SIG_SELECTION_CHANGED;	lstrcpy(lpSignal[i].theName, "Selection Changed");	i++;

	if (bEditable)
		{
		lpSignal[i].theID = I_SIG_EDITED;						lstrcpy(lpSignal[i].theName, "Edited");		i++;
		}

	if (bUpDownSignals)
		{
		lpSignal[i].theID = I_SIG_DOWN;	lstrcpy(lpSignal[i].theName, "Dropped");	i++;
		lpSignal[i].theID = I_SIG_UP;		lstrcpy(lpSignal[i].theName, "Closed");		i++;
		}

	AMemUnlock(miSignals) ;
	return A_OK ;
	}
