#include <stdlib.h>
#include "almsys.h"
#include <windowsx.h>

#include "progsys.h"
#include "objshare.h"
#include "objinst.h"
#include "mdiclass.h"
#include "helpids.h"
#include "resconst.h"
#include "subject.h"
#include "project.h"
#include "sjtwin.h"
#include "objgroup.h"
#include "uidlist.h"
#include "intdebug.h"
#include "helpcls.h"
#include "oiputils.h"
#include "a_almutl.h"

#ifdef DEMO
#include "demo.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  OBJECTSHARE
//
// DESCRIPTION.:  constructor
/////////////////////////////////////////////////////////////////////////////
OBJECTSHARE::OBJECTSHARE(
	SUBJECTWINDOW * pSubjWnd)
	{
	this->pSubjWnd = pSubjWnd;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  ~OBJECTSHARE
//
// DESCRIPTION.:  destructor
/////////////////////////////////////////////////////////////////////////////
OBJECTSHARE::~OBJECTSHARE(
	void)
	{
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  SetupObjectMenu
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
void OBJECTSHARE::SetupObjectMenu()
	{
	HMENU 		    hMenu;
	int				    i,j;
	LPOBJINST	    pObjInst;
	RUNTIMEUID    objectBlock;
	RUNTIMEUID    subjectBlock;
	SUBJECT *  pSubjPtr;
	SUBJECT *  tempSubjPtr;
	BOOL 			    objectsSelected = FALSE;
	BOOL			    makeAlias = FALSE;
	BOOL			    makePersist = FALSE;
	BOOL 					makeNonPersist = FALSE;

	hMenu = GetMenu(PROGSYSTEM::hWndFrame);
	EnableMenuItem(hMenu, IDM_OBJERRORS, MF_GRAYED|MF_BYCOMMAND);
	EnableMenuItem(hMenu, IDM_SHARE, MF_GRAYED|MF_BYCOMMAND);
	EnableMenuItem(hMenu, IDM_UNSHARE, MF_GRAYED|MF_BYCOMMAND);
	EnableMenuItem(hMenu, IDM_ALIAS, MF_GRAYED|MF_BYCOMMAND);
	EnableMenuItem(hMenu, IDM_MAKEPERSIST, MF_GRAYED|MF_BYCOMMAND);
  EnableMenuItem(hMenu, IDM_MAKENONPERSIST, MF_GRAYED|MF_BYCOMMAND);

	i=0;
	pSubjPtr = GetSubjWndPtr()->GetSubjectPtr();
	objectBlock = pSubjPtr->GetObjectList();
	while(0 != (pObjInst = (LPOBJINST)GetObject(objectBlock, i)))
		{
		//If an object is selected then see if its a shared object and if so,
		//set the unshare menu option.
		//If the selected object is not an alias and not shared then enable the
		//share menu option.
		if(pObjInst->TestSelectFlag())
			{
			objectsSelected = TRUE;
			if(pObjInst->TestShareFlag())
				EnableMenuItem(hMenu, IDM_UNSHARE, MF_ENABLED|MF_BYCOMMAND);
			else if(!pObjInst->TestAliasFlag())
				EnableMenuItem(hMenu, IDM_SHARE, MF_ENABLED|MF_BYCOMMAND);

			if(!pObjInst->TestAliasFlag())
      	{
				if(pObjInst->TestPersistentsFlag())
					makeNonPersist = TRUE;
				else
					makePersist = TRUE;
        }

			}
		if(!makeAlias && pObjInst->TestShareFlag())
			makeAlias = TRUE;
		}

	//if the Make Alias menu option still shouldn't be enabled, then go
	//through the rest of the subjects to see if they have any shared objects.
	i=0;
	subjectBlock = pSubjPtr->GetParentProject()->GetSubjectBlock();
	while(!makeAlias &&
		0!=(tempSubjPtr = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		//Make sure its not the subject we checked above
		if(tempSubjPtr != pSubjPtr)
			{
			objectBlock = tempSubjPtr->GetObjectList();
			j=0;
			//Go through all the objects to see if any are shared
			while(0 != (pObjInst = (LPOBJINST)GetObject(objectBlock, j)))
				{
				if(pObjInst->TestShareFlag())
					{
					makeAlias = TRUE;
					break;
					}
				}
			}
		}

	if(makeAlias)
		{
		//we only want to execute the conditional if its the demo version
		//if the conditional is true, then enable the alias menu item
		//If its not the demo, then enable the alias menu item
#ifdef DEMO
		if((GetSubjWndPtr()->GetSubjectPtr()->GetObjectCount() < DEMO_MAXOBJECTS)
			&&
			(GetSubjWndPtr()->GetSubjectPtr()->GetParentProject()->GetObjectCount()
			< DEMO_MAXPROJECTOBJECTS))
#endif
		EnableMenuItem(hMenu, IDM_ALIAS, MF_ENABLED|MF_BYCOMMAND);
		 }

	if(objectsSelected)
		{
		EnableMenuItem(hMenu, IDM_OBJERRORS, MF_ENABLED|MF_BYCOMMAND);
		if(makePersist)
			EnableMenuItem(hMenu, IDM_MAKEPERSIST, MF_ENABLED|MF_BYCOMMAND);

		if(makeNonPersist)
			EnableMenuItem(hMenu, IDM_MAKENONPERSIST, MF_ENABLED|MF_BYCOMMAND);
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MakeShareable
//
// DESCRIPTION.:  makes selected objects shareable
/////////////////////////////////////////////////////////////////////////////
void OBJECTSHARE::MakeShareable(
	BOOL isShareable)
	{
	int				i=0;
	LPOBJGROUP groupPtr;
	RUNTIMEUID groupBlock = GetSubjWndPtr()->GetSubjectPtr()->GetObjGroupList();
	BOOL result=FALSE;

	while (0 != (groupPtr = (LPOBJGROUP)GetObject(groupBlock, i)))
		if(groupPtr->GetSelObjCount() > 0)
			{
			if(isShareable)
      	{
				if(groupPtr->ShareObjects())
					result = TRUE;
        }
			else
      	{
				if(groupPtr->UnShareObjects())
					result = TRUE;
				}
			}
	if(result)
		{
		PROGSYSTEM::GetCurrentProject()->LinkAllAliases(FALSE);
		PROGSYSTEM::GetCurrentProject()->MakeDirty();
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  AliasDialog
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
void OBJECTSHARE::AliasDialog()
	{
	DialogBox(PROGSYSTEM::hInst, "Alias", GetSubjWndPtr()->GetWindowHandle(),
	 (DLGPROC)OBJECTSHARE::AliasDlgProc);

	//there will always be an active subject here since that is where the
	//aliases were added.
	PROGSYSTEM::GetCurrentProject()->GetActiveSubject()->LinkAllAliasObjects(FALSE);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  AliasDlgProc
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK OBJECTSHARE::AliasDlgProc(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM ) // lParam)
	{
	OBJECTSHARE *pObjShare =
	 ((SUBJECTWINDOW *)PROGSYSTEM::GetCurrentProject()->
	 GetActiveSubject()->GetWindowPtr())->GetObjSharePtr();

	switch (message)
		{
		case WM_INITDIALOG:
			CenterDialog(hDlg, PROGSYSTEM::hWndFrame, FALSE);
			pObjShare->AddSubjects(hDlg);
			return TRUE;

		case WM_COMMAND:
			{
			WORD		wID			= GET_WM_COMMAND_ID(wParam, lParam);
			WORD		wNotify	= GET_WM_COMMAND_CMD(wParam, lParam);

			switch(wID)
				{
				case IDOK:
					pObjShare->MakeAlias(hDlg);
					return TRUE;

				case IDCANCEL:
					EndDialog(hDlg, 0);
					return TRUE;

				case ID_HELP:
					WinHelp(hDlg, MDI::pHelpPtr->GetHelpFileName(), HELP_CONTEXT, HELPID_DLG_ALIAS);
					return TRUE;

				case ID_SLIST:
					if(wNotify == LBN_SELCHANGE)
						{
						pObjShare->AddShareObjects(hDlg);
						//set the selection of the first item in the list box
						SendDlgItemMessage(hDlg, ID_OLIST, LB_SETSEL, TRUE, 0L);
						}
					return 0;

				case ID_OLIST:
					if(wNotify == LBN_DBLCLK)
						SendMessage(hDlg, WM_COMMAND, GET_WM_COMMAND_MPS(IDOK, 0, 0));
#ifdef DEMO
					//If making Aliases will excede the maximum allowed objects for the
					//demo, then disable the make alias button.
					if (wNotify == LBN_SELCHANGE)
						{
						if((PROGSYSTEM::GetCurrentProject()->GetActiveSubject()->
							GetObjectCount() +
							(int)SendDlgItemMessage(hDlg, ID_OLIST, LB_GETSELCOUNT, 0L, 0L)
							> DEMO_MAXOBJECTS)
							||
							(PROGSYSTEM::GetCurrentProject()->GetObjectCount() +
							(int)SendDlgItemMessage(hDlg, ID_OLIST, LB_GETSELCOUNT, 0L, 0L)
							> DEMO_MAXPROJECTOBJECTS))
							{
							EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
							}
						else
							{
							EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
							}
						}

#endif
					return 0;

				}
			break;
			}
		}
	return FALSE;
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  AddSubjects
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
void OBJECTSHARE::AddSubjects(
	HWND hDlg)
	{
  LPOBJINST    pObjInst;
	RUNTIMEUID   subjectBlock;
	RUNTIMEUID   objectBlock;
	SUBJECT * pSubjPtr;
	int		 	     i, j, index;
	HWND				hList		= GetDlgItem(hDlg, ID_SLIST);

  i=0;
  subjectBlock = PROGSYSTEM::GetCurrentProject()->GetSubjectBlock();
	while (0 != (pSubjPtr = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		j = 0;
		objectBlock = pSubjPtr->GetObjectList();
  	while(0 != (pObjInst = (LPOBJINST)GetObject(objectBlock, j)))
      {
			if(pObjInst->TestShareFlag())
        {
    		index = ListBox_AddString(hList, pSubjPtr->GetName());
		    ListBox_SetItemData(hList, index, GetUID(pSubjPtr).id);
				break;
				}
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  AddShareObjects
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
void OBJECTSHARE::AddShareObjects(
	HWND hDlg)
	{
	int		        i, j, index;
	long			    subject;
	LPOBJINST     pObjInst;
	RUNTIMEUID  	subjectBlock, groupBlock;
	SUBJECT	*  pSubjPtr;
	LPOBJGROUP 		groupPtr;
	OBJECTTRAVERSEKEY key;
	BOOL			    enableMakeButton = FALSE;
	HWND					hList		= GetDlgItem(hDlg, ID_SLIST);
	HWND					hOList	= GetDlgItem(hDlg, ID_OLIST);

	index = ListBox_GetCurSel(hList);
	subject = ListBox_GetItemData(hList, index);
	ListBox_ResetContent(hList);
	subjectBlock = PROGSYSTEM::GetCurrentProject()->GetSubjectBlock();
	i = 0;
	while(0 != (pSubjPtr = (SUBJECT *)GetObject(subjectBlock, i)))
		{
		if(GetUID(pSubjPtr).id == subject)
			{
      //look through all groups
			groupBlock = pSubjPtr->GetObjGroupList();
			j=0;
			while(0 != (groupPtr = (LPOBJGROUP)GetObject(groupBlock, j)))
				{
        //travers all objects in the group looking for shared objects
				groupPtr->InitializeTraverseKey(&key);
				while(NULL != (pObjInst = groupPtr->GetNextObject(&key)))
					{
					if(pObjInst->TestShareFlag())
						{
            enableMakeButton = TRUE;

						index = ListBox_AddString(hOList, (LPSTR)pObjInst->GetName());
						ListBox_SetItemData(hOList, index, GetUID(pObjInst).id);
						}
          }
        }
			break;
			}
		}

#ifdef DEMO
	//Disable the make alias button if there  are already the max number of
	//objects in the subject for the demo version.
	if(PROGSYSTEM::GetCurrentProject()->GetActiveSubject()->GetObjectCount()
		>= DEMO_MAXOBJECTS 
		||
		PROGSYSTEM::GetCurrentProject()->GetObjectCount()
		>= DEMO_MAXPROJECTOBJECTS)
		{
		enableMakeButton = FALSE;
		}
#endif
	EnableWindow(GetDlgItem(hDlg, IDOK), enableMakeButton);
	}

/////////////////////////////////////////////////////////////////////////////
// FUNCTION....:  MakeAlias
//
// DESCRIPTION.:
/////////////////////////////////////////////////////////////////////////////
void OBJECTSHARE::MakeAlias(
	HWND hDlg)
	{
	int		 		i, index, selCount, *itemArray;
  long      subject;
	LPOBJINST pObjInst;
	RUNTIMEUID objectBlock;
	SUBJECT   *pSubjPtr;
  LPLIST		aliasList;
	LPUIDLIST member;
  long 			itemData;
	HWND			hOList			= GetDlgItem(hDlg, ID_OLIST);
	HWND			hSList			= GetDlgItem(hDlg, ID_SLIST);

	if(!(selCount = ListBox_GetSelCount(hOList)))
		return;

#ifdef DEMO
	if((selCount + pSubjWnd->GetSubjectPtr()->GetObjectCount()>DEMO_MAXOBJECTS)
		 	||
			(selCount + pSubjWnd->GetSubjectPtr()->GetParentProject()->
			GetObjectCount() >= DEMO_MAXPROJECTOBJECTS))
		{
		demo_ObjectMessage();
		return;
		}
#endif

	SetDlgItemText(hDlg, IDCANCEL, "Close");
	itemArray = new int[selCount];
	ListBox_GetSelItems(hOList, selCount, itemArray);
	index = ListBox_GetCurSel(hSList);
	subject = ListBox_GetItemData(hSList, index);
	pSubjPtr = (LPSUBJECT)GetObject((SERUID)subject);
	objectBlock = pSubjPtr->GetObjectList();

	aliasList = new LIST();

	for(i=0;i<selCount;i++)
		{
		//Get the shared object ptr
		itemData = ListBox_GetItemData(hOList, itemArray[i]);
		I_ASSERT(itemData != LB_ERR);
		if(itemData != LB_ERR)
			{
			pObjInst = (LPOBJINST)GetObject((SERUID)itemData);
			member = new UIDLIST(GetUID(pObjInst));
			aliasList->Add(member);
      }
		}

	pSubjWnd->GetSubjectPtr()->AddAliasObjects(aliasList);
	delete itemArray;
	delete aliasList;
#ifdef DEMO
	//See if we need to disable the make alias button because too many shared
	//objects are selected.
	//selCount should still be the number of selected subjects
	if(selCount + pSubjWnd->GetSubjectPtr()->GetObjectCount()>DEMO_MAXOBJECTS
		||
		pSubjWnd->GetSubjectPtr()->GetParentProject()->GetObjectCount() >
		DEMO_MAXPROJECTOBJECTS)
		EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
#endif      */
	}
