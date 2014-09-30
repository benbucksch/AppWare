#include "almsys.h"
#include "mtypelst.h"
#include "intdebug.h"

//node status bits
#define MTL_NODE_INVALIDADMN							0x00000001L
#define MTL_NODE_INVALIDMESSAGESHOWN			0x00000002L
#define MTL_NODE_LOADATTEMPTED						0x00000004L

//master list status bits
#define MTL_MASTER_ALLDONE		0x00000001L

#define MTL_PROCCATEGORY						((LPSTR)"administration")

/////////////////////////////////////////////////////////////
// FUNCTION:			MASTERTYPELIST::MASTERTYPELIST
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
MASTERTYPELIST::MASTERTYPELIST()
	{
	m_admnProcList = new BITREE();
	m_nodeKey = NULL;
	m_status = 0L;
	} 

/////////////////////////////////////////////////////////////
// FUNCTION:			MASTERTYPELIST::~MASTERTYPELIST
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
MASTERTYPELIST::~MASTERTYPELIST()
	{
	delete m_admnProcList;	
	} 


/////////////////////////////////////////////////////////////
// FUNCTION:			MASTERTYPELIST::PrepareNodes
//						 
// PARAMETERS:
//
// DESCRIPTION:		Must be called so that all the nodes can 
//								be retrieved for DLL loading.
//
// RETURNS:
/////////////////////////////////////////////////////////////
void MASTERTYPELIST::PrepareNodes()
	{
	m_admnProcList->SortInOrder();
	} 

																			 
																			 																			 
/////////////////////////////////////////////////////////////
// FUNCTION:			MASTERTYPELIST::LoadAdmnProc
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
BOOL MASTERTYPELIST::LoadAdmnProc(
	LPADMNNODE nodePtr)
	{

	I_ASSERT(nodePtr);
			
	if(!(nodePtr->m_status & MTL_NODE_LOADATTEMPTED))
		{
		nodePtr->m_status |= MTL_NODE_LOADATTEMPTED;

	 	// Try to load that DLL, tell Windows not to complain about
	 	// file not found ... we'll complain ourselves

		UINT oldErrorMode = ::SetErrorMode(SEM_NOOPENFILEERRORBOX);
		nodePtr->m_libHandle = LoadLibrary(nodePtr->m_dllName);
		::SetErrorMode(oldErrorMode);

		if (nodePtr->m_libHandle == NULL)
			{
			nodePtr->m_admnProc = NULL;
			nodePtr->m_errorValue = 0xffff & GetLastError();
			nodePtr->m_status |= MTL_NODE_INVALIDADMN;
			}

		else
			{
			nodePtr->m_admnProc = (ADMNPROC)GetProcAddress(nodePtr->m_libHandle, 
				nodePtr->m_procName);

			if (!(nodePtr->m_admnProc))
				nodePtr->m_status |= MTL_NODE_INVALIDADMN;
			}
		}

	return (nodePtr->m_status & MTL_NODE_INVALIDADMN)?FALSE:TRUE;
	} 

																			 
