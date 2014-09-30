/*************************************************************************
**	File Name:
**		TDEVUSER.CPP	Copyright 1994 Novell Inc.
**
**	Description:
**		The AppWare TDev object's user support functions.
**
**	Author:
**		John Calcote -- Novell, Inc
**		x7517 -- Provo
*/

//	Include files

#include <windows.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include <a_alm.h>
#include <a_almutl.h>
#include <o_text.h>
#include <o_number.h>
#include "acs.h"
#include "csta.h"
#include "alm_csta.h"
#include "o_tdev.h"

/*************************************************************************
**	Notes:
**		The relationship between a TDevice Object and the TDEV Telephony
**		services model is as follows: A TDevice Object is equivalent to a
**		TDEV device on an open stream.	Attributes include Device ID
**		and Stream Handle.
**
**		The number of TDevice Objects in an AppWare application are
**		related to the number of allowable open streams.  This is
**		controlled by the TSAPI specification.
**
**	----------------------------------------------------------------------
**
**	Function:
**		OTDevSetStreamHandle
**
**	Description:
**		Sets the TDevice stream handle in the TDevice data block structure.
*/

BOOL ALMCALLBACK OTDevSetStreamHandle(OBJECTID oiTDevc, ACS acsHandle) {
	LPTDEVICE	lpTDevice;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (!lpTDevice)
		return FALSE;
	lpTDevice->acsHandle = acsHandle;
	AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
	return TRUE;
}

/*************************************************************************
**	Function:
**		OTDevGetStreamHandle
**
**	Description:
**		Retrieves the TDevice stream handle in the TDevice data block
**		structure.
*/

ACS ALMCALLBACK OTDevGetStreamHandle(OBJECTID oiTDevc) {
	LPTDEVICE	lpTDevice;
	ACS 		acsHandle = (ACS)0;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		acsHandle = lpTDevice->acsHandle;
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
	}
	return acsHandle;
}

/*************************************************************************
**	Function:
**		OTDevSetSendQSize
**
**	Description:
**		Sets the TDevice send Q size in the TDevice data block structure.
*/

BOOL ALMCALLBACK OTDevSetSendQSize(OBJECTID oiTDevc, LONG sendQSize) {
	LPTDEVICE	lpTDevice;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		ONmbrSetInteger(lpTDevice->oiSendQSize, sendQSize, TRUE);
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
		return TRUE;
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		OTDevGetSendQSize
**
**	Description:
**		Retrieves the TDevice send Q size in the TDevice data block
**		structure.
*/

LONG ALMCALLBACK OTDevGetSendQSize(OBJECTID oiTDevc) {
	LPTDEVICE	lpTDevice;
	LONG		sendQSize = 0L;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		sendQSize = ONmbrGetInteger(lpTDevice->oiSendQSize);
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
	}
	return sendQSize;
}

/*************************************************************************
**	Function:
**		OTDevSetRecvQSize
**
**	Description:
**		Sets the TDevice receive Q size in the TDevice data block
**		structure.
*/

BOOL ALMCALLBACK OTDevSetRecvQSize(OBJECTID oiTDevc, LONG recvQSize) {
	LPTDEVICE	lpTDevice;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		ONmbrSetInteger(lpTDevice->oiRecvQSize, recvQSize, TRUE);
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
		return TRUE;
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		OTDevGetRecvQSize
**
**	Description:
**		Retrieves the TDevice receive Q size in the TDevice data block
**		structure.
*/

LONG ALMCALLBACK OTDevGetRecvQSize(OBJECTID oiTDevc) {
	LPTDEVICE	lpTDevice;
	LONG		recvQSize = 0L;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		recvQSize = ONmbrGetInteger(lpTDevice->oiRecvQSize);
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
	}
	return recvQSize;
}

/*************************************************************************
**	Function:
**		OTDevSetServerID
**
**	Description:
**		Sets the TDevice Server Name in the TDevice data block structure.
*/

BOOL ALMCALLBACK OTDevSetServerID(OBJECTID oiTDevc, LPSTR szServerID) {
	LPTDEVICE	lpTDevice;
	BOOL		rc = FALSE;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		rc = (BOOL)OTextSetValueFromBuffer(lpTDevice->oiServerID, szServerID);
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
	}
	return rc;
}

/*************************************************************************
**	Function:
**		OTDevGetServerID
**
**	Description:
**		Retrieves the TDevice Server Name in the TDevice data block
**		structure.
*/

BOOL ALMCALLBACK OTDevGetServerID(OBJECTID oiTDevc, LPSTR szServerID, LONG bufLen) {
	LPTDEVICE	lpTDevice;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		OTextGetToBuffer(lpTDevice->oiServerID, szServerID, bufLen);
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
		return TRUE;
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		OTDevSetLoginID
**
**	Description:
**		Sets the TDevice Login Name in the TDevice data block structure.
*/

BOOL ALMCALLBACK OTDevSetLoginID(OBJECTID oiTDevc, LPSTR szLoginID) {
	LPTDEVICE	lpTDevice;
	BOOL		rc = FALSE;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		rc = (BOOL)OTextSetValueFromBuffer(lpTDevice->oiLoginID, szLoginID);
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
	}
	return rc;
}

/*************************************************************************
**	Function:
**		OTDevGetLoginID
**
**	Description:
**		Retrieves the TDevice Login Name in the TDevice data block
**		structure.
*/

