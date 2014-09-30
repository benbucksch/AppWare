#ifndef __CLIPHEAD_H
#define __CLIPHEAD_H

#include "classdef.h"
#include "serrec.h"
#include "fileobj.h"

#define COMPLETESUBJECT 0x00000001
#define NO_ENTRIES      0x00000002

#define PASTE_OFFSET		4

class CLIPHEAD:public SERIUSRECORD
	{
	private:

		RUNTIMEUID		subjectBlock;
		POINT	  			pasteOffset;
		long 					status;
		FILEIDSTAMP   cutContextID;

	public:

		CLIPHEAD();

		~CLIPHEAD();

		BOOL TestCompleteSubjectFlag()
			{
			return (status & COMPLETESUBJECT)?TRUE:FALSE;
			}

		void SetCompleteSubjectFlag()
			{
			status|=COMPLETESUBJECT;
			}

		void ClearCompleteSubjectFlag()
			{
			status&= ~COMPLETESUBJECT;
			}

		BOOL TestEntriesFlag()
			{
			return (status & NO_ENTRIES)?FALSE:TRUE;
			}

		RUNTIMEUID GetSubjectBlock()
			{
			return subjectBlock;
			}

		void ClearPasteOffset()
			{
			pasteOffset.x = 0;
			pasteOffset.y = 0;
			}

		POINT GetPasteOffset()
			{
			return pasteOffset;
			}

		void IncPasteOffset()
			{
			pasteOffset.x+=PASTE_OFFSET;
			pasteOffset.y+=PASTE_OFFSET;
			}

		void SetCutContextID(
			FILEIDSTAMP newContextID)
			{
			cutContextID = newContextID;
			}

		FILEIDSTAMP GetCutContextID()
			{
			return cutContextID;
			}
	};

#endif
