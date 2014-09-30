#if (!defined(__helpcls_h))
	#define __helpcls_h

#include "classdef.h"
#include "memclass.h"
#include "progsys.h"

class HELPCLS:public MEMORY
	{
	protected:
		BOOL		bHelpFlag;
		HCURSOR	hHelpCursor;

	public:
		HELPCLS(
			HINSTANCE hInst);

		~HELPCLS();

		void SetHelpFlag()
			{
			bHelpFlag = TRUE;
			}

		void ClearHelpFlag()
			{
			bHelpFlag = FALSE;
			}

		BOOL TestHelpFlag()
			{
			if (bHelpFlag)
				return TRUE;
			else
				return FALSE;
			}

		char *GetHelpFileName()
      {
      return PROGSYSTEM::GetVABHelpFile();
      }

		HCURSOR	GetHelpCursor()
    	{
	    return hHelpCursor;
	    }

		DWORD	GetMenuContextId(
			WORD wParam);

    DWORD GetCategoryContextId(
	    LPSTR catName);

	};

#endif
