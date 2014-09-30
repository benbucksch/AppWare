//IMPORTANT ASSUMPTION:
//
//Any routine called with a current context must not be called with a NULL context

#include "almsys.h"
#include <windowsx.h>

#include "srchmgr.h"
#include "progsys.h"
#include "subject.h"
#include "project.h"
#include "objinst.h"
#include "funcinst.h"
#include "resconst.h"
#include "objdesc.h"
#include "bitree.h"
#include "intdebug.h"
#include "objgroup.h"
#include "comment.h"
#include "rectypes.h"
#include "param.h"
#include "basewin.h"
#include "mdiclass.h"
#include "stdlib.h"
#include "apbutil.h"
#include "oiputils.h"
#include "a_almutl.h"

//search types
#define SRCHTYPE_NONE									0x00000000L
#define SRCHTYPE_OBJECT 							0x00000001L
#define SRCHTYPE_FUNCTION 						0x00000002L
#define SRCHTYPE_TYPE									0x00000004L
#define SRCHTYPE_GROUP								0x00000008L
#define SRCHTYPE_PARAMETER						0x00000010L
#define SRCHTYPE_COMMENT							0x00000020L

#define SRCHOPTION_CASE								0x00000100L
#define SRCHOPTION_WHOLEWORD					0x00000200L
#define SRCHOPTION_FOUNDLIST					0x00000400L

#define SRCHSTATE_CANSEARCH	 					0x00010000L

#define SRCHIN_ACTIVESUBJECT					0x00100000L
#define SRCHIN_PROJECT								0x00200000L
#define SRCHIN_CHOSENSUBJECT					0x00400000L

#define SRCHFLAG_NEEDSUBJECTLABEL			0x01000000L
#define SRCHFLAG_FOUNDLISTOPEN				0x02000000L
#define SRCHFLAG_FINDALL							0x04000000L


//Search Types Check Boxes IDs
#define SRCHID_OBJECTS		1000
#define SRCHID_TYPES			1002
#define SRCHID_GROUPS			1004
#define SRCHID_PARAMETERS	1006
#define SRCHID_FUNCTIONS	1008
#define SRCHID_COMMENTS		1010

//Search Range	
#define	SRCHID_RANGE			2000

//Search Options
#define SRCHID_CASE						3000
#define SRCHID_WHOLEWORD			3002
#define SRCHID_FOUNDLIST			3004

//Search Buttons
#define SRCH_OK				IDOK
#define SRCH_CANCEL   IDCANCEL
#define SRCH_CLOSE		4000
#define SRCH_HELP			4002

//index of what should be the first valid item in the found list
#define SRCH_FIRSTVALIDITEM 2

//Range constants	- these should all be negative numbers
//so as not to conflict with the 0 based ranged indexes
//ALSO, make sure that negative values do not conflict with
//subject UID's.
#define SRCHRANGE_PROJECT					-4L
#define SRCHRANGE_ACTIVESUBJECT		-5L

#define	SRCH_PROJECTTEXT				("[Current Project]")
#define SRCH_ACTIVESUBJECTTEXT 	("[Active Subject]")

#define SRCH_IGNORECASE(x) AnsiUpper(x)

#define SRCHCTRL_DISPLAYLIST 100 //listbox control ID 

#define SRCHFORMAT_SUBJECT	0
#define SRCHFORMAT_TYPE			1
#define SRCHFORMAT_ITEM			2

//data constants for what is strored by the listbox to identity
//type and subject entries.  These must be small negatives so as not
//to conflict with the subject uid's
#define SRCHITEM_SUBJECT		-4L
#define SRCHITEM_TYPE				-5L

//style of found list listbox.
#define SRCHFOUNDLIST_LISTSTYLE (WS_CHILD | WS_VISIBLE | 	\
																LBS_HASSTRINGS | LBS_NOTIFY \
																| LBS_USETABSTOPS | WS_VSCROLL)

//style of found list owner window
#define SRCHFOUNDLIST_OWNERSTYLE  (WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS)

class FOUNDITEM:public BITREEMEMBER
	{
	public:

		long uid;
		TSTRING name;

		virtual int Compare(
			LPBITREEMEMBER node)
			{
			return lstrcmp(name, 
				((FOUNDITEM *)node)->name);
			
			}

	};

typedef FOUNDITEM * LPFOUNDITEM;

BOOL								SEARCHMANAGER::sm_registered = FALSE;
char								SEARCHMANAGER::sm_className[] = {"SRCH_APPBUILDER_FINDWINDOW"};
char  							SEARCHMANAGER::sm_endMessageTitle[32] = {""};
char								SEARCHMANAGER::sm_notFoundMessage[]=
											{"No matching items were found."};
char  							SEARCHMANAGER::sm_endMessage[]=
											{"End of Search Reached.  Start from beginning?"};
char 								SEARCHMANAGER::sm_cantFindMessage[] = 
											{"Can't find selected item."};
