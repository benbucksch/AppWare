// kba.cpp

#define STRICT
#define ASTRICT
#include <a_alm.h>
#include <intdebug.h>

#include "keyboard.h"
#include "windowsx.h"

#define CORRECTSIGNALVERSION 2

extern "C" {

void ImportObject(AOBJECTID);
void ExportObject(AOBJECTID);


void	ImportObject(
	AOBJECTID	objectID
	)
	{
  KBRECORD*		kbRec		= (KBRECORD*) AObjLockData(objectID, DATA0);
	long				active;
	int					ver;

	if (AUpsfImportField("VERSION", 0L, AUPSF_NATV_INT, &ver, sizeof(int)) != AUPSF_NOERROR)
		{
		AUpsfLogComment("Defaulting to version 1.");
    ver = 1;
    }

	if (AUpsfImportField("ACTIVE", 0L, AUPSF_NATV_LONG, &active, sizeof(long)) != AUPSF_NOERROR)
		{
		AUpsfLogComment("Defaulting to Active Keyboard.");

    active  = 1;
		}

	kbRec->active = active;
  kbRec->version = ver;

	AObjUnlockData(objectID, DATA0);
	}


void	ExportObject(
	AOBJECTID	objectID
	)
	{
	KBRECORD*		kbRec		= (KBRECORD *)AObjLockData(objectID, DATA0);

	AUpsfExportField("VERSION", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &kbRec->version, sizeof(int));
	AUpsfExportField("ACTIVE", 0L, AUPSF_NATV_LONG, AUPSF_UNIV_INTEGER, &kbRec->active, sizeof(long));

	AObjUnlockData(objectID, DATA0);
	}


LONG ALMCALLBACK	admnProc(
	AOBJECTID				theObject,
  pAObjMessage		system
	)
  {
	LONG				lRet						= A_OK;
  KBRECORD    kbRec;

  switch (system->message1)
    {
		case AOBJ_CREATED:
			kbRec.active = 1L;
      kbRec.version = DATAVersion;
			AObjAddData(theObject, &kbRec, sizeof(kbRec));
      break;
          
		case AOBJ_ASSIGNOBJECT:
			{
			KBRECORD*			kbRecSrc;
			AOBJECTID			sourceObject;

		 	sourceObject = (AOBJECTID)(system->message3);

			kbRecSrc = (KBRECORD*) AObjLockData(sourceObject, DATA0);
      kbRecSrc->version = DATAVersion;  // set if previously not set

      AObjSetData(
					theObject,
					DATA0,
					kbRecSrc,
          sizeof(KBRECORD));

      AEvtPostStandard(
					theObject,
					AEVENT_OBJECTCHANGED, 
					0,
          AEVT_ATTAIL|AEVT_KILLDUPLICATES);

			AObjUnlockData(sourceObject, DATA0);

			break;
			}

		case AOBJ_READ:
			{
			KBRECORD*		kbRec			= (KBRECORD*) AObjLockData(theObject, DATA0);


			kbRec->version = DATAVersion;  // set if previously not set

			if (kbRec->active != 0)
        kbRec->active = 1L;

			AObjUnlockData(theObject, DATA0);
			}
      break;
			
		case AOBJ_IMPORTOBJECT:
			ImportObject(theObject);
      break;

		case AOBJ_EXPORTOBJECT:
			ExportObject(theObject);
      break;

    case AOBJ_EDITOBJECT:
      if (EditKey(theObject))   // To manage description dlg
				lRet = A_MODIFIED;
      break;

    case AOBJ_CHECKOBJECT:
      break;

    case AOBJ_BUILDSIGNALS:
      doBuildSignals(system);
      break;

    default:
			lRet = A_NOTHANDLED;
      break;
    }

	return lRet;
  }


}// extern "C"

