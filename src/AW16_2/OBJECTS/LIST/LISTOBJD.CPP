////////////////////////////////////////////////////////
/////////  List Object Definition Routines    ////////
////////////////////////////////////////////////////////
#include <windows.h>
#include <mem.h>
#include <string.h>
#include <a_alm.h>
#include <op_wnd.h>
#include "attribut.h"
#include "list.h"

struct OBJECTSIGNALS {
	char far* name ;
	int ID ;
	} ;

OBJECTSIGNALS signals[] = {
	{"Double-click", DBLCLICKSIGNAL},
	{"Single-click", SINGLECLICKSIGNAL},
	{"Changed", CHANGEDSIGNAL},
	{"Enter Key", ENTERKEYSIGNAL},
	{"Return Key", RETURNKEYSIGNAL},
	{"Activated", ACTIVATEDSIGNAL},
	{"Deactivated", DEACTIVATEDSIGNAL},
	{"Selection Changed", SELCHANGEDSIGNAL},
	{"Edited", EDITEDSIGNAL} } ;

LONG ValidateIDs(OBJECTID oiList, pAObjMessage theSystem)
	{
	LPLIST lst = (LPLIST)AObjLockData(oiList, LIST_INFO) ;
	if (lst != NULL )
		{
		lst->m_oiList = oiList;
		AObjValidateID(&lst->m_oiText, theSystem);
		}
	AObjUnlockData(oiList, LIST_INFO) ;
	return A_OK ;
	}

LONG EvaluateObject(OBJECTID oiList)
	{
	AObjResizeData(oiList, LIST_INFO, sizeof(LIST) );
	AObjSetDataCount(oiList, NUM_LIST_DATAS);
	LPLIST lst = (LPLIST) AObjLockData(oiList, LIST_INFO) ;
	if (lst != NULL )
		{
		lst->m_oiList = oiList;
		if (!AObjCheckType(lst->m_oiText, 0))
			lst->m_oiText = 0;
		}
	AObjUnlockData(oiList, LIST_INFO) ;
	return A_OK ;
	}

LONG BuildSignals(OBJECTID oiList, pAObjMessage theSystem)
	{
	BOOL notEditable = TRUE ;
	LPLIST lst = (LPLIST) AObjLockData(oiList, LIST_INFO);
	if (lst)
		{
		notEditable = !(lst->m_style & LF_EDITABLE_COMBOS) ;
		}
	AObjUnlockData(oiList, LIST_INFO);

	MEMBLOCKID signalBlock = (MEMBLOCKID)theSystem->message2 ;
	int nSignals = sizeof(signals)/sizeof(signals[0]) - notEditable ;
	AMemSetSize(signalBlock, nSignals*sizeof(AObjSignalRecord)) ;
	AObjSignalRecord far* lpSignal = (AObjSignalRecord far*)AMemLock(signalBlock) ;
	if (lpSignal == NULL)
		{
		AMemUnlock(signalBlock) ;
		return A_ERROR ;
		}
	for (int i = 0; i < nSignals; i++)
		{
		lpSignal[i].theID = signals[i].ID ;
		lstrcpy(lpSignal[i].theName, signals[i].name) ;
		}
	AMemUnlock(signalBlock) ;
	return A_OK ;
	}