POINT								SEARCHMANAGER::sm_foundListOrigin;
int									SEARCHMANAGER::sm_widthFoundList;
int									SEARCHMANAGER::sm_heightFoundList;

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANGER::SEARCHMANAGER
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
SEARCHMANAGER::SEARCHMANAGER()
	{	
	m_srchProject=NULL;
	m_srchSubject=NULLSERUID;
	m_subjectIndex = 0;
	m_indexIndex=0;
	m_indexSize=0;
	m_hwndDisplayList = NULL;
	m_hwndOwnerList = NULL;
	m_currentType = 0;
	m_searchContext = NULL;

	if(!sm_registered)
		Register();

	//set end message title text
	if(!sm_endMessageTitle[0])
		LoadString(PROGSYSTEM::hInst, STR_FRAMENAME, sm_endMessageTitle, 32);

	ReadProfileInfo();
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::~SEARCHMANAGER
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
SEARCHMANAGER::~SEARCHMANAGER()
	{
	//Make sure status shows no display list as showing because
	//it's too much trouble to worry about that case.
	//Also, the chosen subject may not be valid during the next
	//running, so just clear the flag to save the trouble of having
	//to worry about this case.
	m_searchStatus &= ~(SRCHFLAG_FOUNDLISTOPEN | SRCHIN_CHOSENSUBJECT);

	WriteProfileInfo();
	} 

																			 

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANGER::FindDlgProc
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL CALLBACK SEARCHMANAGER::FindDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	switch(message)
		{
		case WM_INITDIALOG:
			CenterDialog(hDlg, PROGSYSTEM::hWndFrame, FALSE);
			((LPSEARCHMANAGER)lParam)->InitFindDialog(hDlg);
			break;

		case WM_COMMAND: 
			{
			WORD		wID		= GET_WM_COMMAND_ID(wParam, lParam); 

		 	switch(wID)
				{
				case SRCH_OK:
					//passing in TRUE means to always reset the search indexes.
					if(((LPSEARCHMANAGER)GetWindowLong(hDlg, DWL_USER))->
						GetSearchStateInfo(hDlg, TRUE))
						EndDialog(hDlg, TRUE);

					break;

				case SRCH_CLOSE:
					((LPSEARCHMANAGER)GetWindowLong(hDlg, DWL_USER))
						->GetSearchStateInfo(hDlg);
					//end with false because we don't want to do a search
					EndDialog(hDlg, FALSE);
					break;   

				case SRCH_CANCEL:
					EndDialog(hDlg, FALSE);
					break;
				}
			break; //break out of WM_COMMAND case 
			}

		}
	return 0;
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::ShowFindDialog
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL SEARCHMANAGER::ShowFindDialog()
	{
	int l_rval;

	l_rval = DialogBoxParam(PROGSYSTEM::hInst, "FINDDIALOG",
		GetActiveWindow(), (DLGPROC)SEARCHMANAGER::FindDlgProc,
			(long)this);

	if(l_rval)
		Search();

	return l_rval;
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::Search
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:				TRUE if a match is found
/////////////////////////////////////////////////////////////
BOOL SEARCHMANAGER::Search()
	{
	BOOL l_rval;

	apb_ShowWaitCursor();

	if(m_searchStatus & SRCHOPTION_FOUNDLIST)
		{
		CreateFindWindow();

		//make sure the user can't see stuff being added. If the window
		//is invisible, the following line won't hurt.
		SendMessage(m_hwndDisplayList, WM_SETREDRAW, FALSE, 0L);

		m_searchStatus &= ~SRCHFLAG_NEEDSUBJECTLABEL;

		m_displaySortTree = new BITREE();

		I_ASSERT(m_displaySortTree);
		}
	
	l_rval = DoSearch();

	if(m_searchStatus & SRCHOPTION_FOUNDLIST)
		{
		SendMessage(m_hwndDisplayList, WM_SETREDRAW, TRUE, 0L);
		
		if(l_rval)
			{
			//cause the sort tree to purge itself and add itself to the display
			//window if it contains nodes.
			DisplaySortTree();

			//if the window was already displayed, make sure everything redraws.
			//If the window was invisible, the following lines won't hurt
			//anything.
			InvalidateRect(m_hwndDisplayList, NULL, TRUE);

			ShowWindow(m_hwndOwnerList, SW_SHOWNORMAL);

			BringWindowToTop(m_hwndOwnerList);

			//needed incase the window was already showing and therefore has
			//its client area will need updating.
			UpdateWindow(m_hwndDisplayList);

			//sort of a kluge to get the find window to appear under the
			//windows menu - it doesn't initially because the find window
			//is created invisible.
			SendMessage(PROGSYSTEM::hWndClient, WM_MDISETMENU, TRUE, 0L);  

			m_searchStatus |= SRCHFLAG_FOUNDLISTOPEN;
			}
		else
			{
			MessageBox(NULL,
				sm_notFoundMessage, sm_endMessageTitle,
				MB_OK|MB_TASKMODAL|MB_ICONINFORMATION);

			//if the window is invisible still, destroy it since we don't need
			//it hanging around.
			if(!(m_searchStatus & SRCHFLAG_FOUNDLISTOPEN))
				{
				SendMessage(PROGSYSTEM::hWndClient, WM_MDIDESTROY,
					(WPARAM) m_hwndOwnerList, 0L);
				}
			else
				{
				//all items were probably removed so casue the window to 
				//redraw.
				InvalidateRect(m_hwndDisplayList, NULL, TRUE);
				UpdateWindow(m_hwndDisplayList);
				}
			}

		delete m_displaySortTree;

		//some extra clean up
		m_searchStatus &= ~SRCHFLAG_NEEDSUBJECTLABEL;
		}

	apb_RestoreOldCursor();

	return l_rval;
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::DoSearch
//						 
// PARAMETERS:
//
// DESCRIPTION:		Performs the actual search
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL SEARCHMANAGER::DoSearch()
	{
	LPSUBJECT		   	l_subjectPtr;
	BOOL						l_rval = FALSE; //signal not found.
	char						l_pattern[SRCH_MAXSTRING];
	char						l_itemName[SRCH_MAXSTRING];
	void *				l_item;

	//Exit if we can't search
	if(!(m_searchStatus & SRCHSTATE_CANSEARCH))
		{
		MessageBeep(0);
		}
	else
		{
		//make sure all indexes are set appropriately;
		if(!SetSearchParameters())
			{
			m_searchStatus &= ~SRCHSTATE_CANSEARCH;
			}
		else
			{
			l_subjectPtr = (LPSUBJECT)GetObject(m_srchSubject);

			ReportSubject(l_subjectPtr);
			
			//set the pattern
			lstrcpy(l_pattern, m_searchString);

			//ignore case if necessary by converting the string to uppercase			
			if(!(m_searchStatus & SRCHOPTION_CASE))
				SRCH_IGNORECASE(l_pattern);
			
			while(TRUE) //loop until we break out by force
				{
				l_item = GetNextSearchItem(l_itemName);

				if(l_item)
					{
					//set up the buffer
					if(!(m_searchStatus & SRCHFLAG_FINDALL))
						{
						if(!(m_searchStatus & SRCHOPTION_CASE))
							SRCH_IGNORECASE(l_itemName);
						}

					if((m_searchStatus & SRCHFLAG_FINDALL) || 
						(PatternMatch(l_itemName, l_pattern,
						(BOOL)(m_searchStatus & SRCHOPTION_WHOLEWORD))))
						{
						//see if we were in the middle of a double search for the 
						//type or object name.
						if(m_indexArray[m_indexIndex].indexType ==
							(SRCHTYPE_OBJECT | SRCHTYPE_TYPE))
							{
							//if we just found the type, then we need to reset the
							//paramIndex and increment the index.
							if(m_indexArray[m_indexIndex].paramIndex != 0)
								{
								m_indexArray[m_indexIndex].paramIndex = 0;
								m_indexArray[m_indexIndex].index++;
								}
							}

						ReportFoundItem(l_item);

						//match found so return TRUE;
						l_rval = TRUE;

						//leave search loop if we're doing a singular search.
						if(!(m_searchStatus & SRCHOPTION_FOUNDLIST))
							break;
						}

					}	//end if(l_item)
				
				//the following else block of code needs to remain in the 
				//while loop because it contains numerous continue and break
				//statements.
				else //adjust indexes if necessary to continue search
					{
					//first check to see if we can go to the next item in
					//the indexArray
					if(m_indexIndex < m_indexSize)
						{
						m_indexIndex++;
						continue; //continue search
						}
					else if(m_searchStatus & SRCHIN_PROJECT)
						{
						//the subject index should already be incremented to point
						//to the next subject.
						l_subjectPtr = (LPSUBJECT)GetObject(
							m_srchProject->GetSubjectBlock(), m_subjectIndex);

						if(l_subjectPtr)
							{
							m_srchSubject = GetUID(l_subjectPtr);
							ResetIndexArray();
							ReportSubject(l_subjectPtr);
							continue; //continue the search.
							}
						}

					//end of search reached.
					if(!(m_searchStatus & SRCHOPTION_FOUNDLIST) &&
						MessageBox(GetActiveWindow(),
						sm_endMessage, sm_endMessageTitle,
						MB_YESNO|MB_TASKMODAL|MB_ICONINFORMATION) == IDYES)
						{
						if(m_searchStatus & SRCHIN_PROJECT)
							{
							m_subjectIndex = 0;
							l_subjectPtr = (LPSUBJECT)GetObject(
								m_srchProject->GetSubjectBlock(), m_subjectIndex);

							I_ASSERT(l_subjectPtr);

							m_srchSubject = GetUID(l_subjectPtr);
							ReportSubject(l_subjectPtr);
							}
						ResetIndexArray();
						}
					else
						break;//break out of search loop

					}	// end else

				} //end while loop

			}	 //end else for if(!SetSearchParameters)

		}	//end else for if(!(m_searchStatus & SRCHSTATE_CANNOTSEARCH))
		
	return l_rval;	
	} 

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANGER::GetNextSearchItem
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void * SEARCHMANAGER::GetNextSearchItem(
	LPSTR itemName)
	{
	void * l_itemPtr = NULL;
	
	if(m_indexIndex < m_indexSize)
		{
		I_ASSERT(m_indexArray[m_indexIndex].srchBlock != NULLSERUID);

		//report if we're beginning the search of a new type
		if((m_indexArray[m_indexIndex].index == 0) &&
			(m_indexArray[m_indexIndex].paramIndex == 0))
			ReportType(m_indexArray[m_indexIndex].indexType);

		if(m_indexArray[m_indexIndex].indexType == SRCHTYPE_FUNCTION)
			{
			l_itemPtr = GetNextFunction(itemName);
			}

		else if(m_indexArray[m_indexIndex].indexType & 
			(SRCHTYPE_TYPE | SRCHTYPE_OBJECT))
			{
			l_itemPtr = GetNextObject(itemName);
			}
							
		else if(m_indexArray[m_indexIndex].indexType == (SRCHTYPE_GROUP))
			{
			l_itemPtr = GetNextGroup(itemName);
			}

		else if(m_indexArray[m_indexIndex].indexType & SRCHTYPE_PARAMETER)
			{
			l_itemPtr = GetNextParameter(itemName);
			}
		else if(m_indexArray[m_indexIndex].indexType == SRCHTYPE_COMMENT)
			{
			l_itemPtr = GetNextComment(itemName);
			}
		}	//end m_indexIndex < m_indexSize

	return l_itemPtr;
	} 

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANGER::GetNextFunction
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:				return a pointer to the next function
//								If a function is found, the name will be
//								copied into the itemName buffer.
/////////////////////////////////////////////////////////////
void * SEARCHMANAGER::GetNextFunction	(
	LPSTR itemName)
	{
	LPFUNCINST l_funcPtr;

	l_funcPtr = (LPFUNCINST)GetObject(
		m_indexArray[m_indexIndex].srchBlock,
		m_indexArray[m_indexIndex].index);

	if(l_funcPtr)
		lstrcpy(itemName, l_funcPtr->GetDescPtr()->pGetName());

	return l_funcPtr;
	} 

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::GetNextObject
//						 
// PARAMETERS:
//
// DESCRIPTION:		if the search is a dual search for both
//								objects and types, m_indexIndex will not
//								be incremented. It is the responsibility of
//								whichever routine calls pattern match to
//								increment the index if a match is found
//								for the object.  This routine must also
//								reset the paramIndex since it is incremented
//								to 1 in the case of a dual search.  A value of
//								1 means that the next time through, we'll search the type.
//
// RETURNS:
/////////////////////////////////////////////////////////////
void * SEARCHMANAGER::GetNextObject(
	LPSTR itemName)
	{
	LPOBJINST l_objPtr;

	l_objPtr = (LPOBJINST)GetObject(
		m_indexArray[m_indexIndex].srchBlock,
		m_indexArray[m_indexIndex].index);

	if(l_objPtr)
		{
		if(m_indexArray[m_indexIndex].indexType ==
			(SRCHTYPE_OBJECT | SRCHTYPE_TYPE))
			{
			//CONVENTION: if the paramIndex is 0, we need to get the
			//type name.  Otherwise, get the object name.
			if(m_indexArray[m_indexIndex].paramIndex == 0)
				{
				//get the type name
				lstrcpy(itemName, l_objPtr->objDesc->pGetName());

				//Decrement the index so we can search the same object's
				//name next time through the loop.
				m_indexArray[m_indexIndex].index--;

				//signal to get the object name next
				m_indexArray[m_indexIndex].paramIndex = 1;
				}
			else
				lstrcpy(itemName, l_objPtr->GetName());

			}
		else if(m_indexArray[m_indexIndex].indexType == SRCHTYPE_TYPE)
			lstrcpy(itemName, l_objPtr->objDesc->pGetName());
		else //looking for the object's name
			lstrcpy(itemName, l_objPtr->GetName());							
		}

	return l_objPtr;
	} 

																			 																			 																			 
/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::GetNextGroup
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void * SEARCHMANAGER::GetNextGroup(
	LPSTR itemName)
	{
	LPOBJGROUP l_groupPtr;

	l_groupPtr = (LPOBJGROUP)GetObject(
		m_indexArray[m_indexIndex].srchBlock,
		m_indexArray[m_indexIndex].index);

	if(l_groupPtr)
		lstrcpy(itemName, l_groupPtr->GetName());

	return l_groupPtr;
	} 

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::GetNextParameter
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void * SEARCHMANAGER::GetNextParameter(
	LPSTR itemName)
	{
	LPPARAM l_paramPtr = NULL;
	LPFUNCINST l_funcPtr;

	
	while(0 != (l_funcPtr = (FUNCINST *)GetObject(
		m_indexArray[m_indexIndex].srchBlock,
		m_indexArray[m_indexIndex].index)))
		{
		if(NULL != (l_paramPtr = l_funcPtr->GetNextParam(
			&(m_indexArray[m_indexIndex].paramIndex))))
			{
			break;
			}
		else
			{
			//set to zero so we can check the next function
			m_indexArray[m_indexIndex].paramIndex = 0;
			}
		}

	if(l_paramPtr)
		{
		//decrement the index since still need to search
		//this same function for its parameters the next
		//time through the loop
		m_indexArray[m_indexIndex].index--;

		lstrcpy(itemName, l_paramPtr->GetName());
		}

	return l_paramPtr;
	} 

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::GetNextComment
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void * SEARCHMANAGER::GetNextComment(
	LPSTR itemName)
	{
	LPCOMMENT l_commentPtr;

	l_commentPtr = (LPCOMMENT)GetObject(
		m_indexArray[m_indexIndex].srchBlock,
		m_indexArray[m_indexIndex].index);

	if(l_commentPtr)
		lstrcpy(itemName, l_commentPtr->briefComment);
	
	return l_commentPtr;
	} 

																			 																			 																			 																			 

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::PatternMatch
//						 
// PARAMETERS:
//
// DESCRIPTION:		Whole words must begin with a space or a tab and
//								end with a null terminator or a space or a tab.
//
// RETURNS:
/////////////////////////////////////////////////////////////																			 
BOOL SEARCHMANAGER::PatternMatch(
	LPSTR string,
	LPCSTR pattern,
	BOOL wholeWordMatch)
	{
	int l_sLength;
	int l_pLength;
	
	//For Rabin-Karp
	int l_i;
	long l_dM, l_h1, l_h2;
	int l_match;
	char l_strHold;

	l_sLength = lstrlen(string);
	l_pLength = lstrlen(pattern);


	if(l_pLength > l_sLength)
		{
		//pattern too long so we can't match
		l_match = FALSE;
		}
	else if(l_pLength == l_sLength)
		{
		//same length so just do a string compare
		l_match = (lstrcmp(string, pattern))?FALSE:TRUE;
		}
	else
		{
		//do Robin-Karp search in Algorithms in C, Robert Sedgewick, Addison-
		//Wesley Publishing 1990, p289-290

		#define q 3554393L 	//this is a different prime than the book has.
												//The book has 33554393 but when that prime was
												//multiplied by the alphabet size, the result was
												//too large.
		#define d 128
		l_dM=1;
		l_h1=0; //pattern hash value
		l_h2=0; //string hash value for first pLength characters of the string
		l_match = FALSE;

		//l_dM = (alphabet size to the (pattern size - 1) power) mod q
		for(l_i=1; l_i < l_pLength; l_i++)
			l_dM = (d * l_dM) % q;

		//calculate initial hash values for pattern and for string
		for(l_i=0; l_i < l_pLength; l_i++)
			{
			l_h1 = (l_h1 * d + pattern[l_i])%q;
			l_h2 = (l_h2 * d + string[l_i])%q;
			}

		if(l_h2 == l_h1)
			{
			//now do an actual strcmp to make sure the pattern was actually
			//found.

			//if we're trying to match a whole word, make sure that
			//the match is a whole word.
			if((!wholeWordMatch) || (string[l_pLength] == '\0' ||
				string[l_pLength] == ' ' || string[l_pLength] == '\t'))
				{
				//temporarily terminate the string;
				l_strHold = string[l_pLength];

				string[l_pLength] = '\0';
				if(!lstrcmp(pattern, string))
					l_match = TRUE;
				string[l_pLength] = l_strHold;
				}
			}

		//do search until a match is found
		for(l_i=0; !l_match; l_i++)
			{
			//first subtract the leftmost value - first add in an extra d*q
			//to make sure everything stays positive.
			l_h2 = (l_h2 + (d*q) - (string[l_i] * l_dM)) %q;
			//now add in i+pLength member
			l_h2 = ((l_h2*d) + string[l_i + l_pLength]) % q;
			if(l_h2 == l_h1)
				{
				//now do an actual strcmp to make sure the pattern was actually
				//found.

				//if we're trying to match a whole word, make sure that
				//the match is a whole word.
				if((!wholeWordMatch) || 
					((l_i == 0 ||
					string[l_i] == ' ' ||
					string[l_i] == '\t') && 
					(string[l_i + l_pLength + 1] == '\0' ||
					string[l_i + l_pLength + 1] == ' ' || 
					string[l_i + l_pLength + 1] == '\t')))
					{
					//temporarily terminate the string;
					l_strHold = string[l_i + l_pLength + 1];
					string[l_i + l_pLength + 1] = '\0';
					if(!lstrcmp(pattern, (string + l_i + 1)))
						{
						//we matched
						l_match = TRUE;
						}
					string[l_i + l_pLength + 1] = l_strHold;
					}
				}
			//see if we're at the end of the string and if so break;
			if(l_i > l_sLength - l_pLength)
				break;
			}
		}

	return l_match;
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::InitFindDialog
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
////////////////////////////////////////////////////////////
void SEARCHMANAGER::InitFindDialog(
	HWND hDlg)
	{

	//set the dialog extra bytes to be the this ptr
	SetWindowLong(hDlg, DWL_USER, (long)this);

	InitSearchTypes(hDlg);

	InitSearchOptions(hDlg);

	InitSearchRange(hDlg);

	Edit_SetText(GetDlgItem(hDlg, SRCH_SEARCHTEXT), m_searchString);
	} 

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::InitSearchTypes
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void SEARCHMANAGER::InitSearchTypes(
	HWND hDlg)
	{
	HWND		hTypes		= GetDlgItem(hDlg, SRCHID_TYPES);
	HWND		hObjects	= GetDlgItem(hDlg, SRCHID_OBJECTS);
	HWND		hFuncs		= GetDlgItem(hDlg, SRCHID_FUNCTIONS);
	HWND		hGroups		= GetDlgItem(hDlg, SRCHID_GROUPS);
	HWND		hComments	= GetDlgItem(hDlg, SRCHID_COMMENTS);
	HWND		hParams		= GetDlgItem(hDlg, SRCHID_PARAMETERS);
	BOOL defaultType = TRUE; 	//deafault to needing to set the type
														//to the default type.

	//init the search types
	if(m_searchStatus & SRCHTYPE_TYPE)
		{
		Button_SetCheck(hTypes, 1);
		defaultType = FALSE;
		}
	else
		Button_SetCheck(hTypes, 0);

	if(m_searchStatus & SRCHTYPE_OBJECT)
		{
		Button_SetCheck(hObjects, 1);
		defaultType = FALSE;
		}
	else
		Button_SetCheck(hObjects, 0);

	if(m_searchStatus & SRCHTYPE_FUNCTION)
		{
		Button_SetCheck(hFuncs, 1);
		defaultType = FALSE;
		}
	else
		Button_SetCheck(hFuncs, 0);

	if(m_searchStatus & SRCHTYPE_GROUP)
		{
		Button_SetCheck(hGroups, 1);
		defaultType = FALSE;
		}
	else
		Button_SetCheck(hGroups, 0);

	if(m_searchStatus & SRCHTYPE_COMMENT)
		{
		Button_SetCheck(hComments, 1);
		defaultType = FALSE;
		}
	else
		Button_SetCheck(hComments, 0);

	if(m_searchStatus & SRCHTYPE_PARAMETER)
		{
		Button_SetCheck(hParams, 1);
		defaultType = FALSE;
		}
	else
		Button_SetCheck(hParams, 0);

	if(defaultType)
		Button_SetCheck(hObjects, 1);

	}


/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::InitSearchOptions
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void SEARCHMANAGER::InitSearchOptions(
	HWND hDlg)
	{
	HWND		hCase		= GetDlgItem(hDlg, SRCHID_CASE);
	HWND		hWord		= GetDlgItem(hDlg, SRCHID_WHOLEWORD);
	HWND		hList		= GetDlgItem(hDlg, SRCHID_FOUNDLIST);
																			 
	if(m_searchStatus & SRCHOPTION_CASE)
		Button_SetCheck(hCase, 1);
	else
		Button_SetCheck(hCase, 0);

	if(m_searchStatus & SRCHOPTION_WHOLEWORD)
		Button_SetCheck(hWord, 1);
	else
		Button_SetCheck(hWord, 0);

	if(m_searchStatus & SRCHOPTION_FOUNDLIST)
		Button_SetCheck(hList, 1);
	else
		Button_SetCheck(hList, 0);
	
	} 

																			 
/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::InitSearchRange
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void SEARCHMANAGER::InitSearchRange(
	HWND hDlg)
	{
	HWND			hRange		= GetDlgItem(hDlg, SRCHID_RANGE);
	LRESULT l_result;
	int l_i;
	LPPROJECT l_projectPtr;
	LPSUBJECT l_subjectPtr;
	RUNTIMEUID l_subjectBlock;
	long l_itemCount = 0;
	long l_data;
		
	//fill the range with the names of the subjects listed
	//in alphabetical order.
	l_projectPtr = PROGSYSTEM::GetCurrentProject();
	l_subjectBlock = l_projectPtr->GetSubjectBlock();
	
	l_i=0;
	while(0 != (l_subjectPtr = (LPSUBJECT)GetObject(l_subjectBlock, l_i)))
		{
		l_result = ComboBox_AddString(hRange, (LPSTR) l_subjectPtr->GetName());

		I_ASSERT(l_result != CB_ERR);

		ComboBox_SetItemData(hRange, l_result, GetUID(l_subjectPtr).id);
		
		l_itemCount++;
		}

	//now insert the ACTIVE SUBJECt and CURRENT PROJECT entries as the
	//first two entries.
	l_result =ComboBox_InsertString(hRange, 0, SRCH_PROJECTTEXT);
		
	ComboBox_SetItemData(hRange, l_result, SRCHRANGE_PROJECT);

	l_result = ComboBox_InsertString(hRange, 0, SRCH_ACTIVESUBJECTTEXT);
		
	ComboBox_SetItemData(hRange, l_result, SRCHRANGE_ACTIVESUBJECT);
		
	
	l_itemCount += 2;


	//highlight a range entry if one was previously selected.
	l_data = 0; //default to no range selected.
		
	if(m_searchStatus & SRCHIN_PROJECT)
		l_data = SRCHRANGE_PROJECT;
	else if(m_searchStatus & SRCHIN_ACTIVESUBJECT)
		l_data = SRCHRANGE_ACTIVESUBJECT;

	//we can only use the chosen subject if the search contexts are
	//the same.
	else if(m_searchContext &&
		(m_searchContext->GetFileIDStamp() == pTheSystem->GetFileObject()->GetFileIDStamp()) &&
		(m_searchStatus & SRCHIN_CHOSENSUBJECT))
		l_data = m_srchSubject.id;

	if(l_data)
		{
		for(l_i = 0; l_i < l_itemCount; l_i++)
			{
			if(l_data == ComboBox_GetItemData(hRange, l_i))
				{
				ComboBox_SetCurSel(hRange, l_i);
				break;
				}
			}				
		}

	if(!l_data)
		{
		//highlight the top most entry as a default
		ComboBox_SetCurSel(hRange, 0);
		}	
	} 

																			 																				 

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::GetSearchStateInfo
//						 
// PARAMETERS: 	reset is optional and defaults to FALSE.
//							if reset is TRUE, the indexes are always
//							reset even if no data is modified.
//
// DESCRIPTION:
//
// RETURNS:		 TRUE if it's OK to search
/////////////////////////////////////////////////////////////
BOOL SEARCHMANAGER::GetSearchStateInfo(
	HWND hDlg,
	BOOL reset)
	{
	char l_buffer[SRCH_MAXSTRING+1];
	BOOL l_searchOK = FALSE;
	BOOL l_modified = FALSE;
			
	if(GetSearchOptions(hDlg))
		l_modified = TRUE;

	if(GetSearchType(hDlg, (BOOL *)&l_searchOK))
		l_modified = TRUE;

	if(l_searchOK)
		{
		if(GetSearchRange(hDlg, (BOOL *)&l_searchOK))
			l_modified = TRUE;
		}
	
	if(l_searchOK)
		{
		//set up the search string
		Edit_GetText(GetDlgItem(hDlg, SRCH_SEARCHTEXT), l_buffer,
			SRCH_MAXSTRING);

		if(lstrcmp(m_searchString, l_buffer))
			{
			lstrcpy(m_searchString, l_buffer);
			l_modified = TRUE;
			}

		if(IsFindAllCode(m_searchString))
			m_searchStatus |= SRCHFLAG_FINDALL;
		else
			m_searchStatus &= ~SRCHFLAG_FINDALL;

		m_searchStatus |= SRCHSTATE_CANSEARCH;

		if(l_modified || reset)
			RebuildIndexArray();

		}
	else
		m_searchStatus &= ~SRCHSTATE_CANSEARCH;
	
	return (m_searchStatus & SRCHSTATE_CANSEARCH)?TRUE:FALSE;
	} 

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::RebuildIndexArray
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void SEARCHMANAGER::RebuildIndexArray()
	{

	m_indexIndex = 0;
	m_indexSize = 0;
	
	//combine the object and type search into one search.
	if(m_searchStatus & (SRCHTYPE_OBJECT | SRCHTYPE_TYPE))
		{
		m_indexArray[m_indexIndex].index = 0;
		m_indexArray[m_indexIndex].indexType = m_searchStatus &
			(SRCHTYPE_OBJECT | SRCHTYPE_TYPE);
		m_indexSize++;
		m_indexIndex++;
		}

	if(m_searchStatus & SRCHTYPE_GROUP)
		{
		m_indexArray[m_indexIndex].index = 0;
		m_indexArray[m_indexIndex].indexType = SRCHTYPE_GROUP;
		m_indexSize++;
		m_indexIndex++;
		}

	if(m_searchStatus & SRCHTYPE_PARAMETER)
		{
		m_indexArray[m_indexIndex].index = 0;
		m_indexArray[m_indexIndex].indexType = SRCHTYPE_PARAMETER;
		m_indexSize++;
		m_indexIndex++;
		}

	if(m_searchStatus & SRCHTYPE_FUNCTION)
		{
		m_indexArray[m_indexIndex].index = 0;
		m_indexArray[m_indexIndex].indexType = SRCHTYPE_FUNCTION;
		m_indexSize++;
		m_indexIndex++;
		}

	if(m_searchStatus & SRCHTYPE_COMMENT)
		{
		m_indexArray[m_indexIndex].index = 0;
		m_indexArray[m_indexIndex].indexType = SRCHTYPE_COMMENT;
		m_indexSize++;
		m_indexIndex++;
		}

	m_indexIndex = 0; //reset to point to first element.
	
	//this will force all info to set itself when the search routine
	//is entered.
	m_searchContext = NULL;			
		
	} 

																		 
/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::GetSearchOptions
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:			 
/////////////////////////////////////////////////////////////
BOOL SEARCHMANAGER::GetSearchOptions(
	HWND hDlg)
	{
	long		bOnOptions		= 0x0;
	long		bOffOptions		= 0x0;
	BOOL		l_modified;

	I_ASSERT(IsWindow(hDlg));

	if(Button_GetCheck(GetDlgItem(hDlg, SRCHID_CASE)))
		bOnOptions |= SRCHOPTION_CASE;
	else
		bOffOptions |= SRCHOPTION_CASE;

	if(Button_GetCheck(GetDlgItem(hDlg, SRCHID_WHOLEWORD)))
		bOnOptions |= SRCHOPTION_WHOLEWORD;
	else
		bOffOptions |= SRCHOPTION_WHOLEWORD;

	if(Button_GetCheck(GetDlgItem(hDlg, SRCHID_FOUNDLIST)))
		bOnOptions |= SRCHOPTION_FOUNDLIST;
	else
		bOffOptions |= SRCHOPTION_FOUNDLIST;


	l_modified = ((m_searchStatus ^ bOnOptions) != 0x0);

	m_searchStatus &= ~bOffOptions;
	m_searchStatus |= bOnOptions;

	return l_modified;
	} 

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::GetSearchType
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL SEARCHMANAGER::GetSearchType(
	HWND hDlg,
	BOOL * searchOK)
	{
	long		bOnOptions		= 0x0;
	long		bOffOptions		= 0x0;
	BOOL		l_modified;
	
	I_ASSERT(IsWindow(hDlg));

	*searchOK = FALSE;

	if(Button_GetCheck(GetDlgItem(hDlg, SRCHID_TYPES)))
		bOnOptions |= SRCHTYPE_TYPE;
	else
		bOffOptions |= SRCHTYPE_TYPE;

	if(Button_GetCheck(GetDlgItem(hDlg, SRCHID_OBJECTS)))
		bOnOptions |= SRCHTYPE_OBJECT;
	else
		bOffOptions |= SRCHTYPE_OBJECT;

	if(Button_GetCheck(GetDlgItem(hDlg, SRCHID_FUNCTIONS)))
		bOnOptions |= SRCHTYPE_FUNCTION;
	else
		bOffOptions |= SRCHTYPE_FUNCTION;

	if(Button_GetCheck(GetDlgItem(hDlg, SRCHID_PARAMETERS)))
		bOnOptions |= SRCHTYPE_PARAMETER;
	else
		bOffOptions |= SRCHTYPE_PARAMETER;

	if(Button_GetCheck(GetDlgItem(hDlg, SRCHID_GROUPS)))
		bOnOptions |= SRCHTYPE_GROUP;
	else
		bOffOptions |= SRCHTYPE_GROUP;

	if(Button_GetCheck(GetDlgItem(hDlg, SRCHID_COMMENTS)))
		bOnOptions |= SRCHTYPE_COMMENT;
	else
		bOffOptions |= SRCHTYPE_COMMENT;

	l_modified = ((m_searchStatus ^ bOnOptions) != 0x0);

	*searchOK = ((bOnOptions != 0x0) ? TRUE : FALSE);

	m_searchStatus &= ~bOffOptions;
	m_searchStatus |= bOnOptions;

	return l_modified;
	} 

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::GetSearchRange
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL SEARCHMANAGER::GetSearchRange(
	HWND hDlg,
	BOOL * searchOK)
	{
	HWND		hRange			= GetDlgItem(hDlg, SRCHID_RANGE);
	LRESULT l_itemData;
	LRESULT l_index;
	BOOL l_modified = FALSE;

	*searchOK = FALSE;

	if(CB_ERR != 
		(l_index = ComboBox_GetCurSel(hRange)))
		{
		if(CB_ERR != (l_itemData = ComboBox_GetItemData(hRange, l_index)))
			{
			if(l_itemData == SRCHRANGE_PROJECT)
				{
				//see if we we're searching project wide before.
				if(!(m_searchStatus & SRCHIN_PROJECT))
					{
					//we were not searching project wide before
					m_searchStatus &= ~(SRCHIN_ACTIVESUBJECT |
						SRCHIN_CHOSENSUBJECT);

					m_searchStatus |= SRCHIN_PROJECT;
					
					l_modified = TRUE;
					}

				*searchOK = TRUE;
				}

			else if(l_itemData == SRCHRANGE_ACTIVESUBJECT)
				{
				if(!(m_searchStatus & SRCHIN_ACTIVESUBJECT))
					{
					// we we're not searching subject wide before.
					m_searchStatus &= ~(SRCHIN_CHOSENSUBJECT | SRCHIN_PROJECT);
					
					m_searchStatus |= SRCHIN_ACTIVESUBJECT;
					
					l_modified = TRUE;
					}

				*searchOK = TRUE;
				}

			else if(l_itemData >= 0)	//make sure we have a subject UID
				{
				if(!(m_searchStatus & SRCHIN_CHOSENSUBJECT))
					{
					m_searchStatus	&= ~(SRCHIN_PROJECT | SRCHIN_ACTIVESUBJECT);
					
					m_searchStatus |= SRCHIN_CHOSENSUBJECT;
					
					l_modified = TRUE;
					}
				
				//if the itemData and srchSubject are different, then reset the
				//indexes.
				if(l_itemData != m_srchSubject.id)
					{
					m_srchSubject = l_itemData;
					l_modified = TRUE;
					}

				*searchOK = TRUE;
				}
			}
		}
		
	return l_modified; 
	} 

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::SetSearchParameters
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL SEARCHMANAGER::SetSearchParameters()
	{
	LPFILEOBJECT l_currentContext = pTheSystem->GetFileObject(); 
	BOOL l_rval;

	if(!m_searchContext || (m_searchContext->GetFileIDStamp() != 
		l_currentContext->GetFileIDStamp()))
		l_rval = SetSearchForNewContext(l_currentContext);

	else	//we're still in the same project
		l_rval = SetSearchForSameContext();

	return l_rval;
	}


/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::SetSearchForNewContext
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL SEARCHMANAGER::SetSearchForNewContext(
 	LPFILEOBJECT newContext)
	{
	BOOL l_rval = FALSE; //default to can't setup search
	LPPROJECT l_currentProject = PROGSYSTEM::GetCurrentProject();
	LPSUBJECT l_subjectPtr;
	HWND l_hWnd;
	LPBASEWINDOW l_winPtr;

	I_ASSERT(l_currentProject);
	I_ASSERT(newContext);

	if(l_currentProject)
		{
		m_searchContext = newContext;

		m_srchProject = l_currentProject;
		
		if(m_searchStatus & SRCHIN_PROJECT)
			{				
			m_subjectIndex = 0;
			
			//get the starting subject
			l_subjectPtr = (LPSUBJECT)GetObject(
				m_srchProject->GetSubjectBlock(), m_subjectIndex);
			
			if(l_subjectPtr)	
				{
				l_rval = TRUE;
				m_srchSubject = GetUID(l_subjectPtr);
				}
			else
				m_srchSubject = NULLSERUID;

			}
		else if(m_searchStatus & SRCHIN_ACTIVESUBJECT)
			{
			l_subjectPtr = l_currentProject->GetActiveSubject();
			if(l_subjectPtr != NULL)
				{
				m_srchSubject = GetUID(l_subjectPtr);
				l_rval = TRUE;
				}
			else
				{
				//NULL the srchSubject in case we can't find a subject window.	
				m_srchSubject = NULLSERUID;
				
				//if the display window is open, then the active subject may
				//not actually be active - it may be the subject just below the
				//found-list-window.  So see if we can get a subject window just
				//below the found list window.
				
				//first make sure that the found list window is the active window.
				if(m_hwndOwnerList == (HWND) SendMessage(PROGSYSTEM::hWndClient, WM_MDIGETACTIVE, 0, 0L))
					{
					//see if a subject window is the next in line.
					l_hWnd = GetNextWindow(m_hwndOwnerList ,GW_HWNDNEXT);

					if(l_hWnd)
						{
						if(BASEWINDOW::list)
							{
							l_winPtr = (LPBASEWINDOW)BASEWINDOW::list->GetObject(l_hWnd);
							if(l_winPtr->TestSubjectFlag())
								{
								m_srchSubject = GetUID(((LPSUBJECTWINDOW)l_winPtr)->
									GetSubjectPtr());

								l_rval = TRUE;
								}
							}
						}
					}
				}
			}
		else if(m_searchStatus & SRCHIN_CHOSENSUBJECT)
			{
			//if the chosen subject is not valid, don't reset it because
			//the user may  go back to the project where the chosen subject
			//is valid.
			if((m_srchSubject != NULLSERUID) && (GetObject(m_srchSubject)) &&
				(GetUIDType(m_srchSubject) == RT_SUBJECT))
				{
				//the search subject is valid.
				l_rval = TRUE;
				}
			}

		if(l_rval)
			ResetIndexArray();
		}

	return l_rval;
	} 

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::SetSearchForSameContext
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL SEARCHMANAGER::SetSearchForSameContext()
	{
	LPSUBJECT l_subjectPtr;
	BOOL l_rval = FALSE; //default to can setup for search;

	I_ASSERT(m_srchProject);

	if(m_searchStatus & SRCHIN_ACTIVESUBJECT)
		{
		// see if the active subject has changed.
		l_subjectPtr = m_srchProject->GetActiveSubject();
		
		if(l_subjectPtr)
			{
			l_rval = TRUE;	
			if(GetUID(l_subjectPtr) != m_srchSubject)
				{
				m_srchSubject = GetUID(l_subjectPtr);
				ResetIndexArray();
				}
			}
		}
	else if(m_searchStatus & SRCHIN_CHOSENSUBJECT)
		{
		//see if the subject is valid and if not, set the 
		//srchSubject to NULLSERUID
		if((m_srchSubject == NULLSERUID)  || !GetObject(m_srchSubject) ||
			(GetUIDType(m_srchSubject) != RT_SUBJECT))
			{
			m_srchSubject = NULLSERUID;
			}
		else
			l_rval = TRUE; //valid to search
		}
	else if(m_searchStatus & SRCHIN_PROJECT)
		l_rval = TRUE;

	else
		{
		I_PRINT("Invalid search status in SEARCHMANAGER::DoSearch()");
		}

	return l_rval;
	} 

																			 																			 

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::ResetIndexArray
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
//
// ASSUMPTIONS:  must be called after the srchSubject has been set.
/////////////////////////////////////////////////////////////
BOOL SEARCHMANAGER::ResetIndexArray()
	{
	int l_i;
	LPSUBJECT l_subjectPtr = NULL;

	if(m_srchSubject != NULLSERUID)
		l_subjectPtr = (LPSUBJECT)GetObject(m_srchSubject);

	I_ASSERT(l_subjectPtr);
	
	if(l_subjectPtr)
		{
		I_ASSERT(GetUIDType(m_srchSubject) == RT_SUBJECT);

		for(l_i = 0; l_i < m_indexSize; l_i++)
			{
			m_indexArray[l_i].index = 0;
			m_indexArray[l_i].paramIndex = 0;	
			
			if(m_indexArray[l_i].indexType & 
				(SRCHTYPE_FUNCTION | SRCHTYPE_PARAMETER))
				//search for functions or parameters
				m_indexArray[l_i].srchBlock = l_subjectPtr->GetFunctionList();

			else if(m_indexArray[l_i].indexType & 
				(SRCHTYPE_OBJECT | SRCHTYPE_TYPE))
				{
				//search for objects (by type or name)
				m_indexArray[l_i].srchBlock = l_subjectPtr->GetObjectList();
				}

			else if(m_indexArray[l_i].indexType & SRCHTYPE_COMMENT)
				m_indexArray[l_i].srchBlock = l_subjectPtr->GetCommentList();

			else if(m_indexArray[l_i].indexType & SRCHTYPE_GROUP)
				m_indexArray[l_i].srchBlock = l_subjectPtr->GetObjGroupList();

			else
				{
				m_indexArray[l_i].srchBlock = NULLSERUID;
				I_PRINT("Setting srchBlock to NULL in SEARCHMANAGER::ResetIndexArray");
				}
			}

		m_indexIndex = 0;
		}	

	return (l_subjectPtr != NULL)?TRUE:FALSE;
	}


/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::Register
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////																			 
void SEARCHMANAGER::Register()
	{
	WNDCLASS l_wc;

	if(!PROGSYSTEM::hPrevInst)
		{
		l_wc.style = 0;
		l_wc.lpfnWndProc =SEARCHMANAGER::MasterWndProc;
		l_wc.hInstance = PROGSYSTEM::hInst;
		l_wc.hIcon = LoadIcon(PROGSYSTEM::hInst, 
			MAKEINTRESOURCE(IDIC_SEARCHWINDOW));
		l_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		l_wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
		l_wc.lpszMenuName = NULL;                                                               
		l_wc.lpszClassName = sm_className;
		l_wc.cbClsExtra = 0;
		l_wc.cbWndExtra = sizeof(void *);

		RegisterClass((LPWNDCLASS)&l_wc);
		}

	sm_registered = TRUE;
	}


/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::CreateFindWindow
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL SEARCHMANAGER::CreateFindWindow()
	{
	MDICREATESTRUCT l_mdc;
	BOOL l_rval = TRUE; //assume success.
																		 
	if(m_searchStatus & SRCHFLAG_FOUNDLISTOPEN)
		{
		I_ASSERT(m_hwndOwnerList != NULL);

		if(IsWindow(m_hwndOwnerList))
			{
			//The window is already showing so
			//purge the window without redrawing.
			SendMessage(m_hwndDisplayList, WM_SETREDRAW, FALSE, 0L);
			ListBox_ResetContent(m_hwndDisplayList);
			SendMessage(m_hwndDisplayList, WM_SETREDRAW, TRUE, 0L);
			}
		else
			{
			I_PRINT("Invalid find window.");
			l_rval = FALSE;
			}
		}
	else
		{
		l_mdc.szClass = sm_className;
		l_mdc.szTitle = "Found Items";
		l_mdc.hOwner = PROGSYSTEM::hInst;
		l_mdc.style = SRCHFOUNDLIST_OWNERSTYLE;

		l_mdc.lParam = (LONG)this;
		
		l_mdc.x = sm_foundListOrigin.x;
		l_mdc.y = sm_foundListOrigin.y;
		l_mdc.cx = sm_widthFoundList;
		l_mdc.cy = sm_heightFoundList;

		SendMessage(PROGSYSTEM::hWndClient, WM_MDICREATE, 
			0, (LONG)(LPMDICREATESTRUCT)&l_mdc);

		I_ASSERT(m_hwndOwnerList);
	
		if(!m_hwndOwnerList)
			l_rval = FALSE;
		}

	return l_rval;
	}



/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::MasterWndProc
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
LRESULT CALLBACK SEARCHMANAGER::MasterWndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	LPSEARCHMANAGER l_objectPtr;
	LRESULT l_rval;
	LPCREATESTRUCT l_cs;
	LPMDICREATESTRUCT l_mdc;

	//try to retreive the instance pointer from the window extra bytes.
	l_objectPtr =	(LPSEARCHMANAGER)GetWindowLong(hWnd, 0);

	switch (message)
		{
		case WM_CREATE:
			I_ASSERT(l_objectPtr == 0); 
 				
 			l_cs = (LPCREATESTRUCT)lParam;
			l_mdc = (LPMDICREATESTRUCT)l_cs->lpCreateParams;

			//store the instance pointer in the window extra bytes
			SetWindowLong(hWnd, 0, l_mdc->lParam);

			l_objectPtr = (LPSEARCHMANAGER)l_mdc->lParam;

			break;

		default:
			break;
		}

	if (l_objectPtr)
		l_rval = l_objectPtr->SlaveWndProc(hWnd, message, wParam, lParam);
	else
		l_rval = DefMDIChildProc(hWnd, message, wParam, lParam);

	return l_rval;
	}


/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::SlaveWndProc
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
LRESULT SEARCHMANAGER::SlaveWndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
	{
	BOOL l_processed=FALSE;
	RECT l_rc;
	LRESULT l_rval;

	switch(message)
		{
		case WM_CREATE:
			I_ASSERT(m_hwndOwnerList == NULL);
			m_hwndOwnerList = hWnd;

			I_ASSERT(m_hwndDisplayList == NULL);
			
			GetClientRect(hWnd, (LPRECT)&l_rc);
			
			m_hwndDisplayList = CreateWindow("LISTBOX", NULL,
			 	SRCHFOUNDLIST_LISTSTYLE,
				0,0, 
				l_rc.right - l_rc.left + 1,
				l_rc.bottom - l_rc.top + 1,
				hWnd,
				(HMENU) SRCHCTRL_DISPLAYLIST,
				PROGSYSTEM::hInst,
				NULL);

			I_ASSERT(m_hwndDisplayList != NULL);

			l_processed = TRUE;

			break;

		case WM_SIZE:
			//pass in new width and height
			HandleOwnerWindowSizeChange(wParam, LOWORD(lParam), HIWORD(lParam));
			break;

		case WM_MOVE:
			SetDefaultFoundListLocation();
			break;

		case WM_DESTROY:
			I_ASSERT(m_hwndOwnerList != NULL);
			I_ASSERT(m_hwndDisplayList != NULL);
						
			m_hwndDisplayList = NULL;
			m_hwndOwnerList = NULL;
			m_searchStatus &= ~SRCHFLAG_FOUNDLISTOPEN;
			break;

		case WM_COMMAND: 
			{
			WORD		wNotify		= GET_WM_COMMAND_CMD(wParam, lParam); 
			HWND		hCtl			= GET_WM_COMMAND_HWND(wParam, lParam); 

			if(wNotify == LBN_DBLCLK)
				{
				HighlightValidListItem(hCtl, ListBox_GetCurSel(hCtl));

				l_processed = TRUE;
				}
			break;
			}

		}

	if(!l_processed)
		l_rval = DefMDIChildProc(hWnd, message, wParam, lParam);

	return l_rval;
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANGER::SetDefaultFoundListLocation
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void SEARCHMANAGER::SetDefaultFoundListLocation()
	{
	RECT l_rc;

	GetWindowRect(m_hwndOwnerList, (LPRECT)&l_rc);
	sm_foundListOrigin.x = l_rc.left;
	sm_foundListOrigin.y = l_rc.top;
	sm_widthFoundList = l_rc.right - l_rc.left;
	sm_heightFoundList = l_rc.bottom - l_rc.top;
	
	} 

																			 
/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::HandleOwnerWindowSizeChange()
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void SEARCHMANAGER::HandleOwnerWindowSizeChange(
	WPARAM changeCode,
	int width,
	int height)
	{
	RECT l_rc;
	int l_listHeight;
  RECT l_cl;
	SIZE dif;
  
	//size the listbox to fit the client area
	if(changeCode != SIZE_MINIMIZED)
		MoveWindow(m_hwndDisplayList,
			0,0,
			width,
			height,
			TRUE);	 
	
	//only want to go for an exact fit if the window is not
	//maximized or minimized.
	if(changeCode == SIZE_RESTORED)
		{
		//now make sure the owner window fits the listbox
		GetWindowRect(m_hwndDisplayList, (LPRECT)&l_rc);
		l_listHeight = l_rc.bottom - l_rc.top + 1;

		if(l_listHeight != height)
			{
			//the listbox does not fit the client area
			GetWindowRect(m_hwndOwnerList, (LPRECT)&l_rc);
      GetClientRect(m_hwndOwnerList, &l_cl);
      
			//figure the difference before converting the origin
      dif.cx = (l_rc.right - l_rc.left) - (l_cl.right - l_cl.left);
      dif.cy = (l_rc.bottom - l_rc.top) - (l_cl.bottom - l_cl.top);
			
			//convert the window origin to client coordinates.		
			ScreenToClient(PROGSYSTEM::hWndClient, (LPPOINT)&l_rc);

			MoveWindow(m_hwndOwnerList,
				l_rc.left,
				l_rc.top,
				width + dif.cx,
				l_listHeight + dif.cy,
				TRUE);
        
			}		

		SetDefaultFoundListLocation();
		}
	} 

																			 

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::ReportFoundItem
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void SEARCHMANAGER::ReportFoundItem(
	void * itemPtr)
	{
	LPFOUNDITEM l_node;

	I_ASSERT(m_currentSubject);
	I_ASSERT(m_currentType);
	
	if(m_searchStatus & SRCHOPTION_FOUNDLIST)
		{
		l_node = new FOUNDITEM;

		I_ASSERT(l_node);

		if(m_currentType & (SRCHTYPE_OBJECT | SRCHTYPE_TYPE))
			l_node->name = ((LPOBJINST)itemPtr)->GetName();

		else if(m_currentType & SRCHTYPE_FUNCTION)
 			l_node->name = ((LPFUNCINST)itemPtr)->GetDescPtr()->pGetName();

		else if(m_currentType & SRCHTYPE_GROUP)
 			l_node->name = ((LPOBJGROUP)itemPtr)->GetName();
	
		else if(m_currentType & SRCHTYPE_COMMENT)
			l_node->name = ((LPCOMMENT)itemPtr)->briefComment;

		else if(m_currentType & SRCHTYPE_PARAMETER)
			l_node->name = ((LPPARAM)itemPtr)->GetName();

		l_node->uid = GetUID(itemPtr).id;
		
		m_displaySortTree->Add(l_node);
		}
	else
		{
		HighlightItem(m_currentType, itemPtr, m_currentSubject);																		 
		}						
	}

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::ReportSubject
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void SEARCHMANAGER::ReportSubject(
 	LPSUBJECT subjectPtr)
	{
	if(m_searchStatus & SRCHOPTION_FOUNDLIST)
		{
		//if necessary, cause the tree to purge it self
		DisplaySortTree();

		//set flag for new subject
		m_searchStatus |= SRCHFLAG_NEEDSUBJECTLABEL;
		}
	m_currentSubject = subjectPtr;

	} 

 /////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::ReportType
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void SEARCHMANAGER::ReportType(
	long type)
	{

	if(m_searchStatus & SRCHOPTION_FOUNDLIST)
		{
		//go throught the tree and add all the members to the display list
		DisplaySortTree();
		}

	m_currentType = type;
	} 

																			 																			 
/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::FormatDisplayLine
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void SEARCHMANAGER::FormatDisplayLine(
 	LPSTR line,
	LPCSTR name,
	WORD formatType)
	{
	if(formatType == SRCHFORMAT_SUBJECT)
		lstrcpy(line, "");
	else if(formatType == SRCHFORMAT_TYPE)
		lstrcpy(line, "\t");
	else if(formatType == SRCHFORMAT_ITEM)
		lstrcpy(line, "\t\t");
	else
		{ 
		I_PRINT("Invalid format type.");
		}

	lstrcat(line, name);

	if(formatType != SRCHFORMAT_ITEM)
		lstrcat(line, ":");
	} 
	
/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::AddDisplayLine
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
long SEARCHMANAGER::AddDisplayLine(
 	LPCSTR text,
 	long data)
	{
	long l_rval;

	I_ASSERT(m_hwndDisplayList);
	
	l_rval = ListBox_AddString(m_hwndDisplayList, text);

	I_ASSERT(l_rval != LB_ERR);

	if(l_rval != LB_ERR)
		l_rval = ListBox_SetItemData(m_hwndDisplayList, l_rval, data);

	return l_rval;
	} 

																			 
/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::DisplaySortTree
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void SEARCHMANAGER::DisplaySortTree()
	{
	char l_buffer[128];
	char l_typeBuffer[32];

	I_ASSERT(m_displaySortTree);
	LPFOUNDITEM l_node;

	if(!m_displaySortTree->EmptyTree())
		{
		if(m_searchStatus & SRCHFLAG_NEEDSUBJECTLABEL)
			{
			I_ASSERT(m_currentSubject);

			FormatDisplayLine(l_buffer, m_currentSubject->GetName(),
				SRCHFORMAT_SUBJECT);
				
			AddDisplayLine(l_buffer, SRCHITEM_SUBJECT);

			m_searchStatus &= ~SRCHFLAG_NEEDSUBJECTLABEL;
			}

		//add the old type label
		GetCurrentTypeName(l_typeBuffer);
		
		FormatDisplayLine(l_buffer, l_typeBuffer, SRCHFORMAT_TYPE);

		AddDisplayLine(l_buffer, SRCHITEM_TYPE);

		m_displaySortTree->SortInOrder();

		//now go through tree and add every member to the window
		l_node = NULL;
		while(NULL != (l_node = 
			(LPFOUNDITEM)m_displaySortTree->GetNextSortedMember(l_node)))
			{
			FormatDisplayLine(l_buffer, l_node->name, SRCHFORMAT_ITEM);
			AddDisplayLine(l_buffer, l_node->uid);
			}

		m_displaySortTree->PurgeTree();
		}

	//make sure no items selected.
	ListBox_SetCurSel(m_hwndDisplayList, -1);
	} 

																			 																			 				
/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::GetTypeName
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void SEARCHMANAGER::GetCurrentTypeName(
	LPSTR buffer)
	{
	if(m_currentType & (SRCHTYPE_TYPE | SRCHTYPE_OBJECT))
		lstrcpy(buffer, "Objects");
	
	else if(m_currentType & SRCHTYPE_FUNCTION)
		lstrcpy(buffer, "Functions");

	else if(m_currentType & SRCHTYPE_GROUP)
		lstrcpy(buffer, "Groups");

	else if(m_currentType & SRCHTYPE_PARAMETER)
		lstrcpy(buffer, "Parameters");

	else if(m_currentType & SRCHTYPE_COMMENT)
		lstrcpy(buffer, "Comments");

	else
		{
		buffer[0] = '\0';
		I_PRINT("Invalid current search type.");
		}

	lstrcat(buffer, " Found");
	}
	
/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::IsFindAllCode
//						 
// PARAMETERS:		Assumes a NULL terminated string.
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL SEARCHMANAGER::IsFindAllCode(
	LPCSTR string)
	{
	LPCSTR ptr	= string;
	BOOL l_rval;

	//The find code is either the empty string or a string with only
	//spaces or tab characters in it.
	while(*ptr != '\0' && (*ptr == ' ' || *ptr == '\t'))
		ptr++;

	if(*ptr == '\0')
		l_rval = TRUE;
	else
		l_rval = FALSE;

	return l_rval;
	} 
																			  
/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::HighlightItem
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void SEARCHMANAGER::HighlightItem(
	long type,
	void * itemPtr,
	LPSUBJECT subjectPtr)
	{
	//Go to the found item
	if(type & (SRCHTYPE_OBJECT | SRCHTYPE_TYPE))
		subjectPtr->HighlightObj((LPOBJINST)itemPtr);

	//passing true, true to HighlightWorkbenchItem means the item should flash
	//and be selected.
	else if(type & SRCHTYPE_FUNCTION)
		subjectPtr->HighlightWorkbenchItem((LPFUNCINST)itemPtr, TRUE, TRUE);

	else if(type & SRCHTYPE_GROUP)
		subjectPtr->HighlightWorkbenchItem((LPOBJGROUP)itemPtr, TRUE, TRUE);
	
	else if(type & SRCHTYPE_COMMENT)
	  subjectPtr->HighlightWorkbenchItem((LPCOMMENT)itemPtr, TRUE, TRUE);

	else if(type & SRCHTYPE_PARAMETER)
		subjectPtr->HighlightParameter((LPPARAM)itemPtr);
	} 

																			 																			 
/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::OkToSearchAgain
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL SEARCHMANAGER::OkToSearchAgain()
	{
	BOOL l_rval;

	//the first check is necessary for the case when the used tries to do
	//a find next after opening up a project.  The status may be in a state
	//where it's OK to search but the foundlist has not been displayed yet.
	//Therefore, it's not OK to search again.
	if((m_searchStatus & SRCHOPTION_FOUNDLIST) && 
		(!(m_searchStatus & SRCHFLAG_FOUNDLISTOPEN)))
		l_rval = FALSE;

	else
		l_rval = (m_searchStatus & SRCHSTATE_CANSEARCH)?TRUE:FALSE;

	return l_rval;
	}

																			 																			 
/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::SearchAgain
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL SEARCHMANAGER::SearchAgain(
	LPFILEOBJECT currentContext)
	{
	BOOL l_rval = FALSE;
	long l_count;
	long l_item;
	BOOL l_doPop = FALSE;
	LPFILEOBJECT l_holdContext;

	I_ASSERT(currentContext);

	if(OkToSearchAgain())
		{
		if(m_searchStatus & SRCHFLAG_FOUNDLISTOPEN)
			{
			I_ASSERT(m_searchContext);

			if(m_searchContext)
				{
				if(currentContext->GetFileIDStamp() != m_searchContext->GetFileIDStamp())
					{
					PROGSYSTEM::PushAndRegisterContext(m_searchContext);
					l_doPop = TRUE;
					}

				l_count = ListBox_GetCount(m_hwndDisplayList);
				l_item  = ListBox_GetCurSel(m_hwndDisplayList);

				if(l_item == LB_ERR)
					l_item = 0;
				else
					l_item ++;

				if((l_item == l_count)  && (l_count > 0)) 
					{
					//we're at the end of the list
					if(MessageBox(NULL,
							sm_endMessage, sm_endMessageTitle,
							MB_YESNO|MB_TASKMODAL|MB_ICONINFORMATION) == IDYES)
						{
						//set the 
						l_item = SRCH_FIRSTVALIDITEM;
						}
					}
					
				if((l_item >= 0) && (l_item < l_count))
					{
					l_rval = HighlightValidListItem(m_hwndDisplayList, (int)l_item);
					if(!l_rval)
						MessageBox(NULL, sm_cantFindMessage, 
							sm_endMessageTitle, MB_OK|MB_ICONINFORMATION|MB_TASKMODAL);
					}
				
				if(l_doPop)
					PROGSYSTEM::PopAndRegisterContext();
				}	
			}
		else
			{
			if(!m_searchContext || (
				m_searchContext->GetFileIDStamp() != currentContext->GetFileIDStamp()))
				{
				//store the search context for the 1 case below where it
				//may need to be restored.
				l_holdContext = m_searchContext;
				
				//cause the indexes to reset
				m_searchContext = NULL;

				//make adjustments if we're searching in the chosen subject
				if(m_searchStatus & SRCHIN_CHOSENSUBJECT)
					{
					if(l_holdContext)
						{
						//we don't want the indexes to reset, just switch back to
						//the old context
						m_searchContext = l_holdContext;
						PROGSYSTEM::PushAndRegisterContext(m_searchContext);
						l_doPop = TRUE;
						}
					else
						{
						//just start searching in the active subject of the
						//new context
						m_searchStatus &= ~SRCHIN_CHOSENSUBJECT;
						m_searchStatus |= SRCHIN_ACTIVESUBJECT;
						}
					}
				}	

			l_rval = Search();

			if(l_doPop)
				PROGSYSTEM::PopAndRegisterContext();
			}
		}

	return l_rval;
	} 


/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::HighlightValidItem
//						 
// PARAMETERS:
//
// DESCRIPTION:	
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL SEARCHMANAGER::HighlightValidListItem(
	HWND hListBox,
	int item)
	{
	long	l_data;
	LRESULT l_newIndex;
	long l_type;
	LPSUBJECT l_subjectPtr;
	void * l_itemPtr;
	BOOL l_rval = FALSE;
	BOOL l_doPop = FALSE;
	BOOL l_runDanger = FALSE;

	//find out if the selection is a valid item,
	//and if not, activate the first valid item.

	if((item >= 0) && (item < ListBox_GetCount(hListBox))) 
		{
		I_ASSERT(m_searchContext);

		if(pTheSystem->GetFileObject()->GetFileIDStamp() != 
			m_searchContext->GetFileIDStamp())
			{
			PROGSYSTEM::PushAndRegisterContext(m_searchContext);
			l_doPop = TRUE;
			l_runDanger = TRUE; //don't want to search if we're running	since
												//the window's would be hidden.
			}

		l_data = ListBox_GetItemData(hListBox, item);

		//see if we need to move the selection to a valid double click
		//item.
		if(l_data == SRCHITEM_SUBJECT)
			l_newIndex = item + 2;
		else if(l_data == SRCHITEM_TYPE)
			l_newIndex = item + 1;
		else
			l_newIndex = item;

		//adjust the index to the first valid double click item.
		if(l_newIndex != item)
			{
			ListBox_SetCurSel(hListBox, l_newIndex);
			l_data = ListBox_GetItemData(hListBox, l_newIndex);
			}
		else
			{
			//make sure the correct item is highlighted.
			if(l_newIndex != ListBox_GetCurSel(hListBox))
				ListBox_SetCurSel(hListBox, l_newIndex);
			}

		l_type = 0;
		l_subjectPtr = NULL;
		
		l_itemPtr = GetObject((SERUID)l_data);

		if(l_itemPtr && !(l_runDanger && PROGSYSTEM::TestRunTimeFlag()))
			{
			l_rval = TRUE;
			
			switch(GetUIDType((SERUID)l_data))
				{
				case RT_OBJINST:
					l_type = SRCHTYPE_OBJECT;
					l_subjectPtr = ((LPOBJINST)l_itemPtr)->GetSubjectPtr();
					break;

				case RT_FUNCINST:
					l_type = SRCHTYPE_FUNCTION;
					l_subjectPtr = ((LPFUNCINST)l_itemPtr)->GetSubjectPtr();
					break;

				case RT_COMMENT:
					l_type = SRCHTYPE_COMMENT;
					l_subjectPtr = ((LPCOMMENT)l_itemPtr)->GetSubjectPtr();
					break;

				case RT_OBJGROUP:
					l_type = SRCHTYPE_GROUP;
					l_subjectPtr = ((LPOBJGROUP)l_itemPtr)->GetSubjectPtr();
					break;

				case RT_PARAM:
					l_type = SRCHTYPE_PARAMETER;
					l_subjectPtr = ((LPFUNCINST)GetObject(
						((LPPARAM)l_itemPtr)->ownerFunc))
						->GetSubjectPtr();
					break;

				default:
					l_rval = FALSE;
					break;
				}
			}
		else
			MessageBeep(0);

		if(l_subjectPtr && l_type && l_itemPtr)
			HighlightItem(l_type, l_itemPtr, l_subjectPtr);
		
		if(l_doPop)
			PROGSYSTEM::PopAndRegisterContext();
		}
		
	return l_rval;	
	} 

//////////////////////////////////////////////////////////////
// Preference Stuff

//preference string names
static char SRCHPREF_KEY[]=			 				{"Search Info"};
static char SRCHPREF_STATUS[]=					{"Search Cookie"};
static char SRCHPREF_XFOUNDLIST[]=			{"Found list x origin"};
static char SRCHPREF_YFOUNDLIST[]= 			{"Found list y origin"};
static char SRCHPREF_WIDTHFOUNDLIST[]= 	{"Found list width"};
static char SRCHPREF_HEIGHTFOUNDLIST[]= {"Found list height"};
static char SRCHPREF_SEARCHSTRING[]=		{"Search String"};



	/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::WriteProfileInfo
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void SEARCHMANAGER::WriteProfileInfo()
	{
  HKEY vabKey;
  HKEY srchKey=NULL;
  DWORD action;
	
	//get key to where enironment preferences go
	if((vabKey = AUtlOpenProductSubKey(HKEY_CURRENT_USER, AUTL_KEY_APEBUILDER) )!= NULL)  	
  	{
  	RegCreateKeyEx(vabKey, SRCHPREF_KEY, 0, NULL,
			REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &srchKey, &action);
  	}

	I_VERIFY(AUtlWriteKeyLong(srchKey, SRCHPREF_STATUS, 
		m_searchStatus));

	I_VERIFY(AUtlWriteKeyString(srchKey, SRCHPREF_SEARCHSTRING, 
		m_searchString));

	I_VERIFY(AUtlWriteKeyLong(srchKey, SRCHPREF_XFOUNDLIST, 
		sm_foundListOrigin.x));

	I_VERIFY(AUtlWriteKeyLong(srchKey, SRCHPREF_YFOUNDLIST,
		sm_foundListOrigin.y));

	I_VERIFY(AUtlWriteKeyLong(srchKey, SRCHPREF_WIDTHFOUNDLIST,
		sm_widthFoundList));
	
 I_VERIFY(AUtlWriteKeyLong(srchKey, SRCHPREF_HEIGHTFOUNDLIST,
		sm_heightFoundList));

	if(srchKey)
  	RegCloseKey(srchKey);
	
	if(vabKey)
  	AUtlCloseProductSubKey(vabKey);
    
	} 

/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::ReadProfileInfo
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void SEARCHMANAGER::ReadProfileInfo()
	{
	RECT l_rc;
  HKEY vabKey;
  HKEY srchKey=NULL;

	if((vabKey = AUtlOpenProductSubKey(HKEY_CURRENT_USER, AUTL_KEY_APEBUILDER)) != NULL)
  	{
   	RegOpenKeyEx(vabKey, SRCHPREF_KEY, 0, KEY_ALL_ACCESS, &srchKey); 
    }
    
	m_searchStatus = AUtlGetKeyLong(srchKey, SRCHPREF_STATUS, 0);
  
	//the default search string is the empty string.
	AUtlGetKeyString(srchKey, SRCHPREF_SEARCHSTRING,
		m_searchString, SRCH_MAXSTRING, "");

	//default origin is (0,0)
	sm_foundListOrigin.x = AUtlGetKeyLong(srchKey,
		SRCHPREF_XFOUNDLIST, 0);

	sm_foundListOrigin.y = AUtlGetKeyLong(srchKey,
		SRCHPREF_YFOUNDLIST, 0);

	//frame system client area to calculate the default width and height
	GetClientRect(PROGSYSTEM::hWndClient, (LPRECT)&l_rc);

	//set the width and height to 0 if the profile ints can't be found since
	//we need to check below anyway to make sure they are valid.
	sm_widthFoundList = AUtlGetKeyLong(srchKey,
		SRCHPREF_WIDTHFOUNDLIST, 0);

	sm_heightFoundList = AUtlGetKeyLong(srchKey,
		SRCHPREF_HEIGHTFOUNDLIST, 0);

	if(sm_widthFoundList <= 0)
		sm_widthFoundList = (l_rc.right - l_rc.left) / 4;

	if(sm_heightFoundList <= 0)
		sm_heightFoundList = 3 * (l_rc.bottom - l_rc.top) / 5;

	
	if(srchKey)
  	RegCloseKey(srchKey);
    
  if(vabKey)
  	AUtlCloseProductSubKey(vabKey);
	} 
	
/////////////////////////////////////////////////////////////
// FUNCTION:			SEARCHMANAGER::ReportProjectClosed
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void SEARCHMANAGER::ReportProjectClosed(
	LPFILEOBJECT projectContext)
	{
	I_ASSERT(projectContext);

	if(m_searchContext && (projectContext->GetFileIDStamp() == 
		m_searchContext->GetFileIDStamp()))
		{
		m_searchContext = NULL;
		m_searchStatus &= ~SRCHSTATE_CANSEARCH;
		 
		if(m_searchStatus & SRCHFLAG_FOUNDLISTOPEN)
			{
			//clear the found list
			SendMessage(m_hwndDisplayList, WM_SETREDRAW, FALSE, 0L);
			ListBox_ResetContent(m_hwndDisplayList);
			SendMessage(m_hwndDisplayList, WM_SETREDRAW, TRUE, 0L);

			InvalidateRect(m_hwndDisplayList, NULL, TRUE);
			UpdateWindow(m_hwndDisplayList);
			}
		else if(m_searchStatus & SRCHIN_CHOSENSUBJECT)
			m_searchStatus &= ~SRCHIN_CHOSENSUBJECT;
		}
	}
																			 																		 																			 
																			 																			 






