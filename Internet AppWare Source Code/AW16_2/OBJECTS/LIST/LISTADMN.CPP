////////////////////////////////////////////////////////
/////////  List Object Administration Routines    ////////
////////////////////////////////////////////////////////
#include <windows.h>
#include <string.h>
#include <a_alm.h>
#include <op_wnd.h>
#include "attribut.h"
#include "list.h"

int far pascal _export LibMain (HANDLE, WORD, WORD, LPSTR);
LONG Create(OBJECTID oiList) ;
LONG Copy(OBJECTID oiSource, OBJECTID oiDestination) ;
LONG Export(OBJECTID oiList);
LONG Import(OBJECTID oiList);
LONG Read(OBJECTID oiList, pAObjMessage theSystem);
LONG QueryProtocol(ApQueryInfo* pQuery) ;

extern int _WinAllocFlag;
HINSTANCE hInstance ;
FARPROC OldListboxProc ;
FARPROC OldComboboxProc ;
int listPointerOffset ;
int listObjectOffset ;
int comboPointerOffset ;
int comboObjectOffset ;

LONG ALMAPI Administration(OBJECTID oiList, pAObjMessage theSystem)
	{
	switch(theSystem->message1)
		{
		case AOBJ_CREATED:
			return Create(oiList) ;

		case AOBJ_ASSIGNOBJECT:
			return Copy((OBJECTID)theSystem->message3, oiList) ;

		case AOBJ_EXPORTOBJECT:
			return Export(oiList);

		case AOBJ_IMPORTOBJECT:
			return Import(oiList);

		case AOBJ_READ:
			return Read(oiList, theSystem) ;

		case AOBJ_EDITOBJECT:
			return EditListObject(oiList);

		case AOBJ_VALIDATEOBJECTIDS:
			return ValidateIDs(oiList, theSystem) ;

		case AOBJ_CHECKOBJECT:
			return EvaluateObject(oiList) ;

		case AOBJ_BUILDSIGNALS:
			return BuildSignals(oiList, theSystem) ;

		case AOBJ_QUERYPROTOCOL:
			return QueryProtocol((ApQueryInfo*)theSystem->message4) ;
		}
	return A_NOTHANDLED ;
	}

LONG Create(OBJECTID oiList)
	{
	new(oiList, NUM_LIST_DATAS) LIST(oiList);
	AObjUnlockData(oiList, LIST_INFO) ;
	return A_OK ;
	}

LONG Copy(OBJECTID oiSource, OBJECTID oiDestination)
	{
	LPLIST lstSource = (LPLIST) AObjLockData( oiSource, LIST_INFO);
	LPLIST lstDest = (LPLIST) AObjLockData( oiDestination, LIST_INFO);
	lstDest->m_nItems = lstSource->m_nItems;
	lstDest->m_curSelection = 0;
	lstDest->m_stringsSize = lstSource->m_stringsSize;
	AObjUnlockData( oiSource, LIST_INFO);
	AObjUnlockData( oiDestination, LIST_INFO);

	LPLIST_TABLE tableSource = (LPLIST_TABLE) AObjLockData( oiSource, TABLE_INFO);
	LONG tableSize = AObjGetDataSize( oiSource, TABLE_INFO);
	AObjSetData(oiDestination, TABLE_INFO, tableSource, tableSize);
	AObjUnlockData( oiSource, TABLE_INFO);

	LPSTR stringsSource = (LPSTR) AObjLockData( oiSource, STRINGS_INFO);
	LONG stringsSize = AObjGetDataSize( oiSource, STRINGS_INFO);
	AObjSetData(oiDestination, STRINGS_INFO, stringsSource, stringsSize);
	AObjUnlockData( oiSource, STRINGS_INFO);

	PostObjectChanged(oiDestination, LST_ITEMS_CHANGED);
	return A_OK ;
	}

LONG Export(OBJECTID oiList)
	{
	LIST* list = (LIST*)AObjLockData(oiList, LIST_INFO);

	ExportAttributes(&(list->m_attributes));

	LIST_TABLE*	table = (LIST_TABLE*)AObjLockData(oiList, TABLE_INFO);
	LPSTR strings = (LPSTR)AObjLockData(oiList, STRINGS_INFO);
	for(int i = 0; i < list->m_nItems; i++)
		{
		LPSTR string = GetListString(i, table, strings);
		AUpsfExportField("ITEMSTRG", i, AUPSF_NATV_STRING, AUPSF_UNIV_TEXT, string, strlen(string));
		}
	AObjUnlockData(oiList, STRINGS_INFO);
	AObjUnlockData(oiList, TABLE_INFO);

	AUpsfExportField("SELCTOBJ", 0L, AUPSF_NATV_OBJECTREF, AUPSF_UNIV_OBJECTREF, &list->m_oiText, sizeof(OBJECTID));

	BOOL multiSelect = (list->m_style & LF_MULTISELECT) ? TRUE : FALSE;
	AUpsfExportField("MULTSLCT", 0L, AUPSF_NATV_BOOL, AUPSF_UNIV_BOOL, &multiSelect, sizeof(BOOL));

	int	type;
	if (list->m_style & LF_SIMPLECOMBO)
		type = 1;
	else if (list->m_style & LF_DROPDOWNCOMBO)
		type = 2;
	else if (list->m_style & LF_DROPDOWNLIST)
		type = 3;
	else
		type = 0;    //LF_LISTBOX
	AUpsfExportField("TYPE", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &type, sizeof(int));

	AObjUnlockData(oiList, LIST_INFO);
	return A_OK ;
	}