/////////////////////////////////////////////////////////////
// FUNCTION:			MASTERTYPELIST::ShowErrorMessage
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void MASTERTYPELIST::ShowErrorMessage(
	LPADMNNODE nodePtr)
	{

	struct MSGWORDS
		{
		int    m_value;
		LPSTR  m_words;
		};

	static LPSTR  s_defaultWords = "(unknown error code).";
	static MSGWORDS s_msgWords[] =
		{
			{  2, "File not found." },
			{  3, "Path not found." },
      {  4, "Too many open files."},
			{  5, "Access to the DLL has bee denied." },
			{  8, "Insufficient memory to load a DLL." },
			{ 10, "Encountered a DLL that specified an incorrect Windows version." },
			{ 11, "The DLL is not the correct format for win32." },
			{ 12, "You have incorrect access privledges for opening the DLL." },
			{ 14, "Out of memory" },
			{ 15, "Can not find specified drive." },
      { 18, "No more files can be opened."},
			{ 19, "The media is write protected." },
			{ 20, "Can not find the specified device." },
 			{ 21, "The device is not ready."},
      { 1113, "The DLL could not be initialized."}
 		};

	const int c_numMsgs = sizeof(s_msgWords) / sizeof(s_msgWords[0]);
	char       l_cap[120];
	char       l_buf[300];
	char       l_txt[400];
	LPSTR      l_words;				 
	int        l_i;
 	
	if((nodePtr->m_status & MTL_NODE_INVALIDADMN) &&
		(!(nodePtr->m_status & MTL_NODE_INVALIDMESSAGESHOWN)))
		{
		if((nodePtr->m_libHandle == NULL) && (nodePtr->m_errorValue != ERROR_SUCCESS))
	  	{
			l_words = s_defaultWords;
			for (l_i = 0 ; l_i < c_numMsgs ; l_i++)
				{
				if (((int) nodePtr->m_errorValue) == s_msgWords[l_i].m_value)
					{
					l_words = s_msgWords[l_i].m_words;
					break;
					}
				}
			
			wsprintf(l_cap, "Error Loading %s Object", (LPCSTR)nodePtr->m_typeName);

			lstrcpyn(l_buf,
								"An error occurred while attempting to load the DLL"
								" named '%s', which contains the %s's %s routine.\n\n"
								"Windows Error #%i:  ",
								sizeof(l_buf));

			strncat(l_buf, l_words, lstrlen(l_words));

			wsprintf(l_txt, l_buf,
							 (LPCSTR)nodePtr->m_dllName,
							 (LPCSTR)nodePtr->m_typeName,
							 MTL_PROCCATEGORY,
							 (LPSTR)nodePtr->m_errorValue);

			MessageBox(NULL, l_txt, l_cap, MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
	    
	    }
		else if(nodePtr->m_admnProc == NULL)
			{
			wsprintf(l_cap, "Error Loading %s Object", (LPCSTR)nodePtr->m_typeName);
			wsprintf(l_txt,
							 "Cannot find %s's %s routine "
							 "(searching for entry %s in the DLL %s).",
								(LPCSTR)nodePtr->m_typeName, MTL_PROCCATEGORY,
							 	(LPCSTR)nodePtr->m_procName, (LPCSTR)nodePtr->m_dllName);

			MessageBox(NULL,
								 l_txt, l_cap,
								 MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
				}

		nodePtr->m_status |= MTL_NODE_INVALIDMESSAGESHOWN;
	  }		
	} 

																			 
/////////////////////////////////////////////////////////////
// FUNCTION:			MASTERTYPELIST::GetNode
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
LPADMNNODE MASTERTYPELIST::GetNode(
	ATYPEID typeID)
	{
	return (LPADMNNODE)m_admnProcList->FindNode(typeID);
	} 


/////////////////////////////////////////////////////////////
// FUNCTION:			MASTERTYPELIST::GetAdmnProc
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
ADMNPROC MASTERTYPELIST::GetAdmnProc(
	ATYPEID typeID)
	{
	LPADMNNODE l_node = GetNode(typeID);
	ADMNPROC l_rval = NULL;

	if(l_node)
		{
		if(!(l_node->m_status & MTL_NODE_LOADATTEMPTED))
			LoadAdmnProc(l_node);
		
		if(l_node->m_status & MTL_NODE_INVALIDADMN)
			ShowErrorMessage(l_node);
		else
			l_rval = l_node->m_admnProc;
		}
		
	return l_rval;			
	} 

																			 																			 
/////////////////////////////////////////////////////////////
// FUNCTION:			MASTERTYPELIST::LoadNextDLL
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:			TRUE if there are still DLL's to load else
//							returns FALSE.
/////////////////////////////////////////////////////////////
BOOL MASTERTYPELIST::LoadNextDLL()
	{
	LPADMNNODE l_nodePtr;

	if(!(m_status & MTL_MASTER_ALLDONE))
		{

		l_nodePtr = GetNextNode(m_nodeKey);
	
		m_nodeKey = l_nodePtr;

		if(m_nodeKey == NULL)
			m_status |= MTL_MASTER_ALLDONE;
		else
			LoadAdmnProc(l_nodePtr);
		}

	return (m_nodeKey != NULL)?TRUE:FALSE;
	} 

																			 
/////////////////////////////////////////////////////////////
// FUNCTION:			MASTERTYPELIST::AddTypeNode
//						 
// PARAMETERS:
//
// DESCRIPTION:
//
// RETURNS:
/////////////////////////////////////////////////////////////
void MASTERTYPELIST::AddTypeNode(
	ATYPEID typeID,
	LPCSTR typeName,
	LPCSTR dllName,
	LPCSTR procName)
	{
	LPADMNNODE l_nodePtr = GetNode(typeID);

	if(!l_nodePtr)
		{
		l_nodePtr = new ADMNNODE(typeID, typeName, dllName, procName);

		I_ASSERT(l_nodePtr);

		m_admnProcList->Add(l_nodePtr);
		}
	else
		{
		ADBG_PRINT_W("Trying to add duplicate type in MASTERTYPELIST::AddTypeNode.");
		}	
	} 

																			 
