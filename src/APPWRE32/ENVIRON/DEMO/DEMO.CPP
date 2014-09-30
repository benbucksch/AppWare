#include <almsys.h>
#include "demo.h"
#include "progsys.h"
#include "resconst.h"

void demo_SubjectMessage()
	{
	char message[256];
	char title[32];

	LoadString(PROGSYSTEM::hInst, STR_FRAMENAME, title, sizeof(title));

	wsprintf(message, "You can only have %u subjects per project in"
  	" the demo version of AppWare.", DEMO_MAXSUBJECTS);

  MessageBeep(0);
  MessageBox(NULL, message, title, MB_ICONINFORMATION|MB_TASKMODAL|MB_OK);
  }

void demo_ObjectMessage()
	{
	char message[256];
	char title[32];

	LoadString(PROGSYSTEM::hInst, STR_FRAMENAME, title, sizeof(title));

	wsprintf(message, "You can only have %u objects per subject or a total of "
  	"%u objects in a project in the demo version of AppWare.",
		DEMO_MAXOBJECTS, DEMO_MAXPROJECTOBJECTS);

  MessageBeep(0);
  MessageBox(NULL, message, title, MB_ICONINFORMATION|MB_TASKMODAL|MB_OK);
  }