LONG Import(OBJECTID oiList)
	{
	LIST* list = (LIST*)AObjLockData(oiList, LIST_INFO);
	BOOL multiSelect;
	int	type;

	ImportAttributes(&(list->m_attributes));

  // clear the list if necessary
	while(list->m_nItems > 0)
		list->RemoveItem(0);

	// import the strings
	long numItems = AUpsfFieldCount("ITEMSTRG");
	MEMBLOCKID hStrings = AMemAllocate(0L);
	long numErrors = 0L;
	for(int i = 0; i < (int)numItems; i++)
		{
		long textLen;
		if(AUpsfFieldInfo("ITEMSTRG", i, AUPSF_NATV_STRING, NULL, &textLen) == AUPSF_NOERROR)
			{
			AMemSetSize(hStrings, textLen);
			LPSTR string = (LPSTR)AMemLock(hStrings);
			if(!string)
				{
				AUpsfLogComment("Error importing the strings.");
				break;
				}

			if(AUpsfImportField("ITEMSTRG", i, AUPSF_NATV_STRING, string, textLen)==AUPSF_NOERROR)
				list->addItem(string, (int)(i-numErrors));
			else
				{
				numErrors++;
				AUpsfLogComment("Error importing one of the strings.");
				}
			AMemUnlock(hStrings);
			}
		else
			{
			numErrors++;
			AUpsfLogComment("Error importing one of the strings.");
			}
		}
	AMemFree(hStrings);

	if(AUpsfImportField("SELCTOBJ", 0L, AUPSF_NATV_OBJECTREF, &list->m_oiText, sizeof(OBJECTID)))
		AUpsfLogComment("Error importing the text object the list is connected to.");

	if(AUpsfImportField("MULTSLCT", 0L, AUPSF_NATV_BOOL, &multiSelect, sizeof(BOOL)))
		AUpsfLogComment("Error importing the Multi Select flag -- using default.");
	else
		{
		if(multiSelect)
			list->m_style |= LF_MULTISELECT;
		else
			list->m_style &= ~LF_MULTISELECT;
		}

	type = 0;
	if(AUpsfImportField("TYPE", 0L, AUPSF_NATV_INT, &type, sizeof(int)) != AUPSF_NOERROR)
		AUpsfLogComment("Error importing \"TYPE\" -- defaulting to push button.");
	list->m_style &= ~LF_LIST_STYLES;
	switch(type)
		{
		case 0: list->m_style |= LF_LISTBOX; break;
		case 1:	list->m_style |= LF_SIMPLECOMBO; break;
		case 2: list->m_style |= LF_DROPDOWNCOMBO; break;
		case 3: list->m_style |= LF_DROPDOWNLIST; break;
		}
	AObjUnlockData(oiList, LIST_INFO);
	return A_OK ;
	}

LONG Read(OBJECTID oiList, pAObjMessage theSystem)
	{
	LPLIST lstSource = (LPLIST) AObjLockData(oiList, LIST_INFO);
	if (!lstSource)
		{
		AObjReportError(oiList, theSystem->message1, A_ERROR, "List Object\tALM_LIST.DLL", 0);
		AObjUnlockData(oiList, LIST_INFO);
		return A_ERROR ;
		}
	theSystem->message3 = lstSource->m_version;
	if (!(lstSource->m_style & LF_LIST_STYLES))
		lstSource->m_style |= LF_LISTBOX;
	AObjUnlockData(oiList, LIST_INFO);
	return A_OK ;
	}

LONG QueryProtocol(ApQueryInfo* pQuery)
	{
	if (pQuery->protocol == APROT_WND)
		{
		pQuery->pfnCreate = (APPCFN) CreateListVI ;
		return AP_SUPPORTED ;
		}
	return A_NOTHANDLED ;
	}

extern "C" int far pascal LibMain (HINSTANCE hIns, WORD, WORD wHeapSize, LPSTR)
	{
	_WinAllocFlag = GMEM_SHARE;

	hInstance = hIns;
	if ( wHeapSize != 0 )
		UnlockData( 0 ) ;

	WNDCLASS wndclass ;
	GetClassInfo(NULL, "LISTBOX", &wndclass) ;
	listPointerOffset = wndclass.cbWndExtra ;
	listObjectOffset = listPointerOffset + sizeof(void *) ;
	wndclass.cbWndExtra += (sizeof(void *) + sizeof(OBJECTID)) ;

	OldListboxProc = (FARPROC)wndclass.lpfnWndProc ;
	wndclass.lpfnWndProc = NewListboxProc ;

	wndclass.style &= ~CS_GLOBALCLASS ;
	wndclass.hInstance = hInstance ;
	wndclass.lpszClassName = "ALM Listbox" ;
	RegisterClass(&wndclass) ;

	GetClassInfo(NULL, "COMBOBOX", &wndclass) ;
	comboPointerOffset = wndclass.cbWndExtra ;
	comboObjectOffset = comboPointerOffset + sizeof(void *) ;
	wndclass.cbWndExtra += (sizeof(void *) + sizeof(OBJECTID));

	OldComboboxProc = (FARPROC)wndclass.lpfnWndProc ;
	wndclass.lpfnWndProc = NewComboboxProc ;

	wndclass.style &= ~CS_GLOBALCLASS ;
	wndclass.hInstance = hInstance ;
	wndclass.lpszClassName = "ALM Combobox" ;
	RegisterClass(&wndclass) ;

	return TRUE;   // Indicate that the DLL was initialized successfully.
	}

