// kba.cpp

#define STRICT
#define ASTRICT
#include <a_alm.h>
#include <intdebug.h>

#include "keyboard.h"

#define CORRECTSIGNALVERSION 2

extern "C" {

void ImportObject(OBJECTID);
void ExportObject(OBJECTID);


void	ImportObject(
	OBJECTID	objectID
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
	OBJECTID	objectID
	)
	{
	KBRECORD*		kbRec		= (KBRECORD *)AObjLockData(objectID, DATA0);

	AUpsfExportField("VERSION", 0L, AUPSF_NATV_INT, AUPSF_UNIV_INTEGER, &kbRec->version, sizeof(int));
	AUpsfExportField("ACTIVE", 0L, AUPSF_NATV_LONG, AUPSF_UNIV_INTEGER, &kbRec->active, sizeof(long));

	AObjUnlockData(objectID, DATA0);
	}


// The following function will translate the signals from version 1.0 of keyboard
// to map to the MAC signals. The signals in the 2 platforms for Dec'92
// release (version 1.0) were different.

void	TranslateSignals(
	OBJECTID	objectID
	)
	{
	// Fix discrepancies between MAC and MS WINDOWS keyboard signals
	OKludgeReportChangedSignals(objectID,   0x0D, 0);  // "Enter Key"
  OKludgeReportChangedSignals(objectID,   0x20, 2);  // "Spacebar"
	OKludgeReportChangedSignals(objectID,   0x31, 3);  // "Upper 1 Key"
	OKludgeReportChangedSignals(objectID,   0x32, 4);  // "Upper 2 Key"
	OKludgeReportChangedSignals(objectID,   0x33, 5);  // "Upper 3 Key"
	OKludgeReportChangedSignals(objectID,   0x34, 6);  // "Upper 4 Key"
	OKludgeReportChangedSignals(objectID,   0x35, 7);  // "Upper 5 Key"
	OKludgeReportChangedSignals(objectID,   0x36, 8);  // "Upper 6 Key"
	OKludgeReportChangedSignals(objectID,   0x37, 9);  // "Upper 7 Key"
      
	OKludgeReportChangedSignals(objectID,   0x38, 10); // "Upper 8 Key"
      
	OKludgeReportChangedSignals(objectID,   0x39, 11); // "Upper 9 Key"
	OKludgeReportChangedSignals(objectID,   0x30, 12); // "Upper 0 Key"

	OKludgeReportChangedSignals(objectID,   0x08, 13); // "Backspace Key"       // m: "Delete Key"
	OKludgeReportChangedSignals(objectID,   0x1B, 14); // "Escape Key"

	OKludgeReportChangedSignals(objectID,   0x70, 15); // "F1 Key"
	OKludgeReportChangedSignals(objectID,   0x71, 16); // "F2 Key"
	OKludgeReportChangedSignals(objectID,   0x72, 17); // "F3 Key"
	OKludgeReportChangedSignals(objectID,   0x73, 18); // "F4 Key"
	OKludgeReportChangedSignals(objectID,   0x74, 19); // "F5 Key"
	OKludgeReportChangedSignals(objectID,   0x75, 20); // "F6 Key"
	OKludgeReportChangedSignals(objectID,   0x76, 21); // "F7 Key"
	OKludgeReportChangedSignals(objectID,   0x77, 22); // "F8 Key"
	OKludgeReportChangedSignals(objectID,   0x78, 23); // "F9 Key"
	OKludgeReportChangedSignals(objectID,   0x79, 24); // "F10 Key"
	OKludgeReportChangedSignals(objectID,   0x7A, 25); // "F11 Key"
	OKludgeReportChangedSignals(objectID,   0x7B, 26); // "F12 Key"
	OKludgeReportChangedSignals(objectID,   0x7C, 27); // "F13 Key"
	OKludgeReportChangedSignals(objectID,   0x7D, 28); // "F14 Key"
	OKludgeReportChangedSignals(objectID,   0x7E,  29); // "F15 Key"
	/*
	OKludgeReportChangedSignals(objectID,   0x7F, oldSignalID); // "F16 Key"             // w+
	OKludgeReportChangedSignals(objectID,   0x80, oldSignalID); // "F17 Key"             // w+
	OKludgeReportChangedSignals(objectID,   0x81, oldSignalID); // "F18 Key"             // w+
	OKludgeReportChangedSignals(objectID,   0x82, oldSignalID); // "F19 Key"             // w+
	OKludgeReportChangedSignals(objectID,   0x83, oldSignalID); // "F20 Key"             // w+
	OKludgeReportChangedSignals(objectID,   0x84, oldSignalID); // "F21 Key"             // w+
	OKludgeReportChangedSignals(objectID,   0x85, oldSignalID); // "F22 Key"             // w+
	OKludgeReportChangedSignals(objectID,   0x86, oldSignalID); // "F23 Key"             // w+
	OKludgeReportChangedSignals(objectID,   0x87, oldSignalID); // "F24 Key"             // w+
  */
	OKludgeReportChangedSignals(objectID,   0x41, 31); // "A Key"
	OKludgeReportChangedSignals(objectID,   0x42, 32); // "B Key"
	OKludgeReportChangedSignals(objectID,   0x43, 33); // "C Key"
	OKludgeReportChangedSignals(objectID,   0x44, 34); // "D Key"
	OKludgeReportChangedSignals(objectID,   0x45, 35); // "E Key"
	OKludgeReportChangedSignals(objectID,   0x46, 36); // "F Key"
	OKludgeReportChangedSignals(objectID,   0x47, 37); // "G Key"
	OKludgeReportChangedSignals(objectID,   0x48, 38); // "H Key"
	OKludgeReportChangedSignals(objectID,   0x49, 39); // "I Key"
	OKludgeReportChangedSignals(objectID,   0x4A, 40); // "J Key"
	OKludgeReportChangedSignals(objectID,   0x4B, 41); // "K Key"
	OKludgeReportChangedSignals(objectID,   0x4C, 42); // "L Key"
	OKludgeReportChangedSignals(objectID,   0x4D, 43); // "M Key"
	OKludgeReportChangedSignals(objectID,   0x4E, 44); // "N Key"
	OKludgeReportChangedSignals(objectID,   0x4F, 45); // "O Key"
	OKludgeReportChangedSignals(objectID,   0x50, 46); // "P Key"
	OKludgeReportChangedSignals(objectID,   0x51, 47); // "Q Key"
	OKludgeReportChangedSignals(objectID,   0x52, 48); // "R Key"
	OKludgeReportChangedSignals(objectID,   0x53, 49); // "S Key"
	OKludgeReportChangedSignals(objectID,   0x54, 50); // "T Key"
	OKludgeReportChangedSignals(objectID,   0x55, 51); // "U Key"
	OKludgeReportChangedSignals(objectID,   0x56, 52); // "V Key"
	OKludgeReportChangedSignals(objectID,   0x57, 53); // "W Key"
	OKludgeReportChangedSignals(objectID,   0x58, 54); // "X Key"
	OKludgeReportChangedSignals(objectID,   0x59, 55); // "Y Key"
	OKludgeReportChangedSignals(objectID,   0x5A, 56); // "Z Key"

	OKludgeReportChangedSignals(objectID,   0x2F, 57); // "Help Key"            // v?

  // *** NOTE: the order of these directional key signals
	// ***       is different than on serius/m.  For example, ); //
  // ***       the "Forward Delete Key" signal in serius/m
  // ***       appears in the midst of directional key signals.

	OKludgeReportChangedSignals(objectID,   0x24, 58); // "Home Key"
	OKludgeReportChangedSignals(objectID,   0x21, 59); // "Page Up Key"
	OKludgeReportChangedSignals(objectID,   0x2E, 60); // "Delete Key"          // m: "Forward Delete Key" in Serius/M
	OKludgeReportChangedSignals(objectID,   0x23, 61); // "End Key"

	OKludgeReportChangedSignals(objectID,   0x22, 62); // "Page Down Key"

	OKludgeReportChangedSignals(objectID,   0x25, 63); // "Left Arrow Key"
	OKludgeReportChangedSignals(objectID,   0x27, 64); // "Right Arrow Key"
	OKludgeReportChangedSignals(objectID,   0x26, 65); // "Up Arrow Key"
	OKludgeReportChangedSignals(objectID,   0x28, 66); // "Down Arrow Key"

			
	// OKludgeReportChangedSignals(objectID,   0x2D, oldSignalID); // "Insert Key"          // w+

	OKludgeReportChangedSignals(objectID,   0x0C, 67); // "Clear Key"           // v?

  // -------------------------- // m+ "Numeric = Key"

	OKludgeReportChangedSignals(objectID,   0x6F, 69); // "Numeric / Key"
	OKludgeReportChangedSignals(objectID,   0x6A, 70); // "Numeric * Key"
	OKludgeReportChangedSignals(objectID,   0x6D, 71); // "Numeric - Key"
	OKludgeReportChangedSignals(objectID,   0x6B, 72); // "Numeric + Key"
	OKludgeReportChangedSignals(objectID,   0x6E, 73); // "Numeric . Key"

	OKludgeReportChangedSignals(objectID,   0x60, 74); // "Numeric 0 Key"
	OKludgeReportChangedSignals(objectID,   0x61, 75); // "Numeric 1 Key"
	OKludgeReportChangedSignals(objectID,   0x62, 76); // "Numeric 2 Key"
	OKludgeReportChangedSignals(objectID,   0x63, 77); // "Numeric 3 Key"
	OKludgeReportChangedSignals(objectID,   0x64, 78); // "Numeric 4 Key"
	OKludgeReportChangedSignals(objectID,   0x65, 79); // "Numeric 5 Key"
	OKludgeReportChangedSignals(objectID,   0x66, 80); // "Numeric 6 Key"
	OKludgeReportChangedSignals(objectID,   0x67, 81); // "Numeric 7 Key"
	OKludgeReportChangedSignals(objectID,   0x68, 82); // "Numeric 8 Key"
	OKludgeReportChangedSignals(objectID,   0x69, 83); // "Numeric 9 Key"

	OKludgeReportChangedSignals(objectID,   0x10, 85); // "Shift Key"
	OKludgeReportChangedSignals(objectID,   0x11, 86); // "Control Key"
	OKludgeReportChangedSignals(objectID,   0x12, 87); // "Alt Key"             // m: "Option Key"

  // -------------------------- // m+ "Command Key"

	OKludgeReportChangedSignals(objectID,   0x09, 30); // "Tab Key"

  /**** Windows strays: ****/

	/*
	OKludgeReportChangedSignals(objectID,   0x01, oldSignalID); // "Left Mouse Button"   // w+
	OKludgeReportChangedSignals(objectID,   0x02, oldSignalID); // "Right Mouse Button"  // w+
	OKludgeReportChangedSignals(objectID,   0x03, oldSignalID); // "Cancel Key"          // w+, ); // v?
	OKludgeReportChangedSignals(objectID,   0x04, oldSignalID); // "Middle Mouse Button" // w+

	OKludgeReportChangedSignals(objectID,   0x13, oldSignalID); // "Pause Key"           // w+
	OKludgeReportChangedSignals(objectID,   0x14, oldSignalID); // "Caps Lock Key"       // w+

	OKludgeReportChangedSignals(objectID,   0x29, oldSignalID); // "Select Key"          // w+, oldSignalID); // v?

	OKludgeReportChangedSignals(objectID,   0x2B, oldSignalID); // "Execute Key"         // w+, ); // v?
	OKludgeReportChangedSignals(objectID,   0x2C, oldSignalID); // "Print Screen Key"    // w+

	OKludgeReportChangedSignals(objectID,   0x6C, oldSignalID); // "Numeric Separator Key"  // w+, ); // v?

	OKludgeReportChangedSignals(objectID,   0x90, oldSignalID); // "Num Lock Key"        // w+
	OKludgeReportChangedSignals(objectID,   0x91, oldSignalID); // "Scroll Lock Key"    }  // w+
	*/
  }



LONG ALMCALLBACK	admnProc(
	OBJECTID				theObject,
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
			OBJECTID			sourceObject;

		 	sourceObject = (OBJECTID)(system->message3);

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

			// for all keyboard objects of version 1
			// translate the signals to match the MAC version, MAY 27, 1993
			if (kbRec->version < CORRECTSIGNALVERSION)
				{
				TranslateSignals(theObject);
        }

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