BOOL ALMCALLBACK OTDevGetLoginID(OBJECTID oiTDevc, LPSTR szLoginID, LONG bufLen) {
	LPTDEVICE	lpTDevice;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		OTextGetToBuffer(lpTDevice->oiLoginID, szLoginID, bufLen);
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
		return TRUE;
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		OTDevSetMonitorDevice
**
**	Description:
**		Sets the TDevice Monitor Device PBX Extension in the TDevice
**		data block structure.
*/

BOOL ALMCALLBACK OTDevSetMonitorDevice(OBJECTID oiTDevc, LPSTR szExtension) {
	LPTDEVICE	lpTDevice;
	BOOL		rc = FALSE;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		rc = (BOOL)OTextSetValueFromBuffer(lpTDevice->oiMonDev, szExtension);
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
	}
	return rc;
}

/*************************************************************************
**	Function:
**		OTDevGetMonitorDevice
**
**	Description:
**		Retrieves the TDevice Monitor Device PBX Extension in the TDevice
**		data block structure.
*/

BOOL ALMCALLBACK OTDevGetMonitorDevice(OBJECTID oiTDevc, LPSTR szExtension, LONG bufLen) {
	LPTDEVICE	lpTDevice;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		OTextGetToBuffer(lpTDevice->oiMonDev, szExtension, bufLen);
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
		return TRUE;
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		OTDevSetPassword
**
**	Description:
**		Sets the TDevice Password in the TDevice data block structure.
*/

BOOL ALMCALLBACK OTDevSetPassword(OBJECTID oiTDevc, LPSTR szPassword) {
	LPTDEVICE	lpTDevice;
	BOOL		rc = FALSE;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		rc = (BOOL)OTextSetValueFromBuffer(lpTDevice->oiPassword, szPassword);
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
	}
	return rc;
}

/*************************************************************************
**	Function:
**		OTDevGetPassword
**
**	Description:
**		Retrieves the TDevice Password in the TDevice data block
**		structure.
*/

BOOL ALMCALLBACK OTDevGetPassword(OBJECTID oiTDevc, LPSTR szPassword, LONG bufLen) {
	LPTDEVICE	lpTDevice;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		OTextGetToBuffer(lpTDevice->oiPassword, szPassword, bufLen);
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
		return TRUE;
	}
	return FALSE;
}

/*************************************************************************
**	Function:
**		OTDevGetNumLines
**
**	Description:
**		Retrieves the number of lines in the TDevice data block structure.
*/

LONG ALMCALLBACK OTDevGetNumLines(OBJECTID oiTDevc) {
	LPTDEVICE	lpTDevice;
	LONG		lNumLines = 0L;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		lNumLines = lpTDevice->lNumLines;
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
	}
	return lNumLines;
}

/*************************************************************************
**	Function:
**		OTDevGetNextLine
**
**	Description:
**		Enumerates each TLine object in the TDevice object's line array.
*/

OBJECTID ALMCALLBACK OTDevGetNextLine(OBJECTID oiTDevc, LONG * lKey) {
	LPTDEVICE	lpTDevice;
	OBJECTID	oiReturn = (OBJECTID)0;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		if (lpTDevice->lNumLines)
			if (++(*lKey) < lpTDevice->lNumLines)
				oiReturn = lpTDevice->oiLines[(UINT)*lKey];
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
	}
	return oiReturn;
}

/*************************************************************************
**	Function:
**		OTDevFindLine
**
**	Description:
**		Searches for and return the Line array index of a call with a
**		matching callID in TLine object list of this TDevice object.
**		Returns -1 in case no matching TLine Object is found.
*/

LONG ALMCALLBACK OTDevFindLine(OBJECTID oiTDevc, LONG lCallID) {
	LPTDEVICE	lpTDevice;
	LPTLINE 	lpTLine;
	OBJECTID	oiThisLine;
	LONG		lResult = -1L;
	LONG		lThisCallID;
	UINT		iKey;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		if (lpTDevice->lNumLines) {
			for (iKey = 0; iKey < lpTDevice->lNumLines; iKey++) {
				oiThisLine = lpTDevice->oiLines[iKey];
				lpTLine = (LPTLINE)AObjLockData(oiThisLine, TLINE_MAINBLK);
				if (lpTLine) {
					lThisCallID = lpTLine->lCallID;
					AObjUnlockData(oiThisLine, TLINE_MAINBLK);
					if (lThisCallID == lCallID)
						break;
				}
			}
			if ((LONG)iKey < lpTDevice->lNumLines)
				lResult = (LONG)iKey;
		}
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
	}
	return lResult;
}

/*************************************************************************
**	Function:
**		OTDevGetActiveLine
**
**	Description:
**		Retrieves the Active Line on the TDevice.
*/

OBJECTID ALMCALLBACK OTDevGetActiveLine(OBJECTID oiTDevc) {
	LPTDEVICE	lpTDevice;
	LPTLINE		lpTLine;
	OBJECTID	oiLine = (OBJECTID)0;
	BOOL		fActiveLine = FALSE;
    int			i;

	lpTDevice = (LPTDEVICE)AObjLockData(oiTDevc, TDEVC_MAINBLK);
	if (lpTDevice) {
		for (i = 0; i < lpTDevice->lNumLines; i++) {
			oiLine = lpTDevice->oiLines[i];
			lpTLine = (LPTLINE)AObjLockData(oiLine, TLINE_MAINBLK);
			if (lpTLine) {
				fActiveLine = lpTLine->fActiveLine;
                AObjUnlockData(oiLine, TLINE_MAINBLK);
			}
			if (fActiveLine)
            	break;
		}
		AObjUnlockData(oiTDevc, TDEVC_MAINBLK);
		if (i >= lpTDevice->lNumLines)
			oiLine = (OBJECTID)0;        	
	}
	return oiLine;
}

