
//======================================================================
// app_admn.cpp
//
//   The Administration routine for the Serius Application object
//
// jon 14-Aug-92 [1.0] rework of Shaili's first cut
//======================================================================

#define STRICT
#define ASTRICT
#include <a_alm.h>
#include "app.h"

//////////////////////////////////////////////////////////
// The following function will translate the signals from version 1.0 of keyboard
// to map to the MAC signals. The signals in the 2 platforms for Dec'92
// release (version 1.0) were different.
//
//
//
//
//////////////////////////////////////////////////////////

void TranslateSignals(OBJECTID objectID)
	{
	OKludgeReportChangedSignals(objectID, olddoubleClickSignalID, doubleClickSignalID);
	OKludgeReportChangedSignals(objectID, oldkeyDownSignalID, keyDownSignalID );
	OKludgeReportChangedSignals(objectID, oldkeyUpSignalID, keyUpSignalID );
	OKludgeReportChangedSignals(objectID, olddiskInSignalID, diskInSignalID);
  }

////////////////////////////////////////////////////////////////////////
// describeSignal (static)
//
//   Fills in the specified index of the passed signal array with the
//   appropriate signal ID and description.
////////////////////////////////////////////////////////////////////////

              inline static
              void

describeSignal (pAObjSignalRecord theSigs,
                LONG           theIdx,
                LONG           theSignalID,
                LPSTR          theDescription)

  {
  (*(theSigs + (short) theIdx)).theID = theSignalID;
	lstrcpy((*(theSigs + (short) theIdx)).theName, theDescription);
  return;
  }


////////////////////////////////////////////////////////////////////////
// appAdmn
//
//   The Administration routine for the Application object.
////////////////////////////////////////////////////////////////////////

#pragma argsused

extern "C"
LONG ALMCALLBACK		appAdmn (
	OBJECTID					theObject,
	pAObjMessage      system)
  {
	LONG								lRet					= A_OK;
  MEMBLOCKID					sigsBlockID;
  pAObjSignalRecord		theSigs;

  switch (system->message1)
		{
		case AOBJ_CREATED:
			{
			DATA*			lpData;

			if (AObjAddData(theObject, 0, sizeof(DATA)) &&
					(lpData = (DATA *) AObjLockData(theObject, 0)) != NULL)
				{
				lpData->version = (LONG) DATAVersion;
				AObjUnlockData(theObject, 0);
				}
      }
			break;

		case AOBJ_READ:
			{
			DATA* lpData;

			lpData = (DATA*) AObjLockData(theObject, 0);

      // add a version number to data
			if (lpData)
				{
				if (lpData->version < CORRECTEDSIGNALVERSION)
					{
					TranslateSignals(theObject);
          }
        lpData->version = (LONG)DATAVersion;

				AObjUnlockData(theObject, 0);
				}
			else
				{
				if (AObjSetDataCount(theObject, 1) &&
						AObjResizeData(theObject, 0, sizeof(DATA)))
					{
					lpData = (DATA*) AObjLockData(theObject, 0);

					if (lpData != NULL)
						{
						lpData->version = DATAVersion;
						AObjUnlockData(theObject, 0);
						}
					}
        }
			}
			break;

		case AOBJ_CHECKOBJECT:
			break;

		case AOBJ_BUILDSIGNALS:
        sigsBlockID = (MEMBLOCKID) system->message2;
        AMemSetSize(sigsBlockID, numSignalIDs * sizeof(AObjSignalRecord));
        theSigs = (pAObjSignalRecord)AMemLock(sigsBlockID);

        describeSignal(theSigs, 0, startupSignalID,     "Startup");
        describeSignal(theSigs, 1, suspendedSignalID,   "Suspended");
        describeSignal(theSigs, 2, resumedSignalID,     "Resumed");
        describeSignal(theSigs, 3, mouseDownSignalID,   "Mouse Down");
        describeSignal(theSigs, 4, mouseUpSignalID,     "Mouse Up");
        describeSignal(theSigs, 5, doubleClickSignalID, "Mouse Double");
        describeSignal(theSigs, 6, keyDownSignalID,     "Key Down");
        describeSignal(theSigs, 7, keyUpSignalID,       "Key Up");
        describeSignal(theSigs, 8, diskInSignalID,      "Disk Inserted");

        break;

    default:
				lRet = A_NOTHANDLED;
        break;
    }

	return lRet;
  }
