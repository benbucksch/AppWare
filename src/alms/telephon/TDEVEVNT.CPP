/*************************************************************************
**	File Name:
**		TDEVEVNT.CPP		Copyright 1994 Novell Inc.
**
**	Description:
**		The Event Responders for the AppWare TDEV Object.
**		The TDev Object only responds to the system generated
**		startup event.
**
**	Author:
**		John Calcote -- Novell, Inc
**		x7517 -- Provo
*/

//	Include files

#include <windows.h>
#include <a_alm.h>
#include <a_almutl.h>
#include "acs.h"
#include "csta.h"
#include "alm_csta.h"

#ifdef __cplusplus
extern "C" {
#endif

void ALMCALLBACK TDevStartup(pAEvtInfo theEvent, pAObjMessage theSystem);

#ifdef __cplusplus
}
#endif

/*************************************************************************
**	Function:
**		TDevStartup 			(AppWare Bus)
**
**	Description:
**		Responds to the system's AEVENT_STARTUP event and simply reposts
**		its own private startup signal to itself so that a startup
**		function flow may be initiated.
*/

#pragma argsused
void ALMCALLBACK TDevStartup(pAEvtInfo theEvent, pAObjMessage theSystem) {
	OBJECTID		oiTDevc;
	long			key;

	key = AOBJ_GETFIRSTKEY;
	while ((oiTDevc = AObjGetNextObject(OTYPE_TDEV, &key)) != 0)
		AEvtPostSignalAtTail(oiTDevc, TDEV_EVT_STARTUP);
	return;
}

