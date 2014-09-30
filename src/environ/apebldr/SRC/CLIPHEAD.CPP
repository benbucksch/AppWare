#include "almsys.h"

#include "cliphead.h"

#include "sereclib.h"
#include "clipsjt.h"
#include "rectypes.h"
#include "recblk.h"

CLIPHEAD::CLIPHEAD()
	{
	subjectBlock = GetUID(new RECORDBLOCK(sizeof(CLIPSJT), RT_CLIPSJT));
	status = 0;
	ClearPasteOffset();
	SetCutContextID(NULL);
	}

CLIPHEAD::~CLIPHEAD()
	{
	((LPRECORDBLOCK)GetObject(subjectBlock))->PurgeBlock();
	delete (LPRECORDBLOCK)GetObject(subjectBlock);
	}
